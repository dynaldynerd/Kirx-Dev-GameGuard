#include "pch.h"

#include "CWeeklyGuildRankManager.h"

#include "CRFWorldDatabase.h"
#include "CPlayer.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CGuildBattleController.h"
#include "CLogTypeDBTaskManager.h"
#include "CUnmannedTraderTaxRateManager.h"
#include "AutoMineMachineMng.h"
#include "CNotifyNotifyRaceLeaderSownerUTaxrate.h"
#include "GlobalObjects.h"

#include <atlcomtime.h>
#include <ctime>
#include <cstring>

CWeeklyGuildRankManager *CWeeklyGuildRankManager::Instance()
{
  static CWeeklyGuildRankManager s_instance;
  return &s_instance;
}

bool CWeeklyGuildRankManager::Init()
{
  if (!m_kInfo.Init())
  {
    return false;
  }
  if (!InitNextSetOwnerDate())
  {
    return false;
  }
  SetNextRankDate();
  return true;
}

bool CWeeklyGuildRankManager::InitNextSetOwnerDate()
{
  ATL::CTime now = ATL::CTime::GetTickCount();
  const int dayOfWeek = now.GetDayOfWeek();
  int addDays = 0;
  if (dayOfWeek == 1)
  {
    addDays = 1;
  }
  else
  {
    if (dayOfWeek > 7)
    {
      return false;
    }
    addDays = 7 - dayOfWeek + 2;
  }

  ATL::CTimeSpan span(addDays, 0, 0, 0);
  now += span;
  const int day = now.GetDay();
  const int month = now.GetMonth();
  const int year = now.GetYear();
  ATL::CTime target(year, month, day, 0, 0, 0, -1);
  m_tNextSetOwnerTime = target.GetTime();
  return true;
}

void CWeeklyGuildRankManager::SetNextRankDate()
{
  ATL::CTimeSpan oneDay(1, 0, 0, 0);
  ATL::CTime now = ATL::CTime::GetTickCount();
  ATL::CTime next = now + oneDay;
  const int day = next.GetDay();
  const int month = next.GetMonth();
  const int year = next.GetYear();
  ATL::CTime target(year, month, day, 0, 0, 0, -1);
  m_tNextUpdateTime = target.GetTime();
}

bool CWeeklyGuildRankManager::InsertSettlementOwner(CRFWorldDatabase *pkWorldDB, char *pData)
{
  bool success = true;
  for (int j = 0; j < 3; ++j)
  {
    for (int k = 0; k < 2; ++k)
    {
      const int index = k + 2 * j;
      if (*reinterpret_cast<unsigned int *>(&pData[56 * index]))
      {
        if (!pkWorldDB->Insert_SettlementOwnerLog(
              static_cast<unsigned __int8>(k),
              static_cast<unsigned __int8>(pData[56 * index + 21]),
              *reinterpret_cast<unsigned int *>(&pData[56 * index]),
              &pData[56 * index + 4],
              *reinterpret_cast<unsigned short *>(&pData[56 * index + 22]),
              static_cast<unsigned __int8>(pData[56 * index + 24]),
              *reinterpret_cast<long double *>(&pData[56 * index + 32]),
              *reinterpret_cast<long double *>(&pData[56 * index + 40]),
              *reinterpret_cast<unsigned int *>(&pData[56 * index + 48])))
        {
          success = false;
        }
      }
    }
  }
  return success;
}

void CWeeklyGuildRankManager::Loop()
{
  std::time_t now = std::time(nullptr);
  if (now >= m_tNextUpdateTime)
  {
    SetNextRankDate();
    if (!g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x2Cu, nullptr, 0))
    {
      CPvpUserAndGuildRankingSystem::Instance()->Log(
        "CWeeklyGuildRankManager::Loop() g_Main.PushDQSData(..) Fail!");
    }
  }
}

void CWeeklyGuildRankManager::GetTodayRankDate(char *szDate, int iBuffSize)
{
  if (!szDate)
  {
    return;
  }

  ATL::CTime now = ATL::CTime::GetTickCount();
  const int day = now.GetDay();
  const int month = now.GetMonth();
  const int year = now.GetYear();
  sprintf_s(szDate, iBuffSize, "%04d%02d%02d", year, month, day);
}

