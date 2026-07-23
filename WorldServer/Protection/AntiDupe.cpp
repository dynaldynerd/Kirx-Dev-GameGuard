#include "AntiDupe.h"
#include "ProtectConfig.h"
#include "../Entities/CLogFile.h"
#include "../Entities/CPlayer.h"

// ============================================================================
// AntiDupe.cpp - Anti-Duplication Protection Implementation
// ============================================================================

AntiDupe::AntiDupe()
    : m_pLog(nullptr)
{
}

void AntiDupe::Initialize()
{
    static CLogFile s_dupeLog;
    s_dupeLog.SetWriteLogFile("Protection_AntiDupe", 1, false, true, true);
    m_pLog = &s_dupeLog;

    Reload();
    m_pLog->Write("[AntiDupe] Initialized successfully");
}

void AntiDupe::Reload()
{
    auto& cfg = ProtectConfig::Instance();

    // Transaction delay (Player.ini DELAY_TRANSACTION is in seconds)
    int delaySec = cfg.GetInt("player.ini", "player", "delay_transaction", 60);
    m_config.transactionDelayMs = delaySec * 1000;

    // Post/Mail (System.ini)
    m_config.postEnabled = cfg.GetBool("system.ini", "postmail", "enable", false);

    // UnmannedTrader (UnmannedTrader.ini)
    m_config.umtEnabled = cfg.GetBool("unmannedtrader.ini", "general", "dalantregist", true);
    m_config.umtMaxBuyCount = 1; // Hardcoded to 1 for safety (matching 6.23_Guard)

    if (m_pLog)
        m_pLog->Write("[AntiDupe] Config reloaded - TxDelay=%ums Post=%s UMT=%s",
            m_config.transactionDelayMs,
            m_config.postEnabled ? "ON" : "OFF",
            m_config.umtEnabled ? "ON" : "OFF");
}

// ---- Transaction Lock System ----

bool AntiDupe::TryLockTransaction(unsigned int playerSerial, TransactionType type)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    auto& pl = m_locks[playerSerial];

    // Check if already in ANY transaction
    if (pl.inTrade || pl.inTrunk || pl.inPost || pl.inUnmannedTrader ||
        pl.inCombine || pl.inStore)
    {
        if (m_pLog)
        {
            m_pLog->Write("[AntiDupe] BLOCKED Player Serial=%u tried %d while in another transaction",
                playerSerial, (int)type);
        }
        return false;
    }

    unsigned int now = GetTickCount();

    switch (type)
    {
    case TransactionType::TRADE:
        pl.inTrade = true;
        pl.tradeStartTime = now;
        break;
    case TransactionType::TRUNK:
        pl.inTrunk = true;
        pl.trunkStartTime = now;
        break;
    case TransactionType::POST:
        if (!m_config.postEnabled)
        {
            if (m_pLog) m_pLog->Write("[AntiDupe] Post disabled, blocked Serial=%u", playerSerial);
            return false;
        }
        pl.inPost = true;
        break;
    case TransactionType::UNMANNED_TRADER:
        pl.inUnmannedTrader = true;
        break;
    case TransactionType::COMBINE:
        pl.inCombine = true;
        break;
    case TransactionType::STORE:
        pl.inStore = true;
        break;
    }

    return true;
}

void AntiDupe::UnlockTransaction(unsigned int playerSerial, TransactionType type)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_locks.find(playerSerial);
    if (it == m_locks.end()) return;

    auto& pl = it->second;
    unsigned int now = GetTickCount();

    switch (type)
    {
    case TransactionType::TRADE:          pl.inTrade = false; break;
    case TransactionType::TRUNK:          pl.inTrunk = false; break;
    case TransactionType::POST:           pl.inPost = false; break;
    case TransactionType::UNMANNED_TRADER:pl.inUnmannedTrader = false; break;
    case TransactionType::COMBINE:        pl.inCombine = false; break;
    case TransactionType::STORE:          pl.inStore = false; break;
    }

    pl.lastTransactionTime = now;
}

