// Upstream sets player diving state only on dedicated servers (net-synced to clients);
// offline MaxDiveDepth/SwimVerticleSpeed stay 0 and no gear flag is ever set.
modded class PlayerBase
{
	override void GetJSONPlayerDivingInfo()
	{
		if (GetGame().IsMultiplayer())
		{
			super.GetJSONPlayerDivingInfo();
			return;
		}

		DivingConfig config = GetDayZGame().GetDivingConfig();
		DisableDivingMode = config.GetDisableDivingMode();
		MaxLungCapacity = config.GetMaxLungCapacity();
		LungCapacity = config.GetMaxLungCapacity();
		LungBurnMultiplier = config.GetLungBurnMultiplier();
		SprintLungBurnMultiplier = config.GetSprintLungBurnMultiplier();
		MaxDiveDepth = config.GetMaxDiveDepth();
		SwimSpeed = config.GetSwimSpeed();
		SwimVerticleSpeed = config.GetSwimVerticleSpeed();
		SetSynchDirty();
	}

	override void DivingGearAttach(EntityAI item, string slot_name)
	{
		if (GetGame().IsMultiplayer())
			super.DivingGearAttach(item, slot_name);
		else
			COM_RefreshDivingGear();
	}

	override void DivingGearDetached(EntityAI item, string slot_name)
	{
		if (GetGame().IsMultiplayer())
			super.DivingGearDetached(item, slot_name);
		else
			COM_RefreshDivingGear();
	}

	override void PlayerSpawnUpdate()
	{
		if (GetGame().IsMultiplayer())
			super.PlayerSpawnUpdate();
		else
			COM_RefreshDivingGear();
	}

	// Recomputes the worn-gear state from the body slots.
	void COM_RefreshDivingGear()
	{
		if (!IsPlayerLoaded())
			return;

		bool helmet, suit, snorkel, flippers, tank;
		float swimMulti, sprintMulti;
		int tankSlot = -1;

		array<string> slots = {"Headgear", "Mask", "Eyewear", "Gloves", "Armband", "Vest", "Body", "Back", "Hips", "Legs", "Feet"};
		foreach (string slot : slots)
		{
			ADM_DivingGear_Base gear = ADM_DivingGear_Base.Cast(GetItemOnSlot(slot));
			if (!gear)
				continue;

			if (gear.IsDivingHelmet()) helmet = true;
			if (gear.IsDivingSuit()) suit = true;
			if (gear.IsSnorkel()) snorkel = true;
			if (gear.IsFlippers()) flippers = true;
			if (gear.GetGearSwimSpeedMulti() > 0)
				swimMulti = gear.GetGearSwimSpeedMulti();
			if (gear.GetGearSprintSwimSpeedMulti() > 0)
				sprintMulti = gear.GetGearSprintSwimSpeedMulti();
			if (gear.IsDivingTank() && !tank)
			{
				tank = true;
				tankSlot = InventorySlots.GetSlotIdFromString(slot);
			}
		}

		SetDivingHelmet(helmet);
		SetDivingSuit(suit);
		SetSnorkel(snorkel);
		SetFlippers(flippers);
		SetGearSwimSpeedMulti(swimMulti);
		SetGearSprintSwimSpeedMulti(sprintMulti);
		SetDivingTank(tank);
		SetDivingTankSlot(tankSlot);
	}

	// Tank drain is dedicated-server only upstream.
	override void UpdateLungs(float deltaTime)
	{
		super.UpdateLungs(deltaTime);

		if (GetGame().IsMultiplayer() || !IsUnderWater() || !HasDivingTank() || GetTankCapacity() < 5)
			return;

		float lungBurn = (GetLungBurnMultiplier() * 100) * deltaTime;
		if (IsSprinting())
			lungBurn = (GetSprintLungBurnMultiplier() * 100) * deltaTime;

		ADM_DivingGear_Base divegear = ADM_DivingGear_Base.Cast(GetItemOnSlot(InventorySlots.GetSlotName(m_SlotDivingint)));
		if (divegear)
			divegear.AddQuantity(-lungBurn);
	}
}
