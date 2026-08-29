// Upstream OnEnter requires a PlayerIdentity; none offline. Notification shown locally instead of by RPC.
modded class AreaTrigger
{
	override void OnEnter(Object obj)
	{
		if (GetGame().IsMultiplayer())
		{
			super.OnEnter(obj);
			return;
		}

		PlayerBase player = PlayerBase.Cast(obj);
		if (!player || !IsWorkingTime())
			return;

		int now = GetGame().GetTime() / 1000;
		ATELogger.GetInstance().Log("Triggered " + GetTriggerName() + " time " + now);
		if (!CanTriggerAction(now))
		{
			ATELogger.GetInstance().Log("Cannot trigger action it was called at " + m_LastTriggerTime);
			return;
		}

		SetLastTriggerTime(now);
		TriggerActivate();
		AdditionalAction();
		if (GetTriggerNotification() != "")
			NotificationSystem.AddNotificationExtended(GetTriggerNotificationTime(), GetTriggerName(), GetTriggerNotification());
		ATELogger.GetInstance().Log("offline player triggered " + GetTriggerName(), ATELogger.LOGLEVEL_CRITICAL);
		Print("[COM] AreaTriggerEvents: triggered " + GetTriggerName());
	}
}
