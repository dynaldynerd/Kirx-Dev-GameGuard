#pragma once

#include "IdaCompat.h"

#include "CMainThread.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(2)) _select_avator_report_wrac
{
  _GLBID gidGlobal;
  char wszCharName[17];
  unsigned int dwAvatorSerial;
  unsigned __int8 byLevel;

  __int64 size() const;
};
#pragma pack(pop)

inline __int64 _select_avator_report_wrac::size() const
{
  return 30;
}
