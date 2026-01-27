#pragma once

#include "IdaCompat.h"

struct _store_dummy;
struct _StoreList_fld;
struct _good_storage_info;
struct _limit_item_info;
class CMapData;

/* 3033 */
class __cppobj __declspec(align(8)) CItemStore
{
public:
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
