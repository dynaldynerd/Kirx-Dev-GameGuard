#include "pch.h"

#include "CMgrAccountLobbyHistory.h"

#include <cstdio>
#include <process.h>
#include <winsock2.h>

#include "CMainThread.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "qry_logout.h"

namespace
{
  char sLData[20000];
  char sLBuf[10240];
}

CMgrAccountLobbyHistory::CMgrAccountLobbyHistory()
  : m_dwLastLocalDate(0),
    m_dwLastLocalHour(0),
    m_bIOThread(true)
{
  char returnedString[128]{};
  GetPrivateProfileStringA(
    "System",
    "HistoryPath",
    "C:\\History",
    returnedString,
    128,
    "..\\WorldInfo\\WorldInfo.ini");
  CreateDirectoryA(returnedString, nullptr);
  sprintf_s(m_szStdPath, "%s\\Lobby", returnedString);
  CreateDirectoryA(m_szStdPath, nullptr);

  _strdate(m_szCurDate);
  m_szCurDate[5] = 0;
  _strtime(m_szCurTime);
  m_szCurTime[5] = 0;

  m_tmrUpdateTime.BeginTimer(60000);

  m_listLogData_10K.SetList(254);
  m_listLogDataEmpty_10K.SetList(254);
  for (unsigned int index = 0; index < 254; ++index)
  {
    m_listLogDataEmpty_10K.PushNode_Back(index);
  }

  m_listLogData_1K.SetList(254);
  m_listLogDataEmpty_1K.SetList(254);
  for (unsigned int index = 0; index < 254; ++index)
  {
    m_listLogDataEmpty_1K.PushNode_Back(index);
  }

  m_listLogData_200.SetList(2532);
  m_listLogDataEmpty_200.SetList(2532);
  for (unsigned int index = 0; index < 2532; ++index)
  {
    m_listLogDataEmpty_200.PushNode_Back(index);
  }

  _beginthread(reinterpret_cast<_beginthread_proc_type>(&CMgrAccountLobbyHistory::IOThread), 0, this);
}

CMgrAccountLobbyHistory::~CMgrAccountLobbyHistory()
{
  m_bIOThread = false;
}

void CMgrAccountLobbyHistory::GetNewFileName(unsigned int dwAccountSerial, char *pszFileName)
{
  const unsigned int localDate = GetLocalDate();
  char buffer[132]{};
  sprintf(buffer, "%s\\%d", m_szStdPath, localDate);
  CreateDirectoryA(buffer, nullptr);
  m_dwLastLocalDate = localDate;

  const unsigned int hour = GetCurrentHour();
  char pathName[148]{};
  sprintf(pathName, "%s\\%d\\%d", m_szStdPath, m_dwLastLocalDate, hour);
  CreateDirectoryA(pathName, nullptr);
  m_dwLastLocalHour = hour;

  char hourText[32]{};
  char minText[32]{};
  char secText[16]{};
  if (hour <= 9)
  {
    sprintf(hourText, "0%d", hour);
  }
  else
  {
    sprintf(hourText, "%d", hour);
  }

  const unsigned int minute = GetCurrentMin();
  if (minute <= 9)
  {
    sprintf(minText, "0%d", minute);
  }
  else
  {
    sprintf(minText, "%d", minute);
  }

  const unsigned int second = GetCurrentSec();
  if (second <= 9)
  {
    sprintf(secText, "0%d", second);
  }
  else
  {
    sprintf(secText, "%d", second);
  }

  char timeText[32]{};
  sprintf(timeText, "%s%s%s", hourText, minText, secText);
  sprintf(
    pszFileName,
    "%s\\%d\\%d\\%d_%s.his",
    m_szStdPath,
    m_dwLastLocalDate,
    m_dwLastLocalHour,
    dwAccountSerial,
    timeText);
}

