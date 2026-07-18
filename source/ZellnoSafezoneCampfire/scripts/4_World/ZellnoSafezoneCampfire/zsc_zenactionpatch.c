modded class ActionZenAttachStickToFire
{
    override bool ActionCondition(
        PlayerBase player,
        ActionTarget target,
        ItemBase item
    )
    {
        if (super.ActionCondition(player, target, item))
        {
            return true;
        }

        #ifdef ZENMODPACK
        if (!ZenModEnabled("ZenFireplaceStick"))
        {
            return false;
        }
        #endif

        if (!player || !target || !target.GetObject())
        {
            return false;
        }

        Zellno_SafezoneCampfire campfire =
            Zellno_SafezoneCampfire.Cast(target.GetObject());

        if (!campfire)
        {
            return false;
        }

        if (!campfire.ZSC_AllowsZenFireSticksOnConcrete())
        {
            return false;
        }

        if (!campfire.IsBaseFireplace())
        {
            return false;
        }

        if (campfire.IsOven() || campfire.IsBarrelWithHoles())
        {
            return false;
        }

        vector stickOrientation;
        vector stickPosition = GetZenAttachedProjectedStickPos(player, campfire, stickOrientation);

        if (GetGame().IsDedicatedServer())
        {
            return true;
        }

        return CanAddZenAttachedStick(campfire.GetPosition(), stickPosition);
    }
}
