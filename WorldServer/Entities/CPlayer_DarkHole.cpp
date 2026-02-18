#include "pch.h"

#include "CPlayer.h"
#include "CQuestMgr.h"
#include "CRecordData.h"
#include "CMapOperation.h"
#include "CMapData.h"
#include "CBsp.h"
#include "CObjectList.h"
#include "CMonster.h"
#include "pnt_rect.h"
#include "CMainThread.h"
#include "CItemStore.h"
#include "CNetworkEX.h"
#include "StoreList_fld.h"
#include "CActionPointSystemMgr.h"
#include "CItemStoreManager.h"
#include "CMapItemStoreList.h"
#include "CGuild.h"
#include "CUserDB.h"
#include "CNetProcess.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CUnmannedTraderTaxRateManager.h"
#include "CNationSettingManager.h"
#include "CNationSettingData.h"
#include "GlobalObjects.h"
#include "CGuildBattleController.h"
#include "CGuildMasterEffect.h"
#include "CGuildRoomSystem.h"
#include "CHonorGuild.h"
#include "CHolyStoneSystem.h"
#include "CPotionMgr.h"
#include "guild_honor_set_request_clzo.h"
#include "CMgrGuildHistory.h"
#include "CMgrAvatorItemHistory.h"
#include "CPostSystemManager.h"
#include "CPartyPlayer.h"
#include "LendItemMng.h"
#include "CUnmannedTraderController.h"
#include "cStaticMember_Player.h"
#include "CTransportShip.h"
#include "CLogFile.h"
#include "CItemUpgradeTable.h"
#include "CMoneySupplyMgr.h"
#include "COreAmountMgr.h"
#include "COreCuttingTable.h"
#include "CDarkHole.h"
#include "CDarkHoleChannel.h"
#include "CDarkHoleDungeonQuest.h"
#include "CDarkHoleDungeonQuestSetup.h"
#include "TicketItem_fld.h"
#include "darkhole_create_setdata.h"
#include "ENTER_DUNGEON_NEW_POS.h"
#include "QuestHappenEvent_fld.h"
#include "announ_message_receipt_udp.h"
#include "equip_up_item_lv_limit_zocl.h"
#include "sf_delay_download_result_zocl.h"
#include "darkhole_ask_reenter_inform_zocl.h"
#include "darkhole_answer_reenter_result_zocl.h"
#include "darkhole_clear_out_result_zocl.h"
#include "darkhole_enter_result_zocl.h"
#include "darkhole_giveup_out_result_zocl.h"
#include "darkhole_open_result_zocl.h"
#include "insert_new_quest_inform_zocl.h"
#include "insert_next_quest_inform_zocl.h"
#include "move_to_own_stonemap_inform_zocl.h"
#include "move_to_own_stonemap_result_zocl.h"
#include "pt_result_change_tax_rate_zocl.h"
#include "PCBANG_PRIMIUM_FAVOR.h"
#include "economy_history_data.h"
#include "DqsDbStructs.h"
#include "EconomySystemFunctions.h"
#include "TimeItem.h"
#include "trans_ship_renew_ticket_result_zocl.h"
#include "trans_gm_msg_inform_zocl.h"
#include "trunk_change_passwd_result_zocl.h"
#include "trunk_download_result_zocl.h"
#include "trunk_est_result_zocl.h"
#include "trunk_extend_result_zocl.h"
#include "trunk_hint_answer_result_zocl.h"
#include "trunk_io_money_result_zocl.h"
#include "trunk_io_result_zocl.h"
#include "trunk_potionsocket_division_result_zocl.h"
#include "trunk_pw_hint_index_result_zocl.h"
#include "trunk_res_division_result_zocl.h"
#include "guildroom_rent_request_clzo.h"
#include "guildroom_enter_request_clzo.h"
#include "guildroom_out_request_clzo.h"
#include "GuardTowerItem_fld.h"
#include "UnitKeyItem_fld.h"
#include "TrapItem_fld.h"
#include "UnitFrame_fld.h"
#include "UnitPart_fld.h"
#include "UnitBullet_fld.h"
#include "BulletItem_fld.h"
#include "ItemCombine_exp_fld.h"
#include "combine_ex_item_result_zocl.h"
#include "combine_ex_item_accept_request_clzo.h"
#include "combine_ex_item_accept_result_zocl.h"
#include "qry_case_disjointguild.h"
#include "WorldServerUtil.h"
#include "NetCheckPackets.h"
#include "GlobalObjectDefs.h"

