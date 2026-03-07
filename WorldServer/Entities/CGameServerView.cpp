#include "pch.h"

#include "CGameServerView.h"

#include "CGameServerDoc.h"
#include "COpenDlg.h"
#include "LicensePopupDlg.h"
#include "CMainThread.h"
#include "CMapDisplay.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

IMPLEMENT_DYNCREATE(CGameServerView, CFormView)

BEGIN_MESSAGE_MAP(CGameServerView, CFormView)
  ON_WM_CREATE()
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
    m_btServerClose(),
    m_btPreClose(),
    m_btMonster(),
    m_btDisplayAll(),
    m_btHSKStop(),
    m_btLogFile()
{
}

CGameServerView::~CGameServerView()
{
  // this is not a stub
}

void CGameServerView::DoDataExchange(CDataExchange *pDX)
{
  CFormView::DoDataExchange(pDX);
  DDX_Control(pDX, 1025, m_btServerClose);
  DDX_Control(pDX, 1022, m_btPreClose);
  DDX_Control(pDX, 1024, m_btMonster);
  DDX_Control(pDX, 1029, m_btDisplayAll);
  DDX_Control(pDX, 1043, m_btHSKStop);
  DDX_Control(pDX, 1023, m_btLogFile);
}

BOOL CGameServerView::PreCreateWindow(CREATESTRUCT &cs)
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
}

int CGameServerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CFormView::OnCreate(lpCreateStruct) == -1)
  {
    return -1;
  }

  CGameServerDoc *document = GetDocument();

  if (!g_Main.m_bServiceKeyPass)
  {
    LicensePopupDlg licenseDialog;
    licenseDialog.DoModal();
    if (!licenseDialog.m_bOk)
    {
      MyMessageBox("Error", " License Cancle");
      return -1;
    }
  }

  COpenDlg openDialog;
  CWnd *desktopWindow = CWnd::GetDesktopWindow();
  openDialog.Create(IDD_LOADING, desktopWindow);

  if (!g_Main.Init())
  {
    MyMessageBox("Error", "CGameServerView::OnCreate(...) : \r\ng_Main.Init() Fail!");
    openDialog.DestroyWindow();
    return -1;
  }

  openDialog.DestroyWindow();
  if (document != nullptr)
  {
    document->CreateDisplayView(this);
    document->CreateSheetView(this);
    document->m_pwndMainView = this;
  }

  CFrameWnd *parentFrame = GetParentFrame();
  if (parentFrame != nullptr)
  {
    parentFrame->SetWindowText(_T("RF Online: WorldServer"));
  }

  return 0;
}

void CGameServerView::OnButtonDisplaymode()
{
  if (!g_Main.IsReleaseServiceMode())
  {
    g_Main.m_GameMsg.PackingMsg(1001, 0, 0, 0);
  }
}

void CGameServerView::OnButtonDisplayall()
{
  if (!g_Main.IsReleaseServiceMode())
  {
    g_Main.m_GameMsg.PackingMsg(1009, 0, 0, 0);
  }
}

void CGameServerView::OnButtonMonster()
{
  if (!g_Main.IsReleaseServiceMode())
  {
    g_Main.m_GameMsg.PackingMsg(1003, 0, 0, 0);
  }
}

void CGameServerView::OnBUTTONPreClose()
{
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
  if (!g_Main.IsReleaseServiceMode())
  {
    g_Main.m_GameMsg.PackingMsg(1012, 0, 0, 0);
  }
}

void CGameServerView::OnBUTTONHSKControl()
{
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
  if (!g_Main.IsReleaseServiceMode())
  {
    // this is not a stub
    static bool bCreate = false;
    bCreate = !bCreate;
  }
}

void CGameServerView::OnButtonDummy()
{
  if (!g_Main.IsReleaseServiceMode() && g_pDoc != nullptr)
  {
    CRect drawRect;
    g_pDoc->m_DisplayView.GetDrawableRect(&drawRect);
    g_MapDisplay.InitDummy(&drawRect);
  }
}

void CGameServerView::OnButtonCollline()
{
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
