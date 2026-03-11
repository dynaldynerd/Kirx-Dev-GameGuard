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
#include "Packet/ZoneClientPacket.h"

#include <ctime>
#include <mmsystem.h>
#include <cstdlib>
#include <cmath>
#include <vector>

void CPlayer::SendMsg_StartShopping()
{
  _start_trade_inform_zocl payload{};
  payload.s = 0;

  unsigned __int8 type[2] = {12, 16};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
}

void CPlayer::SendMsg_RemainTimeInform(__int16 iType, int lRemainTime, _SYSTEMTIME *pstEndDate)
{
  _remaintime_inform_zocl payload{};
  payload.iType = iType;
  payload.lRemainMin = lRemainTime;
  if (pstEndDate)
  {
    payload.stEndDate = *pstEndDate;
  }

  unsigned __int8 type[2] = {29, 1};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
}

void CPlayer::SendMsg_BillingTypeChangeInform(
  __int16 iType,
  int lRemainTime,
  _SYSTEMTIME *pstEndDate,
  unsigned __int8 byReason)
{
  _change_billing_type_inform_zocl payload{};
  payload.iCurrentType = this->m_pUserDB->m_BillingInfo.iType;
  payload.iChangeType = iType;
  payload.lRemainMin = lRemainTime;
  if (pstEndDate)
  {
    payload.stEndDate = *pstEndDate;
  }
  payload.byReason = static_cast<char>(byReason);

  unsigned __int8 type[2] = {29, 3};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
}

void CPlayer::SendMsg_EconomyHistoryInform()
{
  _economy_history_inform_zocl msg{};
  _economy_history_data *history = eGetGuideHistory();

  for (int race = 0; race < 3; ++race)
  {
    for (int index = 0; index < 12; ++index)
    {
      msg.wEconomyGuide[race][index] = history[index].wEconomyGuide[race];
    }
  }

  unsigned __int8 type[2] = {12, 14};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CPlayer::SendMsg_EconomyRateInform(char bStart)
{
_economy_rate_inform_zocl msg{};

  msg.bStart = bStart;
  msg.fPayExgRate = static_cast<float>(eGetRate(this->m_Param.GetRaceCode()));
  msg.fTexRate = eGetTex(this->m_Param.GetRaceCode());
  msg.wMgrValue = static_cast<unsigned __int16>(eGetMgrValue());
  msg.fOreSellRate = eGetOreRate(this->m_Param.GetRaceCode());

  if (!bStart)
  {
    for (int race = 0; race < 3; ++race)
    {
      msg.wEconomyGuide[race] = eGetGuide(race);
    }
  }

  unsigned __int8 type[2] = {12, 15};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 21);
}

void CPlayer::UpdateLastCriTicket(
  unsigned __int16 byCurrentYear,
  unsigned __int8 byCurrentMonth,
  unsigned __int8 byCurrentDay,
  unsigned __int8 byCurrentHour,
  unsigned __int8 byNumOfTime)
{
  this->m_MinigTicket.SetLastCriTicket(byCurrentYear, byCurrentMonth, byCurrentDay, byCurrentHour, byNumOfTime);
  if (this->m_pUserDB)
  {
    const unsigned int lastTicket = this->m_MinigTicket.GetLastCriTicket();
    this->m_pUserDB->Update_TakeLastCriTicket(lastTicket);
  }
}

void CPlayer::pc_TransShipRenewTicketRequest(unsigned __int16 wTicketItemSerial)
{
  unsigned __int8 byErrCode = 0;
  CTransportShip *ship = &g_TransportShip[this->m_Param.GetRaceCode()];
  _STORAGE_LIST::_db_con *pTicketItem = this->m_Param.m_dbInven.GetPtrFromSerial(wTicketItemSerial);

  if (this->m_pCurMap != ship->m_pLinkShipMap
      || !ship->IsMemberBeforeLogoff(this->m_Param.GetCharSerial())
      || !ship->m_bAnchor)
  {
    byErrCode = 1;
  }
  else
  {
    const int level = static_cast<int>(this->GetLevel());
    const int minLevel = ship->GetRideLimLevel();
    const int maxLevel = ship->GetRideUpLimLevel();

    if (level < minLevel || (maxLevel != -1 && level > maxLevel))
    {
      byErrCode = 14;
    }
    else if (!pTicketItem || pTicketItem->m_byTableCode != 28)
    {
      byErrCode = 1;
    }
    else
    {
      _TicketItem_fld *pTicketRecord =
        reinterpret_cast<_TicketItem_fld *>(g_Main.m_tblItemData[28].GetRecord(pTicketItem->m_wItemIndex));
      if (!pTicketRecord || !ship->GetCurRideShipThisTicket(pTicketRecord))
      {
        byErrCode = 1;
      }
    }
  }

  if (!byErrCode)
  {
    byErrCode = static_cast<unsigned __int8>(!ship->RenewOldMember(this));
  }

  this->SendMsg_TransShipRenewTicketResult(static_cast<char>(byErrCode));
}

void CPlayer::SendMsg_TransShipRenewTicketResult(char byErrCode)
{
  _trans_ship_renew_ticket_result_zocl msg{};
  msg.byErrCode = static_cast<unsigned __int8>(byErrCode);

  unsigned __int8 pbyType[2] = {33, 6};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), 1u);
}

