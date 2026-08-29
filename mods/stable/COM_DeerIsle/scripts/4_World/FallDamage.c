// Upstream skips fall damage in water only on dedicated servers.
modded class DayZPlayerImplementFallDamage
{
	override void HandleFallDamage(FallDamageData pData)
	{
		if (!GetGame().IsMultiplayer())
		{
			vector waterLevel;
			if (DayZPlayerUtils.CheckWaterLevel(m_Player, waterLevel) == EWaterLevels.LEVEL_SWIM_START)
				return;
		}

		super.HandleFallDamage(pData);
	}
}
