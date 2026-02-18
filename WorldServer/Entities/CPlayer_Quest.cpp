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

void CPlayer::SendMsg_HSKQuestSucc(char byQuestCode, char bSucc)
{
  char payload[2]{};
  payload[0] = byQuestCode;
  payload[1] = bSucc;

  unsigned __int8 type[2] = {25, 13};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 2u);
}

void CPlayer::SendMsg_InsertQuestItemInform(_STORAGE_LIST::_db_con *pItem)
{
  char payload[0x0B]{};
  payload[0] = static_cast<char>(pItem->m_byTableCode);
  *reinterpret_cast<unsigned __int16 *>(payload + 1) = pItem->m_wItemIndex;
  *reinterpret_cast<unsigned int *>(payload + 3) = static_cast<unsigned int>(pItem->m_dwDur);
  *reinterpret_cast<unsigned int *>(payload + 7) = pItem->m_dwLv;

  unsigned __int8 type[2] = {24, 15};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 0x0Bu);
}

void CPlayer::SendMsg_NpcQuestListResult(_NPCQuestIndexTempData *pQuestIndexData)
{

  _npc_quest_list_result_zocl packet{};
  packet.byQuestNum = static_cast<unsigned __int8>(pQuestIndexData->nQuestNum);
  const int questCount = std::min<int>(pQuestIndexData->nQuestNum, 30);
  for (int index = 0; index < questCount; ++index)
  {
    packet.QuestIndexList[index] = pQuestIndexData->IndexData[index].dwQuestIndex;
  }

  unsigned __int8 type[2] = {24, 21};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_ResultNpcQuest(char bSucc)
{
  unsigned __int8 type[2] = {24, 18};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &bSucc, 1u);
}

void CPlayer::SendMsg_InsertQuestFailure(char byEventType, unsigned int dwEventIndex, unsigned __int8 byEventNodeIndex)
{
  char szMsg; // [rsp+34h] [rbp-44h] BYREF
  unsigned int dwIndex; // [rsp+35h] [rbp-43h]
  unsigned __int8 byNode; // [rsp+39h] [rbp-3Fh]
  unsigned __int8 pbyType[36]{}; // [rsp+54h] [rbp-24h] BYREF

  szMsg = byEventType;
  dwIndex = dwEventIndex;
  byNode = byEventNodeIndex;
  pbyType[0] = 24;
  pbyType[1] = 4;
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, &szMsg, 6u);
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
  if (quest->byQuestType == 0xFF)
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
  if (npcRace != raceCode && npcRace != 0xFF)
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
  if (npcRace != raceCode && npcRace != 0xFF)
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
  if (questSlot->byQuestType == 0xFF)
  {
    return;
  }

  for (int index = 0; index < 3; ++index)
  {
    if (questSlot->wNum[index] != 0xFFFF)
    {
      return;
    }
  }

  _base_fld *questRecord = CQuestMgr::s_tblQuest->GetRecord(questSlot->wIndex);
  if (!questRecord)
  {
    return;
  }

  const int hasSelectReward = *reinterpret_cast<int *>(&questRecord[13].m_strCode[60]);
  if (bySelectItemSlotIndex == 0xFF)
  {
    if (hasSelectReward)
    {
      return;
    }
  }
  else if (!hasSelectReward)
  {
    return;
  }

  if (bySelectItemSlotIndex != 0xFF)
  {
    const char *rewardCode = reinterpret_cast<const char *>(&questRecord[14]) + 72 * bySelectItemSlotIndex;
    if (!strncmp(rewardCode, "-1", 2u))
    {
      return;
    }
  }

  if (bySelectLinkQuestIndex != 0xFF)
  {
    const char *linkCode = &questRecord[22].m_strCode[64 * static_cast<unsigned __int64>(bySelectItemSlotIndex) + 44];
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
  bool found = false;
  for (int index = 0; index < 30; ++index)
  {
    if (m_NPCQuestIndexTempData.IndexData[index].dwQuestIndex == dwNPCQuestIndex)
    {
      happenIndex = m_NPCQuestIndexTempData.IndexData[index].dwQuestHappenIndex;
      found = true;
      break;
    }
  }
  if (!found)
  {
    return false;
  }

  _happen_event_cont *happenEvent = m_QuestMgr.CheckNPCQuestStartable(
    pszEventCode,
    static_cast<unsigned __int8>(m_Param.GetRaceCode()),
    static_cast<int>(dwNPCQuestIndex),
    happenIndex);
  if (!happenEvent)
  {
    return false;
  }

  if (Emb_StartQuest(0xFFu, happenEvent))
  {
    return true;
  }

  for (int index = 0; index < 3; ++index)
  {
    if (!m_QuestMgr.m_pTempHappenEvent[index].isset())
    {
      std::memcpy(&m_QuestMgr.m_pTempHappenEvent[index], happenEvent, sizeof(_happen_event_cont));
      break;
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

