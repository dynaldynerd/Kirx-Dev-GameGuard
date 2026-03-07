#include "pch.h"

#include "CMapDisplay.h"

#include <cstring>
#include <new>

#include "CDisplayView.h"
#include "CGameObject.h"
#include "CGameServerDoc.h"
#include "CMainThread.h"
#include "CMapData.h"
#include "CMapOperation.h"
#include "CMonster.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

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
  CDummyDraw::InitPen();
  CCollLineDraw::InitPen();
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

  CDummyDraw::DeletePen();
  CCollLineDraw::DeletePen();

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

char CMapDisplay::OnDisplay(CMapData *pMap, unsigned __int16 wLayerIndex)
{
  if (m_bDisplayMode)
  {
    return 0;
  }

  m_bDisplayMode = true;
  if (InitSurface(pMap) != 0)
  {
    return 0;
  }

  m_pActMap = pMap;
  m_wLayerIndex = 0;
  if (wLayerIndex >= pMap->m_pMapSet->m_nLayerNum)
  {
    m_wLayerIndex = 0;
  }
  return 1;
}

char CMapDisplay::ChangeMap(CMapData *pMap)
{
  if (!m_bDisplayMode)
  {
    return 0;
  }

  if (m_pActMap == pMap)
  {
    return 0;
  }

  InitSurface(pMap);
  m_pActMap = pMap;
  m_wLayerIndex = 0;
  return 1;
}

char CMapDisplay::ChangeLayer(unsigned __int16 wLayerIndex)
{
  if (!m_bDisplayMode)
  {
    return 0;
  }

  if (wLayerIndex < m_pActMap->m_pMapSet->m_nLayerNum && m_wLayerIndex != wLayerIndex)
  {
    m_wLayerIndex = wLayerIndex;
  }

  return 1;
}

