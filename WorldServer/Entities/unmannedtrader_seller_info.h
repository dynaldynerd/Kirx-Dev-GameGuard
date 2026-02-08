#pragma once

#include "IdaCompat.h"

struct _unmannedtrader_seller_info
{
  unsigned int dwPrice;
  unsigned int dwSeller;
  unsigned __int8 byRaceSexCode;
  unsigned int dwDalant;
  unsigned int dwGuildSerial;
  unsigned int dwAccountSerial;
  char szAccountID[13];
  char wszName[17];
  unsigned int dwTax;
};
