#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _holy_quest_report_wrac
{
  unsigned __int8 byRaceCode;
  char wszCharName[17];
  unsigned __int8 byDestroyedRaceCode;

  unsigned __int16 size() const
  {
    return static_cast<unsigned __int16>(sizeof(_holy_quest_report_wrac));
  }
};
#pragma pack(pop)
