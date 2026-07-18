class Zellno_SafezoneCampfire : Fireplace
{
    protected bool m_ZSC_KeepBurning = true;
    protected bool m_ZSC_IgnoreWeather = true;
    protected bool m_ZSC_DisableFireDamage = true;
    protected bool m_ZSC_AllowExtinguish = false;
    protected bool m_ZSC_AllowZenFireSticksOnConcrete = true;
    protected bool m_ZSC_AllowAdministrativeFuel = false;

    void Zellno_SafezoneCampfire()
    {
        SetAllowDamage(false);
    }

    void ZSC_ApplySettings(
        bool keepBurning,
        bool ignoreWeather,
        bool disableFireDamage,
        bool allowExtinguish,
        bool allowZenFireSticksOnConcrete
    )
    {
        m_ZSC_KeepBurning = keepBurning;
        m_ZSC_IgnoreWeather = ignoreWeather;
        m_ZSC_DisableFireDamage = disableFireDamage;
        m_ZSC_AllowExtinguish = allowExtinguish;
        m_ZSC_AllowZenFireSticksOnConcrete = allowZenFireSticksOnConcrete;

        SetAllowDamage(false);

        if (m_ZSC_DisableFireDamage)
        {
            DestroyAreaDamage();
        }
        else if (IsBurning())
        {
            CreateAreaDamage();
        }
    }

    bool ZSC_AllowsZenFireSticksOnConcrete()
    {
        return m_ZSC_AllowZenFireSticksOnConcrete;
    }

    void ZSC_SetAdministrativeFuelMode(bool state)
    {
        m_ZSC_AllowAdministrativeFuel = state;
    }

    bool ZSC_IsKeepBurningEnabled()
    {
        return m_ZSC_KeepBurning;
    }

    void ZSC_DeleteSafely()
    {
        int attachmentIndex = GetInventory().AttachmentCount() - 1;

        while (attachmentIndex >= 0)
        {
            EntityAI attachment = GetInventory().GetAttachmentFromIndex(attachmentIndex);
            ItemBase item = ItemBase.Cast(attachment);

            if (item && (IsKindling(item) || IsFuel(item)))
            {
                GetGame().ObjectDelete(item);
            }

            attachmentIndex--;
        }

        MiscGameplayFunctions.DropAllItemsInInventoryInBounds(this, m_HalfExtents);
        GetGame().ObjectDelete(this);
    }

    override bool IsRoofAbove()
    {
        if (m_ZSC_IgnoreWeather)
        {
            return true;
        }

        return super.IsRoofAbove();
    }

    override protected void SpendFireConsumable(float amount)
    {
        if (m_ZSC_KeepBurning)
        {
            return;
        }

        super.SpendFireConsumable(amount);
    }

    override void CreateAreaDamage()
    {
        if (m_ZSC_DisableFireDamage)
        {
            DestroyAreaDamage();
            return;
        }

        super.CreateAreaDamage();
    }

    override bool CanExtinguishFire()
    {
        if (!m_ZSC_AllowExtinguish)
        {
            return false;
        }

        return super.CanExtinguishFire();
    }

    override bool CanPutIntoHands(EntityAI parent)
    {
        return false;
    }

    override bool CanPutInCargo(EntityAI parent)
    {
        return false;
    }

    override bool CanReceiveItemIntoCargo(EntityAI item)
    {
        return false;
    }

    override bool CanLoadItemIntoCargo(EntityAI item)
    {
        return false;
    }

    override bool IsPrepareToDelete()
    {
        return false;
    }

    override bool CanReceiveAttachment(EntityAI attachment, int slotId)
    {
        ItemBase item = ItemBase.Cast(attachment);
        if (!item)
        {
            return false;
        }

        string slotName = InventorySlots.GetSlotName(slotId);
        if (slotName == "Stones")
        {
            return false;
        }

        if (IsKindling(item) || IsFuel(item))
        {
            if (!m_ZSC_KeepBurning)
            {
                return super.CanReceiveAttachment(attachment, slotId);
            }

            return m_ZSC_AllowAdministrativeFuel;
        }

        return super.CanReceiveAttachment(attachment, slotId);
    }

    override bool CanReleaseAttachment(EntityAI attachment)
    {
        ItemBase item = ItemBase.Cast(attachment);
        if (!item)
        {
            return false;
        }

        if (IsKindling(item) || IsFuel(item))
        {
            if (!m_ZSC_KeepBurning)
            {
                return super.CanReleaseAttachment(attachment);
            }

            return false;
        }

        if (item.GetType() == "Stone")
        {
            return false;
        }

        return super.CanReleaseAttachment(attachment);
    }

    override bool CanBuildOven()
    {
        return false;
    }

    override bool CanBuildStoneCircle()
    {
        return false;
    }

    override bool CanDismantleOven()
    {
        return false;
    }

    override bool CanDismantleStoneCircle()
    {
        return false;
    }

    override void EOnTouch(IEntity other, int extra)
    {
    }

    override void EOnContact(IEntity other, Contact extra)
    {
    }
}
