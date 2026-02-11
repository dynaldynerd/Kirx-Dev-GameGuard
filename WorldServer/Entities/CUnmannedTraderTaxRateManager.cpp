#include "pch.h"

#include "CUnmannedTraderTaxRateManager.h"
#include "CUnmannedTraderEnvironmentValue.h"
#include "CWeeklyGuildRankManager.h"
#include "CGuild.h"

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

char CUnmannedTraderTaxRateManager::Load()
{
  if (this->m_vecTRC.empty())
  {
    return 0;
  }

  for (int j = 0; j < 3; ++j)
  {
    if (!this->m_vecTRC[j])
    {
      return 0;
    }

    CWeeklyGuildRankManager *rankMgr = CWeeklyGuildRankManager::Instance();
    CGuild *ownerGuild = rankMgr->GetPrevOwnerGuild(j, 0);
    this->m_vecTRC[j]->set_owner(ownerGuild);
    if (!this->m_vecTRC[j]->_db_load(j))
    {
      return 0;
    }
  }

  return 1;
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

void CUnmannedTraderTaxRateManager::SendTaxRate(int n, unsigned __int8 byRace)
{
  if (m_vecTRC.empty() || m_vecTRC.size() <= byRace)
  {
    return;
  }

  TRC_AutoTrade *autoTrade = m_vecTRC[byRace];
  autoTrade->sendmsg_taxrate(n, 0);
}

void CUnmannedTraderTaxRateManager::DQSCompleteInAtradTaxMoney(unsigned __int8 byRace, char *pdata)
{
  if (!m_vecTRC.empty() && m_vecTRC.size() > byRace)
  {
    TRC_AutoTrade *autoTrade = m_vecTRC[byRace];
    autoTrade->AddGDalant(pdata);
  }
}

unsigned int CUnmannedTraderTaxRateManager::GetTax(
  unsigned __int8 byRace,
  unsigned int dwGuildSerial,
  unsigned int dwPrice)
{
  if (m_vecTRC.empty() || m_vecTRC.size() <= byRace)
  {
    return dwPrice;
  }

  TRC_AutoTrade *autoTrade = m_vecTRC[byRace];
  return autoTrade->CalcPrice(dwGuildSerial, dwPrice);
}

int CUnmannedTraderTaxRateManager::ChangeOwner(unsigned __int8 byRace, CGuild *pGuild)
{
  if (m_vecTRC.empty() || m_vecTRC.size() <= byRace)
  {
    return -1;
  }

  TRC_AutoTrade *autoTrade = m_vecTRC[byRace];
  return autoTrade->ChangeOwner(pGuild);
}

void CUnmannedTraderTaxRateManager::SetPatriarchTaxMoney(unsigned __int8 byRace, unsigned int dwTax)
{
  if (!m_vecTRC.empty() && m_vecTRC.size() > byRace)
  {
    TRC_AutoTrade *autoTrade = m_vecTRC[byRace];
    autoTrade->SetPatriarchTaxMoney(static_cast<int>(dwTax));
  }
}
