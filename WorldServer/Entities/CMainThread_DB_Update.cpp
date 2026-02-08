#include "pch.h"

#include "CMainThread.h"

#include <cstdio>
#include <cstring>
#include <ctime>
#include <mmsystem.h>

#include "CCheckSumCharacAccountTrunkData.h"
#include "CPotionMgr.h"
#include "CRFWorldDatabase.h"
#include "DqsDbStructs.h"
#include "WorldServerUtil.h"

char CMainThread::db_Update_Avator(
  unsigned int dwSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData,
  bool bCheckLowHigh)
{
  char pszBaseQuery[2048]{};
  char pszGeneralQuery[2048]{};
  char pszSupplementQuery[2048]{};
  char pszInvenQuery[2048]{};
  char pszUnitQuery[2048]{};
  char pszUIQuery[2048]{};
  char pszQuestQuery[2048]{};
  char pszNPCQuestQuery[4096]{};
  char wszBuddyQuery[2048]{};
  char pszItemCombineExQuery[2048]{};
  char wszTrunkQuery[2048]{};
  char wszExtTrunkQuery[2048]{};
  char pszTimeLimitInfoQuery[2048]{};
  char pszAMPInven[10240]{};
  char pszPvpPointLimitQuery[2048]{};
  char pszCryMsgQuery[2048]{};
  char pszPvpOrderViewQurey[2048]{};
  char pszPrimiumPlayTimeQuery[2048]{};
  char pszPotionDelayQuery[2048]{};
  char pszOreCuttingQuery[2048]{};
  char pszPcBangFavorQuery[2048]{};

  char szError[2080]{};

  if (!_db_Update_Base(dwSerial, pNewData, pOldData, pszBaseQuery, bCheckLowHigh))
  {
    m_logSystemError.Write( "db_Update_Avator(sr:%d) => _db_Update_Base..failed ..", dwSerial);
    return 24;
  }

  if (!_db_Update_General(dwSerial, pNewData, pOldData, pszGeneralQuery, bCheckLowHigh))
  {
    m_logSystemError.Write( "db_Update_Avator(sr:%d) => _db_Update_General..failed ..", dwSerial);
    return 24;
  }

  if (!_db_Update_Supplement(dwSerial, pNewData, pOldData, pszSupplementQuery, 2048))
  {
    m_logSystemError.Write( "_db_Update_Supplement(sr:%d) => _db_Update_Supplement..failed ..", dwSerial);
    return 24;
  }

  if (!_db_Update_Inven(dwSerial, pNewData, pOldData, pszInvenQuery))
  {
    m_logSystemError.Write( "db_Update_Avator(sr:%d) => _db_Update_Inven..failed ..", dwSerial);
    return 24;
  }

  if ((static_cast<int>(pNewData->dbAvator.m_byRaceSexCode) >> 1) == 0
      && !_db_Update_Unit(dwSerial, pNewData, pOldData, pszUnitQuery))
  {
    m_logSystemError.Write( "db_Update_Avator(sr:%d) => _db_Update_Unit..failed ..", dwSerial);
    return 24;
  }

  if (!_db_Update_UI(dwSerial, pNewData, pOldData, pszUIQuery))
  {
    m_logSystemError.Write( "_db_Update_UI(sr:%d) => _db_Update_UI..failed ..", dwSerial);
    return 24;
  }

  if (!_db_Update_Quest(dwSerial, pNewData, pOldData, pszQuestQuery))
  {
    m_logSystemError.Write( "_db_Update_Quest(sr:%d) => _db_Update_Quest..failed ..", dwSerial);
    return 24;
  }

  if (!_db_Update_NpcQuest_History(dwSerial, pNewData, pOldData, pszNPCQuestQuery))
  {
    m_logSystemError.Write( "_db_Update_NpcQuest_History(sr:%d) => _db_Update_NpcQuest_History..failed ..", dwSerial);
    return 24;
  }

  if (!_db_Update_Start_NpcQuest_History(dwSerial, pNewData, pOldData))
  {
    m_logSystemError.Write(
      "_db_Update_Start_NpcQuest_History(sr:%d) => _db_Update_NpcQuest_History..failed ..",
      dwSerial);
    return 24;
  }

  if (!_db_Update_Buddy(dwSerial, pNewData, pOldData, wszBuddyQuery))
  {
    m_logSystemError.Write( "_db_Update_Buddy(sr:%d) => _db_Update_Buddy..failed ..", dwSerial);
    return 24;
  }

  if (!_db_Update_ItemCombineEx(dwSerial, pNewData, pOldData, pszItemCombineExQuery))
  {
    m_logSystemError.Write( "_db_Update_ItemCombineEx(sr:%d) => _db_Update_ItemCombineEx..failed ..", dwSerial);
    return 24;
  }

  if (!_db_Update_MacroData(dwSerial, &pNewData->dbMacro, &pOldData->dbMacro))
  {
    m_logSystemError.Write( "_db_Update_MacroData(sr:%d) => _db_Update_MacroData..failed ..", dwSerial);
    return 24;
  }

  if (!_db_Update_Trunk(pNewData->dbAvator.m_dwAccountSerial, pNewData, pOldData, wszTrunkQuery))
  {
    m_logSystemError.Write( "_db_Update_Trunk(sr:%d) => _db_Update_Trunk..failed ..", dwSerial);
    return 24;
  }

  if (!_db_Update_Trunk_Extend(pNewData->dbAvator.m_dwAccountSerial, pNewData, pOldData, wszExtTrunkQuery))
  {
    m_logSystemError.Write( "_db_Update_Trunk_Extend(sr:%d) => _db_Update_Trunk_Extend..failed ..", dwSerial);
    return 24;
  }

  if (!_db_Update_TimeLimitInfo(
        pNewData->dbAvator.m_dwAccountSerial,
        pNewData,
        pOldData,
        pszTimeLimitInfoQuery,
        2048))
  {
    m_logSystemError.Write(
      "_db_Update_TimeLimitInfo(sr:%d) => _db_Update_TimeLimitInfo..failed ..",
      pNewData->dbAvator.m_dwAccountSerial);
    return 24;
  }

  CCheckSumCharacAccountTrunkData checkSum{};
  checkSum.m_dwSerial = dwSerial;
  checkSum.m_dwAccountSerial = pNewData->dbAvator.m_dwAccountSerial;
  checkSum.m_byRace = static_cast<unsigned __int8>(pNewData->dbAvator.m_byRaceSexCode / 2);
  memset_0(checkSum.m_dwValues, 0, sizeof(checkSum.m_dwValues));
  memset_0(checkSum.m_dValues, 0, sizeof(checkSum.m_dValues));
  checkSum.Encode(pNewData);
  if (!checkSum.Update(m_pWorldDB))
  {
    m_logSystemError.Write( "_db_Update_NpcData(sr:%d) => _db_Update_NpcData..failed ..", dwSerial);
    return 24;
  }

  if (pOldData->dbAvator.m_dwGuildSerial != static_cast<unsigned int>(-1)
      && pOldData->dbAvator.m_dwGuildSerial
      && pOldData->dbAvator.m_dPvPPoint != pNewData->dbAvator.m_dPvPPoint)
  {
    long double dPvpPoint = pNewData->dbAvator.m_dPvPPoint - pOldData->dbAvator.m_dPvPPoint;
    if (!CRFWorldDatabase::Update_IncreaseWeeklyGuildKillPvpPointSum(
          m_pWorldDB,
          pOldData->dbAvator.m_dwGuildSerial,
          dPvpPoint))
    {
      m_logSystemError.Write(
        "Update_IncreaseWeeklyGuildKillPvpPointSum(%u,%f) failed!",
        pNewData->dbAvator.m_dwGuildSerial,
        dPvpPoint);
    }
  }

  if (pNewData->m_byHSKTime <= 2u && pNewData->m_byCristalBattleDBInfo != 3)
  {
    if (!CRFWorldDatabase::Update_CristalBattleCharInfo(
          m_pWorldDB,
          dwSerial,
          pNewData->m_byHSKTime,
          pNewData->m_byPvpGrade,
          pNewData->m_iPvpPoint,
          pNewData->m_wKillPoint,
          pNewData->m_wDiePoint))
    {
      m_logSystemError.Write(
        "Web[Pvp.Pk] Update Fail :: CharSerial[%d], HSKTime[%d], CharGrade[%d], PvpPoint[%d], KillPoint[%d], DiePoint[%d]",
        dwSerial,
        pNewData->m_byHSKTime,
        pNewData->m_byPvpGrade,
        pNewData->m_iPvpPoint,
        pNewData->m_wKillPoint,
        pNewData->m_wDiePoint);
    }

    if (!pNewData->m_bCristalBattleDateUpdate
        && !CRFWorldDatabase::update_cristalbattle_date(m_pWorldDB, dwSerial, pNewData->m_byHSKTime))
    {
      if (CRFWorldDatabase::Insert_Supplement(m_pWorldDB, dwSerial))
      {
        if (!CRFWorldDatabase::update_cristalbattle_date(m_pWorldDB, dwSerial, pNewData->m_byHSKTime))
        {
          m_logSystemError.Write(
            "Web[CristalBattle] Date 'Update&Insert&Update' Fail :: CharSerial[%d]",
            dwSerial);
        }
      }
      else
      {
        m_logSystemError.Write(
          "Web[CristalBattle] Date 'Update&Insert' Fail :: CharSerial[%d]",
          dwSerial);
      }
    }
  }

  if (pNewData->dbPersonalAmineInven.bUsable
      && CRFWorldDatabase::select_amine_personal(m_pWorldDB, dwSerial) == 2
      && !CRFWorldDatabase::insert_amine_personal(m_pWorldDB, dwSerial))
  {
    m_logSystemError.Write(
      "db_Update_Avator() : insert_amine_personal(sr:%d) failed call",
      dwSerial);
  }

  if (!_db_update_inven_AMP(dwSerial, pNewData, pOldData, pszAMPInven))
  {
    m_logSystemError.Write( "db_Update_Avator() : _db_Update_inven_AMP(sr:%d) Failed call", dwSerial);
  }

  memset_0(szError, 0, 2048);

  if (!_db_Update_PvpPointLimit(dwSerial, pNewData, pOldData, pszPvpPointLimitQuery, szError))
  {
    m_logSystemError.Write(
      "_db_Update_PvpPointLimit(...) : CharSerial(%u)  old( tUpdatedate(%d), dOriginalPoint(%f), dLimitPoint(%f), dUsePoint(%f) ) , new( tUpdatedate(%d), dOriginalPoint(%f), dLimitPoint(%f), dUsePoint(%f) ), ErrMsg(%s) Fail!",
      dwSerial,
      static_cast<int>(pOldData->dbPvpPointLimit.tUpdatedate),
      static_cast<double>(pOldData->dbPvpPointLimit.dOriginalPoint),
      static_cast<double>(pOldData->dbPvpPointLimit.dLimitPoint),
      static_cast<double>(pOldData->dbPvpPointLimit.dUsePoint),
      static_cast<int>(pNewData->dbPvpPointLimit.tUpdatedate),
      static_cast<double>(pNewData->dbPvpPointLimit.dOriginalPoint),
      static_cast<double>(pNewData->dbPvpPointLimit.dLimitPoint),
      static_cast<double>(pNewData->dbPvpPointLimit.dUsePoint),
      szError);
    return 24;
  }

  if (!_db_Update_CryMsg(dwSerial, pNewData, pOldData, pszCryMsgQuery))
  {
    m_logSystemError.Write( "_db_Update_CryMsg(sr:%d) => _db_Update_CryMsg..failed ..", dwSerial);
    return 24;
  }

  if (!_db_Update_PvpOrderView(dwSerial, pNewData, pOldData, pszPvpOrderViewQurey, szError))
  {
    m_logSystemError.Write( "_db_Update_PvpOrderView(sr:%d) => _db_Update_PvpOrderView..failed ..", dwSerial);
    return 24;
  }

  if (!_db_Update_PrimiumPlayTime(
        pNewData->dbAvator.m_dwAccountSerial,
        pNewData,
        pOldData,
        pszPrimiumPlayTimeQuery,
        szError))
  {
    m_logSystemError.Write(
      "_db_Update_PrimiumPlayTime(sr:%d) => _db_Update_PrimiumPlayTime..failed ..",
      pNewData->dbAvator.m_dwAccountSerial);
    return 24;
  }

  if (!_db_Update_PotionDelay(dwSerial, pNewData, pOldData, pszPotionDelayQuery, 2048))
  {
    m_logSystemError.Write( "_db_Update_PotionDelay(sr:%d) => _db_Update_PotionDelay..failed ..", dwSerial);
    return 24;
  }

  if (!_db_Update_OreCutting(dwSerial, pNewData, pOldData, pszOreCuttingQuery, 2048))
  {
    m_logSystemError.Write( "_db_Update_OreCutting(sr:%d) => _db_Update_OreCutting..failed ..", dwSerial);
    return 24;
  }

  if (!_db_Update_PcBangFavor(dwSerial, pNewData, pOldData, pszPcBangFavorQuery, 2048))
  {
    m_logSystemError.Write( "_db_Update_PcBangFavor(sr:%d) => _db_Update_PcBangFavor..failed ..", dwSerial);
    return 24;
  }

  _worlddb_update_char_query updateQuery{};
  updateQuery.szBaseQuery = pszBaseQuery;
  updateQuery.szGeneralQuery = pszGeneralQuery;
  updateQuery.szInvenQuery = pszInvenQuery;
  updateQuery.szUnitQuery = pszUnitQuery;
  updateQuery.szUIQuery = pszUIQuery;
  updateQuery.szQuestQuery = pszQuestQuery;
  updateQuery.wszBuddyQuery = wszBuddyQuery;
  updateQuery.szItemCombineExQuery = pszItemCombineExQuery;
  updateQuery.wszTrunkQuery = wszTrunkQuery;
  updateQuery.szAMPInvenQuery = pszAMPInven;
  updateQuery.szPvpPointLimitQuery = pszPvpPointLimitQuery;
  updateQuery.wszBossCryMsgQuery = pszCryMsgQuery;
  updateQuery.szPvpOrderViewQurey = pszPvpOrderViewQurey;
  updateQuery.szNPCQuestQuery = pszNPCQuestQuery;
  updateQuery.szPcBangPlayTimeQuery = pszPrimiumPlayTimeQuery;
  updateQuery.szSupplementQuery = pszSupplementQuery;
  updateQuery.szPotionDelayQuery = pszPotionDelayQuery;
  updateQuery.szOreCuttingQuery = pszOreCuttingQuery;
  updateQuery.szPcBangFavorQuery = pszPcBangFavorQuery;
  updateQuery.wszExtTrunkQuery = wszExtTrunkQuery;
  updateQuery.szTimeLimitInfoQuery = pszTimeLimitInfoQuery;

  if (CRFWorldDatabase::Update_CharacterData(m_pWorldDB, dwSerial, &updateQuery))
  {
    return 0;
  }
  return 24;
}

