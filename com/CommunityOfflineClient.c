class CommunityOfflineClient extends MissionGameplay
{
	protected bool HIVE_ENABLED = true; //Local Hive / Economy / Infected spawn

    protected bool m_loaded;

	// Runs the modded MissionServer hooks and the server-side loaders (cfggameplay,
	// underground triggers, effect areas) that MissionGameplay only runs in DIAG builds.
	// Only OnInit/OnMissionStart/OnMissionFinish are forwarded.
	protected MissionServer m_ServerLogic;

	void CommunityOfflineClient()
	{
	    m_loaded = false;

		NewModuleManager();
	}

	override void OnInit()
	{
		super.OnInit();

		SeedDeerIsleProfileDefaults();

		// GetGame().GetMission() is still NULL in the constructor and mod constructors spawn items.
		m_ServerLogic = new MissionServer();
		// Yield defaults are already registered by this mission.
		GetDayZGame().GetYieldDataInitInvoker().Remove( m_ServerLogic.InitWorldYieldDataDefaults );
		m_ServerLogic.OnInit();

		LoadCfgGameplay();

        InitHive();

        SetupWeather();

		SpawnPlayer();

		// Normally triggered by the cfggameplay sync RPC on connect.
		PlayerBase player = COM_GetPB();
		if ( player )
			player.OnGameplayDataHandlerSync();

		GetDayZGame().SetMissionPath( "$saves:CommunityOfflineMode\\" ); // CameraToolsMenu
	}

	override void OnMissionStart()
	{
		super.OnMissionStart();

		m_ServerLogic.OnMissionStart();

        COM_GetModuleManager().OnInit();
		COM_GetModuleManager().OnMissionStart();
	}

	override void OnMissionFinish()
	{
		m_ServerLogic.OnMissionFinish();
		m_ServerLogic = null;

        COM_GetModuleManager().OnMissionFinish();

		CloseAllMenus();

		DestroyAllMenus();

		if( GetHive() )
		{
			DestroyHive();
		}

		super.OnMissionFinish();
	}

    override void OnMissionLoaded()
    {
		COM_GetModuleManager().OnMissionLoaded();

		super.OnMissionLoaded();
    }

	override void OnUpdate( float timeslice )
	{
	    super.OnUpdate( timeslice );

        COM_GetModuleManager().OnUpdate( timeslice );

        if( !m_loaded && !GetDayZGame().IsLoading() )
        {
            m_loaded = true;
            OnMissionLoaded();
        }
	}

	// MissionServer.OnInit() reads cfggameplay.json only with enableCfgGameplayFile in serverDZ.cfg.
	static void LoadCfgGameplay()
	{
		string error;
		if ( JsonFileLoader<CfgGameplayJson>.LoadFile( "$mission:cfggameplay.json", CfgGameplayHandler.m_Data, error ) )
		{
			CfgGameplayHandler.OnLoaded();
			Print( "COM: loaded cfggameplay.json (lightingConfig " + CfgGameplayHandler.GetLightingConfig() + ")" );
		}
		else
		{
			Print( "COM: cfggameplay.json not loaded: " + error );
		}
	}

	// DeerIsle mods create $profile:Deerisle/*.json with fresh-server defaults on first
	// run. Seed single-player values before that; existing files are left alone.
	// Plain text so this compiles without the mod.
	static void SeedDeerIsleProfileDefaults()
	{
		if ( !FileExist( "$profile:Deerisle" ) )
			MakeDirectory( "$profile:Deerisle" );

		// Mod default is drained, refilling 2 h after the door opens.
		// IsFlooded is the drain target: false = water at flood height.
		if ( !FileExist( "$profile:Deerisle/KMUCFloodState.json" ) )
		{
			FileHandle file = OpenFile( "$profile:Deerisle/KMUCFloodState.json", FileMode.WRITE );
			if ( file )
			{
				FPrintln( file, "{" );
				FPrintln( file, "    \"INFO_DO_NOT_CHANGE\": \"ONLY change Enabled, DrainTime and FloodTime. Leave the rest as is!\"," );
				FPrintln( file, "    \"KMUCWaterEnabled\": 1," );
				FPrintln( file, "    \"DrainTimeMins\": 25.0," );
				FPrintln( file, "    \"FloodTimeMins\": 45.0," );
				FPrintln( file, "    \"IsFlooded\": 0," );
				FPrintln( file, "    \"IsMoving\": 0," );
				FPrintln( file, "    \"CurrentHeight\": 574.25" );
				FPrintln( file, "}" );
				CloseFile( file );
				Print( "COM: seeded $profile:Deerisle/KMUCFloodState.json (KMUC flooded)" );
			}
		}
	}

    void SpawnPlayer()
    {
//		#ifndef MODULE_PERSISTENCY
//		GetGame().SelectPlayer( NULL, COM_CreateCustomDefaultCharacter() );
//		#endif

//		#ifdef DISABLE_PERSISTENCY
		GetGame().SelectPlayer( NULL, COM_CreateCustomDefaultCharacter() );
//		#endif
    }

	void InitHive()
	{
		if ( GetGame().IsClient() && GetGame().IsMultiplayer() ) return;

		// RD /s /q "storage_-1" > nul 2>&1
		if ( !HIVE_ENABLED ) return;
	
		Hive oHive = GetHive();
		
		if( !oHive )
		{
			oHive = CreateHive();
		}

		if( oHive )
		{
			oHive.InitOffline();
		}

		oHive.SetShardID("100");
		oHive.SetEnviroment("stable");
	}

    static void SetupWeather()
    {
        Weather weather = g_Game.GetWeather();

        weather.GetOvercast().SetLimits( 0.0 , 2.0 );
        weather.GetRain().SetLimits( 0.0 , 2.0 );
        weather.GetFog().SetLimits( 0.0 , 2.0 );

        weather.GetOvercast().SetForecastChangeLimits( 0.0, 0.0 );
        weather.GetRain().SetForecastChangeLimits( 0.0, 0.0 );
        weather.GetFog().SetForecastChangeLimits( 0.0, 0.0 );

        weather.GetOvercast().SetForecastTimeLimits( 1800 , 1800 );
        weather.GetRain().SetForecastTimeLimits( 600 , 600 );
        weather.GetFog().SetForecastTimeLimits( 600 , 600 );

        weather.GetOvercast().Set( 0.0, 0, 0 );
        weather.GetRain().Set( 0.0, 0, 0 );
        weather.GetFog().Set( 0.0, 0, 0 );

        weather.SetWindMaximumSpeed( 50 );
        weather.SetWindFunctionParams( 0, 0, 1 );
    }

    override UIScriptedMenu CreateScriptedMenu(int id)
    {
        if(id == EditorMenu.MENU_ID)
        {
            return new EditorMenu();
        }

        if(id == MENU_INGAME)
        {
            UIScriptedMenu menu = new COMInGameMenu();
            menu.SetID(id);
            return menu;
        }

        return super.CreateScriptedMenu(id);
    }

	// New character in the running world; the old body stays.
	void Respawn()
	{
		PlayerBase fresh = COM_CreateCustomDefaultCharacter();
		if ( !fresh )
			return;

		GetGame().SelectPlayer( NULL, fresh );
		GetGame().GetUIManager().CloseAll();
		OnPlayerRespawned( fresh );
		Continue();
	}
}
