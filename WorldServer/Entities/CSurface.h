#pragma once

#include "IdaCompat.h"

#include <Windows.h>
#include <ddraw.h>

class  CSurface
{
public:
  CSurface();
  ~CSurface();

  LPDIRECTDRAWSURFACE7 GetDDrawSurface() const;
  DDSURFACEDESC2 *GetDDSurfaceDesc();
  BOOL IsColorKeyed() const;

  HRESULT DrawBitmap(
    HBITMAP hBMP,
    DWORD dwBMPOriginX = 0,
    DWORD dwBMPOriginY = 0,
    DWORD dwBMPWidth = 0,
    DWORD dwBMPHeight = 0);
  HRESULT DrawBitmap(const char *strBMP, DWORD dwDesiredWidth, DWORD dwDesiredHeight);
  HRESULT DrawTextA(HFONT hFont, char *strText, DWORD dwOriginX, DWORD dwOriginY, COLORREF crBackground, COLORREF crForeground);

  HRESULT SetColorKey(DWORD dwColorKey);
  unsigned int ConvertGDIColor(COLORREF dwGDIColor);
  HRESULT FillColor(COLORREF crColor);
  static HRESULT GetBitMaskInfo(DWORD dwBitMask, DWORD *pdwShift, DWORD *pdwBits);

  HRESULT Create(LPDIRECTDRAW7 pDD, DDSURFACEDESC2 *pddsd);
  HRESULT Create(LPDIRECTDRAWSURFACE7 pdds);
  HRESULT Destroy();

  LPDIRECTDRAWSURFACE7 m_pdds;
  DDSURFACEDESC2 m_ddsd;
  BOOL m_bColorKeyed;
};
