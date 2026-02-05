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

struct __declspec(align(8)) _qry_case_dest_guild_out_guildbattlecost
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

struct __declspec(align(8)) _qry_case_addguildbattleschedule
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
  struct __list
  {
    char wszName[17];
    unsigned __int8 byGrade;
    unsigned __int8 byRace;
    unsigned int dwGuildSerial;
  };

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
