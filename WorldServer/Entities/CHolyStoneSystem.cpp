#include "pch.h"

#include "CHolyStoneSystem.h"

#include <atltime.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <mmsystem.h>

#include "CHolyKeeper.h"
#include "CAnimus.h"
#include "CHolyStone.h"
#include "CHolyStoneSystemDataMgr.h"
#include "COreAmountMgr.h"
#include "CMoveMapLimitManager.h"
#include "CGoldenBoxItemMgr.h"
#include "CGuild.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CRaceBossWinRate.h"
#include "CRaceBuffManager.h"
#include "class_fld.h"
#include "create_holy_master_zocl.h"
#include "holy_quest_report_wrac.h"
#include "holy_keeper_attackable_inform_zocl.h"
#include "CNetworkEX.h"
#include "DqsDbStructs.h"
#include "Packet/ZoneWebPacket.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "trand_tbl.h"

namespace
{
const _trand_tbl sTransTBL[6] = {
  {1, 1, 2},
  {2, 2, 3},
  {3, 3, 4},
  {4, 5, 6},
  {5, 5, 6},
  {6, 6, 0},
};

const char *kRaceCodeName[3] = {"Bellato", "Cora", "Accretia"};

const _trand_tbl *_GetTransTBL(int nState)
{
  for (int index = 0; index < static_cast<int>(std::size(sTransTBL)); ++index)
  {
    if (sTransTBL[index].nState == nState)
    {
      return &sTransTBL[index];
    }
  }

  return nullptr;
}
} // namespace

__holy_keeper_data::__holy_keeper_data()
  : pCreateMap(nullptr),
    pRec(nullptr)
{
}

__holy_stone_data::__holy_stone_data()
  : pCreateMap(nullptr),
    pRec(nullptr),
    nRace(0)
{
}

_QUEST_CASH::_QUEST_CASH()
{
  init();
}

_QUEST_CASH_OTHER::_QUEST_CASH_OTHER()
{
  init();
}

CHolyStoneSystem::CHolyStoneSystem()
  : m_pkDestroyer(nullptr),
    m_dwNextStartTime(0),
    m_bConsumable(false),
    m_pMentalPass(true)
{
  m_tmrCumPlayer.BeginTimer(0xEA60u);
  m_tmrHSKSystem.BeginTimer(0x3E8u);
}

CHolyStoneSystem::~CHolyStoneSystem() = default;

bool CHolyStoneSystem::InitHolySystem()
{
  if (!CHolyStoneSystemDataMgr::LoadIni(this))
  {
    MyMessageBox("Error", "CHolyStoneSystem::InitHolySystem() : CHolyStoneSystemDataMgr::LoadIni(*this) Fail!");
    return false;
  }

  if (!CHolyStoneSystemDataMgr::LoadSceduleData(&this->m_ScheculeData))
  {
    MyMessageBox(
      "Error",
      "CHolyStoneSystem::InitHolySystem() : CHolyStoneSystemDataMgr::LoadSceduleData( m_ScheculeData ) Fail!");
    return false;
  }

  CHolyScheduleData::__HolyScheduleNode *firstSchedule = this->m_ScheculeData.GetIndex(1);
  if (!firstSchedule)
  {
    MyMessageBox("Error", "CHolyStoneSystem::InitHolySystem() : pFirstSchedule = m_ScheculeData.GetIndex(1) NULL!");
    return false;
  }

  for (int j = 0; j < this->m_nHolyStoneNum; ++j)
  {
    __holy_stone_data *stoneData = &this->m_HolyStoneData[j];
    _stone_create_setdata data;
    data.m_pMap = stoneData->pCreateMap;
    data.m_nLayerIndex = 0;
    data.m_pRecordSet = stoneData->pRec;
    memcpy_0(data.m_fStartPos, stoneData->CreateDummy.m_fCenterPos, sizeof(data.m_fStartPos));
    data.pDumPosition = &stoneData->CreateDummy;
    data.byMasterRace = static_cast<unsigned char>(stoneData->nRace);
    g_Stone[j].Create(&data);
  }

  COreAmountMgr *oreMgr = COreAmountMgr::Instance();
  if (!oreMgr->LoadINI())
  {
    MyMessageBox("COreAmountMgr Data Init", "COreAmountMgr::Instance()->LoadINI() Fail!");
    ServerProgramExit("COreAmountMgr Data Init", 0);
  }

  bool scheduleInit =
    GetPrivateProfileIntA("Schedule", "ScheduleInit", 1, ".\\Initialize\\WorldSystem.ini") != 0;
  if (scheduleInit)
  {
    WritePrivateProfileStringA("Schedule", "ScheduleInit", "0", ".\\Initialize\\WorldSystem.ini");
    this->m_SaveData.DefaultInit(firstSchedule);
    oreMgr = COreAmountMgr::Instance();
    oreMgr->InitRemainOreAmount(0xFFFFFFFF, 0xFFFFFFFF);
    oreMgr->InitTransferOre(0, 0);
  }

  if (!scheduleInit
      && (!CHolyStoneSystemDataMgr::LoadStateData(&this->m_SaveData) || !this->ContinueStartSystem()))
  {
    this->m_SaveData.DefaultInit(firstSchedule);
    oreMgr = COreAmountMgr::Instance();
    oreMgr->InitRemainOreAmount(0xFFFFFFFF, 0xFFFFFFFF);
    oreMgr->InitTransferOre(0, 0);
    scheduleInit = true;
  }

  if (scheduleInit)
  {
    CHolyStoneSystemDataMgr::SaveStateData(&this->m_SaveData);
  }

  this->m_bScheduleCodePre = 0;
  this->InitQuestCash_Other();
  return true;
}

void CHolyStoneSystem::OnLoop()
{
  if (m_tmrHSKSystem.CountingTimer())
  {
    HSKRespawnSystem();
  }

  const int sceneCode = GetSceneCode();
  if (sceneCode == 1)
  {
    UpdateNotifyHolyStoneHPToRaceBoss();
  }
  else if (sceneCode == 2)
  {
    CheckDestroyerIsArriveMine();
  }
}

void CHolyStoneSystem::HSKRespawnSystem()
{
  m_SaveData.m_dwPassTimeInScene += m_tmrHSKSystem.GetTerm();

  const _trand_tbl *transition = _GetTransTBL(m_SaveData.m_nSceneCode);
  if (transition && GetLoopTime() >= m_dwCheckTime[transition->nCheckTimeIndex])
  {
    const unsigned __int8 numOfTime = GetNumOfTime();
    if (transition->nNextState == 4)
    {
      SetScene(numOfTime, transition->nNextState, 0, 4);
    }
    else
    {
      SetScene(numOfTime, transition->nNextState, 0, 0);
    }
  }

  if (m_tmrCumPlayer.CountingTimer())
  {
    m_SaveData.m_dwCumPlayerNum += CPlayer::s_nLiveNum;
    ++m_SaveData.m_dwCumCount;
  }

  for (int stoneIndex = 0; stoneIndex < MAX_STONE; ++stoneIndex)
  {
    if (g_Stone[stoneIndex].m_bOper)
    {
      m_SaveData.m_nStoneHP_Buffer[stoneIndex] = static_cast<int>(g_Stone[stoneIndex].GetHP());
    }
  }

  COreAmountMgr *oreMgr = COreAmountMgr::Instance();
  m_SaveData.m_dwOreRemainAmount = oreMgr->GetRemainOre();
  oreMgr = COreAmountMgr::Instance();
  m_SaveData.m_dwOreTotalAmount = oreMgr->GetTotalOre();
  oreMgr = COreAmountMgr::Instance();
  m_SaveData.m_byOreTransferCount = oreMgr->GetOreTransferCount();
  oreMgr = COreAmountMgr::Instance();
  m_SaveData.m_dwOreTransferAmount = oreMgr->GetOreTransferAmount();

  CHolyStoneSystemDataMgr::SaveStateData(&m_SaveData);
}

void CHolyStoneSystem::UpdateNotifyHolyStoneHPToRaceBoss()
{
  for (int stoneIndex = 0; stoneIndex < MAX_STONE; ++stoneIndex)
  {
    if (g_Stone[stoneIndex].m_bLive && g_Stone[stoneIndex].IsChangedHP(1u))
    {
      SendHolyStoneHPToRaceBoss();
      return;
    }
  }
}

void CHolyStoneSystem::CheckDestroyerIsArriveMine()
{
  if (GetHolyMasterRace() == -1 || GetSceneCode() != 2 || m_SaveData.m_eDestroyerState != 2)
  {
    return;
  }

  const bool hasMatchedDestroyer =
    m_pkDestroyer && GetDestroyerSerial() == m_pkDestroyer->m_dwObjSerial;
  if (!hasMatchedDestroyer)
  {
    m_pkDestroyer = GetPtrPlayerFromSerial(g_Player, MAX_PLAYER, GetDestroyerSerial());
  }

  if (!m_pkDestroyer || !m_pkDestroyer->m_bLive || !m_pkDestroyer->m_bOper)
  {
    return;
  }

  if (m_pkDestroyer->m_bCorpse)
  {
    m_SaveData.m_eDestroyerState = 0;
    m_SaveData.m_dwDestroyerSerial = static_cast<unsigned int>(-1);
    ReleaseLastAttBuff();
    m_SaveData.m_dwDestroyerGuildSerial = static_cast<unsigned int>(-1);
    CHolyStoneSystemDataMgr::SaveStateData(&m_SaveData);
    SendIsArriveDestroyer(0);
    return;
  }

  CExtDummy *dummy = &m_HolyKeeperData.pCreateMap->m_Dummy;
  if (!dummy->IsInBBox(m_HolyKeeperData.ActiveDummy.m_wLineIndex, m_pkDestroyer->m_fCurPos))
  {
    return;
  }

  const unsigned __int8 numOfTime = GetNumOfTime();
  CHolyScheduleData::__HolyScheduleNode *scheduleNode = m_ScheculeData.GetIndex(numOfTime);
  m_SaveData.m_dwTerm[1] += scheduleNode->m_nSceneTime[6];
  m_dwCheckTime[5] = m_SaveData.m_dwTerm[1] + m_SaveData.m_dwTerm[0] + m_dwCheckTime[3];
  m_SaveData.m_eDestroyerState = 1;
  CHolyStoneSystemDataMgr::SaveStateData(&m_SaveData);
  SendIsArriveDestroyer(1u);
  m_logQuestDestroy.Write(">> Arrive!");

  const unsigned int loopTime = GetLoopTime();
  const int remainControlSec = static_cast<int>(
    (scheduleNode->m_nSceneTime[2] + m_SaveData.m_dwTerm[1] + m_SaveData.m_dwTerm[0] + m_dwCheckTime[2] - loopTime)
    / 1000);
  SendSMS_MineTimeExtend(remainControlSec);
}

