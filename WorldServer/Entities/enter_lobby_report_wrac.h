#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"

struct _enter_lobby_report_wrac
{
  _GLBID gidGlobal;

  unsigned __int16 size() const;
};

inline unsigned __int16 _enter_lobby_report_wrac::size() const
{
  return 8;
}
