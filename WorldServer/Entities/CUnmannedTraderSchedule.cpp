#include "pch.h"

#include "CUnmannedTraderSchedule.h"

CUnmannedTraderSchedule::CUnmannedTraderSchedule()
{
  Clear();
}

void CUnmannedTraderSchedule::Clear()
{
  this->m_eState = STATE::EMPTY;
  this->m_byType = static_cast<unsigned char>(-1);
  this->m_dwRegistSerial = 0;
  this->m_tEndTime = 0;
  this->m_dwOwnerSerial = 0;
  this->m_byItemTableCode = static_cast<unsigned char>(-1);
  this->m_wItemTableIndex = static_cast<unsigned short>(-1);
}
