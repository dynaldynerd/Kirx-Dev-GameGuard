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
#include "CouponItem_fld.h"
#include "buy_offer.h"
#include "sell_offer.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

namespace
{
  char sData[20000]{};
  char sBuf[10240]{};
  char s_personal_amine_log[20000]{};
  static constexpr size_t kHistoryFileNameSize = 64;
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

void CMgrAvatorItemHistory::OnLoop()
{
  if (m_tmrUpdateTime.CountingTimer())
  {
    _strdate(m_szCurDate);
    m_szCurDate[5] = 0;
    _strtime(m_szCurTime);
    m_szCurTime[5] = 0;
  }
}

int CMgrAvatorItemHistory::GetTotalWaitSize()
{
  // narrowing cast for thunk return parity
  return static_cast<int>(m_listLogData_10K.size() + m_listLogData_1K.size() + m_listLogData_200.size());
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
    const unsigned int korTime = static_cast<unsigned int>(GetKorLocalTime());
    const char *itemKorName = GetItemKorName(byTblCode, wItemIndex);
    sprintf_s(
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
    const unsigned int korTime = static_cast<unsigned int>(GetKorLocalTime());
    sprintf_s(
      sData,
      sizeof(sData),
      "[DEL_LENDITEM] : Tbl:%d Idx:%d [UID:%I64u][%u]\r\n",
      byTblCode,
      wItemIndex,
      lnUID,
      korTime);
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
      "Item Effect Add :%s_[%I64u] [Count :%I64u] [%s %s]\r\n",
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
      "Item Effect Del :%s_[%I64u] [Count :%I64u] [%s %s]\r\n",
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
  std::memset(sData, 0, sizeof(sData));
  sprintf_s(sData, sizeof(sData), "[Exp_Prof_Using] :  %d \r\n", count);
  WriteFile(szFile, sData);
}

void CMgrAvatorItemHistory::InitClass(
  int iCostGold,
  unsigned int dwInitClassCnt,
  unsigned __int8 byLastClassGrade,
  char *szOldClass,
  char *szCurClass,
  int *piOldMaxPoint,
  int *piAlterMaxPoint,
  char *pszFileName)
{
  sprintf_s(
    sData,
    "INIT CLASS: G:%d Cnt:%d Gd:%d %s -> %s H:%d F:%d S:%d -> H:%d F:%d S:%d\r\n",
    iCostGold,
    dwInitClassCnt,
    byLastClassGrade,
    szOldClass,
    szCurClass,
    *piOldMaxPoint,
    piOldMaxPoint[1],
    piOldMaxPoint[2],
    *piAlterMaxPoint,
    piAlterMaxPoint[1],
    piAlterMaxPoint[2]);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::ClassUP(
  unsigned __int8 byCurClassGrade,
  unsigned __int8 byLastClassGrade,
  char *szOldClass,
  char *szCurClass,
  int *piOldMaxPoint,
  int *piAlterMaxPoint,
  char *pszFileName)
{
  sprintf_s(
    sData,
    "CLASS UP: CGd:%d LGd:%d %s -> %s H:%d F:%d S:%d -> H:%d F:%d S:%d\r\n",
    byCurClassGrade,
    byLastClassGrade,
    szOldClass,
    szCurClass,
    *piOldMaxPoint,
    piOldMaxPoint[1],
    piOldMaxPoint[2],
    *piAlterMaxPoint,
    piAlterMaxPoint[1],
    piAlterMaxPoint[2]);
  WriteFile(pszFileName, sData);
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
        std::strcpy(category, "mastery_weapon_long");
      }
      else
      {
        std::strcpy(category, "mastery_weapon_close");
      }
      break;
    case 1:
      std::strcpy(category, "mastery_code_defence");
      break;
    case 2:
      std::strcpy(category, "mastery_code_shield");
      break;
    case 3:
      std::strcpy(category, "mastery_code_shield");
      break;
    case 4:
      std::strcpy(category, "mastery_code_force");
      break;
    case 6:
      std::strcpy(category, "mastery_code_special");
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
          std::strcat(sData, "\r\n== UnmannedTrader Regist Info ==\r\n");
        }
        ++hasData;

        const int itemIndex = info->GetItemIndex();
        const unsigned __int8 tableCode = info->GetTableCode();
        _base_fld *record = g_Main.m_tblItemData[tableCode].GetRecord(itemIndex);
        const __int64 *startTimePtr = info->GetStartTimePtr();
        localTime = localtime_5(startTimePtr);
        if (localTime)
        {
          sprintf_s(
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
          const int startTime = static_cast<int>(info->GetStartTime());
          sprintf_s(buffer, sizeof(buffer), "invalid(%u)", startTime);
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

        sprintf_s(
          sBuf,
          sizeof(sBuf),
          "%u : reg(%u) storageinx(%u) %s_%I64u_@%s[%u] price(%u) regtime(%s) sellturm(%u) \r\n",
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
        std::strcat(sData, sBuf);
      }
    }

    if (hasData)
    {
      std::strcat(sData, "================================\r\n\r\n");
    }
  }
  else
  {
    sprintf_s(
      sBuf,
      10240,
      "\r\n== UnmannedTrader Regist Info pkInfo(%p) byMacCnt(%u) ==\r\n",
      pkInfo,
      byMaxCnt);
    strcat_s(sData, 20000, sBuf);
  }
}

void CMgrAvatorItemHistory::GetNewFileName(unsigned int dwAvatorSerial, char *pszFileName)
{
  const unsigned int localDate = GetLocalDate();
  char buffer[132]{};
  sprintf_s(buffer, sizeof(buffer), "%s\\%d", this->m_szStdPath, localDate);
  CreateDirectoryA(buffer, nullptr);
  this->m_dwLastLocalDate = localDate;

  const unsigned int currentHour = GetCurrentHour();
  char pathName[148]{};
  sprintf_s(pathName, sizeof(pathName), "%s\\%d\\%d", this->m_szStdPath, this->m_dwLastLocalDate, currentHour);
  CreateDirectoryA(pathName, nullptr);
  this->m_dwLastLocalHour = currentHour;

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
    "%s\\%d\\%d\\%d_%s.his",
    this->m_szStdPath,
    this->m_dwLastLocalDate,
    this->m_dwLastLocalHour,
    dwAvatorSerial,
    timeStr);
}