void CPlayer::SendMsg_DTradeAskInform(CPlayer *pAsker)
{
  _d_trade_ask_inform_zocl payload{};
  payload.idAsker.wIndex = pAsker->m_ObjID.m_wIndex;
  payload.idAsker.dwSerial = pAsker->m_dwObjSerial;

  unsigned __int8 type[2] = {18, 3};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
}

void CPlayer::SendMsg_DTradeAnswerResult(char byErrCode)
{
  _d_trade_answer_result_zocl payload{};
  payload.byErrCode = byErrCode;

  unsigned __int8 type[2] = {18, 5};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
}

void CPlayer::SendMsg_DTradeStartInform(CPlayer *pAsker, CPlayer *pAnswer, unsigned int *pdwKey)
{
  _d_trade_start_inform_zocl payload{};
  payload.idAsker.wIndex = pAsker->m_ObjID.m_wIndex;
  payload.idAsker.dwSerial = pAsker->m_dwObjSerial;
  payload.byAskerEmptyNum = static_cast<char>(pAsker->m_pmTrd.byEmptyInvenNum);
  payload.idAnswer.wIndex = pAnswer->m_ObjID.m_wIndex;
  payload.idAnswer.dwSerial = pAnswer->m_dwObjSerial;
  payload.byAnswerEmptyNum = static_cast<char>(pAnswer->m_pmTrd.byEmptyInvenNum);
  if (pdwKey)
  {
    std::memcpy(payload.dwKey, pdwKey, sizeof(payload.dwKey));
  }

  unsigned __int8 type[2] = {18, 6};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
}

void CPlayer::SendMsg_DTradeLockResult(char byErrCode)
{
  _d_trade_lock_result_zocl payload{};
  payload.byErrCode = byErrCode;

  unsigned __int8 type[2] = {18, 11};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
}

void CPlayer::SendMsg_DTradeLockInform()
{
  _d_trade_lock_inform_zocl payload{};
  payload.dummy[0] = 0;

  unsigned __int8 type[2] = {18, 12};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
}

void CPlayer::SendMsg_DTradeAddResult(char byErrCode)
{
  _d_trade_add_result_zocl payload{};
  payload.byErrCode = byErrCode;

  unsigned __int8 type[2] = {18, 14};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
}

void CPlayer::SendMsg_DTradeAddInform(char bySlotIndex, _STORAGE_LIST::_db_con *pItem, unsigned __int8 byAmount)
{
  _d_trade_add_inform_zocl payload{};
  payload.bySlotIndex = bySlotIndex;
  payload.byTableCode = static_cast<char>(pItem->m_byTableCode);
  payload.wItemIndex = pItem->m_wItemIndex;
  payload.dwDurPoint = pItem->m_dwDur;
  payload.dwUdtInfo = pItem->m_dwLv;
  payload.byAmount = static_cast<char>(byAmount);
  payload.byEmptyInvenNum = static_cast<char>(this->m_pmTrd.byEmptyInvenNum);
  payload.byCsMethod = static_cast<char>(pItem->m_byCsMethod);
  payload.dwT = pItem->m_dwT;

  unsigned __int8 type[2] = {18, 15};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
}

void CPlayer::SendMsg_DTradeDelResult(char byErrCode)
{
  _d_trade_del_result_zocl payload{};
  payload.byErrCode = byErrCode;

  unsigned __int8 type[2] = {18, 17};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
}

void CPlayer::SendMsg_DTradeDelInform(char bySlotIndex)
{
  _d_trade_del_inform_zocl payload{};
  payload.bySlotIndex = bySlotIndex;
  payload.byEmptyInvenNum = static_cast<char>(this->m_pmTrd.byEmptyInvenNum);

  unsigned __int8 type[2] = {18, 18};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
}

void CPlayer::SendMsg_DTradeBetResult(char byErrCode)
{
  _d_trade_bet_result_zocl payload{};
  payload.byErrCode = byErrCode;

  unsigned __int8 type[2] = {18, 20};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
}

void CPlayer::SendMsg_DTradeBetInform(char byUnitCode, unsigned int dwAmount)
{
  _d_trade_bet_inform_zocl payload{};
  payload.byMoneyUnit = byUnitCode;
  payload.dwBetAmount = dwAmount;

  unsigned __int8 type[2] = {18, 21};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
}

void CPlayer::SendMsg_DTradeUnitInfoInform(char byTradeSlotIndex, _UNIT_DB_BASE::_LIST *pUnitData)
{
  _d_trade_unit_info_inform_zocl payload{};
  payload.byTradeSlotIndex = byTradeSlotIndex;
  payload.byFrame = static_cast<char>(pUnitData->byFrame);
  payload.dwGauge = pUnitData->dwGauge;
  std::memcpy(payload.byPart, pUnitData->byPart, sizeof(payload.byPart));
  std::memcpy(payload.dwBullet, pUnitData->dwBullet, sizeof(payload.dwBullet));
  std::memcpy(payload.dwSpare, pUnitData->dwSpare, sizeof(payload.dwSpare));
  payload.nDebtFee = pUnitData->nPullingFee;

  unsigned __int8 type[2] = {18, 27};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
}

