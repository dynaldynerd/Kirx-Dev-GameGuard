#include "pch.h"

#include "CCouponMgr.h"
#include "CNationSettingManager.h"
#include "CTSingleton.h"
#include "KorLocalTime.h"
#include "WorldServerUtil.h"

#include <cstdlib>
#include <ctime>

void CCouponMgr::Init(unsigned __int16 wIdx)
{
  (void)wIdx;

  m_tmrCheckConnMin.BeginTimer(0xEA60u);
  m_tmrCouponEnableTime.BeginTimer(0x36EE80u);
  if (m_byRemainTime > 5u)
  {
    m_byRemainTime = 5;
  }
  m_bTimeReset = false;
}

void CCouponMgr::ReceivePrimiumCoupon(unsigned __int16 wIdx)
{
  (void)wIdx;

  if (!m_byRemainTime || m_byReceiveCoupon >= 5u)
  {
    return;
  }

  unsigned __int8 toReceive = m_byRemainTime;
  if (toReceive + m_byReceiveCoupon > 5u)
  {
    toReceive = static_cast<unsigned __int8>(5u - m_byReceiveCoupon);
  }

  m_byReceiveCoupon = static_cast<unsigned __int8>(m_byReceiveCoupon + toReceive);
  m_byRemainTime = 0;

  if (m_pkInfo)
  {
    m_pkInfo->byReceiveCoupon = m_byReceiveCoupon;
    m_pkInfo->byEnsureTime = m_byRemainTime;
  }
}

char CCouponMgr::SetCheetContTime(unsigned __int16 wIdx, int nMin)
{
  (void)wIdx;

  if (nMin)
  {
    const unsigned __int8 prevHour = static_cast<unsigned __int8>(m_dwContTime / 60);
    m_dwContTime += static_cast<unsigned int>(nMin);
    if (m_dwContTime > 300)
    {
      m_dwContTime = 300;
    }

    const unsigned __int8 newHour = static_cast<unsigned __int8>(m_dwContTime / 60);
    m_tmrCheckConnMin.TermTimeRun();
    m_tmrCouponEnableTime.TermTimeRun();
    m_tmrCouponEnableTime.BeginTimer(0x36EE80u);

    if (prevHour != newHour)
    {
      m_byRemainTime = static_cast<unsigned __int8>(m_byRemainTime + (newHour - prevHour));
    }
    if (m_byRemainTime + m_byReceiveCoupon > 5u)
    {
      m_byRemainTime = static_cast<unsigned __int8>(5u - m_byReceiveCoupon);
    }
    if (m_byRemainTime > 5u)
    {
      m_byRemainTime = 5;
    }
  }
  else
  {
    m_dwContTime = 0;
    m_byReceiveCoupon = 0;
    m_byRemainTime = 0;
    m_tmrCouponEnableTime.TermTimeRun();
    m_tmrCheckConnMin.TermTimeRun();
  }

  if (m_pkInfo)
  {
    m_pkInfo->dwContPlayTime = m_dwContTime;
    m_pkInfo->byReceiveCoupon = m_byReceiveCoupon;
    m_pkInfo->byEnsureTime = m_byRemainTime;
  }

  return 1;
}

void CCouponMgr::LoadData(unsigned int dwAccSerial, _PCBANG_PLAY_TIME *pkInfo)
{
  CNationSettingManager *manager = CTSingleton<CNationSettingManager>::Instance();
  if (manager->GetNationCode() != 410)
  {
    return;
  }

  char buffer[48]{};
  char dateString[28]{};
  _strdate(buffer);
  dateString[0] = buffer[6];
  dateString[1] = buffer[7];
  dateString[2] = buffer[0];
  dateString[3] = buffer[1];
  dateString[4] = buffer[3];
  dateString[5] = buffer[4];
  dateString[6] = '\0';

  const unsigned int lastConnDate = pkInfo->dwLastConnTime / 0x2710;
  const int today = std::atoi(dateString);
  const unsigned int connectTimeLimit = GetConnectTime_AddBySec(-1800);

  if (lastConnDate == static_cast<unsigned int>(today) && lastConnDate)
  {
    if (pkInfo->dwLastConnTime <= connectTimeLimit || pkInfo->dwContPlayTime <= 0xA)
    {
      m_byReceiveCoupon = pkInfo->byReceiveCoupon;
      m_byRemainTime = 0;
      m_dwContTime = 0;
      m_pkInfo = pkInfo;
      m_pkInfo->bForcedClose = 0;
      m_pkInfo->dwContPlayTime = 0;
      m_pkInfo->byEnsureTime = m_byRemainTime;
    }
    else
    {
      m_dwContTime = pkInfo->dwContPlayTime;
      m_byReceiveCoupon = pkInfo->byReceiveCoupon;
      m_byRemainTime = pkInfo->byEnsureTime;
      if (m_byRemainTime > 5u)
      {
        m_byRemainTime = 5;
      }
      m_pkInfo = pkInfo;
    }
  }
  else
  {
    m_pkInfo = pkInfo;
    m_pkInfo->bForcedClose = 0;
    m_byReceiveCoupon = 0;
    m_pkInfo->byReceiveCoupon = 0;
    m_dwContTime = 0;
    m_pkInfo->dwContPlayTime = 0;
    m_byRemainTime = 0;
    m_pkInfo->byEnsureTime = 0;
  }

  (void)dwAccSerial;
}

void CCouponMgr::LogOut(bool bForceClose)
{
  CNationSettingManager *manager = CTSingleton<CNationSettingManager>::Instance();
  if (manager->GetNationCode() == 410)
  {
    if (this->m_pkInfo)
    {
      this->m_pkInfo->bForcedClose = bForceClose;
      this->m_pkInfo->dwContPlayTime = this->m_dwContTime;
      this->m_pkInfo->dwLastConnTime = GetKorLocalTime();
      this->m_pkInfo->byReceiveCoupon = this->m_byReceiveCoupon;
      this->m_pkInfo->byEnsureTime = this->m_byRemainTime;
    }
  }
}

