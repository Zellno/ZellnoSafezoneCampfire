class CfgPatches
{
    class ZellnoSafezoneCampfire
    {
        units[] =
        {
            "Zellno_SafezoneCampfire"
        };
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] =
        {
            "DZ_Data",
            "DZ_Scripts",
            "DZ_Gear_Cooking",
            "ZenFireplaceStick"
        };
    };
};

class CfgVehicles
{
    class Fireplace;

    class Zellno_SafezoneCampfire : Fireplace
    {
        scope = 2;
        displayName = "Zellno Safezone Campfire";
        descriptionShort = "Administrative permanent campfire for safezones.";
    };
};

class CfgMods
{
    class ZellnoSafezoneCampfire
    {
        dir = "ZellnoSafezoneCampfire";
        name = "Zellno Safezone Campfire";
        author = "Zellno";
        version = "0.1.0";
        type = "mod";

        dependencies[] =
        {
            "Game",
            "World",
            "Mission"
        };

        class defs
        {
            class gameScriptModule
            {
                value = "";
                files[] =
                {
                    "ZellnoSafezoneCampfire/scripts/3_Game"
                };
            };

            class worldScriptModule
            {
                value = "";
                files[] =
                {
                    "ZellnoSafezoneCampfire/scripts/4_World"
                };
            };

            class missionScriptModule
            {
                value = "";
                files[] =
                {
                    "ZellnoSafezoneCampfire/scripts/5_Mission"
                };
            };
        };
    };
};