__int64 CWeeklyGuildRankManager::UpdateTodayTable(char *szDate, _pvppoint_guild_rank_info *pkInfo)
{
  char created = 0;
  char tableName[260]{};
  sprintf_s(tableName, 0xFFu, "tbl_PvpPointGuildRank%s", szDate);
  if (!g_Main.m_pWorldDB->TableExist(tableName))
  {
    if (!CreatePvpPointGuildRank(szDate))
    {
      return -1;
    }
    created = 1;
  }

  memset_0(pkInfo, 0, sizeof(_pvppoint_guild_rank_info));
  const unsigned __int8 result = g_Main.m_pWorldDB->Select_PvpPointGuildRank(szDate, pkInfo);
  if (result == 2)
  {
    return 2;
  }
  if (result == 1)
  {
    return -2;
  }
  return created != 0;
}

void CWeeklyGuildRankManager::OrderRank(_pvppoint_guild_rank_info *pkInfo)
{
  if (!pkInfo)
  {
    return;
  }

  unsigned __int16 rank = 1;
  unsigned __int16 offset = 0;
  pkInfo->list[0].wRank = 1;

  for (unsigned __int16 index = 1; index < pkInfo->wCount; ++index)
  {
    const unsigned __int16 prevIndex = static_cast<unsigned __int16>(index - 1);
    if (pkInfo->list[prevIndex].byRace == pkInfo->list[index].byRace)
    {
      if (pkInfo->list[prevIndex].dSumPvpPoint != pkInfo->list[index].dSumPvpPoint)
      {
        rank = index + 1 - offset;
      }
    }
    else
    {
      rank = 1;
      offset += pkInfo->wRaceCnt[pkInfo->list[prevIndex].byRace];
    }
    pkInfo->list[index].wRank = rank;
  }
}

char CWeeklyGuildRankManager::UpdateRankDBRecord(char *szDate, _pvppoint_guild_rank_info *pkInfo)
{
  if (!szDate || !pkInfo)
  {
    return 0;
  }

  for (unsigned __int16 index = 0; index < pkInfo->wCount; ++index)
  {
    if (!g_Main.m_pWorldDB->Update_PvpPointGuildRankRecord(
          szDate,
          pkInfo->list[index].dwSerial,
          pkInfo->list[index].wRank))
    {
      g_Main.m_logDQS.Write(
        "CWeeklyGuildRankManager::UpdateRankDBRecord( %s, ... ) : g_Main.m_pWorldDB->Update_WeeklyGuildRankRecord( %s, %u, %u ) Fail!",
        szDate,
        szDate,
        pkInfo->list[index].dwSerial,
        pkInfo->list[index].wRank);
      return 0;
    }
  }

  return 1;
}

char CWeeklyGuildRankManager::UpdateOwnerGuild(char *szDate)
{
  char ok = 1;
  for (unsigned __int8 race = 0; race < 3; ++race)
  {
    if (!g_Main.m_pWorldDB->Update_PvpPointGuildRankSumLv(
          szDate,
          race,
          2u,
          SETTLEMENT_AREA_MANAGE_OWNER_LIMIT_GRADE))
    {
      ok = 0;
    }
  }
  return ok;
}

char CWeeklyGuildRankManager::SelectOwnerGuild(char *szDate, _weeklyguildrank_owner_info *pkInfo)
{
  memset_0(pkInfo, 0, sizeof(_weeklyguildrank_owner_info));
  char ok = 1;
  for (unsigned __int8 race = 0; race < 3; ++race)
  {
    if (g_Main.m_pWorldDB->Select_WeeklyGuildRankOwnerGuild(szDate, race, 2u, pkInfo) == 1)
    {
      ok = 0;
    }
  }
  return ok;
}

void CWeeklyGuildRankManager::SetSettlementAreaManageOwnerGuild()
{
  for (unsigned __int8 race = 0; race < 3; ++race)
  {
    CGuild *owner = m_kInfo.GetCurOwnerGuild(race, 0);
    CUnmannedTraderTaxRateManager::Instance()->ChangeOwner(race, owner);
    AutoMineMachineMng::Instance()->ChangeOwner(race, m_kInfo.GetCurOwnerGuild(race, 0), 0);
    AutoMineMachineMng::Instance()->ChangeOwner(race, m_kInfo.GetCurOwnerGuild(race, 1), 1);
  }
  g_Main.m_kEtcNotifyInfo.Notify(0xFFu);
}

