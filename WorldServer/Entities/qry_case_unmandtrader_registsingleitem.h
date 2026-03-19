#pragma once

#include "IdaCompat.h"

struct __declspec(align(8)) _qry_case_unmandtrader_registsingleitem
{
  unsigned __int16 wInx;
  unsigned __int16 wItemSerialOld;
  unsigned int dwLeftDalant;
  unsigned __int16 wSepaSerial;
  unsigned __int8 bySepaAmount;
  unsigned int dwListIndex;
  unsigned __int8 byOldStorageIndex;
  unsigned __int8 byStorageIndex;
  unsigned int dwRegedSerial;
  bool bInserted;
  unsigned __int8 byType;
  unsigned __int8 bySellTurm;
  unsigned __int8 byRace;
  unsigned int dwOwnerSerial;
  unsigned int dwPrice;
  unsigned __int8 byInveninx;
  unsigned int dwK;
  unsigned __int64 dwD;
  unsigned int dwU;
  unsigned __int8 byLv;
  unsigned __int8 byGrade;
  unsigned __int8 byClass1;
  unsigned __int8 byClass2;
  unsigned __int8 byClass3;
  unsigned int dwT;
  unsigned __int64 lnUID;
  unsigned int dwTax;

  int size() const
  {
    return sizeof(*this);
  }
};

static_assert(sizeof(_qry_case_unmandtrader_registsingleitem) == 88);
