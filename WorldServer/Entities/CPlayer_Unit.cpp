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
#include "SiegeKitItem_fld.h"
#include "UnitKeyItem_fld.h"
#include "TrapItem_fld.h"
#include "UnitFrame_fld.h"
#include "UnitPart_fld.h"
#include "WeaponItem_fld.h"
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

unsigned __int16 CPlayer::_DeleteUnitKey(unsigned __int8 bySlotIndex)
{
  for (int index = 0; index < m_Param.m_dbInven.m_nUsedNum; ++index)
  {
    _STORAGE_LIST::_db_con *item = &m_Param.m_dbInven.m_pStorageList[index];
    if (item->m_bLoad && item->m_dwLv == bySlotIndex)
    {
      const unsigned __int16 serial = item->m_wSerial;
      if (Emb_DelStorage(0, static_cast<unsigned __int8>(index), false, true, "CPlayer::_DeleteUnitKey()"))
      {
        return serial;
      }
      return static_cast<unsigned __int16>(-1);
    }
  }
  return static_cast<unsigned __int16>(-1);
}

CParkingUnit *FindEmptyParkingUnit(CParkingUnit *parkingUnits, int maxCount)
{
  const unsigned int now = timeGetTime();
  for (int index = 0; index < maxCount; ++index)
  {
    CParkingUnit *slot = &parkingUnits[index];
    if (!slot->m_bLive && now - slot->m_dwLastDestroyTime > 30000u)
    {
      return slot;
    }
  }
  return nullptr;
}

_UnitKeyItem_fld *gGetUnitKeyMatchFrame(unsigned __int8 byFrameCode)
{
  const int recordNum = g_Main.m_tblItemData[19].GetRecordNum();
  for (int index = 0; index < recordNum; ++index)
  {
    _UnitKeyItem_fld *record = static_cast<_UnitKeyItem_fld *>(g_Main.m_tblItemData[19].GetRecord(index));
    if (record && record->m_nFRAType == byFrameCode)
    {
      return record;
    }
  }
  return nullptr;
}

void CPlayer::SendMsg_UnitReturnResult(char byRetCode, unsigned int dwPayDalant)
{
  _unit_return_result_zocl packet{};
  packet.byRetCode = byRetCode;
  packet.dwPayDalant = dwPayDalant;
  packet.dwNewDalant = this->m_Param.GetDalant();

  unsigned __int8 type[2] = {23, 16};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), 9u);
}

void CPlayer::SendMsg_UnitBulletReplaceResult(char byRetCode)
{
  _unit_bullet_replace_result_zocl packet{};
  packet.byRetCode = byRetCode;

  unsigned __int8 type[2] = {23, 24};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), 1u);
}

void CPlayer::SendMsg_UnitDestroy(char bySlotIndex)
{
  _unit_destroy_inform_zocl packet{};
  packet.bySlotIndex = bySlotIndex;

  unsigned __int8 type[2] = {23, 26};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), 1u);
}

void CPlayer::_UnitDestroy(unsigned __int8 byUnitSlot)
{
  if (byUnitSlot >= 4u)
  {
    return;
  }

  _UNIT_DB_BASE::_LIST *unitData = &m_Param.m_UnitDB.m_List[byUnitSlot];
  if (unitData->byFrame == static_cast<unsigned __int8>(-1))
  {
    return;
  }

  _UnitFrame_fld *record = static_cast<_UnitFrame_fld *>(g_Main.m_tblUnitFrame.GetRecord(unitData->byFrame));
  if (record
      && (!record->m_bRepair || record->m_bDestroy))
  {
    unitData->Init(255);
    _DeleteUnitKey(byUnitSlot);
    if (m_pUserDB)
    {
      m_pUserDB->Update_UnitDelete(byUnitSlot);
    }
  }
  else
  {
    unitData->dwGauge = 0;
    if (m_pUserDB)
    {
      m_pUserDB->Update_UnitData(byUnitSlot, unitData);
    }
    if (m_pUsingUnit)
    {
      _LockUnitKey(m_pUsingUnit->bySlotIndex, false);
    }
  }

  if (m_pUsingUnit == unitData)
  {
    m_pUsingUnit = nullptr;
    m_pParkingUnit = nullptr;
  }

  SendMsg_UnitDestroy(static_cast<char>(byUnitSlot));
}

void CPlayer::Emb_RidindUnit(bool bRiding, CParkingUnit *pCreateUnit)
{
  CashChangeStateFlag changeFlag(0);
  UpdateVisualVer(changeFlag);

  if (bRiding)
  {
    SendMsg_UnitRideChange(true, m_pParkingUnit);
    m_pParkingUnit->Destroy(1u);
    m_pParkingUnit = nullptr;
    m_EP.SetLock(true);
    m_pmWpn.FixUnit(m_pUsingUnit);

    _STORAGE_LIST::_db_con *weapon = m_Param.m_dbEquip.m_pStorageList + 6;
    if (weapon->m_bLoad)
    {
      m_pmWpn.FixWeapon(weapon);
    }

    m_nUnitDefFc = 0;

    for (int j = 0; j < 6; ++j)
    {
      if (m_pUsingUnit->byPart[j] != static_cast<unsigned __int8>(-1))
      {
        _UnitPart_fld *record = static_cast<_UnitPart_fld *>(g_Main.m_tblUnitPart[j].GetRecord(m_pUsingUnit->byPart[j]));
        if (record)
        {
          m_nUnitDefFc += record->m_nDefFc;
        }
      }
    }

    for (int inventoryIndex = 0; inventoryIndex < static_cast<int>(m_Param.GetUseSlot()); ++inventoryIndex)
    {
      _STORAGE_LIST::_db_con *item = &m_Param.m_dbInven.m_pStorageList[inventoryIndex];
      if (item->m_bLoad)
      {
        if (item->m_wItemIndex == 328)
        {
          m_bGeneratorAttack = true;
        }
        else if (item->m_wItemIndex == 329)
        {
          m_bGeneratorDefense = true;
        }
      }
    }

    if (weapon->m_bLoad)
    {
      m_byUnitEffectAttackStep = GetItemUpgedLv(weapon->m_dwLv);
      if (!m_byUnitEffectAttackStep || m_byUnitEffectAttackStep > 7u)
      {
        m_byUnitEffectAttackStep = 0;
      }
    }

    char totalDefenseStep = 0;
    for (int equipmentIndex = 0; equipmentIndex < 4; ++equipmentIndex)
    {
      _STORAGE_LIST::_db_con *equip = &m_Param.m_dbEquip.m_pStorageList[equipmentIndex];
      if (equip->m_bLoad)
      {
        totalDefenseStep += static_cast<char>(GetItemUpgedLv(equip->m_dwLv));
      }
    }

    m_byUnitEffectDefenseStep = static_cast<unsigned __int8>(totalDefenseStep);
    if (!m_byUnitEffectDefenseStep || m_byUnitEffectDefenseStep > 35u)
    {
      m_byUnitEffectDefenseStep = 0;
    }
  }
  else
  {
    if (pCreateUnit)
    {
      m_pParkingUnit = pCreateUnit;
      SendMsg_UnitRideChange(false, pCreateUnit);
    }
    else
    {
      _UnitDestroy(m_pUsingUnit->bySlotIndex);
    }

    m_dwUnitViewOverTime = static_cast<unsigned int>(-1);
    m_EP.SetLock(false);

    _STORAGE_LIST::_db_con *weapon = m_Param.m_dbEquip.m_pStorageList + 6;
    if (weapon->m_bLoad)
    {
      m_pmWpn.FixWeapon(weapon);
    }
    else
    {
      m_pmWpn.FixWeapon(nullptr);
    }

    m_bGeneratorAttack = false;
    m_bGeneratorDefense = false;
    m_byUnitEffectAttackStep = 0;
    m_byUnitEffectDefenseStep = 0;
  }

  CalcDefTol();
  SetHaveEffect(0);
  SetShapeAllBuffer();
  CheckAlterMaxPoint();
}

