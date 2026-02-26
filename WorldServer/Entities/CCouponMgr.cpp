#include "pch.h"

#include "CCouponMgr.h"

#include "add_lend_item_result_zocl.h"
#include "CNetProcess.h"
#include "CNationSettingManager.h"
#include "CTSingleton.h"
#include "GlobalObjects.h"
#include "KorLocalTime.h"
#include "notify_cont_play_time_zocl.h"
#include "notify_coupon_ensure_time_zocl.h"
#include "notify_coupon_error_zocl.h"
#include "notify_remain_coupon_zocl.h"
#include "StorageList.h"
#include "TimeItem.h"
#include "WorldServerUtil.h"

#include <cstdlib>
#include <cstring>
#include <ctime>

CCouponMgr::CCouponMgr()
{
}

CCouponMgr::~CCouponMgr()
{
}

void CCouponMgr::Init(unsigned __int16 wIdx)
{
  CNationSettingManager *manager = CTSingleton<CNationSettingManager>::Instance();
  if (manager->GetNationCode() != 410)
  {
    return;
  }

  m_tmrCheckConnMin.BeginTimer(0xEA60u);
  const unsigned int minuteGap = m_dwContTime % 60u;
  m_tmrCouponEnableTime.BeginTimer(0x36EE80u);
  if (minuteGap)
  {
    m_tmrCouponEnableTime.CountingAddTickOld(60000u * minuteGap);
  }

  InitCuponInfo();
  SendMsg_InPcBangTime(wIdx);
  SendMsg_CouponEnsure(wIdx, m_byRemainTime);
  SendMsg_RemainCouponInform(wIdx, static_cast<unsigned __int8>(5u - m_byReceiveCoupon));
  m_bTimeReset = false;
}

void CCouponMgr::Loop(unsigned __int16 wIdx)
{
  CNationSettingManager *manager = CTSingleton<CNationSettingManager>::Instance();
  if (manager->GetNationCode() != 410)
  {
    return;
  }

  char timeBuffer[48]{};
  char hourMinute[28]{};
  _strtime(timeBuffer);
  hourMinute[0] = timeBuffer[0];
  hourMinute[1] = timeBuffer[1];
  hourMinute[2] = timeBuffer[3];
  hourMinute[3] = timeBuffer[4];
  hourMinute[4] = '\0';

  const int hhmm = std::atoi(hourMinute);
  if (!hhmm && !m_bTimeReset)
  {
    m_dwContTime = 0;
    m_byRemainTime = 0;
    m_byReceiveCoupon = 0;
    SendMsg_InPcBangTime(wIdx);
    SendMsg_CouponEnsure(wIdx, 0);
    SendMsg_RemainCouponInform(wIdx, 5u);
    m_tmrCouponEnableTime.TermTimeRun();
    m_tmrCheckConnMin.TermTimeRun();
    m_bTimeReset = true;
  }

  if (m_tmrCouponEnableTime.CountingTimer() && m_byRemainTime <= 5u - m_byReceiveCoupon && m_dwContTime <= 300u)
  {
    ++m_byRemainTime;
    const unsigned __int8 ensureTime =
      (m_byRemainTime + m_byReceiveCoupon <= 5u) ? m_byRemainTime : static_cast<unsigned __int8>(5u - m_byReceiveCoupon);
    SendMsg_CouponEnsure(wIdx, ensureTime);
    m_pkInfo->dwContPlayTime = m_dwContTime;
  }

  if (m_tmrCheckConnMin.CountingTimer() && m_dwContTime <= 300u)
  {
    ++m_dwContTime;
  }
}

