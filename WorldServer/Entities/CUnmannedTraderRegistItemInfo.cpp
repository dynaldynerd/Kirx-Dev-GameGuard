#include "pch.h"

#include "CUnmannedTraderRegistItemInfo.h"

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

bool CUnmannedTraderRegistItemInfo::IsRegist()
{
  const unsigned int state = m_kState.GetState();
  return state == static_cast<unsigned int>(CUnmannedTraderItemState::STATE::WATEREL)
      || state == static_cast<unsigned int>(CUnmannedTraderItemState::STATE::SOILEL);
}

unsigned __int16 CUnmannedTraderRegistItemInfo::GetItemIndex()
{
  return m_wItemIndex;
}

unsigned __int8 CUnmannedTraderRegistItemInfo::GetTableCode()
{
  return m_byTableCode;
}

__int64 *CUnmannedTraderRegistItemInfo::GetStartTimePtr()
{
  return &m_tStartTime;
}

__int64 CUnmannedTraderRegistItemInfo::GetStartTime()
{
  return m_tStartTime;
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