void CPlayer::SendMsg_DTradeUnitAddInform(unsigned __int16 wUnitKeySerial, _UNIT_DB_BASE::_LIST *pUnitData)
{
  _d_trade_unit_add_inform_zocl payload{};
  payload.wUnitKeySerial = wUnitKeySerial;
  payload.bySlotIndex = static_cast<char>(pUnitData->bySlotIndex);
  payload.byFrame = static_cast<char>(pUnitData->byFrame);
  payload.dwGauge = pUnitData->dwGauge;
  std::memcpy(payload.byPart, pUnitData->byPart, sizeof(payload.byPart));
  std::memcpy(payload.dwBullet, pUnitData->dwBullet, sizeof(payload.dwBullet));
  std::memcpy(payload.dwSpare, pUnitData->dwSpare, sizeof(payload.dwSpare));
  payload.nPullingFee = pUnitData->nPullingFee;

  unsigned __int8 type[2] = {18, 28};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
}

void CPlayer::SendMsg_DTradeOKResult(char byErrCode)
{
  _d_trade_ok_result_zocl payload{};
  payload.byErrCode = byErrCode;
  unsigned __int8 type[2] = {18, 23};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
}

void CPlayer::SendMsg_DTradeOKInform()
{
  _d_trade_ok_inform_zocl payload{};
  payload.dummy[0] = 0;
  unsigned __int8 type[2] = {18, 24};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
}

void CPlayer::SendMsg_DTradeAccomplishInform(bool bSucc, unsigned __int16 wStartSerial)
{

  _d_trade_accomplish_inform_zocl packet{};
  packet.dwDalant = this->m_Param.GetDalant();
  packet.dwGold = this->m_Param.GetGold();
  packet.wStartSerial = wStartSerial;
  packet.bSucc = bSucc;

  unsigned __int8 type[2] = {18, 25};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_DTradeAskResult(char byErrCode)
{
  _d_trade_ask_result_zocl payload{};
  payload.byErrCode = byErrCode;
  unsigned __int8 type[2] = {18, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
}

void CPlayer::pc_DTradeCancleRequest()
{
  CPlayer *tradeDst = nullptr;
  const bool differentTradeState = !DTradeEqualPerson(this, &tradeDst);
  if (differentTradeState)
  {
    this->m_pmTrd.Init();
    this->SendMsg_DTradeCloseInform(0);

    if (tradeDst)
    {
      tradeDst->m_pmTrd.Init();
      tradeDst->SendMsg_DTradeCloseInform(0);
    }

    return;
  }

  this->m_pmTrd.Init();
  tradeDst->m_pmTrd.Init();
  this->SendMsg_DTradeCancleResult(static_cast<unsigned __int8>(differentTradeState));
  tradeDst->SendMsg_DTradeCancleInform();
}

void CPlayer::pc_DTradeAskRequest(unsigned __int16 wDstIndex)
{
  unsigned __int8 result = 0;
  CPlayer *dst = &g_Player[wDstIndex];

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(this->m_ObjID.m_wIndex) == 99)
  {
    this->m_pmTrd.bDTradeMode = false;
    dst->m_pmTrd.bDTradeMode = false;
    result = 23;
  }
  else if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(dst->m_ObjID.m_wIndex) == 99)
  {
    this->m_pmTrd.bDTradeMode = false;
    dst->m_pmTrd.bDTradeMode = false;
    result = 24;
  }
  else if (dst->m_bLive
    && dst->m_bOper
    && dst->m_Param.GetRaceCode() == this->m_Param.GetRaceCode()
    && !dst->m_bCorpse
    && dst->m_pCurMap == this->m_pCurMap
    && dst->GetCurSecNum() != static_cast<unsigned int>(-1)
    && dst->m_EP.GetEff_Have(EFF_HAVE_HIDE_NAME) <= 0.0f)
  {
    if (this->m_pmTrd.bDTradeMode || dst->m_pmTrd.bDTradeMode)
    {
      result = 1;
    }
    else if (this->m_pCurMap->m_pMapSet->m_nMapType == 1)
    {
      result = 3;
    }
    else if (this->m_byUserDgr == dst->m_byUserDgr)
    {
      if (GetSqrt(dst->m_fCurPos, this->m_fCurPos) > 100.0f)
      {
        result = 2;
      }
    }
    else
    {
      result = 3;
    }
  }
  else
  {
    result = 3;
  }

  if (!result)
  {
    this->m_pmTrd.wDTradeDstIndex = dst->m_ObjID.m_wIndex;
    this->m_pmTrd.dwDTradeDstSerial = dst->m_dwObjSerial;

    _d_trade_ask_inform_zocl askMsg{};
    askMsg.idAsker.wIndex = this->m_ObjID.m_wIndex;
    askMsg.idAsker.dwSerial = this->m_dwObjSerial;
    unsigned __int8 askType[2] = {18, 3};
    g_Network.m_pProcess[0]->LoadSendMsg(
      dst->m_ObjID.m_wIndex,
      askType,
      reinterpret_cast<char *>(&askMsg),
      static_cast<unsigned __int16>(sizeof(askMsg)));
  }

  this->SendMsg_DTradeAskResult(static_cast<char>(result));
}

