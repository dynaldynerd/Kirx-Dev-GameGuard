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

  CLogFile *m_pkLogger;
  bool m_bInit;
  CUserRankingProcess m_kUserRankingProcess;
};
