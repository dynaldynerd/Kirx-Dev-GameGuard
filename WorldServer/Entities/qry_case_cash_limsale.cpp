#include "pch.h"

#include "qry_case_cash_limsale.h"

qry_case_cash_limsale::qry_case_cash_limsale()
{
  std::memset(this, 0, sizeof(qry_case_cash_limsale));
}

int qry_case_cash_limsale::size()
{
  // narrowing cast for thunk return parity
  return static_cast<int>(328LL);
}
