#pragma once
// ============================================================================
// AntiDupe.h - Anti-Duplication Protection Module
// Covers: Trade Dupe, UnmannedTrader Dupe, Trunk Swap Dupe, Post/Mail Dupe
// ============================================================================

#include <map>
#include <set>
#include <mutex>
#include <string>

class CPlayer;
class CLogFile;

// ---- Transaction lock states ----
enum class TransactionType
{
    TRADE,
    TRUNK,
    POST,
    UNMANNED_TRADER,
    COMBINE,
    STORE
};

// ---- Per-player transaction lock ----
struct PlayerTransactionLock
{
    bool inTrade;
    bool inTrunk;
    bool inPost;
    bool inUnmannedTrader;
    bool inCombine;
    bool inStore;
    unsigned int tradeStartTime;
    unsigned int trunkStartTime;
    unsigned int lastTransactionTime;
    unsigned int transactionDelayMs;     // minimum delay between transactions

    PlayerTransactionLock()
        : inTrade(false), inTrunk(false), inPost(false)
        , inUnmannedTrader(false), inCombine(false), inStore(false)
        , tradeStartTime(0), trunkStartTime(0)
        , lastTransactionTime(0), transactionDelayMs(1000)
    {}
};

// ---- Anti-Dupe Config ----
struct AntiDupeConfig
{
    // Transaction delay (prevents rapid-fire exploits)
    unsigned int transactionDelayMs;  // from Player.ini DELAY_TRANSACTION * 1000

    // Trade protection
    bool tradeEnabled;
    int  tradeMaxItemsPerTransaction;

    // Trunk protection
    bool trunkSwapLock;

    // Post/Mail protection
    bool postEnabled;       // can be disabled entirely
    bool postDupeCheck;

    // UnmannedTrader protection
    bool umtEnabled;
    int  umtMaxBuyCount;    // max items per purchase (1 = safest)

    AntiDupeConfig()
        : transactionDelayMs(60000)
        , tradeEnabled(true)
        , tradeMaxItemsPerTransaction(8)
        , trunkSwapLock(true)
        , postEnabled(false)
        , postDupeCheck(true)
        , umtEnabled(true)
        , umtMaxBuyCount(1)
    {}
};

class AntiDupe
{
public:
    static AntiDupe& Instance()
    {
        static AntiDupe inst;
        return inst;
    }

    void Initialize();
    void Reload();

    // ---- Transaction Lock System ----

    // Try to acquire a transaction lock. Returns false if player is already
    // in another transaction (prevents overlapping operations).
    bool TryLockTransaction(unsigned int playerSerial, TransactionType type);

    // Release a transaction lock.
    void UnlockTransaction(unsigned int playerSerial, TransactionType type);

    // Check if player is locked in any transaction.
    bool IsInTransaction(unsigned int playerSerial) const;

    // Check transaction delay (anti-rapid-fire).
    // Returns false if too soon since last transaction.
    bool CheckTransactionDelay(unsigned int playerSerial);

    // ---- Item Integrity Checks ----

    // Validate that an item exists in the claimed storage slot before trade.
    // Must be called server-side, not trusting client data.
    bool ValidateItemInSlot(CPlayer* pPlayer, unsigned char storageCode,
                            unsigned short itemSerial);

    // Validate trunk swap: ensure both source and dest items exist.
    bool ValidateTrunkSwap(CPlayer* pPlayer,
                           unsigned short srcSerial, unsigned short dstSerial);

    // Validate post item: ensure item exists and is not locked.
    bool ValidatePostItem(CPlayer* pPlayer, unsigned short itemSerial);

    // Validate UMT purchase: enforce single-item buy limit.
    bool ValidateUMTBuy(unsigned int playerSerial, int itemCount);

    // ---- Player lifecycle ----
    void OnPlayerEnterWorld(unsigned int playerSerial);
    void OnPlayerLeaveWorld(unsigned int playerSerial);

    const AntiDupeConfig& GetConfig() const { return m_config; }

private:
    AntiDupe();
    ~AntiDupe() {}

    std::map<unsigned int, PlayerTransactionLock> m_locks;
    mutable std::mutex m_mutex;
    AntiDupeConfig m_config;
    CLogFile* m_pLog;
};
