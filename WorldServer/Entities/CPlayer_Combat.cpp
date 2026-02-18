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
char s_playerObjectName[256]{};

int ClampToleranceValue(int value)
{
  if (value < -100)
  {
    return -100;
  }
  if (value > 100)
  {
    return 100;
  }
  return value;
}

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


namespace
{

unsigned int GenerateTransientObjSerial()
{
  static unsigned int s_serialCounter = 1;
  return ++s_serialCounter;
}

CTrap *FindEmptyTrapSlot()
{
  for (int index = 0; index < MAX_TRAP; ++index)
  {
    if (!g_Trap[index].m_bLive)
    {
      return &g_Trap[index];
    }
  }
  return nullptr;
}

CTrap *CreateTrapObject(CPlayer *player, unsigned __int16 trapItemIndex, float *position)
{
  CTrap *slot = FindEmptyTrapSlot();
  if (!slot)
  {
    return nullptr;
  }

  _character_create_setdata createData{};
  createData.m_pMap = player->m_pCurMap;
  createData.m_nLayerIndex = player->m_wMapLayerIndex;
  createData.m_pRecordSet = g_Main.m_tblItemData[26].GetRecord(trapItemIndex);
  if (!createData.m_pRecordSet)
  {
    return nullptr;
  }
  std::memcpy(createData.m_fStartPos, position, sizeof(createData.m_fStartPos));
  if (!slot->Create(&createData))
  {
    return nullptr;
  }

  slot->m_dwObjSerial = GenerateTransientObjSerial();
  slot->m_pMaster = player;
  slot->m_dwMasterSerial = player->m_dwObjSerial;
  slot->m_byRaceCode = static_cast<unsigned __int8>(player->m_Param.GetRaceCode());
  slot->m_dwStartMakeTime = GetLoopTime();
  slot->m_nTrapMaxAttackPnt = player->m_nTrapMaxAttackPnt;

  _TrapItem_fld *trapInfo = reinterpret_cast<_TrapItem_fld *>(createData.m_pRecordSet);
  slot->m_nHP = static_cast<int>(trapInfo->m_fMaxHP);
  return slot;
}

CGuardTower *CreateGuardTower(
  CMapData *pMap,
  unsigned __int16 wLayer,
  float *fPos,
  _STORAGE_LIST::_db_con *pItem,
  CPlayer *pMaster,
  unsigned __int8 byRaceCode,
  bool bQuick)
{
  CGuardTower *tower = nullptr;
  for (int index = 0; index < MAX_TOWER; ++index)
  {
    if (!g_Tower[index].m_bLive)
    {
      tower = &g_Tower[index];
      break;
    }
  }

  if (!tower)
  {
    return nullptr;
  }

  _tower_create_setdata data{};
  data.m_pMap = pMap;
  data.m_nLayerIndex = wLayer;
  data.m_pRecordSet = g_Main.m_tblItemData[25].GetRecord(pItem->m_wItemIndex);
  if (!data.m_pRecordSet)
  {
    return nullptr;
  }

  std::memcpy(data.m_fStartPos, fPos, sizeof(data.m_fStartPos));
  data.pMaster = pMaster;
  data.byRaceCode = byRaceCode;
  data.pItem = pItem;
  data.bQuick = bQuick;

  return tower->Create(&data) ? tower : nullptr;
}

char IsOtherTowerNear(CGameObject *pEster, float *pfEstPos, CGuardTower *pEstObj)
{
  if (!pEster || !pEster->m_pCurMap)
  {
    return 0;
  }

  _pnt_rect rect{};
  const int curSecNum = static_cast<int>(pEster->GetCurSecNum());
  pEster->m_pCurMap->GetRectInRadius(&rect, 3, curSecNum);

  for (int y = rect.nStarty; y <= rect.nEndy; ++y)
  {
    for (int x = rect.nStartx; x <= rect.nEndx; ++x)
    {
      _sec_info *secInfo = pEster->m_pCurMap->GetSecInfo();
      const unsigned int secIndex = secInfo->m_nSecNumW * y + x;
      CObjectList *sectorList = pEster->m_pCurMap->GetSectorListObj(pEster->m_wMapLayerIndex, secIndex);
      if (!sectorList)
      {
        continue;
      }

      _object_list_point *node = sectorList->m_Head.m_pNext;
      while (node != &sectorList->m_Tail)
      {
        CGuardTower *tower = static_cast<CGuardTower *>(node->m_pItem);
        node = node->m_pNext;

        if (!tower->m_ObjID.m_byKind
            && tower->m_ObjID.m_byID == 4
            && pEstObj != tower
            && std::abs(tower->m_fCurPos[1] - pfEstPos[1]) <= 100.0f)
        {
          const int nearRange = *reinterpret_cast<int *>(&tower->m_pRecordSet[5].m_strCode[24]);
          const double dist = GetSqrt(tower->m_fCurPos, pfEstPos);
          if (static_cast<double>(nearRange) > dist)
          {
            return 1;
          }
        }
      }
    }
  }

  return 0;
}

char IsOtherInvalidObjNear(CGameObject *pEster, float *pfEstPos, CTrap *pEstObj, _TrapItem_fld *pEstTrapItemInfo)
{
  if (!pEster || !pfEstPos)
  {
    return 0;
  }

  const float checkRange = pEstTrapItemInfo ? pEstTrapItemInfo->m_fReactionDst : 50.0f;
  for (int index = 0; index < MAX_TRAP; ++index)
  {
    CTrap *trap = &g_Trap[index];
    if (!trap->m_bLive || trap == pEstObj)
    {
      continue;
    }
    if (std::fabs(trap->m_fCurPos[1] - pfEstPos[1]) > 100.0f)
    {
      continue;
    }
    if (GetSqrt(trap->m_fCurPos, pfEstPos) < checkRange)
    {
      return 10;
    }
  }

  if (IsOtherTowerNear(pEster, pfEstPos, nullptr))
  {
    return 10;
  }

  return 0;
}

} // namespace

void CPlayer::SendMsg_CreateTowerResult(char byErrCode, unsigned int dwTowerObjSerial)
{

  _make_tower_result_zocl packet{};
  packet.byErrCode = byErrCode;
  packet.dwTowerObjSerial = dwTowerObjSerial;
  packet.wLeftFP = static_cast<unsigned __int16>(this->GetFP());

  unsigned __int8 type[2] = {17, 19};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_BackTowerResult(char byErrCode, unsigned __int16 wItemSerial, unsigned __int16 wLeftHP)
{

  _back_tower_result_zocl packet{};
  packet.byErrCode = byErrCode;
  packet.wItemSerial = wItemSerial;
  packet.wLeftHP = wLeftHP;

  unsigned __int8 type[2] = {17, 21};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_CreateTrapResult(char byErrCode, unsigned int dwTrapObjSerial)
{

  _make_trap_result_zocl packet{};
  packet.byErrCode = byErrCode;
  packet.dwTrapObjSerial = dwTrapObjSerial;
  packet.wLeftFP = static_cast<unsigned __int16>(this->GetFP());

  unsigned __int8 type[2] = {17, 28};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_BackTrapResult(char byErrCode)
{
  unsigned __int8 type[2] = {17, 39};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &byErrCode, 1u);
}

void CPlayer::SendMsg_MadeTrapNumInform(char byNum)
{
  unsigned __int8 type[2] = {17, 29};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &byNum, 1u);
}

void CPlayer::SendMsg_TowerContinue(unsigned __int16 wItemSerial, CGuardTower *pTwr)
{
_continue_tower_inform msg{};

  msg.wItemSerial = wItemSerial;
  msg.wTwrRecIndex = static_cast<unsigned __int16>(pTwr->m_pRecordSet->m_dwIndex);
  msg.wTwrIndex = pTwr->m_ObjID.m_wIndex;
  msg.dwTwrSerial = pTwr->m_dwObjSerial;

  unsigned __int8 type[2] = {17, 30};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 0xAu);
}

