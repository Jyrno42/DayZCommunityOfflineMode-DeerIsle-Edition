#!/usr/bin/env python3
"""Build installable DayZ Community Offline Mode missions for DeerIsle.

A mission is assembled from three layers, later layers overwriting earlier ones:

  1. the DeerIsle central-economy files (``empty.deerisle``) fetched from the
     upstream repository pinned in ``variants/<variant>.json``;
  2. the Community Offline Mode scripts in ``com/``;
  3. optional per-variant files in ``overrides/<variant>/``.

Usage:
  python build.py                       # build every variant into dist/
  python build.py stable                # build one variant
  python build.py stable --install DIR  # also copy the mission into DIR/Missions
  python build.py --list                # show available variants

Only the Python standard library is used, so this runs the same on a
developer machine and in CI.
"""

from __future__ import annotations

import argparse
import hashlib
import io
import json
import re
import shutil
import struct
import sys
import tarfile
import urllib.request
import xml.etree.ElementTree as ET
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent
COM_DIR = ROOT / "com"
VARIANTS_DIR = ROOT / "variants"
OVERRIDES_DIR = ROOT / "overrides"
MODS_DIR = ROOT / "mods"
BUILD_DIR = ROOT / "build"
DIST_DIR = ROOT / "dist"

# The folder name the COM sources are written against. ``#include`` paths in
# the scripts are absolute (``$CurrentDir:missions\<name>\core\...``), so when a
# variant installs under a different name these paths are rewritten.
SOURCE_MISSION_DIR = "DayZCommunityOfflineMode.deerisle"

# Files shipped by upstream that COM replaces outright.
UPSTREAM_FILES_TO_DROP = {"init.c"}

LAUNCHER_NAME = "DayZCOfflineMDeerIsle.bat"
LAUNCHER_TEMPLATE = """@echo off

taskkill /F /IM DayZ_x64.exe /T

RD /s /q "storage_-1" > nul 2>&1

rem Per-mission profile (mod state, settings, logs); settings copied from Documents\\DayZ on first run.
if not exist "profiles" (
    mkdir "profiles"
    copy "%USERPROFILE%\\Documents\\DayZ\\*.DayZProfile" "profiles\\" > nul 2>&1
    copy "%USERPROFILE%\\Documents\\DayZ\\*.xml" "profiles\\" > nul 2>&1
    xcopy /E /I /Q /Y "%USERPROFILE%\\Documents\\DayZ\\Users" "profiles\\Users" > nul 2>&1
)

cd ../../

start DayZ_x64.exe -mission=.\\Missions\\{mission_dir} "-profiles=.\\Missions\\{mission_dir}\\profiles" -nosplash -noPause -noBenchmark -filePatching -doLogs -scriptDebug=true "-mod={mods}"
"""


def log(msg: str) -> None:
    print(msg, flush=True)


def load_variant(name: str) -> dict:
    path = VARIANTS_DIR / f"{name}.json"
    if not path.is_file():
        sys.exit(f"unknown variant '{name}' (no {path.relative_to(ROOT)})")
    with path.open(encoding="utf-8") as fh:
        variant = json.load(fh)
    variant.setdefault("name", name)
    return variant


def list_variants() -> list[str]:
    return sorted(p.stem for p in VARIANTS_DIR.glob("*.json"))


def fetch_upstream(upstream: dict) -> Path:
    """Download (once) and extract the pinned upstream tarball; return the mission dir."""
    repo, ref, sub = upstream["repo"], upstream["ref"], upstream["path"]
    cache = BUILD_DIR / "upstream" / repo.replace("/", "__") / ref
    marker = cache / ".complete"
    if not marker.exists():
        url = f"https://github.com/{repo}/archive/{ref}.tar.gz"
        log(f"  fetching {url}")
        with urllib.request.urlopen(url) as resp:
            data = resp.read()
        shutil.rmtree(cache, ignore_errors=True)
        cache.mkdir(parents=True)
        with tarfile.open(fileobj=io.BytesIO(data), mode="r:gz") as tar:
            # GitHub tarballs have a single top-level directory; strip it.
            members = tar.getmembers()
            prefix = members[0].name.split("/", 1)[0] + "/"
            for m in members:
                if not m.name.startswith(prefix):
                    continue
                m.name = m.name[len(prefix):]
                if m.name:
                    tar.extract(m, cache)
        marker.touch()
    else:
        log(f"  using cached {repo}@{ref[:10]}")
    src = cache / sub
    if not src.is_dir():
        sys.exit(f"upstream path '{sub}' not found in {repo}@{ref}")
    return src


def copy_tree(src: Path, dst: Path, skip: set[str] = frozenset()) -> int:
    count = 0
    for path in src.rglob("*"):
        if path.is_dir() or path.name == ".gitkeep":
            continue
        rel = path.relative_to(src)
        if str(rel) in skip:
            continue
        target = dst / rel
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(path, target)
        count += 1
    return count