#include <ctime>
#include <mmsystem.h>
#include <cstdlib>
#include <cmath>
#include <vector>

void CPlayer::SendMsg_ReEnterAsk(unsigned __int16 wChannelIndex, unsigned int dwChannelSerial)
{
  _darkhole_ask_reenter_inform_zocl msg{};
  msg.wChannelIndex = wChannelIndex;
  msg.dwChannelSerial = dwChannelSerial;

  unsigned __int8 type[2] = {35, static_cast<unsigned __int8>(-47)};
  const unsigned __int16 len = msg.size();
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
}

void CPlayer::SendMsg_DarkHoleOpenFail(unsigned int n, unsigned __int8 byErrCode)
{
  _darkhole_open_result_zocl msg{};
  msg.byRetCode = byErrCode;
  msg.wHoleIndex = static_cast<unsigned __int16>(-1);
  msg.dwHoleSerial = static_cast<unsigned int>(-1);

  unsigned __int8 type[2]{35, static_cast<unsigned __int8>(-54)};
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&msg), msg.size());
}

void CPlayer::SendMsg_DarkHoleOpenResult(
  unsigned int n,
  int bPartyOnly,
  unsigned __int8 byErrCode,
  unsigned __int16 wHoleIndex,
  unsigned int dwHoleSerial)
{
  _darkhole_open_result_zocl msg{};
  msg.byRetCode = byErrCode;
  msg.wHoleIndex = wHoleIndex;
  msg.dwHoleSerial = dwHoleSerial;

  unsigned __int8 type[2]{35, static_cast<unsigned __int8>(-54)};
  if (bPartyOnly)
  {
    CPlayer *partyMembers[8]{};
    const unsigned __int8 memberCount = _GetPartyMemberInCircle(partyMembers, 8, true);
    for (unsigned __int8 j = 0; j < memberCount; ++j)
    {
      CPlayer *member = partyMembers[j];
      if (member && member->m_bLive)
      {
        g_Network.m_pProcess[0]->LoadSendMsg(
          member->m_id.wIndex,
          type,
          reinterpret_cast<char *>(&msg),
          msg.size());
      }
    }
  }
  else
  {
    g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&msg), msg.size());
    CircleReport(type, reinterpret_cast<char *>(&msg), msg.size(), false);
  }
}

void CPlayer::SendMsg_EnterDarkHole(unsigned __int8 byErrCode, unsigned int dwHoleSerial)
{
  _darkhole_enter_result_zocl msg{};
  msg.byRetCode = byErrCode;
  msg.dwHoleSerial = dwHoleSerial;
  msg.byMapCode = static_cast<unsigned __int8>(m_pCurMap->m_pMapSet->m_dwIndex);
  FloatToShort(m_fCurPos, msg.zPos, 3);

  unsigned __int8 type[2]{35, static_cast<unsigned __int8>(-52)};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), msg.size());
}

void CPlayer::SendMsg_GiveupDarkHole(unsigned __int8 byErrCode)
{
  _darkhole_giveup_out_result_zocl msg{};
  msg.byRetCode = byErrCode;
  msg.wTarMapIndex = m_pCurMap->m_pMapSet->m_dwIndex;
  FloatToShort(m_fCurPos, msg.zTarPos, 3);

  unsigned __int8 type[2]{35, static_cast<unsigned __int8>(-50)};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), msg.size());
}

void CPlayer::SendMsg_ClearDarkHole(unsigned __int8 byErrCode)
{
  _darkhole_clear_out_result_zocl msg{};
  msg.byRetCode = byErrCode;
  msg.wTarMapIndex = m_pCurMap->m_pMapSet->m_dwIndex;
  FloatToShort(m_fCurPos, msg.zTarPos, 3);

  unsigned __int8 type[2]{35, static_cast<unsigned __int8>(-48)};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), msg.size());
}

void CPlayer::SendMsg_ReEnterDarkHoleResult(unsigned __int8 byRetCode)
{
  _darkhole_answer_reenter_result_zocl msg{};
  msg.byRetCode = byRetCode;

  unsigned __int8 type[2]{35, static_cast<unsigned __int8>(-45)};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), msg.size());
}

