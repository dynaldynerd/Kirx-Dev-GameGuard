#include "pch.h"

#include "CNationSettingManager.h"

#include "CNationSettingData.h"
#include "CNationSettingDataNULL.h"
#include "CNationSettingFactoryGroup.h"
#include "CHackShieldExSystem.h"
#include "CnProtectSystem.h"
#include "INationGameGuardSystem.h"
#include "NameTxt_fld.h"

namespace
{
char g_szEmptyString[] = "";
char g_szNoneString[] = "None";
}

CNationSettingManager::CNationSettingManager()
{
  m_pData = &CNationSettingDataNULL::ms_NULL;
}

CNationSettingManager::~CNationSettingManager()
{
  if (m_pData && m_pData != &CNationSettingDataNULL::ms_NULL)
  {
    delete m_pData;
  }
  m_pData = nullptr;
}

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

  delete m_pData->m_pGameGuardSystem;
  m_pData->m_pGameGuardSystem = nullptr;

  const int gameGuardType = GetPrivateProfileIntA("GameGuard", "Type", 0, ".\\Initialize\\GameGuard.ini");
  switch (gameGuardType)
  {
    case 0:
      break;
    case 1:
    {
      CHackShieldExSystem *pHackShieldExSystem = new CHackShieldExSystem();
      if (!pHackShieldExSystem->Init<HACKSHEILD_PARAM_ANTICPX_5381>(true))
      {
        delete pHackShieldExSystem;
        return -4;
      }
      m_pData->m_pGameGuardSystem = pHackShieldExSystem;
      break;
    }
    case 2:
    {
      CnProtectSystem *pnProtectSystem = new CnProtectSystem();
      if (!pnProtectSystem->Init<NPROTECT_PARAM_VER_30>(true, true))
      {
        delete pnProtectSystem;
        return -4;
      }
      m_pData->m_pGameGuardSystem = pnProtectSystem;
      break;
    }
    default:
      return -4;
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
  if (m_pData == nullptr)
  {
    return nullptr;
  }
  return m_pData->CreateWorker();
}

int CNationSettingManager::GetCashItemPrice(_CashShop_str_fld *pFld)
{
  if (m_pData == nullptr)
  {
    return 0;
  }
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
  if (m_pData == nullptr)
  {
    return g_szNoneString;
  }
  return m_pData->GetNoneString();
}

int CNationSettingManager::GetNationCode()
{
  if (m_pData == nullptr)
  {
    return -1;
  }
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
  if (m_pData == nullptr)
  {
    return false;
  }
  return m_pData->IsCashDBUseExtRef();
}

bool CNationSettingManager::IsCashDBInit()
{
  if (m_pData == nullptr)
  {
    return false;
  }
  return m_pData->IsCashDBInit();
}

bool CNationSettingManager::IsCashDBDSNSetted()
{
  if (m_pData == nullptr)
  {
    return false;
  }
  return m_pData->IsCashDBDSNSetted();
}

void CNationSettingManager::SetUnitPassiveValue(float *fUnitPv_DefFc)
{
  if (m_pData != nullptr)
  {
    m_pData->SetUnitPassiveValue(fUnitPv_DefFc);
  }
}

unsigned __int16 CNationSettingManager::GetBillingForceCloseDelay()
{
  if (m_pData == nullptr)
  {
    return 0;
  }
  return m_pData->m_wBillingForceCloseDelay;
}

char *CNationSettingManager::GetCashDBName()
{
  if (m_pData == nullptr)
  {
    return g_szEmptyString;
  }
  return m_pData->m_szCashDBName;
}

char *CNationSettingManager::GetCashDBDBIP()
{
  if (m_pData == nullptr)
  {
    return g_szEmptyString;
  }
  return m_pData->m_szCashDBIP;
}

char *CNationSettingManager::GetCashDBID()
{
  if (m_pData == nullptr)
  {
    return g_szEmptyString;
  }
  return m_pData->m_szCashDBID;
}

char *CNationSettingManager::GetCashDBPW()
{
  if (m_pData == nullptr)
  {
    return g_szEmptyString;
  }
  return m_pData->m_szCashDBPW;
}

