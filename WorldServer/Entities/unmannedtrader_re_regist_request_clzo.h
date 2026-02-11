#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _unmannedtrader_re_regist_request_clzo
{
  struct __list
  {
    bool bRegist;
    unsigned __int16 wItemSerial;
    unsigned __int8 byAmount;
    unsigned __int8 byItemTableCode;
    unsigned __int16 wItemIndex;
    unsigned int dwPrice;
    unsigned int dwRegistSerial;
  };

  unsigned __int8 byTaxRate;
  unsigned __int8 byRegedNum;
  __list List[10];
};
#pragma pack(pop)
