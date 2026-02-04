#include "pch.h"

#include "CPvpOrderView.h"

#include "CNetworkEX.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

_pvp_order_view_inform_zocl::_pvp_order_view_inform_zocl()
{
  memset_0(this, 0, sizeof(*this));
}

int _pvp_order_view_inform_zocl::size() const
{
  return 24;
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
