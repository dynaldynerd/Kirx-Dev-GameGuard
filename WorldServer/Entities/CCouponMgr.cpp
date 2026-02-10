#include "pch.h"

#include "CCouponMgr.h"
#include "CNationSettingManager.h"
#include "CTSingleton.h"
#include "KorLocalTime.h"
#include "WorldServerUtil.h"

#include <cstdlib>
#include <ctime>

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

