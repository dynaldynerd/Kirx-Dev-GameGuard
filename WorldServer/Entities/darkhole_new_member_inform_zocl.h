#pragma once

#include "IdaCompat.h"

struct __unaligned __declspec(align(2)) _darkhole_new_member_inform_zocl
{
  unsigned int dwNewMemberSerial;
  char wszNewMemberName[17];
  bool bReconnect;

  unsigned __int16 size() const;
};

inline unsigned __int16 _darkhole_new_member_inform_zocl::size() const
{
  return 22;
}
