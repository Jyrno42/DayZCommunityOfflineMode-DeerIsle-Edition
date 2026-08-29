// Upstream prints GetIdentity().GetName(), NULL offline.
modded class PlayerBase
{
	override void MarkZoneVisited(string zone)
	{
		if (GetIdentity())
		{
			super.MarkZoneVisited(zone);
			return;
		}

		if (!m_VisitedCityZones)
			m_VisitedCityZones = new map<string, bool>;

		if (!m_VisitedCityZones.Contains(zone))
			m_VisitedCityZones.Insert(zone, true);
	}
}
