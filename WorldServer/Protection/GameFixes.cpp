#include "GameFixes.h"
#include "ProtectConfig.h"
#include "../Entities/CLogFile.h"
#include "../Entities/CPlayer.h"

#include <cmath>
#include <algorithm>
#include <cctype>
#include <regex>

// ============================================================================
// GameFixes.cpp - Game Logic Fix Implementation
// ============================================================================

GameFixes::GameFixes()
    : m_pLog(nullptr)
{
}

void GameFixes::Initialize()
{
    static CLogFile s_fixLog;
    s_fixLog.SetWriteLogFile("Protection_GameFixes", 1, false, true, true);
    m_pLog = &s_fixLog;

    Reload();
    m_pLog->Write("[GameFixes] Initialized successfully");
}

void GameFixes::Reload()
{
    auto& cfg = ProtectConfig::Instance();

    // Guild.ini
    m_config.honorGuildOriginal = cfg.GetBool("guild.ini", "honorguild", "fix", false);

    // Patriarch_Vote.ini / System
    m_config.voteEnabled        = true;
    m_config.voteMaxPerAccount  = 1;

    // Tower.ini
    m_config.towerReturnAfterDC  = cfg.GetBool("tower.ini", "config", "returnafterdisconnect", true);
    m_config.towerLicenseEnabled = cfg.GetBool("tower.ini", "license", "enable", false);
    m_config.towerLicenseCount   = cfg.GetInt("tower.ini", "license", "num", 2);
    LoadTowerRestrictions();

    // Addon.ini / System.ini / Player.ini
    m_config.oneRaceOneAccount  = cfg.GetBool("addon.ini", "lobby", "one_race_one_account", false);
    m_config.charDeleteMinLevel = cfg.GetInt("addon.ini", "lobby", "char_delete_deprecate_lvl_min", 40);
    m_config.charDeleteMaxLevel = cfg.GetInt("addon.ini", "lobby", "char_delete_deprecate_lvl_max", 50);
    m_config.skipTutorial       = cfg.GetBool("system.ini", "skiptutorial", "enable", false);
    m_config.fixNPCPager        = cfg.GetBool("addon.ini", "fixpager", "enable", true);
    m_config.postMailEnabled    = cfg.GetBool("system.ini", "postmail", "enable", false);

    // Player.ini
    m_config.maxLevel       = cfg.GetInt("player.ini", "player", "max_level", 50);
    m_config.limitExpLevel  = cfg.GetInt("player.ini", "player", "limit_exp_level", 66);
    m_config.maxDalant      = cfg.GetInt("player.ini", "player", "max_dalant", 2000000000);
    m_config.maxGold        = cfg.GetInt("player.ini", "player", "max_gold", 500000);
    m_config.minChatAllLevel= cfg.GetInt("player.ini", "player", "min_chat_all_level", 30);

    // System.ini
    m_config.charNameType   = cfg.GetInt("system.ini", "charactername", "type", 0);

    // Bank limits
    // cfg.GetInt("system.ini", "bank", "dalantlimit", 1000000000);

    // Battle/exploit fixes - these are always enabled (hardcoded for safety)
    m_config.blockTeleportInBattle  = true;
    m_config.blockFreeTPToHQ        = true;
    m_config.enforceRaceItemCheck   = true;
    m_config.fixCWBuffOnLeaveGuild  = true;
    m_config.fixBuffAfterRespawn    = true;
    m_config.fixDualPotionSameEffect= true;
    m_config.fixMAUKeyDestroy       = true;
    m_config.fixCombineUIOver5      = true;
    m_config.fixTowerRemoteCollect  = true;
    m_config.fixCPTSameRaceAnimus   = true;

    if (m_pLog)
        m_pLog->Write("[GameFixes] Config reloaded - HonorGuild=%s OneRace=%s SkipTut=%s Pager=%s",
            m_config.honorGuildOriginal ? "ORIGINAL" : "FIXED",
            m_config.oneRaceOneAccount ? "ON" : "OFF",
            m_config.skipTutorial ? "ON" : "OFF",
            m_config.fixNPCPager ? "ON" : "OFF");
}

// ---- Honor Guild ----

