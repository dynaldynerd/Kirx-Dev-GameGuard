#pragma once

#include "IdaCompat.h"

#include <utility>
#include <vector>

#include "CCharacter.h"
#include "CMapDataTable.h"
#include "RegionData.h"

class CMapData;

/* 1462 */
class __cppobj CMapOperation
{
public:
  bool Init();
  CMapData *GetMap(const char *szMapCode);
  CMapData *GetStartMap(unsigned __int8 byRaceCode);
  bool LoadMaps();
  bool LoadRegion();
  void CheckMapPortalLink();

  static const char *ms_szSettlementMapName[3][2];

  int m_nLoopStartPoint;
  unsigned int m_dwSpeedHackStdTime;
  CMapDataTable m_tblMapData;
  int m_nMapNum;
  int m_nStdMapNum;
  std::vector<std::pair<int,int>> m_vecStandardMapCodeTable;
  CMapData *m_Map;
  int m_nRegionNum;
  _region_data m_RegionData[100];
  bool m_bReSpawnMonster;
  CMyTimer m_tmrObjTerm;
  CMyTimer m_tmrSystem;
  CMyTimer m_tmrRecover;
  unsigned int m_dwLastObjLoopTime;
  CMapData *m_SettlementMapData[3][2];
  virtual ~CMapOperation() = default;
};

extern CMapOperation g_MapOper;

