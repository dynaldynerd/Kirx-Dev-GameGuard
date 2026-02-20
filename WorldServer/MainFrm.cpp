#include "pch.h"
#include "Entities/CMainThread.h"
#include "Entities/GlobalObjects.h"
#include "MainFrm.h"
#include "resource.h"
#include <iterator>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{
constexpr UINT kRightTabControlId = 9100;
}

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_NOTIFY(TCN_SELCHANGE, kRightTabControlId, &CMainFrame::OnTabSelChange)
END_MESSAGE_MAP()

CMainFrame::CMainFrame() noexcept
    : m_mainView(IDD_MAIN_VIEW),
      m_mainPanel(IDD_MAIN_PANEL),
      m_userView(IDD_USER_VIEW),
      m_serverView(IDD_SERVER_VIEW),
      m_objectView(IDD_OBJECT_VIEW),
      m_mapView(IDD_MAP_VIEW),
      m_logView(IDD_LOG_VIEW)
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

    if (!m_mainView.Create(IDD_MAIN_VIEW, this))
    {
        return -1;
    }

    if (!m_tab.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | TCS_TABS,
                      CRect(0, 0, 0, 0), this, kRightTabControlId))
    {
        return -1;
    }

    m_tab.InsertItem(0, L"Main");
    m_tab.InsertItem(1, L"User");
    m_tab.InsertItem(2, L"Server");
    m_tab.InsertItem(3, L"Object");
    m_tab.InsertItem(4, L"Map");

    auto createPage = [this](CDialog &page, UINT dialogId) -> bool
    {
        if (!page.Create(dialogId, this))
        {
            return false;
        }

        page.ModifyStyle(WS_CAPTION | WS_SYSMENU, 0, SWP_FRAMECHANGED);
        return true;
    };

    if (!createPage(m_mainPanel, IDD_MAIN_PANEL) ||
        !createPage(m_userView, IDD_USER_VIEW) ||
        !createPage(m_serverView, IDD_SERVER_VIEW) ||
        !createPage(m_objectView, IDD_OBJECT_VIEW) ||
        !createPage(m_mapView, IDD_MAP_VIEW))
    {
        return -1;
    }

    if (!m_logView.Create(IDD_LOG_VIEW, this))
    {
        return -1;
    }

    static const UINT indicators[] = {ID_SEPARATOR};
    if (!m_statusBar.Create(this) ||
        !m_statusBar.SetIndicators(indicators, static_cast<int>(std::size(indicators))))
    {
        return -1;
    }
    m_statusBar.SetPaneText(0, L"Ready");

    CRect clientRect;
    GetClientRect(&clientRect);
    LayoutControls(clientRect.Width(), clientRect.Height());
    ShowTabPage(0);

    if (!g_Main.Init())
    {
        AfxMessageBox(L"CMainThread::Init() failed.");
    }

    return 0;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
    CFrameWnd::OnSize(nType, cx, cy);
    LayoutControls(cx, cy);
}

void CMainFrame::OnTabSelChange(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
    ShowTabPage(m_tab.GetCurSel());
    *pResult = 0;
}

void CMainFrame::LayoutControls(int cx, int cy)
{
    if (cx <= 0 || cy <= 0 || m_mainView.GetSafeHwnd() == nullptr)
    {
        return;
    }

    int statusHeight = 0;
    if (m_statusBar.GetSafeHwnd() != nullptr)
    {
        CRect statusRect;
        m_statusBar.GetWindowRect(&statusRect);
        statusHeight = statusRect.Height();
    }

    constexpr int kMargin = 8;
    constexpr int kTopRowHeight = 26;

    const int clientBottom = cy - statusHeight;
    const int topRowY = kMargin;
    const int topRowWidth = max(0, cx - kMargin * 2);
    m_mainView.MoveWindow(kMargin, topRowY, topRowWidth, kTopRowHeight, TRUE);

    const int contentTop = topRowY + kTopRowHeight + 6;
    const int contentHeight = max(0, clientBottom - contentTop - kMargin);

    const int rightWidth = max(220, min(300, cx / 3));
    const int leftWidth = max(100, cx - rightWidth - kMargin * 3);

    m_logView.MoveWindow(kMargin, contentTop, leftWidth, contentHeight, TRUE);

    const int rightX = kMargin + leftWidth + kMargin;
    m_tab.MoveWindow(rightX, contentTop, rightWidth, contentHeight, TRUE);

    CRect pageRect;
    m_tab.GetClientRect(&pageRect);
    m_tab.AdjustRect(FALSE, &pageRect);
    m_tab.MapWindowPoints(this, &pageRect);

    m_mainPanel.MoveWindow(pageRect, TRUE);
    m_userView.MoveWindow(pageRect, TRUE);
    m_serverView.MoveWindow(pageRect, TRUE);
    m_objectView.MoveWindow(pageRect, TRUE);
    m_mapView.MoveWindow(pageRect, TRUE);
}

void CMainFrame::ShowTabPage(int tabIndex)
{
    CDialog *const pages[] = {&m_mainPanel, &m_userView, &m_serverView, &m_objectView, &m_mapView};
    for (CDialog *page : pages)
    {
        if (page->GetSafeHwnd() != nullptr)
        {
            page->ShowWindow(SW_HIDE);
        }
    }

    if (tabIndex < 0 || tabIndex >= static_cast<int>(std::size(pages)))
    {
        tabIndex = 0;
    }

    if (pages[tabIndex]->GetSafeHwnd() != nullptr)
    {
        pages[tabIndex]->ShowWindow(SW_SHOW);
    }
}
