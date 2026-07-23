#include "CombatIntegrity.h"
#include "ProtectConfig.h"
#include "../Entities/CLogFile.h"
#include "../Entities/CPlayer.h"

#include <cmath>

// ============================================================================
// CombatIntegrity.cpp - Combat & Entity Integrity Implementation
// ============================================================================

void CombatIntegrity::Initialize()
{
    static CLogFile s_log;
    s_log.SetWriteLogFile("Protection_CombatIntegrity", 1, false, true, true);
    m_pLog = &s_log;

    Reload();
    m_pLog->Write("[CombatIntegrity] Initialized");
}

void CombatIntegrity::Reload()
{
    auto& cfg = ProtectConfig::Instance();

    // Unit delivery (from Player.ini)
    m_config.unitDeliveryDelayMs = cfg.GetInt("player.ini", "unit", "delay_delivery", 2500);
    m_config.unitDeliveryMinLevel = cfg.GetInt("player.ini", "unit", "min_delivery_level", 30);
    m_config.unitDeliveryMaxDist = cfg.GetFloat("player.ini", "unit", "max_delivery_dist", 30.0f);

    // Tower (from Tower.ini)
    m_config.towerMinLevel = cfg.GetInt("tower.ini", "license", "guardtowerlv0", 35);
    m_config.trapMinLevel = cfg.GetInt("system.ini", "trap", "min_level", 30);

    // Combat ranges
    m_config.meleeMaxRange  = cfg.GetFloat("attacksystem.ini", "combatrange", "melee", 15.0f);
    m_config.rangedMaxRange = cfg.GetFloat("attacksystem.ini", "combatrange", "ranged", 80.0f);
    m_config.forceMaxRange  = cfg.GetFloat("attacksystem.ini", "combatrange", "force", 60.0f);
    m_config.siegeMaxRange  = cfg.GetFloat("attacksystem.ini", "combatrange", "siege", 100.0f);
    m_config.aoeMaxRange    = cfg.GetFloat("attacksystem.ini", "combatrange", "aoe", 40.0f);

    if (m_pLog)
        m_pLog->Write("[CombatIntegrity] Config reloaded - UnitDelay=%dms TowerLv=%d",
            m_config.unitDeliveryDelayMs, m_config.towerMinLevel);
}

// ---- Unit Delivery Validation ----

bool CombatIntegrity::ValidateUnitDelivery(CPlayer* pPlayer, float deliverX,
                                            float deliverY, float deliverZ)
{
    if (!pPlayer || !m_config.validateUnitDelivery) return true;

    // Level check
    if (pPlayer->m_nLevel < m_config.unitDeliveryMinLevel)
    {
        if (m_pLog)
            m_pLog->Write("[UnitDelivery] BLOCKED Serial=%u level=%d < min=%d",
                pPlayer->m_dwSerial, pPlayer->m_nLevel, m_config.unitDeliveryMinLevel);
        return false;
    }

    // Distance check (WH prevention - player trying to deliver from impossible position)
    float dist = Distance3D(
        pPlayer->m_fPosX, pPlayer->m_fPosY, pPlayer->m_fPosZ,
        deliverX, deliverY, deliverZ
    );

    if (dist > m_config.unitDeliveryMaxDist)
    {
        if (m_pLog)
            m_pLog->Write("[UnitDelivery] BLOCKED Serial=%u dist=%.2f > max=%.2f (WH suspected)",
                pPlayer->m_dwSerial, dist, m_config.unitDeliveryMaxDist);
        return false;
    }

    // Height check (FH prevention - delivering to impossible height)
    float heightDiff = std::abs(deliverY - pPlayer->m_fPosY);
    if (heightDiff > 20.0f) // reasonable height difference
    {
        if (m_pLog)
            m_pLog->Write("[UnitDelivery] BLOCKED Serial=%u height diff=%.2f (FH suspected)",
                pPlayer->m_dwSerial, heightDiff);
        return false;
    }

    // Cooldown check
    if (!CheckUnitDeliveryCooldown(pPlayer->m_dwSerial))
    {
        if (m_pLog)
            m_pLog->Write("[UnitDelivery] BLOCKED Serial=%u cooldown not expired",
                pPlayer->m_dwSerial);
        return false;
    }

    RecordUnitDelivery(pPlayer->m_dwSerial);
    return true;
}