void CPlayer::pc_DTradeAnswerRequest(_CLID *pidAsker)
{
  unsigned __int8 result = 0;
  CPlayer *asker = &g_Player[pidAsker->wIndex];

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(this->m_ObjID.m_wIndex) == 99)
  {
    this->m_pmTrd.bDTradeMode = false;
    asker->m_pmTrd.bDTradeMode = false;
    result = 23;
  }
  else if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(asker->m_ObjID.m_wIndex) == 99)
  {
    this->m_pmTrd.bDTradeMode = false;
    asker->m_pmTrd.bDTradeMode = false;
    result = 24;
  }
  else if (asker->m_dwObjSerial != pidAsker->dwSerial)
  {
    result = 1;
  }
  else if (asker->m_pmTrd.wDTradeDstIndex != this->m_ObjID.m_wIndex
    || asker->m_pmTrd.dwDTradeDstSerial != this->m_dwObjSerial)
  {
    result = 4;
  }
  else if (this->m_pmTrd.bDTradeMode || asker->m_pmTrd.bDTradeMode)
  {
    result = 2;
  }
  else if (!(asker->m_bLive
      && asker->m_Param.GetRaceCode() == this->m_Param.GetRaceCode()
      && !asker->m_bCorpse
      && asker->m_pCurMap == this->m_pCurMap
      && asker->GetCurSecNum() != static_cast<unsigned int>(-1)))
  {
    result = 3;
  }
  else if (this->m_pCurMap->m_pMapSet->m_nMapType == 1)
  {
    result = 6;
  }
  else if (this->m_byUserDgr != asker->m_byUserDgr)
  {
    result = 6;
  }
  else if (GetSqrt(asker->m_fCurPos, this->m_fCurPos) > 100.0f)
  {
    result = 5;
  }

  if (result)
  {
    _d_trade_answer_result_zocl answerMsg{};
    answerMsg.byErrCode = static_cast<char>(result);
    unsigned __int8 answerType[2] = {18, 5};
    g_Network.m_pProcess[0]->LoadSendMsg(
      this->m_ObjID.m_wIndex,
      answerType,
      reinterpret_cast<char *>(&answerMsg),
      static_cast<unsigned __int16>(sizeof(answerMsg)));
    return;
  }

  unsigned int codeA[4] = {};
  unsigned int codeB[4] = {};
  for (int index = 0; index < 4; ++index)
  {
    codeA[index] = static_cast<unsigned int>(rand() + (rand() << 16));
    codeB[index] = static_cast<unsigned int>(rand() + (rand() << 16));
  }

  unsigned int *keyA = CalcCodeKey(codeA);
  unsigned int *keyB = CalcCodeKey(codeB);

  auto setTradeStart = [](_DTRADE_PARAM &trade, unsigned __int16 dstIndex, unsigned int dstSerial, unsigned __int8 emptyCount, const unsigned int *key)
  {
    trade.bDTradeMode = true;
    trade.bDTradeLock = false;
    trade.bDTradeOK = false;
    trade.wDTradeDstIndex = dstIndex;
    trade.dwDTradeDstSerial = dstSerial;
    trade.dwDTrade_Dalant = 0;
    trade.dwDTrade_Gold = 0;
    trade.byEmptyInvenNum = emptyCount;
    trade.bySellItemNum = 0;
    for (int slotIndex = 0; slotIndex < 15; ++slotIndex)
    {
      trade.DItemNode[slotIndex].bLoad = false;
    }
    std::memcpy(trade.dwKey, key, sizeof(trade.dwKey));
  };

  setTradeStart(
    this->m_pmTrd,
    asker->m_ObjID.m_wIndex,
    asker->m_dwObjSerial,
    static_cast<unsigned __int8>(this->m_Param.m_dbInven.GetNumEmptyCon()),
    keyA);
  setTradeStart(
    asker->m_pmTrd,
    this->m_ObjID.m_wIndex,
    this->m_dwObjSerial,
    static_cast<unsigned __int8>(asker->m_Param.m_dbInven.GetNumEmptyCon()),
    keyB);


  _d_trade_start_inform_zocl startToThis{};
  startToThis.idAsker.wIndex = asker->m_ObjID.m_wIndex;
  startToThis.idAsker.dwSerial = asker->m_dwObjSerial;
  startToThis.byAskerEmptyNum = asker->m_pmTrd.byEmptyInvenNum;
  startToThis.idAnswer.wIndex = this->m_ObjID.m_wIndex;
  startToThis.idAnswer.dwSerial = this->m_dwObjSerial;
  startToThis.byAnswerEmptyNum = this->m_pmTrd.byEmptyInvenNum;
  std::memcpy(startToThis.dwKey, codeA, sizeof(startToThis.dwKey));

  _d_trade_start_inform_zocl startToAsker{};
  startToAsker.idAsker.wIndex = asker->m_ObjID.m_wIndex;
  startToAsker.idAsker.dwSerial = asker->m_dwObjSerial;
  startToAsker.byAskerEmptyNum = asker->m_pmTrd.byEmptyInvenNum;
  startToAsker.idAnswer.wIndex = this->m_ObjID.m_wIndex;
  startToAsker.idAnswer.dwSerial = this->m_dwObjSerial;
  startToAsker.byAnswerEmptyNum = this->m_pmTrd.byEmptyInvenNum;
  std::memcpy(startToAsker.dwKey, codeB, sizeof(startToAsker.dwKey));

  unsigned __int8 startType[2] = {18, 6};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    startType,
    reinterpret_cast<char *>(&startToThis),
    static_cast<unsigned __int16>(sizeof(startToThis)));
  g_Network.m_pProcess[0]->LoadSendMsg(
    asker->m_ObjID.m_wIndex,
    startType,
    reinterpret_cast<char *>(&startToAsker),
    static_cast<unsigned __int16>(sizeof(startToAsker)));
}

