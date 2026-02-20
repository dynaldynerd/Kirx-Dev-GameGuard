#include "pch.h"

#include "CDisplayView.h"

#include "CMainThread.h"
#include "CMapData.h"
#include "CMapDisplay.h"
#include "CMapExtend.h"
#include "CMsgData.h"
#include "GlobalObjects.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CDisplayView, CDialog)
  ON_WM_CREATE()
  ON_WM_DESTROY()
  ON_WM_KEYDOWN()
  ON_WM_HSCROLL()
  ON_WM_VSCROLL()
  ON_WM_TIMER()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_RBUTTONDOWN()
  ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

CDisplayView::CDisplayView(CWnd *pParent)
  : CDialog(),
    m_ptLBtn(0, 0),
    m_nVerRange(0),
    m_nHorzRange(0),
    m_nVertPos(0),
    m_nHorzPos(0),
    m_nVerPage(0),
    m_nHorzPage(0)
{
}

void CDisplayView::GetDrawableRect(CRect *rcOut)
{
  GetClientRect(rcOut);
}

void CDisplayView::SetExtendMode(int /*bExtend*/)
{
  SetScrollPos(SB_VERT, 0, TRUE);
  SetScrollPos(SB_HORZ, 0, TRUE);
}

BOOL CDisplayView::OnInitDialog()
{
  CDialog::OnInitDialog();
  return TRUE;
}

int CDisplayView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CDialog::OnCreate(lpCreateStruct) == -1)
  {
    return -1;
  }
  return 0;
}

void CDisplayView::OnDestroy()
{
  CDialog::OnDestroy();
}

void CDisplayView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CDisplayView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
  if (g_Main.IsExcuteService())
  {
    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
    return;
  }

  CMapExtend *mapExtend = &g_MapDisplay.m_MapExtend;
  if (mapExtend->m_bExtendMode && g_MapDisplay.m_pActMap != nullptr)
  {
    const int mapX = g_MapDisplay.m_pActMap->GetBspInfo()->m_nMapSize[0];
    switch (nSBCode)
    {
      case SB_LINELEFT:
        mapExtend->ScrollMapLeft(m_nHorzPage / 10);
        break;
      case SB_LINERIGHT:
        mapExtend->ScrollMapRight(mapX, m_nHorzPage / 10);
        break;
      case SB_PAGELEFT:
        mapExtend->ScrollMapLeft(2 * (m_nHorzPage / 3));
        break;
      case SB_PAGERIGHT:
        mapExtend->ScrollMapRight(mapX, 2 * (m_nHorzPage / 3));
        break;
      case SB_THUMBPOSITION:
      case SB_THUMBTRACK:
        if (m_nHorzPos != static_cast<int>(nPos))
        {
          mapExtend->ScrollMapLeft(m_nHorzPos - static_cast<int>(nPos));
        }
        break;
      default:
        break;
    }

    m_nHorzPos = mapExtend->m_ptCenter.x - m_nHorzPage / 2;
    SetScrollPos(SB_HORZ, m_nHorzPos, TRUE);
  }

  CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDisplayView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
  if (g_Main.IsExcuteService())
  {
    CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
    return;
  }

  CMapExtend *mapExtend = &g_MapDisplay.m_MapExtend;
  if (mapExtend->m_bExtendMode && g_MapDisplay.m_pActMap != nullptr)
  {
    const int mapY = g_MapDisplay.m_pActMap->GetBspInfo()->m_nMapSize[2];
    switch (nSBCode)
    {
      case SB_LINEUP:
        mapExtend->ScrollMapUp(m_nVerPage / 10);
        break;
      case SB_LINEDOWN:
        mapExtend->ScrollMapDown(mapY, m_nVerPage / 10);
        break;
      case SB_PAGEUP:
        mapExtend->ScrollMapUp(2 * (m_nVerPage / 3));
        break;
      case SB_PAGEDOWN:
        mapExtend->ScrollMapDown(mapY, 2 * (m_nVerPage / 3));
        break;
      case SB_THUMBPOSITION:
      case SB_THUMBTRACK:
        if (m_nVertPos != static_cast<int>(nPos))
        {
          mapExtend->ScrollMapUp(m_nVertPos - static_cast<int>(nPos));
        }
        break;
      default:
        break;
    }

    m_nVertPos = mapExtend->m_ptCenter.y - m_nVerPage / 2;
    SetScrollPos(SB_VERT, m_nVertPos, TRUE);
  }

  CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CDisplayView::OnTimer(UINT_PTR nIDEvent)
{
  if (!g_Main.IsExcuteService())
  {
    if (g_MapDisplay.m_bDisplayMode)
    {
      return;
    }
    CClientDC dc(this);
    (void)dc;
  }

  CDialog::OnTimer(nIDEvent);
}

