#include "pch.h"

#include "CNationSettingManager.h"

#include "CNationSettingData.h"
#include "CNationSettingFactoryGroup.h"
#include "INationGameGuardSystem.h"
#include "NameTxt_fld.h"

int CNationSettingManager::Init(int nationCode, const char *nationCodeStr, bool serviceMode)
{
  CNationSettingFactoryGroup factoryGroup;
  const int initResult = factoryGroup.Init();
  if (initResult)
  {
    return -1;
  }

  m_pData = factoryGroup.Create(nationCode, nationCodeStr, serviceMode);
  if (!m_pData)
  {
    return -2;
  }

  if (m_pData->Init())
  {
    return -3;
  }

  return 0;
}

CBilling *CNationSettingManager::CreateBilling()
{
  if (m_pData == nullptr)
  {
    return nullptr;
  }
  return m_pData->CreateBilling();
}

CashDbWorker *CNationSettingManager::CreateWorker()
{
  return m_pData->CreateWorker();
}

int CNationSettingManager::GetCashItemPrice(_CashShop_str_fld *pFld)
{
  return m_pData->GetCashItemPrice(pFld);
}

const char *CNationSettingManager::GetItemName(_NameTxt_fld *pFld)
{
  if (pFld == nullptr)
  {
    return "";
  }
  if (m_pData == nullptr)
  {
    return pFld->m_NameTag[0];
  }
  return m_pData->GetItemName(pFld);
}

char *CNationSettingManager::GetNoneString()
{
  return m_pData->GetNoneString();
}

int CNationSettingManager::GetNationCode()
{
  return m_pData->m_iNationCode;
}

bool CNationSettingManager::IsApplyPcbangPrimium(const CPlayer *pUser)
{
  if (!m_pData)
  {
    return false;
  }
  return m_pData->IsApplyPcbangPrimium(pUser);
}

bool CNationSettingManager::IsCashDBUseExtRef()
{
  return m_pData->IsCashDBUseExtRef();
}

bool CNationSettingManager::IsCashDBInit()
{
  return m_pData->IsCashDBInit();
}

bool CNationSettingManager::IsCashDBDSNSetted()
{
  return m_pData->IsCashDBDSNSetted();
}

unsigned __int16 CNationSettingManager::GetBillingForceCloseDelay()
{
  return m_pData->m_wBillingForceCloseDelay;
}

char *CNationSettingManager::GetCashDBName()
{
  return m_pData->m_szCashDBName;
}

char *CNationSettingManager::GetCashDBDBIP()
{
  return m_pData->m_szCashDBIP;
}

char *CNationSettingManager::GetCashDBID()
{
  return m_pData->m_szCashDBID;
}

char *CNationSettingManager::GetCashDBPW()
{
  return m_pData->m_szCashDBPW;
}

unsigned __int16 CNationSettingManager::GetCashDBPort()
{
  return m_pData->m_wCashDBPort;
}

char *CNationSettingManager::GetWorldDBID()
{
  return m_pData->m_szWorldDBID;
}

char *CNationSettingManager::GetWorldDBPW()
{
  return m_pData->m_szWorldDBPW;
}

char *CNationSettingManager::GetNationCodeStr()
{
  return m_pData->m_szNationCodeStr;
}

void CNationSettingManager::SetCashDBInitState()
{
  m_pData->SetCashDBInitFlag();
}

void CNationSettingManager::OnConnectSession(unsigned int n)
{
  INationGameGuardSystem *gameGuard = m_pData->GetGameGuardSystem();
  if (gameGuard)
  {
    gameGuard->OnConnectSession(static_cast<int>(n));
  }
}

void CNationSettingManager::OnDisConnectSession(unsigned int n)
{
  INationGameGuardSystem *gameGuard = m_pData->GetGameGuardSystem();
  if (gameGuard)
  {
    gameGuard->OnDisConnectSession(static_cast<int>(n));
  }
}

void CNationSettingManager::SendCashDBDSNRequest()
{
  m_pData->SendCashDBDSNRequest();
}

void CNationSettingManager::NetClose(CPlayer *pOne)
{
  m_pData->NetClose(pOne);
}
