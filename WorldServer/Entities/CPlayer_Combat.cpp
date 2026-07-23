#include "pch.h"

#include "CPlayer.h"
#include "CQuestMgr.h"
#include "CRecordData.h"
#include "CMapOperation.h"
#include "CMapData.h"
#include "CBsp.h"
#include "CObjectList.h"
#include "CMonster.h"
#include "CTrap.h"
#include "pnt_rect.h"
#include "CMainThread.h"
#include "CItemStore.h"
#include "CItemBox.h"
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
#include "DfnEquipItem_fld.h"
#include "base_fld.h"
#include "CDarkHole.h"
#include "CDarkHoleChannel.h"
#include "CDarkHoleDungeonQuest.h"
#include "CDarkHoleDungeonQuestSetup.h"
#include "../Protection/ProtectionSystem.h"
#include "GuardTowerItem_fld.h"
#include "TicketItem_fld.h"
#include "UnitFrame_fld.h"
#include "UnitPart_fld.h"
#include "WeaponItem_fld.h"
#include "player_fld.h"
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

namespace
{
bool CanSendPlayerViewMessage(CPlayer *sourcePlayer, CPlayer *targetPlayer)
{
  if (sourcePlayer->m_bObserver && !targetPlayer->m_byUserDgr)
  {
    return false;
  }

  // Yorozuya fix implementation (non-IDA): hide stealth/invisible players unless the
  // viewer has the detect effect.
  if (sourcePlayer->GetStealth(true) && targetPlayer->m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_22) <= 0.0f)
  {
    return false;
  }

  return true;
}
}
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
    toSelf.dwItemCum = static_cast<unsigned int>(pForceItem->m_dwDur);
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
  std::memcpy(&toSelf.idDster, pidDst, sizeof(toSelf.idDster));
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
  std::memcpy(&toOther.idDster, pidDst, sizeof(toOther.idDster));
  toOther.byEffectedNum = toSelf.byEffectedNum;
  std::memcpy(toOther.list, toSelf.list, 8LL * toSelf.byEffectedNum);

  unsigned __int8 otherType[2] = {17, 101};
  this->CircleReport(otherType, reinterpret_cast<char *>(&toOther), sizeof(toOther), false);
}

void CPlayer::SendMsg_ThrowUnitResult(unsigned __int8 byErrCode, _CHRID *pidDst, unsigned __int16 wBulletIndex)
{
  _throw_unit_result_one_zocl toSelf{};
  toSelf.byErrCode = byErrCode;
  std::memcpy(&toSelf.idDster, pidDst, sizeof(toSelf.idDster));
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
  std::memcpy(&toOther.idDster, pidDst, sizeof(toOther.idDster));
  toOther.byEffectedNum = toSelf.byEffectedNum;
  std::memcpy(toOther.list, toSelf.list, 8LL * toSelf.byEffectedNum);

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
    const int raceCode = static_cast<int>(targetPlayer->m_Param.GetRaceCode());
    const unsigned __int8 bossType =
      CPvpUserAndGuildRankingSystem::Instance()->GetBossType(static_cast<unsigned __int8>(raceCode), targetSerial);
    if (bossType != 4 && bossType != 8)
    {
      AssistSkill(target, 0, auraSkill, 7, errorCode, &upMastery);
    }
  }
}

