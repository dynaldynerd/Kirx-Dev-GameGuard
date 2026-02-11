#include "pch.h"

#include "CPlayer.h"
#include "NetCheckPackets.h"
#include <array>
#include "CAttack.h"
#include "CPartyModeKillMonsterExpNotify.h"
#include "CPlayerAttack.h"
#include "attack_count_result_zocl.h"
#include "attack_force_result_zocl.h"
#include "attack_gen_result_zocl.h"
#include "attack_selfdestruction_result_zocl.h"
#include "attack_siege_result_zocl.h"
#include "attack_skill_result_zocl.h"
#include "attack_unit_result_zocl.h"
#include "force_result_one_zocl.h"
#include "force_result_other_zocl.h"
#include "skill_result_one_zocl.h"
#include "skill_result_other_zocl.h"
#include "class_skill_result_one_zocl.h"
#include "class_skill_result_other_zocl.h"
#include "make_tower_result_zocl.h"
#include "back_tower_result_zocl.h"
#include "make_trap_result_zocl.h"
#include "back_trap_result_zocl.h"
#include "throw_skill_result_one_zocl.h"
#include "throw_skill_result_other_zocl.h"
#include "throw_unit_result_one_zocl.h"
#include "throw_unit_result_other_zocl.h"
#include "combine_ex_item_accept_result_zocl.h"
#include "combine_ex_item_accept_request_clzo.h"
#include "combine_ex_item_request_clzo.h"
#include "combine_ex_item_result_zocl.h"
#include "exchange_lend_item_result_zocl.h"
#include "buy_offer.h"
#include "sell_offer.h"
#include "buy_store_failure_zocl.h"
#include "buy_store_success_zocl.h"
#include "item_fanfare_zocl.h"
#include "itembox_take_add_result_zocl.h"
#include "itembox_take_new_result_zocl.h"
#include "limit_amount_info.h"
#include "limit_item_num_info_zocl.h"
#include "sell_store_result_zocl.h"
#include "store_list_result_zocl.h"
#include "unit_bullet_param.h"
#include "unit_frame_buy_result_zocl.h"
#include "unit_sell_result_zocl.h"
#include "unit_part_tuning_result_zocl.h"
#include "unit_frame_repair_result_zocl.h"
#include "unit_bullet_fill_result_zocl.h"
#include "unit_pack_fill_result_zocl.h"
#include "unit_delivery_result_zocl.h"
#include "unit_return_result_zocl.h"
#include "unit_take_result_zocl.h"
#include "unit_leave_result_zocl.h"
#include "unit_bullet_replace_result_zocl.h"
#include "UnitKeyItem_fld.h"
#include "quest_check_result.h"
#include "npc_quest_list_result_zocl.h"
#include "insert_new_quest_inform_zocl.h"
#include "insert_next_quest_inform_zocl.h"
#include "mine_start_result_zocl.h"
#include "mine_complete_result_zocl.h"
#include "ore_cutting_result_zocl.h"
#include "ore_into_bag_result_zocl.h"
#include "cutting_complete_result_zocl.h"
#include "party_join_joiner_result_zocl.h"
#include "party_join_member_result_zocl.h"
#include "party_member_info_upd.h"
#include "npclink_check_item_result_zocl.h"
#include "radar_char_list_result_zocl.h"
#include "CRecordData.h"
#include "ObjectCreateSetData.h"
#include "AnimusItem_fld.h"
#include "CAnimus.h"
#include "CMonster.h"
#include "CMapData.h"
#include "CObjectList.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "pnt_rect.h"
#include "CLevel.h"
#include "CTransportShip.h"
#include "CUserDB.h"
#include "CGuardTower.h"
#include "CTrap.h"
#include "CItemStore.h"
#include "CItemBox.h"
#include "CActionPointSystemMgr.h"
#include "CMoneySupplyMgr.h"
#include "COreAmountMgr.h"
#include "COreCuttingTable.h"
#include "CNuclearBombMgr.h"
#include "CNationSettingManager.h"
#include "CParkingUnit.h"
#include "CHolyStoneSystem.h"
#include "CHolyStone.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CGuildBattleController.h"
#include "CGuild.h"
#include "CGuildMasterEffect.h"
#include "CPartyPlayer.h"
#include "PatriarchElectProcessor.h"
#include "CHonorGuild.h"
#include "CGoldenBoxItemMgr.h"
#include "CRaceBuffManager.h"
#include "CRaceBossWinRate.h"
#include "CashItemRemoteStore.h"
#include "CLogFile.h"
#include "CTSingleton.h"
#include "LendItemMng.h"
#include "ITEM_EFFECT.h"
#include "ItemUpgrade_fld.h"
#include "TimeLimitJadeMng.h"
#include "TimeItem.h"
#include "ResourceItem_fld.h"
#include "portal_fld.h"
#include "UIDGenerator.h"
#include "WorldServerUtil.h"
#include "CExchangeEvent.h"
#include "CPcBangFavor.h"
#include "CUnmannedTraderTaxRateManager.h"
#include "CUnmannedTraderController.h"
#include "CUnmannedTraderRegistItemInfo.h"
#include "CMoveMapLimitManager.h"
#include "CPostSystemManager.h"
#include "CBillingManager.h"
#include "CGuildRoomSystem.h"
#include "CCouponMgr.h"
#include "EQUIP_MASTERY_LIM.h"
#include "CDarkHoleChannel.h"
#include "CDarkHoleDungeonQuest.h"
#include "CGuardTower.h"
#include "CTrap.h"
#include "CSetItemEffect.h"
#include "DqsDbStructs.h"
#include "exit_alter_param.h"
#include "alter_cont_effect_time_zocl.h"
#include "notify_not_use_premium_cashitem_zocl.h"
#include "post_content_result_zocl.h"
#include "post_recv_delivery_zocl.h"
#include "post_result_zocl.h"
#include "post_return_zocl.h"
#include "pt_inform_commission_income_zocl.h"
#include "pt_inform_punishment_zocl.h"
#include "pt_result_change_tax_rate_zocl.h"
#include "qry_case_selfleave.h"
#include "chat_message_receipt_udp.h"
#include "announ_message_receipt_udp.h"
#include "chat_far_failure_zocl.h"
#include "chat_guild_failure_zocl.h"
#include "chat_multi_far_failure_zocl.h"
#include "chat_multi_far_trans_zocl.h"
#include "base_download_result_zocl.h"
#include "inven_download_result_zocl.h"
#include "cum_download_result_zocl.h"
#include "force_download_result_zocl.h"
#include "quest_download_result_zocl.h"
#include "quest_history_download_result_zocl.h"
#include "unit_download_result_zocl.h"
#include "animus_download_result_zocl.h"
#include "personal_automine_download_zocl.h"
#include "player_macro_result_zocl.h"
#include "Init_action_point_zocl.h"
#include "w_name.h"
#include "CChatStealSystem.h"
#include "CObjectList.h"
#include "pnt_rect.h"
#include "target_monster_aggro_inform_zocl.h"
#include "CNetworkEX.h"
#include "GlobalObjects.h"

#include <cstdio>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <mmsystem.h>

_skill_fld *CPlayer::ms_pXmas_Snow_Effect = nullptr;
_skill_fld *CPlayer::ms_pXmas_Snow_Bullet_Effect = nullptr;
CMgrAvatorItemHistory CPlayer::s_MgrItemHistory{};
CMgrAvatorLvHistory CPlayer::s_MgrLvHistory{};
int CPlayer::s_nLiveNum = 0;
_DELAY_PROCESS CPlayer::s_AnimusReturnDelay{};
int *CPlayer::s_pnLinkForceItemToEffect = nullptr;
_SKILL_IDX_PER_MASTERY CPlayer::s_SkillIndexPerMastery[8]{};
int CPlayer::s_nAddMstFc[100]{};
int CPlayer::s_nStdDefPoint = 0;
int CPlayer::s_nRevDefPoint = 0;
CRecordData CPlayer::s_tblLimMastery[3][4];
CRecordData CPlayer::s_tblLimMasteryContinue[3][4];
CRecordData CPlayer::s_tblLimMasteryCum[3][4];
CRecordData CPlayer::s_tblLimMasteryCumContinue[3][4];
_BILLING_FORCE_CLOSE_DELAY CPlayer::s_BillingForceCloseDelay{};
int CPlayer::s_nRaceNum[3] = {};
unsigned int CPlayer::s_dwTotalCloseCount = 0;
unsigned int CPlayer::s_dwAbnormalCloseCount = 0;
CRecordData *_WEAPON_PARAM::s_pWeaponData = nullptr;
CRecordData *_MASTERY_PARAM::s_pSkillData = nullptr;
CRecordData *_MASTERY_PARAM::s_pForceData = nullptr;
int _ATTACK_DELAY_CHECKER::s_nSpareTime = 0;
CRecordData ItemCombineMgr::ms_tbl_ItemCombine;
CRecordData ItemCombineMgr::ms_tbl_ItemCombine_Link_Stuff;
CRecordData ItemCombineMgr::ms_tbl_ItemCombine_Link_Result;

namespace
{
  const char *sType[10] = {
    "kill_s_inc",
    "kill_p_inc",
    "die_dec",
    "quest_inc",
    "logoff_inc",
    "logoff_dec",
    "holy_award",
    "holy_dec",
    "cheat",
    "guildbattle",
  };
}

bool ItemCombineMgr::LoadData()
{
  char szMessage[144] = {};
  if (ItemCombineMgr::ms_tbl_ItemCombine.ReadRecord_Ex(
        ".\\script\\CombineTable.dat",
        ".\\script\\CombineTable2.dat",
        0x3C8u,
        szMessage))
  {
    if (ItemCombineMgr::ms_tbl_ItemCombine_Link_Stuff.ReadRecord(
          ".\\script\\LinkedStuff.dat",
          0x1944u,
          szMessage))
    {
      if (ItemCombineMgr::ms_tbl_ItemCombine_Link_Result.ReadRecord(
            ".\\script\\LinkedResult.dat",
            0x1944u,
            szMessage))
      {
        return true;
      }
    }
  }

  MyMessageBox("DatafileInit", szMessage);
  return false;
}

bool ItemCombineMgr::CheckLoadData()
{
  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(ItemCombineMgr::ms_tbl_ItemCombine.GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    (void)ItemCombineMgr::ms_tbl_ItemCombine.GetRecord(n);
  }
  return true;
}

CPlayer::CPlayer() = default;

_ATTACK_DELAY_CHECKER::_eff_list::_eff_list()
{
  byEffectCode = 0xFF;
}

void _ATTACK_DELAY_CHECKER::_eff_list::init(_eff_list *self)
{
  self->byEffectCode = 0xFF;
}

bool _ATTACK_DELAY_CHECKER::_eff_list::fill(_eff_list *self)
{
  return self->byEffectCode != 0xFF;
}

_ATTACK_DELAY_CHECKER::_mas_list::_mas_list()
{
  byEffectCode = 0xFF;
}

void _ATTACK_DELAY_CHECKER::_mas_list::init(_mas_list *self)
{
  self->byEffectCode = 0xFF;
}

bool _ATTACK_DELAY_CHECKER::_mas_list::fill(_mas_list *self)
{
  return self->byEffectCode != 0xFF;
}

_ATTACK_DELAY_CHECKER::_ATTACK_DELAY_CHECKER()
{
  for (int index = 0; index < 10; ++index)
  {
    _eff_list::init(&EFF[index]);
    _mas_list::init(&MAS[index]);
  }
  Init(this);
}

void _ATTACK_DELAY_CHECKER::Init(_ATTACK_DELAY_CHECKER *self)
{
  for (int index = 0; index < 10; ++index)
  {
    _eff_list::init(&self->EFF[index]);
    _mas_list::init(&self->MAS[index]);
  }
  const DWORD now = timeGetTime();
  self->dwNextEffTime = now;
  self->dwNextGenTime = now;
  self->dwLastGnAttackTime = now;
  self->dwLastSFAttackTime = now;
  self->nFailCount = 0;
  self->m_nNextAddTime = 0;
}

char _ATTACK_DELAY_CHECKER::_delay_check(
  _ATTACK_DELAY_CHECKER *self,
  unsigned __int8 code,
  unsigned __int16 index,
  unsigned __int8 mastery)
{
  const DWORD now = timeGetTime();
  self->byTemp_EffectCode = code;
  self->wTemp_EffectIndex = index;
  self->byTemp_EffectMastery = mastery;

  if (code == 0xFF)
  {
    const unsigned int nextTime = self->dwLastSFAttackTime + 1000;
    int diff = static_cast<int>(now - nextTime);
    if (diff < 0)
    {
      if (-diff > _ATTACK_DELAY_CHECKER::s_nSpareTime)
      {
        return 0;
      }
      self->m_nNextAddTime = -diff;
    }

    diff = static_cast<int>(now - self->dwNextGenTime);
    if (diff < 0)
    {
      if (-diff > _ATTACK_DELAY_CHECKER::s_nSpareTime)
      {
        return 0;
      }
      if (self->m_nNextAddTime)
      {
        if (-diff > self->m_nNextAddTime)
        {
          self->m_nNextAddTime = -diff;
        }
      }
      else
      {
        self->m_nNextAddTime = -diff;
      }
    }
    return 1;
  }

  const unsigned int nextTime = self->dwLastGnAttackTime + 1000;
  int diff = static_cast<int>(now - nextTime);
  if (diff < 0)
  {
    if (-diff > _ATTACK_DELAY_CHECKER::s_nSpareTime)
    {
      return 0;
    }
    self->m_nNextAddTime = -diff;
  }

  diff = static_cast<int>(now - self->dwNextEffTime);
  if (diff < 0)
  {
    if (-diff > _ATTACK_DELAY_CHECKER::s_nSpareTime)
    {
      return 0;
    }
    if (self->m_nNextAddTime)
    {
      if (-diff > self->m_nNextAddTime)
      {
        self->m_nNextAddTime = -diff;
      }
    }
    else
    {
      self->m_nNextAddTime = -diff;
    }
  }

  for (int indexList = 0; indexList < 10; ++indexList)
  {
    if (_mas_list::fill(&self->MAS[indexList])
      && self->MAS[indexList].byEffectCode == code
      && self->MAS[indexList].byMastery == mastery)
    {
      diff = static_cast<int>(now - self->MAS[indexList].dwNextTime);
      if (diff < 0)
      {
        if (-diff > _ATTACK_DELAY_CHECKER::s_nSpareTime)
        {
          return 0;
        }
        if (self->m_nNextAddTime)
        {
          if (-diff > self->m_nNextAddTime)
          {
            self->m_nNextAddTime = -diff;
          }
        }
        else
        {
          self->m_nNextAddTime = -diff;
        }
      }
      break;
    }
  }

  for (int indexList = 0; indexList < 10; ++indexList)
  {
    if (_eff_list::fill(&self->EFF[indexList])
      && self->EFF[indexList].byEffectCode == code
      && self->EFF[indexList].wEffectIndex == index)
    {
      diff = static_cast<int>(now - self->EFF[indexList].dwNextTime);
      if (diff < 0)
      {
        if (-diff > _ATTACK_DELAY_CHECKER::s_nSpareTime)
        {
          return 0;
        }
        if (self->m_nNextAddTime)
        {
          if (-diff > self->m_nNextAddTime)
          {
            self->m_nNextAddTime = -diff;
          }
        }
        else
        {
          self->m_nNextAddTime = -diff;
        }
      }
      return 1;
    }
  }

  return 1;
}

char _ATTACK_DELAY_CHECKER::IsDelay(
  _ATTACK_DELAY_CHECKER *self,
  unsigned __int8 code,
  unsigned __int16 index,
  unsigned __int8 mastery)
{
  self->m_nNextAddTime = 0;
  if (_delay_check(self, code, index, mastery))
  {
    return 1;
  }
  ++self->nFailCount;
  return 0;
}

void _ATTACK_DELAY_CHECKER::SetDelay(_ATTACK_DELAY_CHECKER *self, unsigned int delay)
{
  const unsigned int now = self->m_nNextAddTime + timeGetTime();
  if (self->byTemp_EffectCode == 0xFF)
  {
    self->dwNextGenTime = delay + now;
    self->dwLastGnAttackTime = now;
    return;
  }

  int foundIndex = -1;
  for (int index = 0; index < 10; ++index)
  {
    if (_eff_list::fill(&self->EFF[index]))
    {
      if (self->EFF[index].byEffectCode == self->byTemp_EffectCode
        && self->EFF[index].wEffectIndex == self->wTemp_EffectIndex)
      {
        self->EFF[index].dwNextTime = delay + now;
        foundIndex = -1;
        break;
      }
      if (self->EFF[index].dwNextTime < now)
      {
        _eff_list::init(&self->EFF[index]);
        foundIndex = index;
      }
    }
    else
    {
      foundIndex = index;
    }
  }

  if (foundIndex != -1)
  {
    self->EFF[foundIndex].byEffectCode = self->byTemp_EffectCode;
    self->EFF[foundIndex].wEffectIndex = self->wTemp_EffectIndex;
    self->EFF[foundIndex].dwNextTime = delay + now;
  }

  if (self->byTemp_EffectCode != 2 && self->byTemp_EffectMastery != 0xFF)
  {
    foundIndex = -1;
    for (int index = 0; index < 10; ++index)
    {
      if (_mas_list::fill(&self->MAS[index]))
      {
        if (self->MAS[index].byEffectCode == self->byTemp_EffectCode
          && self->MAS[index].byMastery == self->byTemp_EffectMastery)
        {
          self->MAS[index].dwNextTime = delay + now;
          foundIndex = -1;
          break;
        }
        if (self->MAS[index].dwNextTime < now)
        {
          _mas_list::init(&self->MAS[index]);
          foundIndex = index;
        }
      }
      else
      {
        foundIndex = index;
      }
    }

    if (foundIndex != -1)
    {
      self->MAS[foundIndex].byEffectCode = self->byTemp_EffectCode;
      self->MAS[foundIndex].byMastery = self->byTemp_EffectMastery;
      self->MAS[foundIndex].dwNextTime = delay + now;
    }
  }

  self->dwNextEffTime = now + 1000;
  self->dwLastSFAttackTime = now;
}

void _TOWER_PARAM::_list::init()
{
  m_pTowerItem = nullptr;
  m_pTowerObj = nullptr;
}

void _TOWER_PARAM::Init()
{
  for (int j = 0; j < 6; ++j)
  {
    m_List[j].init();
  }
  m_nCount = 0;
}

bool _TRAP_PARAM::_param::isLoad()
{
  return pItem != nullptr;
}

_UNIT_DB_BASE::_LIST::_LIST()
{
  Init(0xFFu);
}

void _UNIT_DB_BASE::_LIST::Init(unsigned __int8 byIndex)
{
  if (byIndex != 0xFFu)
  {
    bySlotIndex = byIndex;
  }
  byFrame = static_cast<unsigned __int8>(-1);
  memset_0(byPart, 0xFF, sizeof(byPart));
  memset_0(dwBullet, -1, sizeof(dwBullet));
  memset_0(dwSpare, -1, sizeof(dwSpare));
  dwGauge = 0;
  nPullingFee = 0;
  dwCutTime = 0;
  wBooster = 0;
}

void _UNIT_DB_BASE::_LIST::DelUnit()
{
  byFrame = static_cast<unsigned __int8>(-1);
  memset_0(byPart, 0xFF, sizeof(byPart));
  memset_0(dwBullet, -1, sizeof(dwBullet));
  memset_0(dwSpare, -1, sizeof(dwSpare));
  dwGauge = 0;
  nPullingFee = 0;
  dwCutTime = 0;
  wBooster = 0;
}

_UNIT_DB_BASE::_UNIT_DB_BASE()
{
  Init();
}

void _UNIT_DB_BASE::Init()
{
  for (int j = 0; j < 4; ++j)
  {
    m_List[j].Init(static_cast<unsigned __int8>(j));
  }
}

_QUEST_DB_BASE::_LIST::_LIST()
{
  Init();
}

void _QUEST_DB_BASE::_LIST::Init()
{
  byQuestType = static_cast<unsigned __int8>(-1);
  wIndex = static_cast<unsigned __int16>(-1);
  for (int j = 0; j < 3; ++j)
  {
    wNum[j] = static_cast<unsigned __int16>(-1);
  }
  dwPassSec = 0;
}

_QUEST_DB_BASE::_NPC_QUEST_HISTORY::_NPC_QUEST_HISTORY()
{
  Init();
}

void _QUEST_DB_BASE::_NPC_QUEST_HISTORY::Init()
{
  memset_0(this, 0, 8u);
  byLevel = static_cast<unsigned __int8>(-1);
}

_QUEST_DB_BASE::_START_NPC_QUEST_HISTORY::_START_NPC_QUEST_HISTORY()
{
  Init();
}

void _QUEST_DB_BASE::_START_NPC_QUEST_HISTORY::Init()
{
  strcpy_0(szQuestCode, "*");
  byLevel = static_cast<unsigned __int8>(-1);
  nEndTime = 0;
  memset_0(&tmStartTime, 0, sizeof(tmStartTime));
}

_QUEST_DB_BASE::_QUEST_DB_BASE()
{
  Init();
}

void _QUEST_DB_BASE::Init()
{
  for (int j = 0; j < 30; ++j)
  {
    m_List[j].Init();
  }
}

void _NPCQuestIndexTempData::Init()
{
  nQuestNum = 0;
  memset_0(IndexData, 0, sizeof(IndexData));
}

void _happen_event_cont::init()
{
  memset_0(this, 0, sizeof(_happen_event_cont));
}

bool _happen_event_cont::isset() const
{
  return m_pEvent != nullptr;
}

void _COMBINEKEY::SetRelease()
{
  byRewardIndex = static_cast<unsigned __int8>(-1);
  byTableCode = static_cast<unsigned __int8>(-1);
  wItemIndex = static_cast<unsigned __int16>(-1);
}

bool _COMBINEKEY::IsFilled()
{
  return byRewardIndex != 0xFFu || byTableCode != 0xFFu || wItemIndex != 0xFFFFu;
}

void _COMBINEKEY::LoadDBKey(_COMBINEKEY pl_nKey)
{
  *this = pl_nKey;
}

__int64 _COMBINEKEY::CovDBKey()
{
  int key = 0;
  memcpy_0(&key, this, sizeof(key));
  return static_cast<__int64>(key);
}

_ITEMCOMBINE_DB_BASE::_LIST::_LIST()
{
  Init();
}

void _ITEMCOMBINE_DB_BASE::_LIST::Init()
{
  Key.SetRelease();
  dwDur = 0;
  dwUpt = 0xFFFFFFF;
}

_ITEMCOMBINE_DB_BASE::_ITEMCOMBINE_DB_BASE()
{
  Init();
}

void _ITEMCOMBINE_DB_BASE::Init()
{
  m_bIsResult = false;
  m_byItemListNum = 0;
  m_dwDalant = 0;
  m_dwCheckKey = static_cast<unsigned int>(-1);
  m_bySelectItemCount = 0;
  for (int j = 0; j < 24; ++j)
  {
    m_List[j].Init();
  }
}

bool _ITEMCOMBINE_DB_BASE::IsCombineData()
{
  return m_bIsResult && m_dwCheckKey != static_cast<unsigned int>(-1);
}

_quick_link::_quick_link()
{
  init();
}

void _quick_link::init()
{
  byLinkIndex = static_cast<unsigned __int8>(-1);
}

bool LoadMasteryLimFile(char *pszErrMsg)
{
  const char *files[4] = {
    ".\\script\\WarriorMasteryLimit.dat",
    ".\\script\\RangerMasteryLimit.dat",
    ".\\script\\SpiritualistMasteryLimit.dat",
    ".\\script\\SpecialistMasteryLimit.dat",
  };

  for (int classIndex = 0; classIndex < 4; ++classIndex)
  {
    for (int masteryIndex = 0; masteryIndex < 3; ++masteryIndex)
    {
      if (!CPlayer::s_tblLimMastery[masteryIndex][classIndex].ReadRecord(
            files[classIndex], 0xE8u, pszErrMsg))
      {
        return false;
      }
      if (!CPlayer::s_tblLimMasteryContinue[masteryIndex][classIndex].ReadRecord(
            files[classIndex], 0xE8u, pszErrMsg))
      {
        return false;
      }
      if (!CPlayer::s_tblLimMasteryCum[masteryIndex][classIndex].ReadRecord(
            files[classIndex], 0xE8u, pszErrMsg))
      {
        return false;
      }
      if (!CPlayer::s_tblLimMasteryCumContinue[masteryIndex][classIndex].ReadRecord(
            files[classIndex], 0xE8u, pszErrMsg))
      {
        return false;
      }

      const float scale = 1.1f;
      for (int recordIndex = 0; ; ++recordIndex)
      {
        const int recordCount =
          CPlayer::s_tblLimMasteryContinue[masteryIndex][classIndex].GetRecordNum();
        if (recordIndex >= recordCount)
        {
          break;
        }
        int *record = reinterpret_cast<int *>(
          CPlayer::s_tblLimMasteryContinue[masteryIndex][classIndex].GetRecord(recordIndex));
        for (int j = 0; j < 2; ++j)
        {
          const float scaled = static_cast<float>(record[j + 18]) * scale;
          record[j + 18] = static_cast<int>(scaled >= 99.0f ? 99.0f : scaled);
        }
        {
          const float scaled = static_cast<float>(record[21]) * scale;
          record[21] = static_cast<int>(scaled >= 99.0f ? 99.0f : scaled);
        }
        {
          const float scaled = static_cast<float>(record[22]) * scale;
          record[22] = static_cast<int>(scaled >= 99.0f ? 99.0f : scaled);
        }
        for (int j = 0; j < 24; ++j)
        {
          const float scaled = static_cast<float>(record[j + 34]) * scale;
          record[j + 34] = static_cast<int>(scaled >= 99.0f ? 99.0f : scaled);
        }
        for (int j = 0; j < 8; ++j)
        {
          const float scaled = static_cast<float>(record[j + 26]) * scale;
          record[j + 26] = static_cast<int>(scaled >= 99.0f ? 99.0f : scaled);
        }
        for (int j = 0; j < 3; ++j)
        {
          const float scaled = static_cast<float>(record[j + 23]) * scale;
          record[j + 23] = static_cast<int>(scaled >= 99.0f ? 99.0f : scaled);
        }
        {
          const float scaled = static_cast<float>(record[20]) * scale;
          record[20] = static_cast<int>(scaled >= 99.0f ? 99.0f : scaled);
        }

        int *cumRecord = reinterpret_cast<int *>(
          CPlayer::s_tblLimMasteryCumContinue[masteryIndex][classIndex].GetRecord(recordIndex));
        for (int j = 0; j < 2; ++j)
        {
          const float scaled = static_cast<float>(cumRecord[j + 18]) * scale;
          cumRecord[j + 18] = static_cast<int>(scaled >= 99.0f ? 99.0f : scaled);
        }
        {
          const float scaled = static_cast<float>(cumRecord[21]) * scale;
          cumRecord[21] = static_cast<int>(scaled >= 99.0f ? 99.0f : scaled);
        }
        {
          const float scaled = static_cast<float>(cumRecord[22]) * scale;
          cumRecord[22] = static_cast<int>(scaled >= 99.0f ? 99.0f : scaled);
        }
        for (int j = 0; j < 24; ++j)
        {
          const float scaled = static_cast<float>(cumRecord[j + 34]) * scale;
          cumRecord[j + 34] = static_cast<int>(scaled >= 99.0f ? 99.0f : scaled);
        }
        for (int j = 0; j < 8; ++j)
        {
          const float scaled = static_cast<float>(cumRecord[j + 26]) * scale;
          cumRecord[j + 26] = static_cast<int>(scaled >= 99.0f ? 99.0f : scaled);
        }
        for (int j = 0; j < 3; ++j)
        {
          const float scaled = static_cast<float>(cumRecord[j + 23]) * scale;
          cumRecord[j + 23] = static_cast<int>(scaled >= 99.0f ? 99.0f : scaled);
        }
        {
          const float scaled = static_cast<float>(cumRecord[20]) * scale;
          cumRecord[20] = static_cast<int>(scaled >= 99.0f ? 99.0f : scaled);
        }
      }

      for (int recordIndex = 0; ; ++recordIndex)
      {
        const int recordCount =
          CPlayer::s_tblLimMasteryCum[masteryIndex][classIndex].GetRecordNum();
        if (recordIndex >= recordCount)
        {
          break;
        }
        int *record = reinterpret_cast<int *>(
          CPlayer::s_tblLimMasteryCum[masteryIndex][classIndex].GetRecord(recordIndex));
        for (int j = 0; j < 2; ++j)
        {
          const float pow2 = std::pow(static_cast<float>(record[j + 18]), 2.0f);
          const float sqrtVal = std::sqrt((1000.0f * 1000.0f) + (4.0f * pow2 * 1000.0f));
          const float calc = std::pow((sqrtVal + -1000.0f) / 2.0f, 2.0f);
          record[j + 18] = static_cast<int>(calc);
        }

        {
          const float pow2 = std::pow(static_cast<float>(record[21]), 2.0f);
          const float sqrtVal = std::sqrt((1000.0f * 1000.0f) + (4.0f * pow2 * 1000.0f));
          const float calc = std::pow((sqrtVal + -1000.0f) / 2.0f, 2.0f);
          record[21] = static_cast<int>(calc);
        }
        {
          const float pow2 = std::pow(static_cast<float>(record[22]), 2.0f);
          const float sqrtVal = std::sqrt((100.0f * 100.0f) + (4.0f * pow2 * 100.0f));
          const float calc = std::pow((sqrtVal + -100.0f) / 2.0f, 2.0f);
          record[22] = static_cast<int>(calc);
        }

        for (int j = 0; j < 24; ++j)
        {
          const float pow2 = std::pow(static_cast<float>(record[j + 34]), 2.0f);
          const float pow4 = std::pow(pow2, 2.0f);
          const int rounded = CalcRoundUp(pow4 / 14.0f);
          record[j + 34] = static_cast<int>((static_cast<float>(rounded - 1) + 0.0099999998f));
        }
        for (int j = 0; j < 8; ++j)
        {
          const float pow2 = std::pow(static_cast<float>(record[j + 26]), 2.0f);
          const float pow4 = std::pow(pow2, 2.0f);
          const int rounded = CalcRoundUp(pow4 / 10.0f);
          record[j + 26] = static_cast<int>((static_cast<float>(rounded - 1) + 0.0099999998f));
        }
        {
          const float pow2 = std::pow(static_cast<float>(record[23]), 2.0f);
          record[23] = static_cast<int>(((pow2 - 1.0f) / 3.0f) * 1.1f + 0.89999998f);
        }
        {
          const float pow2 = std::pow(static_cast<float>(record[24]), 2.0f);
          record[24] = static_cast<int>(((pow2 - 1.0f) / 3.0f) * 1.1f + 0.89999998f);
        }
        {
          const float pow2 = std::pow(static_cast<float>(record[25]), 2.0f);
          record[25] = static_cast<int>(((pow2 - 1.0f) / 3.0f) * 10.0f + 0.89999998f);
        }

        if (masteryIndex)
        {
          if (masteryIndex == 1)
          {
            const float pow2 = std::pow(static_cast<float>(record[20] - 1), 2.0f);
            record[20] = static_cast<int>(pow2 * 15000.0f);
          }
          else if (masteryIndex == 2)
          {
            const float pow2 = std::pow(static_cast<float>(record[20]), 2.0f);
            const float sqrtVal = std::sqrt((1000.0f * 1000.0f) + (4.0f * pow2 * 1000.0f));
            const float calc = std::pow((sqrtVal + -1000.0f) / 2.0f, 2.0f);
            record[20] = static_cast<int>(calc);
          }
        }
        else
        {
          const float pow2 = std::pow(static_cast<float>(record[20] - 1), 2.0f);
          record[20] = static_cast<int>(pow2 * 15000.0f);
        }

        int *cumRecord = reinterpret_cast<int *>(
          CPlayer::s_tblLimMasteryCumContinue[masteryIndex][classIndex].GetRecord(recordIndex));
        for (int j = 0; j < 2; ++j)
        {
          const float pow2 = std::pow(static_cast<float>(cumRecord[j + 18]), 2.0f);
          const float sqrtVal = std::sqrt((1000.0f * 1000.0f) + (4.0f * pow2 * 1000.0f));
          const float calc = std::pow((sqrtVal + -1000.0f) / 2.0f, 2.0f);
          cumRecord[j + 18] = static_cast<int>(calc);
        }
        {
          const float pow2 = std::pow(static_cast<float>(cumRecord[21]), 2.0f);
          const float sqrtVal = std::sqrt((1000.0f * 1000.0f) + (4.0f * pow2 * 1000.0f));
          const float calc = std::pow((sqrtVal + -1000.0f) / 2.0f, 2.0f);
          cumRecord[21] = static_cast<int>(calc);
        }
        {
          const float pow2 = std::pow(static_cast<float>(cumRecord[22]), 2.0f);
          const float sqrtVal =
            std::sqrt((100.0f * 100.0f) + (4.0f * pow2 * 100.0f)) + -100.0f;
          const float calc = std::pow(sqrtVal / 2.0f, 2.0f);
          cumRecord[22] = static_cast<int>(calc);
        }
        for (int j = 0; j < 24; ++j)
        {
          const float pow2 = std::pow(static_cast<float>(cumRecord[j + 34]), 2.0f);
          const float pow4 = std::pow(pow2, 2.0f);
          const int rounded = CalcRoundUp(pow4 / 14.0f);
          cumRecord[j + 34] = static_cast<int>((static_cast<float>(rounded - 1) + 0.0099999998f));
        }
        for (int j = 0; j < 8; ++j)
        {
          const float pow2 = std::pow(static_cast<float>(cumRecord[j + 26]), 2.0f);
          const float pow4 = std::pow(pow2, 2.0f);
          const int rounded = CalcRoundUp(pow4 / 10.0f);
          cumRecord[j + 26] = static_cast<int>((static_cast<float>(rounded - 1) + 0.0099999998f));
        }
        {
          const float pow2 = std::pow(static_cast<float>(cumRecord[23]), 2.0f);
          cumRecord[23] = static_cast<int>(((pow2 - 1.0f) / 3.0f) * 1.1f + 0.89999998f);
        }
        {
          const float pow2 = std::pow(static_cast<float>(cumRecord[24]), 2.0f);
          cumRecord[24] = static_cast<int>(((pow2 - 1.0f) / 3.0f) * 1.1f + 0.89999998f);
        }
        {
          const float pow2 = std::pow(static_cast<float>(cumRecord[25]), 2.0f);
          cumRecord[25] = static_cast<int>(((pow2 - 1.0f) / 3.0f) * 10.0f + 0.89999998f);
        }

        if (static_cast<unsigned int>(masteryIndex) > 1)
        {
          if (masteryIndex == 2)
          {
            const float pow2 = std::pow(static_cast<float>(cumRecord[20]), 2.0f);
            const float sqrtVal =
              std::sqrt((1000.0f * 1000.0f) + (4.0f * pow2 * 1000.0f)) + -1000.0f;
            const float calc = std::pow(sqrtVal / 2.0f, 2.0f);
            cumRecord[20] = static_cast<int>(calc);
          }
        }
        else
        {
          const float pow2 = std::pow(static_cast<float>(cumRecord[20] - 1), 2.0f);
          cumRecord[20] = static_cast<int>(pow2 * 15000.0f);
        }
      }
    }
  }

  return true;
}

void CPlayer::SetStaticMember()
{
  unsigned __int64 recordNum = g_Main.m_tblItemData[15].GetRecordNum();
  s_pnLinkForceItemToEffect = static_cast<int *>(operator new[](saturated_mul(recordNum, 4uLL)));
  memset_0(s_pnLinkForceItemToEffect, -1, 8uLL);

  for (int n = 0; ; ++n)
  {
    const int recordCount = g_Main.m_tblItemData[15].GetRecordNum();
    if (n >= recordCount)
    {
      break;
    }
    _base_fld *record = g_Main.m_tblItemData[15].GetRecord(n);
    if (!record)
    {
      g_Main.m_logSystemError.Write("CPlayer::SetStaticMember() : %d force..NULL", n);
      break;
    }
    _base_fld *effectRecord = g_Main.m_tblEffectData[1].GetRecord(&record[4].m_strCode[28]);
    if (!effectRecord)
    {
      g_Main.m_logSystemError.Write("CPlayer::SetStaticMember() : %s force..NULL", &record[4].m_strCode[28]);
      break;
    }
    s_pnLinkForceItemToEffect[n] = effectRecord->m_dwIndex;
  }

  for (int n = 0; ; ++n)
  {
    const int recordCount = g_Main.m_tblEffectData[0].GetRecordNum();
    if (n >= recordCount)
    {
      break;
    }
    _base_fld *record = g_Main.m_tblEffectData[0].GetRecord(n);
    const unsigned int masteryIndex = *reinterpret_cast<unsigned int *>(&record[1].m_strCode[4]);
    if (record[1].m_dwIndex <= 1 && masteryIndex < 8u && record[1].m_dwIndex != 2)
    {
      _SKILL_IDX_PER_MASTERY *skillInfo = &CPlayer::s_SkillIndexPerMastery[masteryIndex];
      skillInfo->m_nSkillIndex[skillInfo->m_nSkillIndexNum++] = n;
    }
  }

  memset_0(CPlayer::s_nAddMstFc, 1, sizeof(CPlayer::s_nAddMstFc));
  for (int n = 0; n < 100; ++n)
  {
    const float powVal = std::pow(static_cast<float>(n), 3.0f);
    const float sqrtVal = std::sqrt(powVal * 2.0f);
    CPlayer::s_nAddMstFc[n] = static_cast<int>(sqrtVal);
  }

  char errorMessage[132]{};
  if (!LoadMasteryLimFile(errorMessage))
  {
    MyMessageBox("CPlayer::SetStaticMember()", errorMessage);
    ServerProgramExit("MasteryLimData Load Error", 0);
  }

  if (!g_Main.IsReleaseServiceMode())
  {
    const UINT stdDefPoint = GetPrivateProfileIntA(
      "Formula",
      "StdDefPoint",
      0x7FFFFFFF,
      ".\\Initialize\\WorldSystem.ini");
    if (stdDefPoint == 0x7FFFFFFF)
    {
      char buffer[132]{};
      _itoa(s_nStdDefPoint, buffer, 10);
      WritePrivateProfileStringA("Formula", "StdDefPoint", buffer, ".\\Initialize\\WorldSystem.ini");
    }
    else
    {
      s_nStdDefPoint = stdDefPoint;
    }

    const UINT revDefPoint = GetPrivateProfileIntA(
      "Formula",
      "RcvDefPoint",
      0x7FFFFFFF,
      ".\\Initialize\\WorldSystem.ini");
    if (revDefPoint == 0x7FFFFFFF)
    {
      char buffer[144]{};
      _itoa(s_nRevDefPoint, buffer, 10);
      WritePrivateProfileStringA("Formula", "RcvDefPoint", buffer, ".\\Initialize\\WorldSystem.ini");
    }
    else
    {
      s_nRevDefPoint = revDefPoint;
    }
  }

  CNationSettingManager *nationSetting = CTSingleton<CNationSettingManager>::Instance();
  const unsigned __int16 delay = nationSetting->GetBillingForceCloseDelay();
  CPlayer::s_BillingForceCloseDelay.Init(0x9E4u, 1000 * delay);
}

void CPlayer::Init(_object_id *pID)
{
  CCharacter::Init(pID);
  m_EP.AllocEffParam();
  m_tmrIntervalSec.BeginTimer(0x3E8);
  m_tmrBilling.BeginTimer(0x3A980);
  m_pPartyMgr = &g_PartyPlayer[static_cast<unsigned __int64>(pID->m_wIndex)];
  m_bLoad = false;
  m_bOper = false;
  m_bPostLoad = false;
  m_bFullMode = false;
  m_byPosRaceTown = static_cast<unsigned __int8>(-1);
  m_bCheat_100SuccMake = false;
  m_bCheat_makeitem_no_use_matrial = false;
  m_bCheat_Matchless = false;
  m_bFreeRecallWaitTime = false;
  m_pUserDB = nullptr;
  m_pUsingUnit = nullptr;
  m_pParkingUnit = nullptr;
  m_dwUnitViewOverTime = static_cast<unsigned int>(-1);
  m_pRecalledAnimusItem = nullptr;
  m_pRecalledAnimusChar = nullptr;
  m_dwLastRecallTime = 0;
  m_byNextRecallReturn = static_cast<unsigned __int8>(-1);
  m_id.dwSerial = static_cast<unsigned int>(-1);
  m_id.wIndex = pID->m_wIndex;
  m_dwObjSerial = static_cast<unsigned int>(-1);
  m_byUserDgr = 0;
  m_pBindMapData = nullptr;
  m_pBindDummyData = nullptr;
  m_dwNextTimeDungeonDie = 0;
  m_bNeverDie = false;
  m_bRecvMapChat = false;
  PastWhisperInit();
  m_clsSetItem.Init_Info();
  m_nChaosMode = 0;
  m_dwChaosModeTime10Per = 0;
  m_dwChaosModeEndTime = 0;
  m_MinigTicket.Init();
  m_bSnowMan = false;
  m_tmrGroupTargeting.BeginTimer(0x2710);
  m_bAfterEffect = false;
  m_bSFDelayNotCheck = false;
  m_tmrEffectStartTime.BeginTimer(0x36EE80);
  m_tmrEffectEndTime.BeginTimer(0xEA60);
  m_kMoveDelayChecker.Init(0x0A);
  m_NameChangeBuddyInfo.Init();
  m_dwPcBangGiveItemListIndex = static_cast<unsigned int>(-1);
  m_tmrAccumPlayingTime.BeginTimer(0x493E0);
  m_tmrPremiumPVPInform.BeginTimer(0x1B7740);
}

CPlayer::~CPlayer() = default;

void CPlayer::PastWhisperInit()
{
  for (int j = 0; j < 10; ++j)
  {
    m_PastWhiper[j].bMemory = false;
  }
}

void _WEAPON_PARAM::SetStaticMember(CRecordData *itemTable)
{
  s_pWeaponData = itemTable;
}

void _WEAPON_PARAM::Init()
{
  nGaMaxAF = 2;
  nGaMinAF = 1;
  nMaMaxAF = 2;
  nMaMinAF = 1;
  byGaMinSel = 35;
  byGaMaxSel = 60;
  byMaMinSel = 35;
  byMaMaxSel = 60;
  byAttTolType = static_cast<unsigned __int8>(-1);
  byWpClass = 0;
  byWpType = 0;
  pFixWp = nullptr;
  pFixUnit = nullptr;
  wGaAttRange = 40;
  nActiveType = 0;
  nActiveEffLvl = 0;
  nActiveProb = 0;
  memset_0(strActiveCode_key, 0, sizeof(strActiveCode_key));
  memset_0(strEffBulletType, 0, sizeof(strEffBulletType));
}

void _WEAPON_PARAM::FixWeapon(_STORAGE_LIST::_db_con *pWeapon)
{
  if (!pWeapon)
  {
    Init();
    return;
  }
  if (pWeapon->m_byTableCode != 6)
  {
    return;
  }

  _base_fld *record = s_pWeaponData ? s_pWeaponData->GetRecord(pWeapon->m_wItemIndex) : nullptr;
  if (!record)
  {
    Init();
    return;
  }

  if (*reinterpret_cast<int *>(&record[6].m_strCode[8]) == 10)
  {
    Init();
    return;
  }

  byAttTolType = static_cast<unsigned __int8>(GetWeaponTolType(pWeapon));
  byWpClass = GetWeaponClass(pWeapon->m_wItemIndex);
  nGaMaxAF = static_cast<int>(*reinterpret_cast<float *>(record[10].m_strCode));
  nGaMinAF = static_cast<int>(*reinterpret_cast<float *>(&record[9].m_strCode[60]));
  nMaMaxAF = static_cast<int>(*reinterpret_cast<float *>(&record[10].m_strCode[24]));
  nMaMinAF = static_cast<int>(*reinterpret_cast<float *>(&record[10].m_strCode[16]));
  byGaMinSel = record[9].m_strCode[56];
  byGaMaxSel = record[10].m_dwIndex;
  byMaMinSel = record[10].m_strCode[12];
  byMaMaxSel = record[10].m_strCode[20];
  byWpType = record[6].m_strCode[8];
  wGaAttRange = static_cast<unsigned __int16>(static_cast<int>(*reinterpret_cast<float *>(&record[9].m_strCode[48]) + 40.0f));
  nActiveType = *reinterpret_cast<int *>(&record[3].m_strCode[4]);
  nActiveEffLvl = *reinterpret_cast<int *>(&record[4].m_strCode[4]);
  nActiveProb = static_cast<int>(*reinterpret_cast<float *>(&record[4].m_strCode[8]) * 100.0f);
  strncpy(strActiveCode_key, &record[3].m_strCode[8], 0x40u);
  strncpy(strEffBulletType, &record[5].m_strCode[12], 0x40u);
  pFixWp = pWeapon;
  pFixUnit = nullptr;
}

unsigned int _WEAPON_PARAM::GetWeaponTolType(_STORAGE_LIST::_db_con *pItem)
{
  if (!pItem)
  {
    return 0;
  }

  for (int j = 0; j < 7; ++j)
  {
    const unsigned __int8 talik = GetTalikFromSocket(pItem->m_dwLv, static_cast<unsigned __int8>(j));
    if (talik == 15)
    {
      break;
    }
    switch (talik)
    {
      case 7:
        return 0;
      case 8:
        return 1;
      case 9:
        return 2;
      case 10:
        return 3;
      default:
        break;
    }
  }

  _base_fld *record = s_pWeaponData ? s_pWeaponData->GetRecord(pItem->m_wItemIndex) : nullptr;
  if (!record)
  {
    return 0;
  }
  return *reinterpret_cast<unsigned int *>(&record[9].m_strCode[20]);
}

unsigned int _WEAPON_PARAM::GetAttackDelay(int nLv, int nAddDelay)
{
  return static_cast<unsigned int>(nAddDelay + 1000 * (nLv / nLv));
}

unsigned int _WEAPON_PARAM::GetAttackToolType()
{
  if (pFixWp)
  {
    return 1;
  }
  if (pFixUnit)
  {
    return 2;
  }
  return 0;
}

CPlayer::CashChangeStateFlag::CashChangeStateFlag(char cashrename)
{
  const unsigned __int8 current = m_byStateFlag;
  m_byStateFlag = static_cast<unsigned __int8>((cashrename & 7) | (current & 0xF8));
}

void _MASTERY_PARAM::SetStaticMember(CRecordData *pSkillData, CRecordData *pForceData)
{
  s_pSkillData = pSkillData;
  s_pForceData = pForceData;
  InitMasteryFormula(pSkillData, pForceData);
}

void _mastery_up_data::init()
{
  bUpdate = false;
}

void _mastery_up_data::set(unsigned __int8 code, unsigned __int8 index, unsigned __int8 mastery)
{
  bUpdate = true;
  byCode = code;
  byIndex = index;
  byMastery = mastery;
}

void _skill_lv_up_data::init()
{
  bUpdate = false;
}

void _skill_lv_up_data::set(unsigned __int16 index, unsigned __int8 lv)
{
  bUpdate = true;
  wIndex = index;
  byLv = lv;
}

unsigned __int8 _MASTERY_PARAM::GetMasteryPerMast(unsigned __int8 byCode, unsigned __int8 byMast)
{
  return m_ppbyMasteryPtr[byCode][byMast];
}

unsigned int _MASTERY_PARAM::GetCumPerMast(unsigned __int8 byCode, unsigned __int8 byMast)
{
  return m_ppdwMasteryCumPtr[byCode][byMast];
}

unsigned __int8 _MASTERY_PARAM::GetSkillLv(unsigned __int8 bySkillIndex)
{
  return m_lvSkill[bySkillIndex];
}

unsigned __int8 _MASTERY_PARAM::GetEquipMastery(int nEquipMasteryCode)
{
  return *m_ppbyEquipMasteryPrt[nEquipMasteryCode];
}

void _MASTERY_PARAM::UpdateCumPerMast(unsigned __int8 byClass, unsigned __int8 byIndex, unsigned int dwNewCum)
{
  m_MastUpData.init();
  m_SkillUpData.init();

  switch (byClass)
  {
    case 0:
    {
      m_BaseCum.m_dwDamWpCnt[byIndex] = dwNewCum;
      if (m_mtyWp[byIndex] <= 0x63u)
      {
        const unsigned __int8 mastery =
          static_cast<unsigned __int8>(CalcMastery(0, byIndex, m_BaseCum.m_dwDamWpCnt[byIndex], m_byRaceCode));
        m_mtyWp[byIndex] = mastery;
        m_MastUpData.set(0, byIndex, mastery);
      }
      break;
    }
    case 1:
    {
      m_BaseCum.m_dwDefenceCnt = dwNewCum;
      if (m_mtySuffer <= 0x63u)
      {
        const unsigned __int8 mastery =
          static_cast<unsigned __int8>(CalcMastery(1, 0, m_BaseCum.m_dwDefenceCnt, m_byRaceCode));
        m_mtySuffer = mastery;
        m_MastUpData.set(1u, 0, mastery);
      }
      break;
    }
    case 2:
    {
      m_BaseCum.m_dwShieldCnt = dwNewCum;
      if (m_mtyShield <= 0x63u)
      {
        const unsigned __int8 mastery =
          static_cast<unsigned __int8>(CalcMastery(2, 0, m_BaseCum.m_dwShieldCnt, m_byRaceCode));
        m_mtyShield = mastery;
        m_MastUpData.set(2u, 0, mastery);
      }
      break;
    }
    case 3:
    {
      m_BaseCum.m_dwSkillCum[byIndex] = dwNewCum;
      _base_fld *record = s_pSkillData->GetRecord(byIndex);
      const unsigned __int8 skillLevel =
        static_cast<unsigned __int8>(GetSFLevel(*reinterpret_cast<int *>(&record[4].m_strCode[60]),
                                                m_BaseCum.m_dwSkillCum[byIndex]));
      m_lvSkill[byIndex] = skillLevel;
      m_SkillUpData.set(byIndex, skillLevel);

      const int skillClass = *reinterpret_cast<int *>(&record[1].m_strCode[4]);
      if (m_mtySkill[skillClass] <= 0x63u)
      {
        m_dwSkillMasteryCum[skillClass] += dwNewCum;
        const unsigned __int8 mastery =
          static_cast<unsigned __int8>(CalcMastery(3, skillClass, m_dwSkillMasteryCum[skillClass], m_byRaceCode));
        m_mtySkill[skillClass] = mastery;
        m_MastUpData.set(3u, static_cast<unsigned __int8>(record[1].m_strCode[4]), mastery);
      }
      break;
    }
    case 4:
    {
      m_BaseCum.m_dwForceCum[byIndex] = dwNewCum;
      m_dwForceLvCum[byIndex & 3] = dwNewCum;
      if (m_mtyForce[byIndex] <= 0x63u)
      {
        const unsigned __int8 mastery =
          static_cast<unsigned __int8>(CalcMastery(4, byIndex, m_BaseCum.m_dwForceCum[byIndex], m_byRaceCode));
        m_mtyForce[byIndex] = mastery;
        m_MastUpData.set(4u, byIndex, mastery);
      }
      if (m_mtyForce[byIndex] <= 0x63u)
      {
        m_mtyStaff = static_cast<unsigned __int8>(GetStaffMastery(m_dwForceLvCum));
      }
      break;
    }
    case 5:
    {
      m_BaseCum.m_dwMakeCum[byIndex] = dwNewCum;
      if (m_mtyMakeItem[byIndex] <= 0x63u)
      {
        unsigned __int8 mastery = 0;
        if (byIndex)
        {
          if (byIndex == 1)
          {
            mastery =
              static_cast<unsigned __int8>(CalcMastery(5, 1, m_BaseCum.m_dwMakeCum[byIndex], m_byRaceCode));
          }
          else if (byIndex == 2)
          {
            mastery =
              static_cast<unsigned __int8>(CalcMastery(5, 2, m_BaseCum.m_dwMakeCum[byIndex], m_byRaceCode));
          }
        }
        else
        {
          mastery = static_cast<unsigned __int8>(CalcMastery(5, 0, m_BaseCum.m_dwMakeCum[0], m_byRaceCode));
        }
        m_mtyMakeItem[byIndex] = mastery;
        m_MastUpData.set(5u, byIndex, mastery);
      }
      break;
    }
    case 6:
    {
      m_BaseCum.m_dwSpecialCum = dwNewCum;
      if (m_mtySpecial <= 0x63u)
      {
        const unsigned __int8 mastery =
          static_cast<unsigned __int8>(CalcMastery(6, 0, m_BaseCum.m_dwSpecialCum, m_byRaceCode));
        m_mtySpecial = mastery;
        m_MastUpData.set(6u, 0, mastery);
      }
      break;
    }
    default:
      return;
  }
}

float _MASTERY_PARAM::GetAveForceMasteryPerClass(unsigned __int8 byClass)
{
  int total = 0;
  for (int index = 0; index < 4; ++index)
  {
    total += m_ppbyMasteryPtr[4][4 * byClass + index];
  }
  return static_cast<float>(total) / 4.0f;
}

float _MASTERY_PARAM::GetAveSkillMasteryPerClass(unsigned __int8 byClass)
{
  int total = 0;
  int count = 1;
  if (byClass)
  {
    for (int index = 4; index <= 6; ++index)
    {
      total += m_ppbyMasteryPtr[3][index];
    }
    count = 3;
  }
  else
  {
    for (int index = 0; index <= 3; ++index)
    {
      total += m_ppbyMasteryPtr[3][index];
    }
    count = 4;
  }
  return static_cast<float>(total) / static_cast<float>(count);
}

void MiningTicket::_AuthKeyTicket::Init()
{
  ___u0.uiData = 0;
}

void MiningTicket::_AuthKeyTicket::Set(unsigned int uiSrc)
{
  ___u0.uiData = uiSrc;
}

void MiningTicket::_AuthKeyTicket::Set(
  unsigned __int16 byYear,
  unsigned __int8 byMonth,
  unsigned __int8 byDay,
  unsigned __int8 byHour,
  unsigned __int8 byNumofTime)
{
  ___u0.uiData = ((byYear & 0x3FFFu) << 18) | (___u0.uiData & 0x3FFFFu);
  ___u0.uiData = ((byMonth & 0xFu) << 14) | (___u0.uiData & 0xFFFC3FFFu);
  ___u0.uiData = ((byDay & 0x1Fu) << 9) | (___u0.uiData & 0xFFFFC1FFu);
  ___u0.uiData = ((byHour & 0x1Fu) << 4) | (___u0.uiData & 0xFFFFFE0Fu);
  ___u0.uiData = (byNumofTime & 0xFu) | (___u0.uiData & 0xFFFFFFF0u);
}

bool MiningTicket::_AuthKeyTicket::operator==(const _AuthKeyTicket &src) const
{
  return ___u0.uiData == src.___u0.uiData;
}

bool MiningTicket::_AuthKeyTicket::operator!=(const _AuthKeyTicket &src) const
{
  return ___u0.uiData != src.___u0.uiData;
}

void MiningTicket::Init()
{
  m_dwTakeLastMentalTicket.Init();
  m_dwTakeLastCriTicket.Init();
}

bool MiningTicket::AuthLastMentalTicket(
  unsigned __int16 byCurrentYear,
  unsigned __int8 byCurrentMonth,
  unsigned __int8 byCurrentDay,
  unsigned __int8 byCurrentHour,
  unsigned __int8 byNumOfTime)
{
  if (!m_dwTakeLastMentalTicket.___u0.uiData)
  {
    return false;
  }

  _AuthKeyTicket current;
  current.Set(byCurrentYear, byCurrentMonth, byCurrentDay, byCurrentHour, byNumOfTime);
  return !(current != m_dwTakeLastMentalTicket);
}

bool MiningTicket::AuthLastCriTicket(
  unsigned __int16 byCurrentYear,
  unsigned __int8 byCurrentMonth,
  unsigned __int8 byCurrentDay,
  unsigned __int8 byCurrentHour,
  unsigned __int8 byNumOfTime)
{
  if (!m_dwTakeLastCriTicket.___u0.uiData)
  {
    return false;
  }

  _AuthKeyTicket current;
  current.Set(byCurrentYear, byCurrentMonth, byCurrentDay, byCurrentHour, byNumOfTime);
  return !(current != m_dwTakeLastCriTicket);
}

unsigned int MiningTicket::GetLastMentalTicket()
{
  return m_dwTakeLastMentalTicket.___u0.uiData;
}

unsigned int MiningTicket::GetLastCriTicket()
{
  return m_dwTakeLastCriTicket.___u0.uiData;
}

void MiningTicket::SetLastMentalTicket(unsigned int uiMentalTicket)
{
  m_dwTakeLastMentalTicket.___u0.uiData = uiMentalTicket;
}

void MiningTicket::SetLastMentalTicket(
  unsigned __int16 byCurrentYear,
  unsigned __int8 byCurrentMonth,
  unsigned __int8 byCurrentDay,
  unsigned __int8 byCurrentHour,
  unsigned __int8 byNumOfTime)
{
  m_dwTakeLastMentalTicket.___u0.uiData =
    ((byCurrentYear & 0x3FFFu) << 18) | (m_dwTakeLastMentalTicket.___u0.uiData & 0x3FFFFu);
  m_dwTakeLastMentalTicket.___u0.uiData =
    ((byCurrentMonth & 0xFu) << 14) | (m_dwTakeLastMentalTicket.___u0.uiData & 0xFFFC3FFFu);
  m_dwTakeLastMentalTicket.___u0.uiData =
    ((byCurrentDay & 0x1Fu) << 9) | (m_dwTakeLastMentalTicket.___u0.uiData & 0xFFFFC1FFu);
  m_dwTakeLastMentalTicket.___u0.uiData =
    ((byCurrentHour & 0x1Fu) << 4) | (m_dwTakeLastMentalTicket.___u0.uiData & 0xFFFFFE0Fu);
  m_dwTakeLastMentalTicket.___u0.uiData =
    (byNumOfTime & 0xFu) | (m_dwTakeLastMentalTicket.___u0.uiData & 0xFFFFFFF0u);
}

void MiningTicket::SetLastCriTicket(unsigned int uiCriTicket)
{
  m_dwTakeLastCriTicket.___u0.uiData = uiCriTicket;
}

void MiningTicket::SetLastCriTicket(
  unsigned __int16 byCurrentYear,
  unsigned __int8 byCurrentMonth,
  unsigned __int8 byCurrentDay,
  unsigned __int8 byCurrentHour,
  unsigned __int8 byNumOfTime)
{
  m_dwTakeLastCriTicket.___u0.uiData =
    ((byCurrentYear & 0x3FFFu) << 18) | (m_dwTakeLastCriTicket.___u0.uiData & 0x3FFFFu);
  m_dwTakeLastCriTicket.___u0.uiData =
    ((byCurrentMonth & 0xFu) << 14) | (m_dwTakeLastCriTicket.___u0.uiData & 0xFFFC3FFFu);
  m_dwTakeLastCriTicket.___u0.uiData =
    ((byCurrentDay & 0x1Fu) << 9) | (m_dwTakeLastCriTicket.___u0.uiData & 0xFFFFC1FFu);
  m_dwTakeLastCriTicket.___u0.uiData =
    ((byCurrentHour & 0x1Fu) << 4) | (m_dwTakeLastCriTicket.___u0.uiData & 0xFFFFFE0Fu);
  m_dwTakeLastCriTicket.___u0.uiData =
    (byNumOfTime & 0xFu) | (m_dwTakeLastCriTicket.___u0.uiData & 0xFFFFFFF0u);
}

_BUDDY_LIST::__list::__list()
{
  init();
}

void _BUDDY_LIST::__list::init()
{
  dwSerial = static_cast<unsigned int>(-1);
  pPtr = nullptr;
}

bool _BUDDY_LIST::__list::fill()
{
  return dwSerial != static_cast<unsigned int>(-1);
}

void _BUDDY_LIST::__list::ON(char *pwszName, CPlayer *ptr)
{
  strcpy_0(wszName, pwszName);
  pPtr = ptr;
}

_BUDDY_LIST::_BUDDY_LIST()
{
  m_LastApply.SetList(8u);
  Init();
}

_BUDDY_LIST::~_BUDDY_LIST() = default;

void _BUDDY_LIST::Init()
{
  for (int index = 0; index < 50; ++index)
  {
    m_List[index].init();
  }
  m_LastApply.ResetList();
}

int _BUDDY_LIST::GetBuddyNum()
{
  unsigned int count = 0;
  for (int index = 0; index < 50; ++index)
  {
    if (m_List[index].fill())
    {
      ++count;
    }
  }
  return static_cast<int>(count);
}

_BUDDY_LIST::__list *_BUDDY_LIST::GetEmptyData()
{
  for (int index = 0; index < 50; ++index)
  {
    if (!m_List[index].fill())
    {
      return &m_List[index];
    }
  }
  return nullptr;
}

int _BUDDY_LIST::PushBuddy(unsigned int dwSerial, char *pwszName, CPlayer *pPtr)
{
  int buddyIndex = -1;
  for (int index = 0; index < 50; ++index)
  {
    if (m_List[index].fill() && m_List[index].dwSerial == dwSerial)
    {
      buddyIndex = index;
      break;
    }
  }

  if (buddyIndex == -1)
  {
    for (int index = 0; index < 50; ++index)
    {
      if (!m_List[index].fill())
      {
        buddyIndex = index;
        break;
      }
    }
  }

  if (buddyIndex == -1)
  {
    return -1;
  }

  _BUDDY_LIST::__list *entry = &m_List[buddyIndex];
  entry->dwSerial = dwSerial;
  strcpy_0(entry->wszName, pwszName);
  entry->pPtr = pPtr;
  return buddyIndex;
}

int _BUDDY_LIST::PopBuddy(unsigned int dwSerial, CPlayer **ppPoper)
{
  for (int index = 0; index < 50; ++index)
  {
    if (m_List[index].fill() && m_List[index].dwSerial == dwSerial)
    {
      if (ppPoper)
      {
        *ppPoper = m_List[index].pPtr;
      }
      m_List[index].dwSerial = static_cast<unsigned int>(-1);
      return index;
    }
  }
  return -1;
}

bool _BUDDY_LIST::IsBuddy(unsigned int dwSerial)
{
  for (int index = 0; index < 50; ++index)
  {
    if (m_List[index].fill() && m_List[index].dwSerial == dwSerial)
    {
      return true;
    }
  }
  return false;
}

bool _BUDDY_LIST::SearchBuddyLogoff(unsigned int dwSerial)
{
  for (int index = 0; index < 50; ++index)
  {
    if (m_List[index].fill() && m_List[index].dwSerial == dwSerial)
    {
      m_List[index].pPtr = nullptr;
      return true;
    }
  }
  return false;
}

bool _BUDDY_LIST::SearchBuddyLogin(CPlayer *pLoger, unsigned int dwSerial, char *pwszName)
{
  for (int index = 0; index < 50; ++index)
  {
    if (m_List[index].fill() && m_List[index].dwSerial == dwSerial)
    {
      m_List[index].pPtr = pLoger;
      strcpy_0(m_List[index].wszName, pwszName);
      return true;
    }
  }
  return false;
}

void _BUDDY_LIST::PushLastApplyTemp(unsigned int dwDstSerial)
{
  unsigned int outIndex[9]{};
  if (!m_LastApply.IsInList(dwDstSerial))
  {
    if (m_LastApply.size() >= 8)
    {
      m_LastApply.PopNode_Front(outIndex);
    }
    m_LastApply.PushNode_Back(dwDstSerial);
  }
}

void _BUDDY_LIST::PopLastApplyTemp(unsigned int dwDstSerial)
{
  m_LastApply.FindNode(dwDstSerial);
}

bool _BUDDY_LIST::IsPushLastApply(unsigned int dwDstSerial)
{
  return m_LastApply.IsInList(dwDstSerial);
}

void _NameChangeBuddyInfo::Init()
{
  bNameChange = false;
  nSendNum = 0;
}

void CPlayer::SendMsg_GotoRecallResult(
  char byErrCode,
  char byMapCode,
  float *pfStartPos,
  unsigned __int8 byMapInType)
{
  struct
  {
    char errCode;
    char mapCode;
    float startPos[3];
    unsigned __int8 mapInType;
  } msg{};

  msg.errCode = byErrCode;
  msg.mapCode = byMapCode;
  memcpy_0(msg.startPos, pfStartPos, sizeof(msg.startPos));
  msg.mapInType = byMapInType;

  unsigned __int8 type[2]{};
  type[0] = 4;
  type[1] = 29;
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 15);
}

void CPlayer::SendMsg_GotoBasePortalResult(char byErrCode)
{
  char msg[2]{};
  short pos[3]{};

  msg[0] = byErrCode;
  msg[1] = static_cast<char>(m_pCurMap->m_pMapSet->m_dwIndex);
  FloatToShort(m_fCurPos, pos, 3);

  unsigned __int8 type[2]{};
  type[0] = 4;
  type[1] = 27;
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 8u);
}

void CPlayer::SendMsg_MovePortal(
  char byRet,
  char byMapIndex,
  unsigned __int8 byPotalIndex,
  float *pfStartPos,
  bool bEqualZone)
{
  (void)byPotalIndex;
  unsigned __int8 msg[15]{};
  msg[0] = static_cast<unsigned __int8>(byRet);
  msg[1] = static_cast<unsigned __int8>(byMapIndex);
  memcpy_0(msg + 2, pfStartPos, 12);
  msg[14] = static_cast<unsigned __int8>(!bEqualZone);
  unsigned __int8 type[2] = {8, 2};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, reinterpret_cast<char *>(msg), 15u);
}

void CPlayer::SendMsg_MovePortal(char byMapIndex, float *pfStartPos, unsigned __int8 byZoneCode)
{
  unsigned __int8 msg[15]{};
  msg[0] = 0;
  msg[1] = static_cast<unsigned __int8>(byMapIndex);
  memcpy_0(msg + 2, pfStartPos, 12);
  msg[14] = byZoneCode;
  unsigned __int8 type[2] = {8, 2};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, reinterpret_cast<char *>(msg), 15u);
}

void CPlayer::SendMsg_RegistBindResult(char byRetCode)
{
  char msg[1]{};
  msg[0] = byRetCode;
  unsigned __int8 type[2] = {8, 4};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_MoveError(char byRetCode)
{
  char msg[13]{};
  msg[0] = byRetCode;
  memcpy_0(msg + 1, m_fCurPos, 12);

  unsigned __int8 type[2]{4, 3};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 0xDu);
}

void CPlayer::SendMsg_Stop(bool bAll)
{
  char msg[10]{};
  *reinterpret_cast<unsigned int *>(msg) = m_dwObjSerial;
  FloatToShort(m_fCurPos, reinterpret_cast<short *>(msg + 4), 3);

  unsigned __int8 type[2]{4, 20};
  if (bAll)
  {
    CircleReport(type, msg, 10, true);
  }
  else
  {
    g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 0xAu);
  }
}

void CPlayer::SendMsg_MoveNext(bool bOtherSend)
{
  char msg[17]{};
  *reinterpret_cast<unsigned int *>(msg) = m_dwObjSerial;
  FloatToShort(m_fCurPos, reinterpret_cast<short *>(msg + 4), 3);
  auto *shorts = reinterpret_cast<short *>(msg + 4);
  shorts[3] = static_cast<short>(m_fTarPos[0]);
  shorts[4] = static_cast<short>(m_fTarPos[2]);
  shorts[5] = static_cast<short>(GetAddSpeed());
  msg[16] = static_cast<char>(m_byMoveDirect);

  unsigned __int8 type[2]{4, 4};
  if (bOtherSend)
  {
    CircleReport(type, msg, 17, true);
  }
  else
  {
    g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 0x11u);
  }
}

void CPlayer::SendMsg_QuestProcess(char byQuestDBSlot, char byActIndex, unsigned __int16 wCount)
{
  char msg[4]{};
  msg[0] = byQuestDBSlot;
  msg[1] = byActIndex;
  *reinterpret_cast<unsigned __int16 *>(msg + 2) = wCount;

  unsigned __int8 type[2]{24, 6};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 4u);
}

void CPlayer::SendMsg_SelectQuestReward(char byQuestDBSlot)
{
  char msg[1]{};
  msg[0] = byQuestDBSlot;

  unsigned __int8 type[2]{24, 8};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_QuestFailure(char byFailCode, char byQuestDBSlot)
{
  char msg[2]{};
  msg[0] = byFailCode;
  msg[1] = byQuestDBSlot;

  unsigned __int8 type[2]{24, 7};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 2u);
}

void CPlayer::SendMsg_InsertNextQuest(unsigned __int8 bySlotIndex, _QUEST_DB_BASE::_LIST *pQuestDB)
{
  _insert_next_quest_inform_zocl msg{};
  msg.byQuestDBSlot = bySlotIndex;
  memcpy_0(&msg.NewQuestData, pQuestDB, sizeof(msg.NewQuestData));

  unsigned __int8 type[2]{24, 111};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 0xEu);
}

void CPlayer::SendMsg_InsertNewQuest(unsigned __int8 bySlotIndex, _QUEST_DB_BASE::_LIST *pQuestDB)
{
  _insert_new_quest_inform_zocl msg{};
  msg.byQuestDBSlot = bySlotIndex;
  memcpy_0(&msg.NewQuestData, pQuestDB, sizeof(msg.NewQuestData));

  unsigned __int8 type[2]{24, 1};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 0xEu);
}

void CPlayer::SendMsg_QuestComplete(char byQuestDBSlot)
{
  char msg[1]{};
  msg[0] = byQuestDBSlot;

  unsigned __int8 type[2]{24, 5};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_NpcQuestHistoryInform(char bySlotIndex)
{
  char msg[9]{};
  msg[0] = bySlotIndex;
  strcpy_0(msg + 1, m_QuestMgr.m_pQuestData->m_History[static_cast<unsigned __int8>(bySlotIndex)].szQuestCode);

  unsigned __int8 type[2]{24, 19};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 9u);
}

bool CPlayer::IsOutExtraStopPos(float *pfStopPos)
{
  return GetSqrt(m_fTarPos, pfStopPos) > 20.0f;
}

void CPlayer::pc_MoveModeChangeRequest(unsigned __int8 byMoveType)
{
  if (byMoveType != 2)
  {
    BreakCloakBooster();
  }
  m_byMoveType = byMoveType;
  SenseState();
}

void CPlayer::pc_MoveNext(unsigned __int8 byMoveType, float *pfCur, float *pfTar, unsigned __int8 byDirect)
{
  unsigned __int8 errorCode = 0;
  if (m_pmTrd.bDTradeMode)
  {
    errorCode = 7;
  }
  else if (m_bCorpse)
  {
    errorCode = 8;
  }
  else if (IsSiegeMode() || IsActingSiegeMode())
  {
    errorCode = 12;
  }
  else if (m_byStandType == 1)
  {
    errorCode = 10;
  }
  else if (m_bMapLoading)
  {
    errorCode = 1;
  }
  else if (_effect_parameter::GetEff_State(&m_EP, 6))
  {
    errorCode = 2;
  }
  else if (_effect_parameter::GetEff_State(&m_EP, 7) && byMoveType == 1)
  {
    errorCode = 3;
  }
  else if (_effect_parameter::GetEff_State(&m_EP, 20))
  {
    errorCode = 13;
  }
  else if (CMapData::IsMapIn(m_pCurMap, pfCur))
  {
    if (m_byUserDgr || (std::fabs(m_fCurPos[0] - pfCur[0]) <= 100.0f && std::fabs(m_fCurPos[2] - pfCur[2]) <= 100.0f))
    {
      const int raceTown = m_byPosRaceTown;
      const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
      if (raceTown != raceCode && (byMoveType == 1 || byMoveType == 2))
      {
        if (IsRidingUnit())
        {
          if (!m_pUsingUnit->wBooster)
          {
            errorCode = 5;
          }
        }
        else if (m_bMove)
        {
          if (!GetSP())
          {
            errorCode = 5;
          }
        }
        else if (m_fEquipSpeed >= static_cast<float>(GetSP()))
        {
          errorCode = 5;
        }
      }
    }
    else
    {
      errorCode = 11;
    }
  }
  else
  {
    errorCode = 4;
  }

  if (errorCode)
  {
    SendMsg_MoveError(static_cast<char>(errorCode));
    if (m_bMove)
    {
      const bool outExtra = IsOutExtraStopPos(m_fCurPos);
      SendMsg_Stop(outExtra);
      CCharacter::Stop(this);
    }
    return;
  }

  const int raceTown = m_byPosRaceTown;
  const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
  if (raceTown != raceCode && !IsRidingUnit() && !m_bMove && byMoveType == 1)
  {
    int sp = GetSP();
    sp = static_cast<int>(static_cast<float>(sp) - m_fEquipSpeed);
    if (sp < 0)
    {
      sp = 0;
    }
    SetSP(sp, false);
    SendMsg_AlterSPInform();
  }

  m_byMoveType = byMoveType;
  m_byMoveDirect = byDirect;
  memcpy_0(m_fOldPos, m_fCurPos, sizeof(m_fOldPos));
  memcpy_0(m_fCurPos, pfCur, sizeof(m_fCurPos));

  bool otherSend = true;
  if (m_bMove && m_byLastDirect == byDirect)
  {
    const float diffX = std::fabs(m_fSendTarPos[0] - pfTar[0]);
    const float diffZ = std::fabs(m_fSendTarPos[1] - pfTar[2]);
    const float diff = diffX <= diffZ ? diffZ : diffX;
    if (diff < 10.0f)
    {
      otherSend = false;
    }
  }

  if (otherSend)
  {
    m_fSendTarPos[0] = pfTar[0];
    m_fSendTarPos[1] = pfTar[2];
    m_byLastDirect = byDirect;
  }

  memcpy_0(m_fTarPos, pfTar, sizeof(m_fTarPos));
  CCharacter::ResetSlot(this);
  CCharacter::Go(this);
  SendMsg_MoveNext(otherSend);
  if (_effect_parameter::GetEff_State(&m_EP, 14))
  {
    CCharacter::RemoveSFContHelpByEffect(this, 2, 14);
  }
  m_dwLastSetPointTime = GetLoopTime();
  if (m_bMineMode)
  {
    m_bMineMode = false;
    m_dwMineNextTime = static_cast<unsigned int>(-1);
    SendMsg_MineCancle();
  }
}

void CPlayer::pc_RealMovPos(float *pfCur)
{
  unsigned __int8 errorCode = 0;
  if (m_bMove)
  {
    if (CRealMoveRequestDelayChecker::Check(&m_kMoveDelayChecker, this))
    {
      if (m_pmTrd.bDTradeMode)
      {
        errorCode = 7;
      }
      else if (m_bCorpse)
      {
        errorCode = 8;
      }
      else if (IsSiegeMode())
      {
        errorCode = 12;
      }
      else if (!memcmp_0(pfCur, m_fCurPos, 12))
      {
        errorCode = 6;
      }
      else if (m_byStandType == 1)
      {
        errorCode = 10;
      }
      else if (_effect_parameter::GetEff_State(&m_EP, 20))
      {
        errorCode = 13;
      }
      else if (m_byUserDgr
               || (std::fabs(m_fCurPos[0] - pfCur[0]) <= 200.0f
                   && std::fabs(m_fCurPos[2] - pfCur[2]) <= 200.0f))
      {
        if (!CMapData::IsMapIn(m_pCurMap, pfCur))
        {
          errorCode = 4;
        }
      }
      else
      {
        errorCode = 11;
      }
    }
    else
    {
      errorCode = 6;
    }
  }
  else
  {
    errorCode = 6;
  }

  if (errorCode)
  {
    SendMsg_MoveError(static_cast<char>(errorCode));
    if (m_bMove)
    {
      const bool outExtra = IsOutExtraStopPos(m_fCurPos);
      SendMsg_Stop(outExtra);
      CCharacter::Stop(this);
    }
  }
  else
  {
    memcpy_0(m_fOldPos, m_fCurPos, sizeof(m_fOldPos));
    memcpy_0(m_fCurPos, pfCur, sizeof(m_fCurPos));
    ++m_nCheckMovePacket;
    m_dwLastSetPointTime = GetLoopTime();
  }
}

void CPlayer::pc_MoveStop(float *pfCur)
{
  unsigned __int8 errorCode = 0;
  if (m_bMove)
  {
    if (m_pmTrd.bDTradeMode)
    {
      errorCode = 7;
    }
    else if (m_bCorpse)
    {
      errorCode = 8;
    }
    else if (IsSiegeMode())
    {
      errorCode = 12;
    }
    else if (m_byStandType == 1)
    {
      errorCode = 10;
    }
    else if (m_byUserDgr
             || (std::fabs(m_fCurPos[0] - pfCur[0]) <= 200.0f
                 && std::fabs(m_fCurPos[2] - pfCur[2]) <= 200.0f))
    {
      if (!CMapData::IsMapIn(m_pCurMap, pfCur))
      {
        errorCode = 4;
      }
    }
    else
    {
      errorCode = 11;
    }
  }
  else
  {
    errorCode = 6;
  }

  CRealMoveRequestDelayChecker::Reset(&m_kMoveDelayChecker);
  if (errorCode)
  {
    SendMsg_MoveError(static_cast<char>(errorCode));
    if (m_bMove)
    {
      const bool outExtra = IsOutExtraStopPos(m_fCurPos);
      SendMsg_Stop(outExtra);
      CCharacter::Stop(this);
    }
  }
  else
  {
    memcpy_0(m_fOldPos, m_fCurPos, sizeof(m_fOldPos));
    memcpy_0(m_fCurPos, pfCur, sizeof(m_fCurPos));
    ++m_nCheckMovePacket;
    const bool outExtra = IsOutExtraStopPos(m_fCurPos);
    SendMsg_Stop(outExtra);
    CCharacter::Stop(this);
  }
}

void CPlayer::pc_GotoBasePortalRequest(unsigned __int16 wItemSerial)
{
  unsigned __int8 resultCode = 0;
  CMapData *intoMap = nullptr;
  _STORAGE_LIST::_db_con *item = nullptr;
  _base_fld *record = nullptr;
  float newPos[3]{};

  if (m_bInGuildBattle)
  {
    resultCode = 12;
  }
  else if (CGameObject::GetCurSecNum(this) == -1 || m_bMapLoading)
  {
    resultCode = 5;
  }
  else if (m_bCorpse)
  {
    resultCode = 1;
  }
  else if (m_pCurMap->m_pMapSet->m_nMapType)
  {
    resultCode = 8;
  }
  else if (m_byStandType == 1)
  {
    resultCode = 8;
  }
  else if (IsSiegeMode())
  {
    resultCode = 8;
  }
  else if (IsRidingUnit())
  {
    resultCode = 8;
  }
  else if (_effect_parameter::GetEff_State(&m_EP, 20))
  {
    resultCode = 9;
  }
  else if (_effect_parameter::GetEff_State(&m_EP, 28))
  {
    resultCode = 9;
  }
  else if (wItemSerial == 0xFFFF)
  {
    const unsigned __int8 raceCode = CPlayerDB::GetRaceCode(&m_Param);
    intoMap = CMapOperation::GetPosStartMap(&g_MapOper, raceCode, 0, newPos);
    if (!intoMap)
    {
      resultCode = 11;
    }
  }
  else
  {
    item = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, wItemSerial);
    if (item)
    {
      if (item->m_byTableCode == 22)
      {
        if (CHolyStoneSystem::IsUseReturnItem(&g_HolySys, m_dwObjSerial))
        {
          if (item->m_bLock)
          {
            resultCode = 22;
          }
          else
          {
            record = CRecordData::GetRecord(&g_Main.m_tblItemData[22], item->m_wItemIndex);
            if (record)
            {
              if (m_pCurMap->m_pMapSet->m_nMapType == 2)
              {
                resultCode = 11;
              }
              else if (m_pCurMap->m_pMapSet->m_nMapClass
                       && !*reinterpret_cast<int *>(&record[7].m_strCode[44])
                       && (strcmp_0(m_pCurMap->m_pMapSet->m_strCode, "Elan")
                           || (!strcmp_0(m_pCurMap->m_pMapSet->m_strCode, "Platform01")
                               || !strcmp_0(m_pCurMap->m_pMapSet->m_strCode, "Medicallab"))
                             && _effect_parameter::GetEff_Have(&m_EP, 55) == 0.0)
                       && strcmp_0(record[4].m_strCode, m_pCurMap->m_pMapSet->m_strCode))
              {
                resultCode = 11;
              }
              else
              {
                const int level = static_cast<int>(GetLevel());
                if (level >= *reinterpret_cast<int *>(&record[6].m_strCode[20]))
                {
                  if (*reinterpret_cast<int *>(&record[6].m_strCode[24]) == -1
                    || static_cast<int>(GetLevel()) <= *reinterpret_cast<int *>(&record[6].m_strCode[24]))
                  {
                    intoMap = CMapOperation::GetMap(&g_MapOper, record[4].m_strCode);
                    if (intoMap)
                    {
                      _dummy_position *pos = CMapData::GetDummyPostion(intoMap, reinterpret_cast<char *>(&record[5]));
                      if (pos)
                      {
                        if (CMapData::GetRandPosInDummy(intoMap, pos, newPos, 1))
                        {
                          if (Major_Scroll_Item && record)
                          {
                            if (!strcmp_0(record[4].m_strCode, "Elan")
                              || !strcmp_0(record[4].m_strCode, "NeutralA")
                              || !strcmp_0(record[4].m_strCode, "NeutralB")
                              || !strcmp_0(record[4].m_strCode, "NeutralC"))
                            {
                              resultCode = 0;
                            }
                            else
                            {
                              resultCode = 22;
                            }
                          }
                          else
                          {
                            const int cashType = GetUsePcCashType(item->m_byTableCode, item->m_wItemIndex);
                            if (!IsUsableAccountType(cashType))
                            {
                              SendMsg_PremiumCashItemUse(0xFFFF);
                              resultCode = 22;
                            }
                          }
                        }
                        else
                        {
                          resultCode = 7;
                        }
                      }
                      else
                      {
                        resultCode = 2;
                      }
                    }
                    else
                    {
                      resultCode = 11;
                    }
                  }
                  else
                  {
                    resultCode = 15;
                  }
                }
                else
                {
                  resultCode = 15;
                }
              }
            }
            else
            {
              resultCode = 10;
            }
          }
        }
        else
        {
          resultCode = 13;
        }
      }
      else
      {
        resultCode = 10;
      }
    }
    else
    {
      SendMsg_AdjustAmountInform(0, wItemSerial, 0);
      resultCode = 10;
    }
  }

  if (!resultCode)
  {
    OutOfMap(intoMap, 0, 3, newPos);
    if (wItemSerial != 0xFFFF)
    {
      Emb_AlterDurPoint(0, item->m_byStorageIndex, -1, false, false);
    }
    Emb_CheckActForQuest(16, record->m_strCode, 1, false);
  }
  SendMsg_GotoBasePortalResult(static_cast<char>(resultCode));
}

void CPlayer::pc_GotoAvatorRequest(char *pwszAvatorName)
{
  unsigned __int8 resultCode = 0;
  CPlayer *target = nullptr;

  if (m_pmTrd.bDTradeMode)
  {
    resultCode = 6;
  }
  else if (CGameObject::GetCurSecNum(this) == -1 || m_bMapLoading)
  {
    resultCode = 5;
  }
  else if (m_byStandType == 1)
  {
    resultCode = 8;
  }
  else
  {
    for (int j = 0; j < MAX_PLAYER; ++j)
    {
      CPlayer *player = &g_Player[j];
      if (player->m_bLive)
      {
        const char *name = CPlayerDB::GetCharNameW(&player->m_Param);
        if (!strcmp_0(name, pwszAvatorName))
        {
          target = player;
          break;
        }
      }
    }
    if (!target)
    {
      resultCode = 2;
    }
  }

  float startPos[3];
  if (!resultCode)
  {
    startPos[0] = target->m_fCurPos[0];
    startPos[2] = target->m_fCurPos[2];
    startPos[1] = target->m_fCurPos[1];
    OutOfMap(target->m_pCurMap, target->m_wMapLayerIndex, 4, startPos);
  }

  const unsigned __int8 mapCode = CPlayerDB::GetMapCode(&m_Param);
  SendMsg_GotoRecallResult(static_cast<char>(resultCode), mapCode, startPos, 4);
}

void CPlayer::pc_MovePortal(int nPortalIndex, unsigned __int16 *pConsumeSerial)
{
  unsigned __int8 retCode = 0;
  unsigned __int8 mapIndex = 0xFF;
  unsigned __int8 linkPortalIndex = 0xFF;
  _portal_dummy *portal = nullptr;
  _portal_dummy *linkPortal = nullptr;
  CMapData *intoMap = nullptr;
  float newPos[8]{};
  unsigned __int16 layerIndex = 0;

  newPos[7] = 0.0f;

  if (m_bInGuildBattle)
  {
    retCode = 19;
  }
  else if (CGameObject::GetCurSecNum(this) == -1 || m_bMapLoading)
  {
    retCode = 1;
  }
  else if (m_pmTrd.bDTradeMode)
  {
    retCode = 2;
  }
  else if (m_byStandType == 1)
  {
    retCode = 3;
  }
  else if (IsSiegeMode())
  {
    retCode = 4;
  }
  else
  {
    portal = CMapData::GetPortal(m_pCurMap, nPortalIndex);
    if (portal)
    {
      if (!m_byUserDgr)
      {
        _portal_fld *portalRec = portal->m_pPortalRec;
        const int raceSexCode = CPlayerDB::GetRaceSexCode(&m_Param);
        if (portalRec->m_strUseRace[raceSexCode] == '0')
        {
          retCode = 6;
          goto Result;
        }

        const unsigned int lineIndex = portal->m_pDumPos->m_wLineIndex;
        CExtDummy *dummy = &m_pCurMap->m_Dummy;
        if (!CExtDummy::IsInBBox(dummy, lineIndex, m_fCurPos))
        {
          retCode = 7;
          goto Result;
        }
      }

      if (portal->m_pPortalRec)
      {
        intoMap = CMapOperation::GetMap(&g_MapOper, portal->m_pPortalRec->m_strLinkMapCode);
        if (intoMap)
        {
          mapIndex = static_cast<unsigned __int8>(intoMap->m_pMapSet->m_dwIndex);
          if (m_pDHChannel)
          {
            if (m_pDHChannel->m_pQuestSetup->pUseMap != intoMap)
            {
              retCode = 15;
              goto Result;
            }
            if (CDarkHoleChannel::GetLeaderSerial(m_pDHChannel) != m_dwObjSerial)
            {
              retCode = 16;
              goto Result;
            }
            if (!CDarkHoleChannel::IsMoveNextMission(m_pDHChannel, nPortalIndex))
            {
              retCode = 17;
              goto Result;
            }
            if (!CDarkHoleChannel::IsAllMemberNearPosition(m_pDHChannel, portal->m_pDumPos->m_fCenterPos, 50))
            {
              retCode = 18;
              goto Result;
            }
            layerIndex = m_pDHChannel->m_wLayerIndex;
          }
          else
          {
            const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
            if (intoMap == g_TransportShip[raceCode].m_pLinkShipMap)
            {
              CTransportShip *ship = &g_TransportShip[raceCode];
              if (!ship->m_bAnchor)
              {
                retCode = 9;
                goto Result;
              }
              if (CTransportShip::GetMapCurDirect(ship) != m_pCurMap)
              {
                retCode = 11;
                goto Result;
              }

              const int level = static_cast<int>(GetLevel());
              const int rideLimit = CTransportShip::GetRideLimLevel(ship);
              if (level < rideLimit)
              {
                retCode = 14;
                goto Result;
              }

              if (CTransportShip::GetRideUpLimLevel(ship) != -1)
              {
                const int upperLevel = static_cast<int>(GetLevel());
                const int rideUpLimit = CTransportShip::GetRideUpLimLevel(ship);
                if (upperLevel > rideUpLimit)
                {
                  retCode = 14;
                  goto Result;
                }
              }

              bool hasTicket = false;
              for (int j = 0; j < m_Param.m_dbInven.m_nUsedNum; ++j)
              {
                _STORAGE_LIST::_db_con *item = &m_Param.m_dbInven.m_pStorageList[j];
                if (item->m_bLoad)
                {
                  if (item->m_byTableCode == 28)
                  {
                    auto *ticketRecord = reinterpret_cast<_TicketItem_fld *>(
                      CRecordData::GetRecord(&g_Main.m_tblItemData[28], item->m_wItemIndex));
                    if (CTransportShip::GetCurRideShipThisTicket(ship, ticketRecord))
                    {
                      hasTicket = true;
                      break;
                    }
                  }
                }
              }

              if (!hasTicket)
              {
                retCode = 12;
                goto Result;
              }
              layerIndex = CPlayerDB::GetRaceCode(&m_Param);
            }
            else if (m_pCurMap == g_TransportShip[raceCode].m_pLinkShipMap)
            {
              CTransportShip *ship = &g_TransportShip[raceCode];
              if (!ship->m_bAnchor)
              {
                retCode = 9;
                goto Result;
              }

              if (CTransportShip::GetMapCurDirect(ship) != intoMap)
              {
                retCode = 11;
                goto Result;
              }

              const int raceCodeValue = CPlayerDB::GetRaceCode(&m_Param);
              if (nPortalIndex != 3 * ship->m_byDirect + raceCodeValue)
              {
                retCode = 5;
                goto Result;
              }
            }
            else if (!strcmp_0(intoMap->m_pMapSet->m_strCode, "Cauldron01")
              && CPlayerDB::GetLevel(&m_Param) < 40)
            {
              retCode = 14;
              goto Result;
            }
          }

          linkPortal = CMapData::GetPortal(intoMap, portal->m_pPortalRec->m_strLinkPortalCode);
          if (linkPortal)
          {
            linkPortalIndex = static_cast<unsigned __int8>(linkPortal->m_pPortalRec->m_dwIndex);
            if (CMapData::GetRandPosInDummy(intoMap, linkPortal->m_pDumPos, newPos, 1))
            {
              if (!m_pDHChannel || CDarkHoleChannel::SearchMissionFromPos(m_pDHChannel, newPos))
              {
                _portal_fld *portalRec = portal->m_pPortalRec;
                if (portalRec->m_bNeedConCheck)
                {
                  if (portalRec->m_nUpLevelLim > -1)
                  {
                    const int level = static_cast<int>(GetLevel());
                    if (level < portalRec->m_nNeedChrLevel
                      || static_cast<int>(GetLevel()) >= portalRec->m_nUpLevelLim)
                    {
                      retCode = 21;
                      goto Result;
                    }

                    _STORAGE_LIST::_db_con *consumeItems[3]{};
                    int consumeCount[3]{};
                    bool overlap[3]{};
                    if (!GetUseConsumeItem(portalRec->m_ConsumeItemList, pConsumeSerial, consumeItems, consumeCount, overlap))
                    {
                      retCode = 20;
                      goto Result;
                    }
                    DeleteUseConsumeItem(consumeItems, consumeCount, overlap);
                  }
                  else
                  {
                    const int level = static_cast<int>(GetLevel());
                    if (level < portalRec->m_nNeedChrLevel)
                    {
                      retCode = 21;
                      goto Result;
                    }

                    _STORAGE_LIST::_db_con *consumeItems[3]{};
                    int consumeCount[3]{};
                    bool overlap[3]{};
                    if (!GetUseConsumeItem(portalRec->m_ConsumeItemList, pConsumeSerial, consumeItems, consumeCount, overlap))
                    {
                      retCode = 20;
                      goto Result;
                    }
                    DeleteUseConsumeItem(consumeItems, consumeCount, overlap);
                  }
                }

                if (m_Param.m_pGuild)
                {
                  const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
                  CGuildRoomSystem *guildRoom = CGuildRoomSystem::GetInstance();
                  if (m_pCurMap == CGuildRoomSystem::GetMapData(guildRoom, raceCode, 0)
                    || (guildRoom = CGuildRoomSystem::GetInstance(),
                        m_pCurMap == CGuildRoomSystem::GetMapData(guildRoom, CPlayerDB::GetRaceCode(&m_Param), 1u)))
                  {
                    CGuildRoomSystem *roomSystem = CGuildRoomSystem::GetInstance();
                    CGuild *guild = m_Param.m_pGuild;
                    roomSystem->RoomOut(guild->m_dwSerial, m_ObjID.m_wIndex, m_pUserDB->m_dwSerial);
                  }
                }

                if (Major_Sette_Mine_Elan_Map)
                {
                  if (!strcmp_0(intoMap->m_pMapSet->m_strCode, "resources")
                    || !strcmp_0(intoMap->m_pMapSet->m_strCode, "Sette")
                    || !strcmp_0(intoMap->m_pMapSet->m_strCode, "elan")
                    || !strcmp_0(intoMap->m_pMapSet->m_strCode, "NeutralA")
                    || !strcmp_0(intoMap->m_pMapSet->m_strCode, "NeutralB")
                    || !strcmp_0(intoMap->m_pMapSet->m_strCode, "NeutralC"))
                  {
                    retCode = 0;
                  }
                  else
                  {
                    retCode = 8;
                  }
                }
              }
              else
              {
                retCode = 8;
              }
            }
            else
            {
              retCode = 8;
            }
          }
          else
          {
            retCode = 8;
          }
        }
        else
        {
          retCode = 8;
        }
      }
      else
      {
        retCode = 8;
      }
    }
    else
    {
      retCode = 5;
    }
  }

Result:
  if (!retCode)
  {
    OutOfMap(intoMap, layerIndex, 1u, newPos);
    if (m_pDHChannel)
    {
      if (CDarkHoleChannel::GotoNextMissionByPosition(m_pDHChannel, newPos))
      {
        CDarkHoleChannel::NextMissionOtherQuester(
          m_pDHChannel,
          this,
          m_pDHChannel->m_MissionMgr.pCurMssionPtr);
      }
    }
  }

  SendMsg_MovePortal(static_cast<char>(retCode), static_cast<char>(mapIndex), linkPortalIndex, newPos, true);
}

void CPlayer::pc_RegistBind(CItemStore *pStore)
{
  unsigned __int8 resultCode = 0;
  char *dummyCode = nullptr;

  if (m_pUserDB)
  {
    bool hasBindNpc = false;
    for (int j = 0; j < 10; ++j)
    {
      if (pStore->m_pRec->m_nNpc_Class[j] == 13)
      {
        hasBindNpc = true;
        break;
      }
    }

    if (hasBindNpc)
    {
      if (GetSqrt(pStore->m_pDum->m_pDumPos->m_fCenterPos, m_fCurPos) <= 100.0)
      {
        if (m_pCurMap->m_pMapSet->m_nMapType)
        {
          resultCode = 1;
        }
        else if (m_pCurMap->m_pMapSet->m_nMapClass)
        {
          resultCode = 1;
        }
        else
        {
          dummyCode = static_cast<char *>(
            m_pCurMap->m_tbBindDumPos.GetRecord(pStore->m_pRec->m_strBinding_DummyName));
          if (!dummyCode)
          {
            resultCode = 1;
          }
        }
      }
      else
      {
        resultCode = 1;
      }
    }
    else
    {
      resultCode = 1;
    }

    if (!resultCode)
    {
      m_pBindMapData = m_pCurMap;
      m_pBindDummyData = reinterpret_cast<_dummy_position *>(dummyCode);
      m_pUserDB->Update_Bind(m_pCurMap->m_pMapSet->m_strCode, dummyCode, 1);
    }
    SendMsg_RegistBindResult(static_cast<char>(resultCode));
  }
}

void CPlayer::SendMsg_MapOut(unsigned __int8 byMapOutCode, unsigned __int8 byNextMapCode)
{
  struct
  {
    unsigned __int8 mapOutCode;
    unsigned int objSerial;
    unsigned __int8 nextMapCode;
  } msg{};

  msg.mapOutCode = byMapOutCode;
  msg.objSerial = m_dwObjSerial;
  msg.nextMapCode = byNextMapCode;

  unsigned __int8 type[2]{};
  type[0] = 3;
  type[1] = 39;
  CircleReport(type, reinterpret_cast<char *>(&msg), 6, false);
}

bool CPlayer::IsHaveMentalTicket()
{
  const unsigned __int8 byNumOfTime = g_HolySys.GetNumOfTime();
  const unsigned __int8 byCurrentHour = g_HolySys.GetStartHour();
  const unsigned __int8 startDay = g_HolySys.GetStartDay();
  const unsigned __int8 startMonth = g_HolySys.GetStartMonth();
  const unsigned __int16 startYear = g_HolySys.GetStartYear();

  return m_MinigTicket.AuthLastCriTicket(startYear, startMonth, startDay, byCurrentHour, byNumOfTime);
}

bool CPlayer::IsMiningByMinigTicket()
{
  if (!g_HolySys.IsMinigeTicketCheck())
  {
    return true;
  }

  const unsigned __int8 byNumOfTime = g_HolySys.GetNumOfTime();
  const unsigned __int8 byCurrentHour = g_HolySys.GetStartHour();
  const unsigned __int8 startDay = g_HolySys.GetStartDay();
  const unsigned __int8 startMonth = g_HolySys.GetStartMonth();
  const unsigned __int16 startYear = g_HolySys.GetStartYear();

  return m_MinigTicket.AuthLastCriTicket(startYear, startMonth, startDay, byCurrentHour, byNumOfTime) != 0;
}

bool CPlayer::IsUseReleaseRaceBuffPotion()
{
  const unsigned __int8 byNumOfTime = g_HolySys.GetNumOfTime();
  const unsigned __int8 byHour = g_HolySys.GetStartHour();
  const unsigned __int8 startDay = g_HolySys.GetStartDay();
  const unsigned __int8 startMonth = g_HolySys.GetStartMonth();
  const unsigned __int16 startYear = g_HolySys.GetStartYear();

  MiningTicket::_AuthKeyTicket key;
  key.Set(startYear, startMonth, startDay, byHour, byNumOfTime);
  return m_dwRaceBuffClearKey == key;
}

void CPlayer::SetUseReleaseRaceBuffPotion()
{
  const unsigned __int8 byNumOfTime = g_HolySys.GetNumOfTime();
  const unsigned __int8 byHour = g_HolySys.GetStartHour();
  const unsigned __int8 startDay = g_HolySys.GetStartDay();
  const unsigned __int8 startMonth = g_HolySys.GetStartMonth();
  const unsigned __int16 startYear = g_HolySys.GetStartYear();

  m_dwRaceBuffClearKey.Set(startYear, startMonth, startDay, byHour, byNumOfTime);
  m_pUserDB->m_AvatorData.dbSupplement.dwRaceBuffClear = m_dwRaceBuffClearKey.___u0.uiData;
  m_pUserDB->m_bDataUpdate = true;
}

void CPlayer::SendMsg_MapEnvInform(char byMapCode, unsigned int dwMapEnvCode)
{
  struct
  {
    char mapCode;
    unsigned int mapEnvCode;
    int mapTime;
  } msg{};

  msg.mapCode = byMapCode;
  msg.mapEnvCode = dwMapEnvCode;

  std::time_t now{};
  std::time(&now);
  std::tm localTime{};
  if (auto *tmPtr = std::localtime(&now))
  {
    localTime = *tmPtr;
  }
  localTime.tm_hour = g_WorldSch.m_nCurHour;
  localTime.tm_min = g_WorldSch.m_nCurMin;
  localTime.tm_sec = g_WorldSch.m_nCurMilSec / 1000;
  msg.mapTime = static_cast<int>(std::mktime(&localTime));

  unsigned __int8 type[2]{};
  type[0] = 8;
  type[1] = 5;
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 9);
}

void CPlayer::SendMsg_MineCancle()
{
  unsigned __int8 type[2]{};
  type[0] = 14;
  type[1] = 4;

  char msg{};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, &msg, 1);
}

void CPlayer::SendMsg_MineStartResult(char byErrCode)
{
  _mine_start_result_zocl msg{};
  msg.byErrCode = byErrCode;

  unsigned __int8 pbyType[2] = {14, 2};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    1u);
}

void CPlayer::SendMsg_MineCompleteResult(
  char byErrCode,
  unsigned __int8 byNewOreIndex,
  unsigned __int16 wOreSerial,
  unsigned __int8 byOreDur,
  unsigned __int16 wBatteryLeftDurPoint)
{
  _mine_complete_result_zocl msg{};
  msg.byErrCode = byErrCode;
  msg.wEquipLeftDur = static_cast<unsigned __int16>(m_Param.m_dbEquip.m_pStorageList[6].m_dwDur);
  msg.wBatteryLeftDur = wBatteryLeftDurPoint;
  msg.byOreIndex = byNewOreIndex;
  msg.wOreSerial = wOreSerial;
  msg.byOreDur = byOreDur;

  unsigned __int8 pbyType[2] = {14, 6};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    9u);
}

void CPlayer::SendMsg_OreCuttingResult(
  unsigned __int8 byErrCode,
  unsigned __int8 byLeftOreNum,
  unsigned int dwConsumDalant)
{
  _ore_cutting_result_zocl msg;
  msg.byErrCode = byErrCode;
  msg.byCuttingNum = 0;

  if (!byErrCode)
  {
    msg.byLeftNum = byLeftOreNum;
    msg.dwLeftDalant = CPlayerDB::GetDalant(&m_Param);
    msg.dwConsumDalant = dwConsumDalant;

    unsigned __int8 count = 0;
    for (int j = 0; j < GetMaxResKind(); ++j)
    {
      if (m_Param.m_wCuttingResBuffer[j])
      {
        msg.ResList[count].wResIndex = j;
        msg.ResList[count++].byAddAmount = m_Param.m_wCuttingResBuffer[j];
      }
    }
    msg.byCuttingNum = count;
  }

  unsigned __int8 pbyType[2] = {14, 10};
  const unsigned __int16 nLen = msg.size();
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    nLen);
}

void CPlayer::SendMsg_OreIntoBagResult(
  char byErrCode,
  unsigned __int16 wNewSerial,
  unsigned __int8 byLendType,
  unsigned int dwLendTime)
{
  _ore_into_bag_result_zocl msg{};
  msg.byErrCode = byErrCode;
  msg.wNewSerial = wNewSerial;
  msg.byCsMethod = byLendType;
  msg.dwT = dwLendTime;

  unsigned __int8 pbyType[2] = {14, 12};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    8u);
}

void CPlayer::SendMsg_CuttingCompleteResult(unsigned __int8 byRet)
{
  _cutting_complete_result_zocl msg{};
  msg.dwLeftGold = CPlayerDB::GetGold(&m_Param);
  msg.byRet = byRet;

  unsigned __int8 pbyType[2] = {14, 14};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    5u);
}

void CPlayer::SendMsg_DeleteStorageInform(char byStorageCode, unsigned __int16 wSerial)
{
  struct
  {
    char storageCode;
    unsigned __int16 serial;
  } msg{};

  msg.storageCode = byStorageCode;
  msg.serial = wSerial;

  unsigned __int8 type[2]{};
  type[0] = 7;
  type[1] = 23;
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 3);
}

void CPlayer::SendMsg_BuddhaEventMsg(char byErrorCode)
{
  char msg[1]{};
  msg[0] = byErrorCode;

  unsigned __int8 type[2]{59, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 1u);
}

bool CPlayer::OutOfMap(CMapData *pIntoMap, unsigned __int16 wLayerIndex, unsigned __int8 byMapOutType, float *pfStartPos)
{
  if (!m_bInGuildBattle
      && m_byUserDgr >= 2
      && !m_bObserver
      && pIntoMap != m_pCurMap
      && pIntoMap->m_pMapSet->m_nRaceVillageCode < 3
      && static_cast<unsigned int>(m_Param.GetRaceCode()) != static_cast<unsigned int>(pIntoMap->m_pMapSet->m_nRaceVillageCode))
  {
    mgr_tracing(1);
  }

  if (m_pCurMap == g_TransportShip[m_Param.GetRaceCode()].m_pLinkShipMap)
  {
    const int raceCode = m_Param.GetRaceCode();
    g_TransportShip[raceCode].ExitMember(this, false);
  }

  m_bMapLoading = true;
  _AnimusReturn(3);
  DTradeInit();
  m_byMapInModeBuffer = byMapOutType;

  const unsigned int dwMapEnvCode = pIntoMap->m_Level.GetEnvironment();
  SendMsg_MapEnvInform(static_cast<char>(pIntoMap->m_pMapSet->m_dwIndex), dwMapEnvCode);
  SendMsg_MapOut(byMapOutType, static_cast<unsigned __int8>(pIntoMap->m_pMapSet->m_dwIndex));

  const unsigned int curSecNum = GetCurSecNum();
  m_pCurMap->ExitMap(this, curSecNum);
  SetCurSecNum(0xFFFFFFFF);

  if (pIntoMap->m_pMapSet->m_nMapType)
  {
    if (m_pUserDB)
    {
      m_pUserDB->Update_Map(m_pCurMap->m_pMapSet->m_dwIndex, m_fCurPos);
    }
  }
  else
  {
    if (m_pUserDB)
    {
      m_pUserDB->Update_Map(pIntoMap->m_pMapSet->m_dwIndex, pfStartPos);
    }
    memcpy_0(m_fCurPos, pfStartPos, sizeof(m_fCurPos));
  }

  if (CGameObject::s_pSelectObject == this)
  {
    CGameObject::s_pSelectObject = nullptr;
  }

  if (m_EP.GetEff_State(14))
  {
    RemoveSFContHelpByEffect(2, 14);
  }

  if (m_bMineMode)
  {
    m_bMineMode = false;
    m_dwMineNextTime = static_cast<unsigned int>(-1);
    SendMsg_MineCancle();
  }

  m_pCurMap = pIntoMap;
  m_wMapLayerIndex = wLayerIndex;
  m_Param.SetMapCode(pIntoMap->m_pMapSet->m_dwIndex);
  SetCurPos(pfStartPos);
  memcpy_0(m_fTarPos, m_fCurPos, sizeof(m_fTarPos));
  m_bMove = false;
  memcpy_0(m_fLastRecvPos, m_fCurPos, sizeof(m_fLastRecvPos));
  m_byLastRecvMapIndex = m_pCurMap->m_pMapSet->m_dwIndex;
  return true;
}

_STORAGE_LIST::_db_con *CPlayer::Emb_AddStorage(
  unsigned __int8 byStorageCode,
  _STORAGE_LIST::_storage_con *pCon,
  bool /*bEquipChange*/,
  bool bAdd)
{
  if (byStorageCode >= 8)
  {
    const char *charName = m_Param.GetCharNameA();
    g_Main.m_logSystemError.Write(
      "%s: Emb_AddStorage.. total_storage_num is over. storage: %d, item: %d-%d: ",
      charName,
      byStorageCode,
      pCon->m_byTableCode,
      pCon->m_wItemIndex);
    return nullptr;
  }

  unsigned int storageIndex = m_Param.m_pStoragePtr[byStorageCode]->TransInCon(pCon);
  if (storageIndex == 255)
  {
    const char *charName = m_Param.GetCharNameA();
    g_Main.m_logSystemError.Write(
      "%s: Emb_AddStorage.. TransInCon()error storage: %d, item: %d-%d: ",
      charName,
      byStorageCode,
      pCon->m_byTableCode,
      pCon->m_wItemIndex);
    return nullptr;
  }

  _STORAGE_LIST::_db_con *pkItem = &m_Param.m_pStoragePtr[byStorageCode]->m_pStorageList[storageIndex];
  if (!pkItem)
  {
    const char *charName = m_Param.GetCharNameA();
    g_Main.m_logSystemError.Write(
      "%s: Emb_AddStorage.. Item is not exist. storage: %d, item: %d-%d: ",
      charName,
      byStorageCode,
      pCon->m_byTableCode,
      pCon->m_wItemIndex);
    return nullptr;
  }

  if (!pkItem->m_lnUID)
  {
    pkItem->m_lnUID = UIDGenerator::getuid(g_Main.m_byWorldCode);
  }

  if (pkItem->m_byCsMethod)
  {
    if (!LendItemMng::Instance()->InsertLink(m_ObjID.m_wIndex, byStorageCode, pkItem))
    {
      const char *charName = m_Param.GetCharNameA();
      g_Main.m_logSystemError.Write(
        "%s: Emb_AddStorage.. InsertLink ()error storage: %d, item: %d-%d: ",
        charName,
        byStorageCode,
        pCon->m_byTableCode,
        pCon->m_wItemIndex);
      return nullptr;
    }
  }

  if (m_pUserDB)
  {
    m_pUserDB->Update_ItemAdd(byStorageCode, storageIndex, pkItem, bAdd);
  }

  if (byStorageCode == 1 || byStorageCode == 2)
  {
    if (pkItem->m_byTableCode != 10)
    {
      SetEquipEffect(reinterpret_cast<_STORAGE_LIST::_db_con *>(pCon), true);
    }
    SetEffectEquipCode(byStorageCode, static_cast<unsigned __int8>(storageIndex), 1);
  }

  if ((byStorageCode == 1 || byStorageCode == 2)
      && (pkItem->m_byTableCode < 6u
        || pkItem->m_byTableCode == 8
        || pkItem->m_byTableCode == 7
        || pkItem->m_byTableCode == 9))
  {
    CalcDefTol();
  }

  if (byStorageCode == 1)
  {
    CashChangeStateFlag changeFlag(0);
    UpdateVisualVer(changeFlag);
    SendMsg_EquipPartChange(pCon->m_byTableCode);
    if (pkItem->m_byTableCode == 6)
    {
      if (!IsRidingUnit())
      {
        m_pmWpn.FixWeapon(pkItem);
      }
      if (m_bMineMode)
      {
        m_bMineMode = false;
        m_dwMineNextTime = static_cast<unsigned int>(-1);
        SendMsg_MineCancle();
      }
      if (IsSiegeMode())
      {
        SetSiege(nullptr);
      }
    }
    CalcEquipSpeed();
    CalcEquipMaxDP(0);
  }

  if (!byStorageCode && pkItem->m_byTableCode == 18)
  {
    _base_fld *record = g_Main.m_tblItemData[pkItem->m_byTableCode].GetRecord(pkItem->m_wItemIndex);
    auto *fld = reinterpret_cast<_ResourceItem_fld *>(record);
    if (fld && fld->m_nEffectDataNum > 0)
    {
      SetHaveEffect(0);
      if (!fld->m_nEffType1)
      {
        SetMstHaveEffect(fld, pkItem, 1, 0);
      }
    }
  }

  if (byStorageCode == 5)
  {
    const unsigned __int8 raceCode = m_Param.GetRaceCode();
    m_Param.m_dbTrunk.m_byItemSlotRace[storageIndex] = raceCode;
  }
  if (byStorageCode == 7)
  {
    const unsigned __int8 raceCode = m_Param.GetRaceCode();
    m_Param.m_dbExtTrunk.m_byItemSlotRace[storageIndex] = raceCode;
  }

  return pkItem;
}

bool CPlayer::Emb_DelStorage(
  unsigned __int8 byStorageCode,
  unsigned __int8 byStorageIndex,
  bool bEquipChange,
  bool bDelete,
  const char *strErrorCodePos)
{
  if (byStorageCode >= 8)
  {
    const char *charName = m_Param.GetCharNameA();
    if (strErrorCodePos)
    {
      g_Main.m_logSystemError.Write(
        "%s: Emb_DelStorage.. Storage Code Over storage: %d, slot: %d: CodePos: %s",
        charName,
        byStorageCode,
        byStorageIndex,
        strErrorCodePos);
    }
    else
    {
      g_Main.m_logSystemError.Write(
        "%s: Emb_DelStorage.. Storage Code Over storage: %d, slot: %d: ",
        charName,
        byStorageCode,
        byStorageIndex);
    }
    return false;
  }

  _STORAGE_LIST::_db_con *pkItem = &m_Param.m_pStoragePtr[byStorageCode]->m_pStorageList[byStorageIndex];
  if (pkItem->m_byCsMethod)
  {
    if (!LendItemMng::Instance()->DeleteLink(m_ObjID.m_wIndex, byStorageCode, pkItem))
    {
      return false;
    }
  }

  if (!m_Param.m_pStoragePtr[byStorageCode]->EmptyCon(byStorageIndex))
  {
    const char *charName = m_Param.GetCharNameA();
    if (strErrorCodePos)
    {
      g_Main.m_logSystemError.Write(
        "%s: Emb_DelStorage.. EmptyCon()error storage: %d, slot: %d: CodePos: %s",
        charName,
        byStorageCode,
        byStorageIndex,
        strErrorCodePos);
    }
    else
    {
      g_Main.m_logSystemError.Write(
        "%s: Emb_DelStorage.. EmptyCon()error storage: %d, slot: %d: ",
        charName,
        byStorageCode,
        byStorageIndex);
    }
    return false;
  }

  if (m_pUserDB)
  {
    m_pUserDB->Update_ItemDelete(byStorageCode, byStorageIndex, bDelete);
  }

  if (byStorageCode == 1 || byStorageCode == 2)
  {
    if (pkItem->m_byTableCode != 10 && GetEffectEquipCode(byStorageCode, byStorageIndex) == 1)
    {
      SetEquipEffect(pkItem, false);
    }
    SetEffectEquipCode(byStorageCode, byStorageIndex, 0);
    if (pkItem->m_byTableCode < 6u
        || pkItem->m_byTableCode == 8
        || pkItem->m_byTableCode == 7
        || pkItem->m_byTableCode == 9)
    {
      CalcDefTol();
    }
  }

  if (byStorageCode == 1)
  {
    EquipItemSFAgent.ReleaseSFCont(pkItem->m_byTableCode);
    if (!bEquipChange)
    {
      CashChangeStateFlag changeFlag(0);
      UpdateVisualVer(changeFlag);
      SendMsg_EquipPartChange(pkItem->m_byTableCode);
      if (pkItem->m_byTableCode == 6 && !IsRidingUnit())
      {
        m_pmWpn.FixWeapon(nullptr);
      }
    }

    if (pkItem->m_byTableCode == 6 && m_EP.GetEff_State(14))
    {
      RemoveSFContHelpByEffect(2, 14);
    }

    CalcEquipSpeed();
    CalcEquipMaxDP(0);

    if (pkItem->m_byTableCode == 6)
    {
      if (m_bMineMode)
      {
        m_bMineMode = false;
        m_dwMineNextTime = static_cast<unsigned int>(-1);
        SendMsg_MineCancle();
      }
      if (IsSiegeMode())
      {
        SetSiege(nullptr);
      }
    }
  }

  if (!byStorageCode)
  {
    if (pkItem->m_byTableCode == 18)
    {
      _base_fld *record = g_Main.m_tblItemData[pkItem->m_byTableCode].GetRecord(pkItem->m_wItemIndex);
      auto *fld = reinterpret_cast<_ResourceItem_fld *>(record);
      if (fld && fld->m_nEffectDataNum > 0)
      {
        SetHaveEffect(0);
        if (!fld->m_nEffType1)
        {
          SetMstHaveEffect(fld, pkItem, 0, 0);
        }
        if (fld->m_nStartTime != -1)
        {
          if (!TimeLimitJadeMng::Instance()->DeleteList(m_ObjID.m_wIndex, pkItem))
          {
            const char *charName = m_Param.GetCharNameA();
            g_Main.m_logSystemError.Write(
              "%s: Emb_DelStorage.. TimeLimitJadeMng::DeleteList() error storage: %d, item: %d-%d: ",
              charName,
              0,
              pkItem->m_byTableCode,
              pkItem->m_wItemIndex);
          }
        }
      }
    }
    if (IsSiegeMode() && pkItem == m_pSiegeItem)
    {
      SetSiege(nullptr);
    }
  }

  if (bDelete)
  {
    SendMsg_DeleteStorageInform(byStorageCode, pkItem->m_wSerial);
  }
  return true;
}

unsigned __int64 CPlayer::Emb_AlterDurPoint(
  unsigned __int8 byStorageCode,
  unsigned __int8 byStorageIndex,
  int nAlter,
  bool bUpdate,
  bool bSend)
{
  int alter = nAlter;
  _STORAGE_LIST::_db_con *pkItem = &m_Param.m_pStoragePtr[byStorageCode]->m_pStorageList[byStorageIndex];
  if (!pkItem->m_bLoad)
  {
    return 0;
  }

  if (pkItem->m_byTableCode == 15 && nAlter > 0 && pkItem->m_dwDur + nAlter > 0xFFFFFFu)
  {
    alter = static_cast<int>(0xFFFFFFu - pkItem->m_dwDur);
    if (alter <= 0)
    {
      return static_cast<unsigned int>(pkItem->m_dwDur);
    }
  }

  const unsigned __int64 oldDur = pkItem->m_dwDur;
  unsigned __int64 leftDur = 0;
  const unsigned __int16 itemSerial = pkItem->m_wSerial;

  if (alter < 0 && !(pkItem->m_dwDur + alter) && pkItem->m_byCsMethod)
  {
    LendItemMng::Instance()->DeleteLink(m_ObjID.m_wIndex, byStorageCode, pkItem);
  }

  if (m_Param.m_pStoragePtr[byStorageCode]->AlterCurDur(byStorageIndex, alter, &leftDur))
  {
    if (!byStorageCode && pkItem->m_byTableCode == 18)
    {
      auto *fld = reinterpret_cast<_ResourceItem_fld *>(
        g_Main.m_tblItemData[pkItem->m_byTableCode].GetRecord(pkItem->m_wItemIndex));
      if (fld->m_nEffectDataNum > 0)
      {
        SetHaveEffect(0);
        if (!fld->m_nEffType1)
        {
          if (oldDur <= leftDur)
          {
            SetMstHaveEffect(fld, pkItem, 1, alter);
          }
          else
          {
            SetMstHaveEffect(fld, pkItem, 0, alter);
          }
        }
        if (fld->m_nStartTime != -1)
        {
          if (!TimeLimitJadeMng::Instance()->DeleteList(m_ObjID.m_wIndex, pkItem))
          {
            const char *charName = m_Param.GetCharNameA();
            g_Main.m_logSystemError.Write(
              "%s: Emb_AlterDurPoint.. TimeLimitJadeMng::DeleteList() error storage: %d, item[%s]: %d-%d: ",
              charName,
              byStorageCode,
              fld->m_strCode,
              pkItem->m_byTableCode,
              pkItem->m_wItemIndex);
            return static_cast<unsigned int>(pkItem->m_dwDur);
          }
        }
      }
    }

    if (leftDur)
    {
      if (oldDur != leftDur)
      {
        if (bSend)
        {
          SendMsg_AlterItemDurInform(byStorageCode, itemSerial, leftDur);
        }
        if (m_pUserDB)
        {
          m_pUserDB->Update_ItemDur(byStorageCode, byStorageIndex, leftDur, bUpdate);
        }
      }
    }
    else
    {
      if (m_pUserDB)
      {
        m_pUserDB->Update_ItemDelete(byStorageCode, byStorageIndex, bUpdate);
      }
      if (byStorageCode == 1)
      {
        if (GetEffectEquipCode(byStorageCode, byStorageIndex) == 1)
        {
          SetEquipEffect(pkItem, false);
        }
        SetEffectEquipCode(byStorageCode, byStorageIndex, 0);
      }
      if ((byStorageCode == 1 || byStorageCode == 2)
          && (pkItem->m_byTableCode < 6 || pkItem->m_byTableCode == 8 || pkItem->m_byTableCode == 9))
      {
        CalcDefTol();
      }
      if (byStorageCode == 1)
      {
        CashChangeStateFlag changeFlag(0);
        UpdateVisualVer(changeFlag);
        SendMsg_EquipPartChange(pkItem->m_byTableCode);
        if (IsSiegeMode())
        {
          SetSiege(nullptr);
        }
        CalcEquipSpeed();
        CalcEquipMaxDP(0);
      }
      else if (!byStorageCode && IsSiegeMode() && pkItem == m_pSiegeItem)
      {
        SetSiege(nullptr);
      }
      SendMsg_DeleteStorageInform(byStorageCode, itemSerial);
    }

    return static_cast<unsigned int>(leftDur);
  }

  const char *charName = m_Param.GetCharNameA();
  g_Main.m_logSystemError.Write(
    "%s: Emb_AlterDurPoint.. AlterCurDur() error storage: %d, slot: %d: ",
    charName,
    byStorageCode,
    byStorageIndex);
  return static_cast<unsigned int>(pkItem->m_dwDur);
}

unsigned __int16 CPlayer::GetVisualVer()
{
  return m_wVisualVer;
}

void CPlayer::UpdateVisualVer(CashChangeStateFlag byChangeFlagMask)
{
  ++m_wVisualVer;
  if ((byChangeFlagMask.m_byStateFlag & 7) != 0)
  {
    const unsigned __int8 current = m_CashChangeStateFlag.m_byStateFlag;
    const unsigned __int8 updated =
      static_cast<unsigned __int8>(((byChangeFlagMask.m_byStateFlag & 7) + (current & 7)) & 7);
    m_CashChangeStateFlag.m_byStateFlag = static_cast<unsigned __int8>((current & 0xF8) | updated);
  }
  SetShapeAllBuffer();
}

void CPlayer::SetShapeAllBuffer()
{
  m_bufShapeAll.wIndex = m_ObjID.m_wIndex;
  m_bufShapeAll.dwSerial = m_dwObjSerial;
  m_bufShapeAll.wEquipVer = GetVisualVer();
  m_bufShapeAll.byRecIndex = m_Param.GetRaceSexCode();
  m_bufShapeAll.byFaceIndex = m_Param.m_dbChar.m_byDftPart_Face;
  m_bufShapeAll.byHairIndex = m_Param.m_dbChar.m_byDftPart[4];
  m_bufShapeAll.byCashChangeStateFlag = m_CashChangeStateFlag.m_byStateFlag;

  for (int j = 0; j < 8; ++j)
  {
    if (m_Param.m_dbEquip.m_pStorageList[j].m_bLoad)
    {
      m_bufShapeAll.ModelPerPart[j].wPartIndex = m_Param.m_dbEquip.m_pStorageList[j].m_wItemIndex;
      m_bufShapeAll.ModelPerPart[j].byLv = GetItemUpgedLv(m_Param.m_dbEquip.m_pStorageList[j].m_dwLv);
    }
    else if (j >= 5)
    {
      m_bufShapeAll.ModelPerPart[j].wPartIndex = static_cast<unsigned __int16>(-1);
    }
    else
    {
      m_bufShapeAll.ModelPerPart[j].wPartIndex = m_Param.m_dbChar.m_byDftPart[j];
      m_bufShapeAll.ModelPerPart[j].byLv = 0;
    }
  }

  m_bufShapeAll.byUserGrade = m_bSpyGM ? 0 : m_byUserDgr;
  if (m_Param.m_pGuild)
  {
    m_bufShapeAll.dwGuildSerial = m_Param.m_pGuild->m_dwSerial;
    const int raceCode = m_Param.GetRaceCode();
    CHonorGuild *honorGuild = CHonorGuild::Instance();
    m_bufShapeAll.byHonorGuildRank = honorGuild->FindHonorGuildRank(raceCode, m_Param.m_pGuild->m_dwSerial);
  }
  else
  {
    m_bufShapeAll.dwGuildSerial = static_cast<unsigned int>(-1);
    m_bufShapeAll.byHonorGuildRank = static_cast<unsigned __int8>(-1);
  }

  const char *charName = m_Param.GetCharNameW();
  strcpy_0(m_bufShapeAll.wszName, charName);
  m_bufShapeAll.bySpecialPart = static_cast<unsigned __int8>(-1);

  if (IsRidingUnit())
  {
    m_bufShapeAll.bySpecialPart = 0;
  }
  else if (m_pSiegeItem)
  {
    m_bufShapeAll.bySpecialPart = 1;
  }
  else if (m_bTakeSoccerBall)
  {
    m_bufShapeAll.bySpecialPart = 2;
  }
  else if (m_byMoveType == 2)
  {
    m_bufShapeAll.bySpecialPart = 3;
  }

  if (m_bufShapeAll.bySpecialPart)
  {
    if (m_bufShapeAll.bySpecialPart == 1)
    {
      m_bufShapeAll.byFrameIndex = m_pSiegeItem->m_wItemIndex;
    }
    else if (m_bufShapeAll.bySpecialPart == 2)
    {
      m_bufShapeAll.byFrameIndex = m_pSoccerItem->m_wItemIndex;
    }
  }
  else
  {
    m_bufShapeAll.byFrameIndex = m_pUsingUnit->byFrame;
    memcpy_0(m_bufShapeAll.byUnitPartIndex, m_pUsingUnit->byPart, sizeof(m_bufShapeAll.byUnitPartIndex));
  }

  m_bufShapeAll.byColor = m_byGuildBattleColorInx;

  m_bufSpapePart.wIndex = m_ObjID.m_wIndex;
  m_bufSpapePart.dwSerial = m_dwObjSerial;
  m_bufSpapePart.wEquipVer = GetVisualVer();
  m_bufSpapePart.byCashChangeStateFlag = m_CashChangeStateFlag.m_byStateFlag;

  for (int j = 0; j < 8; ++j)
  {
    if (m_Param.m_dbEquip.m_pStorageList[j].m_bLoad)
    {
      m_bufSpapePart.ModelPerPart[j].wPartIndex = m_Param.m_dbEquip.m_pStorageList[j].m_wItemIndex;
      m_bufSpapePart.ModelPerPart[j].byLv = GetItemUpgedLv(m_Param.m_dbEquip.m_pStorageList[j].m_dwLv);
    }
    else if (j >= 5)
    {
      m_bufSpapePart.ModelPerPart[j].wPartIndex = static_cast<unsigned __int16>(-1);
    }
    else
    {
      m_bufSpapePart.ModelPerPart[j].wPartIndex = m_Param.m_dbChar.m_byDftPart[j];
      m_bufSpapePart.ModelPerPart[j].byLv = 0;
    }
  }

  m_bufSpapePart.bySpecialPart = static_cast<unsigned __int8>(-1);
  if (IsRidingUnit())
  {
    m_bufSpapePart.bySpecialPart = 0;
  }
  else if (m_pSiegeItem)
  {
    m_bufSpapePart.bySpecialPart = 1;
  }
  else if (m_bTakeSoccerBall)
  {
    m_bufSpapePart.bySpecialPart = 2;
  }
  else if (m_byMoveType == 2)
  {
    m_bufSpapePart.bySpecialPart = 3;
  }

  if (m_bufSpapePart.bySpecialPart)
  {
    if (m_bufSpapePart.bySpecialPart == 1)
    {
      m_bufSpapePart.byFrameIndex = m_pSiegeItem->m_wItemIndex;
    }
    else if (m_bufSpapePart.bySpecialPart == 2)
    {
      m_bufSpapePart.byFrameIndex = m_pSoccerItem->m_wItemIndex;
    }
  }
  else
  {
    m_bufSpapePart.byFrameIndex = m_pUsingUnit->byFrame;
    memcpy_0(m_bufSpapePart.byUnitPartIndex, m_pUsingUnit->byPart, sizeof(m_bufSpapePart.byUnitPartIndex));
  }

  if (m_Param.m_pGuild)
  {
    const int raceCode = m_Param.GetRaceCode();
    CHonorGuild *honorGuild = CHonorGuild::Instance();
    m_bufSpapePart.byHonorGuildRank =
      honorGuild->FindHonorGuildRank(raceCode, m_Param.m_pGuild->m_dwSerial);
  }
  else
  {
    m_bufSpapePart.byHonorGuildRank = static_cast<unsigned __int8>(-1);
  }
}

void CPlayer::SendMsg_EquipPartChange(unsigned __int8 byPart)
{
#pragma pack(push, 1)
  struct EquipPartChangeMsg
  {
    unsigned __int16 wIndex;
    unsigned int dwSerial;
    unsigned __int16 wEquipVer;
    unsigned __int8 byPart;
    unsigned __int16 wPartIndex;
    unsigned __int8 byLv;
  };
#pragma pack(pop)

  EquipPartChangeMsg msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  msg.wEquipVer = GetVisualVer();
  msg.byPart = byPart;

  if (m_Param.m_dbEquip.m_pStorageList[byPart].m_bLoad)
  {
    msg.wPartIndex = m_Param.m_dbEquip.m_pStorageList[byPart].m_wItemIndex;
    msg.byLv = GetItemUpgedLv(m_Param.m_dbEquip.m_pStorageList[byPart].m_dwLv);
  }
  else if (byPart >= 5u)
  {
    msg.wPartIndex = static_cast<unsigned __int16>(-1);
  }
  else
  {
    msg.wPartIndex = m_Param.m_dbChar.m_byDftPart[byPart];
    msg.byLv = 0;
  }

  unsigned __int8 type[2] = {3, 34};
  CircleReport(type, reinterpret_cast<char *>(&msg), sizeof(msg), false);
}

bool CPlayer::IsRidingUnit()
{
  return m_pUsingUnit && !m_pParkingUnit;
}

bool CPlayer::IsSiegeMode()
{
  return m_pSiegeItem != nullptr;
}

bool CPlayer::IsActingSiegeMode()
{
  return m_bIsSiegeActing;
}

void CPlayer::DTradeInit()
{
  if (m_pmTrd.bDTradeMode)
  {
    CPlayer *dst = nullptr;
    if (DTradeEqualPerson(this, &dst))
    {
      dst->m_pmTrd.Init();
      dst->SendMsg_DTradeCancleInform();
    }
    m_pmTrd.Init();
    SendMsg_DTradeCancleInform();
  }
}

void CPlayer::SendMsg_DTradeCancleInform()
{
  char msg = 0;
  unsigned __int8 type[2] = {18, 9};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, &msg, 1u);
}

void CPlayer::SendMsg_DTradeCloseInform(char byCloseCode)
{
  char msg[1]{};
  msg[0] = byCloseCode;

  unsigned __int8 type[2] = {18, 26};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::NetClose(bool bMoveOutLobby)
{
  if (m_byPatriarchAppointPropose != 255)
  {
    char pdata[16];
    pdata[0] = 1;
    PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
    processor->Doit(_eRespAppoint, this, pdata);
  }

  if (m_Param.m_pAPM)
  {
    m_Param.m_pAPM->unregist_from_map(2u);
  }
  m_Param.m_bPersonalAmineInven = 0;

  LendItemMng::Instance()->Release(m_ObjID.m_wIndex);

  if (m_bLoad && m_bOper)
  {
    g_Main.m_pTimeLimitMgr->Pop_Data(m_pUserDB->m_dwAccountSerial, m_id.wIndex);
    g_Main.m_logSystemError.Write(
      "CPlayer::NetClose() : Time Limit System Stop!! : ID = %s",
      m_pUserDB->m_szAccountID);
  }

  TimeLimitJadeMng::Instance()->Release(m_ObjID.m_wIndex);

  if (m_bLive)
  {
    DTradeInit();
    _TowerAllReturn(2u, false);
    _AnimusReturn(2u);
    ForcePullUnit(true);

    for (int trapIndex = 0; trapIndex < 20; ++trapIndex)
    {
      _TRAP_PARAM::_param *trapParam = &m_pmTrp.m_Item[trapIndex];
      if (trapParam->isLoad() && trapParam->pItem->m_dwObjSerial == trapParam->dwSerial)
      {
        const long double pvpPoint = m_Param.GetPvPPoint();
        trapParam->pItem->MasterNetClose(pvpPoint);
      }
    }

    CExchangeEvent *exchangeEvent = CExchangeEvent::Instance();
    if (exchangeEvent->IsDelete() || CExchangeEvent::Instance()->IsEnable())
    {
      CExchangeEvent::Instance()->DeleteExchangeEventItem(this);
    }

    CPcBangFavor::Instance()->PcBangDeleteItem(this);

    if (m_pCurMap == g_TransportShip[m_Param.GetRaceCode()].m_pLinkShipMap)
    {
      const int raceCode = m_Param.GetRaceCode();
      g_TransportShip[raceCode].ExitMember(this, true);
    }

    if (g_HolySys.GetSceneCode() == 1 && m_byHSKQuestCode != 100)
    {
      g_HolySys.PushStoreQuestCash(
        m_dwObjSerial,
        m_byHSKQuestCode,
        m_nHSKPvpPoint,
        m_wKillPoint,
        m_wDiePoint,
        m_byCristalBattleDBInfo,
        m_byHSKTime);
    }

    if (GetLevel() >= 25 && (g_HolySys.m_bScheduleCodePre == 1 || g_HolySys.GetSceneCode() == 1))
    {
      g_HolySys.PushQuestCash_Other(m_dwObjSerial, m_byStoneMapMoveInfo);
    }

    CGuildBattleController::Instance()->NetClose(this);

    if (m_Param.m_pGuild && m_EP.GetEff_Have(50) <= 0.0)
    {
      m_Param.m_pGuild->LogoffMember(m_dwObjSerial);
      m_Param.m_pGuild->SendMsg_GuildMemberLogoff(m_dwObjSerial);
    }
    if (m_Param.m_pApplyGuild && m_EP.GetEff_Have(50) <= 0.0)
    {
      m_Param.m_pApplyGuild->PopApplier(m_dwObjSerial, 2u);
    }

    SendMsg_Destroy();
    CCharacter::Destroy();

    bool needMove = false;
    if (m_bOutOfMap || m_bCheckMovePacket)
    {
      needMove = true;
    }
    else if (g_Main.m_bServerClosing)
    {
      needMove = false;
    }
    else if (m_bOper)
    {
      needMove = false;
    }
    else if (m_Param.GetLevel() <= 7 && GetHP() > 0)
    {
      needMove = true;
    }

    if (g_HolySys.IsControlScene() && !strncmp(m_pCurMap->m_pMapSet->m_strCode, "resources", 9u))
    {
      needMove = true;
    }

    float bindPos[8];
    if (!m_pDHChannel)
    {
      if (needMove)
      {
        CMapData *bindMap = GetBindMap(bindPos, false);
        if (bindMap)
        {
          m_pCurMap = bindMap;
          memcpy_0(m_fCurPos, bindPos, sizeof(m_fCurPos));
        }
      }
    }
    if (m_pDHChannel)
    {
      _dh_player_mgr::_pos outPos;
      m_pDHChannel->ClearMember(this, m_bOper, &outPos);
      m_pCurMap = outPos.pMap;
      memcpy_0(m_fCurPos, outPos.fPos, sizeof(m_fCurPos));
    }

    m_Param.SetMapCode(m_pCurMap->m_pMapSet->m_dwIndex);
    m_Param.SetCurPos(m_fCurPos);
    ExitUpdateDataToWorld();

    --s_nLiveNum;
    --s_nRaceNum[m_Param.GetRaceCode()];

    CUnmannedTraderController *traderController = CUnmannedTraderController::Instance();
    const unsigned __int8 maxRegistCnt =
      traderController->GetMaxRegistCnt(m_ObjID.m_wIndex, m_dwObjSerial);
    const CUnmannedTraderRegistItemInfo *regItemInfo =
      traderController->GetRegItemInfo(m_ObjID.m_wIndex, m_dwObjSerial);

    s_MgrItemHistory.have_item_close(
      m_ObjID.m_wIndex,
      m_Param.GetCharNameA(),
      &m_pUserDB->m_AvatorData,
      &m_pUserDB->m_AvatorData_bk,
      m_pUserDB->m_szAccountID,
      m_pUserDB->m_dwAccountSerial,
      m_byUserDgr,
      m_pUserDB->m_ipAddress,
      m_dwExpRate,
      regItemInfo,
      maxRegistCnt,
      m_szItemHistoryFileName);

    traderController->LogOut(m_ObjID.m_wIndex, m_dwObjSerial);

    s_MgrItemHistory.post_storage(&m_Param.m_PostStorage, m_szItemHistoryFileName);
    s_MgrItemHistory.return_post_storage(&m_Param.m_ReturnPostStorage, m_szItemHistoryFileName);

    CMoveMapLimitManager::Instance()->LogOut(this);
    wa_ExitWorld(&m_id);

    s_MgrLvHistory.update_mastery(
      m_ObjID.m_wIndex,
      m_byUserDgr,
      m_Param.GetLevel(),
      m_Param.GetExp(),
      m_dwExpRate,
      m_Param.m_byPvPGrade,
      m_nMaxPoint,
      &m_pmMst,
      m_Param.m_dwAlterMastery,
      m_szLvHistoryFileName,
      1u,
      nullptr);

    ++s_dwTotalCloseCount;
    if (m_pUserDB)
    {
      if (m_bOper)
      {
        s_MgrItemHistory.close(m_ObjID.m_wIndex, const_cast<char *>("Abnormal"), m_szItemHistoryFileName);
        ++s_dwAbnormalCloseCount;
      }
      else
      {
        s_MgrItemHistory.close(m_ObjID.m_wIndex, const_cast<char *>("Normal"), m_szItemHistoryFileName);
      }
    }

    if (!m_pUserDB->m_byUserDgr && !m_pUserDB->m_bBillingNoLogout)
    {
      CTSingleton<CBillingManager>::Instance()->Logout(m_pUserDB);
    }

    if (m_EP.GetEff_Have(50) <= 0.0)
    {
      for (int buddyIndex = 0; buddyIndex < 50; ++buddyIndex)
      {
        _BUDDY_LIST::__list *buddyList = &m_pmBuddy.m_List[buddyIndex];
        if (buddyList->fill()
            && buddyList->pPtr
            && buddyList->pPtr->m_pmBuddy.SearchBuddyLogoff(m_dwObjSerial))
        {
          buddyList->pPtr->SendMsg_BuddyLogoffInform(m_dwObjSerial);
        }
      }
    }
  }

  if (m_Param.m_pGuild)
  {
    CGuildRoomSystem *roomSystem = CGuildRoomSystem::GetInstance();
    roomSystem->RoomOut(m_Param.m_pGuild->m_dwSerial, m_ObjID.m_wIndex, m_pUserDB->m_dwSerial);
  }

  const DWORD limitTime = m_AttDelayChker.m_nNextAddTime + timeGetTime();
  if (m_pUserDB)
  {
    for (int index = 0; index < 10; ++index)
    {
      _ATTACK_DELAY_CHECKER::_eff_list *eff = &m_AttDelayChker.EFF[index];
      if (eff->byEffectCode == 255 || eff->dwNextTime <= limitTime)
      {
        m_pUserDB->m_AvatorData.dbSFDelay.EFF[index].byEffectCode = static_cast<unsigned __int8>(-1);
        m_pUserDB->m_AvatorData.dbSFDelay.EFF[index].wEffectIndex = 0;
        m_pUserDB->m_AvatorData.dbSFDelay.EFF[index].dwNextTime = 0;
      }
      else
      {
        m_pUserDB->m_AvatorData.dbSFDelay.EFF[index].byEffectCode = eff->byEffectCode;
        m_pUserDB->m_AvatorData.dbSFDelay.EFF[index].wEffectIndex = eff->wEffectIndex;
        m_pUserDB->m_AvatorData.dbSFDelay.EFF[index].dwNextTime = eff->dwNextTime - limitTime;
      }

      _ATTACK_DELAY_CHECKER::_mas_list *mas = &m_AttDelayChker.MAS[index];
      if (mas->byEffectCode == 255 || mas->dwNextTime <= limitTime)
      {
        m_pUserDB->m_AvatorData.dbSFDelay.MAS[index].byEffectCode = static_cast<unsigned __int8>(-1);
        m_pUserDB->m_AvatorData.dbSFDelay.MAS[index].byMastery = 0;
        m_pUserDB->m_AvatorData.dbSFDelay.MAS[index].dwNextTime = 0;
      }
      else
      {
        m_pUserDB->m_AvatorData.dbSFDelay.MAS[index].byEffectCode = mas->byEffectCode;
        m_pUserDB->m_AvatorData.dbSFDelay.MAS[index].byMastery = mas->byMastery;
        m_pUserDB->m_AvatorData.dbSFDelay.MAS[index].dwNextTime = mas->dwNextTime - limitTime;
      }
    }
  }

  if (IsApplyPcbangPrimium())
  {
    if (bMoveOutLobby || m_bOper)
    {
      m_kPcBangCoupon.LogOut(true);
    }
    else
    {
      m_kPcBangCoupon.LogOut(false);
    }
  }

  CTSingleton<CNationSettingManager>::Instance()->NetClose(this);
  m_Param.InitPlayerDB(this);

  m_byUserDgr = 0;
  m_bCorpse = false;
  m_bLoad = false;
  m_bOper = false;
  m_bMapLoading = false;
  m_bFullMode = false;
  m_pUserDB = nullptr;
  m_pUsingUnit = nullptr;
  m_pParkingUnit = nullptr;
  m_pRecalledAnimusItem = nullptr;
  m_pRecalledAnimusChar = nullptr;
  m_dwLastRecallTime = 0;
  m_byNextRecallReturn = static_cast<unsigned __int8>(-1);
  m_bBaseDownload = true;
  m_bInvenDownload = true;
  m_bForceDownload = true;
  m_bCumDownload = true;
  m_bSpecialDownload = true;
  m_bQuestDownload = true;
  m_bLinkBoardDownload = true;
  m_bRecvMapChat = false;
  m_bSpyGM = false;
  m_clsSetItem.Init_Info();
  m_id.dwSerial = static_cast<unsigned int>(-1);
  m_dwObjSerial = static_cast<unsigned int>(-1);
  m_bTakeGravityStone = false;
  m_bBlockGuildBattleMsg = false;
  m_bInGuildBattle = false;
  m_bNotifyPosition = false;
  m_byGuildBattleColorInx = static_cast<unsigned __int8>(-1);
  m_bTakeSoccerBall = false;
  m_pSoccerItem = nullptr;
  m_nChaosMode = 0;
  m_dwChaosModeTime10Per = 0;
  m_dwChaosModeEndTime = 0;
  m_bSnowMan = false;
  m_bAfterEffect = false;
}

void CPlayer::_AnimusReturn(unsigned __int8 byReturnType)
{
  if (!m_pRecalledAnimusChar)
  {
    return;
  }

  const unsigned __int16 serial = m_pRecalledAnimusItem->m_wSerial;
  if (byReturnType == 3)
  {
    m_wTimeFreeRecallSerial = m_pRecalledAnimusItem->m_wSerial;
  }
  else
  {
    m_wTimeFreeRecallSerial = static_cast<unsigned __int16>(-1);
  }
  m_dwLastRecallTime = GetLoopTime();
  m_pRecalledAnimusItem->lock(false);
  s_AnimusReturnDelay.Delete(m_ObjID.m_wIndex, m_dwObjSerial);
  m_pRecalledAnimusChar->Destroy();
  m_pRecalledAnimusItem = nullptr;
  m_pRecalledAnimusChar = nullptr;
  SendMsg_AnimusReturnResult(0, serial, byReturnType);
}

void CPlayer::SendMsg_AnimusReturnResult(
  char byRetCode,
  unsigned __int16 wAnimusItemSerial,
  unsigned __int8 byReturnType)
{
#pragma pack(push, 1)
  struct AnimusReturnMsg
  {
    char byRetCode;
    unsigned __int16 wAnimusItemSerial;
    unsigned __int8 byReturnType;
  };
#pragma pack(pop)

  AnimusReturnMsg msg{};
  msg.byRetCode = byRetCode;
  msg.wAnimusItemSerial = wAnimusItemSerial;
  msg.byReturnType = byReturnType;

  unsigned __int8 type[2] = {22, 4};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 4u);
}

void CPlayer::pc_AnimusRecallRequest(
  unsigned __int16 wAnimusItemSerial,
  unsigned __int16 wAnimusClientHP,
  unsigned __int16 wAnimusClientFP)
{
  unsigned __int8 resultCode = 0;
  _STORAGE_LIST::_db_con *animusItem = nullptr;
  _animus_fld *animusField = nullptr;
  unsigned int *animusLvParam = nullptr;
  CAnimus *newAnimus = nullptr;
  _base_fld *record = nullptr;
  unsigned __int16 animusHP = 0;
  unsigned __int16 animusFP = 0;

  if (m_bObserver)
  {
    resultCode = 13;
  }
  else if (m_bInGuildBattle && m_bTakeGravityStone)
  {
    resultCode = 14;
  }
  else if (CPlayerDB::GetRaceCode(&m_Param) == 1)
  {
    if (CGameObject::GetCurSecNum(this) == -1 || m_bMapLoading)
    {
      resultCode = 6;
    }
    else if (m_pRecalledAnimusItem)
    {
      resultCode = 1;
    }
    else
    {
      for (int index = 0; index < 4; ++index)
      {
        _STORAGE_LIST::_db_con *entry = &m_Param.m_dbAnimus.m_pStorageList[index];
        if (entry->m_bLoad && entry->m_wSerial == wAnimusItemSerial)
        {
          animusItem = entry;
          break;
        }
      }

      if (animusItem)
      {
        if (animusItem->m_bLock)
        {
          resultCode = 13;
        }
        else
        {
          record = CRecordData::GetRecord(&g_Main.m_tblItemData[24], animusItem->m_wItemIndex);
          if (record)
          {
            if (m_bFreeSFByClass)
            {
              goto AnimusUsable;
            }

            bool animusUsable = false;
            auto *animusRecord = reinterpret_cast<_AnimusItem_fld *>(record);
            if (animusRecord->m_nAnimusType == 1)
            {
              if (m_Param.m_pClassHistory[0]
                  && m_Param.m_pClassHistory[0]->m_nClass == 3
                  && m_Param.m_pClassData->m_nClass == 3)
              {
                animusUsable = m_Param.m_pClassData->m_bAnimusUsable != 0;
              }
            }
            else
            {
              for (int index = 0; index < 4; ++index)
              {
                _class_fld *historyEffect = *m_Param.m_ppHistoryEffect[index];
                if (!historyEffect)
                {
                  break;
                }
                if (historyEffect->m_bAnimusUsable)
                {
                  animusUsable = true;
                  break;
                }
              }
            }

            if (animusUsable)
            {
AnimusUsable:
              if (m_bFreeRecallWaitTime)
              {
                m_bFreeRecallWaitTime = false;
              }
              else if (m_wTimeFreeRecallSerial != animusItem->m_wSerial)
              {
                const unsigned int elapsed = GetLoopTime() - m_dwLastRecallTime;
                if (elapsed < 0x7530)
                {
                  resultCode = 9;
                  goto Result;
                }
              }

              animusField = GetAnimusFldFromExp(animusItem->m_wItemIndex, animusItem->m_dwDur);
              if (!animusField)
              {
                return;
              }

              animusLvParam = &animusItem->m_dwLv;
              animusHP = static_cast<unsigned __int16>(*animusLvParam);
              animusFP = static_cast<unsigned __int16>(*(reinterpret_cast<unsigned __int16 *>(animusLvParam) + 1));
              if (m_wTimeFreeRecallSerial != animusItem->m_wSerial)
              {
                int hpDiff = static_cast<int>(animusHP) - static_cast<int>(wAnimusClientHP);
                if (hpDiff < 0)
                {
                  hpDiff = -hpDiff;
                }
                if (hpDiff <= 4)
                {
                  animusHP = wAnimusClientHP;
                }

                int fpDiff = static_cast<int>(animusFP) - static_cast<int>(wAnimusClientFP);
                if (fpDiff < 0)
                {
                  fpDiff = -fpDiff;
                }
                if (fpDiff <= 4)
                {
                  animusFP = wAnimusClientFP;
                }

                if (static_cast<float>(animusHP) / static_cast<float>(animusField->m_nMaxHP) < 0.30000001f)
                {
                  resultCode = 3;
                  goto Result;
                }
              }

              if (m_wTimeFreeRecallSerial != animusItem->m_wSerial)
              {
                const int masteryPerMast = _MASTERY_PARAM::GetMasteryPerMast(&m_pmMst, 6u, 0);
                if (animusField->m_nLevel < 50)
                {
                  const double powValue = std::pow(static_cast<double>(animusField->m_nLevel - 5), 2.0);
                  if (masteryPerMast < static_cast<int>(powValue / 20.0))
                  {
                    resultCode = 11;
                    goto Result;
                  }
                }
                else
                {
                  if (masteryPerMast < 99)
                  {
                    resultCode = 11;
                    goto Result;
                  }
                  if (animusField->m_nLevel > 50)
                  {
                    const int level = static_cast<int>(GetLevel());
                    if (animusField->m_nLevel > level + 1)
                    {
                      resultCode = 11;
                      goto Result;
                    }
                  }
                }
                if (GetFP() < 60)
                {
                  resultCode = 4;
                  goto Result;
                }
              }

              newAnimus = FindEmptyAnimus(g_Animus, MAX_ANIMUS);
              if (!newAnimus)
              {
                resultCode = 5;
              }
              goto Result;
            }

            resultCode = 12;
          }
          else
          {
            resultCode = 8;
          }
        }
      }
      else
      {
        resultCode = 2;
      }
    }
  }
  else
  {
    resultCode = 10;
  }

Result:
  if (!resultCode)
  {
    if (m_wTimeFreeRecallSerial != animusItem->m_wSerial)
    {
      const int currentFP = GetFP();
      const int consumeFPPoint = 60;
      SetFP(currentFP - consumeFPPoint, true);
    }
    *reinterpret_cast<unsigned __int16 *>(animusLvParam) = animusHP;
    *(reinterpret_cast<unsigned __int16 *>(animusLvParam) + 1) = animusFP;

    _animus_create_setdata animusData;
    animusData.m_pMap = m_pCurMap;
    animusData.m_nLayerIndex = m_wMapLayerIndex;
    animusData.m_pRecordSet = CRecordData::GetRecord(&g_Main.m_tblAnimus, animusItem->m_wItemIndex);
    memcpy_0(animusData.m_fStartPos, m_fCurPos, sizeof(animusData.m_fStartPos));
    animusData.nHP = static_cast<int>(*reinterpret_cast<unsigned __int16 *>(animusLvParam));
    animusData.nFP = static_cast<int>(*(reinterpret_cast<unsigned __int16 *>(animusLvParam) + 1));
    animusData.dwExp = animusItem->m_dwDur;
    animusData.pMaster = this;
    animusData.nMaxAttackPnt = m_nAnimusAttackPnt;
    newAnimus->Create(&animusData);
    m_pRecalledAnimusItem = animusItem;
    m_pRecalledAnimusChar = newAnimus;
    m_wTimeFreeRecallSerial = static_cast<unsigned __int16>(-1);
    animusItem->lock(true);
  }

  const unsigned __int16 leftFP = static_cast<unsigned __int16>(GetFP());
  SendMsg_AnimusRecallResult(static_cast<char>(resultCode), leftFP, newAnimus);
}

void CPlayer::pc_AnimusReturnRequest()
{
  unsigned __int8 resultCode = 0;
  if (!m_pRecalledAnimusItem || !m_pRecalledAnimusChar)
  {
    resultCode = 7;
  }
  if (resultCode)
  {
    SendMsg_AnimusReturnResult(static_cast<char>(resultCode), 0xFFFFu, 0);
  }
  else
  {
    _AnimusReturn(0);
  }
}

void CPlayer::pc_AnimusCommandRequest(unsigned __int8 byCommandCode)
{
  unsigned __int8 resultCode = 0;
  if (!m_pRecalledAnimusItem || !m_pRecalledAnimusChar)
  {
    resultCode = 7;
  }
  if (!resultCode)
  {
    CAnimus::ChangeMode_MasterCommand(m_pRecalledAnimusChar, byCommandCode);
  }
}

void CPlayer::pc_AnimusTargetRequest(
  unsigned __int8 byObjectID,
  unsigned __int16 wObjectIndex,
  unsigned int dwObjectSerial)
{
  (void)dwObjectSerial;
  unsigned __int8 resultCode = 0;
  CPlayer *target = static_cast<CPlayer *>(CMainThread::GetObjectA(&g_Main, 0, byObjectID, wObjectIndex));
  if (m_pRecalledAnimusItem && m_pRecalledAnimusChar)
  {
    if (target->m_bLive && !target->m_bCorpse)
    {
      if (GetSqrt(m_pRecalledAnimusChar->m_fCurPos, target->m_fCurPos) > 400.0f)
      {
        resultCode = 8;
      }
    }
    else
    {
      resultCode = 8;
    }
  }
  else
  {
    resultCode = 7;
  }
  if (!resultCode && !CAnimus::ChangeTarget_MasterCommand(m_pRecalledAnimusChar, target))
  {
    resultCode = 8;
  }
  SendMsg_AnimusTargetResult(static_cast<char>(resultCode));
}

void CPlayer::SendMsg_AnimusRecallResult(
  char byResultCode,
  unsigned __int16 wLeftFP,
  CAnimus *pNewAnimus)
{
  unsigned __int8 msg[0x13];
  msg[0] = static_cast<unsigned __int8>(byResultCode);
  memcpy_0(&msg[0x11], &wLeftFP, sizeof(wLeftFP));
  if (!byResultCode && pNewAnimus)
  {
    memcpy_0(&msg[1], &pNewAnimus->m_dwObjSerial, sizeof(pNewAnimus->m_dwObjSerial));

    const unsigned __int16 animusHP = static_cast<unsigned __int16>(pNewAnimus->m_nHP);
    const unsigned __int16 animusFP = static_cast<unsigned __int16>(pNewAnimus->m_nFP);
    memcpy_0(&msg[5], &animusHP, sizeof(animusHP));
    memcpy_0(&msg[7], &animusFP, sizeof(animusFP));
    memcpy_0(&msg[9], &pNewAnimus->m_dwExp, sizeof(pNewAnimus->m_dwExp));
  }

  unsigned __int8 type[2] = {22, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(msg), 0x13u);
}

void CPlayer::SendMsg_AnimusTargetResult(char byRetCode)
{
  unsigned __int8 msg[1];
  msg[0] = static_cast<unsigned __int8>(byRetCode);

  unsigned __int8 type[2] = {22, 8};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(msg), 1u);
}

void CPlayer::AlterMode_Animus(unsigned __int8 byMode)
{
  if (m_pRecalledAnimusItem)
  {
    SendMsg_AnimusModeInform(static_cast<char>(byMode));
  }
}

void CPlayer::SendMsg_AnimusModeInform(char byMode)
{
  unsigned __int8 msg[1];
  msg[0] = static_cast<unsigned __int8>(byMode);

  unsigned __int8 type[2] = {22, 6};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(msg), 1u);
}

char CPlayer::mgr_tracing(int bOper)
{
  const bool bOn = bOper != 0;
  if (m_bObserver == bOn)
  {
    return 0;
  }
  _AnimusReturn(1);
  SendMsg_Destroy();
  m_bObserver = bOn;
  SendMsg_NewViewOther(0);
  return 1;
}

void CPlayer::SendMsg_AlterItemDurInform(char byStorageCode, unsigned __int16 wItemSerial, unsigned __int64 dwDur)
{
#pragma pack(push, 1)
  struct AlterItemDurMsg
  {
    char byStorageCode;
    unsigned __int16 wItemSerial;
    unsigned __int64 dwDur;
  };
#pragma pack(pop)

  AlterItemDurMsg msg{};
  msg.byStorageCode = byStorageCode;
  msg.wItemSerial = wItemSerial;
  msg.dwDur = dwDur;

  unsigned __int8 type[2] = {7, 25};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 0xBu);
}

void CPlayer::SendMsg_AlterTowerHP(unsigned __int16 wItemSerial, unsigned __int16 wLeftHP)
{
#pragma pack(push, 1)
  struct AlterTowerHpMsg
  {
    unsigned __int16 wItemSerial;
    unsigned __int16 wLeftHP;
  };
#pragma pack(pop)

  AlterTowerHpMsg msg{};
  msg.wItemSerial = wItemSerial;
  msg.wLeftHP = wLeftHP;

  unsigned __int8 type[2] = {17, 22};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 4u);
}

void CPlayer::_TowerDestroy(CGuardTower *pTowerObj)
{
  _STORAGE_LIST::_db_con *pItem = nullptr;
  for (int j = 0; j < 6; ++j)
  {
    if (m_pmTwr.m_List[j].m_pTowerObj == pTowerObj)
    {
      pItem = m_pmTwr.m_List[j].m_pTowerItem;
      m_pmTwr.m_List[j].init();
      --m_pmTwr.m_nCount;
      break;
    }
  }

  if (pItem)
  {
    pItem->m_bLock = false;
    SendMsg_AlterTowerHP(pItem->m_wSerial, 0);
    Emb_DelStorage(0, pItem->m_byStorageIndex, false, true, "CPlayer::_TowerDestroy()");
    s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex, pItem, m_szItemHistoryFileName);
  }
}

void CPlayer::_TowerAllReturn(unsigned __int8 byDestroyType, bool bForceReturn)
{
  for (int j = 0; j < 6; ++j)
  {
    if (m_pmTwr.m_List[j].m_pTowerItem)
    {
      m_pmTwr.m_List[j].m_pTowerItem->lock(false);
      CGuardTower *tower = m_pmTwr.m_List[j].m_pTowerObj;
      if (tower->m_bLive)
      {
        const int nAlter = tower->m_nHP - static_cast<int>(m_pmTwr.m_List[j].m_pTowerItem->m_dwDur);
        Emb_AlterDurPoint(0, m_pmTwr.m_List[j].m_pTowerItem->m_byStorageIndex, nAlter, false, false);
        if (!bForceReturn && m_bOper)
        {
          for (int k = 0; k < MAX_PLAYER; ++k)
          {
            __TEMP_WAIT_TOWER *waitNode = &CGuardTower::s_Temp[k];
            if (waitNode->dwMasterSerial == static_cast<unsigned int>(-1))
            {
              waitNode->dwMasterSerial = tower->m_pMasterTwr->m_dwObjSerial;
              waitNode->byItemIndex = tower->m_pItem->m_byStorageIndex;
              waitNode->pMap = tower->m_pCurMap;
              memcpy_0(waitNode->fPos, tower->m_fCurPos, sizeof(waitNode->fPos));
              waitNode->bComplete = tower->m_bComplete;
              waitNode->dwPushTime = GetLoopTime();
              break;
            }
          }
        }
        tower->Destroy(byDestroyType, false);
      }
      m_pmTwr.m_List[j].init();
      --m_pmTwr.m_nCount;
    }
  }
}

void CPlayer::SendMsg_Destroy()
{
#pragma pack(push, 1)
  struct DestroyMsg
  {
    unsigned __int16 wIndex;
    unsigned int dwSerial;
    unsigned __int8 byDesType;
  };
#pragma pack(pop)

  DestroyMsg msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  msg.byDesType = 0;

  unsigned __int8 type[2] = {3, 22};
  CircleReport(type, reinterpret_cast<char *>(&msg), 7, false);
}

void CPlayer::SendMsg_NewViewOther(unsigned __int8 byViewType)
{
#pragma pack(push, 1)
  struct NewViewOtherMsg
  {
    unsigned __int16 wIndex;
    unsigned int dwSerial;
    unsigned __int16 wVisualVer;
    short pos[3];
    unsigned __int8 byRaceSexCode;
    unsigned __int8 byViewType;
    unsigned __int64 dwStateFlag;
    unsigned __int16 wLastContEffect;
    unsigned __int8 byGuildBattleColorInx;
  };
#pragma pack(pop)

  NewViewOtherMsg msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  msg.wVisualVer = GetVisualVer();
  FloatToShort(m_fCurPos, msg.pos, 3);
  msg.byRaceSexCode = static_cast<unsigned __int8>(m_Param.GetRaceSexCode());
  msg.byViewType = byViewType;
  msg.dwStateFlag = GetStateFlag();
  msg.wLastContEffect = m_wLastContEffect;
  msg.byGuildBattleColorInx = m_byGuildBattleColorInx;

  unsigned __int8 type[2] = {3, 36};
  CircleReport(type, reinterpret_cast<char *>(&msg), 27, false);
}

void CPlayer::SetLastAttBuff(bool bSet)
{
  m_Param.m_bLastAttBuff = bSet;
  if (m_pUserDB)
  {
    m_pUserDB->Update_LastAttBuff(bSet);
  }
}

bool CPlayer::IsLastAttBuff()
{
  return m_Param.m_bLastAttBuff;
}

__int64 CPlayer::GetLevel()
{
  return m_Param.GetLevel();
}

__int64 CPlayer::GetHP()
{
  return m_Param.GetHP();
}

__int64 CPlayer::GetMaxHP()
{
  const float baseMax = static_cast<float>(m_nMaxPoint[0]);
  const float rate = m_EP.GetEff_Rate(9);
  const int maxHp = static_cast<int>(baseMax * rate);
  if (maxHp <= 0)
  {
    return 1;
  }
  return maxHp;
}

unsigned __int64 CPlayer::GetStateFlag()
{
  return m_dwLastState;
}

void CPlayer::SetStateFlag()
{
  if (!m_pPartyMgr)
  {
    return;
  }

  unsigned __int64 state = 0;
  const unsigned __int64 bit = 1;

  if (m_byMoveType == 1)
  {
    state |= bit;
  }
  if (m_byModeType == 1)
  {
    state |= bit << 1;
  }
  if (GetStealth(false))
  {
    state |= bit << 2;
  }
  if (m_bStun)
  {
    state |= bit << 3;
  }
  if (m_pPartyMgr->IsPartyMode())
  {
    state |= bit << 4;
  }
  if (m_bCorpse || !m_Param.GetHP())
  {
    state |= bit << 5;
  }
  if (m_pPartyMgr->IsPartyBoss())
  {
    state |= bit << 6;
  }
  if (m_pmTrd.bDTradeMode)
  {
    state |= bit << 7;
  }
  if (m_EP.GetEff_Plus(22) > 0.0)
  {
    state |= bit << 8;
  }
  if (m_bObserver)
  {
    state |= bit << 9;
  }
  if (m_pPartyMgr->IsPartyLock())
  {
    state |= bit << 10;
  }
  if (m_bMineMode)
  {
    state |= bit << 11;
  }
  if (m_byStandType)
  {
    state |= bit << 13;
  }
  if (m_bBlockParty)
  {
    state |= bit << 14;
  }
  if (m_bBlockTrade)
  {
    state |= bit << 15;
  }

  const int raceCode = m_Param.GetRaceCode();
  CPvpUserAndGuildRankingSystem *rankingSystem = CPvpUserAndGuildRankingSystem::Instance();
  if (m_dwObjSerial == rankingSystem->GetCurrentRaceBossSerial(static_cast<unsigned __int8>(raceCode), 0))
  {
    state |= bit << 16;
  }
  const int viceRaceCode = m_Param.GetRaceCode();
  if (rankingSystem->IsRaceViceBoss(static_cast<unsigned __int8>(viceRaceCode), m_dwObjSerial))
  {
    state |= bit << 17;
  }
  if (m_EP.GetEff_State(20))
  {
    state |= bit << 18;
  }
  if (m_EP.GetEff_State(21))
  {
    state |= bit << 19;
  }
  if (m_EP.GetEff_State(22))
  {
    state |= bit << 20;
  }
  if (m_dwSelfDestructionTime)
  {
    state |= bit << 21;
  }
  if (g_HolySys.GetDestroyerSerial() == m_dwObjSerial)
  {
    state |= bit << 22;
  }
  if (m_bTakeGravityStone)
  {
    state |= bit << 23;
  }
  if (m_bInGuildBattle)
  {
    state |= bit << 24;
  }
  if (m_bTakeSoccerBall)
  {
    state |= bit << 25;
  }
  if (m_byMoveType == 2)
  {
    state |= bit << 26;
  }
  if (m_nChaosMode == 1)
  {
    state |= bit << 27;
  }
  if (m_nChaosMode == 2)
  {
    state |= bit << 28;
  }
  if (m_bSnowMan)
  {
    state |= bit << 30;
  }
  for (int j = 0; j < 2; ++j)
  {
    if (m_PotionParam.m_ContCommonPotionData[j].m_dwPotionEffectIndex == 237)
    {
      state |= bit << 29;
    }
    if (m_PotionParam.m_ContCommonPotionData[j].m_dwPotionEffectIndex == 238)
    {
      state |= bit << 31;
    }
  }
  if (IsPunished(1u, false))
  {
    state |= bit << 32;
  }

  const unsigned int charSerial = m_Param.GetCharSerial();
  const int bossRaceCode = m_Param.GetRaceCode();
  const unsigned __int8 bossType =
    rankingSystem->GetBossType(static_cast<unsigned __int8>(bossRaceCode), charSerial);
  switch (bossType)
  {
    case 1:
      state |= bit << 33;
      break;
    case 2:
      state |= bit << 34;
      break;
    case 3:
      state |= bit << 35;
      break;
    case 4:
      state |= bit << 36;
      break;
    case 5:
      state |= bit << 37;
      break;
    case 6:
      state |= bit << 38;
      break;
    case 7:
      state |= bit << 39;
      break;
    case 8:
      state |= bit << 40;
      break;
    default:
      break;
  }

  if (m_bAfterEffect)
  {
    state |= bit << 41;
  }
  if (m_Param.m_pGuild && m_Param.m_byClassInGuild == 2)
  {
    state |= bit << 42;
  }
  if (m_tmrAuraSkill.m_bOper)
  {
    state |= bit << 43;
  }
  if (m_EP.GetEff_Have(50) > 0.0)
  {
    state |= bit << 44;
  }
  if (m_EP.GetEff_Have(58) > 0.0)
  {
    state |= bit << 45;
  }
  if (IsLastAttBuff())
  {
    state |= bit << 46;
  }
  if (m_byBattleTournamentGrade != 255)
  {
    if (m_byBattleTournamentGrade)
    {
      if (m_byBattleTournamentGrade == 1)
      {
        state |= bit << 48;
      }
      else if (m_byBattleTournamentGrade == 2)
      {
        state |= bit << 49;
      }
    }
    else
    {
      state |= bit << 47;
    }
  }
  if (GetInvisible())
  {
    state |= bit << 51;
  }
  if (m_EP.GetEff_State(28))
  {
    state |= bit << 52;
  }

  m_dwLastState = state;
}

void CPlayer::SenseState()
{
  const unsigned __int64 oldState = GetStateFlag();
  SetStateFlag();
  if (oldState != m_dwLastState)
  {
    SendMsg_StateInform(m_dwLastState);
  }
}

void CPlayer::SendMsg_StateInform(unsigned __int64 dwStateFlag)
{
  struct
  {
    unsigned int dwObjSerial;
    unsigned __int64 dwState;
  } msg{};

  msg.dwObjSerial = m_dwObjSerial;
  msg.dwState = dwStateFlag;

  unsigned __int8 type[2] = {4, 25};
  CircleReport(type, reinterpret_cast<char *>(&msg), 12, true);
}

char CPlayer::IsPunished(unsigned __int8 byType, bool bSend)
{
  if (byType >= 3u)
  {
    return 0;
  }
  if (g_Main.IsReleaseServiceMode() && m_byUserDgr == 2)
  {
    return 0;
  }
  if (!m_pUserDB)
  {
    return 0;
  }
  if (m_pUserDB->m_AvatorData.dbAvator.m_dwPunishment[byType] == static_cast<unsigned int>(-1))
  {
    return 0;
  }

  PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
  const unsigned int currentElectSerial = processor->GetCurrPatriarchElectSerial();
  if (m_pUserDB->m_AvatorData.dbAvator.m_dwElectSerial[byType] != currentElectSerial)
  {
    return 0;
  }

  __int64 nowTime = 0;
  time(&nowTime);
  const int nowMin = static_cast<int>(nowTime / 60);
  if (nowMin - static_cast<int>(m_pUserDB->m_AvatorData.dbAvator.m_dwPunishment[byType]) >= 0x10E0)
  {
    m_pUserDB->m_AvatorData.dbAvator.m_dwPunishment[byType] = static_cast<unsigned int>(-1);
    return 0;
  }

  if (bSend)
  {
    _pt_inform_punishment_zocl msg{};
    msg.byType = byType;
    msg.nRemainMin = 4320 - (nowMin - static_cast<int>(m_pUserDB->m_AvatorData.dbAvator.m_dwPunishment[byType]));

    unsigned __int8 type[2] = {13, 117};
    const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
    g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
  }
  return 1;
}

bool CPlayer::IsMapLoading()
{
  return GetCurSecNum() == static_cast<unsigned int>(-1) || m_bMapLoading;
}

bool CPlayer::IsRidingShip()
{
  return m_pCurMap == g_TransportShip[m_Param.GetRaceCode()].m_pLinkShipMap;
}

CMapData *CPlayer::GetBindMap(float *pfPos, bool bIgnoreMapClass)
{
  if (m_ObjID.m_byKind)
  {
    return nullptr;
  }

  CMapData *bindMap = nullptr;
  CMapData *curMap = m_pCurMap;
  if (GetCurSecNum() == static_cast<unsigned int>(-1) || m_bMapLoading)
  {
    return nullptr;
  }

  if (!bIgnoreMapClass && curMap->m_pMapSet->m_nMapClass)
  {
    const int raceCode = m_Param.GetRaceCode();
    if (curMap->m_nStartDumNum <= raceCode)
    {
      return nullptr;
    }
    if (!curMap->GetRandPosInDummy(curMap->m_pStartDummy[raceCode].m_pDumPos, pfPos, true))
    {
      return nullptr;
    }
    bindMap = curMap;
  }
  else if (m_pBindMapData)
  {
    bindMap = m_pBindMapData;
    if (!m_pBindDummyData)
    {
      m_pUserDB->Update_Bind(const_cast<char *>("0"), const_cast<char *>("0"), false);
      return nullptr;
    }
    if (!m_pBindMapData->GetRandPosInDummy(m_pBindDummyData, pfPos, true))
    {
      m_pUserDB->Update_Bind(const_cast<char *>("0"), const_cast<char *>("0"), false);
      return nullptr;
    }
  }
  else
  {
    const unsigned __int8 raceCode = static_cast<unsigned __int8>(m_Param.GetRaceCode());
    bindMap = g_MapOper.GetPosStartMap(raceCode, false, pfPos);
    if (!bindMap)
    {
      return nullptr;
    }
  }

  if (!Major_Bind_HQ)
  {
    return bindMap;
  }

  const unsigned __int8 raceCode = static_cast<unsigned __int8>(m_Param.GetRaceCode());
  CMapData *result = g_MapOper.GetPosStartMap(raceCode, false, pfPos);
  bindMap = result;
  return result;
}

CMapData *CPlayer::GetBindMapData()
{
  return m_pBindMapData;
}

_dummy_position *CPlayer::GetBindDummy()
{
  return m_pBindDummyData;
}

void CPlayer::SetBindMapData(CMapData *pMapData)
{
  m_pBindMapData = pMapData;
}

void CPlayer::SetBindDummy(_dummy_position *pDummy)
{
  m_pBindDummyData = pDummy;
}

bool CPlayer::SetBindPosition(CMapData *pMap, _dummy_position *pDummy)
{
  if (!m_bOper || !pMap || !pDummy || !m_pUserDB)
  {
    return false;
  }
  m_pBindMapData = pMap;
  m_pBindDummyData = pDummy;
  return m_pUserDB->Update_Bind(pMap->m_pMapSet->m_strCode, pDummy->m_szCode, true);
}

void CPlayer::ClearGravityStone()
{
  m_bTakeGravityStone = false;
  m_EP.SetEff_Plus(3, -30.0, 0);
  ApplyEquipItemEffect(12, true);
}

void CPlayer::ExitUpdateDataToWorld()
{
  if (m_pUserDB && !m_pUserDB->m_bNoneUpdateData)
  {
    _EXIT_ALTER_PARAM param{};
    param.dwHP = m_Param.GetHP();
    param.dwFP = m_Param.GetFP();
    param.dwSP = m_Param.GetSP();
    param.dwDP = m_Param.GetDP();
    param.dExp = m_Param.GetExp();
    param.byMapCode = static_cast<unsigned __int8>(m_Param.GetMapCode());
    float *curPos = m_Param.GetCurPos();
    memcpy_0(param.fStartPos, curPos, sizeof(param.fStartPos));
    param.dwDalant = m_Param.GetDalant();
    param.dwGold = m_Param.GetGold();
    m_pUserDB->Update_Param(&param);

    if (m_Param.GetRaceCode() == 0)
    {
      for (int j = 0; j < 4; ++j)
      {
        _UNIT_DB_BASE::_LIST *pData = &m_Param.m_UnitDB.m_List[j];
        if (m_Param.m_UnitDB.m_List[j].byFrame != 255)
        {
          m_pUserDB->Update_UnitData(j, pData);
        }
      }
    }

    for (int k = 0; k < 2; ++k)
    {
      for (int m = 0; m < 8; ++m)
      {
        if (m_SFCont[k][m].m_bExist)
        {
          unsigned __int8 order = 0;
          for (int j = 0; j < 8; ++j)
          {
            if (m_SFCont[k][j].m_bExist && m != j && m_SFCont[k][m].m_dwStartSec < m_SFCont[k][j].m_dwStartSec)
            {
              ++order;
            }
          }
          m_pUserDB->m_AvatorData.dbSfcont.m_List[k][m].SetOrder(order);
        }
      }
    }
  }
}

void CPlayer::pc_SetInGuildBattle(bool bInGuildBattle, unsigned __int8 byColorInx)
{
  m_bInGuildBattle = bInGuildBattle;
  m_byGuildBattleColorInx = byColorInx;
  m_bufShapeAll.byColor = m_byGuildBattleColorInx;
  if (!m_bInGuildBattle)
  {
    m_bTakeGravityStone = false;
  }
}

char CPlayer::pc_GiveItem(_STORAGE_LIST::_db_con *kItem, char *szReason, bool bDrop)
{
  const char *fallback = "NONE";
  char *reason = szReason;
  if (!reason)
  {
    reason = const_cast<char *>(fallback);
  }

  bool success = true;
  if (m_Param.m_dbInven.GetIndexEmptyCon() == 255)
  {
    if (bDrop)
    {
      if (!CreateItemBox(
            kItem,
            this,
            0xFFFFFFFF,
            false,
            nullptr,
            3u,
            m_pCurMap,
            m_wMapLayerIndex,
            m_fCurPos,
            false))
      {
        success = false;
      }
      CPlayer::s_MgrItemHistory.reward_add_item(
        m_ObjID.m_wIndex,
        reason,
        kItem,
        m_szItemHistoryFileName);
    }
  }
  else
  {
    kItem->m_wSerial = m_Param.GetNewItemSerial();
    if (!Emb_AddStorage(0, kItem, false, true))
    {
      success = false;
    }
    SendMsg_RewardAddItem(kItem, 4u);
    CPlayer::s_MgrItemHistory.reward_add_item(
      m_ObjID.m_wIndex,
      reason,
      kItem,
      m_szItemHistoryFileName);
  }
  return success ? 1 : 0;
}

void CPlayer::SendMsg_RewardAddItem(_STORAGE_LIST::_db_con *pItem, unsigned __int8 byReason)
{
#pragma pack(push, 1)
  struct RewardAddItemMsg
  {
    unsigned __int8 byTableCode;
    unsigned __int16 wItemIndex;
    unsigned __int64 dwDur;
    unsigned int dwLv;
    unsigned __int16 wSerial;
    unsigned __int8 byReason;
    unsigned __int8 byCsMethod;
    unsigned int dwT;
  };
#pragma pack(pop)

  RewardAddItemMsg msg{};
  msg.byTableCode = pItem->m_byTableCode;
  msg.wItemIndex = pItem->m_wItemIndex;
  msg.dwDur = pItem->m_dwDur;
  msg.dwLv = pItem->m_dwLv;
  msg.wSerial = pItem->m_wSerial;
  msg.byReason = byReason;
  msg.byCsMethod = pItem->m_byCsMethod;
  msg.dwT = pItem->m_dwT;

  unsigned __int8 byType[2] = {11, 9};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, byType, reinterpret_cast<char *>(&msg), 0x17u);
}

char CPlayer::SetHP(int nHP, bool bOver)
{
  const int curHp = static_cast<int>(GetHP());
  if (!bOver && nHP > curHp)
  {
    const int maxHp = static_cast<int>(GetMaxHP());
    if (curHp < maxHp && nHP > maxHp)
    {
      nHP = maxHp;
    }
    else if (curHp >= maxHp && nHP >= curHp)
    {
      return 0;
    }
  }

  if (nHP <= 0)
  {
    nHP = 0;
    const int halfMaxHp = static_cast<int>(GetMaxHP()) / 2;
    if (m_bNeverDie && halfMaxHp > 0)
    {
      nHP = halfMaxHp + (rand() % halfMaxHp);
    }
  }

  if (curHp == nHP)
  {
    return 0;
  }
  m_Param.SetHP(nHP);
  return 1;
}

void CPlayer::RecvHSKQuest(
  unsigned __int8 byHSKQuestCode,
  unsigned __int8 byCristalBattleDBInfo,
  int nPvpPoint,
  unsigned __int16 wKillPoint,
  unsigned __int16 wDieCount,
  unsigned __int8 byHSKTime)
{
  if (byCristalBattleDBInfo == 1)
  {
    if (m_byCristalBattleDBInfo != 3)
    {
      m_byCristalBattleDBInfo = byCristalBattleDBInfo;
      m_pUserDB->m_AvatorData.m_byCristalBattleDBInfo = m_byCristalBattleDBInfo;
    }
    return;
  }

  m_byHSKQuestCode = byHSKQuestCode;
  m_nHSKPvpPoint = 0;
  m_wKillPoint = 0;
  m_wDiePoint = 0;
  m_byCristalBattleDBInfo = 3;
  m_byHSKTime = static_cast<unsigned __int8>(-1);

  m_pUserDB->m_AvatorData.m_byHSKTime = m_byHSKTime;
  m_pUserDB->m_AvatorData.m_byPvpGrade = 0;
  m_pUserDB->m_AvatorData.m_iPvpPoint = m_nHSKPvpPoint;
  m_pUserDB->m_AvatorData.m_wKillPoint = m_wKillPoint;
  m_pUserDB->m_AvatorData.m_wDiePoint = m_wDiePoint;
  m_pUserDB->m_AvatorData.m_byCristalBattleDBInfo = m_byCristalBattleDBInfo;
  m_pUserDB->m_AvatorData.m_bCristalBattleDateUpdate = 0;

  if (m_byHSKQuestCode == 100)
  {
    return;
  }

  m_nHSKPvpPoint = nPvpPoint;
  m_wKillPoint = wKillPoint;
  m_wDiePoint = wDieCount;
  m_byHSKTime = static_cast<unsigned __int8>(byHSKTime - 1);
  m_byCristalBattleDBInfo = byCristalBattleDBInfo;

  m_pUserDB->m_AvatorData.m_byHSKTime = m_byHSKTime;
  m_pUserDB->m_AvatorData.m_byPvpGrade = m_Param.m_byPvPGrade;
  m_pUserDB->m_AvatorData.m_iPvpPoint = m_nHSKPvpPoint;
  m_pUserDB->m_AvatorData.m_wKillPoint = m_wKillPoint;
  m_pUserDB->m_AvatorData.m_wDiePoint = m_wDiePoint;
  m_pUserDB->m_AvatorData.m_byCristalBattleDBInfo = m_byCristalBattleDBInfo;

  _SYSTEMTIME tm{};
  ExtractStringToTime(m_pUserDB->m_AvatorData.dbSupplement.dwScanerGetDate, &tm);

  const unsigned __int16 startYear = g_HolySys.GetStartYear();
  const unsigned __int8 startMonth = g_HolySys.GetStartMonth();
  const unsigned __int8 startDay = g_HolySys.GetStartDay();
  const unsigned __int8 startHour = g_HolySys.GetStartHour();

  if (startYear == tm.wYear && startMonth == tm.wMonth && startDay == tm.wDay && byHSKTime == startHour)
  {
    SetCntEnable(false);
  }
  else
  {
    SetCntEnable(true);
  }

  SendMsg_RecvHSKQuest();
}

void CPlayer::SendMsg_RecvHSKQuest()
{
#pragma pack(push, 1)
  struct RecvHSKQuestMsg
  {
    char byHSKQuestCode;
    int nPvpPoint;
    unsigned __int16 wKillPoint;
    bool bMentalTicket;
  };
#pragma pack(pop)

  RecvHSKQuestMsg msg{};
  msg.byHSKQuestCode = m_byHSKQuestCode;
  msg.nPvpPoint = m_nHSKPvpPoint;
  msg.wKillPoint = m_wKillPoint;

  const unsigned __int8 byNumOfTime = g_HolySys.GetNumOfTime();
  const unsigned __int8 byCurrentHour = g_HolySys.GetStartHour();
  const unsigned __int8 byStartDay = g_HolySys.GetStartDay();
  const unsigned __int8 byStartMonth = g_HolySys.GetStartMonth();
  const unsigned __int16 byStartYear = g_HolySys.GetStartYear();

  msg.bMentalTicket =
    m_MinigTicket.AuthLastMentalTicket(byStartYear, byStartMonth, byStartDay, byCurrentHour, byNumOfTime)
    != 0;

  unsigned __int8 type[2] = {25, 10};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::AlterPvPPoint(double dAlter, int AlterType, unsigned int dwDstSerial)
{
  double alterValue = dAlter;
  const double penalty = g_Main.m_pTimeLimitMgr->GetPlayerPenalty(m_id.wIndex);
  if (dAlter > 0.0)
  {
    alterValue = dAlter * penalty;
  }

  const double curPoint = m_Param.GetPvPPoint();
  double nextPoint = curPoint + alterValue;
  if (nextPoint < 0.0)
  {
    nextPoint = 0.0;
  }

  if (curPoint == nextPoint)
  {
    return;
  }

  CHolyStone *holyStone = &g_Stone[static_cast<int>(m_Param.GetRaceCode())];
  if (holyStone->m_pCurMap == m_pCurMap && g_HolySys.GetSceneCode() == 1)
  {
    AlterPvpPointLeak(alterValue);
  }

  m_Param.SetPvPPoint(nextPoint);
  m_kPvpOrderView.UpdatePvPPoint(alterValue, nextPoint);
  m_kPvpOrderView.Notify_OrderView(m_ObjID.m_wIndex);
  s_MgrLvHistory.alter_pvp(
    m_ObjID.m_wIndex,
    alterValue,
    m_pPartyMgr,
    m_szLvHistoryFileName);

  if (curPoint != m_Param.GetPvPPoint())
  {
    if (m_pUserDB)
    {
      m_pUserDB->Update_AlterPvPPoint(m_Param.GetPvPPoint());
    }
    SendMsg_AlterPvPPoint();
  }

  if (AlterType < 10)
  {
    const char *charName = m_Param.GetCharNameA();
    g_Main.m_logPvP.Write(
      "%s [ %d ] DST: [ %d ] type: %s  >> pvp : %.0f  last: %.0f",
      charName,
      m_dwObjSerial,
      dwDstSerial,
      sType[AlterType],
      alterValue,
      m_Param.GetPvPPoint());
  }
}

void CPlayer::SendMsg_AlterPvPPoint()
{
  double msgPoint = m_Param.GetPvPPoint();
  unsigned __int8 type[2] = {11, 10};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msgPoint), 8);
}

void CPlayer::SendMsg_RaceBattlePenelty(int nAlterPoint, char byAlterType)
{
#pragma pack(push, 1)
  struct RaceBattlePenaltyMsg
  {
    char byAlterType;
    int nAlterPoint;
  };
#pragma pack(pop)

  RaceBattlePenaltyMsg msg{};
  msg.byAlterType = byAlterType;
  msg.nAlterPoint = nAlterPoint;
  unsigned __int8 type[2] = {25, 28};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 5);
}

void CPlayer::SetCntEnable(bool bSet)
{
  m_bCntEnable = bSet;
}

bool CPlayer::pc_Resurrect(bool bQuickPotion)
{
  char result = 0;
  if (m_bCorpse)
  {
    if (m_pCurMap->m_pMapSet->m_nMapType == 1)
    {
      result = 2;
    }
  }
  else
  {
    result = 1;
  }

  if (result == 0)
  {
    m_bCorpse = false;
    m_byModeType = 0;
    m_byMoveType = 1;

    const int maxHp = static_cast<int>(GetMaxHP());
    SetHP(maxHp, false);
    SetFP(GetMaxFP(), false);
    SetSP(GetMaxSP(), false);

    SendMsg_ResurrectInform();
    SendMsg_Resurrect(0, bQuickPotion);
    if (m_bAfterEffect)
    {
      pc_NuclearAfterEffect();
    }
    CNuclearBombMgr::Instance()->CheckNuclearState(this);
  }
  return result == 0;
}

void CPlayer::ForcePullUnit(bool bLogout)
{
  if (!m_pUsingUnit || (!bLogout && !m_pParkingUnit))
  {
    return;
  }

  if (m_pParkingUnit)
  {
    m_pParkingUnit->Destroy(0);
  }

  if (bLogout || m_Param.GetDalant() < 1000)
  {
    _UpdateUnitDebt(m_pUsingUnit->bySlotIndex, 1000);
  }
  else
  {
    SubDalant(1000);
    const int level = m_Param.GetLevel();
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      CMoneySupplyMgr::Instance()->UpdateBuyUnitData(level, 1000);
    }
  }

  if (bLogout && m_bOper)
  {
    m_pUsingUnit->dwCutTime = GetKorLocalTime();
    if (m_pUserDB)
    {
      m_pUserDB->Update_UnitData(m_pUsingUnit->bySlotIndex, m_pUsingUnit);
    }
  }

  if (!bLogout)
  {
    SendMsg_UnitForceReturnInform(m_pUsingUnit->bySlotIndex, 1000);
  }

  _LockUnitKey(m_pUsingUnit->bySlotIndex, false);
  m_pUsingUnit = nullptr;
  m_pParkingUnit = nullptr;
}

void CPlayer::SendMsg_RemainOreRate()
{
  char msg = COreAmountMgr::Instance()->GetDepositRate();
  unsigned __int8 type[2] = {14, 68};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, &msg, 1);
}

void CPlayer::SendMsg_OreTransferCount()
{
  char msg = COreAmountMgr::Instance()->GetOreTransferCount();
  unsigned __int8 type[2] = {14, 69};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, &msg, 1);
}

void CPlayer::SendMsg_LendItemTimeExpired(char byStorageCode, unsigned __int16 wSerial)
{
#pragma pack(push, 1)
  struct LendItemTimeExpiredMsg
  {
    char byStorageCode;
    unsigned __int16 wSerial;
  };
#pragma pack(pop)

  LendItemTimeExpiredMsg msg{};
  msg.byStorageCode = byStorageCode;
  msg.wSerial = wSerial;
  unsigned __int8 type[2] = {7, 71};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 3);
}

void CPlayer::SetHaveEffectUseTime(_STORAGE_LIST::_db_con *pItem, bool bAdd)
{
  if (pItem->m_byTableCode != 18)
  {
    return;
  }

  _base_fld *record = g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
  if (!record)
  {
    return;
  }

  if (bAdd)
  {
    for (int j = 0; j < static_cast<int>(record[6].m_dwIndex); ++j)
    {
      const int effIndex = *reinterpret_cast<int *>(&record[6].m_strCode[12 * j]);
      if (effIndex <= 0x52)
      {
        const signed __int64 dur = static_cast<signed __int64>(pItem->m_dwDur);
        float durValue = static_cast<float>(static_cast<int>(dur));
        if (dur < 0)
        {
          durValue += 1.8446744e19f;
        }

        m_EP.m_pDataParam->m_fEff_Have[effIndex] +=
          (*reinterpret_cast<float *>(&record[6].m_strCode[12 * j + 4]) * durValue);

        const float maxValue = *reinterpret_cast<float *>(&record[6].m_strCode[12 * j + 8]);
        if (m_EP.m_pDataParam->m_fEff_Have[effIndex] > maxValue)
        {
          m_EP.m_pDataParam->m_fEff_Have[effIndex] = maxValue;
          if ((effIndex >= 71 && effIndex <= 75)
              || (effIndex >= 5 && effIndex <= 9)
              || effIndex == 0
              || effIndex == 2)
          {
            m_EP.m_pDataParam->m_fEff_Have[effIndex] =
              m_EP.m_pDataParam->m_fEff_Have[effIndex] + FLOAT_1_0;
          }
        }
      }
    }
    s_MgrItemHistory.time_jade_effect_log(record->m_strCode, pItem, true, m_szItemHistoryFileName);
    return;
  }

  for (int k = 0; k < static_cast<int>(record[6].m_dwIndex); ++k)
  {
    const int effIndex = *reinterpret_cast<int *>(&record[6].m_strCode[12 * k]);
    if (effIndex > -1)
    {
      if (effIndex < 83)
      {
        const signed __int64 dur = static_cast<signed __int64>(pItem->m_dwDur);
        float durValue = static_cast<float>(static_cast<int>(dur));
        if (dur < 0)
        {
          durValue += 1.8446744e19f;
        }

        const float mulValue = *reinterpret_cast<float *>(&record[6].m_strCode[12 * k + 4]) * durValue;
        const float maxValue = *reinterpret_cast<float *>(&record[6].m_strCode[12 * k + 8]);

        if (maxValue <= mulValue)
        {
          m_EP.m_pDataParam->m_fEff_Have[effIndex] -= mulValue;
        }
        else
        {
          m_EP.m_pDataParam->m_fEff_Have[effIndex] -= maxValue;
        }

        if ((effIndex < 71 || effIndex > 75)
            && (effIndex < 5 || effIndex > 9)
            && effIndex != 0
            && effIndex != 2)
        {
          if (m_EP.m_pDataParam->m_fEff_Have[effIndex] < 0.0f)
          {
            m_EP.m_pDataParam->m_fEff_Have[effIndex] = 0.0f;
          }
        }
        else if (m_EP.m_pDataParam->m_fEff_Have[effIndex] < FLOAT_1_0)
        {
          m_EP.m_pDataParam->m_fEff_Have[effIndex] = FLOAT_1_0;
        }
      }

      s_MgrItemHistory.time_jade_effect_log(record->m_strCode, pItem, false, m_szItemHistoryFileName);
    }
  }
}

void _DTRADE_ITEM::SetData(
  unsigned __int8 p_byStorageCode,
  unsigned int p_dwSerial,
  unsigned __int8 p_byAmount)
{
  bLoad = true;
  byStorageCode = p_byStorageCode;
  dwSerial = p_dwSerial;
  byAmount = p_byAmount;
}

void _DTRADE_ITEM::ReleaseData()
{
  bLoad = false;
}

void _DTRADE_PARAM::SetDTradeStart(
  unsigned __int16 pl_dwDstIndex,
  unsigned int pl_dwDstSerial,
  unsigned __int8 pl_mEmptyInvenNum,
  unsigned int *pl_dwKey)
{
  bDTradeMode = true;
  bDTradeLock = false;
  bDTradeOK = false;
  wDTradeDstIndex = pl_dwDstIndex;
  dwDTradeDstSerial = pl_dwDstSerial;
  dwDTrade_Dalant = 0;
  dwDTrade_Gold = 0;
  byEmptyInvenNum = pl_mEmptyInvenNum;
  bySellItemNum = 0;
  for (int j = 0; j < 15; ++j)
  {
    DItemNode[j].ReleaseData();
  }
  memcpy_0(dwKey, pl_dwKey, sizeof(dwKey));
}

void _DTRADE_PARAM::Init()
{
  bDTradeMode = false;
  wDTradeDstIndex = static_cast<unsigned __int16>(-1);
  dwDTradeDstSerial = static_cast<unsigned int>(-1);
}

bool DTradeEqualPerson(CPlayer *lp_pOne, CPlayer **lpp_pDst)
{
  if (!lp_pOne->m_bLive || lp_pOne->m_bCorpse || lp_pOne->GetCurSecNum() == static_cast<unsigned int>(-1))
  {
    return false;
  }
  if (!lp_pOne->m_bOper || !lp_pOne->m_pUserDB)
  {
    return false;
  }
  if (!lp_pOne->m_pmTrd.bDTradeMode || lp_pOne->m_pmTrd.wDTradeDstIndex == 0xFFFF)
  {
    return false;
  }

  CPlayer *dst = &g_Player[lp_pOne->m_pmTrd.wDTradeDstIndex];
  if (!dst->m_bLive || !dst->m_pmTrd.bDTradeMode)
  {
    return false;
  }

  if (lp_pOne->m_pmTrd.dwDTradeDstSerial == dst->m_dwObjSerial
      && !dst->m_bCorpse
      && dst->GetCurSecNum() != static_cast<unsigned int>(-1)
      && dst->m_pCurMap == lp_pOne->m_pCurMap
      && dst->m_bOper
      && dst->m_pUserDB
      && dst->m_pmTrd.wDTradeDstIndex == lp_pOne->m_ObjID.m_wIndex
      && dst->m_pmTrd.dwDTradeDstSerial == lp_pOne->m_dwObjSerial
      && dst->m_Param.GetRaceCode() == lp_pOne->m_Param.GetRaceCode())
  {
    if (lpp_pDst)
    {
      *lpp_pDst = dst;
    }
    return true;
  }

  dst->m_pmTrd.Init();
  dst->SendMsg_DTradeCloseInform(0);
  return false;
}

unsigned __int8 CPlayer::GetEffectEquipCode(unsigned __int8 byStorageCode, unsigned __int8 bySlotIndex)
{
  unsigned __int8 *code = nullptr;
  if (byStorageCode == 1)
  {
    code = &m_byEffectEquipCode[bySlotIndex];
  }
  else
  {
    code = reinterpret_cast<unsigned __int8 *>(&m_pRecordSet) + bySlotIndex + 49386;
  }
  return *code;
}

void CPlayer::SetEffectEquipCode(unsigned __int8 byStorageCode, unsigned __int8 bySlotIndex, unsigned __int8 byCode)
{
  unsigned __int8 *code = nullptr;
  if (byStorageCode == 1)
  {
    code = &m_byEffectEquipCode[bySlotIndex];
  }
  else
  {
    code = reinterpret_cast<unsigned __int8 *>(&m_pRecordSet) + bySlotIndex + 49386;
  }
  *code = byCode;
}

void CPlayer::SetEquipEffect(_STORAGE_LIST::_db_con *pItem, bool bEquip)
{
  apply_case_equip_std_effect(pItem, bEquip);
  apply_case_equip_upgrade_effect(pItem, bEquip);
}

char CPlayer::ApplyEquipItemEffect(int iItemEffectCode, bool bEquip)
{
  for (int index = 0; index < 15; ++index)
  {
    if (m_byEffectEquipCode[index] != 1)
    {
      continue;
    }

    _STORAGE_LIST::_db_con *pCon = nullptr;
    if (index >= 8)
    {
      pCon = &m_Param.m_dbEmbellish.m_pStorageList[index - 8];
    }
    else
    {
      pCon = &m_Param.m_dbEquip.m_pStorageList[index];
    }

    if (pCon->m_bLoad)
    {
      if (IsEffectableEquip(pCon))
      {
        _ITEM_EFFECT *itemEffect = _GetItemEffect(pCon);
        if (itemEffect)
        {
          for (int effectIndex = 0; effectIndex < 4; ++effectIndex)
          {
            if (iItemEffectCode == itemEffect[effectIndex].nEffectCode)
            {
              apply_normal_item_std_effect(
                itemEffect[effectIndex].nEffectCode,
                itemEffect[effectIndex].fEffectValue,
                bEquip);
            }
          }
        }
      }
    }
  }
  return 0;
}

char CPlayer::IsEffectableEquip(_STORAGE_LIST::_storage_con *pCon)
{
  const unsigned __int8 itemEquipLevel =
    static_cast<unsigned __int8>(GetItemEquipLevel(pCon->m_byTableCode, pCon->m_wItemIndex));
  const unsigned __int8 itemEquipUpLevel =
    static_cast<unsigned __int8>(GetItemEquipUpLevel(pCon->m_byTableCode, pCon->m_wItemIndex));
  const float level = static_cast<float>(m_Param.GetLevel());
  const float effHave = m_EP.GetEff_Have(4);
  if (static_cast<float>(itemEquipLevel) > (level + effHave))
  {
    return 0;
  }

  if (itemEquipUpLevel != static_cast<unsigned __int8>(-1))
  {
    const float limitLevel = static_cast<float>(itemEquipUpLevel);
    const float curLevel = static_cast<float>(m_Param.GetLevel());
    const float curEffHave = m_EP.GetEff_Have(4);
    if ((curLevel - curEffHave) > limitLevel)
    {
      return 0;
    }
  }

  int limNum = 0;
  _EQUIP_MASTERY_LIM *equipMastery =
    GetItemEquipMastery(pCon->m_byTableCode, pCon->m_wItemIndex, &limNum);
  if (!equipMastery)
  {
    return 1;
  }

  for (int index = 0; index < limNum; ++index)
  {
    const int equipMasteryCode = equipMastery[index].nMasteryCode;
    if (equipMasteryCode != -1)
    {
      const int mastery = m_pmMst.GetEquipMastery(equipMasteryCode);
      const unsigned int masteryLimit = _check_equipmastery_lim(equipMasteryCode);
      const int cappedMastery =
        mastery >= static_cast<int>(masteryLimit)
          ? static_cast<int>(_check_equipmastery_lim(equipMasteryCode))
          : static_cast<int>(m_pmMst.GetEquipMastery(equipMasteryCode));
      if (cappedMastery < equipMastery[index].nLimMastery)
      {
        return 0;
      }
    }
  }

  return 1;
}

unsigned int CPlayer::_check_equipmastery_lim(int EquipMasteryCode)
{
  switch (EquipMasteryCode)
  {
    case 0:
      return static_cast<unsigned int>(_check_mastery_lim(0, 0));
    case 1:
      return static_cast<unsigned int>(_check_mastery_lim(0, 1u));
    case 2:
      return static_cast<unsigned int>(_check_mastery_lim(6u, 0));
    case 3:
      return 99;
    case 4:
      return static_cast<unsigned int>(_check_mastery_lim(2u, 0));
    case 5:
      return static_cast<unsigned int>(_check_mastery_lim(1u, 0));
    default:
      return 99;
  }
}

__int64 CPlayer::_check_mastery_lim(unsigned __int8 byMasteryClass, unsigned __int8 byIndex)
{
  _base_fld *curRecord = nullptr;
  _base_fld *baseRecord = nullptr;

  if (m_Param.GetPtrCurClass()->m_nClass < 4 && m_Param.GetPtrBaseClass()->m_nClass < 4)
  {
    if (m_Param.GetRaceCode() < 3)
    {
      const int maxLevel = static_cast<int>(m_Param.GetMaxLevel());
      const int curLevel = static_cast<int>(GetLevel());
      if (curLevel <= maxLevel && GetLevel() > 0)
      {
        if (m_Param.m_pClassHistory[0])
        {
          _class_fld *curClass = m_Param.GetPtrCurClass();
          _class_fld *baseClass = m_Param.GetPtrBaseClass();
          if (curClass->m_nClass == baseClass->m_nClass)
          {
            const int recordIndex = static_cast<int>(GetLevel()) - 1;
            CRecordData *table = s_tblLimMasteryContinue[m_Param.GetRaceCode()];
            curRecord = table[curClass->m_nClass].GetRecord(recordIndex);
          }
          else
          {
            const int recordIndex = static_cast<int>(GetLevel()) - 1;
            CRecordData *table = s_tblLimMastery[m_Param.GetRaceCode()];
            curRecord = table[curClass->m_nClass].GetRecord(recordIndex);
            baseRecord = table[baseClass->m_nClass].GetRecord(recordIndex);
          }
        }
        else
        {
          const int recordIndex = static_cast<int>(GetLevel()) - 1;
          CRecordData *table = s_tblLimMastery[m_Param.GetRaceCode()];
          _class_fld *curClass = m_Param.GetPtrCurClass();
          curRecord = table[curClass->m_nClass].GetRecord(recordIndex);
        }

        if (curRecord)
        {
          unsigned int curLim = 0;
          unsigned int baseLim = 0;
          switch (byMasteryClass)
          {
            case 0:
              curLim = *reinterpret_cast<unsigned int *>(&curRecord[1].m_strCode[4 * byIndex]);
              if (baseRecord)
              {
                baseLim = *reinterpret_cast<unsigned int *>(&baseRecord[1].m_strCode[4 * byIndex]);
              }
              break;
            case 1:
              curLim = *reinterpret_cast<unsigned int *>(&curRecord[1].m_strCode[12]);
              if (baseRecord)
              {
                baseLim = *reinterpret_cast<unsigned int *>(&baseRecord[1].m_strCode[12]);
              }
              break;
            case 2:
              curLim = *reinterpret_cast<unsigned int *>(&curRecord[1].m_strCode[16]);
              if (baseRecord)
              {
                baseLim = *reinterpret_cast<unsigned int *>(&baseRecord[1].m_strCode[16]);
              }
              break;
            case 3:
              curLim = *reinterpret_cast<unsigned int *>(&curRecord[1].m_strCode[4 * byIndex + 32]);
              if (baseRecord)
              {
                baseLim = *reinterpret_cast<unsigned int *>(&baseRecord[1].m_strCode[4 * byIndex + 32]);
              }
              break;
            case 4:
              curLim = reinterpret_cast<unsigned int *>(&curRecord[2].m_dwIndex)[byIndex];
              if (baseRecord)
              {
                baseLim = reinterpret_cast<unsigned int *>(&baseRecord[2].m_dwIndex)[byIndex];
              }
              break;
            case 5:
              curLim = *reinterpret_cast<unsigned int *>(&curRecord[1].m_strCode[4 * byIndex + 20]);
              if (baseRecord)
              {
                baseLim = *reinterpret_cast<unsigned int *>(&baseRecord[1].m_strCode[4 * byIndex + 20]);
              }
              break;
            case 6:
              curLim = *reinterpret_cast<unsigned int *>(&curRecord[1].m_strCode[8]);
              if (baseRecord)
              {
                baseLim = *reinterpret_cast<unsigned int *>(&baseRecord[1].m_strCode[8]);
              }
              break;
            default:
              break;
          }

          if (static_cast<int>(curLim) <= static_cast<int>(baseLim))
          {
            return baseLim;
          }
          return curLim;
        }

        g_Main.m_logSystemError.Write("_check_mastery_lim.. pCurFld : NULL");
        return 0;
      }

      const int level = static_cast<int>(GetLevel());
      g_Main.m_logSystemError.Write(
        "_check_mastery_lim.. level : %d, max level : %d",
        level,
        maxLevel);
      return 0;
    }

    const int raceCode = m_Param.GetRaceCode();
    g_Main.m_logSystemError.Write("_check_mastery_lim.. racecode : %d", raceCode);
    return 0;
  }

  _class_fld *baseClass = m_Param.GetPtrBaseClass();
  _class_fld *curClass = m_Param.GetPtrCurClass();
  g_Main.m_logSystemError.Write(
    "_check_mastery_lim.. cur_class : %d, base_class : %d",
    curClass->m_nClass,
    baseClass->m_nClass);
  return 0;
}

void CPlayer::apply_case_equip_std_effect(_STORAGE_LIST::_db_con *pItem, bool bEquip)
{
  _ITEM_EFFECT *itemEffect = _GetItemEffect(pItem);
  if (!itemEffect)
  {
    return;
  }

  for (int index = 0; index < 4; ++index)
  {
    apply_normal_item_std_effect(itemEffect[index].nEffectCode, itemEffect[index].fEffectValue, bEquip);
  }
}

_ITEM_EFFECT *CPlayer::_GetItemEffect(_STORAGE_LIST::_db_con *pItem)
{
  if (pItem->m_byTableCode >= 5u)
  {
    const int tableOffset = static_cast<int>(pItem->m_byTableCode) - 5;
    switch (tableOffset)
    {
      case 0:
      {
        auto *record = reinterpret_cast<_ITEM_EFFECT *>(
          g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex));
        return record ? record + 51 : nullptr;
      }
      case 1:
      {
        auto *record = reinterpret_cast<_ITEM_EFFECT *>(
          g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex));
        return record ? record + 89 : nullptr;
      }
      case 2:
      {
        auto *record = reinterpret_cast<_ITEM_EFFECT *>(
          g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex));
        return record ? record + 51 : nullptr;
      }
      case 3:
      {
        auto *record = reinterpret_cast<_ITEM_EFFECT *>(
          g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex));
        return record ? record + 43 : nullptr;
      }
      case 4:
      {
        auto *record = reinterpret_cast<_ITEM_EFFECT *>(
          g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex));
        return record ? record + 43 : nullptr;
      }
      case 22:
      {
        _base_fld *record = g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
        return record ? reinterpret_cast<_ITEM_EFFECT *>(&record[5].m_strCode[28]) : nullptr;
      }
      default:
        return nullptr;
    }
  }

  _base_fld *record = g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
  if (record)
  {
    return reinterpret_cast<_ITEM_EFFECT *>(&record[6]);
  }
  return nullptr;
}

void CPlayer::apply_normal_item_std_effect(int nEffCode, float fVal, bool bEquip)
{
  switch (nEffCode)
  {
    case 1:
      m_EP.SetEff_Rate(11, fVal, bEquip);
      break;
    case 2:
      m_EP.SetEff_Rate(7, fVal, bEquip);
      break;
    case 3:
      for (int paramIndex = 0; paramIndex < 2; ++paramIndex)
      {
        m_EP.SetEff_Plus(paramIndex, fVal, bEquip);
      }
      m_EP.SetEff_Plus(2, fVal, bEquip);
      break;
    case 4:
    case 24:
      m_EP.SetEff_Plus(3, fVal, bEquip);
      break;
    case 5:
      m_EP.SetEff_Rate(9, fVal, bEquip);
      m_EP.SetEff_Rate(10, fVal, bEquip);
      break;
    case 6:
      for (int paramIndex = 0; paramIndex < 2; ++paramIndex)
      {
        m_EP.SetEff_Rate(paramIndex, fVal, bEquip);
        m_EP.SetEff_Rate(paramIndex + 2, fVal, bEquip);
      }
      m_EP.SetEff_Rate(4, fVal, bEquip);
      m_EP.SetEff_Rate(29, fVal, bEquip);
      break;
    case 7:
    case 21:
      m_EP.SetEff_Rate(6, fVal, bEquip);
      break;
    case 8:
      m_EP.SetEff_Plus(19, fVal, bEquip);
      break;
    case 9:
      m_EP.SetEff_Plus(21, 1.0f, bEquip);
      break;
    case 10:
      m_EP.SetEff_Plus(22, 1.0f, bEquip);
      break;
    case 11:
      m_EP.SetEff_Plus(23, 1.0f, bEquip);
      break;
    case 12:
      if (!m_bInGuildBattle || !m_bTakeGravityStone)
      {
        m_EP.SetEff_Plus(20, fVal, bEquip);
      }
      break;
    case 13:
      m_EP.SetEff_Plus(24, 1.0f, bEquip);
      break;
    case 14:
      m_EP.SetEff_Plus(25, fVal, bEquip);
      break;
    case 15:
      m_EP.SetEff_Rate(4, fVal, bEquip);
      break;
    case 16:
      m_EP.SetEff_Rate(10, fVal, bEquip);
      break;
    case 17:
      m_EP.SetEff_Rate(12, fVal, bEquip);
      break;
    case 18:
      m_EP.SetEff_Rate(13, fVal, bEquip);
      break;
    case 19:
      m_EP.SetEff_Plus(14, fVal, bEquip);
      break;
    case 20:
      for (int paramIndex = 0; paramIndex < 2; ++paramIndex)
      {
        m_EP.SetEff_Plus(paramIndex + 4, fVal, bEquip);
        m_EP.SetEff_Plus(paramIndex + 6, fVal, bEquip);
      }
      m_EP.SetEff_Plus(36, fVal, bEquip);
      break;
    case 22:
    case 31:
      m_EP.SetEff_Rate(8, fVal, bEquip);
      break;
    case 23:
      m_EP.SetEff_Rate(14, fVal, bEquip);
      break;
    case 25:
      m_EP.SetEff_Plus(11, fVal, bEquip);
      break;
    case 26:
      m_EP.SetEff_Plus(8, fVal, bEquip);
      break;
    case 27:
      m_EP.SetEff_Plus(37, fVal, bEquip);
      break;
    case 28:
      m_EP.SetEff_Plus(29, fVal, bEquip);
      break;
    case 29:
      m_EP.SetEff_Plus(15, fVal, bEquip);
      m_EP.SetEff_Plus(16, fVal, bEquip);
      m_EP.SetEff_Plus(17, fVal, bEquip);
      m_EP.SetEff_Plus(18, fVal, bEquip);
      break;
    case 30:
      m_EP.SetEff_Rate(9, fVal, bEquip);
      break;
    case 32:
      m_EP.SetEff_Plus(28, fVal, bEquip);
      break;
    case 33:
      m_EP.SetEff_Plus(10, fVal, bEquip);
      break;
    case 34:
      m_EP.SetEff_Plus(12, fVal, bEquip);
      break;
    case 35:
      m_EP.SetEff_Plus(13, fVal, bEquip);
      break;
    case 36:
      m_EP.SetEff_Plus(38, fVal, bEquip);
      break;
    default:
      return;
  }
}

void CPlayer::apply_case_equip_upgrade_effect(_STORAGE_LIST::_db_con *pItem, bool bEquip)
{
  unsigned int dwLvBit[4]{};
  memcpy_0(dwLvBit, &pItem->m_dwLv, sizeof(dwLvBit[0]));

  const unsigned __int8 itemUpgedLv = GetItemUpgedLv(dwLvBit[0]);
  if (!itemUpgedLv || !GetDefItemUpgSocketNum(pItem->m_byTableCode, pItem->m_wItemIndex))
  {
    float effectValue = 0.0f;
    const float have79 = m_EP.GetEff_Have(79);
    if (have79 > 0.0f
        && have79 < 6.0f
        && pItem->m_byTableCode == 6
        && GetItemGrade(pItem->m_byTableCode, pItem->m_wItemIndex) < 3u)
    {
      _ItemUpgrade_fld *record = g_Main.m_tblItemUpgrade.GetRecord(0);
      const int effIndex = static_cast<int>(have79) - 1;
      effectValue = (&record->m_fUp1)[effIndex];

      for (int paramIndex = 0; paramIndex < 2; ++paramIndex)
      {
        m_EP.SetEff_Rate(paramIndex, effectValue, bEquip);
        m_EP.SetEff_Rate(paramIndex + 2, effectValue, bEquip);
      }
      m_EP.SetEff_Rate(4, effectValue, bEquip);
      m_EP.SetEff_Rate(29, effectValue, bEquip);
    }
    else
    {
      const float have80 = m_EP.GetEff_Have(80);
      if (have80 > 0.0f && have80 < 5.0f && pItem->m_byTableCode <= 5u)
      {
        _ItemUpgrade_fld *record = g_Main.m_tblItemUpgrade.GetRecord(5u);
        effectValue = reinterpret_cast<float *>(&record->m_nEffectUnit)[static_cast<int>(have80)];
        m_EP.SetEff_Rate(6, effectValue, bEquip);
        if (bEquip)
        {
          m_fTalik_DefencePoint += effectValue;
        }
        else
        {
          m_fTalik_DefencePoint -= effectValue;
        }
      }
    }
  }

  if (!GetDefItemUpgSocketNum(pItem->m_byTableCode, pItem->m_wItemIndex))
  {
    return;
  }

  for (int socketIndex = 0; socketIndex < itemUpgedLv; ++socketIndex)
  {
    const unsigned __int8 talik = GetTalikFromSocket(dwLvBit[0], static_cast<unsigned __int8>(socketIndex));
    if (talik == 15)
    {
      continue;
    }

    int talikCount = 1;
    for (int nextIndex = socketIndex + 1; nextIndex < itemUpgedLv; ++nextIndex)
    {
      const unsigned __int8 nextTalik = GetTalikFromSocket(dwLvBit[0], static_cast<unsigned __int8>(nextIndex));
      if (talik == nextTalik)
      {
        ++talikCount;
        dwLvBit[0] |= 15u << (4 * nextIndex);
      }
    }

    _ItemUpgrade_fld *record = g_Main.m_tblItemUpgrade.GetRecord(talik);
    if (!record || talikCount < 1 || talikCount > 7)
    {
      continue;
    }

    float talikValue = reinterpret_cast<float *>(&record->m_nEffectUnit)[talikCount];
    switch (talik)
    {
      case 0:
      {
        int effHave79 = static_cast<int>(m_EP.GetEff_Have(79));
        if (effHave79 > 5)
        {
          effHave79 = 5;
        }
        if (effHave79 > talikCount
            && GetItemGrade(pItem->m_byTableCode, pItem->m_wItemIndex) < 3u
            && pItem->m_byTableCode == 6)
        {
          talikValue = reinterpret_cast<float *>(&record->m_nEffectUnit)[effHave79];
        }

        for (int paramIndex = 0; paramIndex < 2; ++paramIndex)
        {
          m_EP.SetEff_Rate(paramIndex, talikValue, bEquip);
          m_EP.SetEff_Rate(paramIndex + 2, talikValue, bEquip);
        }
        m_EP.SetEff_Rate(4, talikValue, bEquip);
        m_EP.SetEff_Rate(29, talikValue, bEquip);
        break;
      }
      case 1:
        m_EP.SetEff_Rate(12, talikValue, bEquip);
        break;
      case 2:
        m_EP.SetEff_Plus(28, talikValue, bEquip);
        break;
      case 3:
        if (pItem->m_byTableCode == 7)
        {
          m_EP.SetEff_Plus(37, talikValue, bEquip);
        }
        else
        {
          m_EP.SetEff_Plus(14, talikValue, bEquip);
        }
        break;
      case 4:
        m_EP.SetEff_Plus(5, talikValue, bEquip);
        break;
      case 5:
      {
        int effHave80 = static_cast<int>(m_EP.GetEff_Have(80));
        if (effHave80 > 4)
        {
          effHave80 = 4;
        }
        if (talikCount < effHave80 && pItem->m_byTableCode <= 5u)
        {
          talikValue = reinterpret_cast<float *>(&record->m_nEffectUnit)[effHave80];
        }
        m_EP.SetEff_Rate(6, talikValue, bEquip);
        if (bEquip)
        {
          m_fTalik_DefencePoint += talikValue;
        }
        else
        {
          m_fTalik_DefencePoint -= talikValue;
        }
        break;
      }
      case 6:
        m_EP.SetEff_Plus(38, talikValue, bEquip);
        break;
      case 7:
        if (pItem->m_byTableCode != 6)
        {
          m_EP.SetEff_Plus(15, talikValue, bEquip);
        }
        break;
      case 8:
        if (pItem->m_byTableCode != 6)
        {
          m_EP.SetEff_Plus(16, talikValue, bEquip);
        }
        break;
      case 9:
        if (pItem->m_byTableCode != 6)
        {
          m_EP.SetEff_Plus(17, talikValue, bEquip);
        }
        break;
      case 10:
        if (pItem->m_byTableCode != 6)
        {
          m_EP.SetEff_Plus(18, talikValue, bEquip);
        }
        break;
      case 11:
        for (int paramIndex = 0; paramIndex < 2; ++paramIndex)
        {
          m_EP.SetEff_Plus(paramIndex, talikValue, bEquip);
        }
        m_EP.SetEff_Plus(2, talikValue, bEquip);
        m_EP.SetEff_Plus(31, talikValue, bEquip);
        m_EP.SetEff_Plus(30, talikValue, bEquip);
        break;
      case 12:
      {
        const float halfValue = talikValue / 2.0f;
        m_EP.SetEff_Plus(3, halfValue, bEquip);
        if (bEquip)
        {
          m_fTalik_AvoidPoint += halfValue;
        }
        else
        {
          m_fTalik_AvoidPoint -= halfValue;
        }
        break;
      }
      default:
        break;
    }
  }
}

void CPlayer::CalcDefTol()
{
  memset_0(m_nTolValue, 0, sizeof(m_nTolValue));

  if (!IsRidingUnit())
  {
    for (int partIndex = 0; partIndex < 5; ++partIndex)
    {
      _STORAGE_LIST::_db_con *equipItem = &m_Param.m_dbEquip.m_pStorageList[partIndex];
      if (equipItem->m_bLoad)
      {
        _base_fld *record = g_Main.m_tblItemData[partIndex].GetRecord(equipItem->m_wItemIndex);
        if (record)
        {
          char *tolPtr = &record[5].m_strCode[28];
          for (int tolIndex = 0; tolIndex < 4; ++tolIndex)
          {
            const float tolValue = *reinterpret_cast<float *>(&tolPtr[4 * tolIndex]);
            m_nTolValue[tolIndex] = static_cast<int>(static_cast<float>(m_nTolValue[tolIndex]) + tolValue);
          }
        }
      }
    }

    _STORAGE_LIST::_db_con *cloakItem = &m_Param.m_dbEquip.m_pStorageList[7];
    if (cloakItem->m_bLoad)
    {
      _base_fld *record = g_Main.m_tblItemData[7].GetRecord(cloakItem->m_wItemIndex);
      if (record)
      {
        char *tolPtr = &record[5].m_strCode[28];
        for (int tolIndex = 0; tolIndex < 4; ++tolIndex)
        {
          const float tolValue = *reinterpret_cast<float *>(&tolPtr[4 * tolIndex]);
          m_nTolValue[tolIndex] = static_cast<int>(static_cast<float>(m_nTolValue[tolIndex]) + tolValue);
        }
      }
    }

    _STORAGE_LIST::_db_con *shieldItem = &m_Param.m_dbEquip.m_pStorageList[5];
    if (shieldItem->m_bLoad)
    {
      _base_fld *record = g_Main.m_tblItemData[5].GetRecord(shieldItem->m_wItemIndex);
      if (record)
      {
        char *tolPtr = &record[5].m_strCode[28];
        for (int tolIndex = 0; tolIndex < 4; ++tolIndex)
        {
          const float tolValue = *reinterpret_cast<float *>(&tolPtr[4 * tolIndex]);
          m_nTolValue[tolIndex] = static_cast<int>(static_cast<float>(m_nTolValue[tolIndex]) + tolValue);
        }
      }
    }

    for (int embelIndex = 0; embelIndex < 7; ++embelIndex)
    {
      _STORAGE_LIST::_db_con *embelItem = &m_Param.m_dbEmbellish.m_pStorageList[embelIndex];
      if (!embelItem->m_bLoad)
      {
        continue;
      }

      char *tolPtr = nullptr;
      if (embelItem->m_byTableCode == 8)
      {
        _base_fld *record = g_Main.m_tblItemData[8].GetRecord(embelItem->m_wItemIndex);
        if (!record)
        {
          continue;
        }
        tolPtr = &record[4].m_strCode[52];
      }
      else if (embelItem->m_byTableCode == 9)
      {
        _base_fld *record = g_Main.m_tblItemData[9].GetRecord(embelItem->m_wItemIndex);
        if (!record)
        {
          continue;
        }
        tolPtr = &record[4].m_strCode[52];
      }
      else
      {
        continue;
      }

      for (int tolIndex = 0; tolIndex < 4; ++tolIndex)
      {
        const float tolValue = *reinterpret_cast<float *>(&tolPtr[4 * tolIndex]);
        m_nTolValue[tolIndex] = static_cast<int>(static_cast<float>(m_nTolValue[tolIndex]) + tolValue);
      }
    }

    return;
  }

  for (int partIndex = 0; partIndex < 6; ++partIndex)
  {
    _base_fld *record = g_Main.m_tblUnitPart[partIndex].GetRecord(m_pUsingUnit->byPart[partIndex]);
    if (!record)
    {
      continue;
    }

    char *tolPtr = &record[5].m_strCode[20];
    for (int tolIndex = 0; tolIndex < 4; ++tolIndex)
    {
      m_nTolValue[tolIndex] += *reinterpret_cast<int *>(&tolPtr[4 * tolIndex]);
    }
  }
}

void CPlayer::CalcEquipSpeed()
{
  const float prevEquipSpeed = m_fEquipSpeed;
  m_fEquipSpeed = FLOAT_1_0;

  for (int partIndex = 0; partIndex < 5; ++partIndex)
  {
    _STORAGE_LIST::_db_con *equipItem = &m_Param.m_dbEquip.m_pStorageList[partIndex];
    if (!equipItem->m_bLoad)
    {
      continue;
    }
    _base_fld *record = g_Main.m_tblItemData[partIndex].GetRecord(equipItem->m_wItemIndex);
    if (record)
    {
      m_fEquipSpeed *= *reinterpret_cast<float *>(&record[5].m_strCode[4]);
    }
  }

  _STORAGE_LIST::_db_con *weaponItem = &m_Param.m_dbEquip.m_pStorageList[6];
  if (weaponItem->m_bLoad)
  {
    _base_fld *record = g_Main.m_tblItemData[6].GetRecord(weaponItem->m_wItemIndex);
    if (record)
    {
      m_fEquipSpeed *= *reinterpret_cast<float *>(&record[9].m_strCode[4]);
    }
  }

  _STORAGE_LIST::_db_con *shieldItem = &m_Param.m_dbEquip.m_pStorageList[5];
  if (shieldItem->m_bLoad)
  {
    _base_fld *record = g_Main.m_tblItemData[5].GetRecord(shieldItem->m_wItemIndex);
    if (record)
    {
      m_fEquipSpeed *= *reinterpret_cast<float *>(&record[5].m_strCode[4]);
    }
  }

  if (m_fEquipSpeed != prevEquipSpeed)
  {
    SendMsg_AlterEquipSPInform();
  }
}

void CPlayer::SendMsg_AlterEquipSPInform()
{
  struct
  {
    float equipSpeed;
  } msg{};

  msg.equipSpeed = m_fEquipSpeed;

  unsigned __int8 type[2] = {11, 13};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 4u);
}

void CPlayer::CalcEquipMaxDP(int bSendMsg)
{
  int maxDp = 1;
  for (int partIndex = 0; partIndex < 5; ++partIndex)
  {
    _STORAGE_LIST::_db_con *equipItem = &m_Param.m_dbEquip.m_pStorageList[partIndex];
    _base_fld *record = nullptr;
    if (equipItem->m_bLoad)
    {
      record = g_Main.m_tblItemData[partIndex].GetRecord(equipItem->m_wItemIndex);
    }
    else
    {
      record = g_Main.m_tblItemData[partIndex].GetRecord(m_Param.m_dbChar.m_byDftPart[partIndex]);
    }
    if (record)
    {
      maxDp += *reinterpret_cast<int *>(&record[5].m_strCode[52]);
    }
  }

  if (m_nMaxDP != maxDp)
  {
    m_nMaxDP = maxDp;
    if (GetDP() > m_nMaxDP)
    {
      SetDP(m_nMaxDP, false);
      SendMsg_SetDPInform();
    }
    if (!bSendMsg)
    {
      SendMsg_AlterMaxDP();
    }
  }
}

int CPlayer::GetMaxDP()
{
  const float baseMax = static_cast<float>(m_nMaxDP);
  const float bonus = m_EP.GetEff_Plus(35);
  return static_cast<int>(baseMax + bonus);
}

int CPlayer::GetDP()
{
  return m_Param.GetDP();
}

char CPlayer::SetDP(int nDP, bool bOver)
{
  const int currentDp = m_Param.GetDP();
  if (!bOver && nDP > currentDp)
  {
    const int maxDp = GetMaxDP();
    if (currentDp >= maxDp || nDP <= GetMaxDP())
    {
      const int currentMax = GetMaxDP();
      if (currentDp >= currentMax && nDP >= currentDp)
      {
        return 0;
      }
    }
    else
    {
      nDP = GetMaxDP();
    }
  }

  if (nDP < 0)
  {
    nDP = 0;
  }
  if (currentDp == nDP)
  {
    return 0;
  }

  m_Param.SetDP(nDP);
  return 1;
}

void CPlayer::SendMsg_SetDPInform()
{
  struct
  {
    unsigned __int16 dp;
  } msg{};

  msg.dp = static_cast<unsigned __int16>(m_Param.GetDP());

  unsigned __int8 type[2] = {17, 114};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 2u);
}

void CPlayer::SendMsg_AlterMaxDP()
{
  struct
  {
    unsigned __int16 maxDp;
  } msg{};

  msg.maxDp = static_cast<unsigned __int16>(GetMaxDP());

  unsigned __int8 type[2] = {11, static_cast<unsigned __int8>(-123)};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 2u);
}

void CPlayer::SetSiege(_STORAGE_LIST::_db_con *pItem)
{
  if (pItem)
  {
    if (m_pSiegeItem)
    {
      return;
    }
    m_pSiegeItem = pItem;
    m_pSiegeItem->lock(true);
    CashChangeStateFlag changeFlag(0);
    UpdateVisualVer(changeFlag);
    apply_case_equip_std_effect(m_pSiegeItem, true);
    SendMsg_TransformSiegeModeResult(0);
  }
  else
  {
    if (!m_pSiegeItem)
    {
      return;
    }
    apply_case_equip_std_effect(m_pSiegeItem, false);
    m_pSiegeItem->lock(false);
    m_pSiegeItem = nullptr;
    CashChangeStateFlag changeFlag(0);
    UpdateVisualVer(changeFlag);
    SendMsg_ReleaseSiegeModeResult(0);
  }

  if (m_bMove)
  {
    CCharacter::Stop();
    CGameObject::SendMsg_BreakStop();
  }
}

void CPlayer::SendMsg_TransformSiegeModeResult(char byRetCode)
{
  char msg[1]{};
  msg[0] = byRetCode;

  unsigned __int8 type[2] = {28, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 1u);

  if (byRetCode)
  {
    return;
  }

#pragma pack(push, 1)
  struct SiegeTransformMsg
  {
    unsigned int objSerial;
    unsigned __int16 equipItemIndex;
    unsigned __int8 siegeItemIndex;
    unsigned __int16 visualVer;
  };
#pragma pack(pop)

  SiegeTransformMsg circleMsg{};
  circleMsg.objSerial = m_dwObjSerial;
  circleMsg.equipItemIndex = m_Param.m_dbEquip.m_pStorageList[6].m_wItemIndex;
  circleMsg.siegeItemIndex = static_cast<unsigned __int8>(m_pSiegeItem->m_wItemIndex);
  circleMsg.visualVer = GetVisualVer();

  unsigned __int8 circleType[2] = {28, 3};
  CGameObject::CircleReport(circleType, reinterpret_cast<char *>(&circleMsg), sizeof(circleMsg), 0);
}

void CPlayer::SendMsg_ReleaseSiegeModeResult(char byRetCode)
{
  char msg[1]{};
  msg[0] = byRetCode;

  unsigned __int8 type[2] = {28, 5};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 1u);

  if (byRetCode)
  {
    return;
  }

#pragma pack(push, 1)
  struct SiegeReleaseMsg
  {
    unsigned int objSerial;
    unsigned __int16 visualVer;
  };
#pragma pack(pop)

  SiegeReleaseMsg circleMsg{};
  circleMsg.objSerial = m_dwObjSerial;
  circleMsg.visualVer = GetVisualVer();

  unsigned __int8 circleType[2] = {28, 6};
  CGameObject::CircleReport(circleType, reinterpret_cast<char *>(&circleMsg), sizeof(circleMsg), 0);
}

void CPlayer::SetHaveEffect(char bLogin)
{
  if (!m_EP.m_pDataParam)
  {
    return;
  }

  const float prevHave4 = m_EP.GetEff_Have(4);
  const float prevHave77 = m_EP.GetEff_Have(77);

  for (int effCode = 12; effCode < 83; ++effCode)
  {
    if ((effCode < 59 || effCode > 65) && m_EP.m_pDataParam->m_fEff_Have[effCode] != 0.0f)
    {
      if (effCode == 79 || effCode == 80)
      {
        apply_have_item_std_effect(effCode, m_EP.m_pDataParam->m_fEff_Have[effCode], false, bLogin);
      }
      else
      {
        apply_have_item_std_effect(effCode, m_EP.m_pDataParam->m_fEff_Have[effCode], false, false);
      }
    }
  }

  for (int slotIndex = 0; ; ++slotIndex)
  {
    const unsigned __int8 bagNum = m_Param.GetBagNum();
    if (slotIndex >= 20 * bagNum)
    {
      break;
    }

    _STORAGE_LIST::_db_con *item = &m_Param.m_dbInven.m_pStorageList[slotIndex];
    if (item->m_bLoad && item->m_byTableCode == 18)
    {
      _base_fld *record = g_Main.m_tblItemData[item->m_byTableCode].GetRecord(item->m_wItemIndex);
      if (record && *reinterpret_cast<int *>(&record[5].m_strCode[44]) != -1)
      {
        TimeLimitJadeMng::Instance()->DeleteList(m_ObjID.m_wIndex, item);
      }
    }
  }

  m_EP.InitEffHave();

  for (int slotIndex = 0; ; ++slotIndex)
  {
    const unsigned __int8 bagNum = m_Param.GetBagNum();
    if (slotIndex >= 20 * bagNum)
    {
      break;
    }

    _STORAGE_LIST::_db_con *item = &m_Param.m_dbInven.m_pStorageList[slotIndex];
    if (!item->m_bLoad || item->m_byTableCode != 18)
    {
      continue;
    }

    _base_fld *record = g_Main.m_tblItemData[item->m_byTableCode].GetRecord(item->m_wItemIndex);
    if (!record)
    {
      continue;
    }

    const int cashType = GetUsePcCashType(item->m_byTableCode, item->m_wItemIndex);
    if (IsUsableAccountType(cashType))
    {
      if (*reinterpret_cast<int *>(&record[5].m_strCode[44]) == -1 || IsRidingUnit())
      {
        if (*reinterpret_cast<int *>(&record[5].m_strCode[52]) != 1)
        {
          for (int effIndex = 0; effIndex < static_cast<int>(record[6].m_dwIndex); ++effIndex)
          {
            const int effectCode = *reinterpret_cast<int *>(&record[6].m_strCode[12 * effIndex]);
            if (effectCode > -1
                && (effectCode < 59 || effectCode > 65)
                && (!IsRidingUnit() || effectCode >= 29 || effectCode <= 32)
                && effectCode < 83
                && m_EP.m_pDataParam->m_fEff_Have[effectCode]
                  < *reinterpret_cast<float *>(&record[6].m_strCode[12 * effIndex + 8]))
            {
              const signed __int64 dur = static_cast<signed __int64>(item->m_dwDur);
              float durValue = static_cast<float>(static_cast<int>(dur));
              if (dur < 0)
              {
                durValue += 1.8446744e19f;
              }

              m_EP.m_pDataParam->m_fEff_Have[effectCode] +=
                *reinterpret_cast<float *>(&record[6].m_strCode[12 * effIndex + 4]) * durValue;

              const float maxValue = *reinterpret_cast<float *>(&record[6].m_strCode[12 * effIndex + 8]);
              if (m_EP.m_pDataParam->m_fEff_Have[effectCode] > maxValue)
              {
                m_EP.m_pDataParam->m_fEff_Have[effectCode] = maxValue;
              }
            }
          }
        }
      }
      else
      {
        TimeLimitJadeMng::Instance()->InsertList(m_ObjID.m_wIndex, item);
      }
    }
    else if (bLogin)
    {
      SendMsg_PremiumCashItemUse(item->m_wSerial);
    }
  }

  m_EP.m_pDataParam->m_fEff_Have[0] += FLOAT_1_0;
  m_EP.m_pDataParam->m_fEff_Have[2] += FLOAT_1_0;
  m_EP.m_pDataParam->m_fEff_Have[5] += FLOAT_1_0;
  m_EP.m_pDataParam->m_fEff_Have[6] += FLOAT_1_0;
  m_EP.m_pDataParam->m_fEff_Have[7] += FLOAT_1_0;
  m_EP.m_pDataParam->m_fEff_Have[8] += FLOAT_1_0;
  m_EP.m_pDataParam->m_fEff_Have[9] += FLOAT_1_0;

  for (int effCode = 12; effCode < 83; ++effCode)
  {
    if ((effCode < 59 || effCode > 65) && m_EP.m_pDataParam->m_fEff_Have[effCode] != 0.0f)
    {
      if (effCode == 77)
      {
        float diff = (m_EP.m_pDataParam->m_fEff_Have[77] - prevHave77) * 100.0f;
        apply_have_item_std_effect(77, m_EP.m_pDataParam->m_fEff_Have[77], true, static_cast<int>(diff));
      }
      if (effCode == 79 || effCode == 80)
      {
        apply_have_item_std_effect(effCode, m_EP.m_pDataParam->m_fEff_Have[effCode], true, bLogin);
      }
      else
      {
        apply_have_item_std_effect(effCode, m_EP.m_pDataParam->m_fEff_Have[effCode], true, false);
      }
    }
  }

  m_EP.m_pDataParam->m_fEff_Have[71] += FLOAT_1_0;
  m_EP.m_pDataParam->m_fEff_Have[72] += FLOAT_1_0;
  m_EP.m_pDataParam->m_fEff_Have[73] += FLOAT_1_0;
  m_EP.m_pDataParam->m_fEff_Have[74] += FLOAT_1_0;
  m_EP.m_pDataParam->m_fEff_Have[75] += FLOAT_1_0;

  const float afterHave4 = m_EP.GetEff_Have(4);
  if (afterHave4 <= prevHave4)
  {
    if (prevHave4 > m_EP.GetEff_Have(4))
    {
      m_bDownCheckEquipEffect = 1;
    }
  }
  else
  {
    m_bUpCheckEquipEffect = 1;
  }

  if (m_EP.m_pDataParam->m_fEff_Have[2] > FLOAT_1_0)
  {
    float diff = (m_EP.m_pDataParam->m_fEff_Have[2] - FLOAT_1_0) * 10.0f;
    const int count = static_cast<int>(diff) % 10;
    s_MgrItemHistory.exp_prof_log(count, m_szItemHistoryFileName);
  }
}

void CPlayer::apply_have_item_std_effect(int nEffCode, float fVal, bool bAdd, int nDiffCnt)
{
  switch (nEffCode)
  {
    case 15:
      m_EP.SetEff_Rate(9, fVal, bAdd);
      break;
    case 16:
      m_EP.SetEff_Rate(10, fVal, bAdd);
      break;
    case 17:
      m_EP.SetEff_Rate(11, fVal, bAdd);
      break;
    case 18:
      m_EP.SetEff_Rate(0, fVal, bAdd);
      m_EP.SetEff_Rate(1, fVal, bAdd);
      m_EP.SetEff_Rate(2, fVal, bAdd);
      m_EP.SetEff_Rate(3, fVal, bAdd);
      m_EP.SetEff_Rate(4, fVal, bAdd);
      m_EP.SetEff_Rate(29, fVal, bAdd);
      break;
    case 19:
      m_EP.SetEff_Rate(0, fVal, bAdd);
      break;
    case 20:
      m_EP.SetEff_Rate(2, fVal, bAdd);
      break;
    case 21:
      m_EP.SetEff_Rate(1, fVal, bAdd);
      break;
    case 22:
      m_EP.SetEff_Rate(3, fVal, bAdd);
      break;
    case 23:
      m_EP.SetEff_Rate(4, fVal, bAdd);
      break;
    case 24:
      m_EP.SetEff_Rate(6, fVal, bAdd);
      break;
    case 25:
      m_EP.SetEff_Rate(12, fVal, bAdd);
      break;
    case 26:
      m_EP.SetEff_Rate(13, fVal, bAdd);
      break;
    case 27:
      m_EP.SetEff_Rate(40, fVal, bAdd);
      break;
    case 29:
      m_EP.SetEff_Rate(41, fVal, bAdd);
      break;
    case 30:
      m_EP.SetEff_Rate(42, fVal, bAdd);
      break;
    case 31:
      m_EP.SetEff_Rate(43, fVal, bAdd);
      break;
    case 32:
      m_EP.SetEff_Rate(44, fVal, bAdd);
      break;
    case 33:
      m_EP.SetEff_Rate(57, fVal, bAdd);
      break;
    case 34:
      m_EP.SetEff_Rate(58, fVal, bAdd);
      break;
    case 35:
      m_EP.SetEff_Rate(59, fVal, bAdd);
      break;
    case 36:
      m_EP.SetEff_Rate(60, fVal, bAdd);
      break;
    case 37:
      m_EP.SetEff_Rate(61, fVal, bAdd);
      break;
    case 38:
      m_EP.SetEff_Rate(0, fVal, bAdd);
      m_EP.SetEff_Rate(1, fVal, bAdd);
      break;
    case 39:
      m_EP.SetEff_Plus(6, fVal, bAdd);
      m_EP.SetEff_Plus(7, fVal, bAdd);
      break;
    case 40:
      m_EP.SetEff_Plus(23, 1.0f, bAdd);
      break;
    case 41:
      m_EP.SetEff_Plus(20, fVal, bAdd);
      break;
    case 43:
      m_EP.SetEff_Plus(0, fVal, bAdd);
      break;
    case 44:
      m_EP.SetEff_Plus(1, fVal, bAdd);
      break;
    case 45:
      m_EP.SetEff_Plus(31, fVal, bAdd);
      break;
    case 46:
      m_EP.SetEff_Plus(30, fVal, bAdd);
      break;
    case 47:
      m_EP.SetEff_Plus(14, fVal, bAdd);
      break;
    case 48:
      m_EP.SetEff_Plus(40, fVal, bAdd);
      break;
    case 49:
      m_EP.SetEff_Plus(3, fVal, bAdd);
      break;
    case 50:
      HideNameEffect(bAdd);
      break;
    case 56:
      m_EP.SetEff_State(8, bAdd);
      break;
    case 57:
      m_EP.SetEff_Plus(29, fVal, bAdd);
      break;
    case 59:
      SetMstPt(0, fVal, bAdd, 0);
      break;
    case 60:
      SetMstPt(0, fVal, bAdd, 1);
      break;
    case 61:
      SetMstPt(4, fVal, bAdd, 0);
      break;
    case 62:
      SetMstPt(1, fVal, bAdd, 0);
      break;
    case 63:
      SetMstPt(2, fVal, bAdd, 0);
      break;
    case 64:
    case 65:
      SetMstPt(6, fVal, bAdd, 0);
      break;
    case 76:
      m_EP.SetEff_Plus(22, fVal, bAdd);
      break;
    case 77:
      if (nDiffCnt > 0 && bAdd)
      {
        const float reduceValue = static_cast<float>(nDiffCnt) * 0.01f;
        DecHalfSFContDam(reduceValue);
      }
      break;
    case 78:
      m_EP.SetEff_Plus(28, fVal, bAdd);
      break;
    case 79:
      if (nDiffCnt != 1 && fVal >= 1.0f)
      {
        SetEquipJadeEffect(79, fVal, bAdd);
      }
      break;
    case 80:
      if (nDiffCnt != 1 && fVal >= 1.0f)
      {
        SetEquipJadeEffect(80, fVal, bAdd);
      }
      break;
    default:
      return;
  }
}

void CPlayer::HideNameEffect(bool bAdd)
{
  const unsigned int dwSerial = m_Param.GetCharSerial();
  const int raceCode = m_Param.GetRaceCode();
  CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
  const unsigned __int8 bossType = ranking->GetBossType(static_cast<unsigned __int8>(raceCode), dwSerial);

  if (!bossType || bossType == 1 || bossType == 5)
  {
    m_EP.m_pDataParam->m_fEff_Have[50] = 0.0f;
  }
  else if (bAdd)
  {
    if (m_pPartyMgr->IsPartyMode())
    {
      pc_PartyLeaveSelfReqeuest();
    }
    for (int j = 0; j < 50; ++j)
    {
      _BUDDY_LIST::__list *buddy = &m_pmBuddy.m_List[j];
      if (buddy->fill() && buddy->pPtr && buddy->pPtr->m_pmBuddy.SearchBuddyLogoff(m_dwObjSerial))
      {
        buddy->pPtr->SendMsg_BuddyLogoffInform(m_dwObjSerial);
      }
    }
    if (m_Param.m_pGuild)
    {
      m_Param.m_pGuild->LogoffMember(m_dwObjSerial);
      m_Param.m_pGuild->SendMsg_GuildMemberLogoff(m_dwObjSerial);
    }
    if (m_Param.m_pApplyGuild)
    {
      m_Param.m_pApplyGuild->PopApplier(m_dwObjSerial, 2u);
    }
  }
  else
  {
    for (int k = 0; k < 50; ++k)
    {
      _BUDDY_LIST::__list *buddy = &m_pmBuddy.m_List[k];
      if (buddy->fill())
      {
        for (int m = 0; m < MAX_PLAYER; ++m)
        {
          CPlayer *ptr = &g_Player[m];
          if (ptr->m_bLive && ptr != this && ptr->m_dwObjSerial == buddy->dwSerial)
          {
            if (ptr->m_pmBuddy.IsBuddy(m_dwObjSerial))
            {
              const char *name = ptr->m_Param.GetCharNameW();
              buddy->ON(const_cast<char *>(name), ptr);
              const char *selfName = m_Param.GetCharNameW();
              ptr->m_pmBuddy.SearchBuddyLogin(this, m_dwObjSerial, const_cast<char *>(selfName));
              ptr->SendMsg_BuddyLoginInform(m_dwObjSerial, m_wRegionMapIndex, m_wRegionIndex);
            }
            break;
          }
        }
      }
    }
    if (m_Param.m_pGuild)
    {
      _guild_member_info *member = m_Param.m_pGuild->LoginMember(m_dwObjSerial, this);
      if (member)
      {
        m_Param.m_pGuildMemPtr = member;
        m_Param.m_pGuild->SendMsg_GuildMemberLogin(m_dwObjSerial, m_wRegionMapIndex, m_wRegionIndex);
        CPlayer *pkPlayer = &g_Player[m_ObjID.m_wIndex];
        CGuildBattleController::Instance()->LogIn(pkPlayer);
        if (member->byClassInGuild == 2)
        {
          CGuildBattleController::Instance()->SendPossibleBattleGuildListFirst(
            m_ObjID.m_wIndex,
            static_cast<unsigned __int8>(m_Param.GetRaceCode()));
        }
        if (member->byClassInGuild == 2)
        {
          const unsigned __int8 grade = m_Param.m_pGuild->GetGrade();
          CGuildMasterEffect::GetInstance()->in_player(this, grade);
        }
      }
      else
      {
        m_Param.m_pGuild = nullptr;
      }
    }
    if (m_Param.m_pApplyGuild)
    {
      m_Param.m_pApplyGuild->PushApplier(this);
    }
  }
}

void CPlayer::SetEquipJadeEffect(int nParam, float fCurVal, bool bAdd)
{
  (void)nParam;

  for (int slotIndex = 0; slotIndex < 7; ++slotIndex)
  {
    _STORAGE_LIST::_db_con *item = &m_Param.m_pStoragePtr[1]->m_pStorageList[slotIndex];
    if (item && item->m_bLoad)
    {
      unsigned int lvBits = item->m_dwLv;
      const unsigned __int8 upgradeLevel = GetItemUpgedLv(lvBits);
      if (upgradeLevel && GetDefItemUpgSocketNum(item->m_byTableCode, item->m_wItemIndex))
      {
        for (int socketIndex = 0; socketIndex < upgradeLevel; ++socketIndex)
        {
          const unsigned __int8 talikType = GetTalikFromSocket(lvBits, static_cast<unsigned __int8>(socketIndex));
          if (talikType != 15)
          {
            int sameCount = 1;
            for (int nextIndex = socketIndex + 1; nextIndex < upgradeLevel; ++nextIndex)
            {
              const unsigned __int8 nextTalik = GetTalikFromSocket(lvBits, static_cast<unsigned __int8>(nextIndex));
              if (talikType == nextTalik)
              {
                ++sameCount;
                lvBits |= 15u << (4 * nextIndex);
              }
            }

            _ItemUpgrade_fld *record = g_Main.m_tblItemUpgrade.GetRecord(talikType);
            if (record && sameCount >= 1 && sameCount <= 7)
            {
              const float baseValue = reinterpret_cast<float *>(&record->m_nEffectUnit)[sameCount];
              float deltaValue = 0.0f;
              if (talikType)
              {
                if (talikType == 5 && item->m_byTableCode != 6)
                {
                  if (fCurVal > 0.0f && sameCount < static_cast<int>(fCurVal))
                  {
                    deltaValue =
                      reinterpret_cast<float *>(&record->m_nEffectUnit)[static_cast<int>(fCurVal)] - baseValue;
                  }
                  m_EP.SetEff_Rate(6, deltaValue, bAdd);
                  m_fTalik_DefencePoint = m_fTalik_DefencePoint + deltaValue;
                }
              }
              else if (item->m_byTableCode == 6)
              {
                if (fCurVal > 0.0f && sameCount < static_cast<int>(fCurVal))
                {
                  deltaValue =
                    reinterpret_cast<float *>(&record->m_nEffectUnit)[static_cast<int>(fCurVal)] - baseValue;
                }
                for (int paramIndex = 0; paramIndex < 2; ++paramIndex)
                {
                  m_EP.SetEff_Rate(paramIndex, deltaValue, bAdd);
                  m_EP.SetEff_Rate(paramIndex + 2, deltaValue, bAdd);
                }
                m_EP.SetEff_Rate(4, deltaValue, bAdd);
                m_EP.SetEff_Rate(29, deltaValue, bAdd);
              }
            }
          }
        }
      }
      else
      {
        float effectValue = 0.0f;
        if (fCurVal > 0.0f
            && fCurVal < 6.0f
            && item->m_byTableCode == 6
            && GetItemGrade(item->m_byTableCode, item->m_wItemIndex) < 3u)
        {
          _ItemUpgrade_fld *record = g_Main.m_tblItemUpgrade.GetRecord(0);
          effectValue = reinterpret_cast<float *>(&record->m_nEffectUnit)[static_cast<int>(fCurVal)];
          for (int paramIndex = 0; paramIndex < 2; ++paramIndex)
          {
            m_EP.SetEff_Rate(paramIndex, effectValue, bAdd);
            m_EP.SetEff_Rate(paramIndex + 2, effectValue, bAdd);
          }
          m_EP.SetEff_Rate(4, effectValue, bAdd);
          m_EP.SetEff_Rate(29, effectValue, bAdd);
        }
        else if (fCurVal > 0.0f && fCurVal < 5.0f && item->m_byTableCode <= 5u)
        {
          _ItemUpgrade_fld *record = g_Main.m_tblItemUpgrade.GetRecord(5u);
          effectValue = reinterpret_cast<float *>(&record->m_nEffectUnit)[static_cast<int>(fCurVal)];
          m_EP.SetEff_Rate(6, effectValue, bAdd);
          m_fTalik_DefencePoint = bAdd ? (m_fTalik_DefencePoint + effectValue)
                                       : (m_fTalik_DefencePoint - effectValue);
        }
      }
    }
  }
}

bool CPlayer::DecHalfSFContDam(float fEffVal)
{
  const unsigned int curTime = _sf_continous::GetSFContCurTime();
  int hitCount = 0;
  int updatedCount = 0;

  for (int index = 0; index < 8; ++index)
  {
    _sf_continous *cont = &m_SFCont[0][index];
    if (cont->m_bExist)
    {
      _base_fld *record = g_Main.m_tblEffectData[3].GetRecord("17");
      if (!record
          || !m_bAfterEffect
          || cont->m_byEffectCode != 3
          || cont->m_wEffectIndex != record->m_dwIndex)
      {
        const unsigned int elapsed = curTime - cont->m_dwStartSec;
        int leftSec = 0;
        if (fEffVal <= 0.0f)
        {
          leftSec = (static_cast<int>(cont->m_wDurSec) >> 1) - static_cast<int>(elapsed);
        }
        else
        {
          leftSec =
            static_cast<int>(static_cast<float>(cont->m_wDurSec) * (1.0f - fEffVal)) - static_cast<int>(elapsed);
        }

        if (leftSec <= 0)
        {
          RemoveSFContEffect(0, static_cast<unsigned __int16>(index), false, false);
        }
        else
        {
          AlterContDurSec(0, static_cast<unsigned __int16>(index), cont->m_dwStartSec, cont->m_wDurSec / 2);
          ++updatedCount;
        }
        ++hitCount;
      }
    }
  }

  if (updatedCount > 0)
  {
    SendMsg_AlterContEffectTime(0);
  }
  return hitCount > 0;
}

void CPlayer::SetMstPt(int nMstCode, float fVal, bool bAdd, unsigned int nWpType)
{
  float deltaValue = bAdd ? fVal : (fVal * -1.0f);
  int curMastery = m_pmMst.GetMasteryPerMast(static_cast<unsigned __int8>(nMstCode), static_cast<unsigned __int8>(nWpType));
  if (static_cast<float>(curMastery) + deltaValue > 99.0f)
  {
    deltaValue = static_cast<float>(99 - curMastery);
  }

  const float base1000 = std::pow(1000.0f, 2.0f);
  const float curPow = std::pow(static_cast<float>(curMastery), 2.0f);
  const float curRoot = std::sqrt(base1000 + (4.0f * curPow) * 1000.0f);
  unsigned int oldCum = static_cast<unsigned int>(std::pow((curRoot - 1000.0f) / 2.0f, 2.0f));

  const float newPow = std::pow(static_cast<float>(curMastery) + deltaValue, 2.0f);
  const float newRoot = std::sqrt(base1000 + (4.0f * newPow) * 1000.0f) - 1000.0f;
  unsigned int newCum = static_cast<unsigned int>(std::pow(newRoot / 2.0f, 2.0f));

  int updated = 0;
  if (nMstCode == 0)
  {
    updated = Emb_UpdateStat(nWpType, newCum, oldCum);
    m_pmMst.UpdateCumPerMast(0, static_cast<unsigned __int8>(nWpType), updated + newCum);
    SendMsg_StatInform(nWpType, updated + newCum, 0);
    s_MgrItemHistory.mastery_change_jade(
      0,
      oldCum,
      newCum,
      curMastery,
      deltaValue,
      m_szItemHistoryFileName,
      nWpType);
  }
  if (nMstCode == 1)
  {
    updated = Emb_UpdateStat(2u, newCum, oldCum);
    m_pmMst.UpdateCumPerMast(1u, 0, newCum);
    SendMsg_StatInform(2u, updated + newCum, 0);
  }
  if (nMstCode == 2)
  {
    const float base100 = std::pow(100.0f, 2.0f);
    const float oldPow = std::pow(static_cast<float>(curMastery), 2.0f);
    const float oldRoot = std::sqrt(base100 + (4.0f * oldPow) * 100.0f);
    oldCum = static_cast<unsigned int>(std::pow((oldRoot - 100.0f) / 2.0f, 2.0f));

    const float newPow2 = std::pow(static_cast<float>(curMastery) + deltaValue, 2.0f);
    const float newRoot2 = std::sqrt(base100 + (4.0f * newPow2) * 100.0f) - 100.0f;
    newCum = static_cast<unsigned int>(std::pow(newRoot2 / 2.0f, 2.0f));

    updated = Emb_UpdateStat(3u, newCum, oldCum);
    m_pmMst.UpdateCumPerMast(2u, 0, newCum);
    SendMsg_StatInform(3u, updated + newCum, 0);
  }
  if (nMstCode == 4)
  {
    for (int index = 0; index < 24; ++index)
    {
      curMastery = m_pmMst.GetMasteryPerMast(4u, static_cast<unsigned __int8>(index));
      const float oldPow = std::pow(static_cast<float>(curMastery), 2.0f);
      const float oldPow2 = std::pow(oldPow, 2.0f);
      oldCum = static_cast<unsigned int>((CalcRoundUp(oldPow2 / 14.0f) - 1) + 0.0099999998f);

      const float newPow3 = std::pow(static_cast<float>(curMastery) + deltaValue, 2.0f);
      const float newPow4 = std::pow(newPow3, 2.0f);
      newCum = static_cast<unsigned int>((CalcRoundUp(newPow4 / 14.0f) - 1) + 0.0099999998f);

      updated = Emb_UpdateStat(index + 52, newCum, oldCum);
      m_pmMst.UpdateCumPerMast(4u, static_cast<unsigned __int8>(index), newCum);
      SendMsg_StatInform(index + 52, updated + newCum, 0);
    }
  }
  if (nMstCode == 6 && static_cast<unsigned int>(m_Param.GetRaceCode()) != 2)
  {
    newCum = static_cast<unsigned int>(std::pow(static_cast<float>((curMastery - 1) + deltaValue), 2.0f) * 15000.0f);
    oldCum = static_cast<unsigned int>(std::pow(static_cast<float>(curMastery - 1), 2.0f) * 15000.0f);
    updated = Emb_UpdateStat(0x4Fu, newCum, oldCum);
    m_pmMst.UpdateCumPerMast(6u, 0, newCum);
    SendMsg_StatInform(0x4Fu, updated + newCum, 0);
  }
  ReCalcMaxHFSP(true, false);
}

void CPlayer::SendMsg_AlterContEffectTime(unsigned __int8 byContType)
{
  _alter_cont_effect_time_zocl msg;
  const unsigned int curTime = _sf_continous::GetSFContCurTime();
  int effectCount = 0;

  for (int index = 0; index < 8; ++index)
  {
    _sf_continous *cont = &m_SFCont[byContType][index];
    if (cont->m_bExist)
    {
      _base_fld *record = g_Main.m_tblEffectData[cont->m_byEffectCode].GetRecord(cont->m_wEffectIndex);
      (void)record;
      const unsigned __int16 effectCode = CalcEffectBit(cont->m_byEffectCode, cont->m_wEffectIndex);
      msg.List[effectCount].wEffectCode = effectCode;
      const unsigned int elapsed = curTime - cont->m_dwStartSec;
      msg.List[effectCount].zLeftSec = static_cast<__int16>(cont->m_wDurSec - elapsed);
      ++effectCount;
    }
  }

  msg.byEffectNum = static_cast<unsigned __int8>(effectCount);

  unsigned __int8 type[2]{};
  type[0] = 17;
  type[1] = 24;
  const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
}

int CPlayer::Emb_UpdateStat(unsigned int dwStatIndex, unsigned int dwNewData, unsigned int dwOldData)
{
  const int diff = static_cast<int>(dwNewData - m_pmMst.m_BaseCum.m_dwDamWpCnt[dwStatIndex]);
  const unsigned int prevDelta = m_pmMst.m_BaseCum.m_dwDamWpCnt[dwStatIndex] - dwOldData;
  const unsigned int newCum = prevDelta + dwNewData;

  m_pmMst.m_BaseCum.m_dwDamWpCnt[dwStatIndex] = newCum;
  if (m_pUserDB)
  {
    m_pUserDB->Update_Stat(dwStatIndex, newCum, 0);
  }
  m_Param.m_dwAlterMastery[dwStatIndex] += diff;
  return static_cast<int>(prevDelta);
}

void CPlayer::SendMsg_StatInform(unsigned __int8 byStatIndex, unsigned int dwNewStat, unsigned __int8 byReason)
{
#pragma pack(push, 1)
  struct StatMsg
  {
    unsigned __int8 byIndex;
    unsigned int dwValue;
    unsigned __int8 byReason;
  };
#pragma pack(pop)

  StatMsg msg{};
  msg.byIndex = byStatIndex;
  msg.dwValue = dwNewStat;
  msg.byReason = byReason;

  unsigned __int8 type[2] = {3, 43};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

CPlayer::__target *CPlayer::GetGroupTarget(unsigned __int8 byGroupType)
{
  return &m_GroupTargetObject[byGroupType];
}

void CPlayer::SendMsg_ReleaseGroupTargetObjectResult(unsigned __int8 byGroupType)
{
  unsigned __int8 type[2] = {13, 109};
  unsigned __int8 msg = byGroupType;
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SetMstHaveEffect(_ResourceItem_fld *pFld, _STORAGE_LIST::_db_con *pItem, int bAdd, int nAlter)
{
  float delta = 0.0f;
  float prevValue = 0.0f;

  for (int j = 0; j < pFld->m_nEffectDataNum; ++j)
  {
    if (pFld->m_EffectData[j].nEffCode > 58 && pFld->m_EffectData[j].nEffCode <= 65)
    {
      if (nAlter)
      {
        delta = pFld->m_EffectData[j].fEffUnit * static_cast<float>(nAlter);
      }
      else
      {
        const signed __int64 dur = static_cast<signed __int64>(pItem->m_dwDur);
        float durValue = static_cast<float>(static_cast<int>(dur));
        if (dur < 0)
        {
          durValue += 1.8446744e19f;
        }
        delta = pFld->m_EffectData[j].fEffUnit * durValue;
      }

      prevValue = m_EP.m_pDataParam->m_fEff_Have[pFld->m_EffectData[j].nEffCode];
      m_EP.m_pDataParam->m_fEff_Have[pFld->m_EffectData[j].nEffCode] += delta;
      if (m_EP.m_pDataParam->m_fEff_Have[pFld->m_EffectData[j].nEffCode] > pFld->m_EffectData[j].fEffUnitMax)
      {
        m_EP.m_pDataParam->m_fEff_Have[pFld->m_EffectData[j].nEffCode] = pFld->m_EffectData[j].fEffUnitMax;
        delta = pFld->m_EffectData[j].fEffUnitMax - prevValue;
      }

      apply_have_item_std_effect(pFld->m_EffectData[j].nEffCode, delta, bAdd, false);
    }
  }
}

int CPlayer::GetFP()
{
  return m_Param.GetFP();
}

int CPlayer::GetSP()
{
  return m_Param.GetSP();
}

int CPlayer::GetMaxFP()
{
  const float baseValue = static_cast<float>(m_nMaxPoint[1]);
  const float rate = m_EP.GetEff_Rate(10);
  const int maxFp = static_cast<int>(baseValue * rate);
  if (maxFp <= 0)
  {
    return 1;
  }
  return maxFp;
}

int CPlayer::GetMaxSP()
{
  const float baseValue = static_cast<float>(m_nMaxPoint[2]);
  const float rate = m_EP.GetEff_Rate(11);
  const int maxSp = static_cast<int>(baseValue * rate);
  if (maxSp <= 0)
  {
    return 1;
  }
  return maxSp;
}

__int64 CPlayer::CalcCurFPRate()
{
  const int curFpScaled = 10000 * GetFP();
  const int maxFp = GetMaxFP();
  return static_cast<unsigned int>(curFpScaled / maxFp);
}

__int64 CPlayer::CalcCurSPRate()
{
  const int curSpScaled = 10000 * GetSP();
  const int maxSp = GetMaxSP();
  return static_cast<unsigned int>(curSpScaled / maxSp);
}

bool CPlayer::SetFP(int nFP, bool bOver)
{
  if (m_bNeverDie)
  {
    return true;
  }

  const int curFp = GetFP();
  if (!bOver && nFP > curFp)
  {
    const int maxFp = GetMaxFP();
    if (curFp >= maxFp || nFP <= GetMaxFP())
    {
      if (curFp >= GetMaxFP() && nFP >= curFp)
      {
        return false;
      }
    }
    else
    {
      nFP = GetMaxFP();
    }
  }

  if (nFP < 0)
  {
    nFP = 0;
  }

  if (curFp == nFP)
  {
    return false;
  }

  m_Param.SetFP(nFP);
  return true;
}

bool CPlayer::SetSP(int nSP, bool bOver)
{
  if (m_bNeverDie)
  {
    return true;
  }

  const int curSp = m_Param.GetSP();
  if (!bOver && nSP > curSp)
  {
    const int maxSp = GetMaxSP();
    if (curSp >= maxSp || nSP <= GetMaxSP())
    {
      if (curSp >= GetMaxSP() && nSP >= curSp)
      {
        return false;
      }
    }
    else
    {
      nSP = GetMaxSP();
    }
  }

  if (nSP < 0)
  {
    nSP = 0;
  }

  if (curSp == nSP)
  {
    return false;
  }

  m_Param.SetSP(nSP);
  return true;
}

void CPlayer::ExtractStringToTime(unsigned int dwTemp, _SYSTEMTIME *tm)
{
  char buffer[44]{};
  char token[28]{};

  memset(buffer, 0, 10);
  sprintf(buffer, "%d", dwTemp);
  if (dwTemp && strlen_0(buffer) >= 9)
  {
    memset(token, 0, 4);
    memcpy_0(token, buffer, 4);
    tm->wYear = static_cast<unsigned __int16>(std::atoi(token));

    memset_0(token, 0, 4);
    memcpy_0(token, &buffer[4], 2);
    tm->wMonth = static_cast<unsigned __int16>(std::atoi(token));

    memset_0(token, 0, 4);
    memcpy_0(token, &buffer[6], 2);
    tm->wDay = static_cast<unsigned __int16>(std::atoi(token));

    memset_0(token, 0, 4);
    memcpy_0(token, &buffer[8], 1);
    tm->wHour = static_cast<unsigned __int16>(std::atoi(token));
    tm->wMinute = 0;
    tm->wSecond = 0;
  }
}

void CPlayer::AlterPvpPointLeak(long double dAlter)
{
  m_Param.m_dPvpPointLeak += dAlter;
  m_pUserDB->Update_PvpPointLeak(m_Param.m_dPvpPointLeak);
}

void CPlayer::SendMsg_ResurrectInform()
{
  unsigned __int8 type[2] = {3, 48};
  unsigned int msg = m_dwObjSerial;
  CircleReport(type, reinterpret_cast<char *>(&msg), sizeof(msg), false);
}

void CPlayer::SendMsg_Resurrect(char byRet, bool bQuickPotion)
{
#pragma pack(push, 1)
  struct ResurrectMsg
  {
    char byRet;
    char byLevel;
    unsigned __int16 wFP;
    unsigned __int16 wHP;
    unsigned __int16 wSP;
    bool bQuickPotion;
  };
#pragma pack(pop)

  ResurrectMsg msg{};
  msg.byRet = byRet;
  msg.byLevel = static_cast<char>(m_Param.GetLevel());
  msg.wFP = static_cast<unsigned __int16>(m_Param.GetFP());
  msg.wHP = static_cast<unsigned __int16>(m_Param.GetHP());
  msg.wSP = static_cast<unsigned __int16>(m_Param.GetSP());
  msg.bQuickPotion = bQuickPotion;

  unsigned __int8 type[2] = {3, 40};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::pc_NuclearAfterEffect()
{
  if (!m_pUserDB)
  {
    return;
  }

  _SFCONT_DB_BASE *dbSfcont = &m_pUserDB->m_AvatorData.dbSfcont;
  _base_fld *record = g_Main.m_tblEffectData[3].GetRecord("17");

  if (!dbSfcont)
  {
    return;
  }

  const unsigned int curTime = _sf_continous::GetSFContCurTime();
  for (int row = 0; row < 2; ++row)
  {
    for (int col = 0; col < 8; ++col)
    {
      _SFCONT_DB_BASE::_LIST *dbEntry = &dbSfcont->m_List[row][col];
      if (!dbEntry->IsFilled())
      {
        continue;
      }

      const unsigned __int8 effectCode = static_cast<unsigned __int8>(dbEntry->GetEffectCode());
      if (effectCode != 3 && effectCode != record->m_dwIndex)
      {
        continue;
      }

      _sf_continous *cont = &m_SFCont[row][col];
      cont->m_byEffectCode = effectCode;
      cont->m_wEffectIndex = static_cast<unsigned __int16>(dbEntry->GetEffectIndex());
      cont->m_byLv = static_cast<unsigned __int8>(dbEntry->GetLv() + 1);
      cont->m_dwStartSec = curTime;
      cont->m_wDurSec = static_cast<unsigned __int16>(dbEntry->GetLeftTime());

      const unsigned __int8 order = static_cast<unsigned __int8>(dbEntry->GetOrder());
      if (curTime > order)
      {
        cont->m_dwStartSec = curTime - order;
        cont->m_wDurSec += order;
      }

      bool invalid = false;
      if (cont->m_byEffectCode < 4u)
      {
        if (g_Main.m_tblEffectData[cont->m_byEffectCode].GetRecord(cont->m_wEffectIndex))
        {
          if (cont->m_byLv > 7u)
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
        m_pUserDB->Update_SFContDelete(static_cast<unsigned __int8>(row), static_cast<unsigned __int8>(col));
        const char *charName = m_Param.GetCharNameW();
        g_Main.m_logSystemError.Write(
          "%s: error stored effect, code: %d, idx: %d: lv: %d",
          charName,
          cont->m_byEffectCode,
          cont->m_wEffectIndex,
          cont->m_byLv);
      }
      else
      {
        SendMsg_StartContSF(cont);
        m_bLastContEffectUpdate = 1;
      }
    }
  }
}

void CPlayer::SendMsg_StartContSF(_sf_continous *pCont)
{
#pragma pack(push, 1)
  struct StartContMsg
  {
    unsigned __int8 lv;
    unsigned __int16 effectBit;
    unsigned __int16 durSec;
  };
#pragma pack(pop)

  StartContMsg msg{};
  msg.lv = pCont->m_byLv;
  msg.effectBit = CalcEffectBit(pCont->m_byEffectCode, pCont->m_wEffectIndex);
  msg.durSec = pCont->m_wDurSec;

  unsigned __int8 type[2] = {17, 25};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_UnitAlterFeeInform(char bySlotIndex, unsigned int dwPullingFee)
{
#pragma pack(push, 1)
  struct UnitAlterFeeMsg
  {
    char bySlotIndex;
    unsigned int dwPullingFee;
  };
#pragma pack(pop)

  UnitAlterFeeMsg msg{};
  msg.bySlotIndex = bySlotIndex;
  msg.dwPullingFee = dwPullingFee;

  unsigned __int8 type[2] = {23, 21};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::_UpdateUnitDebt(unsigned __int8 bySlotIndex, unsigned int dwPull)
{
  if (!m_pUserDB)
  {
    return;
  }

  _UNIT_DB_BASE::_LIST *data = &m_Param.m_UnitDB.m_List[bySlotIndex];
  data->nPullingFee = dwPull;
  SendMsg_UnitAlterFeeInform(bySlotIndex, dwPull);
  m_pUserDB->Update_UnitData(bySlotIndex, data);
}

bool CPlayer::_LockUnitKey(unsigned __int8 bySlotIndex, bool bLock)
{
  for (int index = 0; index < m_Param.m_dbInven.m_nUsedNum; ++index)
  {
    _STORAGE_LIST::_db_con *item = &m_Param.m_dbInven.m_pStorageList[index];
    if (item->m_bLoad && item->m_dwLv == bySlotIndex)
    {
      m_Param.m_dbInven.SetLock(index, bLock);
      return true;
    }
  }
  return false;
}

void CPlayer::SendMsg_UnitForceReturnInform(char bySlotIndex, unsigned int dwDebt)
{
#pragma pack(push, 1)
  struct UnitForceReturnMsg
  {
    char bySlotIndex;
    unsigned int dwDebt;
  };
#pragma pack(pop)

  UnitForceReturnMsg msg{};
  msg.bySlotIndex = bySlotIndex;
  msg.dwDebt = dwDebt;

  unsigned __int8 type[2] = {23, 27};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::AddDalant(int dwPush, bool bApply)
{
  if (bApply)
  {
    const double penalty = g_Main.m_pTimeLimitMgr->GetPlayerPenalty(m_id.wIndex);
    dwPush = static_cast<int>(static_cast<double>(dwPush) * penalty);
  }

  unsigned int newDalant = static_cast<unsigned int>(dwPush) + m_Param.GetDalant();
  const unsigned int currentDalant = m_Param.GetDalant();
  if (!CanAddMoneyForMaxLimMoney(static_cast<unsigned int>(dwPush), currentDalant))
  {
    newDalant = 2000000000;
  }

  if (newDalant != m_Param.GetDalant())
  {
    m_Param.SetDalant(newDalant);
    const unsigned int gold = m_Param.GetGold();
    m_pUserDB->Update_Money(m_Param.GetDalant(), gold);
  }
}

void CPlayer::AlterDalant(double dDalant)
{
  if (dDalant <= 0.0)
  {
    if (dDalant < 0.0)
    {
      SubDalant(static_cast<int>(-dDalant));
    }
  }
  else
  {
    AddDalant(static_cast<int>(dDalant), true);
  }
}

void CPlayer::SubDalant(unsigned int dwSub)
{
  unsigned int nextDalant = m_Param.GetDalant() - dwSub;
  if (dwSub > m_Param.GetDalant())
  {
    nextDalant = 0;
  }

  if (nextDalant != m_Param.GetDalant())
  {
    m_Param.SetDalant(nextDalant);
    const unsigned int gold = m_Param.GetGold();
    m_pUserDB->Update_Money(m_Param.GetDalant(), gold);
  }
}

bool CPlayer::IsUsableAccountType(int nCashType)
{
  if (!nCashType)
  {
    return true;
  }
  if (nCashType == 1)
  {
    return IsApplyPcbangPrimium();
  }
  return false;
}

bool CPlayer::IsApplyPcbangPrimium()
{
  auto *nationSetting = CTSingleton<CNationSettingManager>::Instance();
  return nationSetting->IsApplyPcbangPrimium(this);
}

void CPlayer::SendMsg_PremiumCashItemUse(unsigned __int16 wSerial)
{
  _notify_not_use_premium_cashitem_zocl msg{};
  msg.wItemSerial = wSerial;

  unsigned __int8 type[2] = {59, 16};
  const unsigned __int16 len = msg.size();
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
}

int CPlayer::GetCashAmount()
{
  return m_nCashAmount;
}

void CPlayer::SetCashAmount(int nAmount)
{
  m_nCashAmount = nAmount;
}

void CPlayer::DeleteCouponItem(_STORAGE_POS_INDIV *CouponItem, int n)
{
  for (int j = 0; j < n; ++j)
  {
    _STORAGE_LIST *storage = m_Param.m_pStoragePtr[CouponItem[j].byStorageCode];
    _STORAGE_LIST::_db_con *pItem = storage->GetPtrFromSerial(CouponItem[j].wItemSerial);
    if (pItem)
    {
      _base_fld *record = g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
      if (record)
      {
        if (!*reinterpret_cast<unsigned int *>(&record[4].m_strCode[48]))
        {
          Emb_DelStorage(0, pItem->m_byStorageIndex, 0, 1, "CPlayer::DeleteCouponItem");
          s_MgrItemHistory.consume_del_item(
            m_ObjID.m_wIndex,
            pItem,
            m_szItemHistoryFileName);
        }
      }
    }
  }
}

int CPlayer::_CalcMaxHP()
{
  float masteryPerMast = static_cast<float>(m_pmMst.GetMasteryPerMast(1u, 0));
  masteryPerMast += static_cast<float>(m_nAddPointByClass[0]);

  const int level = m_Param.GetLevel();
  const float levelPow = std::pow(static_cast<float>(level), 2.0f);
  const float value = std::sqrt(levelPow * masteryPerMast) * 20.0f + 80.0f;
  return static_cast<int>(value);
}

int CPlayer::_CalcMaxFP()
{
  float value = 0.0f;
  const int raceCode = m_Param.GetRaceCode();

  if (raceCode == 0)
  {
    float sum = m_pmMst.GetAveForceMasteryPerClass(1u) * 0.40000001f;
    sum += m_pmMst.GetAveForceMasteryPerClass(2u) * 0.075000003f;
    sum += m_pmMst.GetAveForceMasteryPerClass(3u) * 0.1125f;
    sum += m_pmMst.GetAveForceMasteryPerClass(4u) * 0.1125f;
    sum += m_pmMst.GetAveForceMasteryPerClass(5u) * 0.075000003f;
    sum += m_pmMst.GetAveSkillMasteryPerClass(0) * 0.1125f;
    sum += m_pmMst.GetAveSkillMasteryPerClass(1u) * 0.1125f;
    value = sum;
  }
  else if (raceCode == 1)
  {
    float sum = m_pmMst.GetAveForceMasteryPerClass(0) * 0.40000001f;
    sum += m_pmMst.GetAveForceMasteryPerClass(2u) * 0.075000003f;
    sum += m_pmMst.GetAveForceMasteryPerClass(3u) * 0.1125f;
    sum += m_pmMst.GetAveForceMasteryPerClass(4u) * 0.1125f;
    sum += m_pmMst.GetAveForceMasteryPerClass(5u) * 0.075000003f;
    sum += m_pmMst.GetAveSkillMasteryPerClass(0) * 0.1125f;
    sum += m_pmMst.GetAveSkillMasteryPerClass(1u) * 0.1125f;
    value = sum;
  }
  else if (raceCode == 2)
  {
    float sum = m_pmMst.GetAveSkillMasteryPerClass(0) * 0.5f;
    sum += m_pmMst.GetAveSkillMasteryPerClass(1u) * 0.5f;
    value = sum;
  }

  value += static_cast<float>(m_nAddPointByClass[1]);
  const float level = static_cast<float>(m_Param.GetLevel());
  const float powValue = std::pow(value, 2.0f);
  const float result = std::sqrt(level * powValue) * 5.0f + 40.0f;
  return static_cast<int>(result);
}

int CPlayer::_CalcMaxSP()
{
  float value = static_cast<float>(m_pmMst.GetMasteryPerMast(0, 0)) * 0.2f;
  value += static_cast<float>(m_pmMst.GetMasteryPerMast(0, 1u)) * 0.80000001f;
  value += static_cast<float>(m_nAddPointByClass[2]);

  const float level = static_cast<float>(m_Param.GetLevel());
  const float powValue = std::pow(value, 2.0f);
  const float result = std::sqrt(level * powValue) * 2.5f + 160.0f;
  return static_cast<int>(result);
}

void CPlayer::ReCalcMaxHFSP(bool bSend, bool bRatio)
{
  (void)bSend;
  (void)bRatio;

  m_nMaxPoint[0] = _CalcMaxHP();
  m_nMaxPoint[1] = _CalcMaxFP();
  m_nMaxPoint[2] = _CalcMaxSP();
}

void CPlayer::SendMsg_BuddyLogoffInform(unsigned int dwObjSerial)
{
#pragma pack(push, 1)
  struct BuddyLogoffInformMsg
  {
    unsigned int dwSerial;
  };
#pragma pack(pop)

  BuddyLogoffInformMsg msg{};
  msg.dwSerial = dwObjSerial;

  unsigned __int8 type[2] = {31, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 4u);
}

void CPlayer::SendMsg_BuddyLoginInform(unsigned int dwObjSerial, unsigned __int8 byMapIndex, unsigned __int8 byPosCode)
{
#pragma pack(push, 1)
  struct BuddyLoginInformMsg
  {
    unsigned int dwSerial;
    unsigned __int8 byMapIndex;
    unsigned __int8 byPosCode;
  };
#pragma pack(pop)

  BuddyLoginInformMsg msg{};
  msg.dwSerial = dwObjSerial;
  msg.byMapIndex = byMapIndex;
  msg.byPosCode = byPosCode;

  unsigned __int8 type[2] = {31, 1};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 6u);
}

void CPlayer::SendMsg_JadeEffectErr(char byErrorCode)
{
  unsigned __int8 type[2] = {59, 4};
  char msg[1]{};
  msg[0] = byErrorCode;
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::pc_DTradeAskRequest(unsigned __int16 wDstIndex)
{
  unsigned __int8 errCode = 0;
  CPlayer *dst = &g_Player[wDstIndex];
  if (TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, m_id.wIndex) == 99)
  {
    m_pmTrd.bDTradeMode = false;
    dst->m_pmTrd.bDTradeMode = false;
    errCode = 23;
  }
  else if (TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, dst->m_id.wIndex) == 99)
  {
    m_pmTrd.bDTradeMode = false;
    dst->m_pmTrd.bDTradeMode = false;
    errCode = 24;
  }
  else if (dst->m_bLive
           && dst->m_bOper
           && CPlayerDB::GetRaceCode(&dst->m_Param) == CPlayerDB::GetRaceCode(&m_Param)
           && !dst->m_bCorpse
           && dst->m_pCurMap == m_pCurMap
           && dst->GetCurSecNum() != static_cast<unsigned int>(-1)
           && _effect_parameter::GetEff_Have(&dst->m_EP, 50) <= 0.0)
  {
    if (m_pmTrd.bDTradeMode || dst->m_pmTrd.bDTradeMode)
    {
      errCode = 1;
    }
    else if (m_pCurMap->m_pMapSet->m_nMapType == 1)
    {
      errCode = 3;
    }
    else if (m_byUserDgr == dst->m_byUserDgr)
    {
      if (GetSqrt(dst->m_fCurPos, m_fCurPos) > 100.0)
      {
        errCode = 2;
      }
    }
    else
    {
      errCode = 3;
    }
  }
  else
  {
    errCode = 3;
  }

  if (!errCode)
  {
    m_pmTrd.wDTradeDstIndex = dst->m_ObjID.m_wIndex;
    m_pmTrd.dwDTradeDstSerial = dst->m_dwObjSerial;
    dst->SendMsg_DTradeAskInform(this);
  }
  SendMsg_DTradeAskResult(errCode);
}

void CPlayer::pc_DTradeAnswerRequest(_CLID *pidAsker)
{
  unsigned __int8 errCode = 0;
  CPlayer *asker = &g_Player[pidAsker->wIndex];
  if (TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, m_id.wIndex) == 99)
  {
    m_pmTrd.bDTradeMode = false;
    asker->m_pmTrd.bDTradeMode = false;
    errCode = 23;
  }
  else if (TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, asker->m_id.wIndex) == 99)
  {
    m_pmTrd.bDTradeMode = false;
    asker->m_pmTrd.bDTradeMode = false;
    errCode = 24;
  }
  else if (asker->m_dwObjSerial == pidAsker->dwSerial)
  {
    if (asker->m_pmTrd.wDTradeDstIndex == m_ObjID.m_wIndex
        && asker->m_pmTrd.dwDTradeDstSerial == m_dwObjSerial)
    {
      if (m_pmTrd.bDTradeMode || asker->m_pmTrd.bDTradeMode)
      {
        errCode = 2;
      }
      else if (asker->m_bLive
               && CPlayerDB::GetRaceCode(&asker->m_Param) == CPlayerDB::GetRaceCode(&m_Param)
               && !asker->m_bCorpse
               && asker->m_pCurMap == m_pCurMap
               && asker->GetCurSecNum() != static_cast<unsigned int>(-1))
      {
        if (m_pCurMap->m_pMapSet->m_nMapType == 1)
        {
          errCode = 6;
        }
        else if (m_byUserDgr == asker->m_byUserDgr)
        {
          if (GetSqrt(asker->m_fCurPos, m_fCurPos) > 100.0)
          {
            errCode = 5;
          }
        }
        else
        {
          errCode = 6;
        }
      }
      else
      {
        errCode = 3;
      }
    }
    else
    {
      errCode = 4;
    }
  }
  else
  {
    errCode = 1;
  }

  if (errCode)
  {
    SendMsg_DTradeAnswerResult(errCode);
    return;
  }

  unsigned int pdwCode[4]{};
  unsigned int pdwKey[4]{};
  for (int j = 0; j < 4; ++j)
  {
    const int codeHigh = rand() << 16;
    pdwCode[j] = rand() + codeHigh;
    const int keyHigh = rand() << 16;
    pdwKey[j] = rand() + keyHigh;
  }

  unsigned int *localKey = CalcCodeKey(pdwCode);
  const int emptyNum = m_Param.m_dbInven.GetNumEmptyCon();
  m_pmTrd.SetDTradeStart(asker->m_ObjID.m_wIndex, asker->m_dwObjSerial, emptyNum, localKey);

  unsigned int *remoteKey = CalcCodeKey(pdwKey);
  const int otherEmptyNum = asker->m_Param.m_dbInven.GetNumEmptyCon();
  asker->m_pmTrd.SetDTradeStart(m_ObjID.m_wIndex, m_dwObjSerial, otherEmptyNum, remoteKey);

  SendMsg_DTradeStartInform(asker, this, pdwCode);
  asker->SendMsg_DTradeStartInform(asker, this, pdwKey);
}

void CPlayer::pc_DTradeCancleRequest()
{
  CPlayer *dst = nullptr;
  const unsigned __int8 errCode = static_cast<unsigned __int8>(!DTradeEqualPerson(this, &dst));
  if (errCode)
  {
    m_pmTrd.Init();
    SendMsg_DTradeCloseInform(0);
    if (dst)
    {
      dst->m_pmTrd.Init();
      dst->SendMsg_DTradeCloseInform(0);
    }
    return;
  }

  m_pmTrd.Init();
  dst->m_pmTrd.Init();
  SendMsg_DTradeCancleResult(errCode);
  dst->SendMsg_DTradeCancleInform();
}

void CPlayer::pc_DTradeLockRequest()
{
  unsigned __int8 errCode = 0;
  CPlayer *dst = nullptr;
  if (DTradeEqualPerson(this, &dst))
  {
    if (m_pmTrd.bDTradeLock)
    {
      errCode = 2;
    }
    else if (m_pCurMap->m_pMapSet->m_nMapType == 1)
    {
      errCode = 6;
    }
  }
  else
  {
    errCode = 1;
  }

  if (errCode)
  {
    m_pmTrd.Init();
    SendMsg_DTradeCloseInform(0);
    if (dst)
    {
      dst->m_pmTrd.Init();
      dst->SendMsg_DTradeCloseInform(0);
    }
    return;
  }

  m_pmTrd.bDTradeLock = true;
  dst->SendMsg_DTradeLockInform();
  SendMsg_DTradeLockResult(errCode);
}

void CPlayer::pc_DTradeAddRequest(
  unsigned __int8 bySlotIndex,
  unsigned __int8 byStorageCode,
  unsigned int dwSerial,
  unsigned __int8 byAmount)
{
  unsigned __int8 errCode = 0;
  _STORAGE_LIST::_db_con *item = nullptr;
  CPlayer *dst = nullptr;
  if (DTradeEqualPerson(this, &dst))
  {
    if (m_pCurMap->m_pMapSet->m_nMapType == 1)
    {
      errCode = 6;
    }
    else if (m_pmTrd.bDTradeLock)
    {
      errCode = 2;
    }
    else if (bySlotIndex < 0xFu)
    {
      if (m_pmTrd.DItemNode[bySlotIndex].bLoad)
      {
        errCode = 4;
      }
      else
      {
        item = _STORAGE_LIST::GetPtrFromSerial(m_Param.m_pStoragePtr[byStorageCode], dwSerial);
        if (item)
        {
          if (item->m_bLock)
          {
            errCode = 9;
          }
          else if (item->m_byTableCode == 19)
          {
            errCode = 7;
          }
          else if (IsExchangeItem(item->m_byTableCode, item->m_wItemIndex))
          {
            if (!IsOverLapItem(item->m_byTableCode) || (byAmount <= item->m_dwDur && byAmount))
            {
              for (int j = 0; j < 15; ++j)
              {
                _DTRADE_ITEM *node = &m_pmTrd.DItemNode[j];
                if (node->bLoad && node->dwSerial == dwSerial)
                {
                  errCode = 6;
                  break;
                }
              }
            }
            else
            {
              errCode = 6;
            }
          }
          else
          {
            errCode = 10;
          }
        }
        else
        {
          errCode = 5;
        }
      }
    }
    else
    {
      errCode = 3;
    }
  }
  else
  {
    errCode = 1;
  }

  if (errCode)
  {
    m_pmTrd.Init();
    SendMsg_DTradeCloseInform(0);
    if (dst)
    {
      dst->m_pmTrd.Init();
      dst->SendMsg_DTradeCloseInform(0);
    }
    return;
  }

  if (!byStorageCode)
  {
    if (IsOverLapItem(item->m_byTableCode))
    {
      if (item->m_dwDur == byAmount)
      {
        ++m_pmTrd.byEmptyInvenNum;
      }
    }
    else
    {
      ++m_pmTrd.byEmptyInvenNum;
    }
  }

  ++m_pmTrd.bySellItemNum;
  m_pmTrd.DItemNode[bySlotIndex].SetData(byStorageCode, dwSerial, byAmount);
  dst->SendMsg_DTradeAddInform(bySlotIndex, item, byAmount);
  SendMsg_DTradeAddResult(errCode);
}

void CPlayer::pc_DTradeDelRequest(unsigned __int8 bySlotIndex)
{
  unsigned __int8 errCode = 0;
  CPlayer *dst = nullptr;
  if (DTradeEqualPerson(this, &dst))
  {
    if (m_pCurMap->m_pMapSet->m_nMapType == 1)
    {
      errCode = 6;
    }
    else if (m_pmTrd.bDTradeLock)
    {
      errCode = 2;
    }
    else if (bySlotIndex < 0xFu)
    {
      if (!m_pmTrd.DItemNode[bySlotIndex].bLoad)
      {
        errCode = 4;
      }
    }
    else
    {
      errCode = 3;
    }
  }
  else
  {
    errCode = 1;
  }

  if (errCode)
  {
    m_pmTrd.Init();
    SendMsg_DTradeCloseInform(0);
    if (dst)
    {
      dst->m_pmTrd.Init();
      dst->SendMsg_DTradeCloseInform(0);
    }
    return;
  }

  _DTRADE_ITEM *node = &m_pmTrd.DItemNode[bySlotIndex];
  if (!m_pmTrd.DItemNode[bySlotIndex].byStorageCode)
  {
    _STORAGE_LIST::_db_con *item =
      _STORAGE_LIST::GetPtrFromSerial(m_Param.m_pStoragePtr[node->byStorageCode], node->dwSerial);
    if (!item)
    {
      return;
    }
    if (IsOverLapItem(item->m_byTableCode))
    {
      if (item->m_dwDur == node->byAmount)
      {
        --m_pmTrd.byEmptyInvenNum;
      }
    }
    else
    {
      --m_pmTrd.byEmptyInvenNum;
    }
  }

  --m_pmTrd.bySellItemNum;
  m_pmTrd.DItemNode[bySlotIndex].ReleaseData();
  dst->SendMsg_DTradeDelInform(bySlotIndex);
  SendMsg_DTradeDelResult(errCode);
}

void CPlayer::pc_DTradeBetRequest(unsigned __int8 byMoneyUnit, unsigned int dwBetAmount)
{
  unsigned __int8 errCode = 0;
  CPlayer *dst = nullptr;
  if (DTradeEqualPerson(this, &dst))
  {
    if (m_pmTrd.bDTradeLock)
    {
      errCode = 4;
    }
    else if (m_pCurMap->m_pMapSet->m_nMapType == 1)
    {
      errCode = 6;
    }
    else if (byMoneyUnit)
    {
      if (dwBetAmount > CPlayerDB::GetGold(&m_Param))
      {
        errCode = 2;
      }
    }
    else if (dwBetAmount > CPlayerDB::GetDalant(&m_Param))
    {
      errCode = 1;
    }
  }
  else
  {
    errCode = 1;
  }

  if (errCode)
  {
    m_pmTrd.Init();
    SendMsg_DTradeCloseInform(0);
    if (dst)
    {
      dst->m_pmTrd.Init();
      dst->SendMsg_DTradeCloseInform(0);
    }
    return;
  }

  if (byMoneyUnit)
  {
    m_pmTrd.dwDTrade_Gold = dwBetAmount;
  }
  else
  {
    m_pmTrd.dwDTrade_Dalant = dwBetAmount;
  }

  dst->SendMsg_DTradeBetInform(byMoneyUnit, dwBetAmount);
  SendMsg_DTradeBetResult(errCode);
}

void CPlayer::pc_DTradeOKRequest(unsigned int *pdwKey)
{
  unsigned __int8 errCode = 0;
  CPlayer *dst = nullptr;
  std::array<_STORAGE_LIST::_db_con, 30> tradeItems{};
  int outItemNum[2]{};
  if (!m_pUserDB)
  {
    return;
  }

  if (TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, m_id.wIndex) == 99)
  {
    errCode = 23;
  }
  else if (DTradeEqualPerson(this, &dst))
  {
    if (m_pCurMap->m_pMapSet->m_nMapType == 1)
    {
      errCode = 6;
    }
    else if (m_pmTrd.bDTradeLock && dst->m_pmTrd.bDTradeLock)
    {
      if (m_pmTrd.bDTradeOK)
      {
        errCode = 3;
      }
      else if (!memcmp_0(m_pmTrd.dwKey, pdwKey, 0x10u))
      {
        if (m_pmTrd.bySellItemNum <= static_cast<int>(dst->m_pmTrd.byEmptyInvenNum))
        {
          CPlayer *players[2] = {this, dst};
          for (int j = 0; j < 2; ++j)
          {
            CPlayer *player = players[j];
            const unsigned int dalant = CPlayerDB::GetDalant(&player->m_Param);
            if (player->m_pmTrd.dwDTrade_Dalant > dalant)
            {
              errCode = 5;
              goto RESULT;
            }
            const unsigned int gold = CPlayerDB::GetGold(&player->m_Param);
            if (player->m_pmTrd.dwDTrade_Gold > gold)
            {
              errCode = 5;
              goto RESULT;
            }
            if (player->m_pmTrd.dwDTrade_Dalant > 0x77359400 || player->m_pmTrd.dwDTrade_Gold > 0x7A120)
            {
              errCode = 5;
              goto RESULT;
            }
          }

          if (dst->m_pmTrd.bDTradeOK)
          {
            for (int j = 0; j < 2; ++j)
            {
              const int otherIndex = (j + 1) & 1;
              const double dDalant = static_cast<double>(players[otherIndex]->m_pmTrd.dwDTrade_Dalant)
                - static_cast<double>(players[j]->m_pmTrd.dwDTrade_Dalant);
              const double dGold = static_cast<double>(players[otherIndex]->m_pmTrd.dwDTrade_Gold)
                - static_cast<double>(players[j]->m_pmTrd.dwDTrade_Gold);
              if (dDalant > 0.0)
              {
                const unsigned int curDalant = CPlayerDB::GetDalant(&players[j]->m_Param);
                double dalantValue = dDalant;
                unsigned __int64 dalantHigh = 0;
                if (dalantValue > 9.223372036854776e18)
                {
                  dalantValue -= 9.223372036854776e18;
                  dalantHigh = 0x7FFFFFFFFFFFFFFFULL;
                  if (dalantValue > 9.223372036854776e18)
                  {
                    dalantValue -= 9.223372036854776e18;
                    dalantHigh = static_cast<unsigned __int64>(-2);
                  }
                }
                const unsigned __int64 addDalant =
                  dalantHigh + static_cast<unsigned int>(static_cast<int>(dalantValue));
                if (!CanAddMoneyForMaxLimMoney(addDalant, curDalant))
                {
                  errCode = 6;
                  break;
                }
              }
              if (dGold > 0.0)
              {
                const unsigned int curGold = CPlayerDB::GetGold(&players[j]->m_Param);
                double goldValue = dGold;
                unsigned __int64 goldHigh = 0;
                if (goldValue > 9.223372036854776e18)
                {
                  goldValue -= 9.223372036854776e18;
                  goldHigh = 0x7FFFFFFFFFFFFFFFULL;
                  if (goldValue > 9.223372036854776e18)
                  {
                    goldValue -= 9.223372036854776e18;
                    goldHigh = static_cast<unsigned __int64>(-2);
                  }
                }
                const unsigned __int64 addGold =
                  goldHigh + static_cast<unsigned int>(static_cast<int>(goldValue));
                if (!CanAddMoneyForMaxLimGold(addGold, curGold))
                {
                  errCode = 6;
                  break;
                }
              }

              int outCount = 0;
              for (int k = 0; k < 15; ++k)
              {
                _DTRADE_ITEM *node = &players[j]->m_pmTrd.DItemNode[k];
                if (node->bLoad)
                {
                  _STORAGE_LIST *storage = players[j]->m_Param.m_pStoragePtr[node->byStorageCode];
                  _STORAGE_LIST::_db_con *srcItem = _STORAGE_LIST::GetPtrFromSerial(storage, node->dwSerial);
                  if (!srcItem)
                  {
                    errCode = 4;
                    goto RESULT;
                  }
                  if (srcItem->m_bLock)
                  {
                    errCode = 9;
                    goto RESULT;
                  }
                  if (srcItem->m_byTableCode == 19)
                  {
                    errCode = 9;
                    goto RESULT;
                  }
                  _STORAGE_LIST::_db_con *outItem = &tradeItems[j * 15 + outCount];
                  memcpy_0(outItem, srcItem, 0x32u);
                  if (IsOverLapItem(srcItem->m_byTableCode) && srcItem->m_dwDur > node->byAmount)
                  {
                    outItem->m_dwDur = node->byAmount;
                  }
                  ++outCount;
                }
              }
              outItemNum[j] = outCount;
            }
          }
        }
        else
        {
          errCode = 100;
        }
      }
      else
      {
        errCode = 13;
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

RESULT:
  if (errCode)
  {
    m_pmTrd.Init();
    SendMsg_DTradeCloseInform(0);
    if (dst)
    {
      dst->m_pmTrd.Init();
      dst->SendMsg_DTradeCloseInform(0);
    }
    return;
  }

  SendMsg_DTradeOKResult(0);
  dst->SendMsg_DTradeOKInform();
  m_pmTrd.bDTradeOK = true;
  if (!dst->m_pmTrd.bDTradeOK || !m_pmTrd.bDTradeOK)
  {
    return;
  }

  CPlayer *players[2] = {this, dst};
  for (int playerIndex = 0; playerIndex < 2; ++playerIndex)
  {
    const int otherIndex = (playerIndex + 1) & 1;
    CPlayer *player = players[playerIndex];
    CPlayer *other = players[otherIndex];

    for (int m = 0; m < outItemNum[playerIndex]; ++m)
    {
      _STORAGE_LIST::_db_con *outItem = &tradeItems[playerIndex * 15 + m];
      if (IsOverLapItem(outItem->m_byTableCode))
      {
        const int alter = -static_cast<int>(outItem->m_dwDur);
        player->Emb_AlterDurPoint(
          outItem->m_pInList->m_nListCode,
          outItem->m_byStorageIndex,
          alter,
          true,
          false);
      }
      else
      {
        if (!player->Emb_DelStorage(
              outItem->m_pInList->m_nListCode,
              outItem->m_byStorageIndex,
              false,
              true,
              "CPlayer::pc_DTradeOKRequest()")
            && playerIndex == 0
            && outItemNum[0] > 0)
        {
          for (int n = 0; n < m; ++n)
          {
            _STORAGE_LIST::_db_con *rollbackItem = &tradeItems[playerIndex * 15 + n];
            player->Emb_AddStorage(0, rollbackItem, false, true);
            player->SendMsg_RewardAddItem(rollbackItem, 0);
          }
          m_pmTrd.Init();
          SendMsg_DTradeCloseInform(0);
          if (dst)
          {
            dst->m_pmTrd.Init();
            dst->SendMsg_DTradeCloseInform(0);
          }
          return;
        }
      }
    }

    unsigned __int16 startSerial = static_cast<unsigned __int16>(-1);
    int m = 0;
    for (; m < outItemNum[otherIndex]; ++m)
    {
      _STORAGE_LIST::_db_con *inItem = &tradeItems[otherIndex * 15 + m];
      const unsigned __int16 newSerial = CPlayerDB::GetNewItemSerial(&player->m_Param);
      inItem->m_wSerial = newSerial;
      if (startSerial == 0xFFFF)
      {
        startSerial = newSerial;
      }
      if (!player->Emb_AddStorage(0, inItem, false, true) && playerIndex == 0 && outItemNum[otherIndex] > 0)
      {
        break;
      }
    }

    if (m < outItemNum[otherIndex] && playerIndex == 0 && outItemNum[otherIndex] > 0)
    {
      for (int ii = 0; ii < m; ++ii)
      {
        _STORAGE_LIST::_db_con *addedItem = &tradeItems[otherIndex * 15 + ii];
        player->Emb_DelStorage(0, addedItem->m_byStorageIndex, false, true, "CPlayer::pc_DTradeOKRequest() - Emb_Add Item Fail");
      }
      for (int jj = 0; jj < outItemNum[playerIndex]; ++jj)
      {
        _STORAGE_LIST::_db_con *rollbackItem = &tradeItems[playerIndex * 15 + jj];
        player->Emb_AddStorage(0, rollbackItem, false, true);
        player->SendMsg_RewardAddItem(rollbackItem, 0);
      }
      m_pmTrd.Init();
      SendMsg_DTradeCloseInform(0);
      if (dst)
      {
        dst->m_pmTrd.Init();
        dst->SendMsg_DTradeCloseInform(0);
      }
      return;
    }

    double dDalant = static_cast<double>(other->m_pmTrd.dwDTrade_Dalant)
      - static_cast<double>(player->m_pmTrd.dwDTrade_Dalant);
    double dGold = static_cast<double>(other->m_pmTrd.dwDTrade_Gold)
      - static_cast<double>(player->m_pmTrd.dwDTrade_Gold);
    const double penalty = TimeLimitMgr::GetPlayerPenalty(g_Main.m_pTimeLimitMgr, m_id.wIndex);
    dDalant *= penalty;
    dGold *= penalty;
    player->AlterDalant(dDalant);
    player->AlterGold(dGold);
    player->SendMsg_DTradeAccomplishInform(true, startSerial);

    const unsigned int sumGold = CPlayerDB::GetGold(&player->m_Param);
    const unsigned int sumDalant = CPlayerDB::GetDalant(&player->m_Param);
    const char *dstId = other->m_pUserDB->m_szAccountID;
    const unsigned int dstSerial = CPlayerDB::GetCharSerial(&other->m_Param);
    char *dstName = CPlayerDB::GetCharNameA(&other->m_Param);
    CMgrAvatorItemHistory::trade(
      &CPlayer::s_MgrItemHistory,
      player->m_ObjID.m_wIndex,
      &tradeItems[playerIndex * 15],
      outItemNum[playerIndex],
      player->m_pmTrd.dwDTrade_Dalant,
      player->m_pmTrd.dwDTrade_Gold,
      &tradeItems[otherIndex * 15],
      outItemNum[otherIndex],
      other->m_pmTrd.dwDTrade_Dalant,
      other->m_pmTrd.dwDTrade_Gold,
      dstName,
      dstSerial,
      const_cast<char *>(dstId),
      sumDalant,
      sumGold,
      player->m_pCurMap->m_pMapSet->m_strCode,
      player->m_fCurPos,
      player->m_szItemHistoryFileName);
  }

  pc_UpdateDataForTrade(this, dst);
  const char *dstName = CPlayerDB::GetCharNameA(&dst->m_Param);
  const int dstLevel = dst->GetLevel();
  const char *srcName = CPlayerDB::GetCharNameA(&m_Param);
  const int srcLevel = GetLevel();
  CLogFile::Write(
    &g_Main.m_logDTrade,
    "[%d]%s(%d): D(%d) G(%d) # [%d]%s(%d): D(%d) G(%d)",
    srcLevel,
    srcName,
    m_dwObjSerial,
    m_pmTrd.dwDTrade_Dalant,
    m_pmTrd.dwDTrade_Gold,
    dstLevel,
    dstName,
    dst->m_dwObjSerial,
    dst->m_pmTrd.dwDTrade_Dalant,
    dst->m_pmTrd.dwDTrade_Gold);
  m_pmTrd.Init();
  dst->m_pmTrd.Init();
}

void CPlayer::pc_UpdateDataForTrade(CPlayer *pTrader)
{
  _qry_case_update_data_for_trade qryData{};
  qryData.tradelist[0].dwSerial = m_pUserDB->m_dwSerial;
  qryData.tradelist[0].dwDalant = CPlayerDB::GetDalant(&m_Param);
  qryData.tradelist[0].dwGlod = CPlayerDB::GetGold(&m_Param);
  qryData.tradelist[0].pNewData = &m_pUserDB->m_AvatorData;
  qryData.tradelist[0].pOldData = &m_pUserDB->m_AvatorData_bk;
  qryData.tradelist[1].dwSerial = pTrader->m_pUserDB->m_dwSerial;
  qryData.tradelist[1].dwDalant = CPlayerDB::GetDalant(&pTrader->m_Param);
  qryData.tradelist[1].dwGlod = CPlayerDB::GetGold(&pTrader->m_Param);
  qryData.tradelist[1].pNewData = &pTrader->m_pUserDB->m_AvatorData;
  qryData.tradelist[1].pOldData = &pTrader->m_pUserDB->m_AvatorData_bk;
  const unsigned int size = qryData.size();
  CMainThread::PushDQSData(&g_Main, 0xFFFFFFFF, 0, 0xB0u, reinterpret_cast<char *>(&qryData), size);
}

void CPlayer::SendMsg_DTradeAskResult(char byErrCode)
{
  char msg[1]{};
  msg[0] = byErrCode;
  unsigned __int8 type[2] = {18, 2};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_DTradeAskInform(CPlayer *pAsker)
{
  unsigned __int8 msg[6]{};
  memcpy_0(msg, &pAsker->m_ObjID.m_wIndex, sizeof(pAsker->m_ObjID.m_wIndex));
  memcpy_0(msg + sizeof(pAsker->m_ObjID.m_wIndex), &pAsker->m_dwObjSerial, sizeof(pAsker->m_dwObjSerial));
  unsigned __int8 type[2] = {18, 3};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, msg, 6u);
}

void CPlayer::SendMsg_DTradeAnswerResult(char byErrCode)
{
  char msg[1]{};
  msg[0] = byErrCode;
  unsigned __int8 type[2] = {18, 5};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_DTradeStartInform(CPlayer *pAsker, CPlayer *pAnswer, unsigned int *pdwKey)
{
  unsigned __int8 msg[0x1E]{};
  size_t offset = 0;
  memcpy_0(msg + offset, &pAsker->m_ObjID.m_wIndex, sizeof(pAsker->m_ObjID.m_wIndex));
  offset += sizeof(pAsker->m_ObjID.m_wIndex);
  memcpy_0(msg + offset, &pAsker->m_dwObjSerial, sizeof(pAsker->m_dwObjSerial));
  offset += sizeof(pAsker->m_dwObjSerial);
  msg[offset] = pAsker->m_pmTrd.byEmptyInvenNum;
  offset += sizeof(pAsker->m_pmTrd.byEmptyInvenNum);
  memcpy_0(msg + offset, &pAnswer->m_ObjID.m_wIndex, sizeof(pAnswer->m_ObjID.m_wIndex));
  offset += sizeof(pAnswer->m_ObjID.m_wIndex);
  memcpy_0(msg + offset, &pAnswer->m_dwObjSerial, sizeof(pAnswer->m_dwObjSerial));
  offset += sizeof(pAnswer->m_dwObjSerial);
  msg[offset] = pAnswer->m_pmTrd.byEmptyInvenNum;
  offset += sizeof(pAnswer->m_pmTrd.byEmptyInvenNum);
  if (pdwKey)
  {
    memcpy_0(msg + offset, pdwKey, 0x10u);
  }
  unsigned __int8 type[2] = {18, 6};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, msg, 0x1Eu);
}

void CPlayer::SendMsg_DTradeLockResult(char byErrCode)
{
  char msg[1]{};
  msg[0] = byErrCode;
  unsigned __int8 type[2] = {18, 11};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_DTradeLockInform()
{
  char msg[1]{};
  unsigned __int8 type[2] = {18, 12};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_DTradeAddResult(char byErrCode)
{
  char msg[1]{};
  msg[0] = byErrCode;
  unsigned __int8 type[2] = {18, 14};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_DTradeAddInform(char bySlotIndex, _STORAGE_LIST::_db_con *pItem, unsigned __int8 byAmount)
{
  unsigned __int8 msg[0x17]{};
  size_t offset = 0;
  msg[offset++] = static_cast<unsigned __int8>(bySlotIndex);
  msg[offset++] = pItem->m_byTableCode;
  memcpy_0(msg + offset, &pItem->m_wItemIndex, sizeof(pItem->m_wItemIndex));
  offset += sizeof(pItem->m_wItemIndex);
  memcpy_0(msg + offset, &pItem->m_dwDur, sizeof(pItem->m_dwDur));
  offset += sizeof(pItem->m_dwDur);
  memcpy_0(msg + offset, &pItem->m_dwLv, sizeof(pItem->m_dwLv));
  offset += sizeof(pItem->m_dwLv);
  msg[offset++] = byAmount;
  msg[offset++] = m_pmTrd.byEmptyInvenNum;
  msg[offset++] = pItem->m_byCsMethod;
  memcpy_0(msg + offset, &pItem->m_dwT, sizeof(pItem->m_dwT));
  unsigned __int8 type[2] = {18, 15};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, msg, 0x17u);
}

void CPlayer::SendMsg_DTradeDelResult(char byErrCode)
{
  char msg[1]{};
  msg[0] = byErrCode;
  unsigned __int8 type[2] = {18, 17};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_DTradeDelInform(char bySlotIndex)
{
  unsigned __int8 msg[2]{};
  msg[0] = static_cast<unsigned __int8>(bySlotIndex);
  msg[1] = m_pmTrd.byEmptyInvenNum;
  unsigned __int8 type[2] = {18, 18};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, msg, 2u);
}

void CPlayer::SendMsg_DTradeBetResult(char byErrCode)
{
  char msg[1]{};
  msg[0] = byErrCode;
  unsigned __int8 type[2] = {18, 20};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_DTradeBetInform(char byUnitCode, unsigned int dwAmount)
{
  unsigned __int8 msg[5]{};
  msg[0] = static_cast<unsigned __int8>(byUnitCode);
  memcpy_0(msg + 1, &dwAmount, sizeof(dwAmount));
  unsigned __int8 type[2] = {18, 21};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, msg, 5u);
}

void CPlayer::SendMsg_DTradeOKResult(char byErrCode)
{
  char msg[1]{};
  msg[0] = byErrCode;
  unsigned __int8 type[2] = {18, 23};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_DTradeOKInform()
{
  char msg[1]{};
  unsigned __int8 type[2] = {18, 24};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_DTradeCancleResult(char byErrCode)
{
  char msg[1]{};
  msg[0] = byErrCode;
  unsigned __int8 type[2] = {18, 8};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_DTradeUnitInfoInform(char byTradeSlotIndex, _UNIT_DB_BASE::_LIST *pUnitData)
{
  unsigned __int8 msg[0x38]{};
  size_t offset = 0;
  msg[offset++] = static_cast<unsigned __int8>(byTradeSlotIndex);
  msg[offset++] = pUnitData->byFrame;
  memcpy_0(msg + offset, &pUnitData->dwGauge, sizeof(pUnitData->dwGauge));
  offset += sizeof(pUnitData->dwGauge);
  memcpy_0(msg + offset, pUnitData->byPart, sizeof(pUnitData->byPart));
  offset += sizeof(pUnitData->byPart);
  memcpy_0(msg + offset, pUnitData->dwBullet, sizeof(pUnitData->dwBullet));
  offset += sizeof(pUnitData->dwBullet);
  memcpy_0(msg + offset, pUnitData->dwSpare, sizeof(pUnitData->dwSpare));
  offset += sizeof(pUnitData->dwSpare);
  memcpy_0(msg + offset, &pUnitData->nPullingFee, sizeof(pUnitData->nPullingFee));
  unsigned __int8 type[2] = {18, 27};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, msg, 0x38u);
}

void CPlayer::SendMsg_DTradeUnitAddInform(unsigned __int16 wUnitKeySerial, _UNIT_DB_BASE::_LIST *pUnitData)
{
  unsigned __int8 msg[0x3A]{};
  size_t offset = 0;
  memcpy_0(msg + offset, &wUnitKeySerial, sizeof(wUnitKeySerial));
  offset += sizeof(wUnitKeySerial);
  msg[offset++] = pUnitData->bySlotIndex;
  msg[offset++] = pUnitData->byFrame;
  memcpy_0(msg + offset, &pUnitData->dwGauge, sizeof(pUnitData->dwGauge));
  offset += sizeof(pUnitData->dwGauge);
  memcpy_0(msg + offset, pUnitData->byPart, sizeof(pUnitData->byPart));
  offset += sizeof(pUnitData->byPart);
  memcpy_0(msg + offset, pUnitData->dwBullet, sizeof(pUnitData->dwBullet));
  offset += sizeof(pUnitData->dwBullet);
  memcpy_0(msg + offset, pUnitData->dwSpare, sizeof(pUnitData->dwSpare));
  offset += sizeof(pUnitData->dwSpare);
  memcpy_0(msg + offset, &pUnitData->nPullingFee, sizeof(pUnitData->nPullingFee));
  unsigned __int8 type[2] = {18, 28};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, msg, 0x3Au);
}

void CPlayer::SendMsg_DTradeAccomplishInform(bool bSucc, unsigned __int16 wStartSerial)
{
  unsigned __int8 msg[0x0B]{};
  size_t offset = 0;
  const unsigned int dalant = CPlayerDB::GetDalant(&m_Param);
  const unsigned int gold = CPlayerDB::GetGold(&m_Param);
  memcpy_0(msg + offset, &dalant, sizeof(dalant));
  offset += sizeof(dalant);
  memcpy_0(msg + offset, &gold, sizeof(gold));
  offset += sizeof(gold);
  memcpy_0(msg + offset, &wStartSerial, sizeof(wStartSerial));
  offset += sizeof(wStartSerial);
  msg[offset] = bSucc ? 1 : 0;
  unsigned __int8 type[2] = {18, 25};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, msg, 0xBu);
}

void CPlayer::SendMsg_PartyJoinInvitationQuestion(unsigned __int16 wJoinerIndex)
{
  unsigned __int8 type[2] = {16, 2};
  char msg[6]{};
  *reinterpret_cast<unsigned __int16 *>(msg) = m_ObjID.m_wIndex;
  *reinterpret_cast<unsigned int *>(msg + 2) = m_dwObjSerial;
  g_Network.m_pProcess[0]->LoadSendMsg(wJoinerIndex, type, msg, 6u);
}

void CPlayer::SendMsg_PartyJoinFailLevel()
{
  unsigned __int8 type[2] = {16, 66};
  char msg[1]{};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_PartyJoinApplicationQuestion(CPlayer *pJoiner)
{
  unsigned __int8 type[2] = {16, 5};
  char msg[6]{};
  *reinterpret_cast<unsigned __int16 *>(msg) = pJoiner->m_ObjID.m_wIndex;
  *reinterpret_cast<unsigned int *>(msg + 2) = pJoiner->m_dwObjSerial;
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 6u);
}

void CPlayer::SendMsg_PartyJoinMemberResult(CPartyPlayer *pJoiner, char byLootShareMode)
{
  _party_join_member_result_zocl msg{};
  msg.dwJoinerSerial = pJoiner->m_id.dwSerial;
  strcpy_0(msg.wszJoinerName, pJoiner->m_wszName);
  msg.byLootShareMode = static_cast<unsigned __int8>(byLootShareMode);
  msg.wIndex = pJoiner->m_id.wIndex;

  unsigned __int8 type[2] = {16, 8};
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(msg.size()));
}

void CPlayer::SendMsg_PartyJoinJoinerResult()
{
  _party_join_joiner_result_zocl msg{};
  CPartyPlayer **members = m_pPartyMgr->GetPtrPartyMember();
  if (!members)
  {
    return;
  }

  msg.byLootShareMode = m_pPartyMgr->m_pPartyBoss->m_byLootShareSystem;
  msg.byListNum = static_cast<unsigned __int8>(m_pPartyMgr->GetPopPartyMember() - 1);
  for (int index = 0; index < msg.byListNum && members[index]; ++index)
  {
    if (members[index] != m_pPartyMgr)
    {
      msg.List[index].wIndex = members[index]->m_id.wIndex;
      msg.List[index].dwSerial = members[index]->m_id.dwSerial;
      strcpy_0(msg.List[index].wszAvatorName, members[index]->m_wszName);
    }
  }

  unsigned __int8 type[2] = {16, 7};
  const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
}

void CPlayer::SendMsg_PartyLeaveCompulsionResult(CPartyPlayer *pLeaver)
{
  char msg[4]{};
  if (pLeaver)
  {
    *reinterpret_cast<unsigned int *>(msg) = pLeaver->m_id.dwSerial;
  }
  else
  {
    *reinterpret_cast<unsigned int *>(msg) = static_cast<unsigned int>(-1);
  }

  unsigned __int8 type[2] = {16, 12};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 4u);
}

void CPlayer::SendMsg_PartyDisjointResult(char bSuccess)
{
  char msg[1]{};
  msg[0] = bSuccess;

  unsigned __int8 type[2] = {16, 14};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_PartyLockResult(char byRet)
{
  char msg[1]{};
  msg[0] = byRet;

  unsigned __int8 type[2] = {16, 18};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_PartyAlterLootShareResult(char byLootShareMode)
{
  char msg[1]{};
  msg[0] = byLootShareMode;

  unsigned __int8 type[2] = {16, 29};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendData_PartyMemberInfo(unsigned __int16 wDstIndex)
{
  _party_member_info_upd msg{};
  msg.dwMemSerial = m_dwObjSerial;
  msg.wHPRate = static_cast<unsigned __int16>(CalcCurHPRate());
  msg.wFPRate = static_cast<unsigned __int16>(CalcCurFPRate());
  msg.wSPRate = static_cast<unsigned __int16>(CalcCurSPRate());
  msg.byLv = static_cast<unsigned __int8>(CPlayerDB::GetLevel(&m_Param));
  msg.byMapCode = static_cast<unsigned __int8>(CPlayerDB::GetMapCode(&m_Param));
  msg.zPos[0] = static_cast<int>(m_fCurPos[0]);
  msg.zPos[1] = static_cast<int>(m_fCurPos[2]);
  msg.byContEffectNum = 0;

  unsigned __int8 effectCount = 0;
  for (int listIndex = 0; listIndex < 2; ++listIndex)
  {
    for (int effectIndex = 0; effectIndex < 8; ++effectIndex)
    {
      _sf_continous *effect = &m_SFCont[listIndex][effectIndex];
      if (effect->m_bExist)
      {
        const unsigned __int16 effectBit =
          static_cast<unsigned __int16>(CalcEffectBit(effect->m_byEffectCode, effect->m_wEffectIndex));
        msg.Effect[effectCount].wEffectCode = effectBit;
        msg.Effect[effectCount++].byEffectLv = effect->m_byLv;
      }
    }
  }

  msg.byContEffectNum = effectCount;
  unsigned __int8 type[2] = {16, 19};
  const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg(wDstIndex, type, reinterpret_cast<char *>(&msg), len);
}

void CPlayer::SendData_PartyMemberInfoToMembers()
{
  _party_member_info_upd msg{};
  msg.dwMemSerial = m_dwObjSerial;
  msg.wHPRate = static_cast<unsigned __int16>(CalcCurHPRate());
  msg.wFPRate = static_cast<unsigned __int16>(CalcCurFPRate());
  msg.wSPRate = static_cast<unsigned __int16>(CalcCurSPRate());
  msg.byLv = static_cast<unsigned __int8>(CPlayerDB::GetLevel(&m_Param));
  msg.byMapCode = static_cast<unsigned __int8>(CPlayerDB::GetMapCode(&m_Param));
  msg.zPos[0] = static_cast<int>(m_fCurPos[0]);
  msg.zPos[1] = static_cast<int>(m_fCurPos[2]);
  msg.byContEffectNum = 0;

  unsigned __int8 effectCount = 0;
  for (int listIndex = 0; listIndex < 2; ++listIndex)
  {
    for (int effectIndex = 0; effectIndex < 8; ++effectIndex)
    {
      _sf_continous *effect = &m_SFCont[listIndex][effectIndex];
      if (effect->m_bExist)
      {
        const unsigned __int16 effectBit =
          static_cast<unsigned __int16>(CalcEffectBit(effect->m_byEffectCode, effect->m_wEffectIndex));
        msg.Effect[effectCount].wEffectCode = effectBit;
        msg.Effect[effectCount++].byEffectLv = effect->m_byLv;
      }
    }
  }

  msg.byContEffectNum = effectCount;

  CPartyPlayer **members = m_pPartyMgr->GetPtrPartyMember();
  if (!members)
  {
    return;
  }

  const int memberCount = static_cast<int>(m_pPartyMgr->GetPopPartyMember());
  unsigned __int8 type[2] = {16, 19};
  for (int index = 0; index < memberCount; ++index)
  {
    if (members[index] != m_pPartyMgr)
    {
      const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
      g_Network.m_pProcess[0]->LoadSendMsg(members[index]->m_wZoneIndex, type, reinterpret_cast<char *>(&msg), len);
    }
  }
}

void CPlayer::SendMsg_AwayPartyInvitationQuestion(unsigned __int16 wJoinerIndex)
{
  unsigned __int8 type[2] = {16, 33};
  char msg[23]{};
  *reinterpret_cast<unsigned __int16 *>(msg) = m_ObjID.m_wIndex;
  *reinterpret_cast<unsigned int *>(msg + 2) = m_dwObjSerial;
  const char *charName = CPlayerDB::GetCharNameA(&m_Param);
  strcpy_0(msg + 6, charName);
  g_Network.m_pProcess[0]->LoadSendMsg(wJoinerIndex, type, msg, 0x17u);
}

void CPlayer::SendMsg_AwayPartyRequestResult(char byRetCode)
{
  unsigned __int8 type[2] = {16, 32};
  char msg[1]{};
  msg[0] = byRetCode;
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_PartyLeaveSelfResult(CPartyPlayer *pLeaver, bool bWorldExit)
{
#pragma pack(push, 1)
  struct PartyLeaveSelfResultMsg
  {
    unsigned int dwSerial;
    bool bWorldExit;
  };
#pragma pack(pop)

  PartyLeaveSelfResultMsg msg{};
  msg.bWorldExit = bWorldExit;
  msg.dwSerial = pLeaver ? pLeaver->m_id.dwSerial : static_cast<unsigned int>(-1);

  unsigned __int8 type[2] = {16, 10};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 5u);
}

void CPlayer::SendMsg_PartySuccessResult(CPartyPlayer *pSuccessor)
{
  char msg[4]{};
  if (pSuccessor)
  {
    *reinterpret_cast<unsigned int *>(msg) = pSuccessor->m_id.dwSerial;
  }
  else
  {
    *reinterpret_cast<unsigned int *>(msg) = static_cast<unsigned int>(-1);
  }

  unsigned __int8 type[2] = {16, 16};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 4u);
}

void CPlayer::SendMsg_GuildMasterEffect(
  char byState,
  char byGrade,
  char byEffSubAttack,
  unsigned __int8 byEffSubDefence,
  unsigned __int8 byEffAddAttack,
  unsigned __int8 byEffAddDefence)
{
#pragma pack(push, 1)
  struct GuildMasterEffectMsg
  {
    char byState;
    char byGrade;
    char byEffSubAttack;
    unsigned __int8 byEffSubDefence;
    unsigned __int8 byEffAddAttack;
    unsigned __int8 byEffAddDefence;
  };
#pragma pack(pop)

  GuildMasterEffectMsg msg{};
  msg.byState = byState;
  msg.byGrade = byGrade;
  msg.byEffSubAttack = byEffSubAttack;
  msg.byEffSubDefence = byEffSubDefence;
  msg.byEffAddAttack = byEffAddAttack;
  msg.byEffAddDefence = byEffAddDefence;

  unsigned __int8 type[2] = {27, 120};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 6u);
}

void CPlayer::pc_PartyJoinInvitation(unsigned __int16 wDstIndex)
{
  if (TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, m_id.wIndex) == 99)
  {
    SendMsg_TLStatusPenalty(1u);
    return;
  }
  if (IsPunished(2u, true))
  {
    return;
  }

  CPlayer *target = &g_Player[wDstIndex];
  if (target->m_bLive && !target->m_bCorpse && target->m_pCurMap == m_pCurMap)
  {
    if (TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, target->m_id.wIndex) == 99)
    {
      SendMsg_TLStatusPenalty(3u);
      return;
    }
    if (!target->IsPunished(2u, false))
    {
      if (target->m_EP.GetEff_Have(50) > 0.0f)
      {
        SendMsg_JadeEffectErr(1u);
        return;
      }
      const int myRace = CPlayerDB::GetRaceCode(&m_Param);
      const int targetRace = CPlayerDB::GetRaceCode(&target->m_Param);
      if (myRace == targetRace)
      {
        if (target->m_byUserDgr)
        {
          if (!m_byUserDgr)
          {
            return;
          }
        }
        else if (m_byUserDgr)
        {
          return;
        }
        __int16 *dstBillingType = &target->m_pUserDB->m_BillingInfo.iType;
        __int16 *srcBillingType = &m_pUserDB->m_BillingInfo.iType;
        CNationSettingManager *nationSetting = CTSingleton<CNationSettingManager>::Instance();
        if (!CNationSettingManager::IsPersonalFreeFixedAmountBillingType(nationSetting, srcBillingType, dstBillingType)
            && !target->m_pPartyMgr->IsPartyMode() && !m_pPartyMgr->m_bLock)
        {
          const float prof = target->m_EP.GetEff_Have(53);
          const int level = static_cast<int>(target->GetLevel());
          if (m_pPartyMgr->IsJoinPartyLevel(level, prof))
          {
            if (m_pPartyMgr->IsPartyBoss() || !m_pPartyMgr->IsPartyMode())
            {
              SendMsg_PartyJoinInvitationQuestion(wDstIndex);
            }
          }
          else
          {
            SendMsg_PartyJoinFailLevel();
          }
        }
      }
    }
  }
}

void CPlayer::pc_PartyJoinInvitationAnswer(_CLID *pidBoss)
{
  if (TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, m_id.wIndex) == 99)
  {
    SendMsg_TLStatusPenalty(1u);
    return;
  }
  if (IsPunished(2u, true))
  {
    return;
  }

  CPlayer *boss = &g_Player[pidBoss->wIndex];
  if (boss->m_dwObjSerial == pidBoss->dwSerial && boss->m_bLive && !boss->IsPunished(2u, false))
  {
    const int myRace = CPlayerDB::GetRaceCode(&m_Param);
    const int bossRace = CPlayerDB::GetRaceCode(&boss->m_Param);
    if (myRace == bossRace)
    {
      if (boss->m_byUserDgr)
      {
        if (!m_byUserDgr)
        {
          return;
        }
      }
      else if (m_byUserDgr)
      {
        return;
      }
      if (!m_pPartyMgr->IsPartyMode() && !boss->m_pPartyMgr->m_bLock)
      {
        const float prof = m_EP.GetEff_Have(53);
        const int level = static_cast<int>(GetLevel());
        if (boss->m_pPartyMgr->IsJoinPartyLevel(level, prof))
        {
          wa_PartyJoin(&boss->m_id, &m_id);
          if (m_pPartyMgr->IsPartyMode())
          {
            if (boss->m_GroupTargetObject[0].pObject)
            {
              m_GroupTargetObject[0].init();
              m_GroupTargetObject[0].pObject = boss->m_GroupTargetObject[0].pObject;
              m_GroupTargetObject[0].byKind = boss->m_GroupTargetObject[0].byKind;
              m_GroupTargetObject[0].byID = boss->m_GroupTargetObject[0].byID;
              m_GroupTargetObject[0].dwSerial = boss->m_GroupTargetObject[0].dwSerial;
              SendMsg_SetGroupTargetObjectResult(0, 0);
            }
          }
        }
        else
        {
          SendMsg_PartyJoinFailLevel();
        }
      }
    }
  }
}

void CPlayer::pc_PartyJoinApplication(unsigned __int16 wBossIndex)
{
  if (TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, m_id.wIndex) == 99)
  {
    SendMsg_TLStatusPenalty(1u);
    return;
  }
  if (IsPunished(2u, true))
  {
    return;
  }

  CPlayer *boss = &g_Player[wBossIndex];
  if (boss->m_bLive && !boss->m_bCorpse)
  {
    if (TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, boss->m_id.wIndex) == 99)
    {
      SendMsg_TLStatusPenalty(3u);
      return;
    }
    if (!boss->IsPunished(2u, false))
    {
      const int myRace = CPlayerDB::GetRaceCode(&m_Param);
      const int bossRace = CPlayerDB::GetRaceCode(&boss->m_Param);
      if (myRace == bossRace && !m_pPartyMgr->IsPartyMode() && boss->m_pPartyMgr->IsPartyBoss())
      {
        if (boss->m_byUserDgr)
        {
          if (!m_byUserDgr)
          {
            return;
          }
        }
        else if (m_byUserDgr)
        {
          return;
        }
        __int16 *billingType = &m_pUserDB->m_BillingInfo.iType;
        CNationSettingManager *nationSetting = CTSingleton<CNationSettingManager>::Instance();
        if (!CNationSettingManager::IsPersonalFreeFixedAmountBillingType(nationSetting, billingType, nullptr)
            && !boss->m_pPartyMgr->m_bLock)
        {
          const float prof = m_EP.GetEff_Have(53);
          const int level = static_cast<int>(GetLevel());
          if (boss->m_pPartyMgr->IsJoinPartyLevel(level, prof))
          {
            boss->SendMsg_PartyJoinApplicationQuestion(this);
          }
          else
          {
            SendMsg_PartyJoinFailLevel();
          }
        }
      }
    }
  }
}

void CPlayer::pc_PartyJoinApplicationAnswer(_CLID *pidApplicant)
{
  if (TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, m_id.wIndex) == 99)
  {
    SendMsg_TLStatusPenalty(1u);
    return;
  }
  if (IsPunished(2u, true))
  {
    return;
  }

  CPlayer *applicant = &g_Player[pidApplicant->wIndex];
  if (applicant->m_id.dwSerial == pidApplicant->dwSerial && applicant->m_bLive && !applicant->m_bCorpse
      && applicant->m_pCurMap == m_pCurMap && !applicant->IsPunished(2u, false))
  {
    const int myRace = CPlayerDB::GetRaceCode(&m_Param);
    const int appRace = CPlayerDB::GetRaceCode(&applicant->m_Param);
    if (myRace == appRace)
    {
      if (applicant->m_byUserDgr)
      {
        if (!m_byUserDgr)
        {
          return;
        }
      }
      else if (m_byUserDgr)
      {
        return;
      }
      if (!m_pPartyMgr->m_bLock)
      {
        const float prof = applicant->m_EP.GetEff_Have(53);
        const int level = static_cast<int>(applicant->GetLevel());
        if (m_pPartyMgr->IsJoinPartyLevel(level, prof))
        {
          wa_PartyJoin(&m_id, &applicant->m_id);
        }
        else
        {
          SendMsg_PartyJoinFailLevel();
        }
      }
    }
  }
}

void CPlayer::pc_PartyLeaveCompulsionReqeuest(unsigned int dwExiterSerial)
{
  if (m_pPartyMgr->IsPartyBoss())
  {
    CPartyPlayer *exiter = m_pPartyMgr->GetPtrFromSerial(dwExiterSerial);
    if (exiter)
    {
      wa_PartyForceLeave(&m_id, &exiter->m_id);
      return;
    }
  }
  SendMsg_PartyLeaveCompulsionResult(nullptr);
}

void CPlayer::pc_PartyDisJointReqeuest()
{
  if (m_pPartyMgr->IsPartyBoss())
  {
    wa_PartyDisjoint(&m_id);
  }
  else
  {
    SendMsg_PartyDisjointResult(0);
  }
}

void CPlayer::pc_PartySuccessionReqeuest(unsigned int dwSuccessorSerial)
{
  if (m_pPartyMgr->IsPartyBoss())
  {
    CPartyPlayer *successor = m_pPartyMgr->GetPtrFromSerial(dwSuccessorSerial);
    if (successor)
    {
      wa_PartySuccession(&m_id, &successor->m_id);
      return;
    }
  }
  SendMsg_PartySuccessResult(nullptr);
}

void CPlayer::pc_PartyLockReqeuest(bool bLock)
{
  if (!m_pPartyMgr->IsPartyBoss() || m_pPartyMgr->m_bLock == bLock)
  {
    SendMsg_PartyLockResult(static_cast<char>(0xFF));
  }
  else
  {
    wa_PartyLock(&m_id, bLock);
  }
}

void CPlayer::pc_PartyAlterLootShareReqeuest(unsigned __int8 byLootShareMode)
{
  if (m_pPartyMgr->IsPartyBoss())
  {
    wa_PartyLootShareSystem(&m_id, byLootShareMode);
  }
  else
  {
    SendMsg_PartyAlterLootShareResult(static_cast<char>(0xFF));
  }
}

void CPlayer::pc_AwaypartyInvitationRequest(char *pwszCharName)
{
  unsigned __int8 retCode = 0;
  if (IsPunished(2u, true) || TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, m_id.wIndex) == 99)
  {
    return;
  }

  CPlayer *target = g_Main.GetCharW(pwszCharName);
  if (!target || target->m_dwObjSerial == m_dwObjSerial)
  {
    retCode = 1;
    SendMsg_AwayPartyRequestResult(1u);
    return;
  }

  const int targetRace = CPlayerDB::GetRaceCode(&target->m_Param);
  const int myRace = CPlayerDB::GetRaceCode(&m_Param);
  if (targetRace != myRace)
  {
    retCode = 2;
    SendMsg_AwayPartyRequestResult(2u);
    return;
  }

  if (!target->m_bOper || target->m_bCorpse)
  {
    retCode = 3;
    SendMsg_AwayPartyRequestResult(3u);
    return;
  }
  if (target->IsPunished(2u, false))
  {
    return;
  }
  if (target->m_byUserDgr)
  {
    if (!m_byUserDgr)
    {
      retCode = 3;
      SendMsg_AwayPartyRequestResult(3u);
      return;
    }
  }
  else if (m_byUserDgr)
  {
    retCode = 3;
    SendMsg_AwayPartyRequestResult(3u);
    return;
  }

  if (target->m_pPartyMgr->IsPartyMode())
  {
    retCode = 4;
    SendMsg_AwayPartyRequestResult(4u);
  }
  else if (!m_pPartyMgr->IsPartyMode() || m_pPartyMgr->IsPartyBoss())
  {
    if (m_pPartyMgr->m_bLock)
    {
      retCode = 7;
      SendMsg_AwayPartyRequestResult(7u);
    }
    else
    {
      const float prof = target->m_EP.GetEff_Have(53);
      const int level = static_cast<int>(target->GetLevel());
      if (m_pPartyMgr->IsJoinPartyLevel(level, prof))
      {
        if (target->m_bBlockParty)
        {
          retCode = 6;
          SendMsg_AwayPartyRequestResult(6u);
        }
        else if (!g_Main.m_bAwayPartyConsumeItem
                 || _STORAGE_LIST::GetPtrFromItemCode(&target->m_Param.m_dbInven, g_Main.m_strAwayPartyItemCode))
        {
          if (g_Main.m_bAwayPartyConsumeMoney && g_Main.m_dwAwayPartyMoney > target->GetMoney(0))
          {
            retCode = 11;
            SendMsg_AwayPartyRequestResult(0xBu);
          }
          else
          {
            SendMsg_AwayPartyRequestResult(retCode);
            SendMsg_AwayPartyInvitationQuestion(target->m_ObjID.m_wIndex);
          }
        }
        else
        {
          retCode = 9;
          SendMsg_AwayPartyRequestResult(9u);
        }
      }
      else
      {
        retCode = 5;
        SendMsg_AwayPartyRequestResult(5u);
      }
    }
  }
  else
  {
    retCode = 8;
    SendMsg_AwayPartyRequestResult(8u);
  }
}

void CPlayer::pc_AwayPartyJoinInvitationAnswer(_CLID *pidBoss, unsigned __int8 byRetCode)
{
  unsigned __int8 retCode = 0;
  if (IsPunished(2u, true))
  {
    return;
  }

  CPlayer *boss = &g_Player[pidBoss->wIndex];
  if (boss->m_dwObjSerial == pidBoss->dwSerial && boss->m_bLive && boss->m_bOper)
  {
    const int myRace = CPlayerDB::GetRaceCode(&m_Param);
    const int bossRace = CPlayerDB::GetRaceCode(&boss->m_Param);
    if (myRace == bossRace && !boss->IsPunished(2u, false))
    {
      if (boss->m_byUserDgr)
      {
        if (!m_byUserDgr)
        {
          return;
        }
      }
      else if (m_byUserDgr)
      {
        return;
      }

      if (boss->m_pPartyMgr->m_bLock)
      {
        retCode = 7;
        SendMsg_AwayPartyRequestResult(7u);
        boss->SendMsg_AwayPartyRequestResult(retCode);
        return;
      }
      if (boss->m_pPartyMgr->IsPartyMode() && !boss->m_pPartyMgr->IsPartyBoss())
      {
        retCode = 8;
        SendMsg_AwayPartyRequestResult(8u);
        boss->SendMsg_AwayPartyRequestResult(retCode);
        return;
      }
      if (m_pPartyMgr->IsPartyMode())
      {
        retCode = 4;
        SendMsg_AwayPartyRequestResult(4u);
        boss->SendMsg_AwayPartyRequestResult(retCode);
        return;
      }

      const float prof = m_EP.GetEff_Have(53);
      const int level = static_cast<int>(GetLevel());
      if (!boss->m_pPartyMgr->IsJoinPartyLevel(level, prof))
      {
        retCode = 5;
        SendMsg_AwayPartyRequestResult(5u);
        boss->SendMsg_AwayPartyRequestResult(retCode);
        return;
      }

      _STORAGE_LIST::_db_con *item = nullptr;
      if (g_Main.m_bAwayPartyConsumeItem)
      {
        item = _STORAGE_LIST::GetPtrFromItemCode(&m_Param.m_dbInven, g_Main.m_strAwayPartyItemCode);
        if (!item)
        {
          retCode = 9;
          SendMsg_AwayPartyRequestResult(9u);
          boss->SendMsg_AwayPartyRequestResult(retCode);
          return;
        }
      }

      if (g_Main.m_bAwayPartyConsumeMoney && g_Main.m_dwAwayPartyMoney > GetMoney(0))
      {
        retCode = 11;
        SendMsg_AwayPartyRequestResult(0xBu);
        boss->SendMsg_AwayPartyRequestResult(retCode);
        return;
      }

      if (byRetCode)
      {
        retCode = 10;
        boss->SendMsg_AwayPartyRequestResult(0xAu);
        return;
      }

      wa_PartyJoin(&boss->m_id, &m_id);
      if (!m_pPartyMgr->IsPartyMode())
      {
        retCode = 7;
        SendMsg_AwayPartyRequestResult(7u);
        boss->SendMsg_AwayPartyRequestResult(retCode);
        return;
      }

      if (g_Main.m_bAwayPartyConsumeItem && item)
      {
        const unsigned __int16 newAmount = Emb_AlterDurPoint(0, item->m_byStorageIndex, -1, 0, 1);
        if (newAmount)
        {
          SendMsg_AdjustAmountInform(0, item->m_wSerial, newAmount);
        }
        else
        {
          CMgrAvatorItemHistory::consume_del_item(
            &CPlayer::s_MgrItemHistory,
            m_ObjID.m_wIndex,
            item,
            m_szItemHistoryFileName);
        }
      }
      if (g_Main.m_bAwayPartyConsumeMoney)
      {
        SubDalant(g_Main.m_dwAwayPartyMoney);
        SendMsg_AlterMoneyInform(0);
        const int curLevel = CPlayerDB::GetLevel(&m_Param);
        if (curLevel == 30 || curLevel == 40 || curLevel == 50 || curLevel == 60)
        {
          const int level = CPlayerDB::GetLevel(&m_Param);
          const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
          CMoneySupplyMgr *moneySupply = CMoneySupplyMgr::Instance();
          CMoneySupplyMgr::UpdateFeeMoneyData(moneySupply, raceCode, level, g_Main.m_dwAwayPartyMoney);
        }
      }
    }
  }
}

void CPlayer::pc_PartyLeaveSelfReqeuest()
{
  if (m_pPartyMgr->IsPartyMode())
  {
    wa_PartySelfLeave(&m_id);
  }
  else
  {
    SendMsg_PartyLeaveSelfResult(nullptr, false);
  }
}

void wa_PartySelfLeave(_CLID *pidLeaver)
{
  CPartyPlayer *exiter = &g_PartyPlayer[pidLeaver->wIndex];
  CPartyPlayer *partyBoss = exiter->m_pPartyBoss;

  if (exiter->m_id.dwSerial == pidLeaver->dwSerial && exiter->m_bLogin && exiter->IsPartyMode()
      && partyBoss->IsPartyBoss())
  {
    CPartyPlayer *prevMembers[8]{};
    for (int j = 0; j < 8; ++j)
    {
      prevMembers[j] = partyBoss->m_pPartyMember[j];
    }

    CPartyPlayer *newBoss = nullptr;
    if (partyBoss->RemovePartyMember(exiter, &newBoss))
    {
      for (int j = 0; j < 8 && prevMembers[j]; ++j)
      {
        CPlayer *notifyPlayer = &g_Player[prevMembers[j]->m_id.wIndex];
        notifyPlayer->SendMsg_PartyLeaveSelfResult(exiter, false);
      }
    }
  }
}

void wa_PartyJoin(_CLID *pidBoss, _CLID *pidJoiner)
{
  CPartyPlayer *boss = &g_PartyPlayer[pidBoss->wIndex];
  CPartyPlayer *joiner = &g_PartyPlayer[pidJoiner->wIndex];
  if (boss->m_id.dwSerial == pidBoss->dwSerial && joiner->m_id.dwSerial == pidJoiner->dwSerial && boss->m_bLogin
      && joiner->m_bLogin && !joiner->IsPartyMode()
      && (!boss->IsPartyMode() || boss->IsPartyBoss()) && !boss->IsPartyLock())
  {
    if (boss->IsPartyBoss())
    {
      if (!boss->InsertPartyMember(joiner))
      {
        return;
      }
    }
    else if (!boss->IsPartyMode() && !boss->FoundParty(joiner))
    {
      return;
    }

    CPlayer *joinerPlayer = &g_Player[joiner->m_wZoneIndex];
    for (int index = 0; index < 8; ++index)
    {
      CPartyPlayer *member = boss->m_pPartyMember[index];
      if (!member)
      {
        break;
      }
      if (member != joiner)
      {
        CPlayer *memberPlayer = &g_Player[member->m_wZoneIndex];
        memberPlayer->SendMsg_PartyJoinMemberResult(joiner, boss->m_byLootShareSystem);
      }
    }

    if (joinerPlayer)
    {
      joinerPlayer->SendMsg_PartyJoinJoinerResult();
    }

    for (int index = 0; index < 8; ++index)
    {
      CPartyPlayer *member = boss->m_pPartyMember[index];
      if (!member)
      {
        break;
      }
      if (member != joiner)
      {
        CPlayer *memberPlayer = &g_Player[member->m_wZoneIndex];
        memberPlayer->SendData_PartyMemberInfo(joiner->m_wZoneIndex);
      }
    }

    if (joinerPlayer)
    {
      joinerPlayer->SendData_PartyMemberInfoToMembers();
    }
  }
}

void wa_PartyForceLeave(_CLID *pidBoss, _CLID *pidLeaver)
{
  CPartyPlayer *boss = &g_PartyPlayer[pidBoss->wIndex];
  CPartyPlayer *exiter = &g_PartyPlayer[pidLeaver->wIndex];
  if (boss->m_id.dwSerial == pidBoss->dwSerial && exiter->m_id.dwSerial == pidLeaver->dwSerial && boss->m_bLogin
      && exiter->m_bLogin && exiter->IsPartyMode() && boss->IsPartyBoss() && exiter->m_pPartyBoss == boss)
  {
    CPartyPlayer *prevMembers[8]{};
    for (int index = 0; index < 8; ++index)
    {
      prevMembers[index] = boss->m_pPartyMember[index];
    }

    CPartyPlayer *newBoss = nullptr;
    if (boss->RemovePartyMember(exiter, &newBoss))
    {
      for (int index = 0; index < 8 && prevMembers[index]; ++index)
      {
        CPlayer *memberPlayer = &g_Player[prevMembers[index]->m_wZoneIndex];
        memberPlayer->SendMsg_PartyLeaveCompulsionResult(exiter);
      }
    }
  }
}

void wa_PartyDisjoint(_CLID *pidBoss)
{
  CPartyPlayer *boss = &g_PartyPlayer[pidBoss->wIndex];
  if (boss->m_id.dwSerial == pidBoss->dwSerial && boss->IsPartyBoss())
  {
    CPartyPlayer *prevMembers[8]{};
    for (int index = 0; index < 8; ++index)
    {
      prevMembers[index] = boss->m_pPartyMember[index];
    }
    boss->DisjointParty();
    for (int index = 0; index < 8 && prevMembers[index]; ++index)
    {
      CPlayer *memberPlayer = &g_Player[prevMembers[index]->m_wZoneIndex];
      memberPlayer->SendMsg_PartyDisjointResult(1);
    }
  }
}

void wa_PartySuccession(_CLID *pidBoss, _CLID *pidSuccessor)
{
  CPartyPlayer *boss = &g_PartyPlayer[pidBoss->wIndex];
  CPartyPlayer *successor = &g_PartyPlayer[pidSuccessor->wIndex];
  if (boss->m_id.dwSerial == pidBoss->dwSerial && successor->m_id.dwSerial == pidSuccessor->dwSerial && boss->m_bLogin
      && successor->m_bLogin && boss->IsPartyBoss() && successor->IsPartyMode() && successor->m_pPartyBoss == boss
      && boss->InheritBoss(successor))
  {
    for (int index = 0; index < 8; ++index)
    {
      CPartyPlayer *member = successor->m_pPartyMember[index];
      if (!member)
      {
        break;
      }
      CPlayer *memberPlayer = &g_Player[member->m_wZoneIndex];
      memberPlayer->SendMsg_PartySuccessResult(successor);
    }
  }
}

void wa_PartyLock(_CLID *pidBoss, bool bLock)
{
  CPartyPlayer *boss = &g_PartyPlayer[pidBoss->wIndex];
  if (boss->m_id.dwSerial == pidBoss->dwSerial && boss->m_bLogin && boss->IsPartyBoss()
      && boss->m_bLock != bLock && boss->SetLockMode(bLock))
  {
    for (int index = 0; index < 8; ++index)
    {
      CPartyPlayer *member = boss->m_pPartyMember[index];
      if (!member)
      {
        break;
      }
      CPlayer *memberPlayer = &g_Player[member->m_wZoneIndex];
      memberPlayer->SendMsg_PartyLockResult(static_cast<char>(bLock));
    }
  }
}

void wa_PartyLootShareSystem(_CLID *pidBoss, unsigned __int8 byLootShareMode)
{
  CPartyPlayer *boss = &g_PartyPlayer[pidBoss->wIndex];
  if (boss->m_id.dwSerial == pidBoss->dwSerial && boss->m_bLogin && boss->IsPartyBoss()
      && boss->SetLootShareMode(byLootShareMode))
  {
    for (int index = 0; index < 8; ++index)
    {
      CPartyPlayer *member = boss->m_pPartyMember[index];
      if (!member)
      {
        break;
      }
      CPlayer *memberPlayer = &g_Player[member->m_wZoneIndex];
      memberPlayer->SendMsg_PartyAlterLootShareResult(static_cast<char>(byLootShareMode));
    }
  }
}

void wa_ExitWorld(_CLID *pidWorld)
{
  CPartyPlayer *leaver = &g_PartyPlayer[pidWorld->wIndex];
  if (leaver->m_id.dwSerial == pidWorld->dwSerial && leaver->m_bLogin)
  {
    CPartyPlayer *oldBoss = leaver->m_pPartyBoss;
    CPartyPlayer *newBoss = leaver->m_pPartyBoss;
    if (leaver->IsPartyMode())
    {
      CPartyPlayer **members = leaver->GetPtrPartyMember();
      for (int j = 0; j < 8 && members[j]; ++j)
      {
        if (members[j] != leaver)
        {
          CPlayer *notifyPlayer = &g_Player[members[j]->m_wZoneIndex];
          notifyPlayer->SendMsg_PartyLeaveSelfResult(leaver, true);
        }
      }
    }
    leaver->ExitWorld(&newBoss);
    if (newBoss && newBoss != oldBoss)
    {
      CPartyPlayer **members = newBoss->GetPtrPartyMember();
      for (int k = 0; k < 8 && members[k]; ++k)
      {
        CPlayer *notifyPlayer = &g_Player[members[k]->m_wZoneIndex];
        notifyPlayer->SendMsg_PartySuccessResult(newBoss);
      }
    }
  }
}



_target_monster_contsf_allinform_zocl::_target_monster_contsf_allinform_zocl()
{
  Init();
}

void _target_monster_contsf_allinform_zocl::Init()
{
  dwSerial = static_cast<unsigned int>(-1);
  byContCount = 0;
  for (int j = 0; j < 8; ++j)
  {
    m_MonContSf[j].wSfcode = static_cast<unsigned __int16>(-1);
  }
}

_target_player_damage_contsf_allinform_zocl::_target_player_damage_contsf_allinform_zocl()
{
  Init();
}

void _target_player_damage_contsf_allinform_zocl::Init()
{
  dwSerial = static_cast<unsigned int>(-1);
  byContCount = 0;
  for (int j = 0; j < 8; ++j)
  {
    m_PlayerContSf[j].wSfcode = static_cast<unsigned __int16>(-1);
    m_PlayerContSf[j].byContCount = 0;
  }
}

bool _target_monster_contsf_allinform_zocl::IsSame(
  _target_monster_contsf_allinform_zocl *src1,
  _target_monster_contsf_allinform_zocl *src2)
{
  if (src1->dwSerial != src2->dwSerial)
  {
    return false;
  }
  for (int j = 0; j < 8; ++j)
  {
    if (src1->m_MonContSf[j].wSfcode != src2->m_MonContSf[j].wSfcode)
    {
      return false;
    }
  }
  return true;
}

unsigned __int16 _target_monster_contsf_allinform_zocl::size()
{
  if (byContCount > 8u)
  {
    byContCount = 0;
  }
  return static_cast<unsigned __int16>(21 - 2 * (8 - byContCount));
}

bool _target_player_damage_contsf_allinform_zocl::IsSame(
  _target_player_damage_contsf_allinform_zocl *src1,
  _target_player_damage_contsf_allinform_zocl *src2)
{
  if (src1->dwSerial != src2->dwSerial)
  {
    return false;
  }
  for (int j = 0; j < 8; ++j)
  {
    if (src1->m_PlayerContSf[j].wSfcode != src2->m_PlayerContSf[j].wSfcode
        || src1->m_PlayerContSf[j].byContCount != src2->m_PlayerContSf[j].byContCount)
    {
      return false;
    }
  }
  return true;
}

unsigned __int16 _target_player_damage_contsf_allinform_zocl::size()
{
  if (byContCount > 8u)
  {
    byContCount = 0;
  }
  return static_cast<unsigned __int16>(29 - 3 * (8 - byContCount));
}

CPlayer::__target::__target()
{
  init();
}

void CPlayer::__target::init()
{
  pObject = nullptr;
  m_PrevTargetMonsterContInfo.Init();
  m_PrevTargetPlayerDamageContInfo.Init();
}

void CPlayer::SendMsg_TLStatusPenalty(char byErrCode)
{
  char szMsg[32];
  unsigned __int8 pbyType[36];

  szMsg[0] = byErrCode;
  pbyType[0] = 13;
  pbyType[1] = static_cast<unsigned __int8>(-109);
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, szMsg, 1u);
}

void CPlayer::SendMsg_TLStatusInfo(unsigned int /*dwFatigue*/, unsigned __int8 /*wStatus*/)
{
  #pragma pack(push, 1)
  struct TLStatusInfoMsg
  {
    unsigned int dwFatigue;
    unsigned __int16 byTLStatus;
    unsigned int dwRemainTime;
    unsigned int dwPlayValue;
  };
  #pragma pack(pop)

  TLStatusInfoMsg msg{};
  msg.dwFatigue = this->m_pUserDB->m_AvatorData.dbTimeLimitInfo.dwFatigue;
  msg.byTLStatus = this->m_pUserDB->m_AvatorData.dbTimeLimitInfo.byTLStatus;

  const unsigned int playDegree = TimeLimitMgr::GetPlayFDegree(g_Main.m_pTimeLimitMgr);
  msg.dwPlayValue = (playDegree / 1000) * msg.dwFatigue;

  const unsigned __int16 endPlayTime = TimeLimitMgr::GetEndPlayTime(g_Main.m_pTimeLimitMgr);
  if (msg.dwPlayValue < 60u * endPlayTime)
  {
    msg.dwRemainTime = 60u * endPlayTime - msg.dwPlayValue;
  }
  else
  {
    msg.dwRemainTime = 0;
  }

  unsigned __int8 pbyType[2] = {13, static_cast<unsigned __int8>(-113)};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    0xEu);
}

CGameObject *CPlayer::GetTargetObj()
{
  if (!m_TargetObject.pObject)
  {
    return nullptr;
  }

  CGameObject *object = m_TargetObject.pObject;
  if (object->m_bLive && m_TargetObject.byKind == object->m_ObjID.m_byKind
      && m_TargetObject.byID == object->m_ObjID.m_byID && m_TargetObject.dwSerial == object->m_dwObjSerial
      && m_TargetObject.pObject->m_pCurMap == m_pCurMap)
  {
    return m_TargetObject.pObject;
  }

  m_TargetObject.init();
  return nullptr;
}

bool CPlayer::IsChaosMode()
{
  return m_nChaosMode != 0;
}

CAnimus *CPlayer::GetRecallAnimus()
{
  return m_pRecalledAnimusChar;
}

void CPlayer::SendMsg_MonsterAggroData(CCharacter *pCharacter)
{
  if (!pCharacter)
  {
    return;
  }

  CMonster *monster = static_cast<CMonster *>(pCharacter);
  _target_monster_aggro_inform_zocl info;

  info.dwSerial = monster->m_dwObjSerial;
  info.fTimer = static_cast<float>(static_cast<int>(GetLoopTime()))
                - static_cast<float>(static_cast<int>(monster->m_AggroMgr.m_dwShortRankLastTime));

  for (int j = 0; j < 10; ++j)
  {
    _target_monster_aggro_inform_zocl::_aggro_node *nodeInfo = &info.m_AggroNode[j];
    CAggroNode *aggroNode = &monster->m_AggroMgr.m_AggroPool[j];
    if (aggroNode->IsLive())
    {
      CCharacter *aggroCharacter = aggroNode->m_pCharacter;
      if (aggroCharacter && !aggroCharacter->m_ObjID.m_byKind)
      {
        if (aggroCharacter->m_ObjID.m_byID)
        {
          if (aggroCharacter->m_ObjID.m_byID == 3)
          {
            CAnimus *animus = static_cast<CAnimus *>(aggroCharacter);
            sprintf(nodeInfo->m_Name, "[AniMaster:%s]", animus->m_aszMasterName);
          }
        }
        else
        {
          CPlayer *player = static_cast<CPlayer *>(aggroCharacter);
          const char *name = player->m_Param.GetCharNameA();
          sprintf(nodeInfo->m_Name, "%s", name);
        }
      }
      nodeInfo->m_IsData = 1;
      nodeInfo->m_nAggroData = aggroNode->m_nAggroData;
      nodeInfo->m_nDamageData = aggroNode->m_nDamageData;
      nodeInfo->m_nKingPowerDamage = aggroNode->m_nKingPowerDamage;
    }
  }

  unsigned __int8 pbyType[28];
  pbyType[0] = 13;
  pbyType[1] = 99;
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, (char *)&info, 0x1CAu);
}

void CPlayer::SetGrade(unsigned __int8 byGrade)
{
  if (byGrade != m_Param.m_byPvPGrade)
  {
    m_Param.m_byPvPGrade = byGrade;
    m_pUserDB->m_AvatorData.m_byPvpGrade = byGrade;
    SendMsg_AlterGradeInform();
  }
}

void CPlayer::SetRankRate(unsigned __int16 wRankRate, unsigned int dwRank)
{
  if (m_Param.m_dbChar.m_wRankRate != wRankRate || m_Param.m_dbChar.m_dwRank != dwRank)
  {
    m_Param.m_dbChar.m_wRankRate = wRankRate;
    m_Param.m_dbChar.m_dwRank = dwRank;
    SendMsg_AlterPvPRank(wRankRate, dwRank);
  }
}

void CPlayer::SendMsg_AlterGradeInform()
{
  char msg[1]{};
  msg[0] = static_cast<char>(m_Param.m_byPvPGrade);

  unsigned __int8 pbyType[2] = {17, 17};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, msg, 1u);
}

void CPlayer::SendMsg_AlterPvPRank(unsigned __int16 wPvpRate, unsigned int dwPvpRank)
{
  #pragma pack(push, 1)
  struct
  {
    unsigned __int16 wPvpRate;
    unsigned int dwPvpRank;
  } msg{};
  #pragma pack(pop)

  msg.wPvpRate = wPvpRate;
  msg.dwPvpRank = dwPvpRank;

  unsigned __int8 pbyType[2] = {17, 18};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), 6u);
}

void CPlayer::SendMsg_PvpRankListVersionUp(char byVersion)
{
  char msg[1]{};
  msg[0] = byVersion;

  unsigned __int8 pbyType[2] = {13, 24};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, msg, 1u);
}

void CPlayer::SendData_ChatTrans(
  unsigned __int8 byChatType,
  unsigned int dwSenderSerial,
  unsigned __int8 byRaceCode,
  bool bFilter,
  char *pwszMessage,
  unsigned __int8 byPvpGrade,
  char *pwszSender)
{
  _chat_message_receipt_udp msg{};
  msg.byMessageType = byChatType;
  msg.dwSenderSerial = dwSenderSerial;
  if (pwszSender)
  {
    strcpy_s(msg.wszSenderName, 0x11u, pwszSender);
  }
  msg.byPvpGrade = byPvpGrade;
  msg.byRaceCode = byRaceCode;
  msg.bFiltering = bFilter;
  if (bFilter)
  {
    msg.wszChatData[0] = 0;
    msg.bySize = 0;
  }
  else
  {
    msg.bySize = static_cast<unsigned __int8>(strlen_0(pwszMessage));
    memcpy_0(msg.wszChatData, pwszMessage, msg.bySize);
    msg.wszChatData[msg.bySize] = 0;
  }

  unsigned __int8 pbyType[2] = {2, 10};
  const unsigned __int16 nLen = msg.size();
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), nLen);
}

_STAT_DB_BASE::_STAT_DB_BASE()
{
  Init();
}

void _STAT_DB_BASE::Init()
{
  memset_0(this, 0, sizeof(_STAT_DB_BASE));
}

int _STAT_DB_BASE::GetStatIndex(unsigned __int8 byMasteryClass, unsigned __int8 byIndex)
{
  int statIndex = -1;
  switch (byMasteryClass)
  {
    case 0u:
      statIndex = static_cast<int>(byIndex);
      break;
    case 1u:
      statIndex = 2;
      break;
    case 2u:
      statIndex = 3;
      break;
    case 3u:
      statIndex = static_cast<int>(byIndex) + 4;
      break;
    case 4u:
      statIndex = static_cast<int>(byIndex) + 52;
      break;
    case 5u:
      statIndex = static_cast<int>(byIndex) + 76;
      break;
    case 6u:
      statIndex = 79;
      break;
    default:
      break;
  }
  return statIndex;
}

bool _STAT_DB_BASE::IsRangePerMastery(unsigned __int8 byMasteryClass, unsigned __int8 byIndex)
{
  switch (byMasteryClass)
  {
    case 0u:
      return byIndex < 2u;
    case 1u:
      return byIndex == 0;
    case 2u:
      return byIndex == 0;
    case 3u:
      return byIndex < 0x30u;
    case 4u:
      return byIndex < 0x18u;
    case 5u:
      return byIndex < 3u;
    case 6u:
      return byIndex == 0;
    default:
      break;
  }
  return false;
}

_other_shape_all_zocl::_other_shape_all_zocl()
{
  bySpecialPart = static_cast<unsigned __int8>(-1);
}

unsigned __int64 _other_shape_all_zocl::size()
{
  return bySpecialPart == 0xFFu ? 62u : 68u;
}

_other_shape_part_zocl::_other_shape_part_zocl()
{
  bySpecialPart = static_cast<unsigned __int8>(-1);
}

unsigned __int64 _other_shape_part_zocl::size()
{
  return bySpecialPart == 0xFFu ? 36u : 42u;
}

unsigned __int64 _base_download_result_zocl::size()
{
  return byRetCode ? 1u : 258u;
}

_inven_download_result_zocl::_inven_download_result_zocl()
{
  bySlotNum = 0;
}

unsigned __int64 _inven_download_result_zocl::size()
{
  if (byRetCode)
  {
    return 1u;
  }
  if (bySlotNum > 0x64u)
  {
    bySlotNum = 0;
  }
  return static_cast<unsigned __int64>(2103 - 21LL * (100 - bySlotNum));
}

_force_download_result_zocl::_force_download_result_zocl()
{
  bySlotNum = 0;
}

unsigned __int64 _force_download_result_zocl::size()
{
  if (byRetCode)
  {
    return 1u;
  }
  if (bySlotNum > 0x58u)
  {
    bySlotNum = 0;
  }
  return static_cast<unsigned __int64>(970 - 11LL * (88 - bySlotNum));
}

_cum_download_result_zocl::_cum_download_result_zocl()
{
  Stat.Init();
  byLeftCuttingResNum = 0;
}

unsigned __int64 _cum_download_result_zocl::size()
{
  if (byRetCode)
  {
    return 1u;
  }
  if (byLeftCuttingResNum > 0x14u)
  {
    byLeftCuttingResNum = 0;
  }
  return static_cast<unsigned __int64>(362 - 2LL * (20 - byLeftCuttingResNum));
}

_quest_download_result_zocl::__list::__list()
{
  cont.Init();
}

_quest_download_result_zocl::_quest_download_result_zocl()
{
  bySlotNum = 0;
}

unsigned __int64 _quest_download_result_zocl::size()
{
  if (byRetCode)
  {
    return 1u;
  }
  if (bySlotNum > 0x1Eu)
  {
    bySlotNum = 0;
  }
  return static_cast<unsigned __int64>(422 - 14LL * (30 - bySlotNum));
}

_quest_history_download_result_zocl::_quest_history_download_result_zocl()
{
  bySlotNum = 0;
}

unsigned __int64 _quest_history_download_result_zocl::size()
{
  if (bySlotNum > 0x46u)
  {
    bySlotNum = 0;
  }
  return static_cast<unsigned __int64>(631 - 9LL * (70 - bySlotNum));
}

_unit_download_result_zocl::_list::_list()
{
  UnitData.Init(0xFFu);
}

_unit_download_result_zocl::_unit_download_result_zocl()
{
  byUnitNum = 0;
}

unsigned __int64 _unit_download_result_zocl::size()
{
  if (byUnitNum > 4u)
  {
    byUnitNum = 0;
  }
  return static_cast<unsigned __int64>(253 - 63LL * (4 - byUnitNum));
}

_animus_download_result_zocl::_animus_download_result_zocl()
{
  byAnimusNum = 0;
}

unsigned __int64 _animus_download_result_zocl::size()
{
  if (byAnimusNum > 4u)
  {
    byAnimusNum = 0;
  }
  return static_cast<unsigned __int64>(73 - 18LL * (4 - byAnimusNum));
}

unsigned __int64 _personal_automine_download_zocl::size()
{
  return static_cast<unsigned __int64>(202 - 5LL * (40 - byCnt));
}

_Init_action_point_zocl::_Init_action_point_zocl()
{
  memset_0(this, 0, sizeof(_Init_action_point_zocl));
}

void CPlayer::SendMsg_ChangeClassCommand()
{
  char szMsg[32];
  *reinterpret_cast<unsigned __int16 *>(szMsg) = this->m_Param.m_pClassData->m_dwIndex;

  unsigned __int8 pbyType[2] = {11, 5};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], this->m_ObjID.m_wIndex, pbyType, szMsg, 2u);
}

void CPlayer::SendMsg_SelectClassResult(char byErrCode, unsigned __int16 wSelClassIndex)
{
  char szMsg[32]{};
  szMsg[0] = byErrCode;

  unsigned __int8 type[2]{11, 7};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, szMsg, 1u);

  if (!byErrCode)
  {
    char circleMsg[6]{};
    *reinterpret_cast<unsigned int *>(circleMsg) = m_dwObjSerial;
    *reinterpret_cast<unsigned __int16 *>(circleMsg + 4) = wSelClassIndex;
    unsigned __int8 circleType[2]{17, 8};
    CGameObject::CircleReport(this, circleType, circleMsg, 6, 0);
  }
}

void CPlayer::SendMsg_PcRoomCharClass(unsigned int dwPcRoomClassIndex)
{
  char szMsg[32]{};
  *reinterpret_cast<unsigned int *>(szMsg) = dwPcRoomClassIndex;

  unsigned __int8 type[2]{11, 32};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, szMsg, 4u);
}

unsigned int CPlayer::GetInitClassCost()
{
  const unsigned int classInitCount = m_pUserDB->m_AvatorData.dbAvator.m_dwClassInitCnt;
  const unsigned int costs[5] = {1000, 10000, 100000, 1000000, 1000000};
  if (classInitCount > 4)
  {
    return 1000000;
  }
  return costs[classInitCount];
}

unsigned __int8 CPlayer::pc_InitClassRequest()
{
  const unsigned int cost = GetInitClassCost();
  if (cost > CPlayerDB::GetGold(&m_Param))
  {
    return 3;
  }

  const unsigned __int8 result = pc_InitClass();
  if (!result)
  {
    SubGold(cost);
  }
  ++m_pUserDB->m_AvatorData.dbAvator.m_dwClassInitCnt;
  return result;
}

unsigned __int8 CPlayer::pc_InitClass()
{
  if (!m_pUserDB)
  {
    return 1;
  }
  if (!m_Param.m_pClassHistory[0])
  {
    return 2;
  }
  if (m_pUsingUnit || m_pParkingUnit)
  {
    return 4;
  }
  if (_STORAGE_LIST::GetNumUseCon(&m_Param.m_dbAnimus) > 0)
  {
    return 5;
  }
  if (m_bAfterEffect)
  {
    return 11;
  }

  char prevClass[36]{};
  memcpy_0(prevClass, m_pUserDB->m_AvatorData.dbAvator.m_szClassCode, 5u);
  int oldMaxPoint[8]{};
  oldMaxPoint[0] = m_nMaxPoint[0];
  oldMaxPoint[1] = m_nMaxPoint[1];
  oldMaxPoint[2] = m_nMaxPoint[2];
  memset(&oldMaxPoint[3], 0, sizeof(int));

  CPlayerDB::InitClass(&m_Param);
  CalcAddPointByClass();
  m_nAddDfnMstByClass = 0;
  RewardChangeClassMastery(m_Param.m_pClassData);
  CUserDB::InitClass(m_pUserDB, m_Param.m_pClassData->m_strCode);
  ReCalcMaxHFSP(false, false);
  _TowerAllReturn(3u, true);

  for (int j = 0; j < 20; ++j)
  {
    _TRAP_PARAM::_param *trap = &m_pmTrp.m_Item[j];
    if (_TRAP_PARAM::_param::isLoad(trap) && trap->pItem->m_dwObjSerial == trap->dwSerial)
    {
      CTrap::Destroy(trap->pItem, 4u);
    }
  }

  m_fUnitPv_AttFc = FLOAT_1_0;
  m_fUnitPv_DefFc = FLOAT_1_0;
  m_fUnitPv_RepPr = FLOAT_1_0;
  CCharacter::RemoveAllContinousEffect(this);
  SendMsg_AlterMoneyInform(0);

  CMgrAvatorItemHistory::InitClass(
    &CPlayer::s_MgrItemHistory,
    0,
    m_pUserDB->m_AvatorData.dbAvator.m_dwClassInitCnt,
    m_pUserDB->m_AvatorData.dbAvator.m_byLastClassGrade,
    prevClass,
    m_pUserDB->m_AvatorData.dbAvator.m_szClassCode,
    oldMaxPoint,
    m_nMaxPoint,
    m_szItemHistoryFileName);
  CUserDB::WriteLog_ChangeClassAfterInitClass(m_pUserDB, 0, prevClass);
  m_bDownCheckEquipEffect = 1;
  return 0;
}

void CPlayer::CalcAddPointByClass()
{
  memset_0(m_nAddPointByClass, 0, sizeof(m_nAddPointByClass));
  m_nAddPointByClass[0] += m_Param.m_pClassData->m_nBnsForHP;
  m_nAddPointByClass[1] += m_Param.m_pClassData->m_nBnsForFP;
  m_nAddPointByClass[2] += m_Param.m_pClassData->m_nBnsForSP;
  for (int j = 0; j < 3 && m_Param.m_pClassHistory[j]; ++j)
  {
    m_nAddPointByClass[0] += m_Param.m_pClassHistory[j]->m_nBnsForHP;
    m_nAddPointByClass[1] += m_Param.m_pClassHistory[j]->m_nBnsForFP;
    m_nAddPointByClass[2] += m_Param.m_pClassHistory[j]->m_nBnsForSP;
  }
}

void CPlayer::RewardChangeClassMastery(_class_fld *pClassFld)
{
  CPlayer::s_MgrLvHistory.update_mastery(
    m_ObjID.m_wIndex,
    m_byUserDgr,
    CPlayerDB::GetLevel(&m_Param),
    CPlayerDB::GetExp(&m_Param),
    m_dwExpRate,
    m_Param.m_byPvPGrade,
    m_nMaxPoint,
    &m_pmMst,
    m_Param.m_dwAlterMastery,
    m_szLvHistoryFileName,
    0,
    nullptr);
  CPlayerDB::InitAlterMastery(&m_Param);

  for (int j = 0; j < 2; ++j)
  {
    if (pClassFld->m_MasteryLim.m_nBnsMMastery[j])
    {
      Emb_AlterStat(
        0,
        j,
        pClassFld->m_MasteryLim.m_nBnsMMastery[j],
        1u,
        "Player::RewardChangeClassMastery()---0",
        false);
    }
  }
  if (pClassFld->m_MasteryLim.m_nBnsSMastery)
  {
    Emb_AlterStat(
      6u,
      0,
      pClassFld->m_MasteryLim.m_nBnsSMastery,
      1u,
      "Player::RewardChangeClassMastery()---1",
      false);
  }
  if (pClassFld->m_MasteryLim.m_nBnsDefMastery)
  {
    Emb_AlterStat(
      1u,
      0,
      pClassFld->m_MasteryLim.m_nBnsDefMastery,
      1u,
      "Player::RewardChangeClassMastery()---2",
      false);
  }
  if (pClassFld->m_MasteryLim.m_nBnsPryMastery)
  {
    Emb_AlterStat(
      2u,
      0,
      pClassFld->m_MasteryLim.m_nBnsPryMastery,
      1u,
      "Player::RewardChangeClassMastery()---3",
      false);
  }
  for (int j = 0; j < 3; ++j)
  {
    if (pClassFld->m_MasteryLim.m_nBnsMakeMastery[j])
    {
      Emb_AlterStat(
        5u,
        j,
        pClassFld->m_MasteryLim.m_nBnsMakeMastery[j],
        1u,
        "Player::RewardChangeClassMastery()---4",
        false);
    }
  }
  for (int j = 0; j < 24; ++j)
  {
    if (pClassFld->m_MasteryLim.m_nBnsForceMastery[j])
    {
      Emb_AlterStat(
        4u,
        j,
        pClassFld->m_MasteryLim.m_nBnsForceMastery[j],
        1u,
        "Player::RewardChangeClassMastery()---5",
        false);
    }
  }
  for (int j = 0; j < 8; ++j)
  {
    if (pClassFld->m_MasteryLim.m_nBnsSkillMastery[j])
    {
      int totalSkill = 0;
      for (int k = 0; k < CPlayer::s_SkillIndexPerMastery[j].m_nSkillIndexNum; ++k)
      {
        const int skillIndex = CPlayer::s_SkillIndexPerMastery[j].m_nSkillIndex[k];
        totalSkill += m_pmMst.m_BaseCum.m_dwSkillCum[skillIndex];
      }
      if (totalSkill)
      {
        for (int k = 0; k < CPlayer::s_SkillIndexPerMastery[j].m_nSkillIndexNum; ++k)
        {
          const int skillIndex = CPlayer::s_SkillIndexPerMastery[j].m_nSkillIndex[k];
          if (m_pmMst.m_BaseCum.m_dwSkillCum[skillIndex])
          {
            const unsigned int alterValue = static_cast<unsigned int>(
              static_cast<int>(
                static_cast<float>(m_pmMst.m_BaseCum.m_dwSkillCum[skillIndex]) / static_cast<float>(totalSkill)
                * static_cast<float>(pClassFld->m_MasteryLim.m_nBnsSkillMastery[j])
                + 1.0f));
            if (static_cast<int>(alterValue) > 0)
            {
              Emb_AlterStat(3u, skillIndex, alterValue, 1u, "Player::RewardChangeClassMastery()---7", false);
            }
          }
        }
      }
      else
      {
        unsigned int alterValue =
          pClassFld->m_MasteryLim.m_nBnsSkillMastery[j] / CPlayer::s_SkillIndexPerMastery[j].m_nSkillIndexNum;
        if (pClassFld->m_MasteryLim.m_nBnsSkillMastery[j]
            % CPlayer::s_SkillIndexPerMastery[j].m_nSkillIndexNum)
        {
          ++alterValue;
        }
        for (int k = 0; k < CPlayer::s_SkillIndexPerMastery[j].m_nSkillIndexNum; ++k)
        {
          const int skillIndex = CPlayer::s_SkillIndexPerMastery[j].m_nSkillIndex[k];
          if (static_cast<int>(alterValue) > 0)
          {
            Emb_AlterStat(3u, skillIndex, alterValue, 1u, "Player::RewardChangeClassMastery()---6", false);
          }
        }
      }
    }
  }

  CPlayer::s_MgrLvHistory.update_mastery(
    m_ObjID.m_wIndex,
    m_byUserDgr,
    CPlayerDB::GetLevel(&m_Param),
    CPlayerDB::GetExp(&m_Param),
    m_dwExpRate,
    m_Param.m_byPvPGrade,
    m_nMaxPoint,
    &m_pmMst,
    m_Param.m_dwAlterMastery,
    m_szLvHistoryFileName,
    3u,
    pClassFld->m_strCode);
  CPlayerDB::InitAlterMastery(&m_Param);
  m_dwUMWHLastTime = GetLoopTime();
}

void CPlayer::RewardChangeClassRewardItem(_class_fld *pClassFld, unsigned __int8 bySelectRewardItem)
{
  for (int j = 0; j < 9; ++j)
  {
    if (bySelectRewardItem == 0xFF || bySelectRewardItem == j)
    {
      char *entry = pClassFld->m_DefaultItem[j].strDefaultItem;
      if (std::strncmp(entry, "-1", 2) != 0)
      {
        _STORAGE_LIST::_db_con rewardItem{};
        const int tableCode = GetItemTableCode(entry);
        if (tableCode == -1)
        {
          CLogFile::Write(
            &g_Main.m_logSystemError,
            "error bonus item of class change .., class: %s, error table code",
            pClassFld->m_strCode);
          continue;
        }
        if (tableCode == 19)
        {
          CLogFile::Write(
            &g_Main.m_logSystemError,
            "error bonus item of class change .., class: %s, unit key",
            pClassFld->m_strCode);
          continue;
        }
        _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblItemData[tableCode], entry);
        if (!record)
        {
          CLogFile::Write(
            &g_Main.m_logSystemError,
            "error bonus item of class change .., class: %s, nothing in table",
            pClassFld->m_strCode);
          continue;
        }

        unsigned __int64 itemExp = *reinterpret_cast<unsigned int *>(entry + 64);
        const unsigned __int8 itemKind = GetItemKindCode(tableCode);
        if (itemKind)
        {
          if (itemKind != 1)
          {
            continue;
          }
          itemExp = (itemExp & 0xFFFFFFFFu)
            | (static_cast<unsigned __int64>(GetMaxParamFromExp(record->m_dwIndex, static_cast<unsigned int>(itemExp))) << 32);
        }
        else
        {
          const unsigned __int8 defSocketNum = GetDefItemUpgSocketNum(tableCode, record->m_dwIndex);
          itemExp = (itemExp & 0xFFFFFFFFu)
            | (static_cast<unsigned __int64>(GetBitAfterSetLimSocket(defSocketNum)) << 32);
        }

        rewardItem.m_byTableCode = static_cast<unsigned __int8>(tableCode);
        rewardItem.m_wItemIndex = record->m_dwIndex;
        rewardItem.m_dwDur = static_cast<unsigned int>(itemExp);
        rewardItem.m_dwLv = static_cast<unsigned int>(itemExp >> 32);

        if (_STORAGE_LIST::GetIndexEmptyCon(&m_Param.m_dbInven) == 0xFF)
        {
          CreateItemBox(
            &rewardItem,
            this,
            0xFFFFFFFF,
            0,
            nullptr,
            3u,
            m_pCurMap,
            m_wMapLayerIndex,
            m_fCurPos,
            0);
          char clause[144]{};
          sprintf(clause, "Class G (%s)", pClassFld->m_strCode);
          CMgrAvatorItemHistory::reward_add_item(
            &CPlayer::s_MgrItemHistory,
            m_ObjID.m_wIndex,
            clause,
            &rewardItem,
            m_szItemHistoryFileName);
        }
        else
        {
          rewardItem.m_wSerial = CPlayerDB::GetNewItemSerial(&m_Param);
          if (Emb_AddStorage(0, &rewardItem, false, true))
          {
            SendMsg_RewardAddItem(&rewardItem, 1u);
            char clause[160]{};
            sprintf(clause, "Class (%s)", pClassFld->m_strCode);
            CMgrAvatorItemHistory::reward_add_item(
              &CPlayer::s_MgrItemHistory,
              m_ObjID.m_wIndex,
              clause,
              &rewardItem,
              m_szItemHistoryFileName);
          }
        }
      }
    }
  }
}

void CPlayer::RewardChangeClass(_class_fld *pClassFld, unsigned __int8 bySelectRewardItem)
{
  CPlayer::s_MgrLvHistory.update_mastery(
    m_ObjID.m_wIndex,
    m_byUserDgr,
    CPlayerDB::GetLevel(&m_Param),
    CPlayerDB::GetExp(&m_Param),
    m_dwExpRate,
    m_Param.m_byPvPGrade,
    m_nMaxPoint,
    &m_pmMst,
    m_Param.m_dwAlterMastery,
    m_szLvHistoryFileName,
    0,
    nullptr);
  CPlayerDB::InitAlterMastery(&m_Param);

  for (int j = 0; j < 2; ++j)
  {
    if (pClassFld->m_MasteryLim.m_nBnsMMastery[j])
    {
      Emb_AlterStat(
        0,
        j,
        pClassFld->m_MasteryLim.m_nBnsMMastery[j],
        1u,
        "CPlayer::RewardChangeClass()---0",
        true);
    }
  }
  if (pClassFld->m_MasteryLim.m_nBnsSMastery)
  {
    Emb_AlterStat(
      6u,
      0,
      pClassFld->m_MasteryLim.m_nBnsSMastery,
      1u,
      "CPlayer::RewardChangeClass()---1",
      true);
  }
  if (pClassFld->m_MasteryLim.m_nBnsDefMastery)
  {
    Emb_AlterStat(
      1u,
      0,
      pClassFld->m_MasteryLim.m_nBnsDefMastery,
      1u,
      "CPlayer::RewardChangeClass()---2",
      true);
  }
  if (pClassFld->m_MasteryLim.m_nBnsPryMastery)
  {
    Emb_AlterStat(
      2u,
      0,
      pClassFld->m_MasteryLim.m_nBnsPryMastery,
      1u,
      "CPlayer::RewardChangeClass()---3",
      true);
  }
  for (int j = 0; j < 3; ++j)
  {
    if (pClassFld->m_MasteryLim.m_nBnsMakeMastery[j])
    {
      Emb_AlterStat(
        5u,
        j,
        pClassFld->m_MasteryLim.m_nBnsMakeMastery[j],
        1u,
        "CPlayer::RewardChangeClass()---4",
        true);
    }
  }
  for (int j = 0; j < 24; ++j)
  {
    if (pClassFld->m_MasteryLim.m_nBnsForceMastery[j])
    {
      Emb_AlterStat(
        4u,
        j,
        pClassFld->m_MasteryLim.m_nBnsForceMastery[j],
        1u,
        "CPlayer::RewardChangeClass()---5",
        true);
    }
  }
  for (int j = 0; j < 8; ++j)
  {
    if (pClassFld->m_MasteryLim.m_nBnsSkillMastery[j])
    {
      int totalSkill = 0;
      for (int k = 0; k < CPlayer::s_SkillIndexPerMastery[j].m_nSkillIndexNum; ++k)
      {
        const int skillIndex = CPlayer::s_SkillIndexPerMastery[j].m_nSkillIndex[k];
        totalSkill += m_pmMst.m_BaseCum.m_dwSkillCum[skillIndex];
      }
      if (totalSkill)
      {
        for (int k = 0; k < CPlayer::s_SkillIndexPerMastery[j].m_nSkillIndexNum; ++k)
        {
          const int skillIndex = CPlayer::s_SkillIndexPerMastery[j].m_nSkillIndex[k];
          if (m_pmMst.m_BaseCum.m_dwSkillCum[skillIndex])
          {
            const unsigned int alterValue = static_cast<unsigned int>(
              static_cast<int>(
                static_cast<float>(m_pmMst.m_BaseCum.m_dwSkillCum[skillIndex]) / static_cast<float>(totalSkill)
                * static_cast<float>(pClassFld->m_MasteryLim.m_nBnsSkillMastery[j])
                + 1.0f));
            if (static_cast<int>(alterValue) > 0)
            {
              Emb_AlterStat(3u, skillIndex, alterValue, 1u, "CPlayer::RewardChangeClass()---7", true);
            }
          }
        }
      }
      else
      {
        unsigned int alterValue =
          pClassFld->m_MasteryLim.m_nBnsSkillMastery[j] / CPlayer::s_SkillIndexPerMastery[j].m_nSkillIndexNum;
        if (pClassFld->m_MasteryLim.m_nBnsSkillMastery[j]
            % CPlayer::s_SkillIndexPerMastery[j].m_nSkillIndexNum)
        {
          ++alterValue;
        }
        for (int k = 0; k < CPlayer::s_SkillIndexPerMastery[j].m_nSkillIndexNum; ++k)
        {
          const int skillIndex = CPlayer::s_SkillIndexPerMastery[j].m_nSkillIndex[k];
          if (static_cast<int>(alterValue) > 0)
          {
            Emb_AlterStat(3u, skillIndex, alterValue, 1u, "CPlayer::RewardChangeClass()---6", true);
          }
        }
      }
    }
  }

  for (int j = 0; j < 9; ++j)
  {
    if (bySelectRewardItem == 0xFF || bySelectRewardItem == j)
    {
      char *entry = pClassFld->m_DefaultItem[j].strDefaultItem;
      if (std::strncmp(entry, "-1", 2) != 0)
      {
        _STORAGE_LIST::_db_con rewardItem{};
        const int tableCode = GetItemTableCode(entry);
        if (tableCode == -1)
        {
          CLogFile::Write(
            &g_Main.m_logSystemError,
            "error bonus item of class change .., class: %s, error table code",
            pClassFld->m_strCode);
          continue;
        }
        if (tableCode == 19)
        {
          CLogFile::Write(
            &g_Main.m_logSystemError,
            "error bonus item of class change .., class: %s, unit key",
            pClassFld->m_strCode);
          continue;
        }
        _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblItemData[tableCode], entry);
        if (!record)
        {
          CLogFile::Write(
            &g_Main.m_logSystemError,
            "error bonus item of class change .., class: %s, nothing in table",
            pClassFld->m_strCode);
          continue;
        }

        unsigned __int64 itemExp = *reinterpret_cast<unsigned int *>(entry + 64);
        const unsigned __int8 itemKind = GetItemKindCode(tableCode);
        if (itemKind)
        {
          if (itemKind != 1)
          {
            continue;
          }
          itemExp = (itemExp & 0xFFFFFFFFu)
            | (static_cast<unsigned __int64>(GetMaxParamFromExp(record->m_dwIndex, static_cast<unsigned int>(itemExp))) << 32);
        }
        else
        {
          const unsigned __int8 defSocketNum = GetDefItemUpgSocketNum(tableCode, record->m_dwIndex);
          itemExp = (itemExp & 0xFFFFFFFFu)
            | (static_cast<unsigned __int64>(GetBitAfterSetLimSocket(defSocketNum)) << 32);
        }

        rewardItem.m_byTableCode = static_cast<unsigned __int8>(tableCode);
        rewardItem.m_wItemIndex = record->m_dwIndex;
        rewardItem.m_dwDur = static_cast<unsigned int>(itemExp);
        rewardItem.m_dwLv = static_cast<unsigned int>(itemExp >> 32);

        if (_STORAGE_LIST::GetIndexEmptyCon(&m_Param.m_dbInven) == 0xFF)
        {
          CreateItemBox(
            &rewardItem,
            this,
            0xFFFFFFFF,
            0,
            nullptr,
            3u,
            m_pCurMap,
            m_wMapLayerIndex,
            m_fCurPos,
            0);
          char clause[144]{};
          sprintf(clause, "Class G (%s)", pClassFld->m_strCode);
          CMgrAvatorItemHistory::reward_add_item(
            &CPlayer::s_MgrItemHistory,
            m_ObjID.m_wIndex,
            clause,
            &rewardItem,
            m_szItemHistoryFileName);
        }
        else
        {
          rewardItem.m_wSerial = CPlayerDB::GetNewItemSerial(&m_Param);
          if (Emb_AddStorage(0, &rewardItem, false, true))
          {
            SendMsg_RewardAddItem(&rewardItem, 1u);
            char clause[160]{};
            sprintf(clause, "Class (%s)", pClassFld->m_strCode);
            CMgrAvatorItemHistory::reward_add_item(
              &CPlayer::s_MgrItemHistory,
              m_ObjID.m_wIndex,
              clause,
              &rewardItem,
              m_szItemHistoryFileName);
          }
          else
          {
            CMgrAvatorItemHistory::add_storage_fail(
              &CPlayer::s_MgrItemHistory,
              m_ObjID.m_wIndex,
              &rewardItem,
              "RewardChangeClass - Emb_AddStorage() Fail",
              m_szItemHistoryFileName);
          }
        }
      }
    }
  }

  CPlayer::s_MgrLvHistory.update_mastery(
    m_ObjID.m_wIndex,
    m_byUserDgr,
    CPlayerDB::GetLevel(&m_Param),
    CPlayerDB::GetExp(&m_Param),
    m_dwExpRate,
    m_Param.m_byPvPGrade,
    m_nMaxPoint,
    &m_pmMst,
    m_Param.m_dwAlterMastery,
    m_szLvHistoryFileName,
    3u,
    pClassFld->m_strCode);
  CPlayerDB::InitAlterMastery(&m_Param);
  m_dwUMWHLastTime = GetLoopTime();
}

int GetRewardItemNumChangeClass(_class_fld *pClassFld)
{
  unsigned int count = 0;
  for (int j = 0; j < 9; ++j)
  {
    char *entry = pClassFld->m_DefaultItem[j].strDefaultItem;
    if (std::strncmp(entry, "-1", 2) != 0)
    {
      ++count;
    }
  }
  return static_cast<int>(count);
}

void CPlayer::pc_SelectClassRequest(unsigned __int16 wSelClassIndex, unsigned __int8 bySelectRewardItem)
{
  unsigned __int8 errCode = 0;
  _class_fld *currentClass = m_Param.m_pClassData;
  _class_fld *classRecord = (_class_fld *)CRecordData::GetRecord(&g_Main.m_tblClass, wSelClassIndex);
  unsigned __int8 classSlot = 0xFF;
  unsigned __int8 changeSlot = 0xFF;
  bool rewardClassUp = false;

  if (m_pUserDB)
  {
    if (CPlayerDB::IsClassChangeableLv(&m_Param))
    {
      if (classRecord)
      {
        for (int j = 0; j < 8 && std::strncmp(currentClass->m_strCh_Class[j], "-1", 2) != 0; ++j)
        {
          if (!strcmp_0(currentClass->m_strCh_Class[j], classRecord->m_strCode))
          {
            changeSlot = static_cast<unsigned __int8>(j);
            break;
          }
        }
        if (changeSlot == 0xFF)
        {
          errCode = 2;
        }
        else
        {
          for (int j = 0; j < 3; ++j)
          {
            if (!m_Param.m_pClassHistory[j])
            {
              classSlot = static_cast<unsigned __int8>(j);
              break;
            }
          }
          if (classSlot == 0xFF)
          {
            errCode = 3;
          }
          else
          {
            rewardClassUp = classSlot + 1 > m_pUserDB->m_AvatorData.dbAvator.m_byLastClassGrade;
            if (rewardClassUp)
            {
              if (bySelectRewardItem == 0xFF)
              {
                if (classRecord->m_bSelectRewardItem)
                {
                  errCode = 4;
                }
                else
                {
                  const int rewardCount = GetRewardItemNumChangeClass(classRecord);
                  const int emptyCount = _STORAGE_LIST::GetNumEmptyCon(&m_Param.m_dbInven);
                  if (emptyCount < rewardCount)
                  {
                    errCode = 5;
                  }
                }
              }
              else if (classRecord->m_bSelectRewardItem)
              {
                char *entry = classRecord->m_DefaultItem[bySelectRewardItem].strDefaultItem;
                if (std::strncmp(entry, "0", 1) == 0)
                {
                  errCode = 4;
                }
                else if (_STORAGE_LIST::GetNumEmptyCon(&m_Param.m_dbInven) < 1)
                {
                  errCode = 5;
                }
              }
              else
              {
                errCode = 4;
              }
            }
          }
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

    if (!errCode)
    {
      char oldClass[36]{};
      memcpy_0(oldClass, m_pUserDB->m_AvatorData.dbAvator.m_szClassCode, 5u);
      int oldMaxPoint[8]{};
      oldMaxPoint[0] = m_nMaxPoint[0];
      oldMaxPoint[1] = m_nMaxPoint[1];
      oldMaxPoint[2] = m_nMaxPoint[2];
      memset(&oldMaxPoint[3], 0, sizeof(int));

      CPlayerDB::SelectClass(&m_Param, classSlot, classRecord);
      if (m_Param.m_pClassData->m_dwIndex == 49
          && m_Param.m_pClassHistory[0]
          && m_Param.m_pClassHistory[0]->m_dwIndex == 45)
      {
        m_fUnitPv_AttFc = FLOAT_1_1;
        CNationSettingManager *manager = CTSingleton<CNationSettingManager>::Instance();
        CNationSettingManager::SetUnitPassiveValue(manager, &m_fUnitPv_DefFc);
        m_fUnitPv_RepPr = FLOAT_0_60000002;
      }
      CalcAddPointByClass();
      m_nAddDfnMstByClass = 0;
      for (int j = 0; j < 4; ++j)
      {
        _DWORD *historyEffect = *m_Param.m_ppHistoryEffect[j];
        if (!historyEffect)
        {
          break;
        }
        m_nAddDfnMstByClass += *reinterpret_cast<int *>(reinterpret_cast<char *>(historyEffect) + 1476);
      }
      if (rewardClassUp && TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, m_id.wIndex) != 99)
      {
        RewardChangeClassMastery(classRecord);
        RewardChangeClassRewardItem(classRecord, bySelectRewardItem);
      }
      ReCalcMaxHFSP(false, false);
      CUserDB::Update_Class(m_pUserDB, classRecord->m_strCode, classSlot, m_Param.m_pClassHistory[classSlot]->m_dwIndex);
      if (rewardClassUp)
      {
        Emb_CreateQuestEvent(quest_happen_type_class, classRecord->m_strCode);
      }
      CMgrAvatorItemHistory::ClassUP(
        &CPlayer::s_MgrItemHistory,
        classSlot + 1,
        m_pUserDB->m_AvatorData.dbAvator.m_byLastClassGrade,
        oldClass,
        m_pUserDB->m_AvatorData.dbAvator.m_szClassCode,
        oldMaxPoint,
        m_nMaxPoint,
        m_szItemHistoryFileName);
      CUserDB::WriteLog_ChangeClassAfterInitClass(m_pUserDB, 1u, oldClass);
      m_bUpCheckEquipEffect = 1;
    }
    SendMsg_SelectClassResult(errCode, wSelClassIndex);
  }
}

unsigned __int8 CPlayer::pc_CanSelectClassRequest(bool *pIsRealClassUp)
{
  if (!CPlayerDB::IsClassChangeableLv(&m_Param))
  {
    return 1;
  }

  unsigned __int8 nextSlot = 0xFF;
  for (int j = 0; j < 3; ++j)
  {
    if (!m_Param.m_pClassHistory[j])
    {
      nextSlot = static_cast<unsigned __int8>(j);
      break;
    }
  }
  if (nextSlot > 1u)
  {
    return 3;
  }

  if (pIsRealClassUp)
  {
    *pIsRealClassUp = nextSlot + 1 <= m_pUserDB->m_AvatorData.dbAvator.m_byLastClassGrade;
  }
  return 0;
}

void CPlayer::SendMsg_StoreListResult()
{
  _store_list_result_zocl msg{};
  const int mapIndex = CPlayerDB::GetMapCode(&m_Param);
  CMapData *map = g_MapOper.GetMap(mapIndex);
  if (!map)
  {
    return;
  }

  const int serial = CMapData::GetMapCode(map);
  CItemStoreManager *manager = CItemStoreManager::Instance();
  CMapItemStoreList *storeList = CItemStoreManager::GetMapItemStoreListBySerial(manager, serial);
  if (!storeList)
  {
    return;
  }

  for (int j = 0; j < storeList->m_nItemStoreNum; ++j)
  {
    CItemStore *store = &storeList->m_ItemStore[j];
    msg.StorePos[j].dwStoreIndex = store->m_pRec->m_dwIndex;
    memcpy_0(msg.StorePos[j].fPos, store->m_pDum->m_pDumPos->m_fCenterPos, sizeof(msg.StorePos[j].fPos));
  }
  msg.byStoreNum = static_cast<unsigned __int8>(map->m_nItemStoreDumNum);

  unsigned __int8 type[2]{12, 9};
  const unsigned __int16 len = msg.size();
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
}

void CPlayer::SendMsg_BuyItemStoreResult(
  CItemStore *pStore,
  int nOfferNum,
  _buy_offer *pCard,
  unsigned __int8 byErrCode)
{
  if (byErrCode)
  {
    _buy_store_failure_zocl msg{};
    msg.dwDalant = CPlayerDB::GetDalant(&m_Param);
    msg.dwGold = CPlayerDB::GetGold(&m_Param);
    msg.dPoint = CPvpOrderView::GetPvpCash(&m_kPvpOrderView);
    for (int j = 0; j < 3; ++j)
    {
      msg.dwActPoint[j] = CUserDB::GetActPoint(m_pUserDB, j);
    }
    msg.byRetCode = byErrCode;

    unsigned __int8 type[2]{12, 3};
    CNetProcess::LoadSendMsg(
      g_Network.m_pProcess[0],
      m_ObjID.m_wIndex,
      type,
      reinterpret_cast<char *>(&msg),
      0x1Du);
    return;
  }

  _buy_store_success_zocl msg{};
  msg.dwLeftDalant = CPlayerDB::GetDalant(&m_Param);
  msg.dwLeftGold = CPlayerDB::GetGold(&m_Param);
  msg.dwConsumDanlant = CItemStore::GetLastTradeDalant(pStore);
  msg.dwConsumGold = CItemStore::GetLastTradeGold(pStore);
  msg.dwLeftPoint = static_cast<unsigned int>(CPvpOrderView::GetPvpCash(&m_kPvpOrderView));
  msg.dwConsumPoint = CItemStore::GetLastTradePoint(pStore);
  for (int j = 0; j < 3; ++j)
  {
    msg.dwConsumActPoint[j] = CItemStore::GetLastTradeActPoint(pStore, j);
  }
  for (int j = 0; j < 3; ++j)
  {
    msg.dwLeftActPoint[j] = CUserDB::GetActPoint(m_pUserDB, j);
  }
  msg.byDiscountRate = 100 * static_cast<unsigned __int8>(_effect_parameter::GetEff_Have(&m_EP, 1));
  msg.byBuyNum = static_cast<unsigned __int8>(nOfferNum);
  for (int j = 0; j < nOfferNum; ++j)
  {
    msg.OfferList[j].wSerial = pCard[j].wSerial;
    msg.OfferList[j].byCsMethod = pCard[j].Item.m_byCsMethod;
    msg.OfferList[j].dwT = pCard[j].Item.m_dwT;
  }

  unsigned __int8 type[2]{12, 2};
  const unsigned __int16 len = msg.size();
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
}

void CPlayer::SendMsg_SellItemStoreResult(CItemStore *pStore, unsigned __int8 byErrCode)
{
  _sell_store_result_zocl msg{};
  msg.bSucc = byErrCode == 0;
  msg.dwLeftDalant = CPlayerDB::GetDalant(&m_Param);
  msg.dwLeftGold = CPlayerDB::GetGold(&m_Param);
  msg.dwProfitDanlant = CItemStore::GetLastTradeDalant(pStore);
  msg.dwProfitGold = CItemStore::GetLastTradeGold(pStore);
  msg.byDiscountRate = 100 * static_cast<unsigned __int8>(_effect_parameter::GetEff_Have(&m_EP, 1));
  msg.byRetCode = byErrCode;

  unsigned __int8 type[2]{12, 5};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 0x13u);
}

void CPlayer::SendMsg_StoreLimitItemAmountInfo(unsigned int dwStoreIndex, _limit_amount_info *pAmountInfo)
{
  _limit_item_num_info_zocl msg{};
  msg.dwStoreIndex = dwStoreIndex;
  for (int j = 0; j < pAmountInfo->byItemNum; ++j)
  {
    msg.LimitItemInfo[j].dwLimitItemIndex = pAmountInfo->ItemInfo[j].dwLimitItemIndex;
    msg.LimitItemInfo[j].wLimitNum = pAmountInfo->ItemInfo[j].wLimitNum;
  }
  msg.byLimitItemNum = pAmountInfo->byItemNum;

  unsigned __int8 type[2]{12, 18};
  const unsigned __int16 len = msg.size();
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
}

void CPlayer::pc_BuyItemStore(
  CItemStore *pStore,
  unsigned __int8 byOfferNum,
  _buy_store_request_clzo::_list *pList,
  int bUseNPCLinkIntem)
{
  _buy_offer offers[100]{};
  unsigned __int8 storageCounts[16]{};
  memset(storageCounts, 0, 8);

  CTransportShip *ship = &g_TransportShip[CPlayerDB::GetRaceCode(&m_Param)];
  unsigned __int8 resultCode = 0;
  if (TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, m_id.wIndex) == 99)
  {
    resultCode = 23;
  }
  else if (bUseNPCLinkIntem || pStore->m_pExistMap == m_pCurMap)
  {
    const int npcRace = pStore->m_byNpcRaceCode;
    const int playerRace = CPlayerDB::GetRaceCode(&m_Param);
    if (npcRace == playerRace || pStore->m_byNpcRaceCode == 255)
    {
      if (bUseNPCLinkIntem || GetSqrt(pStore->m_pDum->m_pDumPos->m_fCenterPos, m_fCurPos) <= 100.0)
      {
        for (int j = 0; j < byOfferNum; ++j)
        {
          ++storageCounts[pList[j].byStorageCode];
        }
        for (int j = 0; j < 8; ++j)
        {
          const int emptyCount = _STORAGE_LIST::GetNumEmptyCon(m_Param.m_pStoragePtr[j]);
          if (emptyCount < storageCounts[j])
          {
            resultCode = 4;
            goto Result;
          }
        }

        for (int j = 0; j < byOfferNum; ++j)
        {
          offers[j].byGoodIndex = pList[j].dwGoodSerial;
          offers[j].byGoodAmount = pList[j].byAmount;
          offers[j].byStorageCode = pList[j].byStorageCode;
        }

        const float discount = _effect_parameter::GetEff_Have(&m_EP, 1) + _effect_parameter::GetEff_Have(&m_EP, 11);
        unsigned __int8 actCode[16]{};
        const int race = CPlayerDB::GetRaceCode(&m_Param);
        unsigned int *actPoints = CUserDB::GetPtrActPoint(m_pUserDB);
        const double pvpCash = CPvpOrderView::GetPvpCash(&m_kPvpOrderView);
        const unsigned int hasGold = CPlayerDB::GetGold(&m_Param);
        const unsigned int hasDalant = CPlayerDB::GetDalant(&m_Param);
        resultCode = CItemStore::IsSell(
          pStore,
          byOfferNum,
          offers,
          hasDalant,
          hasGold,
          pvpCash,
          actPoints,
          actCode,
          discount,
          race,
          m_Param.m_byPvPGrade);

        unsigned __int8 buyRaceBossGoldBox = static_cast<unsigned __int8>(-1);
        if (!resultCode)
        {
          CTransportShip::InitTicketReserver(ship);
          for (int j = 0; j < byOfferNum; ++j)
          {
            if (offers[j].Item.m_byTableCode == 26)
            {
              _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblItemData[26], offers[j].Item.m_wItemIndex);
              if (record && *reinterpret_cast<unsigned int *>(record[1].m_strCode) == 1)
              {
                const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
                CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
                const unsigned int bossSerial =
                  CPvpUserAndGuildRankingSystem::GetCurrentRaceBossSerial(rank, raceCode, 0);
                const unsigned int charSerial = CPlayerDB::GetCharSerial(&m_Param);
                if (bossSerial != charSerial)
                {
                  resultCode = 6;
                  break;
                }
              }
            }

            if (pStore->m_pRec->m_nStore_trade == 18 && offers[j].Item.m_byTableCode == 28)
            {
              char *civil = GetItemEquipCivil(28, offers[j].Item.m_wItemIndex);
              const int raceSexCode = CPlayerDB::GetRaceSexCode(&m_Param);
              if (civil[raceSexCode] != '1')
              {
                resultCode = 16;
                break;
              }
              _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblItemData[28], offers[j].Item.m_wItemIndex);
              if (CTransportShip::GetLeftTicketIncludeReserNum(ship, &record[4].m_strCode[8], 1) <= 0)
              {
                resultCode = 16;
                break;
              }
            }

            if (offers[j].Item.m_byTableCode == 31)
            {
              _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblItemData[31], offers[j].Item.m_wItemIndex);
              if (*reinterpret_cast<unsigned int *>(&record[4].m_strCode[4]) == 11
                  && !strcmp_0(record->m_strCode, "bxgol01"))
              {
                if (offers[j].byGoodAmount > 1u)
                {
                  offers[j].byGoodAmount = 1;
                }
                CGoldenBoxItemMgr *goldMgr = CGoldenBoxItemMgr::Instance();
                buyRaceBossGoldBox = CGoldenBoxItemMgr::IsBuyRaceBossGoldBox(goldMgr, this);
                if (buyRaceBossGoldBox)
                {
                  resultCode = buyRaceBossGoldBox;
                  break;
                }
              }
            }

            if (!IsStorageCodeWithItemKind(offers[j].Item.m_byTableCode, pList[j].byStorageCode))
            {
              resultCode = 5;
              break;
            }

            if (pList[j].byStorageCode == 1)
            {
              _STORAGE_LIST::_db_con *equip =
                &m_Param.m_dbEquip.m_pStorageList[offers[j].Item.m_byTableCode];
              if (equip->m_bLoad)
              {
                resultCode = 5;
                break;
              }
              if (!_check_equip_part(&offers[j].Item))
              {
                resultCode = 7;
                break;
              }
            }

            if (pList[j].byStorageCode == 2)
            {
              if (offers[j].Item.m_byTableCode != 8
                && offers[j].Item.m_byTableCode != 9
                && offers[j].Item.m_byTableCode != 10)
              {
                resultCode = 5;
                break;
              }
              int sameTypeCount = 0;
              for (int k = 0; k < 7; ++k)
              {
                if (m_Param.m_dbEmbellish.m_pStorageList[k].m_bLoad
                    && m_Param.m_dbEmbellish.m_pStorageList[k].m_byTableCode == offers[j].Item.m_byTableCode)
                {
                  ++sameTypeCount;
                }
              }
              if (sameTypeCount > 1)
              {
                resultCode = 5;
                break;
              }
              if (!_check_embel_part(&offers[j].Item))
              {
                resultCode = 7;
                break;
              }
            }

            if (pList[j].byStorageCode == 3)
            {
              for (int m = 0; m < 88; ++m)
              {
                _STORAGE_LIST::_db_con *forceItem = &m_Param.m_dbForce.m_pStorageList[m];
                if (forceItem->m_bLoad
                    && CPlayer::s_pnLinkForceItemToEffect[forceItem->m_wItemIndex]
                      == CPlayer::s_pnLinkForceItemToEffect[offers[j].Item.m_wItemIndex])
                {
                  resultCode = 7;
                  goto Result;
                }
              }
            }

            if (pList[j].byStorageCode == 4)
            {
              for (int m = 0; m < 4; ++m)
              {
                _STORAGE_LIST::_db_con *animus = &m_Param.m_dbAnimus.m_pStorageList[m];
                if (animus->m_bLoad && animus->m_wItemIndex == offers[j].Item.m_wItemIndex)
                {
                  resultCode = 7;
                  goto Result;
                }
              }
            }
          }
        }
      }
      else
      {
        resultCode = 3;
      }
    }
    else
    {
      resultCode = 2;
    }
  }
  else
  {
    resultCode = 1;
  }

Result:
  if (!resultCode)
  {
    CTransportShip::ApplyTicketReserver(ship);

    bool limitUpdated = false;
    for (int j = 0; j < byOfferNum; ++j)
    {
      _good_storage_info *good = &pStore->m_pStorageItem[offers[j].byGoodIndex];
      if (good->byType == 1)
      {
        CItemStore::SubLimitItemNum(pStore, good->dwLimitIndex, offers[j].byGoodAmount);
        limitUpdated = true;
      }
    }
    if (limitUpdated)
    {
      CItemStore::UpdateLimitItemNum(pStore, true);
    }

    SubDalant(CItemStore::GetLastTradeDalant(pStore));
    SubGold(CItemStore::GetLastTradeGold(pStore));
    SubPoint(CItemStore::GetLastTradePoint(pStore));
    for (int j = 0; j < 3; ++j)
    {
      SubActPoint(j, CItemStore::GetLastTradeActPoint(pStore, j));
    }

    for (int j = 0; j < byOfferNum; ++j)
    {
      offers[j].wSerial = CPlayerDB::GetNewItemSerial(&m_Param);
      offers[j].Item.m_wSerial = offers[j].wSerial;
      if (!Emb_AddStorage(pList[j].byStorageCode, &offers[j].Item, false, true))
      {
        SubDalant(-static_cast<int>(CItemStore::GetLastTradeDalant(pStore)));
        SubGold(-static_cast<int>(CItemStore::GetLastTradeGold(pStore)));
        SubPoint(-static_cast<int>(CItemStore::GetLastTradePoint(pStore)));
        int actPointIndex = 0;
        for (; actPointIndex < 3; ++actPointIndex)
        {
          SubActPoint(
            actPointIndex,
            -static_cast<int>(CItemStore::GetLastTradeActPoint(pStore, actPointIndex)));
        }
        SendMsg_BuyItemStoreResult(pStore, byOfferNum, offers, 0x11u);
        // IDA uses the act-point loop index after completion; keep exact behavior.
        CMgrAvatorItemHistory::add_storage_fail(
          &CPlayer::s_MgrItemHistory,
          m_ObjID.m_wIndex,
          &offers[actPointIndex].Item,
          " CPlayer::pc_BuyItemStore - Emb_AddStorage() Fail",
          m_szItemHistoryFileName);
        return;
      }

      if (!buyRaceBossGoldBox && offers[j].Item.m_byTableCode == 31)
      {
        _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblItemData[31], offers[j].Item.m_wItemIndex);
        if (!strcmp_0(record->m_strCode, "bxgol01"))
        {
          CHolyStoneSystem::SetGoldBoxConsumable(&g_HolySys, 0);
          _STORAGE_LIST::_db_con *item = &offers[j].Item;
          char *charName = CPlayerDB::GetCharNameA(&m_Param);
          const unsigned int charSerial = CPlayerDB::GetCharSerial(&m_Param);
          SendMsg_Notify_Get_Golden_Box(1u, charSerial, charName, item, 0);
        }
      }
    }

    if (!m_byUserDgr)
    {
      if (CItemStore::GetLastTradeDalant(pStore) > 0)
      {
        const int addDalant = CItemStore::GetLastTradeDalant(pStore);
        const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
        eAddDalant(raceCode, addDalant);
      }
      if (CItemStore::GetLastTradeGold(pStore) > 0)
      {
        const int addGold = CItemStore::GetLastTradeGold(pStore);
        const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
        eAddGold(raceCode, addGold);
      }
    }

    const int level = CPlayerDB::GetLevel(&m_Param);
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      if (CItemStore::GetLastTradeDalant(pStore) > 0)
      {
        const unsigned int amount = CItemStore::GetLastTradeDalant(pStore);
        char *classCode = CPlayerDB::GetPtrCurClass(&m_Param)->m_strCode;
        const int curLevel = CPlayerDB::GetLevel(&m_Param);
        const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
        CMoneySupplyMgr *moneyMgr = CMoneySupplyMgr::Instance();
        CMoneySupplyMgr::UpdateBuyData(moneyMgr, raceCode, curLevel, classCode, amount);
      }
      if (CItemStore::GetLastTradeGold(pStore) > 0)
      {
        const unsigned int amount = 2000 * CItemStore::GetLastTradeGold(pStore);
        char *classCode = CPlayerDB::GetPtrCurClass(&m_Param)->m_strCode;
        const int curLevel = CPlayerDB::GetLevel(&m_Param);
        const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
        CMoneySupplyMgr *moneyMgr = CMoneySupplyMgr::Instance();
        CMoneySupplyMgr::UpdateBuyData(moneyMgr, raceCode, curLevel, classCode, amount);
      }
    }

    const unsigned int newGold = CPlayerDB::GetGold(&m_Param);
    const unsigned int newDalant = CPlayerDB::GetDalant(&m_Param);
    const unsigned int costGold = CItemStore::GetLastTradeGold(pStore);
    const unsigned int costDalant = CItemStore::GetLastTradeDalant(pStore);
    CMgrAvatorItemHistory::buy_item(
      &CPlayer::s_MgrItemHistory,
      m_ObjID.m_wIndex,
      offers,
      byOfferNum,
      costDalant,
      costGold,
      newDalant,
      newGold,
      m_szItemHistoryFileName);
  }

  if (!resultCode || resultCode == 19)
  {
    _limit_amount_info amountInfo{};
    CItemStore::GetLimitItemAmount(pStore, &amountInfo);
    SendMsg_StoreLimitItemAmountInfo(pStore->m_pRec->m_dwIndex, &amountInfo);
  }

  SendMsg_BuyItemStoreResult(pStore, byOfferNum, offers, resultCode);
}

void CPlayer::pc_SellItemStore(
  CItemStore *pStore,
  unsigned __int8 byOfferNum,
  _sell_store_request_clzo::_list *pList,
  int bUseNPCLinkIntem)
{
  _sell_offer offers[100]{};
  unsigned __int8 resultCode = 0;
  if (TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, m_id.wIndex) == 99)
  {
    resultCode = 23;
    goto Result;
  }
  if (!bUseNPCLinkIntem && pStore->m_pExistMap != m_pCurMap)
  {
    resultCode = 1;
    goto Result;
  }
  const int npcRace = pStore->m_byNpcRaceCode;
  const int playerRace = CPlayerDB::GetRaceCode(&m_Param);
  if (npcRace != playerRace && pStore->m_byNpcRaceCode != 255)
  {
    resultCode = 2;
    goto Result;
  }
  if (!bUseNPCLinkIntem && GetSqrt(pStore->m_pDum->m_pDumPos->m_fCenterPos, m_fCurPos) > 100.0)
  {
    resultCode = 3;
    goto Result;
  }

  int offerIndex = 0;
  while (true)
  {
    if (offerIndex >= byOfferNum)
    {
      break;
    }

    _STORAGE_LIST::_db_con *item = _STORAGE_LIST::GetPtrFromSerial(
      m_Param.m_pStoragePtr[pList[offerIndex].byStorageCode],
      pList[offerIndex].wSerial);
    offers[offerIndex].pItem = item;
    if (!offers[offerIndex].pItem)
    {
      resultCode = 8;
      goto Result;
    }
    if (offers[offerIndex].pItem->m_byTableCode == 19)
    {
      resultCode = 9;
      goto Result;
    }
    if (offers[offerIndex].pItem->m_bLock)
    {
      resultCode = 10;
      goto Result;
    }
    for (int j = 0; j < offerIndex; ++j)
    {
      if (offers[j].pItem == offers[offerIndex].pItem)
      {
        resultCode = 11;
        goto Result;
      }
    }
    if (!IsSellItem(offers[offerIndex].pItem->m_byTableCode, offers[offerIndex].pItem->m_wItemIndex))
    {
      resultCode = 15;
      goto Result;
    }
    offers[offerIndex].bySlotIndex = offers[offerIndex].pItem->m_byStorageIndex;
    offers[offerIndex].byStorageCode = pList[offerIndex].byStorageCode;
    if (IsOverLapItem(offers[offerIndex].pItem->m_byTableCode))
    {
      if (pList[offerIndex].byAmount > offers[offerIndex].pItem->m_dwDur)
      {
        resultCode = 12;
        goto Result;
      }
      offers[offerIndex].byAmount = pList[offerIndex].byAmount;
    }
    else
    {
      offers[offerIndex].byAmount = 1;
    }
    ++offerIndex;
  }

  const float discount = _effect_parameter::GetEff_Have(&m_EP, 1) + _effect_parameter::GetEff_Have(&m_EP, 10);
  const unsigned __int8 race = static_cast<unsigned __int8>(CPlayerDB::GetRaceCode(&m_Param));
  resultCode = CItemStore::IsBuy(pStore, byOfferNum, offers, discount, race);
  if (!resultCode)
  {
    if (CItemStore::GetLastTradeDalant(pStore) <= 0
        || CanAddMoneyForMaxLimMoney(CItemStore::GetLastTradeDalant(pStore), CPlayerDB::GetDalant(&m_Param)))
    {
      if (CItemStore::GetLastTradeGold(pStore) > 0)
      {
        if (!CanAddMoneyForMaxLimGold(CItemStore::GetLastTradeGold(pStore), CPlayerDB::GetGold(&m_Param)))
        {
          resultCode = 18;
        }
      }
    }
    else
    {
      resultCode = 18;
    }
  }

Result:
  if (!resultCode)
  {
    AddDalant(CItemStore::GetLastTradeDalant(pStore), true);
    AddGold(CItemStore::GetLastTradeGold(pStore), true);

    const unsigned int newGold = CPlayerDB::GetGold(&m_Param);
    const unsigned int newDalant = CPlayerDB::GetDalant(&m_Param);
    const unsigned int incomeGold = CItemStore::GetLastTradeGold(pStore);
    const unsigned int incomeDalant = CItemStore::GetLastTradeDalant(pStore);
    CMgrAvatorItemHistory::sell_item(
      &CPlayer::s_MgrItemHistory,
      m_ObjID.m_wIndex,
      offers,
      byOfferNum,
      incomeDalant,
      incomeGold,
      newDalant,
      newGold,
      m_szItemHistoryFileName);

    for (int k = 0; k < byOfferNum; ++k)
    {
      _STORAGE_LIST::_db_con *storageItem =
        &m_Param.m_pStoragePtr[offers[k].byStorageCode]->m_pStorageList[offers[k].bySlotIndex];
      if (IsOverLapItem(offers[k].pItem->m_byTableCode))
      {
        Emb_AlterDurPoint(offers[k].byStorageCode, offers[k].bySlotIndex, -offers[k].byAmount, true, false);
      }
      else if (!Emb_DelStorage(
                 offers[k].byStorageCode,
                 offers[k].bySlotIndex,
                 false,
                 true,
                 "CPlayer::pc_SellItemStore()"))
      {
        AddGold(-static_cast<int>(CItemStore::GetLastTradeDalant(pStore)), true);
        AddGold(-static_cast<int>(CItemStore::GetLastTradeGold(pStore)), true);
        SendMsg_SellItemStoreResult(pStore, 0xFFu);
        return;
      }
      (void)storageItem;
    }

    if (!m_byUserDgr)
    {
      if (CItemStore::GetLastTradeDalant(pStore) > 0)
      {
        const int addDalant = CItemStore::GetLastTradeDalant(pStore);
        const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
        eAddDalant(raceCode, addDalant);
      }
      if (CItemStore::GetLastTradeGold(pStore) > 0)
      {
        const int addGold = CItemStore::GetLastTradeGold(pStore);
        const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
        eAddGold(raceCode, addGold);
      }
    }

    const int level = CPlayerDB::GetLevel(&m_Param);
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      const unsigned int amount = CItemStore::GetLastTradeDalant(pStore);
      char *classCode = CPlayerDB::GetPtrCurClass(&m_Param)->m_strCode;
      const int curLevel = CPlayerDB::GetLevel(&m_Param);
      const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
      CMoneySupplyMgr *moneyMgr = CMoneySupplyMgr::Instance();
      CMoneySupplyMgr::UpdateSellData(moneyMgr, raceCode, curLevel, classCode, amount);
    }
  }
  SendMsg_SellItemStoreResult(pStore, resultCode);
}

void CPlayer::pc_ExchangeDalantForGold(unsigned int dwDalant)
{
  unsigned __int8 resultCode = 0;
  const unsigned int curDalant = CPlayerDB::GetDalant(&m_Param);
  const unsigned int curGold = CPlayerDB::GetGold(&m_Param);
  const int rate = eGetRate(CPlayerDB::GetRaceCode(&m_Param));
  const int exchangeDalant = static_cast<int>(dwDalant - dwDalant % rate);
  const unsigned int feeDalant = static_cast<unsigned int>(static_cast<float>(exchangeDalant) * 0.1f);
  const int addGold = exchangeDalant / rate;

  if (IsBeNearStore(this, 10))
  {
    const unsigned int totalDalant = feeDalant + exchangeDalant;
    if (totalDalant <= CPlayerDB::GetDalant(&m_Param))
    {
      if (addGold)
      {
        const unsigned int gold = CPlayerDB::GetGold(&m_Param);
        if (!CanAddMoneyForMaxLimGold(addGold, gold))
        {
          resultCode = 2;
        }
      }
    }
    else
    {
      resultCode = 1;
    }
  }
  else
  {
    resultCode = 13;
  }

  if (!resultCode)
  {
    SubDalant(feeDalant + exchangeDalant);
    AddGold(addGold, false);

    CMgrAvatorItemHistory::exchange_money(
      &CPlayer::s_MgrItemHistory,
      m_ObjID.m_wIndex,
      curDalant,
      curGold,
      CPlayerDB::GetDalant(&m_Param),
      CPlayerDB::GetGold(&m_Param),
      m_szItemHistoryFileName);

    const unsigned int level = CPlayerDB::GetLevel(&m_Param);
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      const int lv = CPlayerDB::GetLevel(&m_Param);
      const int race = CPlayerDB::GetRaceCode(&m_Param);
      CMoneySupplyMgr *moneyMgr = CMoneySupplyMgr::Instance();
      CMoneySupplyMgr::UpdateFeeMoneyData(moneyMgr, race, lv, feeDalant);
    }
  }
  SendMsg_ExchangeMoneyResult(resultCode);
}

void CPlayer::pc_ExchangeGoldForDalant(unsigned int dwGold)
{
  unsigned __int8 resultCode = 0;
  const unsigned int curGold = CPlayerDB::GetGold(&m_Param);
  const unsigned int curDalant = CPlayerDB::GetDalant(&m_Param);
  const int rate = eGetRate(CPlayerDB::GetRaceCode(&m_Param));
  const unsigned int dalantTotal = rate * dwGold;
  const unsigned int feeDalant = static_cast<unsigned int>(static_cast<float>(dalantTotal) * 0.1f);
  const unsigned int addDalant = dalantTotal - feeDalant;

  if (IsBeNearStore(this, 10))
  {
    if (dwGold <= curGold)
    {
      if (addDalant)
      {
        const unsigned int dalant = CPlayerDB::GetDalant(&m_Param);
        if (!CanAddMoneyForMaxLimMoney(addDalant, dalant))
        {
          resultCode = 2;
        }
      }
    }
    else
    {
      resultCode = 1;
    }
  }
  else
  {
    resultCode = 13;
  }

  if (!resultCode)
  {
    SubGold(dwGold);
    AddDalant(addDalant, false);

    CMgrAvatorItemHistory::exchange_money(
      &CPlayer::s_MgrItemHistory,
      m_ObjID.m_wIndex,
      curDalant,
      curGold,
      CPlayerDB::GetDalant(&m_Param),
      CPlayerDB::GetGold(&m_Param),
      m_szItemHistoryFileName);

    const unsigned int level = CPlayerDB::GetLevel(&m_Param);
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      const int lv = CPlayerDB::GetLevel(&m_Param);
      const int race = CPlayerDB::GetRaceCode(&m_Param);
      CMoneySupplyMgr *moneyMgr = CMoneySupplyMgr::Instance();
      CMoneySupplyMgr::UpdateFeeMoneyData(moneyMgr, race, lv, feeDalant);
    }
  }
  SendMsg_ExchangeMoneyResult(resultCode);
}

void CPlayer::pc_LimitItemNumRequest(unsigned int dwStoreIndex)
{
  if (!m_pCurMap)
  {
    return;
  }

  CItemStoreManager *storeMgr = CItemStoreManager::Instance();
  if (dwStoreIndex >= CRecordData::GetRecordNum(&storeMgr->m_tblItemStore))
  {
    return;
  }

  CMapItemStoreList *storeList = nullptr;
  if (m_pCurMap->m_pMapSet->m_nMapType == 1)
  {
    if (m_Param.m_pGuild)
    {
      CGuildRoomSystem *room = CGuildRoomSystem::GetInstance();
      if (CGuildRoomSystem::IsGuildRoomMemberIn(
            room,
            m_Param.m_pGuild->m_dwSerial,
            m_ObjID.m_wIndex,
            m_pUserDB->m_dwSerial))
      {
        CItemStoreManager *manager = CItemStoreManager::Instance();
        storeList = CItemStoreManager::GetInstanceStoreListBySerial(manager, m_Param.m_pGuild->m_dwSerial);
      }
    }
  }
  else
  {
    const int serial = CMapData::GetMapCode(m_pCurMap);
    CItemStoreManager *manager = CItemStoreManager::Instance();
    storeList = CItemStoreManager::GetMapItemStoreListBySerial(manager, serial);
  }

  if (!storeList)
  {
    return;
  }

  CItemStore *store = CMapItemStoreList::GetItemStoreFromRecIndex(storeList, dwStoreIndex);
  if (!store)
  {
    return;
  }

  _limit_amount_info amountInfo{};
  CItemStore::GetLimitItemAmount(store, &amountInfo);
  SendMsg_StoreLimitItemAmountInfo(store->m_pRec->m_dwIndex, &amountInfo);
}

void CPlayer::pc_PvpCashRecorver(unsigned __int16 dwItemSerial, unsigned __int8 byItemCnt)
{
  unsigned __int8 resultCode = 0;
  _STORAGE_LIST::_db_con *item = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, dwItemSerial);
  _base_fld *record = nullptr;
  if (item && item->m_dwDur >= byItemCnt)
  {
    record = CRecordData::GetRecord(&g_Main.m_tblItemData[18], item->m_wItemIndex);
    if (record)
    {
      CPvpCashMng *cashMng = CPvpCashMng::Instance();
      if (!CPvpCashMng::IsTalikItem(cashMng, record->m_strCode))
      {
        resultCode = 1;
      }
    }
    else
    {
      resultCode = 1;
    }
  }
  else
  {
    resultCode = 1;
  }

  if (resultCode)
  {
    CPvpCashPoint::SendMsg_RecoverResult(&m_kPvpCashPoint, m_ObjID.m_wIndex, resultCode, 0);
    return;
  }

  const int recoverPoint = CPvpCashPoint::GetTalikRecvrPoint(&m_kPvpCashPoint, item->m_byTableCode, record->m_dwIndex);
  int addedPoint = 0;
  int appliedCount = 0;
  const double pvpCash = CPvpOrderView::GetPvpCash(&m_kPvpOrderView);
  if (pvpCash >= 0.0)
  {
    resultCode = 2;
  }
  CPvpCashPoint::SendMsg_RecoverResult(&m_kPvpCashPoint, m_ObjID.m_wIndex, resultCode, recoverPoint);
  if (!resultCode)
  {
    long double alter = 0.0;
    for (int j = 1; j <= byItemCnt; ++j)
    {
      addedPoint += recoverPoint;
      appliedCount = j;
      if (pvpCash + static_cast<double>(addedPoint) >= 0.0)
      {
        alter = -0.0 - pvpCash;
        break;
      }
      alter = static_cast<double>(addedPoint);
    }
    AlterPvPCashBag(alter, pm_reward);
    if (IsOverLapItem(item->m_byTableCode))
    {
      Emb_AlterDurPoint(0, item->m_byStorageIndex, -appliedCount, 1, 1);
    }
    else if (Emb_DelStorage(0, item->m_byStorageIndex, 0, 1, "CPlayer::pc_PvpCashRecorver"))
    {
      CMgrAvatorItemHistory::consume_del_item(&CPlayer::s_MgrItemHistory, m_ObjID.m_wIndex, item, m_szItemHistoryFileName);
    }
    else
    {
      AlterPvPCashBag(-0.0 - alter, pm_reward);
    }
  }
}

void CPlayer::SendMsg_RadarDelayTime(unsigned int dwDelay)
{
  char szMsg[32];
  *reinterpret_cast<unsigned int *>(szMsg) = dwDelay;

  unsigned __int8 pbyType[2] = {7, 53};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], this->m_ObjID.m_wIndex, pbyType, szMsg, 4u);
}

void CPlayer::SendMsg_StartNewPos(char byMapInMode)
{
  char szMsg[32];
  szMsg[0] = byMapInMode;

  unsigned __int8 pbyType[2] = {3, 2};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], this->m_ObjID.m_wIndex, pbyType, szMsg, 1u);
}

void CPlayer::SendMsg_ExitWorldResult(unsigned __int8 byRetCode)
{
  (void)byRetCode;
  char szMsg[32];

  unsigned __int8 pbyType[2] = {3, 42};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], this->m_ObjID.m_wIndex, pbyType, szMsg, 1u);
}

void CPlayer::SendMsg_Revival(char byRet, bool bEquialZone)
{
  #pragma pack(push, 1)
  struct RevivalMsg
  {
    char byRet;
    __int16 wMap;
    __int16 wPos[3];
    char byLevel;
    __int16 wHP;
    __int16 wFP;
    __int16 wSP;
    bool bNotEqualZone;
  };
  #pragma pack(pop)

  RevivalMsg msg{};
  msg.byRet = byRet;
  msg.byLevel = CPlayerDB::GetLevel(&this->m_Param);
  msg.wFP = CPlayerDB::GetFP(&this->m_Param);
  msg.wHP = CPlayerDB::GetHP(&this->m_Param);
  msg.wSP = CPlayerDB::GetSP(&this->m_Param);
  msg.wMap = CMapOperation::GetMap(&g_MapOper, this->m_pCurMap);
  FloatToShort(this->m_fCurPos, msg.wPos, 3);
  msg.bNotEqualZone = !bEquialZone;

  unsigned __int8 pbyType[2] = {3, 38};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    0x11u);
}

void CPlayer::SendMsg_OtherShapeError(CPlayer *pDst, unsigned __int8 byErrCode)
{
  #pragma pack(push, 1)
  struct OtherShapeErrorMsg
  {
    unsigned __int16 wIndex;
    unsigned __int8 byErrCode;
  };
  #pragma pack(pop)

  OtherShapeErrorMsg msg{};
  msg.wIndex = this->m_ObjID.m_wIndex;
  msg.byErrCode = byErrCode;

  unsigned __int8 pbyType[2] = {3, 33};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    pDst->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    3u);
}

void CPlayer::SendMsg_OtherShapeAll(CPlayer *pDst)
{
  if (!this->m_bObserver || pDst->m_byUserDgr)
  {
    if (this->m_bLive)
    {
      unsigned __int8 pbyType[2] = {3, 31};
      const unsigned __int16 nLen = static_cast<unsigned __int16>(this->m_bufShapeAll.size());
      CNetProcess::LoadSendMsg(
        g_Network.m_pProcess[0],
        pDst->m_ObjID.m_wIndex,
        pbyType,
        reinterpret_cast<char *>(&this->m_bufShapeAll),
        nLen);
    }
    else
    {
      SendMsg_OtherShapeError(pDst, 0);
    }
  }
}

void CPlayer::SendMsg_OtherShapePart(CPlayer *pDst)
{
  if (!this->m_bObserver || pDst->m_byUserDgr)
  {
    if (this->m_bLive)
    {
      unsigned __int8 pbyType[2] = {3, 32};
      const unsigned __int16 nLen = static_cast<unsigned __int16>(this->m_bufSpapePart.size());
      CNetProcess::LoadSendMsg(
        g_Network.m_pProcess[0],
        pDst->m_ObjID.m_wIndex,
        pbyType,
        reinterpret_cast<char *>(&this->m_bufSpapePart),
        nLen);
    }
    else
    {
      SendMsg_OtherShapeError(pDst, 0);
    }
  }
}

void CPlayer::SendMsg_OtherShapePartEx_CashChange(
  CPlayer *pDst,
  CashChangeStateFlag ServerData,
  CashChangeStateFlag ClinetData)
{
  if ((!this->m_bObserver || pDst->m_byUserDgr)
      && (ServerData.m_byStateFlag & 7) != (ClinetData.m_byStateFlag & 7))
  {
    #pragma pack(push, 1)
    struct OtherShapeCashChangeMsg
    {
      unsigned __int16 wIndex;
      unsigned int dwSerial;
      char wszName[17];
    };
    #pragma pack(pop)

    OtherShapeCashChangeMsg msg{};
    msg.wIndex = this->m_ObjID.m_wIndex;
    msg.dwSerial = this->m_dwObjSerial;
    const char *charName = CPlayerDB::GetCharNameW(&this->m_Param);
    strcpy_0(msg.wszName, charName);

    unsigned __int8 pbyType[2] = {3, 61};
    CNetProcess::LoadSendMsg(
      g_Network.m_pProcess[0],
      pDst->m_ObjID.m_wIndex,
      pbyType,
      reinterpret_cast<char *>(&msg),
      0x17u);
  }
}

void CPlayer::SendMsg_Init_Action_Point()
{
  _Init_action_point_zocl msg{};
  for (int j = 0; j < 3; ++j)
  {
    msg.dwActionPoint[j] = this->m_pUserDB->m_AvatorData.dbSupplement.dwActionPoint[j];
  }

  unsigned __int8 pbyType[2] = {11, 37};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    0xCu);
}

void CPlayer::SendMsg_BuyCashItemMode()
{
  char szMsg[32];
  CashItemRemoteStore *store = CashItemRemoteStore::Instance();
  szMsg[0] = !CashItemRemoteStore::IsBuyCashItemByGold(store);

  unsigned __int8 pbyType[2] = {59, 17};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], this->m_ObjID.m_wIndex, pbyType, szMsg, 1u);
}

void CPlayer::SendMsg_BaseDownloadResult()
{
  _base_download_result_zocl msg{};

  this->m_bBaseDownload = 1;
  msg.byRetCode = 0;
  msg.wZoneIndex = this->m_ObjID.m_wIndex;
  msg.dwExpRate = this->m_dwExpRate;
  for (int j = 0; j < 3; ++j)
  {
    if (this->m_Param.m_pClassHistory[j])
    {
      msg.wClassHistory[j] = this->m_Param.m_pClassHistory[j]->m_dwIndex;
    }
    else
    {
      msg.wClassHistory[j] = static_cast<unsigned __int16>(-1);
    }
  }
  msg.wHP = CPlayerDB::GetHP(&this->m_Param);
  msg.wSP = CPlayerDB::GetSP(&this->m_Param);
  msg.wFP = CPlayerDB::GetFP(&this->m_Param);
  msg.wDP = CPlayerDB::GetDP(&this->m_Param);
  msg.dPvpPoint = CPlayerDB::GetPvPPoint(&this->m_Param);
  msg.dPvpCashBag = CPvpOrderView::GetPvpCash(&this->m_kPvpOrderView);
  msg.dPvpTempCash = CPvpOrderView::GetPvpTempCash(&this->m_kPvpOrderView);
  msg.dwPvpRank = this->m_Param.m_dbChar.m_dwRank;
  msg.byPvpClass = this->m_Param.m_byPvPGrade;
  msg.wMaxHP = this->m_nMaxPoint[0];
  msg.wMaxSP = this->m_nMaxPoint[2];
  msg.wMaxFP = this->m_nMaxPoint[1];
  msg.wMaxDP = static_cast<unsigned __int16>(CPlayer::GetMaxDP(this));

  const int mapCode = CPlayerDB::GetMapCode(&this->m_Param);
  unsigned __int16 *record =
    reinterpret_cast<unsigned __int16 *>(g_MapOper.m_tblMapData.GetRecord(mapCode));
  msg.wMapIndex = *record;
  float *curPos = CPlayerDB::GetCurPos(&this->m_Param);
  memcpy_0(msg.fPos, curPos, sizeof(msg.fPos));
  msg.byUseTrunkSlotNum = this->m_Param.m_dbTrunk.m_nUsedNum;

  for (int j = 0; j < 8; ++j)
  {
    _STORAGE_LIST::_db_con *equipItem = &this->m_Param.m_dbEquip.m_pStorageList[j];
    if (equipItem->m_bLoad)
    {
      msg.EquipList[j].sTableCode = equipItem->m_byTableCode;
      msg.EquipList[j].wItemIndex = equipItem->m_wItemIndex;
      msg.EquipList[j].dwUptInfo = equipItem->m_dwLv;
      msg.EquipList[j].byCsMethod = equipItem->m_byCsMethod;
      msg.EquipList[j].dwT = equipItem->m_dwT;
    }
    else
    {
      msg.EquipList[j].sTableCode = -1;
    }
  }

  for (int j = 0; j < 7; ++j)
  {
    _STORAGE_LIST::_db_con *embellishItem = &this->m_Param.m_dbEmbellish.m_pStorageList[j];
    if (embellishItem->m_bLoad)
    {
      msg.EmbellishList[j].sTableCode = embellishItem->m_byTableCode;
      msg.EmbellishList[j].wItemIndex = embellishItem->m_wItemIndex;
      msg.EmbellishList[j].wNum = embellishItem->m_dwDur;
      msg.EmbellishList[j].sClientIndex = embellishItem->m_byClientIndex;
      msg.EmbellishList[j].byCsMethod = embellishItem->m_byCsMethod;
      msg.EmbellishList[j].dwT = embellishItem->m_dwT;
    }
    else
    {
      msg.EmbellishList[j].sTableCode = -1;
    }
  }

  if (CHolyStoneSystem::GetHolyMasterRace(&g_HolySys) == -1)
  {
    if (CHolyStoneSystem::GetSceneCode(&g_HolySys) == 1)
    {
      msg.byHolyMasterState = static_cast<unsigned __int8>(-1);
    }
    else
    {
      msg.byHolyMasterState = 100;
    }
  }
  else
  {
    msg.byHolyMasterState = CHolyStoneSystem::GetHolyMasterRace(&g_HolySys);
  }

  msg.byPlusKey = this->m_byPlusKey;
  msg.wXorKey = this->m_wXorKey;

  if (this->m_Param.m_pGuild)
  {
    msg.dwGuildSerial = this->m_Param.m_pGuild->m_dwSerial;
    const int raceCode = CPlayerDB::GetRaceCode(&this->m_Param);
    CHonorGuild *honorGuild = CHonorGuild::Instance();
    msg.byHonorGuildRank = CHonorGuild::FindHonorGuildRank(honorGuild, raceCode, msg.dwGuildSerial);
  }
  else
  {
    msg.dwGuildSerial = static_cast<unsigned int>(-1);
    msg.byHonorGuildRank = static_cast<unsigned __int8>(-1);
  }

  msg.byGuildGrade = 0;
  msg.byEffectValue[0] = 0;
  msg.byEffectValue[1] = 0;
  if (this->m_Param.m_pGuild)
  {
    CGuildMasterEffect *effect = CGuildMasterEffect::GetInstance();
    const int adjustableGrade = CGuildMasterEffect::get_AdjustableGrade(effect);
    const unsigned __int8 grade = CGuild::GetGrade(this->m_Param.m_pGuild);
    if (adjustableGrade <= grade && this->m_Param.m_byClassInGuild == 2)
    {
      msg.byGuildGrade = CGuild::GetGrade(this->m_Param.m_pGuild);
      effect = CGuildMasterEffect::GetInstance();
      msg.byEffectValue[0] = CGuildMasterEffect::get_AttactValueByGrade(effect, msg.byGuildGrade);
      effect = CGuildMasterEffect::GetInstance();
      msg.byEffectValue[1] = CGuildMasterEffect::get_DefenceValueByGrade(effect, msg.byGuildGrade);
    }
  }

  unsigned __int8 pbyType[2] = {3, 4};
  const unsigned __int16 nLen = static_cast<unsigned __int16>(msg.size());
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    nLen);
}

void CPlayer::SendMsg_InvenDownloadResult()
{
  _inven_download_result_zocl msg;

  this->m_bInvenDownload = 1;
  msg.byRetCode = 0;
  msg.byBagNum = this->m_Param.m_dbChar.m_byUseBagNum;
  int count = 0;
  const int useSlot = CPlayerDB::GetUseSlot(&this->m_Param);
  for (int j = 0; j < useSlot; ++j)
  {
    _STORAGE_LIST::_db_con *item = &this->m_Param.m_dbInven.m_pStorageList[j];
    if (item->m_bLoad)
    {
      msg.ItemSlotInfo[count].byTableCode = item->m_byTableCode;
      msg.ItemSlotInfo[count].sClientIndex = item->m_byClientIndex;
      msg.ItemSlotInfo[count].wItemIndex = item->m_wItemIndex;
      msg.ItemSlotInfo[count].dwUptInfo = item->m_dwLv;
      msg.ItemSlotInfo[count].dwDurPoint = item->m_dwDur;
      msg.ItemSlotInfo[count].byCsMethod = item->m_byCsMethod;
      msg.ItemSlotInfo[count++].dwT = item->m_dwT;
    }
  }
  msg.bySlotNum = static_cast<unsigned __int8>(count);

  unsigned __int8 pbyType[2] = {3, 6};
  const unsigned __int16 nLen = static_cast<unsigned __int16>(msg.size());
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    nLen);
}

void CPlayer::SendMsg_CumDownloadResult()
{
  _cum_download_result_zocl msg;

  this->m_bCumDownload = 1;
  msg.byRetCode = 0;
  memcpy_0(&msg.Stat, &this->m_pmMst.m_BaseCum, sizeof(msg.Stat));

  unsigned __int8 count = 0;
  for (int j = 0; j < GetMaxResKind(); ++j)
  {
    if (this->m_Param.m_wCuttingResBuffer[j])
    {
      const unsigned __int16 packed =
        static_cast<unsigned __int16>((this->m_Param.m_wCuttingResBuffer[j] << 8) | (j & 0xFF));
      msg.wleftResList[count++] = packed;
    }
  }
  msg.byLeftCuttingResNum = count;

  unsigned __int8 pbyType[2] = {3, 8};
  const unsigned __int16 nLen = static_cast<unsigned __int16>(msg.size());
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    nLen);
}

void CPlayer::SendMsg_ForceDownloadResult()
{
  _force_download_result_zocl msg;

  this->m_bForceDownload = 1;
  msg.byRetCode = 0;
  int count = 0;
  for (int j = 0; j < 88; ++j)
  {
    _STORAGE_LIST::_db_con *item = &this->m_Param.m_dbForce.m_pStorageList[j];
    if (item->m_bLoad)
    {
      msg.ItemSlotInfo[count].dwCum = item->m_dwDur;
      msg.ItemSlotInfo[count].wItemIndex = item->m_wItemIndex;
      msg.ItemSlotInfo[count].byCsMethod = item->m_byCsMethod;
      msg.ItemSlotInfo[count++].dwT = item->m_dwT;
    }
  }
  msg.bySlotNum = static_cast<unsigned __int8>(count);

  unsigned __int8 pbyType[2] = {3, 10};
  const unsigned __int16 nLen = static_cast<unsigned __int16>(msg.size());
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    nLen);
}

void CPlayer::SendMsg_QuestDownloadResult()
{
  _quest_download_result_zocl msg;

  this->m_bQuestDownload = 1;
  msg.byRetCode = 0;
  int count = 0;
  for (int j = 0; j < 30; ++j)
  {
    _QUEST_DB_BASE::_LIST *quest = &this->m_Param.m_QuestDB.m_List[j];
    if (quest->byQuestType != 0xFF)
    {
      msg.SlotInfo[count].byDBSlotIndex = static_cast<unsigned __int8>(j);
      memcpy_0(&msg.SlotInfo[count].cont, quest, sizeof(msg.SlotInfo[count].cont));
      ++count;
    }
  }
  msg.bySlotNum = static_cast<unsigned __int8>(count);

  unsigned __int8 pbyType[2] = {3, 12};
  const unsigned __int16 nLen = static_cast<unsigned __int16>(msg.size());
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    nLen);
}

void CPlayer::SendMsg_QuestHistoryDownloadResult()
{
  _quest_history_download_result_zocl msg;

  int count = 0;
  for (int j = 0; j < 70; ++j)
  {
    if (this->m_Param.m_QuestDB.m_History[j].byLevel != 0xFF)
    {
      msg.SlotInfo[count].byIndex = static_cast<unsigned __int8>(j);
      strcpy_0(msg.SlotInfo[count++].szQuestCode, this->m_Param.m_QuestDB.m_History[j].szQuestCode);
    }
  }
  msg.bySlotNum = static_cast<unsigned __int8>(count);

  unsigned __int8 pbyType[2] = {3, 57};
  const unsigned __int16 nLen = static_cast<unsigned __int16>(msg.size());
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    nLen);
}

void CPlayer::SendMsg_SpecialDownloadResult()
{
  this->m_bSpecialDownload = 1;
  const unsigned int raceCode = static_cast<unsigned int>(CPlayerDB::GetRaceCode(&this->m_Param));

  if (raceCode)
  {
    if (raceCode == 1)
    {
      _animus_download_result_zocl msg;
      int count = 0;
      for (int j = 0; j < 4; ++j)
      {
        _STORAGE_LIST::_db_con *item = &this->m_Param.m_dbAnimus.m_pStorageList[j];
        if (item->m_bLoad)
        {
          msg.AnimusList[count].sItemIndex = item->m_wItemIndex;
          msg.AnimusList[count].dwExp = item->m_dwDur;
          msg.AnimusList[count].dwParam = item->m_dwLv;
          msg.AnimusList[count].byCsMethod = item->m_byCsMethod;
          msg.AnimusList[count++].dwT = item->m_dwT;
        }
      }
      msg.byAnimusNum = static_cast<unsigned __int8>(count);

      unsigned __int8 pbyType[2] = {3, 15};
      const unsigned __int16 nLen = static_cast<unsigned __int16>(msg.size());
      CNetProcess::LoadSendMsg(
        g_Network.m_pProcess[0],
        this->m_ObjID.m_wIndex,
        pbyType,
        reinterpret_cast<char *>(&msg),
        nLen);
    }
    else
    {
      _animus_download_result_zocl msg;
      unsigned __int8 pbyType[2] = {3, 15};
      const unsigned __int16 nLen = static_cast<unsigned __int16>(msg.size());
      CNetProcess::LoadSendMsg(
        g_Network.m_pProcess[0],
        this->m_ObjID.m_wIndex,
        pbyType,
        reinterpret_cast<char *>(&msg),
        nLen);
    }
  }
  else
  {
    _unit_download_result_zocl msg;
    int count = 0;
    for (int j = 0; j < 4; ++j)
    {
      if (this->m_Param.m_UnitDB.m_List[j].byFrame != 0xFF)
      {
        msg.UnitList[count].bySlotIndex = static_cast<unsigned __int8>(j);
        memcpy_0(
          &msg.UnitList[count].UnitData,
          &this->m_Param.m_UnitDB.m_List[j],
          sizeof(msg.UnitList[count].UnitData));
        ++count;
      }
    }
    msg.byUnitNum = static_cast<unsigned __int8>(count);

    unsigned __int8 pbyType[2] = {3, 14};
    const unsigned __int16 nLen = static_cast<unsigned __int16>(msg.size());
    CNetProcess::LoadSendMsg(
      g_Network.m_pProcess[0],
      this->m_ObjID.m_wIndex,
      pbyType,
      reinterpret_cast<char *>(&msg),
      nLen);
  }
}

void CPlayer::SendMsg_LinkBoardDownloadResult()
{
  if (!this->m_pUserDB)
  {
    return;
  }

  _LINK_DB_BASE msg;
  for (int j = 0; j < 50; ++j)
  {
    _LINKKEY *key = &this->m_pUserDB->m_AvatorData.dbLink.m_LinkList[j].Key;
    msg.m_LinkList[j].Key = *key;
    if (_LINKKEY::IsFilled(key))
    {
      const unsigned __int16 code = _LINKKEY::GetCode(key);
      if (code == 4)
      {
        const unsigned __int16 index = _LINKKEY::GetIndex(key);
        const int storageIndex = static_cast<int>(index) >> 8;
        const unsigned __int8 slotIndex = static_cast<unsigned __int8>(index);
        if (IsStorageRange(static_cast<unsigned __int8>(index >> 8), slotIndex))
        {
          _STORAGE_LIST::_db_con *item =
            &this->m_Param.m_pStoragePtr[storageIndex]->m_pStorageList[slotIndex];
          if (item)
          {
            _LINK_DB_BASE::_LIST list;
            list.Key.SetData(code, item->m_wSerial);
            msg.m_LinkList[j] = list;
          }
        }
      }
    }
  }

  msg.m_byLinkBoardLock = this->m_pUserDB->m_AvatorData.dbLink.m_byLinkBoardLock;
  memcpy_0(msg.m_dwSkill, this->m_pUserDB->m_AvatorData.dbLink.m_dwSkill, sizeof(msg.m_dwSkill));
  memcpy_0(msg.m_dwForce, this->m_pUserDB->m_AvatorData.dbLink.m_dwForce, sizeof(msg.m_dwForce));
  memcpy_0(msg.m_dwCharacter, this->m_pUserDB->m_AvatorData.dbLink.m_dwCharacter, sizeof(msg.m_dwCharacter));
  memcpy_0(msg.m_dwAnimus, this->m_pUserDB->m_AvatorData.dbLink.m_dwAnimus, sizeof(msg.m_dwAnimus));
  memcpy_0(msg.m_dwInvenBag, this->m_pUserDB->m_AvatorData.dbLink.m_dwInvenBag, 0x14u);
  msg.m_dwInven = this->m_pUserDB->m_AvatorData.dbLink.m_dwInven;

  unsigned __int8 pbyType[2] = {3, 45};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    0x9Du);
}

void CPlayer::SendMsg_MacroRequest()
{
  _player_macro_result_zocl msg;
  _AIOC_A_MACRODATA *macro = &this->m_pUserDB->m_AvatorData.dbMacro;

  memset_0(&msg, 0, sizeof(msg));
  for (int k = 0; k < 3; ++k)
  {
    msg.potion[k] = macro->mcr_Potion[0].Potion[k];
    msg.potionvalue[k] = macro->mcr_Potion[0].PotionValue[k];
  }
  for (int j = 0; j < 3; ++j)
  {
    for (int k = 0; k < 10; ++k)
    {
      msg.behavior[10 * j + k] = macro->mcr_Action[j].Action[k];
    }
  }
  for (int j = 0; j < 2; ++j)
  {
    for (int k = 0; k < 5; ++k)
    {
      strncpy(msg.chatting[5 * j + k], macro->mcr_Chat[j].Chat[k], 0x51u);
    }
  }

  unsigned __int8 pbyType[2] = {3, 50};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    0x3BAu);
}

void CPlayer::SendMsg_AMPInvenDownloadResult()
{
  _personal_automine_download_zocl msg{};

  this->m_bAMPInvenDownload = 1;
  msg.byHaveBag = CPlayerDB::BeHaveBoxOfAMP(&this->m_Param);

  int count = 0;
  for (int j = 0; j < 40; ++j)
  {
    _STORAGE_LIST::_db_con *item = &this->m_Param.m_dbPersonalAmineInven.m_pStorageList[j];
    if (item->m_bLoad)
    {
      msg.list[count].byClientSlotIndex = item->m_byClientIndex;
      msg.list[count].byTblCode = item->m_byTableCode;
      msg.list[count].wItemTblIndex = item->m_wItemIndex;
      msg.list[count++].byDur = static_cast<unsigned __int8>(item->m_dwDur);
    }
  }
  msg.byCnt = static_cast<unsigned __int8>(count);

  unsigned __int8 pbyType[2] = {3, 55};
  const unsigned __int16 nLen = static_cast<unsigned __int16>(msg.size());
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    nLen);
}

void CPlayer::SendMsg_SetGroupTargetObjectResult(char byRetCode, char byGroupType)
{
  #pragma pack(push, 1)
  struct SetGroupTargetObjectResultMsg
  {
    char byRetCode;
    char byGroupType;
    char byMapCode;
    char byID;
    unsigned int dwSerial;
    float fPos[3];
  };
  #pragma pack(pop)

  SetGroupTargetObjectResultMsg msg{};
  msg.byRetCode = byRetCode;

  const unsigned __int8 groupType = static_cast<unsigned __int8>(byGroupType);
  if (!byRetCode && this->m_GroupTargetObject[groupType].pObject)
  {
    msg.byGroupType = byGroupType;
    msg.byMapCode = static_cast<unsigned __int8>(this->m_GroupTargetObject[groupType].pObject->m_pCurMap->m_nMapCode);
    msg.byID = this->m_GroupTargetObject[groupType].byID;
    msg.dwSerial = this->m_GroupTargetObject[groupType].dwSerial;
    memcpy_0(msg.fPos, this->m_GroupTargetObject[groupType].pObject->m_fCurPos, sizeof(msg.fPos));
  }

  unsigned __int8 pbyType[2] = {13, 107};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    0x14u);
}

void CPlayer::SendMsg_SetGroupMapPoint(
  char byRetCode,
  char byGroupType,
  char byMapCode,
  float *pzTar,
  unsigned __int8 byRemain)
{
  #pragma pack(push, 1)
  struct SetGroupMapPointMsg
  {
    char byRetCode;
    char byGroupType;
    char byMapCode;
    char byRemain;
    float fPos[2];
  };
  #pragma pack(pop)

  SetGroupMapPointMsg msg{};
  msg.byRetCode = byRetCode;
  msg.byGroupType = byGroupType;
  msg.byMapCode = byMapCode;
  msg.byRemain = static_cast<char>(byRemain);
  memcpy_0(msg.fPos, pzTar, sizeof(msg.fPos));

  unsigned __int8 pbyType[2] = {13, 114};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    0xCu);
}

void CPlayer::pc_NewPosStart()
{
  unsigned __int8 resultCode = 0;
  if (this->m_bMapLoading)
  {
    if (this->m_byMapInModeBuffer == 0xFF)
    {
      resultCode = 2;
    }
  }
  else
  {
    resultCode = 1;
  }

  if (!resultCode)
  {
    const unsigned __int8 mapInMode = this->m_byMapInModeBuffer;
    if (mapInMode)
    {
      if (mapInMode <= 6u || mapInMode == 8)
      {
        if (!IntoMap(mapInMode))
        {
          resultCode = 3;
        }
      }
      else
      {
        resultCode = 5;
      }
    }
    else if (!Create())
    {
      resultCode = 3;
    }
  }

  if (resultCode)
  {
    const char *charName = CPlayerDB::GetCharNameA(&this->m_Param);
    char buffer[144]{};
    sprintf(buffer, "Close.. %s: MapInMode(%u) Reason(%d)", charName, this->m_byMapInModeBuffer, resultCode);
    CNetworkEX::Close(&g_Network, 0, this->m_ObjID.m_wIndex, false, buffer);
  }
  else
  {
    SendMsg_StartNewPos(this->m_byMapInModeBuffer);
    SendMsg_NewViewOther(this->m_byMapInModeBuffer);
    NewViewCircleObject();
    if (this->m_byMapInModeBuffer && this->m_byMapInModeBuffer != 6)
    {
      if (this->m_byMapInModeBuffer == 2)
      {
        pc_NuclearAfterEffect();
      }
    }
    else
    {
      CreateComplete();
    }

    if (GetCurSecNum() != static_cast<unsigned int>(-1))
    {
      this->m_bMapLoading = 0;
      this->m_byMapInModeBuffer = 0xFF;
    }

    CheckGroupTargeting();
    CheckGroupMapPoint();
    CNuclearBombMgr *manager = CNuclearBombMgr::Instance();
    CNuclearBombMgr::CheckNuclearState(manager, this);
  }
}

void CPlayer::NewViewCircleObject()
{
  _sec_info *secInfo = this->m_pCurMap->GetSecInfo();
  const unsigned int secNum = GetCurSecNum();
  const int useSectorRange = GetUseSectorRange();

  _pnt_rect rect{};
  this->m_pCurMap->GetRectInRadius(&rect, useSectorRange, secNum);

  for (int y = rect.nStarty; y <= rect.nEndy; ++y)
  {
    for (int x = rect.nStartx; x <= rect.nEndx; ++x)
    {
      const unsigned int secIndex = secInfo->m_nSecNumW * y + x;
      CObjectList *list = this->m_pCurMap->GetSectorListObj(this->m_wMapLayerIndex, secIndex);
      if (!list)
      {
        continue;
      }

      for (_object_list_point *node = list->m_Head.m_pNext; node != &list->m_Tail; node = node->m_pNext)
      {
        CGameObject *obj = node->m_pItem;
        if (obj == this)
        {
          continue;
        }

        if (obj->m_bMove)
        {
          obj->SendMsg_RealMovePoint(this->m_ObjID.m_wIndex);
        }
        else
        {
          obj->SendMsg_FixPosition(this->m_ObjID.m_wIndex);
        }
      }
    }
  }
}

void CPlayer::CheckGroupTargeting()
{
  CPlayer *groupPlayer = nullptr;

  for (int groupType = 0; groupType < 3; ++groupType)
  {
    if (groupType)
    {
      if (groupType == 1)
      {
        if (!this->m_Param.m_pGuild)
        {
          continue;
        }

        const unsigned int guildMasterSerial = CGuild::GetGuildMasterSerial(this->m_Param.m_pGuild);
        _guild_member_info *member =
          CGuild::GetMemberFromSerial(this->m_Param.m_pGuild, guildMasterSerial);
        if (!member || !member->pPlayer)
        {
          continue;
        }
        groupPlayer = member->pPlayer;
      }
      else if (groupType == 2)
      {
        const int raceCode = CPlayerDB::GetRaceCode(&this->m_Param);
        CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
        const unsigned int bossSerial = CPvpUserAndGuildRankingSystem::GetCurrentRaceBossSerial(ranking, raceCode, 0);
        groupPlayer = GetPtrPlayerFromSerial(g_Player, MAX_PLAYER, bossSerial);
      }
    }
    else
    {
      if (!this->m_pPartyMgr->IsPartyMode())
      {
        continue;
      }
      groupPlayer = &g_Player[this->m_pPartyMgr->m_pPartyBoss->m_id.wIndex];
    }

    if (groupPlayer)
    {
      if (groupPlayer->m_GroupTargetObject[groupType].pObject)
      {
        if (groupPlayer->m_GroupTargetObject[groupType].pObject->m_pCurMap == this->m_pCurMap
            && groupPlayer->m_GroupTargetObject[groupType].pObject->m_wMapLayerIndex == this->m_wMapLayerIndex)
        {
          this->m_GroupTargetObject[groupType].pObject = groupPlayer->m_GroupTargetObject[groupType].pObject;
          this->m_GroupTargetObject[groupType].byKind = groupPlayer->m_GroupTargetObject[groupType].byKind;
          this->m_GroupTargetObject[groupType].byID = groupPlayer->m_GroupTargetObject[groupType].byID;
          this->m_GroupTargetObject[groupType].dwSerial = groupPlayer->m_GroupTargetObject[groupType].dwSerial;
          SendMsg_SetGroupTargetObjectResult(0, static_cast<char>(groupType));
        }
        groupPlayer = nullptr;
      }
      else if (this->m_GroupTargetObject[groupType].pObject)
      {
        CPlayer::__target::init(&this->m_GroupTargetObject[groupType]);
        SendMsg_ReleaseGroupTargetObjectResult(static_cast<unsigned __int8>(groupType));
      }
    }
  }
}

void CPlayer::CheckGroupMapPoint()
{
  CPlayer *groupPlayer = nullptr;
  const unsigned int loopTime = GetLoopTime();

  for (int groupType = 0; groupType < 3; ++groupType)
  {
    if (groupType)
    {
      if (groupType == 1)
      {
        if (!this->m_Param.m_pGuild)
        {
          continue;
        }

        const unsigned int guildMasterSerial = CGuild::GetGuildMasterSerial(this->m_Param.m_pGuild);
        _guild_member_info *member =
          CGuild::GetMemberFromSerial(this->m_Param.m_pGuild, guildMasterSerial);
        if (!member || !member->pPlayer)
        {
          continue;
        }
        groupPlayer = member->pPlayer;
      }
      else if (groupType == 2)
      {
        const int raceCode = CPlayerDB::GetRaceCode(&this->m_Param);
        CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
        const unsigned int bossSerial = CPvpUserAndGuildRankingSystem::GetCurrentRaceBossSerial(ranking, raceCode, 0);
        groupPlayer = GetPtrPlayerFromSerial(g_Player, MAX_PLAYER, bossSerial);
      }
    }
    else
    {
      if (!this->m_pPartyMgr->IsPartyMode())
      {
        continue;
      }
      groupPlayer = &g_Player[this->m_pPartyMgr->m_pPartyBoss->m_id.wIndex];
    }

    if (groupPlayer)
    {
      const unsigned __int8 mapCode = static_cast<unsigned __int8>(this->m_pCurMap->m_nMapCode);
      if (mapCode == groupPlayer->m_byGroupMapPointMapCode[groupType]
          && this->m_wMapLayerIndex == groupPlayer->m_wGroupMapPointLayerIndex[groupType])
      {
        const unsigned int elapsed = loopTime - groupPlayer->m_dwLastGroupMapPointTime[groupType];
        if (elapsed < 0xEA60)
        {
          const unsigned __int8 remain = static_cast<unsigned __int8>((60000 - elapsed) / 1000);
          if (remain)
          {
            SendMsg_SetGroupMapPoint(
              0,
              static_cast<char>(groupType),
              static_cast<char>(groupPlayer->m_byGroupMapPointMapCode[groupType]),
              groupPlayer->m_fGroupMapPoint[groupType],
              remain);
          }
        }
      }
      groupPlayer = nullptr;
    }
  }
}

void CPlayer::pc_Revival(bool bUseableJade)
{
  (void)bUseableJade;

  if (!this->m_pUserDB)
  {
    return;
  }

  unsigned __int8 resultCode = 0;
  CMapData *intoMap = nullptr;
  float revivePos[3]{};

  if (GetCurSecNum() == static_cast<unsigned int>(-1) || this->m_bMapLoading)
  {
    resultCode = 5;
  }
  else if (this->m_bCorpse)
  {
    if (this->m_pCurMap->m_pMapSet->m_nMapType == 1)
    {
      if (this->m_pDHChannel)
      {
        _dh_player_mgr *playerInfo = CDarkHoleChannel::GetPlayerInfo(this->m_pDHChannel, this->m_dwObjSerial);
        if (playerInfo)
        {
          intoMap = playerInfo->LastPos.pMap;
          memcpy_0(revivePos, playerInfo->LastPos.fPos, sizeof(revivePos));
          CDarkHoleChannel::ClearMember(this->m_pDHChannel, this, 0, nullptr);
        }
      }
    }

    if (!intoMap)
    {
      intoMap = GetBindMap(revivePos, 0);
    }
    if (!intoMap)
    {
      resultCode = 2;
    }
  }
  else
  {
    resultCode = 1;
  }

  if (!resultCode)
  {
    if (intoMap)
    {
      this->m_bCorpse = 0;
      this->m_byModeType = 0;
      this->m_byMoveType = 1;
      const unsigned int maxHP = GetMaxHP();
      SetHP(maxHP, true);
      const int maxFP = GetMaxFP();
      SetFP(maxFP, true);
      const int maxSP = GetMaxSP();
      SetSP(maxSP, true);
      OutOfMap(intoMap, 0, 2u, revivePos);
    }
    else
    {
      pc_NuclearAfterEffect();
      CNuclearBombMgr *manager = CNuclearBombMgr::Instance();
      CNuclearBombMgr::CheckNuclearState(manager, this);
    }
  }

  SendMsg_Revival(resultCode, true);
}

void CPlayer::pc_ExitWorldRequest()
{
  if (!this->m_bOper)
  {
    return;
  }

  if (this->m_bLoad && this->m_bOper)
  {
    TimeLimitMgr::Pop_Data(
      g_Main.m_pTimeLimitMgr,
      this->m_pUserDB->m_dwAccountSerial,
      this->m_id.wIndex);
  }

  this->m_bOper = 0;
  if (this->m_pmTrd.bDTradeMode)
  {
    CPlayer *dst = nullptr;
    if (DTradeEqualPerson(this, &dst))
    {
      dst->m_pmTrd.Init();
      SendMsg_DTradeCloseInform(dst, 0);
    }
    this->m_pmTrd.Init();
  }

  SendMsg_ExitWorldResult(0);
}

void CPlayer::pc_LinkBoardRequest()
{
  if (!this->m_bLinkBoardDownload)
  {
    SendMsg_LinkBoardDownloadResult();
    this->m_bLinkBoardDownload = 1;
  }
}

char CPlayer::IntoMap(unsigned __int8 byMapInMode)
{
  if (this->m_pCurMap->m_pMapSet->m_nMapType == 1
      && !this->m_pCurMap->m_ls[this->m_wMapLayerIndex].IsActiveLayer())
  {
    return 0;
  }

  if (byMapInMode == 6)
  {
    CGuildBattleController *controller = CGuildBattleController::Instance();
    (void)CGuildBattleController::Start(controller, this);
  }

  const unsigned int secIndex = static_cast<unsigned int>(CalcSecIndex());
  _sec_info *secInfo = this->m_pCurMap->GetSecInfo();
  if (secIndex >= static_cast<unsigned int>(secInfo->m_nSecNum))
  {
    return 0;
  }

  this->m_pCurMap->EnterMap(this, secIndex);
  SetCurSecNum(secIndex);
  SendData_PartyMemberPos();

  if (this->m_pCurMap == g_TransportShip[static_cast<int>(CPlayerDB::GetRaceCode(&this->m_Param))].m_pLinkShipMap)
  {
    const int raceCode = CPlayerDB::GetRaceCode(&this->m_Param);
    CTransportShip::EnterMember(&g_TransportShip[raceCode], this);
  }

  SetStateFlag();
  CheckPos_Region();
  return 1;
}

char CPlayer::Create()
{
  if (!this->m_bLoad || this->m_bLive || !this->m_pUserDB)
  {
    return 0;
  }

  const DWORD nowTime = timeGetTime();
  this->m_dwObjSerial = this->m_id.dwSerial;

  _character_create_setdata createData;
  createData.m_pMap = CMapOperation::GetMap(&g_MapOper, this->m_Param.m_dbChar.m_sStartMapCode);
  if (createData.m_pMap->m_pMapSet->m_nMapType == 2)
  {
    createData.m_nLayerIndex = static_cast<int>(this->m_Param.m_dbChar.m_byRaceSexCode) >> 1;
  }
  else
  {
    createData.m_nLayerIndex = 0;
  }
  createData.m_fStartPos[0] = this->m_Param.m_dbChar.m_fStartPos[0];
  createData.m_fStartPos[1] = this->m_Param.m_dbChar.m_fStartPos[1];
  createData.m_fStartPos[2] = this->m_Param.m_dbChar.m_fStartPos[2];
  createData.m_pRecordSet =
    CRecordData::GetRecord(&g_Main.m_tblPlayer, this->m_Param.m_dbChar.m_byRaceSexCode);

  if (!CCharacter::Create(&createData))
  {
    const int posX = static_cast<int>(this->m_fCurPos[0]);
    const int posY = static_cast<int>(this->m_fCurPos[1]);
    const int posZ = static_cast<int>(this->m_fCurPos[2]);
    const char *mapCode = this->m_pCurMap->m_pMapSet->m_strCode;
    const char *charName = CPlayerDB::GetCharNameA(&this->m_Param);
    char buffer[132]{};
    sprintf(buffer, "Close.. %s: Create() Map(%s) Pos(%d, %d, %d)", charName, mapCode, posX, posY, posZ);
    CNetworkEX::Close(&g_Network, 0, this->m_ObjID.m_wIndex, false, buffer);
    return 0;
  }

  if (this->m_pUserDB->m_byUILock == 2)
  {
    this->m_bOper = 1;
  }

  this->m_byModeType = 0;
  this->m_byMoveType = 1;
  this->m_byStandType = 0;
  this->m_byHSKQuestCode = 100;
  this->m_nHSKPvpPoint = 0;
  this->m_wKillPoint = 0;
  this->m_wDiePoint = 0;
  this->m_byCristalBattleDBInfo = 3;
  memcpy_0(this->m_fLastRecvPos, this->m_fCurPos, sizeof(this->m_fLastRecvPos));
  this->m_byLastRecvMapIndex = static_cast<unsigned __int8>(this->m_pCurMap->m_pMapSet->m_dwIndex);
  CheckPos_Region();
  this->m_wVisualVer = 1;
  this->m_dwLastState = 0;
  this->m_pBeforeDungeonMap = nullptr;
  this->m_nLastBeatenPart = 0;
  this->m_pUsingUnit = nullptr;
  this->m_pParkingUnit = nullptr;
  this->m_dwUnitViewOverTime = static_cast<unsigned int>(-1);
  this->m_byUsingWeaponPart = 0;
  this->m_pRecalledAnimusItem = nullptr;
  this->m_pRecalledAnimusChar = nullptr;
  this->m_dwLastRecallTime = 0;
  this->m_byNextRecallReturn = 0xFF;
  this->m_wTimeFreeRecallSerial = 0xFFFF;
  this->m_pSiegeItem = nullptr;
  this->m_dwLastTakeItemTime = 0;

  CMyTimer::TermTimeRun(&this->m_tmrIntervalSec);
  CMyTimer::TermTimeRun(&this->m_tmrBilling);
  CMyTimer::TermTimeRun(&this->m_tmrGroupTargeting);
  if (IsApplyPcbangPrimium())
  {
    CMyTimer::TermTimeRun(&this->m_tmrPremiumPVPInform);
  }

  _RENAME_POTION_USE_INFO::Init(&this->m_ReNamePotionUseInfo);
  GetLocalTime(&this->m_tmLoginTime);
  GetLocalTime(&this->m_tmCalcTime);
  this->m_dwUMWHLastTime = GetLoopTime();

  const int currentMinuteSeconds = 60 * GetCurrentMin();
  const int currentSec = GetCurrentSec();
  CMyTimer::BeginTimerAddLapse(&this->m_tmrEffectStartTime, 0x36EE80u, 1000 * (currentSec + currentMinuteSeconds));

  CPotionParam::Init(&this->m_PotionParam, this);
  if (CPlayerDB::GetHP(&this->m_Param))
  {
    this->m_bCorpse = 0;
  }
  else
  {
    this->m_bCorpse = 1;
  }

  CalcDefTol();
  PastWhisperInit();
  SetHaveEffect(1);
  CQuestMgr::InitMgr(&this->m_QuestMgr, this, &this->m_Param.m_QuestDB);
  ItemCombineMgr::InitMgr(&this->m_ItemCombineMgr, this);

  CPlayer::__target::init(&this->m_TargetObject);
  for (int j = 0; j < 3; ++j)
  {
    CPlayer::__target::init(&this->m_GroupTargetObject[j]);
  }

  this->m_wPointRate_PartySend[0] = 10000;
  this->m_wPointRate_PartySend[1] = 10000;
  this->m_wPointRate_PartySend[2] = 10000;
  this->m_bMineMode = 0;
  this->m_dwMineNextTime = static_cast<unsigned int>(-1);

  _ATTACK_DELAY_CHECKER::Init(&this->m_AttDelayChker);
  for (int k = 0; k < 10; ++k)
  {
    if (this->m_pUserDB->m_AvatorData.dbSFDelay.EFF[k].byEffectCode != 0xFF)
    {
      this->m_AttDelayChker.EFF[k].byEffectCode = this->m_pUserDB->m_AvatorData.dbSFDelay.EFF[k].byEffectCode;
      this->m_AttDelayChker.EFF[k].wEffectIndex = this->m_pUserDB->m_AvatorData.dbSFDelay.EFF[k].wEffectIndex;
      this->m_AttDelayChker.EFF[k].dwNextTime = nowTime + this->m_pUserDB->m_AvatorData.dbSFDelay.EFF[k].dwNextTime;
    }
    if (this->m_pUserDB->m_AvatorData.dbSFDelay.MAS[k].byEffectCode != 0xFF)
    {
      this->m_AttDelayChker.MAS[k].byEffectCode = this->m_pUserDB->m_AvatorData.dbSFDelay.MAS[k].byEffectCode;
      this->m_AttDelayChker.MAS[k].byMastery = this->m_pUserDB->m_AvatorData.dbSFDelay.MAS[k].byMastery;
      this->m_AttDelayChker.MAS[k].dwNextTime = nowTime + this->m_pUserDB->m_AvatorData.dbSFDelay.MAS[k].dwNextTime;
    }
  }

  SendMsg_SFDelayRequest();
  const DWORD potionTime = timeGetTime();
  SendMsg_PotionDelayTime(this->m_pUserDB->m_AvatorData.dbPotionNextUseTime.dwPotionNextUseTime, potionTime);

  this->m_nChaosMode = 0;
  this->m_dwChaosModeTime10Per = 0;
  this->m_dwChaosModeEndTime = 0;
  CEquipItemSFAgent::Init(&this->EquipItemSFAgent, this);
  this->m_bSnowMan = 0;

  char *charNameW = CPlayerDB::GetCharNameW(&this->m_Param);
  const unsigned int charSerial = CPlayerDB::GetCharSerial(&this->m_Param);
  this->m_byBattleTournamentGrade =
    CBattleTournamentInfo::GetWinnerGrade(&g_Main.m_BattleTournamentInfo, charSerial, charNameW);

  this->m_fUnitPv_AttFc = FLOAT_1_0;
  this->m_fUnitPv_DefFc = FLOAT_1_0;
  this->m_fUnitPv_RepPr = FLOAT_1_0;
  if (this->m_Param.m_pClassData->m_dwIndex == 49
      && this->m_Param.m_pClassHistory[0]
      && this->m_Param.m_pClassHistory[0]->m_dwIndex == 45)
  {
    this->m_fUnitPv_AttFc = FLOAT_1_1;
    CNationSettingManager *manager = CTSingleton<CNationSettingManager>::Instance();
    CNationSettingManager::SetUnitPassiveValue(manager, &this->m_fUnitPv_DefFc);
    this->m_fUnitPv_RepPr = FLOAT_0_60000002;
  }

  memcpy_0(this->m_fBeforeDungeonPos, this->m_fCurPos, sizeof(this->m_fBeforeDungeonPos));
  this->m_pBeforeDungeonMap = this->m_pCurMap;

  if (this->m_pUserDB)
  {
    CUserDB::StartFieldMode(this->m_pUserDB);
    const unsigned int radarDelay = 1000 * this->m_pUserDB->m_AvatorData.dbAvator.m_dwRadarDelayTime;
    CRadarItemMgr::Init(&this->m_pUserDB->m_RadarItemMgr, radarDelay);
    MiningTicket::SetLastMentalTicket(
      &this->m_MinigTicket,
      this->m_pUserDB->m_AvatorData.dbAvator.m_dwTakeLastMentalTicket);
    MiningTicket::SetLastCriTicket(
      &this->m_MinigTicket,
      this->m_pUserDB->m_AvatorData.dbAvator.m_dwTakeLastCriTicket);
    MiningTicket::_AuthKeyTicket::Set(
      &this->m_dwRaceBuffClearKey,
      this->m_pUserDB->m_AvatorData.dbSupplement.dwRaceBuffClear);

    CMgrAvatorItemHistory::GetNewFileName(
      &CPlayer::s_MgrItemHistory,
      this->m_pUserDB->m_dwSerial,
      this->m_szItemHistoryFileName);
    CMgrAvatorLvHistory::GetNewFileName(
      &CPlayer::s_MgrLvHistory,
      this->m_pUserDB->m_dwSerial,
      this->m_szLvHistoryFileName);

    CMgrAvatorItemHistory::have_item(
      &CPlayer::s_MgrItemHistory,
      this->m_ObjID.m_wIndex,
      CPlayerDB::GetCharNameA(&this->m_Param),
      &this->m_pUserDB->m_AvatorData,
      &this->m_pUserDB->m_AvatorData_bk,
      this->m_pUserDB->m_szAccountID,
      this->m_pUserDB->m_dwAccountSerial,
      this->m_byUserDgr,
      this->m_pUserDB->m_ipAddress,
      this->m_dwExpRate,
      true,
      this->m_szItemHistoryFileName);

    CMgrAvatorLvHistory::start_mastery(
      &CPlayer::s_MgrLvHistory,
      this->m_ObjID.m_wIndex,
      CPlayerDB::GetCharNameA(&this->m_Param),
      CPlayerDB::GetLevel(&this->m_Param),
      CPlayerDB::GetExp(&this->m_Param),
      this->m_dwExpRate,
      this->m_Param.m_byPvPGrade,
      this->m_nMaxPoint,
      &this->m_pmMst,
      this->m_szLvHistoryFileName);

    CUserDB::WriteLog_CharSelect(this->m_pUserDB);
  }

  if (this->m_pUserDB->m_AvatorData.dbAvator.m_dwDalant
      != this->m_pUserDB->m_AvatorData_bk.dbAvator.m_dwDalant)
  {
    SendMsg_ExchangeMoneyResult(0);
  }

  CMoveMapLimitManager *moveMapLimit = CMoveMapLimitManager::Instance();
  CMoveMapLimitManager::LogIn(moveMapLimit, this);

  this->m_pBindMapData =
    CMapOperation::GetMap(&g_MapOper, this->m_pUserDB->m_AvatorData.dbAvator.m_szBindMapCode);
  if (this->m_pBindMapData)
  {
    this->m_pBindDummyData =
      CDummyPosTable::GetRecord(
        &this->m_pBindMapData->m_tbBindDumPos,
        this->m_pUserDB->m_AvatorData.dbAvator.m_szBindDummy);
    if (!this->m_pBindDummyData)
    {
      this->m_pBindMapData = nullptr;
    }
  }

  if (this->m_pUserDB && !this->m_pBindMapData)
  {
    CUserDB::Update_Bind(this->m_pUserDB, "0", "0", 0);
  }

  SendMsg_EconomyHistoryInform();
  SendMsg_EconomyRateInform(1);

  ++CPlayer::s_nLiveNum;
  ++CPlayer::s_nRaceNum[static_cast<int>(CPlayerDB::GetRaceCode(&this->m_Param))];

  _WA_AVATOR_CODE waCode{};
  memcpy_0(&waCode, &this->m_id, 6u);
  char *playerNameW = CPlayerDB::GetCharNameW(&this->m_Param);
  strcpy_0(waCode.m_wszName, playerNameW);
  wa_EnterWorld(&waCode, this->m_ObjID.m_wIndex);

  _set_db_sf_effect(&this->m_pUserDB->m_AvatorData.dbSfcont);
  CHolyStoneSystem::SendMsg_HolyStoneSystemState(&g_HolySys, this->m_ObjID.m_wIndex);

  this->m_nAddDfnMstByClass = 0;
  for (int i = 0; i < 4; ++i)
  {
    _class_fld *effectRecord = *this->m_Param.m_ppHistoryEffect[i];
    if (!effectRecord)
    {
      break;
    }
    this->m_nAddDfnMstByClass += *reinterpret_cast<int *>(reinterpret_cast<char *>(effectRecord) + 1476);
  }

  if (this->m_Param.m_pGuild && _effect_parameter::GetEff_Have(&this->m_EP, 50) <= 0.0)
  {
    _guild_member_info *member = CGuild::LoginMember(this->m_Param.m_pGuild, this->m_dwObjSerial, this);
    if (member)
    {
      this->m_Param.m_pGuildMemPtr = member;
      CGuild::SendMsg_GuildMemberLogin(
        this->m_Param.m_pGuild,
        this->m_dwObjSerial,
        this->m_wRegionMapIndex,
        this->m_wRegionIndex);

      CGuildBattleController *controller = CGuildBattleController::Instance();
      CGuildBattleController::LogIn(controller, this);

      if (member->byClassInGuild == 2)
      {
        const unsigned __int8 raceCode = CPlayerDB::GetRaceCode(&this->m_Param);
        CGuildBattleController::SendPossibleBattleGuildListFirst(
          controller,
          this->m_ObjID.m_wIndex,
          raceCode);
      }

      if (member->byClassInGuild == 2)
      {
        const unsigned __int8 grade = CGuild::GetGrade(this->m_Param.m_pGuild);
        CGuildMasterEffect *effect = CGuildMasterEffect::GetInstance();
        CGuildMasterEffect::in_player(effect, this, grade);
      }
    }
    else
    {
      this->m_Param.m_pGuild = nullptr;
    }
  }

  if (!this->m_bInGuildBattle
      && this->m_pCurMap->m_pMapSet->m_nRaceVillageCode < 3
      && static_cast<unsigned int>(CPlayerDB::GetRaceCode(&this->m_Param))
        != this->m_pCurMap->m_pMapSet->m_nRaceVillageCode
      && this->m_byUserDgr >= 2u)
  {
    this->m_bObserver = 1;
  }

  SetShapeAllBuffer();
  this->m_byDefMatCount = 0;
  this->m_dwLastSetPointTime = GetLoopTime();

  const int raceCode = CPlayerDB::GetRaceCode(&this->m_Param);
  CTransportShip::SendMsg_TransportShipState(&g_TransportShip[raceCode], this->m_ObjID.m_wIndex);
  if (this->m_pCurMap == g_TransportShip[raceCode].m_pLinkShipMap)
  {
    CTransportShip::ReEnterMember(&g_TransportShip[raceCode], this);
  }

  if (!this->m_pUserDB->m_byUserDgr)
  {
    CBillingManager *billing = CTSingleton<CBillingManager>::Instance();
    CBillingManager::Login(billing, this->m_pUserDB);
  }

  SenseState();
  this->m_nCheckMovePacket = 0;
  this->m_bCheckMovePacket = 0;
  CHolyStoneSystem::SendHolyStoneHP(&g_HolySys, this);
  this->m_byStoneMapMoveInfo = 0;
  this->m_dwPatriarchAppointTime = static_cast<unsigned int>(-1);
  this->m_byPatriarchAppointPropose = static_cast<unsigned __int8>(-1);
  this->m_byBattleMode = 0;
  this->m_dwBattleTime = 0;

  if (this->m_pUserDB)
  {
    CMgrAccountLobbyHistory::player_create(
      &CUserDB::s_MgrLobbyHistory,
      this->m_bFirstStart,
      &this->m_pUserDB->m_AvatorData,
      this->m_pUserDB->m_szLobbyHistoryFileName);
  }

  return 1;
}

void CPlayer::CreateComplete()
{
  if (!this->m_bLive || this->m_bCreateComplete)
  {
    return;
  }

  this->m_bCreateComplete = 1;
  CheckUnitCutTime();
  if (CPlayerDB::IsClassChangeableLv(&this->m_Param))
  {
    SendMsg_ChangeClassCommand();
  }
  CalcEquipSpeed();
  CPvpOrderView::Notify_OrderView(&this->m_kPvpOrderView, this->m_ObjID.m_wIndex);

  for (int j = 0; j < 30; ++j)
  {
    _QUEST_DB_BASE::_LIST *quest = &this->m_Param.m_QuestDB.m_List[j];
    if (quest->byQuestType != 0xFF)
    {
      bool isAllEmpty = true;
      for (int k = 0; k < 3; ++k)
      {
        if (quest->wNum[k] != 0xFFFF)
        {
          isAllEmpty = false;
          break;
        }
      }

      if (isAllEmpty)
      {
        _base_fld *record = CRecordData::GetRecord(CQuestMgr::s_tblQuest, quest->wIndex);
        if (*reinterpret_cast<unsigned int *>(&record[13].m_strCode[60])
            || *reinterpret_cast<unsigned int *>(&record[1].m_strCode[24]))
        {
          SendMsg_SelectQuestReward(j);
        }
        else
        {
          Emb_CompleteQuest(j, 0xFFu, 0xFFu);
        }
      }
    }
  }

  ItemCombineMgr::OnPlayerCreateCompleteProc(&this->m_ItemCombineMgr);

  if (CRadarItemMgr::GetDelayTime(&this->m_pUserDB->m_RadarItemMgr))
  {
    const unsigned int delay = CRadarItemMgr::GetDelayTime(&this->m_pUserDB->m_RadarItemMgr);
    SendMsg_RadarDelayTime(delay);
  }

  this->m_zLastTol[0] = GetFireTol();
  this->m_zLastTol[1] = GetWaterTol();
  this->m_zLastTol[2] = GetSoilTol();
  this->m_zLastTol[3] = GetWindTol();
  SendMsg_AlterTol();
  CheckPosInTown();

  for (int j = 0; j < MAX_TRAP; ++j)
  {
    CTrap *trap = &g_Trap[j];
    if (trap->m_bLive && !trap->m_pMaster && trap->m_dwMasterSerial == this->m_dwObjSerial)
    {
      if (!_TRAP_PARAM::PushItem(&this->m_pmTrp, trap, trap->m_dwObjSerial))
      {
        break;
      }
      CTrap::MasterReStart(trap, this);
    }
  }
  if (this->m_pmTrp.m_nCount > 0)
  {
    SendMsg_MadeTrapNumInform(this->m_pmTrp.m_nCount);
  }

  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    __TEMP_WAIT_TOWER *entry = &CGuardTower::s_Temp[j];
    if (entry->dwMasterSerial == this->m_dwObjSerial)
    {
      entry->dwMasterSerial = static_cast<unsigned int>(-1);
      const int itemIndex = entry->byItemIndex;
      const unsigned __int8 bagNum = CPlayerDB::GetBagNum(&this->m_Param);
      if (itemIndex < 20 * bagNum)
      {
        _STORAGE_LIST::_db_con *item = &this->m_Param.m_dbInven.m_pStorageList[itemIndex];
        if (item->m_bLoad && item->m_byTableCode == 25 && !item->m_bLock)
        {
          _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblItemData[25], item->m_wItemIndex);
          if (record)
          {
            const int raceSexCode = CPlayerDB::GetRaceSexCode(&this->m_Param);
            if (record[3].m_strCode[raceSexCode + 52] == 49)
            {
              const bool quick = entry->bComplete;
              const unsigned __int8 raceCode = CPlayerDB::GetRaceCode(&this->m_Param);
              CGuardTower *tower =
                CreateGuardTower(entry->pMap, 0, entry->fPos, item, this, raceCode, quick);
              if (tower)
              {
                if (IsOtherTowerNear(this, entry->fPos, tower))
                {
                  CGuardTower::Destroy(tower, 0, 0);
                }
                else
                {
                  _STORAGE_LIST::_storage_con::lock(item, 1);
                  _TOWER_PARAM::PushList(&this->m_pmTwr, item, tower);
                  ++this->m_pmTwr.m_nCount;
                  SendMsg_TowerContinue(item->m_wSerial, tower);
                }
              }
            }
          }
        }
      }
    }
  }

  const int raceCode = CPlayerDB::GetRaceCode(&this->m_Param);
  CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
  if (CPvpUserAndGuildRankingSystem::IsCurrentRaceBossGroup(ranking, raceCode, this->m_dwObjSerial))
  {
    SendMsg_RaceTopInform(1);
  }

  if (this->m_bFirstStart)
  {
    const int level = GetLevel();
    char *levelStr = cvt_string(level);
    Emb_CreateQuestEvent(quest_happen_type_lv, levelStr);
    CGoldenBoxItemMgr *golden = CGoldenBoxItemMgr::Instance();
    if (CGoldenBoxItemMgr::Get_Event_Status(golden) == 2)
    {
      CGoldenBoxItemMgr *countMgr = CGoldenBoxItemMgr::Instance();
      if (CGoldenBoxItemMgr::Get_StarterBox_Count(countMgr))
      {
        bool inserted = false;
        for (int m = 0; m < 2; ++m)
        {
          CGoldenBoxItemMgr *boxMgr = CGoldenBoxItemMgr::Instance();
          char *itemCode = CGoldenBoxItemMgr::GetStarterBoxCode(boxMgr, m);
          CGoldenBoxItemMgr *insertMgr = CGoldenBoxItemMgr::Instance();
          if (CGoldenBoxItemMgr::StarterBox_InsertToInven(insertMgr, this, itemCode))
          {
            inserted = true;
          }
        }
        if (inserted)
        {
          CGoldenBoxItemMgr *setMgr = CGoldenBoxItemMgr::Instance();
          CGoldenBoxItemMgr::Set_StarterBox_Count(setMgr, 1u, 0);
        }
      }
    }
  }

  CVoteSystem *voteSystem = &g_VoteSys[static_cast<int>(CPlayerDB::GetRaceCode(&this->m_Param))];
  if (voteSystem->m_bActive)
  {
    if (voteSystem->m_bPunishment)
    {
      const unsigned int serial = CPlayerDB::GetCharSerial(&this->m_Param);
      const int voteRace = CPlayerDB::GetRaceCode(&this->m_Param);
      CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
      if (CPvpUserAndGuildRankingSystem::GetBossType(rank, voteRace, serial) == 0xFF)
      {
        CVoteSystem::SendMsg_StartedVoteInform(voteSystem, this->m_ObjID.m_wIndex, this->m_dwObjSerial, 1);
      }
      else
      {
        CVoteSystem::SendMsg_StartedVoteInform(voteSystem, this->m_ObjID.m_wIndex, this->m_dwObjSerial, 0);
      }
    }
    else
    {
      CVoteSystem::SendMsg_StartedVoteInform(voteSystem, this->m_ObjID.m_wIndex, this->m_dwObjSerial, 0);
    }
  }

  if (CHolyStoneSystem::GetSceneCode(&g_HolySys) == 1)
  {
    _QUEST_CASH *questCash = CHolyStoneSystem::FindStoragedQuestCash(&g_HolySys, this->m_dwObjSerial);
    if (questCash)
    {
      RecvHSKQuest(
        questCash->byQuestType,
        questCash->byCristalBattleDBInfo,
        questCash->nPvpPoint,
        questCash->wKillPoint,
        questCash->wDiePoint,
        questCash->byHSKTime + 1);
    }
    else if (GetLevel() >= 25)
    {
      const unsigned __int8 numOfTime = CHolyStoneSystem::GetNumOfTime(&g_HolySys);
      const int randVal = rand();
      RecvHSKQuest(randVal % 2, 0, 0, 0, 0, numOfTime);
    }
  }

  for (int j = 0; j < 50; ++j)
  {
    _BUDDY_LIST::__list *buddy = &this->m_pmBuddy.m_List[j];
    if (_BUDDY_LIST::__list::fill(buddy))
    {
      for (int n = 0; n < MAX_PLAYER; ++n)
      {
        CPlayer *ptr = &g_Player[n];
        if (ptr->m_bLive
            && ptr != this
            && _effect_parameter::GetEff_Have(&ptr->m_EP, 50) <= 0.0
            && ptr->m_dwObjSerial == buddy->dwSerial)
        {
          if (_BUDDY_LIST::IsBuddy(&ptr->m_pmBuddy, this->m_dwObjSerial))
          {
            char *nameW = CPlayerDB::GetCharNameW(&ptr->m_Param);
            _BUDDY_LIST::__list::ON(buddy, nameW, ptr);
            char *selfNameW = CPlayerDB::GetCharNameW(&this->m_Param);
            _BUDDY_LIST::SearchBuddyLogin(&ptr->m_pmBuddy, this, this->m_dwObjSerial, selfNameW);
            SendMsg_BuddyLoginInform(ptr, this->m_dwObjSerial, this->m_wRegionMapIndex, this->m_wRegionIndex);
          }
          break;
        }
      }
    }
  }

  CUserDB *userDb = this->m_pUserDB;
  TimeLimitMgr::InsertPlayerStatus(
    g_Main.m_pTimeLimitMgr,
    this->m_id.wIndex,
    userDb->m_dwAccountSerial,
    userDb->m_AvatorData.dbTimeLimitInfo.byTLStatus,
    userDb->m_AvatorData.dbTimeLimitInfo.dwFatigue,
    userDb->m_AvatorData.dbTimeLimitInfo.dwLastLogoutTime,
    userDb->m_BillingInfo.bAgeLimit);
  TimeLimitMgr::CheckPlayerStatus(
    g_Main.m_pTimeLimitMgr,
    this->m_id.wIndex,
    this->m_pUserDB->m_AvatorData.dbTimeLimitInfo.dwLastLogoutTime,
    &this->m_pUserDB->m_AvatorData.dbTimeLimitInfo.byTLStatus,
    &this->m_pUserDB->m_AvatorData.dbTimeLimitInfo.dwFatigue);
  this->m_pUserDB->m_bDataUpdate = 1;
  SendMsg_TLStatusInfo(
    this->m_pUserDB->m_AvatorData.dbTimeLimitInfo.dwFatigue,
    this->m_pUserDB->m_AvatorData.dbTimeLimitInfo.byTLStatus);
  SendMsg_Init_Action_Point();

  CUnmannedTraderController *trader = CUnmannedTraderController::Instance();
  CUnmannedTraderController::CompleteCreate(trader, this->m_ObjID.m_wIndex);

  CDarkHoleChannel *channel =
    CDarkHoleDungeonQuest::SearchOncePlayedChannel(&g_DarkHoleQuest, this->m_dwObjSerial);
  if (channel)
  {
    SendMsg_ReEnterAsk(channel->m_wChannelIndex, channel->m_dwChannelSerial);
  }

  if (GetLevel() >= 25)
  {
    _QUEST_CASH_OTHER *questCashOther = nullptr;
    unsigned __int8 stoneMapMoveInfo = 0;
    this->m_byStoneMapMoveInfo = 0;

    if (g_HolySys.m_bScheduleCodePre == 1)
    {
      questCashOther = CHolyStoneSystem::PopStoredQuestCash_Other(&g_HolySys, this->m_dwObjSerial);
      if (questCashOther)
      {
        stoneMapMoveInfo = questCashOther->byStoneMapMoveInfo;
      }
      else
      {
        stoneMapMoveInfo = 1;
      }
      CHolyStoneSystem::SendMsg_NoticeNextQuest(&g_HolySys, this->m_ObjID.m_wIndex, stoneMapMoveInfo);
    }
    else if (CHolyStoneSystem::GetSceneCode(&g_HolySys) == 1)
    {
      questCashOther = CHolyStoneSystem::PopStoredQuestCash_Other(&g_HolySys, this->m_dwObjSerial);
      if (questCashOther)
      {
        stoneMapMoveInfo = questCashOther->byStoneMapMoveInfo;
      }
      else
      {
        stoneMapMoveInfo = 1;
      }
      SendMsg_MoveToOwnStoneMapInform(stoneMapMoveInfo);
    }

    this->m_byStoneMapMoveInfo = stoneMapMoveInfo;
    if (g_HolySys.bFreeMining && CHolyStoneSystem::IsMentalPass(&g_HolySys))
    {
      const unsigned __int8 numOfTime = CHolyStoneSystem::GetNumOfTime(&g_HolySys);
      const unsigned __int8 startHour = CHolyStoneSystem::GetStartHour(&g_HolySys);
      const unsigned __int8 startDay = CHolyStoneSystem::GetStartDay(&g_HolySys);
      const unsigned __int8 startMonth = CHolyStoneSystem::GetStartMonth(&g_HolySys);
      const unsigned __int16 startYear = CHolyStoneSystem::GetStartYear(&g_HolySys);
      UpdateLastCriTicket(startYear, startMonth, startDay, startHour, numOfTime);
    }
  }

  if (this->m_pUserDB->m_byUserDgr == 2 && this->m_bSpyGM)
  {
    char buffer[136]{};
    sprintf(buffer, "SPY GM !!");
    SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, buffer, 0xFFu, nullptr);
  }

  PatriarchElectProcessor *patriarch = PatriarchElectProcessor::Instance();
  PatriarchElectProcessor::SendMsg_ConnectNewUser(patriarch, this);
  SendMsg_GM_Greeting(g_Main.m_wszGMName, g_Main.m_wszMainGreetingMsg);

  char *raceGreeting = g_Main.m_wszRaceGreetingMsg[static_cast<int>(CPlayerDB::GetRaceCode(&this->m_Param))];
  const int raceGreetingIndex = CPlayerDB::GetRaceCode(&this->m_Param);
  SendMsg_RACE_Greeting(g_Main.m_wszBossName[raceGreetingIndex], raceGreeting);

  if (this->m_Param.m_pGuild && this->m_Param.m_pGuild->m_wszGreetingMsg[0])
  {
    char *guildMasterName = CGuild::GetGuildMasterName(this->m_Param.m_pGuild);
    if (!guildMasterName)
    {
      guildMasterName = this->m_Param.m_pGuild->m_wszName;
    }
    SendMsg_GUILD_Greeting(guildMasterName, this->m_Param.m_pGuild->m_wszGreetingMsg);
  }

  const unsigned __int8 notifyRace = CPlayerDB::GetRaceCode(&this->m_Param);
  CNotifyNotifyRaceLeaderSownerUTaxrate::Notify(&g_Main.m_kEtcNotifyInfo, notifyRace, this->m_ObjID.m_wIndex);

  const int notifyTradeRace = CPlayerDB::GetRaceCode(&this->m_Param);
  CUnmannedTraderController *tradeInfo = CUnmannedTraderController::Instance();
  CUnmannedTraderController::CompleteCreateNotifyTradeInfo(tradeInfo, notifyTradeRace, this->m_ObjID.m_wIndex);

  CMoveMapLimitManager *moveMapLimit = CMoveMapLimitManager::Instance();
  CMoveMapLimitManager::CreateComplete(moveMapLimit, this);

  if (!IsUseReleaseRaceBuffPotion())
  {
    CRaceBuffManager *raceBuff = CRaceBuffManager::Instance();
    CRaceBuffManager::CreateComplete(raceBuff, this);
  }

  _qry_case_select_patriarch_comm query{};
  query.dwSerial = CPlayerDB::GetCharSerial(&this->m_Param);
  const int querySize = _qry_case_select_patriarch_comm::size(&query);
  CMainThread::PushDQSData(&g_Main, 0xFFFFFFFF, nullptr, 0x82u, reinterpret_cast<char *>(&query), querySize);

  const int bossRace = CPlayerDB::GetRaceCode(&this->m_Param);
  CPvpUserAndGuildRankingSystem *bossRank = CPvpUserAndGuildRankingSystem::Instance();
  if (CPvpUserAndGuildRankingSystem::GetCurrentRaceBossSerial(bossRank, bossRace, 0) == this->m_dwObjSerial)
  {
    const int honorRace = CPlayerDB::GetRaceCode(&this->m_Param);
    CHonorGuild *honor = CHonorGuild::Instance();
    CHonorGuild::SendInformChange(honor, honorRace, this->m_ObjID.m_wIndex);
  }

  const int winRateRace = CPlayerDB::GetRaceCode(&this->m_Param);
  CRaceBossWinRate *winRate = CRaceBossWinRate::Instance();
  CRaceBossWinRate::Notify(winRate, winRateRace, this->m_ObjID.m_wIndex);

  int number = 0;
  for (int ii = 0; ii < 3; ++ii)
  {
    number += this->m_pUserDB->m_AvatorData.dbAvator.m_dwRaceBattleRecord[ii]
      * g_HolySys.m_nRaceBattlePoint[ii][1];
  }

  if (number)
  {
    if (number < 0)
    {
      const double pvpPoint = CPlayerDB::GetPvPPoint(&this->m_Param);
      if (std::fabs(static_cast<double>(number)) > pvpPoint)
      {
        number = static_cast<int>(-pvpPoint);
      }
    }
    AlterPvPPoint(static_cast<double>(number), holy_dec, 0xFFFFFFFF);
    SendMsg_RaceBattlePenelty(number, 0);
  }

  CashItemRemoteStore *cashStore = CashItemRemoteStore::Instance();
  CashItemRemoteStore::inform_cashdiscount_event(cashStore, this->m_ObjID.m_wIndex);
  CashItemRemoteStore *cashEvent = CashItemRemoteStore::Instance();
  CashItemRemoteStore::Inform_CashEvent(cashEvent, this->m_ObjID.m_wIndex);
  CashItemRemoteStore *conditionalEvent = CashItemRemoteStore::Instance();
  CashItemRemoteStore::Inform_ConditionalEvent(conditionalEvent, this->m_ObjID.m_wIndex);

  if (IsOverOneDay() && !this->m_bFirstStart)
  {
    double pvpPoint = CPlayerDB::GetPvPPoint(&this->m_Param);
    double alter = pvpPoint * 0.2000000029802322;
    if (pvpPoint < 0.0)
    {
      pvpPoint = 0.0;
    }
    AlterPvPPoint(-alter, logoff_dec, 0xFFFFFFFF);
    SendMsg_RaceBattlePenelty(static_cast<int>(alter), 1u);
  }

  const bool isPremium = IsApplyPcbangPrimium();
  CExchangeEvent *exchange = CExchangeEvent::Instance();
  if (exchange->IsDelete() || CExchangeEvent::Instance()->IsEnable())
  {
    CExchangeEvent::Instance()->DeleteExchangeEventItem(this);
  }

  CExchangeEvent *enableMgr = CExchangeEvent::Instance();
  if (enableMgr->IsEnable())
  {
    if (!enableMgr->IsWait() && isPremium)
    {
      enableMgr->GiveEventItem(this);
    }
  }

  if (this->m_pUserDB && !this->m_bFirstStart)
  {
    const unsigned int limitTime = GetConnectTime_AddBySec(-2592000);
    if (this->m_pUserDB->m_AvatorData_bk.dbAvator.m_dwLastConnTime < limitTime)
    {
      SendMsg_Notify_ExceptFromRaceRanking(1);
    }
  }

  CQuestMgr::CheckFailLoop(&this->m_QuestMgr, 2, nullptr);
  SendMsg_RemainOreRate();
  SendMsg_OreTransferCount();

  if (IsApplyPcbangPrimium())
  {
    CCouponMgr::Init(&this->m_kPcBangCoupon, this->m_ObjID.m_wIndex);
  }

  if (this->m_Param.m_pGuild)
  {
    const unsigned int destroyerSerial = CHolyStoneSystem::GetDestroyerGuildSerial(&g_HolySys);
    if (destroyerSerial == static_cast<unsigned int>(-1))
    {
      SetLastAttBuff(0);
    }
    else
    {
      const unsigned int guildSerial = CPlayerDB::GetGuildSerial(&this->m_Param);
      if (destroyerSerial != guildSerial)
      {
        SetLastAttBuff(0);
      }
    }
  }

  const unsigned int endTime = this->m_pUserDB->m_AvatorData.dbSupplement.dwBufPotionEndTime;
  if (endTime <= GetKorLocalTime())
  {
    CExtPotionBuf::SetExtPotionBufUse(&this->m_PotionBufUse, 0);
  }
  else
  {
    CExtPotionBuf::SetExtPotionBufUse(&this->m_PotionBufUse, 1);
    CExtPotionBuf::SetExtPotionEndTime(&this->m_PotionBufUse, endTime);
    CExtPotionBuf::CalcRemainTime(&this->m_PotionBufUse, this->m_ObjID.m_wIndex, 1);
  }

  CPcBangFavor *pcBang = CPcBangFavor::Instance();
  CPcBangFavor::PcBangDeleteItem(pcBang, this);

  CPcBangFavor *pcBangEnable = CPcBangFavor::Instance();
  if (CPcBangFavor::IsEnable(pcBangEnable))
  {
    if (IsApplyPcbangPrimium())
    {
      _AVATOR_DATA *data = &this->m_pUserDB->m_AvatorData;
      CPcBangFavor *parser = CPcBangFavor::Instance();
      CPcBangFavor::ClassCodePasing(parser, data, this);
    }
  }

  if (this->m_pUserDB)
  {
    CMgrAccountLobbyHistory::player_create_complete_money(
      &CUserDB::s_MgrLobbyHistory,
      &this->m_pUserDB->m_AvatorData,
      this->m_pUserDB->m_szLobbyHistoryFileName);
    if (this->m_bFirstStart
        && (this->m_pUserDB->m_AvatorData.dbAvator.m_dwDalant
            || this->m_pUserDB->m_AvatorData.dbAvator.m_dwGold))
    {
      const unsigned int oldDalant = this->m_pUserDB->m_AvatorData.dbAvator.m_dwDalant;
      const unsigned int oldGold = this->m_pUserDB->m_AvatorData.dbAvator.m_dwGold;
      this->m_pUserDB->m_AvatorData.dbAvator.m_dwDalant = 0;
      this->m_pUserDB->m_AvatorData.dbAvator.m_dwGold = 0;
      CMgrAccountLobbyHistory::player_money_fix(
        &CUserDB::s_MgrLobbyHistory,
        oldDalant,
        oldGold,
        &this->m_pUserDB->m_AvatorData,
        this->m_pUserDB->m_szLobbyHistoryFileName);
      SendMsg_ExchangeMoneyResult(0);
    }
  }

  CNationSettingManager *nation = CTSingleton<CNationSettingManager>::Instance();
  CNationSettingManager::CreateComplete(nation, this);
  SendMsg_BuyCashItemMode();
}

char CPlayer::Emb_CheckActForQuest(int nActCode, char *pszReqCode, unsigned __int16 wAddCount, bool bParty)
{
  _quest_check_result *checkResult = m_QuestMgr.CheckReqAct(nActCode, pszReqCode, wAddCount, bParty);
  if (!checkResult)
  {
    return 0;
  }

  for (int j = 0; j < checkResult->m_byCheckNum; ++j)
  {
    _quest_check_result::_node *node = &checkResult->m_List[j];
    _QUEST_DB_BASE::_LIST *slot = &m_Param.m_QuestDB.m_List[node->byQuestDBSlot];
    slot->wNum[node->byActIndex] = node->wCount;

    if (!m_QuestMgr.CheckFailCondition(node->byQuestDBSlot, 1, nullptr))
    {
      bool isAllComplete = true;
      if (node->bORComplete)
      {
        for (int k = 0; k < 3; ++k)
        {
          slot->wNum[k] = 0xFFFF;
        }
      }
      else
      {
        for (int k = 0; k < 3; ++k)
        {
          if (slot->wNum[k] != 0xFFFF)
          {
            isAllComplete = false;
            break;
          }
        }
      }

      if (isAllComplete)
      {
        slot->dwPassSec = static_cast<unsigned int>(-1);
      }

      SendMsg_QuestProcess(static_cast<char>(node->byQuestDBSlot), static_cast<char>(node->byActIndex), node->wCount);

      bool update = false;
      if (node->wCount == 0xFFFF || isAllComplete)
      {
        update = true;
      }
      if (m_pUserDB)
      {
        CUserDB::Update_QuestUpdate(m_pUserDB, node->byQuestDBSlot, slot, update);
      }

      if (isAllComplete)
      {
        _base_fld *record = CRecordData::GetRecord(CQuestMgr::s_tblQuest, slot->wIndex);
        if (*reinterpret_cast<unsigned int *>(&record[13].m_strCode[60])
            || *reinterpret_cast<unsigned int *>(&record[1].m_strCode[24]))
        {
          SendMsg_SelectQuestReward(static_cast<char>(node->byQuestDBSlot));
        }
        else
        {
          Emb_CompleteQuest(node->byQuestDBSlot, 0xFFu, 0xFFu);
        }
      }
    }
  }

  return 1;
}

char CPlayer::Emb_StartQuest(unsigned __int8 bySelectQuest, _happen_event_cont *pHappenEvent)
{
  if (!m_pUserDB)
  {
    return 0;
  }

  _Quest_fld *quest = m_QuestMgr.GetQuestFromEvent(bySelectQuest);
  if (!quest)
  {
    return 0;
  }

  int slotIndex = -1;
  for (int j = 0; j < 30; ++j)
  {
    if (m_Param.m_QuestDB.m_List[j].byQuestType == 0xFF)
    {
      slotIndex = j;
      break;
    }
  }

  if (pHappenEvent->m_pEvent->m_nQuestType)
  {
    if (m_QuestMgr.GetCountQuestType(pHappenEvent->m_pEvent->m_nQuestType) >= 5)
    {
      return 0;
    }
  }
  else if (m_QuestMgr.GetCountQuestType(pHappenEvent->m_pEvent->m_nQuestType) >= 20)
  {
    slotIndex = -1;
    unsigned int maxPassSec = 0;
    for (int j = 0; j < 30; ++j)
    {
      _QUEST_DB_BASE::_LIST *slot = &m_Param.m_QuestDB.m_List[j];
      if (slot->byQuestType == pHappenEvent->m_pEvent->m_nQuestType)
      {
        if (slot->dwPassSec == static_cast<unsigned int>(-1))
        {
          return 0;
        }
        if (slot->dwPassSec >= maxPassSec)
        {
          maxPassSec = slot->dwPassSec;
          slotIndex = j;
        }
      }
    }
    if (slotIndex != -1)
    {
      SendMsg_QuestFailure(5, static_cast<char>(slotIndex));
      m_QuestMgr.DeleteQuestData(static_cast<unsigned __int8>(slotIndex));
      CUserDB::Update_QuestDelete(m_pUserDB, static_cast<unsigned __int8>(slotIndex));
    }
  }

  if (slotIndex == -1)
  {
    return 0;
  }

  _QUEST_DB_BASE::_LIST *slot = &m_Param.m_QuestDB.m_List[slotIndex];
  slot->byQuestType = static_cast<unsigned __int8>(pHappenEvent->m_pEvent->m_nQuestType);
  slot->wIndex = static_cast<unsigned __int16>(quest->m_dwIndex);
  slot->dwPassSec = 0;
  for (int k = 0; k < 3; ++k)
  {
    slot->wNum[k] = 0xFFFF;
    if (quest->m_ActionNode[k].m_nActType != -1)
    {
      slot->wNum[k] = 0;
    }
    if (quest->m_ActionNode[k].m_nReqAct == -1)
    {
      slot->wNum[k] = 0xFFFF;
    }
  }
  CUserDB::Update_QuestInsert(m_pUserDB, static_cast<unsigned __int8>(slotIndex), slot);
  SendMsg_InsertNewQuest(static_cast<unsigned __int8>(slotIndex), slot);

  if (pHappenEvent->m_pEvent->m_nQuestType == 1)
  {
    for (int m = 0; m < 5; ++m)
    {
      if (pHappenEvent->m_pEvent->m_CondNode[m].m_nCondType == 8)
      {
        _STORAGE_LIST::_db_con *item =
          _STORAGE_LIST::GetPtrFromItemCode(&m_Param.m_dbInven, pHappenEvent->m_pEvent->m_CondNode[m].m_sCondVal);
        if (item)
        {
          if (IsOverLapItem(item->m_byTableCode))
          {
            unsigned int dwDur = Emb_AlterDurPoint(0, item->m_byStorageIndex, -1, false, false);
            if (dwDur)
            {
              SendMsg_AdjustAmountInform(0, item->m_wSerial, dwDur);
            }
            else
            {
              CMgrAvatorItemHistory::delete_npc_quest_item(
                &CPlayer::s_MgrItemHistory,
                m_ObjID.m_wIndex,
                item,
                m_szItemHistoryFileName);
            }
          }
          else
          {
            if (!Emb_DelStorage(0, item->m_byStorageIndex, false, true, "CPlayer::pcChatAllRequest()"))
            {
              return 0;
            }
            CMgrAvatorItemHistory::delete_npc_quest_item(
              &CPlayer::s_MgrItemHistory,
              m_ObjID.m_wIndex,
              item,
              m_szItemHistoryFileName);
          }
        }
      }
    }
  }

  return 1;
}

_Quest_fld *CPlayer::_Reward_Quest(_Quest_fld *pQuestFld, unsigned __int8 byRewardItemIndex)
{
  _Quest_fld *linkedQuest = nullptr;
  const long double penalty = TimeLimitMgr::GetPlayerPenalty(g_Main.m_pTimeLimitMgr, m_id.wIndex);

  if (pQuestFld->m_nMaxLevel != -1)
  {
    AlterMaxLevel(pQuestFld->m_nMaxLevel);
  }
  if (pQuestFld->m_dConsExp > 0.0)
  {
    AlterExp(pQuestFld->m_dConsExp, true, false, true);
  }
  if (pQuestFld->m_nConsContribution > 0)
  {
    AlterPvPPoint(static_cast<double>(pQuestFld->m_nConsContribution), quest_inc, static_cast<unsigned int>(-1));
  }
  if (pQuestFld->m_nConspvppoint > 0)
  {
    pQuestFld->m_nConspvppoint = static_cast<int>(static_cast<double>(pQuestFld->m_nConspvppoint) * penalty);
    AlterPvPCashBag(static_cast<double>(pQuestFld->m_nConspvppoint), pm_quest);
  }

  if (pQuestFld->m_nConsDalant > 0 || pQuestFld->m_nConsGold > 0)
  {
    AddDalant(pQuestFld->m_nConsDalant, true);
    AddGold(pQuestFld->m_nConsGold, true);
    SendMsg_ExchangeMoneyResult(0);

    const unsigned int addDalant = static_cast<unsigned int>(
      static_cast<double>(pQuestFld->m_nConsDalant) * penalty);
    const unsigned int addGold = static_cast<unsigned int>(
      static_cast<double>(pQuestFld->m_nConsGold) * penalty);
    const unsigned int sumGold = CPlayerDB::GetGold(&m_Param);
    const unsigned int sumDalant = CPlayerDB::GetDalant(&m_Param);
    CMgrAvatorItemHistory::reward_add_money(
      &CPlayer::s_MgrItemHistory,
      m_ObjID.m_wIndex,
      "Quest",
      addDalant,
      addGold,
      sumDalant,
      sumGold,
      m_szItemHistoryFileName);

    const int level = CPlayerDB::GetLevel(&m_Param);
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      if (pQuestFld->m_nConsDalant > 0)
      {
        const char *classCode = CPlayerDB::GetPtrCurClass(&m_Param)->m_strCode;
        const int curLevel = CPlayerDB::GetLevel(&m_Param);
        const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
        CMoneySupplyMgr *money = CMoneySupplyMgr::Instance();
        CMoneySupplyMgr::UpdateQuestRewardMoneyData(money, raceCode, curLevel, classCode, pQuestFld->m_nConsDalant);
      }
      if (pQuestFld->m_nConsGold > 0)
      {
        const unsigned int amount = 2000 * pQuestFld->m_nConsGold;
        const char *classCode = CPlayerDB::GetPtrCurClass(&m_Param)->m_strCode;
        const int curLevel = CPlayerDB::GetLevel(&m_Param);
        const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
        CMoneySupplyMgr *money = CMoneySupplyMgr::Instance();
        CMoneySupplyMgr::UpdateQuestRewardMoneyData(money, raceCode, curLevel, classCode, amount);
      }
    }
  }

  for (int j = 0; j < 6; ++j)
  {
    const char *itemCode = pQuestFld->m_RewardItem[j].m_strConsITCode;
    if (TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, m_id.wIndex) == 99)
    {
      break;
    }

    if (byRewardItemIndex != 0xFF)
    {
      if (j != byRewardItemIndex)
      {
        continue;
      }
      if (pQuestFld->m_RewardItem[j].m_nLinkQuestIdx != -1)
      {
        linkedQuest = reinterpret_cast<_Quest_fld *>(CRecordData::GetRecord(
          CQuestMgr::s_tblQuest,
          pQuestFld->m_strLinkQuest[pQuestFld->m_RewardItem[j].m_nLinkQuestIdx]));
      }
    }

    if (std::strncmp(itemCode, "-1", 2) == 0)
    {
      break;
    }

    const int tableCode = GetItemTableCode(itemCode);
    if (tableCode == -1)
    {
      continue;
    }

    auto *record = reinterpret_cast<unsigned __int16 *>(
      CRecordData::GetRecord(&g_Main.m_tblItemData[tableCode], itemCode));
    if (!record)
    {
      continue;
    }

    _STORAGE_LIST::_db_con item{};
    item.m_byTableCode = static_cast<unsigned __int8>(tableCode);
    item.m_wItemIndex = *record;
    if (IsOverLapItem(tableCode))
    {
      item.m_dwDur = pQuestFld->m_RewardItem[j].m_nConsITCnt;
    }
    else
    {
      item.m_dwDur = GetItemDurPoint(tableCode, *reinterpret_cast<unsigned int *>(record));
    }
    const unsigned __int8 socketNum = GetDefItemUpgSocketNum(tableCode, *reinterpret_cast<unsigned int *>(record));
    item.m_dwLv = GetBitAfterSetLimSocket(socketNum);

    if (_STORAGE_LIST::GetIndexEmptyCon(&m_Param.m_dbInven) == 0xFF)
    {
      if (TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, m_id.wIndex) == 99)
      {
        break;
      }

      CreateItemBox(
        &item,
        this,
        static_cast<unsigned int>(-1),
        0,
        nullptr,
        3,
        m_pCurMap,
        m_wMapLayerIndex,
        m_fCurPos,
        0);

      char clause[136]{};
      sprintf(clause, "Quest G (%s)", pQuestFld->m_strCode);
      CMgrAvatorItemHistory::reward_add_item(
        &CPlayer::s_MgrItemHistory,
        m_ObjID.m_wIndex,
        clause,
        &item,
        m_szItemHistoryFileName);
    }
    else
    {
      item.m_wSerial = CPlayerDB::GetNewItemSerial(&m_Param);
      const _TimeItem_fld *timeRec = TimeItem::FindTimeRec(tableCode, *reinterpret_cast<unsigned int *>(record));
      if (timeRec && timeRec->m_nCheckType)
      {
        item.m_byCsMethod = static_cast<unsigned __int8>(timeRec->m_nCheckType);
        __time32_t now[11]{};
        _time32(now);
        item.m_dwT = timeRec->m_nUseTime + now[0];
        item.m_dwLendRegdTime = now[0];
      }
      if (!Emb_AddStorage(0, &item, false, true))
      {
        CMgrAvatorItemHistory::add_storage_fail(
          &CPlayer::s_MgrItemHistory,
          m_ObjID.m_wIndex,
          &item,
          "CPlayer::_Reward_Quest() - Emb_AddStorage() Fail",
          m_szItemHistoryFileName);
        continue;
      }

      SendMsg_RewardAddItem(&item, 2);

      char clause[160]{};
      sprintf(clause, "Quest (%s)", pQuestFld->m_strCode);
      CMgrAvatorItemHistory::reward_add_item(
        &CPlayer::s_MgrItemHistory,
        m_ObjID.m_wIndex,
        clause,
        &item,
        m_szItemHistoryFileName);
    }

    SendMsg_FanfareItem(5, &item, nullptr);
  }

  for (int j = 0; j < 2; ++j)
  {
    _quest_reward_mastery *reward = &pQuestFld->m_RewardMastery[j];
    if (reward->m_nConsMasteryID == -1)
    {
      break;
    }
    Emb_AlterStat(
      reward->m_nConsMasteryID,
      reward->m_nConsMasterySubID,
      reward->m_nConsMasteryCnt,
      2,
      "CPlayer::_Reward_Quest()---0",
      true);
  }

  if (std::strncmp(pQuestFld->m_strConsSkillCode, "-1", 2) != 0)
  {
    auto *record = reinterpret_cast<unsigned __int8 *>(
      CRecordData::GetRecord(g_Main.m_tblEffectData, pQuestFld->m_strConsSkillCode));
    if (record)
    {
      Emb_AlterStat(3, *record, pQuestFld->m_nConsSkillCnt, 2, "CPlayer::_Reward_Quest()---1", true);
    }
  }

  if (std::strncmp(pQuestFld->m_strConsForceCode, "-1", 2) != 0)
  {
    _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblEffectData[1], pQuestFld->m_strConsSkillCode);
    if (record)
    {
      for (int k = 0; k < 88; ++k)
      {
        _STORAGE_LIST::_db_con *forceItem = &m_Param.m_dbForce.m_pStorageList[k];
        if (forceItem->m_bLoad
          && static_cast<unsigned int>(CPlayer::s_pnLinkForceItemToEffect[forceItem->m_wItemIndex]) == record->m_dwIndex)
        {
          unsigned int newStat = Emb_AlterDurPoint(3, forceItem->m_byStorageIndex, pQuestFld->m_nConsForceCnt, false, false);
          SendMsg_FcitemInform(forceItem->m_wSerial, newStat);
          return linkedQuest;
        }
      }
    }
  }

  return linkedQuest;
}

void CPlayer::Emb_CompleteQuest(
  unsigned __int8 byQuestDBSlot,
  unsigned __int8 byRewardItemIndex,
  unsigned __int8 byLinkQuestIndex)
{
  if (!m_pUserDB)
  {
    return;
  }

  _QUEST_DB_BASE::_LIST *slot = &m_Param.m_QuestDB.m_List[byQuestDBSlot];
  if (slot->byQuestType == 0xFF)
  {
    return;
  }

  for (int j = 0; j < 3; ++j)
  {
    if (slot->wNum[j] != 0xFFFF)
    {
      return;
    }
  }

  CMgrAvatorLvHistory::update_mastery(
    m_ObjID.m_wIndex,
    m_byUserDgr,
    CPlayerDB::GetLevel(&m_Param),
    CPlayerDB::GetExp(&m_Param),
    m_dwExpRate,
    m_Param.m_byPvPGrade,
    m_nMaxPoint,
    &m_pmMst,
    m_Param.m_dwAlterMastery,
    m_szLvHistoryFileName,
    0,
    nullptr);
  CPlayerDB::InitAlterMastery(&m_Param);

  auto *quest = reinterpret_cast<_Quest_fld *>(CRecordData::GetRecord(CQuestMgr::s_tblQuest, slot->wIndex));
  _Quest_fld *linkedQuest = _Reward_Quest(quest, byRewardItemIndex);
  SendMsg_QuestComplete(static_cast<char>(byQuestDBSlot));

  if (slot->byQuestType == 1)
  {
    const unsigned __int8 inserted = m_QuestMgr.InsertNpcQuestHistory(
      quest->m_strCode,
      static_cast<char>(quest->m_nDifficultyLevel),
      quest->m_dRepeatTime);
    _QUEST_DB_BASE::_NPC_QUEST_HISTORY *history = &m_Param.m_QuestDB.m_History[inserted];
    CUserDB::Update_NPCQuestHistory(m_pUserDB, inserted, history);
    SendMsg_NpcQuestHistoryInform(static_cast<char>(inserted));
  }

  m_QuestMgr.DeleteQuestData(byQuestDBSlot);
  CUserDB::Update_QuestDelete(m_pUserDB, byQuestDBSlot);

  CMgrAvatorLvHistory::update_mastery(
    m_ObjID.m_wIndex,
    m_byUserDgr,
    CPlayerDB::GetLevel(&m_Param),
    CPlayerDB::GetExp(&m_Param),
    m_dwExpRate,
    m_Param.m_byPvPGrade,
    m_nMaxPoint,
    &m_pmMst,
    m_Param.m_dwAlterMastery,
    m_szLvHistoryFileName,
    2,
    quest->m_strCode);
  CPlayerDB::InitAlterMastery(&m_Param);

  m_dwUMWHLastTime = GetLoopTime();
  if (quest->m_nLinkDummyCond == 1)
  {
    if (strcmp_0(quest->m_strLinkDummyCode, "-1") != 0)
    {
      _dummy_position *pos = CMapData::GetDummyPostion(m_pCurMap, quest->m_strLinkDummyCode);
      if (pos)
      {
        float newPos[3]{};
        if (CMapData::GetRandPosInDummy(m_pCurMap, pos, newPos, 1))
        {
          OutOfMap(m_pCurMap, m_wMapLayerIndex, 3, newPos);
          const unsigned __int8 mapCode = CPlayerDB::GetMapCode(&m_Param);
          SendMsg_GotoRecallResult(0, mapCode, newPos, 4);
        }
      }
    }
  }

  if (!linkedQuest)
  {
    int linkCount = 0;
    for (int j = 0; j < 5; ++j)
    {
      if (std::strncmp(quest->m_strLinkQuest[j], "-1", 2) != 0)
      {
        ++linkCount;
      }
    }
    if (linkCount > 0)
    {
      unsigned __int8 linkIndex = byLinkQuestIndex;
      if (byLinkQuestIndex == 0xFF)
      {
        linkIndex = static_cast<unsigned __int8>(rand() % linkCount);
      }
      linkedQuest = reinterpret_cast<_Quest_fld *>(CRecordData::GetRecord(CQuestMgr::s_tblQuest, quest->m_strLinkQuest[linkIndex]));
    }
  }

  if (linkedQuest)
  {
    _QUEST_DB_BASE::_LIST *nextSlot = &m_Param.m_QuestDB.m_List[byQuestDBSlot];
    nextSlot->byQuestType = slot->byQuestType;
    nextSlot->wIndex = static_cast<unsigned __int16>(linkedQuest->m_dwIndex);
    nextSlot->dwPassSec = 0;
    for (int k = 0; k < 3; ++k)
    {
      if (linkedQuest->m_ActionNode[k].m_nActType != -1)
      {
        nextSlot->wNum[k] = 0;
      }
    }
    CUserDB::Update_QuestInsert(m_pUserDB, byQuestDBSlot, nextSlot);
    SendMsg_InsertNextQuest(byQuestDBSlot, nextSlot);
  }

  for (int m = 0; m < 3; ++m)
  {
    if (m_QuestMgr.m_pTempHappenEvent[m].isset())
    {
      memcpy_0(&m_QuestMgr.m_LastHappenEvent, &m_QuestMgr.m_pTempHappenEvent[m], sizeof(m_QuestMgr.m_LastHappenEvent));
      Emb_StartQuest(0xFFu, &m_QuestMgr.m_pTempHappenEvent[m]);
      if (m_QuestMgr.m_pTempHappenEvent[m].m_QtHpType == quest_happen_type_maxlevel)
      {
        CPlayerDB::SetMaxLevel(&m_Param, 50);
        if (m_pUserDB)
        {
          CUserDB::Update_MaxLevel(m_pUserDB, 0x32u);
        }
      }
      m_QuestMgr.m_pTempHappenEvent[m].init();
    }
  }
}

void CPlayer::SendMsg_ChatFarFailure(char bBlock)
{
  _chat_far_failure_zocl msg{};
  msg.bBlock = bBlock != 0;

  unsigned __int8 pbyType[2] = {2, 4};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), msg.size());
}

void CPlayer::SendMsg_AdjustAmountInform(char byStorageCode, unsigned __int16 wSerial, unsigned int dwDur)
{
  #pragma pack(push, 1)
  struct AdjustAmountInformMsg
  {
    char byStorageCode;
    unsigned __int16 wSerial;
    unsigned int dwDur;
  };
  #pragma pack(pop)

  AdjustAmountInformMsg msg{};
  msg.byStorageCode = byStorageCode;
  msg.wSerial = wSerial;
  msg.dwDur = dwDur;

  unsigned __int8 pbyType[2] = {20, 3};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), 7u);
}

unsigned int CPlayer::GetMoney(unsigned __int8 byMoneyCode)
{
  if (byMoneyCode)
  {
    return CPlayerDB::GetGold(&m_Param);
  }
  return CPlayerDB::GetDalant(&m_Param);
}

CPlayer *CPlayer::FindFarChatPlayerWithTemp(char *pwszName)
{
  const unsigned __int8 nameLen = static_cast<unsigned __int8>(strlen_0(pwszName));
  CPlayer *dst = nullptr;

  for (int j = 0; j < 10; ++j)
  {
    _MEM_PAST_WHISPER *entry = &m_PastWhiper[j];
    if (entry->bMemory && entry->byNameLen == nameLen && !strncmp(entry->wszName, pwszName, nameLen))
    {
      if (entry->pDst
          && entry->pDst->m_bLive
          && entry->pDst->m_Param.m_byNameLen == nameLen
          && !strncmp(CPlayerDB::GetCharNameW(&entry->pDst->m_Param), entry->wszName, nameLen))
      {
        dst = entry->pDst;
      }
      else
      {
        entry->bMemory = false;
      }
      break;
    }
  }

  if (!dst)
  {
    for (int k = 0; k < MAX_PLAYER; ++k)
    {
      CPlayer *candidate = &g_Player[k];
      if (candidate->m_bLive && candidate->m_Param.m_byNameLen == nameLen)
      {
        if (!strncmp(CPlayerDB::GetCharNameW(&candidate->m_Param), pwszName, nameLen))
        {
          dst = candidate;
          break;
        }
      }
    }

    if (dst)
    {
      _MEM_PAST_WHISPER *slot = nullptr;
      for (int m = 0; m < 10; ++m)
      {
        if (!m_PastWhiper[m].bMemory)
        {
          slot = &m_PastWhiper[m];
          break;
        }
      }

      if (!slot)
      {
        unsigned int oldest = static_cast<unsigned int>(-1);
        for (int n = 0; n < 10; ++n)
        {
          if (m_PastWhiper[n].dwMemoryTime < oldest)
          {
            oldest = m_PastWhiper[n].dwMemoryTime;
            slot = &m_PastWhiper[n];
          }
        }
      }

      slot->bMemory = true;
      slot->dwMemoryTime = timeGetTime();
      slot->pDst = dst;
      strcpy_0(slot->wszName, CPlayerDB::GetCharNameW(&dst->m_Param));
      slot->byNameLen = static_cast<unsigned __int8>(strlen_0(slot->wszName));
    }
  }

  return dst;
}

void CPlayer::pc_ChatOperatorRequest(unsigned __int8 byRaceCode, char *pwszChatData)
{
  if (m_byUserDgr == 2)
  {
    for (int j = 0; j < MAX_PLAYER; ++j)
    {
      CPlayer *target = &g_Player[j];
      if (target->m_bLive
          && (byRaceCode == 0xFF || target->m_byUserDgr || CPlayerDB::GetRaceCode(&target->m_Param) == byRaceCode))
      {
        target->SendData_ChatTrans(0, 0xFFFFFFFF, byRaceCode, false, pwszChatData, 0xFFu, nullptr);
      }
    }
  }
}

void CPlayer::pc_ChatCircleRequest(char *pwszChatData)
{
  if ((!m_pUserDB || !m_pUserDB->m_bChatLock)
      && m_pCurMap
      && CGameObject::GetCurSecNum(this) != -1
      && !m_bMapLoading)
  {
    _chat_message_receipt_udp msg{};
    msg.byMessageType = 1;
    msg.dwSenderSerial = m_dwObjSerial;
    msg.byRaceCode = CPlayerDB::GetRaceCode(&m_Param);
    msg.bFiltering = false;
    msg.bySize = static_cast<unsigned __int8>(strlen_0(pwszChatData));
    memcpy_0(msg.wszChatData, pwszChatData, msg.bySize);
    msg.wszChatData[msg.bySize] = 0;
    strcpy_0(msg.wszSenderName, CPlayerDB::GetCharNameW(&m_Param));
    msg.byPvpGrade = m_Param.m_byPvPGrade;

    _chat_message_receipt_udp filtered{};
    filtered.byMessageType = 1;
    filtered.dwSenderSerial = m_dwObjSerial;
    filtered.byRaceCode = CPlayerDB::GetRaceCode(&m_Param);
    filtered.bFiltering = true;
    filtered.wszChatData[0] = 0;
    filtered.bySize = 0;
    strcpy_0(filtered.wszSenderName, CPlayerDB::GetCharNameW(&m_Param));
    filtered.byPvpGrade = m_Param.m_byPvPGrade;

    unsigned __int8 type[2] = {2, 10};
    _sec_info *secInfo = CMapData::GetSecInfo(m_pCurMap);
    const int curSec = CGameObject::GetCurSecNum(this);
    _pnt_rect rect{};
    CMapData::GetRectInRadius(m_pCurMap, &rect, 3, curSec);

    CChatStealSystem *stealSystem = CChatStealSystem::Instance();
    stealSystem->StealChatMsg(this, 1u, pwszChatData);

    for (int y = rect.nStarty; y <= rect.nEndy; ++y)
    {
      for (int x = rect.nStartx; x <= rect.nEndx; ++x)
      {
        const unsigned int secIndex = secInfo->m_nSecNumW * y + x;
        CObjectList *list = CMapData::GetSectorListPlayer(m_pCurMap, m_wMapLayerIndex, secIndex);
        if (!list)
        {
          continue;
        }

        _object_list_point *node = list->m_Head.m_pNext;
        while (node != &list->m_Tail)
        {
          CGameObject *obj = node->m_pItem;
          node = node->m_pNext;

          CPlayer *target = static_cast<CPlayer *>(obj);
          bool sendFull = false;
          if (m_byUserDgr == 2)
          {
            sendFull = true;
          }
          else if (CPlayerDB::GetRaceCode(&target->m_Param) == CPlayerDB::GetRaceCode(&m_Param))
          {
            sendFull = true;
          }
          else if (target->m_byUserDgr >= 2u)
          {
            sendFull = true;
          }
          else if (_effect_parameter::GetEff_Have(&target->m_EP, 3) != 0.0)
          {
            sendFull = true;
          }
          else
          {
            const int raceCode = CPlayerDB::GetRaceCode(&target->m_Param);
            CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
            const unsigned int bossSerial = CPvpUserAndGuildRankingSystem::GetCurrentRaceBossSerial(rank, raceCode, 0);
            if (bossSerial == target->m_dwObjSerial)
            {
              sendFull = true;
            }
          }

          if (sendFull)
          {
            const unsigned __int16 len = msg.size();
            CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], target->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
          }
          else
          {
            const unsigned __int16 len = filtered.size();
            CNetProcess::LoadSendMsg(
              g_Network.m_pProcess[0],
              target->m_ObjID.m_wIndex,
              type,
              reinterpret_cast<char *>(&filtered),
              len);
          }
        }
      }
    }
  }
}

void CPlayer::pc_ChatFarRequest(char *pwszName, char *pwszChatData)
{
  if (!m_pUserDB || (!m_pUserDB->m_bChatLock && !IsPunished(0, true)))
  {
    CPlayer *dst = FindFarChatPlayerWithTemp(pwszName);
    if (dst)
    {
      if (m_byUserDgr == 2 || !dst->m_bBlockWhisper)
      {
        bool filter = false;
        unsigned __int8 chatType = 2;
        if (m_byUserDgr == 2)
        {
          chatType = 12;
        }
        else
        {
          const int dstRace = CPlayerDB::GetRaceCode(&dst->m_Param);
          const int myRace = CPlayerDB::GetRaceCode(&m_Param);
          if (dstRace != myRace && dst->m_byUserDgr < 2u && _effect_parameter::GetEff_Have(&dst->m_EP, 3) == 0.0)
          {
            const int raceCode = CPlayerDB::GetRaceCode(&dst->m_Param);
            CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
            const unsigned int bossSerial = CPvpUserAndGuildRankingSystem::GetCurrentRaceBossSerial(rank, raceCode, 0);
            if (bossSerial != dst->m_dwObjSerial)
            {
              filter = true;
            }
          }
        }

        char *senderName = CPlayerDB::GetCharNameW(&m_Param);
        const unsigned __int8 senderRace = CPlayerDB::GetRaceCode(&m_Param);
        dst->SendData_ChatTrans(
          chatType,
          m_dwObjSerial,
          senderRace,
          filter,
          pwszChatData,
          m_Param.m_byPvPGrade,
          senderName);

        char *dstName = CPlayerDB::GetCharNameW(&dst->m_Param);
        const unsigned __int8 selfRace = CPlayerDB::GetRaceCode(&m_Param);
        SendData_ChatTrans(chatType, m_dwObjSerial, selfRace, false, pwszChatData, m_Param.m_byPvPGrade, dstName);

        char buffer[288]{};
        sprintf_s(buffer, 0x110u, dstName);

        CChatStealSystem *stealSystem = CChatStealSystem::Instance();
        stealSystem->StealChatMsg(this, chatType, pwszChatData);
      }
      else
      {
        SendMsg_ChatFarFailure(1);
      }
    }
    else
    {
      SendMsg_ChatFarFailure(0);
    }
  }
}

void CPlayer::pc_ChatPartyRequest(char *pwszChatData)
{
  if ((!m_pUserDB || (!m_pUserDB->m_bChatLock && !IsPunished(0, true))) && CPartyPlayer::IsPartyMode(m_pPartyMgr))
  {
    _chat_message_receipt_udp msg{};
    msg.byMessageType = 3;
    msg.dwSenderSerial = m_dwObjSerial;
    msg.byRaceCode = CPlayerDB::GetRaceCode(&m_Param);
    msg.bFiltering = false;
    msg.bySize = static_cast<unsigned __int8>(strlen_0(pwszChatData));
    memcpy_0(msg.wszChatData, pwszChatData, msg.bySize);
    msg.wszChatData[msg.bySize] = 0;
    strcpy_0(msg.wszSenderName, CPlayerDB::GetCharNameW(&m_Param));
    msg.byPvpGrade = static_cast<unsigned __int8>(-1);

    unsigned __int8 type[2] = {2, 10};
    CPartyPlayer **members = CPartyPlayer::GetPtrPartyMember(m_pPartyMgr);

    CChatStealSystem *stealSystem = CChatStealSystem::Instance();
    stealSystem->StealChatMsg(this, msg.byMessageType, pwszChatData);

    for (int j = 0; j < 8; ++j)
    {
      if (members[j])
      {
        const unsigned __int16 len = msg.size();
        CNetProcess::LoadSendMsg(
          g_Network.m_pProcess[0],
          members[j]->m_wZoneIndex,
          type,
          reinterpret_cast<char *>(&msg),
          len);
      }
    }
  }
}

void CPlayer::pc_ChatRaceRequest(char *pwszChatData)
{
  if (m_pUserDB && !m_pUserDB->m_bChatLock && !IsPunished(0, true))
  {
    const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
    CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
    if (CPvpUserAndGuildRankingSystem::IsCurrentRaceBossGroup(rank, raceCode, m_dwObjSerial))
    {
      _announ_message_receipt_udp msg{};
      msg.byMessageType = 4;
      msg.bySenderRace = CPlayerDB::GetRaceCode(&m_Param);
      msg.dwSenderSerial = m_dwObjSerial;
      strcpy_0(msg.wszSenderName, CPlayerDB::GetCharNameW(&m_Param));
      msg.bySize = static_cast<unsigned __int8>(strlen_0(pwszChatData));
      memcpy_0(msg.wszChatData, pwszChatData, msg.bySize);
      msg.wszChatData[msg.bySize] = 0;
      msg.byPvpGrade = static_cast<unsigned __int8>(-1);

      unsigned __int8 type[2] = {2, 11};
      const int len = msg.size();

      CChatStealSystem *stealSystem = CChatStealSystem::Instance();
      stealSystem->StealChatMsg(this, msg.byMessageType, pwszChatData);

      for (int j = 0; j < MAX_PLAYER; ++j)
      {
        CPlayer *target = &g_Player[j];
        if (target->m_bLive)
        {
          if (target->m_byUserDgr >= 2u
              || CPlayerDB::GetRaceCode(&target->m_Param) == CPlayerDB::GetRaceCode(&m_Param))
          {
            CNetProcess::LoadSendMsg(
              g_Network.m_pProcess[0],
              target->m_ObjID.m_wIndex,
              type,
              reinterpret_cast<char *>(&msg),
              len);
          }
        }
      }
    }
  }
}

void CPlayer::pc_ChatMgrWhisperRequest(char *pwszChatData)
{
  if (m_byUserDgr >= 2u)
  {
    mgr_whisper(pwszChatData);
  }
}

void CPlayer::pc_ChatMapRequest(char *pwszChatData)
{
  if (m_pUserDB
      && !m_pUserDB->m_bChatLock
      && !IsPunished(0, true)
      && CGameObject::GetCurSecNum(this) != -1
      && !m_bMapLoading)
  {
    _chat_message_receipt_udp msg{};
    msg.byMessageType = 9;
    msg.dwSenderSerial = m_dwObjSerial;
    msg.byRaceCode = CPlayerDB::GetRaceCode(&m_Param);
    msg.bFiltering = false;
    msg.bySize = static_cast<unsigned __int8>(strlen_0(pwszChatData));
    memcpy_0(msg.wszChatData, pwszChatData, msg.bySize);
    msg.wszChatData[msg.bySize] = 0;
    strcpy_0(msg.wszSenderName, CPlayerDB::GetCharNameW(&m_Param));
    msg.byPvpGrade = m_Param.m_byPvPGrade;

    unsigned __int8 type[2] = {2, 10};
    CChatStealSystem *stealSystem = CChatStealSystem::Instance();
    stealSystem->StealChatMsg(this, msg.byMessageType, pwszChatData);

    const int len = msg.size();
    _sec_info *secInfo = CMapData::GetSecInfo(m_pCurMap);
    for (int j = 0; j < secInfo->m_nSecNumH; ++j)
    {
      for (int k = 0; k < secInfo->m_nSecNumW; ++k)
      {
        const unsigned int secIndex = k + secInfo->m_nSecNumW * j;
        CObjectList *list = CMapData::GetSectorListPlayer(m_pCurMap, m_wMapLayerIndex, secIndex);
        if (!list)
        {
          continue;
        }

        _object_list_point *node = list->m_Head.m_pNext;
        while (node != &list->m_Tail)
        {
          CGameObject *obj = node->m_pItem;
          node = node->m_pNext;

          CPlayer *target = static_cast<CPlayer *>(obj);
          if (CPlayerDB::GetRaceCode(&target->m_Param) == CPlayerDB::GetRaceCode(&m_Param))
          {
            if (target->m_bRecvMapChat)
            {
              CNetProcess::LoadSendMsg(
                g_Network.m_pProcess[0],
                target->m_ObjID.m_wIndex,
                type,
                reinterpret_cast<char *>(&msg),
                len);
            }
          }
        }
      }
    }
  }
}

void CPlayer::pc_ChatRaceBossRequest(char *pwszChatData)
{
  if (m_pUserDB && !m_pUserDB->m_bChatLock && !IsPunished(0, true))
  {
    const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
    CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
    if (CPvpUserAndGuildRankingSystem::IsCurrentRaceBossGroup(rank, raceCode, m_dwObjSerial))
    {
      _announ_message_receipt_udp msg{};
      msg.byMessageType = 10;
      msg.bySenderRace = CPlayerDB::GetRaceCode(&m_Param);
      msg.dwSenderSerial = m_dwObjSerial;
      strcpy_0(msg.wszSenderName, CPlayerDB::GetCharNameW(&m_Param));
      msg.bySize = static_cast<unsigned __int8>(strlen_0(pwszChatData));
      memcpy_0(msg.wszChatData, pwszChatData, msg.bySize);
      msg.wszChatData[msg.bySize] = 0;
      msg.byPvpGrade = static_cast<unsigned __int8>(-1);

      unsigned __int8 type[2] = {2, 11};
      CChatStealSystem *stealSystem = CChatStealSystem::Instance();
      stealSystem->StealChatMsg(this, msg.byMessageType, pwszChatData);

      const int len = msg.size();
      for (int j = 0; j < MAX_PLAYER; ++j)
      {
        CPlayer *target = &g_Player[j];
        if (target->m_bLive)
        {
          if (CPlayerDB::GetRaceCode(&target->m_Param) == CPlayerDB::GetRaceCode(&m_Param))
          {
            const unsigned int serial = CPlayerDB::GetCharSerial(&target->m_Param);
            const int myRace = CPlayerDB::GetRaceCode(&m_Param);
            CPvpUserAndGuildRankingSystem *rankSys = CPvpUserAndGuildRankingSystem::Instance();
            if (CPvpUserAndGuildRankingSystem::IsCurrentRaceBossGroup(rankSys, myRace, serial))
            {
              CNetProcess::LoadSendMsg(
                g_Network.m_pProcess[0],
                target->m_ObjID.m_wIndex,
                type,
                reinterpret_cast<char *>(&msg),
                len);
            }
          }
        }
      }
    }
  }
}

void CPlayer::pc_ChatGuildEstSenRequest(char *pwszChatData)
{
  if (m_pUserDB
      && !m_pUserDB->m_bChatLock
      && m_Param.m_pGuild
      && m_Param.m_pGuildMemPtr
      && (m_Param.m_byClassInGuild == 1 || m_Param.m_byClassInGuild == 2))
  {
    _announ_message_receipt_udp msg{};
    msg.byMessageType = 11;
    msg.bySenderRace = CPlayerDB::GetRaceCode(&m_Param);
    msg.dwSenderSerial = m_dwObjSerial;
    strcpy_0(msg.wszSenderName, CPlayerDB::GetCharNameW(&m_Param));
    msg.bySize = static_cast<unsigned __int8>(strlen_0(pwszChatData));
    memcpy_0(msg.wszChatData, pwszChatData, msg.bySize);
    msg.wszChatData[msg.bySize] = 0;
    msg.byPvpGrade = static_cast<unsigned __int8>(-1);

    unsigned __int8 type[2] = {2, 11};
    CGuild *guild = m_Param.m_pGuild;
    _guild_member_info *members = guild->m_MemberData;

    CChatStealSystem *stealSystem = CChatStealSystem::Instance();
    stealSystem->StealChatMsg(this, msg.byMessageType, pwszChatData);

    const int len = msg.size();
    for (int j = 0; j < guild->m_nMemberNum; ++j)
    {
      if (_guild_member_info::IsFill(&members[j])
          && members[j].pPlayer
          && (members[j].byClassInGuild == 1 || members[j].byClassInGuild == 2))
      {
        CPlayer *target = members[j].pPlayer;
        CNetProcess::LoadSendMsg(
          g_Network.m_pProcess[0],
          target->m_ObjID.m_wIndex,
          type,
          reinterpret_cast<char *>(&msg),
          len);
      }
    }
  }
}

void CPlayer::pc_ChatRePresentationRequest(char *pwszChatData)
{
  if (m_pUserDB)
  {
    if (!m_pUserDB->m_bChatLock)
    {
      const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
      CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
      if (CPvpUserAndGuildRankingSystem::IsCurrentRaceBossGroup(rank, raceCode, m_dwObjSerial)
          || (m_Param.m_byClassInGuild == 2 && m_Param.m_pGuild))
      {
        _announ_message_receipt_udp msg{};
        msg.byMessageType = 13;
        msg.bySenderRace = CPlayerDB::GetRaceCode(&m_Param);
        msg.dwSenderSerial = m_dwObjSerial;
        strcpy_0(msg.wszSenderName, CPlayerDB::GetCharNameW(&m_Param));
        msg.bySize = static_cast<unsigned __int8>(strlen_0(pwszChatData));
        memcpy_0(msg.wszChatData, pwszChatData, msg.bySize);
        msg.wszChatData[msg.bySize] = 0;
        msg.byPvpGrade = static_cast<unsigned __int8>(-1);

        unsigned __int8 type[2] = {2, 11};
        CChatStealSystem *stealSystem = CChatStealSystem::Instance();
        stealSystem->StealChatMsg(this, msg.byMessageType, pwszChatData);

        const int len = msg.size();
        for (int j = 0; j < MAX_PLAYER; ++j)
        {
          CPlayer *target = &g_Player[j];
          if (target->m_bLive)
          {
            if (CPlayerDB::GetRaceCode(&target->m_Param) == CPlayerDB::GetRaceCode(&m_Param))
            {
              const int myRace = CPlayerDB::GetRaceCode(&m_Param);
              CPvpUserAndGuildRankingSystem *rankSys = CPvpUserAndGuildRankingSystem::Instance();
              if (CPvpUserAndGuildRankingSystem::IsCurrentRaceBossGroup(rankSys, myRace, target->m_dwObjSerial)
                  || (target->m_Param.m_byClassInGuild == 2 && target->m_Param.m_pGuild))
              {
                CNetProcess::LoadSendMsg(
                  g_Network.m_pProcess[0],
                  target->m_ObjID.m_wIndex,
                  type,
                  reinterpret_cast<char *>(&msg),
                  len);
              }
            }
          }
        }
      }
    }
  }
}

void CPlayer::pc_ChatAllRequest(char *pwszChatData)
{
  if (!m_pUserDB)
  {
    return;
  }
  if (m_pUserDB->m_bChatLock)
  {
    return;
  }
  if (IsPunished(0, true))
  {
    return;
  }
  if (CGameObject::GetCurSecNum(this) == -1)
  {
    return;
  }
  if (m_bMapLoading)
  {
    return;
  }

  _STORAGE_LIST::_db_con *item = nullptr;
  if (g_Main.m_bAllRaceChatItemConsume)
  {
    item = _STORAGE_LIST::GetPtrFromItemCode(&m_Param.m_dbInven, g_Main.m_strAllRaceChatItemCode);
    if (!item)
    {
      return;
    }
  }

  if (g_Main.m_bAllRaceChatMoneyConsume && g_Main.m_dwAllRaceChatMoney > GetMoney(0))
  {
    return;
  }

  if (g_Main.m_bAllRaceChatItemConsume && item)
  {
    if (IsOverLapItem(item->m_byTableCode))
    {
      const unsigned int dur = Emb_AlterDurPoint(0, item->m_byStorageIndex, -1, 0, 0);
      if (dur)
      {
        SendMsg_AdjustAmountInform(0, item->m_wSerial, dur);
        goto LABEL_AFTER_ITEM;
      }
    }
    else if (!Emb_DelStorage(0, item->m_byStorageIndex, 0, 1, "CPlayer::pcChatAllRequest()"))
    {
      return;
    }

    CMgrAvatorItemHistory::consume_del_item(&CPlayer::s_MgrItemHistory, m_ObjID.m_wIndex, item, m_szItemHistoryFileName);
  }

LABEL_AFTER_ITEM:
  if (g_Main.m_bAllRaceChatMoneyConsume)
  {
    SubDalant(g_Main.m_dwAllRaceChatMoney);
    SendMsg_AlterMoneyInform(0);
  }

  _announ_message_receipt_udp msg{};
  msg.byMessageType = 14;
  msg.dwSenderSerial = m_dwObjSerial;
  msg.bySenderRace = CPlayerDB::GetRaceCode(&m_Param);
  strcpy_0(msg.wszSenderName, CPlayerDB::GetCharNameW(&m_Param));
  msg.bySize = static_cast<unsigned __int8>(strlen_0(pwszChatData));
  memcpy_0(msg.wszChatData, pwszChatData, msg.bySize);
  msg.wszChatData[msg.bySize] = 0;
  msg.byPvpGrade = m_Param.m_byPvPGrade;

  unsigned __int8 type[2] = {2, 11};
  CChatStealSystem *stealSystem = CChatStealSystem::Instance();
  stealSystem->StealChatMsg(this, msg.byMessageType, pwszChatData);

  const int len = msg.size();
  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    CPlayer *target = &g_Player[j];
    if (target->m_bLive)
    {
      if (CPlayerDB::GetRaceCode(&target->m_Param) == CPlayerDB::GetRaceCode(&m_Param))
      {
        CNetProcess::LoadSendMsg(
          g_Network.m_pProcess[0],
          target->m_ObjID.m_wIndex,
          type,
          reinterpret_cast<char *>(&msg),
          len);
      }
    }
  }
}

void CPlayer::pc_ChatGmNoticeRequest(char *pwszChatData)
{
  if (m_pUserDB && m_byUserDgr == 2)
  {
    CMainThread::pc_AllUserGMNoticeInform(&g_Main, pwszChatData);
  }
}

void CPlayer::pc_ChatTradeRequestMsg(unsigned __int8 bySubType, char *pwszTradeMsg)
{
  if (m_pUserDB
      && !m_pUserDB->m_bChatLock
      && !IsPunished(0, true)
      && CPlayerDB::GetDalant(&m_Param) >= 0x3E8)
  {
    SubDalant(0x3E8u);
    SendMsg_AlterMoneyInform(0);

    const int level = CPlayerDB::GetLevel(&m_Param);
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      const int lv = CPlayerDB::GetLevel(&m_Param);
      const int race = CPlayerDB::GetRaceCode(&m_Param);
      CMoneySupplyMgr *mgr = CMoneySupplyMgr::Instance();
      CMoneySupplyMgr::UpdateFeeMoneyData(mgr, race, lv, 0x3E8u);
    }

    _announ_message_receipt_udp msg{};
    msg.byMessageType = 19;
    msg.dwSenderSerial = m_dwObjSerial;
    msg.bySenderRace = CPlayerDB::GetRaceCode(&m_Param);
    strcpy_0(msg.wszSenderName, CPlayerDB::GetCharNameW(&m_Param));
    msg.bySize = static_cast<unsigned __int8>(strlen_0(pwszTradeMsg));
    memcpy_0(msg.wszChatData, pwszTradeMsg, msg.bySize);
    msg.wszChatData[msg.bySize] = 0;
    msg.byPvpGrade = m_Param.m_byPvPGrade;
    msg.bySubType = bySubType;

    unsigned __int8 type[2] = {2, 11};
    CChatStealSystem *stealSystem = CChatStealSystem::Instance();
    stealSystem->StealChatMsg(this, msg.byMessageType, pwszTradeMsg);

    const int len = msg.size();
    for (int j = 0; j < MAX_PLAYER; ++j)
    {
      CPlayer *target = &g_Player[j];
      if (target->m_bLive)
      {
        if (CPlayerDB::GetRaceCode(&target->m_Param) == CPlayerDB::GetRaceCode(&m_Param))
        {
          CNetProcess::LoadSendMsg(
            g_Network.m_pProcess[0],
            target->m_ObjID.m_wIndex,
            type,
            reinterpret_cast<char *>(&msg),
            len);
        }
      }
    }
  }
}

void CPlayer::pc_ChatGuildRequest(unsigned int dwDstSerial, char *pwszChatData)
{
  char errCode = 0;
  CPlayer *dstPlayer = nullptr;

  if (m_Param.m_pGuild)
  {
    if (dwDstSerial != static_cast<unsigned int>(-1))
    {
      _guild_member_info *member = CGuild::GetMemberFromSerial(m_Param.m_pGuild, dwDstSerial);
      if (member)
      {
        if (member->pPlayer)
        {
          dstPlayer = member->pPlayer;
        }
        else
        {
          errCode = static_cast<char>(-49);
        }
      }
      else
      {
        errCode = static_cast<char>(-54);
      }
    }
  }
  else
  {
    errCode = static_cast<char>(-54);
  }

  if (errCode)
  {
    _chat_guild_failure_zocl msg{};
    msg.byErrCode = static_cast<unsigned __int8>(errCode);
    unsigned __int8 type[2] = {2, 100};
    CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), msg.size());
  }
  else
  {
    _announ_message_receipt_udp msg{};
    msg.bySenderRace = CPlayerDB::GetRaceCode(&m_Param);
    msg.dwSenderSerial = m_dwObjSerial;
    strcpy_0(msg.wszSenderName, CPlayerDB::GetCharNameW(&m_Param));
    msg.byPvpGrade = m_Param.m_byPvPGrade;
    msg.bySize = static_cast<unsigned __int8>(strlen_0(pwszChatData));
    memcpy_0(msg.wszChatData, pwszChatData, msg.bySize);
    msg.wszChatData[msg.bySize] = 0;

    unsigned __int8 type[2] = {2, 11};
    const int baseLen = msg.size();

    if (dstPlayer)
    {
      msg.byMessageType = 8;
      const unsigned __int16 len = msg.size();
      CNetProcess::LoadSendMsg(
        g_Network.m_pProcess[0],
        dstPlayer->m_ObjID.m_wIndex,
        type,
        reinterpret_cast<char *>(&msg),
        len);
    }
    else
    {
      msg.byMessageType = 7;
      for (int j = 0; j < 50; ++j)
      {
        _guild_member_info *member = &m_Param.m_pGuild->m_MemberData[j];
        if (_guild_member_info::IsFill(member))
        {
          if (member->pPlayer)
          {
            CNetProcess::LoadSendMsg(
              g_Network.m_pProcess[0],
              member->pPlayer->m_ObjID.m_wIndex,
              type,
              reinterpret_cast<char *>(&msg),
              baseLen);
          }
        }
      }
    }

    CChatStealSystem *stealSystem = CChatStealSystem::Instance();
    stealSystem->StealChatMsg(this, msg.byMessageType, pwszChatData);
  }
}

void CPlayer::pc_ChatMultiFarRequest(unsigned __int8 byDstNum, _w_name *pDstName, char *pwszMsg)
{
  unsigned __int8 dstCount = 0;
  CPlayer *targets[4]{};

  for (int j = 0; j < byDstNum; ++j)
  {
    CPlayer *dst = FindFarChatPlayerWithTemp(pDstName[j].name);
    char failCode = 0;
    if (dst)
    {
      if (dst->m_bBlockWhisper)
      {
        failCode = 2;
      }
      else if (dst == this)
      {
        failCode = 4;
      }
      else
      {
        for (int k = 0; k < dstCount; ++k)
        {
          if (targets[k] == dst)
          {
            failCode = 5;
            break;
          }
        }
        if (!failCode)
        {
          if (m_byUserDgr != 2 && dst->m_byUserDgr != 2
              && _effect_parameter::GetEff_Have(&dst->m_EP, 3) <= 0.0)
          {
            const int raceCode = CPlayerDB::GetRaceCode(&dst->m_Param);
            CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
            const unsigned int bossSerial = CPvpUserAndGuildRankingSystem::GetCurrentRaceBossSerial(rank, raceCode, 0);
            if (bossSerial != dst->m_dwObjSerial)
            {
              const int dstRace = CPlayerDB::GetRaceCode(&dst->m_Param);
              const int myRace = CPlayerDB::GetRaceCode(&m_Param);
              if (dstRace != myRace)
              {
                failCode = 3;
              }
            }
          }
        }
      }
    }
    else
    {
      failCode = 1;
    }

    if (failCode)
    {
      _chat_multi_far_failure_zocl fail{};
      strcpy_0(fail.wszDstName, pDstName[j].name);
      fail.byFailCode = static_cast<unsigned __int8>(failCode);
      unsigned __int8 type[2] = {2, 103};
      CNetProcess::LoadSendMsg(
        g_Network.m_pProcess[0],
        m_ObjID.m_wIndex,
        type,
        reinterpret_cast<char *>(&fail),
        fail.size());
    }
    else
    {
      if (dstCount < 4)
      {
        targets[dstCount++] = dst;
      }
    }
  }

  if (dstCount)
  {
    _chat_multi_far_trans_zocl msg{};
    int writePos = 0;
    char *sData = msg.sData;

    char *senderName = CPlayerDB::GetCharNameW(&m_Param);
    unsigned __int8 senderLen = static_cast<unsigned __int8>(strlen_0(senderName));
    memcpy_0(&sData[writePos++], &senderLen, sizeof(senderLen));
    memcpy_0(&sData[writePos], senderName, senderLen);
    writePos += senderLen;

    memcpy_0(&sData[writePos++], &dstCount, sizeof(dstCount));
    for (int m = 0; m < dstCount; ++m)
    {
      char *dstName = CPlayerDB::GetCharNameW(&targets[m]->m_Param);
      unsigned __int8 dstLen = static_cast<unsigned __int8>(strlen_0(dstName));
      memcpy_0(&sData[writePos++], &dstLen, sizeof(dstLen));
      memcpy_0(&sData[writePos], dstName, dstLen);
      writePos += dstLen;
    }

    unsigned __int8 msgLen = static_cast<unsigned __int8>(strlen_0(pwszMsg) + 1);
    memcpy_0(&sData[writePos++], &msgLen, sizeof(msgLen));
    memcpy_0(&sData[writePos], pwszMsg, msgLen);
    writePos += msgLen;

    msg.wSize = static_cast<unsigned __int16>(writePos);

    unsigned __int8 type[2] = {2, 102};
    CChatStealSystem *stealSystem = CChatStealSystem::Instance();
    stealSystem->StealChatMsg(this, 9u, pwszMsg);

    const unsigned __int16 len = msg.size();
    for (int m = 0; m < dstCount; ++m)
    {
      CNetProcess::LoadSendMsg(
        g_Network.m_pProcess[0],
        targets[m]->m_ObjID.m_wIndex,
        type,
        reinterpret_cast<char *>(&msg),
        len);
    }
    CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
  }
}

void CPlayer::pc_ChatRaceBossCryRequest(char *pwszChatData)
{
  if (m_pUserDB)
  {
    const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
    CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
    const unsigned int bossSerial = CPvpUserAndGuildRankingSystem::GetCurrentRaceBossSerial(rank, raceCode, 0);
    const unsigned int charSerial = CPlayerDB::GetCharSerial(&m_Param);
    if (bossSerial == charSerial)
    {
      _announ_message_receipt_udp msg{};
      msg.byMessageType = 18;
      msg.bySenderRace = CPlayerDB::GetRaceCode(&m_Param);
      strcpy_0(msg.wszSenderName, CPlayerDB::GetCharNameW(&m_Param));
      msg.bySize = static_cast<unsigned __int8>(strlen_0(pwszChatData));
      memcpy_0(msg.wszChatData, pwszChatData, msg.bySize);
      msg.wszChatData[msg.bySize] = 0;
      msg.dwSenderSerial = m_dwObjSerial;
      msg.byPvpGrade = static_cast<unsigned __int8>(-1);

      unsigned __int8 type[2] = {2, 11};
      const int len = msg.size();
      for (int j = 0; j < MAX_PLAYER; ++j)
      {
        CPlayer *target = &g_Player[j];
        if (target->m_bOper)
        {
          if (CPlayerDB::GetRaceCode(&target->m_Param) == CPlayerDB::GetRaceCode(&m_Param)
              && target->m_pCurMap == m_pCurMap
              && target->m_wMapLayerIndex == m_wMapLayerIndex)
          {
            CNetProcess::LoadSendMsg(
              g_Network.m_pProcess[0],
              target->m_ObjID.m_wIndex,
              type,
              reinterpret_cast<char *>(&msg),
              len);
          }
        }
      }
    }
  }
}

void CPlayer::SendMsg_ExchangeMoneyResult(char byErrCode)
{
  #pragma pack(push, 1)
  struct ExchangeMoneyResultMsg
  {
    char byErrCode;
    unsigned int dwGold;
    unsigned int dwDalant;
  };
  #pragma pack(pop)

  ExchangeMoneyResultMsg msg{};
  msg.byErrCode = byErrCode;
  msg.dwDalant = m_Param.GetDalant();
  msg.dwGold = m_Param.GetGold();

  unsigned __int8 pbyType[2] = {12, 13};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), 9u);
}

void CPlayer::SendMsg_InformTaxIncome(unsigned __int8 byRet, unsigned int dwComm, char *pwszDate)
{
  char monthBuffer[32]{};
  char dayBuffer[32]{};

  strncpy(monthBuffer, pwszDate + 4, 2u);
  monthBuffer[2] = 0;
  strncpy(dayBuffer, pwszDate + 6, 2u);
  dayBuffer[2] = 0;

  _pt_inform_commission_income_zocl msg{};
  msg.byRet = byRet;
  msg.byMonth = static_cast<unsigned __int8>(atoi(monthBuffer));
  msg.byDay = static_cast<unsigned __int8>(atoi(dayBuffer));
  msg.dwCommission = dwComm;

  unsigned __int8 pbyType[2] = {13, 122};
  const unsigned __int16 nLen = msg.size();
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), nLen);
}

void CPlayer::SendMsg_PostDelivery(
  char byIndex,
  unsigned int dwPostSerial,
  char *wszSendName,
  char *wszTitle,
  bool bItem,
  bool bGold,
  unsigned __int8 byState)
{
  _post_recv_delivery_zocl msg{};
  msg.byIndex = byIndex;
  msg.dwPostSerial = dwPostSerial;
  strcpy_s(msg.wszSendName, 0x11u, wszSendName);
  strcpy_s(msg.wszTitle, 0x15u, wszTitle);
  msg.bItem = bItem;
  msg.bGold = bGold;
  msg.byState = byState;

  unsigned __int8 pbyType[2] = {58, 4};
  const unsigned __int16 nLen = msg.size();
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), nLen);
}

void CPlayer::SendMsg_PostReturn(
  char byErrCode,
  unsigned int dwPostSerial,
  char *wszRecvName,
  char *wszTitle,
  char *wszContent,
  unsigned __int8 byTableCode,
  unsigned __int16 wItemIndex,
  unsigned __int64 dwDur,
  unsigned int dwLv,
  unsigned int dwGold)
{
  _post_return_zocl msg{};
  msg.byErrCode = byErrCode;
  msg.dwPostSerial = dwPostSerial;
  strcpy_s(msg.wszRecvName, 0x11u, wszRecvName);
  strcpy_s(msg.wszTitle, 0x15u, wszTitle);
  strcpy_s(msg.wszContent, 0xC9u, wszContent);
  msg.Item.byTableCode = byTableCode;
  msg.Item.wItemIndex = wItemIndex;
  msg.Item.dwDur = dwDur;
  msg.Item.dwLv = dwLv;
  msg.dwGold = dwGold;

  unsigned __int8 pbyType[2] = {58, 13};
  const unsigned __int16 nLen = msg.size();
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), nLen);
}

void CPlayer::SendMsg_PostContent(
  unsigned __int8 byErrCode,
  unsigned int dwPostSerial,
  char *wszContent,
  unsigned __int8 byTableCode,
  unsigned __int16 wItemIndex,
  unsigned __int64 dwDur,
  unsigned int dwLv,
  unsigned int dwGold)
{
  _post_content_result_zocl msg{};
  msg.byErrCode = byErrCode;
  msg.dwPostSerial = dwPostSerial;
  if (!byErrCode)
  {
    strcpy_s(msg.wszContent, 0xC9u, wszContent);
    msg.Item.byTableCode = byTableCode;
    msg.Item.wItemIndex = wItemIndex;
    msg.Item.dwDur = dwDur;
    msg.Item.dwLv = dwLv;
    msg.dwGold = dwGold;
  }

  unsigned __int8 pbyType[2] = {58, 6};
  const unsigned __int16 nLen = msg.size();
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), nLen);
}

CPlayer *GetPtrPlayerFromSerial(CPlayer *pData, int nNum, unsigned int dwSerial)
{
  for (int index = 0; index < nNum; ++index)
  {
    if (pData[index].m_bLive && pData[index].m_dwObjSerial == dwSerial)
    {
      return &pData[index];
    }
  }
  return nullptr;
}

CPlayer *GetPtrPlayerFromAccount(CPlayer *pData, int nNum, char *szAccount)
{
  for (int index = 0; index < nNum; ++index)
  {
    if (pData[index].m_bLive && !strcmp_0(pData[index].m_pUserDB->m_szAccountID, szAccount))
    {
      return &pData[index];
    }
  }
  return nullptr;
}

CPlayer *GetPtrPlayerFromAccountSerial(CPlayer *pData, int nNum, unsigned int dwSerial)
{
  for (int index = 0; index < nNum; ++index)
  {
    if (pData[index].m_bLive && pData[index].m_pUserDB->m_dwAccountSerial == dwSerial)
    {
      return &pData[index];
    }
  }
  return nullptr;
}

char CPlayer::_pre_check_normal_attack(
  CCharacter *pDst,
  unsigned __int16 wBulletSerial,
  bool bCount,
  _STORAGE_LIST::_db_con **ppBulletProp,
  _BulletItem_fld **ppfldBullet,
  unsigned __int16 wEffBtSerial,
  _STORAGE_LIST::_db_con **ppEffBtProp,
  _BulletItem_fld **ppfldEffBt)
{
  _STORAGE_LIST::_db_con *bulletItem = nullptr;
  _BulletItem_fld *bulletField = nullptr;
  _STORAGE_LIST::_db_con *effBulletItem = nullptr;
  _BulletItem_fld *effBulletField = nullptr;

  if (!bCount && !m_bSFDelayNotCheck
      && !_ATTACK_DELAY_CHECKER::IsDelay(&m_AttDelayChker, 0xFFu, 0xFFu, 0xFFu))
  {
    return static_cast<char>(-5);
  }

  if (!pDst)
  {
    return static_cast<char>(-6);
  }

  if (static_cast<unsigned int>(CPlayerDB::GetRaceCode(&m_Param)) == 2 && IsActingSiegeMode())
  {
    return static_cast<char>(-60);
  }

  if (!pDst->m_bLive || pDst->m_bCorpse || pDst->m_pCurMap != m_pCurMap || CGameObject::GetCurSecNum(pDst) == -1)
  {
    return static_cast<char>(-6);
  }

  if (pDst == this)
  {
    return static_cast<char>(-6);
  }

  if (pDst == m_pRecalledAnimusChar)
  {
    return static_cast<char>(-42);
  }

  const unsigned int guildResult = _pre_check_in_guild_battle(reinterpret_cast<CPlayer *>(pDst));
  if (guildResult)
  {
    return static_cast<char>(guildResult);
  }

  if (!IsAttackableInTown() && !pDst->IsAttackableInTown(pDst) && (IsInTown() || pDst->IsInTown(pDst)))
  {
    return static_cast<char>(-31);
  }

  if (!pDst->IsBeDamagedAble(pDst, this))
  {
    return static_cast<char>(-6);
  }

  if (!pDst->IsBeAttackedAble(pDst, true))
  {
    return static_cast<char>(-6);
  }

  if (m_pmWpn.byWpType == 7)
  {
    if (!m_bFreeSFByClass)
    {
      bool hasActive = false;
      for (int j = 0; j < 4; ++j)
      {
        void *effect = m_Param.m_ppHistoryEffect[j];
        if (!effect)
        {
          break;
        }
        if (*reinterpret_cast<int *>(reinterpret_cast<char *>(effect) + 1444))
        {
          hasActive = true;
          break;
        }
      }
      if (!hasActive)
      {
        return static_cast<char>(-27);
      }
    }
  }
  else if (m_pmWpn.byWpType == 11 || m_pmWpn.byWpType == 10)
  {
    return static_cast<char>(-9);
  }

  if (IsRidingUnit())
  {
    return static_cast<char>(-21);
  }

  if (_effect_parameter::GetEff_State(&m_EP, 20))
  {
    return static_cast<char>(-37);
  }

  if (_effect_parameter::GetEff_State(&m_EP, 28))
  {
    return static_cast<char>(-37);
  }

  if (_effect_parameter::GetEff_State(&m_EP, 21))
  {
    return static_cast<char>(-38);
  }

  if (m_byMoveType == 2)
  {
    return static_cast<char>(-41);
  }

  if (wBulletSerial != 0xFFFF)
  {
    bulletItem = IsBulletValidity(wBulletSerial);
    if (!bulletItem)
    {
      SendMsg_AdjustAmountInform(2u, wBulletSerial, 0);
      return static_cast<char>(-17);
    }
    bulletField = reinterpret_cast<_BulletItem_fld *>(
      CRecordData::GetRecord(&g_Main.m_tblItemData[10], bulletItem->m_wItemIndex));
    if (bulletItem->m_bLock)
    {
      return static_cast<char>(-29);
    }
  }

  if ((m_pmWpn.byWpType == 5 || m_pmWpn.byWpType == 6 || m_pmWpn.byWpType == 7 || m_pmWpn.byWpType == 11)
      && !bulletField)
  {
    return static_cast<char>(-17);
  }

  if (wEffBtSerial != 0xFFFF)
  {
    effBulletItem = IsEffBulletValidity(wEffBtSerial);
    if (!effBulletItem)
    {
      SendMsg_AdjustAmountInform(2u, wEffBtSerial, 0);
      return static_cast<char>(-63);
    }
    effBulletField = reinterpret_cast<_BulletItem_fld *>(
      CRecordData::GetRecord(&g_Main.m_tblItemData[10], effBulletItem->m_wItemIndex));
    *ppEffBtProp = effBulletItem;
    *ppfldEffBt = effBulletField;
  }

  float attackRange = 0.0f;
  if (m_pmWpn.byWpType == 7)
  {
    const float baseRange = static_cast<float>(m_pmWpn.wGaAttRange);
    const float width = pDst->GetWidth(pDst);
    const float rangeWithWidth = baseRange + (width / 2.0f);
    const float effPlus = _effect_parameter::GetEff_Plus(&m_EP, 36);
    attackRange = rangeWithWidth + effPlus;
  }
  else
  {
    const float baseRange = static_cast<float>(m_pmWpn.wGaAttRange);
    const float width = pDst->GetWidth(pDst);
    const float rangeWithWidth = baseRange + (width / 2.0f);
    const float effPlus = _effect_parameter::GetEff_Plus(&m_EP, m_pmWpn.byWpClass + 4);
    attackRange = rangeWithWidth + effPlus;
  }

  float dist = GetSqrt(pDst->m_fCurPos, m_fCurPos);
  if (dist > attackRange)
  {
    dist = GetSqrt(pDst->m_fOldPos, m_fCurPos);
    if (dist > attackRange)
    {
      return static_cast<char>(-3);
    }
  }

  *ppBulletProp = bulletItem;
  *ppfldBullet = bulletField;
  return 0;
}

char CPlayer::_pre_check_skill_attack(
  CCharacter *pDst,
  float *pfAttackPos,
  unsigned __int8 byEffectCode,
  _skill_fld *pSkillFld,
  unsigned __int16 wBulletSerial,
  _STORAGE_LIST::_db_con **ppBulletProp,
  _BulletItem_fld **ppfldBullet,
  int nEffectGroup,
  unsigned __int16 *pdwDecPoint,
  unsigned __int16 wEffBtSerial,
  _STORAGE_LIST::_db_con **ppEffBtProp,
  _BulletItem_fld **ppfldEffBt)
{
  _STORAGE_LIST::_db_con *bulletItem = nullptr;
  _BulletItem_fld *bulletField = nullptr;
  _STORAGE_LIST::_db_con *effBulletItem = nullptr;
  _BulletItem_fld *effBulletField = nullptr;

  if (!pSkillFld->m_nAttackable)
  {
    return static_cast<char>(-50);
  }

  if (m_pmWpn.byWpType == 10)
  {
    return static_cast<char>(-9);
  }

  if (static_cast<unsigned int>(CPlayerDB::GetRaceCode(&m_Param)) == 2 && IsActingSiegeMode())
  {
    return static_cast<char>(-60);
  }

  if (pDst)
  {
    if (pDst == this)
    {
      return static_cast<char>(-6);
    }

    if (!pDst->m_bLive || pDst->m_bCorpse || pDst->m_pCurMap != m_pCurMap || CGameObject::GetCurSecNum(pDst) == -1)
    {
      return static_cast<char>(-6);
    }

    if (pDst == m_pRecalledAnimusChar)
    {
      return static_cast<char>(-42);
    }

    const unsigned int guildResult = _pre_check_in_guild_battle(reinterpret_cast<CPlayer *>(pDst));
    if (guildResult)
    {
      return static_cast<char>(guildResult);
    }

    if (!pDst->IsBeAttackedAble(pDst, true))
    {
      return static_cast<char>(-6);
    }

    if (!IsAttackableInTown() && !pDst->IsAttackableInTown(pDst) && (IsInTown() || pDst->IsInTown(pDst)))
    {
      return static_cast<char>(-31);
    }

    if (!pDst->IsBeDamagedAble(pDst, this))
    {
      return static_cast<char>(-6);
    }
  }
  else if (nEffectGroup != 4 && nEffectGroup != 6)
  {
    return static_cast<char>(-6);
  }

  if (!pSkillFld)
  {
    return static_cast<char>(-60);
  }

  if (!IsSFUsableGauge(byEffectCode, pSkillFld->m_dwIndex, pdwDecPoint))
  {
    return static_cast<char>(-25);
  }

  if (wBulletSerial != 0xFFFF)
  {
    bulletItem = IsBulletValidity(wBulletSerial);
    if (!bulletItem)
    {
      SendMsg_AdjustAmountInform(2u, wBulletSerial, 0);
      return static_cast<char>(-17);
    }
    bulletField = reinterpret_cast<_BulletItem_fld *>(
      CRecordData::GetRecord(&g_Main.m_tblItemData[10], bulletItem->m_wItemIndex));
    if (bulletItem->m_bLock)
    {
      return static_cast<char>(-29);
    }
  }

  if ((m_pmWpn.byWpType == 5 || m_pmWpn.byWpType == 6 || m_pmWpn.byWpType == 7 || m_pmWpn.byWpType == 11)
      && !bulletField)
  {
    return static_cast<char>(-17);
  }

  if (wEffBtSerial != 0xFFFF)
  {
    effBulletItem = IsEffBulletValidity(wEffBtSerial);
    if (!effBulletItem)
    {
      SendMsg_AdjustAmountInform(2u, wEffBtSerial, 0);
      return static_cast<char>(-63);
    }
    effBulletField = reinterpret_cast<_BulletItem_fld *>(
      CRecordData::GetRecord(&g_Main.m_tblItemData[10], effBulletItem->m_wItemIndex));
    *ppEffBtProp = effBulletItem;
    *ppfldEffBt = effBulletField;
  }

  int classGrade[4]{};
  classGrade[0] = -1;
  if (byEffectCode)
  {
    if (byEffectCode == 2)
    {
      if (!CPlayerDB::IsActableClassSkill(&m_Param, pSkillFld->m_strCode, classGrade))
      {
        return static_cast<char>(-15);
      }
    }
    else if (byEffectCode == 3)
    {
      if (!bulletField)
      {
        return static_cast<char>(-17);
      }
      if (strcmp_0(bulletField->m_strEffectIndex, pSkillFld->m_strCode))
      {
        return static_cast<char>(-15);
      }
    }
  }
  else
  {
    if (pSkillFld->m_nMastIndex > 8u)
    {
      return static_cast<char>(-60);
    }
    if (!IsSFUsableSFMastery(3u, pSkillFld->m_nMastIndex))
    {
      return static_cast<char>(-14);
    }
    if (!IsSFActableByClass(0, pSkillFld))
    {
      return static_cast<char>(-24);
    }
  }

  unsigned __int8 masteryIndex = static_cast<unsigned __int8>(-1);
  if (byEffectCode)
  {
    if (byEffectCode == 2)
    {
      masteryIndex = static_cast<unsigned __int8>(classGrade[0]);
    }
  }
  else if (pSkillFld->m_nMastIndex < 8u)
  {
    masteryIndex = static_cast<unsigned __int8>(pSkillFld->m_nMastIndex);
  }

  if (!m_bSFDelayNotCheck
      && !_ATTACK_DELAY_CHECKER::IsDelay(&m_AttDelayChker, byEffectCode, pSkillFld->m_dwIndex, masteryIndex))
  {
    return static_cast<char>(-5);
  }

  if (IsRidingUnit())
  {
    return static_cast<char>(-21);
  }

  if (_effect_parameter::GetEff_State(&m_EP, 0))
  {
    return static_cast<char>(-50);
  }

  if (_effect_parameter::GetEff_State(&m_EP, 20))
  {
    return static_cast<char>(-37);
  }

  if (_effect_parameter::GetEff_State(&m_EP, 28))
  {
    return static_cast<char>(-37);
  }

  if (_effect_parameter::GetEff_State(&m_EP, 21))
  {
    return static_cast<char>(-38);
  }

  if (m_byMoveType == 2)
  {
    return static_cast<char>(-41);
  }

  bool weaponOk = false;
  if (m_pmWpn.GetAttackToolType() == 1)
  {
    if (pSkillFld->m_strFixWeapon[m_pmWpn.byWpType] == 49)
    {
      weaponOk = true;
    }
  }
  else if (!strstr(pSkillFld->m_strFixWeapon, "1"))
  {
    weaponOk = true;
  }

  if (!weaponOk)
  {
    return static_cast<char>(-9);
  }

  if (pDst)
  {
    float attackRange = 0.0f;
    if (m_pmWpn.byWpType == 7)
    {
      const float baseRange = static_cast<float>(m_pmWpn.wGaAttRange);
      const float width = pDst->GetWidth(pDst);
      const float rangeWithWidth = baseRange + (width / 2.0f);
      const float effPlus = _effect_parameter::GetEff_Plus(&m_EP, 36);
      const float rangePlus = rangeWithWidth + effPlus;
      const float classPlus = _effect_parameter::GetEff_Plus(&m_EP, m_pmWpn.byWpClass + 6);
      attackRange = (rangePlus + classPlus) + static_cast<float>(pSkillFld->m_nBonusDistance);
    }
    else
    {
      const float baseRange = static_cast<float>(m_pmWpn.wGaAttRange);
      const float width = pDst->GetWidth(pDst);
      const float rangeWithWidth = baseRange + (width / 2.0f);
      const float effPlus = _effect_parameter::GetEff_Plus(&m_EP, m_pmWpn.byWpClass + 4);
      const float rangePlus = rangeWithWidth + effPlus;
      const float classPlus = _effect_parameter::GetEff_Plus(&m_EP, m_pmWpn.byWpClass + 6);
      attackRange = (rangePlus + classPlus) + static_cast<float>(pSkillFld->m_nBonusDistance);
    }

    if (IsSiegeMode())
    {
      _base_fld *siegeRecord = CRecordData::GetRecord(&g_Main.m_tblItemData[27], m_pSiegeItem->m_wItemIndex);
      attackRange = attackRange + *reinterpret_cast<float *>(&siegeRecord[5].m_strCode[16]);
    }

    float dist = GetSqrt(pDst->m_fCurPos, m_fCurPos);
    if (dist > attackRange)
    {
      dist = GetSqrt(pDst->m_fOldPos, m_fCurPos);
      if (dist > attackRange)
      {
        return static_cast<char>(-3);
      }
    }

    if (IsSiegeMode())
    {
      _base_fld *siegeRecord = CRecordData::GetRecord(&g_Main.m_tblItemData[27], m_pSiegeItem->m_wItemIndex);
      if (*reinterpret_cast<float *>(&siegeRecord[5].m_strCode[12]) > dist)
      {
        return static_cast<char>(-3);
      }
    }
  }
  else if (nEffectGroup != 4)
  {
    float attackRange = 0.0f;
    if (m_pmWpn.byWpType == 7)
    {
      const float baseRange = static_cast<float>(m_pmWpn.wGaAttRange);
      const float effPlus = _effect_parameter::GetEff_Plus(&m_EP, 36);
      const float rangePlus = baseRange + effPlus;
      const float classPlus = _effect_parameter::GetEff_Plus(&m_EP, m_pmWpn.byWpClass + 6);
      attackRange = (rangePlus + classPlus) + static_cast<float>(pSkillFld->m_nBonusDistance);
    }
    else
    {
      const float baseRange = static_cast<float>(m_pmWpn.wGaAttRange);
      const float effPlus = _effect_parameter::GetEff_Plus(&m_EP, m_pmWpn.byWpClass + 4);
      const float rangePlus = baseRange + effPlus;
      const float classPlus = _effect_parameter::GetEff_Plus(&m_EP, m_pmWpn.byWpClass + 6);
      attackRange = (rangePlus + classPlus) + static_cast<float>(pSkillFld->m_nBonusDistance);
    }

    if (IsSiegeMode())
    {
      _base_fld *siegeRecord = CRecordData::GetRecord(&g_Main.m_tblItemData[27], m_pSiegeItem->m_wItemIndex);
      attackRange = attackRange + *reinterpret_cast<float *>(&siegeRecord[5].m_strCode[16]);
    }

    const float dist = GetSqrt(pfAttackPos, m_fCurPos);
    if (dist > attackRange)
    {
      return static_cast<char>(-3);
    }

    if (IsSiegeMode())
    {
      _base_fld *siegeRecord = CRecordData::GetRecord(&g_Main.m_tblItemData[27], m_pSiegeItem->m_wItemIndex);
      if (*reinterpret_cast<float *>(&siegeRecord[5].m_strCode[12]) > dist)
      {
        return static_cast<char>(-3);
      }
    }
  }

  *ppBulletProp = bulletItem;
  *ppfldBullet = bulletField;
  return 0;
}

char CPlayer::_pre_check_force_attack(
  CCharacter *pDst,
  float *pfTarPos,
  unsigned __int16 wForceItemSerial,
  _force_fld **ppForceFld,
  _STORAGE_LIST::_db_con **ppForceItem,
  unsigned __int16 *pdwDecPoint,
  unsigned __int16 wEffBtSerial,
  _STORAGE_LIST::_db_con **ppEffBtProp,
  _BulletItem_fld **ppfldEffBt)
{
  _force_fld *forceField = nullptr;
  _STORAGE_LIST::_db_con *forceItem = nullptr;
  _STORAGE_LIST::_db_con *effBulletItem = nullptr;
  _BulletItem_fld *effBulletField = nullptr;

  if (IsRidingUnit())
  {
    return static_cast<char>(-21);
  }

  if (_effect_parameter::GetEff_State(&m_EP, 20))
  {
    return static_cast<char>(-37);
  }

  if (_effect_parameter::GetEff_State(&m_EP, 28))
  {
    return static_cast<char>(-37);
  }

  if (_effect_parameter::GetEff_State(&m_EP, 21))
  {
    return static_cast<char>(-38);
  }

  if (m_byMoveType == 2)
  {
    return static_cast<char>(-41);
  }

  forceItem = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbForce, wForceItemSerial);
  if (!forceItem)
  {
    return static_cast<char>(-16);
  }

  if (forceItem->m_bLock)
  {
    return static_cast<char>(-30);
  }

  forceField = reinterpret_cast<_force_fld *>(
    CRecordData::GetRecord(&g_Main.m_tblEffectData[1], CPlayer::s_pnLinkForceItemToEffect[forceItem->m_wItemIndex]));
  if (!forceField)
  {
    return static_cast<char>(-16);
  }

  unsigned __int8 masteryIndex = static_cast<unsigned __int8>(-1);
  if (static_cast<unsigned int>(forceField->m_nMastIndex) < 0x18u)
  {
    masteryIndex = static_cast<unsigned __int8>(forceField->m_nMastIndex);
  }

  if (!m_bSFDelayNotCheck
      && !_ATTACK_DELAY_CHECKER::IsDelay(&m_AttDelayChker, 1u, forceField->m_dwIndex, masteryIndex))
  {
    return static_cast<char>(-5);
  }

  if (m_pmWpn.byWpType == 11 || m_pmWpn.byWpType == 10)
  {
    return static_cast<char>(-9);
  }

  if (forceField->m_nEffectGroup == 4 || forceField->m_nEffectGroup == 6)
  {
    pDst = nullptr;
  }

  if (static_cast<unsigned int>(CPlayerDB::GetRaceCode(&m_Param)) == 2 && IsActingSiegeMode())
  {
    return static_cast<char>(-60);
  }

  if (pDst)
  {
    if (pDst == this)
    {
      return static_cast<char>(-6);
    }

    if (!pDst->m_bLive || pDst->m_bCorpse || pDst->m_pCurMap != m_pCurMap || CGameObject::GetCurSecNum(pDst) == -1)
    {
      return static_cast<char>(-6);
    }

    if (pDst == m_pRecalledAnimusChar)
    {
      return static_cast<char>(-42);
    }

    const unsigned int guildResult = _pre_check_in_guild_battle(reinterpret_cast<CPlayer *>(pDst));
    if (guildResult)
    {
      return static_cast<char>(guildResult);
    }

    if (!pDst->IsBeAttackedAble(pDst, true))
    {
      return static_cast<char>(-6);
    }

    if (!IsAttackableInTown() && !pDst->IsAttackableInTown(pDst) && (IsInTown() || pDst->IsInTown(pDst)))
    {
      return static_cast<char>(-31);
    }

    if (!pDst->IsBeDamagedAble(pDst, this))
    {
      return static_cast<char>(-6);
    }
  }
  else if (forceField->m_nEffectGroup != 4 && forceField->m_nEffectGroup != 6)
  {
    return static_cast<char>(-6);
  }

  if (!IsSFUsableGauge(1u, forceField->m_dwIndex, pdwDecPoint))
  {
    return static_cast<char>(-25);
  }

  if (!IsSFUsableSFMastery(4u, forceField->m_nMastIndex))
  {
    return static_cast<char>(-14);
  }

  if (!IsSFActableByClass(1u, forceField))
  {
    return static_cast<char>(-24);
  }

  if (_effect_parameter::GetEff_State(&m_EP, 1))
  {
    return static_cast<char>(-50);
  }

  if (_effect_parameter::GetEff_State(&m_EP, 20))
  {
    return static_cast<char>(-37);
  }

  if (_effect_parameter::GetEff_State(&m_EP, 28))
  {
    return static_cast<char>(-37);
  }

  if (_effect_parameter::GetEff_State(&m_EP, 21))
  {
    return static_cast<char>(-38);
  }

  if (m_pmWpn.byWpType != 9)
  {
    return static_cast<char>(-9);
  }

  if (!forceField->m_bAttackable)
  {
    return static_cast<char>(-50);
  }

  if (forceField->m_nEffectGroup == 6 && !CMapData::IsMapIn(m_pCurMap, pfTarPos))
  {
    return static_cast<char>(-50);
  }

  if (pDst)
  {
    const float rangeBase = static_cast<float>(forceField->m_nActDistance + 40);
    const float width = pDst->GetWidth(pDst);
    const float rangeWithWidth = rangeBase + (width / 2.0f);
    const float effPlus = _effect_parameter::GetEff_Plus(&m_EP, 8);
    const int range = static_cast<int>(rangeWithWidth + effPlus);
    float dist = GetSqrt(pDst->m_fCurPos, m_fCurPos);
    if (dist > static_cast<float>(range))
    {
      dist = GetSqrt(pDst->m_fOldPos, m_fCurPos);
      if (dist > static_cast<float>(range))
      {
        return static_cast<char>(-3);
      }
    }
  }
  else if (forceField->m_nEffectGroup != 4)
  {
    int range = forceField->m_nActDistance + 40;
    if (forceField->m_nEffectGroup == 6 || forceField->m_nEffectGroup == 5 || forceField->m_nEffectGroup == 7)
    {
      const float effPlus = _effect_parameter::GetEff_Plus(&m_EP, 8);
      range = static_cast<int>(static_cast<float>(range) + effPlus);
    }

    float dist = GetSqrt(pfTarPos, m_fCurPos);
    if (dist > static_cast<float>(range))
    {
      dist = GetSqrt(pfTarPos, m_fOldPos);
      if (dist > static_cast<float>(range))
      {
        return static_cast<char>(-3);
      }
    }
  }

  *ppForceFld = forceField;
  *ppForceItem = forceItem;

  if (wEffBtSerial != 0xFFFF)
  {
    effBulletItem = IsEffBulletValidity(wEffBtSerial);
    if (!effBulletItem)
    {
      SendMsg_AdjustAmountInform(2u, wEffBtSerial, 0);
      return static_cast<char>(-63);
    }
    effBulletField = reinterpret_cast<_BulletItem_fld *>(
      CRecordData::GetRecord(&g_Main.m_tblItemData[10], effBulletItem->m_wItemIndex));
    *ppEffBtProp = effBulletItem;
    *ppfldEffBt = effBulletField;
  }

  return 0;
}

char CPlayer::_pre_check_unit_attack(
  CCharacter *pDst,
  unsigned __int8 byWeaponPart,
  _UnitPart_fld **ppWeaponFld,
  _UnitBullet_fld **ppBulletFld,
  _unit_bullet_param **ppBulletParam)
{
  _UnitPart_fld *weaponField = nullptr;
  _UnitBullet_fld *bulletField = nullptr;
  _unit_bullet_param *bulletParam = nullptr;

  if (!m_bSFDelayNotCheck && !_ATTACK_DELAY_CHECKER::IsDelay(&m_AttDelayChker, 0xFFu, 0xFFu, 0xFFu))
  {
    return static_cast<char>(-5);
  }

  if (!pDst)
  {
    return static_cast<char>(-6);
  }

  if (!IsRidingUnit())
  {
    return static_cast<char>(-21);
  }

  if (m_byMoveType == 2)
  {
    return static_cast<char>(-41);
  }

  if (_effect_parameter::GetEff_State(&m_EP, 20))
  {
    return static_cast<char>(-37);
  }

  if (_effect_parameter::GetEff_State(&m_EP, 28))
  {
    return static_cast<char>(-37);
  }

  bulletParam = reinterpret_cast<_unit_bullet_param *>(&m_pUsingUnit->dwBullet[byWeaponPart]);
  if (!bulletParam->wLeftNum || bulletParam->wLeftNum == 0xFFFF)
  {
    return static_cast<char>(-17);
  }

  bulletField = reinterpret_cast<_UnitBullet_fld *>(CRecordData::GetRecord(&g_Main.m_tblUnitBullet, bulletParam->wBulletIndex));
  if (!bulletField)
  {
    return static_cast<char>(-60);
  }

  if (byWeaponPart)
  {
    weaponField = reinterpret_cast<_UnitPart_fld *>(CRecordData::GetRecord(&g_Main.m_tblUnitPart[4], m_pUsingUnit->byPart[4]));
  }
  else
  {
    weaponField = reinterpret_cast<_UnitPart_fld *>(CRecordData::GetRecord(&g_Main.m_tblUnitPart[3], m_pUsingUnit->byPart[3]));
  }

  if (!weaponField)
  {
    return static_cast<char>(-60);
  }

  if (pDst == this)
  {
    return static_cast<char>(-6);
  }

  if (!pDst->m_bLive || pDst->m_bCorpse || pDst->m_pCurMap != m_pCurMap || CGameObject::GetCurSecNum(pDst) == -1)
  {
    return static_cast<char>(-6);
  }

  if (!IsAttackableInTown() && !pDst->IsAttackableInTown(pDst) && (IsInTown() || pDst->IsInTown(pDst)))
  {
    return static_cast<char>(-31);
  }

  if (!pDst->IsBeDamagedAble(pDst, this))
  {
    return static_cast<char>(-6);
  }

  const unsigned int guildResult = _pre_check_in_guild_battle(reinterpret_cast<CPlayer *>(pDst));
  if (guildResult)
  {
    return static_cast<char>(guildResult);
  }

  if (!pDst->IsBeAttackedAble(pDst, true))
  {
    return static_cast<char>(-6);
  }

  float dist = GetSqrt(pDst->m_fCurPos, m_fCurPos);
  if (dist > static_cast<float>(weaponField->m_fAttackRange + 40.0))
  {
    dist = GetSqrt(pDst->m_fOldPos, m_fCurPos);
    if (dist > static_cast<float>(weaponField->m_fAttackRange + 40.0))
    {
      return static_cast<char>(-3);
    }
  }

  *ppWeaponFld = weaponField;
  *ppBulletFld = bulletField;
  *ppBulletParam = bulletParam;
  return 0;
}

char CPlayer::_pre_check_siege_attack(
  CCharacter *pDst,
  float *pfAttackPos,
  unsigned __int16 wBulletSerial,
  _STORAGE_LIST::_db_con **ppBulletProp,
  _BulletItem_fld **ppfldBullet,
  unsigned __int16 wEffBtSerial,
  _STORAGE_LIST::_db_con **ppEffBulletProp,
  _BulletItem_fld **ppfldEffBullet)
{
  _STORAGE_LIST::_db_con *bulletItem = nullptr;
  _BulletItem_fld *bulletField = nullptr;
  _STORAGE_LIST::_db_con *effBulletItem = nullptr;
  _BulletItem_fld *effBulletField = nullptr;

  if (!m_bSFDelayNotCheck && !_ATTACK_DELAY_CHECKER::IsDelay(&m_AttDelayChker, 0xFFu, 0xFFu, 0xFFu))
  {
    return static_cast<char>(-5);
  }

  if (!IsSiegeMode() || IsActingSiegeMode())
  {
    return static_cast<char>(-34);
  }

  if (m_byMoveType == 2)
  {
    return static_cast<char>(-41);
  }

  if (!m_pSiegeItem->m_dwDur)
  {
    return static_cast<char>(-35);
  }

  _base_fld *siegeRecord = CRecordData::GetRecord(&g_Main.m_tblItemData[27], m_pSiegeItem->m_wItemIndex);
  if (*reinterpret_cast<int *>(&siegeRecord[3].m_strCode[4]) != m_pmWpn.byWpType)
  {
    return static_cast<char>(-33);
  }

  if (m_pmWpn.byWpType == 7)
  {
    if (!m_bFreeSFByClass)
    {
      bool hasActive = false;
      for (int j = 0; j < 4; ++j)
      {
        void *effect = m_Param.m_ppHistoryEffect[j];
        if (!effect)
        {
          break;
        }
        if (*reinterpret_cast<int *>(reinterpret_cast<char *>(effect) + 1444))
        {
          hasActive = true;
          break;
        }
      }
      if (!hasActive)
      {
        return static_cast<char>(-27);
      }
    }
  }
  else if (m_pmWpn.byWpType == 11 || m_pmWpn.byWpType == 10)
  {
    return static_cast<char>(-9);
  }

  if (wBulletSerial == 0xFFFF && m_pmWpn.byWpClass)
  {
    return static_cast<char>(-26);
  }

  if (wBulletSerial != 0xFFFF)
  {
    bulletItem = IsBulletValidity(wBulletSerial);
    if (!bulletItem)
    {
      SendMsg_AdjustAmountInform(2u, wBulletSerial, 0);
      return static_cast<char>(-17);
    }
    bulletField = reinterpret_cast<_BulletItem_fld *>(
      CRecordData::GetRecord(&g_Main.m_tblItemData[10], bulletItem->m_wItemIndex));
    if (bulletItem->m_bLock)
    {
      return static_cast<char>(-29);
    }
  }

  if (wEffBtSerial != 0xFFFF)
  {
    effBulletItem = IsEffBulletValidity(wEffBtSerial);
    if (!effBulletItem)
    {
      SendMsg_AdjustAmountInform(2u, wEffBtSerial, 0);
      return static_cast<char>(-63);
    }
    effBulletField = reinterpret_cast<_BulletItem_fld *>(
      CRecordData::GetRecord(&g_Main.m_tblItemData[10], effBulletItem->m_wItemIndex));
    if (effBulletItem->m_bLock)
    {
      return static_cast<char>(-29);
    }
  }

  if (pDst)
  {
    if (pDst == this)
    {
      return static_cast<char>(-6);
    }

    if (!pDst->m_bLive || pDst->m_bCorpse || pDst->m_pCurMap != m_pCurMap || CGameObject::GetCurSecNum(pDst) == -1)
    {
      return static_cast<char>(-6);
    }

    const unsigned int guildResult = _pre_check_in_guild_battle(reinterpret_cast<CPlayer *>(pDst));
    if (guildResult)
    {
      return static_cast<char>(guildResult);
    }

    if (!pDst->IsBeAttackedAble(pDst, true))
    {
      return static_cast<char>(-6);
    }

    float attackRange = 0.0f;
    if (m_pmWpn.byWpType == 7)
    {
      const float baseRange = static_cast<float>(m_pmWpn.wGaAttRange);
      const float width = pDst->GetWidth(pDst);
      const float rangeWithWidth = baseRange + (width / 2.0f);
      attackRange = rangeWithWidth + *reinterpret_cast<float *>(&siegeRecord[5].m_strCode[16]);
    }
    else
    {
      const float baseRange = static_cast<float>(m_pmWpn.wGaAttRange);
      const float width = pDst->GetWidth(pDst);
      const float rangeWithWidth = baseRange + (width / 2.0f);
      const float effPlus = _effect_parameter::GetEff_Plus(&m_EP, m_pmWpn.byWpClass + 4);
      attackRange = (rangeWithWidth + *reinterpret_cast<float *>(&siegeRecord[5].m_strCode[16])) + effPlus;
    }

    float dist = GetSqrt(pDst->m_fCurPos, m_fCurPos);
    if (dist > attackRange)
    {
      dist = GetSqrt(pDst->m_fOldPos, m_fCurPos);
      if (dist > attackRange)
      {
        return static_cast<char>(-3);
      }
    }

    if (*reinterpret_cast<float *>(&siegeRecord[5].m_strCode[12]) > dist)
    {
      return static_cast<char>(-3);
    }

    if (!IsAttackableInTown() && !pDst->IsAttackableInTown(pDst) && (IsInTown() || pDst->IsInTown(pDst)))
    {
      return static_cast<char>(-31);
    }

    if (!pDst->IsBeDamagedAble(pDst, this))
    {
      return static_cast<char>(-6);
    }
  }
  else
  {
    if (bulletField->m_nEffectGroup != 4 && bulletField->m_nEffectGroup != 6)
    {
      return static_cast<char>(-6);
    }

    if (!CMapData::IsMapIn(m_pCurMap, pfAttackPos))
    {
      return static_cast<char>(-36);
    }

    float attackRange = 0.0f;
    if (m_pmWpn.byWpType == 7)
    {
      const float baseRange = static_cast<float>(m_pmWpn.wGaAttRange) + *reinterpret_cast<float *>(&siegeRecord[5].m_strCode[16]);
      const float effPlus = _effect_parameter::GetEff_Plus(&m_EP, 36);
      attackRange = baseRange + effPlus;
    }
    else
    {
      const float baseRange = static_cast<float>(m_pmWpn.wGaAttRange) + *reinterpret_cast<float *>(&siegeRecord[5].m_strCode[16]);
      const float effPlus = _effect_parameter::GetEff_Plus(&m_EP, m_pmWpn.byWpClass + 4);
      attackRange = baseRange + effPlus;
    }

    const float dist = GetSqrt(pfAttackPos, m_fCurPos);
    if (dist > attackRange)
    {
      return static_cast<char>(-3);
    }

    if (*reinterpret_cast<float *>(&siegeRecord[5].m_strCode[12]) > dist)
    {
      return static_cast<char>(-3);
    }
  }

  *ppBulletProp = bulletItem;
  *ppfldBullet = bulletField;
  *ppEffBulletProp = effBulletItem;
  *ppfldEffBullet = effBulletField;
  return 0;
}

char CPlayer::_pre_check_wpactive_skill_attack(
  unsigned __int8 byEffectCode,
  _skill_fld *pSkillFld,
  unsigned __int16 wBulletSerial,
  _STORAGE_LIST::_db_con **ppBulletProp,
  _BulletItem_fld **ppfldBullet)
{
  _STORAGE_LIST::_db_con *bulletItem = nullptr;
  _BulletItem_fld *bulletField = nullptr;

  if (IsSiegeMode())
  {
    return 0;
  }

  if (IsRidingUnit())
  {
    return 0;
  }

  if (wBulletSerial != 0xFFFF)
  {
    bulletItem = IsBulletValidity(wBulletSerial);
    if (!bulletItem)
    {
      SendMsg_AdjustAmountInform(2u, wBulletSerial, 0);
      return 0;
    }
    bulletField = reinterpret_cast<_BulletItem_fld *>(
      CRecordData::GetRecord(&g_Main.m_tblItemData[10], bulletItem->m_wItemIndex));
    if (bulletItem->m_bLock)
    {
      return 0;
    }
  }

  if ((m_pmWpn.byWpType == 5 || m_pmWpn.byWpType == 6 || m_pmWpn.byWpType == 7 || m_pmWpn.byWpType == 11)
      && !bulletField)
  {
    return 0;
  }

  if (byEffectCode)
  {
    if (byEffectCode == 3 && strcmp_0(bulletField->m_strEffectIndex, pSkillFld->m_strCode))
    {
      return 0;
    }
  }
  else if (pSkillFld->m_nMastIndex > 8u)
  {
    return 0;
  }

  *ppBulletProp = bulletItem;
  *ppfldBullet = bulletField;
  return 1;
}

bool CPlayer::_pre_check_wpactive_force_attack()
{
  return !IsSiegeMode() && !IsRidingUnit();
}

int CPlayer::_pre_check_in_guild_battle(CPlayer *pDst)
{
  if (m_bInGuildBattle)
  {
    CPlayer *battleOwner = nullptr;
    if (pDst->m_ObjID.m_byID)
    {
      if (pDst->m_ObjID.m_byID == 3)
      {
        battleOwner = *reinterpret_cast<CPlayer **>(&pDst->m_bBaseDownload);
      }
      else if (pDst->m_ObjID.m_byID == 4)
      {
        battleOwner = *reinterpret_cast<CPlayer **>(&pDst->m_byMoveDirect);
      }
    }
    else
    {
      battleOwner = pDst;
    }

    if (!battleOwner)
    {
      return -6;
    }

    if (!battleOwner->m_bInGuildBattle || m_byGuildBattleColorInx == battleOwner->m_byGuildBattleColorInx)
    {
      return -6;
    }
  }
  else
  {
    if (pDst->m_ObjID.m_byID)
    {
      if (pDst->m_ObjID.m_byID == 3)
      {
        CPlayer *master = *reinterpret_cast<CPlayer **>(&pDst->m_bBaseDownload);
        if (master && master->m_bInGuildBattle)
        {
          return -6;
        }
      }
    }
    else
    {
      if (pDst->m_bInGuildBattle)
      {
        return -6;
      }
    }

    if (m_nChaosMode)
    {
      return 0;
    }

    if (pDst->m_ObjID.m_byID)
    {
      if (pDst->m_ObjID.m_byID == 3)
      {
        CPlayer *master = *reinterpret_cast<CPlayer **>(&pDst->m_bBaseDownload);
        if (master && CPlayer::IsPunished(master, 1u, 0))
        {
          return 0;
        }
      }
    }
    else
    {
      if (CPlayer::IsPunished(pDst, 1u, 0))
      {
        return 0;
      }
    }

    const int myRace = GetObjRace();
    const int dstRace = pDst->GetObjRace(pDst);
    if (myRace == dstRace)
    {
      return -6;
    }
  }

  return 0;
}

bool CPlayer::_pre_check_in_guild_battle_race(CPlayer *pDst, bool bEqueal)
{
  if (!pDst)
  {
    return false;
  }

  if (pDst->m_ObjID.m_byID || m_ObjID.m_byID)
  {
    if (pDst->m_ObjID.m_byID || m_ObjID.m_byID != 3)
    {
      if (bEqueal)
      {
        if (IsChaosMode())
        {
          return false;
        }
        if (!pDst->m_ObjID.m_byID && CPlayer::IsPunished(pDst, 1u, 0))
        {
          return false;
        }
        if (pDst->GetObjRace(pDst) == GetObjRace())
        {
          return true;
        }
      }
      else
      {
        if (pDst->GetObjRace(pDst) != GetObjRace())
        {
          return true;
        }
      }
    }
    else
    {
      CPlayer *master = *reinterpret_cast<CPlayer **>(&pDst->m_bBaseDownload);
      if (master)
      {
        if (master->m_bInGuildBattle && m_bInGuildBattle)
        {
          if (bEqueal)
          {
            if (m_byGuildBattleColorInx == master->m_byGuildBattleColorInx)
            {
              return true;
            }
          }
          else if (m_byGuildBattleColorInx != master->m_byGuildBattleColorInx)
          {
            return true;
          }
        }
        else if (!master->m_bInGuildBattle && !m_bInGuildBattle)
        {
          if (bEqueal)
          {
            if (IsChaosMode())
            {
              return false;
            }
            if (!pDst->m_ObjID.m_byID && CPlayer::IsPunished(pDst, 1u, 0))
            {
              return false;
            }
            if (pDst->GetObjRace(pDst) == GetObjRace())
            {
              return true;
            }
          }
          else
          {
            if (pDst->GetObjRace(pDst) != GetObjRace())
            {
              return true;
            }
          }
        }
      }
      else if (bEqueal)
      {
        if (IsChaosMode())
        {
          return false;
        }
        if (!pDst->m_ObjID.m_byID && CPlayer::IsPunished(pDst, 1u, 0))
        {
          return false;
        }
        if (pDst->GetObjRace(pDst) == GetObjRace())
        {
          return true;
        }
      }
      else
      {
        if (pDst->GetObjRace(pDst) != GetObjRace())
        {
          return true;
        }
      }
    }
  }
  else
  {
    if (pDst->m_bInGuildBattle && m_bInGuildBattle)
    {
      if (bEqueal)
      {
        if (m_byGuildBattleColorInx == pDst->m_byGuildBattleColorInx)
        {
          return true;
        }
      }
      else if (m_byGuildBattleColorInx != pDst->m_byGuildBattleColorInx)
      {
        return true;
      }
    }
    else if (!pDst->m_bInGuildBattle && !m_bInGuildBattle)
    {
      if (bEqueal)
      {
        if (IsChaosMode())
        {
          return false;
        }
        if (!pDst->m_ObjID.m_byID && CPlayer::IsPunished(pDst, 1u, 0))
        {
          return false;
        }
        if (pDst->GetObjRace(pDst) == GetObjRace())
        {
          return true;
        }
      }
      else
      {
        if (pDst->GetObjRace(pDst) != GetObjRace())
        {
          return true;
        }
      }
    }
  }

  return false;
}

bool CPlayer::_pre_check_skill_enable(_skill_fld *pSkillFld)
{
  return pSkillFld->m_bEnable != 0;
}

char CPlayer::_pre_check_skill_gradelimit(_skill_fld *pSkillFld)
{
  if (!pSkillFld)
  {
    return 0;
  }

  bool pass = false;
  for (int j = 0; j < 12; ++j)
  {
    if (pSkillFld->m_strGradeLimit[j] == 49)
    {
      switch (j)
      {
        case 0:
          if (pSkillFld->m_nClass != 4 && pSkillFld->m_nClass != 5)
          {
            pass = true;
          }
          break;
        case 1:
        {
          const unsigned int charSerial = CPlayerDB::GetCharSerial(&m_Param);
          const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
          CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
          const unsigned int bossSerial = CPvpUserAndGuildRankingSystem::GetCurrentRaceBossSerial(rank, raceCode, 0);
          if (charSerial == bossSerial)
          {
            pass = true;
          }
          break;
        }
        case 2:
        {
          const unsigned int charSerial = CPlayerDB::GetCharSerial(&m_Param);
          const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
          CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
          if (CPvpUserAndGuildRankingSystem::GetBossType(rank, raceCode, charSerial) == 1)
          {
            pass = true;
          }
          break;
        }
        case 3:
        {
          const unsigned int charSerial = CPlayerDB::GetCharSerial(&m_Param);
          const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
          CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
          if (CPvpUserAndGuildRankingSystem::GetBossType(rank, raceCode, charSerial) == 5)
          {
            pass = true;
          }
          break;
        }
        case 4:
        {
          const unsigned int charSerial = CPlayerDB::GetCharSerial(&m_Param);
          const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
          CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
          if (CPvpUserAndGuildRankingSystem::GetBossType(rank, raceCode, charSerial) == 2)
          {
            pass = true;
          }
          break;
        }
        case 5:
        {
          const unsigned int charSerial = CPlayerDB::GetCharSerial(&m_Param);
          const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
          CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
          if (CPvpUserAndGuildRankingSystem::GetBossType(rank, raceCode, charSerial) == 6)
          {
            pass = true;
          }
          break;
        }
        case 6:
        {
          const unsigned int charSerial = CPlayerDB::GetCharSerial(&m_Param);
          const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
          CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
          if (CPvpUserAndGuildRankingSystem::GetBossType(rank, raceCode, charSerial) == 3)
          {
            pass = true;
          }
          break;
        }
        case 7:
        {
          const unsigned int charSerial = CPlayerDB::GetCharSerial(&m_Param);
          const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
          CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
          if (CPvpUserAndGuildRankingSystem::GetBossType(rank, raceCode, charSerial) == 7)
          {
            pass = true;
          }
          break;
        }
        case 8:
        {
          const unsigned int charSerial = CPlayerDB::GetCharSerial(&m_Param);
          const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
          CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
          if (CPvpUserAndGuildRankingSystem::GetBossType(rank, raceCode, charSerial) == 4)
          {
            pass = true;
          }
          break;
        }
        case 9:
        {
          const unsigned int charSerial = CPlayerDB::GetCharSerial(&m_Param);
          const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
          CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
          if (CPvpUserAndGuildRankingSystem::GetBossType(rank, raceCode, charSerial) == 8)
          {
            pass = true;
          }
          break;
        }
        case 10:
          if (m_Param.m_pGuild)
          {
            const unsigned int charSerial = CPlayerDB::GetCharSerial(&m_Param);
            const unsigned int guildMasterSerial = CGuild::GetGuildMasterSerial(m_Param.m_pGuild);
            if (charSerial == guildMasterSerial)
            {
              pass = true;
            }
          }
          break;
        case 11:
          if (m_pPartyMgr && CPartyPlayer::IsPartyMode(m_pPartyMgr) && CPartyPlayer::IsPartyBoss(m_pPartyMgr))
          {
            pass = true;
          }
          break;
        default:
          break;
      }

      if (pass)
      {
        break;
      }
    }
  }

  return static_cast<char>(pass);
}

void CPlayer::make_gen_attack_param(
  CCharacter *pDst,
  unsigned __int8 byPart,
  _BulletItem_fld *pBulletFld,
  float fAddBulletFc,
  _attack_param *pAP,
  _BulletItem_fld *pEffBtFld,
  float fAddEffBtFc)
{
  pAP->pDst = pDst;
  unsigned __int8 forcedPart = 0xFF;
  if (pDst && !pDst->m_ObjID.m_byID)
  {
    forcedPart = static_cast<unsigned __int8>(pDst[25].m_SFCont[0][5].m_wszPlayerName[16]);
  }
  if (!pDst || pDst->m_ObjID.m_byID || forcedPart == 0xFF)
  {
    pAP->nPart = byPart;
  }
  else
  {
    pAP->nPart = forcedPart;
  }

  if (pBulletFld)
  {
    pAP->nTol = pBulletFld->m_nProperty;
  }
  else
  {
    pAP->nTol = m_pmWpn.byAttTolType;
  }

  pAP->nClass = m_pmWpn.byWpClass;

  int masteryBonus = 1;
  if (m_pmWpn.byWpType == 7)
  {
    masteryBonus = _MASTERY_PARAM::GetMasteryPerMast(&m_pmMst, 6u, 0);
  }
  else
  {
    masteryBonus = _MASTERY_PARAM::GetMasteryPerMast(&m_pmMst, 0, m_pmWpn.byWpClass);
  }

  if (strncmp(m_pmWpn.strEffBulletType, "-1", 2u) && pEffBtFld)
  {
    const float minAf = static_cast<float>(m_pmWpn.nGaMinAF);
    const float effRate = _effect_parameter::GetEff_Rate(&m_EP, 32);
    pAP->nMinAFPlus = static_cast<int>(
      static_cast<float>(((minAf * effRate) * fAddBulletFc) * fAddEffBtFc) + static_cast<float>(CPlayer::s_nAddMstFc[masteryBonus]));

    const float maxAf = static_cast<float>(m_pmWpn.nGaMaxAF);
    const float maxRate = _effect_parameter::GetEff_Rate(&m_EP, 32);
    pAP->nMaxAFPlus = static_cast<int>(
      static_cast<float>(((maxAf * maxRate) * fAddBulletFc) * fAddEffBtFc) + static_cast<float>(CPlayer::s_nAddMstFc[masteryBonus]));
  }

  const float minAf = static_cast<float>(m_pmWpn.nGaMinAF);
  const float minRate = _effect_parameter::GetEff_Rate(&m_EP, 32);
  pAP->nMinAF = static_cast<int>((minAf * minRate) * fAddBulletFc + static_cast<float>(CPlayer::s_nAddMstFc[masteryBonus]));

  const float maxAf = static_cast<float>(m_pmWpn.nGaMaxAF);
  const float maxRate = _effect_parameter::GetEff_Rate(&m_EP, 32);
  pAP->nMaxAF = static_cast<int>((maxAf * maxRate) * fAddBulletFc + static_cast<float>(CPlayer::s_nAddMstFc[masteryBonus]));

  pAP->nMinSel = m_pmWpn.byGaMinSel;
  pAP->nMaxSel = m_pmWpn.byGaMaxSel;
  pAP->nExtentRange = 20;

  if (pBulletFld)
  {
    pAP->nAttactType = pBulletFld->m_nEffectGroup;
    pAP->nShotNum = 1;
  }

  if (pEffBtFld)
  {
    pAP->nEffShotNum = 1;
  }

  if (pDst)
  {
    memcpy_0(pAP->fArea, pDst->m_fCurPos, sizeof(pAP->fArea));
  }

  pAP->bMatchless = m_bCheat_Matchless;
  pAP->nMaxAttackPnt = m_nMaxAttackPnt;
  if (pDst && pDst->m_ObjID.m_byKind == 1 && !CMonster::IsViewArea(reinterpret_cast<CMonster *>(pDst), this))
  {
    pAP->bBackAttack = 1;
  }
}

void CPlayer::make_skill_attack_param(
  CCharacter *pDst,
  float *pfAttackPos,
  unsigned __int8 byEffectCode,
  _skill_fld *pSkillFld,
  int nAttType,
  _STORAGE_LIST::_db_con *pBulletItem,
  float fAddBulletFc,
  _attack_param *pAP,
  _STORAGE_LIST::_db_con *pEffBulletItem,
  float fAddEffBulletFc)
{
  pAP->pDst = pDst;
  const int attackPart = pDst ? static_cast<int>(pDst->GetAttackRandomPart()) : static_cast<int>(GetAttackRandomPart());
  pAP->nPart = attackPart;

  if (pBulletItem)
  {
    _base_fld *bulletRecord = CRecordData::GetRecord(&g_Main.m_tblItemData[10], pBulletItem->m_wItemIndex);
    pAP->nTol = *reinterpret_cast<int *>(&bulletRecord[6].m_strCode[40]);
  }
  else
  {
    pAP->nTol = m_pmWpn.byAttTolType;
  }

  pAP->nClass = m_pmWpn.byWpClass;

  if (m_pmWpn.nGaMaxAF < 0 || _effect_parameter::GetEff_Rate(&m_EP, 32) < 0.0f || fAddBulletFc < 0.0f)
  {
    const float effRate = _effect_parameter::GetEff_Rate(&m_EP, 32);
    CLogFile::Write(
      &g_Main.m_logSystemError,
      "Skill Attack Warning : nGaMaxAF(%d), Potion_Inc_Fc(%f), fAddBulletFc(%f)",
      m_pmWpn.nGaMaxAF,
      effRate,
      fAddBulletFc);
  }

  if (m_pmWpn.byWpType == 7)
  {
    const float minAf = static_cast<float>(m_pmWpn.nGaMinAF);
    const float rate = _effect_parameter::GetEff_Rate(&m_EP, 32);
    const float tempMin = (minAf * rate) * fAddBulletFc;
    const int mastery = _MASTERY_PARAM::GetMasteryPerMast(&m_pmMst, 6u, 0);
    pAP->nMinAF = static_cast<int>(tempMin + static_cast<float>(mastery));

    const float maxAf = static_cast<float>(m_pmWpn.nGaMaxAF);
    const float maxRate = _effect_parameter::GetEff_Rate(&m_EP, 32);
    const float tempMax = (maxAf * maxRate) * fAddBulletFc;
    const int maxMastery = _MASTERY_PARAM::GetMasteryPerMast(&m_pmMst, 6u, 0);
    pAP->nMaxAF = static_cast<int>(tempMax + static_cast<float>(maxMastery));
  }
  else
  {
    const float minAf = static_cast<float>(m_pmWpn.nGaMinAF);
    const float rate = _effect_parameter::GetEff_Rate(&m_EP, 32);
    const float tempMin = (minAf * rate) * fAddBulletFc;
    const int mastery = _MASTERY_PARAM::GetMasteryPerMast(&m_pmMst, 0, m_pmWpn.byWpClass);
    pAP->nMinAF = static_cast<int>(tempMin + static_cast<float>(mastery));

    const float maxAf = static_cast<float>(m_pmWpn.nGaMaxAF);
    const float maxRate = _effect_parameter::GetEff_Rate(&m_EP, 32);
    const float tempMax = (maxAf * maxRate) * fAddBulletFc;
    const int maxMastery = _MASTERY_PARAM::GetMasteryPerMast(&m_pmMst, 0, m_pmWpn.byWpClass);
    pAP->nMaxAF = static_cast<int>(tempMax + static_cast<float>(maxMastery));
  }

  if (strncmp(m_pmWpn.strEffBulletType, "-1", 2u) && pEffBulletItem)
  {
    if (m_pmWpn.byWpType == 7)
    {
      const float minAf = static_cast<float>(m_pmWpn.nGaMinAF);
      const float rate = _effect_parameter::GetEff_Rate(&m_EP, 32);
      const float tempMin = ((minAf * rate) * fAddBulletFc) * fAddEffBulletFc;
      const int mastery = _MASTERY_PARAM::GetMasteryPerMast(&m_pmMst, 6u, 0);
      pAP->nMinAFPlus = static_cast<int>(tempMin + static_cast<float>(mastery));

      const float maxAf = static_cast<float>(m_pmWpn.nGaMaxAF);
      const float maxRate = _effect_parameter::GetEff_Rate(&m_EP, 32);
      const float tempMax = ((maxAf * maxRate) * fAddBulletFc) * fAddEffBulletFc;
      const int maxMastery = _MASTERY_PARAM::GetMasteryPerMast(&m_pmMst, 6u, 0);
      pAP->nMaxAFPlus = static_cast<int>(tempMax + static_cast<float>(maxMastery));
    }
    else
    {
      const float minAf = static_cast<float>(m_pmWpn.nGaMinAF);
      const float rate = _effect_parameter::GetEff_Rate(&m_EP, 32);
      const float tempMin = ((minAf * rate) * fAddBulletFc) * fAddEffBulletFc;
      const int mastery = _MASTERY_PARAM::GetMasteryPerMast(&m_pmMst, 0, m_pmWpn.byWpClass);
      pAP->nMinAFPlus = static_cast<int>(tempMin + static_cast<float>(mastery));

      const float maxAf = static_cast<float>(m_pmWpn.nGaMaxAF);
      const float maxRate = _effect_parameter::GetEff_Rate(&m_EP, 32);
      const float tempMax = ((maxAf * maxRate) * fAddBulletFc) * fAddEffBulletFc;
      const int maxMastery = _MASTERY_PARAM::GetMasteryPerMast(&m_pmMst, 0, m_pmWpn.byWpClass);
      pAP->nMaxAFPlus = static_cast<int>(tempMax + static_cast<float>(maxMastery));
    }
  }

  if (IsSiegeMode())
  {
    _base_fld *siegeRecord = CRecordData::GetRecord(&g_Main.m_tblItemData[27], m_pSiegeItem->m_wItemIndex);
    const float effRate = _effect_parameter::GetEff_Rate(&m_EP, 23);
    pAP->nMinAF = static_cast<int>(static_cast<float>(pAP->nMinAF)
                                   * (*reinterpret_cast<float *>(&siegeRecord[5].m_strCode[24]) * effRate));
    if (strncmp(m_pmWpn.strEffBulletType, "-1", 2u) && pEffBulletItem)
    {
      const float rate = _effect_parameter::GetEff_Rate(&m_EP, 23);
      pAP->nMinAFPlus = static_cast<int>(static_cast<float>(pAP->nMinAFPlus)
                                         * (*reinterpret_cast<float *>(&siegeRecord[5].m_strCode[24]) * rate));
    }

    const float maxRate = _effect_parameter::GetEff_Rate(&m_EP, 23);
    pAP->nMaxAF = static_cast<int>(static_cast<float>(pAP->nMaxAF)
                                   * (*reinterpret_cast<float *>(&siegeRecord[5].m_strCode[24]) * maxRate));
    if (strncmp(m_pmWpn.strEffBulletType, "-1", 2u) && pEffBulletItem)
    {
      const float rate = _effect_parameter::GetEff_Rate(&m_EP, 23);
      pAP->nMaxAFPlus = static_cast<int>(static_cast<float>(pAP->nMaxAFPlus)
                                         * (*reinterpret_cast<float *>(&siegeRecord[5].m_strCode[24]) * rate));
    }
  }

  pAP->nMinSel = m_pmWpn.byGaMinSel;
  pAP->nMaxSel = m_pmWpn.byGaMaxSel;
  pAP->nExtentRange = 20;

  if (pBulletItem)
  {
    pAP->nShotNum = pSkillFld->m_nAttNeedBt;
    if (pBulletItem->m_dwDur < pSkillFld->m_nAttNeedBt)
    {
      pAP->nShotNum = pBulletItem->m_dwDur;
    }
  }

  if (pEffBulletItem)
  {
    pAP->nEffShotNum = pSkillFld->m_nAttNeedBt;
    if (pEffBulletItem->m_dwDur < pSkillFld->m_nAttNeedBt)
    {
      pAP->nEffShotNum = pEffBulletItem->m_dwDur;
    }
  }

  if (nAttType == 3)
  {
    pAP->nAddAttPnt = static_cast<int>(static_cast<float>(GetHP()) * 0.89999998f);
  }

  pAP->pFld = pSkillFld;
  pAP->byEffectCode = byEffectCode;
  if (byEffectCode)
  {
    pAP->nLevel = 1;
    pAP->nMastery = 99;
  }
  else
  {
    pAP->nLevel = _MASTERY_PARAM::GetSkillLv(&m_pmMst, pSkillFld->m_dwIndex);
    pAP->nLevel = static_cast<int>(static_cast<float>(pAP->nLevel) + _effect_parameter::GetEff_Plus(&m_EP, 19));
    if (pAP->nLevel > 7)
    {
      pAP->nLevel = 7;
    }
    pAP->nMastery = _MASTERY_PARAM::GetMasteryPerMast(&m_pmMst, 3u, pSkillFld->m_nMastIndex);
  }

  memcpy_0(pAP->fArea, pfAttackPos, sizeof(pAP->fArea));
  pAP->nMaxAttackPnt = m_nMaxAttackPnt;
  if (pDst && pDst->m_ObjID.m_byKind == 1 && !CMonster::IsViewArea(reinterpret_cast<CMonster *>(pDst), this))
  {
    pAP->bBackAttack = 1;
  }
}

void CPlayer::make_force_attack_param(
  CCharacter *pDst,
  _force_fld *pForceFld,
  _STORAGE_LIST::_db_con *pForceItem,
  float *pTar,
  _attack_param *pAP,
  _STORAGE_LIST::_db_con *pEffBulletItem,
  float fAddEffBtFc)
{
  pAP->pDst = pDst;
  const int attackPart = pDst ? static_cast<int>(pDst->GetAttackRandomPart()) : static_cast<int>(GetAttackRandomPart());
  pAP->nPart = attackPart;
  pAP->nTol = pForceFld->m_nProperty;
  pAP->nClass = m_pmWpn.byWpClass;

  if (strncmp(m_pmWpn.strEffBulletType, "-1", 2u) && pEffBulletItem)
  {
    const float minAf = static_cast<float>(m_pmWpn.nMaMinAF);
    const float rate = _effect_parameter::GetEff_Rate(&m_EP, 32);
    pAP->nMinAFPlus = static_cast<int>(((minAf * rate) * fAddEffBtFc) + static_cast<float>(m_pmMst.m_mtyStaff));

    const float maxAf = static_cast<float>(m_pmWpn.nMaMaxAF);
    const float maxRate = _effect_parameter::GetEff_Rate(&m_EP, 32);
    pAP->nMaxAFPlus = static_cast<int>(((maxAf * maxRate) * fAddEffBtFc) + static_cast<float>(m_pmMst.m_mtyStaff));
  }

  const float minAf = static_cast<float>(m_pmWpn.nMaMinAF);
  const float minRate = _effect_parameter::GetEff_Rate(&m_EP, 32);
  pAP->nMinAF = static_cast<int>((minAf * minRate) + static_cast<float>(m_pmMst.m_mtyStaff));

  const float maxAf = static_cast<float>(m_pmWpn.nMaMaxAF);
  const float maxRate = _effect_parameter::GetEff_Rate(&m_EP, 32);
  pAP->nMaxAF = static_cast<int>((maxAf * maxRate) + static_cast<float>(m_pmMst.m_mtyStaff));

  pAP->nMinSel = m_pmWpn.byMaMinSel;
  pAP->nMaxSel = m_pmWpn.byMaMaxSel;
  pAP->pFld = pForceFld;
  pAP->byEffectCode = 1;
  pAP->nLevel = GetSFLevel(pForceFld->m_nLv, pForceItem->m_dwDur);
  pAP->nMastery = _MASTERY_PARAM::GetMasteryPerMast(&m_pmMst, 4u, pForceFld->m_nMastIndex);

  if (pDst)
  {
    memcpy_0(pAP->fArea, pDst->m_fCurPos, sizeof(pAP->fArea));
  }
  else
  {
    memcpy_0(pAP->fArea, pTar, sizeof(pAP->fArea));
  }

  pAP->nMaxAttackPnt = m_nMaxAttackPnt;
  if (pDst && pDst->m_ObjID.m_byKind == 1 && !CMonster::IsViewArea(reinterpret_cast<CMonster *>(pDst), this))
  {
    pAP->bBackAttack = 1;
  }

  if (pEffBulletItem)
  {
    pAP->nEffShotNum = 1;
  }
}

void CPlayer::make_unit_attack_param(CCharacter *pDst, _UnitPart_fld *pWeaponFld, float fAddBulletFc, _attack_param *pAP)
{
  pAP->pDst = pDst;
  const int attackPart = pDst ? static_cast<int>(pDst->GetAttackRandomPart()) : static_cast<int>(GetAttackRandomPart());
  pAP->nPart = attackPart;
  pAP->nTol = -1;
  pAP->nClass = pWeaponFld->m_nWPType;
  pAP->nMinAF = static_cast<int>(static_cast<float>(pWeaponFld->m_nGAMinAF) * fAddBulletFc * m_fUnitPv_AttFc);

  const float maxAf = static_cast<float>(pWeaponFld->m_nGAMaxAF) * fAddBulletFc;
  const int mastery = _MASTERY_PARAM::GetMasteryPerMast(&m_pmMst, 6u, 0);
  pAP->nMaxAF = static_cast<int>((maxAf + static_cast<float>(mastery)) * m_fUnitPv_AttFc);

  pAP->nMinSel = pWeaponFld->m_nGAMinSelProb;
  pAP->nMaxSel = pWeaponFld->m_nGAMaxSelProb;
  pAP->nExtentRange = static_cast<int>(pWeaponFld->m_fAttackRange);
  pAP->pFld = pWeaponFld;
  pAP->nMaxAttackPnt = m_nMaxAttackPnt;
  if (pDst && pDst->m_ObjID.m_byKind == 1 && !CMonster::IsViewArea(reinterpret_cast<CMonster *>(pDst), this))
  {
    pAP->bBackAttack = 1;
  }
}

void CPlayer::make_siege_attack_param(
  CCharacter *pDst,
  float *pfAttackPos,
  unsigned __int8 byPart,
  _BulletItem_fld *pBulletFld,
  float fAddBulletFc,
  _attack_param *pAP,
  _BulletItem_fld *pEffBulletFld,
  float fAddEffBtFc)
{
  pAP->pDst = pDst;
  unsigned __int8 forcedPart = 0xFF;
  if (pDst && !pDst->m_ObjID.m_byID)
  {
    forcedPart = static_cast<unsigned __int8>(pDst[25].m_SFCont[0][5].m_wszPlayerName[16]);
  }
  if (!pDst || pDst->m_ObjID.m_byID || forcedPart == 0xFF)
  {
    pAP->nPart = byPart;
  }
  else
  {
    pAP->nPart = forcedPart;
  }

  if (pBulletFld)
  {
    pAP->nTol = pBulletFld->m_nProperty;
  }
  else
  {
    pAP->nTol = m_pmWpn.byAttTolType;
  }

  pAP->nClass = m_pmWpn.byWpClass;
  pAP->nWpType = m_pmWpn.byWpType;

  int masteryBonus = 1;
  if (m_pmWpn.byWpType == 7)
  {
    masteryBonus = _MASTERY_PARAM::GetMasteryPerMast(&m_pmMst, 6u, 0);
  }
  else
  {
    masteryBonus = _MASTERY_PARAM::GetMasteryPerMast(&m_pmMst, 0, m_pmWpn.byWpClass);
  }

  _base_fld *siegeRecord = CRecordData::GetRecord(&g_Main.m_tblItemData[27], m_pSiegeItem->m_wItemIndex);

  if (strncmp(m_pmWpn.strEffBulletType, "-1", 2u) && pEffBulletFld)
  {
    const float minAf = static_cast<float>(m_pmWpn.nGaMinAF);
    const float minRate = _effect_parameter::GetEff_Rate(&m_EP, 32);
    const float minValue =
      ((minAf * minRate) * fAddBulletFc) * fAddEffBtFc + static_cast<float>(CPlayer::s_nAddMstFc[masteryBonus]);
    const float minMul = minValue * (*reinterpret_cast<float *>(&siegeRecord[5].m_strCode[24]));
    const float minEffRate = _effect_parameter::GetEff_Rate(&m_EP, 23);
    pAP->nMinAFPlus = static_cast<int>(minMul * minEffRate);

    const float maxAf = static_cast<float>(m_pmWpn.nGaMaxAF);
    const float maxRate = _effect_parameter::GetEff_Rate(&m_EP, 32);
    const float maxValue =
      ((maxAf * maxRate) * fAddBulletFc) * fAddEffBtFc + static_cast<float>(CPlayer::s_nAddMstFc[masteryBonus]);
    const float maxMul = maxValue * (*reinterpret_cast<float *>(&siegeRecord[5].m_strCode[24]));
    const float maxEffRate = _effect_parameter::GetEff_Rate(&m_EP, 23);
    pAP->nMaxAFPlus = static_cast<int>(maxMul * maxEffRate);
    pAP->nEffShotNum = 1;
  }

  const float minAf = static_cast<float>(m_pmWpn.nGaMinAF);
  const float minRate = _effect_parameter::GetEff_Rate(&m_EP, 32);
  const float minValue = (minAf * minRate) * fAddBulletFc + static_cast<float>(CPlayer::s_nAddMstFc[masteryBonus]);
  const float minMul = minValue * (*reinterpret_cast<float *>(&siegeRecord[5].m_strCode[24]));
  const float minEffRate = _effect_parameter::GetEff_Rate(&m_EP, 23);
  pAP->nMinAF = static_cast<int>(minMul * minEffRate);

  const float maxAf = static_cast<float>(m_pmWpn.nGaMaxAF);
  const float maxRate = _effect_parameter::GetEff_Rate(&m_EP, 32);
  const float maxValue = (maxAf * maxRate) * fAddBulletFc + static_cast<float>(CPlayer::s_nAddMstFc[masteryBonus]);
  const float maxMul = maxValue * (*reinterpret_cast<float *>(&siegeRecord[5].m_strCode[24]));
  const float maxEffRate = _effect_parameter::GetEff_Rate(&m_EP, 23);
  pAP->nMaxAF = static_cast<int>(maxMul * maxEffRate);

  pAP->nMinSel = m_pmWpn.byGaMinSel;
  pAP->nMaxSel = m_pmWpn.byGaMaxSel;
  pAP->nExtentRange = 20;

  if (pBulletFld)
  {
    pAP->nAttactType = pBulletFld->m_nEffectGroup;
    pAP->nShotNum = 1;
  }

  memcpy_0(pAP->fArea, pfAttackPos, sizeof(pAP->fArea));
  pAP->nMaxAttackPnt = m_nMaxAttackPnt;
  if (pDst && pDst->m_ObjID.m_byKind == 1 && !CMonster::IsViewArea(reinterpret_cast<CMonster *>(pDst), this))
  {
    pAP->bBackAttack = 1;
  }
}

void CPlayer::make_wpactive_skill_attack_param(
  CCharacter *pDst,
  _skill_fld *pSkillFld,
  float *pfAttackPos,
  unsigned __int8 byEffectCode,
  int nAttType,
  _STORAGE_LIST::_db_con *pBulletItem,
  float fAddBulletFc,
  _attack_param *pAP,
  int *nShotNum)
{
  pAP->pDst = pDst;
  const int attackPart = pDst ? static_cast<int>(pDst->GetAttackRandomPart()) : static_cast<int>(GetAttackRandomPart());
  pAP->nPart = attackPart;

  if (pBulletItem)
  {
    _base_fld *bulletRecord = CRecordData::GetRecord(&g_Main.m_tblItemData[10], pBulletItem->m_wItemIndex);
    pAP->nTol = *reinterpret_cast<int *>(&bulletRecord[6].m_strCode[40]);
  }
  else
  {
    pAP->nTol = m_pmWpn.byAttTolType;
  }

  pAP->nClass = m_pmWpn.byWpClass;
  if (m_pmWpn.byWpType == 7)
  {
    const float minAf = static_cast<float>(m_pmWpn.nGaMinAF);
    const float minRate = _effect_parameter::GetEff_Rate(&m_EP, 32);
    const float tempMin = (minAf * minRate) * fAddBulletFc;
    const int mastery = _MASTERY_PARAM::GetMasteryPerMast(&m_pmMst, 6u, 0);
    pAP->nMinAF = static_cast<int>(tempMin + static_cast<float>(mastery));

    const float maxAf = static_cast<float>(m_pmWpn.nGaMaxAF);
    const float maxRate = _effect_parameter::GetEff_Rate(&m_EP, 32);
    const float tempMax = (maxAf * maxRate) * fAddBulletFc;
    const int maxMastery = _MASTERY_PARAM::GetMasteryPerMast(&m_pmMst, 6u, 0);
    pAP->nMaxAF = static_cast<int>(tempMax + static_cast<float>(maxMastery));
  }
  else
  {
    const float minAf = static_cast<float>(m_pmWpn.nGaMinAF);
    const float minRate = _effect_parameter::GetEff_Rate(&m_EP, 32);
    const float tempMin = (minAf * minRate) * fAddBulletFc;
    const int mastery = _MASTERY_PARAM::GetMasteryPerMast(&m_pmMst, 0, m_pmWpn.byWpClass);
    pAP->nMinAF = static_cast<int>(tempMin + static_cast<float>(mastery));

    const float maxAf = static_cast<float>(m_pmWpn.nGaMaxAF);
    const float maxRate = _effect_parameter::GetEff_Rate(&m_EP, 32);
    const float tempMax = (maxAf * maxRate) * fAddBulletFc;
    const int maxMastery = _MASTERY_PARAM::GetMasteryPerMast(&m_pmMst, 0, m_pmWpn.byWpClass);
    pAP->nMaxAF = static_cast<int>(tempMax + static_cast<float>(maxMastery));
  }

  pAP->nMinSel = m_pmWpn.byGaMinSel;
  pAP->nMaxSel = m_pmWpn.byGaMaxSel;
  pAP->nExtentRange = 20;

  if (pBulletItem)
  {
    pAP->nShotNum = pSkillFld->m_nAttNeedBt;
    if (pBulletItem->m_dwDur < pSkillFld->m_nAttNeedBt)
    {
      pAP->nShotNum = pBulletItem->m_dwDur;
    }
    *nShotNum += pAP->nShotNum;
  }

  if (nAttType == 3)
  {
    pAP->nAddAttPnt = static_cast<int>(static_cast<float>(GetHP()) * 0.89999998f);
  }

  pAP->pFld = pSkillFld;
  pAP->byEffectCode = byEffectCode;
  if (byEffectCode)
  {
    pAP->nLevel = 1;
    pAP->nMastery = 99;
  }
  else
  {
    pAP->nLevel = m_pmWpn.nActiveEffLvl;
    if (pAP->nLevel > 7)
    {
      pAP->nLevel = 7;
    }
    pAP->nMastery = _MASTERY_PARAM::GetMasteryPerMast(&m_pmMst, 3u, pSkillFld->m_nMastIndex);
  }

  memcpy_0(pAP->fArea, pfAttackPos, sizeof(pAP->fArea));
  pAP->nMaxAttackPnt = m_nMaxAttackPnt;
  if (pDst && pDst->m_ObjID.m_byKind == 1 && !CMonster::IsViewArea(reinterpret_cast<CMonster *>(pDst), this))
  {
    pAP->bBackAttack = 1;
  }
}

void CPlayer::make_wpactive_force_attack_param(
  CCharacter *pDst,
  _force_fld *pForceFld,
  float *pfAttackPos,
  _attack_param *pAP)
{
  pAP->pDst = pDst;
  const int attackPart = pDst ? static_cast<int>(pDst->GetAttackRandomPart()) : static_cast<int>(GetAttackRandomPart());
  pAP->nPart = attackPart;
  pAP->nTol = pForceFld->m_nProperty;

  const float minAf = static_cast<float>(m_pmWpn.nMaMinAF);
  const float minRate = _effect_parameter::GetEff_Rate(&m_EP, 32);
  pAP->nMinAF = static_cast<int>((minAf * minRate) + static_cast<float>(m_pmMst.m_mtyStaff));

  const float maxAf = static_cast<float>(m_pmWpn.nMaMaxAF);
  const float maxRate = _effect_parameter::GetEff_Rate(&m_EP, 32);
  pAP->nMaxAF = static_cast<int>((maxAf * maxRate) + static_cast<float>(m_pmMst.m_mtyStaff));

  pAP->nMinSel = m_pmWpn.byMaMinSel;
  pAP->nMaxSel = m_pmWpn.byMaMaxSel;
  pAP->pFld = pForceFld;
  pAP->byEffectCode = 1;
  pAP->nLevel = m_pmWpn.nActiveEffLvl;
  pAP->nMastery = _MASTERY_PARAM::GetMasteryPerMast(&m_pmMst, 4u, pForceFld->m_nMastIndex);
  memcpy_0(pAP->fArea, pfAttackPos, sizeof(pAP->fArea));
  pAP->nMaxAttackPnt = m_nMaxAttackPnt;
  if (pDst && pDst->m_ObjID.m_byKind == 1 && !CMonster::IsViewArea(reinterpret_cast<CMonster *>(pDst), this))
  {
    pAP->bBackAttack = 1;
  }
}

char CPlayer::WeaponSFActive(
  _be_damaged_char *pDamList,
  int *nDamagedObjNum,
  int *nShotNum,
  unsigned __int16 wBulletSerial)
{
  if (!pDamList)
  {
    return 0;
  }

  char result = 0;
  const int effectCode = m_pmWpn.nActiveType;
  if (effectCode)
  {
    if (effectCode == 1)
    {
      _force_fld *forceField = reinterpret_cast<_force_fld *>(
        CRecordData::GetRecordByHash(&g_Main.m_tblEffectData[effectCode], m_pmWpn.strActiveCode_key, 0, 4));
      if (forceField)
      {
        if (forceField->m_bAttackable)
        {
          return pc_WPActiveAttack_Force(pDamList, nDamagedObjNum, forceField);
        }
        return WPActiveForce(pDamList, *nDamagedObjNum, forceField);
      }
    }
    else if (effectCode == 2)
    {
      _skill_fld *skillField = reinterpret_cast<_skill_fld *>(
        CRecordData::GetRecordByHash(&g_Main.m_tblEffectData[effectCode], m_pmWpn.strActiveCode_key, 0, 5));
      if (skillField)
      {
        if (skillField->m_nAttackable <= 0)
        {
          return WPActiveSkill(pDamList, *nDamagedObjNum, skillField, effectCode);
        }
        return pc_WPActiveAttack_Skill(pDamList, nDamagedObjNum, nShotNum, skillField, effectCode, wBulletSerial);
      }
    }
  }
  else
  {
    _skill_fld *skillField = reinterpret_cast<_skill_fld *>(
      CRecordData::GetRecordByHash(g_Main.m_tblEffectData, m_pmWpn.strActiveCode_key, 0, 5));
    if (skillField)
    {
      if (skillField->m_nAttackable <= 0)
      {
        return WPActiveSkill(pDamList, *nDamagedObjNum, skillField, effectCode);
      }
      return pc_WPActiveAttack_Skill(pDamList, nDamagedObjNum, nShotNum, skillField, effectCode, wBulletSerial);
    }
  }

  return result;
}

char CPlayer::pc_WPActiveAttack_Skill(
  _be_damaged_char *pDamList,
  int *nDamagedObjNum,
  int *nShotNum,
  _skill_fld *pSkillFld,
  unsigned __int8 byEffectCode,
  unsigned __int16 wBulletSerial)
{
  _STORAGE_LIST::_db_con *bulletItem = nullptr;
  _BulletItem_fld *bulletField = nullptr;
  int attackType = 0;
  float attackPos[3]{};

  memcpy_0(attackPos, pDamList->m_pChar->m_fCurPos, sizeof(attackPos));
  if (byEffectCode)
  {
    attackType = pSkillFld->m_nAttType[0];
  }
  else
  {
    attackType = *reinterpret_cast<int *>(&pSkillFld->m_strEffectCode[4 * m_pmWpn.nActiveEffLvl + 64]);
  }

  if (!_pre_check_wpactive_skill_attack(byEffectCode, pSkillFld, wBulletSerial, &bulletItem, &bulletField))
  {
    return 0;
  }

  float addBulletFc = FLOAT_1_0;
  if (bulletItem)
  {
    addBulletFc = bulletField->m_fGAAF;
  }

  _attack_param param{};
  make_wpactive_skill_attack_param(
    pDamList->m_pChar,
    pSkillFld,
    attackPos,
    byEffectCode,
    attackType,
    bulletItem,
    addBulletFc,
    &param,
    nShotNum);

  CPlayerAttack attack(this);
  attack.WPActiveAttackSkill(&param);

  if (attack.m_nDamagedObjNum > 0)
  {
    for (int j = 0; j < attack.m_nDamagedObjNum; ++j)
    {
      bool found = false;
      for (int k = 0; k < *nDamagedObjNum; ++k)
      {
        if (pDamList[k].m_pChar == attack.m_DamList[j].m_pChar)
        {
          found = true;
          pDamList[k].m_bActiveSucc = attack.m_DamList[j].m_nDamage > 0;
          pDamList[k].m_nActiveDamage = attack.m_DamList[j].m_nDamage;
          break;
        }
      }

      if (!found && *nDamagedObjNum < 30)
      {
        pDamList[*nDamagedObjNum].m_pChar = attack.m_DamList[j].m_pChar;
        pDamList[*nDamagedObjNum].m_nDamage = 0;
        pDamList[*nDamagedObjNum].m_bActiveSucc = attack.m_DamList[j].m_nDamage > 0;
        pDamList[(*nDamagedObjNum)++].m_nActiveDamage = attack.m_DamList[j].m_nDamage;
      }
    }
  }

  return 1;
}

char CPlayer::pc_WPActiveAttack_Force(_be_damaged_char *pDamList, int *nDamagedObjNum, _force_fld *pForceFld)
{
  float attackPos[3]{};
  memcpy_0(attackPos, pDamList->m_pChar->m_fCurPos, sizeof(attackPos));

  if (!_pre_check_wpactive_force_attack())
  {
    return 0;
  }

  _attack_param param{};
  make_wpactive_force_attack_param(pDamList->m_pChar, pForceFld, attackPos, &param);

  CPlayerAttack attack(this);
  attack.WPActiveAttackForce(&param);

  if (attack.m_nDamagedObjNum > 0)
  {
    for (int j = 0; j < attack.m_nDamagedObjNum; ++j)
    {
      bool found = false;
      for (int k = 0; k < *nDamagedObjNum; ++k)
      {
        if (pDamList[k].m_pChar == attack.m_DamList[j].m_pChar)
        {
          found = true;
          pDamList[k].m_bActiveSucc = attack.m_DamList[j].m_nDamage > 0;
          pDamList[k].m_nActiveDamage = attack.m_DamList[j].m_nDamage;
          break;
        }
      }

      if (!found && *nDamagedObjNum < 30)
      {
        pDamList[*nDamagedObjNum].m_pChar = attack.m_DamList[j].m_pChar;
        pDamList[*nDamagedObjNum].m_nDamage = 0;
        pDamList[*nDamagedObjNum].m_bActiveSucc = attack.m_DamList[j].m_nDamage > 0;
        pDamList[(*nDamagedObjNum)++].m_nActiveDamage = attack.m_DamList[j].m_nDamage;
      }
    }
  }

  return 1;
}

bool CPlayer::WPActiveSkill(
  _be_damaged_char *pDamList,
  int nDamagedObjNum,
  _skill_fld *pSkillFld,
  int nEffectCode)
{
  CCharacter *target = pDamList->m_pChar;
  if (!target || !pSkillFld)
  {
    return false;
  }

  if (!nEffectCode && pSkillFld->m_nMastIndex > 8u)
  {
    return false;
  }

  if (pSkillFld->m_nTempEffectType >= 150)
  {
    return false;
  }

  if (pSkillFld->m_nTempEffectType == -1 && pSkillFld->m_nContEffectType == -1)
  {
    return false;
  }

  const bool usableTemp = IsUsableTempEffectAtStoneState(pSkillFld->m_nTempEffectType);
  bool success = false;
  int skillLevel = nEffectCode ? 1 : m_pmWpn.nActiveEffLvl;
  if (skillLevel > 7)
  {
    skillLevel = 7;
  }

  if (nDamagedObjNum <= 1)
  {
    if (!CCharacter::IsEffectableDst(this, pSkillFld->m_strActableDst, target))
    {
      return false;
    }

    if (pSkillFld->m_nContEffectType != -1 && !target->IsRecvableContEffect(target))
    {
      return false;
    }

    if (!pSkillFld->m_nContEffectType && !IsAttackableInTown() && !target->IsAttackableInTown(target))
    {
      if (IsInTown())
      {
        return false;
      }

      if (target->IsInTown(target))
      {
        return false;
      }

      if (m_Param.m_pGuild)
      {
        CGuildRoomSystem *roomSystem = CGuildRoomSystem::GetInstance();
        if (CGuildRoomSystem::IsGuildRoomMemberIn(
              roomSystem,
              m_Param.m_pGuild->m_dwSerial,
              m_ObjID.m_wIndex,
              m_pUserDB->m_dwSerial))
        {
          return false;
        }
      }
    }

    unsigned __int8 errorCode[32]{};
    bool upMastery = false;
    success = CCharacter::AssistSkill(this, target, nEffectCode, pSkillFld, skillLevel, errorCode, &upMastery);
    if (!errorCode[0])
    {
      pDamList->m_bActiveSucc = 1;
    }
  }
  else
  {
    for (int j = 0; j < nDamagedObjNum; ++j)
    {
      if (CCharacter::IsEffectableDst(this, pSkillFld->m_strActableDst, target)
          && (pSkillFld->m_nContEffectType == -1 || target->IsRecvableContEffect(target))
          && (!_effect_parameter::GetEff_State(&target->m_EP, 20)
              || (pSkillFld->m_nTempEffectType != -1 && usableTemp))
          && !_effect_parameter::GetEff_State(&target->m_EP, 28))
      {
        if (pSkillFld->m_nContEffectType || IsAttackableInTown() || target->IsAttackableInTown(target)
            || (!IsInTown() && !target->IsInTown(target)
                && (!m_Param.m_pGuild
                    || (CGuildRoomSystem::GetInstance(),
                        !CGuildRoomSystem::IsGuildRoomMemberIn(
                          CGuildRoomSystem::GetInstance(),
                          m_Param.m_pGuild->m_dwSerial,
                          m_ObjID.m_wIndex,
                          m_pUserDB->m_dwSerial)))))
        {
          if (CCharacter::AssistSkillToOne(this, pDamList[j].m_pChar, nEffectCode, pSkillFld, skillLevel))
          {
            pDamList[j].m_bActiveSucc = 1;
            success = true;
          }
        }
      }
    }
  }

  return success;
}

bool CPlayer::WPActiveForce(_be_damaged_char *pDamList, int nDamagedObjNum, _force_fld *pForceFld)
{
  CCharacter *target = pDamList->m_pChar;
  if (!target || !pForceFld)
  {
    return false;
  }

  if (pForceFld->m_nTempEffectType >= 150)
  {
    return false;
  }

  if (pForceFld->m_nTempEffectType == -1 && pForceFld->m_nContEffectType == -1)
  {
    return false;
  }

  int forceLevel = m_pmWpn.nActiveEffLvl;
  const bool usableTemp = IsUsableTempEffectAtStoneState(pForceFld->m_nTempEffectType);
  bool success = false;
  if (forceLevel > 7)
  {
    forceLevel = 7;
  }

  if (nDamagedObjNum <= 1)
  {
    if (!CCharacter::IsEffectableDst(this, pForceFld->m_strActableDst, target))
    {
      return false;
    }

    if (pForceFld->m_nContEffectType != -1 && !target->IsRecvableContEffect(target))
    {
      return false;
    }

    if (!pForceFld->m_nContEffectType && !IsAttackableInTown() && !target->IsAttackableInTown(target))
    {
      if (IsInTown())
      {
        return false;
      }

      if (target->IsInTown(target))
      {
        return false;
      }

      if (m_Param.m_pGuild)
      {
        CGuildRoomSystem *roomSystem = CGuildRoomSystem::GetInstance();
        if (CGuildRoomSystem::IsGuildRoomMemberIn(
              roomSystem,
              m_Param.m_pGuild->m_dwSerial,
              m_ObjID.m_wIndex,
              m_pUserDB->m_dwSerial))
        {
          return false;
        }
      }
    }

    unsigned __int8 errorCode[32]{};
    bool upMastery = false;
    success = CCharacter::AssistForce(this, target, pForceFld, forceLevel, errorCode, &upMastery);
    if (!errorCode[0])
    {
      pDamList->m_bActiveSucc = 1;
    }
  }
  else
  {
    for (int j = 0; j < nDamagedObjNum; ++j)
    {
      if (CCharacter::IsEffectableDst(this, pForceFld->m_strActableDst, pDamList[j].m_pChar)
          && (pForceFld->m_nContEffectType == -1 || pDamList[j].m_pChar->IsRecvableContEffect(pDamList[j].m_pChar))
          && (!_effect_parameter::GetEff_State(&pDamList[j].m_pChar->m_EP, 20)
              || (pForceFld->m_nTempEffectType != -1 && usableTemp))
          && !_effect_parameter::GetEff_State(&pDamList[j].m_pChar->m_EP, 28))
      {
        if (pForceFld->m_nContEffectType || IsAttackableInTown() || target->IsAttackableInTown(target)
            || (!IsInTown() && !target->IsInTown(target)
                && (!m_Param.m_pGuild
                    || (CGuildRoomSystem::GetInstance(),
                        !CGuildRoomSystem::IsGuildRoomMemberIn(
                          CGuildRoomSystem::GetInstance(),
                          m_Param.m_pGuild->m_dwSerial,
                          m_ObjID.m_wIndex,
                          m_pUserDB->m_dwSerial)))))
        {
          if (CCharacter::AssistForceToOne(this, pDamList[j].m_pChar, pForceFld, forceLevel))
          {
            pDamList[j].m_bActiveSucc = 1;
            success = true;
          }
        }
      }
    }
  }

  return success;
}

void CPlayer::pc_PlayAttack_Gen(
  CCharacter *pDst,
  unsigned __int8 byAttPart,
  unsigned __int16 wBulletSerial,
  unsigned __int16 wEffBtSerial,
  bool bCount)
{
  _STORAGE_LIST::_db_con *pItem = nullptr;
  _BulletItem_fld *pBulletFld = nullptr;
  _STORAGE_LIST::_db_con *pEffBulletItem = nullptr;
  _BulletItem_fld *pEffBulletFld = nullptr;
  float fAddEffBtFc = FLOAT_1_0;

  if (!bCount || !m_pmWpn.byWpClass)
  {
    const char errCode = _pre_check_normal_attack(
      pDst,
      wBulletSerial,
      bCount,
      &pItem,
      &pBulletFld,
      wEffBtSerial,
      &pEffBulletItem,
      &pEffBulletFld);
    if (errCode)
    {
      SendMsg_AttackResult_Error(errCode);
      if (m_bMove)
      {
        CCharacter::Stop(this);
        CGameObject::SendMsg_BreakStop(this);
      }
      return;
    }

    float fAddBulletFc = FLOAT_1_0;
    unsigned __int16 bulletIndex = static_cast<unsigned __int16>(-1);
    if (pItem)
    {
      fAddBulletFc = pBulletFld->m_fGAAF;
      bulletIndex = pItem->m_wItemIndex;
    }

    CAttack pAt(this);
    _attack_param pAP{};

    if (pEffBulletItem && pEffBulletFld)
    {
      fAddEffBtFc = pEffBulletFld->m_fGAAF;
    }

    make_gen_attack_param(pDst, byAttPart, pBulletFld, fAddBulletFc, &pAP, pEffBulletFld, fAddEffBtFc);
    if (pEffBulletItem && pEffBulletFld)
    {
      pAt.AttackGen(&pAP, false, true);
    }
    else
    {
      pAt.AttackGen(&pAP, false, false);
    }

    bool bActiveSucc = false;
    if (pAt.m_DamList[0].m_nDamage > 0 && m_pmWpn.nActiveType > -1 && rand() % 100 < m_pmWpn.nActiveProb)
    {
      bActiveSucc = WeaponSFActive(pAt.m_DamList, &pAt.m_nDamagedObjNum, &pAP.nShotNum, wBulletSerial);
    }
    pAt.SetActiveSucc(bActiveSucc);

    if (!bCount)
    {
      int effPlus = 0;
      if (m_pmWpn.byWpType != 7)
      {
        effPlus = static_cast<int>(_effect_parameter::GetEff_Plus(&m_EP, m_pmWpn.byWpClass + 9));
      }

      const int addDelay = static_cast<int>(CalcEquipAttackDelay());
      const int level = static_cast<int>(GetLevel());
      const unsigned int attackDelay = m_pmWpn.GetAttackDelay(level, addDelay);
      int delay = effPlus + static_cast<int>(attackDelay);
      if (m_pmWpn.byWpType == 7)
      {
        const float effPlusInner = _effect_parameter::GetEff_Plus(&m_EP, 11);
        delay = static_cast<int>(static_cast<float>(delay) + effPlusInner);
      }
      _ATTACK_DELAY_CHECKER::SetDelay(&m_AttDelayChker, static_cast<unsigned int>(delay));
    }

    if (_effect_parameter::GetEff_State(&m_EP, 14))
    {
      CCharacter::RemoveSFContHelpByEffect(this, 2, 14);
    }
    if (_effect_parameter::GetEff_State(&m_EP, 21))
    {
      if (pAP.nAttactType != 4 && pAP.nAttactType != 5 && pAP.nAttactType != 6 && pAP.nAttactType != 7)
      {
        return;
      }
      CCharacter::RemoveSFContHelpByEffect(this, 2, 21);
    }

    if (m_bFreeSFByClass)
    {
      pAt.m_bIsCrtAtt = true;
    }

    CPartyModeKillMonsterExpNotify kPartyExpNotify;
    if (!pAt.m_bFailure)
    {
      const int nTotalDam = static_cast<int>(_check_exp_after_attack(pAt.m_nDamagedObjNum, pAt.m_DamList, &kPartyExpNotify));
      if (nTotalDam > 0)
      {
        _check_dst_param_after_attack(nTotalDam, pDst);
      }
    }

    if (bCount)
    {
      SendMsg_AttackResult_Count(&pAt);
    }
    else
    {
      SendMsg_AttackResult_Gen(&pAt, bulletIndex);
    }
    kPartyExpNotify.Notify();

    int masterySum = 0;
    int masteryCount = 0;
    for (int j = 0; j < pAt.m_nDamagedObjNum; ++j)
    {
      const unsigned int totalDamage =
        static_cast<unsigned int>(pAt.m_DamList[j].m_nActiveDamage + pAt.m_DamList[j].m_nDamage);
      const int level = static_cast<int>(CPlayerDB::GetLevel(&m_Param));
      CCharacter *damaged = pAt.m_DamList[j].m_pChar;
      damaged->SetDamage(static_cast<int>(totalDamage), this, level, pAt.m_bIsCrtAtt, -1, 0, true);

      if (IsChaosMode())
      {
        if (GetObjRace() == damaged->GetObjRace(damaged))
        {
          continue;
        }
      }

      if (GetObjRace() == damaged->GetObjRace(damaged) && !damaged->m_ObjID.m_byID)
      {
        CPlayer *damagedPlayer = static_cast<CPlayer *>(damaged);
        if (CPlayer::IsPunished(damagedPlayer, 1u, 0))
        {
          continue;
        }
      }

      if (!pAt.m_bFailure)
      {
        const int dstLevel = static_cast<int>(damaged->GetLevel(damaged));
        if (IsPassMasteryLimitLvDiff(dstLevel))
        {
          const float damageRatio =
            static_cast<float>(pAt.m_DamList[j].m_nDamage)
            / static_cast<float>(damaged->GetMaxHP(damaged));
          if (damageRatio >= 0.0099999998f)
          {
            const int level = static_cast<int>(damaged->GetLevel(damaged));
            masterySum += static_cast<int>(GetMasteryCumAfterAttack(level));
            ++masteryCount;
          }
        }
      }
    }

    if (masteryCount > 0 && !IsInTown())
    {
      const unsigned int dwAlter = static_cast<unsigned int>(masterySum / masteryCount);
      if (dwAlter > 0)
      {
        if (m_pmWpn.byWpType == 7)
        {
          Emb_AlterStat(6u, 0, dwAlter, 0, "CPlayer::pc_PlayAttack_Gen()---0", 1);
        }
        else
        {
          Emb_AlterStat(0, m_pmWpn.byWpClass, dwAlter, 0, "CPlayer::pc_PlayAttack_Gen()---1", 1);
        }
      }
    }

    if (pDst && m_pRecalledAnimusChar && pDst->GetHP(pDst) > 0)
    {
      CAnimus::MasterAttack_MasterInform(m_pRecalledAnimusChar, pDst);
    }
    if (pDst && pDst->GetHP(pDst) > 0)
    {
      _TOWER_PARAM::NotifyOwnerAttackInform(&m_pmTwr, pDst);
    }

    if (pItem)
    {
      const unsigned __int16 dur = Emb_AlterDurPoint(2u, pItem->m_byStorageIndex, -pAP.nShotNum, 0, 1);
      if (dur)
      {
        SendMsg_AlterWeaponBulletInform(pItem->m_wSerial, dur);
      }
      else
      {
        CMgrAvatorItemHistory::consume_del_item(
          &CPlayer::s_MgrItemHistory,
          m_ObjID.m_wIndex,
          pItem,
          m_szItemHistoryFileName);
      }
    }

    SetBattleMode(true);

    if (pEffBulletItem && pAP.nEffShotNum > 0)
    {
      const unsigned __int16 dur = Emb_AlterDurPoint(2u, pEffBulletItem->m_byStorageIndex, -pAP.nEffShotNum, 0, 1);
      if (dur)
      {
        SendMsg_AlterWeaponBulletInform(pEffBulletItem->m_wSerial, dur);
      }
      else
      {
        CMgrAvatorItemHistory::consume_del_item(
          &CPlayer::s_MgrItemHistory,
          m_ObjID.m_wIndex,
          pEffBulletItem,
          m_szItemHistoryFileName);
      }
    }
  }
}

void CPlayer::pc_PlayAttack_Skill(
  CCharacter *pDst,
  float *pfAttackPos,
  unsigned __int8 byEffectCode,
  unsigned __int16 wSkillIndex,
  unsigned __int16 wBulletSerial,
  unsigned __int16 *pConsumeSerial,
  unsigned __int16 wEffBtSerial)
{
  _skill_fld *pSkillFld = nullptr;
  _STORAGE_LIST::_db_con *pBulletItem = nullptr;
  _BulletItem_fld *pBulletFld = nullptr;
  _STORAGE_LIST::_db_con *pEffBulletItem = nullptr;
  _BulletItem_fld *pEffBulletFld = nullptr;

  pSkillFld = reinterpret_cast<_skill_fld *>(CRecordData::GetRecord(&g_Main.m_tblEffectData[byEffectCode], wSkillIndex));
  int nAttType = 0;
  if (byEffectCode)
  {
    nAttType = pSkillFld->m_nAttType[0];
  }
  else
  {
    const int skillLv = _MASTERY_PARAM::GetSkillLv(&m_pmMst, pSkillFld->m_dwIndex);
    nAttType = *reinterpret_cast<int *>(&pSkillFld->m_strEffectCode[4 * skillLv + 64]);
  }

  CCharacter *pTarget = pDst;
  unsigned __int16 decPoint[3]{};
  if (nAttType == 4)
  {
    pTarget = nullptr;
    memcpy_0(pfAttackPos, m_fCurPos, sizeof(float) * 3);
  }
  if (pTarget)
  {
    memcpy_0(pfAttackPos, pTarget->m_fCurPos, sizeof(float) * 3);
  }

  char errCode = _pre_check_skill_attack(
    pTarget,
    pfAttackPos,
    byEffectCode,
    pSkillFld,
    wBulletSerial,
    &pBulletItem,
    &pBulletFld,
    nAttType,
    decPoint,
    wEffBtSerial,
    &pEffBulletItem,
    &pEffBulletFld);

  _STORAGE_LIST::_db_con *consumeItems[3]{};
  int consumeCounts[3]{};
  bool consumeOverlap[3]{};
  if (!errCode && !GetUseConsumeItem(pSkillFld->m_ConsumeItemList, pConsumeSerial, consumeItems, consumeCounts, consumeOverlap))
  {
    errCode = -61;
  }

  if (errCode)
  {
    SendMsg_AttackResult_Error(errCode);
    if (m_bMove)
    {
      CCharacter::Stop(this);
      CGameObject::SendMsg_BreakStop(this);
    }
    return;
  }

  float fAddBulletFc = FLOAT_1_0;
  unsigned __int16 bulletIndex = static_cast<unsigned __int16>(-1);
  if (pBulletItem)
  {
    fAddBulletFc = pBulletFld->m_fGAAF;
    bulletIndex = pBulletItem->m_wItemIndex;
  }

  CPlayerAttack pAt(this);
  _attack_param pAP{};

  float fAddEffBulletFc = FLOAT_1_0;
  if (pEffBulletItem)
  {
    fAddEffBulletFc = pEffBulletFld->m_fGAAF;
  }

  make_skill_attack_param(
    pTarget,
    pfAttackPos,
    byEffectCode,
    pSkillFld,
    nAttType,
    pBulletItem,
    fAddBulletFc,
    &pAP,
    pEffBulletItem,
    fAddEffBulletFc);

  if (wEffBtSerial == 0xFFFF || !pEffBulletItem)
  {
    pAt.AttackSkill(&pAP, false);
  }
  else
  {
    pAt.AttackSkill(&pAP, true);
  }

  bool bActiveSucc = false;
  if (pAt.m_DamList[0].m_nDamage > 0 && m_pmWpn.nActiveType > -1 && rand() % 100 < m_pmWpn.nActiveProb)
  {
    bActiveSucc = WeaponSFActive(pAt.m_DamList, &pAt.m_nDamagedObjNum, &pAP.nShotNum, wBulletSerial);
  }
  pAt.SetActiveSucc(bActiveSucc);

  const float effPlus = _effect_parameter::GetEff_Plus(&m_EP, 12);
  _ATTACK_DELAY_CHECKER::SetDelay(&m_AttDelayChker, static_cast<unsigned int>(pSkillFld->m_fActDelay + effPlus));

  if (_effect_parameter::GetEff_State(&m_EP, 14))
  {
    CCharacter::RemoveSFContHelpByEffect(this, 2, 14);
  }
  if (_effect_parameter::GetEff_State(&m_EP, 21))
  {
    if (pAP.nAttactType != 4 && pAP.nAttactType != 5 && pAP.nAttactType != 6 && pAP.nAttactType != 7)
    {
      return;
    }
    CCharacter::RemoveSFContHelpByEffect(this, 2, 21);
  }

  for (int nParamCode = 0; nParamCode < 3; ++nParamCode)
  {
    if (decPoint[nParamCode])
    {
      const int gauge = GetGauge(nParamCode);
      int newValue = 0;
      if (gauge - decPoint[nParamCode] > 0)
      {
        newValue = gauge - decPoint[nParamCode];
      }
      SetGauge(nParamCode, newValue, true);
    }
    SendMsg_Recover();
  }

  CPartyModeKillMonsterExpNotify kPartyExpNotify;
  if (!pAt.m_bFailure)
  {
    const int nTotalDam = static_cast<int>(_check_exp_after_attack(pAt.m_nDamagedObjNum, pAt.m_DamList, &kPartyExpNotify));
    _check_dst_param_after_attack(nTotalDam, pTarget);
  }

  if (pAP.nShotNum > 0)
  {
    const unsigned __int16 dur = Emb_AlterDurPoint(2u, pBulletItem->m_byStorageIndex, -pAP.nShotNum, 0, 1);
    if (dur)
    {
      SendMsg_AlterWeaponBulletInform(pBulletItem->m_wSerial, dur);
    }
    else
    {
      CMgrAvatorItemHistory::consume_del_item(
        &CPlayer::s_MgrItemHistory,
        m_ObjID.m_wIndex,
        pBulletItem,
        m_szItemHistoryFileName);
    }
  }

  if (pEffBulletItem && pAP.nEffShotNum > 0)
  {
    const unsigned __int16 dur = Emb_AlterDurPoint(2u, pEffBulletItem->m_byStorageIndex, -pAP.nEffShotNum, 0, 1);
    if (dur)
    {
      SendMsg_AlterWeaponBulletInform(pEffBulletItem->m_wSerial, dur);
    }
    else
    {
      CMgrAvatorItemHistory::consume_del_item(
        &CPlayer::s_MgrItemHistory,
        m_ObjID.m_wIndex,
        pEffBulletItem,
        m_szItemHistoryFileName);
    }
  }

  DeleteUseConsumeItem(consumeItems, consumeCounts, consumeOverlap);
  SendMsg_AttackResult_Skill(byEffectCode, &pAt, bulletIndex);
  kPartyExpNotify.Notify();

  if (pSkillFld->m_nAttackable == 2)
  {
    pAt.m_bIsCrtAtt = true;
  }

  int masterySum = 0;
  int masteryCount = 0;
  for (int j = 0; j < pAt.m_nDamagedObjNum; ++j)
  {
    const unsigned int totalDamage =
      static_cast<unsigned int>(pAt.m_DamList[j].m_nActiveDamage + pAt.m_DamList[j].m_nDamage);
    const int level = static_cast<int>(CPlayerDB::GetLevel(&m_Param));
    CCharacter *damaged = pAt.m_DamList[j].m_pChar;
    damaged->SetDamage(
      static_cast<int>(totalDamage),
      this,
      level,
      pAt.m_bIsCrtAtt,
      byEffectCode,
      pSkillFld->m_dwIndex,
      true);

    if (IsChaosMode())
    {
      if (GetObjRace() == damaged->GetObjRace(damaged))
      {
        continue;
      }
    }

    if (GetObjRace() == damaged->GetObjRace(damaged) && !damaged->m_ObjID.m_byID)
    {
      CPlayer *damagedPlayer = static_cast<CPlayer *>(damaged);
      if (CPlayer::IsPunished(damagedPlayer, 1u, 0))
      {
        continue;
      }
    }

    if (!pAt.m_bFailure && !byEffectCode && pSkillFld->m_nMastIndex < 8u)
    {
      const int dstLevel = static_cast<int>(damaged->GetLevel(damaged));
      if (IsPassMasteryLimitLvDiff(dstLevel))
      {
        const float damageRatio =
          static_cast<float>(pAt.m_DamList[j].m_nDamage)
          / static_cast<float>(damaged->GetMaxHP(damaged));
        if (damageRatio >= 0.0099999998f)
        {
          if (pSkillFld->m_nClass == 1)
          {
            const int level = static_cast<int>(damaged->GetLevel(damaged));
            masterySum += 2 * static_cast<int>(GetMasteryCumAfterAttack(level));
          }
          else
          {
            const int level = static_cast<int>(damaged->GetLevel(damaged));
            masterySum += static_cast<int>(GetMasteryCumAfterAttack(level));
          }
          ++masteryCount;
        }
      }
    }
  }

  if (masteryCount > 0 && !IsInTown())
  {
    const unsigned int dwAlter = static_cast<unsigned int>(masterySum / masteryCount);
    if (dwAlter > 0 && !byEffectCode)
    {
      Emb_AlterStat(3u, pSkillFld->m_dwIndex, dwAlter, 0, "CPlayer::pc_PlayAttack_Skill()---0", 1);
      if (m_pmWpn.byWpType == 7)
      {
        Emb_AlterStat(6u, 0, 1u, 0, "CPlayer::pc_PlayAttack_Skill()---1", 1);
      }
      else
      {
        Emb_AlterStat(0, m_pmWpn.byWpClass, 1u, 0, "CPlayer::pc_PlayAttack_Skill()---2", 1);
      }
    }
  }

  if (pTarget && m_pRecalledAnimusChar && pTarget->GetHP(pTarget) > 0)
  {
    CAnimus::MasterAttack_MasterInform(m_pRecalledAnimusChar, pTarget);
  }
  if (pTarget && pTarget->GetHP(pTarget) > 0)
  {
    _TOWER_PARAM::NotifyOwnerAttackInform(&m_pmTwr, pTarget);
  }

  if (IsSiegeMode())
  {
    _STORAGE_LIST::_db_con *pItem = m_pSiegeItem;
    if (!Emb_AlterDurPoint(0, m_pSiegeItem->m_byStorageIndex, -1, 0, 1))
    {
      CMgrAvatorItemHistory::consume_del_item(
        &CPlayer::s_MgrItemHistory,
        m_ObjID.m_wIndex,
        pItem,
        m_szItemHistoryFileName);
    }
  }

  SetBattleMode(true);
}

void CPlayer::pc_PlayAttack_Force(
  CCharacter *pDst,
  float *pfAreaPos,
  unsigned __int16 wForceSerial,
  unsigned __int16 *pConsumeSerial,
  unsigned __int16 wEffBtSerial)
{
  _STORAGE_LIST::_db_con *pForceItem = nullptr;
  _force_fld *pForceFld = nullptr;
  _STORAGE_LIST::_db_con *pEffBulletItem = nullptr;
  _BulletItem_fld *pEffBulletFld = nullptr;

  float tarPos[3] = {pfAreaPos[0], m_fCurPos[1], pfAreaPos[1]};
  CCharacter *pTarget = pDst;
  unsigned __int16 decPoint[3]{};

  if (pDst)
  {
    memcpy_0(pfAreaPos, pDst->m_fCurPos, sizeof(float) * 3);
  }

  char errCode = _pre_check_force_attack(
    pTarget,
    tarPos,
    wForceSerial,
    &pForceFld,
    &pForceItem,
    decPoint,
    wEffBtSerial,
    &pEffBulletItem,
    &pEffBulletFld);

  _STORAGE_LIST::_db_con *consumeItems[3]{};
  int consumeCounts[3]{};
  bool consumeOverlap[3]{};
  if (!errCode
      && !GetUseConsumeItem(
        pForceFld->m_ConsumeItemList,
        pConsumeSerial,
        consumeItems,
        consumeCounts,
        consumeOverlap))
  {
    errCode = -61;
  }

  if (errCode)
  {
    SendMsg_AttackResult_Error(errCode);
    if (m_bMove)
    {
      CCharacter::Stop(this);
      CGameObject::SendMsg_BreakStop(this);
    }
    return;
  }

  if (pForceFld->m_nEffectGroup == 6 || pForceFld->m_nEffectGroup == 4)
  {
    pTarget = nullptr;
  }
  if (pForceFld->m_nEffectGroup == 4)
  {
    memcpy_0(tarPos, m_fCurPos, sizeof(float) * 3);
  }

  CAttack pAt(this);
  if (_effect_parameter::GetEff_State(&m_EP, 14))
  {
    CCharacter::RemoveSFContHelpByEffect(this, 2, 14);
  }

  _attack_param pAP{};
  unsigned __int16 bulletIndex = static_cast<unsigned __int16>(-1);
  float fAddEffBulletFc = FLOAT_1_0;
  if (pEffBulletItem)
  {
    fAddEffBulletFc = pEffBulletFld->m_fGAAF;
    bulletIndex = pEffBulletItem->m_wItemIndex;
  }

  make_force_attack_param(pTarget, pForceFld, pForceItem, tarPos, &pAP, pEffBulletItem, fAddEffBulletFc);
  if (!pEffBulletItem || wEffBtSerial == 0xFFFF)
  {
    pAt.AttackForce(&pAP, false);
  }
  else
  {
    pAt.AttackForce(&pAP, true);
  }

  bool bActiveSucc = false;
  if (pAt.m_DamList[0].m_nDamage > 0 && m_pmWpn.nActiveType > -1 && rand() % 100 < m_pmWpn.nActiveProb)
  {
    bActiveSucc = WeaponSFActive(pAt.m_DamList, &pAt.m_nDamagedObjNum, &pAP.nShotNum, 0xFFFF);
  }
  pAt.SetActiveSucc(bActiveSucc);

  const float effPlus = _effect_parameter::GetEff_Plus(&m_EP, 13);
  _ATTACK_DELAY_CHECKER::SetDelay(&m_AttDelayChker, static_cast<unsigned int>(pForceFld->m_fActDelay + effPlus));

  if (_effect_parameter::GetEff_State(&m_EP, 21))
  {
    if (pAP.nAttactType != 4 && pAP.nAttactType != 5 && pAP.nAttactType != 6 && pAP.nAttactType != 7)
    {
      return;
    }
    CCharacter::RemoveSFContHelpByEffect(this, 2, 21);
  }

  for (int nParamCode = 0; nParamCode < 3; ++nParamCode)
  {
    if (decPoint[nParamCode])
    {
      const int gauge = GetGauge(nParamCode);
      int newValue = 0;
      if (gauge - decPoint[nParamCode] > 0)
      {
        newValue = gauge - decPoint[nParamCode];
      }
      SetGauge(nParamCode, newValue, true);
    }
    SendMsg_Recover();
  }

  DeleteUseConsumeItem(consumeItems, consumeCounts, consumeOverlap);

  CPartyModeKillMonsterExpNotify kPartyExpNotify;
  if (!pAt.m_bFailure)
  {
    const int nTotalDam = static_cast<int>(_check_exp_after_attack(pAt.m_nDamagedObjNum, pAt.m_DamList, &kPartyExpNotify));
    if (nTotalDam > 0)
    {
      _check_dst_param_after_attack(nTotalDam, pTarget);
    }
  }

  SendMsg_AttackResult_Force(&pAt);
  kPartyExpNotify.Notify();

  int masterySum = 0;
  int masteryCount = 0;
  for (int j = 0; j < pAt.m_nDamagedObjNum; ++j)
  {
    const unsigned int totalDamage =
      static_cast<unsigned int>(pAt.m_DamList[j].m_nActiveDamage + pAt.m_DamList[j].m_nDamage);
    const int level = static_cast<int>(CPlayerDB::GetLevel(&m_Param));
    CCharacter *damaged = pAt.m_DamList[j].m_pChar;
    damaged->SetDamage(
      static_cast<int>(totalDamage),
      this,
      level,
      pAt.m_bIsCrtAtt,
      1,
      pForceFld->m_dwIndex,
      true);

    if (IsChaosMode())
    {
      if (GetObjRace() == damaged->GetObjRace(damaged))
      {
        continue;
      }
    }

    if (GetObjRace() == damaged->GetObjRace(damaged) && !damaged->m_ObjID.m_byID)
    {
      CPlayer *damagedPlayer = static_cast<CPlayer *>(damaged);
      if (CPlayer::IsPunished(damagedPlayer, 1u, 0))
      {
        continue;
      }
    }

    if (!pAt.m_bFailure && pForceFld->m_nMastIndex < 0x18u)
    {
      const int dstLevel = static_cast<int>(damaged->GetLevel(damaged));
      if (IsPassMasteryLimitLvDiff(dstLevel))
      {
        const float damageRatio =
          static_cast<float>(pAt.m_DamList[j].m_nDamage)
          / static_cast<float>(damaged->GetMaxHP(damaged));
        if (damageRatio >= 0.0099999998f)
        {
          const int level = static_cast<int>(damaged->GetLevel(damaged));
          masterySum += static_cast<int>(GetMasteryCumAfterAttack(level));
          ++masteryCount;
        }
      }
    }
  }

  if (masteryCount > 0 && !IsInTown())
  {
    const unsigned int dwAlter = static_cast<unsigned int>(masterySum / masteryCount);
    if (dwAlter > 0)
    {
      Emb_AlterStat(4u, pForceFld->m_nMastIndex, dwAlter, 0, "CPlayer::pc_PlayAttack_Force()---0", 1);
      const int level = static_cast<int>(GetLevel());
      int nAlter = 5 * (level / 10 + 1) * static_cast<int>(dwAlter);
      const int baseAlter = nAlter;
      if (IsApplyPcbangPrimium())
      {
        nAlter = static_cast<int>(
          static_cast<float>(nAlter)
          + static_cast<float>(baseAlter)
              * static_cast<float>(PCBANG_PRIMIUM_FAVOR::SKILL_FORCE_MASTERY - 1.0));
      }
      else
      {
        nAlter = static_cast<int>(
          static_cast<float>(nAlter)
          + static_cast<float>(baseAlter)
              * static_cast<float>(FORCE_LIVER_ACCUM_RATE - 1.0));
      }
      const float effHave = _effect_parameter::GetEff_Have(&m_EP, 6);
      if (effHave > 1.0f)
      {
        nAlter = static_cast<int>(
          static_cast<float>(nAlter)
          + static_cast<float>(baseAlter)
              * static_cast<float>(effHave - 1.0f));
      }
      const unsigned int dwNewStat = Emb_AlterDurPoint(3u, pForceItem->m_byStorageIndex, nAlter, 0, 1);
      SendMsg_FcitemInform(wForceSerial, dwNewStat);
    }
  }

  if (pTarget && m_pRecalledAnimusChar && pTarget->GetHP(pTarget) > 0)
  {
    CAnimus::MasterAttack_MasterInform(m_pRecalledAnimusChar, pTarget);
  }
  if (pTarget && pTarget->GetHP(pTarget) > 0)
  {
    _TOWER_PARAM::NotifyOwnerAttackInform(&m_pmTwr, pTarget);
  }

  SetBattleMode(true);

  if (pEffBulletItem && pAP.nEffShotNum > 0)
  {
    const unsigned __int16 dur = Emb_AlterDurPoint(2u, pEffBulletItem->m_byStorageIndex, -pAP.nEffShotNum, 0, 1);
    if (dur)
    {
      SendMsg_AlterWeaponBulletInform(pEffBulletItem->m_wSerial, dur);
    }
    else
    {
      CMgrAvatorItemHistory::consume_del_item(
        &CPlayer::s_MgrItemHistory,
        m_ObjID.m_wIndex,
        pEffBulletItem,
        m_szItemHistoryFileName);
    }
  }
}

void CPlayer::pc_PlayAttack_Unit(CCharacter *pDst, unsigned __int8 byWeaponPart)
{
  _unit_bullet_param *pBulletParam = nullptr;
  _UnitBullet_fld *pBulletFld = nullptr;
  _UnitPart_fld *pWeaponFld = nullptr;

  const char errCode = _pre_check_unit_attack(pDst, byWeaponPart, &pWeaponFld, &pBulletFld, &pBulletParam);
  if (errCode)
  {
    SendMsg_AttackResult_Error(errCode);
    if (m_bMove)
    {
      CCharacter::Stop(this);
      CGameObject::SendMsg_BreakStop(this);
    }
    return;
  }

  float fAddBulletFc = FLOAT_1_0;
  unsigned __int16 bulletIndex = static_cast<unsigned __int16>(-1);
  if (pBulletFld)
  {
    fAddBulletFc = pBulletFld->m_fGAAF;
    bulletIndex = pBulletFld->m_dwIndex;
  }

  CPlayerAttack pAt(this);
  _attack_param pAP{};

  make_unit_attack_param(pDst, pWeaponFld, fAddBulletFc, &pAP);
  m_byUsingWeaponPart = byWeaponPart;
  pAt.AttackUnit(&pAP);

  _ATTACK_DELAY_CHECKER::SetDelay(&m_AttDelayChker, static_cast<unsigned int>(pWeaponFld->m_nAttackDel));

  if (_effect_parameter::GetEff_State(&m_EP, 14))
  {
    CCharacter::RemoveSFContHelpByEffect(this, 2, 14);
  }
  if (_effect_parameter::GetEff_State(&m_EP, 21))
  {
    if (pAP.nAttactType != 4 && pAP.nAttactType != 5 && pAP.nAttactType != 6 && pAP.nAttactType != 7)
    {
      return;
    }
    CCharacter::RemoveSFContHelpByEffect(this, 2, 21);
  }

  CPartyModeKillMonsterExpNotify kPartyExpNotify;
  if (!pAt.m_bFailure)
  {
    _check_exp_after_attack(pAt.m_nDamagedObjNum, pAt.m_DamList, &kPartyExpNotify);
  }

  if (pBulletParam->wLeftNum >= pWeaponFld->m_nNeedBt)
  {
    pBulletParam->wLeftNum -= static_cast<unsigned __int16>(pWeaponFld->m_nNeedBt);
  }
  else
  {
    pBulletParam->wLeftNum = 0;
  }

  const unsigned __int16 wLeftNum = pBulletParam->wLeftNum;
  SendMsg_AlterUnitBulletInform(byWeaponPart, wLeftNum);
  if (!pBulletParam->wLeftNum)
  {
    pBulletParam->wBulletIndex = 0xFFFF;
    pBulletParam->wLeftNum = 0xFFFF;
  }

  SendMsg_AttackResult_Unit(&pAt, byWeaponPart, bulletIndex);
  kPartyExpNotify.Notify();

  for (int j = 0; j < pAt.m_nDamagedObjNum; ++j)
  {
    const int level = static_cast<int>(CPlayerDB::GetLevel(&m_Param));
    CCharacter *damaged = pAt.m_DamList[j].m_pChar;
    damaged->SetDamage(pAt.m_DamList[j].m_nDamage, this, level, pAt.m_bIsCrtAtt, -1, 0, true);
  }

  SetBattleMode(true);
}

void CPlayer::pc_PlayAttack_Siege(
  CCharacter *pDst,
  float *pfAttackPos,
  unsigned __int8 byAttPart,
  unsigned __int16 wBulletSerial,
  unsigned __int16 wEffBtSerial)
{
  _STORAGE_LIST::_db_con *pItem = nullptr;
  _BulletItem_fld *pBulletFld = nullptr;
  _STORAGE_LIST::_db_con *pEffBulletItem = nullptr;
  _BulletItem_fld *pEffBulletFld = nullptr;

  if (pDst)
  {
    memcpy_0(pfAttackPos, pDst->m_fCurPos, sizeof(float) * 3);
  }

  const char errCode = _pre_check_siege_attack(
    pDst,
    pfAttackPos,
    wBulletSerial,
    &pItem,
    &pBulletFld,
    wEffBtSerial,
    &pEffBulletItem,
    &pEffBulletFld);
  if (errCode)
  {
    SendMsg_AttackResult_Error(errCode);
    if (m_bMove)
    {
      CCharacter::Stop(this);
      CGameObject::SendMsg_BreakStop(this);
    }
    return;
  }

  float fAddBulletFc = FLOAT_1_0;
  unsigned __int16 bulletIndex = static_cast<unsigned __int16>(-1);
  if (pItem)
  {
    fAddBulletFc = pBulletFld->m_fGAAF;
    bulletIndex = pItem->m_wItemIndex;
  }

  CAttack pAt(this);
  _attack_param pAP{};

  unsigned __int16 effBulletIndex = static_cast<unsigned __int16>(-1);
  float fAddEffBtFc = 0.0f;
  if (pEffBulletItem)
  {
    fAddEffBtFc = pEffBulletFld->m_fGAAF;
    effBulletIndex = pEffBulletItem->m_wItemIndex;
  }

  make_siege_attack_param(pDst, pfAttackPos, byAttPart, pBulletFld, fAddBulletFc, &pAP, pEffBulletFld, fAddEffBtFc);
  if (!pEffBulletItem || effBulletIndex == 0xFFFF)
  {
    pAt.AttackGen(&pAP, false, false);
  }
  else
  {
    pAt.AttackGen(&pAP, false, true);
  }

  for (int j = 0; j < pAt.m_nDamagedObjNum; ++j)
  {
    pAt.m_DamList[j].m_nDamage = static_cast<int>(static_cast<double>(pAt.m_DamList[j].m_nDamage) * 1.25);
  }

  int effPlus = 0;
  if (m_pmWpn.byWpType != 7)
  {
    effPlus = static_cast<int>(_effect_parameter::GetEff_Plus(&m_EP, m_pmWpn.byWpClass + 9));
  }

  const int addDelay = static_cast<int>(CalcEquipAttackDelay());
  const int level = static_cast<int>(GetLevel());
  const unsigned int attackDelay = m_pmWpn.GetAttackDelay(level, addDelay);
  int delay = effPlus + static_cast<int>(attackDelay);
  if (m_pmWpn.byWpType == 7)
  {
    const float effPlusInner = _effect_parameter::GetEff_Plus(&m_EP, 11);
    delay = static_cast<int>(static_cast<float>(delay) + effPlusInner);
  }
  _ATTACK_DELAY_CHECKER::SetDelay(&m_AttDelayChker, static_cast<unsigned int>(delay));

  if (_effect_parameter::GetEff_State(&m_EP, 14))
  {
    CCharacter::RemoveSFContHelpByEffect(this, 2, 14);
  }
  if (_effect_parameter::GetEff_State(&m_EP, 21))
  {
    if (pAP.nAttactType != 4 && pAP.nAttactType != 5 && pAP.nAttactType != 6 && pAP.nAttactType != 7)
    {
      return;
    }
    CCharacter::RemoveSFContHelpByEffect(this, 2, 21);
  }

  if (m_bFreeSFByClass)
  {
    pAt.m_bIsCrtAtt = true;
  }

  CPartyModeKillMonsterExpNotify kPartyExpNotify;
  if (!pAt.m_bFailure)
  {
    const int nTotalDam = static_cast<int>(_check_exp_after_attack(pAt.m_nDamagedObjNum, pAt.m_DamList, &kPartyExpNotify));
    if (nTotalDam > 0)
    {
      _check_dst_param_after_attack(nTotalDam, pDst);
    }
  }

  SendMsg_AttackResult_Siege(&pAt, bulletIndex);
  kPartyExpNotify.Notify();

  int masterySum = 0;
  int masteryCount = 0;
  for (int j = 0; j < pAt.m_nDamagedObjNum; ++j)
  {
    const int level = static_cast<int>(CPlayerDB::GetLevel(&m_Param));
    CCharacter *damaged = pAt.m_DamList[j].m_pChar;
    damaged->SetDamage(pAt.m_DamList[j].m_nDamage, this, level, pAt.m_bIsCrtAtt, -1, 0, true);

    if (IsChaosMode())
    {
      if (GetObjRace() == damaged->GetObjRace(damaged))
      {
        continue;
      }
    }

    if (GetObjRace() == damaged->GetObjRace(damaged) && !damaged->m_ObjID.m_byID)
    {
      CPlayer *damagedPlayer = static_cast<CPlayer *>(damaged);
      if (CPlayer::IsPunished(damagedPlayer, 1u, 0))
      {
        continue;
      }
    }

    if (!pAt.m_bFailure)
    {
      const int dstLevel = static_cast<int>(damaged->GetLevel(damaged));
      if (IsPassMasteryLimitLvDiff(dstLevel))
      {
        const float damageRatio =
          static_cast<float>(pAt.m_DamList[j].m_nDamage)
          / static_cast<float>(damaged->GetMaxHP(damaged));
        if (damageRatio >= 0.029999999f)
        {
          const int level = static_cast<int>(damaged->GetLevel(damaged));
          masterySum += static_cast<int>(GetMasteryCumAfterAttack(level));
          ++masteryCount;
        }
      }
    }
  }

  if (masteryCount > 0 && !IsInTown())
  {
    const unsigned int dwAlter = static_cast<unsigned int>(masterySum / masteryCount);
    if (dwAlter > 0)
    {
      if (m_pmWpn.byWpType == 7)
      {
        Emb_AlterStat(6u, 0, dwAlter, 0, "CPlayer::pc_PlayAttack_Siege()---0", 1);
      }
      else
      {
        Emb_AlterStat(0, m_pmWpn.byWpClass, dwAlter, 0, "CPlayer::pc_PlayAttack_Siege()---1", 1);
      }
    }
  }

  if (pItem)
  {
    const unsigned __int16 dur = Emb_AlterDurPoint(2u, pItem->m_byStorageIndex, -1, 0, 1);
    if (dur)
    {
      SendMsg_AlterWeaponBulletInform(pItem->m_wSerial, dur);
    }
    else
    {
      CMgrAvatorItemHistory::consume_del_item(
        &CPlayer::s_MgrItemHistory,
        m_ObjID.m_wIndex,
        pItem,
        m_szItemHistoryFileName);
    }
  }

  if (IsSiegeMode())
  {
    _STORAGE_LIST::_db_con *siegeItem = m_pSiegeItem;
    if (!Emb_AlterDurPoint(0, m_pSiegeItem->m_byStorageIndex, -1, 0, 1))
    {
      CMgrAvatorItemHistory::consume_del_item(
        &CPlayer::s_MgrItemHistory,
        m_ObjID.m_wIndex,
        siegeItem,
        m_szItemHistoryFileName);
    }
  }

  SetBattleMode(true);

  if (pEffBulletItem)
  {
    const unsigned __int16 dur = Emb_AlterDurPoint(2u, pEffBulletItem->m_byStorageIndex, -1, 0, 1);
    if (dur)
    {
      SendMsg_AlterWeaponBulletInform(pEffBulletItem->m_wSerial, dur);
    }
    else
    {
      CMgrAvatorItemHistory::consume_del_item(
        &CPlayer::s_MgrItemHistory,
        m_ObjID.m_wIndex,
        pEffBulletItem,
        m_szItemHistoryFileName);
    }
  }
}

void CPlayer::pc_PlayAttack_Test(
  unsigned __int8 byEffectCode,
  unsigned __int8 byEffectIndex,
  unsigned __int16 wBulletItemSerial,
  unsigned __int8 byWeaponPart,
  __int16 *pzTar)
{
  _STORAGE_LIST::_db_con *pItem = nullptr;
  unsigned __int16 bulletIndex = static_cast<unsigned __int16>(-1);
  unsigned __int8 sfLevel = 1;

  if (!IsRidingUnit())
  {
    if (wBulletItemSerial != 0xFFFF)
    {
      pItem = IsBulletValidity(wBulletItemSerial);
      if (pItem)
      {
        const unsigned __int16 dur = Emb_AlterDurPoint(2u, pItem->m_byStorageIndex, -1, 0, 1);
        if (dur)
        {
          SendMsg_AlterWeaponBulletInform(pItem->m_wSerial, pItem->m_dwDur);
        }
        else
        {
          CMgrAvatorItemHistory::consume_del_item(
            &CPlayer::s_MgrItemHistory,
            m_ObjID.m_wIndex,
            pItem,
            m_szItemHistoryFileName);
        }
      }
    }

    if (byEffectCode)
    {
      if (byEffectCode == 1)
      {
        _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblEffectData[1], byEffectIndex);
        _STORAGE_LIST::_db_con *forceItem = nullptr;
        for (int j = 0; j < 88; ++j)
        {
          _STORAGE_LIST::_db_con *entry = &m_Param.m_dbForce.m_pStorageList[j];
          if (entry->m_bLoad && CPlayer::s_pnLinkForceItemToEffect[entry->m_wItemIndex] == byEffectIndex)
          {
            forceItem = entry;
            break;
          }
        }
        if (forceItem && record)
        {
          sfLevel = static_cast<unsigned __int8>(
            GetSFLevel(*reinterpret_cast<int *>(&record[4].m_strCode[60]), forceItem->m_dwDur));
        }
      }
    }
    else
    {
      unsigned __int8 *record = reinterpret_cast<unsigned __int8 *>(CRecordData::GetRecord(g_Main.m_tblEffectData, byEffectIndex));
      if (record)
      {
        sfLevel = _MASTERY_PARAM::GetSkillLv(&m_pmMst, *record);
      }
    }

    if (_effect_parameter::GetEff_State(&m_EP, 14))
    {
      CCharacter::RemoveSFContHelpByEffect(this, 2, 14);
    }
    SendMsg_TestAttackResult(byEffectCode, byEffectIndex, bulletIndex, sfLevel, byWeaponPart, pzTar);
    return;
  }

  unsigned __int16 *unitBullet = reinterpret_cast<unsigned __int16 *>(&m_pUsingUnit->dwBullet[byWeaponPart]);
  if (unitBullet[1] && unitBullet[1] != 0xFFFF)
  {
    bulletIndex = *unitBullet;
    _base_fld *unitRecord = nullptr;
    if (byWeaponPart)
    {
      unitRecord = CRecordData::GetRecord(&g_Main.m_tblUnitPart[4], m_pUsingUnit->byPart[4]);
    }
    else
    {
      unitRecord = CRecordData::GetRecord(&g_Main.m_tblUnitPart[3], m_pUsingUnit->byPart[3]);
    }
    if (unitRecord)
    {
      if (unitBullet[1] >= *reinterpret_cast<int *>(unitRecord[4].m_strCode))
      {
        unitBullet[1] -= *reinterpret_cast<unsigned __int16 *>(unitRecord[4].m_strCode);
      }
      else
      {
        unitBullet[1] = 0;
      }
      const unsigned __int16 leftNum = unitBullet[1];
      SendMsg_AlterUnitBulletInform(byWeaponPart, leftNum);
      if (!unitBullet[1])
      {
        *reinterpret_cast<unsigned int *>(unitBullet) = static_cast<unsigned int>(-1);
      }

      if (_effect_parameter::GetEff_State(&m_EP, 14))
      {
        CCharacter::RemoveSFContHelpByEffect(this, 2, 14);
      }
      SendMsg_TestAttackResult(byEffectCode, byEffectIndex, bulletIndex, sfLevel, byWeaponPart, pzTar);
    }
  }
}

void CPlayer::pc_PlayAttack_SelfDestruction()
{
  char errCode = 0;
  if (!IsAttackableInTown() && IsInTown())
  {
    errCode = -31;
  }
  if (m_pmWpn.byWpType == 11 || m_pmWpn.byWpType == 10)
  {
    errCode = -9;
  }
  if (IsRidingUnit())
  {
    errCode = -21;
  }
  if (_effect_parameter::GetEff_State(&m_EP, 20))
  {
    errCode = -37;
  }
  if (_effect_parameter::GetEff_State(&m_EP, 28))
  {
    errCode = -37;
  }
  if (_effect_parameter::GetEff_State(&m_EP, 21))
  {
    errCode = -38;
  }
  if (m_byMoveType == 2)
  {
    errCode = -41;
  }

  if (errCode)
  {
    SendMsg_AttackResult_Error(errCode);
    if (m_bMove)
    {
      CCharacter::Stop(this);
      CGameObject::SendMsg_BreakStop(this);
    }
    return;
  }

  CAttack pAt(this);
  _attack_param pAP{};
  pAP.pDst = nullptr;
  memcpy_0(pAP.fArea, m_fCurPos, sizeof(pAP.fArea));
  pAP.nPart = CCharacter::GetAttackRandomPart(this);
  pAP.nTol = -1;
  pAP.nClass = 1;

  const float nGaMinAF = static_cast<float>(m_pmWpn.nGaMinAF);
  const float effRate32 = _effect_parameter::GetEff_Rate(&m_EP, 32);
  const float minBase = nGaMinAF * effRate32;
  const float effRate29 = _effect_parameter::GetEff_Rate(&m_EP, 29);
  pAP.nMinAF = static_cast<int>(minBase * effRate29 * m_fSelfDestructionDamage);

  const float nGaMaxAF = static_cast<float>(m_pmWpn.nGaMaxAF);
  const float maxBase = nGaMaxAF * effRate32;
  pAP.nMaxAF = static_cast<int>(maxBase * effRate29 * m_fSelfDestructionDamage);

  pAP.nMaxAttackPnt = m_nMaxAttackPnt;
  pAP.nMinSel = 30;
  pAP.nMaxSel = 50;
  pAP.nAttactType = 6;
  pAP.nExtentRange = 110;

  pAt.AttackGen(&pAP, false, false);

  if (_effect_parameter::GetEff_State(&m_EP, 14))
  {
    CCharacter::RemoveSFContHelpByEffect(this, 2, 14);
  }

  CPartyModeKillMonsterExpNotify kPartyExpNotify;
  if (!pAt.m_bFailure)
  {
    _check_exp_after_attack(pAt.m_nDamagedObjNum, pAt.m_DamList, &kPartyExpNotify);
  }

  SendMsg_AttackResult_SelfDestruction(&pAt);
  SetBattleMode(true);
  kPartyExpNotify.Notify();

  for (int j = 0; j < pAt.m_nDamagedObjNum; ++j)
  {
    const int level = static_cast<int>(CPlayerDB::GetLevel(&m_Param));
    CCharacter *damaged = pAt.m_DamList[j].m_pChar;
    damaged->SetDamage(pAt.m_DamList[j].m_nDamage, this, level, pAt.m_bIsCrtAtt, -1, 0, true);
  }

  m_byMoveType = 0;
  SenseState();
  SetHP(1, false);
  SetFP(1, false);
  SetSP(1, false);
  SendMsg_SetHPInform();
  SendMsg_SetFPInform();
  SendMsg_SetSPInform();
}

unsigned int CPlayer::_check_exp_after_attack(
  int nDamagedObjNum,
  _be_damaged_char *pList,
  CPartyModeKillMonsterExpNotify *kPartyExpNotify)
{
  unsigned int totalDamage = 0;
  for (int j = 0; j < nDamagedObjNum; ++j)
  {
    _be_damaged_char *entry = &pList[j];
    if (entry->m_nDamage > 0)
    {
      totalDamage += entry->m_nDamage;
      totalDamage += entry->m_nActiveDamage;
    }
    if (entry->m_pChar->IsRewardExp(entry->m_pChar) && entry->m_nDamage > 1)
    {
      const int nDam = entry->m_nActiveDamage + entry->m_nDamage;
      CalcExp(entry->m_pChar, nDam, kPartyExpNotify);
    }
  }
  return totalDamage;
}

void CPlayer::_check_dst_param_after_attack(int nTotalDam, CCharacter *pTarget)
{
  if (_effect_parameter::GetEff_Rate(&m_EP, 13) > 1.0f)
  {
    const float base = static_cast<float>(nTotalDam);
    const float effRate = _effect_parameter::GetEff_Rate(&m_EP, 13);
    const float rateBase = base * (effRate - 1.0f);
    const float rateMul = _effect_parameter::GetEff_Rate(&m_EP, 19);
    const int addFp = static_cast<int>(rateBase * rateMul);
    int newFp = CPlayerDB::GetFP(&m_Param) + addFp;
    const int maxFp = GetMaxFP();
    if (newFp > maxFp)
    {
      newFp = GetMaxFP();
    }
    SetFP(newFp, false);
  }

  if (_effect_parameter::GetEff_Rate(&m_EP, 12) > 1.0f)
  {
    const float base = static_cast<float>(nTotalDam);
    const float effRate = _effect_parameter::GetEff_Rate(&m_EP, 12);
    const float rateBase = base * (effRate - 1.0f);
    const float rateMul = _effect_parameter::GetEff_Rate(&m_EP, 18);
    const int addHp = static_cast<int>(rateBase * rateMul);
    const int curHp = CPlayerDB::GetHP(&m_Param);
    SetHP(curHp + addHp, false);
  }

  if (_effect_parameter::GetEff_Rate(&m_EP, 40) > 1.0f)
  {
    const float base = static_cast<float>(nTotalDam);
    const float effRate = _effect_parameter::GetEff_Rate(&m_EP, 40);
    const float rateBase = base * (effRate - 1.0f);
    const float rateMul = _effect_parameter::GetEff_Rate(&m_EP, 20);
    const int addSp = static_cast<int>(rateBase * rateMul);
    int newSp = CPlayerDB::GetSP(&m_Param) + addSp;
    const int maxSp = GetMaxSP();
    if (newSp > maxSp)
    {
      newSp = GetMaxSP();
    }
    SetSP(newSp, false);
  }

  if (pTarget)
  {
    if (_effect_parameter::GetEff_State(&m_EP, 13))
    {
      if (pTarget->GetHP(pTarget) > 0)
      {
        const bool byte2Zero = BYTE2(pTarget[1].m_fCurPos[2]) == 0;
        const bool byte0Zero = reinterpret_cast<unsigned char *>(&pTarget[1].m_fCurPos[2])[0] == 0;
        if (pTarget->m_ObjID.m_byID || byte2Zero || byte0Zero)
        {
          pTarget->SetStun(true);
          pTarget->SendMsg_StunInform(pTarget);
        }
      }
    }
    else if (_effect_parameter::GetEff_Rate(&m_EP, 5) > 1.0f)
    {
      const unsigned __int16 stunRate = static_cast<unsigned __int16>(
        (_effect_parameter::GetEff_Rate(&m_EP, 5) - 1.0f) * 100.0f);
      if (stunRate > rand() % 1000 && pTarget->GetHP(pTarget) > 0)
      {
        const bool byte2Zero = BYTE2(pTarget[1].m_fCurPos[2]) == 0;
        const bool byte0Zero = reinterpret_cast<unsigned char *>(&pTarget[1].m_fCurPos[2])[0] == 0;
        if (pTarget->m_ObjID.m_byID || byte2Zero || byte0Zero)
        {
          pTarget->SetStun(true);
          pTarget->SendMsg_StunInform(pTarget);
        }
      }
    }
  }
}

bool CPlayer::IsPassExpLimitLvDiff(int iDstLevel, bool *bGetAttackExp)
{
  *bGetAttackExp = true;
  const int diff = static_cast<int>(GetLevel()) - iDstLevel;
  const float effHave = _effect_parameter::GetEff_Have(&m_EP, 52);
  if (static_cast<float>(diff) > effHave + 3.0f)
  {
    *bGetAttackExp = false;
    return false;
  }
  if ((-10.0f - effHave) > static_cast<float>(diff))
  {
    if (!CPartyPlayer::IsPartyMode(m_pPartyMgr))
    {
      *bGetAttackExp = false;
      return false;
    }
    *bGetAttackExp = false;
  }
  return true;
}

float CPlayer::GetPartyExpDistributionRate(int iPartyMemberLevel, int iMaxLevel, int i2ndLevel)
{
  if (!iMaxLevel || !i2ndLevel)
  {
    return 0.0f;
  }
  const float rate = 0.050000001f;
  int diff = 0;
  if (iPartyMemberLevel >= iMaxLevel)
  {
    diff = iPartyMemberLevel - i2ndLevel;
  }
  else
  {
    diff = iPartyMemberLevel - iMaxLevel;
  }
  return rate * static_cast<float>(diff);
}

void CPlayer::CalcExp(CCharacter *pDst, int nDam, CPartyModeKillMonsterExpNotify *kPartyExpNotify)
{
  auto *pMon = static_cast<CMonster *>(pDst);
  if (pMon->m_ObjID.m_byID == 1 && nDam > 0 && !IsInTown())
  {
    bool bGetAttackExp = true;
    const int dstLevel = static_cast<int>(pMon->GetLevel(pMon));
    if (IsPassExpLimitLvDiff(dstLevel, &bGetAttackExp))
    {
      _base_fld *record = pMon->m_pRecordSet;
      int remainHp = static_cast<int>(pMon->GetHP(pMon)) - nDam;
      if (CMonster::IsBossMonster(pMon))
      {
        bGetAttackExp = false;
      }
      if (bGetAttackExp)
      {
        int dam = nDam;
        if (remainHp < 0)
        {
          remainHp = 0;
          dam = static_cast<int>(pMon->GetHP(pMon));
        }
        const float baseExp =
          (*(float *)&record[4].m_strCode[16] * 0.69999999f)
          * (static_cast<float>(dam) / *reinterpret_cast<float *>(&record[25].m_strCode[4]));
        if (IsRidingUnit())
        {
          const float expUnit = baseExp / 180.0f;
          const int level = static_cast<int>(pMon->GetLevel(pMon));
          Emb_AlterStat(6u, 0, static_cast<int>(expUnit + static_cast<float>(level)), 0, "CPlayer::CalcExp()--0", 1);
        }
        else
        {
          AlterExp(baseExp, 0, 0, 0);
        }
      }

      if (!remainHp)
      {
        float killExp = 0.0f;
        const int emoState = CMonster::GetEmotionState(pMon);
        if (emoState == 4)
        {
          killExp = *reinterpret_cast<float *>(&record[4].m_strCode[16]) * 0.5f;
        }
        else
        {
          killExp = *reinterpret_cast<float *>(&record[4].m_strCode[16]) * 0.30000001f;
        }

        if (CPartyPlayer::IsPartyMode(m_pPartyMgr))
        {
          CPlayer *members[8]{};
          const unsigned __int8 memberCount = _GetPartyMemberInCircle(members, 8, 1);
          if (memberCount)
          {
            killExp = killExp * CPlayer::s_fExpDivUnderParty_Kill[memberCount - 1];
          }

          int maxLevel = 0;
          int secondLevel = 0;
          float levelSum = 0.0f;
          for (int j = 0; j < memberCount; ++j)
          {
            const int level = static_cast<int>(members[j]->GetLevel(members[j]));
            levelSum = levelSum + static_cast<float>(level);
            if (level <= maxLevel)
            {
              if (level > secondLevel)
              {
                secondLevel = level;
              }
            }
            else
            {
              secondLevel = maxLevel;
              maxLevel = level;
            }
          }

          if (levelSum > 1.0f)
          {
            kPartyExpNotify->SetKillMonsterFlag();
            for (int j = 0; j < memberCount; ++j)
            {
              const int level = static_cast<int>(members[j]->GetLevel(members[j]));
              float fExp = (killExp * static_cast<float>(level)) / levelSum;
              const int memberLevel = static_cast<int>(members[j]->GetLevel(members[j]));
              const float partyRate = GetPartyExpDistributionRate(memberLevel, maxLevel, secondLevel);
              fExp = fExp + (partyRate * fExp);
              if (fExp >= 1.0f)
              {
                if (members[j]->IsRidingUnit())
                {
                  const float expUnit = fExp / 180.0f;
                  const int monLevel = static_cast<int>(pMon->GetLevel(pMon));
                  members[j]->Emb_AlterStat(
                    6u,
                    0,
                    static_cast<int>(expUnit + static_cast<float>(monLevel)),
                    0,
                    "CPlayer::CalcExp()--2",
                    1);
                }
                else
                {
                  members[j]->AlterExp(fExp, 0, 0, 0);
                  kPartyExpNotify->Add(members[j], fExp);
                }
              }
            }
          }
        }
        else if (IsRidingUnit())
        {
          const float expUnit = killExp / 180.0f;
          const int level = static_cast<int>(pMon->GetLevel(pMon));
          Emb_AlterStat(6u, 0, static_cast<int>(expUnit + static_cast<float>(level)), 0, "CPlayer::CalcExp()--1", 1);
        }
        else
        {
          AlterExp(killExp, 0, 0, 0);
        }
      }
    }
  }
}

void CPlayer::SendMsg_AttackResult_Gen(CAttack *pAt, unsigned __int16 wBulletIndex)
{
  _attack_gen_result_zocl msg;
  msg.byAtterID = m_ObjID.m_byID;
  msg.dwAtterSerial = m_dwObjSerial;
  msg.byAttackPart = pAt->m_pp->nPart;
  msg.bCritical = pAt->m_bIsCrtAtt;
  msg.wBulletIndex = wBulletIndex;
  msg.bWPActive = pAt->m_bActiveSucc;
  msg.byListNum = static_cast<unsigned __int8>(pAt->m_nDamagedObjNum);
  for (int j = 0; j < pAt->m_nDamagedObjNum; ++j)
  {
    msg.DamList[j].byDstID = pAt->m_DamList[j].m_pChar->m_ObjID.m_byID;
    msg.DamList[j].dwDstSerial = pAt->m_DamList[j].m_pChar->m_dwObjSerial;
    msg.DamList[j].wDamage = static_cast<unsigned __int16>(pAt->m_DamList[j].m_nDamage);
    msg.DamList[j].bActive = pAt->m_DamList[j].m_bActiveSucc;
    msg.DamList[j].wActiveDamage = static_cast<unsigned __int16>(pAt->m_DamList[j].m_nActiveDamage);
  }
  unsigned __int8 type[2]{5, 7};
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(msg.size()), true);
}

void CPlayer::SendMsg_AttackResult_Skill(unsigned __int8 byEffectCode, CPlayerAttack *pAt, unsigned __int16 wBulletIndex)
{
  _attack_skill_result_zocl msg;
  msg.byAtterID = m_ObjID.m_byID;
  msg.dwAtterSerial = m_dwObjSerial;
  msg.byEffectCode = byEffectCode;
  msg.wSkillIndex = pAt->m_pp->pFld->m_dwIndex;
  msg.bySkillLv = static_cast<unsigned __int8>(pAt->m_pp->nLevel);
  msg.byAttackPart = pAt->m_pp->nPart;
  msg.bCritical = pAt->m_bIsCrtAtt;
  msg.wBulletIndex = wBulletIndex;
  msg.zAttackPos[0] = static_cast<int>(pAt->m_pp->fArea[0]);
  msg.zAttackPos[1] = static_cast<int>(pAt->m_pp->fArea[2]);
  msg.bWPActive = pAt->m_bActiveSucc;
  msg.byListNum = static_cast<unsigned __int8>(pAt->m_nDamagedObjNum);
  for (int j = 0; j < pAt->m_nDamagedObjNum; ++j)
  {
    msg.DamList[j].byDstID = pAt->m_DamList[j].m_pChar->m_ObjID.m_byID;
    msg.DamList[j].dwDstSerial = pAt->m_DamList[j].m_pChar->m_dwObjSerial;
    msg.DamList[j].wDamage = static_cast<unsigned __int16>(pAt->m_DamList[j].m_nDamage);
    msg.DamList[j].bActive = pAt->m_DamList[j].m_bActiveSucc;
    msg.DamList[j].wActiveDamage = static_cast<unsigned __int16>(pAt->m_DamList[j].m_nActiveDamage);
  }
  unsigned __int8 type[2]{5, 8};
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(msg.size()), true);
}

void CPlayer::SendMsg_AttackResult_Force(CAttack *pAt)
{
  _attack_force_result_zocl msg;
  msg.byAtterID = m_ObjID.m_byID;
  msg.dwAtterSerial = m_dwObjSerial;
  msg.byForceIndex = pAt->m_pp->pFld->m_dwIndex;
  msg.byForceLv = static_cast<unsigned __int8>(pAt->m_pp->nLevel);
  msg.zAreaPos[0] = static_cast<int>(pAt->m_pp->fArea[0]);
  msg.zAreaPos[1] = static_cast<int>(pAt->m_pp->fArea[2]);
  msg.byAttackPart = pAt->m_pp->nPart;
  msg.bCritical = pAt->m_bIsCrtAtt;
  msg.bWPActive = pAt->m_bActiveSucc;
  msg.byListNum = static_cast<unsigned __int8>(pAt->m_nDamagedObjNum);
  for (int j = 0; j < pAt->m_nDamagedObjNum; ++j)
  {
    msg.DamList[j].byDstID = pAt->m_DamList[j].m_pChar->m_ObjID.m_byID;
    msg.DamList[j].dwDstSerial = pAt->m_DamList[j].m_pChar->m_dwObjSerial;
    msg.DamList[j].wDamage = static_cast<unsigned __int16>(pAt->m_DamList[j].m_nDamage);
    msg.DamList[j].bActive = pAt->m_DamList[j].m_bActiveSucc;
    msg.DamList[j].wActiveDamage = static_cast<unsigned __int16>(pAt->m_DamList[j].m_nActiveDamage);
  }
  unsigned __int8 type[2]{5, 9};
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(msg.size()), true);
}

void CPlayer::SendMsg_AttackResult_Unit(CAttack *pAt, unsigned __int8 byWeaponPart, unsigned __int16 wBulletIndex)
{
  _attack_unit_result_zocl msg;
  msg.dwAtterSerial = m_dwObjSerial;
  msg.byWeaponPart = byWeaponPart;
  msg.wWeaponIndex = pAt->m_pp->pFld->m_dwIndex;
  msg.wBulletIndex = wBulletIndex;
  msg.byAttackPart = pAt->m_pp->nPart;
  msg.bCritical = pAt->m_bIsCrtAtt;
  msg.byListNum = static_cast<unsigned __int8>(pAt->m_nDamagedObjNum);
  for (int j = 0; j < pAt->m_nDamagedObjNum; ++j)
  {
    msg.DamList[j].byDstID = pAt->m_DamList[j].m_pChar->m_ObjID.m_byID;
    msg.DamList[j].dwDstSerial = pAt->m_DamList[j].m_pChar->m_dwObjSerial;
    msg.DamList[j].wDamage = static_cast<unsigned __int16>(pAt->m_DamList[j].m_nDamage);
  }
  unsigned __int8 type[2]{5, 10};
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(msg.size()), true);
}

void CPlayer::SendMsg_AttackResult_Siege(CAttack *pAt, unsigned __int16 wBulletIndex)
{
  _attack_siege_result_zocl msg;
  msg.byAtterID = m_ObjID.m_byID;
  msg.dwAtterSerial = m_dwObjSerial;
  msg.byAttackPart = pAt->m_pp->nPart;
  msg.bCritical = pAt->m_bIsCrtAtt;
  msg.wBulletIndex = wBulletIndex;
  msg.zTar[0] = static_cast<short>(pAt->m_pp->fArea[0]);
  msg.zTar[1] = static_cast<short>(pAt->m_pp->fArea[2]);
  msg.byListNum = static_cast<unsigned __int8>(pAt->m_nDamagedObjNum);
  for (int j = 0; j < pAt->m_nDamagedObjNum; ++j)
  {
    msg.DamList[j].byDstID = pAt->m_DamList[j].m_pChar->m_ObjID.m_byID;
    msg.DamList[j].dwDstSerial = pAt->m_DamList[j].m_pChar->m_dwObjSerial;
    msg.DamList[j].wDamage = static_cast<unsigned __int16>(pAt->m_DamList[j].m_nDamage);
  }
  unsigned __int8 type[2]{5, 122};
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(msg.size()), true);
}

void CPlayer::SendMsg_AttackResult_Count(CAttack *pAt)
{
  _attack_count_result_zocl msg;
  msg.dwAtterSerial = m_dwObjSerial;
  msg.byAttackPart = pAt->m_pp->nPart;
  msg.bCritical = pAt->m_bIsCrtAtt;
  msg.bWPActive = pAt->m_bActiveSucc;
  msg.byListNum = static_cast<unsigned __int8>(pAt->m_nDamagedObjNum);
  for (int j = 0; j < pAt->m_nDamagedObjNum; ++j)
  {
    msg.DamList[j].byDstID = pAt->m_DamList[j].m_pChar->m_ObjID.m_byID;
    msg.DamList[j].dwDstSerial = pAt->m_DamList[j].m_pChar->m_dwObjSerial;
    msg.DamList[j].wDamage = static_cast<unsigned __int16>(pAt->m_DamList[j].m_nDamage);
    msg.DamList[j].bActive = pAt->m_DamList[j].m_bActiveSucc;
    msg.DamList[j].wActiveDamage = static_cast<unsigned __int16>(pAt->m_DamList[j].m_nActiveDamage);
  }
  unsigned __int8 type[2]{5, 11};
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(msg.size()), true);
}

void CPlayer::SendMsg_AttackResult_SelfDestruction(CAttack *pAt)
{
  _attack_selfdestruction_result_zocl msg;
  msg.byAtterID = m_ObjID.m_byID;
  msg.dwAtterSerial = m_dwObjSerial;
  msg.byAttackPart = pAt->m_pp->nPart;
  msg.bCritical = pAt->m_bIsCrtAtt;
  msg.wBulletIndex = static_cast<unsigned __int16>(-1);
  msg.byListNum = static_cast<unsigned __int8>(pAt->m_nDamagedObjNum);
  for (int j = 0; j < pAt->m_nDamagedObjNum; ++j)
  {
    msg.DamList[j].byDstID = pAt->m_DamList[j].m_pChar->m_ObjID.m_byID;
    msg.DamList[j].dwDstSerial = pAt->m_DamList[j].m_pChar->m_dwObjSerial;
    msg.DamList[j].wDamage = static_cast<unsigned __int16>(pAt->m_DamList[j].m_nDamage);
  }
  unsigned __int8 type[2]{5, 25};
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(msg.size()), true);
}

void CPlayer::SendMsg_AttackResult_Error(char nErrCode)
{
  char msg[32]{};
  msg[0] = nErrCode;
  unsigned __int8 type[2]{5, 6};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_TestAttackResult(
  unsigned __int8 byEffectCode,
  unsigned __int8 byEffectIndex,
  unsigned __int16 wBulletItemIndex,
  unsigned __int8 byEffectLv,
  unsigned __int8 byWeaponPart,
  __int16 *pzTar)
{
  char msg[14]{};
  *reinterpret_cast<unsigned int *>(msg) = m_dwObjSerial;
  *reinterpret_cast<unsigned __int16 *>(msg + 4) = wBulletItemIndex;
  msg[6] = static_cast<char>(byEffectCode);
  msg[7] = static_cast<char>(byEffectIndex);
  msg[8] = static_cast<char>(byEffectLv);
  msg[9] = static_cast<char>(byWeaponPart);
  *reinterpret_cast<__int16 *>(msg + 10) = pzTar[0];
  *reinterpret_cast<__int16 *>(msg + 12) = pzTar[1];

  unsigned __int8 type[2]{5, 12};
  CircleReport(type, msg, 14, true);
}

void CPlayer::SendMsg_AlterWeaponBulletInform(unsigned __int16 wItemSerial, unsigned __int16 wLeftNum)
{
  char msg[4]{};
  *reinterpret_cast<unsigned __int16 *>(msg) = wItemSerial;
  *reinterpret_cast<unsigned __int16 *>(msg + 2) = wLeftNum;
  unsigned __int8 type[2]{5, 21};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, msg, 4u);
}

void CPlayer::SendMsg_AlterUnitBulletInform(char byPart, unsigned __int16 wLeftNum)
{
  char msg[3]{};
  msg[0] = byPart;
  *reinterpret_cast<unsigned __int16 *>(msg + 1) = wLeftNum;
  unsigned __int8 type[2]{5, 22};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, msg, 3u);
}

void CPlayer::SendMsg_Recover()
{
  char msg[8]{};
  *reinterpret_cast<unsigned __int16 *>(msg) = static_cast<unsigned __int16>(CPlayerDB::GetHP(&m_Param));
  *reinterpret_cast<unsigned __int16 *>(msg + 2) = static_cast<unsigned __int16>(CPlayerDB::GetFP(&m_Param));
  *reinterpret_cast<unsigned __int16 *>(msg + 4) = static_cast<unsigned __int16>(CPlayerDB::GetSP(&m_Param));
  *reinterpret_cast<unsigned __int16 *>(msg + 6) = static_cast<unsigned __int16>(CPlayerDB::GetDP(&m_Param));
  unsigned __int8 type[2]{11, 2};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_ObjID.m_wIndex, type, msg, 8u);
}

char CPlayer::GetUseConsumeItem(
  _consume_item_list *pConsumeList,
  unsigned __int16 *pItemSerials,
  _STORAGE_LIST::_db_con **ppConsumeItems,
  int *pnConsume,
  bool *pbOverLap)
{
  if (!pConsumeList || !pItemSerials || !ppConsumeItems || !pnConsume || !pbOverLap)
  {
    return 0;
  }

  for (int j = 0; j < 3; ++j)
  {
    const int needCount = pConsumeList[j].m_nNeedItemCount;
    if (strcmp_0(pConsumeList[j].m_itmNeedItemCode, "-1") && needCount >= 0)
    {
      if (pItemSerials[j] == 255)
      {
        return 0;
      }
      _STORAGE_LIST::_db_con *item = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, pItemSerials[j]);
      if (!item)
      {
        return 0;
      }
      _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblItemData[item->m_byTableCode], item->m_wItemIndex);
      if (strcmp_0(pConsumeList[j].m_itmNeedItemCode, record->m_strCode))
      {
        return 0;
      }
      if (needCount > 0 && IsOverLapItem(item->m_byTableCode))
      {
        if (item->m_dwDur < needCount)
        {
          return 0;
        }
        pbOverLap[j] = true;
      }
      ppConsumeItems[j] = item;
      pnConsume[j] = needCount;
    }
  }
  return 1;
}

char CPlayer::DeleteUseConsumeItem(
  _STORAGE_LIST::_db_con **ppConsumeItems,
  int *pnConsume,
  bool *pbOverLap)
{
  if (!ppConsumeItems || !pnConsume || !pbOverLap)
  {
    return 0;
  }

  for (int j = 0; j < 3; ++j)
  {
    if (ppConsumeItems[j] && pnConsume[j] > 0)
    {
      if (pbOverLap[j])
      {
        const unsigned int dwDur = Emb_AlterDurPoint(
          ppConsumeItems[j]->m_pInList->m_nListCode,
          ppConsumeItems[j]->m_byStorageIndex,
          -pnConsume[j],
          0,
          0);
        if (dwDur)
        {
          SendMsg_AdjustAmountInform(0, ppConsumeItems[j]->m_wSerial, dwDur);
        }
        else
        {
          CMgrAvatorItemHistory::consume_del_item(
            &CPlayer::s_MgrItemHistory,
            m_ObjID.m_wIndex,
            ppConsumeItems[j],
            m_szItemHistoryFileName);
        }
      }
      else
      {
        Emb_DelStorage(
          ppConsumeItems[j]->m_pInList->m_nListCode,
          ppConsumeItems[j]->m_byStorageIndex,
          0,
          1,
          "CPlayer::DeleteUseConsumeItem()");
        CMgrAvatorItemHistory::consume_del_item(
          &CPlayer::s_MgrItemHistory,
          m_ObjID.m_wIndex,
          ppConsumeItems[j],
          m_szItemHistoryFileName);
      }
    }
  }
  return 1;
}

__int64 CPlayer::CalcEquipAttackDelay()
{
  unsigned int totalDelay = 0;
  for (int j = 0; j < 5; ++j)
  {
    _STORAGE_LIST::_db_con *equip = &m_Param.m_dbEquip.m_pStorageList[j];
    if (equip->m_bLoad)
    {
      _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblItemData[j], equip->m_wItemIndex);
      totalDelay += *reinterpret_cast<unsigned int *>(&record[5].m_strCode[20]);
    }
  }

  _STORAGE_LIST::_db_con *equip6 = m_Param.m_dbEquip.m_pStorageList + 6;
  if (equip6->m_bLoad)
  {
    _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblItemData[6], equip6->m_wItemIndex);
    totalDelay += *reinterpret_cast<unsigned int *>(&record[9].m_strCode[52]);
  }

  _STORAGE_LIST::_db_con *equip5 = m_Param.m_dbEquip.m_pStorageList + 5;
  if (equip5->m_bLoad)
  {
    _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblItemData[5], equip5->m_wItemIndex);
    totalDelay += *reinterpret_cast<unsigned int *>(&record[5].m_strCode[20]);
  }

  if (IsSiegeMode())
  {
    _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblItemData[27], m_pSiegeItem->m_wItemIndex);
    totalDelay += *reinterpret_cast<unsigned int *>(&record[5].m_strCode[20]);
  }

  return totalDelay;
}

__int64 CPlayer::GetMasteryCumAfterAttack(int nDstLv)
{
  int diff = nDstLv - static_cast<int>(GetLevel());
  if (diff <= 5)
  {
    if (diff <= 0)
    {
      diff = 1;
    }
  }
  else
  {
    diff = 5;
  }

  int limit = nDstLv / 10;
  if (nDstLv % 10)
  {
    ++limit;
  }

  if (diff > limit)
  {
    return static_cast<unsigned int>(limit);
  }
  return static_cast<unsigned int>(diff);
}

bool CPlayer::IsPassMasteryLimitLvDiff(int iDstLevel)
{
  const int diff = static_cast<int>(GetLevel()) - iDstLevel;
  return diff <= 3 && diff >= -10;
}

void CPlayer::SetBattleMode(bool bAttack)
{
  if (bAttack)
  {
    m_byBattleMode = 1;
  }
  else
  {
    m_byBattleMode = 2;
  }
  m_dwBattleTime = timeGetTime();
}

__int64 CPlayer::AttackableHeight()
{
  if (m_pmWpn.byWpClass)
  {
    return 350;
  }
  return 50;
}

__int64 CPlayer::GetAttackDP()
{
  if (IsRidingUnit())
  {
    _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblUnitPart[3], m_pUsingUnit->byPart[3]);
    if (record)
    {
      return *reinterpret_cast<unsigned int *>(&record[4].m_strCode[40]);
    }
  }
  else
  {
    _STORAGE_LIST::_db_con *equip = m_Param.m_dbEquip.m_pStorageList + 6;
    if (equip->m_bLoad)
    {
      _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblItemData[6], equip->m_wItemIndex);
      if (record)
      {
        return *reinterpret_cast<unsigned int *>(&record[9].m_strCode[44]);
      }
    }
  }
  return 0;
}

__int64 CPlayer::GetAttackLevel()
{
  if (IsRidingUnit())
  {
    _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblUnitPart[3], m_pUsingUnit->byPart[3]);
    if (record)
    {
      return *reinterpret_cast<unsigned int *>(&record[4].m_strCode[12]);
    }
  }
  else
  {
    _STORAGE_LIST::_db_con *equip = m_Param.m_dbEquip.m_pStorageList + 6;
    if (equip->m_bLoad)
    {
      _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblItemData[6], equip->m_wItemIndex);
      if (record)
      {
        return *reinterpret_cast<unsigned int *>(&record[8].m_strCode[8]);
      }
    }
  }
  return 1;
}

float CPlayer::GetAttackRange()
{
  return static_cast<float>(m_pmWpn.wGaAttRange);
}

__int64 CPlayer::GetGenAttackProb(CCharacter *pDst, int nPart, bool bBackAttack)
{
  float masteryPerMast = 0.0f;
  if (m_pmWpn.byWpType == 255 || m_pmWpn.byWpType == 9)
  {
    masteryPerMast = 10.0f;
  }
  else
  {
    masteryPerMast = static_cast<float>(_MASTERY_PARAM::GetMasteryPerMast(&m_pmMst, 0, m_pmWpn.byWpClass));
  }

  float baseValue = FLOAT_1_0;
  if (IsRidingUnit())
  {
    _base_fld *record = CRecordData::GetRecord(
      &g_Main.m_tblUnitPart[m_byUsingWeaponPart],
      m_pUsingUnit->byPart[m_byUsingWeaponPart]);
    if (record)
    {
      baseValue = static_cast<float>(static_cast<int>(record[5].m_dwIndex));
    }
    _base_fld *baseRecord = CRecordData::GetRecord(g_Main.m_tblUnitPart, m_pUsingUnit->byPart[0]);
    if (baseRecord)
    {
      baseValue = baseValue + static_cast<float>(static_cast<int>(baseRecord[5].m_dwIndex));
    }
  }
  else
  {
    const int level = static_cast<int>(CPlayerDB::GetLevel(&m_Param));
    baseValue = static_cast<float>(level) + masteryPerMast;
  }

  float defSkill = 0.0f;
  if (pDst->m_ObjID.m_byID)
  {
    if (pDst->m_ObjID.m_byID == 1)
    {
      defSkill = static_cast<float>(pDst->GetDefSkill(pDst, bBackAttack));
    }
  }
  else
  {
    int scale = 1;
    if (BYTE2(pDst[1].m_fCurPos[2]))
    {
      scale = 2;
    }
    const int dstLevel = static_cast<int>(CPlayerDB::GetLevel(reinterpret_cast<CPlayerDB *>(&pDst[1].m_fOldPos[2])));
    const int extra = scale * dstLevel;
    defSkill = static_cast<float>(static_cast<int>(pDst->GetDefSkill(pDst, bBackAttack)) + extra);
  }

  int attackProb = static_cast<int>((baseValue - defSkill) / 4.0f) + 95;
  const float partRates[5] = {0.23f, 0.22f, 0.18000001f, 0.17f, 0.2f};
  const float partMul = 5.0f * partRates[nPart];
  attackProb = static_cast<int>(static_cast<float>(attackProb) * partMul);

  int effPlus = 0;
  if (m_pmWpn.byWpType == 7)
  {
    effPlus = static_cast<int>(_effect_parameter::GetEff_Plus(&m_EP, 2));
  }
  else
  {
    effPlus = static_cast<int>(_effect_parameter::GetEff_Plus(&m_EP, m_pmWpn.byWpClass));
  }
  const int avoidRate = static_cast<int>(pDst->GetAvoidRate(pDst));
  const float effPlus2 = _effect_parameter::GetEff_Plus(&m_EP, 40);
  effPlus = static_cast<int>(static_cast<float>(effPlus) + effPlus2);
  attackProb += effPlus - avoidRate;

  if (attackProb <= 5)
  {
    attackProb = 5;
  }
  if (attackProb >= 95)
  {
    return 95;
  }
  return static_cast<unsigned int>(attackProb);
}

bool CPlayer::IsBeAttackedAble(bool bFirst)
{
  if (!m_bLive || m_bObserver || m_bCorpse)
  {
    return false;
  }
  if (_effect_parameter::GetEff_State(&m_EP, 20))
  {
    return false;
  }
  if (_effect_parameter::GetEff_State(&m_EP, 28))
  {
    return false;
  }
  return !bFirst || !_effect_parameter::GetEff_State(&m_EP, 21);
}

void CPlayer::SetAttackPart(int nAttactPart)
{
  m_nLastBeatenPart = nAttactPart;
}

char CPlayer::dev_trap_attack_grade(int nPoint)
{
  m_nTrapMaxAttackPnt = nPoint;
  for (int j = 0; j < 20; ++j)
  {
    if (_TRAP_PARAM::_param::isLoad(&m_pmTrp.m_Item[j]))
    {
      m_pmTrp.m_Item[j].pItem->m_nTrapMaxAttackPnt = nPoint;
    }
  }
  return 1;
}

char CPlayer::mgr_MaxAttackPoint(int nMax)
{
  if (m_nMaxAttackPnt == nMax)
  {
    return 0;
  }
  m_nMaxAttackPnt = nMax;
  return 1;
}

char CPlayer::mgr_set_animus_attack_point(int nPoint)
{
  if (!m_pUserDB)
  {
    return 0;
  }
  if (m_nAnimusAttackPnt == nPoint)
  {
    return 0;
  }
  m_nAnimusAttackPnt = nPoint;
  if (m_pRecalledAnimusChar)
  {
    m_pRecalledAnimusChar->m_nMaxAttackPnt = nPoint;
  }
  return 1;
}

void CPlayer::SendMsg_TakeNewResult(char byErrCode, _STORAGE_LIST::_db_con *pItem)
{
  _itembox_take_new_result_zocl msg{};
  msg.sErrorCode = byErrCode;
  if (!byErrCode)
  {
    msg.byItemTableCode = pItem->m_byTableCode;
    msg.wItemIndex = pItem->m_wItemIndex;
    msg.dwCurDurPoint = pItem->m_dwDur;
    msg.wItemSerial = pItem->m_wSerial;
    msg.dwUptInfo = pItem->m_dwLv;
    msg.byCsMethod = pItem->m_byCsMethod;
    msg.dwT = pItem->m_dwT;
  }

  unsigned __int8 type[2]{7, 3};
  const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg.sErrorCode),
    len);
}

void CPlayer::SendMsg_TakeAddResult(char byErrCode, _STORAGE_LIST::_db_con *pItem)
{
  _itembox_take_add_result_zocl msg{};
  msg.sErrorCode = byErrCode;
  if (!byErrCode)
  {
    msg.wItemSerial = pItem->m_wSerial;
    msg.byAmount = static_cast<unsigned __int8>(pItem->m_dwDur);
  }

  unsigned __int8 type[2]{7, 4};
  const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg.sErrorCode),
    len);
}

void CPlayer::SendMsg_ThrowStorageResult(char byErrCode)
{
  char msg[1]{};
  msg[0] = byErrCode;

  unsigned __int8 type[2]{7, 6};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_UsePotionResult(char byErrCode, unsigned __int16 wSerial, unsigned __int8 byLeftNum)
{
  char msg[10]{};
  msg[0] = byErrCode;
  *reinterpret_cast<unsigned __int16 *>(msg + 1) = wSerial;
  *reinterpret_cast<short *>(msg + 3) = static_cast<short>(CPlayerDB::GetHP(&m_Param));
  *reinterpret_cast<short *>(msg + 5) = static_cast<short>(CPlayerDB::GetFP(&m_Param));
  *reinterpret_cast<short *>(msg + 7) = static_cast<short>(CPlayerDB::GetSP(&m_Param));
  msg[9] = static_cast<char>(byLeftNum);

  unsigned __int8 type[2]{7, 8};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 0xAu);
}

void CPlayer::SendMsg_EquipPartResult(char byErrCode)
{
  char msg[1]{};
  msg[0] = byErrCode;

  unsigned __int8 type[2]{7, 10};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_EmbellishResult(char byErrCode)
{
  char msg[1]{};
  msg[0] = byErrCode;

  unsigned __int8 type[2]{7, 12};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_OffPartResult(char byErrCode)
{
  char msg[1]{};
  msg[0] = byErrCode;

  unsigned __int8 type[2]{7, 14};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_MakeItemResult(char byErrCode)
{
  char msg[1]{};
  msg[0] = byErrCode;

  unsigned __int8 type[2]{7, 16};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_ItemUpgrade(char byErrCode)
{
  char msg[1]{};
  msg[0] = byErrCode;

  unsigned __int8 type[2]{7, 18};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_ItemDowngrade(char byErrCode)
{
  char msg[1]{};
  msg[0] = byErrCode;

  unsigned __int8 type[2]{7, 20};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_AddBagResult(char byErrCode)
{
  char msg[1]{};
  msg[0] = byErrCode;

  unsigned __int8 type[2]{7, 22};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_CombineItemResult(char byErrCode, unsigned int dwFee, _STORAGE_LIST::_db_con *pNewItem)
{
  struct _combine_item_result_msg
  {
    char byErrCode;
    char pad0[3];
    unsigned int dwFee;
    unsigned int dwLeftDalant;
    unsigned __int8 byTableCode;
    unsigned __int8 pad1;
    unsigned __int16 wItemIndex;
    unsigned int dwDur;
    unsigned __int16 wSerial;
  } msg{};

  msg.byErrCode = byErrCode;
  msg.dwFee = dwFee;
  msg.dwLeftDalant = CPlayerDB::GetDalant(&m_Param);
  msg.byTableCode = pNewItem->m_byTableCode;
  msg.wItemIndex = pNewItem->m_wItemIndex;
  msg.dwDur = pNewItem->m_dwDur;
  msg.wSerial = pNewItem->m_wSerial;

  unsigned __int8 type[2]{7, 29};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 0x16u);
}

void CPlayer::SendMsg_CombineLendItemResult(
  unsigned __int8 byErrCode,
  unsigned int dwFee,
  _STORAGE_LIST::_db_con *pNewItem)
{
  _combine_lend_item_result_zocl msg{};
  msg.byErrorCode = byErrCode;
  if (!byErrCode)
  {
    msg.dwFee = dwFee;
    msg.dwLeftDalant = CPlayerDB::GetDalant(&m_Param);
    msg.Item.byTblCode = pNewItem->m_byTableCode;
    msg.Item.wItemIdx = pNewItem->m_wItemIndex;
    msg.Item.dwDur = pNewItem->m_dwDur;
    msg.Item.dwUp = pNewItem->m_dwLv;
    msg.Item.dwItemSerial = pNewItem->m_wSerial;
    msg.Item.byCsMethod = pNewItem->m_byCsMethod;
    msg.Item.dwT = pNewItem->m_dwT;
  }

  unsigned __int8 type[2]{7, 61};
  const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
}

void CPlayer::SendMsg_ExchangeItemResult(char byErrCode, _STORAGE_LIST::_db_con *pNewItem)
{
  char msg[14]{};
  msg[0] = byErrCode;
  msg[1] = static_cast<char>(pNewItem->m_byTableCode);
  *reinterpret_cast<unsigned __int16 *>(msg + 2) = pNewItem->m_wItemIndex;
  *reinterpret_cast<unsigned int *>(msg + 4) = pNewItem->m_dwDur;
  *reinterpret_cast<unsigned __int16 *>(msg + 8) = pNewItem->m_wSerial;
  memcpy_0(msg + 10, &pNewItem->m_dwLv, sizeof(pNewItem->m_dwLv));

  unsigned __int8 type[2]{7, 31};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 0xEu);
}

void CPlayer::SendMsg_CombineItemExResult(_combine_ex_item_result_zocl *pSend)
{
  unsigned __int8 type[2]{7, 33};
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(pSend),
    0x134u);
}

void CPlayer::SendMsg_CombineItemExAcceptResult(_combine_ex_item_accept_result_zocl *pSend)
{
  unsigned __int8 type[2]{7, 36};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(pSend), 1u);
}

void CPlayer::SendMsg_UseRadarResult(char byErrCode, unsigned __int16 wSerial, unsigned int dwDelay)
{
  char msg[7]{};
  msg[0] = byErrCode;
  *reinterpret_cast<unsigned __int16 *>(msg + 1) = wSerial;
  *reinterpret_cast<unsigned int *>(msg + 3) = dwDelay;

  unsigned __int8 type[2]{7, 50};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 7u);
}

void CPlayer::SendMsg_RadarCharSearchResult()
{
  _radar_char_list_result_zocl msg{};
  msg.bEnd = m_pUserDB->m_RadarItemMgr.m_bPlayerEnd && m_pUserDB->m_RadarItemMgr.m_bMonEnd;

  const _detected_char_list &radarResult = m_pUserDB->m_RadarItemMgr.m_RadarResult;
  msg.byListNum = static_cast<unsigned __int8>(radarResult.nSize);
  for (int index = 0; index < radarResult.nSize; ++index)
  {
    msg.CharInfo[index].m_byCharType = radarResult.CharInfoList[index].m_byCharType;
    memcpy_0(
      msg.CharInfo[index].m_fPos,
      radarResult.CharInfoList[index].m_fPos,
      sizeof(msg.CharInfo[index].m_fPos));
  }

  unsigned __int8 type[2]{7, 52};
  const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg.bEnd),
    len);
}

void CPlayer::SendMsg_FanfareItem(unsigned __int8 byGetType, _STORAGE_LIST::_db_con *pItem, CItemBox *pItemBox)
{
  if (!CRecordData::GetRecord(&g_Main.m_tblItemData[pItem->m_byTableCode], pItem->m_wItemIndex))
  {
    return;
  }

  unsigned __int8 itemGrade = 0;
  unsigned __int8 itemUpgedLv = 0;
  if ((byGetType == 1 || byGetType == 0)
      && GetDefItemUpgSocketNum(pItem->m_byTableCode, pItem->m_wItemIndex))
  {
    itemUpgedLv = GetItemUpgedLv(pItem->m_dwLv);
  }
  if (byGetType)
  {
    itemGrade = GetItemGrade(pItem->m_byTableCode, pItem->m_wItemIndex);
  }

  _item_fanfare_zocl msg{};
  unsigned __int8 type[2]{7, 58};
  msg.byTakeType = byGetType;

  bool allSend = false;
  auto &subTypeByte = *reinterpret_cast<unsigned __int8 *>(&msg.bySubType);
  if (itemUpgedLv < 5u)
  {
    if (itemGrade < 3u)
    {
      if (!itemUpgedLv || itemUpgedLv > 5u)
      {
        return;
      }
      subTypeByte &= 0xFCu;
      subTypeByte = static_cast<unsigned __int8>((4 * (itemUpgedLv & 0x3Fu)) | (subTypeByte & 3u));
      allSend = false;
    }
    else
    {
      subTypeByte = static_cast<unsigned __int8>((subTypeByte & 0xFCu) | 1u);
      subTypeByte = static_cast<unsigned __int8>((4 * (itemGrade & 0x3Fu)) | (subTypeByte & 3u));
      allSend = true;
    }
  }
  else
  {
    subTypeByte &= 0xFCu;
    subTypeByte = static_cast<unsigned __int8>((4 * (itemUpgedLv & 0x3Fu)) | (subTypeByte & 3u));
    allSend = true;
  }

  msg.byTableCode = pItem->m_byTableCode;
  msg.wItemIndex = pItem->m_wItemIndex;
  msg.dwCharacterSerial = m_dwObjSerial;
  msg.wMonsterIndex = pItemBox ? pItemBox->m_wMonRecIndex : static_cast<unsigned __int16>(-1);
  msg.wMapIndex = static_cast<unsigned __int16>(m_pCurMap->m_nMapIndex);
  const char *charName = CPlayerDB::GetCharNameW(&m_Param);
  strcpy_0(msg.strCharacterName, charName);
  msg.bAllSend = allSend;

  if (m_byUserDgr < 3u)
  {
    if (allSend)
    {
      for (unsigned int index = 0; index < MAX_PLAYER; ++index)
      {
        CPlayer *targetPlayer = &g_Player[index];
        if (targetPlayer->m_bLive)
        {
          const int targetRace = CPlayerDB::GetRaceCode(&targetPlayer->m_Param);
          const int myRace = CPlayerDB::GetRaceCode(&m_Param);
          if (targetRace == myRace)
          {
            g_Network.m_pProcess[0]->LoadSendMsg(
              targetPlayer->m_ObjID.m_wIndex,
              type,
              reinterpret_cast<char *>(&msg),
              0x1Fu);
          }
        }
      }
    }
    else
    {
      CircleReport(type, reinterpret_cast<char *>(&msg), 31, true);
    }
  }
  else
  {
    g_Network.m_pProcess[0]->LoadSendMsg(
      m_ObjID.m_wIndex,
      type,
      reinterpret_cast<char *>(&msg),
      0x1Fu);
  }
}

void CPlayer::SendMsg_PartyLootItemInform(
  unsigned int dwTakerSerial,
  unsigned __int8 byTableCode,
  unsigned __int16 wItemIndex,
  unsigned __int8 byNum)
{
  char msg[8]{};
  *reinterpret_cast<unsigned int *>(msg) = dwTakerSerial;
  msg[4] = static_cast<char>(byTableCode);
  *reinterpret_cast<unsigned __int16 *>(msg + 5) = wItemIndex;
  msg[7] = static_cast<char>(byNum);

  unsigned __int8 type[2]{16, 30};
  CPartyPlayer **partyMembers = CPartyPlayer::GetPtrPartyMember(m_pPartyMgr);
  if (partyMembers)
  {
    for (int index = 0; index < 8 && partyMembers[index]; ++index)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(partyMembers[index]->m_wZoneIndex, type, msg, 8u);
    }
  }
}

void CPlayer::SendMsg_Notify_Get_Golden_Box(
  char byBoxType,
  unsigned int dwCharSerial,
  char *szCharName,
  _STORAGE_LIST::_db_con *pItem,
  bool bCircle)
{
  struct _golden_box_notify_msg
  {
    unsigned __int8 byBoxType;
    unsigned __int8 byTableCode;
    unsigned __int16 wItemIndex;
    unsigned __int8 byDur;
    unsigned int dwCharSerial;
    char szCharName[17];
  } msg{};

  msg.byBoxType = static_cast<unsigned __int8>(byBoxType);
  msg.byTableCode = pItem->m_byTableCode;
  msg.wItemIndex = pItem->m_wItemIndex;
  msg.byDur = static_cast<unsigned __int8>(pItem->m_dwDur);
  msg.dwCharSerial = dwCharSerial;
  strcpy_0(msg.szCharName, szCharName);

  unsigned __int8 type[2]{13, static_cast<unsigned __int8>(0x95)};
  if (bCircle)
  {
    CircleReport(type, reinterpret_cast<char *>(&msg), 26, true);
  }
  else
  {
    for (unsigned int index = 0; index < MAX_PLAYER; ++index)
    {
      CPlayer *targetPlayer = &g_Player[index];
      if (targetPlayer && targetPlayer->m_bLive)
      {
        g_Network.m_pProcess[0]->LoadSendMsg(
          targetPlayer->m_ObjID.m_wIndex,
          type,
          reinterpret_cast<char *>(&msg),
          0x1Au);
      }
    }
  }
}

void CPlayer::SendMsg_Notify_Me_Get_Golden_Box(char byBoxType, _STORAGE_LIST::_db_con *pItem)
{
  struct _golden_box_notify_msg
  {
    unsigned __int8 byBoxType;
    unsigned __int8 byTableCode;
    unsigned __int16 wItemIndex;
    unsigned __int8 byDur;
    unsigned int dwCharSerial;
    char szCharName[17];
  } msg{};

  msg.byBoxType = static_cast<unsigned __int8>(byBoxType);
  msg.byTableCode = pItem->m_byTableCode;
  msg.wItemIndex = pItem->m_wItemIndex;
  msg.byDur = static_cast<unsigned __int8>(pItem->m_dwDur);
  msg.dwCharSerial = CPlayerDB::GetCharSerial(&m_Param);
  const char *charName = CPlayerDB::GetCharNameA(&m_Param);
  strcpy_0(msg.szCharName, charName);

  unsigned __int8 type[2]{13, static_cast<unsigned __int8>(0x95)};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 0x1Au);
}

void CPlayer::SendMsg_ExchangeLendItemResult(unsigned __int8 byErrCode, _STORAGE_LIST::_db_con *pNewItem)
{
  _exchange_lend_item_result_zocl msg{};
  msg.byErrorCode = byErrCode;
  if (!byErrCode)
  {
    msg.Item.byTblCode = pNewItem->m_byTableCode;
    msg.Item.wItemIdx = pNewItem->m_wItemIndex;
    msg.Item.dwDur = pNewItem->m_dwDur;
    msg.Item.dwUp = pNewItem->m_dwLv;
    msg.Item.dwItemSerial = pNewItem->m_wSerial;
    msg.Item.byCsMethod = pNewItem->m_byCsMethod;
    msg.Item.dwT = pNewItem->m_dwT;
  }

  unsigned __int8 type[2]{7, 62};
  const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    len);
}

void CPlayer::SendMsg_TalikCrystalExchangeResult(
  unsigned __int8 byRet,
  unsigned __int8 byExchangeNum,
  _STORAGE_LIST::_db_con *pNewItem)
{
  (void)byRet;
  (void)byExchangeNum;
  (void)pNewItem;
  // this is not a stub
}

void CPlayer::SendMsg_CharacterRenameCashResult(char bChange, char byErrCode)
{
  char msg[2]{};
  msg[0] = bChange;
  msg[1] = byErrCode;

  unsigned __int8 type[2]{7, 67};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 2u);
}

void CPlayer::SendMsg_SetItemCheckResult(char byResult, unsigned int dwSetItem, unsigned __int8 bySetEffectNum)
{
  char msg[6]{};
  msg[0] = byResult;
  memcpy_0(msg + 1, &dwSetItem, sizeof(dwSetItem));
  msg[5] = static_cast<char>(bySetEffectNum);

  unsigned __int8 type[2]{7, 46};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 6u);
}

void CPlayer::SendMsg_NPCLinkItemCheckResult(unsigned __int8 byResCode, _STORAGE_POS_INDIV *pStorage)
{
  _npclink_check_item_result_zocl msg{};
  memset_0(&msg, 0, sizeof(msg));
  msg.byRet = byResCode;
  msg.storage = *pStorage;

  unsigned __int8 type[2]{7, 60};
  const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
}

void CPlayer::pc_TakeGroundingItem(CItemBox *pBox, unsigned __int16 wAddSerial)
{
  unsigned __int8 errCode = 0;
  _STORAGE_LIST::_db_con item{};
  _STORAGE_LIST::_db_con *addTarget = nullptr;
  const unsigned int now = timeGetTime();
  int saveItem = 1;
  _STORAGE_LIST::_db_con *addedItem = nullptr;

  if (TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, m_id.wIndex) == 99)
  {
    errCode = 16;
    goto RESULT;
  }
  if (now - m_dwLastTakeItemTime < 0x190)
  {
    errCode = 9;
    goto RESULT;
  }
  if (!pBox->m_bLive)
  {
    errCode = 4;
    goto RESULT;
  }
  if (!CItemBox::IsTakeRight(pBox, this))
  {
    errCode = 5;
    goto RESULT;
  }
  if (GetSqrt(pBox->m_fCurPos, m_fCurPos) > 100.0)
  {
    errCode = 6;
    goto RESULT;
  }

  memcpy_0(&item, &pBox->m_Item, sizeof(item));
  saveItem = IsSaveItem(item.m_byTableCode);
  if (item.m_byTableCode == 19)
  {
    errCode = 8;
    goto RESULT;
  }
  if (_effect_parameter::GetEff_State(&m_EP, 20))
  {
    errCode = 13;
    goto RESULT;
  }
  if (_effect_parameter::GetEff_State(&m_EP, 28))
  {
    errCode = 13;
    goto RESULT;
  }
  if (saveItem)
  {
    if (wAddSerial == 0xFFFF)
    {
      if (_STORAGE_LIST::GetIndexEmptyCon(&m_Param.m_dbInven) == 0xFF)
      {
        errCode = 1;
      }
    }
    else
    {
      if (!IsOverLapItem(item.m_byTableCode))
      {
        errCode = 7;
        goto RESULT;
      }
      addTarget = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, wAddSerial);
      if (!addTarget)
      {
        SendMsg_AdjustAmountInform(0, wAddSerial, 0);
        errCode = 2;
        goto RESULT;
      }
      if (addTarget->m_byCsMethod && item.m_byCsMethod && addTarget->m_dwT != item.m_dwT)
      {
        if (_STORAGE_LIST::GetIndexEmptyCon(&m_Param.m_dbInven) == 0xFF)
        {
          errCode = 1;
          goto RESULT;
        }
        wAddSerial = 0xFFFF;
      }
      if (addTarget->m_bLock)
      {
        errCode = 12;
      }
      else if (addTarget->m_byTableCode == item.m_byTableCode && addTarget->m_wItemIndex == item.m_wItemIndex)
      {
        if (item.m_dwDur + addTarget->m_dwDur <= 0x63)
        {
          if (addTarget->m_dwT != item.m_dwT)
          {
            errCode = 14;
          }
        }
        else
        {
          SendMsg_AdjustAmountInform(0, wAddSerial, addTarget->m_dwDur);
          errCode = 3;
        }
      }
      else
      {
        errCode = 2;
      }
    }
  }

RESULT:
  if (!errCode)
  {
    m_dwLastTakeItemTime = now;
    if (saveItem)
    {
      if (wAddSerial == 0xFFFF)
      {
        item.m_wSerial = CPlayerDB::GetNewItemSerial(&m_Param);
        addedItem = Emb_AddStorage(0, &item, false, true);
        if (!addedItem)
        {
          SendMsg_TakeNewResult(0xFFu, nullptr);
          return;
        }
        if (pBox->m_byThrowerID == 1)
        {
          SendMsg_FanfareItem(3u, &item, pBox);
        }
      }
      else
      {
        const bool protectItem = IsProtectItem(item.m_byTableCode) != 0;
        Emb_AlterDurPoint(0, addTarget->m_byStorageIndex, static_cast<int>(item.m_dwDur), protectItem, 0);
      }
      if (m_pUserDB && m_bCntEnable && item.m_byTableCode == 20)
      {
        _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblItemData[item.m_byTableCode], item.m_wItemIndex);
        if (!record)
        {
          return;
        }
        if (!strcmp_0(record->m_strCode, "iyhol01"))
        {
          const unsigned __int8 numOfTime = CHolyStoneSystem::GetNumOfTime(&g_HolySys);
          CUserDB::Update_UserGetScaner(m_pUserDB, 1u, numOfTime);
        }
      }
      CGoldenBoxItemMgr *goldenMgr = CGoldenBoxItemMgr::Instance();
      if (CGoldenBoxItemMgr::Get_Event_Status(goldenMgr) == 2 && m_pUserDB && item.m_byTableCode == 31)
      {
        _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblItemData[item.m_byTableCode], item.m_wItemIndex);
        if (!record)
        {
          return;
        }
        if (!strcmp_0(record->m_strCode, "bxgol03") && pBox->m_dwThrowerCharSerial == static_cast<unsigned int>(-1))
        {
          char *charName = CPlayerDB::GetCharNameA(&m_Param);
          const unsigned int charSerial = CPlayerDB::GetCharSerial(&m_Param);
          SendMsg_Notify_Get_Golden_Box(3u, charSerial, charName, &item, false);
        }
      }
      bool logHistory = true;
      if (!IsProtectItem(item.m_byTableCode) && pBox->m_byThrowerID == 1)
      {
        logHistory = false;
      }
      if (logHistory)
      {
        if (pBox->m_wMonRecIndex != 0xFFFF && pBox->m_bBossMob)
        {
          _base_fld *monRecord = CRecordData::GetRecord(&g_Main.m_tblMonster, pBox->m_wMonRecIndex);
          _base_fld *itemRecord = CRecordData::GetRecord(&g_Main.m_tblItemData[item.m_byTableCode], item.m_wItemIndex);
          const unsigned int nowSec = timeGetTime() / 0x3E8;
          char *charName = CPlayerDB::GetCharNameA(&m_Param);
          CLogFile::Write(
            &CMonster::s_logTrace_Boss_Looting,
            "\t Mob: %s Item: %s => %s ( %d ), Sec: %d",
            monRecord->m_strCode,
            itemRecord->m_strCode,
            charName,
            m_dwObjSerial,
            nowSec);
        }
        CMgrAvatorItemHistory::take_ground_item(
          &CPlayer::s_MgrItemHistory,
          m_ObjID.m_wIndex,
          pBox->m_byCreateCode,
          &item,
          pBox->m_aszThrowerName,
          pBox->m_dwThrowerCharSerial,
          pBox->m_szThrowerID,
          pBox->m_wMonRecIndex,
          m_pCurMap->m_pMapSet->m_strCode,
          m_fCurPos,
          m_szItemHistoryFileName);
        if (pBox->m_dwThrowerCharSerial != static_cast<unsigned int>(-1)
            && pBox->m_dwThrowerCharSerial != m_dwObjSerial)
        {
          char *takerId = m_pUserDB->m_szAccountID;
          char *charName = CPlayerDB::GetCharNameA(&m_Param);
          CMgrAvatorItemHistory::trans_ground_item(
            &CPlayer::s_MgrItemHistory,
            &item,
            charName,
            m_dwObjSerial,
            takerId,
            pBox->m_szThrowerItemHistoryFileName);
        }
      }
    }
    _base_fld *itemRecord = CRecordData::GetRecord(&g_Main.m_tblItemData[item.m_byTableCode], item.m_wItemIndex);
    CQuestMgr::CheckFailLoop(&m_QuestMgr, 4, itemRecord->m_strCode);
    bool darkHoleEvent = false;
    if (pBox->m_byCreateCode == 5 && m_pDHChannel)
    {
      darkHoleEvent = CDarkHoleChannel::CheckEvent(
        m_pDHChannel,
        dh_event_take,
        item.m_byTableCode,
        item.m_wItemIndex,
        item.m_dwDur,
        pBox);
    }
    if (pBox->m_byThrowerID == 1)
    {
      if (!darkHoleEvent)
      {
        _base_fld *questItemRecord = CRecordData::GetRecord(&g_Main.m_tblItemData[item.m_byTableCode], item.m_wItemIndex);
        Emb_CheckActForQuest(4, questItemRecord->m_strCode, 1u, 0);
        if (CPartyPlayer::IsPartyMode(m_pPartyMgr))
        {
          Emb_CheckActForQuestParty(4, questItemRecord->m_strCode, 1u);
        }
        CheckMentalTakeAndUpdateLastMetalTicket(questItemRecord->m_strCode);
      }
      if (CPartyPlayer::IsPartyMode(m_pPartyMgr)
          && pBox->m_dwPartyBossSerial == m_pPartyMgr->m_pPartyBoss->m_id.dwSerial)
      {
        unsigned __int8 dur = 1;
        if (IsOverLapItem(item.m_byTableCode))
        {
          dur = static_cast<unsigned __int8>(item.m_dwDur);
        }
        SendMsg_PartyLootItemInform(m_dwObjSerial, item.m_byTableCode, item.m_wItemIndex, dur);
      }
    }
    CItemBox::Destroy(pBox);
  }

  if (wAddSerial == 0xFFFF)
  {
    if (errCode || addedItem)
    {
      SendMsg_TakeNewResult(errCode, addedItem);
    }
    else
    {
      CLogFile::Write(
        &g_Main.m_logSystemError,
        "pc_TakeGroundingItem() : Error Take New Item Result : BoxItem TableCode(%u), ItemIndex(%u)",
        item.m_byTableCode,
        item.m_wItemIndex);
      SendMsg_TakeNewResult(0xFu, addedItem);
    }
  }
  else if (errCode || addTarget)
  {
    SendMsg_TakeAddResult(errCode, addTarget);
  }
  else
  {
    CLogFile::Write(
      &g_Main.m_logSystemError,
      "pc_TakeGroundingItem() : Error Take Add Item Result : BoxItem TableCode(%u), ItemIndex(%u)",
      item.m_byTableCode,
      item.m_wItemIndex);
    SendMsg_TakeNewResult(0xFu, addTarget);
  }
}

void CPlayer::pc_ThrowStorageItem(_STORAGE_POS_INDIV *pItem)
{
  unsigned __int8 errCode = 0;
  _STORAGE_LIST *storage = m_Param.m_pStoragePtr[pItem->byStorageCode];
  _STORAGE_LIST::_db_con *srcItem = nullptr;

  if (m_pUserDB)
  {
    if (_effect_parameter::GetEff_State(&m_EP, 20))
    {
      errCode = 5;
    }
    else if (_effect_parameter::GetEff_State(&m_EP, 28))
    {
      errCode = 5;
    }
    else
    {
      srcItem = _STORAGE_LIST::GetPtrFromSerial(storage, pItem->wItemSerial);
      if (srcItem)
      {
        if (srcItem->m_bLock)
        {
          errCode = 9;
        }
        else
        {
          __int16 *billingType = &m_pUserDB->m_BillingInfo.iType;
          CNationSettingManager *nationSetting = CTSingleton<CNationSettingManager>::Instance();
          if (CNationSettingManager::IsPersonalFreeFixedAmountBillingType(nationSetting, billingType, nullptr))
          {
            errCode = 9;
          }
          else if (IsOverLapItem(srcItem->m_byTableCode)
                   && pItem->byNum > static_cast<unsigned __int64>(srcItem->m_dwDur))
          {
            SendMsg_AdjustAmountInform(pItem->byStorageCode, pItem->wItemSerial, srcItem->m_dwDur);
            errCode = 3;
          }
          else if (srcItem->m_byTableCode == 19)
          {
            errCode = 4;
          }
          else if (!IsGroundableItem(srcItem->m_byTableCode, srcItem->m_wItemIndex))
          {
            errCode = 4;
          }
        }
      }
      else
      {
        errCode = 2;
      }
    }

    if (!errCode)
    {
      _STORAGE_LIST::_db_con dropItem{};
      memcpy_0(&dropItem, srcItem, sizeof(dropItem));
      if (IsOverLapItem(srcItem->m_byTableCode))
      {
        dropItem.m_dwDur = pItem->byNum;
      }
      CItemBox *itemBox = CreateItemBox(
        &dropItem,
        nullptr,
        0xFFFFFFFF,
        0,
        this,
        1u,
        m_pCurMap,
        m_wMapLayerIndex,
        m_fCurPos,
        0);
      if (itemBox)
      {
        if (IsOverLapItem(srcItem->m_byTableCode))
        {
          Emb_AlterDurPoint(storage->m_nListCode, srcItem->m_byStorageIndex, -pItem->byNum, false, false);
        }
        else if (!Emb_DelStorage(
                   storage->m_nListCode,
                   srcItem->m_byStorageIndex,
                   0,
                   true,
                   "CPlayer::pc_ThrowStorageItem()"))
        {
          CItemBox::Destroy(itemBox);
          SendMsg_ThrowStorageResult(0xFFu);
          return;
        }
        _map_fld *mapRecord = m_pCurMap->m_pMapSet;
        CMgrAvatorItemHistory::throw_ground_item(
          &CPlayer::s_MgrItemHistory,
          m_ObjID.m_wIndex,
          &dropItem,
          mapRecord->m_strCode,
          m_fCurPos,
          m_szItemHistoryFileName);
      }
      else
      {
        errCode = 4;
      }
    }
    SendMsg_ThrowStorageResult(errCode);
  }
}

void CPlayer::pc_UsePotionItem(CPlayer *pTargetPlayer, _STORAGE_POS_INDIV *pItem)
{
  unsigned __int8 errCode = 0;
  if (pItem && !pItem->byStorageCode && pTargetPlayer)
  {
    _STORAGE_LIST *storage = m_Param.m_pStoragePtr[pItem->byStorageCode];
    _STORAGE_LIST::_db_con *useItem = nullptr;
    _PotionItem_fld *potionFld = nullptr;
    unsigned int currTime = static_cast<unsigned int>(-1);

    if (IsRidingUnit())
    {
      errCode = 7;
    }
    else
    {
      useItem = _STORAGE_LIST::GetPtrFromSerial(storage, pItem->wItemSerial);
      if (useItem)
      {
        if (useItem->m_byTableCode == 13)
        {
          if (useItem->m_bLock)
          {
            errCode = 11;
          }
          else
          {
            const int cashType = GetUsePcCashType(useItem->m_byTableCode, useItem->m_wItemIndex);
            if (!IsUsableAccountType(cashType))
            {
              SendMsg_PremiumCashItemUse(0xFFFFu);
              SendMsg_UsePotionResult(0x19u, pItem->wItemSerial, 0);
              return;
            }
            currTime = timeGetTime();
            potionFld = (_PotionItem_fld *)CRecordData::GetRecord(&g_Main.m_tblItemData[13], useItem->m_wItemIndex);
            if (potionFld)
            {
              if (potionFld->m_nUseState == 3 || CPlayerDB::GetHP(&m_Param))
              {
                const int level = CPlayerDB::GetLevel(&m_Param);
                if (level >= potionFld->m_nLevelLim)
                {
                  if (potionFld->m_nUpLevelLim == -1
                      || CPlayerDB::GetLevel(&m_Param) <= potionFld->m_nUpLevelLim)
                  {
                    errCode = CPotionMgr::UsePotion(&g_PotionMgr, this, pTargetPlayer, potionFld, currTime);
                  }
                  else
                  {
                    errCode = 25;
                  }
                }
                else
                {
                  errCode = 25;
                }
              }
              else
              {
                errCode = 6;
              }
            }
            else
            {
              const int itemIndex = useItem->m_wItemIndex;
              const int tableCode = useItem->m_byTableCode;
              char *charName = CPlayerDB::GetCharNameA(&m_Param);
              CLogFile::Write(
                &g_Main.m_logSystemError,
                "CPlayer::pc_UsePotionItem(...) : User %s(%u) Item Table(%u) Index(%u) Not Exist!",
                charName,
                m_dwObjSerial,
                tableCode,
                itemIndex);
              errCode = 48;
            }
          }
        }
        else
        {
          errCode = 3;
        }
      }
      else
      {
        SendMsg_AdjustAmountInform(pItem->byStorageCode, pItem->wItemSerial, 0);
        errCode = 2;
      }
    }

    unsigned __int8 leftNum = 0;
    if (!errCode)
    {
      leftNum = static_cast<unsigned __int8>(
        Emb_AlterDurPoint(storage->m_nListCode, useItem->m_byStorageIndex, -1, false, false));
      if (potionFld->m_bIsCash == 1)
      {
        CMgrAvatorItemHistory::cash_item_use(
          &CPlayer::s_MgrItemHistory,
          m_ObjID.m_wIndex,
          useItem,
          m_szItemHistoryFileName);
      }
      _base_fld *effectRecord = CRecordData::GetRecord(&g_PotionMgr.m_tblPotionEffectData, potionFld->m_strEffCode);
      if (effectRecord && (effectRecord[13].m_dwIndex == 50 || effectRecord[13].m_dwIndex == 51))
      {
        CMgrAvatorItemHistory::consume_del_item(
          &CPlayer::s_MgrItemHistory,
          m_ObjID.m_wIndex,
          useItem,
          m_szItemHistoryFileName);
      }
      SetPotionActDelay(potionFld->m_nDelayType, currTime, static_cast<int>(potionFld->m_fActDelay));
    }
    SendMsg_UsePotionResult(errCode, pItem->wItemSerial, leftNum);
  }
}

void CPlayer::pc_EquipPart(_STORAGE_POS_INDIV *pItem)
{
  unsigned __int8 errCode = 0;
  _STORAGE_LIST *inventory = &m_Param.m_dbInven;
  _STORAGE_LIST::_db_con *fixingItem = nullptr;
  _STORAGE_LIST::_db_con *equipSlot = nullptr;
  _base_fld *record = nullptr;

  if (_effect_parameter::GetEff_State(&m_EP, 20))
  {
    errCode = 8;
  }
  else if (_effect_parameter::GetEff_State(&m_EP, 28))
  {
    errCode = 8;
  }
  else
  {
    fixingItem = _STORAGE_LIST::GetPtrFromSerial(inventory, pItem->wItemSerial);
    if (fixingItem)
    {
      record = CRecordData::GetRecord(&g_Main.m_tblItemData[fixingItem->m_byTableCode], fixingItem->m_wItemIndex);
      if (fixingItem->m_byTableCode < 8u)
      {
        if (fixingItem->m_bLock)
        {
          errCode = 10;
        }
        else
        {
          equipSlot = &m_Param.m_dbEquip.m_pStorageList[fixingItem->m_byTableCode];
          if (equipSlot->m_bLoad && equipSlot->m_bLock)
          {
            errCode = 10;
          }
          else
          {
            const unsigned __int8 itemGrade = GetItemEquipGrade(fixingItem->m_byTableCode, fixingItem->m_wItemIndex);
            if (IsEquipAbleGrade(itemGrade))
            {
              if (!_check_equip_part(fixingItem))
              {
                errCode = 7;
              }
            }
            else
            {
              errCode = 9;
            }
          }
        }
      }
      else
      {
        errCode = 3;
      }
    }
    else
    {
      errCode = 2;
    }
  }

  if (!errCode)
  {
    _STORAGE_LIST::_db_con invenItem{};
    memcpy_0(&invenItem, fixingItem, sizeof(invenItem));
    if (!Emb_DelStorage(
          inventory->m_nListCode,
          fixingItem->m_byStorageIndex,
          0,
          false,
          "CPlayer::pc_EquipPart() -- 0"))
    {
      SendMsg_EquipPartResult(0xFFu);
      return;
    }
    if (equipSlot->m_bLoad)
    {
      _STORAGE_LIST::_db_con equipItem{};
      memcpy_0(&equipItem, equipSlot, sizeof(equipItem));
      if (!Emb_AddStorage(inventory->m_nListCode, &equipItem, true, false))
      {
LABEL_EQUIP_FAIL:
        Emb_AddStorage(inventory->m_nListCode, &invenItem, true, false);
        SendMsg_EquipPartResult(0xFFu);
        return;
      }
      if (!Emb_DelStorage(1u, equipSlot->m_byStorageIndex, true, false, "CPlayer::pc_EquipPart() -- 1"))
      {
        Emb_DelStorage(inventory->m_nListCode, equipSlot->m_byStorageIndex, true, false, nullptr);
        Emb_AddStorage(inventory->m_nListCode, &invenItem, true, false);
        SendMsg_EquipPartResult(0xFFu);
        return;
      }
    }
    if (!Emb_AddStorage(1u, &invenItem, true, false))
    {
      if (equipSlot->m_bLoad)
      {
        Emb_DelStorage(inventory->m_nListCode, equipSlot->m_byStorageIndex, true, false, nullptr);
      }
      goto LABEL_EQUIP_FAIL;
    }
    Emb_EquipLink();
  }
  SendMsg_EquipPartResult(errCode);
}

void CPlayer::pc_EmbellishPart(_STORAGE_POS_INDIV *pItem, unsigned __int16 wChangeSerial)
{
  unsigned __int8 errCode = 0;
  _STORAGE_LIST *inventory = &m_Param.m_dbInven;
  _STORAGE_LIST *embellish = &m_Param.m_dbEmbellish;
  _STORAGE_LIST::_db_con *fixingItem = nullptr;
  _STORAGE_LIST::_db_con *embelItem = nullptr;
  int sameTypeCount = 0;

  if (_effect_parameter::GetEff_State(&m_EP, 20))
  {
    errCode = 8;
    goto RESULT;
  }
  if (_effect_parameter::GetEff_State(&m_EP, 28))
  {
    errCode = 8;
    goto RESULT;
  }
  fixingItem = _STORAGE_LIST::GetPtrFromSerial(inventory, pItem->wItemSerial);
  if (!fixingItem)
  {
    errCode = 2;
    goto RESULT;
  }
  if (fixingItem->m_byTableCode != 8 && fixingItem->m_byTableCode != 9 && fixingItem->m_byTableCode != 10)
  {
    errCode = 3;
    goto RESULT;
  }
  if (!_check_embel_part(fixingItem))
  {
    errCode = 4;
    goto RESULT;
  }
  if (wChangeSerial != 0xFFFF)
  {
    embelItem = _STORAGE_LIST::GetPtrFromSerial(embellish, wChangeSerial);
    if (!embelItem)
    {
      errCode = 2;
      goto RESULT;
    }
    if (embelItem->m_byTableCode != fixingItem->m_byTableCode)
    {
      errCode = 6;
      goto RESULT;
    }
  }
  if (fixingItem->m_bLock)
  {
    errCode = 10;
    goto RESULT;
  }
  if (embelItem && embelItem->m_bLock)
  {
    errCode = 10;
    goto RESULT;
  }
  if (wChangeSerial == 0xFFFF && _STORAGE_LIST::GetIndexEmptyCon(embellish) == 0xFF)
  {
    errCode = 5;
    goto RESULT;
  }
  for (int index = 0; index < 7; ++index)
  {
    if (embellish->m_pStorageList[index].m_bLoad
        && embellish->m_pStorageList[index].m_byTableCode == fixingItem->m_byTableCode)
    {
      ++sameTypeCount;
    }
  }
  if (fixingItem->m_byTableCode == 10)
  {
    if ((wChangeSerial == 0xFFFF && sameTypeCount > 2)
        || (wChangeSerial != 0xFFFF && sameTypeCount > 3))
    {
      errCode = 5;
      goto RESULT;
    }
  }
  else if ((wChangeSerial == 0xFFFF && sameTypeCount > 1)
           || (wChangeSerial != 0xFFFF && sameTypeCount > 2))
  {
    errCode = 5;
    goto RESULT;
  }
  if (fixingItem->m_byTableCode == 10)
  {
    const int cashType = GetUsePcCashType(fixingItem->m_byTableCode, fixingItem->m_wItemIndex);
    if (!IsUsableAccountType(cashType))
    {
      SendMsg_PremiumCashItemUse(0xFFFFu);
      SendMsg_EmbellishResult(4u);
      return;
    }
  }

RESULT:
  if (!errCode)
  {
    _STORAGE_LIST::_db_con invenItem{};
    memcpy_0(&invenItem, fixingItem, sizeof(invenItem));
    if (!Emb_DelStorage(
          inventory->m_nListCode,
          fixingItem->m_byStorageIndex,
          0,
          false,
          "CPlayer::pc_EmbellishPart() -- 0"))
    {
      SendMsg_EmbellishResult(0xFFu);
      return;
    }
    if (embelItem)
    {
      _STORAGE_LIST::_db_con oldEmbel{};
      memcpy_0(&oldEmbel, embelItem, sizeof(oldEmbel));
      if (!Emb_AddStorage(inventory->m_nListCode, &oldEmbel, true, false))
      {
LABEL_EMBEL_FAIL:
        Emb_AddStorage(inventory->m_nListCode, &invenItem, true, false);
        SendMsg_EmbellishResult(0xFFu);
        return;
      }
      if (!Emb_DelStorage(
            embellish->m_nListCode,
            embelItem->m_byStorageIndex,
            false,
            false,
            "CPlayer::pc_EmbellishPart() -- 1"))
      {
        Emb_DelStorage(
          inventory->m_nListCode,
          embelItem->m_byStorageIndex,
          false,
          false,
          "CPlayer::pc_EmbellishPart() -- 1 Fail");
        Emb_AddStorage(inventory->m_nListCode, &invenItem, true, false);
        SendMsg_EmbellishResult(0xFFu);
      }
    }
    if (!Emb_AddStorage(embellish->m_nListCode, &invenItem, true, false))
    {
      if (!embelItem)
      {
        goto LABEL_EMBEL_FAIL;
      }
      _STORAGE_LIST::_db_con restoreEmbel{};
      memcpy_0(&restoreEmbel, embelItem, sizeof(restoreEmbel));
      Emb_AddStorage(embellish->m_nListCode, &restoreEmbel, true, false);
      Emb_DelStorage(
        inventory->m_nListCode,
        embelItem->m_byStorageIndex,
        false,
        false,
        "CPlayer::pc_EmbellishPart() -- 1 Fail");
      Emb_AddStorage(inventory->m_nListCode, &invenItem, true, false);
      SendMsg_EmbellishResult(0xFFu);
    }
    Emb_EquipLink();
  }
  SendMsg_EmbellishResult(errCode);
}

void CPlayer::pc_OffPart(_STORAGE_POS_INDIV *pItem)
{
  unsigned __int8 errCode = 0;
  _STORAGE_LIST *storage = nullptr;
  _STORAGE_LIST *inventory = &m_Param.m_dbInven;
  _STORAGE_LIST::_db_con *srcItem = nullptr;

  if (_effect_parameter::GetEff_State(&m_EP, 20))
  {
    errCode = 8;
  }
  else if (_effect_parameter::GetEff_State(&m_EP, 28))
  {
    errCode = 8;
  }
  else if (pItem->byStorageCode == 1 || pItem->byStorageCode == 2)
  {
    storage = m_Param.m_pStoragePtr[pItem->byStorageCode];
    srcItem = _STORAGE_LIST::GetPtrFromSerial(storage, pItem->wItemSerial);
    if (srcItem)
    {
      if (srcItem->m_bLock)
      {
        errCode = 11;
      }
      else if (_STORAGE_LIST::GetIndexEmptyCon(inventory) == 0xFF)
      {
        errCode = 3;
      }
    }
    else
    {
      errCode = 2;
    }
  }
  else
  {
    errCode = 2;
  }

  if (!errCode)
  {
    _STORAGE_LIST::_db_con movedItem{};
    memcpy_0(&movedItem, srcItem, sizeof(movedItem));
    if (!Emb_DelStorage(storage->m_nListCode, srcItem->m_byStorageIndex, 0, false, "CPlayer::pc_OffPart()"))
    {
      SendMsg_OffPartResult(0xFFu);
      return;
    }
    if (!Emb_AddStorage(inventory->m_nListCode, &movedItem, true, false))
    {
      Emb_AddStorage(storage->m_nListCode, &movedItem, true, false);
      errCode = 11;
      SendMsg_OffPartResult(0xBu);
    }
    Emb_EquipLink();
  }
  SendMsg_OffPartResult(errCode);
}

void CPlayer::Emb_ItemUpgrade(
  unsigned __int8 byUpgradeType,
  unsigned __int8 byStorageCode,
  unsigned __int8 byStorageIndex,
  unsigned int dwGradeInfo)
{
  _STORAGE_LIST *storage = m_Param.m_pStoragePtr[byStorageCode];
  _STORAGE_LIST::_db_con *item = &storage->m_pStorageList[byStorageIndex];
  if (byStorageCode == 1)
  {
    if (GetEffectEquipCode(byStorageCode, byStorageIndex) == 1)
    {
      SetEquipEffect(item, false);
    }
    SetEffectEquipCode(byStorageCode, byStorageIndex, 0);
  }

  if (byUpgradeType == 0)
  {
    storage->GradeUp(byStorageIndex, dwGradeInfo);
  }
  else if (byUpgradeType == 1)
  {
    storage->GradeDown(byStorageIndex, dwGradeInfo);
  }
  else if (byUpgradeType == 2)
  {
    const unsigned __int8 itemUpgLimSocket = GetItemUpgLimSocket(item->m_dwLv);
    const unsigned int resetGradeInfo = GetBitAfterSetLimSocket(itemUpgLimSocket);
    storage->SetGrade(byStorageIndex, 0, resetGradeInfo);
  }

  if (byStorageCode == 1)
  {
    if (IsEffectableEquip(item))
    {
      SetEquipEffect(item, true);
      SetEffectEquipCode(byStorageCode, byStorageIndex, 1u);
    }
    else
    {
      SetEffectEquipCode(byStorageCode, byStorageIndex, 2u);
    }
    if (item->m_byTableCode == 6)
    {
      _WEAPON_PARAM::FixWeapon(&m_pmWpn, item);
    }
  }

  if (byStorageCode == 1)
  {
    CashChangeStateFlag changeFlag(0);
    UpdateVisualVer(changeFlag);
    SendMsg_EquipPartChange(item->m_byTableCode);
  }

  if (m_pUserDB)
  {
    CUserDB::Update_ItemUpgrade(m_pUserDB, byStorageCode, byStorageIndex, dwGradeInfo, 1);
  }
}

void CPlayer::pc_MakeItem(
  _STORAGE_POS_INDIV *pipMakeTool,
  unsigned __int16 wManualIndex,
  unsigned __int8 byMaterialNum,
  _STORAGE_POS_INDIV *pipMaterials)
{
  if (m_bCheat_makeitem_no_use_matrial)
  {
    byMaterialNum = 0;
  }

  unsigned __int8 errCode = 0;
  _STORAGE_LIST::_db_con *makeToolItem = nullptr;
  _base_fld *manualRecord = CRecordData::GetRecord(&g_Main.m_tblItemMakeData, wManualIndex);
  int tableCode = 0;
  _base_fld *makeItemRecord = nullptr;
  bool useRandomResult = false;

  if (!manualRecord)
  {
    errCode = 12;
    goto RESULT;
  }

  tableCode = GetItemTableCode(manualRecord->m_strCode);

  const int raceSexCode = CPlayerDB::GetRaceSexCode(&m_Param);
  if (manualRecord[1].m_strCode[raceSexCode] != '1')
  {
    errCode = 11;
    goto RESULT;
  }

  unsigned __int8 masteryType = 0;
  if (tableCode < 5)
  {
    masteryType = 1;
  }
  else if (tableCode <= 6)
  {
    masteryType = 0;
  }
  else if (tableCode == 10)
  {
    masteryType = 2;
  }
  else
  {
    masteryType = 1;
  }

  const int masteryValue = _MASTERY_PARAM::GetMasteryPerMast(&m_pmMst, 5u, masteryType);
  if (static_cast<int>(manualRecord[1].m_dwIndex) > masteryValue)
  {
    errCode = 13;
    goto RESULT;
  }
  if (GetItemKindCode(tableCode))
  {
    errCode = 9;
    goto RESULT;
  }
  if (!m_bFreeSFByClass)
  {
    bool hasHistory = false;
    for (int historyIndex = 0; historyIndex < 4; ++historyIndex)
    {
      _DWORD *historyEffect = *m_Param.m_ppHistoryEffect[historyIndex];
      if (!historyEffect)
      {
        break;
      }
      if (masteryType == 0)
      {
        if (historyEffect[362])
        {
          hasHistory = true;
        }
      }
      else if (masteryType == 1)
      {
        if (historyEffect[363])
        {
          hasHistory = true;
        }
      }
      else if (masteryType == 2)
      {
        if (historyEffect[364])
        {
          hasHistory = true;
        }
      }
      if (hasHistory)
      {
        break;
      }
    }
    if (!hasHistory)
    {
      errCode = 7;
      goto RESULT;
    }
  }

  if (CPlayerDB::GetPtrCurClass(&m_Param)->m_nGrade > 1
      && CPlayerDB::GetPtrCurClass(&m_Param)->m_nClass == 3)
  {
    _class_fld *curClass = CPlayerDB::GetPtrCurClass(&m_Param);
    _class_fld *baseClass = CPlayerDB::GetPtrBaseClass(&m_Param);
    if (curClass->m_nClass == baseClass->m_nClass && m_Param.m_pClassData->m_dwIndex != 49)
    {
      useRandomResult = true;
    }
  }

  makeToolItem = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, pipMakeTool->wItemSerial);
  if (!makeToolItem)
  {
    errCode = 1;
    goto RESULT;
  }
  if (makeToolItem->m_byTableCode != 11)
  {
    errCode = 2;
    goto RESULT;
  }
  if (makeToolItem->m_bLock)
  {
    errCode = 10;
    goto RESULT;
  }
  if (_STORAGE_LIST::GetIndexEmptyCon(&m_Param.m_dbInven) == 0xFF)
  {
    errCode = 3;
    goto RESULT;
  }

  if (useRandomResult)
  {
    const unsigned int roll = rand() % 10000;
    unsigned int acc = 0;
    for (int entryIndex = 0; entryIndex < 30; ++entryIndex)
    {
      char *entry = &manualRecord[2].m_strCode[12 * entryIndex + 56];
      if (std::strncmp(entry, "-1", 2) == 0)
      {
        break;
      }
      acc += reinterpret_cast<unsigned int *>(entry)[2];
      if (roll < acc)
      {
        const unsigned __int8 entryTableCode = GetItemTableCode(entry);
        if (entryTableCode != 0xFF)
        {
          makeItemRecord = CRecordData::GetRecordByHash(&g_Main.m_tblItemData[entryTableCode], entry, 2, 5);
        }
        break;
      }
    }
  }
  else
  {
    makeItemRecord = CRecordData::GetRecordByHash(&g_Main.m_tblItemData[tableCode], manualRecord->m_strCode, 2, 5);
    if (!makeItemRecord)
    {
      errCode = 9;
      goto RESULT;
    }
  }

  _STORAGE_LIST::_db_con *materialItems[100]{};
  int requiredMaterial[19]{};
  if (makeItemRecord)
  {
    memcpy_0(requiredMaterial, &manualRecord[2], 0x3Cu);
    if (!m_bCheat_makeitem_no_use_matrial)
    {
      for (int j = 0; j < byMaterialNum; ++j)
      {
        materialItems[j] = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, pipMaterials[j].wItemSerial);
        if (!materialItems[j])
        {
          SendMsg_AdjustAmountInform(0, pipMaterials[j].wItemSerial, 0);
          errCode = 4;
          goto RESULT;
        }
        if (materialItems[j]->m_bLock)
        {
          errCode = 10;
          goto RESULT;
        }
        if (static_cast<unsigned __int64>(pipMaterials[j].byNum) > materialItems[j]->m_dwDur)
        {
          SendMsg_AdjustAmountInform(
            0,
            pipMaterials[j].wItemSerial,
            static_cast<unsigned int>(materialItems[j]->m_dwDur));
          errCode = 5;
          goto RESULT;
        }
        for (int m = 0; m < j; ++m)
        {
          if (pipMaterials[m].wItemSerial == pipMaterials[j].wItemSerial)
          {
            errCode = 5;
            goto RESULT;
          }
        }
        bool matched = false;
        _base_fld *materialRecord = CRecordData::GetRecord(
          &g_Main.m_tblItemData[materialItems[j]->m_byTableCode],
          materialItems[j]->m_wItemIndex);
        for (int n = 0; n < 5; ++n)
        {
          if (!std::strncmp(materialRecord->m_strCode, reinterpret_cast<const char *>(&requiredMaterial[3 * n]), 7u))
          {
            requiredMaterial[3 * n + 2] -= pipMaterials[j].byNum;
            matched = true;
            break;
          }
        }
        if (!matched)
        {
          errCode = 8;
          goto RESULT;
        }
      }
      for (int j = 0; j < 5; ++j)
      {
        if (static_cast<int>(requiredMaterial[3 * j + 2]) > 0)
        {
          errCode = 6;
          break;
        }
      }
    }
  }
  else
  {
    errCode = 12;
  }

RESULT:
  if (!errCode)
  {
    _STORAGE_LIST::_db_con historyMaterials[100]{};
    unsigned __int8 materialCounts[116]{};
    if (!m_bCheat_makeitem_no_use_matrial)
    {
      for (int j = 0; j < byMaterialNum; ++j)
      {
        memcpy_0(&historyMaterials[j], materialItems[j], sizeof(_STORAGE_LIST::_db_con));
        materialCounts[j] = pipMaterials[j].byNum;
      }
    }

    const int masteryValue = _MASTERY_PARAM::GetMasteryPerMast(&m_pmMst, 5u, masteryType);
    int successRate = 0;
    int baseLevel = 0;
    int itemEquipLevel = 0;
    if (tableCode == 10)
    {
      successRate = static_cast<int>(static_cast<float>(masteryValue) * 0.5f + 40.0f);
    }
    else
    {
      baseLevel = static_cast<int>((std::sqrt(150.0f * masteryValue + 1300.0f) / 2.0f + 0.5f) - 18.0f);
      itemEquipLevel = GetItemEquipLevel(tableCode, makeItemRecord->m_dwIndex);
      successRate = static_cast<int>((50.0f - (static_cast<float>(itemEquipLevel - baseLevel) * 11.25f)) / 2.0f);
    }
    if (successRate >= 95)
    {
      successRate = 95;
    }
    if (successRate <= 0)
    {
      successRate = 0;
    }
    if (m_bCheat_100SuccMake)
    {
      successRate = 100;
    }

    const unsigned __int16 randValue = _100_per_random_table::GetRand(&m_MakeRandTable);
    if (successRate < randValue)
    {
      errCode = 100;
    }

    if (!m_bCheat_makeitem_no_use_matrial)
    {
      for (int j = 0; j < byMaterialNum; ++j)
      {
        Emb_AlterDurPoint(0, materialItems[j]->m_byStorageIndex, -pipMaterials[j].byNum, false, false);
      }
    }

    _STORAGE_LIST::_db_con newItem{};
    if (!errCode)
    {
      newItem.m_byTableCode = tableCode;
      newItem.m_wItemIndex = makeItemRecord->m_dwIndex;
      newItem.m_dwDur = GetItemDurPoint(tableCode, makeItemRecord->m_dwIndex);
      const unsigned __int8 itemKindCode = GetItemKindCode(tableCode);
      (void)itemKindCode;
      const unsigned __int8 defSocketNum = GetDefItemUpgSocketNum(tableCode, makeItemRecord->m_dwIndex);
      unsigned __int8 socketCount = 0;
      if (defSocketNum)
      {
        socketCount = static_cast<unsigned __int8>(rand() % defSocketNum + 1);
      }
      newItem.m_dwLv = GetBitAfterSetLimSocket(socketCount);
      newItem.m_wSerial = CPlayerDB::GetNewItemSerial(&m_Param);
      if (!Emb_AddStorage(0, &newItem, 0, 1))
      {
        SendMsg_MakeItemResult(0xFFu);
        return;
      }
      SendMsg_FanfareItem(6u, &newItem, nullptr);
      SendMsg_InsertItemInform(0, &newItem);
      if (tableCode == 10)
      {
        Emb_AlterStat(5u, masteryType, 1u, 0, "CPlayer::pc_MakeItem()---1", 1);
      }
      else if (baseLevel - itemEquipLevel <= 4)
      {
        Emb_AlterStat(5u, masteryType, 1u, 0, "CPlayer::pc_MakeItem()---0", 1);
      }
      _base_fld *itemRecord = CRecordData::GetRecord(&g_Main.m_tblItemData[tableCode], makeItemRecord->m_dwIndex);
      Emb_CheckActForQuest(5, itemRecord->m_strCode, 1u, 0);
    }

    if (m_bCheat_makeitem_no_use_matrial)
    {
      CMgrAvatorItemHistory::cheat_make_item_no_material(
        &CPlayer::s_MgrItemHistory,
        m_ObjID.m_wIndex,
        errCode,
        &newItem,
        m_szItemHistoryFileName);
    }
    else
    {
      CMgrAvatorItemHistory::make_item(
        &CPlayer::s_MgrItemHistory,
        m_ObjID.m_wIndex,
        historyMaterials,
        materialCounts,
        byMaterialNum,
        errCode,
        1,
        &newItem,
        m_szItemHistoryFileName);
    }
  }

  SendMsg_MakeItemResult(errCode);
}

void CPlayer::pc_UpgradeItem(
  _STORAGE_POS_INDIV *pposTalik,
  _STORAGE_POS_INDIV *pposToolItem,
  _STORAGE_POS_INDIV *pposUpgItem,
  unsigned __int8 byJewelNum,
  _STORAGE_POS_INDIV *pposUpgJewel)
{
  unsigned __int8 errCode = 0;
  _STORAGE_LIST *storage = m_Param.m_pStoragePtr[pposUpgItem->byStorageCode];
  _STORAGE_LIST::_db_con *upgradeItem = nullptr;
  _STORAGE_LIST::_db_con *talikItem = nullptr;
  _STORAGE_LIST::_db_con *toolItem = nullptr;
  _STORAGE_LIST::_db_con *jewelItems[4]{};
  _ItemUpgrade_fld *jewelRecords[4]{};
  _ItemUpgrade_fld *primaryTalikRecord = nullptr;
  unsigned __int8 itemUpgLimSocket = 0;

  if (_effect_parameter::GetEff_State(&m_EP, 20))
  {
    errCode = 8;
  }
  else if (_effect_parameter::GetEff_State(&m_EP, 28))
  {
    errCode = 8;
  }
  else
  {
    upgradeItem = _STORAGE_LIST::GetPtrFromSerial(storage, pposUpgItem->wItemSerial);
    if (upgradeItem)
    {
      if (upgradeItem->m_bLock)
      {
        errCode = 13;
      }
      else if (GetItemKindCode(upgradeItem->m_byTableCode))
      {
        errCode = 9;
      }
      else if (GetDefItemUpgSocketNum(upgradeItem->m_byTableCode, upgradeItem->m_wItemIndex))
      {
        itemUpgLimSocket = GetItemUpgLimSocket(upgradeItem->m_dwLv);
        const unsigned __int8 upgLevel = GetItemUpgedLv(upgradeItem->m_dwLv);
        if (upgLevel < itemUpgLimSocket)
        {
          talikItem = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, pposTalik->wItemSerial);
          if (talikItem)
          {
            if (talikItem->m_byTableCode == 18)
            {
              if (talikItem->m_bLock)
              {
                errCode = 13;
              }
              else
              {
                primaryTalikRecord =
                  CItemUpgradeTable::GetRecordFromRes(&g_Main.m_tblItemUpgrade, talikItem->m_wItemIndex);
                if (primaryTalikRecord)
                {
                  if (primaryTalikRecord->m_dwIndex < 0xD)
                  {
                    toolItem = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, pposToolItem->wItemSerial);
                    if (toolItem)
                    {
                      if (toolItem->m_byTableCode == 11)
                      {
                        if (toolItem->m_bLock)
                        {
                          errCode = 13;
                        }
                        else
                        {
                          for (int j = 0; j < byJewelNum; ++j)
                          {
                            jewelItems[j] =
                              _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, pposUpgJewel[j].wItemSerial);
                            if (!jewelItems[j])
                            {
                              SendMsg_AdjustAmountInform(0, pposUpgJewel[j].wItemSerial, 0);
                              errCode = 6;
                              goto RESULT_UPGRADE;
                            }
                            if (jewelItems[j]->m_byTableCode != 18)
                            {
                              errCode = 7;
                              goto RESULT_UPGRADE;
                            }
                            if (jewelItems[j]->m_bLock)
                            {
                              errCode = 13;
                              goto RESULT_UPGRADE;
                            }
                            jewelRecords[j] = CItemUpgradeTable::GetRecordFromRes(
                              &g_Main.m_tblItemUpgrade,
                              jewelItems[j]->m_wItemIndex);
                            if (!jewelRecords[j])
                            {
                              return;
                            }
                            for (int k = 0; k < j; ++k)
                            {
                              if (jewelItems[k] == jewelItems[j])
                              {
                                errCode = 7;
                                goto RESULT_UPGRADE;
                              }
                            }
                            for (int k = 0; k < j; ++k)
                            {
                              if (jewelRecords[k] == jewelRecords[j])
                              {
                                errCode = 7;
                                goto RESULT_UPGRADE;
                              }
                            }
                            if (primaryTalikRecord == jewelRecords[j])
                            {
                              errCode = 7;
                              goto RESULT_UPGRADE;
                            }
                          }
                          if (!IsAddAbleTalikToItem(
                                upgradeItem->m_byTableCode,
                                upgradeItem->m_wItemIndex,
                                upgradeItem->m_dwLv,
                                primaryTalikRecord->m_dwIndex,
                                &primaryTalikRecord->m_nUpperUp))
                          {
                            errCode = 11;
                          }
                        }
                      }
                      else
                      {
                        errCode = 4;
                      }
                    }
                    else
                    {
                      SendMsg_AdjustAmountInform(0, pposToolItem->wItemSerial, 0);
                      errCode = 3;
                    }
                  }
                  else
                  {
                    errCode = 2;
                  }
                }
                else
                {
                  errCode = 2;
                }
              }
            }
            else
            {
              errCode = 2;
            }
          }
          else
          {
            SendMsg_AdjustAmountInform(0, pposTalik->wItemSerial, 0);
            errCode = 1;
          }
        }
        else
        {
          errCode = 10;
        }
      }
      else
      {
        errCode = 9;
      }
    }
    else
    {
      errCode = 5;
    }
  }

RESULT_UPGRADE:
  if (!errCode)
  {
    _STORAGE_LIST::_db_con itemSnapshot{};
    _STORAGE_LIST::_db_con talikSnapshot{};
    _STORAGE_LIST::_db_con jewelSnapshot[4]{};
    memcpy_0(&itemSnapshot, upgradeItem, sizeof(itemSnapshot));
    memcpy_0(&talikSnapshot, talikItem, sizeof(talikSnapshot));
    for (int j = 0; j < byJewelNum; ++j)
    {
      memcpy_0(&jewelSnapshot[j], jewelItems[j], sizeof(_STORAGE_LIST::_db_con));
    }

    Emb_AlterDurPoint(0, talikItem->m_byStorageIndex, -1, false, false);
    for (int j = 0; j < byJewelNum; ++j)
    {
      Emb_AlterDurPoint(0, jewelItems[j]->m_byStorageIndex, -1, false, false);
    }

    float successBonus = 0.0f;
    for (int j = 0; j < 4; ++j)
    {
      if (jewelRecords[j])
      {
        successBonus += *reinterpret_cast<float *>(reinterpret_cast<char *>(jewelRecords[j]) + 136);
      }
      else
      {
        successBonus += 0.125f;
      }
    }

    bool recordUse = true;
    if (CMainThread::IsReleaseServiceMode(&g_Main) && m_byUserDgr)
    {
      recordUse = false;
    }
    if (recordUse)
    {
      CGameStatistics::_DAY *stats = CGameStatistics::CurWriteData(&g_GameStatistics);
      stats->dwDaePokUse_Evt += static_cast<int>(successBonus);
    }

    static const unsigned int successTable[77] = {
      1000, 750, 500, 250, 100, 50, 0,
      1000, 750, 500, 250, 100, 50, 0,
      1000, 750, 500, 250, 100, 50, 0,
      1000, 750, 500, 250, 100, 50, 0,
      328, 246, 164, 82, 49, 16, 0,
      640, 480, 320, 160, 96, 32, 0,
      512, 384, 256, 128, 77, 26, 0,
      410, 307, 205, 102, 61, 20, 0,
      800, 600, 400, 200, 120, 40, 0,
      262, 197, 131, 66, 39, 13, 0,
      210, 157, 105, 52, 31, 10, 0
    };
    static const unsigned int zeroTable[77] = {
      0, 0, 0, 0, 1600, 3200, 6400,
      0, 0, 1200, 1600, 3000, 4400, 6800,
      0, 0, 0, 1000, 2250, 3500, 5750,
      0, 750, 1125, 1500, 2375, 3250, 4625,
      2400, 3940, 6170, 6790, 7470, 8220, 9040,
      7000, 7700, 8470, 9320, 10000, 10000, 10000,
      4900, 6160, 7620, 8390, 9220, 10000, 10000,
      3430, 4930, 6860, 7550, 8300, 9130, 10000,
      10000, 10000, 10000, 10000, 10000, 10000, 10000,
      1680, 3150, 5560, 6110, 6720, 7400, 8140,
      1180, 2520, 5000, 5500, 6050, 6660, 7320
    };
    static const unsigned int destroyTable[77] = {
      0, 0, 0, 0, 2000, 4000, 8000,
      0, 0, 2400, 3200, 6000, 8800, 9500,
      0, 0, 0, 2000, 4500, 7000, 9500,
      0, 3000, 4500, 6000, 9500, 9500, 9500,
      0, 7300, 9500, 9500, 9500, 9500, 9500,
      0, 4300, 6400, 8600, 9500, 9500, 9500,
      0, 5100, 7600, 9500, 9500, 9500, 9500,
      0, 6100, 9100, 9500, 9500, 9500, 9500,
      0, 3600, 5400, 7200, 9500, 9500, 9500,
      0, 8700, 9500, 9500, 9500, 9500, 9500,
      0, 9500, 9500, 9500, 9500, 9500, 9500
    };

    const unsigned __int8 itemGrade = GetItemGrade(upgradeItem->m_byTableCode, upgradeItem->m_wItemIndex);
    const unsigned __int8 curUpgLv = GetItemUpgedLv(upgradeItem->m_dwLv);
    unsigned int successThreshold =
      static_cast<unsigned int>((static_cast<float>(successTable[7 * itemGrade + curUpgLv]) * successBonus / 4.0f)
                                * 100.0f);
    const unsigned __int8 itemEquipLevel = GetItemEquipLevel(upgradeItem->m_byTableCode, upgradeItem->m_wItemIndex);
    if (itemEquipLevel)
    {
      successThreshold = static_cast<unsigned int>(static_cast<float>(successThreshold) * (30.0f / itemEquipLevel));
    }

    const unsigned int randomValue =
      (static_cast<unsigned int>(rand()) << 16) + static_cast<unsigned int>(rand());
    if (m_bCheat_100SuccMake)
    {
      successThreshold = static_cast<unsigned int>(-1);
    }

    if (successThreshold > (randomValue % 0x186A0))
    {
      const unsigned int newGradeInfo =
        GetBitAfterUpgrade(upgradeItem->m_dwLv, primaryTalikRecord->m_dwIndex, curUpgLv);
      Emb_ItemUpgrade(0, storage->m_nListCode, upgradeItem->m_byStorageIndex, newGradeInfo);
      SendMsg_FanfareItem(0, upgradeItem, nullptr);
      _base_fld *record = CRecordData::GetRecord(
        &g_Main.m_tblItemData[upgradeItem->m_byTableCode],
        upgradeItem->m_wItemIndex);
      Emb_CheckActForQuest(10, record->m_strCode, 1u, 0);

      bool recordSuccess = true;
      if (CMainThread::IsReleaseServiceMode(&g_Main) && m_byUserDgr)
      {
        recordSuccess = false;
      }
      if (recordSuccess)
      {
        CGameStatistics::_DAY *stats = CGameStatistics::CurWriteData(&g_GameStatistics);
        if (curUpgLv == 3)
        {
          if (primaryTalikRecord->m_dwIndex)
          {
            if (primaryTalikRecord->m_dwIndex == 5)
            {
              ++stats->dw4EunUpgradeSucc_Evt;
            }
            else if (primaryTalikRecord->m_dwIndex == 12)
            {
              ++stats->dw4JaUpgradeSucc_Evt;
            }
          }
          else
          {
            ++stats->dw4MuUpgradeSucc_Evt;
          }
        }
        else if (curUpgLv == 4)
        {
          if (primaryTalikRecord->m_dwIndex)
          {
            if (primaryTalikRecord->m_dwIndex == 5)
            {
              ++stats->dw5EunUpgradeSucc_Evt;
            }
            else if (primaryTalikRecord->m_dwIndex == 12)
            {
              ++stats->dw5JaUpgradeSucc_Evt;
            }
          }
          else
          {
            ++stats->dw5MuUpgradeSucc_Evt;
          }
        }
      }
    }
    else
    {
      if (zeroTable[7 * itemGrade + curUpgLv] <= static_cast<unsigned int>(rand() % 10000))
      {
        if (destroyTable[7 * itemGrade + curUpgLv] <= static_cast<unsigned int>(rand() % 10000))
        {
          errCode = 100;
        }
        else
        {
          const unsigned int resetGradeInfo = GetBitAfterSetLimSocket(itemUpgLimSocket);
          Emb_ItemUpgrade(2u, pposUpgItem->byStorageCode, upgradeItem->m_byStorageIndex, resetGradeInfo);
          errCode = 101;
        }
      }
      else
      {
        if (!Emb_DelStorage(storage->m_nListCode, upgradeItem->m_byStorageIndex, 0, 1, "CPlayer::pc_UpgradeItem()"))
        {
          SendMsg_ItemUpgrade(0xFFu);
          return;
        }
        errCode = 102;
      }
    }

    CMgrAvatorItemHistory::grade_up_item(
      &CPlayer::s_MgrItemHistory,
      m_ObjID.m_wIndex,
      &itemSnapshot,
      &talikSnapshot,
      jewelSnapshot,
      byJewelNum,
      errCode,
      upgradeItem->m_dwLv,
      m_szItemHistoryFileName);
  }

  SendMsg_ItemUpgrade(errCode);
}

void CPlayer::pc_DowngradeItem(
  _STORAGE_POS_INDIV *pposTalik,
  _STORAGE_POS_INDIV *pposToolItem,
  _STORAGE_POS_INDIV *pposUpgItem)
{
  unsigned __int8 errCode = 0;
  _STORAGE_LIST *storage = m_Param.m_pStoragePtr[pposUpgItem->byStorageCode];
  _STORAGE_LIST::_db_con *talikItem = nullptr;
  _STORAGE_LIST::_db_con *toolItem = nullptr;
  _STORAGE_LIST::_db_con *upgradeItem = nullptr;
  _ItemUpgrade_fld *talikRecord = nullptr;

  if (_effect_parameter::GetEff_State(&m_EP, 20))
  {
    errCode = 7;
  }
  else if (_effect_parameter::GetEff_State(&m_EP, 28))
  {
    errCode = 7;
  }
  else
  {
    upgradeItem = _STORAGE_LIST::GetPtrFromSerial(storage, pposUpgItem->wItemSerial);
    if (upgradeItem)
    {
      if (upgradeItem->m_bLock)
      {
        errCode = 13;
      }
      else if (GetItemKindCode(upgradeItem->m_byTableCode))
      {
        errCode = 9;
      }
      else if (GetDefItemUpgSocketNum(upgradeItem->m_byTableCode, upgradeItem->m_wItemIndex))
      {
        if (GetItemUpgedLv(upgradeItem->m_dwLv))
        {
          talikItem = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, pposTalik->wItemSerial);
          if (talikItem)
          {
            if (talikItem->m_byTableCode == 18)
            {
              if (talikItem->m_bLock)
              {
                errCode = 13;
              }
              else
              {
                talikRecord = CItemUpgradeTable::GetRecordFromRes(&g_Main.m_tblItemUpgrade, talikItem->m_wItemIndex);
                if (talikRecord)
                {
                  if (talikRecord->m_nDataEffect == 14)
                  {
                    toolItem = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, pposToolItem->wItemSerial);
                    if (toolItem)
                    {
                      if (toolItem->m_byTableCode == 11)
                      {
                        if (toolItem->m_bLock)
                        {
                          errCode = 13;
                        }
                      }
                      else
                      {
                        errCode = 4;
                      }
                    }
                    else
                    {
                      SendMsg_AdjustAmountInform(0, pposToolItem->wItemSerial, 0);
                      errCode = 3;
                    }
                  }
                  else
                  {
                    errCode = 2;
                  }
                }
                else
                {
                  errCode = 2;
                }
              }
            }
            else
            {
              errCode = 2;
            }
          }
          else
          {
            SendMsg_AdjustAmountInform(0, pposTalik->wItemSerial, 0);
            errCode = 1;
          }
        }
        else
        {
          errCode = 12;
        }
      }
      else
      {
        errCode = 9;
      }
    }
    else
    {
      errCode = 5;
    }
  }

  if (!errCode)
  {
    _STORAGE_LIST::_db_con itemSnapshot{};
    _STORAGE_LIST::_db_con talikSnapshot{};
    memcpy_0(&itemSnapshot, upgradeItem, sizeof(itemSnapshot));
    memcpy_0(&talikSnapshot, talikItem, sizeof(talikSnapshot));
    Emb_AlterDurPoint(0, talikItem->m_byStorageIndex, -1, false, false);

    const unsigned int currentLvBit = upgradeItem->m_dwLv;
    const unsigned __int8 upgLevel = GetItemUpgedLv(currentLvBit);
    const unsigned __int8 talikFromSocket = GetTalikFromSocket(currentLvBit, static_cast<unsigned __int8>(upgLevel - 1));
    const unsigned int newGradeInfo = GetBitAfterDowngrade(currentLvBit, upgLevel);
    Emb_ItemUpgrade(1u, storage->m_nListCode, upgradeItem->m_byStorageIndex, newGradeInfo);

    const int talikItemIndex = FixTalikItemIndex(talikFromSocket);
    if (talikItemIndex > 0)
    {
      _STORAGE_LIST::_db_con rewardItem{};
      rewardItem.m_byTableCode = 18;
      rewardItem.m_wItemIndex = static_cast<unsigned __int16>(talikItemIndex);
      rewardItem.m_dwDur = GetItemDurPoint(18, talikItemIndex);
      rewardItem.m_dwLv = 0xFFFFFFF;
      if (_STORAGE_LIST::GetIndexEmptyCon(&m_Param.m_dbInven) == 0xFF)
      {
        CreateItemBox(
          &rewardItem,
          this,
          0xFFFFFFFF,
          false,
          nullptr,
          3u,
          m_pCurMap,
          m_wMapLayerIndex,
          m_fCurPos,
          false);
        CMgrAvatorItemHistory::reward_add_item(
          &CPlayer::s_MgrItemHistory,
          m_ObjID.m_wIndex,
          "GradeDown Ground Reward",
          &rewardItem,
          m_szItemHistoryFileName);
      }
      else
      {
        rewardItem.m_wSerial = CPlayerDB::GetNewItemSerial(&m_Param);
        if (!Emb_AddStorage(0, &rewardItem, 0, 1))
        {
          SendMsg_ItemDowngrade(0xFFu);
          CMgrAvatorItemHistory::add_storage_fail(
            &CPlayer::s_MgrItemHistory,
            m_ObjID.m_wIndex,
            &rewardItem,
            "CPlayer::pc_DowngradeItem - Emb_AddStorage() Fail",
            m_szItemHistoryFileName);
          return;
        }
        SendMsg_RewardAddItem(&rewardItem, 0);
        CMgrAvatorItemHistory::reward_add_item(
          &CPlayer::s_MgrItemHistory,
          m_ObjID.m_wIndex,
          "GradeDown Reward",
          &rewardItem,
          m_szItemHistoryFileName);
      }
    }

    (void)talikSnapshot;
    CMgrAvatorItemHistory::grade_down_item(
      &CPlayer::s_MgrItemHistory,
      m_ObjID.m_wIndex,
      &itemSnapshot,
      talikItem,
      upgradeItem->m_dwLv,
      m_szItemHistoryFileName);
  }

  SendMsg_ItemDowngrade(errCode);
}

void CPlayer::pc_CombineItem(
  unsigned __int16 wManualIndex,
  unsigned __int8 byMaterialNum,
  _STORAGE_POS_INDIV *pipMaterials,
  unsigned __int16 wOverlapItemSerial)
{
  unsigned __int8 errCode = 0;
  _STORAGE_LIST::_db_con *materialItems[100]{};
  _base_fld *manualRecord = CRecordData::GetRecord(&g_Main.m_tblItemCombineData, wManualIndex);
  unsigned __int8 itemTableCode = static_cast<unsigned __int8>(-1);
  _base_fld *combineRecord = nullptr;
  _STORAGE_LIST::_db_con *overlapItem = nullptr;

  if (manualRecord)
  {
    const int raceSexCode = CPlayerDB::GetRaceSexCode(&m_Param);
    if (manualRecord[1].m_strCode[raceSexCode + 4] == '1')
    {
      const unsigned int dalant = CPlayerDB::GetDalant(&m_Param);
      if (*reinterpret_cast<unsigned int *>(manualRecord[1].m_strCode) <= dalant)
      {
        if (_STORAGE_LIST::GetIndexEmptyCon(&m_Param.m_dbInven) == 0xFF)
        {
          errCode = 3;
        }
        else
        {
          itemTableCode = GetItemTableCode(manualRecord->m_strCode);
          if (itemTableCode == 0xFF)
          {
            errCode = 12;
          }
          else
          {
            combineRecord =
              CRecordData::GetRecordByHash(&g_Main.m_tblItemData[itemTableCode], manualRecord->m_strCode, 2, 5);
            if (combineRecord)
            {
              if (wOverlapItemSerial != 0xFFFF)
              {
                _STORAGE_LIST::_db_con *candidate =
                  _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, wOverlapItemSerial);
                if (candidate && candidate->m_byTableCode == itemTableCode
                    && candidate->m_wItemIndex == combineRecord->m_dwIndex && candidate->m_dwDur < 0x63
                    && !candidate->m_bLock)
                {
                  overlapItem = candidate;
                }
              }

              int requiredMaterial[19]{};
              memcpy_0(requiredMaterial, &manualRecord[2].m_strCode[8], 0x3Cu);
              for (int j = 0; j < byMaterialNum; ++j)
              {
                materialItems[j] = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, pipMaterials[j].wItemSerial);
                if (!materialItems[j])
                {
                  SendMsg_AdjustAmountInform(0, pipMaterials[j].wItemSerial, 0);
                  errCode = 4;
                  goto RESULT_COMBINE;
                }
                if (materialItems[j]->m_bLock)
                {
                  errCode = 10;
                  goto RESULT_COMBINE;
                }
                if (static_cast<unsigned __int64>(pipMaterials[j].byNum) > materialItems[j]->m_dwDur)
                {
                  SendMsg_AdjustAmountInform(
                    0,
                    pipMaterials[j].wItemSerial,
                    static_cast<unsigned int>(materialItems[j]->m_dwDur));
                  errCode = 5;
                  goto RESULT_COMBINE;
                }
                for (int k = 0; k < j; ++k)
                {
                  if (pipMaterials[k].wItemSerial == pipMaterials[j].wItemSerial)
                  {
                    errCode = 5;
                    goto RESULT_COMBINE;
                  }
                }
                bool matched = false;
                _base_fld *materialRecord = CRecordData::GetRecord(
                  &g_Main.m_tblItemData[materialItems[j]->m_byTableCode],
                  materialItems[j]->m_wItemIndex);
                for (int m = 0; m < 5; ++m)
                {
                  if (!std::strncmp(
                        materialRecord->m_strCode,
                        reinterpret_cast<const char *>(&requiredMaterial[3 * m]),
                        7u))
                  {
                    requiredMaterial[3 * m + 2] -= pipMaterials[j].byNum;
                    matched = true;
                    break;
                  }
                }
                if (!matched)
                {
                  errCode = 8;
                  goto RESULT_COMBINE;
                }
              }
              for (int j = 0; j < 5; ++j)
              {
                if (static_cast<int>(requiredMaterial[3 * j + 2]) > 0)
                {
                  errCode = 6;
                  goto RESULT_COMBINE;
                }
              }

              if (*reinterpret_cast<unsigned int *>(&manualRecord[2].m_strCode[4]) != 0xFFFFFFFF
                  && *reinterpret_cast<unsigned int *>(manualRecord[2].m_strCode) == 6)
              {
                CGoldenBoxItemMgr *goldenBox = CGoldenBoxItemMgr::Instance();
                if (CGoldenBoxItemMgr::Get_Event_Status(goldenBox) == 2)
                {
                  unsigned int actPoint = CUserDB::GetActPoint(m_pUserDB, 2u);
                  if (actPoint >= *reinterpret_cast<unsigned int *>(&manualRecord[2].m_strCode[4]))
                  {
                    unsigned int curActPoint = CUserDB::GetActPoint(m_pUserDB, 2u);
                    CUserDB::SetActPoint(
                      m_pUserDB,
                      2u,
                      curActPoint - *reinterpret_cast<unsigned int *>(&manualRecord[2].m_strCode[4]));
                    unsigned int leftActPoint = CUserDB::GetActPoint(m_pUserDB, 2u);
                    SendMsg_Alter_Action_Point(2u, leftActPoint);
                  }
                  else
                  {
                    errCode = 15;
                  }
                }
                else
                {
                  errCode = 12;
                }
              }
            }
            else
            {
              errCode = 12;
            }
          }
        }
      }
      else
      {
        errCode = 14;
      }
    }
    else
    {
      errCode = 11;
    }
  }
  else
  {
    errCode = 12;
  }

RESULT_COMBINE:
  unsigned int feeDalant = 0;
  _STORAGE_LIST::_db_con newItem{};
  const _TimeItem_fld *timeRec = nullptr;
  if (errCode)
  {
    goto SEND_COMBINE_RESULT;
  }

  _STORAGE_LIST::_db_con historyMaterials[100]{};
  unsigned __int8 materialCounts[117]{};
  for (int j = 0; j < byMaterialNum; ++j)
  {
    memcpy_0(&historyMaterials[j], materialItems[j], sizeof(_STORAGE_LIST::_db_con));
    materialCounts[j] = pipMaterials[j].byNum;
  }

  timeRec = TimeItem::FindTimeRec(itemTableCode, combineRecord->m_dwIndex);
  if (overlapItem && (!timeRec || !timeRec->m_nCheckType))
  {
    memcpy_0(&newItem, overlapItem, sizeof(newItem));
    ++newItem.m_dwDur;
    Emb_AlterDurPoint(0, overlapItem->m_byStorageIndex, 1, false, true);
    goto APPLY_COMBINE_COST;
  }

  newItem.m_byTableCode = itemTableCode;
  newItem.m_wItemIndex = combineRecord->m_dwIndex;
  newItem.m_dwDur = GetItemDurPoint(itemTableCode, combineRecord->m_dwIndex);
  materialCounts[116] = GetItemKindCode(itemTableCode);
  const unsigned __int8 defSocketNum = GetDefItemUpgSocketNum(itemTableCode, combineRecord->m_dwIndex);
  unsigned __int8 socketCount = 0;
  if (defSocketNum)
  {
    socketCount = static_cast<unsigned __int8>(rand() % defSocketNum + 1);
  }
  newItem.m_dwLv = GetBitAfterSetLimSocket(socketCount);
  newItem.m_wSerial = CPlayerDB::GetNewItemSerial(&m_Param);
  if (timeRec && timeRec->m_nCheckType)
  {
    newItem.m_byCsMethod = timeRec->m_nCheckType;
    __time32_t timeValue[5]{};
    _time32(timeValue);
    newItem.m_dwT = timeRec->m_nUseTime + timeValue[0];
    newItem.m_dwLendRegdTime = timeValue[0];
  }

  if (Emb_AddStorage(0, &newItem, 0, 1))
  {
    SendMsg_FanfareItem(1u, &newItem, nullptr);
    if (*reinterpret_cast<unsigned int *>(&manualRecord[2].m_strCode[4]) != 0xFFFFFFFF
        && *reinterpret_cast<unsigned int *>(manualRecord[2].m_strCode) == 6)
    {
      CGoldenBoxItemMgr *goldenBox = CGoldenBoxItemMgr::Instance();
      if (CGoldenBoxItemMgr::Get_Event_Status(goldenBox) == 2)
      {
        _base_fld *itemRecord =
          CRecordData::GetRecord(&g_Main.m_tblItemData[newItem.m_byTableCode], newItem.m_wItemIndex);
        if (!itemRecord)
        {
          return;
        }
        if (!strcmp_0(itemRecord->m_strCode, "bxgol04"))
        {
          const char *charName = CPlayerDB::GetCharNameA(&m_Param);
          const unsigned int charSerial = CPlayerDB::GetCharSerial(&m_Param);
          SendMsg_Notify_Get_Golden_Box(5u, charSerial, charName, &newItem, 0);
        }
      }
    }
    goto APPLY_COMBINE_COST;
  }

  if (timeRec && timeRec->m_nCheckType)
  {
    SendMsg_CombineLendItemResult(0xFFu, feeDalant, &newItem);
  }
  else
  {
    SendMsg_CombineItemResult(0xFFu, feeDalant, &newItem);
  }
  CMgrAvatorItemHistory::add_storage_fail(
    &CPlayer::s_MgrItemHistory,
    m_ObjID.m_wIndex,
    &newItem,
    "CPlayer::pc_CombineItem - Emb_AddStorage() Fail",
    m_szItemHistoryFileName);
  if (*reinterpret_cast<unsigned int *>(manualRecord[2].m_strCode) == 6)
  {
    CGoldenBoxItemMgr *goldenBox = CGoldenBoxItemMgr::Instance();
    if (CGoldenBoxItemMgr::Get_Event_Status(goldenBox) == 2)
    {
      unsigned int actPoint = CUserDB::GetActPoint(m_pUserDB, 2u);
      CUserDB::SetActPoint(
        m_pUserDB,
        2u,
        *reinterpret_cast<unsigned int *>(&manualRecord[2].m_strCode[4]) + actPoint);
      unsigned int leftActPoint = CUserDB::GetActPoint(m_pUserDB, 2u);
      SendMsg_Alter_Action_Point(
        2u,
        *reinterpret_cast<unsigned int *>(&manualRecord[2].m_strCode[4]) + leftActPoint);
    }
  }
  return;

APPLY_COMBINE_COST:
  for (int j = 0; j < byMaterialNum; ++j)
  {
    Emb_AlterDurPoint(0, materialItems[j]->m_byStorageIndex, -pipMaterials[j].byNum, false, false);
  }
  feeDalant = *reinterpret_cast<unsigned int *>(manualRecord[1].m_strCode);
  if (feeDalant)
  {
    SubDalant(feeDalant);
    const int level = CPlayerDB::GetLevel(&m_Param);
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      const int lv = CPlayerDB::GetLevel(&m_Param);
      const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
      CMoneySupplyMgr *moneySupply = CMoneySupplyMgr::Instance();
      CMoneySupplyMgr::UpdateFeeMoneyData(moneySupply, raceCode, lv, feeDalant);
    }
  }
  SendMsg_ExchangeMoneyResult(0);
  {
    const unsigned int leftDalant = CPlayerDB::GetDalant(&m_Param);
    CMgrAvatorItemHistory::combine_item(
      &CPlayer::s_MgrItemHistory,
      m_ObjID.m_wIndex,
      historyMaterials,
      materialCounts,
      byMaterialNum,
      &newItem,
      feeDalant,
      leftDalant,
      m_szItemHistoryFileName);
  }

SEND_COMBINE_RESULT:
  if (timeRec && timeRec->m_nCheckType)
  {
    SendMsg_CombineLendItemResult(errCode, feeDalant, &newItem);
  }
  else
  {
    SendMsg_CombineItemResult(errCode, feeDalant, &newItem);
  }
}

void CPlayer::pc_ExchangeItem(unsigned __int16 wManualIndex, unsigned __int16 wItemSerial)
{
  unsigned __int8 errCode = 0;
  _base_fld *exchangeRecord = nullptr;
  _base_fld *outItemRecord = nullptr;
  int newTableCode = -1;
  unsigned __int8 newDur = 0;

  exchangeRecord = CRecordData::GetRecord(&g_Main.m_tblItemExchangeData, wManualIndex);
  _STORAGE_LIST::_db_con *useItem = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, wItemSerial);
  if (!useItem)
  {
    errCode = 4;
    goto RESULT_EXCHANGE;
  }
  if (useItem->m_bLock)
  {
    errCode = 10;
    goto RESULT_EXCHANGE;
  }

  _base_fld *useItemRecord =
    CRecordData::GetRecord(&g_Main.m_tblItemData[useItem->m_byTableCode], useItem->m_wItemIndex);
  if (!useItemRecord)
  {
    errCode = 12;
    goto RESULT_EXCHANGE;
  }

  const unsigned __int8 useTableCode = useItem->m_byTableCode;
  const unsigned __int16 useItemIndex = useItem->m_wItemIndex;
  const int cashType = GetUsePcCashType(useItem->m_byTableCode, useItem->m_wItemIndex);
  if (!IsUsableAccountType(cashType))
  {
    SendMsg_PremiumCashItemUse(0xFFFFu);
    errCode = 12;
    goto RESULT_EXCHANGE;
  }

  if (useItem->m_byTableCode == 17)
  {
    _base_fld *goldRecord = CRecordData::GetRecord(&g_Main.m_tblItemData[17], useItem->m_wItemIndex);
    if (!goldRecord)
    {
      errCode = 12;
      goto RESULT_EXCHANGE;
    }
    if (_STORAGE_LIST::GetIndexEmptyCon(&m_Param.m_dbInven) == 0xFF)
    {
      errCode = 3;
      goto RESULT_EXCHANGE;
    }

    if (*reinterpret_cast<unsigned int *>(&goldRecord[3].m_strCode[4]))
    {
      CGoldenBoxItemMgr *goldenBox = CGoldenBoxItemMgr::Instance();
      if (CGoldenBoxItemMgr::Get_Event_Status(goldenBox) != 2)
      {
        errCode = 12;
        goto RESULT_EXCHANGE;
      }

      unsigned __int8 boxIndex = 0;
      char goldItems[640]{};
      CGoldenBoxItemMgr *goldenBoxItems = CGoldenBoxItemMgr::Instance();
      char *goldItemPtr = CGoldenBoxItemMgr::GetGoldBoxItemPtr(goldenBoxItems);
      memcpy_0(goldItems, goldItemPtr, 0x280u);
      for (int j = 0;; ++j)
      {
        CGoldenBoxItemMgr *loopMgr = CGoldenBoxItemMgr::Instance();
        const unsigned __int8 loopCount = CGoldenBoxItemMgr::GetLoopCount(loopMgr);
        if (j >= loopCount)
        {
          break;
        }
        if (!strcmp_0(goldRecord->m_strCode, &goldItems[64 * j]))
        {
          boxIndex = static_cast<unsigned __int8>(j);
          break;
        }
      }

      CGoldenBoxItemMgr *boxMgr = CGoldenBoxItemMgr::Instance();
      char *itemCode = static_cast<char *>(CGoldenBoxItemMgr::BoxItemOpen(boxMgr, boxIndex));
      if (itemCode)
      {
        const int tableCode = GetItemTableCode(itemCode);
        if (tableCode < 0)
        {
          errCode = 12;
          goto RESULT_EXCHANGE;
        }
        newTableCode = tableCode;
        outItemRecord = CRecordData::GetRecordByHash(
          &g_Main.m_tblItemData[static_cast<unsigned __int8>(tableCode)],
          itemCode,
          2,
          5);
        newDur = static_cast<unsigned __int8>(itemCode[8]);
        if (outItemRecord)
        {
          CGoldenBoxItemMgr *updateMgr = CGoldenBoxItemMgr::Instance();
          CGoldenBoxItemMgr::Set_BoxItem_Count(updateMgr, boxIndex, outItemRecord->m_dwIndex);
        }
      }
    }
  }
  else
  {
    if (!exchangeRecord)
    {
      errCode = 12;
      goto RESULT_EXCHANGE;
    }
    if (useItem->m_byTableCode != 31)
    {
      errCode = 12;
      goto RESULT_EXCHANGE;
    }
    if (!CheckSameItemFromString_CodeIndex(exchangeRecord->m_strCode, useItem->m_byTableCode, useItem->m_wItemIndex))
    {
      errCode = 12;
      goto RESULT_EXCHANGE;
    }
    _base_fld *itemRecord =
      CRecordData::GetRecord(&g_Main.m_tblItemData[useItem->m_byTableCode], useItem->m_wItemIndex);
    if (!itemRecord)
    {
      errCode = 12;
      goto RESULT_EXCHANGE;
    }
    const int level = CPlayerDB::GetLevel(&m_Param);
    if (level < *reinterpret_cast<unsigned int *>(itemRecord[4].m_strCode))
    {
      errCode = 21;
      goto RESULT_EXCHANGE;
    }
    if (*reinterpret_cast<unsigned int *>(&itemRecord[4].m_strCode[8]) != 0xFFFFFFFF)
    {
      const int curLevel = CPlayerDB::GetLevel(&m_Param);
      if (curLevel > *reinterpret_cast<unsigned int *>(&itemRecord[4].m_strCode[8]))
      {
        errCode = 21;
        goto RESULT_EXCHANGE;
      }
    }
    if (_STORAGE_LIST::GetIndexEmptyCon(&m_Param.m_dbInven) == 0xFF)
    {
      errCode = 3;
      goto RESULT_EXCHANGE;
    }

    const unsigned int roll = rand() % 10000;
    unsigned int acc = 0;
    for (int k = 0; k < 61; ++k)
    {
      char *entry = reinterpret_cast<char *>(&exchangeRecord[1]) + 16 * k;
      if (strlen_0(entry) < 2 || !std::strncmp(entry, "-", 1u) || !std::strncmp(entry, "-1", 2u))
      {
        break;
      }
      acc += reinterpret_cast<unsigned int *>(entry)[3];
      if (roll < acc)
      {
        const int entryTableCode = GetItemTableCode(entry);
        if (entryTableCode < 0)
        {
          break;
        }
        newTableCode = entryTableCode;
        outItemRecord = CRecordData::GetRecordByHash(
          &g_Main.m_tblItemData[static_cast<unsigned __int8>(entryTableCode)],
          entry,
          2,
          5);
        newDur = static_cast<unsigned __int8>(entry[8]);
        if (outItemRecord)
        {
          char *itemEquipCivil = GetItemEquipCivil(static_cast<unsigned __int8>(newTableCode), outItemRecord->m_dwIndex);
          const int raceSex = CPlayerDB::GetRaceSexCode(&m_Param);
          if (itemEquipCivil[raceSex] != '1')
          {
            entry = reinterpret_cast<char *>(&exchangeRecord[1]);
            newTableCode = GetItemTableCode(entry);
            outItemRecord = CRecordData::GetRecordByHash(
              &g_Main.m_tblItemData[static_cast<unsigned __int8>(newTableCode)],
              entry,
              2,
              5);
            newDur = static_cast<unsigned __int8>(entry[8]);
          }
          break;
        }
      }
    }
  }

  if (!outItemRecord)
  {
    errCode = 12;
  }

RESULT_EXCHANGE:
  const _TimeItem_fld *timeRec = nullptr;
  _STORAGE_LIST::_db_con newItem{};
  if (!errCode)
  {
    if (IsOverLapItem(useItem->m_byTableCode))
    {
      Emb_AlterDurPoint(0, useItem->m_byStorageIndex, -1, true, true);
    }
    else if (!Emb_DelStorage(0, useItem->m_byStorageIndex, 0, 1, "CPlayer::pc_ExchangeItem()"))
    {
      if (timeRec && timeRec->m_nCheckType)
      {
        SendMsg_ExchangeLendItemResult(0xFFu, &newItem);
        return;
      }
      SendMsg_ExchangeItemResult(0xFFu, &newItem);
      return;
    }

    timeRec = TimeItem::FindTimeRec(static_cast<unsigned __int8>(newTableCode), outItemRecord->m_dwIndex);
    newItem.m_byTableCode = static_cast<unsigned __int8>(newTableCode);
    newItem.m_wItemIndex = outItemRecord->m_dwIndex;
    if (IsOverLapItem(static_cast<unsigned __int8>(newTableCode)))
    {
      newItem.m_dwDur = newDur;
    }
    else
    {
      newItem.m_dwDur = GetItemDurPoint(static_cast<unsigned __int8>(newTableCode), outItemRecord->m_dwIndex);
    }
    const unsigned __int8 itemKindCode = GetItemKindCode(static_cast<unsigned __int8>(newTableCode));
    (void)itemKindCode;
    const unsigned __int8 defSocketNum =
      GetDefItemUpgSocketNum(static_cast<unsigned __int8>(newTableCode), outItemRecord->m_dwIndex);
    unsigned __int8 socketCount = 0;
    if (defSocketNum)
    {
      socketCount = static_cast<unsigned __int8>(rand() % defSocketNum + 1);
    }
    newItem.m_dwLv = GetBitAfterSetLimSocket(socketCount);
    newItem.m_wSerial = CPlayerDB::GetNewItemSerial(&m_Param);
    if (timeRec && timeRec->m_nCheckType)
    {
      newItem.m_byCsMethod = timeRec->m_nCheckType;
      __time32_t timeValue[8]{};
      _time32(timeValue);
      newItem.m_dwT = timeRec->m_nUseTime + timeValue[0];
      newItem.m_dwLendRegdTime = timeValue[0];
    }
    if (!Emb_AddStorage(0, &newItem, 0, 1))
    {
      Emb_AddStorage(0, useItem, 0, 1);
      if (timeRec && timeRec->m_nCheckType)
      {
        SendMsg_ExchangeLendItemResult(0xFFu, &newItem);
        return;
      }
      SendMsg_ExchangeItemResult(0xFFu, &newItem);
      return;
    }

    SendMsg_FanfareItem(2u, &newItem, nullptr);
    CGoldenBoxItemMgr *goldenBox = CGoldenBoxItemMgr::Instance();
    if (CGoldenBoxItemMgr::Get_Event_Status(goldenBox) == 2)
    {
      unsigned __int8 effectType[32]{};
      bool bCircle = true;
      _base_fld *useRecord =
        CRecordData::GetRecord(&g_Main.m_tblItemData[useTableCode], useItemIndex);
      _base_fld *newRecord =
        CRecordData::GetRecord(&g_Main.m_tblItemData[newItem.m_byTableCode], newItem.m_wItemIndex);
      if (!useRecord || !newRecord)
      {
        return;
      }
      char *newItemCode = newRecord->m_strCode;
      char *useItemCode = useRecord->m_strCode;
      CGoldenBoxItemMgr *effectMgr = CGoldenBoxItemMgr::Instance();
      CGoldenBoxItemMgr::BoxItemOpenEffectType(effectMgr, useItemCode, newItemCode, effectType, &bCircle);
      if (effectType[0] == 6 || effectType[0] == 8)
      {
        const char *charName = CPlayerDB::GetCharNameA(&m_Param);
        const unsigned int charSerial = CPlayerDB::GetCharSerial(&m_Param);
        SendMsg_Notify_Get_Golden_Box(effectType[0], charSerial, charName, &newItem, 0);
        if (effectType[0] == 6)
        {
          CGoldenBoxItemMgr *logMgr = CGoldenBoxItemMgr::Instance();
          CGoldenBoxItemMgr::WriteGetGoldBarLog(logMgr, this, &newItem);
        }
        else if (effectType[0] == 8)
        {
          CGoldenBoxItemMgr *logMgr = CGoldenBoxItemMgr::Instance();
          CGoldenBoxItemMgr::WriteEventCouponLog(logMgr, this, &newItem);
        }
      }
      else
      {
        SendMsg_Notify_Me_Get_Golden_Box(effectType[0], &newItem);
      }
    }

    CMgrAvatorItemHistory::exchange_item(
      &CPlayer::s_MgrItemHistory,
      m_ObjID.m_wIndex,
      useItem,
      &newItem,
      m_szItemHistoryFileName);
  }

  if (timeRec && timeRec->m_nCheckType)
  {
    SendMsg_ExchangeLendItemResult(errCode, &newItem);
  }
  else
  {
    SendMsg_ExchangeItemResult(errCode, &newItem);
  }
}

void CPlayer::pc_AddBag(unsigned __int16 wBagItemSerial)
{
  unsigned __int8 errCode = 0;
  _STORAGE_LIST *inventory = m_Param.m_pStoragePtr[0];
  _STORAGE_LIST::_db_con *bagItem = nullptr;

  if (m_pUserDB)
  {
    if (_effect_parameter::GetEff_State(&m_EP, 20))
    {
      errCode = 3;
    }
    else if (_effect_parameter::GetEff_State(&m_EP, 28))
    {
      errCode = 3;
    }
    else
    {
      bagItem = _STORAGE_LIST::GetPtrFromSerial(inventory, wBagItemSerial);
      if (bagItem)
      {
        if (bagItem->m_byTableCode == 12)
        {
          if (bagItem->m_bLock)
          {
            errCode = 11;
          }
          else if (CPlayerDB::GetBagNum(&m_Param) >= 5u)
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
        errCode = 1;
      }
    }

    if (!errCode)
    {
      const unsigned __int8 oldBagNum = CPlayerDB::GetBagNum(&m_Param);
      CPlayerDB::SetBagNum(&m_Param, static_cast<unsigned __int8>(oldBagNum + 1));
      inventory->SetUseListNum(20 * CPlayerDB::GetBagNum(&m_Param));
      if (!Emb_DelStorage(inventory->m_nListCode, bagItem->m_byStorageIndex, 0, true, "CPlayer::pc_AddBag()"))
      {
        CPlayerDB::SetBagNum(&m_Param, oldBagNum);
        inventory->SetUseListNum(20 * CPlayerDB::GetBagNum(&m_Param));
        SendMsg_AddBagResult(0xFFu);
        return;
      }
      CMgrAvatorItemHistory::consume_del_item(
        &CPlayer::s_MgrItemHistory,
        m_ObjID.m_wIndex,
        bagItem,
        m_szItemHistoryFileName);
      if (m_pUserDB)
      {
        CUserDB::Update_BagNum(m_pUserDB, CPlayerDB::GetBagNum(&m_Param));
      }
    }
    SendMsg_AddBagResult(errCode);
  }
}

char CPlayer::pc_UseRecoverLossExpItem(unsigned __int16 wItemSerial)
{
  if (CPlayerDB::GetLossExp(&m_Param) == 0.0)
  {
    return -1;
  }
  if (CGameObject::GetCurSecNum(this) == -1 || m_bMapLoading)
  {
    return -2;
  }
  if (m_bCorpse)
  {
    return -3;
  }
  if (m_byStandType == 1)
  {
    return -4;
  }
  if (IsSiegeMode())
  {
    return -5;
  }
  if (IsRidingUnit())
  {
    return -6;
  }
  if (_effect_parameter::GetEff_State(&m_EP, 20))
  {
    return -7;
  }
  if (_effect_parameter::GetEff_State(&m_EP, 28))
  {
    return -7;
  }

  _STORAGE_LIST::_db_con *item = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, wItemSerial);
  if (!item)
  {
    SendMsg_AdjustAmountInform(0, wItemSerial, 0);
    return -8;
  }
  if (item->m_byTableCode != 30)
  {
    return -8;
  }

  _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblItemData[30], item->m_wItemIndex);
  if (!record)
  {
    return -8;
  }
  if (item->m_bLock)
  {
    return -9;
  }

  const int level = CPlayerDB::GetLevel(&m_Param);
  if (level < *reinterpret_cast<int *>(record[4].m_strCode))
  {
    return -9;
  }
  if (*reinterpret_cast<int *>(&record[4].m_strCode[4]) != -1
      && CPlayerDB::GetLevel(&m_Param) > *reinterpret_cast<int *>(&record[4].m_strCode[4]))
  {
    return -9;
  }

  int range = *reinterpret_cast<int *>(&record[4].m_strCode[40])
    - *reinterpret_cast<int *>(&record[4].m_strCode[36]) + 1;
  if (range < 1)
  {
    range = 1;
  }
  const int prob = *reinterpret_cast<int *>(&record[4].m_strCode[36]) + rand() % range;

  long double alterExp = CPlayerDB::GetLossExp(&m_Param) * (double)prob / 100.0;
  if (alterExp < 0.0)
  {
    alterExp = 0.0;
  }
  if (alterExp == 0.0)
  {
    return -10;
  }

  const long double oldExp = CPlayerDB::GetExp(&m_Param);
  const unsigned int oldRate = m_dwExpRate;
  const long double oldLossExp = CPlayerDB::GetLossExp(&m_Param);

  AlterExp(static_cast<double>(alterExp), 1, 1, 0);
  CPlayerDB::SetLossExp(&m_Param, 0.0);
  CUserDB::Update_LossExp(m_pUserDB, 0.0);
  Emb_AlterDurPoint(0, item->m_byStorageIndex, -1, false, false);
  CMgrAvatorItemHistory::consume_del_item(
    &CPlayer::s_MgrItemHistory,
    m_ObjID.m_wIndex,
    item,
    m_szItemHistoryFileName);

  CMgrAvatorLvHistory::recovery_exp(
    &CPlayer::s_MgrLvHistory,
    m_ObjID.m_wIndex,
    oldExp,
    oldRate,
    CPlayerDB::GetExp(&m_Param),
    m_dwExpRate,
    oldLossExp,
    prob,
    record->m_strCode,
    m_szLvHistoryFileName);
  return static_cast<char>(prob);
}

void CPlayer::pc_CombineItemEx(_combine_ex_item_request_clzo *pRecv)
{
  _combine_ex_item_result_zocl send{};
  ItemCombineMgr::RequestCombineProcess(&m_ItemCombineMgr, pRecv, &send);
  SendMsg_CombineItemExResult(&send);
}

void CPlayer::pc_CombineItemExAccept(_combine_ex_item_accept_request_clzo *pRecv)
{
  _combine_ex_item_accept_result_zocl send{};
  unsigned __int8 errCode = ItemCombineMgr::RequestCombineAcceptProcess(&m_ItemCombineMgr, pRecv, &send);
  if (errCode)
  {
    send.byErrCode = errCode;
  }
  SendMsg_CombineItemExAcceptResult(&send);
}

__int64 CPlayer::pc_UseFireCracker(unsigned __int16 wItemSerial)
{
  if (CGameObject::GetCurSecNum(this) == -1 || m_bMapLoading)
  {
    return -1;
  }
  if (m_bCorpse)
  {
    return -2;
  }
  if (m_byStandType == 1)
  {
    return -3;
  }
  if (IsSiegeMode())
  {
    return -4;
  }
  if (IsRidingUnit())
  {
    return -5;
  }
  if (_effect_parameter::GetEff_State(&m_EP, 20))
  {
    return -6;
  }
  if (_effect_parameter::GetEff_State(&m_EP, 28))
  {
    return -6;
  }

  _STORAGE_LIST::_db_con *item = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, wItemSerial);
  if (!item)
  {
    SendMsg_AdjustAmountInform(0, wItemSerial, 0);
    return -7;
  }
  if (item->m_byTableCode != 32)
  {
    return -7;
  }
  if (!CRecordData::GetRecord(&g_Main.m_tblItemData[32], item->m_wItemIndex))
  {
    return -7;
  }
  if (item->m_bLock)
  {
    return -8;
  }

  Emb_AlterDurPoint(0, item->m_byStorageIndex, -1, false, false);
  CMgrAvatorItemHistory::consume_del_item(
    &CPlayer::s_MgrItemHistory,
    m_ObjID.m_wIndex,
    item,
    m_szItemHistoryFileName);
  return item->m_wItemIndex;
}

unsigned __int8 CPlayer::pc_UserSoccerBall(unsigned __int16 wItemSerial, unsigned __int16 *wItemIndex)
{
  if (CGameObject::GetCurSecNum(this) == -1 || m_bMapLoading)
  {
    return 1;
  }
  if (m_bCorpse)
  {
    return 2;
  }
  if (m_byStandType == 1)
  {
    return 3;
  }
  if (IsSiegeMode())
  {
    return 4;
  }
  if (IsRidingUnit())
  {
    return 5;
  }
  if (_effect_parameter::GetEff_State(&m_EP, 20))
  {
    return 6;
  }
  if (_effect_parameter::GetEff_State(&m_EP, 28))
  {
    return 6;
  }

  _STORAGE_LIST::_db_con *item = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, wItemSerial);
  if (!item)
  {
    SendMsg_AdjustAmountInform(0, wItemSerial, 0);
    return 7;
  }
  if (item->m_byTableCode != 27)
  {
    return 7;
  }
  if (!CRecordData::GetRecord(&g_Main.m_tblItemData[27], item->m_wItemIndex))
  {
    return 7;
  }
  if (item->m_bLock)
  {
    return 8;
  }

  m_bTakeSoccerBall = !m_bTakeSoccerBall;
  *wItemIndex = item->m_wItemIndex;
  m_pSoccerItem = m_bTakeSoccerBall ? item : nullptr;
  CashChangeStateFlag changeFlag(0);
  UpdateVisualVer(changeFlag);
  return 0;
}

char CPlayer::pc_UseRadarItem(_STORAGE_POS_INDIV *pItem, unsigned __int16 *pConsumeSerial)
{
  unsigned __int8 errCode = 0;
  CRadarItemMgr *radarMgr = &m_pUserDB->m_RadarItemMgr;
  _STORAGE_LIST *storage = nullptr;
  _STORAGE_LIST::_db_con *radarItem = nullptr;
  _RadarItem_fld *radarFld = nullptr;
  unsigned int delay = 0;
  _STORAGE_LIST::_db_con *consumeItems[6]{};
  int consumeCounts[11]{};
  bool overlapFlags[36]{};

  if (CPlayerDB::GetHP(&m_Param))
  {
    if (IsRidingUnit())
    {
      errCode = 6;
    }
    else if (IsSiegeMode())
    {
      errCode = 7;
    }
    else if (pItem->byStorageCode < 8u)
    {
      storage = m_Param.m_pStoragePtr[pItem->byStorageCode];
      radarItem = _STORAGE_LIST::GetPtrFromSerial(storage, pItem->wItemSerial);
      if (radarItem)
      {
        if (radarItem->m_byTableCode == 34)
        {
          radarFld = (_RadarItem_fld *)CRecordData::GetRecord(&g_Main.m_tblItemData[34], radarItem->m_wItemIndex);
          if (radarFld)
          {
            delay = static_cast<unsigned int>(radarFld->m_fActDelay);
            if (radarFld->m_nDuration <= static_cast<int>(delay))
            {
              if (radarItem->m_bLock)
              {
                errCode = 4;
              }
              else
              {
                CUserDB::CalcRadarDelay(m_pUserDB);
                if (CRadarItemMgr::IsUse(radarMgr))
                {
                  errCode = 8;
                }
                else
                {
                  memset_0(consumeItems, 0, sizeof(consumeItems));
                  memset_0(consumeCounts, 0, sizeof(consumeCounts));
                  memset_0(overlapFlags, 0, sizeof(overlapFlags));
                  if (GetUseConsumeItem(
                        radarFld->m_ConsumeItemList,
                        pConsumeSerial,
                        consumeItems,
                        consumeCounts,
                        overlapFlags))
                  {
                    if (rand() % 100 < radarFld->m_nSucPro)
                    {
                      DeleteUseConsumeItem(consumeItems, consumeCounts, overlapFlags);
                    }
                    else
                    {
                      errCode = 9;
                      Emb_AlterDurPoint(storage->m_nListCode, radarItem->m_byStorageIndex, -1, false, false);
                      CMgrAvatorItemHistory::consume_del_item(
                        &CPlayer::s_MgrItemHistory,
                        m_ObjID.m_wIndex,
                        radarItem,
                        m_szItemHistoryFileName);
                    }
                  }
                  else
                  {
                    errCode = 11;
                  }
                }
              }
            }
            else
            {
              errCode = 10;
            }
          }
          else
          {
            errCode = 2;
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
      errCode = 1;
    }
  }
  else
  {
    errCode = 3;
  }

  SendMsg_UseRadarResult(errCode, pItem->wItemSerial, delay);
  if (!errCode)
  {
    Emb_AlterDurPoint(storage->m_nListCode, radarItem->m_byStorageIndex, -1, false, false);
    CRadarItemMgr::SetUseRadar(
      radarMgr,
      reinterpret_cast<char (*)[64]>(radarFld->m_strCode),
      this,
      radarFld->m_nDuration,
      delay);
    CUserDB::SetRadarDelay(m_pUserDB, delay);
    CRadarItemMgr::RadarProc(radarMgr, radarFld);
    SendMsg_RadarCharSearchResult();
  }
  return 1;
}

char CPlayer::pc_RadarCharInfo()
{
  if (!m_pUserDB)
  {
    return 0;
  }
  CRadarItemMgr *radarMgr = &m_pUserDB->m_RadarItemMgr;
  if (CRadarItemMgr::IsUse(radarMgr))
  {
    _RadarItem_fld *radarFld = (_RadarItem_fld *)CRecordData::GetRecord(
      &g_Main.m_tblItemData[34],
      radarMgr->m_strRadarCode);
    if (!radarFld)
    {
      return 0;
    }
    CRadarItemMgr::RadarProc(radarMgr, radarFld);
    SendMsg_RadarCharSearchResult();
  }
  return 1;
}

char CPlayer::pc_NPCLinkCheckItemRequest(_STORAGE_POS_INDIV *pStorage)
{
  const unsigned __int8 result = pc_NPCLinkCheckItemRequest_Use(pStorage);
  SendMsg_NPCLinkItemCheckResult(result, pStorage);
  return 1;
}

unsigned __int8 CPlayer::pc_NPCLinkCheckItemRequest_Check(_STORAGE_POS_INDIV *pStorage)
{
  if (!pStorage || pStorage->byStorageCode >= 8u)
  {
    return 1;
  }

  _STORAGE_LIST *storage = m_Param.m_pStoragePtr[pStorage->byStorageCode];
  _STORAGE_LIST::_db_con *item = _STORAGE_LIST::GetPtrFromSerial(storage, pStorage->wItemSerial);
  if (!item || !storage)
  {
    return 1;
  }
  if (TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, m_id.wIndex) == 99)
  {
    return 13;
  }
  if (m_bInGuildBattle)
  {
    return 5;
  }
  if (m_byStandType == 1)
  {
    return 6;
  }
  if (IsSiegeMode())
  {
    return 7;
  }
  if (IsRidingUnit())
  {
    return 8;
  }
  if (m_pCurMap->m_pMapSet->m_nMapType == 2)
  {
    return 9;
  }
  if (CGameObject::GetCurSecNum(this) == -1 || m_bMapLoading)
  {
    return 10;
  }
  if (m_bCorpse)
  {
    return 11;
  }
  if (_effect_parameter::GetEff_State(&m_EP, 20))
  {
    return 12;
  }
  if (_effect_parameter::GetEff_State(&m_EP, 28))
  {
    return 12;
  }
  if (!item)
  {
    return 2;
  }
  if (item->m_byTableCode != 35)
  {
    return 2;
  }
  if (!CRecordData::GetRecord(&g_Main.m_tblItemData[35], item->m_wItemIndex))
  {
    return 3;
  }
  if (item->m_dwDur)
  {
    return 0;
  }
  return 4;
}

unsigned __int8 CPlayer::pc_NPCLinkCheckItemRequest_Use(_STORAGE_POS_INDIV *pStorage)
{
  unsigned __int8 result = pc_NPCLinkCheckItemRequest_Check(pStorage);
  if (!result)
  {
    _STORAGE_LIST *storage = m_Param.m_pStoragePtr[pStorage->byStorageCode];
    _STORAGE_LIST::_db_con *useItem = _STORAGE_LIST::GetPtrFromSerial(storage, pStorage->wItemSerial);
    _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblItemData[35], useItem->m_wItemIndex);
    if (record)
    {
      if (!*reinterpret_cast<int *>(&record[5].m_strCode[56]))
      {
        const int oldDur = static_cast<int>(useItem->m_dwDur);
        const unsigned int left = Emb_AlterDurPoint(storage->m_nListCode, useItem->m_byStorageIndex, -1, false, false);
        pStorage->byNum = static_cast<unsigned __int8>(left);
        if (oldDur - 1 == static_cast<int>(left))
        {
          if (*reinterpret_cast<int *>(&record[7].m_strCode[8]) == 1)
          {
            CMgrAvatorItemHistory::cash_item_use(
              &CPlayer::s_MgrItemHistory,
              m_ObjID.m_wIndex,
              useItem,
              m_szItemHistoryFileName);
          }
          result = 0;
          pStorage->wItemSerial = useItem->m_wItemIndex;
        }
        else
        {
          return 4;
        }
      }
    }
    else
    {
      return 3;
    }
  }
  return result;
}

void CPlayer::pc_TalikCrystalExchange(unsigned __int8 byExchangeNum, _talik_crystal_exchange_clzo::_list *pList)
{
  CTalkCrystalCombineManager *manager = CTalkCrystalCombineManager::Instance();
  CTalkCrystalCombineManager::Doit(manager, this, byExchangeNum, pList);
}

unsigned __int8 CPlayer::pc_RenameItemNConditionCheck(_STORAGE_POS_INDIV *pItemInfo, _STORAGE_LIST::_db_con **ppItem)
{
  if (!pItemInfo || *ppItem)
  {
    return 1;
  }

  _STORAGE_LIST *storage = m_Param.m_pStoragePtr[pItemInfo->byStorageCode];
  _STORAGE_LIST::_db_con *item = _STORAGE_LIST::GetPtrFromSerial(storage, pItemInfo->wItemSerial);
  if (!item)
  {
    return 1;
  }
  if (!CPlayerDB::GetHP(&m_Param))
  {
    return 2;
  }
  if (item->m_byTableCode != 13)
  {
    return 9;
  }
  if (m_Param.m_pGuild)
  {
    const unsigned int mySerial = CPlayerDB::GetCharSerial(&m_Param);
    const unsigned int guildMasterSerial = CGuild::GetGuildMasterSerial(m_Param.m_pGuild);
    if (mySerial == guildMasterSerial)
    {
      return 7;
    }
  }
  const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
  CPvpUserAndGuildRankingSystem *rankSystem = CPvpUserAndGuildRankingSystem::Instance();
  if (CPvpUserAndGuildRankingSystem::IsCurrentRaceBossGroup(rankSystem, raceCode, m_dwObjSerial))
  {
    return 8;
  }
  *ppItem = item;
  return 0;
}

char CPlayer::pc_CharacterRenameCash(bool bChange, _STORAGE_POS_INDIV *pItem, const char *strCharacterName)
{
  if (!pItem)
  {
    return 0;
  }

  _STORAGE_POS_INDIV itemInfo{};
  if (bChange)
  {
    memcpy_s(&itemInfo, sizeof(itemInfo), &m_ReNamePotionUseInfo, sizeof(itemInfo));
  }
  else
  {
    memcpy_s(&itemInfo, sizeof(itemInfo), pItem, sizeof(itemInfo));
  }
  if (itemInfo.byStorageCode)
  {
    return 0;
  }

  _STORAGE_LIST::_db_con *item = nullptr;
  unsigned __int8 errCode = pc_RenameItemNConditionCheck(&itemInfo, &item);
  if (errCode)
  {
    SendMsg_CharacterRenameCashResult(bChange, errCode);
    return 0;
  }
  if (!item)
  {
    SendMsg_CharacterRenameCashResult(bChange, 1u);
    return 0;
  }

  const int cashType = GetUsePcCashType(item->m_byTableCode, item->m_wItemIndex);
  if (!IsUsableAccountType(cashType))
  {
    SendMsg_PremiumCashItemUse(0xFFFFu);
    return 1;
  }

  if (bChange)
  {
    if (m_ReNamePotionUseInfo.wszChangeName[0])
    {
      const unsigned int currTime = timeGetTime();
      _PotionItem_fld *potionFld = (_PotionItem_fld *)CRecordData::GetRecord(&g_Main.m_tblItemData[13], item->m_wItemIndex);
      if (potionFld)
      {
        _base_fld *effectRecord = CRecordData::GetRecord(&g_PotionMgr.m_tblPotionEffectData, potionFld->m_strEffCode);
        if (effectRecord && !errCode && effectRecord[13].m_dwIndex == 47)
        {
          _STORAGE_LIST::_storage_con::lock(item, true);
          errCode = CPotionMgr::UsePotion(&g_PotionMgr, this, this, potionFld, currTime);
          if (errCode)
          {
            _RENAME_POTION_USE_INFO::Init(&m_ReNamePotionUseInfo);
            SendMsg_CharacterRenameCashResult(1, errCode);
            return 0;
          }
          return 1;
        }
        _RENAME_POTION_USE_INFO::Init(&m_ReNamePotionUseInfo);
        SendMsg_CharacterRenameCashResult(1, 9u);
        return 0;
      }

      _RENAME_POTION_USE_INFO::Init(&m_ReNamePotionUseInfo);
      char *charName = CPlayerDB::GetCharNameA(&m_Param);
      CLogFile::Write(
        &g_Main.m_logSystemError,
        "CPlayer::pc_CharacterRenameCash(...) : User %s(%u) Item Table(%u) Index(%u) Not Exist!",
        charName,
        m_dwObjSerial,
        item->m_byTableCode,
        item->m_wItemIndex);
      errCode = 48;
      return 0;
    }

    _RENAME_POTION_USE_INFO::Init(&m_ReNamePotionUseInfo);
    SendMsg_CharacterRenameCashResult(1, 0x30u);
    return 0;
  }

  errCode = pc_CharacterRenameCheck(strCharacterName);
  if (errCode)
  {
    SendMsg_CharacterRenameCashResult(0, errCode);
  }
  else
  {
    _STORAGE_LIST::_storage_con::lock(item, true);
    _qry_case_character_rename qry{};
    qry.dwCharSerial = m_pUserDB->m_dwSerial;
    qry.dwAlreadySerial = static_cast<unsigned int>(-1);
    memcpy_s(&qry.ItemInfo, sizeof(qry.ItemInfo), &itemInfo, sizeof(itemInfo));
    strcpy_s(qry.wszCharName, 0x11u, strCharacterName);
    const int size = static_cast<int>(qry.size());
    if (!CMainThread::PushDQSData(&g_Main, 0xFFFFFFFF, nullptr, 0x91u, reinterpret_cast<char *>(&qry), size))
    {
      SendMsg_CharacterRenameCashResult(0, 0xAu);
    }
  }
  return 1;
}

char CPlayer::pc_CharacterRenameCheck(const char *strCharacterName)
{
  char errCode = 0;
  char dest[17]{};
  strncpy_s<17>(reinterpret_cast<char (*)[17]>(dest), strCharacterName, 0x10u);
  if (dest[0] == 42 || dest[0] == 33)
  {
    errCode = 5;
  }
  for (int index = 0; index < 3; ++index)
  {
    if (!strcmp_0(dest, wszNonMakeName[index]))
    {
      errCode = 6;
      break;
    }
  }

  CNationSettingManager *nationSetting = CTSingleton<CNationSettingManager>::Instance();
  if (!CNationSettingManager::IsNormalString(nationSetting, dest))
  {
    return 5;
  }
  return errCode;
}

char CPlayer::pc_SetItemCheckRequest(
  unsigned int dwSetItem,
  unsigned __int8 bySetItemNum,
  unsigned __int8 bySetEffectNum,
  bool bSet)
{
  CSUItemSystem *suSystem = CSUItemSystem::Instance();
  CSetItemType *setType = CSUItemSystem::GetCSetItemType(suSystem);
  if (!setType)
  {
    SendMsg_SetItemCheckResult(7u, 0, 0);
    return 0;
  }

  si_interpret *si = CSetItemType::Getsi_interpret(setType, dwSetItem);
  if (!si)
  {
    SendMsg_SetItemCheckResult(2u, 0, 0);
    return 0;
  }

  int result = 9;
  if (bSet)
  {
    bool gradeOk = false;
    char strCode[68]{};
    const int tableCode = CSUItemSystem::GetSetItemTableInfo(suSystem, dwSetItem, strCode, 64);
    if (tableCode > -1)
    {
      const unsigned __int8 grade = GetItemEquipGrade(tableCode, strCode);
      if (IsEquipAbleGrade(grade))
      {
        gradeOk = true;
      }
    }
    if (gradeOk)
    {
      result = CSetItemEffect::SetOnEffect(&m_clsSetItem, &m_pUserDB->m_AvatorData, dwSetItem, bySetItemNum, bySetEffectNum);
      switch (result)
      {
        case 0:
          ApplySetItemEffect(si, dwSetItem, bySetItemNum, bySetEffectNum, true);
          break;
        case 2:
        case 3:
        case 4:
        case 7:
          break;
        case 8:
        {
          const unsigned __int8 resetEffect = CSetItemEffect::GetResetEffectNum(&m_clsSetItem);
          const unsigned __int8 resetItem = CSetItemEffect::GetResetItemNum(&m_clsSetItem);
          const unsigned int resetIdx = CSetItemEffect::GetResetIdx(&m_clsSetItem);
          ApplySetItemEffect(si, resetIdx, resetItem, resetEffect, false);
          ApplySetItemEffect(si, dwSetItem, bySetItemNum, bySetEffectNum, true);
          break;
        }
        default:
          result = 9;
          break;
      }
    }
  }
  else
  {
    result = CSetItemEffect::SetOffEffect(&m_clsSetItem, dwSetItem, bySetItemNum, bySetEffectNum);
    if (result == 1)
    {
      ApplySetItemEffect(si, dwSetItem, bySetItemNum, bySetEffectNum, false);
    }
  }

  SendMsg_SetItemCheckResult(static_cast<char>(result), dwSetItem, bySetEffectNum);
  return 1;
}

void CPlayer::UpdateLastMetalTicket(
  unsigned __int16 byCurrentYear,
  unsigned __int8 byCurrentMonth,
  unsigned __int8 byCurrentDay,
  unsigned __int8 byCurrentHour,
  unsigned __int8 byNumOfTime)
{
  if (!m_MinigTicket.AuthLastMentalTicket(
        byCurrentYear,
        byCurrentMonth,
        byCurrentDay,
        byCurrentHour,
        byNumOfTime))
  {
    m_MinigTicket.SetLastMentalTicket(byCurrentYear, byCurrentMonth, byCurrentDay, byCurrentHour, byNumOfTime);
    if (m_pUserDB)
    {
      const unsigned int lastMentalTicket = m_MinigTicket.GetLastMentalTicket();
      m_pUserDB->Update_TakeLastMentalTicket(lastMentalTicket);
    }
    SendMsg_HSKQuestActCum();
  }
}

void CPlayer::CheckMentalTakeAndUpdateLastMetalTicket(const char *strItemCode)
{
  if (!strcmp_0(g_HolySys.m_strHolyMental, strItemCode))
  {
    const unsigned __int8 byNumOfTime = g_HolySys.GetNumOfTime();
    const unsigned __int8 byCurrentHour = g_HolySys.GetStartHour();
    const unsigned __int8 startDay = g_HolySys.GetStartDay();
    const unsigned __int8 startMonth = g_HolySys.GetStartMonth();
    const unsigned __int16 startYear = g_HolySys.GetStartYear();
    UpdateLastMetalTicket(startYear, startMonth, startDay, byCurrentHour, byNumOfTime);
  }
}

void CPlayer::SendMsg_MakeItemCheatSendButtonEnable(char bEnableSendButton)
{
  char msg[1]{};
  msg[0] = bEnableSendButton;
  unsigned __int8 type[2] = {7, 34};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_AlterMoneyInform(char byReasonCode)
{
#pragma pack(push, 1)
  struct AlterMoneyMsg
  {
    char byReasonCode;
    unsigned int dwGold;
    unsigned int dwDalant;
  };
#pragma pack(pop)

  AlterMoneyMsg msg{};
  msg.byReasonCode = byReasonCode;
  msg.dwGold = m_Param.GetGold();
  msg.dwDalant = m_Param.GetDalant();

  unsigned __int8 type[2] = {11, 19};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 9u);
}

void CPlayer::RecallRandomPositionInRange(
  CMapData *pIntoMap,
  unsigned __int16 wMapLayerIndex,
  float *pStartPos,
  int iRange)
{
  if (pStartPos)
  {
    pc_Resurrect(0);

    float newPos[3]{};
    if (!pIntoMap->GetRandPosInRange(pStartPos, iRange, newPos))
    {
      memcpy_0(newPos, pStartPos, sizeof(newPos));
    }

    OutOfMap(pIntoMap, wMapLayerIndex, 4u, newPos);
    const unsigned __int8 mapCode = m_Param.GetMapCode();
    SendMsg_GotoRecallResult(0, mapCode, newPos, 4u);
  }
}

void CPlayer::SendMsg_HSKQuestActCum()
{
  struct _hsk_act_cum
  {
    int nHSKPvpPoint;
    unsigned __int16 wKillPoint;
    bool bAuthTicket;
  };

  _hsk_act_cum msg{};
  msg.nHSKPvpPoint = m_nHSKPvpPoint;
  msg.wKillPoint = m_wKillPoint;

  const unsigned __int8 byNumOfTime = g_HolySys.GetNumOfTime();
  const unsigned __int8 byCurrentHour = g_HolySys.GetStartHour();
  const unsigned __int8 startDay = g_HolySys.GetStartDay();
  const unsigned __int8 startMonth = g_HolySys.GetStartMonth();
  const unsigned __int16 startYear = g_HolySys.GetStartYear();
  msg.bAuthTicket =
    m_MinigTicket.AuthLastMentalTicket(startYear, startMonth, startDay, byCurrentHour, byNumOfTime) != 0;

  unsigned __int8 type[2] = {25, 12};
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    7u);
}

void CPlayer::SendMsg_ProposeVoteResult(char byRetCode)
{
  char msg[1]{};
  msg[0] = byRetCode;

  unsigned __int8 type[2] = {26, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::AddGold(int dwPush, bool bApply)
{
  if (bApply)
  {
    const double penalty = g_Main.m_pTimeLimitMgr->GetPlayerPenalty(m_id.wIndex);
    dwPush = static_cast<int>(static_cast<double>(dwPush) * penalty);
  }

  unsigned int newGold = dwPush + m_Param.GetGold();
  const unsigned int currentGold = m_Param.GetGold();
  if (!CanAddMoneyForMaxLimGold(static_cast<unsigned int>(dwPush), currentGold))
  {
    newGold = 500000;
  }

  if (newGold != m_Param.GetGold())
  {
    m_Param.SetGold(newGold);
    const unsigned int dalant = m_Param.GetDalant();
    m_pUserDB->Update_Money(dalant, m_Param.GetGold());
  }
}

void CPlayer::SubGold(unsigned int dwSub)
{
  unsigned int newGold = m_Param.GetGold() - dwSub;
  if (dwSub > m_Param.GetGold())
  {
    newGold = 0;
  }

  if (newGold != m_Param.GetGold())
  {
    m_Param.SetGold(newGold);
    const unsigned int dalant = m_Param.GetDalant();
    m_pUserDB->Update_Money(dalant, m_Param.GetGold());
  }
}

void CPlayer::AlterGold(double dGold)
{
  if (dGold <= 0.0)
  {
    if (dGold < 0.0)
    {
      SubGold(static_cast<unsigned int>(static_cast<int>(-0.0 - dGold)));
    }
  }
  else
  {
    AddGold(static_cast<int>(dGold), true);
  }
}

void CPlayer::SetPotionActDelay(unsigned __int8 byPotionClass, unsigned int dwCurrTime, unsigned int dwActDelay)
{
  m_PotionParam.SetPotionActDelay(byPotionClass, dwCurrTime, dwActDelay);
  if (m_pUserDB)
  {
    m_pUserDB->Update_PotionNextUseTime(byPotionClass, dwActDelay + dwCurrTime);
  }
}

void CPlayer::SendMsg_GuildSelfLeaveResult(char byRetCode)
{
  char msg[1]{};
  msg[0] = byRetCode;

  unsigned __int8 type[2] = {27, 17};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::pc_GuildSelfLeaveRequest()
{
  char retCode = 0;
  CGuild *guild = m_Param.m_pGuild;
  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    retCode = 106;
  }
  else if (m_Param.m_bGuildLock)
  {
    retCode = static_cast<char>(-51);
  }
  else if (guild)
  {
    if (guild->m_bRankWait)
    {
      retCode = 71;
    }
    else if (guild->m_bNowProcessSgtMter
             && !guild->m_SuggestedMatter.byMatterType
             && guild->m_SuggestedMatter.dwMatterDst == m_dwObjSerial)
    {
      retCode = 81;
    }
  }
  else
  {
    retCode = static_cast<char>(-54);
  }

  if (!retCode)
  {
    char pushed = 1;
    _qry_case_selfleave qry{};
    qry.in_leaverserial = m_dwObjSerial;
    qry.tmp_leaverindex = m_ObjID.m_wIndex;
    qry.tmp_guildindex = guild->m_nIndex;
    qry.tmp_guildserial = guild->m_dwSerial;
    qry.in_MemberNum = static_cast<int>(guild->GetMemberNum()) - 1;

    const int size = qry.size();
    if (!g_Main.PushDQSData(0xFFFFFFFF, &m_id, 0x11u, reinterpret_cast<char *>(&qry), size))
    {
      pushed = 0;
    }
    if (pushed)
    {
      m_Param.m_bGuildLock = true;
    }
    else
    {
      retCode = static_cast<char>(-1);
    }
  }

  if (retCode)
  {
    SendMsg_GuildSelfLeaveResult(retCode);
  }
  if (!retCode)
  {
    if (m_GroupTargetObject[1].pObject)
    {
      m_GroupTargetObject[1].init();
      SendMsg_ReleaseGroupTargetObjectResult(1u);
    }
  }
}

void CPlayer::SendMsg_UpdateTLStatusInfo(unsigned int dwFatigue, unsigned __int8 wStatus)
{
  unsigned char msg[14]{};

  const unsigned int fatigue = m_pUserDB->m_AvatorData.dbTimeLimitInfo.dwFatigue;
  const unsigned __int16 tlStatus = m_pUserDB->m_AvatorData.dbTimeLimitInfo.byTLStatus;

  std::memcpy(msg, &fatigue, sizeof(fatigue));
  std::memcpy(msg + 4, &tlStatus, sizeof(tlStatus));

  const unsigned int playFDegree = g_Main.m_pTimeLimitMgr->GetPlayFDegree();
  const unsigned int playedSeconds = playFDegree / 0x3E8u * fatigue;

  const unsigned __int16 endPlayTime = g_Main.m_pTimeLimitMgr->GetEndPlayTime();
  unsigned int remainSeconds = 0;
  if (playedSeconds < 60u * endPlayTime)
  {
    const unsigned __int16 endPlayTime2 = g_Main.m_pTimeLimitMgr->GetEndPlayTime();
    remainSeconds = 60u * endPlayTime2 - playedSeconds;
  }

  std::memcpy(msg + 6, &remainSeconds, sizeof(remainSeconds));
  std::memcpy(msg + 10, &playedSeconds, sizeof(playedSeconds));

  unsigned __int8 pbyType[2]{};
  pbyType[0] = 13;
  pbyType[1] = static_cast<unsigned __int8>(-112);
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(msg), 0xEu);
  (void)dwFatigue;
  (void)wStatus;
}

void CPlayer::SendMsg_RaceTopInform(char bTop)
{
  char msg[1]{};
  msg[0] = bTop;

  unsigned __int8 type[2] = {3, 47};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_PostSendReply(unsigned __int8 byErrCode)
{
  _post_result_zocl msg{};
  msg.byErrCode = byErrCode;
  if (!byErrCode)
  {
    msg.dwGold = m_Param.GetGold();
  }

  unsigned __int8 pbyType[2] = {58, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), 5u);
}

void CPlayer::UpdatePost(unsigned int dwIndex)
{
  if (dwIndex >= 0x32)
  {
    return;
  }

  CPostData *postData = m_Param.m_PostStorage.GetPostDataFromInx(dwIndex);
  if (postData)
  {
    int key = postData->m_Key.CovDBKey();
    m_pUserDB->Update_Post(
      dwIndex,
      postData->m_dwPSSerial,
      postData->m_nNumber,
      postData->m_byState,
      key,
      postData->m_dwDur,
      postData->m_dwUpt,
      postData->m_dwGold,
      postData->m_lnUID);
    return;
  }

  CPostSystemManager *manager = CPostSystemManager::Instace();
  manager->Log("CPlayer::UpdatePost() : pPost Is Null : Index(%u)", dwIndex);
}

void CPlayer::Emb_EquipLink()
{
  for (int linkIndex = 0; linkIndex < 50; ++linkIndex)
  {
    _quick_link *link = &m_Param.m_QLink[linkIndex];
    if (link->byLinkIndex != 0xFF)
    {
      unsigned __int8 storageCode = 0;
      _STORAGE_LIST::_db_con *item = CPlayerDB::GetPtrItemStorage(&m_Param, link->wSerial, &storageCode);
      if (item)
      {
        unsigned __int16 storageIndex = 0;
        storageIndex = static_cast<unsigned __int16>(item->m_byStorageIndex | (storageCode << 8));
        CUserDB::Update_LinkBoardSlot(m_pUserDB, linkIndex, 4u, storageIndex);
      }
      else
      {
        CPlayerDB::PopLink(&m_Param, linkIndex);
        CUserDB::Update_LinkBoardSlot(m_pUserDB, linkIndex, 0xFFu, 0xFFFFu);
      }
    }
  }
}

void CPlayer::pc_ForceInvenChange(_STORAGE_POS_INDIV *pItem, unsigned __int16 wReplaceSerial)
{
  unsigned __int8 errCode = 0;
  _STORAGE_LIST::_db_con *sourceItem = nullptr;
  _STORAGE_LIST::_db_con *replaceItem = nullptr;

  _STORAGE_LIST *sourceList = m_Param.m_pStoragePtr[pItem->byStorageCode];
  _STORAGE_LIST *targetList = nullptr;
  if (pItem->byStorageCode)
  {
    if (pItem->byStorageCode != 3)
    {
      return;
    }
    targetList = m_Param.m_pStoragePtr[0];
  }
  else
  {
    targetList = m_Param.m_pStoragePtr[3];
  }

  sourceItem = _STORAGE_LIST::GetPtrFromSerial(sourceList, pItem->wItemSerial);
  if (sourceItem)
  {
    if (sourceItem->m_byTableCode == 15)
    {
      if (sourceItem->m_bLock)
      {
        errCode = 11;
      }
      else if (m_pCurMap->m_pMapSet->m_nMapType != 1)
      {
        if (wReplaceSerial != 0xFFFF)
        {
          replaceItem = _STORAGE_LIST::GetPtrFromSerial(targetList, wReplaceSerial);
          if (!replaceItem)
          {
            errCode = 2;
            goto RESULT_FORCE_INVEN_CHANGE;
          }
          if (replaceItem->m_byTableCode != 15)
          {
            errCode = 2;
            goto RESULT_FORCE_INVEN_CHANGE;
          }
          if (replaceItem->m_bLock)
          {
            errCode = 11;
            goto RESULT_FORCE_INVEN_CHANGE;
          }
          if (CPlayer::s_pnLinkForceItemToEffect[sourceItem->m_wItemIndex]
              != CPlayer::s_pnLinkForceItemToEffect[replaceItem->m_wItemIndex])
          {
            errCode = 3;
            goto RESULT_FORCE_INVEN_CHANGE;
          }
        }
        if (wReplaceSerial == 0xFFFF && _STORAGE_LIST::GetIndexEmptyCon(targetList) == 0xFF)
        {
          errCode = 5;
        }
        else if (wReplaceSerial == 0xFFFF && !pItem->byStorageCode)
        {
          for (int j = 0; j < targetList->m_nUsedNum; ++j)
          {
            _STORAGE_LIST::_db_con *entry = &targetList->m_pStorageList[j];
            if (entry->m_bLoad
                && CPlayer::s_pnLinkForceItemToEffect[entry->m_wItemIndex]
                     == CPlayer::s_pnLinkForceItemToEffect[sourceItem->m_wItemIndex])
            {
              errCode = 4;
              goto RESULT_FORCE_INVEN_CHANGE;
            }
          }
        }
      }
      else
      {
        errCode = 12;
      }
    }
    else
    {
      errCode = 1;
    }
  }
  else
  {
    errCode = 1;
  }

RESULT_FORCE_INVEN_CHANGE:
  if (!errCode)
  {
    _STORAGE_LIST::_db_con srcCopy{};
    memcpy_0(&srcCopy, sourceItem, sizeof(srcCopy));
    if (!Emb_DelStorage(
          sourceList->m_nListCode,
          sourceItem->m_byStorageIndex,
          0,
          0,
          "CPlayer::pc_ForceInvenChange() -- 0"))
    {
      SendMsg_ForceInvenChange(0xFFu);
      return;
    }
    if (replaceItem)
    {
      _STORAGE_LIST::_db_con replaceCopy{};
      memcpy_0(&replaceCopy, replaceItem, sizeof(replaceCopy));
      if (!Emb_AddStorage(sourceList->m_nListCode, &replaceCopy, 1, 0))
      {
LABEL_FORCE_INVEN_RESTORE:
        Emb_AddStorage(sourceList->m_nListCode, &srcCopy, 1, 0);
        SendMsg_ForceInvenChange(0xFFu);
        return;
      }
      if (!Emb_DelStorage(
            targetList->m_nListCode,
            replaceItem->m_byStorageIndex,
            0,
            0,
            "CPlayer::pc_ForceInvenChange() -- 1"))
      {
        Emb_DelStorage(sourceList->m_nListCode, replaceItem->m_byStorageIndex, 0, 0, nullptr);
        Emb_AddStorage(sourceList->m_nListCode, &srcCopy, 1, 0);
        SendMsg_ForceInvenChange(0xFFu);
        return;
      }
    }
    if (replaceItem)
    {
      _STORAGE_LIST::_db_con restoreCopy{};
      memcpy_0(&restoreCopy, replaceItem, sizeof(restoreCopy));
      Emb_AddStorage(targetList->m_nListCode, &restoreCopy, 1, 0);
      Emb_DelStorage(sourceList->m_nListCode, replaceItem->m_byStorageIndex, 0, 0, nullptr);
      Emb_AddStorage(sourceList->m_nListCode, &srcCopy, 1, 0);
      SendMsg_ForceInvenChange(0xFFu);
      return;
    }
    if (!Emb_AddStorage(targetList->m_nListCode, &srcCopy, 1, 0))
    {
      goto LABEL_FORCE_INVEN_RESTORE;
    }
    Emb_EquipLink();
  }
  SendMsg_ForceInvenChange(errCode);
}

void CPlayer::SendMsg_ForceInvenChange(char byErrCode)
{
  char msg[1]{};
  msg[0] = byErrCode;

  unsigned __int8 pbyType[2] = {13, 3};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    pbyType,
    msg,
    1u);
}

void CPlayer::pc_AnimusInvenChange(_STORAGE_POS_INDIV *pItem, unsigned __int16 wReplaceSerial)
{
  unsigned __int8 errCode = 0;
  _STORAGE_LIST::_db_con *sourceItem = nullptr;
  _STORAGE_LIST::_db_con *replaceItem = nullptr;
  _base_fld *record = nullptr;

  _STORAGE_LIST *sourceList = m_Param.m_pStoragePtr[pItem->byStorageCode];
  _STORAGE_LIST *targetList = nullptr;
  if (pItem->byStorageCode)
  {
    if (pItem->byStorageCode != 4)
    {
      return;
    }
    targetList = m_Param.m_pStoragePtr[0];
  }
  else
  {
    targetList = m_Param.m_pStoragePtr[4];
  }

  sourceItem = _STORAGE_LIST::GetPtrFromSerial(sourceList, pItem->wItemSerial);
  if (sourceItem)
  {
    if (sourceItem->m_byTableCode == 24)
    {
      if (sourceItem->m_bLock)
      {
        errCode = 11;
      }
      else if (sourceItem == m_pRecalledAnimusItem)
      {
        errCode = 6;
      }
      else if (m_pCurMap->m_pMapSet->m_nMapType != 1)
      {
        if (wReplaceSerial != 0xFFFF)
        {
          replaceItem = _STORAGE_LIST::GetPtrFromSerial(targetList, wReplaceSerial);
          if (!replaceItem)
          {
            errCode = 2;
            goto RESULT_ANIMUS_INVEN_CHANGE;
          }
          if (replaceItem->m_byTableCode != 24)
          {
            errCode = 2;
            goto RESULT_ANIMUS_INVEN_CHANGE;
          }
          if (replaceItem->m_bLock)
          {
            errCode = 11;
            goto RESULT_ANIMUS_INVEN_CHANGE;
          }
          if (replaceItem->m_wItemIndex != sourceItem->m_wItemIndex)
          {
            errCode = 3;
            goto RESULT_ANIMUS_INVEN_CHANGE;
          }
          if (replaceItem == m_pRecalledAnimusItem)
          {
            errCode = 6;
            goto RESULT_ANIMUS_INVEN_CHANGE;
          }
        }
        if (wReplaceSerial == 0xFFFF && _STORAGE_LIST::GetIndexEmptyCon(targetList) == 0xFF)
        {
          errCode = 5;
        }
        else if (wReplaceSerial == 0xFFFF && !pItem->byStorageCode)
        {
          record = CRecordData::GetRecord(&g_Main.m_tblItemData[24], sourceItem->m_wItemIndex);
          if (record)
          {
            bool allowDuplicateCheck = false;
            if (m_bFreeSFByClass)
            {
              allowDuplicateCheck = true;
            }
            else
            {
              bool allowByClass = false;
              if (*reinterpret_cast<unsigned int *>(&record[3].m_strCode[60]) == 1)
              {
                if (m_Param.m_pClassHistory[0] && m_Param.m_pClassHistory[0]->m_nClass == 3)
                {
                  allowByClass = (m_Param.m_pClassData->m_nClass == 3);
                }
              }
              else
              {
                for (int j = 0; j < 4; ++j)
                {
                  _class_fld *historyEffect = *m_Param.m_ppHistoryEffect[j];
                  if (!historyEffect)
                  {
                    break;
                  }
                  if (*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(historyEffect) + 1440))
                  {
                    allowByClass = true;
                    break;
                  }
                }
              }
              if (allowByClass)
              {
                allowDuplicateCheck = true;
              }
              else
              {
                errCode = 7;
              }
            }
            if (allowDuplicateCheck)
            {
              for (int j = 0; j < targetList->m_nUsedNum; ++j)
              {
                _STORAGE_LIST::_db_con *entry = &targetList->m_pStorageList[j];
                if (entry->m_bLoad && entry->m_wItemIndex == sourceItem->m_wItemIndex)
                {
                  errCode = 4;
                  goto RESULT_ANIMUS_INVEN_CHANGE;
                }
              }
            }
          }
          else
          {
            errCode = 8;
          }
        }
      }
      else
      {
        errCode = 12;
      }
    }
    else
    {
      errCode = 1;
    }
  }
  else
  {
    errCode = 1;
  }

RESULT_ANIMUS_INVEN_CHANGE:
  if (!errCode)
  {
    _STORAGE_LIST::_db_con srcCopy{};
    memcpy_0(&srcCopy, sourceItem, sizeof(srcCopy));
    if (!Emb_DelStorage(
          sourceList->m_nListCode,
          sourceItem->m_byStorageIndex,
          0,
          0,
          "CPlayer::pc_AnimusInvenChange() -- 0"))
    {
      SendMsg_AnimusInvenChange(0xFFu);
      return;
    }
    if (replaceItem)
    {
      _STORAGE_LIST::_db_con replaceCopy{};
      memcpy_0(&replaceCopy, replaceItem, sizeof(replaceCopy));
      if (!Emb_AddStorage(sourceList->m_nListCode, &replaceCopy, 1, 0))
      {
LABEL_ANIMUS_RESTORE:
        Emb_AddStorage(sourceList->m_nListCode, &srcCopy, 1, 0);
        SendMsg_AnimusInvenChange(0xFFu);
        return;
      }
      if (!Emb_DelStorage(
            targetList->m_nListCode,
            replaceItem->m_byStorageIndex,
            0,
            0,
            "CPlayer::pc_AnimusInvenChange() -- 1"))
      {
        Emb_DelStorage(
          sourceList->m_nListCode,
          replaceItem->m_byStorageIndex,
          0,
          0,
          "CPlayer::pc_AnimusInvenChange() -- 1 Fail");
        Emb_AddStorage(sourceList->m_nListCode, &srcCopy, 1, 0);
        SendMsg_AnimusInvenChange(0xFFu);
        return;
      }
    }
    if (replaceItem)
    {
      _STORAGE_LIST::_db_con restoreCopy{};
      memcpy_0(&restoreCopy, replaceItem, sizeof(restoreCopy));
      if (!Emb_AddStorage(targetList->m_nListCode, &srcCopy, 1, 0))
      {
        Emb_AddStorage(targetList->m_nListCode, &restoreCopy, 1, 0);
        Emb_DelStorage(
          sourceList->m_nListCode,
          replaceItem->m_byStorageIndex,
          0,
          0,
          "CPlayer::pc_AnimusInvenChange() -- 1 Fail");
        Emb_AddStorage(sourceList->m_nListCode, &srcCopy, 1, 0);
        SendMsg_AnimusInvenChange(0xFFu);
        return;
      }
    }
    else if (!Emb_AddStorage(targetList->m_nListCode, &srcCopy, 1, 0))
    {
      goto LABEL_ANIMUS_RESTORE;
    }
    Emb_EquipLink();
  }
  SendMsg_AnimusInvenChange(errCode);
}

void CPlayer::SendMsg_AnimusInvenChange(char byErrCode)
{
  char msg[1]{};
  msg[0] = byErrCode;

  unsigned __int8 pbyType[2] = {13, 5};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    pbyType,
    msg,
    1u);
}

void CPlayer::pc_ResSeparation(unsigned __int16 wStartSerial, unsigned __int8 byMoveAmount)
{
  // this is not a stub
  SendMsg_ResSeparation(1u, nullptr, nullptr);
  (void)wStartSerial;
  (void)byMoveAmount;
}

void CPlayer::SendMsg_ResSeparation(
  char byErrCode,
  _STORAGE_LIST::_db_con *pStartOre,
  _STORAGE_LIST::_db_con *pNewOre)
{
  #pragma pack(push, 1)
  struct ResSeparationMsg
  {
    char byErrCode;
    unsigned __int16 wStartSerial;
    unsigned __int8 byStartDur;
    unsigned __int16 wNewSerial;
    unsigned __int8 byNewDur;
  };
  #pragma pack(pop)

  ResSeparationMsg msg{};
  msg.byErrCode = byErrCode;
  if (!byErrCode)
  {
    msg.wStartSerial = pStartOre->m_wSerial;
    msg.byStartDur = static_cast<unsigned __int8>(pStartOre->m_dwDur);
    msg.wNewSerial = pNewOre->m_wSerial;
    msg.byNewDur = static_cast<unsigned __int8>(pNewOre->m_dwDur);
  }

  unsigned __int8 pbyType[2] = {13, 7};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    7u);
}

void CPlayer::pc_ResDivision(
  unsigned __int16 wStartSerial,
  unsigned __int16 wTarSerial,
  unsigned __int8 byMoveAmount)
{
  unsigned __int8 errCode = 0;
  _STORAGE_LIST::_db_con *startOre = nullptr;
  _STORAGE_LIST::_db_con *targetOre = nullptr;

  startOre = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, wStartSerial);
  if (startOre)
  {
    if (startOre->m_bLock)
    {
      errCode = 11;
    }
    else
    {
      targetOre = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, wTarSerial);
      if (targetOre)
      {
        if (targetOre->m_bLock)
        {
          errCode = 11;
        }
        else if (IsOverLapItem(startOre->m_byTableCode) && IsOverLapItem(targetOre->m_byTableCode))
        {
          if (startOre->m_byCsMethod && targetOre->m_byCsMethod && startOre->m_dwT != targetOre->m_dwT)
          {
            errCode = 23;
          }
          else if (startOre->m_byTableCode == targetOre->m_byTableCode)
          {
            if (startOre->m_wItemIndex == targetOre->m_wItemIndex)
            {
              if (startOre->m_dwDur >= byMoveAmount)
              {
                if (targetOre->m_dwDur + byMoveAmount > 0x63)
                {
                  errCode = 9;
                }
              }
              else
              {
                errCode = 8;
              }
            }
            else
            {
              errCode = 6;
            }
          }
          else
          {
            errCode = 6;
          }
        }
        else
        {
          errCode = 3;
        }
      }
      else
      {
        errCode = 5;
      }
    }
  }
  else
  {
    errCode = 5;
  }

  if (!errCode)
  {
    Emb_AlterDurPoint(0, startOre->m_byStorageIndex, -byMoveAmount, 0, 0);
    Emb_AlterDurPoint(0, targetOre->m_byStorageIndex, byMoveAmount, 0, 0);
  }
  SendMsg_ResDivision(errCode, startOre, targetOre);
}

void CPlayer::SendMsg_ResDivision(
  char byErrCode,
  _STORAGE_LIST::_db_con *pStartOre,
  _STORAGE_LIST::_db_con *pTargetOre)
{
  #pragma pack(push, 1)
  struct ResDivisionMsg
  {
    char byErrCode;
    unsigned __int16 wStartSerial;
    unsigned __int8 byStartDur;
    unsigned __int16 wTargetSerial;
    unsigned __int8 byTargetDur;
  };
  #pragma pack(pop)

  ResDivisionMsg msg{};
  msg.byErrCode = byErrCode;
  if (!byErrCode)
  {
    msg.wStartSerial = pStartOre->m_wSerial;
    msg.byStartDur = pStartOre->m_bLoad ? static_cast<unsigned __int8>(pStartOre->m_dwDur) : 0;
    msg.wTargetSerial = pTargetOre->m_wSerial;
    msg.byTargetDur = static_cast<unsigned __int8>(pTargetOre->m_dwDur);
  }

  unsigned __int8 pbyType[2] = {13, 9};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    7u);
}

void CPlayer::pc_PotionSeparation(unsigned __int16 wSerial, unsigned __int8 byAmount)
{
  // this is not a stub
  SendMsg_PotionSeparation(wSerial, 0, 0xFFu, byAmount, -1);
}

void CPlayer::SendMsg_PotionSeparation(
  unsigned __int16 wParentSerial,
  unsigned __int8 byParentAmount,
  unsigned __int16 wChildSerial,
  unsigned __int8 byChildAmount,
  char nRet)
{
  #pragma pack(push, 1)
  struct PotionSeparationMsg
  {
    char byRet;
    unsigned __int16 wParentSerial;
    unsigned __int8 byParentAmount;
    unsigned __int16 wChildSerial;
    unsigned __int8 byChildAmount;
  };
  #pragma pack(pop)

  PotionSeparationMsg msg{};
  msg.byRet = nRet;
  msg.wParentSerial = wParentSerial;
  msg.byParentAmount = byParentAmount;
  msg.wChildSerial = wChildSerial;
  msg.byChildAmount = byChildAmount;

  unsigned __int8 pbyType[2]{};
  memset(pbyType, 13, 2);
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    7u);
}

void CPlayer::pc_PotionDivision(
  unsigned __int16 wSerial,
  unsigned __int16 wTarSerial,
  unsigned __int8 byAmount)
{
  char errCode = 0;
  _STORAGE_LIST::_db_con *sourceItem = nullptr;
  _STORAGE_LIST::_db_con *targetItem = nullptr;
  unsigned __int8 parentAmount = 0;
  unsigned __int8 childAmount = 0;

  sourceItem = _STORAGE_LIST::GetPtrFromSerial(m_Param.m_pStoragePtr[0], wSerial);
  if (sourceItem)
  {
    if (sourceItem->m_bLock)
    {
      errCode = -11;
    }
    else
    {
      targetItem = _STORAGE_LIST::GetPtrFromSerial(m_Param.m_pStoragePtr[0], wTarSerial);
      if (targetItem)
      {
        if (targetItem->m_bLock)
        {
          errCode = -11;
        }
        else if (sourceItem->m_byTableCode == 13 && targetItem->m_byTableCode == 13)
        {
          if (sourceItem->m_wItemIndex == targetItem->m_wItemIndex)
          {
            if (sourceItem->m_dwDur >= byAmount)
            {
              if (targetItem->m_dwDur + byAmount > 0x63)
              {
                errCode = -4;
              }
            }
            else
            {
              errCode = -4;
            }
          }
          else
          {
            errCode = -6;
          }
        }
        else
        {
          errCode = -3;
        }
      }
      else
      {
        errCode = -5;
      }
    }
  }
  else
  {
    errCode = -5;
  }

  if (!errCode)
  {
    parentAmount = static_cast<unsigned __int8>(
      Emb_AlterDurPoint(0, sourceItem->m_byStorageIndex, -byAmount, 0, 0));
    childAmount = static_cast<unsigned __int8>(
      Emb_AlterDurPoint(0, targetItem->m_byStorageIndex, byAmount, 0, 0));
  }
  SendMsg_PotionDivision(wSerial, parentAmount, wTarSerial, childAmount, errCode);
}

void CPlayer::SendMsg_PotionDivision(
  unsigned __int16 wSerial,
  unsigned __int8 byParentAmount,
  unsigned __int16 wChildSerial,
  unsigned __int8 byChildAmount,
  char nRet)
{
  #pragma pack(push, 1)
  struct PotionDivisionMsg
  {
    char byRet;
    unsigned __int16 wParentSerial;
    unsigned __int8 byParentAmount;
    unsigned __int16 wChildSerial;
    unsigned __int8 byChildAmount;
  };
  #pragma pack(pop)

  PotionDivisionMsg msg{};
  msg.byRet = nRet;
  msg.wParentSerial = wSerial;
  msg.byParentAmount = byParentAmount;
  msg.wChildSerial = wChildSerial;
  msg.byChildAmount = byChildAmount;

  unsigned __int8 pbyType[2] = {13, 15};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    7u);
}

void CPlayer::pc_AlterItemSlotRequest(unsigned __int8 byNum, _alter_item_slot_request_clzo::__list *pList)
{
  if (!m_pUserDB)
  {
    return;
  }

  for (int j = 0; j < byNum; ++j)
  {
    _STORAGE_LIST::_db_con *item = _STORAGE_LIST::GetPtrFromSerial(
      m_Param.m_pStoragePtr[pList[j].byStorageIndex],
      pList[j].dwItemSerial);
    if (item)
    {
      item->m_byClientIndex = pList[j].byClientSlotIndex;
      CUserDB::Update_ItemSlot(
        m_pUserDB,
        pList[j].byStorageIndex,
        item->m_byStorageIndex,
        pList[j].byClientSlotIndex);
    }
  }
}

void CPlayer::pc_AlterLinkBoardSlotRequest(
  unsigned __int8 byNum,
  _alter_link_slot_request_clzo::__list *pList,
  unsigned __int8 byLBLock)
{
  if (!m_pUserDB)
  {
    return;
  }

  for (int j = 0; j < byNum; ++j)
  {
    _alter_link_slot_request_clzo::__list *entry = &pList[j];
    unsigned __int8 linkCode = entry->byLinkCode;
    unsigned __int16 index = entry->wIndex;
    unsigned __int8 storageCode = 0;
    if (linkCode == 4)
    {
      _STORAGE_LIST::_db_con *item = CPlayerDB::GetPtrItemStorage(&m_Param, index, &storageCode);
      if (item)
      {
        index = 0;
        index = static_cast<unsigned __int16>(item->m_byStorageIndex | (storageCode << 8));
        CPlayerDB::PushLink(&m_Param, entry->bySlotIndex, item->m_wSerial, 0);
      }
      else
      {
        linkCode = 0xFF;
        index = 0xFFFF;
      }
    }
    else if (linkCode == 0xFF)
    {
      CPlayerDB::PopLink(&m_Param, entry->bySlotIndex);
    }
    CUserDB::Update_LinkBoardSlot(m_pUserDB, entry->bySlotIndex, linkCode, index);
  }

  CUserDB::Update_LinkBoardLock(m_pUserDB, byLBLock);
}

void CPlayer::pc_ChangeModeType(unsigned __int8 nModeType, unsigned __int8 nStandType)
{
  m_byModeType = nModeType;
  m_byStandType = nStandType;
  SenseState();
}

void CPlayer::pc_GestureRequest(unsigned __int8 byGestureType)
{
  SendMsg_GestureInform(byGestureType);
}

void CPlayer::SendMsg_GestureInform(unsigned __int8 byGestureType)
{
  #pragma pack(push, 1)
  struct GestureInformMsg
  {
    unsigned int dwSerial;
    unsigned __int8 byGestureType;
  };
  #pragma pack(pop)

  GestureInformMsg msg{};
  msg.dwSerial = m_dwObjSerial;
  msg.byGestureType = byGestureType;

  unsigned __int8 pbyType[2] = {13, 23};
  CircleReport(pbyType, reinterpret_cast<char *>(&msg), 5, 1);
}

void CPlayer::pc_AlterWindowInfoRequest(
  unsigned int *pdwSkill,
  unsigned int *pdwForce,
  unsigned int *pdwChar,
  unsigned int *pdwAnimus,
  unsigned int dwInven,
  unsigned int *pdwInvenBag)
{
  if (m_pUserDB)
  {
    CUserDB::Update_WindowInfo(m_pUserDB, pdwSkill, pdwForce, pdwChar, pdwAnimus, dwInven, pdwInvenBag);
  }
}

void CPlayer::pc_SetTargetObjectRequest(CGameObject *pTar, unsigned int dwSerial, bool bForce)
{
  unsigned __int8 errCode = 0;
  if (pTar)
  {
    if (pTar->m_bLive)
    {
      if (pTar->m_dwObjSerial == dwSerial)
      {
        if (!IsTargeting(pTar))
        {
          errCode = 4;
        }
      }
      else
      {
        errCode = 3;
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

  if (!errCode)
  {
    CPlayer::__target::init(&m_TargetObject);
    m_TargetObject.pObject = pTar;
    m_TargetObject.byKind = pTar->m_ObjID.m_byKind;
    m_TargetObject.byID = pTar->m_ObjID.m_byID;
    m_TargetObject.dwSerial = pTar->m_dwObjSerial;
    m_TargetObject.wHPRate = static_cast<unsigned __int16>(pTar->CalcCurHPRate());
    pTar->BeTargeted(this);
  }

  SendMsg_SetTargetObjectResult(errCode, bForce);
  SendTargetMonsterSFContInfo();
  SendTargetPlayerDamageContInfo();
}

void CPlayer::SendMsg_SetTargetObjectResult(char byRetCode, bool bForce)
{
  #pragma pack(push, 1)
  struct SetTargetObjectResultMsg
  {
    char byRetCode;
    unsigned __int8 byKind;
    unsigned __int8 byID;
    unsigned int dwSerial;
    unsigned __int16 wHPRate;
    bool bCorpse;
    bool bForce;
  };
  #pragma pack(pop)

  SetTargetObjectResultMsg msg{};
  msg.byRetCode = byRetCode;
  if (!byRetCode && m_TargetObject.pObject)
  {
    msg.byKind = m_TargetObject.byKind;
    msg.byID = m_TargetObject.byID;
    msg.dwSerial = m_TargetObject.dwSerial;
    msg.wHPRate = m_TargetObject.wHPRate;
    if (!m_TargetObject.byKind && !m_TargetObject.byID)
    {
      msg.bCorpse = m_TargetObject.pObject->m_bCorpse;
    }
  }
  msg.bForce = bForce;

  unsigned __int8 pbyType[2] = {13, 27};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    0xBu);
}

void CPlayer::SendTargetMonsterSFContInfo()
{
  CMonster *target = static_cast<CMonster *>(GetTargetObj());
  if (target && !target->m_ObjID.m_byKind && target->m_ObjID.m_byID == 1)
  {
    _target_monster_contsf_allinform_zocl msg{};
    msg.dwSerial = target->m_dwObjSerial;
    unsigned __int8 count = 0;
    for (int contType = 0; contType < 2; ++contType)
    {
      for (int index = 0; index < CMonster::GetMaxDMGSFContCount(target); ++index)
      {
        _sf_continous *cont = &target->m_SFCont[contType][index];
        if (cont->m_bExist)
        {
          if (count < static_cast<unsigned __int8>(CMonster::GetMaxDMGSFContCount(target)))
          {
            msg.m_MonContSf[count++].wSfcode = CalcEffectBit(cont->m_byEffectCode, cont->m_wEffectIndex);
            msg.byContCount = count;
          }
        }
      }
    }

    if (!_target_monster_contsf_allinform_zocl::IsSame(&msg, &m_TargetObject.m_PrevTargetMonsterContInfo))
    {
      unsigned __int8 pbyType[2] = {13, 100};
      const unsigned __int16 nLen = _target_monster_contsf_allinform_zocl::size(&msg);
      CNetProcess::LoadSendMsg(
        g_Network.m_pProcess[0],
        m_ObjID.m_wIndex,
        pbyType,
        reinterpret_cast<char *>(&msg),
        nLen);
      memcpy_0(&m_TargetObject.m_PrevTargetMonsterContInfo, &msg, sizeof(m_TargetObject.m_PrevTargetMonsterContInfo));
    }
  }
}

void CPlayer::SendTargetPlayerDamageContInfo()
{
  CGameObject *target = GetTargetObj();
  if (target && !target->m_ObjID.m_byKind && !target->m_ObjID.m_byID)
  {
    CPlayer *targetPlayer = static_cast<CPlayer *>(target);
    _target_player_damage_contsf_allinform_zocl msg{};
    msg.dwSerial = targetPlayer->m_dwObjSerial;

    unsigned __int8 count = 0;
    for (int j = 0; j < 8; ++j)
    {
      _sf_continous *cont = &targetPlayer->m_SFCont[0][j];
      if (cont->m_bExist)
      {
        msg.m_PlayerContSf[count].wSfcode = CalcEffectBit(cont->m_byEffectCode, cont->m_wEffectIndex);
        msg.m_PlayerContSf[count].byContCount = static_cast<unsigned __int8>(cont->m_nCumulCounter);
        msg.byContCount = ++count;
      }
    }

    if (!_target_player_damage_contsf_allinform_zocl::IsSame(
          &msg,
          &m_TargetObject.m_PrevTargetPlayerDamageContInfo))
    {
      unsigned __int8 pbyType[2] = {13, 112};
      const unsigned __int16 nLen = _target_player_damage_contsf_allinform_zocl::size(&msg);
      CNetProcess::LoadSendMsg(
        g_Network.m_pProcess[0],
        m_ObjID.m_wIndex,
        pbyType,
        reinterpret_cast<char *>(&msg),
        nLen);
      memcpy_0(
        &m_TargetObject.m_PrevTargetPlayerDamageContInfo,
        &msg,
        sizeof(m_TargetObject.m_PrevTargetPlayerDamageContInfo));
    }
  }
}

void CPlayer::pc_ReleaseTargetObjectRequest()
{
  m_TargetObject.pObject = nullptr;
}

void CPlayer::pc_PartyReqBlock(bool bBlock)
{
  if (m_bBlockParty != bBlock)
  {
    m_bBlockParty = bBlock;
  }
}

void CPlayer::pc_WhisperBlock(bool bBlock)
{
  if (m_bBlockWhisper != bBlock)
  {
    m_bBlockWhisper = bBlock;
  }
}

void CPlayer::pc_TradeBlock(bool bBlock)
{
  if (m_bBlockTrade != bBlock)
  {
    m_bBlockTrade = bBlock;
  }
}

void CPlayer::pc_GuildBattleBlock(bool bBlock)
{
  m_bBlockGuildBattleMsg = bBlock;
}

void CPlayer::pc_MacroUpdate(char *pBuf)
{
  CUserDB::Update_Macro(m_pUserDB, pBuf);
}

void CPlayer::pc_SetRaceBossCryMsg(unsigned __int8 bySlot, char *pwszCryMsg)
{
  if (m_pUserDB && IsSQLValidString(pwszCryMsg))
  {
    CNationSettingManager *manager = CTSingleton<CNationSettingManager>::Instance();
    if (CNationSettingManager::IsNormalString(manager, pwszCryMsg))
    {
      strcpy_0(m_pmCryMsg.m_List[bySlot].wszCryMsg, pwszCryMsg);
      CUserDB::Update_BossCryMsg(m_pUserDB, bySlot, pwszCryMsg);
    }
  }
}

void CPlayer::pc_NotifyRaceBossCryMsg()
{
  if (m_pUserDB)
  {
    SendMsg_RaceBossCryMsg();
  }
}

void CPlayer::SendMsg_RaceBossCryMsg()
{
  char msg[650]{};
  for (int j = 0; j < 10; ++j)
  {
    strcpy_0(&msg[65 * j], m_pmCryMsg.m_List[j].wszCryMsg);
  }

  unsigned __int8 pbyType[2] = {13, 105};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    pbyType,
    msg,
    0x28Au);
}

void CPlayer::pc_SetGroupTargetObjectRequest(
  CGameObject *pTar,
  unsigned int dwSerial,
  unsigned __int8 byGroupType)
{
  unsigned __int8 errCode = 0;
  CPlayer *memberPlayer = nullptr;
  CPartyPlayer **partyMembers = nullptr;
  _guild_member_info *guildMember = nullptr;

  if (pTar)
  {
    if (pTar->m_bLive)
    {
      if (pTar->m_dwObjSerial == dwSerial)
      {
        if (!IsTargeting(pTar) || pTar->m_ObjID.m_byID == 2)
        {
          errCode = 4;
          SendMsg_SetGroupTargetObjectResult(4u, byGroupType);
        }
        else
        {
          if (byGroupType == 1)
          {
            if (m_Param.m_pGuild)
            {
              const unsigned int masterSerial = CGuild::GetGuildMasterSerial(m_Param.m_pGuild);
              const unsigned int charSerial = CPlayerDB::GetCharSerial(&m_Param);
              if (masterSerial == charSerial)
              {
                for (int j = 0; j < 50; ++j)
                {
                  guildMember = &m_Param.m_pGuild->m_MemberData[j];
                  if (_guild_member_info::IsFill(guildMember))
                  {
                    memberPlayer = guildMember->pPlayer;
                    if (memberPlayer)
                    {
                      if (memberPlayer->m_bOper
                          && memberPlayer->m_pCurMap == m_pCurMap
                          && memberPlayer->m_wMapLayerIndex == m_wMapLayerIndex)
                      {
                        CPlayer::__target::init(&memberPlayer->m_GroupTargetObject[byGroupType]);
                        memberPlayer->m_GroupTargetObject[byGroupType].pObject = pTar;
                        memberPlayer->m_GroupTargetObject[byGroupType].byKind = pTar->m_ObjID.m_byKind;
                        memberPlayer->m_GroupTargetObject[byGroupType].byID = pTar->m_ObjID.m_byID;
                        memberPlayer->m_GroupTargetObject[byGroupType].dwSerial = pTar->m_dwObjSerial;
                        memberPlayer->SendMsg_SetGroupTargetObjectResult(errCode, byGroupType);
                      }
                    }
                  }
                }
              }
            }
          }
          else if (byGroupType == 2)
          {
            const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
            CPvpUserAndGuildRankingSystem *rankingSystem = CPvpUserAndGuildRankingSystem::Instance();
            const unsigned int bossSerial =
              CPvpUserAndGuildRankingSystem::GetCurrentRaceBossSerial(rankingSystem, raceCode, 0);
            if (bossSerial == CPlayerDB::GetCharSerial(&m_Param))
            {
              char targetName[17]{};
              if (!pTar->m_ObjID.m_byKind && !pTar->m_ObjID.m_byID)
              {
                const CPlayer *targetPlayer = static_cast<const CPlayer *>(pTar);
                if (targetPlayer->m_bOper)
                {
                  const char *name = CPlayerDB::GetCharNameW(&targetPlayer->m_Param);
                  strcpy_0(targetName, name);
                }
              }
              for (int j = 0; j < MAX_PLAYER; ++j)
              {
                memberPlayer = &g_Player[j];
                if (memberPlayer->m_bOper)
                {
                  if (CPlayerDB::GetRaceCode(&memberPlayer->m_Param) == CPlayerDB::GetRaceCode(&m_Param))
                  {
                    if (memberPlayer->GetLevel() >= 25 && targetName[0])
                    {
                      memberPlayer->SendMsg_GroupTargetInform(byGroupType, targetName);
                    }
                    if (memberPlayer->m_pCurMap == m_pCurMap
                        && memberPlayer->m_wMapLayerIndex == m_wMapLayerIndex)
                    {
                      CPlayer::__target::init(&memberPlayer->m_GroupTargetObject[byGroupType]);
                      memberPlayer->m_GroupTargetObject[byGroupType].pObject = pTar;
                      memberPlayer->m_GroupTargetObject[byGroupType].byKind = pTar->m_ObjID.m_byKind;
                      memberPlayer->m_GroupTargetObject[byGroupType].byID = pTar->m_ObjID.m_byID;
                      memberPlayer->m_GroupTargetObject[byGroupType].dwSerial = pTar->m_dwObjSerial;
                      memberPlayer->SendMsg_SetGroupTargetObjectResult(errCode, byGroupType);
                    }
                  }
                }
              }
            }
          }
          else if (CPartyPlayer::IsPartyMode(m_pPartyMgr) && CPartyPlayer::IsPartyBoss(m_pPartyMgr))
          {
            partyMembers = CPartyPlayer::GetPtrPartyMember(m_pPartyMgr);
            if (partyMembers)
            {
              for (int j = 0; j < 8; ++j)
              {
                if (partyMembers[j])
                {
                  memberPlayer = &g_Player[partyMembers[j]->m_id.wIndex];
                  if (memberPlayer->m_bOper
                      && memberPlayer->m_pCurMap == m_pCurMap
                      && memberPlayer->m_wMapLayerIndex == m_wMapLayerIndex)
                  {
                    CPlayer::__target::init(memberPlayer->m_GroupTargetObject);
                    memberPlayer->m_GroupTargetObject[0].pObject = pTar;
                    memberPlayer->m_GroupTargetObject[0].byKind = pTar->m_ObjID.m_byKind;
                    memberPlayer->m_GroupTargetObject[0].byID = pTar->m_ObjID.m_byID;
                    memberPlayer->m_GroupTargetObject[0].dwSerial = pTar->m_dwObjSerial;
                    memberPlayer->SendMsg_SetGroupTargetObjectResult(errCode, 0);
                  }
                }
              }
            }
          }
        }
      }
      else
      {
        errCode = 3;
        SendMsg_SetGroupTargetObjectResult(3u, byGroupType);
      }
    }
    else
    {
      errCode = 2;
      SendMsg_SetGroupTargetObjectResult(2u, byGroupType);
    }
  }
  else
  {
    errCode = 1;
    SendMsg_SetGroupTargetObjectResult(1u, byGroupType);
  }
}

void CPlayer::pc_ReleaseGroupTargetObjectRequest(unsigned __int8 byGroupType)
{
  CPlayer *memberPlayer = nullptr;
  CPartyPlayer **partyMembers = nullptr;
  _guild_member_info *guildMember = nullptr;

  if (m_GroupTargetObject[byGroupType].pObject)
  {
    if (byGroupType == 1)
    {
      if (m_Param.m_pGuild)
      {
        const unsigned int masterSerial = CGuild::GetGuildMasterSerial(m_Param.m_pGuild);
        const unsigned int charSerial = CPlayerDB::GetCharSerial(&m_Param);
        if (masterSerial == charSerial)
        {
          for (int j = 0; j < 50; ++j)
          {
            guildMember = &m_Param.m_pGuild->m_MemberData[j];
            if (_guild_member_info::IsFill(guildMember))
            {
              memberPlayer = guildMember->pPlayer;
              if (memberPlayer)
              {
                if (memberPlayer->m_bOper && memberPlayer->m_GroupTargetObject[byGroupType].pObject)
                {
                  CPlayer::__target::init(&memberPlayer->m_GroupTargetObject[byGroupType]);
                  memberPlayer->SendMsg_ReleaseGroupTargetObjectResult(byGroupType);
                }
              }
            }
          }
        }
      }
    }
    else if (byGroupType == 2)
    {
      const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
      CPvpUserAndGuildRankingSystem *rankingSystem = CPvpUserAndGuildRankingSystem::Instance();
      const unsigned int bossSerial =
        CPvpUserAndGuildRankingSystem::GetCurrentRaceBossSerial(rankingSystem, raceCode, 0);
      if (bossSerial == CPlayerDB::GetCharSerial(&m_Param))
      {
        for (int j = 0; j < MAX_PLAYER; ++j)
        {
          memberPlayer = &g_Player[j];
          if (memberPlayer->m_bOper)
          {
            if (CPlayerDB::GetRaceCode(&memberPlayer->m_Param) == CPlayerDB::GetRaceCode(&m_Param))
            {
              if (memberPlayer->m_GroupTargetObject[byGroupType].pObject)
              {
                CPlayer::__target::init(&memberPlayer->m_GroupTargetObject[byGroupType]);
                memberPlayer->SendMsg_ReleaseGroupTargetObjectResult(byGroupType);
              }
            }
          }
        }
      }
    }
    else if (CPartyPlayer::IsPartyMode(m_pPartyMgr) && CPartyPlayer::IsPartyBoss(m_pPartyMgr))
    {
      partyMembers = CPartyPlayer::GetPtrPartyMember(m_pPartyMgr);
      if (partyMembers)
      {
        for (int j = 0; j < 8; ++j)
        {
          if (partyMembers[j])
          {
            memberPlayer = &g_Player[partyMembers[j]->m_id.wIndex];
            if (memberPlayer->m_bOper)
            {
              if (memberPlayer->m_GroupTargetObject[0].pObject)
              {
                CPlayer::__target::init(memberPlayer->m_GroupTargetObject);
                memberPlayer->SendMsg_ReleaseGroupTargetObjectResult(0);
              }
            }
          }
        }
      }
    }
  }
}

void CPlayer::pc_SetGroupMapPointRequest(unsigned __int8 byGroupType, float *pzTar)
{
  CPlayer *memberPlayer = nullptr;
  CPartyPlayer **partyMembers = nullptr;
  _guild_member_info *guildMember = nullptr;

  const unsigned int curTime = GetLoopTime();
  float checkPos[3]{};
  checkPos[0] = pzTar[0];
  checkPos[2] = pzTar[1];

  if (65535.0f == CLevel::GetFirstYpos(&m_pCurMap->m_Level, checkPos, -1))
  {
    SendMsg_SetGroupMapPoint(5u, byGroupType, m_pCurMap->m_pMapSet->m_dwIndex, pzTar, 0);
    return;
  }

  if (byGroupType == 1)
  {
    if (m_Param.m_pGuild)
    {
      const unsigned int masterSerial = CGuild::GetGuildMasterSerial(m_Param.m_pGuild);
      const unsigned int charSerial = CPlayerDB::GetCharSerial(&m_Param);
      if (masterSerial == charSerial)
      {
        if (curTime - m_dwLastGroupMapPointTime[byGroupType] < 0x57E40)
        {
          pzTar[0] =
            static_cast<float>((360000 - (curTime - m_dwLastGroupMapPointTime[byGroupType])) / 1000);
          SendMsg_SetGroupMapPoint(2u, byGroupType, m_pCurMap->m_pMapSet->m_dwIndex, pzTar, 0);
          return;
        }
        for (int j = 0; j < 50; ++j)
        {
          guildMember = &m_Param.m_pGuild->m_MemberData[j];
          if (_guild_member_info::IsFill(guildMember))
          {
            memberPlayer = guildMember->pPlayer;
            if (memberPlayer)
            {
              if (memberPlayer->m_bOper
                  && memberPlayer->m_pCurMap == m_pCurMap
                  && memberPlayer->m_wMapLayerIndex == m_wMapLayerIndex)
              {
                memberPlayer->SendMsg_SetGroupMapPoint(
                  0,
                  byGroupType,
                  m_pCurMap->m_pMapSet->m_dwIndex,
                  pzTar,
                  0x3Cu);
              }
            }
          }
        }
        goto UPDATE_GROUP_MAP_POINT;
      }
    }
  }
  else if (byGroupType == 2)
  {
    const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
    CPvpUserAndGuildRankingSystem *rankingSystem = CPvpUserAndGuildRankingSystem::Instance();
    const unsigned int bossSerial =
      CPvpUserAndGuildRankingSystem::GetCurrentRaceBossSerial(rankingSystem, raceCode, 0);
    if (bossSerial == CPlayerDB::GetCharSerial(&m_Param))
    {
      if (strcmp_0(m_pCurMap->m_pMapSet->m_strCode, "resources"))
      {
        SendMsg_SetGroupMapPoint(3u, byGroupType, m_pCurMap->m_pMapSet->m_dwIndex, pzTar, 0);
        return;
      }
      if (CHolyStoneSystem::GetSceneCode(&g_HolySys) != 1)
      {
        SendMsg_SetGroupMapPoint(4u, byGroupType, m_pCurMap->m_pMapSet->m_dwIndex, pzTar, 0);
        return;
      }
      if (curTime - m_dwLastGroupMapPointTime[byGroupType] < 0x2BF20)
      {
        pzTar[0] =
          static_cast<float>((180000 - (curTime - m_dwLastGroupMapPointTime[byGroupType])) / 1000);
        SendMsg_SetGroupMapPoint(2u, byGroupType, m_pCurMap->m_pMapSet->m_dwIndex, pzTar, 0);
        return;
      }
      for (int j = 0; j < MAX_PLAYER; ++j)
      {
        memberPlayer = &g_Player[j];
        if (memberPlayer->m_bOper)
        {
          if (CPlayerDB::GetRaceCode(&memberPlayer->m_Param) == CPlayerDB::GetRaceCode(&m_Param)
              && memberPlayer->m_pCurMap == m_pCurMap)
          {
            memberPlayer->SendMsg_SetGroupMapPoint(
              0,
              byGroupType,
              m_pCurMap->m_pMapSet->m_dwIndex,
              pzTar,
              0x3Cu);
          }
        }
      }
      goto UPDATE_GROUP_MAP_POINT;
    }
  }
  else if (CPartyPlayer::IsPartyMode(m_pPartyMgr) && CPartyPlayer::IsPartyBoss(m_pPartyMgr))
  {
    if (curTime - m_dwLastGroupMapPointTime[0] < 0xEA60)
    {
      pzTar[0] = static_cast<float>((60000 - (curTime - m_dwLastGroupMapPointTime[0])) / 1000);
      SendMsg_SetGroupMapPoint(2u, 0, m_pCurMap->m_pMapSet->m_dwIndex, pzTar, 0);
      return;
    }
    partyMembers = CPartyPlayer::GetPtrPartyMember(m_pPartyMgr);
    if (partyMembers)
    {
      for (int j = 0; j < 8; ++j)
      {
        if (partyMembers[j])
        {
          memberPlayer = &g_Player[partyMembers[j]->m_id.wIndex];
          if (memberPlayer->m_bOper
              && memberPlayer->m_pCurMap == m_pCurMap
              && memberPlayer->m_wMapLayerIndex == m_wMapLayerIndex)
          {
            memberPlayer->SendMsg_SetGroupMapPoint(0, 0, m_pCurMap->m_pMapSet->m_dwIndex, pzTar, 0x3Cu);
          }
        }
      }
      goto UPDATE_GROUP_MAP_POINT;
    }
  }

  SendMsg_SetGroupMapPoint(1u, byGroupType, m_pCurMap->m_pMapSet->m_dwIndex, pzTar, 0);
  return;

UPDATE_GROUP_MAP_POINT:
  m_dwLastGroupMapPointTime[byGroupType] = curTime;
  m_fGroupMapPoint[byGroupType][0] = pzTar[0];
  m_fGroupMapPoint[byGroupType][1] = pzTar[1];
  m_byGroupMapPointMapCode[byGroupType] = CMapData::GetMapCode(m_pCurMap);
  m_wGroupMapPointLayerIndex[byGroupType] = m_wMapLayerIndex;
}

void CPlayer::SendMsg_GroupTargetInform(char byGroupType, char *pwszName)
{
  #pragma pack(push, 1)
  struct GroupTargetInformMsg
  {
    char byGroupType;
    char wszName[65];
  };
  #pragma pack(pop)

  GroupTargetInformMsg msg{};
  msg.byGroupType = byGroupType;
  strcpy_0(msg.wszName, pwszName);

  unsigned __int8 pbyType[2] = {13, 111};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    0x42u);
}

void CPlayer::pc_RequestPatriarchPunishment(unsigned __int8 byType, char *pwszName, char *pwszCont)
{
  unsigned __int8 errCode = 0;

  if (!CMainThread::IsReleaseServiceMode(&g_Main))
  {
    errCode = 0;
  }
  else
  {
    const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
    CPvpUserAndGuildRankingSystem *rankingSystem = CPvpUserAndGuildRankingSystem::Instance();
    if (CPvpUserAndGuildRankingSystem::GetCurrentRaceBossSerial(rankingSystem, raceCode, 0) != m_dwObjSerial)
    {
      errCode = 1;
    }
  }

  if (!errCode)
  {
    if (g_VoteSys[static_cast<int>(CPlayerDB::GetRaceCode(&m_Param))].m_bActive)
    {
      errCode = 2;
    }
    else if (byType == 2)
    {
      errCode = 9;
    }
    else if (!IsSQLValidString(pwszName))
    {
      const char *charName = CPlayerDB::GetCharNameA(&m_Param);
      CLogFile::Write(
        &g_Main.m_logSystemError,
        "CPlayer::pc_RequestPatriarchPunishment() : %u(%s) ::IsSQLValidString(pwszName(%s)) Invalid!",
        m_dwObjSerial,
        charName,
        pwszName);
      errCode = 7;
    }
  }

  if (errCode)
  {
    SendMsg_ProposeVoteResult(errCode);
    return;
  }

  _qry_case_select_charserial qryData{};
  qryData.byRace = static_cast<unsigned __int8>(CPlayerDB::GetRaceCode(&m_Param));
  qryData.byType = byType;
  strcpy_0(qryData.wszCharName, pwszName);
  strcpy_0(qryData.wszContent, pwszCont);
  qryData.dwAvatorSerial = static_cast<unsigned int>(-1);

  const int nSize = _qry_case_select_charserial::size(&qryData);
  CMainThread::PushDQSData(&g_Main, 0xFFFFFFFF, nullptr, 0x80u, reinterpret_cast<char *>(&qryData), nSize);
}

void CPlayer::pc_RequestTaxRate()
{
  const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
  CPvpUserAndGuildRankingSystem *rankingSystem = CPvpUserAndGuildRankingSystem::Instance();
  if (CPvpUserAndGuildRankingSystem::GetCurrentRaceBossSerial(rankingSystem, raceCode, 5u) == m_dwObjSerial)
  {
    const int selfRace = CPlayerDB::GetRaceCode(&m_Param);
    const int index = m_ObjID.m_wIndex;
    CUnmannedTraderTaxRateManager *manager = CUnmannedTraderTaxRateManager::Instance();
    manager->SendTaxRatePatriarch(index, selfRace);
  }
}

void CPlayer::pc_RequestChangeTaxRate(unsigned __int8 byTaxRate)
{
  unsigned __int8 errCode = 0;
  const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
  CPvpUserAndGuildRankingSystem *rankingSystem = CPvpUserAndGuildRankingSystem::Instance();
  if (CPvpUserAndGuildRankingSystem::GetCurrentRaceBossSerial(rankingSystem, raceCode, 5u) == m_dwObjSerial)
  {
    if (byTaxRate >= 5u && byTaxRate <= 0x14u)
    {
      const int selfRace = CPlayerDB::GetRaceCode(&m_Param);
      CUnmannedTraderTaxRateManager *manager = CUnmannedTraderTaxRateManager::Instance();
      const unsigned int suggestedTime = manager->GetSuggestedTime(selfRace);
      const unsigned int now = GetKorLocalTime();
      if (now - suggestedTime < 0x3C)
      {
        errCode = 2;
      }
    }
    else
    {
      errCode = 3;
    }
  }
  else
  {
    errCode = 1;
  }

  if (errCode)
  {
    SendMsg_ResultChangeTaxRate(errCode, 0xFFu);
    return;
  }

  const unsigned int dwNext = byTaxRate;
  char *wszMatterDst = CPlayerDB::GetCharNameW(&m_Param);
  const unsigned int dwMatterDst = CPlayerDB::GetCharSerial(&m_Param);
  const int selfRace = CPlayerDB::GetRaceCode(&m_Param);
  CUnmannedTraderTaxRateManager *manager = CUnmannedTraderTaxRateManager::Instance();
  manager->SetSuggested(selfRace, 0, dwMatterDst, wszMatterDst, dwNext);
  SendMsg_ResultChangeTaxRate(errCode, byTaxRate);
}

void CPlayer::SendMsg_ResultChangeTaxRate(unsigned __int8 byRetCode, unsigned __int8 byNextTax)
{
  _pt_result_change_tax_rate_zocl msg{};
  msg.byRet = byRetCode;
  msg.byNextTax = byNextTax;

  unsigned __int8 pbyType[2] = {13, 121};
  const unsigned __int16 nLen = _pt_result_change_tax_rate_zocl::size(&msg);
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    nLen);
}

void CPlayer::pc_RequestUILockInit(
  CUserDB *pUserDB,
  char *szUILockPW,
  char *szUILockPW_Confirm,
  unsigned __int8 byUILock_HintIndex,
  char *uszUILock_HintAnswer)
{
  if (!pUserDB->m_byUserDgr)
  {
    unsigned __int8 result = 0;
    if (szUILockPW
        && szUILockPW_Confirm
        && uszUILock_HintAnswer
        && strlen_0(szUILockPW)
        && strlen_0(szUILockPW_Confirm)
        && strlen_0(uszUILock_HintAnswer))
    {
      if (strlen_0(szUILockPW) <= 0xC
          && strlen_0(szUILockPW_Confirm) <= 0xC
          && strlen_0(uszUILock_HintAnswer) <= 0x10)
      {
        if (!strcmp_0(szUILockPW, szUILockPW_Confirm))
        {
          if (!strcmp_0(szUILockPW, pUserDB->m_szAccount_PW))
          {
            result = 2;
          }
          else if (CPlayerDB::GetTrunkSlotNum(&m_Param)
                   && !strcmp_0(szUILockPW, CPlayerDB::GetTrunkPasswdW(&m_Param)))
          {
            result = 2;
          }
          else if (isalphastr(szUILockPW) || isdigitstr(szUILockPW))
          {
            result = 5;
          }
          else if (isalnumstr(szUILockPW))
          {
            if (IsSQLValidString(uszUILock_HintAnswer))
            {
              if (pUserDB->m_byUILock)
              {
                result = 11;
              }
            }
            else
            {
              CLogFile::Write(
                &g_Main.m_logSystemError,
                "CPlayer::pc_CharacterRenameCheck() : Account %u(%s) !::IsSQLValidString( uszUILock_HintAnswer(%s) ) Invalid!",
                pUserDB->m_dwAccountSerial,
                pUserDB->m_szAccountID,
                uszUILock_HintAnswer);
              result = 6;
            }
          }
          else
          {
            result = 5;
          }
        }
        else
        {
          result = 1;
        }
      }
      else
      {
        result = 6;
      }
    }
    else
    {
      result = 6;
    }

    if (result)
    {
      SendMsg_UILock_Init_Result(result);
    }
    else
    {
      SendMsg_UILock_Init_Request_ToAccount(
        pUserDB->m_dwAccountSerial,
        szUILockPW,
        pUserDB->m_idWorld.wIndex,
        byUILock_HintIndex,
        uszUILock_HintAnswer);
    }
  }
}

void CPlayer::pc_RequestUILockCertify(CUserDB *pUserDB, char *uszUILockPW)
{
  if (!pUserDB->m_byUserDgr)
  {
    unsigned __int8 result = 0;
    if (uszUILockPW && strlen_0(uszUILockPW))
    {
      if (pUserDB->m_byUILock)
      {
        if (pUserDB->m_byUILock == 2)
        {
          result = 2;
        }
        else if (strcmp_0(pUserDB->m_szUILock_PW, uszUILockPW))
        {
          if (++pUserDB->m_byUILock_FailCnt < 5u)
          {
            result = 1;
          }
          else
          {
            result = 4;
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
      result = 5;
    }

    if (result)
    {
      if (result == 4)
      {
        SendMsg_UILock_Login_Result(4u, pUserDB->m_byUILock_FailCnt);
        CUserDB::ForceCloseCommand(pUserDB, 8u, 0, 1, "UILOCK Certify Fail");
      }
      else
      {
        SendMsg_UILock_Login_Result(result, pUserDB->m_byUILock_FailCnt);
      }
    }
    else
    {
      pUserDB->m_byUILock = 2;
      pUserDB->m_byUILock_FailCnt = 0;
      pUserDB->m_byUILockFindPassFailCount = 0;
      SendMsg_UILock_Login_Result(result, pUserDB->m_byUILock_FailCnt);
    }
  }
}

void CPlayer::pc_RequestUILockUpdate(
  char *uszUILockPWOld,
  char *uszUILockPW,
  char *uszUILockPW_Confirm,
  unsigned __int8 byUILock_HintIndex,
  char *uszUILock_HintAnswer)
{
  if (!m_pUserDB->m_byUserDgr)
  {
    unsigned __int8 result = 0;
    if (uszUILockPW
        && uszUILockPW_Confirm
        && uszUILock_HintAnswer
        && strlen_0(uszUILockPW)
        && strlen_0(uszUILockPW_Confirm)
        && strlen_0(uszUILock_HintAnswer))
    {
      if (strlen_0(uszUILockPW) <= 0xC
          && strlen_0(uszUILockPW_Confirm) <= 0xC
          && strlen_0(uszUILock_HintAnswer) <= 0x10)
      {
        if (m_pUserDB->m_byUILock == 2)
        {
          if (!strcmp_0(uszUILockPWOld, m_pUserDB->m_szUILock_PW))
          {
            if (!strcmp_0(uszUILockPW, m_pUserDB->m_szUILock_PW))
            {
              result = 8;
            }
            else if (!strcmp_0(uszUILockPW, uszUILockPW_Confirm))
            {
              if (!strcmp_0(uszUILockPW, m_pUserDB->m_szAccount_PW))
              {
                result = 2;
              }
              else if (CPlayerDB::GetTrunkSlotNum(&m_Param)
                       && !strcmp_0(uszUILockPW, CPlayerDB::GetTrunkPasswdW(&m_Param)))
              {
                result = 2;
              }
              else if (isalphastr(uszUILockPW) || isdigitstr(uszUILockPW))
              {
                result = 5;
              }
              else if (isalnumstr(uszUILockPW))
              {
                if (!IsSQLValidString(uszUILock_HintAnswer))
                {
                  CLogFile::Write(
                    &g_Main.m_logSystemError,
                    "CPlayer::pc_RequestUILockUpdate() : Account : %u(%s) Character : %u !::IsSQLValidString( uszUILock_H"
                    "intAnswer(%s) ) Invalid!",
                    m_pUserDB->m_dwAccountSerial,
                    m_pUserDB->m_szAccountID,
                    m_dwObjSerial,
                    uszUILock_HintAnswer);
                  result = 6;
                }
              }
              else
              {
                result = 5;
              }
            }
            else
            {
              result = 1;
            }
          }
          else
          {
            result = 9;
          }
        }
        else
        {
          result = 13;
        }
      }
      else
      {
        result = 6;
      }
    }
    else
    {
      result = 6;
    }

    if (result)
    {
      SendMsg_UILock_Update_Result(result);
    }
    else
    {
      SendMsg_UILock_Update_Request_ToAccount(
        m_pUserDB->m_dwAccountSerial,
        uszUILockPW,
        m_pUserDB->m_idWorld.wIndex,
        byUILock_HintIndex,
        uszUILock_HintAnswer);
    }
  }
}

void CPlayer::pc_RequestUILockFindPW(CUserDB *pUserDB, char *uszHintAnswer)
{
  if (!pUserDB->m_byUserDgr)
  {
    unsigned __int8 result = 0;
    if (uszHintAnswer && strlen_0(uszHintAnswer))
    {
      if (pUserDB->m_byUILock)
      {
        if (pUserDB->m_byUILock == 2)
        {
          result = 3;
        }
        else if (strcmp_0(pUserDB->m_uszUILock_HintAnswer, uszHintAnswer))
        {
          if (++pUserDB->m_byUILockFindPassFailCount < 5u)
          {
            result = 1;
          }
          else
          {
            result = 5;
          }
        }
      }
      else
      {
        result = 2;
      }
    }
    else
    {
      result = 4;
    }

    if (result)
    {
      if (result == 5)
      {
        SendMsg_UILock_FindPW_Result(result, nullptr, pUserDB->m_byUILockFindPassFailCount);
        CUserDB::ForceCloseCommand(pUserDB, 9u, 0, 1, "UILOCK Find Password Fail");
      }
      else
      {
        SendMsg_UILock_FindPW_Result(result, nullptr, pUserDB->m_byUILockFindPassFailCount);
      }
    }
    else
    {
      pUserDB->m_byUILockFindPassFailCount = 0;
      SendMsg_UILock_FindPW_Result(result, pUserDB->m_szUILock_PW, pUserDB->m_byUILockFindPassFailCount);
    }
  }
}

void CPlayer::SendMsg_UILock_Init_Result(char byRet)
{
  char msg[2]{};
  msg[0] = byRet;
  msg[1] = 0;

  unsigned __int8 pbyType[2] = {13, 0x80};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    pbyType,
    msg,
    2u);
}

void CPlayer::SendMsg_UILock_Login_Result(char byRet, char byFailCount)
{
  char msg[2]{};
  msg[0] = byRet;
  msg[1] = byFailCount;

  unsigned __int8 pbyType[2] = {13, static_cast<unsigned __int8>(-126)};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    pbyType,
    msg,
    2u);
}

void CPlayer::SendMsg_UILock_Init_Request_ToAccount(
  unsigned int dwSerial,
  char *uszUILockPW,
  unsigned __int16 wUserIndex,
  unsigned __int8 byHintIndex,
  char *uszHintAnswer)
{
  #pragma pack(push, 1)
  struct UILockInitRequestMsg
  {
    unsigned __int16 wUserIndex;
    unsigned int dwSerial;
    char szPw[13];
    unsigned __int8 byHintIndex;
    char szHintAnswer[17];
  };
  #pragma pack(pop)

  UILockInitRequestMsg msg{};
  msg.wUserIndex = wUserIndex;
  msg.dwSerial = dwSerial;
  strcpy_s(msg.szPw, sizeof(msg.szPw), uszUILockPW);
  msg.byHintIndex = byHintIndex;
  strcpy_s(msg.szHintAnswer, sizeof(msg.szHintAnswer), uszHintAnswer);

  unsigned __int8 pbyType[2] = {1, 15};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[1],
    0,
    pbyType,
    reinterpret_cast<char *>(&msg),
    0x25u);
}

void CPlayer::SendMsg_UILock_Update_Result(char byRet)
{
  char msg[1]{};
  msg[0] = byRet;

  unsigned __int8 pbyType[2] = {13, static_cast<unsigned __int8>(-124)};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    pbyType,
    msg,
    1u);
}

void CPlayer::SendMsg_UILock_Update_Request_ToAccount(
  unsigned int dwSerial,
  char *uszUILockPW,
  unsigned __int16 wUserIndex,
  unsigned __int8 byHintIndex,
  char *uszHintAnswer)
{
  #pragma pack(push, 1)
  struct UILockUpdateRequestMsg
  {
    unsigned __int16 wUserIndex;
    unsigned int dwSerial;
    char szPw[13];
    unsigned __int8 byHintIndex;
    char szHintAnswer[17];
  };
  #pragma pack(pop)

  UILockUpdateRequestMsg msg{};
  msg.wUserIndex = wUserIndex;
  msg.dwSerial = dwSerial;
  strcpy_s(msg.szPw, sizeof(msg.szPw), uszUILockPW);
  msg.byHintIndex = byHintIndex;
  strcpy_s(msg.szHintAnswer, sizeof(msg.szHintAnswer), uszHintAnswer);

  unsigned __int8 pbyType[2] = {1, 17};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[1],
    0,
    pbyType,
    reinterpret_cast<char *>(&msg),
    0x25u);
}

void CPlayer::SendMsg_UILock_FindPW_Result(char byRet, char *uszUILockPW, char byFindPassFailCount)
{
  #pragma pack(push, 1)
  struct UILockFindPwResultMsg
  {
    char byRet;
    char byFailCount;
    char szPw[13];
  };
  #pragma pack(pop)

  UILockFindPwResultMsg msg{};
  msg.byRet = byRet;
  msg.byFailCount = byFindPassFailCount;
  if (uszUILockPW)
  {
    strcpy_s(msg.szPw, sizeof(msg.szPw), uszUILockPW);
  }

  unsigned __int8 pbyType[2] = {13, static_cast<unsigned __int8>(-121)};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    0xFu);
}

void CPlayer::pc_MineStart(unsigned __int8 byMineIndex, unsigned __int8 byOreIndex, unsigned __int16 wBatterySerial)
{
  unsigned __int8 errCode = 0;
  _base_fld *oreRecord = CRecordData::GetRecord(&g_Main.m_tblItemData[17], byOreIndex);
  _STORAGE_LIST::_db_con *equipItem = nullptr;
  _STORAGE_LIST::_db_con *batteryItem = nullptr;
  _res_dummy *resDummy = nullptr;
  _base_fld *equipRecord = nullptr;
  int resDummySector = -1;

  if (TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, m_id.wIndex) == 99)
  {
    errCode = 33;
  }
  else if (m_Param.m_pAPM && m_Param.m_pAPM->is_installed())
  {
    errCode = 25;
  }
  else if (m_dwMineNextTime == static_cast<unsigned int>(-1))
  {
    if (*reinterpret_cast<unsigned int *>(oreRecord[3].m_strCode) <= 2u)
    {
      if (byMineIndex < m_pCurMap->m_nResDumNum)
      {
        resDummy = &m_pCurMap->m_pResDummy[byMineIndex];
        if (resDummy && resDummy->GetQualityGrade() != 2)
        {
          if (resDummy->GetQualityGrade() == 2 || IsMiningByMinigTicket())
          {
            resDummySector = m_pCurMap->GetResDummySector(byMineIndex, m_fCurPos);
            if (resDummySector == -1)
            {
              errCode = 2;
            }
            else
            {
              equipItem = m_Param.m_dbEquip.m_pStorageList + 6;
              if (equipItem->m_bLoad)
              {
                if (equipItem->m_bLock)
                {
                  errCode = 12;
                }
                else
                {
                  equipRecord = CRecordData::GetRecord(&g_Main.m_tblItemData[6], equipItem->m_wItemIndex);
                  if (equipRecord)
                  {
                    if (*reinterpret_cast<unsigned int *>(&equipRecord[6].m_strCode[8]) == 10)
                    {
                      batteryItem = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, wBatterySerial);
                      if (batteryItem)
                      {
                        if (batteryItem->m_byTableCode == 16)
                        {
                          if (batteryItem->m_wItemIndex < 6u || batteryItem->m_wItemIndex > 9u)
                          {
                            if (batteryItem->m_bLock)
                            {
                              errCode = 12;
                            }
                            else
                            {
                              if (!COreAmountMgr::Instance()->IsOreRemain())
                              {
                                errCode = 27;
                              }
                            }
                          }
                          else
                          {
                            errCode = 8;
                          }
                        }
                        else
                        {
                          errCode = 8;
                        }
                      }
                      else
                      {
                        errCode = 8;
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
                }
              }
              else
              {
                errCode = 4;
              }
            }
          }
          else
          {
            errCode = 26;
          }
        }
        else
        {
          errCode = 28;
        }
      }
      else
      {
        errCode = 1;
      }
    }
    else
    {
      errCode = 9;
    }
  }
  else
  {
    errCode = 7;
  }

  if (!errCode)
  {
    m_bMineMode = true;
    const bool isPcBang = m_pUserDB->m_BillingInfo.IsPcBangType();
    int delay = m_pCurMap->m_pResDummy[byMineIndex].GetDelay(resDummySector, isPcBang);
    const float effHave = m_EP.GetEff_Have(7);
    const double penalty = TimeLimitMgr::GetPlayerPenalty(g_Main.m_pTimeLimitMgr, m_id.wIndex);
    int penaltyRate = 0;
    if (penalty != 0.0)
    {
      penaltyRate = static_cast<int>(1.0 / penalty);
    }
    if (effHave > 1.0f)
    {
      delay = static_cast<int>(static_cast<float>(delay) / effHave);
    }
    const float effRate = m_EP.GetEff_Rate(37);
    if (effRate > 1.0f)
    {
      delay = static_cast<int>(static_cast<float>(delay) / effRate);
    }
    delay *= penaltyRate;
    m_dwMineNextTime = delay + GetLoopTime();
    m_byDelaySec = static_cast<unsigned __int8>(delay / 0x3E8u);
    m_wBatterySerialTmp = wBatterySerial;
    m_bySelectOreIndex = byOreIndex;
    m_zMinePos[0] = static_cast<__int16>(static_cast<int>(m_fCurPos[0]));
    m_zMinePos[1] = static_cast<__int16>(static_cast<int>(m_fCurPos[2]));
    if (m_bMove)
    {
      const bool outExtra = IsOutExtraStopPos(m_fCurPos);
      SendMsg_Stop(outExtra);
      CCharacter::Stop(this);
    }
    CCharacter::BreakStealth(this);
  }

  SendMsg_MineStartResult(errCode);
}

void CPlayer::pc_MineCancle()
{
  if (m_bMineMode)
  {
    m_bMineMode = false;
    m_dwMineNextTime = static_cast<unsigned int>(-1);
    SendMsg_MineCancle();
  }
}

void CPlayer::pc_MineComplete()
{
  unsigned __int8 errCode = 0;
  int targetIndex = 0xFF;
  _STORAGE_LIST::_db_con *batteryItem = nullptr;
  bool gotGoldenBox = false;
  unsigned __int8 boxIndex = 0;
  unsigned __int8 selectedOreIndex = 0;

  if (COreAmountMgr::Instance()->IsOreRemain())
  {
    batteryItem = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, m_wBatterySerialTmp);
    if (batteryItem)
    {
      if (batteryItem->m_bLock)
      {
        errCode = 12;
      }
      else
      {
        CGoldenBoxItemMgr *goldenMgr = CGoldenBoxItemMgr::Instance();
        if (goldenMgr->Get_Event_Status() == 2)
        {
          for (unsigned int j = 0;; ++j)
          {
            const unsigned __int8 loopCount = CGoldenBoxItemMgr::GetLoopCount(goldenMgr);
            if (j >= static_cast<unsigned int>(loopCount))
            {
              break;
            }

            const unsigned __int16 boxItemIndex = CGoldenBoxItemMgr::GetGoldBoxItemIndex(goldenMgr, j);
            _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblItemData[17], boxItemIndex);
            if (record)
            {
              const unsigned int chance = *reinterpret_cast<unsigned int *>(&record[3].m_strCode[4]);
              if (chance)
              {
                const int r1 = rand();
                const int r2 = rand();
                const unsigned int rate = (r2 + (r1 << 16)) % 0x1770u + 1;
                if (chance <= rate)
                {
                  if (CGoldenBoxItemMgr::Get_Box_Count(goldenMgr, j))
                  {
                    selectedOreIndex = static_cast<unsigned __int8>(
                      CGoldenBoxItemMgr::GetGoldBoxItemIndex(goldenMgr, j));
                    gotGoldenBox = true;
                    boxIndex = static_cast<unsigned __int8>(j);
                    break;
                  }
                }
              }
            }
          }
        }

        if (!gotGoldenBox)
        {
          selectedOreIndex = static_cast<unsigned __int8>(m_bySelectOreIndex + rand() % 3);
        }

        const int recordNum = CRecordData::GetRecordNum(&g_Main.m_tblItemData[17]);
        if (selectedOreIndex < recordNum)
        {
          for (int k = 0;; ++k)
          {
            const unsigned __int8 useSlot = CPlayerDB::GetUseSlot(&m_Param);
            if (k >= useSlot)
            {
              break;
            }

            _STORAGE_LIST::_db_con *slot = &m_Param.m_dbInven.m_pStorageList[k];
            if (slot->m_bLoad
              && slot->m_byTableCode == 17
              && slot->m_wItemIndex == selectedOreIndex
              && slot->m_dwDur < 0x63
              && !slot->m_bLock)
            {
              targetIndex = k;
              break;
            }
          }

          if (targetIndex == 0xFF && _STORAGE_LIST::GetIndexEmptyCon(&m_Param.m_dbInven) == 0xFF)
          {
            errCode = 10;
          }
          else
          {
            const __int16 curPosX = static_cast<__int16>(static_cast<int>(m_fCurPos[0]));
            const __int16 curPosZ = static_cast<__int16>(static_cast<int>(m_fCurPos[2]));
            if (m_zMinePos[0] != curPosX || m_zMinePos[1] != curPosZ)
            {
              errCode = 13;
            }
          }
        }
        else
        {
          errCode = 9;
        }
      }
    }
    else
    {
      errCode = 8;
    }
  }
  else
  {
    errCode = 27;
  }

  if (errCode)
  {
    SendMsg_MineCompleteResult(errCode, 0xFFu, 0xFFFFu, 0xFFu, 0xFFFFu);
  }
  else
  {
    unsigned int oreDurResult = 0;
    _STORAGE_LIST::_db_con newOre{};
    unsigned __int16 oreSerial = 0;

    if (targetIndex == 0xFF)
    {
      _STORAGE_LIST::_db_con newItem{};
      newItem.m_byTableCode = 17;
      newItem.m_dwDur = 1;
      newItem.m_wItemIndex = selectedOreIndex;
      newItem.m_dwLv = 0xFFFFFFF;
      newItem.m_wSerial = CPlayerDB::GetNewItemSerial(&m_Param);
      if (!Emb_AddStorage(0, &newItem, false, true))
      {
        CMgrAvatorItemHistory::add_storage_fail(
          &s_MgrItemHistory,
          m_ObjID.m_wIndex,
          &newItem,
          "CPlayer::pc_MineComplete() - Emb_AddStorage() Fail",
          m_szItemHistoryFileName);
      }
      memcpy_0(&newOre, &newItem, sizeof(newOre));
      newOre.m_dwDur = 1;
      oreSerial = newItem.m_wSerial;
      oreDurResult = 1;
    }
    else
    {
      _STORAGE_LIST::_db_con *existing = &m_Param.m_dbInven.m_pStorageList[targetIndex];
      oreDurResult = Emb_AlterDurPoint(0, existing->m_byStorageIndex, 1, 0, 0);
      oreSerial = existing->m_wSerial;
      memset_0(&newOre, 0, sizeof(newOre));
      memcpy_0(&newOre, existing, sizeof(newOre));
    }

    const unsigned int batteryLeftDur =
      Emb_AlterDurPoint(0, batteryItem->m_byStorageIndex, -static_cast<int>(m_byDelaySec), 0, 0);
    _base_fld *oreRecord = CRecordData::GetRecord(&g_Main.m_tblItemData[17], selectedOreIndex);
    Emb_CheckActForQuest(12, oreRecord->m_strCode, 1u, 0);

    SendMsg_MineCompleteResult(
      errCode,
      selectedOreIndex,
      oreSerial,
      static_cast<unsigned __int8>(oreDurResult),
      static_cast<unsigned __int16>(batteryLeftDur));

    if (gotGoldenBox)
    {
      if (!errCode)
      {
        if (CGoldenBoxItemMgr::Instance()->Get_Event_Status() == 2)
        {
          CGoldenBoxItemMgr::Set_Box_Count(CGoldenBoxItemMgr::Instance(), boxIndex);
          newOre.m_dwDur = 1;
          const char *charName = CPlayerDB::GetCharNameA(&m_Param);
          const unsigned int charSerial = CPlayerDB::GetCharSerial(&m_Param);
          SendMsg_Notify_Get_Golden_Box(4u, charSerial, charName, &newOre, 0);
        }
      }
    }

    const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
    const unsigned __int8 oreKind = static_cast<unsigned __int8>(oreRecord[3].m_strCode[0]);
    eAddMineOre(raceCode, oreKind, 1);
    COreAmountMgr::Instance()->DecreaseOre(1u);
  }

  m_dwMineNextTime = static_cast<unsigned int>(-1);
  m_bMineMode = false;
}

void CPlayer::pc_OreCutting(unsigned __int16 wOreSerial, unsigned __int8 byProcessNum)
{
  unsigned __int8 errCode = 0;
  _STORAGE_LIST::_db_con *oreItem = nullptr;
  _base_fld *record = nullptr;
  unsigned int leftOreDur = 0;

  const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
  const unsigned int texRate = eGetTexRate(raceCode) + 10000;
  const int raceCode2 = CPlayerDB::GetRaceCode(&m_Param);
  const float texRateValue = eGetTex(raceCode2) + 1.0f;
  (void)texRateValue;

  unsigned __int64 totalCost = 0;
  if (CPlayerDB::GetResBufferNum(&m_Param))
  {
    errCode = 4;
  }
  else
  {
    oreItem = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, wOreSerial);
    if (oreItem)
    {
      if (oreItem->m_byTableCode == 17)
      {
        if (oreItem->m_bLock)
        {
          errCode = 11;
        }
        else
        {
          record = CRecordData::GetRecord(&g_Main.m_tblItemData[17], oreItem->m_wItemIndex);
          if (record)
          {
            if (byProcessNum <= oreItem->m_dwDur)
            {
              const float baseCost = static_cast<float>(*reinterpret_cast<int *>(&record[4].m_strCode[4]));
              const float addRate = baseCost / 1000.0f;
              float costValue =
                (baseCost + (addRate * static_cast<float>(eGetMgrValue()))) * static_cast<float>(byProcessNum);

              __int64 high = 0;
              if (costValue > 9.223372e18)
              {
                costValue -= 9.223372e18;
                high = 0x7FFFFFFFFFFFFFFFLL;
                if (costValue > 9.223372e18)
                {
                  costValue -= 9.223372e18;
                  high = -2LL;
                }
              }
              totalCost = static_cast<unsigned __int64>(high)
                + static_cast<unsigned int>(static_cast<int>(costValue));
              totalCost *= texRate;
              totalCost /= 0x2710uLL;

              const unsigned int dalant = CPlayerDB::GetDalant(&m_Param);
              if (totalCost > dalant)
              {
                errCode = 3;
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
      }
      else
      {
        errCode = 1;
      }
    }
    else
    {
      errCode = 1;
    }
  }

  if (!errCode)
  {
    for (int j = 0; j < byProcessNum; ++j)
    {
      const int minIndex = record[4].m_dwIndex;
      const int maxIndex = *reinterpret_cast<int *>(record[4].m_strCode);
      const int cutCount = minIndex + rand() % (maxIndex - minIndex + 1);
      for (int k = 0; k < cutCount; ++k)
      {
        const int r1 = rand();
        const int r2 = rand();
        const unsigned int rate = r2 + (r1 << 16);
        const int oreIndex =
          COreCuttingTable::GetOreIndexFromRate(&g_Main.m_tblOreCutting, record->m_dwIndex, rate);
        if (oreIndex != -1 && m_Param.m_wCuttingResBuffer[oreIndex] < 0xFFu)
        {
          ++m_Param.m_wCuttingResBuffer[oreIndex];
        }
      }
    }

    leftOreDur = Emb_AlterDurPoint(0, oreItem->m_byStorageIndex, -byProcessNum, 0, 0);
    const unsigned int cost = static_cast<unsigned int>(totalCost);
    SubDalant(cost);

    if (!m_byUserDgr)
    {
      const unsigned __int64 mgrValue = static_cast<unsigned __int64>(eGetMgrValue()) + 1000;
      const int dalantFee = static_cast<int>(totalCost / mgrValue);
      if (dalantFee > 0)
      {
        const int nAdd = -dalantFee;
        const int race = CPlayerDB::GetRaceCode(&m_Param);
        eAddDalant(race, nAdd);
      }
    }

    const int level = CPlayerDB::GetLevel(&m_Param);
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      const int nLv = CPlayerDB::GetLevel(&m_Param);
      const unsigned __int8 race = static_cast<unsigned __int8>(CPlayerDB::GetRaceCode(&m_Param));
      CMoneySupplyMgr::Instance()->UpdateFeeMoneyData(race, nLv, cost);
    }

    if (CActionPointSystemMgr::Instance()->GetEventStatus(0) == 2)
    {
      const unsigned int actPoint = CUserDB::GetActPoint(m_pUserDB, 0);
      const unsigned int pointAdd =
        byProcessNum * *reinterpret_cast<unsigned int *>(&record[3].m_strCode[36]) + actPoint;
      CUserDB::Update_User_Action_Point(m_pUserDB, 0, pointAdd);
      SendMsg_Alter_Action_Point(0, pointAdd);
    }

    const unsigned int newDalant = CPlayerDB::GetDalant(&m_Param);
    CMgrAvatorItemHistory::cut_item(
      &s_MgrItemHistory,
      m_ObjID.m_wIndex,
      oreItem,
      byProcessNum,
      m_Param.m_wCuttingResBuffer,
      cost,
      newDalant,
      m_szItemHistoryFileName);

    if (m_pUserDB)
    {
      int count = 0;
      _CUTTING_DB_BASE::_LIST list[20];
      for (int j = 0; j < GetMaxResKind() && count < 20; ++j)
      {
        if (m_Param.m_wCuttingResBuffer[j])
        {
          list[count].Key.byTableCode = 18;
          list[count].Key.wItemIndex = static_cast<unsigned __int16>(j);
          list[count++].dwDur = m_Param.m_wCuttingResBuffer[j];
        }
      }
      CUserDB::Update_CuttingPush(m_pUserDB, static_cast<unsigned __int8>(count), list);
    }

    if (record)
    {
      const int race = CPlayerDB::GetRaceCode(&m_Param);
      const unsigned __int8 oreKind = static_cast<unsigned __int8>(record[3].m_strCode[0]);
      eAddCutOre(race, oreKind, byProcessNum);
    }
  }

  SendMsg_OreCuttingResult(errCode, static_cast<unsigned __int8>(leftOreDur), static_cast<unsigned int>(totalCost));
}

void CPlayer::pc_OreIntoBag(unsigned __int16 wResIndex, unsigned __int16 wSerial, unsigned __int8 byAddAmount)
{
  unsigned __int8 errCode = 0;
  _STORAGE_LIST::_db_con *oreItem = nullptr;
  unsigned __int16 newSerial = wSerial;
  int existingDur = 0;

  if (byAddAmount <= m_Param.m_wCuttingResBuffer[wResIndex])
  {
    if (wSerial == 0xFFFF && _STORAGE_LIST::GetIndexEmptyCon(&m_Param.m_dbInven) == 0xFF)
    {
      errCode = 4;
    }
    else if (wSerial != 0xFFFF)
    {
      oreItem = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, wSerial);
      if (oreItem)
      {
        if (oreItem->m_byTableCode == 18 && oreItem->m_wItemIndex == wResIndex)
        {
          if (oreItem->m_bLock)
          {
            errCode = 11;
          }
          else
          {
            existingDur = static_cast<int>(oreItem->m_dwDur);
            if (byAddAmount + existingDur > 99)
            {
              errCode = 6;
            }
          }
        }
        else
        {
          errCode = 7;
        }
      }
      else
      {
        errCode = 5;
      }
    }
  }
  else
  {
    errCode = 2;
  }

  const _TimeItem_fld *timeRec = TimeItem::FindTimeRec(18, wResIndex);
  _STORAGE_LIST::_db_con newItem{};
  if (!errCode)
  {
    m_Param.m_wCuttingResBuffer[wResIndex] -= byAddAmount;
    if (!oreItem || timeRec)
    {
      newItem.m_byTableCode = 18;
      newItem.m_wItemIndex = wResIndex;
      newItem.m_dwDur = byAddAmount;
      newItem.m_dwLv = 0xFFFFFFF;
      newItem.m_wSerial = CPlayerDB::GetNewItemSerial(&m_Param);
      if (timeRec && timeRec->m_nCheckType)
      {
        __time32_t now{};
        _time32(&now);
        newItem.m_byCsMethod = timeRec->m_nCheckType;
        newItem.m_dwT = timeRec->m_nUseTime + now;
        newItem.m_dwLendRegdTime = now;
      }
      if (!Emb_AddStorage(0, &newItem, false, false))
      {
        SendMsg_OreIntoBagResult(static_cast<char>(0xFFu), newSerial, 0, 0);
        return;
      }
      newSerial = newItem.m_wSerial;
    }
    else
    {
      Emb_AlterDurPoint(0, oreItem->m_byStorageIndex, byAddAmount, 0, 0);
    }

    if (m_pUserDB)
    {
      CUserDB::Update_CuttingTrans(m_pUserDB, wResIndex, m_Param.m_wCuttingResBuffer[wResIndex]);
    }
    _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblItemData[18], wResIndex);
    Emb_CheckActForQuest(13, record->m_strCode, byAddAmount, 0);
  }

  SendMsg_OreIntoBagResult(static_cast<char>(errCode), newSerial, newItem.m_byCsMethod, newItem.m_dwT);
}

void CPlayer::pc_CuttingComplete(unsigned __int8 byNpcRace)
{
  unsigned __int64 addGold = 0;
  const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
  const unsigned int texRate = 10000 - eGetTexRate(raceCode);
  char hasRes = 0;
  const int raceCode2 = CPlayerDB::GetRaceCode(&m_Param);
  const float texValue = 1.0f - eGetTex(raceCode2);
  (void)texValue;
  (void)byNpcRace;

  for (int n = 0; n < GetMaxResKind(); ++n)
  {
    if (m_Param.m_wCuttingResBuffer[n])
    {
      _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblItemData[18], n);
      const __int64 value =
        static_cast<__int64>(m_Param.m_wCuttingResBuffer[n])
        * *reinterpret_cast<int *>(&record[5].m_strCode[12]);
      const int low = static_cast<int>(value);
      const int high = static_cast<int>(static_cast<unsigned __int64>(value) >> 32);
      addGold += static_cast<unsigned __int64>((low - high) >> 1);
      hasRes = 1;
    }
  }

  addGold *= texRate;
  addGold /= 0x2710uLL;
  if (hasRes)
  {
    const unsigned int gold = CPlayerDB::GetGold(&m_Param);
    if (!CanAddMoneyForMaxLimGold(addGold, gold))
    {
      SendMsg_CuttingCompleteResult(1u);
      return;
    }
    AddGold(addGold, 1);
    const unsigned int newGold = CPlayerDB::GetGold(&m_Param);
    CMgrAvatorItemHistory::cut_clear_item(
      &s_MgrItemHistory,
      m_ObjID.m_wIndex,
      m_Param.m_wCuttingResBuffer,
      addGold,
      newGold,
      m_szItemHistoryFileName);
  }

  CPlayerDB::InitResBuffer(&m_Param);
  if (!m_byUserDgr)
  {
    const int race = CPlayerDB::GetRaceCode(&m_Param);
    eAddGold(race, addGold);
  }
  if (m_pUserDB)
  {
    CUserDB::Update_CuttingEmpty(m_pUserDB);
  }
  SendMsg_CuttingCompleteResult(0);
}

bool IsTargeting(CGameObject *pTar)
{
  if (!pTar->m_ObjID.m_byKind && !pTar->m_ObjID.m_byID)
  {
    CCharacter *character = static_cast<CCharacter *>(pTar);
    if (character->m_EP.GetEff_State(21))
    {
      return false;
    }
  }

  if (!pTar->m_ObjID.m_byKind)
  {
    return true;
  }

  return pTar->m_ObjID.m_byKind == 1 && pTar->m_ObjID.m_byID == 2;
}

bool _CheckDestMonsterLimitLv(int myLevel, int dstLevel, unsigned __int8 byID)
{
  if (byID != 1)
  {
    return true;
  }

  const int diff = myLevel - dstLevel;
  return diff <= 3 && diff >= -10;
}

bool __fastcall DE_MakeGuardTower(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet)
{
  (void)pActChar;
  (void)pTargetChar;
  (void)fEffectValue;
  (void)byRet;
  // this is not a stub
  return true;
}

bool __fastcall DE_LayTrap(CCharacter *pActChar, CCharacter *pTargetChar, float fEffectValue, unsigned __int8 *byRet)
{
  (void)pActChar;
  (void)pTargetChar;
  (void)fEffectValue;
  (void)byRet;
  // this is not a stub
  return true;
}

CPlayer *GetPtrPlayerFromName(CPlayer *pData, int nNum, char *pwszName)
{
  const unsigned __int8 nameLen = static_cast<unsigned __int8>(strlen_0(pwszName));
  for (int j = 0; j < nNum; ++j)
  {
    if (pData[j].m_bLive && pData[j].m_Param.m_byNameLen == nameLen)
    {
      const char *name = CPlayerDB::GetCharNameW(&pData[j].m_Param);
      if (!strncmp(name, pwszName, nameLen))
      {
        return &pData[j];
      }
    }
  }

  return nullptr;
}

void CPlayer::pc_ForceRequest(unsigned __int16 wForceSerial, _CHRID *pidDst, unsigned __int16 *pConsumeSerial)
{
  unsigned __int8 errorCode = 0;
  _STORAGE_LIST::_db_con *forceItem = nullptr;
  _base_fld *forceRecord = nullptr;
  CCharacter *dst = nullptr;
  unsigned __int16 delPoint[8]{};
  int forceLevel = 0;
  _STORAGE_LIST::_db_con *consumeItems[6]{};
  int consumeCounts[11]{};
  bool consumeOverlap[16]{};

  if (IsRidingUnit())
  {
    errorCode = 14;
    goto RESULT;
  }
  if (m_byMoveType == 2)
  {
    errorCode = 28;
    goto RESULT;
  }
  if (_effect_parameter::GetEff_State(&m_EP, 20))
  {
    errorCode = 24;
    goto RESULT;
  }
  if (_effect_parameter::GetEff_State(&m_EP, 28))
  {
    errorCode = 24;
    goto RESULT;
  }
  if (_effect_parameter::GetEff_State(&m_EP, 21))
  {
    errorCode = 25;
    goto RESULT;
  }
  if (m_dwSelfDestructionTime)
  {
    errorCode = 26;
    goto RESULT;
  }
  if (m_bInGuildBattle && m_bTakeGravityStone)
  {
    errorCode = 27;
    goto RESULT;
  }

  forceItem = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbForce, wForceSerial);
  if (!forceItem)
  {
    errorCode = 1;
    goto RESULT;
  }
  if (forceItem->m_bLock)
  {
    errorCode = 17;
    goto RESULT;
  }

  forceRecord = CRecordData::GetRecord(
    &g_Main.m_tblEffectData[1],
    *(CPlayer::s_pnLinkForceItemToEffect + forceItem->m_wItemIndex));
  if (!forceRecord)
  {
    errorCode = 1;
    goto RESULT;
  }

  if (!m_bSFDelayNotCheck
      && !_ATTACK_DELAY_CHECKER::IsDelay(
        &m_AttDelayChker,
        1u,
        forceRecord->m_dwIndex,
        forceRecord[1].m_strCode[4]))
  {
    errorCode = 9;
    goto RESULT;
  }
  if (*reinterpret_cast<int *>(&forceRecord[12].m_strCode[12]) >= 150)
  {
    errorCode = 21;
    goto RESULT;
  }

  dst = static_cast<CCharacter *>(CMainThread::GetObjectA(&g_Main, 0, pidDst->byID, pidDst->wIndex));
  if (!dst)
  {
    errorCode = 2;
    goto RESULT;
  }
  if (!dst->m_bLive || dst->m_pCurMap != m_pCurMap)
  {
    errorCode = 2;
    goto RESULT;
  }
  if (!CCharacter::IsEffectableDst(this, forceRecord[6].m_strCode, dst))
  {
    errorCode = 5;
    goto RESULT;
  }

  if (*reinterpret_cast<int *>(&forceRecord[12].m_strCode[48]) != -1)
  {
    if (_effect_parameter::GetEff_State(&m_EP, 4) && !*reinterpret_cast<int *>(&forceRecord[12].m_strCode[48]))
    {
      errorCode = 7;
      goto RESULT;
    }
    if (_effect_parameter::GetEff_State(&m_EP, 3) && *reinterpret_cast<int *>(&forceRecord[12].m_strCode[48]) == 1)
    {
      errorCode = 7;
      goto RESULT;
    }
    if (!*reinterpret_cast<int *>(&forceRecord[12].m_strCode[48]) && !IsAttackableInTown(this)
        && !dst->IsAttackableInTown(dst))
    {
      if (IsInTown(this) || dst->IsInTown(dst)
          || (m_Param.m_pGuild
              && CGuildRoomSystem::IsGuildRoomMemberIn(
                CGuildRoomSystem::GetInstance(),
                m_Param.m_pGuild->m_dwSerial,
                m_ObjID.m_wIndex,
                m_pUserDB->m_dwSerial)))
      {
        errorCode = 18;
        goto RESULT;
      }
    }
  }

  if (*reinterpret_cast<int *>(&forceRecord[12].m_strCode[12]) == -1
      && *reinterpret_cast<int *>(&forceRecord[12].m_strCode[48]) == -1)
  {
    errorCode = 8;
    goto RESULT;
  }
  if (*reinterpret_cast<int *>(&forceRecord[12].m_strCode[48]) != -1 && !dst->IsRecvableContEffect(dst))
  {
    errorCode = 13;
    goto RESULT;
  }
  if (_effect_parameter::GetEff_State(&dst->m_EP, 20))
  {
    if (*reinterpret_cast<int *>(&forceRecord[12].m_strCode[12]) == -1)
    {
      errorCode = 24;
      goto RESULT;
    }
    if (!IsUsableTempEffectAtStoneState(*reinterpret_cast<int *>(&forceRecord[12].m_strCode[12])))
    {
      errorCode = 24;
      goto RESULT;
    }
  }

  if (_effect_parameter::GetEff_State(&dst->m_EP, 28))
  {
    errorCode = 24;
  }
  else if (IsSFUsableSFMastery(4u, *reinterpret_cast<int *>(&forceRecord[1].m_strCode[4])))
  {
    if (IsSFActableByClass(1u, forceRecord))
    {
      if (IsSFUseableRace(1u, forceRecord->m_dwIndex))
      {
        if (IsSFUsableGauge(1u, forceRecord->m_dwIndex, delPoint))
        {
          memset(consumeItems, 0, 24);
          memset(consumeCounts, 0, 12);
          memset(consumeOverlap, 0, 3);
          if (GetUseConsumeItem(
                reinterpret_cast<_consume_item_list *>(&forceRecord[9].m_strCode[16]),
                pConsumeSerial,
                consumeItems,
                consumeCounts,
                consumeOverlap))
          {
            forceLevel = GetSFLevel(*reinterpret_cast<int *>(&forceRecord[4].m_strCode[60]), forceItem->m_dwDur);
          }
          else
          {
            errorCode = 32;
          }
        }
        else
        {
          errorCode = 6;
        }
      }
      else
      {
        errorCode = 4;
      }
    }
    else
    {
      errorCode = 16;
    }
  }
  else
  {
    errorCode = 3;
  }

RESULT:
  if (!errorCode)
  {
    const bool stealth = CCharacter::GetStealth(this, 1);
    bool upMty = false;
    bool acted = false;
    if (CCharacter::AssistForce(this, dst, reinterpret_cast<_force_fld *>(forceRecord), forceLevel, &errorCode, &upMty))
    {
      acted = true;
      if (upMty && !IsInTown(this))
      {
        const int dstLevel = dst->GetLevel(dst);
        const int myLevel = GetLevel(this);
        if (_CheckDestMonsterLimitLv(myLevel, dstLevel, dst->m_ObjID.m_byID))
        {
          if (!m_Param.m_pGuild
              || !CGuildRoomSystem::IsGuildRoomMemberIn(
                CGuildRoomSystem::GetInstance(),
                m_Param.m_pGuild->m_dwSerial,
                m_ObjID.m_wIndex,
                m_pUserDB->m_dwSerial))
          {
            unsigned int alter = 1;
            if (GetObjRace(this) != dst->GetObjRace(dst) || !IsChaosMode(this)
                || *reinterpret_cast<int *>(&forceRecord[12].m_strCode[48]))
            {
              if (dst->m_ObjID.m_byID || GetObjRace(this) == dst->GetObjRace(dst))
              {
                Emb_AlterStat(4u, forceRecord[1].m_strCode[4], alter, 0, "CPlayer::pc_ForceRequest()---0", 1);
              }
              else
              {
                auto *dstPlayer = static_cast<CPlayer *>(dst);
                if (!CPlayer::IsPunished(dstPlayer, 1u, 0))
                {
                  Emb_AlterStat(4u, forceRecord[1].m_strCode[4], alter, 0, "CPlayer::pc_ForceRequest()---0", 1);
                }
              }
            }

            const unsigned int oldDur = forceItem->m_dwDur;
            int alterValue = static_cast<int>(alter);
            alterValue *= GetLevel(this) / 10 + 1;
            const int baseAlter = alterValue;
            if (IsApplyPcbangPrimium())
            {
              alterValue =
                static_cast<int>(static_cast<float>(alterValue)
                                 + static_cast<float>(baseAlter)
                                   * static_cast<float>(PCBANG_PRIMIUM_FAVOR::SKILL_FORCE_MASTERY - 1.0f));
            }
            else
            {
              alterValue = static_cast<int>(
                static_cast<float>(alterValue)
                + static_cast<float>(baseAlter) * static_cast<float>(FORCE_LIVER_ACCUM_RATE - 1.0f));
            }
            const float effHave = _effect_parameter::GetEff_Have(&m_EP, 6);
            if (effHave > 1.0f)
            {
              alterValue = static_cast<int>(
                static_cast<float>(alterValue) + static_cast<float>(baseAlter) * static_cast<float>(effHave - 1.0f));
            }

            if (GetObjRace(this) != dst->GetObjRace(dst) || !IsChaosMode(this)
                || *reinterpret_cast<int *>(&forceRecord[12].m_strCode[48]))
            {
              unsigned int newStat = oldDur;
              if (alterValue > 0)
              {
                newStat = Emb_AlterDurPoint(
                  forceItem->m_pInList->m_nListCode,
                  forceItem->m_byStorageIndex,
                  alterValue,
                  0,
                  0);
              }
              if (newStat != oldDur)
              {
                SendMsg_FcitemInform(wForceSerial, newStat);
              }
            }
          }
        }
      }
    }

    if (acted)
    {
      for (int paramCode = 0; paramCode < 3; ++paramCode)
      {
        if (delPoint[paramCode])
        {
          int newValue = 0;
          const int gauge = GetGauge(paramCode);
          if (gauge - delPoint[paramCode] > 0)
          {
            newValue = GetGauge(paramCode) - delPoint[paramCode];
          }
          SetGauge(paramCode, newValue, 1);
        }
      }
      SendMsg_Recover();
      DeleteUseConsumeItem(consumeItems, consumeCounts, consumeOverlap);
      const float effPlus = _effect_parameter::GetEff_Plus(&m_EP, 13);
      _ATTACK_DELAY_CHECKER::SetDelay(
        &m_AttDelayChker,
        static_cast<int>(static_cast<float>(*reinterpret_cast<float *>(&forceRecord[9].m_strCode[52]) + effPlus)));
      if (stealth)
      {
        CCharacter::BreakStealth(this);
      }
    }
  }

  SendMsg_ForceResult(errorCode, pidDst, forceItem, static_cast<char>(forceLevel));
}

void CPlayer::pc_SkillRequest(unsigned __int8 bySkillIndex, _CHRID *pidDst, unsigned __int16 *pConsumeSerial)
{
  int skillLevel[1]{1};
  const bool stealth = CCharacter::GetStealth(this, 1);
  const unsigned __int8 result = skill_process(0, static_cast<char>(bySkillIndex), pidDst, pConsumeSerial, skillLevel);
  if ((!result || result == 100) && stealth)
  {
    CCharacter::BreakStealth(this);
  }
  SendMsg_SkillResult(result, pidDst, bySkillIndex, static_cast<unsigned __int8>(skillLevel[0]));
}

void CPlayer::pc_ClassSkillRequest(unsigned __int16 wSkillIndex, _CHRID *pidDst, unsigned __int16 *pConsumeSerial)
{
  const bool stealth = CCharacter::GetStealth(this, 1);
  const unsigned __int8 result = skill_process(2, static_cast<char>(wSkillIndex), pidDst, pConsumeSerial, nullptr);
  if ((!result || result == 100) && stealth)
  {
    CCharacter::BreakStealth(this);
  }
  SendMsg_ClassSkillResult(result, pidDst, wSkillIndex);
}

void CPlayer::pc_MakeTowerRequest(
  unsigned __int16 wSkillIndex,
  unsigned __int16 wTowerItemSerial,
  unsigned __int8 byMaterialNum,
  _make_tower_request_clzo::__material *pMaterial,
  float *pfPos,
  unsigned __int16 *pConsumeSerial)
{
  unsigned __int8 errorCode = 0;
  _base_fld *towerItemRecord = nullptr;
  _STORAGE_LIST::_db_con *towerItem = nullptr;
  _STORAGE_LIST::_db_con *materialItems[30]{};
  unsigned __int8 materialCounts[3]{};
  _base_fld *skillRecord = CRecordData::GetRecord(&g_Main.m_tblEffectData[2], wSkillIndex);
  int fpCost = 0;
  int classGrade[9]{};
  classGrade[0] = -1;
  _STORAGE_LIST::_db_con *consumeItems[6]{};
  int consumeCounts[11]{};
  bool consumeOverlap[20]{};

  if (m_byPosRaceTown == 255)
  {
    if (CGuardTower::IsHaveEmpty())
    {
      if (m_byMoveType == 2)
      {
        errorCode = 13;
      }
      else if (_TOWER_PARAM::IsEmpty(&m_pmTwr))
      {
        if (CPlayerDB::IsActableClassSkill(&m_Param, skillRecord->m_strCode, classGrade))
        {
          if (m_bSFDelayNotCheck
              || _ATTACK_DELAY_CHECKER::IsDelay(&m_AttDelayChker, 2u, skillRecord->m_dwIndex, classGrade[0]))
          {
            memset(consumeItems, 0, 24);
            memset(consumeCounts, 0, 12);
            memset(consumeOverlap, 0, 3);
            if (GetUseConsumeItem(
                  reinterpret_cast<_consume_item_list *>(&skillRecord[9].m_strCode[16]),
                  pConsumeSerial,
                  consumeItems,
                  consumeCounts,
                  consumeOverlap))
            {
              if (GetSqrt(m_fCurPos, pfPos) <= 40.0)
              {
                if (IsOtherTowerNear(this, pfPos, nullptr))
                {
                  errorCode = 15;
                }
                else if (skillRecord[13].m_dwIndex == -1)
                {
                  errorCode = 12;
                }
                else
                {
                  auto tempFunc = g_TempEffectFunc[skillRecord[13].m_dwIndex];
                  if (tempFunc == DE_MakeGuardTower)
                  {
                    const float baseCost = static_cast<float>(*reinterpret_cast<int *>(&skillRecord[9].m_strCode[8]));
                    const float rate = _effect_parameter::GetEff_Rate(&m_EP, 7);
                    fpCost = static_cast<int>(baseCost * rate);
                    if (fpCost <= GetFP())
                    {
                      towerItem = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, wTowerItemSerial);
                      if (towerItem)
                      {
                        if (towerItem->m_byTableCode == 25)
                        {
                          towerItemRecord =
                            CRecordData::GetRecord(&g_Main.m_tblItemData[25], towerItem->m_wItemIndex);
                          if (towerItem->m_bLock)
                          {
                            errorCode = 17;
                          }
                          else
                          {
                            for (int j = 0; j < byMaterialNum; ++j)
                            {
                              const unsigned __int16 itemSerial = pMaterial[j].wItemSerial;
                              for (int k = 0; k < j; ++k)
                              {
                                if (itemSerial == pMaterial[k].wItemSerial)
                                {
                                  errorCode = 4;
                                  goto RESULT;
                                }
                              }

                              materialItems[j] =
                                _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, pMaterial[j].wItemSerial);
                              if (!materialItems[j])
                              {
                                errorCode = 5;
                                goto RESULT;
                              }

                              _base_fld *materialRecord = CRecordData::GetRecord(
                                &g_Main.m_tblItemData[materialItems[j]->m_byTableCode],
                                materialItems[j]->m_wItemIndex);
                              if (strncmp(
                                    materialRecord->m_strCode,
                                    &towerItemRecord[pMaterial[j].byMaterSlotIndex + 7].m_strCode[4],
                                    7u))
                              {
                                errorCode = 6;
                                goto RESULT;
                              }

                              if (static_cast<unsigned __int64>(pMaterial[j].byAmount) > materialItems[j]->m_dwDur)
                              {
                                errorCode = 7;
                                goto RESULT;
                              }
                              materialCounts[pMaterial[j].byMaterSlotIndex] += pMaterial[j].byAmount;
                            }

                            for (int j = 0; j < 3; ++j)
                            {
                              if (static_cast<unsigned __int8>(materialCounts[j])
                                  != *reinterpret_cast<unsigned int *>(towerItemRecord[j + 8].m_strCode))
                              {
                                errorCode = 8;
                                break;
                              }
                            }
                          }
                        }
                        else
                        {
                          errorCode = 3;
                        }
                      }
                      else
                      {
                        errorCode = 2;
                      }
                    }
                    else
                    {
                      errorCode = 14;
                    }
                  }
                  else
                  {
                    errorCode = 12;
                  }
                }
              }
              else
              {
                errorCode = 9;
              }
            }
            else
            {
              errorCode = 20;
            }
          }
          else
          {
            errorCode = 18;
          }
        }
        else
        {
          errorCode = 13;
        }
      }
      else
      {
        errorCode = 11;
      }
    }
    else
    {
      errorCode = 1;
    }
  }
  else
  {
    errorCode = 16;
  }

RESULT:
  unsigned int towerSerial = static_cast<unsigned int>(-1);
  if (!errorCode)
  {
    int newFp = 0;
    const int curFp = GetFP();
    if (curFp - fpCost > 0)
    {
      newFp = GetFP() - fpCost;
    }
    SetFP(newFp, 1);
    const unsigned __int8 raceCode = CPlayerDB::GetRaceCode(&m_Param);
    CGuardTower *tower =
      CreateGuardTower(m_pCurMap, m_wMapLayerIndex, pfPos, towerItem, this, raceCode, false);
    if (tower)
    {
      for (int j = 0; j < byMaterialNum; ++j)
      {
        Emb_AlterDurPoint(0, materialItems[j]->m_byStorageIndex, -pMaterial[j].byAmount, 0, 0);
      }
      _STORAGE_LIST::_storage_con::lock(towerItem, true);
      _TOWER_PARAM::PushList(&m_pmTwr, towerItem, tower);
      ++m_pmTwr.m_nCount;
      towerSerial = tower->m_dwObjSerial;
      DeleteUseConsumeItem(consumeItems, consumeCounts, consumeOverlap);
      const float effPlus = _effect_parameter::GetEff_Plus(&m_EP, 12);
      _ATTACK_DELAY_CHECKER::SetDelay(
        &m_AttDelayChker,
        static_cast<int>(static_cast<float>(*reinterpret_cast<float *>(&skillRecord[9].m_strCode[52]) + effPlus)));
    }
    else
    {
      errorCode = 1;
    }
  }

  SendMsg_CreateTowerResult(errorCode, towerSerial);
}

void CPlayer::pc_BackTowerRequest(unsigned int dwTowerObjSerial)
{
  unsigned __int8 errorCode = 0;
  _STORAGE_LIST::_db_con *towerItem = nullptr;

  for (int j = 0; j < 6; ++j)
  {
    if (m_pmTwr.m_List[j].m_pTowerItem && m_pmTwr.m_List[j].m_pTowerObj->m_dwObjSerial == dwTowerObjSerial)
    {
      _STORAGE_LIST::_db_con *listItem = m_pmTwr.m_List[j].m_pTowerItem;
      towerItem = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, listItem->m_wSerial);
      if (!towerItem)
      {
        errorCode = 2;
        goto RESULT2;
      }
      if (towerItem->m_byTableCode != 25)
      {
        errorCode = 3;
        goto RESULT2;
      }
      break;
    }
  }
  if (!towerItem)
  {
    errorCode = 3;
  }

RESULT2:
  unsigned __int16 itemSerial = static_cast<unsigned __int16>(-1);
  unsigned __int16 leftHp = 0;
  if (!errorCode)
  {
    leftHp = _TowerReturn(towerItem);
    itemSerial = towerItem->m_wSerial;
  }
  SendMsg_BackTowerResult(errorCode, itemSerial, leftHp);
}

void CPlayer::pc_MakeTrapRequest(
  unsigned __int16 wSkillIndex,
  unsigned __int16 wTrapItemSerial,
  float *pfPos,
  unsigned __int16 *pConsumeSerial)
{
  unsigned __int8 errorCode = 0;
  _TrapItem_fld *trapItemInfo = nullptr;
  _STORAGE_LIST::_db_con *trapItem = nullptr;
  _base_fld *skillRecord = CRecordData::GetRecord(&g_Main.m_tblEffectData[2], wSkillIndex);
  int fpCost = 0;
  int classGrade[9]{};
  classGrade[0] = -1;
  _STORAGE_LIST::_db_con *consumeItems[6]{};
  int consumeCounts[11]{};
  bool consumeOverlap[20]{};

  if (m_byPosRaceTown == 255)
  {
    if (CTrap::IsHaveEmpty())
    {
      if (m_byMoveType == 2)
      {
        errorCode = 13;
      }
      else if (_TRAP_PARAM::GetNum(&m_pmTrp) < m_Param.m_nMakeTrapMaxNum)
      {
        if (CPlayerDB::IsActableClassSkill(&m_Param, skillRecord->m_strCode, classGrade))
        {
          if (m_bSFDelayNotCheck
              || _ATTACK_DELAY_CHECKER::IsDelay(&m_AttDelayChker, 2u, skillRecord->m_dwIndex, classGrade[0]))
          {
            memset(consumeItems, 0, 24);
            memset(consumeCounts, 0, 12);
            memset(consumeOverlap, 0, 3);
            if (GetUseConsumeItem(
                  reinterpret_cast<_consume_item_list *>(&skillRecord[9].m_strCode[16]),
                  pConsumeSerial,
                  consumeItems,
                  consumeCounts,
                  consumeOverlap))
            {
              if (GetSqrt(m_fCurPos, pfPos) <= 40.0)
              {
                trapItem = _STORAGE_LIST::GetPtrFromSerial(&m_Param.m_dbInven, wTrapItemSerial);
                if (trapItem)
                {
                  if (trapItem->m_byTableCode == 26)
                  {
                    trapItemInfo = reinterpret_cast<_TrapItem_fld *>(
                      CRecordData::GetRecord(&g_Main.m_tblItemData[26], trapItem->m_wItemIndex));
                    errorCode = IsOtherInvalidObjNear(this, pfPos, nullptr, trapItemInfo);
                    if (!errorCode)
                    {
                      if (skillRecord[13].m_dwIndex == -1)
                      {
                        errorCode = 12;
                      }
                      else
                      {
                        auto tempFunc = g_TempEffectFunc[skillRecord[13].m_dwIndex];
                        if (tempFunc == DE_LayTrap)
                        {
                          const float baseCost = static_cast<float>(
                            *reinterpret_cast<int *>(&skillRecord[9].m_strCode[8]));
                          const float rate = _effect_parameter::GetEff_Rate(&m_EP, 7);
                          fpCost = static_cast<int>(baseCost * rate);
                          if (fpCost <= GetFP())
                          {
                            if (trapItemInfo->m_nLevelLim >= 40
                                && (!m_Param.m_pClassHistory[0]
                                    || m_Param.m_pClassHistory[0]->m_nClass != 1
                                    || !m_Param.m_pClassHistory[1]
                                    || m_Param.m_pClassHistory[1]->m_nClass != 1))
                            {
                              errorCode = 19;
                            }
                          }
                          else
                          {
                            errorCode = 14;
                          }
                        }
                        else
                        {
                          errorCode = 12;
                        }
                      }
                    }
                  }
                  else
                  {
                    errorCode = 3;
                  }
                }
                else
                {
                  errorCode = 2;
                }
              }
              else
              {
                errorCode = 9;
              }
            }
            else
            {
              errorCode = 20;
            }
          }
          else
          {
            errorCode = 18;
          }
        }
        else
        {
          errorCode = 13;
        }
      }
      else
      {
        errorCode = 11;
      }
    }
    else
    {
      errorCode = 1;
    }
  }
  else
  {
    errorCode = 16;
  }

  unsigned int trapSerial = static_cast<unsigned int>(-1);
  if (!errorCode)
  {
    int newFp = 0;
    const int curFp = GetFP();
    if (curFp - fpCost > 0)
    {
      newFp = GetFP() - fpCost;
    }
    SetFP(newFp, 1);
    CTrap *trap = CreateTrap(m_pCurMap, m_wMapLayerIndex, pfPos, this, trapItem->m_wItemIndex);
    if (trap)
    {
      if (IsOverLapItem(26))
      {
        Emb_AlterDurPoint(0, trapItem->m_byStorageIndex, -1, 1, 1);
      }
      else if (!Emb_DelStorage(0, trapItem->m_byStorageIndex, 0, 1, "CPlayer::pc_MakeTrapRequest()"))
      {
        SendMsg_CreateTrapResult(0xCu, trapSerial);
        return;
      }
      CMgrAvatorItemHistory::consume_del_item(
        &CPlayer::s_MgrItemHistory,
        m_ObjID.m_wIndex,
        trapItem,
        m_szItemHistoryFileName);
      _TRAP_PARAM::PushItem(&m_pmTrp, trap, trap->m_dwObjSerial);
      SendMsg_MadeTrapNumInform(m_pmTrp.m_nCount);
      trapSerial = trap->m_dwObjSerial;
      DeleteUseConsumeItem(consumeItems, consumeCounts, consumeOverlap);
      const float effPlus = _effect_parameter::GetEff_Plus(&m_EP, 12);
      _ATTACK_DELAY_CHECKER::SetDelay(
        &m_AttDelayChker,
        static_cast<int>(static_cast<float>(*reinterpret_cast<float *>(&skillRecord[9].m_strCode[52]) + effPlus)));
    }
    else
    {
      errorCode = 1;
    }
  }

  SendMsg_CreateTrapResult(errorCode, trapSerial);
}

void CPlayer::pc_BackTrapRequest(unsigned int dwTrapObjSerial, unsigned __int16 wAddSerial)
{
  unsigned __int8 errorCode = 2;
  CTrap *trap = nullptr;

  if (m_pmTrp.m_nCount > 0)
  {
    for (int j = 0; j < 20; ++j)
    {
      if (_TRAP_PARAM::_param::isLoad(&m_pmTrp.m_Item[j]) && m_pmTrp.m_Item[j].dwSerial == dwTrapObjSerial)
      {
        trap = m_pmTrp.m_Item[j].pItem;
        if (!trap)
        {
          errorCode = 2;
          goto RESULT3;
        }
        if (trap->m_dwMasterSerial != m_dwObjSerial)
        {
          errorCode = 21;
          goto RESULT3;
        }
        if (GetSqrt(m_fCurPos, trap->m_fCurPos) > 150.0)
        {
          errorCode = 22;
          goto RESULT3;
        }
        errorCode = 0;
        break;
      }
    }
    if (errorCode)
    {
      const char *charName = CPlayerDB::GetCharNameA(&m_Param);
      CLogFile::Write(
        &g_Main.m_logSystemError,
        "CPlayer::pc_BackTrapRequest() : Can't find trap (Player:%s Count:%d)",
        charName,
        m_pmTrp.m_nCount);
    }
  }
  else
  {
    if (m_pmTrp.m_nCount < 0)
    {
      CLogFile::Write(
        &g_Main.m_logSystemError,
        "CPlayer::pc_BackTrapRequest() : m_pmTrp.m_nCount ZERO and less ( %d )",
        m_pmTrp.m_nCount);
    }
    errorCode = 2;
  }

RESULT3:
  if (!errorCode)
  {
    _TrapReturn(trap, wAddSerial);
    CTrap::Destroy(trap, 2u);
  }
  SendMsg_BackTrapResult(errorCode);
}

void CPlayer::pc_ThrowSkillRequest(unsigned __int16 wBulletSerial, _CHRID *pidDst, unsigned __int16 *pConsumeSerial)
{
  unsigned __int8 errorCode = 0;
  _STORAGE_LIST::_db_con *bulletItem = nullptr;
  _base_fld *bulletRecord = nullptr;
  _skill_fld *skillRecord = nullptr;
  CCharacter *dst = nullptr;
  _STORAGE_LIST::_db_con *consumeItems[6]{};
  int consumeCounts[11]{};
  bool consumeOverlap[16]{};

  if (IsRidingUnit())
  {
    errorCode = 14;
  }
  else if (m_byMoveType == 2)
  {
    errorCode = 28;
  }
  else if (_effect_parameter::GetEff_State(&m_EP, 20))
  {
    errorCode = 24;
  }
  else if (_effect_parameter::GetEff_State(&m_EP, 28))
  {
    errorCode = 24;
  }
  else if (_effect_parameter::GetEff_State(&m_EP, 21))
  {
    errorCode = 25;
  }
  else if (m_pmWpn.byWpType == 11 || m_pmWpn.byWpType == 6)
  {
    if (m_pmWpn.byWpType == 6
        && (!m_pmWpn.pFixWp || m_pmWpn.pFixWp->m_byTableCode != 6 || m_pmWpn.pFixWp->m_wItemIndex != 8518))
    {
      errorCode = 22;
    }
    bulletItem = IsBulletValidity(wBulletSerial);
    if (bulletItem)
    {
      if (bulletItem->m_bLock)
      {
        errorCode = 20;
      }
      else
      {
        bulletRecord = CRecordData::GetRecord(&g_Main.m_tblItemData[10], bulletItem->m_wItemIndex);
        if (bulletRecord)
        {
          if (!strncmp(reinterpret_cast<const char *>(&bulletRecord[7]), "-1", 2u))
          {
            errorCode = 19;
          }
          else
          {
            skillRecord = reinterpret_cast<_skill_fld *>(
              CRecordData::GetRecord(&g_Main.m_tblEffectData[3], reinterpret_cast<const char *>(&bulletRecord[7])));
            if (skillRecord)
            {
              if (m_bSFDelayNotCheck || _ATTACK_DELAY_CHECKER::IsDelay(&m_AttDelayChker, 0xFFu, 0xFFu, 0xFFu))
              {
                dst = static_cast<CCharacter *>(CMainThread::GetObjectA(&g_Main, 0, pidDst->byID, pidDst->wIndex));
                if (dst)
                {
                  if (dst->m_bLive && dst->m_pCurMap == m_pCurMap)
                  {
                    if (CCharacter::IsEffectableDst(this, skillRecord->m_strActableDst, dst))
                    {
                      if (skillRecord->m_nTempEffectType == -1 && skillRecord->m_nContEffectType == -1)
                      {
                        errorCode = 8;
                      }
                      else
                      {
                        if (skillRecord->m_nContEffectType != -1)
                        {
                          if (!dst->IsRecvableContEffect(dst))
                          {
                            errorCode = 13;
                            goto RESULT_THROW_SKILL;
                          }
                          if (!skillRecord->m_nContEffectType && !IsAttackableInTown(this)
                              && !dst->IsAttackableInTown(dst))
                          {
                            if (IsInTown(this) || dst->IsInTown(dst)
                                || (m_Param.m_pGuild
                                    && CGuildRoomSystem::IsGuildRoomMemberIn(
                                      CGuildRoomSystem::GetInstance(),
                                      m_Param.m_pGuild->m_dwSerial,
                                      m_ObjID.m_wIndex,
                                      m_pUserDB->m_dwSerial)))
                            {
                              errorCode = 18;
                              goto RESULT_THROW_SKILL;
                            }
                          }
                        }
                        memset(consumeItems, 0, 24);
                        memset(consumeCounts, 0, 12);
                        memset(consumeOverlap, 0, 3);
                        if (GetUseConsumeItem(
                              skillRecord->m_ConsumeItemList,
                              pConsumeSerial,
                              consumeItems,
                              consumeCounts,
                              consumeOverlap))
                        {
                          if (skillRecord->m_nContEffectType != -1)
                          {
                            if (skillRecord->m_nContEffectType == 1)
                            {
                              if (_pre_check_in_guild_battle_race(static_cast<CPlayer *>(dst), 0))
                              {
                                errorCode = 13;
                              }
                            }
                            else if (_pre_check_in_guild_battle_race(static_cast<CPlayer *>(dst), 1)
                                     && !CCharacter::IsEffectableDst(this, skillRecord->m_strActableDst, dst))
                            {
                              errorCode = 13;
                            }
                          }
                        }
                        else
                        {
                          errorCode = 32;
                        }
                      }
                    }
                    else
                    {
                      errorCode = 5;
                    }
                  }
                  else
                  {
                    errorCode = 2;
                  }
                }
                else
                {
                  errorCode = 2;
                }
              }
              else
              {
                errorCode = 9;
              }
            }
            else
            {
              errorCode = 8;
            }
          }
        }
        else
        {
          errorCode = 19;
        }
      }
    }
    else
    {
      errorCode = 19;
    }
  }
  else
  {
    errorCode = 22;
  }

RESULT_THROW_SKILL:
  unsigned __int8 skillIndex = static_cast<unsigned __int8>(-1);
  g_tmpEffectedNum = 0;
  if (!errorCode)
  {
    skillIndex = static_cast<unsigned __int8>(skillRecord->m_dwIndex);
    const bool stealth = CCharacter::GetStealth(this, 1);
    bool upMty = false;
    if (CCharacter::AssistSkill(this, dst, 3, skillRecord, 1, &errorCode, &upMty))
    {
      const unsigned __int16 newDur = Emb_AlterDurPoint(2u, bulletItem->m_byStorageIndex, -1, 0, 1);
      if (newDur)
      {
        SendMsg_AlterWeaponBulletInform(bulletItem->m_wSerial, newDur);
      }
      else
      {
        CMgrAvatorItemHistory::consume_del_item(
          &CPlayer::s_MgrItemHistory,
          m_ObjID.m_wIndex,
          bulletItem,
          m_szItemHistoryFileName);
      }
      DeleteUseConsumeItem(consumeItems, consumeCounts, consumeOverlap);
      const int addDelay = static_cast<int>(CalcEquipAttackDelay());
      const int level = GetLevel(this);
      unsigned int delay = _WEAPON_PARAM::GetAttackDelay(&m_pmWpn, level, addDelay);
      if (m_pmWpn.byWpType != 11 && !m_pmWpn.byWpClass)
      {
        const float effPlus = _effect_parameter::GetEff_Plus(&m_EP, 9);
        delay = static_cast<int>(static_cast<float>(static_cast<int>(delay)) + effPlus);
      }
      if (m_pmWpn.byWpType == 7 || m_pmWpn.byWpType == 11)
      {
        const float effPlus = _effect_parameter::GetEff_Plus(&m_EP, 11);
        delay = static_cast<int>(static_cast<float>(static_cast<int>(delay)) + effPlus);
      }
      _ATTACK_DELAY_CHECKER::SetDelay(&m_AttDelayChker, delay);
      if (stealth)
      {
        CCharacter::BreakStealth(this);
      }
    }
  }

  SendMsg_ThrowSkillResult(errorCode, pidDst, skillIndex);
}

void CPlayer::pc_ThrowUnitRequest(_CHRID *pidDst, unsigned __int16 *pConsumeSerial)
{
  unsigned __int8 errorCode = 0;
  _base_fld *bulletRecord = nullptr;
  _base_fld *partRecord = nullptr;
  unsigned int *unitBullet = nullptr;
  _skill_fld *skillRecord = nullptr;
  CCharacter *dst = nullptr;
  _STORAGE_LIST::_db_con *consumeItems[6]{};
  int consumeCounts[11]{};
  bool consumeOverlap[16]{};

  if (IsRidingUnit())
  {
    unitBullet = &m_pUsingUnit->dwBullet[1];
    if (*reinterpret_cast<unsigned __int16 *>(unitBullet + 1)
        && *reinterpret_cast<unsigned __int16 *>(unitBullet + 1) != 0xFFFF)
    {
      bulletRecord = CRecordData::GetRecord(&g_Main.m_tblUnitBullet, *reinterpret_cast<unsigned __int16 *>(unitBullet));
      if (bulletRecord)
      {
        partRecord = CRecordData::GetRecord(&g_Main.m_tblUnitPart[4], m_pUsingUnit->byPart[4]);
        if (partRecord)
        {
          if (!strncmp(&bulletRecord[3].m_strCode[20], "-1", 2u))
          {
            errorCode = 19;
          }
          else
          {
            skillRecord = reinterpret_cast<_skill_fld *>(
              CRecordData::GetRecord(&g_Main.m_tblEffectData[3], &bulletRecord[3].m_strCode[20]));
            if (skillRecord)
            {
              if (m_bSFDelayNotCheck || _ATTACK_DELAY_CHECKER::IsDelay(&m_AttDelayChker, 0xFFu, 0xFFu, 0xFFu))
              {
                dst = static_cast<CCharacter *>(CMainThread::GetObjectA(&g_Main, 0, pidDst->byID, pidDst->wIndex));
                if (dst)
                {
                  if (dst->m_bLive && dst->m_pCurMap == m_pCurMap)
                  {
                    if (CCharacter::IsEffectableDst(this, skillRecord->m_strActableDst, dst))
                    {
                      if (skillRecord->m_nTempEffectType == -1 && skillRecord->m_nContEffectType == -1)
                      {
                        errorCode = 8;
                      }
                      else
                      {
                        if (skillRecord->m_nContEffectType != -1)
                        {
                          if (!dst->IsRecvableContEffect(dst))
                          {
                            errorCode = 13;
                            goto RESULT_THROW_UNIT;
                          }
                          if (!skillRecord->m_nContEffectType && !IsAttackableInTown(this)
                              && !dst->IsAttackableInTown(dst))
                          {
                            if (IsInTown(this) || dst->IsInTown(dst)
                                || (m_Param.m_pGuild
                                    && CGuildRoomSystem::IsGuildRoomMemberIn(
                                      CGuildRoomSystem::GetInstance(),
                                      m_Param.m_pGuild->m_dwSerial,
                                      m_ObjID.m_wIndex,
                                      m_pUserDB->m_dwSerial)))
                            {
                              errorCode = 18;
                              goto RESULT_THROW_UNIT;
                            }
                          }
                        }
                        memset(consumeItems, 0, 24);
                        memset(consumeCounts, 0, 12);
                        memset(consumeOverlap, 0, 3);
                        if (GetUseConsumeItem(
                              skillRecord->m_ConsumeItemList,
                              pConsumeSerial,
                              consumeItems,
                              consumeCounts,
                              consumeOverlap))
                        {
                          if (skillRecord->m_nContEffectType != -1)
                          {
                            if (skillRecord->m_nContEffectType == 1)
                            {
                              if (_pre_check_in_guild_battle_race(static_cast<CPlayer *>(dst), 0))
                              {
                                errorCode = 13;
                              }
                            }
                            else if (_pre_check_in_guild_battle_race(static_cast<CPlayer *>(dst), 1))
                            {
                              errorCode = 13;
                            }
                          }
                        }
                        else
                        {
                          errorCode = 32;
                        }
                      }
                    }
                    else
                    {
                      errorCode = 5;
                    }
                  }
                  else
                  {
                    errorCode = 2;
                  }
                }
                else
                {
                  errorCode = 2;
                }
              }
              else
              {
                errorCode = 9;
              }
            }
            else
            {
              errorCode = 8;
            }
          }
        }
        else
        {
          errorCode = 21;
        }
      }
      else
      {
        errorCode = 21;
      }
    }
    else
    {
      errorCode = 19;
    }
  }
  else
  {
    errorCode = 23;
  }

RESULT_THROW_UNIT:
  unsigned __int16 bulletIndex = static_cast<unsigned __int16>(-1);
  g_tmpEffectedNum = 0;
  if (!errorCode)
  {
    bulletIndex = static_cast<unsigned __int16>(bulletRecord->m_dwIndex);
    const bool stealth = CCharacter::GetStealth(this, 1);
    bool upMty = false;
    if (CCharacter::AssistSkill(this, dst, 3, skillRecord, 1, &errorCode, &upMty))
    {
      SendMsg_AlterUnitBulletInform(1u, --*reinterpret_cast<unsigned __int16 *>(unitBullet + 1));
      if (!*reinterpret_cast<unsigned __int16 *>(unitBullet + 1))
      {
        *unitBullet = static_cast<unsigned int>(-1);
      }
      DeleteUseConsumeItem(consumeItems, consumeCounts, consumeOverlap);
      _ATTACK_DELAY_CHECKER::SetDelay(&m_AttDelayChker, *reinterpret_cast<int *>(&partRecord[4].m_strCode[48]));
      if (stealth)
      {
        CCharacter::BreakStealth(this);
      }
    }
  }

  SendMsg_ThrowUnitResult(errorCode, pidDst, bulletIndex);
}

unsigned __int8 CPlayer::skill_process(
  int nEffectCode,
  char nSkillIndex,
  _CHRID *pidDst,
  unsigned __int16 *pConsumeSerial,
  int *pnLv)
{
  auto *skill = reinterpret_cast<_skill_fld *>(CRecordData::GetRecord(&g_Main.m_tblEffectData[nEffectCode], nSkillIndex));
  unsigned __int16 delPoint[8]{};
  int skillLevel = 1;
  CCharacter *dst = nullptr;
  int classGrade[4]{};
  classGrade[0] = -1;

  if (nEffectCode)
  {
    if (nEffectCode == 2 && !CPlayerDB::IsActableClassSkill(&m_Param, skill->m_strCode, classGrade))
    {
      return 15;
    }
  }
  else
  {
    if (skill->m_nMastIndex > 8u)
    {
      return 8;
    }
    if (!_pre_check_skill_enable(skill))
    {
      return 21;
    }
    if (!_pre_check_skill_gradelimit(skill))
    {
      return 34;
    }
    if (skill->m_nClass == 4 && (IsChaosMode() || CPlayer::IsPunished(this, 1u, 0)))
    {
      return 35;
    }
  }

  if ((!nEffectCode || nEffectCode == 2) && m_bInGuildBattle && m_bTakeGravityStone)
  {
    return 27;
  }

  int masteryIndex = -1;
  if (nEffectCode)
  {
    if (nEffectCode == 2)
    {
      masteryIndex = classGrade[0];
    }
  }
  else
  {
    if (skill->m_nMastIndex < 8u)
    {
      masteryIndex = skill->m_nMastIndex;
    }
    if (skill->m_nClass == 2)
    {
      masteryIndex = -1;
    }
  }

  if (!m_bSFDelayNotCheck
      && !_ATTACK_DELAY_CHECKER::IsDelay(&m_AttDelayChker, nEffectCode, skill->m_dwIndex, masteryIndex))
  {
    return 9;
  }
  if (skill->m_nTempEffectType >= 150)
  {
    return 21;
  }
  if (skill->m_nContEffectType == 1 && _effect_parameter::GetEff_State(&m_EP, 2))
  {
    return 7;
  }
  if (IsRidingUnit())
  {
    return 14;
  }
  if (CPlayerDB::GetRaceCode(&m_Param) == 2 && IsActingSiegeMode())
  {
    return 21;
  }
  if (m_dwSelfDestructionTime)
  {
    return 26;
  }

  dst = static_cast<CCharacter *>(CMainThread::GetObjectA(&g_Main, 0, pidDst->byID, pidDst->wIndex));
  if (!dst)
  {
    return 2;
  }
  if (!CCharacter::IsEffectableDst(this, skill->m_strActableDst, dst))
  {
    return 5;
  }
  if (!dst->m_bLive)
  {
    return 2;
  }
  if (skill->m_nTempEffectType == -1 && skill->m_nContEffectType == -1)
  {
    return 8;
  }
  if (skill->m_nContEffectType != -1 && !dst->IsRecvableContEffect(dst))
  {
    return 13;
  }
  if (_effect_parameter::GetEff_State(&dst->m_EP, 20))
  {
    if (skill->m_nTempEffectType == -1)
    {
      return 24;
    }
    if (!IsUsableTempEffectAtStoneState(skill->m_nTempEffectType))
    {
      return 24;
    }
  }
  if (_effect_parameter::GetEff_State(&dst->m_EP, 28))
  {
    return 24;
  }
  if (_effect_parameter::GetEff_State(&m_EP, 21))
  {
    return 25;
  }
  if (!skill->m_nContEffectType && !IsAttackableInTown(this) && !dst->IsAttackableInTown(dst))
  {
    if (IsInTown(this))
    {
      return 18;
    }
    if (dst->IsInTown(dst))
    {
      return 18;
    }
    if (m_Param.m_pGuild)
    {
      if (CGuildRoomSystem::IsGuildRoomMemberIn(
            CGuildRoomSystem::GetInstance(),
            m_Param.m_pGuild->m_dwSerial,
            m_ObjID.m_wIndex,
            m_pUserDB->m_dwSerial))
      {
        return 18;
      }
    }
  }

  if (!nEffectCode)
  {
    if (skill->m_nClass == 2)
    {
      const unsigned __int8 enableResult = CEquipItemSFAgent::IsEnableSkill(&EquipItemSFAgent, skill);
      if (enableResult)
      {
        return enableResult;
      }
    }
    else
    {
      if (!IsSFUsableSFMastery(3u, skill->m_nMastIndex))
      {
        return 3;
      }
      if (!IsSFActableByClass(0, skill))
      {
        return 16;
      }
    }
  }

  if (m_byMoveType == 2)
  {
    return 28;
  }
  if (!IsSFUseableRace(nEffectCode, skill->m_dwIndex))
  {
    return 4;
  }
  if (!IsSFUsableGauge(nEffectCode, skill->m_dwIndex, delPoint))
  {
    return 6;
  }

  _STORAGE_LIST::_db_con *consumeItems[6]{};
  int consumeCounts[11]{};
  bool consumeOverlap[32]{};
  memset(consumeItems, 0, 24);
  memset(consumeCounts, 0, 12);
  memset(consumeOverlap, 0, 3);
  if (!GetUseConsumeItem(skill->m_ConsumeItemList, pConsumeSerial, consumeItems, consumeCounts, consumeOverlap))
  {
    return 32;
  }

  if (!nEffectCode)
  {
    const float masteryLv = static_cast<float>(_MASTERY_PARAM::GetSkillLv(&m_pmMst, nSkillIndex));
    const float effPlus = _effect_parameter::GetEff_Plus(&m_EP, 19);
    skillLevel = static_cast<int>(masteryLv + effPlus);
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
  bool upMty = false;
  bool acted = false;
  if (CCharacter::AssistSkill(this, dst, nEffectCode, skill, skillLevel, &errorCode, &upMty))
  {
    acted = true;
    if (upMty && !IsInTown(this))
    {
      if (!m_Param.m_pGuild
          || !CGuildRoomSystem::IsGuildRoomMemberIn(
            CGuildRoomSystem::GetInstance(),
            m_Param.m_pGuild->m_dwSerial,
            m_ObjID.m_wIndex,
            m_pUserDB->m_dwSerial))
      {
        if (skill->m_nClass != 2 && skill->m_nClass != 4 && !nEffectCode && skill->m_nMastIndex < 8u)
        {
          unsigned int alter = 1;
          if (skill->m_nClass == 1)
          {
            alter = 2;
          }
          if (GetObjRace(this) != dst->GetObjRace(dst) || !IsChaosMode() || skill->m_nContEffectType)
          {
            if (GetObjRace(this) != dst->GetObjRace(dst) || dst->m_ObjID.m_byID)
            {
              Emb_AlterStat(3u, skill->m_dwIndex, alter, 0, "CPlayer::skill_process()---0", 1);
            }
            else
            {
              auto *dstPlayer = static_cast<CPlayer *>(dst);
              if (!CPlayer::IsPunished(dstPlayer, 1u, 0))
              {
                Emb_AlterStat(3u, skill->m_dwIndex, alter, 0, "CPlayer::skill_process()---0", 1);
              }
            }
          }
        }
      }
    }

    if (skill->m_nClass == 4)
    {
      CMyTimer::BeginTimer(&m_tmrAuraSkill, 0x1388u);
      skill_process_for_aura(skill->m_dwIndex);
    }
  }

  if (acted)
  {
    for (int paramCode = 0; paramCode < 3; ++paramCode)
    {
      if (delPoint[paramCode])
      {
        int newValue = 0;
        const int gauge = GetGauge(paramCode);
        if (gauge - delPoint[paramCode] > 0)
        {
          newValue = GetGauge(paramCode) - delPoint[paramCode];
        }
        SetGauge(paramCode, newValue, 1);
      }
    }
    SendMsg_Recover();
    DeleteUseConsumeItem(consumeItems, consumeCounts, consumeOverlap);
    const float effPlus = _effect_parameter::GetEff_Plus(&m_EP, 12);
    _ATTACK_DELAY_CHECKER::SetDelay(
      &m_AttDelayChker,
      static_cast<int>(static_cast<float>(skill->m_fActDelay + effPlus)));
  }

  return errorCode;
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
  _base_fld *record = CRecordData::GetRecord(g_Main.m_tblEffectData, nSkillIndex);
  if (record && record[1].m_dwIndex == 4)
  {
    auto *skill = reinterpret_cast<_skill_fld *>(CRecordData::GetRecord(g_Main.m_tblEffectData, record[12].m_strCode));
    if (skill)
    {
      const bool contEffect = skill->m_nContEffectType == 1;
      CCharacter *dstList[31]{};
      const int memberCount = CCharacter::_GetAreaEffectMember(
        this,
        this,
        contEffect,
        record[12].m_dwIndex,
        m_fCurPos,
        skill->m_strActableDst,
        dstList);
      unsigned __int8 errorCode = 0;
      bool upMty = false;
      for (int j = 0; j < memberCount; ++j)
      {
        if (!dstList[j]->m_ObjID.m_byKind && !dstList[j]->m_ObjID.m_byID)
        {
          auto *dstPlayer = static_cast<CPlayer *>(dstList[j]);
          const unsigned int serial = CPlayerDB::GetCharSerial(&dstPlayer->m_Param);
          const int race = CPlayerDB::GetRaceCode(&dstPlayer->m_Param);
          CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
          const unsigned __int8 bossType = CPvpUserAndGuildRankingSystem::GetBossType(ranking, race, serial);
          if (bossType != 4 && bossType != 8)
          {
            CCharacter::AssistSkill(this, dstList[j], 0, skill, 7, &errorCode, &upMty);
          }
        }
      }
    }
  }
}

void CPlayer::SendMsg_ForceResult(
  char byErrCode,
  _CHRID *pidDst,
  _STORAGE_LIST::_db_con *pForceItem,
  char nSFLv)
{
  _force_result_one_zocl result{};
  result.byErrCode = static_cast<unsigned __int8>(byErrCode);
  result.dwItemCum = pForceItem ? pForceItem->m_dwDur : 0;
  result.byAttackSerial = 0;

  unsigned __int8 type[2]{17, 2};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&result),
    sizeof(result));

  if ((!byErrCode || byErrCode == 100)
      && (m_nCirclePlayerNum <= 500 || m_ObjID.m_byID != pidDst->byID || m_dwObjSerial != pidDst->dwSerial))
  {
    _force_result_other_zocl other{};
    other.byRetCode = static_cast<unsigned __int8>(byErrCode);
    other.byForceIndex =
      pForceItem ? static_cast<unsigned __int8>(CPlayer::s_pnLinkForceItemToEffect[pForceItem->m_wItemIndex]) : 0xFF;
    other.byForceLv = static_cast<unsigned __int8>(nSFLv);
    other.idPerformer.byID = m_ObjID.m_byID;
    other.idPerformer.wIndex = m_ObjID.m_wIndex;
    other.idPerformer.dwSerial = m_dwObjSerial;
    other.idDster = *pidDst;
    other.byAttackSerial = 0;

    unsigned __int8 circleType[2]{17, 3};
    CGameObject::CircleReport(this, circleType, reinterpret_cast<char *>(&other), sizeof(other), 0);
  }
}

void CPlayer::SendMsg_SkillResult(
  char byErrCode,
  _CHRID *pidDst,
  unsigned __int8 bySkillIndex,
  unsigned __int8 nSFLv)
{
  _skill_result_one_zocl result{};
  result.byErrCode = static_cast<unsigned __int8>(byErrCode);
  result.byAttackSerial = 0;

  unsigned __int8 type[2]{17, 5};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&result),
    sizeof(result));

  if ((!byErrCode || byErrCode == 100)
      && (m_nCirclePlayerNum <= 500 || m_ObjID.m_byID != pidDst->byID || m_dwObjSerial != pidDst->dwSerial))
  {
    _skill_result_other_zocl other{};
    other.byRetCode = static_cast<unsigned __int8>(byErrCode);
    other.idPerformer.byID = m_ObjID.m_byID;
    other.idPerformer.wIndex = m_ObjID.m_wIndex;
    other.idPerformer.dwSerial = m_dwObjSerial;
    other.idDster = *pidDst;
    other.bySkillIndex = bySkillIndex;
    other.bySkillLv = nSFLv;
    other.byAttackSerial = 0;

    unsigned __int8 circleType[2]{17, 6};
    CGameObject::CircleReport(this, circleType, reinterpret_cast<char *>(&other), sizeof(other), 0);
  }
}

void CPlayer::SendMsg_ClassSkillResult(char byErrCode, _CHRID *pidDst, unsigned __int16 wSkillIndex)
{
  _class_skill_result_one_zocl result{};
  result.byErrCode = static_cast<unsigned __int8>(byErrCode);
  result.byAttackSerial = 0;

  unsigned __int8 type[2]{17, 8};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&result),
    sizeof(result));

  if ((!byErrCode || byErrCode == 100)
      && (m_nCirclePlayerNum <= 500 || m_ObjID.m_byID != pidDst->byID || m_dwObjSerial != pidDst->dwSerial))
  {
    _class_skill_result_other_zocl other{};
    other.byRetCode = static_cast<unsigned __int8>(byErrCode);
    other.idPerformer.byID = m_ObjID.m_byID;
    other.idPerformer.wIndex = m_ObjID.m_wIndex;
    other.idPerformer.dwSerial = m_dwObjSerial;
    other.idDster = *pidDst;
    other.wSkillIndex = wSkillIndex;
    other.byAttackSerial = 0;

    unsigned __int8 circleType[2]{17, 9};
    CGameObject::CircleReport(this, circleType, reinterpret_cast<char *>(&other), sizeof(other), 0);
  }
}

void CPlayer::SendMsg_CreateTowerResult(char byErrCode, unsigned int dwTowerObjSerial)
{
  _make_tower_result_zocl msg{};
  msg.byErrCode = static_cast<unsigned __int8>(byErrCode);
  msg.dwTowerObjSerial = dwTowerObjSerial;
  msg.wLeftFP = static_cast<unsigned __int16>(GetFP());

  unsigned __int8 type[2]{17, 19};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    sizeof(msg));
}

void CPlayer::SendMsg_BackTowerResult(char byErrCode, unsigned __int16 wItemSerial, unsigned __int16 wLeftHP)
{
  _back_tower_result_zocl msg{};
  msg.byErrCode = static_cast<unsigned __int8>(byErrCode);
  msg.wItemSerial = wItemSerial;
  msg.wLeftHP = wLeftHP;

  unsigned __int8 type[2]{17, 21};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    sizeof(msg));
}

void CPlayer::SendMsg_CreateTrapResult(char byErrCode, unsigned int dwTrapObjSerial)
{
  _make_trap_result_zocl msg{};
  msg.byErrCode = static_cast<unsigned __int8>(byErrCode);
  msg.dwTrapObjSerial = dwTrapObjSerial;
  msg.wLeftFP = static_cast<unsigned __int16>(GetFP());

  unsigned __int8 type[2]{17, 28};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    sizeof(msg));
}

void CPlayer::SendMsg_BackTrapResult(char byErrCode)
{
  _back_trap_result_zocl msg{};
  msg.byErrCode = static_cast<unsigned __int8>(byErrCode);

  unsigned __int8 type[2]{17, 39};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    sizeof(msg));
}

void CPlayer::SendMsg_ThrowSkillResult(unsigned __int8 byErrCode, _CHRID *pidDst, unsigned __int8 bySkillIndex)
{
  _throw_skill_result_one_zocl selfMsg;
  selfMsg.byErrCode = byErrCode;
  selfMsg.byAttackSerial = 0;
  selfMsg.idDster = *pidDst;
  selfMsg.byEffectedNum = static_cast<unsigned __int8>(g_tmpEffectedNum);
  for (int j = 0; j < g_tmpEffectedNum; ++j)
  {
    selfMsg.list[j].byObjID = g_tmpEffectedList[j].pMem->m_ObjID.m_byID;
    selfMsg.list[j].dwObjSerial = g_tmpEffectedList[j].pMem->m_dwObjSerial;
    selfMsg.list[j].byRetCode = g_tmpEffectedList[j].byRetCode;
    selfMsg.list[j].wEffectValue = g_tmpEffectedList[j].wEffectValue;
  }

  unsigned __int8 type[2]{17, 100};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&selfMsg),
    sizeof(selfMsg));

  if (!byErrCode)
  {
    _throw_skill_result_other_zocl otherMsg;
    otherMsg.byRetCode = 0;
    otherMsg.idPerformer.byID = m_ObjID.m_byID;
    otherMsg.idPerformer.wIndex = m_ObjID.m_wIndex;
    otherMsg.idPerformer.dwSerial = m_dwObjSerial;
    otherMsg.bySkillIndex = bySkillIndex;
    otherMsg.byAttackSerial = 0;
    otherMsg.idDster = *pidDst;
    otherMsg.byEffectedNum = static_cast<unsigned __int8>(g_tmpEffectedNum);
    memcpy_0(otherMsg.list, selfMsg.list, 8LL * g_tmpEffectedNum);

    unsigned __int8 circleType[2]{17, 101};
    CGameObject::CircleReport(this, circleType, reinterpret_cast<char *>(&otherMsg), 258, 0);
  }
}

void CPlayer::SendMsg_ThrowUnitResult(unsigned __int8 byErrCode, _CHRID *pidDst, unsigned __int16 wBulletIndex)
{
  _throw_unit_result_one_zocl selfMsg;
  selfMsg.byErrCode = byErrCode;
  selfMsg.byAttackSerial = 0;
  selfMsg.idDster = *pidDst;
  selfMsg.byEffectedNum = static_cast<unsigned __int8>(g_tmpEffectedNum);
  for (int j = 0; j < g_tmpEffectedNum; ++j)
  {
    selfMsg.list[j].byObjID = g_tmpEffectedList[j].pMem->m_ObjID.m_byID;
    selfMsg.list[j].dwObjSerial = g_tmpEffectedList[j].pMem->m_dwObjSerial;
    selfMsg.list[j].byRetCode = g_tmpEffectedList[j].byRetCode;
    selfMsg.list[j].wEffectValue = g_tmpEffectedList[j].wEffectValue;
  }

  unsigned __int8 type[2]{17, 103};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&selfMsg),
    sizeof(selfMsg));

  if (!byErrCode)
  {
    _throw_unit_result_other_zocl otherMsg;
    otherMsg.byRetCode = 0;
    otherMsg.idPerformer.byID = m_ObjID.m_byID;
    otherMsg.idPerformer.wIndex = m_ObjID.m_wIndex;
    otherMsg.idPerformer.dwSerial = m_dwObjSerial;
    otherMsg.wBulletIndex = wBulletIndex;
    otherMsg.byAttackSerial = 0;
    otherMsg.idDster = *pidDst;
    otherMsg.byEffectedNum = static_cast<unsigned __int8>(g_tmpEffectedNum);
    memcpy_0(otherMsg.list, selfMsg.list, 8LL * g_tmpEffectedNum);

    unsigned __int8 circleType[2]{17, 104};
    CGameObject::CircleReport(this, circleType, reinterpret_cast<char *>(&otherMsg), 259, 0);
  }
}

unsigned __int16 CPlayer::_DeleteUnitKey(unsigned __int8 bySlotIndex)
{
  for (int slotIndex = 0;; ++slotIndex)
  {
    if (slotIndex >= m_Param.m_dbInven.m_nUsedNum)
    {
      return static_cast<unsigned __int16>(-1);
    }

    _STORAGE_LIST::_db_con *slot = &m_Param.m_dbInven.m_pStorageList[slotIndex];
    if (slot->m_bLoad && slot->m_dwLv == bySlotIndex)
    {
      if (Emb_DelStorage(this, 0, slotIndex, 0, 1, "CPlayer::_DeleteUnitKey()"))
      {
        return slot->m_wSerial;
      }
      return static_cast<unsigned __int16>(-1);
    }
  }
}

void CPlayer::_UnitDestroy(unsigned __int8 byUnitSlot)
{
  if (byUnitSlot >= 4)
  {
    return;
  }

  _UNIT_DB_BASE::_LIST *unitData = &m_Param.m_UnitDB.m_List[byUnitSlot];
  if (unitData->byFrame == 255)
  {
    return;
  }

  _base_fld *frameRecord = CRecordData::GetRecord(&g_Main.m_tblUnitFrame, unitData->byFrame);
  if (frameRecord && (!*reinterpret_cast<int *>(&frameRecord[1].m_strCode[60]) || *reinterpret_cast<int *>(&frameRecord[1].m_strCode[56])))
  {
    CMgrAvatorItemHistory::destroy_unit(
      &CPlayer::s_MgrItemHistory,
      m_ObjID.m_wIndex,
      byUnitSlot,
      unitData->byFrame,
      m_szItemHistoryFileName);
    _UNIT_DB_BASE::_LIST::Init(unitData, 0xFFu);
    _DeleteUnitKey(byUnitSlot);
    if (m_pUserDB)
    {
      CUserDB::Update_UnitDelete(m_pUserDB, byUnitSlot);
    }
  }
  else
  {
    unitData->dwGauge = 0;
    CUserDB::Update_UnitData(m_pUserDB, byUnitSlot, unitData);
    _LockUnitKey(m_pUsingUnit->bySlotIndex, false);
  }

  if (m_pUsingUnit == unitData)
  {
    m_pUsingUnit = nullptr;
    m_pParkingUnit = nullptr;
  }

  SendMsg_UnitDestroy(byUnitSlot);
}

void CPlayer::SendMsg_UnitDestroy(char bySlotIndex)
{
  unsigned __int8 type[2]{23, 26};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&bySlotIndex),
    sizeof(bySlotIndex));
}

void CPlayer::SendMsg_UnitRideChange(bool bTake, CParkingUnit *pUnit)
{
#pragma pack(push, 1)
  struct UnitRideChangeMsg
  {
    unsigned __int16 wIndex;
    unsigned int dwObjSerial;
    __int16 wVisualVer;
    bool bTake;
    unsigned int dwUnitSerial;
    __int16 zPos[3];
  };
#pragma pack(pop)

  UnitRideChangeMsg msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwObjSerial = m_dwObjSerial;
  msg.wVisualVer = GetVisualVer();
  msg.bTake = bTake;
  msg.dwUnitSerial = pUnit ? pUnit->m_dwObjSerial : 0;
  FloatToShort(m_fCurPos, msg.zPos, 3);

  unsigned __int8 type[2]{3, 35};
  CGameObject::CircleReport(this, type, reinterpret_cast<char *>(&msg), sizeof(msg), 0);
}

void CPlayer::Emb_RidindUnit(bool bRiding, CParkingUnit *pCreateUnit)
{
  CashChangeStateFlag changeFlag(0);
  UpdateVisualVer(changeFlag);

  if (bRiding)
  {
    SendMsg_UnitRideChange(true, m_pParkingUnit);
    m_pParkingUnit->Destroy(1);
    m_pParkingUnit = nullptr;
    _effect_parameter::SetLock(&m_EP, 1);
    _WEAPON_PARAM::FixUnit(&m_pmWpn, m_pUsingUnit);
    m_nUnitDefFc = 0;
    for (int partIndex = 0; partIndex < 6; ++partIndex)
    {
      if (m_pUsingUnit->byPart[partIndex] != 255)
      {
        _base_fld *partRecord = CRecordData::GetRecord(&g_Main.m_tblUnitPart[partIndex], m_pUsingUnit->byPart[partIndex]);
        if (partRecord)
        {
          m_nUnitDefFc += *reinterpret_cast<int *>(&partRecord[5].m_strCode[8]);
        }
      }
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
    _effect_parameter::SetLock(&m_EP, 0);
    _STORAGE_LIST::_db_con *weaponSlot = m_Param.m_dbEquip.m_pStorageList + 6;
    if (weaponSlot->m_bLoad)
    {
      _WEAPON_PARAM::FixWeapon(&m_pmWpn, weaponSlot);
    }
    else
    {
      _WEAPON_PARAM::FixWeapon(&m_pmWpn, nullptr);
    }
  }

  CalcDefTol();
  SetHaveEffect(0);
  SetShapeAllBuffer();
  CheckAlterMaxPoint();
}

void CPlayer::pc_UnitFrameBuyRequest(unsigned __int8 byFrameCode, int bUseNPCLinkIntem)
{
  unsigned __int8 resultCode = 0;
  unsigned __int8 emptySlotIndex = 0xFF;
  std::array<unsigned int, 7> consumMoney{};
  _base_fld *frameRecord = CRecordData::GetRecord(&g_Main.m_tblUnitFrame, byFrameCode);
  _UnitKeyItem_fld *unitKeyMatch = nullptr;
  _base_fld *partRecord = nullptr;
  std::array<unsigned __int8, 6> partIndices{};
  partIndices.fill(static_cast<unsigned __int8>(-1));

  if (!m_pUserDB)
  {
    return;
  }

  if (TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, m_id.wIndex) == 99)
  {
    resultCode = 34;
  }
  else
  {
    const unsigned int taxRate = eGetTexRate(0) + 10000;
    [[maybe_unused]] const float tax = eGetTex(0) + 1.0f;
    if (bUseNPCLinkIntem || IsBeNearStore(this, 4))
    {
      if (frameRecord)
      {
        if (CPlayerDB::GetRaceCode(&m_Param))
        {
          resultCode = 1;
        }
        else if (m_pUsingUnit)
        {
          resultCode = 2;
        }
        else
        {
          for (int j = 0; j < 4; ++j)
          {
            if (m_Param.m_UnitDB.m_List[j].byFrame == 255)
            {
              emptySlotIndex = static_cast<unsigned __int8>(j);
              break;
            }
          }
          if (emptySlotIndex == 0xFF)
          {
            resultCode = 3;
          }
          else if (_STORAGE_LIST::GetNumEmptyCon(&m_Param.m_dbInven))
          {
            unitKeyMatch = gGetUnitKeyMatchFrame(byFrameCode);
            if (unitKeyMatch)
            {
              unsigned __int64 costValue = *reinterpret_cast<int *>(&frameRecord[1].m_strCode[28]);
              costValue *= taxRate;
              const int moneyType = *reinterpret_cast<int *>(&frameRecord[1].m_strCode[24]);
              consumMoney[moneyType] = static_cast<unsigned int>(costValue / 0x2710);
              const unsigned int availableMoney = GetMoney(moneyType);
              if (consumMoney[moneyType] <= availableMoney)
              {
                const char *partCodeBase = reinterpret_cast<const char *>(frameRecord + 2);
                for (int j = 0; j < 6; ++j)
                {
                  if (strncmp(partCodeBase + 64LL * j, "-1", 2uLL))
                  {
                    partRecord = CRecordData::GetRecord(&g_Main.m_tblUnitPart[j], partCodeBase + 64LL * j);
                    if (!partRecord)
                    {
                      resultCode = 100;
                      break;
                    }
                    partIndices[j] = static_cast<unsigned __int8>(partRecord->m_dwIndex);
                  }
                }
              }
              else
              {
                resultCode = 7;
              }
            }
            else
            {
              resultCode = 6;
            }
          }
          else
          {
            resultCode = 4;
          }
        }
      }
      else
      {
        resultCode = 9;
      }
    }
    else
    {
      resultCode = 21;
    }
  }

  unsigned __int16 keySerial = static_cast<unsigned __int16>(-1);
  unsigned __int16 keyIndex = static_cast<unsigned __int16>(-1);
  if (!resultCode)
  {
    _UNIT_DB_BASE::_LIST *unitSlot = &m_Param.m_UnitDB.m_List[emptySlotIndex];
    _UNIT_DB_BASE::_LIST::DelUnit(unitSlot);
    unitSlot->byFrame = byFrameCode;
    for (int j = 0; j < 6; ++j)
    {
      unitSlot->byPart[j] = partIndices[j];
    }
    unitSlot->dwGauge = *reinterpret_cast<unsigned int *>(frameRecord[1].m_strCode);
    CUserDB::Update_UnitInsert(m_pUserDB, emptySlotIndex, unitSlot);
    keySerial = CPlayerDB::GetNewItemSerial(&m_Param);
    keyIndex = static_cast<unsigned __int16>(unitKeyMatch->m_dwIndex);
    _STORAGE_LIST::_storage_con con(0x13u, keyIndex, 0, emptySlotIndex, keySerial);
    if (!Emb_AddStorage(this, 0, &con, 0, 1))
    {
      SendMsg_UnitFrameBuyResult(
        static_cast<unsigned __int8>(-1),
        byFrameCode,
        emptySlotIndex,
        keyIndex,
        keySerial,
        consumMoney.data());
      return;
    }
    SubDalant(consumMoney[0]);
    SubGold(consumMoney[1]);

    const unsigned int currentGold = CPlayerDB::GetGold(&m_Param);
    const unsigned int currentDalant = CPlayerDB::GetDalant(&m_Param);
    CMgrAvatorItemHistory::buy_unit(
      &CPlayer::s_MgrItemHistory,
      m_ObjID.m_wIndex,
      emptySlotIndex,
      unitSlot,
      consumMoney.data(),
      currentDalant,
      currentGold,
      m_szItemHistoryFileName);

    if (!m_byUserDgr)
    {
      if (consumMoney[1])
      {
        eAddGold(0, consumMoney[1]);
      }
      if (consumMoney[0])
      {
        eAddDalant(0, consumMoney[0]);
      }
    }

    const int level = CPlayerDB::GetLevel(&m_Param);
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      if (consumMoney[1])
      {
        const unsigned int amount = 2000 * consumMoney[1];
        const int lv = CPlayerDB::GetLevel(&m_Param);
        CMoneySupplyMgr::UpdateBuyUnitData(CMoneySupplyMgr::Instance(), lv, amount);
      }
      if (consumMoney[0])
      {
        const int lv = CPlayerDB::GetLevel(&m_Param);
        CMoneySupplyMgr::UpdateBuyUnitData(CMoneySupplyMgr::Instance(), lv, consumMoney[0]);
      }
    }
  }

  SendMsg_UnitFrameBuyResult(resultCode, byFrameCode, emptySlotIndex, keyIndex, keySerial, consumMoney.data());
}

void CPlayer::SendMsg_UnitFrameBuyResult(
  char byRetCode,
  char byFrameCode,
  char byUnitSlotIndex,
  unsigned __int16 wKeyIndex,
  unsigned __int16 wKeySerial,
  unsigned int *pdwConsumMoney)
{
  _unit_frame_buy_result_zocl msg{};
  msg.byRetCode = static_cast<unsigned __int8>(byRetCode);
  msg.byFrameCode = static_cast<unsigned __int8>(byFrameCode);
  msg.byAddUnitSlot = static_cast<unsigned __int8>(byUnitSlotIndex);
  msg.byKeyIndex = static_cast<unsigned __int8>(wKeyIndex);
  msg.wKeySerial = wKeySerial;
  msg.dwLeftMoney[0] = CPlayerDB::GetDalant(&m_Param);
  msg.dwLeftMoney[1] = CPlayerDB::GetGold(&m_Param);
  msg.dwConsumMoney[0] = pdwConsumMoney[0];
  msg.dwConsumMoney[1] = pdwConsumMoney[1];

  unsigned __int8 type[2]{23, 2};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    sizeof(msg));
}

void CPlayer::pc_UnitSellRequest(unsigned __int8 bySlotIndex, int bUseNPCLinkIntem)
{
  unsigned __int8 resultCode = 0;
  const unsigned __int8 frameCode = m_Param.m_UnitDB.m_List[bySlotIndex].byFrame;
  _base_fld *frameRecord = CRecordData::GetRecord(&g_Main.m_tblUnitFrame, frameCode);
  _UNIT_DB_BASE::_LIST *unitData = &m_Param.m_UnitDB.m_List[bySlotIndex];
  unsigned int totalNonPay = 0;
  const unsigned int taxRateMinus = 10000 - eGetTexRate(0);
  const unsigned int taxRatePlus = eGetTexRate(0) + 10000;
  [[maybe_unused]] const float taxMinus = 1.0f - eGetTex(0);
  [[maybe_unused]] const float taxPlus = eGetTex(0) + 1.0f;
  unsigned int sellMoney = 0;
  int addMoney = 0;
  float gaugeRate = 1.0f;

  if (!m_pUserDB)
  {
    return;
  }

  if (TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, m_id.wIndex) == 99)
  {
    resultCode = 34;
  }
  else if (bUseNPCLinkIntem || IsBeNearStore(this, 4))
  {
    if (frameRecord)
    {
      if (CPlayerDB::GetRaceCode(&m_Param))
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
        unsigned __int64 pullingFee = unitData->nPullingFee;
        pullingFee *= taxRatePlus;
        totalNonPay = static_cast<unsigned int>(pullingFee / 0x2710);
        const unsigned int dalant = CPlayerDB::GetDalant(&m_Param);
        if (totalNonPay <= dalant)
        {
          gaugeRate = static_cast<float>(static_cast<int>(unitData->dwGauge))
              / static_cast<float>(*reinterpret_cast<int *>(frameRecord[1].m_strCode));
          double sellBase = (static_cast<double>(*reinterpret_cast<int *>(&frameRecord[1].m_strCode[28])) * gaugeRate) / 2.0;
          unsigned __int64 baseValue = 0;
          if (sellBase > 9.223372e18)
          {
            sellBase -= 9.223372e18;
            baseValue = 0x7FFFFFFFFFFFFFFFULL;
            if (sellBase > 9.223372e18)
            {
              sellBase -= 9.223372e18;
              baseValue = 0xFFFFFFFFFFFFFFFEULL;
            }
          }
          baseValue += static_cast<unsigned int>(static_cast<int>(sellBase));
          unsigned __int64 sellValue = static_cast<unsigned __int64>(taxRateMinus) * baseValue + 5000;
          sellMoney = static_cast<unsigned int>(sellValue / 0x2710);
          addMoney = static_cast<int>(sellMoney) - static_cast<int>(totalNonPay);
          if (addMoney <= 0)
          {
            if (addMoney < 0)
            {
              const unsigned int needDalant = static_cast<unsigned int>(-addMoney);
              if (needDalant > CPlayerDB::GetDalant(&m_Param))
              {
                resultCode = 10;
              }
            }
          }
          else if (!CanAddMoneyForMaxLimMoney(addMoney, CPlayerDB::GetDalant(&m_Param)))
          {
            resultCode = 30;
          }
        }
        else
        {
          resultCode = 10;
        }
      }
    }
    else
    {
      resultCode = 9;
    }
  }
  else
  {
    resultCode = 21;
  }

  unsigned __int16 keySerial = static_cast<unsigned __int16>(-1);
  if (!resultCode)
  {
    unitData->byFrame = static_cast<unsigned __int8>(-1);
    keySerial = _DeleteUnitKey(bySlotIndex);
    const double penalty = TimeLimitMgr::GetPlayerPenalty(g_Main.m_pTimeLimitMgr, m_id.wIndex);
    addMoney = static_cast<int>(static_cast<double>(addMoney) * penalty);
    AlterDalant(static_cast<double>(addMoney));
    CUserDB::Update_UnitDelete(m_pUserDB, bySlotIndex);

    const unsigned int currentGold = CPlayerDB::GetGold(&m_Param);
    const unsigned int currentDalant = CPlayerDB::GetDalant(&m_Param);
    CMgrAvatorItemHistory::sell_unit(
      &CPlayer::s_MgrItemHistory,
      m_ObjID.m_wIndex,
      bySlotIndex,
      frameCode,
      gaugeRate,
      sellMoney,
      totalNonPay,
      currentDalant,
      currentGold,
      m_szItemHistoryFileName);
    if (!m_byUserDgr)
    {
      eAddDalant(0, addMoney);
    }
  }

  const unsigned int sumGold = CPlayerDB::GetGold(&m_Param);
  const unsigned int sumDalant = CPlayerDB::GetDalant(&m_Param);
  SendMsg_UnitSellResult(resultCode, bySlotIndex, keySerial, addMoney, totalNonPay, sumDalant, sumGold);
}

void CPlayer::SendMsg_UnitSellResult(
  char byRetCode,
  char bySlotIndex,
  unsigned __int16 wKeySerial,
  int nAddMoney,
  unsigned int dwTotalNonpay,
  unsigned int dwSumDalant,
  unsigned int dwSumGold)
{
  _unit_sell_result_zocl msg{};
  msg.byRetCode = static_cast<unsigned __int8>(byRetCode);
  msg.bySlotIndex = static_cast<unsigned __int8>(bySlotIndex);
  msg.wKeySerial = wKeySerial;
  msg.dwNonPayDalant = dwTotalNonpay;
  msg.nAddMoney[0] = nAddMoney;
  msg.nAddMoney[1] = 0;
  msg.dwLeftMoney[0] = dwSumDalant;
  msg.dwLeftMoney[1] = dwSumGold;

  unsigned __int8 type[2]{23, 4};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    sizeof(msg));
}

void CPlayer::pc_UnitPartTuningRequest(
  unsigned __int8 bySlotIndex,
  unsigned __int8 byTuningNum,
  _tuning_data *pTuningData,
  int bUseNPCLinkIntem)
{
  unsigned __int8 resultCode = 0;
  _UNIT_DB_BASE::_LIST *unitData = &m_Param.m_UnitDB.m_List[bySlotIndex];
  const unsigned __int8 frameCode = unitData->byFrame;
  std::array<unsigned int, 11> consumMoney{};
  const unsigned int taxRatePlus = eGetTexRate(0) + 10000;
  const unsigned int taxRateMinus = 10000 - eGetTexRate(0);
  [[maybe_unused]] const float taxPlus = eGetTex(0) + 1.0f;
  [[maybe_unused]] const float taxMinus = 1.0f - eGetTex(0);

  if (!m_pUserDB)
  {
    return;
  }

  if (TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, m_id.wIndex) == 99)
  {
    resultCode = 34;
  }
  else if (bUseNPCLinkIntem || IsBeNearStore(this, 4))
  {
    if (CPlayerDB::GetRaceCode(&m_Param))
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
      _base_fld *frameRecord = CRecordData::GetRecord(&g_Main.m_tblUnitFrame, frameCode);
      if (frameRecord)
      {
        if (*reinterpret_cast<int *>(&frameRecord[1].m_strCode[60]))
        {
          if (unitData->dwGauge)
          {
            for (int j = 0; j < byTuningNum; ++j)
            {
              _base_fld *partRecord =
                CRecordData::GetRecord(&g_Main.m_tblUnitPart[pTuningData[j].byPartCode], pTuningData[j].byPartIndex);
              if (!partRecord)
              {
                resultCode = 8;
                break;
              }
              if (reinterpret_cast<unsigned __int8 *>(&partRecord[3].m_dwIndex)[frameCode] == 48)
              {
                resultCode = 6;
                break;
              }
              if (unitData->byPart[pTuningData[j].byPartCode] == pTuningData[j].byPartIndex)
              {
                resultCode = 29;
                break;
              }
              const int level = CPlayerDB::GetLevel(&m_Param);
              if (*reinterpret_cast<int *>(&partRecord[4].m_strCode[12]) > level)
              {
                resultCode = 12;
                break;
              }
              if (*reinterpret_cast<int *>(&partRecord[4].m_strCode[16]) != -1)
              {
                const int checkLevel = CPlayerDB::GetLevel(&m_Param);
                if (*reinterpret_cast<int *>(&partRecord[4].m_strCode[16]) < checkLevel)
                {
                  resultCode = 12;
                  break;
                }
              }
              char *masteryData = &partRecord[4].m_strCode[20];
              for (int k = 0; k < 2; ++k)
              {
                const int masteryCode = *reinterpret_cast<int *>(&masteryData[8 * k]);
                if (masteryCode != -1)
                {
                  if (masteryCode >= 6)
                  {
                    resultCode = 13;
                    break;
                  }
                  const unsigned __int8 mastery = _MASTERY_PARAM::GetEquipMastery(&m_pmMst, masteryCode);
                  if (mastery < *reinterpret_cast<int *>(&masteryData[8 * k + 4]))
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

              unsigned __int64 costValue = *reinterpret_cast<int *>(&partRecord[5].m_strCode[40]);
              costValue *= taxRatePlus;
              const int moneyType = *reinterpret_cast<int *>(&partRecord[5].m_strCode[36]);
              const unsigned int currentValue = consumMoney[moneyType];
              consumMoney[moneyType] = static_cast<unsigned int>(costValue / 0x2710 + currentValue);
              if (pTuningData[j].byPartCode == 5)
              {
                for (int m = 0; m < 8; ++m)
                {
                  if (unitData->dwSpare[m] != static_cast<unsigned int>(-1))
                  {
                    unsigned int *spareEntry = &unitData->dwSpare[m];
                    _base_fld *bulletRecord =
                      CRecordData::GetRecord(&g_Main.m_tblUnitBullet, *reinterpret_cast<unsigned __int16 *>(spareEntry));
                    if (bulletRecord)
                    {
                      unsigned __int64 refund =
                        static_cast<unsigned __int64>(taxRateMinus)
                          * static_cast<unsigned int>(*reinterpret_cast<int *>(&bulletRecord[3].m_strCode[8]) / 2)
                        + 5000;
                      const int bulletMoneyType = *reinterpret_cast<int *>(&bulletRecord[3].m_strCode[4]);
                      const int current = static_cast<int>(consumMoney[bulletMoneyType]);
                      consumMoney[bulletMoneyType] = static_cast<unsigned int>(current - static_cast<int>(refund / 0x2710));
                    }
                  }
                }
              }
            }
            if (!resultCode)
            {
              for (int j = 0; j < 7; ++j)
              {
                if (consumMoney[j] > GetMoney(j))
                {
                  resultCode = 7;
                  break;
                }
              }
            }
          }
          else
          {
            resultCode = 36;
          }
        }
        else
        {
          resultCode = 38;
        }
      }
      else
      {
        resultCode = 5;
      }
    }
  }
  else
  {
    resultCode = 21;
  }

  if (!resultCode)
  {
    for (int j = 0; j < byTuningNum; ++j)
    {
      unitData->byPart[pTuningData[j].byPartCode] = pTuningData[j].byPartIndex;
      if (pTuningData[j].byPartCode == 5)
      {
        memset_0(unitData->dwSpare, -1, sizeof(unitData->dwSpare));
      }
      if (pTuningData[j].byPartCode == 3 || pTuningData[j].byPartCode == 4)
      {
        const int bulletSlot = gnBulletPerPart[pTuningData[j].byPartCode];
        if (bulletSlot != -1)
        {
          unsigned int *bulletEntry = &unitData->dwBullet[bulletSlot];
          const unsigned __int16 bulletCount = static_cast<unsigned __int16>(unitData->dwBullet[bulletSlot] >> 16);
          if (bulletCount)
          {
            if (bulletCount != 0xFFFF)
            {
              _base_fld *bulletRecord =
                CRecordData::GetRecord(&g_Main.m_tblUnitBullet, *reinterpret_cast<unsigned __int16 *>(bulletEntry));
              if (bulletRecord)
              {
                _base_fld *partRecord = CRecordData::GetRecord(
                  &g_Main.m_tblUnitPart[pTuningData[j].byPartCode],
                  pTuningData[j].byPartIndex);
                if (partRecord)
                {
                  if (*reinterpret_cast<unsigned int *>(&bulletRecord[2].m_strCode[60])
                      != *reinterpret_cast<unsigned int *>(&partRecord[3].m_strCode[60]))
                  {
                    unitData->dwBullet[bulletSlot] = static_cast<unsigned int>(-1);
                  }
                }
              }
            }
          }
        }
      }
    }

    SubDalant(consumMoney[0]);
    SubGold(consumMoney[1]);
    CUserDB::Update_UnitData(m_pUserDB, bySlotIndex, unitData);

    const unsigned int currentGold = CPlayerDB::GetGold(&m_Param);
    const unsigned int currentDalant = CPlayerDB::GetDalant(&m_Param);
    CMgrAvatorItemHistory::tuning_unit(
      &CPlayer::s_MgrItemHistory,
      m_ObjID.m_wIndex,
      bySlotIndex,
      unitData,
      reinterpret_cast<int *>(consumMoney.data()),
      currentDalant,
      currentGold,
      m_szItemHistoryFileName);
    if (!m_byUserDgr)
    {
      eAddGold(0, consumMoney[1]);
      eAddDalant(0, consumMoney[0]);
    }

    const int level = CPlayerDB::GetLevel(&m_Param);
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      if (static_cast<int>(consumMoney[1]) > 0)
      {
        const unsigned int amount = 2000 * consumMoney[1];
        const int lv = CPlayerDB::GetLevel(&m_Param);
        CMoneySupplyMgr::UpdateBuyUnitData(CMoneySupplyMgr::Instance(), lv, amount);
      }
      if (static_cast<int>(consumMoney[0]) > 0)
      {
        const int lv = CPlayerDB::GetLevel(&m_Param);
        CMoneySupplyMgr::UpdateBuyUnitData(CMoneySupplyMgr::Instance(), lv, consumMoney[0]);
      }
    }
  }

  SendMsg_UnitPartTuningResult(resultCode, bySlotIndex, reinterpret_cast<int *>(consumMoney.data()));
}

void CPlayer::SendMsg_UnitPartTuningResult(char byRetCode, char bySlotIndex, int *pnCost)
{
  _unit_part_tuning_result_zocl msg{};
  msg.byRetCode = static_cast<unsigned __int8>(byRetCode);
  msg.bySlotIndex = static_cast<unsigned __int8>(bySlotIndex);
  memcpy_0(msg.byPart, m_Param.m_UnitDB.m_List[static_cast<unsigned __int8>(bySlotIndex)].byPart, sizeof(msg.byPart));
  memcpy_0(msg.dwBullet, m_Param.m_UnitDB.m_List[static_cast<unsigned __int8>(bySlotIndex)].dwBullet, sizeof(msg.dwBullet));
  msg.nCost[0] = pnCost[0];
  msg.nCost[1] = pnCost[1];
  msg.dwLeftMoney[0] = CPlayerDB::GetDalant(&m_Param);
  msg.dwLeftMoney[1] = CPlayerDB::GetGold(&m_Param);

  unsigned __int8 type[2]{23, 6};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    sizeof(msg));
}

void CPlayer::pc_UnitFrameRepairRequest(unsigned __int8 bySlotIndex, int bUseNPCLinkIntem)
{
  unsigned __int8 resultCode = 0;
  _UNIT_DB_BASE::_LIST *unitData = &m_Param.m_UnitDB.m_List[bySlotIndex];
  const unsigned __int8 frameCode = unitData->byFrame;
  _base_fld *frameRecord = CRecordData::GetRecord(&g_Main.m_tblUnitFrame, frameCode);
  unsigned int consumDalant = 0;
  unsigned int newGauge = 0;
  const unsigned int taxRate = eGetTexRate(0) + 10000;
  [[maybe_unused]] const float tax = eGetTex(0) + 1.0f;
  int gaugeDiff = 0;

  if (!m_pUserDB)
  {
    return;
  }

  if (TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, m_id.wIndex) == 99)
  {
    resultCode = 34;
  }
  else if (bUseNPCLinkIntem || IsBeNearStore(this, 4))
  {
    _base_fld *checkRecord = CRecordData::GetRecord(&g_Main.m_tblUnitFrame, frameCode);
    if (checkRecord)
    {
      if (*reinterpret_cast<int *>(&checkRecord[1].m_strCode[60]))
      {
        if (CPlayerDB::GetRaceCode(&m_Param))
        {
          resultCode = 1;
        }
        else if (m_pUsingUnit)
        {
          resultCode = 2;
        }
        else if (frameRecord)
        {
          if (frameCode == 255)
          {
            resultCode = 5;
          }
          else
          {
            newGauge = *reinterpret_cast<unsigned int *>(frameRecord[1].m_strCode);
            gaugeDiff = static_cast<int>(newGauge) - static_cast<int>(unitData->dwGauge);
            if (gaugeDiff > 0)
            {
              if (*reinterpret_cast<int *>(&frameRecord[1].m_strCode[52]) > 0)
              {
                consumDalant = *reinterpret_cast<unsigned int *>(&frameRecord[1].m_strCode[52]);
              }
              int extraCost = 0;
              for (int j = 0; j < 6; ++j)
              {
                _base_fld *partRecord = CRecordData::GetRecord(&g_Main.m_tblUnitPart[j], unitData->byPart[j]);
                if (partRecord)
                {
                  if (*reinterpret_cast<int *>(&partRecord[5].m_strCode[48]) > 0)
                  {
                    consumDalant += *reinterpret_cast<unsigned int *>(&partRecord[5].m_strCode[48]);
                  }
                  if (!unitData->dwGauge)
                  {
                    extraCost += *reinterpret_cast<unsigned int *>(&partRecord[5].m_strCode[52]);
                  }
                }
              }
              const float repairRatio = static_cast<float>(gaugeDiff) * m_fUnitPv_RepPr;
              const float repairCost = static_cast<float>(static_cast<int>(consumDalant)) * repairRatio;
              const unsigned int repairCostInt = static_cast<unsigned int>(static_cast<int>(repairCost));
              const unsigned __int64 scaled = static_cast<unsigned __int64>(taxRate) * repairCostInt;
              consumDalant = extraCost + static_cast<unsigned int>(scaled / 0x2710);
              if (consumDalant > CPlayerDB::GetDalant(&m_Param))
              {
                resultCode = 7;
              }
            }
            else
            {
              resultCode = 14;
            }
          }
        }
        else
        {
          resultCode = 9;
        }
      }
      else
      {
        resultCode = 37;
      }
    }
    else
    {
      resultCode = 5;
    }
  }
  else
  {
    resultCode = 21;
  }

  if (!resultCode)
  {
    unitData->dwGauge = newGauge;
    SubDalant(consumDalant);
    CUserDB::Update_UnitData(m_pUserDB, bySlotIndex, unitData);
    if (consumDalant)
    {
      const unsigned int currentGold = CPlayerDB::GetGold(&m_Param);
      const unsigned int currentDalant = CPlayerDB::GetDalant(&m_Param);
      CMgrAvatorItemHistory::pay_money(
        &CPlayer::s_MgrItemHistory,
        m_ObjID.m_wIndex,
        "Unit_Repair",
        consumDalant,
        0,
        currentDalant,
        currentGold,
        m_szItemHistoryFileName);
    }
    if (!m_byUserDgr)
    {
      eAddDalant(0, consumDalant);
    }
    const int level = CPlayerDB::GetLevel(&m_Param);
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      const int lv = CPlayerDB::GetLevel(&m_Param);
      CMoneySupplyMgr::UpdateUnitRepairingChargesData(CMoneySupplyMgr::Instance(), lv, consumDalant);
    }
  }

  SendMsg_UnitFrameRepairResult(resultCode, bySlotIndex, newGauge, consumDalant);
}

void CPlayer::SendMsg_UnitFrameRepairResult(
  char byRetCode,
  char bySlotIndex,
  unsigned int dwNewGauge,
  unsigned int dwConsumDalant)
{
  _unit_frame_repair_result_zocl msg{};
  msg.byRetCode = static_cast<unsigned __int8>(byRetCode);
  msg.bySlotIndex = static_cast<unsigned __int8>(bySlotIndex);
  msg.dwNewGauge = dwNewGauge;
  msg.dwConsumDalant = dwConsumDalant;
  msg.dwLeftDalant = CPlayerDB::GetDalant(&m_Param);

  unsigned __int8 type[2]{23, 8};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    sizeof(msg));
}

void CPlayer::pc_UnitBulletFillRequest(
  unsigned __int8 bySlotIndex,
  unsigned __int16 *pwBulletIndex,
  int bUseNPCLinkIntem)
{
  unsigned __int8 resultCode = 0;
  _UNIT_DB_BASE::_LIST *unitData = &m_Param.m_UnitDB.m_List[bySlotIndex];
  const unsigned __int8 frameCode = unitData->byFrame;
  _base_fld *bulletRecords[2]{};
  bulletRecords[0] = CRecordData::GetRecord(&g_Main.m_tblUnitBullet, *pwBulletIndex);
  bulletRecords[1] = CRecordData::GetRecord(&g_Main.m_tblUnitBullet, pwBulletIndex[1]);
  std::array<unsigned int, 11> consumMoney{};
  const unsigned int taxRate = eGetTexRate(0) + 10000;
  [[maybe_unused]] const float tax = eGetTex(0) + 1.0f;

  if (!m_pUserDB)
  {
    return;
  }

  if (TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, m_id.wIndex) == 99)
  {
    resultCode = 34;
  }
  else if (bUseNPCLinkIntem || IsBeNearStore(this, 4))
  {
    if (CPlayerDB::GetRaceCode(&m_Param))
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
      for (int j = 0; j < 2; ++j)
      {
        if (bulletRecords[j])
        {
          const unsigned __int8 weaponPart = gbyWeaponPerBullet[j];
          _base_fld *partRecord = CRecordData::GetRecord(&g_Main.m_tblUnitPart[weaponPart], unitData->byPart[weaponPart]);
          if (!partRecord)
          {
            resultCode = 26;
            break;
          }
          if (*reinterpret_cast<unsigned int *>(&partRecord[3].m_strCode[60])
              != *reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(bulletRecords[j]) + 200))
          {
            resultCode = 26;
            break;
          }
        }
      }
      if (!resultCode)
      {
        for (int j = 0; j < 2; ++j)
        {
          if (bulletRecords[j])
          {
            const int moneyType = *reinterpret_cast<int *>(reinterpret_cast<char *>(bulletRecords[j]) + 212);
            consumMoney[moneyType] += *reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(bulletRecords[j]) + 216);
          }
        }
        for (int j = 0; j < 7; ++j)
        {
          unsigned __int64 value = consumMoney[j];
          value *= taxRate;
          consumMoney[j] = static_cast<unsigned int>(value / 0x2710);
          if (consumMoney[j] > GetMoney(j))
          {
            resultCode = 7;
            break;
          }
        }
      }
    }
  }
  else
  {
    resultCode = 21;
  }

  if (!resultCode)
  {
    for (int j = 0; j < 2; ++j)
    {
      if (bulletRecords[j])
      {
        unsigned int *bulletEntry = &unitData->dwBullet[j];
        *reinterpret_cast<unsigned __int16 *>(bulletEntry) = pwBulletIndex[j];
        reinterpret_cast<unsigned __int16 *>(bulletEntry)[1] =
          *reinterpret_cast<unsigned __int16 *>(reinterpret_cast<char *>(bulletRecords[j]) + 208);
      }
    }
    SubDalant(consumMoney[0]);
    SubGold(consumMoney[1]);
    CUserDB::Update_UnitData(m_pUserDB, bySlotIndex, unitData);
    if (consumMoney[0] || consumMoney[1])
    {
      const unsigned int currentGold = CPlayerDB::GetDalant(&m_Param);
      const unsigned int currentDalant = CPlayerDB::GetGold(&m_Param);
      CMgrAvatorItemHistory::pay_money(
        &CPlayer::s_MgrItemHistory,
        m_ObjID.m_wIndex,
        "Unit_Bullet_Charge",
        consumMoney[0],
        consumMoney[1],
        currentDalant,
        currentGold,
        m_szItemHistoryFileName);
    }
    if (!m_byUserDgr)
    {
      eAddGold(0, consumMoney[1]);
      eAddDalant(0, consumMoney[0]);
    }
    const int level = CPlayerDB::GetLevel(&m_Param);
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      if (consumMoney[1])
      {
        const unsigned int amount = 2000 * consumMoney[1];
        const int lv = CPlayerDB::GetLevel(&m_Param);
        CMoneySupplyMgr::UpdateBuyUnitData(CMoneySupplyMgr::Instance(), lv, amount);
      }
      if (consumMoney[0])
      {
        const int lv = CPlayerDB::GetLevel(&m_Param);
        CMoneySupplyMgr::UpdateBuyUnitData(CMoneySupplyMgr::Instance(), lv, consumMoney[0]);
      }
    }
  }

  SendMsg_UnitBulletFillResult(resultCode, bySlotIndex, pwBulletIndex, consumMoney.data());
}

void CPlayer::SendMsg_UnitBulletFillResult(
  char byRetCode,
  char bySlotIndex,
  unsigned __int16 *pwBulletIndex,
  unsigned int *pdwConsumMoney)
{
  _unit_bullet_fill_result_zocl msg{};
  msg.byRetCode = static_cast<unsigned __int8>(byRetCode);
  msg.bySlotIndex = static_cast<unsigned __int8>(bySlotIndex);
  msg.wBulletIndex[0] = pwBulletIndex[0];
  msg.dwComsumMoney[0] = pdwConsumMoney[0];
  msg.dwComsumMoney[1] = pdwConsumMoney[1];
  msg.dwLeftMoney[0] = CPlayerDB::GetDalant(&m_Param);
  msg.dwLeftMoney[1] = CPlayerDB::GetGold(&m_Param);

  unsigned __int8 type[2]{23, 10};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    sizeof(msg));
}

void CPlayer::pc_UnitPackFillRequest(
  unsigned __int8 bySlotIndex,
  unsigned __int8 byFillNum,
  _unit_pack_fill_request_clzo::__list *pList,
  int bUseNPCLinkIntem)
{
  unsigned __int8 resultCode = 0;
  _UNIT_DB_BASE::_LIST *unitData = &m_Param.m_UnitDB.m_List[bySlotIndex];
  const unsigned __int8 frameCode = unitData->byFrame;
  _base_fld *spareRecord = CRecordData::GetRecord(&g_Main.m_tblUnitPart[5], unitData->byPart[5]);
  _base_fld *spareBullet[8]{};
  std::array<unsigned int, 11> consumMoney{};
  const unsigned int taxRate = eGetTexRate(0) + 10000;
  [[maybe_unused]] const float tax = eGetTex(0) + 1.0f;

  if (!m_pUserDB)
  {
    return;
  }

  if (TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, m_id.wIndex) == 99)
  {
    resultCode = 34;
  }
  else if (bUseNPCLinkIntem || IsBeNearStore(this, 4))
  {
    if (CPlayerDB::GetRaceCode(&m_Param))
    {
      resultCode = 1;
    }
    else if (frameCode == 255)
    {
      resultCode = 5;
    }
    else if (spareRecord)
    {
      for (int j = 0; j < byFillNum; ++j)
      {
        if (pList[j].bySpareIndex >= static_cast<int>(spareRecord[6].m_dwIndex))
        {
          resultCode = 18;
          break;
        }
        _base_fld *bulletRecord = CRecordData::GetRecord(&g_Main.m_tblUnitBullet, pList[j].wBulletIndex);
        spareBullet[pList[j].bySpareIndex] = bulletRecord;
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
          for (int j = 0; j < 8; ++j)
          {
            if (spareBullet[j])
            {
              const int moneyType = *reinterpret_cast<int *>(reinterpret_cast<char *>(spareBullet[j]) + 212);
              consumMoney[moneyType] += *reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(spareBullet[j]) + 216);
            }
          }
          for (int j = 0; j < 7; ++j)
          {
            unsigned __int64 value = consumMoney[j];
            value *= taxRate;
            consumMoney[j] = static_cast<unsigned int>(value / 0x2710);
            if (consumMoney[j] > GetMoney(j))
            {
              resultCode = 7;
              break;
            }
          }
        }
      }
    }
    else
    {
      resultCode = 17;
    }
  }
  else
  {
    resultCode = 21;
  }

  if (!resultCode)
  {
    for (int j = 0; j < 8; ++j)
    {
      if (spareBullet[j])
      {
        unsigned int *spareEntry = &unitData->dwSpare[j];
        *reinterpret_cast<unsigned __int16 *>(spareEntry) = *reinterpret_cast<unsigned __int16 *>(spareBullet[j]);
        reinterpret_cast<unsigned __int16 *>(spareEntry)[1] =
          *reinterpret_cast<unsigned __int16 *>(reinterpret_cast<char *>(spareBullet[j]) + 208);
      }
    }
    SubDalant(consumMoney[0]);
    SubGold(consumMoney[1]);
    CUserDB::Update_UnitData(m_pUserDB, bySlotIndex, unitData);
    if (consumMoney[1] || consumMoney[0])
    {
      const unsigned int currentGold = CPlayerDB::GetDalant(&m_Param);
      const unsigned int currentDalant = CPlayerDB::GetGold(&m_Param);
      CMgrAvatorItemHistory::pay_money(
        &CPlayer::s_MgrItemHistory,
        m_ObjID.m_wIndex,
        "Unit_Spare_Charge",
        consumMoney[0],
        consumMoney[1],
        currentDalant,
        currentGold,
        m_szItemHistoryFileName);
    }
    if (!m_byUserDgr)
    {
      eAddGold(0, consumMoney[1]);
      eAddDalant(0, consumMoney[0]);
    }
    const int level = CPlayerDB::GetLevel(&m_Param);
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      if (consumMoney[1])
      {
        const unsigned int amount = 2000 * consumMoney[1];
        const int lv = CPlayerDB::GetLevel(&m_Param);
        CMoneySupplyMgr::UpdateBuyUnitData(CMoneySupplyMgr::Instance(), lv, amount);
      }
      if (consumMoney[0])
      {
        const int lv = CPlayerDB::GetLevel(&m_Param);
        CMoneySupplyMgr::UpdateBuyUnitData(CMoneySupplyMgr::Instance(), lv, consumMoney[0]);
      }
    }
  }

  SendMsg_UnitPackFillResult(resultCode, bySlotIndex, byFillNum, pList, consumMoney.data());
}

void CPlayer::SendMsg_UnitPackFillResult(
  unsigned __int8 byRetCode,
  unsigned __int8 bySlotIndex,
  unsigned __int8 byFillNum,
  _unit_pack_fill_request_clzo::__list *pList,
  unsigned int *pdwConsumMoney)
{
  _unit_pack_fill_result_zocl msg{};
  msg.byRetCode = byRetCode;
  msg.bySlotIndex = bySlotIndex;
  msg.byFillNum = byFillNum;
  memcpy_0(msg.List, pList, sizeof(_unit_pack_fill_request_clzo::__list) * byFillNum);
  msg.dwComsumMoney[0] = pdwConsumMoney[0];
  msg.dwComsumMoney[1] = pdwConsumMoney[1];
  msg.dwLeftMoney[0] = CPlayerDB::GetDalant(&m_Param);
  msg.dwLeftMoney[1] = CPlayerDB::GetGold(&m_Param);

  unsigned __int8 type[2]{23, 12};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    sizeof(msg));
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
  unsigned int consumDalant = 0;
  CParkingUnit *parkingUnit = nullptr;
  unsigned __int8 transDistCode = 0;

  if (!unitData->dwGauge)
  {
    resultCode = 35;
    goto RESULT;
  }

  const unsigned int taxRate = eGetTexRate(CPlayerDB::GetRaceCode(&m_Param)) + 10000;
  [[maybe_unused]] const float tax = eGetTex(CPlayerDB::GetRaceCode(&m_Param)) + 1.0f;

  if (m_bObserver)
  {
    resultCode = 27;
    goto RESULT;
  }
  if (CPlayerDB::GetRaceCode(&m_Param))
  {
    resultCode = 1;
    goto RESULT;
  }
  if (unitData->byFrame == 255)
  {
    resultCode = 5;
    goto RESULT;
  }
  if (m_pCurMap->m_pMapSet->m_nMapType >= 2u)
  {
    resultCode = 27;
    goto RESULT;
  }
  if (m_pUsingUnit)
  {
    resultCode = 2;
    goto RESULT;
  }
  if (pStore)
  {
    if (pStore->m_pRec->m_nStore_trade != 4)
    {
      resultCode = 23;
      goto RESULT;
    }
    if (!bUseNPCLinkIntem)
    {
      float *storePos = pStore->GetStorePos();
      if (GetSqrt(m_fCurPos, storePos) > 540.0f)
      {
        resultCode = 21;
        goto RESULT;
      }
    }
  }
  else
  {
    transDistCode = 1;
  }

  if (bPayFee)
  {
    consumDalant = unitData->nPullingFee + 10000;
    if (transDistCode == 1)
    {
      consumDalant += 1000;
    }
    consumDalant = static_cast<unsigned int>((static_cast<unsigned __int64>(taxRate) * consumDalant) / 0x2710);
    if (consumDalant > CPlayerDB::GetDalant(&m_Param))
    {
      resultCode = 11;
    }
  }

  if (!resultCode)
  {
    if (GetSqrt(m_fCurPos, pfNewPos) <= 40.0f)
    {
      parkingUnit = FindEmptyParkingUnit(g_ParkingUnit, MAX_PARKING_UNIT);
      if (!parkingUnit)
      {
        resultCode = 19;
      }
    }
    else
    {
      resultCode = 24;
    }
  }

RESULT:
  unsigned int parkingSerial = static_cast<unsigned int>(-1);
  if (!resultCode)
  {
    _parkingunit_create_setdata param;
    param.byFrame = unitData->byFrame;
    memcpy_0(param.byPartCode, unitData->byPart, sizeof(param.byPartCode));
    param.m_pRecordSet = CRecordData::GetRecord(&g_Main.m_tblUnitFrame, unitData->byFrame);
    param.pOwner = this;
    param.byCreateType = 0;
    param.m_pMap = m_pCurMap;
    param.m_nLayerIndex = m_wMapLayerIndex;
    memcpy_0(param.m_fStartPos, pfNewPos, sizeof(param.m_fStartPos));
    param.byTransDistCode = transDistCode;
    unsigned int baseGauge = 10000;
    _base_fld *frameRecord = CRecordData::GetRecord(&g_Main.m_tblUnitFrame, unitData->byFrame);
    if (*reinterpret_cast<int *>(frameRecord[1].m_strCode) > 0)
    {
      baseGauge = *reinterpret_cast<unsigned int *>(frameRecord[1].m_strCode);
    }
    param.wHPRate = static_cast<unsigned __int16>((10000 * unitData->dwGauge) / baseGauge);
    parkingUnit->Create(&param);
    parkingSerial = parkingUnit->m_dwObjSerial;
    _LockUnitKey(bySlotIndex, true);
    _base_fld *sparePart = CRecordData::GetRecord(&g_Main.m_tblUnitPart[5], unitData->byPart[5]);
    if (sparePart)
    {
      unitData->wBooster = *reinterpret_cast<unsigned __int16 *>(&sparePart[5].m_strCode[56]);
    }
    m_pUsingUnit = unitData;
    m_pParkingUnit = parkingUnit;
    m_dwUnitViewOverTime = static_cast<unsigned int>(-1);
    SubDalant(consumDalant);
    if (!m_byUserDgr)
    {
      eAddDalant(CPlayerDB::GetRaceCode(&m_Param), consumDalant);
    }
    const int level = CPlayerDB::GetLevel(&m_Param);
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      const int lv = CPlayerDB::GetLevel(&m_Param);
      CMoneySupplyMgr::UpdateBuyUnitData(CMoneySupplyMgr::Instance(), lv, consumDalant);
    }
    if (consumDalant)
    {
      const unsigned int currentGold = CPlayerDB::GetGold(&m_Param);
      const unsigned int currentDalant = CPlayerDB::GetDalant(&m_Param);
      CMgrAvatorItemHistory::pay_money(
        &CPlayer::s_MgrItemHistory,
        m_ObjID.m_wIndex,
        "Unit_Delivery",
        consumDalant,
        0,
        currentDalant,
        currentGold,
        m_szItemHistoryFileName);
    }
  }

  SendMsg_UnitDeliveryResult(resultCode, bySlotIndex, parkingSerial, consumDalant);
}

void CPlayer::SendMsg_UnitDeliveryResult(
  char byRetCode,
  char bySlotIndex,
  unsigned int dwParkingUnitSerial,
  unsigned int dwPayDalant)
{
  _unit_delivery_result_zocl msg{};
  msg.byRetCode = static_cast<unsigned __int8>(byRetCode);
  msg.bySlotIndex = static_cast<unsigned __int8>(bySlotIndex);
  msg.dwParkingUnitSerial = dwParkingUnitSerial;
  msg.dwPayDalant = dwPayDalant;
  msg.dwLeftDalant = CPlayerDB::GetDalant(&m_Param);

  unsigned __int8 type[2]{23, 14};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    sizeof(msg));
}

void CPlayer::pc_UnitReturnRequest()
{
  unsigned __int8 resultCode = 0;
  char payFee = 1;

  if (CPlayerDB::GetRaceCode(&m_Param))
  {
    resultCode = 1;
  }
  else if (m_pParkingUnit)
  {
    if (m_pParkingUnit->m_dwOwnerSerial == m_dwObjSerial)
    {
      if (GetSqrt(m_fCurPos, m_pParkingUnit->m_fCurPos) <= 540.0f)
      {
        if (IsBeNearStore(this, 4))
        {
          payFee = 0;
        }
      }
      else
      {
        resultCode = 20;
      }
    }
    else
    {
      resultCode = 2;
    }
  }
  else
  {
    resultCode = 2;
  }

  unsigned int payDalant = 0;
  if (!resultCode)
  {
    m_pParkingUnit->Destroy(0);
    if (payFee == 1)
    {
      payDalant = 1000;
    }
    _UpdateUnitDebt(m_pUsingUnit->bySlotIndex, payDalant);
    _LockUnitKey(m_pUsingUnit->bySlotIndex, false);
    m_pUsingUnit = nullptr;
    m_pParkingUnit = nullptr;
  }

  SendMsg_UnitReturnResult(resultCode, payDalant);
}

void CPlayer::SendMsg_UnitReturnResult(char byRetCode, unsigned int dwPayDalant)
{
  _unit_return_result_zocl msg{};
  msg.byRetCode = static_cast<unsigned __int8>(byRetCode);
  msg.dwPayDalant = dwPayDalant;
  msg.dwNewDalant = CPlayerDB::GetDalant(&m_Param);

  unsigned __int8 type[2]{23, 16};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    sizeof(msg));
}

void CPlayer::pc_UnitTakeRequest()
{
  unsigned __int8 resultCode = 0;
  BreakCloakBooster();

  if (m_bInGuildBattle && m_bTakeGravityStone)
  {
    resultCode = 31;
  }
  else
  {
    if (m_bFreeSFByClass)
    {
      goto CHECK_TAKE;
    }
    bool hasHistoryEffect = false;
    for (int j = 0; j < 4; ++j)
    {
      if (!m_Param.m_ppHistoryEffect[j])
      {
        break;
      }
      if (*reinterpret_cast<int *>(reinterpret_cast<char *>(m_Param.m_ppHistoryEffect[j]) + 1436))
      {
        hasHistoryEffect = true;
        break;
      }
    }
    if (hasHistoryEffect)
    {
CHECK_TAKE:
      if (m_pSiegeItem)
      {
        resultCode = 28;
      }
      else if (m_pParkingUnit && m_pUsingUnit)
      {
        if (m_pParkingUnit->m_dwOwnerSerial == m_dwObjSerial)
        {
          if (GetSqrt(m_fCurPos, m_pParkingUnit->m_fCurPos) <= 50.0f)
          {
            if (m_bAfterEffect)
            {
              resultCode = 32;
            }
            else if (m_pParkingUnit->GetHP() <= 0)
            {
              resultCode = 35;
            }
          }
          else
          {
            resultCode = 20;
          }
        }
        else
        {
          resultCode = 2;
        }
      }
      else
      {
        resultCode = 2;
      }
    }
    else
    {
      resultCode = 28;
    }
  }

  if (!resultCode)
  {
    float newPos[3];
    newPos[0] = m_pParkingUnit->m_fCurPos[0];
    newPos[1] = m_pParkingUnit->m_fCurPos[1];
    newPos[2] = m_pParkingUnit->m_fCurPos[2];
    CMapData::GetRandPosInRange(m_pCurMap, m_pParkingUnit->m_fCurPos, 10, newPos);
    memcpy_0(m_fOldPos, m_fCurPos, sizeof(m_fOldPos));
    memcpy_0(m_fCurPos, newPos, sizeof(m_fCurPos));
    Emb_RidindUnit(true, nullptr);
    SendMsg_AlterUnitHPInform(m_pUsingUnit->bySlotIndex, m_pUsingUnit->dwGauge);
  }

  SendMsg_UnitTakeResult(resultCode);
}

void CPlayer::SendMsg_UnitTakeResult(char byRetCode)
{
  _unit_take_result_zocl msg{};
  msg.byRetCode = static_cast<unsigned __int8>(byRetCode);

  unsigned __int8 type[2]{23, 18};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    sizeof(msg));
}

void CPlayer::pc_UnitLeaveRequest(float *pfNewPos)
{
  unsigned __int8 resultCode = 0;
  CParkingUnit *createUnit = nullptr;

  if (CPlayerDB::GetRaceCode(&m_Param))
  {
    resultCode = 1;
  }
  else if (m_pUsingUnit && !m_pParkingUnit)
  {
    if (GetSqrt(m_fCurPos, pfNewPos) <= 40.0f)
    {
      createUnit = FindEmptyParkingUnit(g_ParkingUnit, MAX_PARKING_UNIT);
      if (!createUnit)
      {
        resultCode = 19;
      }
    }
    else
    {
      resultCode = 24;
    }
  }
  else
  {
    resultCode = 22;
  }

  if (!resultCode)
  {
    _parkingunit_create_setdata param;
    param.byFrame = m_pUsingUnit->byFrame;
    memcpy_0(param.byPartCode, m_pUsingUnit->byPart, sizeof(param.byPartCode));
    param.m_pRecordSet = CRecordData::GetRecord(&g_Main.m_tblUnitFrame, m_pUsingUnit->byFrame);
    param.pOwner = this;
    param.byCreateType = 1;
    param.m_pMap = m_pCurMap;
    param.m_nLayerIndex = m_wMapLayerIndex;
    memcpy_0(param.m_fStartPos, m_fCurPos, sizeof(param.m_fStartPos));
    unsigned int baseGauge = 10000;
    _base_fld *frameRecord = CRecordData::GetRecord(&g_Main.m_tblUnitFrame, m_pUsingUnit->byFrame);
    if (*reinterpret_cast<int *>(frameRecord[1].m_strCode) > 0)
    {
      baseGauge = *reinterpret_cast<unsigned int *>(frameRecord[1].m_strCode);
    }
    param.wHPRate = static_cast<unsigned __int16>((10000 * m_pUsingUnit->dwGauge) / baseGauge);
    createUnit->Create(&param);
    Emb_RidindUnit(false, createUnit);
  }

  SendMsg_UnitLeaveResult(resultCode);
}

void CPlayer::SendMsg_UnitLeaveResult(char byRetCode)
{
  _unit_leave_result_zocl msg{};
  msg.byRetCode = static_cast<unsigned __int8>(byRetCode);

  unsigned __int8 type[2]{23, 20};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    sizeof(msg));
}

void CPlayer::pc_UnitBulletReplaceRequest(unsigned __int8 bySlotIndex, unsigned __int8 byPackIndex, unsigned __int8 byBulletPart)
{
  unsigned __int8 resultCode = 0;
  _UNIT_DB_BASE::_LIST *unitData = &m_Param.m_UnitDB.m_List[bySlotIndex];
  void *bulletEntry = &unitData->dwBullet[byBulletPart];
  _unit_bullet_param *spareEntry = reinterpret_cast<_unit_bullet_param *>(&unitData->dwSpare[byPackIndex]);

  if (!m_pUserDB)
  {
    return;
  }

  if (unitData->byFrame == 255)
  {
    resultCode = 5;
  }
  else if (!spareEntry->IsFill())
  {
    resultCode = 25;
  }

  if (!resultCode)
  {
    memcpy_0(bulletEntry, spareEntry, 4uLL);
    *reinterpret_cast<unsigned int *>(spareEntry) = 0xFFFFFFFFu;
    CUserDB::Update_UnitData(m_pUserDB, bySlotIndex, unitData);
  }

  SendMsg_UnitBulletReplaceResult(resultCode);
}

void CPlayer::SendMsg_UnitBulletReplaceResult(char byRetCode)
{
  _unit_bullet_replace_result_zocl msg{};
  msg.byRetCode = static_cast<unsigned __int8>(byRetCode);

  unsigned __int8 type[2]{23, 24};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    sizeof(msg));
}

void CPlayer::pc_SelectQuestAfterHappenEvent(unsigned __int8 bySelectIndex)
{
  void *lastEvent = CQuestMgr::GetLastHappenEvent(&m_QuestMgr);
  if (!lastEvent)
  {
    return;
  }

  _happen_event_cont happenEvent;
  happenEvent.init();
  memcpy_0(&happenEvent, lastEvent, sizeof(happenEvent));
  if (!Emb_StartQuest(bySelectIndex, &happenEvent))
  {
    SendMsg_InsertQuestFailure(happenEvent.m_QtHpType, happenEvent.m_nIndexInType, happenEvent.m_nRaceCode);
  }
}

void CPlayer::pc_SelectQuestReward(
  unsigned __int8 byQuestDBSlot,
  unsigned __int8 bySelectItemSlotIndex,
  unsigned __int8 bySelectLinkQuestIndex)
{
  _QUEST_DB_BASE::_LIST *questData = &m_Param.m_QuestDB.m_List[byQuestDBSlot];
  if (questData->byQuestType == 255)
  {
    return;
  }

  for (int j = 0; j < 3; ++j)
  {
    if (questData->wNum[j] != 0xFFFF)
    {
      return;
    }
  }

  _base_fld *record = CRecordData::GetRecord(CQuestMgr::s_tblQuest, questData->wIndex);
  if (bySelectItemSlotIndex == 255)
  {
    if (*reinterpret_cast<int *>(&record[13].m_strCode[60]))
    {
      return;
    }
  }
  else if (!*reinterpret_cast<int *>(&record[13].m_strCode[60]))
  {
    return;
  }

  if (bySelectItemSlotIndex == 255
      || (strncmp(reinterpret_cast<char *>(&record[14]) + 72LL * bySelectItemSlotIndex, "-1", 2uLL)))
  {
    if (bySelectLinkQuestIndex == 255
        || strncmp(&record[22].m_strCode[64LL * bySelectItemSlotIndex + 44], "-1", 2uLL))
    {
      Emb_CompleteQuest(byQuestDBSlot, bySelectItemSlotIndex, bySelectLinkQuestIndex);
    }
  }
}

void CPlayer::pc_BriefPass(unsigned __int8 byQuestSlotIndex)
{
  _QUEST_DB_BASE::_LIST *questData = &m_Param.m_QuestDB.m_List[byQuestSlotIndex];
  if (questData->byQuestType == 255)
  {
    return;
  }

  _base_fld *record = CRecordData::GetRecord(CQuestMgr::s_tblQuest, questData->wIndex);
  if (record)
  {
    Emb_CheckActForQuest(15, record->m_strCode, 1u, false);
  }
}

void CPlayer::pc_RequestDialogWithNPC(CItemStore *pStore)
{
  float *storePos = pStore->GetStorePos();
  if (GetSqrt(m_fCurPos, storePos) > 60.0f)
  {
    return;
  }

  const int npcRace = pStore->m_byNpcRaceCode;
  const int playerRace = CPlayerDB::GetRaceCode(&m_Param);
  if (npcRace != playerRace && pStore->m_byNpcRaceCode != 255)
  {
    return;
  }

  char *npcCode = pStore->GetNpcCode();
  if (!npcCode)
  {
    return;
  }

  Emb_CheckActForQuest(14, npcCode, 1u, false);
  Emb_CheckActForQuest(1, npcCode, 1u, false);
}

void CPlayer::pc_RequestWatchingWithNPC(CItemStore *pStore)
{
  float *storePos = pStore->GetStorePos();
  if (GetSqrt(m_fCurPos, storePos) > 60.0f)
  {
    return;
  }

  const int npcRace = pStore->m_byNpcRaceCode;
  const int playerRace = CPlayerDB::GetRaceCode(&m_Param);
  if (npcRace != playerRace && pStore->m_byNpcRaceCode != 255)
  {
    return;
  }

  char *npcCode = pStore->GetNpcCode();
  if (npcCode)
  {
    Emb_CheckActForQuest(17, npcCode, 1u, false);
  }
}

bool CPlayer::Emb_CreateNPCQuest(char *pszEventCode, unsigned int dwNPCQuestIndex)
{
  unsigned int happenIndex = 0;
  char found = 0;
  for (int j = 0; j < 30; ++j)
  {
    if (m_NPCQuestIndexTempData.IndexData[j].dwQuestIndex == dwNPCQuestIndex)
    {
      happenIndex = m_NPCQuestIndexTempData.IndexData[j].dwQuestHappenIndex;
      found = 1;
      break;
    }
  }
  if (!found)
  {
    return false;
  }

  const unsigned __int8 race = CPlayerDB::GetRaceCode(&m_Param);
  void *startEvent = CQuestMgr::CheckNPCQuestStartable(&m_QuestMgr, pszEventCode, race, dwNPCQuestIndex, happenIndex);
  if (!startEvent)
  {
    return false;
  }

  char historySkip = 0;
  _base_fld *record = CRecordData::GetRecord(CQuestMgr::s_tblQuest, dwNPCQuestIndex);
  if (m_pUserDB->m_AvatorData.dbQuest.dwListCnt)
  {
    for (unsigned int k = 0; k < m_pUserDB->m_AvatorData.dbQuest.dwListCnt; ++k)
    {
      if (!strcmp_0(m_pUserDB->m_AvatorData.dbQuest.m_StartHistory[k].szQuestCode, record->m_strCode))
      {
        if (*reinterpret_cast<int *>(&record[1].m_strCode[4]))
        {
          break;
        }
        historySkip = 1;
        return false;
      }
    }
  }

  _happen_event_cont happenEvent;
  happenEvent.init();
  memcpy_0(&happenEvent, startEvent, sizeof(happenEvent));
  if (Emb_StartQuest(0xFFu, &happenEvent))
  {
    if (historySkip)
    {
      return true;
    }
    if (*reinterpret_cast<int *>(&record[1].m_strCode[4]))
    {
      return true;
    }
    _QUEST_DB_BASE::_START_NPC_QUEST_HISTORY historyData;
    strcpy_0(historyData.szQuestCode, record->m_strCode);
    historyData.byLevel = CPlayerDB::GetLevel(&m_Param);
    __int64 now = 0;
    time_0(&now);
    historyData.nEndTime =
      static_cast<unsigned int>(static_cast<int>(static_cast<double>(static_cast<int>(now))
                                                 + *reinterpret_cast<double *>(&record[1].m_strCode[8])));
    return CUserDB::Update_StartNPCQuestHistory(
      m_pUserDB,
      m_pUserDB->m_AvatorData.dbQuest.dwListCnt,
      &historyData);
  }

  for (int m = 0; m < 3; ++m)
  {
    if (!m_QuestMgr.m_pTempHappenEvent[m].isset())
    {
      memcpy_0(&m_QuestMgr.m_pTempHappenEvent[m], &happenEvent, sizeof(m_QuestMgr.m_pTempHappenEvent[m]));
      return false;
    }
  }
  return false;
}

void CPlayer::SendMsg_ResultNpcQuest(char bSucc)
{
  unsigned __int8 type[2]{24, 18};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&bSucc),
    sizeof(bSucc));
}

void CPlayer::pc_RequestQuestFromNPC(CItemStore *pStore, unsigned int dwNPCQuestIndex)
{
  float *storePos = pStore->GetStorePos();
  if (GetSqrt(m_fCurPos, storePos) > 80.0f)
  {
    return;
  }

  const int npcRace = pStore->m_byNpcRaceCode;
  const int playerRace = CPlayerDB::GetRaceCode(&m_Param);
  if (npcRace != playerRace && pStore->m_byNpcRaceCode != 255)
  {
    return;
  }

  char *eventCode = pStore->GetNpcCode();
  if (!eventCode)
  {
    return;
  }

  const bool success = Emb_CreateNPCQuest(eventCode, dwNPCQuestIndex);
  SendMsg_ResultNpcQuest(success);
}

void CPlayer::pc_RequestQuestListFromNPC(CItemStore *pStore)
{
  float *storePos = pStore->GetStorePos();
  if (GetSqrt(m_fCurPos, storePos) > 80.0f)
  {
    return;
  }

  const int npcRace = pStore->m_byNpcRaceCode;
  const int playerRace = CPlayerDB::GetRaceCode(&m_Param);
  if (npcRace != playerRace && pStore->m_byNpcRaceCode != 255)
  {
    return;
  }

  char *eventCode = pStore->GetNpcCode();
  if (!eventCode)
  {
    return;
  }

  m_NPCQuestIndexTempData.Init();
  const unsigned __int8 race = CPlayerDB::GetRaceCode(&m_Param);
  CQuestMgr::CheckNPCQuestList(&m_QuestMgr, eventCode, race, &m_NPCQuestIndexTempData);
  SendMsg_NpcQuestListResult(&m_NPCQuestIndexTempData);
}

void CPlayer::SendMsg_NpcQuestListResult(_NPCQuestIndexTempData *pQuestIndexData)
{
  _npc_quest_list_result_zocl msg{};
  msg.byQuestNum = static_cast<unsigned __int8>(pQuestIndexData->nQuestNum);
  for (int j = 0; j < pQuestIndexData->nQuestNum; ++j)
  {
    msg.QuestIndexList[j] = pQuestIndexData->IndexData[j].dwQuestIndex;
  }

  unsigned __int8 type[2]{24, 21};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    sizeof(msg));
}

void CPlayer::pc_QuestGiveupRequest(unsigned __int8 byQuestDBSlot)
{
  unsigned __int8 returnSlot = byQuestDBSlot;
  if (CQuestMgr::CanGiveupQuest(&m_QuestMgr, byQuestDBSlot))
  {
    CQuestMgr::DeleteQuestData(&m_QuestMgr, byQuestDBSlot);
    CUserDB::Update_QuestDelete(m_pUserDB, byQuestDBSlot);
  }
  else
  {
    returnSlot = static_cast<unsigned __int8>(-1);
  }
  SendMsg_QuestGiveUpResult(returnSlot);
}

void CPlayer::SendMsg_QuestGiveUpResult(char byReturnSlot)
{
  unsigned __int8 type[2]{24, 50};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&byReturnSlot),
    sizeof(byReturnSlot));
}
