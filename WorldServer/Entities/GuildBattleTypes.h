#pragma once

#include "IdaCompat.h"
#include "Packet/ZoneClientPacket.h"

struct _qry_case_in_guildbattlecost
{
  unsigned int dwGuildIndex;
  unsigned int dwGuildSerial;
  unsigned int dwAddGold;
  unsigned int dwAddDalant;
  unsigned __int8 byDate[4];
  long double out_totalgold;
  long double out_totaldalant;

  int size();
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

  int size();
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

  int size();
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

  int size();
};


