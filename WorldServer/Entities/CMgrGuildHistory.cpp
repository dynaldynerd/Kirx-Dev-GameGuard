#include "pch.h"

#include "CMgrGuildHistory.h"

#include <process.h>
#include <cstdio>
#include <cstring>

#include "CGuild.h"
#include "WorldServerUtil.h"

namespace
{
  char s_guildHistoryLogData[10000]{};
  const char kGuildHistoryMoneyFmt[] =
    "GUILD MONEY : Name(%s) Serial(%u) D:%d G:%d TotalD:%.0f TotalG:%.0f [%s]\r\n";
  const char kGuildHistoryStartFmt[] =
    "START GUILD : Name(%s) Serial(%u) Grade(%d) TotalD:%.0f TotalG:%.0f [%s]\r\n";
  const char kGuildHistoryMemberCountFmt[] = "MEMBER COUNT : %u\r\n";
  const char kGuildHistoryJoinFmt[] =
    "JOIN MEMBER : Name(%s) Serial(%u) Approver(%s,%u) total(%d) [%s]\r\n";
  const char kGuildHistoryLeaveSelfFmt[] = "SELF LEAVE ( %s , %u ) total(%d) [%s]\r\n";
  const char kGuildHistoryLeavePunishFmt[] = "FORCED LEAVE ( %s , %u ) total(%d) [%s]\r\n";
  const char kGuildHistoryLeaveFmt[] = "LEAVE MEMBER ( %s , %u ) total(%d) [%s]\r\n";
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
  sprintf(m_szStdPath, "%s\\%u", m_szStdPath, korLocalTime);
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

void CMgrGuildHistory::GetNewFileName(unsigned int dwGuildSerial, char *pszFileName)
{
  sprintf(pszFileName, "%s\\%u.ghi", m_szStdPath, dwGuildSerial);
}

void CMgrGuildHistory::start_guild(CGuild *pGuild, char *pszFileName)
{
  char memberName[128]{};
  s_guildHistoryLogData[0] = 0;

  char line[128]{};
  sprintf_s(
    line,
    sizeof(line),
    kGuildHistoryStartFmt,
    pGuild->m_aszName,
    pGuild->m_dwSerial,
    pGuild->m_byGrade,
    pGuild->m_dTotalDalant,
    pGuild->m_dTotalGold,
    m_szCurTime);
  strcat_s(s_guildHistoryLogData, sizeof(s_guildHistoryLogData), line);

  sprintf_s(line, sizeof(line), kGuildHistoryMemberCountFmt, static_cast<unsigned int>(pGuild->m_nMemberNum));
  strcat_s(s_guildHistoryLogData, sizeof(s_guildHistoryLogData), line);

  for (int j = 0; j < pGuild->m_nMemberNum; ++j)
  {
    W2M(pGuild->m_MemberData[j].wszName, memberName, sizeof(memberName));
    sprintf_s(line, sizeof(line), "\t %s %u\r\n", memberName, pGuild->m_MemberData[j].dwSerial);
    strcat_s(s_guildHistoryLogData, sizeof(s_guildHistoryLogData), line);
  }

  s_guildHistoryLogData[9999] = 0;
  WriteFile(pszFileName, s_guildHistoryLogData);
}

void CMgrGuildHistory::join_member(
  char *pszJoinerName,
  unsigned int dwJoinerSerial,
  char *pszOKerName,
  unsigned int dwOKSerial,
  int nMemNum,
  char *pszFileName)
{
  s_guildHistoryLogData[0] = 0;
  sprintf_s(
    s_guildHistoryLogData,
    sizeof(s_guildHistoryLogData),
    kGuildHistoryJoinFmt,
    pszJoinerName,
    dwJoinerSerial,
    pszOKerName,
    dwOKSerial,
    nMemNum,
    m_szCurTime);
  s_guildHistoryLogData[9999] = 0;
  WriteFile(pszFileName, s_guildHistoryLogData);
}

void CMgrGuildHistory::leave_member(
  char *pszLeaverName,
  unsigned int dwLeaverSerial,
  bool bSelf,
  int nMemNum,
  char *pszFileName,
  bool bPunish)
{
  s_guildHistoryLogData[0] = 0;

  if (bSelf)
  {
    sprintf_s(
      s_guildHistoryLogData,
      sizeof(s_guildHistoryLogData),
      kGuildHistoryLeaveSelfFmt,
      pszLeaverName,
      dwLeaverSerial,
      nMemNum,
      m_szCurTime);
  }
  else if (bPunish)
  {
    sprintf_s(
      s_guildHistoryLogData,
      sizeof(s_guildHistoryLogData),
      kGuildHistoryLeavePunishFmt,
      pszLeaverName,
      dwLeaverSerial,
      nMemNum,
      m_szCurTime);
  }
  else
  {
    sprintf_s(
      s_guildHistoryLogData,
      sizeof(s_guildHistoryLogData),
      kGuildHistoryLeaveFmt,
      pszLeaverName,
      dwLeaverSerial,
      nMemNum,
      m_szCurTime);
  }

  s_guildHistoryLogData[9999] = 0;
  WriteFile(pszFileName, s_guildHistoryLogData);
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
