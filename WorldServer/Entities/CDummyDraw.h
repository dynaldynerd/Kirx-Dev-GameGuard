#pragma once

#include "IdaCompat.h"

#include <windows.h>

class CMapData;
class CRect;
class CSurface;

class  CDummyDraw
{
public:
  CDummyDraw();
  virtual ~CDummyDraw();

  static void InitPen();
  static void DeletePen();

  void SetDummyRange(CMapData *pMap, float *pLT, float *pRB, float *pRT, float *pLB, int nType, CRect *prcWnd);
  char CalcAbsExt(CRect *prcArea);
  void CalcScrExt(CRect *prcArea, CRect *prcWnd);
  __int64 Draw(CSurface *pSF, CRect *prcArea);

  static HPEN s_hPen[10];
  static HPEN s_hDirection;

  CMapData *m_pMap;
  int m_nType;
  float m_fPosAbs[8];
  float m_fReAbs[8];
  float m_fScrNor[8];
  float m_fScrExt[8];
};

