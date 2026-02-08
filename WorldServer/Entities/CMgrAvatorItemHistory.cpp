#include "pch.h"

#include "CMgrAvatorItemHistory.h"

#include <cstdio>
#include <cstring>
#include <ctime>
#include <process.h>
#include <windows.h>

#include "CPostData.h"
#include "CPostReturnStorage.h"
#include "CPostStorage.h"
#include "CRecordData.h"
#include "CUnmannedTraderRegistItemInfo.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

namespace
{
  char sData[20000]{};
  char sBuf[0x2800]{};
  char s_personal_amine_log[20000]{};
  const char *pRace_0[3] = {
    "Bellato",
    "Cora",
    "Accratia"
  };
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

void CMgrAvatorItemHistory::mastery_change_jade(
  int nMstCode,
  unsigned int dwOldCum,
  unsigned int dwNewCum,
  int nLv,
  float fVal,
  char *szFileName,
  int nWpType)
{
  char category[56]{};
  switch (nMstCode)
  {
    case 0:
      if (nWpType)
      {
        strcpy_0(category, "mastery_weapon_long");
      }
      else
      {
        strcpy_0(category, "mastery_weapon_close");
      }
      break;
    case 1:
      strcpy_0(category, "mastery_code_defence");
      break;
    case 2:
      strcpy_0(category, "mastery_code_shield");
      break;
    case 3:
      strcpy_0(category, "mastery_code_shield");
      break;
    case 4:
      strcpy_0(category, "mastery_code_force");
      break;
    case 6:
      strcpy_0(category, "mastery_code_special");
      break;
    default:
      break;
  }

  sprintf_s(
    sData,
    sizeof(sData),
    "MasteryChange(%s) : %d Lv[%d] => %d Lv[%d] \r\n",
    category,
    nLv,
    dwOldCum,
    static_cast<unsigned int>(static_cast<float>(nLv) + fVal),
    dwNewCum);
  WriteFile(szFileName, sData);
}

void CMgrAvatorItemHistory::have_auto_item(
  int n,
  const CUnmannedTraderRegistItemInfo *pkInfo,
  unsigned __int8 byMaxCnt)
{
  (void)n;

  if (pkInfo && byMaxCnt)
  {
    tm *localTime = nullptr;
    char buffer[260]{};
    std::memset(buffer, 0, 256);
    char hasData = 0;

    for (unsigned __int8 index = 0; index < byMaxCnt; ++index)
    {
      auto *info = const_cast<CUnmannedTraderRegistItemInfo *>(&pkInfo[index]);
      if (info->IsRegist())
      {
        if (!hasData)
        {
          strcat_0(sData, "\r\n== UnmannedTrader Regist Info ==\r\n");
        }
        ++hasData;

        const int itemIndex = info->GetItemIndex();
        const unsigned __int8 tableCode = info->GetTableCode();
        _base_fld *record = g_Main.m_tblItemData[tableCode].GetRecord(itemIndex);
        const __int64 *startTimePtr = info->GetStartTimePtr();
        localTime = localtime_5(startTimePtr);
        if (localTime)
        {
          std::sprintf(
            buffer,
            "%04d-%02d-%02d %02d:%02d:%02d",
            localTime->tm_year + 1900,
            localTime->tm_mon + 1,
            localTime->tm_mday,
            localTime->tm_hour,
            localTime->tm_min,
            localTime->tm_sec);
        }
        else
        {
          const int startTime = info->GetStartTime();
          std::sprintf(buffer, "invalid(%u)", startTime);
        }

        const int sellTurn = info->GetSellTurm();
        const unsigned int price = info->GetPrice();
        const unsigned int etSerial = info->GetETSerial();
        const unsigned int dwLvBit = info->GetU();
        const unsigned __int8 upgTableCode = info->GetTableCode();
        char *upgInfo = DisplayItemUpgInfo(upgTableCode, dwLvBit);
        const unsigned __int64 dValue = info->GetD();
        char *code = record->m_strCode;
        const int storageIndex = info->GetStorageIndex();
        const unsigned int registSerial = info->GetRegistSerial();

        std::sprintf(
          sBuf,
          "%u : reg(%u) storageinx(%u) %s_%u_@%s[%u] price(%u) regtime(%s) sellturm(%u) \r\n",
          index,
          registSerial,
          storageIndex,
          code,
          dValue,
          upgInfo,
          etSerial,
          price,
          buffer,
          sellTurn);
        strcat_0(sData, sBuf);
      }
    }

    if (hasData)
    {
      strcat_0(sData, "================================\r\n\r\n");
    }
  }
  else
  {
    sprintf_s(
      sBuf,
      0x2800uLL,
      "\r\n== UnmannedTrader Regist Info pkInfo(%p) byMacCnt(%u) ==\r\n",
      pkInfo,
      byMaxCnt);
    strcat_s(sData, 0x4E20uLL, sBuf);
  }
}

void CMgrAvatorItemHistory::have_item_close(
  int n,
  char *pszName,
  _AVATOR_DATA *pLoadData,
  _AVATOR_DATA *pBackupData,
  char *pszID,
  unsigned int dwIDSerial,
  unsigned __int8 byDgr,
  unsigned int dwIP,
  unsigned int dwExpRate,
  const CUnmannedTraderRegistItemInfo *pkInfo,
  unsigned __int8 byMaxCnt,
  char *pszFileName)
{
  (void)n;
  (void)pszName;
  (void)pBackupData;
  (void)pszID;
  (void)byDgr;
  (void)dwIP;

  sData[0] = 0;
  g_Main.m_tblClass.GetRecord(pLoadData->dbAvator.m_zClassHistory[0]);
  g_Main.m_tblClass.GetRecord(pLoadData->dbAvator.m_zClassHistory[1]);
  g_Main.m_tblClass.GetRecord(pLoadData->dbAvator.m_zClassHistory[2]);
  strcat_0(sData, "\r\n\t============\r\n\r\n");
  have_auto_item(n, pkInfo, byMaxCnt);

  std::sprintf(
    sBuf,
    "LV: %d\r\nXP: %.0f (%d)\r\n$D: %u\r\n$G: %u\r\nPvP: %.0f\r\nCB: %.0f\r\nTIME: %d\r\n\r\n",
    pLoadData->dbAvator.m_byLevel,
    static_cast<double>(pLoadData->dbAvator.m_dExp),
    dwExpRate,
    pLoadData->dbAvator.m_dwDalant,
    pLoadData->dbAvator.m_dwGold,
    static_cast<double>(pLoadData->dbAvator.m_dPvPPoint),
    static_cast<double>(pLoadData->dbAvator.m_dPvPCashBag),
    pLoadData->dbAvator.m_dwTotalPlayMin);
  strcat_0(sData, sBuf);

  std::sprintf(sBuf, "EQUIP\r\n");
  strcat_0(sData, sBuf);
  for (int tableCode = 0; tableCode < 8; ++tableCode)
  {
    _EQUIPKEY *equipKey = &pLoadData->dbAvator.m_EquipKey[tableCode];
    if (equipKey->IsFilled())
    {
      _base_fld *record = g_Main.m_tblItemData[tableCode].GetRecord(equipKey->zItemIndex);
      const char *upgInfo = DisplayItemUpgInfo(tableCode, pLoadData->dbAvator.m_dwFixEquipLv[tableCode]);
      std::sprintf(
        sBuf,
        "\t%s_@%s[%I64u]\r\n",
        record->m_strCode,
        upgInfo,
        pLoadData->dbAvator.m_lnUID[tableCode]);
      strcat_0(sData, sBuf);
    }
  }

  std::sprintf(sBuf, "EMBELL\r\n");
  strcat_0(sData, sBuf);
  for (int tableCode = 0; tableCode < 7; ++tableCode)
  {
    _EQUIP_DB_BASE::_EMBELLISH_LIST *embellish = &pLoadData->dbEquip.m_EmbellishList[tableCode];
    if (embellish->Key.IsFilled())
    {
      _base_fld *record =
        g_Main.m_tblItemData[embellish->Key.byTableCode].GetRecord(embellish->Key.wItemIndex);
      std::sprintf(sBuf, "\t%s[%I64u]\r\n", record->m_strCode, embellish->lnUID);
      strcat_0(sData, sBuf);
    }
  }

  std::sprintf(sBuf, "INVEN\r\n");
  strcat_0(sData, sBuf);
  for (int slot = 0; slot < 20 * pLoadData->dbAvator.m_byBagNum; ++slot)
  {
    _INVEN_DB_BASE::_LIST *listEntry = &pLoadData->dbInven.m_List[slot];
    if (listEntry->Key.IsFilled())
    {
      _base_fld *record =
        g_Main.m_tblItemData[listEntry->Key.byTableCode].GetRecord(listEntry->Key.wItemIndex);
      if (record)
      {
        const char *upgInfo = DisplayItemUpgInfo(listEntry->Key.byTableCode, listEntry->dwUpt);
        std::sprintf(
          sBuf,
          "\t%s_%u_@%s[%I64u]\r\n",
          record->m_strCode,
          listEntry->dwDur,
          upgInfo,
          listEntry->lnUID);
        strcat_0(sData, sBuf);
      }
    }
  }

  std::sprintf(sBuf, "FORCE\r\n");
  strcat_0(sData, sBuf);
  for (int index = 0; index < 88; ++index)
  {
    _FORCE_DB_BASE::_LIST *forceEntry = &pLoadData->dbForce.m_List[index];
    if (forceEntry->Key.IsFilled())
    {
      const unsigned __int8 forceIndex = forceEntry->Key.GetIndex();
      _base_fld *record = g_Main.m_tblItemData[15].GetRecord(forceIndex);
      const unsigned int stat = forceEntry->Key.GetStat();
      std::sprintf(sBuf, "\t%s_%u[%I64u]\r\n", record->m_strCode, stat, forceEntry->lnUID);
      strcat_0(sData, sBuf);
    }
  }

  std::sprintf(sBuf, "RES\r\n");
  strcat_0(sData, sBuf);
  for (int index = 0; index < 20; ++index)
  {
    _CUTTING_DB_BASE::_LIST *listEntry = &pLoadData->dbCutting.m_List[index];
    if (listEntry->Key.IsFilled())
    {
      _base_fld *record = g_Main.m_tblItemData[18].GetRecord(listEntry->Key.wItemIndex);
      if (record)
      {
        std::sprintf(sBuf, "\t%s_%u\r\n", record->m_strCode, listEntry->dwDur);
        strcat_0(sData, sBuf);
      }
      else
      {
        g_Main.m_logSystemError.Write(
          "CMgrAvatorItemHistory::have_item_close() : _CUTTING_DB_BASE::_LIST* pList->Key.wItemIndex(%u) i(%d) Serial(%u)",
          listEntry->Key.wItemIndex,
          index,
          dwIDSerial);
      }
    }
  }

  const unsigned __int8 raceCode = static_cast<unsigned __int8>(pLoadData->dbAvator.m_byRaceSexCode >> 1);
  if (raceCode)
  {
    if (raceCode == 1)
    {
      std::sprintf(sBuf, "ANIMUS\r\n");
      strcat_0(sData, sBuf);
      for (int index = 0; index < 4; ++index)
      {
        _ANIMUS_DB_BASE::_LIST *listEntry = &pLoadData->dbAnimus.m_List[index];
        if (listEntry->Key.IsFilled())
        {
          _base_fld *record = g_Main.m_tblItemData[24].GetRecord(listEntry->Key.byItemIndex);
          std::sprintf(
            sBuf,
            "\t%s_%u[%I64u]\r\n",
            record->m_strCode,
            listEntry->dwExp,
            listEntry->lnUID);
          strcat_0(sData, sBuf);
        }
      }
    }
  }
  else
  {
    std::sprintf(sBuf, "UNIT\r\n");
    strcat_0(sData, sBuf);
    for (int index = 0; index < 4; ++index)
    {
      unsigned __int8 *unitData = &pLoadData->dbUnit.m_List[index].bySlotIndex;
      if (pLoadData->dbUnit.m_List[index].byFrame != 255)
      {
        std::sprintf(
          sBuf,
          "\t%d>fr:%d %d/%d/%d/%d/%d/%d\r\n",
          index,
          unitData[1],
          unitData[6],
          unitData[7],
          unitData[8],
          unitData[9],
          unitData[10],
          unitData[11]);
        strcat_0(sData, sBuf);
      }
    }
  }

  std::sprintf(
    sBuf,
    "TRUNK (slot:%d, ^D:%.0f, ^G:%.0f)\r\n",
    pLoadData->dbTrunk.bySlotNum,
    static_cast<double>(pLoadData->dbTrunk.dDalant),
    static_cast<double>(pLoadData->dbTrunk.dGold));
  strcat_0(sData, sBuf);
  for (int index = 0; index < pLoadData->dbTrunk.bySlotNum; ++index)
  {
    _TRUNK_DB_BASE::_LIST *listEntry = &pLoadData->dbTrunk.m_List[index];
    if (listEntry->Key.IsFilled())
    {
      _base_fld *record =
        g_Main.m_tblItemData[listEntry->Key.byTableCode].GetRecord(listEntry->Key.wItemIndex);
      const unsigned __int8 raceIndex = listEntry->byRace;
      const char *upgInfo = DisplayItemUpgInfo(listEntry->Key.byTableCode, listEntry->dwUpt);
      std::sprintf(
        sBuf,
        "\t%s_%u_@%s[%I64u] %s\r\n",
        record->m_strCode,
        listEntry->dwDur,
        upgInfo,
        listEntry->lnUID,
        pRace_0[raceIndex]);
      strcat_0(sData, sBuf);
    }
  }

  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::post_storage(CPostStorage *pStorage, char *pFileName)
{
  char szTran[21]{};
  char sender[17]{};
  char buffer[64]{};

  sData[0] = 0;
  strcat_s(sData, 0x4E20uLL, "\r\n\t============\r\n\r\n");
  const int size = pStorage->GetSize();
  sprintf_s(
    sBuf,
    0x2800uLL,
    "POST STORAGE >> POST NUM = %d\r\n\r\n\t%s%14s%11s%18s%17s%37s%15s\r\n",
    size,
    "[No]",
    "[PostSerial]",
    "[State]",
    "[Sender]",
    "[Title]",
    "[Item]",
    "[Gold]");
  strcat_s(sData, 0x4E20uLL, sBuf);
  if (size > 0)
  {
    for (int index = 0; index < 50; ++index)
    {
      CPostData *postData = pStorage->GetPostDataFromInx(index);
      if (postData && postData->GetState() != 255)
      {
        if (postData->m_Key.IsFilled())
        {
          _base_fld *record =
            g_Main.m_tblItemData[postData->m_Key.byTableCode].GetRecord(postData->m_Key.wItemIndex);
          const char *upgInfo = DisplayItemUpgInfo(postData->m_Key.byTableCode, postData->m_dwUpt);
          sprintf_s(
            buffer,
            0x40uLL,
            "%s_%I64u_@%s[%I64u]",
            record->m_strCode,
            postData->m_dwDur,
            upgInfo,
            postData->m_lnUID);
        }
        else
        {
          sprintf_s(buffer, 0x40uLL, "NoItem");
        }
        W2M(postData->m_wszTitle, szTran, 0x15u);
        W2M(postData->m_wszSendName, sender, 0x11u);
        const char *state = postData->m_byState == 1 ? "Read" : "NotRead";
        sprintf_s(
          sBuf,
          0x2800uLL,
          "\t%3d %13u %10s %17s %22s %34s %10u\r\n",
          postData->m_nNumber,
          postData->m_dwPSSerial,
          state,
          sender,
          szTran,
          buffer,
          postData->m_dwGold);
        strcat_s(sData, 0x4E20uLL, sBuf);
      }
    }
  }
  strcat_s(sData, 0x4E20uLL, "\r\n\t============\r\n\r\n");
  WriteFile(pFileName, sData);
}

void CMgrAvatorItemHistory::return_post_storage(CPostReturnStorage *pReturn, char *pFileName)
{
  char szTran[21]{};
  char receiver[17]{};
  char buffer[64]{};

  sData[0] = 0;
  strcat_s(sData, 0x4E20uLL, "\r\n\t============\r\n\r\n");
  const int size = pReturn->GetSize();
  sprintf_s(
    sBuf,
    0x2800uLL,
    "RETURN POST STORAGE >> RETURN POST NUM = %d\r\n\r\n\t%s%14s%11s%18s%17s%37s%15s\r\n",
    size,
    "[No]",
    "[PostSerial]",
    "[Receiver]",
    "[State]",
    "[Title]",
    "[Item]",
    "[Gold]");
  strcat_s(sData, 0x4E20uLL, sBuf);
  if (size > 0)
  {
    for (int index = 0; index < 10; ++index)
    {
      CPostData *postData = pReturn->GetPostDataFromInx(index);
      if (postData && postData->GetState() != 255)
      {
        if (postData->m_Key.IsFilled())
        {
          _base_fld *record =
            g_Main.m_tblItemData[postData->m_Key.byTableCode].GetRecord(postData->m_Key.wItemIndex);
          const char *upgInfo = DisplayItemUpgInfo(postData->m_Key.byTableCode, postData->m_dwUpt);
          sprintf_s(
            buffer,
            0x40uLL,
            "%s_%I64u_@%s[%I64u]",
            record->m_strCode,
            postData->m_dwDur,
            upgInfo,
            postData->m_lnUID);
        }
        else
        {
          sprintf_s(buffer, 0x40uLL, "NoItem");
        }
        W2M(postData->m_wszTitle, szTran, 0x15u);
        W2M(postData->m_wszRecvName, receiver, 0x11u);
        const char *state = postData->m_byState == 1 ? "Read" : "NotRead";
        sprintf_s(
          sBuf,
          0x2800uLL,
          "\t%3d %13u %10s %17s %22s %34s %10u\r\n",
          postData->m_nNumber,
          postData->m_dwPSSerial,
          state,
          receiver,
          szTran,
          buffer,
          postData->m_dwGold);
        strcat_s(sData, 0x4E20uLL, sBuf);
      }
    }
  }
  strcat_s(sData, 0x4E20uLL, "\r\n\t============\r\n\r\n");
  WriteFile(pFileName, sData);
}

void CMgrAvatorItemHistory::close(int n, char *pCloseCode, char *pszFileName)
{
  (void)n;

  std::sprintf(sData, "\r\nCLOSE %s [%s %s]\r\n\r\n", pCloseCode, this->m_szCurDate, this->m_szCurTime);
  WriteFile(pszFileName, sData);
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

  m_listLogData_10K.SetList(0xFE);
  m_listLogDataEmpty_10K.SetList(0xFE);
  for (unsigned int index = 0; index < 254; ++index)
    m_listLogDataEmpty_10K.PushNode_Back(index);

  m_listLogData_1K.SetList(0xFE);
  m_listLogDataEmpty_1K.SetList(0xFE);
  for (unsigned int index = 0; index < 254; ++index)
    m_listLogDataEmpty_1K.PushNode_Back(index);

  m_listLogData_200.SetList(0x9E4);
  m_listLogDataEmpty_200.SetList(0x9E4);
  for (unsigned int index = 0; index < 2532; ++index)
    m_listLogDataEmpty_200.PushNode_Back(index);

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
    while (self->m_listLogData_10K.PopNode_Front(&outIndex))
    {
      __LOG_DATA_10K &entry = self->m_LogData_10K[outIndex];
      IOFileWrite_0(entry.szFileName, entry.nLen, entry.sData);
      self->m_listLogDataEmpty_10K.PushNode_Back(outIndex);
      Sleep(0);
    }

    while (self->m_listLogData_1K.PopNode_Front(&outIndex))
    {
      __LOG_DATA_1K &entry = self->m_LogData_1K[outIndex];
      IOFileWrite_0(entry.szFileName, entry.nLen, entry.sData);
      self->m_listLogDataEmpty_1K.PushNode_Back(outIndex);
      Sleep(0);
    }

    while (self->m_listLogData_200.PopNode_Front(&outIndex))
    {
      __LOG_DATA_200 &entry = self->m_LogData_200[outIndex];
      IOFileWrite_0(entry.szFileName, entry.nLen, entry.sData);
      self->m_listLogDataEmpty_200.PushNode_Back(outIndex);
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
      if (logLen < 0x2710 && m_listLogDataEmpty_10K.PopNode_Front(&outIndex))
      {
        __LOG_DATA_10K &entry = m_LogData_10K[outIndex];
        strcpy_0(entry.szFileName, pszFileName);
        entry.nLen = logLen;
        memcpy_0(entry.sData, pszLog, static_cast<unsigned int>(logLen));
        entry.sData[logLen] = '\0';
        m_listLogData_10K.PushNode_Back(outIndex);
        return;
      }
    }
    else if (m_listLogDataEmpty_1K.PopNode_Front(&outIndex))
    {
      __LOG_DATA_1K &entry = m_LogData_1K[outIndex];
      strcpy_0(entry.szFileName, pszFileName);
      entry.nLen = logLen;
      memcpy_0(entry.sData, pszLog, static_cast<unsigned int>(logLen));
      entry.sData[logLen] = '\0';
      m_listLogData_1K.PushNode_Back(outIndex);
      return;
    }
  }
  else if (m_listLogDataEmpty_200.PopNode_Front(&outIndex))
  {
    __LOG_DATA_200 &entry = m_LogData_200[outIndex];
    strcpy_0(entry.szFileName, pszFileName);
    entry.nLen = logLen;
    memcpy_0(entry.sData, pszLog, static_cast<unsigned int>(logLen));
    entry.sData[logLen] = '\0';
    m_listLogData_200.PushNode_Back(outIndex);
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

void CMgrAvatorItemHistory::reward_add_item(
  int n,
  char *pszClause,
  _STORAGE_LIST::_db_con *pItem,
  char *pszFileName)
{

  _base_fld *record = g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
  char *curTime = m_szCurTime;
  char *curDate = m_szCurDate;
  const int tableCode = pItem->m_byTableCode;
  const char *upgradeInfo = DisplayItemUpgInfo(tableCode, pItem->m_dwLv);

  sprintf(
    sData,
    "REWARD: %s /ITEM %s_%u_@%s[%I64u] [%s %s]\r\n",
    pszClause,
    record->m_strCode,
    pItem->m_dwDur,
    upgradeInfo,
    pItem->m_lnUID,
    curDate,
    curTime);

  (void)n;
  WriteFile(pszFileName, sData);
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
