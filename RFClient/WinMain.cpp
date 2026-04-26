#include <windows.h>

#include "RFClientApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int)
{
  CMainApp *l_pApp = new CMainApp;
  if (!l_pApp)
  {
    MessageBoxA(nullptr, "Failed to allocate RF_Online application.", "RF_Online", MB_ICONERROR | MB_OK);
    return 1;
  }

  if (!l_pApp->Initialize(hInstance, lpCmdLine))
  {
    delete l_pApp;
    MessageBoxA(nullptr, "Failed to initialize RF_Online.", "RF_Online", MB_ICONERROR | MB_OK);
    return 1;
  }

  const int l_nExitCode = l_pApp->Run();
  delete l_pApp;
  return l_nExitCode;
}