void CHolyStoneSystem::SendSMS_MineTimeExtend(int nControlSec)
{
  const int currentHour = GetCurrentHour();
  int targetHour = (nControlSec / 60 / 60 + currentHour) % 24;

  const int currentMin = GetCurrentMin();
  const int targetMin = (nControlSec / 60 + currentMin) % 60;
  if (targetMin < currentMin)
  {
    targetHour = (targetHour + 1) % 24;
  }

  _holy_minetime_extend_inform_zowb msg{};

  msg.nWorldCode = g_Main.m_byWorldCode;
  msg.byChaosHour = static_cast<char>(targetHour);
  msg.byChaosMin = static_cast<char>(targetMin);
  msg.byNumOfTime = GetNumOfTime();

  unsigned __int8 type[2]{51, 2};
  if (g_Main.m_bConnectedWebAgentServer)
  {
    g_Network.m_pProcess[2]->LoadSendMsg(g_Main.m_byWebAgentServerNetInx, type, reinterpret_cast<char *>(&msg), 7u);
  }
}

bool CHolyStoneSystem::ContinueStartSystem()
{
  int storedStoneHp[3]{};
  memcpy_0(storedStoneHp, m_SaveData.m_nStoneHP_Buffer, sizeof(storedStoneHp));

  if (!SetScene(m_SaveData.m_byNumOfTime, m_SaveData.m_nSceneCode, m_SaveData.m_dwPassTimeInScene, 2))
  {
    return false;
  }

  if (m_SaveData.m_nSceneCode == 1)
  {
    for (int j = 0; j < 3; ++j)
    {
      g_Stone[j].SetHP(storedStoneHp[j], false);
    }
  }

  COreAmountMgr *oreMgr = COreAmountMgr::Instance();
  oreMgr->InitRemainOreAmount(m_SaveData.m_dwOreRemainAmount, m_SaveData.m_dwOreTotalAmount);
  oreMgr = COreAmountMgr::Instance();
  m_SaveData.m_dwOreRemainAmount = oreMgr->GetRemainOre();
  oreMgr = COreAmountMgr::Instance();
  m_SaveData.m_dwOreTotalAmount = oreMgr->GetTotalOre();

  oreMgr = COreAmountMgr::Instance();
  oreMgr->InitTransferOre(m_SaveData.m_dwOreTransferAmount, m_SaveData.m_byOreTransferCount);
  oreMgr = COreAmountMgr::Instance();
  m_SaveData.m_dwOreTransferAmount = oreMgr->GetOreTransferAmount();
  oreMgr = COreAmountMgr::Instance();
  m_SaveData.m_byOreTransferCount = oreMgr->GetOreTransferCount();

  m_logQuest.Write(
    "Continue Start >> scene:%d, pass:%d, b_stHP:%d, c_stHP:%d, a_stHP:%d, master:%d, total_ore:%d, remain_ore:%d",
    m_SaveData.m_nSceneCode,
    m_SaveData.m_dwPassTimeInScene,
    m_SaveData.m_nStoneHP_Buffer[0],
    m_SaveData.m_nStoneHP_Buffer[1],
    m_SaveData.m_nStoneHP_Buffer[2],
    m_SaveData.m_nHolyMasterRace,
    m_SaveData.m_dwOreTotalAmount,
    m_SaveData.m_dwOreRemainAmount);
  return true;
}

void CHolyStoneSystem::InitQuestCash_Other()
{
  for (size_t j = 0; j < std::size(this->m_cashQuestOther); ++j)
  {
    this->m_cashQuestOther[j].init();
  }
}

void CHolyStoneSystem::InitQuestCash()
{
  for (size_t j = 0; j < std::size(this->m_cashQuest); ++j)
  {
    this->m_cashQuest[j].init();
  }
}

void CHolyStoneSystem::AlterSchedule(unsigned __int8 byScheduleCode, unsigned __int8 byNumOfTime)
{
  if (byScheduleCode)
  {
    if (byScheduleCode == 1)
    {
      m_bScheduleCodePre = 0;
      const unsigned int prevHolyMasterRace = static_cast<unsigned int>(m_SaveData.m_nHolyMasterRace);
      int prevStoneHp[3]{};
      memcpy_0(prevStoneHp, m_SaveData.m_nStoneHP_Buffer, sizeof(prevStoneHp));

      SetScene(byNumOfTime, 1, 0, 0);

      if (prevHolyMasterRace <= 2)
      {
        unsigned int adjustedHp = static_cast<unsigned int>(
          static_cast<int>(static_cast<float>(m_SaveData.m_nStartStoneHP) * 0.2f
                           + static_cast<float>(prevStoneHp[prevHolyMasterRace])));
        if (adjustedHp > static_cast<unsigned int>(m_SaveData.m_nStartStoneHP))
        {
          adjustedHp = static_cast<unsigned int>(m_SaveData.m_nStartStoneHP);
        }
        g_Stone[prevHolyMasterRace].SetHP(adjustedHp, false);
      }
      m_logQuest.Write("Holy Quset Start >> ");
    }
  }
  else
  {
    m_bScheduleCodePre = 1;
    InitQuestCash_Other();
    m_dwNextStartTime = timeGetTime() + 3600000;
    SendMsg_NoticeNextQuest(static_cast<unsigned int>(-1), 1);
    m_logQuest.Write("Notice Next Quest >> ");
  }
}

bool CHolyStoneSystem::SetScene(
  unsigned __int8 byNumOfTime,
  int nSceneCode,
  unsigned int nPassTime,
  int nChangeReason)
{
  CHolyScheduleData::__HolyScheduleNode *scheduleNode = m_ScheculeData.GetIndex(byNumOfTime);
  if (!scheduleNode)
  {
    return false;
  }

  m_SaveData.m_byNumOfTime = byNumOfTime;
  m_SaveData.m_nSceneCode = nSceneCode;

  switch (nSceneCode)
  {
    case 1:
    {
      CRaceBuffManager::Instance()->RequestHolyQuestRaceBuff(1);
      m_dwCheckTime[0] = GetLoopTime() - nPassTime;
      m_dwCheckTime[1] = scheduleNode->m_nSceneTime[1] + m_dwCheckTime[0];
      SetTermTimeDefault(byNumOfTime);
      m_dwCheckTime[2] = scheduleNode->m_nSceneTime[0] + m_dwCheckTime[1];
      m_dwCheckTime[3] = scheduleNode->m_nSceneTime[2] + m_dwCheckTime[2];
      m_dwCheckTime[5] = m_SaveData.m_dwTerm[1] + m_SaveData.m_dwTerm[0] + m_dwCheckTime[3];
      m_dwCheckTime[6] = m_dwCheckTime[0] + m_ScheculeData.GetTotalSceduleTerm(byNumOfTime);
      On_HS_SCENE_BATTLE_TIME();
      m_logQuest.Write(">> HS_SCENE_BATTLE_TIME");
      break;
    }
    case 2:
    {
      m_dwCheckTime[1] = GetLoopTime() - nPassTime;
      m_dwCheckTime[2] = scheduleNode->m_nSceneTime[0] + m_dwCheckTime[1];
      if (GetHolyMasterRace() == -1)
      {
        m_dwCheckTime[3] = m_dwCheckTime[2];
        m_dwCheckTime[5] = m_dwCheckTime[3];
        CheckKeeperPlusTime();
        CRaceBuffManager::Instance()->RequestHolyQuestRaceBuff(2);
      }
      else
      {
        m_dwCheckTime[3] = scheduleNode->m_nSceneTime[2] + m_dwCheckTime[2];
        m_dwCheckTime[5] = m_SaveData.m_dwTerm[1] + m_SaveData.m_dwTerm[0] + m_dwCheckTime[3];
      }
      const unsigned int startBattleTickTime = GetStartBattleTickTime();
      m_dwCheckTime[6] = m_ScheculeData.GetTotalSceduleTerm(byNumOfTime) + startBattleTickTime;
      On_HS_SCENE_BATTLE_END_WAIT_TIME();
      m_logQuest.Write(">> HS_SCENE_BATTLE_END_WAIT_TIME");
      break;
    }
    case 3:
    {
      m_dwCheckTime[2] = GetLoopTime() - nPassTime;
      m_dwCheckTime[3] = scheduleNode->m_nSceneTime[2] + m_dwCheckTime[2];
      if (GetHolyMasterRace() == -1)
      {
        m_dwCheckTime[3] = m_dwCheckTime[2];
        m_dwCheckTime[5] = m_dwCheckTime[3];
      }
      else
      {
        m_dwCheckTime[5] = m_SaveData.m_dwTerm[1] + m_SaveData.m_dwTerm[0] + m_dwCheckTime[3];
      }
      const unsigned int startBattleTickTime = GetStartBattleTickTime();
      m_dwCheckTime[6] = m_ScheculeData.GetTotalSceduleTerm(byNumOfTime) + startBattleTickTime;
      On_HS_SCENE_KEEPER_ATTACKABLE_TIME();
      m_logQuest.Write(">> HS_SCENE_KEEPER_ATTACKABLE_TIME");
      break;
    }
    case 4:
    {
      if (g_Main.IsReleaseServiceMode())
      {
        if (nChangeReason == 4)
        {
          SendMsg_NotifyHolyKeeperAttackTimeBeKeepKeeper(1);
          CRaceBuffManager::Instance()->RequestHolyQuestRaceBuff(3);
        }
      }
      else if (nChangeReason == 4 || nChangeReason == 1)
      {
        SendMsg_NotifyHolyKeeperAttackTimeBeKeepKeeper(1);
        CRaceBuffManager::Instance()->RequestHolyQuestRaceBuff(3);
      }

      m_dwCheckTime[3] = GetLoopTime() - nPassTime;
      if (GetHolyMasterRace() == -1)
      {
        m_dwCheckTime[5] = m_dwCheckTime[3];
      }
      else
      {
        m_dwCheckTime[5] = m_SaveData.m_dwTerm[1] + m_SaveData.m_dwTerm[0] + m_dwCheckTime[3];
      }
      const unsigned int startBattleTickTime = GetStartBattleTickTime();
      m_dwCheckTime[6] = m_ScheculeData.GetTotalSceduleTerm(byNumOfTime) + startBattleTickTime;
      On_HS_SCENE_KEEPER_DEATTACKABLE_TIME();
      m_logQuest.Write(">> HS_SCENE_KEEPER_DEATTACKABLE_TIME");
      break;
    }
    case 5:
    {
      if (nChangeReason == 3)
      {
        SendMsg_NotifyHolyKeeperAttackTimeBeKeepKeeper(0);
      }
      m_dwCheckTime[4] = GetLoopTime() - nPassTime;
      const unsigned int startBattleTickTime = GetStartBattleTickTime();
      m_dwCheckTime[6] = m_ScheculeData.GetTotalSceduleTerm(byNumOfTime) + startBattleTickTime;
      m_dwCheckTime[5] = m_dwCheckTime[6] - scheduleNode->m_nSceneTime[4];
      On_HS_SCENE_KEEPER_DIE_TIME();
      m_logQuest.Write(">> HS_SCENE_KEEPER_DIE_TIME");
      break;
    }
    case 6:
    {
      m_dwCheckTime[5] = GetLoopTime() - nPassTime;
      const unsigned int startBattleTickTime = GetStartBattleTickTime();
      m_dwCheckTime[6] = m_ScheculeData.GetTotalSceduleTerm(byNumOfTime) + startBattleTickTime;
      On_HS_SCENE_KEEPER_CHAOS_TIME();
      m_logQuest.Write(">> HS_SCENE_KEEPER_CHAOS_TIME");
      break;
    }
    default:
      break;
  }

  m_SaveData.m_dwPassTimeInScene = nPassTime;
  return true;
}

