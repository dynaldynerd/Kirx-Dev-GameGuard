#pragma once

#include "IdaCompat.h"

class CMapData;

class __cppobj CDummyDraw
{
public:
  CDummyDraw();
  virtual ~CDummyDraw();

  CMapData *m_pMap;
  int m_nType;
  float m_fPosAbs[8];
  float m_fReAbs[8];
  float m_fScrNor[8];
  float m_fScrExt[8];
};