void CPlayer::SendMsg_UnitRideChange(bool bTake, CParkingUnit *pUnit)
{
  _other_unit_ride_change_zocl payload{};
  payload.wIndex = this->m_ObjID.m_wIndex;
  payload.dwSerial = this->m_dwObjSerial;
  payload.wEquipVer = this->GetVisualVer();
  payload.bTake = bTake;
  payload.dwUnitSerial = pUnit->m_dwObjSerial;
  FloatToShort(this->m_fCurPos, payload.zNewPos, 3);

  unsigned __int8 type[2] = {3, 35};
  this->CircleReport(type, reinterpret_cast<char *>(&payload), 19u, false);
}

void CPlayer::SendMsg_UnitFrameBuyResult(char byRetCode, char byFrameCode, char byUnitSlotIndex, unsigned __int16 wKeyIndex, unsigned __int16 wKeySerial, unsigned int *pdwConsumMoney)
{

  _unit_frame_buy_result_zocl packet{};
  packet.byRetCode = byRetCode;
  packet.byFrameCode = byFrameCode;
  packet.byAddUnitSlot = byUnitSlotIndex;
  packet.byKeyIndex = static_cast<unsigned __int8>(wKeyIndex);
  packet.wKeySerial = wKeySerial;
  packet.dwLeftMoney[0] = this->m_Param.GetDalant();
  packet.dwLeftMoney[1] = this->m_Param.GetGold();
  packet.dwConsumMoney[0] = pdwConsumMoney[0];
  packet.dwConsumMoney[1] = pdwConsumMoney[1];

  unsigned __int8 type[2] = {23, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), 0x3Eu);
}

void CPlayer::SendMsg_UnitSellResult(char byRetCode, char bySlotIndex, unsigned __int16 wKeySerial, int nAddMoney, unsigned int dwTotalNonpay, unsigned int dwSumDalant, unsigned int dwSumGold)
{

  _unit_sell_result_zocl packet{};
  packet.byRetCode = byRetCode;
  packet.bySlotIndex = bySlotIndex;
  packet.wKeySerial = wKeySerial;
  packet.dwNonPayDalant = dwTotalNonpay;
  packet.nAddMoney[0] = nAddMoney;
  packet.dwLeftMoney[0] = dwSumDalant;
  packet.dwLeftMoney[1] = dwSumGold;

  unsigned __int8 type[2] = {23, 4};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), 0x40u);
}

void CPlayer::SendMsg_UnitPartTuningResult(char byRetCode, char bySlotIndex, int *pnCost)
{

  _unit_part_tuning_result_zocl packet{};
  packet.byRetCode = byRetCode;
  packet.bySlotIndex = bySlotIndex;
  std::memcpy(packet.byPart, this->m_Param.m_UnitDB.m_List[static_cast<unsigned __int8>(bySlotIndex)].byPart, sizeof(packet.byPart));
  std::memcpy(packet.dwBullet, this->m_Param.m_UnitDB.m_List[static_cast<unsigned __int8>(bySlotIndex)].dwBullet, sizeof(packet.dwBullet));
  packet.nCost[0] = pnCost[0];
  packet.nCost[1] = pnCost[1];
  packet.dwLeftMoney[0] = this->m_Param.GetDalant();
  packet.dwLeftMoney[1] = this->m_Param.GetGold();

  unsigned __int8 type[2] = {23, 6};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), 0x48u);
}

void CPlayer::SendMsg_UnitFrameRepairResult(char byRetCode, char bySlotIndex, unsigned int dwNewGauge, unsigned int dwConsumDalant)
{

  _unit_frame_repair_result_zocl packet{};
  packet.byRetCode = byRetCode;
  packet.bySlotIndex = bySlotIndex;
  packet.dwNewGauge = dwNewGauge;
  packet.dwConsumDalant = dwConsumDalant;
  packet.dwLeftDalant = this->m_Param.GetDalant();

  unsigned __int8 type[2] = {23, 8};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), 0xEu);
}

void CPlayer::SendMsg_UnitBulletFillResult(char byRetCode, char bySlotIndex, unsigned __int16 *pwBulletIndex, unsigned int *pdwConsumMoney)
{

  _unit_bullet_fill_result_zocl packet{};
  packet.byRetCode = byRetCode;
  packet.bySlotIndex = bySlotIndex;
  packet.wBulletIndex[0] = pwBulletIndex[0];
  packet.wBulletIndex[1] = pwBulletIndex[1];
  packet.dwComsumMoney[0] = static_cast<int>(pdwConsumMoney[0]);
  packet.dwComsumMoney[1] = pdwConsumMoney[1];
  packet.dwLeftMoney[0] = this->m_Param.GetDalant();
  packet.dwLeftMoney[1] = this->m_Param.GetGold();

  unsigned __int8 type[2] = {23, 10};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), 0x3Eu);
}

void CPlayer::SendMsg_UnitPackFillResult(unsigned __int8 byRetCode, unsigned __int8 bySlotIndex, unsigned __int8 byFillNum, _unit_pack_fill_request_clzo::__list *pList, unsigned int *pdwConsumMoney)
{

  _unit_pack_fill_result_zocl packet{};
  packet.byRetCode = byRetCode;
  packet.bySlotIndex = bySlotIndex;
  packet.byFillNum = byFillNum;
  if (byFillNum > 0)
  {
    std::memcpy(packet.List, pList, static_cast<size_t>(byFillNum) * sizeof(_unit_pack_fill_request_clzo::__list));
  }
  packet.dwComsumMoney[0] = pdwConsumMoney[0];
  packet.dwComsumMoney[1] = pdwConsumMoney[1];
  packet.dwLeftMoney[0] = this->m_Param.GetDalant();
  packet.dwLeftMoney[1] = this->m_Param.GetGold();

  unsigned __int8 type[2] = {23, 12};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), 0x53u);
}

void CPlayer::SendMsg_UnitDeliveryResult(char byRetCode, char bySlotIndex, unsigned int dwParkingUnitSerial, unsigned int dwPayDalant)
{

  _unit_delivery_result_zocl packet{};
  packet.byRetCode = byRetCode;
  packet.bySlotIndex = bySlotIndex;
  packet.dwParkingUnitSerial = dwParkingUnitSerial;
  packet.dwPayDalant = dwPayDalant;
  packet.dwLeftDalant = this->m_Param.GetDalant();

  unsigned __int8 type[2] = {23, 14};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), 0xFu);
}

void CPlayer::SendMsg_UnitTakeResult(char byRetCode)
{
  _unit_take_result_zocl packet{};
  packet.byRetCode = byRetCode;
  unsigned __int8 type[2] = {23, 18};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), 1u);
}

void CPlayer::SendMsg_UnitLeaveResult(char byRetCode)
{
  _unit_leave_result_zocl packet{};
  packet.byRetCode = byRetCode;
  unsigned __int8 type[2] = {23, 20};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), 1u);
}

void CPlayer::SendMsg_AlterUnitHPInform(char bySlotIndex, unsigned int dwGauge)
{
  _alter_unit_hp_inform_zocl packet{};
  packet.bySlotIndex = bySlotIndex;
  packet.dwLeftHP = dwGauge;

  unsigned __int8 pbyType[2] = {5, 23};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&packet), sizeof(packet));
}

float CPlayer::GetAddSpeed()
{
  const float effectSpeed = this->m_EP.GetEff_Plus(EFF_PLUS_MOVE_RUN_SPEED) * 100.0f;
  const float baseSpeed = static_cast<float>(static_cast<__int16>(static_cast<int>(effectSpeed)));

  if (this->m_byMoveType == 2 && this->EquipItemSFAgent.IsUseBooster())
  {
    const float boosterSpeed = this->EquipItemSFAgent.GetBoosterAddSpeed() * 100.0f;
    return static_cast<float>(static_cast<__int16>(static_cast<int>(boosterSpeed)));
  }

  return baseSpeed;
}

void CPlayer::BreakCloakBooster()
{
  EquipItemSFAgent.ReleaseSFCont(7);
}

bool CPlayer::IsUseCloakBooster()
{
  return this->EquipItemSFAgent.IsUseBooster();
}

void CPlayer::Cheet_BufEffectEnd()
{
  const unsigned int endTime = static_cast<unsigned int>(GetKorLocalTime());
  this->m_PotionBufUse.SetExtPotionEndTime(endTime);
}

