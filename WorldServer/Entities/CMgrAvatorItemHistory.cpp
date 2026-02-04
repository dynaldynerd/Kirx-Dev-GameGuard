#include "pch.h"

#include "CMgrAvatorItemHistory.h"

#include <cstdio>
#include <cstring>
#include <ctime>
#include <process.h>
#include <windows.h>

#include "GlobalObjects.h"
#include "WorldServerUtil.h"

namespace
{
  char sData[20000]{};
  char s_personal_amine_log[20000]{};
  const char *kPersonalAmineUninstallDesc[] = {
    "TYPE0",
    "TYPE1",
    "TYPE2",
    "TYPE3"
  };
}

void CMgrAvatorItemHistory::lenditem_del_from_inven(
  unsigned __int8 byTblCode,
  unsigned __int16 wItemIndex,
  unsigned __int64 lnUID,
  char *pFN)
{
  sData[0] = '\0';

  _base_fld *record = g_Main.m_tblItemData[byTblCode].GetRecord(wItemIndex);
  __time32_t timeValue[4]{};
  _time32(timeValue);
  tm *localTime = _localtime32(timeValue);

  if (record)
  {
    const unsigned int korTime = GetKorLocalTime();
    const char *itemKorName = GetItemKorName(byTblCode, wItemIndex);
    sprintf(
      sData,
      "[DEL_LENDITEM] : %s(%s) [UID:%I64u][T:%u/%d]\r\n",
      record->m_strCode,
      itemKorName,
      lnUID,
      korTime,
      localTime->tm_sec);
  }
  else
  {
    const unsigned int korTime = GetKorLocalTime();
    sprintf(sData, "[DEL_LENDITEM] : Tbl:%d Idx:%d [UID:%I64u][%u]\r\n", byTblCode, wItemIndex, lnUID, korTime);
  }

  WriteFile(pFN, sData);
}

void CMgrAvatorItemHistory::time_jade_effect_log(
  char *pszItemName,
  _STORAGE_LIST::_db_con *pItem,
  bool bAdd,
  char *pszFileName)
{
  if (bAdd)
  {
    sprintf_s(
      sData,
      20000,
      "Item Effect Add :%s_[%I64u] [Count :%d] [%s %s]\r\n",
      pszItemName,
      pItem->m_lnUID,
      pItem->m_dwDur,
      m_szCurDate,
      m_szCurTime);
  }
  else
  {
    sprintf_s(
      sData,
      20000,
      "Item Effect Del :%s_[%I64u] [Count :%d] [%s %s]\r\n",
      pszItemName,
      pItem->m_lnUID,
      pItem->m_dwDur,
      m_szCurDate,
      m_szCurTime);
  }

  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::exp_prof_log(int count, char *szFile)
{
  memset_0(sData, 0, sizeof(sData));
  sprintf_s(sData, sizeof(sData), "[Exp_Prof_Using] :  %d \r\n", count);
  WriteFile(szFile, sData);
}

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

void CMgrAvatorItemHistory::WriteFile(const char *pszFileName, const char *pszLog)
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

  IOFileWrite_0(
    const_cast<char *>(pszFileName),
    logLen,
    const_cast<char *>(pszLog));
}

void CMgrAvatorItemHistory::consume_del_item(int n, _STORAGE_LIST::_db_con *pItem, const char *pszFileName)
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

void CMgrAvatorItemHistory::personal_amine_itemlog(
  const char *szLogDesc,
  unsigned __int8 byPos,
  unsigned __int8 byTblCode,
  unsigned __int16 wItemIndex,
  unsigned int dwDur,
  const char *szFileName)
{
  s_personal_amine_log[0] = '\0';
  const char *itemName = GetItemKorName(byTblCode, wItemIndex);
  sprintf_s(
    s_personal_amine_log,
    sizeof(s_personal_amine_log),
    "[%s] %s(pos:%d code:%d idx:%d) dur(%d)\r\n",
    szLogDesc,
    itemName,
    static_cast<int>(byPos),
    static_cast<int>(byTblCode),
    static_cast<int>(wItemIndex),
    static_cast<int>(dwDur));
  WriteFile(szFileName, s_personal_amine_log);
}

void CMgrAvatorItemHistory::personal_amine_uninstall(
  unsigned __int8 byType,
  const unsigned int *pdwMineCnt,
  int nMaxOreNum,
  _STORAGE_LIST::_db_con *pItem,
  const char *szFileName)
{
  std::memset(s_personal_amine_log, 0, sizeof(s_personal_amine_log));
  const char *itemName = GetItemKorName(pItem->m_byTableCode, pItem->m_wItemIndex);
  sprintf_s(
    s_personal_amine_log,
    sizeof(s_personal_amine_log),
    "[PERSONAL_AMINE_UNINSTALL][%s] - %s[%I64u]\r\n",
    kPersonalAmineUninstallDesc[byType],
    itemName,
    pItem->m_lnUID);

  char *buffer = &s_personal_amine_log[strlen_0(s_personal_amine_log)];
  for (int nItemIndex = 0; nItemIndex < nMaxOreNum; ++nItemIndex)
  {
    if (pdwMineCnt[nItemIndex])
    {
      const char *oreName = GetItemKorName(17, nItemIndex);
      const size_t remaining = sizeof(s_personal_amine_log) - static_cast<size_t>(buffer - s_personal_amine_log);
      sprintf_s(
        buffer,
        remaining,
        "%s >> num:%d\r\n",
        oreName,
        static_cast<int>(pdwMineCnt[nItemIndex]));
      buffer += strlen_0(buffer);
    }
  }

  WriteFile(szFileName, s_personal_amine_log);
}
