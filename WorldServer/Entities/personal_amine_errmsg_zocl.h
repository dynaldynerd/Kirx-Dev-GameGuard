#pragma once

#include "IdaCompat.h"

struct __cppobj __unaligned __declspec(align(1)) _personal_amine_errmsg_zocl
{
  unsigned __int8 byErrCode;

  _personal_amine_errmsg_zocl();
  __int64 size() const;
};

inline _personal_amine_errmsg_zocl::_personal_amine_errmsg_zocl()
{
  byErrCode = static_cast<unsigned __int8>(-1);
}

inline __int64 _personal_amine_errmsg_zocl::size() const
{
  return 1;
}