void CMapDisplay::DrawDisplay()
{
  if (!m_bDisplayMode || !m_tmrDraw.CountingTimer())
  {
    return;
  }

  UpdateBounds();
  DrawMap();
  DrawCollisionLine();
  DrawDummy();
  DrawObject();
  DrawTextA();
  m_MapExtend.DrawRect();
  Blt(0, 0, m_pSFMap, nullptr);
  Present();
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

void CMapDisplay::DrawMap()
{
  LPDIRECTDRAWSURFACE7 mapSurface = m_pSFMap->GetDDrawSurface();
  LPDIRECTDRAWSURFACE7 bufferSurface = m_pSFBuf->GetDDrawSurface();
  mapSurface->BltFast(0, 0, bufferSurface, nullptr, 0);
}

void CMapDisplay::DrawCollisionLine()
{
  const int mapIndex = g_MapOper.GetMap(m_pActMap);
  CRect *area = nullptr;
  if (m_MapExtend.m_bExtendMode)
  {
    area = &m_MapExtend.m_rcExtend;
  }

  m_CollLineDraw[mapIndex].Draw(m_pSFMap, area);
}

void CMapDisplay::InitCollLine(CRect *prcWnd)
{
  for (int mapIndex = 0; mapIndex < g_MapOper.m_nMapNum; ++mapIndex)
  {
    CMapData *map = &g_MapOper.m_Map[mapIndex];
    m_CollLineDraw[mapIndex].InitLine(map, prcWnd);
  }
}

void CMapDisplay::InitDummy(CRect *prcWnd)
{
  for (int mapIndex = 0; mapIndex < g_MapOper.m_nMapNum; ++mapIndex)
  {
    CMapData *map = &g_MapOper.m_Map[mapIndex];
    m_nDummyDrawNum[mapIndex] = map->m_nQuestDumNum + map->m_nResDumNum + map->m_nBindDumNum + map->m_nStartDumNum
                              + map->m_nItemStoreDumNum + map->m_nPortalNum + map->m_nMonDumNum;

    if (m_nDummyDrawNum[mapIndex] > 3000)
    {
      MyMessageBox(
        "CMapDisplay::InitDummy",
        "%s map's dummy_num(%d) is more than max_dummy_num(%d)",
        map->m_pMapSet->m_strCode,
        m_nDummyDrawNum[mapIndex],
        3000);
      continue;
    }

    if (m_DummyDraw[mapIndex] != nullptr)
    {
      continue;
    }

    m_DummyDraw[mapIndex] = new (std::nothrow) CDummyDraw[m_nDummyDrawNum[mapIndex]];
    if (!m_DummyDraw[mapIndex])
    {
      continue;
    }

    int cursor = 0;
    for (int blockIndex = 0; blockIndex < map->m_nMonBlockNum; ++blockIndex)
    {
      _mon_block *block = &map->m_pMonBlock[blockIndex];
      for (unsigned int dummyIndex = 0; dummyIndex < block->m_pBlkRec->m_dwDummyNum; ++dummyIndex)
      {
        _dummy_position *dummy = block->m_pDumPos[dummyIndex];
        const int type = dummy->m_bPosAble ? 0 : 9;
        m_DummyDraw[mapIndex][cursor].SetDummyRange(
          map,
          dummy->m_fMin,
          dummy->m_fMax,
          dummy->m_fRT,
          dummy->m_fLB,
          type,
          prcWnd);
        ++cursor;
      }
    }

    for (int portalIndex = 0; portalIndex < map->m_nPortalNum; ++portalIndex)
    {
      _dummy_position *dummy = map->m_pPortal[portalIndex].m_pDumPos;
      const int type = dummy->m_bPosAble ? 1 : 9;
      m_DummyDraw[mapIndex][cursor].SetDummyRange(
        map,
        dummy->m_fMin,
        dummy->m_fMax,
        dummy->m_fRT,
        dummy->m_fLB,
        type,
        prcWnd);
      ++cursor;
    }

    for (int storeIndex = 0; storeIndex < map->m_nItemStoreDumNum; ++storeIndex)
    {
      _dummy_position *dummy = map->m_pItemStoreDummy[storeIndex].m_pDumPos;
      const int type = dummy->m_bPosAble ? 2 : 9;
      m_DummyDraw[mapIndex][cursor].SetDummyRange(
        map,
        dummy->m_fMin,
        dummy->m_fMax,
        dummy->m_fRT,
        dummy->m_fLB,
        type,
        prcWnd);
      ++cursor;
    }

    for (int startIndex = 0; startIndex < map->m_nStartDumNum; ++startIndex)
    {
      _dummy_position *dummy = map->m_pStartDummy[startIndex].m_pDumPos;
      const int type = dummy->m_bPosAble ? 3 : 9;
      m_DummyDraw[mapIndex][cursor].SetDummyRange(
        map,
        dummy->m_fMin,
        dummy->m_fMax,
        dummy->m_fRT,
        dummy->m_fLB,
        type,
        prcWnd);
      ++cursor;
    }

    for (int bindIndex = 0; bindIndex < map->m_nBindDumNum; ++bindIndex)
    {
      _dummy_position *dummy = map->m_pBindDummy[bindIndex].m_pDumPos;
      const int type = dummy->m_bPosAble ? 4 : 9;
      m_DummyDraw[mapIndex][cursor].SetDummyRange(
        map,
        dummy->m_fMin,
        dummy->m_fMax,
        dummy->m_fRT,
        dummy->m_fLB,
        type,
        prcWnd);
      ++cursor;
    }

    for (int questIndex = 0; questIndex < map->m_nQuestDumNum; ++questIndex)
    {
      _dummy_position *dummy = map->m_pQuestDummy[questIndex].m_pDumPos;
      const int type = dummy->m_bPosAble ? 8 : 9;
      m_DummyDraw[mapIndex][cursor].SetDummyRange(
        map,
        dummy->m_fMin,
        dummy->m_fMax,
        dummy->m_fRT,
        dummy->m_fLB,
        type,
        prcWnd);
      ++cursor;
    }
  }
}

void CMapDisplay::DrawDummy()
{
  CRect area{};
  CRect *activeArea = nullptr;
  if (m_MapExtend.m_bExtendMode)
  {
    area = m_MapExtend.m_rcExtend;
    activeArea = &area;
  }

  const int mapIndex = g_MapOper.GetMap(m_pActMap);
  for (int index = 0; index < m_nDummyDrawNum[mapIndex]; ++index)
  {
    m_DummyDraw[mapIndex][index].Draw(m_pSFMap, activeArea);
  }

  if (!m_MapExtend.m_bExtendMode)
  {
    return;
  }

  HDC hdc = nullptr;
  DDSURFACEDESC2 *surfaceDesc = m_pSFMap->GetDDSurfaceDesc();
  LPDIRECTDRAWSURFACE7 mapSurface = m_pSFMap->GetDDrawSurface();
  if (mapSurface->GetDC(&hdc) == DD_OK)
  {
    ::SelectObject(hdc, m_hPenBorder);
    MoveToEx(hdc, 0, 0, nullptr);
    LineTo(hdc, static_cast<int>(surfaceDesc->dwWidth), 0);
    LineTo(hdc, static_cast<int>(surfaceDesc->dwWidth), static_cast<int>(surfaceDesc->dwHeight));
    LineTo(hdc, 0, static_cast<int>(surfaceDesc->dwHeight));
    LineTo(hdc, 0, 0);
    mapSurface->ReleaseDC(hdc);
  }
}

void CMapDisplay::DrawObject()
{
  for (int index = 0; index < CGameObject::s_nTotalObjectNum; ++index)
  {
    CGameObject *object = CGameObject::s_pTotalObject[index];
    if (!object->m_bLive || object->m_pCurMap != m_pActMap || object->m_wMapLayerIndex != m_wLayerIndex)
    {
      continue;
    }

    CSurface *surface = m_pSFObj[object->m_ObjID.m_byKind][object->m_ObjID.m_byID];
    if (object->m_bCorpse)
    {
      surface = m_pSFCorpse;
    }
    _DrawObject(object, surface);
  }

  if (!CGameObject::s_pSelectObject)
  {
    return;
  }

  if (CGameObject::s_pSelectObject->m_pCurMap == m_pActMap && CGameObject::s_pSelectObject->m_wMapLayerIndex == m_wLayerIndex)
  {
    _DrawObject(CGameObject::s_pSelectObject, m_pSFSelect);
    if (!CGameObject::s_pSelectObject->m_ObjID.m_byKind && CGameObject::s_pSelectObject->m_ObjID.m_byID == 1)
    {
      DrawSelectMonsterLookAtPos(reinterpret_cast<CMonster *>(CGameObject::s_pSelectObject));
    }
  }
}

__int64 CMapDisplay::_DrawObject(CGameObject *pObj, CSurface *pSF)
{
  if (!pObj || !pSF)
  {
    return 0;
  }

  if (pObj->GetCurSecNum() == -1)
  {
    return 0;
  }

  if (m_MapExtend.m_bExtendMode)
  {
    if (pObj->m_fAbsPos[0] <= static_cast<float>(m_MapExtend.m_ptStartMap.x)
        || static_cast<float>(m_MapExtend.m_ptEndMap.x) <= pObj->m_fAbsPos[0]
        || pObj->m_fAbsPos[2] <= static_cast<float>(m_MapExtend.m_ptStartMap.y)
        || static_cast<float>(m_MapExtend.m_ptEndMap.y) <= pObj->m_fAbsPos[2])
    {
      pObj->m_nScreenPos[0] = -1;
      pObj->m_nScreenPos[1] = -1;
      return 0;
    }

    pObj->CalcScrExtendPoint(&m_rcWnd, &m_MapExtend.m_rcExtend);
    return m_pSFMap->GetDDrawSurface()->BltFast(
      pObj->m_nScreenPos[0],
      pObj->m_nScreenPos[1],
      pSF->GetDDrawSurface(),
      nullptr,
      DDBLTFAST_SRCCOLORKEY);
  }

  pObj->CalcScrNormalPoint(&m_rcWnd);
  return m_pSFMap->GetDDrawSurface()->BltFast(
    pObj->m_nScreenPos[0],
    pObj->m_nScreenPos[1],
    pSF->GetDDrawSurface(),
    nullptr,
    DDBLTFAST_SRCCOLORKEY);
}

__int64 CMapDisplay::DrawSelectMonsterLookAtPos(CMonster *pMon)
{
  if (!m_pSFCircle || !pMon)
  {
    return 0;
  }

  if (pMon->GetCurSecNum() == -1 || !pMon->m_pCurMap)
  {
    return 0;
  }

  _bsp_info *bspInfo = pMon->m_pCurMap->GetBspInfo();
  const float posX = static_cast<float>(-bspInfo->m_nMapMinSize[0]) + pMon->m_fLookAtPos[0];
  const float posZ = static_cast<float>(bspInfo->m_nMapMaxSize[2]) - pMon->m_fLookAtPos[2];

  const float localX = posX - static_cast<float>(m_MapExtend.m_rcExtend.left);
  const float localY = posZ - static_cast<float>(m_MapExtend.m_rcExtend.top);

  const float drawX = static_cast<float>(static_cast<int>(
    (localX * static_cast<float>(m_rcWnd.right))
    / static_cast<float>(m_MapExtend.m_rcExtend.right - m_MapExtend.m_rcExtend.left)));

  const float drawY = static_cast<float>(static_cast<int>(
    (localY * static_cast<float>(m_rcWnd.bottom))
    / static_cast<float>(m_MapExtend.m_rcExtend.bottom - m_MapExtend.m_rcExtend.top)));

  if (m_MapExtend.m_bExtendMode && posX > static_cast<float>(m_MapExtend.m_ptStartMap.x)
      && static_cast<float>(m_MapExtend.m_ptEndMap.x) > posX && posZ > static_cast<float>(m_MapExtend.m_ptStartMap.y)
      && static_cast<float>(m_MapExtend.m_ptEndMap.y) > posZ)
  {
    return m_pSFMap->GetDDrawSurface()->BltFast(
      static_cast<int>(drawX),
      static_cast<int>(drawY),
      m_pSFCircle->GetDDrawSurface(),
      nullptr,
      DDBLTFAST_SRCCOLORKEY);
  }

  return 0;
}

void CMapDisplay::DrawTextA()
{
  HDC hdc = nullptr;
  LPDIRECTDRAWSURFACE7 mapSurface = m_pSFMap->GetDDrawSurface();
  if (mapSurface->GetDC(&hdc) != DD_OK)
  {
    return;
  }

  SetBkMode(hdc, TRANSPARENT);
  SetTextAlign(hdc, TA_LEFT);
  SetTextColor(hdc, 16777215);
  ::SelectObject(hdc, static_cast<HFONT>(m_Font.GetSafeHandle()));

  char buffer[136]{};
  _map_fld *mapSet = g_MapDisplay.m_pActMap->m_pMapSet;
  sprintf_s(buffer, sizeof(buffer), "E:%s Layer: %d / %d", mapSet->m_strCode, m_wLayerIndex + 1, mapSet->m_nLayerNum);
  TextOutA(hdc, 0, 0, buffer, static_cast<int>(strlen(buffer)));

  if (m_MapExtend.m_bExtendMode)
  {
    _bsp_info *bspInfo = m_pActMap->GetBspInfo();
    SetTextColor(hdc, 65535);
    sprintf_s(
      buffer,
      sizeof(buffer),
      "MinX=%d, MinZ=%d, MaxX=%d, MaxZ=%d",
      m_MapExtend.m_rcExtend.left + bspInfo->m_nMapMinSize[0],
      bspInfo->m_nMapMaxSize[2] - m_MapExtend.m_rcExtend.bottom,
      m_MapExtend.m_rcExtend.right + bspInfo->m_nMapMinSize[0],
      bspInfo->m_nMapMaxSize[2] - m_MapExtend.m_rcExtend.top);
    TextOutA(hdc, 0, 12, buffer, static_cast<int>(strlen(buffer)));
  }

  mapSurface->ReleaseDC(hdc);
}

CGameObject *CMapDisplay::SelectObject(CPoint *pt)
{
  for (int index = 0; index < CGameObject::s_nTotalObjectNum; ++index)
  {
    CGameObject *object = CGameObject::s_pTotalObject[index];
    if (object->m_bLive && object->m_pCurMap == m_pActMap && object->m_wMapLayerIndex == m_wLayerIndex
        && pt->x >= object->m_nScreenPos[0] && pt->x <= object->m_nScreenPos[0] + 6 && pt->y >= object->m_nScreenPos[1]
        && pt->y <= object->m_nScreenPos[1] + 6)
    {
      return object;
    }
  }

  return nullptr;
}

int CMapDisplay::InitSurface(CMapData *pMap)
{
  if (m_bDisplayMode && ReleaseDisplay() != 0)
  {
    MyMessageBox("CMapDisplay Error", "Release Failure");
    return 1;
  }

  g_pDoc->m_DisplayView.GetDrawableRect(&m_rcWnd);
  int result = CreateWindowedDisplay(g_pDoc->m_DisplayView.GetSafeHwnd(), m_rcWnd.right, m_rcWnd.bottom);
  if (result < 0)
  {
    MyMessageBox("CMapDisplay Error", "CreateWindowedDisplay Failure");
    return result;
  }

  result = CreateSurface(&m_pSFMap, m_rcWnd.right, m_rcWnd.bottom);
  if (result < 0)
  {
    MyMessageBox("CMapDisplay Error", "CreateSurface Failure");
    return result;
  }

  result = CreateSurface(&m_pSFBuf, m_rcWnd.right, m_rcWnd.bottom);
  if (result < 0)
  {
    MyMessageBox("CMapDisplay Error", "CreateSurface Failure");
    return result;
  }

  result = m_pSFBuf->FillColor(10206546);
  if (result < 0)
  {
    MyMessageBox("CMapDisplay Error", "FillColor Failure");
    return result;
  }

  result = CreateObjSurface();
  if (result < 0)
  {
    MyMessageBox("CMapDisplay Error", "CreateObjSurface Failure");
  }

  return result;
}

int CMapDisplay::CreateObjSurface()
{
  const char *characterBitmaps[13] = {
    ".\\Bitmap\\Player.bmp",
    ".\\Bitmap\\Monster.bmp",
    ".\\Bitmap\\NPC.bmp",
    ".\\Bitmap\\Animus.bmp",
    ".\\Bitmap\\Tower.bmp",
    ".\\Bitmap\\Stone.bmp",
    ".\\Bitmap\\Keeper.bmp",
    ".\\Bitmap\\Trap.bmp",
    ".\\Bitmap\\Trap.bmp",
    ".\\Bitmap\\Trap.bmp",
    ".\\Bitmap\\Trap.bmp",
    ".\\Bitmap\\Trap.bmp",
    ".\\Bitmap\\Trap.bmp",
  };

  const char *miscBitmaps[5] = {
    ".\\Bitmap\\Item.bmp",
    ".\\Bitmap\\DungeonGate.bmp",
    ".\\Bitmap\\ParkingUnit.bmp",
    ".\\Bitmap\\Item.bmp",
    ".\\Bitmap\\Item.bmp",
  };

  int result = 0;
  for (int index = 0; index < 13; ++index)
  {
    result = CreateSurfaceFromBitmap(&m_pSFObj[0][index], characterBitmaps[index], 7u, 7u);
    if (result < 0)
    {
      return result;
    }
    m_pSFObj[0][index]->SetColorKey(0);
  }

  for (int index = 0; index < 5; ++index)
  {
    result = CreateSurfaceFromBitmap(&m_pSFObj[1][index], miscBitmaps[index], 7u, 7u);
    if (result < 0)
    {
      return result;
    }
    m_pSFObj[1][index]->SetColorKey(0);
  }

  result = CreateSurfaceFromBitmap(&m_pSFSelect, ".\\Bitmap\\Select.bmp", 7u, 7u);
  if (result < 0)
  {
    return result;
  }
  m_pSFSelect->SetColorKey(0);

  result = CreateSurfaceFromBitmap(&m_pSFCircle, ".\\Bitmap\\Circle.bmp", 7u, 7u);
  if (result < 0)
  {
    return result;
  }
  m_pSFCircle->SetColorKey(0);

  result = CreateSurfaceFromBitmap(&m_pSFCorpse, ".\\Bitmap\\Corpse.bmp", 7u, 7u);
  if (result < 0)
  {
    return result;
  }
  m_pSFCorpse->SetColorKey(0);
  return result;
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
