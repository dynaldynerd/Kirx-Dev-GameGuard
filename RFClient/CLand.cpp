#include "CLand.h"

#include <cstring>
#include <new>

#include "ClientDataPath.h"
#include "DataFile.h"
#include "R3Engine/1stclass/r3d3d8.h"
#include "R3Engine/2ndclass/2dsprite.h"
#include "R3Engine/2ndclass/r3move.h"
#include "R3Engine/2ndclass/r3text.h"
#include "R3Engine/2ndclass/r3util.h"

CLand *g_pLand = NULL;
CLevel *g_pLevel = NULL;

namespace
{
BOOL ReadSourceData(CDataString *pi_pSourceData, void *po_pBuffer, int pi_nSize, int pi_nReadCount = 1)
{
  if (!pi_pSourceData || !po_pBuffer)
  {
    return FALSE;
  }

  if (!pi_nSize || !pi_nReadCount)
  {
    return TRUE;
  }

  const DWORD l_dwExpectedReadSize = static_cast<DWORD>(pi_nSize * pi_nReadCount);
  return pi_pSourceData->Read(po_pBuffer, pi_nSize, pi_nReadCount) == l_dwExpectedReadSize;
}

void CopyBufferString(char *po_pDestination,
                      size_t pi_nDestinationSize,
                      const char *pi_pSource,
                      size_t pi_nSourceSize)
{
  if (!po_pDestination || !pi_nDestinationSize)
  {
    return;
  }

  size_t l_nLength = 0;
  if (pi_pSource)
  {
    while (l_nLength < pi_nSourceSize && pi_pSource[l_nLength] != '\0')
    {
      ++l_nLength;
    }
  }

  if (l_nLength >= pi_nDestinationSize)
  {
    l_nLength = pi_nDestinationSize - 1;
  }

  if (l_nLength > 0)
  {
    memcpy(po_pDestination, pi_pSource, l_nLength);
  }
  po_pDestination[l_nLength] = '\0';
}

void CopyVector3f(Vector3f &po_vecDestination, const float *pi_pSource)
{
  po_vecDestination[0] = pi_pSource[0];
  po_vecDestination[1] = pi_pSource[1];
  po_vecDestination[2] = pi_pSource[2];
}

BOOL ReadMapOverlayString(CDataString *pi_pSourceData,
                          char *po_pDestination,
                          size_t pi_nDestinationSize,
                          DWORD pi_dwReadSize)
{
  if (!po_pDestination || !pi_nDestinationSize)
  {
    return FALSE;
  }

  ZeroMemory(po_pDestination, pi_nDestinationSize);
  if (pi_dwReadSize == 0)
  {
    return TRUE;
  }

  std::vector<char> l_vecBuffer(pi_dwReadSize);
  if (!ReadSourceData(pi_pSourceData,
                      &l_vecBuffer[0],
                      1,
                      static_cast<int>(pi_dwReadSize)))
  {
    return FALSE;
  }

  CopyBufferString(po_pDestination,
                   pi_nDestinationSize,
                   &l_vecBuffer[0],
                   l_vecBuffer.size());
  return TRUE;
}

BOOL LoadMapOverlayData(CDataString *pi_pMapSourceData,
                        CDataString *pi_pNationMapSourceData,
                        std::vector<MAP_OVERLAY_INFO> &po_vecOverlayData)
{
  po_vecOverlayData.clear();

  DWORD l_dwOverlayNum = 0;
  DWORD l_dwNationOverlayNum = 0;
  if (!ReadSourceData(pi_pMapSourceData, &l_dwOverlayNum, sizeof(l_dwOverlayNum)) ||
      !ReadSourceData(pi_pNationMapSourceData, &l_dwNationOverlayNum, sizeof(l_dwNationOverlayNum)))
  {
    return FALSE;
  }

  po_vecOverlayData.reserve(l_dwOverlayNum);
  for (DWORD i = 0; i < l_dwOverlayNum; ++i)
  {
    MAP_OVERLAY_INFO l_stOverlayInfo;
    l_stOverlayInfo.dwHeight = 0;
    l_stOverlayInfo.dwWidth = 0;
    ZeroMemory(l_stOverlayInfo.pMapName, sizeof(l_stOverlayInfo.pMapName));

    DWORD l_dwMapNameSize = 0;
    if (!ReadSourceData(pi_pMapSourceData, &l_stOverlayInfo.dwWidth, sizeof(l_stOverlayInfo.dwWidth)) ||
        !ReadSourceData(pi_pMapSourceData, &l_stOverlayInfo.dwHeight, sizeof(l_stOverlayInfo.dwHeight)) ||
        !ReadSourceData(pi_pMapSourceData, &l_dwMapNameSize, sizeof(l_dwMapNameSize)) ||
        !ReadMapOverlayString(pi_pMapSourceData,
                              l_stOverlayInfo.pMapName,
                              sizeof(l_stOverlayInfo.pMapName),
                              l_dwMapNameSize))
    {
      return FALSE;
    }

    DWORD l_dwPointNum = 0;
    if (!ReadSourceData(pi_pMapSourceData, &l_dwPointNum, sizeof(l_dwPointNum)))
    {
      return FALSE;
    }

    l_stOverlayInfo.vecPoint.resize(l_dwPointNum);
    for (DWORD j = 0; j < l_dwPointNum; ++j)
    {
      MAP_OVERLAY_POINT &l_stPoint = l_stOverlayInfo.vecPoint[j];
      ZeroMemory(&l_stPoint, sizeof(l_stPoint));

      if (!ReadSourceData(pi_pMapSourceData, &l_stPoint.dwColor, sizeof(l_stPoint.dwColor)) ||
          !ReadSourceData(pi_pMapSourceData, &l_stPoint.dwX, sizeof(l_stPoint.dwX)) ||
          !ReadSourceData(pi_pMapSourceData, &l_stPoint.dwY, sizeof(l_stPoint.dwY)) ||
          !ReadSourceData(pi_pMapSourceData, &l_stPoint.dwUnknown, sizeof(l_stPoint.dwUnknown)))
      {
        return FALSE;
      }
      l_stPoint.dwColor |= 0xFF000000;

      DWORD l_dwPointNameSize = 0;
      if (!ReadSourceData(pi_pNationMapSourceData, &l_dwPointNameSize, sizeof(l_dwPointNameSize)) ||
          !ReadMapOverlayString(pi_pNationMapSourceData,
                                l_stPoint.pName,
                                sizeof(l_stPoint.pName),
                                l_dwPointNameSize))
      {
        return FALSE;
      }
    }

    DWORD l_dwRawDataSize = 0;
    if (!ReadSourceData(pi_pMapSourceData, &l_dwRawDataSize, sizeof(l_dwRawDataSize)))
    {
      return FALSE;
    }

    if (l_dwRawDataSize > 0)
    {
      l_stOverlayInfo.vecRawData.resize(l_dwRawDataSize);
      if (!ReadSourceData(pi_pMapSourceData,
                          &l_stOverlayInfo.vecRawData[0],
                          static_cast<int>(l_dwRawDataSize)))
      {
        return FALSE;
      }
    }

    po_vecOverlayData.push_back(l_stOverlayInfo);
  }

  UNREFERENCED_PARAMETER(l_dwNationOverlayNum);
  return TRUE;
}
} // namespace