bool AntiDupe::IsInTransaction(unsigned int playerSerial) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_locks.find(playerSerial);
    if (it == m_locks.end()) return false;

    const auto& pl = it->second;
    return (pl.inTrade || pl.inTrunk || pl.inPost ||
            pl.inUnmannedTrader || pl.inCombine || pl.inStore);
}

bool AntiDupe::CheckTransactionDelay(unsigned int playerSerial)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_locks.find(playerSerial);
    if (it == m_locks.end()) return true;

    unsigned int now = GetTickCount();
    unsigned int elapsed = now - it->second.lastTransactionTime;

    if (elapsed < m_config.transactionDelayMs)
    {
        if (m_pLog)
        {
            m_pLog->Write("[AntiDupe] Transaction too fast for Serial=%u (elapsed=%ums, min=%ums)",
                playerSerial, elapsed, m_config.transactionDelayMs);
        }
        return false;
    }
    return true;
}

// ---- Item Integrity Checks ----

bool AntiDupe::ValidateItemInSlot(CPlayer* pPlayer, unsigned char storageCode,
                                   unsigned short itemSerial)
{
    if (!pPlayer) return false;

    // Server-side validation: verify the item actually exists in the
    // player's storage at the claimed position. Do NOT trust client data.
    //
    // Integration point: Use pPlayer->m_Storage or equivalent to check:
    //   _STORAGE_LIST::_db_con* pItem = pPlayer->GetItemBySerial(storageCode, itemSerial);
    //   if (!pItem) { /* dupe attempt - item doesn't exist */ return false; }
    //
    // NOTE: The actual field/method names depend on your WorldServer version.
    // The key principle is: ALWAYS verify server-side before any item move.

    if (m_pLog)
    {
        m_pLog->Write("[AntiDupe] ValidateItemInSlot Serial=%u storage=%d item=%d",
            pPlayer->m_dwSerial, storageCode, itemSerial);
    }

    return true; // Placeholder - integrate with actual storage check
}

bool AntiDupe::ValidateTrunkSwap(CPlayer* pPlayer,
                                  unsigned short srcSerial, unsigned short dstSerial)
{
    if (!pPlayer) return false;

    if (!m_config.trunkSwapLock) return true;

    // Validate both items exist before allowing swap
    // The trunk swap dupe works by manipulating packet timing to swap
    // items that don't exist or create phantom copies.
    //
    // Integration: Check both srcSerial and dstSerial exist in trunk storage.
    // If either doesn't exist, block the operation.

    // Also ensure player is properly locked in trunk transaction
    if (!IsInTransaction(pPlayer->m_dwSerial))
    {
        if (m_pLog)
        {
            m_pLog->Write("[AntiDupe] BLOCKED trunk swap without transaction lock Serial=%u",
                pPlayer->m_dwSerial);
        }
        return false;
    }

    return true;
}

bool AntiDupe::ValidatePostItem(CPlayer* pPlayer, unsigned short itemSerial)
{
    if (!pPlayer) return false;

    if (!m_config.postEnabled)
    {
        if (m_pLog)
            m_pLog->Write("[AntiDupe] Post system disabled, blocked Serial=%u", pPlayer->m_dwSerial);
        return false;
    }

    // Validate item exists in player inventory before attaching to mail
    // The post dupe works by sending items via mail faster than the server
    // can process, creating duplicate items.
    //
    // Integration: Lock the item first, then validate existence.

    return true;
}

bool AntiDupe::ValidateUMTBuy(unsigned int playerSerial, int itemCount)
{
    if (itemCount > m_config.umtMaxBuyCount)
    {
        if (m_pLog)
        {
            m_pLog->Write("[AntiDupe] UMT buy limit exceeded Serial=%u count=%d max=%d",
                playerSerial, itemCount, m_config.umtMaxBuyCount);
        }
        return false;
    }
    return true;
}

// ---- Player lifecycle ----

void AntiDupe::OnPlayerEnterWorld(unsigned int playerSerial)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_locks[playerSerial] = PlayerTransactionLock();
    m_locks[playerSerial].transactionDelayMs = m_config.transactionDelayMs;
}

void AntiDupe::OnPlayerLeaveWorld(unsigned int playerSerial)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_locks.erase(playerSerial);
}
