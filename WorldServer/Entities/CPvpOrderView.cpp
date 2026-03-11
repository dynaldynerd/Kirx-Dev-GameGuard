#include "pch.h"

#include "CPvpOrderView.h"

#include "CNetworkEX.h"
#include "CMainThread.h"
#include "GlobalObjects.h"
#include "Packet/ZoneClientPacket.h"
#include "WorldServerUtil.h"

#include <cstring>
#include <ctime>
#include <mmsystem.h>

_pvp_order_view_inform_zocl::_pvp_order_view_inform_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

int _pvp_order_view_inform_zocl::size() const
{
  return 24;
}

char CPvpOrderView::Init()
{
  m_dwLastAttackTime = 0;
  m_dwLastDamagedTime = 0;
  m_bAttack = false;
  m_bDamaged = false;
  return 1;
}

void CPvpOrderView::SetOrderViewAttackState()
{
  m_bAttack = true;
  m_dwLastAttackTime = timeGetTime();
}

void CPvpOrderView::SetOrderViewDamagedState()
{
  m_bDamaged = true;
  m_dwLastDamagedTime = timeGetTime();
}

void CPvpOrderView::Loop(unsigned __int16 wIndex)
{
  const unsigned int now = timeGetTime();
  const int attackEndDelayMs = 180000;
  const int damagedEndDelayMs = 180000;

  if (m_dwLastAttackTime + attackEndDelayMs <= now && m_bAttack)
  {
    Notify_PvPEnd(wIndex);
    m_dwLastAttackTime = 0;
    m_bAttack = false;
  }

  if (m_dwLastDamagedTime + damagedEndDelayMs <= now && m_bDamaged)
  {
    Notify_PvPEnd(wIndex);
    m_dwLastDamagedTime = 0;
    m_bDamaged = false;
  }
}

long double CPvpOrderView::GetPvpCash()
{
  return m_dPvpCash;
}

long double CPvpOrderView::GetPvpTempCash()
{
  return m_dPvpTempCash;
}

void CPvpOrderView::SetPvpCash(long double dAlter)
{
  m_dPvpCash = dAlter;
}

char CPvpOrderView::SetPvpOrderView(long double dPvpPoint, _PVP_ORDER_VIEW_DB_BASE *pkInfo, CPlayer *pOne)
{
  std::tm lastTime{};
  if (std::tm *localLast = localtime_2(&pkInfo->tUpdatedate))
  {
    std::memcpy(&lastTime, localLast, sizeof(lastTime));
  }

  __int64 updateDate = 0;
  time_20(&updateDate);
  std::tm *localNow = localtime_2(&updateDate);
  m_pkInfo = pkInfo;

  if (localNow && lastTime.tm_year == localNow->tm_year && lastTime.tm_mon == localNow->tm_mon
      && lastTime.tm_mday == localNow->tm_mday)
  {
    m_nKillCnt = pkInfo->nKill;
    m_nDeahtCnt = pkInfo->nDeath;
    m_dTodayPvpPoint = pkInfo->dTodayStacked;
    m_dOriginalPvpPoint = pkInfo->dPvpPoint;
    m_dPvpPoint = 0.0;
    m_dPvpTempCash = pkInfo->dPvpTempCash;
    m_dPvpCash = pkInfo->dPvpCash;
    return 0;
  }

  m_dPvpCash = pkInfo->dPvpTempCash + pkInfo->dPvpCash;
  UpdatePvpCash(m_dPvpCash);
  Update(updateDate, 0, 0, 0.0, dPvpPoint, 0.0);
  Update_RaceWarRecvr(false);
  ResetPvPOrderView();
  m_dOriginalPvpPoint = pkInfo->dPvpPoint;
return 1;
}

void CPvpOrderView::UpdatePvpCash(double dPvpCash)
{
  if (!m_pkInfo)
  {
    return;
  }

  double clamped = dPvpCash;
  if (clamped > 999999.0)
  {
    clamped = 999999.0;
  }
  if (clamped < -999999.0)
  {
    clamped = -999999.0;
  }

  __int64 updateTime = 0;
  time_20(&updateTime);
  m_pkInfo->tUpdatedate = updateTime;
  m_pkInfo->dPvpCash = clamped;
}

void CPvpOrderView::UpdatePvPPoint(long double dUpPoint, long double dNewPoint)
{
  m_dPvpPoint = dUpPoint;
  m_dTodayPvpPoint = m_dTodayPvpPoint + dUpPoint;
  m_dOriginalPvpPoint = dNewPoint;

  __int64 updateTime = 0;
  time_20(&updateTime);
  Update(updateTime, m_nDeahtCnt, m_nKillCnt, m_dTodayPvpPoint, m_dOriginalPvpPoint, m_dPvpTempCash);
}

void CPvpOrderView::Update_PvpTempCash(unsigned __int16 wIndex, double dTempPvpCash)
{
  const double playerPenalty = g_Main.m_pTimeLimitMgr->GetPlayerPenalty(wIndex);
  double adjusted = dTempPvpCash;
  if (dTempPvpCash > 0.0)
  {
    adjusted = dTempPvpCash * playerPenalty;
  }

  m_dPvpTempCash += adjusted;
  if (m_dPvpTempCash > 999999.0)
  {
    m_dPvpTempCash = 999999.0;
  }
  if (m_dPvpTempCash < -999999.0)
  {
    m_dPvpTempCash = -999999.0;
  }

  __int64 now = 0;
  time_20(&now);
  Update(now, m_nDeahtCnt, m_nKillCnt, m_dTodayPvpPoint, m_dOriginalPvpPoint, m_dPvpTempCash);
  Notify_PvpTempCash(wIndex);
}

