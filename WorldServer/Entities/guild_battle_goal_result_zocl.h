#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _guild_battle_goal_result_zocl
{
  unsigned __int8 byRet;
  char wszGuildName[17];
  unsigned int dwObjSerial;
  char wszCharName[17];
  unsigned int dwLeftRedGoalCnt;
  unsigned int dwLeftRedScore;
  unsigned int dwRightBlueGoalCnt;
  unsigned int dwRightBlueScore;
  unsigned __int8 byLeftHour;
  unsigned __int8 byLeftMin;
  unsigned __int8 byLeftSec;

  unsigned __int16 size() const
  {
    return static_cast<unsigned __int16>(sizeof(_guild_battle_goal_result_zocl));
  }
};
#pragma pack(pop)

static_assert(sizeof(_guild_battle_goal_result_zocl) == 58, "guild battle goal packet size mismatch");

