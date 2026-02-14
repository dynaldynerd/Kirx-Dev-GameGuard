#pragma once

#include "IdaCompat.h"

struct __cppobj _apex_id
{
  explicit _apex_id(unsigned __int8 byId);
  _apex_id *operator&();

  unsigned __int8 m_byID;
};

inline _apex_id::_apex_id(unsigned __int8 byId) : m_byID(byId)
{
}

inline _apex_id *_apex_id::operator&()
{
  return this;
}
