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


namespace
{

bool IsSameTargetAsPerformer(const CPlayer *player, const _CHRID *targetId)
{
  return targetId
    && player->m_ObjID.m_byID == targetId->byID
    && player->m_dwObjSerial == targetId->dwSerial;
}

bool IsUsableTempEffectAtStoneState(int nTempEffectType)
{
  if (nTempEffectType < 0)
  {
    return false;
  }

  if (nTempEffectType == 5)
  {
    return true;
  }

  return (nTempEffectType > 6 && nTempEffectType <= 14) || nTempEffectType == 28;
}

void FillThrowEffectList(_throw_skill_result_one_zocl::__effected_list *outList, char *outCount)
{
  *outCount = g_tmpEffectedNum;
  for (int index = 0; index < g_tmpEffectedNum; ++index)
  {
    outList[index].byObjID = g_tmpEffectedList[index].pMem->m_ObjID.m_byID;
    outList[index].dwObjSerial = g_tmpEffectedList[index].pMem->m_dwObjSerial;
    outList[index].byRetCode = g_tmpEffectedList[index].byRetCode;
    outList[index].wEffectValue = g_tmpEffectedList[index].wEffectValue;
  }
}

void ApplyGaugeConsumeAndSendRecover(CPlayer *player, const unsigned __int16 *pwDelPoint)
{
  for (int paramCode = 0; paramCode < 3; ++paramCode)
  {
    if (!pwDelPoint[paramCode])
    {
      continue;
    }

    const int gauge = player->GetGauge(paramCode);
    const int consumeAmount = static_cast<int>(pwDelPoint[paramCode]);
    const int remain = (gauge > consumeAmount) ? (gauge - consumeAmount) : 0;
    player->SetGauge(paramCode, remain, true);
  }
  player->SendMsg_Recover();
}

CCharacter *GetLiveTargetInSameMap(CPlayer *player, _CHRID *targetId)
{
  if (!player || !targetId)
  {
    return nullptr;
  }

  CCharacter *target = reinterpret_cast<CCharacter *>(g_Main.GetObjectA(0, targetId->byID, targetId->wIndex));
  if (!target || !target->m_bLive || target->m_pCurMap != player->m_pCurMap)
  {
    return nullptr;
  }
  return target;
}

} // namespace

void CPlayer::SendMsg_SkillResult(unsigned __int8 byErrCode, const _CHRID *targetId, unsigned __int8 bySkillIndex, unsigned __int8 nSFLv)
{


  _skill_result_one_zocl toSelf{};
  toSelf.byErrCode = byErrCode;
  unsigned __int8 selfType[2] = {17, 5};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    selfType,
    reinterpret_cast<char *>(&toSelf),
    static_cast<unsigned __int16>(sizeof(toSelf)));

  if (!((byErrCode == 0 || byErrCode == 100)
      && (this->m_nCirclePlayerNum <= 500 || !IsSameTargetAsPerformer(this, targetId))))
  {
    return;
  }

  _skill_result_other_zocl toOther{};
  toOther.byRetCode = byErrCode;
  if (targetId)
  {
    toOther.idDster = *targetId;
  }
  toOther.idPerformer.byID = this->m_ObjID.m_byID;
  toOther.idPerformer.wIndex = this->m_ObjID.m_wIndex;
  toOther.idPerformer.dwSerial = this->m_dwObjSerial;
  toOther.bySkillIndex = bySkillIndex;
  toOther.bySkillLv = nSFLv;

  unsigned __int8 otherType[2] = {17, 6};
  this->CircleReport(otherType, reinterpret_cast<char *>(&toOther), sizeof(toOther), false);
}

void CPlayer::SendMsg_ClassSkillResult(unsigned __int8 byErrCode, const _CHRID *targetId, unsigned __int16 wSkillIndex)
{


  _class_skill_result_one_zocl toSelf{};
  toSelf.byErrCode = byErrCode;
  unsigned __int8 selfType[2] = {17, 8};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    selfType,
    reinterpret_cast<char *>(&toSelf),
    static_cast<unsigned __int16>(sizeof(toSelf)));

  if (!((byErrCode == 0 || byErrCode == 100)
      && (this->m_nCirclePlayerNum <= 500 || !IsSameTargetAsPerformer(this, targetId))))
  {
    return;
  }

  _class_skill_result_other_zocl toOther{};
  toOther.byRetCode = byErrCode;
  if (targetId)
  {
    toOther.idDster = *targetId;
  }
  toOther.idPerformer.byID = this->m_ObjID.m_byID;
  toOther.idPerformer.wIndex = this->m_ObjID.m_wIndex;
  toOther.idPerformer.dwSerial = this->m_dwObjSerial;
  toOther.wSkillIndex = wSkillIndex;

  unsigned __int8 otherType[2] = {17, 9};
  this->CircleReport(otherType, reinterpret_cast<char *>(&toOther), sizeof(toOther), false);
}

