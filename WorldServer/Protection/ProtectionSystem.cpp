#include "ProtectionSystem.h"
#include "../Entities/CLogFile.h"
#include "../Entities/CPlayer.h"

// ============================================================================
// ProtectionSystem.cpp - Main Protection System Implementation
// ============================================================================

bool ProtectionSystem::Initialize(const char* configDir)
{
    // Create main log
    static CLogFile s_mainLog;
    s_mainLog.SetWriteLogFile("Protection_System", 1, false, true, true);
    m_pLog = &s_mainLog;

    m_pLog->Write("========================================");
    m_pLog->Write("  RF-Online Protection System v2.0");
    m_pLog->Write("  6.23_Guard 2 + Extended Fixes");
    m_pLog->Write("========================================");

    // 1. Load all config files
    m_pLog->Write("[Init] Loading configs from: %s", configDir);
    if (!ProtectConfig::Instance().LoadAll(configDir))
    {
        m_pLog->Write("[Init] WARNING: No config files found in %s, using defaults", configDir);
    }

    // 2. Initialize 6.23_Guard 2 subsystems
    m_pLog->Write("[Init] Initializing AntiHack...");
    AntiHack::Instance().Initialize();

    m_pLog->Write("[Init] Initializing AntiDupe...");
    AntiDupe::Instance().Initialize();

    m_pLog->Write("[Init] Initializing GameFixes...");
    GameFixes::Instance().Initialize();

    // 3. Initialize extended subsystems (missing fixes)
    m_pLog->Write("[Init] Initializing InputSanitizer...");
    InputSanitizer::Instance().Initialize();

    m_pLog->Write("[Init] Initializing ItemIntegrity...");
    ItemIntegrity::Instance().Initialize();

    m_pLog->Write("[Init] Initializing CombatIntegrity...");
    CombatIntegrity::Instance().Initialize();

    m_pLog->Write("[Init] Initializing AntiCheat...");
    AntiCheat::Instance().Initialize();

    m_initialized = true;
    m_pLog->Write("[Init] Protection System READY - 7 modules loaded");
    m_pLog->Write("========================================");

    return true;
}

bool ProtectionSystem::ReloadConfigs()
{
    if (!m_initialized) return false;

    if (m_pLog)
        m_pLog->Write("[Reload] Reloading all protection configs...");

    ProtectConfig::Instance().ReloadAll();

    // 6.23_Guard 2 modules
    AntiHack::Instance().Reload();
    AntiDupe::Instance().Reload();
    GameFixes::Instance().Reload();

    // Extended modules
    InputSanitizer::Instance().Reload();
    ItemIntegrity::Instance().Reload();
    CombatIntegrity::Instance().Reload();
    AntiCheat::Instance().Reload();

    if (m_pLog)
        m_pLog->Write("[Reload] All 7 module configs reloaded successfully");

    return true;
}

void ProtectionSystem::OnPlayerEnterWorld(CPlayer* pPlayer)
{
    if (!m_initialized || !pPlayer) return;

    unsigned int serial = pPlayer->m_dwSerial;
    AntiHack::Instance().OnPlayerEnterWorld(serial);
    AntiDupe::Instance().OnPlayerEnterWorld(serial);
    AntiCheat::Instance().OnPlayerEnterWorld(serial);
}

void ProtectionSystem::OnPlayerLeaveWorld(CPlayer* pPlayer)
{
    if (!m_initialized || !pPlayer) return;

    unsigned int serial = pPlayer->m_dwSerial;
    AntiHack::Instance().OnPlayerLeaveWorld(serial);
    AntiDupe::Instance().OnPlayerLeaveWorld(serial);
    AntiCheat::Instance().OnPlayerLeaveWorld(serial);
}

void ProtectionSystem::OnTick()
{
    if (!m_initialized) return;

    // Run periodic anti-cheat integrity checks + score decay
    AntiCheat::Instance().RunIntegrityCheck();
}
