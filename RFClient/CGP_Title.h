#pragma once

#include "CGameProgress.h"

class CGP_Title : public CGameProgress
{
public:
  CGP_Title();
  ~CGP_Title();

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
