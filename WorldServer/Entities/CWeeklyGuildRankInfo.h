#pragma once

#include "IdaCompat.h"

class __cppobj __declspec(align(8)) CWeeklyGuildRankOwnerInfo
{
public:
  CWeeklyGuildRankOwnerInfo();
  void Clear();

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
  bool Init();
  void Clear();
  void ClearRank();
  void ClearOwner();

  bool m_bInit;
  bool m_NoDataPrev;
  bool m_bNoDataToday;
  unsigned int m_dwRecordCnt[3];
  CWeeklyGuildRankRecord **m_ppkRaceStartPos[3];
  CWeeklyGuildRankOwnerInfo m_kOwnerInfo[3][2];
  unsigned int m_dwMaxCnt;
  CWeeklyGuildRankRecord **m_ppkInfo;
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
  };

  _weekly_guild_rank_result_zocl *m_pkSendList;
};

extern int SETTLEMENT_AREA_MANAGE_OWNER_LIMIT_GRADE;
