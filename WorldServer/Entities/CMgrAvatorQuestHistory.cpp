#include "pch.h"

#include "CMgrAvatorQuestHistory.h"

#include <cstdio>
#include <cstring>
#include <process.h>

#include "CQuestMgr.h"
#include "WorldServerUtil.h"

namespace
{
constexpr char kHistoryIniSection[] = "System";
constexpr char kHistoryPathKey[] = "HistoryPath";
constexpr char kHistoryPathDefault[] = "c:\\History";
constexpr char kHistoryIniFile[] = "..\\WorldInfo\\WorldInfo.ini";
constexpr char kHistoryQuestSubdirFmt[] = "%s\\Quest";
static constexpr size_t kHistoryFileNameSize = 64;
}

CMgrAvatorQuestHistory::CMgrAvatorQuestHistory()
{
  m_dwLastLocalDate = 0;
  m_dwLastLocalHour = 0;

  char returnedString[132]{};
  GetPrivateProfileStringA(
    kHistoryIniSection,
    kHistoryPathKey,
    kHistoryPathDefault,
    returnedString,
    128,
    kHistoryIniFile);
  CreateDirectoryA(returnedString, nullptr);

  sprintf_s(m_szStdPath, sizeof(m_szStdPath), kHistoryQuestSubdirFmt, returnedString);
  CreateDirectoryA(m_szStdPath, nullptr);

  _strtime(m_szCurTime);
  m_szCurTime[5] = 0;

  m_tmrUpdateTime.BeginTimer(60000);
  m_listLogData.SetList(2532);
  m_listLogDataEmpty.SetList(2532);

  for (unsigned int index = 0; index < 2532; ++index)
  {
    m_listLogDataEmpty.PushNode_Back(index);
  }

  m_bIOThread = true;
  _beginthread(&CMgrAvatorQuestHistory::IOThread, 0, this);
}

CMgrAvatorQuestHistory::~CMgrAvatorQuestHistory()
{
  m_bIOThread = false;
}

void __cdecl CMgrAvatorQuestHistory::IOThread(void *pv)
{
  CMgrAvatorQuestHistory *const history = static_cast<CMgrAvatorQuestHistory *>(pv);
  unsigned int outIndex[5]{};
  while (history->m_bIOThread)
  {
    history->m_FrameRate.CalcSpeedPerFrame();
    while (history->m_listLogData.PopNode_Front(outIndex))
    {
      __LOG_DATA &entry = history->m_LogData[outIndex[0]];
      IOFileWrite_2(entry.szFileName, static_cast<unsigned int>(entry.nLen), entry.sData);
      history->m_listLogDataEmpty.PushNode_Back(outIndex[0]);
      Sleep(0);
    }
    Sleep(1u);
  }

  _endthreadex(0);
}

void CMgrAvatorQuestHistory::OnLoop()
{
  if (m_tmrUpdateTime.CountingTimer())
  {
    _strtime(m_szCurTime);
    m_szCurTime[5] = 0;
  }
}

int CMgrAvatorQuestHistory::GetTotalWaitSize()
{
  // narrowing cast for thunk return parity
  return static_cast<int>(m_listLogData.size());
}

void CMgrAvatorQuestHistory::WriteFile(char *pszFileName, char *pszLog)
{
  const unsigned int size = std::strlen(pszLog);
  unsigned int outIndex[4]{};
  if (size < 0x12C && m_listLogDataEmpty.PopNode_Front(outIndex))
  {
    __LOG_DATA *entry = &m_LogData[outIndex[0]];
    std::strcpy(entry->szFileName, pszFileName);
    entry->nLen = size;
    std::memcpy(entry->sData, pszLog, size);
    entry->sData[size] = 0;
    m_listLogData.PushNode_Back(outIndex[0]);
    return;
  }

  IOFileWrite_2(pszFileName, size, pszLog);
}