CLand::CLand()
{
  Init();
}

CLand::~CLand()
{
  Destroy();
}

void CLand::Init(void)
{
  m_pLevel = NULL;
  m_pMapInfo = NULL;
  m_byMaxMapNum = 0;
  m_byMapIndex = 0xFF;
  m_byPortalIndex = 0xFF;
}

BOOL CLand::Create(void)
{
  if (m_pLevel)
  {
    return TRUE;
  }

  m_pLevel = new CLevel;
  g_pLevel = m_pLevel;
  g_pLand = this;
  return TRUE;
}

BOOL CLand::Destroy(void)
{
  CLevel *l_pLevel = m_pLevel;
  if (m_pLevel)
  {
    UnloadLevel();
    delete m_pLevel;
    m_pLevel = NULL;
  }

  ReleaseData();

  if (g_pLevel == l_pLevel)
  {
    g_pLevel = NULL;
  }

  if (g_pLand == this)
  {
    g_pLand = NULL;
  }

  return TRUE;
}

BOOL CLand::LoadLevel(char *pi_pLevelName)
{
  if (!pi_pLevelName || !m_pLevel)
  {
    return FALSE;
  }

  m_pLevel->LoadLevel(pi_pLevelName);
  return m_pLevel->IsLoadedBsp();
}

