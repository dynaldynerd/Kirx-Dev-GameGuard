#include "pch.h"
#include "WorldServer.h"
#include "MainFrm.h"
#include "Entities/CGameServerDoc.h"
#include "Entities/GlobalObjects.h"
#include "Entities/R3EngineState.h"
#include "resource.h"
#include <commctrl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CWorldServerApp, CWinApp)
END_MESSAGE_MAP()

CWorldServerApp theApp;

CWorldServerApp::CWorldServerApp() noexcept
{
}

BOOL CWorldServerApp::InitInstance()
{
    INITCOMMONCONTROLSEX initCtrls{};
    initCtrls.dwSize = sizeof(initCtrls);
    initCtrls.dwICC = ICC_WIN95_CLASSES | ICC_BAR_CLASSES | ICC_TAB_CLASSES | ICC_TREEVIEW_CLASSES;
    InitCommonControlsEx(&initCtrls);

    CWinApp::InitInstance();

    WSADATA wsaData{};
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        AfxMessageBox(L"WSAStartup failed.");
        return FALSE;
    }

    if (!m_iocp.Start(0))
    {
        AfxMessageBox(L"IOCP initialization failed.");
        WSACleanup();
        return FALSE;
    }

    g_pDoc = new CGameServerDoc();

    auto* pFrame = new CMainFrame();
    if (!pFrame->LoadFrame(IDR_MAINFRAME))
    {
        delete g_pDoc;
        g_pDoc = nullptr;
        m_iocp.Stop();
        WSACleanup();
        return FALSE;
    }

    m_pMainWnd = pFrame;
    pFrame->ShowWindow(SW_SHOW);
    pFrame->UpdateWindow();

    return TRUE;
}

int CWorldServerApp::ExitInstance()
{
    delete g_pDoc;
    g_pDoc = nullptr;
    m_iocp.Stop();
    WSACleanup();
    return CWinApp::ExitInstance();
}
