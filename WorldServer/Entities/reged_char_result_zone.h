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
  __int64 size();
};
#pragma pack(pop)

inline _reged_char_result_zone::_reged_char_result_zone()
  : byCharNum(0)
{
}

inline __int64 _reged_char_result_zone::size()
{
  if (byCharNum > 3u)
  {
    byCharNum = 0;
  }
  return 221 - 69LL * (3 - byCharNum);
}