BOOL CLand::LoadLevel(BYTE pi_byMapIndex)
{
  if (!m_pLevel)
  {
    return FALSE;
  }

  if (pi_byMapIndex == 0xFF)
  {
    pi_byMapIndex = m_byMapIndex;
  }

  MAP_INFO *l_pMapInfo = GetMapInfo(pi_byMapIndex);
  if (!l_pMapInfo)
  {
    return FALSE;
  }

  SetStateFullScreenEffect(FALSE);
  SetMapIndex(pi_byMapIndex);
  m_pLevel->LoadLevel(l_pMapInfo->pFileName);
  if (!m_pLevel->IsLoadedBsp())
  {
    return FALSE;
  }

  SetFadeIn(0x00, 0.0f, 1.0f);
  UI_SetRadar();
  return TRUE;
}

BOOL CLand::UnloadLevel(void)
{
  if (!m_pLevel)
  {
    return FALSE;
  }

  m_pLevel->ReleaseLevel();
  return TRUE;
}

BOOL CLand::Render(Vector3f &pi_vecPos)
{
  if (!m_pLevel || !m_pLevel->IsLoadedBsp())
  {
    return FALSE;
  }

  m_pLevel->SetCameraPos(pi_vecPos);
  m_pLevel->SetViewMatrix(R3MoveGetViewMatrix());
  m_pLevel->DrawSkyBoxRender();
  m_pLevel->DrawMapRender();
  m_pLevel->DrawMapEntitiesRender();
  return TRUE;
}

BOOL CLand::RenderAlpha(Vector3f &pi_vecPos)
{
  if (!m_pLevel || !m_pLevel->IsLoadedBsp())
  {
    return FALSE;
  }

  m_pLevel->DrawMapAlphaRender(pi_vecPos);
  return TRUE;
}

void CLand::OpenMinimapWindow(char *pi_pMapName, char *pi_pItemName)
{
  if (!pi_pMapName || !pi_pItemName)
  {
    return;
  }

  for (DWORD i = 0; i < m_byMaxMapNum; ++i)
  {
    MAP_INFO *l_pMapInfo = &m_pMapInfo[i];
    if (_stricmp(l_pMapInfo->pName, pi_pMapName) == 0)
    {
      strcpy_s(l_pMapInfo->pName_UI, sizeof(l_pMapInfo->pName_UI), pi_pItemName);
      UI_SetMinimap(static_cast<BYTE>(l_pMapInfo->byIndex));
      break;
    }
  }
}

void CLand::UI_SetMinimap(BYTE pi_byMapIndex)
{
  UNREFERENCED_PARAMETER(pi_byMapIndex);
}

void CLand::UI_OpenMoveMapWindow(void)
{
}

void CLand::UI_SetRadar(void)
{
}