void CCouponMgr::ReceivePrimiumCoupon(unsigned __int16 wIdx)
{
  CNationSettingManager *manager = CTSingleton<CNationSettingManager>::Instance();
  if (manager->GetNationCode() != 410)
  {
    return;
  }

  CPlayer *player = &g_Player[wIdx];
  unsigned __int8 grantedCouponCount = 0;

  if (!m_byRemainTime)
  {
    SendMsg_CouponError(wIdx, 1u);
    return;
  }

  if (m_byReceiveCoupon >= 5u)
  {
    SendMsg_CouponError(wIdx, 2u);
    return;
  }

  if (player->m_Param.m_dbInven.GetIndexEmptyCon() == 255)
  {
    SendMsg_CouponError(wIdx, 4u);
    return;
  }

  if (m_byRemainTime + m_byReceiveCoupon <= 5u)
  {
    grantedCouponCount = m_byRemainTime;
  }
  else
  {
    grantedCouponCount = static_cast<unsigned __int8>(5u - m_byReceiveCoupon);
  }

  CouponInfo *couponInfo = GetCouponInfo(grantedCouponCount);
  if (!couponInfo)
  {
    return;
  }

  _STORAGE_LIST::_db_con *loot = MakeLoot(couponInfo->byTableCode, static_cast<unsigned __int16>(couponInfo->dwIndex));
  if (!loot)
  {
    return;
  }

  _STORAGE_LIST::_db_con couponData;
  std::memcpy(&couponData, loot, sizeof(couponData));

  const _TimeItem_fld *timeRecord = TimeItem::FindTimeRec(couponInfo->byTableCode, static_cast<unsigned __int16>(couponInfo->dwIndex));
  if (timeRecord && timeRecord->m_nCheckType)
  {
    couponData.m_byCsMethod = static_cast<unsigned __int8>(timeRecord->m_nCheckType);
    __time32_t currentTime{};
    _time32(&currentTime);
    couponData.m_dwT = static_cast<unsigned int>(timeRecord->m_nUseTime + currentTime);
    couponData.m_dwDur = static_cast<unsigned int>(currentTime);
  }

  couponData.m_wSerial = player->m_Param.GetNewItemSerial();
  if (player->Emb_AddStorage(0, reinterpret_cast<_STORAGE_LIST::_storage_con *>(&couponData), false, true))
  {
    SendMsg_CouponLendResult(wIdx, &couponData);
    m_byRemainTime = 0;
    SendMsg_InPcBangTime(wIdx);
    m_byReceiveCoupon = static_cast<unsigned __int8>(m_byReceiveCoupon + grantedCouponCount);
    m_pkInfo->byReceiveCoupon = m_byReceiveCoupon;
    m_pkInfo->byEnsureTime = m_byRemainTime;

    if (m_byReceiveCoupon == 5u)
    {
      SendMsg_CouponError(wIdx, 3u);
    }
    else
    {
      SendMsg_RemainCouponInform(wIdx, static_cast<unsigned __int8>(5u - m_byReceiveCoupon));
    }
  }
  else
  {
    SendMsg_CouponError(wIdx, 5u);
  }
}

char CCouponMgr::SetCheetContTime(unsigned __int16 wIdx, int nMin)
{
  if (nMin)
  {
    const unsigned __int8 prevHour = static_cast<unsigned __int8>(m_dwContTime / 60u);
    m_dwContTime += nMin;
    const unsigned __int8 newHour = static_cast<unsigned __int8>(m_dwContTime / 60u);

    if (m_dwContTime > 300u)
    {
      m_dwContTime = 300;
    }

    m_tmrCheckConnMin.TermTimeRun();
    m_tmrCouponEnableTime.TermTimeRun();
    m_tmrCouponEnableTime.CountingAddTickOld(60000u * (m_dwContTime % 60u));

    if (prevHour != newHour)
    {
      m_byRemainTime = static_cast<unsigned __int8>(m_byRemainTime + newHour - prevHour);
    }

    if (m_byRemainTime + m_byReceiveCoupon > 5u)
    {
      m_byRemainTime = static_cast<unsigned __int8>(5u - m_byReceiveCoupon);
    }

    if (m_byRemainTime > 5u)
    {
      m_byRemainTime = 5;
    }

    SendMsg_InPcBangTime(wIdx);
    SendMsg_CouponEnsure(wIdx, m_byRemainTime);
    return 1;
  }

  m_dwContTime = 0;
  m_byReceiveCoupon = 0;
  m_byRemainTime = 0;
  m_tmrCouponEnableTime.TermTimeRun();
  m_tmrCheckConnMin.TermTimeRun();
  SendMsg_InPcBangTime(wIdx);
  SendMsg_CouponEnsure(wIdx, m_byRemainTime);
  SendMsg_RemainCouponInform(wIdx, static_cast<unsigned __int8>(5u - m_byReceiveCoupon));
  return 1;
}

