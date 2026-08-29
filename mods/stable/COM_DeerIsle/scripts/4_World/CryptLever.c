// 5.9's lever action matches on the "doors1" component name, which
// ActionTargets.Update() no longer reports on 1.29. Ported from 6.1.
modded class jmc_dungeon_Door06_Double_Lever
{
	override void InitializeDoor(vector doorPosition, vector doorOrientation)
	{
		super.InitializeDoor(doorPosition, doorOrientation);
		if (linkedDoor)
			linkedDoor.COM_SetLinkedLever(this);
	}

	bool CanPullLever()
	{
		if (!linkedDoor)
			return false;

		return IsDoorClosed(0) && linkedDoor.IsDoorClosed(0) && linkedDoor.CanChangeDoorState();
	}

	override void PullLever()
	{
		if (!GetGame().IsServer() || !CanPullLever())
			return;

		super.PullLever();
	}
}

modded class jmc_dungeon_Door06_Double
{
	protected jmc_dungeon_Door06_Double_Lever m_COM_LinkedLever;

	void COM_SetLinkedLever(jmc_dungeon_Door06_Double_Lever lever)
	{
		m_COM_LinkedLever = lever;
	}

	override void OnDoorCloseStart(DoorStartParams params)
	{
		super.OnDoorCloseStart(params);

		if (!GetGame().IsServer() || params.param1 != 0 || !m_COM_LinkedLever)
			return;

		m_COM_LinkedLever.CloseDoor(0);
	}
}

modded class ActionOpenCryptoDoor
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!player || !target || !IsBuilding(target))
			return false;

		jmc_dungeon_Door06_Double_Lever lever;
		if (!Class.CastTo(lever, target.GetObject()))
			return false;

		if (!IsInReach(player, target, UAMaxDistances.DEFAULT))
			return false;

		return lever.CanPullLever();
	}
}
