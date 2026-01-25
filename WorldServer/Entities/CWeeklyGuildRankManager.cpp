#include "pch.h"

#include "CWeeklyGuildRankManager.h"

#include <ctime>

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
  std::time_t now = std::time(nullptr);
  std::tm local{};
  if (localtime_s(&local, &now) != 0)
  {
    return false;
  }

  const int dayOfWeek = local.tm_wday + 1; // 1 = Sunday
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

  std::tm target = local;
  target.tm_mday += addDays;
  target.tm_hour = 0;
  target.tm_min = 0;
  target.tm_sec = 0;
  const std::time_t next = std::mktime(&target);
  if (next == static_cast<std::time_t>(-1))
  {
    return false;
  }
  m_tNextSetOwnerTime = static_cast<long long>(next);
  return true;
}

void CWeeklyGuildRankManager::SetNextRankDate()
{
  std::time_t now = std::time(nullptr);
  std::tm local{};
  if (localtime_s(&local, &now) != 0)
  {
    m_tNextUpdateTime = 0;
    return;
  }

  std::tm target = local;
  target.tm_mday += 1;
  target.tm_hour = 0;
  target.tm_min = 0;
  target.tm_sec = 0;
  const std::time_t next = std::mktime(&target);
  if (next == static_cast<std::time_t>(-1))
  {
    m_tNextUpdateTime = 0;
    return;
  }
  m_tNextUpdateTime = static_cast<long long>(next);
}