bool CombatIntegrity::CheckUnitDeliveryCooldown(unsigned int playerSerial)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_unitDeliveryTimes.find(playerSerial);
    if (it == m_unitDeliveryTimes.end()) return true;

    unsigned int now = GetTickCount();
    return (now - it->second) >= static_cast<unsigned int>(m_config.unitDeliveryDelayMs);
}

void CombatIntegrity::RecordUnitDelivery(unsigned int playerSerial)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_unitDeliveryTimes[playerSerial] = GetTickCount();
}

// ---- Tower Creation Validation ----

bool CombatIntegrity::ValidateTowerCreate(CPlayer* pPlayer, float posX, float posY,
                                           float posZ, int mapIndex, bool isMapLoading)
{
    if (!pPlayer || !m_config.validateTowerCreate) return true;

    // Map loading check
    if (m_config.towerCheckMapLoading && isMapLoading)
    {
        if (m_pLog)
            m_pLog->Write("[Tower] BLOCKED Serial=%u tower during map loading",
                pPlayer->m_dwSerial);
        return false;
    }

    // Level check
    if (pPlayer->m_nLevel < m_config.towerMinLevel)
    {
        if (m_pLog)
            m_pLog->Write("[Tower] BLOCKED Serial=%u level=%d < min=%d",
                pPlayer->m_dwSerial, pPlayer->m_nLevel, m_config.towerMinLevel);
        return false;
    }

    // Position integrity (WH check)
    float dist = Distance3D(pPlayer->m_fPosX, pPlayer->m_fPosY, pPlayer->m_fPosZ,
                             posX, posY, posZ);
    if (dist > 25.0f)
    {
        if (m_pLog)
            m_pLog->Write("[Tower] BLOCKED Serial=%u build dist=%.2f (WH suspected)",
                pPlayer->m_dwSerial, dist);
        return false;
    }

    // Height check (FH)
    float heightDiff = std::abs(posY - pPlayer->m_fPosY);
    if (heightDiff > 15.0f)
    {
        if (m_pLog)
            m_pLog->Write("[Tower] BLOCKED Serial=%u height diff=%.2f (FH suspected)",
                pPlayer->m_dwSerial, heightDiff);
        return false;
    }

    // Position validity on map
    if (!IsValidPosition(mapIndex, posX, posY, posZ))
    {
        if (m_pLog)
            m_pLog->Write("[Tower] BLOCKED Serial=%u invalid position on map=%d",
                pPlayer->m_dwSerial, mapIndex);
        return false;
    }

    return true;
}

// ---- Trap Creation Validation ----

bool CombatIntegrity::ValidateTrapCreate(CPlayer* pPlayer, float posX, float posY,
                                          float posZ, int mapIndex, bool isMapLoading)
{
    if (!pPlayer || !m_config.validateTrapCreate) return true;

    // Map loading check
    if (m_config.trapCheckMapLoading && isMapLoading)
    {
        if (m_pLog)
            m_pLog->Write("[Trap] BLOCKED Serial=%u trap during map loading",
                pPlayer->m_dwSerial);
        return false;
    }

    // Level check
    if (pPlayer->m_nLevel < m_config.trapMinLevel)
    {
        if (m_pLog)
            m_pLog->Write("[Trap] BLOCKED Serial=%u level=%d < min=%d",
                pPlayer->m_dwSerial, pPlayer->m_nLevel, m_config.trapMinLevel);
        return false;
    }

    // Position integrity (same as tower)
    float dist = Distance3D(pPlayer->m_fPosX, pPlayer->m_fPosY, pPlayer->m_fPosZ,
                             posX, posY, posZ);
    if (dist > 20.0f)
    {
        if (m_pLog)
            m_pLog->Write("[Trap] BLOCKED Serial=%u build dist=%.2f (WH suspected)",
                pPlayer->m_dwSerial, dist);
        return false;
    }

    float heightDiff = std::abs(posY - pPlayer->m_fPosY);
    if (heightDiff > 10.0f)
    {
        if (m_pLog)
            m_pLog->Write("[Trap] BLOCKED Serial=%u height diff=%.2f (FH suspected)",
                pPlayer->m_dwSerial, heightDiff);
        return false;
    }

    return true;
}

