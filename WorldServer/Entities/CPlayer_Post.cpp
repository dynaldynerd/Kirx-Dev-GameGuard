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

void CPlayer::SendMsg_PostItemGold(char byErrCode)
{
  char payload[1];
  payload[0] = byErrCode;

  unsigned __int8 type[2]{58, 10};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, payload, 1u);
}

void CPlayer::SendMsg_PostDelete(unsigned __int8 byErrCode, unsigned int dwPostSerial)
{
  char payload[5];
  memcpy_0(payload, &dwPostSerial, sizeof(dwPostSerial));
  payload[4] = static_cast<char>(byErrCode);

  unsigned __int8 type[2]{58, 12};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, payload, 5u);
}

void CPlayer::SendMsg_PostReturnConfirm(char byErrCode, unsigned int dwPostSerial)
{
  char payload[5];
  payload[0] = byErrCode;
  memcpy_0(payload + 1, &dwPostSerial, sizeof(dwPostSerial));

  unsigned __int8 type[2]{58, 15};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, payload, 5u);
}

bool CPlayer::UpdateDelPost(unsigned int dwPostSerial, int nIndex)
{
  return m_pUserDB->Update_DelPost(dwPostSerial, static_cast<unsigned int>(nIndex)) != 0;
}

void CPlayer::DelPostData(unsigned int dwIndex)
{
  m_Param.m_PostStorage.DelPostData(dwIndex);
  m_pUserDB->DelPostData(dwIndex);
}

void CPlayer::SortPost(int nNumber)
{
  for (unsigned int index = 0; index < 50; ++index)
  {
    CPostData *post = &m_Param.m_PostStorage.m_PostData[index];
    if (post->GetState() != 255 && post->m_nNumber > nNumber)
    {
      --post->m_nNumber;
      UpdatePost(index);
    }
  }
}

void CPlayer::UpdatePostAddLog(int dwIndex, bool bLog, int nItemKey)
{
  _POSTDATA_DB_BASE *postData = &m_pUserDB->m_AvatorData.dbPostData;
  _POSTDATA_DB_BASE *backupData = &m_pUserDB->m_AvatorData_bk.dbPostData;
  CPostData *post = m_Param.m_PostStorage.GetPostDataFromInx(dwIndex);
  if (!post)
  {
    CPostSystemManager::Instace()->Log("CPlayer::UpdatePostAddLog() : pPost Is Null : Index(%u)", dwIndex);
    return;
  }

  if (post->m_dwPSSerial && post->m_dwPSSerial != postData->dbPost.m_PostList[dwIndex].dwPSSerial)
  {
    CPostSystemManager::Instace()->Log(
      "GetItem Wrong DB Data >> Name:%s >> Inx:%d >> Storage(S:%d) : DB(S:%d)",
      m_Param.GetCharNameW(),
      dwIndex,
      post->m_dwPSSerial,
      postData->dbPost.m_PostList[dwIndex].dwPSSerial);
  }

  const int key = post->m_Key.CovDBKey();
  m_pUserDB->Update_Post(
    dwIndex,
    post->m_dwPSSerial,
    post->m_nNumber,
    post->m_byState,
    key,
    post->m_dwDur,
    post->m_dwUpt,
    post->m_dwGold,
    post->m_lnUID);

  if (bLog
      && !postData->dbPost.m_PostList[dwIndex].bNew
      && postData->dbPost.m_PostList[dwIndex].dwPSSerial
      && postData->dbPost.m_PostList[dwIndex].nKey == backupData->dbPost.m_PostList[dwIndex].nKey)
  {
    CPostSystemManager::Instace()->Log(
      "GetItem KeyError >> Name:%s >> INX(%d) >> ORIGIN(%d) : NEW(%d) PS(%u) : OLD(%d) PS(%u)",
      m_Param.GetCharNameW(),
      dwIndex,
      nItemKey,
      postData->dbPost.m_PostList[dwIndex].nKey,
      postData->dbPost.m_PostList[dwIndex].dwPSSerial,
      backupData->dbPost.m_PostList[dwIndex].nKey,
      backupData->dbPost.m_PostList[dwIndex].dwPSSerial);

    if (postData->dbPost.m_PostList[dwIndex].dwPSSerial == backupData->dbPost.m_PostList[dwIndex].dwPSSerial)
    {
      CPostSystemManager::Instace()->Log("And Post Serial Is Equal!");
    }
    else
    {
      bool found = false;
      for (int j = 0; j < 50; ++j)
      {
        if (postData->dbPost.m_PostList[dwIndex].dwPSSerial == backupData->dbPost.m_PostList[j].dwPSSerial)
        {
          CPostSystemManager::Instace()->Log(
            "And Post DB Base Index Wrong >> Name:%s >> NEW : INX(%d) PS(%u) >> OLD : INX(%d) PS(%u)",
            m_Param.GetCharNameW(),
            dwIndex,
            postData->dbPost.m_PostList[dwIndex].dwPSSerial,
            j,
            backupData->dbPost.m_PostList[j].dwPSSerial);
          found = true;
          break;
        }
      }

      if (!found)
      {
        CPostSystemManager::Instace()->Log("And Matched Serial Is None!");
      }
    }
  }
}

