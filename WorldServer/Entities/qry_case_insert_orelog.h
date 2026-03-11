#pragma once

#include "IdaCompat.h"

struct  _qry_case_insert_orelog
{
  unsigned __int8 byType;
  char szLogDate[17];
  int nLiveUsercnt;
  unsigned int dwOreRemain;
  unsigned int dwTAmount;

  _qry_case_insert_orelog()
  {
    std::memset(this, 0, sizeof(*this));
  }
};
