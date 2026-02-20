#pragma once

#include "IdaCompat.h"

#include <Windows.h>
#include <ddraw.h>

class CSurface;

class  CDisplay
{
public:
  CDisplay();
  virtual ~CDisplay();

  HRESULT CreateFullScreenDisplay(HWND hWnd, DWORD dwWidth, DWORD dwHeight, DWORD dwBPP);
  HRESULT CreateWindowedDisplay(HWND hWnd, DWORD dwWidth, DWORD dwHeight);
  HRESULT InitClipper();
  HRESULT UpdateBounds();
  virtual HRESULT DestroyObjects();

  HRESULT CreateSurface(CSurface **ppSurface, DWORD dwWidth, DWORD dwHeight);
  HRESULT CreateSurfaceFromBitmap(CSurface **ppSurface, const char *strBMP, DWORD dwDesiredWidth, DWORD dwDesiredHeight);
  HRESULT CreateSurfaceFromText(
    CSurface **ppSurface,
    HFONT hFont,
    char *strText,
    COLORREF crBackground,
    COLORREF crForeground);
  HRESULT CreatePaletteFromBitmap(LPDIRECTDRAWPALETTE *ppPalette, const char *strBMP);

  HRESULT Clear(DWORD dwColor = 0L);
  HRESULT ColorKeyBlt(DWORD x, DWORD y, LPDIRECTDRAWSURFACE7 pdds, RECT *prc = nullptr);
  HRESULT Blt(DWORD x, DWORD y, LPDIRECTDRAWSURFACE7 pdds, RECT *prc = nullptr, DWORD dwFlags = 0);
  HRESULT Blt(DWORD x, DWORD y, CSurface *pSurface, RECT *prc = nullptr);
  HRESULT ShowBitmap(HBITMAP hbm, LPDIRECTDRAWPALETTE pPalette = nullptr);
  HRESULT SetPalette(LPDIRECTDRAWPALETTE pPalette);
  HRESULT Present();

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