void CPlayer::pc_DTradeLockRequest()
{
  unsigned __int8 result = 0;
  CPlayer *tradeDst = nullptr;
  if (DTradeEqualPerson(this, &tradeDst))
  {
    if (this->m_pmTrd.bDTradeLock)
    {
      result = 2;
    }
    else if (this->m_pCurMap->m_pMapSet->m_nMapType == 1)
    {
      result = 6;
    }
  }
  else
  {
    result = 1;
  }

  if (result)
  {
    this->m_pmTrd.Init();
    this->SendMsg_DTradeCloseInform(0);
    if (tradeDst)
    {
      tradeDst->m_pmTrd.Init();
      tradeDst->SendMsg_DTradeCloseInform(0);
    }
    return;
  }

  this->m_pmTrd.bDTradeLock = true;

  _d_trade_lock_inform_zocl lockInform{};
  lockInform.dummy[0] = 0;
  unsigned __int8 lockInformType[2] = {18, 12};
  g_Network.m_pProcess[0]->LoadSendMsg(
    tradeDst->m_ObjID.m_wIndex,
    lockInformType,
    reinterpret_cast<char *>(&lockInform),
    static_cast<unsigned __int16>(sizeof(lockInform)));

  _d_trade_lock_result_zocl lockResult{};
  lockResult.byErrCode = 0;
  unsigned __int8 lockResultType[2] = {18, 11};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    lockResultType,
    reinterpret_cast<char *>(&lockResult),
    static_cast<unsigned __int16>(sizeof(lockResult)));
}

void CPlayer::pc_DTradeAddRequest(
  unsigned __int8 bySlotIndex,
  unsigned __int8 byStorageCode,
  unsigned int dwSerial,
  unsigned __int8 byAmount)
{
  unsigned __int8 result = 0;
  _STORAGE_LIST::_db_con *item = nullptr;
  CPlayer *tradeDst = nullptr;

  if (DTradeEqualPerson(this, &tradeDst))
  {
    if (this->m_pCurMap->m_pMapSet->m_nMapType == 1)
    {
      result = 6;
    }
    else if (this->m_pmTrd.bDTradeLock)
    {
      result = 2;
    }
    else if (bySlotIndex < 15u)
    {
      if (this->m_pmTrd.DItemNode[bySlotIndex].bLoad)
      {
        result = 4;
      }
      else
      {
        item = this->m_Param.m_pStoragePtr[byStorageCode]->GetPtrFromSerial(dwSerial);
        if (!item)
        {
          result = 5;
        }
        else if (item->m_bLock)
        {
          result = 9;
        }
        else if (item->m_byTableCode == 19)
        {
          result = 7;
        }
        else if (!IsExchangeItem(item->m_byTableCode, item->m_wItemIndex))
        {
          result = 10;
        }
        else if (IsOverLapItem(item->m_byTableCode) && (byAmount == 0 || byAmount > item->m_dwDur))
        {
          result = 6;
        }
        else
        {
          for (int slotIndex = 0; slotIndex < 15; ++slotIndex)
          {
            if (this->m_pmTrd.DItemNode[slotIndex].bLoad && this->m_pmTrd.DItemNode[slotIndex].dwSerial == dwSerial)
            {
              result = 6;
              break;
            }
          }
        }
      }
    }
    else
    {
      result = 3;
    }
  }
  else
  {
    result = 1;
  }

  if (result)
  {
    this->m_pmTrd.Init();
    this->SendMsg_DTradeCloseInform(0);
    if (tradeDst)
    {
      tradeDst->m_pmTrd.Init();
      tradeDst->SendMsg_DTradeCloseInform(0);
    }
    return;
  }

  if (byStorageCode == 0)
  {
    if (IsOverLapItem(item->m_byTableCode))
    {
      if (item->m_dwDur == byAmount)
      {
        ++this->m_pmTrd.byEmptyInvenNum;
      }
    }
    else
    {
      ++this->m_pmTrd.byEmptyInvenNum;
    }
  }

  ++this->m_pmTrd.bySellItemNum;
  _DTRADE_ITEM &tradeItem = this->m_pmTrd.DItemNode[bySlotIndex];
  tradeItem.bLoad = true;
  tradeItem.byStorageCode = byStorageCode;
  tradeItem.dwSerial = dwSerial;
  tradeItem.byAmount = byAmount;

  _d_trade_add_inform_zocl inform{};
  inform.bySlotIndex = static_cast<char>(bySlotIndex);
  inform.byTableCode = static_cast<char>(item->m_byTableCode);
  inform.wItemIndex = item->m_wItemIndex;
  inform.dwDurPoint = item->m_dwDur;
  inform.dwUdtInfo = item->m_dwLv;
  inform.byAmount = static_cast<char>(byAmount);
  inform.byEmptyInvenNum = static_cast<char>(this->m_pmTrd.byEmptyInvenNum);
  inform.byCsMethod = static_cast<char>(item->m_byCsMethod);
  inform.dwT = item->m_dwT;
  unsigned __int8 informType[2] = {18, 15};
  g_Network.m_pProcess[0]->LoadSendMsg(
    tradeDst->m_ObjID.m_wIndex,
    informType,
    reinterpret_cast<char *>(&inform),
    static_cast<unsigned __int16>(sizeof(inform)));

  _d_trade_add_result_zocl addResult{};
  addResult.byErrCode = 0;
  unsigned __int8 addResultType[2] = {18, 14};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    addResultType,
    reinterpret_cast<char *>(&addResult),
    static_cast<unsigned __int16>(sizeof(addResult)));
}

