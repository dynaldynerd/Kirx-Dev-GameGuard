#pragma once
// ============================================================================
// ProtectionSystem.h - Main Protection System Manager
// Ties together: ProtectConfig, AntiHack, AntiDupe, GameFixes,
//                InputSanitizer, ItemIntegrity, CombatIntegrity, AntiCheat
// ============================================================================

#include "ProtectConfig.h"
#include "AntiHack.h"
#include "AntiDupe.h"
#include "GameFixes.h"
#include "InputSanitizer.h"
#include "ItemIntegrity.h"
#include "CombatIntegrity.h"
#include "AntiCheat.h"

class CLogFile;

class ProtectionSystem
{
public:
    static ProtectionSystem& Instance()
    {
        static ProtectionSystem inst;
        return inst;
    }

    // Call once during CMainThread initialization
    // configDir = path to PROTECT/Configs/ folder
    bool Initialize(const char* configDir);

    // Hot-reload all configs (can be called from GM command)
    bool ReloadConfigs();

    // Player lifecycle events (call from CMainThread)
    void OnPlayerEnterWorld(CPlayer* pPlayer);
    void OnPlayerLeaveWorld(CPlayer* pPlayer);

    // Call from CMainThread tick loop (periodic anti-cheat checks)
    void OnTick();

    // Quick access to subsystems
    ProtectConfig&    Config()    { return ProtectConfig::Instance(); }
    AntiHack&         Hack()      { return AntiHack::Instance(); }
    AntiDupe&         Dupe()      { return AntiDupe::Instance(); }
    GameFixes&        Fixes()     { return GameFixes::Instance(); }
    InputSanitizer&   Sanitizer() { return InputSanitizer::Instance(); }
    ItemIntegrity&    Items()     { return ItemIntegrity::Instance(); }
    CombatIntegrity&  Combat()    { return CombatIntegrity::Instance(); }
    AntiCheat&        Cheat()     { return AntiCheat::Instance(); }

    bool IsInitialized() const { return m_initialized; }

private:
    ProtectionSystem() : m_initialized(false), m_pLog(nullptr) {}
    ~ProtectionSystem() {}

    bool m_initialized;
    CLogFile* m_pLog;
};
