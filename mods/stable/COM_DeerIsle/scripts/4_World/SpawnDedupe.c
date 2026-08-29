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
		if (type == "")
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
			GetGame().ObjectDelete(older);
		}
		s_Seen.Set(key, obj);
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
