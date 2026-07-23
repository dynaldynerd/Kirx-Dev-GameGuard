#pragma once
// ============================================================================
// GameFixes.h - Game Logic Fix Module
// Covers: Honor Guild, Vote Fix, Tower/Trap Restriction, NPC Pager,
//         Character Creation, Teleport Battle Mode, Race Item Check,
//         Buff Fixes, MAU Key, Combine UI, Tower Remote Collect
// ============================================================================

#include <map>
#include <set>
#include <vector>
#include <mutex>
#include <string>

class CPlayer;
class CLogFile;

// ---- Tower/Trap Build Restriction ----
struct TowerRestriction
{
    int  requiredLevel;
    bool licenseRequired;
    std::string licenseItemCode;
    std::vector<int> allowedMaps; // empty = all maps
};

// ---- Vote System Fix ----
struct VoteRecord
{
    unsigned int accountId;
    unsigned int lastVoteTime;
    int voteCount;
};

// ---- Game Fixes Config ----
struct GameFixesConfig
{
    // Honor Guild
    bool honorGuildOriginal;     // true = use original system (race leader assigns)

    // Vote
    bool voteEnabled;
    int  voteMaxPerAccount;

    // Tower
    bool towerReturnAfterDC;
    bool towerLicenseEnabled;
    int  towerLicenseCount;
    std::vector<TowerRestriction> towerRestrictions;

    // Character creation
    bool oneRaceOneAccount;
    int  charDeleteMinLevel;
    int  charDeleteMaxLevel;
    bool skipTutorial;

    // Teleport
    bool blockTeleportInBattle;  // prevent teleport during battle mode
    bool blockFreeTPToHQ;        // prevent non-GM from using GM teleport

    // Race item check
    bool enforceRaceItemCheck;   // block using items from another race

    // Buff fixes
    bool fixCWBuffOnLeaveGuild;  // remove CW buff when leaving guild
    bool fixBuffAfterRespawn;    // clear invalid buffs after respawn
    bool fixDualPotionSameEffect;// prevent stacking same-effect potions

    // MAU
    bool fixMAUKeyDestroy;       // fix MAU key not being consumed properly

    // Combine UI
    bool fixCombineUIOver5;      // fix combine crash when > 5 results

    // Tower collect
    bool fixTowerRemoteCollect;  // prevent collecting tower from wrong location

    // CPT from same race animus
    bool fixCPTSameRaceAnimus;

    // NPC Pager
    bool fixNPCPager;

    // Post/Mail
    bool postMailEnabled;

    // Player config
    int maxLevel;
    int limitExpLevel;
    long long maxDalant;
    long long maxGold;
    int minChatAllLevel;

    // Character name
    int charNameType;  // 0 = alphanumeric only, 1 = with special chars

    GameFixesConfig()
        : honorGuildOriginal(false)
        , voteEnabled(true), voteMaxPerAccount(1)
        , towerReturnAfterDC(true), towerLicenseEnabled(false), towerLicenseCount(2)
        , oneRaceOneAccount(false), charDeleteMinLevel(40), charDeleteMaxLevel(50)
        , skipTutorial(false)
        , blockTeleportInBattle(true), blockFreeTPToHQ(true)
        , enforceRaceItemCheck(true)
        , fixCWBuffOnLeaveGuild(true), fixBuffAfterRespawn(true)
        , fixDualPotionSameEffect(true)
        , fixMAUKeyDestroy(true), fixCombineUIOver5(true)
        , fixTowerRemoteCollect(true), fixCPTSameRaceAnimus(true)
        , fixNPCPager(true)
        , postMailEnabled(false)
        , maxLevel(50), limitExpLevel(66)
        , maxDalant(2000000000LL), maxGold(500000LL)
        , minChatAllLevel(30), charNameType(0)
    {}
};

class GameFixes
{
public:
    static GameFixes& Instance()
    {
        static GameFixes inst;
        return inst;
    }

    void Initialize();
    void Reload();

    // ---- Validation Hooks ----

    // Honor Guild: Check if player can assign honor guild
    bool CanAssignHonorGuild(CPlayer* pPlayer);

    // Vote: Check if account already voted
    bool CanVote(unsigned int accountId);
    void RecordVote(unsigned int accountId);

    // Tower: Validate tower build request
    bool CanBuildTower(CPlayer* pPlayer, int towerLevel, int mapIndex);

    // Trap: Validate trap build request
    bool CanBuildTrap(CPlayer* pPlayer, int mapIndex);

    // Character Creation: Validate new character
    bool CanCreateCharacter(unsigned int accountId, unsigned char raceCode);
    bool CanDeleteCharacter(CPlayer* pPlayer);

    // Character Name: Validate name
    bool ValidateCharacterName(const char* name);

    // Teleport: Check if teleport allowed
    bool CanTeleport(CPlayer* pPlayer, bool isBattleMode);
    bool CanUseFreeTPToHQ(CPlayer* pPlayer);

    // Race Item: Check if player can use item of given race
    bool CanUseItem(CPlayer* pPlayer, unsigned char itemRaceCode);

    // Buff: Fix CW buff on guild leave
    void OnPlayerLeaveGuild(CPlayer* pPlayer);

    // Buff: Clean invalid buffs after respawn
    void OnPlayerRespawn(CPlayer* pPlayer);

    // Potion: Check dual potion same effect
    bool CanUsePotionEffect(CPlayer* pPlayer, int effectType);

    // MAU: Fix key destroy
    void OnMAUKeyUse(CPlayer* pPlayer);

    // Combine: Validate result count
    bool ValidateCombineResults(int resultCount);

    // Tower: Validate collect distance
    bool CanCollectTower(CPlayer* pPlayer, float towerX, float towerY, float towerZ);

    // CPT: Check animus race
    bool CanGetCPTFromAnimus(CPlayer* pPlayer, unsigned char animusRaceCode);

    // NPC Pager: Check enabled
    bool IsNPCPagerFixed() const { return m_config.fixNPCPager; }

    // Skip tutorial
    bool ShouldSkipTutorial() const { return m_config.skipTutorial; }

    // Post mail
    bool IsPostMailEnabled() const { return m_config.postMailEnabled; }

    // Player limits
    int  GetMaxLevel() const { return m_config.maxLevel; }
    long long GetMaxDalant() const { return m_config.maxDalant; }
    long long GetMaxGold() const { return m_config.maxGold; }

    const GameFixesConfig& GetConfig() const { return m_config; }

private:
    GameFixes();
    ~GameFixes() {}

    float Distance3D(float x1, float y1, float z1, float x2, float y2, float z2) const;
    void LoadTowerRestrictions();

    GameFixesConfig m_config;
    std::map<unsigned int, VoteRecord> m_voteRecords;
    std::map<unsigned int, std::set<unsigned char>> m_accountRaces; // accountId -> races used
    std::map<unsigned int, std::set<int>> m_playerActiveEffects;    // serial -> active potion effects
    mutable std::mutex m_mutex;
    CLogFile* m_pLog;
};
