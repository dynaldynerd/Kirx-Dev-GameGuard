#include "pch.h"

#include "CUnmannedTraderRegistItemInfo.h"
#include "WorldServerUtil.h"

CUnmannedTraderRegistItemInfo::CUnmannedTraderRegistItemInfo()
  : m_dwRegistSerial(0),
    m_wItemSerial(0),
    m_dwETSerialNumber(0),
    m_dwPrice(0),
    m_tStartTime(0),
    m_bySellTurm(0),
    m_dwBuyerSerial(0),
    m_dwTax(0),
    m_tResultTime(0),
    m_wszBuyerName{},
    m_szBuyerAccount{},
    m_byTableCode(0),
    m_wItemIndex(0),
    m_byStorageIndex(0),
    m_dwD(0),
    m_dwU(0),
    m_kState()
{
  Clear();
}

void CUnmannedTraderRegistItemInfo::Clear()
{
  this->m_kState.Clear();
  this->m_dwRegistSerial = 0;
  this->m_wItemSerial = static_cast<unsigned short>(-1);
  this->m_dwETSerialNumber = 0;
  this->m_dwPrice = 0;
  this->m_tStartTime = 0;
  this->m_bySellTurm = 0;
  this->m_dwBuyerSerial = 0;
  this->m_dwTax = 0;
  this->m_tResultTime = 0;
  this->m_wszBuyerName[0] = '\0';
  this->m_szBuyerAccount[0] = '\0';
  this->m_byTableCode = static_cast<unsigned char>(-1);
  this->m_wItemIndex = 255;
  this->m_byStorageIndex = static_cast<unsigned char>(-1);
  this->m_dwD = static_cast<unsigned long long>(-1);
  this->m_dwU = 0x7FFFFFF;
}

void CUnmannedTraderRegistItemInfo::RegistItem(
  unsigned int dwRegistSerial,
  unsigned __int16 wItemSerial,
  unsigned int dwETSerialNumber,
  unsigned int dwPrice,
  unsigned __int8 bySellTurm,
  unsigned __int8 byTableCode,
  unsigned __int16 wItemIndex,
  unsigned __int8 byStorageIndex,
  unsigned __int64 dwD,
  unsigned int dwU,
  bool bInserted)
{
  __int64 now = 0;
  time_18(&now);

  m_tStartTime = now;
  m_dwRegistSerial = dwRegistSerial;
  m_wItemSerial = wItemSerial;
  m_dwETSerialNumber = dwETSerialNumber;
  m_dwPrice = dwPrice;
  m_bySellTurm = bySellTurm;
  m_dwBuyerSerial = 0xFFFFFFFFu;
  m_dwTax = 0;
  m_tResultTime = 0;
  m_wszBuyerName[0] = '\0';
  m_szBuyerAccount[0] = '\0';
  m_byTableCode = byTableCode;
  m_wItemIndex = wItemIndex;
  m_byStorageIndex = byStorageIndex;
  m_dwD = dwD;
  m_dwU = dwU;

  const unsigned __int8 state = bInserted ? 2u : 1u;
  m_kState.Set(state);
}

void CUnmannedTraderRegistItemInfo::RepriceItem(unsigned int dwPrice)
{
  m_dwPrice = dwPrice;
}

void CUnmannedTraderRegistItemInfo::ReRegistItem(unsigned int dwPrice)
{
  __int64 now = 0;
  time_18(&now);

  m_tStartTime = now;
  m_dwPrice = dwPrice;
  m_kState.Set(1u);
}

void CUnmannedTraderRegistItemInfo::SellComplete(
  unsigned int dwPrice,
  unsigned int dwBuyerSerial,
  unsigned int dwTax,
  __int64 tResultTime,
  const char *wszBuyerName,
  const char *szBuyerAccount)
{
  m_dwPrice = dwPrice;
  m_dwBuyerSerial = dwBuyerSerial;
  m_dwTax = dwTax;
  m_tResultTime = tResultTime;
  strcpy_s(m_wszBuyerName, 0x11u, wszBuyerName);
  strcpy_s(m_szBuyerAccount, 0xDu, szBuyerAccount);
  m_kState.Set(3u);
}

