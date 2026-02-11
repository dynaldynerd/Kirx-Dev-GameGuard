#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _uilock_update_info_request_clzo
{
  char uszUILockPW_Old[13];
  char uszUILockPW[13];
  char uszUILockPW_Confirm[13];
  unsigned __int8 byHintIndex;
  char uszHintAnswer[17];
};
#pragma pack(pop)
