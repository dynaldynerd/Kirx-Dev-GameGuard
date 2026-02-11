#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _guild_manage_request_clzo
{
  _guild_manage_request_clzo();
  __int64 size();

  unsigned __int8 byManageType;
  unsigned int dwManageDst;
  unsigned int dwManageObj1;
  unsigned int dwManageObj2;
  unsigned int dwManageObj3;
};
#pragma pack(pop)
