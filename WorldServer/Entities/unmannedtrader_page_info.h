#pragma once

#include "IdaCompat.h"

struct _unmannedtrader_page_info
{
  struct __list
  {
    unsigned int dwSerial;
    unsigned int dwK;
    unsigned __int64 dwD;
    unsigned int dwU;
    unsigned int dwPrice;
    unsigned int dwOwner;
    char wszOwnerName[17];
    __int64 tRegdate;
    unsigned __int8 bySellturm;
    unsigned int dwT;
    unsigned __int64 lnUID;
  };

  unsigned int dwCnt;
  __list list[10];
};