void CHolyStoneSystem::SetTermTimeDefault(unsigned __int8 byNumOfTime)
{
  CHolyScheduleData::__HolyScheduleNode *scheduleNode = m_ScheculeData.GetIndex(byNumOfTime);
  if (scheduleNode)
  {
    m_SaveData.m_dwTerm[0] =
      scheduleNode->m_nSceneTime[5]
      - (scheduleNode->m_nSceneTime[2] + scheduleNode->m_nSceneTime[0] + scheduleNode->m_nSceneTime[1]);
    m_SaveData.m_dwTerm[1] = scheduleNode->m_nSceneTime[3];
  }
}

bool CHolyStoneSystem::AuthMiningTicket(unsigned int dwKey)
{
  const unsigned __int8 numOfTime = GetNumOfTime();
  const unsigned __int8 startHour = GetStartHour();
  const unsigned __int8 startDay = GetStartDay();
  const unsigned __int8 startMonth = GetStartMonth();
  const unsigned __int16 startYear = GetStartYear();

  MiningTicket::_AuthKeyTicket authKey{};
  authKey.Set(startYear, startMonth, startDay, startHour, numOfTime);
  return authKey.___u0.uiData == dwKey;
}

char CHolyStoneSystem::ct_KeeperStart(int nKeeperState, int nRace, unsigned int nPassTime)
{
  if (m_SaveData.m_nSceneCode < 2 || m_SaveData.m_nSceneCode >= 6)
  {
    return 0;
  }

  m_SaveData.m_nHolyMasterRace = nRace;
  SetScene(m_SaveData.m_byNumOfTime, nKeeperState, nPassTime, 1);
  return 1;
}

char CHolyStoneSystem::ct_State(CPlayer *pOne)
{
  unsigned int remainMinute = 0;
  bool showStage1 = false;
  bool showStage2 = false;
  bool showStage3 = false;
  bool showStage4 = false;
  bool showStage5 = false;
  bool showStage6 = false;

  const int numOfTime = GetNumOfTime();
  const int startMin = GetStartMin();
  const int startHour = GetStartHour();
  const int startDay = GetStartDay();
  const unsigned int startMonth = GetStartMonth();
  const unsigned int startYear = GetStartYear();

  char chat[2000]{};
  sprintf(chat, "HolySchedule %u/%u/%u %u:%u (%u)", startYear, startMonth, startDay, startHour, startMin, numOfTime);
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, chat, 0xFFu, nullptr);

  const int sceneCode = GetSceneCode();
  switch (sceneCode)
  {
    case 0:
      sprintf(chat, "Scene 0");
      pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, chat, 0xFFu, nullptr);
      break;
    case 1:
      sprintf(chat, "Scene 1");
      pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, chat, 0xFFu, nullptr);
      showStage1 = true;
      showStage2 = true;
      showStage6 = true;
      break;
    case 2:
      sprintf(chat, "Scene 2");
      pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, chat, 0xFFu, nullptr);
      showStage1 = true;
      showStage3 = true;
      showStage4 = true;
      showStage5 = true;
      showStage6 = true;
      break;
    case 3:
      sprintf(chat, "Scene 3");
      pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, chat, 0xFFu, nullptr);
      showStage4 = true;
      showStage5 = true;
      showStage6 = true;
      break;
    case 4:
      sprintf(chat, "Scene 4");
      pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, chat, 0xFFu, nullptr);
      showStage5 = true;
      showStage6 = true;
      break;
    case 5:
      sprintf(chat, "Scene 5");
      pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, chat, 0xFFu, nullptr);
      showStage5 = true;
      showStage6 = true;
      break;
    case 6:
      sprintf(chat, "Scene 6");
      pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, chat, 0xFFu, nullptr);
      showStage6 = true;
      break;
    default:
      break;
  }

  if (showStage1)
  {
    remainMinute = static_cast<unsigned int>(g_24Time.Get24TimeFromTickTime(m_dwCheckTime[0]));
    sprintf(chat, "    1. %u:%u", remainMinute / 60, remainMinute % 60);
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, chat, 0xFFu, nullptr);
  }
  if (showStage2)
  {
    remainMinute = static_cast<unsigned int>(g_24Time.Get24TimeFromTickTime(m_dwCheckTime[1]));
    sprintf(chat, "    2. %u:%u", remainMinute / 60, remainMinute % 60);
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, chat, 0xFFu, nullptr);
  }
  if (showStage3)
  {
    remainMinute = static_cast<unsigned int>(g_24Time.Get24TimeFromTickTime(m_dwCheckTime[2]));
    sprintf(chat, "    3. %u:%u", remainMinute / 60, remainMinute % 60);
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, chat, 0xFFu, nullptr);
  }
  if (showStage4)
  {
    remainMinute = static_cast<unsigned int>(g_24Time.Get24TimeFromTickTime(m_dwCheckTime[3]));
    sprintf(chat, "    4. %u:%u", remainMinute / 60, remainMinute % 60);
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, chat, 0xFFu, nullptr);
  }
  if (showStage5)
  {
    remainMinute = static_cast<unsigned int>(g_24Time.Get24TimeFromTickTime(m_dwCheckTime[5]));
    sprintf(chat, "    5. %u:%u", remainMinute / 60, remainMinute % 60);
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, chat, 0xFFu, nullptr);
  }
  if (showStage6)
  {
    remainMinute = static_cast<unsigned int>(g_24Time.Get24TimeFromTickTime(m_dwCheckTime[6]));
    sprintf(chat, "    6. %u:%u", remainMinute / 60, remainMinute % 60);
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, chat, 0xFFu, nullptr);
  }

  return 1;
}

char CHolyStoneSystem::ct_StopBattle()
{
  if (m_SaveData.m_nSceneCode != 1)
  {
    return 0;
  }

  m_dwCheckTime[1] = GetLoopTime();
  return 1;
}

char *CHolyStoneSystem::GetHolyMentalString()
{
  return m_strHolyMental;
}

CMapData *CHolyStoneSystem::GetMapData()
{
  return m_HolyKeeperData.pCreateMap;
}

_portal_dummy *CHolyStoneSystem::GetPortalDummy(unsigned __int8 byRace)
{
  if (byRace < 3u)
  {
    return m_pPortalDummy[byRace];
  }

  return nullptr;
}

void CHolyStoneSystem::On_HS_SCENE_INIT()
{
  DestroyHolyStone();
  DestroyHolyKeeper();
}

void CHolyStoneSystem::CheckKeeperPlusTime()
{
  const unsigned __int8 numOfTime = GetNumOfTime();
  CHolyScheduleData::__HolyScheduleNode *scheduleNode = m_ScheculeData.GetIndex(numOfTime);
  const unsigned int passTime = GetLoopTime() - m_dwCheckTime[0];
  const int remain =
    scheduleNode->m_nSceneTime[5] - (scheduleNode->m_nSceneTime[2] + scheduleNode->m_nSceneTime[0] + passTime);
  m_SaveData.m_dwTerm[0] = remain;
}

unsigned __int8 CHolyStoneSystem::GetNumOfTime()
{
  return m_SaveData.m_byNumOfTime;
}

int CHolyStoneSystem::GetDestroyerState()
{
  return static_cast<int>(m_SaveData.m_eDestroyerState);
}

unsigned int CHolyStoneSystem::GetDestroyerSerial()
{
  return m_SaveData.m_dwDestroyerSerial;
}

int CHolyStoneSystem::GetHolyMasterRace()
{
  return m_SaveData.m_nHolyMasterRace;
}

int CHolyStoneSystem::GetSceneCode()
{
  return m_SaveData.m_nSceneCode;
}

int CHolyStoneSystem::GetControlLeftTime()
{
  if (GetHolyMasterRace() != -1)
  {
    return 0;
  }

  const int sceneCode = GetSceneCode();
  if ((sceneCode == 2 || sceneCode == 3 || sceneCode == 4) && m_dwCheckTime[5] > GetLoopTime())
  {
    return static_cast<int>(m_dwCheckTime[5] - GetLoopTime());
  }

  return 0;
}

bool CHolyStoneSystem::IsMinigeTicketCheck()
{
  const int sceneCode = GetSceneCode();
  return sceneCode == 3 || sceneCode == 4 || sceneCode == 5 || sceneCode == 6;
}

int CHolyStoneSystem::GetDestroyStoneRace()
{
  return m_SaveData.m_nDestroyStoneRace;
}

unsigned int CHolyStoneSystem::GetStartBattleTickTime()
{
  const int startMin = GetStartMin();
  const int startHour = GetStartHour();
  const int startDay = GetStartDay();
  const int startMonth = GetStartMonth();
  const int startYear = GetStartYear();

  ATL::CTime startTime(startYear, startMonth, startDay, startHour, startMin, 0, -1);
  ATL::CTime now = ATL::CTime::GetCurrentTime();
  ATL::CTimeSpan span = now - startTime;

  const int totalHours = (span.GetDays() * 24) + span.GetHours();
  const int totalMinutes = (totalHours * 60) + span.GetMinutes();
  return GetLoopTime() - 60000 * totalMinutes;
}

unsigned __int16 CHolyStoneSystem::GetStartYear()
{
  return m_SaveData.m_wStartYear;
}

unsigned __int8 CHolyStoneSystem::GetStartMonth()
{
  return m_SaveData.m_byStartMonth;
}

unsigned __int8 CHolyStoneSystem::GetStartDay()
{
  return m_SaveData.m_byStartDay;
}

unsigned __int8 CHolyStoneSystem::GetStartHour()
{
  return m_SaveData.m_byStartHour;
}

unsigned __int8 CHolyStoneSystem::GetStartMin()
{
  return m_SaveData.m_byStartMin;
}

unsigned int CHolyStoneSystem::GetDestroyerGuildSerial()
{
  return m_SaveData.m_dwDestroyerGuildSerial;
}

void CHolyStoneSystem::SetHolyMasterRace(int nMaster)
{
  m_SaveData.m_nHolyMasterRace = nMaster;
}

void CHolyStoneSystem::SetDestroyStoneRace(int nRace)
{
  m_SaveData.m_nDestroyStoneRace = nRace;
}