bool CPlayer::IsReturnPostUpdate()
{
  return m_pUserDB->IsReturnPostUpdate();
}

void CPlayer::UpdateReturnPost(unsigned int dwSerial)
{
  m_pUserDB->Update_ReturnPost(dwSerial);
}

void CPlayer::pc_PostListRequest()
{
  _qry_case_post_storage_list_get qry{};
  qry.dwMasterSerial = m_pUserDB->m_dwSerial;
  const int size = static_cast<int>(qry.size());
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x4Fu, reinterpret_cast<char *>(&qry), size);
}

void CPlayer::pc_PostContentRequest(unsigned int dwIndex)
{
  CPostData *post = m_Param.m_PostStorage.GetPostDataFromInx(dwIndex);
  if (post)
  {
    if (post->GetState() != 1)
    {
      post->SetState(1u);
      UpdatePost(dwIndex);
    }
    if (m_Param.m_PostStorage.IsContentLoad(dwIndex))
    {
      SendMsg_PostContent(
        0,
        dwIndex,
        post->m_wszContent,
        post->m_Key.byTableCode,
        post->m_Key.wItemIndex,
        post->m_dwDur,
        post->m_dwUpt,
        post->m_dwGold);
    }
    else
    {
      _qry_case_post_content_get qry{};
      qry.dwMasterSerial = m_pUserDB->m_dwSerial;
      qry.dwSerial = post->m_dwPSSerial;
      qry.dwIndex = dwIndex;
      const int size = static_cast<int>(qry.size());
      g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x52u, reinterpret_cast<char *>(&qry), size);
    }
  }
  else
  {
    SendMsg_PostItemGold(0xBu);
  }
}

void CPlayer::pc_PostDeleteRequest(unsigned int dwIndex)
{
  CPostData *post = m_Param.m_PostStorage.GetPostDataFromInx(dwIndex);
  if (post)
  {
    if (UpdateDelPost(post->m_dwPSSerial, dwIndex))
    {
      if (post->m_Key.IsFilled() || post->m_dwGold)
      {
        CPlayer::s_MgrItemHistory.post_delete(post, m_szItemHistoryFileName);
      }
      const int number = post->m_nNumber;
      DelPostData(dwIndex);
      SortPost(number);
      SendMsg_PostDelete(0, dwIndex);
    }
    else
    {
      SendMsg_PostDelete(8u, dwIndex);
    }
  }
  else
  {
    SendMsg_PostDelete(0xBu, dwIndex);
  }
}

