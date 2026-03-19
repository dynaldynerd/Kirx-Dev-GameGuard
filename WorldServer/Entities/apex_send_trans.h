#pragma once

#include "IdaCompat.h"

struct _apex_send_trans
{
  int m_nRet;

  int size() const;
};

inline int _apex_send_trans::size() const
{
  return 4;
}