int CPlayer::GetRewardItems_DarkDungeon(
  _dh_reward_sub_setup *pSetup,
  _STORAGE_LIST::_db_con *pItems,
  bool bRealBoss)
{
  if (!pSetup || !pItems)
  {
    return 0;
  }

  unsigned int minMatchedPercent = 0x7FFF8000u;
  int rewardCount = 0;

  const int high = rand() << 16;
  unsigned int drawValue = static_cast<unsigned int>(rand() + high);
  if (!bRealBoss)
  {
    drawValue *= 10;
  }

  for (int index = 0; index < pSetup->nItemNum; ++index)
  {
    const unsigned int percent = pSetup->m_dwGivePercent[index];
    if (drawValue <= percent)
    {
      if (minMatchedPercent > percent)
      {
        minMatchedPercent = percent;
      }
    }
  }

  for (int index = 0; index < pSetup->nItemNum; ++index)
  {
    if (pSetup->m_dwGivePercent[index] == minMatchedPercent)
    {
      const unsigned __int8 raceSexCode = m_pUserDB->m_AvatorData.dbAvator.m_byRaceSexCode;
      _STORAGE_LIST::_db_con *candidate = pSetup->Item[index];
      if (candidate
          && IsItemEquipCivil(candidate->m_byTableCode, candidate->m_wItemIndex, raceSexCode))
      {
        memcpy_0(&pItems[rewardCount], candidate, sizeof(_STORAGE_LIST::_db_con));
        ++rewardCount;
      }
    }
  }

  return rewardCount;
}

void CPlayer::Reward_DarkDungeon(
  _dh_reward_sub_setup *pSetup,
  char *pszTitle,
  bool bRealBoss,
  _STORAGE_LIST::_db_con *pItem,
  int *bIsRewarded)
{
  if (!pSetup || !bIsRewarded)
  {
    return;
  }

  _STORAGE_LIST::_db_con rewardCandidates[128]{};
  const long double playerPenalty = g_Main.m_pTimeLimitMgr->GetPlayerPenalty(m_id.wIndex);
  const int rewardCandidateCount = GetRewardItems_DarkDungeon(pSetup, rewardCandidates, bRealBoss);

  if (rewardCandidateCount > 0 && g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) != 99)
  {
    _STORAGE_LIST::_db_con rewardItem{};
    const int pick = rand() % rewardCandidateCount;
    memcpy_0(&rewardItem, &rewardCandidates[pick], sizeof(rewardItem));

    if (m_Param.m_dbInven.GetIndexEmptyCon() == 0xFF)
    {
      CreateItemBox(
        &rewardItem,
        this,
        static_cast<unsigned int>(-1),
        false,
        nullptr,
        3u,
        m_pCurMap,
        m_wMapLayerIndex,
        m_fCurPos,
        false);

      char historyClause[132]{};
      sprintf(historyClause, "Quest G (%s)", pszTitle ? pszTitle : "");
      CPlayer::s_MgrItemHistory.reward_add_item(
        m_ObjID.m_wIndex,
        historyClause,
        &rewardItem,
        m_szItemHistoryFileName);
    }
    else
    {
      rewardItem.m_wSerial = m_Param.GetNewItemSerial();

      const _TimeItem_fld *timeRec = TimeItem::FindTimeRec(rewardItem.m_byTableCode, rewardItem.m_wItemIndex);
      if (timeRec && timeRec->m_nCheckType)
      {
        __time32_t now[11]{};
        _time32(now);
        rewardItem.m_byCsMethod = static_cast<unsigned __int8>(timeRec->m_nCheckType);
        rewardItem.m_dwT = timeRec->m_nUseTime + now[0];
        rewardItem.m_dwLendRegdTime = now[0];
      }

      if (!Emb_AddStorage(0, &rewardItem, false, true))
      {
        CPlayer::s_MgrItemHistory.add_storage_fail(
          m_ObjID.m_wIndex,
          &rewardItem,
          "CPlayer::Reward_DarkDungeon - Emb_AddStorage() Fail",
          m_szItemHistoryFileName);
      }
      else
      {
        SendMsg_RewardAddItem(&rewardItem, 2u);

        char historyClause[160]{};
        sprintf(historyClause, "Quest (%s)", pszTitle ? pszTitle : "");
        CPlayer::s_MgrItemHistory.reward_add_item(
          m_ObjID.m_wIndex,
          historyClause,
          &rewardItem,
          m_szItemHistoryFileName);
      }
    }

    SendMsg_FanfareItem(4u, &rewardItem, nullptr);

    if (pItem)
    {
      *bIsRewarded = 1;
      pItem->m_byTableCode = rewardItem.m_byTableCode;
      pItem->m_wItemIndex = rewardItem.m_wItemIndex;
    }
  }
  else if (rewardCandidateCount <= 0)
  {
    *bIsRewarded = 0;
  }

  if (pSetup->dwDalant)
  {
    AddDalant(pSetup->dwDalant, true);
    SendMsg_ExchangeMoneyResult(0);

    const unsigned int addDalant =
      static_cast<unsigned int>(static_cast<double>(static_cast<int>(pSetup->dwDalant)) * static_cast<double>(playerPenalty));
    const unsigned int sumGold = m_Param.GetGold();
    const unsigned int sumDalant = m_Param.GetDalant();

    char historyClause[132]{};
    sprintf(historyClause, "Quest (%s)", pszTitle ? pszTitle : "");
    CPlayer::s_MgrItemHistory.reward_add_money(
      m_ObjID.m_wIndex,
      historyClause,
      addDalant,
      0,
      sumDalant,
      sumGold,
      m_szItemHistoryFileName);

    const int level = m_Param.GetLevel();
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      CMoneySupplyMgr *moneySupplyMgr = CMoneySupplyMgr::Instance();
      moneySupplyMgr->UpdateQuestRewardMoneyData(
        static_cast<unsigned __int8>(m_Param.GetRaceCode()),
        level,
        const_cast<char *>(m_Param.GetPtrCurClass()->m_strCode),
        pSetup->dwDalant);
    }
  }

  if (pSetup->dExp > 0.0)
  {
    AlterExp(static_cast<double>(pSetup->dExp), true, false, true);
  }

  if (pSetup->dwPvp)
  {
    AlterPvPPoint(static_cast<double>(static_cast<int>(pSetup->dwPvp)), quest_inc, static_cast<unsigned int>(-1));
  }
}

