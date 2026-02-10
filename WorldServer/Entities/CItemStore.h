#pragma once

#include "IdaCompat.h"
#include "ItemStoreTypes.h"
#include "buy_offer.h"
#include "limit_amount_info.h"
#include "sell_offer.h"

struct _base_fld;
struct _store_dummy;
struct _StoreList_fld;
class CMapData;

/* 3033 */
class __cppobj __declspec(align(8)) CItemStore
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
