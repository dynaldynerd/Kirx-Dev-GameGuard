#include "CGP_Title.h"

CGP_Title::CGP_Title()
{
}

CGP_Title::~CGP_Title()
{
  Destroy();
}

BOOL CGP_Title::Create(void)
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

  m_bExit = TRUE;
  m_bIsCreated = TRUE;
  return TRUE;
}

BOOL CGP_Title::Destroy(void)
{
  if (m_bIsLoadedData)
  {
    UnloadData();
  }

  m_bIsCreated = FALSE;
  return CGameProgress::Destroy();
}

HRESULT CGP_Title::RestoreDeviceObjects(void)
{
  return S_OK;
}

HRESULT CGP_Title::InvalidateDeviceObjects(void)
{
  return S_OK;
}

LRESULT CGP_Title::MsgProc(HWND, UINT uMsg, WPARAM wParam, LPARAM)
{
  (void)uMsg;
  (void)wParam;
  return 0;
}

HRESULT CGP_Title::FrameMove(void)
{
  return S_OK;
}

HRESULT CGP_Title::Render(void)
{
  return S_OK;
}

BOOL CGP_Title::InputProcess(void)
{
  m_bExit = TRUE;
  return TRUE;
}

BOOL CGP_Title::LoadData(void)
{
  m_bIsLoadedData = TRUE;
  return TRUE;
}

BOOL CGP_Title::UnloadData(void)
{
  m_bIsLoadedData = FALSE;
  return TRUE;
}
