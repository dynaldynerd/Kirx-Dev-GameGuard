#include "pch.h"

#include "CMapOperation.h"

#include <cstdio>
#include <cstring>

#include "CMapData.h"
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
  for (int dwIndex = 0; dwIndex < this->m_nMapNum; ++dwIndex)
  {
    _map_fld *pMapSet = m_tblMapData.GetRecord(dwIndex);
    // CMapData::Init and OpenMap usually handle the setup
    // m_Map[dwIndex].Init(pMapSet); 
    // if (!m_Map[dwIndex].OpenMap(pMapSet->m_strFileName, pMapSet, 1)) return false;

    // Standard Map mapping
    if (pMapSet->m_nMapType == 0)
    {
      m_vecStandardMapCodeTable.push_back(std::make_pair(m_Map[dwIndex].m_nMapCode, m_nStdMapNum));
      m_nStdMapNum++;
    }
  }
  CheckMapPortalLink();
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
      strcpy_s(this->m_RegionData[this->m_nRegionNum].szRegionData, Source);
      this->m_RegionData[this->m_nRegionNum].pMap = v7;
      this->m_RegionData[this->m_nRegionNum].wDummyLineIndex = *((unsigned short *)Source + 32);
      this->m_nRegionNum++;
    }
  }
  return true;
}

void CMapOperation::CheckMapPortalLink()
{
  // Logic to link portals between maps
}

CMapData *CMapOperation::GetMap(const char *szMapCode)
{
  int scratch[12]{};
  std::memset(scratch, 0xCC, sizeof(scratch));

  for (int j = 0; j < m_nMapNum; ++j)
  {
    if (!std::strcmp(m_Map[j].m_pMapSet->m_strCode, szMapCode))
    {
      return &m_Map[j];
    }
  }
  return nullptr;
}
