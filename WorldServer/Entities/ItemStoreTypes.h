#pragma once

#include "IdaCompat.h"
#include "InvenKey.h"

struct __cppobj _good_storage_info
{
  unsigned __int8 byItemTableCode;
  unsigned __int16 wItemIndex;
  int bExist;
  unsigned __int8 byMoneyUnit;
  int nStdPrice;
  int nStdPoint;
  int nGoldPoint;
  int nKillPoint;
  int nResPoint;
  unsigned int dwDurPoint;
  unsigned int dwUpCode;
  unsigned __int8 byType;
  unsigned int dwLimitIndex;

  _good_storage_info();
};

struct __cppobj _limit_item_info
{
  bool bLoad;
  unsigned int dwStorageIndex;
  int nLimitNum;
  _INVENKEY Key;

  void init();
  _limit_item_info();
};
