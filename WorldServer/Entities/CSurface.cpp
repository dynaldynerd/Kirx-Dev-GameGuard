#include "pch.h"

#include "CSurface.h"

CSurface::CSurface()
  : m_pdds(nullptr),
    m_ddsd{},
    m_bColorKeyed(FALSE)
{
}

CSurface::~CSurface()
{
  if (m_pdds)
  {
    m_pdds->Release();
    m_pdds = nullptr;
  }
}

LPDIRECTDRAWSURFACE7 CSurface::GetDDrawSurface() const
{
  return m_pdds;
}

DDSURFACEDESC2 *CSurface::GetDDSurfaceDesc()
{
  return &m_ddsd;
}

BOOL CSurface::IsColorKeyed() const
{
  return m_bColorKeyed;
}

HRESULT CSurface::Create(LPDIRECTDRAW7 pDD, DDSURFACEDESC2 *pddsd)
{
  if (!pDD || !pddsd)
  {
    return E_INVALIDARG;
  }

  HRESULT hr = pDD->CreateSurface(pddsd, &m_pdds, nullptr);
  if (FAILED(hr))
  {
    return hr;
  }

  m_ddsd.dwSize = sizeof(m_ddsd);
  m_pdds->GetSurfaceDesc(&m_ddsd);
  return S_OK;
}

HRESULT CSurface::Create(LPDIRECTDRAWSURFACE7 pdds)
{
  m_pdds = pdds;
  if (m_pdds)
  {
    m_pdds->AddRef();
    m_ddsd.dwSize = sizeof(m_ddsd);
    m_pdds->GetSurfaceDesc(&m_ddsd);
  }
  return S_OK;
}

HRESULT CSurface::Destroy()
{
  if (m_pdds)
  {
    m_pdds->Release();
    m_pdds = nullptr;
  }
  return S_OK;
}

HRESULT CSurface::DrawBitmap(HBITMAP hBMP, DWORD dwBMPOriginX, DWORD dwBMPOriginY, DWORD dwBMPWidth, DWORD dwBMPHeight)
{
  if (!hBMP || !m_pdds)
  {
    return E_INVALIDARG;
  }

  HRESULT hr = m_pdds->Restore();
  if (FAILED(hr))
  {
    return hr;
  }

  DDSURFACEDESC2 ddsd{};
  ddsd.dwSize = sizeof(ddsd);
  m_pdds->GetSurfaceDesc(&ddsd);
  if (ddsd.ddpfPixelFormat.dwFlags == DDPF_FOURCC)
  {
    return E_NOTIMPL;
  }

  HDC hDCImage = CreateCompatibleDC(nullptr);
  if (!hDCImage)
  {
    return E_FAIL;
  }
  SelectObject(hDCImage, hBMP);

  BITMAP bmp{};
  GetObjectA(hBMP, sizeof(bmp), &bmp);

  dwBMPWidth = (dwBMPWidth == 0) ? static_cast<DWORD>(bmp.bmWidth) : dwBMPWidth;
  dwBMPHeight = (dwBMPHeight == 0) ? static_cast<DWORD>(bmp.bmHeight) : dwBMPHeight;

  HDC hDC = nullptr;
  hr = m_pdds->GetDC(&hDC);
  if (FAILED(hr))
  {
    DeleteDC(hDCImage);
    return hr;
  }

  StretchBlt(
    hDC,
    0,
    0,
    static_cast<int>(ddsd.dwWidth),
    static_cast<int>(ddsd.dwHeight),
    hDCImage,
    static_cast<int>(dwBMPOriginX),
    static_cast<int>(dwBMPOriginY),
    static_cast<int>(dwBMPWidth),
    static_cast<int>(dwBMPHeight),
    SRCCOPY);

  hr = m_pdds->ReleaseDC(hDC);
  DeleteDC(hDCImage);
  return hr;
}

HRESULT CSurface::DrawBitmap(const char *strBMP, DWORD dwDesiredWidth, DWORD dwDesiredHeight)
{
  if (!m_pdds || !strBMP)
  {
    return E_INVALIDARG;
  }

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

  HRESULT hr = DrawBitmap(hBMP, 0, 0, 0, 0);
  DeleteObject(hBMP);
  return hr;
}