void CPlayer::pc_DTradeDelRequest(unsigned __int8 bySlotIndex)
{
  unsigned __int8 result = 0;
  CPlayer *tradeDst = nullptr;
  if (DTradeEqualPerson(this, &tradeDst))
  {
    if (this->m_pCurMap->m_pMapSet->m_nMapType == 1)
    {
      result = 6;
    }
    else if (this->m_pmTrd.bDTradeLock)
    {
      result = 2;
    }
    else if (bySlotIndex < 15u)
    {
      if (!this->m_pmTrd.DItemNode[bySlotIndex].bLoad)
      {
        result = 4;
      }
    }
    else
    {
      result = 3;
    }
  }
  else
  {
    result = 1;
  }

  if (result)
  {
    this->m_pmTrd.Init();
    this->SendMsg_DTradeCloseInform(0);
    if (tradeDst)
    {
      tradeDst->m_pmTrd.Init();
      tradeDst->SendMsg_DTradeCloseInform(0);
    }
    return;
  }

  _DTRADE_ITEM &tradeItem = this->m_pmTrd.DItemNode[bySlotIndex];
  if (tradeItem.byStorageCode == 0)
  {
    _STORAGE_LIST::_db_con *item = this->m_Param.m_pStoragePtr[tradeItem.byStorageCode]->GetPtrFromSerial(tradeItem.dwSerial);
    if (!item)
    {
      return;
    }

    if (IsOverLapItem(item->m_byTableCode))
    {
      if (item->m_dwDur == tradeItem.byAmount)
      {
        --this->m_pmTrd.byEmptyInvenNum;
      }
    }
    else
    {
      --this->m_pmTrd.byEmptyInvenNum;
    }
  }

  --this->m_pmTrd.bySellItemNum;
  tradeItem.bLoad = false;

  _d_trade_del_inform_zocl delInform{};
  delInform.bySlotIndex = static_cast<char>(bySlotIndex);
  delInform.byEmptyInvenNum = static_cast<char>(this->m_pmTrd.byEmptyInvenNum);
  unsigned __int8 delInformType[2] = {18, 18};
  g_Network.m_pProcess[0]->LoadSendMsg(
    tradeDst->m_ObjID.m_wIndex,
    delInformType,
    reinterpret_cast<char *>(&delInform),
    static_cast<unsigned __int16>(sizeof(delInform)));

  _d_trade_del_result_zocl delResult{};
  delResult.byErrCode = 0;
  unsigned __int8 delResultType[2] = {18, 17};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    delResultType,
    reinterpret_cast<char *>(&delResult),
    static_cast<unsigned __int16>(sizeof(delResult)));
}

void CPlayer::pc_DTradeBetRequest(unsigned __int8 byMoneyUnit, unsigned int dwBetAmount)
{
  unsigned __int8 result = 0;
  CPlayer *tradeDst = nullptr;
  if (DTradeEqualPerson(this, &tradeDst))
  {
    if (this->m_pmTrd.bDTradeLock)
    {
      result = 4;
    }
    else if (this->m_pCurMap->m_pMapSet->m_nMapType == 1)
    {
      result = 6;
    }
    else if (byMoneyUnit)
    {
      if (dwBetAmount > this->m_Param.GetGold())
      {
        result = 2;
      }
    }
    else if (dwBetAmount > this->m_Param.GetDalant())
    {
      result = 1;
    }
  }
  else
  {
    result = 1;
  }

  if (result)
  {
    this->m_pmTrd.Init();
    this->SendMsg_DTradeCloseInform(0);
    if (tradeDst)
    {
      tradeDst->m_pmTrd.Init();
      tradeDst->SendMsg_DTradeCloseInform(0);
    }
    return;
  }

  if (byMoneyUnit)
  {
    this->m_pmTrd.dwDTrade_Gold = dwBetAmount;
  }
  else
  {
    this->m_pmTrd.dwDTrade_Dalant = dwBetAmount;
  }

  _d_trade_bet_inform_zocl betInform{};
  betInform.byMoneyUnit = static_cast<char>(byMoneyUnit);
  betInform.dwBetAmount = dwBetAmount;
  unsigned __int8 betInformType[2] = {18, 21};
  g_Network.m_pProcess[0]->LoadSendMsg(
    tradeDst->m_ObjID.m_wIndex,
    betInformType,
    reinterpret_cast<char *>(&betInform),
    static_cast<unsigned __int16>(sizeof(betInform)));

  _d_trade_bet_result_zocl betResult{};
  betResult.byErrCode = 0;
  unsigned __int8 betResultType[2] = {18, 20};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    betResultType,
    reinterpret_cast<char *>(&betResult),
    static_cast<unsigned __int16>(sizeof(betResult)));
}