void CPlayer::CheckUnitCutTime()
{
  if (!this->m_pUserDB)
  {
    return;
  }

  const unsigned __int64 now = GetKorLocalTime();
  for (int index = 0; index < 4; ++index)
  {
    _UNIT_DB_BASE::_LIST *unit = &this->m_Param.m_UnitDB.m_List[index];
    const unsigned __int64 unitCutTime = this->m_pUserDB->GetCanonicalUnitCutTime(static_cast<unsigned __int8>(index));
    if (unit->byFrame != 255 && unitCutTime && now - unitCutTime < 5)
    {
      unit->nPullingFee = 0;
      SendMsg_UnitAlterFeeInform(index, 0);
      this->m_pUserDB->Update_UnitData(index, unit);

      float newPos[3]{this->m_fCurPos[0], this->m_fCurPos[1], this->m_fCurPos[2]};
      this->m_pCurMap->GetRandPosInRange(this->m_fCurPos, 20, newPos);
      pc_UnitDeliveryRequest(index, nullptr, 0, newPos, 0);
      break;
    }
  }

  for (int index = 0; index < 4; ++index)
  {
    _UNIT_DB_BASE::_LIST *unit = &this->m_Param.m_UnitDB.m_List[index];
    if (unit->byFrame != 255 && this->m_pUserDB->GetCanonicalUnitCutTime(static_cast<unsigned __int8>(index)))
    {
      unit->dwCutTime = 0;
      this->m_pUserDB->SetCanonicalUnitCutTime(static_cast<unsigned __int8>(index), 0);
      this->m_pUserDB->Update_UnitData(index, unit);
    }
  }
}

void CPlayer::SendMsg_AlterBooster()
{
  _unit_booster_charge_inform_zocl packet{};
  packet.bySlotIndex = static_cast<char>(m_pUsingUnit->bySlotIndex);
  packet.wBoosterGauge = m_pUsingUnit->wBooster;

  unsigned __int8 type[2]{23, 25};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), 3u);
}

void CPlayer::AutoCharge_Booster()
{
  if (!m_pUsingUnit)
  {
    return;
  }

  _UnitPart_fld *unitPartRecord = static_cast<_UnitPart_fld *>(g_Main.m_tblUnitPart[5].GetRecord(m_pUsingUnit->byPart[5]));
  if (!unitPartRecord)
  {
    return;
  }

  const int maxBooster = unitPartRecord->m_nBstCha;
  if (!maxBooster)
  {
    return;
  }

  const int currentBooster = m_pUsingUnit->wBooster;
  int delta = 0;
  if (m_bMove && !m_pParkingUnit)
  {
    if (m_byMoveType == 1)
    {
      delta = -2;
    }
  }
  else
  {
    delta = 2;
  }

  if (!delta)
  {
    return;
  }

  int nextBooster = currentBooster + delta;
  if (nextBooster < 0)
  {
    nextBooster = 0;
  }
  else if (nextBooster > maxBooster)
  {
    nextBooster = maxBooster;
  }

  m_pUsingUnit->wBooster = static_cast<unsigned __int16>(nextBooster);
  if (currentBooster != nextBooster)
  {
    SendMsg_AlterBooster();
  }
}

void CPlayer::pc_ReleaseSiegeModeRequest()
{
  unsigned __int8 byRetCode = 0;
  if (!this->IsSiegeMode())
  {
    byRetCode = 11;
  }

  if (byRetCode)
  {
    this->SendMsg_ReleaseSiegeModeResult(static_cast<char>(byRetCode));
    return;
  }

  this->m_bIsSiegeActing = true;
  this->m_tmrSiegeTime.BeginTimer(2000);
  this->SetSiege(nullptr);
}

void CPlayer::pc_TransformSiegeModeRequest(unsigned __int16 wItemSerial)
{
  unsigned __int8 byRetCode = 0;
  _STORAGE_LIST::_db_con *pInvenItem = this->m_Param.m_dbInven.GetPtrFromSerial(wItemSerial);
  _STORAGE_LIST::_db_con *pWeapon = this->m_Param.m_dbEquip.m_pStorageList + 6;

  // Yorozuya fix implementation (non-IDA): prevent jumping into siege kit while moving.
  if (m_bMove)
  {
    byRetCode = 11;
  }

  if (!byRetCode && (!pInvenItem || pInvenItem->m_byTableCode != 27))
  {
    byRetCode = 1;
  }
  else
  {
    _SiegeKitItem_fld *pSiegeRecord =
      static_cast<_SiegeKitItem_fld *>(g_Main.m_tblItemData[27].GetRecord(pInvenItem->m_wItemIndex));
    if (!pSiegeRecord)
    {
      byRetCode = 1;
    }
    else if (this->IsSiegeMode())
    {
      byRetCode = 3;
    }
    else if (!pInvenItem->m_dwDur)
    {
      byRetCode = 4;
    }
    else if (!pWeapon->m_bLoad)
    {
      byRetCode = 5;
    }
    else
    {
      _WeaponItem_fld *pWeaponRecord =
        static_cast<_WeaponItem_fld *>(g_Main.m_tblItemData[6].GetRecord(pWeapon->m_wItemIndex));
      if (!pWeaponRecord)
      {
        byRetCode = 6;
      }
      else if (
        this->m_pmWpn.byWpType != pSiegeRecord->m_nUsableTyoe
        || pWeaponRecord->m_nSubType != pSiegeRecord->m_nSubType)
      {
        byRetCode = 6;
      }
      else if (!IsItemEquipCivil(
                 pInvenItem->m_byTableCode,
                 pInvenItem->m_wItemIndex,
                 static_cast<unsigned __int8>(this->m_Param.GetRaceSexCode())))
      {
        byRetCode = 7;
      }
      else if (!this->IsEffectableEquip(pInvenItem))
      {
        byRetCode = 8;
      }
      else if (this->IsRidingUnit())
      {
        byRetCode = 9;
      }
      else if (pInvenItem->m_bLock)
      {
        byRetCode = 10;
      }
    }
  }

  if (byRetCode)
  {
    this->SendMsg_TransformSiegeModeResult(static_cast<char>(byRetCode));
    return;
  }

  this->m_bIsSiegeActing = true;
  this->m_tmrSiegeTime.BeginTimer(2000);
  this->SetSiege(pInvenItem);
}