void CPlayer::pc_MakeTowerRequest(
  unsigned __int16 wSkillIndex,
  unsigned __int16 wTowerItemSerial,
  unsigned __int8 byMaterialNum,
  _make_tower_request_clzo::__material *pMaterial,
  float *pfPos,
  unsigned __int16 *pConsumeSerial)
{
  unsigned __int8 byErrCode = 0;
  unsigned int dwTowerObjSerial = static_cast<unsigned int>(-1);
  _skill_fld *classSkill = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[2].GetRecord(wSkillIndex));
  _GuardTowerItem_fld *towerItemInfo = nullptr;
  _STORAGE_LIST::_db_con *towerItem = nullptr;
  _STORAGE_LIST::_db_con *materialItems[30]{};
  unsigned __int8 materialTotals[3]{};
  int classGrade[2] = {-1, -1};
  _STORAGE_LIST::_db_con *consumeItems[3]{};
  int consumeCounts[3]{};
  bool consumeOverlap[3]{};
  int needFp = 0;

  if (m_byPosRaceTown != 0xFF)
  {
    byErrCode = 16;
  }
  else if (!classSkill)
  {
    byErrCode = 13;
  }
  else if (m_byMoveType == 2)
  {
    byErrCode = 13;
  }
  else if (m_pmTwr.m_nCount >= 6)
  {
    byErrCode = 11;
  }
  else if (!m_Param.IsActableClassSkill(classSkill->m_strCode, classGrade))
  {
    byErrCode = 13;
  }
  else if (!m_bSFDelayNotCheck
           && !_ATTACK_DELAY_CHECKER::IsDelay(&m_AttDelayChker, 2u, classSkill->m_dwIndex, classGrade[0]))
  {
    byErrCode = 18;
  }
  else if (!GetUseConsumeItem(classSkill->m_ConsumeItemList, pConsumeSerial, consumeItems, consumeCounts, consumeOverlap))
  {
    byErrCode = 20;
  }
  else if (GetSqrt(m_fCurPos, pfPos) > 40.0)
  {
    byErrCode = 9;
  }
  else if (IsOtherTowerNear(this, pfPos, nullptr))
  {
    byErrCode = 15;
  }
  else if (classSkill->m_nTempEffectType == -1)
  {
    byErrCode = 12;
  }

  if (!byErrCode)
  {
    needFp = static_cast<int>(static_cast<float>(classSkill->m_nNeedFP) * m_EP.GetEff_Rate(7));
    if (needFp > GetFP())
    {
      byErrCode = 14;
    }
  }

  if (!byErrCode)
  {
    towerItem = m_Param.m_dbInven.GetPtrFromSerial(wTowerItemSerial);
    if (!towerItem)
    {
      byErrCode = 2;
    }
    else if (towerItem->m_byTableCode != 25)
    {
      byErrCode = 3;
    }
    else if (towerItem->m_bLock)
    {
      byErrCode = 17;
    }
    else
    {
      towerItemInfo = reinterpret_cast<_GuardTowerItem_fld *>(g_Main.m_tblItemData[25].GetRecord(towerItem->m_wItemIndex));
      if (!towerItemInfo)
      {
        byErrCode = 12;
      }
    }
  }

  if (!byErrCode)
  {
    for (int index = 0; index < byMaterialNum; ++index)
    {
      const unsigned __int16 serial = pMaterial[index].wItemSerial;
      const unsigned __int8 slot = pMaterial[index].byMaterSlotIndex;
      const unsigned __int8 amount = pMaterial[index].byAmount;
      if (slot >= 3)
      {
        byErrCode = 4;
        break;
      }

      for (int prev = 0; prev < index; ++prev)
      {
        if (serial == pMaterial[prev].wItemSerial)
        {
          byErrCode = 4;
          break;
        }
      }
      if (byErrCode)
      {
        break;
      }

      _STORAGE_LIST::_db_con *material = m_Param.m_dbInven.GetPtrFromSerial(serial);
      materialItems[index] = material;
      if (!material)
      {
        byErrCode = 5;
        break;
      }

      _base_fld *materialRecord = g_Main.m_tblItemData[material->m_byTableCode].GetRecord(material->m_wItemIndex);
      if (!materialRecord || strncmp(materialRecord->m_strCode, towerItemInfo->m_Material[slot].strMaterialCode, 7))
      {
        byErrCode = 6;
        break;
      }
      if (static_cast<unsigned __int64>(amount) > material->m_dwDur)
      {
        byErrCode = 7;
        break;
      }
      materialTotals[slot] = static_cast<unsigned __int8>(materialTotals[slot] + amount);
    }

    if (!byErrCode)
    {
      for (int slot = 0; slot < 3; ++slot)
      {
        if (materialTotals[slot] != towerItemInfo->m_Material[slot].nMaterialNum)
        {
          byErrCode = 8;
          break;
        }
      }
    }
  }

  if (!byErrCode)
  {
    const int currentFp = GetFP();
    const int remainFp = (currentFp > needFp) ? (currentFp - needFp) : 0;
    SetFP(remainFp, true);

    CGuardTower *tower = CreateGuardTower(
      m_pCurMap,
      m_wMapLayerIndex,
      pfPos,
      towerItem,
      this,
      static_cast<unsigned __int8>(m_Param.GetRaceCode()),
      false);
    if (!tower)
    {
      byErrCode = 1;
    }
    else
    {
      for (int index = 0; index < byMaterialNum; ++index)
      {
        Emb_AlterDurPoint(0u, materialItems[index]->m_byStorageIndex, -pMaterial[index].byAmount, false, false);
      }
      towerItem->lock(true);
      m_pmTwr.PushList(towerItem, tower);
      ++m_pmTwr.m_nCount;
      dwTowerObjSerial = tower->m_dwObjSerial;
      DeleteUseConsumeItem(consumeItems, consumeCounts, consumeOverlap);
      const float addDelay = m_EP.GetEff_Plus(12);
      _ATTACK_DELAY_CHECKER::SetDelay(
        &m_AttDelayChker,
        static_cast<unsigned int>(classSkill->m_fActDelay + addDelay));
    }
  }

  this->SendMsg_CreateTowerResult(static_cast<char>(byErrCode), dwTowerObjSerial);
}

void CPlayer::pc_BackTowerRequest(unsigned int dwTowerObjSerial)
{
  unsigned __int8 byErrCode = 0;
  _STORAGE_LIST::_db_con *towerItem = nullptr;
  CGuardTower *tower = nullptr;
  int towerSlot = -1;

  for (int index = 0; index < 6; ++index)
  {
    if (m_pmTwr.m_List[index].m_pTowerItem
        && m_pmTwr.m_List[index].m_pTowerObj
        && m_pmTwr.m_List[index].m_pTowerObj->m_dwObjSerial == dwTowerObjSerial)
    {
      towerSlot = index;
      tower = m_pmTwr.m_List[index].m_pTowerObj;
      towerItem = m_Param.m_dbInven.GetPtrFromSerial(m_pmTwr.m_List[index].m_pTowerItem->m_wSerial);
      break;
    }
  }

  if (!towerItem)
  {
    byErrCode = 2;
  }
  else if (towerItem->m_byTableCode != 25)
  {
    byErrCode = 3;
  }

  unsigned __int16 wItemSerial = static_cast<unsigned __int16>(-1);
  unsigned __int16 wLeftHp = 0;
  if (!byErrCode)
  {
    wItemSerial = towerItem->m_wSerial;
    if (tower)
    {
      const int nAlter = tower->m_nHP - static_cast<int>(towerItem->m_dwDur);
      Emb_AlterDurPoint(0u, towerItem->m_byStorageIndex, nAlter, false, false);
      int hp = tower->m_nHP;
      if (hp < 0)
      {
        hp = 0;
      }
      else if (hp > 0xFFFF)
      {
        hp = 0xFFFF;
      }
      wLeftHp = static_cast<unsigned __int16>(hp);
      tower->Destroy(2u, false);
    }
    towerItem->lock(false);
    if (towerSlot >= 0)
    {
      m_pmTwr.m_List[towerSlot].init();
      --m_pmTwr.m_nCount;
    }
  }

  this->SendMsg_BackTowerResult(static_cast<char>(byErrCode), wItemSerial, wLeftHp);
}

