#pragma once

#include "../IdaCompat.h"

// Zone -> Web/Control packet structs.

// login_zone_result_zoct.h

#pragma pack(push, 1)
struct _login_zone_result_zoct
{
  unsigned __int8 byRet;

  unsigned __int16 size() const
  {
    return 1;
  }
};
#pragma pack(pop)

// connection_status_result_zoct.h

#pragma pack(push, 1)
struct _connection_status_result_zoct
{
  unsigned __int8 bStatus[6];

  unsigned __int16 size() const
  {
    return 6;
  }
};
#pragma pack(pop)

static_assert(sizeof(_connection_status_result_zoct) == 6, "connection status packet size mismatch");

// world_onoff_notice_zowb.h

#pragma pack(push, 1)
struct _world_onoff_notice_zowb
{
  unsigned __int8 byWorldCode;
  unsigned __int8 byOnOff;
  char cServerType;
};
#pragma pack(pop)

static_assert(sizeof(_world_onoff_notice_zowb) == 3, "world on/off notice packet size mismatch");

// holy_quest_now_report_zowb.h

#pragma pack(push, 1)
struct _holy_quest_now_report_zowb
{
  unsigned __int8 byWorldCode;
  unsigned __int8 byRaceCode;
};
#pragma pack(pop)

static_assert(sizeof(_holy_quest_now_report_zowb) == 2, "holy quest now report packet size mismatch");

// notify_guild_battle_state_zowb.h

#pragma pack(push, 1)
struct _notify_guild_battle_state_zowb
{
  unsigned __int8 byTimeID;
  unsigned __int8 byRace;
  unsigned __int8 byState;
};
#pragma pack(pop)

static_assert(sizeof(_notify_guild_battle_state_zowb) == 3, "guild battle state packet size mismatch");

// money_supply_gatering_inform_zowb.h

#pragma pack(push, 1)
struct _money_supply_gatering_inform_zowb
{
  #pragma pack(push, 1)
  struct _money_supply
  {
    int nLv[4];
    int nRace[3];
    int nClass[60];
  };
  #pragma pack(pop)

  __int64 dwAmount[9];
  _money_supply ms_data[4];
  int nFeeLv[4];
  int nFeeRace[3];
  int nHonorGuildRace[2][3];
  int nUnitRepairLv[4];
  int nBuyUnitLv[4];
};
#pragma pack(pop)

static_assert(sizeof(_money_supply_gatering_inform_zowb) == 1228, "money supply packet size mismatch");

// guild_battle_complete_msg_zowb.h

#pragma pack(push, 1)
struct _guild_battle_complete_msg_zowb
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
  char byBattleResult;
  unsigned int dwMaxGoalCharacSerial;
  char wszMaxGoalCharacName[17];
  unsigned int dwMaxKillCharacSerial;
  char wszMaxKillCharacName[17];
  char byJoinLimit;
  unsigned int dwGuildBattleCost;
  char szBattleMapCode[12];
};
#pragma pack(pop)

static_assert(sizeof(_guild_battle_complete_msg_zowb) == 168, "guild battle complete packet size mismatch");

// guild_battle_reserve_msg_zowb.h

#pragma pack(push, 1)
struct _guild_battle_reserve_msg_zowb
{
  char byTimeID;
  char byRace;
  unsigned int dw1PGuildSerial;
  char by1PGuildGrade;
  char wsz1PName[17];
  unsigned int dw2PGuildSerial;
  char by2PGuildGrade;
  char wsz2PName[17];
  char byJoinLimit;
  unsigned int dwGuildBattleCost;
  char szBattleMapCode[12];
};
#pragma pack(pop)

static_assert(sizeof(_guild_battle_reserve_msg_zowb) == 63, "guild battle reserve packet size mismatch");

// economy_data_inform_zowb.h

#pragma pack(push, 1)
struct _economy_data_inform_zowb
{
  float fPayExgRate[3];
  float fTexRate[3];
  unsigned __int16 wEconomyGuide[3];
  unsigned __int16 wYear;
  char byMonth;
  char byDay;
};
#pragma pack(pop)

