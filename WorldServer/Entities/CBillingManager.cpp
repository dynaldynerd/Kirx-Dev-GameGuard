#include "pch.h"

#include "CBillingManager.h"

#include "CBillingNULL.h"
#include "CUserDB.h"
#include "CNationSettingManager.h"

CBillingManager::CBillingManager()
  : m_pBill(&CBillingNULL::ms_NULL)
{
}

CBillingManager::~CBillingManager() = default;

bool CBillingManager::LoadINI()
{
  const UINT billOper = GetPrivateProfileIntA("System", "BillOper", -1, "..\\WorldInfo\\WorldInfo.ini");
  if (billOper == static_cast<UINT>(-1))
  {
    MyMessageBox("CBillingManager::Init()", "WorldInfo.ini : BillOper Not Exist!");
    return false;
  }
  if (billOper == 0)
  {
    m_pBill->m_bOper = false;
    return true;
  }
  if (billOper == 1)
  {
    m_pBill->m_bOper = true;
    return true;
  }
  MyMessageBox("CBillingManager::Init()", "WorldInfo.ini : BillOper = %d Invalid!", billOper);
  return false;
}

bool CBillingManager::Init()
{
  if (!LoadINI())
  {
    return false;
  }
  if (!m_pBill->m_bOper)
  {
    return true;
  }
  CBilling *billing = CTSingleton<CNationSettingManager>::Instance()->CreateBilling();
  if (billing != nullptr)
  {
    m_pBill = billing;
  }
  return true;
}

void CBillingManager::Start()
{
  m_pBill->Start();
}

void CBillingManager::Login(CUserDB *pUserDB)
{
  m_pBill->Login(pUserDB);
}

void CBillingManager::Alive(CUserDB *pUserDB)
{
  m_pBill->Alive(pUserDB);
}

void CBillingManager::Logout(CUserDB *pUserDB)
{
  m_pBill->Logout(pUserDB);
}

bool CBillingManager::IsOperate()
{
  return m_pBill->m_bOper;
}

void CBillingManager::BillingClose(char *szID)
{
  m_pBill->BillingClose(szID);
}

void CBillingManager::Remaintime_Personal(
  char *szID,
  __int16 iType,
  int lRemaintime,
  _SYSTEMTIME *pstEndDate)
{
  m_pBill->Remaintime_Personal(szID, iType, lRemaintime, pstEndDate);
}

void CBillingManager::Remaintime_PCBang(
  char *szCMSCode,
  __int16 iType,
  int lRemaintime,
  _SYSTEMTIME *pstEndDate)
{
  m_pBill->Remaintime_PCBang(szCMSCode, iType, lRemaintime, pstEndDate);
}

void CBillingManager::Change_BillingType(
  char *szID,
  char *szCMSCode,
  __int16 iType,
  int lRemainTime,
  _SYSTEMTIME *pstEndDate,
  unsigned __int8 byReason)
{
  m_pBill->Change_BillingType(szID, szCMSCode, iType, lRemainTime, pstEndDate, byReason);
}

void CBillingManager::Expire_Personal(char *szID)
{
  m_pBill->Expire_Personal(szID);
}

void CBillingManager::Expire_PCBang(char *szCMS)
{
  m_pBill->Expire_PCBang(szCMS);
}

void CBillingManager::Expire_IPOverflow(char *szID)
{
  m_pBill->Expire_IPOverflow(szID);
}

void CBillingManager::Change_Primium(char *szID, bool bResult)
{
  m_pBill->Change_Primium(szID, bResult);
}

void CBillingManager::SendMsg_ZoneAliveCheck(unsigned int dwData)
{
  m_pBill->SendMsg_ZoneAliveCheck(dwData);
}