unsigned __int16 CNationSettingManager::GetCashDBPort()
{
  if (m_pData == nullptr)
  {
    return 0;
  }
  return m_pData->m_wCashDBPort;
}

char *CNationSettingManager::GetWorldDBID()
{
  if (m_pData == nullptr)
  {
    return g_szEmptyString;
  }
  return m_pData->m_szWorldDBID;
}

char *CNationSettingManager::GetWorldDBPW()
{
  if (m_pData == nullptr)
  {
    return g_szEmptyString;
  }
  return m_pData->m_szWorldDBPW;
}

char *CNationSettingManager::GetNationCodeStr()
{
  if (m_pData == nullptr)
  {
    return g_szEmptyString;
  }
  return m_pData->m_szNationCodeStr;
}

CHEAT_COMMAND *CNationSettingManager::GetCheatTable()
{
  if (m_pData == nullptr || m_pData->m_vecCheatData.empty())
  {
    return nullptr;
  }
  return &m_pData->m_vecCheatData[0];
}

void CNationSettingManager::SetCashDBDSN(
  char *szIP,
  char *szDBName,
  char *szAccount,
  char *szPassword,
  unsigned __int16 dwPort)
{
  if (m_pData != nullptr)
  {
    m_pData->SetCashDBDSN(szIP, szDBName, szAccount, szPassword, dwPort);
  }
}

void CNationSettingManager::SetCashDBInitState()
{
  if (m_pData != nullptr)
  {
    m_pData->SetCashDBInitFlag();
  }
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

bool CNationSettingManager::OnCheckSession_FirstVerify(unsigned int n)
{
  INationGameGuardSystem *gameGuard = m_pData->GetGameGuardSystem();
  if (!gameGuard)
  {
    return true;
  }
  return gameGuard->OnCheckSession_FirstVerify(static_cast<int>(n));
}

bool CNationSettingManager::CheckEnterWorldRequest(unsigned int n, char *pBuf)
{
  INationGameGuardSystem *gameGuard = m_pData->GetGameGuardSystem();
  if (gameGuard && !gameGuard->OnCheckSession_FirstVerify(static_cast<int>(n)))
  {
    return false;
  }
  return m_pData->CheckEnterWorldRequest(static_cast<int>(n), pBuf);
}

bool CNationSettingManager::IsPersonalFreeFixedAmountBillingType(__int16 *pDest1, __int16 *pDest2)
{
  if (!m_pData)
  {
    return false;
  }
  return m_pData->IsPersonalFreeFixedAmountBillingType(pDest1, pDest2);
}

void CNationSettingManager::CreateComplete(CPlayer *pOne)
{
  if (m_pData)
  {
    m_pData->CreateComplete(pOne);
  }
}

bool CNationSettingManager::IsNormalString(const char *szString)
{
  return m_pData->IsNormalString(szString);
}

bool CNationSettingManager::ValidMacAddress()
{
  return m_pData->ValidMacAddress();
}

char *CNationSettingManager::GetServerVaildKey()
{
  return m_pData->m_szVaildKey;
}

bool CNationSettingManager::RecvGameGuardData(unsigned int n, _MSG_HEADER *pHeader, char *pBuff)
{
  if (!m_pData->GetGameGuardSystem())
  {
    return false;
  }
  INationGameGuardSystem *gameGuard = m_pData->GetGameGuardSystem();
  return gameGuard->RecvClientLine(static_cast<int>(n), pHeader, pBuff);
}

void CNationSettingManager::SendCashDBDSNRequest()
{
  if (m_pData != nullptr)
  {
    m_pData->SendCashDBDSNRequest();
  }
}

void CNationSettingManager::NetClose(CPlayer *pOne)
{
  if (m_pData != nullptr)
  {
    m_pData->NetClose(pOne);
  }
}

void CNationSettingManager::Loop()
{
  if (m_pData == nullptr)
  {
    return;
  }
  m_pData->Loop();
  INationGameGuardSystem *gameGuard = m_pData->GetGameGuardSystem();
  if (gameGuard)
  {
    gameGuard->OnLoop();
  }
}
