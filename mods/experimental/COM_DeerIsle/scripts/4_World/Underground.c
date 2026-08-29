// Vanilla: server spawns carriers, the client spawns the trigger on net-sync, client
// trigger events feed UndergroundHandlerClient, EOnFrame ticks it for INSTANCETYPE_CLIENT
// only. Single player is wired up in DIAG builds only.

modded class UndergroundTriggerCarrier
{
	override void EEInit()
	{
		super.EEInit();
		if (!GetGame().IsMultiplayer() && !m_Trigger)
			SpawnTrigger();
	}

	override void SpawnTrigger()
	{
		if (m_Trigger)
			return;
		super.SpawnTrigger();
		if (m_Trigger)
			Print("[COM] underground trigger " + m_TriggerIndex + " spawned at " + GetPosition());
	}
}

modded class UndergroundTrigger
{
	bool m_COM_ClientInside;

	override protected void OnEnterClientEvent(TriggerInsider insider)
	{
		if (m_COM_ClientInside)
			return;
		m_COM_ClientInside = true;
		Print("[COM] underground trigger enter at " + GetPosition() + " type " + m_Type + " acco " + m_Accommodation);
		super.OnEnterClientEvent(insider);
	}

	override protected void OnLeaveClientEvent(TriggerInsider insider)
	{
		if (!m_COM_ClientInside)
			return;
		m_COM_ClientInside = false;
		super.OnLeaveClientEvent(insider);
	}

	override protected void OnEnterServerEvent(TriggerInsider insider)
	{
		super.OnEnterServerEvent(insider);
		if (!GetGame().IsMultiplayer())
			OnEnterClientEvent(insider);
	}

	override protected void OnLeaveServerEvent(TriggerInsider insider)
	{
		super.OnLeaveServerEvent(insider);
		if (!GetGame().IsMultiplayer())
			OnLeaveClientEvent(insider);
	}
}

modded class PlayerBase
{
	bool m_COM_InstanceTypeLogged;

	override void EOnFrame(IEntity other, float timeSlice)
	{
		super.EOnFrame(other, timeSlice);

		if (GetGame().IsMultiplayer() || !IsControlledPlayer())
			return;

		if (!m_COM_InstanceTypeLogged)
		{
			m_COM_InstanceTypeLogged = true;
			Print("[COM] local player instance type " + GetInstanceType());
		}

		// Vanilla ticks the underground handlers only for INSTANCETYPE_CLIENT.
		if (GetInstanceType() != DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
		{
			if (m_UndergroundHandler)
				m_UndergroundHandler.Tick(timeSlice);
			if (m_UndergroundBunkerHandler)
				m_UndergroundBunkerHandler.Tick(timeSlice);
		}
	}
}
