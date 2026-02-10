#include "pch.h"

#include "CMgrGuildHistory.h"

#include <process.h>
#include <cstdio>
#include <cstring>

#include "WorldServerUtil.h"

namespace
{
  char s_guildHistoryLogData[10000]{};
  const char kGuildHistoryMoneyFmt[] =
    "GUILD MONEY : Name(%s) Serial(%u) D:%d G:%d TotalD:%.0f TotalG:%.0f [%s]\r\n";
}

CMgrGuildHistory::CMgrGuildHistory()
  : m_szStdPath{},
    m_dwLastLocalDate(0),
    m_dwLastLocalHour(0),
    m_tmrUpdateTime(),
    m_szCurTime{},
    m_LogData{},
    m_listLogData(),
    m_listLogDataEmpty(),
    m_bIOThread(true)
{
  char returnedString[128]{};
  GetPrivateProfileStringA(
    "System",
    "HistoryPath",
    "C:\\History",
    returnedString,
    0x80u,
    "..\\WorldInfo\\WorldInfo.ini");
  CreateDirectoryA(returnedString, nullptr);
  sprintf(m_szStdPath, "%s\\Guild", returnedString);
  CreateDirectoryA(m_szStdPath, nullptr);

  const unsigned int korLocalTime = GetKorLocalTime();
  sprintf(m_szStdPath, "%s\\%d", m_szStdPath, korLocalTime);
  CreateDirectoryA(m_szStdPath, nullptr);

  char dateBuffer[160]{};
  char timeBuffer[128]{};
  _strdate_s(dateBuffer, 0x80u);
  dateBuffer[5] = 0;
  _strtime_s(timeBuffer, 0x80u);
  timeBuffer[5] = 0;
  sprintf_s(m_szCurTime, 0x20u, "%s %s", dateBuffer, timeBuffer);

  m_tmrUpdateTime.BeginTimer(0xEA60u);
  m_listLogData.SetList(0x9E4u);
  m_listLogDataEmpty.SetList(0x9E4u);
  for (unsigned int index = 0; static_cast<int>(index) < 2532; ++index)
  {
    m_listLogDataEmpty.PushNode_Back(index);
  }

  m_bIOThread = true;
  _beginthread(reinterpret_cast<_beginthread_proc_type>(&CMgrGuildHistory::IOThread), 0, this);
}

CMgrGuildHistory::~CMgrGuildHistory()
{
  m_bIOThread = false;
}

void CMgrGuildHistory::IOThread(void *pv)
{
  auto *self = reinterpret_cast<CMgrGuildHistory *>(pv);
  unsigned int outIndex[5]{};

  while (self->m_bIOThread)
  {
    while (self->m_listLogData.PopNode_Front(outIndex))
    {
      CMgrGuildHistory::__LOG_DATA &entry = self->m_LogData[outIndex[0]];
      IOFileWrite_3(entry.szFileName, static_cast<unsigned int>(entry.nLen), entry.sData);
      self->m_listLogDataEmpty.PushNode_Back(outIndex[0]);
      Sleep(0);
    }
    Sleep(1u);
  }

  _endthreadex(0);
}

void CMgrGuildHistory::WriteFile(const char *pszFileName, const char *pszLog)
{
  const unsigned int logSize = static_cast<unsigned int>(strlen_0(pszLog));
  unsigned int outIndex[4]{};
  if (logSize < 0x12C && m_listLogDataEmpty.PopNode_Front(outIndex))
  {
    __LOG_DATA &entry = m_LogData[outIndex[0]];
    strcpy_s(entry.szFileName, 0x80u, pszFileName);
    entry.nLen = static_cast<int>(logSize);
    memcpy_0(entry.sData, pszLog, logSize);
    entry.sData[logSize] = 0;
    m_listLogData.PushNode_Back(outIndex[0]);
  }
  else
  {
    IOFileWrite_3(pszFileName, logSize, pszLog);
  }
}

void CMgrGuildHistory::push_money(
  const char *pszIOerName,
  unsigned int dwIOerSerial,
  int nPushDalant,
  int nPushGold,
  long double dTotalDalant,
  long double dTotalGold,
  const char *pszFileName)
{
  s_guildHistoryLogData[0] = 0;
  sprintf(
    s_guildHistoryLogData,
    kGuildHistoryMoneyFmt,
    pszIOerName,
    dwIOerSerial,
    nPushDalant,
    nPushGold,
    dTotalDalant,
    dTotalGold,
    m_szCurTime);
  s_guildHistoryLogData[9999] = 0;
  WriteFile(pszFileName, s_guildHistoryLogData);
}
