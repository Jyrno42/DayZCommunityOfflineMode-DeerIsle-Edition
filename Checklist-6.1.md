# Questline for 6.1 checklist in offline mode

- [x] Purple card spawns where its supposed to spawn
- [x] X65 spawns where its supposed to spawn
- [x] Punchcard spawns where its supposed to
- [x] Punchcard conversion devices on Paris Island work and can make other mode cards
- [x] convoys/campsites/helicrashes/submarines work
- [x] Water at KMUC
  - COM now seeds `%localappdata%\DayZ\Deerisle\KMUCFloodState.json` as flooded on first run (mod default was drained; water only rose 120 min after the door opened). Delete the file to re-seed
  - Verified: flooded on start, drains when the door opens
- [x] KMUC door works
  - Opens after the 300 s cooldown from SecurityDoorConfig.json
  - Alarm sound: mod gates it on IsMultiplayer(); companion mod COM_DeerIsle plays it offline (verified)
- [x] Green card door works
  - Was doubled (DeerIsleBase constructed twice offline); COM_DeerIsle dedupes its spawns and keeps one endgame manager/plate maze
- [x] Green card spawns loot
  - Needed a fresh SecurityDoorConfig.json (2023 file lacked the loot tables); per-mission profiles avoid that now
- [x] staff spawns when purple used
  - Skull spawns at the moment the KMUC door opens (after the cooldown)
- [x] Hammer head spawns where its supposed to
- [x] Hammer handle spawns where its supposed to after teleporting with staff and smoke nade
- [x] Diving gear works
  - ADM sets gear/player diving state only on dedicated servers; COM_DeerIsle (DivingGear.c, DivingPlayer.c) applies it offline
- [x] Diving crates exist and have loot
  - Placed by DeerIsle_FinderOuter's MissionServer hook (shadow MissionServer)
- [x] Beacon tracker for diving crates works
  - Tracker only measured on dedicated servers; COM_DeerIsle (GpsTracker.c) does it locally, chat text via the mission chat event
- [x] Sinking to the sea floor in a diving suit no longer kills (fall damage in water is skipped offline, FallDamage.c)
- [x] Ice temple has a door and hammer works to open it
  - Door is placed by DeerIsleBase / IceChunkBlocker from the MissionServer hook; works since the shadow MissionServer
- [x] Levers spawn in ice temple and pulling them opens the door
- [x] Green card spawns in ice temple
- [x] Smokey spawns when lever pulled in ice temple
  - [x] Knocks you out when you walk into him in the reactor room at ice temple

- [x] Boat ticket works
  - [x] spawned boat works
- [x] Diving tank ticket works

- [ ] staff can be lit (anti cheese checks still in play)
- [ ] lit staff can be used to light temple of gods bowl and the water level lowers when bowl gets lit
- [ ] teleport to EG area from temple of gods works
- [ ] kmuc door in EG are works
- [ ] punchcard can be used to open reactor room
- [ ] lit staff can be used to start EG event


## Misc

- [x] smokey TP does not work
  - Works with the staff in hands
- [x] thrown smokey from nade stays in one place
  - Since 1.29 sleeping physics bodies get no EOnSimulate; COM_DeerIsle keeps JMC_Smokey awake (SetDynamicPhysicsLifeTime(-1) + dBodyActive)
- [x] Cant fill diving tank
  - Same fix; crouch next to a running compressor with the tank in hands
- [x] Theres no gas at crater
  - Works: EffectAreaLoader (cfgEffectArea.json) now runs via the shadow MissionServer
- [x] Theres no gas at aircraft carrier
  - Works, same fix
- [x] ATE's may not work
  - Underground darkness: vanilla only spawned the local trigger on net-sync; COM_DeerIsle spawns it at carrier init (KMUC and others verified; some mines have no trigger in cfgundergroundtriggers.json)
  - AreaTriggerEvents required a PlayerIdentity; COM_DeerIsle fires them for the offline player
- [x] Dark nights: cfggameplay.json was never loaded offline (needs serverDZ.cfg); COM loads it now, and the experimental variant patches lightingConfig to 1
- [x] exploration does not seem to work
  - Works now; harmless `MarkZoneVisited` NULL identity print in the log on each new zone
- [x] Bodies dont persist upon death
  - Offline "Restart" reloaded the whole mission. Pause menu now has Respawn (new character, same world, body stays) and a small Restart with a confirmation dialog
