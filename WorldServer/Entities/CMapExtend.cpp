#include "pch.h"

#include "CMapExtend.h"

#include <algorithm>
#include <cstdlib>

#include "CMapData.h"
#include "CSurface.h"

void _map_rate::Setting(int x, int y)
{
  if (x < y)
  {
    fSide[1] = 1.0f;
    fSide[0] = static_cast<float>(x) / static_cast<float>(y);
    nStandard = 1;
    nPartner = 0;
  }
  else
  {
    fSide[0] = 1.0f;
    fSide[1] = static_cast<float>(y) / static_cast<float>(x);
    nStandard = 0;
    nPartner = 1;
  }
}

CMapExtend::CMapExtend()
  : m_bExtendMode(false),
    m_ptStartMap(0, 0),
    m_ptEndMap(0, 0),
    m_ptCenter(0, 0),
    m_rcExtend(0, 0, 0, 0),
    m_ptStartScreen(0, 0),
    m_ptEndScreen(0, 0),
    m_ptMoveScreen(0, 0),
    m_sizeExtend(0, 0),
    m_hPen(CreatePen(0, 1, 6579300)),
    m_pSF(nullptr),
    m_Rate{},
    m_bSetArea(0),
    m_bMove(0)
{
}

CMapExtend::CMapExtend(CSurface **pSF)
  : m_bExtendMode(false),
    m_ptStartMap(0, 0),
    m_ptEndMap(0, 0),
    m_ptCenter(0, 0),
    m_rcExtend(0, 0, 0, 0),
    m_ptStartScreen(0, 0),
    m_ptEndScreen(0, 0),
    m_ptMoveScreen(0, 0),
    m_sizeExtend(0, 0),
    m_hPen(CreatePen(0, 1, 6579300)),
    m_pSF(pSF),
    m_Rate{},
    m_bSetArea(0),
    m_bMove(0)
{
}

CMapExtend::~CMapExtend()
{
  DeleteObject(m_hPen);
}

void CMapExtend::Init(CSurface **pSF)
{
  m_pSF = pSF;
}

void CMapExtend::DrawRect()
{
  if (!m_bSetArea || !m_pSF || !*m_pSF)
  {
    return;
  }

  HDC hdc = nullptr;
  LPDIRECTDRAWSURFACE7 surface = (*m_pSF)->GetDDrawSurface();
  if (surface->GetDC(&hdc) == DD_OK)
  {
    SelectObject(hdc, m_hPen);
    MoveToEx(hdc, m_ptStartScreen.x, m_ptStartScreen.y, nullptr);
    LineTo(hdc, m_ptMoveScreen.x, m_ptStartScreen.y);
    LineTo(hdc, m_ptMoveScreen.x, m_ptMoveScreen.y);
    LineTo(hdc, m_ptStartScreen.x, m_ptMoveScreen.y);
    LineTo(hdc, m_ptStartScreen.x, m_ptStartScreen.y);
    surface->ReleaseDC(hdc);
  }
}

void CMapExtend::StartScreenPoint(CPoint *pt, CMapData *pMap, CRect *rcWnd)
{
  if (m_bExtendMode || !pt || !pMap || !rcWnd)
  {
    return;
  }

  m_bSetArea = 1;
  m_bMove = 0;
  m_ptStartScreen = *pt;
  m_ptMoveScreen = *pt;
  m_Rate.Setting(rcWnd->right, rcWnd->bottom);
}

void CMapExtend::MoveScreenPoint(CPoint *pt)
{
  if (!m_bSetArea || !pt || (m_ptStartScreen.x == pt->x && m_ptStartScreen.y == pt->y))
  {
    return;
  }

  m_bMove = 1;
  (&m_ptMoveScreen.x)[m_Rate.nStandard] = (&pt->x)[m_Rate.nStandard];

  int standardSize = std::abs((&m_ptStartScreen.x)[m_Rate.nStandard] - (&pt->x)[m_Rate.nStandard]);
  if (standardSize > 200)
  {
    standardSize = 200;
  }

  const int partnerSize = static_cast<int>(static_cast<float>(standardSize) * m_Rate.fSide[m_Rate.nPartner]);
  if ((&m_ptStartScreen.x)[m_Rate.nPartner] < (&pt->x)[m_Rate.nPartner])
  {
    (&m_ptMoveScreen.x)[m_Rate.nPartner] = (&m_ptStartScreen.x)[m_Rate.nPartner] + partnerSize;
  }
  else
  {
    (&m_ptMoveScreen.x)[m_Rate.nPartner] = (&m_ptStartScreen.x)[m_Rate.nPartner] - partnerSize;
  }
}

