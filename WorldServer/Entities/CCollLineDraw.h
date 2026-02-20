#pragma once

#include "IdaCompat.h"

class CMapData;
class CSurface;
struct _TOOL_COL_LINE;

struct _coll_point
{
  void InitPoint(CMapData *pMap, float *pPos, CRect *prcWnd);

  float m_fPosAbs[2];
  float m_fScrNor[2];
  float m_fReAbs[2];
  float m_fScrExt[2];
};

class  CCollLineDraw
{
public:
  CCollLineDraw();
  virtual ~CCollLineDraw();

  static void InitPen();
  static void DeletePen();

  char InitLine(CMapData *pMap, CRect *prcWnd);
  char Draw(CSurface *pSF, CRect *prcArea);
  char DrawEx(int nLineIndex, HDC *pDC, CRect *prcArea);

  static HPEN s_hPen;

private:
  CMapData *m_pMap;
  CRect m_rcWnd;
  int m_nPointNum;
  _coll_point *m_Point;
  int m_nLineNum;
  _TOOL_COL_LINE *m_pLine;
};

