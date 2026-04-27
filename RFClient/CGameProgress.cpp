#include "CGameProgress.h"
#include "RFClientApp.h"

CGameProgress::CGameProgress()
{
  Init();
}

CGameProgress::~CGameProgress()
{
  Destroy();
}

void CGameProgress::Init(void)
{
  m_bExit = FALSE;
  m_bRequestExit = FALSE;
  m_bIsCreated = FALSE;
  m_bIsLoadedData = FALSE;
}

BOOL CGameProgress::Create(void)
{
  return TRUE;
}

BOOL CGameProgress::Destroy(void)
{
  return TRUE;
}

BOOL CGameProgress::MainLoop(BOOL pi_bIsActive)
{
  if (pi_bIsActive)
  {
    InputProcess();
  }
  if (m_bExit)
  {
    return TRUE;
  }

  FrameMove();
  const BOOL l_bShouldRender = pi_bIsActive ||
                               (_GetMainApp() && _GetMainApp()->GetGameProgressID() != GPI_MAIN_GAME);
  if (l_bShouldRender)
  {
    Render();
  }

  return FALSE;
}

BOOL CGameProgress::IsCreated(void) const
{
  return m_bIsCreated;
}