void CLand::RenderInfo(void)
{
  MAP_INFO *l_pMapInfo = GetCurMapInfo();
  if (!m_pLevel || !l_pMapInfo)
  {
    return;
  }

  int l_nPosX = 10;
  int l_nPosY = 270;
  int l_nUsablePortalCnt = -1;
  char l_pBuf[128];
  static const char PORTAL_TYPE[2][16] = {"Road", "Way Point"};

  for (DWORD i = 0; i < l_pMapInfo->byPortalNum; ++i)
  {
    if (l_pMapInfo->pPortalInfo[i].bIsUsable)
    {
      ++l_nUsablePortalCnt;
    }
  }

  if (l_nUsablePortalCnt > -1)
  {
    Draw2DRectangle(5, 265, 350, l_nPosY + (l_nUsablePortalCnt * 15) + 20, 0x70020250);
  }

  l_nUsablePortalCnt = -1;
  for (DWORD i = 0; i < l_pMapInfo->byPortalNum; ++i)
  {
    const PORTAL_INFO &l_cPortalInfo = l_pMapInfo->pPortalInfo[i];
    if (l_cPortalInfo.bIsUsable)
    {
      ++l_nUsablePortalCnt;

      const DWORD l_dwPortalType = l_cPortalInfo.byPortalType < 2 ? l_cPortalInfo.byPortalType : 0;
      sprintf_s(l_pBuf,
                sizeof(l_pBuf),
                "< Ctrl + %d > : %s ( %s )",
                l_nUsablePortalCnt + 1,
                l_cPortalInfo.pDesc,
                PORTAL_TYPE[l_dwPortalType]);

      DrawR3Hangul(l_nPosX,
                   l_nPosY + (static_cast<int>(i) * 15),
                   l_pBuf,
                   D3DCOLOR_XRGB(234, 140, 0));
    }

    D3DXMATRIX l_matWorld;
    D3DXMatrixIdentity(&l_matWorld);
    GetD3dDevice()->SetTransform(D3DTS_WORLD, &l_matWorld);
    GetD3dDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);

    float l_vecMin[3] = {l_cPortalInfo.vecMin[0], l_cPortalInfo.vecMin[1], l_cPortalInfo.vecMin[2]};
    float l_vecMax[3] = {l_cPortalInfo.vecMax[0], l_cPortalInfo.vecMax[1], l_cPortalInfo.vecMax[2]};
    m_pLevel->DrawBBox(l_vecMin, l_vecMax, D3DCOLOR_XRGB(100, 255, 100));

    GetD3dDevice()->SetRenderState(D3DRS_LIGHTING, TRUE);
  }

  for (DWORD i = 0; i < l_pMapInfo->byMineNum; ++i)
  {
    D3DXMATRIX l_matWorld;
    D3DXMatrixIdentity(&l_matWorld);
    GetD3dDevice()->SetTransform(D3DTS_WORLD, &l_matWorld);
    GetD3dDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pLevel->DrawBBox(l_pMapInfo->pMineInfo[i].vecMin,
                       l_pMapInfo->pMineInfo[i].vecMax,
                       D3DCOLOR_XRGB(100, 255, 100));

    GetD3dDevice()->SetRenderState(D3DRS_LIGHTING, TRUE);
  }
}