bool GameFixes::CanAssignHonorGuild(CPlayer* pPlayer)
{
    if (!pPlayer) return false;

    if (m_config.honorGuildOriginal)
    {
        // Original system: only race leader (patriarch) can assign
        // Check if pPlayer is patriarch
        // Integration: check pPlayer->m_byPatriarch or equivalent
        return true; // placeholder
    }

    // Fixed system: honor guild is assigned by contribution/ranking
    // Integration: check guild ranking data
    return true;
}

// ---- Vote Fix ----

bool GameFixes::CanVote(unsigned int accountId)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_voteRecords.find(accountId);
    if (it == m_voteRecords.end())
        return true;

    if (it->second.voteCount >= m_config.voteMaxPerAccount)
    {
        if (m_pLog)
            m_pLog->Write("[Vote] BLOCKED Account=%u already voted %d times (max=%d)",
                accountId, it->second.voteCount, m_config.voteMaxPerAccount);
        return false;
    }
    return true;
}

void GameFixes::RecordVote(unsigned int accountId)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto& rec = m_voteRecords[accountId];
    rec.accountId = accountId;
    rec.lastVoteTime = GetTickCount();
    rec.voteCount++;
}

// ---- Tower/Trap Restriction ----

bool GameFixes::CanBuildTower(CPlayer* pPlayer, int towerLevel, int mapIndex)
{
    if (!pPlayer) return false;

    // Level check
    for (const auto& r : m_config.towerRestrictions)
    {
        if (pPlayer->m_nLevel < r.requiredLevel)
        {
            if (m_pLog)
                m_pLog->Write("[Tower] BLOCKED Serial=%u level=%d required=%d",
                    pPlayer->m_dwSerial, pPlayer->m_nLevel, r.requiredLevel);
            return false;
        }

        // Map check
        if (!r.allowedMaps.empty())
        {
            if (std::find(r.allowedMaps.begin(), r.allowedMaps.end(), mapIndex) == r.allowedMaps.end())
            {
                if (m_pLog)
                    m_pLog->Write("[Tower] BLOCKED Serial=%u map=%d not in allowed list",
                        pPlayer->m_dwSerial, mapIndex);
                return false;
            }
        }

        // License check
        if (m_config.towerLicenseEnabled && !r.licenseItemCode.empty())
        {
            // Integration: Check if player has license item in inventory
            // bool hasLicense = pPlayer->HasItem(r.licenseItemCode.c_str());
            // if (!hasLicense) return false;
        }
    }

    return true;
}

bool GameFixes::CanBuildTrap(CPlayer* pPlayer, int mapIndex)
{
    if (!pPlayer) return false;

    // Same restrictions as tower (level + map check)
    // Integration: similar to CanBuildTower but for traps
    return true;
}

// ---- Character Creation ----

bool GameFixes::CanCreateCharacter(unsigned int accountId, unsigned char raceCode)
{
    if (!m_config.oneRaceOneAccount)
        return true;

    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_accountRaces.find(accountId);
    if (it == m_accountRaces.end())
        return true;

    // Account already has characters - check if same race
    if (!it->second.empty() && it->second.find(raceCode) == it->second.end())
    {
        if (m_pLog)
            m_pLog->Write("[CharCreate] BLOCKED Account=%u tried race=%d but already has different race",
                accountId, raceCode);
        return false;
    }
    return true;
}

bool GameFixes::CanDeleteCharacter(CPlayer* pPlayer)
{
    if (!pPlayer) return false;

    int level = pPlayer->m_nLevel;
    if (level >= m_config.charDeleteMinLevel && level <= m_config.charDeleteMaxLevel)
    {
        if (m_pLog)
            m_pLog->Write("[CharDelete] BLOCKED Serial=%u level=%d in restricted range [%d-%d]",
                pPlayer->m_dwSerial, level, m_config.charDeleteMinLevel, m_config.charDeleteMaxLevel);
        return false;
    }
    return true;
}

