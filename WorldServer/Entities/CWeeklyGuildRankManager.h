#pragma once

#include "IdaCompat.h"
#include "CWeeklyGuildRankInfo.h"
#include "pvppoint_guild_rank_info.h"
#include "weeklyguildrank_owner_info.h"

class CRFWorldDatabase;
class CGuild;
class CPlayer;

class __cppobj CWeeklyGuildRankManager
{
public:
  static CWeeklyGuildRankManager *Instance();
  bool Init();
  bool InitNextSetOwnerDate();
  void SetNextRankDate();
  bool InsertSettlementOwner(CRFWorldDatabase *pkWorldDB, char *pData);
  void Loop();
  void GetTodayRankDate(char *szDate, int iBuffSize);
  __int64 UpdateTodayTable(char *szDate, _pvppoint_guild_rank_info *pkInfo);
  void OrderRank(_pvppoint_guild_rank_info *pkInfo);
  char UpdateRankDBRecord(char *szDate, _pvppoint_guild_rank_info *pkInfo);
  char UpdateOwnerGuild(char *szDate);
  char SelectOwnerGuild(char *szDate, _weeklyguildrank_owner_info *pkInfo);
  void SetSettlementAreaManageOwnerGuild();
  bool SaveINI();
  void PushSettlementOwnerDBLog(char *pInfo);
  char UpdateTodayRank(_pvppoint_guild_rank_info *pLoadData);
  bool UpdateWeeklyOwner(_weeklyguildrank_owner_info *pLoadData);
  void CompleteLoadeTodayRank(unsigned __int8 byRet, _pvppoint_guild_rank_info *pLoadData);
  void CompleteUpdateWeeklyOwner(unsigned __int8 byRet, _weeklyguildrank_owner_info *pLoadData);
  void CompleteUpdateClear(unsigned __int8 byRet);
  bool CreatePvpPointGuildRank(char *szDate);
  char Load();
  bool LoadPrevOwner();
  bool LoadPrevTable(char *szDate, _pvppoint_guild_rank_info *kInfo);
  __int64 LoadINILastRankTime();
  void GetPrevRankDate(char *szDate, int iBuffSize);
  char InsertDefaultWeeklyPvpPointSumRecord();
  char PushDQSIncWeeklyPvpPointSum(unsigned int dwGuildSerial, long double dPoint);
  char IsEmptyRank(_pvppoint_guild_rank_info *pkInfo);
  CGuild *GetPrevOwnerGuild(unsigned __int8 byRace, unsigned __int8 byNth);
  void Send(unsigned int dwVer, unsigned __int8 byTabRace, CPlayer *pkPlayer);

  long long m_tNextUpdateTime;
  long long m_tNextSetOwnerTime;
  CWeeklyGuildRankInfo m_kInfo;
};
