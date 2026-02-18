#pragma once

#include "IdaCompat.h"
#include "CTSingleton.h"
#include "param_cash.h"

class CashDbWorker;

class  CCashDBWorkManager : public CTSingleton<CCashDBWorkManager>
{
public:
  CCashDBWorkManager();
  ~CCashDBWorkManager();

  bool Initialize();
  void Start();
  bool PushTask(int nTaskCode, unsigned __int8 *p, unsigned __int64 size);
  void GetUseCashQueryStr(_param_cash_update *rParam, int nIdx, char *wszQuery, unsigned __int64 tBufferSize);
  void CompleteWork();
  bool InitializeWorker();
  bool GetBillingDBConnectionStatus();

  CashDbWorker *m_pWorker;
};
