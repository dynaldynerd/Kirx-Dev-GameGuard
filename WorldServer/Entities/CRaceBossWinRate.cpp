#include "pch.h"

#include "CRaceBossWinRate.h"

#include "CNetworkEX.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CRFWorldDatabase.h"
#include "notify_race_boss_winrate_zocl.h"
#include "raceboss_acc_winrate.h"
#include "GlobalObjects.h"
#include "DqsDbStructs.h"
#include "WorldServerUtil.h"

CRaceBossWinRate *CRaceBossWinRate::m_Inst;

CRaceBossWinRate::CRaceBossWinRate()
{
  m_byTotalBattleCnt = 0;
  std::memset(m_byWinCnt, 0, sizeof(m_byWinCnt));
  std::memset(m_dwAccBattleCnt, 0, sizeof(m_dwAccBattleCnt));
  std::memset(m_dwAccWinCnt, 0, sizeof(m_dwAccWinCnt));
}

CRaceBossWinRate::~CRaceBossWinRate()
{
  // this is not a stub
}

CRaceBossWinRate *CRaceBossWinRate::Instance()
{
  if (!m_Inst)
  {
    m_Inst = new CRaceBossWinRate();
  }
  return m_Inst;
}

void CRaceBossWinRate::Destroy()
{
  if (m_Inst)
  {
    delete m_Inst;
    m_Inst = nullptr;
  }
}

void CRaceBossWinRate::UpdateTotalCnt()
{
  ++m_byTotalBattleCnt;
  Notify();
}

void CRaceBossWinRate::UpdateWinCnt(unsigned __int8 byRace)
{
  ++m_byWinCnt[byRace];
  ++m_byTotalBattleCnt;
  Notify();
}

void CRaceBossWinRate::UpdateRaceBossWinRate(unsigned __int8 byRace)
{
  m_byWinCnt[byRace] = 0;
}

void CRaceBossWinRate::UpdateRaceBossWinRate()
{
  for (int j = 0; j < 3; ++j)
  {
    m_byWinCnt[j] = 0;
  }
  m_byTotalBattleCnt = 0;
  Notify();
}

char CRaceBossWinRate::LoadINI()
{
  m_byTotalBattleCnt = static_cast<unsigned __int8>(
    GetPrivateProfileIntA("RaceBossWinRate", "TotalBattleCnt", 0, "..\\SystemSave\\ServerState.ini"));
  m_byWinCnt[0] = static_cast<unsigned __int8>(
    GetPrivateProfileIntA("RaceBossWinRate", "B_WinCnt", 0, "..\\SystemSave\\ServerState.ini"));
  m_byWinCnt[1] = static_cast<unsigned __int8>(
    GetPrivateProfileIntA("RaceBossWinRate", "C_WinCnt", 0, "..\\SystemSave\\ServerState.ini"));
  m_byWinCnt[2] = static_cast<unsigned __int8>(
    GetPrivateProfileIntA("RaceBossWinRate", "A_WinCnt", 0, "..\\SystemSave\\ServerState.ini"));
  return 1;
}

char CRaceBossWinRate::SaveINI()
{
  char buffer[272]{};
  std::memset(buffer, 0, 255u);

  sprintf(buffer, "%d", m_byTotalBattleCnt);
  if (!WritePrivateProfileStringA("RaceBossWinRate", "TotalBattleCnt", buffer, "..\\SystemSave\\ServerState.ini"))
  {
    g_Main.m_logSystemError.Write(
      "CRaceBuffOldHistoryInfo::SaveINI() : WritePrivateProfileString( %s, %s, %s, %s )",
      "RaceBossWinRate",
      "TotalBattleCnt",
      buffer,
      "..\\SystemSave\\ServerState.ini");
    return 0;
  }

  sprintf(buffer, "%d", m_byWinCnt[0]);
  if (!WritePrivateProfileStringA("RaceBossWinRate", "B_WinCnt", buffer, "..\\SystemSave\\ServerState.ini"))
  {
    g_Main.m_logSystemError.Write(
      "CRaceBuffOldHistoryInfo::SaveINI() : WritePrivateProfileString( %s, %s, %s, %s )",
      "RaceBossWinRate",
      "B_WinCnt",
      buffer,
      "..\\SystemSave\\ServerState.ini");
    return 0;
  }

  sprintf(buffer, "%d", m_byWinCnt[1]);
  if (!WritePrivateProfileStringA("RaceBossWinRate", "C_WinCnt", buffer, "..\\SystemSave\\ServerState.ini"))
  {
    g_Main.m_logSystemError.Write(
      "CRaceBuffOldHistoryInfo::SaveINI() : WritePrivateProfileString( %s, %s, %s, %s )",
      "RaceBossWinRate",
      "C_WinCnt",
      buffer,
      "..\\SystemSave\\ServerState.ini");
    return 0;
  }

  sprintf(buffer, "%d", m_byWinCnt[2]);
  if (!WritePrivateProfileStringA("RaceBossWinRate", "A_WinCnt", buffer, "..\\SystemSave\\ServerState.ini"))
  {
    g_Main.m_logSystemError.Write(
      "CRaceBuffOldHistoryInfo::SaveINI() : WritePrivateProfileString( %s, %s, %s, %s )",
      "RaceBossWinRate",
      "A_WinCnt",
      buffer,
      "..\\SystemSave\\ServerState.ini");
    return 0;
  }

  return 1;
}

