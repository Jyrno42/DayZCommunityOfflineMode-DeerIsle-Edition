// Gated on IsMultiplayer() upstream.
modded class SireneManager
{
	override void StartSireneSounds()
	{
		if (GetGame().IsMultiplayer())
		{
			super.StartSireneSounds();
			return;
		}

		foreach (Object obj : sirenes)
		{
			if (obj)
				obj.PlaySound("deerisle_security_system_alarm_sound", 500, false);
		}
	}
}
