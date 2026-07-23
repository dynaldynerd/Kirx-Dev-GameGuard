#pragma once
// ============================================================================
// ItemIntegrity.h - Item & Equipment Integrity Validation
// Covers: Premium Item Hack, Set Item Effect Hack, Ore Mining Hack,
//         Embellish Storage Check, Race Item Enforcement
// ============================================================================

#include <map>
#include <set>
#include <mutex>

class CPlayer;
class CLogFile;

// ---- Ore Grade Limits ----
struct OreGradeLimit
{
    int maxOreLevel;         // max ore level player can mine
    int playerLevelRequired; // min player level for this ore grade
};

// ---- Item Integrity Config ----
struct ItemIntegrityConfig
{
    // Premium item hack prevention
    bool validatePremiumItems;      // server-side check premium item ownership
    bool validateCashItemUsage;     // validate cash item before use

    // Set item effect hack prevention
    bool validateSetItemEffects;    // verify set item effects server-side
    int  maxSetItemEffects;         // max simultaneous set effects (default 6)

    // Ore mining hack
    bool validateOreGrade;          // check ore grade vs player level
    int  maxOreGrade;               // absolute max ore grade (0-3, where 3=+4)
    std::vector<OreGradeLimit> oreGradeLimits;

    // Embellish (socket/upgrade) storage
    bool validateEmbellishStorage;  // check embellish data integrity

    ItemIntegrityConfig()
        : validatePremiumItems(true)
        , validateCashItemUsage(true)
        , validateSetItemEffects(true)
        , maxSetItemEffects(6)
        , validateOreGrade(true)
        , maxOreGrade(2)            // max +3 ore (index 0-2), +4 blocked
        , validateEmbellishStorage(true)
    {}
};

class ItemIntegrity
{
public:
    static ItemIntegrity& Instance()
    {
        static ItemIntegrity inst;
        return inst;
    }

    void Initialize();
    void Reload();

    // ---- Premium Item Validation ----

    // Validate that a premium/cash item is legitimately owned
    // Check: item exists in cash DB, not expired, correct account
    bool ValidatePremiumItem(CPlayer* pPlayer, unsigned short itemSerial,
                             const char* itemCode);

    // Validate cash item usage (prevent using items not purchased)
    bool ValidateCashItemUse(CPlayer* pPlayer, unsigned short itemSerial);

    // ---- Set Item Effect Validation ----

    // Validate set item effect is legitimate
    // Prevents fake set effects by verifying all required items are equipped
    bool ValidateSetItemEffect(CPlayer* pPlayer, unsigned int setItemId,
                                unsigned char setItemNum, unsigned char setEffectNum);

    // Count active set effects (prevent exceeding max)
    bool CanApplySetEffect(CPlayer* pPlayer);

    // ---- Ore Mining Validation ----

    // Validate ore grade before mining result
    // The +4 ore hack sends fake ore index to get higher grade ore
    bool ValidateOreGrade(CPlayer* pPlayer, int oreIndex, int oreLevel);

    // Validate ore cutting result
    bool ValidateOreCutting(CPlayer* pPlayer, int resultOreLevel);

    // ---- Embellish (Socket) Storage Validation ----

    // Validate embellish data integrity on item
    bool ValidateEmbellishData(CPlayer* pPlayer, unsigned char storageIndex,
                                int embellishSlot);

    // Validate item grade/upgrade level is within bounds
    bool ValidateItemGrade(unsigned char grade, unsigned char maxGrade = 7);

    const ItemIntegrityConfig& GetConfig() const { return m_config; }

private:
    ItemIntegrity() : m_pLog(nullptr) {}

    ItemIntegrityConfig m_config;
    CLogFile* m_pLog;
    mutable std::mutex m_mutex;

    // Track active set effects per player
    std::map<unsigned int, int> m_activeSetEffects; // serial -> count
};