char CRaceBossWinRate::LoadBossCurrentWinRate()
{
  char outDay[48]{};
  _SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  int subDay = 0;
  if (systemTime.wDayOfWeek <= 1u)
  {
    if (systemTime.wDayOfWeek == 0)
    {
      subDay = 6;
    }
  }
  else
  {
    subDay = systemTime.wDayOfWeek - 1;
  }

  GetSubDayStr(subDay, outDay);

  for (int j = 0; j < 3; ++j)
  {
    CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
    const unsigned int bossSerial = ranking->GetCurrentRaceBossSerial(static_cast<unsigned __int8>(j), 0);
    if (bossSerial != static_cast<unsigned int>(-1))
    {
      unsigned int totalCnt = 0;
      unsigned int winCnt = 0;
      const unsigned __int8 result =
        g_Main.m_pWorldDB->Select_RaceBossCurrentWinRate(j, outDay, &totalCnt, &winCnt);
      if (result == 1)
      {
        return 0;
      }

      m_byTotalBattleCnt = static_cast<unsigned __int8>(totalCnt);
      m_byWinCnt[j] = static_cast<unsigned __int8>(winCnt);
    }
  }

  return 1;
}

bool CRaceBossWinRate::LoadDB()
{
  return LoadBossCurrentWinRate() && LoadBossAccmulationWinRate(nullptr) == 0;
}

unsigned __int8 CRaceBossWinRate::LoadBossAccmulationWinRate(_qry_case_raceboss_accumulation_winrate *pData)
{
  const bool noData = pData == nullptr;
  unsigned int bossSerial = static_cast<unsigned int>(-1);

  for (int race = 0; race < 3; ++race)
  {
    CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
    bossSerial = ranking->GetCurrentRaceBossSerial(static_cast<unsigned __int8>(race), 0);
    if (bossSerial != static_cast<unsigned int>(-1))
    {
      if (noData)
      {
        m_dwAccBattleCnt[race] = 0;
        m_dwAccWinCnt[race] = 0;
      }
      else
      {
        pData->dwTotalCnt[race] = 0;
        pData->dwWinCnt[race] = 0;
      }

      _raceboss_acc_winrate winData{};
      const unsigned __int8 result =
        g_Main.m_pWorldDB->Select_RaceBossAccumulationWinRate(race, bossSerial, &winData);
      if (result == 1)
      {
        g_Main.m_logSystemError.Write(
          "CRaceBossWinRate::LoadBossAccmulationWinRate() : Select_RaceBossAccumulationWinRate( %d, %d ) Boss Accumulation Win Rate Load Error",
          race,
          bossSerial);
      }
      else if (noData)
      {
        m_dwAccBattleCnt[race] = winData.dwTotalCnt;
        m_dwAccWinCnt[race] = winData.dwWinCnt;
      }
      else
      {
        pData->dwTotalCnt[race] = winData.dwTotalCnt;
        pData->dwWinCnt[race] = winData.dwWinCnt;
      }
    }
  }

  return 0;
}

void CRaceBossWinRate::CompleteBossAccmulationWinRate(_qry_case_raceboss_accumulation_winrate *pData)
{
  if (!pData)
  {
    return;
  }

  for (int race = 0; race < 3; ++race)
  {
    m_dwAccBattleCnt[race] = pData->dwTotalCnt[race];
    m_dwAccWinCnt[race] = pData->dwWinCnt[race];
  }
  UpdateRaceBossWinRate();
}

void CRaceBossWinRate::Notify()
{
  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    CPlayer *player = &g_Player[index];
    if (player->m_bOper && player->m_bLive)
    {
      const unsigned __int8 race = static_cast<unsigned __int8>(player->m_Param.GetRaceCode());
      unsigned __int8 type[2]{};
      type[0] = 13;
      type[1] = 123;

      _notify_race_boss_winrate_zocl msg{};
      msg.byTotalCnt = m_byTotalBattleCnt;
      msg.byCurWinCnt = m_byWinCnt[race];
      msg.dwAccTotalCnt = m_dwAccBattleCnt[race];
      msg.dwAccWinCnt = m_dwAccWinCnt[race];

      const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
      g_Network.m_pProcess[0]->LoadSendMsg(index, type, reinterpret_cast<char *>(&msg), len);
    }
  }
}
