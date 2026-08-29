// Upstream DoWork measures only on dedicated servers and starts the beep via net-sync.
modded class jmc_GPSReceiver
{
	override void DoWork()
	{
		if (GetGame().IsMultiplayer())
		{
			super.DoWork();
			return;
		}

		PlayerBase player;
		if (!Class.CastTo(player, GetHierarchyParent()))
			return;

		JMC_ItemSearchManager searchManager = GetDayZGame().GetJMCIteamSearchManager();
		float distanceToNearest = searchManager.GetDistanceToNearestStash(GetPosition());

		// PlayerBase.Message() goes to CGame.Chat(), which does not display offline.
		GetGame().GetMission().OnEvent(ChatMessageEventTypeID, new ChatMessageEventParams(CCDirect, "", searchManager.GetGPSMeasurementResult(distanceToNearest), ""));
		SetSoundIntervalType(distanceToNearest);
		if (m_LocalPlaySoundInterval != m_PlaySoundInterval)
			PlaySounds();
	}
}
