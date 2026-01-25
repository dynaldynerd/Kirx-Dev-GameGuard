#include "pch.h"

#include "CPvpCashMng.h"

CPvpCashMng *CPvpCashMng::Instance()
{
  static CPvpCashMng s_instance;
  return &s_instance;
}

bool CPvpCashMng::LoadData()
{
  return true;
}
