#include "pch.h"

#include "CGameServerDoc.h"

#include "resource.h"

CGameServerDoc::CGameServerDoc()
  : m_pStatusBar(nullptr),
    m_pwndMainFrame(nullptr),
    m_pwndMainView(nullptr)
{
}

void CGameServerDoc::CreateDisplayView(CWnd *pWnd)
{
  m_DisplayView.Create(IDD_LOG_VIEW, pWnd);
  m_DisplayView.ModifyStyleEx(0, WS_EX_CONTROLPARENT, 0);
  m_DisplayView.MoveWindow(18, 40, 400, 400, TRUE);
  m_DisplayView.ShowWindow(SW_SHOW);
}

void CGameServerDoc::CreateSheetView(CWnd *pWnd)
{
  m_InfoSheet.Construct("Display Sheet", pWnd, 0);
  m_InfoSheet.Create(pWnd, WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0);
  m_InfoSheet.ModifyStyleEx(0, WS_EX_CONTROLPARENT, 0);
  m_InfoSheet.SetActiveIndex(0);
  m_InfoSheet.MoveWindow(436, 35, 250, 400);
}