void CPlayer::SendMsg_DarkHoleRewardMessage(
  _STORAGE_LIST::_db_con *pItem,
  unsigned int dwMemberIndex,
  int isRewarded)
{

  _darkhole_reward_message_inform_zocl msg{};
  msg.bIsRewarded = isRewarded;
  msg.dwPartyMemberIndex = dwMemberIndex;
  if (pItem)
  {
    msg.byTableCode = pItem->m_byTableCode;
    msg.wItemIndex = pItem->m_wItemIndex;
  }

  unsigned __int8 type[2]{35, static_cast<unsigned __int8>(-44)};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::pc_DarkHoleOpenRequest(unsigned __int16 dwItemSerial)
{
  unsigned __int8 errCode = 0;
  _STORAGE_LIST::_db_con *item = nullptr;
  _base_fld *record = nullptr;
  CDarkHole *hole = nullptr;
  _dh_quest_setup *questSetup = nullptr;

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    errCode = 28;
  }
  else if (IsExistDarkHoleOpenGate())
  {
    errCode = 3;
  }
  else if (m_pmTrd.bDTradeMode || m_bCorpse)
  {
    errCode = 16;
  }
  else if (m_bInGuildBattle)
  {
    errCode = 20;
  }
  else
  {
    hole = SerarchEmptyDarkHole();
    if (hole)
    {
      item = m_Param.m_dbInven.GetPtrFromSerial(dwItemSerial);
      if (item)
      {
        if (item->m_bLock)
        {
          errCode = 13;
        }
        else if (item->m_byTableCode == 23)
        {
          record = g_Main.m_tblItemData[23].GetRecord(item->m_wItemIndex);
          if (record)
          {
            questSetup = g_DarkHoleQuest.GetQuestSetupPtr(item->m_wItemIndex);
            if (questSetup)
            {
              if (g_DarkHoleQuest.CanOpenChannel(record->m_dwIndex))
              {
                if (!questSetup->bPartyOnly || m_pPartyMgr->IsPartyMode())
                {
                  const int levelMin = GetLevel();
                  if (levelMin < questSetup->nLimitLvMin || GetLevel() > questSetup->nLimitLvMax)
                  {
                    errCode = 19;
                  }
                }
                else
                {
                  errCode = 17;
                }
              }
              else
              {
                errCode = 3;
              }
            }
            else
            {
              errCode = 100;
            }
          }
          else
          {
            errCode = 100;
          }
        }
        else
        {
          errCode = 2;
        }
      }
      else
      {
        errCode = 1;
      }
    }
    else
    {
      errCode = 3;
    }
  }

  unsigned int holeSerial = static_cast<unsigned int>(-1);
  if (!errCode)
  {
    _darkhole_create_setdata param;
    param.m_pRecordSet = record;
    param.m_pMap = m_pCurMap;
    param.m_nLayerIndex = m_wMapLayerIndex;
    m_pCurMap->GetRandPosInRange(m_fCurPos, 10, param.m_fStartPos);
    param.pOpener = this;
    if (hole->Create(&param))
    {
      Emb_AlterDurPoint(0, item->m_byStorageIndex, -1, false, false);
      CPlayer::s_MgrItemHistory.consume_del_item(
        m_ObjID.m_wIndex,
        item,
        m_szItemHistoryFileName);
      const char *holeCode = record[2].m_strCode;
      const char *charName = m_Param.GetCharNameA();
      g_Main.m_logDungeon.Write("OPEN: %s, DARKHOLE:%s", charName, holeCode);
      holeSerial = hole->m_dwObjSerial;
      m_pPartyMgr->m_pDarkHole = hole;
    }
    else
    {
      errCode = 4;
    }
  }

  if (errCode)
  {
    SendMsg_DarkHoleOpenFail(m_id.wIndex, errCode);
  }
  else
  {
    SendMsg_DarkHoleOpenResult(
      m_id.wIndex,
      questSetup->bPartyOnly,
      0,
      hole->m_ObjID.m_wIndex,
      holeSerial);
  }
}

