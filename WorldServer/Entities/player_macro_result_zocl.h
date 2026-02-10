#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(2)) _player_macro_result_zocl
{
  unsigned int potion[3];
  unsigned int potionvalue[3];
  unsigned int behavior[30];
  char chatting[10][81];
};
#pragma pack(pop)
