#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _notify_remain_coupon_zocl
{
  unsigned __int8 byRemainCoupon;

  unsigned __int16 size() const
  {
    return static_cast<unsigned __int16>(sizeof(*this));
  }
};
#pragma pack(pop)
