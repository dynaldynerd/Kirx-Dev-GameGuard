#pragma once

#include "IdaCompat.h"
#include "InvenKey.h"
#include <cstddef>

struct  _good_storage_info
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

struct  _limit_item_info
{
  bool bLoad;
  unsigned int dwStorageIndex;
  int nLimitNum;
  _INVENKEY Key;

  void init();
  _limit_item_info();
};

static_assert(sizeof(_good_storage_info) == 48, "_good_storage_info size must match IDA (48)");
static_assert(offsetof(_good_storage_info, byItemTableCode) == 0, "_good_storage_info.byItemTableCode offset mismatch");
static_assert(offsetof(_good_storage_info, wItemIndex) == 2, "_good_storage_info.wItemIndex offset mismatch");
static_assert(offsetof(_good_storage_info, bExist) == 4, "_good_storage_info.bExist offset mismatch");
static_assert(offsetof(_good_storage_info, byMoneyUnit) == 8, "_good_storage_info.byMoneyUnit offset mismatch");
static_assert(offsetof(_good_storage_info, nStdPrice) == 12, "_good_storage_info.nStdPrice offset mismatch");
static_assert(offsetof(_good_storage_info, nStdPoint) == 16, "_good_storage_info.nStdPoint offset mismatch");
static_assert(offsetof(_good_storage_info, nGoldPoint) == 20, "_good_storage_info.nGoldPoint offset mismatch");
static_assert(offsetof(_good_storage_info, nKillPoint) == 24, "_good_storage_info.nKillPoint offset mismatch");
static_assert(offsetof(_good_storage_info, nResPoint) == 28, "_good_storage_info.nResPoint offset mismatch");
static_assert(offsetof(_good_storage_info, dwDurPoint) == 32, "_good_storage_info.dwDurPoint offset mismatch");
static_assert(offsetof(_good_storage_info, dwUpCode) == 36, "_good_storage_info.dwUpCode offset mismatch");
static_assert(offsetof(_good_storage_info, byType) == 40, "_good_storage_info.byType offset mismatch");
static_assert(offsetof(_good_storage_info, dwLimitIndex) == 44, "_good_storage_info.dwLimitIndex offset mismatch");

static_assert(sizeof(_limit_item_info) == 16, "_limit_item_info size must match IDA (16)");
static_assert(offsetof(_limit_item_info, bLoad) == 0, "_limit_item_info.bLoad offset mismatch");
static_assert(offsetof(_limit_item_info, dwStorageIndex) == 4, "_limit_item_info.dwStorageIndex offset mismatch");
static_assert(offsetof(_limit_item_info, nLimitNum) == 8, "_limit_item_info.nLimitNum offset mismatch");
static_assert(offsetof(_limit_item_info, Key) == 12, "_limit_item_info.Key offset mismatch");
