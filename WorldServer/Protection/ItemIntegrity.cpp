#include "ItemIntegrity.h"
#include "ProtectConfig.h"
#include "../Entities/CLogFile.h"
#include "../Entities/CPlayer.h"

// ============================================================================
// ItemIntegrity.cpp - Item & Equipment Integrity Implementation
// ============================================================================

void ItemIntegrity::Initialize()
{
    static CLogFile s_log;
    s_log.SetWriteLogFile("Protection_ItemIntegrity", 1, false, true, true);
    m_pLog = &s_log;

    Reload();
    m_pLog->Write("[ItemIntegrity] Initialized");
}

void ItemIntegrity::Reload()
{
    auto& cfg = ProtectConfig::Instance();

    // Ore mining config
    m_config.maxOreGrade = cfg.GetInt("player.ini", "mining", "max_ore_grade", 2);

    // Set item config
    m_config.maxSetItemEffects = cfg.GetInt("system.ini", "setitem", "max_effects", 6);

    // Ore grade limits based on player level
    m_config.oreGradeLimits.clear();
    // Default: level 1-29 = grade 0, 30-39 = grade 1, 40-49 = grade 2, 50+ = grade 2
    m_config.oreGradeLimits.push_back({0, 1});   // grade 0 from level 1
    m_config.oreGradeLimits.push_back({1, 30});  // grade 1 from level 30
    m_config.oreGradeLimits.push_back({2, 40});  // grade 2 from level 40

    if (m_pLog)
        m_pLog->Write("[ItemIntegrity] Config reloaded - MaxOreGrade=%d MaxSetEffects=%d",
            m_config.maxOreGrade, m_config.maxSetItemEffects);
}

// ---- Premium Item Validation ----

bool ItemIntegrity::ValidatePremiumItem(CPlayer* pPlayer, unsigned short itemSerial,
                                         const char* itemCode)
{
    if (!pPlayer || !m_config.validatePremiumItems) return true;

    // Server-side validation:
    // 1. Check if itemCode exists in cash shop table (tbl_cashitem)
    // 2. Verify the item was actually purchased for this account
    // 3. Check item hasn't expired
    // 4. Verify item quantity matches DB records
    //
    // Integration with WorldServer:
    //   CRFCashItemDatabase* pCashDB = ...;
    //   bool exists = pCashDB->ValidateItemOwnership(
    //       pPlayer->m_Param.GetAccountSerial(), itemCode, itemSerial);
    //
    // The hack works by modifying client memory to change item codes,
    // making non-premium items appear as premium. Server-side check
    // catches this because the item won't exist in the cash DB.

    // Check item code format validity
    if (!itemCode || strlen(itemCode) < 3 || strlen(itemCode) > 8)
    {
        if (m_pLog)
            m_pLog->Write("[PremiumItem] Invalid item code format Serial=%u code='%s'",
                pPlayer->m_dwSerial, itemCode ? itemCode : "NULL");
        return false;
    }

    // Check for commonly exploited premium item code patterns
    // Premium items in RF typically start with specific prefixes
    // If the item claims to be premium but has a non-premium code prefix,
    // it's likely a hack attempt.

    return true; // placeholder - wire to CRFCashItemDatabase
}

bool ItemIntegrity::ValidateCashItemUse(CPlayer* pPlayer, unsigned short itemSerial)
{
    if (!pPlayer || !m_config.validateCashItemUsage) return true;

    // Validate the cash item exists in player's cash inventory
    // before allowing usage. Prevents using items via packet manipulation.
    //
    // Integration:
    //   _STORAGE_LIST::_db_con* pItem = pPlayer->GetCashItem(itemSerial);
    //   if (!pItem) {
    //       m_pLog->Write("[CashItem] BLOCKED non-existent cash item use");
    //       return false;
    //   }
    //   // Also check: item not already used, correct item type, etc.

    return true; // placeholder
}

// ---- Set Item Effect Validation ----

bool ItemIntegrity::ValidateSetItemEffect(CPlayer* pPlayer, unsigned int setItemId,
                                            unsigned char setItemNum, unsigned char setEffectNum)
{
    if (!pPlayer || !m_config.validateSetItemEffects) return true;

    // The set item effect hack works by sending fake SetOnEffect packets
    // to apply set bonuses without having the required items equipped.
    //
    // Validation steps:
    // 1. Get the set item definition from CSUItemSystem
    // 2. Count how many items from this set the player actually has equipped
    // 3. Compare with setItemNum claimed in the packet
    // 4. If player doesn't have enough items, reject

    // Integration with CSetItemEffect:
    //   CSUItemSystem* su = CSUItemSystem::Instance();
    //   _SetItemEff_fld* setFld = su->GetSetItemRecord(setItemId);
    //   if (!setFld) return false;
    //
    //   int actualEquippedCount = 0;
    //   for each equipped item in pPlayer:
    //       if item belongs to setItemId: actualEquippedCount++
    //
    //   if (actualEquippedCount < setItemNum) {
    //       m_pLog->Write("[SetItem] FAKE set effect! Serial=%u claimed=%d actual=%d",
    //           pPlayer->m_dwSerial, setItemNum, actualEquippedCount);
    //       return false;
    //   }

    // Validate setEffectNum is within bounds
    if (setEffectNum > m_config.maxSetItemEffects)
    {
        if (m_pLog)
            m_pLog->Write("[SetItem] Effect num out of bounds Serial=%u effectNum=%d max=%d",
                pPlayer->m_dwSerial, setEffectNum, m_config.maxSetItemEffects);
        return false;
    }

    return true;
}

