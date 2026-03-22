#include "pch.h"

#include "CGameServerView.h"

#include "MainFrm.h"
#include "CGameServerDoc.h"
#include "COpenDlg.h"
#include "DatabaseSetupDlg.h"
#include "CMainThread.h"
#include "CMapDisplay.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

namespace
{
bool HasRequiredStartupSettings()
{
  return CDatabaseSetupDlg::HasRequiredSettings();
}
}

IMPLEMENT_DYNCREATE(CGameServerView, CFormView)

BEGIN_MESSAGE_MAP(CGameServerView, CFormView)
  ON_WM_CREATE()
  ON_BN_CLICKED(IDC_MAIN_START, &CGameServerView::OnButtonStart)
  ON_BN_CLICKED(1029, &CGameServerView::OnButtonDisplayall)
  ON_BN_CLICKED(1024, &CGameServerView::OnButtonMonster)
  ON_BN_CLICKED(1022, &CGameServerView::OnBUTTONPreClose)
  ON_BN_CLICKED(1025, &CGameServerView::OnBUTTONServerClose)
  ON_BN_CLICKED(1043, &CGameServerView::OnBUTTONHSKControl)
  ON_BN_CLICKED(1023, &CGameServerView::OnButtonLogfile)
  ON_COMMAND(32771, &CGameServerView::OnButtonDisplaymode)
END_MESSAGE_MAP()

CGameServerView::CGameServerView()
  : CFormView(IDD_MAIN_VIEW),
    m_btStart(),
    m_btServerClose(),
    m_btPreClose(),
    m_btMonster(),
    m_btDisplayAll(),
    m_btHSKStop(),
    m_btLogFile(),
    m_bStartupInProgress(false),
    m_bStartupCompleted(false),
    m_bRuntimeViewsCreated(false)
{
}

CGameServerView::~CGameServerView()
{
  // this is not a stub
}

void CGameServerView::DoDataExchange(CDataExchange *pDX)
{
  CFormView::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_MAIN_START, m_btStart);
  DDX_Control(pDX, 1025, m_btServerClose);
  DDX_Control(pDX, 1022, m_btPreClose);
  DDX_Control(pDX, 1024, m_btMonster);
  DDX_Control(pDX, 1029, m_btDisplayAll);
  DDX_Control(pDX, 1043, m_btHSKStop);
  DDX_Control(pDX, 1023, m_btLogFile);
}

int CGameServerView::PreCreateWindow(CREATESTRUCT &cs)
{
  return CFormView::PreCreateWindow(cs);
}

void CGameServerView::OnInitialUpdate()
{
  CFormView::OnInitialUpdate();
  CFrameWnd *parentFrame = GetParentFrame();
  if (parentFrame != nullptr)
  {
    parentFrame->RecalcLayout(TRUE);
  }
  ResizeParentToFit(TRUE);
  UpdateStartupControls();
}

int CGameServerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CFormView::OnCreate(lpCreateStruct) == -1)
  {
    return -1;
  }

  CGameServerDoc *document = GetDocument();
  if (document != nullptr)
  {
    document->m_pwndMainView = this;
  }

  if (CDatabaseSetupDlg::IsAutoStartEnabled())
  {
    OnButtonStart();
  }

  return 0;
}

void CGameServerView::OnButtonStart()
{
  if (m_bStartupInProgress || m_bStartupCompleted)
  {
    return;
  }

  CMainFrame *mainFrame = GetMainFrame();
  m_bStartupInProgress = true;
  UpdateStartupControls();
  if (mainFrame != nullptr)
  {
    mainFrame->SetServerStartupState(CMainFrame::kStarting);
  }

  if (!HasRequiredStartupSettings())
  {
    CDatabaseSetupDlg databaseSetupDialog(this);
    if (databaseSetupDialog.DoModal() != IDOK)
    {
      m_bStartupInProgress = false;
      UpdateStartupControls();
      if (mainFrame != nullptr)
      {
        mainFrame->SetServerStartupState(CMainFrame::kNotStarted);
      }
      return;
    }
  }

  COpenDlg openDialog;
  CWnd *desktopWindow = CWnd::GetDesktopWindow();
  if (desktopWindow != nullptr)
  {
    openDialog.Create(IDD_LOADING, desktopWindow);
  }

  if (!g_Main.Init())
  {
    MyMessageBox("Error", "CGameServerView::OnButtonStart(...) : \r\ng_Main.Init() Fail!");
    if (openDialog.GetSafeHwnd() != nullptr)
    {
      openDialog.DestroyWindow();
    }

    m_bStartupInProgress = false;
    UpdateStartupControls();
    if (mainFrame != nullptr)
    {
      mainFrame->SetServerStartupState(CMainFrame::kNotStarted);
    }
    return;
  }

  if (openDialog.GetSafeHwnd() != nullptr)
  {
    openDialog.DestroyWindow();
  }

  CGameServerDoc *document = GetDocument();
  if (document != nullptr)
  {
    EnsureRuntimeViewsCreated(document);
    document->m_pwndMainView = this;
  }

  m_bStartupCompleted = true;
  m_bStartupInProgress = false;
  UpdateStartupControls();
  if (mainFrame != nullptr)
  {
    mainFrame->SetServerStartupState(CMainFrame::kStarted);
  }
}