void CMgrAvatorQuestHistory::GetNewFileName(unsigned int dwAvatorSerial, char *pszFileName)
{
  const unsigned int localDate = GetLocalDate();
  char buffer[132]{};
  sprintf_s(buffer, sizeof(buffer), "%s\\%d", m_szStdPath, localDate);
  CreateDirectoryA(buffer, nullptr);
  m_dwLastLocalDate = localDate;

  const unsigned int currentHour = GetCurrentHour();
  char pathName[148]{};
  sprintf_s(pathName, sizeof(pathName), "%s\\%d\\%d", m_szStdPath, m_dwLastLocalDate, currentHour);
  CreateDirectoryA(pathName, nullptr);
  m_dwLastLocalHour = currentHour;

  char hourStr[32]{};
  if (currentHour <= 9)
  {
    sprintf_s(hourStr, sizeof(hourStr), "0%d", currentHour);
  }
  else
  {
    sprintf_s(hourStr, sizeof(hourStr), "%d", currentHour);
  }

  const unsigned int currentMin = GetCurrentMin();
  char minStr[32]{};
  if (currentMin <= 9)
  {
    sprintf_s(minStr, sizeof(minStr), "0%d", currentMin);
  }
  else
  {
    sprintf_s(minStr, sizeof(minStr), "%d", currentMin);
  }

  const unsigned int currentSec = GetCurrentSec();
  char secStr[16]{};
  if (currentSec <= 9)
  {
    sprintf_s(secStr, sizeof(secStr), "0%d", currentSec);
  }
  else
  {
    sprintf_s(secStr, sizeof(secStr), "%d", currentSec);
  }

  char timeStr[32]{};
  sprintf_s(timeStr, sizeof(timeStr), "%s%s%s", hourStr, minStr, secStr);
  sprintf_s(
    pszFileName,
    kHistoryFileNameSize,
    "%s\\%d\\%d\\%d_%s.lhi",
    m_szStdPath,
    m_dwLastLocalDate,
    m_dwLastLocalHour,
    dwAvatorSerial,
    timeStr);
}

void CMgrAvatorQuestHistory::init_quest(char *pszAvatorName, _QUEST_DB_BASE *pQuestDB, char *pszFileName)
{
  char sData[10000]{};
  char sBuf[128]{};
  sprintf_s(sBuf, sizeof(sBuf), "%s [%s]\n", pszAvatorName, m_szCurTime);
  std::strcat(sData, sBuf);
  sprintf_s(sBuf, sizeof(sBuf), "START\n");
  std::strcat(sData, sBuf);
  for (int slotIndex = 0; slotIndex < 30; ++slotIndex)
  {
    _QUEST_DB_BASE::_LIST *quest = &pQuestDB->m_List[slotIndex];
    if (quest->byQuestType != 255)
    {
      _base_fld *record = CQuestMgr::s_tblQuest->GetRecord(quest->wIndex);
      if (record)
      {
        sprintf_s(sBuf, sizeof(sBuf), "\t%d: %s\n", slotIndex, record->m_strCode);
        std::strcat(sData, sBuf);
      }
    }
  }
  sprintf_s(sBuf, sizeof(sBuf), "\n");
  std::strcat(sData, sBuf);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorQuestHistory::insert_quest(unsigned int nSlot, char *pszQuestCode, char *pszFileName)
{
  char sData[10000]{};
  sprintf_s(sData, sizeof(sData), "ADD: %d: %s [%s]\n", nSlot, pszQuestCode, m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorQuestHistory::complete_quest(unsigned int nSlot, char *pszQuestCode, char *pszFileName)
{
  char sData[10000]{};
  sprintf_s(sData, sizeof(sData), "END: %d: %s [%s]\n", nSlot, pszQuestCode, m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorQuestHistory::fail_quest(
  unsigned int nSlot,
  char *pszQuestCode,
  char *pszFailCode,
  char *pszFileName)
{
  char sData[10000]{};
  sprintf_s(
    sData,
    sizeof(sData),
    "FAIL: %d: %s (%s) [%s]\n",
    nSlot,
    pszQuestCode,
    pszFailCode,
    m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorQuestHistory::char_copy(char *pszDstName, unsigned int dwDstSerial, char *pszFileName)
{
  char sData[10000]{};
  sprintf_s(sData, sizeof(sData), "Char Copy: dst(%s:%d)\n", pszDstName, dwDstSerial);
  WriteFile(pszFileName, sData);
}

