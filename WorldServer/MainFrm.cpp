#include "pch.h"
#include "Entities/CMainThread.h"
#include "Entities/CGameServerDoc.h"
#include "Entities/GlobalObjects.h"
#include "Entities/CNationSettingManager.h"
#include "Entities/CPlayer.h"
#include "Entities/CMapData.h"
#include "Entities/CMapDisplay.h"
#include "Entities/UIMessageIds.h"
#include "MainFrm.h"
#include "resource.h"
#include <iterator>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{
constexpr UINT kStatusTimerId = 1;
constexpr int kFixedWindowWidth = 700;
constexpr int kFixedWindowHeight = 540;
constexpr UINT kStatusIndicators[] = {
    ID_SEPARATOR,
    ID_SEPARATOR,
    ID_SEPARATOR,
    ID_SEPARATOR,
    ID_SEPARATOR,
    ID_SEPARATOR,
    ID_SEPARATOR,
    ID_SEPARATOR,
};
}

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_TIMER()
    ON_WM_DESTROY()
    ON_WM_PAINT()
    ON_BN_CLICKED(1029, &CMainFrame::OnButtonDisplayAll)
    ON_BN_CLICKED(1024, &CMainFrame::OnButtonMonster)
    ON_BN_CLICKED(1023, &CMainFrame::OnButtonAction)
    ON_BN_CLICKED(1025, &CMainFrame::OnButtonServerClose)
    ON_BN_CLICKED(1043, &CMainFrame::OnButtonDungeonLoad)
    ON_MESSAGE(WM_WS_UPDATE_MAP, &CMainFrame::OnUiUpdateMap)
    ON_MESSAGE(WM_WS_UPDATE_SERVER, &CMainFrame::OnUiUpdateServer)
    ON_MESSAGE(WM_WS_UPDATE_OBJECT, &CMainFrame::OnUiUpdateObject)
END_MESSAGE_MAP()

CMainFrame::CMainFrame() noexcept
    : m_mainView(IDD_MAIN_VIEW)
{
    g_pFrame = this;
}

CMainFrame::~CMainFrame() = default;

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT &cs)
{
    if (!CFrameWnd::PreCreateWindow(cs))
    {
        return FALSE;
    }

    cs.style &= 0xFF30FFFFu;
    cs.style |= 0xA0000u;
    cs.lpszClass = AfxRegisterWndClass(0);
    return TRUE;
}

LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_SETTEXT)
    {
        CStringA mode;
        MakeModeName(mode);

        CString title;
        if (g_Main.IsExcuteService())
        {
            title = _T("ZoneServer");
        }
        else
        {
            title.Format(
                _T("RF Online: %S Server, %S DB (%S), MAX(%d), lim(%d)"),
                g_Main.m_szWorldName,
                g_Main.m_szWorldDBName,
                mode.GetString(),
                MAX_PLAYER,
                g_Main.m_nLimUserNum);
        }

        lParam = reinterpret_cast<LPARAM>(title.GetString());
    }
    else if (message == WM_CLOSE)
    {
        int result = IDYES;
        if (!g_Main.IsExcuteService())
        {
            result = ::MessageBoxA(m_hWnd, "Program Exit?", "Warning", MB_ICONQUESTION | MB_YESNO);
        }

        if (result == IDYES)
        {
            g_Main.gm_ServerClose();
            g_MapDisplay.m_MapExtend.m_bExtendMode = false;
        }
        else if (result == IDNO)
        {
            return 0;
        }
    }

    return CFrameWnd::WindowProc(message, wParam, lParam);
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

    g_pDoc->CreateDisplayView(this);
    if (g_pDoc->m_DisplayView.GetSafeHwnd() == nullptr)
    {
        return -1;
    }
    g_pDoc->CreateSheetView(this);
    if (g_pDoc->m_InfoSheet.GetSafeHwnd() == nullptr)
    {
        return -1;
    }
    g_pDoc->m_pwndMainFrame = this;
    g_pDoc->m_pwndMainView = &m_mainView;

    const bool createdStatusBar = m_statusBar.Create(this) != FALSE;
    const bool setIndicators =
        createdStatusBar &&
        (m_statusBar.SetIndicators(kStatusIndicators, static_cast<int>(std::size(kStatusIndicators))) != FALSE);
    if (setIndicators)
    {
        m_statusBar.SetPaneInfo(0, kStatusIndicators[0], 0, 60);
        m_statusBar.SetPaneInfo(1, kStatusIndicators[1], 0, 60);
        m_statusBar.SetPaneInfo(2, kStatusIndicators[2], 0, 80);
        m_statusBar.SetPaneInfo(3, kStatusIndicators[3], 0, 80);
        m_statusBar.SetPaneInfo(4, kStatusIndicators[4], 0, 80);
        m_statusBar.SetPaneInfo(5, kStatusIndicators[5], 0, 80);
        m_statusBar.SetPaneInfo(6, kStatusIndicators[6], 0, 60);
        m_statusBar.SetPaneInfo(7, kStatusIndicators[7], 0, 100);
        g_pDoc->m_pStatusBar = &m_statusBar;
        SetTimer(kStatusTimerId, 1000, nullptr);
    }
    else
    {
        g_pDoc->m_pStatusBar = nullptr;
    }

    CRect clientRect;
    GetClientRect(&clientRect);
    LayoutControls(clientRect.Width(), clientRect.Height());

    if (!g_Main.Init())
    {
        AfxMessageBox(L"CMainThread::Init() failed.");
    }
    else
    {
        g_pDoc->m_InfoSheet.m_tabMap.UpdateTab();
        CMapData *map = g_MapOper.GetMap(0);
        if (map && g_MapDisplay.OnDisplay(map, 0))
        {
            CRect drawRect;
            g_pDoc->m_DisplayView.GetDrawableRect(&drawRect);
            g_MapDisplay.InitCollLine(&drawRect);
            g_MapDisplay.InitDummy(&drawRect);
        }
    }

    g_pDoc->m_InfoSheet.m_tabTCP.UpdateTab();
    g_pDoc->m_InfoSheet.m_tabIPX.UpdateTab();
    g_pDoc->m_InfoSheet.m_tabServer.UpdateServerTab();
    g_pDoc->m_InfoSheet.m_tabObject.UpdateTab();
    UpdateStatusBar();
    SendMessage(WM_SETTEXT, 0, 0);

    return 0;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
    CFrameWnd::OnSize(nType, cx, cy);

    CRect wndRect;
    GetWindowRect(&wndRect);
    if (wndRect.Width() != kFixedWindowWidth || wndRect.Height() != kFixedWindowHeight)
    {
        MoveWindow(wndRect.left, wndRect.top, kFixedWindowWidth, kFixedWindowHeight, TRUE);
    }

    CRect clientRect;
    GetClientRect(&clientRect);
    LayoutControls(clientRect.Width(), clientRect.Height());
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

    const int rightWidth = max(240, min(300, cx / 3));
    const int leftWidth = max(100, cx - rightWidth - kMargin * 3);

    if (g_pDoc->m_DisplayView.GetSafeHwnd() != nullptr)
    {
        g_pDoc->m_DisplayView.MoveWindow(kMargin, contentTop, leftWidth, contentHeight, TRUE);
    }

    if (g_pDoc->m_InfoSheet.GetSafeHwnd() != nullptr)
    {
        const int rightX = kMargin + leftWidth + kMargin;
        g_pDoc->m_InfoSheet.MoveWindow(rightX, contentTop, rightWidth, contentHeight);
    }
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
    UpdateStatusBar();
    CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnDestroy()
{
    KillTimer(kStatusTimerId);
    CFrameWnd::OnDestroy();
}

void CMainFrame::OnPaint()
{
    CPaintDC dc(this);
    (void)dc;
}