void CUnmannedTraderRegistItemInfo::ClearRegist()
{
  Clear();
  m_kState.Set(5u);
}

void CUnmannedTraderRegistItemInfo::ClearToWaitState()
{
  Clear();
  m_kState.Set(0u);
}

bool CUnmannedTraderRegistItemInfo::IsEmpty()
{
  const unsigned int state = m_kState.GetState();
  return state == static_cast<unsigned int>(CUnmannedTraderItemState::STATE::FIREEL)
      || state == static_cast<unsigned int>(CUnmannedTraderItemState::STATE::WINDEL)
      || state == 5
      || state == (static_cast<unsigned int>(CUnmannedTraderItemState::STATE::WINDEL) | 0x4)
      || state == (static_cast<unsigned int>(CUnmannedTraderItemState::STATE::WINDEL) | 0x8)
      || state == 8
      || state == 9
      || state == 12;
}

bool CUnmannedTraderRegistItemInfo::IsRegist()
{
  const unsigned int state = m_kState.GetState();
  return state == static_cast<unsigned int>(CUnmannedTraderItemState::STATE::WATEREL)
      || state == static_cast<unsigned int>(CUnmannedTraderItemState::STATE::SOILEL);
}

bool CUnmannedTraderRegistItemInfo::IsOverRegistTime()
{
  return GetLeftSec() == 0;
}

unsigned __int16 CUnmannedTraderRegistItemInfo::GetItemIndex()
{
  return m_wItemIndex;
}

unsigned __int16 CUnmannedTraderRegistItemInfo::GetItemSerial()
{
  return m_wItemSerial;
}

unsigned int CUnmannedTraderRegistItemInfo::GetBuyerSerial()
{
  return m_dwBuyerSerial;
}

unsigned __int8 CUnmannedTraderRegistItemInfo::GetTableCode()
{
  return m_byTableCode;
}

CUnmannedTraderItemState::STATE CUnmannedTraderRegistItemInfo::GetState()
{
  return static_cast<CUnmannedTraderItemState::STATE>(m_kState.GetState());
}

__int64 *CUnmannedTraderRegistItemInfo::GetStartTimePtr()
{
  return &m_tStartTime;
}

__int64 CUnmannedTraderRegistItemInfo::GetStartTime()
{
  return m_tStartTime;
}

__int64 CUnmannedTraderRegistItemInfo::GetResultTime()
{
  return m_tResultTime;
}

__int64 CUnmannedTraderRegistItemInfo::GetLeftSec()
{
  __int64 now = 0;
  time_18(&now);

  const __int64 endTime = m_tStartTime + 3600 * m_bySellTurm;
  if (endTime <= now)
  {
    return 0;
  }

  return static_cast<unsigned int>(endTime - now);
}

unsigned __int8 CUnmannedTraderRegistItemInfo::GetSellTurm()
{
  return m_bySellTurm;
}

unsigned int CUnmannedTraderRegistItemInfo::GetPrice()
{
  return m_dwPrice;
}

unsigned int CUnmannedTraderRegistItemInfo::GetETSerial()
{
  return m_dwETSerialNumber;
}

unsigned int CUnmannedTraderRegistItemInfo::GetU()
{
  return m_dwU;
}

unsigned __int64 CUnmannedTraderRegistItemInfo::GetD()
{
  return m_dwD;
}

unsigned __int8 CUnmannedTraderRegistItemInfo::GetStorageIndex()
{
  return m_byStorageIndex;
}

unsigned int CUnmannedTraderRegistItemInfo::GetRegistSerial()
{
  return m_dwRegistSerial;
}

bool CUnmannedTraderRegistItemInfo::SetState(unsigned __int8 byState)
{
  return m_kState.Set(byState) != 0;
}

void CUnmannedTraderRegistItemInfo::SetOverRegistTime()
{
  __int64 now = 0;
  time_18(&now);
  m_tStartTime = now - 3600 * m_bySellTurm;
}

bool CUnmannedTraderRegistItemInfo::IsWaitNoitfyClose()
{
  const unsigned int state = m_kState.GetState();
  return state == 6 || state == 13;
}
