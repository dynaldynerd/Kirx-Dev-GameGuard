#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct  _racebosssms_fromweb_send_error_result_zowb
{
  unsigned __int8 byErrCode;
  unsigned int dwWebSendDBID;

  int size() const;
};
#pragma pack(pop)

inline int _racebosssms_fromweb_send_error_result_zowb::size() const
{
  return 5;
}
