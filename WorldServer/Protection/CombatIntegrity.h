#pragma once
// ============================================================================
// CombatIntegrity.h - Combat & Entity Integrity Validation
// Covers: Unit Delivery Vulnerability (WH/FH), Trap Make Vulnerability,
//         Tower Make Vulnerability (WH/FH/Level/MapLoading),
//         Teleport in Battle Mode, Long Distance Attack (enhanced)
// ============================================================================

#include <map>
#include <mutex>
#include <set>

class CPlayer;
class CLogFile;

// ---- Combat Integrity Config ----
struct CombatIntegrityConfig
{
    // Unit delivery
    bool validateUnitDelivery;       // WH/FH check on unit delivery
    float unitDeliveryMaxDist;       // max distance for unit delivery
    int   unitDeliveryMinLevel;      // min level to deliver unit
    int   unitDeliveryDelayMs;       // minimum delay between deliveries

    // Tower/Trap creation
    bool validateTowerCreate;        // WH/FH/Level check
    bool validateTrapCreate;         // WH/FH/Level check
    int  towerMinLevel;              // min level to build tower
    int  trapMinLevel;               // min level to build trap
    bool towerCheckMapLoading;       // block during map loading
    bool trapCheckMapLoading;

    // Teleport in battle
    bool blockTeleportInBattle;
    bool blockTeleportDuringCW;      // also block during chip war

    // Combat range (enhanced from AntiHack)
    float meleeMaxRange;
    float rangedMaxRange;
    float forceMaxRange;             // force/magic skill range
    float siegeMaxRange;             // siege kit range
    float aoeMaxRange;               // area of effect range

    CombatIntegrityConfig()
        : validateUnitDelivery(true)
        , unitDeliveryMaxDist(30.0f)
        , unitDeliveryMinLevel(30)
        , unitDeliveryDelayMs(2500)
        , validateTowerCreate(true)
        , validateTrapCreate(true)
        , towerMinLevel(35)
        , trapMinLevel(30)
        , towerCheckMapLoading(true)
        , trapCheckMapLoading(true)
        , blockTeleportInBattle(true)
        , blockTeleportDuringCW(true)
        , meleeMaxRange(15.0f)
        , rangedMaxRange(80.0f)
        , forceMaxRange(60.0f)
        , siegeMaxRange(100.0f)
        , aoeMaxRange(40.0f)
    {}
};

class CombatIntegrity
{
public:
    static CombatIntegrity& Instance()
    {
        static CombatIntegrity inst;
        return inst;
    }

    void Initialize();
    void Reload();

    // ---- Unit Delivery Validation ----

    // Validate unit (MAU) delivery request
    // Checks: position validity (WH/FH), level, cooldown
    bool ValidateUnitDelivery(CPlayer* pPlayer, float deliverX, float deliverY, float deliverZ);

    // Track unit delivery cooldown
    bool CheckUnitDeliveryCooldown(unsigned int playerSerial);
    void RecordUnitDelivery(unsigned int playerSerial);

    // ---- Tower Creation Validation ----

    // Full tower validation: level, position (WH/FH), map loading state
    bool ValidateTowerCreate(CPlayer* pPlayer, float posX, float posY, float posZ,
                              int mapIndex, bool isMapLoading);

    // ---- Trap Creation Validation ----

    // Full trap validation: level, position (WH/FH), map loading state
    bool ValidateTrapCreate(CPlayer* pPlayer, float posX, float posY, float posZ,
                             int mapIndex, bool isMapLoading);

    // ---- Teleport Validation (Enhanced) ----

    // Check all teleport conditions
    bool CanPlayerTeleport(CPlayer* pPlayer, bool isBattleMode, bool isChipWar);

    // ---- Combat Range Validation (Enhanced) ----

    // Validate attack range based on weapon type
    enum WeaponType { MELEE, RANGED, FORCE, SIEGE, AOE };
    bool ValidateCombatRange(CPlayer* pPlayer, float targetX, float targetY, float targetZ,
                              WeaponType weaponType);

    // ---- Position Integrity ----

    // Check if position is valid on current map (basic height/boundary check)
    bool IsValidPosition(int mapIndex, float posX, float posY, float posZ);

    // Check if player position changed abnormally (wall hack indicator)
    bool CheckPositionIntegrity(CPlayer* pPlayer, float newX, float newY, float newZ);

    const CombatIntegrityConfig& GetConfig() const { return m_config; }

private:
    CombatIntegrity() : m_pLog(nullptr) {}

    float Distance3D(float x1, float y1, float z1, float x2, float y2, float z2) const;

    CombatIntegrityConfig m_config;
    CLogFile* m_pLog;
    mutable std::mutex m_mutex;

    // Unit delivery cooldown tracking
    std::map<unsigned int, unsigned int> m_unitDeliveryTimes; // serial -> last delivery time
};