void CPlayer::pc_MakeTrapRequest(
  unsigned __int16 wSkillIndex,
  unsigned __int16 wTrapItemSerial,
  float *pfPos,
  unsigned __int16 *pConsumeSerial)
{
  unsigned __int8 byErrCode = 0;
  unsigned int dwTrapObjSerial = static_cast<unsigned int>(-1);
  _skill_fld *classSkill = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[2].GetRecord(wSkillIndex));
  _TrapItem_fld *trapItemInfo = nullptr;
  _STORAGE_LIST::_db_con *trapItem = nullptr;
  _STORAGE_LIST::_db_con *consumeItems[3]{};
  int consumeCounts[3]{};
  bool consumeOverlap[3]{};
  int classGrade[2] = {-1, -1};
  int needFp = 0;

  if (m_byPosRaceTown != 0xFF)
  {
    byErrCode = 16;
  }
  else if (!classSkill)
  {
    byErrCode = 13;
  }
  else if (m_byMoveType == 2)
  {
    byErrCode = 13;
  }
  else if (m_pmTrp.m_nCount >= m_Param.m_nMakeTrapMaxNum)
  {
    byErrCode = 11;
  }
  else if (!m_Param.IsActableClassSkill(classSkill->m_strCode, classGrade))
  {
    byErrCode = 13;
  }
  else if (!m_bSFDelayNotCheck
           && !_ATTACK_DELAY_CHECKER::IsDelay(&m_AttDelayChker, 2u, classSkill->m_dwIndex, classGrade[0]))
  {
    byErrCode = 18;
  }
  else if (!GetUseConsumeItem(classSkill->m_ConsumeItemList, pConsumeSerial, consumeItems, consumeCounts, consumeOverlap))
  {
    byErrCode = 20;
  }
  else if (GetSqrt(m_fCurPos, pfPos) > 40.0)
  {
    byErrCode = 9;
  }
  else
  {
    trapItem = m_Param.m_dbInven.GetPtrFromSerial(wTrapItemSerial);
    if (!trapItem)
    {
      byErrCode = 2;
    }
    else if (trapItem->m_byTableCode != 26)
    {
      byErrCode = 3;
    }
    else
    {
      trapItemInfo = reinterpret_cast<_TrapItem_fld *>(g_Main.m_tblItemData[26].GetRecord(trapItem->m_wItemIndex));
      if (!trapItemInfo)
      {
        byErrCode = 2;
      }
      else
      {
        byErrCode = static_cast<unsigned __int8>(IsOtherInvalidObjNear(this, pfPos, nullptr, trapItemInfo));
      }
    }
  }

  if (!byErrCode && classSkill->m_nTempEffectType == -1)
  {
    byErrCode = 12;
  }
  if (!byErrCode)
  {
    needFp = static_cast<int>(static_cast<float>(classSkill->m_nNeedFP) * m_EP.GetEff_Rate(7));
    if (needFp > GetFP())
    {
      byErrCode = 14;
    }
    else if (trapItemInfo->m_nLevelLim >= 40
             && (!m_Param.m_pClassHistory[0]
                 || m_Param.m_pClassHistory[0]->m_nClass != 1
                 || !m_Param.m_pClassHistory[1]
                 || m_Param.m_pClassHistory[1]->m_nClass != 1))
    {
      byErrCode = 19;
    }
  }

  if (!byErrCode)
  {
    const int currentFp = GetFP();
    const int remainFp = (currentFp > needFp) ? (currentFp - needFp) : 0;
    SetFP(remainFp, true);

    CTrap *trap = CreateTrapObject(this, trapItem->m_wItemIndex, pfPos);
    if (!trap)
    {
      byErrCode = 1;
    }
    else
    {
      if (IsOverLapItem(26))
      {
        Emb_AlterDurPoint(0u, trapItem->m_byStorageIndex, -1, true, true);
      }
      else if (!Emb_DelStorage(0u, trapItem->m_byStorageIndex, false, true, "CPlayer::pc_MakeTrapRequest()"))
      {
        this->SendMsg_CreateTrapResult(12, dwTrapObjSerial);
        return;
      }

      CPlayer::s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex, trapItem, m_szItemHistoryFileName);
      m_pmTrp.PushItem(trap, trap->m_dwObjSerial);
      SendMsg_MadeTrapNumInform(static_cast<char>(m_pmTrp.m_nCount));
      dwTrapObjSerial = trap->m_dwObjSerial;
      DeleteUseConsumeItem(consumeItems, consumeCounts, consumeOverlap);
      const float addDelay = m_EP.GetEff_Plus(12);
      _ATTACK_DELAY_CHECKER::SetDelay(
        &m_AttDelayChker,
        static_cast<unsigned int>(classSkill->m_fActDelay + addDelay));
    }
  }

  this->SendMsg_CreateTrapResult(static_cast<char>(byErrCode), dwTrapObjSerial);
}

void CPlayer::pc_BackTrapRequest(unsigned int dwTrapObjSerial, unsigned __int16 wAddSerial)
{
  unsigned __int8 byErrCode = 2;
  CTrap *trap = nullptr;
  int trapSlot = -1;

  if (m_pmTrp.m_nCount > 0)
  {
    for (int index = 0; index < 20; ++index)
    {
      if (!m_pmTrp.m_Item[index].isLoad() || m_pmTrp.m_Item[index].dwSerial != dwTrapObjSerial)
      {
        continue;
      }

      trapSlot = index;
      trap = m_pmTrp.m_Item[index].pItem;
      if (!trap)
      {
        byErrCode = 2;
      }
      else if (trap->m_dwMasterSerial != m_dwObjSerial)
      {
        byErrCode = 21;
      }
      else if (GetSqrt(m_fCurPos, trap->m_fCurPos) > 150.0)
      {
        byErrCode = 22;
      }
      else
      {
        byErrCode = 0;
      }
      break;
    }

    if (byErrCode == 2)
    {
      g_Main.m_logSystemError.Write(
        "CPlayer::pc_BackTrapRequest() : Can't find trap (Player:%s Count:%d)",
        m_Param.GetCharNameA(),
        m_pmTrp.m_nCount);
    }
  }
  else
  {
    if (m_pmTrp.m_nCount < 0)
    {
      g_Main.m_logSystemError.Write(
        "CPlayer::pc_BackTrapRequest() : m_pmTrp.m_nCount ZERO and less ( %d )",
        m_pmTrp.m_nCount);
    }
    byErrCode = 2;
  }

  if (!byErrCode && trap)
  {
    const unsigned __int16 trapIndex = static_cast<unsigned __int16>(trap->m_pRecordSet->m_dwIndex);
    bool added = false;

    if (IsOverLapItem(26) && wAddSerial != 0xFFFF)
    {
      _STORAGE_LIST::_db_con *addTarget = m_Param.m_dbInven.GetPtrFromSerial(wAddSerial);
      if (addTarget && addTarget->m_byTableCode == 26 && addTarget->m_wItemIndex == trapIndex)
      {
        const unsigned __int64 dur = Emb_AlterDurPoint(0u, addTarget->m_byStorageIndex, 1, true, true);
        SendMsg_AdjustAmountInform(0u, addTarget->m_wSerial, static_cast<unsigned int>(dur));
        added = true;
      }
    }

    if (!added)
    {
      _STORAGE_LIST::_storage_con item{};
      item.m_bLoad = true;
      item.m_byTableCode = 26;
      item.m_wItemIndex = trapIndex;
      item.m_dwDur = 1;
      item.m_dwLv = 0;
      item.m_wSerial = m_Param.GetNewItemSerial();
      _STORAGE_LIST::_db_con *newItem = Emb_AddStorage(0u, &item, true, true);
      if (!newItem)
      {
        byErrCode = 2;
      }
      else
      {
        SendMsg_RewardAddItem(newItem, 0);
        added = true;
      }
    }

    if (!byErrCode && added)
    {
      trap->Destroy();
      if (trapSlot >= 0)
      {
        m_pmTrp.m_Item[trapSlot].init();
        --m_pmTrp.m_nCount;
      }
      SendMsg_MadeTrapNumInform(static_cast<char>(m_pmTrp.m_nCount));
    }
  }

  this->SendMsg_BackTrapResult(static_cast<char>(byErrCode));
}


