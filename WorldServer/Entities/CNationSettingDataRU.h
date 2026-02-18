#pragma once

#include "IdaCompat.h"

#include "CNationSettingData.h"

class  CNationSettingDataRU : public CNationSettingData
{
public:
  CNationSettingDataRU();

  int Init() override;
  CashDbWorker *CreateWorker() override;
  CBilling *CreateBilling() override;
  const char *GetItemName(_NameTxt_fld *pFld) override;
  void SendCashDBDSNRequest() override;
  bool ReadSystemPass() override;
  int GetCashItemPrice(_CashShop_str_fld *pFld) override;
};
