#pragma once

#include <windows.h>

#include "DataFile.h"
#include "ResourceData.h"

enum
{
  MAX_RES_DATA_LIST = 9,
  RLI_PLAYER = 0,
  RLI_MONSTER = 1,
  RLI_ANIMUS = 2,
  RLI_SKILL = 3,
  RLI_NPC = 4,
  RLI_ITEM = 5,
  RLI_FORCE = 6,
  RLI_BULLET = 7,
  RLI_UNIT = 8
};

class CCharResData
{
public:
  CCharResData();
  ~CCharResData();

  void Init(void);
  void Destroy(void);

  BOOL LoadBoneDataList(BYTE pi_byResFileType, CDataString *pi_pDataString);
  BOOL LoadMeshDataList(BYTE pi_byResFileType, CDataString *pi_pDataString);
  BOOL LoadAniDataList(BYTE pi_byResFileType, CDataString *pi_pDataString);

  BONE_DATA *GetBoneData(DWORD pi_dwIndex);
  MESH_DATA *GetMeshData(DWORD pi_dwIndex);
  ANI_DATA *GetAniData(DWORD pi_dwIndex);

  BONE_DATA *GetBoneDataByOrder(DWORD pi_dwIndex);
  MESH_DATA *GetMeshDataByOrder(DWORD pi_dwIndex);
  ANI_DATA *GetAniDataByOrder(DWORD pi_dwIndex);

  DWORD GetTotalBoneNum(void) const;
  DWORD GetTotalMeshNum(void) const;
  DWORD GetTotalAniNum(void) const;

private:
  DWORD m_dwBoneNum;
  DWORD m_dwMeshNum;
  DWORD m_dwAniNum;

  BONE_DATA *m_listBone;
  MESH_DATA *m_listMesh;
  ANI_DATA *m_listAni;
};

class CCharResDataMgr
{
public:
  CCharResDataMgr();
  ~CCharResDataMgr();

  BOOL LoadData(void);
  void UnloadData(void);
  BOOL IsLoaded(void) const;

  CCharResData *GetResourceList(BYTE pi_byType);
  const CCharResData *GetResourceList(BYTE pi_byType) const;

private:
  CCharResData m_listResData[MAX_RES_DATA_LIST];
  BOOL m_bLoaded;
};

extern CCharResDataMgr *g_pCharResDataMgr;

#define _GetCharResDataMgr() g_pCharResDataMgr
