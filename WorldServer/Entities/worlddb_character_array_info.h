#pragma once

#include "IdaCompat.h"

struct _worlddb_character_array_info
{
  struct __worlddb_charater_info
  {
    unsigned __int16 wWorldCode;
    char wszAvatorName[17];
    unsigned __int8 bySlotIndex;
    unsigned __int8 byRaceCode;
    unsigned __int8 byLevel;
    unsigned int dwSerial;
    unsigned __int8 byDck;
  };

  unsigned __int16 wCharacterCount;
  __worlddb_charater_info CharacterInfo[200];
};