void CPlayer::pc_UnitReturnRequest()
{
  unsigned __int8 result = 0;
  bool chargePullFee = true;

  if (this->m_Param.GetRaceCode())
  {
    result = 1;
  }
  else if (this->m_pParkingUnit)
  {
    if (this->m_pParkingUnit->m_dwOwnerSerial == this->m_dwObjSerial)
    {
      if (GetSqrt(this->m_fCurPos, this->m_pParkingUnit->m_fCurPos) <= 540.0f)
      {
        if (IsBeNearStore(this, 4))
        {
          chargePullFee = false;
        }
      }
      else
      {
        result = 20;
      }
    }
    else
    {
      result = 2;
    }
  }
  else
  {
    result = 2;
  }

  unsigned int pullFee = 0;
  if (!result)
  {
    this->m_pParkingUnit->Destroy(0);
    if (chargePullFee)
    {
      pullFee = 1000;
    }
    this->_UpdateUnitDebt(this->m_pUsingUnit->bySlotIndex, pullFee);
    this->_LockUnitKey(this->m_pUsingUnit->bySlotIndex, false);
    this->m_pUsingUnit = nullptr;
    this->m_pParkingUnit = nullptr;
  }

  _unit_return_result_zocl msg{};
  msg.byRetCode = static_cast<char>(result);
  msg.dwPayDalant = pullFee;
  msg.dwNewDalant = this->m_Param.GetDalant();
  unsigned __int8 type[2] = {23, 16};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::pc_UnitBulletReplaceRequest(
  unsigned __int8 bySlotIndex,
  unsigned __int8 byPackIndex,
  unsigned __int8 byBulletPart)
{
  if (!this->m_pUserDB)
  {
    return;
  }

  unsigned __int8 result = 0;
  _UNIT_DB_BASE::_LIST *unitData = &this->m_Param.m_UnitDB.m_List[bySlotIndex];
  if (unitData->byFrame == 255)
  {
    result = 5;
  }
  else if (unitData->dwSpare[byPackIndex] == -1)
  {
    result = 25;
  }

  if (!result)
  {
    unitData->dwBullet[byBulletPart] = unitData->dwSpare[byPackIndex];
    unitData->dwSpare[byPackIndex] = -1;
    this->m_pUserDB->Update_UnitData(bySlotIndex, unitData);
  }

  _unit_bullet_replace_result_zocl msg{};
  msg.byRetCode = static_cast<char>(result);
  unsigned __int8 type[2] = {23, 24};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::pc_UnitFrameBuyRequest(unsigned __int8 byFrameCode, int bUseNPCLinkIntem)
{
  if (!m_pUserDB)
  {
    return;
  }

  unsigned __int8 resultCode = 0;
  unsigned __int8 emptyUnitSlot = static_cast<unsigned __int8>(-1);
  unsigned int consumeMoney[12] = {};
  unsigned __int8 defaultParts[6];
  std::memset(defaultParts, 255, sizeof(defaultParts));

  _UnitFrame_fld *frameRecord = reinterpret_cast<_UnitFrame_fld *>(g_Main.m_tblUnitFrame.GetRecord(byFrameCode));
  _UnitKeyItem_fld *unitKeyRecord = nullptr;

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    resultCode = 34;
  }
  else
  {
    const unsigned int taxRate = eGetTexRate(0) + 10000;
    if (!bUseNPCLinkIntem && !IsBeNearStore(this, 4))
    {
      resultCode = 21;
    }
    else if (!frameRecord)
    {
      resultCode = 9;
    }
    else if (m_Param.GetRaceCode() != 0)
    {
      resultCode = 1;
    }
    else if (m_pUsingUnit)
    {
      resultCode = 2;
    }
    else
    {
      for (int slotIndex = 0; slotIndex < 4; ++slotIndex)
      {
        if (m_Param.m_UnitDB.m_List[slotIndex].byFrame == 255)
        {
          emptyUnitSlot = static_cast<unsigned __int8>(slotIndex);
          break;
        }
      }

      if (emptyUnitSlot == 255)
      {
        resultCode = 3;
      }
      else if (!m_Param.m_dbInven.GetNumEmptyCon())
      {
        resultCode = 4;
      }
      else
      {
        unitKeyRecord = gGetUnitKeyMatchFrame(byFrameCode);
        if (!unitKeyRecord)
        {
          resultCode = 6;
        }
        else
        {
          const int moneyCode = frameRecord->m_nMoney;
          if (moneyCode < 0 || moneyCode >= 7)
          {
            resultCode = 9;
          }
          else
          {
            const unsigned long long rawCost =
              static_cast<unsigned long long>(frameRecord->m_nStdPrice) * static_cast<unsigned long long>(taxRate);
            consumeMoney[moneyCode] = static_cast<unsigned int>(rawCost / 10000u);
            if (consumeMoney[moneyCode] > GetMoney(static_cast<unsigned __int8>(moneyCode)))
            {
              resultCode = 7;
            }
            else
            {
              const char *defaultPartCodes[6] = {
                frameRecord->m_strDFHead,
                frameRecord->m_strDFUpper,
                frameRecord->m_strDFLower,
                frameRecord->m_strDFArms,
                frameRecord->m_strDFShoulder,
                frameRecord->m_strDFBack,
              };

              for (int partCode = 0; partCode < 6; ++partCode)
              {
                if (strncmp(defaultPartCodes[partCode], "-1", 2u) != 0)
                {
                  _base_fld *partRecord =
                    g_Main.m_tblUnitPart[partCode].GetRecord(const_cast<char *>(defaultPartCodes[partCode]));
                  if (!partRecord)
                  {
                    resultCode = 100;
                    break;
                  }
                  defaultParts[partCode] = static_cast<unsigned __int8>(partRecord->m_dwIndex);
                }
              }
            }
          }
        }
      }
    }
  }

  unsigned __int16 keySerial = static_cast<unsigned __int16>(-1);
  unsigned __int16 keyIndex = static_cast<unsigned __int16>(-1);

  if (!resultCode)
  {
    _UNIT_DB_BASE::_LIST *slotData = &m_Param.m_UnitDB.m_List[emptyUnitSlot];
    slotData->DelUnit();
    slotData->byFrame = byFrameCode;
    std::memcpy(slotData->byPart, defaultParts, sizeof(slotData->byPart));
    slotData->dwGauge = static_cast<unsigned int>(frameRecord->m_nUnit_HP);
    m_pUserDB->Update_UnitInsert(emptyUnitSlot, slotData);

    keySerial = m_Param.GetNewItemSerial();
    keyIndex = static_cast<unsigned __int16>(unitKeyRecord->m_dwIndex);

    _STORAGE_LIST::_storage_con keyItem;
    keyItem.m_byTableCode = 19;
    keyItem.m_wItemIndex = keyIndex;
    keyItem.m_dwDur = 0;
    keyItem.m_dwLv = emptyUnitSlot;
    keyItem.m_wSerial = keySerial;

    if (!Emb_AddStorage(0, &keyItem, false, true))
    {
      this->SendMsg_UnitFrameBuyResult(static_cast<char>(255), static_cast<char>(byFrameCode), static_cast<char>(emptyUnitSlot), keyIndex, keySerial, consumeMoney);
      return;
    }

    SubDalant(consumeMoney[0]);
    SubGold(consumeMoney[1]);

    if (!m_byUserDgr)
    {
      if (consumeMoney[1])
      {
        eAddGold(0, consumeMoney[1]);
      }
      if (consumeMoney[0])
      {
        eAddDalant(0, consumeMoney[0]);
      }
    }

    const int level = m_Param.GetLevel();
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      if (consumeMoney[1])
      {
        const unsigned int amount = 2000u * consumeMoney[1];
        CMoneySupplyMgr::Instance()->UpdateBuyUnitData(level, amount);
      }
      if (consumeMoney[0])
      {
        CMoneySupplyMgr::Instance()->UpdateBuyUnitData(level, consumeMoney[0]);
      }
    }
  }

  this->SendMsg_UnitFrameBuyResult(static_cast<char>(resultCode), static_cast<char>(byFrameCode), static_cast<char>(emptyUnitSlot), keyIndex, keySerial, consumeMoney);
}

void CPlayer::pc_UnitSellRequest(unsigned __int8 bySlotIndex, int bUseNPCLinkIntem)
{
  if (!m_pUserDB)
  {
    return;
  }

  unsigned __int8 resultCode = 0;
  _UNIT_DB_BASE::_LIST *unitData = &m_Param.m_UnitDB.m_List[bySlotIndex];
  const unsigned __int8 frameCode = unitData->byFrame;
  _UnitFrame_fld *frameRecord = reinterpret_cast<_UnitFrame_fld *>(g_Main.m_tblUnitFrame.GetRecord(frameCode));

  unsigned int totalDebt = 0;
  unsigned int sellMoney = 0;
  int dalantDelta = 0;

  const unsigned int sellRate = 10000u - eGetTexRate(0);
  const unsigned int debtRate = eGetTexRate(0) + 10000u;

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    resultCode = 34;
  }
  else if (!bUseNPCLinkIntem && !IsBeNearStore(this, 4))
  {
    resultCode = 21;
  }
  else if (!frameRecord)
  {
    resultCode = 9;
  }
  else if (m_Param.GetRaceCode() != 0)
  {
    resultCode = 1;
  }
  else if (m_pUsingUnit)
  {
    resultCode = 2;
  }
  else if (frameCode == 255)
  {
    resultCode = 5;
  }
  else
  {
    const unsigned long long rawDebt =
      static_cast<unsigned long long>(unitData->nPullingFee) * static_cast<unsigned long long>(debtRate);
    totalDebt = static_cast<unsigned int>(rawDebt / 10000u);

    if (totalDebt > m_Param.GetDalant())
    {
      resultCode = 10;
    }
    else
    {
      const float gaugeRate =
        static_cast<float>(unitData->dwGauge) / static_cast<float>(frameRecord->m_nUnit_HP);
      const double baseSell =
        static_cast<double>(frameRecord->m_nStdPrice) * static_cast<double>(gaugeRate) / 2.0;
      const unsigned long long roundedSell =
        static_cast<unsigned long long>(sellRate * static_cast<unsigned long long>(static_cast<unsigned int>(baseSell)) + 5000u);
      sellMoney = static_cast<unsigned int>(roundedSell / 10000u);

      dalantDelta = static_cast<int>(sellMoney) - static_cast<int>(totalDebt);
      if (dalantDelta > 0)
      {
        if (!CanAddMoneyForMaxLimMoney(static_cast<unsigned int>(dalantDelta), m_Param.GetDalant()))
        {
          resultCode = 30;
        }
      }
      else if (dalantDelta < 0)
      {
        if (static_cast<unsigned int>(-dalantDelta) > m_Param.GetDalant())
        {
          resultCode = 10;
        }
      }
    }
  }

  unsigned __int16 removedKeySerial = static_cast<unsigned __int16>(-1);
  if (!resultCode)
  {
    unitData->byFrame = static_cast<unsigned __int8>(-1);
    removedKeySerial = _DeleteUnitKey(bySlotIndex);

    const double playerPenalty = g_Main.m_pTimeLimitMgr->GetPlayerPenalty(m_id.wIndex);
    dalantDelta = static_cast<int>(static_cast<double>(dalantDelta) * playerPenalty);

    AlterDalant(static_cast<double>(dalantDelta));
    m_pUserDB->Update_UnitDelete(bySlotIndex);

    if (!m_byUserDgr)
    {
      eAddDalant(0, dalantDelta);
    }
  }

  this->SendMsg_UnitSellResult(static_cast<char>(resultCode), static_cast<char>(bySlotIndex), removedKeySerial, static_cast<int>(sellMoney), totalDebt, m_Param.GetDalant(), m_Param.GetGold());
}

