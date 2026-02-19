#pragma once

#include "IdaCompat.h"

struct _qry_case_in_guildbattlecost
{
  unsigned int dwGuildIndex;
  unsigned int dwGuildSerial;
  unsigned int dwAddGold;
  unsigned int dwAddDalant;
  unsigned __int8 byDate[4];
  long double out_totalgold;
  long double out_totaldalant;

  __int64 size();
};

struct _qry_case_dest_guild_out_guildbattlecost
{
  unsigned int dwGuildIndex;
  unsigned int dwGuildSerial;
  unsigned int dwSubGold;
  unsigned int dwSubDalant;
  unsigned __int8 byDate[4];
  long double out_totalgold;
  long double out_totaldalant;
  unsigned int dwSrcGuildSerial;
  unsigned int dwStartTimeInx;
  unsigned int dwMemberCntInx;
  unsigned int dwMapInx;
  unsigned __int8 byProcRet;

  __int64 size();
};

struct _qry_case_src_guild_out_guildbattlecost
{
  unsigned int dwGuildIndex;
  unsigned int dwGuildSerial;
  unsigned int dwSubGold;
  unsigned int dwSubDalant;
  unsigned __int8 byDate[4];
  long double out_totalgold;
  long double out_totaldalant;
  unsigned int dwSrcGuildSerial;
  unsigned int dwStartTimeInx;
  unsigned int dwMemberCntInx;
  unsigned int dwMapInx;
  unsigned __int8 byProcRet;

  __int64 size();
};

struct _qry_case_addguildbattleschedule
{
  unsigned int dwID;
  unsigned int dw1PGuildSerial;
  unsigned int dw2PGuildSerial;
  unsigned int dwMapID;
  unsigned __int8 byBattleNumber;
  unsigned int dwSLID;
  unsigned __int8 byState;
  __int64 tStartTime;
  unsigned __int16 wTurmMin;

  __int64 size();
};

#pragma pack(push, 1)
struct _possible_battle_guild_list_result_zocl
{
  #pragma pack(push, 1)
  struct __list
  {
    char wszName[17];
    unsigned __int8 byGrade;
    unsigned __int8 byRace;
    unsigned int dwGuildSerial;
  };
  #pragma pack(pop)

  unsigned __int8 byPage;
  unsigned __int8 byMaxPage;
  unsigned int dwCurVer;
  unsigned int dwBattleCost;
  unsigned __int8 byMapCnt;
  unsigned __int8 byMapInx[100];
  unsigned __int8 byCount;
  __list DestGuild[4];

  __int64 size();
};
#pragma pack(pop)

#pragma pack(push, 2)
struct _guild_battle_rank_list_result_zocl_list
{
  int nRank;
  unsigned __int8 byGrade;
  char wszName[17];
  unsigned int dwWin;
  unsigned int dwLose;
  unsigned int dwDraw;
  unsigned int dwScore;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _guild_battle_rank_list_result_zocl
{
  #pragma pack(push, 1)
  struct __list
  {
    int nRank;
    char byGrade;
    char wszName[17];
    unsigned int dwWin;
    unsigned int dwLose;
    unsigned int dwDraw;
    unsigned int dwScore;
  };
  #pragma pack(pop)

  unsigned int dwCurVer;
  unsigned __int8 byRace;
  unsigned __int8 byCurPage;
  unsigned __int8 byMaxPage;
  unsigned __int8 byExistSelfGuildInfo;
  unsigned __int8 byCnt;
  _guild_battle_rank_list_result_zocl_list list[11];

  __int64 size();
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _guild_battle_reserved_schedule_result_zocl
{
  #pragma pack(push, 1)
  struct __list
  {
    char wsz1PName[17];
    unsigned __int8 by1PRace;
    char wsz2PName[17];
    unsigned __int8 by2PRace;
    unsigned __int8 byStartHour;
    unsigned __int8 byStartMin;
    unsigned __int8 byEndHour;
    unsigned __int8 byEndMin;
  };
  #pragma pack(pop)

  unsigned int dwVer;
  unsigned __int8 byToDay;
  unsigned __int8 byTomorrow;
  unsigned __int8 byDate;
  unsigned __int8 byPage;
  unsigned __int8 byMaxPage;
  unsigned __int8 bySelfScheduleInx;
  unsigned __int8 byCnt;
  __list List[5];

  __int64 size();
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _guild_battle_current_battle_info_result_zocl
{
  char wszLeftRedName[17];
  char wszRightBlueName[17];
  unsigned int dwLeftRedScore;
  unsigned int dwRighBluetScore;
  unsigned int dwLeftRedGoalCnt;
  unsigned int dwRighBluetGoalCnt;
  unsigned __int8 byLeftHour;
  unsigned __int8 byLeftMin;
  unsigned __int8 byLeftSec;
};
#pragma pack(pop)

