#include "pch.h"
#include "WorldServer.h"
#include "MainFrm.h"
#include "Entities/CGameServerDoc.h"
#include "Entities/CGameServerView.h"
#include "Entities/GlobalObjects.h"
#include "Entities/R3EngineState.h"
#include "resource.h"
#include <afxdisp.h>
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
    AfxEnableControlContainer();
    SetRegistryKey(_T("Local AppWizard-Generated Applications"));
    LoadStdProfileSettings(4);

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

    auto* docTemplate = new CSingleDocTemplate(
        IDR_MAINFRAME,
        RUNTIME_CLASS(CGameServerDoc),
        RUNTIME_CLASS(CMainFrame),
        RUNTIME_CLASS(CGameServerView));
    if (docTemplate == nullptr)
    {
        m_iocp.Stop();
        WSACleanup();
        return FALSE;
    }
    AddDocTemplate(docTemplate);

    CCommandLineInfo parsedCommandLine;
    ParseCommandLine(parsedCommandLine);

    g_Main.m_bServiceKeyPass = true;

    bool commandProcessed = false;
    if (parsedCommandLine.m_nShellCommand == CCommandLineInfo::FileNew
        || parsedCommandLine.m_nShellCommand == CCommandLineInfo::FileNothing)
    {
        commandProcessed = (docTemplate->OpenDocumentFile(nullptr) != nullptr);
    }
    else
    {
        commandProcessed = ProcessShellCommand(parsedCommandLine) != FALSE;
    }

    if (!commandProcessed)
    {
        m_iocp.Stop();
        WSACleanup();
        return FALSE;
    }

    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();

    return TRUE;
}

int CWorldServerApp::ExitInstance()
{
    g_pDoc = nullptr;
    m_iocp.Stop();
    WSACleanup();
    return CWinApp::ExitInstance();
}