BOOL CLand::LoadData(void)
{
  ReleaseData();

  char l_szMapInfoFileName[] = ".\\DataTable\\Map.edf";
  CDataFile l_fileMapInfo(l_szMapInfoFileName);
  CDataString *l_pMapSourceData = l_fileMapInfo.GetSourceData();
  if (!l_pMapSourceData || !l_pMapSourceData->GetString())
  {
    return FALSE;
  }

  char l_szNationMapInfoFileName[MAX_PATH];
  if (!GetNationDataFileName("NDMap.edf", l_szNationMapInfoFileName, sizeof(l_szNationMapInfoFileName)))
  {
    return FALSE;
  }

  CDataFile l_fileNationMapInfo(l_szNationMapInfoFileName);
  CDataString *l_pNationMapSourceData = l_fileNationMapInfo.GetSourceData();
  if (!l_pNationMapSourceData || !l_pNationMapSourceData->GetString())
  {
    return FALSE;
  }

  DWORD l_dwMapNum = 0;
  DWORD l_dwNationMapNum = 0;
  if (!ReadSourceData(l_pMapSourceData, &l_dwMapNum, sizeof(l_dwMapNum)) ||
      !ReadSourceData(l_pNationMapSourceData, &l_dwNationMapNum, sizeof(l_dwNationMapNum)) ||
      l_dwMapNum == 0 ||
      l_dwMapNum != l_dwNationMapNum)
  {
    return FALSE;
  }

  m_pMapInfo = new (std::nothrow) MAP_INFO[l_dwMapNum];
  if (!m_pMapInfo)
  {
    return FALSE;
  }

  ZeroMemory(m_pMapInfo, sizeof(MAP_INFO) * l_dwMapNum);
  SetMaxMapNum(l_dwMapNum);

  for (DWORD i = 0; i < l_dwMapNum; ++i)
  {
    MAP_INFO *l_pMapInfo = &m_pMapInfo[i];
    _base l_cBase;
    char l_szMapUIName[64];

    ZeroMemory(&l_cBase, sizeof(l_cBase));
    ZeroMemory(l_szMapUIName, sizeof(l_szMapUIName));
    if (!ReadSourceData(l_pMapSourceData, &l_cBase, sizeof(l_cBase)) ||
        !ReadSourceData(l_pNationMapSourceData, l_szMapUIName, sizeof(l_szMapUIName)))
    {
      ReleaseData();
      return FALSE;
    }

    l_pMapInfo->byIndex = l_cBase.index;
    CopyBufferString(l_pMapInfo->pName, sizeof(l_pMapInfo->pName), l_cBase.map, sizeof(l_cBase.map));
    CopyBufferString(l_pMapInfo->pName_UI, sizeof(l_pMapInfo->pName_UI), l_szMapUIName, sizeof(l_szMapUIName));
    CopyBufferString(l_pMapInfo->pFileName, sizeof(l_pMapInfo->pFileName), l_cBase.BSP, sizeof(l_cBase.BSP));

    char l_szMinimapFileName[MAX_PATH];
    ZeroMemory(l_szMinimapFileName, sizeof(l_szMinimapFileName));
    sprintf_s(l_szMinimapFileName, sizeof(l_szMinimapFileName), "./SpriteImage/Common/Map/%s", l_cBase.minimapSPR);
    strcpy_s(l_pMapInfo->pMinimapFileName, sizeof(l_pMapInfo->pMinimapFileName), l_szMinimapFileName);

    l_pMapInfo->wMinimapAngle = l_cBase.wMinimapAngle;
    l_pMapInfo->byPortalNum = l_cBase.portalNum;

    if (l_pMapInfo->byPortalNum > 0)
    {
      l_pMapInfo->pPortalInfo = new (std::nothrow) PORTAL_INFO[l_pMapInfo->byPortalNum];
      if (!l_pMapInfo->pPortalInfo)
      {
        ReleaseData();
        return FALSE;
      }

      ZeroMemory(l_pMapInfo->pPortalInfo, sizeof(PORTAL_INFO) * l_pMapInfo->byPortalNum);
      for (DWORD j = 0; j < l_pMapInfo->byPortalNum; ++j)
      {
        PORTAL_INFO *l_pPortalInfo = &l_pMapInfo->pPortalInfo[j];
        _portal_data l_cPortalData;

        ZeroMemory(&l_cPortalData, sizeof(l_cPortalData));
        if (!ReadSourceData(l_pMapSourceData, &l_cPortalData, sizeof(l_cPortalData)))
        {
          ReleaseData();
          return FALSE;
        }

        l_pPortalInfo->byMapIndex = l_cPortalData.mapindex;
        l_pPortalInfo->byIndex = l_cPortalData.index;
        l_pPortalInfo->byPortalType = l_cPortalData.byPortalType;
        l_pPortalInfo->bIsMovable = l_cPortalData.bIsMovable;
        memcpy(l_pPortalInfo->byUsableRace, l_cPortalData.byUsableRace, sizeof(l_pPortalInfo->byUsableRace));
        CopyBufferString(l_pPortalInfo->pName, sizeof(l_pPortalInfo->pName), l_cPortalData.pName, sizeof(l_cPortalData.pName));
        CopyBufferString(l_pPortalInfo->pDesc, sizeof(l_pPortalInfo->pDesc), l_cPortalData.pDesc, sizeof(l_cPortalData.pDesc));
        CopyVector3f(l_pPortalInfo->vecMin, l_cPortalData.MinRadius);
        CopyVector3f(l_pPortalInfo->vecMax, l_cPortalData.MaxRadius);
        memcpy(l_pPortalInfo->SPTCoor, l_cPortalData.SPTCoor, sizeof(l_pPortalInfo->SPTCoor));
        CopyVector3f(l_pPortalInfo->vecCenter, l_cPortalData.CenterCoor);
        l_pPortalInfo->fAngleCenter = l_cPortalData.anglecenter;
        l_pPortalInfo->byLinkedPortalNum = l_cPortalData.byLinkedPortalNum;
        l_pPortalInfo->bIsUsable = FALSE;
      }

      for (DWORD j = 0; j < l_pMapInfo->byPortalNum; ++j)
      {
        PORTAL_INFO *l_pPortalInfo = &l_pMapInfo->pPortalInfo[j];
        if (!ReadSourceData(l_pNationMapSourceData, l_pPortalInfo->pDesc, sizeof(l_pPortalInfo->pDesc)))
        {
          ReleaseData();
          return FALSE;
        }
        l_pPortalInfo->pDesc[sizeof(l_pPortalInfo->pDesc) - 1] = '\0';

        if (l_pPortalInfo->byLinkedPortalNum > 0)
        {
          l_pPortalInfo->pLinkedPortal = new (std::nothrow) _portal_link[l_pPortalInfo->byLinkedPortalNum];
          if (!l_pPortalInfo->pLinkedPortal ||
              !ReadSourceData(l_pMapSourceData,
                              l_pPortalInfo->pLinkedPortal,
                              sizeof(_portal_link),
                              l_pPortalInfo->byLinkedPortalNum))
          {
            ReleaseData();
            return FALSE;
          }
        }
      }
    }

    if (!ReadSourceData(l_pMapSourceData, &l_pMapInfo->byMineNum, sizeof(l_pMapInfo->byMineNum)))
    {
      ReleaseData();
      return FALSE;
    }

    if (l_pMapInfo->byMineNum > 0)
    {
      l_pMapInfo->pMineInfo = new (std::nothrow) MINE_INFO[l_pMapInfo->byMineNum];
      if (!l_pMapInfo->pMineInfo)
      {
        ReleaseData();
        return FALSE;
      }

      ZeroMemory(l_pMapInfo->pMineInfo, sizeof(MINE_INFO) * l_pMapInfo->byMineNum);
      for (DWORD j = 0; j < l_pMapInfo->byMineNum; ++j)
      {
        _ore_data l_cOreData;
        ZeroMemory(&l_cOreData, sizeof(l_cOreData));
        if (!ReadSourceData(l_pMapSourceData, &l_cOreData, sizeof(l_cOreData)))
        {
          ReleaseData();
          return FALSE;
        }

        l_pMapInfo->pMineInfo[j].byMapIndex = l_cOreData.mapindex;
        l_pMapInfo->pMineInfo[j].byIndex = l_cOreData.index;
        memcpy(l_pMapInfo->pMineInfo[j].vecMin, l_cOreData.radmin, sizeof(l_cOreData.radmin));
        memcpy(l_pMapInfo->pMineInfo[j].vecMax, l_cOreData.radmax, sizeof(l_cOreData.radmax));
        memcpy(l_pMapInfo->pMineInfo[j].fUnknown, l_cOreData.unknown, sizeof(l_cOreData.unknown));
      }
    }

    if (!ReadSourceData(l_pMapSourceData, &l_pMapInfo->byAreaNum, sizeof(l_pMapInfo->byAreaNum)))
    {
      ReleaseData();
      return FALSE;
    }

    if (l_pMapInfo->byAreaNum > 0)
    {
      l_pMapInfo->pAreaInfo = new (std::nothrow) MAP_AREA_INFO[l_pMapInfo->byAreaNum];
      if (!l_pMapInfo->pAreaInfo)
      {
        ReleaseData();
        return FALSE;
      }

      ZeroMemory(l_pMapInfo->pAreaInfo, sizeof(MAP_AREA_INFO) * l_pMapInfo->byAreaNum);
      for (DWORD j = 0; j < l_pMapInfo->byAreaNum; ++j)
      {
        _area_info l_cAreaInfo;
        ZeroMemory(&l_cAreaInfo, sizeof(l_cAreaInfo));
        if (!ReadSourceData(l_pMapSourceData, &l_cAreaInfo, sizeof(l_cAreaInfo)) ||
            !ReadSourceData(l_pNationMapSourceData, l_cAreaInfo.AreaName, sizeof(l_cAreaInfo.AreaName)))
        {
          ReleaseData();
          return FALSE;
        }

        l_pMapInfo->pAreaInfo[j].dwIndex = l_cAreaInfo.index;
        l_pMapInfo->pAreaInfo[j].recArea.left = l_cAreaInfo.X1;
        l_pMapInfo->pAreaInfo[j].recArea.top = l_cAreaInfo.Y1;
        l_pMapInfo->pAreaInfo[j].recArea.right = l_cAreaInfo.X2;
        l_pMapInfo->pAreaInfo[j].recArea.bottom = l_cAreaInfo.Y2;
        CopyBufferString(l_pMapInfo->pAreaInfo[j].pName,
                         sizeof(l_pMapInfo->pAreaInfo[j].pName),
                         l_cAreaInfo.AreaName,
                         sizeof(l_cAreaInfo.AreaName));
      }
    }

    if (!ReadSourceData(l_pMapSourceData, &l_pMapInfo->TheRest, sizeof(l_pMapInfo->TheRest)))
    {
      ReleaseData();
      return FALSE;
    }
  }

  if (!LoadMapOverlayData(l_pMapSourceData,
                          l_pNationMapSourceData,
                          m_vecMapOverlayData) ||
      !LoadMapOverlayData(l_pMapSourceData,
                          l_pNationMapSourceData,
                          m_vecWorldMapOverlayData))
  {
    ReleaseData();
    return FALSE;
  }

  return TRUE;
}

