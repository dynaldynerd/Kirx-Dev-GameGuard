#include "pch.h"

#include "CMainThread.h"

#include <cmath>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <mmsystem.h>
#include <vector>

#include "CCheckSumCharacAccountTrunkData.h"
#include "CPotionMgr.h"
#include "CRFWorldDatabase.h"
#include "DqsDbStructs.h"
#include "WorldServerUtil.h"

static constexpr size_t kDbQuerySize = 65536;
static constexpr size_t kNpcQuestQuerySize = 131072;
static constexpr size_t kErrorMsgSize = 2080;

char CMainThread::db_Update_Avator(
  unsigned int dwSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData,
  bool bCheckLowHigh,
  const unsigned __int64 *pdwCanonicalNewUnitCutTime,
  const unsigned __int64 *pdwCanonicalOldUnitCutTime)
{
  static constexpr size_t kQuerySize = 65536;
  static constexpr size_t kNpcQuestQuerySize = 131072;
  static constexpr size_t kAmpInvenQuerySize = 65536;
  static constexpr size_t kInvenQuerySize = 20000;

  std::vector<char> queryBase(kQuerySize, 0);
  std::vector<char> queryGeneral(kQuerySize, 0);
  std::vector<char> querySupplement(kQuerySize, 0);
  std::vector<char> queryUnit(kQuerySize, 0);
  std::vector<char> queryUI(kQuerySize, 0);
  std::vector<char> queryQuest(kQuerySize, 0);
  std::vector<char> queryNPCQuest(kNpcQuestQuerySize, 0);
  std::vector<char> queryBuddy(kQuerySize, 0);
  std::vector<char> queryItemCombineEx(kQuerySize, 0);
  std::vector<char> queryTrunk(kQuerySize, 0);
  std::vector<char> queryExtTrunk(kQuerySize, 0);
  std::vector<char> queryTimeLimitInfo(kQuerySize, 0);
  std::vector<char> queryAMPInven(kAmpInvenQuerySize, 0);
  std::vector<char> queryPvpPointLimit(kQuerySize, 0);
  std::vector<char> queryCryMsg(kQuerySize, 0);
  std::vector<char> queryPvpOrderView(kQuerySize, 0);
  std::vector<char> queryPrimiumPlayTime(kQuerySize, 0);
  std::vector<char> queryPotionDelay(kQuerySize, 0);
  std::vector<char> queryOreCutting(kQuerySize, 0);
  std::vector<char> queryPcBangFavor(kQuerySize, 0);

  char *pszBaseQuery = queryBase.data();
  char *pszGeneralQuery = queryGeneral.data();
  char *pszSupplementQuery = querySupplement.data();
  static char pszInvenQuery[kInvenQuerySize]{};
  std::memset(pszInvenQuery, 0, sizeof(pszInvenQuery));
  char *pszUnitQuery = queryUnit.data();
  char *pszUIQuery = queryUI.data();
  char *pszQuestQuery = queryQuest.data();
  char *pszNPCQuestQuery = queryNPCQuest.data();
  char *wszBuddyQuery = queryBuddy.data();
  char *pszItemCombineExQuery = queryItemCombineEx.data();
  char *wszTrunkQuery = queryTrunk.data();
  char *wszExtTrunkQuery = queryExtTrunk.data();
  char *pszTimeLimitInfoQuery = queryTimeLimitInfo.data();
  char *pszAMPInven = queryAMPInven.data();
  char *pszPvpPointLimitQuery = queryPvpPointLimit.data();
  char *pszCryMsgQuery = queryCryMsg.data();
  char *pszPvpOrderViewQurey = queryPvpOrderView.data();
  char *pszPrimiumPlayTimeQuery = queryPrimiumPlayTime.data();
  char *pszPotionDelayQuery = queryPotionDelay.data();
  char *pszOreCuttingQuery = queryOreCutting.data();
  char *pszPcBangFavorQuery = queryPcBangFavor.data();

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

  if (!_db_Update_Supplement(
        dwSerial,
        pNewData,
        pOldData,
        pszSupplementQuery,
        static_cast<int>(querySupplement.size())))
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
      && !_db_Update_Unit(
        dwSerial,
        pNewData,
        pOldData,
        pszUnitQuery,
        pdwCanonicalNewUnitCutTime,
        pdwCanonicalOldUnitCutTime))
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
        static_cast<int>(queryTimeLimitInfo.size())))
  {
    m_logSystemError.Write(
      "_db_Update_TimeLimitInfo(sr:%d) => _db_Update_TimeLimitInfo..failed ..",
      pNewData->dbAvator.m_dwAccountSerial);
    return 24;
  }

  // NOTE: This is intentionally against IDA parity.
  // IDA updates checksum rows unconditionally in db_Update_Avator, but we skip this when checksum is disabled.
  // Reason: with CheckSum=FALSE, load path skips checksum seeding, so unconditional update may fail with SQL_NO_DATA
  // on tbl_NpcData and block character save.
  if (m_bCheckSumActive)
  {
    CCheckSumCharacAccountTrunkData checkSum(
      dwSerial,
      pNewData->dbAvator.m_dwAccountSerial,
      static_cast<unsigned __int8>(pNewData->dbAvator.m_byRaceSexCode / 2));
    checkSum.Encode(pNewData);
    if (!checkSum.Update(m_pWorldDB))
    {
      m_logSystemError.Write( "_db_Update_NpcData(sr:%d) => _db_Update_NpcData..failed ..", dwSerial);
      return 24;
    }
  }

  if (pOldData->dbAvator.m_dwGuildSerial != static_cast<unsigned int>(-1)
      && pOldData->dbAvator.m_dwGuildSerial
      && pOldData->dbAvator.m_dPvPPoint != pNewData->dbAvator.m_dPvPPoint)
  {
    long double dPvpPoint = pNewData->dbAvator.m_dPvPPoint - pOldData->dbAvator.m_dPvPPoint;
    if (!m_pWorldDB->Update_IncreaseWeeklyGuildKillPvpPointSum(pOldData->dbAvator.m_dwGuildSerial,
          dPvpPoint))
    {
      m_logSystemError.Write(
        "Update_IncreaseWeeklyGuildKillPvpPointSum(%u,%f) failed!",
        pNewData->dbAvator.m_dwGuildSerial,
        dPvpPoint);
    }
  }

  if (pNewData->m_byHSKTime <= 2 && pNewData->m_byCristalBattleDBInfo != 3)
  {
    if (!m_pWorldDB->Update_CristalBattleCharInfo(dwSerial,
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
        && !m_pWorldDB->update_cristalbattle_date(dwSerial, pNewData->m_byHSKTime))
    {
      if (m_pWorldDB->Insert_Supplement(dwSerial))
      {
        if (!m_pWorldDB->update_cristalbattle_date(dwSerial, pNewData->m_byHSKTime))
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
      && m_pWorldDB->select_amine_personal(dwSerial) == 2
      && !m_pWorldDB->insert_amine_personal(dwSerial))
  {
    m_logSystemError.Write(
      "db_Update_Avator() : insert_amine_personal(sr:%d) failed call",
      dwSerial);
  }

  if (!_db_update_inven_AMP(dwSerial, pNewData, pOldData, pszAMPInven))
  {
    m_logSystemError.Write( "db_Update_Avator() : _db_Update_inven_AMP(sr:%d) Failed call", dwSerial);
  }

  std::memset(szError, 0, 2048);

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

  if (!_db_Update_PotionDelay(
        dwSerial,
        pNewData,
        pOldData,
        pszPotionDelayQuery,
        static_cast<int>(queryPotionDelay.size())))
  {
    m_logSystemError.Write( "_db_Update_PotionDelay(sr:%d) => _db_Update_PotionDelay..failed ..", dwSerial);
    return 24;
  }

  if (!_db_Update_OreCutting(
        dwSerial,
        pNewData,
        pOldData,
        pszOreCuttingQuery,
        static_cast<int>(queryOreCutting.size())))
  {
    m_logSystemError.Write( "_db_Update_OreCutting(sr:%d) => _db_Update_OreCutting..failed ..", dwSerial);
    return 24;
  }

  if (!_db_Update_PcBangFavor(
        dwSerial,
        pNewData,
        pOldData,
        pszPcBangFavorQuery,
        static_cast<int>(queryPcBangFavor.size())))
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

  if (m_pWorldDB->Update_CharacterData(dwSerial, &updateQuery))
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
      sprintf_s(buffer, 2048, "update tbl_PostStorage set dck=1 where ");
      if (newDeletePost->m_List[storageIndex].dwDelSerial)
      {
        sprintf_s(source, 128, "serial=%d", newDeletePost->m_List[storageIndex].dwDelSerial);
      }
      else
      {
        sprintf_s(
          source,
          128,
          "sindex=%d and owner=%d",
          newDeletePost->m_List[storageIndex].nStorageIndex,
          dwAvatorSerial);
      }
      strcat_s(buffer, 2048, source);
      m_pWorldDB->Update_Post(buffer);
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
            if (!m_pWorldDB->Select_PostStorageRecordCheck())
            {
              m_pWorldDB->Insert_PostStorageRecord();
            }
            if (m_pWorldDB->Select_PostStorageEmptyRecordSerial(&postEntry->dwPSSerial))
            {
              unsigned __int8 numberList[28]{};
              numberList[0] = static_cast<unsigned __int8>(postEntry->nNumber);
              bool updated = g_Main.m_pWorldDB->Update_PostStorageSendToRecver(dwAvatorSerial,
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
              sprintf_s(buffer, 2048, "update tbl_PostStorage set ");
              if (postEntry->byState != oldEntry->byState)
              {
                sprintf_s(source, 128, "poststate=%d,", postEntry->byState);
                strcat_s(buffer, 2048, source);
                changed = true;
              }
              if (postEntry->nNumber != oldEntry->nNumber)
              {
                sprintf_s(source, 128, "postinx=%d,", postEntry->nNumber);
                strcat_s(buffer, 2048, source);
                changed = true;
              }
              if (postEntry->nKey != oldEntry->nKey)
              {
                sprintf_s(source, 128, "k=%d,", postEntry->nKey);
                strcat_s(buffer, 2048, source);
                changed = true;
              }
              if (postEntry->dwDur != oldEntry->dwDur)
              {
                sprintf_s(source, 128, "d=%d,", static_cast<unsigned int>(postEntry->dwDur));
                strcat_s(buffer, 2048, source);
                changed = true;
              }
              if (postEntry->dwUpt != oldEntry->dwUpt)
              {
                sprintf_s(source, 128, "u=%d,", postEntry->dwUpt);
                strcat_s(buffer, 2048, source);
                changed = true;
              }
              if (postEntry->lnUID != oldEntry->lnUID)
              {
                sprintf_s(source, 128, "uid=%I64d,", postEntry->lnUID);
                strcat_s(buffer, 2048, source);
                changed = true;
              }
              if (postEntry->dwGold != oldEntry->dwGold)
              {
                sprintf_s(source, 128, "gold=%d,", postEntry->dwGold);
                strcat_s(buffer, 2048, source);
                changed = true;
              }
              if (postEntry->bUpdateIndex)
              {
                sprintf_s(source, 128, "sindex=%d,", storageIndex);
                strcat_s(buffer, 2048, source);
                changed = true;
              }
              sprintf_s(source, 128, "where serial=%d", postEntry->dwPSSerial);
              buffer[std::strlen(buffer) - 1] = 32;
              strcat_s(buffer, 2048, source);
            }
            else
            {
              sprintf_s(
                buffer,
                2048,
                "update tbl_PostStorage set poststate=%d,postinx=%d,k=%d,d=%d,u=%d,gold=%d",
                postEntry->byState,
                postEntry->nNumber,
                postEntry->nKey,
                static_cast<unsigned int>(postEntry->dwDur),
                postEntry->dwUpt,
                postEntry->dwGold);
              sprintf_s(source, 128, ",uid=%I64d", postEntry->lnUID);
              strcat_s(buffer, 2048, source);
              sprintf_s(
                source,
                128,
                " where dck=0 and sindex=%d and owner=%u",
                storageIndex,
                dwAvatorSerial);
              strcat_s(buffer, 2048, source);
              changed = true;
            }
            if (changed)
            {
              bool updated = m_pWorldDB->Update_Post(buffer);
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
        2048,
        "update tbl_PostStorage set dck=1 where serial=%d",
        newReturnPost->m_RetSerials[storageIndex]);
      m_pWorldDB->Update_Post(buffer);
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
  sprintf_s(pSzQuery, kDbQuerySize, "UPDATE tbl_base SET ");

  if (pOldData->dbAvator.m_byLevel != pNewData->dbAvator.m_byLevel)
  {
    sprintf_s(source, sizeof(source), "Lv=%d,", pNewData->dbAvator.m_byLevel);
    std::strcat(buffer, source);
  }
  if (std::strcmp(pOldData->dbAvator.m_szClassCode, pNewData->dbAvator.m_szClassCode))
  {
    pNewData->dbAvator.m_szClassCode[4] = 0;
    sprintf_s(source, sizeof(source), "class='%s',", pNewData->dbAvator.m_szClassCode);
    std::strcat(buffer, source);
  }
  if (pOldData->dbAvator.m_dwDalant != pNewData->dbAvator.m_dwDalant)
  {
    sprintf_s(source, sizeof(source), "Dalant=%d,", pNewData->dbAvator.m_dwDalant);
    std::strcat(buffer, source);
  }
  if (pOldData->dbAvator.m_dwGold != pNewData->dbAvator.m_dwGold)
  {
    sprintf_s(source, sizeof(source), "Gold=%d,", pNewData->dbAvator.m_dwGold);
    std::strcat(buffer, source);
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
          sprintf_s(source, sizeof(source), "EK%d=%d,", index, dbKey);
          std::strcat(buffer, source);
        }
        if (pNewData->dbAvator.m_dwFixEquipLv[index] != pOldData->dbAvator.m_dwFixEquipLv[index])
        {
          sprintf_s(source, sizeof(source), "EU%d=%d,", index, pNewData->dbAvator.m_dwFixEquipLv[index]);
          std::strcat(buffer, source);
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
            sprintf_s(source, sizeof(source), "ET%d=%d,", index, newET);
            std::strcat(buffer, source);
          }
        }
        if (pNewData->dbAvator.m_lnUID[index] != pOldData->dbAvator.m_lnUID[index])
        {
          sprintf_s(source, sizeof(source), "ES%d=%I64d,", index, pNewData->dbAvator.m_lnUID[index]);
          std::strcat(buffer, source);
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
        sprintf_s(
          source,
          sizeof(source),
          "EK%d=%d,EU%d=%d,ET%d=%d,ES%d=%I64d,",
          index,
          dbKey,
          index,
          pNewData->dbAvator.m_dwFixEquipLv[index],
          index,
          newET,
          index,
          pNewData->dbAvator.m_lnUID[index]);
        std::strcat(buffer, source);
      }
    }
    else if (pOldData->dbAvator.m_EquipKey[index].IsFilled())
    {
      int dbKey = pNewData->dbAvator.m_EquipKey[index].CovDBKey();
      sprintf_s(source, sizeof(source), "EK%d=%d,", index, dbKey);
      std::strcat(buffer, source);
    }
  }

  sprintf_s(source, sizeof(source), "Slot=%d,", pNewData->dbAvator.m_bySlotIndex);
  std::strcat(buffer, source);

  unsigned __int64 localTime = GetKorLocalTime();
  sprintf_s(source, sizeof(source), "LastConnTime=%I64u WHERE Serial=%d", localTime, dwSerial);
  std::strcat(buffer, source);

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
  sprintf_s(pSzQuery, kDbQuerySize, "UPDATE tbl_general SET ");
  const size_t initialLength = std::strlen(buffer);

  // Coordinate dupe fix (non-IDA): sanitize each saved X/Y/Z component so a bad
  // value cannot poison tbl_general and roll back the whole character save.
  for (int index = 0; index < 3; ++index)
  {
    float &oldPos = pOldData->dbAvator.m_fStartPos[index];
    float &newPos = pNewData->dbAvator.m_fStartPos[index];
    if (!std::isfinite(oldPos))
    {
      oldPos = 0.0f;
    }
    if (!std::isfinite(newPos))
    {
      newPos = 1.0f;
    }
  }

  if (pOldData->dbAvator.m_dExp != pNewData->dbAvator.m_dExp)
  {
    sprintf_s(source, sizeof(source), "Exp=%f,", static_cast<double>(pNewData->dbAvator.m_dExp));
    std::strcat(buffer, source);
  }
  if (pOldData->dbAvator.m_dLossExp != pNewData->dbAvator.m_dLossExp)
  {
    sprintf_s(source, sizeof(source), "LossExp=%f,", static_cast<double>(pNewData->dbAvator.m_dLossExp));
    std::strcat(buffer, source);
  }
  for (int index = 0; index < 3; ++index)
  {
    if (pOldData->dbAvator.m_zClassHistory[index] != pNewData->dbAvator.m_zClassHistory[index])
    {
      sprintf_s(source, sizeof(source), "Class%d=%d,", index, pNewData->dbAvator.m_zClassHistory[index]);
      std::strcat(buffer, source);
    }
  }
  if (pOldData->dbAvator.m_dwClassInitCnt != pNewData->dbAvator.m_dwClassInitCnt)
  {
    sprintf_s(source, sizeof(source), "ClassInitCnt=%d,", pNewData->dbAvator.m_dwClassInitCnt);
    std::strcat(buffer, source);
  }
  if (pOldData->dbAvator.m_byLastClassGrade != pNewData->dbAvator.m_byLastClassGrade)
  {
    sprintf_s(source, sizeof(source), "LastClassGrade=%d,", pNewData->dbAvator.m_byLastClassGrade);
    std::strcat(buffer, source);
  }
  if (pOldData->dbAvator.m_dwHP != pNewData->dbAvator.m_dwHP)
  {
    sprintf_s(source, sizeof(source), "HP=%d,", pNewData->dbAvator.m_dwHP);
    std::strcat(buffer, source);
  }
  if (pOldData->dbAvator.m_dwFP != pNewData->dbAvator.m_dwFP)
  {
    sprintf_s(source, sizeof(source), "FP=%d,", pNewData->dbAvator.m_dwFP);
    std::strcat(buffer, source);
  }
  if (pOldData->dbAvator.m_dwSP != pNewData->dbAvator.m_dwSP)
  {
    sprintf_s(source, sizeof(source), "SP=%d,", pNewData->dbAvator.m_dwSP);
    std::strcat(buffer, source);
  }
  if (pOldData->dbAvator.m_dwDP != pNewData->dbAvator.m_dwDP)
  {
    sprintf_s(source, sizeof(source), "DP=%d,", pNewData->dbAvator.m_dwDP);
    std::strcat(buffer, source);
  }
  if (pOldData->dbAvator.m_byMapCode != pNewData->dbAvator.m_byMapCode)
  {
    sprintf_s(source, sizeof(source), "Map=%d,", pNewData->dbAvator.m_byMapCode);
    std::strcat(buffer, source);
  }
  if (pOldData->dbAvator.m_fStartPos[0] != pNewData->dbAvator.m_fStartPos[0])
  {
    sprintf_s(source, sizeof(source), "X=%f,", pNewData->dbAvator.m_fStartPos[0]);
    std::strcat(buffer, source);
  }
  if (pOldData->dbAvator.m_fStartPos[1] != pNewData->dbAvator.m_fStartPos[1])
  {
    sprintf_s(source, sizeof(source), "Y=%f,", pNewData->dbAvator.m_fStartPos[1]);
    std::strcat(buffer, source);
  }
  if (pOldData->dbAvator.m_fStartPos[2] != pNewData->dbAvator.m_fStartPos[2])
  {
    sprintf_s(source, sizeof(source), "Z=%f,", pNewData->dbAvator.m_fStartPos[2]);
    std::strcat(buffer, source);
  }
  if (pOldData->dbAvator.m_byBagNum != pNewData->dbAvator.m_byBagNum)
  {
    sprintf_s(source, sizeof(source), "BagNum=%d,", pNewData->dbAvator.m_byBagNum);
    std::strcat(buffer, source);
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
          sprintf_s(source, sizeof(source), "EK%d=%d,", index, dbKey);
          std::strcat(buffer, source);
        }
        if (pNewData->dbEquip.m_EmbellishList[index].wAmount
            != pOldData->dbEquip.m_EmbellishList[index].wAmount)
        {
          if (pNewData->dbEquip.m_EmbellishList[index].wAmount == static_cast<unsigned __int16>(-1))
          {
            sprintf_s(source, sizeof(source), "ED%d=-1,", index);
          }
          else
          {
            sprintf_s(
              source,
              sizeof(source),
              "ED%d=%d,",
              index,
              static_cast<__int16>(pNewData->dbEquip.m_EmbellishList[index].wAmount));
          }
          std::strcat(buffer, source);
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
            sprintf_s(source, sizeof(source), "ET%d=%d,", index, newET);
            std::strcat(buffer, source);
          }
        }
        if (pNewData->dbEquip.m_EmbellishList[index].lnUID
            != pOldData->dbEquip.m_EmbellishList[index].lnUID)
        {
          sprintf_s(
            source,
            sizeof(source),
            "ES%d=%I64d,",
            index,
            pNewData->dbEquip.m_EmbellishList[index].lnUID);
          std::strcat(buffer, source);
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

        if (pNewData->dbEquip.m_EmbellishList[index].wAmount == static_cast<unsigned __int16>(-1))
        {
          int dbKey = pNewData->dbEquip.m_EmbellishList[index].Key.CovDBKey();
          sprintf_s(
            source,
            sizeof(source),
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
          sprintf_s(
            source,
            sizeof(source),
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
        std::strcat(buffer, source);
      }
    }
    else if (pOldData->dbEquip.m_EmbellishList[index].Key.IsFilled())
    {
      int dbKey = pNewData->dbEquip.m_EmbellishList[index].Key.CovDBKey();
      sprintf_s(source, sizeof(source), "EK%d=%d,", index, dbKey);
      std::strcat(buffer, source);
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
          sprintf_s(source, sizeof(source), "F%d=%d,", index, dbKey);
          std::strcat(buffer, source);
        }
        if (pNewData->dbForce.m_List[index].lnUID != pOldData->dbForce.m_List[index].lnUID)
        {
          sprintf_s(
            source,
            sizeof(source),
            "FS%d=%I64d,",
            index,
            pNewData->dbForce.m_List[index].lnUID);
          std::strcat(buffer, source);
        }
      }
      else
      {
        int dbKey = pNewData->dbForce.m_List[index].Key.CovDBKey();
        sprintf_s(
          source,
          sizeof(source),
          "F%d=%d,FS%d=%I64d,",
          index,
          dbKey,
          index,
          pNewData->dbForce.m_List[index].lnUID);
        std::strcat(buffer, source);
      }
    }
    else if (pOldData->dbForce.m_List[index].Key.IsFilled())
    {
      int dbKey = pNewData->dbForce.m_List[index].Key.CovDBKey();
      sprintf_s(source, sizeof(source), "F%d=%d,", index, dbKey);
      std::strcat(buffer, source);
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
            sprintf_s(
              source,
              sizeof(source),
              "AK%d=%d,AS%d=%I64d,",
              index,
              dbKey,
              index,
              pNewData->dbAnimus.m_List[index].lnUID);
            std::strcat(buffer, source);
          }
          if (pNewData->dbAnimus.m_List[index].dwExp != pOldData->dbAnimus.m_List[index].dwExp)
          {
            sprintf_s(
              source,
              sizeof(source),
              "AD%d=%I64d,",
              index,
              pNewData->dbAnimus.m_List[index].dwExp);
            std::strcat(buffer, source);
          }
          if (pNewData->dbAnimus.m_List[index].dwParam != pOldData->dbAnimus.m_List[index].dwParam)
          {
            sprintf_s(
              source,
              sizeof(source),
              "AP%d=%d,",
              index,
              pNewData->dbAnimus.m_List[index].dwParam);
            std::strcat(buffer, source);
          }
        }
        else
        {
          unsigned __int8 dbKey = pNewData->dbAnimus.m_List[index].Key.CovDBKey();
          sprintf_s(
            source,
            sizeof(source),
            "AK%d=%d,AD%d=%I64d,AP%d=%d,AS%d=%I64d,",
            index,
            dbKey,
            index,
            pNewData->dbAnimus.m_List[index].dwExp,
            index,
            pNewData->dbAnimus.m_List[index].dwParam,
            index,
            pNewData->dbAnimus.m_List[index].lnUID);
          std::strcat(buffer, source);
        }
      }
      else if (pOldData->dbAnimus.m_List[index].Key.IsFilled())
      {
        unsigned __int8 dbKey = pNewData->dbAnimus.m_List[index].Key.CovDBKey();
        sprintf_s(source, sizeof(source), "AK%d=%d,", index, dbKey);
        std::strcat(buffer, source);
      }
    }
  }

  for (int index = 0; index < 2; ++index)
  {
    if (pOldData->dbStat.m_dwDamWpCnt[index] != pNewData->dbStat.m_dwDamWpCnt[index])
    {
      sprintf_s(source, sizeof(source), "WM%d=%d,", index, pNewData->dbStat.m_dwDamWpCnt[index]);
      std::strcat(buffer, source);
    }
  }
  for (int index = 0; index < 24; ++index)
  {
    if (pOldData->dbStat.m_dwForceCum[index] != pNewData->dbStat.m_dwForceCum[index])
    {
      sprintf_s(source, sizeof(source), "FM%d=%d,", index, pNewData->dbStat.m_dwForceCum[index]);
      std::strcat(buffer, source);
    }
  }
  for (int index = 0; index < 48; ++index)
  {
    if (pOldData->dbStat.m_dwSkillCum[index] != pNewData->dbStat.m_dwSkillCum[index])
    {
      sprintf_s(source, sizeof(source), "SM%d=%d,", index, pNewData->dbStat.m_dwSkillCum[index]);
      std::strcat(buffer, source);
    }
  }
  for (int index = 0; index < 3; ++index)
  {
    if (pOldData->dbStat.m_dwMakeCum[index] != pNewData->dbStat.m_dwMakeCum[index])
    {
      sprintf_s(source, sizeof(source), "MI%d=%d,", index, pNewData->dbStat.m_dwMakeCum[index]);
      std::strcat(buffer, source);
    }
  }
  if (pOldData->dbStat.m_dwSpecialCum != pNewData->dbStat.m_dwSpecialCum)
  {
    sprintf_s(source, sizeof(source), "SR=%d,", pNewData->dbStat.m_dwSpecialCum);
    std::strcat(buffer, source);
  }
  if (pOldData->dbStat.m_dwDefenceCnt != pNewData->dbStat.m_dwDefenceCnt)
  {
    sprintf_s(source, sizeof(source), "DM=%d,", pNewData->dbStat.m_dwDefenceCnt);
    std::strcat(buffer, source);
  }
  if (pOldData->dbStat.m_dwShieldCnt != pNewData->dbStat.m_dwShieldCnt)
  {
    sprintf_s(source, sizeof(source), "PM=%d,", pNewData->dbStat.m_dwShieldCnt);
    std::strcat(buffer, source);
  }
  if (pOldData->dbAvator.m_dPvPPoint != pNewData->dbAvator.m_dPvPPoint)
  {
    sprintf_s(
      source,
      sizeof(source),
      "PvpPoint=%f,",
      static_cast<double>(pNewData->dbAvator.m_dPvPPoint));
    std::strcat(buffer, source);
  }
  if (pOldData->dbAvator.m_dPvPCashBag != pNewData->dbAvator.m_dPvPCashBag)
  {
    sprintf_s(
      source,
      sizeof(source),
      "PvpCashBag=%f,",
      static_cast<double>(pNewData->dbAvator.m_dPvPCashBag));
    std::strcat(buffer, source);
  }
  if (std::strcmp(pOldData->dbAvator.m_szBindMapCode, pNewData->dbAvator.m_szBindMapCode))
  {
    pNewData->dbAvator.m_szBindMapCode[11] = 0;
    sprintf_s(source, sizeof(source), "BindMapCode='%s',", pNewData->dbAvator.m_szBindMapCode);
    std::strcat(buffer, source);
  }
  if (std::strcmp(pOldData->dbAvator.m_szBindDummy, pNewData->dbAvator.m_szBindDummy))
  {
    pNewData->dbAvator.m_szBindDummy[11] = 0;
    sprintf_s(source, sizeof(source), "BindDummy='%s',", pNewData->dbAvator.m_szBindDummy);
    std::strcat(buffer, source);
  }
  if (pOldData->dbAvator.m_dwRadarDelayTime != pNewData->dbAvator.m_dwRadarDelayTime)
  {
    sprintf_s(source, 200, "RDDelay=%d,", pNewData->dbAvator.m_dwRadarDelayTime);
    std::strcat(buffer, source);
  }
  if (pOldData->dbAvator.m_dwTakeLastMentalTicket != pNewData->dbAvator.m_dwTakeLastMentalTicket)
  {
    sprintf_s(source, 200, "TakeLastMentalTicket=%d,", pNewData->dbAvator.m_dwTakeLastMentalTicket);
    std::strcat(buffer, source);
  }
  if (pOldData->dbAvator.m_dwTakeLastCriTicket != pNewData->dbAvator.m_dwTakeLastCriTicket)
  {
    sprintf_s(source, 200, "TakeLastCriTicket=%d,", pNewData->dbAvator.m_dwTakeLastCriTicket);
    std::strcat(buffer, source);
  }
  if (pOldData->dbAvator.m_byMaxLevel != pNewData->dbAvator.m_byMaxLevel)
  {
    sprintf_s(source, sizeof(source), "MaxLevel=%d,", pNewData->dbAvator.m_byMaxLevel);
    std::strcat(buffer, source);
  }
  if (pNewData->dbCutting.m_bOldDataLoad)
  {
    std::strcat(buffer, "LeftResList='*',");
  }
  sprintf_s(source, sizeof(source), "TotalPlayMin=%d,", pNewData->dbAvator.m_dwTotalPlayMin);
  std::strcat(buffer, source);

  size_t queryLength = std::strlen(buffer);
  if (queryLength <= initialLength)
  {
    std::memset(buffer, 0, initialLength);
  }
  else
  {
    sprintf_s(source, sizeof(source), "WHERE Serial = %d", dwSerial);
    size_t len = std::strlen(buffer);
    buffer[len - 1] = 32;
    std::strcat(buffer, source);
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
  int initialLength = static_cast<int>(std::strlen(pSzQuery));

  if (pOldData->dbSupplement.dPvpPointLeak != pNewData->dbSupplement.dPvpPointLeak)
  {
    sprintf_s(buffer, 128, "PvpPointLeak=%f,", static_cast<double>(pNewData->dbSupplement.dPvpPointLeak));
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  if (pOldData->dbSupplement.bLastAttBuff != pNewData->dbSupplement.bLastAttBuff)
  {
    int lastAttBuff = pNewData->dbSupplement.bLastAttBuff;
    sprintf_s(buffer, 128, "LastAttBuff=%d,", lastAttBuff);
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  if (pOldData->dbSupplement.dwBufPotionEndTime != pNewData->dbSupplement.dwBufPotionEndTime)
  {
    sprintf_s(buffer, 128, "BufEndTime = %I64u,", pNewData->dbSupplement.dwBufPotionEndTime);
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  if (pOldData->dbSupplement.dwRaceBuffClear != pNewData->dbSupplement.dwRaceBuffClear)
  {
    sprintf_s(buffer, 128, "RaceBuffClear = %u,", pNewData->dbSupplement.dwRaceBuffClear);
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  if (pOldData->dbSupplement.dwAccumPlayTime != pNewData->dbSupplement.dwAccumPlayTime)
  {
    sprintf_s(buffer, 128, " AccumPlayTime = %d,", pNewData->dbSupplement.dwAccumPlayTime);
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  if (pOldData->dbSupplement.dwLastResetDate != pNewData->dbSupplement.dwLastResetDate)
  {
    sprintf_s(buffer, 128, " ResetAccumPlayTime = %d,", pNewData->dbSupplement.dwLastResetDate);
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  if (pOldData->dbSupplement.byVoted != pNewData->dbSupplement.byVoted)
  {
    sprintf_s(buffer, 128, " IsVoted = %d,", pNewData->dbSupplement.byVoted);
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  if (pOldData->dbSupplement.VoteEnable != pNewData->dbSupplement.VoteEnable)
  {
    sprintf_s(buffer, 128, " VoteEnable = %d,", pNewData->dbSupplement.VoteEnable);
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  if (pOldData->dbSupplement.wScanerCnt != pNewData->dbSupplement.wScanerCnt)
  {
    sprintf_s(
      buffer,
      128,
      " ScanerCnt = %d%d,",
      pNewData->dbSupplement.dwScanerGetDate,
      pNewData->dbSupplement.wScanerCnt);
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  if (pOldData->dbSupplement.dwGuildEntryDelay != pNewData->dbSupplement.dwGuildEntryDelay)
  {
    sprintf_s(buffer, 128, " GuildEntryDelay = %I64u,", pNewData->dbSupplement.dwGuildEntryDelay);
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  for (int index = 0; index < 3; ++index)
  {
    if (pOldData->dbSupplement.dwActionPoint[index] != pNewData->dbSupplement.dwActionPoint[index])
    {
      sprintf_s(buffer, 128, " ActionPoint_%d = %d,", index, pNewData->dbSupplement.dwActionPoint[index]);
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
        sprintf_s(buffer, 128, "EK%d=%d,", embellishIndex, dbKey);
        strcat_s(pSzQuery, nBufferSize, buffer);
      }
      if (pNewData->dbEquip.m_EmbellishList[embellishIndex].wAmount
          != pOldData->dbEquip.m_EmbellishList[embellishIndex].wAmount)
      {
        if (pNewData->dbEquip.m_EmbellishList[embellishIndex].wAmount == static_cast<unsigned __int16>(-1))
        {
          sprintf_s(buffer, 128, "ED%d=-1,", embellishIndex);
        }
        else
        {
          sprintf_s(
            buffer,
            128,
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
          sprintf_s(buffer, 128, "ET%d=%d,", embellishIndex, newET);
          strcat_s(pSzQuery, nBufferSize, buffer);
        }
      }
      if (pNewData->dbEquip.m_EmbellishList[embellishIndex].lnUID
          != pOldData->dbEquip.m_EmbellishList[embellishIndex].lnUID)
      {
        sprintf_s(
          buffer,
          128,
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

      if (pNewData->dbEquip.m_EmbellishList[embellishIndex].wAmount == static_cast<unsigned __int16>(-1))
      {
        int dbKey = pNewData->dbEquip.m_EmbellishList[embellishIndex].Key.CovDBKey();
        sprintf_s(
          buffer,
          128,
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
          128,
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
    sprintf_s(buffer, 128, "EK%d=%d,", embellishIndex, dbKey);
    strcat_s(pSzQuery, nBufferSize, buffer);
  }

  size_t queryLength = std::strlen(pSzQuery);
  if (queryLength <= static_cast<size_t>(initialLength))
  {
    std::memset(pSzQuery, 0, initialLength);
  }
  else
  {
    sprintf_s(buffer, 128, "WHERE Serial = %d", dwSerial);
    pSzQuery[std::strlen(pSzQuery) - 1] = 32;
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  return 1;
}

char CMainThread::_db_Update_Unit(
  unsigned int dwSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData,
  char *pSzQuery,
  const unsigned __int64 *pdwCanonicalNewUnitCutTime,
  const unsigned __int64 *pdwCanonicalOldUnitCutTime)
{
  char source[136]{};
  char *buffer = pSzQuery;
  sprintf_s(pSzQuery, kDbQuerySize, "UPDATE tbl_unit SET ");
  size_t initialLength = std::strlen(buffer);

  for (int unitIndex = 0; unitIndex < 4; ++unitIndex)
  {
    if (pNewData->dbUnit.m_List[unitIndex].byFrame == 255)
    {
      if (pOldData->dbUnit.m_List[unitIndex].byFrame != 255)
      {
        sprintf_s(source, sizeof(source), "F_%d=%d,", unitIndex, 255);
        std::strcat(buffer, source);
      }
    }
    else
    {
      if (pNewData->dbUnit.m_List[unitIndex].byFrame != pOldData->dbUnit.m_List[unitIndex].byFrame)
      {
        sprintf_s(
          source,
          sizeof(source),
          "F_%d=%d,",
          unitIndex,
          pNewData->dbUnit.m_List[unitIndex].byFrame);
        std::strcat(buffer, source);
      }
      if (pNewData->dbUnit.m_List[unitIndex].dwGauge != pOldData->dbUnit.m_List[unitIndex].dwGauge)
      {
        sprintf_s(
          source,
          sizeof(source),
          "Gg_%d=%d,",
          unitIndex,
          pNewData->dbUnit.m_List[unitIndex].dwGauge);
        std::strcat(buffer, source);
      }
      if (pNewData->dbUnit.m_List[unitIndex].byPart[0] != pOldData->dbUnit.m_List[unitIndex].byPart[0])
      {
        sprintf_s(
          source,
          sizeof(source),
          "H_%d=%d,",
          unitIndex,
          pNewData->dbUnit.m_List[unitIndex].byPart[0]);
        std::strcat(buffer, source);
      }
      if (pNewData->dbUnit.m_List[unitIndex].byPart[1] != pOldData->dbUnit.m_List[unitIndex].byPart[1])
      {
        sprintf_s(
          source,
          sizeof(source),
          "U_%d=%d,",
          unitIndex,
          pNewData->dbUnit.m_List[unitIndex].byPart[1]);
        std::strcat(buffer, source);
      }
      if (pNewData->dbUnit.m_List[unitIndex].byPart[2] != pOldData->dbUnit.m_List[unitIndex].byPart[2])
      {
        sprintf_s(
          source,
          sizeof(source),
          "L_%d=%d,",
          unitIndex,
          pNewData->dbUnit.m_List[unitIndex].byPart[2]);
        std::strcat(buffer, source);
      }
      if (pNewData->dbUnit.m_List[unitIndex].byPart[3] != pOldData->dbUnit.m_List[unitIndex].byPart[3])
      {
        sprintf_s(
          source,
          sizeof(source),
          "A_%d=%d,",
          unitIndex,
          pNewData->dbUnit.m_List[unitIndex].byPart[3]);
        std::strcat(buffer, source);
      }
      if (pNewData->dbUnit.m_List[unitIndex].byPart[4] != pOldData->dbUnit.m_List[unitIndex].byPart[4])
      {
        sprintf_s(
          source,
          sizeof(source),
          "S_%d=%d,",
          unitIndex,
          pNewData->dbUnit.m_List[unitIndex].byPart[4]);
        std::strcat(buffer, source);
      }
      if (pNewData->dbUnit.m_List[unitIndex].byPart[5] != pOldData->dbUnit.m_List[unitIndex].byPart[5])
      {
        sprintf_s(
          source,
          sizeof(source),
          "B_%d=%d,",
          unitIndex,
          pNewData->dbUnit.m_List[unitIndex].byPart[5]);
        std::strcat(buffer, source);
      }
      if (pNewData->dbUnit.m_List[unitIndex].dwBullet[0] != pOldData->dbUnit.m_List[unitIndex].dwBullet[0])
      {
        sprintf_s(
          source,
          sizeof(source),
          "AB_%d=%d,",
          unitIndex,
          pNewData->dbUnit.m_List[unitIndex].dwBullet[0]);
        std::strcat(buffer, source);
      }
      if (pNewData->dbUnit.m_List[unitIndex].dwBullet[1] != pOldData->dbUnit.m_List[unitIndex].dwBullet[1])
      {
        sprintf_s(
          source,
          sizeof(source),
          "SB_%d=%d,",
          unitIndex,
          pNewData->dbUnit.m_List[unitIndex].dwBullet[1]);
        std::strcat(buffer, source);
      }
      for (int spareIndex = 0; spareIndex < 8; ++spareIndex)
      {
        if (pNewData->dbUnit.m_List[unitIndex].dwSpare[spareIndex]
            != pOldData->dbUnit.m_List[unitIndex].dwSpare[spareIndex])
        {
          sprintf_s(
            source,
            sizeof(source),
            "Sp%d_%d=%d,",
            spareIndex,
            unitIndex,
            pNewData->dbUnit.m_List[unitIndex].dwSpare[spareIndex]);
          std::strcat(buffer, source);
        }
      }
      if (pNewData->dbUnit.m_List[unitIndex].nPullingFee != pOldData->dbUnit.m_List[unitIndex].nPullingFee)
      {
        sprintf_s(
          source,
          sizeof(source),
          "PF_%d=%d,",
          unitIndex,
          pNewData->dbUnit.m_List[unitIndex].nPullingFee);
        std::strcat(buffer, source);
      }
      const unsigned __int64 newCanonicalCutTime =
        pdwCanonicalNewUnitCutTime ? pdwCanonicalNewUnitCutTime[unitIndex] : pNewData->dbUnit.m_List[unitIndex].dwCutTime;
      const unsigned __int64 oldCanonicalCutTime =
        pdwCanonicalOldUnitCutTime ? pdwCanonicalOldUnitCutTime[unitIndex] : pOldData->dbUnit.m_List[unitIndex].dwCutTime;
      if (newCanonicalCutTime != oldCanonicalCutTime)
      {
        sprintf_s(
          source,
          sizeof(source),
          "Cut_%d=%I64u,",
          unitIndex,
          newCanonicalCutTime);
        std::strcat(buffer, source);
      }
    }
  }

  size_t queryLength = std::strlen(buffer);
  if (queryLength <= initialLength)
  {
    std::memset(buffer, 0, initialLength);
  }
  else
  {
    sprintf_s(source, sizeof(source), "WHERE Serial=%d", dwSerial);
    size_t length = std::strlen(buffer);
    buffer[length - 1] = 32;
    std::strcat(buffer, source);
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
  sprintf_s(pSzQuery, kDbQuerySize, "UPDATE tbl_userinterface SET ");
  size_t initialLength = std::strlen(buffer);

  for (int index = 0; index < 50; ++index)
  {
    const __int16 oldKey =
      static_cast<__int16>(pOldData->dbLink.m_LinkList[index].Key.CovDBKey());
    const __int16 newKey =
      static_cast<__int16>(pNewData->dbLink.m_LinkList[index].Key.CovDBKey());
    if (oldKey != newKey)
    {
      const __int16 dbKey =
        static_cast<__int16>(pNewData->dbLink.m_LinkList[index].Key.CovDBKey());
      sprintf_s(source, sizeof(source), "LB%d=%d,", index, dbKey);
      std::strcat(buffer, source);
    }
  }
  for (int index = 0; index < 8; ++index)
  {
    if (pOldData->dbSfcont.m_List[0][index].dwKey != pNewData->dbSfcont.m_List[0][index].dwKey)
    {
      sprintf_s(
        source,
        sizeof(source),
        "DCF%d=%d,",
        index,
        pNewData->dbSfcont.m_List[0][index].dwKey);
      std::strcat(buffer, source);
    }
  }
  for (int index = 0; index < 8; ++index)
  {
    if (pOldData->dbSfcont.m_List[1][index].dwKey != pNewData->dbSfcont.m_List[1][index].dwKey)
    {
      sprintf_s(
        source,
        sizeof(source),
        "HCF%d=%d,",
        index,
        pNewData->dbSfcont.m_List[1][index].dwKey);
      std::strcat(buffer, source);
    }
  }
  for (int index = 0; index < 2; ++index)
  {
    if (pOldData->dbLink.m_dwSkill[index] != pNewData->dbLink.m_dwSkill[index])
    {
      sprintf_s(
        source,
        sizeof(source),
        "SkillWin%d=%d,",
        index,
        pNewData->dbLink.m_dwSkill[index]);
      std::strcat(buffer, source);
    }
  }
  for (int index = 0; index < 2; ++index)
  {
    if (pOldData->dbLink.m_dwForce[index] != pNewData->dbLink.m_dwForce[index])
    {
      sprintf_s(
        source,
        sizeof(source),
        "ForceWin%d=%d,",
        index,
        pNewData->dbLink.m_dwForce[index]);
      std::strcat(buffer, source);
    }
  }
  for (int index = 0; index < 2; ++index)
  {
    if (pOldData->dbLink.m_dwCharacter[index] != pNewData->dbLink.m_dwCharacter[index])
    {
      sprintf_s(
        source,
        sizeof(source),
        "CharWin%d=%d,",
        index,
        pNewData->dbLink.m_dwCharacter[index]);
      std::strcat(buffer, source);
    }
  }
  for (int index = 0; index < 2; ++index)
  {
    if (pOldData->dbLink.m_dwAnimus[index] != pNewData->dbLink.m_dwAnimus[index])
    {
      sprintf_s(
        source,
        sizeof(source),
        "AnimusWin%d=%d,",
        index,
        pNewData->dbLink.m_dwAnimus[index]);
      std::strcat(buffer, source);
    }
  }
  if (pOldData->dbLink.m_dwInven != pNewData->dbLink.m_dwInven)
  {
    sprintf_s(source, sizeof(source), "InvenWin=%d,", pNewData->dbLink.m_dwInven);
    std::strcat(buffer, source);
  }
  for (int index = 0; index < 5; ++index)
  {
    if (pOldData->dbLink.m_dwInvenBag[index] != pNewData->dbLink.m_dwInvenBag[index])
    {
      sprintf_s(
        source,
        sizeof(source),
        "InvenBag%d=%d,",
        index,
        pNewData->dbLink.m_dwInvenBag[index]);
      std::strcat(buffer, source);
    }
  }
  if (pOldData->dbLink.m_byLinkBoardLock != pNewData->dbLink.m_byLinkBoardLock)
  {
    sprintf_s(source, sizeof(source), "LBLock=%d,", pNewData->dbLink.m_byLinkBoardLock);
    std::strcat(buffer, source);
  }

  size_t queryLength = std::strlen(buffer);
  if (queryLength <= initialLength)
  {
    std::memset(buffer, 0, initialLength);
  }
  else
  {
    sprintf_s(source, sizeof(source), "WHERE Serial=%d", dwSerial);
    size_t length = std::strlen(buffer);
    buffer[length - 1] = 32;
    std::strcat(buffer, source);
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
  sprintf_s(pSzQuery, kDbQuerySize, "UPDATE tbl_quest SET ");
  size_t initialLength = std::strlen(buffer);

  for (int index = 0; index < 30; ++index)
  {
    if (pNewData->dbQuest.m_List[index].byQuestType != pOldData->dbQuest.m_List[index].byQuestType)
    {
      sprintf_s(source, sizeof(source), "Type%d=%d,", index, pNewData->dbQuest.m_List[index].byQuestType);
      std::strcat(buffer, source);
    }
    if (pNewData->dbQuest.m_List[index].wIndex != pOldData->dbQuest.m_List[index].wIndex)
    {
      sprintf_s(
        source,
        sizeof(source),
        "QuestIndex%d=%d,",
        index,
        static_cast<__int16>(pNewData->dbQuest.m_List[index].wIndex));
      std::strcat(buffer, source);
    }
    if (pNewData->dbQuest.m_List[index].dwPassSec != pOldData->dbQuest.m_List[index].dwPassSec)
    {
      sprintf_s(source, sizeof(source), "PassSec%d=%d,", index, pNewData->dbQuest.m_List[index].dwPassSec);
      std::strcat(buffer, source);
    }
    if (pNewData->dbQuest.m_List[index].wNum[0] != pOldData->dbQuest.m_List[index].wNum[0])
    {
      sprintf_s(
        source,
        sizeof(source),
        "FirstAct%d=%d,",
        index,
        static_cast<__int16>(pNewData->dbQuest.m_List[index].wNum[0]));
      std::strcat(buffer, source);
    }
    if (pNewData->dbQuest.m_List[index].wNum[1] != pOldData->dbQuest.m_List[index].wNum[1])
    {
      sprintf_s(
        source,
        sizeof(source),
        "SecondAct%d=%d,",
        index,
        static_cast<__int16>(pNewData->dbQuest.m_List[index].wNum[1]));
      std::strcat(buffer, source);
    }
    if (pNewData->dbQuest.m_List[index].wNum[2] != pOldData->dbQuest.m_List[index].wNum[2])
    {
      sprintf_s(
        source,
        sizeof(source),
        "ThirdAct%d=%d,",
        index,
        static_cast<__int16>(pNewData->dbQuest.m_List[index].wNum[2]));
      std::strcat(buffer, source);
    }
  }

  size_t queryLength = std::strlen(buffer);
  if (queryLength <= initialLength)
  {
    std::memset(buffer, 0, initialLength);
  }
  else
  {
    sprintf_s(source, sizeof(source), "WHERE Serial=%d", dwSerial);
    size_t length = std::strlen(buffer);
    buffer[length - 1] = 32;
    std::strcat(buffer, source);
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
  sprintf_s(pSzQuery, kNpcQuestQuerySize, "UPDATE tbl_npc_quest_history SET ");
  size_t initialLength = std::strlen(destination);

  for (int index = 0; index < 70; ++index)
  {
    if (pNewData->dbQuest.m_History[index].byLevel != pOldData->dbQuest.m_History[index].byLevel
        || std::strncmp(
          pNewData->dbQuest.m_History[index].szQuestCode,
          pOldData->dbQuest.m_History[index].szQuestCode,
          7))
    {
      int level = pNewData->dbQuest.m_History[index].byLevel;
      sprintf_s(
        buffer,
        sizeof(buffer),
        "Code%d='%s',Level%d=%d ,Time%d=%I64u,",
        index + 1,
        pNewData->dbQuest.m_History[index].szQuestCode,
        index + 1,
        level,
        index + 1,
        pNewData->dbQuest.m_History[index].dwEventEndTime);
      std::strcat(destination, buffer);
    }
  }

  size_t queryLength = std::strlen(destination);
  if (queryLength <= initialLength)
  {
    std::memset(destination, 0, initialLength);
  }
  else
  {
    sprintf_s(buffer, sizeof(buffer), "WHERE Serial=%d", dwSerial);
    size_t length = std::strlen(destination);
    destination[length - 1] = 32;
    std::strcat(destination, buffer);
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
      std::memcpy(rawTime, &pNewData->dbQuest.m_StartHistory[listIndex].tmStartTime, sizeof(rawTime));
      std::memcpy(timeParts, rawTime, sizeof(rawTime));
      sprintf_s(
        timeBuffer,
        25,
        "%04d-%02d-%02d %02d:%02d:%02d.%d",
        timeParts[0],
        timeParts[1],
        timeParts[3],
        timeParts[4],
        timeParts[5],
        timeParts[6],
        timeParts[7]);
      if (!m_pWorldDB->Insert_Start_NpcQuest_History(dwSerial,
            pNewData->dbQuest.m_StartHistory[listIndex].szQuestCode,
            pNewData->dbQuest.m_StartHistory[listIndex].byLevel,
            timeBuffer,
            pNewData->dbQuest.m_StartHistory[listIndex].nEndTime))
      {
        return 0;
      }
      std::memset(timeBuffer, 0, 25);
      std::memset(timeParts, 0, sizeof(timeParts));
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
  sprintf_s(pSzQuery, kDbQuerySize, "UPDATE tbl_Buddy SET ");
  size_t initialLength = std::strlen(buffer);

  for (int index = 0; index < 50; ++index)
  {
    if (pNewData->dbBuddy.m_List[index].dwSerial != pOldData->dbBuddy.m_List[index].dwSerial)
    {
      sprintf_s(source, sizeof(source), "Serial%d=%d,", index, pNewData->dbBuddy.m_List[index].dwSerial);
      std::strcat(buffer, source);
    }
  }

  size_t queryLength = std::strlen(buffer);
  if (queryLength <= initialLength)
  {
    std::memset(buffer, 0, initialLength);
  }
  else
  {
    sprintf_s(source, sizeof(source), "WHERE Serial=%d", dwSerial);
    size_t length = std::strlen(buffer);
    buffer[length - 1] = 32;
    std::strcat(buffer, source);
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
    sprintf_s(pSzQuery, kDbQuerySize, "UPDATE tbl_itemcombine_ex_result SET ");
    unsigned int initialLength = static_cast<unsigned int>(std::strlen(pSzQuery));
    if (pNewData->dbItemCombineEx.m_dwCheckKey != pOldData->dbItemCombineEx.m_dwCheckKey)
    {
      sprintf_s(
        buffer,
        sizeof(buffer),
        "IsResult=%d,Dalant=%d,CheckKey=%d,DlgType=%d,ItemSelectCount=%d,ItemListCount=%d,",
        pNewData->dbItemCombineEx.m_bIsResult,
        pNewData->dbItemCombineEx.m_dwDalant,
        pNewData->dbItemCombineEx.m_dwCheckKey,
        pNewData->dbItemCombineEx.m_byDlgType,
        pNewData->dbItemCombineEx.m_bySelectItemCount,
        pNewData->dbItemCombineEx.m_byItemListNum);
      std::strcat(pSzQuery, buffer);
      int listCount = pNewData->dbItemCombineEx.m_byItemListNum;
      for (int index = 0; index < listCount; ++index)
      {
        int dbKey = pNewData->dbItemCombineEx.m_List[index].Key.CovDBKey();
        sprintf_s(
          buffer,
          sizeof(buffer),
          "K%d=%d,D%d=%d,U%d=%d,",
          index,
          dbKey,
          index,
          pNewData->dbItemCombineEx.m_List[index].dwDur,
          index,
          pNewData->dbItemCombineEx.m_List[index].dwUpt);
        std::strcat(pSzQuery, buffer);
      }
      sprintf_s(buffer, sizeof(buffer), "resulteffecttype=%d,", pNewData->dbItemCombineEx.m_dwResultEffectType);
      std::strcat(pSzQuery, buffer);
      sprintf_s(
        buffer,
        sizeof(buffer),
        "resulteffectmsgcode=%d,",
        pNewData->dbItemCombineEx.m_dwResultEffectMsgCode);
      std::strcat(pSzQuery, buffer);
    }

    size_t queryLength = std::strlen(pSzQuery);
    if (queryLength <= initialLength)
    {
      std::memset(pSzQuery, 0, initialLength);
    }
    else
    {
      sprintf_s(buffer, sizeof(buffer), "WHERE Serial=%d", dwSerial);
      pSzQuery[std::strlen(pSzQuery) - 1] = 32;
      std::strcat(pSzQuery, buffer);
    }
  }
  else if (pNewData->dbItemCombineEx.m_bIsResult != pOldData->dbItemCombineEx.m_bIsResult)
  {
    sprintf_s(pSzQuery, kDbQuerySize, "{ CALL pUpdate_CombineEx_Result_Pop( %d ) }", dwSerial);
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
  sprintf_s(pSzQuery, kDbQuerySize, "UPDATE tbl_AccountTrunk SET ");
  unsigned int initialLength = static_cast<unsigned int>(std::strlen(buffer));
  unsigned __int8 slotCount = pNewData->dbTrunk.bySlotNum;
  unsigned __int8 raceIndex = pNewData->dbAvator.m_byRaceSexCode / 2;

  if (std::strcmp(pNewData->dbTrunk.wszPasswd, pOldData->dbTrunk.wszPasswd))
  {
    sprintf_s(
      source,
      sizeof(source),
      "TrunkPass=cast('%s' as binary),",
      pNewData->dbTrunk.wszPasswd);
    std::strcat(buffer, source);
  }
  if (pNewData->dbTrunk.dDalant != pOldData->dbTrunk.dDalant)
  {
    sprintf_s(
      source,
      sizeof(source),
      "Dalant%d=%f,",
      raceIndex,
      static_cast<double>(pNewData->dbTrunk.dDalant));
    std::strcat(buffer, source);
  }
  if (pNewData->dbTrunk.dGold != pOldData->dbTrunk.dGold)
  {
    sprintf_s(
      source,
      sizeof(source),
      "Gold%d=%f,",
      raceIndex,
      static_cast<double>(pNewData->dbTrunk.dGold));
    std::strcat(buffer, source);
  }
  if (pNewData->dbTrunk.byHintIndex != pOldData->dbTrunk.byHintIndex)
  {
    sprintf_s(source, sizeof(source), "HintIndex=%d,", pNewData->dbTrunk.byHintIndex);
    std::strcat(buffer, source);
  }
  if (std::strcmp(pNewData->dbTrunk.wszHintAnswer, pOldData->dbTrunk.wszHintAnswer))
  {
    sprintf_s(source, sizeof(source), "HintAnswer='%s',", pNewData->dbTrunk.wszHintAnswer);
    std::strcat(buffer, source);
  }
  if (pNewData->dbTrunk.bySlotNum != pOldData->dbTrunk.bySlotNum)
  {
    sprintf_s(source, sizeof(source), "EstSlot=%d,", pNewData->dbTrunk.bySlotNum);
    std::strcat(buffer, source);
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
          sprintf_s(source, sizeof(source), "K%d=%d,", index, dbKey);
          std::strcat(buffer, source);
        }
        if (pNewData->dbTrunk.m_List[index].dwDur != pOldData->dbTrunk.m_List[index].dwDur)
        {
          sprintf_s(
            source,
            sizeof(source),
            "D%d=%I64d,",
            index,
            pNewData->dbTrunk.m_List[index].dwDur);
          std::strcat(buffer, source);
        }
        if (pNewData->dbTrunk.m_List[index].dwUpt != pOldData->dbTrunk.m_List[index].dwUpt)
        {
          sprintf_s(source, sizeof(source), "U%d=%d,", index, pNewData->dbTrunk.m_List[index].dwUpt);
          std::strcat(buffer, source);
        }
        if (pNewData->dbTrunk.m_List[index].byRace != pOldData->dbTrunk.m_List[index].byRace)
        {
          sprintf_s(source, sizeof(source), "R%d=%d,", index, pNewData->dbTrunk.m_List[index].byRace);
          std::strcat(buffer, source);
        }
        if (pNewData->dbTrunk.m_List[index].lnUID != pOldData->dbTrunk.m_List[index].lnUID)
        {
          sprintf_s(
            source,
            sizeof(source),
            "S%d=%I64d,",
            index,
            pNewData->dbTrunk.m_List[index].lnUID);
          std::strcat(buffer, source);
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
            sprintf_s(source, sizeof(source), "T%d=%d,", index, newET);
            std::strcat(buffer, source);
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
        sprintf_s(
          source,
          sizeof(source),
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
        std::strcat(buffer, source);
      }
    }
    else if (pOldData->dbTrunk.m_List[index].Key.IsFilled())
    {
      int dbKey = pNewData->dbTrunk.m_List[index].Key.CovDBKey();
      sprintf_s(source, sizeof(source), "K%d=%d,", index, dbKey);
      std::strcat(buffer, source);
    }
  }

  size_t queryLength = std::strlen(buffer);
  if (queryLength <= initialLength)
  {
    std::memset(buffer, 0, initialLength);
  }
  else
  {
    sprintf_s(source, sizeof(source), "WHERE AccountSerial=%d", dwAccountSerial);
    size_t length = std::strlen(buffer);
    buffer[length - 1] = 32;
    std::strcat(buffer, source);
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
  sprintf_s(pSzQuery, kDbQuerySize, "UPDATE tbl_AccountTrunk_Extend SET ");
  unsigned int initialLength = static_cast<unsigned int>(std::strlen(buffer));
  unsigned __int8 slotCount = pNewData->dbTrunk.byExtSlotNum;

  if (pNewData->dbTrunk.byExtSlotNum != pOldData->dbTrunk.byExtSlotNum)
  {
    sprintf_s(source, sizeof(source), "EstSlot=%d,", pNewData->dbTrunk.byExtSlotNum);
    std::strcat(buffer, source);
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
          sprintf_s(source, sizeof(source), "K%d=%d,", index, dbKey);
          std::strcat(buffer, source);
        }
        if (pNewData->dbTrunk.m_ExtList[index].dwDur != pOldData->dbTrunk.m_ExtList[index].dwDur)
        {
          sprintf_s(
            source,
            sizeof(source),
            "D%d=%I64d,",
            index,
            pNewData->dbTrunk.m_ExtList[index].dwDur);
          std::strcat(buffer, source);
        }
        if (pNewData->dbTrunk.m_ExtList[index].dwUpt != pOldData->dbTrunk.m_ExtList[index].dwUpt)
        {
          sprintf_s(source, sizeof(source), "U%d=%d,", index, pNewData->dbTrunk.m_ExtList[index].dwUpt);
          std::strcat(buffer, source);
        }
        if (pNewData->dbTrunk.m_ExtList[index].byRace != pOldData->dbTrunk.m_ExtList[index].byRace)
        {
          sprintf_s(source, sizeof(source), "R%d=%d,", index, pNewData->dbTrunk.m_ExtList[index].byRace);
          std::strcat(buffer, source);
        }
        if (pNewData->dbTrunk.m_ExtList[index].lnUID != pOldData->dbTrunk.m_ExtList[index].lnUID)
        {
          sprintf_s(
            source,
            sizeof(source),
            "S%d=%I64d,",
            index,
            pNewData->dbTrunk.m_ExtList[index].lnUID);
          std::strcat(buffer, source);
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
            sprintf_s(source, sizeof(source), "T%d=%d,", index, newET);
            std::strcat(buffer, source);
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
        sprintf_s(
          source,
          sizeof(source),
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
        std::strcat(buffer, source);
      }
    }
    else if (pOldData->dbTrunk.m_ExtList[index].Key.IsFilled())
    {
      int dbKey = pNewData->dbTrunk.m_ExtList[index].Key.CovDBKey();
      sprintf_s(source, sizeof(source), "K%d=%d,", index, dbKey);
      std::strcat(buffer, source);
    }
  }

  size_t queryLength = std::strlen(buffer);
  if (queryLength <= initialLength)
  {
    std::memset(buffer, 0, initialLength);
  }
  else
  {
    sprintf_s(source, sizeof(source), "WHERE AccountSerial=%d", dwAccountSerial);
    size_t length = std::strlen(buffer);
    buffer[length - 1] = 32;
    std::strcat(buffer, source);
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
  int initialLength = static_cast<int>(std::strlen(pSzQuery));

  if (pOldData->dbTimeLimitInfo.dwFatigue != pNewData->dbTimeLimitInfo.dwFatigue)
  {
    sprintf_s(buffer, 128, "Fatigue=%d,", pNewData->dbTimeLimitInfo.dwFatigue);
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  if (pOldData->dbTimeLimitInfo.byTLStatus != pNewData->dbTimeLimitInfo.byTLStatus)
  {
    sprintf_s(buffer, 128, "TLStatus = %u,", pNewData->dbTimeLimitInfo.byTLStatus);
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  unsigned __int64 localTime = GetKorLocalTime();
  sprintf_s(buffer, 128, "LastLogoutTime=%I64u,", localTime);
  strcat_s(pSzQuery, nBufferSize, buffer);

  size_t queryLength = std::strlen(pSzQuery);
  if (queryLength <= static_cast<size_t>(initialLength))
  {
    std::memset(pSzQuery, 0, initialLength);
  }
  else
  {
    sprintf_s(buffer, 128, "WHERE AccountSerial = %d", dwAccSerial);
    pSzQuery[std::strlen(pSzQuery) - 1] = 32;
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
  sprintf_s(pSzQuery, kDbQuerySize, "update [dbo].[tbl_aminepersonal_inven] set ");
  size_t initialLength = std::strlen(buffer);

  for (int index = 0; index < 40; ++index)
  {
    const _PERSONALAMINE_INVEN_DB_BASE::_LIST &newItem = pNewData->dbPersonalAmineInven.m_List[index];
    const _PERSONALAMINE_INVEN_DB_BASE::_LIST &oldItem = pOldData->dbPersonalAmineInven.m_List[index];

    if (newItem.Key.IsFilled())
    {
      if (oldItem.Key.IsFilled())
      {
        int newKey = static_cast<int>(newItem.Key.CovDBKey());
        int oldKey = static_cast<int>(oldItem.Key.CovDBKey());
        if (newKey != oldKey)
        {
          int dbKey = static_cast<int>(newItem.Key.CovDBKey());
          sprintf_s(source, sizeof(source), "K%d=%d,", index, dbKey);
          std::strcat(buffer, source);
        }
        if (newItem.dwDur != oldItem.dwDur)
        {
          sprintf_s(source, sizeof(source), "N%d=%d,", index, newItem.dwDur);
          std::strcat(buffer, source);
        }
      }
      else
      {
        int dbKey = static_cast<int>(newItem.Key.CovDBKey());
        sprintf_s(
          source,
          sizeof(source),
          "K%d=%d,N%d=%d,",
          index,
          dbKey,
          index,
          newItem.dwDur);
        std::strcat(buffer, source);
      }
    }
    else if (oldItem.Key.IsFilled())
    {
      int dbKey = static_cast<int>(newItem.Key.CovDBKey());
      sprintf_s(source, sizeof(source), "K%d=%d,", index, dbKey);
      std::strcat(buffer, source);
    }
  }

  size_t queryLength = std::strlen(buffer);
  if (queryLength <= initialLength)
  {
    std::memset(buffer, 0, initialLength);
  }
  else
  {
    sprintf_s(source, sizeof(source), "WHERE avatorserial=%d", dwSerial);
    size_t length = std::strlen(buffer);
    buffer[length - 1] = 32;
    std::strcat(buffer, source);
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
  sprintf_s(pSzQuery, kDbQuerySize, "update [dbo].[tbl_pvppointlimitinfo] set ");
  int initialLength = static_cast<int>(std::strlen(pSzQuery));

  if (pNewData->dbPvpPointLimit.tUpdatedate != pOldData->dbPvpPointLimit.tUpdatedate)
  {
    tm *localTime = localtime_2(&pNewData->dbPvpPointLimit.tUpdatedate);
    if (!localTime)
    {
      sprintf_s(
        pErrMsg,
        kErrorMsgSize,
        "localtime( dbPvpPointLimit.tUpdatedate(%d) ) NULL!",
        static_cast<int>(pNewData->dbPvpPointLimit.tUpdatedate));
      return 0;
    }
    sprintf_s(
      buffer,
      sizeof(buffer),
      "[updatedate]='%04u-%02u-%02u %02u:%02u:%02u',",
      localTime->tm_year + 1900,
      localTime->tm_mon + 1,
      localTime->tm_mday,
      localTime->tm_hour,
      localTime->tm_min,
      localTime->tm_sec);
    std::strcat(pSzQuery, buffer);
  }
  if (pNewData->dbPvpPointLimit.bUseUp != pOldData->dbPvpPointLimit.bUseUp)
  {
    sprintf_s(buffer, sizeof(buffer), "[useup]=%u,", pNewData->dbPvpPointLimit.bUseUp);
    std::strcat(pSzQuery, buffer);
  }
  if (pNewData->dbPvpPointLimit.byLimitRate != pOldData->dbPvpPointLimit.byLimitRate)
  {
    sprintf_s(buffer, sizeof(buffer), "[limitrate]=%u,", pNewData->dbPvpPointLimit.byLimitRate);
    std::strcat(pSzQuery, buffer);
  }
  if (pNewData->dbPvpPointLimit.dOriginalPoint != pOldData->dbPvpPointLimit.dOriginalPoint)
  {
    sprintf_s(
      buffer,
      sizeof(buffer),
      "[originalpoint]=%.10f,",
      static_cast<double>(pNewData->dbPvpPointLimit.dOriginalPoint));
    std::strcat(pSzQuery, buffer);
  }
  if (pNewData->dbPvpPointLimit.dLimitPoint != pOldData->dbPvpPointLimit.dLimitPoint)
  {
    sprintf_s(
      buffer,
      sizeof(buffer),
      "[limitpoint]=%.10f,",
      static_cast<double>(pNewData->dbPvpPointLimit.dLimitPoint));
    std::strcat(pSzQuery, buffer);
  }
  if (pNewData->dbPvpPointLimit.dUsePoint != pOldData->dbPvpPointLimit.dUsePoint)
  {
    sprintf_s(
      buffer,
      sizeof(buffer),
      "[usepoint]=%.10f ",
      static_cast<double>(pNewData->dbPvpPointLimit.dUsePoint));
    std::strcat(pSzQuery, buffer);
  }

  size_t queryLength = std::strlen(pSzQuery);
  if (queryLength <= static_cast<size_t>(initialLength))
  {
    std::memset(pSzQuery, 0, initialLength);
  }
  else
  {
    sprintf_s(buffer, sizeof(buffer), "where serial=%d", dwSerial);
    pSzQuery[std::strlen(pSzQuery) - 1] = 32;
    std::strcat(pSzQuery, buffer);
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
  sprintf_s(pSzQuery, kDbQuerySize, "UPDATE tbl_CryMsg SET ");
  size_t initialLength = std::strlen(buffer);
  for (int index = 0; index < 10; ++index)
  {
    char *newMsg = pNewData->dbBossCry.m_List[index].wszCryMsg;
    if (!IsSQLValidString(newMsg))
    {
      newMsg[0] = '\0';
    }
  }
  if (pOldData)
  {
    for (int index = 0; index < 10; ++index)
    {
      char *oldMsg = pOldData->dbBossCry.m_List[index].wszCryMsg;
      if (!IsSQLValidString(oldMsg))
      {
        oldMsg[0] = '\0';
      }
    }
  }

  for (int index = 0; index < 10; ++index)
  {
    if (std::strcmp(pNewData->dbBossCry.m_List[index].wszCryMsg, pOldData->dbBossCry.m_List[index].wszCryMsg))
    {
      sprintf_s(
        source,
        sizeof(source),
        "CryMsg%d='%s',",
        index + 1,
        pNewData->dbBossCry.m_List[index].wszCryMsg);
      std::strcat(buffer, source);
    }
  }

  size_t queryLength = std::strlen(buffer);
  if (queryLength <= initialLength)
  {
    std::memset(buffer, 0, initialLength);
  }
  else
  {
    sprintf_s(source, sizeof(source), "WHERE Serial=%d", dwSerial);
    size_t length = std::strlen(buffer);
    buffer[length - 1] = 32;
    std::strcat(buffer, source);
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
  sprintf_s(pSzQuery, kDbQuerySize, "update [dbo].[tbl_pvporderview] set ");
  int initialLength = static_cast<int>(std::strlen(pSzQuery));

  if (pNewData->dbPvpOrderView.tUpdatedate != pOldData->dbPvpOrderView.tUpdatedate)
  {
    tm *localTime = localtime_2(&pNewData->dbPvpOrderView.tUpdatedate);
    if (!localTime)
    {
      sprintf_s(
        pErrMsg,
        kErrorMsgSize,
        "localtime( dbPvpOrderView.tUpdatedate(%d) ) NULL!",
        static_cast<int>(pNewData->dbPvpOrderView.tUpdatedate));
      return 0;
    }
    sprintf_s(
      buffer,
      sizeof(buffer),
      "[UpdateDate]='%04u-%02u-%02u %02u:%02u:%02u',",
      localTime->tm_year + 1900,
      localTime->tm_mon + 1,
      localTime->tm_mday,
      localTime->tm_hour,
      localTime->tm_min,
      localTime->tm_sec);
    std::strcat(pSzQuery, buffer);
  }
  if (pNewData->dbPvpOrderView.nDeath != pOldData->dbPvpOrderView.nDeath)
  {
    sprintf_s(buffer, sizeof(buffer), "[Death]= %d,", pNewData->dbPvpOrderView.nDeath);
    std::strcat(pSzQuery, buffer);
  }
  if (pNewData->dbPvpOrderView.nKill != pOldData->dbPvpOrderView.nKill)
  {
    sprintf_s(buffer, sizeof(buffer), "[Kill]= %d,", pNewData->dbPvpOrderView.nKill);
    std::strcat(pSzQuery, buffer);
  }
  if (pNewData->dbPvpOrderView.dTodayStacked != pOldData->dbPvpOrderView.dTodayStacked)
  {
    sprintf_s(
      buffer,
      sizeof(buffer),
      "[TodayStacked]= %.10f,",
      static_cast<double>(pNewData->dbPvpOrderView.dTodayStacked));
    std::strcat(pSzQuery, buffer);
  }
  if (pNewData->dbPvpOrderView.dPvpPoint != pOldData->dbPvpOrderView.dPvpPoint)
  {
    sprintf_s(
      buffer,
      sizeof(buffer),
      "[PvpPoint]= %.10f,",
      static_cast<double>(pNewData->dbPvpOrderView.dPvpPoint));
    std::strcat(pSzQuery, buffer);
  }
  if (pNewData->dbPvpOrderView.dPvpTempCash != pOldData->dbPvpOrderView.dPvpTempCash)
  {
    sprintf_s(
      buffer,
      sizeof(buffer),
      "[PvpTempCash]= %.10f,",
      static_cast<double>(pNewData->dbPvpOrderView.dPvpTempCash));
    std::strcat(pSzQuery, buffer);
  }
  if (pNewData->dbPvpOrderView.dPvpCash != pOldData->dbPvpOrderView.dPvpCash)
  {
    sprintf_s(
      buffer,
      sizeof(buffer),
      "[PvpCash]= %.10f,",
      static_cast<double>(pNewData->dbPvpOrderView.dPvpCash));
    std::strcat(pSzQuery, buffer);
  }
  for (int index = 0; index < 10; ++index)
  {
    if (pNewData->dbPvpOrderView.dwKillerSerial[index] != pOldData->dbPvpOrderView.dwKillerSerial[index])
    {
      sprintf_s(
        buffer,
        sizeof(buffer),
        "[KillerSerial%d] = %d,",
        index,
        pNewData->dbPvpOrderView.dwKillerSerial[index]);
      std::strcat(pSzQuery, buffer);
    }
  }
  if (pNewData->dbPvpOrderView.byContHaveCash != pOldData->dbPvpOrderView.byContHaveCash)
  {
    sprintf_s(buffer, sizeof(buffer), "[ContHaveCash] = %d,", pNewData->dbPvpOrderView.byContHaveCash);
    std::strcat(pSzQuery, buffer);
  }
  if (pNewData->dbPvpOrderView.byContLoseCash != pOldData->dbPvpOrderView.byContLoseCash)
  {
    sprintf_s(buffer, sizeof(buffer), "[ContLoseCash] = %d,", pNewData->dbPvpOrderView.byContLoseCash);
    std::strcat(pSzQuery, buffer);
  }
  if (pNewData->dbPvpOrderView.bRaceWarRecvr != pOldData->dbPvpOrderView.bRaceWarRecvr)
  {
    sprintf_s(buffer, sizeof(buffer), "[RaceWarRecvr] = %d ", pNewData->dbPvpOrderView.bRaceWarRecvr);
    std::strcat(pSzQuery, buffer);
  }

  size_t queryLength = std::strlen(pSzQuery);
  if (queryLength <= static_cast<size_t>(initialLength))
  {
    std::memset(pSzQuery, 0, initialLength);
  }
  else
  {
    sprintf_s(buffer, sizeof(buffer), "where serial=%d", dwSerial);
    pSzQuery[std::strlen(pSzQuery) - 1] = 32;
    std::strcat(pSzQuery, buffer);
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
  sprintf_s(pSzQuery, kDbQuerySize, "update [dbo].[tbl_PrimiumPlayTime] set ");
  unsigned int initialLength = static_cast<unsigned int>(std::strlen(pSzQuery));

  if (pNewData->dbPlayTimeInPcbang.dwLastConnTime != pOldData->dbPlayTimeInPcbang.dwLastConnTime)
  {
    sprintf_s(buffer, sizeof(buffer), "[LastConnTime]= %I64u,", pNewData->dbPlayTimeInPcbang.dwLastConnTime);
    std::strcat(pSzQuery, buffer);
  }
  if (pNewData->dbPlayTimeInPcbang.dwContPlayTime != pOldData->dbPlayTimeInPcbang.dwContPlayTime)
  {
    sprintf_s(buffer, sizeof(buffer), "[ContPlayTime]= %d,", pNewData->dbPlayTimeInPcbang.dwContPlayTime);
    std::strcat(pSzQuery, buffer);
  }
  if (pNewData->dbPlayTimeInPcbang.bForcedClose != pOldData->dbPlayTimeInPcbang.bForcedClose)
  {
    sprintf_s(buffer, sizeof(buffer), "[ForcedClose]= %d,", pNewData->dbPlayTimeInPcbang.bForcedClose);
    std::strcat(pSzQuery, buffer);
  }
  if (pNewData->dbPlayTimeInPcbang.byReceiveCoupon != pOldData->dbPlayTimeInPcbang.byReceiveCoupon)
  {
    sprintf_s(buffer, sizeof(buffer), "[ReceiveCoupon]= %d,", pNewData->dbPlayTimeInPcbang.byReceiveCoupon);
    std::strcat(pSzQuery, buffer);
  }
  if (pNewData->dbPlayTimeInPcbang.byEnsureTime != pOldData->dbPlayTimeInPcbang.byEnsureTime)
  {
    sprintf_s(buffer, sizeof(buffer), "[EnsureTime]= %d ", pNewData->dbPlayTimeInPcbang.byEnsureTime);
    std::strcat(pSzQuery, buffer);
  }

  size_t queryLength = std::strlen(pSzQuery);
  if (queryLength <= initialLength)
  {
    std::memset(pSzQuery, 0, initialLength);
  }
  else
  {
    sprintf_s(buffer, sizeof(buffer), "where AccountSerial=%d", dwAccSerial);
    pSzQuery[std::strlen(pSzQuery) - 1] = 32;
    std::strcat(pSzQuery, buffer);
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
  int initialLength = static_cast<int>(std::strlen(pSzQuery));
  DWORD now = timeGetTime();

  for (int index = 0; index < 38; ++index)
  {
    if (g_PotionMgr.IsPotionDelayUseIndex(index)
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
      sprintf_s(buffer, 128, "PD%d=%u,", index, delay);
      strcat_s(pSzQuery, nBufferSize, buffer);
    }
  }

  size_t queryLength = std::strlen(pSzQuery);
  if (queryLength <= static_cast<size_t>(initialLength))
  {
    std::memset(pSzQuery, 0, initialLength);
  }
  else
  {
    sprintf_s(buffer, 128, "WHERE Serial = %d", dwSerial);
    pSzQuery[std::strlen(pSzQuery) - 1] = 32;
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
  int initialLength = static_cast<int>(std::strlen(pSzQuery));

  for (int index = 0; index < 20; ++index)
  {
    const _CUTTING_DB_BASE::_LIST &newItem = pNewData->dbCutting.m_List[index];
    const _CUTTING_DB_BASE::_LIST &oldItem = pOldData->dbCutting.m_List[index];

    int newKey = static_cast<int>(newItem.Key.CovDBKey());
    int oldKey = static_cast<int>(oldItem.Key.CovDBKey());
    if (newKey != oldKey)
    {
      sprintf_s(buffer, 128, "K%d=%d,", index, newKey);
      strcat_s(pSzQuery, nBufferSize, buffer);
    }
    unsigned int newDur = newItem.dwDur;
    if (newDur != oldItem.dwDur)
    {
      sprintf_s(buffer, 128, "D%d=%d,", index, newDur);
      strcat_s(pSzQuery, nBufferSize, buffer);
    }
  }

  size_t queryLength = std::strlen(pSzQuery);
  if (queryLength <= static_cast<size_t>(initialLength))
  {
    std::memset(pSzQuery, 0, initialLength);
  }
  else
  {
    sprintf_s(buffer, 128, "WHERE Serial = %d", dwSerial);
    pSzQuery[std::strlen(pSzQuery) - 1] = 32;
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
  int initialLength = static_cast<int>(std::strlen(pSzQuery));

  for (int index = 0; index < 50; ++index)
  {
    if (pNewData->dbPcBangFavorItem.lnUID[index] != pOldData->dbPcBangFavorItem.lnUID[index])
    {
      sprintf_s(buffer, sizeof(buffer), "K%d=%I64d,", index, pNewData->dbPcBangFavorItem.lnUID[index]);
      strcat_s(pSzQuery, nBufferSize, buffer);
    }
  }

  size_t queryLength = std::strlen(pSzQuery);
  if (queryLength <= static_cast<size_t>(initialLength))
  {
    std::memset(pSzQuery, 0, initialLength);
  }
  else
  {
    sprintf_s(buffer, 128, "WHERE Serial = %d", dwSerial);
    pSzQuery[std::strlen(pSzQuery) - 1] = 32;
    strcat_s(pSzQuery, nBufferSize, buffer);
  }
  return 1;
}

// END