void CHolyStoneSystem::ReleaseLastAttBuff()
{
  const unsigned int destroyerGuildSerial = GetDestroyerGuildSerial();
  if (destroyerGuildSerial != static_cast<unsigned int>(-1))
  {
    for (int j = 0; j < MAX_PLAYER; ++j)
    {
      if (g_Player[j].m_bLive && g_Player[j].m_Param.m_pGuild)
      {
        const unsigned int guildSerial = g_Player[j].m_Param.GetGuildSerial();
        if (guildSerial == destroyerGuildSerial)
        {
          g_Player[j].SetLastAttBuff(false);
        }
      }
    }
  }
}

void CHolyStoneSystem::CreateHolyStone()
{
  float avgPlayers = 0.0f;
  if (m_SaveData.m_dwCumCount)
  {
    avgPlayers = static_cast<float>(static_cast<int>(m_SaveData.m_dwCumPlayerNum))
                 / static_cast<float>(static_cast<int>(m_SaveData.m_dwCumCount));
  }

  float hpRate = avgPlayers / 1500.0f;
  if (hpRate < 0.5f)
  {
    hpRate = 0.5f;
  }
  else if (hpRate > 2.0f)
  {
    hpRate = 2.0f;
  }

  char iniKey[132];
  sprintf_s(iniKey, "CrystalHPMultiple_%d", m_SaveData.m_byNumOfTime);

  char result[132]{};
  GetPrivateProfileStringA("Rule", iniKey, "0", result, 0x80u, "..\\WorldInfo\\WorldInfo.ini");
  if (!strcmp_0(result, "0"))
  {
    GetPrivateProfileStringA("Rule", "CrystalHPMultiple", "1", result, 0x80u, "..\\WorldInfo\\WorldInfo.ini");
  }

  float multiple = static_cast<float>(std::atof(result));
  if (multiple == 0.0f)
  {
    multiple = 1.0f;
    WritePrivateProfileStringA("Rule", "CrystalHPMultiple", "1.0", "..\\WorldInfo\\WorldInfo.ini");
  }

  for (int j = 0; j < m_nHolyStoneNum; ++j)
  {
    g_Stone[j].SetOper(true, hpRate * multiple);
  }

  m_SaveData.m_nStartStoneHP = g_Stone[0].GetHP();
  m_SaveData.m_dwCumPlayerNum = 0;
  m_SaveData.m_dwCumCount = 0;
}

void CHolyStoneSystem::DestroyHolyStone()
{
  for (int j = 0; j < m_nHolyStoneNum; ++j)
  {
    if (g_Stone[j].m_bLive)
    {
      g_Stone[j].SetOper(false, 1.0f);
    }
  }

  InitQuestCash();

  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    if (g_Player[j].m_bLive)
    {
      g_Player[j].RecvHSKQuest(0x64u, 1u, 0, 0, 0, 0xFFu);
    }
  }
}

void CHolyStoneSystem::CreateHolyKeeper(int nCreateType)
{
  _keeper_create_setdata data;
  data.m_pMap = m_HolyKeeperData.pCreateMap;
  data.m_nLayerIndex = 0;
  data.m_pRecordSet = m_HolyKeeperData.pRec;
  memcpy_0(data.m_fStartPos, m_HolyKeeperData.CreateDummy.m_fCenterPos, sizeof(data.m_fStartPos));
  data.pPosCreate = &m_HolyKeeperData.CreateDummy;
  data.pPosActive = &m_HolyKeeperData.ActiveDummy;
  data.pPosCenter = &m_HolyKeeperData.CenterDummy;
  data.nMasterRace = GetHolyMasterRace();

  if (g_Keeper->m_bLive)
  {
    g_Keeper->Destroy(1, nullptr);
  }

  g_Keeper->Create(&data, nCreateType);

  if (nCreateType)
  {
    g_Keeper->SetMaxHP(m_SaveData.m_nStartStoneHP);
    g_Keeper->SetHP(m_SaveData.m_nStartStoneHP);
  }
  else
  {
    const float keeperHpRate = m_fKeeperHPRate;
    const float firstKeeperHpRate = m_fFirstKeeperHPRate;
    const float maxHp = static_cast<float>(m_SaveData.m_nStartStoneHP) * keeperHpRate;
    g_Keeper->SetMaxHP(static_cast<int>(maxHp));

    if (m_SaveData.m_nHolyMasterRace > 2)
    {
      g_Keeper->SetHP(static_cast<int>(static_cast<float>(m_SaveData.m_nStartStoneHP) * keeperHpRate));
    }
    else
    {
      g_Keeper->SetHP(
        static_cast<int>(
          static_cast<float>(m_SaveData.m_nStoneHP_Buffer[m_SaveData.m_nHolyMasterRace]) * keeperHpRate)
        + static_cast<int>(static_cast<float>(m_SaveData.m_nStartStoneHP) * firstKeeperHpRate));
    }
  }
}

void CHolyStoneSystem::DestroyHolyKeeper()
{
  if (g_Keeper->m_bLive)
  {
    g_Keeper->Exit();
  }
}

bool CHolyStoneSystem::IsUseReturnItem(unsigned int dwObjSerial)
{
  return m_SaveData.m_eDestroyerState != 2 || dwObjSerial != m_SaveData.m_dwDestroyerSerial;
}

bool CHolyStoneSystem::GetGoldBoxConsumable()
{
  return m_bConsumable;
}

void CHolyStoneSystem::SetGoldBoxConsumable(bool bFlag)
{
  m_bConsumable = bFlag;
}

void CHolyStoneSystem::SetKeeperDestroyRace(unsigned __int8 byRace)
{
  m_byKeeperDestroyRace = byRace;
}

unsigned __int8 CHolyStoneSystem::GetKeeperDestroyRace()
{
  return m_byKeeperDestroyRace;
}

char CHolyStoneSystem::IsItemLootAuthority(CPlayer *pOne, unsigned __int8 byCreateCode)
{
  if (byCreateCode == 4)
  {
    if (g_Main.IsReleaseServiceMode())
    {
      if (pOne->m_Param.GetLevel() >= 25)
      {
        const int raceCode = pOne->m_Param.GetRaceCode();
        if (raceCode == GetHolyMasterRace() && !pOne->m_byUserDgr)
        {
          return 1;
        }
      }
    }
    else if (pOne->m_Param.GetLevel() >= 25)
    {
      if (pOne->m_Param.GetRaceCode() == GetHolyMasterRace())
      {
        return 1;
      }
    }
  }
  else if (byCreateCode == 6)
  {
    if (g_Main.IsReleaseServiceMode())
    {
      if (pOne->m_Param.GetLevel() >= 25)
      {
        const int raceCode = pOne->m_Param.GetRaceCode();
        if (raceCode == GetKeeperDestroyRace() && !pOne->m_byUserDgr)
        {
          return 1;
        }
      }
    }
    else if (pOne->m_Param.GetLevel() >= 25)
    {
      if (pOne->m_Param.GetRaceCode() == GetKeeperDestroyRace())
      {
        return 1;
      }
    }
  }

  return 0;
}

void CHolyStoneSystem::UnAllRegisterPerAutoMine()
{
  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    CPlayer *player = &g_Player[j];
    if (player->m_bLive)
    {
      AutominePersonal *autoMine = player->m_Param.m_pAPM;
      if (autoMine && autoMine->is_installed())
      {
        const unsigned int lineIndex = m_HolyKeeperData.ActiveDummy.m_wLineIndex;
        CExtDummy *dummy = &m_HolyKeeperData.pCreateMap->m_Dummy;
        if (dummy->IsInBBox(lineIndex, autoMine->m_fCurPos))
        {
          autoMine->unregist_from_map(0);
        }
      }
    }
  }
}

void CHolyStoneSystem::SendIsArriveDestroyer(char byArrive)
{
  if (!m_pkDestroyer)
  {
    return;
  }

  _notice_is_arrive_master msg{};

  msg.byArrive = byArrive;
  msg.byRaceCode = static_cast<char>(m_pkDestroyer->m_Param.GetRaceCode());
  const char *charName = m_pkDestroyer->m_Param.GetCharNameW();
  memcpy_0(msg.wszCharName, charName, 0x10u);
  msg.wszCharName[16] = 0;
  msg.dwObjSerial = m_SaveData.m_dwDestroyerSerial;

  unsigned __int8 type[2]{};
  type[0] = 25;
  type[1] = 17;

  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    if (g_Player[index].m_bLive)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        index,
        type,
        reinterpret_cast<char *>(&msg),
        static_cast<unsigned __int16>(sizeof(msg)));
    }
  }
}

void CHolyStoneSystem::SendMsg_to_webagent_about_last_attacker_for_keeper(CPlayer *pPlayer, int bByAnimus)
{
  _last_attacker_for_the_keeper_inform_zowb msg{};
  msg.nWorldCode = g_Main.m_byWorldCode;
  msg.byNumOfTime = static_cast<char>(GetNumOfTime());
  msg.wStartYear = GetStartYear();
  msg.byStartMonth = static_cast<char>(GetStartMonth());
  msg.byStartDay = static_cast<char>(GetStartDay());

  if (pPlayer)
  {
    strcpy_0(msg.uszKeeperHitter, pPlayer->m_Param.GetCharNameW());
    msg.bByAnimus = bByAnimus;

    unsigned __int8 type[2]{};
    type[0] = 51;
    type[1] = 19;
    if (g_Main.m_bConnectedWebAgentServer)
    {
      g_Network.m_pProcess[2]->LoadSendMsg(
        g_Main.m_byWebAgentServerNetInx,
        type,
        reinterpret_cast<char *>(&msg),
        sizeof(msg));
    }

    return;
  }

  m_logQuestDestroy.Write(
    ">> Laster Attacker for Keeper[%04d%02d%02d][Cnt:%d] - pPlayer Error",
    GetStartYear(),
    GetStartMonth(),
    GetStartDay(),
    GetNumOfTime());
}

void CHolyStoneSystem::SendMsg_CreateHolyMaster(CPlayer *pkDestroyer, int nControlSec)
{
  _create_holy_master_zocl msg{};
  msg.byHolyStoneRaceCode = static_cast<unsigned __int8>(pkDestroyer->m_Param.GetRaceCode());
  msg.byPlayerRaceCode = static_cast<unsigned __int8>(m_SaveData.m_nHolyMasterRace);

  const char *masterName = pkDestroyer->m_Param.GetCharNameW();
  msg.byNameLen = static_cast<unsigned __int8>(strlen_0(masterName) + 1);
  strcpy_0(msg.wszName, masterName);

  msg.nControlSec = nControlSec;
  msg.dwObjSerial = m_SaveData.m_dwDestroyerSerial;

  unsigned __int8 type[2]{};
  type[0] = 25;
  type[1] = 11;

  for (unsigned int index = 0; index < MAX_PLAYER; ++index)
  {
    if (g_Player[index].m_bLive)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        static_cast<unsigned __int16>(index),
        type,
        reinterpret_cast<char *>(&msg),
        msg.size());
    }
  }
}