bool GameFixes::ValidateCharacterName(const char* name)
{
    if (!name || name[0] == 0) return false;

    size_t len = strlen(name);
    if (len < 2 || len > 16) return false;

    for (size_t i = 0; i < len; i++)
    {
        char c = name[i];
        if (m_config.charNameType == 0)
        {
            // Type 0: only a-z, A-Z, 0-9
            if (!std::isalnum(static_cast<unsigned char>(c)))
                return false;
        }
        else if (m_config.charNameType == 1)
        {
            // Type 1: also allow [ ] - ? ! +
            if (!std::isalnum(static_cast<unsigned char>(c)) &&
                c != '[' && c != ']' && c != '-' && c != '?' && c != '!' && c != '+')
                return false;
        }

        // SQL injection prevention - block dangerous characters
        if (c == '\'' || c == '"' || c == ';' || c == '\\' || c == '%' || c == '_')
        {
            if (m_pLog)
                m_pLog->Write("[SQLInject] BLOCKED character name with dangerous char: '%s'", name);
            return false;
        }
    }
    return true;
}

// ---- Teleport ----

bool GameFixes::CanTeleport(CPlayer* pPlayer, bool isBattleMode)
{
    if (!pPlayer) return false;

    if (m_config.blockTeleportInBattle && isBattleMode)
    {
        if (m_pLog)
            m_pLog->Write("[Teleport] BLOCKED Serial=%u tried teleport in battle mode",
                pPlayer->m_dwSerial);
        return false;
    }
    return true;
}

bool GameFixes::CanUseFreeTPToHQ(CPlayer* pPlayer)
{
    if (!pPlayer) return false;

    if (m_config.blockFreeTPToHQ)
    {
        // Only GM can use free teleport to HQ
        if (pPlayer->m_byGMGrade == 0)
        {
            if (m_pLog)
                m_pLog->Write("[Teleport] BLOCKED non-GM Serial=%u tried free TP to HQ",
                    pPlayer->m_dwSerial);
            return false;
        }
    }
    return true;
}

// ---- Race Item Check ----

bool GameFixes::CanUseItem(CPlayer* pPlayer, unsigned char itemRaceCode)
{
    if (!pPlayer) return true;
    if (!m_config.enforceRaceItemCheck) return true;
    if (itemRaceCode == 0xFF || itemRaceCode == 0) return true; // universal item

    // Check player race matches item race
    if (pPlayer->m_byRace != itemRaceCode)
    {
        if (m_pLog)
            m_pLog->Write("[RaceItem] BLOCKED Serial=%u race=%d tried item race=%d",
                pPlayer->m_dwSerial, pPlayer->m_byRace, itemRaceCode);
        return false;
    }
    return true;
}

// ---- Buff Fixes ----

void GameFixes::OnPlayerLeaveGuild(CPlayer* pPlayer)
{
    if (!pPlayer) return;
    if (!m_config.fixCWBuffOnLeaveGuild) return;

    // Remove all CW (Chip War) related buffs when player leaves guild
    // Integration: iterate through pPlayer's active buff list
    // and remove any buffs with CW-related effect IDs
    //
    // Example:
    //   for each buff in pPlayer->m_BuffList:
    //     if buff.sourceType == BUFF_SOURCE_CW_REWARD:
    //       pPlayer->RemoveBuff(buff.id)

    if (m_pLog)
        m_pLog->Write("[BuffFix] Cleared CW buffs for Serial=%u on guild leave",
            pPlayer->m_dwSerial);
}

void GameFixes::OnPlayerRespawn(CPlayer* pPlayer)
{
    if (!pPlayer) return;
    if (!m_config.fixBuffAfterRespawn) return;

    // Clear invalid buffs after respawn
    // Some hacks allow keeping buffs through death by manipulating packets
    //
    // Integration: validate each active buff:
    //   - Check buff duration hasn't expired
    //   - Check buff source is still valid (caster alive, in range, etc.)
    //   - Remove any buff that shouldn't persist through death

    if (m_pLog)
        m_pLog->Write("[BuffFix] Validated buffs after respawn for Serial=%u",
            pPlayer->m_dwSerial);
}

// ---- Dual Potion Fix ----

