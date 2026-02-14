#include "pch.h"

#include "CMapDisplay.h"

#include <cstring>

#include "CMapData.h"

CMapDisplay::CMapDisplay()
  : CDisplay(),
    m_pSFMap(nullptr),
    m_pSFObj{},
    m_pSFCorpse(nullptr),
    m_pSFSelect(nullptr),
    m_pSFCircle(nullptr),
    m_pSFBuf(nullptr),
    m_nDummyDrawNum{},
    m_DummyDraw{},
    m_CollLineDraw{},
    m_rcWnd(),
    m_tmrDraw(),
    m_Font(),
    m_hPenBorder(nullptr),
    m_bDisplayMode(false),
    m_pActMap(nullptr),
    m_wLayerIndex(0),
    m_pOldActMap(nullptr),
    m_wOldLayerIndex(0),
    m_MapExtend()
{
  m_tmrDraw.BeginTimer(100u);
  m_hPenBorder = CreatePen(0, 2, 0);
  m_Font.CreateFont(15, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 32u, _T("Arial"));
  m_MapExtend.Init(&m_pSFMap);
}

CMapDisplay::~CMapDisplay()
{
  if (m_bDisplayMode)
  {
    ReleaseDisplay();
  }

  if (m_hPenBorder)
  {
    DeleteObject(m_hPenBorder);
    m_hPenBorder = nullptr;
  }

  m_Font.DeleteObject();

  for (int j = 0; j < 60; ++j)
  {
    if (m_DummyDraw[j])
    {
      delete[] m_DummyDraw[j];
      m_DummyDraw[j] = nullptr;
    }
  }
}

void CMapDisplay::DrawDisplay()
{
  // TODO: temporary non-GUI stub; replace with real MapDisplay integration when GUI subsystem is restored.
}

char CMapDisplay::OffDisplay()
{
  if (!m_bDisplayMode)
  {
    return 0;
  }

  if (FAILED(ReleaseDisplay()))
  {
    return 0;
  }

  std::memset(m_nDummyDrawNum, 0, sizeof(m_nDummyDrawNum));
  for (int j = 0; j < 60; ++j)
  {
    if (m_DummyDraw[j])
    {
      delete[] m_DummyDraw[j];
      m_DummyDraw[j] = nullptr;
    }
  }

  m_bDisplayMode = false;
  m_pOldActMap = m_pActMap;
  m_wOldLayerIndex = m_wLayerIndex;
  m_pActMap = nullptr;
  m_wLayerIndex = 0;
  return 1;
}

HRESULT CMapDisplay::ReleaseDisplay()
{
  if (m_pSFMap)
  {
    delete m_pSFMap;
    m_pSFMap = nullptr;
  }

  if (m_pSFBuf)
  {
    delete m_pSFBuf;
    m_pSFBuf = nullptr;
  }

  for (int j = 0; j < 2; ++j)
  {
    for (int k = 0; k < 13; ++k)
    {
      if (m_pSFObj[j][k])
      {
        delete m_pSFObj[j][k];
        m_pSFObj[j][k] = nullptr;
      }
    }
  }

  if (m_pSFSelect)
  {
    delete m_pSFSelect;
    m_pSFSelect = nullptr;
  }

  if (m_pSFCircle)
  {
    delete m_pSFCircle;
    m_pSFCircle = nullptr;
  }

  m_MapExtend.m_bExtendMode = false;
  return CDisplay::DestroyObjects();
}