void CMgrAvatorItemHistory::have_item(
  int n,
  char *pszName,
  _AVATOR_DATA *pLoadData,
  _AVATOR_DATA *pBackupData,
  char *pszID,
  unsigned int dwIDSerial,
  unsigned __int8 byDgr,
  unsigned int dwIP,
  unsigned int dwExpRate,
  bool bStart,
  char *pszFileName)
{
  sData[0] = 0;

  _base_fld *class0 = g_Main.m_tblClass.GetRecord(pLoadData->dbAvator.m_zClassHistory[0]);
  _base_fld *class1 = g_Main.m_tblClass.GetRecord(pLoadData->dbAvator.m_zClassHistory[1]);
  _base_fld *class2 = g_Main.m_tblClass.GetRecord(pLoadData->dbAvator.m_zClassHistory[2]);

  if (bStart)
  {
    const char *classCode2 = class2 ? class2->m_strCode : "-1";
    const char *classCode1 = class1 ? class1->m_strCode : "-1";
    const char *classCode0 = class0 ? class0->m_strCode : "-1";

    in_addr ipAddr{};
    ipAddr.S_un.S_addr = dwIP;
    const char *ipStr = inet_ntoa(ipAddr);

    const unsigned __int8 bagNum = pLoadData->dbAvator.m_byBagNum;
    const unsigned int dalantDiff = pLoadData->dbAvator.m_dwDalant - pBackupData->dbAvator.m_dwDalant;
    const unsigned __int8 lastClassGrade = pLoadData->dbAvator.m_byLastClassGrade;
    const unsigned __int8 sexCode = pLoadData->dbAvator.m_byRaceSexCode & 1;
    const unsigned __int8 raceSexCode = pLoadData->dbAvator.m_byRaceSexCode;

    sprintf_s(
      sBuf,
      "NAME: %s [%s %s]\r\n"
      "WORLD: %s\r\n"
      "SR: %d\r\n"
      "ID: %s (%d)\r\n"
      "DGR: %d\r\n"
      "LV: %d\r\n"
      "XP: %.0f (%d)\r\n"
      "RACE: %d\r\n"
      "SEX: %d\r\n"
      "CurClass:%s\r\n"
      "OldClass 0:%s, 1:%s, 2:%s\r\n"
      "LastClassGrade: %d\r\n"
      "InitClassCnt: %d\r\n"
      "$D: %u  ( %u push )\r\n"
      "$G: %u\r\n"
      "PvP: %.0f\r\n"
      "CB: %.0f\r\n"
      "BAG: %d\r\n"
      "IP: %s\r\n"
      "TIME: %d\r\n"
      "\r\n",
      pszName,
      this->m_szCurDate,
      this->m_szCurTime,
      g_Main.m_szWorldName,
      pLoadData->dbAvator.m_dwRecordNum,
      pszID,
      dwIDSerial,
      byDgr,
      pLoadData->dbAvator.m_byLevel,
      static_cast<double>(pLoadData->dbAvator.m_dExp),
      dwExpRate,
      raceSexCode / 2,
      sexCode,
      pLoadData->dbAvator.m_szClassCode,
      classCode0,
      classCode1,
      classCode2,
      lastClassGrade,
      pLoadData->dbAvator.m_dwClassInitCnt,
      pLoadData->dbAvator.m_dwDalant,
      dalantDiff,
      pLoadData->dbAvator.m_dwGold,
      static_cast<double>(pLoadData->dbAvator.m_dPvPPoint),
      static_cast<double>(pLoadData->dbAvator.m_dPvPCashBag),
      bagNum,
      ipStr,
      pLoadData->dbAvator.m_dwTotalPlayMin);
    std::strcat(sData, sBuf);
  }
  else
  {
    std::strcat(sData, "\r\n\t============\r\n\r\n");
    sprintf_s(
      sBuf,
      "LV: %d\r\n"
      "XP: %.0f (%d)\r\n"
      "$D: %u\r\n"
      "$G: %u\r\n"
      "PvP: %.0f\r\n"
      "CB: %.0f\r\n"
      "TIME: %d\r\n"
      "\r\n",
      pLoadData->dbAvator.m_byLevel,
      static_cast<double>(pLoadData->dbAvator.m_dExp),
      dwExpRate,
      pLoadData->dbAvator.m_dwDalant,
      pLoadData->dbAvator.m_dwGold,
      static_cast<double>(pLoadData->dbAvator.m_dPvPPoint),
      static_cast<double>(pLoadData->dbAvator.m_dPvPCashBag),
      pLoadData->dbAvator.m_dwTotalPlayMin);
    std::strcat(sData, sBuf);
  }

  sprintf_s(sBuf, sizeof(sBuf), "EQUIP\r\n");
  std::strcat(sData, sBuf);
  for (int tableCode = 0; tableCode < 8; ++tableCode)
  {
    _EQUIPKEY *equipKey = &pLoadData->dbAvator.m_EquipKey[tableCode];
    if (equipKey->IsFilled())
    {
      _base_fld *record = g_Main.m_tblItemData[tableCode].GetRecord(equipKey->zItemIndex);
      const char *upgInfo = DisplayItemUpgInfo(tableCode, pLoadData->dbAvator.m_dwFixEquipLv[tableCode]);
      sprintf_s(
        sBuf,
        "\t%s_@%s[%I64u]\r\n",
        record->m_strCode,
        upgInfo,
        pLoadData->dbAvator.m_lnUID[tableCode]);
      std::strcat(sData, sBuf);
    }
  }

  sprintf_s(sBuf, sizeof(sBuf), "EMBELL\r\n");
  std::strcat(sData, sBuf);
  for (int tableCode = 0; tableCode < 7; ++tableCode)
  {
    _EQUIP_DB_BASE::_EMBELLISH_LIST *embellish = &pLoadData->dbEquip.m_EmbellishList[tableCode];
    if (embellish->Key.IsFilled())
    {
      _base_fld *record =
        g_Main.m_tblItemData[embellish->Key.byTableCode].GetRecord(embellish->Key.wItemIndex);
      sprintf_s(sBuf, sizeof(sBuf), "\t%s[%I64u]\r\n", record->m_strCode, embellish->lnUID);
      std::strcat(sData, sBuf);
    }
  }

  sprintf_s(sBuf, sizeof(sBuf), "INVEN\r\n");
  std::strcat(sData, sBuf);
  for (int slot = 0; slot < 20 * pLoadData->dbAvator.m_byBagNum; ++slot)
  {
    _INVEN_DB_BASE::_LIST *listEntry = &pLoadData->dbInven.m_List[slot];
    if (listEntry->Key.IsFilled())
    {
      _base_fld *record =
        g_Main.m_tblItemData[listEntry->Key.byTableCode].GetRecord(listEntry->Key.wItemIndex);
      const char *upgInfo = DisplayItemUpgInfo(listEntry->Key.byTableCode, listEntry->dwUpt);
      if (bStart)
      {
        if (pBackupData->dbInven.m_List[slot].Key.IsFilled())
        {
          sprintf_s(
            sBuf,
            "\t%s_%u_@%s[%I64u]\r\n",
            record->m_strCode,
            static_cast<unsigned int>(listEntry->dwDur),
            upgInfo,
            listEntry->lnUID);
        }
        else
        {
          sprintf_s(
            sBuf,
            "\t%s_%u_@%s[%I64u] \t#push\r\n",
            record->m_strCode,
            static_cast<unsigned int>(listEntry->dwDur),
            upgInfo,
            listEntry->lnUID);
        }
      }
      else
      {
        sprintf_s(
          sBuf,
          "\t%s_%u_@%s[%I64u]\r\n",
          record->m_strCode,
          static_cast<unsigned int>(listEntry->dwDur),
          upgInfo,
          listEntry->lnUID);
      }
      std::strcat(sData, sBuf);
    }
  }

  sprintf_s(sBuf, sizeof(sBuf), "FORCE\r\n");
  std::strcat(sData, sBuf);
  for (int slot = 0; slot < 88; ++slot)
  {
    _FORCE_DB_BASE::_LIST *forceEntry = &pLoadData->dbForce.m_List[slot];
    if (forceEntry->Key.IsFilled())
    {
      const unsigned __int8 index = forceEntry->Key.GetIndex();
      _base_fld *record = g_Main.m_tblItemData[15].GetRecord(index);
      const unsigned int stat = forceEntry->Key.GetStat();
      sprintf_s(sBuf, sizeof(sBuf), "\t%s_%u[%I64u]\r\n", record->m_strCode, stat, forceEntry->lnUID);
      std::strcat(sData, sBuf);
    }
  }

  sprintf_s(sBuf, sizeof(sBuf), "RES\r\n");
  std::strcat(sData, sBuf);
  for (int slot = 0; slot < 20; ++slot)
  {
    _CUTTING_DB_BASE::_LIST *listEntry = &pLoadData->dbCutting.m_List[slot];
    if (listEntry->Key.IsFilled())
    {
      _base_fld *record = g_Main.m_tblItemData[18].GetRecord(listEntry->Key.wItemIndex);
      if (record)
      {
        sprintf_s(sBuf, sizeof(sBuf), "\t%s_%u\r\n", record->m_strCode, listEntry->dwDur);
        std::strcat(sData, sBuf);
      }
      else
      {
        g_Main.m_logSystemError.Write(
          "CMgrAvatorItemHistory::have_item() : _CUTTING_DB_BASE::_LIST* pList->Key.wItemIndex(%u) i(%d) Serial(%u)",
          listEntry->Key.wItemIndex,
          slot,
          dwIDSerial);
      }
    }
  }

  const unsigned __int8 raceCode = static_cast<unsigned __int8>(pLoadData->dbAvator.m_byRaceSexCode >> 1);
  if (raceCode)
  {
    if (raceCode == 1)
    {
      sprintf_s(sBuf, sizeof(sBuf), "ANIMUS\r\n");
      std::strcat(sData, sBuf);
      for (int slot = 0; slot < 4; ++slot)
      {
        _ANIMUS_DB_BASE::_LIST *listEntry = &pLoadData->dbAnimus.m_List[slot];
        if (listEntry->Key.IsFilled())
        {
          _base_fld *record = g_Main.m_tblItemData[24].GetRecord(listEntry->Key.byItemIndex);
          sprintf_s(
            sBuf,
            "\t%s_%I64u[%I64u]\r\n",
            record->m_strCode,
            listEntry->dwExp,
            listEntry->lnUID);
          std::strcat(sData, sBuf);
        }
      }
    }
  }
  else
  {
    sprintf_s(sBuf, sizeof(sBuf), "UNIT\r\n");
    std::strcat(sData, sBuf);
    for (int slot = 0; slot < 4; ++slot)
    {
      _UNIT_DB_BASE::_LIST *unit = &pLoadData->dbUnit.m_List[slot];
      if (unit->byFrame != 255)
      {
        sprintf_s(
          sBuf,
          "\t%d>fr:%d %d/%d/%d/%d/%d/%d\r\n",
          slot,
          unit->byFrame,
          unit->byPart[0],
          unit->byPart[1],
          unit->byPart[2],
          unit->byPart[3],
          unit->byPart[4],
          unit->byPart[5]);
        std::strcat(sData, sBuf);
      }
    }
  }

  if (bStart)
  {
    const long double dalantDiff = pLoadData->dbTrunk.dDalant - pBackupData->dbTrunk.dDalant;
    const long double goldDiff = pLoadData->dbTrunk.dGold - pBackupData->dbTrunk.dGold;
    sprintf_s(
      sBuf,
      "TRUNK (slot:%d, ^D:%.0f, ^G:%.0f) ( ^D:%.0f, ^G:%.0f push )\r\n",
      pLoadData->dbTrunk.bySlotNum,
      static_cast<double>(pLoadData->dbTrunk.dDalant),
      static_cast<double>(pLoadData->dbTrunk.dGold),
      static_cast<double>(dalantDiff),
      static_cast<double>(goldDiff));
  }
  else
  {
    sprintf_s(
      sBuf,
      "TRUNK (slot:%d, ^D:%.0f, ^G:%.0f)\r\n",
      pLoadData->dbTrunk.bySlotNum,
      static_cast<double>(pLoadData->dbTrunk.dDalant),
      static_cast<double>(pLoadData->dbTrunk.dGold));
  }
  std::strcat(sData, sBuf);

  for (int slot = 0; slot < pLoadData->dbTrunk.bySlotNum; ++slot)
  {
    _TRUNK_DB_BASE::_LIST *listEntry = &pLoadData->dbTrunk.m_List[slot];
    if (listEntry->Key.IsFilled())
    {
      _base_fld *record =
        g_Main.m_tblItemData[listEntry->Key.byTableCode].GetRecord(listEntry->Key.wItemIndex);
      const char *upgInfo = DisplayItemUpgInfo(listEntry->Key.byTableCode, listEntry->dwUpt);
      const unsigned __int8 raceIndex = listEntry->byRace;
      if (bStart)
      {
        if (pBackupData->dbTrunk.m_List[slot].Key.IsFilled())
        {
          sprintf_s(
            sBuf,
            "\t%s_%u_@%s[%I64u] %s\r\n",
            record->m_strCode,
            static_cast<unsigned int>(listEntry->dwDur),
            upgInfo,
            listEntry->lnUID,
            pRace_0[raceIndex]);
        }
        else
        {
          sprintf_s(
            sBuf,
            "\t%s_%u_@%s[%I64u] %s\t#push\r\n",
            record->m_strCode,
            static_cast<unsigned int>(listEntry->dwDur),
            upgInfo,
            listEntry->lnUID,
            pRace_0[raceIndex]);
        }
      }
      else
      {
        sprintf_s(
          sBuf,
          "\t%s_%u_@%s[%I64u] %s\r\n",
          record->m_strCode,
          static_cast<unsigned int>(listEntry->dwDur),
          upgInfo,
          listEntry->lnUID,
          pRace_0[raceIndex]);
      }
      std::strcat(sData, sBuf);
    }
  }

  if (bStart)
  {
    std::strcat(sData, "\r\n\t============\r\n\r\n");
  }

  WriteFile(pszFileName, sData);
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
sData[0] = 0;
  g_Main.m_tblClass.GetRecord(pLoadData->dbAvator.m_zClassHistory[0]);
  g_Main.m_tblClass.GetRecord(pLoadData->dbAvator.m_zClassHistory[1]);
  g_Main.m_tblClass.GetRecord(pLoadData->dbAvator.m_zClassHistory[2]);
  std::strcat(sData, "\r\n\t============\r\n\r\n");
  have_auto_item(n, pkInfo, byMaxCnt);

  sprintf_s(
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
  std::strcat(sData, sBuf);

  sprintf_s(sBuf, sizeof(sBuf), "EQUIP\r\n");
  std::strcat(sData, sBuf);
  for (int tableCode = 0; tableCode < 8; ++tableCode)
  {
    _EQUIPKEY *equipKey = &pLoadData->dbAvator.m_EquipKey[tableCode];
    if (equipKey->IsFilled())
    {
      _base_fld *record = g_Main.m_tblItemData[tableCode].GetRecord(equipKey->zItemIndex);
      const char *upgInfo = DisplayItemUpgInfo(tableCode, pLoadData->dbAvator.m_dwFixEquipLv[tableCode]);
      sprintf_s(
        sBuf,
        "\t%s_@%s[%I64u]\r\n",
        record->m_strCode,
        upgInfo,
        pLoadData->dbAvator.m_lnUID[tableCode]);
      std::strcat(sData, sBuf);
    }
  }

  sprintf_s(sBuf, sizeof(sBuf), "EMBELL\r\n");
  std::strcat(sData, sBuf);
  for (int tableCode = 0; tableCode < 7; ++tableCode)
  {
    _EQUIP_DB_BASE::_EMBELLISH_LIST *embellish = &pLoadData->dbEquip.m_EmbellishList[tableCode];
    if (embellish->Key.IsFilled())
    {
      _base_fld *record =
        g_Main.m_tblItemData[embellish->Key.byTableCode].GetRecord(embellish->Key.wItemIndex);
      sprintf_s(sBuf, sizeof(sBuf), "\t%s[%I64u]\r\n", record->m_strCode, embellish->lnUID);
      std::strcat(sData, sBuf);
    }
  }

  sprintf_s(sBuf, sizeof(sBuf), "INVEN\r\n");
  std::strcat(sData, sBuf);
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
        sprintf_s(
          sBuf,
          sizeof(sBuf),
          "\t%s_%I64u_@%s[%I64u]\r\n",
          record->m_strCode,
          listEntry->dwDur,
          upgInfo,
          listEntry->lnUID);
        std::strcat(sData, sBuf);
      }
    }
  }

  sprintf_s(sBuf, sizeof(sBuf), "FORCE\r\n");
  std::strcat(sData, sBuf);
  for (int index = 0; index < 88; ++index)
  {
    _FORCE_DB_BASE::_LIST *forceEntry = &pLoadData->dbForce.m_List[index];
    if (forceEntry->Key.IsFilled())
    {
      const unsigned __int8 forceIndex = forceEntry->Key.GetIndex();
      _base_fld *record = g_Main.m_tblItemData[15].GetRecord(forceIndex);
      const unsigned int stat = forceEntry->Key.GetStat();
      sprintf_s(sBuf, sizeof(sBuf), "\t%s_%u[%I64u]\r\n", record->m_strCode, stat, forceEntry->lnUID);
      std::strcat(sData, sBuf);
    }
  }

  sprintf_s(sBuf, sizeof(sBuf), "RES\r\n");
  std::strcat(sData, sBuf);
  for (int index = 0; index < 20; ++index)
  {
    _CUTTING_DB_BASE::_LIST *listEntry = &pLoadData->dbCutting.m_List[index];
    if (listEntry->Key.IsFilled())
    {
      _base_fld *record = g_Main.m_tblItemData[18].GetRecord(listEntry->Key.wItemIndex);
      if (record)
      {
        sprintf_s(sBuf, sizeof(sBuf), "\t%s_%u\r\n", record->m_strCode, listEntry->dwDur);
        std::strcat(sData, sBuf);
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
      sprintf_s(sBuf, sizeof(sBuf), "ANIMUS\r\n");
      std::strcat(sData, sBuf);
      for (int index = 0; index < 4; ++index)
      {
        _ANIMUS_DB_BASE::_LIST *listEntry = &pLoadData->dbAnimus.m_List[index];
        if (listEntry->Key.IsFilled())
        {
          _base_fld *record = g_Main.m_tblItemData[24].GetRecord(listEntry->Key.byItemIndex);
          sprintf_s(
            sBuf,
            sizeof(sBuf),
            "\t%s_%I64u[%I64u]\r\n",
            record->m_strCode,
            listEntry->dwExp,
            listEntry->lnUID);
          std::strcat(sData, sBuf);
        }
      }
    }
  }
  else
  {
    sprintf_s(sBuf, sizeof(sBuf), "UNIT\r\n");
    std::strcat(sData, sBuf);
    for (int index = 0; index < 4; ++index)
    {
      unsigned __int8 *unitData = &pLoadData->dbUnit.m_List[index].bySlotIndex;
      if (pLoadData->dbUnit.m_List[index].byFrame != 255)
      {
        sprintf_s(
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
        std::strcat(sData, sBuf);
      }
    }
  }

  sprintf_s(
    sBuf,
    "TRUNK (slot:%d, ^D:%.0f, ^G:%.0f)\r\n",
    pLoadData->dbTrunk.bySlotNum,
    static_cast<double>(pLoadData->dbTrunk.dDalant),
    static_cast<double>(pLoadData->dbTrunk.dGold));
  std::strcat(sData, sBuf);
  for (int index = 0; index < pLoadData->dbTrunk.bySlotNum; ++index)
  {
    _TRUNK_DB_BASE::_LIST *listEntry = &pLoadData->dbTrunk.m_List[index];
    if (listEntry->Key.IsFilled())
    {
      _base_fld *record =
        g_Main.m_tblItemData[listEntry->Key.byTableCode].GetRecord(listEntry->Key.wItemIndex);
      const unsigned __int8 raceIndex = listEntry->byRace;
      const char *upgInfo = DisplayItemUpgInfo(listEntry->Key.byTableCode, listEntry->dwUpt);
      sprintf_s(
        sBuf,
        sizeof(sBuf),
        "\t%s_%I64u_@%s[%I64u] %s\r\n",
        record->m_strCode,
        listEntry->dwDur,
        upgInfo,
        listEntry->lnUID,
        pRace_0[raceIndex]);
      std::strcat(sData, sBuf);
    }
  }

  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::take_ground_item(
  int /*n*/,
  unsigned __int8 byItemBoxCode,
  _STORAGE_LIST::_db_con *pItem,
  const char *pszThrowerName,
  unsigned int dwThrowerSerial,
  const char *pszThrowerID,
  unsigned __int16 wMonRecIndex,
  const char *pMapCode,
  float *pfPos,
  const char *pszFileName)
{
  _base_fld *record = g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);

  if (dwThrowerSerial == static_cast<unsigned int>(-1))
  {
    char destination[48]{};
    if (byItemBoxCode || wMonRecIndex == 65535)
    {
      switch (byItemBoxCode)
      {
        case 2:
          sprintf_s(destination, sizeof(destination), "cheat");
          break;
        case 3:
          sprintf_s(destination, sizeof(destination), "reward");
          break;
        case 4:
          sprintf_s(destination, sizeof(destination), "crystal");
          break;
        case 6:
          sprintf_s(destination, sizeof(destination), "holykeeper");
          break;
        default:
          sprintf_s(destination, sizeof(destination), "loot");
          break;
      }
    }
    else
    {
      _base_fld *monRecord = g_Main.m_tblMonster.GetRecord(wMonRecIndex);
      std::strcpy(destination, monRecord->m_strCode);
    }

    const int posX = static_cast<int>(pfPos[0]);
    const int posY = static_cast<int>(pfPos[1]);
    const int posZ = static_cast<int>(pfPos[2]);
    const char *upgradeInfo = DisplayItemUpgInfo(pItem->m_byTableCode, pItem->m_dwLv);

    sprintf_s(
      sData,
      sizeof(sData),
      "PICK UP: %s_%I64u_@%s[%I64u] mob(%s) \t{POS:%s (%d, %d, %d)} [%s %s]\r\n",
      record->m_strCode,
      pItem->m_dwDur,
      upgradeInfo,
      pItem->m_lnUID,
      destination,
      pMapCode,
      posX,
      posY,
      posZ,
      m_szCurDate,
      m_szCurTime);
  }
  else
  {
    const int posX = static_cast<int>(pfPos[0]);
    const int posY = static_cast<int>(pfPos[1]);
    const int posZ = static_cast<int>(pfPos[2]);
    const char *upgradeInfo = DisplayItemUpgInfo(pItem->m_byTableCode, pItem->m_dwLv);

    sprintf_s(
      sData,
      sizeof(sData),
      "PICK UP: %s_%I64u_@%s[%I64u] twr(%s:%d id:%s) \t{POS:%s (%d, %d, %d)} [%s %s]\r\n",
      record->m_strCode,
      pItem->m_dwDur,
      upgradeInfo,
      pItem->m_lnUID,
      pszThrowerName,
      dwThrowerSerial,
      pszThrowerID,
      pMapCode,
      posX,
      posY,
      posZ,
      m_szCurDate,
      m_szCurTime);
  }

  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::trans_ground_item(
  _STORAGE_LIST::_db_con *pItem,
  const char *pszTakerName,
  unsigned int dwTakerSerial,
  const char *pszTakerID,
  const char *pszFileName)
{
  _base_fld *record = g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
  const char *upgradeInfo = DisplayItemUpgInfo(pItem->m_byTableCode, pItem->m_dwLv);

  sprintf_s(
    sData,
    sizeof(sData),
    "TRANS: %s_%I64u_@%s[%I64u] taker(%s:%d id:%s) [%s %s]\r\n",
    record->m_strCode,
    pItem->m_dwDur,
    upgradeInfo,
    pItem->m_lnUID,
    pszTakerName,
    dwTakerSerial,
    pszTakerID,
    m_szCurDate,
    m_szCurTime);

  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::post_delete(CPostData *pPost, char *pFileName)
{
  char sender[17]{};
  sData[0] = 0;

  W2M(pPost->m_wszSendName, sender, 17);
  if (pPost->m_Key.IsFilled())
  {
    _base_fld *record = g_Main.m_tblItemData[pPost->m_Key.byTableCode].GetRecord(pPost->m_Key.wItemIndex);
    const char *upgInfo = DisplayItemUpgInfo(pPost->m_Key.byTableCode, pPost->m_dwUpt);
    sprintf_s(sBuf, 10240, "%s_%I64u_@%s[%I64u]", record->m_strCode, pPost->m_dwDur, upgInfo, pPost->m_lnUID);
  }
  else
  {
    sprintf_s(sBuf, 10240, "NoItem");
  }

  sprintf_s(
    sData,
    20000,
    "[PostSystem : Post Delete] - PostSerial[%u] - No[%d] - Item[%s] - Gold[%u] - Sender[%s] - [%s %s]\r\n",
    pPost->m_dwPSSerial,
    pPost->m_nNumber,
    sBuf,
    pPost->m_dwGold,
    sender,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pFileName, sData);
}

void CMgrAvatorItemHistory::post_getpresent(
  char *wszSendName,
  unsigned int dwPostSerial,
  _STORAGE_LIST::_db_con *Item,
  unsigned __int64 dwDur,
  unsigned int dwGold,
  char *pFileName)
{
  char sender[17]{};
  char buffer[64]{};
  sData[0] = 0;

  W2M(wszSendName, sender, 17);
  if (Item)
  {
    _base_fld *record = g_Main.m_tblItemData[Item->m_byTableCode].GetRecord(Item->m_wItemIndex);
    const char *upgInfo = DisplayItemUpgInfo(Item->m_byTableCode, Item->m_dwLv);
    sprintf_s(buffer, 64, "%s_%I64u_@%s[%I64u]", record->m_strCode, dwDur, upgInfo, Item->m_lnUID);
  }
  else
  {
    sprintf_s(buffer, 64, "NoItem");
  }

  sprintf_s(
    sData,
    "[PostSystem : Get Item & Gold In Inven] - PostSerial[%u] - Item[%s] - Gold[%u] - Sender[%s] - [%s %s]\r\n",
    dwPostSerial,
    buffer,
    dwGold,
    sender,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pFileName, sData);
}

void CMgrAvatorItemHistory::post_senditem(
  char *wszRecvName,
  _STORAGE_LIST::_db_con *Item,
  unsigned __int64 dwDur,
  unsigned int dwGold,
  char *pFileName)
{
  char receiver[17]{};
  char buffer[64]{};
  sData[0] = 0;

  W2M(wszRecvName, receiver, 17);
  if (Item)
  {
    _base_fld *record = g_Main.m_tblItemData[Item->m_byTableCode].GetRecord(Item->m_wItemIndex);
    const char *upgInfo = DisplayItemUpgInfo(Item->m_byTableCode, Item->m_dwLv);
    sprintf_s(buffer, 64, "%s_%I64u_@%s[%I64u]", record->m_strCode, dwDur, upgInfo, Item->m_lnUID);
  }
  else
  {
    sprintf_s(buffer, 64, "NoItem");
  }

  sprintf_s(
    sData,
    "[PostSystem : Send Item & Gold] - Item[%s] - Gold[%u] - Receiver[%s] - [%s %s]\r\n",
    buffer,
    dwGold,
    receiver,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pFileName, sData);
}

void CMgrAvatorItemHistory::post_return(
  char *wszRecvName,
  unsigned int dwPostSerial,
  _STORAGE_LIST::_db_con *Item,
  unsigned __int64 dwDur,
  unsigned int dwGold,
  char *pFileName)
{
  char receiver[17]{};
  char buffer[64]{};
  sData[0] = 0;

  W2M(wszRecvName, receiver, 17);
  if (Item)
  {
    _base_fld *record = g_Main.m_tblItemData[Item->m_byTableCode].GetRecord(Item->m_wItemIndex);
    const char *upgInfo = DisplayItemUpgInfo(Item->m_byTableCode, Item->m_dwLv);
    sprintf_s(buffer, 64, "%s_%I64u_@%s[%I64u]", record->m_strCode, dwDur, upgInfo, Item->m_lnUID);
  }
  else
  {
    sprintf_s(buffer, 64, "NoItem");
  }

  sprintf_s(
    sData,
    "[PostSystem : Get Return Item & Gold In Inven] - PostSerial[%u] - Item[%s] - Gold[%u] - Receiver[%s] - [%s %s]\r\n",
    dwPostSerial,
    buffer,
    dwGold,
    receiver,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pFileName, sData);
}

void CMgrAvatorItemHistory::post_receive(CPostData *pPost, char *pFileName)
{
  char sender[17]{};
  sData[0] = 0;

  W2M(pPost->m_wszSendName, sender, 17);
  if (pPost->m_Key.IsFilled())
  {
    _base_fld *record = g_Main.m_tblItemData[pPost->m_Key.byTableCode].GetRecord(pPost->m_Key.wItemIndex);
    const char *upgInfo = DisplayItemUpgInfo(pPost->m_Key.byTableCode, pPost->m_dwUpt);
    sprintf_s(sBuf, 10240, "%s_%I64u_@%s[%I64u]", record->m_strCode, pPost->m_dwDur, upgInfo, pPost->m_lnUID);
  }
  else
  {
    sprintf_s(sBuf, 10240, "NoItem");
  }

  sprintf_s(
    sData,
    20000,
    "[PostSystem : Post Receive] - PostSerial[%u] - Item[%s] - Gold[%u] - Sender[%s] - [%s %s]\r\n",
    pPost->m_dwPSSerial,
    sBuf,
    pPost->m_dwGold,
    sender,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pFileName, sData);
}

void CMgrAvatorItemHistory::post_storage(CPostStorage *pStorage, char *pFileName)
{
  char szTran[21]{};
  char sender[17]{};
  char buffer[64]{};

  sData[0] = 0;
  strcat_s(sData, 20000, "\r\n\t============\r\n\r\n");
  const int size = pStorage->GetSize();
  sprintf_s(
    sBuf,
    10240,
    "POST STORAGE >> POST NUM = %d\r\n\r\n\t%s%14s%11s%18s%17s%37s%15s\r\n",
    size,
    "[No]",
    "[PostSerial]",
    "[State]",
    "[Sender]",
    "[Title]",
    "[Item]",
    "[Gold]");
  strcat_s(sData, 20000, sBuf);
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
            64,
            "%s_%I64u_@%s[%I64u]",
            record->m_strCode,
            postData->m_dwDur,
            upgInfo,
            postData->m_lnUID);
        }
        else
        {
          sprintf_s(buffer, 64, "NoItem");
        }
        W2M(postData->m_wszTitle, szTran, 21);
        W2M(postData->m_wszSendName, sender, 17);
        const char *state = postData->m_byState == 1 ? "Read" : "NotRead";
        sprintf_s(
          sBuf,
          10240,
          "\t%3d %13u %10s %17s %22s %34s %10u\r\n",
          postData->m_nNumber,
          postData->m_dwPSSerial,
          state,
          sender,
          szTran,
          buffer,
          postData->m_dwGold);
        strcat_s(sData, 20000, sBuf);
      }
    }
  }
  strcat_s(sData, 20000, "\r\n\t============\r\n\r\n");
  WriteFile(pFileName, sData);
}

