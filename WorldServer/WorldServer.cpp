#include "pch.h"
#include "WorldServer.h"
#include "MainFrm.h"
#include "Entities/R3EngineState.h"
#include "resource.h"

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
    CWinApp::InitInstance();

    strcpy_s(stState.MapPath, ".");

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

    auto* pFrame = new CMainFrame();
    if (!pFrame->LoadFrame(IDR_MAINFRAME))
    {
        delete pFrame;
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
    m_iocp.Stop();
    WSACleanup();
    return CWinApp::ExitInstance();
}
