// 5.9's diving HUD checks IngameHud.IsHudVisible(), a stub that returns false on 1.29.
modded class IngameHud
{
	override void SetHUDDiveVisibility(bool wearingtank, float tankamount, bool show)
	{
		if (!m_DivingPanel)
			return;

		if (!show || m_HudVisibility.IsContextFlagActive(IngameHudVisibility.HUD_HIDE_FLAGS))
		{
			m_DivingPanel.Show(false);
			return;
		}

		bool tank = wearingtank && tankamount >= 5;
		m_DivingPanel.Show(true);
		m_TankIcon.Show(tank);
		m_DiveTankCapactiy.Show(tank);
		m_LungIcon.Show(!tank);
		m_LungCapactiy.Show(!tank);
		m_LungCapactiyBackground.Show(true);
	}
}
