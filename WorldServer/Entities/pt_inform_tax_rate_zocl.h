#pragma once

#include "IdaCompat.h"

struct _pt_inform_tax_rate_zocl
{
  unsigned __int8 byCurrTax;
  unsigned __int8 byNextTax;

  _pt_inform_tax_rate_zocl();
  __int64 size() const;
};

inline _pt_inform_tax_rate_zocl::_pt_inform_tax_rate_zocl()
{
  memset_0(this, 0, sizeof(*this));
}

inline __int64 _pt_inform_tax_rate_zocl::size() const
{
  return 2;
}
