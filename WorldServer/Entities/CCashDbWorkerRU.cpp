#include "pch.h"

#include "CCashDbWorkerRU.h"

#include <new>

#include "CNationSettingManager.h"
#include "CRusiaBillingMgr.h"
#include "Task.h"

CCashDbWorkerRU::CCashDbWorkerRU()
  : CashDbWorker()
{
}

CCashDbWorkerRU::~CCashDbWorkerRU() = default;

void CCashDbWorkerRU::Release()
{
  if (_pkBill)
  {
    _pkBill->Release();
    _pkBill = nullptr;
  }
}

bool CCashDbWorkerRU::_init_database()
{
  auto *bill = static_cast<CRusiaBillingMgr *>(operator new(sizeof(CRusiaBillingMgr)));
  if (bill)
  {
    new (bill) CRusiaBillingMgr();
  }

  _pkBill = bill;
  if (!_pkBill)
  {
    return false;
  }

  if (!_pkBill->Instance())
  {
    return false;
  }

  auto *nationSetting = CTSingleton<CNationSettingManager>::Instance();
  nationSetting->SetCashDBInitState();
  return true;
}

int CCashDbWorkerRU::_wait_tsk_cash_select(Task *pkTsk)
{
  auto *rParam = reinterpret_cast<_param_cash_select *>(pkTsk->GetTaskBuf());
  return _pkBill->CallFunc_RFOnline_Auth(rParam) != 0;
}

int CCashDbWorkerRU::_wait_tsk_cash_update(Task *pkTsk)
{
  auto *rParam = reinterpret_cast<_param_cash_update *>(pkTsk->GetTaskBuf());
  for (int nIdx = 0; nIdx < rParam->in_nNum10; ++nIdx)
  {
    auto *item = &rParam->in_item[nIdx];
    if (_pkBill->CallFunc_Item_Buy(rParam, nIdx))
    {
      return 1;
    }
    item->out_cState = 0;
  }
  return 0;
}

int CCashDbWorkerRU::_wait_tsk_cash_rollback(Task *pkTsk)
{
  auto *sheet = reinterpret_cast<_param_cash_rollback *>(pkTsk->GetTaskBuf());
  for (int j = 0; j < sheet->in_byNum; ++j)
  {
    auto *entry = &sheet->data[j];
    if (_pkBill->CallFunc_Item_Cancel(entry, reinterpret_cast<char *>(sheet) + 16))
    {
      entry->out_nCashAmount = 0;
      sheet->out_nCashAmount = 0;
      entry->out_cStatus = 1;
    }
    else
    {
      entry->out_cStatus = 0;
      if (entry->out_cStatus)
      {
        entry->out_nCashAmount = 0;
        sheet->out_nCashAmount = 0;
      }
      else
      {
        sheet->out_nCashAmount = entry->out_nCashAmount;
      }
    }
  }
  return 0;
}
