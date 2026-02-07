#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _EXIT_ALTER_PARAM
{
  unsigned int dwHP;
  unsigned int dwFP;
  unsigned int dwSP;
  unsigned int dwDP;
  long double dExp;
  unsigned __int8 byMapCode;
  float fStartPos[3];
  unsigned int dwDalant;
  unsigned int dwGold;
};
#pragma pack(pop)