namespace
{

bool IsXmasSnowEffect(const _sf_continous *sfCont)
{
  return sfCont
      && CPlayer::ms_pXmas_Snow_Effect
      && sfCont->m_byEffectCode == 3
      && sfCont->m_wEffectIndex == CPlayer::ms_pXmas_Snow_Effect->m_dwIndex;
}

unsigned __int16 GetTargetMonsterContInfoSize(_target_monster_contsf_allinform_zocl *info)
{
  if (info->byContCount > 8u)
  {
    info->byContCount = 0;
  }
  return static_cast<unsigned __int16>(21 - 2 * (8 - info->byContCount));
}

bool IsSameTargetMonsterContInfo(
  const _target_monster_contsf_allinform_zocl &left,
  const _target_monster_contsf_allinform_zocl &right)
{
  if (left.dwSerial != right.dwSerial)
  {
    return false;
  }

  for (int index = 0; index < 8; ++index)
  {
    if (left.m_MonContSf[index].wSfcode != right.m_MonContSf[index].wSfcode)
    {
      return false;
    }
  }
  return true;
}

unsigned __int16 GetTargetPlayerDamageContInfoSize(_target_player_damage_contsf_allinform_zocl *info)
{
  if (info->byContCount > 8u)
  {
    info->byContCount = 0;
  }
  return static_cast<unsigned __int16>(29 - 3 * (8 - info->byContCount));
}

bool IsSameTargetPlayerDamageContInfo(
  const _target_player_damage_contsf_allinform_zocl &left,
  const _target_player_damage_contsf_allinform_zocl &right)
{
  if (left.dwSerial != right.dwSerial)
  {
    return false;
  }

  for (int index = 0; index < 8; ++index)
  {
    if (left.m_PlayerContSf[index].wSfcode != right.m_PlayerContSf[index].wSfcode ||
        left.m_PlayerContSf[index].byContCount != right.m_PlayerContSf[index].byContCount)
    {
      return false;
    }
  }
  return true;
}

} // namespace

void CPlayer::SFContInsertMessage(
  unsigned __int8 byContCode,
  unsigned __int8 byListIndex,
  bool bAuraSkill,
  CPlayer *pPlayerAct)
{
  _sf_continous *sfCont = bAuraSkill ? &m_SFContAura[byContCode][byListIndex] : &m_SFCont[byContCode][byListIndex];

  EquipItemSFAgent.StartContSF(sfCont);
  if (IsXmasSnowEffect(sfCont))
  {
    m_bSnowMan = true;
  }

  if (pPlayerAct)
  {
    const char *playerName = pPlayerAct->m_Param.GetCharNameW();
    const unsigned int playerSerial = pPlayerAct->m_Param.GetCharSerial();
    const unsigned __int16 effectBit = static_cast<unsigned __int16>(CalcEffectBit(sfCont->m_byEffectCode, sfCont->m_wEffectIndex));
    SendMsg_AddEffect(effectBit, sfCont->m_byLv, sfCont->m_wDurSec, playerSerial, const_cast<char *>(playerName));
  }
  else
  {
    const unsigned __int16 effectBit = static_cast<unsigned __int16>(CalcEffectBit(sfCont->m_byEffectCode, sfCont->m_wEffectIndex));
    SendMsg_AddEffect(effectBit, sfCont->m_byLv, sfCont->m_wDurSec, 0u, nullptr);
  }

  if (m_pUserDB && !bAuraSkill)
  {
    m_pUserDB->Update_SFContInsert(
      byContCode,
      byListIndex,
      sfCont->m_byEffectCode,
      sfCont->m_wEffectIndex,
      static_cast<unsigned __int8>(sfCont->m_byLv - 1),
      sfCont->m_wDurSec);
  }
}

void CPlayer::SFContDelMessage(unsigned __int8 byContCode, unsigned __int8 byListIndex, bool bSend, bool bAura)
{
  if (!m_bOper)
  {
    return;
  }

  if (bSend)
  {
    _sf_continous *sfCont = bAura ? &m_SFContAura[byContCode][byListIndex] : &m_SFCont[byContCode][byListIndex];

    SendMsg_DelEffect(sfCont->m_byEffectCode, sfCont->m_wEffectIndex, sfCont->m_byLv);
    EquipItemSFAgent.EndContSF(sfCont);
    if (IsXmasSnowEffect(sfCont))
    {
      m_bSnowMan = false;
    }

    _base_fld *afterEffect = g_Main.m_tblEffectData[3].GetRecord("17");
    if (afterEffect && sfCont->m_byEffectCode == 3 && sfCont->m_wEffectIndex == afterEffect->m_dwIndex)
    {
      m_bAfterEffect = false;
    }
  }

  if (m_pUserDB && !bAura)
  {
    m_pUserDB->Update_SFContDelete(byContCode, byListIndex);
  }
}

void CPlayer::SFContUpdateTimeMessage(unsigned __int8 byContCode, unsigned __int8 byListIndex, int nLeftTime)
{
  if (m_pUserDB && nLeftTime > 0)
  {
    m_pUserDB->Update_SFContUpdate(byContCode, byListIndex, static_cast<unsigned __int16>(nLeftTime));
  }
}

void CPlayer::SendMsg_AddEffect(
  unsigned __int16 wEffectCode,
  unsigned __int8 byLv,
  unsigned __int16 wDurSec,
  unsigned int dwPlayerSerial,
  char *wszPlayerName)
{
  char payload[0x1A]{};
  payload[0] = static_cast<char>(byLv);
  *reinterpret_cast<unsigned __int16 *>(payload + 1) = wEffectCode;
  *reinterpret_cast<unsigned __int16 *>(payload + 3) = wDurSec;
  *reinterpret_cast<unsigned int *>(payload + 5) = dwPlayerSerial;
  if (dwPlayerSerial != 0)
  {
    strcpy_s(payload + 9, 0x11u, wszPlayerName);
  }

  unsigned __int8 type[2] = {17, 10};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 0x1Au);
  this->SendData_PartyMemberEffect(0, wEffectCode, byLv);
}

void CPlayer::SendMsg_DelEffect(unsigned __int8 byEffectCode, unsigned __int16 wEffectIndex, unsigned __int8 byLv)
{
  const unsigned __int16 effectBit =
    static_cast<unsigned __int16>(this->CalcEffectBit(byEffectCode, wEffectIndex));

  char payload[6]{};
  *reinterpret_cast<unsigned __int16 *>(payload) = effectBit;
  *reinterpret_cast<unsigned int *>(payload + 2) = this->m_dwObjSerial;

  unsigned __int8 type[2] = {17, 11};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 6u);
  this->SendData_PartyMemberEffect(1, effectBit, byLv);
}

