#include "pch.h"

#include "MainFrm.h"

#include <iterator>
#include <cstring>

#include "Entities/DatabaseSetupDlg.h"
#include "Entities/ServerNameDlg.h"
#include "Entities/CGameServerDoc.h"
#include "Entities/CMainThread.h"
#include "Entities/CMapDisplay.h"
#include "Entities/CNationSettingManager.h"
#include "Entities/CPlayer.h"
#include "Entities/GlobalObjects.h"

namespace
{
constexpr int kFixedWindowWidth = 700;
constexpr int kFixedWindowHeight = 600;
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

LPCTSTR GetStartupStateTitle(CMainFrame::ServerStartupState state)
{
  switch (state)
  {
    case CMainFrame::kStarting:
      return _T("Starting");
    case CMainFrame::kStarted:
      return _T("Started");
    case CMainFrame::kNotStarted:
    default:
      return _T("Not Started");
  }
}

bool HasPendingShutdownLogout()
{
  return g_Main.GetShutdownPendingLogoutCount() != 0;
}
}

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_TIMER()
  ON_WM_DESTROY()
  ON_WM_PAINT()
  ON_COMMAND(ID_SETTINGS_DATABASE, &CMainFrame::OnSettingsDatabase)
  ON_COMMAND(ID_SETTINGS_CHANGE_SERVER_NAME, &CMainFrame::OnSettingsChangeServerName)
  ON_COMMAND(ID_SETTINGS_AUTOSTART, &CMainFrame::OnSettingsAutostart)
  ON_UPDATE_COMMAND_UI(ID_SETTINGS_AUTOSTART, &CMainFrame::OnUpdateSettingsAutostart)
END_MESSAGE_MAP()

CMainFrame::CMainFrame() noexcept
  : m_bExitConfirm(false),
    m_bAllowWindowClose(false),
    m_serverStartupState(kNotStarted),
    m_statusBar()
{
  g_pFrame = this;
}

CMainFrame::~CMainFrame() = default;

int CMainFrame::PreCreateWindow(CREATESTRUCT &cs)
{
  if (!CFrameWnd::PreCreateWindow(cs))
  {
    return 0;
  }

  cs.style &= 0xFF30FFFFu;
  cs.style |= 0xA0000u;
  return 1;
}

__int64 CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
  if (message == WM_CLOSE)
  {
    if (m_bAllowWindowClose)
    {
      return CFrameWnd::WindowProc(message, wParam, lParam);
    }

    if (m_serverStartupState == kStarting)
    {
      AfxMessageBox(L"WorldServer startup is still in progress.");
      return 0;
    }

    if (!IsServerStarted())
    {
      m_bAllowWindowClose = true;
      return CFrameWnd::WindowProc(message, wParam, lParam);
    }

    if (!m_bExitConfirm)
    {
      int messageResult = IDYES;
      if (!g_Main.IsExcuteService())
      {
        messageResult = ::MessageBoxA(m_hWnd, "Program Exit?", "Warning", MB_ICONQUESTION | MB_YESNO);
      }

      if (messageResult == IDYES)
      {
        m_bExitConfirm = true;
        g_Main.gm_ServerClose();
        g_MapDisplay.m_MapExtend.m_bExtendMode = false;
      }
    }

    return 0;
  }

  return CFrameWnd::WindowProc(message, wParam, lParam);
}

void CMainFrame::SetServerStartupState(ServerStartupState state)
{
  m_serverStartupState = state;
  UpdateStatusBar();
  RefreshWindowTitle();
  if (GetSafeHwnd() != nullptr)
  {
    UpdateWindow();
  }
}

CMainFrame::ServerStartupState CMainFrame::GetServerStartupState() const
{
  return m_serverStartupState;
}

