#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CGuildRanking.h"

#include <vector>

struct __unaligned _PVP_RANK_DATA;
struct __declspec(align(2)) _PVP_RANK_PACKED_DATA;
struct _PVP_RANK_REFRESH_USER;
struct _guild_member_refresh_data;

class __cppobj CPvpUserRankingInfo
{
public:
  std::vector<_PVP_RANK_DATA *> m_vecPvpRankDataCurrent;
  std::vector<_PVP_RANK_DATA *> m_vecPvpRankDataTomorrow;
  std::vector<_PVP_RANK_PACKED_DATA *> m_vecPackedRankData;
  unsigned __int8 m_byPvpRankDataVersion;
  std::vector<unsigned long *> m_dwUpdateRaceBossSerial;
  std::vector<unsigned long *> m_dwCurrentRaceBossSerial;
};

class __cppobj CPvpUserRankingTargetUserList
{
public:
  unsigned int m_uiAddTotalCnt;
  unsigned int m_uiRefreshCnt;
  std::vector<_PVP_RANK_REFRESH_USER *> m_PvpRankRefreshUser;
};

class __cppobj __declspec(align(8)) CUserRankingProcess
{
public:
  enum PROC_STATE : int
  {
    PS_NONE = -1,
    PS_INVALID = -2,
    PS_WAIT = 0,
    PS_SAVE_TARGET_LIST = 1,
    PS_RANK_START = 2,
    PS_RANK_COMPLETE = 3,
    PS_WAIT_DAY_CHANGED = 4,
    PS_NOTIFY_VERSIONUP = 5,
    PS_APPLY_GUILD_GRADE = 6,
    PS_APPLY_RANK_IN_GUILD = 7,
    PS_FAILED_WAIT = 8,
    PS_RANK_SUCCESS = 9,
    PS_VALID_MAX_STATE_CNT = 10,
    PS_USER_RANK_PROCESS = 20,
    PS_GUILD_RANK_PROCESS = 21,
    PS_RANK_IN_GUILD_PROCESS = 22,
    PS_UPDATE_AND_LOAD_GRADE = 23,
  };

  PROC_STATE m_eState;
  CLogFile *m_pkLogger;
  int m_iOldDay;
  long long m_tStartTime;
  long long m_tFailStartTime;
  int m_iPvpSaveJobIndexOffset;
  int m_iPvpRankDataVersionUpSendOffset;
  char m_szPvpRankDate[32];
  std::vector<void (__cdecl CUserRankingProcess::*)(void)> m_vecProc;
  CPvpUserRankingInfo m_kPvpRankingInfo;
  CPvpUserRankingTargetUserList m_kTargetUserList;
  CGuildRanking m_kGuildRanking;
};

struct __unaligned _PVP_RANK_DATA
{
  unsigned __int8 byRank;
  unsigned __int8 byLv;
  unsigned __int16 wRate;
  char wszName[17];
  unsigned __int8 byGrade;
  char wszGuildName[17];
  unsigned int dwAvatorSerial;
  unsigned int dwGuildSerial;
  long double dPvpPoint;
};

struct __declspec(align(2)) _PVP_RANK_PACKED_DATA
{
  unsigned __int16 wDataLen;
  char szPackedData[381];
};

struct _PVP_RANK_REFRESH_USER
{
  unsigned int dwSerial;
  unsigned __int8 byLv;
  unsigned __int8 byRace;
  unsigned __int16 wPvpRate;
  unsigned int dwPvpRank;
};

struct _guild_member_refresh_data
{
  unsigned int dwGuildSerial;
  unsigned __int16 wMemberCount;

  struct __refresh_data
  {
    unsigned int dwSerial;
    unsigned __int8 byGrade;
    unsigned __int8 byLv;
    unsigned int dwPvpPoint;
  };

  __refresh_data rMemberData[50];
};
