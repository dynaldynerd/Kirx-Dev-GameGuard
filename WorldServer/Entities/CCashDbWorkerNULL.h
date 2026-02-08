#pragma once

#include "IdaCompat.h"

#include "CashDbWorker.h"

class __cppobj CCashDbWorkerNULL : public CashDbWorker
{
public:
  CCashDbWorkerNULL();
  ~CCashDbWorkerNULL() override = default;

  static CCashDbWorkerNULL ms_NULL;

  void DoWork() override;
  void Release() override;
  bool Initialize() override;
  void CompleteWork() override;
  void GetUseCashQueryStr(_param_cash_update *rParam, int nIdx, char *wszQuery, unsigned __int64 tBufferSize) override;
  int _wait_tsk_cash_select(Task *pkTsk) override;
  int _wait_tsk_cash_update(Task *pkTsk) override;
  int _wait_tsk_cash_rollback(Task *pkTsk) override;
  void _all_rollback(const _param_cash_update *psheet) override;
  bool _init_database() override;
  int ConvertErrorCode(char state) override;

  int _wait_tsk_cash_buy_dblog(Task *pkTsk);
  int _wait_tst_cash_total_selling_select(Task *pkTsk);
};