void CPlayer::SendMsg_ForceResult(unsigned __int8 byErrCode, const _CHRID *targetId, _STORAGE_LIST::_db_con *pForceItem, unsigned __int8 nSFLv)
{


  _force_result_one_zocl toSelf{};
  toSelf.byErrCode = byErrCode;
  if (pForceItem)
  {
    toSelf.dwItemCum = pForceItem->m_dwDur;
  }

  unsigned __int8 selfType[2] = {17, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    selfType,
    reinterpret_cast<char *>(&toSelf),
    static_cast<unsigned __int16>(sizeof(toSelf)));

  if (!((byErrCode == 0 || byErrCode == 100)
      && (this->m_nCirclePlayerNum <= 500 || !IsSameTargetAsPerformer(this, targetId))))
  {
    return;
  }

  _force_result_other_zocl toOther{};
  toOther.byRetCode = byErrCode;
  if (targetId)
  {
    toOther.idDster = *targetId;
  }
  toOther.idPerformer.byID = this->m_ObjID.m_byID;
  toOther.idPerformer.wIndex = this->m_ObjID.m_wIndex;
  toOther.idPerformer.dwSerial = this->m_dwObjSerial;
  if (pForceItem && CPlayer::s_pnLinkForceItemToEffect)
  {
    toOther.byForceIndex = static_cast<unsigned __int8>(CPlayer::s_pnLinkForceItemToEffect[pForceItem->m_wItemIndex]);
  }
  else
  {
    toOther.byForceIndex = static_cast<unsigned __int8>(-1);
  }
  toOther.byForceLv = nSFLv;

  unsigned __int8 otherType[2] = {17, 3};
  this->CircleReport(otherType, reinterpret_cast<char *>(&toOther), sizeof(toOther), false);
}

void CPlayer::SendMsg_ThrowSkillResult(unsigned __int8 byErrCode, _CHRID *pidDst, unsigned __int8 bySkillIndex)
{
  _throw_skill_result_one_zocl toSelf{};
  toSelf.byErrCode = byErrCode;
  memcpy_0(&toSelf.idDster, pidDst, sizeof(toSelf.idDster));
  FillThrowEffectList(toSelf.list, &toSelf.byEffectedNum);

  unsigned __int8 selfType[2] = {17, 100};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    selfType,
    reinterpret_cast<char *>(&toSelf),
    static_cast<unsigned __int16>(sizeof(toSelf)));

  if (byErrCode != 0)
  {
    return;
  }

  _throw_skill_result_other_zocl toOther{};
  toOther.byRetCode = 0;
  toOther.idPerformer.byID = this->m_ObjID.m_byID;
  toOther.idPerformer.wIndex = this->m_ObjID.m_wIndex;
  toOther.idPerformer.dwSerial = this->m_dwObjSerial;
  toOther.bySkillIndex = bySkillIndex;
  memcpy_0(&toOther.idDster, pidDst, sizeof(toOther.idDster));
  toOther.byEffectedNum = toSelf.byEffectedNum;
  memcpy_0(toOther.list, toSelf.list, 8LL * toSelf.byEffectedNum);

  unsigned __int8 otherType[2] = {17, 101};
  this->CircleReport(otherType, reinterpret_cast<char *>(&toOther), sizeof(toOther), false);
}

void CPlayer::SendMsg_ThrowUnitResult(unsigned __int8 byErrCode, _CHRID *pidDst, unsigned __int16 wBulletIndex)
{
  _throw_unit_result_one_zocl toSelf{};
  toSelf.byErrCode = byErrCode;
  memcpy_0(&toSelf.idDster, pidDst, sizeof(toSelf.idDster));
  FillThrowEffectList(toSelf.list, &toSelf.byEffectedNum);

  unsigned __int8 selfType[2] = {17, 103};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    selfType,
    reinterpret_cast<char *>(&toSelf),
    static_cast<unsigned __int16>(sizeof(toSelf)));

  if (byErrCode != 0)
  {
    return;
  }

  _throw_unit_result_other_zocl toOther{};
  toOther.byRetCode = 0;
  toOther.idPerformer.byID = this->m_ObjID.m_byID;
  toOther.idPerformer.wIndex = this->m_ObjID.m_wIndex;
  toOther.idPerformer.dwSerial = this->m_dwObjSerial;
  toOther.wBulletIndex = wBulletIndex;
  memcpy_0(&toOther.idDster, pidDst, sizeof(toOther.idDster));
  toOther.byEffectedNum = toSelf.byEffectedNum;
  memcpy_0(toOther.list, toSelf.list, 8LL * toSelf.byEffectedNum);

  unsigned __int8 otherType[2] = {17, 104};
  this->CircleReport(otherType, reinterpret_cast<char *>(&toOther), sizeof(toOther), false);
}

unsigned __int8 CPlayer::skill_process_for_item(int nSkillIndex, _CHRID *pidDst, int *pnLv)
{
  (void)nSkillIndex;
  (void)pidDst;
  (void)pnLv;
  // this is not a stub
  return 0;
}