void CPlayer::pc_PostItemGoldRequest(unsigned int dwIndex)
{
  CPostData *post = m_Param.m_PostStorage.GetPostDataFromInx(dwIndex);
  if (!post)
  {
    SendMsg_PostItemGold(0xBu);
    return;
  }

  bool hasItem = false;
  int itemKey = -1;
  _STORAGE_LIST::_db_con *addedItem = nullptr;
  bool hadChange = false;

  if (post->m_Key.IsFilled() && m_Param.m_dbInven.GetIndexEmptyCon() == 255)
  {
    SendMsg_PostItemGold(0xEu);
    return;
  }

  if (post->m_dwGold)
  {
    const unsigned int money = GetMoney(1u);
    if (post->m_dwGold + money > 0x7A120)
    {
      SendMsg_PostItemGold(0xFu);
      return;
    }
  }

  if (post->m_Key.IsFilled())
  {
    _STORAGE_LIST::_db_con item;
    item.m_byTableCode = post->m_Key.byTableCode;
    item.m_wItemIndex = post->m_Key.wItemIndex;
    item.m_dwDur = post->m_dwDur;
    item.m_dwLv = post->m_dwUpt;
    item.m_lnUID = post->m_lnUID;
    item.m_wSerial = m_Param.GetNewItemSerial();
    addedItem = Emb_AddStorage(0, &item, false, true);
    if (!addedItem)
    {
      SendMsg_PostItemGold(2u);
      return;
    }
    SendMsg_RewardAddItem(addedItem, 5u);
    hasItem = true;
    itemKey = post->m_Key.CovDBKey();
    hadChange = true;
  }

  if (post->m_dwGold)
  {
    AddGold(post->m_dwGold, true);
    SendMsg_AlterMoneyInform(0);
    hadChange = true;
  }

  if (addedItem || post->m_dwGold)
  {
    CPlayer::s_MgrItemHistory.post_getpresent(
      post->m_wszSendName,
      post->m_dwPSSerial,
      addedItem,
      post->m_dwDur,
      post->m_dwGold,
      m_szItemHistoryFileName);
  }

  post->m_Key.SetRelease();
  post->m_dwDur = 0;
  post->m_dwUpt = 0xFFFFFFF;
  post->m_lnUID = 0;
  post->m_dwGold = 0;
  if (hadChange)
  {
    UpdatePostAddLog(dwIndex, hasItem, itemKey);
    SendMsg_PostItemGold(0);
  }
  else
  {
    SendMsg_PostItemGold(0xCu);
  }
}

void CPlayer::pc_PostReturnConfirmRequest(unsigned int dwPostSerial)
{
  if (!dwPostSerial)
  {
    return;
  }

  CPostData *post = m_Param.m_ReturnPostStorage.GetPostDataFromSerial(dwPostSerial);
  if (!post)
  {
    SendMsg_PostReturnConfirm(0xBu, dwPostSerial);
    return;
  }
  if (!IsReturnPostUpdate())
  {
    SendMsg_PostReturnConfirm(8u, dwPostSerial);
    return;
  }

  _STORAGE_LIST::_db_con *addedItem = nullptr;
  if (post->m_Key.IsFilled() && m_Param.m_dbInven.GetIndexEmptyCon() == 255)
  {
    SendMsg_PostReturnConfirm(0xEu, dwPostSerial);
    return;
  }

  const unsigned int money = GetMoney(1u);
  if (post->m_dwGold + money + 5 > 0x7A120)
  {
    SendMsg_PostReturnConfirm(0xFu, dwPostSerial);
    return;
  }

  if (post->m_Key.IsFilled())
  {
    _STORAGE_LIST::_db_con item;
    item.m_byTableCode = post->m_Key.byTableCode;
    item.m_wItemIndex = post->m_Key.wItemIndex;
    item.m_dwDur = post->m_dwDur;
    item.m_dwLv = post->m_dwUpt;
    item.m_lnUID = post->m_lnUID;
    item.m_wSerial = m_Param.GetNewItemSerial();
    addedItem = Emb_AddStorage(0, &item, false, true);
    if (!addedItem)
    {
      SendMsg_PostReturnConfirm(8u, dwPostSerial);
      return;
    }
    SendMsg_RewardAddItem(addedItem, 5u);
  }

  if (post->m_dwGold)
  {
    AddGold(post->m_dwGold, true);
    SendMsg_AlterMoneyInform(0);
  }

  if (addedItem || post->m_dwGold)
  {
    CPlayer::s_MgrItemHistory.post_return(
      post->m_wszRecvName,
      post->m_dwPSSerial,
      addedItem,
      post->m_dwDur,
      post->m_dwGold,
      m_szItemHistoryFileName);
  }

  post->m_Key.SetRelease();
  post->m_dwDur = 0;
  post->m_dwUpt = 0xFFFFFFF;
  post->m_lnUID = 0;
  post->m_dwGold = 0;
  AddGold(5u, true);
  SendMsg_AlterMoneyInform(0);
  UpdateReturnPost(dwPostSerial);
  m_Param.m_ReturnPostStorage.DelPostData(dwPostSerial);
  SendMsg_PostReturnConfirm(0, dwPostSerial);
}

void CPlayer::pc_UpdateDataForPostSend()
{
  _qry_case_update_data_for_post_send qry{};
  qry.dwSerial = m_pUserDB->m_dwSerial;
  qry.dwGlod = m_Param.GetGold();
  qry.pNewData = &m_pUserDB->m_AvatorData;
  qry.pOldData = &m_pUserDB->m_AvatorData_bk;
  const int size = static_cast<int>(qry.size());
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0xAFu, reinterpret_cast<char *>(&qry), size);
}

