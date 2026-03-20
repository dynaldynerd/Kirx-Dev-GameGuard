#pragma once

#include "IdaCompat.h"

#include "CMainThread.h"

#pragma pack(push, 1)
struct   _reged_char_result_zone
{
  unsigned __int8 byRetCode;
  unsigned __int8 byCharNum;
  int iLockType[3];
  _REGED_AVATOR_DB RegedList[3];

  _reged_char_result_zone();
  int size();
};
#pragma pack(pop)

inline _reged_char_result_zone::_reged_char_result_zone()
  : byCharNum(0)
{
}

inline int _reged_char_result_zone::size()
{
  if (byCharNum > 3u)
  {
    byCharNum = 0;
  }
  // narrowing cast for thunk return parity
  return static_cast<int>(221 - 69LL * (3 - byCharNum));
}
