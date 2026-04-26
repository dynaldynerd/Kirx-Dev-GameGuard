#include "CGP_MainGame.h"

#include "RFClientApp.h"

CGP_MainGame::CGP_MainGame()
{
}

CGP_MainGame::~CGP_MainGame()
{
  Destroy();
}

BOOL CGP_MainGame::Create(void)
{
  if (m_bIsCreated)
  {
    return TRUE;
  }

  if (!CGameProgress::Create())
  {
    return FALSE;
  }

  if (!LoadData())
  {
    return FALSE;
  }

  m_bIsCreated = TRUE;
  return TRUE;
}

BOOL CGP_MainGame::Destroy(void)
{
  if (m_bIsLoadedData)
  {
    UnloadData();
  }

  m_bIsCreated = FALSE;
  return CGameProgress::Destroy();
}

HRESULT CGP_MainGame::RestoreDeviceObjects(void)
{
  return _GetMainApp() ? _GetMainApp()->RestoreDeviceObjects() : E_FAIL;
}

HRESULT CGP_MainGame::InvalidateDeviceObjects(void)
{
  return _GetMainApp() ? _GetMainApp()->InvalidateDeviceObjects() : E_FAIL;
}

LRESULT CGP_MainGame::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  return _GetMainApp() ? _GetMainApp()->MsgProcMainGame(hWnd, uMsg, wParam, lParam) : 1;
}

HRESULT CGP_MainGame::FrameMove(void)
{
  return _GetMainApp() ? _GetMainApp()->FrameMoveMainGame() : E_FAIL;
}

HRESULT CGP_MainGame::Render(void)
{
  return _GetMainApp() ? _GetMainApp()->RenderMainGame() : E_FAIL;
}

BOOL CGP_MainGame::InputProcess(void)
{
  return TRUE;
}

BOOL CGP_MainGame::LoadData(void)
{
  if (!_GetMainApp())
  {
    return FALSE;
  }

  if (!_GetMainApp()->LoadMainGameData())
  {
    return FALSE;
  }

  m_bIsLoadedData = TRUE;
  return TRUE;
}

BOOL CGP_MainGame::UnloadData(void)
{
  if (_GetMainApp())
  {
    _GetMainApp()->UnloadMainGameData();
  }

  m_bIsLoadedData = FALSE;
  return TRUE;
}
