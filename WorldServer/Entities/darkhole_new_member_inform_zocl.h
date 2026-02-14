#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(2)) _darkhole_new_member_inform_zocl
{
  unsigned int dwNewMemberSerial;
  char wszNewMemberName[17];
  bool bReconnect;

  unsigned __int16 size() const;
};
#pragma pack(pop)

inline unsigned __int16 _darkhole_new_member_inform_zocl::size() const
{
  return 22;
}
