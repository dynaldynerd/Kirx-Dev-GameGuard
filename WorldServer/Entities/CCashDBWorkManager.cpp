#include "pch.h"

#include "CCashDBWorkManager.h"

#include "CashDbWorker.h"
#include "CNationSettingManager.h"

bool CCashDBWorkManager::Initialize()
{
  CNationSettingManager *nationSetting = CTSingleton<CNationSettingManager>::Instance();
  CashDbWorker *worker = nationSetting->CreateWorker();
  if (!worker)
  {
    return false;
  }

  m_pWorker = worker;
  nationSetting = CTSingleton<CNationSettingManager>::Instance();
  return nationSetting->IsCashDBUseExtRef() || m_pWorker->Initialize();
}
