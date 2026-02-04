#include "pch.h"

#include "CMapOperation.h"

#include <new>
#include <cstdio>
#include <cstring>

#include "CMapData.h"
#include "CItemStore.h"
#include "CItemStoreManager.h"
#include "CTransportShip.h"
#include "CHolyStoneSystem.h"
#include "CWorldSchedule.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

const char *CMapOperation::ms_szSettlementMapName[3][2] = {
    {"NeutralBS1", "NeutralBS2"},
    {"NeutralCS1", "NeutralCS2"},
    {"NeutralAS1", "NeutralAS2"}
};

CMapOperation g_MapOper;

AreaList *g_AreaIndexTable[100];
std::map<std::string, AreaList> g_strLMapMap;

bool LoadRegionData(int nMapNum, char **ppszMapNameList, char *pszErrMsg);

bool CMapOperation::Init()
{
  if (!m_tblMapData.ReadScript(".\\Map\\Map_Data.spt"))
  {
    MyMessageBox("DatafileInit", "MapSptfile Load Error");
    return false;
  }

  m_nMapNum = m_tblMapData.GetRecordNum();
  m_Map = new (std::nothrow) CMapData[m_nMapNum];

  if (!CItemStoreManager::Instance()->Init(m_nMapNum, 90))
  {
    return false;
  }

  if (!LoadMaps())
  {
    return false;
  }

  if (!LoadRegion())
  {
    return false;
  }

  CMapData *pLinkShipMap = GetMap("Transport01");
  CMapData *pLinkPlatformMap = GetMap("Platform01");

  if (pLinkShipMap && pLinkPlatformMap)
  {
    for (int j = 0; j < 3; ++j)
    {
      CMapData *pLinkMainbaseMap = GetStartMap(j);
      if (!pLinkMainbaseMap)
      {
        MyMessageBox("CMapOperation::Init() Error", "if(!pMainBaseMap)");
        return false;
      }
      if (!CTransportShip::InitShip(&g_TransportShip[j], pLinkShipMap, pLinkMainbaseMap, pLinkPlatformMap, j))
      {
        return false;
      }
    }

    for (int k = 0; k < 3; ++k)
    {
      for (int m = 0; m < 2; ++m)
      {
        m_SettlementMapData[k][m] = GetMap(ms_szSettlementMapName[k][m]);
        if (!m_SettlementMapData[k][m])
        {
          MyMessageBox(
            "Error",
            "CMapOperation::Init() : m_SettlementMapData[%d][%d] = GetMap( ms_szSettlementMapName[%d][%d](%s) ) Fail!",
            k,
            m,
            k,
            m,
            ms_szSettlementMapName[k][m]);
          return false;
        }
      }
    }

    return CHolyStoneSystem::InitHolySystem(&g_HolySys) && CWorldSchedule::Init(&g_WorldSch);
  }

  MyMessageBox("CMapOperation::Init() Error", "if(!pShipMap || !pPlatformMap)");
  return false;
}

CMapData *CMapOperation::GetStartMap(unsigned __int8 byRaceCode)
{
  int nRecordNum = m_tblMapData.GetRecordNum();
  for (int i = 0; i < nRecordNum; ++i)
  {
    _map_fld *pRecord = m_tblMapData.GetRecord(i);
    if (pRecord->m_nRaceVillageCode == byRaceCode)
    {
      return &m_Map[i];
    }
  }
  return nullptr;
}

CMapData *CMapOperation::GetPosStartMap(unsigned __int8 byRaceCode, bool bRand, float *pfoutPos)
{
  CMapData *startMap = GetStartMap(byRaceCode);
  if (!startMap)
  {
    return nullptr;
  }

  int startIndex = 0;
  if (bRand)
  {
    startIndex = rand() % startMap->m_nStartDumNum;
  }

  if (startMap->GetRandPosInDummy(startMap->m_pStartDummy[startIndex].m_pDumPos, pfoutPos, true))
  {
    return startMap;
  }

  return nullptr;
}

