#include "pch.h"

#include "CDisplay.h"

#include <windowsx.h>

#include "CSurface.h"

namespace
{
LPDIRECTDRAW7 AsDD(void *ptr)
{
  return reinterpret_cast<LPDIRECTDRAW7>(ptr);
}

LPDIRECTDRAWSURFACE7 AsDDSurface(void *ptr)
{
  return reinterpret_cast<LPDIRECTDRAWSURFACE7>(ptr);
}
}

CDisplay::CDisplay()
  : m_pDD(nullptr),
    m_pddsFrontBuffer(nullptr),
    m_pddsBackBuffer(nullptr),
    m_pddsBackBufferLeft(nullptr),
    m_hWnd(nullptr),
    m_rcWindow{},
    m_sizeWindow{},
    m_bWindowed(FALSE),
    m_bStereo(FALSE)
{
}

CDisplay::~CDisplay()
{
  DestroyObjects();
}

HRESULT CDisplay::DestroyObjects()
{
  auto releaseSurface = [](void *&ptr) {
    if (ptr)
    {
      AsDDSurface(ptr)->Release();
      ptr = nullptr;
    }
  };

  releaseSurface(m_pddsBackBufferLeft);
  releaseSurface(m_pddsBackBuffer);
  releaseSurface(m_pddsFrontBuffer);

  if (m_pDD)
  {
    if (m_hWnd)
    {
      AsDD(m_pDD)->SetCooperativeLevel(m_hWnd, DDSCL_NORMAL);
    }
    AsDD(m_pDD)->Release();
    m_pDD = nullptr;
  }

  return S_OK;
}

HRESULT CDisplay::CreateFullScreenDisplay(HWND hWnd, DWORD dwWidth, DWORD dwHeight, DWORD dwBPP)
{
  HRESULT hr = S_OK;

  m_sizeWindow.cx = static_cast<LONG>(dwWidth);
  m_sizeWindow.cy = static_cast<LONG>(dwHeight);

  DestroyObjects();

  LPDIRECTDRAW7 pDD = nullptr;
  if (FAILED(DirectDrawCreateEx(nullptr, reinterpret_cast<void **>(&pDD), IID_IDirectDraw7, nullptr)))
  {
    return E_FAIL;
  }
  m_pDD = pDD;

  hr = pDD->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
  if (FAILED(hr))
  {
    return E_FAIL;
  }

  if (FAILED(pDD->SetDisplayMode(dwWidth, dwHeight, dwBPP, 0, 0)))
  {
    return E_FAIL;
  }

  DDSURFACEDESC2 ddsd{};
  ddsd.dwSize = sizeof(ddsd);
  ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
  ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_3DDEVICE;
  ddsd.dwBackBufferCount = 1;

  LPDIRECTDRAWSURFACE7 pFront = nullptr;
  if (FAILED(pDD->CreateSurface(&ddsd, &pFront, nullptr)))
  {
    return E_FAIL;
  }
  m_pddsFrontBuffer = pFront;

  DDSCAPS2 ddscaps{};
  ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
  LPDIRECTDRAWSURFACE7 pBack = nullptr;
  if (FAILED(pFront->GetAttachedSurface(&ddscaps, &pBack)))
  {
    return E_FAIL;
  }
  pBack->AddRef();
  m_pddsBackBuffer = pBack;

  m_hWnd = hWnd;
  m_bWindowed = FALSE;
  UpdateBounds();
  return S_OK;
}

