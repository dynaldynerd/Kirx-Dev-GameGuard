#pragma once

#include "IdaCompat.h"

struct _qry_case_unmandtrader_buy_update_wait
{
  struct __list
  {
    unsigned int dwRegistSerial;
    unsigned int dwSeller;
    char szAccountID[13];
    unsigned int dwAccountSerial;
    char wszName[17];
    unsigned __int8 byProcRet;
    unsigned int dwTax;
    unsigned __int8 byOldState;
    unsigned __int8 byInveninx;
    unsigned int dwK;
    unsigned __int64 dwD;
    unsigned int dwU;
    unsigned int dwPrice;
    unsigned int dwT;
    unsigned __int64 lnUID;
  };

  unsigned __int16 wInx;
  unsigned int dwBuyer;
  unsigned __int8 byRace;
  unsigned __int8 byDivision;
  unsigned __int8 byClass;
  unsigned __int8 bySubClass;
  __int64 tResultTime;
  unsigned __int8 byType;
  unsigned __int8 byNum;
  __list List[10];
};

struct _qry_case_unmandtrader_buy_update_complete
{
  struct __declspec(align(4)) __list
  {
    unsigned __int8 byProcRet;
    unsigned __int8 byProcUpdate;
    unsigned int dwSeller;
    unsigned int dwRegistSerial;
    unsigned __int8 byUpdateState;
  };

  unsigned __int16 wInx;
  unsigned int dwBuyer;
  unsigned __int8 byRace;
  bool bAllSuccess;
  unsigned __int8 byType;
  unsigned __int8 byNum;
  __list List[10];
};

struct _qry_case_unmandtrader_buy_update_rollback
{
  struct __declspec(align(4)) __list
  {
    unsigned __int8 byProcRet;
    unsigned int dwRegistSerial;
    unsigned __int8 byOldState;
  };

  unsigned __int16 wInx;
  unsigned int dwBuyer;
  unsigned __int8 byRace;
  unsigned __int8 byType;
  unsigned __int8 byNum;
  __list List[10];
};
