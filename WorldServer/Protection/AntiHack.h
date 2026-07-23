#pragma once
// ============================================================================
// AntiHack.h - Anti-Hack Detection Module
// Covers: Wall Hack, Speed Hack, Fly Hack, InvisHack, Long Distance Attack
// ============================================================================

#include <map>
#include <mutex>
#include <cmath>
#include <cstring>

// Forward declarations (from WorldServer)
class CPlayer;
class CLogFile;

// ---- Player movement tracking data ----
struct PlayerMoveData
{
    float lastPosX;
    float lastPosY;
    float lastPosZ;
    unsigned int lastMoveTime;      // GetTickCount timestamp
    int speedViolationCount;
    int wallViolationCount;
    int flyViolationCount;
    int invisViolationCount;
    unsigned int lastSpeedHackAlert; // cooldown for alerts
    bool initialized;

    PlayerMoveData()
        : lastPosX(0), lastPosY(0), lastPosZ(0)
        , lastMoveTime(0)
        , speedViolationCount(0)
        , wallViolationCount(0)
        , flyViolationCount(0)
        , invisViolationCount(0)
        , lastSpeedHackAlert(0)
        , initialized(false)
    {}
};

// ---- Anti-Hack Config (loaded from Player.ini / AttackSystem.ini) ----
struct AntiHackConfig
{
    // Speed hack
    float moveSpeedStep;        // base speed multiplier cap
    float moveSpeedTolerance;   // tolerance before flagging
    int   speedHackMaxCount;    // max violations before kick
    bool  speedHackAutoKick;

    // Wall hack
    bool  fixWallHack;          // enable wall hack fix

    // Fly hack
    float maxHeightDelta;       // max allowed Y change per tick
    int   flyHackMaxCount;
    bool  flyHackAutoKick;

    // Invis hack
    bool  invisHackAutoKick;
    int   invisHackMaxTrigger;

    // Long distance attack
    float maxAttackRange;       // max allowed attack distance
    float maxDebuffRange;       // max allowed debuff distance

    AntiHackConfig()
        : moveSpeedStep(1.150f)
        , moveSpeedTolerance(0.2f)
        , speedHackMaxCount(10)
        , speedHackAutoKick(true)
        , fixWallHack(true)
        , maxHeightDelta(50.0f)
        , flyHackMaxCount(5)
        , flyHackAutoKick(true)
        , invisHackAutoKick(false)
        , invisHackMaxTrigger(0)
        , maxAttackRange(100.0f)
        , maxDebuffRange(120.0f)
    {}
};

class AntiHack
{
public:
    static AntiHack& Instance()
    {
        static AntiHack inst;
        return inst;
    }

    // Initialize from config files
    void Initialize();

    // Reload config
    void Reload();

    // ---- Validation hooks (call from packet handlers) ----

    // Validate player movement packet. Returns false if hack detected.
    // If returning false, player should be teleported back or kicked.
    bool ValidateMovement(CPlayer* pPlayer, float newX, float newY, float newZ);

    // Validate attack range. Returns false if distance too far.
    bool ValidateAttackRange(CPlayer* pAttacker, float targetX, float targetY, float targetZ);

    // Validate debuff/skill range.
    bool ValidateSkillRange(CPlayer* pCaster, float targetX, float targetY, float targetZ);

    // Check invis hack (call periodically on player tick)
    bool ValidateVisibility(CPlayer* pPlayer);

    // Register/unregister player tracking
    void OnPlayerEnterWorld(unsigned int playerSerial);
    void OnPlayerLeaveWorld(unsigned int playerSerial);

    // Get violation counts for a player
    const PlayerMoveData* GetPlayerData(unsigned int playerSerial) const;

    // Config access
    const AntiHackConfig& GetConfig() const { return m_config; }

private:
    AntiHack();
    ~AntiHack() {}

    float Distance3D(float x1, float y1, float z1, float x2, float y2, float z2) const;
    float Distance2D(float x1, float z1, float x2, float z2) const;

    // Player serial -> move data
    std::map<unsigned int, PlayerMoveData> m_playerData;
    mutable std::mutex m_mutex;
    AntiHackConfig m_config;
    CLogFile* m_pLog;
};
