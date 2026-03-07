#include "pch.h"

#include "CDummyDraw.h"

#include "CDisplayView.h"
#include "CGameServerDoc.h"
#include "CMapData.h"
#include "CSurface.h"
#include "GlobalObjects.h"

HPEN CDummyDraw::s_hPen[10] = {};
HPEN CDummyDraw::s_hDirection = nullptr;

CDummyDraw::CDummyDraw()
  : m_pMap(nullptr),
    m_nType(0),
    m_fPosAbs{},
    m_fReAbs{},
    m_fScrNor{},
    m_fScrExt{}
{
}

CDummyDraw::~CDummyDraw()
{
  // this is not a stub
}

void CDummyDraw::InitPen()
{
  const COLORREF colors[10] = {
    7883506,
    16777215,
    5173094,
    1355506,
    44546,
    15878752,
    57540,
    13107455,
    0,
    328965,
  };

  for (int index = 0; index < 10; ++index)
  {
    HPEN pen = CreatePen(PS_SOLID, 1, colors[index]);
    if (pen)
    {
      s_hPen[index] = pen;
    }
  }

  HPEN directionPen = CreatePen(PS_SOLID, 1, 16713215);
  if (directionPen)
  {
    s_hDirection = directionPen;
  }
}

void CDummyDraw::DeletePen()
{
  for (int index = 0; index < 10; ++index)
  {
    if (s_hPen[index])
    {
      DeleteObject(s_hPen[index]);
      s_hPen[index] = nullptr;
    }
  }

  if (s_hDirection)
  {
    DeleteObject(s_hDirection);
    s_hDirection = nullptr;
  }
}

void CDummyDraw::SetDummyPoint(CMapData *pMap, float *pCenterPos, int nType, CRect *prcWnd)
{
  m_pMap = pMap;
  m_nType = nType;

  _bsp_info *bspInfo = pMap->GetBspInfo();
  const float left = static_cast<float>(-bspInfo->m_nMapMinSize[0]) + (pCenterPos[0] - 80.0f);
  const float top = static_cast<float>(bspInfo->m_nMapMaxSize[2]) - (pCenterPos[2] + 80.0f);
  const float right = static_cast<float>(-bspInfo->m_nMapMinSize[0]) + (pCenterPos[0] + 80.0f);
  const float bottom = static_cast<float>(bspInfo->m_nMapMaxSize[2]) - (pCenterPos[2] - 80.0f);

  m_fPosAbs[0] = left;
  m_fPosAbs[1] = top;
  m_fPosAbs[2] = right;
  m_fPosAbs[3] = top;
  m_fPosAbs[4] = left;
  m_fPosAbs[5] = bottom;
  m_fPosAbs[6] = right;
  m_fPosAbs[7] = bottom;

  for (int j = 0; j < 4; ++j)
  {
    m_fScrNor[2 * j] = (m_fPosAbs[2 * j] * static_cast<float>(prcWnd->right)) / static_cast<float>(bspInfo->m_nMapSize[0]);
    m_fScrNor[2 * j + 1] =
      (m_fPosAbs[2 * j + 1] * static_cast<float>(prcWnd->bottom)) / static_cast<float>(bspInfo->m_nMapSize[2]);
  }
}

void CDummyDraw::SetDummyRange(
  CMapData *pMap,
  float *pLT,
  float *pRB,
  float *pRT,
  float *pLB,
  int nType,
  CRect *prcWnd)
{
  m_pMap = pMap;
  m_nType = nType;

  _bsp_info *bspInfo = pMap->GetBspInfo();
  m_fPosAbs[0] = static_cast<float>(-bspInfo->m_nMapMinSize[0]) + pLT[0];
  m_fPosAbs[1] = static_cast<float>(bspInfo->m_nMapMaxSize[2]) - pLT[2];
  m_fPosAbs[2] = static_cast<float>(-bspInfo->m_nMapMinSize[0]) + pRT[0];
  m_fPosAbs[3] = static_cast<float>(bspInfo->m_nMapMaxSize[2]) - pRT[2];
  m_fPosAbs[4] = static_cast<float>(-bspInfo->m_nMapMinSize[0]) + pLB[0];
  m_fPosAbs[5] = static_cast<float>(bspInfo->m_nMapMaxSize[2]) - pLB[2];
  m_fPosAbs[6] = static_cast<float>(-bspInfo->m_nMapMinSize[0]) + pRB[0];
  m_fPosAbs[7] = static_cast<float>(bspInfo->m_nMapMaxSize[2]) - pRB[2];

  for (int vertexIndex = 0; vertexIndex < 4; ++vertexIndex)
  {
    m_fScrNor[2 * vertexIndex] =
      (m_fPosAbs[2 * vertexIndex] * static_cast<float>(prcWnd->right)) / static_cast<float>(bspInfo->m_nMapSize[0]);
    m_fScrNor[2 * vertexIndex + 1] =
      (m_fPosAbs[2 * vertexIndex + 1] * static_cast<float>(prcWnd->bottom)) / static_cast<float>(bspInfo->m_nMapSize[2]);
  }
}

