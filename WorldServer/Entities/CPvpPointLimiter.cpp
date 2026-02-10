#include "pch.h"

#include "CPvpPointLimiter.h"

#include "CMainThread.h"
#include "WorldServerUtil.h"

#include <ctime>

CPvpPointLimiter::CPvpPointLimiter()
  : m_pkInfo(nullptr)
{
}

CPvpPointLimiter::~CPvpPointLimiter()
{
}

char CPvpPointLimiter::Set(long double dOriginalPvpPoint, _PVPPOINT_LIMIT_DB_BASE *pkInfo)
{
  tm lastTime{};
  if (tm *localLast = localtime_2(&pkInfo->tUpdatedate))
  {
    std::memcpy(&lastTime, localLast, sizeof(lastTime));
  }

  __int64 updateTime = 0;
  time_20(&updateTime);
  tm *localNow = localtime_2(&updateTime);

  m_pkInfo = pkInfo;
  if (pkInfo->byLimitRate == 3 && localNow && lastTime.tm_year == localNow->tm_year && lastTime.tm_mon == localNow->tm_mon
      && lastTime.tm_mday == localNow->tm_mday)
  {
    return 0;
  }

  Update(updateTime, dOriginalPvpPoint, 0.0, 0);
  return 1;
}

void CPvpPointLimiter::CheatUpdate(long double dOriginalPvpPoint)
{
  __int64 now = 0;
  time_20(&now);
  Update(now, dOriginalPvpPoint, 0.0, 0);
}

void CPvpPointLimiter::Update(__int64 tUpdateTime, long double dOriginalPvpPoint, double dUsePoint, int /*unused*/)
{
  if (!m_pkInfo)
  {
    return;
  }

  double usePoint = dUsePoint;
  if (usePoint < 0.0)
  {
    usePoint = 0.0;
  }

  m_pkInfo->tUpdatedate = tUpdateTime;
  m_pkInfo->byLimitRate = 3;
  m_pkInfo->dOriginalPoint = dOriginalPvpPoint;
  m_pkInfo->dLimitPoint = m_pkInfo->dOriginalPoint * 3.0 / 100.0;
  if (usePoint <= m_pkInfo->dLimitPoint)
  {
    m_pkInfo->dUsePoint = usePoint;
  }
  else
  {
    m_pkInfo->dUsePoint = m_pkInfo->dLimitPoint;
  }
  m_pkInfo->bUseUp = m_pkInfo->dUsePoint >= m_pkInfo->dLimitPoint;
}
