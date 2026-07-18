modded class MissionServer
{
    void MissionServer()
    {
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ZSC_StartManager, 1000, false);
    }

    void ~MissionServer()
    {
        if (g_ZSCManager)
        {
            g_ZSCManager.Stop();
        }
    }

    protected void ZSC_StartManager()
    {
        GetZSCManager().Start();
    }
}
