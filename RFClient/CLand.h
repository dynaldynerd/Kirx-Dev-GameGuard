#pragma once

#include <windows.h>

#include "R3Engine/2ndclass/level.h"

#define MAX_MAP (100)
#define MAX_PORTAL (50)
#define MAX_MINE (50)
#define MAX_AREA (50)
#define MAX_LINKED_PORTAL MAX_PORTAL

#define PTI_ROAD 0
#define PTI_WAYPOINT 1

#pragma pack(push, 1)
struct _portal_link
{
  int mapindex;
  int linkportalindex;
};

struct _spt_coor
{
  float coor[3];
  float anglez1;
};

struct _portal_data
{
  DWORD mapindex;
  DWORD index;
  float MinRadius[3];
  float MaxRadius[3];
  _spt_coor SPTCoor[3];
  float CenterCoor[3];
  float anglecenter;
  DWORD byPortalType;
  BOOL bIsMovable;
  BYTE byUsableRace[5];
  char pName[64];
  char pDesc[64];
  BYTE byLinkedPortalNum;
  BYTE byUnknown[3];
  BYTE byUnknownEx[47];
};

struct _ore_data
{
  DWORD mapindex;
  DWORD index;
  float radmin[3];
  float radmax[3];
  float unknown[16];
};

struct _area_info
{
  DWORD index;
  LONG X1;
  LONG Y1;
  LONG X2;
  LONG Y2;
  char AreaName[64];
};

struct DoubleRepeat
{
  float zoommodule;
  int x1;
  int y1;
  int x2;
  int y2;
};

struct _TheRest
{
  DoubleRepeat DoubleRep[2];
  int MapsizeX;
  int Thesizeofthecardsin;
  int DisplacementmapsX;
  int displacementmapsin;
  int LeafNum;
  int LeafNum1;
  int SizeX;
  int SizeY;
  int SizeZ;
};

struct _base
{
  DWORD index;
  char map[32];
  char BSP[128];
  char minimapSPR[128];
  WORD wMinimapAngle;
  DWORD portalNum;
};
#pragma pack(pop)

static_assert(sizeof(_portal_link) == 8, "_portal_link size mismatch");
static_assert(sizeof(_spt_coor) == 16, "_spt_coor size mismatch");
static_assert(sizeof(_portal_data) == 288, "_portal_data size mismatch");
static_assert(sizeof(_ore_data) == 96, "_ore_data size mismatch");
static_assert(sizeof(_area_info) == 84, "_area_info size mismatch");
static_assert(sizeof(DoubleRepeat) == 20, "DoubleRepeat size mismatch");
static_assert(sizeof(_TheRest) == 76, "_TheRest size mismatch");
static_assert(sizeof(_base) == 298, "_base size mismatch");

struct MINE_INFO
{
  DWORD byMapIndex;
  DWORD byIndex;

  float vecMin[3];
  float vecMax[3];
  float fUnknown[16];
};

struct PORTAL_INFO
{
  DWORD byMapIndex;
  DWORD byIndex;
  DWORD byPortalType;

  BOOL bIsMovable;
  BYTE byUsableRace[5];

  char pName[64];
  char pDesc[64];
  Vector3f vecMin;
  Vector3f vecMax;
  _spt_coor SPTCoor[3];
  Vector3f vecCenter;
  float fAngleCenter;

  BYTE byLinkedPortalNum;
  _portal_link *pLinkedPortal;

  BOOL bIsUsable;
};

struct MAP_AREA_INFO
{
  DWORD dwIndex;
  RECT recArea;
  char pName[64];
};

struct MAP_INFO
{
  DWORD byIndex;
  char pName[32];
  char pName_UI[64];
  char pFileName[128];
  char pMinimapFileName[128];
  WORD wMinimapAngle;

  DWORD byPortalNum;
  PORTAL_INFO *pPortalInfo;

  DWORD byMineNum;
  MINE_INFO *pMineInfo;

  DWORD byAreaNum;
  MAP_AREA_INFO *pAreaInfo;

  _TheRest TheRest;
};

class CLand;

extern CLand *g_pLand;
extern CLevel *g_pLevel;

#define _GetLand() g_pLand
#define _GetLevel() g_pLevel

