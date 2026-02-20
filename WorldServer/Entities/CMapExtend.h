#pragma once

#include "IdaCompat.h"
#include <afxwin.h>

struct _map_rate;
class CMapData;
class CSurface;

/* 2898 */
struct _map_rate
{
  float fSide[2];
  int nStandard;
  int nPartner;

  void Setting(int x, int y);
};

/* 3009 */
class  CMapExtend
{
public:
  CMapExtend();
  explicit CMapExtend(CSurface **pSF);
  ~CMapExtend();

  void Init(CSurface **pSF);
  void DrawRect();
  void StartScreenPoint(CPoint *pt, CMapData *pMap, CRect *rcWnd);
  void MoveScreenPoint(CPoint *pt);
  bool EndScreenPoint(CSize *szMap);
  void ConvertToMap(CSize *szMap);
  CSize *GetExtendSize();
  void ScrollMapLeft(int nInterval);
  void ScrollMapRight(int nMapX, int nInterval);
  void ScrollMapUp(int nInterval);
  void ScrollMapDown(int nMapY, int nInterval);

  bool m_bExtendMode;
  CPoint m_ptStartMap;
  CPoint m_ptEndMap;
  CPoint m_ptCenter;
  CRect m_rcExtend;
  CPoint m_ptStartScreen;
  CPoint m_ptEndScreen;
  CPoint m_ptMoveScreen;
  CSize m_sizeExtend;
  HPEN__ *m_hPen;
  CSurface **m_pSF;
  _map_rate m_Rate;
  int m_bSetArea;
  int m_bMove;
};