void CMgrAccountLobbyHistory::enter_lobby(
  unsigned int dwAccountSerial,
  char *pAccountID,
  unsigned __int8 byUserDgr,
  unsigned int dwIpAddress,
  bool bFirst,
  char *pszFileName)
{
  sLData[0] = 0;
  if (bFirst)
  {
    strcat_s(sLData, "[First Enter Lobby]\r\n");
  }
  else
  {
    strcat_s(sLData, "[ReEnter Lobby]\r\n");
  }

  in_addr addr{};
  addr.S_un.S_addr = dwIpAddress;
  const char *ipText = inet_ntoa(addr);
  sprintf_s(
    sLBuf,
    "ID: %s [%s %s]\r\nWORLD: %s\r\nSR: %d\r\nDGR: %d\r\nIP: %s\r\n\r\n",
    pAccountID,
    m_szCurDate,
    m_szCurTime,
    g_Main.m_szWorldName,
    dwAccountSerial,
    byUserDgr,
    ipText);
  strcat_s(sLData, sLBuf);
  strcat_s(sLData, "\r\n\t============\r\n\r\n");
  WriteFile(pszFileName, sLData);
}

void CMgrAccountLobbyHistory::lobby_disconnect(_qry_case_lobby_logout *pRegeData, char *pszFileName)
{
  sLData[0] = 0;
  const bool ok = pRegeData->byDBRet == 0;
  const char *result = ok ? "SUCCESS" : "ERROR";
  sprintf_s(
    sLBuf,
    "Lobby Logout RegedDB Result: %d (%s)\r\n",
    pRegeData->byDBRet,
    result);
  strcat_s(sLData, sLBuf);
  sprintf_s(sLBuf, "AccountSerial: %d\r\n", pRegeData->dwAccountSerial);
  strcat_s(sLData, sLBuf);
  if (ok)
  {
    sprintf_s(sLBuf, "CharNum: %d\r\n", pRegeData->nRegeNum);
    strcat_s(sLData, sLBuf);
    for (int j = 0; j < 3; ++j)
    {
      if (pRegeData->RegeList[j].bySlotIndex != 255)
      {
        sprintf_s(
          sLBuf,
          "[Slot%d]\r\nNAME: %s\r\nCharSR: %d\r\nLV: %d\r\n$D: %d\r\n$G: %d\r\n\r\n",
          j,
          pRegeData->RegeList[j].szCharName,
          pRegeData->RegeList[j].dwCharSerial,
          pRegeData->RegeList[j].nLevel,
          pRegeData->RegeList[j].dwDalant,
          pRegeData->RegeList[j].dwGold);
        strcat_s(sLData, sLBuf);
      }
    }
  }

  sprintf_s(
    sLBuf,
    "Lobby Logout RegedDB Complete [%s %s]\r\n",
    m_szCurDate,
    m_szCurTime);
  strcat_s(sLData, sLBuf);
  strcat_s(sLData, "\r\n\t============\r\n\r\n");
  WriteFile(pszFileName, sLData);
}

void CMgrAccountLobbyHistory::reged_char_request(char *pszFileName)
{
  sLData[0] = '\0';
  sprintf_s(sLBuf, "Regist Character Request [%s %s]\r\n", m_szCurDate, m_szCurTime);
  strcat_s(sLData, sLBuf);
  WriteFile(pszFileName, sLData);
}

void CMgrAccountLobbyHistory::add_char_request(char *pszFileName)
{
  sLData[0] = '\0';
  sprintf_s(sLBuf, "Add Character Request [%s %s]\r\n", m_szCurDate, m_szCurTime);
  strcat_s(sLData, sLBuf);
  WriteFile(pszFileName, sLData);
}

void CMgrAccountLobbyHistory::del_char_request(
  unsigned __int8 bySlotIndex,
  unsigned int dwAvatorSerial,
  char *pszFileName)
{
  sLData[0] = '\0';
  sprintf_s(
    sLBuf,
    "Del Character Request [%s %s]\r\nSlot: %d\r\nCharSR: %d\r\n",
    m_szCurDate,
    m_szCurTime,
    bySlotIndex,
    dwAvatorSerial);
  strcat_s(sLData, sLBuf);
  WriteFile(pszFileName, sLData);
}

void CMgrAccountLobbyHistory::sel_char_request(
  unsigned __int8 bySlotIndex,
  unsigned int dwAvatorSerial,
  char *pszFileName)
{
  sLData[0] = '\0';
  sprintf_s(
    sLBuf,
    "Select Character Request [%s %s]\r\nSlot: %d\r\nCharSR: %d\r\n",
    m_szCurDate,
    m_szCurTime,
    bySlotIndex,
    dwAvatorSerial);
  strcat_s(sLData, sLBuf);
  WriteFile(pszFileName, sLData);
}

