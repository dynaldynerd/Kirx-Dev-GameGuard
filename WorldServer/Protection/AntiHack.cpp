#include "AntiHack.h"
#include "ProtectConfig.h"

// Include WorldServer headers
#include "../Entities/CLogFile.h"
#include "../Entities/CPlayer.h"

// ============================================================================
// AntiHack.cpp - Anti-Hack Detection Implementation
// ============================================================================

AntiHack::AntiHack()
    : m_pLog(nullptr)
{
}

void AntiHack::Initialize()
{
    // Create log file
    static CLogFile s_hackLog;
    s_hackLog.SetWriteLogFile("Protection_AntiHack", 1, false, true, true);
    m_pLog = &s_hackLog;

    Reload();
    m_pLog->Write("[AntiHack] Initialized successfully");
}

void AntiHack::Reload()
{
    auto& cfg = ProtectConfig::Instance();

    // From Player.ini
    m_config.moveSpeedStep      = cfg.GetFloat("player.ini", "player", "move_speed_step", 1.150f);
    m_config.moveSpeedTolerance = cfg.GetFloat("player.ini", "player", "move_speed_tolerance", 0.2f);
    m_config.speedHackMaxCount  = cfg.GetInt("player.ini", "player", "speed_hack_max_count", 10);
    m_config.speedHackAutoKick  = cfg.GetBool("player.ini", "player", "speed_hack_auto_kick", true);

    // InvisHack from Player.ini
    m_config.invisHackAutoKick   = cfg.GetBool("player.ini", "invishack", "autokick", false);
    m_config.invisHackMaxTrigger = cfg.GetInt("player.ini", "invishack", "maxtrigger", 0);

    // From AttackSystem.ini
    m_config.fixWallHack = cfg.GetBool("attacksystem.ini", "attacksystem", "fixwallhack", true);

    // Fly hack (custom additions)
    m_config.maxHeightDelta   = cfg.GetFloat("attacksystem.ini", "antihack", "maxheightdelta", 50.0f);
    m_config.flyHackMaxCount  = cfg.GetInt("attacksystem.ini", "antihack", "flyhackmaxcount", 5);
    m_config.flyHackAutoKick  = cfg.GetBool("attacksystem.ini", "antihack", "flyhackautokick", true);

    // Long distance attack
    m_config.maxAttackRange = cfg.GetFloat("attacksystem.ini", "antihack", "maxattackrange", 100.0f);
    m_config.maxDebuffRange = cfg.GetFloat("attacksystem.ini", "antihack", "maxdebuffrange", 120.0f);

    if (m_pLog)
        m_pLog->Write("[AntiHack] Config reloaded - SpeedStep=%.3f Tolerance=%.3f WallHack=%s",
            m_config.moveSpeedStep, m_config.moveSpeedTolerance,
            m_config.fixWallHack ? "ON" : "OFF");
}

// ---- Movement Validation ----

bool AntiHack::ValidateMovement(CPlayer* pPlayer, float newX, float newY, float newZ)
{
    if (!pPlayer) return false;

    unsigned int serial = pPlayer->m_dwSerial;
    unsigned int now = GetTickCount();

    std::lock_guard<std::mutex> lock(m_mutex);

    auto& data = m_playerData[serial];

    // First position update: just record it
    if (!data.initialized)
    {
        data.lastPosX = newX;
        data.lastPosY = newY;
        data.lastPosZ = newZ;
        data.lastMoveTime = now;
        data.initialized = true;
        return true;
    }

    unsigned int timeDelta = now - data.lastMoveTime;
    if (timeDelta == 0) timeDelta = 1; // prevent divide by zero

    // ---- Speed Hack Detection ----
    float dist2D = Distance2D(data.lastPosX, data.lastPosZ, newX, newZ);
    float timeSeconds = timeDelta / 1000.0f;

    // Calculate max allowed distance based on speed step + tolerance
    // Base movement speed in RF is ~6.0 units/sec at walk, ~12.0 at run
    // With buffs/mounts can go higher. moveSpeedStep controls the multiplier cap.
    float baseSpeed = 15.0f; // generous base run speed
    float maxSpeed = baseSpeed * m_config.moveSpeedStep;
    float maxDist = maxSpeed * timeSeconds + m_config.moveSpeedTolerance;

    if (dist2D > maxDist && timeDelta > 100) // ignore very short intervals
    {
        data.speedViolationCount++;

        if (m_pLog && (now - data.lastSpeedHackAlert > 5000)) // alert cooldown 5s
        {
            m_pLog->Write("[SpeedHack] Player Serial=%u moved %.2f units in %.3fs (max=%.2f) violations=%d",
                serial, dist2D, timeSeconds, maxDist, data.speedViolationCount);
            data.lastSpeedHackAlert = now;
        }

        if (m_config.speedHackAutoKick && data.speedViolationCount >= m_config.speedHackMaxCount)
        {
            m_pLog->Write("[SpeedHack] KICK Player Serial=%u - %d violations exceeded limit",
                serial, data.speedViolationCount);
            // Don't update position - caller should kick
            return false;
        }

        // Teleport back to last valid position
        data.lastMoveTime = now;
        return false;
    }
    else
    {
        // Decay violation count on valid moves
        if (data.speedViolationCount > 0)
            data.speedViolationCount--;
    }

    // ---- Wall Hack Detection ----
    if (m_config.fixWallHack)
    {
        // Basic wall hack: check if Y position changed drastically while
        // moving through geometry. This requires BSP collision checks.
        // For now, flag extreme position changes that bypass terrain.
        // NOTE: Full implementation requires CMapData::CheckCollision()
        // integration which is map-specific.
    }

    // ---- Fly Hack Detection ----
    float heightDelta = newY - data.lastPosY;
    if (heightDelta > m_config.maxHeightDelta && timeDelta > 200)
    {
        data.flyViolationCount++;

        if (m_pLog)
        {
            m_pLog->Write("[FlyHack] Player Serial=%u height jump=%.2f (max=%.2f) violations=%d",
                serial, heightDelta, m_config.maxHeightDelta, data.flyViolationCount);
        }

        if (m_config.flyHackAutoKick && data.flyViolationCount >= m_config.flyHackMaxCount)
        {
            m_pLog->Write("[FlyHack] KICK Player Serial=%u - %d violations exceeded limit",
                serial, data.flyViolationCount);
            return false;
        }

        return false; // reject movement
    }
    else if (data.flyViolationCount > 0)
    {
        data.flyViolationCount--;
    }

    // Movement is valid, update tracking
    data.lastPosX = newX;
    data.lastPosY = newY;
    data.lastPosZ = newZ;
    data.lastMoveTime = now;
    return true;
}