void CMainFrame::OnButtonDisplayAll()
{
    if (!g_Main.IsReleaseServiceMode())
    {
        g_Main.m_GameMsg.PackingMsg(0x3F1u, 0, 0, 0);
    }
}

void CMainFrame::OnButtonMonster()
{
    if (!g_Main.IsReleaseServiceMode())
    {
        g_Main.m_GameMsg.PackingMsg(0x3EBu, 0, 0, 0);
    }
}

void CMainFrame::OnButtonAction()
{
    if (!g_Main.IsReleaseServiceMode())
    {
        g_Main.m_GameMsg.PackingMsg(0x3F2u, 0, 0, 0);
    }
}

void CMainFrame::OnButtonServerClose()
{
    if (!g_Main.IsReleaseServiceMode())
    {
        g_Main.m_GameMsg.PackingMsg(0x3F4u, 0, 0, 0);
    }
}

void CMainFrame::OnButtonDungeonLoad()
{
    if (!g_Main.IsReleaseServiceMode())
    {
        g_Main.m_GameMsg.PackingMsg(0x3F7u, 0, 0, 0);
    }
}

LRESULT CMainFrame::OnUiUpdateMap(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    if (g_pDoc->m_InfoSheet.GetSafeHwnd() != nullptr)
    {
        g_pDoc->m_InfoSheet.m_tabMap.UpdateTab();
    }
    return 0;
}

LRESULT CMainFrame::OnUiUpdateServer(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    if (g_pDoc->m_InfoSheet.GetSafeHwnd() != nullptr)
    {
        g_pDoc->m_InfoSheet.m_tabServer.UpdateServerTab();
    }
    return 0;
}

LRESULT CMainFrame::OnUiUpdateObject(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    if (g_pDoc->m_InfoSheet.GetSafeHwnd() != nullptr)
    {
        g_pDoc->m_InfoSheet.m_tabObject.UpdateTab();
    }
    return 0;
}

void CMainFrame::MakeModeName(CStringA &mode) const
{
    mode = g_Main.IsReleaseServiceMode() ? "SERVICE" : "DEV";
    mode += ": debug";
    mode += "(T)";
    CStringA nationCode;
    nationCode.Format("[%s]", CNationSettingManager::Instance()->GetNationCodeStr());
    mode += nationCode;
}

void CMainFrame::UpdateStatusBar()
{
    if (m_statusBar.GetSafeHwnd() == nullptr)
    {
        return;
    }

    if (!g_Main.IsExcuteService())
    {
        CString pane;
        pane.Format(_T("M: %u"), g_Main.m_MainFrameRate.GetFPS());
        m_statusBar.SetPaneText(0, pane, TRUE);

        const unsigned int conn0 = g_Network.m_pProcess[0] ? g_Network.m_pProcess[0]->m_nTryConnectCount : 0;
        const unsigned int conn1 = g_Network.m_pProcess[1] ? g_Network.m_pProcess[1]->m_nTryConnectCount : 0;
        pane.Format(_T("%u/%u"), conn0, conn1);
        m_statusBar.SetPaneText(1, pane, TRUE);

        pane.Format(_T("D: %u - %u"), g_Main.m_DBFrameRate.GetFPS(), g_Main.m_listDQSData.size());
        m_statusBar.SetPaneText(2, pane, TRUE);

        m_statusBar.SetPaneText(3, _T("I: X - X"), TRUE);
        m_statusBar.SetPaneText(4, _T("L: X - X"), TRUE);
        m_statusBar.SetPaneText(5, _T("Q: X - X"), TRUE);

        pane.Format(_T("%u/%u"), CPlayer::s_dwAbnormalCloseCount, CPlayer::s_dwTotalCloseCount);
        m_statusBar.SetPaneText(7, pane, TRUE);
    }

    CString openPane;
    openPane.Format(_T("OPN: %d - %d"), g_Main.m_bWorldOpen ? 1 : 0, g_Main.m_bWorldService ? 1 : 0);
    m_statusBar.SetPaneText(6, openPane, TRUE);
}
