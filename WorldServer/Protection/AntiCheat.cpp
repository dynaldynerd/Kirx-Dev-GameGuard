#include "AntiCheat.h"
#include "ProtectConfig.h"
#include "../Entities/CLogFile.h"
#include "../Entities/CPlayer.h"

#include <algorithm>

// ============================================================================
// AntiCheat.cpp - Regular Anti-Cheat System Implementation
// ============================================================================

void AntiCheat::Initialize()
{
    static CLogFile s_log;
    s_log.SetWriteLogFile("Protection_AntiCheat", 1, false, true, true);
    m_pLog = &s_log;

    Reload();
    LoadBlockedHWIDs();

    m_lastDecayTick = GetTickCount();
    m_lastCheckTick = GetTickCount();

    m_pLog->Write("[AntiCheat] Initialized - %zu blocked HWIDs loaded",
        m_blockedHWIDs.size());
}

void AntiCheat::Reload()
{
    auto& cfg = ProtectConfig::Instance();

    m_config.warnThreshold      = cfg.GetInt("system.ini", "anticheat", "warn_threshold", 50);
    m_config.kickThreshold      = cfg.GetInt("system.ini", "anticheat", "kick_threshold", 100);
    m_config.banThreshold       = cfg.GetInt("system.ini", "anticheat", "ban_threshold", 200);
    m_config.scoreDecayRate     = cfg.GetFloat("system.ini", "anticheat", "score_decay_rate", 5.0f);
    m_config.maxPacketsPerSecond= cfg.GetInt("system.ini", "anticheat", "max_packets_per_second", 60);
    m_config.packetFloodKickCount = cfg.GetInt("system.ini", "anticheat", "packet_flood_kick_count", 5);
    m_config.hwidTrackingEnabled = cfg.GetBool("system.ini", "anticheat", "hwid_tracking", true);
    m_config.hwidBlockingEnabled = cfg.GetBool("system.ini", "anticheat", "hwid_blocking", true);
    m_config.checkIntervalMs    = cfg.GetInt("system.ini", "anticheat", "check_interval_ms", 10000);

    LoadBlockedHWIDs();

    if (m_pLog)
        m_pLog->Write("[AntiCheat] Config reloaded - Warn=%d Kick=%d Ban=%d MaxPPS=%d",
            m_config.warnThreshold, m_config.kickThreshold,
            m_config.banThreshold, m_config.maxPacketsPerSecond);
}

// ---- Cheat Scoring ----

void AntiCheat::AddScore(unsigned int playerSerial, int points, const char* reason)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto& score = m_scores[playerSerial];
    score.totalScore += points;

    if (m_pLog)
        m_pLog->Write("[CheatScore] Serial=%u +%d (%s) total=%d",
            playerSerial, points, reason ? reason : "unknown", score.totalScore);
}

int AntiCheat::GetScore(unsigned int playerSerial) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_scores.find(playerSerial);
    if (it == m_scores.end()) return 0;
    return it->second.totalScore;
}

AntiCheat::CheatAction AntiCheat::EvaluatePlayer(unsigned int playerSerial)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_scores.find(playerSerial);
    if (it == m_scores.end()) return ACTION_NONE;

    int score = it->second.totalScore;

    if (score >= m_config.banThreshold)
    {
        if (m_pLog)
            m_pLog->Write("[AntiCheat] BAN Serial=%u score=%d", playerSerial, score);

        // Auto-block HWID if available
        auto hwidIt = m_playerHWIDs.find(playerSerial);
        if (hwidIt != m_playerHWIDs.end() && m_config.hwidBlockingEnabled)
        {
            m_blockedHWIDs.insert(hwidIt->second);
            m_pLog->Write("[AntiCheat] HWID blocked: %s", hwidIt->second.c_str());
        }
        return ACTION_BAN;
    }

    if (score >= m_config.kickThreshold)
    {
        if (m_pLog)
            m_pLog->Write("[AntiCheat] KICK Serial=%u score=%d", playerSerial, score);
        return ACTION_KICK;
    }

    if (score >= m_config.warnThreshold)
    {
        it->second.warnCount++;
        if (it->second.warnCount <= 3) // limit warn spam
        {
            if (m_pLog)
                m_pLog->Write("[AntiCheat] WARN Serial=%u score=%d warn#%d",
                    playerSerial, score, it->second.warnCount);
            return ACTION_WARN;
        }
    }

    return ACTION_NONE;
}

void AntiCheat::TickScoreDecay()
{
    unsigned int now = GetTickCount();
    if (now - m_lastDecayTick < 60000) return; // decay every 60 seconds
    m_lastDecayTick = now;

    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto it = m_scores.begin(); it != m_scores.end(); )
    {
        it->second.totalScore -= static_cast<int>(m_config.scoreDecayRate);
        if (it->second.totalScore < 0)
            it->second.totalScore = 0;

        // Clean up players with zero score who disconnected
        if (it->second.totalScore == 0)
        {
            // Keep the entry (player might still be online)
        }
        ++it;
    }
}

