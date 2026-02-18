#pragma once

#include "IdaCompat.h"
#include <Windows.h>

struct _map_rate;
class CSurface;

/* 2898 */
struct _map_rate
{
  float fSide[2];
  int nStandard;
  int nPartner;
};

/* 3009 */
class  CMapExtend
{
public:
  CMapExtend();
  explicit CMapExtend(CSurface **pSF);
  ~CMapExtend();

  void Init(CSurface **pSF);

  bool m_bExtendMode;

private:
  tagPOINT m_ptStartMap;
  tagPOINT m_ptEndMap;
  tagPOINT m_ptCenter;
  tagRECT m_rcExtend;
  tagPOINT m_ptStartScreen;
  tagPOINT m_ptEndScreen;
  tagPOINT m_ptMoveScreen;
  tagSIZE m_sizeExtend;
  HPEN__ *m_hPen;
  CSurface **m_pSF;
  _map_rate m_Rate;
  int m_bSetArea;
  int m_bMove;
};