void CPlayer::skill_process_for_aura(int nSkillIndex)
{
  _skill_fld *sourceSkill = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[0].GetRecord(nSkillIndex));
  if (!sourceSkill || sourceSkill->m_nClass != 4)
  {
    return;
  }

  _skill_fld *auraSkill =
    reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[0].GetRecord(sourceSkill->m_strRangeEffCode));
  if (!auraSkill)
  {
    return;
  }

  const bool beneficial = auraSkill->m_nContEffectType == 1;
  CCharacter *targets[31]{};
  const int affectedCount = static_cast<int>(_GetAreaEffectMember(
    this,
    beneficial,
    sourceSkill->m_nBonusDistance,
    m_fCurPos,
    auraSkill->m_strActableDst,
    targets));

  unsigned __int8 errorCode[32]{};
  bool upMastery = false;
  for (int index = 0; index < affectedCount; ++index)
  {
    CCharacter *target = targets[index];
    if (!target || target->m_ObjID.m_byKind || target->m_ObjID.m_byID)
    {
      continue;
    }

    CPlayer *targetPlayer = static_cast<CPlayer *>(target);
    const unsigned int targetSerial = targetPlayer->m_Param.GetCharSerial();
    const int raceCode = targetPlayer->m_Param.GetRaceCode();
    const unsigned __int8 bossType =
      CPvpUserAndGuildRankingSystem::Instance()->GetBossType(static_cast<unsigned __int8>(raceCode), targetSerial);
    if (bossType != 4 && bossType != 8)
    {
      AssistSkill(target, 0, auraSkill, 7, errorCode, &upMastery);
    }
  }
}