void CPlayer::UpdateAuraSFCont()
{
  const unsigned int sfContCurTime = _sf_continous::GetSFContCurTime();
  for (int contCode = 0; contCode < 2; ++contCode)
  {
    for (int slot = 0; slot < 8; ++slot)
    {
      _sf_continous *cont = &m_SFContAura[contCode][slot];
      if (cont->m_bExist)
      {
        const unsigned int elapsed = sfContCurTime - cont->m_dwStartSec;
        if (elapsed >= cont->m_wDurSec)
        {
          RemoveSFContEffect(static_cast<unsigned __int8>(contCode), static_cast<unsigned __int16>(slot), false, true);
        }
      }
    }
  }

  if (m_tmrAuraSkill.CountingTimer())
  {
    int hasAura = 0;
    for (int contCode = 0; contCode < 2; ++contCode)
    {
      for (int slot = 0; slot < 8; ++slot)
      {
        _sf_continous *cont = &m_SFContAura[contCode][slot];
        if (!cont->m_bExist)
        {
          continue;
        }

        _skill_fld *skillField = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[0].GetRecord(cont->m_wEffectIndex));
        if (!skillField || skillField->m_nClass != 4)
        {
          continue;
        }

        if (IsChaosMode() || IsPunished(1u, false))
        {
          RemoveSFContEffect(static_cast<unsigned __int8>(contCode), static_cast<unsigned __int16>(slot), false, false);
        }
        else
        {
          const int maxFp = GetMaxFP();
          if (maxFp >= skillField->m_nNeedFP && _pre_check_skill_gradelimit(skillField))
          {
            hasAura = 1;
            skill_process_for_aura(static_cast<int>(cont->m_wEffectIndex));
          }
          else
          {
            RemoveSFContEffect(static_cast<unsigned __int8>(contCode), static_cast<unsigned __int16>(slot), false, true);
          }
        }
      }
    }

    if (!hasAura)
    {
      m_tmrAuraSkill.StopTimer();
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

  // Yorozuya fix (non-IDA parity): block skill usage in siege mode.
  if (IsSiegeMode())
  {
    return 14;
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
  if (skillField->m_nContEffectType == 1 && m_EP.GetEff_State(EFF_STATE_UNKNOWN_2))
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
  if (!(nEffectCode == 2 && skillField->m_nTempEffectType == 36 && skillField->m_nEffectClass == 6))
  {
    // Yorozuya fix implementation (non-IDA): enforce skill cast radius for buff/debuff (with Y distance check).
    float availableDist = static_cast<float>(m_pmWpn.wGaAttRange);
    availableDist += static_cast<float>(skillField->m_nBonusDistance);
    availableDist += target->GetWidth() / 2.0f;
    if (m_pmWpn.byWpType == 7)
    {
      availableDist += m_EP.GetEff_Plus(EFF_PLUS_ATTACK_RANGE);
    }
    else
    {
      availableDist += m_EP.GetEff_Plus(m_pmWpn.byWpClass + 4);
    }
    availableDist += m_EP.GetEff_Plus(m_pmWpn.byWpClass + 6);

    float dist = GetSqrt(target->m_fCurPos, m_fCurPos);
    if (dist > availableDist)
    {
      dist = GetSqrt(target->m_fOldPos, m_fCurPos);
      if (dist > availableDist)
      {
        return static_cast<unsigned __int8>(-3);
      }
    }

    const float yDiffCur = std::fabs(target->m_fCurPos[1] - m_fCurPos[1]);
    const float yDiffOld = std::fabs(target->m_fOldPos[1] - m_fCurPos[1]);
    if (yDiffCur > 200.0f && yDiffOld > 200.0f)
    {
      return static_cast<unsigned __int8>(-3);
    }
  }

  // Yorozuya fix (non-IDA parity): extra skill delay tracking for skill_process.
  if (!m_bSFDelayNotCheck)
  {
    const DWORD now = GetTickCount();
    if (nEffectCode == 2)
    {
      EnsureClassSkillDelayStorage();
      if (m_pdwClassSkillAttackDelayEnd && m_dwClassSkillDelayCount > 0)
      {
        const unsigned int skillIndex = static_cast<unsigned int>(skillField->m_dwIndex);
        if (skillIndex < m_dwClassSkillDelayCount)
        {
          if (!IsAttackDelayReady(now, m_pdwClassSkillAttackDelayEnd[skillIndex]))
          {
            return 9;
          }
        }
      }
    }
    else
    {
      const int skillClass = static_cast<int>(skillField->m_nClass);
      const int skillLevel = static_cast<int>(skillField->m_nLv);
      if (skillClass >= 0 && skillClass < 4 && skillLevel >= 0 && skillLevel < 4)
      {
        if (!IsAttackDelayReady(now, m_dwSkillAttackDelayEnd[skillClass][skillLevel]))
        {
          return 9;
        }
      }
    }
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
  if (target->m_EP.GetEff_State(EFF_STATE_INSUPERABLE))
  {
    if (skillField->m_nTempEffectType == -1 || !IsUsableTempEffectAtStoneState(skillField->m_nTempEffectType))
    {
      return 24;
    }
  }
  if (target->m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
  {
    return 24;
  }
  if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_21))
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
      + m_EP.GetEff_Plus(EFF_PLUS_SKILL_LEVEL);
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
      m_tmrAuraSkill.BeginTimer(5000);
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
    const float addDelay = m_EP.GetEff_Plus(EFF_PLUS_SKILL_ACT_DELAY);
    _ATTACK_DELAY_CHECKER::SetDelay(&m_AttDelayChker, static_cast<unsigned int>(skillField->m_fActDelay + addDelay));

    // Yorozuya fix (non-IDA parity): extra skill delay tracking for skill_process.
    if (!m_bSFDelayNotCheck)
    {
      const DWORD now = GetTickCount();
      int delay = static_cast<int>(skillField->m_fActDelay + addDelay);
      if (delay < 0)
      {
        delay = 0;
      }
      const unsigned int delayMs = AdjustAttackDelayMs(static_cast<unsigned int>(delay));
      if (nEffectCode == 2)
      {
        if (m_pdwClassSkillAttackDelayEnd && m_dwClassSkillDelayCount > 0)
        {
          const unsigned int skillIndex = static_cast<unsigned int>(skillField->m_dwIndex);
          if (skillIndex < m_dwClassSkillDelayCount)
          {
            m_pdwClassSkillAttackDelayEnd[skillIndex] = now + delayMs;
          }
        }
      }
      else
      {
        const int skillClass = static_cast<int>(skillField->m_nClass);
        const int skillLevel = static_cast<int>(skillField->m_nLv);
        if (skillClass >= 0 && skillClass < 4 && skillLevel >= 0 && skillLevel < 4)
        {
          m_dwSkillAttackDelayEnd[skillClass][skillLevel] = now + delayMs;
        }
      }
    }
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
  _skill_fld *classSkill = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[2].GetRecord(wSkillIndex));
  const bool wasStealth = GetStealth(true);
  const unsigned __int8 errCode = skill_process(2, wSkillIndex, pidDst, pConsumeSerial, nullptr);
  if (classSkill)
  {
    AnimusDebugLog(
      "ClassSkillCast: acc=%u char=%s objSerial=%u skillIndex=%u skillCode=%s tempType=%d contType=%d err=%u",
      m_pUserDB ? m_pUserDB->m_dwSerial : 0u,
      m_Param.GetCharNameA(),
      m_dwObjSerial,
      wSkillIndex,
      classSkill->m_strCode,
      classSkill->m_nTempEffectType,
      classSkill->m_nContEffectType,
      errCode);
  }
  else
  {
    AnimusDebugLog(
      "ClassSkillCast: acc=%u char=%s objSerial=%u skillIndex=%u record=null err=%u",
      m_pUserDB ? m_pUserDB->m_dwSerial : 0u,
      m_Param.GetCharNameA(),
      m_dwObjSerial,
      wSkillIndex,
      errCode);
  }

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
  // Yorozuya fix (non-IDA parity): block force request in siege mode.
  else if (IsSiegeMode())
  {
    byErrCode = 14;
  }
  else if (m_byMoveType == 2)
  {
    byErrCode = 28;
  }
  else if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE) || m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
  {
    byErrCode = 24;
  }
  else if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_21))
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

  // Yorozuya fix (non-IDA parity): enforce fixed-weapon compatibility.
  if (!byErrCode && std::strchr(forceFld->m_strFixWeapon, '1') != nullptr)
  {
    if (m_pmWpn.GetAttackToolType() != 1 || forceFld->m_strFixWeapon[m_pmWpn.byWpType] != '1')
    {
      byErrCode = 8;
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
    else
    {
      // Yorozuya fix implementation (non-IDA): enforce force cast radius for buff/debuff.
      float availableDist = static_cast<float>(forceFld->m_nActDistance + 40);
      availableDist += target->GetWidth() / 2.0f;
      availableDist += m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_8);
      float dist = GetSqrt(target->m_fCurPos, m_fCurPos);
      if (dist > availableDist)
      {
        dist = GetSqrt(target->m_fOldPos, m_fCurPos);
        if (dist > availableDist)
        {
          byErrCode = 8;
        }
      }

      if (!byErrCode)
      {
        const float yDiffCur = std::fabs(target->m_fCurPos[1] - m_fCurPos[1]);
        const float yDiffOld = std::fabs(target->m_fOldPos[1] - m_fCurPos[1]);
        if (yDiffCur > 200.0f && yDiffOld > 200.0f)
        {
          byErrCode = 8;
        }
      }

      // Yorozuya fix (non-IDA parity): extra force attack delay tracking.
      if (!byErrCode && !m_bSFDelayNotCheck)
      {
        const int forceClass = static_cast<int>(forceFld->m_nClass);
        const int forceLevel = static_cast<int>(forceFld->m_nLv);
        if (forceClass >= 0 && forceClass < 6 && forceLevel >= 0 && forceLevel < 4)
        {
          const DWORD now = GetTickCount();
          if (!IsAttackDelayReady(now, m_dwForceAttackDelayEnd[forceClass][forceLevel]))
          {
            byErrCode = 9;
          }
          else
          {
            int delay = static_cast<int>(forceFld->m_fActDelay + m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_13));
            if (delay < 0)
            {
              delay = 0;
            }
            m_dwForceAttackDelayEnd[forceClass][forceLevel] = now + AdjustAttackDelayMs(static_cast<unsigned int>(delay));
          }
        }
      }
    }
    if (!byErrCode && !IsEffectableDst(forceFld->m_strActableDst, target))
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
  if (!byErrCode && target->m_EP.GetEff_State(EFF_STATE_INSUPERABLE)
      && (forceFld->m_nTempEffectType == -1 || !IsUsableTempEffectAtStoneState(forceFld->m_nTempEffectType)))
  {
    byErrCode = 24;
  }
  if (!byErrCode && target->m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
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
    forceLevel = static_cast<int>(GetSFLevel(forceFld->m_nLv, static_cast<unsigned int>(forceItem->m_dwDur)));
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

          const float effHave = m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_6);
          if (effHave > 1.0f)
          {
            alter = static_cast<int>(
              static_cast<float>(alter)
              + static_cast<float>(baseAlter)
                  * static_cast<float>(effHave - 1.0f));
          }

          const unsigned int oldDur = static_cast<unsigned int>(forceItem->m_dwDur);
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
      const float addDelay = m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_13);
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
  else if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE) || m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
  {
    byErrCode = 24;
  }
  else if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_21))
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
    else
    {
      // Yorozuya fix (non-IDA parity): enforce throw-skill target range (with Y distance check).
      float availableDist = (target->GetWidth() / 2.0f) + static_cast<float>(m_pmWpn.wGaAttRange);
      if (m_pmWpn.byWpType == 7 || m_pmWpn.byWpType == 11)
      {
        availableDist += m_EP.GetEff_Plus(EFF_PLUS_ATTACK_RANGE);
      }
      else
      {
        availableDist += m_EP.GetEff_Plus(m_pmWpn.byWpClass + 4);
      }

      float dist = GetSqrt(target->m_fCurPos, m_fCurPos);
      if (dist > availableDist)
      {
        dist = GetSqrt(target->m_fOldPos, m_fCurPos);
        if (dist > availableDist)
        {
          byErrCode = 5;
        }
      }

      if (!byErrCode)
      {
        const float yDiffCur = std::fabs(target->m_fCurPos[1] - m_fCurPos[1]);
        const float yDiffOld = std::fabs(target->m_fOldPos[1] - m_fCurPos[1]);
        if (yDiffCur > 200.0f && yDiffOld > 200.0f)
        {
          byErrCode = 5;
        }
      }

      // Yorozuya fix (non-IDA parity): extra normal-attack delay tracking for throw skills.
      if (!byErrCode && !m_bSFDelayNotCheck)
      {
        const DWORD now = GetTickCount();
        if (!IsAttackDelayReady(now, m_dwNormalAttackDelayEnd))
        {
          byErrCode = 9;
        }
        else
        {
          const int addDelay = static_cast<int>(CalcEquipAttackDelay());
          const int level = static_cast<int>(GetLevel());
          int attackDelay = static_cast<int>(m_pmWpn.GetAttackDelay(level, addDelay));
          if (m_pmWpn.byWpType != 11 && !m_pmWpn.byWpClass)
          {
            attackDelay = static_cast<int>(static_cast<float>(attackDelay) + m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_9));
          }
          if (m_pmWpn.byWpType == 7 || m_pmWpn.byWpType == 11)
          {
            attackDelay = static_cast<int>(static_cast<float>(attackDelay) + m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_11));
          }
          if (attackDelay < 0)
          {
            attackDelay = 0;
          }
          m_dwNormalAttackDelayEnd = now + AdjustAttackDelayMs(static_cast<unsigned int>(attackDelay));
        }
      }

      if (!byErrCode && !IsEffectableDst(skillFld->m_strActableDst, target))
      {
        byErrCode = 5;
      }
      else if (!byErrCode && skillFld->m_nTempEffectType == -1 && skillFld->m_nContEffectType == -1)
      {
        byErrCode = 8;
      }
      else if (!byErrCode && skillFld->m_nContEffectType != -1)
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
        attackDelay = static_cast<int>(static_cast<float>(attackDelay) + m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_9));
      }
      if (m_pmWpn.byWpType == 7 || m_pmWpn.byWpType == 11)
      {
        attackDelay = static_cast<int>(static_cast<float>(attackDelay) + m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_11));
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
  _unit_bullet_param *bulletParam = nullptr;
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
    bulletParam = &m_pUsingUnit->m_BulletParam[1];
    const unsigned __int16 bulletLeft = bulletParam->wLeftNum;
    const unsigned __int16 bulletItemIndex = bulletParam->wBulletIndex;
    if (!bulletLeft || bulletLeft == 65535)
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
    else
    {
      // Yorozuya fix (non-IDA parity): enforce unit throw range (with Y distance check).
      float availableDist = (target->GetWidth() / 2.0f) + weaponRecord->m_fAttackRange;
      float dist = GetSqrt(target->m_fCurPos, m_fCurPos);
      if (dist > availableDist)
      {
        dist = GetSqrt(target->m_fOldPos, m_fCurPos);
        if (dist > availableDist)
        {
          byErrCode = 5;
        }
      }

      if (!byErrCode)
      {
        const float yDiffCur = std::fabs(target->m_fCurPos[1] - m_fCurPos[1]);
        const float yDiffOld = std::fabs(target->m_fOldPos[1] - m_fCurPos[1]);
        if (yDiffCur > 200.0f && yDiffOld > 200.0f)
        {
          byErrCode = 5;
        }
      }

      // Yorozuya fix (non-IDA parity): extra unit-attack delay tracking.
      if (!byErrCode && !m_bSFDelayNotCheck)
      {
        const DWORD now = GetTickCount();
        if (!IsAttackDelayReady(now, m_dwUnitAttackDelayEnd))
        {
          byErrCode = 9;
        }
        else
        {
          int delay = static_cast<int>(weaponRecord->m_nAttackDel);
          if (delay < 0)
          {
            delay = 0;
          }
          m_dwUnitAttackDelayEnd = now + AdjustAttackDelayMs(static_cast<unsigned int>(delay));
        }
      }

      if (!byErrCode && !IsEffectableDst(skillFld->m_strActableDst, target))
      {
        byErrCode = 5;
      }
      else if (!byErrCode && skillFld->m_nTempEffectType == -1 && skillFld->m_nContEffectType == -1)
      {
        byErrCode = 8;
      }
      else if (!byErrCode && skillFld->m_nContEffectType != -1)
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
      unsigned __int16 bulletLeft = bulletParam->wLeftNum;
      if (bulletLeft > 0)
      {
        --bulletLeft;
      }
      SendMsg_AlterUnitBulletInform(1u, bulletLeft);
      if (!bulletLeft)
      {
        bulletParam->wBulletIndex = static_cast<unsigned __int16>(-1);
        bulletParam->wLeftNum = static_cast<unsigned __int16>(-1);
      }
      else
      {
        bulletParam->wLeftNum = bulletLeft;
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
          const _GuardTowerItem_fld *towerRecord = static_cast<const _GuardTowerItem_fld *>(tower->m_pRecordSet);
          const int nearRange = towerRecord->m_nGADst;
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
  _back_trap_result_zocl msg{};
  msg.byErrCode = byErrCode;

  unsigned __int8 type[2] = {17, 39};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CPlayer::SendMsg_MadeTrapNumInform(char byNum)
{
  _made_trap_num_inform_zocl msg{};
  msg.byNum = byNum;

  unsigned __int8 type[2] = {17, 29};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CPlayer::SendMsg_TowerContinue(unsigned __int16 wItemSerial, CGuardTower *pTwr)
{
_continue_tower_inform msg{};

  msg.wItemSerial = wItemSerial;
  msg.wTwrRecIndex = static_cast<unsigned __int16>(pTwr->m_pRecordSet->m_dwIndex);
  msg.wTwrIndex = pTwr->m_ObjID.m_wIndex;
  msg.dwTwrSerial = pTwr->m_dwObjSerial;

  unsigned __int8 type[2] = {17, 30};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
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
  float towerPos[3] = {m_fCurPos[0], pfPos[1], m_fCurPos[2]};
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

  if (m_byPosRaceTown != 255)
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
  // Yorozuya fix implementation (non-IDA): reject tower placement while the player is still
  // loading into a map because the request otherwise runs with invalid sector state.
  else if (IsMapLoading())
  {
    byErrCode = 1;
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
  // Yorozuya fix implementation (non-IDA): guard towers are placed at the player's feet, so the
  // server clamps x/z to the live player position instead of trusting packet coordinates.
  else if (GetSqrt(m_fCurPos, towerPos) > 40.0)
  {
    byErrCode = 9;
  }
  else if (IsOtherTowerNear(this, towerPos, nullptr))
  {
    byErrCode = 15;
  }
  else if (classSkill->m_nTempEffectType == -1)
  {
    byErrCode = 12;
  }

  // ---- Protection System: Tower creation level/validation check ----
  if (!byErrCode && ProtectionSystem::Instance().IsInitialized())
  {
    if (!CombatIntegrity::Instance().ValidateTowerCreate(this, towerPos[0], towerPos[1], towerPos[2]))
    {
      AntiCheat::Instance().AddScore(m_dwSerial, 15, "tower_hack");
      byErrCode = 13;
    }
  }

  if (!byErrCode)
  {
    needFp = static_cast<int>(static_cast<float>(classSkill->m_nNeedFP) * m_EP.GetEff_Rate(EFF_RATE_FP_COST));
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
    else if (!IsEffectableEquip(towerItem))
    {
      byErrCode = 1;
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
      towerPos,
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
      const float addDelay = m_EP.GetEff_Plus(EFF_PLUS_SKILL_ACT_DELAY);
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
      else if (hp > 65535)
      {
        hp = 65535;
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
  float trapPos[3] = {m_fCurPos[0], pfPos[1], m_fCurPos[2]};
  _skill_fld *classSkill = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[2].GetRecord(wSkillIndex));
  _TrapItem_fld *trapItemInfo = nullptr;
  _STORAGE_LIST::_db_con *trapItem = nullptr;
  _STORAGE_LIST::_db_con *consumeItems[3]{};
  int consumeCounts[3]{};
  bool consumeOverlap[3]{};
  int classGrade[2] = {-1, -1};
  int needFp = 0;

  if (m_byPosRaceTown != 255)
  {
    byErrCode = 16;
  }
  else if (!CTrap::IsHaveEmpty())
  {
    byErrCode = 1;
  }
  else if (!classSkill)
  {
    byErrCode = 13;
  }
  else if (m_byMoveType == 2)
  {
    byErrCode = 13;
  }
  // Yorozuya fix implementation (non-IDA): reject trap placement while the player is still
  // loading into a map because overlap checks otherwise run with invalid sector state.
  else if (IsMapLoading())
  {
    byErrCode = 1;
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
  // Yorozuya fix implementation (non-IDA): traps are placed at the player's feet, so the server
  // clamps x/z to the live player position instead of trusting packet coordinates.
  else if (GetSqrt(m_fCurPos, trapPos) > 40.0)
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
    else if (!IsEffectableEquip(trapItem))
    {
      byErrCode = 1;
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
        byErrCode = static_cast<unsigned __int8>(IsOtherInvalidObjNear(this, trapPos, nullptr, trapItemInfo));
      }
    }
  }

  if (!byErrCode && classSkill->m_nTempEffectType == -1)
  {
    byErrCode = 12;
  }
  if (!byErrCode)
  {
    needFp = static_cast<int>(static_cast<float>(classSkill->m_nNeedFP) * m_EP.GetEff_Rate(EFF_RATE_FP_COST));
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

    CTrap *trap = CreateTrap(this->m_pCurMap, this->m_wMapLayerIndex, trapPos, this, trapItem->m_wItemIndex);
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
      const float addDelay = m_EP.GetEff_Plus(EFF_PLUS_SKILL_ACT_DELAY);
      _ATTACK_DELAY_CHECKER::SetDelay(
        &m_AttDelayChker,
        static_cast<unsigned int>(classSkill->m_fActDelay + addDelay));
    }
  }

  this->SendMsg_CreateTrapResult(static_cast<char>(byErrCode), dwTrapObjSerial);
}

void CPlayer::_TrapReturn(CTrap *pTrap, unsigned __int16 wAddSerial)
{
  if (!pTrap)
  {
    return;
  }

  const int tableCode = GetItemTableCode(pTrap->m_pRecordSet->m_strCode);
  if (tableCode == -1)
  {
    return;
  }

  _base_fld *itemRecord = g_Main.m_tblItemData[tableCode].GetRecord(pTrap->m_pRecordSet->m_strCode);
  if (!itemRecord)
  {
    return;
  }

  if (wAddSerial == 65535)
  {
    _STORAGE_LIST::_db_con item;
    item.Init();
    item.m_byTableCode = static_cast<unsigned __int8>(tableCode);
    item.m_wItemIndex = static_cast<unsigned __int16>(itemRecord->m_dwIndex);
    if (IsOverLapItem(tableCode))
    {
      item.m_dwDur = 1;
    }
    else
    {
      item.m_dwDur = GetItemDurPoint(tableCode, item.m_wItemIndex);
    }

    if (m_Param.m_dbInven.GetIndexEmptyCon() == 255)
    {
      CreateItemBox(
        &item,
        this,
        -1,
        false,
        nullptr,
        1u,
        m_pCurMap,
        m_wMapLayerIndex,
        m_fCurPos,
        false);
      s_MgrItemHistory.back_trap_item(m_ObjID.m_wIndex, &item, m_szItemHistoryFileName);
    }
    else
    {
      item.m_wSerial = m_Param.GetNewItemSerial();
      if (Emb_AddStorage(0, &item, false, true))
      {
        SendMsg_RewardAddItem(&item, 6u);
        s_MgrItemHistory.back_trap_item(m_ObjID.m_wIndex, &item, m_szItemHistoryFileName);
      }
      else
      {
        s_MgrItemHistory.add_storage_fail(
          m_ObjID.m_wIndex,
          &item,
          "_TrapDestroy - Emb_AddStorage() Fail",
          m_szItemHistoryFileName);
      }
    }
    return;
  }

  if (!IsOverLapItem(tableCode))
  {
    return;
  }

  _STORAGE_LIST::_db_con *targetItem = m_Param.m_dbInven.GetPtrFromSerial(wAddSerial);
  if (!targetItem)
  {
    SendMsg_AdjustAmountInform(0, wAddSerial, 0);
    return;
  }

  if (!targetItem->m_bLock
      && targetItem->m_byTableCode == static_cast<unsigned __int8>(tableCode)
      && targetItem->m_wItemIndex == static_cast<unsigned __int16>(itemRecord->m_dwIndex))
  {
    if (targetItem->m_dwDur + 1 <= MAX_ITEM_OVERLAP)
    {
      const bool isProtectItem = IsProtectItem(tableCode) != 0;
      (void)isProtectItem;
      const unsigned int newDur = static_cast<unsigned int>(Emb_AlterDurPoint(0, targetItem->m_byStorageIndex, 1, true, false));
      SendMsg_AdjustAmountInform(0, wAddSerial, newDur);
    }
    else
    {
      SendMsg_AdjustAmountInform(0, wAddSerial, static_cast<unsigned int>(targetItem->m_dwDur));
    }
  }
}

void CPlayer::pc_BackTrapRequest(unsigned int dwTrapObjSerial, unsigned __int16 wAddSerial)
{
  unsigned __int8 byErrCode = 2;
  CTrap *trap = nullptr;

  if (m_pmTrp.m_nCount > 0)
  {
    for (int index = 0; index < 20; ++index)
    {
      if (m_pmTrp.m_Item[index].isLoad() && m_pmTrp.m_Item[index].dwSerial == dwTrapObjSerial)
      {
        trap = m_pmTrp.m_Item[index].pItem;
        if (!trap)
        {
          byErrCode = 2;
          break;
        }

        if (trap->m_dwMasterSerial != m_dwObjSerial)
        {
          byErrCode = 21;
          break;
        }

        if (GetSqrt(m_fCurPos, trap->m_fCurPos) > 150.0)
        {
          byErrCode = 22;
          break;
        }

        byErrCode = 0;
        break;
      }
    }

    if (byErrCode)
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

  if (!byErrCode)
  {
    _TrapReturn(trap, wAddSerial);
    trap->Destroy(2u);
  }

  SendMsg_BackTrapResult(static_cast<char>(byErrCode));
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
  _effect_add_inform_zocl payload{};
  payload.byLv = static_cast<char>(byLv);
  payload.wEffectCode = wEffectCode;
  payload.wDurSec = wDurSec;
  payload.dwPlayerSerial = dwPlayerSerial;
  if (dwPlayerSerial != 0)
  {
    strcpy_s(payload.wszPlayerName, sizeof(payload.wszPlayerName), wszPlayerName);
  }

  unsigned __int8 type[2] = {17, 10};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
  this->SendData_PartyMemberEffect(0, wEffectCode, byLv);
}

void CPlayer::SendMsg_DelEffect(unsigned __int8 byEffectCode, unsigned __int16 wEffectIndex, unsigned __int8 byLv)
{
  const unsigned __int16 effectBit =
    static_cast<unsigned __int16>(this->CalcEffectBit(byEffectCode, wEffectIndex));

  _effect_remove_inform_zocl payload{};
  payload.wEffectCode = effectBit;
  payload.dwPlayerSerial = this->m_dwObjSerial;

  unsigned __int8 type[2] = {17, 11};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
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

  std::memcpy(
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
  CPlayer *targetPlayer = static_cast<CPlayer *>(targetObject);
  for (int index = 0; index < 8; ++index)
  {
    _sf_continous *cont = &targetPlayer->m_SFCont[0][index];
    if (!cont->m_bExist)
    {
      continue;
    }

    currentInfo.m_PlayerContSf[contCount].wSfcode = static_cast<unsigned __int16>(this->CalcEffectBit(
      cont->m_byEffectCode,
      cont->m_wEffectIndex));
    currentInfo.m_PlayerContSf[contCount].byContCount = static_cast<unsigned __int8>(cont->m_nCumulCounter);
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

  std::memcpy(
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
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
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
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
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
  _player_die_zocl payload{};
  payload.wIndex = this->m_ObjID.m_wIndex;
  payload.dwSerial = this->m_dwObjSerial;

  unsigned __int8 type[2] = {3, 23};
  this->CircleReport(type, reinterpret_cast<char *>(&payload), static_cast<unsigned __int16>(sizeof(payload)), true);
}

void CPlayer::SendMsg_BreakdownEquipItem(unsigned __int8 byPartIndex, unsigned __int16 wSerial)
{
  _breakdown_equip_item_zocl payload{};
  payload.wPlayerIndex = this->m_ObjID.m_wIndex;
  payload.dwPlayerSerial = this->m_dwObjSerial;
  payload.dwEquipVer = this->GetVisualVer();
  payload.byPartIndex = static_cast<char>(byPartIndex);
  payload.wItemSerial = wSerial;

  unsigned __int8 type[2] = {5, 19};
  this->CircleReport(type, reinterpret_cast<char *>(&payload), static_cast<unsigned __int16>(sizeof(payload)), true);
}

void CPlayer::SendMsg_Notify_Gravity_Stone_Owner_Die()
{
  _guild_battle_notify_gravity_stone_owner_die_zocl payload{};
  payload.byObjID = static_cast<char>(this->m_ObjID.m_byID);
  payload.dwObjSerial = this->m_dwObjSerial;

  unsigned __int8 type[2] = {27, 86};
  this->CircleReport(type, reinterpret_cast<char *>(&payload), static_cast<unsigned __int16>(sizeof(payload)), true);
}

void CPlayer::SendMsg_FixPosition(int n)
{

  CPlayer *targetPlayer = &g_Player[n];
  if (!CanSendPlayerViewMessage(this, targetPlayer))
  {
    return;
  }

  _player_fixpositon_zocl msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  msg.wEquipVer = static_cast<unsigned __int16>(GetVisualVer());
  msg.byRaceCode = static_cast<char>(m_Param.GetRaceSexCode());
  FloatToShort(m_fCurPos, msg.zCur, 3);
  msg.wLastEffectCode = static_cast<unsigned __int16>(m_wLastContEffect);
  msg.dwStateFlag = GetStateFlag();
  msg.dwStateFlagEx = GetStateFlagEx();
  msg.byColor = m_byGuildBattleColorInx;

  unsigned __int8 type[2] = {4, 9};
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&msg), sizeof(msg));

  // Yorozuya fix (non-IDA parity): refresh full shape-all for the target viewer.
  if (!m_bLive)
  {
    SendMsg_OtherShapeError(targetPlayer, 0);
    return;
  }

  unsigned __int8 shapeType[2] = {3, 31};
  const unsigned __int16 shapeLen = static_cast<unsigned __int16>(m_bufShapeAll.size());
  g_Network.m_pProcess[0]->LoadSendMsg(
    targetPlayer->m_ObjID.m_wIndex,
    shapeType,
    reinterpret_cast<char *>(&m_bufShapeAll),
    shapeLen);
}

void CPlayer::SendMsg_RealMovePoint(int n)
{

  CPlayer *targetPlayer = &g_Player[n];
  if (!CanSendPlayerViewMessage(this, targetPlayer))
  {
    return;
  }

  _player_real_move_zocl msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  msg.dwEquipVer = static_cast<unsigned __int16>(GetVisualVer());
  msg.byRaceCode = static_cast<char>(m_Param.GetRaceSexCode());
  FloatToShort(m_fCurPos, msg.zCur, 3);
  msg.zTar[0] = static_cast<__int16>(static_cast<int>(m_fTarPos[0]));
  msg.zTar[1] = static_cast<__int16>(static_cast<int>(m_fTarPos[2]));
  msg.wLastEffectCode = static_cast<unsigned __int16>(m_wLastContEffect);
  msg.dwStateFlag = GetStateFlag();
  msg.dwStateFlagEx = GetStateFlagEx();
  msg.nAddSpeed = static_cast<__int16>(static_cast<int>(GetAddSpeed()));
  msg.byDirect = m_byMoveDirect;
  msg.byColor = m_byGuildBattleColorInx;

  unsigned __int8 type[2] = {4, 21};
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&msg), sizeof(msg));

  // Yorozuya fix (non-IDA parity): refresh full shape-all for the target viewer.
  if (!m_bLive)
  {
    SendMsg_OtherShapeError(targetPlayer, 0);
    return;
  }

  unsigned __int8 shapeType[2] = {3, 31};
  const unsigned __int16 shapeLen = static_cast<unsigned __int16>(m_bufShapeAll.size());
  g_Network.m_pProcess[0]->LoadSendMsg(
    targetPlayer->m_ObjID.m_wIndex,
    shapeType,
    reinterpret_cast<char *>(&m_bufShapeAll),
    shapeLen);
}

float CPlayer::CalcDPRate()
{
  const float dpRate = (2.0f * static_cast<float>(GetDP()) / static_cast<float>(GetMaxDP())) - 0.40000001f;
  if (dpRate < 0.0f)
  {
    return 0.0f;
  }

  if (dpRate > 1.0f)
  {
    return 1.0f;
  }

  return dpRate;
}

int CPlayer::GetAvoidRate()
{
  if (m_fTalik_AvoidPoint <= 0.0f)
  {
    // narrowing cast for thunk return parity
    return static_cast<int>(static_cast<unsigned int>(static_cast<int>(m_EP.GetEff_Plus(EFF_PLUS_AVOID_RATE))));
  }

  const int talikPenalty = static_cast<int>(m_fTalik_AvoidPoint * (1.0f - CalcDPRate()));
  // narrowing cast for thunk return parity
  return static_cast<int>(static_cast<unsigned int>(static_cast<int>(m_EP.GetEff_Plus(EFF_PLUS_AVOID_RATE) - static_cast<float>(talikPenalty))));
}

int CPlayer::GetDefFC(int nAttactPart, CCharacter *pAttChar, int *pnConvertPart)
{
  m_nLastBeatenPart = nAttactPart;

  float defenseValue = 0.0f;
  if (IsRidingUnit())
  {
    defenseValue = static_cast<float>(m_nUnitDefFc) * m_fUnitPv_DefFc;
    if (m_bGeneratorDefense)
    {
      CCharacter *playerAttacker = nullptr;
      if (pAttChar && pAttChar->m_ObjID.m_byID == 0)
      {
        playerAttacker = pAttChar;
      }

      float generatorDefenseBase = 1.0f;
      if (playerAttacker)
      {
        for (int partIndex = 0; partIndex < 5; ++partIndex)
        {
          _STORAGE_LIST::_db_con *equipItem = &m_Param.m_dbEquip.m_pStorageList[partIndex];
          if (!equipItem->m_bLoad)
          {
            continue;
          }

          _DfnEquipItem_fld *equipRecord =
            static_cast<_DfnEquipItem_fld *>(g_Main.m_tblItemData[partIndex].GetRecord(equipItem->m_wItemIndex));
          if (equipRecord)
          {
            generatorDefenseBase += equipRecord->m_fDefFc;
          }
        }
      }

      defenseValue += generatorDefenseBase / 5.0f;

      m_EP.SetLock(false);
      defenseValue += m_EP.GetEff_Rate(EFF_RATE_ARMOR_DEFENSE) - 1.0f;
      if (m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_91) > 0.0f)
      {
        defenseValue += generatorDefenseBase * (m_EP.GetEff_Rate(EFF_RATE_FINAL_DEFENSE) - 1.0f);
        defenseValue += m_EP.GetEff_Rate(EFF_RATE_UNKNOWN_65) - 1.0f;
      }
      defenseValue *= m_EP.GetEff_Rate(EFF_RATE_UNKNOWN_44);
      m_EP.SetLock(true);
    }
  }
  else
  {
    bool shieldIgnore = false;
    if (pAttChar && pAttChar->m_ObjID.m_byID == 0)
    {
      shieldIgnore = pAttChar->m_EP.GetEff_State(EFF_STATE_UNKNOWN_11);
      if (!shieldIgnore)
      {
        CCharacter *playerAttacker = nullptr;
        if (pAttChar->m_ObjID.m_byID == 0)
        {
          playerAttacker = pAttChar;
        }

        if (pAttChar->m_EP.GetEff_Plus(EFF_PLUS_SHIELD_IGNORE_RATE) > 0.0f || playerAttacker)
        {
          float ignoreChance = pAttChar->m_EP.GetEff_Plus(EFF_PLUS_SHIELD_IGNORE_RATE);
          if (playerAttacker)
          {
            ignoreChance += pAttChar->m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_41);
          }

          if ((rand() % 100) <= static_cast<int>(ignoreChance))
          {
            shieldIgnore = true;
          }
        }
      }
    }

    if (shieldIgnore)
    {
      pAttChar->SendMsg_AttackActEffect(1u, this);
    }

    _STORAGE_LIST::_db_con *shieldItem = &m_Param.m_dbEquip.m_pStorageList[5];
    bool hasShield = false;
    if (shieldItem->m_bLoad)
    {
      hasShield = GetEffectEquipCode(1u, 5u) == 1;
    }

    if (hasShield)
    {
      _STORAGE_LIST::_db_con *weaponItem = &m_Param.m_dbEquip.m_pStorageList[6];
      if (weaponItem->m_bLoad)
      {
        _WeaponItem_fld *weaponRecord = static_cast<_WeaponItem_fld *>(g_Main.m_tblItemData[6].GetRecord(weaponItem->m_wItemIndex));
        if (weaponRecord->m_nFixPart == 100)
        {
          hasShield = false;
        }
      }
    }

    if (pAttChar && hasShield && !shieldIgnore)
    {
      const int shieldMastery = m_pmMst.GetMasteryPerMast(2u, 0);
      int blockChance = static_cast<int>((static_cast<float>(shieldMastery) / 99.0f) * 20.0f + 5.0f);
      blockChance = static_cast<int>(static_cast<float>(blockChance) + m_EP.GetEff_Plus(EFF_PLUS_SHIELD_BLOCK_RATE));
      if (blockChance > 100)
      {
        blockChance = 100;
      }

      if ((rand() % 100) < blockChance)
      {
        m_nLastBeatenPart = 5;
        return -2;
      }
    }

    if (hasShield)
    {
      float partDefense[5]{};
      for (int partIndex = 0; partIndex < 5; ++partIndex)
      {
        _STORAGE_LIST::_db_con *equipItem = &m_Param.m_dbEquip.m_pStorageList[partIndex];
        _DfnEquipItem_fld *partRecord = nullptr;
        if (equipItem->m_bLoad && GetEffectEquipCode(1u, static_cast<unsigned __int8>(partIndex)) == 1)
        {
          partRecord = static_cast<_DfnEquipItem_fld *>(g_Main.m_tblItemData[partIndex].GetRecord(equipItem->m_wItemIndex));
        }
        else
        {
          partRecord = static_cast<_DfnEquipItem_fld *>(
            g_Main.m_tblItemData[partIndex].GetRecord(m_Param.m_dbChar.m_byDftPart[partIndex]));
        }

        if (partRecord)
        {
          partDefense[partIndex] = partRecord->m_fDefFc;
        }
      }

      _DfnEquipItem_fld *shieldRecord = static_cast<_DfnEquipItem_fld *>(g_Main.m_tblItemData[5].GetRecord(shieldItem->m_wItemIndex));
      for (int index = 0; index < 5; ++index)
      {
        defenseValue += partDefense[index] * s_fPartGravity[index];
      }
      defenseValue += shieldRecord->m_fDefFc;
      defenseValue *= m_EP.GetEff_Rate(EFF_RATE_DEFENSE);
    }
    else
    {
      _STORAGE_LIST::_db_con *equipItem = &m_Param.m_dbEquip.m_pStorageList[m_nLastBeatenPart];
      if (equipItem->m_bLoad && GetEffectEquipCode(1u, static_cast<unsigned __int8>(m_nLastBeatenPart)) == 1)
      {
        _DfnEquipItem_fld *equipRecord =
          static_cast<_DfnEquipItem_fld *>(g_Main.m_tblItemData[m_nLastBeatenPart].GetRecord(equipItem->m_wItemIndex));
        defenseValue = equipRecord->m_fDefFc;
      }
      else
      {
        _DfnEquipItem_fld *defaultRecord = static_cast<_DfnEquipItem_fld *>(
          g_Main.m_tblItemData[m_nLastBeatenPart].GetRecord(m_Param.m_dbChar.m_byDftPart[m_nLastBeatenPart]));
        defenseValue = defaultRecord->m_fDefFc;
      }
    }

    if (pnConvertPart)
    {
      *pnConvertPart = m_nLastBeatenPart;
    }

    defenseValue *= m_EP.GetEff_Rate(EFF_RATE_ARMOR_DEFENSE);
    if (IsSiegeMode())
    {
      defenseValue *= m_EP.GetEff_Rate(EFF_RATE_SIEGE_DEFENSE);
    }
  }

  if (!m_bInGuildBattle)
  {
    const unsigned int charSerial = m_Param.GetCharSerial();
    const int raceCode = static_cast<int>(m_Param.GetRaceCode());
    const unsigned __int8 bossType = CPvpUserAndGuildRankingSystem::Instance()->GetBossType(raceCode, charSerial);
    if (!bossType)
    {
      defenseValue *= 1.3f;
    }
    else if (bossType == 1 || bossType == 5)
    {
      defenseValue *= 1.5f;
    }
    else if (bossType == 3 || bossType == 7)
    {
      defenseValue *= 1.2f;
    }
  }

  if (g_HolySys.GetDestroyerSerial() == m_dwObjSerial || IsLastAttBuff())
  {
    defenseValue *= 1.3f;
  }

  if (!IsRidingUnit())
  {
    if (m_fTalik_DefencePoint > 0.0f)
    {
      const float talikPenalty = m_fTalik_DefencePoint * (1.0f - CalcDPRate());
      defenseValue *= (m_EP.GetEff_Rate(EFF_RATE_FINAL_DEFENSE) - talikPenalty);
    }
    else
    {
      defenseValue *= m_EP.GetEff_Rate(EFF_RATE_FINAL_DEFENSE);
    }
  }

  // narrowing cast for thunk return parity
  return static_cast<int>(static_cast<int>(defenseValue));
}

float CPlayer::GetDefFacing(int nPart)
{
  if (IsRidingUnit())
  {
    if (m_pUsingUnit)
    {
      _UnitFrame_fld *record = static_cast<_UnitFrame_fld *>(g_Main.m_tblUnitFrame.GetRecord(m_pUsingUnit->byFrame));
      if (record)
      {
        return record->m_fDefFacing;
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

  _DfnEquipItem_fld *record = static_cast<_DfnEquipItem_fld *>(g_Main.m_tblItemData[nPart].GetRecord(partCode));
  if (!record)
  {
    return FLOAT_0_5;
  }
  return record->m_fDefFacing;
}

float CPlayer::GetDefGap(int nPart)
{
  if (IsRidingUnit())
  {
    if (m_pUsingUnit)
    {
      _UnitFrame_fld *record = static_cast<_UnitFrame_fld *>(g_Main.m_tblUnitFrame.GetRecord(m_pUsingUnit->byFrame));
      if (record)
      {
        return record->m_fDefGap;
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

  _DfnEquipItem_fld *record = static_cast<_DfnEquipItem_fld *>(g_Main.m_tblItemData[nPart].GetRecord(partCode));
  if (!record)
  {
    return FLOAT_0_5;
  }
  return record->m_fDefGap;
}

int CPlayer::GetDefSkill(bool bBackAttackDamage)
{
if (!IsRidingUnit())
  {
    // narrowing cast for thunk return parity
    return static_cast<int>(m_pmMst.GetMasteryPerMast(1u, 0u));
  }

  int totalDefSkill = 0;
  _UnitPart_fld *leftPart = static_cast<_UnitPart_fld *>(g_Main.m_tblUnitPart[0].GetRecord(m_pUsingUnit->byPart[0]));
  if (leftPart)
  {
    totalDefSkill += leftPart->m_nDefMastery;
  }

  _UnitPart_fld *rightPart = static_cast<_UnitPart_fld *>(g_Main.m_tblUnitPart[1].GetRecord(m_pUsingUnit->byPart[1]));
  if (rightPart)
  {
    totalDefSkill += rightPart->m_nDefMastery;
  }

  // narrowing cast for thunk return parity
  return static_cast<int>(static_cast<unsigned int>(totalDefSkill));
}

int CPlayer::GetFireTol()
{
  const float total = static_cast<float>(m_nTolValue[0]) + m_EP.GetEff_Plus(EFF_PLUS_FIRE_TOLERANCE);
  int value = static_cast<int>(total * m_EP.GetEff_Rate(EFF_RATE_FIRE_TOLERANCE));
  value = ClampToleranceValue(value);
  if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_19) && value > 0)
  {
    value = -value;
  }
  // narrowing cast for thunk return parity
  return static_cast<int>(static_cast<unsigned int>(value));
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

int CPlayer::GetObjRace()
{
  // narrowing cast for thunk return parity
  return static_cast<int>(static_cast<int>(m_Param.GetRaceCode()));
}

int CPlayer::GetSoilTol()
{
  const float total = static_cast<float>(m_nTolValue[2]) + m_EP.GetEff_Plus(EFF_PLUS_SOIL_TOLERANCE);
  int value = static_cast<int>(total * m_EP.GetEff_Rate(EFF_RATE_SOIL_TOLERANCE));
  value = ClampToleranceValue(value);
  if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_19) && value > 0)
  {
    value = -value;
  }
  // narrowing cast for thunk return parity
  return static_cast<int>(static_cast<unsigned int>(value));
}

int CPlayer::GetWaterTol()
{
  const float total = static_cast<float>(m_nTolValue[1]) + m_EP.GetEff_Plus(EFF_PLUS_WATER_TOLERANCE);
  int value = static_cast<int>(total * m_EP.GetEff_Rate(EFF_RATE_WATER_TOLERANCE));
  value = ClampToleranceValue(value);
  if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_19) && value > 0)
  {
    value = -value;
  }
  // narrowing cast for thunk return parity
  return static_cast<int>(static_cast<unsigned int>(value));
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

  _WeaponItem_fld *weaponRecord = static_cast<_WeaponItem_fld *>(g_Main.m_tblItemData[6].GetRecord(weaponItem->m_wItemIndex));
  if (!weaponRecord)
  {
    return 0.0f;
  }
  if (weaponRecord->m_nType == 10)
  {
    return 0.0f;
  }

  return weaponRecord->m_fAttGap;
}

int CPlayer::GetWeaponClass()
{
  // narrowing cast for thunk return parity
  return static_cast<int>(m_pmWpn.byWpClass);
}

float CPlayer::GetWidth()
{
  return static_cast<_player_fld *>(m_pRecordSet)->m_fWidth;
}

int CPlayer::GetWindTol()
{
  const float total = static_cast<float>(m_nTolValue[3]) + m_EP.GetEff_Plus(EFF_PLUS_WIND_TOLERANCE);
  int value = static_cast<int>(total * m_EP.GetEff_Rate(EFF_RATE_WIND_TOLERANCE));
  value = ClampToleranceValue(value);
  if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_19) && value > 0)
  {
    value = -value;
  }
  // narrowing cast for thunk return parity
  return static_cast<int>(static_cast<unsigned int>(value));
}

bool CPlayer::IsBeDamagedAble(CCharacter *pAtter)
{
  if (!pAtter)
  {
    return false;
  }

  if (pAtter->m_ObjID.m_byID != 0)
  {
    return true;
  }

  CPlayer *attacker = static_cast<CPlayer *>(pAtter);
  if (!attacker->m_bInGuildBattle && m_bInGuildBattle)
  {
    return false;
  }
  if (attacker->m_bInGuildBattle && !m_bInGuildBattle)
  {
    return false;
  }
  if (!attacker->m_bInGuildBattle && !m_bInGuildBattle)
  {
    return true;
  }

  return (attacker->m_byGuildBattleColorInx != m_byGuildBattleColorInx) != 0;
}

bool CPlayer::IsRecvableContEffect()
{
  if (IsRidingUnit())
  {
    return false;
  }
  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE))
  {
    return false;
  }
  return (m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE) ? 0 : 1) != 0;
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
  char *raceCode = cvt_string(static_cast<int>(deadPlayer->m_Param.GetRaceCode()));
  if (!Emb_CreateQuestEvent(quest_happen_type_pk, raceCode))
  {
    Emb_CheckActForQuest(2, raceCode, 1u, false);
  }

  CalcPvP(deadPlayer, 0);
  CalPvpTempCash(deadPlayer, 0);
  IncCriEffKillPoint();
}

