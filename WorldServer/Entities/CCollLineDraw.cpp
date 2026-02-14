#include "pch.h"

#include "CCollLineDraw.h"

CCollLineDraw::CCollLineDraw()
{
  m_pMap = nullptr;
  m_nPointNum = 0;
  m_Point = nullptr;
  m_nLineNum = 0;
  m_pLine = nullptr;
}

CCollLineDraw::~CCollLineDraw()
{
  if (m_Point)
  {
    operator delete[](m_Point);
  }
}

