#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned _racebosssms_fromweb_send_error_result_zowb
{
  unsigned __int8 byErrCode;
  unsigned int dwWebSendDBID;

  unsigned __int16 size() const;
};
#pragma pack(pop)

inline unsigned __int16 _racebosssms_fromweb_send_error_result_zowb::size() const
{
  return 5;
}