void CPlayer::CalcPvP(CPlayer *pDier, unsigned __int8 byKillerObjID)
{
  (void)byKillerObjID;

  if (IsChaosMode() && GetObjRace() == pDier->GetObjRace())
  {
    return;
  }

  if (pDier->IsPunished(1u, false) && GetObjRace() == pDier->GetObjRace())
  {
    return;
  }

  long double pvpPoint = 1.0;
  if (pDier->m_Param.GetPvPPoint() <= 0.0 || m_byUserDgr != pDier->m_byUserDgr)
  {
    if (!pDier->m_bInGuildBattle)
    {
      pvpPoint = pvpPoint * 50.0 / 100.0;
      if (pvpPoint < 1.0)
      {
        pvpPoint = 1.0;
      }

      CPlayer *partyMembers[8]{};
      unsigned __int8 memberCount = 0;
      if (m_pPartyMgr->IsPartyMode()
          && pvpPoint != 1.0
          && (memberCount = _GetPartyMemberInCircle(partyMembers, 8, false), static_cast<double>(memberCount) <= pvpPoint)
          && memberCount)
      {
        IncPvPPoint(pvpPoint * 0.699999988079071, kill_s_inc, pDier->m_dwObjSerial);

        int totalLevel = 0;
        for (int index = 0; index < memberCount; ++index)
        {
          totalLevel += static_cast<int>(partyMembers[index]->GetLevel());
        }

        for (int index = 0; index < memberCount; ++index)
        {
          const float levelRate = static_cast<float>(partyMembers[index]->GetLevel()) / static_cast<float>(totalLevel);
          const long double alterPoint = pvpPoint * 0.300000011920929 * levelRate;
          if (alterPoint >= 1.0)
          {
            partyMembers[index]->IncPvPPoint(alterPoint, kill_p_inc, pDier->m_dwObjSerial);
          }
        }
      }
      else
      {
        IncPvPPoint(pvpPoint, kill_s_inc, pDier->m_dwObjSerial);
      }
    }
    return;
  }

  const double dstPoint = pDier->m_Param.GetPvPPoint() + 10000.0;
  const double srcPoint = m_Param.GetPvPPoint() + 10000.0;
  pvpPoint = dstPoint / srcPoint * 500.0 + 0.5;
  if (pvpPoint < 1.0)
  {
    pvpPoint = 1.0;
  }
  if (pvpPoint > 100000000.0)
  {
    pvpPoint = 100000000.0;
  }

  __int16 *dstBillingType = &pDier->m_pUserDB->m_BillingInfo.iType;
  __int16 *srcBillingType = &m_pUserDB->m_BillingInfo.iType;
  CNationSettingManager *nationSetting = CTSingleton<CNationSettingManager>::Instance();
  if (nationSetting->IsPersonalFreeFixedAmountBillingType(srcBillingType, dstBillingType))
  {
    return;
  }

  if (pvpPoint > pDier->m_Param.GetPvPPoint())
  {
    pvpPoint = pDier->m_Param.GetPvPPoint();
  }

  CHolyStone *holyStone = &g_Stone[static_cast<int>(m_Param.GetRaceCode())];
  if (holyStone->m_pCurMap == m_pCurMap && g_HolySys.GetSceneCode() == 1)
  {
    pvpPoint *= 3.0;
  }

  if (!pDier->m_bInGuildBattle && !pDier->m_kPvpPointLimiter.TakePvpPoint(&pvpPoint, pDier, this))
  {
    return;
  }

  if (!pDier->m_bInGuildBattle)
  {
    pDier->AlterPvPPoint(-static_cast<double>(pvpPoint), die_dec, m_dwObjSerial);
  }

  if (!pDier->m_bInGuildBattle)
  {
    pvpPoint = pvpPoint * 50.0 / 100.0;
    if (pvpPoint < 1.0)
    {
      pvpPoint = 1.0;
    }

    CPlayer *partyMembers[8]{};
    unsigned __int8 memberCount = 0;
    if (m_pPartyMgr->IsPartyMode()
        && pvpPoint != 1.0
        && (memberCount = _GetPartyMemberInCircle(partyMembers, 8, false), static_cast<double>(memberCount) <= pvpPoint)
        && memberCount)
    {
      IncPvPPoint(pvpPoint * 0.699999988079071, kill_s_inc, pDier->m_dwObjSerial);

      int totalLevel = 0;
      for (int index = 0; index < memberCount; ++index)
      {
        totalLevel += static_cast<int>(partyMembers[index]->GetLevel());
      }

      for (int index = 0; index < memberCount; ++index)
      {
        const float levelRate = static_cast<float>(partyMembers[index]->GetLevel()) / static_cast<float>(totalLevel);
        const long double alterPoint = pvpPoint * 0.300000011920929 * levelRate;
        if (alterPoint >= 1.0)
        {
          partyMembers[index]->IncPvPPoint(alterPoint, kill_p_inc, pDier->m_dwObjSerial);
        }
      }
    }
    else
    {
      IncPvPPoint(pvpPoint, kill_s_inc, pDier->m_dwObjSerial);
    }
  }
}

