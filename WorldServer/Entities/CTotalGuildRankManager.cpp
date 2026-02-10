#include "pch.h"

#include "CTotalGuildRankManager.h"
#include "WorldServerUtil.h"

#include "CPvpUserAndGuildRankingSystem.h"
#include "GlobalObjects.h"
#include "CRFWorldDatabase.h"

#include <cstring>

CTotalGuildRankManager *CTotalGuildRankManager::Instance()
{
  static CTotalGuildRankManager s_instance;
  return &s_instance;
}

bool CTotalGuildRankManager::Init()
{
  m_iOldDay = GetCurDay();
  return m_iOldDay != -1 && m_kInfo.Init();
}

bool CTotalGuildRankManager::Load()
{
  _total_guild_rank_info info{};
  const int result = static_cast<int>(Load(0, &info));
  if (result < 0)
  {
    return false;
  }
  if (!result)
  {
    return m_kInfo.Load(&info);
  }
  m_kInfo.SetNoDataFlag();
  return true;
}

__int64 CTotalGuildRankManager::Load(unsigned __int8 byDayAfter, _total_guild_rank_info *pkInfo)
{
  char dateStr[160]{};
  GetDateStrAfterDay(dateStr, 128, byDayAfter);

  char tableName[260]{};
  sprintf_s(tableName, 0x100u, "tbl_GuildRank%s", dateStr);
  if (!g_Main.m_pWorldDB->TableExist(tableName))
  {
    return 1;
  }

  memset_0(pkInfo, 0, sizeof(_total_guild_rank_info));
  const unsigned __int8 result = g_Main.m_pWorldDB->Select_TotalGuildRank(dateStr, pkInfo);
  if (result == 2)
  {
    return 1;
  }
  if (result == 1)
  {
    return -1;
  }

  OrderRank(pkInfo);
  return 0;
}

void CTotalGuildRankManager::Loop()
{
  if (IsDayChanged(&m_iOldDay) && !g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x2Bu, nullptr, 0))
  {
    CPvpUserAndGuildRankingSystem::Instance()->Log(
      "CTotalGuildRankManager::Loop() g_Main.PushDQSData(..) Fail!");
  }
}

void CTotalGuildRankManager::Send(unsigned int dwVer, unsigned __int8 byTabRace, CPlayer *pkPlayer)
{
  unsigned int guildSerial = static_cast<unsigned int>(-1);
  if (pkPlayer->m_Param.m_pGuild)
  {
    guildSerial = pkPlayer->m_Param.m_pGuild->m_dwSerial;
  }
  const unsigned __int8 selfRace = static_cast<unsigned __int8>(pkPlayer->m_Param.GetRaceCode());
  m_kInfo.Send(dwVer, pkPlayer->m_ObjID.m_wIndex, byTabRace, selfRace, guildSerial);
}

void CTotalGuildRankManager::OrderRank(_total_guild_rank_info *pkInfo)
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
      if (pkInfo->list[prevIndex].dPowerPoint != pkInfo->list[index].dPowerPoint)
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

bool CTotalGuildRankManager::Update(_total_guild_rank_info *pLoadData)
{
  return Load(0, pLoadData) == 0;
}

void CTotalGuildRankManager::UpdateComlete(unsigned __int8 byRet, _total_guild_rank_info *pLoadData)
{
  if (byRet || !m_kInfo.Update(pLoadData))
  {
    CPvpUserAndGuildRankingSystem::Instance()->Log(
      "CTotalGuildRankManager::Update() m_kInfo.Update() Fail!");
  }
  else
  {
    CPvpUserAndGuildRankingSystem::Instance()->Log(
      "CTotalGuildRankManager::Update() m_kInfo.Update() Success!");
  }
}