bool CMainFrame::IsServerStarted() const
{
  return m_serverStartupState == kStarted;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
  {
    return -1;
  }

  if (!m_statusBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM, AFX_IDW_STATUS_BAR)
      || !m_statusBar.SetIndicators(kStatusIndicators, static_cast<int>(std::size(kStatusIndicators))))
  {
    AfxMessageBox(L"CMainFrame::OnCreate failed to create status bar.");
    return -1;
  }

  m_statusBar.SetPaneInfo(0, kStatusIndicators[0], 0, 60);
  m_statusBar.SetPaneInfo(1, kStatusIndicators[1], 0, 60);
  m_statusBar.SetPaneInfo(2, kStatusIndicators[2], 0, 80);
  m_statusBar.SetPaneInfo(3, kStatusIndicators[3], 0, 80);
  m_statusBar.SetPaneInfo(4, kStatusIndicators[4], 0, 80);
  m_statusBar.SetPaneInfo(5, kStatusIndicators[5], 0, 80);
  m_statusBar.SetPaneInfo(6, kStatusIndicators[6], 0, 60);
  m_statusBar.SetPaneInfo(7, kStatusIndicators[7], 0, 100);

  if (g_pDoc != nullptr)
  {
    g_pDoc->m_pwndMainFrame = this;
    g_pDoc->m_pStatusBar = &m_statusBar;
  }

  SetTimer(0, 1000, nullptr);
  UpdateStatusBar();
  RefreshWindowTitle();

  return 0;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
  CFrameWnd::OnSize(nType, cx, cy);

  CRect windowRect;
  GetWindowRect(&windowRect);
  if (windowRect.Width() != kFixedWindowWidth || windowRect.Height() != kFixedWindowHeight)
  {
    MoveWindow(windowRect.left, windowRect.top, kFixedWindowWidth, kFixedWindowHeight, TRUE);
  }
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
  if (nIDEvent == 0 && IsServerStarted() && m_bExitConfirm && !m_bAllowWindowClose)
  {
    if (!g_Main.IsServerClosing() && !HasPendingShutdownLogout())
    {
      FinalizeShutdown();
      return;
    }
  }

  UpdateStatusBar();
  RefreshWindowTitle();
  CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnDestroy()
{
  KillTimer(0);
  CFrameWnd::OnDestroy();
}

void CMainFrame::OnPaint()
{
  CPaintDC dc(this);
  (void)dc;
}

void CMainFrame::OnSettingsDatabase()
{
  CDatabaseSetupDlg dialog(this);
  if (dialog.DoModal() == IDOK)
  {
    AfxMessageBox(L"Setup files updated. Restart WorldServer for the changes to take effect.");
  }
}

void CMainFrame::OnSettingsChangeServerName()
{
  CServerNameDlg dialog(this);
  if (dialog.DoModal() != IDOK)
  {
    return;
  }

  AfxMessageBox(L"Server name saved to WorldInfo.ini. Restart WorldServer for the change to take effect.");
}

void CMainFrame::OnSettingsAutostart()
{
  const bool enableAutoStart = !CDatabaseSetupDlg::IsAutoStartEnabled();
  if (!CDatabaseSetupDlg::SetAutoStartEnabled(enableAutoStart))
  {
    AfxMessageBox(L"Failed to update Initialize\\settings.ini.");
    return;
  }

  AfxMessageBox(enableAutoStart
                  ? L"Autostart enabled. WorldServer will auto-start on the next launch."
                  : L"Autostart disabled. WorldServer will wait for Start on the next launch.");
}

void CMainFrame::OnUpdateSettingsAutostart(CCmdUI *pCmdUI)
{
  if (pCmdUI == nullptr)
  {
    return;
  }

  pCmdUI->SetCheck(CDatabaseSetupDlg::IsAutoStartEnabled() ? 1 : 0);
}

void CMainFrame::FinalizeShutdown()
{
  m_bAllowWindowClose = true;
  KillTimer(0);

  g_Main.m_bRuleThread = false;
  g_Main.m_bDQSThread = false;
  Sleep(3000);

  g_MapDisplay.OffDisplay();
  PostMessage(WM_CLOSE);
}

