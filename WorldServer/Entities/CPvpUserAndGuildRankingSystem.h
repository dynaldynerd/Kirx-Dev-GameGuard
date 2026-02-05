#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CGuildRanking.h"
#include "CUserRankingProcess.h"

class __cppobj CPvpUserAndGuildRankingSystem
{
public:
  static CPvpUserAndGuildRankingSystem *Instance();
  bool Init();
  bool InitLogger();
  bool IsCurrentRaceBossGroup(unsigned __int8 byRace, unsigned int dwSerial);
  unsigned int GetCurrentRaceBossSerial(unsigned __int8 byRace, unsigned __int8 byNth);
  bool IsRaceViceBoss(unsigned __int8 byRace, unsigned int dwSerial);
  unsigned __int8 GetBossType(unsigned __int8 byRace, unsigned int dwSerial);

  CLogFile *m_pkLogger;
  bool m_bInit;
  CUserRankingProcess m_kUserRankingProcess;
};