unsigned __int8 CPlayer::skill_process(
  int nEffectCode,
  int nSkillIndex,
  _CHRID *pidDst,
  unsigned __int16 *pConsumeSerial,
  int *pnLv)
{
  _skill_fld *skillField = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[nEffectCode].GetRecord(nSkillIndex));
  unsigned __int16 gaugeCost[3]{};
  int skillLevel = 1;
  CCharacter *target = nullptr;
  int classGrade[4]{};
  classGrade[0] = -1;

  if (nEffectCode)
  {
    if (nEffectCode == 2 && !m_Param.IsActableClassSkill(skillField->m_strCode, classGrade))
    {
      return 15;
    }
  }
  else
  {
    if (skillField->m_nMastIndex > 8u)
    {
      return 8;
    }
    if (!_pre_check_skill_enable(skillField))
    {
      return 21;
    }
    if (!_pre_check_skill_gradelimit(skillField))
    {
      return 34;
    }
    if (skillField->m_nClass == 4 && (IsChaosMode() || IsPunished(1u, false)))
    {
      return 35;
    }
  }

  if ((nEffectCode == 0 || nEffectCode == 2) && m_bInGuildBattle && m_bTakeGravityStone)
  {
    return 27;
  }

  unsigned __int8 masteryIndex = static_cast<unsigned __int8>(-1);
  if (nEffectCode)
  {
    if (nEffectCode == 2)
    {
      masteryIndex = static_cast<unsigned __int8>(classGrade[0]);
    }
  }
  else
  {
    if (skillField->m_nMastIndex < 8u)
    {
      masteryIndex = static_cast<unsigned __int8>(skillField->m_nMastIndex);
    }
    if (skillField->m_nClass == 2)
    {
      masteryIndex = static_cast<unsigned __int8>(-1);
    }
  }

  if (!m_bSFDelayNotCheck
      && !_ATTACK_DELAY_CHECKER::IsDelay(
        &m_AttDelayChker,
        static_cast<unsigned __int8>(nEffectCode),
        static_cast<unsigned __int16>(skillField->m_dwIndex),
        masteryIndex))
  {
    return 9;
  }

  if (skillField->m_nTempEffectType >= 150)
  {
    return 21;
  }
  if (skillField->m_nContEffectType == 1 && m_EP.GetEff_State(2))
  {
    return 7;
  }
  if (IsRidingUnit())
  {
    return 14;
  }
  if (static_cast<unsigned int>(m_Param.GetRaceCode()) == 2 && IsActingSiegeMode())
  {
    return 21;
  }
  if (m_dwSelfDestructionTime)
  {
    return 26;
  }

  target = reinterpret_cast<CCharacter *>(g_Main.GetObjectA(0, pidDst->byID, pidDst->wIndex));
  if (!target)
  {
    return 2;
  }
  if (!IsEffectableDst(skillField->m_strActableDst, target))
  {
    return 5;
  }
  if (!target->m_bLive)
  {
    return 2;
  }
  if (skillField->m_nTempEffectType == -1 && skillField->m_nContEffectType == -1)
  {
    return 8;
  }
  if (skillField->m_nContEffectType != -1 && !target->IsRecvableContEffect())
  {
    return 13;
  }
  if (target->m_EP.GetEff_State(20))
  {
    if (skillField->m_nTempEffectType == -1 || !IsUsableTempEffectAtStoneState(skillField->m_nTempEffectType))
    {
      return 24;
    }
  }
  if (target->m_EP.GetEff_State(28))
  {
    return 24;
  }
  if (m_EP.GetEff_State(21))
  {
    return 25;
  }
  if (!skillField->m_nContEffectType && !IsAttackableInTown() && !target->IsAttackableInTown())
  {
    if (IsInTown() || target->IsInTown())
    {
      return 18;
    }
    if (m_Param.m_pGuild
        && CGuildRoomSystem::GetInstance()->IsGuildRoomMemberIn(
          m_Param.m_pGuild->m_dwSerial,
          m_ObjID.m_wIndex,
          m_pUserDB->m_dwSerial))
    {
      return 18;
    }
  }

  if (!nEffectCode)
  {
    if (skillField->m_nClass == 2)
    {
      const unsigned __int8 equipSkillError = EquipItemSFAgent.IsEnableSkill(skillField);
      if (equipSkillError)
      {
        return equipSkillError;
      }
    }
    else
    {
      if (!IsSFUsableSFMastery(3u, skillField->m_nMastIndex))
      {
        return 3;
      }
      if (!IsSFActableByClass(0, skillField))
      {
        return 16;
      }
    }
  }

  if (m_byMoveType == 2)
  {
    return 28;
  }
  if (!IsSFUseableRace(static_cast<unsigned __int8>(nEffectCode), static_cast<unsigned __int16>(skillField->m_dwIndex)))
  {
    return 4;
  }
  if (!IsSFUsableGauge(
        static_cast<unsigned __int8>(nEffectCode),
        static_cast<unsigned __int16>(skillField->m_dwIndex),
        gaugeCost))
  {
    return 6;
  }

  _STORAGE_LIST::_db_con *consumeItems[3]{};
  int consumeCount[3]{};
  bool overlap[3]{};
  if (!GetUseConsumeItem(skillField->m_ConsumeItemList, pConsumeSerial, consumeItems, consumeCount, overlap))
  {
    return 32;
  }

  if (!nEffectCode)
  {
    const float totalSkillLevel = static_cast<float>(m_pmMst.GetSkillLv(static_cast<unsigned __int8>(nSkillIndex)))
      + m_EP.GetEff_Plus(19);
    skillLevel = static_cast<int>(totalSkillLevel);
    if (skillLevel > 7)
    {
      skillLevel = 7;
    }
  }
  if (pnLv)
  {
    *pnLv = skillLevel;
  }

  unsigned __int8 errorCode = 0;
  bool upMastery = false;
  const bool assistSuccess = AssistSkill(target, nEffectCode, skillField, skillLevel, &errorCode, &upMastery) != 0;
  if (assistSuccess)
  {
    if (upMastery && !IsInTown())
    {
      bool inGuildRoom = false;
      if (m_Param.m_pGuild)
      {
        inGuildRoom = CGuildRoomSystem::GetInstance()->IsGuildRoomMemberIn(
          m_Param.m_pGuild->m_dwSerial,
          m_ObjID.m_wIndex,
          m_pUserDB->m_dwSerial);
      }

      if (!inGuildRoom
          && skillField->m_nClass != 2
          && skillField->m_nClass != 4
          && !nEffectCode
          && skillField->m_nMastIndex < 8u)
      {
        unsigned int alterValue = 1;
        if (skillField->m_nClass == 1)
        {
          alterValue = 2;
        }

        if (GetObjRace() != target->GetObjRace() || !IsChaosMode() || skillField->m_nContEffectType)
        {
          if (GetObjRace() != target->GetObjRace() || target->m_ObjID.m_byID)
          {
            Emb_AlterStat(
              3u,
              static_cast<unsigned __int8>(skillField->m_dwIndex),
              static_cast<int>(alterValue),
              0,
              "CPlayer::skill_process()---0",
              true);
          }
          else
          {
            CPlayer *targetPlayer = static_cast<CPlayer *>(target);
            if (!targetPlayer->IsPunished(1u, false))
            {
              Emb_AlterStat(
                3u,
                static_cast<unsigned __int8>(skillField->m_dwIndex),
                static_cast<int>(alterValue),
                0,
                "CPlayer::skill_process()---0",
                true);
            }
          }
        }
      }
    }

    if (skillField->m_nClass == 4)
    {
      m_tmrAuraSkill.BeginTimer(0x1388u);
      skill_process_for_aura(static_cast<int>(skillField->m_dwIndex));
    }

    for (int paramCode = 0; paramCode < 3; ++paramCode)
    {
      if (!gaugeCost[paramCode])
      {
        continue;
      }

      const int currentGauge = GetGauge(paramCode);
      const int nextGauge = (currentGauge - gaugeCost[paramCode] > 0) ? (currentGauge - gaugeCost[paramCode]) : 0;
      SetGauge(paramCode, nextGauge, true);
    }

    SendMsg_Recover();
    DeleteUseConsumeItem(consumeItems, consumeCount, overlap);
    const float addDelay = m_EP.GetEff_Plus(12);
    _ATTACK_DELAY_CHECKER::SetDelay(&m_AttDelayChker, static_cast<unsigned int>(skillField->m_fActDelay + addDelay));
  }

  return errorCode;
}

