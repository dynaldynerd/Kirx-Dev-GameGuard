#pragma once

#include "IdaCompat.h"

struct _pvppoint_guild_rank_info;
struct _weeklyguildrank_owner_info;
class CGuild;

class __cppobj __declspec(align(8)) CWeeklyGuildRankOwnerInfo
{
public:
  CWeeklyGuildRankOwnerInfo();
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

class __cppobj CWeeklyGuildRankRecord
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

class __cppobj CWeeklyGuildRankInfo
{
public:
  CWeeklyGuildRankInfo();
  ~CWeeklyGuildRankInfo();
  bool Init();
  void Clear();
  void ClearRank();
  void ClearOwner();
  __int64 Find(unsigned __int8 byRace, unsigned int dwGuildSerial);
  char CheckEmpty(_pvppoint_guild_rank_info *pkInfo);
  char Load(_pvppoint_guild_rank_info *pkInfo, bool *bNoData);
  bool LoadToday(_pvppoint_guild_rank_info *pkInfo);
  bool LoadPrev(_pvppoint_guild_rank_info *pkInfo);
  char LoadOwner(_weeklyguildrank_owner_info *pkInfo);
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
#pragma pack(push, 1)
  struct __unaligned __declspec(align(1)) _weekly_guild_rank_result_zocl
  {
    struct _list
    {
      unsigned __int8 byRank;
      char wszGuildName[17];
      unsigned __int8 byGrade;
      unsigned int dwPvpPoint;
    };

    unsigned int dwVer;
    unsigned __int8 byRace;
    unsigned __int8 byExistSelfRankInfo;
    unsigned __int8 byCnt;
    _list list[11];

    __int64 size();
  };
#pragma pack(pop)

  _weekly_guild_rank_result_zocl *m_pkSendList;
};

extern int SETTLEMENT_AREA_MANAGE_OWNER_LIMIT_GRADE;
