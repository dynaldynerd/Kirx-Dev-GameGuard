#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CGuildRanking.h"
#include "guild_member_refresh_data.h"

#include <vector>

struct  _PVP_RANK_DATA;
struct _PVP_RANK_PACKED_DATA;
struct _PVP_RANK_REFRESH_USER;
struct _qry_case_rank_racerank_guildrank;

class  CPvpUserRankingInfo
{
public:
  CPvpUserRankingInfo();
  ~CPvpUserRankingInfo();

  bool assign();
  void ClearTomorrowPvpRankData();
  void DoDayChangedWork(CLogFile *pkLogger);
  unsigned __int8 GetPvpRankDataVersion();
  void PvpRankListRequest(
    unsigned __int16 wIndex,
    unsigned __int8 byRace,
    unsigned __int8 byVersion,
    unsigned __int8 byPage);
  void SendMsg_PvpRankListData(
    unsigned __int16 wIndex,
    unsigned __int8 byRace,
    unsigned __int8 byVersion,
    unsigned __int8 byPage);
  void SendMsg_PvpRankListNodata(
    unsigned __int16 wIndex,
    char byRace,
    char byPage,
    unsigned __int8 byRet);
  bool IsCurrentRaceBossGroup(unsigned __int8 byRace, unsigned int dwSerial);
  unsigned int GetCurrentRaceBossSerial(unsigned __int8 byRace, unsigned __int8 byNth);
  void SetCurrentRaceBossSerial(unsigned __int8 byRace, unsigned __int8 byNth, unsigned int dwSerial);
  const _PVP_RANK_PACKED_DATA *GetRankPackedData(unsigned __int8 byRace, unsigned __int8 byPage);
  unsigned int FindRank(unsigned __int8 byRaceCode, unsigned int dwAvatorSerial);
  const _PVP_RANK_DATA *GetCurrentPvpRankData(unsigned __int8 byRace, unsigned __int8 byNth);
  unsigned __int8 GetBossType(unsigned __int8 byRaceCode, unsigned int dwSerial);
  char IsRaceViceBoss(unsigned __int8 byRace, unsigned int dwSerial);
  void SetUpdateRaceBossSerial(unsigned __int8 byRace, unsigned __int8 byNth, unsigned int dwSerial);
  void FlipPvPRankTop();
  void IncreaseVesion();
  char LoadPvpRank(char *szDate);
  void PvpRankDataPacking(CLogFile *pkLogger);
  _PVP_RANK_DATA *GetTomorrowPvpRankData(unsigned __int8 byRace, unsigned __int8 byNth);
  unsigned __int8 UpdateRaceRankStep1(char *szData);
  unsigned __int8 UpdateRaceRankStep2(char *szData);
  unsigned __int8 UpdateRaceRankStep3(char *szData);
  unsigned __int8 UpdateRaceRankStep4(char *szData);
  unsigned __int8 UpdateRaceRankStep5(char *szData);
  unsigned __int8 UpdateRaceRankStep6(char *szData);
  unsigned __int8 UpdateRaceRankStep7(char *szData);
  unsigned __int8 UpdateRaceRankStep8(char *szData);
  unsigned __int8 UpdateRaceRankStep9(char *szData);
  unsigned __int8 UpdateRaceRankStep10(_qry_case_rank_racerank_guildrank *szData);

  std::vector<_PVP_RANK_DATA *> m_vecPvpRankDataCurrent;
  std::vector<_PVP_RANK_DATA *> m_vecPvpRankDataTomorrow;
  std::vector<_PVP_RANK_PACKED_DATA *> m_vecPackedRankData;
  unsigned __int8 m_byPvpRankDataVersion;
  std::vector<unsigned long *> m_dwUpdateRaceBossSerial;
  std::vector<unsigned long *> m_dwCurrentRaceBossSerial;
};

class  CPvpUserRankingTargetUserList
{
public:
  CPvpUserRankingTargetUserList();
  ~CPvpUserRankingTargetUserList();

  bool assign();
  void ClearRankingStart();
  void Add(unsigned int dwSerial, unsigned __int8 byLv, unsigned __int8 byRace);
  unsigned int GetAddedTotalCnt();
  void UpdateCharGrade();
  unsigned __int8 UpdateRaceRankStep11(_qry_case_rank_racerank_guildrank *szData);

  unsigned int m_uiAddTotalCnt;
  unsigned int m_uiRefreshCnt;
  std::vector<_PVP_RANK_REFRESH_USER *> m_PvpRankRefreshUser;
};