void CDisplayView::OnLButtonDown(UINT nFlags, CPoint point)
{
  if (!g_Main.IsExcuteService())
  {
    SetCapture();
    SetFocus();
    if (g_MapDisplay.m_bDisplayMode)
    {
      m_ptLBtn = point;
      g_Main.m_GameMsg.PackingMsg(0x3ECu, 0, 0, 0);
      CRect rcOut;
      GetDrawableRect(&rcOut);
      g_MapDisplay.m_MapExtend.StartScreenPoint(&point, g_MapDisplay.m_pActMap, &rcOut);
    }
  }

  CDialog::OnLButtonDown(nFlags, point);
}

void CDisplayView::OnLButtonUp(UINT nFlags, CPoint point)
{
  if (!g_Main.IsExcuteService())
  {
    CMapExtend *mapExtend = &g_MapDisplay.m_MapExtend;
    CMapData *actMap = g_MapDisplay.m_pActMap;
    ::ReleaseCapture();
    if (g_MapDisplay.m_bDisplayMode && actMap != nullptr)
    {
      _bsp_info *bspInfo = actMap->GetBspInfo();
      CSize mapSize(bspInfo->m_nMapSize[0], bspInfo->m_nMapSize[2]);
      if (mapExtend->EndScreenPoint(&mapSize))
      {
        m_nHorzPage = mapExtend->GetExtendSize()->cx;
        m_nHorzRange = bspInfo->m_nMapSize[0] - m_nHorzPage;
        m_nHorzPos = mapExtend->m_ptCenter.x - m_nHorzPage / 2;
        SetScrollRange(SB_HORZ, 0, m_nHorzRange, TRUE);
        SetScrollPos(SB_HORZ, m_nHorzPos, TRUE);

        m_nVerPage = mapExtend->GetExtendSize()->cy;
        m_nVerRange = bspInfo->m_nMapSize[2] - m_nVerPage;
        m_nVertPos = mapExtend->m_ptCenter.y - m_nVerPage / 2;
        SetScrollRange(SB_VERT, 0, m_nVerRange, TRUE);
        SetScrollPos(SB_VERT, m_nVertPos, TRUE);
      }
    }
  }

  CDialog::OnLButtonUp(nFlags, point);
}

void CDisplayView::OnMouseMove(UINT nFlags, CPoint point)
{
  if (!g_Main.IsExcuteService() && g_MapDisplay.m_bDisplayMode)
  {
    g_MapDisplay.m_MapExtend.MoveScreenPoint(&point);
  }

  CDialog::OnMouseMove(nFlags, point);
}

void CDisplayView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
  if (!g_Main.IsExcuteService() && g_MapDisplay.m_bDisplayMode)
  {
    g_MapDisplay.m_MapExtend.m_bExtendMode = false;
    SetExtendMode(0);
  }

  CDialog::OnLButtonDblClk(nFlags, point);
}

void CDisplayView::OnRButtonDown(UINT nFlags, CPoint point)
{
  if (!g_Main.IsExcuteService())
  {
    CRect rcOut;
    GetDrawableRect(&rcOut);
    if (g_MapDisplay.m_bDisplayMode && g_MapDisplay.m_pActMap != nullptr && rcOut.right > 0 && rcOut.bottom > 0)
    {
      _bsp_info *bspInfo = g_MapDisplay.m_pActMap->GetBspInfo();
      float mapX = 0.0f;
      float mapY = 0.0f;

      if (g_MapDisplay.m_MapExtend.m_bExtendMode)
      {
        const int extendHeight = g_MapDisplay.m_MapExtend.m_rcExtend.bottom - g_MapDisplay.m_MapExtend.m_rcExtend.top;
        const int extendWidth = g_MapDisplay.m_MapExtend.m_rcExtend.right - g_MapDisplay.m_MapExtend.m_rcExtend.left;
        mapX =
          static_cast<float>((extendWidth * point.x) / rcOut.right) + static_cast<float>(g_MapDisplay.m_MapExtend.m_rcExtend.left);
        mapY = static_cast<float>((extendHeight * point.y) / rcOut.bottom)
             + static_cast<float>(g_MapDisplay.m_MapExtend.m_rcExtend.top);
      }
      else
      {
        mapX = static_cast<float>(bspInfo->m_nMapSize[0] * point.x) / static_cast<float>(rcOut.right);
        mapY = static_cast<float>(bspInfo->m_nMapSize[2] * point.y) / static_cast<float>(rcOut.bottom);
      }

      g_fPoint[0] = static_cast<float>(bspInfo->m_nMapMinSize[0]) + mapX;
      g_fPoint[1] = static_cast<float>(bspInfo->m_nMapMaxSize[2]) - mapY;
    }
  }

  CDialog::OnRButtonDown(nFlags, point);
}

BOOL CDisplayView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
  if (!g_Main.IsExcuteService() && g_MapDisplay.m_bDisplayMode)
  {
    if (zDelta <= 0)
    {
      g_MapDisplay.ChangeLayer(g_MapDisplay.m_wLayerIndex + 1);
    }
    else
    {
      g_MapDisplay.ChangeLayer(g_MapDisplay.m_wLayerIndex - 1);
    }
  }

  return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}
