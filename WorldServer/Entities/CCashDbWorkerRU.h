#pragma once

#include "IdaCompat.h"

#include "CashDbWorker.h"

class CRusiaBillingMgr;

class __cppobj CCashDbWorkerRU : public CashDbWorker
{
public:
  CCashDbWorkerRU();
  ~CCashDbWorkerRU() override;

  void Release() override;

protected:
  bool _init_database() override;
  int _wait_tsk_cash_select(Task *pkTsk) override;
  int _wait_tsk_cash_update(Task *pkTsk) override;
  int _wait_tsk_cash_rollback(Task *pkTsk) override;

private:
  CRusiaBillingMgr *_pkBill;
};

static_assert(sizeof(CCashDbWorkerRU) == 0x230, "CCashDbWorkerRU size mismatch");
