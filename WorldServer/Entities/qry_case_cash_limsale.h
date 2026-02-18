#pragma once

#include "IdaCompat.h"

struct _db_cash_limited_sale
{
  struct _db_cash_limited_info
  {
    int nLimcode;
    int nLimcount;
  };

  unsigned __int8 byDck;
  unsigned __int8 byLimited_sale_num;
  _db_cash_limited_info List[20];
};

struct  qry_case_cash_limsale
{
  _db_cash_limited_sale NewSale;
  _db_cash_limited_sale OldSale;

  qry_case_cash_limsale();
  __int64 size();
};
