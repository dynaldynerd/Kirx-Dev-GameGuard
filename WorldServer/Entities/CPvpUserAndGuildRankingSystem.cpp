#include "pch.h"

#include "CPvpUserAndGuildRankingSystem.h"

#include "KorLocalTime.h"
#include "WorldServerUtil.h"

#include <cstdio>

CPvpUserAndGuildRankingSystem *CPvpUserAndGuildRankingSystem::Instance()
{
  static CPvpUserAndGuildRankingSystem s_instance;
  return &s_instance;
}

bool CPvpUserAndGuildRankingSystem::InitLogger()
{
  m_pkLogger = new CLogFile();
  if (m_pkLogger == nullptr)
  {
    g_Main.m_logLoadingError.Write(
      "CPvpUserAndGuildRankingSystem::InitLogger() NULL == new CLogFile!\r\n");
    return false;
  }

  char path[1024]{};
  const unsigned int now = GetKorLocalTime();
  sprintf_s(path, sizeof(path), "..\\ZoneServerLog\\Systemlog\\RankJob%u.log", now);
  m_pkLogger->SetWriteLogFile(path, 1, 0, 1, 1);
  m_kUserRankingProcess.SetLogger(m_pkLogger);
  return true;
}

bool CPvpUserAndGuildRankingSystem::Init()
{
  return InitLogger() && m_kUserRankingProcess.Init();
}
