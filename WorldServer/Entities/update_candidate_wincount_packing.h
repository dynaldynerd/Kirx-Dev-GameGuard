#pragma once

#include "IdaCompat.h"

struct  _update_candidate_wincount_packing
{
  unsigned __int8 byRace;
  unsigned int dwWinCnt;
  char wszdName[17];

  _update_candidate_wincount_packing();
};

inline _update_candidate_wincount_packing::_update_candidate_wincount_packing()
{
  std::memset(this, 0, sizeof(*this));
}

