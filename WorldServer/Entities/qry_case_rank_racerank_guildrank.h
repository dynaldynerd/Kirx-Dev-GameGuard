#pragma once

#include "IdaCompat.h"

struct  _qry_case_rank_racerank_guildrank
{
  char szDate[9];
  char scProcRet;
  unsigned int dwParam1;
  unsigned int dwParam2;

  _qry_case_rank_racerank_guildrank();
  void ClearRetParam();
};
