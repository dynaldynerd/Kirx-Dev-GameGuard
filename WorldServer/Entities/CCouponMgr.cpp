#include "pch.h"

#include "CCouponMgr.h"
#include "CNationSettingManager.h"
#include "CTSingleton.h"
#include "KorLocalTime.h"

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