class CLand
{
private:
  CLevel *m_pLevel;
  MAP_INFO *m_pMapInfo;
  DWORD m_byMaxMapNum;
  BYTE m_byMapIndex;
  BYTE m_byPortalIndex;

public:
  CLand();
  ~CLand();

  void Init(void);
  BOOL Create(void);
  BOOL Destroy(void);

  BOOL LoadLevel(char *pi_pLevelName);
  BOOL LoadLevel(BYTE pi_byMapIndex = 0xFF);
  BOOL UnloadLevel(void);
  BOOL Render(Vector3f &pi_vecPos);
  BOOL RenderAlpha(Vector3f &pi_vecPos);

  void OpenMinimapWindow(char *pi_pMapName, char *pi_pItemName);
  void UI_OpenMoveMapWindow(void);
  void UI_SetRadar(void);

  void RenderInfo(void);
  BOOL LoadData(void);

private:
  void UI_SetMinimap(BYTE pi_byMapIndex = 0xFF);
  void ReleaseData(void);
  void ReleaseMapInfo(MAP_INFO *pi_pMapInfo);

public:
  inline CLevel *GetLevel(void) { return m_pLevel; }

  inline void SetMaxMapNum(DWORD pi_byMaxNum) { m_byMaxMapNum = pi_byMaxNum; }
  inline DWORD GetMaxMapNum(void) { return m_byMaxMapNum; }
  inline void SetMapIndex(BYTE pi_byMapIndex) { m_byMapIndex = pi_byMapIndex; }
  inline BYTE GetMapIndex(void) { return m_byMapIndex; }
  inline void SetPortalIndex(BYTE pi_byPortalIndex) { m_byPortalIndex = pi_byPortalIndex; }
  inline BYTE GetPortalIndex(void) { return m_byPortalIndex; }

  inline DWORD GetMaxPortalNum(BYTE pi_byMapIndex)
  {
    MAP_INFO *l_pMapInfo = GetMapInfo(pi_byMapIndex);
    return l_pMapInfo ? l_pMapInfo->byPortalNum : 0;
  }

  inline MAP_INFO *GetCurMapInfo(void)
  {
    return m_pMapInfo && m_byMapIndex != 0xFF && m_byMapIndex < m_byMaxMapNum ? &m_pMapInfo[m_byMapIndex] : NULL;
  }

  inline const MAP_INFO *GetCurMapInfo(void) const
  {
    return m_pMapInfo && m_byMapIndex != 0xFF && m_byMapIndex < m_byMaxMapNum ? &m_pMapInfo[m_byMapIndex] : NULL;
  }

  inline MAP_INFO *GetMapInfo(BYTE pi_byIndex)
  {
    return m_pMapInfo && pi_byIndex < m_byMaxMapNum ? &m_pMapInfo[pi_byIndex] : NULL;
  }

  inline const MAP_INFO *GetMapInfo(BYTE pi_byIndex) const
  {
    return m_pMapInfo && pi_byIndex < m_byMaxMapNum ? &m_pMapInfo[pi_byIndex] : NULL;
  }

  inline PORTAL_INFO *GetCurPortalInfo(void)
  {
    return GetPortalInfo(m_byMapIndex, m_byPortalIndex);
  }

  inline PORTAL_INFO *GetPortalInfo(BYTE pi_byMapIndex, BYTE pi_byPortalIndex)
  {
    MAP_INFO *l_pMapInfo = GetMapInfo(pi_byMapIndex);
    return l_pMapInfo && pi_byPortalIndex < l_pMapInfo->byPortalNum ? &l_pMapInfo->pPortalInfo[pi_byPortalIndex] : NULL;
  }

  inline DWORD GetMaxMineNum(BYTE pi_byMapIndex)
  {
    MAP_INFO *l_pMapInfo = GetMapInfo(pi_byMapIndex);
    return l_pMapInfo ? l_pMapInfo->byMineNum : 0;
  }

  inline MINE_INFO *GetMineInfo(BYTE pi_byMapIndex, BYTE pi_byMineIndex)
  {
    MAP_INFO *l_pMapInfo = GetMapInfo(pi_byMapIndex);
    return l_pMapInfo && pi_byMineIndex < l_pMapInfo->byMineNum ? &l_pMapInfo->pMineInfo[pi_byMineIndex] : NULL;
  }
};