char CDummyDraw::CalcAbsExt(CRect *prcArea)
{
  memcpy(m_fReAbs, m_fPosAbs, sizeof(m_fReAbs));

  bool hasPointInside[4] = {};
  for (int pointIndex = 0; pointIndex < 4; ++pointIndex)
  {
    hasPointInside[pointIndex] = true;

    if (static_cast<float>(prcArea->left) > m_fPosAbs[2 * pointIndex])
    {
      m_fReAbs[2 * pointIndex] = static_cast<float>(prcArea->left);
      hasPointInside[pointIndex] = false;
    }

    if (m_fPosAbs[2 * pointIndex] > static_cast<float>(prcArea->right))
    {
      m_fReAbs[2 * pointIndex] = static_cast<float>(prcArea->right);
      hasPointInside[pointIndex] = false;
    }

    if (static_cast<float>(prcArea->top) > m_fPosAbs[2 * pointIndex + 1])
    {
      m_fReAbs[2 * pointIndex + 1] = static_cast<float>(prcArea->top);
      hasPointInside[pointIndex] = false;
    }

    if (m_fPosAbs[2 * pointIndex + 1] > static_cast<float>(prcArea->bottom))
    {
      m_fReAbs[2 * pointIndex + 1] = static_cast<float>(prcArea->bottom);
      hasPointInside[pointIndex] = false;
    }
  }

  for (bool inside : hasPointInside)
  {
    if (inside)
    {
      return 1;
    }
  }
  return 0;
}

void CDummyDraw::CalcScrExt(CRect *prcArea, CRect *prcWnd)
{
  for (int vertexIndex = 0; vertexIndex < 4; ++vertexIndex)
  {
    const float remapX = m_fReAbs[2 * vertexIndex] - static_cast<float>(prcArea->left);
    const float remapY = m_fReAbs[2 * vertexIndex + 1] - static_cast<float>(prcArea->top);

    m_fScrExt[2 * vertexIndex] =
      (remapX * static_cast<float>(prcWnd->right)) / static_cast<float>(prcArea->right - prcArea->left);
    m_fScrExt[2 * vertexIndex + 1] =
      (remapY * static_cast<float>(prcWnd->bottom)) / static_cast<float>(prcArea->bottom - prcArea->top);
  }
}

__int64 CDummyDraw::Draw(CSurface *pSF, CRect *prcArea)
{
  if (!m_pMap)
  {
    return -1;
  }

  float *screenPoints = m_fScrNor;
  CRect drawRect{};
  if (prcArea)
  {
    if (!CalcAbsExt(prcArea))
    {
      return -2;
    }

    g_pDoc->m_DisplayView.GetDrawableRect(&drawRect);
    const int values[4] = {drawRect.left, drawRect.top, drawRect.right, drawRect.bottom};
    for (int value : values)
    {
      if (value < 0 || value > 10000)
      {
        return -1;
      }
    }

    CalcScrExt(prcArea, &drawRect);
    screenPoints = m_fScrExt;
  }

  HDC hdc = nullptr;
  LPDIRECTDRAWSURFACE7 surface = pSF->GetDDrawSurface();
  const HRESULT hr = surface->GetDC(&hdc);
  if (hr == DD_OK)
  {
    SelectObject(hdc, s_hPen[m_nType]);
    MoveToEx(hdc, static_cast<int>(screenPoints[0]), static_cast<int>(screenPoints[1]), nullptr);
    LineTo(hdc, static_cast<int>(screenPoints[2]), static_cast<int>(screenPoints[3]));
    LineTo(hdc, static_cast<int>(screenPoints[6]), static_cast<int>(screenPoints[7]));
    LineTo(hdc, static_cast<int>(screenPoints[4]), static_cast<int>(screenPoints[5]));
    LineTo(hdc, static_cast<int>(screenPoints[0]), static_cast<int>(screenPoints[1]));

    SelectObject(hdc, s_hDirection);
    LineTo(hdc, static_cast<int>(screenPoints[2]), static_cast<int>(screenPoints[3]));
    surface->ReleaseDC(hdc);
  }

  return hr;
}
