#include "pch.h"
#include "Entities/CMainThread.h"
#include "Entities/GlobalObjects.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_WM_CREATE()
    ON_WM_SIZE()
END_MESSAGE_MAP()

CMainFrame::CMainFrame() noexcept
{
    g_pFrame = this;
}

CMainFrame::~CMainFrame() = default;

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CFrameWnd::PreCreateWindow(cs))
    {
        return FALSE;
    }
    cs.lpszClass = AfxRegisterWndClass(0);
    return TRUE;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
    {
        return -1;
    }

    if (!m_log.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOINTEGRALHEIGHT,
                      CRect(0, 0, 0, 0), this, 1))
    {
        return -1;
    }

    if (!g_Main.Init())
    {
        AfxMessageBox(L"CMainThread::Init() failed.");
    }

    m_log.AddString(L"World server initialized.");
    return 0;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
    CFrameWnd::OnSize(nType, cx, cy);
    if (m_log.GetSafeHwnd() != nullptr)
    {
        m_log.MoveWindow(0, 0, cx, cy);
    }
}
