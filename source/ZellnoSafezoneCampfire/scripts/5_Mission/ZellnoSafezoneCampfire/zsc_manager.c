class ZSCManager
{
    static const string APPLIED_CONFIG_PATH = "$profile:ZellnoSafezoneCampfire/AppliedConfig.json";
    static const float ADMIN_LIFETIME_SECONDS = 31536000.0;

    protected bool m_Running = false;

    void Start()
    {
        if (m_Running)
        {
            return;
        }

        ZSCSettings settings = GetZSCSettings();

        if (!settings.Load())
        {
            Print("[ZSC] Gerenciador não iniciado: a configuração precisa ser revisada.");
            return;
        }

        ReconcilePreviousConfiguration(settings);
        ReconcileCurrentConfiguration(settings);
        SaveAppliedConfiguration(settings);

        int intervalMilliseconds = settings.CheckIntervalSeconds * 1000;
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CheckCampfires, intervalMilliseconds, true);

        m_Running = true;

        Print("[ZSC] Gerenciador iniciado. Intervalo=" + settings.CheckIntervalSeconds.ToString() + "s.");
    }

    void Stop()
    {
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(CheckCampfires);
        m_Running = false;
    }

    protected void CheckCampfires()
    {
        if (!m_Running)
        {
            return;
        }

        ReconcileCurrentConfiguration(GetZSCSettings());
    }

    protected void ReconcilePreviousConfiguration(ZSCSettings current)
    {
        if (!FileExist(APPLIED_CONFIG_PATH))
        {
            return;
        }

        ZSCSettings previous = new ZSCSettings;
        JsonFileLoader<ZSCSettings>.JsonLoadFile(APPLIED_CONFIG_PATH, previous);

        if (!previous.Campfires)
        {
            Print("[ZSC] Snapshot anterior inválido; limpeza histórica ignorada.");
            return;
        }

        foreach (ZSCCampfireEntry oldEntry : previous.Campfires)
        {
            if (!oldEntry)
            {
                continue;
            }

            ZSCCampfireEntry currentEntry = FindEntryById(current, oldEntry.Id);
            bool mustRemove = false;

            if (!currentEntry || !currentEntry.Enabled)
            {
                mustRemove = true;
            }
            else if (vector.Distance(oldEntry.Position, currentEntry.Position) > 0.05)
            {
                mustRemove = true;
            }

            if (mustRemove)
            {
                RemoveCampfiresNear(oldEntry.Position, oldEntry.MatchRadius, "configuração anterior: " + oldEntry.Id);
            }
        }
    }

    protected void ReconcileCurrentConfiguration(ZSCSettings settings)
    {
        foreach (ZSCCampfireEntry entry : settings.Campfires)
        {
            if (!entry)
            {
                continue;
            }

            if (!entry.Enabled)
            {
                RemoveCampfiresNear(entry.Position, entry.MatchRadius, "entrada desativada: " + entry.Id);
                continue;
            }

            EnsureCampfire(entry);
        }
    }

    protected void EnsureCampfire(ZSCCampfireEntry entry)
    {
        ref array<Zellno_SafezoneCampfire> found = new array<Zellno_SafezoneCampfire>;
        FindCampfiresNear(entry.Position, entry.MatchRadius, found);

        Zellno_SafezoneCampfire campfire;

        if (found.Count() == 0)
        {
            campfire = SpawnCampfire(entry);

            if (!campfire)
            {
                Print("[ZSC] Falha ao criar fogueira: " + entry.Id);
                return;
            }
        }
        else
        {
            campfire = FindNearest(found, entry.Position);

            foreach (Zellno_SafezoneCampfire duplicate : found)
            {
                if (duplicate && duplicate != campfire)
                {
                    Print("[ZSC] Duplicata removida em " + entry.Id);
                    duplicate.ZSC_DeleteSafely();
                }
            }
        }

        ApplyEntry(campfire, entry);
    }

    protected Zellno_SafezoneCampfire SpawnCampfire(ZSCCampfireEntry entry)
    {
        int creationFlags = ECE_CREATEPHYSICS | ECE_UPDATEPATHGRAPH;
        Object createdObject = GetGame().CreateObjectEx("Zellno_SafezoneCampfire", entry.Position, creationFlags);
        Zellno_SafezoneCampfire campfire = Zellno_SafezoneCampfire.Cast(createdObject);

        if (!campfire)
        {
            return null;
        }

        campfire.SetPosition(entry.Position);
        campfire.SetOrientation(entry.Orientation);

        Print("[ZSC] Fogueira criada: " + entry.Id + " em " + entry.Position.ToString());

        return campfire;
    }

    protected void ApplyEntry(Zellno_SafezoneCampfire campfire, ZSCCampfireEntry entry)
    {
        if (!campfire)
        {
            return;
        }

        campfire.ZSC_ApplySettings(entry.KeepBurning, entry.IgnoreWeather, entry.DisableFireDamage, entry.AllowExtinguish, entry.AllowZenFireSticksOnConcrete);
        campfire.SetOrientation(entry.Orientation);
        campfire.SetLifetimeMax(ADMIN_LIFETIME_SECONDS);
        campfire.SetLifetime(ADMIN_LIFETIME_SECONDS);

        EntityAI fuel;

        if (entry.KeepBurning)
        {
            fuel = campfire.FindAttachmentBySlotName("Firewood");

            if (!fuel)
            {
                campfire.ZSC_SetAdministrativeFuelMode(true);
                fuel = campfire.GetInventory().CreateAttachment("Firewood");
                campfire.ZSC_SetAdministrativeFuelMode(false);

                if (!fuel)
                {
                    Print("[ZSC] Não foi possível instalar combustível em " + entry.Id);
                    return;
                }

                Print("[ZSC] Combustível administrativo instalado: " + entry.Id);
            }
        }

        if (entry.IgnoreWeather)
        {
            campfire.SetWet(0);
        }

        if (entry.KeepBurning && !campfire.IsBurning())
        {
            campfire.StartFire(true);
            Print("[ZSC] Fogueira acesa: " + entry.Id);
        }
    }

    protected void FindCampfiresNear(vector position, float radius, notnull array<Zellno_SafezoneCampfire> result)
    {
        ref array<Object> objects = new array<Object>;
        GetGame().GetObjectsAtPosition3D(position, radius, objects, NULL);

        foreach (Object candidate : objects)
        {
            Zellno_SafezoneCampfire campfire = Zellno_SafezoneCampfire.Cast(candidate);

            if (campfire)
            {
                result.Insert(campfire);
            }
        }
    }

    protected Zellno_SafezoneCampfire FindNearest(notnull array<Zellno_SafezoneCampfire> campfires, vector position)
    {
        Zellno_SafezoneCampfire nearest;
        float nearestDistance = 1000000.0;

        foreach (Zellno_SafezoneCampfire campfire : campfires)
        {
            if (!campfire)
            {
                continue;
            }

            float distance = vector.Distance(campfire.GetPosition(), position);

            if (distance < nearestDistance)
            {
                nearest = campfire;
                nearestDistance = distance;
            }
        }

        return nearest;
    }

    protected void RemoveCampfiresNear(vector position, float radius, string reason)
    {
        ref array<Zellno_SafezoneCampfire> found = new array<Zellno_SafezoneCampfire>;
        FindCampfiresNear(position, radius, found);

        foreach (Zellno_SafezoneCampfire campfire : found)
        {
            if (!campfire)
            {
                continue;
            }

            Print("[ZSC] Removendo fogueira gerenciada: " + reason);
            campfire.ZSC_DeleteSafely();
        }
    }

    protected ZSCCampfireEntry FindEntryById(ZSCSettings settings, string id)
    {
        if (!settings || !settings.Campfires)
        {
            return null;
        }

        foreach (ZSCCampfireEntry entry : settings.Campfires)
        {
            if (entry && entry.Id == id)
            {
                return entry;
            }
        }

        return null;
    }

    protected void SaveAppliedConfiguration(ZSCSettings settings)
    {
        JsonFileLoader<ZSCSettings>.JsonSaveFile(APPLIED_CONFIG_PATH, settings);
    }
}

static ref ZSCManager g_ZSCManager;

ZSCManager GetZSCManager()
{
    if (!g_ZSCManager)
    {
        g_ZSCManager = new ZSCManager;
    }

    return g_ZSCManager;
}