bool CWeeklyGuildRankManager::SaveINI()
{
  std::time_t now = std::time(nullptr);
  std::tm *tm = std::localtime(&now);
  if (!tm)
  {
    return false;
  }

  char buffer[272]{};
  char *year = _itoa(tm->tm_year + 1900, buffer, 10);
  if (!WritePrivateProfileStringA("PvpPointGuildRank", "Year", year, "..\\SystemSave\\ServerState.ini"))
  {
    return false;
  }

  char *month = _itoa(tm->tm_mon + 1, buffer, 10);
  if (!WritePrivateProfileStringA("PvpPointGuildRank", "Month", month, "..\\SystemSave\\ServerState.ini"))
  {
    return false;
  }

  char *day = _itoa(tm->tm_mday, buffer, 10);
  return WritePrivateProfileStringA("PvpPointGuildRank", "Day", day, "..\\SystemSave\\ServerState.ini") != 0;
}

void CWeeklyGuildRankManager::PushSettlementOwnerDBLog(char *pInfo)
{
  char data[0x150]{};
  for (int index = 0; index < 6; ++index)
  {
    if (*reinterpret_cast<unsigned int *>(pInfo + 56 * index + 16))
    {
      *reinterpret_cast<unsigned int *>(data + 56 * index) =
        *reinterpret_cast<unsigned int *>(pInfo + 56 * index + 16);
      strcpy_s(data + 56 * index + 4, 0x11u, pInfo + 56 * index + 20);
      data[56 * index + 37] = pInfo[56 * index + 37];
      *reinterpret_cast<unsigned __int16 *>(data + 56 * index + 38) =
        *reinterpret_cast<unsigned __int16 *>(pInfo + 56 * index + 38);
      data[56 * index + 40] = pInfo[56 * index + 40];
      *reinterpret_cast<unsigned long long *>(data + 56 * index + 44) =
        *reinterpret_cast<unsigned long long *>(pInfo + 56 * index + 48);
      *reinterpret_cast<unsigned long long *>(data + 56 * index + 52) =
        *reinterpret_cast<unsigned long long *>(pInfo + 56 * index + 56);
      *reinterpret_cast<unsigned int *>(data + 56 * index + 60) =
        *reinterpret_cast<unsigned int *>(pInfo + 56 * index + 64);
    }
  }

  CLogTypeDBTaskManager::Instance()->Push(0, data, 0x150u);
  CPvpUserAndGuildRankingSystem::Instance()->Log(
    "CWeeklyGuildRankManager::PushSettlementOwnerDBLog()!");
}

char CWeeklyGuildRankManager::UpdateTodayRank(_pvppoint_guild_rank_info *pLoadData)
{
  char date[264]{};
  GetTodayRankDate(date, 255);
  const int updated = static_cast<int>(UpdateTodayTable(date, pLoadData));
  if (updated < 0)
  {
    g_Main.m_logDQS.Write(
      "CWeeklyGuildRankManager::UpdateTodayRank() UpdateTodayTable(...) Fail!");
    return 0;
  }

  if (updated > 1)
  {
    g_Main.m_logDQS.Write(
      "CWeeklyGuildRankManager::UpdateTodayRank() UpdateTodayTable(...) No Data!");
    return 0;
  }

  OrderRank(pLoadData);
  if (UpdateRankDBRecord(date, pLoadData))
  {
    return 1;
  }

  g_Main.m_logDQS.Write(
    "CWeeklyGuildRankManager::UpdateTodayRank() : UpdateRankDBRecord( %s ) Fail!",
    date);
  return 0;
}

bool CWeeklyGuildRankManager::UpdateWeeklyOwner(_weeklyguildrank_owner_info *pLoadData)
{
  char date[260]{};
  GetTodayRankDate(date, 255);
  const bool updated = UpdateOwnerGuild(date) != 0;
  const bool selected = SelectOwnerGuild(date, pLoadData) != 0;
  return updated && selected;
}

