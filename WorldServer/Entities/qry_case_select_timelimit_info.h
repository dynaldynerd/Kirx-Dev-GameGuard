#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __declspec(align(4)) _qry_case_select_timelimit_info
{
  unsigned __int16 wIndex;
  unsigned int dwAccountSerial;
  unsigned int dwLastLogoutTime;

  unsigned __int16 size() const;
};
#pragma pack(pop)

inline unsigned __int16 _qry_case_select_timelimit_info::size() const
{
  return 12;
}
