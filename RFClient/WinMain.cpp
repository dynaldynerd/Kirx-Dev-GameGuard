#include <windows.h>

#include "RFClientApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int)
{
  CRFClientApp app;
  if (!app.Initialize(hInstance, lpCmdLine))
  {
    MessageBoxA(nullptr, "Failed to initialize RF_Online.", "RF_Online", MB_ICONERROR | MB_OK);
    return 1;
  }

  return app.Run();
}
