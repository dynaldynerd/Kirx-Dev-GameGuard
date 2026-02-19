#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"

#pragma pack(push, 1)
struct  _enter_world_request_wrac
{
  _GLBID gidGlobal;
  _CLID idLocal;
  unsigned int ulConnectIP;

  __int64 size() const;
};
#pragma pack(pop)

inline __int64 _enter_world_request_wrac::size() const
{
  return 18LL;
}