void CMgrAccountLobbyHistory::tutorial_process_report_recv(char *pszFileName)
{
  sLData[0] = '\0';
  sprintf_s(sLBuf, "Tutorial Process Report Received [%s %s]\r\n", m_szCurDate, m_szCurTime);
  strcat_s(sLData, sLBuf);
  strcat_s(sLData, "\r\n\t============\r\n\r\n");
  WriteFile(pszFileName, sLData);
}

void CMgrAccountLobbyHistory::recovery_char_complete(
  unsigned __int8 byRetCode,
  _REGED *pAvator,
  char *pszFileName)
{
  sLData[0] = 0;
  const bool ok = byRetCode == 0;
  const char *result = ok ? "SUCCESS" : "ERROR";
  sprintf_s(
    sLBuf,
    "Recovery Character Result: %d (%s) [%s %s]\r\n",
    byRetCode,
    result,
    m_szCurDate,
    m_szCurTime);
  strcat_s(sLData, sLBuf);
  if (ok)
  {
    sprintf_s(
      sLBuf,
      "[Slot%d]\r\nNAME: %s\r\nCharSR: %d\r\nLV: %d\r\n$D: %d\r\n$G: %d\r\n\r\n",
      pAvator->m_bySlotIndex,
      pAvator->m_wszAvatorName,
      pAvator->m_dwRecordNum,
      pAvator->m_byLevel,
      pAvator->m_dwDalant,
      pAvator->m_dwGold);
    strcat_s(sLData, sLBuf);
  }
  strcat_s(sLData, "\r\n\t============\r\n\r\n");
  WriteFile(pszFileName, sLData);
}

void CMgrAccountLobbyHistory::reged_char_complete(
  unsigned __int8 byRetCode,
  int nCharNum,
  _REGED *pRegedList,
  char *pszFileName)
{
  sLData[0] = 0;
  const bool ok = byRetCode == 0;
  const char *result = ok ? "SUCCESS" : "ERROR";
  sprintf_s(sLBuf, "Reged Result: %d (%s)\r\n", byRetCode, result);
  strcat_s(sLData, sLBuf);
  if (ok)
  {
    sprintf_s(sLBuf, "CharNum: %d\r\n", nCharNum);
    strcat_s(sLData, sLBuf);
    for (int j = 0; j < 3; ++j)
    {
      if (pRegedList[j].m_bySlotIndex != 255)
      {
        sprintf_s(
          sLBuf,
          "[Slot%d]\r\nNAME: %s\r\nCharSR: %d\r\nLV: %d\r\n$D: %d\r\n$G: %d\r\n\r\n",
          j,
          pRegedList[j].m_wszAvatorName,
          pRegedList[j].m_dwRecordNum,
          pRegedList[j].m_byLevel,
          pRegedList[j].m_dwDalant,
          pRegedList[j].m_dwGold);
        strcat_s(sLData, sLBuf);
      }
    }
  }
  sprintf_s(
    sLBuf,
    "Regist Character Complete [%s %s]\r\n",
    m_szCurDate,
    m_szCurTime);
  strcat_s(sLData, sLBuf);
  strcat_s(sLData, "\r\n\t============\r\n\r\n");
  WriteFile(pszFileName, sLData);
}

void CMgrAccountLobbyHistory::add_char_complete(
  unsigned __int8 byRetCode,
  _REGED_AVATOR_DB *pInsertData,
  char *pszFileName)
{
  sLData[0] = 0;
  const bool ok = byRetCode == 0;
  const char *result = ok ? "SUCCESS" : "ERROR";
  sprintf_s(sLBuf, "Add Result: %d (%s)\r\n", byRetCode, result);
  strcat_s(sLData, sLBuf);
  if (ok)
  {
    sprintf_s(
      sLBuf,
      "[Slot%d]\r\nNAME: %s\r\nCharSR: %d\r\nLV: %d\r\n$D: %d\r\n$G: %d\r\n\r\n",
      pInsertData->m_bySlotIndex,
      pInsertData->m_wszAvatorName,
      pInsertData->m_dwRecordNum,
      pInsertData->m_byLevel,
      pInsertData->m_dwDalant,
      pInsertData->m_dwGold);
    strcat_s(sLData, sLBuf);
  }
  sprintf_s(sLBuf, "Add Character Complete [%s %s]\r\n", m_szCurDate, m_szCurTime);
  strcat_s(sLData, sLBuf);
  strcat_s(sLData, "\r\n\t============\r\n\r\n");
  WriteFile(pszFileName, sLData);
}

