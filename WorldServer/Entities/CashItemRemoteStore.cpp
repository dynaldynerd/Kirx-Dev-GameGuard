#include "pch.h"

#include "CashItemRemoteStore.h"

CashItemRemoteStore *CashItemRemoteStore::Instance()
{
  static CashItemRemoteStore s_instance;
  return &s_instance;
}

bool CashItemRemoteStore::Initialize()
{
  return true;
}