void CCouponMgr::InitCuponInfo()
{
  for (int index = 0; index < 5; ++index)
  {
    _base_fld *record = g_Main.m_tblItemData[36].GetRecord(index);
    if (record)
    {
      m_Coupon[index].byTableCode = 36;
      m_Coupon[index].dwIndex = record->m_dwIndex;
    }
  }
}

CouponInfo *CCouponMgr::GetCouponInfo(unsigned __int8 byCouponTime)
{
  if (byCouponTime <= 5u)
  {
    return &m_Coupon[byCouponTime - 1];
  }
  return nullptr;
}

void CCouponMgr::SendMsg_InPcBangTime(unsigned __int16 wIdx)
{
  _notify_cont_play_time_zocl msg{};
  msg.byContTime = static_cast<unsigned __int8>(m_dwContTime / 60u);
  msg.byContMin = static_cast<unsigned __int8>(m_dwContTime % 60u);

  unsigned __int8 type[2]{59, 7};
  g_Network.m_pProcess[0]->LoadSendMsg(wIdx, type, reinterpret_cast<char *>(&msg), msg.size());
}

void CCouponMgr::SendMsg_CouponEnsure(unsigned __int16 wIdx, unsigned __int8 byCouponTime)
{
  _notify_coupon_ensure_time_zocl msg{};
  msg.byCouponTime = byCouponTime;

  unsigned __int8 type[2]{59, 6};
  g_Network.m_pProcess[0]->LoadSendMsg(wIdx, type, reinterpret_cast<char *>(&msg), msg.size());
}

void CCouponMgr::SendMsg_RemainCouponInform(unsigned __int16 wIdx, unsigned __int8 byRemainCoupon)
{
  _notify_remain_coupon_zocl msg{};
  msg.byRemainCoupon = byRemainCoupon;

  unsigned __int8 type[2]{59, 9};
  g_Network.m_pProcess[0]->LoadSendMsg(wIdx, type, reinterpret_cast<char *>(&msg), msg.size());
}

void CCouponMgr::SendMsg_CouponError(unsigned __int16 wIdx, unsigned __int8 byRet)
{
  _notify_coupon_error_zocl msg{};
  msg.byRetCode = byRet;

  unsigned __int8 type[2]{59, 8};
  g_Network.m_pProcess[0]->LoadSendMsg(wIdx, type, reinterpret_cast<char *>(&msg), msg.size());
}

void CCouponMgr::SendMsg_CouponLendResult(unsigned __int16 wIdx, _STORAGE_LIST::_db_con *pCoupon)
{
  _add_lend_item_result_zocl msg{};
  msg.byTblCode = pCoupon->m_byTableCode;
  msg.wItemIdx = pCoupon->m_wItemIndex;
  msg.dwDur = pCoupon->m_dwDur;
  msg.dwUp = pCoupon->m_dwLv;
  msg.dwItemSerial = pCoupon->m_wSerial;
  msg.byCsMethod = pCoupon->m_byCsMethod;
  msg.dwT = pCoupon->m_dwT;

  unsigned __int8 type[2]{7, 69};
  g_Network.m_pProcess[0]->LoadSendMsg(wIdx, type, reinterpret_cast<char *>(&msg), msg.size());
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
  const unsigned long long connectTimeLimit = GetConnectTime_AddBySec(-1800);

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
}

void CCouponMgr::LogOut(bool bForceClose)
{
  CNationSettingManager *manager = CTSingleton<CNationSettingManager>::Instance();
  if (manager->GetNationCode() == 410)
  {
    if (m_pkInfo)
    {
      m_pkInfo->bForcedClose = bForceClose;
      m_pkInfo->dwContPlayTime = m_dwContTime;
      m_pkInfo->dwLastConnTime = GetKorLocalTime();
      m_pkInfo->byReceiveCoupon = m_byReceiveCoupon;
      m_pkInfo->byEnsureTime = m_byRemainTime;
    }
  }
}
