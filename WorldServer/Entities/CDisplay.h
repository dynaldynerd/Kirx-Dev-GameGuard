#pragma once

#include "IdaCompat.h"

#include <Windows.h>

class __cppobj CDisplay
{
public:
  CDisplay();
  virtual ~CDisplay();

  virtual HRESULT DestroyObjects();

  void *m_pDD;
  void *m_pddsFrontBuffer;
  void *m_pddsBackBuffer;
  void *m_pddsBackBufferLeft;
  HWND m_hWnd;
  RECT m_rcWindow;
  SIZE m_sizeWindow;
  int m_bWindowed;
  int m_bStereo;
};