void CPlayer::pc_UpdateDataForTrade(CPlayer *pTrader)
{
  if (!this->m_pUserDB || !pTrader || !pTrader->m_pUserDB)
  {
    return;
  }

  _qry_case_update_data_for_trade query;
  query.tradelist[0].dwSerial = this->m_pUserDB->m_dwSerial;
  query.tradelist[0].dwDalant = this->m_Param.GetDalant();
  query.tradelist[0].dwGlod = this->m_Param.GetGold();
  query.tradelist[0].pNewData = &this->m_pUserDB->m_AvatorData;
  query.tradelist[0].pOldData = &this->m_pUserDB->m_AvatorData_bk;
  query.tradelist[1].dwSerial = pTrader->m_pUserDB->m_dwSerial;
  query.tradelist[1].dwDalant = pTrader->m_Param.GetDalant();
  query.tradelist[1].dwGlod = pTrader->m_Param.GetGold();
  query.tradelist[1].pNewData = &pTrader->m_pUserDB->m_AvatorData;
  query.tradelist[1].pOldData = &pTrader->m_pUserDB->m_AvatorData_bk;

  g_Main.PushDQSData(
    -1,
    nullptr,
    176,
    reinterpret_cast<char *>(&query),
    static_cast<int>(query.size()));
}

