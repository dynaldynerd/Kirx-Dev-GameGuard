#pragma once

#include "IdaCompat.h"

struct __unaligned __declspec(align(1)) _racebosssms_fromweb_send_error_result_zowb
{
  unsigned __int8 byErrCode;
  unsigned int dwWebSendDBID;

  unsigned __int16 size() const;
};

inline unsigned __int16 _racebosssms_fromweb_send_error_result_zowb::size() const
{
  return 5;
}
