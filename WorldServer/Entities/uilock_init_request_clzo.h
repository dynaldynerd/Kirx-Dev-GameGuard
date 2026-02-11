#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _uilock_init_request_clzo
{
  char uszUILockPW[13];
  char uszUILockPW_Confirm[13];
  unsigned __int8 byUILock_HintIndex;
  char uszUILock_HintAnswer[17];
};
#pragma pack(pop)