unsigned __int8 CMainThread::db_Update_PostStorage(
  unsigned int dwAvatorSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData)
{
  char buffer[2080]{};
  char source[136]{};

  _POSTDATA_DB_BASE *newPostData = &pNewData->dbPostData;
  _POSTDATA_DB_BASE *oldPostData = &pOldData->dbPostData;
  _RETURNPOST_DB_BASE *newReturnPost = &pNewData->dbPostData.dbRetPost;
  _DELPOST_DB_BASE *newDeletePost = &pNewData->dbPostData.dbDelPost;

  if (newDeletePost->m_bUpdate)
  {
    for (int storageIndex = 0; storageIndex < newDeletePost->m_nCum; ++storageIndex)
    {
      sprintf_s(buffer, 0x800u, "update tbl_PostStorage set dck=1 where ");
      if (newDeletePost->m_List[storageIndex].dwDelSerial)
      {
        sprintf_s(source, 0x80u, "serial=%d", newDeletePost->m_List[storageIndex].dwDelSerial);
      }
      else
      {
        sprintf_s(
          source,
          0x80u,
          "sindex=%d and owner=%d",
          newDeletePost->m_List[storageIndex].nStorageIndex,
          dwAvatorSerial);
      }
      strcat_s(buffer, 0x800u, source);
      CRFWorldDatabase::Update_Post(m_pWorldDB, buffer);
    }
  }

  if (newPostData->dbPost.m_bUpdate)
  {
    for (int storageIndex = 0; storageIndex < 50; ++storageIndex)
    {
      _POSTSTORAGE_DB_BASE::__list *postEntry = &newPostData->dbPost.m_PostList[storageIndex];
      _POSTSTORAGE_DB_BASE::__list *oldEntry = &oldPostData->dbPost.m_PostList[storageIndex];
      if (postEntry->bUpdate)
      {
        if (postEntry->byState != 255)
        {
          if (postEntry->bNew)
          {
            if (!CRFWorldDatabase::Select_PostStorageRecordCheck(m_pWorldDB))
            {
              CRFWorldDatabase::Insert_PostStorageRecord(m_pWorldDB);
            }
            if (CRFWorldDatabase::Select_PostStorageEmptyRecordSerial(m_pWorldDB, &postEntry->dwPSSerial))
            {
              unsigned __int8 numberList[28]{};
              numberList[0] = static_cast<unsigned __int8>(postEntry->nNumber);
              bool updated = CRFWorldDatabase::Update_PostStorageSendToRecver(
                g_Main.m_pWorldDB,
                dwAvatorSerial,
                postEntry->dwPSSerial,
                postEntry->byState,
                postEntry->wszSendName,
                postEntry->wszRecvName,
                postEntry->wszTitle,
                postEntry->wszContent,
                postEntry->nKey,
                postEntry->dwDur,
                postEntry->dwUpt,
                postEntry->dwGold,
                0,
                static_cast<unsigned __int16>(storageIndex),
                numberList,
                false,
                postEntry->lnUID);
              postEntry->bRetProc = updated;
            }
          }
          else
          {
            bool changed = false;
            if (postEntry->dwPSSerial)
            {
              sprintf_s(buffer, 0x800u, "update tbl_PostStorage set ");
              if (postEntry->byState != oldEntry->byState)
              {
                sprintf_s(source, 0x80u, "poststate=%d,", postEntry->byState);
                strcat_s(buffer, 0x800u, source);
                changed = true;
              }
              if (postEntry->nNumber != oldEntry->nNumber)
              {
                sprintf_s(source, 0x80u, "postinx=%d,", postEntry->nNumber);
                strcat_s(buffer, 0x800u, source);
                changed = true;
              }
              if (postEntry->nKey != oldEntry->nKey)
              {
                sprintf_s(source, 0x80u, "k=%d,", postEntry->nKey);
                strcat_s(buffer, 0x800u, source);
                changed = true;
              }
              if (postEntry->dwDur != oldEntry->dwDur)
              {
                sprintf_s(source, 0x80u, "d=%d,", static_cast<unsigned int>(postEntry->dwDur));
                strcat_s(buffer, 0x800u, source);
                changed = true;
              }
              if (postEntry->dwUpt != oldEntry->dwUpt)
              {
                sprintf_s(source, 0x80u, "u=%d,", postEntry->dwUpt);
                strcat_s(buffer, 0x800u, source);
                changed = true;
              }
              if (postEntry->lnUID != oldEntry->lnUID)
              {
                sprintf_s(source, 0x80u, "uid=%I64d,", postEntry->lnUID);
                strcat_s(buffer, 0x800u, source);
                changed = true;
              }
              if (postEntry->dwGold != oldEntry->dwGold)
              {
                sprintf_s(source, 0x80u, "gold=%d,", postEntry->dwGold);
                strcat_s(buffer, 0x800u, source);
                changed = true;
              }
              if (postEntry->bUpdateIndex)
              {
                sprintf_s(source, 0x80u, "sindex=%d,", storageIndex);
                strcat_s(buffer, 0x800u, source);
                changed = true;
              }
              sprintf_s(source, 0x80u, "where serial=%d", postEntry->dwPSSerial);
              buffer[strlen_0(buffer) - 1] = 32;
              strcat_s(buffer, 0x800u, source);
            }
            else
            {
              sprintf_s(
                buffer,
                0x800u,
                "update tbl_PostStorage set poststate=%d,postinx=%d,k=%d,d=%d,u=%d,gold=%d",
                postEntry->byState,
                postEntry->nNumber,
                postEntry->nKey,
                static_cast<unsigned int>(postEntry->dwDur),
                postEntry->dwUpt,
                postEntry->dwGold);
              sprintf_s(source, 0x80u, ",uid=%I64d", postEntry->lnUID);
              strcat_s(buffer, 0x800u, source);
              sprintf_s(
                source,
                0x80u,
                " where dck=0 and sindex=%d and owner=%u",
                storageIndex,
                dwAvatorSerial);
              strcat_s(buffer, 0x800u, source);
              changed = true;
            }
            if (changed)
            {
              bool updated = CRFWorldDatabase::Update_Post(m_pWorldDB, buffer);
              postEntry->bRetProc = updated;
            }
          }
        }
      }
    }
  }

  if (newReturnPost->m_bUpdate)
  {
    for (int storageIndex = 0; storageIndex < newReturnPost->m_nCum; ++storageIndex)
    {
      sprintf_s(
        buffer,
        0x800u,
        "update tbl_PostStorage set dck=1 where serial=%d",
        newReturnPost->m_RetSerials[storageIndex]);
      CRFWorldDatabase::Update_Post(m_pWorldDB, buffer);
    }
  }

  return 0;
}

char CMainThread::_db_Update_Base(
  unsigned int dwSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData,
  char *pSzQuery,
  bool bCheckLowHigh)
{
  static_cast<void>(bCheckLowHigh);

  char source[136]{};
  char *buffer = pSzQuery;
  sprintf(pSzQuery, "UPDATE tbl_base SET ");

  if (pOldData->dbAvator.m_byLevel != pNewData->dbAvator.m_byLevel)
  {
    sprintf(source, "Lv=%d,", pNewData->dbAvator.m_byLevel);
    strcat_0(buffer, source);
  }
  if (strcmp_0(pOldData->dbAvator.m_szClassCode, pNewData->dbAvator.m_szClassCode))
  {
    pNewData->dbAvator.m_szClassCode[4] = 0;
    sprintf(source, "class='%s',", pNewData->dbAvator.m_szClassCode);
    strcat_0(buffer, source);
  }
  if (pOldData->dbAvator.m_dwDalant != pNewData->dbAvator.m_dwDalant)
  {
    sprintf(source, "Dalant=%d,", pNewData->dbAvator.m_dwDalant);
    strcat_0(buffer, source);
  }
  if (pOldData->dbAvator.m_dwGold != pNewData->dbAvator.m_dwGold)
  {
    sprintf(source, "Gold=%d,", pNewData->dbAvator.m_dwGold);
    strcat_0(buffer, source);
  }

  for (int index = 0; index < 8; ++index)
  {
    if (pNewData->dbAvator.m_EquipKey[index].IsFilled())
    {
      if (pOldData->dbAvator.m_EquipKey[index].IsFilled())
      {
        int newKey = pNewData->dbAvator.m_EquipKey[index].CovDBKey();
        int oldKey = pOldData->dbAvator.m_EquipKey[index].CovDBKey();
        if (newKey != oldKey)
        {
          int dbKey = pNewData->dbAvator.m_EquipKey[index].CovDBKey();
          sprintf(source, "EK%d=%d,", index, dbKey);
          strcat_0(buffer, source);
        }
        if (pNewData->dbAvator.m_dwFixEquipLv[index] != pOldData->dbAvator.m_dwFixEquipLv[index])
        {
          sprintf(source, "EU%d=%d,", index, pNewData->dbAvator.m_dwFixEquipLv[index]);
          strcat_0(buffer, source);
        }
        if (pNewData->dbAvator.m_byCsMethod[index])
        {
          unsigned int newET = 0;
          unsigned int oldET = 0;
          if (pNewData->dbAvator.m_byCsMethod[index] == 1)
          {
            newET = pNewData->dbAvator.m_dwET[index] - g_Main.m_tmDbUpdate;
            oldET = pOldData->dbAvator.m_dwET[index] - g_Main.m_tmDbUpdate;
          }
          else if (pNewData->dbAvator.m_byCsMethod[index] == 2)
          {
            newET = pNewData->dbAvator.m_dwET[index];
            oldET = pOldData->dbAvator.m_dwET[index];
          }
          if (newET != oldET)
          {
            sprintf(source, "ET%d=%d,", index, newET);
            strcat_0(buffer, source);
          }
        }
        if (pNewData->dbAvator.m_lnUID[index] != pOldData->dbAvator.m_lnUID[index])
        {
          sprintf(source, "ES%d=%I64d,", index, pNewData->dbAvator.m_lnUID[index]);
          strcat_0(buffer, source);
        }
      }
      else
      {
        unsigned int newET = 0;
        if (pNewData->dbAvator.m_byCsMethod[index] == 1)
        {
          newET = pNewData->dbAvator.m_dwET[index] - g_Main.m_tmDbUpdate;
        }
        else if (pNewData->dbAvator.m_byCsMethod[index] == 2)
        {
          newET = pNewData->dbAvator.m_dwET[index];
        }
        int dbKey = pNewData->dbAvator.m_EquipKey[index].CovDBKey();
        sprintf(
          source,
          "EK%d=%d,EU%d=%d,ET%d=%d,ES%d=%I64d,",
          index,
          dbKey,
          index,
          pNewData->dbAvator.m_dwFixEquipLv[index],
          index,
          newET,
          index,
          pNewData->dbAvator.m_lnUID[index]);
        strcat_0(buffer, source);
      }
    }
    else if (pOldData->dbAvator.m_EquipKey[index].IsFilled())
    {
      int dbKey = pNewData->dbAvator.m_EquipKey[index].CovDBKey();
      sprintf(source, "EK%d=%d,", index, dbKey);
      strcat_0(buffer, source);
    }
  }

  sprintf(source, "Slot=%d,", pNewData->dbAvator.m_bySlotIndex);
  strcat_0(buffer, source);

  unsigned int localTime = GetKorLocalTime();
  sprintf(source, "LastConnTime=%d WHERE Serial=%d", localTime, dwSerial);
  strcat_0(buffer, source);

  return 1;
}