void CPlayer::SendTargetMonsterSFContInfo()
{
  CGameObject *targetObject = this->GetTargetObj();
  if (!targetObject || targetObject->m_ObjID.m_byKind != 0 || targetObject->m_ObjID.m_byID != 1)
  {
    return;
  }

  CMonster *targetMonster = reinterpret_cast<CMonster *>(targetObject);
  _target_monster_contsf_allinform_zocl currentInfo{};
  currentInfo.Init();
  currentInfo.dwSerial = targetMonster->m_dwObjSerial;

  unsigned __int8 contCount = 0;
  for (int groupIndex = 0; groupIndex < 2; ++groupIndex)
  {
    for (int effectIndex = 0; effectIndex < targetMonster->GetMaxDMGSFContCount(); ++effectIndex)
    {
      _sf_continous *cont = &targetMonster->m_SFCont[groupIndex][effectIndex];
      if (!cont->m_bExist)
      {
        continue;
      }

      if (contCount < targetMonster->GetMaxDMGSFContCount())
      {
        currentInfo.m_MonContSf[contCount].wSfcode =
          static_cast<unsigned __int16>(this->CalcEffectBit(cont->m_byEffectCode, cont->m_wEffectIndex));
        ++contCount;
        currentInfo.byContCount = contCount;
      }
    }
  }

  if (IsSameTargetMonsterContInfo(currentInfo, this->m_TargetObject.m_PrevTargetMonsterContInfo))
  {
    return;
  }

  unsigned __int8 type[2] = {13, 100};
  const unsigned __int16 length = GetTargetMonsterContInfoSize(&currentInfo);
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&currentInfo),
    length);

  memcpy_0(
    &this->m_TargetObject.m_PrevTargetMonsterContInfo,
    &currentInfo,
    sizeof(this->m_TargetObject.m_PrevTargetMonsterContInfo));
}

void CPlayer::SendTargetPlayerDamageContInfo()
{
  CGameObject *targetObject = this->GetTargetObj();
  if (!targetObject || targetObject->m_ObjID.m_byKind != 0 || targetObject->m_ObjID.m_byID != 0)
  {
    return;
  }

  _target_player_damage_contsf_allinform_zocl currentInfo{};
  currentInfo.Init();
  currentInfo.dwSerial = targetObject->m_dwObjSerial;

  unsigned __int8 contCount = 0;
  for (int index = 0; index < 8; ++index)
  {
    char *contData = reinterpret_cast<char *>(&targetObject[1].m_fAbsPos[12 * index + 2]);
    if (!contData[0])
    {
      continue;
    }

    currentInfo.m_PlayerContSf[contCount].wSfcode = static_cast<unsigned __int16>(this->CalcEffectBit(
      static_cast<unsigned __int8>(contData[1]),
      *reinterpret_cast<unsigned __int16 *>(contData + 2)));
    currentInfo.m_PlayerContSf[contCount].byContCount = static_cast<unsigned __int8>(contData[20]);
    ++contCount;
    currentInfo.byContCount = contCount;
  }

  if (IsSameTargetPlayerDamageContInfo(currentInfo, this->m_TargetObject.m_PrevTargetPlayerDamageContInfo))
  {
    return;
  }

  unsigned __int8 type[2] = {13, 112};
  const unsigned __int16 length = GetTargetPlayerDamageContInfoSize(&currentInfo);
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&currentInfo),
    length);

  memcpy_0(
    &this->m_TargetObject.m_PrevTargetPlayerDamageContInfo,
    &currentInfo,
    sizeof(this->m_TargetObject.m_PrevTargetPlayerDamageContInfo));
}

void CPlayer::SendMsg_SFDelayRequest()
{
  const DWORD now = timeGetTime();
  _sf_delay_download_result_zocl msg{};

  int effCount = 0;
  int masCount = 0;
  for (int index = 0; index < 10; ++index)
  {
    if (this->m_AttDelayChker.EFF[index].byEffectCode != 255)
    {
      msg.EFF[effCount].byEffectCode = this->m_AttDelayChker.EFF[index].byEffectCode;
      msg.EFF[effCount].wEffectIndex = this->m_AttDelayChker.EFF[index].wEffectIndex;
      msg.EFF[effCount].dwRemainTime = this->m_AttDelayChker.EFF[index].dwNextTime - now;
      ++effCount;
    }

    if (this->m_AttDelayChker.MAS[index].byEffectCode != 255)
    {
      msg.MAS[masCount].byEffectCode = this->m_AttDelayChker.MAS[index].byEffectCode;
      msg.MAS[masCount].byMastery = this->m_AttDelayChker.MAS[index].byMastery;
      msg.MAS[masCount].dwRemainTime = this->m_AttDelayChker.MAS[index].dwNextTime - now;
      ++masCount;
    }
  }

  unsigned __int8 type[2] = {3, 56};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 0x82u);
}

void CPlayer::SendMsg_PotionDelayTime(unsigned int *pdwPotionNextUseTime, unsigned int dwCurTime)
{
  _potion_delay_time_information_zocl msg{};

  msg.nMaxNum = 38;
  for (int index = 0; index < 38; ++index)
  {
    const int remain = static_cast<int>(pdwPotionNextUseTime[index] - dwCurTime);
    msg.dwPotionDelayTime[index] = (remain <= 0) ? 0u : static_cast<unsigned int>(remain);
  }

  unsigned __int8 type[2] = {3, 60};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 0x9Cu);
}

void CPlayer::_set_db_sf_effect(_SFCONT_DB_BASE *pDBBase)
{
  if (!this->m_pUserDB)
  {
    return;
  }

  const unsigned int curTime = _sf_continous::GetSFContCurTime();
  for (int group = 0; group < 2; ++group)
  {
    for (int index = 0; index < 8; ++index)
    {
      _SFCONT_DB_BASE::_LIST *dbEntry = &pDBBase->m_List[group][index];
      if (!dbEntry->IsFilled())
      {
        continue;
      }

      _sf_continous *cont = &this->m_SFCont[group][index];
      const unsigned __int8 effectCode = static_cast<unsigned __int8>(dbEntry->GetEffectCode());
      const unsigned __int16 effectIndex = static_cast<unsigned __int16>(dbEntry->GetEffectIndex());
      const unsigned __int8 effectLevel = static_cast<unsigned __int8>(dbEntry->GetLv() + 1);
      unsigned int startSec = curTime;
      unsigned __int16 durSec = static_cast<unsigned __int16>(dbEntry->GetLeftTime());
      const unsigned __int8 order = static_cast<unsigned __int8>(dbEntry->GetOrder());

      if (curTime > order)
      {
        startSec = curTime - order;
        durSec = static_cast<unsigned __int16>(durSec + order);
      }

      bool invalid = false;
      if (effectCode < 4u)
      {
        if (g_Main.m_tblEffectData[effectCode].GetRecord(effectIndex))
        {
          if (effectLevel > 7u)
          {
            invalid = true;
          }
        }
        else
        {
          invalid = true;
        }
      }
      else
      {
        invalid = true;
      }

      if (invalid)
      {
        this->m_pUserDB->Update_SFContDelete(group, index);
        const char *charName = this->m_Param.GetCharNameA();
        g_Main.m_logSystemError.Write(
          "%s: error stored effect, code: %d, idx: %d: lv: %d",
          charName,
          effectCode,
          effectIndex,
          effectLevel);
        continue;
      }

      this->_set_sf_cont(cont, effectCode, effectIndex, effectLevel, startSec, durSec, 0);
      this->EquipItemSFAgent.StartContSF(cont);

      _base_fld *record = g_Main.m_tblEffectData[3].GetRecord("17");
      if (record && cont->m_byEffectCode == 3 && cont->m_wEffectIndex == record->m_dwIndex)
      {
        this->m_bAfterEffect = 1;
      }

      SendMsg_StartContSF(cont);
      this->m_bLastContEffectUpdate = 1;
    }
  }
}

