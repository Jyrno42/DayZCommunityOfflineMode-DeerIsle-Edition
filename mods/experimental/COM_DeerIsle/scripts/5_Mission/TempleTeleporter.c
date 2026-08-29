// World.GetPlayerList() is empty offline.
modded class MissionServer
{
	override void TempleTeleporter()
	{
		if (GetGame().IsMultiplayer())
		{
			super.TempleTeleporter();
			return;
		}

		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		foreach (Man man : players)
		{
			PlayerBase player = PlayerBase.Cast(man);
			if (player && g_DeerIsleCore)
			{
				g_DeerIsleCore.TempleTeleportHandler(player);
				g_DeerIsleCore.TempleTeleportExitHandler(player);
			}
		}
	}
}
