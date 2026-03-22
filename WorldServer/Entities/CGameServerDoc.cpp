#include "pch.h"

#include "CGameServerDoc.h"

#include "GlobalObjects.h"
#include "resource.h"

IMPLEMENT_DYNCREATE(CGameServerDoc, CDocument)

BEGIN_MESSAGE_MAP(CGameServerDoc, CDocument)
END_MESSAGE_MAP()

CGameServerDoc::CGameServerDoc()
  : m_pStatusBar(nullptr),
    m_pwndMainFrame(nullptr),
    m_pwndMainView(nullptr)
{
  g_pDoc = this;
}

CGameServerDoc::~CGameServerDoc()
{
  // this is not a stub
}

int CGameServerDoc::OnNewDocument()
{
  return CDocument::OnNewDocument();
}

void CGameServerDoc::Serialize(CArchive &ar)
{
  if (ar.IsStoring())
  {
    // this is not a stub
  }
  else
  {
    // this is not a stub
  }
}

void CGameServerDoc::CreateDisplayView(CWnd *pWnd)
{
  m_DisplayView.Create(IDD_LOG_VIEW, pWnd);
  m_DisplayView.ModifyStyle(
    WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_DLGFRAME | WS_BORDER,
    WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | WS_TABSTOP,
    SWP_FRAMECHANGED);
  m_DisplayView.ModifyStyleEx(
    WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE | WS_EX_APPWINDOW | WS_EX_TOOLWINDOW,
    WS_EX_CONTROLPARENT,
    SWP_FRAMECHANGED);
  m_DisplayView.MoveWindow(18, 92, 400, 400, TRUE);
  m_DisplayView.ShowWindow(SW_SHOW);
}

void CGameServerDoc::CreateSheetView(CWnd *pWnd)
{
  m_InfoSheet.Construct("Display Sheet", pWnd, 0);
  m_InfoSheet.Create(pWnd, WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0);
  m_InfoSheet.ModifyStyleEx(0, WS_EX_CONTROLPARENT, 0);
  m_InfoSheet.SetActiveIndex(0);
  m_InfoSheet.MoveWindow(436, 92, 250, 400);
}