bool ItemIntegrity::CanApplySetEffect(CPlayer* pPlayer)
{
    if (!pPlayer) return false;

    std::lock_guard<std::mutex> lock(m_mutex);
    int count = m_activeSetEffects[pPlayer->m_dwSerial];
    if (count >= m_config.maxSetItemEffects)
    {
        if (m_pLog)
            m_pLog->Write("[SetItem] Max set effects reached Serial=%u count=%d",
                pPlayer->m_dwSerial, count);
        return false;
    }
    m_activeSetEffects[pPlayer->m_dwSerial] = count + 1;
    return true;
}

// ---- Ore Mining Validation ----

bool ItemIntegrity::ValidateOreGrade(CPlayer* pPlayer, int oreIndex, int oreLevel)
{
    if (!pPlayer || !m_config.validateOreGrade) return true;

    // The ore mining hack (+4 ore) works by sending a modified ore index
    // in the mining result packet, pointing to a higher-grade ore than
    // what the player should be able to mine.
    //
    // m_nOre_Level in _OreItem_fld determines ore grade:
    // 0 = normal, 1 = +1, 2 = +2, 3 = +3 (sometimes called +4 by players)

    // Check absolute max ore grade
    if (oreLevel > m_config.maxOreGrade)
    {
        if (m_pLog)
            m_pLog->Write("[OreMining] BLOCKED ore grade %d > max %d for Serial=%u oreIndex=%d",
                oreLevel, m_config.maxOreGrade, pPlayer->m_dwSerial, oreIndex);
        return false;
    }

    // Check ore grade vs player level
    int playerLevel = pPlayer->m_nLevel;
    int maxAllowedGrade = 0;

    for (const auto& limit : m_config.oreGradeLimits)
    {
        if (playerLevel >= limit.playerLevelRequired)
            maxAllowedGrade = limit.maxOreLevel;
    }

    if (oreLevel > maxAllowedGrade)
    {
        if (m_pLog)
            m_pLog->Write("[OreMining] BLOCKED ore grade %d for level %d player Serial=%u (max=%d)",
                oreLevel, playerLevel, pPlayer->m_dwSerial, maxAllowedGrade);
        return false;
    }

    // Validate ore index is within valid table range
    // RF ore items are in tblItemData[17] (OreItem table)
    // Valid indices should be within the table bounds
    if (oreIndex < 0 || oreIndex > 500) // reasonable upper bound
    {
        if (m_pLog)
            m_pLog->Write("[OreMining] Invalid ore index %d for Serial=%u",
                oreIndex, pPlayer->m_dwSerial);
        return false;
    }

    return true;
}

bool ItemIntegrity::ValidateOreCutting(CPlayer* pPlayer, int resultOreLevel)
{
    if (!pPlayer || !m_config.validateOreGrade) return true;

    // Ore cutting (processing) should not produce ore higher than max grade
    if (resultOreLevel > m_config.maxOreGrade)
    {
        if (m_pLog)
            m_pLog->Write("[OreCutting] BLOCKED result grade %d for Serial=%u",
                resultOreLevel, pPlayer->m_dwSerial);
        return false;
    }

    return true;
}

// ---- Embellish Storage Validation ----

bool ItemIntegrity::ValidateEmbellishData(CPlayer* pPlayer, unsigned char storageIndex,
                                            int embellishSlot)
{
    if (!pPlayer || !m_config.validateEmbellishStorage) return true;

    // Validate embellish (socket gem) data integrity
    // _EQUIP_DB_BASE has m_EmbellishList[7] - max 7 embellish slots
    if (embellishSlot < 0 || embellishSlot >= 7)
    {
        if (m_pLog)
            m_pLog->Write("[Embellish] Invalid slot %d for Serial=%u storage=%d",
                embellishSlot, pPlayer->m_dwSerial, storageIndex);
        return false;
    }

    // Integration:
    //   Verify the embellish item actually exists in the gem table
    //   Verify the slot is not already occupied (prevent overwrite exploit)
    //   Verify the item being socketed matches allowed gem types for slot

    return true;
}

bool ItemIntegrity::ValidateItemGrade(unsigned char grade, unsigned char maxGrade)
{
    // Item upgrade grade (enchant level) must be within bounds
    // Standard max is +7 (grade 0-7)
    return (grade <= maxGrade);
}
