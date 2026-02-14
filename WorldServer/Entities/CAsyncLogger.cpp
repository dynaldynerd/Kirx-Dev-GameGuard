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

CAsyncLogger::CAsyncLogger()
  : m_pSystemLogInfo(nullptr),
    m_kBufferList(nullptr)
{
}

CAsyncLogger::~CAsyncLogger()
{
  m_bProcThread = false;

  delete[] m_kBufferList;
  m_kBufferList = nullptr;

  for (auto &entry : m_mapLogInfo)
  {
    delete entry.second;
    entry.second = nullptr;
  }

  m_mapLogInfo.clear();
}

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
  m_klistEmpty.SetList(0xBE0u);
  for (unsigned int index = 0; index < dwMaxBufNum; ++index)
  {
    m_klistEmpty.PushNode_Back(index);
  }
  m_klistProc.SetList(dwMaxBufNum);
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
  while (m_klistProc.PopNode_Front(&outIndex))
  {
    unsigned long &listIndex = m_vecPushList[outIndex];
    m_kBufferList[listIndex].ProcWrite();
    listIndex = static_cast<unsigned long>(-1);
    m_klistEmpty.PushNode_Back(outIndex);
  }
}

void CAsyncLogger::Loop()
{
  if (!m_kCheckUpdateLogFileNameDelay.CountingTimer())
  {
    return;
  }

  for (auto &entry : m_mapLogInfo)
  {
    CAsyncLogInfo *logInfo = entry.second;
    if (logInfo)
    {
      logInfo->UpdateLogFileName();
    }
  }
}

void CAsyncLogger::Log(const char *szFileName, const char *szLog, int iLenStr)
{
  if (m_kBufferList == nullptr || szFileName == nullptr || szLog == nullptr || iLenStr <= 0)
  {
    return;
  }

  int bufferIndex = -1;
  for (int index = 0; index < 3; ++index)
  {
    if (m_kBufferList[index].GetBufferSize() >= iLenStr && m_kBufferList[index].GetEmptySize())
    {
      bufferIndex = index;
      break;
    }
  }

  if (bufferIndex < 0)
  {
    SystemLog("FileName(%s) iLenStr(%d) Log(%s) Exceed BuffSize!", szFileName, iLenStr, szLog);
    return;
  }

  unsigned int outIndex = 0;
  if (m_klistEmpty.PopNode_Front(&outIndex))
  {
    m_kBufferList[bufferIndex].Log(szFileName, szLog, iLenStr);
    m_vecPushList[outIndex] = static_cast<unsigned long>(bufferIndex);
    m_klistProc.PushNode_Back(outIndex);
  }
  else
  {
    SystemLog("FileName(%s) iLenStr(%d) Log(%s) Exceed BuffSize!", szFileName, iLenStr, szLog);
  }
}

bool CAsyncLogger::Log(int iType, const char *szLog)
{
  auto it = m_mapLogInfo.find(iType);
  if (it == m_mapLogInfo.end())
  {
    return false;
  }

  char buffer[11288]{};
  _SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  const unsigned int count = it->second->GetCount();
  const int headerLen = sprintf_s(
    buffer,
    0x2C00u,
    "%u\t%04d-%02d-%02d %02d:%02d:%02d.%03d : %s",
    count,
    systemTime.wYear,
    systemTime.wMonth,
    systemTime.wDay,
    systemTime.wHour,
    systemTime.wMinute,
    systemTime.wSecond,
    systemTime.wMilliseconds,
    szLog);

  if (headerLen <= 0 || headerLen >= 11264)
  {
    return false;
  }

  const char *fileName = it->second->GetFileName();
  Log(fileName, buffer, headerLen);
  it->second->IncreaseCount();
  return true;
}

bool CAsyncLogger::LogFromArg(int iType, char *fmt, va_list arg_ptr)
{
  auto it = m_mapLogInfo.find(iType);
  if (it == m_mapLogInfo.end())
  {
    return false;
  }

  char buffer[11288]{};
  _SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  const unsigned int count = it->second->GetCount();
  const int headerLen = sprintf_s(
    buffer,
    0x2C00u,
    "%u\t%04d-%02d-%02d %02d:%02d:%02d.%03d : ",
    count,
    systemTime.wYear,
    systemTime.wMonth,
    systemTime.wDay,
    systemTime.wHour,
    systemTime.wMinute,
    systemTime.wSecond,
    systemTime.wMilliseconds);

  if (headerLen <= 0 || headerLen >= 11264)
  {
    return false;
  }

  const int msgLen = vsprintf_s(&buffer[headerLen], 11264LL - headerLen, fmt, arg_ptr);
  if (msgLen <= 0)
  {
    return false;
  }

  const int totalLen = headerLen + msgLen;
  const char *fileName = it->second->GetFileName();
  Log(fileName, buffer, totalLen);
  it->second->IncreaseCount();
  return true;
}

bool CAsyncLogger::FormatLog(int iType, const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  bool result = LogFromArg(iType, const_cast<char *>(fmt), args);
  va_end(args);
  return result;
}

void CAsyncLogger::SystemLog(const char *fmt, ...)
{
  if (m_pSystemLogInfo == nullptr)
  {
    return;
  }

  char buffer[11288]{};
  _SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  const unsigned int count = m_pSystemLogInfo->GetCount();
  const int headerLen = sprintf_s(
    buffer,
    0x2C00u,
    "%u\t%04d-%02d-%02d %02d:%02d:%02d.%03d : ",
    count,
    systemTime.wYear,
    systemTime.wMonth,
    systemTime.wDay,
    systemTime.wHour,
    systemTime.wMinute,
    systemTime.wSecond,
    systemTime.wMilliseconds);

  if (headerLen <= 0 || headerLen >= 11264)
  {
    return;
  }

  va_list args;
  va_start(args, fmt);
  const int msgLen = vsprintf_s(&buffer[headerLen], 11264LL - headerLen, fmt, args);
  va_end(args);

  if (msgLen <= 0)
  {
    return;
  }

  const int totalLen = headerLen + msgLen;
  const char *fileName = m_pSystemLogInfo->GetFileName();
  CAsyncLogBufferList::WriteFile(fileName, static_cast<unsigned long>(totalLen), buffer);
}
