// DeerIsleBase is constructed twice offline (MissionGameplay and MissionServer hooks
// both recreate it), so everything it places exists twice. SpawnObject is static and
// cannot be overridden, so while DeerIsleBase() runs, a House initialising at the spot
// of an earlier one of the same type deletes the earlier one. The newer copy is kept
// because the mod's globals point at it.
class COM_SpawnDedupe
{
	static ref map<string, Object> s_Seen;
	static bool s_Armed;

	// Random position; matched on type.
	static ref TStringArray s_UniqueTypes = {"jmc_dungeon_Door06_Double_Lever"};
	// Singleton handled separately.
	static ref TStringArray s_IgnoredTypes = {"jmc_eg_firebowl"};

	// DeerIsleBase() calls Init() first and spawns synchronously; disarmed next frame.
	static void Arm()
	{
		if (!s_Seen)
			s_Seen = new map<string, Object>;
		s_Armed = true;
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(Disarm, 0, false);
	}

	static void Disarm()
	{
		s_Armed = false;
	}

	static void OnObjectInit(Object obj)
	{
		if (!s_Armed || GetGame().IsMultiplayer())
			return;

		string type = obj.GetType();
		if (type == "" || s_IgnoredTypes.Find(type) != -1)
			return;

		string key = type;
		if (s_UniqueTypes.Find(type) == -1)
		{
			vector pos = obj.GetPosition();
			key = type + "|" + Math.Round(pos[0] * 10) + "|" + Math.Round(pos[1] * 10) + "|" + Math.Round(pos[2] * 10);
		}

		Object older;
		if (s_Seen.Find(key, older) && older && older != obj)
		{
			Print("[COM] deleting duplicate " + type + " at " + older.GetPosition());
			Cleanup(older);
			GetGame().ObjectDelete(older);
		}
		s_Seen.Set(key, obj);
	}

	// Objects the older copy spawned.
	static void Cleanup(Object older)
	{
		Land_jmc_eg_EGFloodPanel panel;
		if (Class.CastTo(panel, older) && panel.m_ActiveWaterPlanes)
		{
			foreach (Object plane : panel.m_ActiveWaterPlanes)
			{
				if (plane)
					GetGame().ObjectDelete(plane);
			}
		}
	}
}

modded class House
{
	override void EEInit()
	{
		super.EEInit();
		COM_SpawnDedupe.OnObjectInit(this);
	}
}

modded class DeerIsleBase
{
	override void Init()
	{
		super.Init();
		COM_SpawnDedupe.Arm();
	}
}

// Spawned by DeerIsleBase, so constructed twice; its singleton check compares against
// this, and a second InitEG() lays a second plate maze. Keep the first instance.
modded class jmc_eg_firebowl
{
	static jmc_eg_firebowl s_COM_FirstInstance;

	override void InitEG()
	{
		if (!GetGame().IsMultiplayer() && s_COM_FirstInstance && s_COM_FirstInstance != this)
		{
			Print("[COM] second jmc_eg_firebowl created, keeping the first one");
			s_Instance = s_COM_FirstInstance;
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetGame().ObjectDelete, 0, false, this);
			return;
		}

		s_COM_FirstInstance = this;
		super.InitEG();
	}
}
