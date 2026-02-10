#pragma once

#include "IdaCompat.h"

struct __unaligned __declspec(align(1)) _starting_vote_inform_zocl
{
  int nVoteSerial;
  unsigned __int8 byLimGrade;
  unsigned __int16 wLeftSec;
  unsigned __int16 wContentSize;
  char wszContent[1280];

  _starting_vote_inform_zocl();
  __int64 size();
};