void CPlayer::pc_UnitPartTuningRequest(
  unsigned __int8 bySlotIndex,
  unsigned __int8 byTuningNum,
  _tuning_data *pTuningData,
  int bUseNPCLinkIntem)
{
  if (!m_pUserDB)
  {
    return;
  }

  static constexpr int gnBulletPerPart[6] = {-1, -1, -1, 0, 1, -1};

  unsigned __int8 resultCode = 0;
  _UNIT_DB_BASE::_LIST *unitData = &m_Param.m_UnitDB.m_List[bySlotIndex];
  const unsigned __int8 frameCode = unitData->byFrame;
  unsigned int consumeMoney[7] = {};

  const unsigned int buyTaxRate = eGetTexRate(0) + 10000u;
  const unsigned int sellTaxRate = 10000u - eGetTexRate(0);
  (void)(eGetTex(0) + 1.0f);
  (void)(1.0f - eGetTex(0));

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    resultCode = 34;
  }
  else if (!bUseNPCLinkIntem && !IsBeNearStore(this, 4))
  {
    resultCode = 21;
  }
  else if (m_Param.GetRaceCode() != 0)
  {
    resultCode = 1;
  }
  else if (m_pUsingUnit)
  {
    resultCode = 2;
  }
  else if (frameCode == 255)
  {
    resultCode = 5;
  }
  else
  {
    _UnitFrame_fld *frameRecord = reinterpret_cast<_UnitFrame_fld *>(g_Main.m_tblUnitFrame.GetRecord(frameCode));
    if (!frameRecord)
    {
      resultCode = 5;
    }
    else if (!frameRecord->m_bRepair)
    {
      resultCode = 38;
    }
    else if (!unitData->dwGauge)
    {
      resultCode = 36;
    }
    else
    {
      for (int tuningIndex = 0; tuningIndex < byTuningNum; ++tuningIndex)
      {
        const unsigned __int8 partCode = pTuningData[tuningIndex].byPartCode;
        const unsigned __int8 partIndex = pTuningData[tuningIndex].byPartIndex;

        _UnitPart_fld *newPartRecord =
          reinterpret_cast<_UnitPart_fld *>(g_Main.m_tblUnitPart[partCode].GetRecord(partIndex));
        if (!newPartRecord)
        {
          resultCode = 8;
          break;
        }

        if (newPartRecord->m_strDefFrame[frameCode] == '0')
        {
          resultCode = 6;
          break;
        }

        if (unitData->byPart[partCode] == partIndex)
        {
          resultCode = 29;
          break;
        }

        const int level = m_Param.GetLevel();
        if (newPartRecord->m_nLevelLim > level)
        {
          resultCode = 12;
          break;
        }
        if (newPartRecord->m_nUpLevelLim != -1 && newPartRecord->m_nUpLevelLim < level)
        {
          resultCode = 12;
          break;
        }

        const int masteryCodeList[2] = {newPartRecord->m_nExpertID1, newPartRecord->m_nExpertID2};
        const int masteryLimitList[2] = {newPartRecord->m_nExpertLim1, newPartRecord->m_nExpertLim2};
        for (int masterySlot = 0; masterySlot < 2; ++masterySlot)
        {
          const int masteryCode = masteryCodeList[masterySlot];
          if (masteryCode != -1)
          {
            if (masteryCode >= 6)
            {
              resultCode = 13;
              break;
            }
            if (m_pmMst.GetEquipMastery(static_cast<unsigned __int8>(masteryCode))
                < masteryLimitList[masterySlot])
            {
              resultCode = 13;
              break;
            }
          }
        }
        if (resultCode)
        {
          break;
        }

        const unsigned long long partCostRaw =
          static_cast<unsigned long long>(newPartRecord->m_nStdPrice)
          * static_cast<unsigned long long>(buyTaxRate);
        const int currentCost = static_cast<int>(consumeMoney[newPartRecord->m_nMoney]);
        consumeMoney[newPartRecord->m_nMoney] =
          static_cast<unsigned int>(currentCost + static_cast<int>(partCostRaw / 10000u));

        if (partCode == 5)
        {
          for (int spareSlot = 0; spareSlot < 8; ++spareSlot)
          {
            if (unitData->dwSpare[spareSlot] != -1)
            {
              _UnitBullet_fld *spareBullet =
                reinterpret_cast<_UnitBullet_fld *>(
                  g_Main.m_tblUnitBullet.GetRecord(static_cast<unsigned __int16>(unitData->dwSpare[spareSlot])));
              if (spareBullet)
              {
                const unsigned long long spareRefundRaw =
                  static_cast<unsigned long long>(sellTaxRate)
                  * static_cast<unsigned long long>(spareBullet->m_nStdPrice / 2)
                  + 5000u;
                const int currentCost = static_cast<int>(consumeMoney[spareBullet->m_nMoney]);
                consumeMoney[spareBullet->m_nMoney] =
                  static_cast<unsigned int>(currentCost - static_cast<int>(spareRefundRaw / 10000u));
              }
            }
          }
        }
      }

      if (!resultCode)
      {
        for (int moneyCode = 0; moneyCode < 7; ++moneyCode)
        {
          if (consumeMoney[moneyCode] > GetMoney(static_cast<unsigned __int8>(moneyCode)))
          {
            resultCode = 7;
            break;
          }
        }
      }
    }
  }

  if (!resultCode)
  {
    for (int tuningIndex = 0; tuningIndex < byTuningNum; ++tuningIndex)
    {
      const unsigned __int8 partCode = pTuningData[tuningIndex].byPartCode;
      const unsigned __int8 partIndex = pTuningData[tuningIndex].byPartIndex;

      unitData->byPart[partCode] = partIndex;
      if (partCode == 5)
      {
        std::memset(unitData->dwSpare, 255, sizeof(unitData->dwSpare));
      }

      if (partCode == 3 || partCode == 4)
      {
        const int bulletSlot = gnBulletPerPart[partCode];
        if (bulletSlot != -1)
        {
          unsigned int &bulletData = unitData->dwBullet[bulletSlot];
          const unsigned __int16 bulletItemIndex = static_cast<unsigned __int16>((bulletData >> 16) & 0xFFFF);
          if (bulletItemIndex && bulletItemIndex != 65535)
          {
            _UnitBullet_fld *bulletRecord =
              reinterpret_cast<_UnitBullet_fld *>(g_Main.m_tblUnitBullet.GetRecord(bulletItemIndex));
            _UnitPart_fld *partRecord =
              reinterpret_cast<_UnitPart_fld *>(g_Main.m_tblUnitPart[partCode].GetRecord(partIndex));
            if (bulletRecord
                && partRecord
                && bulletRecord->m_nWPType != partRecord->m_nWPType)
            {
              bulletData = -1;
            }
          }
        }
      }
    }

    SubDalant(consumeMoney[0]);
    SubGold(consumeMoney[1]);
    m_pUserDB->Update_UnitData(bySlotIndex, unitData);
    CPlayer::s_MgrItemHistory.tuning_unit(
      m_ObjID.m_wIndex,
      bySlotIndex,
      unitData,
      reinterpret_cast<int *>(consumeMoney),
      m_Param.GetDalant(),
      m_Param.GetGold(),
      m_szItemHistoryFileName);

    if (!m_byUserDgr)
    {
      eAddGold(0, consumeMoney[1]);
      eAddDalant(0, consumeMoney[0]);
    }

    const int level = m_Param.GetLevel();
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      if (static_cast<int>(consumeMoney[1]) > 0)
      {
        const unsigned int amount = 2000u * consumeMoney[1];
        CMoneySupplyMgr::Instance()->UpdateBuyUnitData(level, amount);
      }
      if (static_cast<int>(consumeMoney[0]) > 0)
      {
        CMoneySupplyMgr::Instance()->UpdateBuyUnitData(level, consumeMoney[0]);
      }
    }
  }

  this->SendMsg_UnitPartTuningResult(
    static_cast<char>(resultCode),
    static_cast<char>(bySlotIndex),
    reinterpret_cast<int *>(consumeMoney));
}