void CHolyStoneSystem::SendMsg_HolyKeeperAttackAbleState(bool bAttackAble)
{
  _holy_keeper_attackable_inform_zocl msg{};
  msg.dwKeeperSerial = g_Keeper->m_dwObjSerial;
  msg.byMasterRace = static_cast<unsigned __int8>(GetHolyMasterRace());
  msg.bAttackAbleState = bAttackAble;

  unsigned __int8 type[2]{};
  type[0] = 25;
  type[1] = 22;

  for (unsigned int index = 0; index < MAX_PLAYER; ++index)
  {
    if (g_Player[index].m_bLive)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        static_cast<unsigned __int16>(index),
        type,
        reinterpret_cast<char *>(&msg),
        msg.size());
    }
  }
}

void CHolyStoneSystem::SendSMS_CompleteQuest(
  char byDestroyedRace,
  char *pwszMasterName,
  int nControlSec,
  char *szMasterClass,
  unsigned __int8 byMasterLv)
{
  if (!g_Main.m_bWorldOpen || !g_Main.m_bWorldService)
  {
    return;
  }

  _holy_complete_quest_inform_zowb msg{};
  msg.nWorldCode = g_Main.m_byWorldCode;
  strcpy_0(msg.szWorldName, g_Main.m_szWorldName);

  msg.byMasterRaceCode = static_cast<unsigned __int8>(GetHolyMasterRace());
  msg.byDestoryRaceCode = static_cast<unsigned __int8>(byDestroyedRace);
  strcpy_0(msg.wszMasterName, pwszMasterName);
  msg.byMasterLv = byMasterLv;

  memcpy_0(msg.szMasterClass, szMasterClass, 4u);
  msg.szMasterClass[4] = 0;

  const int holyMasterRace = GetHolyMasterRace();
  if (holyMasterRace >= 0 && holyMasterRace < 3)
  {
    const _PVP_RANK_DATA *rankData =
      CPvpUserAndGuildRankingSystem::Instance()->GetCurrentPvpRankData(static_cast<unsigned __int8>(holyMasterRace), 0);
    if (rankData)
    {
      for (int j = 0; j < 5; ++j)
      {
        strcpy_0(msg.wszMasterRaceSubBoss[j], rankData[j + 1].wszName);
      }
    }
  }

  for (unsigned __int8 race = 0; race < 3; ++race)
  {
    const _PVP_RANK_DATA *rankData = CPvpUserAndGuildRankingSystem::Instance()->GetCurrentPvpRankData(race, 0);
    if (rankData && rankData->wszName[0])
    {
      strcpy_0(msg.wszRaceBossName[race], rankData->wszName);
      msg.wszRaceBossName[race][16] = 0;
    }
    else
    {
      msg.wszRaceBossName[race][0] = 0;
    }
  }

  const int currentHour = GetCurrentHour();
  int targetHour = (nControlSec / 60 / 60 + currentHour) % 24;

  const int currentMin = GetCurrentMin();
  const int targetMin = (nControlSec / 60 + currentMin) % 60;
  if (targetMin < currentMin)
  {
    targetHour = (targetHour + 1) % 24;
  }

  msg.byChaosHour = static_cast<unsigned __int8>(targetHour);
  msg.byChaosMin = static_cast<unsigned __int8>(targetMin);
  msg.byNumOfTime = static_cast<unsigned __int8>(GetNumOfTime());

  msg.byStartHour = static_cast<unsigned __int8>(GetStartHour());
  msg.byStartMin = static_cast<unsigned __int8>(GetStartMin());
  msg.wStartYear = GetStartYear();
  msg.byStartMonth = static_cast<unsigned __int8>(GetStartMonth());
  msg.byStartDay = static_cast<unsigned __int8>(GetStartDay());

  unsigned __int8 type[2]{};
  type[0] = 51;
  type[1] = 3;

  if (g_Main.m_bConnectedWebAgentServer)
  {
    g_Network.m_pProcess[2]->LoadSendMsg(
      g_Main.m_byWebAgentServerNetInx,
      type,
      reinterpret_cast<char *>(&msg),
      static_cast<unsigned __int16>(sizeof(msg)));
  }

  m_logQuestDestroy.Write(">> Complete Quest! Next(%d:%d)", targetHour, targetMin);
}

void CHolyStoneSystem::SendMsg_HolyStoneSystemState(int nPlayerIndex)
{
  switch (m_SaveData.m_nSceneCode)
  {
    case 0:
      SendMsg_WaitStone(nPlayerIndex);
      break;
    case 1:
      SendMsg_EnterStone(nPlayerIndex);
      break;
    case 2:
      SendMsg_WaitKeeper(nPlayerIndex, 0);
      break;
    case 3:
    case 4:
    case 6:
      SendMsg_EnterKeeper(nPlayerIndex);
      break;
    case 5:
      SendMsg_WaitKeeper(nPlayerIndex, 1u);
      break;
    default:
      break;
  }
}

void CHolyStoneSystem::SendMsg_WaitStone(unsigned int n)
{
  unsigned __int8 type[2]{};
  type[0] = 25;
  type[1] = 45;
  _stone_wait_inform_zocl msg{};

  if (n == static_cast<unsigned int>(-1))
  {
    for (int index = 0; index < MAX_PLAYER; ++index)
    {
      if (g_Player[index].m_bLive)
      {
        g_Network.m_pProcess[0]->LoadSendMsg(
          index,
          type,
          reinterpret_cast<char *>(&msg),
          static_cast<unsigned __int16>(sizeof(msg)));
      }
    }
  }
  else
  {
    g_Network.m_pProcess[0]->LoadSendMsg(
      n,
      type,
      reinterpret_cast<char *>(&msg),
      static_cast<unsigned __int16>(sizeof(msg)));
  }
}

void CHolyStoneSystem::SendMsg_NoticeNextQuest(unsigned int n, unsigned __int8 byStoneMapMoveInfo)
{
  if (!m_dwNextStartTime)
  {
    return;
  }

  const unsigned int now = timeGetTime();
  const unsigned int remainMs = m_dwNextStartTime - now;
  const unsigned short remainSec = static_cast<unsigned short>(remainMs / 1000);

  _notice_next_quest_inform_zocl msg{};

  msg.wLeftSec = remainSec;
  msg.byStoneMapMoveInfo = byStoneMapMoveInfo;

  unsigned __int8 type[2]{};
  type[0] = 25;
  type[1] = 16;

  if (n == static_cast<unsigned int>(-1))
  {
    for (int index = 0; index < MAX_PLAYER; ++index)
    {
      if (!g_Player[index].m_bLive)
      {
        continue;
      }
      if (g_Player[index].GetLevel() < 25)
      {
        g_Player[index].m_byStoneMapMoveInfo = 0;
      }
      else
      {
        g_Player[index].m_byStoneMapMoveInfo = byStoneMapMoveInfo;
        g_Network.m_pProcess[0]->LoadSendMsg(
          index,
          type,
          reinterpret_cast<char *>(&msg),
          static_cast<unsigned __int16>(sizeof(msg)));
      }
    }
  }
  else
  {
    g_Network.m_pProcess[0]->LoadSendMsg(
      n,
      type,
      reinterpret_cast<char *>(&msg),
      static_cast<unsigned __int16>(sizeof(msg)));
  }
}

void CHolyStoneSystem::SendMsg_NotifyHolyKeeperAttackTimeBeKeepKeeper(char bKeepKeeper)
{
  _notify_holy_keeper_attack_time_be_keep_keeper_zocl msg{};
  msg.bKeepKeeper = (bKeepKeeper != 0);

  unsigned __int8 type[2]{};
  type[0] = 25;
  type[1] = 27;

  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    if (g_Player[index].m_bLive)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        index,
        type,
        reinterpret_cast<char *>(&msg),
        static_cast<unsigned __int16>(sizeof(msg)));
    }
  }
}

void CHolyStoneSystem::SendMsg_EnterStone(unsigned int n)
{
  CHolyScheduleData::__HolyScheduleNode *scheduleNode = m_ScheculeData.GetIndex(m_SaveData.m_byNumOfTime);
  if (!scheduleNode)
  {
    return;
  }

  const unsigned int loopTime = GetLoopTime();
  const unsigned int delta = m_dwCheckTime[1] - scheduleNode->m_nSceneTime[0];

  _stone_enter_inform_zocl msg{};

  msg.bNow = (n == static_cast<unsigned int>(-1));
  msg.wTotalSecTime = static_cast<unsigned __int16>((m_dwCheckTime[1] - m_dwCheckTime[0]) / 1000);
  msg.zHurrySecTime = static_cast<__int16>((delta - loopTime) / 1000);
  msg.zExitSecTime = static_cast<__int16>((m_dwCheckTime[1] - loopTime) / 1000);
  msg.wElapseTime = static_cast<unsigned __int16>((timeGetTime() - m_dwCheckTime[0]) / 1000);

  unsigned __int8 type[2]{};
  type[0] = 25;
  type[1] = 1;

  if (n == static_cast<unsigned int>(-1))
  {
    for (int index = 0; index < MAX_PLAYER; ++index)
    {
      if (g_Player[index].m_bLive)
      {
        g_Network.m_pProcess[0]->LoadSendMsg(
          index,
          type,
          reinterpret_cast<char *>(&msg),
          static_cast<unsigned __int16>(sizeof(msg)));
      }
    }
  }
  else
  {
    g_Network.m_pProcess[0]->LoadSendMsg(
      n,
      type,
      reinterpret_cast<char *>(&msg),
      static_cast<unsigned __int16>(sizeof(msg)));
  }
}

void CHolyStoneSystem::SendMsg_ExitStone()
{
  _stone_exit_inform_zocl msg{};
  msg.byMasterRace = static_cast<char>(GetHolyMasterRace());

  unsigned __int8 type[2]{};
  type[0] = 25;
  type[1] = 2;

  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    if (g_Player[index].m_bLive)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        index,
        type,
        reinterpret_cast<char *>(&msg),
        static_cast<unsigned __int16>(sizeof(msg)));
    }
  }
}

void CHolyStoneSystem::SendMsg_StartBattle()
{
  _notice_start_quest_inform_zocl msg{};

  unsigned __int8 type[2]{};
  type[0] = 25;
  type[1] = 18;

  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    if (g_Player[index].m_bLive)
    {
      g_Player[index].SetCntEnable(true);
      g_Network.m_pProcess[0]->LoadSendMsg(
        index,
        type,
        reinterpret_cast<char *>(&msg),
        static_cast<unsigned __int16>(sizeof(msg)));
    }
  }
}

