#pragma once

#include "IdaCompat.h"

struct _unmannedtrader_re_regist_result_zocl
{
  struct __list
  {
    unsigned __int8 byRet;
    bool bRegist;
    unsigned __int16 wItemSerial;
    unsigned int dwPrice;
    unsigned int dwRegedSerial;
    unsigned int dwListIndex;
    unsigned int dwTax;
  };

  unsigned int dwLeftDalant;
  unsigned __int8 byNum;
  __list List[10];

  __int64 size();
};

inline __int64 _unmannedtrader_re_regist_result_zocl::size()
{
  if (byNum > 0xAu)
  {
    byNum = 0;
  }
  return 205 - 20LL * (10 - byNum);
}