void CPlayer::pc_DTradeOKRequest(unsigned int *pdwKey)
{
  if (!this->m_pUserDB)
  {
    return;
  }

  unsigned __int8 resultCode = 0;
  CPlayer *tradeDst = nullptr;
  _STORAGE_LIST::_db_con tradeItems[2][15] = {};
  int tradedItemCount[2] = {0, 0};
  CPlayer *players[2] = {this, nullptr};

  auto closeTrade = [&]() {
    this->m_pmTrd.Init();
    this->SendMsg_DTradeCloseInform(0);
    if (tradeDst)
    {
      tradeDst->m_pmTrd.Init();
      tradeDst->SendMsg_DTradeCloseInform(0);
    }
  };

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(this->m_id.wIndex) == 99)
  {
    resultCode = 23;
  }
  else if (!DTradeEqualPerson(this, &tradeDst))
  {
    resultCode = 1;
  }
  else if (this->m_pCurMap->m_pMapSet->m_nMapType == 1)
  {
    resultCode = 6;
  }
  else if (!(this->m_pmTrd.bDTradeLock && tradeDst->m_pmTrd.bDTradeLock))
  {
    resultCode = 2;
  }
  else if (this->m_pmTrd.bDTradeOK)
  {
    resultCode = 3;
  }
  else if (std::memcmp(this->m_pmTrd.dwKey, pdwKey, sizeof(this->m_pmTrd.dwKey)) != 0)
  {
    resultCode = 13;
  }
  else if (this->m_pmTrd.bySellItemNum > tradeDst->m_pmTrd.byEmptyInvenNum)
  {
    resultCode = 100;
  }
  else
  {
    players[1] = tradeDst;
    for (int side = 0; side < 2; ++side)
    {
      CPlayer *current = players[side];
      if (current->m_pmTrd.dwDTrade_Dalant > current->m_Param.GetDalant()
          || current->m_pmTrd.dwDTrade_Gold > current->m_Param.GetGold())
      {
        resultCode = 5;
        break;
      }

      if (current->m_pmTrd.dwDTrade_Dalant > MAX_DALANT
          || current->m_pmTrd.dwDTrade_Gold > MAX_GOLD)
      {
        resultCode = 5;
        break;
      }
    }

    if (!resultCode && tradeDst->m_pmTrd.bDTradeOK)
    {
      for (int side = 0; side < 2; ++side)
      {
        CPlayer *current = players[side];
        CPlayer *other = players[(side + 1) & 1];

        const double dalantDelta =
          static_cast<double>(other->m_pmTrd.dwDTrade_Dalant)
          - static_cast<double>(current->m_pmTrd.dwDTrade_Dalant);
        if (dalantDelta > 0.0)
        {
          if (!CanAddMoneyForMaxLimMoney(static_cast<unsigned int>(dalantDelta), current->m_Param.GetDalant()))
          {
            resultCode = 6;
            break;
          }
        }

        const double goldDelta =
          static_cast<double>(other->m_pmTrd.dwDTrade_Gold)
          - static_cast<double>(current->m_pmTrd.dwDTrade_Gold);
        if (goldDelta > 0.0)
        {
          if (!CanAddMoneyForMaxLimGold(static_cast<unsigned int>(goldDelta), current->m_Param.GetGold()))
          {
            resultCode = 6;
            break;
          }
        }

        int copiedCount = 0;
        for (int tradeSlot = 0; tradeSlot < 15; ++tradeSlot)
        {
          _DTRADE_ITEM *tradeItem = &current->m_pmTrd.DItemNode[tradeSlot];
          if (!tradeItem->bLoad)
          {
            continue;
          }

          _STORAGE_LIST *storage = current->m_Param.m_pStoragePtr[tradeItem->byStorageCode];
          _STORAGE_LIST::_db_con *source = storage->GetPtrFromSerial(static_cast<unsigned __int16>(tradeItem->dwSerial));
          if (!source)
          {
            resultCode = 4;
            break;
          }

          if (source->m_bLock || source->m_byTableCode == 19)
          {
            resultCode = 9;
            break;
          }

          std::memcpy(&tradeItems[side][copiedCount], source, sizeof(_STORAGE_LIST::_db_con));
          if (IsOverLapItem(source->m_byTableCode) && source->m_dwDur > tradeItem->byAmount)
          {
            tradeItems[side][copiedCount].m_dwDur = tradeItem->byAmount;
          }
          ++copiedCount;
        }

        tradedItemCount[side] = copiedCount;
        if (resultCode)
        {
          break;
        }
      }
    }
  }

  if (resultCode)
  {
    closeTrade();
    return;
  }

  this->SendMsg_DTradeOKResult(0);
  tradeDst->SendMsg_DTradeOKInform();
  this->m_pmTrd.bDTradeOK = true;

  if (!(tradeDst->m_pmTrd.bDTradeOK && this->m_pmTrd.bDTradeOK))
  {
    return;
  }

  players[0] = this;
  players[1] = tradeDst;
  for (int side = 0; side < 2; ++side)
  {
    const int oppositeSide = (side + 1) & 1;
    CPlayer *current = players[side];
    CPlayer *other = players[oppositeSide];

    for (int itemIndex = 0; itemIndex < tradedItemCount[side]; ++itemIndex)
    {
      _STORAGE_LIST::_db_con &item = tradeItems[side][itemIndex];
      if (!item.m_pInList)
      {
        resultCode = 4;
        break;
      }

      const unsigned __int8 storageCode = static_cast<unsigned __int8>(item.m_pInList->m_nListCode);
      if (IsOverLapItem(item.m_byTableCode))
      {
        current->Emb_AlterDurPoint(
          storageCode,
          item.m_byStorageIndex,
          -static_cast<int>(item.m_dwDur),
          true,
          false);
      }
      else if (!current->Emb_DelStorage(
                 storageCode,
                 item.m_byStorageIndex,
                 false,
                 true,
                 "CPlayer::pc_DTradeOKRequest()"))
      {
        if (side == 0 && tradedItemCount[0] > 0)
        {
          for (int restored = 0; restored < itemIndex; ++restored)
          {
            current->Emb_AddStorage(0, &tradeItems[side][restored], false, true);
            current->SendMsg_RewardAddItem(&tradeItems[side][restored], 0);
          }
          closeTrade();
          return;
        }
      }
    }

    unsigned __int16 firstReceivedSerial = static_cast<unsigned __int16>(-1);
    int addedIncomingCount = 0;
    bool addFailed = false;

    for (int itemIndex = 0; itemIndex < tradedItemCount[oppositeSide]; ++itemIndex)
    {
      _STORAGE_LIST::_db_con &incoming = tradeItems[oppositeSide][itemIndex];
      incoming.m_wSerial = current->m_Param.GetNewItemSerial();
      if (firstReceivedSerial == static_cast<unsigned __int16>(-1))
      {
        firstReceivedSerial = incoming.m_wSerial;
      }

      _STORAGE_LIST::_db_con *added =
        current->Emb_AddStorage(0, &incoming, false, true);
      if (!added)
      {
        if (side == 0 && tradedItemCount[oppositeSide] > 0)
        {
          addFailed = true;
          break;
        }
        continue;
      }

      incoming.m_byStorageIndex = added->m_byStorageIndex;
      ++addedIncomingCount;
    }

    if (addFailed)
    {
      for (int removed = 0; removed < addedIncomingCount; ++removed)
      {
        current->Emb_DelStorage(
          0,
          tradeItems[oppositeSide][removed].m_byStorageIndex,
          false,
          true,
          "CPlayer::pc_DTradeOKRequest() - Emb_Add Item Fail");
      }

      for (int restored = 0; restored < tradedItemCount[side]; ++restored)
      {
        current->Emb_AddStorage(0, &tradeItems[side][restored], false, true);
        current->SendMsg_RewardAddItem(&tradeItems[side][restored], 0);
      }

      closeTrade();
      return;
    }

    double dalantDelta =
      static_cast<double>(other->m_pmTrd.dwDTrade_Dalant)
      - static_cast<double>(current->m_pmTrd.dwDTrade_Dalant);
    double goldDelta =
      static_cast<double>(other->m_pmTrd.dwDTrade_Gold)
      - static_cast<double>(current->m_pmTrd.dwDTrade_Gold);

    const double playerPenalty =
      g_Main.m_pTimeLimitMgr->GetPlayerPenalty(this->m_id.wIndex);
    dalantDelta *= playerPenalty;
    goldDelta *= playerPenalty;

    current->AlterDalant(dalantDelta);
    current->AlterGold(goldDelta);
    current->SendMsg_DTradeAccomplishInform(true, firstReceivedSerial);
  }

  this->pc_UpdateDataForTrade(tradeDst);
  g_Main.m_logDTrade.Write(
    "[%d]%s(%d): D(%d) G(%d) # [%d]%s(%d): D(%d) G(%d)",
    static_cast<int>(this->GetLevel()),
    this->m_Param.GetCharNameA(),
    this->m_dwObjSerial,
    this->m_pmTrd.dwDTrade_Dalant,
    this->m_pmTrd.dwDTrade_Gold,
    static_cast<int>(tradeDst->GetLevel()),
    tradeDst->m_Param.GetCharNameA(),
    tradeDst->m_dwObjSerial,
    tradeDst->m_pmTrd.dwDTrade_Dalant,
    tradeDst->m_pmTrd.dwDTrade_Gold);

  this->m_pmTrd.Init();
  tradeDst->m_pmTrd.Init();
}

void CPlayer::SendMsg_DTradeCancleResult(char byErrCode)
{
  _d_trade_cancle_result_zocl msg{};
  msg.byErrCode = byErrCode;
  unsigned __int8 type[2] = {18, 8};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