// ---- Teleport Validation ----

bool CombatIntegrity::CanPlayerTeleport(CPlayer* pPlayer, bool isBattleMode, bool isChipWar)
{
    if (!pPlayer) return false;

    if (m_config.blockTeleportInBattle && isBattleMode)
    {
        if (m_pLog)
            m_pLog->Write("[Teleport] BLOCKED Serial=%u in battle mode",
                pPlayer->m_dwSerial);
        return false;
    }

    if (m_config.blockTeleportDuringCW && isChipWar)
    {
        if (m_pLog)
            m_pLog->Write("[Teleport] BLOCKED Serial=%u during chip war",
                pPlayer->m_dwSerial);
        return false;
    }

    return true;
}

// ---- Combat Range Validation ----

bool CombatIntegrity::ValidateCombatRange(CPlayer* pPlayer, float targetX,
                                           float targetY, float targetZ,
                                           WeaponType weaponType)
{
    if (!pPlayer) return false;

    float dist = Distance3D(
        pPlayer->m_fPosX, pPlayer->m_fPosY, pPlayer->m_fPosZ,
        targetX, targetY, targetZ
    );

    float maxRange = 0.0f;
    const char* typeName = "unknown";

    switch (weaponType)
    {
    case MELEE:  maxRange = m_config.meleeMaxRange;  typeName = "melee";  break;
    case RANGED: maxRange = m_config.rangedMaxRange; typeName = "ranged"; break;
    case FORCE:  maxRange = m_config.forceMaxRange;  typeName = "force";  break;
    case SIEGE:  maxRange = m_config.siegeMaxRange;  typeName = "siege";  break;
    case AOE:    maxRange = m_config.aoeMaxRange;    typeName = "aoe";    break;
    }

    if (dist > maxRange)
    {
        if (m_pLog)
            m_pLog->Write("[CombatRange] BLOCKED Serial=%u %s dist=%.2f > max=%.2f",
                pPlayer->m_dwSerial, typeName, dist, maxRange);
        return false;
    }
    return true;
}

// ---- Position Integrity ----

bool CombatIntegrity::IsValidPosition(int mapIndex, float posX, float posY, float posZ)
{
    // Basic boundary check per map
    // RF Online maps typically have bounds roughly -5000 to 5000 on X/Z
    // and height within reasonable terrain bounds
    //
    // Integration: Use CMapData to get actual map boundaries
    //   CMapData* pMap = g_Main.GetMapData(mapIndex);
    //   if (pMap) return pMap->IsInBounds(posX, posY, posZ);

    // Fallback: generic bounds check
    const float MAP_BOUND = 10000.0f;
    const float MIN_HEIGHT = -500.0f;
    const float MAX_HEIGHT = 2000.0f;

    if (std::abs(posX) > MAP_BOUND || std::abs(posZ) > MAP_BOUND)
        return false;
    if (posY < MIN_HEIGHT || posY > MAX_HEIGHT)
        return false;

    return true;
}

bool CombatIntegrity::CheckPositionIntegrity(CPlayer* pPlayer, float newX, float newY, float newZ)
{
    if (!pPlayer) return false;

    // Check for impossible position change (teleport/noclip detection)
    // This is more aggressive than AntiHack speed check - it catches
    // instant position changes that bypass movement packets entirely

    float dist = Distance3D(pPlayer->m_fPosX, pPlayer->m_fPosY, pPlayer->m_fPosZ,
                             newX, newY, newZ);

    // If position changed more than 200 units in a single packet,
    // it's likely a hack (normal teleport goes through server)
    const float MAX_SINGLE_MOVE = 200.0f;
    if (dist > MAX_SINGLE_MOVE)
    {
        if (m_pLog)
            m_pLog->Write("[Position] BLOCKED Serial=%u impossible move dist=%.2f",
                pPlayer->m_dwSerial, dist);
        return false;
    }

    return true;
}

// ---- Helpers ----

float CombatIntegrity::Distance3D(float x1, float y1, float z1,
                                   float x2, float y2, float z2) const
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float dz = z2 - z1;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}