void CPlayer::pc_DarkHoleEnterRequest(unsigned __int16 wHoleIndex, unsigned int dwHoleSerial)
{
  unsigned __int8 errCode = 0;
  _dh_quest_setup *questSetup = nullptr;
  CDarkHole *hole = &g_DarkHole[wHoleIndex];
  _ENTER_DUNGEON_NEW_POS newPos{};

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    errCode = 28;
  }
  else if (m_bInGuildBattle)
  {
    errCode = 20;
  }
  else if (hole->m_bLive && hole->m_dwObjSerial == dwHoleSerial && hole->m_pChannel)
  {
    if (m_pCurMap->m_pMapSet->m_nMapType)
    {
      errCode = 11;
    }
    else if (GetCurSecNum() == -1 || m_bMapLoading)
    {
      errCode = 8;
    }
    else if (hole->IsNewEnterAbleNum())
    {
      if (hole->IsNewEnterAblePlayer(this))
      {
        if (hole->GetEnterNewPos(&newPos))
        {
          questSetup = hole->m_pChannel->m_pQuestSetup;
          if (!questSetup)
          {
            return;
          }
          const int levelMin = GetLevel();
          if (levelMin < questSetup->nLimitLvMin || GetLevel() > questSetup->nLimitLvMax)
          {
            errCode = 19;
          }
        }
        else
        {
          errCode = 7;
        }
      }
      else
      {
        errCode = 6;
      }
    }
    else
    {
      errCode = 18;
    }
  }
  else
  {
    errCode = 5;
  }

  if (!errCode)
  {
    CMapData *oldMap = m_pCurMap;
    float oldPos[3]{m_fCurPos[0], m_fCurPos[1], m_fCurPos[2]};
    const unsigned __int16 oldLayer = m_wMapLayerIndex;
    CMapData *intoMap = g_MapOper.GetMap(newPos.byMapCode);
    OutOfMap(intoMap, newPos.wLayerIndex, 5u, newPos.fPos);
    hole->EnterPlayer(this, oldMap, oldLayer, oldPos, false);
    if (hole->m_pRecordSet)
    {
      const char *holeCode = hole->m_pRecordSet[2].m_strCode;
      const char *openerName = hole->m_aszOpenerName;
      const char *charName = m_Param.GetCharNameA();
      g_Main.m_logDungeon.Write(
        "ENTER: %s, (OPENER:%s, DARKHOLE:%s)",
        charName,
        openerName,
        holeCode);
    }
    else
    {
      const char *openerName = hole->m_aszOpenerName;
      const char *charName = m_Param.GetCharNameA();
      g_Main.m_logDungeon.Write(
        "ENTER: %s, (OPENER:%s, DARKHOLE:%s)",
        charName,
        openerName,
        "InvalidDungenName");
    }
  }

  SendMsg_EnterDarkHole(errCode, dwHoleSerial);
}

