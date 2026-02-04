#include "pch.h"

#include "CashItemRemoteStore.h"

#include "CashShop_fld.h"

std::unordered_map<int, const _CashShop_fld *> CashItemRemoteStore::s_cashTables[0x25];

CashItemRemoteStore *CashItemRemoteStore::Instance()
{
  static CashItemRemoteStore s_instance;
  return &s_instance;
}

bool CashItemRemoteStore::Initialize()
{
  return true;
}

const _CashShop_fld *CashItemRemoteStore::FindCashRec(unsigned int nTbl, int nIdx)
{
  if (nTbl > 0x24)
  {
    return nullptr;
  }

  auto &table = s_cashTables[nTbl];
  auto it = table.find(nIdx);
  if (it == table.end())
  {
    return nullptr;
  }
  return it->second;
}
