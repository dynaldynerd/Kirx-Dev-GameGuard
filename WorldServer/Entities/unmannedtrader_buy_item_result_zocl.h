#pragma once

#include "IdaCompat.h"

struct __unaligned __declspec(align(2)) _unmannedtrader_buy_item_result_zocl
{
  struct __unaligned __declspec(align(2)) __list
  {
    unsigned __int8 byRet;
    unsigned int dwPrice;
    unsigned __int16 dwNewItemSerial;
    unsigned __int8 byItemTableCode;
    unsigned __int16 wItemIndex;
    unsigned __int64 dwDur;
    unsigned int dwLv;
  };

  unsigned __int8 byRetCode;
  unsigned int dwPayDalant;
  unsigned int dwLeftDalant;
  unsigned __int8 byNum;
  __list List[10];

  _unmannedtrader_buy_item_result_zocl();
  __int64 size() const;
};

inline _unmannedtrader_buy_item_result_zocl::_unmannedtrader_buy_item_result_zocl()
{
  byRetCode = static_cast<unsigned __int8>(-1);
  byNum = 0;
}

inline __int64 _unmannedtrader_buy_item_result_zocl::size() const
{
  if (byRetCode && byRetCode != 0xFF)
  {
    return 1;
  }
  if (byNum <= 0xAu)
  {
    return 230 - 22LL * (10 - byNum);
  }
  return 0;
}
