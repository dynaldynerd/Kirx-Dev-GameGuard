#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CGuildRanking.h"
#include "CUserRankingProcess.h"

class __cppobj CPvpUserAndGuildRankingSystem
{
public:
  CPvpUserAndGuildRankingSystem();
  ~CPvpUserAndGuildRankingSystem();

  static CPvpUserAndGuildRankingSystem *Instance();
  bool Init();
  void Loop();
  bool InitLogger();
  bool Load();
  bool IsCurrentRaceBossGroup(unsigned __int8 byRace, unsigned int dwSerial);
  unsigned int GetCurrentRaceBossSerial(unsigned __int8 byRace, unsigned __int8 byNth);
  unsigned int FindRank(unsigned __int8 byRaceCode, unsigned int dwAvatorSerial);
  const _PVP_RANK_DATA *GetCurrentPvpRankData(unsigned __int8 byRace, unsigned __int8 byNth);
  void PvpRankListRequest(
    unsigned __int16 wIndex,
    unsigned __int8 byRace,
    unsigned __int8 byVersion,
    unsigned __int8 byPage);
  bool IsRaceViceBoss(unsigned __int8 byRace, unsigned int dwSerial);
  unsigned __int8 GetBossType(unsigned __int8 byRace, unsigned int dwSerial);
  void Log(const char *fmt, ...);
  void SetUpdateRaceBossSerial(unsigned __int8 byRace, unsigned __int8 byNth, unsigned int dwSerial);
  void ApplyUpdatedBossInfo();

  unsigned __int8 UpdateRaceRankStep1(char *szData);
  unsigned __int8 UpdateRaceRankStep2(char *szData);
  unsigned __int8 UpdateRaceRankStep3(char *szData);
  unsigned __int8 UpdateRaceRankStep4(char *szData);
  unsigned __int8 UpdateRaceRankStep5(char *szData);
  unsigned __int8 UpdateRaceRankStep6(char *szData);
  unsigned __int8 UpdateRaceRankStep7(char *szData);
  unsigned __int8 UpdateRaceRankStep8(char *szData);
  unsigned __int8 UpdateRaceRankStep9(char *szData);
  unsigned __int8 UpdateRaceRankStep10(char *szData);
  unsigned __int8 UpdateRaceRankStep11(char *szData);
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
  void CompleteRankInGuildStep1(unsigned __int8 byRet, char *szData);
  void CompleteRankInGuildStep2(unsigned __int8 byRet, char *szData);
  void CompleteRankInGuildStep3(unsigned __int8 byRet, char *szData);
  void CompleteRankInGuildStep4(unsigned __int8 byRet, char *szData);
  void CompleteRankInGuildStep5(unsigned __int8 byRet, char *szData);
  void CompleteRankInGuildStep6(unsigned __int8 byRet, char *szData);
  void CompleteRankUpdateAndSelectGarde(unsigned __int8 byRet, char *szData);

  CLogFile *m_pkLogger;
  bool m_bInit;
  CUserRankingProcess m_kUserRankingProcess;
};
