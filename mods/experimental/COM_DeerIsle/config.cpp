// Single-player patches for DeerIsle mods; packed by build.py into <mission>/mod/addons.
class CfgPatches
{
	class COM_DeerIsle
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "JMC_EG", "ADM_Diving_Mod", "DeerIsle_FinderOuter", "AreaTriggerEvents"};
	};
};
class CfgMods
{
	class COM_DeerIsle
	{
		dir = "COM_DeerIsle";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "COM DeerIsle offline fixes";
		credits = "";
		author = "Jyrno42";
		authorID = "0";
		version = "1.0";
		extra = 0;
		type = "mod";
		dependencies[] = {"World", "Mission"};
		class defs
		{
			class worldScriptModule
			{
				value = "";
				files[] = {"COM_DeerIsle/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"COM_DeerIsle/scripts/5_Mission"};
			};
		};
	};
};