def rewrite_mission_dir(mission: Path, new_name: str) -> int:
    """Point the absolute ``#include`` paths in the scripts at ``new_name``."""
    if new_name.lower() == SOURCE_MISSION_DIR.lower():
        return 0
    pattern = re.compile(re.escape(SOURCE_MISSION_DIR), re.IGNORECASE)
    changed = 0
    for path in mission.rglob("*.c"):
        text = path.read_text(encoding="utf-8")
        new_text, n = pattern.subn(new_name, text)
        if n:
            path.write_text(new_text, encoding="utf-8", newline="\n")
            changed += 1
    return changed


def generate_spawn_points(mission: Path) -> int:
    """Turn the <fresh> spawn bubbles of cfgplayerspawnpoints.xml into core/SpawnPoints.c.

    Offline mode never runs the server's spawn-point logic, so the COM scripts
    pick from this generated list instead (see COM_PickSpawnPosition in
    core/StaticFunctions.c).
    """
    xml_path = next(
        (p for p in mission.iterdir() if p.name.lower() == "cfgplayerspawnpoints.xml"), None
    )
    if xml_path is None:
        sys.exit(f"{mission.name}: cfgplayerspawnpoints.xml missing, cannot generate spawn points")
    root = ET.parse(xml_path).getroot()
    points = [
        (float(pos.get("x")), float(pos.get("z")))
        for pos in root.findall("./fresh/generator_posbubbles/pos")
    ]
    if not points:
        sys.exit(f"{xml_path}: no <fresh>/<generator_posbubbles>/<pos> entries found")

    lines = [
        "// GENERATED by build.py from cfgplayerspawnpoints.xml (<fresh> bubbles) - do not edit.",
        "static TVectorArray COM_GetSpawnPoints()",
        "{",
        "    return {",
    ]
    lines += [f'        "{x:.2f} 0 {z:.2f}",' for x, z in points]
    lines[-1] = lines[-1].rstrip(",")
    lines += ["    };", "}", ""]
    (mission / "core" / "SpawnPoints.c").write_text("\n".join(lines), encoding="utf-8", newline="\n")
    return len(points)


def apply_json_patches(mission: Path, variant: dict) -> int:
    """Deep-merge ``json_patches`` from the variant into the mission's JSON files (dicts merge, other values replace)."""
    patches = variant.get("json_patches", {})
    for name, patch in patches.items():
        path = next((p for p in mission.iterdir() if p.name.lower() == name.lower()), None)
        if path is None:
            sys.exit(f"json_patches: {name} not found in the mission")
        data = json.loads(path.read_text(encoding="utf-8-sig"))

        def merge(dst: dict, src: dict) -> None:
            for key, value in src.items():
                if isinstance(value, dict) and isinstance(dst.get(key), dict):
                    merge(dst[key], value)
                else:
                    dst[key] = value

        merge(data, patch)
        path.write_text(json.dumps(data, indent=4), encoding="utf-8", newline="\n")
        log(f"  patched {path.name}: {patch}")
    return len(patches)


def generate_spawn_sets(mission: Path, variant: dict) -> int:
    """Generate core/SpawnSets.c from ``spawn_sets``; items are ``Class[/Attachment...]``."""
    sets = variant.get("spawn_sets", [])
    lines = [
        "// GENERATED by build.py from variants/<variant>.json (spawn_sets) - do not edit.",
        "static TStringArray COM_GetSpawnSetNames()",
        "{",
        "    TStringArray names = new TStringArray;",
    ]
    lines += [f'    names.Insert("{s["name"]}");' for s in sets]
    lines += [
        "    return names;",
        "}",
        "",
        "static TStringArray COM_GetSpawnSetItems(int index)",
        "{",
        "    TStringArray items = new TStringArray;",
    ]
    for i, s in enumerate(sets):
        lines.append(f"    if (index == {i})")
        lines.append("    {")
        lines += [f'        items.Insert("{item}");' for item in s["items"]]
        lines.append("    }")
    lines += ["    return items;", "}", ""]
    (mission / "core" / "SpawnSets.c").write_text("\n".join(lines), encoding="utf-8", newline="\n")
    return len(sets)


def write_pbo(src_dir: Path, out: Path, prefix: str) -> int:
    """Pack a directory into an uncompressed PBO: Vers entry with prefix, header entries
        (name, method, original, reserved, timestamp, size), empty terminator, data, then 0x00 + SHA-1."""
    files = sorted(p for p in src_dir.rglob("*") if p.is_file())
    body = bytearray()

    def entry(name: bytes, method: int, original: int, reserved: int, timestamp: int, size: int) -> None:
        body.extend(name + b"\0")
        body.extend(struct.pack("<IIIII", method, original, reserved, timestamp, size))

    entry(b"", 0x56657273, 0, 0, 0, 0)  # 'Vers'
    body.extend(b"prefix\0" + prefix.encode("ascii") + b"\0\0")
    payloads = []
    for path in files:
        data = path.read_bytes()
        rel = path.relative_to(src_dir).as_posix().replace("/", "\\").encode("ascii")
        entry(rel, 0, 0, 0, int(path.stat().st_mtime), len(data))
        payloads.append(data)
    entry(b"", 0, 0, 0, 0, 0)
    for data in payloads:
        body.extend(data)
    body.append(0)
    body.extend(hashlib.sha1(bytes(body[:-1])).digest())

    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_bytes(bytes(body))
    return len(files)


