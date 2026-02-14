#include "pch.h"

#include "CSurface.h"
#include <Unknwn.h>

CSurface::CSurface()
  : m_pdds(nullptr),
    m_bColorKeyed(FALSE)
{
}

CSurface::~CSurface()
{
  if (m_pdds)
  {
    static_cast<IUnknown *>(m_pdds)->Release();
    m_pdds = nullptr;
  }
}
