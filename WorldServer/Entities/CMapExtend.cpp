#include "pch.h"

#include "CMapExtend.h"

CMapExtend::CMapExtend()
  : m_bExtendMode(false),
    m_hPen(CreatePen(0, 1, 0x646464u)),
    m_pSF(nullptr),
    m_bSetArea(0),
    m_bMove(0)
{
}

CMapExtend::CMapExtend(CSurface **pSF)
  : m_bExtendMode(false),
    m_hPen(CreatePen(0, 1, 0x646464u)),
    m_pSF(pSF),
    m_bSetArea(0),
    m_bMove(0)
{
}

CMapExtend::~CMapExtend()
{
  DeleteObject(m_hPen);
}

void CMapExtend::Init(CSurface **pSF)
{
  m_pSF = pSF;
}