HRESULT CSurface::DrawTextA(
  HFONT hFont,
  char *strText,
  DWORD dwOriginX,
  DWORD dwOriginY,
  COLORREF crBackground,
  COLORREF crForeground)
{
  if (!m_pdds || !strText)
  {
    return E_INVALIDARG;
  }

  HRESULT hr = m_pdds->Restore();
  if (FAILED(hr))
  {
    return hr;
  }

  HDC hDC = nullptr;
  hr = m_pdds->GetDC(&hDC);
  if (FAILED(hr))
  {
    return hr;
  }

  SetBkColor(hDC, crBackground);
  SetTextColor(hDC, crForeground);
  SetBkMode(hDC, TRANSPARENT);
  if (hFont)
  {
    SelectObject(hDC, hFont);
  }

  TextOutA(hDC, static_cast<int>(dwOriginX), static_cast<int>(dwOriginY), strText, static_cast<int>(strlen(strText)));
  return m_pdds->ReleaseDC(hDC);
}

HRESULT CSurface::SetColorKey(DWORD dwColorKey)
{
  if (!m_pdds)
  {
    return E_POINTER;
  }

  m_bColorKeyed = TRUE;
  DDCOLORKEY ddck{};
  ddck.dwColorSpaceLowValue = ConvertGDIColor(dwColorKey);
  ddck.dwColorSpaceHighValue = ConvertGDIColor(dwColorKey);
  return m_pdds->SetColorKey(DDCKEY_SRCBLT, &ddck);
}

DWORD CSurface::ConvertGDIColor(COLORREF dwGDIColor)
{
  if (!m_pdds)
  {
  return 0;
  }

  COLORREF originalColor = CLR_INVALID;
  HDC hdc = nullptr;
  DWORD convertedColor = CLR_INVALID;

  if (dwGDIColor != CLR_INVALID && m_pdds->GetDC(&hdc) == DD_OK)
  {
    originalColor = GetPixel(hdc, 0, 0);
    SetPixel(hdc, 0, 0, dwGDIColor);
    m_pdds->ReleaseDC(hdc);
  }

  DDSURFACEDESC2 ddsd{};
  ddsd.dwSize = sizeof(ddsd);
  if (m_pdds->Lock(nullptr, &ddsd, DDLOCK_WAIT, nullptr) == DD_OK)
  {
    convertedColor = *reinterpret_cast<DWORD *>(ddsd.lpSurface);
    if (ddsd.ddpfPixelFormat.dwRGBBitCount < 32)
    {
      convertedColor &= (1u << ddsd.ddpfPixelFormat.dwRGBBitCount) - 1;
    }
    m_pdds->Unlock(nullptr);
  }

  if (dwGDIColor != CLR_INVALID && m_pdds->GetDC(&hdc) == DD_OK)
  {
    SetPixel(hdc, 0, 0, originalColor);
    m_pdds->ReleaseDC(hdc);
  }

  return convertedColor;
}

HRESULT CSurface::FillColor(COLORREF crColor)
{
  DDBLTFX fx{};
  fx.dwSize = sizeof(fx);
  fx.dwFillColor = crColor;
  return m_pdds->Blt(nullptr, nullptr, nullptr, DDBLT_COLORFILL | DDBLT_DONOTWAIT, &fx);
}

HRESULT CSurface::GetBitMaskInfo(DWORD dwBitMask, DWORD *pdwShift, DWORD *pdwBits)
{
  if (!pdwShift || !pdwBits)
  {
    return E_INVALIDARG;
  }

  DWORD shift = 0;
  DWORD bits = 0;

  while (dwBitMask && (dwBitMask & 1) == 0)
  {
    ++shift;
    dwBitMask >>= 1;
  }

  while ((dwBitMask & 1) != 0)
  {
    ++bits;
    dwBitMask >>= 1;
  }

  *pdwShift = shift;
  *pdwBits = bits;
  return S_OK;
}
