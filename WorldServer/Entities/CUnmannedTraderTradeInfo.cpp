#include "pch.h"

#include "CUnmannedTraderTradeInfo.h"

void CUnmannedTraderTradeInfo::AddIncome(unsigned int dwOriPrice)
{
  this->m_ui64TotalCurrentIncome += dwOriPrice;
  this->m_bNeedUpdateSave = true;
}
