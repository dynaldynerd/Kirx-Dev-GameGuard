#include "pch.h"

#include "CAsyncLogger.h"

#include "CAsyncLogBufferList.h"
#include "CAsyncLogInfo.h"
#include "WorldServerUtil.h"

#include <process.h>
#include <cstring>
#include <string.h>
#include <utility>

bool CAsyncLogger::m_bProcThread = false;

CAsyncLogger *CAsyncLogger::Instance()
{
  static CAsyncLogger s_instance;
  return &s_instance;
}

bool CAsyncLogger::Regist(
  ASYNC_LOG_TYPE eType,
  const char *directory,
  const char *filePrefix,
  bool isDaily,
  unsigned int dwUpdateFileNameDelay)
{
  if (m_mapLogInfo.find(static_cast<int>(eType)) != m_mapLogInfo.end())
  {
    m_logLoading.Write(
      "CAsyncLogger::Regist( AYNC_LOG_TYPE eType(%d), char * szDirPath(%s), char * szTypeName(%s) ) :  already exist type!",
      eType,
      directory,
      filePrefix);
    return false;
  }

  for (auto &entry : m_mapLogInfo)
  {
    if (entry.second == nullptr)
    {
      continue;
    }
    if (_stricmp(entry.second->GetDirPath(), directory) == 0 &&
        _stricmp(entry.second->GetTypeName(), filePrefix) == 0)
    {
      m_logLoading.Write(
        "CAsyncLogger::Regist( AYNC_LOG_TYPE eType(%d), char * szDirPath(%s), char * szTypeName(%s) ) :  type(%d) DirPath(%s) TypeName(%s) already exist!",
        eType,
        directory,
        filePrefix,
        entry.first,
        entry.second->GetDirPath(),
        entry.second->GetTypeName());
      return false;
    }
  }

  CAsyncLogInfo *logInfo = new CAsyncLogInfo();
  if (logInfo == nullptr)
  {
    g_Main.m_logLoadingError.Write(
      "CAsyncLogger::Regist( AYNC_LOG_TYPE eType(%d), char * szDirPath(%s), char * szTypeName(%s) ) :  new CAsyncLogInfo NULL!",
      eType,
      directory,
      filePrefix);
    return false;
  }

  if (!logInfo->Init(eType, directory, filePrefix, isDaily, dwUpdateFileNameDelay, &m_logLoading))
  {
    delete logInfo;
    return false;
  }

  m_mapLogInfo.insert(std::make_pair(static_cast<int>(eType), logInfo));
  CreateDirectoryA(directory, nullptr);
  clear_file(directory, 0xFu);
  return true;
}

int CAsyncLogger::Init()
{
  CreateDirectoryA("..\\ZoneServerLog\\SystemLog\\AsyncLogger", nullptr);
  clear_file("..\\ZoneServerLog\\SystemLog\\AsyncLogger", 0xFu);

  char dest[128]{};
  sprintf_s(dest, "%s\\Loading.log", "..\\ZoneServerLog\\SystemLog\\AsyncLogger");
  m_logLoading.SetWriteLogFile(dest, 1, 0, 1, 1);
  m_logLoading.Write("CAsyncLogger::Init() : Loading.log Create Success!");

  if (!Regist(
        ALT_ASYNC_LOGGER_SYSTEM_LOG,
        "..\\ZoneServerLog\\SystemLog\\AsyncLogger",
        "System",
        true,
        0xFFFFFFFFu))
  {
    m_logLoading.Write(
      "CAsyncLogger::Init() : Regist( ALT_ASYNC_LOGGER_SYSTEM_LOG, ASYNC_LOGGER_UPPER_SYSTEM_LOG_PATH, System ) Fail!");
    return -2;
  }

  auto it = m_mapLogInfo.find(static_cast<int>(ALT_ASYNC_LOGGER_SYSTEM_LOG));
  if (it == m_mapLogInfo.end())
  {
    m_logLoading.Write(
      "CAsyncLogger::Init() : m_mapLogInfo.find( ALT_ASYNC_LOGGER_SYSTEM_LOG ) Fail!");
    return -3;
  }
  m_pSystemLogInfo = it->second;

  m_kBufferList = new CAsyncLogBufferList[3];
  if (m_kBufferList == nullptr)
  {
    m_logLoading.Write(
      "CAsyncLogger::Init() : new CAsyncLogBufferList[BT_MAX_TYPE(%u)] NULL!",
      3);
    return -4;
  }

  if (!m_kBufferList[0].Init(0x9E4u, 0xC8u, &m_logLoading))
  {
    return -5;
  }
  if (!m_kBufferList[1].Init(0xFEu, 0x400u, &m_logLoading))
  {
    return -6;
  }
  if (!m_kBufferList[2].Init(0xFEu, 0x2800u, &m_logLoading))
  {
    return -7;
  }

  const unsigned int dwMaxBufNum = 3040;
  CNetIndexList::SetList(&m_klistEmpty, 0xBE0u);
  for (unsigned int index = 0; index < dwMaxBufNum; ++index)
  {
    CNetIndexList::PushNode_Back(&m_klistEmpty, index);
  }
  CNetIndexList::SetList(&m_klistProc, dwMaxBufNum);
  m_vecPushList.assign(dwMaxBufNum, static_cast<unsigned long>(-1));

  m_kCheckUpdateLogFileNameDelay.BeginTimer(0x2710u);
  m_bProcThread = true;

  if (_beginthread(CAsyncLogger::ProcThread, 0, this) == -1L)
  {
    m_logLoading.Write("CAsyncLogger::Init() : ::_beginthread( ... ) Fail!");
    return -4;
  }

  return 0;
}

void CAsyncLogger::ProcThread(void *param)
{
  CAsyncLogger *logger = static_cast<CAsyncLogger *>(param);
  while (m_bProcThread)
  {
    logger->ProcWrite();
    Sleep(1u);
  }
}

void CAsyncLogger::ProcWrite()
{
  m_FrameRate.CalcSpeedPerFrame();
  if (m_kBufferList == nullptr)
  {
    return;
  }

  unsigned int outIndex = 0;
  while (CNetIndexList::PopNode_Front(&m_klistProc, &outIndex))
  {
    unsigned long &listIndex = m_vecPushList[outIndex];
    m_kBufferList[listIndex].ProcWrite();
    listIndex = static_cast<unsigned long>(-1);
    CNetIndexList::PushNode_Back(&m_klistEmpty, outIndex);
  }
}