void CMgrAvatorItemHistory::post_returnreceive(CPostData *pPost, char *pFileName)
{
  char receiver[17]{};
  sData[0] = 0;

  W2M(pPost->m_wszRecvName, receiver, 17);
  if (pPost->m_Key.IsFilled())
  {
    _base_fld *record = g_Main.m_tblItemData[pPost->m_Key.byTableCode].GetRecord(pPost->m_Key.wItemIndex);
    const char *upgInfo = DisplayItemUpgInfo(pPost->m_Key.byTableCode, pPost->m_dwUpt);
    sprintf_s(sBuf, 10240, "%s_%I64u_@%s[%I64u]", record->m_strCode, pPost->m_dwDur, upgInfo, pPost->m_lnUID);
  }
  else
  {
    sprintf_s(sBuf, 10240, "NoItem");
  }

  sprintf_s(
    sData,
    20000,
    "[PostSystem : Return Post Receive] - PostSerial[%u] - Item[%s] - Gold[%u] - Receiver[%s] - [%s %s]\r\n",
    pPost->m_dwPSSerial,
    sBuf,
    pPost->m_dwGold,
    receiver,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pFileName, sData);
}

void CMgrAvatorItemHistory::return_post_storage(CPostReturnStorage *pReturn, char *pFileName)
{
  char szTran[21]{};
  char receiver[17]{};
  char buffer[64]{};

  sData[0] = 0;
  strcat_s(sData, 20000, "\r\n\t============\r\n\r\n");
  const int size = pReturn->GetSize();
  sprintf_s(
    sBuf,
    10240,
    "RETURN POST STORAGE >> RETURN POST NUM = %d\r\n\r\n\t%s%14s%11s%18s%17s%37s%15s\r\n",
    size,
    "[No]",
    "[PostSerial]",
    "[Receiver]",
    "[State]",
    "[Title]",
    "[Item]",
    "[Gold]");
  strcat_s(sData, 20000, sBuf);
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
            64,
            "%s_%I64u_@%s[%I64u]",
            record->m_strCode,
            postData->m_dwDur,
            upgInfo,
            postData->m_lnUID);
        }
        else
        {
          sprintf_s(buffer, 64, "NoItem");
        }
        W2M(postData->m_wszTitle, szTran, 21);
        W2M(postData->m_wszRecvName, receiver, 17);
        const char *state = postData->m_byState == 1 ? "Read" : "NotRead";
        sprintf_s(
          sBuf,
          10240,
          "\t%3d %13u %10s %17s %22s %34s %10u\r\n",
          postData->m_nNumber,
          postData->m_dwPSSerial,
          state,
          receiver,
          szTran,
          buffer,
          postData->m_dwGold);
        strcat_s(sData, 20000, sBuf);
      }
    }
  }
  strcat_s(sData, 20000, "\r\n\t============\r\n\r\n");
  WriteFile(pFileName, sData);
}

