#pragma once
// ============================================================================
// AntiCheat.h - Regular Anti-Cheat System
// Covers: Periodic integrity checks, HWID tracking/blocking, packet rate
//         limiting, suspicious behavior scoring
// ============================================================================

#include <map>
#include <set>
#include <vector>
#include <string>
#include <mutex>

class CPlayer;
class CLogFile;

// ---- Player cheat score tracking ----
struct CheatScore
{
    int totalScore;              // accumulated cheat suspicion score
    int speedHackHits;
    int positionHackHits;
    int packetFloodHits;
    int itemExploitHits;
    unsigned int lastCheckTime;
    unsigned int lastPacketTime;
    int packetCountPerSecond;
    int warnCount;               // number of warnings issued

    CheatScore()
        : totalScore(0), speedHackHits(0), positionHackHits(0)
        , packetFloodHits(0), itemExploitHits(0)
        , lastCheckTime(0), lastPacketTime(0)
        , packetCountPerSecond(0), warnCount(0)
    {}
};

// ---- Anti-Cheat Config ----
struct AntiCheatConfig
{
    // Scoring thresholds
    int warnThreshold;       // score to issue warning
    int kickThreshold;       // score to auto-kick
    int banThreshold;        // score to auto-ban (HWID block)
    float scoreDecayRate;    // score reduction per minute (rehabilitation)

    // Packet rate limiting
    int maxPacketsPerSecond;
    int packetFloodKickCount; // consecutive floods before kick

    // HWID
    bool hwidTrackingEnabled;
    bool hwidBlockingEnabled;

    // Periodic checks
    int checkIntervalMs;     // how often to run integrity checks (ms)

    AntiCheatConfig()
        : warnThreshold(50)
        , kickThreshold(100)
        , banThreshold(200)
        , scoreDecayRate(5.0f)
        , maxPacketsPerSecond(60)
        , packetFloodKickCount(5)
        , hwidTrackingEnabled(true)
        , hwidBlockingEnabled(true)
        , checkIntervalMs(10000)
    {}
};

class AntiCheat
{
public:
    static AntiCheat& Instance()
    {
        static AntiCheat inst;
        return inst;
    }

    void Initialize();
    void Reload();

    // ---- Cheat Scoring System ----

    // Add cheat score to a player (called by other modules when violations detected)
    void AddScore(unsigned int playerSerial, int points, const char* reason);

    // Get current score
    int GetScore(unsigned int playerSerial) const;

    // Check if player should be warned/kicked/banned
    enum CheatAction { ACTION_NONE, ACTION_WARN, ACTION_KICK, ACTION_BAN };
    CheatAction EvaluatePlayer(unsigned int playerSerial);

    // Decay scores over time (call periodically from main thread)
    void TickScoreDecay();

    // ---- Packet Rate Limiting ----

    // Call on every incoming packet from a player
    // Returns false if player is flooding
    bool OnPacketReceived(unsigned int playerSerial);

    // ---- HWID System ----

    // Check if HWID is blocked
    bool IsHWIDBlocked(const char* hwid);

    // Block a HWID
    void BlockHWID(const char* hwid, const char* reason);

    // Register player HWID (for tracking)
    void RegisterHWID(unsigned int playerSerial, const char* hwid);

    // Get HWID for a player
    std::string GetPlayerHWID(unsigned int playerSerial) const;

    // ---- Periodic Integrity Check ----

    // Run on all connected players (call from CMainThread timer)
    void RunIntegrityCheck();

    // ---- Player lifecycle ----
    void OnPlayerEnterWorld(unsigned int playerSerial);
    void OnPlayerLeaveWorld(unsigned int playerSerial);

    const AntiCheatConfig& GetConfig() const { return m_config; }

private:
    AntiCheat() : m_pLog(nullptr), m_lastDecayTick(0), m_lastCheckTick(0) {}

    void LoadBlockedHWIDs();

    AntiCheatConfig m_config;
    CLogFile* m_pLog;
    mutable std::mutex m_mutex;

    std::map<unsigned int, CheatScore> m_scores;          // serial -> score
    std::map<unsigned int, std::string> m_playerHWIDs;    // serial -> hwid
    std::set<std::string> m_blockedHWIDs;                 // blocked hardware IDs

    unsigned int m_lastDecayTick;
    unsigned int m_lastCheckTick;
};