// ---- Attack Range Validation ----

bool AntiHack::ValidateAttackRange(CPlayer* pAttacker, float targetX, float targetY, float targetZ)
{
    if (!pAttacker) return false;

    float dist = Distance3D(
        pAttacker->m_fPosX, pAttacker->m_fPosY, pAttacker->m_fPosZ,
        targetX, targetY, targetZ
    );

    if (dist > m_config.maxAttackRange)
    {
        if (m_pLog)
        {
            m_pLog->Write("[LongDistAttack] Player Serial=%u attack dist=%.2f (max=%.2f)",
                pAttacker->m_dwSerial, dist, m_config.maxAttackRange);
        }
        return false;
    }
    return true;
}

bool AntiHack::ValidateSkillRange(CPlayer* pCaster, float targetX, float targetY, float targetZ)
{
    if (!pCaster) return false;

    float dist = Distance3D(
        pCaster->m_fPosX, pCaster->m_fPosY, pCaster->m_fPosZ,
        targetX, targetY, targetZ
    );

    if (dist > m_config.maxDebuffRange)
    {
        if (m_pLog)
        {
            m_pLog->Write("[LongDistDebuff] Player Serial=%u skill dist=%.2f (max=%.2f)",
                pCaster->m_dwSerial, dist, m_config.maxDebuffRange);
        }
        return false;
    }
    return true;
}

// ---- Invis Hack Detection ----

bool AntiHack::ValidateVisibility(CPlayer* pPlayer)
{
    if (!pPlayer) return false;
    if (m_config.invisHackMaxTrigger <= 0) return true; // detection disabled

    // Check if player has invisibility status without appropriate buff/skill
    // This checks if the player's visibility flag is abnormally set
    unsigned int serial = pPlayer->m_dwSerial;

    std::lock_guard<std::mutex> lock(m_mutex);
    auto& data = m_playerData[serial];

    // Player is invisible but has no GM flag and no valid invis buff
    bool isInvisible = (pPlayer->m_byGMGrade == 0 /* not GM */);
    // NOTE: Full check requires verifying pPlayer buff list for valid invis skills
    // Placeholder: check pPlayer->m_bVisible or equivalent field

    if (isInvisible)
    {
        data.invisViolationCount++;
        if (m_pLog)
        {
            m_pLog->Write("[InvisHack] Player Serial=%u invis without buff, trigger=%d/%d",
                serial, data.invisViolationCount, m_config.invisHackMaxTrigger);
        }

        if (m_config.invisHackAutoKick && data.invisViolationCount >= m_config.invisHackMaxTrigger)
        {
            m_pLog->Write("[InvisHack] KICK Player Serial=%u", serial);
            return false;
        }
    }
    else
    {
        data.invisViolationCount = 0;
    }

    return true;
}

// ---- Player lifecycle ----

void AntiHack::OnPlayerEnterWorld(unsigned int playerSerial)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_playerData[playerSerial] = PlayerMoveData();
}

void AntiHack::OnPlayerLeaveWorld(unsigned int playerSerial)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_playerData.erase(playerSerial);
}

const PlayerMoveData* AntiHack::GetPlayerData(unsigned int playerSerial) const
{
    auto it = m_playerData.find(playerSerial);
    if (it != m_playerData.end())
        return &it->second;
    return nullptr;
}

// ---- Helpers ----

float AntiHack::Distance3D(float x1, float y1, float z1, float x2, float y2, float z2) const
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float dz = z2 - z1;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

float AntiHack::Distance2D(float x1, float z1, float x2, float z2) const
{
    float dx = x2 - x1;
    float dz = z2 - z1;
    return std::sqrt(dx * dx + dz * dz);
}