char CMainThread::_db_Update_General(
  unsigned int dwSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData,
  char *pSzQuery,
  bool bCheckLowHigh)
{
  static_cast<void>(bCheckLowHigh);

  char source[216]{};
  char *buffer = pSzQuery;
  sprintf(pSzQuery, "UPDATE tbl_general SET ");
  const size_t initialLength = strlen_0(buffer);

  if (pOldData->dbAvator.m_dExp != pNewData->dbAvator.m_dExp)
  {
    sprintf(source, "Exp=%f,", static_cast<double>(pNewData->dbAvator.m_dExp));
    strcat_0(buffer, source);
  }
  if (pOldData->dbAvator.m_dLossExp != pNewData->dbAvator.m_dLossExp)
  {
    sprintf(source, "LossExp=%f,", static_cast<double>(pNewData->dbAvator.m_dLossExp));
    strcat_0(buffer, source);
  }
  for (int index = 0; index < 3; ++index)
  {
    if (pOldData->dbAvator.m_zClassHistory[index] != pNewData->dbAvator.m_zClassHistory[index])
    {
      sprintf(source, "Class%d=%d,", index, pNewData->dbAvator.m_zClassHistory[index]);
      strcat_0(buffer, source);
    }
  }
  if (pOldData->dbAvator.m_dwClassInitCnt != pNewData->dbAvator.m_dwClassInitCnt)
  {
    sprintf(source, "ClassInitCnt=%d,", pNewData->dbAvator.m_dwClassInitCnt);
    strcat_0(buffer, source);
  }
  if (pOldData->dbAvator.m_byLastClassGrade != pNewData->dbAvator.m_byLastClassGrade)
  {
    sprintf(source, "LastClassGrade=%d,", pNewData->dbAvator.m_byLastClassGrade);
    strcat_0(buffer, source);
  }
  if (pOldData->dbAvator.m_dwHP != pNewData->dbAvator.m_dwHP)
  {
    sprintf(source, "HP=%d,", pNewData->dbAvator.m_dwHP);
    strcat_0(buffer, source);
  }
  if (pOldData->dbAvator.m_dwFP != pNewData->dbAvator.m_dwFP)
  {
    sprintf(source, "FP=%d,", pNewData->dbAvator.m_dwFP);
    strcat_0(buffer, source);
  }
  if (pOldData->dbAvator.m_dwSP != pNewData->dbAvator.m_dwSP)
  {
    sprintf(source, "SP=%d,", pNewData->dbAvator.m_dwSP);
    strcat_0(buffer, source);
  }
  if (pOldData->dbAvator.m_dwDP != pNewData->dbAvator.m_dwDP)
  {
    sprintf(source, "DP=%d,", pNewData->dbAvator.m_dwDP);
    strcat_0(buffer, source);
  }
  if (pOldData->dbAvator.m_byMapCode != pNewData->dbAvator.m_byMapCode)
  {
    sprintf(source, "Map=%d,", pNewData->dbAvator.m_byMapCode);
    strcat_0(buffer, source);
  }
  if (pOldData->dbAvator.m_fStartPos[0] != pNewData->dbAvator.m_fStartPos[0])
  {
    sprintf(source, "X=%f,", pNewData->dbAvator.m_fStartPos[0]);
    strcat_0(buffer, source);
  }
  if (pOldData->dbAvator.m_fStartPos[1] != pNewData->dbAvator.m_fStartPos[1])
  {
    sprintf(source, "Y=%f,", pNewData->dbAvator.m_fStartPos[1]);
    strcat_0(buffer, source);
  }
  if (pOldData->dbAvator.m_fStartPos[2] != pNewData->dbAvator.m_fStartPos[2])
  {
    sprintf(source, "Z=%f,", pNewData->dbAvator.m_fStartPos[2]);
    strcat_0(buffer, source);
  }
  if (pOldData->dbAvator.m_byBagNum != pNewData->dbAvator.m_byBagNum)
  {
    sprintf(source, "BagNum=%d,", pNewData->dbAvator.m_byBagNum);
    strcat_0(buffer, source);
  }

  for (int index = 0; index < 6; ++index)
  {
    if (pNewData->dbEquip.m_EmbellishList[index].Key.IsFilled())
    {
      if (pOldData->dbEquip.m_EmbellishList[index].Key.IsFilled())
      {
        int newKey = pNewData->dbEquip.m_EmbellishList[index].Key.CovDBKey();
        int oldKey = pOldData->dbEquip.m_EmbellishList[index].Key.CovDBKey();
        if (newKey != oldKey)
        {
          int dbKey = pNewData->dbEquip.m_EmbellishList[index].Key.CovDBKey();
          sprintf(source, "EK%d=%d,", index, dbKey);
          strcat_0(buffer, source);
        }
        if (pNewData->dbEquip.m_EmbellishList[index].wAmount
            != pOldData->dbEquip.m_EmbellishList[index].wAmount)
        {
          if (pNewData->dbEquip.m_EmbellishList[index].wAmount == 0xFFFF)
          {
            sprintf(source, "ED%d=-1,", index);
          }
          else
          {
            sprintf(
              source,
              "ED%d=%d,",
              index,
              static_cast<__int16>(pNewData->dbEquip.m_EmbellishList[index].wAmount));
          }
          strcat_0(buffer, source);
        }
        if (pNewData->dbEquip.m_EmbellishList[index].byCsMethod)
        {
          unsigned int newET = 0;
          unsigned int oldET = 0;
          if (pNewData->dbEquip.m_EmbellishList[index].byCsMethod == 1)
          {
            newET = pNewData->dbEquip.m_EmbellishList[index].dwT - g_Main.m_tmDbUpdate;
            oldET = pOldData->dbEquip.m_EmbellishList[index].dwT - g_Main.m_tmDbUpdate;
          }
          else if (pNewData->dbEquip.m_EmbellishList[index].byCsMethod == 2)
          {
            newET = pNewData->dbEquip.m_EmbellishList[index].dwT;
            oldET = pOldData->dbEquip.m_EmbellishList[index].dwT;
          }
          if (newET != oldET)
          {
            sprintf(source, "ET%d=%d,", index, newET);
            strcat_0(buffer, source);
          }
        }
        if (pNewData->dbEquip.m_EmbellishList[index].lnUID
            != pOldData->dbEquip.m_EmbellishList[index].lnUID)
        {
          sprintf(source, "ES%d=%I64d,", index, pNewData->dbEquip.m_EmbellishList[index].lnUID);
          strcat_0(buffer, source);
        }
      }
      else
      {
        unsigned int newET = 0;
        if (pNewData->dbEquip.m_EmbellishList[index].byCsMethod == 1)
        {
          newET = pNewData->dbEquip.m_EmbellishList[index].dwT - g_Main.m_tmDbUpdate;
        }
        else if (pNewData->dbEquip.m_EmbellishList[index].byCsMethod == 2)
        {
          newET = pNewData->dbEquip.m_EmbellishList[index].dwT;
        }

        if (pNewData->dbEquip.m_EmbellishList[index].wAmount == 0xFFFF)
        {
          int dbKey = pNewData->dbEquip.m_EmbellishList[index].Key.CovDBKey();
          sprintf(
            source,
            "EK%d=%d,ED%d=-1,ET%d=%d,ES%d=%I64d,",
            index,
            dbKey,
            index,
            index,
            newET,
            index,
            pNewData->dbEquip.m_EmbellishList[index].lnUID);
        }
        else
        {
          int dbKey = pNewData->dbEquip.m_EmbellishList[index].Key.CovDBKey();
          __int16 wAmount = static_cast<__int16>(pNewData->dbEquip.m_EmbellishList[index].wAmount);
          sprintf(
            source,
            "EK%d=%d,ED%d=%d,ET%d=%d,ES%d=%I64d,",
            index,
            dbKey,
            index,
            wAmount,
            index,
            newET,
            index,
            pNewData->dbEquip.m_EmbellishList[index].lnUID);
        }
        strcat_0(buffer, source);
      }
    }
    else if (pOldData->dbEquip.m_EmbellishList[index].Key.IsFilled())
    {
      int dbKey = pNewData->dbEquip.m_EmbellishList[index].Key.CovDBKey();
      sprintf(source, "EK%d=%d,", index, dbKey);
      strcat_0(buffer, source);
    }
  }

  for (int index = 0; index < 88; ++index)
  {
    if (pNewData->dbForce.m_List[index].Key.IsFilled())
    {
      if (pOldData->dbForce.m_List[index].Key.IsFilled())
      {
        int newKey = pNewData->dbForce.m_List[index].Key.CovDBKey();
        int oldKey = pOldData->dbForce.m_List[index].Key.CovDBKey();
        if (newKey != oldKey)
        {
          int dbKey = pNewData->dbForce.m_List[index].Key.CovDBKey();
          sprintf(source, "F%d=%d,", index, dbKey);
          strcat_0(buffer, source);
        }
        if (pNewData->dbForce.m_List[index].lnUID != pOldData->dbForce.m_List[index].lnUID)
        {
          sprintf(source, "FS%d=%I64d,", index, pNewData->dbForce.m_List[index].lnUID);
          strcat_0(buffer, source);
        }
      }
      else
      {
        int dbKey = pNewData->dbForce.m_List[index].Key.CovDBKey();
        sprintf(
          source,
          "F%d=%d,FS%d=%I64d,",
          index,
          dbKey,
          index,
          pNewData->dbForce.m_List[index].lnUID);
        strcat_0(buffer, source);
      }
    }
    else if (pOldData->dbForce.m_List[index].Key.IsFilled())
    {
      int dbKey = pNewData->dbForce.m_List[index].Key.CovDBKey();
      sprintf(source, "F%d=%d,", index, dbKey);
      strcat_0(buffer, source);
    }
  }

  if ((static_cast<int>(pOldData->dbAvator.m_byRaceSexCode) >> 1) == 1)
  {
    for (int index = 0; index < 4; ++index)
    {
      if (pNewData->dbAnimus.m_List[index].Key.IsFilled())
      {
        if (pOldData->dbAnimus.m_List[index].Key.IsFilled())
        {
          unsigned __int8 newKey = pNewData->dbAnimus.m_List[index].Key.CovDBKey();
          unsigned __int8 oldKey = pOldData->dbAnimus.m_List[index].Key.CovDBKey();
          if (newKey != oldKey)
          {
            unsigned __int8 dbKey = pNewData->dbAnimus.m_List[index].Key.CovDBKey();
            sprintf(
              source,
              "AK%d=%d,AS%d=%I64d,",
              index,
              dbKey,
              index,
              pNewData->dbAnimus.m_List[index].lnUID);
            strcat_0(buffer, source);
          }
          if (pNewData->dbAnimus.m_List[index].dwExp != pOldData->dbAnimus.m_List[index].dwExp)
          {
            sprintf(source, "AD%d=%I64d,", index, pNewData->dbAnimus.m_List[index].dwExp);
            strcat_0(buffer, source);
          }
          if (pNewData->dbAnimus.m_List[index].dwParam != pOldData->dbAnimus.m_List[index].dwParam)
          {
            sprintf(source, "AP%d=%d,", index, pNewData->dbAnimus.m_List[index].dwParam);
            strcat_0(buffer, source);
          }
        }
        else
        {
          unsigned __int8 dbKey = pNewData->dbAnimus.m_List[index].Key.CovDBKey();
          sprintf(
            source,
            "AK%d=%d,AD%d=%I64d,AP%d=%d,AS%d=%I64d,",
            index,
            dbKey,
            index,
            pNewData->dbAnimus.m_List[index].dwExp,
            index,
            pNewData->dbAnimus.m_List[index].dwParam,
            index,
            pNewData->dbAnimus.m_List[index].lnUID);
          strcat_0(buffer, source);
        }
      }
      else if (pOldData->dbAnimus.m_List[index].Key.IsFilled())
      {
        unsigned __int8 dbKey = pNewData->dbAnimus.m_List[index].Key.CovDBKey();
        sprintf(source, "AK%d=%d,", index, dbKey);
        strcat_0(buffer, source);
      }
    }
  }

  for (int index = 0; index < 2; ++index)
  {
    if (pOldData->dbStat.m_dwDamWpCnt[index] != pNewData->dbStat.m_dwDamWpCnt[index])
    {
      sprintf(source, "WM%d=%d,", index, pNewData->dbStat.m_dwDamWpCnt[index]);
      strcat_0(buffer, source);
    }
  }
  for (int index = 0; index < 24; ++index)
  {
    if (pOldData->dbStat.m_dwForceCum[index] != pNewData->dbStat.m_dwForceCum[index])
    {
      sprintf(source, "FM%d=%d,", index, pNewData->dbStat.m_dwForceCum[index]);
      strcat_0(buffer, source);
    }
  }
  for (int index = 0; index < 48; ++index)
  {
    if (pOldData->dbStat.m_dwSkillCum[index] != pNewData->dbStat.m_dwSkillCum[index])
    {
      sprintf(source, "SM%d=%d,", index, pNewData->dbStat.m_dwSkillCum[index]);
      strcat_0(buffer, source);
    }
  }
  for (int index = 0; index < 3; ++index)
  {
    if (pOldData->dbStat.m_dwMakeCum[index] != pNewData->dbStat.m_dwMakeCum[index])
    {
      sprintf(source, "MI%d=%d,", index, pNewData->dbStat.m_dwMakeCum[index]);
      strcat_0(buffer, source);
    }
  }
  if (pOldData->dbStat.m_dwSpecialCum != pNewData->dbStat.m_dwSpecialCum)
  {
    sprintf(source, "SR=%d,", pNewData->dbStat.m_dwSpecialCum);
    strcat_0(buffer, source);
  }
  if (pOldData->dbStat.m_dwDefenceCnt != pNewData->dbStat.m_dwDefenceCnt)
  {
    sprintf(source, "DM=%d,", pNewData->dbStat.m_dwDefenceCnt);
    strcat_0(buffer, source);
  }
  if (pOldData->dbStat.m_dwShieldCnt != pNewData->dbStat.m_dwShieldCnt)
  {
    sprintf(source, "PM=%d,", pNewData->dbStat.m_dwShieldCnt);
    strcat_0(buffer, source);
  }
  if (pOldData->dbAvator.m_dPvPPoint != pNewData->dbAvator.m_dPvPPoint)
  {
    sprintf(source, "PvpPoint=%f,", static_cast<double>(pNewData->dbAvator.m_dPvPPoint));
    strcat_0(buffer, source);
  }
  if (pOldData->dbAvator.m_dPvPCashBag != pNewData->dbAvator.m_dPvPCashBag)
  {
    sprintf(source, "PvpCashBag=%f,", static_cast<double>(pNewData->dbAvator.m_dPvPCashBag));
    strcat_0(buffer, source);
  }
  if (strcmp_0(pOldData->dbAvator.m_szBindMapCode, pNewData->dbAvator.m_szBindMapCode))
  {
    pNewData->dbAvator.m_szBindMapCode[11] = 0;
    sprintf(source, "BindMapCode='%s',", pNewData->dbAvator.m_szBindMapCode);
    strcat_0(buffer, source);
  }
  if (strcmp_0(pOldData->dbAvator.m_szBindDummy, pNewData->dbAvator.m_szBindDummy))
  {
    pNewData->dbAvator.m_szBindDummy[11] = 0;
    sprintf(source, "BindDummy='%s',", pNewData->dbAvator.m_szBindDummy);
    strcat_0(buffer, source);
  }
  if (pOldData->dbAvator.m_dwRadarDelayTime != pNewData->dbAvator.m_dwRadarDelayTime)
  {
    sprintf_s(source, 200, "RDDelay=%d,", pNewData->dbAvator.m_dwRadarDelayTime);
    strcat_0(buffer, source);
  }
  if (pOldData->dbAvator.m_dwTakeLastMentalTicket != pNewData->dbAvator.m_dwTakeLastMentalTicket)
  {
    sprintf_s(source, 200, "TakeLastMentalTicket=%d,", pNewData->dbAvator.m_dwTakeLastMentalTicket);
    strcat_0(buffer, source);
  }
  if (pOldData->dbAvator.m_dwTakeLastCriTicket != pNewData->dbAvator.m_dwTakeLastCriTicket)
  {
    sprintf_s(source, 200, "TakeLastCriTicket=%d,", pNewData->dbAvator.m_dwTakeLastCriTicket);
    strcat_0(buffer, source);
  }
  if (pOldData->dbAvator.m_byMaxLevel != pNewData->dbAvator.m_byMaxLevel)
  {
    sprintf(source, "MaxLevel=%d,", pNewData->dbAvator.m_byMaxLevel);
    strcat_0(buffer, source);
  }
  if (pNewData->dbCutting.m_bOldDataLoad)
  {
    strcat_0(buffer, "LeftResList='*',");
  }
  sprintf(source, "TotalPlayMin=%d,", pNewData->dbAvator.m_dwTotalPlayMin);
  strcat_0(buffer, source);

  size_t queryLength = strlen_0(buffer);
  if (queryLength <= initialLength)
  {
    memset_0(buffer, 0, initialLength);
  }
  else
  {
    sprintf(source, "WHERE Serial = %d", dwSerial);
    size_t len = strlen_0(buffer);
    buffer[len - 1] = 32;
    strcat_0(buffer, source);
  }
  return 1;
}