HRESULT CDisplay::CreateWindowedDisplay(HWND hWnd, DWORD dwWidth, DWORD dwHeight)
{
  m_sizeWindow.cx = static_cast<LONG>(dwWidth);
  m_sizeWindow.cy = static_cast<LONG>(dwHeight);

  DestroyObjects();

  LPDIRECTDRAW7 pDD = nullptr;
  if (FAILED(DirectDrawCreateEx(nullptr, reinterpret_cast<void **>(&pDD), IID_IDirectDraw7, nullptr)))
  {
    return E_FAIL;
  }
  m_pDD = pDD;

  if (FAILED(pDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL)))
  {
    return E_FAIL;
  }

  DDSURFACEDESC2 ddsd{};
  ddsd.dwSize = sizeof(ddsd);
  ddsd.dwFlags = DDSD_CAPS;
  ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

  LPDIRECTDRAWSURFACE7 pFront = nullptr;
  if (FAILED(pDD->CreateSurface(&ddsd, &pFront, nullptr)))
  {
    return E_FAIL;
  }
  m_pddsFrontBuffer = pFront;

  ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
  ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
  ddsd.dwWidth = dwWidth;
  ddsd.dwHeight = dwHeight;

  LPDIRECTDRAWSURFACE7 pBack = nullptr;
  if (FAILED(pDD->CreateSurface(&ddsd, &pBack, nullptr)))
  {
    return E_FAIL;
  }
  m_pddsBackBuffer = pBack;

  LPDIRECTDRAWCLIPPER pClipper = nullptr;
  if (FAILED(pDD->CreateClipper(0, &pClipper, nullptr)))
  {
    return E_FAIL;
  }

  if (FAILED(pClipper->SetHWnd(0, hWnd)))
  {
    pClipper->Release();
    return E_FAIL;
  }

  if (FAILED(pFront->SetClipper(pClipper)))
  {
    pClipper->Release();
    return E_FAIL;
  }

  pClipper->Release();

  m_hWnd = hWnd;
  m_bWindowed = TRUE;
  UpdateBounds();
  return S_OK;
}

HRESULT CDisplay::InitClipper()
{
  if (!m_pDD || !m_pddsFrontBuffer)
  {
    return E_POINTER;
  }

  LPDIRECTDRAWCLIPPER pClipper = nullptr;
  if (FAILED(AsDD(m_pDD)->CreateClipper(0, &pClipper, nullptr)))
  {
    return E_FAIL;
  }

  pClipper->SetHWnd(0, m_hWnd);
  if (FAILED(AsDDSurface(m_pddsFrontBuffer)->SetClipper(pClipper)))
  {
    pClipper->Release();
    return E_FAIL;
  }

  pClipper->Release();
  return S_OK;
}

