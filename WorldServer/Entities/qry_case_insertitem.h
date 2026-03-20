#pragma once

#include "IdaCompat.h"

struct _qry_case_insertitem
{
  unsigned int dwSerial;
  unsigned int dwItemCodeK;
  unsigned int dwItemCodeD;
  unsigned int dwItemCodeU;
  unsigned __int8 byType;
  unsigned int dwRemainTime;
  unsigned __int64 lnUID;

  int size() const;
};

inline int _qry_case_insertitem::size() const
{
  return 32;
}