void CPlayer::SendMsg_Circle_DelEffect(
  unsigned __int8 byEffectCode,
  unsigned __int16 wEffectIndex,
  unsigned __int8 byLv,
  bool bToOne)
{

  _effect_remove_inform_zocl msg{};
  msg.wEffectCode = static_cast<unsigned __int16>(this->CalcEffectBit(byEffectCode, wEffectIndex));
  msg.dwPlayerSerial = this->m_dwObjSerial;

  unsigned __int8 type[2]{17, 11};
  this->CircleReport(type, reinterpret_cast<char *>(&msg), sizeof(msg), bToOne);
}

void CPlayer::SendMsg_Die()
{
  char payload[6]{};
  *reinterpret_cast<unsigned __int16 *>(payload) = this->m_ObjID.m_wIndex;
  *reinterpret_cast<unsigned int *>(payload + 2) = this->m_dwObjSerial;

  unsigned __int8 type[2] = {3, 23};
  this->CircleReport(type, payload, 6, true);
}

void CPlayer::SendMsg_BreakdownEquipItem(unsigned __int8 byPartIndex, unsigned __int16 wSerial)
{
  char payload[0x0B]{};
  *reinterpret_cast<unsigned __int16 *>(payload) = this->m_ObjID.m_wIndex;
  *reinterpret_cast<unsigned int *>(payload + 2) = this->m_dwObjSerial;
  *reinterpret_cast<unsigned __int16 *>(payload + 6) = this->GetVisualVer();
  payload[8] = static_cast<char>(byPartIndex);
  *reinterpret_cast<unsigned __int16 *>(payload + 9) = wSerial;

  unsigned __int8 type[2] = {5, 19};
  this->CircleReport(type, payload, 11, true);
}

void CPlayer::SendMsg_Notify_Gravity_Stone_Owner_Die()
{
  char payload[5]{};
  payload[0] = static_cast<char>(this->m_ObjID.m_byID);
  *reinterpret_cast<unsigned int *>(payload + 1) = this->m_dwObjSerial;

  unsigned __int8 type[2] = {27, 86};
  this->CircleReport(type, payload, 5, true);
}

