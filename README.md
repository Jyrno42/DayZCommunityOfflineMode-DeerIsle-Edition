# DayZ Community Offline Mode - DeerIsle Edition

Play DayZ on the [DeerIsle](https://steamcommunity.com/sharedfiles/filedetails/?id=1602372402) map (stable) or its [experimental 6.1 dev build](https://steamcommunity.com/sharedfiles/filedetails/?id=1750506510) offline, in single player. You get a free camera, teleporting, item and object spawning, an object editor and infinite ammo. This is not normal DayZ gameplay. It is meant for looking around the map, testing things and building, without touching your online progress.

Supported map versions:

| Download | DeerIsle version | Workshop item |
|---|---|---|
| `DayZCommunityOfflineMode-DeerIsle-stable.zip` | 5.9 (stable) | [DeerIsle](https://steamcommunity.com/sharedfiles/filedetails/?id=1602372402) |
| `DayZCommunityOfflineMode-DeerIsle-experimental.zip` | 6.1 (experimental) | [DeerIsle Official (Experimental - Dev Build)](https://steamcommunity.com/sharedfiles/filedetails/?id=1750506510) |

Both are tested on DayZ 1.29. Get them from the [Releases](https://github.com/Jyrno42/DayZCommunityOfflineMode-DeerIsle-Edition/releases) page.

This is a maintained fork of [CypherMediaGIT/DayZCommunityOfflineMode-DeerIsle-Edition](https://github.com/CypherMediaGIT/DayZCommunityOfflineMode-DeerIsle-Edition), which is based on Arkensor's [DayZCommunityOfflineMode](https://github.com/Arkensor/DayZCommunityOfflineMode). Neither of those is updated any more, so this fork keeps things working with current DeerIsle and DayZ versions.

## Installation

1. On the Steam Workshop, subscribe to the DeerIsle map you want (see the table above) and to [Community Framework (CF)](https://steamcommunity.com/sharedfiles/filedetails/?id=1559212036), which DeerIsle requires. Start DayZ once through the official launcher with those mods enabled so that Steam downloads them into `DayZ\!Workshop\`.
2. Download the matching zip from [Releases](https://github.com/Jyrno42/DayZCommunityOfflineMode-DeerIsle-Edition/releases).
3. Extract it into the `Missions` folder of your DayZ install, for example `C:\Program Files (x86)\Steam\steamapps\common\DayZ\Missions`. This gives you `Missions\DayZCommunityOfflineMode.deerisle` (stable) or `Missions\DayZCommunityOfflineModeExp.deerisle` (experimental). The two can be installed side by side.
4. Run `DayZCOfflineMDeerIsle.bat` inside the mission folder. It starts the game straight into the offline mission with the right mods loaded. Note that it closes any DayZ that is already running and deletes the previous session's `storage_-1` folder, so every start is a fresh world.

Even though this is not directly bannable by BattlEye, just to make sure: rename your `Battleye` folder to `Battleye.disabled`, and rename `DayZ_BE.exe` to `DayZ_BE.exe.disabled`.

To uninstall, delete the mission folder(s) and restore any renamed BattlEye files.

## Controls

| Key | Action |
|---|---|
| Y (Z on QWERTZ) | Open the COM toolbar menu |
| X | Toggle auto-jog / walk / run |
| Shift + X | Auto-run (X again to stop) |
| Ctrl + X | Auto-walk (X again to stop) |
| End | Teleport to where you are looking |
| O | Spawn a random infected |
| Ctrl + O | Spawn a wolf (attacks players and infected) |
| Shift + O | Spawn a random animal |
| R | Reload and refill ammo (infinite ammo) |
| P | Print your position to chat and to the script log |
| B | Toggle debug monitor |
| Insert | Toggle free camera (teleports you to the camera position when turned off) |

## Object editor

Open it from the toolbar menu.

* Click an object to select it. Click and drag to move it. Click on nothing to deselect.
* Middle click snaps the selected object to the ground (not always exact).
* Spawn new objects with the object spawner in the toolbar.
* Values in the object editor panel can be typed in, or changed with the scroll wheel while hovering over them.
* `SAVE` in the Object Info panel writes your placed objects to `Documents\DayZ\COMObjectEditorSaveDeerIsle.json`. You can do this at any time so you don't lose your progress.
* `EXPORT` copies the placed objects as script code to the clipboard, which you can paste into the `init.c` of a server or another mission. Arkensor's wiki page covers this: [Add custom objects to your server or mission](https://github.com/Arkensor/DayZCommunityOfflineMode/wiki/Add-custom-objects-to-your-server-or-mission).

## Spawning, loot and infected

* You spawn with a basic loadout at one of the map's own fresh spawn locations (the ones in DeerIsle's `cfgplayerspawnpoints.xml`).
* After dying (or any time from the pause menu), `Respawn` gives you a new character in the same world, so your body and gear stay where they were. `Restart` reloads the whole mission and asks for confirmation first, because everything since launch is lost.
* The "hive" that spawns loot and infected is enabled by default. Disabling it improves performance; see [Toggle loot and infected spawn](https://github.com/CypherMediaGIT/DayZCommunityOfflineMode-DeerIsle-Edition/wiki/Toggle-Loot-and-Infected-Spawn).
* The location list in the teleport menu was last updated for DeerIsle 4.x, so some entries may be off on newer map versions. [dayz.ginfo.gg/deerIsle](https://dayz.ginfo.gg/deerIsle/) has a current map.

## Log files

They are in the `profiles` folder inside the mission folder, for example `Missions\DayZCommunityOfflineMode.deerisle\profiles`. `DayZ_x64_*.RPT` is the engine log, `script_*.log` the script log. Positions you print with `P` end up in the script log, so you can find them again later. When reporting a problem, attach the newest of both.

## Reporting problems

Open an issue in the [issue tracker](https://github.com/Jyrno42/DayZCommunityOfflineMode-DeerIsle-Edition/issues). Mention which zip you use (stable or experimental) and your DayZ version, and attach the log files described above.

---

## For developers

### Repository layout

```
com/                 Community Offline Mode scripts (core/, init.c, config.cpp)
variants/<name>.json Where the DeerIsle mission files come from, mission folder name, -mod list
overrides/<name>/    Files copied over the mission for one variant (optional)
mods/<name>/<Mod>/   Companion mod sources, packed into <mission>/mod/addons/<Mod>.pbo
build.py             Assembles and zips a mission
.github/workflows/   CI: builds every variant, publishes zips on v* tags
```

The DeerIsle central economy files (`cfg*.xml`, `db/`, `env/`, `mapgroup*.xml`, `areaflags.map` and so on) are not stored in this repository. `build.py` downloads them from
[johnmclane666/Deerisle-Stable](https://github.com/johnmclane666/Deerisle-Stable) (`V5.9/empty.deerisle`) and
[johnmclane666/Deerisle-6.0-Experimental](https://github.com/johnmclane666/Deerisle-6.0-Experimental) (`empty.deerisle`)
at the commit pinned in the variant file, then layers `com/` and `overrides/<variant>/` on top.

The build also:

* applies the variant's `json_patches` to the mission's JSON files (experimental sets `lightingConfig` to 1 for dark nights) and generates `core/SpawnSets.c` from its `spawn_sets` (the "Spawn dive set" entry in the COM script menu);
* generates `core/SpawnPoints.c` (`COM_GetSpawnPoints()`) from the map's `cfgplayerspawnpoints.xml` `<fresh>` bubbles;
* rewrites the absolute `#include` / layout paths in the scripts when a variant uses a different mission folder name (the sources are written against `DayZCommunityOfflineMode.deerisle`);
* packs every folder under `mods/<variant>/` into a PBO in `<mission>/mod/addons/` and appends that mod folder to the `-mod=` list. Mission scripts only see the base game's script modules, so anything that has to patch a map mod (`modded class` on its classes) lives here. On experimental, `COM_DeerIsle` stops `DeerIsleBase` from placing its objects twice, keeps Smokey's physics body awake so it moves, plays the security-door alarm, makes the diving mod, the crate beacon and in-water fall damage work in single player, and drops the identity print in `MarkZoneVisited`. Most of those are the same bug: the mod does its server-side work only when `IsDedicatedServer()` is true and relies on net-sync to reach the client, so in single player neither side runs;
* writes `DayZCOfflineMDeerIsle.bat` with the variant's `-mod=` list. The launcher also passes `-profiles=<mission>\profiles`, so each variant keeps its own mod state (`Deerisle\*.json`), game settings and logs inside its mission folder; on the first start it copies your video and control settings from `Documents\DayZ`.

### Building

Requires Python 3 (standard library only).

```
python build.py                       # every variant -> dist/*.zip
python build.py stable                # one variant
python build.py --list                # show variants
python build.py stable --install "C:\Program Files (x86)\Steam\steamapps\common\DayZ"
                                      # build and copy into <DayZ>\Missions (keeps storage_-1)
```

Intermediate files go to `build/` and zips to `dist/`. Both are git-ignored. Downloaded upstream tarballs are cached in `build/upstream/`.

### Updating to a new DeerIsle release

1. Bump `upstream.ref` (and `path`, if the upstream repo moved things) in `variants/<variant>.json`.
2. Build, `--install`, launch, and check `<mission>\profiles\DayZ_x64_*.RPT` for `Virtual Machine Exception` after `Player connect enabled`.
3. If a mission file needs to differ for that map version, put the replacement in `overrides/<variant>/` rather than editing `com/`.

Offline mode runs as a `MissionGameplay`, but DeerIsle keeps most of its world logic (Smokey, KMUC flooding, temple cage, area trigger events, diving config, midnight events, and so on) in `modded class MissionServer`, which never runs in a plain `MissionGameplay`. The vanilla loaders for `cfggameplay.json`, underground darkness triggers and contaminated areas also only run there. So `CommunityOfflineClient.OnInit()` creates a `MissionServer` instance of its own and calls its `OnInit()` and `OnMissionStart()`; that runs every mod's server-side hooks without listing them one by one. Nothing else is forwarded to it (no `OnUpdate`, `OnEvent`, `InvokeOnConnect`), so hooks that need a `PlayerIdentity` or RPCs stay inactive, which is fine offline because the config globals are shared in one process anyway. A "NULL pointer to instance" on `GetIdentity()` in a mod's log print is the usual harmless side effect.

### Releasing

Push a `v*` tag. CI builds both zips and attaches them to a GitHub release.

## Credits

* [Jyrno42](https://github.com/Jyrno42) - fork maintainer
* [Cypher](https://github.com/CypherMediaGIT) - DeerIsle edition
* [Arkensor](https://github.com/Arkensor) - DayZ Community Offline Mode, without which this would not exist
* [DannyDog](https://github.com/DannyDog), [Jacob_Mango](https://github.com/Jacob-Mango) - COM development
* [johnmclane666](https://github.com/johnmclane666) - DeerIsle mission files
* Contributors: [Chubby The Gamer](https://github.com/ChubbyTheGamer) (custom objects sinking fix), [gallexme](https://github.com/gallexme) (mission-based version), [DuhOneZ](https://twitter.com/DuhOneZ) (code snippets), [Watchman](https://twitter.com/watchman113) (documentation), [n8m4re](https://github.com/n8m4re) (SaveManager), [wriley](https://github.com/wriley) (beards), [PR9INICHEK](https://github.com/PR9INICHEK) (object spawner additions)

## License

[Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International](https://creativecommons.org/licenses/by-nc-sa/4.0/), the same license as the works it is derived from. See [LICENSE](LICENSE) for the full text.

Copyright Paul-Eric Lange (Arkensor) 2018, Copyright Cypher 2019, Copyright Jürno Ader (Jyrno42) 2023-2026.