HRESULT CDisplay::UpdateBounds()
{
  if (m_bWindowed)
  {
    GetClientRect(m_hWnd, &m_rcWindow);
    POINT topLeft = {m_rcWindow.left, m_rcWindow.top};
    POINT bottomRight = {m_rcWindow.right, m_rcWindow.bottom};
    ClientToScreen(m_hWnd, &topLeft);
    ClientToScreen(m_hWnd, &bottomRight);
    m_rcWindow.left = topLeft.x;
    m_rcWindow.top = topLeft.y;
    m_rcWindow.right = bottomRight.x;
    m_rcWindow.bottom = bottomRight.y;
  }
  else
  {
    SetRect(&m_rcWindow, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
  }

  return S_OK;
}

HRESULT CDisplay::CreateSurface(CSurface **ppSurface, DWORD dwWidth, DWORD dwHeight)
{
  if (!m_pDD)
  {
    return E_POINTER;
  }
  if (!ppSurface)
  {
    return E_INVALIDARG;
  }

  DDSURFACEDESC2 ddsd{};
  ddsd.dwSize = sizeof(ddsd);
  ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
  ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
  ddsd.dwWidth = dwWidth;
  ddsd.dwHeight = dwHeight;

  CSurface *surface = new CSurface();
  if (!surface)
  {
    return E_OUTOFMEMORY;
  }

  HRESULT hr = surface->Create(AsDD(m_pDD), &ddsd);
  if (FAILED(hr))
  {
    delete surface;
    return hr;
  }

  *ppSurface = surface;
  return S_OK;
}

HRESULT CDisplay::CreateSurfaceFromBitmap(CSurface **ppSurface, const char *strBMP, DWORD dwDesiredWidth, DWORD dwDesiredHeight)
{
  if (!m_pDD || !strBMP || !ppSurface)
  {
    return E_INVALIDARG;
  }

  *ppSurface = nullptr;

  HBITMAP hBMP = static_cast<HBITMAP>(
    LoadImageA(GetModuleHandleA(nullptr), strBMP, IMAGE_BITMAP, dwDesiredWidth, dwDesiredHeight, LR_CREATEDIBSECTION));
  if (!hBMP)
  {
    hBMP = static_cast<HBITMAP>(
      LoadImageA(nullptr, strBMP, IMAGE_BITMAP, dwDesiredWidth, dwDesiredHeight, LR_LOADFROMFILE | LR_CREATEDIBSECTION));
    if (!hBMP)
    {
      return E_FAIL;
    }
  }

  BITMAP bmp{};
  GetObjectA(hBMP, sizeof(bmp), &bmp);

  DDSURFACEDESC2 ddsd{};
  ddsd.dwSize = sizeof(ddsd);
  ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
  ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
  ddsd.dwWidth = static_cast<DWORD>(bmp.bmWidth);
  ddsd.dwHeight = static_cast<DWORD>(bmp.bmHeight);

  CSurface *surface = new CSurface();
  if (!surface)
  {
    DeleteObject(hBMP);
    return E_OUTOFMEMORY;
  }

  HRESULT hr = surface->Create(AsDD(m_pDD), &ddsd);
  if (FAILED(hr))
  {
    delete surface;
    DeleteObject(hBMP);
    return hr;
  }

  hr = surface->DrawBitmap(hBMP, 0, 0, 0, 0);
  DeleteObject(hBMP);
  if (FAILED(hr))
  {
    delete surface;
    return hr;
  }

  *ppSurface = surface;
  return S_OK;
}

HRESULT CDisplay::CreateSurfaceFromText(
  CSurface **ppSurface,
  HFONT hFont,
  char *strText,
  COLORREF crBackground,
  COLORREF crForeground)
{
  if (!m_pDD || !strText || !ppSurface)
  {
    return E_INVALIDARG;
  }

  *ppSurface = nullptr;
  HDC hDC = GetDC(nullptr);
  if (!hDC)
  {
    return E_FAIL;
  }

  if (hFont)
  {
    SelectObject(hDC, hFont);
  }

  SIZE sizeText{};
  GetTextExtentPoint32A(hDC, strText, static_cast<int>(strlen(strText)), &sizeText);
  ReleaseDC(nullptr, hDC);

  DDSURFACEDESC2 ddsd{};
  ddsd.dwSize = sizeof(ddsd);
  ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
  ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
  ddsd.dwWidth = static_cast<DWORD>(sizeText.cx);
  ddsd.dwHeight = static_cast<DWORD>(sizeText.cy);

  CSurface *surface = new CSurface();
  if (!surface)
  {
    return E_OUTOFMEMORY;
  }

  HRESULT hr = surface->Create(AsDD(m_pDD), &ddsd);
  if (FAILED(hr))
  {
    delete surface;
    return hr;
  }

  hr = surface->DrawTextA(hFont, strText, 0, 0, crBackground, crForeground);
  if (FAILED(hr))
  {
    delete surface;
    return hr;
  }

  *ppSurface = surface;
  return S_OK;
}

HRESULT CDisplay::CreatePaletteFromBitmap(LPDIRECTDRAWPALETTE *ppPalette, const char *strBMP)
{
  if (!m_pDD || !strBMP || !ppPalette)
  {
    return E_INVALIDARG;
  }

  *ppPalette = nullptr;

  HRSRC hResource = FindResourceA(nullptr, strBMP, reinterpret_cast<LPCSTR>(RT_BITMAP));
  PALETTEENTRY palette[256]{};

  if (hResource)
  {
    auto *pbi = static_cast<BITMAPINFOHEADER *>(LockResource(LoadResource(nullptr, hResource)));
    if (!pbi)
    {
      return E_FAIL;
    }

    auto *pRGB = reinterpret_cast<RGBQUAD *>(reinterpret_cast<BYTE *>(pbi) + pbi->biSize);

    DWORD colorCount = 0;
    if (pbi->biSize >= sizeof(BITMAPINFOHEADER) && pbi->biBitCount <= 8)
    {
      colorCount = (pbi->biClrUsed == 0) ? (1u << pbi->biBitCount) : pbi->biClrUsed;
    }

    for (DWORD colorIndex = 0; colorIndex < colorCount; ++colorIndex)
    {
      palette[colorIndex].peRed = pRGB[colorIndex].rgbRed;
      palette[colorIndex].peGreen = pRGB[colorIndex].rgbGreen;
      palette[colorIndex].peBlue = pRGB[colorIndex].rgbBlue;
      palette[colorIndex].peFlags = 0;
    }

    return AsDD(m_pDD)->CreatePalette(DDPCAPS_8BIT, palette, ppPalette, nullptr);
  }

  HANDLE hFile = CreateFileA(strBMP, GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);
  if (hFile == INVALID_HANDLE_VALUE)
  {
    return E_FAIL;
  }

  BITMAPFILEHEADER bf{};
  BITMAPINFOHEADER bi{};
  DWORD bytesRead = 0;

  const BOOL readHeaderOk = ReadFile(hFile, &bf, sizeof(bf), &bytesRead, nullptr) && bytesRead == sizeof(bf)
                         && ReadFile(hFile, &bi, sizeof(bi), &bytesRead, nullptr) && bytesRead == sizeof(bi)
                         && ReadFile(hFile, palette, sizeof(palette), &bytesRead, nullptr) && bytesRead == sizeof(palette);
  CloseHandle(hFile);
  if (!readHeaderOk)
  {
    return E_FAIL;
  }

  DWORD colorCount = 0;
  if (bi.biSize == sizeof(BITMAPINFOHEADER) && bi.biBitCount <= 8)
  {
    colorCount = (bi.biClrUsed == 0) ? (1u << bi.biBitCount) : bi.biClrUsed;
  }

  for (DWORD colorIndex = 0; colorIndex < colorCount; ++colorIndex)
  {
    BYTE red = palette[colorIndex].peRed;
    palette[colorIndex].peRed = palette[colorIndex].peBlue;
    palette[colorIndex].peBlue = red;
  }

  return AsDD(m_pDD)->CreatePalette(DDPCAPS_8BIT, palette, ppPalette, nullptr);
}

HRESULT CDisplay::Clear(DWORD dwColor)
{
  if (!m_pddsBackBuffer)
  {
    return E_POINTER;
  }

  DDBLTFX fx{};
  fx.dwSize = sizeof(fx);
  fx.dwFillColor = dwColor;
  return AsDDSurface(m_pddsBackBuffer)->Blt(nullptr, nullptr, nullptr, DDBLT_COLORFILL, &fx);
}

HRESULT CDisplay::ColorKeyBlt(DWORD x, DWORD y, LPDIRECTDRAWSURFACE7 pdds, RECT *prc)
{
  if (!m_pddsBackBuffer)
  {
    return E_POINTER;
  }

  return AsDDSurface(m_pddsBackBuffer)->BltFast(x, y, pdds, prc, DDBLTFAST_SRCCOLORKEY);
}

HRESULT CDisplay::Blt(DWORD x, DWORD y, LPDIRECTDRAWSURFACE7 pdds, RECT *prc, DWORD dwFlags)
{
  if (!m_pddsBackBuffer)
  {
    return E_POINTER;
  }

  return AsDDSurface(m_pddsBackBuffer)->BltFast(x, y, pdds, prc, dwFlags);
}

HRESULT CDisplay::Blt(DWORD x, DWORD y, CSurface *pSurface, RECT *prc)
{
  if (!pSurface)
  {
    return E_INVALIDARG;
  }

  return pSurface->IsColorKeyed() ? Blt(x, y, pSurface->GetDDrawSurface(), prc, DDBLTFAST_SRCCOLORKEY)
                                  : Blt(x, y, pSurface->GetDDrawSurface(), prc, 0);
}

HRESULT CDisplay::ShowBitmap(HBITMAP hbm, LPDIRECTDRAWPALETTE pPalette)
{
  if (!m_pddsFrontBuffer || !m_pddsBackBuffer)
  {
    return E_POINTER;
  }

  if (pPalette)
  {
    AsDDSurface(m_pddsFrontBuffer)->SetPalette(pPalette);
  }

  CSurface backBuffer;
  backBuffer.Create(AsDDSurface(m_pddsBackBuffer));
  if (FAILED(backBuffer.DrawBitmap(hbm, 0, 0, 0, 0)))
  {
    return E_FAIL;
  }

  return Present();
}

HRESULT CDisplay::SetPalette(LPDIRECTDRAWPALETTE pPalette)
{
  if (!m_pddsFrontBuffer)
  {
    return E_POINTER;
  }

  return AsDDSurface(m_pddsFrontBuffer)->SetPalette(pPalette);
}

HRESULT CDisplay::Present()
{
  if (!m_pddsFrontBuffer || !m_pddsBackBuffer)
  {
    return E_POINTER;
  }

  LPDIRECTDRAWSURFACE7 pFront = AsDDSurface(m_pddsFrontBuffer);
  LPDIRECTDRAWSURFACE7 pBack = AsDDSurface(m_pddsBackBuffer);

  while (true)
  {
    HRESULT hr = m_bWindowed ? pFront->Blt(&m_rcWindow, pBack, nullptr, DDBLT_WAIT, nullptr) : pFront->Flip(nullptr, 0);

    if (hr == DDERR_SURFACELOST)
    {
      pFront->Restore();
      pBack->Restore();
    }

    if (hr != DDERR_WASSTILLDRAWING)
    {
      return hr;
    }
  }
}
