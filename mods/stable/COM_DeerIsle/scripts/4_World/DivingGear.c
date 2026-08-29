// Upstream JSONSynch/QuantityAdjust run only on dedicated servers and reach clients by net-sync.
modded class ADM_DivingGear_Base
{
	override void JSONSynch()
	{
		if (GetGame().IsMultiplayer())
		{
			super.JSONSynch();
			return;
		}

		DivingConfig config = GetDayZGame().GetDivingConfig();
		string classname = GetType();
		m_IsDivingHelmet = config.GetIsDivingHelmet(classname);
		m_IsDivingSuit = config.GetIsDivingSuit(classname);
		m_IsDivingTank = config.GetIsDivingTank(classname);
		m_IsSnorkel = config.GetIsSnorkel(classname);
		m_IsFlippers = config.GetIsFlippers(classname);
		m_GearSwimSpeedMulti = config.GetGearSwimSpeedMulti(classname);
		m_GearSprintSwimSpeedMulti = config.GetGearSprintSwimSpeedMulti(classname);
		m_MaxTankCapacity = config.GetMaxTankCapacity(classname);
		SetSynchDirty();
	}

	override void QuantityAdjust()
	{
		if (GetGame().IsMultiplayer())
		{
			super.QuantityAdjust();
			return;
		}

		if (GetQuantity() > GetMaxTankCapacity())
			SetQuantity(GetMaxTankCapacity());
	}
}
