#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"

#pragma pack(push, 8)
struct  _qry_sheet_load
{
  unsigned int dwAvatorSerial;
  _AVATOR_DATA LoadData;
  bool bAddItem[100];
  unsigned int dwAddDalant;
  unsigned int dwAddGold;
  unsigned int dwCheckSum;
  bool bTrunkAddItem[100];
  unsigned __int8 byTrunkOldSlot;
  long double dTrunkOldDalant;
  long double dTrunkOldGold;
  bool bCreateTrunkFree;
  unsigned int dwRefineDate;
  unsigned __int8 byRefinedCnt;
  bool bExtTrunkAddItem[40];
  unsigned __int8 byExtTrunkOldSlot;

  _qry_sheet_load();
  ~_qry_sheet_load();
  __int64 size() const;
};
#pragma pack(pop)

inline _qry_sheet_load::_qry_sheet_load()
{
  memset_0(this->bAddItem, 0, sizeof(this->bAddItem));
  this->dwAddDalant = 0;
  memset_0(this->bTrunkAddItem, 0, sizeof(this->bTrunkAddItem));
  this->bCreateTrunkFree = false;
  memset_0(this->bExtTrunkAddItem, 0, sizeof(this->bExtTrunkAddItem));
}

inline _qry_sheet_load::~_qry_sheet_load() = default;

inline __int64 _qry_sheet_load::size() const
{
  return 37512LL;
}

