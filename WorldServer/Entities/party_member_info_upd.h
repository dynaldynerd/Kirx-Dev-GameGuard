#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) _party_member_info_upd
{
  struct __unaligned __declspec(align(1)) _EFFECT
  {
    unsigned __int16 wEffectCode;
    unsigned __int8 byEffectLv;
  };

  unsigned int dwMemSerial;
  unsigned __int16 wHPRate;
  unsigned __int16 wFPRate;
  unsigned __int16 wSPRate;
  unsigned __int8 byLv;
  unsigned __int8 byMapCode;
  __int16 zPos[2];
  unsigned __int8 byContEffectNum;
  _EFFECT Effect[16];

  _party_member_info_upd();
  __int64 size();
};
#pragma pack(pop)

inline _party_member_info_upd::_party_member_info_upd() : byContEffectNum(0) {}

inline __int64 _party_member_info_upd::size()
{
  if (byContEffectNum > 0x10u)
  {
    byContEffectNum = 0;
  }
  return 65 - 3LL * (16 - byContEffectNum);
}
