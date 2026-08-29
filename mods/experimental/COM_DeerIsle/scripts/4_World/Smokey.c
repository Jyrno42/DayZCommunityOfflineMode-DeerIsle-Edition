// Sleeping physics bodies get no EOnSimulate since 1.29. dBodyActive returns false
// in the constructor and sticks from EEInit, hence both.
modded class JMC_Smokey
{
	bool m_COM_SimulateSeen;

	void JMC_Smokey()
	{
		COM_KeepAwake();
	}

	override void EEInit()
	{
		super.EEInit();
		COM_KeepAwake();
	}

	void COM_KeepAwake()
	{
		if (GetGame().IsMultiplayer())
			return;

		SetDynamicPhysicsLifeTime(-1);
		dBodyActive(this, true);
	}

	override void AttackPlayer(PlayerBase player)
	{
		Print("[COM] JMC_Smokey: AttackPlayer, distance " + vector.Distance(player.GetPosition(), GetPosition()) + ", under control " + isUnderControll);
		super.AttackPlayer(player);
	}

	override void EOnSimulate(IEntity other, float dt)
	{
		if (!m_COM_SimulateSeen)
		{
			m_COM_SimulateSeen = true;
			Print("[COM] JMC_Smokey: EOnSimulate is delivered (dt " + dt + ")");
		}
		super.EOnSimulate(other, dt);
	}
}