void CLand::ReleaseData(void)
{
  if (m_pMapInfo)
  {
    for (DWORD i = 0; i < m_byMaxMapNum; ++i)
    {
      ReleaseMapInfo(&m_pMapInfo[i]);
    }

    delete[] m_pMapInfo;
    m_pMapInfo = NULL;
  }

  m_byMaxMapNum = 0;
  m_byMapIndex = 0xFF;
  m_byPortalIndex = 0xFF;
  m_vecMapOverlayData.clear();
  m_vecWorldMapOverlayData.clear();
}

void CLand::ReleaseMapInfo(MAP_INFO *pi_pMapInfo)
{
  if (!pi_pMapInfo)
  {
    return;
  }

  if (pi_pMapInfo->pPortalInfo)
  {
    for (DWORD i = 0; i < pi_pMapInfo->byPortalNum; ++i)
    {
      delete[] pi_pMapInfo->pPortalInfo[i].pLinkedPortal;
      pi_pMapInfo->pPortalInfo[i].pLinkedPortal = NULL;
    }

    delete[] pi_pMapInfo->pPortalInfo;
    pi_pMapInfo->pPortalInfo = NULL;
  }

  delete[] pi_pMapInfo->pMineInfo;
  pi_pMapInfo->pMineInfo = NULL;

  delete[] pi_pMapInfo->pAreaInfo;
  pi_pMapInfo->pAreaInfo = NULL;

  pi_pMapInfo->byPortalNum = 0;
  pi_pMapInfo->byMineNum = 0;
  pi_pMapInfo->byAreaNum = 0;
}
