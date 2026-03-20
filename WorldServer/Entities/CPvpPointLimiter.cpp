#include "pch.h"

#include "CPvpPointLimiter.h"

#include "CPlayer.h"
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

bool CPvpPointLimiter::Set(long double dOriginalPvpPoint, _PVPPOINT_LIMIT_DB_BASE *pkInfo)
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
    return false;
  }

  Update(updateTime, dOriginalPvpPoint, 0.0);
  return true;
}

bool CPvpPointLimiter::TakePvpPoint(long double *dPvpPoint, CPlayer *pkSelf, CPlayer *pkDest)
{
  if (!m_pkInfo)
  {
    return false;
  }

  if (m_pkInfo->dLimitPoint < 0.0)
  {
    m_pkInfo->dLimitPoint = 0.0;
  }

  if (m_pkInfo->dUsePoint < 0.0)
  {
    m_pkInfo->dUsePoint = 0.0;
  }

  if (m_pkInfo->bUseUp || m_pkInfo->dUsePoint >= m_pkInfo->dLimitPoint)
  {
    m_pkInfo->dUsePoint = m_pkInfo->dLimitPoint;
    *dPvpPoint = 0.0;
    m_pkInfo->bUseUp = true;

    if (!g_Main.IsReleaseServiceMode() && pkDest)
    {
      char buffer[1024]{};
      sprintf_s(
        buffer,
        sizeof(buffer),
        "Dest %s Limit : %.10f Use : %.10f Dest Limit Done!",
        pkSelf->m_Param.GetCharNameA(),
        static_cast<double>(m_pkInfo->dLimitPoint),
        static_cast<double>(m_pkInfo->dUsePoint));
      pkDest->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, buffer, 0xFFu, nullptr);
    }

    return false;
  }

  const double leftLimit = static_cast<double>(m_pkInfo->dLimitPoint - m_pkInfo->dUsePoint);
  if (*dPvpPoint > leftLimit)
  {
    *dPvpPoint = leftLimit;
  }

  m_pkInfo->dUsePoint += *dPvpPoint;
  if (m_pkInfo->dUsePoint >= m_pkInfo->dLimitPoint)
  {
    m_pkInfo->dUsePoint = m_pkInfo->dLimitPoint;
    m_pkInfo->bUseUp = true;
  }

  if (!g_Main.IsReleaseServiceMode() && pkSelf)
  {
    const char *limitState = m_pkInfo->bUseUp ? "Done" : "Left";
    char buffer[1024]{};
    sprintf_s(
      buffer,
      sizeof(buffer),
      "Limit : %.10f Use : %.10f, Limit %s, %s %.10f Take",
      static_cast<double>(m_pkInfo->dLimitPoint),
      static_cast<double>(m_pkInfo->dUsePoint),
      limitState,
      pkDest->m_Param.GetCharNameA(),
      static_cast<double>(*dPvpPoint));
    pkSelf->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, buffer, 0xFFu, nullptr);
  }

  if (!g_Main.IsReleaseServiceMode() && pkDest)
  {
    const char *limitState = m_pkInfo->bUseUp ? "Done" : "Left";
    char buffer[1024]{};
    sprintf_s(
      buffer,
      sizeof(buffer),
      "Src %s Limit : %.10f Use : %.10f, Limit %s, %.10f Take",
      pkSelf->m_Param.GetCharNameA(),
      static_cast<double>(m_pkInfo->dLimitPoint),
      static_cast<double>(m_pkInfo->dUsePoint),
      limitState,
      static_cast<double>(*dPvpPoint));
    pkDest->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, buffer, 0xFFu, nullptr);
  }

  return true;
}

void CPvpPointLimiter::CheatUpdate(long double dOriginalPvpPoint)
{
  __int64 now = 0;
  time_20(&now);
  Update(now, dOriginalPvpPoint, 0.0);
}

void CPvpPointLimiter::Clear(__int64 tUpdateTime, long double dOriginalPvpPoint, CPlayer *pkSelf)
{
  if (!m_pkInfo)
  {
    return;
  }

  m_pkInfo->tUpdatedate = tUpdateTime;
  m_pkInfo->bUseUp = false;
  m_pkInfo->byLimitRate = 3;
  m_pkInfo->dOriginalPoint = dOriginalPvpPoint;
  m_pkInfo->dLimitPoint = m_pkInfo->dOriginalPoint * 3.0 / 100.0;
  m_pkInfo->dUsePoint = 0.0;

  if (!g_Main.IsReleaseServiceMode() && pkSelf)
  {
    char buffer[1024]{};
    sprintf_s(
      buffer,
      sizeof(buffer),
      "Current : %.10f Limit : %.10f Rate : %d Init Limit!",
      static_cast<double>(m_pkInfo->dOriginalPoint),
      static_cast<double>(m_pkInfo->dLimitPoint),
      3);
    pkSelf->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, buffer, 0xFFu, nullptr);
  }
}

void CPvpPointLimiter::Update(__int64 tUpdateTime, long double dOriginalPvpPoint, double dUsePoint)
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