void CHolyStoneSystem::SendMsg_EndBattle(char byLoseRace)
{
  _notice_end_quest_inform_zocl msg{};
  msg.byWinRace = static_cast<char>(GetHolyMasterRace());
  msg.byLoseRace = byLoseRace;

  unsigned __int8 type[2]{};
  type[0] = 25;
  type[1] = 19;

  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    if (g_Player[index].m_bLive)
    {
      g_Player[index].SetCntEnable(false);
      g_Network.m_pProcess[0]->LoadSendMsg(
        index,
        type,
        reinterpret_cast<char *>(&msg),
        static_cast<unsigned __int16>(sizeof(msg)));
    }
  }
}

void CHolyStoneSystem::SendMsg_EnterKeeper(unsigned int n)
{
  const unsigned __int8 numOfTime = GetNumOfTime();
  CHolyScheduleData::__HolyScheduleNode *scheduleNode = m_ScheculeData.GetIndex(numOfTime);
  if (!scheduleNode)
  {
    return;
  }

  const unsigned int loopTime = GetLoopTime();
  const unsigned int delta = m_dwCheckTime[6] - scheduleNode->m_nSceneTime[0];

  _keeper_enter_inform_zocl msg{};

  msg.bNow = (n == static_cast<unsigned int>(-1));
  msg.wTotalSecTime = static_cast<unsigned __int16>((m_dwCheckTime[6] - m_dwCheckTime[2]) / 1000);
  msg.zHurrySecTime = static_cast<__int16>((delta - loopTime) / 1000);
  msg.zExitSecTime = static_cast<__int16>((m_dwCheckTime[6] - loopTime) / 1000);
  msg.byMasterRace = static_cast<char>(GetHolyMasterRace());
  msg.bChaos = (GetSceneCode() == 6);
  msg.byAttackAbleType = static_cast<char>(GetSceneCode() == 3);

  unsigned __int8 type[2]{};
  type[0] = 25;
  type[1] = 3;

  if (n == static_cast<unsigned int>(-1))
  {
    for (int index = 0; index < MAX_PLAYER; ++index)
    {
      if (g_Player[index].m_bLive)
      {
        g_Network.m_pProcess[0]->LoadSendMsg(
          index,
          type,
          reinterpret_cast<char *>(&msg),
          static_cast<unsigned __int16>(sizeof(msg)));
      }
    }
  }
  else
  {
    g_Network.m_pProcess[0]->LoadSendMsg(
      n,
      type,
      reinterpret_cast<char *>(&msg),
      static_cast<unsigned __int16>(sizeof(msg)));
  }
}

void CHolyStoneSystem::SendMsg_WaitKeeper(unsigned int n, char byWaitType)
{
  _kepper_wait_inform_zocl msg{};
  msg.byWaitType = byWaitType;
  msg.byMasterRace = static_cast<char>(GetHolyMasterRace());

  unsigned __int8 type[2]{};
  type[0] = 25;
  type[1] = 44;

  if (n == static_cast<unsigned int>(-1))
  {
    for (int index = 0; index < MAX_PLAYER; ++index)
    {
      if (g_Player[index].m_bLive)
      {
        g_Network.m_pProcess[0]->LoadSendMsg(
          index,
          type,
          reinterpret_cast<char *>(&msg),
          static_cast<unsigned __int16>(sizeof(msg)));
      }
    }
  }
  else
  {
    g_Network.m_pProcess[0]->LoadSendMsg(
      n,
      type,
      reinterpret_cast<char *>(&msg),
      static_cast<unsigned __int16>(sizeof(msg)));
  }
}

void CHolyStoneSystem::SendMsg_HolyKeeperStateChaos()
{
  _holy_keeper_change_chaos_inform msg{};

  msg.dwKeeperSerial = g_Keeper->m_dwObjSerial;
  msg.byMasterRace = static_cast<char>(GetHolyMasterRace());

  unsigned __int8 type[2]{};
  type[0] = 25;
  type[1] = 15;

  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    if (g_Player[index].m_bLive)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        index,
        type,
        reinterpret_cast<char *>(&msg),
        static_cast<unsigned __int16>(sizeof(msg)));
    }
  }
}

void CHolyStoneSystem::GiveHSKQuest()
{
  InitQuestCash();

  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    if (g_Player[j].m_bLive)
    {
      g_Player[j].RecvHSKQuest(0x64u, 3u, 0, 0, 0, 0xFFu);
    }
  }

  int randSeed = rand();
  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    ++randSeed;
    CPlayer *player = &g_Player[j];
    if (player->m_bLive && player->m_byHSKQuestCode == 100)
    {
      if (player->GetLevel() < 25)
      {
        player->RecvHSKQuest(0x64u, 3u, 0, 0, 0, 0xFFu);
      }
      else if (player->m_pPartyMgr && player->m_pPartyMgr->IsPartyMode())
      {
        CPartyPlayer **partyMembers = player->m_pPartyMgr->GetPtrPartyMember();
        if (partyMembers)
        {
          for (int k = 0; k < 8 && partyMembers[k]; ++k)
          {
            CPlayer *member = &g_Player[partyMembers[k]->m_wZoneIndex];
            const int memberRace = member->m_Param.GetRaceCode();
            const int playerRace = player->m_Param.GetRaceCode();
            if (memberRace == playerRace && member->m_byHSKQuestCode == 100)
            {
              const unsigned __int8 byNumOfTime = GetNumOfTime();
              member->RecvHSKQuest(static_cast<unsigned __int8>(randSeed % 2), 0, 0, 0, 0, byNumOfTime);
            }
          }
        }
      }
      else
      {
        const unsigned __int8 byNumOfTime = GetNumOfTime();
        player->RecvHSKQuest(static_cast<unsigned __int8>(randSeed % 2), 0, 0, 0, 0, byNumOfTime);
      }
    }
  }
}

void CHolyStoneSystem::SendHolyStoneHPToRaceBoss()
{
  _holy_stone_hp_inform_zocl msg{};

  memset_0(&msg, 0, sizeof(msg));
  for (int j = 0; j < 3; ++j)
  {
    msg.wHPRate[j] = static_cast<unsigned __int16>(g_Stone[j].CalcCurHPRate());
  }

  unsigned __int8 type[2]{};
  type[0] = 13;
  type[1] = 33;

  for (int k = 0; k < MAX_PLAYER; ++k)
  {
    CPlayer *player = &g_Player[k];
    if (player->m_bLive)
    {
      const unsigned int serial = player->m_Param.GetCharSerial();
      const int race = player->m_Param.GetRaceCode();
      if (CPvpUserAndGuildRankingSystem::Instance()->IsCurrentRaceBossGroup(race, serial))
      {
        g_Network.m_pProcess[0]->LoadSendMsg(
          player->m_ObjID.m_wIndex,
          type,
          reinterpret_cast<char *>(&msg),
          static_cast<unsigned __int16>(sizeof(msg)));
      }
    }
  }
}

void CHolyStoneSystem::SendHolyStoneHP(CPlayer *pkPlayer)
{
  if (m_SaveData.m_nSceneCode != 1 || m_SaveData.m_nHolyMasterRace != -1)
  {
    return;
  }

  const unsigned int serial = pkPlayer->m_Param.GetCharSerial();
  const unsigned __int8 raceCode = pkPlayer->m_Param.GetRaceCode();
  CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
  if (!ranking->IsCurrentRaceBossGroup(raceCode, serial))
  {
    return;
  }

  _holy_stone_hp_inform_zocl msg{};
  for (int j = 0; j < 3; ++j)
  {
    msg.wHPRate[j] = static_cast<unsigned __int16>(g_Stone[j].CalcCurHPRate());
  }

  unsigned __int8 type[2]{};
  type[0] = 13;
  type[1] = 33;
  g_Network.m_pProcess[0]->LoadSendMsg(
    pkPlayer->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CHolyStoneSystem::SendNotifyHolyStoneDestroyedToRaceBoss()
{
  _notify_holy_stone_destroyed_zocl msg{};

  unsigned __int8 type[2]{};
  type[0] = 13;
  type[1] = 34;

  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    CPlayer *player = &g_Player[j];
    if (player->m_bLive && player->m_bOper)
    {
      const unsigned int serial = player->m_Param.GetCharSerial();
      const int race = player->m_Param.GetRaceCode();
      if (CPvpUserAndGuildRankingSystem::Instance()->IsCurrentRaceBossGroup(race, serial))
      {
        g_Network.m_pProcess[0]->LoadSendMsg(
          player->m_ObjID.m_wIndex,
          type,
          reinterpret_cast<char *>(&msg),
          static_cast<unsigned __int16>(sizeof(msg)));
      }
    }
  }
}

void CHolyStoneSystem::WriteLogPer10Min_Combat()
{
  char timeBuffer[152]{};
  _strtime(timeBuffer);
  m_logPer10Min.Write("TIME : %s", timeBuffer);

  int classCountByRace[3][4]{};
  int guildMemberCount[MAX_GUILD]{};

  CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    CPlayer *player = &g_Player[index];
    if (!player->m_bLive || player->m_bCorpse || player->GetLevel() < 25 || player->m_pCurMap != g_Stone[0].m_pCurMap)
    {
      continue;
    }

    const int race = player->m_Param.GetRaceCode();
    if (race < 0 || race >= 3)
    {
      continue;
    }

    _class_fld *classData = player->m_Param.GetPtrCurClass();
    if (classData && classData->m_nClass >= 0 && classData->m_nClass < 4)
    {
      ++classCountByRace[race][classData->m_nClass];
    }

    if (player->m_Param.m_pGuild && player->m_Param.m_pGuild->m_nIndex < MAX_GUILD)
    {
      ++guildMemberCount[player->m_Param.m_pGuild->m_nIndex];
    }

    const unsigned int currentRaceBossSerial = ranking->GetCurrentRaceBossSerial(static_cast<unsigned __int8>(race), 0);
    if (player->m_dwObjSerial == currentRaceBossSerial)
    {
      m_logPer10Min.Write("R.boss(%s): %s", kRaceCodeName[race], player->m_Param.GetCharNameA());
      continue;
    }

    if (ranking->IsRaceViceBoss(static_cast<unsigned __int8>(race), player->m_dwObjSerial))
    {
      m_logPer10Min.Write("vice-R.boss(%s): %s", kRaceCodeName[race], player->m_Param.GetCharNameA());
    }
  }

  for (int race = 0; race < 3; ++race)
  {
    const int total =
      classCountByRace[race][0] + classCountByRace[race][1] + classCountByRace[race][2] + classCountByRace[race][3];
    m_logPer10Min.Write(
      "%s >> Total: %d Wr: %d Rr: %d Sp: %d Sc: %d",
      kRaceCodeName[race],
      total,
      classCountByRace[race][0],
      classCountByRace[race][1],
      classCountByRace[race][2],
      classCountByRace[race][3]);
  }

  if (g_Guild)
  {
    for (int guildIndex = 0; guildIndex < MAX_GUILD; ++guildIndex)
    {
      if (g_Guild[guildIndex].IsFill() && guildMemberCount[guildIndex] > 0)
      {
        m_logPer10Min.Write(
          "%s (%s) : %d / %d",
          g_Guild[guildIndex].m_aszName,
          kRaceCodeName[g_Guild[guildIndex].m_byRace],
          guildMemberCount[guildIndex],
          g_Guild[guildIndex].m_nMemberNum);
      }
    }
  }

  m_logPer10Min.Write("============================================");
  m_logPer10Min.Write("============================================");
}

