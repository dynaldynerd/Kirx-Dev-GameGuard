#pragma once

#include "IdaCompat.h"
#include "Worker.h"
#include "CLogFile.h"
#include "param_cash.h"

class CRFCashItemDatabase;
class Task;
class CPlayer;

class __cppobj CashDbWorker : public Worker
{
public:
  CashDbWorker();
  ~CashDbWorker() override;

  void DoWork() override;

  virtual void Release();
  virtual bool Initialize();
  virtual void CompleteWork();
  virtual void GetUseCashQueryStr(_param_cash_update *rParam, int nIdx, char *wszQuery, unsigned __int64 tBufferSize);
  virtual int _wait_tsk_cash_select(Task *pkTsk);
  virtual int _wait_tsk_cash_update(Task *pkTsk);
  virtual int _wait_tsk_cash_rollback(Task *pkTsk);
  virtual void _all_rollback(const _param_cash_update *psheet);
  virtual bool _init_database();
  virtual int ConvertErrorCode(char state);

  bool PushTask(int nTaskCode, unsigned __int8 *p, unsigned __int64 size);
  bool IsNULL();
  bool GetBillingDBConnectionStatus();
  void SendMsgSucceedBuy(unsigned __int16 wSock, const _param_cash_update *sheet);

protected:
  bool _init_loggers();
  int _wait_tsk_cash_buy_dblog(Task *pkTsk);
  int _wait_tst_cash_total_selling_select(Task *pkTsk);
  void _complete_tsk_cash_select(Task *pkTsk);
  void _complete_tsk_cash_update(Task *pkTsk);
  void _complete_tsk_cash_rollback(Task *pkTsk);
  void _complete_tsk_cashitem_buy_dblog(Task *pkTsk);
  void _complete_tsk_cash_total_selling_select(Task *pkTsk);

  CPlayer *_get_player(unsigned __int16 wSock, unsigned int dwAvator);
  bool _insert_to_inven(CPlayer *pOne, _param_cash_update::__item *pGII);
  void _delete_from_inven(CPlayer *pOne, const _param_cash_update *psheet, int nNum);

public:
  CLogFile _kLogger[2];
  CRFCashItemDatabase *_pkDb;
};
