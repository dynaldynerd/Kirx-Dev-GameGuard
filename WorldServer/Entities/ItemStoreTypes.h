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

static_assert(sizeof(_good_storage_info) == 0x30, "_good_storage_info size must match IDA (0x30)");
static_assert(offsetof(_good_storage_info, byItemTableCode) == 0x00, "_good_storage_info.byItemTableCode offset mismatch");
static_assert(offsetof(_good_storage_info, wItemIndex) == 0x02, "_good_storage_info.wItemIndex offset mismatch");
static_assert(offsetof(_good_storage_info, bExist) == 0x04, "_good_storage_info.bExist offset mismatch");
static_assert(offsetof(_good_storage_info, byMoneyUnit) == 0x08, "_good_storage_info.byMoneyUnit offset mismatch");
static_assert(offsetof(_good_storage_info, nStdPrice) == 0x0C, "_good_storage_info.nStdPrice offset mismatch");
static_assert(offsetof(_good_storage_info, nStdPoint) == 0x10, "_good_storage_info.nStdPoint offset mismatch");
static_assert(offsetof(_good_storage_info, nGoldPoint) == 0x14, "_good_storage_info.nGoldPoint offset mismatch");
static_assert(offsetof(_good_storage_info, nKillPoint) == 0x18, "_good_storage_info.nKillPoint offset mismatch");
static_assert(offsetof(_good_storage_info, nResPoint) == 0x1C, "_good_storage_info.nResPoint offset mismatch");
static_assert(offsetof(_good_storage_info, dwDurPoint) == 0x20, "_good_storage_info.dwDurPoint offset mismatch");
static_assert(offsetof(_good_storage_info, dwUpCode) == 0x24, "_good_storage_info.dwUpCode offset mismatch");
static_assert(offsetof(_good_storage_info, byType) == 0x28, "_good_storage_info.byType offset mismatch");
static_assert(offsetof(_good_storage_info, dwLimitIndex) == 0x2C, "_good_storage_info.dwLimitIndex offset mismatch");

static_assert(sizeof(_limit_item_info) == 0x10, "_limit_item_info size must match IDA (0x10)");
static_assert(offsetof(_limit_item_info, bLoad) == 0x00, "_limit_item_info.bLoad offset mismatch");
static_assert(offsetof(_limit_item_info, dwStorageIndex) == 0x04, "_limit_item_info.dwStorageIndex offset mismatch");
static_assert(offsetof(_limit_item_info, nLimitNum) == 0x08, "_limit_item_info.nLimitNum offset mismatch");
static_assert(offsetof(_limit_item_info, Key) == 0x0C, "_limit_item_info.Key offset mismatch");
