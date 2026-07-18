class ZSCCampfireEntry
{
    string Id = "prison_black_market";
    string Name = "Prison Black Market";
    bool Enabled = true;

    vector Position = "0 0 0";
    vector Orientation = "0 0 0";

    float MatchRadius = 1.0;

    bool KeepBurning = true;
    bool IgnoreWeather = true;
    bool DisableFireDamage = true;
    bool AllowExtinguish = false;
    bool AllowZenFireSticksOnConcrete = true;
}

class ZSCSettings
{
    static const int CURRENT_CONFIG_VERSION = 1;

    static const string DIRECTORY =
        "$profile:ZellnoSafezoneCampfire";

    static const string FILE_PATH =
        "$profile:ZellnoSafezoneCampfire/Config.json";

    static const string INVALID_FILE_PATH =
        "$profile:ZellnoSafezoneCampfire/Config.invalid.json";

    int ConfigVersion = CURRENT_CONFIG_VERSION;
    bool DebugEnabled = false;
    int CheckIntervalSeconds = 60;

    ref array<ref ZSCCampfireEntry> Campfires;

    void ZSCSettings()
    {
        Campfires = new array<ref ZSCCampfireEntry>;
    }

    void SetDefaults()
    {
        ConfigVersion = CURRENT_CONFIG_VERSION;
        DebugEnabled = false;
        CheckIntervalSeconds = 60;

        if (!Campfires)
        {
            Campfires = new array<ref ZSCCampfireEntry>;
        }
        else
        {
            Campfires.Clear();
        }

        Campfires.Insert(new ZSCCampfireEntry);
    }

    bool Load()
    {
        MakeDirectory(DIRECTORY);

        if (!FileExist(FILE_PATH))
        {
            SetDefaults();
            Save();

            Print("[ZSC] Configuração padrão criada. Defina uma posição válida antes de iniciar as fogueiras.");

            return false;
        }

        JsonFileLoader<ZSCSettings>.JsonLoadFile(FILE_PATH, this);

        string validationError;
        if (Validate(validationError))
        {
            Print("[ZSC] Configuração carregada e validada.");
            return true;
        }

        Print("[ZSC] Configuração inválida: " + validationError);

        if (FileExist(INVALID_FILE_PATH))
        {
            DeleteFile(INVALID_FILE_PATH);
        }

        CopyFile(FILE_PATH, INVALID_FILE_PATH);

        SetDefaults();
        Save();

        Print("[ZSC] Arquivo inválido preservado em: " + INVALID_FILE_PATH);

        return false;
    }

    void Save()
    {
        MakeDirectory(DIRECTORY);

        JsonFileLoader<ZSCSettings>.JsonSaveFile(FILE_PATH, this);
    }

    bool Validate(out string error)
    {
        if (ConfigVersion != CURRENT_CONFIG_VERSION)
        {
            error = "ConfigVersion incompatível. Esperado=" + CURRENT_CONFIG_VERSION.ToString() + " recebido=" + ConfigVersion.ToString();
            return false;
        }

        if (CheckIntervalSeconds < 10 || CheckIntervalSeconds > 3600)
        {
            error = "CheckIntervalSeconds deve estar entre 10 e 3600.";
            return false;
        }

        if (!Campfires)
        {
            error = "Campfires não pode ser nulo.";
            return false;
        }

        ref array<string> ids = new array<string>;

        foreach (ZSCCampfireEntry entry : Campfires)
        {
            if (!entry)
            {
                error = "Campfires contém uma entrada nula.";
                return false;
            }

            if (entry.Id == "")
            {
                error = "Toda fogueira precisa de Id.";
                return false;
            }

            if (ids.Find(entry.Id) != -1)
            {
                error = "Id duplicado: " + entry.Id;
                return false;
            }

            ids.Insert(entry.Id);

            if (entry.Name == "")
            {
                error = "A fogueira " + entry.Id + " precisa de Name.";
                return false;
            }

            if (entry.MatchRadius < 0.25 || entry.MatchRadius > 25.0)
            {
                error = "MatchRadius inválido para " + entry.Id;
                return false;
            }

            if (entry.Enabled && entry.Position == vector.Zero)
            {
                error = "Posição ainda não definida para " + entry.Id;
                return false;
            }
        }

        for (int i = 0; i < Campfires.Count(); i++)
        {
            ZSCCampfireEntry first = Campfires.Get(i);

            if (!first || !first.Enabled)
            {
                continue;
            }

            for (int j = i + 1; j < Campfires.Count(); j++)
            {
                ZSCCampfireEntry second = Campfires.Get(j);

                if (!second || !second.Enabled)
                {
                    continue;
                }

                float minimumDistance = first.MatchRadius + second.MatchRadius;

                if (vector.Distance(first.Position, second.Position) <= minimumDistance)
                {
                    error = "Áreas sobrepostas: " + first.Id + " e " + second.Id;
                    return false;
                }
            }
        }

        error = "";
        return true;
    }
}

static ref ZSCSettings g_ZSCSettings;

ZSCSettings GetZSCSettings()
{
    if (!g_ZSCSettings)
    {
        g_ZSCSettings = new ZSCSettings;
    }

    return g_ZSCSettings;
}
