#pragma once

#include <windows.h>

class CGameProgress
{
public:
  CGameProgress();
  virtual ~CGameProgress();

  virtual void Init(void);
  virtual BOOL Create(void);
  virtual BOOL Destroy(void);

  virtual BOOL MainLoop(BOOL pi_bIsActive);

  virtual HRESULT RestoreDeviceObjects(void) = 0;
  virtual HRESULT InvalidateDeviceObjects(void) = 0;

protected:
  virtual HRESULT FrameMove(void) = 0;
  virtual HRESULT Render(void) = 0;
  virtual BOOL InputProcess(void) = 0;

  virtual BOOL LoadData(void) = 0;
  virtual BOOL UnloadData(void) = 0;

public:
  virtual LRESULT MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

  BOOL IsCreated(void) const;

protected:
  BOOL m_bExit;
  BOOL m_bRequestExit;
  BOOL m_bIsCreated;
  BOOL m_bIsLoadedData;
};
