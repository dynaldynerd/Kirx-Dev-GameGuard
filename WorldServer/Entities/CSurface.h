#pragma once

#include "IdaCompat.h"

#include <Windows.h>

class  CSurface
{
public:
  CSurface();
  ~CSurface();

  void *m_pdds;
  BOOL m_bColorKeyed;
};
