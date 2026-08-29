// Upstream sends this as an identity RPC, dropped offline.
modded class DeerIsleBase
{
	override void SendChatMessageToPlayer(PlayerBase player, string message)
	{
		if (GetGame().IsMultiplayer())
		{
			super.SendChatMessageToPlayer(player, message);
			return;
		}

		GetGame().GetMission().OnEvent(ChatMessageEventTypeID, new ChatMessageEventParams(CCDirect, "", "DeerIsle: " + message, ""));
	}
}