void CPvpOrderView::Update_ContLoseCash(unsigned __int8 byCnt)
{
  if (!m_pkInfo)
  {
    return;
  }

  __int64 now = 0;
  time_20(&now);
  m_pkInfo->tUpdatedate = now;
  m_pkInfo->byContLoseCash = byCnt;
}

void CPvpOrderView::Update_ContHaveCash(unsigned __int8 byCnt)
{
  if (!m_pkInfo)
  {
    return;
  }

  __int64 now = 0;
  time_20(&now);
  m_pkInfo->tUpdatedate = now;
  m_pkInfo->byContHaveCash = byCnt;
}

void CPvpOrderView::Notify_OrderView(unsigned __int16 wIndex)
{
  _pvp_order_view_inform_zocl msg;
  msg.nKillCnt = m_nKillCnt;
  msg.nDeahtCnt = m_nDeahtCnt;
  msg.dTodayPvpPoint = m_dTodayPvpPoint;
  msg.dOriginalPvpPoint = m_dOriginalPvpPoint;

  unsigned __int8 type[2] = {13, 124};
  const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg(wIndex, type, reinterpret_cast<char *>(&msg), len);
}

void CPvpOrderView::Notify_Point(unsigned __int16 wIndex, long double dChangePoint, unsigned int dwTarSerial)
{

  _pvp_order_view_point_inform_zocl msg{};
  msg.dwSerial = dwTarSerial;
  msg.dPvpPoint = dChangePoint;

  unsigned __int8 pbyType[2]{13, 126};
  g_Network.m_pProcess[0]->LoadSendMsg(
    wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CPvpOrderView::Notify_PvPEnd(unsigned __int16 wIndex)
{
  unsigned __int8 pbyType[2]{13, 125};
  g_Network.m_pProcess[0]->LoadSendMsg(wIndex, pbyType, nullptr, 0);
}

void CPvpOrderView::Notify_PvpTempCash(unsigned __int16 wIndex)
{
  _alter_pvp_temp_cash_zocl msg{};
  msg.dNewTempPoint = m_dPvpTempCash;

  unsigned __int8 type[2] = {11, 15};
  g_Network.m_pProcess[0]->LoadSendMsg(
    wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CPvpOrderView::UpdatePvPKill(unsigned __int16 wIndex, unsigned int dwTarSerial)
{
  ++m_nKillCnt;

  __int64 updateTime = 0;
  time_20(&updateTime);
  Update(updateTime, m_nDeahtCnt, m_nKillCnt, m_dTodayPvpPoint, m_dOriginalPvpPoint, m_dPvpTempCash);
  Notify_OrderView(wIndex);
  if (m_dPvpPoint > 0.0)
  {
    Notify_Point(wIndex, m_dPvpPoint, dwTarSerial);
    m_dPvpPoint = 0.0;
  }
}

void CPvpOrderView::UpdatePvPDeath(unsigned __int16 wIndex, unsigned int dwTarSerial)
{
  ++m_nDeahtCnt;

  __int64 updateTime = 0;
  time_20(&updateTime);
  Update(updateTime, m_nDeahtCnt, m_nKillCnt, m_dTodayPvpPoint, m_dOriginalPvpPoint, m_dPvpTempCash);
  Notify_OrderView(wIndex);
  if (m_dPvpPoint < 0.0)
  {
    Notify_Point(wIndex, m_dPvpPoint, dwTarSerial);
    m_dPvpPoint = 0.0;
  }
}

void CPvpOrderView::Update_KillerList(unsigned int dwSerial, int nIndex)
{
  if (m_pkInfo)
  {
    __int64 updateTime = 0;
    time_20(&updateTime);
    m_pkInfo->tUpdatedate = updateTime;
    m_pkInfo->dwKillerSerial[nIndex] = dwSerial;
  }
}

void CPvpOrderView::Update_RaceWarRecvr(bool bUse)
{
  if (!m_pkInfo)
  {
    return;
  }

  __int64 now = 0;
  time_20(&now);
  m_pkInfo->tUpdatedate = now;
  m_pkInfo->bRaceWarRecvr = bUse;
}

void CPvpOrderView::ResetPvPOrderView()
{
  m_dPvpPoint = 0.0;
  m_dTodayPvpPoint = 0.0;
  m_nKillCnt = 0;
  m_nDeahtCnt = 0;
  m_dPvpTempCash = 0.0;
}

void CPvpOrderView::Update(
  __int64 tUpdateDate,
  int nDeath,
  int nKill,
  long double dTodayStacked,
  long double dPvpPoint,
  long double dPvpTempCash)
{
  if (!m_pkInfo)
  {
    return;
  }

  m_pkInfo->tUpdatedate = tUpdateDate;
  m_pkInfo->nDeath = nDeath;
  m_pkInfo->nKill = nKill;
  m_pkInfo->dTodayStacked = dTodayStacked;
  m_pkInfo->dPvpPoint = dPvpPoint;
  m_pkInfo->dPvpTempCash = dPvpTempCash;
}
