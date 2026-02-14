#pragma once

#include "IdaCompat.h"

#include <Windows.h>

class __cppobj CSurface
{
public:
  CSurface();
  ~CSurface();

  void *m_pdds;
  BOOL m_bColorKeyed;
};
