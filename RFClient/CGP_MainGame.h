#pragma once

#include "CGameProgress.h"

class CGP_MainGame : public CGameProgress
{
public:
  CGP_MainGame();
  ~CGP_MainGame();

  BOOL Create(void) override;
  BOOL Destroy(void) override;

  HRESULT RestoreDeviceObjects(void) override;
  HRESULT InvalidateDeviceObjects(void) override;
  LRESULT MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

protected:
  HRESULT FrameMove(void) override;
  HRESULT Render(void) override;
  BOOL InputProcess(void) override;

  BOOL LoadData(void) override;
  BOOL UnloadData(void) override;
};