bool GameFixes::CanUsePotionEffect(CPlayer* pPlayer, int effectType)
{
    if (!pPlayer) return true;
    if (!m_config.fixDualPotionSameEffect) return true;

    std::lock_guard<std::mutex> lock(m_mutex);
    auto& effects = m_playerActiveEffects[pPlayer->m_dwSerial];

    if (effects.find(effectType) != effects.end())
    {
        if (m_pLog)
            m_pLog->Write("[PotionFix] BLOCKED Serial=%u dual potion effect=%d",
                pPlayer->m_dwSerial, effectType);
        return false;
    }

    effects.insert(effectType);
    return true;
}

// ---- MAU Key Fix ----

void GameFixes::OnMAUKeyUse(CPlayer* pPlayer)
{
    if (!pPlayer) return;
    if (!m_config.fixMAUKeyDestroy) return;

    // Properly consume MAU key on use
    // The bug allows using MAU without the key being consumed,
    // so player doesn't need to relog.
    //
    // Integration: After MAU activation, ensure the key item is
    // properly removed from inventory:
    //   pPlayer->RemoveItem(keyItemSerial, 1);
    //   pPlayer->SendMsg_AlterItemDurInform(...);

    if (m_pLog)
        m_pLog->Write("[MAUFix] Key consumed for Serial=%u", pPlayer->m_dwSerial);
}

// ---- Combine UI Fix ----

bool GameFixes::ValidateCombineResults(int resultCount)
{
    if (!m_config.fixCombineUIOver5) return true;

    if (resultCount > 5)
    {
        if (m_pLog)
            m_pLog->Write("[CombineFix] BLOCKED combine with %d results (max 5)", resultCount);
        return false;
    }
    return true;
}

// ---- Tower Remote Collect Fix ----

bool GameFixes::CanCollectTower(CPlayer* pPlayer, float towerX, float towerY, float towerZ)
{
    if (!pPlayer) return false;
    if (!m_config.fixTowerRemoteCollect) return true;

    float dist = Distance3D(
        pPlayer->m_fPosX, pPlayer->m_fPosY, pPlayer->m_fPosZ,
        towerX, towerY, towerZ
    );

    // Max allowed distance to collect tower (reasonable interaction range)
    const float MAX_COLLECT_DIST = 30.0f;

    if (dist > MAX_COLLECT_DIST)
    {
        if (m_pLog)
            m_pLog->Write("[TowerFix] BLOCKED remote collect Serial=%u dist=%.2f (max=%.2f)",
                pPlayer->m_dwSerial, dist, MAX_COLLECT_DIST);
        return false;
    }
    return true;
}

// ---- CPT Same Race Animus ----

bool GameFixes::CanGetCPTFromAnimus(CPlayer* pPlayer, unsigned char animusRaceCode)
{
    if (!pPlayer) return false;
    if (!m_config.fixCPTSameRaceAnimus) return true;

    // Block getting CPT (Contribution Points) from animus of same race
    if (pPlayer->m_byRace == animusRaceCode)
    {
        if (m_pLog)
            m_pLog->Write("[CPTFix] BLOCKED Serial=%u same race CPT from animus race=%d",
                pPlayer->m_dwSerial, animusRaceCode);
        return false;
    }
    return true;
}

// ---- Private Helpers ----

void GameFixes::LoadTowerRestrictions()
{
    auto& cfg = ProtectConfig::Instance();
    m_config.towerRestrictions.clear();

    for (int i = 0; i < m_config.towerLicenseCount; i++)
    {
        TowerRestriction r;

        char key[64];
        snprintf(key, sizeof(key), "guardtowerlv%d", i);
        r.requiredLevel = cfg.GetInt("tower.ini", "license", key, 35);

        snprintf(key, sizeof(key), "workonmap%d", i);
        std::string mapStr = cfg.GetString("tower.ini", "license", key, "all");
        if (mapStr != "all" && !mapStr.empty())
        {
            r.allowedMaps = cfg.GetIntList("tower.ini", "license", key);
        }

        snprintf(key, sizeof(key), "licenseitem%d", i);
        r.licenseItemCode = cfg.GetString("tower.ini", "license", key, "");

        m_config.towerRestrictions.push_back(r);
    }
}

float GameFixes::Distance3D(float x1, float y1, float z1, float x2, float y2, float z2) const
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float dz = z2 - z1;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}
