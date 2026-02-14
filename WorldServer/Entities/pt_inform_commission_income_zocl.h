#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) _pt_inform_commission_income_zocl
{
  unsigned __int8 byRet;
  unsigned __int8 byMonth;
  unsigned __int8 byDay;
  unsigned int dwCommission;

  unsigned __int16 size() const;
};
#pragma pack(pop)

inline unsigned __int16 _pt_inform_commission_income_zocl::size() const
{
  return static_cast<unsigned __int16>(sizeof(_pt_inform_commission_income_zocl));
}