void CPlayer::pc_UnitFrameRepairRequest(unsigned __int8 bySlotIndex, int bUseNPCLinkIntem, int bUnitRepairOut)
{
  if (!m_pUserDB)
  {
    return;
  }

  unsigned __int8 resultCode = 0;
  _UNIT_DB_BASE::_LIST *unitData = &m_Param.m_UnitDB.m_List[bySlotIndex];
  const unsigned __int8 frameCode = unitData->byFrame;
  _UnitFrame_fld *frameRecord = reinterpret_cast<_UnitFrame_fld *>(g_Main.m_tblUnitFrame.GetRecord(frameCode));

  unsigned int consumeDalant = 0;
  unsigned int newGauge = 0;
  const unsigned int taxRate = eGetTexRate(0) + 10000u;

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    resultCode = 34;
  }
  else if (!bUseNPCLinkIntem && !IsBeNearStore(this, 4))
  {
    resultCode = 21;
  }
  else
  {
    _UnitFrame_fld *repairableFrame = reinterpret_cast<_UnitFrame_fld *>(g_Main.m_tblUnitFrame.GetRecord(frameCode));
    if (!repairableFrame)
    {
      resultCode = 5;
    }
    else if (!repairableFrame->m_bRepair)
    {
      resultCode = 37;
    }
    else if (m_Param.GetRaceCode() != 0)
    {
      resultCode = 1;
    }
    else if (m_pUsingUnit)
    {
      resultCode = 2;
    }
    else if (!frameRecord)
    {
      resultCode = 9;
    }
    else if (frameCode == 255)
    {
      resultCode = 5;
    }
    else
    {
      newGauge = static_cast<unsigned int>(frameRecord->m_nUnit_HP);
      const int gaugeDiff = static_cast<int>(newGauge) - static_cast<int>(unitData->dwGauge);
      if (gaugeDiff <= 0)
      {
        resultCode = 14;
      }
      else
      {
        if (frameRecord->m_nRepPrice > 0)
        {
          consumeDalant = static_cast<unsigned int>(frameRecord->m_nRepPrice);
        }

        int brokenBonus = 0;
        for (int partCode = 0; partCode < 6; ++partCode)
        {
          _UnitPart_fld *partRecord =
            reinterpret_cast<_UnitPart_fld *>(g_Main.m_tblUnitPart[partCode].GetRecord(unitData->byPart[partCode]));
          if (!partRecord)
          {
            continue;
          }

          if (partRecord->m_nRepPrice > 0)
          {
            consumeDalant += static_cast<unsigned int>(partRecord->m_nRepPrice);
          }
          if (!unitData->dwGauge)
          {
            brokenBonus += partRecord->m_nDesrepPrice;
          }
        }

        const float scaledCost =
          static_cast<float>(consumeDalant)
          * static_cast<float>(gaugeDiff)
          * m_fUnitPv_RepPr;
        const unsigned long long taxedCost =
          static_cast<unsigned long long>(taxRate) * static_cast<unsigned long long>(static_cast<unsigned int>(scaledCost));
        consumeDalant = static_cast<unsigned int>(brokenBonus) + static_cast<unsigned int>(taxedCost / 10000u);

        if (bUnitRepairOut == 1)
        {
          resultCode = 43;
          this->SendMsg_UnitFrameRepairResult(
            static_cast<char>(resultCode),
            static_cast<char>(bySlotIndex),
            newGauge,
            consumeDalant);
          return;
        }

        if (consumeDalant > m_Param.GetDalant())
        {
          resultCode = 7;
        }
      }
    }
  }

  if (!resultCode)
  {
    unitData->dwGauge = newGauge;
    SubDalant(consumeDalant);
    m_pUserDB->Update_UnitData(bySlotIndex, unitData);

    if (!m_byUserDgr)
    {
      eAddDalant(0, consumeDalant);
    }

    const int level = m_Param.GetLevel();
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      CMoneySupplyMgr::Instance()->UpdateUnitRepairingChargesData(level, consumeDalant);
    }
  }

  this->SendMsg_UnitFrameRepairResult(static_cast<char>(resultCode), static_cast<char>(bySlotIndex), newGauge, consumeDalant);
}

void CPlayer::pc_UnitBulletFillRequest(
  unsigned __int8 bySlotIndex,
  unsigned __int16 *pwBulletIndex,
  int bUseNPCLinkIntem)
{
  if (!m_pUserDB)
  {
    return;
  }

  static constexpr unsigned __int8 kWeaponPartPerBullet[2] = {3, 4};

  unsigned __int8 resultCode = 0;
  _UNIT_DB_BASE::_LIST *unitData = &m_Param.m_UnitDB.m_List[bySlotIndex];
  const unsigned __int8 frameCode = unitData->byFrame;

  _UnitBullet_fld *bulletRecords[2] = {
    reinterpret_cast<_UnitBullet_fld *>(g_Main.m_tblUnitBullet.GetRecord(pwBulletIndex[0])),
    reinterpret_cast<_UnitBullet_fld *>(g_Main.m_tblUnitBullet.GetRecord(pwBulletIndex[1])),
  };

  unsigned int consumeMoney[7] = {};
  const unsigned int taxRate = eGetTexRate(0) + 10000u;
  (void)(eGetTex(0) + 1.0f);

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    resultCode = 34;
  }
  else if (!bUseNPCLinkIntem && !IsBeNearStore(this, 4))
  {
    resultCode = 21;
  }
  else if (m_Param.GetRaceCode() != 0)
  {
    resultCode = 1;
  }
  else if (m_pUsingUnit)
  {
    resultCode = 2;
  }
  else if (frameCode == 255)
  {
    resultCode = 5;
  }
  else
  {
    for (int bulletSlot = 0; bulletSlot < 2; ++bulletSlot)
    {
      _UnitBullet_fld *bulletRecord = bulletRecords[bulletSlot];
      if (!bulletRecord)
      {
        continue;
      }

      const unsigned __int8 partCode = kWeaponPartPerBullet[bulletSlot];
      _UnitPart_fld *partRecord =
        reinterpret_cast<_UnitPart_fld *>(g_Main.m_tblUnitPart[partCode].GetRecord(unitData->byPart[partCode]));
      if (!partRecord)
      {
        resultCode = 26;
        break;
      }

      if (partRecord->m_nWPType != bulletRecord->m_nWPType)
      {
        resultCode = 26;
        break;
      }
    }

    if (!resultCode)
    {
      for (int bulletSlot = 0; bulletSlot < 2; ++bulletSlot)
      {
        _UnitBullet_fld *bulletRecord = bulletRecords[bulletSlot];
        if (bulletRecord)
        {
          const int moneyCode = bulletRecord->m_nMoney;
          if (moneyCode >= 0 && moneyCode < 7)
          {
            consumeMoney[moneyCode] += static_cast<unsigned int>(bulletRecord->m_nStdPrice);
          }
        }
      }

      for (int moneyCode = 0; moneyCode < 7; ++moneyCode)
      {
        const unsigned long long taxed =
          static_cast<unsigned long long>(consumeMoney[moneyCode]) * static_cast<unsigned long long>(taxRate);
        consumeMoney[moneyCode] = static_cast<unsigned int>(taxed / 10000u);
        if (consumeMoney[moneyCode] > GetMoney(static_cast<unsigned __int8>(moneyCode)))
        {
          resultCode = 7;
          break;
        }
      }
    }
  }

  if (!resultCode)
  {
    for (int bulletSlot = 0; bulletSlot < 2; ++bulletSlot)
    {
      _UnitBullet_fld *bulletRecord = bulletRecords[bulletSlot];
      if (bulletRecord)
      {
        unsigned int &slotData = unitData->dwBullet[bulletSlot];
        reinterpret_cast<unsigned __int16 &>(slotData) = pwBulletIndex[bulletSlot];
        reinterpret_cast<unsigned __int16 *>(&slotData)[1] = static_cast<unsigned __int16>(bulletRecord->m_nDurUnit);
      }
    }

    SubDalant(consumeMoney[0]);
    SubGold(consumeMoney[1]);
    m_pUserDB->Update_UnitData(bySlotIndex, unitData);
    if (consumeMoney[0] || consumeMoney[1])
    {
      const unsigned int newDalant = m_Param.GetDalant();
      const unsigned int newGold = m_Param.GetGold();
      CPlayer::s_MgrItemHistory.pay_money(
        m_ObjID.m_wIndex,
        "Unit_Bullet_Charge",
        consumeMoney[0],
        consumeMoney[1],
        newDalant,
        newGold,
        m_szItemHistoryFileName);
    }

    if (!m_byUserDgr)
    {
      eAddGold(0, consumeMoney[1]);
      eAddDalant(0, consumeMoney[0]);
    }

    const int level = m_Param.GetLevel();
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      if (consumeMoney[1])
      {
        const unsigned int amount = 2000u * consumeMoney[1];
        CMoneySupplyMgr::Instance()->UpdateBuyUnitData(level, amount);
      }
      if (consumeMoney[0])
      {
        CMoneySupplyMgr::Instance()->UpdateBuyUnitData(level, consumeMoney[0]);
      }
    }
  }

  this->SendMsg_UnitBulletFillResult(static_cast<char>(resultCode), static_cast<char>(bySlotIndex), pwBulletIndex, consumeMoney);
}