void CPlayer::pc_SkillRequest(unsigned __int8 bySkillIndex, _CHRID *pidDst, unsigned __int16 *pConsumeSerial)
{
  int skillLevel = 1;
  const bool wasStealth = GetStealth(true);
  const unsigned __int8 errCode = skill_process(0, bySkillIndex, pidDst, pConsumeSerial, &skillLevel);
  if ((!errCode || errCode == 100) && wasStealth)
  {
    BreakStealth();
  }
  SendMsg_SkillResult(errCode, pidDst, bySkillIndex, skillLevel);
}

void CPlayer::pc_ClassSkillRequest(unsigned __int16 wSkillIndex, _CHRID *pidDst, unsigned __int16 *pConsumeSerial)
{
  const bool wasStealth = GetStealth(true);
  const unsigned __int8 errCode = skill_process(2, wSkillIndex, pidDst, pConsumeSerial, nullptr);
  if ((!errCode || errCode == 100) && wasStealth)
  {
    BreakStealth();
  }
  SendMsg_ClassSkillResult(errCode, pidDst, wSkillIndex);
}

void CPlayer::pc_ForceRequest(unsigned __int16 wForceSerial, _CHRID *pidDst, unsigned __int16 *pConsumeSerial)
{
  unsigned __int8 byErrCode = 0;
  _STORAGE_LIST::_db_con *forceItem = nullptr;
  _force_fld *forceFld = nullptr;
  CCharacter *target = nullptr;
  unsigned __int16 delPoint[3]{};
  _STORAGE_LIST::_db_con *consumeItems[3]{};
  int consumeCounts[3]{};
  bool consumeOverlap[3]{};
  int forceLevel = 1;

  if (IsRidingUnit())
  {
    byErrCode = 14;
  }
  else if (m_byMoveType == 2)
  {
    byErrCode = 28;
  }
  else if (m_EP.GetEff_State(20) || m_EP.GetEff_State(28))
  {
    byErrCode = 24;
  }
  else if (m_EP.GetEff_State(21))
  {
    byErrCode = 25;
  }
  else if (m_dwSelfDestructionTime)
  {
    byErrCode = 26;
  }
  else if (m_bInGuildBattle && m_bTakeGravityStone)
  {
    byErrCode = 27;
  }

  if (!byErrCode)
  {
    forceItem = m_Param.m_dbForce.GetPtrFromSerial(wForceSerial);
    if (!forceItem)
    {
      byErrCode = 1;
    }
    else if (forceItem->m_bLock)
    {
      byErrCode = 17;
    }
  }

  if (!byErrCode)
  {
    const int effectIndex = CPlayer::s_pnLinkForceItemToEffect
      ? CPlayer::s_pnLinkForceItemToEffect[forceItem->m_wItemIndex]
      : -1;
    forceFld = reinterpret_cast<_force_fld *>(g_Main.m_tblEffectData[1].GetRecord(effectIndex));
    if (!forceFld)
    {
      byErrCode = 1;
    }
  }

  if (!byErrCode && !m_bSFDelayNotCheck
      && !_ATTACK_DELAY_CHECKER::IsDelay(&m_AttDelayChker, 1u, forceFld->m_dwIndex, forceFld->m_nMastIndex))
  {
    byErrCode = 9;
  }
  if (!byErrCode && forceFld->m_nTempEffectType >= 150)
  {
    byErrCode = 21;
  }

  if (!byErrCode)
  {
    target = GetLiveTargetInSameMap(this, pidDst);
    if (!target)
    {
      byErrCode = 2;
    }
    else if (!IsEffectableDst(forceFld->m_strActableDst, target))
    {
      byErrCode = 5;
    }
    else if (forceFld->m_nContEffectType == 0 && !IsAttackableInTown() && !target->IsAttackableInTown())
    {
      const bool inGuildRoom = m_Param.m_pGuild
        && CGuildRoomSystem::GetInstance()->IsGuildRoomMemberIn(
          m_Param.m_pGuild->m_dwSerial,
          m_ObjID.m_wIndex,
          m_pUserDB->m_dwSerial);
      if (IsInTown() || target->IsInTown() || inGuildRoom)
      {
        byErrCode = 18;
      }
    }
  }

  if (!byErrCode && forceFld->m_nTempEffectType == -1 && forceFld->m_nContEffectType == -1)
  {
    byErrCode = 8;
  }
  if (!byErrCode && forceFld->m_nContEffectType != -1 && !target->IsRecvableContEffect())
  {
    byErrCode = 13;
  }
  if (!byErrCode && target->m_EP.GetEff_State(20)
      && (forceFld->m_nTempEffectType == -1 || !IsUsableTempEffectAtStoneState(forceFld->m_nTempEffectType)))
  {
    byErrCode = 24;
  }
  if (!byErrCode && target->m_EP.GetEff_State(28))
  {
    byErrCode = 24;
  }

  if (!byErrCode && !IsSFUsableSFMastery(4u, forceFld->m_nMastIndex))
  {
    byErrCode = 3;
  }
  if (!byErrCode && !IsSFActableByClass(1u, reinterpret_cast<_base_fld *>(forceFld)))
  {
    byErrCode = 16;
  }
  if (!byErrCode && !IsSFUsableGauge(1u, static_cast<unsigned __int16>(forceFld->m_dwIndex), delPoint))
  {
    byErrCode = 6;
  }
  if (!byErrCode && !GetUseConsumeItem(forceFld->m_ConsumeItemList, pConsumeSerial, consumeItems, consumeCounts, consumeOverlap))
  {
    byErrCode = 32;
  }

  if (!byErrCode)
  {
    forceLevel = GetSFLevel(forceFld->m_nLv, forceItem->m_dwDur);
    const bool hadStealth = GetStealth(true);
    bool upMastery = false;
    unsigned __int8 assistErr = 0;
    const bool success = AssistForce(target, forceFld, forceLevel, &assistErr, &upMastery) != 0;
    byErrCode = assistErr;

    if (success)
    {
      if (upMastery && !IsInTown())
      {
        bool canGainMastery = true;
        if (IsChaosMode() && GetObjRace() == target->GetObjRace() && forceFld->m_nContEffectType == 0)
        {
          if (target->m_ObjID.m_byID == 0)
          {
            CPlayer *targetPlayer = static_cast<CPlayer *>(target);
            if (targetPlayer->IsPunished(1u, 0))
            {
              canGainMastery = false;
            }
          }
          else
          {
            canGainMastery = false;
          }
        }

        if (canGainMastery)
        {
          Emb_AlterStat(4u, forceFld->m_nMastIndex, 1u, 0, "CPlayer::pc_ForceRequest()---0", true);

          const int level = static_cast<int>(GetLevel());
          const int baseAlter = level / 10 + 1;
          int alter = baseAlter;
          if (IsApplyPcbangPrimium())
          {
            alter = static_cast<int>(
              static_cast<float>(alter)
              + static_cast<float>(baseAlter)
                  * static_cast<float>(PCBANG_PRIMIUM_FAVOR::SKILL_FORCE_MASTERY - 1.0f));
          }
          else
          {
            alter = static_cast<int>(
              static_cast<float>(alter)
              + static_cast<float>(baseAlter)
                  * static_cast<float>(FORCE_LIVER_ACCUM_RATE - 1.0f));
          }

          const float effHave = m_EP.GetEff_Have(6);
          if (effHave > 1.0f)
          {
            alter = static_cast<int>(
              static_cast<float>(alter)
              + static_cast<float>(baseAlter)
                  * static_cast<float>(effHave - 1.0f));
          }

          const unsigned int oldDur = forceItem->m_dwDur;
          unsigned int newDur = oldDur;
          if (alter > 0)
          {
            newDur = Emb_AlterDurPoint(forceItem->m_pInList->m_nListCode, forceItem->m_byStorageIndex, alter, 0, 0);
          }
          if (newDur != oldDur)
          {
            SendMsg_FcitemInform(wForceSerial, newDur);
          }
        }
      }

      ApplyGaugeConsumeAndSendRecover(this, delPoint);
      DeleteUseConsumeItem(consumeItems, consumeCounts, consumeOverlap);
      const float addDelay = m_EP.GetEff_Plus(13);
      _ATTACK_DELAY_CHECKER::SetDelay(
        &m_AttDelayChker,
        static_cast<unsigned int>(forceFld->m_fActDelay + addDelay));
      if (hadStealth)
      {
        BreakStealth();
      }
    }
  }

  this->SendMsg_ForceResult(byErrCode, pidDst, forceItem, static_cast<unsigned __int8>(forceLevel));
}

