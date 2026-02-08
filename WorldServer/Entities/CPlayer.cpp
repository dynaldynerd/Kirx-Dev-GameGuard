#include "pch.h"

#include "CPlayer.h"
#include "CRecordData.h"
#include "CAnimus.h"
#include "CMonster.h"
#include "CMapData.h"
#include "CLevel.h"
#include "CTransportShip.h"
#include "CUserDB.h"
#include "CItemBox.h"
#include "CMoneySupplyMgr.h"
#include "COreAmountMgr.h"
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
#include "CLogFile.h"
#include "CTSingleton.h"
#include "LendItemMng.h"
#include "ITEM_EFFECT.h"
#include "ItemUpgrade_fld.h"
#include "TimeLimitJadeMng.h"
#include "ResourceItem_fld.h"
#include "UIDGenerator.h"
#include "WorldServerUtil.h"
#include "CExchangeEvent.h"
#include "CPcBangFavor.h"
#include "CUnmannedTraderController.h"
#include "CUnmannedTraderRegistItemInfo.h"
#include "CMoveMapLimitManager.h"
#include "CBillingManager.h"
#include "CGuildRoomSystem.h"
#include "CCouponMgr.h"
#include "EQUIP_MASTERY_LIM.h"
#include "CDarkHoleChannel.h"
#include "CGuardTower.h"
#include "CTrap.h"
#include "CSetItemEffect.h"
#include "exit_alter_param.h"
#include "alter_cont_effect_time_zocl.h"
#include "notify_not_use_premium_cashitem_zocl.h"
#include "pt_inform_punishment_zocl.h"
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