void CPlayer::SendMsg_FixPosition(int n)
{

  if (m_bObserver && !g_Player[n].m_byUserDgr)
  {
    return;
  }

  _player_fixpositon_zocl msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  msg.wEquipVer = static_cast<unsigned __int16>(GetVisualVer());
  msg.byRaceCode = m_Param.GetRaceSexCode();
  FloatToShort(m_fCurPos, msg.zCur, 3);
  msg.wLastEffectCode = static_cast<unsigned __int16>(m_wLastContEffect);
  msg.dwStateFlag = GetStateFlag();
  msg.byColor = m_byGuildBattleColorInx;

  unsigned __int8 type[2] = {4, 9};
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_RealMovePoint(int n)
{

  if (m_bObserver && !g_Player[n].m_byUserDgr)
  {
    return;
  }

  _player_real_move_zocl msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  msg.dwEquipVer = static_cast<unsigned __int16>(GetVisualVer());
  msg.byRaceCode = m_Param.GetRaceSexCode();
  FloatToShort(m_fCurPos, msg.zCur, 3);
  msg.zTar[0] = static_cast<__int16>(static_cast<int>(m_fTarPos[0]));
  msg.zTar[1] = static_cast<__int16>(static_cast<int>(m_fTarPos[2]));
  msg.wLastEffectCode = static_cast<unsigned __int16>(m_wLastContEffect);
  msg.dwStateFlag = GetStateFlag();
  msg.nAddSpeed = static_cast<__int16>(static_cast<int>(GetAddSpeed()));
  msg.byDirect = m_byMoveDirect;
  msg.byColor = m_byGuildBattleColorInx;

  unsigned __int8 type[2] = {4, 21};
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

__int64 CPlayer::GetAvoidRate()
{
  if (m_fTalik_AvoidPoint <= 0.0f)
  {
    return static_cast<unsigned int>(static_cast<int>(m_EP.GetEff_Plus(3)));
  }

  const int maxDp = GetMaxDP();
  float dpRate = 0.0f;
  if (maxDp > 0)
  {
    dpRate = (2.0f * static_cast<float>(GetDP()) / static_cast<float>(maxDp)) - 0.4f;
    if (dpRate < 0.0f)
    {
      dpRate = 0.0f;
    }
    else if (dpRate > 1.0f)
    {
      dpRate = 1.0f;
    }
  }

  const int talikPenalty = static_cast<int>(m_fTalik_AvoidPoint * (1.0f - dpRate));
  const float avoidValue = m_EP.GetEff_Plus(3) - static_cast<float>(talikPenalty);
  return static_cast<unsigned int>(static_cast<int>(avoidValue));
}

__int64 CPlayer::GetDefFC(int nAttactPart, CCharacter *pAttChar, int *pnConvertPart)
{
  m_nLastBeatenPart = nAttactPart;

  float defenseValue = 0.0f;
  if (IsRidingUnit())
  {
    defenseValue = static_cast<float>(m_nUnitDefFc) * m_fUnitPv_DefFc;
  }
  else
  {
    bool ignoreShieldByEffect = false;
    if (pAttChar && pAttChar->m_ObjID.m_byID == 0)
    {
      ignoreShieldByEffect = pAttChar->m_EP.GetEff_State(11);
      if (!ignoreShieldByEffect)
      {
        float ignoreProb = pAttChar->m_EP.GetEff_Plus(28);
        if (ignoreProb > 0.0f)
        {
          ignoreProb += pAttChar->m_EP.GetEff_Plus(41);
          if ((rand() % 100) <= static_cast<int>(ignoreProb))
          {
            ignoreShieldByEffect = true;
          }
        }
      }
      if (ignoreShieldByEffect)
      {
        pAttChar->SendMsg_AttackActEffect(1u, this);
      }
    }

    _STORAGE_LIST::_db_con *shieldItem = &m_Param.m_dbEquip.m_pStorageList[5];
    bool shieldEnabled = false;
    if (shieldItem->m_bLoad)
    {
      shieldEnabled = GetEffectEquipCode(1u, 5u) == 1;
      if (shieldEnabled)
      {
        _STORAGE_LIST::_db_con *weaponItem = &m_Param.m_dbEquip.m_pStorageList[6];
        if (weaponItem->m_bLoad)
        {
          _base_fld *weaponRecord = g_Main.m_tblItemData[6].GetRecord(weaponItem->m_wItemIndex);
          if (weaponRecord && *reinterpret_cast<int *>(&weaponRecord[4].m_strCode[12]) == 100)
          {
            shieldEnabled = false;
          }
        }
      }
    }

    if (pAttChar && shieldEnabled && !ignoreShieldByEffect)
    {
      const int masteryValue = m_pmMst.GetMasteryPerMast(2u, 0u);
      int blockChance = static_cast<int>((static_cast<float>(masteryValue) / 99.0f) * 20.0f + 5.0f);
      blockChance += static_cast<int>(m_EP.GetEff_Plus(29));
      if (blockChance > 100)
      {
        blockChance = 100;
      }
      if ((rand() % 100) < blockChance)
      {
        m_nLastBeatenPart = 5;
        if (pnConvertPart)
        {
          *pnConvertPart = m_nLastBeatenPart;
        }
        return 0xFFFFFFFEu;
      }
    }

    if (m_nLastBeatenPart < 0 || m_nLastBeatenPart >= 5)
    {
      m_nLastBeatenPart = rand() % 5;
    }

    const int partIndex = m_nLastBeatenPart;
    int partCode = m_Param.m_dbChar.m_byDftPart[partIndex];
    _STORAGE_LIST::_db_con *partItem = &m_Param.m_dbEquip.m_pStorageList[partIndex];
    if (partItem->m_bLoad && GetEffectEquipCode(1u, static_cast<unsigned __int8>(partIndex)) == 1)
    {
      partCode = partItem->m_wItemIndex;
    }

    _base_fld *partRecord = g_Main.m_tblItemData[partIndex].GetRecord(partCode);
    if (partRecord)
    {
      defenseValue = *reinterpret_cast<float *>(&partRecord[5].m_strCode[44]);
    }

    if (shieldEnabled && m_nLastBeatenPart == 5)
    {
      _base_fld *shieldRecord = g_Main.m_tblItemData[5].GetRecord(shieldItem->m_wItemIndex);
      if (shieldRecord)
      {
        defenseValue = *reinterpret_cast<float *>(&shieldRecord[5].m_strCode[44]);
      }
    }

    if (pnConvertPart)
    {
      *pnConvertPart = m_nLastBeatenPart;
    }

    defenseValue *= m_EP.GetEff_Rate(33);
    if (IsSiegeMode())
    {
      defenseValue *= m_EP.GetEff_Rate(24);
    }
  }

  if (!m_bInGuildBattle)
  {
    const unsigned int charSerial = m_Param.GetCharSerial();
    const int raceCode = m_Param.GetRaceCode();
    const unsigned __int8 bossType =
      CPvpUserAndGuildRankingSystem::Instance()->GetBossType(raceCode, charSerial);
    switch (bossType)
    {
    case 0:
      defenseValue *= 1.3f;
      break;
    case 1:
    case 5:
      defenseValue *= 1.5f;
      break;
    case 3:
    case 7:
      defenseValue *= 1.2f;
      break;
    default:
      break;
    }
  }

  if (g_HolySys.GetDestroyerSerial() == m_dwObjSerial || IsLastAttBuff())
  {
    defenseValue *= 1.3f;
  }

  if (!IsRidingUnit())
  {
    const int maxDp = GetMaxDP();
    float dpRate = 0.0f;
    if (maxDp > 0)
    {
      dpRate = (2.0f * static_cast<float>(GetDP()) / static_cast<float>(maxDp)) - 0.4f;
      if (dpRate < 0.0f)
      {
        dpRate = 0.0f;
      }
      else if (dpRate > 1.0f)
      {
        dpRate = 1.0f;
      }
    }

    if (m_fTalik_DefencePoint > 0.0f)
    {
      const float talikPenalty = m_fTalik_DefencePoint * (1.0f - dpRate);
      defenseValue *= (m_EP.GetEff_Rate(6) - talikPenalty);
    }
    else
    {
      defenseValue *= m_EP.GetEff_Rate(6);
    }
  }

  return static_cast<unsigned int>(static_cast<int>(defenseValue));
}

float CPlayer::GetDefFacing(int nPart)
{
  if (IsRidingUnit())
  {
    if (m_pUsingUnit)
    {
      _base_fld *record = g_Main.m_tblUnitFrame.GetRecord(m_pUsingUnit->byFrame);
      if (record)
      {
        return *reinterpret_cast<float *>(&record[1].m_strCode[8]);
      }
    }
    return FLOAT_0_5;
  }

  if (nPart >= 8)
  {
    return FLOAT_0_5;
  }

  int partCode = m_Param.m_dbChar.m_byDftPart[nPart];
  _STORAGE_LIST::_db_con *equipPart = &m_Param.m_dbEquip.m_pStorageList[nPart];
  if (equipPart->m_bLoad && GetEffectEquipCode(1u, static_cast<unsigned __int8>(nPart)) == 1)
  {
    partCode = equipPart->m_wItemIndex;
  }

  _base_fld *record = g_Main.m_tblItemData[nPart].GetRecord(partCode);
  if (!record)
  {
    return FLOAT_0_5;
  }
  return *reinterpret_cast<float *>(&record[5].m_strCode[60]);
}

float CPlayer::GetDefGap(int nPart)
{
  if (IsRidingUnit())
  {
    if (m_pUsingUnit)
    {
      _base_fld *record = g_Main.m_tblUnitFrame.GetRecord(m_pUsingUnit->byFrame);
      if (record)
      {
        return *reinterpret_cast<float *>(&record[1].m_strCode[4]);
      }
    }
    return FLOAT_0_5;
  }

  if (nPart >= 8)
  {
    return FLOAT_0_5;
  }

  int partCode = m_Param.m_dbChar.m_byDftPart[nPart];
  _STORAGE_LIST::_db_con *equipPart = &m_Param.m_dbEquip.m_pStorageList[nPart];
  if (equipPart->m_bLoad && GetEffectEquipCode(1u, static_cast<unsigned __int8>(nPart)) == 1)
  {
    partCode = equipPart->m_wItemIndex;
  }

  _base_fld *record = g_Main.m_tblItemData[nPart].GetRecord(partCode);
  if (!record)
  {
    return FLOAT_0_5;
  }
  return *reinterpret_cast<float *>(&record[5].m_strCode[56]);
}

__int64 CPlayer::GetDefSkill(bool bBackAttackDamage)
{
if (!IsRidingUnit())
  {
    return m_pmMst.GetMasteryPerMast(1u, 0u);
  }

  int totalDefSkill = 0;
  _base_fld *leftPart = g_Main.m_tblUnitPart[0].GetRecord(m_pUsingUnit->byPart[0]);
  if (leftPart)
  {
    totalDefSkill += *reinterpret_cast<int *>(&leftPart[5].m_strCode[12]);
  }

  _base_fld *rightPart = g_Main.m_tblUnitPart[1].GetRecord(m_pUsingUnit->byPart[1]);
  if (rightPart)
  {
    totalDefSkill += *reinterpret_cast<int *>(&rightPart[5].m_strCode[12]);
  }

  return static_cast<unsigned int>(totalDefSkill);
}

__int64 CPlayer::GetFireTol()
{
  const float total = static_cast<float>(m_nTolValue[0]) + m_EP.GetEff_Plus(15);
  int value = static_cast<int>(total * m_EP.GetEff_Rate(25));
  value = ClampToleranceValue(value);
  if (m_EP.GetEff_State(19) && value > 0)
  {
    value = -value;
  }
  return static_cast<unsigned int>(value);
}

char *CPlayer::GetObjName()
{
  std::snprintf(
    s_playerObjectName,
    sizeof(s_playerObjectName),
    "[PLAYER][%d] >> %s (pos: %s {%d, %d, %d})",
    static_cast<int>(GetObjRace()),
    m_Param.GetCharNameA(),
    m_pCurMap ? m_pCurMap->m_pMapSet->m_strCode : "<null-map>",
    static_cast<int>(m_fCurPos[0]),
    static_cast<int>(m_fCurPos[1]),
    static_cast<int>(m_fCurPos[2]));
  return s_playerObjectName;
}

__int64 CPlayer::GetObjRace()
{
  return static_cast<unsigned int>(m_Param.GetRaceCode());
}

__int64 CPlayer::GetSoilTol()
{
  const float total = static_cast<float>(m_nTolValue[2]) + m_EP.GetEff_Plus(17);
  int value = static_cast<int>(total * m_EP.GetEff_Rate(27));
  value = ClampToleranceValue(value);
  if (m_EP.GetEff_State(19) && value > 0)
  {
    value = -value;
  }
  return static_cast<unsigned int>(value);
}

__int64 CPlayer::GetWaterTol()
{
  const float total = static_cast<float>(m_nTolValue[1]) + m_EP.GetEff_Plus(16);
  int value = static_cast<int>(total * m_EP.GetEff_Rate(26));
  value = ClampToleranceValue(value);
  if (m_EP.GetEff_State(19) && value > 0)
  {
    value = -value;
  }
  return static_cast<unsigned int>(value);
}

float CPlayer::GetWeaponAdjust()
{
  if (IsRidingUnit())
  {
    return FLOAT_1_0;
  }

  _STORAGE_LIST::_db_con *weaponItem = &m_Param.m_dbEquip.m_pStorageList[6];
  if (!weaponItem->m_bLoad || GetEffectEquipCode(1u, 6u) != 1)
  {
    return 0.0f;
  }

  _base_fld *weaponRecord = g_Main.m_tblItemData[6].GetRecord(weaponItem->m_wItemIndex);
  if (!weaponRecord)
  {
    return 0.0f;
  }
  if (*reinterpret_cast<int *>(&weaponRecord[6].m_strCode[8]) == 10)
  {
    return 0.0f;
  }

  return *reinterpret_cast<float *>(&weaponRecord[9].m_strCode[40]);
}

__int64 CPlayer::GetWeaponClass()
{
  return m_pmWpn.byWpClass;
}

float CPlayer::GetWidth()
{
  return *reinterpret_cast<float *>(&m_pRecordSet[2].m_strCode[12]);
}

__int64 CPlayer::GetWindTol()
{
  const float total = static_cast<float>(m_nTolValue[3]) + m_EP.GetEff_Plus(18);
  int value = static_cast<int>(total * m_EP.GetEff_Rate(28));
  value = ClampToleranceValue(value);
  if (m_EP.GetEff_State(19) && value > 0)
  {
    value = -value;
  }
  return static_cast<unsigned int>(value);
}

char CPlayer::IsBeDamagedAble(CCharacter *pAtter)
{
  if (!pAtter)
  {
    return 0;
  }

  if (pAtter->m_ObjID.m_byID != 0)
  {
    return 1;
  }

  CPlayer *attacker = static_cast<CPlayer *>(pAtter);
  if (!attacker->m_bInGuildBattle && m_bInGuildBattle)
  {
    return 0;
  }
  if (attacker->m_bInGuildBattle && !m_bInGuildBattle)
  {
    return 0;
  }
  if (!attacker->m_bInGuildBattle && !m_bInGuildBattle)
  {
    return 1;
  }

  return attacker->m_byGuildBattleColorInx != m_byGuildBattleColorInx;
}

char CPlayer::IsRecvableContEffect()
{
  if (IsRidingUnit())
  {
    return 0;
  }
  if (m_EP.GetEff_State(20))
  {
    return 0;
  }
  return m_EP.GetEff_State(28) ? 0 : 1;
}

bool CPlayer::Is_Battle_Mode()
{
  return m_byBattleMode != 0;
}

void CPlayer::OutOfSec()
{
  if (m_pUserDB)
  {
    char buffer[144]{};
    std::snprintf(
      buffer,
      sizeof(buffer),
      "CLOSE>> OutOfSec() ID: %s, NM: %s",
      m_pUserDB->m_szAccountID,
      m_Param.GetCharNameA());
    g_Network.Close(0, m_ObjID.m_wIndex, false, buffer);
  }
  else
  {
    g_Network.Close(0, m_ObjID.m_wIndex, false, nullptr);
  }

  m_bOutOfMap = true;
}

void CPlayer::RecvKillMessage(CCharacter *pDier)
{
  if (!pDier)
  {
    return;
  }

  if (pDier->m_ObjID.m_byID)
  {
    if (pDier->m_ObjID.m_byID != 1)
    {
      return;
    }

    CMonster *deadMonster = static_cast<CMonster *>(pDier);
    m_QuestMgr.CheckFailLoop(3, deadMonster->m_pRecordSet->m_strCode);

    if (m_pUserDB && CActionPointSystemMgr::Instance()->GetEventStatus(1u) == 2)
    {
      const int killerLevel = static_cast<int>(m_Param.GetLevel());
      const int deadLevel = static_cast<int>(deadMonster->GetLevel());
      if (std::abs(killerLevel - deadLevel) < 10)
      {
        const unsigned int currentPoint = m_pUserDB->GetActPoint(1u);
        unsigned int addPoint = 0;
        if (deadMonster->m_pMonRec && deadMonster->m_pMonRec->m_nKillPoint > 0)
        {
          addPoint = static_cast<unsigned int>(deadMonster->m_pMonRec->m_nKillPoint);
        }
        const unsigned int nextPoint = currentPoint + addPoint;
        m_pUserDB->Update_User_Action_Point(1u, nextPoint);
        SendMsg_Alter_Action_Point(1u, nextPoint);
      }
    }

    bool handledByDarkHole = false;
    if (m_pDHChannel && deadMonster->m_pMonRec)
    {
      handledByDarkHole =
        m_pDHChannel->CheckEvent(dh_event_hunt, 255, static_cast<int>(deadMonster->m_pMonRec->m_dwIndex), 1, deadMonster)
        != 0;
    }

    if (!handledByDarkHole)
    {
      Emb_CheckActForQuest(3, deadMonster->m_pRecordSet->m_strCode, 1u, false);
      if (m_pPartyMgr->IsPartyMode())
      {
        Emb_CheckActForQuestParty(3, deadMonster->m_pRecordSet->m_strCode, 1u);
      }
    }
    return;
  }

  CPlayer *deadPlayer = static_cast<CPlayer *>(pDier);
  char *raceCode = cvt_string(deadPlayer->m_Param.GetRaceCode());
  if (!Emb_CreateQuestEvent(quest_happen_type_pk, raceCode))
  {
    Emb_CheckActForQuest(2, raceCode, 1u, false);
  }
}

void CPlayer::SendMsg_MaxPvpPointInform(int nMax)
{
  char payload[4]{};
  *reinterpret_cast<int *>(payload) = nMax;

  unsigned __int8 type[2] = {59, 15};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 4u);
}

void CPlayer::IncCriEffPvPCashBag(double dAlter)
{
  if (dAlter > 0.0 && this->m_byHSKQuestCode != 100)
  {
    CHolyStone *stone = &g_Stone[static_cast<int>(this->m_Param.GetRaceCode())];
    if (stone->m_bOper && stone->m_pCurMap == this->m_pCurMap)
    {
      this->m_nHSKPvpPoint = static_cast<int>(static_cast<double>(this->m_nHSKPvpPoint) + dAlter);
      SendMsg_HSKQuestActCum();
      this->m_pUserDB->m_AvatorData.m_iPvpPoint = this->m_nHSKPvpPoint;
    }
  }
}

void CPlayer::SendMsg_FcitemInform(unsigned __int16 wItemSerial, unsigned int dwNewStat)
{
  char szMsg[2]; // [rsp+34h] [rbp-44h] BYREF
  unsigned int v7; // [rsp+36h] [rbp-42h]
  unsigned __int8 pbyType[36]; // [rsp+54h] [rbp-24h] BYREF

  *(_WORD *)szMsg = wItemSerial;
  v7 = dwNewStat;
  pbyType[0] = 3;
  pbyType[1] = 44;
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, szMsg, 6u);
}

