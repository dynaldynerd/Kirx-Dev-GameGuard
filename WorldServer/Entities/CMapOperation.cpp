#include "pch.h"

#include "CMapOperation.h"

#include <new>
#include <cstdio>
#include <cstring>

#include "CMapData.h"
#include "CGameObject.h"
#include "CItemStore.h"
#include "CItemStoreManager.h"
#include "CMonster.h"
#include "CMonsterEventRespawn.h"
#include "CMonsterEventSet.h"
#include "CPlayer.h"
#include "CBossMonsterScheduleSystem.h"
#include "CTransportShip.h"
#include "CHolyStoneSystem.h"
#include "CWorldSchedule.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include <atltime.h>
#include <mmsystem.h>

const char *CMapOperation::ms_szSettlementMapName[3][2] = {
    {"NeutralBS1", "NeutralBS2"},
    {"NeutralCS1", "NeutralCS2"},
    {"NeutralAS1", "NeutralAS2"}
};

CMapOperation g_MapOper;

AreaList *g_AreaIndexTable[100];
std::map<std::string, AreaList> g_strLMapMap;

bool LoadRegionData(int nMapNum, char **ppszMapNameList, char *pszErrMsg);

int GetRegionIndex(int nMapIndex, unsigned int x, unsigned int y, unsigned int dwMaxX, unsigned int dwMaxY)
{
  AreaList *areaList = g_AreaIndexTable[nMapIndex];
  if (!areaList)
  {
    return -1;
  }

  const unsigned int regionX = areaList->Width * x / dwMaxX;
  const unsigned int regionY = areaList->Height * y / dwMaxY;
  if (regionX > areaList->Width)
  {
    return 0;
  }
  if (regionY > areaList->Height)
  {
    return 0;
  }

  const unsigned int regionIndex = areaList->pRealData[areaList->Width * regionY + regionX];
  if (regionIndex >= areaList->Count)
  {
    return -1;
  }
  return static_cast<int>(regionIndex);
}

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
      if (!g_TransportShip[j].InitShip(pLinkShipMap, pLinkMainbaseMap, pLinkPlatformMap, j))
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

    return g_HolySys.InitHolySystem() && g_WorldSch.Init();
  }

  MyMessageBox("CMapOperation::Init() Error", "if(!pShipMap || !pPlatformMap)");
  return false;
}

void CMapOperation::OnLoop()
{
  if (m_tmrRecover.CountingTimer())
  {
    for (int playerIndex = 0; playerIndex < MAX_PLAYER; ++playerIndex)
    {
      CPlayer *player = &g_Player[playerIndex];
      if (!player->m_bLive)
      {
        continue;
      }

      if (!player->m_bCorpse)
      {
        player->AutoRecover();
        if (player->m_Param.GetRaceCode() == 1)
        {
          player->AutoRecover_Animus();
        }
      }

      if (player->m_Param.GetRaceCode() == 0)
      {
        player->AutoCharge_Booster();
      }
    }
  }

  if (m_tmrSystem.CountingTimer())
  {
    if (m_bReSpawnMonster)
    {
      RespawnMonster();
    }

    for (int mapIndex = 0; mapIndex < m_nMapNum; ++mapIndex)
    {
      if (m_Map[mapIndex].m_bUse)
      {
        m_Map[mapIndex].OnLoop();
      }
    }

    g_MonsterEventRespawn.CheckRespawnEvent();
    g_MonsterEventSet.CheckEventSetRespawn();
  }

  if (m_tmrObjTerm.CountingTimer())
  {
    if (!m_nLoopStartPoint)
    {
      R3CalculateTime();
    }

    for (int objectIndex = m_nLoopStartPoint; objectIndex < CGameObject::s_nTotalObjectNum; objectIndex += 10)
    {
      if (CGameObject::s_pTotalObject[objectIndex]->m_bLive)
      {
        CGameObject::s_pTotalObject[objectIndex]->OnLoop();
      }
    }

    ++m_nLoopStartPoint;
    m_nLoopStartPoint %= 10;
  }
}

