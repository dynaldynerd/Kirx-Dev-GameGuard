#include "pch.h"

#include "CPlayer.h"
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
#include "combine_ex_item_accept_result_zocl.h"
#include "combine_ex_item_accept_request_clzo.h"
#include "combine_ex_item_request_clzo.h"
#include "combine_ex_item_result_zocl.h"
#include "exchange_lend_item_result_zocl.h"
#include "guild_honor_set_request_clzo.h"
#include "guildroom_out_result_zocl.h"
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
#include "quest_check_result.h"
#include "insert_new_quest_inform_zocl.h"
#include "insert_next_quest_inform_zocl.h"
#include "npclink_check_item_result_zocl.h"
#include "radar_char_list_result_zocl.h"
#include "Quest_fld.h"
#include "CRecordData.h"
#include "CAnimus.h"
#include "CMonster.h"
#include "CMapData.h"
#include "CObjectList.h"
#include "pnt_rect.h"
#include "CLevel.h"
#include "CTransportShip.h"
#include "CUserDB.h"
#include "CItemStore.h"
#include "CItemStoreManager.h"
#include "CMapItemStoreList.h"
#include "CItemBox.h"
#include "CMoneySupplyMgr.h"
#include "COreAmountMgr.h"
#include "CNuclearBombMgr.h"
#include "CNationSettingManager.h"
#include "CParkingUnit.h"
#include "CHolyStoneSystem.h"
#include "CHolyStone.h"
#include "CPvpCashMng.h"
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
#include "CReturnGateController.h"
#include "CashItemRemoteStore.h"
#include "CLogFile.h"
#include "CTSingleton.h"
#include "LendItemMng.h"
#include "ITEM_EFFECT.h"
#include "ItemUpgrade_fld.h"
#include "TimeLimitJadeMng.h"
#include "TimeItem.h"
#include "ResourceItem_fld.h"
#include "GuardTowerItem_fld.h"
#include "BulletItem_fld.h"
#include "DfnEquipItem_fld.h"
#include "ForceItem_fld.h"
#include "TOWNItem_fld.h"
#include "TrapItem_fld.h"
#include "BoxItem_fld.h"
#include "OreItem_fld.h"
#include "RecoveryItem_fld.h"
#include "NPCLink_fld.h"
#include "force_fld.h"
#include "skill_fld.h"
#include "WeaponItem_fld.h"
#include "CloakItem_fld.h"
#include "RingItem_fld.h"
#include "AmuletItem_fld.h"
#include "SiegeKitItem_fld.h"
#include "ItemMakeData_fld.h"
#include "ItemCombineData_fld.h"
#include "ItemExchangeData_fld.h"
#include "UnitFrame_fld.h"
#include "UnitPart_fld.h"
#include "UnitBullet_fld.h"
#include "PotionItem_fld.h"
#include "CouponItem_fld.h"
#include "StoreList_fld.h"
#include "MasteryLimit_fld.h"
#include "portal_fld.h"
#include "UIDGenerator.h"
#include "ItemDataLoader.h"
#include "WorldServerUtil.h"
#include "CExchangeEvent.h"
#include "CPcBangFavor.h"
#include "PCBANG_PRIMIUM_FAVOR.h"
#include "CPotionMgr.h"
#include "CActionPointSystemMgr.h"
#include "CSUItemSystem.h"
#include "CSetItemType.h"
#include "CTalkCrystalCombineManager.h"
#include "CUnmannedTraderController.h"
#include "CUnmannedTraderRegistItemInfo.h"
#include "CMoveMapLimitManager.h"
#include "CPostSystemManager.h"
#include "CBillingManager.h"
#include "CGuildRoomSystem.h"
#include "GuildBattle.h"
#include "CCouponMgr.h"
#include "EQUIP_MASTERY_LIM.h"
#include "CDarkHoleChannel.h"
#include "CDarkHoleDungeonQuest.h"
#include "CGuardTower.h"
#include "CTrap.h"
#include "CSetItemEffect.h"
#include "DqsDbStructs.h"
#include "exit_alter_param.h"
#include "alter_action_point_zocl.h"
#include "alter_cont_effect_time_zocl.h"
#include "notify_not_use_premium_cashitem_zocl.h"
#include "post_content_result_zocl.h"
#include "post_recv_delivery_zocl.h"
#include "post_result_zocl.h"
#include "post_return_zocl.h"
#include "pt_inform_commission_income_zocl.h"
#include "pt_inform_punishment_zocl.h"
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
#include "RadarItem_fld.h"
#include "player_fld.h"
#include "w_name.h"
#include "CChatStealSystem.h"
#include "CObjectList.h"
#include "pnt_rect.h"
#include "target_monster_aggro_inform_zocl.h"
#include "CNetworkEX.h"
#include "GlobalObjects.h"
#include "cStaticMember_Player.h"

#include <cstdio>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <mmsystem.h>

_skill_fld *CPlayer::ms_pXmas_Snow_Effect;
_skill_fld *CPlayer::ms_pXmas_Snow_Bullet_Effect;
CMgrAvatorItemHistory CPlayer::s_MgrItemHistory;
CMgrAvatorLvHistory CPlayer::s_MgrLvHistory;
int CPlayer::s_nLiveNum;
_ANIMUS_RETURN_DELAY CPlayer::s_AnimusReturnDelay;
int *CPlayer::s_pnLinkForceItemToEffect;
_SKILL_IDX_PER_MASTERY CPlayer::s_SkillIndexPerMastery[8];
int CPlayer::s_nAddMstFc[100];
float CPlayer::s_fPartGravity[5] = {0.2f, 0.23f, 0.22f, 0.18f, 0.17f};
int CPlayer::s_nStdDefPoint = 6;
int CPlayer::s_nRevDefPoint = 10;
int CPlayer::s_nMonDefPoint = 12;
CRecordData CPlayer::s_tblLimMastery[3][4];
CRecordData CPlayer::s_tblLimMasteryContinue[3][4];

namespace
{
int GetSkillAttackTypeForLevel(const _skill_fld *skillField, int skillLevel)
{
  const int *attackTypeByLevel = &skillField->m_nAttackable;
  return attackTypeByLevel[skillLevel];
}

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

bool CheckSameItemFromString_CodeIndex(char *psItemCode, unsigned __int8 byTableCode, unsigned __int16 wIndex)
{
  if (!psItemCode)
  {
    return false;
  }
  if (byTableCode >= 37)
  {
    return false;
  }

  _base_fld *record = g_Main.m_tblItemData[byTableCode].GetRecord(wIndex);
  return record && std::strcmp(record->m_strCode, psItemCode) == 0;
}

bool IsTalikAboutTol(int nTalikIndex)
{
  return nTalikIndex == 7 || nTalikIndex == 8 || nTalikIndex == 9 || nTalikIndex == 10;
}

char IsAddAbleTalikToItem(
  unsigned __int8 byItemTableCode,
  unsigned __int16 wItemIndex,
  unsigned int dwItemCurLv,
  unsigned __int16 wTalikIndex,
  int *pnTalikLim)
{
  CRecordData *itemTable = &g_Main.m_tblItemData[byItemTableCode];
  if (wTalikIndex >= 13)
  {
    return 0;
  }

  if (byItemTableCode == 6 && IsTalikAboutTol(wTalikIndex))
  {
    const unsigned __int8 itemUpgedLv = GetItemUpgedLv(dwItemCurLv);
    for (int j = 0; j < itemUpgedLv; ++j)
    {
      const unsigned __int8 talikEffect = GetTalikFromSocket(dwItemCurLv, j);
      if (IsTalikAboutTol(talikEffect))
      {
        return 0;
      }
    }
  }

  int tableGroup = byItemTableCode;
  switch (byItemTableCode)
  {
    case 6u:
    {
      _WeaponItem_fld *itemRecord = reinterpret_cast<_WeaponItem_fld *>(itemTable->GetRecord(wItemIndex));
      if (!itemRecord)
      {
        return 0;
      }
      const int classCode = itemRecord->m_nType;
      if (classCode == 6 || classCode == 8 || classCode == 7 || classCode == 5)
      {
        tableGroup = 7;
      }
      break;
    }
      case 19:
      tableGroup = 8;
      break;
    case 7u:
      tableGroup = 9;
      break;
    default:
      break;
  }

  if (tableGroup > 9)
  {
    return 0;
  }

  if (byItemTableCode == 7)
  {
    _CloakItem_fld *record = reinterpret_cast<_CloakItem_fld *>(itemTable->GetRecord(wItemIndex));
    if (!record)
    {
      return 0;
    }

    if (record->m_nProperty <= 0)
    {
      if (record->m_fFireTol <= 0.0f)
      {
        if (record->m_fWaterTol <= 0.0f)
        {
          if (record->m_fSoilTol <= 0.0f)
          {
            if (wTalikIndex != 3)
            {
              return 0;
            }
          }
          else if (wTalikIndex != 10)
          {
            return 0;
          }
        }
        else if (wTalikIndex != 9)
        {
          return 0;
        }
      }
      else if (wTalikIndex != 8)
      {
        return 0;
      }
    }
    else if (wTalikIndex != 7)
    {
      return 0;
    }
  }

  if (pnTalikLim[tableGroup] <= 0)
  {
    return 0;
  }

  int talikCount = 0;
  const unsigned __int8 currentUpgLv = GetItemUpgedLv(dwItemCurLv);
  for (int k = 0; k < currentUpgLv; ++k)
  {
    if (GetTalikFromSocket(dwItemCurLv, k) == wTalikIndex)
    {
      ++talikCount;
    }
    if (talikCount > pnTalikLim[tableGroup])
    {
      return 0;
    }
  }

  return 1;
}

int FixTalikItemIndex(unsigned __int8 byTalikEffectNum)
{
  if (byTalikEffectNum < 13)
  {
    return static_cast<int>(byTalikEffectNum + 20);
  }
  return -1;
}
CRecordData CPlayer::s_tblLimMasteryCum[3][4];
CRecordData CPlayer::s_tblLimMasteryCumContinue[3][4];
_BILLING_FORCE_CLOSE_DELAY CPlayer::s_BillingForceCloseDelay;
int CPlayer::s_nRaceNum[3];
float CPlayer::s_fExpDivUnderParty_Kill[8] = {1.0f, 1.1f, 1.5f, 2.0f, 2.5f, 3.0f, 3.5f, 4.5f};
unsigned int CPlayer::s_dwTotalCloseCount;
unsigned int CPlayer::s_dwAbnormalCloseCount;
CRecordData *_WEAPON_PARAM::s_pWeaponData;
CRecordData *_MASTERY_PARAM::s_pSkillData;
CRecordData *_MASTERY_PARAM::s_pForceData;
int _ATTACK_DELAY_CHECKER::s_nSpareTime = 100;
CRecordData ItemCombineMgr::ms_tbl_ItemCombine;
CRecordData ItemCombineMgr::ms_tbl_ItemCombine_Link_Stuff;
CRecordData ItemCombineMgr::ms_tbl_ItemCombine_Link_Result;

namespace
{
  int nMPL[4] = {0, 30, 60, 90};
  int nSpMPL[4] = {0, 30, 60, 90};
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

  int get_sf_record_class(unsigned __int8 byEffectCode, const _base_fld *sfRecord)
  {
    if (byEffectCode == 1)
    {
      const _force_fld *forceRecord = static_cast<const _force_fld *>(sfRecord);
      return forceRecord ? forceRecord->m_nClass : 0;
    }

    const _skill_fld *skillRecord = static_cast<const _skill_fld *>(sfRecord);
    return skillRecord ? skillRecord->m_nClass : 0;
  }

  int get_sf_need_special_type(unsigned __int8 byEffectCode, const _base_fld *sfRecord)
  {
    if (byEffectCode == 1)
    {
      const _force_fld *forceRecord = static_cast<const _force_fld *>(sfRecord);
      return forceRecord ? forceRecord->m_nNeedSpecialType : 0;
    }

    const _skill_fld *skillRecord = static_cast<const _skill_fld *>(sfRecord);
    return skillRecord ? skillRecord->m_nNeedSpecialType : 0;
  }

  char check_sf_class(int nClassCode, unsigned __int8 byEffectCode, const _base_fld *pSFFld)
  {
    const int sfClassCode = get_sf_record_class(byEffectCode, pSFFld);
    const int needSpecialType = get_sf_need_special_type(byEffectCode, pSFFld);

    if (nClassCode != 0)
    {
      switch (nClassCode)
      {
        case 1:
          if (byEffectCode)
          {
            if (byEffectCode == 1 && needSpecialType > 0)
            {
              return 0;
            }
          }
          else if (!sfClassCode && needSpecialType > 0)
          {
            return 0;
          }
          break;
        case 2:
          if (!byEffectCode && needSpecialType > 0)
          {
            return 0;
          }
          break;
        case 3:
          if (byEffectCode)
          {
            if (byEffectCode == 1 && needSpecialType > 0)
            {
              return 0;
            }
          }
          else if (needSpecialType > 1)
          {
            return 0;
          }
          break;
        default:
          return 0;
      }
    }
    else if (byEffectCode)
    {
      if (byEffectCode == 1 && needSpecialType > 0)
      {
        return 0;
      }
    }
    else if (sfClassCode == 1 && needSpecialType > 0)
    {
      return 0;
    }

    return 1;
  }
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
          const _GuardTowerItem_fld *towerRecord = static_cast<const _GuardTowerItem_fld *>(tower->m_pRecordSet);
          const int nearRange = towerRecord ? towerRecord->m_nGADst : 0;
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

bool ItemCombineMgr::LoadData()
{
  char szMessage[144] = {};
  if (ItemCombineMgr::ms_tbl_ItemCombine.ReadRecord_Ex(
        ".\\script\\CombineTable.dat",
        ".\\script\\CombineTable2.dat",
      968,
        szMessage))
  {
    if (ItemCombineMgr::ms_tbl_ItemCombine_Link_Stuff.ReadRecord(
          ".\\script\\LinkedStuff.dat",
      6468,
          szMessage))
    {
      if (ItemCombineMgr::ms_tbl_ItemCombine_Link_Result.ReadRecord(
            ".\\script\\LinkedResult.dat",
      6468,
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

CPlayer::CPlayer()
{
  m_pParkingUnit = nullptr;
}

_ATTACK_DELAY_CHECKER::_eff_list::_eff_list()
{
  byEffectCode = 255;
}

void _ATTACK_DELAY_CHECKER::_eff_list::init(_eff_list *self)
{
  self->byEffectCode = 255;
}

bool _ATTACK_DELAY_CHECKER::_eff_list::fill(_eff_list *self)
{
  return self->byEffectCode != 255;
}

_ATTACK_DELAY_CHECKER::_mas_list::_mas_list()
{
  byEffectCode = 255;
}

void _ATTACK_DELAY_CHECKER::_mas_list::init(_mas_list *self)
{
  self->byEffectCode = 255;
}

bool _ATTACK_DELAY_CHECKER::_mas_list::fill(_mas_list *self)
{
  return self->byEffectCode != 255;
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

  if (code == 255)
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
  if (self->byTemp_EffectCode == 255)
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

  if (self->byTemp_EffectCode != 2 && self->byTemp_EffectMastery != 255)
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

void _TRAP_PARAM::_param::init()
{
  pItem = nullptr;
}

void _TRAP_PARAM::Init()
{
  for (int j = 0; j < 20; ++j)
  {
    m_Item[j].init();
  }
  m_nCount = 0;
}

char _TOWER_PARAM::PushList(_STORAGE_LIST::_db_con *pTowerItem, CGuardTower *pTowerObj)
{
  for (int index = 0; index < 6; ++index)
  {
    if (!m_List[index].m_pTowerItem)
    {
      m_List[index].m_pTowerItem = pTowerItem;
      m_List[index].m_wItemSerial = pTowerItem->m_wSerial;
      m_List[index].m_pTowerObj = pTowerObj;
      return 1;
    }
  }
  return 0;
}

void _TOWER_PARAM::NotifyOwnerAttackInform(CCharacter *pDst)
{
  for (int index = 0; index < 6; ++index)
  {
    CGuardTower *tower = m_List[index].m_pTowerObj;
    if (tower)
    {
      tower->NotifyOwnerAttackInform(pDst);
    }
  }
}

char _TRAP_PARAM::PushItem(CTrap *pTrap, unsigned int dwTrapSerial)
{
  for (int index = 0; index < 20; ++index)
  {
    if (!m_Item[index].isLoad())
    {
      m_Item[index].pItem = pTrap;
      m_Item[index].dwSerial = dwTrapSerial;
      ++m_nCount;
      return 1;
    }
  }
  return 0;
}

char _TRAP_PARAM::PopItem(unsigned int dwTrapSerial)
{
  for (int index = 0; index < 20; ++index)
  {
    if (m_Item[index].isLoad() && m_Item[index].dwSerial == dwTrapSerial)
    {
      m_Item[index].init();
      --m_nCount;
      return 1;
    }
  }
  return 0;
}

void _CRYMSG_LIST::_LIST::Init()
{
  wszCryMsg[0] = 0;
}

void _CRYMSG_LIST::Init()
{
  for (int j = 0; j < 10; ++j)
  {
    m_List[j].Init();
  }
}

_UNIT_DB_BASE::_LIST::_LIST()
{
  Init(255);
}

void _UNIT_DB_BASE::_LIST::Init(unsigned __int8 byIndex)
{
  if (byIndex != 255)
  {
    bySlotIndex = byIndex;
  }
  byFrame = static_cast<unsigned __int8>(-1);
  std::memset(byPart, 255, sizeof(byPart));
  std::memset(dwBullet, -1, sizeof(dwBullet));
  std::memset(dwSpare, -1, sizeof(dwSpare));
  dwGauge = 0;
  nPullingFee = 0;
  dwCutTime = 0;
  wBooster = 0;
}

void _UNIT_DB_BASE::_LIST::DelUnit()
{
  byFrame = static_cast<unsigned __int8>(-1);
  std::memset(byPart, 255, sizeof(byPart));
  std::memset(dwBullet, -1, sizeof(dwBullet));
  std::memset(dwSpare, -1, sizeof(dwSpare));
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
  std::memset(this, 0, 8u);
  byLevel = static_cast<unsigned __int8>(-1);
}

_QUEST_DB_BASE::_START_NPC_QUEST_HISTORY::_START_NPC_QUEST_HISTORY()
{
  Init();
}

void _QUEST_DB_BASE::_START_NPC_QUEST_HISTORY::Init()
{
  std::strcpy(szQuestCode, "*");
  byLevel = static_cast<unsigned __int8>(-1);
  nEndTime = 0;
  std::memset(&tmStartTime, 0, sizeof(tmStartTime));
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

_happen_event_cont::_happen_event_cont()
{
  init();
}

void _happen_event_cont::init()
{
  std::memset(this, 0, sizeof(_happen_event_cont));
}

bool _happen_event_cont::isset() const
{
  return m_pEvent != nullptr;
}

void _happen_event_cont::set(_happen_event_node *pPoint, QUEST_HAPPEN QtHpType, int nIndexInType, int nRaceCode)
{
  m_pEvent = pPoint;
  m_QtHpType = QtHpType;
  m_nIndexInType = nIndexInType;
  m_nRaceCode = nRaceCode;
}

void _COMBINEKEY::SetRelease()
{
  byRewardIndex = static_cast<unsigned __int8>(-1);
  byTableCode = static_cast<unsigned __int8>(-1);
  wItemIndex = static_cast<unsigned __int16>(-1);
}

bool _COMBINEKEY::IsFilled()
{
  return byRewardIndex != 255 || byTableCode != 255 || wItemIndex != 65535;
}

void _COMBINEKEY::LoadDBKey(_COMBINEKEY pl_nKey)
{
  *this = pl_nKey;
}

__int64 _COMBINEKEY::CovDBKey()
{
  int key = 0;
  std::memcpy(&key, this, sizeof(key));
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
  dwUpt = 268435455;
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
            files[classIndex], 232, pszErrMsg))
      {
        return false;
      }
      if (!CPlayer::s_tblLimMasteryContinue[masteryIndex][classIndex].ReadRecord(
            files[classIndex], 232, pszErrMsg))
      {
        return false;
      }
      if (!CPlayer::s_tblLimMasteryCum[masteryIndex][classIndex].ReadRecord(
            files[classIndex], 232, pszErrMsg))
      {
        return false;
      }
      if (!CPlayer::s_tblLimMasteryCumContinue[masteryIndex][classIndex].ReadRecord(
            files[classIndex], 232, pszErrMsg))
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
  std::memset(s_pnLinkForceItemToEffect, -1, 8uLL);

  for (int n = 0; ; ++n)
  {
    const int recordCount = g_Main.m_tblItemData[15].GetRecordNum();
    if (n >= recordCount)
    {
      break;
    }
    _ForceItem_fld *record = reinterpret_cast<_ForceItem_fld *>(g_Main.m_tblItemData[15].GetRecord(n));
    if (!record)
    {
      g_Main.m_logSystemError.Write("CPlayer::SetStaticMember() : %d force..NULL", n);
      break;
    }
    _base_fld *effectRecord = g_Main.m_tblEffectData[1].GetRecord(record->m_strForce_Codekey);
    if (!effectRecord)
    {
      g_Main.m_logSystemError.Write("CPlayer::SetStaticMember() : %s force..NULL", record->m_strForce_Codekey);
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
    _skill_fld *record = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[0].GetRecord(n));
    const unsigned int masteryIndex = static_cast<unsigned int>(record->m_nMastIndex);
    if (record->m_nClass <= 1 && masteryIndex < 8u && record->m_nClass != 2)
    {
      _SKILL_IDX_PER_MASTERY *skillInfo = &CPlayer::s_SkillIndexPerMastery[masteryIndex];
      skillInfo->m_nSkillIndex[skillInfo->m_nSkillIndexNum++] = n;
    }
  }

  std::memset(CPlayer::s_nAddMstFc, 1, sizeof(CPlayer::s_nAddMstFc));
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
      2147483647,
      ".\\Initialize\\WorldSystem.ini");
    if (stdDefPoint == 2147483647)
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
      2147483647,
      ".\\Initialize\\WorldSystem.ini");
    if (revDefPoint == 2147483647)
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
  CPlayer::s_BillingForceCloseDelay.Init(2532, 1000 * delay);
}

void CPlayer::Init(_object_id *pID)
{
  CCharacter::Init(pID);
  m_EP.AllocEffParam();
  m_tmrIntervalSec.BeginTimer(1000);
  m_tmrBilling.BeginTimer(240000);
  m_pPartyMgr = &g_PartyPlayer[static_cast<unsigned __int64>(pID->m_wIndex)];
  m_bLoad = false;
  m_bOper = false;
  m_bPostLoad = false;
  m_bPostLoading = false;
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
  m_tmrGroupTargeting.BeginTimer(10000);
  m_bAfterEffect = false;
  m_bSFDelayNotCheck = false;
  m_tmrEffectStartTime.BeginTimer(3600000);
  m_tmrEffectEndTime.BeginTimer(60000);
  m_kMoveDelayChecker.Init(10);
  m_NameChangeBuddyInfo.Init();
  m_dwPcBangGiveItemListIndex = static_cast<unsigned int>(-1);
  m_tmrAccumPlayingTime.BeginTimer(300000);
  m_tmrPremiumPVPInform.BeginTimer(1800000);
}

char CPlayer::Load(CUserDB *pUser, bool bFirstStart)
{
  _AVATOR_DATA pData{};
  std::memcpy(&pData, &pUser->m_AvatorData, sizeof(pData));

  this->m_id.dwSerial = pData.dbAvator.m_dwRecordNum;
  this->m_pUserDB = nullptr;
  this->m_byUserDgr = pUser->m_byUserDgr;
  this->m_bySubDgr = pUser->m_bySubDgr;
  this->m_bFirstStart = bFirstStart;
  this->m_Param.InitPlayerDB(this);
  this->m_bMapLoading = 0;
  this->m_bOper = 0;
  this->m_bFullMode = pUser->m_bWndFullMode;
  this->m_bCheat_100SuccMake = 0;
  this->m_bCheat_makeitem_no_use_matrial = 0;
  this->m_bCheat_Matchless = 0;
  this->m_nMaxAttackPnt = 0;
  this->m_nAnimusAttackPnt = 0;
  this->m_nTrapMaxAttackPnt = 0;
  this->m_byDamagePart = static_cast<unsigned __int8>(-1);
  this->m_bFreeRecallWaitTime = 0;
  this->m_bFreeSFByClass = 0;
  this->m_bLootFree = 0;
  this->m_bOutOfMap = 0;
  this->m_dwNextTimeDungeonDie = 0;
  this->m_bNeverDie = 0;
  this->m_nVoteSerial = -1;
  this->m_bBlockParty = 0;
  this->m_bBlockWhisper = 0;
  this->m_bBlockTrade = 0;
  this->m_bSpyGM = 0;
  this->m_bTakeGravityStone = 0;
  this->m_bBlockGuildBattleMsg = 0;
  this->m_bInGuildBattle = 0;
  this->m_bNotifyPosition = 0;
  this->m_byGuildBattleColorInx = static_cast<unsigned __int8>(-1);
  this->m_bTakeSoccerBall = 0;
  this->m_pSoccerItem = nullptr;
  this->m_dwSelfDestructionTime = 0;
  this->m_fSelfDestructionDamage = 0.0f;
  this->m_fTalik_DefencePoint = 0.0f;
  this->m_fTalik_AvoidPoint = 0.0f;
  this->m_bCheckMovePacket = 0;
  this->m_bLinkBoardDownload = 0;
  this->m_bSpecialDownload = 0;
  this->m_bQuestDownload = 0;
  this->m_bCumDownload = 0;
  this->m_bForceDownload = 0;
  this->m_bInvenDownload = 0;
  this->m_bBaseDownload = 0;
  this->m_bAMPInvenDownload = 0;
  this->m_bBuddyListDownload = 0;
  this->m_bGuildDownload = 0;
  this->m_bGuildListDownload = 0;
  this->m_byMapInModeBuffer = 0;
  this->m_pBeforeTownCheckMap = nullptr;
  this->m_bCreateComplete = 0;
  this->m_bUpCheckEquipEffect = 1;
  this->m_bDownCheckEquipEffect = 0;
  std::memset(this->m_byEffectEquipCode, 0, sizeof(this->m_byEffectEquipCode));
  this->m_dwPcBangGiveItemListIndex = static_cast<unsigned int>(-1);

  CMapData *map = g_MapOper.GetMap(pData.dbAvator.m_byMapCode);
  const int raceIndex = pData.dbAvator.m_byRaceSexCode / 2;
  if (g_TransportShip[raceIndex].IsMemberBeforeLogoff(pData.dbAvator.m_dwRecordNum))
  {
    map = g_TransportShip[raceIndex].m_pLinkShipMap;
    pData.dbAvator.m_byMapCode = map->m_pMapSet->m_dwIndex;
    g_TransportShip[raceIndex].GetStartPosInShip(pData.dbAvator.m_fStartPos);
  }
  else
  {
    if (!map || !map->IsMapIn(pData.dbAvator.m_fStartPos) || map->m_pMapSet->m_nMapType)
    {
      CMapData *posStartMap = g_MapOper.GetPosStartMap(
        pData.dbAvator.m_byRaceSexCode / 2,
        1,
        pData.dbAvator.m_fStartPos);
      if (!posStartMap)
      {
        g_Main.m_logSystemError.Write(
          "Load() : failure : %s.. no find start position(race:%d, x:%d, y:%d, z:%d)..",
          pUser->m_aszAvatorName,
          pData.dbAvator.m_byRaceSexCode / 2,
          static_cast<int>(pData.dbAvator.m_fStartPos[0]),
          static_cast<int>(pData.dbAvator.m_fStartPos[1]),
          static_cast<int>(pData.dbAvator.m_fStartPos[2]));
        return 0;
      }
      pData.dbAvator.m_byMapCode = posStartMap->m_pMapSet->m_dwIndex;
    }
    map = g_MapOper.GetMap(pData.dbAvator.m_byMapCode);
    if (!map->m_bUse)
    {
      g_Main.m_logSystemError.Write(
        "Load() : failure : %s.. no used map(%s)..",
        pUser->m_aszAvatorName,
        map->m_pMapSet->m_strCode);
      return 0;
    }
  }

  LendItemMng::Instance()->Release(this->m_ObjID.m_wIndex);
  if (this->m_Param.ConvertAvatorDB(&pData)
      && this->m_Param.ConvertGeneralDB(&pData, &pUser->m_AvatorData))
  {
    CUnmannedTraderController::Instance()->Load(
      this->m_ObjID.m_wIndex,
      this->m_id.dwSerial,
      &pData.dbTrade);
    this->m_bLoad = 1;
    this->m_pUserDB = pUser;
    this->m_bMapLoading = 1;
    this->m_dwLastCheckRegionTime = 0;
    this->m_wRegionIndex = -1;
    this->m_wRegionMapIndex = -1;

    const long double exp = this->m_Param.GetExp();
    const int level = this->m_Param.GetLevel();
    const long double limitExp = cStaticMember_Player::Instance()->GetLimitExp(level);
    this->m_dwExpRate = static_cast<int>(exp / limitExp * 1000000.0);

    const unsigned __int8 raceCode = this->m_Param.GetRaceCode();
    this->m_pmMst.Init(&pUser->m_AvatorData.dbStat, raceCode);
    this->m_pmTrd.Init();
    this->m_pmWpn.Init();
    this->m_pmTwr.Init();
    this->m_pmTrp.Init();
    this->m_pmBuddy.Init();
    this->m_pmCryMsg.Init();

    this->CalcAddPointByClass();
    this->m_nMaxPoint[0] = this->_CalcMaxHP();
    this->m_nMaxPoint[1] = this->_CalcMaxFP();
    this->m_nMaxPoint[2] = this->_CalcMaxSP();
    this->CalcEquipMaxDP(1);
    std::memcpy(this->m_nOldPoint, this->m_nMaxPoint, sizeof(this->m_nOldPoint));
    this->m_nOldMaxDP = this->GetMaxDP();

    if (bFirstStart)
    {
      this->SetHP(this->m_nMaxPoint[0], 0);
      this->SetFP(this->m_nMaxPoint[1], 0);
      this->SetSP(this->m_nMaxPoint[2], 0);
      this->SetDP(this->GetMaxDP(), 0);
    }

    this->m_fEquipSpeed = 0.0;
    for (int j = 0; j < 50; ++j)
    {
      if (this->m_pUserDB->m_AvatorData.dbBuddy.m_List[j].IsFilled())
      {
        if (this->m_pUserDB->m_AvatorData.dbBuddy.m_List[j].wszName[0] == '*')
        {
          this->m_pUserDB->Update_DelBuddy(j);
        }
        this->m_pmBuddy.m_List[j].dwSerial = this->m_pUserDB->m_AvatorData.dbBuddy.m_List[j].dwSerial;
        std::strcpy(this->m_pmBuddy.m_List[j].wszName, this->m_pUserDB->m_AvatorData.dbBuddy.m_List[j].wszName);
      }
    }

    this->m_NameChangeBuddyInfo.Init();

    const unsigned int mapEnvCode = map->m_Level.GetEnvironment();
    this->SendMsg_MapEnvInform(map->m_pMapSet->m_dwIndex, mapEnvCode);
    this->m_pDHChannel = nullptr;
    if (this->m_pUserDB->m_byUserDgr == 2)
    {
      this->m_bSpyGM = !strncmp(&this->m_pUserDB->m_szAccountID[1], "SPY", 3u);
    }

    _PVPPOINT_LIMIT_DB_BASE *pkInfo = &this->m_pUserDB->m_AvatorData.dbPvpPointLimit;
    const long double pvpPoint = this->m_Param.GetPvPPoint();
    if (this->m_kPvpPointLimiter.Set(pvpPoint, pkInfo))
    {
      this->m_pUserDB->UpdateContUserSave(1);
    }

    for (int k = 0; k < 10; ++k)
    {
      std::strcpy(this->m_pmCryMsg.m_List[k].wszCryMsg, this->m_pUserDB->m_AvatorData.dbBossCry.m_List[k].wszCryMsg);
    }

    _PVP_ORDER_VIEW_DB_BASE *p_dbPvpOrderView = &this->m_pUserDB->m_AvatorData.dbPvpOrderView;
    const long double pvpPointView = this->m_Param.GetPvPPoint();
    if (this->m_kPvpOrderView.SetPvpOrderView(pvpPointView, p_dbPvpOrderView, this))
    {
      this->m_pUserDB->UpdateContUserSave(1);
    }

    for (int m = 0; m < 3; ++m)
    {
      this->m_fGroupMapPoint[m][0] = 0.0;
      this->m_fGroupMapPoint[m][1] = 0.0;
      this->m_byGroupMapPointMapCode[m] = 0;
      this->m_wGroupMapPointLayerIndex[m] = 0;
      this->m_dwLastGroupMapPointTime[m] = 0;
    }

    this->m_byPlusKey = rand();
    this->m_wXorKey = rand();
    this->m_dwMoveCount = 0;
    this->m_dwTargetCount = 0;
    this->m_dwAttackCount = 0;
    this->m_bSFDelayNotCheck = 0;

    this->m_kPvpCashPoint.Init(&this->m_pUserDB->m_AvatorData.dbPvpOrderView);

    _PCBANG_PLAY_TIME *p_dbPlayTimeInPcbang = &this->m_pUserDB->m_AvatorData.dbPlayTimeInPcbang;
    CUserDB *m_pUserDB = this->m_pUserDB;
    this->m_kPcBangCoupon.LoadData(m_pUserDB->m_dwAccountSerial, p_dbPlayTimeInPcbang);

    this->SetLastAttBuff(this->m_pUserDB->m_AvatorData.dbSupplement.bLastAttBuff);

    for (int n = 0; n < 38; ++n)
    {
      this->m_PotionParam.SetPotionActDelay(
        n,
        0,
        this->m_pUserDB->m_AvatorData.dbPotionNextUseTime.dwPotionNextUseTime[n]);
    }

    this->m_bCntEnable = 0;
    return 1;
  }

  g_Main.m_logSystemError.Write(
    "Load() : failure : %d:%s",
    pData.dbAvator.m_dwRecordNum,
    pUser->m_aszAvatorName);
  return 0;
}

CPlayer::~CPlayer()
{
  if (s_pnLinkForceItemToEffect)
  {
    operator delete[](s_pnLinkForceItemToEffect);
    s_pnLinkForceItemToEffect = nullptr;
  }
}

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
  std::memset(strActiveCode_key, 0, sizeof(strActiveCode_key));
  std::memset(strEffBulletType, 0, sizeof(strEffBulletType));
}

void _WEAPON_PARAM::FixWeapon(_STORAGE_LIST::_db_con *pWeapon)
{
  if (!pWeapon)
  {
    Init();
    return;
  }
  if (pWeapon->m_byTableCode == 6)
  {
    _WeaponItem_fld *record = nullptr;
    if (s_pWeaponData)
    {
      record = reinterpret_cast<_WeaponItem_fld *>(s_pWeaponData->GetRecord(pWeapon->m_wItemIndex));
    }

    if (!record || record->m_nType == 10)
    {
      Init();
      return;
    }

    byAttTolType = static_cast<unsigned __int8>(GetWeaponTolType(pWeapon));
    byWpClass = static_cast<unsigned __int8>(GetWeaponClass(pWeapon->m_wItemIndex));
    nGaMaxAF = static_cast<int>(record->m_fGAMaxAF);
    nGaMinAF = static_cast<int>(record->m_fGAMinAF);
    nMaMaxAF = static_cast<int>(record->m_fMAMaxAF);
    nMaMinAF = static_cast<int>(record->m_fMAMinAF);
    byGaMinSel = static_cast<unsigned __int8>(record->m_nGAMinSelProb);
    byGaMaxSel = static_cast<unsigned __int8>(record->m_nGAMaxSelProb);
    byMaMinSel = static_cast<unsigned __int8>(record->m_nMAMinSelProb);
    byMaMaxSel = static_cast<unsigned __int8>(record->m_nMAMaxSelProb);
    byWpType = static_cast<unsigned __int8>(record->m_nType);
    wGaAttRange = static_cast<unsigned __int16>(static_cast<int>(record->m_fGADst + 40.0f));
    nActiveType = record->m_nActiveType;
    nActiveEffLvl = record->m_nActiveEffLvl;
    nActiveProb = static_cast<int>(record->m_fActiveProba * 100.0f);
  strncpy(strActiveCode_key, record->m_strActiveCode_key, 64);
  strncpy(strEffBulletType, record->m_strEffBulletType, 64);
    pFixWp = pWeapon;
    pFixUnit = nullptr;
  }
}

void _WEAPON_PARAM::FixUnit(_UNIT_DB_BASE::_LIST *pUnit)
{
  Init();
  byWpClass = 0;
  byWpType = 0;
  pFixWp = nullptr;
  pFixUnit = pUnit;
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

  _WeaponItem_fld *record = nullptr;
  if (s_pWeaponData)
  {
    record = reinterpret_cast<_WeaponItem_fld *>(s_pWeaponData->GetRecord(pItem->m_wItemIndex));
  }
  if (!record)
  {
    return 0;
  }
  return static_cast<unsigned int>(record->m_nProperty);
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

void _RENAME_POTION_USE_INFO::Init()
{
  std::memset(this, 0, sizeof(*this));
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

char _MASTERY_PARAM::Init(_STAT_DB_BASE *pStatBase, unsigned __int8 byRaceCode)
{
  m_byRaceCode = byRaceCode;
  std::memcpy(&m_BaseCum, pStatBase, sizeof(m_BaseCum));
  std::memset(m_dwForceLvCum, 0, sizeof(m_dwForceLvCum));
  m_MastUpData.init();
  m_SkillUpData.init();
  m_bUpdateEquipMast = 0;

  for (int nMasteryIndex = 0; nMasteryIndex < 2; ++nMasteryIndex)
  {
    const unsigned __int8 mastery =
      static_cast<unsigned __int8>(CalcMastery(0, nMasteryIndex, m_BaseCum.m_dwDamWpCnt[nMasteryIndex], m_byRaceCode));
    m_mtyWp[nMasteryIndex] = mastery;
  }

  m_mtySuffer =
    static_cast<unsigned __int8>(CalcMastery(1, 0, m_BaseCum.m_dwDefenceCnt, m_byRaceCode));
  m_mtyShield =
    static_cast<unsigned __int8>(CalcMastery(2, 0, m_BaseCum.m_dwShieldCnt, m_byRaceCode));

  std::memset(m_dwSkillMasteryCum, 0, sizeof(m_dwSkillMasteryCum));
  for (int nMasteryIndex = 0; nMasteryIndex < 48; ++nMasteryIndex)
  {
    _skill_fld *record = reinterpret_cast<_skill_fld *>(s_pSkillData->GetRecord(nMasteryIndex));
    if (!record)
    {
      return 0;
    }
    if (record->m_nLv > 3)
    {
      return 0;
    }
    if (record->m_nLv >= 0)
    {
      const unsigned __int8 sfLevel = GetSFLevel(
        record->m_nLv,
        m_BaseCum.m_dwSkillCum[nMasteryIndex]);
      m_lvSkill[nMasteryIndex] = sfLevel;
    }
    if (record->m_nMastIndex < 8)
    {
      m_dwSkillMasteryCum[record->m_nMastIndex] +=
        m_BaseCum.m_dwSkillCum[nMasteryIndex];
    }
  }

  for (int nMasteryIndex = 0; nMasteryIndex < 8; ++nMasteryIndex)
  {
    const unsigned __int8 mastery =
      static_cast<unsigned __int8>(CalcMastery(3, nMasteryIndex, m_dwSkillMasteryCum[nMasteryIndex], m_byRaceCode));
    m_mtySkill[nMasteryIndex] = mastery;
  }

  for (int nMasteryIndex = 0; nMasteryIndex < 24; ++nMasteryIndex)
  {
    const unsigned __int8 mastery =
      static_cast<unsigned __int8>(CalcMastery(4, nMasteryIndex, m_BaseCum.m_dwForceCum[nMasteryIndex], m_byRaceCode));
    m_mtyForce[nMasteryIndex] = mastery;
    m_dwForceLvCum[nMasteryIndex % 4] = m_BaseCum.m_dwForceCum[nMasteryIndex] + m_dwForceLvCum[nMasteryIndex % 4];
  }

  m_mtyStaff = GetStaffMastery(m_dwForceLvCum);
  m_mtyMakeItem[0] = static_cast<unsigned __int8>(CalcMastery(5, 0, m_BaseCum.m_dwMakeCum[0], m_byRaceCode));
  m_mtyMakeItem[1] = static_cast<unsigned __int8>(CalcMastery(5, 1, m_BaseCum.m_dwMakeCum[1], m_byRaceCode));
  m_mtyMakeItem[2] = static_cast<unsigned __int8>(CalcMastery(5, 2, m_BaseCum.m_dwMakeCum[2], m_byRaceCode));
  m_mtySpecial = static_cast<unsigned __int8>(CalcMastery(6, 0, m_BaseCum.m_dwSpecialCum, m_byRaceCode));

  m_ppdwMasteryCumPtr[0] = m_BaseCum.m_dwDamWpCnt;
  m_ppbyMasteryPtr[0] = m_mtyWp;
  m_ppdwMasteryCumPtr[1] = &m_BaseCum.m_dwDefenceCnt;
  m_ppbyMasteryPtr[1] = &m_mtySuffer;
  m_ppdwMasteryCumPtr[2] = &m_BaseCum.m_dwShieldCnt;
  m_ppbyMasteryPtr[2] = &m_mtyShield;
  m_ppdwMasteryCumPtr[3] = m_dwSkillMasteryCum;
  m_ppbyMasteryPtr[3] = m_mtySkill;
  m_ppdwMasteryCumPtr[4] = m_BaseCum.m_dwForceCum;
  m_ppbyMasteryPtr[4] = m_mtyForce;
  m_ppdwMasteryCumPtr[5] = m_BaseCum.m_dwMakeCum;
  m_ppbyMasteryPtr[5] = m_mtyMakeItem;
  m_ppdwMasteryCumPtr[6] = &m_BaseCum.m_dwSpecialCum;
  m_ppbyMasteryPtr[6] = &m_mtySpecial;
  m_ppbyEquipMasteryPrt[0] = m_mtyWp;
  m_ppbyEquipMasteryPrt[1] = &m_mtyWp[1];
  m_ppbyEquipMasteryPrt[2] = &m_mtySpecial;
  m_ppbyEquipMasteryPrt[3] = &m_mtyStaff;
  m_ppbyEquipMasteryPrt[4] = &m_mtyShield;
  m_ppbyEquipMasteryPrt[5] = &m_mtySuffer;
  return 1;
}

char _MASTERY_PARAM::IsValidMasteryCode(unsigned __int8 byCode, unsigned __int8 byIndex)
{
  if (byCode)
  {
    switch (byCode)
    {
      case 3u:
        if (byIndex >= 8u)
        {
          return 0;
        }
        break;
      case 4u:
  if (byIndex >= 24)
        {
          return 0;
        }
        break;
      case 5u:
        if (byIndex >= 3u)
        {
          return 0;
        }
        break;
      case 1u:
      case 2u:
      case 6u:
        if (byIndex)
        {
          return 0;
        }
        break;
      default:
        return 0;
    }
  }
  else if (byIndex >= 2u)
  {
    return 0;
  }
  return 1;
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

bool _MASTERY_PARAM::AlterCumPerMast(
  unsigned __int8 byClass,
  unsigned __int8 byIndex,
  unsigned int dwAlterCum,
  unsigned int *pdwAfterCum)
{
  bool updated = false;

  m_MastUpData.init();
  m_SkillUpData.init();
  m_bUpdateEquipMast = false;

  switch (byClass)
  {
    case 0:
    {
      m_BaseCum.m_dwDamWpCnt[byIndex] += dwAlterCum;
      *pdwAfterCum = m_BaseCum.m_dwDamWpCnt[byIndex];
  if (m_mtyWp[byIndex] < 99)
      {
        const unsigned int mastery = CalcMastery(0, byIndex, m_BaseCum.m_dwDamWpCnt[byIndex], m_byRaceCode);
        if (m_mtyWp[byIndex] < mastery)
        {
          m_mtyWp[byIndex] = static_cast<unsigned __int8>(mastery);
          m_MastUpData.set(0, byIndex, static_cast<unsigned __int8>(mastery));
          updated = true;
          m_bUpdateEquipMast = true;
        }
      }
      break;
    }
    case 1:
    {
      m_BaseCum.m_dwDefenceCnt += dwAlterCum;
      *pdwAfterCum = m_BaseCum.m_dwDefenceCnt;
  if (m_mtySuffer < 99)
      {
        const unsigned int mastery = CalcMastery(1, 0, m_BaseCum.m_dwDefenceCnt, m_byRaceCode);
        if (m_mtySuffer < mastery)
        {
          m_mtySuffer = static_cast<unsigned __int8>(mastery);
          m_MastUpData.set(1u, 0, static_cast<unsigned __int8>(mastery));
          updated = true;
          m_bUpdateEquipMast = true;
        }
      }
      break;
    }
    case 2:
    {
      m_BaseCum.m_dwShieldCnt += dwAlterCum;
      *pdwAfterCum = m_BaseCum.m_dwShieldCnt;
  if (m_mtyShield < 99)
      {
        const unsigned int mastery = CalcMastery(2, 0, m_BaseCum.m_dwShieldCnt, m_byRaceCode);
        if (m_mtyShield < mastery)
        {
          m_mtyShield = static_cast<unsigned __int8>(mastery);
          m_MastUpData.set(2u, 0, static_cast<unsigned __int8>(mastery));
          updated = true;
          m_bUpdateEquipMast = true;
        }
      }
      break;
    }
    case 3:
    {
      m_BaseCum.m_dwSkillCum[byIndex] += dwAlterCum;
      *pdwAfterCum = m_BaseCum.m_dwSkillCum[byIndex];
      _skill_fld *record = reinterpret_cast<_skill_fld *>(s_pSkillData->GetRecord(byIndex));
      const unsigned __int8 sfLevel = static_cast<unsigned __int8>(
        GetSFLevel(record->m_nLv, m_BaseCum.m_dwSkillCum[byIndex]));
      if (sfLevel > m_lvSkill[byIndex])
      {
        m_lvSkill[byIndex] = sfLevel;
        m_SkillUpData.set(byIndex, sfLevel);
        updated = true;
      }
      const int skillClass = record->m_nMastIndex;
  if (m_mtySkill[skillClass] < 99)
      {
        m_dwSkillMasteryCum[skillClass] += dwAlterCum;
        const unsigned int mastery =
          CalcMastery(3, skillClass, m_dwSkillMasteryCum[skillClass], m_byRaceCode);
        if (m_mtySkill[skillClass] < mastery)
        {
          m_mtySkill[skillClass] = static_cast<unsigned __int8>(mastery);
          m_MastUpData.set(3u, static_cast<unsigned __int8>(record->m_nMastIndex),
                           static_cast<unsigned __int8>(mastery));
          updated = true;
        }
      }
      break;
    }
    case 4:
    {
      m_BaseCum.m_dwForceCum[byIndex] += dwAlterCum;
      *pdwAfterCum = m_BaseCum.m_dwForceCum[byIndex];
      const unsigned int forceLvCum = dwAlterCum + m_dwForceLvCum[byIndex & 3];
      m_dwForceLvCum[byIndex & 3] = forceLvCum;
  if (m_mtyForce[byIndex] < 99)
      {
        const unsigned int mastery = CalcMastery(4, byIndex, m_BaseCum.m_dwForceCum[byIndex], m_byRaceCode);
        if (m_mtyForce[byIndex] < mastery)
        {
          m_mtyForce[byIndex] = static_cast<unsigned __int8>(mastery);
          m_MastUpData.set(4u, byIndex, static_cast<unsigned __int8>(mastery));
          updated = true;
        }
      }
  if (m_mtyForce[byIndex] < 99)
      {
        const unsigned int staffMastery = GetStaffMastery(m_dwForceLvCum);
        if (m_mtyStaff < staffMastery)
        {
          m_mtyStaff = static_cast<unsigned __int8>(staffMastery);
          updated = true;
          m_bUpdateEquipMast = true;
        }
      }
      break;
    }
    case 5:
    {
      m_BaseCum.m_dwMakeCum[byIndex] += dwAlterCum;
      *pdwAfterCum = m_BaseCum.m_dwMakeCum[byIndex];
  if (m_mtyMakeItem[byIndex] < 99)
      {
        unsigned int mastery = 0;
        if (byIndex)
        {
          if (byIndex == 1)
          {
            mastery = CalcMastery(5, 1, m_BaseCum.m_dwMakeCum[byIndex], m_byRaceCode);
          }
          else if (byIndex == 2)
          {
            mastery = CalcMastery(5, 2, m_BaseCum.m_dwMakeCum[byIndex], m_byRaceCode);
          }
        }
        else
        {
          mastery = CalcMastery(5, 0, m_BaseCum.m_dwMakeCum[0], m_byRaceCode);
        }
        if (m_mtyMakeItem[byIndex] < mastery)
        {
          m_mtyMakeItem[byIndex] = static_cast<unsigned __int8>(mastery);
          m_MastUpData.set(5u, byIndex, static_cast<unsigned __int8>(mastery));
          updated = true;
        }
      }
      break;
    }
    case 6:
    {
      m_BaseCum.m_dwSpecialCum += dwAlterCum;
      *pdwAfterCum = m_BaseCum.m_dwSpecialCum;
  if (m_mtySpecial < 99)
      {
        const unsigned int mastery = CalcMastery(6, 0, m_BaseCum.m_dwSpecialCum, m_byRaceCode);
        if (m_mtySpecial < mastery)
        {
          m_mtySpecial = static_cast<unsigned __int8>(mastery);
          m_MastUpData.set(6u, 0, static_cast<unsigned __int8>(mastery));
          updated = true;
        }
      }
      break;
    }
    default:
      return updated;
  }

  return updated;
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
  if (m_mtyWp[byIndex] <= 99)
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
  if (m_mtySuffer <= 99)
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
  if (m_mtyShield <= 99)
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
      _skill_fld *record = reinterpret_cast<_skill_fld *>(s_pSkillData->GetRecord(byIndex));
      const unsigned __int8 skillLevel =
        static_cast<unsigned __int8>(GetSFLevel(record->m_nLv,
                                                m_BaseCum.m_dwSkillCum[byIndex]));
      m_lvSkill[byIndex] = skillLevel;
      m_SkillUpData.set(byIndex, skillLevel);

      const int skillClass = record->m_nMastIndex;
  if (m_mtySkill[skillClass] <= 99)
      {
        m_dwSkillMasteryCum[skillClass] += dwNewCum;
        const unsigned __int8 mastery =
          static_cast<unsigned __int8>(CalcMastery(3, skillClass, m_dwSkillMasteryCum[skillClass], m_byRaceCode));
        m_mtySkill[skillClass] = mastery;
        m_MastUpData.set(3u, static_cast<unsigned __int8>(record->m_nMastIndex), mastery);
      }
      break;
    }
    case 4:
    {
      m_BaseCum.m_dwForceCum[byIndex] = dwNewCum;
      m_dwForceLvCum[byIndex & 3] = dwNewCum;
  if (m_mtyForce[byIndex] <= 99)
      {
        const unsigned __int8 mastery =
          static_cast<unsigned __int8>(CalcMastery(4, byIndex, m_BaseCum.m_dwForceCum[byIndex], m_byRaceCode));
        m_mtyForce[byIndex] = mastery;
        m_MastUpData.set(4u, byIndex, mastery);
      }
  if (m_mtyForce[byIndex] <= 99)
      {
        m_mtyStaff = static_cast<unsigned __int8>(GetStaffMastery(m_dwForceLvCum));
      }
      break;
    }
    case 5:
    {
      m_BaseCum.m_dwMakeCum[byIndex] = dwNewCum;
  if (m_mtyMakeItem[byIndex] <= 99)
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
  if (m_mtySpecial <= 99)
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

MiningTicket::MiningTicket()
{
  Init();
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
  std::strcpy(wszName, pwszName);
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
  std::strcpy(entry->wszName, pwszName);
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
      std::strcpy(m_List[index].wszName, pwszName);
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
  _goto_avator_result_zocl msg{};
  msg.byRet = byErrCode;
  msg.byMapCode = byMapCode;
  std::memcpy(msg.fStartPos, pfStartPos, sizeof(msg.fStartPos));
  msg.byMapInType = static_cast<char>(byMapInType);

  unsigned __int8 type[2]{};
  type[0] = 4;
  type[1] = 29;
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_GotoBasePortalResult(char byErrCode)
{
  _goto_baseportal_result_zocl msg{};
  msg.byRet = byErrCode;
  msg.byMapCode = static_cast<char>(m_pCurMap->m_pMapSet->m_dwIndex);
  FloatToShort(m_fCurPos, msg.zPos, 3);

  unsigned __int8 type[2]{};
  type[0] = 4;
  type[1] = 27;
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_MovePortal(
  char byRet,
  char byMapIndex,
  unsigned __int8 byPotalIndex,
  float *pfStartPos,
  bool bEqualZone)
{
  _move_potal_result_zocl msg{};
  msg.byRet = byRet;
  msg.byMapIndex = byMapIndex;
  std::memcpy(msg.fStartPos, pfStartPos, sizeof(msg.fStartPos));
  msg.byZoneCode = static_cast<char>(!bEqualZone);
  unsigned __int8 type[2] = {8, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_MovePortal(char byMapIndex, float *pfStartPos, unsigned __int8 byZoneCode)
{
  _move_potal_result_zocl msg{};
  msg.byRet = 0;
  msg.byMapIndex = byMapIndex;
  std::memcpy(msg.fStartPos, pfStartPos, sizeof(msg.fStartPos));
  msg.byZoneCode = static_cast<char>(byZoneCode);
  unsigned __int8 type[2] = {8, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_GuildRoomOutResult(
  unsigned __int8 byRetCode,
  unsigned __int8 byMapIndex,
  unsigned __int16 wMapLayer,
  float *pPos)
{
  _guildroom_out_result_zocl msg{};
  msg.byRetCode = byRetCode;
  msg.byMapIndex = byMapIndex;
  msg.wMapLayerIndex = wMapLayer;
  if (pPos)
  {
    FloatToShort(pPos, msg.sNewPos, 3);
  }

  unsigned __int8 type[2] = {27, 107};
  unsigned __int16 nLen = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg( m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), nLen);
}

void CPlayer::SendMsg_RegistBindResult(char byRetCode)
{
  _exit_world_result_zocl msg{};
  msg.dummy[0] = byRetCode;
  unsigned __int8 type[2] = {8, 4};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_MoveError(char byRetCode)
{
  _move_error_result_zocl msg{};
  msg.byErrCode = byRetCode;
  std::memcpy(msg.fCur, m_fCurPos, sizeof(msg.fCur));

  unsigned __int8 type[2]{4, 3};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_Stop(bool bAll)
{
  _player_stop_zocl msg{};
  msg.dwSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, msg.zCur, 3);

  unsigned __int8 type[2]{4, 20};
  if (bAll)
  {
    CircleReport(type, reinterpret_cast<char *>(&msg), sizeof(msg), true);
  }
  else
  {
    g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
  }
}

void CPlayer::SendMsg_MoveNext(bool bOtherSend)
{
  _player_move_zocl msg{};
  msg.dwSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, msg.zCur, 3);
  msg.zTar[0] = static_cast<short>(m_fTarPos[0]);
  msg.zTar[1] = static_cast<short>(m_fTarPos[2]);
  msg.nAddSpeed = static_cast<short>(GetAddSpeed());
  msg.byDirect = static_cast<char>(m_byMoveDirect);

  unsigned __int8 type[2]{4, 4};
  if (bOtherSend)
  {
    CircleReport(type, reinterpret_cast<char *>(&msg), sizeof(msg), true);
  }
  else
  {
    g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
  }
}

void CPlayer::SendMsg_QuestProcess(char byQuestDBSlot, char byActIndex, unsigned __int16 wCount)
{
  _quest_process_inform_zocl msg{};
  msg.byQuestDBSlot = byQuestDBSlot;
  msg.byActIndex = byActIndex;
  msg.wCount = wCount;

  unsigned __int8 type[2]{24, 6};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_SelectQuestReward(char byQuestDBSlot)
{
  _quest_select_reward_inform_zocl msg{};
  msg.byQuestDBSlot = byQuestDBSlot;

  unsigned __int8 type[2]{24, 8};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_QuestFailure(char byFailCode, char byQuestDBSlot)
{
  _quest_fail_inform_zocl msg{};
  msg.byFailCode = byFailCode;
  msg.byQuestDBSlot = byQuestDBSlot;

  unsigned __int8 type[2]{24, 7};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_InsertNextQuest(unsigned __int8 bySlotIndex, _QUEST_DB_BASE::_LIST *pQuestDB)
{
  _insert_next_quest_inform_zocl msg{};
  msg.byQuestDBSlot = bySlotIndex;
  std::memcpy(&msg.NewQuestData, pQuestDB, sizeof(msg.NewQuestData));

  unsigned __int8 type[2]{24, 111};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_InsertNewQuest(unsigned __int8 bySlotIndex, _QUEST_DB_BASE::_LIST *pQuestDB)
{
  _insert_new_quest_inform_zocl msg{};
  msg.byQuestDBSlot = bySlotIndex;
  std::memcpy(&msg.NewQuestData, pQuestDB, sizeof(msg.NewQuestData));

  unsigned __int8 type[2]{24, 1};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_QuestComplete(char byQuestDBSlot)
{
  _quest_complete_inform_zocl msg{};
  msg.byQuestDBSlot = byQuestDBSlot;

  unsigned __int8 type[2]{24, 5};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_NpcQuestHistoryInform(char bySlotIndex)
{
  _npc_quest_history_inform_zocl msg{};
  msg.byIndex = bySlotIndex;
  std::strcpy(msg.szQuestCode, m_QuestMgr.m_pQuestData->m_History[static_cast<unsigned __int8>(bySlotIndex)].szQuestCode);

  unsigned __int8 type[2]{24, 19};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
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
  const unsigned __int8 prevMoveType = m_byMoveType;
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
  else if (m_EP.GetEff_State(EFF_STATE_MOVE_RESTRICTED))
  {
    errorCode = 2;
  }
  else if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_7) && byMoveType == 1)
  {
    errorCode = 3;
  }
  else if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE))
  {
    errorCode = 13;
  }
  else if (m_pCurMap->IsMapIn(pfCur))
  {
    if (m_byUserDgr || (std::fabs(m_fCurPos[0] - pfCur[0]) <= 100.0f && std::fabs(m_fCurPos[2] - pfCur[2]) <= 100.0f))
    {
      const int raceTown = m_byPosRaceTown;
      const int raceCode = m_Param.GetRaceCode();
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
      Stop();
    }
    return;
  }

  const int raceTown = m_byPosRaceTown;
  const int raceCode = m_Param.GetRaceCode();
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
  std::memcpy(m_fOldPos, m_fCurPos, sizeof(m_fOldPos));
  std::memcpy(m_fCurPos, pfCur, sizeof(m_fCurPos));

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

  std::memcpy(m_fTarPos, pfTar, sizeof(m_fTarPos));
  ResetSlot();
  Go();
  SendMsg_MoveNext(otherSend);
  if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_14))
  {
    RemoveSFContHelpByEffect(2, 14);
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
    if (m_kMoveDelayChecker.Check(this))
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
      else if (!std::memcmp(pfCur, m_fCurPos, 12))
      {
        errorCode = 6;
      }
      else if (m_byStandType == 1)
      {
        errorCode = 10;
      }
      else if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE))
      {
        errorCode = 13;
      }
      else if (m_byUserDgr
               || (std::fabs(m_fCurPos[0] - pfCur[0]) <= 200.0f
                   && std::fabs(m_fCurPos[2] - pfCur[2]) <= 200.0f))
      {
        if (!m_pCurMap->IsMapIn(pfCur))
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
      Stop();
    }
  }
  else
  {
    std::memcpy(m_fOldPos, m_fCurPos, sizeof(m_fOldPos));
    std::memcpy(m_fCurPos, pfCur, sizeof(m_fCurPos));
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
      if (!m_pCurMap->IsMapIn(pfCur))
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

  m_kMoveDelayChecker.Reset();
  if (errorCode)
  {
    SendMsg_MoveError(static_cast<char>(errorCode));
    if (m_bMove)
    {
      const bool outExtra = IsOutExtraStopPos(m_fCurPos);
      SendMsg_Stop(outExtra);
      Stop();
    }
  }
  else
  {
    std::memcpy(m_fOldPos, m_fCurPos, sizeof(m_fOldPos));
    std::memcpy(m_fCurPos, pfCur, sizeof(m_fCurPos));
    ++m_nCheckMovePacket;
    const bool outExtra = IsOutExtraStopPos(m_fCurPos);
    SendMsg_Stop(outExtra);
    Stop();
  }
}

void CPlayer::pc_Stop()
{
  if (!m_bCorpse && m_bMove)
  {
    Stop();
    SendMsg_BreakStop();
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
  // Yorozuya fix implementation (non-IDA): block forced base-portal packets while
  // a normal player is in battle mode instead of trusting the client to hide the action.
  else if (!m_byUserDgr && Is_Battle_Mode())
  {
    resultCode = 8;
  }
  else if (GetCurSecNum() == -1 || m_bMapLoading)
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
  else if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE))
  {
    resultCode = 9;
  }
  else if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
  {
    resultCode = 9;
  }
  // Yorozuya fix implementation (non-IDA): the free start-point teleport path is GM-only.
  else if (!m_byUserDgr && wItemSerial == 65535)
  {
    resultCode = 8;
  }
  else if (wItemSerial == 65535)
  {
    const unsigned __int8 raceCode = m_Param.GetRaceCode();
    intoMap = g_MapOper.GetPosStartMap(raceCode, 0, newPos);
    if (!intoMap)
    {
      resultCode = 11;
    }
  }
  else
  {
    item = m_Param.m_dbInven.GetPtrFromSerial(wItemSerial);
    if (item)
    {
      if (item->m_byTableCode == 22)
      {
        if (g_HolySys.IsUseReturnItem(m_dwObjSerial))
        {
          if (item->m_bLock)
          {
            resultCode = 22;
          }
          else
          {
            _TOWNItem_fld *townRecord =
              reinterpret_cast<_TOWNItem_fld *>(g_Main.m_tblItemData[22].GetRecord(item->m_wItemIndex));
            record = reinterpret_cast<_base_fld *>(townRecord);
            if (record)
            {
              if (m_pCurMap->m_pMapSet->m_nMapType == 2)
              {
                resultCode = 11;
              }
              else if (m_pCurMap->m_pMapSet->m_nMapClass
                       && !townRecord->m_bUseableNormalAcc
                       && (std::strcmp(m_pCurMap->m_pMapSet->m_strCode, "Elan")
                           || (!std::strcmp(m_pCurMap->m_pMapSet->m_strCode, "Platform01")
                               || !std::strcmp(m_pCurMap->m_pMapSet->m_strCode, "Medicallab"))
                             && m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_55) == 0.0)
                       && std::strcmp(townRecord->m_strMapCode, m_pCurMap->m_pMapSet->m_strCode))
              {
                resultCode = 11;
              }
              else
              {
                const int level = static_cast<int>(GetLevel());
                if (level >= townRecord->m_nLV)
                {
                  if (townRecord->m_nUpLevelLim == -1
                    || static_cast<int>(GetLevel()) <= townRecord->m_nUpLevelLim)
                  {
                    intoMap = g_MapOper.GetMap(townRecord->m_strMapCode);
                    if (intoMap)
                    {
                      _dummy_position *pos = intoMap->GetDummyPostion(townRecord->m_strDummyName);
                      if (pos)
                      {
                        if (intoMap->GetRandPosInDummy(pos, newPos, 1))
                        {
                          if (Major_Scroll_Item && record)
                          {
                            if (!std::strcmp(townRecord->m_strMapCode, "Elan")
                              || !std::strcmp(townRecord->m_strMapCode, "NeutralA")
                              || !std::strcmp(townRecord->m_strMapCode, "NeutralB")
                              || !std::strcmp(townRecord->m_strMapCode, "NeutralC"))
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
                              SendMsg_PremiumCashItemUse(65535);
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
    if (wItemSerial != 65535)
    {
      Emb_AlterDurPoint(0, item->m_byStorageIndex, -1, false, false);
    }
    Emb_CheckActForQuest(16, record->m_strCode, 1, false);
  }
  SendMsg_GotoBasePortalResult(static_cast<char>(resultCode));
}

void CPlayer::pc_GotoAvatorRequest(const char *pwszAvatorName)
{
  unsigned __int8 resultCode = 0;
  CPlayer *target = nullptr;

  if (m_pmTrd.bDTradeMode)
  {
    resultCode = 6;
  }
  else if (GetCurSecNum() == -1 || m_bMapLoading)
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
        const char *name = player->m_Param.GetCharNameW();
        if (!std::strcmp(name, pwszAvatorName))
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

  const unsigned __int8 mapCode = m_Param.GetMapCode();
  SendMsg_GotoRecallResult(static_cast<char>(resultCode), mapCode, startPos, 4);
}

void CPlayer::pc_MovePortal(int nPortalIndex, unsigned __int16 *pConsumeSerial)
{
  unsigned __int8 retCode = 0;
  unsigned __int8 mapIndex = 255;
  unsigned __int8 linkPortalIndex = 255;
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
  // Yorozuya fix implementation (non-IDA): block forced portal packets while in battle mode.
  else if (Is_Battle_Mode())
  {
    retCode = 4;
  }
  else if (GetCurSecNum() == -1 || m_bMapLoading)
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
    do
    {
      portal = m_pCurMap->GetPortal(nPortalIndex);
      if (!portal)
      {
        retCode = 5;
        break;
      }

      if (!m_byUserDgr)
      {
        _portal_fld *portalRec = portal->m_pPortalRec;
        const int raceSexCode = m_Param.GetRaceSexCode();
        if (portalRec->m_strUseRace[raceSexCode] == '0')
        {
          retCode = 6;
          break;
        }

        const unsigned int lineIndex = portal->m_pDumPos->m_wLineIndex;
        CExtDummy *dummy = &m_pCurMap->m_Dummy;
        if (!dummy->IsInBBox(lineIndex, m_fCurPos))
        {
          retCode = 7;
          break;
        }
      }

      if (!portal->m_pPortalRec)
      {
        retCode = 8;
        break;
      }

      intoMap = g_MapOper.GetMap(portal->m_pPortalRec->m_strLinkMapCode);
      if (!intoMap)
      {
        retCode = 8;
        break;
      }

      mapIndex = static_cast<unsigned __int8>(intoMap->m_pMapSet->m_dwIndex);
      if (m_pDHChannel)
      {
        if (m_pDHChannel->m_pQuestSetup->pUseMap != intoMap)
        {
          retCode = 15;
          break;
        }
        if (m_pDHChannel->GetLeaderSerial() != m_dwObjSerial)
        {
          retCode = 16;
          break;
        }
        if (!m_pDHChannel->IsMoveNextMission(nPortalIndex))
        {
          retCode = 17;
          break;
        }
        if (!m_pDHChannel->IsAllMemberNearPosition(portal->m_pDumPos->m_fCenterPos, 50))
        {
          retCode = 18;
          break;
        }
        layerIndex = m_pDHChannel->m_wLayerIndex;
      }
      else
      {
        const int raceCode = m_Param.GetRaceCode();
        if (intoMap == g_TransportShip[raceCode].m_pLinkShipMap)
        {
          CTransportShip *ship = &g_TransportShip[raceCode];
          if (!ship->m_bAnchor)
          {
            retCode = 9;
            break;
          }
          if (ship->GetMapCurDirect() != m_pCurMap)
          {
            retCode = 11;
            break;
          }

          const int level = static_cast<int>(GetLevel());
          const int rideLimit = ship->GetRideLimLevel();
          if (level < rideLimit)
          {
            retCode = 14;
            break;
          }

          if (ship->GetRideUpLimLevel() != -1)
          {
            const int upperLevel = static_cast<int>(GetLevel());
            const int rideUpLimit = ship->GetRideUpLimLevel();
            if (upperLevel > rideUpLimit)
            {
              retCode = 14;
              break;
            }
          }

          bool hasTicket = false;
          for (int j = 0; j < m_Param.m_dbInven.m_nUsedNum; ++j)
          {
            _STORAGE_LIST::_db_con *item = &m_Param.m_dbInven.m_pStorageList[j];
            if (item->m_bLoad && item->m_byTableCode == 28)
            {
              auto *ticketRecord = reinterpret_cast<_TicketItem_fld *>(
                g_Main.m_tblItemData[28].GetRecord(item->m_wItemIndex));
              if (ship->GetCurRideShipThisTicket(ticketRecord))
              {
                hasTicket = true;
                break;
              }
            }
          }

          if (!hasTicket)
          {
            retCode = 12;
            break;
          }
          layerIndex = static_cast<unsigned __int16>(raceCode);
        }
        else if (m_pCurMap == g_TransportShip[raceCode].m_pLinkShipMap)
        {
          CTransportShip *ship = &g_TransportShip[raceCode];
          if (!ship->m_bAnchor)
          {
            retCode = 9;
            break;
          }
          if (ship->GetMapCurDirect() != intoMap)
          {
            retCode = 11;
            break;
          }
          if (nPortalIndex != 3 * ship->m_byDirect + raceCode)
          {
            retCode = 5;
            break;
          }
        }
        else if (!std::strcmp(intoMap->m_pMapSet->m_strCode, "Cauldron01") && m_Param.GetLevel() < 40)
        {
          retCode = 14;
          break;
        }
      }

      linkPortal = intoMap->GetPortal(portal->m_pPortalRec->m_strLinkPortalCode);
      if (!linkPortal)
      {
        retCode = 8;
        break;
      }

      linkPortalIndex = static_cast<unsigned __int8>(linkPortal->m_pPortalRec->m_dwIndex);
      if (!intoMap->GetRandPosInDummy(linkPortal->m_pDumPos, newPos, 1))
      {
        retCode = 8;
        break;
      }
      if (m_pDHChannel && !m_pDHChannel->SearchMissionFromPos(newPos))
      {
        retCode = 8;
        break;
      }

      _portal_fld *portalRec = portal->m_pPortalRec;
      if (portalRec->m_bNeedConCheck)
      {
        _STORAGE_LIST::_db_con *consumeItems[3]{};
        int consumeCount[3]{};
        bool overlap[3]{};
        const int level = static_cast<int>(GetLevel());
        if (portalRec->m_nUpLevelLim > -1)
        {
          if (level < portalRec->m_nNeedChrLevel || level >= portalRec->m_nUpLevelLim)
          {
            retCode = 21;
            break;
          }
        }
        else if (level < portalRec->m_nNeedChrLevel)
        {
          retCode = 21;
          break;
        }

        if (!GetUseConsumeItem(portalRec->m_ConsumeItemList, pConsumeSerial, consumeItems, consumeCount, overlap))
        {
          retCode = 20;
          break;
        }
        DeleteUseConsumeItem(consumeItems, consumeCount, overlap);
      }

      if (m_Param.m_pGuild)
      {
        const int raceCode = m_Param.GetRaceCode();
        CGuildRoomSystem *guildRoom = CGuildRoomSystem::GetInstance();
        if (m_pCurMap == guildRoom->GetMapData(raceCode, 0)
          || (guildRoom = CGuildRoomSystem::GetInstance(),
              m_pCurMap == guildRoom->GetMapData(m_Param.GetRaceCode(), 1u)))
        {
          CGuildRoomSystem *roomSystem = CGuildRoomSystem::GetInstance();
          CGuild *guild = m_Param.m_pGuild;
          roomSystem->RoomOut(guild->m_dwSerial, m_ObjID.m_wIndex, m_pUserDB->m_dwSerial);
        }
      }

      if (Major_Sette_Mine_Elan_Map)
      {
        if (!std::strcmp(intoMap->m_pMapSet->m_strCode, "resources")
          || !std::strcmp(intoMap->m_pMapSet->m_strCode, "Sette")
          || !std::strcmp(intoMap->m_pMapSet->m_strCode, "elan")
          || !std::strcmp(intoMap->m_pMapSet->m_strCode, "NeutralA")
          || !std::strcmp(intoMap->m_pMapSet->m_strCode, "NeutralB")
          || !std::strcmp(intoMap->m_pMapSet->m_strCode, "NeutralC"))
        {
          retCode = 0;
        }
        else
        {
          retCode = 8;
        }
      }
    } while (false);
  }

  if (!retCode)
  {
    OutOfMap(intoMap, layerIndex, 1u, newPos);
    if (m_pDHChannel)
    {
      if (m_pDHChannel->GotoNextMissionByPosition(newPos))
      {
        m_pDHChannel->NextMissionOtherQuester(this, m_pDHChannel->m_MissionMgr.pCurMssionPtr);
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
  _other_map_out_zocl msg{};
  msg.byMapOutType = static_cast<char>(byMapOutCode);
  msg.dwSerial = m_dwObjSerial;
  msg.byNextMap = static_cast<char>(byNextMapCode);

  unsigned __int8 type[2]{};
  type[0] = 3;
  type[1] = 39;
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(sizeof(msg)), false);
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
  _map_env_code_inform_zocl msg{};
  msg.byMapCode = byMapCode;
  msg.dwEnvCode = dwMapEnvCode;

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
  msg.long_time = static_cast<int>(std::mktime(&localTime));

  unsigned __int8 type[2]{};
  type[0] = 8;
  type[1] = 5;
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_MineCancle()
{
  unsigned __int8 type[2]{};
  type[0] = 14;
  type[1] = 4;

  _mine_cancle_result_zocl msg{};
  msg.sDum = 0;
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_HonorGuildMark(char byRank)
{
  _guild_honorguild_mark_zocl msg{};
  msg.byHonorRank = byRank;
  msg.dwSerial = m_dwObjSerial;

  unsigned __int8 type[2]{};
  type[0] = 27;
  type[1] = 122;
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(sizeof(msg)), true);
}

void CPlayer::SendMsg_DeleteStorageInform(char byStorageCode, unsigned __int16 wSerial)
{
  _delete_storage_inform_zocl msg{};

  msg.byStorageCode = byStorageCode;
  msg.wSerial = wSerial;

  unsigned __int8 type[2]{};
  type[0] = 7;
  type[1] = 23;
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 3);
}

void CPlayer::SendMsg_BuddhaEventMsg(char byErrorCode)
{
  _notice_buddha_event_msg_zocl msg{};
  msg.byErrorCode = byErrorCode;

  unsigned __int8 type[2]{59, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
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
  SetCurSecNum(-1);

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
    std::memcpy(m_fCurPos, pfStartPos, sizeof(m_fCurPos));
  }

  if (CGameObject::s_pSelectObject == this)
  {
    CGameObject::s_pSelectObject = nullptr;
  }

  if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_14))
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
  std::memcpy(m_fTarPos, m_fCurPos, sizeof(m_fTarPos));
  m_bMove = false;
  std::memcpy(m_fLastRecvPos, m_fCurPos, sizeof(m_fLastRecvPos));
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

  if (byStorageCode == 1 || byStorageCode == 2)
  {
    UpdateActiveSetItemEffects();
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

    if (pkItem->m_byTableCode == 6 && m_EP.GetEff_State(EFF_STATE_UNKNOWN_14))
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

  if (byStorageCode == 1 || byStorageCode == 2)
  {
    UpdateActiveSetItemEffects();
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

  if (pkItem->m_byTableCode == 15 && nAlter > 0 && pkItem->m_dwDur + nAlter > 16777215)
  {
    alter = static_cast<int>(16777215 - pkItem->m_dwDur);
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
  std::strcpy(m_bufShapeAll.wszName, charName);
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
    std::memcpy(m_bufShapeAll.byUnitPartIndex, m_pUsingUnit->byPart, sizeof(m_bufShapeAll.byUnitPartIndex));
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
    std::memcpy(m_bufSpapePart.byUnitPartIndex, m_pUsingUnit->byPart, sizeof(m_bufSpapePart.byUnitPartIndex));
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

  _other_shape_change_zocl msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  msg.wEquipVer = GetVisualVer();
  msg.byPartIndex = byPart;

  if (m_Param.m_dbEquip.m_pStorageList[byPart].m_bLoad)
  {
    msg.ModelPerPart.wPartIndex = m_Param.m_dbEquip.m_pStorageList[byPart].m_wItemIndex;
    msg.ModelPerPart.byLv = GetItemUpgedLv(m_Param.m_dbEquip.m_pStorageList[byPart].m_dwLv);
  }
  else if (byPart >= 5u)
  {
    msg.ModelPerPart.wPartIndex = static_cast<unsigned __int16>(-1);
  }
  else
  {
    msg.ModelPerPart.wPartIndex = m_Param.m_dbChar.m_byDftPart[byPart];
    msg.ModelPerPart.byLv = 0;
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
  _d_trade_cancle_inform_zocl msg{};
  msg.dummy[0] = 0;
  unsigned __int8 type[2] = {18, 9};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_DTradeCloseInform(char byCloseCode)
{
  _d_trade_close_inform_zocl msg{};
  msg.byCloseCode = byCloseCode;

  unsigned __int8 type[2] = {18, 26};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::NetClose(bool bMoveOutLobby)
{
  if (m_byPatriarchAppointPropose != 255)
  {
    char responseData = 1;
    PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
    processor->Doit(_eRespAppoint, this, &responseData);
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

    if (m_Param.m_pGuild && m_EP.GetEff_Have(EFF_HAVE_HIDE_NAME) <= 0.0)
    {
      m_Param.m_pGuild->LogoffMember(m_dwObjSerial);
      m_Param.m_pGuild->SendMsg_GuildMemberLogoff(m_dwObjSerial);
    }
    if (m_Param.m_pApplyGuild && m_EP.GetEff_Have(EFF_HAVE_HIDE_NAME) <= 0.0)
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
          std::memcpy(m_fCurPos, bindPos, sizeof(m_fCurPos));
        }
      }
    }
    if (m_pDHChannel)
    {
      _dh_player_mgr::_pos outPos;
      m_pDHChannel->ClearMember(this, m_bOper, &outPos);
      m_pCurMap = outPos.pMap;
      std::memcpy(m_fCurPos, outPos.fPos, sizeof(m_fCurPos));
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

    if (m_EP.GetEff_Have(EFF_HAVE_HIDE_NAME) <= 0.0)
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

  _animus_return_result_zocl msg{};
  msg.byResultCode = byRetCode;
  msg.wAnimusItemSerial = wAnimusItemSerial;
  msg.byReturnType = byReturnType;

  unsigned __int8 type[2] = {22, 4};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
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

  _alter_item_dur_zocl msg{};
  msg.byStorageCode = byStorageCode;
  msg.wItemSerial = wItemSerial;
  msg.dwDur = dwDur;

  unsigned __int8 type[2] = {7, 25};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_AlterTowerHP(unsigned __int16 wItemSerial, unsigned __int16 wLeftHP)
{

  _alter_tower_hp_zocl msg{};
  msg.wItemSerial = wItemSerial;
  msg.wLeftHP = wLeftHP;

  unsigned __int8 type[2] = {17, 22};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
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

void CPlayer::_TrapDestroy(CTrap *pTrap, unsigned __int8 /*byDestroyCode*/)
{
  if (m_pmTrp.PopItem(pTrap->m_dwObjSerial))
  {
    SendMsg_MadeTrapNumInform(m_pmTrp.m_nCount);
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
              std::memcpy(waitNode->fPos, tower->m_fCurPos, sizeof(waitNode->fPos));
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

  _player_destroy_zocl msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  msg.byState = 0;

  unsigned __int8 type[2] = {3, 22};
  CircleReport(type, reinterpret_cast<char *>(&msg), 7, false);
}

void CPlayer::SendMsg_NewViewOther(unsigned __int8 byViewType)
{

  _other_new_view_zocl msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  msg.wEquipVer = GetVisualVer();
  FloatToShort(m_fCurPos, msg.zPos, 3);
  msg.byRaceCode = static_cast<unsigned __int8>(m_Param.GetRaceSexCode());
  msg.byViewType = byViewType;
  msg.dwStateFlag = GetStateFlag();
  msg.wLastEffectCode = m_wLastContEffect;
  msg.byColor = m_byGuildBattleColorInx;

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

float CPlayer::GetMoveSpeed()
{
  float moveSpeed = 0.0f;
  if (IsRidingUnit())
  {
    _UnitFrame_fld *frameRecord = reinterpret_cast<_UnitFrame_fld *>(g_Main.m_tblUnitFrame.GetRecord(m_pUsingUnit->byFrame));
    if (frameRecord)
    {
      moveSpeed = frameRecord->m_fMoveRate_Seed;
    }

    // Yorozuya move-speed fix (non-IDA): IDA parity overwrites the MAU/unit move speed with the
    // last part record, which produces the wrong speed. Use frame seed plus equipped part speed.
    switch (m_byMoveType)
    {
      case 0:
      {
        for (int j = 0; j < 6; ++j)
        {
          _UnitPart_fld *partRecord =
            reinterpret_cast<_UnitPart_fld *>(g_Main.m_tblUnitPart[j].GetRecord(m_pUsingUnit->byPart[j]));
          if (partRecord)
          {
            moveSpeed += partRecord->m_fMoveSpdRev;
          }
        }
        break;
      }
      case 1:
      {
        _UnitPart_fld *boosterRecord =
          reinterpret_cast<_UnitPart_fld *>(g_Main.m_tblUnitPart[5].GetRecord(m_pUsingUnit->byPart[5]));
        if (boosterRecord)
        {
          moveSpeed += boosterRecord->m_fBstSpd;
        }
        break;
      }
      default:
        break;
    }
    return moveSpeed;
  }

  const _player_fld *playerRecord = static_cast<const _player_fld *>(m_pRecordSet);
  if (!playerRecord)
  {
    return 0.0f;
  }

  if (m_byMoveType == 1)
  {
    if (m_bInGuildBattle && m_bTakeGravityStone)
    {
      return 3.0f + m_EP.GetEff_Plus(EFF_PLUS_MOVE_RUN_SPEED);
    }
    return playerRecord->m_fMoveRunRate + m_EP.GetEff_Plus(EFF_PLUS_MOVE_RUN_SPEED);
  }

  if (m_byMoveType == 2)
  {
    if (m_bInGuildBattle && m_bTakeGravityStone)
    {
      return 3.0f;
    }
    return playerRecord->m_fMoveRunRate + EquipItemSFAgent.GetBoosterAddSpeed();
  }

  if (m_byMoveType == 0)
  {
    return playerRecord->m_fMoveWalkRate + m_EP.GetEff_Plus(EFF_PLUS_MOVE_RUN_SPEED);
  }
  return 0.0f;
}

__int64 CPlayer::GetMaxHP()
{
  const float baseMax = static_cast<float>(m_nMaxPoint[0]);
  const float rate = m_EP.GetEff_Rate(EFF_RATE_MAX_HP);
  const int maxHp = static_cast<int>(baseMax * rate);
  if (maxHp <= 0)
  {
    return 1;
  }
  return maxHp;
}

unsigned __int16 CPlayer::CalcCurHPRate()
{
  if (IsRidingUnit())
  {
    int maxGauge = 10000;
    _UnitFrame_fld *frameRecord = reinterpret_cast<_UnitFrame_fld *>(g_Main.m_tblUnitFrame.GetRecord(m_pUsingUnit->byFrame));
    if (frameRecord && frameRecord->m_nUnit_HP > 0)
    {
      maxGauge = frameRecord->m_nUnit_HP;
    }
    return static_cast<unsigned int>(
      static_cast<int>((static_cast<float>(static_cast<int>(m_pUsingUnit->dwGauge)) / static_cast<float>(maxGauge)) * 10000.0f));
  }

  const float hp = static_cast<float>(GetHP());
  const int maxHp = static_cast<int>(GetMaxHP());
  return static_cast<unsigned int>(static_cast<int>((hp / static_cast<float>(maxHp)) * 10000.0f));
}

__int64 CPlayer::CalcCurFPRate()
{
  const int scaledFp = 10000 * GetFP();
  const int maxFp = GetMaxFP();
  return static_cast<unsigned int>(scaledFp / maxFp);
}

__int64 CPlayer::CalcCurSPRate()
{
  const int scaledSp = 10000 * GetSP();
  const int maxSp = GetMaxSP();
  return static_cast<unsigned int>(scaledSp / maxSp);
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
  if (m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_22) > 0.0)
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
  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE))
  {
    state |= bit << 18;
  }
  if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_21))
  {
    state |= bit << 19;
  }
  if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_22))
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
  if (m_EP.GetEff_Have(EFF_HAVE_HIDE_NAME) > 0.0)
  {
    state |= bit << 44;
  }
  if (m_EP.GetEff_Have(EFF_HAVE_POTION_DELAY_BYPASS) > 0.0)
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
  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
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
    // Yorozuya fix implementation (non-IDA): refresh view when transparency-related flags are cleared.
    const unsigned __int64 newState = m_dwLastState;
    bool breakTransparent = false;
    const int transparentBits[] = {2, 9, 51, 52};
    for (int index = 0; index < 4; ++index)
    {
      const unsigned __int64 bit = 1ULL << transparentBits[index];
      if ((oldState & bit) && !(newState & bit))
      {
        breakTransparent = true;
        break;
      }
    }
    if (breakTransparent)
    {
      SendMsg_NewViewOther(3);
    }
    SendMsg_StateInform(m_dwLastState);
  }
}

void CPlayer::SendMsg_StateInform(unsigned __int64 dwStateFlag)
{
  _state_inform_zocl msg{};

  msg.dwSerial = m_dwObjSerial;
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
  if (nowMin - static_cast<int>(m_pUserDB->m_AvatorData.dbAvator.m_dwPunishment[byType]) >= 4320)
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

void CPlayer::TakeGravityStone()
{
  m_EP.SetEff_Plus(EFF_PLUS_AVOID_RATE, -30.0, true);
  ApplyEquipItemEffect(12, false);
  SF_RemoveAllContHelp_Once(this, 0.0f);
  m_bTakeGravityStone = true;
  BreakCloakBooster();
}

void CPlayer::ClearGravityStone()
{
  m_bTakeGravityStone = false;
  m_EP.SetEff_Plus(EFF_PLUS_AVOID_RATE, -30.0, 0);
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
    std::memcpy(param.fStartPos, curPos, sizeof(param.fStartPos));
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
            -1,
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

  _reward_add_item_zocl msg{};
  msg.byTableCode = pItem->m_byTableCode;
  msg.wItemIndex = pItem->m_wItemIndex;
  msg.dwDur = pItem->m_dwDur;
  msg.dwLv = pItem->m_dwLv;
  msg.wItemSerial = pItem->m_wSerial;
  msg.byReason = byReason;
  msg.byCsMethod = pItem->m_byCsMethod;
  msg.dwT = pItem->m_dwT;

  unsigned __int8 byType[2] = {11, 9};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, byType, reinterpret_cast<char *>(&msg), sizeof(msg));
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

bool CPlayer::SetTarPos(float *fTarPos, bool /*bColl*/)
{
  return CCharacter::SetTarPos(fTarPos, false);
}

bool CPlayer::RobbedHP(CCharacter *pDst, int nDecHP)
{
  if (nDecHP >= static_cast<int>(GetHP()))
  {
    return false;
  }

  SetDamage(nDecHP, pDst, static_cast<int>(pDst->GetLevel()), false, -1, 0, true);
  SendMsg_RobedHP(pDst, static_cast<unsigned __int16>(nDecHP));
  return true;
}

char CPlayer::SF_HFSInc_Once(CPlayer *pDstObj)
{
  bool bValidDst = false;
  if (!pDstObj->m_ObjID.m_byID)
  {
    bValidDst = !pDstObj->m_bCorpse;
  }
  if (!bValidDst)
  {
    return 0;
  }

  const int hp = static_cast<int>(pDstObj->GetHP());
  const int maxHp = static_cast<int>(pDstObj->GetMaxHP());
  const int hpGap = maxHp - hp;
  const int fp = pDstObj->GetFP();
  const int maxFp = pDstObj->GetMaxFP();
  const int fpGap = maxFp - fp;
  const int sp = pDstObj->GetSP();
  const int maxSp = pDstObj->GetMaxSP();
  const int spGap = maxSp - sp;

  if (static_cast<float>(hp) / static_cast<float>(maxHp) > 0.80000001f)
  {
    return 0;
  }

  if (hpGap > 0)
  {
    pDstObj->SetHP(maxHp, false);
    pDstObj->SendMsg_SetHPInform();
  }
  if (fpGap > 0)
  {
    pDstObj->SetFP(maxFp, false);
    pDstObj->SendMsg_SetFPInform();
  }
  if (spGap > 0)
  {
    pDstObj->SetSP(maxSp, false);
    pDstObj->SendMsg_SetSPInform();
  }
  return 1;
}

bool CPlayer::SF_Resurrect_Once(CCharacter *pDstObj)
{
  bool canResurrect = false;
  if (!pDstObj->m_ObjID.m_byID)
  {
    canResurrect = pDstObj->GetHP() == 0;
  }
  if (!canResurrect)
  {
    return false;
  }

  static_cast<CPlayer *>(pDstObj)->Resurrect();
  return true;
}

bool CPlayer::SF_MakePortalReturnBindPositionPartyMember(
  CCharacter *pDstObj,
  float /*fEffectValue*/,
  unsigned __int8 *byRet)
{
  if (pDstObj->m_ObjID.m_byID)
  {
    return false;
  }

  CReturnGateController *returnGateController = CReturnGateController::Instance();
  if (returnGateController->Open(static_cast<CPlayer *>(pDstObj)))
  {
    return true;
  }

  *byRet = static_cast<unsigned __int8>(-1);
  return false;
}

bool CPlayer::SF_OthersContHelpSFRemove_Once(float fEffectValue)
{
  int removedCount = 0;

  _pnt_rect rect{};
  m_pCurMap->GetRectInRadius(&rect, 2, static_cast<int>(GetCurSecNum()));
  for (int y = rect.nStarty; y < rect.nEndy; ++y)
  {
    for (int x = rect.nStartx; x < rect.nEndx; ++x)
    {
      _sec_info *secInfo = m_pCurMap->GetSecInfo();
      const unsigned int secIndex = secInfo->m_nSecNumW * y + x;
      CObjectList *sectorList = m_pCurMap->GetSectorListObj(m_wMapLayerIndex, secIndex);
      if (!sectorList)
      {
        continue;
      }

      _object_list_point *next = sectorList->m_Head.m_pNext;
      while (next != &sectorList->m_Tail)
      {
        CCharacter *target = static_cast<CCharacter *>(next->m_pItem);
        next = next->m_pNext;
        if (target == this || target->m_ObjID.m_byKind || target->m_ObjID.m_byID)
        {
          continue;
        }

        const int distance = static_cast<int>(GetSqrt(m_fCurPos, target->m_fCurPos));
        if (static_cast<float>(distance) > fEffectValue)
        {
          continue;
        }

        for (int slot = 0; slot < 8; ++slot)
        {
          if (target->m_SFCont[1][slot].m_bExist)
          {
            target->RemoveSFContEffect(1u, static_cast<unsigned __int16>(slot), false, false);
            ++removedCount;
          }
        }
        target->BreakStealth();
      }
    }
  }

  return removedCount > 0;
}

bool CPlayer::SF_OverHealing_Once(CCharacter *pDstObj, float fEffectValue)
{
  bool canApply = false;
  if (!pDstObj->m_ObjID.m_byID)
  {
    canApply = !pDstObj->m_bCorpse;
  }
  if (!canApply)
  {
    return false;
  }

  const int currentHp = static_cast<int>(pDstObj->GetHP());
  const int maxHp = static_cast<int>(pDstObj->GetMaxHP());
  const float overHealingAmount = static_cast<float>(maxHp) * fEffectValue;
  const float effectRate = pDstObj->m_EP.GetEff_Rate(EFF_RATE_HP_GAIN);
  const int addHp = static_cast<int>(overHealingAmount * effectRate);
  pDstObj->SetHP(addHp + currentHp, true);

  if (currentHp == pDstObj->GetHP())
  {
    return false;
  }

  pDstObj->SendMsg_SetHPInform();
  return true;
}

bool CPlayer::SF_ReleaseMonsterTarget(CCharacter *pDstObj, float /*fEffectValue*/)
{
  int releasedCount = 0;

  _pnt_rect rect{};
  pDstObj->m_pCurMap->GetRectInRadius(&rect, 1, static_cast<int>(GetCurSecNum()));
  for (int y = rect.nStarty; y < rect.nEndy; ++y)
  {
    for (int x = rect.nStartx; x < rect.nEndx; ++x)
    {
      _sec_info *secInfo = m_pCurMap->GetSecInfo();
      const unsigned int secIndex = secInfo->m_nSecNumW * y + x;
      CObjectList *sectorList = m_pCurMap->GetSectorListObj(m_wMapLayerIndex, secIndex);
      if (!sectorList)
      {
        continue;
      }

      _object_list_point *next = sectorList->m_Head.m_pNext;
      while (next != &sectorList->m_Tail)
      {
        CMonster *monster = static_cast<CMonster *>(next->m_pItem);
        next = next->m_pNext;
        if (monster == reinterpret_cast<CMonster *>(this) || monster->m_ObjID.m_byKind || monster->m_ObjID.m_byID != 1)
        {
          continue;
        }

        const int distance = static_cast<int>(GetSqrt(m_fCurPos, monster->m_fCurPos));
        if (distance <= 200 && monster->ConvertTargetPlayer(nullptr))
        {
          ++releasedCount;
        }
      }
    }
  }

  return releasedCount > 0;
}

bool CPlayer::SF_RemoveAllContHelp_Once(CCharacter *pDstObj, float /*fEffectValue*/)
{
  int removedCount = 0;
  for (int contCode = 0; contCode < 2; ++contCode)
  {
    for (int slot = 0; slot < 8; ++slot)
    {
      _sf_continous *contEffect = &pDstObj->m_SFCont[contCode][slot];
      if (!contEffect->m_bExist)
      {
        continue;
      }

      _base_fld *effect17 = g_Main.m_tblEffectData[3].GetRecord("17");
      if (effect17)
      {
        if (pDstObj->m_ObjID.m_byID)
        {
          return false;
        }

        const bool hasAuraGuard = static_cast<unsigned __int8>(pDstObj->m_SFContAura[0][5].m_wDurSec >> 8) != 0;
        const bool isProtectedEffect = hasAuraGuard && contEffect->m_byEffectCode == 3
                                    && contEffect->m_wEffectIndex == effect17->m_dwIndex;
        if (isProtectedEffect)
        {
          continue;
        }
      }

      pDstObj->RemoveSFContEffect(
        static_cast<unsigned __int8>(contCode),
        static_cast<unsigned __int16>(slot),
        false,
        false);
      ++removedCount;
    }
  }

  return removedCount > 0;
}

bool CPlayer::SF_AllContDamageRemove_Once(CCharacter *pDstObj)
{
  int removedCount = 0;
  for (int index = 0; index < 8; ++index)
  {
    _sf_continous *contEffect = &pDstObj->m_SFCont[0][index];
    if (!contEffect->m_bExist)
    {
      continue;
    }

    _base_fld *effect17 = g_Main.m_tblEffectData[3].GetRecord("17");
    const bool afterEffectGuard = static_cast<unsigned __int8>(pDstObj->m_SFContAura[0][5].m_wDurSec >> 8) != 0;
    const bool keepEffect =
      effect17
      && afterEffectGuard
      && contEffect->m_byEffectCode == 3
      && contEffect->m_wEffectIndex == effect17->m_dwIndex;
    if (keepEffect)
    {
      continue;
    }

    pDstObj->RemoveSFContEffect(0, static_cast<unsigned __int16>(index), false, false);
    ++removedCount;
  }

  return removedCount > 0;
}

bool CPlayer::SF_AllContDamageForceRemove_Once(CCharacter *pDstObj)
{
  int removedCount = 0;
  for (int index = 0; index < 8; ++index)
  {
    _sf_continous *contEffect = &pDstObj->m_SFCont[0][index];
    if (!contEffect->m_bExist || contEffect->m_byEffectCode != 1)
    {
      continue;
    }

    _base_fld *effect17 = g_Main.m_tblEffectData[3].GetRecord("17");
    const bool afterEffectGuard = static_cast<unsigned __int8>(pDstObj->m_SFContAura[0][5].m_wDurSec >> 8) != 0;
    const bool keepEffect =
      effect17
      && afterEffectGuard
      && contEffect->m_byEffectCode == 3
      && contEffect->m_wEffectIndex == effect17->m_dwIndex;
    if (keepEffect)
    {
      continue;
    }

    pDstObj->RemoveSFContEffect(0, static_cast<unsigned __int16>(index), false, false);
    ++removedCount;
  }

  return removedCount > 0;
}

bool CPlayer::SF_AllContHelpForceRemove_Once(CCharacter *pDstObj)
{
  const float roll = static_cast<float>(rand() % 100);
  if (pDstObj->m_EP.GetEff_Plus(EFF_PLUS_CONT_HELP_REMOVE_RESIST) > roll)
  {
    return false;
  }

  int removedCount = 0;
  for (int index = 0; index < 8; ++index)
  {
    _sf_continous *contEffect = &pDstObj->m_SFCont[1][index];
    if (contEffect->m_bExist && contEffect->m_byEffectCode == 1)
    {
      pDstObj->RemoveSFContEffect(1u, static_cast<unsigned __int16>(index), false, false);
      ++removedCount;
    }
  }

  return removedCount > 0;
}

bool CPlayer::SF_AllContHelpSkillRemove_Once(CCharacter *pDstObj)
{
  const float roll = static_cast<float>(rand() % 100);
  if (pDstObj->m_EP.GetEff_Plus(EFF_PLUS_CONT_HELP_REMOVE_RESIST) > roll)
  {
    return false;
  }

  int removedCount = 0;
  for (int index = 0; index < 8; ++index)
  {
    _sf_continous *contEffect = &pDstObj->m_SFCont[1][index];
    if (contEffect->m_bExist && !contEffect->m_byEffectCode)
    {
      pDstObj->RemoveSFContEffect(1u, static_cast<unsigned __int16>(index), false, false);
      ++removedCount;
    }
  }

  return removedCount > 0;
}

bool CPlayer::SF_AttHPtoDstFP_Once(CCharacter *pDstObj, float fEffectValue)
{
  if (!GetHP() || pDstObj->m_ObjID.m_byID)
  {
    return false;
  }

  CPlayer *dstPlayer = static_cast<CPlayer *>(pDstObj);
  const int curFp = dstPlayer->GetFP();
  const int maxFp = dstPlayer->GetMaxFP();
  const float effectRate = pDstObj->m_EP.GetEff_Rate(EFF_RATE_FP_GAIN);
  const int addFp = static_cast<int>(static_cast<float>(maxFp) * (fEffectValue * effectRate));
  dstPlayer->SetFP(curFp + addFp, false);

  if ((static_cast<float>(curFp) / static_cast<float>(maxFp)) > 0.80000001f)
  {
    return false;
  }

  dstPlayer->SendMsg_SetFPInform();
  return true;
}

bool CPlayer::SF_ContDamageTimeInc_Once(CCharacter *pDstObj, float fEffectValue)
{
  int thresholdExceededCount = 0;
  int updatedCount = 0;
  const unsigned int currentTime = _sf_continous::GetSFContCurTime();

  for (int index = 0; index < 8; ++index)
  {
    _sf_continous *contEffect = &pDstObj->m_SFCont[0][index];
    if (!contEffect->m_bExist)
    {
      continue;
    }

    _base_fld *record = g_Main.m_tblEffectData[contEffect->m_byEffectCode].GetRecord(contEffect->m_wEffectIndex);
    int baseDuration = 0;
    const unsigned int lvIndex = static_cast<unsigned int>(contEffect->m_byLv) - 1;
    if (contEffect->m_byEffectCode && contEffect->m_byEffectCode != 2)
    {
      if (contEffect->m_byEffectCode == 1)
      {
        baseDuration = reinterpret_cast<_force_fld *>(record)->m_nContEffectSec[lvIndex];
      }
    }
    else
    {
      baseDuration = reinterpret_cast<_skill_fld *>(record)->m_nContEffectSec[lvIndex];
    }

    const unsigned int remainingTime = contEffect->m_wDurSec - (currentTime - contEffect->m_dwStartSec);
    if (baseDuration > static_cast<int>(remainingTime))
    {
      ++thresholdExceededCount;
    }

    unsigned __int16 newDuration = static_cast<unsigned __int16>(
      static_cast<int>(static_cast<float>(static_cast<int>(remainingTime)) + static_cast<float>(baseDuration) * fEffectValue));
    if (newDuration > static_cast<unsigned int>(2 * baseDuration))
    {
      newDuration = static_cast<unsigned __int16>(2 * baseDuration);
    }

    pDstObj->AlterContDurSec(0, static_cast<unsigned __int16>(index), currentTime, newDuration);
    ++updatedCount;
  }

  if (updatedCount > 0 && !pDstObj->m_ObjID.m_byID)
  {
    static_cast<CPlayer *>(pDstObj)->SendMsg_AlterContEffectTime(0);
  }
  return thresholdExceededCount > 0;
}

bool CPlayer::SF_ContHelpTimeInc_Once(CCharacter *pDstObj, float fEffectValue)
{
  int thresholdExceededCount = 0;
  int updatedCount = 0;
  const unsigned int currentTime = _sf_continous::GetSFContCurTime();

  for (int index = 0; index < 8; ++index)
  {
    _sf_continous *contEffect = &pDstObj->m_SFCont[1][index];
    if (!contEffect->m_bExist || contEffect->m_byEffectCode != 1)
    {
      continue;
    }

    _base_fld *record = g_Main.m_tblEffectData[contEffect->m_byEffectCode].GetRecord(contEffect->m_wEffectIndex);
    int baseDuration = 0;
    const unsigned int lvIndex = static_cast<unsigned int>(contEffect->m_byLv) - 1;
    if (contEffect->m_byEffectCode && contEffect->m_byEffectCode != 2)
    {
      if (contEffect->m_byEffectCode == 1)
      {
        baseDuration = reinterpret_cast<_force_fld *>(record)->m_nContEffectSec[lvIndex];
      }
    }
    else
    {
      baseDuration = reinterpret_cast<_skill_fld *>(record)->m_nContEffectSec[lvIndex];
    }

    const unsigned int remainingTime = contEffect->m_wDurSec - (currentTime - contEffect->m_dwStartSec);
    if (baseDuration >= static_cast<int>(remainingTime))
    {
      ++thresholdExceededCount;
    }

    unsigned __int16 newDuration = static_cast<unsigned __int16>(
      static_cast<int>(static_cast<float>(static_cast<int>(remainingTime)) + static_cast<float>(baseDuration) * fEffectValue));
    if (newDuration > static_cast<unsigned int>(2 * baseDuration))
    {
      newDuration = static_cast<unsigned __int16>(2 * baseDuration);
    }

    pDstObj->AlterContDurSec(1u, static_cast<unsigned __int16>(index), currentTime, newDuration);
    ++updatedCount;
  }

  if (updatedCount > 0 && !pDstObj->m_ObjID.m_byID)
  {
    static_cast<CPlayer *>(pDstObj)->SendMsg_AlterContEffectTime(1u);
  }
  return thresholdExceededCount > 0;
}

bool CPlayer::SF_ConvertMonsterTarget(CCharacter *pDstObj, float /*fEffectValue*/)
{
  int convertedCount = 0;
  if (!pDstObj || !pDstObj->m_pCurMap)
  {
    return false;
  }

  _pnt_rect rect{};
  pDstObj->m_pCurMap->GetRectInRadius(&rect, 1, static_cast<int>(GetCurSecNum()));
  for (int y = rect.nStarty; y < rect.nEndy; ++y)
  {
    for (int x = rect.nStartx; x < rect.nEndx; ++x)
    {
      _sec_info *secInfo = this->m_pCurMap->GetSecInfo();
      const unsigned int secIndex = secInfo->m_nSecNumW * y + x;
      CObjectList *sectorList = this->m_pCurMap->GetSectorListObj(this->m_wMapLayerIndex, secIndex);
      if (!sectorList)
      {
        continue;
      }

      _object_list_point *next = sectorList->m_Head.m_pNext;
      while (next != &sectorList->m_Tail)
      {
        CMonster *monster = static_cast<CMonster *>(next->m_pItem);
        next = next->m_pNext;
        if (!monster->m_ObjID.m_byKind && monster->m_ObjID.m_byID == 1)
        {
          const int distance = static_cast<int>(GetSqrt(this->m_fCurPos, monster->m_fCurPos));
          if (distance <= 200 && monster->ConvertTargetPlayer(this))
          {
            ++convertedCount;
          }
        }
      }
    }
  }

  return convertedCount > 0;
}

bool CPlayer::SF_ConvertTargetDest(CCharacter *pDstObj, float fEffectValue)
{
  return pDstObj->FixTargetWhile(this, static_cast<unsigned int>(static_cast<int>(fEffectValue)));
}

bool CPlayer::SF_DamageAndStun(CCharacter *pDstObj, float /*fEffectValue*/)
{
  pDstObj->SetDamage(0, this, static_cast<int>(GetLevel()), true, -1, 0, true);
  return true;
}

bool CPlayer::SF_FPDec(CCharacter *pDstObj, float fEffectValue)
{
  if (pDstObj->m_ObjID.m_byID)
  {
    return false;
  }

  CPlayer *dstPlayer = static_cast<CPlayer *>(pDstObj);
  const int fp = dstPlayer->GetFP();
  if (fp < 1)
  {
    return true;
  }

  int newFp = static_cast<int>(static_cast<float>(fp) * fEffectValue);
  if (newFp <= 1)
  {
    newFp = 1;
  }
  dstPlayer->SetFP(newFp, false);
  dstPlayer->SendMsg_Recover();
  return true;
}

bool CPlayer::SF_ReturnBindPosition(CCharacter *pDstObj, float /*fEffectValue*/)
{
  if (pDstObj->m_ObjID.m_byID)
  {
    return false;
  }

  CPlayer *dstPlayer = static_cast<CPlayer *>(pDstObj);
  float bindPos[3]{};
  CMapData *intoMap = dstPlayer->GetBindMap(bindPos, true);
  if (!intoMap)
  {
    return false;
  }

  dstPlayer->OutOfMap(intoMap, 0, 3u, bindPos);
  dstPlayer->SendMsg_MovePortal(static_cast<char>(intoMap->m_pMapSet->m_dwIndex), bindPos, 2u);
  return true;
}

bool CPlayer::SF_SelfDestruction(CCharacter * /*pDstObj*/, float fEffectValue)
{
  if (GetStealth(true))
  {
    return false;
  }

  m_dwSelfDestructionTime = GetLoopTime() + 5000;
  m_fSelfDestructionDamage = fEffectValue;
  return true;
}

bool CPlayer::SF_SkillContHelpTimeInc_Once(CCharacter *pDstObj, float fEffectValue)
{
  int thresholdExceededCount = 0;
  int updatedCount = 0;
  const unsigned int currentTime = _sf_continous::GetSFContCurTime();

  for (int index = 0; index < 8; ++index)
  {
    _sf_continous *contEffect = &pDstObj->m_SFCont[1][index];
    if (!contEffect->m_bExist || contEffect->m_byEffectCode)
    {
      continue;
    }

    _base_fld *record = g_Main.m_tblEffectData[contEffect->m_byEffectCode].GetRecord(contEffect->m_wEffectIndex);
    int baseDuration = 0;
    const unsigned int lvIndex = static_cast<unsigned int>(contEffect->m_byLv) - 1;
    if (contEffect->m_byEffectCode)
    {
      if (contEffect->m_byEffectCode == 1)
      {
        baseDuration = reinterpret_cast<_force_fld *>(record)->m_nContEffectSec[lvIndex];
      }
    }
    else
    {
      baseDuration = reinterpret_cast<_skill_fld *>(record)->m_nContEffectSec[lvIndex];
    }

    int remainingDuration = static_cast<int>(contEffect->m_wDurSec) - static_cast<int>(currentTime - contEffect->m_dwStartSec);
    if (remainingDuration <= 0)
    {
      continue;
    }

    if (baseDuration >= remainingDuration)
    {
      ++thresholdExceededCount;
    }

    unsigned __int16 newDuration = static_cast<unsigned __int16>(
      static_cast<int>(static_cast<float>(remainingDuration) + static_cast<float>(baseDuration) * fEffectValue));
    if (newDuration > static_cast<unsigned int>(2 * baseDuration))
    {
      newDuration = static_cast<unsigned __int16>(2 * baseDuration);
    }

    pDstObj->AlterContDurSec(1u, static_cast<unsigned __int16>(index), currentTime, newDuration);
    ++updatedCount;
  }

  if (updatedCount > 0 && !pDstObj->m_ObjID.m_byID)
  {
    static_cast<CPlayer *>(pDstObj)->SendMsg_AlterContEffectTime(1u);
  }

  return thresholdExceededCount > 0;
}

bool CPlayer::SF_SPDec(CCharacter *pDstObj, float fEffectValue)
{
  if (pDstObj->m_ObjID.m_byID)
  {
    return false;
  }

  CPlayer *dstPlayer = static_cast<CPlayer *>(pDstObj);
  const int sp = dstPlayer->GetSP();
  if (sp < 1)
  {
    return true;
  }

  int newSp = static_cast<int>(static_cast<float>(sp) * fEffectValue);
  if (newSp <= 1)
  {
    newSp = 1;
  }

  dstPlayer->SetSP(newSp, false);
  dstPlayer->SendMsg_Recover();
  return true;
}

bool CPlayer::SF_STInc_Once(CCharacter *pDstObj, float fEffectValue)
{
  if (pDstObj->m_ObjID.m_byID)
  {
    return false;
  }

  CPlayer *dstPlayer = static_cast<CPlayer *>(pDstObj);
  const int currentSp = dstPlayer->GetSP();
  const int maxSp = dstPlayer->GetMaxSP();
  const float effectRate = dstPlayer->m_EP.GetEff_Rate(EFF_RATE_SP_GAIN);
  const int addSp = static_cast<int>(static_cast<float>(maxSp) * (fEffectValue * effectRate));
  dstPlayer->SetSP(addSp + currentSp, false);

  if (dstPlayer->GetSP() - currentSp < 20)
  {
    return false;
  }

  dstPlayer->SendMsg_SetSPInform();
  return true;
}

bool CPlayer::SF_Stun(CCharacter *pDstObj, float /*fEffectValue*/)
{
  if (!pDstObj->m_ObjID.m_byID)
  {
    CPlayer *dstPlayer = static_cast<CPlayer *>(pDstObj);
    if (dstPlayer->m_bInGuildBattle && dstPlayer->m_bTakeGravityStone)
    {
      return false;
    }
  }

  pDstObj->SetStun(true);
  return true;
}

bool CPlayer::SF_TeleportToDestination(CCharacter *pDstObj, bool bStone)
{
  unsigned __int8 errorCode = 0;
  if (pDstObj->m_ObjID.m_byID)
  {
    return false;
  }

  CPlayer *dstPlayer = static_cast<CPlayer *>(pDstObj);
  if (m_bInGuildBattle || dstPlayer->m_bInGuildBattle)
  {
    if (m_bInGuildBattle && !dstPlayer->m_bInGuildBattle)
    {
      errorCode = 10;
    }
    else if (!m_bInGuildBattle && dstPlayer->m_bInGuildBattle)
    {
      errorCode = 9;
    }
    else if (m_bInGuildBattle && dstPlayer->m_bInGuildBattle)
    {
      const unsigned int srcGuildSerial = m_Param.GetGuildSerial();
      GUILD_BATTLE::CNormalGuildBattleManager *battleManager = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
      GUILD_BATTLE::CNormalGuildBattle *srcBattle = battleManager->GetBattleByGuildSerial(srcGuildSerial);
      CMapData *srcBattleMap = srcBattle->m_pkField->m_pkMap;

      const unsigned int dstGuildSerial = dstPlayer->m_Param.GetGuildSerial();
      GUILD_BATTLE::CNormalGuildBattle *dstBattle = battleManager->GetBattleByGuildSerial(dstGuildSerial);
      CMapData *dstBattleMap = dstBattle->m_pkField->m_pkMap;
      if (srcBattleMap != dstBattleMap)
      {
        errorCode = 9;
      }
      else if (m_bTakeGravityStone)
      {
        errorCode = 5;
      }
    }
  }

  if (!errorCode && (pDstObj->GetCurSecNum() == static_cast<__int64>(-1) || pDstObj->m_bMapLoading))
  {
    errorCode = 7;
  }

  if (!errorCode && (dstPlayer->m_pDHChannel || m_pDHChannel))
  {
    if (m_pDHChannel && dstPlayer->m_pDHChannel)
    {
      if (dstPlayer->m_pDHChannel != m_pDHChannel)
      {
        errorCode = 6;
      }
    }
    else if (m_pDHChannel && !dstPlayer->m_pDHChannel)
    {
      errorCode = 8;
    }
    else if (!m_pDHChannel && dstPlayer->m_pDHChannel)
    {
      errorCode = 6;
    }
  }

  if (!errorCode)
  {
    const int levelLimit = dstPlayer->m_pCurMap->GetLevelLimit();
    if (static_cast<int>(GetLevel()) < levelLimit)
    {
      errorCode = 2;
    }
    else
    {
      const int upLevelLimit = dstPlayer->m_pCurMap->m_pMapSet->m_nUpLevelLim;
      if (upLevelLimit != -1 && static_cast<int>(GetLevel()) > upLevelLimit)
      {
        errorCode = 2;
      }
      else if (g_HolySys.GetDestroyerState() == 2 && g_HolySys.GetDestroyerSerial() == m_dwObjSerial)
      {
        errorCode = 4;
      }
    }

    if (!errorCode && dstPlayer->m_Param.m_pGuild)
    {
      const int raceCode = dstPlayer->m_Param.GetRaceCode();
      for (int roomType = 0; roomType < 2; ++roomType)
      {
        CGuildRoomSystem *guildRoomSystem = CGuildRoomSystem::GetInstance();
        CMapData *guildRoomMap =
          guildRoomSystem->GetMapData(static_cast<unsigned __int8>(raceCode), static_cast<unsigned __int8>(roomType));
        if (guildRoomMap == dstPlayer->m_pCurMap)
        {
          if (!m_Param.m_pGuild || m_Param.GetGuildSerial() != dstPlayer->m_Param.GetGuildSerial())
          {
            errorCode = 11;
          }
          break;
        }
      }
    }
  }
  if (errorCode)
  {
    SendMsg_TeleportError(static_cast<char>(errorCode), dstPlayer->m_pCurMap->m_pMapSet->m_dwIndex);
    return false;
  }

  CMapData *intoMap = dstPlayer->m_pCurMap;
  float newPos[3]{};
  intoMap->GetRandPosInRange(dstPlayer->m_fCurPos, 30, newPos);
  OutOfMap(intoMap, dstPlayer->m_wMapLayerIndex, 3u, newPos);
  SendMsg_MovePortal(static_cast<char>(intoMap->m_pMapSet->m_dwIndex), newPos, 2u);

  if (bStone)
  {
    g_PotionMgr.InsertMovePotionStoneEffect(this);
    SenseState();
    SendMsg_NewMovePotionResult();
  }

  return true;
}

bool CPlayer::SF_TransDestHP(CCharacter *pDstObj, float fEffectValue, unsigned __int8 *byRet)
{
  if (pDstObj == this || pDstObj->m_ObjID.m_byKind)
  {
    return false;
  }

  const unsigned __int8 targetId = pDstObj->m_ObjID.m_byID;
  if (targetId)
  {
    if (targetId == 1)
    {
      const unsigned int monsterState = *reinterpret_cast<unsigned int *>(
        reinterpret_cast<char *>(&reinterpret_cast<CPlayer *>(pDstObj)->m_pSoccerItem[5].m_dwETSerialNumber) + 2);
      if (monsterState == 1)
      {
        return false;
      }
    }
    else if (targetId <= 2 || targetId > 4)
    {
      return false;
    }
  }
  else
  {
    CPlayer *dstPlayer = static_cast<CPlayer *>(pDstObj);
    if (dstPlayer->IsInTown())
    {
      *byRet = 18;
      return false;
    }

    if (m_Param.GetRaceCode() == dstPlayer->m_Param.GetRaceCode())
    {
      return false;
    }
  }

  const int dstHp = static_cast<int>(pDstObj->GetHP());
  const int dstMaxHp = static_cast<int>(pDstObj->GetMaxHP());
  const int minRemainHp = dstMaxHp / 10;
  int absorbHp = 0;
  if (dstHp > minRemainHp)
  {
    absorbHp = dstHp - minRemainHp;
    if (static_cast<float>(absorbHp) > fEffectValue)
    {
      absorbHp = static_cast<int>(fEffectValue);
    }
  }

  const int newHp = static_cast<int>(GetHP()) + absorbHp;
  SetHP(newHp, false);
  SendMsg_SetHPInform();
  pDstObj->RobbedHP(this, absorbHp);
  return true;
}

bool CPlayer::SF_TransMonsterHP(CCharacter * /*pDstObj*/, float fEffectValue)
{
  const int currentHp = static_cast<int>(GetHP());
  const int maxHp = static_cast<int>(GetMaxHP());
  if (currentHp >= maxHp)
  {
    return false;
  }

  int drainedCount = 0;
  _pnt_rect rect{};
  m_pCurMap->GetRectInRadius(&rect, 1, static_cast<int>(GetCurSecNum()));
  for (int y = rect.nStarty; y < rect.nEndy; ++y)
  {
    for (int x = rect.nStartx; x < rect.nEndx; ++x)
    {
      _sec_info *secInfo = m_pCurMap->GetSecInfo();
      const unsigned int secIndex = secInfo->m_nSecNumW * y + x;
      CObjectList *sectorList = m_pCurMap->GetSectorListObj(m_wMapLayerIndex, secIndex);
      if (!sectorList)
      {
        continue;
      }

      _object_list_point *next = sectorList->m_Head.m_pNext;
      while (next != &sectorList->m_Tail)
      {
        CCharacter *target = static_cast<CCharacter *>(next->m_pItem);
        next = next->m_pNext;
        if (target == this || target->m_ObjID.m_byKind || target->m_ObjID.m_byID != 1)
        {
          continue;
        }

        const unsigned int monsterState = *reinterpret_cast<unsigned int *>(
          reinterpret_cast<char *>(&reinterpret_cast<CPlayer *>(target)->m_pSoccerItem[5].m_dwETSerialNumber) + 2);
        if (monsterState == 1)
        {
          continue;
        }

        const int distance = static_cast<int>(GetSqrt(m_fCurPos, target->m_fCurPos));
        if (distance > 200)
        {
          continue;
        }

        const int targetHp = static_cast<int>(target->GetHP());
        const int targetMaxHp = static_cast<int>(target->GetMaxHP());
        if ((static_cast<float>(targetHp) / static_cast<float>(targetMaxHp)) <= 0.30000001f)
        {
          continue;
        }

        const int drainedHp = static_cast<int>(static_cast<float>(targetHp) * fEffectValue);
        if (drainedHp <= 0)
        {
          continue;
        }

        const int selfCurHp = static_cast<int>(GetHP());
        const int selfMaxHp = static_cast<int>(GetMaxHP());
        if (selfCurHp >= selfMaxHp)
        {
          return drainedCount > 0;
        }

        SetHP(selfCurHp + drainedHp, false);
        SendMsg_SetHPInform();
        target->RobbedHP(this, drainedHp);
        ++drainedCount;
      }
    }
  }

  return drainedCount > 0;
}

bool CPlayer::SF_HPInc_Once(CCharacter *pDstObj, float fEffectValue)
{
  bool canApply = false;
  if (!pDstObj->m_ObjID.m_byID)
  {
    canApply = !pDstObj->m_bCorpse;
  }
  if (!canApply)
  {
    return false;
  }

  const int currentHp = static_cast<int>(pDstObj->GetHP());
  const int maxHp = static_cast<int>(pDstObj->GetMaxHP());
  const float effectRate = pDstObj->m_EP.GetEff_Rate(EFF_RATE_HP_GAIN);
  const float scaledEffectValue = fEffectValue * effectRate;
  const int addHp = static_cast<int>(static_cast<float>(maxHp) * scaledEffectValue);
  pDstObj->SetHP(addHp + currentHp, false);

  if ((static_cast<float>(currentHp) / static_cast<float>(maxHp)) > 0.80000001f)
  {
    return false;
  }

  pDstObj->SendMsg_SetHPInform();
  return true;
}

bool CPlayer::SF_IncHPCircleParty(CCharacter * /*pDstObj*/, float fEffectValue)
{
  if (!m_pPartyMgr->IsPartyMode())
  {
    return false;
  }

  int alteredCount = 0;
  CPlayer *partyMembers[8]{};
  const unsigned __int8 memberCount = _GetPartyMemberInCircle(partyMembers, 8, true);
  for (int memberIndex = 0; memberIndex < memberCount; ++memberIndex)
  {
    CPlayer *member = partyMembers[memberIndex];
    if (member->m_bCorpse)
    {
      continue;
    }
    if (member->m_bInGuildBattle && member->m_bTakeGravityStone)
    {
      continue;
    }
    if (m_bInGuildBattle
        && (!member->m_bInGuildBattle || m_byGuildBattleColorInx != member->m_byGuildBattleColorInx))
    {
      continue;
    }

    const int currentHp = member->GetHP();
    const int addHp = static_cast<int>(fEffectValue * static_cast<float>(member->GetMaxHP()));
    member->SetHP(addHp + currentHp, false);
    if (currentHp != member->GetHP())
    {
      ++alteredCount;
      member->SendMsg_SetHPInform();
    }
  }

  return alteredCount > 0;
}

bool CPlayer::SF_IncreaseDP(CCharacter *pDstObj, float fEffectValue)
{
  if (pDstObj->m_ObjID.m_byID)
  {
    return false;
  }

  CPlayer *dstPlayer = static_cast<CPlayer *>(pDstObj);
  int newDp = static_cast<int>(static_cast<float>(dstPlayer->GetMaxDP()) * fEffectValue);
  newDp += dstPlayer->GetDP();
  dstPlayer->SetDP(newDp, false);
  SendMsg_Recover();
  return true;
}

bool CPlayer::SF_LateContDamageRemove_Once(CCharacter *pDstObj)
{
  int latestIndex = -1;
  _sf_continous *latestCont = nullptr;
  for (int index = 0; index < 8; ++index)
  {
    _sf_continous *contEffect = &pDstObj->m_SFCont[0][index];
    if (!contEffect->m_bExist)
    {
      continue;
    }

    _base_fld *effect17 = g_Main.m_tblEffectData[3].GetRecord("17");
    const bool hasAuraGuard = static_cast<unsigned __int8>(pDstObj->m_SFContAura[0][5].m_wDurSec >> 8) != 0;
    const bool isProtectedEffect = effect17 && hasAuraGuard && contEffect->m_byEffectCode == 3
                                && contEffect->m_wEffectIndex == effect17->m_dwIndex;
    if (isProtectedEffect)
    {
      continue;
    }

    if (latestIndex == -1 || contEffect->m_dwStartSec > latestCont->m_dwStartSec)
    {
      latestIndex = index;
      latestCont = contEffect;
    }
  }

  if (latestIndex == -1)
  {
    return false;
  }

  if (ms_pXmas_Snow_Bullet_Effect && ms_pXmas_Snow_Bullet_Effect->m_dwIndex == latestCont->m_wEffectIndex
      && latestCont->m_byEffectCode == 3)
  {
    return false;
  }

  pDstObj->RemoveSFContEffect(0, static_cast<unsigned __int16>(latestIndex), false, false);
  return true;
}

bool CPlayer::SF_LateContHelpForceRemove_Once(CCharacter *pDstObj)
{
  const float randomValue = static_cast<float>(rand() % 100);
  const float resistValue = pDstObj->m_EP.GetEff_Plus(EFF_PLUS_CONT_HELP_REMOVE_RESIST);
  if (resistValue > randomValue)
  {
    return false;
  }

  int latestIndex = -1;
  _sf_continous *latestCont = nullptr;
  for (int index = 0; index < 8; ++index)
  {
    _sf_continous *contEffect = &pDstObj->m_SFCont[1][index];
    if (!contEffect->m_bExist || contEffect->m_byEffectCode != 1)
    {
      continue;
    }

    if (latestIndex == -1 || contEffect->m_dwStartSec > latestCont->m_dwStartSec)
    {
      latestIndex = index;
      latestCont = contEffect;
    }
  }

  if (latestIndex == -1)
  {
    return false;
  }

  pDstObj->RemoveSFContEffect(1u, static_cast<unsigned __int16>(latestIndex), false, false);
  return true;
}

bool CPlayer::SF_LateContHelpSkillRemove_Once(CCharacter *pDstObj)
{
  const float randomValue = static_cast<float>(rand() % 100);
  const float resistValue = pDstObj->m_EP.GetEff_Plus(EFF_PLUS_CONT_HELP_REMOVE_RESIST);
  if (resistValue > randomValue)
  {
    return false;
  }

  int latestIndex = -1;
  _sf_continous *latestCont = nullptr;
  for (int index = 0; index < 8; ++index)
  {
    _sf_continous *contEffect = &pDstObj->m_SFCont[1][index];
    if (!contEffect->m_bExist || contEffect->m_byEffectCode)
    {
      continue;
    }

    if (latestIndex == -1 || contEffect->m_dwStartSec > latestCont->m_dwStartSec)
    {
      latestIndex = index;
      latestCont = contEffect;
    }
  }

  if (latestIndex == -1)
  {
    return false;
  }

  pDstObj->RemoveSFContEffect(1u, static_cast<unsigned __int16>(latestIndex), false, false);
  return true;
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

void CPlayer::HSKQuestEnd_Att(unsigned __int8 byDestroyStoneRaceCode, CPlayer *pDestroyer)
{
  (void)byDestroyStoneRaceCode;

  bool isMentalPass = false;
  _Quest_fld *questFld = reinterpret_cast<_Quest_fld *>(g_HolySys.m_tblQuest.GetRecord(m_Param.GetRaceCode()));
  if (questFld)
  {
    if (pDestroyer == this || pDestroyer->m_Param.GetRaceCode() == m_Param.GetRaceCode())
    {
      isMentalPass = true;
    }
  }

  if (isMentalPass)
  {
    const unsigned __int8 numOfTime = g_HolySys.GetNumOfTime();
    const unsigned __int8 startHour = g_HolySys.GetStartHour();
    const unsigned __int8 startDay = g_HolySys.GetStartDay();
    const unsigned __int8 startMonth = g_HolySys.GetStartMonth();
    const unsigned __int16 startYear = g_HolySys.GetStartYear();
    if (!m_MinigTicket.AuthLastMentalTicket(startYear, startMonth, startDay, startHour, numOfTime))
    {
      isMentalPass = g_HolySys.IsMentalPass();
    }
  }

  if (isMentalPass)
  {
    const unsigned __int8 numOfTime = g_HolySys.GetNumOfTime();
    const unsigned __int8 startHour = g_HolySys.GetStartHour();
    const unsigned __int8 startDay = g_HolySys.GetStartDay();
    const unsigned __int8 startMonth = g_HolySys.GetStartMonth();
    const unsigned __int16 startYear = g_HolySys.GetStartYear();

    UpdateLastCriTicket(startYear, startMonth, startDay, startHour, numOfTime);
    _Reward_Quest(questFld, 255);
    SendMsg_HSKQuestSucc(0, 1);
    if (questFld->m_nConsContribution > 0)
    {
      SendMsg_RaceBattlePenelty(questFld->m_nConsContribution, 0);
    }
  }
  else
  {
    if (pDestroyer->m_Param.GetRaceCode() == m_Param.GetRaceCode())
    {
      const int alterPoint = g_HolySys.m_nRaceBattlePoint[0][1];
      AlterPvPPoint(static_cast<double>(alterPoint), holy_dec, -1);
      SendMsg_RaceBattlePenelty(alterPoint, 0);
    }
    SendMsg_HSKQuestSucc(0, 0);
  }

  m_byHSKQuestCode = 100;
}

void CPlayer::SendMsg_RecvHSKQuest()
{

  _recv_hsk_quest_inform_zocl msg{};
  msg.byHSKQuestCode = m_byHSKQuestCode;
  msg.nPvpPoint = m_nHSKPvpPoint;
  msg.wKillPoint = m_wKillPoint;

  const unsigned __int8 byNumOfTime = g_HolySys.GetNumOfTime();
  const unsigned __int8 byCurrentHour = g_HolySys.GetStartHour();
  const unsigned __int8 byStartDay = g_HolySys.GetStartDay();
  const unsigned __int8 byStartMonth = g_HolySys.GetStartMonth();
  const unsigned __int16 byStartYear = g_HolySys.GetStartYear();

  msg.byHolyMentalCount =
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

void CPlayer::RewardRaceWarPvpCash()
{
  const double playerPenalty = g_Main.m_pTimeLimitMgr->GetPlayerPenalty(m_id.wIndex);
  long double dAlter = static_cast<long double>(5 * GetLevel());
  if (dAlter > 0.0)
  {
    dAlter *= playerPenalty;
  }

  AlterPvPCashBag(dAlter, pm_scaner);
  m_kPvpCashPoint.SetRaceWarRecvr(true);
  m_kPvpOrderView.Update_RaceWarRecvr(true);
}

void CPlayer::IncPvPPoint(long double dAlter, int AlterType, unsigned int dwDstSerial)
{
  if (dAlter >= 1.0)
  {
    AlterPvPPoint(static_cast<double>(dAlter), AlterType, dwDstSerial);
  }
}

void CPlayer::SendMsg_AlterPvPPoint()
{
  _alter_pvp_point_zocl msg{};
  msg.dNewPoint = static_cast<long double>(m_Param.GetPvPPoint());
  unsigned __int8 type[2] = {11, 10};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_RaceBattlePenelty(int nAlterPoint, char byAlterType)
{

  _notify_race_battle_penelty_zocl msg{};
  msg.byLoseType = byAlterType;
  msg.nAlterPoint = nAlterPoint;
  unsigned __int8 type[2] = {25, 28};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SetCntEnable(bool bSet)
{
  m_bCntEnable = bSet;
}

void CPlayer::Resurrect()
{
  unsigned __int8 result = 0;
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

  if (!result)
  {
    m_bCorpse = false;
    m_byModeType = 0;
    m_byMoveType = 1;

    SetHP(static_cast<int>(GetMaxHP()), false);
    SetFP(GetMaxFP(), false);
    SetSP(GetMaxSP(), false);
    SendMsg_ResurrectInform();
  }

  SendMsg_Resurrect(static_cast<char>(result), false);
  if (m_bAfterEffect)
  {
    pc_NuclearAfterEffect();
  }
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
  _mine_ore_deposit_inform_zocl msg{};
  msg.byDepositRate = COreAmountMgr::Instance()->GetDepositRate();
  unsigned __int8 type[2] = {14, 68};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_OreTransferCount()
{
  _ore_carry_forword_count_zocl msg{};
  msg.byOreTransferCount = COreAmountMgr::Instance()->GetOreTransferCount();
  unsigned __int8 type[2] = {14, 69};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_LendItemTimeExpired(char byStorageCode, unsigned __int16 wSerial)
{

  _lend_item_time_expired_zocl msg{};
  msg.byStorageCode = byStorageCode;
  msg.wSerial = wSerial;
  unsigned __int8 type[2] = {7, 71};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SetHaveEffectUseTime(_STORAGE_LIST::_db_con *pItem, bool bAdd)
{
  if (pItem->m_byTableCode == 18)
  {
    _ResourceItem_fld *record = reinterpret_cast<_ResourceItem_fld *>(
      g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex));
    if (record)
    {
      if (bAdd)
      {
        for (int j = 0; j < record->m_nEffectDataNum; ++j)
        {
          const int effCode = record->m_EffectData[j].nEffCode;
          if (static_cast<unsigned int>(effCode) <= 82u)
          {
            const signed __int64 dur = static_cast<signed __int64>(pItem->m_dwDur);
            float durValue = static_cast<float>(static_cast<int>(dur));
            if (dur < 0)
            {
              durValue += 1.8446744e19f;
            }

            m_EP.m_pDataParam->m_fEff_Have[effCode] += record->m_EffectData[j].fEffUnit * durValue;
            if (m_EP.m_pDataParam->m_fEff_Have[effCode] > record->m_EffectData[j].fEffUnitMax)
            {
              m_EP.m_pDataParam->m_fEff_Have[effCode] = record->m_EffectData[j].fEffUnitMax;
              if (((effCode >= 71 && effCode <= 75)
                   || (effCode >= 5 && effCode <= 9)
                   || effCode == 0
                   || effCode == 2))
              {
                m_EP.m_pDataParam->m_fEff_Have[effCode] = m_EP.m_pDataParam->m_fEff_Have[effCode] + FLOAT_1_0;
              }
            }
          }
        }

        s_MgrItemHistory.time_jade_effect_log(record->m_strCode, pItem, true, m_szItemHistoryFileName);
      }
      else
      {
        for (int k = 0; k < record->m_nEffectDataNum; ++k)
        {
          const int effCode = record->m_EffectData[k].nEffCode;
          if (effCode > -1)
          {
            if (effCode < 83)
            {
              const signed __int64 durForCmp = static_cast<signed __int64>(pItem->m_dwDur);
              float durValueForCmp = static_cast<float>(static_cast<int>(durForCmp));
              if (durForCmp < 0)
              {
                durValueForCmp += 1.8446744e19f;
              }

              if (record->m_EffectData[k].fEffUnitMax <= (record->m_EffectData[k].fEffUnit * durValueForCmp))
              {
                const signed __int64 durForSub = static_cast<signed __int64>(pItem->m_dwDur);
                float durValueForSub = static_cast<float>(static_cast<int>(durForSub));
                if (durForSub < 0)
                {
                  durValueForSub += 1.8446744e19f;
                }

                m_EP.m_pDataParam->m_fEff_Have[effCode] -= record->m_EffectData[k].fEffUnit * durValueForSub;
              }
              else
              {
                m_EP.m_pDataParam->m_fEff_Have[effCode] -= record->m_EffectData[k].fEffUnitMax;
              }

              if ((effCode < 71 || effCode > 75)
                  && (effCode < 5 || effCode > 9)
                  && effCode != 0
                  && effCode != 2)
              {
                if (m_EP.m_pDataParam->m_fEff_Have[effCode] < 0.0f)
                {
                  m_EP.m_pDataParam->m_fEff_Have[effCode] = 0.0f;
                }
              }
              else if (m_EP.m_pDataParam->m_fEff_Have[effCode] < FLOAT_1_0)
              {
                m_EP.m_pDataParam->m_fEff_Have[effCode] = FLOAT_1_0;
              }
            }

            s_MgrItemHistory.time_jade_effect_log(record->m_strCode, pItem, false, m_szItemHistoryFileName);
          }
        }
      }
    }
  }
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
  if (!lp_pOne->m_pmTrd.bDTradeMode || lp_pOne->m_pmTrd.wDTradeDstIndex == 65535)
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
  if (byStorageCode == 1 && bySlotIndex < 8u)
  {
    return m_byEffectEquipCode[bySlotIndex];
  }

  if (byStorageCode == 2 && bySlotIndex < 7u)
  {
    return m_byEffectEquipCode[bySlotIndex + 8];
  }

  return 0;
}

void CPlayer::SetEffectEquipCode(unsigned __int8 byStorageCode, unsigned __int8 bySlotIndex, unsigned __int8 byCode)
{
  if (byStorageCode == 1 && bySlotIndex < 8u)
  {
    m_byEffectEquipCode[bySlotIndex] = byCode;
    return;
  }

  if (byStorageCode == 2 && bySlotIndex < 7u)
  {
    m_byEffectEquipCode[bySlotIndex + 8] = byCode;
  }
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
  const float effHave = m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_4);
  if (static_cast<float>(itemEquipLevel) > (level + effHave))
  {
    return 0;
  }

  if (itemEquipUpLevel != static_cast<unsigned __int8>(-1))
  {
    const float limitLevel = static_cast<float>(itemEquipUpLevel);
    const float curLevel = static_cast<float>(m_Param.GetLevel());
    const float curEffHave = m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_4);
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

bool CPlayer::_check_equip_part(_STORAGE_LIST::_db_con *pFixingItem)
{
  const unsigned __int8 raceSexCode = m_Param.GetRaceSexCode();
  const int itemIndex = pFixingItem->m_wItemIndex;
  const int tableCode = pFixingItem->m_byTableCode;
  if (!IsItemEquipCivil(tableCode, itemIndex, raceSexCode))
  {
    return false;
  }
  if (!IsEffectableEquip(pFixingItem))
  {
    return false;
  }
  if (pFixingItem->m_byTableCode == 6)
  {
    _WeaponItem_fld *record =
      reinterpret_cast<_WeaponItem_fld *>(g_Main.m_tblItemData[6].GetRecord(pFixingItem->m_wItemIndex));
    if (!record)
    {
      return false;
    }
    const int equipCode = record->m_nType;
    if (equipCode == 100 && m_Param.m_dbEquip.m_pStorageList[5].m_bLoad)
    {
      return false;
    }
    bool hasClassMatch = false;
    for (int j = 0; j < 4; ++j)
    {
      _class_fld *historyClass = *m_Param.m_ppHistoryEffect[j];
      if (!historyClass || historyClass->m_nClass >= 4u)
      {
        break;
      }
      if (record->m_strClassLim[historyClass->m_nClass] == '1')
      {
        hasClassMatch = true;
        break;
      }
    }
    if (!hasClassMatch)
    {
      return false;
    }
  }
  if (pFixingItem->m_byTableCode == 5)
  {
    _STORAGE_LIST::_db_con *shieldItem = m_Param.m_dbEquip.m_pStorageList + 6;
    if (shieldItem->m_bLoad)
    {
      _WeaponItem_fld *record =
        reinterpret_cast<_WeaponItem_fld *>(g_Main.m_tblItemData[6].GetRecord(shieldItem->m_wItemIndex));
      if (record && record->m_nType == 100)
      {
        return false;
      }
    }
  }
  return true;
}

bool CPlayer::_check_embel_part(_STORAGE_LIST::_db_con *pFixingItem)
{
  const unsigned __int8 raceSexCode = m_Param.GetRaceSexCode();
  const int itemIndex = pFixingItem->m_wItemIndex;
  const int tableCode = pFixingItem->m_byTableCode;
  if (!IsItemEquipCivil(tableCode, itemIndex, raceSexCode))
  {
    return false;
  }
  const float itemEquipLevel = static_cast<float>(GetItemEquipLevel(pFixingItem->m_byTableCode, pFixingItem->m_wItemIndex));
  const float level = static_cast<float>(m_Param.GetLevel());
  const float effHave = m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_4);
  return itemEquipLevel <= (level + effHave);
}

__int64 CPlayer::_check_mastery_lim(unsigned __int8 byMasteryClass, unsigned __int8 byIndex)
{
  _MasteryLimit_fld *curRecord = nullptr;
  _MasteryLimit_fld *baseRecord = nullptr;

  if (m_Param.GetPtrCurClass()->m_nClass < 4 && m_Param.GetPtrBaseClass()->m_nClass < 4)
  {
    if (m_Param.GetRaceCode() < 3)
    {
      const int maxLevel = static_cast<int>(m_Param.GetMaxLevel());
      const int level = static_cast<int>(GetLevel());
      if (level <= maxLevel && level > 0)
      {
        if (m_Param.m_pClassHistory[0])
        {
          _class_fld *curClass = m_Param.GetPtrCurClass();
          _class_fld *baseClass = m_Param.GetPtrBaseClass();
          if (curClass->m_nClass == baseClass->m_nClass)
          {
            const int recordIndex = level - 1;
            CRecordData *table = s_tblLimMasteryContinue[m_Param.GetRaceCode()];
            curRecord = reinterpret_cast<_MasteryLimit_fld *>(table[curClass->m_nClass].GetRecord(recordIndex));
          }
          else
          {
            const int recordIndex = level - 1;
            CRecordData *table = s_tblLimMastery[m_Param.GetRaceCode()];
            curRecord = reinterpret_cast<_MasteryLimit_fld *>(table[curClass->m_nClass].GetRecord(recordIndex));
            baseRecord = reinterpret_cast<_MasteryLimit_fld *>(table[baseClass->m_nClass].GetRecord(recordIndex));
          }
        }
        else
        {
          const int recordIndex = level - 1;
          CRecordData *table = s_tblLimMastery[m_Param.GetRaceCode()];
          _class_fld *curClass = m_Param.GetPtrCurClass();
          curRecord = reinterpret_cast<_MasteryLimit_fld *>(table[curClass->m_nClass].GetRecord(recordIndex));
        }

        if (curRecord)
        {
          int curLim = 0;
          int baseLim = 0;
          switch (byMasteryClass)
          {
            case 0:
              curLim = curRecord->m_MasteryLim.m_nBnsMMastery[byIndex];
              if (baseRecord)
              {
                baseLim = baseRecord->m_MasteryLim.m_nBnsMMastery[byIndex];
              }
              break;
            case 1:
              curLim = curRecord->m_MasteryLim.m_nBnsDefMastery;
              if (baseRecord)
              {
                baseLim = baseRecord->m_MasteryLim.m_nBnsDefMastery;
              }
              break;
            case 2:
              curLim = curRecord->m_MasteryLim.m_nBnsPryMastery;
              if (baseRecord)
              {
                baseLim = baseRecord->m_MasteryLim.m_nBnsPryMastery;
              }
              break;
            case 3:
              curLim = curRecord->m_MasteryLim.m_nBnsSkillMastery[byIndex];
              if (baseRecord)
              {
                baseLim = baseRecord->m_MasteryLim.m_nBnsSkillMastery[byIndex];
              }
              break;
            case 4:
              curLim = curRecord->m_MasteryLim.m_nBnsForceMastery[byIndex];
              if (baseRecord)
              {
                baseLim = baseRecord->m_MasteryLim.m_nBnsForceMastery[byIndex];
              }
              break;
            case 5:
              curLim = curRecord->m_MasteryLim.m_nBnsMakeMastery[byIndex];
              if (baseRecord)
              {
                baseLim = baseRecord->m_MasteryLim.m_nBnsMakeMastery[byIndex];
              }
              break;
            case 6:
              curLim = curRecord->m_MasteryLim.m_nBnsSMastery;
              if (baseRecord)
              {
                baseLim = baseRecord->m_MasteryLim.m_nBnsSMastery;
              }
              break;
            default:
              break;
          }

          if (curLim <= baseLim)
          {
            return static_cast<unsigned int>(baseLim);
          }
          return static_cast<unsigned int>(curLim);
        }

        g_Main.m_logSystemError.Write("_check_mastery_lim.. pCurFld : NULL");
        return 0;
      }

      g_Main.m_logSystemError.Write("_check_mastery_lim.. level : %d, max level : %d", level, maxLevel);
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
        _DfnEquipItem_fld *record = reinterpret_cast<_DfnEquipItem_fld *>(
          g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex));
        return record ? record->m_Effect : nullptr;
      }
      case 1:
      {
        _WeaponItem_fld *record = reinterpret_cast<_WeaponItem_fld *>(
          g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex));
        return record ? record->m_Effect : nullptr;
      }
      case 2:
      {
        _CloakItem_fld *record = reinterpret_cast<_CloakItem_fld *>(
          g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex));
        return record ? record->m_Effect : nullptr;
      }
      case 3:
      {
        _RingItem_fld *record = reinterpret_cast<_RingItem_fld *>(
          g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex));
        return record ? record->m_Effect : nullptr;
      }
      case 4:
      {
        _AmuletItem_fld *record = reinterpret_cast<_AmuletItem_fld *>(
          g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex));
        return record ? record->m_Effect : nullptr;
      }
      case 22:
      {
        _SiegeKitItem_fld *record = reinterpret_cast<_SiegeKitItem_fld *>(
          g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex));
        return record ? record->m_Effect : nullptr;
      }
      default:
        return nullptr;
    }
  }

  _DfnEquipItem_fld *record = reinterpret_cast<_DfnEquipItem_fld *>(
    g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex));
  return record ? record->m_Effect : nullptr;
}

void CPlayer::apply_normal_item_std_effect(int nEffCode, float fVal, bool bEquip)
{
  switch (nEffCode)
  {
    case 1:
      m_EP.SetEff_Rate(EFF_RATE_MAX_SP, fVal, bEquip);
      break;
    case 2:
      m_EP.SetEff_Rate(EFF_RATE_FP_COST, fVal, bEquip);
      break;
    case 3:
      for (int paramIndex = 0; paramIndex < 2; ++paramIndex)
      {
        m_EP.SetEff_Plus(paramIndex, fVal, bEquip);
      }
      m_EP.SetEff_Plus(EFF_PLUS_UNKNOWN_2, fVal, bEquip);
      break;
    case 4:
    case 24:
      m_EP.SetEff_Plus(EFF_PLUS_AVOID_RATE, fVal, bEquip);
      break;
    case 5:
      m_EP.SetEff_Rate(EFF_RATE_MAX_HP, fVal, bEquip);
      m_EP.SetEff_Rate(EFF_RATE_MAX_FP, fVal, bEquip);
      break;
    case 6:
      for (int paramIndex = 0; paramIndex < 2; ++paramIndex)
      {
        m_EP.SetEff_Rate(paramIndex, fVal, bEquip);
        m_EP.SetEff_Rate(paramIndex + 2, fVal, bEquip);
      }
      m_EP.SetEff_Rate(EFF_RATE_FORCE_ATTACK, fVal, bEquip);
      m_EP.SetEff_Rate(EFF_RATE_LAUNCHER_ATTACK, fVal, bEquip);
      break;
    case 7:
    case 21:
      m_EP.SetEff_Rate(EFF_RATE_FINAL_DEFENSE, fVal, bEquip);
      break;
    case 8:
      m_EP.SetEff_Plus(EFF_PLUS_SKILL_LEVEL, fVal, bEquip);
      break;
    case 9:
      m_EP.SetEff_Plus(EFF_PLUS_UNKNOWN_21, 1.0f, bEquip);
      break;
    case 10:
      m_EP.SetEff_Plus(EFF_PLUS_UNKNOWN_22, 1.0f, bEquip);
      break;
    case 11:
      m_EP.SetEff_Plus(EFF_PLUS_UNKNOWN_23, 1.0f, bEquip);
      break;
    case 12:
      if (!m_bInGuildBattle || !m_bTakeGravityStone)
      {
        m_EP.SetEff_Plus(EFF_PLUS_MOVE_RUN_SPEED, fVal, bEquip);
      }
      break;
    case 13:
      m_EP.SetEff_Plus(EFF_PLUS_UNKNOWN_24, 1.0f, bEquip);
      break;
    case 14:
      m_EP.SetEff_Plus(EFF_PLUS_FP_RECOVERY_RATE_POINTS, fVal, bEquip);
      break;
    case 15:
      m_EP.SetEff_Rate(EFF_RATE_FORCE_ATTACK, fVal, bEquip);
      break;
    case 16:
      m_EP.SetEff_Rate(EFF_RATE_MAX_FP, fVal, bEquip);
      break;
    case 17:
      m_EP.SetEff_Rate(EFF_RATE_DAMAGE_TO_HP, fVal, bEquip);
      break;
    case 18:
      m_EP.SetEff_Rate(EFF_RATE_DAMAGE_TO_FP, fVal, bEquip);
      break;
    case 19:
      m_EP.SetEff_Plus(EFF_PLUS_CRITICAL_RATE, fVal, bEquip);
      break;
    case 20:
      for (int paramIndex = 0; paramIndex < 2; ++paramIndex)
      {
        m_EP.SetEff_Plus(paramIndex + 4, fVal, bEquip);
        m_EP.SetEff_Plus(paramIndex + 6, fVal, bEquip);
      }
      m_EP.SetEff_Plus(EFF_PLUS_ATTACK_RANGE, fVal, bEquip);
      break;
    case 22:
    case 31:
      m_EP.SetEff_Rate(EFF_RATE_FORCE_CONT_DURATION, fVal, bEquip);
      break;
    case 23:
      m_EP.SetEff_Rate(EFF_RATE_HP_RECOVERY, fVal, bEquip);
      break;
    case 25:
      m_EP.SetEff_Plus(EFF_PLUS_UNKNOWN_11, fVal, bEquip);
      break;
    case 26:
      m_EP.SetEff_Plus(EFF_PLUS_UNKNOWN_8, fVal, bEquip);
      break;
    case 27:
      m_EP.SetEff_Plus(EFF_PLUS_UNKNOWN_37, fVal, bEquip);
      break;
    case 28:
      m_EP.SetEff_Plus(EFF_PLUS_SHIELD_BLOCK_RATE, fVal, bEquip);
      break;
    case 29:
      m_EP.SetEff_Plus(EFF_PLUS_FIRE_TOLERANCE, fVal, bEquip);
      m_EP.SetEff_Plus(EFF_PLUS_WATER_TOLERANCE, fVal, bEquip);
      m_EP.SetEff_Plus(EFF_PLUS_SOIL_TOLERANCE, fVal, bEquip);
      m_EP.SetEff_Plus(EFF_PLUS_WIND_TOLERANCE, fVal, bEquip);
      break;
    case 30:
      m_EP.SetEff_Rate(EFF_RATE_MAX_HP, fVal, bEquip);
      break;
    case 32:
      m_EP.SetEff_Plus(EFF_PLUS_SHIELD_IGNORE_RATE, fVal, bEquip);
      break;
    case 33:
      m_EP.SetEff_Plus(EFF_PLUS_UNKNOWN_10, fVal, bEquip);
      break;
    case 34:
      m_EP.SetEff_Plus(EFF_PLUS_SKILL_ACT_DELAY, fVal, bEquip);
      break;
    case 35:
      m_EP.SetEff_Plus(EFF_PLUS_UNKNOWN_13, fVal, bEquip);
      break;
    case 36:
      m_EP.SetEff_Plus(EFF_PLUS_CONT_HELP_REMOVE_RESIST, fVal, bEquip);
      break;
    default:
      return;
  }
}

void CPlayer::apply_case_equip_upgrade_effect(_STORAGE_LIST::_db_con *pItem, bool bEquip)
{
  unsigned int dwLvBit[4]{};
  std::memcpy(dwLvBit, &pItem->m_dwLv, sizeof(dwLvBit[0]));

  const unsigned __int8 itemUpgedLv = GetItemUpgedLv(dwLvBit[0]);
  if (!itemUpgedLv || !GetDefItemUpgSocketNum(pItem->m_byTableCode, pItem->m_wItemIndex))
  {
    float effectValue = 0.0f;
    const float have79 = m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_79);
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
      m_EP.SetEff_Rate(EFF_RATE_FORCE_ATTACK, effectValue, bEquip);
      m_EP.SetEff_Rate(EFF_RATE_LAUNCHER_ATTACK, effectValue, bEquip);
    }
    else
    {
      const float have80 = m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_80);
      if (have80 > 0.0f && have80 < 5.0f && pItem->m_byTableCode <= 5u)
      {
        _ItemUpgrade_fld *record = g_Main.m_tblItemUpgrade.GetRecord(5u);
        effectValue = record->m_fEffectUnitList[static_cast<int>(have80)];
        m_EP.SetEff_Rate(EFF_RATE_FINAL_DEFENSE, effectValue, bEquip);
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

    float talikValue = record->m_fEffectUnitList[talikCount];
    switch (talik)
    {
      case 0:
      {
        int effHave79 = static_cast<int>(m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_79));
        if (effHave79 > 5)
        {
          effHave79 = 5;
        }
        if (effHave79 > talikCount
            && GetItemGrade(pItem->m_byTableCode, pItem->m_wItemIndex) < 3u
            && pItem->m_byTableCode == 6)
        {
          talikValue = record->m_fEffectUnitList[effHave79];
        }

        for (int paramIndex = 0; paramIndex < 2; ++paramIndex)
        {
          m_EP.SetEff_Rate(paramIndex, talikValue, bEquip);
          m_EP.SetEff_Rate(paramIndex + 2, talikValue, bEquip);
        }
        m_EP.SetEff_Rate(EFF_RATE_FORCE_ATTACK, talikValue, bEquip);
        m_EP.SetEff_Rate(EFF_RATE_LAUNCHER_ATTACK, talikValue, bEquip);
        break;
      }
      case 1:
        m_EP.SetEff_Rate(EFF_RATE_DAMAGE_TO_HP, talikValue, bEquip);
        break;
      case 2:
        m_EP.SetEff_Plus(EFF_PLUS_SHIELD_IGNORE_RATE, talikValue, bEquip);
        break;
      case 3:
        if (pItem->m_byTableCode == 7)
        {
          m_EP.SetEff_Plus(EFF_PLUS_UNKNOWN_37, talikValue, bEquip);
        }
        else
        {
          m_EP.SetEff_Plus(EFF_PLUS_CRITICAL_RATE, talikValue, bEquip);
        }
        break;
      case 4:
        m_EP.SetEff_Plus(EFF_PLUS_UNKNOWN_5, talikValue, bEquip);
        break;
      case 5:
      {
        int effHave80 = static_cast<int>(m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_80));
        if (effHave80 > 4)
        {
          effHave80 = 4;
        }
        if (talikCount < effHave80 && pItem->m_byTableCode <= 5u)
        {
          talikValue = record->m_fEffectUnitList[effHave80];
        }
        m_EP.SetEff_Rate(EFF_RATE_FINAL_DEFENSE, talikValue, bEquip);
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
        m_EP.SetEff_Plus(EFF_PLUS_CONT_HELP_REMOVE_RESIST, talikValue, bEquip);
        break;
      case 7:
        if (pItem->m_byTableCode != 6)
        {
          m_EP.SetEff_Plus(EFF_PLUS_FIRE_TOLERANCE, talikValue, bEquip);
        }
        break;
      case 8:
        if (pItem->m_byTableCode != 6)
        {
          m_EP.SetEff_Plus(EFF_PLUS_WATER_TOLERANCE, talikValue, bEquip);
        }
        break;
      case 9:
        if (pItem->m_byTableCode != 6)
        {
          m_EP.SetEff_Plus(EFF_PLUS_SOIL_TOLERANCE, talikValue, bEquip);
        }
        break;
      case 10:
        if (pItem->m_byTableCode != 6)
        {
          m_EP.SetEff_Plus(EFF_PLUS_WIND_TOLERANCE, talikValue, bEquip);
        }
        break;
      case 11:
        for (int paramIndex = 0; paramIndex < 2; ++paramIndex)
        {
          m_EP.SetEff_Plus(paramIndex, talikValue, bEquip);
        }
        m_EP.SetEff_Plus(EFF_PLUS_UNKNOWN_2, talikValue, bEquip);
        m_EP.SetEff_Plus(EFF_PLUS_UNKNOWN_31, talikValue, bEquip);
        m_EP.SetEff_Plus(EFF_PLUS_UNKNOWN_30, talikValue, bEquip);
        break;
      case 12:
      {
        const float halfValue = talikValue / 2.0f;
        m_EP.SetEff_Plus(EFF_PLUS_AVOID_RATE, halfValue, bEquip);
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
  std::memset(m_nTolValue, 0, sizeof(m_nTolValue));

  auto addFloatTol = [this](float fireTol, float waterTol, float soilTol, float windTol)
  {
    m_nTolValue[0] = static_cast<int>(static_cast<float>(m_nTolValue[0]) + fireTol);
    m_nTolValue[1] = static_cast<int>(static_cast<float>(m_nTolValue[1]) + waterTol);
    m_nTolValue[2] = static_cast<int>(static_cast<float>(m_nTolValue[2]) + soilTol);
    m_nTolValue[3] = static_cast<int>(static_cast<float>(m_nTolValue[3]) + windTol);
  };

  if (!IsRidingUnit())
  {
    for (int partIndex = 0; partIndex < 5; ++partIndex)
    {
      _STORAGE_LIST::_db_con *equipItem = &m_Param.m_dbEquip.m_pStorageList[partIndex];
      if (equipItem->m_bLoad)
      {
        _DfnEquipItem_fld *record = static_cast<_DfnEquipItem_fld *>(
          g_Main.m_tblItemData[partIndex].GetRecord(equipItem->m_wItemIndex));
        if (record)
        {
          addFloatTol(record->m_fFireTol, record->m_fWaterTol, record->m_fSoilTol, record->m_fWindTol);
        }
      }
    }

    _STORAGE_LIST::_db_con *cloakItem = &m_Param.m_dbEquip.m_pStorageList[7];
    if (cloakItem->m_bLoad)
    {
      _CloakItem_fld *record = static_cast<_CloakItem_fld *>(
        g_Main.m_tblItemData[7].GetRecord(cloakItem->m_wItemIndex));
      if (record)
      {
        addFloatTol(record->m_fFireTol, record->m_fWaterTol, record->m_fSoilTol, record->m_fWindTol);
      }
    }

    _STORAGE_LIST::_db_con *shieldItem = &m_Param.m_dbEquip.m_pStorageList[5];
    if (shieldItem->m_bLoad)
    {
      _DfnEquipItem_fld *record = static_cast<_DfnEquipItem_fld *>(
        g_Main.m_tblItemData[5].GetRecord(shieldItem->m_wItemIndex));
      if (record)
      {
        addFloatTol(record->m_fFireTol, record->m_fWaterTol, record->m_fSoilTol, record->m_fWindTol);
      }
    }

    for (int embelIndex = 0; embelIndex < 7; ++embelIndex)
    {
      _STORAGE_LIST::_db_con *embelItem = &m_Param.m_dbEmbellish.m_pStorageList[embelIndex];
      if (!embelItem->m_bLoad)
      {
        continue;
      }

      if (embelItem->m_byTableCode == 8)
      {
        _RingItem_fld *record = static_cast<_RingItem_fld *>(
          g_Main.m_tblItemData[8].GetRecord(embelItem->m_wItemIndex));
        if (!record)
        {
          continue;
        }
        addFloatTol(record->m_fFireTol, record->m_fWaterTol, record->m_fSoilTol, record->m_fWindTol);
      }
      else if (embelItem->m_byTableCode == 9)
      {
        _AmuletItem_fld *record = static_cast<_AmuletItem_fld *>(
          g_Main.m_tblItemData[9].GetRecord(embelItem->m_wItemIndex));
        if (!record)
        {
          continue;
        }
        addFloatTol(record->m_fFireTol, record->m_fWaterTol, record->m_fSoilTol, record->m_fWindTol);
      }
      else
      {
        continue;
      }
    }

    return;
  }

  for (int partIndex = 0; partIndex < 6; ++partIndex)
  {
    _UnitPart_fld *record = static_cast<_UnitPart_fld *>(
      g_Main.m_tblUnitPart[partIndex].GetRecord(m_pUsingUnit->byPart[partIndex]));
    if (!record)
    {
      continue;
    }

    m_nTolValue[0] += record->m_nFireTol;
    m_nTolValue[1] += record->m_nWaterTol;
    m_nTolValue[2] += record->m_nSoilTol;
    m_nTolValue[3] += record->m_nWindTol;
  }
}

void CPlayer::CalcEquipSpeed()
{
  const float oldEquipSpeed = m_fEquipSpeed;
  m_fEquipSpeed = FLOAT_1_0;

  for (int partIndex = 0; partIndex < 5; ++partIndex)
  {
    _STORAGE_LIST::_db_con *equipItem = &m_Param.m_dbEquip.m_pStorageList[partIndex];
    if (!equipItem->m_bLoad)
    {
      continue;
    }
    _DfnEquipItem_fld *record = reinterpret_cast<_DfnEquipItem_fld *>(
      g_Main.m_tblItemData[partIndex].GetRecord(equipItem->m_wItemIndex));
    if (record)
    {
      m_fEquipSpeed *= record->m_fEquipSpeed;
    }
  }

  _STORAGE_LIST::_db_con *weaponItem = &m_Param.m_dbEquip.m_pStorageList[6];
  if (weaponItem->m_bLoad)
  {
    _WeaponItem_fld *record = reinterpret_cast<_WeaponItem_fld *>(
      g_Main.m_tblItemData[6].GetRecord(weaponItem->m_wItemIndex));
    if (record)
    {
      m_fEquipSpeed *= record->m_fEquipSpeed;
    }
  }

  _STORAGE_LIST::_db_con *shieldItem = &m_Param.m_dbEquip.m_pStorageList[5];
  if (shieldItem->m_bLoad)
  {
    _DfnEquipItem_fld *record = reinterpret_cast<_DfnEquipItem_fld *>(
      g_Main.m_tblItemData[5].GetRecord(shieldItem->m_wItemIndex));
    if (record)
    {
      m_fEquipSpeed *= record->m_fEquipSpeed;
    }
  }

  if (m_fEquipSpeed != oldEquipSpeed)
  {
    SendMsg_AlterEquipSPInform();
  }
}

void CPlayer::SendMsg_AlterEquipSPInform()
{
  _alter_equip_sp_inform_zocl msg{};

  msg.fEquipUseSP = m_fEquipSpeed;

  unsigned __int8 type[2] = {11, 13};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::CalcEquipMaxDP(bool bInit)
{
  int calcMaxDp = 1;
  for (int partIndex = 0; partIndex < 5; ++partIndex)
  {
    _STORAGE_LIST::_db_con *equipItem = &m_Param.m_dbEquip.m_pStorageList[partIndex];
    _DfnEquipItem_fld *record = nullptr;
    if (equipItem->m_bLoad)
    {
      record = reinterpret_cast<_DfnEquipItem_fld *>(
        g_Main.m_tblItemData[partIndex].GetRecord(equipItem->m_wItemIndex));
    }
    else
    {
      record = reinterpret_cast<_DfnEquipItem_fld *>(
        g_Main.m_tblItemData[partIndex].GetRecord(m_Param.m_dbChar.m_byDftPart[partIndex]));
    }
    if (record)
    {
      calcMaxDp += record->m_nMaxDP;
    }
  }

  if (m_nMaxDP != calcMaxDp)
  {
    m_nMaxDP = calcMaxDp;
    if (GetDP() > m_nMaxDP)
    {
      SetDP(m_nMaxDP, false);
      SendMsg_SetDPInform();
    }
    if (!bInit)
    {
      SendMsg_AlterMaxDP();
    }
  }
}

int CPlayer::GetMaxDP()
{
  const float baseMax = static_cast<float>(m_nMaxDP);
  const float bonus = m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_35);
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
  _set_dp_inform_zocl msg{};

  msg.wDP = static_cast<unsigned __int16>(m_Param.GetDP());

  unsigned __int8 type[2] = {17, 114};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_AlterMaxDP()
{
  _max_dp_zocl msg{};

  msg.wMaxDP = static_cast<unsigned __int16>(GetMaxDP());

  unsigned __int8 type[2] = {11, static_cast<unsigned __int8>(-123)};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
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
  _transform_siege_mode_result_zocl msg{};
  msg.byErrCode = byRetCode;

  unsigned __int8 type[2] = {28, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));

  if (byRetCode)
  {
    return;
  }


  _transform_siege_mode_other_inform_zocl circleMsg{};
  circleMsg.dwAvatorSerial = m_dwObjSerial;
  circleMsg.wWeaponItemIndex = m_Param.m_dbEquip.m_pStorageList[6].m_wItemIndex;
  circleMsg.bySiegeItemIndex = static_cast<unsigned __int8>(m_pSiegeItem->m_wItemIndex);
  circleMsg.wVisualVer = GetVisualVer();

  unsigned __int8 circleType[2] = {28, 3};
  CGameObject::CircleReport(circleType, reinterpret_cast<char *>(&circleMsg), sizeof(circleMsg), 0);
}

void CPlayer::SendMsg_ReleaseSiegeModeResult(char byRetCode)
{
  _release_siege_mode_result_zocl msg{};
  msg.byErrCode = byRetCode;

  unsigned __int8 type[2] = {28, 5};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));

  if (byRetCode)
  {
    return;
  }


  _release_siege_mode_other_inform_zocl circleMsg{};
  circleMsg.dwAvatorSerial = m_dwObjSerial;
  circleMsg.wVisualVer = GetVisualVer();

  unsigned __int8 circleType[2] = {28, 6};
  CGameObject::CircleReport(circleType, reinterpret_cast<char *>(&circleMsg), sizeof(circleMsg), 0);
}

void CPlayer::SetHaveEffect(char bLogin)
{
  if (!m_EP.m_pDataParam)
  {
    return;
  }

  const float prevHave4 = m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_4);
  const float prevHave77 = m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_77);

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
      _ResourceItem_fld *record = reinterpret_cast<_ResourceItem_fld *>(
        g_Main.m_tblItemData[item->m_byTableCode].GetRecord(item->m_wItemIndex));
      if (record && record->m_nStartTime != -1)
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

    _ResourceItem_fld *record = reinterpret_cast<_ResourceItem_fld *>(
      g_Main.m_tblItemData[item->m_byTableCode].GetRecord(item->m_wItemIndex));
    if (!record)
    {
      continue;
    }

    const int cashType = GetUsePcCashType(item->m_byTableCode, item->m_wItemIndex);
    if (IsUsableAccountType(cashType))
    {
      if (record->m_nStartTime == -1 || IsRidingUnit())
      {
        if (record->m_nEffType1 != 1)
        {
          for (int effIndex = 0; effIndex < record->m_nEffectDataNum; ++effIndex)
          {
            const int effectCode = record->m_EffectData[effIndex].nEffCode;
            if (effectCode > -1
                && (effectCode < 59 || effectCode > 65)
                && (!IsRidingUnit() || effectCode >= 29 || effectCode <= 32)
                && effectCode < 83
                && m_EP.m_pDataParam->m_fEff_Have[effectCode]
                  < record->m_EffectData[effIndex].fEffUnitMax)
            {
              const signed __int64 dur = static_cast<signed __int64>(item->m_dwDur);
              float durValue = static_cast<float>(static_cast<int>(dur));
              if (dur < 0)
              {
                durValue += 1.8446744e19f;
              }

              m_EP.m_pDataParam->m_fEff_Have[effectCode] +=
                record->m_EffectData[effIndex].fEffUnit * durValue;

              const float maxValue = record->m_EffectData[effIndex].fEffUnitMax;
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

  m_EP.m_pDataParam->m_fEff_Have[EFF_HAVE_UNKNOWN_0] += FLOAT_1_0;
  m_EP.m_pDataParam->m_fEff_Have[EFF_HAVE_UNKNOWN_2] += FLOAT_1_0;
  m_EP.m_pDataParam->m_fEff_Have[EFF_HAVE_UNKNOWN_5] += FLOAT_1_0;
  m_EP.m_pDataParam->m_fEff_Have[EFF_HAVE_UNKNOWN_6] += FLOAT_1_0;
  m_EP.m_pDataParam->m_fEff_Have[EFF_HAVE_UNKNOWN_7] += FLOAT_1_0;
  m_EP.m_pDataParam->m_fEff_Have[EFF_HAVE_UNKNOWN_8] += FLOAT_1_0;
  m_EP.m_pDataParam->m_fEff_Have[EFF_HAVE_UNKNOWN_9] += FLOAT_1_0;

  for (int effCode = 12; effCode < 83; ++effCode)
  {
    if ((effCode < 59 || effCode > 65) && m_EP.m_pDataParam->m_fEff_Have[effCode] != 0.0f)
    {
      if (effCode == 77)
      {
        float diff = (m_EP.m_pDataParam->m_fEff_Have[EFF_HAVE_UNKNOWN_77] - prevHave77) * 100.0f;
        apply_have_item_std_effect(77, m_EP.m_pDataParam->m_fEff_Have[EFF_HAVE_UNKNOWN_77], true, static_cast<int>(diff));
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

  m_EP.m_pDataParam->m_fEff_Have[EFF_HAVE_UNKNOWN_71] += FLOAT_1_0;
  m_EP.m_pDataParam->m_fEff_Have[EFF_HAVE_UNKNOWN_72] += FLOAT_1_0;
  m_EP.m_pDataParam->m_fEff_Have[EFF_HAVE_UNKNOWN_73] += FLOAT_1_0;
  m_EP.m_pDataParam->m_fEff_Have[EFF_HAVE_UNKNOWN_74] += FLOAT_1_0;
  m_EP.m_pDataParam->m_fEff_Have[EFF_HAVE_UNKNOWN_75] += FLOAT_1_0;

  const float afterHave4 = m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_4);
  if (afterHave4 <= prevHave4)
  {
    if (prevHave4 > m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_4))
    {
      m_bDownCheckEquipEffect = 1;
    }
  }
  else
  {
    m_bUpCheckEquipEffect = 1;
  }

  if (m_EP.m_pDataParam->m_fEff_Have[EFF_HAVE_UNKNOWN_2] > FLOAT_1_0)
  {
    float diff = (m_EP.m_pDataParam->m_fEff_Have[EFF_HAVE_UNKNOWN_2] - FLOAT_1_0) * 10.0f;
    const int count = static_cast<int>(diff) % 10;
    s_MgrItemHistory.exp_prof_log(count, m_szItemHistoryFileName);
  }
}

void CPlayer::apply_have_item_std_effect(int nEffCode, float fVal, bool bAdd, int nDiffCnt)
{
  switch (nEffCode)
  {
    case 15:
      m_EP.SetEff_Rate(EFF_RATE_MAX_HP, fVal, bAdd);
      break;
    case 16:
      m_EP.SetEff_Rate(EFF_RATE_MAX_FP, fVal, bAdd);
      break;
    case 17:
      m_EP.SetEff_Rate(EFF_RATE_MAX_SP, fVal, bAdd);
      break;
    case 18:
      m_EP.SetEff_Rate(EFF_RATE_CLOSE_RANGE_ATTACK, fVal, bAdd);
      m_EP.SetEff_Rate(EFF_RATE_RANGED_ATTACK, fVal, bAdd);
      m_EP.SetEff_Rate(EFF_RATE_CLOSE_RANGE_SKILL_ATTACK, fVal, bAdd);
      m_EP.SetEff_Rate(EFF_RATE_RANGED_SKILL_ATTACK, fVal, bAdd);
      m_EP.SetEff_Rate(EFF_RATE_FORCE_ATTACK, fVal, bAdd);
      m_EP.SetEff_Rate(EFF_RATE_LAUNCHER_ATTACK, fVal, bAdd);
      break;
    case 19:
      m_EP.SetEff_Rate(EFF_RATE_CLOSE_RANGE_ATTACK, fVal, bAdd);
      break;
    case 20:
      m_EP.SetEff_Rate(EFF_RATE_CLOSE_RANGE_SKILL_ATTACK, fVal, bAdd);
      break;
    case 21:
      m_EP.SetEff_Rate(EFF_RATE_RANGED_ATTACK, fVal, bAdd);
      break;
    case 22:
      m_EP.SetEff_Rate(EFF_RATE_RANGED_SKILL_ATTACK, fVal, bAdd);
      break;
    case 23:
      m_EP.SetEff_Rate(EFF_RATE_FORCE_ATTACK, fVal, bAdd);
      break;
    case 24:
      m_EP.SetEff_Rate(EFF_RATE_FINAL_DEFENSE, fVal, bAdd);
      break;
    case 25:
      m_EP.SetEff_Rate(EFF_RATE_DAMAGE_TO_HP, fVal, bAdd);
      break;
    case 26:
      m_EP.SetEff_Rate(EFF_RATE_DAMAGE_TO_FP, fVal, bAdd);
      break;
    case 27:
      m_EP.SetEff_Rate(EFF_RATE_UNKNOWN_40, fVal, bAdd);
      break;
    case 29:
      m_EP.SetEff_Rate(EFF_RATE_UNKNOWN_41, fVal, bAdd);
      break;
    case 30:
      m_EP.SetEff_Rate(EFF_RATE_UNKNOWN_42, fVal, bAdd);
      break;
    case 31:
      m_EP.SetEff_Rate(EFF_RATE_UNKNOWN_43, fVal, bAdd);
      break;
    case 32:
      m_EP.SetEff_Rate(EFF_RATE_UNKNOWN_44, fVal, bAdd);
      break;
    case 33:
      m_EP.SetEff_Rate(EFF_RATE_UNKNOWN_57, fVal, bAdd);
      break;
    case 34:
      m_EP.SetEff_Rate(EFF_RATE_UNKNOWN_58, fVal, bAdd);
      break;
    case 35:
      m_EP.SetEff_Rate(EFF_RATE_UNKNOWN_59, fVal, bAdd);
      break;
    case 36:
      m_EP.SetEff_Rate(EFF_RATE_UNKNOWN_60, fVal, bAdd);
      break;
    case 37:
      m_EP.SetEff_Rate(EFF_RATE_UNKNOWN_61, fVal, bAdd);
      break;
    case 38:
      m_EP.SetEff_Rate(EFF_RATE_CLOSE_RANGE_ATTACK, fVal, bAdd);
      m_EP.SetEff_Rate(EFF_RATE_RANGED_ATTACK, fVal, bAdd);
      break;
    case 39:
      m_EP.SetEff_Plus(EFF_PLUS_UNKNOWN_6, fVal, bAdd);
      m_EP.SetEff_Plus(EFF_PLUS_UNKNOWN_7, fVal, bAdd);
      break;
    case 40:
      m_EP.SetEff_Plus(EFF_PLUS_UNKNOWN_23, 1.0f, bAdd);
      break;
    case 41:
      m_EP.SetEff_Plus(EFF_PLUS_MOVE_RUN_SPEED, fVal, bAdd);
      break;
    case 43:
      m_EP.SetEff_Plus(EFF_PLUS_UNKNOWN_0, fVal, bAdd);
      break;
    case 44:
      m_EP.SetEff_Plus(EFF_PLUS_UNKNOWN_1, fVal, bAdd);
      break;
    case 45:
      m_EP.SetEff_Plus(EFF_PLUS_UNKNOWN_31, fVal, bAdd);
      break;
    case 46:
      m_EP.SetEff_Plus(EFF_PLUS_UNKNOWN_30, fVal, bAdd);
      break;
    case 47:
      m_EP.SetEff_Plus(EFF_PLUS_CRITICAL_RATE, fVal, bAdd);
      break;
    case 48:
      m_EP.SetEff_Plus(EFF_PLUS_ATTACK_ACCURACY, fVal, bAdd);
      break;
    case 49:
      m_EP.SetEff_Plus(EFF_PLUS_AVOID_RATE, fVal, bAdd);
      break;
    case 50:
      HideNameEffect(bAdd);
      break;
    case 56:
      m_EP.SetEff_State(EFF_STATE_UNKNOWN_8, bAdd);
      break;
    case 57:
      m_EP.SetEff_Plus(EFF_PLUS_SHIELD_BLOCK_RATE, fVal, bAdd);
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
      m_EP.SetEff_Plus(EFF_PLUS_UNKNOWN_22, fVal, bAdd);
      break;
    case 77:
      if (nDiffCnt > 0 && bAdd)
      {
        const float reduceValue = static_cast<float>(nDiffCnt) * 0.01f;
        DecHalfSFContDam(reduceValue);
      }
      break;
    case 78:
      m_EP.SetEff_Plus(EFF_PLUS_SHIELD_IGNORE_RATE, fVal, bAdd);
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
    m_EP.m_pDataParam->m_fEff_Have[EFF_HAVE_HIDE_NAME] = 0.0f;
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
              const float baseValue = record->m_fEffectUnitList[sameCount];
              float deltaValue = 0.0f;
              if (talikType)
              {
                if (talikType == 5 && item->m_byTableCode != 6)
                {
                  if (fCurVal > 0.0f && sameCount < static_cast<int>(fCurVal))
                  {
                    deltaValue = record->m_fEffectUnitList[static_cast<int>(fCurVal)] - baseValue;
                  }
                  m_EP.SetEff_Rate(EFF_RATE_FINAL_DEFENSE, deltaValue, bAdd);
                  m_fTalik_DefencePoint = m_fTalik_DefencePoint + deltaValue;
                }
              }
              else if (item->m_byTableCode == 6)
              {
                if (fCurVal > 0.0f && sameCount < static_cast<int>(fCurVal))
                {
                  deltaValue = record->m_fEffectUnitList[static_cast<int>(fCurVal)] - baseValue;
                }
                for (int paramIndex = 0; paramIndex < 2; ++paramIndex)
                {
                  m_EP.SetEff_Rate(paramIndex, deltaValue, bAdd);
                  m_EP.SetEff_Rate(paramIndex + 2, deltaValue, bAdd);
                }
                m_EP.SetEff_Rate(EFF_RATE_FORCE_ATTACK, deltaValue, bAdd);
                m_EP.SetEff_Rate(EFF_RATE_LAUNCHER_ATTACK, deltaValue, bAdd);
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
          effectValue = record->m_fEffectUnitList[static_cast<int>(fCurVal)];
          for (int paramIndex = 0; paramIndex < 2; ++paramIndex)
          {
            m_EP.SetEff_Rate(paramIndex, effectValue, bAdd);
            m_EP.SetEff_Rate(paramIndex + 2, effectValue, bAdd);
          }
          m_EP.SetEff_Rate(EFF_RATE_FORCE_ATTACK, effectValue, bAdd);
          m_EP.SetEff_Rate(EFF_RATE_LAUNCHER_ATTACK, effectValue, bAdd);
        }
        else if (fCurVal > 0.0f && fCurVal < 5.0f && item->m_byTableCode <= 5u)
        {
          _ItemUpgrade_fld *record = g_Main.m_tblItemUpgrade.GetRecord(5u);
          effectValue = record->m_fEffectUnitList[static_cast<int>(fCurVal)];
          m_EP.SetEff_Rate(EFF_RATE_FINAL_DEFENSE, effectValue, bAdd);
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
    updated = Emb_UpdateStat(79, newCum, oldCum);
    m_pmMst.UpdateCumPerMast(6u, 0, newCum);
    SendMsg_StatInform(79, updated + newCum, 0);
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

  _stat_inform_zocl msg{};
  msg.byStatIndex = byStatIndex;
  msg.dwNewStat = dwNewStat;
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
  _release_group_target_object_request_zocl msg{};
  msg.byGroupType = static_cast<char>(byGroupType);

  unsigned __int8 type[2] = {13, 109};
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
  const float rate = m_EP.GetEff_Rate(EFF_RATE_MAX_FP);
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
  const float rate = m_EP.GetEff_Rate(EFF_RATE_MAX_SP);
  const int maxSp = static_cast<int>(baseValue * rate);
  if (maxSp <= 0)
  {
    return 1;
  }
  return maxSp;
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
  sprintf_s(buffer, "%d", dwTemp);
  if (dwTemp && std::strlen(buffer) >= 9)
  {
    memset(token, 0, 4);
    std::memcpy(token, buffer, 4);
    tm->wYear = static_cast<unsigned __int16>(std::atoi(token));

    std::memset(token, 0, 4);
    std::memcpy(token, &buffer[4], 2);
    tm->wMonth = static_cast<unsigned __int16>(std::atoi(token));

    std::memset(token, 0, 4);
    std::memcpy(token, &buffer[6], 2);
    tm->wDay = static_cast<unsigned __int16>(std::atoi(token));

    std::memset(token, 0, 4);
    std::memcpy(token, &buffer[8], 1);
    tm->wHour = static_cast<unsigned __int16>(std::atoi(token));
    tm->wMinute = 0;
    tm->wSecond = 0;
  }
}

long double CPlayer::GetPvpPointLeak()
{
  return m_Param.m_dPvpPointLeak;
}

void CPlayer::AlterPvpPointLeak(long double dAlter)
{
  m_Param.m_dPvpPointLeak += dAlter;
  m_pUserDB->Update_PvpPointLeak(m_Param.m_dPvpPointLeak);
}

void CPlayer::SetPvpPointLeak(long double dValue)
{
  m_Param.m_dPvpPointLeak = dValue;
  m_pUserDB->Update_PvpPointLeak(dValue);
}

void CPlayer::SendMsg_ResurrectInform()
{
  unsigned __int8 type[2] = {3, 48};
  unsigned int msg = m_dwObjSerial;
  CircleReport(type, reinterpret_cast<char *>(&msg), sizeof(msg), false);
}

void CPlayer::SendMsg_Resurrect(char byRet, bool bQuickPotion)
{

  _player_resurrect_zocl msg{};
  msg.byRet = byRet;
  msg.byLevel = static_cast<char>(m_Param.GetLevel());
  msg.wCurFP = static_cast<unsigned __int16>(m_Param.GetFP());
  msg.wCurHP = static_cast<unsigned __int16>(m_Param.GetHP());
  msg.wCurST = static_cast<unsigned __int16>(m_Param.GetSP());
  msg.bQuickPotion = bQuickPotion;

  unsigned __int8 type[2] = {3, 40};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

_sf_continous *CPlayer::GetAfterEffect()
{
  if (!m_pUserDB)
  {
    return nullptr;
  }

  _SFCONT_DB_BASE *dbSfcont = &m_pUserDB->m_AvatorData.dbSfcont;
  _base_fld *record = g_Main.m_tblEffectData[3].GetRecord("17");
  if (!dbSfcont || !record)
  {
    return nullptr;
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
        cont->m_wDurSec = static_cast<unsigned __int16>(cont->m_wDurSec + order);
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

      if (!invalid)
      {
        return cont;
      }

      m_pUserDB->Update_SFContDelete(static_cast<unsigned __int8>(row), static_cast<unsigned __int8>(col));
      const char *charName = m_Param.GetCharNameA();
      g_Main.m_logSystemError.Write(
        "%s: error stored effect, code: %d, idx: %d: lv: %d",
        charName,
        cont->m_byEffectCode,
        cont->m_wEffectIndex,
        cont->m_byLv);
    }
  }

  return nullptr;
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

  _effect_start_inform_zocl msg{};
  msg.byLv = pCont->m_byLv;
  msg.wEffectCode = CalcEffectBit(pCont->m_byEffectCode, pCont->m_wEffectIndex);
  msg.wDurSec = pCont->m_wDurSec;

  unsigned __int8 type[2] = {17, 25};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_UnitAlterFeeInform(char bySlotIndex, unsigned int dwPullingFee)
{

  _unit_alter_fee_inform_zocl msg{};
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

  _unit_force_return_inform_zocl msg{};
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
    newDalant = MAX_DALANT;
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

void CPlayer::AlterMaxLevel(unsigned __int8 byMaxLevel)
{
  const int currentLevel = static_cast<int>(GetLevel());
  if (byMaxLevel <= currentLevel)
  {
    return;
  }

  const int currentMaxLevel = static_cast<int>(m_Param.GetMaxLevel());
  if (byMaxLevel <= currentMaxLevel)
  {
    return;
  }

  cStaticMember_Player *playerStatic = cStaticMember_Player::Instance();
  const int allowedMaxLv = playerStatic->GetMaxLv();
  if (byMaxLevel > allowedMaxLv)
  {
    return;
  }

  m_Param.SetMaxLevel(byMaxLevel);
  if (m_pUserDB)
  {
    m_pUserDB->Update_MaxLevel(byMaxLevel);
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

__int64 CPlayer::GetBillingType()
{
  if (m_pUserDB)
  {
    return static_cast<unsigned int>(m_pUserDB->GetBillingType());
  }
  return 0;
}

void CPlayer::pc_BillingInfoRequest()
{
// this is not a stub
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
      _CouponItem_fld *record = reinterpret_cast<_CouponItem_fld *>(
        g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex));
      if (record)
      {
        if (!record->m_nStoragePart)
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
m_nMaxPoint[0] = _CalcMaxHP();
  m_nMaxPoint[1] = _CalcMaxFP();
  m_nMaxPoint[2] = _CalcMaxSP();
}

__int16 CPlayer::_TowerReturn(_STORAGE_LIST::_db_con *pTowerItem)
{
  if (!pTowerItem)
  {
    return 0;
  }

  for (int index = 0; index < 6; ++index)
  {
    if (m_pmTwr.m_List[index].m_pTowerItem == pTowerItem)
    {
      m_pmTwr.m_List[index].m_pTowerItem->m_bLock = false;

      const int alterDur = static_cast<int>(m_pmTwr.m_List[index].m_pTowerObj->m_nHP)
        - static_cast<int>(m_pmTwr.m_List[index].m_pTowerItem->m_dwDur);
      const __int16 result = Emb_AlterDurPoint(
        0,
        m_pmTwr.m_List[index].m_pTowerItem->m_byStorageIndex,
        alterDur,
        false,
        false);

      m_pmTwr.m_List[index].m_pTowerObj->Destroy(0u, false);
      m_pmTwr.m_List[index].init();
      --m_pmTwr.m_nCount;
      return result;
    }
  }

  return 0;
}

void CPlayer::SendMsg_GuildMasterEffect(
  char byState,
  char byGrade,
  char byEffSubAttack,
  unsigned __int8 byEffSubDefence,
  unsigned __int8 byEffAddAttack,
  unsigned __int8 byEffAddDefence)
{

  _guild_master_info_zocl msg{};
  msg.byState = byState;
  msg.byGrade = byGrade;
  msg.byEffectValue[0] = byEffSubAttack;
  msg.byEffectValue[1] = byEffSubDefence;
  msg.byEffectValue[2] = byEffAddAttack;
  msg.byEffectValue[3] = byEffAddDefence;

  unsigned __int8 type[2] = {27, 120};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
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
  _notify_time_limit_panalty_status_zocl szMsg{};
  szMsg.byRetCode = byErrCode;
  unsigned __int8 pbyType[2];
  pbyType[0] = 13;
  pbyType[1] = static_cast<unsigned __int8>(-109);
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&szMsg),
    sizeof(szMsg));
}

void CPlayer::SendMsg_TLStatusInfo(unsigned int /*dwFatigue*/, unsigned __int8 /*wStatus*/)
{

  _notify_player_time_limit_Info_zocl msg{};
  msg.dwFatigue = this->m_pUserDB->m_AvatorData.dbTimeLimitInfo.dwFatigue;
  msg.wStatus = this->m_pUserDB->m_AvatorData.dbTimeLimitInfo.byTLStatus;

  const unsigned int playDegree = g_Main.m_pTimeLimitMgr->GetPlayFDegree();
  msg.dwAccumPlayTime = (playDegree / 1000) * msg.dwFatigue;

  const unsigned __int16 endPlayTime = g_Main.m_pTimeLimitMgr->GetEndPlayTime();
  if (msg.dwAccumPlayTime < 60u * endPlayTime)
  {
    msg.dwRemainTime = 60u * endPlayTime - msg.dwAccumPlayTime;
  }
  else
  {
    msg.dwRemainTime = 0;
  }

  unsigned __int8 pbyType[2] = {13, static_cast<unsigned __int8>(-113)};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    14);
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

bool CPlayer::IsTargetObj(CGameObject *pkObj)
{
  if (!pkObj || !m_TargetObject.pObject)
  {
    return false;
  }

  CGameObject *targetObject = m_TargetObject.pObject;
  if (targetObject->m_bLive && m_TargetObject.byKind == targetObject->m_ObjID.m_byKind
      && m_TargetObject.byID == targetObject->m_ObjID.m_byID
      && m_TargetObject.dwSerial == targetObject->m_dwObjSerial
      && m_TargetObject.pObject->m_pCurMap == m_pCurMap)
  {
    return m_TargetObject.byKind == pkObj->m_ObjID.m_byKind && m_TargetObject.byID == pkObj->m_ObjID.m_byID
           && m_TargetObject.dwSerial == pkObj->m_dwObjSerial;
  }

  m_TargetObject.init();
  return false;
}

bool CPlayer::IsInTown()
{
  return m_byPosRaceTown != static_cast<unsigned __int8>(-1);
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
            sprintf_s(nodeInfo->m_Name, "[AniMaster:%s]", animus->m_aszMasterName);
          }
        }
        else
        {
          CPlayer *player = static_cast<CPlayer *>(aggroCharacter);
          const char *name = player->m_Param.GetCharNameA();
          sprintf_s(nodeInfo->m_Name, "%s", name);
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
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&info),
    static_cast<unsigned __int16>(sizeof(info)));
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
  _alter_grade_inform_zocl msg{};
  msg.byGrade = static_cast<char>(m_Param.m_byPvPGrade);

  unsigned __int8 pbyType[2] = {17, 17};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_AlterPvPRank(unsigned __int16 wPvpRate, unsigned int dwPvpRank)
{
  _alter_ranking_inform_zocl msg{};

  msg.wRankRate = wPvpRate;
  msg.dwRank = dwPvpRank;

  unsigned __int8 pbyType[2] = {17, 18};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_PvpRankListVersionUp(char byVersion)
{
  _pvp_rank_list_version_up_zocl msg{};
  msg.byVersion = byVersion;

  unsigned __int8 pbyType[2] = {13, 24};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), sizeof(msg));
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
  std::memset(msg.wszSenderName, 0, sizeof(msg.wszSenderName));
  if (pwszSender)
  {
    strcpy_s(msg.wszSenderName, 17, pwszSender);
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
    msg.bySize = static_cast<unsigned __int8>(std::strlen(pwszMessage));
    std::memcpy(msg.wszChatData, pwszMessage, msg.bySize);
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
  std::memset(this, 0, sizeof(_STAT_DB_BASE));
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
      return byIndex < 48;
    case 4u:
      return byIndex < 24;
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
  return bySpecialPart == 255 ? 62u : 68u;
}

_other_shape_part_zocl::_other_shape_part_zocl()
{
  bySpecialPart = static_cast<unsigned __int8>(-1);
}

unsigned __int64 _other_shape_part_zocl::size()
{
  return bySpecialPart == 255 ? 36u : 42u;
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
  if (bySlotNum > 100)
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
  if (bySlotNum > 88)
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
  if (byLeftCuttingResNum > 20)
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
  if (bySlotNum > 30)
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
  if (bySlotNum > 70)
  {
    bySlotNum = 0;
  }
  return static_cast<unsigned __int64>(631 - 9LL * (70 - bySlotNum));
}

_unit_download_result_zocl::_list::_list()
{
  UnitData.Init(255);
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
  std::memset(this, 0, sizeof(_Init_action_point_zocl));
}

void CPlayer::SendMsg_ChangeClassCommand()
{
  _change_class_command_zocl szMsg{};
  szMsg.wCurClassIndex = static_cast<unsigned __int16>(this->m_Param.m_pClassData->m_dwIndex);

  unsigned __int8 pbyType[2] = {11, 5};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&szMsg),
    sizeof(szMsg));
}

void CPlayer::SendMsg_SelectClassResult(char byErrCode, unsigned __int16 wSelClassIndex)
{
  _select_class_result_zocl szMsg{};
  szMsg.byRetCode = byErrCode;

  unsigned __int8 type[2]{11, 7};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&szMsg), sizeof(szMsg));

  if (!byErrCode)
  {
    _select_class_result_other_inform_zocl circleMsg{};
    circleMsg.dwObjSerial = m_dwObjSerial;
    circleMsg.wSelClassIndex = wSelClassIndex;
    unsigned __int8 circleType[2]{17, 8};
    this->CircleReport(
      circleType,
      reinterpret_cast<char *>(&circleMsg),
      static_cast<unsigned __int16>(sizeof(circleMsg)),
      0);
  }
}

void CPlayer::SendMsg_PcRoomCharClass(unsigned int dwPcRoomClassIndex)
{
  _pc_room_charclass_info_zocl szMsg{};
  szMsg.dwPcRoomIndex = dwPcRoomClassIndex;

  unsigned __int8 type[2]{11, 32};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&szMsg), sizeof(szMsg));
}

void CPlayer::SendMsg_PcRoomError(char byErrorCode)
{
  _pc_room_favor_item_error_zocl szMsg{};
  szMsg.byErrorCode = byErrorCode;
  unsigned __int8 pbyType[2]{11, 34};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&szMsg), sizeof(szMsg));
}

void CPlayer::SendMsg_BillingExipreInform(char byKind, unsigned __int16 wWaitSec)
{
  _billing_expire_inform_zocl szMsg{};
  szMsg.byKind = byKind;
  szMsg.wWaitSec = wWaitSec;

  unsigned __int8 pbyType[2]{29, 5};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&szMsg), sizeof(szMsg));
}

void CPlayer::ReservationForceClose()
{
  s_BillingForceCloseDelay.Push(m_ObjID.m_wIndex, m_dwObjSerial);
}

void CPlayer::Billing_Logout()
{
  CNationSettingManager *manager = CTSingleton<CNationSettingManager>::Instance();
  unsigned __int16 delaySec = 0;
  if (manager)
  {
    delaySec = manager->GetBillingForceCloseDelay();
  }
  SendMsg_BillingExipreInform(0, delaySec);
  ReservationForceClose();
}

void CPlayer::PushDQSCheatPlyerVoteInfo()
{
  _qry_case_cheat_player_vote_info query{};
  query.dwAccountSerial = m_pUserDB->m_dwAccountSerial;
  query.dwCharSerial = m_Param.GetCharSerial();
  std::strcpy(query.wszCharName, m_Param.GetCharNameW());
  g_Main.PushDQSData(-1, nullptr, 151, reinterpret_cast<char *>(&query), query.size());
}

void CPlayer::PushDQSUpdateVoteAvilable()
{
  _qry_case_update_vote_available query{};
  query.byVoteEnable = 0;
  query.dwAccountSerial = m_pUserDB->m_dwAccountSerial;
  query.dwCharSerial = m_Param.GetCharSerial();
  std::strcpy(query.wszCharName, m_Param.GetCharNameW());

  g_Main.PushDQSData(-1, nullptr, 149, reinterpret_cast<char *>(&query), query.size());
}

void CPlayer::PushDQSUpdatePlyerVoteInfo()
{
  _qry_case_update_player_vote_info query{};
  query.dwAccumPlayTime = m_pUserDB->m_AvatorData.dbSupplement.dwAccumPlayTime;
  query.byIsVoted = m_pUserDB->m_AvatorData.dbSupplement.byVoted;
  query.byVoteEnable = m_pUserDB->m_AvatorData.dbSupplement.VoteEnable;
  query.wScaner = m_pUserDB->m_AvatorData.dbSupplement.wScanerCnt;

  char scanerDataBuffer[30]{};
  sprintf_s(
    scanerDataBuffer,
    sizeof(scanerDataBuffer),
    "%d%d",
    m_pUserDB->m_AvatorData.dbSupplement.dwScanerGetDate,
    m_pUserDB->m_AvatorData.dbSupplement.wScanerCnt);
  query.dwScanerData = _atoi64(scanerDataBuffer);
  query.dwAccountSerial = m_pUserDB->m_dwAccountSerial;
  query.dwCharSerial = m_Param.GetCharSerial();
  std::strcpy(query.wszCharName, m_Param.GetCharNameW());

  g_Main.PushDQSData(-1, nullptr, 150, reinterpret_cast<char *>(&query), query.size());
}

void CPlayer::SendMsg_Alter_Action_Point(unsigned __int8 byActCode, unsigned int dwActPoint)
{
  _alter_action_point_zocl msg{};
  msg.byActionCode = byActCode;
  msg.dwActionPoint = dwActPoint;

  unsigned __int8 pbyType[2]{11, 36};
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    msg.size());
}

char CPlayer::Update_GoldPoint(unsigned int dwPoint)
{
  CActionPointSystemMgr *actionPointMgr = CActionPointSystemMgr::Instance();
  if (actionPointMgr->GetEventStatus(2u) != 2)
  {
    return 0;
  }

  const unsigned int totalPoint = dwPoint + m_pUserDB->GetActPoint(2u);
  m_pUserDB->Update_User_Action_Point(2u, totalPoint);
  SendMsg_Alter_Action_Point(2u, totalPoint);
  return 1;
}

void CPlayer::Potion_Buf_Extend()
{
  m_PotionBufUse.UseBuffPotion(this);
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
  if (cost > m_Param.GetGold())
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
  if (m_Param.m_dbAnimus.GetNumUseCon() > 0)
  {
    return 5;
  }
  if (m_bAfterEffect)
  {
    return 11;
  }

  char prevClass[36]{};
  std::memcpy(prevClass, m_pUserDB->m_AvatorData.dbAvator.m_szClassCode, 5u);
  int oldMaxPoint[8]{};
  oldMaxPoint[0] = m_nMaxPoint[0];
  oldMaxPoint[1] = m_nMaxPoint[1];
  oldMaxPoint[2] = m_nMaxPoint[2];
  memset(&oldMaxPoint[3], 0, sizeof(int));

  m_Param.InitClass();
  CalcAddPointByClass();
  m_nAddDfnMstByClass = 0;
  RewardChangeClassMastery(m_Param.m_pClassData);
  m_pUserDB->InitClass(m_Param.m_pClassData->m_strCode);
  ReCalcMaxHFSP(false, false);
  _TowerAllReturn(3u, true);

  for (int j = 0; j < 20; ++j)
  {
    _TRAP_PARAM::_param *trap = &m_pmTrp.m_Item[j];
    if (trap->isLoad() && trap->pItem->m_dwObjSerial == trap->dwSerial)
    {
      trap->pItem->Destroy();
    }
  }

  m_fUnitPv_AttFc = FLOAT_1_0;
  m_fUnitPv_DefFc = FLOAT_1_0;
  m_fUnitPv_RepPr = FLOAT_1_0;
  RemoveAllContinousEffect();
  SendMsg_AlterMoneyInform(0);

  CPlayer::s_MgrItemHistory.InitClass(
    0,
    m_pUserDB->m_AvatorData.dbAvator.m_dwClassInitCnt,
    m_pUserDB->m_AvatorData.dbAvator.m_byLastClassGrade,
    prevClass,
    m_pUserDB->m_AvatorData.dbAvator.m_szClassCode,
    oldMaxPoint,
    m_nMaxPoint,
    m_szItemHistoryFileName);
  m_pUserDB->WriteLog_ChangeClassAfterInitClass(0, prevClass);
  m_bDownCheckEquipEffect = 1;
  return 0;
}

void CPlayer::CalcAddPointByClass()
{
  std::memset(m_nAddPointByClass, 0, sizeof(m_nAddPointByClass));
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
    m_Param.GetLevel(),
    m_Param.GetExp(),
    m_dwExpRate,
    m_Param.m_byPvPGrade,
    m_nMaxPoint,
    &m_pmMst,
    m_Param.m_dwAlterMastery,
    m_szLvHistoryFileName,
    0,
    nullptr);
  m_Param.InitAlterMastery();

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
    m_Param.GetLevel(),
    m_Param.GetExp(),
    m_dwExpRate,
    m_Param.m_byPvPGrade,
    m_nMaxPoint,
    &m_pmMst,
    m_Param.m_dwAlterMastery,
    m_szLvHistoryFileName,
    3u,
    pClassFld->m_strCode);
  m_Param.InitAlterMastery();
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
          g_Main.m_logSystemError.Write(
            "error bonus item of class change .., class: %s, error table code",
            pClassFld->m_strCode);
          continue;
        }
        if (tableCode == 19)
        {
          g_Main.m_logSystemError.Write(
            "error bonus item of class change .., class: %s, unit key",
            pClassFld->m_strCode);
          continue;
        }
        _base_fld *record = g_Main.m_tblItemData[tableCode].GetRecord(entry);
        if (!record)
        {
          g_Main.m_logSystemError.Write(
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

        if (m_Param.m_dbInven.GetIndexEmptyCon() == 255)
        {
          CreateItemBox(
            &rewardItem,
            this,
            -1,
            0,
            nullptr,
            3u,
            m_pCurMap,
            m_wMapLayerIndex,
            m_fCurPos,
            0);
          char clause[144]{};
          sprintf_s(clause, "Class G (%s)", pClassFld->m_strCode);
          CPlayer::s_MgrItemHistory.reward_add_item(
            m_ObjID.m_wIndex,
            clause,
            &rewardItem,
            m_szItemHistoryFileName);
        }
        else
        {
          rewardItem.m_wSerial = m_Param.GetNewItemSerial();
          if (Emb_AddStorage(0, &rewardItem, false, true))
          {
            SendMsg_RewardAddItem(&rewardItem, 1u);
            char clause[160]{};
            sprintf_s(clause, "Class (%s)", pClassFld->m_strCode);
            CPlayer::s_MgrItemHistory.reward_add_item(
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
    m_Param.GetLevel(),
    m_Param.GetExp(),
    m_dwExpRate,
    m_Param.m_byPvPGrade,
    m_nMaxPoint,
    &m_pmMst,
    m_Param.m_dwAlterMastery,
    m_szLvHistoryFileName,
    0,
    nullptr);
  m_Param.InitAlterMastery();

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
          g_Main.m_logSystemError.Write(
            "error bonus item of class change .., class: %s, error table code",
            pClassFld->m_strCode);
          continue;
        }
        if (tableCode == 19)
        {
          g_Main.m_logSystemError.Write(
            "error bonus item of class change .., class: %s, unit key",
            pClassFld->m_strCode);
          continue;
        }
        _base_fld *record = g_Main.m_tblItemData[tableCode].GetRecord(entry);
        if (!record)
        {
          g_Main.m_logSystemError.Write(
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

        if (m_Param.m_dbInven.GetIndexEmptyCon() == 255)
        {
          CreateItemBox(
            &rewardItem,
            this,
            -1,
            0,
            nullptr,
            3u,
            m_pCurMap,
            m_wMapLayerIndex,
            m_fCurPos,
            0);
          char clause[144]{};
          sprintf_s(clause, "Class G (%s)", pClassFld->m_strCode);
          CPlayer::s_MgrItemHistory.reward_add_item(
            m_ObjID.m_wIndex,
            clause,
            &rewardItem,
            m_szItemHistoryFileName);
        }
        else
        {
          rewardItem.m_wSerial = m_Param.GetNewItemSerial();
          if (Emb_AddStorage(0, &rewardItem, false, true))
          {
            SendMsg_RewardAddItem(&rewardItem, 1u);
            char clause[160]{};
            sprintf_s(clause, "Class (%s)", pClassFld->m_strCode);
            CPlayer::s_MgrItemHistory.reward_add_item(
              m_ObjID.m_wIndex,
              clause,
              &rewardItem,
              m_szItemHistoryFileName);
          }
          else
          {
            CPlayer::s_MgrItemHistory.add_storage_fail(
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
    m_Param.GetLevel(),
    m_Param.GetExp(),
    m_dwExpRate,
    m_Param.m_byPvPGrade,
    m_nMaxPoint,
    &m_pmMst,
    m_Param.m_dwAlterMastery,
    m_szLvHistoryFileName,
    3u,
    pClassFld->m_strCode);
  m_Param.InitAlterMastery();
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
  _class_fld *classRecord = (_class_fld *)g_Main.m_tblClass.GetRecord(wSelClassIndex);
  unsigned __int8 classSlot = 255;
  unsigned __int8 changeSlot = 255;
  bool rewardClassUp = false;

  if (m_pUserDB)
  {
    if (m_Param.IsClassChangeableLv())
    {
      if (classRecord)
      {
        for (int j = 0; j < 8 && std::strncmp(currentClass->m_strCh_Class[j], "-1", 2) != 0; ++j)
        {
          if (!std::strcmp(currentClass->m_strCh_Class[j], classRecord->m_strCode))
          {
            changeSlot = static_cast<unsigned __int8>(j);
            break;
          }
        }
        if (changeSlot == 255)
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
          if (classSlot == 255)
          {
            errCode = 3;
          }
          else
          {
            rewardClassUp = classSlot + 1 > m_pUserDB->m_AvatorData.dbAvator.m_byLastClassGrade;
            if (rewardClassUp)
            {
              if (bySelectRewardItem == 255)
              {
                if (classRecord->m_bSelectRewardItem)
                {
                  errCode = 4;
                }
                else
                {
                  const int rewardCount = GetRewardItemNumChangeClass(classRecord);
                  const int emptyCount = m_Param.m_dbInven.GetNumEmptyCon();
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
                else if (m_Param.m_dbInven.GetNumEmptyCon() < 1)
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
      std::memcpy(oldClass, m_pUserDB->m_AvatorData.dbAvator.m_szClassCode, 5u);
      int oldMaxPoint[8]{};
      oldMaxPoint[0] = m_nMaxPoint[0];
      oldMaxPoint[1] = m_nMaxPoint[1];
      oldMaxPoint[2] = m_nMaxPoint[2];
      memset(&oldMaxPoint[3], 0, sizeof(int));

      m_Param.SelectClass(classSlot, classRecord);
      if (m_Param.m_pClassData->m_dwIndex == 49
          && m_Param.m_pClassHistory[0]
          && m_Param.m_pClassHistory[0]->m_dwIndex == 45)
      {
        m_fUnitPv_AttFc = 1.1f;
        CNationSettingManager *manager = CTSingleton<CNationSettingManager>::Instance();
        manager->SetUnitPassiveValue(&m_fUnitPv_DefFc);
        m_fUnitPv_RepPr = 0.60000002f;
      }
      CalcAddPointByClass();
      m_nAddDfnMstByClass = 0;
      for (int j = 0; j < 4; ++j)
      {
        _class_fld *historyEffect = *m_Param.m_ppHistoryEffect[j];
        if (!historyEffect)
        {
          break;
        }
        m_nAddDfnMstByClass += historyEffect->m_nUpValueDefMastery;
      }
      if (rewardClassUp && g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) != 99)
      {
        RewardChangeClassMastery(classRecord);
        RewardChangeClassRewardItem(classRecord, bySelectRewardItem);
      }
      ReCalcMaxHFSP(false, false);
      m_pUserDB->Update_Class(
        classRecord->m_strCode,
        classSlot,
        static_cast<unsigned __int16>(m_Param.m_pClassHistory[classSlot]->m_dwIndex));
      if (rewardClassUp)
      {
        Emb_CreateQuestEvent(quest_happen_type_class, classRecord->m_strCode);
      }
      CPlayer::s_MgrItemHistory.ClassUP(
        classSlot + 1,
        m_pUserDB->m_AvatorData.dbAvator.m_byLastClassGrade,
        oldClass,
        m_pUserDB->m_AvatorData.dbAvator.m_szClassCode,
        oldMaxPoint,
        m_nMaxPoint,
        m_szItemHistoryFileName);
      m_pUserDB->WriteLog_ChangeClassAfterInitClass(1u, oldClass);
      m_bUpCheckEquipEffect = 1;
    }
    SendMsg_SelectClassResult(errCode, wSelClassIndex);
  }
}

unsigned __int8 CPlayer::pc_CanSelectClassRequest(bool *pIsRealClassUp)
{
  if (!m_Param.IsClassChangeableLv())
  {
    return 1;
  }

  unsigned __int8 nextSlot = 255;
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
  const int mapIndex = m_Param.GetMapCode();
  CMapData *map = g_MapOper.GetMap(mapIndex);
  if (!map)
  {
    return;
  }

  const int serial = map->GetMapCode();
  CItemStoreManager *manager = CItemStoreManager::Instance();
  CMapItemStoreList *storeList = manager->GetMapItemStoreListBySerial(serial);
  if (!storeList)
  {
    return;
  }

  for (int j = 0; j < storeList->m_nItemStoreNum; ++j)
  {
    CItemStore *store = &storeList->m_ItemStore[j];
    msg.StorePos[j].dwStoreIndex = store->m_pRec->m_dwIndex;
    std::memcpy(msg.StorePos[j].fPos, store->m_pDum->m_pDumPos->m_fCenterPos, sizeof(msg.StorePos[j].fPos));
  }
  msg.byStoreNum = static_cast<unsigned __int8>(map->m_nItemStoreDumNum);

  unsigned __int8 type[2]{12, 9};
  const unsigned __int16 len = msg.size();
  g_Network.m_pProcess[0]->LoadSendMsg( m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
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
    msg.dwDalant = m_Param.GetDalant();
    msg.dwGold = m_Param.GetGold();
    msg.dPoint = m_kPvpOrderView.GetPvpCash();
    for (int j = 0; j < 3; ++j)
    {
      msg.dwActPoint[j] = m_pUserDB->GetActPoint(static_cast<unsigned __int8>(j));
    }
    msg.byRetCode = byErrCode;

    unsigned __int8 type[2]{12, 3};
    g_Network.m_pProcess[0]->LoadSendMsg(
      m_ObjID.m_wIndex,
      type,
      reinterpret_cast<char *>(&msg),
      29);
    return;
  }

  _buy_store_success_zocl msg{};
  msg.dwLeftDalant = m_Param.GetDalant();
  msg.dwLeftGold = m_Param.GetGold();
  msg.dwConsumDanlant = pStore->GetLastTradeDalant();
  msg.dwConsumGold = pStore->GetLastTradeGold();
  msg.dwLeftPoint = static_cast<unsigned int>(m_kPvpOrderView.GetPvpCash());
  msg.dwConsumPoint = pStore->GetLastTradePoint();
  for (int j = 0; j < 3; ++j)
  {
    msg.dwConsumActPoint[j] = pStore->GetLastTradeActPoint(j);
  }
  for (int j = 0; j < 3; ++j)
  {
    msg.dwLeftActPoint[j] = m_pUserDB->GetActPoint(static_cast<unsigned __int8>(j));
  }
  msg.byDiscountRate = 100 * static_cast<unsigned __int8>(m_EP.GetEff_Have(EFF_HAVE_ITEM_STORE_DISCOUNT_RATE));
  msg.byBuyNum = static_cast<unsigned __int8>(nOfferNum);
  for (int j = 0; j < nOfferNum; ++j)
  {
    msg.OfferList[j].wSerial = pCard[j].wSerial;
    msg.OfferList[j].byCsMethod = pCard[j].Item.m_byCsMethod;
    msg.OfferList[j].dwT = pCard[j].Item.m_dwT;
  }

  unsigned __int8 type[2]{12, 2};
  const unsigned __int16 len = msg.size();
  g_Network.m_pProcess[0]->LoadSendMsg( m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
}

void CPlayer::SendMsg_SellItemStoreResult(CItemStore *pStore, unsigned __int8 byErrCode)
{
  _sell_store_result_zocl msg{};
  msg.bSucc = byErrCode == 0;
  msg.dwLeftDalant = m_Param.GetDalant();
  msg.dwLeftGold = m_Param.GetGold();
  msg.dwProfitDanlant = pStore->GetLastTradeDalant();
  msg.dwProfitGold = pStore->GetLastTradeGold();
  msg.byDiscountRate = 100 * static_cast<unsigned __int8>(m_EP.GetEff_Have(EFF_HAVE_ITEM_STORE_DISCOUNT_RATE));
  msg.byRetCode = byErrCode;

  unsigned __int8 type[2]{12, 5};
  g_Network.m_pProcess[0]->LoadSendMsg( m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 19);
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
  g_Network.m_pProcess[0]->LoadSendMsg( m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
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

  CTransportShip *ship = &g_TransportShip[m_Param.GetRaceCode()];
  unsigned __int8 resultCode = 0;
  unsigned __int8 buyRaceBossGoldBox = static_cast<unsigned __int8>(-1);
  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    resultCode = 23;
  }
  else if (bUseNPCLinkIntem || pStore->m_pExistMap == m_pCurMap)
  {
    const int npcRace = pStore->m_byNpcRaceCode;
    const int playerRace = m_Param.GetRaceCode();
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
          const int emptyCount = m_Param.m_pStoragePtr[j]->GetNumEmptyCon();
          if (emptyCount < storageCounts[j])
          {
            resultCode = 4;
            break;
          }
        }
        if (!resultCode)
        {
          for (int j = 0; j < byOfferNum; ++j)
          {
            offers[j].byGoodIndex = pList[j].dwGoodSerial;
            offers[j].byGoodAmount = pList[j].byAmount;
            offers[j].byStorageCode = pList[j].byStorageCode;
          }

          const float discount = m_EP.GetEff_Have(EFF_HAVE_ITEM_STORE_DISCOUNT_RATE) + m_EP.GetEff_Have(EFF_HAVE_ITEM_STORE_SELL_DISCOUNT_RATE);
          unsigned __int8 actCode[16]{};
          const int race = m_Param.GetRaceCode();
          unsigned int *actPoints = m_pUserDB->GetPtrActPoint();
          const double pvpCash = static_cast<double>(m_kPvpOrderView.GetPvpCash());
          const unsigned int hasGold = m_Param.GetGold();
          const unsigned int hasDalant = m_Param.GetDalant();
          resultCode = pStore->IsSell(
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
        }

        if (!resultCode)
        {
          ship->InitTicketReserver();
          for (int j = 0; j < byOfferNum; ++j)
          {
            if (offers[j].Item.m_byTableCode == 26)
            {
              _TrapItem_fld *record =
                reinterpret_cast<_TrapItem_fld *>(g_Main.m_tblItemData[26].GetRecord(offers[j].Item.m_wItemIndex));
              if (record && record->m_nType == 1)
              {
                const int raceCode = m_Param.GetRaceCode();
                CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
                const unsigned int bossSerial = rank->GetCurrentRaceBossSerial(raceCode, 0);
                const unsigned int charSerial = m_Param.GetCharSerial();
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
              const int raceSexCode = m_Param.GetRaceSexCode();
              if (civil[raceSexCode] != '1')
              {
                resultCode = 16;
                break;
              }
              _TicketItem_fld *record =
                reinterpret_cast<_TicketItem_fld *>(g_Main.m_tblItemData[28].GetRecord(offers[j].Item.m_wItemIndex));
              if (ship->GetLeftTicketIncludeReserNum(record->m_strMapCode, 1) <= 0)
              {
                resultCode = 16;
                break;
              }
            }

            if (offers[j].Item.m_byTableCode == 31)
            {
              _BoxItem_fld *record =
                reinterpret_cast<_BoxItem_fld *>(g_Main.m_tblItemData[31].GetRecord(offers[j].Item.m_wItemIndex));
              if (record && record->m_nClassGradeLim == 11 && !std::strcmp(record->m_strCode, "bxgol01"))
              {
                if (offers[j].byGoodAmount > 1u)
                {
                  offers[j].byGoodAmount = 1;
                }
                CGoldenBoxItemMgr *goldMgr = CGoldenBoxItemMgr::Instance();
                buyRaceBossGoldBox = goldMgr->IsBuyRaceBossGoldBox(this);
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
                  break;
                }
              }
              if (resultCode)
              {
                break;
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
                  break;
                }
              }
              if (resultCode)
              {
                break;
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
  if (!resultCode)
  {
    ship->ApplyTicketReserver();

    bool limitUpdated = false;
    for (int j = 0; j < byOfferNum; ++j)
    {
      _good_storage_info *good = &pStore->m_pStorageItem[offers[j].byGoodIndex];
      if (good->byType == 1)
      {
        pStore->SubLimitItemNum(good->dwLimitIndex, offers[j].byGoodAmount);
        limitUpdated = true;
      }
    }
    if (limitUpdated)
    {
      pStore->UpdateLimitItemNum(true);
    }

    SubDalant(pStore->GetLastTradeDalant());
    SubGold(pStore->GetLastTradeGold());
    SubPoint(static_cast<int>(pStore->GetLastTradePoint()));
    for (int j = 0; j < 3; ++j)
    {
      SubActPoint(static_cast<unsigned __int8>(j), pStore->GetLastTradeActPoint(j));
    }

    for (int j = 0; j < byOfferNum; ++j)
    {
      offers[j].wSerial = m_Param.GetNewItemSerial();
      offers[j].Item.m_wSerial = offers[j].wSerial;
      if (!Emb_AddStorage(pList[j].byStorageCode, &offers[j].Item, false, true))
      {
        SubDalant(-static_cast<int>(pStore->GetLastTradeDalant()));
        SubGold(-static_cast<int>(pStore->GetLastTradeGold()));
        SubPoint(-static_cast<int>(pStore->GetLastTradePoint()));
        int actPointIndex = 0;
        for (; actPointIndex < 3; ++actPointIndex)
        {
          SubActPoint(
            static_cast<unsigned __int8>(actPointIndex),
            -static_cast<int>(pStore->GetLastTradeActPoint(actPointIndex)));
        }
        SendMsg_BuyItemStoreResult(pStore, byOfferNum, offers, 17);
        // IDA uses the act-point loop index after completion; keep exact behavior.
        CPlayer::s_MgrItemHistory.add_storage_fail(
          m_ObjID.m_wIndex,
          &offers[actPointIndex].Item,
          " CPlayer::pc_BuyItemStore - Emb_AddStorage() Fail",
          m_szItemHistoryFileName);
        return;
      }

      if (!buyRaceBossGoldBox && offers[j].Item.m_byTableCode == 31)
      {
        _base_fld *record = g_Main.m_tblItemData[31].GetRecord(offers[j].Item.m_wItemIndex);
        if (!std::strcmp(record->m_strCode, "bxgol01"))
        {
          g_HolySys.SetGoldBoxConsumable(false);
          _STORAGE_LIST::_db_con *item = &offers[j].Item;
          char *charName = m_Param.GetCharNameA();
          const unsigned int charSerial = m_Param.GetCharSerial();
          SendMsg_Notify_Get_Golden_Box(1u, charSerial, charName, item, 0);
        }
      }
    }

    if (!m_byUserDgr)
    {
      if (pStore->GetLastTradeDalant() > 0)
      {
        const int addDalant = static_cast<int>(pStore->GetLastTradeDalant());
        const int raceCode = m_Param.GetRaceCode();
        eAddDalant(raceCode, addDalant);
      }
      if (pStore->GetLastTradeGold() > 0)
      {
        const int addGold = static_cast<int>(pStore->GetLastTradeGold());
        const int raceCode = m_Param.GetRaceCode();
        eAddGold(raceCode, addGold);
      }
    }

    const int level = m_Param.GetLevel();
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      if (pStore->GetLastTradeDalant() > 0)
      {
        const unsigned int amount = pStore->GetLastTradeDalant();
        char *classCode = m_Param.GetPtrCurClass()->m_strCode;
        const int curLevel = m_Param.GetLevel();
        const int raceCode = m_Param.GetRaceCode();
        CMoneySupplyMgr *moneyMgr = CMoneySupplyMgr::Instance();
        moneyMgr->UpdateBuyData(raceCode, curLevel, classCode, amount);
      }
      if (pStore->GetLastTradeGold() > 0)
      {
        const unsigned int amount = 2000 * pStore->GetLastTradeGold();
        char *classCode = m_Param.GetPtrCurClass()->m_strCode;
        const int curLevel = m_Param.GetLevel();
        const int raceCode = m_Param.GetRaceCode();
        CMoneySupplyMgr *moneyMgr = CMoneySupplyMgr::Instance();
        moneyMgr->UpdateBuyData(raceCode, curLevel, classCode, amount);
      }
    }

    const unsigned int newGold = m_Param.GetGold();
    const unsigned int newDalant = m_Param.GetDalant();
    const unsigned int costGold = pStore->GetLastTradeGold();
    const unsigned int costDalant = pStore->GetLastTradeDalant();
    CPlayer::s_MgrItemHistory.buy_item(
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
    pStore->GetLimitItemAmount(&amountInfo);
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
  int npcRace = 0;
  int playerRace = 0;
  int offerIndex = 0;
  float discount = 0.0f;
  unsigned __int8 race = 0;
  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    resultCode = 23;
  }
  else if (!bUseNPCLinkIntem && pStore->m_pExistMap != m_pCurMap)
  {
    resultCode = 1;
  }
  else
  {
    npcRace = pStore->m_byNpcRaceCode;
    playerRace = m_Param.GetRaceCode();
    if (npcRace != playerRace && pStore->m_byNpcRaceCode != 255)
    {
      resultCode = 2;
    }
    else if (!bUseNPCLinkIntem && GetSqrt(pStore->m_pDum->m_pDumPos->m_fCenterPos, m_fCurPos) > 100.0)
    {
      resultCode = 3;
    }
    else
    {
      for (offerIndex = 0; offerIndex < byOfferNum; ++offerIndex)
      {
        _STORAGE_LIST::_db_con *item =
          m_Param.m_pStoragePtr[pList[offerIndex].byStorageCode]->GetPtrFromSerial(pList[offerIndex].wSerial);
        offers[offerIndex].pItem = item;
        if (!offers[offerIndex].pItem)
        {
          resultCode = 8;
          break;
        }
        if (offers[offerIndex].pItem->m_byTableCode == 19)
        {
          resultCode = 9;
          break;
        }
        if (offers[offerIndex].pItem->m_bLock)
        {
          resultCode = 10;
          break;
        }
        for (int j = 0; j < offerIndex; ++j)
        {
          if (offers[j].pItem == offers[offerIndex].pItem)
          {
            resultCode = 11;
            break;
          }
        }
        if (resultCode)
        {
          break;
        }
        if (!IsSellItem(offers[offerIndex].pItem->m_byTableCode, offers[offerIndex].pItem->m_wItemIndex))
        {
          resultCode = 15;
          break;
        }
        offers[offerIndex].bySlotIndex = offers[offerIndex].pItem->m_byStorageIndex;
        offers[offerIndex].byStorageCode = pList[offerIndex].byStorageCode;
        if (IsOverLapItem(offers[offerIndex].pItem->m_byTableCode))
        {
          if (pList[offerIndex].byAmount > offers[offerIndex].pItem->m_dwDur)
          {
            resultCode = 12;
            break;
          }
          offers[offerIndex].byAmount = pList[offerIndex].byAmount;
        }
        else
        {
          offers[offerIndex].byAmount = 1;
        }
      }

      if (!resultCode)
      {
        discount = m_EP.GetEff_Have(EFF_HAVE_ITEM_STORE_DISCOUNT_RATE) + m_EP.GetEff_Have(EFF_HAVE_ITEM_STORE_BUY_DISCOUNT_RATE);
        race = static_cast<unsigned __int8>(m_Param.GetRaceCode());
        resultCode = pStore->IsBuy(byOfferNum, offers, discount, race);
        if (!resultCode)
        {
          if (pStore->GetLastTradeDalant() <= 0
              || CanAddMoneyForMaxLimMoney(static_cast<int>(pStore->GetLastTradeDalant()), m_Param.GetDalant()))
          {
            if (pStore->GetLastTradeGold() > 0)
            {
              if (!CanAddMoneyForMaxLimGold(static_cast<int>(pStore->GetLastTradeGold()), m_Param.GetGold()))
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
      }
    }
  }
  if (!resultCode)
  {
    AddDalant(static_cast<int>(pStore->GetLastTradeDalant()), true);
    AddGold(static_cast<int>(pStore->GetLastTradeGold()), true);

    const unsigned int newGold = m_Param.GetGold();
    const unsigned int newDalant = m_Param.GetDalant();
    const unsigned int incomeGold = pStore->GetLastTradeGold();
    const unsigned int incomeDalant = pStore->GetLastTradeDalant();
    CPlayer::s_MgrItemHistory.sell_item(
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
        AddGold(-static_cast<int>(pStore->GetLastTradeDalant()), true);
        AddGold(-static_cast<int>(pStore->GetLastTradeGold()), true);
        SendMsg_SellItemStoreResult(pStore, 255);
        return;
      }
}

    if (!m_byUserDgr)
    {
      if (pStore->GetLastTradeDalant() > 0)
      {
        const int addDalant = static_cast<int>(pStore->GetLastTradeDalant());
        const int raceCode = m_Param.GetRaceCode();
        eAddDalant(raceCode, addDalant);
      }
      if (pStore->GetLastTradeGold() > 0)
      {
        const int addGold = static_cast<int>(pStore->GetLastTradeGold());
        const int raceCode = m_Param.GetRaceCode();
        eAddGold(raceCode, addGold);
      }
    }

    const int level = m_Param.GetLevel();
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      const unsigned int amount = pStore->GetLastTradeDalant();
      char *classCode = m_Param.GetPtrCurClass()->m_strCode;
      const int curLevel = m_Param.GetLevel();
      const int raceCode = m_Param.GetRaceCode();
      CMoneySupplyMgr *moneyMgr = CMoneySupplyMgr::Instance();
      moneyMgr->UpdateSellData(raceCode, curLevel, classCode, amount);
    }
  }
  SendMsg_SellItemStoreResult(pStore, resultCode);
}

void CPlayer::pc_ExchangeDalantForGold(unsigned int dwDalant)
{
  unsigned __int8 resultCode = 0;
  const unsigned int curDalant = m_Param.GetDalant();
  const unsigned int curGold = m_Param.GetGold();
  const int rate = eGetRate(m_Param.GetRaceCode());
  const int exchangeDalant = static_cast<int>(dwDalant - dwDalant % rate);
  const unsigned int feeDalant = static_cast<unsigned int>(static_cast<float>(exchangeDalant) * 0.1f);
  const int addGold = exchangeDalant / rate;

  if (IsBeNearStore(this, 10))
  {
    const unsigned int totalDalant = feeDalant + exchangeDalant;
    if (totalDalant <= m_Param.GetDalant())
    {
      if (addGold)
      {
        const unsigned int gold = m_Param.GetGold();
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

    CPlayer::s_MgrItemHistory.exchange_money(
      m_ObjID.m_wIndex,
      curDalant,
      curGold,
      m_Param.GetDalant(),
      m_Param.GetGold(),
      m_szItemHistoryFileName);

    const unsigned int level = m_Param.GetLevel();
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      const int lv = m_Param.GetLevel();
      const int race = m_Param.GetRaceCode();
      CMoneySupplyMgr *moneyMgr = CMoneySupplyMgr::Instance();
      moneyMgr->UpdateFeeMoneyData(static_cast<unsigned __int8>(race), lv, feeDalant);
    }
  }
  SendMsg_ExchangeMoneyResult(resultCode);
}

void CPlayer::pc_ExchangeGoldForDalant(unsigned int dwGold)
{
  unsigned __int8 resultCode = 0;
  const unsigned int curGold = m_Param.GetGold();
  const unsigned int curDalant = m_Param.GetDalant();
  const int rate = eGetRate(m_Param.GetRaceCode());
  const unsigned int dalantTotal = rate * dwGold;
  const unsigned int feeDalant = static_cast<unsigned int>(static_cast<float>(dalantTotal) * 0.1f);
  const unsigned int addDalant = dalantTotal - feeDalant;

  if (IsBeNearStore(this, 10))
  {
    if (dwGold <= curGold)
    {
      if (addDalant)
      {
        const unsigned int dalant = m_Param.GetDalant();
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

    CPlayer::s_MgrItemHistory.exchange_money(
      m_ObjID.m_wIndex,
      curDalant,
      curGold,
      m_Param.GetDalant(),
      m_Param.GetGold(),
      m_szItemHistoryFileName);

    const unsigned int level = m_Param.GetLevel();
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      const int lv = m_Param.GetLevel();
      const int race = m_Param.GetRaceCode();
      CMoneySupplyMgr *moneyMgr = CMoneySupplyMgr::Instance();
      moneyMgr->UpdateFeeMoneyData(static_cast<unsigned __int8>(race), lv, feeDalant);
    }
  }
  SendMsg_ExchangeMoneyResult(resultCode);
}

void CPlayer::pc_ExchangeGoldForPvP(unsigned int dwGold)
{
  unsigned __int8 resultCode = 0;
  const double requiredPvPCash = static_cast<double>(static_cast<int>(200u * dwGold));
  if (requiredPvPCash > static_cast<double>(m_Param.GetPvPCashBag()))
  {
    resultCode = 1;
  }
  else if (dwGold)
  {
    const unsigned int currentGold = m_Param.GetGold();
    if (!CanAddMoneyForMaxLimGold(dwGold, currentGold))
    {
      resultCode = 2;
    }
  }

  if (!resultCode)
  {
    AlterPvPCashBag(-requiredPvPCash, pm_kill);
    AddGold(static_cast<int>(dwGold), true);
    s_MgrItemHistory.exchange_pvp_gold(m_ObjID.m_wIndex, dwGold, m_Param.GetDalant(), m_Param.GetGold(), m_szItemHistoryFileName);
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
  if (dwStoreIndex >= storeMgr->m_tblItemStore.GetRecordNum())
  {
    return;
  }

  CMapItemStoreList *storeList = nullptr;
  if (m_pCurMap->m_pMapSet->m_nMapType == 1)
  {
    if (m_Param.m_pGuild)
    {
      CGuildRoomSystem *room = CGuildRoomSystem::GetInstance();
      if (room->IsGuildRoomMemberIn(m_Param.m_pGuild->m_dwSerial, m_ObjID.m_wIndex, m_pUserDB->m_dwSerial))
      {
        CItemStoreManager *manager = CItemStoreManager::Instance();
        storeList = manager->GetInstanceStoreListBySerial(m_Param.m_pGuild->m_dwSerial);
      }
    }
  }
  else
  {
    const int serial = m_pCurMap->GetMapCode();
    CItemStoreManager *manager = CItemStoreManager::Instance();
    storeList = manager->GetMapItemStoreListBySerial(serial);
  }

  if (!storeList)
  {
    return;
  }

  CItemStore *store = storeList->GetItemStoreFromRecIndex(dwStoreIndex);
  if (!store)
  {
    return;
  }

  _limit_amount_info amountInfo{};
  store->GetLimitItemAmount(&amountInfo);
  SendMsg_StoreLimitItemAmountInfo(store->m_pRec->m_dwIndex, &amountInfo);
}

void CPlayer::pc_PvpCashRecorver(unsigned __int16 dwItemSerial, unsigned __int8 byItemCnt)
{
  unsigned __int8 resultCode = 0;
  _STORAGE_LIST::_db_con *item = m_Param.m_dbInven.GetPtrFromSerial(dwItemSerial);
  _base_fld *record = nullptr;
  if (item && item->m_dwDur >= byItemCnt)
  {
    record = g_Main.m_tblItemData[18].GetRecord(item->m_wItemIndex);
    if (record)
    {
      CPvpCashMng *cashMng = CPvpCashMng::Instance();
      if (!cashMng->IsTalikItem(record->m_strCode))
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
    m_kPvpCashPoint.SendMsg_RecoverResult(m_ObjID.m_wIndex, resultCode, 0);
    return;
  }

  const int recoverPoint = m_kPvpCashPoint.GetTalikRecvrPoint(item->m_byTableCode, record->m_dwIndex);
  int addedPoint = 0;
  int appliedCount = 0;
  const double pvpCash = static_cast<double>(m_kPvpOrderView.GetPvpCash());
  if (pvpCash >= 0.0)
  {
    resultCode = 2;
  }
  m_kPvpCashPoint.SendMsg_RecoverResult(m_ObjID.m_wIndex, resultCode, recoverPoint);
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
      CPlayer::s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex, item, m_szItemHistoryFileName);
    }
    else
    {
      AlterPvPCashBag(-0.0 - alter, pm_reward);
    }
  }
}

void CPlayer::SendMsg_RadarDelayTime(unsigned int dwDelay)
{
  _radar_delay_inform_zocl szMsg{};
  szMsg.dwDelay = dwDelay;

  unsigned __int8 pbyType[2] = {7, 53};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&szMsg),
    sizeof(szMsg));
}

void CPlayer::SendMsg_StartNewPos(char byMapInMode)
{
  _new_pos_start_result_zocl szMsg{};
  szMsg.byMapInMode = byMapInMode;

  unsigned __int8 pbyType[2] = {3, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&szMsg),
    sizeof(szMsg));
}

void CPlayer::SendMsg_ExitWorldResult(unsigned __int8 byRetCode)
{
  _exit_world_result_zocl szMsg{};
  szMsg.dummy[0] = static_cast<char>(byRetCode);

  unsigned __int8 pbyType[2] = {3, 42};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&szMsg),
    sizeof(szMsg));
}

void CPlayer::SendMsg_Revival(char byRet, bool bEquialZone)
{

  _player_revival_zocl msg{};
  msg.byRet = byRet;
  msg.byLevel = this->m_Param.GetLevel();
  msg.wCurFP = this->m_Param.GetFP();
  msg.wCurHP = this->m_Param.GetHP();
  msg.wCurST = this->m_Param.GetSP();
  msg.wStartMapIndex = g_MapOper.GetMap(this->m_pCurMap);
  FloatToShort(this->m_fCurPos, msg.zPos, 3);
  msg.byZoneCode = static_cast<unsigned __int8>(!bEquialZone);

  unsigned __int8 pbyType[2] = {3, 38};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    17);
}

void CPlayer::SendMsg_OtherShapeError(CPlayer *pDst, unsigned __int8 byErrCode)
{

  _other_shape_failure_zocl msg{};
  msg.wOtherIndex = this->m_ObjID.m_wIndex;
  msg.byErrCode = byErrCode;

  unsigned __int8 pbyType[2] = {3, 33};
  g_Network.m_pProcess[0]->LoadSendMsg(
    pDst->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    3u);
}

void CPlayer::SendMsg_OtherShapeAll(CPlayer *pDst)
{
  if (!CanSendPlayerViewMessage(this, pDst))
  {
    return;
  }

  if (this->m_bLive)
  {
    unsigned __int8 pbyType[2] = {3, 31};
    const unsigned __int16 nLen = static_cast<unsigned __int16>(this->m_bufShapeAll.size());
    g_Network.m_pProcess[0]->LoadSendMsg(
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

void CPlayer::SendMsg_OtherShapePart(CPlayer *pDst)
{
  if (!CanSendPlayerViewMessage(this, pDst))
  {
    return;
  }

  if (this->m_bLive)
  {
    unsigned __int8 pbyType[2] = {3, 32};
    const unsigned __int16 nLen = static_cast<unsigned __int16>(this->m_bufSpapePart.size());
    g_Network.m_pProcess[0]->LoadSendMsg(
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

void CPlayer::SendMsg_OtherShapePartEx_CashChange(
  CPlayer *pDst,
  CashChangeStateFlag ServerData,
  CashChangeStateFlag ClinetData)
{
  if ((!this->m_bObserver || pDst->m_byUserDgr)
      && (ServerData.m_byStateFlag & 7) != (ClinetData.m_byStateFlag & 7))
  {

    _other_shape_cash_rename_zocl msg{};
    msg.wIndex = this->m_ObjID.m_wIndex;
    msg.dwSerial = this->m_dwObjSerial;
    const char *charName = this->m_Param.GetCharNameW();
    std::strcpy(msg.wszName, charName);

    unsigned __int8 pbyType[2] = {3, 61};
    g_Network.m_pProcess[0]->LoadSendMsg(
      pDst->m_ObjID.m_wIndex,
      pbyType,
      reinterpret_cast<char *>(&msg),
      23);
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
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    12);
}

void CPlayer::SendMsg_BuyCashItemMode()
{
  _notify_buy_cashitem_mode szMsg{};
  CashItemRemoteStore *store = CashItemRemoteStore::Instance();
  szMsg.byMode = static_cast<char>(!store->_bIsBuyCashItemByGold);

  unsigned __int8 pbyType[2] = {59, 17};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&szMsg),
    sizeof(szMsg));
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
  msg.wHP = this->m_Param.GetHP();
  msg.wSP = this->m_Param.GetSP();
  msg.wFP = this->m_Param.GetFP();
  msg.wDP = this->m_Param.GetDP();
  msg.dPvpPoint = this->m_Param.GetPvPPoint();
  msg.dPvpCashBag = this->m_kPvpOrderView.GetPvpCash();
  msg.dPvpTempCash = m_kPvpOrderView.GetPvpTempCash();
  msg.dwPvpRank = this->m_Param.m_dbChar.m_dwRank;
  msg.byPvpClass = this->m_Param.m_byPvPGrade;
  msg.wMaxHP = this->m_nMaxPoint[0];
  msg.wMaxSP = this->m_nMaxPoint[2];
  msg.wMaxFP = this->m_nMaxPoint[1];
  msg.wMaxDP = static_cast<unsigned __int16>(GetMaxDP());

  const int mapCode = this->m_Param.GetMapCode();
  unsigned __int16 *record =
    reinterpret_cast<unsigned __int16 *>(g_MapOper.m_tblMapData.GetRecord(mapCode));
  msg.wMapIndex = *record;
  float *curPos = this->m_Param.GetCurPos();
  std::memcpy(msg.fPos, curPos, sizeof(msg.fPos));
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

  if (g_HolySys.GetHolyMasterRace() == -1)
  {
    if (g_HolySys.GetSceneCode() == 1)
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
    msg.byHolyMasterState = static_cast<unsigned __int8>(g_HolySys.GetHolyMasterRace());
  }

  msg.byPlusKey = this->m_byPlusKey;
  msg.wXorKey = this->m_wXorKey;

  if (this->m_Param.m_pGuild)
  {
    msg.dwGuildSerial = this->m_Param.m_pGuild->m_dwSerial;
    const int raceCode = this->m_Param.GetRaceCode();
    CHonorGuild *honorGuild = CHonorGuild::Instance();
    msg.byHonorGuildRank = honorGuild->FindHonorGuildRank(static_cast<unsigned __int8>(raceCode), msg.dwGuildSerial);
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
    const int adjustableGrade = effect->get_AdjustableGrade();
    const unsigned __int8 grade = this->m_Param.m_pGuild->GetGrade();
    if (adjustableGrade <= grade && this->m_Param.m_byClassInGuild == 2)
    {
      msg.byGuildGrade = this->m_Param.m_pGuild->GetGrade();
      effect = CGuildMasterEffect::GetInstance();
      msg.byEffectValue[0] = effect->get_AttactValueByGrade(msg.byGuildGrade);
      effect = CGuildMasterEffect::GetInstance();
      msg.byEffectValue[1] = effect->get_DefenceValueByGrade(msg.byGuildGrade);
    }
  }

  unsigned __int8 pbyType[2] = {3, 4};
  const unsigned __int16 nLen = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg(
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
  const int useSlot = this->m_Param.GetUseSlot();
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
  g_Network.m_pProcess[0]->LoadSendMsg(
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
  std::memcpy(&msg.Stat, &this->m_pmMst.m_BaseCum, sizeof(msg.Stat));

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
  g_Network.m_pProcess[0]->LoadSendMsg(
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
  g_Network.m_pProcess[0]->LoadSendMsg(
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
    if (quest->byQuestType != 255)
    {
      msg.SlotInfo[count].byDBSlotIndex = static_cast<unsigned __int8>(j);
      std::memcpy(&msg.SlotInfo[count].cont, quest, sizeof(msg.SlotInfo[count].cont));
      ++count;
    }
  }
  msg.bySlotNum = static_cast<unsigned __int8>(count);

  unsigned __int8 pbyType[2] = {3, 12};
  const unsigned __int16 nLen = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg(
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
    if (this->m_Param.m_QuestDB.m_History[j].byLevel != 255)
    {
      msg.SlotInfo[count].byIndex = static_cast<unsigned __int8>(j);
      std::strcpy(msg.SlotInfo[count++].szQuestCode, this->m_Param.m_QuestDB.m_History[j].szQuestCode);
    }
  }
  msg.bySlotNum = static_cast<unsigned __int8>(count);

  unsigned __int8 pbyType[2] = {3, 57};
  const unsigned __int16 nLen = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    nLen);
}

void CPlayer::SendMsg_SpecialDownloadResult()
{
  this->m_bSpecialDownload = 1;
  const unsigned int raceCode = static_cast<unsigned int>(this->m_Param.GetRaceCode());

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
      g_Network.m_pProcess[0]->LoadSendMsg(
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
      g_Network.m_pProcess[0]->LoadSendMsg(
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
      if (this->m_Param.m_UnitDB.m_List[j].byFrame != 255)
      {
        msg.UnitList[count].bySlotIndex = static_cast<unsigned __int8>(j);
        std::memcpy(
          &msg.UnitList[count].UnitData,
          &this->m_Param.m_UnitDB.m_List[j],
          sizeof(msg.UnitList[count].UnitData));
        ++count;
      }
    }
    msg.byUnitNum = static_cast<unsigned __int8>(count);

    unsigned __int8 pbyType[2] = {3, 14};
    const unsigned __int16 nLen = static_cast<unsigned __int16>(msg.size());
    g_Network.m_pProcess[0]->LoadSendMsg(
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
    if (key->IsFilled())
    {
      const unsigned __int16 code = key->GetCode();
      if (code == 4)
      {
        const unsigned __int16 index = key->GetIndex();
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
  std::memcpy(msg.m_dwSkill, this->m_pUserDB->m_AvatorData.dbLink.m_dwSkill, sizeof(msg.m_dwSkill));
  std::memcpy(msg.m_dwForce, this->m_pUserDB->m_AvatorData.dbLink.m_dwForce, sizeof(msg.m_dwForce));
  std::memcpy(msg.m_dwCharacter, this->m_pUserDB->m_AvatorData.dbLink.m_dwCharacter, sizeof(msg.m_dwCharacter));
  std::memcpy(msg.m_dwAnimus, this->m_pUserDB->m_AvatorData.dbLink.m_dwAnimus, sizeof(msg.m_dwAnimus));
  std::memcpy(msg.m_dwInvenBag, this->m_pUserDB->m_AvatorData.dbLink.m_dwInvenBag, 20);
  msg.m_dwInven = this->m_pUserDB->m_AvatorData.dbLink.m_dwInven;

  unsigned __int8 pbyType[2] = {3, 45};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    157);
}

void CPlayer::SendMsg_MacroRequest()
{
  _player_macro_result_zocl msg;
  _AIOC_A_MACRODATA *macro = &this->m_pUserDB->m_AvatorData.dbMacro;

  std::memset(&msg, 0, sizeof(msg));
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
      strncpy(msg.chatting[5 * j + k], macro->mcr_Chat[j].Chat[k], 81);
    }
  }

  unsigned __int8 pbyType[2] = {3, 50};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    954);
}

void CPlayer::SendMsg_AMPInvenDownloadResult()
{
  _personal_automine_download_zocl msg{};

  this->m_bAMPInvenDownload = 1;
  msg.byHaveBag = this->m_Param.BeHaveBoxOfAMP();

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
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    nLen);
}

void CPlayer::SendMsg_SetGroupTargetObjectResult(char byRetCode, char byGroupType)
{

  _set_group_target_object_result_zocl msg{};
  msg.byRetCode = byRetCode;

  const unsigned __int8 groupType = static_cast<unsigned __int8>(byGroupType);
  if (!byRetCode && this->m_GroupTargetObject[groupType].pObject)
  {
    msg.byGroupType = byGroupType;
    msg.byMapCode = static_cast<unsigned __int8>(this->m_GroupTargetObject[groupType].pObject->m_pCurMap->m_nMapCode);
    msg.byID = this->m_GroupTargetObject[groupType].byID;
    msg.dwSerial = this->m_GroupTargetObject[groupType].dwSerial;
    std::memcpy(msg.fPos, this->m_GroupTargetObject[groupType].pObject->m_fCurPos, sizeof(msg.fPos));
  }

  unsigned __int8 pbyType[2] = {13, 107};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    20);
}

void CPlayer::SendMsg_SetGroupMapPoint(
  char byRetCode,
  char byGroupType,
  char byMapCode,
  float *pzTar,
  unsigned __int8 byRemain)
{

  _set_group_map_point_result_zocl msg{};
  msg.byRetCode = byRetCode;
  msg.byGroupType = byGroupType;
  msg.byMapCode = byMapCode;
  msg.byRemain = static_cast<char>(byRemain);
  std::memcpy(msg.zPos, pzTar, sizeof(msg.zPos));

  unsigned __int8 pbyType[2] = {13, 114};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    12);
}

void CPlayer::pc_NewPosStart()
{
  unsigned __int8 resultCode = 0;
  if (this->m_bMapLoading)
  {
    if (this->m_byMapInModeBuffer == 255)
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
    const char *charName = this->m_Param.GetCharNameA();
    char buffer[144]{};
    sprintf_s(buffer, "Close.. %s: MapInMode(%u) Reason(%d)", charName, this->m_byMapInModeBuffer, resultCode);
    g_Network.Close(0, this->m_ObjID.m_wIndex, false, buffer);
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
      this->m_byMapInModeBuffer = 255;
    }

    CheckGroupTargeting();
    CheckGroupMapPoint();
    CNuclearBombMgr *manager = CNuclearBombMgr::Instance();
    manager->CheckNuclearState(this);
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

        const unsigned int guildMasterSerial = this->m_Param.m_pGuild->GetGuildMasterSerial();
        _guild_member_info *member = this->m_Param.m_pGuild->GetMemberFromSerial(guildMasterSerial);
        if (!member || !member->pPlayer)
        {
          continue;
        }
        groupPlayer = member->pPlayer;
      }
      else if (groupType == 2)
      {
        const int raceCode = this->m_Param.GetRaceCode();
        CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
        const unsigned int bossSerial = ranking->GetCurrentRaceBossSerial(raceCode, 0);
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
        this->m_GroupTargetObject[groupType].init();
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

        const unsigned int guildMasterSerial = this->m_Param.m_pGuild->GetGuildMasterSerial();
        _guild_member_info *member = this->m_Param.m_pGuild->GetMemberFromSerial(guildMasterSerial);
        if (!member || !member->pPlayer)
        {
          continue;
        }
        groupPlayer = member->pPlayer;
      }
      else if (groupType == 2)
      {
        const int raceCode = this->m_Param.GetRaceCode();
        CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
        const unsigned int bossSerial = ranking->GetCurrentRaceBossSerial(raceCode, 0);
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
        if (elapsed < 60000)
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

char CPlayer::Corpse(CCharacter *pAtter)
{
  Stop();
  DTradeInit();
  _sf_continous *afterEffect = GetAfterEffect();
  CCharacter::SFContInit();
  if (afterEffect && m_bAfterEffect)
  {
    bool upMastery = true;
    if (!InsertSFContEffect(
          0,
          3,
          afterEffect->m_wEffectIndex,
          afterEffect->m_wDurSec,
          1,
          &upMastery,
          reinterpret_cast<CPlayer *>(pAtter)))
    {
      if (afterEffect->m_wDurSec)
      {
        m_bAfterEffect = true;
      }
    }
  }

  if (m_bMineMode)
  {
    m_bMineMode = false;
    m_dwMineNextTime = static_cast<unsigned int>(-1);
    SendMsg_MineCancle();
  }

  if (m_pSiegeItem)
  {
    SetSiege(nullptr);
  }

  SetBreakTranspar(false);
  SendMsg_Die();

  unsigned __int16 jadeRevivalRate = static_cast<unsigned __int16>(m_EP.GetEff_Have(EFF_HAVE_JADE_REVIVAL_RATE) * 100.0f);
  if (jadeRevivalRate > 100)
  {
    jadeRevivalRate = 100;
  }
  if (jadeRevivalRate && static_cast<unsigned int>(rand() % 100) <= jadeRevivalRate)
  {
    SendMsg_RevivalOfJade(jadeRevivalRate);
  }

  CPlayer *killerOwner = nullptr;
  if (pAtter)
  {
    switch (pAtter->m_ObjID.m_byID)
    {
      case 0:
        killerOwner = static_cast<CPlayer *>(pAtter);
        break;
      case 3:
      {
        CAnimus *animus = static_cast<CAnimus *>(pAtter);
        killerOwner = animus->m_pMaster;
        break;
      }
      case 4:
      {
        CGuardTower *tower = static_cast<CGuardTower *>(pAtter);
        killerOwner = tower->m_pMasterTwr;
        break;
      }
      case 7:
        break;
      case 12:
      {
        CTrap *trap = static_cast<CTrap *>(pAtter);
        killerOwner = trap->m_pMaster;
        _skill_fld *effectRecord = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[3].GetRecord("17"));
        if (effectRecord)
        {
          bool upMastery = false;
          const unsigned __int8 contCode = static_cast<unsigned __int8>(effectRecord->m_nContEffectType);
          const unsigned __int16 durSec = static_cast<unsigned __int16>(effectRecord->m_nContEffectSec[0]);
          InsertSFContEffect(
            contCode,
            3,
            effectRecord->m_dwIndex,
            durSec,
            1,
            &upMastery,
            reinterpret_cast<CPlayer *>(pAtter));
          m_bAfterEffect = true;
        }
        break;
      }
      default:
        break;
    }

    if (killerOwner
        && m_bInGuildBattle
        && killerOwner->m_bInGuildBattle
        && m_byGuildBattleColorInx != killerOwner->m_byGuildBattleColorInx)
    {
      CGuildBattleController::Instance()->Kill(killerOwner, this);
    }

    if (!IsInTown() && !m_pCurMap->m_pMapSet->m_nMapType && m_Param.GetExp() > 0.0 && pAtter->m_ObjID.m_byID == 1)
    {
      CMonster *attackerMon = static_cast<CMonster *>(pAtter);
      if (attackerMon->m_bRobExp)
      {
        const int level = static_cast<int>(m_Param.GetLevel());
        if (level > 7 && level <= m_Param.GetMaxLevel() && (rand() % 2))
        {
          const int levelDiff = std::abs(level - static_cast<int>(pAtter->GetLevel()));
          float lossRate = static_cast<float>(levelDiff + (rand() % 5) + 1);
          if (lossRate > 20.0f)
          {
            lossRate = 20.0f;
          }
          lossRate *= 0.01f;

          const double oldExp = static_cast<double>(m_Param.GetExp());
          const double randomRate = static_cast<double>(rand() % 100) / 100.0;
          double lossExp = oldExp * static_cast<double>(lossRate) * randomRate;
          if (lossExp < 0.0)
          {
            lossExp = 0.0;
          }
          if (lossExp > oldExp)
          {
            lossExp = oldExp;
          }
          if (lossExp > 1.0)
          {
            AlterExp(-lossExp, false, false, false);
          }
        }
      }
    }

    if (killerOwner && !killerOwner->IsChaosMode())
    {
      const unsigned int now = timeGetTime();
      ++killerOwner->m_kPvpOrderView.m_nKillCnt;
      killerOwner->m_kPvpOrderView.m_dwLastAttackTime = now;
      killerOwner->m_kPvpOrderView.m_bAttack = true;
      killerOwner->m_kPvpOrderView.Notify_OrderView(killerOwner->m_ObjID.m_wIndex);

      ++m_kPvpOrderView.m_nDeahtCnt;
      m_kPvpOrderView.m_dwLastDamagedTime = now;
      m_kPvpOrderView.m_bDamaged = true;
      m_kPvpOrderView.Notify_OrderView(m_ObjID.m_wIndex);
    }
  }

  if (m_bInGuildBattle && m_bTakeGravityStone)
  {
    SendMsg_Notify_Gravity_Stone_Owner_Die();
    CGuildBattleController::Instance()->DropGravityStone(this);
  }

  if (m_pRecalledAnimusChar && !s_AnimusReturnDelay.Push(m_ObjID.m_wIndex, m_dwObjSerial))
  {
    _AnimusReturn(2u);
  }

  if (killerOwner && killerOwner->IsRidingUnit())
  {
    killerOwner->m_EP.SetLock(true);
    killerOwner->CalcDefTol();
    killerOwner->SetHaveEffect(false);
  }

  m_bCorpse = true;
  m_byModeType = 0;
  m_byMoveType = 1;
  m_byStandType = 0;
  m_dwSelfDestructionTime = 0;
  m_fSelfDestructionDamage = 0.0f;

  if (m_pCurMap->m_pMapSet->m_nMapType == 1)
  {
    m_dwNextTimeDungeonDie = GetLoopTime() + 5000;
  }

  if (m_byHSKTime <= 2 && !m_byCristalBattleDBInfo && m_pUserDB)
  {
    CHolyStone *holyStone = &g_Stone[m_Param.GetRaceCode()];
    if (holyStone->m_bOper && holyStone->m_pCurMap == m_pCurMap)
    {
      m_pUserDB->m_AvatorData.m_wDiePoint = ++m_wDiePoint;
    }
  }

  m_QuestMgr.CheckFailLoop(2, nullptr);
  SenseState();
  return 1;
}

void CPlayer::pc_Revival(bool bUseableJade)
{
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
        _dh_player_mgr *playerInfo = this->m_pDHChannel->GetPlayerInfo(this->m_dwObjSerial);
        if (playerInfo)
        {
          intoMap = playerInfo->LastPos.pMap;
          std::memcpy(revivePos, playerInfo->LastPos.fPos, sizeof(revivePos));
          this->m_pDHChannel->ClearMember(this, false, nullptr);
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
      manager->CheckNuclearState(this);
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
    g_Main.m_pTimeLimitMgr->Pop_Data(this->m_pUserDB->m_dwAccountSerial,
      this->m_id.wIndex);
  }

  this->m_bOper = 0;
  if (this->m_pmTrd.bDTradeMode)
  {
      CPlayer *dst = nullptr;
      if (DTradeEqualPerson(this, &dst))
      {
        dst->m_pmTrd.Init();
        dst->SendMsg_DTradeCloseInform(0);
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
    (void)controller->Start(this);
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

  if (this->m_pCurMap == g_TransportShip[static_cast<int>(this->m_Param.GetRaceCode())].m_pLinkShipMap)
  {
    const int raceCode = this->m_Param.GetRaceCode();
    g_TransportShip[raceCode].EnterMember(this);
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
  createData.m_pMap = g_MapOper.GetMap(this->m_Param.m_dbChar.m_sStartMapCode);
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
    g_Main.m_tblPlayer.GetRecord(this->m_Param.m_dbChar.m_byRaceSexCode);

  if (!CCharacter::Create(&createData))
  {
    const int posX = static_cast<int>(this->m_fCurPos[0]);
    const int posY = static_cast<int>(this->m_fCurPos[1]);
    const int posZ = static_cast<int>(this->m_fCurPos[2]);
    const char *mapCode = this->m_pCurMap->m_pMapSet->m_strCode;
    const char *charName = this->m_Param.GetCharNameA();
    char buffer[132]{};
    sprintf_s(buffer, "Close.. %s: Create() Map(%s) Pos(%d, %d, %d)", charName, mapCode, posX, posY, posZ);
    g_Network.Close(0, this->m_ObjID.m_wIndex, false, buffer);
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
  std::memcpy(this->m_fLastRecvPos, this->m_fCurPos, sizeof(this->m_fLastRecvPos));
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
  this->m_byNextRecallReturn = 255;
  this->m_wTimeFreeRecallSerial = 65535;
  this->m_pSiegeItem = nullptr;
  this->m_dwLastTakeItemTime = 0;

  this->m_tmrIntervalSec.TermTimeRun();
  this->m_tmrBilling.TermTimeRun();
  this->m_tmrGroupTargeting.TermTimeRun();
  if (IsApplyPcbangPrimium())
  {
    this->m_tmrPremiumPVPInform.TermTimeRun();
  }

  this->m_ReNamePotionUseInfo.Init();
  GetLocalTime(&this->m_tmLoginTime);
  GetLocalTime(&this->m_tmCalcTime);
  this->m_dwUMWHLastTime = GetLoopTime();

  const int currentMinuteSeconds = 60 * GetCurrentMin();
  const int currentSec = GetCurrentSec();
  this->m_tmrEffectStartTime.BeginTimerAddLapse(3600000, 1000 * (currentSec + currentMinuteSeconds));

  this->m_PotionParam.Init(this);
  if (this->m_Param.GetHP())
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

  this->m_QuestMgr.InitMgr(this, &this->m_Param.m_QuestDB);
  this->m_ItemCombineMgr.InitMgr(this);

  this->m_TargetObject.init();
  for (int j = 0; j < 3; ++j)
  {
    this->m_GroupTargetObject[j].init();
  }

  this->m_wPointRate_PartySend[0] = 10000;
  this->m_wPointRate_PartySend[1] = 10000;
  this->m_wPointRate_PartySend[2] = 10000;
  this->m_bMineMode = 0;
  this->m_dwMineNextTime = static_cast<unsigned int>(-1);

  _ATTACK_DELAY_CHECKER::Init(&this->m_AttDelayChker);
  for (int k = 0; k < 10; ++k)
  {
    if (this->m_pUserDB->m_AvatorData.dbSFDelay.EFF[k].byEffectCode != 255)
    {
      this->m_AttDelayChker.EFF[k].byEffectCode = this->m_pUserDB->m_AvatorData.dbSFDelay.EFF[k].byEffectCode;
      this->m_AttDelayChker.EFF[k].wEffectIndex = this->m_pUserDB->m_AvatorData.dbSFDelay.EFF[k].wEffectIndex;
      this->m_AttDelayChker.EFF[k].dwNextTime = nowTime + this->m_pUserDB->m_AvatorData.dbSFDelay.EFF[k].dwNextTime;
    }
    if (this->m_pUserDB->m_AvatorData.dbSFDelay.MAS[k].byEffectCode != 255)
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
  this->EquipItemSFAgent.Init(this);
  this->m_bSnowMan = 0;

  char *charNameW = this->m_Param.GetCharNameW();
  const unsigned int charSerial = this->m_Param.GetCharSerial();
  this->m_byBattleTournamentGrade =
    g_Main.m_BattleTournamentInfo.GetWinnerGrade(charSerial, charNameW);

  this->m_fUnitPv_AttFc = FLOAT_1_0;
  this->m_fUnitPv_DefFc = FLOAT_1_0;
  this->m_fUnitPv_RepPr = FLOAT_1_0;
  if (this->m_Param.m_pClassData->m_dwIndex == 49
      && this->m_Param.m_pClassHistory[0]
      && this->m_Param.m_pClassHistory[0]->m_dwIndex == 45)
  {
    this->m_fUnitPv_AttFc = FLOAT_1_1;
    CNationSettingManager *manager = CTSingleton<CNationSettingManager>::Instance();
    manager->SetUnitPassiveValue(&this->m_fUnitPv_DefFc);
    this->m_fUnitPv_RepPr = FLOAT_0_60000002;
  }

  std::memcpy(this->m_fBeforeDungeonPos, this->m_fCurPos, sizeof(this->m_fBeforeDungeonPos));
  this->m_pBeforeDungeonMap = this->m_pCurMap;

  if (this->m_pUserDB)
  {
    m_pUserDB->StartFieldMode();
    const unsigned int radarDelay = 1000 * this->m_pUserDB->m_AvatorData.dbAvator.m_dwRadarDelayTime;
    this->m_pUserDB->m_RadarItemMgr.Init(radarDelay);
    this->m_MinigTicket.SetLastMentalTicket(this->m_pUserDB->m_AvatorData.dbAvator.m_dwTakeLastMentalTicket);
    this->m_MinigTicket.SetLastCriTicket(this->m_pUserDB->m_AvatorData.dbAvator.m_dwTakeLastCriTicket);
    this->m_dwRaceBuffClearKey.Set(this->m_pUserDB->m_AvatorData.dbSupplement.dwRaceBuffClear);

    CPlayer::s_MgrItemHistory.GetNewFileName(this->m_pUserDB->m_dwSerial,
      this->m_szItemHistoryFileName);
    CPlayer::s_MgrLvHistory.GetNewFileName(this->m_pUserDB->m_dwSerial, this->m_szLvHistoryFileName);

    CPlayer::s_MgrItemHistory.have_item(this->m_ObjID.m_wIndex,
      this->m_Param.GetCharNameA(),
      &this->m_pUserDB->m_AvatorData,
      &this->m_pUserDB->m_AvatorData_bk,
      this->m_pUserDB->m_szAccountID,
      this->m_pUserDB->m_dwAccountSerial,
      this->m_byUserDgr,
      this->m_pUserDB->m_ipAddress,
      this->m_dwExpRate,
      true,
      this->m_szItemHistoryFileName);

    CPlayer::s_MgrLvHistory.start_mastery(
      this->m_ObjID.m_wIndex,
      this->m_Param.GetCharNameA(),
      this->m_Param.GetLevel(),
      this->m_Param.GetExp(),
      this->m_dwExpRate,
      this->m_Param.m_byPvPGrade,
      this->m_nMaxPoint,
      &this->m_pmMst,
      this->m_szLvHistoryFileName);

    m_pUserDB->WriteLog_CharSelect();
  }

  if (this->m_pUserDB->m_AvatorData.dbAvator.m_dwDalant
      != this->m_pUserDB->m_AvatorData_bk.dbAvator.m_dwDalant)
  {
    SendMsg_ExchangeMoneyResult(0);
  }

  CMoveMapLimitManager *moveMapLimit = CMoveMapLimitManager::Instance();
  moveMapLimit->LogIn(this);

  this->m_pBindMapData =
    g_MapOper.GetMap(this->m_pUserDB->m_AvatorData.dbAvator.m_szBindMapCode);
  if (this->m_pBindMapData)
  {
    this->m_pBindDummyData =
      reinterpret_cast<_dummy_position *>(this->m_pBindMapData->m_tbBindDumPos.GetRecord(
        this->m_pUserDB->m_AvatorData.dbAvator.m_szBindDummy));
    if (!this->m_pBindDummyData)
    {
      this->m_pBindMapData = nullptr;
    }
  }

  if (this->m_pUserDB && !this->m_pBindMapData)
  {
    char bindMapCode[] = "0";
    char bindDummyCode[] = "0";
    m_pUserDB->Update_Bind(bindMapCode, bindDummyCode, false);
  }

  SendMsg_EconomyHistoryInform();
  SendMsg_EconomyRateInform(1);

  ++CPlayer::s_nLiveNum;
  ++CPlayer::s_nRaceNum[static_cast<int>(this->m_Param.GetRaceCode())];

  _WA_AVATOR_CODE waCode{};
  std::memcpy(&waCode, &this->m_id, 6u);
  char *playerNameW = this->m_Param.GetCharNameW();
  std::strcpy(waCode.m_wszName, playerNameW);
  wa_EnterWorld(&waCode, this->m_ObjID.m_wIndex);

  _set_db_sf_effect(&this->m_pUserDB->m_AvatorData.dbSfcont);
  g_HolySys.SendMsg_HolyStoneSystemState(this->m_ObjID.m_wIndex);

  this->m_nAddDfnMstByClass = 0;
  for (int i = 0; i < 4; ++i)
  {
    _class_fld *effectRecord = *this->m_Param.m_ppHistoryEffect[i];
    if (!effectRecord)
    {
      break;
    }
    this->m_nAddDfnMstByClass += effectRecord->m_nUpValueDefMastery;
  }

  if (this->m_Param.m_pGuild && this->m_EP.GetEff_Have(EFF_HAVE_HIDE_NAME) <= 0.0)
  {
    _guild_member_info *member = this->m_Param.m_pGuild->LoginMember(this->m_dwObjSerial, this);
    if (member)
    {
      this->m_Param.m_pGuildMemPtr = member;
      this->m_Param.m_pGuild->SendMsg_GuildMemberLogin(this->m_dwObjSerial, this->m_wRegionMapIndex, this->m_wRegionIndex);

      CGuildBattleController *controller = CGuildBattleController::Instance();
      controller->LogIn(this);

      if (member->byClassInGuild == 2)
      {
        const unsigned __int8 raceCode = this->m_Param.GetRaceCode();
        controller->SendPossibleBattleGuildListFirst(this->m_ObjID.m_wIndex, raceCode);
      }

      if (member->byClassInGuild == 2)
      {
        const unsigned __int8 grade = this->m_Param.m_pGuild->GetGrade();
        CGuildMasterEffect *effect = CGuildMasterEffect::GetInstance();
        effect->in_player(this, grade);
      }
    }
    else
    {
      this->m_Param.m_pGuild = nullptr;
    }
  }

  if (!this->m_bInGuildBattle
      && this->m_pCurMap->m_pMapSet->m_nRaceVillageCode < 3
      && static_cast<unsigned int>(this->m_Param.GetRaceCode())
        != this->m_pCurMap->m_pMapSet->m_nRaceVillageCode
      && this->m_byUserDgr >= 2u)
  {
    this->m_bObserver = 1;
  }

  SetShapeAllBuffer();
  this->m_byDefMatCount = 0;
  this->m_dwLastSetPointTime = GetLoopTime();

  const int raceCode = this->m_Param.GetRaceCode();
  g_TransportShip[raceCode].SendMsg_TransportShipState(this->m_ObjID.m_wIndex);
  if (this->m_pCurMap == g_TransportShip[raceCode].m_pLinkShipMap)
  {
    g_TransportShip[raceCode].ReEnterMember(this);
  }

  if (!this->m_pUserDB->m_byUserDgr)
  {
    CBillingManager *billing = CTSingleton<CBillingManager>::Instance();
    billing->Login(this->m_pUserDB);
  }

  SenseState();
  this->m_nCheckMovePacket = 0;
  this->m_bCheckMovePacket = 0;
  g_HolySys.SendHolyStoneHP(this);
  this->m_byStoneMapMoveInfo = 0;
  this->m_dwPatriarchAppointTime = static_cast<unsigned int>(-1);
  this->m_byPatriarchAppointPropose = static_cast<unsigned __int8>(-1);
  this->m_byBattleMode = 0;
  this->m_dwBattleTime = 0;

  if (this->m_pUserDB)
  {
    CUserDB::s_MgrLobbyHistory.player_create(
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
  if (this->m_Param.IsClassChangeableLv())
  {
    SendMsg_ChangeClassCommand();
  }
  CalcEquipSpeed();
  m_kPvpOrderView.Notify_OrderView(this->m_ObjID.m_wIndex);

  for (int j = 0; j < 30; ++j)
  {
    _QUEST_DB_BASE::_LIST *quest = &this->m_Param.m_QuestDB.m_List[j];
    if (quest->byQuestType != 255)
    {
      bool isAllEmpty = true;
      for (int k = 0; k < 3; ++k)
      {
        if (quest->wNum[k] != 65535)
        {
          isAllEmpty = false;
          break;
        }
      }

      if (isAllEmpty)
      {
        _Quest_fld *record = reinterpret_cast<_Quest_fld *>(CQuestMgr::s_tblQuest->GetRecord(quest->wIndex));
        if (record && (record->m_bSelectQuestMenual || record->m_bSelectConsITMenual))
        {
          SendMsg_SelectQuestReward(j);
        }
        else
        {
          Emb_CompleteQuest(j, 255, 255);
        }
      }
    }
  }

  this->m_ItemCombineMgr.OnPlayerCreateCompleteProc();

  if (this->m_pUserDB->m_RadarItemMgr.GetDelayTime())
  {
    const unsigned int delay = this->m_pUserDB->m_RadarItemMgr.GetDelayTime();
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
      if (!this->m_pmTrp.PushItem(trap, trap->m_dwObjSerial))
      {
        break;
      }
      trap->MasterReStart(this);
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
      const unsigned __int8 bagNum = this->m_Param.GetBagNum();
      if (itemIndex < 20 * bagNum)
      {
        _STORAGE_LIST::_db_con *item = &this->m_Param.m_dbInven.m_pStorageList[itemIndex];
        if (item->m_bLoad && item->m_byTableCode == 25 && !item->m_bLock)
        {
          _GuardTowerItem_fld *record =
            reinterpret_cast<_GuardTowerItem_fld *>(g_Main.m_tblItemData[25].GetRecord(item->m_wItemIndex));
          if (record)
          {
            const int raceSexCode = this->m_Param.GetRaceSexCode();
            if (record->m_strCivil[raceSexCode] == '1')
            {
              const bool quick = entry->bComplete;
              const unsigned __int8 raceCode = this->m_Param.GetRaceCode();
              CGuardTower *tower =
                CreateGuardTower(entry->pMap, 0, entry->fPos, item, this, raceCode, quick);
              if (tower)
              {
                if (IsOtherTowerNear(this, entry->fPos, tower))
                {
                  tower->Destroy(0, 0);
                }
                else
                {
                  item->lock(1);
                  this->m_pmTwr.PushList(item, tower);
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

  const int raceCode = this->m_Param.GetRaceCode();
  CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
  if (ranking->IsCurrentRaceBossGroup(raceCode, this->m_dwObjSerial))
  {
    SendMsg_RaceTopInform(1);
  }

  if (this->m_bFirstStart)
  {
    const int level = GetLevel();
    char *levelStr = cvt_string(level);
    Emb_CreateQuestEvent(quest_happen_type_lv, levelStr);
    CGoldenBoxItemMgr *golden = CGoldenBoxItemMgr::Instance();
    if (golden->Get_Event_Status() == 2)
    {
      CGoldenBoxItemMgr *countMgr = CGoldenBoxItemMgr::Instance();
      if (countMgr->Get_StarterBox_Count())
      {
        bool inserted = false;
        for (int m = 0; m < 2; ++m)
        {
          CGoldenBoxItemMgr *boxMgr = CGoldenBoxItemMgr::Instance();
          char *itemCode = boxMgr->GetStarterBoxCode(m);
          CGoldenBoxItemMgr *insertMgr = CGoldenBoxItemMgr::Instance();
          if (insertMgr->StarterBox_InsertToInven(this, itemCode))
          {
            inserted = true;
          }
        }
        if (inserted)
        {
          CGoldenBoxItemMgr *setMgr = CGoldenBoxItemMgr::Instance();
          setMgr->Set_StarterBox_Count(1u, 0);
        }
      }
    }
  }

  CVoteSystem *voteSystem = &g_VoteSys[static_cast<int>(this->m_Param.GetRaceCode())];
  if (voteSystem->m_bActive)
  {
    if (voteSystem->m_bPunishment)
    {
      const unsigned int serial = this->m_Param.GetCharSerial();
      const int voteRace = this->m_Param.GetRaceCode();
      CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
      if (rank->GetBossType(voteRace, serial) == 255)
      {
        voteSystem->SendMsg_StartedVoteInform(this->m_ObjID.m_wIndex, this->m_dwObjSerial, 1);
      }
      else
      {
        voteSystem->SendMsg_StartedVoteInform(this->m_ObjID.m_wIndex, this->m_dwObjSerial, 0);
      }
    }
    else
    {
      voteSystem->SendMsg_StartedVoteInform(this->m_ObjID.m_wIndex, this->m_dwObjSerial, 0);
    }
  }

  if (g_HolySys.GetSceneCode() == 1)
  {
    _QUEST_CASH *questCash = g_HolySys.FindStoragedQuestCash(this->m_dwObjSerial);
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
      const unsigned __int8 numOfTime = g_HolySys.GetNumOfTime();
      const int randVal = rand();
      RecvHSKQuest(randVal % 2, 0, 0, 0, 0, numOfTime);
    }
  }

  for (int j = 0; j < 50; ++j)
  {
    _BUDDY_LIST::__list *buddy = &this->m_pmBuddy.m_List[j];
    if (buddy->fill())
    {
      for (int n = 0; n < MAX_PLAYER; ++n)
      {
        CPlayer *ptr = &g_Player[n];
        if (ptr->m_bLive
            && ptr != this
            && ptr->m_EP.GetEff_Have(EFF_HAVE_HIDE_NAME) <= 0.0
            && ptr->m_dwObjSerial == buddy->dwSerial)
        {
          if (ptr->m_pmBuddy.IsBuddy(this->m_dwObjSerial))
          {
            char *nameW = ptr->m_Param.GetCharNameW();
            buddy->ON(nameW, ptr);
            char *selfNameW = this->m_Param.GetCharNameW();
            ptr->m_pmBuddy.SearchBuddyLogin(this, this->m_dwObjSerial, selfNameW);
            ptr->SendMsg_BuddyLoginInform(this->m_dwObjSerial, this->m_wRegionMapIndex, this->m_wRegionIndex);
          }
          break;
        }
      }
    }
  }

  CUserDB *userDb = this->m_pUserDB;
  g_Main.m_pTimeLimitMgr->InsertPlayerStatus(this->m_id.wIndex,
    userDb->m_dwAccountSerial,
    userDb->m_AvatorData.dbTimeLimitInfo.byTLStatus,
    userDb->m_AvatorData.dbTimeLimitInfo.dwFatigue,
    userDb->m_AvatorData.dbTimeLimitInfo.dwLastLogoutTime,
    userDb->m_BillingInfo.bAgeLimit);
  g_Main.m_pTimeLimitMgr->CheckPlayerStatus(this->m_id.wIndex,
    this->m_pUserDB->m_AvatorData.dbTimeLimitInfo.dwLastLogoutTime,
    &this->m_pUserDB->m_AvatorData.dbTimeLimitInfo.byTLStatus,
    &this->m_pUserDB->m_AvatorData.dbTimeLimitInfo.dwFatigue);
  this->m_pUserDB->m_bDataUpdate = 1;
  SendMsg_TLStatusInfo(
    this->m_pUserDB->m_AvatorData.dbTimeLimitInfo.dwFatigue,
    this->m_pUserDB->m_AvatorData.dbTimeLimitInfo.byTLStatus);
  SendMsg_Init_Action_Point();

  CUnmannedTraderController *trader = CUnmannedTraderController::Instance();
  trader->CompleteCreate(this->m_ObjID.m_wIndex);

  CDarkHoleChannel *channel =
    g_DarkHoleQuest.SearchOncePlayedChannel(this->m_dwObjSerial);
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
      questCashOther = g_HolySys.PopStoredQuestCash_Other(this->m_dwObjSerial);
      if (questCashOther)
      {
        stoneMapMoveInfo = questCashOther->byStoneMapMoveInfo;
      }
      else
      {
        stoneMapMoveInfo = 1;
      }
      g_HolySys.SendMsg_NoticeNextQuest(this->m_ObjID.m_wIndex, stoneMapMoveInfo);
    }
    else if (g_HolySys.GetSceneCode() == 1)
    {
      questCashOther = g_HolySys.PopStoredQuestCash_Other(this->m_dwObjSerial);
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
    if (g_HolySys.bFreeMining && g_HolySys.IsMentalPass())
    {
      const unsigned __int8 numOfTime = g_HolySys.GetNumOfTime();
      const unsigned __int8 startHour = g_HolySys.GetStartHour();
      const unsigned __int8 startDay = g_HolySys.GetStartDay();
      const unsigned __int8 startMonth = g_HolySys.GetStartMonth();
      const unsigned __int16 startYear = g_HolySys.GetStartYear();
      UpdateLastCriTicket(startYear, startMonth, startDay, startHour, numOfTime);
    }
  }

  if (this->m_pUserDB->m_byUserDgr == 2 && this->m_bSpyGM)
  {
    char buffer[136]{};
    sprintf_s(buffer, "SPY GM !!");
    SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, buffer, 0xFFu, nullptr);
  }

  PatriarchElectProcessor *patriarch = PatriarchElectProcessor::Instance();
  patriarch->SendMsg_ConnectNewUser(this);
  SendMsg_GM_Greeting(g_Main.m_wszGMName, g_Main.m_wszMainGreetingMsg);

  char *raceGreeting = g_Main.m_wszRaceGreetingMsg[static_cast<int>(this->m_Param.GetRaceCode())];
  const int raceGreetingIndex = this->m_Param.GetRaceCode();
  SendMsg_RACE_Greeting(g_Main.m_wszBossName[raceGreetingIndex], raceGreeting);

  if (this->m_Param.m_pGuild && this->m_Param.m_pGuild->m_wszGreetingMsg[0])
  {
    char *guildMasterName = this->m_Param.m_pGuild->GetGuildMasterName();
    if (!guildMasterName)
    {
      guildMasterName = this->m_Param.m_pGuild->m_wszName;
    }
    SendMsg_GUILD_Greeting(guildMasterName, this->m_Param.m_pGuild->m_wszGreetingMsg);
  }

  const unsigned __int8 notifyRace = this->m_Param.GetRaceCode();
  g_Main.m_kEtcNotifyInfo.Notify(notifyRace, this->m_ObjID.m_wIndex);

  const int notifyTradeRace = this->m_Param.GetRaceCode();
  CUnmannedTraderController *tradeInfo = CUnmannedTraderController::Instance();
  tradeInfo->CompleteCreateNotifyTradeInfo(notifyTradeRace, this->m_ObjID.m_wIndex);

  CMoveMapLimitManager *moveMapLimit = CMoveMapLimitManager::Instance();
  moveMapLimit->CreateComplete(this);

  if (!IsUseReleaseRaceBuffPotion())
  {
    CRaceBuffManager *raceBuff = CRaceBuffManager::Instance();
    raceBuff->CreateComplete(this);
  }

  _qry_case_select_patriarch_comm query{};
  query.dwSerial = this->m_Param.GetCharSerial();
  const int querySize = query.size();
  g_Main.PushDQSData(-1, nullptr, 130, reinterpret_cast<char *>(&query), querySize);

  const int bossRace = this->m_Param.GetRaceCode();
  CPvpUserAndGuildRankingSystem *bossRank = CPvpUserAndGuildRankingSystem::Instance();
  if (bossRank->GetCurrentRaceBossSerial(bossRace, 0) == this->m_dwObjSerial)
  {
    const int honorRace = this->m_Param.GetRaceCode();
    CHonorGuild *honor = CHonorGuild::Instance();
    honor->SendInformChange(honorRace, this->m_ObjID.m_wIndex);
  }

  const int winRateRace = this->m_Param.GetRaceCode();
  CRaceBossWinRate *winRate = CRaceBossWinRate::Instance();
  winRate->Notify();

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
      const double pvpPoint = this->m_Param.GetPvPPoint();
      if (std::fabs(static_cast<double>(number)) > pvpPoint)
      {
        number = static_cast<int>(-pvpPoint);
      }
    }
    AlterPvPPoint(static_cast<double>(number), holy_dec, -1);
    SendMsg_RaceBattlePenelty(number, 0);
  }

  CashItemRemoteStore::Instance()->inform_cashdiscount_event(this->m_ObjID.m_wIndex);
  CashItemRemoteStore::Instance()->Inform_CashEvent(this->m_ObjID.m_wIndex);
  CashItemRemoteStore::Instance()->Inform_ConditionalEvent(this->m_ObjID.m_wIndex);

  if (IsOverOneDay() && !this->m_bFirstStart)
  {
    double pvpPoint = this->m_Param.GetPvPPoint();
    double alter = pvpPoint * 0.2000000029802322;
    if (pvpPoint < 0.0)
    {
      pvpPoint = 0.0;
    }
    AlterPvPPoint(-alter, logoff_dec, -1);
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
    const unsigned long long limitTime = GetConnectTime_AddBySec(-2592000);
    if (this->m_pUserDB->m_AvatorData_bk.dbAvator.m_dwLastConnTime < limitTime)
    {
      SendMsg_Notify_ExceptFromRaceRanking(1);
    }
  }

  this->m_QuestMgr.CheckFailLoop(2, nullptr);
  SendMsg_RemainOreRate();
  SendMsg_OreTransferCount();

  if (IsApplyPcbangPrimium())
  {
    this->m_kPcBangCoupon.Init(this->m_ObjID.m_wIndex);
  }

  if (this->m_Param.m_pGuild)
  {
    const unsigned int destroyerSerial = g_HolySys.GetDestroyerGuildSerial();
    if (destroyerSerial == static_cast<unsigned int>(-1))
    {
      SetLastAttBuff(0);
    }
    else
    {
      const unsigned int guildSerial = this->m_Param.GetGuildSerial();
      if (destroyerSerial != guildSerial)
      {
        SetLastAttBuff(0);
      }
    }
  }

  const unsigned int endTime = this->m_pUserDB->m_AvatorData.dbSupplement.dwBufPotionEndTime;
  if (endTime <= GetKorLocalTime())
  {
    this->m_PotionBufUse.SetExtPotionBufUse(0);
  }
  else
  {
    this->m_PotionBufUse.SetExtPotionBufUse(1);
    this->m_PotionBufUse.SetExtPotionEndTime(endTime);
    this->m_PotionBufUse.CalcRemainTime(this->m_ObjID.m_wIndex, 1);
  }

  CPcBangFavor *pcBang = CPcBangFavor::Instance();
  pcBang->PcBangDeleteItem(this);

  CPcBangFavor *pcBangEnable = CPcBangFavor::Instance();
  if (pcBangEnable->IsEnable())
  {
    if (IsApplyPcbangPrimium())
    {
      _AVATOR_DATA *data = &this->m_pUserDB->m_AvatorData;
      CPcBangFavor *parser = CPcBangFavor::Instance();
      parser->ClassCodePasing(data, this);
    }
  }

  if (this->m_pUserDB)
  {
    CUserDB::s_MgrLobbyHistory.player_create_complete_money(
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
      CUserDB::s_MgrLobbyHistory.player_money_fix(
        oldDalant,
        oldGold,
        &this->m_pUserDB->m_AvatorData,
        this->m_pUserDB->m_szLobbyHistoryFileName);
      SendMsg_ExchangeMoneyResult(0);
    }
  }

  CNationSettingManager *nation = CTSingleton<CNationSettingManager>::Instance();
  nation->CreateComplete(this);
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
          slot->wNum[k] = 65535;
        }
      }
      else
      {
        for (int k = 0; k < 3; ++k)
        {
          if (slot->wNum[k] != 65535)
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
        m_pUserDB->Update_QuestUpdate(node->byQuestDBSlot, slot, update);
      }

      if (isAllComplete)
      {
        _Quest_fld *record = reinterpret_cast<_Quest_fld *>(CQuestMgr::s_tblQuest->GetRecord(slot->wIndex));
        if (record->m_bSelectConsITMenual || record->m_bSelectQuestMenual)
        {
          SendMsg_SelectQuestReward(static_cast<char>(node->byQuestDBSlot));
        }
        else
        {
          Emb_CompleteQuest(node->byQuestDBSlot, 255, 255);
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
    if (m_Param.m_QuestDB.m_List[j].byQuestType == 255)
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
      m_pUserDB->Update_QuestDelete(static_cast<unsigned __int8>(slotIndex));
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
    slot->wNum[k] = 65535;
    if (quest->m_ActionNode[k].m_nActType != -1)
    {
      slot->wNum[k] = 0;
    }
    if (quest->m_ActionNode[k].m_nReqAct == -1)
    {
      slot->wNum[k] = 65535;
    }
  }
  m_pUserDB->Update_QuestInsert(static_cast<unsigned __int8>(slotIndex), slot);
  SendMsg_InsertNewQuest(static_cast<unsigned __int8>(slotIndex), slot);

  if (pHappenEvent->m_pEvent->m_nQuestType == 1)
  {
    for (int m = 0; m < 5; ++m)
    {
      if (pHappenEvent->m_pEvent->m_CondNode[m].m_nCondType == 8)
      {
        _STORAGE_LIST::_db_con *item =
          m_Param.m_dbInven.GetPtrFromItemCode(pHappenEvent->m_pEvent->m_CondNode[m].m_sCondVal);
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
              CPlayer::s_MgrItemHistory.delete_npc_quest_item(m_ObjID.m_wIndex,
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
            CPlayer::s_MgrItemHistory.delete_npc_quest_item(m_ObjID.m_wIndex,
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
  const long double penalty = g_Main.m_pTimeLimitMgr->GetPlayerPenalty(m_id.wIndex);

  if (pQuestFld->m_nMaxLevel != -1)
  {
    this->AlterMaxLevel(pQuestFld->m_nMaxLevel);
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
    const unsigned int sumGold = m_Param.GetGold();
    const unsigned int sumDalant = m_Param.GetDalant();
    CPlayer::s_MgrItemHistory.reward_add_money(m_ObjID.m_wIndex,
      "Quest",
      addDalant,
      addGold,
      sumDalant,
      sumGold,
      m_szItemHistoryFileName);

    const int level = m_Param.GetLevel();
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      if (pQuestFld->m_nConsDalant > 0)
      {
        const char *classCode = m_Param.GetPtrCurClass()->m_strCode;
        const int curLevel = m_Param.GetLevel();
        const int raceCode = m_Param.GetRaceCode();
        CMoneySupplyMgr *money = CMoneySupplyMgr::Instance();
        money->UpdateQuestRewardMoneyData(raceCode, curLevel, const_cast<char *>(classCode), pQuestFld->m_nConsDalant);
      }
      if (pQuestFld->m_nConsGold > 0)
      {
        const unsigned int amount = 2000 * pQuestFld->m_nConsGold;
        const char *classCode = m_Param.GetPtrCurClass()->m_strCode;
        const int curLevel = m_Param.GetLevel();
        const int raceCode = m_Param.GetRaceCode();
        CMoneySupplyMgr *money = CMoneySupplyMgr::Instance();
        money->UpdateQuestRewardMoneyData(raceCode, curLevel, const_cast<char *>(classCode), amount);
      }
    }
  }

  for (int j = 0; j < 6; ++j)
  {
    const char *itemCode = pQuestFld->m_RewardItem[j].m_strConsITCode;
    if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
    {
      break;
    }

    if (byRewardItemIndex != 255)
    {
      if (j != byRewardItemIndex)
      {
        continue;
      }
      if (pQuestFld->m_RewardItem[j].m_nLinkQuestIdx != -1)
      {
        linkedQuest = reinterpret_cast<_Quest_fld *>(
          CQuestMgr::s_tblQuest->GetRecord(pQuestFld->m_strLinkQuest[pQuestFld->m_RewardItem[j].m_nLinkQuestIdx]));
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
      g_Main.m_tblItemData[tableCode].GetRecord(itemCode));
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

    if (m_Param.m_dbInven.GetIndexEmptyCon() == 255)
    {
      if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
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
      sprintf_s(clause, "Quest G (%s)", pQuestFld->m_strCode);
      CPlayer::s_MgrItemHistory.reward_add_item(m_ObjID.m_wIndex,
        clause,
        &item,
        m_szItemHistoryFileName);
    }
    else
    {
      item.m_wSerial = m_Param.GetNewItemSerial();
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
        CPlayer::s_MgrItemHistory.add_storage_fail(m_ObjID.m_wIndex,
          &item,
          "CPlayer::_Reward_Quest() - Emb_AddStorage() Fail",
          m_szItemHistoryFileName);
        continue;
      }

      SendMsg_RewardAddItem(&item, 2);

      char clause[160]{};
      sprintf_s(clause, "Quest (%s)", pQuestFld->m_strCode);
      CPlayer::s_MgrItemHistory.reward_add_item(m_ObjID.m_wIndex,
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
      g_Main.m_tblEffectData[0].GetRecord(pQuestFld->m_strConsSkillCode));
    if (record)
    {
      Emb_AlterStat(3, *record, pQuestFld->m_nConsSkillCnt, 2, "CPlayer::_Reward_Quest()---1", true);
    }
  }

  if (std::strncmp(pQuestFld->m_strConsForceCode, "-1", 2) != 0)
  {
    _base_fld *record = g_Main.m_tblEffectData[1].GetRecord(pQuestFld->m_strConsSkillCode);
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
  _QUEST_DB_BASE::_LIST originalSlot{};
  std::memcpy(&originalSlot, slot, sizeof(originalSlot));

  if (originalSlot.byQuestType == 255)
  {
    return;
  }

  for (int j = 0; j < 3; ++j)
  {
    if (originalSlot.wNum[j] != 65535)
    {
      return;
    }
  }

  CPlayer::s_MgrLvHistory.update_mastery(
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
    0,
    nullptr);
  m_Param.InitAlterMastery();

  auto *quest = reinterpret_cast<_Quest_fld *>(CQuestMgr::s_tblQuest->GetRecord(originalSlot.wIndex));
  _Quest_fld *linkedQuest = _Reward_Quest(quest, byRewardItemIndex);
  SendMsg_QuestComplete(static_cast<char>(byQuestDBSlot));

  if (originalSlot.byQuestType == 1)
  {
    const unsigned __int8 inserted = m_QuestMgr.InsertNpcQuestHistory(
      quest->m_strCode,
      static_cast<char>(quest->m_nDifficultyLevel),
      quest->m_dRepeatTime);
    _QUEST_DB_BASE::_NPC_QUEST_HISTORY *history = &m_Param.m_QuestDB.m_History[inserted];
    m_pUserDB->Update_NPCQuestHistory(inserted, history);
    SendMsg_NpcQuestHistoryInform(static_cast<char>(inserted));
  }

  m_QuestMgr.DeleteQuestData(byQuestDBSlot);
  m_pUserDB->Update_QuestDelete(byQuestDBSlot);

  CPlayer::s_MgrLvHistory.update_mastery(
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
    2,
    quest->m_strCode);
  m_Param.InitAlterMastery();

  m_dwUMWHLastTime = GetLoopTime();
  if (quest->m_nLinkDummyCond == 1)
  {
    if (std::strcmp(quest->m_strLinkDummyCode, "-1") != 0)
    {
      _dummy_position *pos = m_pCurMap->GetDummyPostion(quest->m_strLinkDummyCode);
      if (pos)
      {
        float newPos[3]{};
        if (m_pCurMap->GetRandPosInDummy(pos, newPos, 1))
        {
          OutOfMap(m_pCurMap, m_wMapLayerIndex, 3, newPos);
          const unsigned __int8 mapCode = m_Param.GetMapCode();
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
      if (byLinkQuestIndex == 255)
      {
        linkIndex = static_cast<unsigned __int8>(rand() % linkCount);
      }
      linkedQuest = reinterpret_cast<_Quest_fld *>(CQuestMgr::s_tblQuest->GetRecord(quest->m_strLinkQuest[linkIndex]));
    }
  }

  if (linkedQuest)
  {
    _QUEST_DB_BASE::_LIST *nextSlot = &m_Param.m_QuestDB.m_List[byQuestDBSlot];
    nextSlot->byQuestType = originalSlot.byQuestType;
    nextSlot->wIndex = static_cast<unsigned __int16>(linkedQuest->m_dwIndex);
    nextSlot->dwPassSec = 0;
    for (int k = 0; k < 3; ++k)
    {
      if (linkedQuest->m_ActionNode[k].m_nActType != -1)
      {
        nextSlot->wNum[k] = 0;
      }
    }
    m_pUserDB->Update_QuestInsert(byQuestDBSlot, nextSlot);
    SendMsg_InsertNextQuest(byQuestDBSlot, nextSlot);
  }

  for (int m = 0; m < 3; ++m)
  {
    if (m_QuestMgr.m_pTempHappenEvent[m].isset())
    {
      std::memcpy(&m_QuestMgr.m_LastHappenEvent, &m_QuestMgr.m_pTempHappenEvent[m], sizeof(m_QuestMgr.m_LastHappenEvent));
      Emb_StartQuest(255, &m_QuestMgr.m_pTempHappenEvent[m]);
      if (m_QuestMgr.m_pTempHappenEvent[m].m_QtHpType == quest_happen_type_maxlevel)
      {
        m_Param.SetMaxLevel(50);
        if (m_pUserDB)
        {
          m_pUserDB->Update_MaxLevel(50);
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

  _adjust_amount_inform_zocl msg{};
  msg.byStorageCode = byStorageCode;
  msg.wSerial = wSerial;
  msg.dwNewDur = dwDur;

  unsigned __int8 pbyType[2] = {20, 3};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), 7u);
}

unsigned int CPlayer::GetMoney(unsigned __int8 byMoneyCode)
{
  if (byMoneyCode)
  {
    return m_Param.GetGold();
  }
  return m_Param.GetDalant();
}

CPlayer *CPlayer::FindFarChatPlayerWithTemp(char *pwszName)
{
  const unsigned __int8 nameLen = static_cast<unsigned __int8>(std::strlen(pwszName));
  CPlayer *dst = nullptr;

  for (int j = 0; j < 10; ++j)
  {
    _MEM_PAST_WHISPER *entry = &m_PastWhiper[j];
    if (entry->bMemory && entry->byNameLen == nameLen && !strncmp(entry->wszName, pwszName, nameLen))
    {
      if (entry->pDst
          && entry->pDst->m_bLive
          && entry->pDst->m_Param.m_byNameLen == nameLen
          && !strncmp(entry->pDst->m_Param.GetCharNameW(), entry->wszName, nameLen))
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
        if (!strncmp(candidate->m_Param.GetCharNameW(), pwszName, nameLen))
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
      std::strcpy(slot->wszName, dst->m_Param.GetCharNameW());
      slot->byNameLen = static_cast<unsigned __int8>(std::strlen(slot->wszName));
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
          && (byRaceCode == 0xFF || target->m_byUserDgr || target->m_Param.GetRaceCode() == byRaceCode))
      {
        target->SendData_ChatTrans(0, -1, byRaceCode, false, pwszChatData, 255, nullptr);
      }
    }
  }
}

void CPlayer::pc_ChatCircleRequest(char *pwszChatData)
{
  if (pwszChatData && pwszChatData[0] == '@')
  {
    if (pc_CustomCommand(&pwszChatData[1]))
    {
      return;
    }
  }

  if ((!m_pUserDB || !m_pUserDB->m_bChatLock)
      && m_pCurMap
      && GetCurSecNum() != -1
      && !m_bMapLoading)
  {
    _chat_message_receipt_udp msg{};
    msg.byMessageType = 1;
    msg.dwSenderSerial = m_dwObjSerial;
    msg.byRaceCode = m_Param.GetRaceCode();
    msg.bFiltering = false;
    msg.bySize = static_cast<unsigned __int8>(std::strlen(pwszChatData));
    std::memcpy(msg.wszChatData, pwszChatData, msg.bySize);
    msg.wszChatData[msg.bySize] = 0;
    std::strcpy(msg.wszSenderName, m_Param.GetCharNameW());
    msg.byPvpGrade = m_Param.m_byPvPGrade;

    _chat_message_receipt_udp filtered{};
    filtered.byMessageType = 1;
    filtered.dwSenderSerial = m_dwObjSerial;
    filtered.byRaceCode = m_Param.GetRaceCode();
    filtered.bFiltering = true;
    filtered.wszChatData[0] = 0;
    filtered.bySize = 0;
    std::strcpy(filtered.wszSenderName, m_Param.GetCharNameW());
    filtered.byPvpGrade = m_Param.m_byPvPGrade;

    unsigned __int8 type[2] = {2, 10};
    _sec_info *secInfo = m_pCurMap->GetSecInfo();
    const int curSec = GetCurSecNum();
    _pnt_rect rect{};
    m_pCurMap->GetRectInRadius(&rect, 3, curSec);

    CChatStealSystem *stealSystem = CChatStealSystem::Instance();
    stealSystem->StealChatMsg(this, 1u, pwszChatData);

    for (int y = rect.nStarty; y <= rect.nEndy; ++y)
    {
      for (int x = rect.nStartx; x <= rect.nEndx; ++x)
      {
        const unsigned int secIndex = secInfo->m_nSecNumW * y + x;
        CObjectList *list = m_pCurMap->GetSectorListPlayer(m_wMapLayerIndex, secIndex);
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
          else if (target->m_Param.GetRaceCode() == m_Param.GetRaceCode())
          {
            sendFull = true;
          }
          else if (target->m_byUserDgr >= 2u)
          {
            sendFull = true;
          }
          else if (target->m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_3) != 0.0)
          {
            sendFull = true;
          }
          else
          {
            const int raceCode = target->m_Param.GetRaceCode();
            CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
            const unsigned int bossSerial = rank->GetCurrentRaceBossSerial(raceCode, 0);
            if (bossSerial == target->m_dwObjSerial)
            {
              sendFull = true;
            }
          }

          if (sendFull)
          {
            const unsigned __int16 len = msg.size();
            g_Network.m_pProcess[0]->LoadSendMsg( target->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
          }
          else
          {
            const unsigned __int16 len = filtered.size();
            g_Network.m_pProcess[0]->LoadSendMsg(
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

bool CPlayer::pc_CustomCommand(char *pwszChatData)
{
  if (!pwszChatData || !pwszChatData[0])
  {
    return false;
  }

  char commandLine[272]{};
  strcpy_s(commandLine, sizeof(commandLine), pwszChatData);

  char *context = nullptr;
  char *command = strtok_s(commandLine, " ", &context);
  if (!command)
  {
    return false;
  }

  if (_stricmp(command, "guildhonor") == 0)
  {
    return pc_CustomCommand_GuildHonor(context);
  }

  return false;
}

bool CPlayer::pc_CustomCommand_GuildHonor(char *pwszCommandArgs)
{
  const unsigned int charSerial = m_Param.GetCharSerial();
  const int raceCode = m_Param.GetRaceCode();
  const unsigned int currentRaceBossSerial =
    CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(raceCode, 0);
  if (charSerial != currentRaceBossSerial)
  {
    return false;
  }

  char usageMessage[] = "guildhonor usage: @guildhonor <guild> <tax> [<guild> <tax> ...] (1-5 pairs)";
  char guildNameError[] = "guildhonor error: guild name must be 1-16 chars";
  char taxError[] = "guildhonor error: tax must be an integer between 0 and 50";
  char pairLimitError[] = "guildhonor error: supports at most 5 guild/tax pairs";
  _guild_honor_set_request_clzo request{};
  int pairCount = 0;
  char commandArgs[272]{};
  if (pwszCommandArgs && pwszCommandArgs[0])
  {
    strcpy_s(commandArgs, sizeof(commandArgs), pwszCommandArgs);
  }
  char *context = nullptr;

  for (; pairCount < 5; ++pairCount)
  {
    char *guildName = strtok_s(pairCount == 0 ? commandArgs : nullptr, " ", &context);
    if (!guildName)
    {
      break;
    }

    char *taxToken = strtok_s(nullptr, " ", &context);
    if (!taxToken)
    {
      SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, usageMessage, 0xFFu, nullptr);
      return true;
    }

    if (!guildName[0] || std::strlen(guildName) > 16)
    {
      SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, guildNameError, 0xFFu, nullptr);
      return true;
    }

    char *end = nullptr;
    const long taxRate = std::strtol(taxToken, &end, 10);
    if (end == taxToken || *end != 0 || taxRate < 0 || taxRate > 50)
    {
      SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, taxError, 0xFFu, nullptr);
      return true;
    }

    strcpy_s(request.GuildList[pairCount].wszGuildName, sizeof(request.GuildList[pairCount].wszGuildName), guildName);
    request.GuildList[pairCount].byTaxRate = static_cast<unsigned __int8>(taxRate);
  }

  if (pairCount == 0)
  {
    SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, usageMessage, 0xFFu, nullptr);
    return true;
  }

  if (strtok_s(nullptr, " ", &context))
  {
    SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, pairLimitError, 0xFFu, nullptr);
    return true;
  }

  request.byListNum = static_cast<unsigned __int8>(pairCount);
  pc_GuildSetHonorRequest(&request);
  return true;
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
          const int dstRace = dst->m_Param.GetRaceCode();
          const int myRace = m_Param.GetRaceCode();
          if (dstRace != myRace && dst->m_byUserDgr < 2u && dst->m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_3) == 0.0)
          {
            const int raceCode = dst->m_Param.GetRaceCode();
            CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
            const unsigned int bossSerial = rank->GetCurrentRaceBossSerial(raceCode, 0);
            if (bossSerial != dst->m_dwObjSerial)
            {
              filter = true;
            }
          }
        }

        char *senderName = m_Param.GetCharNameW();
        const unsigned __int8 senderRace = m_Param.GetRaceCode();
        dst->SendData_ChatTrans(
          chatType,
          m_dwObjSerial,
          senderRace,
          filter,
          pwszChatData,
          m_Param.m_byPvPGrade,
          senderName);

        char *dstName = dst->m_Param.GetCharNameW();
        const unsigned __int8 selfRace = m_Param.GetRaceCode();
        SendData_ChatTrans(chatType, m_dwObjSerial, selfRace, false, pwszChatData, m_Param.m_byPvPGrade, dstName);

        char buffer[288]{};
        sprintf_s(buffer, 272, dstName);

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
  if ((!m_pUserDB || (!m_pUserDB->m_bChatLock && !IsPunished(0, true))) && m_pPartyMgr->IsPartyMode())
  {
    _chat_message_receipt_udp msg{};
    msg.byMessageType = 3;
    msg.dwSenderSerial = m_dwObjSerial;
    msg.byRaceCode = m_Param.GetRaceCode();
    msg.bFiltering = false;
    msg.bySize = static_cast<unsigned __int8>(std::strlen(pwszChatData));
    std::memcpy(msg.wszChatData, pwszChatData, msg.bySize);
    msg.wszChatData[msg.bySize] = 0;
    std::strcpy(msg.wszSenderName, m_Param.GetCharNameW());
    msg.byPvpGrade = static_cast<unsigned __int8>(-1);

    unsigned __int8 type[2] = {2, 10};
    CPartyPlayer **members = m_pPartyMgr->GetPtrPartyMember();

    CChatStealSystem *stealSystem = CChatStealSystem::Instance();
    stealSystem->StealChatMsg(this, msg.byMessageType, pwszChatData);

    for (int j = 0; j < 8; ++j)
    {
      if (members[j])
      {
        const unsigned __int16 len = msg.size();
        g_Network.m_pProcess[0]->LoadSendMsg(
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
    const int raceCode = m_Param.GetRaceCode();
    CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
    if (rank->IsCurrentRaceBossGroup(raceCode, m_dwObjSerial))
    {
      _announ_message_receipt_udp msg{};
      msg.byMessageType = 4;
      msg.bySenderRace = m_Param.GetRaceCode();
      msg.dwSenderSerial = m_dwObjSerial;
      std::strcpy(msg.wszSenderName, m_Param.GetCharNameW());
      msg.bySize = static_cast<unsigned __int8>(std::strlen(pwszChatData));
      std::memcpy(msg.wszChatData, pwszChatData, msg.bySize);
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
              || target->m_Param.GetRaceCode() == m_Param.GetRaceCode())
          {
            g_Network.m_pProcess[0]->LoadSendMsg(
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
      && GetCurSecNum() != -1
      && !m_bMapLoading)
  {
    _chat_message_receipt_udp msg{};
    msg.byMessageType = 9;
    msg.dwSenderSerial = m_dwObjSerial;
    msg.byRaceCode = m_Param.GetRaceCode();
    msg.bFiltering = false;
    msg.bySize = static_cast<unsigned __int8>(std::strlen(pwszChatData));
    std::memcpy(msg.wszChatData, pwszChatData, msg.bySize);
    msg.wszChatData[msg.bySize] = 0;
    std::strcpy(msg.wszSenderName, m_Param.GetCharNameW());
    msg.byPvpGrade = m_Param.m_byPvPGrade;

    unsigned __int8 type[2] = {2, 10};
    CChatStealSystem *stealSystem = CChatStealSystem::Instance();
    stealSystem->StealChatMsg(this, msg.byMessageType, pwszChatData);

    const int len = msg.size();
    _sec_info *secInfo = m_pCurMap->GetSecInfo();
    for (int j = 0; j < secInfo->m_nSecNumH; ++j)
    {
      for (int k = 0; k < secInfo->m_nSecNumW; ++k)
      {
        const unsigned int secIndex = k + secInfo->m_nSecNumW * j;
        CObjectList *list = m_pCurMap->GetSectorListPlayer(m_wMapLayerIndex, secIndex);
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
          if (target->m_Param.GetRaceCode() == m_Param.GetRaceCode())
          {
            if (target->m_bRecvMapChat)
            {
              g_Network.m_pProcess[0]->LoadSendMsg(
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
    const int raceCode = m_Param.GetRaceCode();
    CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
    if (rank->IsCurrentRaceBossGroup(raceCode, m_dwObjSerial))
    {
      _announ_message_receipt_udp msg{};
      msg.byMessageType = 10;
      msg.bySenderRace = m_Param.GetRaceCode();
      msg.dwSenderSerial = m_dwObjSerial;
      std::strcpy(msg.wszSenderName, m_Param.GetCharNameW());
      msg.bySize = static_cast<unsigned __int8>(std::strlen(pwszChatData));
      std::memcpy(msg.wszChatData, pwszChatData, msg.bySize);
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
          if (target->m_Param.GetRaceCode() == m_Param.GetRaceCode())
          {
            const unsigned int serial = target->m_Param.GetCharSerial();
            const int myRace = m_Param.GetRaceCode();
            CPvpUserAndGuildRankingSystem *rankSys = CPvpUserAndGuildRankingSystem::Instance();
            if (rankSys->IsCurrentRaceBossGroup(myRace, serial))
            {
              g_Network.m_pProcess[0]->LoadSendMsg(
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
    msg.bySenderRace = m_Param.GetRaceCode();
    msg.dwSenderSerial = m_dwObjSerial;
    std::strcpy(msg.wszSenderName, m_Param.GetCharNameW());
    msg.bySize = static_cast<unsigned __int8>(std::strlen(pwszChatData));
    std::memcpy(msg.wszChatData, pwszChatData, msg.bySize);
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
      if (members[j].IsFill()
          && members[j].pPlayer
          && (members[j].byClassInGuild == 1 || members[j].byClassInGuild == 2))
      {
        CPlayer *target = members[j].pPlayer;
        g_Network.m_pProcess[0]->LoadSendMsg(
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
      const int raceCode = m_Param.GetRaceCode();
      CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
      if (rank->IsCurrentRaceBossGroup(raceCode, m_dwObjSerial)
          || (m_Param.m_byClassInGuild == 2 && m_Param.m_pGuild))
      {
        _announ_message_receipt_udp msg{};
        msg.byMessageType = 13;
        msg.bySenderRace = m_Param.GetRaceCode();
        msg.dwSenderSerial = m_dwObjSerial;
        std::strcpy(msg.wszSenderName, m_Param.GetCharNameW());
        msg.bySize = static_cast<unsigned __int8>(std::strlen(pwszChatData));
        std::memcpy(msg.wszChatData, pwszChatData, msg.bySize);
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
            if (target->m_Param.GetRaceCode() == m_Param.GetRaceCode())
            {
              const int myRace = m_Param.GetRaceCode();
              CPvpUserAndGuildRankingSystem *rankSys = CPvpUserAndGuildRankingSystem::Instance();
              if (rankSys->IsCurrentRaceBossGroup(myRace, target->m_dwObjSerial)
                  || (target->m_Param.m_byClassInGuild == 2 && target->m_Param.m_pGuild))
              {
                g_Network.m_pProcess[0]->LoadSendMsg(
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
  if (GetCurSecNum() == -1)
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
    item = m_Param.m_dbInven.GetPtrFromItemCode(g_Main.m_strAllRaceChatItemCode);
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
      }
      else
      {
        CPlayer::s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex, item, m_szItemHistoryFileName);
      }
    }
    else if (!Emb_DelStorage(0, item->m_byStorageIndex, 0, 1, "CPlayer::pcChatAllRequest()"))
    {
      return;
    }
    else
    {
      CPlayer::s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex, item, m_szItemHistoryFileName);
    }
  }
  if (g_Main.m_bAllRaceChatMoneyConsume)
  {
    SubDalant(g_Main.m_dwAllRaceChatMoney);
    SendMsg_AlterMoneyInform(0);
  }

  _announ_message_receipt_udp msg{};
  msg.byMessageType = 14;
  msg.dwSenderSerial = m_dwObjSerial;
  msg.bySenderRace = m_Param.GetRaceCode();
  std::strcpy(msg.wszSenderName, m_Param.GetCharNameW());
  msg.bySize = static_cast<unsigned __int8>(std::strlen(pwszChatData));
  std::memcpy(msg.wszChatData, pwszChatData, msg.bySize);
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
      if (target->m_Param.GetRaceCode() == m_Param.GetRaceCode())
      {
        g_Network.m_pProcess[0]->LoadSendMsg(
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
    g_Main.pc_AllUserGMNoticeInform(pwszChatData);
  }
}

void CPlayer::pc_ChatTradeRequestMsg(unsigned __int8 bySubType, char *pwszTradeMsg)
{
  if (m_pUserDB
      && !m_pUserDB->m_bChatLock
      && !IsPunished(0, true)
      && m_Param.GetDalant() >= 1000)
  {
    SubDalant(1000);
    SendMsg_AlterMoneyInform(0);

    const int level = m_Param.GetLevel();
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      const int lv = m_Param.GetLevel();
      const int race = m_Param.GetRaceCode();
      CMoneySupplyMgr *mgr = CMoneySupplyMgr::Instance();
      mgr->UpdateFeeMoneyData(static_cast<unsigned __int8>(race), lv, 1000);
    }

    _announ_message_receipt_udp msg{};
    msg.byMessageType = 19;
    msg.dwSenderSerial = m_dwObjSerial;
    msg.bySenderRace = m_Param.GetRaceCode();
    std::strcpy(msg.wszSenderName, m_Param.GetCharNameW());
    msg.bySize = static_cast<unsigned __int8>(std::strlen(pwszTradeMsg));
    std::memcpy(msg.wszChatData, pwszTradeMsg, msg.bySize);
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
        if (target->m_Param.GetRaceCode() == m_Param.GetRaceCode())
        {
          g_Network.m_pProcess[0]->LoadSendMsg(
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
      _guild_member_info *member = m_Param.m_pGuild->GetMemberFromSerial(dwDstSerial);
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
    g_Network.m_pProcess[0]->LoadSendMsg( m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), msg.size());
  }
  else
  {
    _announ_message_receipt_udp msg{};
    msg.bySenderRace = m_Param.GetRaceCode();
    msg.dwSenderSerial = m_dwObjSerial;
    std::strcpy(msg.wszSenderName, m_Param.GetCharNameW());
    msg.byPvpGrade = m_Param.m_byPvPGrade;
    msg.bySize = static_cast<unsigned __int8>(std::strlen(pwszChatData));
    std::memcpy(msg.wszChatData, pwszChatData, msg.bySize);
    msg.wszChatData[msg.bySize] = 0;

    unsigned __int8 type[2] = {2, 11};
    const int baseLen = msg.size();

    if (dstPlayer)
    {
      msg.byMessageType = 8;
      const unsigned __int16 len = msg.size();
      g_Network.m_pProcess[0]->LoadSendMsg(
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
        if (member->IsFill())
        {
          if (member->pPlayer)
          {
            g_Network.m_pProcess[0]->LoadSendMsg(
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
              && dst->m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_3) <= 0.0)
          {
            const int raceCode = dst->m_Param.GetRaceCode();
            CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
            const unsigned int bossSerial = rank->GetCurrentRaceBossSerial(raceCode, 0);
            if (bossSerial != dst->m_dwObjSerial)
            {
              const int dstRace = dst->m_Param.GetRaceCode();
              const int myRace = m_Param.GetRaceCode();
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
      std::strcpy(fail.wszDstName, pDstName[j].name);
      fail.byFailCode = static_cast<unsigned __int8>(failCode);
      unsigned __int8 type[2] = {2, 103};
      g_Network.m_pProcess[0]->LoadSendMsg(
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

    char *senderName = m_Param.GetCharNameW();
    unsigned __int8 senderLen = static_cast<unsigned __int8>(std::strlen(senderName));
    std::memcpy(&sData[writePos++], &senderLen, sizeof(senderLen));
    std::memcpy(&sData[writePos], senderName, senderLen);
    writePos += senderLen;

    std::memcpy(&sData[writePos++], &dstCount, sizeof(dstCount));
    for (int m = 0; m < dstCount; ++m)
    {
      char *dstName = targets[m]->m_Param.GetCharNameW();
      unsigned __int8 dstLen = static_cast<unsigned __int8>(std::strlen(dstName));
      std::memcpy(&sData[writePos++], &dstLen, sizeof(dstLen));
      std::memcpy(&sData[writePos], dstName, dstLen);
      writePos += dstLen;
    }

    unsigned __int8 msgLen = static_cast<unsigned __int8>(std::strlen(pwszMsg) + 1);
    std::memcpy(&sData[writePos++], &msgLen, sizeof(msgLen));
    std::memcpy(&sData[writePos], pwszMsg, msgLen);
    writePos += msgLen;

    msg.wSize = static_cast<unsigned __int16>(writePos);

    unsigned __int8 type[2] = {2, 102};
    CChatStealSystem *stealSystem = CChatStealSystem::Instance();
    stealSystem->StealChatMsg(this, 9u, pwszMsg);

    const unsigned __int16 len = msg.size();
    for (int m = 0; m < dstCount; ++m)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        targets[m]->m_ObjID.m_wIndex,
        type,
        reinterpret_cast<char *>(&msg),
        len);
    }
    g_Network.m_pProcess[0]->LoadSendMsg( m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
  }
}

void CPlayer::pc_ChatRaceBossCryRequest(char *pwszChatData)
{
  if (m_pUserDB)
  {
    const int raceCode = m_Param.GetRaceCode();
    CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
    const unsigned int bossSerial = rank->GetCurrentRaceBossSerial(raceCode, 0);
    const unsigned int charSerial = m_Param.GetCharSerial();
    if (bossSerial == charSerial)
    {
      _announ_message_receipt_udp msg{};
      msg.byMessageType = 18;
      msg.bySenderRace = m_Param.GetRaceCode();
      std::strcpy(msg.wszSenderName, m_Param.GetCharNameW());
      msg.bySize = static_cast<unsigned __int8>(std::strlen(pwszChatData));
      std::memcpy(msg.wszChatData, pwszChatData, msg.bySize);
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
          if (target->m_Param.GetRaceCode() == m_Param.GetRaceCode()
              && target->m_pCurMap == m_pCurMap
              && target->m_wMapLayerIndex == m_wMapLayerIndex)
          {
            g_Network.m_pProcess[0]->LoadSendMsg(
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

  _exchange_money_result_zocl msg{};
  msg.byErrCode = byErrCode;
  msg.dwLeftDalant = m_Param.GetDalant();
  msg.dwLeftGold = m_Param.GetGold();

  unsigned __int8 pbyType[2] = {12, 13};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), sizeof(msg));
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
  strcpy_s(msg.wszSendName, 17, wszSendName);
  strcpy_s(msg.wszTitle, 21, wszTitle);
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
  strcpy_s(msg.wszRecvName, 17, wszRecvName);
  strcpy_s(msg.wszTitle, 21, wszTitle);
  strcpy_s(msg.wszContent, 201, wszContent);
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
    strcpy_s(msg.wszContent, 201, wszContent);
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
    if (pData[index].m_bLive && !std::strcmp(pData[index].m_pUserDB->m_szAccountID, szAccount))
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

__int64 GetItemEquipGrade(int nTableCode, int nItemIndex)
{
  CRecordData *table = &s_ptblItemData[nTableCode];

  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    {
      _DfnEquipItem_fld *record = reinterpret_cast<_DfnEquipItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nClassGradeLim) : 1;
    }
    case 6:
    {
      _WeaponItem_fld *record = reinterpret_cast<_WeaponItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nClassGradeLim) : 1;
    }
    case 7:
    {
      _CloakItem_fld *record = reinterpret_cast<_CloakItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nClassGradeLim) : 1;
    }
    case 8:
    {
      _RingItem_fld *record = reinterpret_cast<_RingItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nClassGradeLim) : 1;
    }
    case 9:
    {
      _AmuletItem_fld *record = reinterpret_cast<_AmuletItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nClassGradeLim) : 1;
    }
    default:
      return 1;
  }
}

__int64 GetItemEquipGrade(int nTableCode, const char *szRecordCode)
{
  CRecordData *table = &s_ptblItemData[nTableCode];

  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    {
      _DfnEquipItem_fld *record = reinterpret_cast<_DfnEquipItem_fld *>(table->GetRecord(szRecordCode));
      return record ? static_cast<unsigned int>(record->m_nClassGradeLim) : 1;
    }
    case 6:
    {
      _WeaponItem_fld *record = reinterpret_cast<_WeaponItem_fld *>(table->GetRecord(szRecordCode));
      return record ? static_cast<unsigned int>(record->m_nClassGradeLim) : 1;
    }
    case 7:
    {
      _CloakItem_fld *record = reinterpret_cast<_CloakItem_fld *>(table->GetRecord(szRecordCode));
      return record ? static_cast<unsigned int>(record->m_nClassGradeLim) : 1;
    }
    case 8:
    {
      _RingItem_fld *record = reinterpret_cast<_RingItem_fld *>(table->GetRecord(szRecordCode));
      return record ? static_cast<unsigned int>(record->m_nClassGradeLim) : 1;
    }
    case 9:
    {
      _AmuletItem_fld *record = reinterpret_cast<_AmuletItem_fld *>(table->GetRecord(szRecordCode));
      return record ? static_cast<unsigned int>(record->m_nClassGradeLim) : 1;
    }
    default:
      return 1;
  }
}

_STORAGE_LIST::_db_con *CPlayer::IsBulletValidity(unsigned __int16 wBulletSerial)
{
  if (wBulletSerial == 65535)
  {
    return nullptr;
  }

  _STORAGE_LIST::_db_con *item = m_Param.m_dbEmbellish.GetPtrFromSerial(wBulletSerial);
  if (!item)
  {
    return nullptr;
  }

  _STORAGE_LIST::_db_con *weapon = m_Param.m_dbEquip.m_pStorageList + 6;
  if (!weapon->m_bLoad)
  {
    return nullptr;
  }

  if (item->m_byTableCode != 10)
  {
    return nullptr;
  }

  const int cashType = GetUsePcCashType(item->m_byTableCode, item->m_wItemIndex);
  if (!IsUsableAccountType(cashType))
  {
    SendMsg_PremiumCashItemUse(65535);
    return nullptr;
  }

  _WeaponItem_fld *weaponRecord = reinterpret_cast<_WeaponItem_fld *>(g_Main.m_tblItemData[6].GetRecord(weapon->m_wItemIndex));
  if (!weaponRecord)
  {
    return nullptr;
  }

  if (!std::strncmp(weaponRecord->m_strBulletType, "-1", 2u))
  {
    return item;
  }

  _BulletItem_fld *bulletRecord = reinterpret_cast<_BulletItem_fld *>(g_Main.m_tblItemData[10].GetRecord(item->m_wItemIndex));
  if (!bulletRecord)
  {
    return nullptr;
  }

  const size_t bulletLenRaw = std::strlen(bulletRecord->m_strBulletType);
  const int bulletLen = static_cast<int>(bulletLenRaw >= 64 ? 64 : bulletLenRaw);
  for (int j = 0; j < bulletLen; ++j)
  {
    if (strchr(weaponRecord->m_strBulletType, bulletRecord->m_strBulletType[j]))
    {
      return item;
    }
  }

  return nullptr;
}

_STORAGE_LIST::_db_con *CPlayer::IsEffBulletValidity(unsigned __int16 wEffBulletSerial)
{
  if (wEffBulletSerial == 65535)
  {
    return nullptr;
  }

  _STORAGE_LIST::_db_con *item = m_Param.m_dbEmbellish.GetPtrFromSerial(wEffBulletSerial);
  if (!item)
  {
    return nullptr;
  }

  _STORAGE_LIST::_db_con *weapon = m_Param.m_dbEquip.m_pStorageList + 6;
  if (!weapon)
  {
    return nullptr;
  }

  if (item->m_byTableCode != 10)
  {
    return nullptr;
  }

  const int cashType = GetUsePcCashType(item->m_byTableCode, item->m_wItemIndex);
  if (!IsUsableAccountType(cashType))
  {
    SendMsg_PremiumCashItemUse(65535);
    return nullptr;
  }

  _WeaponItem_fld *weaponRecord = reinterpret_cast<_WeaponItem_fld *>(g_Main.m_tblItemData[6].GetRecord(weapon->m_wItemIndex));
  _BulletItem_fld *bulletRecord = reinterpret_cast<_BulletItem_fld *>(g_Main.m_tblItemData[10].GetRecord(item->m_wItemIndex));
  if (!weaponRecord || !bulletRecord)
  {
    return nullptr;
  }

  if (!std::strncmp(weaponRecord->m_strEffBulletType, "-1", 2u))
  {
    return nullptr;
  }

  if (!std::strncmp(weaponRecord->m_strEffBulletType, "D", 1u))
  {
    return std::strncmp(bulletRecord->m_strEffBulletType, "-1", 2u) ? item : nullptr;
  }

  const size_t bulletLenRaw = std::strlen(bulletRecord->m_strEffBulletType);
  const int bulletLen = static_cast<int>(bulletLenRaw >= 64 ? 64 : bulletLenRaw);
  for (int j = 0; j < bulletLen; ++j)
  {
    if (strchr(weaponRecord->m_strEffBulletType, bulletRecord->m_strEffBulletType[j]))
    {
      return item;
    }
  }

  return nullptr;
}

char CPlayer::IsSFUsableGauge(unsigned __int8 byEffectCode, unsigned __int16 wEffectIndex, unsigned __int16 *pwDelPoint)
{
  int hpCost = 0;
  int fpCost = 0;
  int spCost = 0;

  if (byEffectCode == 1)
  {
    _force_fld *record = reinterpret_cast<_force_fld *>(g_Main.m_tblEffectData[byEffectCode].GetRecord(wEffectIndex));
    if (!record)
    {
      return 0;
    }
    hpCost = record->m_nNeedHP;
    const float fpBase = static_cast<float>(record->m_nNeedFP);
    fpCost = static_cast<int>(fpBase * m_EP.GetEff_Rate(EFF_RATE_FP_COST));
    spCost = record->m_nNeedSP;
  }
  else
  {
    _skill_fld *record = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[byEffectCode].GetRecord(wEffectIndex));
    if (!record)
    {
      return 0;
    }
    hpCost = record->m_nNeedHP;
    const float fpBase = static_cast<float>(record->m_nNeedFP);
    fpCost = static_cast<int>(fpBase * m_EP.GetEff_Rate(EFF_RATE_FP_COST));
    spCost = record->m_nNeedSP;
  }

  if (hpCost < 0)
  {
    hpCost = 0;
  }
  if (fpCost < 0)
  {
    fpCost = 0;
  }
  if (spCost < 0)
  {
    spCost = 0;
  }

  if (GetHP() < hpCost || GetFP() < fpCost || GetSP() < spCost)
  {
    return 0;
  }

  if (pwDelPoint)
  {
    pwDelPoint[0] = static_cast<unsigned __int16>(hpCost);
    pwDelPoint[1] = static_cast<unsigned __int16>(fpCost);
    pwDelPoint[2] = static_cast<unsigned __int16>(spCost);
  }

  return 1;
}

char CPlayer::IsSFUseableRace(unsigned __int8 byEffectCode, unsigned __int16 wEffectIndex)
{
  const unsigned __int8 raceSexCode = m_Param.GetRaceSexCode();

  if (byEffectCode == 0 || byEffectCode == 2 || byEffectCode == 3)
  {
    _skill_fld *record = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[byEffectCode].GetRecord(wEffectIndex));
    if (record->m_strUsableRace[raceSexCode] == '1')
    {
      return 1;
    }
  }
  else
  {
    _force_fld *record = reinterpret_cast<_force_fld *>(g_Main.m_tblEffectData[byEffectCode].GetRecord(wEffectIndex));
    if (record->m_strUsableRace[raceSexCode] == '1')
    {
      return 1;
    }
  }
  return 0;
}

char CPlayer::IsSFUsableSFMastery(unsigned __int8 byMasteryCode, int nMasteryIndex)
{
  if (nMasteryIndex < 0)
  {
    return 0;
  }

  if (byMasteryCode == 4)
  {
    if (nMasteryIndex >= 24)
    {
      return 0;
    }
  }
  else
  {
    if (byMasteryCode != 3 || nMasteryIndex >= 8)
    {
      return 0;
    }
  }

  int *limits = nMPL;
  if (m_Param.m_pClassHistory[0])
  {
    if (m_Param.m_pClassHistory[0]->m_nClass == 3 && m_Param.m_pClassData->m_nClass == 3)
    {
      limits = nSpMPL;
    }
  }

  const unsigned __int8 subIndex = static_cast<unsigned __int8>(nMasteryIndex % 4);
  const int baseIndex = nMasteryIndex - subIndex;
  for (int j = 0; j < static_cast<int>(subIndex); ++j)
  {
    const int mastery = m_pmMst.GetMasteryPerMast(byMasteryCode, static_cast<unsigned __int8>(j + baseIndex));
    if (mastery < limits[j])
    {
      return 0;
    }
  }

  return 1;
}

char CPlayer::IsSFActableByClass(unsigned __int8 byEffectCode, _base_fld *pSFFld)
{
  if (byEffectCode == 2 || m_bFreeSFByClass)
  {
    return 1;
  }

  int classCount = 1;
  int classCode = m_Param.m_pClassData->m_nClass;
  int prevClass = -1;

  if (m_Param.m_pClassHistory[0])
  {
    if (m_Param.m_pClassHistory[0]->m_nClass != m_Param.m_pClassData->m_nClass)
    {
      prevClass = m_Param.m_pClassHistory[0]->m_nClass;
      classCount = 2;
    }
  }
  else if (m_Param.m_pClassData->m_nClass == 3 && !byEffectCode
           && get_sf_need_special_type(byEffectCode, pSFFld) > 0)
  {
    return 0;
  }

  for (int j = 0; j < classCount; ++j)
  {
    const int checkClass = (j == 0) ? classCode : prevClass;
    if (check_sf_class(checkClass, byEffectCode, pSFFld))
    {
      return 1;
    }
  }

  return 0;
}

unsigned __int8 CPlayer::_GetPartyMemberInCircle(CPlayer **out_ppMember, int nMax, bool bOne)
{
if (!m_pPartyMgr)
  {
    return 0;
  }

  CPartyPlayer **partyMembers = m_pPartyMgr->GetPtrPartyMember();
  if (!partyMembers)
  {
    return 0;
  }

  unsigned __int8 count = 0;
  for (int j = 0; j < 8 && partyMembers[j]; ++j)
  {
    CPlayer *member = &g_Player[partyMembers[j]->m_wZoneIndex];
    if ((bOne || member != this)
        && member->m_bLive
        && !member->m_bCorpse
        && member->m_pCurMap == m_pCurMap
        && member->m_wMapLayerIndex == m_wMapLayerIndex
        && std::fabs(member->m_fCurPos[0] - m_fCurPos[0]) <= 540.0f
        && std::fabs(member->m_fCurPos[2] - m_fCurPos[2]) <= 540.0f
        && std::fabs(member->m_fCurPos[1] - m_fCurPos[1]) <= 100.0f)
    {
      out_ppMember[count++] = member;
    }
  }

  return count;
}

void CPlayer::Emb_CheckActForQuestParty(int nActCode, char *pszReqCode, unsigned __int16 wAddCount)
{
  if (!m_pPartyMgr || !m_pPartyMgr->IsPartyMode())
  {
    return;
  }

  CPlayer *partyMembers[8]{};
  const unsigned __int8 partyCount = _GetPartyMemberInCircle(partyMembers, 8, true);
  for (int j = 0; j < partyCount; ++j)
  {
    if (partyMembers[j] != this)
    {
      partyMembers[j]->Emb_CheckActForQuest(nActCode, pszReqCode, wAddCount, true);
    }
  }
}

void CPlayer::Emb_EquipLink()
{
  for (int linkIndex = 0; linkIndex < 50; ++linkIndex)
  {
    _quick_link *link = &m_Param.m_QLink[linkIndex];
    if (link->byLinkIndex == 255)
    {
      continue;
    }

    unsigned __int8 storageCode = 0;
    _STORAGE_LIST::_db_con *item = m_Param.GetPtrItemStorage(link->wSerial, &storageCode);
    if (item)
    {
      const unsigned __int16 packedIndex = static_cast<unsigned __int16>(item->m_byStorageIndex | (storageCode << 8));
      m_pUserDB->Update_LinkBoardSlot(static_cast<unsigned __int8>(linkIndex), 4u, packedIndex);
    }
    else
    {
      m_Param.PopLink(linkIndex);
      m_pUserDB->Update_LinkBoardSlot(static_cast<unsigned __int8>(linkIndex), 255, 65535);
    }
  }
}

void CPlayer::SendMsg_InsertItemInform(char byStorageCode, _STORAGE_LIST::_db_con *pItem)
{
  _insert_item_inform_zocl msg{};

  msg.byStorageCode = byStorageCode;
  msg.byTableCode = pItem->m_byTableCode;
  msg.wItemIndex = pItem->m_wItemIndex;
  msg.dwDurPoint = pItem->m_dwDur;
  msg.wSerial = pItem->m_wSerial;
  msg.dwLv = pItem->m_dwLv;

  unsigned __int8 pbyType[2] = {17, 15};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), sizeof(msg));
}

char CPlayer::IsEquipAbleGrade(unsigned __int8 byGradeLv)
{
  if (byGradeLv >= 8u)
  {
    if (byGradeLv == 8)
    {
      if (!m_Param.m_pGuild)
      {
        return 0;
      }

      const unsigned int guildMasterSerial = m_Param.m_pGuild->GetGuildMasterSerial();
      if (guildMasterSerial != m_Param.GetCharSerial())
      {
        return 0;
      }
    }
    else if (byGradeLv == 9)
    {
      const unsigned int charSerial = m_Param.GetCharSerial();
      const int raceCode = m_Param.GetRaceCode();
      CPvpUserAndGuildRankingSystem *rankingSystem = CPvpUserAndGuildRankingSystem::Instance();
      if (!rankingSystem->IsCurrentRaceBossGroup(raceCode, charSerial))
      {
        return 0;
      }
    }
    else
    {
      if (byGradeLv != 10)
      {
        return 0;
      }

      const unsigned int charSerial = m_Param.GetCharSerial();
      const int raceCode = m_Param.GetRaceCode();
      CPvpUserAndGuildRankingSystem *rankingSystem = CPvpUserAndGuildRankingSystem::Instance();
      const unsigned int raceBossSerial = rankingSystem->GetCurrentRaceBossSerial(raceCode, 0);
      if (charSerial != raceBossSerial)
      {
        return 0;
      }
    }
  }
  else if (byGradeLv > static_cast<unsigned int>(m_Param.m_byPvPGrade))
  {
    return 0;
  }

  return 1;
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

  if (static_cast<unsigned int>(m_Param.GetRaceCode()) == 2 && IsActingSiegeMode())
  {
    return static_cast<char>(-60);
  }

  if (!pDst->m_bLive || pDst->m_bCorpse || pDst->m_pCurMap != m_pCurMap || pDst->GetCurSecNum() == -1)
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

  if (!IsAttackableInTown() && !pDst->IsAttackableInTown() && (IsInTown() || pDst->IsInTown()))
  {
    return static_cast<char>(-31);
  }

  if (!pDst->IsBeDamagedAble(this))
  {
    return static_cast<char>(-6);
  }

  if (!pDst->IsBeAttackedAble(true))
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
        _class_fld *historyClass = *m_Param.m_ppHistoryEffect[j];
        if (!historyClass)
        {
          break;
        }
        if (historyClass->m_bLauncherUsable)
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

  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE))
  {
    return static_cast<char>(-37);
  }

  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
  {
    return static_cast<char>(-37);
  }

  if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_21))
  {
    return static_cast<char>(-38);
  }

  if (m_byMoveType == 2)
  {
    return static_cast<char>(-41);
  }

  if (wBulletSerial != 65535)
  {
    bulletItem = IsBulletValidity(wBulletSerial);
    if (!bulletItem)
    {
      SendMsg_AdjustAmountInform(2u, wBulletSerial, 0);
      return static_cast<char>(-17);
    }
    bulletField = reinterpret_cast<_BulletItem_fld *>(
      g_Main.m_tblItemData[10].GetRecord(bulletItem->m_wItemIndex));
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

  if (wEffBtSerial != 65535)
  {
    effBulletItem = IsEffBulletValidity(wEffBtSerial);
    if (!effBulletItem)
    {
      SendMsg_AdjustAmountInform(2u, wEffBtSerial, 0);
      return static_cast<char>(-63);
    }
    effBulletField = reinterpret_cast<_BulletItem_fld *>(
      g_Main.m_tblItemData[10].GetRecord(effBulletItem->m_wItemIndex));
    *ppEffBtProp = effBulletItem;
    *ppfldEffBt = effBulletField;
  }

  float attackRange = 0.0f;
  if (m_pmWpn.byWpType == 7)
  {
    const float baseRange = static_cast<float>(m_pmWpn.wGaAttRange);
    const float width = pDst->GetWidth();
    const float rangeWithWidth = baseRange + (width / 2.0f);
    const float effPlus = m_EP.GetEff_Plus(EFF_PLUS_ATTACK_RANGE);
    attackRange = rangeWithWidth + effPlus;
  }
  else
  {
    const float baseRange = static_cast<float>(m_pmWpn.wGaAttRange);
    const float width = pDst->GetWidth();
    const float rangeWithWidth = baseRange + (width / 2.0f);
    const float effPlus = m_EP.GetEff_Plus(m_pmWpn.byWpClass + 4);
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

  if (static_cast<unsigned int>(m_Param.GetRaceCode()) == 2 && IsActingSiegeMode())
  {
    return static_cast<char>(-60);
  }

  if (pDst)
  {
    if (pDst == this)
    {
      return static_cast<char>(-6);
    }

    if (!pDst->m_bLive || pDst->m_bCorpse || pDst->m_pCurMap != m_pCurMap || pDst->GetCurSecNum() == -1)
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

    if (!pDst->IsBeAttackedAble(true))
    {
      return static_cast<char>(-6);
    }

    if (!IsAttackableInTown() && !pDst->IsAttackableInTown() && (IsInTown() || pDst->IsInTown()))
    {
      return static_cast<char>(-31);
    }

    if (!pDst->IsBeDamagedAble(this))
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

  if (byEffectCode == 2)
  {
    if (IsSiegeMode())
    {
      // Yorozuya fix implementation (non-IDA): siege mode can only use siege skills.
      if (pSkillFld->m_nTempEffectType != 23)
      {
        return static_cast<char>(-20);
      }

      if (m_pSiegeItem)
      {
        _SiegeKitItem_fld *siegeRecord = reinterpret_cast<_SiegeKitItem_fld *>(
          g_Main.m_tblItemData[27].GetRecord(m_pSiegeItem->m_wItemIndex));
        if (siegeRecord)
        {
          if (siegeRecord->m_nLevelLim == 30 && pSkillFld->m_nLv != 0)
          {
            return static_cast<char>(-20);
          }
          if (siegeRecord->m_nLevelLim == 40 && pSkillFld->m_nLv != 1)
          {
            return static_cast<char>(-20);
          }
        }
      }
    }
    else if (pSkillFld->m_nTempEffectType == 23)
    {
      return static_cast<char>(-20);
    }
  }

  if (!IsSFUsableGauge(byEffectCode, pSkillFld->m_dwIndex, pdwDecPoint))
  {
    return static_cast<char>(-25);
  }

  if (wBulletSerial != 65535)
  {
    bulletItem = IsBulletValidity(wBulletSerial);
    if (!bulletItem)
    {
      SendMsg_AdjustAmountInform(2u, wBulletSerial, 0);
      return static_cast<char>(-17);
    }
    bulletField = reinterpret_cast<_BulletItem_fld *>(
      g_Main.m_tblItemData[10].GetRecord(bulletItem->m_wItemIndex));
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

  if (wEffBtSerial != 65535)
  {
    effBulletItem = IsEffBulletValidity(wEffBtSerial);
    if (!effBulletItem)
    {
      SendMsg_AdjustAmountInform(2u, wEffBtSerial, 0);
      return static_cast<char>(-63);
    }
    effBulletField = reinterpret_cast<_BulletItem_fld *>(
      g_Main.m_tblItemData[10].GetRecord(effBulletItem->m_wItemIndex));
    *ppEffBtProp = effBulletItem;
    *ppfldEffBt = effBulletField;
  }

  int classGrade[4]{};
  classGrade[0] = -1;
  if (byEffectCode)
  {
    if (byEffectCode == 2)
    {
      if (!m_Param.IsActableClassSkill(pSkillFld->m_strCode, classGrade))
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
      if (std::strcmp(bulletField->m_strEffectIndex, pSkillFld->m_strCode))
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

  if (m_EP.GetEff_State(EFF_STATE_SKILL_RESTRICTED))
  {
    return static_cast<char>(-50);
  }

  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE))
  {
    return static_cast<char>(-37);
  }

  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
  {
    return static_cast<char>(-37);
  }

  if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_21))
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
    if (pSkillFld->m_strFixWeapon[m_pmWpn.byWpType] == '1')
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
      const float width = pDst->GetWidth();
      const float rangeWithWidth = baseRange + (width / 2.0f);
      const float effPlus = m_EP.GetEff_Plus(EFF_PLUS_ATTACK_RANGE);
      const float rangePlus = rangeWithWidth + effPlus;
      const float classPlus = m_EP.GetEff_Plus(m_pmWpn.byWpClass + 6);
      attackRange = (rangePlus + classPlus) + static_cast<float>(pSkillFld->m_nBonusDistance);
    }
    else
    {
      const float baseRange = static_cast<float>(m_pmWpn.wGaAttRange);
      const float width = pDst->GetWidth();
      const float rangeWithWidth = baseRange + (width / 2.0f);
      const float effPlus = m_EP.GetEff_Plus(m_pmWpn.byWpClass + 4);
      const float rangePlus = rangeWithWidth + effPlus;
      const float classPlus = m_EP.GetEff_Plus(m_pmWpn.byWpClass + 6);
      attackRange = (rangePlus + classPlus) + static_cast<float>(pSkillFld->m_nBonusDistance);
    }

    if (IsSiegeMode())
    {
      _SiegeKitItem_fld *siegeRecord =
        reinterpret_cast<_SiegeKitItem_fld *>(g_Main.m_tblItemData[27].GetRecord(m_pSiegeItem->m_wItemIndex));
      if (siegeRecord)
      {
        attackRange = attackRange + siegeRecord->m_fMaxDst;
      }
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
      _SiegeKitItem_fld *siegeRecord =
        reinterpret_cast<_SiegeKitItem_fld *>(g_Main.m_tblItemData[27].GetRecord(m_pSiegeItem->m_wItemIndex));
      if (siegeRecord && siegeRecord->m_fMinDst > dist)
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
      const float effPlus = m_EP.GetEff_Plus(EFF_PLUS_ATTACK_RANGE);
      const float rangePlus = baseRange + effPlus;
      const float classPlus = m_EP.GetEff_Plus(m_pmWpn.byWpClass + 6);
      attackRange = (rangePlus + classPlus) + static_cast<float>(pSkillFld->m_nBonusDistance);
    }
    else
    {
      const float baseRange = static_cast<float>(m_pmWpn.wGaAttRange);
      const float effPlus = m_EP.GetEff_Plus(m_pmWpn.byWpClass + 4);
      const float rangePlus = baseRange + effPlus;
      const float classPlus = m_EP.GetEff_Plus(m_pmWpn.byWpClass + 6);
      attackRange = (rangePlus + classPlus) + static_cast<float>(pSkillFld->m_nBonusDistance);
    }

    if (IsSiegeMode())
    {
      _SiegeKitItem_fld *siegeRecord =
        reinterpret_cast<_SiegeKitItem_fld *>(g_Main.m_tblItemData[27].GetRecord(m_pSiegeItem->m_wItemIndex));
      if (siegeRecord)
      {
        attackRange = attackRange + siegeRecord->m_fMaxDst;
      }
    }

    const float dist = GetSqrt(pfAttackPos, m_fCurPos);
    if (dist > attackRange)
    {
      return static_cast<char>(-3);
    }

    if (IsSiegeMode())
    {
      _SiegeKitItem_fld *siegeRecord =
        reinterpret_cast<_SiegeKitItem_fld *>(g_Main.m_tblItemData[27].GetRecord(m_pSiegeItem->m_wItemIndex));
      if (siegeRecord && siegeRecord->m_fMinDst > dist)
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

  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE))
  {
    return static_cast<char>(-37);
  }

  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
  {
    return static_cast<char>(-37);
  }

  if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_21))
  {
    return static_cast<char>(-38);
  }

  if (m_byMoveType == 2)
  {
    return static_cast<char>(-41);
  }

  forceItem = m_Param.m_dbForce.GetPtrFromSerial(wForceItemSerial);
  if (!forceItem)
  {
    return static_cast<char>(-16);
  }

  if (forceItem->m_bLock)
  {
    return static_cast<char>(-30);
  }

  forceField = reinterpret_cast<_force_fld *>(
    g_Main.m_tblEffectData[1].GetRecord(CPlayer::s_pnLinkForceItemToEffect[forceItem->m_wItemIndex]));
  if (!forceField)
  {
    return static_cast<char>(-16);
  }

  unsigned __int8 masteryIndex = static_cast<unsigned __int8>(-1);
  if (static_cast<unsigned int>(forceField->m_nMastIndex) < 24)
  {
    masteryIndex = static_cast<unsigned __int8>(forceField->m_nMastIndex);
  }

  if (!m_bSFDelayNotCheck
      && !_ATTACK_DELAY_CHECKER::IsDelay(&m_AttDelayChker, 1u, forceField->m_dwIndex, masteryIndex))
  {
    return static_cast<char>(-5);
  }

  if (!IsRidingUnit() && m_pmWpn.byWpType == 0)
  {
    _STORAGE_LIST::_db_con *weapon = &m_Param.m_dbEquip.m_pStorageList[6];
    if (weapon->m_bLoad && weapon->m_byTableCode == 6)
    {
      m_pmWpn.FixWeapon(weapon);
    }
  }

  if (m_pmWpn.byWpType == 11 || m_pmWpn.byWpType == 10)
  {
    return static_cast<char>(-9);
  }

  const int attackType = forceField->m_nEffectGroup;
  if (attackType == 4 || attackType == 6)
  {
    pDst = nullptr;
  }

  if (static_cast<unsigned int>(m_Param.GetRaceCode()) == 2 && IsActingSiegeMode())
  {
    return static_cast<char>(-60);
  }

  if (pDst)
  {
    if (pDst == this)
    {
      return static_cast<char>(-6);
    }

    if (!pDst->m_bLive || pDst->m_bCorpse || pDst->m_pCurMap != m_pCurMap || pDst->GetCurSecNum() == -1)
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

    if (!pDst->IsBeAttackedAble(true))
    {
      return static_cast<char>(-6);
    }

    if (!IsAttackableInTown() && !pDst->IsAttackableInTown() && (IsInTown() || pDst->IsInTown()))
    {
      return static_cast<char>(-31);
    }

    if (!pDst->IsBeDamagedAble(this))
    {
      return static_cast<char>(-6);
    }
  }
  else if (attackType != 4 && attackType != 6)
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

  if (m_EP.GetEff_State(EFF_STATE_FORCE_RESTRICTED))
  {
    return static_cast<char>(-50);
  }

  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE))
  {
    return static_cast<char>(-37);
  }

  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
  {
    return static_cast<char>(-37);
  }

  if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_21))
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

  if (attackType == 6 && !m_pCurMap->IsMapIn(pfTarPos))
  {
    return static_cast<char>(-50);
  }

  if (pDst)
  {
    const float rangeBase = static_cast<float>(forceField->m_nActDistance + 40);
    const float width = pDst->GetWidth();
    const float rangeWithWidth = rangeBase + (width / 2.0f);
    const float effPlus = m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_8);
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
  else if (attackType != 4)
  {
    int range = forceField->m_nActDistance + 40;
    if (attackType == 6 || attackType == 5 || attackType == 7)
    {
      const float effPlus = m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_8);
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

  if (wEffBtSerial != 65535)
  {
    effBulletItem = IsEffBulletValidity(wEffBtSerial);
    if (!effBulletItem)
    {
      SendMsg_AdjustAmountInform(2u, wEffBtSerial, 0);
      return static_cast<char>(-63);
    }
    effBulletField = reinterpret_cast<_BulletItem_fld *>(
      g_Main.m_tblItemData[10].GetRecord(effBulletItem->m_wItemIndex));
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

  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE))
  {
    return static_cast<char>(-37);
  }

  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
  {
    return static_cast<char>(-37);
  }

  bulletParam = &m_pUsingUnit->m_BulletParam[byWeaponPart];
  if (!bulletParam->wLeftNum || bulletParam->wLeftNum == 65535)
  {
    return static_cast<char>(-17);
  }

  bulletField = reinterpret_cast<_UnitBullet_fld *>(g_Main.m_tblUnitBullet.GetRecord(bulletParam->wBulletIndex));
  if (!bulletField)
  {
    return static_cast<char>(-60);
  }

  if (byWeaponPart)
  {
    weaponField = reinterpret_cast<_UnitPart_fld *>(g_Main.m_tblUnitPart[4].GetRecord(m_pUsingUnit->byPart[4]));
  }
  else
  {
    weaponField = reinterpret_cast<_UnitPart_fld *>(g_Main.m_tblUnitPart[3].GetRecord(m_pUsingUnit->byPart[3]));
  }

  if (!weaponField)
  {
    return static_cast<char>(-60);
  }

  if (pDst == this)
  {
    return static_cast<char>(-6);
  }

  if (!pDst->m_bLive || pDst->m_bCorpse || pDst->m_pCurMap != m_pCurMap || pDst->GetCurSecNum() == -1)
  {
    return static_cast<char>(-6);
  }

  if (!IsAttackableInTown() && !pDst->IsAttackableInTown() && (IsInTown() || pDst->IsInTown()))
  {
    return static_cast<char>(-31);
  }

  if (!pDst->IsBeDamagedAble(this))
  {
    return static_cast<char>(-6);
  }

  const unsigned int guildResult = _pre_check_in_guild_battle(reinterpret_cast<CPlayer *>(pDst));
  if (guildResult)
  {
    return static_cast<char>(guildResult);
  }

  if (!pDst->IsBeAttackedAble(true))
  {
    return static_cast<char>(-6);
  }

  const float rangeLimit = static_cast<float>(weaponField->m_fAttackRange + 40.0);
  const float distCur = GetSqrt(pDst->m_fCurPos, m_fCurPos);
  if (distCur > rangeLimit)
  {
    const float distOld = GetSqrt(pDst->m_fOldPos, m_fCurPos);
    if (distOld > rangeLimit)
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

  _SiegeKitItem_fld *siegeRecord =
    reinterpret_cast<_SiegeKitItem_fld *>(g_Main.m_tblItemData[27].GetRecord(m_pSiegeItem->m_wItemIndex));
  if (!siegeRecord || siegeRecord->m_nUsableTyoe != m_pmWpn.byWpType)
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
        _class_fld *historyClass = *m_Param.m_ppHistoryEffect[j];
        if (!historyClass)
        {
          break;
        }
        if (historyClass->m_bLauncherUsable)
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

  if (wBulletSerial != 65535)
  {
    bulletItem = IsBulletValidity(wBulletSerial);
    if (!bulletItem)
    {
      SendMsg_AdjustAmountInform(2u, wBulletSerial, 0);
      return static_cast<char>(-17);
    }
    bulletField = reinterpret_cast<_BulletItem_fld *>(
      g_Main.m_tblItemData[10].GetRecord(bulletItem->m_wItemIndex));
    if (bulletItem->m_bLock)
    {
      return static_cast<char>(-29);
    }
  }

  if (wEffBtSerial != 65535)
  {
    effBulletItem = IsEffBulletValidity(wEffBtSerial);
    if (!effBulletItem)
    {
      SendMsg_AdjustAmountInform(2u, wEffBtSerial, 0);
      return static_cast<char>(-63);
    }
    effBulletField = reinterpret_cast<_BulletItem_fld *>(
      g_Main.m_tblItemData[10].GetRecord(effBulletItem->m_wItemIndex));
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

    if (!pDst->m_bLive || pDst->m_bCorpse || pDst->m_pCurMap != m_pCurMap || pDst->GetCurSecNum() == -1)
    {
      return static_cast<char>(-6);
    }

    const unsigned int guildResult = _pre_check_in_guild_battle(reinterpret_cast<CPlayer *>(pDst));
    if (guildResult)
    {
      return static_cast<char>(guildResult);
    }

    if (!pDst->IsBeAttackedAble(true))
    {
      return static_cast<char>(-6);
    }

    float attackRange = 0.0f;
    if (m_pmWpn.byWpType == 7)
    {
      const float baseRange = static_cast<float>(m_pmWpn.wGaAttRange);
      const float width = pDst->GetWidth();
      const float rangeWithWidth = baseRange + (width / 2.0f);
      attackRange = rangeWithWidth + siegeRecord->m_fMaxDst;
    }
    else
    {
      const float baseRange = static_cast<float>(m_pmWpn.wGaAttRange);
      const float width = pDst->GetWidth();
      const float rangeWithWidth = baseRange + (width / 2.0f);
      const float effPlus = m_EP.GetEff_Plus(m_pmWpn.byWpClass + 4);
      attackRange = (rangeWithWidth + siegeRecord->m_fMaxDst) + effPlus;
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

    if (siegeRecord->m_fMinDst > dist)
    {
      return static_cast<char>(-3);
    }

    if (!IsAttackableInTown() && !pDst->IsAttackableInTown() && (IsInTown() || pDst->IsInTown()))
    {
      return static_cast<char>(-31);
    }

    if (!pDst->IsBeDamagedAble(this))
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

    if (!m_pCurMap->IsMapIn(pfAttackPos))
    {
      return static_cast<char>(-36);
    }

    float attackRange = 0.0f;
    if (m_pmWpn.byWpType == 7)
    {
      const float baseRange = static_cast<float>(m_pmWpn.wGaAttRange) + siegeRecord->m_fMaxDst;
      const float effPlus = m_EP.GetEff_Plus(EFF_PLUS_ATTACK_RANGE);
      attackRange = baseRange + effPlus;
    }
    else
    {
      const float baseRange = static_cast<float>(m_pmWpn.wGaAttRange) + siegeRecord->m_fMaxDst;
      const float effPlus = m_EP.GetEff_Plus(m_pmWpn.byWpClass + 4);
      attackRange = baseRange + effPlus;
    }

    const float dist = GetSqrt(pfAttackPos, m_fCurPos);
    if (dist > attackRange)
    {
      return static_cast<char>(-3);
    }

    if (siegeRecord->m_fMinDst > dist)
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

  if (wBulletSerial != 65535)
  {
    bulletItem = IsBulletValidity(wBulletSerial);
    if (!bulletItem)
    {
      SendMsg_AdjustAmountInform(2u, wBulletSerial, 0);
      return 0;
    }
    bulletField = reinterpret_cast<_BulletItem_fld *>(
      g_Main.m_tblItemData[10].GetRecord(bulletItem->m_wItemIndex));
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
    if (byEffectCode == 3 && std::strcmp(bulletField->m_strEffectIndex, pSkillFld->m_strCode))
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
        battleOwner = reinterpret_cast<CAnimus *>(pDst)->m_pMaster;
      }
      else if (pDst->m_ObjID.m_byID == 4)
      {
        battleOwner = reinterpret_cast<CGuardTower *>(pDst)->m_pMasterTwr;
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
        CPlayer *master = reinterpret_cast<CAnimus *>(pDst)->m_pMaster;
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
        CPlayer *master = reinterpret_cast<CAnimus *>(pDst)->m_pMaster;
        if (master && master->IsPunished(1u, 0))
        {
          return 0;
        }
      }
    }
    else
    {
      if (pDst->IsPunished(1u, 0))
      {
        return 0;
      }
    }

    const int myRace = GetObjRace();
    const int dstRace = pDst->GetObjRace();
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
        if (!pDst->m_ObjID.m_byID && pDst->IsPunished(1u, 0))
        {
          return false;
        }
        if (pDst->GetObjRace() == GetObjRace())
        {
          return true;
        }
      }
      else
      {
        if (pDst->GetObjRace() != GetObjRace())
        {
          return true;
        }
      }
    }
    else
    {
      CPlayer *master = reinterpret_cast<CAnimus *>(pDst)->m_pMaster;
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
            if (!pDst->m_ObjID.m_byID && pDst->IsPunished(1u, 0))
            {
              return false;
            }
            if (pDst->GetObjRace() == GetObjRace())
            {
              return true;
            }
          }
          else
          {
            if (pDst->GetObjRace() != GetObjRace())
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
        if (!pDst->m_ObjID.m_byID && pDst->IsPunished(1u, 0))
        {
          return false;
        }
        if (pDst->GetObjRace() == GetObjRace())
        {
          return true;
        }
      }
      else
      {
        if (pDst->GetObjRace() != GetObjRace())
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
        if (!pDst->m_ObjID.m_byID && pDst->IsPunished(1u, 0))
        {
          return false;
        }
        if (pDst->GetObjRace() == GetObjRace())
        {
          return true;
        }
      }
      else
      {
        if (pDst->GetObjRace() != GetObjRace())
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
    if (pSkillFld->m_strGradeLimit[j] == '1')
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
          const unsigned int charSerial = m_Param.GetCharSerial();
          const int raceCode = m_Param.GetRaceCode();
          CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
          const unsigned int bossSerial = rank->GetCurrentRaceBossSerial(raceCode, 0);
          if (charSerial == bossSerial)
          {
            pass = true;
          }
          break;
        }
        case 2:
        {
          const unsigned int charSerial = m_Param.GetCharSerial();
          const int raceCode = m_Param.GetRaceCode();
          CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
          if (rank->GetBossType(raceCode, charSerial) == 1)
          {
            pass = true;
          }
          break;
        }
        case 3:
        {
          const unsigned int charSerial = m_Param.GetCharSerial();
          const int raceCode = m_Param.GetRaceCode();
          CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
          if (rank->GetBossType(raceCode, charSerial) == 5)
          {
            pass = true;
          }
          break;
        }
        case 4:
        {
          const unsigned int charSerial = m_Param.GetCharSerial();
          const int raceCode = m_Param.GetRaceCode();
          CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
          if (rank->GetBossType(raceCode, charSerial) == 2)
          {
            pass = true;
          }
          break;
        }
        case 5:
        {
          const unsigned int charSerial = m_Param.GetCharSerial();
          const int raceCode = m_Param.GetRaceCode();
          CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
          if (rank->GetBossType(raceCode, charSerial) == 6)
          {
            pass = true;
          }
          break;
        }
        case 6:
        {
          const unsigned int charSerial = m_Param.GetCharSerial();
          const int raceCode = m_Param.GetRaceCode();
          CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
          if (rank->GetBossType(raceCode, charSerial) == 3)
          {
            pass = true;
          }
          break;
        }
        case 7:
        {
          const unsigned int charSerial = m_Param.GetCharSerial();
          const int raceCode = m_Param.GetRaceCode();
          CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
          if (rank->GetBossType(raceCode, charSerial) == 7)
          {
            pass = true;
          }
          break;
        }
        case 8:
        {
          const unsigned int charSerial = m_Param.GetCharSerial();
          const int raceCode = m_Param.GetRaceCode();
          CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
          if (rank->GetBossType(raceCode, charSerial) == 4)
          {
            pass = true;
          }
          break;
        }
        case 9:
        {
          const unsigned int charSerial = m_Param.GetCharSerial();
          const int raceCode = m_Param.GetRaceCode();
          CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
          if (rank->GetBossType(raceCode, charSerial) == 8)
          {
            pass = true;
          }
          break;
        }
        case 10:
          if (m_Param.m_pGuild)
          {
            const unsigned int charSerial = m_Param.GetCharSerial();
            const unsigned int guildMasterSerial = m_Param.m_pGuild->GetGuildMasterSerial();
            if (charSerial == guildMasterSerial)
            {
              pass = true;
            }
          }
          break;
        case 11:
          if (m_pPartyMgr && m_pPartyMgr->IsPartyMode() && m_pPartyMgr->IsPartyBoss())
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
  unsigned __int8 forcedPart = 255;
  if (pDst && !pDst->m_ObjID.m_byID)
  {
    forcedPart = static_cast<CPlayer *>(pDst)->m_byDamagePart;
  }
  if (!pDst || pDst->m_ObjID.m_byID || forcedPart == 255)
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
    masteryBonus = m_pmMst.GetMasteryPerMast(6u, 0);
  }
  else
  {
    masteryBonus = m_pmMst.GetMasteryPerMast(0, m_pmWpn.byWpClass);
  }

  if (strncmp(m_pmWpn.strEffBulletType, "-1", 2u) && pEffBtFld)
  {
    const float minAf = static_cast<float>(m_pmWpn.nGaMinAF);
    const float effRate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
    pAP->nMinAFPlus = static_cast<int>(
      static_cast<float>(((minAf * effRate) * fAddBulletFc) * fAddEffBtFc) + static_cast<float>(CPlayer::s_nAddMstFc[masteryBonus]));

    const float maxAf = static_cast<float>(m_pmWpn.nGaMaxAF);
    const float maxRate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
    pAP->nMaxAFPlus = static_cast<int>(
      static_cast<float>(((maxAf * maxRate) * fAddBulletFc) * fAddEffBtFc) + static_cast<float>(CPlayer::s_nAddMstFc[masteryBonus]));
  }

  const float minAf = static_cast<float>(m_pmWpn.nGaMinAF);
  const float minRate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
  pAP->nMinAF = static_cast<int>((minAf * minRate) * fAddBulletFc + static_cast<float>(CPlayer::s_nAddMstFc[masteryBonus]));

  const float maxAf = static_cast<float>(m_pmWpn.nGaMaxAF);
  const float maxRate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
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
    std::memcpy(pAP->fArea, pDst->m_fCurPos, sizeof(pAP->fArea));
  }

  pAP->bMatchless = m_bCheat_Matchless;
  pAP->nMaxAttackPnt = m_nMaxAttackPnt;
  if (pDst && pDst->m_ObjID.m_byKind == 1 && !reinterpret_cast<CMonster *>(pDst)->IsViewArea(this))
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
    _BulletItem_fld *bulletRecord = reinterpret_cast<_BulletItem_fld *>(g_Main.m_tblItemData[10].GetRecord(pBulletItem->m_wItemIndex));
    pAP->nTol = bulletRecord ? bulletRecord->m_nProperty : m_pmWpn.byAttTolType;
  }
  else
  {
    pAP->nTol = m_pmWpn.byAttTolType;
  }

  pAP->nClass = m_pmWpn.byWpClass;

  if (m_pmWpn.nGaMaxAF < 0 || m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE) < 0.0f || fAddBulletFc < 0.0f)
  {
    const float effRate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
    g_Main.m_logSystemError.Write(
      "Skill Attack Warning : nGaMaxAF(%d), Potion_Inc_Fc(%f), fAddBulletFc(%f)",
      m_pmWpn.nGaMaxAF,
      effRate,
      fAddBulletFc);
  }

  if (m_pmWpn.byWpType == 7)
  {
    const float minAf = static_cast<float>(m_pmWpn.nGaMinAF);
    const float rate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
    const float tempMin = (minAf * rate) * fAddBulletFc;
    const int mastery = m_pmMst.GetMasteryPerMast(6u, 0);
    pAP->nMinAF = static_cast<int>(tempMin + static_cast<float>(mastery));

    const float maxAf = static_cast<float>(m_pmWpn.nGaMaxAF);
    const float maxRate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
    const float tempMax = (maxAf * maxRate) * fAddBulletFc;
    const int maxMastery = m_pmMst.GetMasteryPerMast(6u, 0);
    pAP->nMaxAF = static_cast<int>(tempMax + static_cast<float>(maxMastery));
  }
  else
  {
    const float minAf = static_cast<float>(m_pmWpn.nGaMinAF);
    const float rate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
    const float tempMin = (minAf * rate) * fAddBulletFc;
    const int mastery = m_pmMst.GetMasteryPerMast(0, m_pmWpn.byWpClass);
    pAP->nMinAF = static_cast<int>(tempMin + static_cast<float>(mastery));

    const float maxAf = static_cast<float>(m_pmWpn.nGaMaxAF);
    const float maxRate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
    const float tempMax = (maxAf * maxRate) * fAddBulletFc;
    const int maxMastery = m_pmMst.GetMasteryPerMast(0, m_pmWpn.byWpClass);
    pAP->nMaxAF = static_cast<int>(tempMax + static_cast<float>(maxMastery));
  }

  if (strncmp(m_pmWpn.strEffBulletType, "-1", 2u) && pEffBulletItem)
  {
    if (m_pmWpn.byWpType == 7)
    {
      const float minAf = static_cast<float>(m_pmWpn.nGaMinAF);
      const float rate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
      const float tempMin = ((minAf * rate) * fAddBulletFc) * fAddEffBulletFc;
      const int mastery = m_pmMst.GetMasteryPerMast(6u, 0);
      pAP->nMinAFPlus = static_cast<int>(tempMin + static_cast<float>(mastery));

      const float maxAf = static_cast<float>(m_pmWpn.nGaMaxAF);
      const float maxRate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
      const float tempMax = ((maxAf * maxRate) * fAddBulletFc) * fAddEffBulletFc;
      const int maxMastery = m_pmMst.GetMasteryPerMast(6u, 0);
      pAP->nMaxAFPlus = static_cast<int>(tempMax + static_cast<float>(maxMastery));
    }
    else
    {
      const float minAf = static_cast<float>(m_pmWpn.nGaMinAF);
      const float rate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
      const float tempMin = ((minAf * rate) * fAddBulletFc) * fAddEffBulletFc;
      const int mastery = m_pmMst.GetMasteryPerMast(0, m_pmWpn.byWpClass);
      pAP->nMinAFPlus = static_cast<int>(tempMin + static_cast<float>(mastery));

      const float maxAf = static_cast<float>(m_pmWpn.nGaMaxAF);
      const float maxRate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
      const float tempMax = ((maxAf * maxRate) * fAddBulletFc) * fAddEffBulletFc;
      const int maxMastery = m_pmMst.GetMasteryPerMast(0, m_pmWpn.byWpClass);
      pAP->nMaxAFPlus = static_cast<int>(tempMax + static_cast<float>(maxMastery));
    }
  }

  if (IsSiegeMode())
  {
    _SiegeKitItem_fld *siegeRecord =
      reinterpret_cast<_SiegeKitItem_fld *>(g_Main.m_tblItemData[27].GetRecord(m_pSiegeItem->m_wItemIndex));
    if (siegeRecord)
    {
      const float effRate = m_EP.GetEff_Rate(EFF_RATE_SIEGE_ATTACK);
      pAP->nMinAF = static_cast<int>(static_cast<float>(pAP->nMinAF)
                                     * (siegeRecord->m_fGAAF * effRate));
      if (strncmp(m_pmWpn.strEffBulletType, "-1", 2u) && pEffBulletItem)
      {
        const float rate = m_EP.GetEff_Rate(EFF_RATE_SIEGE_ATTACK);
        pAP->nMinAFPlus = static_cast<int>(static_cast<float>(pAP->nMinAFPlus)
                                           * (siegeRecord->m_fGAAF * rate));
      }

      const float maxRate = m_EP.GetEff_Rate(EFF_RATE_SIEGE_ATTACK);
      pAP->nMaxAF = static_cast<int>(static_cast<float>(pAP->nMaxAF)
                                     * (siegeRecord->m_fGAAF * maxRate));
      if (strncmp(m_pmWpn.strEffBulletType, "-1", 2u) && pEffBulletItem)
      {
        const float rate = m_EP.GetEff_Rate(EFF_RATE_SIEGE_ATTACK);
        pAP->nMaxAFPlus = static_cast<int>(static_cast<float>(pAP->nMaxAFPlus)
                                           * (siegeRecord->m_fGAAF * rate));
      }
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
    pAP->nLevel = m_pmMst.GetSkillLv(pSkillFld->m_dwIndex);
    pAP->nLevel = static_cast<int>(static_cast<float>(pAP->nLevel) + m_EP.GetEff_Plus(EFF_PLUS_SKILL_LEVEL));
    if (pAP->nLevel > 7)
    {
      pAP->nLevel = 7;
    }
    pAP->nMastery = m_pmMst.GetMasteryPerMast(3u, pSkillFld->m_nMastIndex);
  }

  std::memcpy(pAP->fArea, pfAttackPos, sizeof(pAP->fArea));
  pAP->nMaxAttackPnt = m_nMaxAttackPnt;
  if (pDst && pDst->m_ObjID.m_byKind == 1 && !reinterpret_cast<CMonster *>(pDst)->IsViewArea(this))
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
    const float rate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
    pAP->nMinAFPlus = static_cast<int>(((minAf * rate) * fAddEffBtFc) + static_cast<float>(m_pmMst.m_mtyStaff));

    const float maxAf = static_cast<float>(m_pmWpn.nMaMaxAF);
    const float maxRate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
    pAP->nMaxAFPlus = static_cast<int>(((maxAf * maxRate) * fAddEffBtFc) + static_cast<float>(m_pmMst.m_mtyStaff));
  }

  const float minAf = static_cast<float>(m_pmWpn.nMaMinAF);
  const float minRate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
  pAP->nMinAF = static_cast<int>((minAf * minRate) + static_cast<float>(m_pmMst.m_mtyStaff));

  const float maxAf = static_cast<float>(m_pmWpn.nMaMaxAF);
  const float maxRate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
  pAP->nMaxAF = static_cast<int>((maxAf * maxRate) + static_cast<float>(m_pmMst.m_mtyStaff));

  pAP->nMinSel = m_pmWpn.byMaMinSel;
  pAP->nMaxSel = m_pmWpn.byMaMaxSel;
  pAP->pFld = pForceFld;
  pAP->byEffectCode = 1;
  pAP->nLevel = GetSFLevel(pForceFld->m_nLv, pForceItem->m_dwDur);
  pAP->nMastery = m_pmMst.GetMasteryPerMast(4u, pForceFld->m_nMastIndex);

  if (pDst)
  {
    std::memcpy(pAP->fArea, pDst->m_fCurPos, sizeof(pAP->fArea));
  }
  else
  {
    std::memcpy(pAP->fArea, pTar, sizeof(pAP->fArea));
  }

  pAP->nMaxAttackPnt = m_nMaxAttackPnt;
  if (pDst && pDst->m_ObjID.m_byKind == 1 && !reinterpret_cast<CMonster *>(pDst)->IsViewArea(this))
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
  const int mastery = m_pmMst.GetMasteryPerMast(6u, 0);
  pAP->nMaxAF = static_cast<int>((maxAf + static_cast<float>(mastery)) * m_fUnitPv_AttFc);

  pAP->nMinSel = pWeaponFld->m_nGAMinSelProb;
  pAP->nMaxSel = pWeaponFld->m_nGAMaxSelProb;
  pAP->nExtentRange = static_cast<int>(pWeaponFld->m_fAttackRange);
  pAP->pFld = pWeaponFld;
  pAP->nMaxAttackPnt = m_nMaxAttackPnt;
  if (pDst && pDst->m_ObjID.m_byKind == 1 && !reinterpret_cast<CMonster *>(pDst)->IsViewArea(this))
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
  unsigned __int8 forcedPart = 255;
  if (pDst && !pDst->m_ObjID.m_byID)
  {
    forcedPart = static_cast<CPlayer *>(pDst)->m_byDamagePart;
  }
  if (!pDst || pDst->m_ObjID.m_byID || forcedPart == 255)
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
    masteryBonus = m_pmMst.GetMasteryPerMast(6u, 0);
  }
  else
  {
    masteryBonus = m_pmMst.GetMasteryPerMast(0, m_pmWpn.byWpClass);
  }

  _SiegeKitItem_fld *siegeRecord =
    reinterpret_cast<_SiegeKitItem_fld *>(g_Main.m_tblItemData[27].GetRecord(m_pSiegeItem->m_wItemIndex));
  const float siegeMul = siegeRecord ? siegeRecord->m_fGAAF : 1.0f;

  if (strncmp(m_pmWpn.strEffBulletType, "-1", 2u) && pEffBulletFld)
  {
    const float minAf = static_cast<float>(m_pmWpn.nGaMinAF);
    const float minRate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
    const float minValue =
      ((minAf * minRate) * fAddBulletFc) * fAddEffBtFc + static_cast<float>(CPlayer::s_nAddMstFc[masteryBonus]);
    const float minMul = minValue * siegeMul;
    const float minEffRate = m_EP.GetEff_Rate(EFF_RATE_SIEGE_ATTACK);
    pAP->nMinAFPlus = static_cast<int>(minMul * minEffRate);

    const float maxAf = static_cast<float>(m_pmWpn.nGaMaxAF);
    const float maxRate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
    const float maxValue =
      ((maxAf * maxRate) * fAddBulletFc) * fAddEffBtFc + static_cast<float>(CPlayer::s_nAddMstFc[masteryBonus]);
    const float maxMul = maxValue * siegeMul;
    const float maxEffRate = m_EP.GetEff_Rate(EFF_RATE_SIEGE_ATTACK);
    pAP->nMaxAFPlus = static_cast<int>(maxMul * maxEffRate);
    pAP->nEffShotNum = 1;
  }

  const float minAf = static_cast<float>(m_pmWpn.nGaMinAF);
  const float minRate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
  const float minValue = (minAf * minRate) * fAddBulletFc + static_cast<float>(CPlayer::s_nAddMstFc[masteryBonus]);
  const float minMul = minValue * siegeMul;
  const float minEffRate = m_EP.GetEff_Rate(EFF_RATE_SIEGE_ATTACK);
  pAP->nMinAF = static_cast<int>(minMul * minEffRate);

  const float maxAf = static_cast<float>(m_pmWpn.nGaMaxAF);
  const float maxRate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
  const float maxValue = (maxAf * maxRate) * fAddBulletFc + static_cast<float>(CPlayer::s_nAddMstFc[masteryBonus]);
  const float maxMul = maxValue * siegeMul;
  const float maxEffRate = m_EP.GetEff_Rate(EFF_RATE_SIEGE_ATTACK);
  pAP->nMaxAF = static_cast<int>(maxMul * maxEffRate);

  pAP->nMinSel = m_pmWpn.byGaMinSel;
  pAP->nMaxSel = m_pmWpn.byGaMaxSel;
  pAP->nExtentRange = 20;

  if (pBulletFld)
  {
    pAP->nAttactType = pBulletFld->m_nEffectGroup;
    pAP->nShotNum = 1;
  }

  std::memcpy(pAP->fArea, pfAttackPos, sizeof(pAP->fArea));
  pAP->nMaxAttackPnt = m_nMaxAttackPnt;
  if (pDst && pDst->m_ObjID.m_byKind == 1 && !reinterpret_cast<CMonster *>(pDst)->IsViewArea(this))
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
    _BulletItem_fld *bulletRecord = reinterpret_cast<_BulletItem_fld *>(g_Main.m_tblItemData[10].GetRecord(pBulletItem->m_wItemIndex));
    pAP->nTol = bulletRecord ? bulletRecord->m_nProperty : m_pmWpn.byAttTolType;
  }
  else
  {
    pAP->nTol = m_pmWpn.byAttTolType;
  }

  pAP->nClass = m_pmWpn.byWpClass;
  if (m_pmWpn.byWpType == 7)
  {
    const float minAf = static_cast<float>(m_pmWpn.nGaMinAF);
    const float minRate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
    const float tempMin = (minAf * minRate) * fAddBulletFc;
    const int mastery = m_pmMst.GetMasteryPerMast(6u, 0);
    pAP->nMinAF = static_cast<int>(tempMin + static_cast<float>(mastery));

    const float maxAf = static_cast<float>(m_pmWpn.nGaMaxAF);
    const float maxRate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
    const float tempMax = (maxAf * maxRate) * fAddBulletFc;
    const int maxMastery = m_pmMst.GetMasteryPerMast(6u, 0);
    pAP->nMaxAF = static_cast<int>(tempMax + static_cast<float>(maxMastery));
  }
  else
  {
    const float minAf = static_cast<float>(m_pmWpn.nGaMinAF);
    const float minRate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
    const float tempMin = (minAf * minRate) * fAddBulletFc;
    const int mastery = m_pmMst.GetMasteryPerMast(0, m_pmWpn.byWpClass);
    pAP->nMinAF = static_cast<int>(tempMin + static_cast<float>(mastery));

    const float maxAf = static_cast<float>(m_pmWpn.nGaMaxAF);
    const float maxRate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
    const float tempMax = (maxAf * maxRate) * fAddBulletFc;
    const int maxMastery = m_pmMst.GetMasteryPerMast(0, m_pmWpn.byWpClass);
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
    pAP->nMastery = m_pmMst.GetMasteryPerMast(3u, pSkillFld->m_nMastIndex);
  }

  std::memcpy(pAP->fArea, pfAttackPos, sizeof(pAP->fArea));
  pAP->nMaxAttackPnt = m_nMaxAttackPnt;
  if (pDst && pDst->m_ObjID.m_byKind == 1 && !reinterpret_cast<CMonster *>(pDst)->IsViewArea(this))
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
  const float minRate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
  pAP->nMinAF = static_cast<int>((minAf * minRate) + static_cast<float>(m_pmMst.m_mtyStaff));

  const float maxAf = static_cast<float>(m_pmWpn.nMaMaxAF);
  const float maxRate = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
  pAP->nMaxAF = static_cast<int>((maxAf * maxRate) + static_cast<float>(m_pmMst.m_mtyStaff));

  pAP->nMinSel = m_pmWpn.byMaMinSel;
  pAP->nMaxSel = m_pmWpn.byMaMaxSel;
  pAP->pFld = pForceFld;
  pAP->byEffectCode = 1;
  pAP->nLevel = m_pmWpn.nActiveEffLvl;
  pAP->nMastery = m_pmMst.GetMasteryPerMast(4u, pForceFld->m_nMastIndex);
  std::memcpy(pAP->fArea, pfAttackPos, sizeof(pAP->fArea));
  pAP->nMaxAttackPnt = m_nMaxAttackPnt;
  if (pDst && pDst->m_ObjID.m_byKind == 1 && !reinterpret_cast<CMonster *>(pDst)->IsViewArea(this))
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
        g_Main.m_tblEffectData[effectCode].GetRecordByHash(m_pmWpn.strActiveCode_key, 0, 4));
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
        g_Main.m_tblEffectData[effectCode].GetRecordByHash(m_pmWpn.strActiveCode_key, 0, 5));
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
      g_Main.m_tblEffectData->GetRecordByHash(m_pmWpn.strActiveCode_key, 0, 5));
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

  std::memcpy(attackPos, pDamList->m_pChar->m_fCurPos, sizeof(attackPos));
  if (byEffectCode)
  {
    attackType = pSkillFld->m_nAttType[0];
  }
  else
  {
    attackType = GetSkillAttackTypeForLevel(pSkillFld, m_pmWpn.nActiveEffLvl);
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
  std::memcpy(attackPos, pDamList->m_pChar->m_fCurPos, sizeof(attackPos));

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
    if (!IsEffectableDst(pSkillFld->m_strActableDst, target))
    {
      return false;
    }

    if (pSkillFld->m_nContEffectType != -1 && !target->IsRecvableContEffect())
    {
      return false;
    }

    if (!pSkillFld->m_nContEffectType && !IsAttackableInTown() && !target->IsAttackableInTown())
    {
      if (IsInTown())
      {
        return false;
      }

      if (target->IsInTown())
      {
        return false;
      }

      if (m_Param.m_pGuild)
      {
        CGuildRoomSystem *roomSystem = CGuildRoomSystem::GetInstance();
        if (roomSystem->IsGuildRoomMemberIn(m_Param.m_pGuild->m_dwSerial, m_ObjID.m_wIndex, m_pUserDB->m_dwSerial))
        {
          return false;
        }
      }
    }

    unsigned __int8 errorCode[32]{};
    bool upMastery = false;
    success = AssistSkill(target, nEffectCode, pSkillFld, skillLevel, errorCode, &upMastery);
    if (!errorCode[0])
    {
      pDamList->m_bActiveSucc = 1;
    }
  }
  else
  {
    for (int j = 0; j < nDamagedObjNum; ++j)
    {
      if (IsEffectableDst(pSkillFld->m_strActableDst, target)
          && (pSkillFld->m_nContEffectType == -1 || target->IsRecvableContEffect())
          && (!target->m_EP.GetEff_State(EFF_STATE_INSUPERABLE)
              || (pSkillFld->m_nTempEffectType != -1 && usableTemp))
          && !target->m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
      {
        if (pSkillFld->m_nContEffectType || IsAttackableInTown() || target->IsAttackableInTown()
            || (!IsInTown() && !target->IsInTown()
                && (!m_Param.m_pGuild
                    || !CGuildRoomSystem::GetInstance()->IsGuildRoomMemberIn(
                         m_Param.m_pGuild->m_dwSerial,
                         m_ObjID.m_wIndex,
                         m_pUserDB->m_dwSerial))))
        {
          if (AssistSkillToOne(pDamList[j].m_pChar, nEffectCode, pSkillFld, skillLevel))
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
    if (!IsEffectableDst(pForceFld->m_strActableDst, target))
    {
      return false;
    }

    if (pForceFld->m_nContEffectType != -1 && !target->IsRecvableContEffect())
    {
      return false;
    }

    if (!pForceFld->m_nContEffectType && !IsAttackableInTown() && !target->IsAttackableInTown())
    {
      if (IsInTown())
      {
        return false;
      }

      if (target->IsInTown())
      {
        return false;
      }

      if (m_Param.m_pGuild)
      {
        CGuildRoomSystem *roomSystem = CGuildRoomSystem::GetInstance();
        if (roomSystem->IsGuildRoomMemberIn(m_Param.m_pGuild->m_dwSerial, m_ObjID.m_wIndex, m_pUserDB->m_dwSerial))
        {
          return false;
        }
      }
    }

    unsigned __int8 errorCode[32]{};
    bool upMastery = false;
    success = AssistForce(target, pForceFld, forceLevel, errorCode, &upMastery);
    if (!errorCode[0])
    {
      pDamList->m_bActiveSucc = 1;
    }
  }
  else
  {
    for (int j = 0; j < nDamagedObjNum; ++j)
    {
      if (IsEffectableDst(pForceFld->m_strActableDst, pDamList[j].m_pChar)
          && (pForceFld->m_nContEffectType == -1 || pDamList[j].m_pChar->IsRecvableContEffect())
          && (!pDamList[j].m_pChar->m_EP.GetEff_State(EFF_STATE_INSUPERABLE)
              || (pForceFld->m_nTempEffectType != -1 && usableTemp))
          && !pDamList[j].m_pChar->m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
      {
        if (pForceFld->m_nContEffectType || IsAttackableInTown() || target->IsAttackableInTown()
            || (!IsInTown() && !target->IsInTown()
                && (!m_Param.m_pGuild
                    || !CGuildRoomSystem::GetInstance()->IsGuildRoomMemberIn(
                         m_Param.m_pGuild->m_dwSerial,
                         m_ObjID.m_wIndex,
                         m_pUserDB->m_dwSerial))))
        {
          if (AssistForceToOne(pDamList[j].m_pChar, pForceFld, forceLevel))
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
        Stop();
        SendMsg_BreakStop();
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
        effPlus = static_cast<int>(m_EP.GetEff_Plus(m_pmWpn.byWpClass + 9));
      }

      const int addDelay = static_cast<int>(CalcEquipAttackDelay());
      const int level = static_cast<int>(GetLevel());
      const unsigned int attackDelay = m_pmWpn.GetAttackDelay(level, addDelay);
      int delay = effPlus + static_cast<int>(attackDelay);
      if (m_pmWpn.byWpType == 7)
      {
        const float effPlusInner = m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_11);
        delay = static_cast<int>(static_cast<float>(delay) + effPlusInner);
      }
      _ATTACK_DELAY_CHECKER::SetDelay(&m_AttDelayChker, static_cast<unsigned int>(delay));
    }

    if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_14))
    {
      RemoveSFContHelpByEffect(2, 14);
    }
    if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_21))
    {
      if (pAP.nAttactType != 4 && pAP.nAttactType != 5 && pAP.nAttactType != 6 && pAP.nAttactType != 7)
      {
        return;
      }
      RemoveSFContHelpByEffect(2, 21);
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
      CCharacter *damaged = pAt.m_DamList[j].m_pChar;
      const unsigned int totalDamage =
        static_cast<unsigned int>(pAt.m_DamList[j].m_nActiveDamage + pAt.m_DamList[j].m_nDamage);
      const int level = static_cast<int>(m_Param.GetLevel());
      damaged->SetDamage(static_cast<int>(totalDamage), this, level, pAt.m_bIsCrtAtt, -1, 0, true);

      if (IsChaosMode())
      {
        if (GetObjRace() == damaged->GetObjRace())
        {
          continue;
        }
      }

      if (GetObjRace() == damaged->GetObjRace() && !damaged->m_ObjID.m_byID)
      {
        CPlayer *damagedPlayer = static_cast<CPlayer *>(damaged);
        if (damagedPlayer->IsPunished(1u, 0))
        {
          continue;
        }
      }

      if (!pAt.m_bFailure)
      {
        const int dstLevel = static_cast<int>(damaged->GetLevel());
        if (IsPassMasteryLimitLvDiff(dstLevel))
        {
          const float damageRatio =
            static_cast<float>(pAt.m_DamList[j].m_nDamage)
            / static_cast<float>(damaged->GetMaxHP());
          if (damageRatio >= 0.0099999998f)
          {
            const int level = static_cast<int>(damaged->GetLevel());
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

    if (pDst && m_pRecalledAnimusChar && pDst->GetHP() > 0)
    {
      m_pRecalledAnimusChar->MasterAttack_MasterInform(pDst);
    }
    if (pDst && pDst->GetHP() > 0)
    {
      m_pmTwr.NotifyOwnerAttackInform(pDst);
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
        CPlayer::s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex,
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
        CPlayer::s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex,
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

  pSkillFld = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[byEffectCode].GetRecord(wSkillIndex));
  int nAttType = 0;
  if (byEffectCode)
  {
    nAttType = pSkillFld->m_nAttType[0];
  }
  else
  {
    const int skillLv = m_pmMst.GetSkillLv(pSkillFld->m_dwIndex);
    nAttType = GetSkillAttackTypeForLevel(pSkillFld, skillLv);
  }

  CCharacter *pTarget = pDst;
  unsigned __int16 decPoint[3]{};
  if (nAttType == 4)
  {
    pTarget = nullptr;
    std::memcpy(pfAttackPos, m_fCurPos, sizeof(float) * 3);
  }
  if (pTarget)
  {
    std::memcpy(pfAttackPos, pTarget->m_fCurPos, sizeof(float) * 3);
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
      Stop();
      SendMsg_BreakStop();
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

  const float effPlus = m_EP.GetEff_Plus(EFF_PLUS_SKILL_ACT_DELAY);
  _ATTACK_DELAY_CHECKER::SetDelay(&m_AttDelayChker, static_cast<unsigned int>(pSkillFld->m_fActDelay + effPlus));

  if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_14))
  {
    RemoveSFContHelpByEffect(2, 14);
  }
  if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_21))
  {
    if (pAP.nAttactType != 4 && pAP.nAttactType != 5 && pAP.nAttactType != 6 && pAP.nAttactType != 7)
    {
      return;
    }
    RemoveSFContHelpByEffect(2, 21);
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
      CPlayer::s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex,
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
      CPlayer::s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex,
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
    const int level = static_cast<int>(m_Param.GetLevel());
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
      if (GetObjRace() == damaged->GetObjRace())
      {
        continue;
      }
    }

    if (GetObjRace() == damaged->GetObjRace() && !damaged->m_ObjID.m_byID)
    {
      CPlayer *damagedPlayer = static_cast<CPlayer *>(damaged);
      if (damagedPlayer->IsPunished(1u, 0))
      {
        continue;
      }
    }

    if (!pAt.m_bFailure && !byEffectCode && pSkillFld->m_nMastIndex < 8u)
    {
      const int dstLevel = static_cast<int>(damaged->GetLevel());
      if (IsPassMasteryLimitLvDiff(dstLevel))
      {
        const float damageRatio =
          static_cast<float>(pAt.m_DamList[j].m_nDamage)
          / static_cast<float>(damaged->GetMaxHP());
        if (damageRatio >= 0.0099999998f)
        {
          if (pSkillFld->m_nClass == 1)
          {
            const int level = static_cast<int>(damaged->GetLevel());
            masterySum += 2 * static_cast<int>(GetMasteryCumAfterAttack(level));
          }
          else
          {
            const int level = static_cast<int>(damaged->GetLevel());
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

  if (pTarget && m_pRecalledAnimusChar && pTarget->GetHP() > 0)
  {
    m_pRecalledAnimusChar->MasterAttack_MasterInform(pTarget);
  }
  if (pTarget && pTarget->GetHP() > 0)
  {
    m_pmTwr.NotifyOwnerAttackInform(pTarget);
  }

  if (IsSiegeMode())
  {
    _STORAGE_LIST::_db_con *pItem = m_pSiegeItem;
    if (!Emb_AlterDurPoint(0, m_pSiegeItem->m_byStorageIndex, -1, 0, 1))
    {
      CPlayer::s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex,
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
    std::memcpy(pfAreaPos, pDst->m_fCurPos, sizeof(float) * 3);
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
      Stop();
      SendMsg_BreakStop();
    }
    return;
  }

  const int attackType = pForceFld->m_nEffectGroup;
  if (attackType == 6 || attackType == 4)
  {
    pTarget = nullptr;
  }
  if (attackType == 4)
  {
    std::memcpy(tarPos, m_fCurPos, sizeof(float) * 3);
  }

  CAttack pAt(this);
  if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_14))
  {
    RemoveSFContHelpByEffect(2, 14);
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
  if (!pEffBulletItem || wEffBtSerial == 65535)
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
    bActiveSucc = WeaponSFActive(pAt.m_DamList, &pAt.m_nDamagedObjNum, &pAP.nShotNum, 65535);
  }
  pAt.SetActiveSucc(bActiveSucc);

  const float effPlus = m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_13);
  _ATTACK_DELAY_CHECKER::SetDelay(&m_AttDelayChker, static_cast<unsigned int>(pForceFld->m_fActDelay + effPlus));

  if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_21))
  {
    if (pAP.nAttactType != 4 && pAP.nAttactType != 5 && pAP.nAttactType != 6 && pAP.nAttactType != 7)
    {
      return;
    }
    RemoveSFContHelpByEffect(2, 21);
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
    const int level = static_cast<int>(m_Param.GetLevel());
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
      if (GetObjRace() == damaged->GetObjRace())
      {
        continue;
      }
    }

    if (GetObjRace() == damaged->GetObjRace() && !damaged->m_ObjID.m_byID)
    {
      CPlayer *damagedPlayer = static_cast<CPlayer *>(damaged);
      if (damagedPlayer->IsPunished(1u, 0))
      {
        continue;
      }
    }

    if (!pAt.m_bFailure && pForceFld->m_nMastIndex < 24)
    {
      const int dstLevel = static_cast<int>(damaged->GetLevel());
      if (IsPassMasteryLimitLvDiff(dstLevel))
      {
        const float damageRatio =
          static_cast<float>(pAt.m_DamList[j].m_nDamage)
          / static_cast<float>(damaged->GetMaxHP());
        if (damageRatio >= 0.0099999998f)
        {
          const int level = static_cast<int>(damaged->GetLevel());
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
      const float effHave = m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_6);
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

  if (pTarget && m_pRecalledAnimusChar && pTarget->GetHP() > 0)
  {
    m_pRecalledAnimusChar->MasterAttack_MasterInform(pTarget);
  }
  if (pTarget && pTarget->GetHP() > 0)
  {
    m_pmTwr.NotifyOwnerAttackInform(pTarget);
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
      CPlayer::s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex,
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
      Stop();
      SendMsg_BreakStop();
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

  if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_14))
  {
    RemoveSFContHelpByEffect(2, 14);
  }
  if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_21))
  {
    if (pAP.nAttactType != 4 && pAP.nAttactType != 5 && pAP.nAttactType != 6 && pAP.nAttactType != 7)
    {
      return;
    }
    RemoveSFContHelpByEffect(2, 21);
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
    pBulletParam->wBulletIndex = 65535;
    pBulletParam->wLeftNum = 65535;
  }

  SendMsg_AttackResult_Unit(&pAt, byWeaponPart, bulletIndex);
  kPartyExpNotify.Notify();

  for (int j = 0; j < pAt.m_nDamagedObjNum; ++j)
  {
    const int level = static_cast<int>(m_Param.GetLevel());
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
    std::memcpy(pfAttackPos, pDst->m_fCurPos, sizeof(float) * 3);
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
      Stop();
      SendMsg_BreakStop();
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
  if (!pEffBulletItem || effBulletIndex == 65535)
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
    effPlus = static_cast<int>(m_EP.GetEff_Plus(m_pmWpn.byWpClass + 9));
  }

  const int addDelay = static_cast<int>(CalcEquipAttackDelay());
  const int level = static_cast<int>(GetLevel());
  const unsigned int attackDelay = m_pmWpn.GetAttackDelay(level, addDelay);
  int delay = effPlus + static_cast<int>(attackDelay);
  if (m_pmWpn.byWpType == 7)
  {
    const float effPlusInner = m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_11);
    delay = static_cast<int>(static_cast<float>(delay) + effPlusInner);
  }
  _ATTACK_DELAY_CHECKER::SetDelay(&m_AttDelayChker, static_cast<unsigned int>(delay));

  if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_14))
  {
    RemoveSFContHelpByEffect(2, 14);
  }
  if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_21))
  {
    if (pAP.nAttactType != 4 && pAP.nAttactType != 5 && pAP.nAttactType != 6 && pAP.nAttactType != 7)
    {
      return;
    }
    RemoveSFContHelpByEffect(2, 21);
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
    const int level = static_cast<int>(m_Param.GetLevel());
    CCharacter *damaged = pAt.m_DamList[j].m_pChar;
    damaged->SetDamage(pAt.m_DamList[j].m_nDamage, this, level, pAt.m_bIsCrtAtt, -1, 0, true);

    if (IsChaosMode())
    {
      if (GetObjRace() == damaged->GetObjRace())
      {
        continue;
      }
    }

    if (GetObjRace() == damaged->GetObjRace() && !damaged->m_ObjID.m_byID)
    {
      CPlayer *damagedPlayer = static_cast<CPlayer *>(damaged);
      if (damagedPlayer->IsPunished(1u, 0))
      {
        continue;
      }
    }

    if (!pAt.m_bFailure)
    {
      const int dstLevel = static_cast<int>(damaged->GetLevel());
      if (IsPassMasteryLimitLvDiff(dstLevel))
      {
        const float damageRatio =
          static_cast<float>(pAt.m_DamList[j].m_nDamage)
          / static_cast<float>(damaged->GetMaxHP());
        if (damageRatio >= 0.029999999f)
        {
          const int level = static_cast<int>(damaged->GetLevel());
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
      CPlayer::s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex,
        pItem,
        m_szItemHistoryFileName);
    }
  }

  if (IsSiegeMode())
  {
    _STORAGE_LIST::_db_con *siegeItem = m_pSiegeItem;
    if (!Emb_AlterDurPoint(0, m_pSiegeItem->m_byStorageIndex, -1, 0, 1))
    {
      CPlayer::s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex,
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
      CPlayer::s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex,
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
    if (wBulletItemSerial != 65535)
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
          CPlayer::s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex,
            pItem,
            m_szItemHistoryFileName);
        }
      }
    }

    if (byEffectCode)
    {
      if (byEffectCode == 1)
      {
        _skill_fld *record = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[1].GetRecord(byEffectIndex));
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
          sfLevel =
            static_cast<unsigned __int8>(GetSFLevel(record->m_nLv, static_cast<unsigned int>(forceItem->m_dwDur)));
        }
      }
    }
    else
    {
      _skill_fld *record = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[1].GetRecord(byEffectIndex));
      if (record)
      {
        sfLevel = m_pmMst.GetSkillLv(static_cast<unsigned __int8>(record->m_dwIndex));
      }
    }

    if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_14))
    {
      RemoveSFContHelpByEffect(2, 14);
    }
    SendMsg_TestAttackResult(byEffectCode, byEffectIndex, bulletIndex, sfLevel, byWeaponPart, pzTar);
    return;
  }

  _unit_bullet_param &unitBullet = m_pUsingUnit->m_BulletParam[byWeaponPart];
  if (unitBullet.wLeftNum && unitBullet.wLeftNum != 65535)
  {
    bulletIndex = unitBullet.wBulletIndex;
    _UnitPart_fld *unitRecord = nullptr;
    if (byWeaponPart)
    {
      unitRecord = reinterpret_cast<_UnitPart_fld *>(g_Main.m_tblUnitPart[4].GetRecord(m_pUsingUnit->byPart[4]));
    }
    else
    {
      unitRecord = reinterpret_cast<_UnitPart_fld *>(g_Main.m_tblUnitPart[3].GetRecord(m_pUsingUnit->byPart[3]));
    }
    if (unitRecord)
    {
      if (unitBullet.wLeftNum >= unitRecord->m_nDurUnit)
      {
        unitBullet.wLeftNum = static_cast<unsigned __int16>(unitBullet.wLeftNum - unitRecord->m_nDurUnit);
      }
      else
      {
        unitBullet.wLeftNum = 0;
      }
      const unsigned __int16 leftNum = unitBullet.wLeftNum;
      SendMsg_AlterUnitBulletInform(byWeaponPart, leftNum);
      if (!unitBullet.wLeftNum)
      {
        unitBullet.wBulletIndex = static_cast<unsigned __int16>(-1);
        unitBullet.wLeftNum = static_cast<unsigned __int16>(-1);
      }

      if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_14))
      {
        RemoveSFContHelpByEffect(2, 14);
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
  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE))
  {
    errCode = -37;
  }
  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
  {
    errCode = -37;
  }
  if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_21))
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
      Stop();
      SendMsg_BreakStop();
    }
    return;
  }

  CAttack pAt(this);
  _attack_param pAP{};
  pAP.pDst = nullptr;
  std::memcpy(pAP.fArea, m_fCurPos, sizeof(pAP.fArea));
  pAP.nPart = static_cast<int>(GetAttackRandomPart());
  pAP.nTol = -1;
  pAP.nClass = 1;

  const float nGaMinAF = static_cast<float>(m_pmWpn.nGaMinAF);
  const float effRate32 = m_EP.GetEff_Rate(EFF_RATE_ATTACK_FORCE);
  const float minBase = nGaMinAF * effRate32;
  const float effRate29 = m_EP.GetEff_Rate(EFF_RATE_LAUNCHER_ATTACK);
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

  if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_14))
  {
    RemoveSFContHelpByEffect(2, 14);
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
    const int level = static_cast<int>(m_Param.GetLevel());
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
    if (entry->m_pChar->IsRewardExp() && entry->m_nDamage > 1)
    {
      const int nDam = entry->m_nActiveDamage + entry->m_nDamage;
      CalcExp(entry->m_pChar, nDam, kPartyExpNotify);
    }
  }
  return totalDamage;
}

void CPlayer::_check_dst_param_after_attack(int nTotalDam, CCharacter *pTarget)
{
  if (m_EP.GetEff_Rate(EFF_RATE_DAMAGE_TO_FP) > 1.0f)
  {
    const float base = static_cast<float>(nTotalDam);
    const float effRate = m_EP.GetEff_Rate(EFF_RATE_DAMAGE_TO_FP);
    const float rateBase = base * (effRate - 1.0f);
    const float rateMul = m_EP.GetEff_Rate(EFF_RATE_FP_GAIN);
    const int addFp = static_cast<int>(rateBase * rateMul);
    int newFp = m_Param.GetFP() + addFp;
    const int maxFp = GetMaxFP();
    if (newFp > maxFp)
    {
      newFp = GetMaxFP();
    }
    SetFP(newFp, false);
  }

  if (m_EP.GetEff_Rate(EFF_RATE_DAMAGE_TO_HP) > 1.0f)
  {
    const float base = static_cast<float>(nTotalDam);
    const float effRate = m_EP.GetEff_Rate(EFF_RATE_DAMAGE_TO_HP);
    const float rateBase = base * (effRate - 1.0f);
    const float rateMul = m_EP.GetEff_Rate(EFF_RATE_HP_GAIN);
    const int addHp = static_cast<int>(rateBase * rateMul);
    const int curHp = m_Param.GetHP();
    SetHP(curHp + addHp, false);
  }

  if (m_EP.GetEff_Rate(EFF_RATE_UNKNOWN_40) > 1.0f)
  {
    const float base = static_cast<float>(nTotalDam);
    const float effRate = m_EP.GetEff_Rate(EFF_RATE_UNKNOWN_40);
    const float rateBase = base * (effRate - 1.0f);
    const float rateMul = m_EP.GetEff_Rate(EFF_RATE_SP_GAIN);
    const int addSp = static_cast<int>(rateBase * rateMul);
    int newSp = m_Param.GetSP() + addSp;
    const int maxSp = GetMaxSP();
    if (newSp > maxSp)
    {
      newSp = GetMaxSP();
    }
    SetSP(newSp, false);
  }

  if (pTarget)
  {
    if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_13))
    {
      if (pTarget->GetHP() > 0)
      {
        CPlayer *targetPlayer = pTarget->m_ObjID.m_byID ? nullptr : static_cast<CPlayer *>(pTarget);
        if (pTarget->m_ObjID.m_byID
          || !targetPlayer->m_bInGuildBattle
          || !targetPlayer->m_bTakeGravityStone)
        {
          pTarget->SetStun(true);
          pTarget->SendMsg_StunInform();
        }
      }
    }
    else if (m_EP.GetEff_Rate(EFF_RATE_STUN_RATE) > 1.0f)
    {
      const unsigned __int16 stunRate = static_cast<unsigned __int16>(
        (m_EP.GetEff_Rate(EFF_RATE_STUN_RATE) - 1.0f) * 100.0f);
      if (stunRate > rand() % 1000 && pTarget->GetHP() > 0)
      {
        CPlayer *targetPlayer = pTarget->m_ObjID.m_byID ? nullptr : static_cast<CPlayer *>(pTarget);
        if (pTarget->m_ObjID.m_byID
          || !targetPlayer->m_bInGuildBattle
          || !targetPlayer->m_bTakeGravityStone)
        {
          pTarget->SetStun(true);
          pTarget->SendMsg_StunInform();
        }
      }
    }
  }
}

bool CPlayer::IsPassExpLimitLvDiff(int iDstLevel, bool *bGetAttackExp)
{
  *bGetAttackExp = true;
  const int diff = static_cast<int>(GetLevel()) - iDstLevel;
  const float effHave = m_EP.GetEff_Have(EFF_HAVE_EXP_LEVEL_DIFFERENCE_TOLERANCE);
  if (static_cast<float>(diff) > effHave + 3.0f)
  {
    *bGetAttackExp = false;
    return false;
  }
  if ((-10.0f - effHave) > static_cast<float>(diff))
  {
    if (!m_pPartyMgr->IsPartyMode())
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
    const int dstLevel = static_cast<int>(pMon->GetLevel());
    if (IsPassExpLimitLvDiff(dstLevel, &bGetAttackExp))
    {
      _monster_fld *record = pMon->m_pMonRec;
      if (!record)
      {
        return;
      }
      int remainHp = static_cast<int>(pMon->GetHP()) - nDam;
      if (pMon->IsBossMonster())
      {
        bGetAttackExp = false;
      }
      if (bGetAttackExp)
      {
        int dam = nDam;
        if (remainHp < 0)
        {
          remainHp = 0;
          dam = static_cast<int>(pMon->GetHP());
        }
        const float baseExp = (record->m_fExt * 0.69999999f) * (static_cast<float>(dam) / record->m_fMaxHP);
        if (IsRidingUnit())
        {
          const float expUnit = baseExp / 180.0f;
          const int level = static_cast<int>(pMon->GetLevel());
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
        const int emoState = pMon->GetEmotionState();
        if (emoState == 4)
        {
          killExp = record->m_fExt * 0.5f;
        }
        else
        {
          killExp = record->m_fExt * 0.30000001f;
        }

        if (m_pPartyMgr->IsPartyMode())
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
            const int level = static_cast<int>(members[j]->GetLevel());
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
              const int level = static_cast<int>(members[j]->GetLevel());
              float fExp = (killExp * static_cast<float>(level)) / levelSum;
              const int memberLevel = static_cast<int>(members[j]->GetLevel());
              const float partyRate = GetPartyExpDistributionRate(memberLevel, maxLevel, secondLevel);
              fExp = fExp + (partyRate * fExp);
              if (fExp >= 1.0f)
              {
                if (members[j]->IsRidingUnit())
                {
                  const float expUnit = fExp / 180.0f;
                  const int monLevel = static_cast<int>(pMon->GetLevel());
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
          const int level = static_cast<int>(pMon->GetLevel());
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
  _attack_error_result_zocl msg{};
  msg.sFailureCode = nErrCode;
  unsigned __int8 type[2]{5, 6};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_TestAttackResult(
  unsigned __int8 byEffectCode,
  unsigned __int8 byEffectIndex,
  unsigned __int16 wBulletItemIndex,
  unsigned __int8 byEffectLv,
  unsigned __int8 byWeaponPart,
  __int16 *pzTar)
{
  _attack_test_result_zocl msg{};
  msg.dwAtterSerial = m_dwObjSerial;
  msg.wBulletIndex = wBulletItemIndex;
  msg.byEffectCode = static_cast<char>(byEffectCode);
  msg.byEffectIndex = static_cast<char>(byEffectIndex);
  msg.byEffectLv = static_cast<char>(byEffectLv);
  msg.byWeaponPart = static_cast<char>(byWeaponPart);
  msg.zTar[0] = pzTar[0];
  msg.zTar[1] = pzTar[1];

  unsigned __int8 type[2]{5, 12};
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(sizeof(msg)), true);
}

void CPlayer::SendMsg_AlterWeaponBulletInform(unsigned __int16 wItemSerial, unsigned __int16 wLeftNum)
{
  _alter_weapon_bullet_inform_zocl msg{};
  msg.wItemSerial = wItemSerial;
  msg.wLeftNum = wLeftNum;
  unsigned __int8 type[2]{5, 21};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_AlterUnitBulletInform(char byPart, unsigned __int16 wLeftNum)
{
  _alter_unit_bullet_inform_zocl msg{};
  msg.byPart = byPart;
  msg.wLeftNum = wLeftNum;
  unsigned __int8 type[2]{5, 22};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_Recover()
{
  _recover_zocl msg{};
  msg.wHP = static_cast<unsigned __int16>(m_Param.GetHP());
  msg.wFP = static_cast<unsigned __int16>(m_Param.GetFP());
  msg.wST = static_cast<unsigned __int16>(m_Param.GetSP());
  msg.wDP = static_cast<unsigned __int16>(m_Param.GetDP());
  unsigned __int8 type[2]{11, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
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
    if (std::strcmp(pConsumeList[j].m_itmNeedItemCode, "-1") && needCount >= 0)
    {
      if (pItemSerials[j] == 255)
      {
        return 0;
      }
      _STORAGE_LIST::_db_con *item = m_Param.m_dbInven.GetPtrFromSerial(pItemSerials[j]);
      if (!item)
      {
        return 0;
      }
      _base_fld *record = g_Main.m_tblItemData[item->m_byTableCode].GetRecord(item->m_wItemIndex);
      if (std::strcmp(pConsumeList[j].m_itmNeedItemCode, record->m_strCode))
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
          CPlayer::s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex,
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
        CPlayer::s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex,
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
      _DfnEquipItem_fld *record =
        reinterpret_cast<_DfnEquipItem_fld *>(g_Main.m_tblItemData[j].GetRecord(equip->m_wItemIndex));
      if (record)
      {
        totalDelay += static_cast<unsigned int>(record->m_nGASpd);
      }
    }
  }

  _STORAGE_LIST::_db_con *equip6 = m_Param.m_dbEquip.m_pStorageList + 6;
  if (equip6->m_bLoad)
  {
    _WeaponItem_fld *record = reinterpret_cast<_WeaponItem_fld *>(g_Main.m_tblItemData[6].GetRecord(equip6->m_wItemIndex));
    if (record)
    {
      totalDelay += static_cast<unsigned int>(record->m_nGASpd);
    }
  }

  _STORAGE_LIST::_db_con *equip5 = m_Param.m_dbEquip.m_pStorageList + 5;
  if (equip5->m_bLoad)
  {
    _DfnEquipItem_fld *record =
      reinterpret_cast<_DfnEquipItem_fld *>(g_Main.m_tblItemData[5].GetRecord(equip5->m_wItemIndex));
    if (record)
    {
      totalDelay += static_cast<unsigned int>(record->m_nGASpd);
    }
  }

  if (IsSiegeMode())
  {
    _SiegeKitItem_fld *record =
      reinterpret_cast<_SiegeKitItem_fld *>(g_Main.m_tblItemData[27].GetRecord(m_pSiegeItem->m_wItemIndex));
    if (record)
    {
      totalDelay += static_cast<unsigned int>(record->m_nGACorSpd);
    }
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

__int64 CPlayer::GetDamageLevel(int nAttackPart)
{
  if (IsRidingUnit() || nAttackPart > 5)
  {
    return 1;
  }

  _STORAGE_LIST::_db_con *equip = &m_Param.m_dbEquip.m_pStorageList[nAttackPart];
  if (!equip->m_bLoad)
  {
    return 1;
  }

  _DfnEquipItem_fld *record = reinterpret_cast<_DfnEquipItem_fld *>(
    g_Main.m_tblItemData[nAttackPart].GetRecord(equip->m_wItemIndex));
  if (record)
  {
    return static_cast<unsigned int>(record->m_nLevelLim);
  }

  return 1;
}

__int64 CPlayer::GetDamageDP(int nAttackPart)
{
  if (IsRidingUnit() || nAttackPart > 5)
  {
    return 0;
  }

  _DfnEquipItem_fld *record = nullptr;
  _STORAGE_LIST::_db_con *equip = &m_Param.m_dbEquip.m_pStorageList[nAttackPart];
  if (equip->m_bLoad)
  {
    record = reinterpret_cast<_DfnEquipItem_fld *>(g_Main.m_tblItemData[nAttackPart].GetRecord(equip->m_wItemIndex));
  }
  else if (nAttackPart < 5)
  {
    record = reinterpret_cast<_DfnEquipItem_fld *>(
      g_Main.m_tblItemData[nAttackPart].GetRecord(m_Param.m_dbChar.m_byDftPart[nAttackPart]));
  }

  if (record)
  {
    return static_cast<unsigned int>(record->m_nDefence_DP);
  }

  return 0;
}

__int64 CPlayer::GetAttackDP()
{
  if (IsRidingUnit())
  {
    _UnitPart_fld *record = reinterpret_cast<_UnitPart_fld *>(g_Main.m_tblUnitPart[3].GetRecord(m_pUsingUnit->byPart[3]));
    if (record)
    {
      return static_cast<unsigned int>(record->m_nAttack_DP);
    }
  }
  else
  {
    _STORAGE_LIST::_db_con *equip = m_Param.m_dbEquip.m_pStorageList + 6;
    if (equip->m_bLoad)
    {
      _WeaponItem_fld *record = reinterpret_cast<_WeaponItem_fld *>(g_Main.m_tblItemData[6].GetRecord(equip->m_wItemIndex));
      if (record)
      {
        return static_cast<unsigned int>(record->m_nAttack_DP);
      }
    }
  }
  return 0;
}

__int64 CPlayer::GetAttackLevel()
{
  if (IsRidingUnit())
  {
    _UnitPart_fld *record = reinterpret_cast<_UnitPart_fld *>(g_Main.m_tblUnitPart[3].GetRecord(m_pUsingUnit->byPart[3]));
    if (record)
    {
      return static_cast<unsigned int>(record->m_nLevelLim);
    }
  }
  else
  {
    _STORAGE_LIST::_db_con *equip = m_Param.m_dbEquip.m_pStorageList + 6;
    if (equip->m_bLoad)
    {
      _WeaponItem_fld *record = reinterpret_cast<_WeaponItem_fld *>(g_Main.m_tblItemData[6].GetRecord(equip->m_wItemIndex));
      if (record)
      {
        return static_cast<unsigned int>(record->m_nLevelLim);
      }
    }
  }
  return 1;
}

float CPlayer::GetAttackRange()
{
  return static_cast<float>(m_pmWpn.wGaAttRange);
}

__int64 CPlayer::GetWeaponRange()
{
  return static_cast<unsigned int>(m_pmWpn.wGaAttRange) - 40;
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
    masteryPerMast = static_cast<float>(m_pmMst.GetMasteryPerMast(0, m_pmWpn.byWpClass));
  }

  float baseValue = FLOAT_1_0;
  if (IsRidingUnit())
  {
    _UnitPart_fld *record = reinterpret_cast<_UnitPart_fld *>(
      g_Main.m_tblUnitPart[m_byUsingWeaponPart].GetRecord(m_pUsingUnit->byPart[m_byUsingWeaponPart]));
    if (record)
    {
      baseValue = static_cast<float>(record->m_nAttMastery);
    }
    _UnitPart_fld *baseRecord = reinterpret_cast<_UnitPart_fld *>(g_Main.m_tblUnitPart[0].GetRecord(m_pUsingUnit->byPart[0]));
    if (baseRecord)
    {
      baseValue = baseValue + static_cast<float>(baseRecord->m_nAttMastery);
    }
  }
  else
  {
    const int level = static_cast<int>(m_Param.GetLevel());
    baseValue = static_cast<float>(level) + masteryPerMast;
  }

  float defSkill = 0.0f;
  if (pDst->m_ObjID.m_byID)
  {
    if (pDst->m_ObjID.m_byID == 1)
    {
      defSkill = static_cast<float>(pDst->GetDefSkill(bBackAttack));
    }
  }
  else
  {
    int scale = 1;
    CPlayer *targetPlayer = static_cast<CPlayer *>(pDst);
    if (targetPlayer->m_bInGuildBattle)
    {
      scale = 2;
    }
    const int dstLevel = targetPlayer->m_Param.GetLevel();
    const int extra = scale * dstLevel;
    defSkill = static_cast<float>(static_cast<int>(pDst->GetDefSkill(bBackAttack)) + extra);
  }

  int attackProb = static_cast<int>((baseValue - defSkill) / 4.0f) + 95;
  const float partRates[5] = {0.23f, 0.22f, 0.18000001f, 0.17f, 0.2f};
  const float partMul = 5.0f * partRates[nPart];
  attackProb = static_cast<int>(static_cast<float>(attackProb) * partMul);

  int effPlus = 0;
  if (m_pmWpn.byWpType == 7)
  {
    effPlus = static_cast<int>(m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_2));
  }
  else
  {
    effPlus = static_cast<int>(m_EP.GetEff_Plus(m_pmWpn.byWpClass));
  }
  const int avoidRate = static_cast<int>(pDst->GetAvoidRate());
  const float effPlus2 = m_EP.GetEff_Plus(EFF_PLUS_ATTACK_ACCURACY);
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
  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE))
  {
    return false;
  }
  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
  {
    return false;
  }
  return !bFirst || !m_EP.GetEff_State(EFF_STATE_UNKNOWN_21);
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
    if (m_pmTrp.m_Item[j].isLoad())
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
    reinterpret_cast<char *>(&msg),
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
    reinterpret_cast<char *>(&msg),
    len);
}

void CPlayer::SendMsg_ThrowStorageResult(char byErrCode)
{
  _throw_storage_result_zocl msg{};
  msg.byErrCode = byErrCode;

  unsigned __int8 type[2]{7, 6};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_UsePotionResult(char byErrCode, unsigned __int16 wSerial, unsigned __int8 byLeftNum)
{
  _use_potion_result_zocl msg{};
  msg.byErrCode = byErrCode;
  msg.wPotionSerial = wSerial;
  msg.wHP = static_cast<unsigned __int16>(m_Param.GetHP());
  msg.wFP = static_cast<unsigned __int16>(m_Param.GetFP());
  msg.wSP = static_cast<unsigned __int16>(m_Param.GetSP());
  msg.byLeftNum = static_cast<char>(byLeftNum);

  unsigned __int8 type[2]{7, 8};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_EquipPartResult(char byErrCode)
{
  _equip_part_result_zocl msg{};
  msg.byErrCode = byErrCode;

  unsigned __int8 type[2]{7, 10};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_EmbellishResult(char byErrCode)
{
  _embellish_result_zocl msg{};
  msg.byErrCode = byErrCode;

  unsigned __int8 type[2]{7, 12};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_OffPartResult(char byErrCode)
{
  _off_part_result_zocl msg{};
  msg.byErrCode = byErrCode;

  unsigned __int8 type[2]{7, 14};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_MakeItemResult(char byErrCode)
{
  _make_item_result_zocl msg{};
  msg.byErrCode = byErrCode;

  unsigned __int8 type[2]{7, 16};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_ItemUpgrade(char byErrCode)
{
  _upgrade_item_result_zocl msg{};
  msg.byErrCode = byErrCode;

  unsigned __int8 type[2]{7, 18};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_ItemDowngrade(char byErrCode)
{
  _downgrade_item_result_zocl msg{};
  msg.byErrCode = byErrCode;

  unsigned __int8 type[2]{7, 20};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_AddBagResult(char byErrCode)
{
  _add_bag_result_zocl msg{};
  msg.byErrCode = byErrCode;

  unsigned __int8 type[2]{7, 22};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_CombineItemResult(char byErrCode, unsigned int dwFee, _STORAGE_LIST::_db_con *pNewItem)
{
  _combine_item_result_zocl msg{};

  msg.byErrCode = byErrCode;
  msg.dwFee = dwFee;
  msg.dwLeftDalant = m_Param.GetDalant();
  msg.NewItem.byTableCode = pNewItem->m_byTableCode;
  msg.NewItem.wItemIndex = pNewItem->m_wItemIndex;
  msg.NewItem.dwDurPoint = pNewItem->m_dwDur;
  msg.NewItem.wSerial = pNewItem->m_wSerial;
  msg.NewItem.dwLv = pNewItem->m_dwLv;

  unsigned __int8 type[2]{7, 29};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
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
    msg.dwLeftDalant = m_Param.GetDalant();
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
  _exchange_item_result_zocl msg{};
  msg.byErrCode = byErrCode;
  msg.NewItem.byTableCode = static_cast<char>(pNewItem->m_byTableCode);
  msg.NewItem.wItemIndex = pNewItem->m_wItemIndex;
  msg.NewItem.dwDurPoint = pNewItem->m_dwDur;
  msg.NewItem.wSerial = pNewItem->m_wSerial;
  msg.NewItem.dwLv = pNewItem->m_dwLv;

  unsigned __int8 type[2]{7, 31};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_CombineItemExResult(_combine_ex_item_result_zocl *pSend)
{
  unsigned __int8 type[2]{7, 33};
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(pSend),
    308);
}

void CPlayer::SendMsg_CombineItemExAcceptResult(_combine_ex_item_accept_result_zocl *pSend)
{
  unsigned __int8 type[2]{7, 36};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(pSend), 1u);
}

void CPlayer::SendMsg_UseRadarResult(char byErrCode, unsigned __int16 wSerial, unsigned int dwDelay)
{
  _use_radar_result_zocl msg{};
  msg.byErrCode = byErrCode;
  msg.wRadarSerial = wSerial;
  msg.dwDelay = dwDelay;

  unsigned __int8 type[2]{7, 50};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
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
    std::memcpy(
      msg.CharInfo[index].m_fPos,
      radarResult.CharInfoList[index].m_fPos,
      sizeof(msg.CharInfo[index].m_fPos));
  }

  unsigned __int8 type[2]{7, 52};
  const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    len);
}

void CPlayer::SendMsg_FanfareItem(unsigned __int8 byGetType, _STORAGE_LIST::_db_con *pItem, CItemBox *pItemBox)
{
  if (!g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex))
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
  const char *charName = m_Param.GetCharNameW();
  std::strcpy(msg.strCharacterName, charName);
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
          const int targetRace = targetPlayer->m_Param.GetRaceCode();
          const int myRace = m_Param.GetRaceCode();
          if (targetRace == myRace)
          {
            g_Network.m_pProcess[0]->LoadSendMsg(
              targetPlayer->m_ObjID.m_wIndex,
              type,
              reinterpret_cast<char *>(&msg),
              31);
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
      31);
  }
}

void CPlayer::SendMsg_PartyLootItemInform(
  unsigned int dwTakerSerial,
  unsigned __int8 byTableCode,
  unsigned __int16 wItemIndex,
  unsigned __int8 byNum)
{
  _take_loot_item_inform_zocl msg{};
  msg.dwTakerSerial = dwTakerSerial;
  msg.byItemTableCode = static_cast<char>(byTableCode);
  msg.wItemIndex = wItemIndex;
  msg.byNum = static_cast<char>(byNum);

  unsigned __int8 type[2]{16, 30};
  CPartyPlayer **partyMembers = m_pPartyMgr->GetPtrPartyMember();
  if (partyMembers)
  {
    for (int index = 0; index < 8 && partyMembers[index]; ++index)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        partyMembers[index]->m_wZoneIndex,
        type,
        reinterpret_cast<char *>(&msg),
        sizeof(msg));
    }
  }
}

void CPlayer::SendMsg_Notify_Get_Golden_Box(
  char byBoxType,
  unsigned int dwCharSerial,
  const char *szCharName,
  _STORAGE_LIST::_db_con *pItem,
  bool bCircle)
{
  _notify_effect_for_get_gold_box_item_zocl msg{};

  msg.byBoxType = static_cast<unsigned __int8>(byBoxType);
  msg.byTableCode = pItem->m_byTableCode;
  msg.wItemIndex = pItem->m_wItemIndex;
  msg.byBoxDur = static_cast<unsigned __int8>(pItem->m_dwDur);
  msg.dwCharSerial = dwCharSerial;
  std::strcpy(msg.szCharacterName, szCharName);

  unsigned __int8 type[2]{13, static_cast<unsigned __int8>(149)};
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
          26);
      }
    }
  }
}

void CPlayer::SendMsg_Notify_Me_Get_Golden_Box(char byBoxType, _STORAGE_LIST::_db_con *pItem)
{
  _notify_effect_for_get_gold_box_item_zocl msg{};

  msg.byBoxType = static_cast<unsigned __int8>(byBoxType);
  msg.byTableCode = pItem->m_byTableCode;
  msg.wItemIndex = pItem->m_wItemIndex;
  msg.byBoxDur = static_cast<unsigned __int8>(pItem->m_dwDur);
  msg.dwCharSerial = m_Param.GetCharSerial();
  const char *charName = m_Param.GetCharNameA();
  std::strcpy(msg.szCharacterName, charName);

  unsigned __int8 type[2]{13, static_cast<unsigned __int8>(149)};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
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
// this is not a stub
}

void CPlayer::SendMsg_CharacterRenameCashResult(char bChange, char byErrCode)
{
  _character_rename_cash_result_zocl msg{};
  msg.bChange = bChange != 0;
  msg.byErrorCode = byErrCode;

  unsigned __int8 type[2]{7, 67};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_SetItemCheckResult(char byResult, unsigned int dwSetItem, unsigned __int8 bySetEffectNum)
{
  _set_item_check_result_zocl msg{};
  msg.byResult = byResult;
  msg.dwSetIndex = dwSetItem;
  msg.bySetEffectNum = static_cast<char>(bySetEffectNum);

  unsigned __int8 type[2]{7, 46};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_NPCLinkItemCheckResult(unsigned __int8 byResCode, _STORAGE_POS_INDIV *pStorage)
{
  _npclink_check_item_result_zocl msg{};
  std::memset(&msg, 0, sizeof(msg));
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

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    errCode = 16;
  }
  else if (now - m_dwLastTakeItemTime < 400)
  {
    errCode = 9;
  }
  else if (!pBox->m_bLive)
  {
    errCode = 4;
  }
  else if (!pBox->IsTakeRight(this))
  {
    errCode = 5;
  }
  else if (GetSqrt(pBox->m_fCurPos, m_fCurPos) > 100.0)
  {
    errCode = 6;
  }
  else
  {
    std::memcpy(&item, &pBox->m_Item, sizeof(item));
    saveItem = IsSaveItem(item.m_byTableCode);
    if (item.m_byTableCode == 19)
    {
      errCode = 8;
    }
    else if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE) || m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
    {
      errCode = 13;
    }
    else if (saveItem)
    {
      if (wAddSerial == 65535)
      {
        if (m_Param.m_dbInven.GetIndexEmptyCon() == 255)
        {
          errCode = 1;
        }
      }
      else if (!IsOverLapItem(item.m_byTableCode))
      {
        errCode = 7;
      }
      else
      {
        addTarget = m_Param.m_dbInven.GetPtrFromSerial(wAddSerial);
        if (!addTarget)
        {
          SendMsg_AdjustAmountInform(0, wAddSerial, 0);
          errCode = 2;
        }
        else if (addTarget->m_byCsMethod && item.m_byCsMethod && addTarget->m_dwT != item.m_dwT)
        {
          if (m_Param.m_dbInven.GetIndexEmptyCon() == 255)
          {
            errCode = 1;
          }
          else
          {
            wAddSerial = 65535;
          }
        }
        else if (addTarget->m_bLock)
        {
          errCode = 12;
        }
        else if (addTarget->m_byTableCode == item.m_byTableCode && addTarget->m_wItemIndex == item.m_wItemIndex)
        {
          if (item.m_dwDur + addTarget->m_dwDur <= MAX_ITEM_OVERLAP)
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
  }
  if (!errCode)
  {
    m_dwLastTakeItemTime = now;
    if (saveItem)
    {
      if (wAddSerial == 65535)
      {
        item.m_wSerial = m_Param.GetNewItemSerial();
        addedItem = Emb_AddStorage(0, &item, false, true);
        if (!addedItem)
        {
          SendMsg_TakeNewResult(255, nullptr);
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
        _base_fld *record = g_Main.m_tblItemData[item.m_byTableCode].GetRecord(item.m_wItemIndex);
        if (!record)
        {
          return;
        }
        if (!std::strcmp(record->m_strCode, "iyhol01"))
        {
          const unsigned __int8 numOfTime = g_HolySys.GetNumOfTime();
          m_pUserDB->Update_UserGetScaner(1u, numOfTime);
        }
      }
      CGoldenBoxItemMgr *goldenMgr = CGoldenBoxItemMgr::Instance();
      if (goldenMgr->Get_Event_Status() == 2 && m_pUserDB && item.m_byTableCode == 31)
      {
        _base_fld *record = g_Main.m_tblItemData[item.m_byTableCode].GetRecord(item.m_wItemIndex);
        if (!record)
        {
          return;
        }
        if (!std::strcmp(record->m_strCode, "bxgol03") && pBox->m_dwThrowerCharSerial == static_cast<unsigned int>(-1))
        {
          char *charName = m_Param.GetCharNameA();
          const unsigned int charSerial = m_Param.GetCharSerial();
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
          _base_fld *monRecord = g_Main.m_tblMonster.GetRecord(pBox->m_wMonRecIndex);
          _base_fld *itemRecord = g_Main.m_tblItemData[item.m_byTableCode].GetRecord(item.m_wItemIndex);
          const unsigned int nowSec = timeGetTime() / 1000;
          char *charName = m_Param.GetCharNameA();
          CMonster::s_logTrace_Boss_Looting.Write(
            "\t Mob: %s Item: %s => %s ( %d ), Sec: %d",
            monRecord->m_strCode,
            itemRecord->m_strCode,
            charName,
            m_dwObjSerial,
            nowSec);
        }
        CPlayer::s_MgrItemHistory.take_ground_item(m_ObjID.m_wIndex,
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
          char *charName = m_Param.GetCharNameA();
          CPlayer::s_MgrItemHistory.trans_ground_item(&item,
            charName,
            m_dwObjSerial,
            takerId,
            pBox->m_szThrowerItemHistoryFileName);
        }
      }
    }
    _base_fld *itemRecord = g_Main.m_tblItemData[item.m_byTableCode].GetRecord(item.m_wItemIndex);
    m_QuestMgr.CheckFailLoop(4, itemRecord->m_strCode);
    bool darkHoleEvent = false;
    if (pBox->m_byCreateCode == 5 && m_pDHChannel)
    {
      darkHoleEvent = m_pDHChannel->CheckEvent(
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
        _base_fld *questItemRecord = g_Main.m_tblItemData[item.m_byTableCode].GetRecord(item.m_wItemIndex);
        Emb_CheckActForQuest(4, questItemRecord->m_strCode, 1u, 0);
        if (m_pPartyMgr->IsPartyMode())
        {
          Emb_CheckActForQuestParty(4, questItemRecord->m_strCode, 1u);
        }
        CheckMentalTakeAndUpdateLastMetalTicket(questItemRecord->m_strCode);
      }
      if (m_pPartyMgr->IsPartyMode()
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
    pBox->Destroy();
  }

  if (wAddSerial == 65535)
  {
    if (errCode || addedItem)
    {
      SendMsg_TakeNewResult(errCode, addedItem);
    }
    else
    {
      g_Main.m_logSystemError.Write(
        "pc_TakeGroundingItem() : Error Take New Item Result : BoxItem TableCode(%u), ItemIndex(%u)",
        item.m_byTableCode,
        item.m_wItemIndex);
      SendMsg_TakeNewResult(15, addedItem);
    }
  }
  else if (errCode || addTarget)
  {
    SendMsg_TakeAddResult(errCode, addTarget);
  }
  else
  {
    g_Main.m_logSystemError.Write(
      "pc_TakeGroundingItem() : Error Take Add Item Result : BoxItem TableCode(%u), ItemIndex(%u)",
      item.m_byTableCode,
      item.m_wItemIndex);
    SendMsg_TakeNewResult(15, addTarget);
  }
}

void CPlayer::pc_ThrowStorageItem(_STORAGE_POS_INDIV *pItem)
{
  unsigned __int8 errCode = 0;
  _STORAGE_LIST *storage = m_Param.m_pStoragePtr[pItem->byStorageCode];
  _STORAGE_LIST::_db_con *srcItem = nullptr;

  if (m_pUserDB)
  {
    if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE))
    {
      errCode = 5;
    }
    else if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
    {
      errCode = 5;
    }
    else
    {
      srcItem = storage->GetPtrFromSerial(pItem->wItemSerial);
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
          if (nationSetting->IsPersonalFreeFixedAmountBillingType(billingType, nullptr))
          {
            errCode = 9;
          }
          // Yorozuya box fix (non-IDA): reject zero-count overlap drops so the
          // throw path cannot create a ground item without consuming stack data.
          else if (IsOverLapItem(srcItem->m_byTableCode)
                   && (pItem->byNum == 0 || pItem->byNum > static_cast<unsigned __int64>(srcItem->m_dwDur)))
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
      std::memcpy(&dropItem, srcItem, sizeof(dropItem));
      if (IsOverLapItem(srcItem->m_byTableCode))
      {
        dropItem.m_dwDur = pItem->byNum;
      }
      CItemBox *itemBox = CreateItemBox(
        &dropItem,
        nullptr,
        -1,
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
          itemBox->Destroy();
          SendMsg_ThrowStorageResult(255);
          return;
        }
        _map_fld *mapRecord = m_pCurMap->m_pMapSet;
        CPlayer::s_MgrItemHistory.throw_ground_item(m_ObjID.m_wIndex,
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
      useItem = storage->GetPtrFromSerial(pItem->wItemSerial);
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
              SendMsg_PremiumCashItemUse(65535);
              SendMsg_UsePotionResult(25, pItem->wItemSerial, 0);
              return;
            }
            currTime = timeGetTime();
            potionFld = (_PotionItem_fld *)g_Main.m_tblItemData[13].GetRecord(useItem->m_wItemIndex);
            if (potionFld)
            {
              if (potionFld->m_nUseState == 3 || m_Param.GetHP())
              {
                const int level = m_Param.GetLevel();
                if (level >= potionFld->m_nLevelLim)
                {
                  if (potionFld->m_nUpLevelLim == -1
                      || m_Param.GetLevel() <= potionFld->m_nUpLevelLim)
                  {
                    errCode = static_cast<unsigned __int8>(
                      g_PotionMgr.UsePotion(this, pTargetPlayer, potionFld, currTime));
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
              char *charName = m_Param.GetCharNameA();
              g_Main.m_logSystemError.Write(
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
        CPlayer::s_MgrItemHistory.cash_item_use(m_ObjID.m_wIndex,
          useItem,
          m_szItemHistoryFileName);
      }
      _skill_fld *effectRecord =
        reinterpret_cast<_skill_fld *>(g_PotionMgr.m_tblPotionEffectData.GetRecord(potionFld->m_strEffCode));
      if (effectRecord && (effectRecord->m_nTempEffectType == 50 || effectRecord->m_nTempEffectType == 51))
      {
        CPlayer::s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex,
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
  _STORAGE_LIST::_db_con equipItem{};

  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE))
  {
    errCode = 8;
  }
  else if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
  {
    errCode = 8;
  }
  else
  {
    fixingItem = inventory->GetPtrFromSerial(pItem->wItemSerial);
    if (fixingItem)
    {
      record = g_Main.m_tblItemData[fixingItem->m_byTableCode].GetRecord(fixingItem->m_wItemIndex);
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
    std::memcpy(&invenItem, fixingItem, sizeof(invenItem));
    auto failEquipPart = [&]() {
      Emb_AddStorage(inventory->m_nListCode, &invenItem, true, false);
      SendMsg_EquipPartResult(255);
    };
    if (!Emb_DelStorage(
          inventory->m_nListCode,
          fixingItem->m_byStorageIndex,
          0,
          false,
          "CPlayer::pc_EquipPart() -- 0"))
    {
      SendMsg_EquipPartResult(255);
      return;
    }
    if (equipSlot->m_bLoad)
    {
      std::memcpy(&equipItem, equipSlot, sizeof(equipItem));
      if (!Emb_AddStorage(inventory->m_nListCode, &equipItem, true, false))
      {
        failEquipPart();
        return;
      }
      if (!Emb_DelStorage(1u, equipSlot->m_byStorageIndex, true, false, "CPlayer::pc_EquipPart() -- 1"))
      {
        Emb_DelStorage(inventory->m_nListCode, equipSlot->m_byStorageIndex, true, false, nullptr);
        Emb_AddStorage(inventory->m_nListCode, &invenItem, true, false);
        SendMsg_EquipPartResult(255);
        return;
      }
    }
    if (!Emb_AddStorage(1u, &invenItem, true, false))
    {
      if (equipSlot->m_bLoad)
      {
        Emb_DelStorage(inventory->m_nListCode, equipSlot->m_byStorageIndex, true, false, nullptr);
      }
      failEquipPart();
      return;
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
  _STORAGE_LIST::_db_con oldEmbel{};

  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE))
  {
    errCode = 8;
  }
  else if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
  {
    errCode = 8;
  }
  else
  {
    fixingItem = inventory->GetPtrFromSerial(pItem->wItemSerial);
    if (!fixingItem)
    {
      errCode = 2;
    }
    else if (fixingItem->m_byTableCode != 8 && fixingItem->m_byTableCode != 9 && fixingItem->m_byTableCode != 10)
    {
      errCode = 3;
    }
    else if (!_check_embel_part(fixingItem))
    {
      errCode = 4;
    }
    else
    {
      if (wChangeSerial != 65535)
      {
        embelItem = embellish->GetPtrFromSerial(wChangeSerial);
        if (!embelItem)
        {
          errCode = 2;
        }
        else if (embelItem->m_byTableCode != fixingItem->m_byTableCode)
        {
          errCode = 6;
        }
      }
      if (!errCode && fixingItem->m_bLock)
      {
        errCode = 10;
      }
      if (!errCode && embelItem && embelItem->m_bLock)
      {
        errCode = 10;
      }
      if (!errCode && wChangeSerial == 0xFFFF && embellish->GetIndexEmptyCon() == 255)
      {
        errCode = 5;
      }
      if (!errCode)
      {
        for (int index = 0; index < 7; ++index)
        {
          if (embellish->m_pStorageList[index].m_bLoad
              && embellish->m_pStorageList[index].m_byTableCode == fixingItem->m_byTableCode)
          {
            ++sameTypeCount;
          }
        }
      }
      if (!errCode && fixingItem->m_byTableCode == 10)
      {
        if ((wChangeSerial == 0xFFFF && sameTypeCount > 2)
            || (wChangeSerial != 0xFFFF && sameTypeCount > 3))
        {
          errCode = 5;
        }
      }
      else if (!errCode
               && ((wChangeSerial == 0xFFFF && sameTypeCount > 1)
                   || (wChangeSerial != 0xFFFF && sameTypeCount > 2)))
      {
        errCode = 5;
      }
      if (!errCode && fixingItem->m_byTableCode == 10)
      {
        const int cashType = GetUsePcCashType(fixingItem->m_byTableCode, fixingItem->m_wItemIndex);
        if (!IsUsableAccountType(cashType))
        {
          SendMsg_PremiumCashItemUse(65535);
          SendMsg_EmbellishResult(4u);
          return;
        }
      }
    }
  }
  if (!errCode)
  {
    _STORAGE_LIST::_db_con invenItem{};
    std::memcpy(&invenItem, fixingItem, sizeof(invenItem));
    auto failEmbellishPart = [&]() {
      Emb_AddStorage(inventory->m_nListCode, &invenItem, true, false);
      SendMsg_EmbellishResult(255);
    };
    if (!Emb_DelStorage(
          inventory->m_nListCode,
          fixingItem->m_byStorageIndex,
          0,
          false,
          "CPlayer::pc_EmbellishPart() -- 0"))
    {
      SendMsg_EmbellishResult(255);
      return;
    }
    if (embelItem)
    {
      std::memcpy(&oldEmbel, embelItem, sizeof(oldEmbel));
      if (!Emb_AddStorage(inventory->m_nListCode, &oldEmbel, true, false))
      {
        failEmbellishPart();
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
        SendMsg_EmbellishResult(255);
      }
    }
    if (!Emb_AddStorage(embellish->m_nListCode, &invenItem, true, false))
    {
      if (!embelItem)
      {
        failEmbellishPart();
        return;
      }
      _STORAGE_LIST::_db_con restoreEmbel{};
      std::memcpy(&restoreEmbel, embelItem, sizeof(restoreEmbel));
      Emb_AddStorage(embellish->m_nListCode, &restoreEmbel, true, false);
      Emb_DelStorage(
        inventory->m_nListCode,
        embelItem->m_byStorageIndex,
        false,
        false,
        "CPlayer::pc_EmbellishPart() -- 1 Fail");
      Emb_AddStorage(inventory->m_nListCode, &invenItem, true, false);
      SendMsg_EmbellishResult(255);
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

  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE))
  {
    errCode = 8;
  }
  else if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
  {
    errCode = 8;
  }
  else if (pItem->byStorageCode == 1 || pItem->byStorageCode == 2)
  {
    storage = m_Param.m_pStoragePtr[pItem->byStorageCode];
    srcItem = storage->GetPtrFromSerial(pItem->wItemSerial);
    if (srcItem)
    {
      if (srcItem->m_bLock)
      {
        errCode = 11;
      }
      else if (inventory->GetIndexEmptyCon() == 255)
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
    std::memcpy(&movedItem, srcItem, sizeof(movedItem));
    if (!Emb_DelStorage(storage->m_nListCode, srcItem->m_byStorageIndex, 0, false, "CPlayer::pc_OffPart()"))
    {
      SendMsg_OffPartResult(255);
      return;
    }
    if (!Emb_AddStorage(inventory->m_nListCode, &movedItem, true, false))
    {
      Emb_AddStorage(storage->m_nListCode, &movedItem, true, false);
      errCode = 11;
      SendMsg_OffPartResult(11);
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
      m_pmWpn.FixWeapon(item);
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
    m_pUserDB->Update_ItemUpgrade(byStorageCode, byStorageIndex, dwGradeInfo, 1);
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
  _ItemMakeData_fld *manualRecord = reinterpret_cast<_ItemMakeData_fld *>(g_Main.m_tblItemMakeData.GetRecord(wManualIndex));
  int tableCode = 0;
  _base_fld *makeItemRecord = nullptr;
  bool useRandomResult = false;
  int raceSexCode = 0;
  unsigned __int8 masteryType = 0;
  int masteryValue = 0;
  _STORAGE_LIST::_db_con *materialItems[100]{};
  _ItemMakeData_fld::_material_list requiredMaterial[5]{};
  _STORAGE_LIST::_db_con historyMaterials[100]{};
  unsigned __int8 materialCounts[116]{};

  do
  {
    if (!manualRecord)
    {
      errCode = 12;
      break;
    }

    tableCode = GetItemTableCode(manualRecord->m_strCode);

    raceSexCode = m_Param.GetRaceSexCode();
    if (manualRecord->m_strCivil[raceSexCode] != '1')
    {
      errCode = 11;
      break;
    }

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

    masteryValue = m_pmMst.GetMasteryPerMast(5u, masteryType);
    if (manualRecord->m_nMakeMastery > masteryValue)
    {
      errCode = 13;
      break;
    }
    if (GetItemKindCode(tableCode))
    {
      errCode = 9;
      break;
    }
    if (!m_bFreeSFByClass)
    {
      bool hasHistory = false;
      for (int historyIndex = 0; historyIndex < 4; ++historyIndex)
      {
        unsigned int *historyEffect =
          reinterpret_cast<unsigned int *>(*m_Param.m_ppHistoryEffect[historyIndex]);
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
        break;
      }
    }

    if (m_Param.GetPtrCurClass()->m_nGrade > 1
        && m_Param.GetPtrCurClass()->m_nClass == 3)
    {
      _class_fld *curClass = m_Param.GetPtrCurClass();
      _class_fld *baseClass = m_Param.GetPtrBaseClass();
      if (curClass->m_nClass == baseClass->m_nClass && m_Param.m_pClassData->m_dwIndex != 49)
      {
        useRandomResult = true;
      }
    }

    makeToolItem = m_Param.m_dbInven.GetPtrFromSerial(pipMakeTool->wItemSerial);
    if (!makeToolItem)
    {
      errCode = 1;
      break;
    }
    if (makeToolItem->m_byTableCode != 11)
    {
      errCode = 2;
      break;
    }
    if (makeToolItem->m_bLock)
    {
      errCode = 10;
      break;
    }
    if (m_Param.m_dbInven.GetIndexEmptyCon() == 255)
    {
      errCode = 3;
      break;
    }

    if (useRandomResult)
    {
      const unsigned int roll = rand() % 10000;
      unsigned int acc = 0;
      for (int entryIndex = 0; entryIndex < 30; ++entryIndex)
      {
        _ItemMakeData_fld::_output_list *entry = &manualRecord->m_listOutput[entryIndex];
        if (std::strncmp(entry->m_itmPdOutput, "-1", 2) == 0)
        {
          break;
        }
        acc += entry->m_dwPdProp;
        if (roll < acc)
        {
          const unsigned __int8 entryTableCode = GetItemTableCode(entry->m_itmPdOutput);
          if (entryTableCode != 255)
          {
            makeItemRecord = g_Main.m_tblItemData[entryTableCode].GetRecordByHash(entry->m_itmPdOutput, 2, 5);
          }
          break;
        }
      }
    }
    else
    {
      makeItemRecord = g_Main.m_tblItemData[tableCode].GetRecordByHash(manualRecord->m_strCode, 2, 5);
      if (!makeItemRecord)
      {
        errCode = 9;
        break;
      }
    }

    if (!makeItemRecord)
    {
      errCode = 12;
      break;
    }

    std::memcpy(requiredMaterial, manualRecord->m_listMaterial, sizeof(requiredMaterial));
    if (!m_bCheat_makeitem_no_use_matrial)
    {
      for (int j = 0; j < byMaterialNum; ++j)
      {
        materialItems[j] = m_Param.m_dbInven.GetPtrFromSerial(pipMaterials[j].wItemSerial);
        if (!materialItems[j])
        {
          SendMsg_AdjustAmountInform(0, pipMaterials[j].wItemSerial, 0);
          errCode = 4;
          break;
        }
        if (materialItems[j]->m_bLock)
        {
          errCode = 10;
          break;
        }
        if (static_cast<unsigned __int64>(pipMaterials[j].byNum) > materialItems[j]->m_dwDur)
        {
          SendMsg_AdjustAmountInform(
            0,
            pipMaterials[j].wItemSerial,
            static_cast<unsigned int>(materialItems[j]->m_dwDur));
          errCode = 5;
          break;
        }
        for (int m = 0; m < j; ++m)
        {
          if (pipMaterials[m].wItemSerial == pipMaterials[j].wItemSerial)
          {
            errCode = 5;
            break;
          }
        }
        if (errCode)
        {
          break;
        }
        bool matched = false;
        _base_fld *materialRecord =
          g_Main.m_tblItemData[materialItems[j]->m_byTableCode].GetRecord(materialItems[j]->m_wItemIndex);
        for (int n = 0; n < 5; ++n)
        {
          if (!std::strncmp(materialRecord->m_strCode, requiredMaterial[n].m_itmPdMat, 7u))
          {
            requiredMaterial[n].m_nPdMatNum -= pipMaterials[j].byNum;
            matched = true;
            break;
          }
        }
        if (!matched)
        {
          errCode = 8;
          break;
        }
      }
      if (errCode)
      {
        break;
      }
      for (int j = 0; j < 5; ++j)
      {
        if (requiredMaterial[j].m_nPdMatNum > 0)
        {
          errCode = 6;
          break;
        }
      }
    }
  } while (false);

  if (!errCode)
  {
    std::memset(historyMaterials, 0, sizeof(historyMaterials));
    std::memset(materialCounts, 0, sizeof(materialCounts));
    if (!m_bCheat_makeitem_no_use_matrial)
    {
      for (int j = 0; j < byMaterialNum; ++j)
      {
        std::memcpy(&historyMaterials[j], materialItems[j], sizeof(_STORAGE_LIST::_db_con));
        materialCounts[j] = pipMaterials[j].byNum;
      }
    }

    const int masteryValue = m_pmMst.GetMasteryPerMast(5u, masteryType);
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

    const unsigned __int16 randValue = m_MakeRandTable.GetRand();
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
const unsigned __int8 defSocketNum = GetDefItemUpgSocketNum(tableCode, makeItemRecord->m_dwIndex);
      unsigned __int8 socketCount = 0;
      if (defSocketNum)
      {
        socketCount = static_cast<unsigned __int8>(rand() % defSocketNum + 1);
      }
      newItem.m_dwLv = GetBitAfterSetLimSocket(socketCount);
      newItem.m_wSerial = m_Param.GetNewItemSerial();
      if (!Emb_AddStorage(0, &newItem, 0, 1))
      {
        SendMsg_MakeItemResult(255);
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
      _base_fld *itemRecord = g_Main.m_tblItemData[tableCode].GetRecord(makeItemRecord->m_dwIndex);
      Emb_CheckActForQuest(5, itemRecord->m_strCode, 1u, 0);
    }

    if (m_bCheat_makeitem_no_use_matrial)
    {
      CPlayer::s_MgrItemHistory.cheat_make_item_no_material(m_ObjID.m_wIndex,
        errCode,
        &newItem,
        m_szItemHistoryFileName);
    }
    else
    {
      CPlayer::s_MgrItemHistory.make_item(m_ObjID.m_wIndex,
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

  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE))
  {
    errCode = 8;
  }
  else if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
  {
    errCode = 8;
  }
  else
  {
    upgradeItem = storage->GetPtrFromSerial(pposUpgItem->wItemSerial);
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
          talikItem = m_Param.m_dbInven.GetPtrFromSerial(pposTalik->wItemSerial);
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
                primaryTalikRecord = g_Main.m_tblItemUpgrade.GetRecordFromRes(talikItem->m_wItemIndex);
                if (primaryTalikRecord)
                {
                  if (primaryTalikRecord->m_dwIndex < 13)
                  {
                    toolItem = m_Param.m_dbInven.GetPtrFromSerial(pposToolItem->wItemSerial);
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
                              m_Param.m_dbInven.GetPtrFromSerial(pposUpgJewel[j].wItemSerial);
                            if (!jewelItems[j])
                            {
                              SendMsg_AdjustAmountInform(0, pposUpgJewel[j].wItemSerial, 0);
                              errCode = 6;
                              break;
                            }
                            if (jewelItems[j]->m_byTableCode != 18)
                            {
                              errCode = 7;
                              break;
                            }
                            if (jewelItems[j]->m_bLock)
                            {
                              errCode = 13;
                              break;
                            }
                            jewelRecords[j] =
                              g_Main.m_tblItemUpgrade.GetRecordFromRes(jewelItems[j]->m_wItemIndex);
                            if (!jewelRecords[j])
                            {
                              return;
                            }
                            for (int k = 0; k < j; ++k)
                            {
                              if (jewelItems[k] == jewelItems[j])
                              {
                                errCode = 7;
                                break;
                              }
                            }
                            if (errCode)
                            {
                              break;
                            }
                            for (int k = 0; k < j; ++k)
                            {
                              if (jewelRecords[k] == jewelRecords[j])
                              {
                                errCode = 7;
                                break;
                              }
                            }
                            if (errCode)
                            {
                              break;
                            }
                            if (primaryTalikRecord == jewelRecords[j])
                            {
                              errCode = 7;
                              break;
                            }
                          }
                          if (!errCode && !IsAddAbleTalikToItem(
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
  if (!errCode)
  {
    _STORAGE_LIST::_db_con itemSnapshot{};
    _STORAGE_LIST::_db_con talikSnapshot{};
    _STORAGE_LIST::_db_con jewelSnapshot[4]{};
    std::memcpy(&itemSnapshot, upgradeItem, sizeof(itemSnapshot));
    std::memcpy(&talikSnapshot, talikItem, sizeof(talikSnapshot));
    for (int j = 0; j < byJewelNum; ++j)
    {
      std::memcpy(&jewelSnapshot[j], jewelItems[j], sizeof(_STORAGE_LIST::_db_con));
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
    if (g_Main.IsReleaseServiceMode() && m_byUserDgr)
    {
      recordUse = false;
    }
    if (recordUse)
    {
      CGameStatistics::_DAY *stats = g_GameStatistics.CurWriteData();
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

    if (successThreshold > (randomValue % 100000))
    {
      const unsigned int newGradeInfo =
        GetBitAfterUpgrade(upgradeItem->m_dwLv, primaryTalikRecord->m_dwIndex, curUpgLv);
      Emb_ItemUpgrade(0, storage->m_nListCode, upgradeItem->m_byStorageIndex, newGradeInfo);
      SendMsg_FanfareItem(0, upgradeItem, nullptr);
      _base_fld *record = g_Main.m_tblItemData[upgradeItem->m_byTableCode].GetRecord(upgradeItem->m_wItemIndex);
      Emb_CheckActForQuest(10, record->m_strCode, 1u, 0);

      bool recordSuccess = true;
      if (g_Main.IsReleaseServiceMode() && m_byUserDgr)
      {
        recordSuccess = false;
      }
      if (recordSuccess)
      {
        CGameStatistics::_DAY *stats = g_GameStatistics.CurWriteData();
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
          SendMsg_ItemUpgrade(255);
          return;
        }
        errCode = 102;
      }
    }

    CPlayer::s_MgrItemHistory.grade_up_item(m_ObjID.m_wIndex,
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

  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE))
  {
    errCode = 7;
  }
  else if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
  {
    errCode = 7;
  }
  else
  {
    upgradeItem = storage->GetPtrFromSerial(pposUpgItem->wItemSerial);
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
          talikItem = m_Param.m_dbInven.GetPtrFromSerial(pposTalik->wItemSerial);
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
                talikRecord = g_Main.m_tblItemUpgrade.GetRecordFromRes(talikItem->m_wItemIndex);
                if (talikRecord)
                {
                  if (talikRecord->m_nDataEffect == 14)
                  {
                    toolItem = m_Param.m_dbInven.GetPtrFromSerial(pposToolItem->wItemSerial);
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
    std::memcpy(&itemSnapshot, upgradeItem, sizeof(itemSnapshot));
    std::memcpy(&talikSnapshot, talikItem, sizeof(talikSnapshot));
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
      rewardItem.m_dwLv = 268435455;
      if (m_Param.m_dbInven.GetIndexEmptyCon() == 255)
      {
        CreateItemBox(
          &rewardItem,
          this,
          -1,
          false,
          nullptr,
          3u,
          m_pCurMap,
          m_wMapLayerIndex,
          m_fCurPos,
          false);
        CPlayer::s_MgrItemHistory.reward_add_item(m_ObjID.m_wIndex,
          "GradeDown Ground Reward",
          &rewardItem,
          m_szItemHistoryFileName);
      }
      else
      {
        rewardItem.m_wSerial = m_Param.GetNewItemSerial();
        if (!Emb_AddStorage(0, &rewardItem, 0, 1))
        {
          SendMsg_ItemDowngrade(255);
          CPlayer::s_MgrItemHistory.add_storage_fail(m_ObjID.m_wIndex,
            &rewardItem,
            "CPlayer::pc_DowngradeItem - Emb_AddStorage() Fail",
            m_szItemHistoryFileName);
          return;
        }
        SendMsg_RewardAddItem(&rewardItem, 0);
        CPlayer::s_MgrItemHistory.reward_add_item(m_ObjID.m_wIndex,
          "GradeDown Reward",
          &rewardItem,
          m_szItemHistoryFileName);
      }
    }
CPlayer::s_MgrItemHistory.grade_down_item(m_ObjID.m_wIndex,
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
  _ItemCombine_fld::_material requiredMaterial[5]{};
  _STORAGE_LIST::_db_con historyMaterials[100]{};
  unsigned __int8 materialCounts[117]{};
  unsigned __int8 defSocketNum = 0;
  unsigned __int8 socketCount = 0;
  _ItemCombine_fld *manualRecord =
    reinterpret_cast<_ItemCombine_fld *>(g_Main.m_tblItemCombineData.GetRecord(wManualIndex));
  unsigned __int8 itemTableCode = static_cast<unsigned __int8>(-1);
  _base_fld *combineRecord = nullptr;
  _STORAGE_LIST::_db_con *overlapItem = nullptr;

  do
  {
    if (!manualRecord)
    {
      errCode = 12;
      break;
    }

    const int raceSexCode = m_Param.GetRaceSexCode();
    if (manualRecord->m_strCivil[raceSexCode] != '1')
    {
      errCode = 11;
      break;
    }

    const unsigned int dalant = m_Param.GetDalant();
    if (manualRecord->m_dwFee > dalant)
    {
      errCode = 14;
      break;
    }
    if (m_Param.m_dbInven.GetIndexEmptyCon() == 255)
    {
      errCode = 3;
      break;
    }

    itemTableCode = GetItemTableCode(manualRecord->m_strCode);
    if (itemTableCode == 255)
    {
      errCode = 12;
      break;
    }

    combineRecord = g_Main.m_tblItemData[itemTableCode].GetRecordByHash(manualRecord->m_strCode, 2, 5);
    if (!combineRecord)
    {
      errCode = 12;
      break;
    }

    if (wOverlapItemSerial != 65535)
    {
      _STORAGE_LIST::_db_con *candidate = m_Param.m_dbInven.GetPtrFromSerial(wOverlapItemSerial);
      if (candidate && candidate->m_byTableCode == itemTableCode
          && candidate->m_wItemIndex == combineRecord->m_dwIndex
          && candidate->m_dwDur < MAX_ITEM_OVERLAP
          && !candidate->m_bLock)
      {
        overlapItem = candidate;
      }
    }

    std::memcpy(requiredMaterial, manualRecord->m_Material, sizeof(requiredMaterial));
    for (int j = 0; j < byMaterialNum; ++j)
    {
      materialItems[j] = m_Param.m_dbInven.GetPtrFromSerial(pipMaterials[j].wItemSerial);
      if (!materialItems[j])
      {
        SendMsg_AdjustAmountInform(0, pipMaterials[j].wItemSerial, 0);
        errCode = 4;
        break;
      }
      if (materialItems[j]->m_bLock)
      {
        errCode = 10;
        break;
      }
      if (static_cast<unsigned __int64>(pipMaterials[j].byNum) > materialItems[j]->m_dwDur)
      {
        SendMsg_AdjustAmountInform(
          0,
          pipMaterials[j].wItemSerial,
          static_cast<unsigned int>(materialItems[j]->m_dwDur));
        errCode = 5;
        break;
      }
      for (int k = 0; k < j; ++k)
      {
        if (pipMaterials[k].wItemSerial == pipMaterials[j].wItemSerial)
        {
          errCode = 5;
          break;
        }
      }
      if (errCode)
      {
        break;
      }

      bool matched = false;
      _base_fld *materialRecord =
        g_Main.m_tblItemData[materialItems[j]->m_byTableCode].GetRecord(materialItems[j]->m_wItemIndex);
      for (int m = 0; m < 5; ++m)
      {
        if (!std::strncmp(materialRecord->m_strCode, requiredMaterial[m].m_itmPdMat, 7u))
        {
          requiredMaterial[m].m_nDur -= pipMaterials[j].byNum;
          matched = true;
          break;
        }
      }
      if (!matched)
      {
        errCode = 8;
        break;
      }
    }
    if (errCode)
    {
      break;
    }

    for (int j = 0; j < 5; ++j)
    {
      if (requiredMaterial[j].m_nDur > 0)
      {
        errCode = 6;
        break;
      }
    }
    if (errCode)
    {
      break;
    }

    if (manualRecord->m_dwTradeMoney != 0xFFFFFFFFu && manualRecord->m_TradeValue == 6)
    {
      CGoldenBoxItemMgr *goldenBox = CGoldenBoxItemMgr::Instance();
      if (goldenBox->Get_Event_Status() == 2)
      {
        unsigned int actPoint = m_pUserDB->GetActPoint(2u);
        if (actPoint >= manualRecord->m_dwTradeMoney)
        {
          unsigned int curActPoint = m_pUserDB->GetActPoint(2u);
          m_pUserDB->SetActPoint(2u, curActPoint - manualRecord->m_dwTradeMoney);
          unsigned int leftActPoint = m_pUserDB->GetActPoint(2u);
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
  } while (false);

  unsigned int feeDalant = 0;
  _STORAGE_LIST::_db_con newItem{};
  const _TimeItem_fld *timeRec = nullptr;
  if (errCode)
  {
    if (timeRec && timeRec->m_nCheckType)
    {
      SendMsg_CombineLendItemResult(errCode, feeDalant, &newItem);
    }
    else
    {
      SendMsg_CombineItemResult(errCode, feeDalant, &newItem);
    }
    return;
  }

  std::memset(historyMaterials, 0, sizeof(historyMaterials));
  std::memset(materialCounts, 0, sizeof(materialCounts));
  for (int j = 0; j < byMaterialNum; ++j)
  {
    std::memcpy(&historyMaterials[j], materialItems[j], sizeof(_STORAGE_LIST::_db_con));
    materialCounts[j] = pipMaterials[j].byNum;
  }

  timeRec = TimeItem::FindTimeRec(itemTableCode, combineRecord->m_dwIndex);
  bool applyCombineCost = false;
  if (overlapItem && (!timeRec || !timeRec->m_nCheckType))
  {
    std::memcpy(&newItem, overlapItem, sizeof(newItem));
    ++newItem.m_dwDur;
    Emb_AlterDurPoint(0, overlapItem->m_byStorageIndex, 1, false, true);
    applyCombineCost = true;
  }
  else
  {
    newItem.m_byTableCode = itemTableCode;
    newItem.m_wItemIndex = combineRecord->m_dwIndex;
    newItem.m_dwDur = GetItemDurPoint(itemTableCode, combineRecord->m_dwIndex);
    materialCounts[116] = GetItemKindCode(itemTableCode);
    defSocketNum = GetDefItemUpgSocketNum(itemTableCode, combineRecord->m_dwIndex);
    socketCount = 0;
    if (defSocketNum)
    {
      socketCount = static_cast<unsigned __int8>(rand() % defSocketNum + 1);
    }
    newItem.m_dwLv = GetBitAfterSetLimSocket(socketCount);
    newItem.m_wSerial = m_Param.GetNewItemSerial();
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
      if (manualRecord->m_dwTradeMoney != 0xFFFFFFFFu && manualRecord->m_TradeValue == 6)
      {
        CGoldenBoxItemMgr *goldenBox = CGoldenBoxItemMgr::Instance();
        if (goldenBox->Get_Event_Status() == 2)
        {
          _base_fld *itemRecord =
            g_Main.m_tblItemData[newItem.m_byTableCode].GetRecord(newItem.m_wItemIndex);
          if (!itemRecord)
          {
            return;
          }
          if (!std::strcmp(itemRecord->m_strCode, "bxgol04"))
          {
            const char *charName = m_Param.GetCharNameA();
            const unsigned int charSerial = m_Param.GetCharSerial();
            SendMsg_Notify_Get_Golden_Box(5u, charSerial, charName, &newItem, 0);
          }
        }
      }
      applyCombineCost = true;
    }
    else
    {
      if (timeRec && timeRec->m_nCheckType)
      {
        SendMsg_CombineLendItemResult(255, feeDalant, &newItem);
      }
      else
      {
        SendMsg_CombineItemResult(255, feeDalant, &newItem);
      }
      CPlayer::s_MgrItemHistory.add_storage_fail(m_ObjID.m_wIndex,
        &newItem,
        "CPlayer::pc_CombineItem - Emb_AddStorage() Fail",
        m_szItemHistoryFileName);
      if (manualRecord->m_TradeValue == 6)
      {
        CGoldenBoxItemMgr *goldenBox = CGoldenBoxItemMgr::Instance();
        if (goldenBox->Get_Event_Status() == 2)
        {
          unsigned int actPoint = m_pUserDB->GetActPoint(2u);
          m_pUserDB->SetActPoint(2u, manualRecord->m_dwTradeMoney + actPoint);
          unsigned int leftActPoint = m_pUserDB->GetActPoint(2u);
          SendMsg_Alter_Action_Point(2u, manualRecord->m_dwTradeMoney + leftActPoint);
        }
      }
      return;
    }
  }
  if (applyCombineCost)
  {
    for (int j = 0; j < byMaterialNum; ++j)
    {
      Emb_AlterDurPoint(0, materialItems[j]->m_byStorageIndex, -pipMaterials[j].byNum, false, false);
    }
    feeDalant = manualRecord->m_dwFee;
    if (feeDalant)
    {
      SubDalant(feeDalant);
      const int level = m_Param.GetLevel();
      if (level == 30 || level == 40 || level == 50 || level == 60)
      {
        const int lv = m_Param.GetLevel();
        const int raceCode = m_Param.GetRaceCode();
        CMoneySupplyMgr *moneySupply = CMoneySupplyMgr::Instance();
        moneySupply->UpdateFeeMoneyData(static_cast<unsigned __int8>(raceCode), lv, feeDalant);
      }
    }
    SendMsg_ExchangeMoneyResult(0);
    {
      const unsigned int leftDalant = m_Param.GetDalant();
      CPlayer::s_MgrItemHistory.combine_item(m_ObjID.m_wIndex,
        historyMaterials,
        materialCounts,
        byMaterialNum,
        &newItem,
        feeDalant,
        leftDalant,
        m_szItemHistoryFileName);
    }
  }
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
  _STORAGE_LIST::_db_con *useItem = nullptr;
  _ItemExchangeData_fld *exchangeRecord = nullptr;
  _base_fld *useItemRecord = nullptr;
  _base_fld *outItemRecord = nullptr;
  unsigned __int8 useTableCode = 0;
  unsigned __int16 useItemIndex = 0;
  int cashType = 0;
  int newTableCode = -1;
  unsigned __int8 newDur = 0;

  exchangeRecord = reinterpret_cast<_ItemExchangeData_fld *>(g_Main.m_tblItemExchangeData.GetRecord(wManualIndex));
  do
  {
    useItem = m_Param.m_dbInven.GetPtrFromSerial(wItemSerial);
    if (!useItem)
    {
      errCode = 4;
      break;
    }
    if (useItem->m_bLock)
    {
      errCode = 10;
      break;
    }

    useItemRecord = g_Main.m_tblItemData[useItem->m_byTableCode].GetRecord(useItem->m_wItemIndex);
    if (!useItemRecord)
    {
      errCode = 12;
      break;
    }

    useTableCode = useItem->m_byTableCode;
    useItemIndex = useItem->m_wItemIndex;
    cashType = GetUsePcCashType(useItem->m_byTableCode, useItem->m_wItemIndex);
    if (!IsUsableAccountType(cashType))
    {
      SendMsg_PremiumCashItemUse(65535);
      errCode = 12;
      break;
    }

    // Yorozuya box fix (non-IDA): only consume exchange-box stacks with a
    // sane count so invalid stack values cannot duplicate the opened result.
    if (!useItem->m_dwDur || useItem->m_dwDur > 99)
    {
      errCode = 4;
      break;
    }

    if (useItem->m_byTableCode == 17)
    {
      auto *goldRecord = reinterpret_cast<_OreItem_fld *>(
        g_Main.m_tblItemData[17].GetRecord(useItem->m_wItemIndex));
      if (!goldRecord)
      {
        errCode = 12;
        break;
      }
      if (m_Param.m_dbInven.GetIndexEmptyCon() == 255)
      {
        errCode = 3;
        break;
      }

      if (goldRecord->m_dwOreProbability)
      {
        CGoldenBoxItemMgr *goldenBox = CGoldenBoxItemMgr::Instance();
        if (goldenBox->Get_Event_Status() != 2)
        {
          errCode = 12;
          break;
        }

        unsigned __int8 boxIndex = 0;
        char goldItems[640]{};
        CGoldenBoxItemMgr *goldBoxManager = CGoldenBoxItemMgr::Instance();
        char *goldItemPtr = goldBoxManager->GetGoldBoxItemPtr();
        std::memcpy(goldItems, goldItemPtr, 640);
        for (int j = 0;; ++j)
        {
          CGoldenBoxItemMgr *loopMgr = CGoldenBoxItemMgr::Instance();
          const unsigned __int8 loopCount = loopMgr->GetLoopCount();
          if (j >= loopCount)
          {
            break;
          }
          if (!std::strcmp(goldRecord->m_strCode, &goldItems[64 * j]))
          {
            boxIndex = static_cast<unsigned __int8>(j);
            break;
          }
        }

        CGoldenBoxItemMgr *boxMgr = CGoldenBoxItemMgr::Instance();
        char *itemCode = static_cast<char *>(boxMgr->BoxItemOpen(boxIndex));
        if (itemCode)
        {
          newTableCode = GetItemTableCode(itemCode);
          if (newTableCode < 0)
          {
            errCode = 12;
            break;
          }
          outItemRecord = g_Main.m_tblItemData[static_cast<unsigned __int8>(newTableCode)].GetRecordByHash(
            itemCode,
            2,
            5);
          newDur = static_cast<unsigned __int8>(itemCode[8]);
          if (outItemRecord)
          {
            CGoldenBoxItemMgr *updateMgr = CGoldenBoxItemMgr::Instance();
            updateMgr->Set_BoxItem_Count(boxIndex, outItemRecord->m_dwIndex);
          }
        }
      }
    }
    else
    {
      if (!exchangeRecord)
      {
        errCode = 12;
        break;
      }
      if (useItem->m_byTableCode != 31)
      {
        errCode = 12;
        break;
      }
      if (!CheckSameItemFromString_CodeIndex(exchangeRecord->m_strCode, useItem->m_byTableCode, useItem->m_wItemIndex))
      {
        errCode = 12;
        break;
      }

      auto *itemRecord = reinterpret_cast<_BoxItem_fld *>(
        g_Main.m_tblItemData[useItem->m_byTableCode].GetRecord(useItem->m_wItemIndex));
      if (!itemRecord)
      {
        errCode = 12;
        break;
      }

      const int level = m_Param.GetLevel();
      if (level < itemRecord->m_nLevelLim)
      {
        errCode = 21;
        break;
      }
      if (itemRecord->m_nUpLevelLim != -1)
      {
        const int curLevel = m_Param.GetLevel();
        if (curLevel > itemRecord->m_nUpLevelLim)
        {
          errCode = 21;
          break;
        }
      }
      if (m_Param.m_dbInven.GetIndexEmptyCon() == 255)
      {
        errCode = 3;
        break;
      }

      const unsigned int roll = rand() % 10000;
      unsigned int acc = 0;
      for (int k = 0; k < 61; ++k)
      {
        _ItemExchangeData_fld::_output *output = &exchangeRecord->m_listOutput[k];
        const char *entry = output->m_strOutCode;
        if (std::strlen(entry) < 2 || !std::strncmp(entry, "-", 1u) || !std::strncmp(entry, "-1", 2u))
        {
          break;
        }

        acc += output->m_dwProb;
        if (roll < acc)
        {
          newTableCode = GetItemTableCode(entry);
          if (newTableCode < 0)
          {
            break;
          }

          outItemRecord = g_Main.m_tblItemData[static_cast<unsigned __int8>(newTableCode)].GetRecordByHash(
            entry,
            2,
            5);
          newDur = output->m_byDur;
          if (outItemRecord)
          {
            char *itemEquipCivil =
              GetItemEquipCivil(static_cast<unsigned __int8>(newTableCode), outItemRecord->m_dwIndex);
            const int raceSexCode = m_Param.GetRaceSexCode();
            if (itemEquipCivil[raceSexCode] != '1')
            {
              const _ItemExchangeData_fld::_output *fallback = &exchangeRecord->m_listOutput[0];
              entry = fallback->m_strOutCode;
              newTableCode = GetItemTableCode(entry);
              outItemRecord = g_Main.m_tblItemData[static_cast<unsigned __int8>(newTableCode)].GetRecordByHash(
                entry,
                2,
                5);
              newDur = fallback->m_byDur;
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
  } while (false);

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
        SendMsg_ExchangeLendItemResult(255, &newItem);
        return;
      }
      SendMsg_ExchangeItemResult(255, &newItem);
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
    [[maybe_unused]] const unsigned __int8 itemKindCode =
      GetItemKindCode(static_cast<unsigned __int8>(newTableCode));
    const unsigned __int8 defSocketNum =
      GetDefItemUpgSocketNum(static_cast<unsigned __int8>(newTableCode), outItemRecord->m_dwIndex);
    unsigned __int8 socketCount = 0;
    if (defSocketNum)
    {
      socketCount = static_cast<unsigned __int8>(rand() % defSocketNum + 1);
    }
    newItem.m_dwLv = GetBitAfterSetLimSocket(socketCount);
    newItem.m_wSerial = m_Param.GetNewItemSerial();
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
        SendMsg_ExchangeLendItemResult(255, &newItem);
        return;
      }
      SendMsg_ExchangeItemResult(255, &newItem);
      return;
    }

    SendMsg_FanfareItem(2u, &newItem, nullptr);
    CGoldenBoxItemMgr *goldenBox = CGoldenBoxItemMgr::Instance();
    if (goldenBox->Get_Event_Status() == 2)
    {
      unsigned __int8 effectType[32]{};
      bool bCircle = true;
      _base_fld *useRecord =
        g_Main.m_tblItemData[useTableCode].GetRecord(useItemIndex);
      _base_fld *newRecord =
        g_Main.m_tblItemData[newItem.m_byTableCode].GetRecord(newItem.m_wItemIndex);
      if (!useRecord || !newRecord)
      {
        return;
      }
      char *newItemCode = newRecord->m_strCode;
      char *useItemCode = useRecord->m_strCode;
      CGoldenBoxItemMgr *effectMgr = CGoldenBoxItemMgr::Instance();
      effectMgr->BoxItemOpenEffectType(useItemCode, newItemCode, effectType, &bCircle);
      if (effectType[0] == 6 || effectType[0] == 8)
      {
        const char *charName = m_Param.GetCharNameA();
        const unsigned int charSerial = m_Param.GetCharSerial();
        SendMsg_Notify_Get_Golden_Box(effectType[0], charSerial, charName, &newItem, 0);
        if (effectType[0] == 6)
        {
          CGoldenBoxItemMgr *logMgr = CGoldenBoxItemMgr::Instance();
          logMgr->WriteGetGoldBarLog(this, &newItem);
        }
        else if (effectType[0] == 8)
        {
          CGoldenBoxItemMgr *logMgr = CGoldenBoxItemMgr::Instance();
          logMgr->WriteEventCouponLog(this, &newItem);
        }
      }
      else
      {
        SendMsg_Notify_Me_Get_Golden_Box(effectType[0], &newItem);
      }
    }

    CPlayer::s_MgrItemHistory.exchange_item(m_ObjID.m_wIndex,
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
    if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE))
    {
      errCode = 3;
    }
    else if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
    {
      errCode = 3;
    }
    else
    {
      bagItem = inventory->GetPtrFromSerial(wBagItemSerial);
      if (bagItem)
      {
        if (bagItem->m_byTableCode == 12)
        {
          if (bagItem->m_bLock)
          {
            errCode = 11;
          }
          else if (m_Param.GetBagNum() >= 5u)
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
      const unsigned __int8 oldBagNum = m_Param.GetBagNum();
      m_Param.SetBagNum(static_cast<unsigned __int8>(oldBagNum + 1));
      inventory->SetUseListNum(20 * m_Param.GetBagNum());
      if (!Emb_DelStorage(inventory->m_nListCode, bagItem->m_byStorageIndex, 0, true, "CPlayer::pc_AddBag()"))
      {
        m_Param.SetBagNum(oldBagNum);
        inventory->SetUseListNum(20 * m_Param.GetBagNum());
        SendMsg_AddBagResult(255);
        return;
      }
      CPlayer::s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex,
        bagItem,
        m_szItemHistoryFileName);
      if (m_pUserDB)
      {
        m_pUserDB->Update_BagNum(m_Param.GetBagNum());
      }
    }
    SendMsg_AddBagResult(errCode);
  }
}

char CPlayer::pc_UseRecoverLossExpItem(unsigned __int16 wItemSerial)
{
  if (m_Param.GetLossExp() == 0.0)
  {
    return -1;
  }
  if (GetCurSecNum() == -1 || m_bMapLoading)
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
  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE))
  {
    return -7;
  }
  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
  {
    return -7;
  }

  _STORAGE_LIST::_db_con *item = m_Param.m_dbInven.GetPtrFromSerial(wItemSerial);
  if (!item)
  {
    SendMsg_AdjustAmountInform(0, wItemSerial, 0);
    return -8;
  }
  if (item->m_byTableCode != 30)
  {
    return -8;
  }

  _RecoveryItem_fld *record = reinterpret_cast<_RecoveryItem_fld *>(g_Main.m_tblItemData[30].GetRecord(item->m_wItemIndex));
  if (!record)
  {
    return -8;
  }
  if (item->m_bLock)
  {
    return -9;
  }

  const int level = m_Param.GetLevel();
  if (level < record->m_nLevelLim)
  {
    return -9;
  }
  if (record->m_nUpLevelLim != -1 && m_Param.GetLevel() > record->m_nUpLevelLim)
  {
    return -9;
  }

  int range = record->m_nMaxRecoveryPro - record->m_nMinRecoveryPro + 1;
  if (range < 1)
  {
    range = 1;
  }
  const int prob = record->m_nMinRecoveryPro + rand() % range;

  long double alterExp = m_Param.GetLossExp() * (double)prob / 100.0;
  if (alterExp < 0.0)
  {
    alterExp = 0.0;
  }
  if (alterExp == 0.0)
  {
    return -10;
  }

  const long double oldExp = m_Param.GetExp();
  const unsigned int oldRate = m_dwExpRate;
  const long double oldLossExp = m_Param.GetLossExp();

  AlterExp(static_cast<double>(alterExp), 1, 1, 0);
  m_Param.SetLossExp(0.0);
  m_pUserDB->Update_LossExp(0.0);
  Emb_AlterDurPoint(0, item->m_byStorageIndex, -1, false, false);
  CPlayer::s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex,
    item,
    m_szItemHistoryFileName);

  CPlayer::s_MgrLvHistory.recovery_exp(
    m_ObjID.m_wIndex,
    oldExp,
    oldRate,
    m_Param.GetExp(),
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
  m_ItemCombineMgr.RequestCombineProcess(pRecv, &send);
  SendMsg_CombineItemExResult(&send);
}

void CPlayer::pc_CombineItemExAccept(_combine_ex_item_accept_request_clzo *pRecv)
{
  _combine_ex_item_accept_result_zocl send{};
  unsigned __int8 errCode = m_ItemCombineMgr.RequestCombineAcceptProcess(pRecv, &send);
  if (errCode)
  {
    send.byErrCode = errCode;
  }
  SendMsg_CombineItemExAcceptResult(&send);
}

__int64 CPlayer::pc_UseFireCracker(unsigned __int16 wItemSerial)
{
  if (GetCurSecNum() == -1 || m_bMapLoading)
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
  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE))
  {
    return -6;
  }
  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
  {
    return -6;
  }

  _STORAGE_LIST::_db_con *item = m_Param.m_dbInven.GetPtrFromSerial(wItemSerial);
  if (!item)
  {
    SendMsg_AdjustAmountInform(0, wItemSerial, 0);
    return -7;
  }
  if (item->m_byTableCode != 32)
  {
    return -7;
  }
  if (!g_Main.m_tblItemData[32].GetRecord(item->m_wItemIndex))
  {
    return -7;
  }
  if (item->m_bLock)
  {
    return -8;
  }

  Emb_AlterDurPoint(0, item->m_byStorageIndex, -1, false, false);
  CPlayer::s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex,
    item,
    m_szItemHistoryFileName);
  return item->m_wItemIndex;
}

unsigned __int8 CPlayer::pc_UserSoccerBall(unsigned __int16 wItemSerial, unsigned __int16 *wItemIndex)
{
  if (GetCurSecNum() == -1 || m_bMapLoading)
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
  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE))
  {
    return 6;
  }
  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
  {
    return 6;
  }

  _STORAGE_LIST::_db_con *item = m_Param.m_dbInven.GetPtrFromSerial(wItemSerial);
  if (!item)
  {
    SendMsg_AdjustAmountInform(0, wItemSerial, 0);
    return 7;
  }
  if (item->m_byTableCode != 27)
  {
    return 7;
  }
  if (!g_Main.m_tblItemData[27].GetRecord(item->m_wItemIndex))
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

  if (m_Param.GetHP())
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
      radarItem = storage->GetPtrFromSerial(pItem->wItemSerial);
      if (radarItem)
      {
        if (radarItem->m_byTableCode == 34)
        {
          radarFld = (_RadarItem_fld *)g_Main.m_tblItemData[34].GetRecord(radarItem->m_wItemIndex);
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
                m_pUserDB->CalcRadarDelay();
                if (radarMgr->IsUse())
                {
                  errCode = 8;
                }
                else
                {
                  std::memset(consumeItems, 0, sizeof(consumeItems));
                  std::memset(consumeCounts, 0, sizeof(consumeCounts));
                  std::memset(overlapFlags, 0, sizeof(overlapFlags));
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
                      CPlayer::s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex,
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
    radarMgr->SetUseRadar(
      reinterpret_cast<char (*)[64]>(radarFld->m_strCode),
      this,
      radarFld->m_nDuration,
      delay);
    m_pUserDB->SetRadarDelay(delay);
    radarMgr->RadarProc(radarFld);
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
  if (radarMgr->IsUse())
  {
    _RadarItem_fld *radarFld = (_RadarItem_fld *)g_Main.m_tblItemData[34].GetRecord(radarMgr->m_strRadarCode);
    if (!radarFld)
    {
      return 0;
    }
    radarMgr->RadarProc(radarFld);
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
  _STORAGE_LIST::_db_con *item = storage->GetPtrFromSerial(pStorage->wItemSerial);
  if (!item || !storage)
  {
    return 1;
  }
  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
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
  if (GetCurSecNum() == -1 || m_bMapLoading)
  {
    return 10;
  }
  if (m_bCorpse)
  {
    return 11;
  }
  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE))
  {
    return 12;
  }
  if (m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
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
  if (!g_Main.m_tblItemData[35].GetRecord(item->m_wItemIndex))
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
    _STORAGE_LIST::_db_con *useItem = storage->GetPtrFromSerial(pStorage->wItemSerial);
    _NPCLink_fld *record = reinterpret_cast<_NPCLink_fld *>(g_Main.m_tblItemData[35].GetRecord(useItem->m_wItemIndex));
    if (record)
    {
      if (!record->m_nStoragePart)
      {
        const int oldDur = static_cast<int>(useItem->m_dwDur);
        const unsigned int left = Emb_AlterDurPoint(storage->m_nListCode, useItem->m_byStorageIndex, -1, false, false);
        pStorage->byNum = static_cast<unsigned __int8>(left);
        if (oldDur - 1 == static_cast<int>(left))
        {
          if (record->m_bIsCash == 1)
          {
            CPlayer::s_MgrItemHistory.cash_item_use(m_ObjID.m_wIndex,
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
  manager->Doit(this, byExchangeNum, pList);
}

unsigned __int8 CPlayer::pc_RenameItemNConditionCheck(_STORAGE_POS_INDIV *pItemInfo, _STORAGE_LIST::_db_con **ppItem)
{
  if (!pItemInfo || *ppItem)
  {
    return 1;
  }

  _STORAGE_LIST *storage = m_Param.m_pStoragePtr[pItemInfo->byStorageCode];
  _STORAGE_LIST::_db_con *item = storage->GetPtrFromSerial(pItemInfo->wItemSerial);
  if (!item)
  {
    return 1;
  }
  if (!m_Param.GetHP())
  {
    return 2;
  }
  if (item->m_byTableCode != 13)
  {
    return 9;
  }
  if (m_Param.m_pGuild)
  {
    const unsigned int mySerial = m_Param.GetCharSerial();
    const unsigned int guildMasterSerial = m_Param.m_pGuild->GetGuildMasterSerial();
    if (mySerial == guildMasterSerial)
    {
      return 7;
    }
  }
  const int raceCode = m_Param.GetRaceCode();
  CPvpUserAndGuildRankingSystem *rankSystem = CPvpUserAndGuildRankingSystem::Instance();
  if (rankSystem->IsCurrentRaceBossGroup(raceCode, m_dwObjSerial))
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
    SendMsg_PremiumCashItemUse(65535);
    return 1;
  }

  if (bChange)
  {
    if (m_ReNamePotionUseInfo.wszChangeName[0])
    {
      const unsigned int currTime = timeGetTime();
      _PotionItem_fld *potionFld = (_PotionItem_fld *)g_Main.m_tblItemData[13].GetRecord(item->m_wItemIndex);
      if (potionFld)
      {
        _skill_fld *effectRecord =
          reinterpret_cast<_skill_fld *>(g_PotionMgr.m_tblPotionEffectData.GetRecord(potionFld->m_strEffCode));
        if (effectRecord && !errCode && effectRecord->m_nTempEffectType == 47)
        {
          item->lock(true);
          errCode = static_cast<unsigned __int8>(g_PotionMgr.UsePotion(this, this, potionFld, currTime));
          if (errCode)
          {
            m_ReNamePotionUseInfo.Init();
            SendMsg_CharacterRenameCashResult(1, errCode);
            return 0;
          }
          return 1;
        }
        m_ReNamePotionUseInfo.Init();
        SendMsg_CharacterRenameCashResult(1, 9u);
        return 0;
      }

      m_ReNamePotionUseInfo.Init();
      char *charName = m_Param.GetCharNameA();
      g_Main.m_logSystemError.Write(
        "CPlayer::pc_CharacterRenameCash(...) : User %s(%u) Item Table(%u) Index(%u) Not Exist!",
        charName,
        m_dwObjSerial,
        item->m_byTableCode,
        item->m_wItemIndex);
      errCode = 48;
      return 0;
    }

    m_ReNamePotionUseInfo.Init();
    SendMsg_CharacterRenameCashResult(1, 48);
    return 0;
  }

  errCode = pc_CharacterRenameCheck(strCharacterName);
  if (errCode)
  {
    SendMsg_CharacterRenameCashResult(0, errCode);
  }
  else
  {
    item->lock(true);
    _qry_case_character_rename qry{};
    qry.dwCharSerial = m_pUserDB->m_dwSerial;
    qry.dwAlreadySerial = static_cast<unsigned int>(-1);
    memcpy_s(&qry.ItemInfo, sizeof(qry.ItemInfo), &itemInfo, sizeof(itemInfo));
    strcpy_s(qry.wszCharName, 17, strCharacterName);
    const int size = static_cast<int>(sizeof(qry));
    if (!g_Main.PushDQSData(-1, nullptr, 145, reinterpret_cast<char *>(&qry), size))
    {
      SendMsg_CharacterRenameCashResult(0, 10);
    }
  }
  return 1;
}

char CPlayer::pc_CharacterRenameCheck(const char *strCharacterName)
{
  char errCode = 0;
  char dest[17]{};
  strncpy_s(dest, strCharacterName, 16u);
  if (dest[0] == '*' || dest[0] == '!')
  {
    errCode = 5;
  }
  for (int index = 0; index < 3; ++index)
  {
    if (!std::strcmp(dest, wszNonMakeName[index]))
    {
      errCode = 6;
      break;
    }
  }
  if (!IsSQLValidString(dest))
  {
    return 6;
  }

  CNationSettingManager *nationSetting = CTSingleton<CNationSettingManager>::Instance();
  if (!nationSetting->IsNormalString(dest))
  {
    return 5;
  }
  return errCode;
}

void CPlayer::ApplySetItemEffect(
  si_interpret *pSI,
  unsigned int dwSetItem,
  unsigned __int8 bySetItemNum,
  unsigned __int8 bySetEffectNum,
  bool bSetEffect)
{
for (unsigned __int8 idx = 0; idx < bySetEffectNum; ++idx)
  {
    const int effCode = pSI->GetEffectCode(idx);
    const float effValue = pSI->GetEffectValue(idx);
    apply_normal_item_std_effect(effCode, effValue, bSetEffect);
  }
}

void CPlayer::UpdateActiveSetItemEffects()
{
  // Yorozuya fix implementation (non-IDA): reconcile active set-item effects
  // from the current equip and embellish state after storage changes.
  if (!m_pUserDB)
  {
    return;
  }

  CSUItemSystem *suSystem = CSUItemSystem::Instance();
  if (!suSystem || !suSystem->GetCRecordData_SetItem())
  {
    return;
  }

  CSetItemType *setType = suSystem->GetCSetItemType();
  if (!setType)
  {
    return;
  }

  CRecordData *setRecordData = &suSystem->m_SUOrigin[0];
  const bool notifyClient = m_bLoad && m_bOper;
  const unsigned int recordCount = static_cast<unsigned int>(setRecordData->GetRecordNum());
  for (unsigned int setItemIndex = 0; setItemIndex < recordCount; ++setItemIndex)
  {
    si_interpret *si = setType->Getsi_interpret(static_cast<int>(setItemIndex));
    if (!si)
    {
      continue;
    }

    const unsigned __int8 effectTypeCount = si->GetEffectTypeCount();
    bool hasCurrent = false;
    unsigned __int8 currentItemNum = 0;
    unsigned __int8 currentEffectNum = 0;
    if (m_clsSetItem.IsSetOn(setItemIndex))
    {
      for (unsigned __int8 effectTypeIndex = 0; effectTypeIndex < effectTypeCount; ++effectTypeIndex)
      {
        const unsigned __int8 effectItemNum = si->GetCountOfItem(effectTypeIndex);
        const unsigned __int8 effectCount = si->GetCountOfEffect(effectTypeIndex);
        if (m_clsSetItem.IsSetOnComplete(setItemIndex, effectItemNum, effectCount))
        {
          hasCurrent = true;
          currentItemNum = effectItemNum;
          currentEffectNum = effectCount;
          break;
        }
      }
    }

    bool hasDesired = false;
    unsigned __int8 desiredItemNum = 0;
    unsigned __int8 desiredEffectNum = 0;
    _SetItemEff_fld *setField = reinterpret_cast<_SetItemEff_fld *>(setRecordData->GetRecord(setItemIndex));
    if (setField && setField->m_strCivil[m_Param.GetRaceSexCode()] != '0')
    {
      char itemCode[68]{};
      bool gradeOk = true;
      const int tableCode = suSystem->GetSetItemTableInfo(static_cast<int>(setItemIndex), itemCode, 64);
      if (tableCode > -1)
      {
        const unsigned __int8 grade = GetItemEquipGrade(tableCode, itemCode);
        gradeOk = IsEquipAbleGrade(grade) != 0;
      }

      if (gradeOk)
      {
        const unsigned __int8 equipCount = m_clsSetItem.Check_EquipItem(&m_pUserDB->m_AvatorData, setField);
        for (unsigned __int8 effectTypeIndex = 0; effectTypeIndex < effectTypeCount; ++effectTypeIndex)
        {
          const unsigned __int8 effectItemNum = si->GetCountOfItem(effectTypeIndex);
          if (effectItemNum <= equipCount)
          {
            hasDesired = true;
            desiredItemNum = effectItemNum;
            desiredEffectNum = si->GetCountOfEffect(effectTypeIndex);
          }
        }
      }
    }

    if (!hasDesired)
    {
      if (hasCurrent)
      {
        ProcessSetItemCheckRequest(setItemIndex, currentItemNum, currentEffectNum, false, notifyClient);
      }
      continue;
    }

    if (!hasCurrent || currentItemNum != desiredItemNum || currentEffectNum != desiredEffectNum)
    {
      // Yorozuya fix implementation (non-IDA): recompute the active set-item state
      // from live equip and embellish storage after each storage change.
      ProcessSetItemCheckRequest(setItemIndex, desiredItemNum, desiredEffectNum, true, notifyClient);
    }
  }
}

char CPlayer::ProcessSetItemCheckRequest(
  unsigned int dwSetItem,
  unsigned __int8 bySetItemNum,
  unsigned __int8 bySetEffectNum,
  bool bSet,
  bool bNotify)
{
  CSUItemSystem *suSystem = CSUItemSystem::Instance();
  CSetItemType *setType = suSystem->GetCSetItemType();
  if (!setType)
  {
    if (bNotify)
    {
      SendMsg_SetItemCheckResult(7u, 0, 0);
    }
    return 0;
  }

  si_interpret *si = setType->Getsi_interpret(dwSetItem);
  if (!si)
  {
    if (bNotify)
    {
      SendMsg_SetItemCheckResult(2u, 0, 0);
    }
    return 0;
  }

  int result = 9;
  if (bSet)
  {
    bool gradeOk = false;
    char strCode[68]{};
    const int tableCode = suSystem->GetSetItemTableInfo(dwSetItem, strCode, 64);
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
      result = static_cast<int>(
        m_clsSetItem.SetOnEffect(&m_pUserDB->m_AvatorData, dwSetItem, bySetItemNum, bySetEffectNum));
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
          const unsigned __int8 resetEffect = m_clsSetItem.GetResetEffectNum();
          const unsigned __int8 resetItem = m_clsSetItem.GetResetItemNum();
          const unsigned int resetIdx = static_cast<unsigned int>(m_clsSetItem.GetResetIdx());
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
    result = static_cast<int>(m_clsSetItem.SetOffEffect(dwSetItem, bySetItemNum, bySetEffectNum));
    if (result == 1)
    {
      ApplySetItemEffect(si, dwSetItem, bySetItemNum, bySetEffectNum, false);
    }
  }

  if (bNotify)
  {
    SendMsg_SetItemCheckResult(static_cast<char>(result), dwSetItem, bySetEffectNum);
  }
  return 1;
}

char CPlayer::pc_SetItemCheckRequest(
  unsigned int dwSetItem,
  unsigned __int8 bySetItemNum,
  unsigned __int8 bySetEffectNum,
  bool bSet)
{
  return ProcessSetItemCheckRequest(dwSetItem, bySetItemNum, bySetEffectNum, bSet, true);
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
  if (!std::strcmp(g_HolySys.m_strHolyMental, strItemCode))
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
  _make_item_cheatcode_setui_zocl msg{};
  msg.bEnableMakeUiButton = bEnableSendButton != 0;
  unsigned __int8 type[2] = {7, 34};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_AlterMoneyInform(char byReasonCode)
{

  _alter_money_inform_zocl msg{};
  msg.byReason = byReasonCode;
  msg.dwGold = m_Param.GetGold();
  msg.dwDalant = m_Param.GetDalant();

  unsigned __int8 type[2] = {11, 19};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
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
      std::memcpy(newPos, pStartPos, sizeof(newPos));
    }

    OutOfMap(pIntoMap, wMapLayerIndex, 4u, newPos);
    const unsigned __int8 mapCode = m_Param.GetMapCode();
    SendMsg_GotoRecallResult(0, mapCode, newPos, 4u);
  }
}

void CPlayer::SendMsg_HSKQuestActCum()
{

  _add_holyquest_actcum_inform_zocl msg{};
  msg.nPvpPoint = m_nHSKPvpPoint;
  msg.wKillPoint = m_wKillPoint;

  const unsigned __int8 byNumOfTime = g_HolySys.GetNumOfTime();
  const unsigned __int8 byCurrentHour = g_HolySys.GetStartHour();
  const unsigned __int8 startDay = g_HolySys.GetStartDay();
  const unsigned __int8 startMonth = g_HolySys.GetStartMonth();
  const unsigned __int16 startYear = g_HolySys.GetStartYear();
  msg.byHolyMentalCount =
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
  _propose_vote_result_zocl msg{};
  msg.byRetCode = byRetCode;

  unsigned __int8 type[2] = {26, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
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
    newGold = MAX_GOLD;
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

void CPlayer::SubPoint(int dwSub)
{
  if (!dwSub)
  {
    return;
  }

  long double newPvpCash = m_kPvpOrderView.GetPvpCash() - static_cast<double>(dwSub);
  const double subAsDouble = static_cast<double>(dwSub);
  if (subAsDouble > m_kPvpOrderView.GetPvpCash() && m_kPvpOrderView.GetPvpCash() > 0.0)
  {
    newPvpCash = 0.0;
  }

  if (newPvpCash != m_kPvpOrderView.GetPvpCash())
  {
    m_kPvpOrderView.SetPvpCash(newPvpCash);
    m_kPvpOrderView.UpdatePvpCash(newPvpCash);
  }
}

void CPlayer::SubActPoint(unsigned __int8 byCode, unsigned int dwSub)
{
  unsigned int leftPoint = m_pUserDB->GetActPoint(byCode) - dwSub;
  if (dwSub > m_pUserDB->GetActPoint(byCode))
  {
    leftPoint = 0;
  }

  if (leftPoint != m_pUserDB->GetActPoint(byCode))
  {
    m_pUserDB->Update_User_Action_Point(byCode, leftPoint);
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
  _guild_self_leave_result_zocl msg{};
  msg.byRetCode = byRetCode;

  unsigned __int8 type[2] = {27, 17};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
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
    if (!g_Main.PushDQSData(-1, &m_id, 17, reinterpret_cast<char *>(&qry), size))
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
  _notify_update_player_time_limit_Info_zocl msg{};

  const unsigned int fatigue = m_pUserDB->m_AvatorData.dbTimeLimitInfo.dwFatigue;
  const unsigned __int16 tlStatus = m_pUserDB->m_AvatorData.dbTimeLimitInfo.byTLStatus;
  msg.dwFatigue = fatigue;
  msg.wStatus = tlStatus;

  const unsigned int playFDegree = g_Main.m_pTimeLimitMgr->GetPlayFDegree();
  const unsigned int playedSeconds = playFDegree / 1000 * fatigue;

  const unsigned __int16 endPlayTime = g_Main.m_pTimeLimitMgr->GetEndPlayTime();
  unsigned int remainSeconds = 0;
  if (playedSeconds < 60u * endPlayTime)
  {
    const unsigned __int16 endPlayTime2 = g_Main.m_pTimeLimitMgr->GetEndPlayTime();
    remainSeconds = 60u * endPlayTime2 - playedSeconds;
  }

  msg.dwRemainTime = remainSeconds;
  msg.dwAccumPlayTime = playedSeconds;

  unsigned __int8 pbyType[2]{};
  pbyType[0] = 13;
  pbyType[1] = static_cast<unsigned __int8>(-112);
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_RaceTopInform(char bTop)
{
  _race_top_inform_zocl msg{};
  msg.bTop = bTop != 0;

  unsigned __int8 type[2] = {3, 47};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
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
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::UpdatePost(unsigned int dwIndex)
{
  if (dwIndex >= 50)
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

void CPlayer::pc_RequestPatriarchPunishment(unsigned __int8 byType, char *pwszName, char *pwszCont)
{
  unsigned __int8 result = 0;
  int raceCode = 0;
  if (!g_Main.IsReleaseServiceMode()
    || (raceCode = m_Param.GetRaceCode(),
        CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(raceCode, 0) == m_dwObjSerial))
  {
    if (g_VoteSys[raceCode].m_bActive)
    {
      result = 2;
    }
    else if (byType == 2)
    {
      result = 9;
    }
    else if (!IsSQLValidString(pwszName))
    {
      const char *charName = m_Param.GetCharNameA();
      g_Main.m_logSystemError.Write(
        "CPlayer::pc_RequestPatriarchPunishment() : %u(%s) ::IsSQLValidString(pwszName(%s)) Invalid!",
        m_dwObjSerial,
        charName,
        pwszName);
      result = 7;
    }
  }
  else
  {
    result = 1;
  }

  if (result)
  {
    SendMsg_ProposeVoteResult(result);
    return;
  }

  _qry_case_select_charserial pQryData{};
  pQryData.byRace = static_cast<unsigned __int8>(m_Param.GetRaceCode());
  pQryData.byType = byType;
  std::strcpy(pQryData.wszCharName, pwszName);
  std::strcpy(pQryData.wszContent, pwszCont);
  pQryData.dwAvatorSerial = static_cast<unsigned int>(-1);
  const int nSize = static_cast<int>(pQryData.size());
  g_Main.PushDQSData(-1, nullptr, 128, reinterpret_cast<char *>(&pQryData), nSize);
}

void CPlayer::SendMsg_QuestGiveUpResult(char byReturnSlot)
{
  _quest_give_up_result_zocl szMsg{};
  szMsg.byQuestDBSlot = byReturnSlot;
  unsigned __int8 pbyType[2]{24, 50};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&szMsg), sizeof(szMsg));
}

void CPlayer::SendMsg_SelectWaitedQuest(
  char byEventType,
  unsigned int dwEventIndex,
  unsigned __int8 byEventNodeIndex)
{
  _select_waited_quest_command_zocl msg{};

  msg.byEventType = byEventType;
  msg.dwEventIndex = dwEventIndex;
  msg.byEventNodeIndex = byEventNodeIndex;

  unsigned __int8 pbyType[2]{24, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    6u);
}

void CPlayer::SendMsg_GuildManageResult(char byRetCode)
{
  _guild_manage_result_zocl szMsg{};
  szMsg.byRetCode = byRetCode;
  unsigned __int8 pbyType[2]{27, 124};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&szMsg), sizeof(szMsg));
}

void CPlayer::pc_QuestGiveupRequest(unsigned __int8 byQuestDBSlot)
{
  unsigned __int8 result = byQuestDBSlot;
  if (m_QuestMgr.CanGiveupQuest(byQuestDBSlot))
  {
    m_QuestMgr.DeleteQuestData(byQuestDBSlot);
    m_pUserDB->Update_QuestDelete(byQuestDBSlot);
  }
  else
  {
    result = static_cast<unsigned __int8>(-1);
  }
  SendMsg_QuestGiveUpResult(result);
}

char CPlayer::Emb_CreateQuestEvent(__int32 HappenType, char *pszEventCode)
{
  const unsigned __int8 raceCode = static_cast<unsigned __int8>(m_Param.GetRaceCode());
  _happen_event_cont *src =
    m_QuestMgr.CheckQuestHappenEvent(static_cast<QUEST_HAPPEN>(HappenType), pszEventCode, raceCode);
  if (!src)
  {
    return 0;
  }

  _happen_event_cont happenEvent{};
  happenEvent.init();
  std::memcpy(&happenEvent, src, sizeof(happenEvent));
  if (happenEvent.m_pEvent->m_bSelectQuestManual)
  {
    SendMsg_SelectWaitedQuest(HappenType, happenEvent.m_nIndexInType, happenEvent.m_nRaceCode);
    return 1;
  }

  if (Emb_StartQuest(255, &happenEvent))
  {
    return 1;
  }

  for (int j = 0; j < 3; ++j)
  {
    if (!m_QuestMgr.m_pTempHappenEvent[j].isset())
    {
      std::memcpy(&m_QuestMgr.m_pTempHappenEvent[j], &happenEvent, sizeof(m_QuestMgr.m_pTempHappenEvent[j]));
      return 0;
    }
  }
  return 0;
}

void CPlayer::pc_GuildManageRequest(
  unsigned __int8 byType,
  unsigned int dwDst,
  unsigned int dwObj1,
  unsigned int dwObj2,
  unsigned int dwObj3)
{
  unsigned __int8 result = 0;
  CGuild *guild = m_Param.m_pGuild;
  if (!guild)
  {
    result = static_cast<unsigned __int8>(-54);
  }
  // Yorozuya fix implementation (non-IDA): guard the guild pointer before checking master
  // ownership so forced guild-manage packets cannot dereference a null guild and crash.
  else if (!m_byUserDgr && guild->m_MasterData.dwSerial != m_Param.GetCharSerial())
  {
    result = static_cast<unsigned __int8>(-53);
  }

  if (!result)
  {
    switch (byType)
    {
      case 0:
        result = guild->ManageProposeGuildBattle(dwDst, dwObj1, dwObj2, dwObj3);
        break;
      case 1:
        result = guild->ManageExpulseMember(dwDst);
        break;
      case 2:
        result = guild->ManagePopGuildMoney(dwDst, dwObj1, dwObj2);
        break;
      case 3:
        result = guild->ManageBuyGuildEmblem(dwDst, dwObj1, dwObj2);
        break;
      case 4:
        result = guild->ManageGuildCommittee(dwDst, dwObj1 != 0);
        break;
      case 5:
        result = guild->ManageAcceptORRefuseGuildBattle(dwDst != 0);
        break;
      default:
        break;
    }
  }
  SendMsg_GuildManageResult(result);
}

void CPlayer::SendMsg_AlterPvPCash(char nIOCode)
{
  _alter_pvp_cash_zocl szMsg{};
  szMsg.dNewPoint = m_kPvpOrderView.GetPvpCash();
  szMsg.byIOCode = nIOCode;
  unsigned __int8 pbyType[2]{};
  memset(pbyType, 11, 2);
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&szMsg),
    sizeof(szMsg));
}

void CPlayer::SendMsg_GuildForceLeaveBoradori()
{
  _guild_force_leave_boradori szMsg{};
  szMsg.sDum = 0;
  unsigned __int8 pbyType[2]{};
  pbyType[0] = 27;
  pbyType[1] = 121;
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&szMsg), sizeof(szMsg));
}




















