#include "pch.h"

#include "CHolyStoneSystem.h"

#include <atltime.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <mmsystem.h>

#include "CHolyKeeper.h"
#include "CHolyStone.h"
#include "CHolyStoneSystemDataMgr.h"
#include "COreAmountMgr.h"
#include "CMoveMapLimitManager.h"
#include "CGoldenBoxItemMgr.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CRaceBossWinRate.h"
#include "CRaceBuffManager.h"
#include "CNetworkEX.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

bool CHolyStoneSystem::InitHolySystem(CHolyStoneSystem *pSystem)
{
  if (!CHolyStoneSystemDataMgr::LoadIni(pSystem))
  {
    MyMessageBox("Error", "CHolyStoneSystem::InitHolySystem() : CHolyStoneSystemDataMgr::LoadIni(*this) Fail!");
    return false;
  }

  if (!CHolyStoneSystemDataMgr::LoadSceduleData(&pSystem->m_ScheculeData))
  {
    MyMessageBox(
      "Error",
      "CHolyStoneSystem::InitHolySystem() : CHolyStoneSystemDataMgr::LoadSceduleData( m_ScheculeData ) Fail!");
    return false;
  }

  CHolyScheduleData::__HolyScheduleNode *firstSchedule = pSystem->m_ScheculeData.GetIndex(1);
  if (!firstSchedule)
  {
    MyMessageBox("Error", "CHolyStoneSystem::InitHolySystem() : pFirstSchedule = m_ScheculeData.GetIndex(1) NULL!");
    return false;
  }

  for (int j = 0; j < pSystem->m_nHolyStoneNum; ++j)
  {
    __holy_stone_data *stoneData = &pSystem->m_HolyStoneData[j];
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
    pSystem->m_SaveData.DefaultInit(firstSchedule);
    oreMgr = COreAmountMgr::Instance();
    oreMgr->InitRemainOreAmount(0xFFFFFFFF, 0xFFFFFFFF);
    oreMgr->InitTransferOre(0, 0);
  }

  if (!scheduleInit
      && (!CHolyStoneSystemDataMgr::LoadStateData(&pSystem->m_SaveData) || !pSystem->ContinueStartSystem()))
  {
    pSystem->m_SaveData.DefaultInit(firstSchedule);
    oreMgr = COreAmountMgr::Instance();
    oreMgr->InitRemainOreAmount(0xFFFFFFFF, 0xFFFFFFFF);
    oreMgr->InitTransferOre(0, 0);
    scheduleInit = true;
  }

  if (scheduleInit)
  {
    CHolyStoneSystemDataMgr::SaveStateData(&pSystem->m_SaveData);
  }

  pSystem->m_bScheduleCodePre = 0;
  pSystem->InitQuestCash_Other();
  return true;
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

void CHolyStoneSystem::ReleaseLastAttBuff()
{
  const unsigned int destroyerGuildSerial = GetDestroyerGuildSerial();
  if (destroyerGuildSerial != static_cast<unsigned int>(-1))
  {
    for (int j = 0; j < MAX_PLAYER; ++j)
    {
      if (g_Player[j].m_bLive && g_Player[j].m_Param.m_pGuild)
      {
        const unsigned int guildSerial = CPlayerDB::GetGuildSerial(&g_Player[j].m_Param);
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

void CHolyStoneSystem::SetGoldBoxConsumable(bool bFlag)
{
  m_bConsumable = bFlag;
}

void CHolyStoneSystem::SetKeeperDestroyRace(unsigned __int8 byRace)
{
  m_byKeeperDestroyRace = byRace;
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

#pragma pack(push, 1)
  struct
  {
    char arrive;
    char race;
    char name[17];
    unsigned int destroyerSerial;
  } msg{};
#pragma pack(pop)

  msg.arrive = byArrive;
  msg.race = static_cast<char>(CPlayerDB::GetRaceCode(&m_pkDestroyer->m_Param));
  const char *charName = CPlayerDB::GetCharNameW(&m_pkDestroyer->m_Param);
  memcpy_0(msg.name, charName, 0x10u);
  msg.name[16] = 0;
  msg.destroyerSerial = m_SaveData.m_dwDestroyerSerial;

  unsigned __int8 type[2]{};
  type[0] = 25;
  type[1] = 17;

  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    if (g_Player[index].m_bLive)
    {
      g_Network.m_pProcess[0]->LoadSendMsg( index, type, reinterpret_cast<char *>(&msg), 0x17u);
    }
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

#pragma pack(push, 1)
  struct
  {
    unsigned short remainSec;
    unsigned __int8 moveInfo;
  } msg{};
#pragma pack(pop)

  msg.remainSec = remainSec;
  msg.moveInfo = byStoneMapMoveInfo;

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
        g_Network.m_pProcess[0]->LoadSendMsg( index, type, reinterpret_cast<char *>(&msg), 3u);
      }
    }
  }
  else
  {
    g_Network.m_pProcess[0]->LoadSendMsg( n, type, reinterpret_cast<char *>(&msg), 3u);
  }
}

void CHolyStoneSystem::SendMsg_NotifyHolyKeeperAttackTimeBeKeepKeeper(char bKeepKeeper)
{
  char msg[1]{};
  msg[0] = bKeepKeeper;

  unsigned __int8 type[2]{};
  type[0] = 25;
  type[1] = 27;

  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    if (g_Player[index].m_bLive)
    {
      g_Network.m_pProcess[0]->LoadSendMsg( index, type, msg, 1u);
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

#pragma pack(push, 1)
  struct
  {
    unsigned __int8 isAll;
    short totalSec;
    short remainSec1;
    short remainSec2;
    short passSec;
  } msg{};
#pragma pack(pop)

  msg.isAll = (n == static_cast<unsigned int>(-1));
  msg.totalSec = static_cast<short>((m_dwCheckTime[1] - m_dwCheckTime[0]) / 1000);
  msg.remainSec1 = static_cast<short>((delta - loopTime) / 1000);
  msg.remainSec2 = static_cast<short>((m_dwCheckTime[1] - loopTime) / 1000);
  msg.passSec = static_cast<short>((timeGetTime() - m_dwCheckTime[0]) / 1000);

  unsigned __int8 type[2]{};
  type[0] = 25;
  type[1] = 1;

  if (n == static_cast<unsigned int>(-1))
  {
    for (int index = 0; index < MAX_PLAYER; ++index)
    {
      if (g_Player[index].m_bLive)
      {
        g_Network.m_pProcess[0]->LoadSendMsg( index, type, reinterpret_cast<char *>(&msg), 9u);
      }
    }
  }
  else
  {
    g_Network.m_pProcess[0]->LoadSendMsg( n, type, reinterpret_cast<char *>(&msg), 9u);
  }
}

void CHolyStoneSystem::SendMsg_ExitStone()
{
  char msg[1]{};
  msg[0] = static_cast<char>(GetHolyMasterRace());

  unsigned __int8 type[2]{};
  type[0] = 25;
  type[1] = 2;

  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    if (g_Player[index].m_bLive)
    {
      g_Network.m_pProcess[0]->LoadSendMsg( index, type, msg, 1u);
    }
  }
}

void CHolyStoneSystem::SendMsg_StartBattle()
{
  char msg[1]{};

  unsigned __int8 type[2]{};
  type[0] = 25;
  type[1] = 18;

  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    if (g_Player[index].m_bLive)
    {
      g_Player[index].SetCntEnable(true);
      g_Network.m_pProcess[0]->LoadSendMsg( index, type, msg, 1u);
    }
  }
}

void CHolyStoneSystem::SendMsg_EndBattle(char byLoseRace)
{
  char msg[2]{};
  msg[0] = static_cast<char>(GetHolyMasterRace());
  msg[1] = byLoseRace;

  unsigned __int8 type[2]{};
  type[0] = 25;
  type[1] = 19;

  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    if (g_Player[index].m_bLive)
    {
      g_Player[index].SetCntEnable(false);
      g_Network.m_pProcess[0]->LoadSendMsg( index, type, msg, 2u);
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

#pragma pack(push, 1)
  struct
  {
    unsigned __int8 isAll;
    short totalSec;
    short remainSec1;
    short remainSec2;
    char masterRace;
    unsigned __int8 isChaos;
    unsigned __int8 isAttackable;
  } msg{};
#pragma pack(pop)

  msg.isAll = (n == static_cast<unsigned int>(-1));
  msg.totalSec = static_cast<short>((m_dwCheckTime[6] - m_dwCheckTime[2]) / 1000);
  msg.remainSec1 = static_cast<short>((delta - loopTime) / 1000);
  msg.remainSec2 = static_cast<short>((m_dwCheckTime[6] - loopTime) / 1000);
  msg.masterRace = static_cast<char>(GetHolyMasterRace());
  msg.isChaos = (GetSceneCode() == 6);
  msg.isAttackable = (GetSceneCode() == 3);

  unsigned __int8 type[2]{};
  type[0] = 25;
  type[1] = 3;

  if (n == static_cast<unsigned int>(-1))
  {
    for (int index = 0; index < MAX_PLAYER; ++index)
    {
      if (g_Player[index].m_bLive)
      {
        g_Network.m_pProcess[0]->LoadSendMsg( index, type, reinterpret_cast<char *>(&msg), 0xAu);
      }
    }
  }
  else
  {
    g_Network.m_pProcess[0]->LoadSendMsg( n, type, reinterpret_cast<char *>(&msg), 0xAu);
  }
}

void CHolyStoneSystem::SendMsg_WaitKeeper(unsigned int n, char byWaitType)
{
  char msg[2]{};
  msg[0] = byWaitType;
  msg[1] = static_cast<char>(GetHolyMasterRace());

  unsigned __int8 type[2]{};
  type[0] = 25;
  type[1] = 44;

  if (n == static_cast<unsigned int>(-1))
  {
    for (int index = 0; index < MAX_PLAYER; ++index)
    {
      if (g_Player[index].m_bLive)
      {
        g_Network.m_pProcess[0]->LoadSendMsg( index, type, msg, 2u);
      }
    }
  }
  else
  {
    g_Network.m_pProcess[0]->LoadSendMsg( n, type, msg, 2u);
  }
}

void CHolyStoneSystem::SendMsg_HolyKeeperStateChaos()
{
#pragma pack(push, 1)
  struct
  {
    unsigned int keeperSerial;
    unsigned __int8 masterRace;
  } msg{};
#pragma pack(pop)

  msg.keeperSerial = g_Keeper->m_dwObjSerial;
  msg.masterRace = static_cast<unsigned __int8>(GetHolyMasterRace());

  unsigned __int8 type[2]{};
  type[0] = 25;
  type[1] = 15;

  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    if (g_Player[index].m_bLive)
    {
      g_Network.m_pProcess[0]->LoadSendMsg( index, type, reinterpret_cast<char *>(&msg), 5u);
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
            const int memberRace = CPlayerDB::GetRaceCode(&member->m_Param);
            const int playerRace = CPlayerDB::GetRaceCode(&player->m_Param);
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
#pragma pack(push, 1)
  struct
  {
    unsigned __int16 hpRate[3];
  } msg{};
#pragma pack(pop)

  memset_0(&msg, 0, sizeof(msg));
  for (int j = 0; j < 3; ++j)
  {
    msg.hpRate[j] = static_cast<unsigned __int16>(g_Stone[j].CalcCurHPRate());
  }

  unsigned __int8 type[2]{};
  type[0] = 13;
  type[1] = 33;

  for (int k = 0; k < MAX_PLAYER; ++k)
  {
    CPlayer *player = &g_Player[k];
    if (player->m_bLive)
    {
      const unsigned int serial = CPlayerDB::GetCharSerial(&player->m_Param);
      const int race = CPlayerDB::GetRaceCode(&player->m_Param);
      if (CPvpUserAndGuildRankingSystem::Instance()->IsCurrentRaceBossGroup(race, serial))
      {
        g_Network.m_pProcess[0]->LoadSendMsg( player->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 6u);
      }
    }
  }
}

void CHolyStoneSystem::SendNotifyHolyStoneDestroyedToRaceBoss()
{
  char msg[1]{};

  unsigned __int8 type[2]{};
  type[0] = 13;
  type[1] = 34;

  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    CPlayer *player = &g_Player[j];
    if (player->m_bLive && player->m_bOper)
    {
      const unsigned int serial = CPlayerDB::GetCharSerial(&player->m_Param);
      const int race = CPlayerDB::GetRaceCode(&player->m_Param);
      if (CPvpUserAndGuildRankingSystem::Instance()->IsCurrentRaceBossGroup(race, serial))
      {
        g_Network.m_pProcess[0]->LoadSendMsg( player->m_ObjID.m_wIndex, type, msg, 1u);
      }
    }
  }
}

void CHolyStoneSystem::PeneltyFailRace(unsigned __int8 byFailRace)
{
  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    CPlayer *player = &g_Player[j];
    if (player->m_bLive && player->m_bOper && player->m_byHSKQuestCode != 100)
    {
      const int race = CPlayerDB::GetRaceCode(&player->m_Param);
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
          const double curPoint = CPlayerDB::GetPvPPoint(&player->m_Param);
          if (static_cast<double>(std::abs(alterPoint)) > curPoint)
          {
            alterPoint = static_cast<int>(-0.0 - CPlayerDB::GetPvPPoint(&player->m_Param));
          }
          player->AlterPvPPoint(static_cast<double>(alterPoint), holy_dec, 0xFFFFFFFF);
          player->SendMsg_RaceBattlePenelty(alterPoint, 0);
        }
        else
        {
          int alterPoint = m_nRaceBattlePoint[1][1];
          const double curPoint = CPlayerDB::GetPvPPoint(&player->m_Param);
          if (static_cast<double>(std::abs(alterPoint)) > curPoint)
          {
            alterPoint = static_cast<int>(-0.0 - CPlayerDB::GetPvPPoint(&player->m_Param));
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

      (void)raceBossSerial0;
      (void)raceBossSerial1;
      (void)raceBossSerial2;

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
