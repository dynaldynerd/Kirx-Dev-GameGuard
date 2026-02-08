#include "pch.h"

#include "CCashDBWorkManager.h"

#include "CCashDbWorkerNULL.h"
#include "CashDbWorker.h"
#include "CNationSettingManager.h"

CCashDBWorkManager::CCashDBWorkManager()
  : m_pWorker(&CCashDbWorkerNULL::ms_NULL)
{
}

bool CCashDBWorkManager::Initialize()
{
  auto *nationSetting = CTSingleton<CNationSettingManager>::Instance();
  CashDbWorker *worker = nationSetting->CreateWorker();
  if (!worker)
  {
    return false;
  }

  m_pWorker = worker;
  nationSetting = CTSingleton<CNationSettingManager>::Instance();
  return nationSetting->IsCashDBUseExtRef() || m_pWorker->Initialize();
}

void CCashDBWorkManager::Start()
{
  m_pWorker->Start();
}

bool CCashDBWorkManager::PushTask(int nTaskCode, unsigned __int8 *p, unsigned __int64 size)
{
  return m_pWorker->PushTask(nTaskCode, p, size);
}

void CCashDBWorkManager::GetUseCashQueryStr(
  _param_cash_update *rParam,
  int nIdx,
  char *wszQuery,
  unsigned __int64 tBufferSize)
{
  m_pWorker->GetUseCashQueryStr(rParam, nIdx, wszQuery, tBufferSize);
}

void CCashDBWorkManager::CompleteWork()
{
  m_pWorker->CompleteWork();
}

bool CCashDBWorkManager::InitializeWorker()
{
  return !m_pWorker->IsNULL() && m_pWorker->Initialize();
}

bool CCashDBWorkManager::GetBillingDBConnectionStatus()
{
  return m_pWorker->GetBillingDBConnectionStatus();
}
