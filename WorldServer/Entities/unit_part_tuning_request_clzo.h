#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _tuning_data
{
  unsigned __int8 byPartCode;
  unsigned __int8 byPartIndex;
};

struct __cppobj __unaligned __declspec(align(2)) _unit_part_tuning_request_clzo
{
  unsigned __int8 bySlotIndex;
  unsigned __int8 byTuningNum;
  int bUseNPCLinkIntem;
  _tuning_data TuningList[6];
};
#pragma pack(pop)
