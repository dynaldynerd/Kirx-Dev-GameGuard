#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _party_join_member_result_zocl
{
  unsigned int dwJoinerSerial;
  char wszJoinerName[17];
  unsigned __int8 byLootShareMode;
  unsigned __int16 wIndex;

  __int64 size() const;
};
#pragma pack(pop)

inline __int64 _party_join_member_result_zocl::size() const
{
  return static_cast<__int64>(sizeof(_party_join_member_result_zocl));
}
