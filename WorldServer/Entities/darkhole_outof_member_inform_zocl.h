#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) _darkhole_outof_member_inform_zocl
{
  unsigned int dwMemberSerial;
  bool bDisconnect;

  unsigned __int16 size() const;
};
#pragma pack(pop)