void CHolyStoneSystem::SetEffectToDestroyerGuildMember()
{
  if (!m_pkDestroyer || !m_pkDestroyer->m_Param.m_pGuild)
  {
    return;
  }

  for (int index = 0; index < 50; ++index)
  {
    _guild_member_info *member = &m_pkDestroyer->m_Param.m_pGuild->m_MemberData[index];
    if (member->IsFill() && member->pPlayer && member->pPlayer->m_bOper && member->pPlayer != m_pkDestroyer)
    {
      member->pPlayer->SetLastAttBuff(true);
    }
  }
}

void CHolyStoneSystem::RecoverPvpCash()
{
  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    CPlayer *player = &g_Player[index];
    if (!player->m_bLive || !player->m_bOper || player->m_byHSKQuestCode == 100)
    {
      continue;
    }

    if (!player->m_MinigTicket.AuthLastMentalTicket(
          g_HolySys.GetStartYear(),
          g_HolySys.GetStartMonth(),
          g_HolySys.GetStartDay(),
          g_HolySys.GetStartHour(),
          g_HolySys.GetNumOfTime()))
    {
      continue;
    }

    if (!player->m_kPvpCashPoint.GetRaceWarRecvr())
    {
      player->RewardRaceWarPvpCash();
    }
  }
}

void CHolyStoneSystem::ReceiveDestroyKeeper(CCharacter *pCharacter)
{
  if (GetSceneCode() == 3)
  {
    SetScene(GetNumOfTime(), 5, 0, 3);
    if (pCharacter->m_ObjID.m_byID == 3)
    {
      CAnimus *animus = static_cast<CAnimus *>(pCharacter);
      if (animus->m_pMaster)
      {
        SendMsg_to_webagent_about_last_attacker_for_keeper(animus->m_pMaster, 1);
        SetKeeperDestroyRace(static_cast<unsigned __int8>(animus->m_pMaster->m_Param.GetRaceCode()));
      }
    }
    else if (pCharacter->m_ObjID.m_byID == 0)
    {
      CPlayer *player = static_cast<CPlayer *>(pCharacter);
      SendMsg_to_webagent_about_last_attacker_for_keeper(player, 0);
      SetKeeperDestroyRace(static_cast<unsigned __int8>(player->m_Param.GetRaceCode()));
    }

    COreAmountMgr *oreMgr = COreAmountMgr::Instance();
    oreMgr->InitRemainOreAmount(0xFFFFFFFF, 0xFFFFFFFF);
    oreMgr = COreAmountMgr::Instance();
    oreMgr->InsertOreLog(1u);
    oreMgr = COreAmountMgr::Instance();
    oreMgr->IncreaseOreAmount();
    oreMgr = COreAmountMgr::Instance();
    oreMgr->IncreaseOreCount();
  }
}

char CHolyStoneSystem::CheckHolyMaster(CPlayer *pAtter, unsigned __int8 byDestroyStoneRaceCode)
{
  if (GetSceneCode() != 1)
  {
    return 0;
  }

  CHolyScheduleData::__HolyScheduleNode *scheduleNode = m_ScheculeData.GetIndex(GetNumOfTime());
  if (!scheduleNode)
  {
    return 0;
  }

  if (GetHolyMasterRace() != -1)
  {
    return 0;
  }

  WriteLogPer10Min_Combat();

  const int attackerRace = pAtter->m_Param.GetRaceCode();
  SetHolyMasterRace(attackerRace);
  SetDestroyStoneRace(byDestroyStoneRaceCode);
  CRaceBuffManager::Instance()->RequestHolyQuestRaceBuff(2);
  CheckKeeperPlusTime();

  const int scene3Term = scheduleNode->m_nSceneTime[3];
  float hpRate = 0.0f;
  if (m_SaveData.m_nStartStoneHP)
  {
    CHolyStone *holyStone = &g_Stone[GetHolyMasterRace()];
    hpRate = static_cast<float>(holyStone->GetHP()) / static_cast<float>(m_SaveData.m_nStartStoneHP);
  }
  if (hpRate >= 1.0f)
  {
    hpRate = 1.0f;
  }

  m_SaveData.m_dwTerm[1] = static_cast<unsigned int>(static_cast<float>(scene3Term) * hpRate);
  SetScene(GetNumOfTime(), 2, 0, 5);

  m_logQuest.Write(
    "Create Master >> race:%d, name:%s, stone:%d",
    attackerRace,
    pAtter->m_Param.GetCharNameA(),
    static_cast<int>(byDestroyStoneRaceCode));
  m_logQuest.Write(
    ">> Change Schedule : %d -> HS_SCENE_BATTLE_END_WAIT_TIME, PlusSecTime:%d, controlSecTime:%d",
    GetSceneCode(),
    static_cast<int>(m_SaveData.m_dwTerm[0] / 1000),
    static_cast<int>(m_SaveData.m_dwTerm[1] / 1000));
  m_logPer10Min.Write(
    "WIN : %s (%s), Destroy(%s)",
    pAtter->m_Param.GetCharNameA(),
    kRaceCodeName[attackerRace],
    kRaceCodeName[byDestroyStoneRaceCode]);

  const unsigned __int8 failRaceCode =
    static_cast<unsigned __int8>(3 - (byDestroyStoneRaceCode + attackerRace));
  PeneltyLoseRace(byDestroyStoneRaceCode);
  PeneltyFailRace(failRaceCode);

  m_pkDestroyer = pAtter;
  m_SaveData.m_dwDestroyerSerial = pAtter->m_dwObjSerial;
  m_SaveData.m_eDestroyerState = 2;
  SendNotifyHolyStoneDestroyedToRaceBoss();

  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    CPlayer *player = &g_Player[index];
    if (!player->m_bOper || !player->m_bLive)
    {
      continue;
    }

    if (GetHolyMasterRace() == player->m_Param.GetRaceCode() && player->m_byHSKQuestCode != 100)
    {
      const long double pvpLeak = player->GetPvpPointLeak();
      if (pvpLeak < 0.0)
      {
        player->AlterPvPPoint(-static_cast<double>(pvpLeak), holy_award, 0xFFFFFFFFu);
      }
    }
    player->SetPvpPointLeak(0.0);
  }

  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    CPlayer *player = &g_Player[index];
    if (player->m_bLive && (player->m_byHSKQuestCode != 100 || player == pAtter))
    {
      player->HSKQuestEnd_Att(byDestroyStoneRaceCode, pAtter);
    }
  }

  const int controlSec =
    static_cast<int>((m_SaveData.m_dwTerm[1] + m_SaveData.m_dwTerm[0] + scheduleNode->m_nSceneTime[0]) / 1000);
  SendMsg_CreateHolyMaster(pAtter, controlSec);

  if (m_pkDestroyer->m_Param.m_pGuild)
  {
    m_SaveData.m_dwDestroyerGuildSerial = m_pkDestroyer->m_Param.m_pGuild->m_dwSerial;
  }
  else
  {
    m_SaveData.m_dwDestroyerGuildSerial = static_cast<unsigned int>(-1);
  }

  SetEffectToDestroyerGuildMember();
  SendSMS_CompleteQuest(
    byDestroyStoneRaceCode,
    pAtter->m_Param.GetCharNameW(),
    controlSec,
    pAtter->m_Param.m_pClassData->m_strCode,
    static_cast<unsigned __int8>(pAtter->GetLevel()));
  SendMsg_EndBattle(byDestroyStoneRaceCode);
  DestroyHolyStone();
  SendMsg_ExitStone();
  RecoverPvpCash();

  CRaceBossWinRate::Instance()->UpdateWinCnt(pAtter->m_Param.GetRaceCode());

  _race_battle_log_info qryData{};
  qryData.byNth = static_cast<unsigned __int8>(m_SaveData.m_byNumOfTime);
  qryData.dwEndTime = GetKorLocalTime();
  qryData.byWinRace = static_cast<unsigned __int8>(pAtter->m_Param.GetRaceCode());
  qryData.byLoseRace = byDestroyStoneRaceCode;
  qryData.dwBossSerilal0 = CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(0, 0);
  qryData.dwBossSerilal1 = CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(1, 0);
  qryData.dwBossSerilal2 = CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(2, 0);
  g_Main.PushDQSData(0xFFFFFFFFu, nullptr, 0x89u, reinterpret_cast<char *>(&qryData), sizeof(qryData));

  if (g_Main.IsReleaseServiceMode())
  {
    _holy_quest_report_wrac report{};
    report.byRaceCode = static_cast<unsigned __int8>(pAtter->m_Param.GetRaceCode());
    strcpy_0(report.wszCharName, pAtter->m_Param.GetCharNameW());
    report.byDestroyedRaceCode = byDestroyStoneRaceCode;

    unsigned __int8 type[2]{};
    memcpy_0(type, "2e", 2u);
    g_Network.m_pProcess[1]->LoadSendMsg(
      0,
      type,
      reinterpret_cast<char *>(&report),
      report.size());
  }

  return 1;
}

void CHolyStoneSystem::PeneltyLoseRace(unsigned __int8 byDestroyedRace)
{
  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    CPlayer *player = &g_Player[index];
    if (!player->m_bLive || !player->m_bOper || player->m_byHSKQuestCode == 100)
    {
      continue;
    }

    if (player->m_Param.GetRaceCode() != byDestroyedRace)
    {
      continue;
    }

    const bool hasMentalTicket = player->m_MinigTicket.AuthLastMentalTicket(
      g_HolySys.GetStartYear(),
      g_HolySys.GetStartMonth(),
      g_HolySys.GetStartDay(),
      g_HolySys.GetStartHour(),
      g_HolySys.GetNumOfTime());

    int alterPoint = hasMentalTicket ? m_nRaceBattlePoint[2][0] : m_nRaceBattlePoint[2][1];
    const double currentPoint = player->m_Param.GetPvPPoint();
    if (static_cast<double>(std::abs(alterPoint)) > currentPoint)
    {
      alterPoint = static_cast<int>(-0.0 - player->m_Param.GetPvPPoint());
    }

    player->AlterPvPPoint(static_cast<double>(alterPoint), holy_dec, 0xFFFFFFFF);
    player->SendMsg_RaceBattlePenelty(alterPoint, 0);
  }
}