def build_companion_mods(name: str, mission: Path) -> list[str]:
    """Pack mods/<variant>/*/ into <mission>/mod/addons/ and return the -mod= entries to add."""
    mods_dir = MODS_DIR / name
    if not mods_dir.is_dir():
        return []
    entries = []
    for mod_src in sorted(p for p in mods_dir.iterdir() if p.is_dir()):
        out = mission / "mod" / "addons" / f"{mod_src.name}.pbo"
        n = write_pbo(mod_src, out, prefix=mod_src.name)
        log(f"  companion mod {mod_src.name}: {n} files -> {out.relative_to(mission)}")
    if entries or (mission / "mod").is_dir():
        entries.append(f"Missions\\{mission.name}\\mod")
    return entries


def write_launcher(mission: Path, mission_dir: str, mods: list[str]) -> None:
    content = LAUNCHER_TEMPLATE.format(mission_dir=mission_dir, mods=";".join(mods))
    # Batch files want CRLF.
    (mission / LAUNCHER_NAME).write_bytes(content.replace("\n", "\r\n").encode("utf-8"))


def zip_mission(mission: Path, out: Path) -> None:
    out.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(out, "w", zipfile.ZIP_DEFLATED) as zf:
        for path in sorted(mission.rglob("*")):
            if path.is_file():
                zf.write(path, f"{mission.name}/{path.relative_to(mission).as_posix()}")


def build(name: str, install_root: Path | None) -> Path:
    variant = load_variant(name)
    mission_dir = variant["mission_dir"]
    log(f"[{name}] {variant.get('description', '')}")

    upstream_src = fetch_upstream(variant["upstream"])

    mission = BUILD_DIR / name / mission_dir
    shutil.rmtree(mission.parent, ignore_errors=True)
    mission.mkdir(parents=True)

    n = copy_tree(upstream_src, mission, skip=UPSTREAM_FILES_TO_DROP)
    log(f"  upstream CE files: {n}")
    n = copy_tree(COM_DIR, mission / "core", skip={"init.c", "config.cpp", "cyphermedia.nfo"})
    for top in ("init.c", "config.cpp", "cyphermedia.nfo"):
        shutil.copy2(COM_DIR / top, mission / top)
        n += 1
    log(f"  COM files: {n}")
    overrides = OVERRIDES_DIR / name
    if overrides.is_dir():
        n = copy_tree(overrides, mission)
        if n:
            log(f"  override files: {n}")

    apply_json_patches(mission, variant)
    n = generate_spawn_points(mission)
    log(f"  spawn points: {n}")
    n = generate_spawn_sets(mission, variant)
    log(f"  spawn sets: {n}")
    mod_entries = build_companion_mods(name, mission)

    n = rewrite_mission_dir(mission, mission_dir)
    if n:
        log(f"  rewrote include paths in {n} script(s) -> {mission_dir}")
    write_launcher(mission, mission_dir, variant["mods"] + mod_entries)

    out = DIST_DIR / f"DayZCommunityOfflineMode-DeerIsle-{name}.zip"
    zip_mission(mission, out)
    log(f"  wrote {out.relative_to(ROOT)} ({out.stat().st_size // 1024} KiB)")

    if install_root is not None:
        target = install_root / "Missions" / mission_dir
        # Keep profiles/ (state, settings, logs) and storage_-1. Locked files mean DayZ is running.
        keep = {"profiles", "storage_-1"}
        try:
            if target.is_dir():
                for entry in target.iterdir():
                    if entry.name in keep:
                        continue
                    if entry.is_dir():
                        shutil.rmtree(entry)
                    else:
                        entry.unlink()
            shutil.copytree(mission, target, dirs_exist_ok=True)
        except OSError as exc:
            sys.exit(f"cannot replace {target} (is DayZ still running?): {exc}")
        log(f"  installed to {target}")
    return out


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("variants", nargs="*", help="variant name(s); default: all")
    parser.add_argument("--list", action="store_true", help="list variants and exit")
    parser.add_argument(
        "--install",
        metavar="DAYZ_DIR",
        type=Path,
        help="copy the built mission into DAYZ_DIR/Missions (keeps any existing storage_-1)",
    )
    args = parser.parse_args(argv)

    if args.list:
        for v in list_variants():
            print(f"{v}\t{load_variant(v).get('description', '')}")
        return 0

    names = args.variants or list_variants()
    if args.install and len(names) > 1:
        # Different variants use different mission folder names, so this is
        # safe, but be explicit about what's happening.
        log(f"installing {len(names)} variants into {args.install}")
    for name in names:
        build(name, args.install)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