// ---- Packet Rate Limiting ----

bool AntiCheat::OnPacketReceived(unsigned int playerSerial)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto& score = m_scores[playerSerial];

    unsigned int now = GetTickCount();
    unsigned int elapsed = now - score.lastPacketTime;

    if (elapsed >= 1000)
    {
        // Reset counter every second
        score.packetCountPerSecond = 1;
        score.lastPacketTime = now;
        return true;
    }

    score.packetCountPerSecond++;

    if (score.packetCountPerSecond > m_config.maxPacketsPerSecond)
    {
        score.packetFloodHits++;

        if (m_pLog && score.packetFloodHits <= 3)
            m_pLog->Write("[PacketFlood] Serial=%u %d packets/sec (max=%d) floods=%d",
                playerSerial, score.packetCountPerSecond,
                m_config.maxPacketsPerSecond, score.packetFloodHits);

        // Add cheat score for flooding
        score.totalScore += 5;

        if (score.packetFloodHits >= m_config.packetFloodKickCount)
        {
            if (m_pLog)
                m_pLog->Write("[PacketFlood] KICK Serial=%u - %d consecutive floods",
                    playerSerial, score.packetFloodHits);
            return false;
        }

        return false; // drop this packet
    }
    else
    {
        // Reset flood counter on normal traffic
        if (score.packetFloodHits > 0)
            score.packetFloodHits--;
    }

    return true;
}

// ---- HWID System ----

bool AntiCheat::IsHWIDBlocked(const char* hwid)
{
    if (!hwid || !m_config.hwidBlockingEnabled) return false;

    std::lock_guard<std::mutex> lock(m_mutex);
    return m_blockedHWIDs.find(std::string(hwid)) != m_blockedHWIDs.end();
}

void AntiCheat::BlockHWID(const char* hwid, const char* reason)
{
    if (!hwid) return;

    std::lock_guard<std::mutex> lock(m_mutex);
    m_blockedHWIDs.insert(std::string(hwid));

    if (m_pLog)
        m_pLog->Write("[HWID] Blocked: %s reason: %s", hwid, reason ? reason : "manual");
}

void AntiCheat::RegisterHWID(unsigned int playerSerial, const char* hwid)
{
    if (!hwid || !m_config.hwidTrackingEnabled) return;

    std::lock_guard<std::mutex> lock(m_mutex);
    m_playerHWIDs[playerSerial] = std::string(hwid);

    if (m_pLog)
        m_pLog->Write("[HWID] Registered Serial=%u HWID=%s", playerSerial, hwid);
}

std::string AntiCheat::GetPlayerHWID(unsigned int playerSerial) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_playerHWIDs.find(playerSerial);
    if (it != m_playerHWIDs.end())
        return it->second;
    return "";
}

// ---- Periodic Integrity Check ----

void AntiCheat::RunIntegrityCheck()
{
    unsigned int now = GetTickCount();
    if (now - m_lastCheckTick < static_cast<unsigned int>(m_config.checkIntervalMs))
        return;
    m_lastCheckTick = now;

    // This runs periodically from CMainThread
    // Integration: iterate all online players and run checks:
    //
    // for each CPlayer* pPlayer in online list:
    //   1. Check memory integrity (client checksum validation)
    //   2. Verify position is on valid terrain
    //   3. Check for impossible stat values
    //   4. Verify equipment integrity
    //   5. Check buff list validity
    //
    // Example:
    //   CObjectList* pList = g_Main.GetObjectList();
    //   for (int i = 0; i < pList->GetCount(); i++) {
    //       CPlayer* p = pList->GetPlayer(i);
    //       if (!p) continue;
    //       // Run validation checks...
    //   }

    TickScoreDecay();
}

// ---- Player Lifecycle ----

void AntiCheat::OnPlayerEnterWorld(unsigned int playerSerial)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_scores[playerSerial] = CheatScore();
}

void AntiCheat::OnPlayerLeaveWorld(unsigned int playerSerial)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    // Keep score data for a while (don't erase immediately)
    // This prevents disconnect-reconnect to reset cheat score
    // Scores will be cleaned up by decay system
    m_playerHWIDs.erase(playerSerial);
}

// ---- Private Helpers ----

void AntiCheat::LoadBlockedHWIDs()
{
    auto& cfg = ProtectConfig::Instance();

    std::lock_guard<std::mutex> lock(m_mutex);
    m_blockedHWIDs.clear();

    int count = cfg.GetInt("hardwareblocked.ini", "config", "num", 0);
    for (int i = 0; i < count; i++)
    {
        char key[32];
        snprintf(key, sizeof(key), "hwid%d", i);
        std::string hwid = cfg.GetString("hardwareblocked.ini", "config", key, "");
        if (!hwid.empty())
        {
            m_blockedHWIDs.insert(hwid);
        }
    }
}
