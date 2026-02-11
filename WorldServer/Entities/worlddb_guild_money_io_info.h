#pragma once

#include "IdaCompat.h"

struct __declspec(align(8)) _worlddb_guild_money_io_info
{
  struct __declspec(align(8)) __io_money_data
  {
    char wszIOerName[17];
    unsigned int dwIOerSerial;
    long double dIODalant;
    long double dIOGold;
    long double dLeftDalant;
    long double dLeftGold;
    unsigned __int8 byDate[4];
  };

  unsigned __int16 wRecordCount;
  __io_money_data IOData[100];
};
