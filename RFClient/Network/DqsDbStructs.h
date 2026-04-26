#pragma once

#include <cstring>

#include <windows.h>

#pragma pack(push, 1)
struct _NOT_ARRANGED_AVATOR_DB
{
  unsigned int dwSerial;
  unsigned __int8 byLv;
  unsigned __int8 byRaceSexCode;
  unsigned int dwDalant;
  unsigned int dwGold;
  char wszName[17];
  char szServer[33];
  char szClassCode[5];

  _NOT_ARRANGED_AVATOR_DB();
  void Init();
};
#pragma pack(pop)

inline _NOT_ARRANGED_AVATOR_DB::_NOT_ARRANGED_AVATOR_DB()
{
  Init();
}

inline void _NOT_ARRANGED_AVATOR_DB::Init()
{
  std::memset(this, 0, sizeof(_NOT_ARRANGED_AVATOR_DB));
  dwSerial = static_cast<unsigned int>(-1);
}

static_assert(sizeof(_NOT_ARRANGED_AVATOR_DB) == 69, "_NOT_ARRANGED_AVATOR_DB packing mismatch");
