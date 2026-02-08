#pragma once

#include "IdaCompat.h"

/* 1797 */
class __cppobj CCouponMgr
{
public:
  void LogOut(bool bForceClose);

private:
  CouponInfo m_Coupon[5];
  unsigned __int8 m_byRemainTime;
  unsigned __int8 m_byReceiveCoupon;
  unsigned int m_dwContTime;
  unsigned __int8 m_byInitTime;
  bool m_bTimeReset;
  CMyTimer m_tmrCheckConnMin;
  CMyTimer m_tmrCouponEnableTime;
  _PCBANG_PLAY_TIME *m_pkInfo;
};

