#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _combine_lend_item_result_zocl
{
  struct _lend_item
  {
    unsigned __int8 byTblCode;
    __unaligned __declspec(align(1)) unsigned __int16 wItemIdx;
    __unaligned __declspec(align(1)) unsigned int dwDur;
    __unaligned __declspec(align(1)) unsigned int dwUp;
    __unaligned __declspec(align(1)) unsigned int dwItemSerial;
    unsigned __int8 byCsMethod;
    unsigned int dwT;
  };

  unsigned __int8 byErrorCode;
  unsigned int dwFee;
  unsigned int dwLeftDalant;
  _lend_item Item;

  __int64 size() const { return 29LL; }
};

struct __unaligned __declspec(align(1)) _exchange_lend_item_result_zocl
{
  unsigned __int8 byErrorCode;
  _combine_lend_item_result_zocl::_lend_item Item;

  __int64 size() const;
};
#pragma pack(pop)