void CMapOperation::RespawnMonster()
{
  CBossMonsterScheduleSystem::Instance()->RespawnMonster();
  if (CMonster::s_nLiveNum >= MAX_MONSTER)
  {
    return;
  }

  const DWORD now = timeGetTime();
  for (int mapIndex = 0; mapIndex < m_nMapNum; ++mapIndex)
  {
    CMapData *map = &m_Map[mapIndex];
    if (!map->m_bUse || map->m_pMapSet->m_nMapType || !map->m_ls->IsActiveLayer())
    {
      continue;
    }

    _MULTI_BLOCK *multiBlock = map->m_ls->m_pMB;
    for (int blockIndex = 0; blockIndex < map->m_nMonBlockNum; ++blockIndex)
    {
      _mon_block *block = &map->m_pMonBlock[blockIndex];
      const int activeCount = multiBlock->m_ptbMonBlock[blockIndex].GetRecordNum();
      for (int activeIndex = 0; activeIndex < activeCount; ++activeIndex)
      {
        _mon_active *active = &map->m_ls->m_MonAct[blockIndex][activeIndex];
        if (active->m_nLimRegenNum <= 0)
        {
          continue;
        }

        _mon_active_fld *activeRec = active->m_pActRec;
        BossSchedule *bossSchedule = active->GetBossSchedule();
        if (bossSchedule)
        {
          const ATL::CTime emptyTime(0);
          if (bossSchedule->m_LastRespawnSystemTime != emptyTime)
          {
            const ATL::CTime currentTime = ATL::CTime::GetCurrentTime();
            const ATL::CTimeSpan diff = currentTime - bossSchedule->m_LastRespawnSystemTime;
            const ATL::CTimeSpan regenSpan(static_cast<__time64_t>(activeRec->m_dwRegenTime / 1000));
            if (diff < regenSpan)
            {
              continue;
            }
          }

          ATL::CTime currentTime = ATL::CTime::GetCurrentTime();
          bossSchedule->Save_LastRespawnSystemTime(&currentTime);
        }
        else
        {
          if (active->m_dwLastRespawnTime && now - active->m_dwLastRespawnTime <= activeRec->m_dwRegenTime)
          {
            continue;
          }
          active->m_dwLastRespawnTime = now;
        }

        if (active->m_wMonRecIndex == 0xFFFF || activeRec->m_dwRegenProp <= static_cast<unsigned int>(rand() % 100))
        {
          continue;
        }

        const int missingCount = active->m_nLimRegenNum - active->m_zCurMonNum;
        if (missingCount <= 0)
        {
          continue;
        }

        const float dummyPerSpawn = static_cast<float>(active->m_pBlk->m_pBlkRec->m_dwDummyNum)
          / static_cast<float>(missingCount);
        const int fixedDummySpan = missingCount / static_cast<int>(active->m_pBlk->m_pBlkRec->m_dwDummyNum);
        int lastDummyIndex = 0;
        const bool noAliveInBlock = (active->m_zCurMonNum == 0);

        for (int spawnIndex = 0; spawnIndex < missingCount && active->m_zCurMonNum < active->m_nLimRegenNum; ++spawnIndex)
        {
          int dummyIndex = -1;
          if (noAliveInBlock)
          {
            if (dummyPerSpawn <= 1.0f)
            {
              if (dummyPerSpawn >= 1.0f)
              {
                dummyIndex = spawnIndex;
              }
              else
              {
                const int fixedSpanEnd = static_cast<int>(active->m_pBlk->m_pBlkRec->m_dwDummyNum) * fixedDummySpan;
                if (spawnIndex <= fixedSpanEnd)
                {
                  dummyIndex = spawnIndex / fixedDummySpan;
                }
                else
                {
                  dummyIndex = block->SelectDummyIndex();
                }
              }
            }
            else
            {
              if (spawnIndex)
              {
                dummyIndex = static_cast<int>(static_cast<float>(lastDummyIndex) + dummyPerSpawn);
              }
              else
              {
                dummyIndex = rand() % static_cast<int>(dummyPerSpawn);
              }
              lastDummyIndex = dummyIndex;
            }
          }
          else
          {
            unsigned int fallbackIndex = 0;
            int minActiveMon = block->m_pDumPos[0]->m_wActiveMon;
            for (unsigned int dummy = 0; dummy < active->m_pBlk->m_pBlkRec->m_dwDummyNum; ++dummy)
            {
              if (block->m_pDumPos[dummy]->m_bPosAble && !block->m_pDumPos[dummy]->m_wActiveMon)
              {
                dummyIndex = static_cast<int>(dummy);
                break;
              }

              if (minActiveMon > block->m_pDumPos[dummy]->m_wActiveMon)
              {
                minActiveMon = block->m_pDumPos[dummy]->m_wActiveMon;
                fallbackIndex = dummy;
              }
            }

            if (dummyIndex == -1)
            {
              dummyIndex = static_cast<int>(fallbackIndex);
            }
          }

          if (dummyIndex >= 0
              && static_cast<unsigned int>(dummyIndex) < active->m_pBlk->m_pBlkRec->m_dwDummyNum
              && block->m_pDumPos[dummyIndex]->m_bPosAble
              && !CreateRespawnMonster(
                map,
                0,
                active->m_wMonRecIndex,
                active,
                block->m_pDumPos[dummyIndex],
                true,
                true,
                false,
                false,
                true))
          {
            return;
          }
        }
      }
    }
  }
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
              pListInMap->pData = new char[areaList.DataEnd];
          }
          fread(pListInMap->pData, pListInMap->DataEnd, 1, Stream);
          
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

char CMapOperation::IsInRegion(char *pszRegionCode, CGameObject *pObj)
{
  for (int j = 0; j < m_nRegionNum; ++j)
  {
    _region_data *region = &m_RegionData[j];
    if (!strcmp_0(region->szRegionData, pszRegionCode) && region->pMap == pObj->m_pCurMap)
    {
      CExtDummy *dummy = &region->pMap->m_Dummy;
      if (dummy->IsInBBox(region->wDummyLineIndex, pObj->m_fCurPos))
      {
        return 1;
      }
    }
  }

  return 0;
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
