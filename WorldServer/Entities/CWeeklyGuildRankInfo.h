#pragma once

#include "IdaCompat.h"

struct _pvppoint_guild_rank_info;
struct _weeklyguildrank_owner_info;
struct _weekly_guild_rank_result_zocl;
class CGuild;

class  CWeeklyGuildRankOwnerInfo
{
public:
  CWeeklyGuildRankOwnerInfo();
  ~CWeeklyGuildRankOwnerInfo();
  void Clear();
  bool IsEmpty();

  unsigned int m_dwSerial;
  char m_wszGuildName[17];
  unsigned __int8 m_byRace;
  unsigned __int16 m_wRank;
  unsigned __int8 m_byGrade;
  long double m_dKillPvpPoint;
  long double m_dGuildBattlePvpPoint;
  unsigned int m_dwSumLv;
};

class  CWeeklyGuildRankRecord
{
public:
  CWeeklyGuildRankRecord();
  ~CWeeklyGuildRankRecord();
  void Clear();

  unsigned int m_dwInx;
  unsigned __int16 m_wRank;
  unsigned int m_dwSerial;
  char m_wszGuildName[17];
  unsigned __int8 m_byGrade;
  long double m_dKillPvpPoint;
  long double m_dGuildBattlePvpPoint;
};

class  CWeeklyGuildRankInfo
{
public:
  CWeeklyGuildRankInfo();
  ~CWeeklyGuildRankInfo();
  bool Init();
  void Clear();
  void ClearRank();
  void ClearOwner();
  int Find(unsigned __int8 byRace, unsigned int dwGuildSerial);
  bool CheckEmpty(_pvppoint_guild_rank_info *pkInfo);
  bool Load(_pvppoint_guild_rank_info *pkInfo, bool *bNoData);
  bool LoadToday(_pvppoint_guild_rank_info *pkInfo);
  bool LoadPrev(_pvppoint_guild_rank_info *pkInfo);
  bool LoadOwner(_weeklyguildrank_owner_info *pkInfo);
  bool Update(_pvppoint_guild_rank_info *pkInfo);
  bool UpdateOwner(_weeklyguildrank_owner_info *pkInfo);
  CGuild *GetOwnerGuild(unsigned __int8 byRace, unsigned __int8 byNth);
  CGuild *GetPrevOwnerGuild(unsigned __int8 byRace, unsigned __int8 byNth);
  CGuild *GetCurOwnerGuild(unsigned __int8 byRace, unsigned __int8 byNth);
  bool IsNoDataPrev();
  bool IsNoDataToday();
  void SetNoDataFlagToday();
  void Send(
    unsigned int dwVer,
    unsigned int n,
    unsigned __int8 byTabRace,
    unsigned __int8 bySelfRace,
    unsigned int dwGuildSerial);

  bool m_bInit;
  bool m_NoDataPrev;
  bool m_bNoDataToday;
  unsigned int m_dwRecordCnt[3];
  CWeeklyGuildRankRecord **m_ppkRaceStartPos[3];
  CWeeklyGuildRankOwnerInfo m_kOwnerInfo[3][2];
  unsigned int m_dwMaxCnt;
  CWeeklyGuildRankRecord **m_ppkInfo;
  _weekly_guild_rank_result_zocl *m_pkSendList;
};

extern int SETTLEMENT_AREA_MANAGE_OWNER_LIMIT_GRADE;

