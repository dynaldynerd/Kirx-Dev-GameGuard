#pragma once

#include <windows.h>

#include "CCharacter.h"
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

class CPlayer : public CCharacter
{
public:
  CPlayer();
  virtual ~CPlayer();

  bool Initialize(void);
  void Shutdown();

  bool LoadAccretia();
  BOOL Animation(DWORD pi_dwAniFrame = static_cast<DWORD>(-1)) override;
  void FrameMove() override;
  BOOL Render() override;
  void CreateShadow() override;
  void SetMoveMode(bool pi_bMoving,
                   BYTE pi_byMoveMode,
                   bool pi_bUseMoveAniDirection = false,
                   float pi_fFacingRotY = 0.0f,
                   float pi_fMoveRotY = 0.0f);
  void SetWalkMode(BYTE pi_byWalkMode);
  BYTE GetWalkMode() const;
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
  static bool IsTextureFileName(const char *pi_pFileName);

private:
  CHARACTEROBJECT *m_pMesh[MAX_DEFAULT_PART];
  ChAnimation *m_pStandAni;
  ChAnimation *m_pWalkAni;
  ChAnimation *m_pRunAni;
  ChAnimation *m_pBwWalkAni;
  ChAnimation *m_pLfWalkAni;
  ChAnimation *m_pRtWalkAni;
  ChAnimation *m_pBwRunAni;
  ChAnimation *m_pLfRunAni;
  ChAnimation *m_pRtRunAni;
  ChAnimation *m_pCurAni;
  bool m_bInitialized;
  bool m_bLoaded;
  bool m_bMountedRFS;
  bool m_bPreparedTextureCache;
  bool m_bUseBoneRender;
  BYTE m_byWalkMode;
  char m_szBoneName[MAX_PATH];
  char m_szBBoxName[MAX_PATH];
  char m_szAniName[MAX_PATH];
  char m_szMeshName[MAX_DEFAULT_PART][MAX_PATH];
};
