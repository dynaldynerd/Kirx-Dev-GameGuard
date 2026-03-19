#pragma once

#include "CRFNewDatabase.h"
#include "param_cash.h"

class  CRFCashItemDatabase : public CRFNewDatabase
{
public:
  CRFCashItemDatabase();
  ~CRFCashItemDatabase() override;

  int CallProc_RFOnlineAuth(_param_cash_select *rParam);
  int CallProc_RFOnlineUse(_param_cash_update *rParam, int nIdx);
  int CallProc_RFONLINE_Cancel(_param_cash_rollback::__list *list);
  bool CallProc_InsertCashItemLog(
    unsigned int dwSerial,
    unsigned __int8 byLv,
    char *szItemCode,
    char *szItemName,
    unsigned __int8 byNum,
    unsigned int dwCost);
  int CallProc_RFOnlineAvg_Event(unsigned int *iAvgCashSelling);
  void dhRExtractSubString(char *szSub, char *szFull, int n);
};
