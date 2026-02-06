#include "pch.h"

#include "CUnmannedTraderUserInfo.h"

CUnmannedTraderUserInfo CUnmannedTraderUserInfo::ms_kNull;

CUnmannedTraderUserInfo::CUnmannedTraderUserInfo()
  : m_eState(LOG_IN_STATE::UTUI_NONE),
    m_wInx(static_cast<unsigned short>(-1)),
    m_dwUserSerial(0),
    m_byRegistCnt(0),
    m_byMaxRegistCnt(0),
    m_kRequestState(),
    m_vecRegistItemInfo(),
    m_vecLoadItemInfo()
{
  Clear();
}

bool CUnmannedTraderUserInfo::Init(unsigned short wInx)
{
  this->m_wInx = wInx;

  CUnmannedTraderRegistItemInfo regInfo;
  this->m_vecRegistItemInfo.assign(10u, regInfo);
  if (this->m_vecRegistItemInfo.size() != 10u)
  {
    return false;
  }

  CUnmannedTraderRegistItemInfo loadInfo;
  this->m_vecLoadItemInfo.assign(20u, loadInfo);
  if (this->m_vecLoadItemInfo.size() != 20u)
  {
    return false;
  }

  this->m_kRequestState.ClearRequset();
  this->m_eState = LOG_IN_STATE::UTUI_EMPTY;
  return true;
}

void CUnmannedTraderUserInfo::Clear()
{
  if (IsNull())
  {
    return;
  }

  for (CUnmannedTraderRegistItemInfo &item : this->m_vecRegistItemInfo)
  {
    item.Clear();
  }

  ClearLoadItemInfo();
  this->m_dwUserSerial = 0;
  this->m_byRegistCnt = 0;
}

void CUnmannedTraderUserInfo::ClearLoadItemInfo()
{
  for (CUnmannedTraderRegistItemInfo &item : this->m_vecLoadItemInfo)
  {
    item.Clear();
  }
}

bool CUnmannedTraderUserInfo::IsNull()
{
  return this == &CUnmannedTraderUserInfo::ms_kNull;
}
