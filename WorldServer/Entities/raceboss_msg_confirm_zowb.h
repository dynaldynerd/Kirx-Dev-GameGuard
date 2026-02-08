#pragma once

#include "IdaCompat.h"

struct __unaligned __declspec(align(1)) _raceboss_msg_confirm_zowb
{
  int nCountIndex;
  int nWorldCode;
  unsigned __int8 byRaceCode;
  char wszMasterName[17];
  char wszMsg[255];

  unsigned __int16 size() const;
};

inline unsigned __int16 _raceboss_msg_confirm_zowb::size() const
{
  return 0x119;
}
