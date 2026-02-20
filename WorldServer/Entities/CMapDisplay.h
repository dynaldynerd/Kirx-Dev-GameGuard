#pragma once

#include "IdaCompat.h"

#include "CCollLineDraw.h"
#include "CDummyDraw.h"
#include "CDisplay.h"
#include "CMapExtend.h"
#include "CMyTimer.h"
#include "CSurface.h"

class CMapData;
class CGameObject;
class CMonster;

class  CMapDisplay : public CDisplay
{
public:
  CMapDisplay();
  ~CMapDisplay() override;

  char OnDisplay(CMapData *pMap, unsigned __int16 wLayerIndex);
  char ChangeMap(CMapData *pMap);
  char ChangeLayer(unsigned __int16 wLayerIndex);
  void DrawDisplay();
  void DrawMap();
  void DrawCollisionLine();
  void DrawDummy();
  void DrawObject();
  __int64 _DrawObject(CGameObject *pObj, CSurface *pSF);
  __int64 DrawSelectMonsterLookAtPos(CMonster *pMon);
  void DrawTextA();
  void InitDummy(CRect *prcWnd);
  void InitCollLine(CRect *prcWnd);
  CGameObject *SelectObject(CPoint *pt);

  int InitSurface(CMapData *pMap);
  int CreateObjSurface();

  char OffDisplay();
  HRESULT ReleaseDisplay();

  CSurface *m_pSFMap;
  CSurface *m_pSFObj[2][13];
  CSurface *m_pSFCorpse;
  CSurface *m_pSFSelect;
  CSurface *m_pSFCircle;
  CSurface *m_pSFBuf;
  int m_nDummyDrawNum[60];
  CDummyDraw *m_DummyDraw[60];
  CCollLineDraw m_CollLineDraw[60];
  CRect m_rcWnd;
  CMyTimer m_tmrDraw;
  CFont m_Font;
  HPEN m_hPenBorder;
  bool m_bDisplayMode;
  CMapData *m_pActMap;
  unsigned __int16 m_wLayerIndex;
  CMapData *m_pOldActMap;
  unsigned __int16 m_wOldLayerIndex;
  CMapExtend m_MapExtend;
};

extern CMapDisplay g_MapDisplay;