bool CMapExtend::EndScreenPoint(CSize *szMap)
{
  if (!m_bSetArea)
  {
    return m_bExtendMode;
  }

  if (!m_bMove)
  {
    m_bSetArea = 0;
    return m_bExtendMode;
  }

  const int startX = (m_ptStartScreen.x < m_ptMoveScreen.x) ? m_ptStartScreen.x : m_ptMoveScreen.x;
  const int startY = (m_ptStartScreen.y < m_ptMoveScreen.y) ? m_ptStartScreen.y : m_ptMoveScreen.y;
  const int endX = (m_ptStartScreen.x > m_ptMoveScreen.x) ? m_ptStartScreen.x : m_ptMoveScreen.x;
  const int endY = (m_ptStartScreen.y > m_ptMoveScreen.y) ? m_ptStartScreen.y : m_ptMoveScreen.y;

  const int width = endX - startX;
  const int height = endY - startY;

  m_ptStartScreen.x = startX;
  m_ptStartScreen.y = startY;
  m_ptEndScreen.x = endX;
  m_ptEndScreen.y = endY;

  if (width < 10 && height < 10)
  {
    m_bSetArea = 0;
    return m_bExtendMode;
  }

  ConvertToMap(szMap);
  m_bSetArea = 0;
  m_bExtendMode = true;
  m_rcExtend.SetRect(m_ptStartMap, m_ptEndMap);
  return m_bExtendMode;
}

void CMapExtend::ConvertToMap(CSize *szMap)
{
  if (!szMap || !m_pSF || !*m_pSF)
  {
    return;
  }

  DDSURFACEDESC2 *surfaceDesc = (*m_pSF)->GetDDSurfaceDesc();
  if (!surfaceDesc || surfaceDesc->dwWidth == 0 || surfaceDesc->dwHeight == 0)
  {
    return;
  }

  const int drawWidth = static_cast<int>(surfaceDesc->dwWidth);
  const int drawHeight = static_cast<int>(surfaceDesc->dwHeight);

  m_ptStartMap.x = m_ptStartScreen.x * szMap->cx / drawWidth;
  m_ptStartMap.y = m_ptStartScreen.y * szMap->cy / drawHeight;
  m_ptEndMap.x = m_ptEndScreen.x * szMap->cx / drawWidth;
  m_ptEndMap.y = m_ptEndScreen.y * szMap->cy / drawHeight;

  m_sizeExtend.cx = m_ptEndMap.x - m_ptStartMap.x;
  m_sizeExtend.cy = m_ptEndMap.y - m_ptStartMap.y;
  m_ptCenter.x = m_ptStartMap.x + m_sizeExtend.cx / 2;
  m_ptCenter.y = m_ptStartMap.y + m_sizeExtend.cy / 2;
}

CSize *CMapExtend::GetExtendSize()
{
  return &m_sizeExtend;
}

void CMapExtend::ScrollMapLeft(int nInterval)
{
  if (!m_bExtendMode)
  {
    return;
  }

  if (m_ptStartMap.x - nInterval >= 0)
  {
    m_ptStartMap.x -= nInterval;
    m_ptEndMap.x -= nInterval;
    m_ptCenter.x -= nInterval;
  }
  else
  {
    m_ptStartMap.x = 0;
    m_ptEndMap.x = m_sizeExtend.cx;
    m_ptCenter.x = m_sizeExtend.cx / 2;
  }

  m_rcExtend.SetRect(m_ptStartMap, m_ptEndMap);
}

void CMapExtend::ScrollMapRight(int nMapX, int nInterval)
{
  if (!m_bExtendMode)
  {
    return;
  }

  if (nInterval + m_ptEndMap.x <= nMapX)
  {
    m_ptStartMap.x += nInterval;
    m_ptEndMap.x += nInterval;
    m_ptCenter.x += nInterval;
  }
  else
  {
    m_ptEndMap.x = nMapX;
    m_ptStartMap.x = m_ptEndMap.x - m_sizeExtend.cx;
    m_ptCenter.x = m_ptEndMap.x - m_sizeExtend.cx / 2;
  }

  m_rcExtend.SetRect(m_ptStartMap, m_ptEndMap);
}

void CMapExtend::ScrollMapUp(int nInterval)
{
  if (!m_bExtendMode)
  {
    return;
  }

  if (m_ptStartMap.y - nInterval >= 0)
  {
    m_ptStartMap.y -= nInterval;
    m_ptEndMap.y -= nInterval;
    m_ptCenter.y -= nInterval;
  }
  else
  {
    m_ptStartMap.y = 0;
    m_ptEndMap.y = m_sizeExtend.cy;
    m_ptCenter.y = m_sizeExtend.cy / 2;
  }

  m_rcExtend.SetRect(m_ptStartMap, m_ptEndMap);
}

void CMapExtend::ScrollMapDown(int nMapY, int nInterval)
{
  if (!m_bExtendMode)
  {
    return;
  }

  if (nInterval + m_ptEndMap.y <= nMapY)
  {
    m_ptStartMap.y += nInterval;
    m_ptEndMap.y += nInterval;
    m_ptCenter.y += nInterval;
  }
  else
  {
    m_ptEndMap.y = nMapY;
    m_ptStartMap.y = m_ptEndMap.y - m_sizeExtend.cy;
    m_ptCenter.y = m_ptEndMap.y - m_sizeExtend.cy / 2;
  }

  m_rcExtend.SetRect(m_ptStartMap, m_ptEndMap);
}
