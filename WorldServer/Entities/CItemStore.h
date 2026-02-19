#pragma once

#include "IdaCompat.h"
#include "ItemStoreTypes.h"
#include "buy_offer.h"
#include "limit_amount_info.h"
#include "sell_offer.h"
#include <cstddef>

struct _base_fld;
struct _store_dummy;
struct _StoreList_fld;
class CMapData;

/* 3033 */
class  CItemStore
{
public:
  CItemStore();
  ~CItemStore();

  bool Init(int nIndex, CMapData *pExistMap, _store_dummy *pDum, _StoreList_fld *pRec);
  _base_fld *GetNpcRecord();
  char *GetNpcCode();
  bool GetNpcRaceCode(unsigned __int8 *pbyRaceCode);
  float *GetStorePos();
  _limit_item_info *GetLimitItem(int nIndex);
  void InitLimitItemInfo();
  void GetLimitItemAmount(_limit_amount_info *pAmountInfo);
  unsigned __int8 IsSell(
    unsigned __int8 byOfferNum,
    _buy_offer *pOffer,
    unsigned int dwHasDalant,
    unsigned int dwHasGold,
    double dHasPoint,
    unsigned int *dwHasActPoint,
    unsigned __int8 *pbyActCode,
    float fDiscountRate,
    unsigned __int8 byRace,
    unsigned __int8 byPvpGrade);
  unsigned __int8 IsBuy(
    unsigned __int8 byOfferNum,
    _sell_offer *pOffer,
    float fDiscountRate,
    unsigned __int8 byRace);
  float CalcBuyPrice(unsigned __int8 byTableCode, unsigned __int16 wItemIndex, unsigned __int8 *pbyMoneyUnit);
  unsigned int CalcSellPrice(int nGoodIndex, unsigned __int8 *pbyMoneyUnit);
  void SetZeroTradeMoney();
  unsigned int GetLastTradeDalant();
  unsigned int GetLastTradeGold();
  unsigned int GetLastTradePoint();
  unsigned int GetLastTradeActPoint(int nActPoint);
  unsigned int CalcSecIndex(float x, float z);
  void SubLimitItemNum(int nLimitItemIndex, int nSubNum);
  void UpdateLimitItemNum(bool bUpdate);
  void SetLimitItemInitTime();

  bool m_bLive;
  int m_nIndex;
  unsigned int m_dwSecIndex;
  unsigned __int8 m_byNpcRaceCode;
  CMapData *m_pExistMap;
  _store_dummy *m_pDum;
  _StoreList_fld *m_pRec;
  int m_nStorageItemNum;
  _good_storage_info *m_pStorageItem;
  unsigned __int64 m_dwLimitInitTime;
  bool m_bDBDataCheck;
  unsigned int m_dwDBSerial;
  int m_nLimitStorageItemNum;
  _limit_item_info *m_pLimitStorageItem;
  bool m_bUpdate;
  unsigned int m_dwLastTradeDalant;
  unsigned int m_dwLastTradeGold;
  unsigned int m_dwLastTradePoint;
  unsigned int m_dwLastTradeActPoint[3];
};

static_assert(sizeof(CItemStore) == 0x78, "CItemStore size must match IDA (0x78)");
static_assert(alignof(CItemStore) == 8, "CItemStore alignment must match IDA (8)");
static_assert(offsetof(CItemStore, m_bLive) == 0x00, "CItemStore.m_bLive offset mismatch");
static_assert(offsetof(CItemStore, m_nIndex) == 0x04, "CItemStore.m_nIndex offset mismatch");
static_assert(offsetof(CItemStore, m_dwSecIndex) == 0x08, "CItemStore.m_dwSecIndex offset mismatch");
static_assert(offsetof(CItemStore, m_byNpcRaceCode) == 0x0C, "CItemStore.m_byNpcRaceCode offset mismatch");
static_assert(offsetof(CItemStore, m_pExistMap) == 0x10, "CItemStore.m_pExistMap offset mismatch");
static_assert(offsetof(CItemStore, m_pDum) == 0x18, "CItemStore.m_pDum offset mismatch");
static_assert(offsetof(CItemStore, m_pRec) == 0x20, "CItemStore.m_pRec offset mismatch");
static_assert(offsetof(CItemStore, m_nStorageItemNum) == 0x28, "CItemStore.m_nStorageItemNum offset mismatch");
static_assert(offsetof(CItemStore, m_pStorageItem) == 0x30, "CItemStore.m_pStorageItem offset mismatch");
static_assert(offsetof(CItemStore, m_dwLimitInitTime) == 0x38, "CItemStore.m_dwLimitInitTime offset mismatch");
static_assert(offsetof(CItemStore, m_bDBDataCheck) == 0x40, "CItemStore.m_bDBDataCheck offset mismatch");
static_assert(offsetof(CItemStore, m_dwDBSerial) == 0x44, "CItemStore.m_dwDBSerial offset mismatch");
static_assert(offsetof(CItemStore, m_nLimitStorageItemNum) == 0x48, "CItemStore.m_nLimitStorageItemNum offset mismatch");
static_assert(offsetof(CItemStore, m_pLimitStorageItem) == 0x50, "CItemStore.m_pLimitStorageItem offset mismatch");
static_assert(offsetof(CItemStore, m_bUpdate) == 0x58, "CItemStore.m_bUpdate offset mismatch");
static_assert(offsetof(CItemStore, m_dwLastTradeDalant) == 0x5C, "CItemStore.m_dwLastTradeDalant offset mismatch");
static_assert(offsetof(CItemStore, m_dwLastTradeGold) == 0x60, "CItemStore.m_dwLastTradeGold offset mismatch");
static_assert(offsetof(CItemStore, m_dwLastTradePoint) == 0x64, "CItemStore.m_dwLastTradePoint offset mismatch");
static_assert(offsetof(CItemStore, m_dwLastTradeActPoint) == 0x68, "CItemStore.m_dwLastTradeActPoint offset mismatch");

