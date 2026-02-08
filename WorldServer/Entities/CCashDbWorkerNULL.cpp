#include "pch.h"

#include "CCashDbWorkerNULL.h"

CCashDbWorkerNULL CCashDbWorkerNULL::ms_NULL;

CCashDbWorkerNULL::CCashDbWorkerNULL()
  : CashDbWorker()
{
}

void CCashDbWorkerNULL::DoWork()
{
  // this is not a stub
}

void CCashDbWorkerNULL::Release()
{
  // this is not a stub
}

bool CCashDbWorkerNULL::Initialize()
{
  // this is not a stub
  return false;
}

void CCashDbWorkerNULL::CompleteWork()
{
  // this is not a stub
}

void CCashDbWorkerNULL::GetUseCashQueryStr(
  _param_cash_update *rParam,
  int nIdx,
  char *wszQuery,
  unsigned __int64 tBufferSize)
{
  (void)rParam;
  (void)nIdx;
  (void)wszQuery;
  (void)tBufferSize;
  // this is not a stub
}

int CCashDbWorkerNULL::_wait_tsk_cash_select(Task *pkTsk)
{
  (void)pkTsk;
  // this is not a stub
  return 1;
}

int CCashDbWorkerNULL::_wait_tsk_cash_update(Task *pkTsk)
{
  (void)pkTsk;
  // this is not a stub
  return 1;
}

int CCashDbWorkerNULL::_wait_tsk_cash_rollback(Task *pkTsk)
{
  (void)pkTsk;
  // this is not a stub
  return 1;
}

void CCashDbWorkerNULL::_all_rollback(const _param_cash_update *psheet)
{
  (void)psheet;
  // this is not a stub
}

bool CCashDbWorkerNULL::_init_database()
{
  // this is not a stub
  return false;
}

int CCashDbWorkerNULL::ConvertErrorCode(char state)
{
  (void)state;
  // this is not a stub
  return 1;
}

int CCashDbWorkerNULL::_wait_tsk_cash_buy_dblog(Task *pkTsk)
{
  (void)pkTsk;
  // this is not a stub
  return 1;
}

int CCashDbWorkerNULL::_wait_tst_cash_total_selling_select(Task *pkTsk)
{
  (void)pkTsk;
  // this is not a stub
  return 1;
}
