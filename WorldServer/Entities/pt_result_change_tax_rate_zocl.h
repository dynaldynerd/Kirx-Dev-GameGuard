#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _pt_result_change_tax_rate_zocl
{
  unsigned __int8 byRet;
  unsigned __int8 byNextTax;

  unsigned __int16 size() const;
};
#pragma pack(pop)

inline unsigned __int16 _pt_result_change_tax_rate_zocl::size() const
{
  return 2;
}