void CPlayer::pc_UnitPackFillRequest(
  unsigned __int8 bySlotIndex,
  unsigned __int8 byFillNum,
  _unit_pack_fill_request_clzo::__list *pList,
  int bUseNPCLinkIntem)
{
  if (!m_pUserDB)
  {
    return;
  }

  unsigned __int8 resultCode = 0;
  _UNIT_DB_BASE::_LIST *unitData = &m_Param.m_UnitDB.m_List[bySlotIndex];
  const unsigned __int8 frameCode = unitData->byFrame;

  _UnitPart_fld *backPartRecord =
    reinterpret_cast<_UnitPart_fld *>(g_Main.m_tblUnitPart[5].GetRecord(unitData->byPart[5]));

  _UnitBullet_fld *bulletPerSpare[8] = {};
  unsigned int consumeMoney[7] = {};
  const unsigned int taxRate = eGetTexRate(0) + 10000u;

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    resultCode = 34;
  }
  else if (!bUseNPCLinkIntem && !IsBeNearStore(this, 4))
  {
    resultCode = 21;
  }
  else if (m_Param.GetRaceCode() != 0)
  {
    resultCode = 1;
  }
  else if (frameCode == 255)
  {
    resultCode = 5;
  }
  else if (!backPartRecord)
  {
    resultCode = 17;
  }
  else
  {
    for (int fillIndex = 0; fillIndex < byFillNum; ++fillIndex)
    {
      if (pList[fillIndex].bySpareIndex >= backPartRecord->m_nBackSlt)
      {
        resultCode = 18;
        break;
      }

      _UnitBullet_fld *bulletRecord =
        reinterpret_cast<_UnitBullet_fld *>(g_Main.m_tblUnitBullet.GetRecord(pList[fillIndex].wBulletIndex));
      bulletPerSpare[pList[fillIndex].bySpareIndex] = bulletRecord;
      if (!bulletRecord)
      {
        resultCode = 18;
        break;
      }
    }

    if (!resultCode)
    {
      if (m_pUsingUnit)
      {
        resultCode = 2;
      }
      else
      {
        for (int spareSlot = 0; spareSlot < 8; ++spareSlot)
        {
          _UnitBullet_fld *bulletRecord = bulletPerSpare[spareSlot];
          if (bulletRecord)
          {
            const int moneyCode = bulletRecord->m_nMoney;
            if (moneyCode >= 0 && moneyCode < 7)
            {
              consumeMoney[moneyCode] += static_cast<unsigned int>(bulletRecord->m_nStdPrice);
            }
          }
        }

        for (int moneyCode = 0; moneyCode < 7; ++moneyCode)
        {
          const unsigned long long taxed =
            static_cast<unsigned long long>(consumeMoney[moneyCode]) * static_cast<unsigned long long>(taxRate);
          consumeMoney[moneyCode] = static_cast<unsigned int>(taxed / 10000u);
          if (consumeMoney[moneyCode] > GetMoney(static_cast<unsigned __int8>(moneyCode)))
          {
            resultCode = 7;
            break;
          }
        }
      }
    }
  }

  if (!resultCode)
  {
    for (int spareSlot = 0; spareSlot < 8; ++spareSlot)
    {
      _UnitBullet_fld *bulletRecord = bulletPerSpare[spareSlot];
      if (bulletRecord)
      {
        unsigned int &spareData = unitData->dwSpare[spareSlot];
        reinterpret_cast<unsigned __int16 &>(spareData) = static_cast<unsigned __int16>(bulletRecord->m_dwIndex);
        reinterpret_cast<unsigned __int16 *>(&spareData)[1] = static_cast<unsigned __int16>(bulletRecord->m_nDurUnit);
      }
    }

    SubDalant(consumeMoney[0]);
    SubGold(consumeMoney[1]);
    m_pUserDB->Update_UnitData(bySlotIndex, unitData);

    if (!m_byUserDgr)
    {
      eAddGold(0, consumeMoney[1]);
      eAddDalant(0, consumeMoney[0]);
    }

    const int level = m_Param.GetLevel();
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      if (consumeMoney[1])
      {
        const unsigned int amount = 2000u * consumeMoney[1];
        CMoneySupplyMgr::Instance()->UpdateBuyUnitData(level, amount);
      }
      if (consumeMoney[0])
      {
        CMoneySupplyMgr::Instance()->UpdateBuyUnitData(level, consumeMoney[0]);
      }
    }
  }

  this->SendMsg_UnitPackFillResult(resultCode, bySlotIndex, byFillNum, pList, consumeMoney);
}

