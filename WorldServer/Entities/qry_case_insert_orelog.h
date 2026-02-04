#pragma once

#include "IdaCompat.h"

struct __cppobj _qry_case_insert_orelog
{
  unsigned __int8 byType;
  char szLogDate[16];
  int nLiveUsercnt;
  unsigned int dwOreRemain;
  unsigned int dwTAmount;

  _qry_case_insert_orelog()
  {
    memset_0(this, 0, sizeof(*this));
  }
};