void CPlayer::AlterPvPCashBag(long double dAlter, int IOCode)
{
  double PvpCash; // [rsp+20h] [rbp-18h]
  long double dAltera; // [rsp+28h] [rbp-10h]

  PvpCash = this->m_kPvpOrderView.GetPvpCash();
  dAltera = PvpCash + dAlter;
  if ( PvpCash + dAlter > 999999.0 )
    dAltera = 999999.0;
  if ( dAltera < -999999.0 )
    dAltera = -999999.0;
  if ( this->GetLevel() > 39 && this->m_Param.m_pClassData->m_nGrade >= 1 )
  {
    this->m_kPvpOrderView.SetPvpCash(dAltera);
    if ( dAltera > PvpCash && dAlter > 0.0 )
      this->IncCriEffPvPCashBag(dAlter);
    const long double currentCash = this->m_kPvpOrderView.GetPvpCash();
    if ( PvpCash != currentCash )
    {
      this->m_kPvpOrderView.UpdatePvpCash(dAltera);
      this->SendMsg_AlterPvPCash(IOCode);
    }
  }
}

CPvpPointLimiter *CPlayer::GetPvpPointLimiter(CPvpPointLimiter *result)
{
  *result = m_kPvpPointLimiter;
  return result;
}

CPvpOrderView *CPlayer::GetPvpOrderView()
{
  return &m_kPvpOrderView;
}