void CMgrAccountLobbyHistory::del_char_complete(unsigned __int8 byRetCode, char *pszFileName)
{
  sLData[0] = 0;
  const bool ok = byRetCode == 0;
  const char *result = ok ? "SUCCESS" : "ERROR";
  sprintf_s(sLBuf, "Del Result: %d (%s)\r\n", byRetCode, result);
  strcat_s(sLData, sLBuf);
  sprintf_s(sLBuf, "Del Character Complete [%s %s]\r\n", m_szCurDate, m_szCurTime);
  strcat_s(sLData, sLBuf);
  strcat_s(sLData, "\r\n\t============\r\n\r\n");
  WriteFile(pszFileName, sLData);
}

void CMgrAccountLobbyHistory::sel_char_complete(
  unsigned __int8 byRetCode,
  _AVATOR_DATA *pAvator,
  unsigned int dwAddDalant,
  unsigned int dwAddGold,
  char *pszFileName)
{
  sLData[0] = 0;
  const bool ok = byRetCode == 0;
  const char *result = ok ? "SUCCESS" : "ERROR";
  sprintf_s(sLBuf, "Select Result: %d (%s)\r\n", byRetCode, result);
  strcat_s(sLData, sLBuf);
  if (ok)
  {
    sprintf_s(
      sLBuf,
      "[Slot%d]\r\nNAME: %s\r\nCharSR: %d\r\nLV: %d\r\n$D: %d Charge(%d)\r\n$G: %d Charge(%d)\r\n\r\n",
      pAvator->dbAvator.m_bySlotIndex,
      pAvator->dbAvator.m_wszAvatorName,
      pAvator->dbAvator.m_dwRecordNum,
      pAvator->dbAvator.m_byLevel,
      pAvator->dbAvator.m_dwDalant - dwAddDalant,
      dwAddDalant,
      pAvator->dbAvator.m_dwGold - dwAddGold,
      dwAddGold);
    strcat_s(sLData, sLBuf);
  }
  strcat_s(sLData, "\r\n\t============\r\n\r\n");
  WriteFile(pszFileName, sLData);
}

void CMgrAccountLobbyHistory::player_create(bool bFirstStart, _AVATOR_DATA *pAvator, char *pszFileName)
{
  sLData[0] = 0;
  const char *createState = bFirstStart ? "First Connect Character" : "Old Character";
  sprintf_s(sLBuf, "Player Create: %s [%s %s]\r\n", createState, m_szCurDate, m_szCurTime);
  strcat_s(sLData, sLBuf);

  if (bFirstStart && (pAvator->dbAvator.m_dwDalant || pAvator->dbAvator.m_dwGold))
  {
    sprintf_s(
      sLBuf,
      "WARNNING : First Connect Char Money Wrong : $D(%d), $G(%d)",
      pAvator->dbAvator.m_dwDalant,
      pAvator->dbAvator.m_dwGold);
    strcat_s(sLData, sLBuf);
  }

  strcat_s(sLData, "\r\n\t============\r\n\r\n");
  WriteFile(pszFileName, sLData);
}

void CMgrAccountLobbyHistory::player_create_complete_money(_AVATOR_DATA *pAvator, char *pszFileName)
{
  sLData[0] = 0;
  sprintf_s(
    sLBuf,
    "Create Complete Player Money: $D(%d), $G(%d)\r\n",
    pAvator->dbAvator.m_dwDalant,
    pAvator->dbAvator.m_dwGold);
  strcat_s(sLData, sLBuf);
  strcat_s(sLData, "\r\n\t============\r\n\r\n");
  WriteFile(pszFileName, sLData);
}