bool CMapOperation::LoadMaps()
{
  char *ppszMapNameList[100];
  char pszErrMsg[144];

  for (int mapIndex = 0; mapIndex < this->m_nMapNum; ++mapIndex)
  {
    _map_fld *pMapSet = this->m_tblMapData.GetRecord(mapIndex);
    this->m_Map[mapIndex].Init(pMapSet);
    ppszMapNameList[mapIndex] = pMapSet->m_strCode;

    NetTrace("Map Loading : %s", pMapSet->m_strCode);

    if (!this->m_Map[mapIndex].OpenMap(pMapSet->m_strFileName, pMapSet, 1))
    {
      MyMessageBox("Map Load Error", "%s - Read Error", pMapSet->m_strFileName);
      return false;
    }

    CItemStoreManager *itemStoreManager = CItemStoreManager::Instance();
    CMapItemStoreList *mapItemStoreList = itemStoreManager->GetMapItemStoreListByNum(mapIndex);
    mapItemStoreList->SetTypeNSerial(0, this->m_Map[mapIndex].m_nMapIndex);

    if (!mapItemStoreList->CreateStores(&this->m_Map[mapIndex]))
    {
      MyMessageBox("ItemStore Load Error", "LoadMaps() : pMapItemStoreList->CreateStores(%s)", pMapSet->m_strFileName);
      return false;
    }

    NetTrace("ItemStore Loaded : %s", pMapSet->m_strCode);

    if (!pMapSet->m_nMapType)
    {
      if (this->m_Map[mapIndex].m_pItemStoreDummy)
      {
        for (int nStoreNum = 0; nStoreNum < this->m_Map[mapIndex].m_nItemStoreDumNum; ++nStoreNum)
        {
          CMerchant *emptyNpc = FindEmptyNPC(g_NPC, MAX_NPC);
          if (!emptyNpc)
          {
            MyMessageBox("error", "CMerchant find error");
            break;
          }

          _npc_create_setdata pData;
          this->m_Map[mapIndex].GetRandPosInDummy(
              this->m_Map[mapIndex].m_pItemStoreDummy[nStoreNum].m_pDumPos,
              pData.m_fStartPos,
              1);

          itemStoreManager = CItemStoreManager::Instance();
          CItemStore *mapItemStore = itemStoreManager->GetMapItemStoreFromList(mapIndex, nStoreNum);
          pData.m_pLinkItemStore = mapItemStore;
          pData.m_pMap = &this->m_Map[mapIndex];
          pData.m_nLayerIndex = 0;
          pData.m_pRecordSet = mapItemStore->GetNpcRecord();
          pData.m_byRaceCode = mapItemStore->m_byNpcRaceCode;

          emptyNpc->Create(&pData);
        }
      }
      this->m_vecStandardMapCodeTable.push_back(
          std::make_pair(this->m_Map[mapIndex].m_nMapCode, this->m_nStdMapNum));
      ++this->m_nStdMapNum;
    }
  }

  this->CheckMapPortalLink();

  if (LoadRegionData(this->m_nMapNum, ppszMapNameList, pszErrMsg))
    return true;

  MyMessageBox("LoadMaps", "%s", pszErrMsg);
  return false;
}

bool LoadRegionData(int nMapNum, char **ppszMapNameList, char *pszErrMsg)
{
    for (int j = 0; j < 100; ++j) g_AreaIndexTable[j] = nullptr;

    FILE *Stream = nullptr;
    if (fopen_s(&Stream, ".\\Map\\MAPDATA.DAT", "rb") != 0 || !Stream)
    {
        sprintf_s(pszErrMsg, 144, "MAPDATA.DAT Open Error");
        return false;
    }

    unsigned int dwMapNumInFile = 0;
    fread(&dwMapNumInFile, 4, 1, Stream);

    for (unsigned int k = 0; k < dwMapNumInFile; ++k)
    {
        AreaList areaList;
        fread(&areaList.Height, 4, 1, Stream);
        fread(&areaList.Width, 4, 1, Stream);

        unsigned int dwStrLen = 0;
        fread(&dwStrLen, 4, 1, Stream);

        char szMapCode[288]{};
        fread(szMapCode, dwStrLen, 1, Stream);
        _strlwr_s(szMapCode, sizeof(szMapCode));

        unsigned int dwAreaCount = 0;
        fread(&dwAreaCount, 4, 1, Stream);

        for (unsigned int m = 0; m < dwAreaCount; ++m)
        {
            AreaData adata;
            fread(&adata.dwRGB, 4, 1, Stream);
            fread(&adata.dwX, 4, 1, Stream);
            fread(&adata.dwY, 4, 1, Stream);
            fread(&adata.dwSound, 4, 1, Stream);
            areaList.Push(&adata);
        }

        fread(&areaList.DataEnd, 4, 1, Stream);
        
        g_strLMapMap[szMapCode] = areaList;
        AreaList *pListInMap = &g_strLMapMap[szMapCode];

        if (areaList.DataEnd > 0)
        {
            pListInMap->m_Data.resize(areaList.DataEnd);
            fread(pListInMap->m_Data.data(), areaList.DataEnd, 1, Stream);
        }
        
        pListInMap->ExtractData();

        for (int n = 0; n < nMapNum; ++n)
        {
            char szTarget[144]{};
            strcpy_s(szTarget, sizeof(szTarget), ppszMapNameList[n]);
            _strlwr_s(szTarget, sizeof(szTarget));

            if (strcmp(szMapCode, szTarget) == 0)
            {
                g_AreaIndexTable[n] = pListInMap;
                break;
            }
        }
    }

    fclose(Stream);
    return true;
}

