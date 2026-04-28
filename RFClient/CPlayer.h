#pragma once

#include <windows.h>

#include "CCharacter.h"
#include "ResourceData.h"

struct _REGED_AVATOR_DB;

enum
{
  CDPT_FACE = 0,
  CDPT_UPPER_PART = 1,
  CDPT_LOWER_PART = 2,
  CDPT_GLOVES = 3,
  CDPT_SHOES = 4,
  CDPT_HELMET = 5,
  CEPT_WEAPON = 6,
  CEPT_SHIELD = 7,
  CEPT_CLOAK = 8,
  CEPT_HAIR = 9,
  MAX_DEFAULT_PART = 6,
  MAX_PLAYER_RENDER_PART = 10
};

struct PLAYER_PART_EFFECT
{
  BOOL bEnabled;
  BOOL bApplied;
  BYTE byItemType;
  DWORD dwMeshID;
  DWORD dwLevel;
};

#define CMM_MOVE_WALK 0
#define CMM_MOVE_RUN 1

#define CAI_MOVE_STOP 0x10000000

#define ID_DEFAULT_BONE_AC 4

class CPlayer : public CCharacter
{
public:
  CPlayer();
  virtual ~CPlayer();

  bool Initialize(void);
  void Shutdown();

  bool LoadAccretia();
  bool LoadRegedAvatar(const _REGED_AVATOR_DB &pi_stRegedAvatar);
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
  bool LoadPart(DWORD pi_dwPartType, BYTE pi_byResourceList, const MESH_DATA &pi_stMeshData);
  bool LoadBone(const BONE_DATA &pi_stBoneData);
  BOOL SetVertexBlending(DWORD pi_dwPartIndex, CHARACTEROBJECT *pi_pMesh, BOOL pi_bForAnimation);
  void ReleaseVertexBlendMatrices();
  void ClearPartEffects(void);
  void SetPartEffect(DWORD pi_dwPartType,
                     BYTE pi_byItemType,
                     DWORD pi_dwMeshID,
                     DWORD pi_dwLevel,
                     BOOL pi_bEnabled);
  static bool IsTextureFileName(const char *pi_pFileName);

private:
  CHARACTEROBJECT *m_pMesh[MAX_PLAYER_RENDER_PART];
  PLAYER_PART_EFFECT m_stPartEffect[MAX_PLAYER_RENDER_PART];
  D3DXMATRIX *m_pMatResMat[MAX_PLAYER_RENDER_PART][2];
  DWORD m_dwMaxResult[MAX_PLAYER_RENDER_PART];
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
  char m_szLastTextureLoadPath[MAX_PATH];
  char m_szMeshName[MAX_PLAYER_RENDER_PART][MAX_PATH];
};
