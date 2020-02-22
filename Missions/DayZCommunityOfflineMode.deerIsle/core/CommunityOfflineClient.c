class CommunityOfflineClient extends MissionGameplay
{
	protected bool HIVE_ENABLED = true; //Local Hive / Economy / Infected spawn

    protected bool m_Loaded;

	void CommunityOfflineClient()
	{
	    m_Loaded = false;

		NewModuleManager();
	}

	override void OnInit()
	{
		super.OnInit();

        InitHive();

        SetupWeather();

		SpawnPlayer();

		GetDayZGame().SetMissionPath( "$saves:CommunityOfflineMode\\" ); // CameraToolsMenu
	}

	override void OnMissionStart()
	{
		super.OnMissionStart();

        GetModuleManager().OnInit();
		GetModuleManager().OnMissionStart();
	}

	override void OnMissionFinish()
	{
        GetModuleManager().OnMissionFinish();

		CloseAllMenus();

		DestroyAllMenus();

		if( GetHive() )
		{
			DestroyHive();
		}

		super.OnMissionFinish();
	}

    void OnMissionLoaded()
    {
		GetModuleManager().OnMissionLoaded();
    }

	override void OnUpdate( float timeslice )
	{
	    super.OnUpdate( timeslice );

        GetModuleManager().OnUpdate( timeslice );

        if( !m_Loaded && !GetDayZGame().IsLoading() )
        {
            m_Loaded = true;
            OnMissionLoaded();
        }
	}

    void SpawnPlayer()
    {
//		#ifndef MODULE_PERSISTENCY
//		GetGame().SelectPlayer( NULL, CreateCustomDefaultCharacter() );
//		#endif

//		#ifdef DISABLE_PERSISTENCY
		GetGame().SelectPlayer( NULL, CreateCustomDefaultCharacter() );
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
};