void CPlayer::pc_ThrowSkillRequest(unsigned __int16 wBulletSerial, _CHRID *pidDst, unsigned __int16 *pConsumeSerial)
{
  unsigned __int8 byErrCode = 0;
  _STORAGE_LIST::_db_con *bulletItem = nullptr;
  _BulletItem_fld *bulletRecord = nullptr;
  _skill_fld *skillFld = nullptr;
  CCharacter *target = nullptr;
  _STORAGE_LIST::_db_con *consumeItems[3]{};
  int consumeCounts[3]{};
  bool consumeOverlap[3]{};
  unsigned __int8 bySkillIndex = static_cast<unsigned __int8>(-1);

  if (IsRidingUnit())
  {
    byErrCode = 14;
  }
  else if (m_byMoveType == 2)
  {
    byErrCode = 28;
  }
  else if (m_EP.GetEff_State(20) || m_EP.GetEff_State(28))
  {
    byErrCode = 24;
  }
  else if (m_EP.GetEff_State(21))
  {
    byErrCode = 25;
  }
  else if (m_pmWpn.byWpType != 11 && m_pmWpn.byWpType != 6)
  {
    byErrCode = 22;
  }

  if (!byErrCode && m_pmWpn.byWpType == 6
      && (!m_pmWpn.pFixWp
          || m_pmWpn.pFixWp->m_byTableCode != 6
          || m_pmWpn.pFixWp->m_wItemIndex != 8518))
  {
    byErrCode = 22;
  }

  if (!byErrCode)
  {
    bulletItem = IsBulletValidity(wBulletSerial);
    if (!bulletItem)
    {
      byErrCode = 19;
    }
    else if (bulletItem->m_bLock)
    {
      byErrCode = 20;
    }
    else
    {
      bulletRecord = reinterpret_cast<_BulletItem_fld *>(g_Main.m_tblItemData[10].GetRecord(bulletItem->m_wItemIndex));
      if (!bulletRecord)
      {
        byErrCode = 19;
      }
      else if (!strncmp(bulletRecord->m_strEffectIndex, "-1", 2))
      {
        byErrCode = 19;
      }
      else
      {
        skillFld = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[3].GetRecord(bulletRecord->m_strEffectIndex));
        if (!skillFld)
        {
          byErrCode = 8;
        }
      }
    }
  }

  if (!byErrCode && !m_bSFDelayNotCheck
      && !_ATTACK_DELAY_CHECKER::IsDelay(&m_AttDelayChker, 0xFFu, 0xFFu, 0xFFu))
  {
    byErrCode = 9;
  }

  if (!byErrCode)
  {
    target = GetLiveTargetInSameMap(this, pidDst);
    if (!target)
    {
      byErrCode = 2;
    }
    else if (!IsEffectableDst(skillFld->m_strActableDst, target))
    {
      byErrCode = 5;
    }
    else if (skillFld->m_nTempEffectType == -1 && skillFld->m_nContEffectType == -1)
    {
      byErrCode = 8;
    }
    else if (skillFld->m_nContEffectType != -1)
    {
      if (!target->IsRecvableContEffect())
      {
        byErrCode = 13;
      }
      else if (skillFld->m_nContEffectType == 0 && !IsAttackableInTown() && !target->IsAttackableInTown())
      {
        const bool inGuildRoom = m_Param.m_pGuild
          && CGuildRoomSystem::GetInstance()->IsGuildRoomMemberIn(
            m_Param.m_pGuild->m_dwSerial,
            m_ObjID.m_wIndex,
            m_pUserDB->m_dwSerial);
        if (IsInTown() || target->IsInTown() || inGuildRoom)
        {
          byErrCode = 18;
        }
      }
    }
  }

  if (!byErrCode && !GetUseConsumeItem(skillFld->m_ConsumeItemList, pConsumeSerial, consumeItems, consumeCounts, consumeOverlap))
  {
    byErrCode = 32;
  }

  if (!byErrCode && skillFld->m_nContEffectType != -1)
  {
    CPlayer *dstPlayer = target && target->m_ObjID.m_byID == 0 ? static_cast<CPlayer *>(target) : nullptr;
    if (skillFld->m_nContEffectType == 1)
    {
      if (dstPlayer && _pre_check_in_guild_battle_race(dstPlayer, false))
      {
        byErrCode = 13;
      }
    }
    else if (dstPlayer && _pre_check_in_guild_battle_race(dstPlayer, true))
    {
      byErrCode = 13;
    }
  }

  g_tmpEffectedNum = 0;
  if (!byErrCode)
  {
    bySkillIndex = static_cast<unsigned __int8>(skillFld->m_dwIndex);
    const bool hadStealth = GetStealth(true);
    bool upMastery = false;
    unsigned __int8 assistErr = 0;
    const bool success = AssistSkill(target, 3u, skillFld, 1, &assistErr, &upMastery) != 0;
    byErrCode = assistErr;

    if (success)
    {
      const unsigned __int16 leftDur = Emb_AlterDurPoint(2u, bulletItem->m_byStorageIndex, -1, 0, 1);
      if (leftDur)
      {
        SendMsg_AlterWeaponBulletInform(bulletItem->m_wSerial, leftDur);
      }
      else
      {
        CPlayer::s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex, bulletItem, m_szItemHistoryFileName);
      }

      DeleteUseConsumeItem(consumeItems, consumeCounts, consumeOverlap);

      const int addDelay = static_cast<int>(CalcEquipAttackDelay());
      const int level = static_cast<int>(GetLevel());
      int attackDelay = static_cast<int>(m_pmWpn.GetAttackDelay(level, addDelay));
      if (m_pmWpn.byWpType != 11 && !m_pmWpn.byWpClass)
      {
        attackDelay = static_cast<int>(static_cast<float>(attackDelay) + m_EP.GetEff_Plus(9));
      }
      if (m_pmWpn.byWpType == 7 || m_pmWpn.byWpType == 11)
      {
        attackDelay = static_cast<int>(static_cast<float>(attackDelay) + m_EP.GetEff_Plus(11));
      }
      _ATTACK_DELAY_CHECKER::SetDelay(&m_AttDelayChker, static_cast<unsigned int>(attackDelay));

      if (hadStealth)
      {
        BreakStealth();
      }
    }
  }

  this->SendMsg_ThrowSkillResult(byErrCode, pidDst, bySkillIndex);
}

