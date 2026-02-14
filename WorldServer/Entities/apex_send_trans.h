#pragma once

#include "IdaCompat.h"

struct _apex_send_trans
{
  int m_nRet;

  __int64 size() const;
};

inline __int64 _apex_send_trans::size() const
{
  return 4;
}
