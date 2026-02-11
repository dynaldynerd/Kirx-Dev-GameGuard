#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _alter_window_info_request_clzo
{
  unsigned int dwSkill[2];
  unsigned int dwForce[2];
  unsigned int dwCharacter[2];
  unsigned int dwAnimus[2];
  unsigned int dwInven;
  unsigned int dwInvenBag[5];
};
#pragma pack(pop)
