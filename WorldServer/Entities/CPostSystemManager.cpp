#include "pch.h"

#include "CPostSystemManager.h"

#include "KorLocalTime.h"
#include "WorldServerUtil.h"

#include <cstdio>

CPostSystemManager *CPostSystemManager::Instace()
{
  static CPostSystemManager s_instance;
  return &s_instance;
}

bool CPostSystemManager::InitLogger()
{
  CreateDirectoryA("..\\ZoneServerLog\\Systemlog\\PostSystem", nullptr);

  m_pkLogger = new CLogFile();
  if (m_pkLogger == nullptr)
  {
    g_Main.m_logLoadingError.Write(
      "CPostSystemManager::InitLogger() NULL == new CLogFile!\r\n");
    return false;
  }

  char path[128]{};
  const unsigned int now = GetKorLocalTime();
  sprintf_s(path, sizeof(path), "..\\ZoneServerLog\\Systemlog\\PostSystem\\PostSystem%u.log", now);
  m_pkLogger->SetWriteLogFile(path, 1, 0, 1, 1);
  return true;
}

bool CPostSystemManager::Init()
{
  if (!InitLogger())
  {
    return false;
  }

  m_PostData = new CPostData[500];
  if (m_PostData == nullptr)
  {
    return false;
  }

  m_tmrRegiTime.BeginTimer(1000);
  m_tmrProcTime.BeginTimer(30000);

  CNetIndexList::SetList(&m_listEmpty, 500);
  CNetIndexList::SetList(&m_listRegist, 500);
  CNetIndexList::SetList(&m_listProc, 500);

  for (unsigned int i = 0; i < 500; ++i)
  {
    CNetIndexList::PushNode_Back(&m_listEmpty, i);
  }

  return true;
}