void CPlayer::pc_DarkHoleGiveupOutRequest()
{
  unsigned __int8 errCode = 0;
  _dh_player_mgr *playerInfo = nullptr;

  if (m_pCurMap->m_pMapSet->m_nMapType == 1)
  {
    if (m_pDHChannel)
    {
      playerInfo = m_pDHChannel->GetPlayerInfo(m_dwObjSerial);
      if (!playerInfo)
      {
        errCode = 9;
      }
    }
    else
    {
      errCode = 9;
    }
  }
  else
  {
    errCode = 9;
  }

  if (!errCode)
  {
    OutOfMap(playerInfo->LastPos.pMap, 0, 4u, playerInfo->LastPos.fPos);
    m_pDHChannel->ClearMember(this, false, nullptr);
  }
  SendMsg_GiveupDarkHole(errCode);
}

void CPlayer::pc_DarkHoleClearOutRequest()
{
  unsigned __int8 errCode = 0;
  _dh_player_mgr *playerInfo = nullptr;

  if (m_pCurMap->m_pMapSet->m_nMapType == 1)
  {
    if (m_pDHChannel)
    {
      playerInfo = m_pDHChannel->GetPlayerInfo(m_dwObjSerial);
      if (!playerInfo)
      {
        errCode = 9;
      }
    }
    else
    {
      errCode = 9;
    }
  }
  else
  {
    errCode = 9;
  }

  if (!errCode)
  {
    OutOfMap(playerInfo->LastPos.pMap, 0, 4u, playerInfo->LastPos.fPos);
    m_pDHChannel->ClearMember(this, false, nullptr);
  }
  SendMsg_ClearDarkHole(errCode);
}

void CPlayer::pc_DarkHoleAnswerReenterRequest(
  bool bEnter,
  unsigned __int16 wChannelIndex,
  unsigned int dwChannelSerial)
{
unsigned __int8 errCode = 0;
  _ENTER_DUNGEON_NEW_POS newPos{};
  CDarkHoleChannel *channel = g_DarkHoleQuest.GetChannel(wChannelIndex);

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    errCode = 28;
  }
  else if (m_pDHChannel)
  {
    errCode = 15;
  }
  else if (channel->IsFill() && channel->m_dwChannelSerial == dwChannelSerial)
  {
    if (channel->IsReEnterable(m_dwObjSerial))
    {
      if (m_pCurMap->m_pMapSet->m_nMapType)
      {
        errCode = 11;
      }
      else if (GetCurSecNum() == -1 || m_bMapLoading)
      {
        errCode = 8;
      }
      else if (IsRidingUnit())
      {
        errCode = 12;
      }
      else
      {
        const unsigned int memberNum = channel->GetCurrentMemberNum();
        if (memberNum < channel->m_pQuestSetup->dwMaxMember)
        {
          if (!channel->GetEnterNewPos(&newPos))
          {
            errCode = 7;
          }
        }
        else
        {
          errCode = 18;
        }
      }
    }
    else
    {
      errCode = 5;
    }
  }
  else
  {
    errCode = 5;
  }

  if (!errCode)
  {
    CMapData *oldMap = m_pCurMap;
    float oldPos[3]{m_fCurPos[0], m_fCurPos[1], m_fCurPos[2]};
    const unsigned __int16 oldLayer = m_wMapLayerIndex;
    CMapData *intoMap = g_MapOper.GetMap(newPos.byMapCode);
    OutOfMap(intoMap, newPos.wLayerIndex, 5u, newPos.fPos);
    channel->PushMember(this, true, oldMap, oldLayer, oldPos);
    const char *questTitle = channel->m_pQuestSetup->szQuestTitle;
    const char *openerName = channel->m_aszOpenerName;
    const char *charName = m_Param.GetCharNameA();
    g_Main.m_logDungeon.Write(
      "REENTER: %s, (OPENER:%s, DARKHOLE:%s)",
      charName,
      openerName,
      questTitle);
  }

  SendMsg_ReEnterDarkHoleResult(errCode);
}

