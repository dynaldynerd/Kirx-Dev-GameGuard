#pragma once

#include "IdaCompat.h"

#include <utility>
#include <vector>

#include "CCharacter.h"
#include "CMapDataTable.h"
#include "RegionData.h"

class CMapData;
class CGameObject;

/* 1462 */
class  CMapOperation
{
public:
  CMapOperation();
  bool Init();
  void OnLoop();
  void RespawnMonster();
  CMapData *GetMap(const char *szMapCode);
  CMapData *GetMap(int nIndex);
  int GetMap(CMapData *pMap);
  CMapData *GetSettlementMapData(int iRace, int iTh);
  CMapData *GetStartMap(unsigned __int8 byRaceCode);
  CMapData *GetPosStartMap(unsigned __int8 byRaceCode, bool bRand, float *pfoutPos);
  bool LoadMaps();
  bool LoadRegion();
  bool IsExistStdMapID(int iMapID);
  char IsInRegion(char *pszRegionCode, CGameObject *pObj);
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
  virtual ~CMapOperation();
};

#include <map>
#include "AreaList.h"

extern CMapOperation g_MapOper;
extern AreaList *g_AreaIndexTable[100];
extern std::map<std::string, AreaList> g_strLMapMap;

int GetRegionIndex(int nMapIndex, unsigned int x, unsigned int y, unsigned int dwMaxX, unsigned int dwMaxY);
