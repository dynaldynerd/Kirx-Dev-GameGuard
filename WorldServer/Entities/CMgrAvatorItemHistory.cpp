#include "pch.h"

#include "CMgrAvatorItemHistory.h"

#include <cstdio>
#include <process.h>
#include <windows.h>

#include "GlobalObjects.h"
#include "WorldServerUtil.h"

CMgrAvatorItemHistory::CMgrAvatorItemHistory()
{
  m_dwLastLocalDate = 0;
  m_dwLastLocalHour = 0;

  char returned[128]{};
  GetPrivateProfileStringA(
    "System",
    "HistoryPath",
    "C:\\History",
    returned,
    static_cast<DWORD>(sizeof(returned)),
    "..\\WorldInfo\\WorldInfo.ini");
  CreateDirectoryA(returned, nullptr);
  sprintf_s(m_szStdPath, "%s\\Item", returned);
  CreateDirectoryA(m_szStdPath, nullptr);

  _strdate(m_szCurDate);
  m_szCurDate[5] = '\0';
  _strtime(m_szCurTime);
  m_szCurTime[5] = '\0';

  m_tmrUpdateTime.BeginTimer(0xEA60);

  CNetIndexList::SetList(&m_listLogData_10K, 0xFE);
  CNetIndexList::SetList(&m_listLogDataEmpty_10K, 0xFE);
  for (unsigned int index = 0; index < 254; ++index)
    CNetIndexList::PushNode_Back(&m_listLogDataEmpty_10K, index);

  CNetIndexList::SetList(&m_listLogData_1K, 0xFE);
  CNetIndexList::SetList(&m_listLogDataEmpty_1K, 0xFE);
  for (unsigned int index = 0; index < 254; ++index)
    CNetIndexList::PushNode_Back(&m_listLogDataEmpty_1K, index);

  CNetIndexList::SetList(&m_listLogData_200, 0x9E4);
  CNetIndexList::SetList(&m_listLogDataEmpty_200, 0x9E4);
  for (unsigned int index = 0; index < 2532; ++index)
    CNetIndexList::PushNode_Back(&m_listLogDataEmpty_200, index);

  m_bIOThread = true;
  _beginthread(&CMgrAvatorItemHistory::IOThread, 0, this);
}

void CMgrAvatorItemHistory::IOThread(void *pv)
{
  auto *self = static_cast<CMgrAvatorItemHistory *>(pv);
  while (self->m_bIOThread)
  {
    self->m_FrameRate.CalcSpeedPerFrame();

    unsigned int outIndex = 0;
    while (CNetIndexList::PopNode_Front(&self->m_listLogData_10K, &outIndex))
    {
      __LOG_DATA_10K &entry = self->m_LogData_10K[outIndex];
      IOFileWrite_0(entry.szFileName, entry.nLen, entry.sData);
      CNetIndexList::PushNode_Back(&self->m_listLogDataEmpty_10K, outIndex);
      Sleep(0);
    }

    while (CNetIndexList::PopNode_Front(&self->m_listLogData_1K, &outIndex))
    {
      __LOG_DATA_1K &entry = self->m_LogData_1K[outIndex];
      IOFileWrite_0(entry.szFileName, entry.nLen, entry.sData);
      CNetIndexList::PushNode_Back(&self->m_listLogDataEmpty_1K, outIndex);
      Sleep(0);
    }

    while (CNetIndexList::PopNode_Front(&self->m_listLogData_200, &outIndex))
    {
      __LOG_DATA_200 &entry = self->m_LogData_200[outIndex];
      IOFileWrite_0(entry.szFileName, entry.nLen, entry.sData);
      CNetIndexList::PushNode_Back(&self->m_listLogDataEmpty_200, outIndex);
      Sleep(0);
    }

    Sleep(1);
  }

  _endthreadex(0);
}

void CMgrAvatorItemHistory::WriteFile(char *pszFileName, char *pszLog)
{
  const int logLen = static_cast<int>(strlen_0(pszLog));
  unsigned int outIndex = 0;

  if (logLen >= 0xC8)
  {
    if (logLen >= 0x3E8)
    {
      if (logLen < 0x2710 && CNetIndexList::PopNode_Front(&m_listLogDataEmpty_10K, &outIndex))
      {
        __LOG_DATA_10K &entry = m_LogData_10K[outIndex];
        strcpy_0(entry.szFileName, pszFileName);
        entry.nLen = logLen;
        memcpy_0(entry.sData, pszLog, static_cast<unsigned int>(logLen));
        entry.sData[logLen] = '\0';
        CNetIndexList::PushNode_Back(&m_listLogData_10K, outIndex);
        return;
      }
    }
    else if (CNetIndexList::PopNode_Front(&m_listLogDataEmpty_1K, &outIndex))
    {
      __LOG_DATA_1K &entry = m_LogData_1K[outIndex];
      strcpy_0(entry.szFileName, pszFileName);
      entry.nLen = logLen;
      memcpy_0(entry.sData, pszLog, static_cast<unsigned int>(logLen));
      entry.sData[logLen] = '\0';
      CNetIndexList::PushNode_Back(&m_listLogData_1K, outIndex);
      return;
    }
  }
  else if (CNetIndexList::PopNode_Front(&m_listLogDataEmpty_200, &outIndex))
  {
    __LOG_DATA_200 &entry = m_LogData_200[outIndex];
    strcpy_0(entry.szFileName, pszFileName);
    entry.nLen = logLen;
    memcpy_0(entry.sData, pszLog, static_cast<unsigned int>(logLen));
    entry.sData[logLen] = '\0';
    CNetIndexList::PushNode_Back(&m_listLogData_200, outIndex);
    return;
  }

  IOFileWrite_0(pszFileName, logLen, pszLog);
}

void CMgrAvatorItemHistory::consume_del_item(int n, _STORAGE_LIST::_db_con *pItem, char *pszFileName)
{
  auto *record = g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
  const char *upgradeInfo = DisplayItemUpgInfo(pItem->m_byTableCode, pItem->m_dwLv);

  char logBuffer[256]{};
  sprintf_s(
    logBuffer,
    "CONSUM: %s_@%s[%I64u] [%s %s]\r\n",
    record->m_strCode,
    upgradeInfo,
    pItem->m_lnUID,
    m_szCurDate,
    m_szCurTime);

  (void)n;
  WriteFile(pszFileName, logBuffer);
}
