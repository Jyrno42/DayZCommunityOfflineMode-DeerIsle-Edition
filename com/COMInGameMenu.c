// Offline, InGameMenu "Restart" is RestartMission() (world reload). Here that button
// respawns into the running world and a separate confirmed button does the restart.
class COMInGameMenu extends InGameMenu
{
	static const int COM_IDC_RESTART = 13374;
	static const string COM_BUTTON_LAYOUT = "$CurrentDir:missions\\DayZCommunityOfflineMode.deerisle\\core\\modules\\ComMenu\\gui\\layouts\\IngameMenuButton.layout";

	protected Widget m_COM_RestartButton;

	override Widget Init()
	{
		Widget root = super.Init();
		if ( GetGame().IsMultiplayer() )
			return root;

		ButtonSetText( m_RestartButton, "#main_menu_respawn" );

		Widget group = m_RestartButton.GetParent();
		if ( group )
		{
			m_COM_RestartButton = GetGame().GetWorkspace().CreateWidgets( COM_BUTTON_LAYOUT, group );
			if ( m_COM_RestartButton )
			{
				// WrapSpacer orders children by sort index
				m_ExitButton.SetSort( 0 );
				m_RestartButton.SetSort( 1 );
				m_COM_RestartButton.SetSort( 2 );
				if ( m_FeedbackButton )
					m_FeedbackButton.SetSort( 3 );
				m_OptionsButton.SetSort( 4 );
				group.Update();
			}
		}
		return root;
	}

	override protected void OnClick_Restart()
	{
		if ( GetGame().IsMultiplayer() )
		{
			super.OnClick_Restart();
			return;
		}

		CommunityOfflineClient mission = COM_GetClientMission();
		if ( mission )
			GetGame().GetCallQueue( CALL_CATEGORY_GUI ).Call( mission.Respawn );
	}

	override bool OnClick( Widget w, int x, int y, int button )
	{
		if ( m_COM_RestartButton && w == m_COM_RestartButton )
		{
			GetGame().GetUIManager().ShowDialog( "Restart mission",
				"This restarts the whole mission. The world is reloaded and everything since launch is lost: bodies, dropped and spawned items, opened doors and other progress.",
				COM_IDC_RESTART, DBT_YESNO, DBB_NO, DMT_QUESTION, this );
			return true;
		}
		return super.OnClick( w, x, y, button );
	}

	override bool OnModalResult( Widget w, int x, int y, int code, int result )
	{
		if ( code == COM_IDC_RESTART )
		{
			if ( result == DBB_YES )
				GetGame().GetCallQueue( CALL_CATEGORY_GUI ).Call( GetGame().RestartMission );
			return true;
		}
		return super.OnModalResult( w, x, y, code, result );
	}
}
