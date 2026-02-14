#pragma once

#include "IdaCompat.h"

struct _qry_case_guild_battel_result_log
{
  char szStartTime[17];
  char szEndTime[17];
  unsigned int dwRedSerial;
  char wszRedName[17];
  unsigned int dwBlueSerial;
  char wszBlueName[17];
  unsigned int dwRedScore;
  unsigned int dwBlueScore;
  unsigned int dwRedMaxJoinCnt;
  unsigned int dwBlueMaxJoinCnt;
  unsigned int dwRedGoalCnt;
  unsigned int dwBlueGoalCnt;
  unsigned int dwRedKillCntSum;
  unsigned int dwBlueKillCntSum;
  unsigned __int8 byBattleResult;
  unsigned int dwMaxGoalCharacSerial;
  char wszMaxGoalCharacName[17];
  unsigned int dwMaxKillCharacSerial;
  char wszMaxKillCharacName[17];
  unsigned __int8 byJoinLimit;
  unsigned int dwGuildBattleCostGold;
  char szBattleMapCode[12];
};