class  CUserRankingProcess
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

  CUserRankingProcess();
  ~CUserRankingProcess();

  bool Init();
  void Loop();
  void SetLogger(CLogFile *pkLogger);
  char Load();
  void GetRankDateStr(char *szDate, unsigned __int64 tDateStrSize);
  void GetTommorrowStr(char *szTommorrow);
  char CheckAndCreateTodayPvpRankTable(char *szDate);
  void CheckTomorrowPvpRankDate();
  void LoadINI(unsigned int *piHour, unsigned int *piMin);
  bool SetRankingStartTime(int iHour, int iMin);
  bool AllocObject();
  bool InitProcFunc();
  void UpdateNextRankingStartTime();

  bool IsCurrentRaceBossGroup(unsigned __int8 byRace, unsigned int dwSerial);
  unsigned int GetCurrentRaceBossSerial(unsigned __int8 byRace, unsigned __int8 byNth);
  void SetCurrentRaceBossSerial(unsigned __int8 byRace, unsigned __int8 byNth, unsigned int dwSerial);
  unsigned int FindRank(unsigned __int8 byRaceCode, unsigned int dwAvatorSerial);
  const _PVP_RANK_DATA *GetCurrentPvpRankData(unsigned __int8 byRace, unsigned __int8 byNth);
  unsigned __int8 GetBossType(unsigned __int8 byRace, unsigned int dwSerial);
  bool IsRaceViceBoss(unsigned __int8 byRace, unsigned int dwSerial);
  void SetUpdateRaceBossSerial(unsigned __int8 byRace, unsigned __int8 byNth, unsigned int dwSerial);
  void FlipPvPRankTop();
  void PvpRankDataPacking();
  void PvpRankListRequest(
    unsigned __int16 wIndex,
    unsigned __int8 byRace,
    unsigned __int8 byVersion,
    unsigned __int8 byPage);
  void IncreaseVesion();

  void ProcWait();
  void ProcSaveTargetList();
  void ProcRankStart();
  void ProcRankComplete();
  void ProcWaitDayChanged();
  void ProcNotifyVersionUp();
  void ProcApplyGuildGrade();
  void ProcApplyRankInGuild();
  void ProcFailedWait();
  void ProcRankSuccess();

  unsigned __int8 UpdateRaceRankStep1(char *szData);
  unsigned __int8 UpdateRaceRankStep2(char *szData);
  unsigned __int8 UpdateRaceRankStep3(char *szData);
  unsigned __int8 UpdateRaceRankStep4(char *szData);
  unsigned __int8 UpdateRaceRankStep5(char *szData);
  unsigned __int8 UpdateRaceRankStep6(char *szData);
  unsigned __int8 UpdateRaceRankStep7(char *szData);
  unsigned __int8 UpdateRaceRankStep8(char *szData);
  unsigned __int8 UpdateRaceRankStep9(char *szData);
  unsigned __int8 UpdateRaceRankStep10(_qry_case_rank_racerank_guildrank *szData);
  unsigned __int8 UpdateRaceRankStep11(_qry_case_rank_racerank_guildrank *szData);
  unsigned __int8 UpdateGuildRankStep1(char *szData);
  unsigned __int8 UpdateGuildRankStep2(char *szData);
  unsigned __int8 UpdateGuildRankStep3(char *szData);
  unsigned __int8 UpdateGuildRankStep4(char *szData);
  unsigned __int8 UpdateRankinGuildStep1(char *szData);
  unsigned __int8 UpdateRankinGuildStep2(char *szData);
  unsigned __int8 UpdateRankinGuildStep3(char *szData);
  unsigned __int8 UpdateRankinGuildStep4(char *szData);
  unsigned __int8 UpdateRankinGuildStep5(char *szData);
  unsigned __int8 UpdateRankinGuildStep6(char *szData);
  unsigned __int8 UpdateAndSelectGuildGrade(char *szData);

  void CompleteRaceRankStep1(unsigned __int8 byRet, char *szData);
  void CompleteRaceRankStep2(unsigned __int8 byRet, char *szData);
  void CompleteRaceRankStep3(unsigned __int8 byRet, char *szData);
  void CompleteRaceRankStep4(unsigned __int8 byRet, char *szData);
  void CompleteRaceRankStep5(unsigned __int8 byRet, char *szData);
  void CompleteRaceRankStep6(unsigned __int8 byRet, char *szData);
  void CompleteRaceRankStep7(unsigned __int8 byRet, char *szData);
  void CompleteRaceRankStep8(unsigned __int8 byRet, char *szData);
  void CompleteRaceRankStep9(unsigned __int8 byRet, char *szData);
  void CompleteRaceRankStep10(unsigned __int8 byRet, char *szData);
  void CompleteRaceRankStep11(unsigned __int8 byRet, char *szData);
  void CompleteGuildRankStep1(unsigned __int8 byRet, char *szData);
  void CompleteGuildRankStep2(unsigned __int8 byRet, char *szData);
  void CompleteGuildRankStep3(unsigned __int8 byRet, char *szData);
  void CompleteGuildRankStep4(unsigned __int8 byRet, char *szData);
  void CompleteRankInGuildStep1(unsigned __int8 byRet, _qry_case_rank_racerank_guildrank *szData);
  void CompleteRankInGuildStep2(unsigned __int8 byRet, _qry_case_rank_racerank_guildrank *szData);
  void CompleteRankInGuildStep3(unsigned __int8 byRet, _qry_case_rank_racerank_guildrank *szData);
  void CompleteRankInGuildStep4(unsigned __int8 byRet, _qry_case_rank_racerank_guildrank *szData);
  void CompleteRankInGuildStep5(unsigned __int8 byRet, _qry_case_rank_racerank_guildrank *szData);
  void CompleteRankInGuildStep6(unsigned __int8 byRet, _qry_case_rank_racerank_guildrank *szData);
  void CompleteRankUpdateAndSelectGarde(unsigned __int8 byRet, _qry_case_rank_racerank_guildrank *szData);
};

#pragma pack(push, 1)
struct  _PVP_RANK_DATA
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
#pragma pack(pop)

struct _PVP_RANK_PACKED_DATA
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

  void Init();
  bool IsFilled();
  void SetData(unsigned int dwSerialP, unsigned __int8 byLvP, unsigned __int8 byRaceP);
};

