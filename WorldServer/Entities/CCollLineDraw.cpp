#include "pch.h"

#include "CCollLineDraw.h"

#include "CMapData.h"
#include "CSurface.h"
#include "WorldServerUtil.h"

HPEN CCollLineDraw::s_hPen = nullptr;

void _coll_point::InitPoint(CMapData *pMap, float *pPos, CRect *prcWnd)
{
  _bsp_info *bspInfo = pMap->GetBspInfo();
  m_fPosAbs[0] = static_cast<float>(-bspInfo->m_nMapMinSize[0]) + pPos[0];
  m_fPosAbs[1] = static_cast<float>(bspInfo->m_nMapMaxSize[2]) - pPos[2];
  m_fScrNor[0] = (m_fPosAbs[0] * static_cast<float>(prcWnd->right)) / static_cast<float>(bspInfo->m_nMapSize[0]);
  m_fScrNor[1] = (m_fPosAbs[1] * static_cast<float>(prcWnd->bottom)) / static_cast<float>(bspInfo->m_nMapSize[2]);
}

CCollLineDraw::CCollLineDraw()
  : m_pMap(nullptr),
    m_rcWnd(),
    m_nPointNum(0),
    m_Point(nullptr),
    m_nLineNum(0),
    m_pLine(nullptr)
{
}

CCollLineDraw::~CCollLineDraw()
{
  if (m_Point)
  {
    operator delete[](m_Point);
    m_Point = nullptr;
  }
}

void CCollLineDraw::InitPen()
{
  HPEN pen = CreatePen(PS_SOLID, 1, 7895160);
  if (pen)
  {
    s_hPen = pen;
  }
}

void CCollLineDraw::DeletePen()
{
  if (s_hPen)
  {
    DeleteObject(s_hPen);
    s_hPen = nullptr;
  }
}

bool CCollLineDraw::InitLine(CMapData *pMap, CRect *prcWnd)
{
  if (m_pMap)
  {
    return false;
  }

  m_pMap = pMap;
  m_nPointNum = pMap->m_Level.mBsp->mCFVertexNum;
  if (m_nPointNum > 15000)
  {
    MyMessageBox(
      "CCollLineDraw::InitLine",
      "%s map's coll_point_num(%d) is more than max_coll_point(%d)",
      pMap->m_pMapSet->m_strCode,
      m_nPointNum,
      15000);
    return false;
  }

  if (!m_Point)
  {
    const size_t pointCount = static_cast<size_t>(m_nPointNum);
    if (pointCount > (SIZE_MAX / sizeof(_coll_point)))
    {
      return false;
    }
    m_Point = static_cast<_coll_point *>(operator new[](pointCount * sizeof(_coll_point)));
  }

  m_rcWnd = *prcWnd;

  for (int index = 0; index < m_nPointNum; ++index)
  {
    float *pos = pMap->m_Level.mBsp->mCFVertex[index];
    m_Point[index].InitPoint(pMap, pos, &m_rcWnd);
  }

  m_nLineNum = pMap->m_Level.mBsp->mCFLineNum;
  m_pLine = pMap->m_Level.mBsp->mCFLine;
  return true;
}

bool CCollLineDraw::DrawEx(int nLineIndex, HDC *pDC, CRect *prcArea)
{
  const int startVertex = m_pLine[nLineIndex].start_v;
  const int endVertex = m_pLine[nLineIndex].end_v;

  const float startX = m_Point[startVertex].m_fPosAbs[0] - static_cast<float>(prcArea->left);
  const float startY = m_Point[startVertex].m_fPosAbs[1] - static_cast<float>(prcArea->top);
  const float endX = m_Point[endVertex].m_fPosAbs[0] - static_cast<float>(prcArea->left);
  const float endY = m_Point[endVertex].m_fPosAbs[1] - static_cast<float>(prcArea->top);

  m_Point[startVertex].m_fScrExt[0] =
    (startX * static_cast<float>(m_rcWnd.right)) / static_cast<float>(prcArea->right - prcArea->left);
  m_Point[startVertex].m_fScrExt[1] =
    (startY * static_cast<float>(m_rcWnd.bottom)) / static_cast<float>(prcArea->bottom - prcArea->top);

  m_Point[endVertex].m_fScrExt[0] =
    (endX * static_cast<float>(m_rcWnd.right)) / static_cast<float>(prcArea->right - prcArea->left);
  m_Point[endVertex].m_fScrExt[1] =
    (endY * static_cast<float>(m_rcWnd.bottom)) / static_cast<float>(prcArea->bottom - prcArea->top);

  MoveToEx(*pDC, static_cast<int>(m_Point[startVertex].m_fScrExt[0]), static_cast<int>(m_Point[startVertex].m_fScrExt[1]), nullptr);
  LineTo(*pDC, static_cast<int>(m_Point[endVertex].m_fScrExt[0]), static_cast<int>(m_Point[endVertex].m_fScrExt[1]));
  return true;
}

bool CCollLineDraw::Draw(CSurface *pSF, CRect *prcArea)
{
  if (!m_pMap)
  {
    return false;
  }

  HDC hdc = nullptr;
  LPDIRECTDRAWSURFACE7 surface = pSF->GetDDrawSurface();
  if (surface->GetDC(&hdc) != DD_OK)
  {
    return false;
  }

  SelectObject(hdc, s_hPen);
  for (int lineIndex = 0; lineIndex < m_nLineNum; ++lineIndex)
  {
    if (prcArea)
    {
      DrawEx(lineIndex, &hdc, prcArea);
    }
    else
    {
      float *start = m_Point[m_pLine[lineIndex].start_v].m_fScrNor;
      float *end = m_Point[m_pLine[lineIndex].end_v].m_fScrNor;
      MoveToEx(hdc, static_cast<int>(start[0]), static_cast<int>(start[1]), nullptr);
      LineTo(hdc, static_cast<int>(end[0]), static_cast<int>(end[1]));
    }
  }

  surface->ReleaseDC(hdc);
  return true;
}