long double CPlayer::CalPvpCashPoint(int nDstLv, int nSrcLv, char *pSrcClass)
{
  return m_kPvpCashPoint.CalPvpCashPoint(nDstLv, nSrcLv, pSrcClass, 1u);
}

void CPlayer::CalPvpTempCash(CPlayer *pDier, unsigned __int8 byKillerObjID)
{
  (void)byKillerObjID;

  if (IsChaosMode() && GetObjRace() == pDier->GetObjRace())
  {
    return;
  }
  if (pDier->IsChaosMode() && GetObjRace() == pDier->GetObjRace())
  {
    return;
  }
  if (pDier->IsPunished(1u, false) && GetObjRace() == pDier->GetObjRace())
  {
    return;
  }
  if (IsPunished(1u, false) && GetObjRace() == pDier->GetObjRace())
  {
    return;
  }
  if (pDier->m_bInGuildBattle || m_bInGuildBattle)
  {
    return;
  }

  bool haveCashChanged = false;
  bool loseCashChanged = false;
  double cashPoint = 0.0;
  long double tempCashDelta = 0.0;
  int tempPointLimit = 0;
  const long double oldTempPoint = pDier->m_kPvpOrderView.GetPvpTempCash();

  char *srcClass = m_Param.m_pClassData->m_strCode;
  const int srcLevel = static_cast<int>(GetLevel());
  const int dstLevel = static_cast<int>(pDier->GetLevel());
  cashPoint = m_kPvpCashPoint.CalPvpCashPoint(dstLevel, srcLevel, srcClass, 1u);

  if (!pDier->m_kPvpCashPoint.CheckPvpLoseCondition(this, pDier))
  {
    cashPoint = 0.0;
  }

  if (cashPoint != 0.0)
  {
    tempPointLimit = pDier->m_kPvpCashPoint.GetMinTempPoint(static_cast<unsigned __int8>(pDier->m_Param.GetLevel()));
    if (static_cast<double>(tempPointLimit) <= pDier->m_kPvpOrderView.GetPvpTempCash() - cashPoint)
    {
      tempCashDelta = -cashPoint;
    }
    else
    {
      tempCashDelta = static_cast<double>(tempPointLimit) - pDier->m_kPvpOrderView.GetPvpTempCash();
    }

    pDier->m_kPvpOrderView.Update_PvpTempCash(pDier->m_ObjID.m_wIndex, static_cast<double>(tempCashDelta));
    const int contLose = pDier->m_kPvpCashPoint.GetContPvpLose();
    pDier->m_kPvpCashPoint.SetContPvpLose(static_cast<unsigned __int8>(contLose + 1));
    pDier->m_kPvpOrderView.Update_ContLoseCash(static_cast<unsigned __int8>(pDier->m_kPvpCashPoint.GetContPvpLose()));
    loseCashChanged = true;
  }

  pDier->m_kPvpCashPoint.SetContPvpHave(0);
  pDier->m_kPvpOrderView.Update_ContHaveCash(0);
  if (!m_kPvpCashPoint.CheckPvpHaveCondition(this, pDier, static_cast<double>(oldTempPoint)))
  {
    cashPoint = 0.0;
  }

  const double pvpCash = static_cast<double>(pDier->m_kPvpOrderView.GetPvpCash());
  tempPointLimit = pDier->m_kPvpCashPoint.GetMinTempPoint(static_cast<unsigned __int8>(pDier->m_Param.GetLevel()));
  if (static_cast<double>(10 * tempPointLimit) > pvpCash)
  {
    cashPoint = 0.0;
  }

  if (cashPoint != 0.0)
  {
    const bool premium = IsApplyPcbangPrimium();
    tempPointLimit = m_kPvpCashPoint.GetMaxTempPoint(static_cast<unsigned __int8>(m_Param.GetLevel()), premium);
    if (IsApplyPcbangPrimium())
    {
      cashPoint *= PCBANG_PRIMIUM_FAVOR::PVP_RATE;
    }

    if (cashPoint + m_kPvpOrderView.GetPvpTempCash() <= static_cast<double>(tempPointLimit))
    {
      tempCashDelta = cashPoint;
    }
    else
    {
      tempCashDelta = static_cast<double>(tempPointLimit) - m_kPvpOrderView.GetPvpTempCash();
    }

    m_kPvpOrderView.Update_PvpTempCash(m_ObjID.m_wIndex, static_cast<double>(tempCashDelta));
    const int contHave = m_kPvpCashPoint.GetContPvpHave();
    m_kPvpCashPoint.SetContPvpHave(static_cast<unsigned __int8>(contHave + 1));
    m_kPvpOrderView.Update_ContHaveCash(static_cast<unsigned __int8>(m_kPvpCashPoint.GetContPvpHave()));
    haveCashChanged = true;
  }

  m_kPvpCashPoint.SetContPvpLose(0);
  m_kPvpOrderView.Update_ContLoseCash(0);

  if (haveCashChanged || loseCashChanged)
  {
    const unsigned int killerSerial = m_Param.GetCharSerial();
    if (pDier->m_kPvpCashPoint.SetKillerList(killerSerial))
    {
      pDier->m_kPvpCashPoint.UpdateKillerList(&pDier->m_pUserDB->m_AvatorData.dbPvpOrderView);
    }
  }
}

void CPlayer::SendMsg_MaxPvpPointInform(int nMax)
{
  _notify_max_pvp_point_zocl payload{};
  payload.nMaxPoint = nMax;

  unsigned __int8 type[2] = {59, 15};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
}

void CPlayer::IncCriEffKillPoint()
{
  if (m_byHSKQuestCode != 100)
  {
    CHolyStone *holyStone = &g_Stone[static_cast<int>(m_Param.GetRaceCode())];
    if (holyStone->m_bOper && holyStone->m_pCurMap == m_pCurMap)
    {
      ++m_wKillPoint;
      SendMsg_HSKQuestActCum();
      m_pUserDB->m_AvatorData.m_wKillPoint = m_wKillPoint;
    }
  }
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
  _fcitem_inform_zocl msg{};
  msg.wItemSerial = wItemSerial;
  msg.dwNewStat = dwNewStat;

  unsigned __int8 pbyType[2]{3, 44};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
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

