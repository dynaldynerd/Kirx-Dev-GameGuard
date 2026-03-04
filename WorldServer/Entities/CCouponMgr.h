#pragma once

#include "IdaCompat.h"
#include "CouponInfo.h"
#include "CMyTimer.h"
#include "StorageList.h"

struct _PCBANG_PLAY_TIME;

/* 1797 */
class  CCouponMgr
{
public:
  CCouponMgr();
  ~CCouponMgr();

  void Init(unsigned __int16 wIdx);
  void Loop(unsigned __int16 wIdx);
  bool IsEnableGetCoupon();
  void ReceivePrimiumCoupon(unsigned __int16 wIdx);
  char SetCheetContTime(unsigned __int16 wIdx, int nMin);
  void LoadData(unsigned int dwAccSerial, _PCBANG_PLAY_TIME *pkInfo);
  void LogOut(bool bForceClose);
  void InitCuponInfo();
  CouponInfo *GetCouponInfo(unsigned __int8 byCouponTime);
  void SendMsg_InPcBangTime(unsigned __int16 wIdx);
  void SendMsg_CouponEnsure(unsigned __int16 wIdx, unsigned __int8 byCouponTime);
  void SendMsg_RemainCouponInform(unsigned __int16 wIdx, unsigned __int8 byRemainCoupon);
  void SendMsg_CouponError(unsigned __int16 wIdx, unsigned __int8 byRet);
  void SendMsg_CouponLendResult(unsigned __int16 wIdx, _STORAGE_LIST::_db_con *pCoupon);

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

