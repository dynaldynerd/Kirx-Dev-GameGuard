#pragma once

#include <windows.h>

#include "Character.h"
#include "ResourceData.h"

enum
{
  CDPT_FACE = 0,
  CDPT_UPPER_PART = 1,
  CDPT_LOWER_PART = 2,
  CDPT_GLOVES = 3,
  CDPT_SHOES = 4,
  MAX_DEFAULT_PART = 5
};

#define CMM_MOVE_WALK 0
#define CMM_MOVE_RUN 1

#define ID_DEFAULT_BONE_AC 4

class CRFClientPlayer
{
public:
  CRFClientPlayer();
  ~CRFClientPlayer();

  bool Initialize(IDirect3DDevice8 *pi_pD3DDevice);
  void Shutdown();

  bool LoadAccretia();
  void FrameMove();
  void Render();
  void CreateShadow();
  void SetMoveMode(bool pi_bMoving, BYTE pi_byMoveMode);
  void SetWalkMode(BYTE pi_byWalkMode);

  void SetPosition(float pi_fX, float pi_fY, float pi_fZ);
  void GetPosition(float po_pfPos[3]) const;
  void SetRotY(float pi_fRotY);
  void SetMapColor(DWORD pi_dwMapColor);
  BYTE GetWalkMode() const;
  float GetRotY() const;
  void GetCameraTarget(float po_pfTarget[3]) const;
  float GetCameraExtent() const;
  void GetScaleAddPos(float *po_pfScale, float *po_pfAddPos) const;
  bool IsLoaded() const;

private:
  bool MountRFS();
  bool LoadResourceData(BONE_DATA &po_stBoneData,
                        MESH_DATA po_astMeshData[MAX_DEFAULT_PART],
                        ANI_DATA &po_stAniData);
  bool LoadAnimation(char *pi_pAniName, ChAnimation **po_ppAnimation);
  bool SetAnimation(ChAnimation *pi_pAnimation);
  bool LoadTexturePath(const char *pi_pTexturePath);
  bool LoadPart(DWORD pi_dwPartType, const MESH_DATA &pi_stMeshData);
  static bool IsValidBBoxHeight(float pi_fHeight);
  static bool IsTextureFileName(const char *pi_pFileName);

private:
  ChInterface *m_pCharIF;
  AnimationManager *m_pAnimationMgr;
  TextureManager *m_pTextureMgr;
  CObjectManager *m_pBoneMgr;
  CObjectManager *m_pBodyPartMgr;
  CHARACTEROBJECT *m_pBone;
  CHARACTEROBJECT *m_pMesh[MAX_DEFAULT_PART];
  ChAnimation *m_pStandAni;
  ChAnimation *m_pWalkAni;
  ChAnimation *m_pRunAni;
  ChAnimation *m_pCurAni;
  bool m_bInitialized;
  bool m_bLoaded;
  bool m_bMountedRFS;
  bool m_bPreparedTextureCache;
  bool m_bUseBoneRender;
  BYTE m_byWalkMode;
  DWORD m_dwMapColor;
  float m_fBoneFrame;
  float m_vecPos[3];
  float m_fRotY;
  float m_vecBBoxMin[3];
  float m_vecBBoxMax[3];
  char m_szBoneName[MAX_PATH];
  char m_szBBoxName[MAX_PATH];
  char m_szAniName[MAX_PATH];
  char m_szMeshName[MAX_DEFAULT_PART][MAX_PATH];
};