void CPlayer::pc_ThrowUnitRequest(_CHRID *pidDst, unsigned __int16 *pConsumeSerial)
{
  unsigned __int8 byErrCode = 0;
  _UnitBullet_fld *bulletRecord = nullptr;
  _UnitPart_fld *weaponRecord = nullptr;
  _skill_fld *skillFld = nullptr;
  CCharacter *target = nullptr;
  unsigned int *bulletParam = nullptr;
  _STORAGE_LIST::_db_con *consumeItems[3]{};
  int consumeCounts[3]{};
  bool consumeOverlap[3]{};
  unsigned __int16 wBulletIndex = static_cast<unsigned __int16>(-1);

  if (!IsRidingUnit())
  {
    byErrCode = 23;
  }
  else
  {
    bulletParam = &m_pUsingUnit->dwBullet[1];
    const unsigned __int16 bulletLeft = HIWORD(*bulletParam);
    const unsigned __int16 bulletItemIndex = LOWORD(*bulletParam);
    if (!bulletLeft || bulletLeft == 0xFFFF)
    {
      byErrCode = 19;
    }
    else
    {
      bulletRecord = reinterpret_cast<_UnitBullet_fld *>(g_Main.m_tblUnitBullet.GetRecord(bulletItemIndex));
      weaponRecord = reinterpret_cast<_UnitPart_fld *>(g_Main.m_tblUnitPart[4].GetRecord(m_pUsingUnit->byPart[4]));
      if (!bulletRecord || !weaponRecord)
      {
        byErrCode = 21;
      }
      else if (!strncmp(bulletRecord->m_strEffectIndex, "-1", 2))
      {
        byErrCode = 19;
      }
      else
      {
        skillFld = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[3].GetRecord(bulletRecord->m_strEffectIndex));
        if (!skillFld)
        {
          byErrCode = 8;
        }
      }
    }
  }

  if (!byErrCode && !m_bSFDelayNotCheck
      && !_ATTACK_DELAY_CHECKER::IsDelay(&m_AttDelayChker, 0xFFu, 0xFFu, 0xFFu))
  {
    byErrCode = 9;
  }

  if (!byErrCode)
  {
    target = GetLiveTargetInSameMap(this, pidDst);
    if (!target)
    {
      byErrCode = 2;
    }
    else if (!IsEffectableDst(skillFld->m_strActableDst, target))
    {
      byErrCode = 5;
    }
    else if (skillFld->m_nTempEffectType == -1 && skillFld->m_nContEffectType == -1)
    {
      byErrCode = 8;
    }
    else if (skillFld->m_nContEffectType != -1)
    {
      if (!target->IsRecvableContEffect())
      {
        byErrCode = 13;
      }
      else if (skillFld->m_nContEffectType == 0 && !IsAttackableInTown() && !target->IsAttackableInTown())
      {
        const bool inGuildRoom = m_Param.m_pGuild
          && CGuildRoomSystem::GetInstance()->IsGuildRoomMemberIn(
            m_Param.m_pGuild->m_dwSerial,
            m_ObjID.m_wIndex,
            m_pUserDB->m_dwSerial);
        if (IsInTown() || target->IsInTown() || inGuildRoom)
        {
          byErrCode = 18;
        }
      }
    }
  }

  if (!byErrCode && !GetUseConsumeItem(skillFld->m_ConsumeItemList, pConsumeSerial, consumeItems, consumeCounts, consumeOverlap))
  {
    byErrCode = 32;
  }

  if (!byErrCode && skillFld->m_nContEffectType != -1)
  {
    CPlayer *dstPlayer = target && target->m_ObjID.m_byID == 0 ? static_cast<CPlayer *>(target) : nullptr;
    if (skillFld->m_nContEffectType == 1)
    {
      if (dstPlayer && _pre_check_in_guild_battle_race(dstPlayer, false))
      {
        byErrCode = 13;
      }
    }
    else if (dstPlayer && _pre_check_in_guild_battle_race(dstPlayer, true))
    {
      byErrCode = 13;
    }
  }

  g_tmpEffectedNum = 0;
  if (!byErrCode)
  {
    wBulletIndex = static_cast<unsigned __int16>(bulletRecord->m_dwIndex);
    const bool hadStealth = GetStealth(true);
    bool upMastery = false;
    unsigned __int8 assistErr = 0;
    const bool success = AssistSkill(target, 3u, skillFld, 1, &assistErr, &upMastery) != 0;
    byErrCode = assistErr;

    if (success)
    {
      unsigned __int16 bulletLeft = HIWORD(*bulletParam);
      if (bulletLeft > 0)
      {
        --bulletLeft;
      }
      SendMsg_AlterUnitBulletInform(1u, bulletLeft);
      if (!bulletLeft)
      {
        *bulletParam = static_cast<unsigned int>(-1);
      }

      DeleteUseConsumeItem(consumeItems, consumeCounts, consumeOverlap);
      _ATTACK_DELAY_CHECKER::SetDelay(&m_AttDelayChker, static_cast<unsigned int>(weaponRecord->m_nAttackDel));
      if (hadStealth)
      {
        BreakStealth();
      }
    }
  }

  this->SendMsg_ThrowUnitResult(byErrCode, pidDst, wBulletIndex);
}

