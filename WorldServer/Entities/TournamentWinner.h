#pragma once

#include "IdaCompat.h"

struct __declspec(align(4)) TournamentWinner
{
  unsigned int dwSerial;
  char wszCharName[17];
  unsigned __int8 byGrade;
};