bool CMapOperation::LoadRegion()
{
  this->m_nRegionNum = 0;
  for (int k = 0; k < this->m_nStdMapNum; ++k)
  {
    CMapData *mapData = &this->m_Map[k];
    char Buffer[152];
    sprintf_s(Buffer, ".\\map\\%s\\%s.spt", mapData->m_pMapSet->m_strCode, mapData->m_pMapSet->m_strCode);

    CDummyPosTable pPosTable;
    if (!pPosTable.LoadDummyPosition(Buffer, "*rg_"))
    {
      MyMessageBox(
          "CMapOperation Error", "CMapOperation::LoadRegion(%s) == false", mapData->m_pMapSet->m_strCode);
      return false;
    }

    if (!mapData->ConvertLocalToWorldDummy(&pPosTable, 0))
    {
      return false;
    }

    int RecordNum = pPosTable.GetRecordNum();
    for (int i = 0; i < RecordNum; ++i)
    {
      char *Source = (char *)pPosTable.GetRecord(i);
      strcpy_s(
          this->m_RegionData[this->m_nRegionNum].szRegionData,
          sizeof(this->m_RegionData[this->m_nRegionNum].szRegionData),
          Source);
      this->m_RegionData[this->m_nRegionNum].pMap = mapData;
      this->m_RegionData[this->m_nRegionNum].wDummyLineIndex = *((unsigned short *)Source + 32);
      this->m_nRegionNum++;
    }
  }
  return true;
}

void CMapOperation::CheckMapPortalLink()
{
  for (int j = 0; j < this->m_nMapNum; ++j)
  {
    CMapData *mapData = &this->m_Map[j];
    for (int nPortalIndex = 0; nPortalIndex < mapData->m_nPortalNum; ++nPortalIndex)
    {
      _portal_dummy *Portal = mapData->GetPortal(nPortalIndex);
      if (Portal->m_pPortalRec && std::strcmp(Portal->m_pPortalRec->m_strLinkMapCode, "0") != 0)
      {
        CMapData *linkedMap = this->GetMap(Portal->m_pPortalRec->m_strLinkMapCode);
        if (linkedMap)
        {
          if (!linkedMap->GetPortal(Portal->m_pPortalRec->m_strLinkPortalCode))
          {
            g_Main.m_logLoadingError.Write(
                "Portal Link Check: %s.. %dth >> Map: %s, Portal: LinkPortalCode(%s)",
                mapData->m_pMapSet->m_strCode,
                nPortalIndex,
                Portal->m_pPortalRec->m_strLinkMapCode,
                Portal->m_pPortalRec->m_strLinkPortalCode);
          }
        }
        else
        {
          g_Main.m_logLoadingError.Write(
              "Portal Link Check: %s.. %dth >> Portal: LinkMapCode(%s)",
              mapData->m_pMapSet->m_strCode,
              nPortalIndex,
              Portal->m_pPortalRec->m_strLinkMapCode);
        }
      }
    }
  }
}

bool CMapOperation::IsExistStdMapID(int iMapID)
{
  for (size_t j = 0; j < m_vecStandardMapCodeTable.size(); ++j)
  {
    if (iMapID == m_vecStandardMapCodeTable[j].first)
    {
      return true;
    }
  }
  return false;
}

CMapData *CMapOperation::GetMap(const char *szMapCode)
{
  for (int j = 0; j < m_nMapNum; ++j)
  {
    if (!std::strcmp(m_Map[j].m_pMapSet->m_strCode, szMapCode))
    {
      return &m_Map[j];
    }
  }
  return nullptr;
}

CMapData *CMapOperation::GetMap(int nIndex)
{
  if (nIndex < m_nMapNum && nIndex >= 0)
  {
    return &m_Map[nIndex];
  }
  return nullptr;
}

int CMapOperation::GetMap(CMapData *pMap)
{
  for (int j = 0; j < m_nMapNum; ++j)
  {
    if (&m_Map[j] == pMap)
    {
      return j;
    }
  }
  return -1;
}
