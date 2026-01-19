#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CGuildRanking.h"
#include "CUserRankingProcess.h"

class __cppobj CPvpUserAndGuildRankingSystem
{
public:
  CLogFile *m_pkLogger;
  bool m_bInit;
  CUserRankingProcess m_kUserRankingProcess;
};