void CMgrAccountLobbyHistory::player_money_fix(
  unsigned int dwOldDalant,
  unsigned int dwOldGold,
  _AVATOR_DATA *pAvator,
  char *pszFileName)
{
  sLData[0] = 0;
  sprintf_s(
    sLBuf,
    "Player Money FIX: $D(%d -> %d), $G(%d -> %d) [%s %s]\r\n",
    dwOldDalant,
    pAvator->dbAvator.m_dwDalant,
    dwOldGold,
    pAvator->dbAvator.m_dwGold,
    m_szCurDate,
    m_szCurTime);
  strcat_s(sLData, sLBuf);
  strcat_s(sLData, "\r\n\t============\r\n\r\n");
  WriteFile(pszFileName, sLData);
}

void CMgrAccountLobbyHistory::WriteFile(char *pszFileName, char *pszLog)
{
  unsigned int outIndex[4]{};
  const int size = static_cast<int>(strlen_0(pszLog));
  if (size >= 200)
  {
    if (size >= 1000)
    {
      if (size < 0x2710 && m_listLogDataEmpty_10K.PopNode_Front(outIndex))
      {
        __LOG_DATA_10K &entry = m_LogData_10K[outIndex[0]];
        strcpy_0(entry.szFileName, pszFileName);
        entry.nLen = size;
        memcpy_0(entry.sData, pszLog, static_cast<unsigned int>(size));
        entry.sData[size] = 0;
        m_listLogData_10K.PushNode_Back(outIndex[0]);
        return;
      }
    }
    else if (m_listLogDataEmpty_1K.PopNode_Front(outIndex))
    {
      __LOG_DATA_1K &entry = m_LogData_1K[outIndex[0]];
      strcpy_0(entry.szFileName, pszFileName);
      entry.nLen = size;
      memcpy_0(entry.sData, pszLog, static_cast<unsigned int>(size));
      entry.sData[size] = 0;
      m_listLogData_1K.PushNode_Back(outIndex[0]);
      return;
    }
  }
  else if (m_listLogDataEmpty_200.PopNode_Front(outIndex))
  {
    __LOG_DATA_200 &entry = m_LogData_200[outIndex[0]];
    strcpy_0(entry.szFileName, pszFileName);
    entry.nLen = size;
    memcpy_0(entry.sData, pszLog, static_cast<unsigned int>(size));
    entry.sData[size] = 0;
    m_listLogData_200.PushNode_Back(outIndex[0]);
    return;
  }

  IOFileWrite(pszFileName, size, pszLog);
}

void CMgrAccountLobbyHistory::OnLoop()
{
  if (m_tmrUpdateTime.CountingTimer())
  {
    _strdate(m_szCurDate);
    m_szCurDate[5] = 0;
    _strtime(m_szCurTime);
    m_szCurTime[5] = 0;
  }
}

void CMgrAccountLobbyHistory::IOThread(char *pv)
{
  auto *self = reinterpret_cast<CMgrAccountLobbyHistory *>(pv);
  unsigned int outIndex[5]{};

  while (self->m_bIOThread)
  {
    self->m_FrameRate.CalcSpeedPerFrame();
    while (self->m_listLogData_10K.PopNode_Front(outIndex))
    {
      __LOG_DATA_10K &entry = self->m_LogData_10K[outIndex[0]];
      IOFileWrite(entry.szFileName, entry.nLen, entry.sData);
      self->m_listLogDataEmpty_10K.PushNode_Back(outIndex[0]);
      Sleep(0);
    }
    while (self->m_listLogData_1K.PopNode_Front(outIndex))
    {
      __LOG_DATA_1K &entry = self->m_LogData_1K[outIndex[0]];
      IOFileWrite(entry.szFileName, entry.nLen, entry.sData);
      self->m_listLogDataEmpty_1K.PushNode_Back(outIndex[0]);
      Sleep(0);
    }
    while (self->m_listLogData_200.PopNode_Front(outIndex))
    {
      __LOG_DATA_200 &entry = self->m_LogData_200[outIndex[0]];
      IOFileWrite(entry.szFileName, entry.nLen, entry.sData);
      self->m_listLogDataEmpty_200.PushNode_Back(outIndex[0]);
      Sleep(0);
    }
    Sleep(1);
  }
  _endthreadex(0);
}
