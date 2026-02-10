#pragma once

#include "IdaCompat.h"

struct _worlddb_rankinguild_info
{
  struct __worlddb_rankinguild_data
  {
    unsigned int dwSerial;
    unsigned __int8 byGrade;
    unsigned __int8 byLv;
    unsigned int dwPvpPoint;
  };

  unsigned __int16 wRecordCount;
  __worlddb_rankinguild_data MemberData[50];
};