char CMainThread::_db_Update_Supplement(
  unsigned int dwSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData,
  char *pSzQuery,
  int nBufferSize)
{
  char buffer[132]{};
  sprintf_s(pSzQuery, nBufferSize, "UPDATE tbl_supplement Set ");
  int initialLength = static_cast<int>(strlen_0(pSzQuery));

  if (pOldData->dbSupplement.dPvpPointLeak != pNewData->dbSupplement.dPvpPointLeak)
  {
    sprintf_s(buffer, 0x80u, "PvpPointLeak=%f,", static_cast<double>(pNewData->dbSupplement.dPvpPointLeak));
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  if (pOldData->dbSupplement.bLastAttBuff != pNewData->dbSupplement.bLastAttBuff)
  {
    int lastAttBuff = pNewData->dbSupplement.bLastAttBuff;
    sprintf_s(buffer, 0x80u, "LastAttBuff=%d,", lastAttBuff);
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  if (pOldData->dbSupplement.dwBufPotionEndTime != pNewData->dbSupplement.dwBufPotionEndTime)
  {
    sprintf_s(buffer, 0x80u, "BufEndTime = %u,", pNewData->dbSupplement.dwBufPotionEndTime);
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  if (pOldData->dbSupplement.dwRaceBuffClear != pNewData->dbSupplement.dwRaceBuffClear)
  {
    sprintf_s(buffer, 0x80u, "RaceBuffClear = %u,", pNewData->dbSupplement.dwRaceBuffClear);
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  if (pOldData->dbSupplement.dwAccumPlayTime != pNewData->dbSupplement.dwAccumPlayTime)
  {
    sprintf_s(buffer, 0x80u, " AccumPlayTime = %d,", pNewData->dbSupplement.dwAccumPlayTime);
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  if (pOldData->dbSupplement.dwLastResetDate != pNewData->dbSupplement.dwLastResetDate)
  {
    sprintf_s(buffer, 0x80u, " ResetAccumPlayTime = %d,", pNewData->dbSupplement.dwLastResetDate);
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  if (pOldData->dbSupplement.byVoted != pNewData->dbSupplement.byVoted)
  {
    sprintf_s(buffer, 0x80u, " IsVoted = %d,", pNewData->dbSupplement.byVoted);
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  if (pOldData->dbSupplement.VoteEnable != pNewData->dbSupplement.VoteEnable)
  {
    sprintf_s(buffer, 0x80u, " VoteEnable = %d,", pNewData->dbSupplement.VoteEnable);
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  if (pOldData->dbSupplement.wScanerCnt != pNewData->dbSupplement.wScanerCnt)
  {
    sprintf_s(
      buffer,
      0x80u,
      " ScanerCnt = %d%d,",
      pNewData->dbSupplement.dwScanerGetDate,
      pNewData->dbSupplement.wScanerCnt);
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  for (int index = 0; index < 3; ++index)
  {
    if (pOldData->dbSupplement.dwActionPoint[index] != pNewData->dbSupplement.dwActionPoint[index])
    {
      sprintf_s(buffer, 0x80u, " ActionPoint_%d = %d,", index, pNewData->dbSupplement.dwActionPoint[index]);
      strcat_s(pSzQuery, nBufferSize, buffer);
    }
  }

  const int embellishIndex = 6;
  if (pNewData->dbEquip.m_EmbellishList[embellishIndex].Key.IsFilled())
  {
    if (pOldData->dbEquip.m_EmbellishList[embellishIndex].Key.IsFilled())
    {
      int newKey = pNewData->dbEquip.m_EmbellishList[embellishIndex].Key.CovDBKey();
      int oldKey = pOldData->dbEquip.m_EmbellishList[embellishIndex].Key.CovDBKey();
      if (newKey != oldKey)
      {
        int dbKey = pNewData->dbEquip.m_EmbellishList[embellishIndex].Key.CovDBKey();
        sprintf_s(buffer, 0x80u, "EK%d=%d,", embellishIndex, dbKey);
        strcat_s(pSzQuery, nBufferSize, buffer);
      }
      if (pNewData->dbEquip.m_EmbellishList[embellishIndex].wAmount
          != pOldData->dbEquip.m_EmbellishList[embellishIndex].wAmount)
      {
        if (pNewData->dbEquip.m_EmbellishList[embellishIndex].wAmount == 0xFFFF)
        {
          sprintf_s(buffer, 0x80u, "ED%d=-1,", embellishIndex);
        }
        else
        {
          sprintf_s(
            buffer,
            0x80u,
            "ED%d=%d,",
            embellishIndex,
            static_cast<__int16>(pNewData->dbEquip.m_EmbellishList[embellishIndex].wAmount));
        }
        strcat_s(pSzQuery, nBufferSize, buffer);
      }
      if (pNewData->dbEquip.m_EmbellishList[embellishIndex].byCsMethod)
      {
        unsigned int newET = 0;
        unsigned int oldET = 0;
        if (pNewData->dbEquip.m_EmbellishList[embellishIndex].byCsMethod == 1)
        {
          newET = pNewData->dbEquip.m_EmbellishList[embellishIndex].dwT - g_Main.m_tmDbUpdate;
          oldET = pOldData->dbEquip.m_EmbellishList[embellishIndex].dwT - g_Main.m_tmDbUpdate;
        }
        else if (pNewData->dbEquip.m_EmbellishList[embellishIndex].byCsMethod == 2)
        {
          newET = pNewData->dbEquip.m_EmbellishList[embellishIndex].dwT;
          oldET = pOldData->dbEquip.m_EmbellishList[embellishIndex].dwT;
        }
        if (newET != oldET)
        {
          sprintf_s(buffer, 0x80u, "ET%d=%d,", embellishIndex, newET);
          strcat_s(pSzQuery, nBufferSize, buffer);
        }
      }
      if (pNewData->dbEquip.m_EmbellishList[embellishIndex].lnUID
          != pOldData->dbEquip.m_EmbellishList[embellishIndex].lnUID)
      {
        sprintf_s(
          buffer,
          0x80u,
          "ES%d=%I64d,",
          embellishIndex,
          pNewData->dbEquip.m_EmbellishList[embellishIndex].lnUID);
        strcat_s(pSzQuery, nBufferSize, buffer);
      }
    }
    else
    {
      unsigned int newET = 0;
      if (pNewData->dbEquip.m_EmbellishList[embellishIndex].byCsMethod == 1)
      {
        newET = pNewData->dbEquip.m_EmbellishList[embellishIndex].dwT - g_Main.m_tmDbUpdate;
      }
      else if (pNewData->dbEquip.m_EmbellishList[embellishIndex].byCsMethod == 2)
      {
        newET = pNewData->dbEquip.m_EmbellishList[embellishIndex].dwT;
      }

      if (pNewData->dbEquip.m_EmbellishList[embellishIndex].wAmount == 0xFFFF)
      {
        int dbKey = pNewData->dbEquip.m_EmbellishList[embellishIndex].Key.CovDBKey();
        sprintf_s(
          buffer,
          0x80u,
          "EK%d=%d,ED%d=-1,ET%d=%d,ES%d=%I64d,",
          embellishIndex,
          dbKey,
          embellishIndex,
          embellishIndex,
          newET,
          embellishIndex,
          pNewData->dbEquip.m_EmbellishList[embellishIndex].lnUID);
      }
      else
      {
        int dbKey = pNewData->dbEquip.m_EmbellishList[embellishIndex].Key.CovDBKey();
        __int16 amount = static_cast<__int16>(pNewData->dbEquip.m_EmbellishList[embellishIndex].wAmount);
        sprintf_s(
          buffer,
          0x80u,
          "EK%d=%d,ED%d=%d,ET%d=%d,ES%d=%I64d,",
          embellishIndex,
          dbKey,
          embellishIndex,
          amount,
          embellishIndex,
          newET,
          embellishIndex,
          pNewData->dbEquip.m_EmbellishList[embellishIndex].lnUID);
      }
      strcat_s(pSzQuery, nBufferSize, buffer);
    }
  }
  else if (pOldData->dbEquip.m_EmbellishList[embellishIndex].Key.IsFilled())
  {
    int dbKey = pNewData->dbEquip.m_EmbellishList[embellishIndex].Key.CovDBKey();
    sprintf_s(buffer, 0x80u, "EK%d=%d,", embellishIndex, dbKey);
    strcat_s(pSzQuery, nBufferSize, buffer);
  }

  size_t queryLength = strlen_0(pSzQuery);
  if (queryLength <= static_cast<size_t>(initialLength))
  {
    memset_0(pSzQuery, 0, initialLength);
  }
  else
  {
    sprintf_s(buffer, 0x80u, "WHERE Serial = %d", dwSerial);
    pSzQuery[strlen_0(pSzQuery) - 1] = 32;
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  return 1;
}

char CMainThread::_db_Update_Unit(
  unsigned int dwSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData,
  char *pSzQuery)
{
  char source[136]{};
  char *buffer = pSzQuery;
  sprintf(pSzQuery, "UPDATE tbl_unit SET ");
  size_t initialLength = strlen_0(buffer);

  for (int unitIndex = 0; unitIndex < 4; ++unitIndex)
  {
    if (pNewData->dbUnit.m_List[unitIndex].byFrame == 255)
    {
      if (pOldData->dbUnit.m_List[unitIndex].byFrame != 255)
      {
        sprintf(source, "F_%d=%d,", unitIndex, 255);
        strcat_0(buffer, source);
      }
    }
    else
    {
      if (pNewData->dbUnit.m_List[unitIndex].byFrame != pOldData->dbUnit.m_List[unitIndex].byFrame)
      {
        sprintf(source, "F_%d=%d,", unitIndex, pNewData->dbUnit.m_List[unitIndex].byFrame);
        strcat_0(buffer, source);
      }
      if (pNewData->dbUnit.m_List[unitIndex].dwGauge != pOldData->dbUnit.m_List[unitIndex].dwGauge)
      {
        sprintf(source, "Gg_%d=%d,", unitIndex, pNewData->dbUnit.m_List[unitIndex].dwGauge);
        strcat_0(buffer, source);
      }
      if (pNewData->dbUnit.m_List[unitIndex].byPart[0] != pOldData->dbUnit.m_List[unitIndex].byPart[0])
      {
        sprintf(source, "H_%d=%d,", unitIndex, pNewData->dbUnit.m_List[unitIndex].byPart[0]);
        strcat_0(buffer, source);
      }
      if (pNewData->dbUnit.m_List[unitIndex].byPart[1] != pOldData->dbUnit.m_List[unitIndex].byPart[1])
      {
        sprintf(source, "U_%d=%d,", unitIndex, pNewData->dbUnit.m_List[unitIndex].byPart[1]);
        strcat_0(buffer, source);
      }
      if (pNewData->dbUnit.m_List[unitIndex].byPart[2] != pOldData->dbUnit.m_List[unitIndex].byPart[2])
      {
        sprintf(source, "L_%d=%d,", unitIndex, pNewData->dbUnit.m_List[unitIndex].byPart[2]);
        strcat_0(buffer, source);
      }
      if (pNewData->dbUnit.m_List[unitIndex].byPart[3] != pOldData->dbUnit.m_List[unitIndex].byPart[3])
      {
        sprintf(source, "A_%d=%d,", unitIndex, pNewData->dbUnit.m_List[unitIndex].byPart[3]);
        strcat_0(buffer, source);
      }
      if (pNewData->dbUnit.m_List[unitIndex].byPart[4] != pOldData->dbUnit.m_List[unitIndex].byPart[4])
      {
        sprintf(source, "S_%d=%d,", unitIndex, pNewData->dbUnit.m_List[unitIndex].byPart[4]);
        strcat_0(buffer, source);
      }
      if (pNewData->dbUnit.m_List[unitIndex].byPart[5] != pOldData->dbUnit.m_List[unitIndex].byPart[5])
      {
        sprintf(source, "B_%d=%d,", unitIndex, pNewData->dbUnit.m_List[unitIndex].byPart[5]);
        strcat_0(buffer, source);
      }
      if (pNewData->dbUnit.m_List[unitIndex].dwBullet[0] != pOldData->dbUnit.m_List[unitIndex].dwBullet[0])
      {
        sprintf(source, "AB_%d=%d,", unitIndex, pNewData->dbUnit.m_List[unitIndex].dwBullet[0]);
        strcat_0(buffer, source);
      }
      if (pNewData->dbUnit.m_List[unitIndex].dwBullet[1] != pOldData->dbUnit.m_List[unitIndex].dwBullet[1])
      {
        sprintf(source, "SB_%d=%d,", unitIndex, pNewData->dbUnit.m_List[unitIndex].dwBullet[1]);
        strcat_0(buffer, source);
      }
      for (int spareIndex = 0; spareIndex < 8; ++spareIndex)
      {
        if (pNewData->dbUnit.m_List[unitIndex].dwSpare[spareIndex]
            != pOldData->dbUnit.m_List[unitIndex].dwSpare[spareIndex])
        {
          sprintf(
            source,
            "Sp%d_%d=%d,",
            spareIndex,
            unitIndex,
            pNewData->dbUnit.m_List[unitIndex].dwSpare[spareIndex]);
          strcat_0(buffer, source);
        }
      }
      if (pNewData->dbUnit.m_List[unitIndex].nPullingFee != pOldData->dbUnit.m_List[unitIndex].nPullingFee)
      {
        sprintf(source, "PF_%d=%d,", unitIndex, pNewData->dbUnit.m_List[unitIndex].nPullingFee);
        strcat_0(buffer, source);
      }
      if (pNewData->dbUnit.m_List[unitIndex].dwCutTime != pOldData->dbUnit.m_List[unitIndex].dwCutTime)
      {
        sprintf(source, "Cut_%d=%d,", unitIndex, pNewData->dbUnit.m_List[unitIndex].dwCutTime);
        strcat_0(buffer, source);
      }
    }
  }

  size_t queryLength = strlen_0(buffer);
  if (queryLength <= initialLength)
  {
    memset_0(buffer, 0, initialLength);
  }
  else
  {
    sprintf(source, "WHERE Serial=%d", dwSerial);
    size_t length = strlen_0(buffer);
    buffer[length - 1] = 32;
    strcat_0(buffer, source);
  }
  return 1;
}

char CMainThread::_db_Update_UI(
  unsigned int dwSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData,
  char *pSzQuery)
{
  char source[136]{};
  char *buffer = pSzQuery;
  sprintf(pSzQuery, "UPDATE tbl_userinterface SET ");
  size_t initialLength = strlen_0(buffer);

  for (int index = 0; index < 50; ++index)
  {
    int oldKey = pOldData->dbLink.m_LinkList[index].Key.CovDBKey();
    int newKey = pNewData->dbLink.m_LinkList[index].Key.CovDBKey();
    if (oldKey != newKey)
    {
      int dbKey = pNewData->dbLink.m_LinkList[index].Key.CovDBKey();
      sprintf(source, "LB%d=%d,", index, dbKey);
      strcat_0(buffer, source);
    }
  }
  for (int index = 0; index < 8; ++index)
  {
    if (pOldData->dbSfcont.m_List[0][index].dwKey != pNewData->dbSfcont.m_List[0][index].dwKey)
    {
      sprintf(source, "DCF%d=%d,", index, pNewData->dbSfcont.m_List[0][index].dwKey);
      strcat_0(buffer, source);
    }
  }
  for (int index = 0; index < 8; ++index)
  {
    if (pOldData->dbSfcont.m_List[1][index].dwKey != pNewData->dbSfcont.m_List[1][index].dwKey)
    {
      sprintf(source, "HCF%d=%d,", index, pNewData->dbSfcont.m_List[1][index].dwKey);
      strcat_0(buffer, source);
    }
  }
  for (int index = 0; index < 2; ++index)
  {
    if (pOldData->dbLink.m_dwSkill[index] != pNewData->dbLink.m_dwSkill[index])
    {
      sprintf(source, "SkillWin%d=%d,", index, pNewData->dbLink.m_dwSkill[index]);
      strcat_0(buffer, source);
    }
  }
  for (int index = 0; index < 2; ++index)
  {
    if (pOldData->dbLink.m_dwForce[index] != pNewData->dbLink.m_dwForce[index])
    {
      sprintf(source, "ForceWin%d=%d,", index, pNewData->dbLink.m_dwForce[index]);
      strcat_0(buffer, source);
    }
  }
  for (int index = 0; index < 2; ++index)
  {
    if (pOldData->dbLink.m_dwCharacter[index] != pNewData->dbLink.m_dwCharacter[index])
    {
      sprintf(source, "CharWin%d=%d,", index, pNewData->dbLink.m_dwCharacter[index]);
      strcat_0(buffer, source);
    }
  }
  for (int index = 0; index < 2; ++index)
  {
    if (pOldData->dbLink.m_dwAnimus[index] != pNewData->dbLink.m_dwAnimus[index])
    {
      sprintf(source, "AnimusWin%d=%d,", index, pNewData->dbLink.m_dwAnimus[index]);
      strcat_0(buffer, source);
    }
  }
  if (pOldData->dbLink.m_dwInven != pNewData->dbLink.m_dwInven)
  {
    sprintf(source, "InvenWin=%d,", pNewData->dbLink.m_dwInven);
    strcat_0(buffer, source);
  }
  for (int index = 0; index < 5; ++index)
  {
    if (pOldData->dbLink.m_dwInvenBag[index] != pNewData->dbLink.m_dwInvenBag[index])
    {
      sprintf(source, "InvenBag%d=%d,", index, pNewData->dbLink.m_dwInvenBag[index]);
      strcat_0(buffer, source);
    }
  }
  if (pOldData->dbLink.m_byLinkBoardLock != pNewData->dbLink.m_byLinkBoardLock)
  {
    sprintf(source, "LBLock=%d,", pNewData->dbLink.m_byLinkBoardLock);
    strcat_0(buffer, source);
  }

  size_t queryLength = strlen_0(buffer);
  if (queryLength <= initialLength)
  {
    memset_0(buffer, 0, initialLength);
  }
  else
  {
    sprintf(source, "WHERE Serial=%d", dwSerial);
    size_t length = strlen_0(buffer);
    buffer[length - 1] = 32;
    strcat_0(buffer, source);
  }
  return 1;
}

char CMainThread::_db_Update_Quest(
  unsigned int dwSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData,
  char *pSzQuery)
{
  char source[136]{};
  char *buffer = pSzQuery;
  sprintf(pSzQuery, "UPDATE tbl_quest SET ");
  size_t initialLength = strlen_0(buffer);

  for (int index = 0; index < 30; ++index)
  {
    if (pNewData->dbQuest.m_List[index].byQuestType != pOldData->dbQuest.m_List[index].byQuestType)
    {
      sprintf(source, "Type%d=%d,", index, pNewData->dbQuest.m_List[index].byQuestType);
      strcat_0(buffer, source);
    }
    if (pNewData->dbQuest.m_List[index].wIndex != pOldData->dbQuest.m_List[index].wIndex)
    {
      sprintf(source, "QuestIndex%d=%d,", index, static_cast<__int16>(pNewData->dbQuest.m_List[index].wIndex));
      strcat_0(buffer, source);
    }
    if (pNewData->dbQuest.m_List[index].dwPassSec != pOldData->dbQuest.m_List[index].dwPassSec)
    {
      sprintf(source, "PassSec%d=%d,", index, pNewData->dbQuest.m_List[index].dwPassSec);
      strcat_0(buffer, source);
    }
    if (pNewData->dbQuest.m_List[index].wNum[0] != pOldData->dbQuest.m_List[index].wNum[0])
    {
      sprintf(source, "FirstAct%d=%d,", index, static_cast<__int16>(pNewData->dbQuest.m_List[index].wNum[0]));
      strcat_0(buffer, source);
    }
    if (pNewData->dbQuest.m_List[index].wNum[1] != pOldData->dbQuest.m_List[index].wNum[1])
    {
      sprintf(source, "SecondAct%d=%d,", index, static_cast<__int16>(pNewData->dbQuest.m_List[index].wNum[1]));
      strcat_0(buffer, source);
    }
    if (pNewData->dbQuest.m_List[index].wNum[2] != pOldData->dbQuest.m_List[index].wNum[2])
    {
      sprintf(source, "ThirdAct%d=%d,", index, static_cast<__int16>(pNewData->dbQuest.m_List[index].wNum[2]));
      strcat_0(buffer, source);
    }
  }

  size_t queryLength = strlen_0(buffer);
  if (queryLength <= initialLength)
  {
    memset_0(buffer, 0, initialLength);
  }
  else
  {
    sprintf(source, "WHERE Serial=%d", dwSerial);
    size_t length = strlen_0(buffer);
    buffer[length - 1] = 32;
    strcat_0(buffer, source);
  }
  return 1;
}

char CMainThread::_db_Update_NpcQuest_History(
  unsigned int dwSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData,
  char *pSzQuery)
{
  char buffer[1288]{};
  char *destination = pSzQuery;
  strcpy_0(pSzQuery, "UPDATE tbl_npc_quest_history SET ");
  size_t initialLength = strlen_0(destination);

  for (int index = 0; index < 70; ++index)
  {
    if (pNewData->dbQuest.m_History[index].byLevel != pOldData->dbQuest.m_History[index].byLevel
        || strncmp_0(
          pNewData->dbQuest.m_History[index].szQuestCode,
          pOldData->dbQuest.m_History[index].szQuestCode,
          7))
    {
      int level = pNewData->dbQuest.m_History[index].byLevel;
      sprintf(
        buffer,
        "Code%d='%s',Level%d=%d ,Time%d=%d,",
        index + 1,
        pNewData->dbQuest.m_History[index].szQuestCode,
        index + 1,
        level,
        index + 1,
        pNewData->dbQuest.m_History[index].dwEventEndTime);
      strcat_0(destination, buffer);
    }
  }

  size_t queryLength = strlen_0(destination);
  if (queryLength <= initialLength)
  {
    memset_0(destination, 0, initialLength);
  }
  else
  {
    sprintf(buffer, "WHERE Serial=%d", dwSerial);
    size_t length = strlen_0(destination);
    destination[length - 1] = 32;
    strcat_0(destination, buffer);
  }
  return 1;
}

char CMainThread::_db_Update_Start_NpcQuest_History(
  unsigned int dwSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData)
{
  if (pNewData->dbQuest.dwListCnt > pOldData->dbQuest.dwListCnt)
  {
    unsigned int addCount = pNewData->dbQuest.dwListCnt - pOldData->dbQuest.dwListCnt;
    unsigned int listIndex = pOldData->dbQuest.dwListCnt;
    char timeBuffer[64]{};
    unsigned short timeParts[8]{};
    unsigned char rawTime[16]{};
    unsigned int count = 0;
    while (count < addCount)
    {
      memcpy_0(rawTime, &pNewData->dbQuest.m_StartHistory[listIndex].tmStartTime, sizeof(rawTime));
      memcpy_0(timeParts, rawTime, sizeof(rawTime));
      sprintf_s(
        timeBuffer,
        0x19u,
        "%04d-%02d-%02d %02d:%02d:%02d.%d",
        timeParts[0],
        timeParts[1],
        timeParts[3],
        timeParts[4],
        timeParts[5],
        timeParts[6],
        timeParts[7]);
      if (!CRFWorldDatabase::Insert_Start_NpcQuest_History(
            m_pWorldDB,
            dwSerial,
            pNewData->dbQuest.m_StartHistory[listIndex].szQuestCode,
            pNewData->dbQuest.m_StartHistory[listIndex].byLevel,
            timeBuffer,
            pNewData->dbQuest.m_StartHistory[listIndex].nEndTime))
      {
        return 0;
      }
      memset_0(timeBuffer, 0, 0x19u);
      memset_0(timeParts, 0, sizeof(timeParts));
      ++count;
      ++listIndex;
    }
  }
  return 1;
}

char CMainThread::_db_Update_Buddy(
  unsigned int dwSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData,
  char *pSzQuery)
{
  char source[136]{};
  char *buffer = pSzQuery;
  sprintf(pSzQuery, "UPDATE tbl_Buddy SET ");
  size_t initialLength = strlen_0(buffer);

  for (int index = 0; index < 50; ++index)
  {
    if (pNewData->dbBuddy.m_List[index].dwSerial != pOldData->dbBuddy.m_List[index].dwSerial)
    {
      sprintf(source, "Serial%d=%d,", index, pNewData->dbBuddy.m_List[index].dwSerial);
      strcat_0(buffer, source);
    }
  }

  size_t queryLength = strlen_0(buffer);
  if (queryLength <= initialLength)
  {
    memset_0(buffer, 0, initialLength);
  }
  else
  {
    sprintf(source, "WHERE Serial=%d", dwSerial);
    size_t length = strlen_0(buffer);
    buffer[length - 1] = 32;
    strcat_0(buffer, source);
  }
  return 1;
}

char CMainThread::_db_Update_ItemCombineEx(
  unsigned int dwSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData,
  char *pSzQuery)
{
  if (pNewData->dbItemCombineEx.m_bIsResult)
  {
    char buffer[132]{};
    sprintf(pSzQuery, "UPDATE tbl_itemcombine_ex_result SET ");
    unsigned int initialLength = static_cast<unsigned int>(strlen_0(pSzQuery));
    if (pNewData->dbItemCombineEx.m_dwCheckKey != pOldData->dbItemCombineEx.m_dwCheckKey)
    {
      sprintf(
        buffer,
        "IsResult=%d,Dalant=%d,CheckKey=%d,DlgType=%d,ItemSelectCount=%d,ItemListCount=%d,",
        pNewData->dbItemCombineEx.m_bIsResult,
        pNewData->dbItemCombineEx.m_dwDalant,
        pNewData->dbItemCombineEx.m_dwCheckKey,
        pNewData->dbItemCombineEx.m_byDlgType,
        pNewData->dbItemCombineEx.m_bySelectItemCount,
        pNewData->dbItemCombineEx.m_byItemListNum);
      strcat_0(pSzQuery, buffer);
      int listCount = pNewData->dbItemCombineEx.m_byItemListNum;
      for (int index = 0; index < listCount; ++index)
      {
        int dbKey = pNewData->dbItemCombineEx.m_List[index].Key.CovDBKey();
        sprintf(
          buffer,
          "K%d=%d,D%d=%d,U%d=%d,",
          index,
          dbKey,
          index,
          pNewData->dbItemCombineEx.m_List[index].dwDur,
          index,
          pNewData->dbItemCombineEx.m_List[index].dwUpt);
        strcat_0(pSzQuery, buffer);
      }
      sprintf(buffer, "resulteffecttype=%d,", pNewData->dbItemCombineEx.m_dwResultEffectType);
      strcat_0(pSzQuery, buffer);
      sprintf(buffer, "resulteffectmsgcode=%d,", pNewData->dbItemCombineEx.m_dwResultEffectMsgCode);
      strcat_0(pSzQuery, buffer);
    }

    size_t queryLength = strlen_0(pSzQuery);
    if (queryLength <= initialLength)
    {
      memset_0(pSzQuery, 0, initialLength);
    }
    else
    {
      sprintf(buffer, "WHERE Serial=%d", dwSerial);
      pSzQuery[strlen_0(pSzQuery) - 1] = 32;
      strcat_0(pSzQuery, buffer);
    }
  }
  else if (pNewData->dbItemCombineEx.m_bIsResult != pOldData->dbItemCombineEx.m_bIsResult)
  {
    sprintf(pSzQuery, "{ CALL pUpdate_CombineEx_Result_Pop( %d ) }", dwSerial);
  }

  return 1;
}

char CMainThread::_db_Update_Trunk(
  unsigned int dwAccountSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData,
  char *pSzQuery)
{
  char source[136]{};
  char *buffer = pSzQuery;
  sprintf(pSzQuery, "UPDATE tbl_AccountTrunk SET ");
  unsigned int initialLength = static_cast<unsigned int>(strlen_0(buffer));
  unsigned __int8 slotCount = pNewData->dbTrunk.bySlotNum;
  unsigned __int8 raceIndex = pNewData->dbAvator.m_byRaceSexCode / 2;

  if (strcmp_0(pNewData->dbTrunk.wszPasswd, pOldData->dbTrunk.wszPasswd))
  {
    sprintf(source, "TrunkPass=cast('%s' as binary),", pNewData->dbTrunk.wszPasswd);
    strcat_0(buffer, source);
  }
  if (pNewData->dbTrunk.dDalant != pOldData->dbTrunk.dDalant)
  {
    sprintf(source, "Dalant%d=%f,", raceIndex, static_cast<double>(pNewData->dbTrunk.dDalant));
    strcat_0(buffer, source);
  }
  if (pNewData->dbTrunk.dGold != pOldData->dbTrunk.dGold)
  {
    sprintf(source, "Gold%d=%f,", raceIndex, static_cast<double>(pNewData->dbTrunk.dGold));
    strcat_0(buffer, source);
  }
  if (pNewData->dbTrunk.byHintIndex != pOldData->dbTrunk.byHintIndex)
  {
    sprintf(source, "HintIndex=%d,", pNewData->dbTrunk.byHintIndex);
    strcat_0(buffer, source);
  }
  if (strcmp_0(pNewData->dbTrunk.wszHintAnswer, pOldData->dbTrunk.wszHintAnswer))
  {
    sprintf(source, "HintAnswer='%s',", pNewData->dbTrunk.wszHintAnswer);
    strcat_0(buffer, source);
  }
  if (pNewData->dbTrunk.bySlotNum != pOldData->dbTrunk.bySlotNum)
  {
    sprintf(source, "EstSlot=%d,", pNewData->dbTrunk.bySlotNum);
    strcat_0(buffer, source);
  }

  for (int index = 0; index < slotCount; ++index)
  {
    if (pNewData->dbTrunk.m_List[index].Key.IsFilled())
    {
      if (pOldData->dbTrunk.m_List[index].Key.IsFilled())
      {
        int newKey = pNewData->dbTrunk.m_List[index].Key.CovDBKey();
        int oldKey = pOldData->dbTrunk.m_List[index].Key.CovDBKey();
        if (newKey != oldKey)
        {
          int dbKey = pNewData->dbTrunk.m_List[index].Key.CovDBKey();
          sprintf(source, "K%d=%d,", index, dbKey);
          strcat_0(buffer, source);
        }
        if (pNewData->dbTrunk.m_List[index].dwDur != pOldData->dbTrunk.m_List[index].dwDur)
        {
          sprintf(source, "D%d=%I64d,", index, pNewData->dbTrunk.m_List[index].dwDur);
          strcat_0(buffer, source);
        }
        if (pNewData->dbTrunk.m_List[index].dwUpt != pOldData->dbTrunk.m_List[index].dwUpt)
        {
          sprintf(source, "U%d=%d,", index, pNewData->dbTrunk.m_List[index].dwUpt);
          strcat_0(buffer, source);
        }
        if (pNewData->dbTrunk.m_List[index].byRace != pOldData->dbTrunk.m_List[index].byRace)
        {
          sprintf(source, "R%d=%d,", index, pNewData->dbTrunk.m_List[index].byRace);
          strcat_0(buffer, source);
        }
        if (pNewData->dbTrunk.m_List[index].lnUID != pOldData->dbTrunk.m_List[index].lnUID)
        {
          sprintf(source, "S%d=%I64d,", index, pNewData->dbTrunk.m_List[index].lnUID);
          strcat_0(buffer, source);
        }
        if (pNewData->dbTrunk.m_List[index].byCsMethod)
        {
          unsigned int newET = 0;
          unsigned int oldET = 0;
          if (pNewData->dbTrunk.m_List[index].byCsMethod == 1)
          {
            newET = pNewData->dbTrunk.m_List[index].dwT - g_Main.m_tmDbUpdate;
            oldET = pOldData->dbTrunk.m_List[index].dwT - g_Main.m_tmDbUpdate;
          }
          else if (pNewData->dbTrunk.m_List[index].byCsMethod == 2)
          {
            newET = pNewData->dbTrunk.m_List[index].dwT;
            oldET = pOldData->dbTrunk.m_List[index].dwT;
          }
          if (newET != oldET)
          {
            sprintf(source, "T%d=%d,", index, newET);
            strcat_0(buffer, source);
          }
        }
      }
      else
      {
        unsigned int newET = 0;
        if (pNewData->dbTrunk.m_List[index].byCsMethod == 1)
        {
          newET = pNewData->dbTrunk.m_List[index].dwT - g_Main.m_tmDbUpdate;
        }
        else if (pNewData->dbTrunk.m_List[index].byCsMethod == 2)
        {
          newET = pNewData->dbTrunk.m_List[index].dwT;
        }
        int dbKey = pNewData->dbTrunk.m_List[index].Key.CovDBKey();
        sprintf(
          source,
          "K%d=%d,D%d=%I64d,U%d=%d,R%d=%d,S%d=%I64d,T%d=%d,",
          index,
          dbKey,
          index,
          pNewData->dbTrunk.m_List[index].dwDur,
          index,
          pNewData->dbTrunk.m_List[index].dwUpt,
          index,
          pNewData->dbTrunk.m_List[index].byRace,
          index,
          pNewData->dbTrunk.m_List[index].lnUID,
          index,
          newET);
        strcat_0(buffer, source);
      }
    }
    else if (pOldData->dbTrunk.m_List[index].Key.IsFilled())
    {
      int dbKey = pNewData->dbTrunk.m_List[index].Key.CovDBKey();
      sprintf(source, "K%d=%d,", index, dbKey);
      strcat_0(buffer, source);
    }
  }

  size_t queryLength = strlen_0(buffer);
  if (queryLength <= initialLength)
  {
    memset_0(buffer, 0, initialLength);
  }
  else
  {
    sprintf(source, "WHERE AccountSerial=%d", dwAccountSerial);
    size_t length = strlen_0(buffer);
    buffer[length - 1] = 32;
    strcat_0(buffer, source);
  }
  return 1;
}

char CMainThread::_db_Update_Trunk_Extend(
  unsigned int dwAccountSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData,
  char *pSzQuery)
{
  char source[136]{};
  char *buffer = pSzQuery;
  sprintf(pSzQuery, "UPDATE tbl_AccountTrunk_Extend SET ");
  unsigned int initialLength = static_cast<unsigned int>(strlen_0(buffer));
  unsigned __int8 slotCount = pNewData->dbTrunk.byExtSlotNum;

  if (pNewData->dbTrunk.byExtSlotNum != pOldData->dbTrunk.byExtSlotNum)
  {
    sprintf(source, "EstSlot=%d,", pNewData->dbTrunk.byExtSlotNum);
    strcat_0(buffer, source);
  }

  for (int index = 0; index < slotCount; ++index)
  {
    if (pNewData->dbTrunk.m_ExtList[index].Key.IsFilled())
    {
      if (pOldData->dbTrunk.m_ExtList[index].Key.IsFilled())
      {
        int newKey = pNewData->dbTrunk.m_ExtList[index].Key.CovDBKey();
        int oldKey = pOldData->dbTrunk.m_ExtList[index].Key.CovDBKey();
        if (newKey != oldKey)
        {
          int dbKey = pNewData->dbTrunk.m_ExtList[index].Key.CovDBKey();
          sprintf(source, "K%d=%d,", index, dbKey);
          strcat_0(buffer, source);
        }
        if (pNewData->dbTrunk.m_ExtList[index].dwDur != pOldData->dbTrunk.m_ExtList[index].dwDur)
        {
          sprintf(source, "D%d=%I64d,", index, pNewData->dbTrunk.m_ExtList[index].dwDur);
          strcat_0(buffer, source);
        }
        if (pNewData->dbTrunk.m_ExtList[index].dwUpt != pOldData->dbTrunk.m_ExtList[index].dwUpt)
        {
          sprintf(source, "U%d=%d,", index, pNewData->dbTrunk.m_ExtList[index].dwUpt);
          strcat_0(buffer, source);
        }
        if (pNewData->dbTrunk.m_ExtList[index].byRace != pOldData->dbTrunk.m_ExtList[index].byRace)
        {
          sprintf(source, "R%d=%d,", index, pNewData->dbTrunk.m_ExtList[index].byRace);
          strcat_0(buffer, source);
        }
        if (pNewData->dbTrunk.m_ExtList[index].lnUID != pOldData->dbTrunk.m_ExtList[index].lnUID)
        {
          sprintf(source, "S%d=%I64d,", index, pNewData->dbTrunk.m_ExtList[index].lnUID);
          strcat_0(buffer, source);
        }
        if (pNewData->dbTrunk.m_ExtList[index].byCsMethod)
        {
          unsigned int newET = 0;
          unsigned int oldET = 0;
          if (pNewData->dbTrunk.m_ExtList[index].byCsMethod == 1)
          {
            newET = pNewData->dbTrunk.m_ExtList[index].dwT - g_Main.m_tmDbUpdate;
            oldET = pOldData->dbTrunk.m_ExtList[index].dwT - g_Main.m_tmDbUpdate;
          }
          else if (pNewData->dbTrunk.m_ExtList[index].byCsMethod == 2)
          {
            newET = pNewData->dbTrunk.m_ExtList[index].dwT;
            oldET = pOldData->dbTrunk.m_ExtList[index].dwT;
          }
          if (newET != oldET)
          {
            sprintf(source, "T%d=%d,", index, newET);
            strcat_0(buffer, source);
          }
        }
      }
      else
      {
        unsigned int newET = 0;
        if (pNewData->dbTrunk.m_ExtList[index].byCsMethod == 1)
        {
          newET = pNewData->dbTrunk.m_ExtList[index].dwT - g_Main.m_tmDbUpdate;
        }
        else if (pNewData->dbTrunk.m_ExtList[index].byCsMethod == 2)
        {
          newET = pNewData->dbTrunk.m_ExtList[index].dwT;
        }
        int dbKey = pNewData->dbTrunk.m_ExtList[index].Key.CovDBKey();
        sprintf(
          source,
          "K%d=%d,D%d=%I64d,U%d=%d,R%d=%d,S%d=%I64d,T%d=%d,",
          index,
          dbKey,
          index,
          pNewData->dbTrunk.m_ExtList[index].dwDur,
          index,
          pNewData->dbTrunk.m_ExtList[index].dwUpt,
          index,
          pNewData->dbTrunk.m_ExtList[index].byRace,
          index,
          pNewData->dbTrunk.m_ExtList[index].lnUID,
          index,
          newET);
        strcat_0(buffer, source);
      }
    }
    else if (pOldData->dbTrunk.m_ExtList[index].Key.IsFilled())
    {
      int dbKey = pNewData->dbTrunk.m_ExtList[index].Key.CovDBKey();
      sprintf(source, "K%d=%d,", index, dbKey);
      strcat_0(buffer, source);
    }
  }

  size_t queryLength = strlen_0(buffer);
  if (queryLength <= initialLength)
  {
    memset_0(buffer, 0, initialLength);
  }
  else
  {
    sprintf(source, "WHERE AccountSerial=%d", dwAccountSerial);
    size_t length = strlen_0(buffer);
    buffer[length - 1] = 32;
    strcat_0(buffer, source);
  }
  return 1;
}

unsigned __int8 CMainThread::_db_Update_TimeLimitInfo(
  unsigned int dwAccSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData,
  char *pSzQuery,
  int nBufferSize)
{
  char buffer[132]{};
  sprintf_s(pSzQuery, nBufferSize, "UPDATE tbl_TimeLimitInfo Set ");
  int initialLength = static_cast<int>(strlen_0(pSzQuery));

  if (pOldData->dbTimeLimitInfo.dwFatigue != pNewData->dbTimeLimitInfo.dwFatigue)
  {
    sprintf_s(buffer, 0x80u, "Fatigue=%d,", pNewData->dbTimeLimitInfo.dwFatigue);
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  if (pOldData->dbTimeLimitInfo.byTLStatus != pNewData->dbTimeLimitInfo.byTLStatus)
  {
    sprintf_s(buffer, 0x80u, "TLStatus = %u,", pNewData->dbTimeLimitInfo.byTLStatus);
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  unsigned int localTime = GetKorLocalTime();
  sprintf_s(buffer, 0x80u, "LastLogoutTime=%d,", localTime);
  strcat_s(pSzQuery, nBufferSize, buffer);

  size_t queryLength = strlen_0(pSzQuery);
  if (queryLength <= static_cast<size_t>(initialLength))
  {
    memset_0(pSzQuery, 0, initialLength);
  }
  else
  {
    sprintf_s(buffer, 0x80u, "WHERE AccountSerial = %d", dwAccSerial);
    pSzQuery[strlen_0(pSzQuery) - 1] = 32;
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  return 1;
}

char CMainThread::_db_update_inven_AMP(
  unsigned int dwSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData,
  char *pSzQuery)
{
  char source[10248]{};
  char *buffer = pSzQuery;
  sprintf(pSzQuery, "update [dbo].[tbl_aminepersonal_inven] set ");
  size_t initialLength = strlen_0(buffer);

  for (int index = 0; index < 40; ++index)
  {
    if (reinterpret_cast<_INVENKEY *>(&pNewData->dbPersonalAmineInven.m_List[index])->IsFilled())
    {
      if (reinterpret_cast<_INVENKEY *>(&pOldData->dbPersonalAmineInven.m_List[index])->IsFilled())
      {
        int newKey = reinterpret_cast<_INVENKEY *>(&pNewData->dbPersonalAmineInven.m_List[index])->CovDBKey();
        int oldKey = reinterpret_cast<_INVENKEY *>(&pOldData->dbPersonalAmineInven.m_List[index])->CovDBKey();
        if (newKey != oldKey)
        {
          int dbKey = reinterpret_cast<_INVENKEY *>(&pNewData->dbPersonalAmineInven.m_List[index])->CovDBKey();
          sprintf(source, "K%d=%d,", index, dbKey);
          strcat_0(buffer, source);
        }
        if (pNewData->dbPersonalAmineInven.m_List[index].dwDur
            != pOldData->dbPersonalAmineInven.m_List[index].dwDur)
        {
          sprintf(source, "N%d=%d,", index, pNewData->dbPersonalAmineInven.m_List[index].dwDur);
          strcat_0(buffer, source);
        }
      }
      else
      {
        int dbKey = reinterpret_cast<_INVENKEY *>(&pNewData->dbPersonalAmineInven.m_List[index])->CovDBKey();
        sprintf(
          source,
          "K%d=%d,N%d=%d,",
          index,
          dbKey,
          index,
          pNewData->dbPersonalAmineInven.m_List[index].dwDur);
        strcat_0(buffer, source);
      }
    }
    else if (reinterpret_cast<_INVENKEY *>(&pOldData->dbPersonalAmineInven.m_List[index])->IsFilled())
    {
      int dbKey = reinterpret_cast<_INVENKEY *>(&pNewData->dbPersonalAmineInven.m_List[index])->CovDBKey();
      sprintf(source, "K%d=%d,", index, dbKey);
      strcat_0(buffer, source);
    }
  }

  size_t queryLength = strlen_0(buffer);
  if (queryLength <= initialLength)
  {
    memset_0(buffer, 0, initialLength);
  }
  else
  {
    sprintf(source, "WHERE avatorserial=%d", dwSerial);
    size_t length = strlen_0(buffer);
    buffer[length - 1] = 32;
    strcat_0(buffer, source);
  }
  return 1;
}

char CMainThread::_db_Update_PvpPointLimit(
  unsigned int dwSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData,
  char *pSzQuery,
  char *pErrMsg)
{
  char buffer[132]{};
  sprintf(pSzQuery, "update [dbo].[tbl_pvppointlimitinfo] set ");
  int initialLength = static_cast<int>(strlen_0(pSzQuery));

  if (pNewData->dbPvpPointLimit.tUpdatedate != pOldData->dbPvpPointLimit.tUpdatedate)
  {
    tm *localTime = localtime_2(&pNewData->dbPvpPointLimit.tUpdatedate);
    if (!localTime)
    {
      sprintf(
        pErrMsg,
        "localtime( dbPvpPointLimit.tUpdatedate(%d) ) NULL!",
        static_cast<int>(pNewData->dbPvpPointLimit.tUpdatedate));
      return 0;
    }
    sprintf(
      buffer,
      "[updatedate]='%04u-%02u-%02u %02u:%02u:%02u',",
      localTime->tm_year + 1900,
      localTime->tm_mon + 1,
      localTime->tm_mday,
      localTime->tm_hour,
      localTime->tm_min,
      localTime->tm_sec);
    strcat_0(pSzQuery, buffer);
  }
  if (pNewData->dbPvpPointLimit.bUseUp != pOldData->dbPvpPointLimit.bUseUp)
  {
    sprintf(buffer, "[useup]=%u,", pNewData->dbPvpPointLimit.bUseUp);
    strcat_0(pSzQuery, buffer);
  }
  if (pNewData->dbPvpPointLimit.byLimitRate != pOldData->dbPvpPointLimit.byLimitRate)
  {
    sprintf(buffer, "[limitrate]=%u,", pNewData->dbPvpPointLimit.byLimitRate);
    strcat_0(pSzQuery, buffer);
  }
  if (pNewData->dbPvpPointLimit.dOriginalPoint != pOldData->dbPvpPointLimit.dOriginalPoint)
  {
    sprintf(buffer, "[originalpoint]=%.10f,", static_cast<double>(pNewData->dbPvpPointLimit.dOriginalPoint));
    strcat_0(pSzQuery, buffer);
  }
  if (pNewData->dbPvpPointLimit.dLimitPoint != pOldData->dbPvpPointLimit.dLimitPoint)
  {
    sprintf(buffer, "[limitpoint]=%.10f,", static_cast<double>(pNewData->dbPvpPointLimit.dLimitPoint));
    strcat_0(pSzQuery, buffer);
  }
  if (pNewData->dbPvpPointLimit.dUsePoint != pOldData->dbPvpPointLimit.dUsePoint)
  {
    sprintf(buffer, "[usepoint]=%.10f ", static_cast<double>(pNewData->dbPvpPointLimit.dUsePoint));
    strcat_0(pSzQuery, buffer);
  }

  size_t queryLength = strlen_0(pSzQuery);
  if (queryLength <= static_cast<size_t>(initialLength))
  {
    memset_0(pSzQuery, 0, initialLength);
  }
  else
  {
    sprintf(buffer, "where serial=%d", dwSerial);
    pSzQuery[strlen_0(pSzQuery) - 1] = 32;
    strcat_0(pSzQuery, buffer);
  }
  return 1;
}

char CMainThread::_db_Update_CryMsg(
  unsigned int dwSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData,
  char *pSzQuery)
{
  char source[520]{};
  char *buffer = pSzQuery;
  sprintf(pSzQuery, "UPDATE tbl_CryMsg SET ");
  size_t initialLength = strlen_0(buffer);

  for (int index = 0; index < 10; ++index)
  {
    if (strcmp_0(pNewData->dbBossCry.m_List[index].wszCryMsg, pOldData->dbBossCry.m_List[index].wszCryMsg))
    {
      sprintf(source, "CryMsg%d='%s',", index + 1, pNewData->dbBossCry.m_List[index].wszCryMsg);
      strcat_0(buffer, source);
    }
  }

  size_t queryLength = strlen_0(buffer);
  if (queryLength <= initialLength)
  {
    memset_0(buffer, 0, initialLength);
  }
  else
  {
    sprintf(source, "WHERE Serial=%d", dwSerial);
    size_t length = strlen_0(buffer);
    buffer[length - 1] = 32;
    strcat_0(buffer, source);
  }
  return 1;
}

char CMainThread::_db_Update_PvpOrderView(
  unsigned int dwSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData,
  char *pSzQuery,
  char *pErrMsg)
{
  char buffer[132]{};
  sprintf(pSzQuery, "update [dbo].[tbl_pvporderview] set ");
  int initialLength = static_cast<int>(strlen_0(pSzQuery));

  if (pNewData->dbPvpOrderView.tUpdatedate != pOldData->dbPvpOrderView.tUpdatedate)
  {
    tm *localTime = localtime_2(&pNewData->dbPvpOrderView.tUpdatedate);
    if (!localTime)
    {
      sprintf(
        pErrMsg,
        "localtime( dbPvpOrderView.tUpdatedate(%d) ) NULL!",
        static_cast<int>(pNewData->dbPvpOrderView.tUpdatedate));
      return 0;
    }
    sprintf(
      buffer,
      "[UpdateDate]='%04u-%02u-%02u %02u:%02u:%02u',",
      localTime->tm_year + 1900,
      localTime->tm_mon + 1,
      localTime->tm_mday,
      localTime->tm_hour,
      localTime->tm_min,
      localTime->tm_sec);
    strcat_0(pSzQuery, buffer);
  }
  if (pNewData->dbPvpOrderView.nDeath != pOldData->dbPvpOrderView.nDeath)
  {
    sprintf(buffer, "[Death]= %d,", pNewData->dbPvpOrderView.nDeath);
    strcat_0(pSzQuery, buffer);
  }
  if (pNewData->dbPvpOrderView.nKill != pOldData->dbPvpOrderView.nKill)
  {
    sprintf(buffer, "[Kill]= %d,", pNewData->dbPvpOrderView.nKill);
    strcat_0(pSzQuery, buffer);
  }
  if (pNewData->dbPvpOrderView.dTodayStacked != pOldData->dbPvpOrderView.dTodayStacked)
  {
    sprintf(buffer, "[TodayStacked]= %.10f,", static_cast<double>(pNewData->dbPvpOrderView.dTodayStacked));
    strcat_0(pSzQuery, buffer);
  }
  if (pNewData->dbPvpOrderView.dPvpPoint != pOldData->dbPvpOrderView.dPvpPoint)
  {
    sprintf(buffer, "[PvpPoint]= %.10f,", static_cast<double>(pNewData->dbPvpOrderView.dPvpPoint));
    strcat_0(pSzQuery, buffer);
  }
  if (pNewData->dbPvpOrderView.dPvpTempCash != pOldData->dbPvpOrderView.dPvpTempCash)
  {
    sprintf(buffer, "[PvpTempCash]= %.10f,", static_cast<double>(pNewData->dbPvpOrderView.dPvpTempCash));
    strcat_0(pSzQuery, buffer);
  }
  if (pNewData->dbPvpOrderView.dPvpCash != pOldData->dbPvpOrderView.dPvpCash)
  {
    sprintf(buffer, "[PvpCash]= %.10f,", static_cast<double>(pNewData->dbPvpOrderView.dPvpCash));
    strcat_0(pSzQuery, buffer);
  }
  for (int index = 0; index < 10; ++index)
  {
    if (pNewData->dbPvpOrderView.dwKillerSerial[index] != pOldData->dbPvpOrderView.dwKillerSerial[index])
    {
      sprintf(buffer, "[KillerSerial%d] = %d,", index, pNewData->dbPvpOrderView.dwKillerSerial[index]);
      strcat_0(pSzQuery, buffer);
    }
  }
  if (pNewData->dbPvpOrderView.byContHaveCash != pOldData->dbPvpOrderView.byContHaveCash)
  {
    sprintf(buffer, "[ContHaveCash] = %d,", pNewData->dbPvpOrderView.byContHaveCash);
    strcat_0(pSzQuery, buffer);
  }
  if (pNewData->dbPvpOrderView.byContLoseCash != pOldData->dbPvpOrderView.byContLoseCash)
  {
    sprintf(buffer, "[ContLoseCash] = %d,", pNewData->dbPvpOrderView.byContLoseCash);
    strcat_0(pSzQuery, buffer);
  }
  if (pNewData->dbPvpOrderView.bRaceWarRecvr != pOldData->dbPvpOrderView.bRaceWarRecvr)
  {
    sprintf(buffer, "[RaceWarRecvr] = %d ", pNewData->dbPvpOrderView.bRaceWarRecvr);
    strcat_0(pSzQuery, buffer);
  }

  size_t queryLength = strlen_0(pSzQuery);
  if (queryLength <= static_cast<size_t>(initialLength))
  {
    memset_0(pSzQuery, 0, initialLength);
  }
  else
  {
    sprintf(buffer, "where serial=%d", dwSerial);
    pSzQuery[strlen_0(pSzQuery) - 1] = 32;
    strcat_0(pSzQuery, buffer);
  }
  return 1;
}

char CMainThread::_db_Update_PrimiumPlayTime(
  unsigned int dwAccSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData,
  char *pSzQuery,
  char *pErrMsg)
{
  static_cast<void>(pErrMsg);

  char buffer[132]{};
  sprintf(pSzQuery, "update [dbo].[tbl_PrimiumPlayTime] set ");
  unsigned int initialLength = static_cast<unsigned int>(strlen_0(pSzQuery));

  if (pNewData->dbPlayTimeInPcbang.dwLastConnTime != pOldData->dbPlayTimeInPcbang.dwLastConnTime)
  {
    sprintf(buffer, "[LastConnTime]= %d,", pNewData->dbPlayTimeInPcbang.dwLastConnTime);
    strcat_0(pSzQuery, buffer);
  }
  if (pNewData->dbPlayTimeInPcbang.dwContPlayTime != pOldData->dbPlayTimeInPcbang.dwContPlayTime)
  {
    sprintf(buffer, "[ContPlayTime]= %d,", pNewData->dbPlayTimeInPcbang.dwContPlayTime);
    strcat_0(pSzQuery, buffer);
  }
  if (pNewData->dbPlayTimeInPcbang.bForcedClose != pOldData->dbPlayTimeInPcbang.bForcedClose)
  {
    sprintf(buffer, "[ForcedClose]= %d,", pNewData->dbPlayTimeInPcbang.bForcedClose);
    strcat_0(pSzQuery, buffer);
  }
  if (pNewData->dbPlayTimeInPcbang.byReceiveCoupon != pOldData->dbPlayTimeInPcbang.byReceiveCoupon)
  {
    sprintf(buffer, "[ReceiveCoupon]= %d,", pNewData->dbPlayTimeInPcbang.byReceiveCoupon);
    strcat_0(pSzQuery, buffer);
  }
  if (pNewData->dbPlayTimeInPcbang.byEnsureTime != pOldData->dbPlayTimeInPcbang.byEnsureTime)
  {
    sprintf(buffer, "[EnsureTime]= %d ", pNewData->dbPlayTimeInPcbang.byEnsureTime);
    strcat_0(pSzQuery, buffer);
  }

  size_t queryLength = strlen_0(pSzQuery);
  if (queryLength <= initialLength)
  {
    memset_0(pSzQuery, 0, initialLength);
  }
  else
  {
    sprintf(buffer, "where AccountSerial=%d", dwAccSerial);
    pSzQuery[strlen_0(pSzQuery) - 1] = 32;
    strcat_0(pSzQuery, buffer);
  }
  return 1;
}

char CMainThread::_db_Update_PotionDelay(
  unsigned int dwSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData,
  char *pSzQuery,
  int nBufferSize)
{
  static_cast<void>(pOldData);

  char buffer[132]{};
  sprintf_s(pSzQuery, nBufferSize, "UPDATE tbl_potion_delay Set ");
  int initialLength = static_cast<int>(strlen_0(pSzQuery));
  DWORD now = timeGetTime();

  for (int index = 0; index < 38; ++index)
  {
    if (CPotionMgr::IsPotionDelayUseIndex(&g_PotionMgr, index)
        && pNewData->dbPotionNextUseTime.dwPotionNextUseTime[index])
    {
      unsigned int delay = 0;
      if (pNewData->dbPotionNextUseTime.dwPotionNextUseTime[index] < now)
      {
        pNewData->dbPotionNextUseTime.dwPotionNextUseTime[index] = 0;
        delay = 0;
      }
      else
      {
        delay = pNewData->dbPotionNextUseTime.dwPotionNextUseTime[index] - now;
      }
      sprintf_s(buffer, 0x80u, "PD%d=%u,", index, delay);
      strcat_s(pSzQuery, nBufferSize, buffer);
    }
  }

  size_t queryLength = strlen_0(pSzQuery);
  if (queryLength <= static_cast<size_t>(initialLength))
  {
    memset_0(pSzQuery, 0, initialLength);
  }
  else
  {
    sprintf_s(buffer, 0x80u, "WHERE Serial = %d", dwSerial);
    pSzQuery[strlen_0(pSzQuery) - 1] = 32;
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  return 1;
}

char CMainThread::_db_Update_OreCutting(
  unsigned int dwSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData,
  char *pSzQuery,
  int nBufferSize)
{
  char buffer[132]{};
  sprintf_s(pSzQuery, nBufferSize, "UPDATE tbl_OreCutting Set ");
  int initialLength = static_cast<int>(strlen_0(pSzQuery));

  for (int index = 0; index < 20; ++index)
  {
    int newKey = reinterpret_cast<_INVENKEY *>(&pNewData->dbCutting.m_List[index])->CovDBKey();
    int oldKey = reinterpret_cast<_INVENKEY *>(&pOldData->dbCutting.m_List[index])->CovDBKey();
    if (newKey != oldKey)
    {
      sprintf_s(buffer, 0x80u, "K%d=%d,", index, newKey);
      strcat_s(pSzQuery, nBufferSize, buffer);
    }
    unsigned int newDur = pNewData->dbCutting.m_List[index].dwDur;
    if (newDur != pOldData->dbCutting.m_List[index].dwDur)
    {
      sprintf_s(buffer, 0x80u, "D%d=%d,", index, newDur);
      strcat_s(pSzQuery, nBufferSize, buffer);
    }
  }

  size_t queryLength = strlen_0(pSzQuery);
  if (queryLength <= static_cast<size_t>(initialLength))
  {
    memset_0(pSzQuery, 0, initialLength);
  }
  else
  {
    sprintf_s(buffer, 0x80u, "WHERE Serial = %d", dwSerial);
    pSzQuery[strlen_0(pSzQuery) - 1] = 32;
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  return 1;
}

char CMainThread::_db_Update_PcBangFavor(
  unsigned int dwSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData,
  char *pSzQuery,
  int nBufferSize)
{
  char buffer[132]{};
  sprintf_s(pSzQuery, nBufferSize, "UPDATE tbl_pcbangitem Set ");
  int initialLength = static_cast<int>(strlen_0(pSzQuery));

  for (int index = 0; index < 50; ++index)
  {
    if (pNewData->dbPcBangFavorItem.lnUID[index] != pOldData->dbPcBangFavorItem.lnUID[index])
    {
      sprintf(buffer, "K%d=%I64d,", index, pNewData->dbPcBangFavorItem.lnUID[index]);
      strcat_s(pSzQuery, nBufferSize, buffer);
    }
  }

  size_t queryLength = strlen_0(pSzQuery);
  if (queryLength <= static_cast<size_t>(initialLength))
  {
    memset_0(pSzQuery, 0, initialLength);
  }
  else
  {
    sprintf_s(buffer, 0x80u, "WHERE Serial = %d", dwSerial);
    pSzQuery[strlen_0(pSzQuery) - 1] = 32;
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  return 1;
}

// END

