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
#include "Packet/ZoneClientPacket.h"
#include "qry_case_disjointguild.h"
#include "WorldServerUtil.h"
#include "NetCheckPackets.h"
#include "GlobalObjectDefs.h"

#include <ctime>
#include <mmsystem.h>
#include <cstdlib>
#include <cmath>
#include <vector>

void CPlayer::SendMsg_HSKQuestSucc(char byQuestCode, char bSucc)
{
  _holyquest_succ_inform_zocl msg{};
  msg.byQuestCode = byQuestCode;
  msg.bSucc = bSucc != 0;

  unsigned __int8 type[2] = {25, 13};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CPlayer::SendMsg_InsertQuestItemInform(_STORAGE_LIST::_db_con *pItem)
{
  _insert_quest_item_inform_zocl msg{};
  msg.byTableCode = static_cast<char>(pItem->m_byTableCode);
  msg.wItemIndex = pItem->m_wItemIndex;
  msg.dwDurPoint = static_cast<unsigned int>(pItem->m_dwDur);
  msg.dwLv = pItem->m_dwLv;

  unsigned __int8 type[2] = {24, 15};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CPlayer::SendMsg_NpcQuestListResult(_NPCQuestIndexTempData *pQuestIndexData)
{
  _npc_quest_list_result_zocl packet{};
  packet.byQuestNum = static_cast<unsigned __int8>(pQuestIndexData->nQuestNum);
  for (int index = 0; index < pQuestIndexData->nQuestNum; ++index)
  {
    packet.QuestIndexList[index] = pQuestIndexData->IndexData[index].dwQuestIndex;
  }

  unsigned __int8 type[2] = {24, 21};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    0x79u);
}

void CPlayer::SendMsg_ResultNpcQuest(char bSucc)
{
  _npc_quest_result_zocl msg{};
  msg.bSuccess = bSucc != 0;

  unsigned __int8 type[2] = {24, 18};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CPlayer::SendMsg_InsertQuestFailure(char byEventType, unsigned int dwEventIndex, unsigned __int8 byEventNodeIndex)
{
  _insert_quest_failure_zocl msg{};
  msg.byEventType = byEventType;
  msg.dwEventIndex = dwEventIndex;
  msg.byEventNodeIndex = static_cast<char>(byEventNodeIndex);

  unsigned __int8 pbyType[2]{24, 4};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CPlayer::pc_SelectQuestAfterHappenEvent(unsigned __int8 bySelectIndex)
{
  if (!this->m_QuestMgr.m_LastHappenEvent.isset())
  {
    return;
  }

  _happen_event_cont happenEvent = this->m_QuestMgr.m_LastHappenEvent;
  if (!this->Emb_StartQuest(bySelectIndex, &happenEvent))
  {
    this->SendMsg_InsertQuestFailure(
      happenEvent.m_QtHpType,
      static_cast<unsigned int>(happenEvent.m_nIndexInType),
      static_cast<unsigned __int8>(happenEvent.m_nRaceCode));
  }
}

void CPlayer::pc_BriefPass(unsigned __int8 byQuestSlotIndex)
{
  _QUEST_DB_BASE::_LIST *quest = &this->m_Param.m_QuestDB.m_List[byQuestSlotIndex];
  if (quest->byQuestType == 255)
  {
    return;
  }

  _base_fld *record = CQuestMgr::s_tblQuest->GetRecord(quest->wIndex);
  if (record)
  {
    this->Emb_CheckActForQuest(15, record->m_strCode, 1u, false);
  }
}

void CPlayer::pc_RequestDialogWithNPC(CItemStore *pStore)
{
  if (GetSqrt(this->m_fCurPos, pStore->GetStorePos()) > 60.0f)
  {
    return;
  }

  const int npcRace = pStore->m_byNpcRaceCode;
  const int raceCode = this->m_Param.GetRaceCode();
  if (npcRace != raceCode && npcRace != 255)
  {
    return;
  }

  char *npcCode = pStore->GetNpcCode();
  if (!npcCode)
  {
    return;
  }

  this->Emb_CheckActForQuest(14, npcCode, 1u, false);
  this->Emb_CheckActForQuest(1, npcCode, 1u, false);
}

void CPlayer::pc_RequestWatchingWithNPC(CItemStore *pStore)
{
  if (GetSqrt(this->m_fCurPos, pStore->GetStorePos()) > 60.0f)
  {
    return;
  }

  const int npcRace = pStore->m_byNpcRaceCode;
  const int raceCode = this->m_Param.GetRaceCode();
  if (npcRace != raceCode && npcRace != 255)
  {
    return;
  }

  char *npcCode = pStore->GetNpcCode();
  if (npcCode)
  {
    this->Emb_CheckActForQuest(17, npcCode, 1u, false);
  }
}

void CPlayer::pc_SelectQuestReward(
  unsigned __int8 byQuestDBSlot,
  unsigned __int8 bySelectItemSlotIndex,
  unsigned __int8 bySelectLinkQuestIndex)
{
  _QUEST_DB_BASE::_LIST *questSlot = &m_Param.m_QuestDB.m_List[byQuestDBSlot];
  if (questSlot->byQuestType == 255)
  {
    return;
  }

  for (int index = 0; index < 3; ++index)
  {
    if (questSlot->wNum[index] != 65535)
    {
      return;
    }
  }

  _Quest_fld *questRecord = reinterpret_cast<_Quest_fld *>(CQuestMgr::s_tblQuest->GetRecord(questSlot->wIndex));
  if (!questRecord)
  {
    return;
  }

  if (bySelectItemSlotIndex == 255)
  {
    if (questRecord->m_bSelectConsITMenual)
    {
      return;
    }
  }
  else if (!questRecord->m_bSelectConsITMenual)
  {
    return;
  }

  if (bySelectItemSlotIndex != 255)
  {
    const char *rewardCode = questRecord->m_RewardItem[bySelectItemSlotIndex].m_strConsITCode;
    if (!strncmp(rewardCode, "-1", 2u))
    {
      return;
    }
  }

  if (bySelectLinkQuestIndex != 255)
  {
    const char *linkCode = questRecord->m_strLinkQuest[bySelectItemSlotIndex];
    if (!strncmp(linkCode, "-1", 2u))
    {
      return;
    }
  }

  Emb_CompleteQuest(byQuestDBSlot, bySelectItemSlotIndex, bySelectLinkQuestIndex);
}

void CPlayer::pc_RequestQuestListFromNPC(CItemStore *pStore)
{
  if (!pStore)
  {
    return;
  }

  float *storePos = pStore->GetStorePos();
  if (GetSqrt(m_fCurPos, storePos) > 80.0)
  {
    return;
  }

  const int npcRaceCode = pStore->m_byNpcRaceCode;
  const int raceCode = m_Param.GetRaceCode();
  if (npcRaceCode != raceCode && npcRaceCode != 255)
  {
    return;
  }

  char *eventCode = pStore->GetNpcCode();
  if (!eventCode)
  {
    return;
  }

  std::memset(&m_NPCQuestIndexTempData, 0, sizeof(m_NPCQuestIndexTempData));
  m_QuestMgr.CheckNPCQuestList(
    eventCode,
    static_cast<unsigned __int8>(m_Param.GetRaceCode()),
    &m_NPCQuestIndexTempData);
  this->SendMsg_NpcQuestListResult(&m_NPCQuestIndexTempData);
}

void CPlayer::pc_RequestQuestFromNPC(CItemStore *pStore, unsigned int dwNPCQuestIndex)
{
  if (!pStore)
  {
    return;
  }

  float *storePos = pStore->GetStorePos();
  if (GetSqrt(m_fCurPos, storePos) > 80.0)
  {
    return;
  }

  const int npcRaceCode = pStore->m_byNpcRaceCode;
  const int raceCode = m_Param.GetRaceCode();
  if (npcRaceCode != raceCode && npcRaceCode != 255)
  {
    return;
  }

  char *eventCode = pStore->GetNpcCode();
  if (!eventCode)
  {
    return;
  }

  const bool created = Emb_CreateNPCQuest(eventCode, dwNPCQuestIndex);
  this->SendMsg_ResultNpcQuest(created ? 1 : 0);
}

bool CPlayer::Emb_CreateNPCQuest(char *pszEventCode, unsigned int dwNPCQuestIndex)
{
  unsigned int happenIndex = 0;
  bool hasIndex = false;
  for (int index = 0; index < 30; ++index)
  {
    if (m_NPCQuestIndexTempData.IndexData[index].dwQuestIndex == dwNPCQuestIndex)
    {
      happenIndex = m_NPCQuestIndexTempData.IndexData[index].dwQuestHappenIndex;
      hasIndex = true;
      break;
    }
  }
  if (!hasIndex)
  {
    return false;
  }

  _happen_event_cont *sourceEvent = m_QuestMgr.CheckNPCQuestStartable(
    pszEventCode,
    static_cast<unsigned __int8>(m_Param.GetRaceCode()),
    static_cast<int>(dwNPCQuestIndex),
    happenIndex);
  if (!sourceEvent)
  {
    return false;
  }

  char skipStartHistoryInsert = 0;
  _Quest_fld *questRecord = static_cast<_Quest_fld *>(CQuestMgr::s_tblQuest->GetRecord(static_cast<int>(dwNPCQuestIndex)));
  if (m_pUserDB->m_AvatorData.dbQuest.dwListCnt)
  {
    for (unsigned int index = 0; index < m_pUserDB->m_AvatorData.dbQuest.dwListCnt; ++index)
    {
      if (!std::strcmp(m_pUserDB->m_AvatorData.dbQuest.m_StartHistory[index].szQuestCode, questRecord->m_strCode))
      {
        if (questRecord->m_bQuestRepeat)
        {
          break;
        }
        skipStartHistoryInsert = 1;
        return false;
      }
    }
  }

  _happen_event_cont happenEvent{};
  std::memcpy(&happenEvent, sourceEvent, sizeof(happenEvent));
  if (Emb_StartQuest(255, &happenEvent))
  {
    if (skipStartHistoryInsert)
    {
      return true;
    }
    if (questRecord->m_bQuestRepeat)
    {
      return true;
    }

    _QUEST_DB_BASE::_START_NPC_QUEST_HISTORY history{};
    std::strcpy(history.szQuestCode, questRecord->m_strCode);
    history.byLevel = static_cast<unsigned __int8>(m_Param.GetLevel());
    __time64_t now = 0;
    _time64(&now);
    history.nEndTime = static_cast<unsigned int>(static_cast<int>(
      static_cast<double>(static_cast<int>(now)) + static_cast<double>(questRecord->m_dRepeatTime)));
    return m_pUserDB->Update_StartNPCQuestHistory(
             static_cast<unsigned __int8>(m_pUserDB->m_AvatorData.dbQuest.dwListCnt),
             &history)
      != 0;
  }

  for (int index = 0; index < 3; ++index)
  {
    if (!m_QuestMgr.m_pTempHappenEvent[index].isset())
    {
      std::memcpy(&m_QuestMgr.m_pTempHappenEvent[index], &happenEvent, sizeof(m_QuestMgr.m_pTempHappenEvent[index]));
      return false;
    }
  }

  return false;
}

void CPlayer::LimLvNpcQuestDelete(unsigned __int8 byQuestDBSlot)
{
  SendMsg_QuestFailure(6u, byQuestDBSlot);
  this->m_QuestMgr.DeleteQuestData(byQuestDBSlot);
  this->m_pUserDB->Update_QuestDelete(byQuestDBSlot);
}