void CPlayer::pc_UnitDeliveryRequest(
  unsigned __int8 bySlotIndex,
  CItemStore *pStore,
  bool bPayFee,
  float *pfNewPos,
  int bUseNPCLinkIntem)
{
  unsigned __int8 resultCode = 0;
  _UNIT_DB_BASE::_LIST *unitData = &m_Param.m_UnitDB.m_List[bySlotIndex];
  const int raceCode = static_cast<int>(m_Param.GetRaceCode());
  unsigned int consumeDalant = 0;
  CParkingUnit *parkingUnit = nullptr;
  unsigned __int8 transDistCode = 0;
  // Yorozuya fix (non-IDA parity): ignore packet position and use current player position.
  float deliveryPos[3]{};
  std::memcpy(deliveryPos, m_fCurPos, sizeof(deliveryPos));

  if (!unitData->dwGauge)
  {
    resultCode = 35;
  }
  else
  {
    const unsigned int taxRate = eGetTexRate(raceCode) + 10000u;

    if (m_bObserver)
    {
      resultCode = 27;
    }
    else if (raceCode != 0)
    {
      resultCode = 1;
    }
    else if (unitData->byFrame == 255)
    {
      resultCode = 5;
    }
    else if (m_pCurMap->m_pMapSet->m_nMapType >= 2u)
    {
      resultCode = 27;
    }
    else if (m_pUsingUnit)
    {
      resultCode = 2;
    }
    else
    {
      if (pStore)
      {
        if (pStore->m_pRec->m_nStore_trade != 4)
        {
          resultCode = 23;
        }
        else if (!bUseNPCLinkIntem && GetSqrt(m_fCurPos, pStore->GetStorePos()) > 540.0f)
        {
          resultCode = 21;
        }
      }
      else
      {
        transDistCode = 1;
      }

      if (!resultCode && bPayFee)
      {
        consumeDalant = static_cast<unsigned int>(unitData->nPullingFee) + 10000u;
        if (transDistCode == 1)
        {
          consumeDalant += 1000u;
        }

        const unsigned long long taxed =
          static_cast<unsigned long long>(taxRate) * static_cast<unsigned long long>(consumeDalant);
        consumeDalant = static_cast<unsigned int>(taxed / 10000u);
        if (consumeDalant > m_Param.GetDalant())
        {
          resultCode = 11;
        }
      }

      if (!resultCode)
      {
        if (GetSqrt(m_fCurPos, deliveryPos) > 40.0f)
        {
          resultCode = 24;
        }
        else
        {
          parkingUnit = FindEmptyParkingUnit(g_ParkingUnit, MAX_PARKING_UNIT);
          if (!parkingUnit)
          {
            resultCode = 19;
          }
        }
      }
    }
  }

  unsigned int parkingSerial = static_cast<unsigned int>(-1);
  if (!resultCode)
  {
    _parkingunit_create_setdata createData{};
    createData.byFrame = unitData->byFrame;
    std::memcpy(createData.byPartCode, unitData->byPart, sizeof(createData.byPartCode));
    createData.m_pRecordSet = g_Main.m_tblUnitFrame.GetRecord(unitData->byFrame);
    createData.pOwner = this;
    createData.byCreateType = 0;
    createData.m_pMap = m_pCurMap;
    createData.m_nLayerIndex = m_wMapLayerIndex;
    std::memcpy(createData.m_fStartPos, deliveryPos, sizeof(createData.m_fStartPos));
    createData.byTransDistCode = transDistCode;

    unsigned int maxGauge = 10000u;
    _UnitFrame_fld *frameRecord = static_cast<_UnitFrame_fld *>(g_Main.m_tblUnitFrame.GetRecord(unitData->byFrame));
    if (frameRecord && frameRecord->m_nUnit_HP > 0)
    {
      maxGauge = static_cast<unsigned int>(frameRecord->m_nUnit_HP);
    }
    createData.wHPRate = static_cast<unsigned __int16>(10000u * unitData->dwGauge / maxGauge);

    if (!createData.m_pRecordSet || !parkingUnit->Create(&createData))
    {
      resultCode = 19;
    }
    else
    {
      parkingSerial = parkingUnit->m_dwObjSerial;
      _LockUnitKey(bySlotIndex, true);

      _UnitPart_fld *backPartRecord =
        reinterpret_cast<_UnitPart_fld *>(g_Main.m_tblUnitPart[5].GetRecord(unitData->byPart[5]));
      if (backPartRecord)
      {
        unitData->wBooster = static_cast<unsigned __int16>(backPartRecord->m_nBstCha);
      }

      m_pUsingUnit = unitData;
      m_pParkingUnit = parkingUnit;
      m_dwUnitViewOverTime = static_cast<unsigned int>(-1);

      SubDalant(consumeDalant);
      if (!m_byUserDgr)
      {
        eAddDalant(raceCode, consumeDalant);
      }

      const int level = m_Param.GetLevel();
      if (level == 30 || level == 40 || level == 50 || level == 60)
      {
        CMoneySupplyMgr::Instance()->UpdateBuyUnitData(level, consumeDalant);
      }

    }
  }

  this->SendMsg_UnitDeliveryResult(static_cast<char>(resultCode), static_cast<char>(bySlotIndex), parkingSerial, consumeDalant);
}

void CPlayer::pc_UnitTakeRequest()
{
  unsigned __int8 byRetCode = 0;
  BreakCloakBooster();

  if (m_bInGuildBattle && m_bTakeGravityStone)
  {
    byRetCode = 31;
  }
  else
  {
    bool canUseUnit = m_bFreeSFByClass;
    if (!canUseUnit)
    {
      for (int index = 0; index < 4; ++index)
      {
        _class_fld *historyEffect = *m_Param.m_ppHistoryEffect[index];
        if (!historyEffect)
        {
          break;
        }
        if (historyEffect->m_bUnitUsable)
        {
          canUseUnit = true;
          break;
        }
      }
    }

    if (!canUseUnit)
    {
      byRetCode = 28;
    }
    else if (m_pSiegeItem)
    {
      byRetCode = 28;
    }
    else if (!m_pParkingUnit || !m_pUsingUnit)
    {
      byRetCode = 2;
    }
    else if (m_pParkingUnit->m_dwOwnerSerial != m_dwObjSerial)
    {
      byRetCode = 2;
    }
    else if (GetSqrt(m_fCurPos, m_pParkingUnit->m_fCurPos) > 50.0)
    {
      byRetCode = 20;
    }
    else if (m_bAfterEffect)
    {
      byRetCode = 32;
    }
    else if (m_pParkingUnit->GetHP() <= 0)
    {
      byRetCode = 35;
    }
  }

  if (!byRetCode)
  {
    float newPos[3] = {
      m_pParkingUnit->m_fCurPos[0],
      m_pParkingUnit->m_fCurPos[1],
      m_pParkingUnit->m_fCurPos[2],
    };
    m_pCurMap->GetRandPosInRange(m_pParkingUnit->m_fCurPos, 10, newPos);
    std::memcpy(m_fOldPos, m_fCurPos, sizeof(m_fOldPos));
    std::memcpy(m_fCurPos, newPos, sizeof(m_fCurPos));
    Emb_RidindUnit(true, nullptr);
    SendMsg_AlterUnitHPInform(static_cast<char>(m_pUsingUnit->bySlotIndex), m_pUsingUnit->dwGauge);
  }

  this->SendMsg_UnitTakeResult(static_cast<char>(byRetCode));
}

void CPlayer::pc_UnitLeaveRequest(float *pfNewPos)
{
  unsigned __int8 byRetCode = 0;
  CParkingUnit *parkingUnit = nullptr;

  if (m_Param.GetRaceCode() != 0)
  {
    byRetCode = 1;
  }
  else if (!m_pUsingUnit || m_pParkingUnit)
  {
    byRetCode = 22;
  }
  else if (GetSqrt(m_fCurPos, pfNewPos) > 40.0)
  {
    byRetCode = 24;
  }
  else
  {
    parkingUnit = FindEmptyParkingUnit(g_ParkingUnit, MAX_PARKING_UNIT);
    if (!parkingUnit)
    {
      byRetCode = 19;
    }
  }

  if (!byRetCode && parkingUnit)
  {
    _parkingunit_create_setdata createData{};
    createData.byFrame = m_pUsingUnit->byFrame;
    std::memcpy(createData.byPartCode, m_pUsingUnit->byPart, sizeof(createData.byPartCode));
    createData.m_pRecordSet = g_Main.m_tblUnitFrame.GetRecord(m_pUsingUnit->byFrame);
    createData.pOwner = this;
    createData.byCreateType = 1;
    createData.m_pMap = m_pCurMap;
    createData.m_nLayerIndex = m_wMapLayerIndex;
    std::memcpy(createData.m_fStartPos, m_fCurPos, sizeof(createData.m_fStartPos));
    createData.byTransDistCode = 0;

    unsigned int maxGauge = 10000u;
    _UnitFrame_fld *frameRecord = static_cast<_UnitFrame_fld *>(g_Main.m_tblUnitFrame.GetRecord(m_pUsingUnit->byFrame));
    if (frameRecord && frameRecord->m_nUnit_HP > 0)
    {
      maxGauge = static_cast<unsigned int>(frameRecord->m_nUnit_HP);
    }
    createData.wHPRate = static_cast<unsigned __int16>(10000u * m_pUsingUnit->dwGauge / maxGauge);

    if (!createData.m_pRecordSet || !parkingUnit->Create(&createData))
    {
      byRetCode = 19;
    }
    else
    {
      Emb_RidindUnit(false, parkingUnit);
    }
  }

  this->SendMsg_UnitLeaveResult(static_cast<char>(byRetCode));
}

