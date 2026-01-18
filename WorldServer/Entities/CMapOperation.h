#pragma once

#include "IdaCompat.h"

/* 1462 */
struct __cppobj CMapOperation
{
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
