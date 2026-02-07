#include "pch.h"

#include "CUnmannedTraderTaxRateManager.h"
#include "CUnmannedTraderEnvironmentValue.h"

CUnmannedTraderTaxRateManager *CUnmannedTraderTaxRateManager::ms_Instance = nullptr;

CUnmannedTraderTaxRateManager::CUnmannedTraderTaxRateManager()
  : m_pkTimer(nullptr), m_vecTRC()
{
}

CUnmannedTraderTaxRateManager *CUnmannedTraderTaxRateManager::Instance()
{
  if (!CUnmannedTraderTaxRateManager::ms_Instance)
  {
    CUnmannedTraderTaxRateManager::ms_Instance = new CUnmannedTraderTaxRateManager();
  }
  return CUnmannedTraderTaxRateManager::ms_Instance;
}

bool CUnmannedTraderTaxRateManager::Init(CLogFile *pkLogger)
{
  this->m_pkTimer = new CMyTimer();
  if (!this->m_pkTimer)
  {
    return false;
  }

  for (int i = 0; i < 3; ++i)
  {
    TRC_AutoTrade *autoTrade = new TRC_AutoTrade(static_cast<unsigned char>(i));
    this->m_vecTRC.push_back(autoTrade);

    if (!this->m_vecTRC.back())
    {
      pkLogger->Write("CUnmannedTraderTaxRateManager::Init(...) : new TRC_AutoTrade NULL!\r\n");
      return false;
    }

    if (!this->m_vecTRC.back()->Initialzie())
    {
      pkLogger->Write(
        "CUnmannedTraderTaxRateManager::Init(...) : m_vecTRC.back()->Initialzie() Fail!\r\n");
      return false;
    }
  }

  this->m_pkTimer->BeginTimer(CUnmannedTraderEnvironmentValue::Unmanned_Trader_Change_Taxrate_Check_Changed_Day_Delay);
  return true;
}

bool CUnmannedTraderTaxRateManager::IsOwnerGuild(unsigned __int8 byRace, unsigned int dwGuildSerial)
{
  if (m_vecTRC.empty() || m_vecTRC.size() <= byRace)
  {
    return false;
  }

  return m_vecTRC[byRace]->IsOwnerGuild(dwGuildSerial);
}

float CUnmannedTraderTaxRateManager::GetTaxRate(unsigned __int8 byRace)
{
  if (m_vecTRC.empty() || m_vecTRC.size() <= byRace)
  {
    return FLOAT_255_0;
  }

  return m_vecTRC[byRace]->get_taxrate();
}
