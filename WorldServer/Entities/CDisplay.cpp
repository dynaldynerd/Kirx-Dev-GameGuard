#include "pch.h"

#include "CDisplay.h"

#include <Unknwn.h>

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
  auto releaseUnknown = [](void *ptr) {
    if (ptr)
    {
      static_cast<IUnknown *>(ptr)->Release();
    }
  };

  if (m_pddsBackBufferLeft)
  {
    releaseUnknown(m_pddsBackBufferLeft);
    m_pddsBackBufferLeft = nullptr;
  }

  if (m_pddsBackBuffer)
  {
    releaseUnknown(m_pddsBackBuffer);
    m_pddsBackBuffer = nullptr;
  }

  if (m_pddsFrontBuffer)
  {
    releaseUnknown(m_pddsFrontBuffer);
    m_pddsFrontBuffer = nullptr;
  }

  if (m_pDD)
  {
    releaseUnknown(m_pDD);
    m_pDD = nullptr;
  }

  return S_OK;
}
