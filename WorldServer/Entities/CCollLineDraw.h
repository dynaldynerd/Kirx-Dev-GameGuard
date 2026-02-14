#pragma once

#include "IdaCompat.h"

class CMapData;
struct _TOOL_COL_LINE;

struct _coll_point
{
  float m_fPosAbs[2];
  float m_fScrNor[2];
  float m_fReAbs[2];
  float m_fScrExt[2];
};

class __cppobj CCollLineDraw
{
public:
  CCollLineDraw();
  virtual ~CCollLineDraw();

private:
  CMapData *m_pMap;
  CRect m_rcWnd;
  int m_nPointNum;
  _coll_point *m_Point;
  int m_nLineNum;
  _TOOL_COL_LINE *m_pLine;
};

