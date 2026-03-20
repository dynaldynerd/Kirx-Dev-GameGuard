#include "pch.h"

#include "CUnmannedTraderTaxRateManager.h"
#include "CUnmannedTraderEnvironmentValue.h"
#include "CWeeklyGuildRankManager.h"
#include "CGuild.h"
#include "GlobalObjects.h"

CUnmannedTraderTaxRateManager *CUnmannedTraderTaxRateManager::ms_Instance;

CUnmannedTraderTaxRateManager::CUnmannedTraderTaxRateManager()
  : m_pkTimer(nullptr), m_vecTRC()
{
}

CUnmannedTraderTaxRateManager::~CUnmannedTraderTaxRateManager()
{
  CleanUp();
}

void CUnmannedTraderTaxRateManager::CleanUp()
{
  if (m_pkTimer)
  {
    delete m_pkTimer;
    m_pkTimer = nullptr;
  }

  if (!m_vecTRC.empty())
  {
    for (TRC_AutoTrade *autoTrade : m_vecTRC)
    {
      if (autoTrade)
      {
        delete autoTrade;
      }
    }
    m_vecTRC.clear();
  }
}

CUnmannedTraderTaxRateManager *CUnmannedTraderTaxRateManager::Instance()
{
  if (!CUnmannedTraderTaxRateManager::ms_Instance)
  {
    CUnmannedTraderTaxRateManager::ms_Instance = new CUnmannedTraderTaxRateManager();
  }
  return CUnmannedTraderTaxRateManager::ms_Instance;
}

void CUnmannedTraderTaxRateManager::Destroy()
{
  if (CUnmannedTraderTaxRateManager::ms_Instance)
  {
    delete CUnmannedTraderTaxRateManager::ms_Instance;
    CUnmannedTraderTaxRateManager::ms_Instance = nullptr;
  }
}

void CUnmannedTraderTaxRateManager::Loop()
{
  if (m_pkTimer && !m_vecTRC.empty() && m_pkTimer->CountingTimer())
  {
    for (int race = 0; race < 3; ++race)
    {
      m_vecTRC[race]->ChangeTaxRate();
    }
  }
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

bool CUnmannedTraderTaxRateManager::Load()
{
  if (this->m_vecTRC.empty())
  {
    return false;
  }

  for (int j = 0; j < 3; ++j)
  {
    if (!this->m_vecTRC[j])
    {
      return false;
    }

    CWeeklyGuildRankManager *rankMgr = CWeeklyGuildRankManager::Instance();
    CGuild *ownerGuild = rankMgr->GetPrevOwnerGuild(j, 0);
    this->m_vecTRC[j]->set_owner(ownerGuild);
    if (!this->m_vecTRC[j]->_db_load(j))
    {
      return false;
    }
  }

  return true;
}

void CUnmannedTraderTaxRateManager::CompleteCreate(unsigned __int16 wInx)
{
  if (wInx >= MAX_PLAYER || this->m_vecTRC.empty())
  {
    return;
  }

  CPlayer *player = &g_Player[wInx];
  const size_t raceCode = static_cast<size_t>(player->m_Param.GetRaceCode());
  if (this->m_vecTRC.size() <= raceCode)
  {
    return;
  }

  TRC_AutoTrade *autoTrade = this->m_vecTRC[raceCode];
  if (autoTrade)
  {
    autoTrade->SendMsg_UserLogInNotifyTaxRate(wInx);
  }
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

void CUnmannedTraderTaxRateManager::SendTaxRatePatriarch(int n, unsigned __int8 byRace)
{
  if (!m_vecTRC.empty() && m_vecTRC.size() > byRace)
  {
    TRC_AutoTrade *autoTrade = m_vecTRC[byRace];
    autoTrade->SendMsg_PatriarchTaxRate(n);
  }
}

unsigned int CUnmannedTraderTaxRateManager::GetSuggestedTime(unsigned __int8 byRace)
{
  if (m_vecTRC.empty() || m_vecTRC.size() <= byRace)
  {
    return static_cast<unsigned int>(-1);
  }

  TRC_AutoTrade *autoTrade = m_vecTRC[byRace];
  return autoTrade->getSuggestedTime();
}

void CUnmannedTraderTaxRateManager::SetSuggested(
  unsigned __int8 byRace,
  unsigned __int8 byMatterType,
  unsigned int dwMatterDst,
  char *wszMatterDst,
  unsigned int dwNext)
{
  if (!m_vecTRC.empty() && m_vecTRC.size() > byRace)
  {
    TRC_AutoTrade *autoTrade = m_vecTRC[byRace];
    autoTrade->set_suggested(byMatterType, dwMatterDst, wszMatterDst, dwNext);
  }
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

bool CUnmannedTraderTaxRateManager::CheatChangeTaxRate(
  unsigned __int8 byRace,
  int dwNewTaxRate,
  char *pCheaterName)
{
  if (m_vecTRC.empty() || m_vecTRC.size() <= byRace)
  {
    return false;
  }

  const float newTaxRate = static_cast<float>(dwNewTaxRate) / 100.0f;
  if (m_vecTRC[byRace]->ChangeTaxRate(newTaxRate))
  {
    return false;
  }

  m_vecTRC[byRace]->history_used_cheet_changetaxrate(dwNewTaxRate, pCheaterName);
  return true;
}

void CUnmannedTraderTaxRateManager::SetGuildMaintainMoney(
  unsigned __int8 byRace,
  unsigned int dwTax,
  unsigned int dwSeller)
{
  if (!m_vecTRC.empty() && m_vecTRC.size() > byRace)
  {
    m_vecTRC[byRace]->SetGuildMaintainMoney(dwTax, dwSeller);
  }
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