void CMainFrame::MakeModeName(CStringA &mode) const
{
  mode = g_Main.IsReleaseServiceMode() ? "SERVICE" : "DEV";
  mode += ": debug";
  mode += "(T)";

  char nationCode[5] = {};
  sprintf_s(nationCode, sizeof(nationCode), "[%s]", CNationSettingManager::Instance()->GetNationCodeStr());
  mode += nationCode;
}

void CMainFrame::BuildAsciiTitleToken(const char *source, char *destination, size_t destinationSize) const
{
  if (destination == nullptr || destinationSize == 0)
  {
    return;
  }

  if (source == nullptr)
  {
    destination[0] = '\0';
    return;
  }

  const size_t sourceLength = strlen(source);
  const size_t maxCopy = (destinationSize - 1 < sourceLength) ? destinationSize - 1 : sourceLength;
  for (size_t i = 0; i < maxCopy; ++i)
  {
    const unsigned char ch = static_cast<unsigned char>(source[i]);
    destination[i] = (ch >= 0x20 && ch <= 126) ? static_cast<char>(ch) : '?';
  }
  destination[maxCopy] = '\0';
}

void CMainFrame::RefreshWindowTitle()
{
  if (GetSafeHwnd() == nullptr)
  {
    return;
  }

  const LPCTSTR startupStateTitle = GetStartupStateTitle(m_serverStartupState);
  if (!IsServerStarted())
  {
    CString title;
    title.Format(_T("RF Online: WorldServer [%s]"), startupStateTitle);
    SetWindowText(title);
    return;
  }

  CStringA mode;
  MakeModeName(mode);

  CString title;
  if (g_Main.IsExcuteService())
  {
    title.Format(_T("ZoneServer [%s]"), startupStateTitle);
  }
  else
  {
    char worldName[64] = {};
    char worldDbName[64] = {};
    BuildAsciiTitleToken(g_Main.m_szWorldName, worldName, std::size(worldName));
    BuildAsciiTitleToken(g_Main.m_szWorldDBName, worldDbName, std::size(worldDbName));

    if (worldName[0] == '\0')
    {
      strcpy_s(worldName, "World");
    }
    if (worldDbName[0] == '\0')
    {
      strcpy_s(worldDbName, "RF_World");
    }

    title.Format(
      _T("RF Online: %S Server, %S DB (%S), MAX(%d), lim(%d) [%s]"),
      worldName,
      worldDbName,
      mode.GetString(),
      MAX_PLAYER,
      g_Main.m_nLimUserNum,
      startupStateTitle);
  }

  SetWindowText(title);
}

void CMainFrame::UpdateStatusBar()
{
  if (m_statusBar.GetSafeHwnd() == nullptr)
  {
    return;
  }

  if (!IsServerStarted())
  {
    for (int paneIndex = 0; paneIndex < 6; ++paneIndex)
    {
      m_statusBar.SetPaneText(paneIndex, _T(""), TRUE);
    }
    m_statusBar.SetPaneText(6, _T("OPN: 0 - 0"), TRUE);
    m_statusBar.SetPaneText(7, _T(""), TRUE);
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

    pane.Format(
      _T("I: %u - %u"),
      CPlayer::s_MgrItemHistory.m_FrameRate.GetFPS(),
      CPlayer::s_MgrItemHistory.GetTotalWaitSize());
    m_statusBar.SetPaneText(3, pane, TRUE);

    pane.Format(
      _T("L: %u - %u"),
      CPlayer::s_MgrLvHistory.m_FrameRate.GetFPS(),
      CPlayer::s_MgrLvHistory.GetTotalWaitSize());
    m_statusBar.SetPaneText(4, pane, TRUE);

    m_statusBar.SetPaneText(5, _T("Q: X - X"), TRUE);

    pane.Format(_T("%u/%u"), CPlayer::s_dwAbnormalCloseCount, CPlayer::s_dwTotalCloseCount);
    m_statusBar.SetPaneText(7, pane, TRUE);
  }

  CString openPane;
  openPane.Format(_T("OPN: %d - %d"), g_Main.m_bWorldOpen ? 1 : 0, g_Main.m_bWorldService ? 1 : 0);
  m_statusBar.SetPaneText(6, openPane, TRUE);
}
