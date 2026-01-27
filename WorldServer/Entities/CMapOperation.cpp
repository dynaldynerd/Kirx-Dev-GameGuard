#include "pch.h"

#include "CMapOperation.h"

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

bool CMapOperation::Init()
{
  if (!m_tblMapData.ReadScript(".\\Map\\Map_Data.spt"))
  {
    return false;
  }

  m_nMapNum = m_tblMapData.GetRecordNum();
  m_Map = new CMapData[m_nMapNum];

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
          return false;
        }
      }
    }

    return CHolyStoneSystem::InitHolySystem(&g_HolySys) && CWorldSchedule::Init(&g_WorldSch);
  }

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

bool CMapOperation::LoadMaps()
{
    char *ppszMapNameList[100];
    char pszErrMsg[144];

    for (int dwIndex = 0; dwIndex < this->m_nMapNum; ++dwIndex)
    {
        _map_fld *pMapSet = this->m_tblMapData.GetRecord(dwIndex);
        this->m_Map[dwIndex].Init(pMapSet);
        ppszMapNameList[dwIndex] = pMapSet->m_strCode;

        NetTrace("Map Loading : %s", pMapSet->m_strCode);

        if (!this->m_Map[dwIndex].OpenMap(pMapSet->m_strFileName, pMapSet, 1))
        {
            MyMessageBox("Map Load Error", "%s - Read Error", pMapSet->m_strFileName);
            return false;
        }

        CItemStoreManager *v4 = CItemStoreManager::Instance();
        CMapItemStoreList *MapItemStoreListByNum = v4->GetMapItemStoreListByNum(dwIndex);
        MapItemStoreListByNum->SetTypeNSerial(0, this->m_Map[dwIndex].m_nMapIndex);

        if (!MapItemStoreListByNum->CreateStores(&this->m_Map[dwIndex]))
        {
            return false;
        }

        NetTrace("ItemStore Loaded : %s", pMapSet->m_strCode);

        if (!pMapSet->m_nMapType)
        {
            if (this->m_Map[dwIndex].m_pItemStoreDummy)
            {
                for (int nStoreNum = 0; nStoreNum < this->m_Map[dwIndex].m_nItemStoreDumNum; ++nStoreNum)
                {
                    CMerchant *EmptyNPC = FindEmptyNPC(g_NPC, MAX_NPC);
                    if (!EmptyNPC)
                    {
                         MyMessageBox("error", "CMerchant find error");
                         break;
                    }

                    _npc_create_setdata pData;
                    this->m_Map[dwIndex].GetRandPosInDummy(
                        this->m_Map[dwIndex].m_pItemStoreDummy[nStoreNum].m_pDumPos,
                        pData.m_fStartPos,
                        1);

                    CItemStore *MapItemStoreFromList = v4->GetMapItemStoreFromList(dwIndex, nStoreNum);
                    pData.m_pLinkItemStore = MapItemStoreFromList;
                    pData.m_pMap = &this->m_Map[dwIndex];
                    pData.m_nLayerIndex = 0;
                    if (MapItemStoreFromList)
                    {
                        pData.m_pRecordSet = (_base_fld*)MapItemStoreFromList->m_pRec;
                        pData.m_byRaceCode = MapItemStoreFromList->m_byNpcRaceCode;
                    }

                    EmptyNPC->Create(&pData);
                }
            }
            this->m_vecStandardMapCodeTable.push_back(std::make_pair(this->m_Map[dwIndex].m_nMapCode, this->m_nStdMapNum));
            ++this->m_nStdMapNum;
        }
    }

    this->CheckMapPortalLink();

    if (LoadRegionData(this->m_nMapNum, ppszMapNameList, pszErrMsg))
        return true;

    MyMessageBox("LoadMaps", "%s", pszErrMsg);
    return false;
}

bool CMapOperation::LoadRegionData(int nMapNum, char **ppszMapNameList, char *pszErrMsg)
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
        AreaList v17;
        fread(&v17.Height, 4, 1, Stream);
        fread(&v17.Width, 4, 1, Stream);

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
            v17.Push(&adata);
        }

        fread(&v17.DataEnd, 4, 1, Stream);
        
        g_strLMapMap[szMapCode] = v17;
        AreaList *pListInMap = &g_strLMapMap[szMapCode];

        if (v17.DataEnd > 0)
        {
            pListInMap->m_Data.resize(v17.DataEnd);
            fread(pListInMap->m_Data.data(), v17.DataEnd, 1, Stream);
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
    CMapData *v7 = &this->m_Map[k];
    char Buffer[152];
    sprintf_s(Buffer, ".\\map\\%s\\%s.spt", v7->m_pMapSet->m_strCode, v7->m_pMapSet->m_strCode);
    
    CDummyPosTable pPosTable;
    if (!pPosTable.LoadDummyPosition(Buffer, "*rg_"))
    {
      return false;
    }
    
    if (!v7->ConvertLocalToWorldDummy(&pPosTable, 0))
    {
      return false;
    }

    int RecordNum = pPosTable.GetRecordNum();
    for (int i = 0; i < RecordNum; ++i)
    {
      char *Source = (char *)pPosTable.GetRecord(i);
      strcpy_s(this->m_RegionData[this->m_nRegionNum].szRegionData, sizeof(this->m_RegionData[this->m_nRegionNum].szRegionData), Source);
      this->m_RegionData[this->m_nRegionNum].pMap = v7;
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
        CMapData *v5 = &this->m_Map[j];
        for (int nPortalIndex = 0; nPortalIndex < v5->m_nPortalNum; ++nPortalIndex)
        {
            _portal_dummy *Portal = &v5->m_pPortal[nPortalIndex];
            if (Portal->m_pPortalRec && strcmp(Portal->m_pPortalRec->m_strLinkMapCode, "0") != 0)
            {
                CMapData *Map = this->GetMap(Portal->m_pPortalRec->m_strLinkMapCode);
                if (Map)
                {
                    bool bFound = false;
                    for (int k = 0; k < Map->m_nPortalNum; ++k)
                    {
                        if (strcmp(Map->m_pPortal[k].m_pPortalRec->m_strCode, Portal->m_pPortalRec->m_strLinkPortalCode) == 0)
                        {
                            bFound = true;
                            break;
                        }
                    }
                    if (!bFound)
                    {
                        NetTrace("Portal Link Check: %s.. %dth >> Map: %s, Portal: LinkPortalCode(%s)",
                            v5->m_pMapSet->m_strCode,
                            nPortalIndex,
                            Portal->m_pPortalRec->m_strLinkMapCode,
                            Portal->m_pPortalRec->m_strLinkPortalCode);
                    }
                }
                else
                {
                    NetTrace("Portal Link Check: %s.. %dth >> Portal: LinkMapCode(%s)",
                        v5->m_pMapSet->m_strCode,
                        nPortalIndex,
                        Portal->m_pPortalRec->m_strLinkMapCode);
                }
            }
        }
    }
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
