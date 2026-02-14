#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"

#pragma pack(push, 1)
struct _enter_lobby_report_wrac
{
  _GLBID gidGlobal;

  unsigned __int16 size() const;
};
#pragma pack(pop)

inline unsigned __int16 _enter_lobby_report_wrac::size() const
{
  return 8;
}
