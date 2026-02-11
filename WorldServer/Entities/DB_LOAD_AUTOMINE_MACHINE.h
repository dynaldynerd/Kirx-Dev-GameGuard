#pragma once

#include "IdaCompat.h"
#include "InvenKey.h"

struct _AUTOMINE_SLOT
{
  int nLumpIndex;
  _INVENKEY item;
  int nOverlapNum;
};

struct __declspec(align(1)) _DB_LOAD_AUTOMINE_MACHINE
{
  unsigned __int8 byCollisionType;
  unsigned __int8 byRace;
  unsigned int dwGuildSerial;
  unsigned int dwGMasterSerial;
  bool bWorking;
  unsigned __int8 bySelectedOre;
  unsigned int dwBatteryGage;
  unsigned __int8 bySlotCnt;
  _AUTOMINE_SLOT slot[80];

  _DB_LOAD_AUTOMINE_MACHINE();
  __int64 size();
};
