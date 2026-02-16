#pragma once

#pragma pack(push, 1)
    struct _guild_battle_result_log_notify_web
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
#pragma pack(pop)

#pragma pack(push, 1)
    struct _guild_battle_result_notify
    {
      char byResult;
      char wszRedName[17];
      char wszBlueName[17];
    };
#pragma pack(pop)

#pragma pack(push, 1)
    struct _guild_battle_before_start_notify
    {
      char byLeftMin;
      char wszRedName[17];
      char wszBlueName[17];
    };
#pragma pack(pop)

#pragma pack(push, 1)
    struct _guild_battle_restart_result
    {
      char byColor;
      unsigned int dwLeftRedScore;
      unsigned int dwRightBlueScore;
      unsigned int dwLeftRedGoalCnt;
      unsigned int dwRightBlueGoalCnt;
      unsigned __int8 byLeftHour;
      unsigned __int8 byLeftMin;
      unsigned __int8 byLeftSec;
      int iRedPortalInx;
      int iBluePortalInx;
      int iRegenPortalInx[3];
    };
#pragma pack(pop)

#pragma pack(push, 1)
    struct _guild_battle_start_result
    {
      char byColor;
      unsigned int dwLeftRedScore;
      unsigned int dwRightBlueScore;
      unsigned int dwLeftRedGoalCnt;
      unsigned int dwRightBlueGoalCnt;
      unsigned __int8 byLeftHour;
      unsigned __int8 byLeftMin;
      unsigned __int8 byLeftSec;
      int iRedPortalInx;
      int iBluePortalInx;
      int iRegenPortalInx[3];
    };
#pragma pack(pop)

