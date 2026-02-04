#include "pch.h"

#include "CMoneySupplyMgr.h"

#include <new>

CMoneySupplyMgr *CMoneySupplyMgr::pInstance = nullptr;

CMoneySupplyMgr::CMoneySupplyMgr() = default;

CMoneySupplyMgr *CMoneySupplyMgr::Instance()
{
  if (!pInstance)
  {
    pInstance = new (std::nothrow) CMoneySupplyMgr();
  }
  return pInstance;
}

void CMoneySupplyMgr::UpdateBuyUnitData(int nLv, unsigned int nAmount)
{
  m_MS_data.dwAmount[7] += nAmount;
  switch (nLv)
  {
    case 30:
      ++m_MS_data.nBuyUnitLv[0];
      break;
    case 40:
      ++m_MS_data.nBuyUnitLv[1];
      break;
    case 50:
      ++m_MS_data.nBuyUnitLv[2];
      break;
    case 60:
      ++m_MS_data.nBuyUnitLv[3];
      break;
    default:
      break;
  }
}