void CGameServerView::OnButtonDisplaymode()
{
  if (!IsServerStarted())
  {
    return;
  }

  if (!g_Main.IsReleaseServiceMode())
  {
    g_Main.m_GameMsg.PackingMsg(1001, 0, 0, 0);
  }
}

void CGameServerView::OnButtonDisplayall()
{
  if (!IsServerStarted())
  {
    return;
  }

  if (!g_Main.IsReleaseServiceMode())
  {
    g_Main.m_GameMsg.PackingMsg(1009, 0, 0, 0);
  }
}

void CGameServerView::OnButtonMonster()
{
  if (!IsServerStarted())
  {
    return;
  }

  if (!g_Main.IsReleaseServiceMode())
  {
    g_Main.m_GameMsg.PackingMsg(1003, 0, 0, 0);
  }
}

void CGameServerView::OnBUTTONPreClose()
{
  if (!IsServerStarted())
  {
    return;
  }

  if (!g_Main.IsReleaseServiceMode())
  {
    g_Main.m_GameMsg.PackingMsg(1010, 0, 0, 0);
  }
}

void CGameServerView::OnButtonLogfile()
{
  // this is not a stub
}

void CGameServerView::OnBUTTONServerClose()
{
  if (!IsServerStarted())
  {
    return;
  }

  if (!g_Main.IsReleaseServiceMode())
  {
    g_Main.m_GameMsg.PackingMsg(1012, 0, 0, 0);
  }
}

void CGameServerView::OnBUTTONHSKControl()
{
  if (!IsServerStarted())
  {
    return;
  }

  if (!g_Main.IsReleaseServiceMode())
  {
    g_Main.m_GameMsg.PackingMsg(1015, 0, 0, 0);
  }
}

void CGameServerView::OnBUTTONWorldConnect()
{
  // this is not a stub
}

void CGameServerView::OnButtonOffplayer()
{
  if (!IsServerStarted())
  {
    return;
  }

  if (!g_Main.IsReleaseServiceMode())
  {
    // this is not a stub
    static bool bCreate = false;
    bCreate = !bCreate;
  }
}

void CGameServerView::OnButtonDummy()
{
  if (!IsServerStarted())
  {
    return;
  }

  if (!g_Main.IsReleaseServiceMode() && g_pDoc != nullptr)
  {
    CRect drawRect;
    g_pDoc->m_DisplayView.GetDrawableRect(&drawRect);
    g_MapDisplay.InitDummy(&drawRect);
  }
}

void CGameServerView::OnButtonCollline()
{
  if (!IsServerStarted())
  {
    return;
  }

  if (!g_Main.IsReleaseServiceMode() && g_pDoc != nullptr)
  {
    CRect drawRect;
    g_pDoc->m_DisplayView.GetDrawableRect(&drawRect);
    g_MapDisplay.InitCollLine(&drawRect);
  }
}

CGameServerDoc *CGameServerView::GetDocument() const
{
  if (m_pDocument == nullptr || !m_pDocument->IsKindOf(RUNTIME_CLASS(CGameServerDoc)))
  {
    ASSERT(FALSE);
    return nullptr;
  }
  return static_cast<CGameServerDoc *>(m_pDocument);
}

CMainFrame *CGameServerView::GetMainFrame() const
{
  return DYNAMIC_DOWNCAST(CMainFrame, GetParentFrame());
}

bool CGameServerView::IsServerStarted() const
{
  return m_bStartupCompleted;
}

void CGameServerView::EnsureRuntimeViewsCreated(CGameServerDoc *document)
{
  if (document == nullptr || m_bRuntimeViewsCreated)
  {
    return;
  }

  document->CreateDisplayView(this);
  document->CreateSheetView(this);
  m_bRuntimeViewsCreated = true;
}

void CGameServerView::UpdateStartupControls()
{
  const BOOL enableServerControls = m_bStartupCompleted ? TRUE : FALSE;
  if (m_btDisplayAll.GetSafeHwnd() != nullptr)
  {
    m_btDisplayAll.EnableWindow(enableServerControls);
  }
  if (m_btMonster.GetSafeHwnd() != nullptr)
  {
    m_btMonster.EnableWindow(enableServerControls);
  }
  if (m_btLogFile.GetSafeHwnd() != nullptr)
  {
    m_btLogFile.EnableWindow(enableServerControls);
  }
  if (m_btPreClose.GetSafeHwnd() != nullptr)
  {
    m_btPreClose.EnableWindow(enableServerControls);
  }
  if (m_btServerClose.GetSafeHwnd() != nullptr)
  {
    m_btServerClose.EnableWindow(enableServerControls);
  }
  if (m_btHSKStop.GetSafeHwnd() != nullptr)
  {
    m_btHSKStop.EnableWindow(enableServerControls);
  }
  if (m_btStart.GetSafeHwnd() != nullptr)
  {
    m_btStart.EnableWindow((!m_bStartupInProgress && !m_bStartupCompleted) ? TRUE : FALSE);
    if (m_bStartupCompleted)
    {
      m_btStart.SetWindowText(_T("Started"));
    }
    else if (m_bStartupInProgress)
    {
      m_btStart.SetWindowText(_T("Starting..."));
    }
    else
    {
      m_btStart.SetWindowText(_T("Start"));
    }
  }
}