void CHolyStoneSystem::PeneltyFailRace(unsigned __int8 byFailRace)
{
  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    CPlayer *player = &g_Player[j];
    if (player->m_bLive && player->m_bOper && player->m_byHSKQuestCode != 100)
    {
      const int race = player->m_Param.GetRaceCode();
      if (race == byFailRace)
      {
        const unsigned __int8 byNumOfTime = g_HolySys.GetNumOfTime();
        const unsigned __int8 byCurrentHour = g_HolySys.GetStartHour();
        const unsigned __int8 startDay = g_HolySys.GetStartDay();
        const unsigned __int8 startMonth = g_HolySys.GetStartMonth();
        const unsigned __int16 startYear = g_HolySys.GetStartYear();

        if (player->m_MinigTicket.AuthLastMentalTicket(
              startYear,
              startMonth,
              startDay,
              byCurrentHour,
              byNumOfTime))
        {
          int alterPoint = m_nRaceBattlePoint[1][0];
          const double curPoint = player->m_Param.GetPvPPoint();
          if (static_cast<double>(std::abs(alterPoint)) > curPoint)
          {
            alterPoint = static_cast<int>(-0.0 - player->m_Param.GetPvPPoint());
          }
          player->AlterPvPPoint(static_cast<double>(alterPoint), holy_dec, 0xFFFFFFFF);
          player->SendMsg_RaceBattlePenelty(alterPoint, 0);
        }
        else
        {
          int alterPoint = m_nRaceBattlePoint[1][1];
          const double curPoint = player->m_Param.GetPvPPoint();
          if (static_cast<double>(std::abs(alterPoint)) > curPoint)
          {
            alterPoint = static_cast<int>(-0.0 - player->m_Param.GetPvPPoint());
          }
          player->AlterPvPPoint(static_cast<double>(alterPoint), holy_dec, 0xFFFFFFFF);
          player->SendMsg_RaceBattlePenelty(alterPoint, 0);
        }
      }
    }
  }
}

void CHolyStoneSystem::On_HS_SCENE_BATTLE_TIME()
{
  g_Keeper->Destroy(1u, nullptr);
  m_dwNextStartTime = 0;
  m_pkDestroyer = nullptr;
  m_SaveData.m_nHolyMasterRace = -1;
  m_SaveData.m_nDestroyStoneRace = -1;
  m_SaveData.m_dwDestroyerSerial = static_cast<unsigned int>(-1);
  m_SaveData.m_eDestroyerState = 3;
  m_SaveData.m_wStartYear = GetCurrentYear();
  m_SaveData.m_byStartMonth = GetCurrentMonth();
  m_SaveData.m_byStartDay = GetCurrentDay();
  m_SaveData.m_byStartHour = GetCurrentHour();
  m_SaveData.m_byStartMin = GetCurrentMin();
  ReleaseLastAttBuff();
  m_SaveData.m_dwDestroyerGuildSerial = static_cast<unsigned int>(-1);

  CreateHolyStone();
  SendMsg_EnterStone(static_cast<unsigned int>(-1));
  SendMsg_StartBattle();
  GiveHSKQuest();
  SendHolyStoneHPToRaceBoss();

  CMoveMapLimitManager::Instance()->RequestElanMapUserForceMoveHQ();
  if (CGoldenBoxItemMgr::Instance()->Get_Event_Status() == 2)
  {
    SetGoldBoxConsumable(true);
  }

  const int startMin = GetStartMin();
  const int startHour = GetStartHour();
  const int startDay = GetStartDay();
  const int startMonth = GetStartMonth();
  const unsigned __int16 startYear = GetStartYear();

  char logPath[128];
  sprintf_s(
    logPath,
    "..\\ZoneServerLog\\ServiceLog\\%s_HolyInfo_%d_%d_%d_%d_%d.txt",
    g_Main.m_szWorldName,
    startYear,
    startMonth,
    startDay,
    startHour,
    startMin);
  m_logPer10Min.SetWriteLogFile(logPath, 1, 0, 0, 0);
}

void CHolyStoneSystem::On_HS_SCENE_BATTLE_END_WAIT_TIME()
{
  DestroyHolyStone();

  if (GetHolyMasterRace() == -1)
  {
    SendMsg_ExitStone();
    SendMsg_EndBattle(static_cast<char>(0xFF));
    CRaceBossWinRate::Instance()->UpdateTotalCnt();
    if (g_Main.m_bWorldOpen)
    {
      char qryData[4]{};
      qryData[0] = static_cast<char>(m_SaveData.m_byNumOfTime);
      (void)GetKorLocalTime();

      const unsigned int raceBossSerial0 = CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(0, 0);
      const unsigned int raceBossSerial1 = CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(1, 0);
      const unsigned int raceBossSerial2 = CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(2, 0);
g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x89u, qryData, 24);
      for (int j = 0; j < 3; ++j)
      {
        PeneltyFailRace(static_cast<unsigned __int8>(j));
      }
    }
  }

  SendNotifyHolyStoneDestroyedToRaceBoss();
}

void CHolyStoneSystem::On_HS_SCENE_KEEPER_ATTACKABLE_TIME()
{
  if (m_SaveData.m_eDestroyerState == 2)
  {
    m_SaveData.m_eDestroyerState = 0;
    m_SaveData.m_dwDestroyerSerial = static_cast<unsigned int>(-1);
    ReleaseLastAttBuff();
    m_SaveData.m_dwDestroyerGuildSerial = static_cast<unsigned int>(-1);
    SendIsArriveDestroyer(0);
  }

  UnAllRegisterPerAutoMine();

  if (GetHolyMasterRace() != -1)
  {
    SetKeeperDestroyRace(0xFFu);
    CreateHolyKeeper(0);
    g_Keeper->SetDamageAbleState(1);
    SendMsg_EnterKeeper(static_cast<unsigned int>(-1));
  }
}

void CHolyStoneSystem::On_HS_SCENE_KEEPER_DEATTACKABLE_TIME()
{
  if (GetHolyMasterRace() != -1)
  {
    CreateHolyKeeper(1);
    g_Keeper->SetDamageAbleState(0);
    SendMsg_EnterKeeper(static_cast<unsigned int>(-1));
  }

  COreAmountMgr *oreMgr = COreAmountMgr::Instance();
  oreMgr->ReLoad();
  oreMgr = COreAmountMgr::Instance();
  oreMgr->InitRemainOreAmount(0xFFFFFFFF, 0xFFFFFFFF);
  oreMgr = COreAmountMgr::Instance();
  oreMgr->InsertOreLog(1u);

  oreMgr = COreAmountMgr::Instance();
  const unsigned int remainOre = oreMgr->GetRemainOre();
  oreMgr = COreAmountMgr::Instance();
  const unsigned int transferOre = oreMgr->GetOreTransferAmount();
  const unsigned int totalRemain = transferOre + remainOre;
  oreMgr = COreAmountMgr::Instance();
  const unsigned int totalOre = oreMgr->GetTotalOre();
  oreMgr = COreAmountMgr::Instance();
  oreMgr->InitRemainOreAmount(totalRemain, totalOre);
  oreMgr = COreAmountMgr::Instance();
  oreMgr->InitTransferOre(0, 0);
}

void CHolyStoneSystem::On_HS_SCENE_KEEPER_DIE_TIME()
{
  DestroyHolyKeeper();
  SendMsg_WaitKeeper(static_cast<unsigned int>(-1), 1u);
}

void CHolyStoneSystem::On_HS_SCENE_KEEPER_CHAOS_TIME()
{
  CreateHolyKeeper(1);
  g_Keeper->SetStateChaos();
  g_Keeper->SetDamageAbleState(0);
  SendMsg_HolyKeeperStateChaos();
}

bool _QUEST_CASH::isload()
{
  return dwAvatorSerial != static_cast<unsigned int>(-1);
}

bool _QUEST_CASH_OTHER::isLoaded()
{
  return dwAvatorSerial != 0;
}

bool CHolyStoneSystem::IsControlScene()
{
  return GetSceneCode() == 3 || GetSceneCode() == 4;
}

void CHolyStoneSystem::PushStoreQuestCash(
  unsigned int dwAvatorSerial,
  unsigned __int8 byQuestType,
  int nPvpPoint,
  unsigned __int16 wKillPoint,
  unsigned __int16 wDiePoint,
  unsigned __int8 byCristalBattleDBInfo,
  unsigned __int8 byHSKTime)
{
  if (m_SaveData.m_nSceneCode == 1)
  {
    for (int j = 0; j < 5064; ++j)
    {
      _QUEST_CASH *entry = &m_cashQuest[j];
      if (entry->isload() && entry->dwAvatorSerial == dwAvatorSerial)
      {
        entry->byQuestType = byQuestType;
        entry->nPvpPoint = nPvpPoint;
        entry->wKillPoint = wKillPoint;
        entry->wDiePoint = wDiePoint;
        entry->byCristalBattleDBInfo = byCristalBattleDBInfo;
        entry->byHSKTime = byHSKTime;
        return;
      }
    }
    for (int j = 0; j < 5064; ++j)
    {
      _QUEST_CASH *entry = &m_cashQuest[j];
      if (!entry->isload())
      {
        entry->dwAvatorSerial = dwAvatorSerial;
        entry->byQuestType = byQuestType;
        entry->nPvpPoint = nPvpPoint;
        entry->wKillPoint = wKillPoint;
        entry->wDiePoint = wDiePoint;
        entry->byCristalBattleDBInfo = byCristalBattleDBInfo;
        entry->byHSKTime = byHSKTime;
        return;
      }
    }
  }
}

_QUEST_CASH *CHolyStoneSystem::FindStoragedQuestCash(unsigned int dwAvatorSerial)
{
  for (int j = 0; j < 5064; ++j)
  {
    if (m_cashQuest[j].dwAvatorSerial == dwAvatorSerial)
    {
      m_cashQuest[j].init();
      return &m_cashQuest[j];
    }
  }

  return nullptr;
}

void CHolyStoneSystem::PushQuestCash_Other(unsigned int dwAvatorSerial, unsigned __int8 byStoneMapMoveInfo)
{
  for (int j = 0; j < 5064; ++j)
  {
    _QUEST_CASH_OTHER *entry = &m_cashQuestOther[j];
    if (entry->isLoaded() && entry->dwAvatorSerial == dwAvatorSerial)
    {
      entry->byStoneMapMoveInfo = byStoneMapMoveInfo;
      return;
    }
  }
  for (int j = 0; j < 5064; ++j)
  {
    _QUEST_CASH_OTHER *entry = &m_cashQuestOther[j];
    if (!entry->isLoaded())
    {
      entry->dwAvatorSerial = dwAvatorSerial;
      entry->byStoneMapMoveInfo = byStoneMapMoveInfo;
      return;
    }
  }
}

_QUEST_CASH_OTHER *CHolyStoneSystem::PopStoredQuestCash_Other(unsigned int dwAvatorSerial)
{
  for (int j = 0; j < 5064; ++j)
  {
    if (m_cashQuestOther[j].dwAvatorSerial == dwAvatorSerial)
    {
      m_cashQuestOther[j].init();
      return &m_cashQuestOther[j];
    }
  }

  return nullptr;
}

bool CHolyStoneSystem::IsMentalPass()
{
  return m_pMentalPass;
}
