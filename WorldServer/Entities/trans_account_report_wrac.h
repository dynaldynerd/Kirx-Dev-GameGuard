#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) _trans_account_report_wrac
{
  _GLBID gidGlobal;
  unsigned __int8 byRetCode;

  unsigned __int16 size() const;
};
#pragma pack(pop)

inline unsigned __int16 _trans_account_report_wrac::size() const
{
  return 9;
}
