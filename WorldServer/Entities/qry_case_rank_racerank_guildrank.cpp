#include "pch.h"

#include "qry_case_rank_racerank_guildrank.h"

_qry_case_rank_racerank_guildrank::_qry_case_rank_racerank_guildrank()
{
  scProcRet = 0;
  dwParam1 = 0;
  dwParam2 = 0;
  szDate[0] = 0;
}

void _qry_case_rank_racerank_guildrank::ClearRetParam()
{
  scProcRet = 0;
  dwParam1 = 0;
  dwParam2 = 0;
}