void CMgrAvatorItemHistory::patriarch_push_money(
  char *pwszPatriarchName,
  unsigned int dwPushDalant,
  unsigned int dwLeftDalant,
  char *pszFileName)
{
  std::memset(sData, 0, sizeof(sData));
  sprintf_s(
    sData,
    "[PATRIARCH PUSH TAX MONEY]: PATRIARCH(%s) pay(D:%u) $D:%u [%s %s]\r\n",
    pwszPatriarchName,
    dwPushDalant,
    dwLeftDalant,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::raceboss_giveback(unsigned int dwSerial, unsigned int dwDalant, char *pszFileName)
{
  const unsigned int korTime = static_cast<unsigned int>(GetKorLocalTime());
  sprintf_s(
    sData,
    sizeof(sData),
    "[RACE BOSS]giveback: Avator Serial:%d\t$D:%u\tTime:%u\n",
    dwSerial,
    dwDalant,
    korTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::raceboss_candidate(int ncost, unsigned int dwSerial, char *pszFileName)
{
  const unsigned int korTime = static_cast<unsigned int>(GetKorLocalTime());
  sprintf_s(
    sData,
    sizeof(sData),
    "[RACE BOSS]candidate >> Avator Serial:%d\t$D:%d \t^Time:%u\n",
    dwSerial,
    ncost,
    korTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::guild_est_money_rollback(
  int n,
  char *pszGuildName,
  unsigned int dwEstDalant,
  unsigned int dwLeftDalant,
  char *pszFileName)
{
  sprintf_s(
    sData,
    "GUILD EST PAY ROLLBACK: guild(%s) pay(D:%u) $D:%u [%s %s]\r\n",
    pszGuildName,
    dwEstDalant,
    dwLeftDalant,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::guild_pop_money(
  int n,
  char *pszGuildName,
  unsigned int dwPopDalant,
  unsigned int dwPopGold,
  unsigned int dwLeftDalant,
  unsigned int dwLeftGold,
  char *pszFileName)
{
  sprintf_s(
    sData,
    "GUILD MONEY POP: guild(%s) rev(D:%u G%u) $D:%u $G:%u [%s %s]\r\n",
    pszGuildName,
    dwPopDalant,
    dwPopGold,
    dwLeftDalant,
    dwLeftGold,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::guild_pop_money_rollback(
  int n,
  char *pszGuildName,
  unsigned int dwPopDalant,
  unsigned int dwPopGold,
  unsigned int dwLeftDalant,
  unsigned int dwLeftGold,
  char *pszFileName)
{
  sprintf_s(
    sData,
    "GUILD MONEY POP CANCEL: guild(%s) pay(D:%u G%u) $D:%u $G:%u [%s %s]\r\n",
    pszGuildName,
    dwPopDalant,
    dwPopGold,
    dwLeftDalant,
    dwLeftGold,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::close(int n, char *pCloseCode, char *pszFileName)
{
sprintf_s(
  sData,
  sizeof(sData),
  "\r\nCLOSE %s [%s %s]\r\n\r\n",
  pCloseCode,
  this->m_szCurDate,
  this->m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::read_cashamount(
  unsigned int dwAC,
  unsigned int dwAV,
  int nCash,
  char *pFileName)
{
  sData[0] = 0;
  sprintf_s(sData, sizeof(sData), "[READ_CASH] : AC:%u AV:%u Cash:%u\r\n", dwAC, dwAV, nCash);
  WriteFile(pFileName, sData);
}

void CMgrAvatorItemHistory::coupon_use_buy_item(
  _STORAGE_LIST::_db_con *pCouponItem,
  char *ApplyItem,
  char *pszFileName)
{
  _CouponItem_fld *record = reinterpret_cast<_CouponItem_fld *>(
    g_Main.m_tblItemData[pCouponItem->m_byTableCode].GetRecord(pCouponItem->m_wItemIndex));
  sprintf_s(
    sData,
    "Use CouponItem at %s(%d down): %s_[%I64u] [%s %s]\r\n",
    ApplyItem,
    record->m_nPoint,
    record->m_strCode,
    pCouponItem->m_lnUID,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::buy_to_inven_cashitem(
  unsigned __int8 byTbl,
  unsigned __int16 wIndex,
  int nPrice,
  int nDis,
  int nNum,
  int nBuyPrice,
  int nAmount,
  char *pFileName,
  unsigned __int64 lnUID,
  unsigned __int8 byEventType)
{
sData[0] = 0;
  _base_fld *record = g_Main.m_tblItemData[byTbl].GetRecord(wIndex);
  __time32_t now[5];
  _time32(now);
  struct tm *timeInfo = _localtime32(now);
  if (record)
  {
    (void)GetItemTableCode(record->m_strCode);
    const unsigned int korTime = static_cast<unsigned int>(GetKorLocalTime());
    const char *itemKorName = GetItemKorName(byTbl, wIndex);
    sprintf_s(
      sData,
      "[BUY_CASHITEM] : %s(%s) [UID: %I64u] [Price:%d Discount:%d Num:%d Event: %d] [Remain:%d][T:%u/%d]\r\n",
      record->m_strCode,
      itemKorName,
      lnUID,
      nPrice,
      nDis,
      nNum,
      byEventType,
      nAmount,
      korTime,
      timeInfo->tm_sec);
  }
  else
  {
    const unsigned int korTime = static_cast<unsigned int>(GetKorLocalTime());
    sprintf_s(sData, sizeof(sData), "[BUY_CASHITEM] - [Report Server Developer][T:%u]\r\n", korTime);
  }
  WriteFile(pFileName, sData);
}

void CMgrAvatorItemHistory::cash_item_use(int n, _STORAGE_LIST::_db_con *pUseItem, char *pszFileName)
{
sData[0] = 0;
  _base_fld *record = g_Main.m_tblItemData[pUseItem->m_byTableCode].GetRecord(pUseItem->m_wItemIndex);
  const char *upgInfo = DisplayItemUpgInfo(pUseItem->m_byTableCode, pUseItem->m_dwLv);
  sprintf_s(
    sBuf,
    sizeof(sBuf),
    "USE CASH ITEM: %s_%I64u_@%s[%I64u]  [%s %s]\r\n",
    record->m_strCode,
    pUseItem->m_dwDur,
    upgInfo,
    pUseItem->m_lnUID,
    m_szCurDate,
    m_szCurTime);
  std::strcat(sData, sBuf);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::cashitem_del_from_inven(
  unsigned __int8 byTblCode,
  unsigned __int16 wItemIndex,
  unsigned __int64 lnUID,
  char *pFN)
{
  sData[0] = 0;
  _base_fld *record = g_Main.m_tblItemData[byTblCode].GetRecord(wItemIndex);
  __time32_t now[5];
  _time32(now);
  struct tm *timeInfo = _localtime32(now);
  if (record)
  {
    (void)GetItemTableCode(record->m_strCode);
    const unsigned int korTime = static_cast<unsigned int>(GetKorLocalTime());
    const char *itemKorName = GetItemKorName(byTblCode, wItemIndex);
    sprintf_s(
      sData,
      "[DEL_CASHITEM] : %s(%s) [UID:%I64u][T:%u/%d]\r\n",
      record->m_strCode,
      itemKorName,
      lnUID,
      korTime,
      timeInfo->tm_sec);
  }
  else
  {
    const unsigned int korTime = static_cast<unsigned int>(GetKorLocalTime());
    sprintf_s(
      sData,
      sizeof(sData),
      "[DEL_CASHITEM] : Tbl:%d Idx:%d [UID:%I64u][%u]\r\n",
      byTblCode,
      wItemIndex,
      lnUID,
      korTime);
  }
  WriteFile(pFN, sData);
}

void CMgrAvatorItemHistory::rollback_cashitem(
  char *szRet,
  unsigned __int64 lnUID,
  char *strItemCode,
  int nCash,
  char *pFileName)
{
  sData[0] = 0;
  const unsigned int korTime = static_cast<unsigned int>(GetKorLocalTime());
  sprintf_s(
    sData,
    sizeof(sData),
    "[CS_ROLLBACK_%s] UID:%I64u ICODE:%s Cash:%d[T:%u]\r\n",
    szRet,
    lnUID,
    strItemCode,
    nCash,
    korTime);
  WriteFile(pFileName, sData);
}

void CMgrAvatorItemHistory::used_cash(int nCurCash, int nUseCash, char *pFileName)
{
  sData[0] = 0;
  sprintf_s(
    sData,
    sizeof(sData),
    "[CASH_AMOUNT] : [cash:%d] - [used:%d] = [remain:%d]\r\n",
    nCurCash,
    nUseCash,
    nCurCash - nUseCash);
  WriteFile(pFileName, sData);
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
  sprintf_s(m_szStdPath, sizeof(m_szStdPath), "%s\\Item", returned);
  CreateDirectoryA(m_szStdPath, nullptr);

  _strdate(m_szCurDate);
  m_szCurDate[5] = '\0';
  _strtime(m_szCurTime);
  m_szCurTime[5] = '\0';

  m_tmrUpdateTime.BeginTimer(60000);

  m_listLogData_10K.SetList(254);
  m_listLogDataEmpty_10K.SetList(254);
  for (unsigned int index = 0; index < 254; ++index)
    m_listLogDataEmpty_10K.PushNode_Back(index);

  m_listLogData_1K.SetList(254);
  m_listLogDataEmpty_1K.SetList(254);
  for (unsigned int index = 0; index < 254; ++index)
    m_listLogDataEmpty_1K.PushNode_Back(index);

  m_listLogData_200.SetList(2532);
  m_listLogDataEmpty_200.SetList(2532);
  for (unsigned int index = 0; index < 2532; ++index)
    m_listLogDataEmpty_200.PushNode_Back(index);

  m_bIOThread = true;
  _beginthread(&CMgrAvatorItemHistory::IOThread, 0, this);
}

CMgrAvatorItemHistory::~CMgrAvatorItemHistory()
{
  m_bIOThread = false;
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
  const int logLen = static_cast<int>(std::strlen(pszLog));
  unsigned int outIndex = 0;

  if (logLen >= 200)
  {
    if (logLen >= 1000)
    {
      if (logLen < 0x2710 && m_listLogDataEmpty_10K.PopNode_Front(&outIndex))
      {
        __LOG_DATA_10K &entry = m_LogData_10K[outIndex];
        std::strcpy(entry.szFileName, pszFileName);
        entry.nLen = logLen;
        std::memcpy(entry.sData, pszLog, static_cast<unsigned int>(logLen));
        entry.sData[logLen] = '\0';
        m_listLogData_10K.PushNode_Back(outIndex);
        return;
      }
    }
    else if (m_listLogDataEmpty_1K.PopNode_Front(&outIndex))
    {
      __LOG_DATA_1K &entry = m_LogData_1K[outIndex];
      std::strcpy(entry.szFileName, pszFileName);
      entry.nLen = logLen;
      std::memcpy(entry.sData, pszLog, static_cast<unsigned int>(logLen));
      entry.sData[logLen] = '\0';
      m_listLogData_1K.PushNode_Back(outIndex);
      return;
    }
  }
  else if (m_listLogDataEmpty_200.PopNode_Front(&outIndex))
  {
    __LOG_DATA_200 &entry = m_LogData_200[outIndex];
    std::strcpy(entry.szFileName, pszFileName);
    entry.nLen = logLen;
    std::memcpy(entry.sData, pszLog, static_cast<unsigned int>(logLen));
    entry.sData[logLen] = '\0';
    m_listLogData_200.PushNode_Back(outIndex);
    return;
  }

  IOFileWrite_0(
    const_cast<char *>(pszFileName),
    logLen,
    const_cast<char *>(pszLog));
}

void CMgrAvatorItemHistory::char_copy(int n, char *pszDstName, unsigned int dwDstSerial, char *pszFileName)
{
sprintf_s(
  sData,
  sizeof(sData),
  "CHAR COPY: dst(%s:%d) [%s %s]\r\n",
  pszDstName,
  dwDstSerial,
  m_szCurDate,
  m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::cheat_alter_money(
  int n,
  unsigned int dwNewDalant,
  unsigned int dwNewGold,
  char *pszFileName)
{
sprintf_s(
    sData,
    "CHEAT(MONEY): $D:%u $G:%u [%s %s]\r\n",
    dwNewDalant,
    dwNewGold,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::cheat_add_item(
  int n,
  _STORAGE_LIST::_db_con *pItem,
  unsigned __int8 byAddNum,
  char *pszFileName)
{
sData[0] = 0;
  sprintf_s(sBuf, sizeof(sBuf), "CHEAT(ITEM+): num:%d [%s %s]\r\n", byAddNum, m_szCurDate, m_szCurTime);
  std::strcat(sData, sBuf);
  for (int index = 0; index < byAddNum; ++index)
  {
    if (IsProtectItem(pItem[index].m_byTableCode))
    {
      _base_fld *record = g_Main.m_tblItemData[pItem[index].m_byTableCode].GetRecord(pItem[index].m_wItemIndex);
      const char *upgInfo = DisplayItemUpgInfo(pItem[index].m_byTableCode, pItem[index].m_dwLv);
      sprintf_s(
        sBuf,
        sizeof(sBuf),
        "\t+ %s_%I64u_@%s[%I64u]\r\n",
        record->m_strCode,
        pItem[index].m_dwDur,
        upgInfo,
        pItem[index].m_lnUID);
      std::strcat(sData, sBuf);
    }
  }
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::cheat_del_item(
  int n,
  _STORAGE_LIST::_db_con *pItem,
  unsigned __int8 byDelNum,
  char *pszFileName)
{
sData[0] = 0;
  sprintf_s(sBuf, sizeof(sBuf), "CHEAT(ITEM-): num:%d [%s %s]\r\n", byDelNum, m_szCurDate, m_szCurTime);
  std::strcat(sData, sBuf);
  for (int index = 0; index < byDelNum; ++index)
  {
    if (IsProtectItem(pItem[index].m_byTableCode))
    {
      _base_fld *record = g_Main.m_tblItemData[pItem[index].m_byTableCode].GetRecord(pItem[index].m_wItemIndex);
      const char *upgInfo = DisplayItemUpgInfo(pItem[index].m_byTableCode, pItem[index].m_dwLv);
      sprintf_s(
        sBuf,
        sizeof(sBuf),
        "\t- %s_%I64u_@%s[%I64u]\r\n",
        record->m_strCode,
        pItem[index].m_dwDur,
        upgInfo,
        pItem[index].m_lnUID);
      std::strcat(sData, sBuf);
    }
  }
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::delete_npc_quest_item(int n, _STORAGE_LIST::_db_con *pItem, char *pszFileName)
{
_base_fld *record = g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
  sprintf_s(
    sData,
    sizeof(sData),
    "DELETE NPC QUEST ITEM : %s_%I64u_[%I64u] [%s %s]\r\n",
    record->m_strCode,
    pItem->m_dwDur,
    pItem->m_lnUID,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData);
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
WriteFile(pszFileName, logBuffer);
}

void CMgrAvatorItemHistory::back_trap_item(int n, _STORAGE_LIST::_db_con *pItem, char *pszFileName)
{
  _base_fld *record = g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
  sprintf_s(
    sData,
    sizeof(sData),
    "BACK TRAP ITEM : %s_%I64u_[%I64u] [%s %s]\r\n",
    record->m_strCode,
    pItem->m_dwDur,
    pItem->m_lnUID,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::pay_money(
  int n,
  const char *pszClause,
  unsigned int dwPayDalant,
  unsigned int dwPayGold,
  unsigned int dwNewDalant,
  unsigned int dwNewGold,
  char *pszFileName)
{
  sprintf_s(
    sData,
    "PAY: %s pay(D:%u G:%u) $D:%u $G:%u\r\n",
    pszClause,
    dwPayDalant,
    dwPayGold,
    dwNewDalant,
    dwNewGold);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::tuning_unit(
  int n,
  unsigned __int8 bySlotIndex,
  void *pData,
  int *pnPayMoney,
  unsigned int dwNewDalant,
  unsigned int dwNewGold,
  char *pszFileName)
{
#pragma pack(push, 1)
  struct _unit_tuning_log_view
  {
    unsigned __int8 bySlotIndex;
    unsigned __int8 byFrame;
    unsigned int dwGauge;
    unsigned __int8 byPart[6];
  };
#pragma pack(pop)
  _unit_tuning_log_view *unitData = reinterpret_cast<_unit_tuning_log_view *>(pData);

  sprintf_s(
    sData,
    "UNIT TUNING: %d>fr:%d %d/%d/%d/%d/%d/%d pay(D:%u G:%u) $D:%u $G:%u [%s %s]\r\n",
    bySlotIndex,
    unitData->byFrame,
    unitData->byPart[0],
    unitData->byPart[1],
    unitData->byPart[2],
    unitData->byPart[3],
    unitData->byPart[4],
    unitData->byPart[5],
    pnPayMoney[0],
    pnPayMoney[1],
    dwNewDalant,
    dwNewGold,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::reward_add_money(
  int n,
  const char *pszClause,
  unsigned int dwAddDalant,
  unsigned int dwAddGold,
  unsigned int dwSumDalant,
  unsigned int dwSumGold,
  char *pszFileName)
{
  sprintf_s(
    sData,
    "REWARD: %s /MONEY rev(D:%u G:%u) $D:%u $G:%u [%s %s]\r\n",
    pszClause,
    dwAddDalant,
    dwAddGold,
    dwSumDalant,
    dwSumGold,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::reward_add_item(
  int n,
  const char *pszClause,
  _STORAGE_LIST::_db_con *pItem,
  char *pszFileName)
{

  _base_fld *record = g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
  char *curTime = m_szCurTime;
  char *curDate = m_szCurDate;
  const int tableCode = pItem->m_byTableCode;
  const char *upgradeInfo = DisplayItemUpgInfo(tableCode, pItem->m_dwLv);

  sprintf_s(
    sData,
    sizeof(sData),
    "REWARD: %s /ITEM %s_%I64u_@%s[%I64u] [%s %s]\r\n",
    pszClause,
    record->m_strCode,
    pItem->m_dwDur,
    upgradeInfo,
    pItem->m_lnUID,
    curDate,
    curTime);
WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::throw_ground_item(
  int n,
  _STORAGE_LIST::_db_con *pItem,
  char *pMapCode,
  float *pfPos,
  char *pszFileName)
{
  _base_fld *record = g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
  const char *upgradeInfo = DisplayItemUpgInfo(pItem->m_byTableCode, pItem->m_dwLv);
  const int posX = static_cast<int>(pfPos[0]);
  const int posY = static_cast<int>(pfPos[1]);
  const int posZ = static_cast<int>(pfPos[2]);
  sprintf_s(
    sData,
    sizeof(sData),
    "DUMP: %s_%I64u_@%s[%I64u] \t{POS:%s (%d, %d, %d)} [%s %s]\r\n",
    record->m_strCode,
    pItem->m_dwDur,
    upgradeInfo,
    pItem->m_lnUID,
    pMapCode,
    posX,
    posY,
    posZ,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::exchange_money(
  int n,
  unsigned int dwCurDalant,
  unsigned int dwCurGold,
  unsigned int dwNewDalant,
  unsigned int dwNewGold,
  char *pszFileName)
{
  sprintf_s(
    sData,
    "EXCHANGE: D:%u->$D:%u / G:%u->$G:%u [%s %s]\r\n",
    dwCurDalant,
    dwNewDalant,
    dwCurGold,
    dwNewGold,
    m_szCurDate,
    m_szCurTime);
WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::buy_item(
  int n,
  _buy_offer *pOffer,
  unsigned __int8 byOfferNum,
  unsigned int dwCostDalant,
  unsigned int dwCostGold,
  unsigned int dwNewDalant,
  unsigned int dwNewGold,
  char *pszFileName)
{
  sData[0] = '\0';
  sprintf_s(
    sBuf,
    "BUY: num:%d pay(D:%u G:%u) $D:%u $G:%u [%s %s]\r\n",
    byOfferNum,
    dwCostDalant,
    dwCostGold,
    dwNewDalant,
    dwNewGold,
    m_szCurDate,
    m_szCurTime);
  std::strcat(sData, sBuf);

  for (int j = 0; j < byOfferNum; ++j)
  {
    _STORAGE_LIST::_db_con *pItem = &pOffer[j].Item;
    _base_fld *record =
      g_Main.m_tblItemData[pOffer[j].Item.m_byTableCode].GetRecord(pOffer[j].Item.m_wItemIndex);
    const char *upgradeInfo = DisplayItemUpgInfo(pItem->m_byTableCode, pItem->m_dwLv);
    sprintf_s(
      sBuf,
      sizeof(sBuf),
      "\t+ %s_%I64u_@%s[%I64u]\r\n",
      record->m_strCode,
      pItem->m_dwDur,
      upgradeInfo,
      pItem->m_lnUID);
    std::strcat(sData, sBuf);
  }
WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::sell_item(
  int n,
  _sell_offer *pOffer,
  unsigned __int8 byOfferNum,
  unsigned int dwIncomeDalant,
  unsigned int dwIncomeGold,
  unsigned int dwNewDalant,
  unsigned int dwNewGold,
  char *pszFileName)
{
  sData[0] = '\0';
  sprintf_s(
    sBuf,
    "SELL: num:%u rev(D:%u G:%u) $D:%u $G:%u [%s %s]\r\n",
    byOfferNum,
    dwIncomeDalant,
    dwIncomeGold,
    dwNewDalant,
    dwNewGold,
    m_szCurDate,
    m_szCurTime);
  std::strcat(sData, sBuf);

  for (int j = 0; j < byOfferNum; ++j)
  {
    _STORAGE_LIST::_db_con *pItem = pOffer[j].pItem;
    _base_fld *record = g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
    if (IsOverLapItem(pItem->m_byTableCode))
    {
      sprintf_s(
        sBuf,
        sizeof(sBuf),
        "\t- %s_%u[%I64u]\r\n",
        record->m_strCode,
        pOffer[j].byAmount,
        pItem->m_lnUID);
    }
    else
    {
      const char *upgradeInfo = DisplayItemUpgInfo(pItem->m_byTableCode, pItem->m_dwLv);
      sprintf_s(
        sBuf,
        sizeof(sBuf),
        "\t- %s_%I64u_@%s[%I64u]\r\n",
        record->m_strCode,
        pItem->m_dwDur,
        upgradeInfo,
        pItem->m_lnUID);
    }
    std::strcat(sData, sBuf);
  }
WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::add_storage_fail(
  int n,
  _STORAGE_LIST::_db_con *pItem,
  const char *strErrorCodePos,
  char *pszFileName)
{
  sprintf_s(
    sData,
    sizeof(sData),
    "Amb_AddStorage ERR - item:[%d-%d], CodePos:(%s) \r\n",
    pItem->m_byTableCode,
    pItem->m_wItemIndex,
    strErrorCodePos);
WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::make_item(
  int n,
  _STORAGE_LIST::_db_con *pMaterial,
  unsigned __int8 *pbyMtrNum,
  unsigned __int8 byMaterialNum,
  unsigned __int8 byRetCode,
  bool bInsert,
  _STORAGE_LIST::_db_con *pMakeItem,
  char *pszFileName)
{
  sData[0] = 0;
  if (byRetCode)
  {
    sprintf_s(sBuf, sizeof(sBuf), "MAKE(FAIL): [%s %s]\r\n", m_szCurDate, m_szCurTime);
    std::strcat(sData, sBuf);
  }
  else
  {
    _base_fld *record = g_Main.m_tblItemData[pMakeItem->m_byTableCode].GetRecord(pMakeItem->m_wItemIndex);
    if (bInsert)
    {
      const char *upgradeInfo = DisplayItemUpgInfo(pMakeItem->m_byTableCode, pMakeItem->m_dwLv);
      sprintf_s(
        sBuf,
        "MAKE(SUCC): %s_%I64u_@%s[%I64u] [%s %s]\r\n",
        record->m_strCode,
        pMakeItem->m_dwDur,
        upgradeInfo,
        pMakeItem->m_lnUID,
        m_szCurDate,
        m_szCurTime);
    }
    else
    {
      const char *upgradeInfo = DisplayItemUpgInfo(pMakeItem->m_byTableCode, pMakeItem->m_dwLv);
      sprintf_s(
        sBuf,
        "MAKE(QUEST): %s_%I64u_@%s[%I64u] [%s %s]\r\n",
        record->m_strCode,
        pMakeItem->m_dwDur,
        upgradeInfo,
        pMakeItem->m_lnUID,
        m_szCurDate,
        m_szCurTime);
    }
    std::strcat(sData, sBuf);
  }

  for (int j = 0; j < byMaterialNum; ++j)
  {
    _base_fld *record =
      g_Main.m_tblItemData[pMaterial[j].m_byTableCode].GetRecord(pMaterial[j].m_wItemIndex);
    sprintf_s(sBuf, sizeof(sBuf), "\t- %s_%u[%I64u]\r\n", record->m_strCode, pbyMtrNum[j], pMaterial[j].m_lnUID);
    std::strcat(sData, sBuf);
  }
WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::cheat_make_item_no_material(
  int n,
  unsigned __int8 byRetCode,
  _STORAGE_LIST::_db_con *pMakeItem,
  char *pszFileName)
{
  sData[0] = 0;
  if (byRetCode)
  {
    sprintf_s(
      sBuf,
      sizeof(sBuf),
      "MAKE(FAIL): [%s %s] ( CHEAT_MATERIAL ) \r\n",
      m_szCurDate,
      m_szCurTime);
  }
  else
  {
    _base_fld *record = g_Main.m_tblItemData[pMakeItem->m_byTableCode].GetRecord(pMakeItem->m_wItemIndex);
    const char *upgradeInfo = DisplayItemUpgInfo(pMakeItem->m_byTableCode, pMakeItem->m_dwLv);
    sprintf_s(
      sBuf,
      "MAKE(SUCC): %s_%I64u_@%s[%I64u] [%s %s] ( CHEAT_MATERIAL ) \r\n",
      record->m_strCode,
      pMakeItem->m_dwDur,
      upgradeInfo,
      pMakeItem->m_lnUID,
      m_szCurDate,
      m_szCurTime);
  }
  std::strcat(sData, sBuf);
WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::grade_up_item(
  int n,
  _STORAGE_LIST::_db_con *pItem,
  _STORAGE_LIST::_db_con *pTalik,
  _STORAGE_LIST::_db_con *pJewel,
  unsigned __int8 byJewelNum,
  unsigned __int8 byErrCode,
  unsigned int dwAfterLv,
  char *pszFileName)
{
  sData[0] = 0;
  const char *beforeUpgradeInfo = DisplayItemUpgInfo(pItem->m_byTableCode, pItem->m_dwLv);
  char beforeUpgrade[48]{};
  std::strcpy(beforeUpgrade, beforeUpgradeInfo);

  _base_fld *record = g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
  if (byErrCode)
  {
    switch (byErrCode)
    {
      case 100:
        sprintf_s(
          sBuf,
          "UPGRADE(FAIL): %s_%I64u_@%s[%I64u] [%s %s]\r\n",
          record->m_strCode,
          pItem->m_dwDur,
          beforeUpgrade,
          pItem->m_lnUID,
          m_szCurDate,
          m_szCurTime);
        std::strcat(sData, sBuf);
        break;
      case 101:
      {
        const char *afterUpgradeInfo = DisplayItemUpgInfo(pItem->m_byTableCode, dwAfterLv);
        sprintf_s(
          sBuf,
          "UPGRADE(ZERO): %s_%I64u_@%s[%I64u]->%s [%s %s]\r\n",
          record->m_strCode,
          pItem->m_dwDur,
          beforeUpgrade,
          pItem->m_lnUID,
          afterUpgradeInfo,
          m_szCurDate,
          m_szCurTime);
        std::strcat(sData, sBuf);
        break;
      }
      case 102:
        sprintf_s(
          sBuf,
          "UPGRADE(LOST): %s_%I64u_@%s[%I64u] [%s %s]\r\n",
          record->m_strCode,
          pItem->m_dwDur,
          beforeUpgrade,
          pItem->m_lnUID,
          m_szCurDate,
          m_szCurTime);
        std::strcat(sData, sBuf);
        break;
      default:
        break;
    }
  }
  else
  {
    const char *afterUpgradeInfo = DisplayItemUpgInfo(pItem->m_byTableCode, dwAfterLv);
    sprintf_s(
      sBuf,
      "UPGRADE(SUCC): %s_%I64u_@%s[%I64u]->%s [%s %s]\r\n",
      record->m_strCode,
      pItem->m_dwDur,
      beforeUpgrade,
      pItem->m_lnUID,
      afterUpgradeInfo,
      m_szCurDate,
      m_szCurTime);
    std::strcat(sData, sBuf);
  }

  _base_fld *talikRecord =
    g_Main.m_tblItemData[pTalik->m_byTableCode].GetRecord(pTalik->m_wItemIndex);
  sprintf_s(sBuf, sizeof(sBuf), "\t- T %s \r\n", talikRecord->m_strCode);
  std::strcat(sData, sBuf);
  for (int j = 0; j < byJewelNum; ++j)
  {
    _base_fld *jewelRecord =
      g_Main.m_tblItemData[pJewel[j].m_byTableCode].GetRecord(pJewel[j].m_wItemIndex);
    sprintf_s(sBuf, sizeof(sBuf), "\t- R %s\r\n", jewelRecord->m_strCode);
    std::strcat(sData, sBuf);
  }
WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::grade_down_item(
  int n,
  _STORAGE_LIST::_db_con *pItem,
  _STORAGE_LIST::_db_con *pTalik,
  unsigned int dwAfterLv,
  char *pszFileName)
{
  sData[0] = 0;
  const char *beforeUpgradeInfo = DisplayItemUpgInfo(pItem->m_byTableCode, pItem->m_dwLv);
  char beforeUpgrade[48]{};
  std::strcpy(beforeUpgrade, beforeUpgradeInfo);

  _base_fld *record = g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
  const char *afterUpgradeInfo = DisplayItemUpgInfo(pItem->m_byTableCode, dwAfterLv);
  sprintf_s(
    sBuf,
    "DOWNGRADE: %s_%I64u_@%s[%I64u] -> %s [%s %s]\r\n",
    record->m_strCode,
    pItem->m_dwDur,
    beforeUpgrade,
    pItem->m_lnUID,
    afterUpgradeInfo,
    m_szCurDate,
    m_szCurTime);
  std::strcat(sData, sBuf);

  _base_fld *talikRecord =
    g_Main.m_tblItemData[pTalik->m_byTableCode].GetRecord(pTalik->m_wItemIndex);
  sprintf_s(sBuf, sizeof(sBuf), "\t- T %s\r\n", talikRecord->m_strCode);
  std::strcat(sData, sBuf);
WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::combine_item(
  int n,
  _STORAGE_LIST::_db_con *pMaterial,
  unsigned __int8 *pbyMtrNum,
  unsigned __int8 byMaterialNum,
  _STORAGE_LIST::_db_con *pMakeItem,
  unsigned int dwFee,
  unsigned int dwLeftDalant,
  char *pszFileName)
{
  sData[0] = 0;
  _base_fld *record = g_Main.m_tblItemData[pMakeItem->m_byTableCode].GetRecord(pMakeItem->m_wItemIndex);
  const char *upgradeInfo = DisplayItemUpgInfo(pMakeItem->m_byTableCode, pMakeItem->m_dwLv);
  sprintf_s(
    sBuf,
    "COMBINE: %s_%I64u_@%s[%I64u] pay(D:%u) $D:%u [%s %s]\r\n",
    record->m_strCode,
    pMakeItem->m_dwDur,
    upgradeInfo,
    pMakeItem->m_lnUID,
    dwFee,
    dwLeftDalant,
    m_szCurDate,
    m_szCurTime);
  std::strcat(sData, sBuf);
  for (int j = 0; j < byMaterialNum; ++j)
  {
    _base_fld *materialRecord =
      g_Main.m_tblItemData[pMaterial[j].m_byTableCode].GetRecord(pMaterial[j].m_wItemIndex);
    sprintf_s(sBuf, sizeof(sBuf), "\t- %s_%d\r\n", materialRecord->m_strCode, pbyMtrNum[j]);
    std::strcat(sData, sBuf);
  }
WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::exchange_item(
  int n,
  _STORAGE_LIST::_db_con *pUseItem,
  _STORAGE_LIST::_db_con *pOutItem,
  char *pszFileName)
{
  sData[0] = 0;
  _base_fld *record = g_Main.m_tblItemData[pOutItem->m_byTableCode].GetRecord(pOutItem->m_wItemIndex);
  const char *upgradeInfo = DisplayItemUpgInfo(pOutItem->m_byTableCode, pOutItem->m_dwLv);
  sprintf_s(
    sBuf,
    "EXCHANGE: %s_%I64u_@%s[%I64u] [%s %s]\r\n",
    record->m_strCode,
    pOutItem->m_dwDur,
    upgradeInfo,
    pOutItem->m_lnUID,
    m_szCurDate,
    m_szCurTime);
  std::strcat(sData, sBuf);
  _base_fld *useRecord = g_Main.m_tblItemData[pUseItem->m_byTableCode].GetRecord(pUseItem->m_wItemIndex);
  const char *useUpgradeInfo = DisplayItemUpgInfo(pUseItem->m_byTableCode, pUseItem->m_dwLv);
  sprintf_s(sBuf, sizeof(sBuf), "\t- %s_%I64u_%s\r\n", useRecord->m_strCode, pUseItem->m_dwDur, useUpgradeInfo);
  std::strcat(sData, sBuf);
WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::cut_clear_item(
  int n,
  unsigned __int16 *pwCuttingResBuffer,
  unsigned int dwAddGold,
  unsigned int dwNewGold,
  char *pszFileName)
{
sData[0] = '\0';
  sprintf_s(
    sBuf,
    sizeof(sBuf),
    "CUT SELL: rev(G:%u) $G:%u [%s %s]\r\n",
    dwAddGold,
    dwNewGold,
    m_szCurDate,
    m_szCurTime);
  std::strcat(sData, sBuf);

  for (int resourceIndex = 0; resourceIndex < GetMaxResKind(); ++resourceIndex)
  {
    if (!pwCuttingResBuffer[resourceIndex])
    {
      continue;
    }

    _base_fld *record = g_Main.m_tblItemData[18].GetRecord(resourceIndex);
    sprintf_s(sBuf, sizeof(sBuf), "\t- %s_%d\r\n", record->m_strCode, pwCuttingResBuffer[resourceIndex]);
    std::strcat(sData, sBuf);
  }

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

void CMgrAvatorItemHistory::personal_amine_stop(
  const unsigned int *pdwMineCnt,
  int nMaxOreNum,
  unsigned __int8 byTblCode,
  unsigned __int16 wItemIndex,
  char *szFileName)
{
  std::memset(sData, 0, sizeof(sData));
  sprintf_s(sData, sizeof(sData), "[PERSONAL_AMINE_STOP] - %s\r\n", GetItemKorName(byTblCode, wItemIndex));

  char *buffer = &sData[std::strlen(sData)];
  for (int itemIndex = 0; itemIndex < nMaxOreNum; ++itemIndex)
  {
    sprintf_s(
      buffer,
      sizeof(sData) - static_cast<size_t>(buffer - sData),
      "%s >> num:%d\r\n",
      GetItemKorName(17, itemIndex),
      pdwMineCnt[itemIndex]);
    buffer += std::strlen(buffer);
  }

  WriteFile(szFileName, sData);
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

  char *buffer = &s_personal_amine_log[std::strlen(s_personal_amine_log)];
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
      buffer += std::strlen(buffer);
    }
  }

  WriteFile(szFileName, s_personal_amine_log);
}

void CMgrAvatorItemHistory::reg_auto_trade(
  int n,
  unsigned int dwRegistSerial,
  _STORAGE_LIST::_db_con *pRegItem,
  unsigned int dwPrice,
  unsigned int dwfee,
  unsigned int dwLeftDalant,
  char *pszFileName)
{
  _base_fld *record = g_Main.m_tblItemData[pRegItem->m_byTableCode].GetRecord(pRegItem->m_wItemIndex);
  const char *upgradeInfo = DisplayItemUpgInfo(pRegItem->m_byTableCode, pRegItem->m_dwLv);

  sprintf_s(
    sData,
    "REG_AUTO_TRADE: reg(%u) %s_%I64u_@%s[%I64u] pr(D:%u), tax(D:%u) $D:%u [%s %s]\r\n",
    dwRegistSerial,
    record->m_strCode,
    pRegItem->m_dwDur,
    upgradeInfo,
    pRegItem->m_lnUID,
    dwPrice,
    dwfee,
    dwLeftDalant,
    m_szCurDate,
    m_szCurTime);

  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::self_cancel_auto_trade(
  int n,
  unsigned int dwRegistSerial,
  _STORAGE_LIST::_db_con *pRegItem,
  char *pszFileName)
{
  _base_fld *record = g_Main.m_tblItemData[pRegItem->m_byTableCode].GetRecord(pRegItem->m_wItemIndex);
  const char *upgradeInfo = DisplayItemUpgInfo(pRegItem->m_byTableCode, pRegItem->m_dwLv);

  sprintf_s(
    sData,
    "CANCEL_AUTO_TRADE: reg(%u) %s_%I64u_@%s[%I64u] [%s %s]\r\n",
    dwRegistSerial,
    record->m_strCode,
    pRegItem->m_dwDur,
    upgradeInfo,
    pRegItem->m_lnUID,
    m_szCurDate,
    m_szCurTime);

  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::time_out_cancel_auto_trade(
  int n,
  unsigned int dwRegistSerial,
  _STORAGE_LIST::_db_con *pRegItem,
  char *pszFileName)
{
  _base_fld *record = g_Main.m_tblItemData[pRegItem->m_byTableCode].GetRecord(pRegItem->m_wItemIndex);
  const char *upgradeInfo = DisplayItemUpgInfo(pRegItem->m_byTableCode, pRegItem->m_dwLv);

  sprintf_s(
    sData,
    "TIMEOUT_AUTO_TRADE: reg(%u) %s_%I64u_@%s[%I64u] [%s %s]\r\n",
    dwRegistSerial,
    record->m_strCode,
    pRegItem->m_dwDur,
    upgradeInfo,
    pRegItem->m_lnUID,
    m_szCurDate,
    m_szCurTime);

  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::price_auto_trade(
  int n,
  unsigned int dwRegistSerial,
  _STORAGE_LIST::_db_con *pRegItem,
  unsigned int dwTax,
  unsigned int dwOldPrice,
  unsigned int dwNewPrice,
  char *pszFileName)
{
  _base_fld *record = g_Main.m_tblItemData[pRegItem->m_byTableCode].GetRecord(pRegItem->m_wItemIndex);
  const char *upgradeInfo = DisplayItemUpgInfo(pRegItem->m_byTableCode, pRegItem->m_dwLv);

  sprintf_s(
    sData,
    "REPRICE_AUTO_TRADE: reg(%u) %s_%I64u_@%s[%I64u] tax(%u) pr(D:%u -> %u) [%s %s]\r\n",
    dwRegistSerial,
    record->m_strCode,
    pRegItem->m_dwDur,
    upgradeInfo,
    pRegItem->m_lnUID,
    dwTax,
    dwOldPrice,
    dwNewPrice,
    m_szCurDate,
    m_szCurTime);

  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::re_reg_auto_trade(
  int n,
  unsigned int dwRegistSerial,
  _STORAGE_LIST::_db_con *pRegItem,
  unsigned int dwPrice,
  unsigned int dwfee,
  unsigned int dwLeftDalant,
  char *pszFileName)
{
  _base_fld *record = g_Main.m_tblItemData[pRegItem->m_byTableCode].GetRecord(pRegItem->m_wItemIndex);
  const char *upgradeInfo = DisplayItemUpgInfo(pRegItem->m_byTableCode, pRegItem->m_dwLv);

  sprintf_s(
    sData,
    "RE_REG_AUTO_TRADE: re_reg(%u) %s_%I64u_@%s[%I64u] pr(D:%u), tax(D:%u) $D:%u [%s %s]\r\n",
    dwRegistSerial,
    record->m_strCode,
    pRegItem->m_dwDur,
    upgradeInfo,
    pRegItem->m_lnUID,
    dwPrice,
    dwfee,
    dwLeftDalant,
    m_szCurDate,
    m_szCurTime);

  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::auto_trade_sell(
  const char *szBuyerName,
  unsigned int dwBuyerSerial,
  const char *szBuyerID,
  unsigned int dwRegistSerial,
  _STORAGE_LIST::_db_con *pItem,
  unsigned int dwPrice,
  unsigned int dwTax,
  unsigned int dwLeftDalant,
  unsigned int dwLeftGold,
  char *pszFileName)
{
  sData[0] = 0;
  sprintf_s(
    sBuf,
    sizeof(sBuf),
    "AUTO TRADE(SELL): reg(%u) buyer(%s:%d id:%s) recv(D:%u) tax(%u) $D:%u $G:%u [%s %s]\r\n",
    dwRegistSerial,
    szBuyerName,
    dwBuyerSerial,
    szBuyerID,
    dwPrice,
    dwTax,
    dwLeftDalant,
    dwLeftGold,
    m_szCurDate,
    m_szCurTime);
  strcat_s(sData, sizeof(sData), sBuf);

  _base_fld *record = g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
  const char *upgradeInfo = DisplayItemUpgInfo(pItem->m_byTableCode, pItem->m_dwLv);
  sprintf_s(sBuf, sizeof(sBuf), "\t- %s_%I64u_@%s[%I64u]\r\n", record->m_strCode, pItem->m_dwDur, upgradeInfo, pItem->m_lnUID);
  strcat_s(sData, sizeof(sData), sBuf);

  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::auto_trade_buy(
  const char *szSellerName,
  unsigned int dwSellerSerial,
  const char *szSellerID,
  unsigned int dwRegistSerial,
  _STORAGE_LIST::_db_con *pItem,
  unsigned int dwPrice,
  unsigned int dwLeftDalant,
  unsigned int dwLeftGold,
  char *pszFileName)
{
  sData[0] = 0;
  sprintf_s(
    sBuf,
    sizeof(sBuf),
    "AUTO TRADE(BUY): reg(%u) seller(%s:%d id:%s) pay(D:%u) tax(none) $D:%u $G:%u [%s %s]\r\n",
    dwRegistSerial,
    szSellerName,
    dwSellerSerial,
    szSellerID,
    dwPrice,
    dwLeftDalant,
    dwLeftGold,
    m_szCurDate,
    m_szCurTime);
  std::strcat(sData, sBuf);

  _base_fld *record = g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
  const char *upgradeInfo = DisplayItemUpgInfo(pItem->m_byTableCode, pItem->m_dwLv);
  sprintf_s(sBuf, sizeof(sBuf), "\t+ %s_%I64u_@%s[%I64u]\r\n", record->m_strCode, pItem->m_dwDur, upgradeInfo, pItem->m_lnUID);
  strcat_s(sData, sizeof(sData), sBuf);

  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::item_serial_full(int n, char *pszFileName)
{
  sprintf_s(sData, sizeof(sData), "\r\nITEM SERIAL FULL [%s %s]\r\n", m_szCurDate, m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::ClearLogBuffer()
{
  sData[0] = 0;
}

void CMgrAvatorItemHistory::WriteLog(char *pszFileName)
{
  if (sData[0])
  {
    WriteFile(pszFileName, sData);
  }
}

void CMgrAvatorItemHistory::combine_ex_reward_item(
  int n,
  unsigned __int8 byMakeNum,
  _ITEMCOMBINE_DB_BASE *pCombineDB,
  unsigned __int8 *pbyRewardTypeList,
  unsigned __int64 *lnUIDs,
  char *strFileName)
{
  if (!pCombineDB)
  {
    return;
  }

  const unsigned __int16 excelIndex = GetExcelIndexFromCombineExCheckKey(pCombineDB->m_dwCheckKey);
  const int listCount = pCombineDB->m_byItemListNum >= 24 ? 24 : pCombineDB->m_byItemListNum;
  sprintf_s(
    sData,
    "\r\nCOMBINE_EX[REWARD]\r\n\tCombine%d@%d, type:%d, num:%d, (D:%u) [%s %s]\r\n",
    excelIndex + 1,
    pCombineDB->m_dwCheckKey,
    pCombineDB->m_byDlgType,
    byMakeNum,
    pCombineDB->m_dwDalant,
    m_szCurDate,
    m_szCurTime);

  for (int itemIndex = 0; itemIndex < listCount; ++itemIndex)
  {
    _ITEMCOMBINE_DB_BASE::_LIST *list = &pCombineDB->m_List[itemIndex];
    _base_fld *record = g_Main.m_tblItemData[list->Key.byTableCode].GetRecord(list->Key.wItemIndex);
    if (!record)
    {
      continue;
    }

    if (pbyRewardTypeList[itemIndex] == 2)
    {
      const char *upgInfo = DisplayItemUpgInfo(list->Key.byTableCode, list->dwUpt);
      sprintf_s(sBuf, sizeof(sBuf), "\t%s_%u_@%s World \r\n", record->m_strCode, list->dwDur, upgInfo);
      std::strcat(sData, sBuf);
    }
    else if (pbyRewardTypeList[itemIndex] == 1)
    {
      const char *upgInfo = DisplayItemUpgInfo(list->Key.byTableCode, list->dwUpt);
      sprintf_s(
        sBuf,
        sizeof(sBuf),
        "\t%s_%I64u_@%s[%I64u] Inven \r\n",
        record->m_strCode,
        static_cast<unsigned __int64>(list->dwDur),
        upgInfo,
        lnUIDs[itemIndex]);
      std::strcat(sData, sBuf);
    }
  }

  WriteFile(strFileName, sData);
}

void CMgrAvatorItemHistory::combine_ex_using_material(
  int n,
  unsigned int dwCheckKey,
  unsigned __int8 bySlotNum,
  _STORAGE_LIST::_db_con **ppMaterial,
  unsigned __int8 *pbyMtrNum,
  unsigned int dwFee,
  char *strFileName,
  int bSucc,
  unsigned int dwFailCount)
{
  if (!ppMaterial || !pbyMtrNum)
  {
    return;
  }

  const unsigned __int16 excelIndex = GetExcelIndexFromCombineExCheckKey(dwCheckKey);
  if (bSucc)
  {
    sprintf_s(
      sData,
      "\r\nCOMBINE_EX[CONSUME]\r\n\tCombine%d@%d,  num:%d, (D:%d) [%s %s] : Succ \r\n",
      excelIndex + 1,
      dwCheckKey,
      bySlotNum,
      dwFee,
      m_szCurDate,
      m_szCurTime);
  }
  else
  {
    sprintf_s(
      sData,
      "\r\nCOMBINE_EX[CONSUME]\r\n\tCombine%d@%d,  num:%d, (D:%d) [%s %s] : Fail( %u ) \r\n",
      excelIndex + 1,
      dwCheckKey,
      bySlotNum,
      dwFee,
      m_szCurDate,
      m_szCurTime,
      dwFailCount);
  }

  for (int itemIndex = 0; itemIndex < bySlotNum; ++itemIndex)
  {
    _STORAGE_LIST::_db_con *material = ppMaterial[itemIndex];
    if (!material)
    {
      continue;
    }

    _base_fld *record = g_Main.m_tblItemData[material->m_byTableCode].GetRecord(material->m_wItemIndex);
    if (!record)
    {
      continue;
    }

    if (pbyMtrNum[itemIndex])
    {
      sprintf_s(
        sBuf,
        sizeof(sBuf),
        "\t - %s_%I64u [%u] [%I64u] Delete \r\n",
        record->m_strCode,
        material->m_dwDur,
        pbyMtrNum[itemIndex],
        material->m_lnUID);
    }
    else
    {
      sprintf_s(
        sBuf,
        sizeof(sBuf),
        "\t - %s_%I64u [%u] [%I64u]\r\n",
        record->m_strCode,
        material->m_dwDur,
        pbyMtrNum[itemIndex],
        material->m_lnUID);
    }
    std::strcat(sData, sBuf);
  }

  WriteFile(strFileName, sData);
}

void CMgrAvatorItemHistory::cut_item(
  int n,
  _STORAGE_LIST::_db_con *pOreItem,
  int nOreNum,
  unsigned __int16 *pwCuttingResBuffer,
  unsigned int dwCostDalant,
  unsigned int dwNewDalant,
  char *pszFileName)
{
  sData[0] = 0;
  _base_fld *record = g_Main.m_tblItemData[pOreItem->m_byTableCode].GetRecord(pOreItem->m_wItemIndex);
  sprintf_s(
    sBuf,
    "CUT: %s * %d pay(D:%u) $D:%u [%s %s]\r\n",
    record->m_strCode,
    nOreNum,
    dwCostDalant,
    dwNewDalant,
    m_szCurDate,
    m_szCurTime);
  std::strcat(sData, sBuf);

  for (int resourceIndex = 0; resourceIndex < GetMaxResKind(); ++resourceIndex)
  {
    if (!pwCuttingResBuffer[resourceIndex])
    {
      continue;
    }

    _base_fld *resourceRecord = g_Main.m_tblItemData[18].GetRecord(resourceIndex);
    sprintf_s(sBuf, sizeof(sBuf), "\t+ %s_%d\r\n", resourceRecord->m_strCode, pwCuttingResBuffer[resourceIndex]);
    std::strcat(sData, sBuf);
  }

  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::buy_unit(
  int n,
  unsigned __int8 bySlotIndex,
  void *pData,
  unsigned int *pdwConsumMoney,
  unsigned int dwNewDalant,
  unsigned int dwNewGold,
  char *pszFileName)
{
  _UNIT_DB_BASE::_LIST *unitData = static_cast<_UNIT_DB_BASE::_LIST *>(pData);
  sprintf_s(
    sData,
    "UNIT BUY: %d>fr:%d %d/%d/%d/%d/%d/%d pay(D:%u G%u) $D:%u $G:%u [%s %s]\r\n",
    bySlotIndex,
    unitData->byFrame,
    unitData->byPart[0],
    unitData->byPart[1],
    unitData->byPart[2],
    unitData->byPart[3],
    unitData->byPart[4],
    unitData->byPart[5],
    *pdwConsumMoney,
    pdwConsumMoney[1],
    dwNewDalant,
    dwNewGold,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::destroy_unit(
  int n,
  unsigned __int8 bySlotIndex,
  unsigned __int8 byFrameCode,
  char *pszFileName)
{
  sprintf_s(
    sData,
    sizeof(sData),
    "UNIT DESTROY: %d>fr:%d [%s %s]\r\n",
    bySlotIndex,
    byFrameCode,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::exchange_pvp_gold(
  int n,
  unsigned int dwPoint,
  unsigned int dwNewDalant,
  unsigned int dwNewGold,
  char *pszFileName)
{
  sprintf_s(
    sData,
    "CB EXCHANGE: rev(G:%d) $D:%d $G:%d [%s %s]\r\n",
    dwPoint,
    dwNewDalant,
    dwNewGold,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::guild_est_money(
  int n,
  char *pszGuildName,
  unsigned int dwEstDalant,
  unsigned int dwLeftDalant,
  char *pszFileName)
{
  sprintf_s(
    sData,
    "GUILD EST PAY: guild(%s) pay(D:%u) $D:%u [%s %s]\r\n",
    pszGuildName,
    dwEstDalant,
    dwLeftDalant,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::guild_push_money(
  int n,
  char *pszGuildName,
  unsigned int dwPushDalant,
  unsigned int dwPushGold,
  unsigned int dwLeftDalant,
  unsigned int dwLeftGold,
  char *pszFileName)
{
  sprintf_s(
    sData,
    "GUILD MONEY PUSH: guild(%s) pay(D:%u G%u) $D:%u $G:%u [%s %s]\r\n",
    pszGuildName,
    dwPushDalant,
    dwPushGold,
    dwLeftDalant,
    dwLeftGold,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::guild_push_money_rollback(
  int n,
  char *pszGuildName,
  unsigned int dwPushDalant,
  unsigned int dwPushGold,
  unsigned int dwLeftDalant,
  unsigned int dwLeftGold,
  char *pszFileName)
{
  sprintf_s(
    sData,
    "GUILD MONEY PUSH CANCEL: guild(%s) rev(D:%u G%u) $D:%u $G:%u [%s %s]\r\n",
    pszGuildName,
    dwPushDalant,
    dwPushGold,
    dwLeftDalant,
    dwLeftGold,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::guild_suggest_change_taxrate(
  unsigned int dwGuild,
  unsigned int dwMatterObj2,
  char *szFile)
{
  std::memset(sData, 0, sizeof(sData));
  const unsigned int korLocalTime = static_cast<unsigned int>(GetKorLocalTime());
  sprintf_s(sData, sizeof(sData), "[SUGGEST TAX RATE][Guild:%d] - %d - %d\r\n", dwGuild, dwMatterObj2, korLocalTime);
  WriteFile(szFile, sData);
}

void CMgrAvatorItemHistory::login_cancel_auto_trade(
  int n,
  unsigned int dwRegistSerial,
  _STORAGE_LIST::_db_con *pRegItem,
  __int64 tResultTime)
{
  _base_fld *record = g_Main.m_tblItemData[pRegItem->m_byTableCode].GetRecord(pRegItem->m_wItemIndex);
  sBuf[0] = 0;

  tm *localTime = localtime_5(&tResultTime);
  if (localTime)
  {
    const char *upgInfo = DisplayItemUpgInfo(pRegItem->m_byTableCode, pRegItem->m_dwLv);
    sprintf_s(
      sBuf,
      sizeof(sBuf),
      "TIMEOUT_AUTO_TRADE: login canceldate(%04d-%02d-%02d %02d:%02d:%02d) reg(%u) %s_%I64u_@%s[%I64u] [%s %s]\r\n",
      localTime->tm_year + 1900,
      localTime->tm_mon + 1,
      localTime->tm_mday,
      localTime->tm_hour,
      localTime->tm_min,
      localTime->tm_sec,
      dwRegistSerial,
      record->m_strCode,
      pRegItem->m_dwDur,
      upgInfo,
      pRegItem->m_lnUID,
      m_szCurDate,
      m_szCurTime);
  }
  else
  {
    const char *upgInfo = DisplayItemUpgInfo(pRegItem->m_byTableCode, pRegItem->m_dwLv);
    sprintf_s(
      sBuf,
      sizeof(sBuf),
      "TIMEOUT_AUTO_TRADE: login canceldate(invalid(%u)) reg(%u) %s_%I64u_@%s[%I64u] [%s %s]\r\n",
      static_cast<unsigned int>(tResultTime),
      dwRegistSerial,
      record->m_strCode,
      pRegItem->m_dwDur,
      upgInfo,
      pRegItem->m_lnUID,
      m_szCurDate,
      m_szCurTime);
  }

  strcat_s(sData, sizeof(sData), sBuf);
}

void CMgrAvatorItemHistory::personal_amine_install(
  unsigned __int8 byTblCode,
  unsigned __int16 wItemIndex,
  _personal_amine_inven_db_load *pCon,
  char *szFileName)
{
  std::memset(sData, 0, sizeof(sData));
  const char *itemKorName = GetItemKorName(byTblCode, wItemIndex);
  sprintf_s(sData, sizeof(sData), "[PERSONA_AMINE_INSTALL] - %s\r\n", itemKorName);

  char *buffer = &sData[std::strlen(sData)];
  for (int itemIndex = 0; itemIndex < 40; ++itemIndex)
  {
    if (!pCon->m_List[itemIndex].m_bLoad)
    {
      continue;
    }

    const char *oreName = GetItemKorName(pCon->m_List[itemIndex].m_byTableCode, pCon->m_List[itemIndex].m_wItemIndex);
    sprintf_s(
      buffer,
      sizeof(sData) - static_cast<size_t>(buffer - sData),
      "[%02d]%s >> num: %I64u\r\n",
      pCon->m_List[itemIndex].m_byStorageIndex,
      oreName,
      pCon->m_List[itemIndex].m_dwDur);
    buffer += std::strlen(buffer);
  }

  WriteFile(szFileName, sData);
}

void CMgrAvatorItemHistory::raceboss_vote(
  unsigned int dwSerial,
  unsigned int dwAvatorSerial,
  char *pAvatorName,
  char *pszFileName)
{
  const unsigned int korLocalTime = static_cast<unsigned int>(GetKorLocalTime());
  sprintf_s(
    sData,
    "[RACE BOSS]vote: Serial:%d\tTarget:%s(%d)\tTime:%d\n",
    dwSerial,
    pAvatorName,
    dwAvatorSerial,
    korLocalTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::sell_unit(
  int n,
  unsigned __int8 bySlotIndex,
  unsigned __int8 byFrameCode,
  float fGaugeRate,
  unsigned int dwSellMoney,
  unsigned int dwPayDalant,
  unsigned int dwNewDalant,
  unsigned int dwNewGold,
  char *pszFileName)
{
  sprintf_s(
    sData,
    "UNIT SELL: %d>fr:%d gauge:%.0f%% rev(D:%u) pay(D:%u) $D:%u $G:%u [%s %s]\r\n",
    bySlotIndex,
    byFrameCode,
    (float)(fGaugeRate * 100.0),
    dwSellMoney,
    dwPayDalant,
    dwNewDalant,
    dwNewGold,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::trade(
  int n,
  _STORAGE_LIST::_db_con *pOutItem,
  int nOutItemNum,
  unsigned int dwOutDalant,
  unsigned int dwOutGold,
  _STORAGE_LIST::_db_con *pInItem,
  int nInItemNum,
  unsigned int dwInDalant,
  unsigned int dwInGold,
  char *pszDstName,
  unsigned int dwDstSerial,
  char *pszDstID,
  unsigned int dwSumDalant,
  unsigned int dwSumGold,
  char *pMapCode,
  float *pfPos,
  char *pszFileName)
{
  sData[0] = 0;
  sprintf_s(
    sBuf,
    "TRADE: dst(%s:%d id:%s) pay(D:%u G:%u) rev(D:%u G:%u) $D:%u $G:%u \t{POS:%s (%d, %d, %d)} [%s %s]\r\n",
    pszDstName,
    dwDstSerial,
    pszDstID,
    dwOutDalant,
    dwOutGold,
    dwInDalant,
    dwInGold,
    dwSumDalant,
    dwSumGold,
    pMapCode,
    (int)*pfPos,
    (int)pfPos[1],
    (int)pfPos[2],
    m_szCurDate,
    m_szCurTime);
  std::strcat(sData, sBuf);

  for (int itemIndex = 0; itemIndex < nOutItemNum; ++itemIndex)
  {
    _base_fld *record = g_Main.m_tblItemData[pOutItem[itemIndex].m_byTableCode].GetRecord(pOutItem[itemIndex].m_wItemIndex);
    const char *upgInfo = DisplayItemUpgInfo(pOutItem[itemIndex].m_byTableCode, pOutItem[itemIndex].m_dwLv);
    sprintf_s(
      sBuf,
      sizeof(sBuf),
      "\t- %s_%I64u_@%s[%I64u]\r\n",
      record->m_strCode,
      pOutItem[itemIndex].m_dwDur,
      upgInfo,
      pOutItem[itemIndex].m_lnUID);
    std::strcat(sData, sBuf);
  }

  for (int itemIndex = 0; itemIndex < nInItemNum; ++itemIndex)
  {
    _base_fld *record = g_Main.m_tblItemData[pInItem[itemIndex].m_byTableCode].GetRecord(pInItem[itemIndex].m_wItemIndex);
    const char *upgInfo = DisplayItemUpgInfo(pInItem[itemIndex].m_byTableCode, pInItem[itemIndex].m_dwLv);
    sprintf_s(
      sBuf,
      sizeof(sBuf),
      "\t+ %s_%I64u_@%s[%I64u]\r\n",
      record->m_strCode,
      pInItem[itemIndex].m_dwDur,
      upgInfo,
      pInItem[itemIndex].m_lnUID);
    std::strcat(sData, sBuf);
  }

  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::trunk_io_item(
  int n,
  _STORAGE_LIST::_db_con *pIOItem,
  bool bInput,
  unsigned int dwFeeDalant,
  unsigned int dwNewDalant,
  char *pszFileName)
{
  const char *ioType[2] = { "OUT", "IN" };
  _base_fld *record = g_Main.m_tblItemData[pIOItem->m_byTableCode].GetRecord(pIOItem->m_wItemIndex);
  const char *upgInfo = DisplayItemUpgInfo(pIOItem->m_byTableCode, pIOItem->m_dwLv);
  sprintf_s(
    sData,
    "TRUNK ITEM %s: %s_%I64u_@%s[%I64u] pay(%d) $D:%d [%s %s]\r\n",
    ioType[bInput ? 1 : 0],
    record->m_strCode,
    pIOItem->m_dwDur,
    upgInfo,
    pIOItem->m_lnUID,
    dwFeeDalant,
    dwNewDalant,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::trunk_io_money(
  int n,
  bool bInput,
  unsigned int dwIODalant,
  unsigned int dwIOGold,
  unsigned int dwPayDalant,
  unsigned int dwInvenDalant,
  unsigned int dwInvenGold,
  unsigned int dwTrkDalant,
  unsigned int dwTrkGold,
  char *pszFileName)
{
  const char *ioType[2] = { "OUT", "IN" };
  sprintf_s(
    sData,
    "TRUNK MONEY %s: io(D:%u, G:%u) pay(%u) $D:%u $G:%u \t^D%u ^G:%u [%s %s]\r\n",
    ioType[bInput ? 1 : 0],
    dwIODalant,
    dwIOGold,
    dwPayDalant,
    dwInvenDalant,
    dwInvenGold,
    dwTrkDalant,
    dwTrkGold,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::trunk_swap_item(
  int n,
  _STORAGE_LIST::_db_con *pInputItem,
  _STORAGE_LIST::_db_con *pOutputItem,
  unsigned int dwFeeDalant,
  unsigned int dwNewDalant,
  char *pszFileName)
{
  _base_fld *inRecord = g_Main.m_tblItemData[pInputItem->m_byTableCode].GetRecord(pInputItem->m_wItemIndex);
  _base_fld *outRecord = g_Main.m_tblItemData[pOutputItem->m_byTableCode].GetRecord(pOutputItem->m_wItemIndex);
  const char *inUpgInfo = DisplayItemUpgInfo(pInputItem->m_byTableCode, pInputItem->m_dwLv);
  const char *outUpgInfo = DisplayItemUpgInfo(pOutputItem->m_byTableCode, pOutputItem->m_dwLv);
  sprintf_s(
    sData,
    "TRUNK ITEM SWAP: IN> %s_%I64u_@%s[%I64u] OUT> %s_%I64u_@%s[%I64u], pay(%u) $D:%u [%s %s]\r\n",
    inRecord->m_strCode,
    pInputItem->m_dwDur,
    inUpgInfo,
    pInputItem->m_lnUID,
    outRecord->m_strCode,
    pOutputItem->m_dwDur,
    outUpgInfo,
    pOutputItem->m_lnUID,
    dwFeeDalant,
    dwNewDalant,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::auto_trade_login_sell(
  const char *szBuyerName,
  unsigned int dwBuyerSerial,
  const char *szBuyerID,
  unsigned int dwRegistSerial,
  _STORAGE_LIST::_db_con *pItem,
  __int64 tResultTime,
  unsigned int dwPrice,
  unsigned int dwTax,
  unsigned int dwLeftDalant,
  unsigned int dwLeftGold)
{
  tm *localTime = localtime_5(&tResultTime);
  sBuf[0] = 0;
  if (localTime)
  {
    sprintf_s(
      sBuf,
      sizeof(sBuf),
      "AUTO TRADE(SELL): login sell selldate(%04d-%02d-%02d %02d:%02d:%02d) reg(%u) buyer(%s:%u id:%s) recv(D:%u) tax(%u) $D:%u $G:%u [%s %s]\r\n",
      localTime->tm_year + 1900,
      localTime->tm_mon + 1,
      localTime->tm_mday,
      localTime->tm_hour,
      localTime->tm_min,
      localTime->tm_sec,
      dwRegistSerial,
      szBuyerName,
      dwBuyerSerial,
      szBuyerID,
      dwPrice,
      dwTax,
      dwLeftDalant,
      dwLeftGold,
      m_szCurDate,
      m_szCurTime);
  }
  else
  {
    sprintf_s(
      sBuf,
      sizeof(sBuf),
      "AUTO TRADE(SELL): login sell selldate(invalid) reg(%u) buyer(%s:%u id:%s) recv(D:%u) tax(%u) $D:%u $G:%u [%s %s]\r\n",
      dwRegistSerial,
      szBuyerName,
      dwBuyerSerial,
      szBuyerID,
      dwPrice,
      dwTax,
      dwLeftDalant,
      dwLeftGold,
      m_szCurDate,
      m_szCurTime);
  }
  strcat_s(sData, sizeof(sData), sBuf);

  _base_fld *record = g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
  const char *upgInfo = DisplayItemUpgInfo(pItem->m_byTableCode, pItem->m_dwLv);
  sprintf_s(sBuf, sizeof(sBuf), "\t- %s_%I64u_@%s[%I64u]\r\n", record->m_strCode, pItem->m_dwDur, upgInfo, pItem->m_lnUID);
  strcat_s(sData, sizeof(sData), sBuf);
}


