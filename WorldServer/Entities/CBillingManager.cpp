#include "pch.h"

#include "CBillingManager.h"

#include "CBillingNULL.h"
#include "CUserDB.h"
#include "CNationSettingManager.h"

CBillingManager::CBillingManager()
  : m_pBill(&CBillingNULL::ms_NULL)
{
}

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

void CBillingManager::Logout(CUserDB *pUserDB)
{
  m_pBill->Logout(pUserDB);
}
