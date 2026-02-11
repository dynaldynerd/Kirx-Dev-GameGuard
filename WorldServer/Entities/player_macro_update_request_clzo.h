#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(2)) _player_macro_update_request_clzo
{
  unsigned int potion[3];
  unsigned int potionvalue[3];
  unsigned int behavior[30];
  char chatting[2][5][81];
};
#pragma pack(pop)
