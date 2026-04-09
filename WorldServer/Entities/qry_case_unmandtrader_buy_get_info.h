#pragma once

#include "IdaCompat.h"

#pragma pack(push, 8)

struct _qry_case_unmandtrader_buy_get_info
{
  struct __list
  {
    unsigned int dwRegistSerial;
    unsigned int dwPrice;
    unsigned int dwSeller;
    unsigned __int8 byRaceSexCode;
    unsigned int dwDalant;
    unsigned int dwGuildSerial;
    unsigned __int8 byUserGrade;
    unsigned int dwAccountSerial;
    char szAccountID[13];
    char wszName[17];
    unsigned int dwTax;
    unsigned __int8 byProcRet;
  };

  unsigned __int16 wInx;
  unsigned int dwBuyer;
  unsigned __int8 byRace;
  unsigned __int8 byUserGrade;
  unsigned __int8 byDivision;
  unsigned __int8 byClass;
  unsigned __int8 bySubClass;
  unsigned __int8 byType;
  unsigned __int8 byNum;
  __list List[10];

  int size() const
  {
    return sizeof(*this);
  }
};

static_assert(sizeof(_qry_case_unmandtrader_buy_get_info::__list) == 72);
static_assert(sizeof(_qry_case_unmandtrader_buy_get_info) == 736);

#pragma pack(pop)