static_assert(sizeof(_economy_data_inform_zowb) == 34, "economy data packet size mismatch");

// confirm_sms_infom_zoct.h

#pragma pack(push, 1)
struct _confirm_sms_infom_zoct
{
  int nCountIndex;
  int nWorldCode;
  char byRaceCode;
  char wszMasterName[17];
  char wszMsg[255];
};
#pragma pack(pop)

static_assert(sizeof(_confirm_sms_infom_zoct) == 281, "confirm sms packet size mismatch");

// send_raceboss_msg_request_zowb.h

#pragma pack(push, 1)
struct _send_raceboss_msg_request_zowb
{
  int nCountIndex;
  int nWorldCode;
  char byRaceCode;
};
#pragma pack(pop)

static_assert(sizeof(_send_raceboss_msg_request_zowb) == 9, "raceboss send packet size mismatch");

// send_fromweb_raceboss_msg_request_zowb.h

#pragma pack(push, 1)
struct _send_fromweb_raceboss_msg_request_zowb
{
  unsigned int dwWebSendDBID;
  int nCountIndex;
  int nWorldCode;
  char byRaceCode;
};
#pragma pack(pop)

static_assert(sizeof(_send_fromweb_raceboss_msg_request_zowb) == 13, "raceboss send from web packet size mismatch");

// cancel_raceboss_msg_request_zowb.h

#pragma pack(push, 1)
struct _cancel_raceboss_msg_request_zowb
{
  int nCountIndex;
  int nWorldCode;
  char byRaceCode;
};
#pragma pack(pop)

static_assert(sizeof(_cancel_raceboss_msg_request_zowb) == 9, "raceboss cancel packet size mismatch");

// cancel_raceboss_msg_request_ctzo.h

#pragma pack(push, 1)
struct _cancel_raceboss_msg_request_ctzo
{
  char byRaceCode;
  int nID;
};
#pragma pack(pop)

static_assert(sizeof(_cancel_raceboss_msg_request_ctzo) == 5, "raceboss control cancel packet size mismatch");

// last_attacker_for_the_keeper_inform_zowb.h

#pragma pack(push, 1)
struct _last_attacker_for_the_keeper_inform_zowb
{
  int nWorldCode;
  char byNumOfTime;
  unsigned __int16 wStartYear;
  char byStartMonth;
  char byStartDay;
  char uszKeeperHitter[17];
  int bByAnimus;
};
#pragma pack(pop)

static_assert(sizeof(_last_attacker_for_the_keeper_inform_zowb) == 30, "keeper attacker packet size mismatch");

// holy_minetime_extend_inform_zowb.h

#pragma pack(push, 1)
struct _holy_minetime_extend_inform_zowb
{
  int nWorldCode;
  char byChaosHour;
  char byChaosMin;
  char byNumOfTime;
};
#pragma pack(pop)

static_assert(sizeof(_holy_minetime_extend_inform_zowb) == 7, "holy minetime packet size mismatch");

// holy_complete_quest_inform_zowb.h

#pragma pack(push, 1)
struct _holy_complete_quest_inform_zowb
{
  int nWorldCode;
  char szWorldName[33];
  char wszRaceBossName[3][17];
  char byMasterRaceCode;
  char byDestoryRaceCode;
  char wszMasterName[17];
  char byChaosHour;
  char byChaosMin;
  char byNumOfTime;
  char byMasterLv;
  char szMasterClass[5];
  char wszMasterRaceSubBoss[5][17];
  char byStartHour;
  char byStartMin;
  unsigned __int16 wStartYear;
  char byStartMonth;
  char byStartDay;
  char byEndHour;
  char byEndMin;
  unsigned __int16 wEndYear;
  char byEndMonth;
  char byEndDay;
};
#pragma pack(pop)

static_assert(sizeof(_holy_complete_quest_inform_zowb) == 213, "holy complete quest packet size mismatch");

// world_user_num_zowb.h

#pragma pack(push, 1)
struct _world_user_num_zowb
{
  char byWorldCode;
  unsigned __int16 wUserNum;
};
#pragma pack(pop)

static_assert(sizeof(_world_user_num_zowb) == 3, "world user num packet size mismatch");