void CWeeklyGuildRankManager::CompleteLoadeTodayRank(
  unsigned __int8 byRet,
  _pvppoint_guild_rank_info *pLoadData)
{
  if (!byRet)
  {
    if (!m_kInfo.Update(pLoadData))
    {
      CPvpUserAndGuildRankingSystem::Instance()->Log(
        "CWeeklyGuildRankManager::CompleteLoadeTodayRank() : m_kInfo.Update() Fail!");
    }
    CPvpUserAndGuildRankingSystem::Instance()->Log(
      "CWeeklyGuildRankManager::CompleteLoadeTodayRank() : m_kInfo.Update() Success!");
  }

  std::time_t now = std::time(nullptr);
  if (m_tNextSetOwnerTime <= now)
  {
    CGuildBattleController::Instance()->PushCreateGuildBattleRankTable();
    if (byRet || m_kInfo.IsNoDataToday())
    {
      m_kInfo.ClearOwner();
      InitNextSetOwnerDate();
      SetSettlementAreaManageOwnerGuild();
      SaveINI();
      g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x2Eu, nullptr, 0);
      CPvpUserAndGuildRankingSystem::Instance()->Log(
        "CWeeklyGuildRankManager::CompleteLoadeTodayRank() : ( RET_CODE_SUCCESS != byRet || m_kInfo.IsNoDataToday() ) Push Clear!");
      CGuildBattleController::Instance()->PushClearGuildBattleRank();
    }
    else
    {
      g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x2Du, nullptr, 0);
      CPvpUserAndGuildRankingSystem::Instance()->Log(
        "CWeeklyGuildRankManager::CompleteLoadeTodayRank() : Push Update Owner!");
    }
  }
}

void CWeeklyGuildRankManager::CompleteUpdateWeeklyOwner(
  unsigned __int8 byRet,
  _weeklyguildrank_owner_info *pLoadData)
{
  const char *result = byRet ? "Fail" : "Success";
  CPvpUserAndGuildRankingSystem::Instance()->Log(
    "CWeeklyGuildRankManager::CompleteUpdateWeeklyOwner() : Update Weekly Owner %s!",
    result);

  m_kInfo.UpdateOwner(pLoadData);
  InitNextSetOwnerDate();
  SetSettlementAreaManageOwnerGuild();
  SaveINI();
  PushSettlementOwnerDBLog(reinterpret_cast<char *>(pLoadData));
  CPvpUserAndGuildRankingSystem::Instance()->Log(
    "CWeeklyGuildRankManager::CompleteUpdateWeeklyOwner() : PushSettlementOwnerDBLog()!");

  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x2Eu, nullptr, 0);
  CPvpUserAndGuildRankingSystem::Instance()->Log(
    "CWeeklyGuildRankManager::CompleteUpdateWeeklyOwner() : Push Clear!");
  CGuildBattleController::Instance()->PushClearGuildBattleRank();
}

void CWeeklyGuildRankManager::CompleteUpdateClear(unsigned __int8 byRet)
{
  const char *result = byRet ? "Fail" : "Success";
  CPvpUserAndGuildRankingSystem::Instance()->Log(
    "CWeeklyGuildRankManager::CompleteUpdateClear() : Clear Weekly Guild PVPPoint Sum %s!",
    result);
}

void CWeeklyGuildRankManager::Send(unsigned int dwVer, unsigned __int8 byTabRace, CPlayer *pkPlayer)
{
  unsigned int guildSerial = static_cast<unsigned int>(-1);
  if (pkPlayer->m_Param.m_pGuild)
  {
    guildSerial = pkPlayer->m_Param.m_pGuild->m_dwSerial;
  }

  const unsigned __int8 bySelfRace = CPlayerDB::GetRaceCode(&pkPlayer->m_Param);
  const unsigned __int16 wIndex = pkPlayer->m_ObjID.m_wIndex;
  m_kInfo.Send(dwVer, wIndex, byTabRace, bySelfRace, guildSerial);
}

bool CWeeklyGuildRankManager::CreatePvpPointGuildRank(char *szDate)
{
  if (!szDate)
  {
    return false;
  }

  char buffer[1040]{};
  sprintf_s(buffer, 0x400u, "tbl_PvpPointGuildRank%s", szDate);
  if (g_Main.m_pWorldDB->TableExist(buffer))
  {
    return true;
  }

  if (g_Main.m_pWorldDB->Create_PvpPointGuildRankTable(szDate))
  {
    if (g_Main.m_pWorldDB->Insert_PvpPointGuildRankData(szDate))
    {
      return true;
    }

    g_Main.m_logDQS.Write(
      "CWeeklyGuildRankManager::CreatePvpPointGuildRank() : pkDB->Insert_WeeklyGuildPvpPointSum(%s)",
      szDate);
    return false;
  }

  g_Main.m_logDQS.Write(
    "CWeeklyGuildRankManager::CreatePvpPointGuildRank() : pkDB->Create_WeeklyGuildRankTable(%s)",
    szDate);
  return false;
}
