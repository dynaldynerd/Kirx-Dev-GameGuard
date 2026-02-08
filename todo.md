# Init gaps from CMainThread::Init audit

## CBossMonsterScheduleSystem.cpp
- CBossMonsterScheduleSystem::Init (stub: ignores mapOper, returns true)

## CNationSettingManager.cpp
- CNationSettingManager::Init (stub; IDA uses CNationSettingFactoryGroup::Init/Create + CNationSettingData::Init)

## CNationSettingFactoryGroup.cpp / CNationSettingFactoryGroup.h
- CNationSettingFactoryGroup::Init (missing)
- CNationSettingFactoryGroup::Create (missing)
- CNationSettingFactoryGroup::m_kPool (CHashMapPtrPool<int, CNationSettingFactory>) missing in class layout

## CHashMapPtrPool.h / CHashMapPtrPool.cpp
- Needs templated CHashMapPtrPool<TKey, TValue> with regist/get/findkey (IDA uses CHashMapPtrPool<int, CNationSettingFactory>); current non-template class is a stub and conflicts with template in CNationCodeStrTable.h

## CNationSettingData.h / CNationSettingData.cpp
- CNationSettingData::Init (missing; IDA expects virtual Init())
- CNationSettingData*::CreateWorker (missing overrides: BR/CN/ES/GB/ID/JP/KR/PH/RU/TH/TW/US)

## CCashDbWorker*.cpp / CCashDbWorker*.h
- CCashDbWorker* classes are empty and do not inherit CashDbWorker
- Region workers missing core implementation (Initialize/_init_database/DoWork/CompleteWork/etc per IDA)

## CashDbWorker.h
- Interface incomplete vs IDA (needs additional virtuals beyond Initialize; see CashDbWorker__* in IDA)

## CCashDBWorkManager.h / CCashDBWorkManager.cpp
- Constructor missing (IDA sets m_pWorker = &CCashDbWorkerNULL::ms_NULL)

## CMainThread.cpp / CNetworkEX.cpp / CNetProcess.cpp
- CNetworkEX::DataAnalysis (IDA dispatch to Client/Account/Web/Billing line analysis)
- CNetworkEX::ClientLineAnalysis (missing)
- CNetworkEX::AccountLineAnalysis (missing)
- CNetworkEX::BillingLineAnalysis (missing)
- CNetworkEX::WebAgentLineAnalysis (missing)
