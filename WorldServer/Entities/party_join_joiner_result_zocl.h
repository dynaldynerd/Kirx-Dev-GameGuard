#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) _party_join_joiner_result_zocl
{
  struct __unaligned __declspec(align(1)) _list
  {
    unsigned __int16 wIndex;
    unsigned int dwSerial;
    char wszAvatorName[17];
  };

  unsigned __int8 byLootShareMode;
  unsigned __int8 byListNum;
  _list List[8];

  _party_join_joiner_result_zocl();
  __int64 size();
};
#pragma pack(pop)

inline _party_join_joiner_result_zocl::_party_join_joiner_result_zocl() : byListNum(0) {}

inline __int64 _party_join_joiner_result_zocl::size()
{
  if (byListNum > 8u)
  {
    byListNum = 0;
  }
  return 186 - 23LL * (8 - byListNum);
}
