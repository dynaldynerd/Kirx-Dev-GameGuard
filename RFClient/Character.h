#pragma once

#include <windows.h>

#include <d3d8.h>
#include <d3dx8math.h>

void ReadRFSHeader(char *pi_pPath);
void LoadFile(char *pi_pFileName);
char *GetFileImme(char *pi_pFileName);
char *GetFileFromRFS(char *pi_pFileName);

struct ObjectMesh;

class CHARACTEROBJECT
{
public:
  DWORD m_ID;
  char m_szFileName[0x100];
  DWORD m_Load;
  DWORD m_dwReserved108;
  CHARACTEROBJECT *m_pParentBone;
  BYTE m_abyReserved110[0x100];
  DWORD m_MaxObj;
  ObjectMesh *m_pMesh;
  BYTE m_abyReserved218[0x80];
  DWORD m_dwReserved298;
  DWORD m_Frame;
  DWORD m_OMaxFrame;
  BOOL m_Transition;
  DWORD m_dwReserved2A8;
  DWORD m_TransFrame;
  DWORD m_TransTime;
  BYTE m_abyReserved2B4[0x50];
  DWORD m_dwReserved304;

  int IsHaveEffect(void);
};

class ChAnimation
{
public:
  BYTE m_data[0x118];

  ChAnimation(void);
  ~ChAnimation(void);

  bool LoadRealAnimation(bool pi_bUseImmediateFile);
};

class AnimationManager
{
public:
  BYTE m_data[0x18];

  ~AnimationManager(void);

  int Init(void);
  unsigned long GetAnimationID(char *pi_pAniName);
  ChAnimation *GetAnimationDataByNoPath(char *pi_pAniName);
  ChAnimation *GetAnimationData(char *pi_pAniName);
  ChAnimation *GetAnimationData(unsigned long pi_dwAniID);
  bool AddAnimation(char *pi_pAniName);
  void DeleteAnimation(char *pi_pAniName);
  void DeleteAnimation2(unsigned long pi_dwAniID);

  static AnimationManager *m_AManager;
};

class TextureManager
{
public:
  IDirect3DDevice8 *m_pD3DDevice;
  BYTE m_data[0x9C];

  ~TextureManager(void);

  int Init(void);
  bool AddTexture(char const *pi_pPathName, char const *pi_pFileName);
  IDirect3DTexture8 *GetTexture(char *pi_pTextureName);
  IDirect3DTexture8 *GetTexture2(unsigned long pi_dwTextureID);
  unsigned long GetTextureID(char const *pi_pTextureName);
  void DeleteTexture(char *pi_pTextureName);
  void DeleteTexture2(unsigned long pi_dwTextureID);
  void Clear(void);

  static TextureManager *m_TManager;
};

class CObjectManager
{
public:
  BYTE m_data[0x94];

  ~CObjectManager(void);

  void Clear(void);
  void AddCharacter(CHARACTEROBJECT *pi_pCharacterObject);
  CHARACTEROBJECT *GetCharacter(char *pi_pName);
  CHARACTEROBJECT *GetCharacter(unsigned long pi_dwID);
  unsigned long GetID(char *pi_pName);
  void DeleteCharacter(char *pi_pName);
  void DeleteCharacter(unsigned long pi_dwID);
};

class CRender
{
public:
  BYTE m_data00[0x198];
  TextureManager m_TM;
  BYTE m_data01[0xD0];

  void Init(void);
  void SetState(void);
  void UnSetState(void);
  void SetLight(D3DLIGHT8 pi_stLight);
  bool LoadRealData(CHARACTEROBJECT *pi_pCharacterObject, TextureManager *pi_pTextureManager, bool pi_bUseImmediateFile);
  void FrameMove(CHARACTEROBJECT *pi_pCharacterObject);
  void FrameStep(CHARACTEROBJECT *pi_pCharacterObject, int pi_nFrame);
  void InitEffect(CHARACTEROBJECT *pi_pCharacterObject);
  void DrawCharacterMat(CHARACTEROBJECT *pi_pCharacterObject, D3DXMATRIX *pi_pWorldMatrix, float pi_fScroll);
  void DrawCharacter(CHARACTEROBJECT *pi_pCharacterObject,
                     float *const pi_pfPos,
                     float pi_fRotY,
                     float pi_fScale = 1.0f,
                     float pi_fScroll = 0.0f);
  void DrawCharacterShadow(CHARACTEROBJECT *pi_pCharacterObject,
                           float *const pi_pfPos,
                           float pi_fRotY,
                           float pi_fAlpha = 1.0f,
                           float pi_fScale = 1.0f);
  void MatchTextureToMesh(TextureManager *pi_pTextureManager, CHARACTEROBJECT *pi_pCharacterObject);
  void SetTransform(CHARACTEROBJECT *pi_pCharacterObject, int pi_nObjectIndex, D3DXMATRIX &pi_matWorld);
  void SetRender(void)
  {
    m_Render = this;
  }

private:
  static CRender *m_Render;
};

class ChInterface : public CRender
{
public:
  void InitCharacter(IDirect3DDevice8 *pi_pD3DDevice);
  void LoadAnimationData(CHARACTEROBJECT *pi_pCharacterObject, char *pi_pAniName);
  void LoadTextureData(TextureManager *pi_pTextureManager, char *pi_pPathName, char *pi_pFileName);
  CHARACTEROBJECT *LoadMeshData(TextureManager *pi_pTextureManager,
                                CObjectManager *pi_pObjectManager,
                                char *pi_pFileName,
                                bool pi_bUseImmediateFile,
                                CHARACTEROBJECT *pi_pParentBone);
  CHARACTEROBJECT *LoadMeshData(char *pi_pFileName, bool pi_bUseImmediateFile, CHARACTEROBJECT *pi_pParentBone);
  CHARACTEROBJECT *LoadMeshData(CObjectManager *pi_pObjectManager,
                                char *pi_pFileName,
                                bool pi_bUseImmediateFile,
                                CHARACTEROBJECT *pi_pParentBone);
  CHARACTEROBJECT *LoadBoneData(char *pi_pFileName, bool pi_bUseImmediateFile);
  CHARACTEROBJECT *GetMeshData(char *pi_pFileName);
  CHARACTEROBJECT *GetMeshData(CObjectManager *pi_pObjectManager, char *pi_pFileName);
  CHARACTEROBJECT *GetMeshData(CObjectManager *pi_pObjectManager, unsigned long pi_dwID);
  unsigned long GetID(CObjectManager *pi_pObjectManager, char *pi_pFileName);
  CHARACTEROBJECT *GetBoneData(char *pi_pFileName);
  void DeleteMeshData(char *pi_pFileName);
  void DeleteMeshData(unsigned long pi_dwID);
  void DeleteBoneData(char *pi_pFileName);
  void DeleteBoneData(unsigned long pi_dwID);
  ChAnimation *AddAnimationData(AnimationManager *pi_pAniManager, char *pi_pAniName);
  ChAnimation *GetAnimationData(AnimationManager *pi_pAniManager, char *pi_pAniName);
  bool IsLoadAnimation(ChAnimation *pi_pAnimation);
  void MatchAnimationToMesh(ChAnimation *pi_pAnimation, CHARACTEROBJECT *pi_pCharacterObject);
  void AnimationReset(CHARACTEROBJECT *pi_pCharacterObject);
  void AddPartToMM(char *pi_pPartName);
  CObjectManager *GetPart(char *pi_pPartName);
  void SetAlpha(CHARACTEROBJECT *pi_pCharacterObject, float pi_fAlpha);
  float GetAlpha(CHARACTEROBJECT *pi_pCharacterObject);
  void SetMaterial(CHARACTEROBJECT *pi_pCharacterObject, D3DMATERIAL8 &pi_stMaterial);
  int ImportBoundBox(char *pi_pFileName, float *const po_pfMin, float *const po_pfMax);
  void RematchParent(CHARACTEROBJECT *pi_pParentBone, CHARACTEROBJECT *pi_pCharacterObject);
  void RestoreParent(CHARACTEROBJECT *pi_pCharacterObject);
  void SetEyeVec(char *pi_pDummyName, CHARACTEROBJECT *pi_pCharacterObject, D3DXMATRIX pi_matRot);
};

static_assert(sizeof(CHARACTEROBJECT) == 0x308, "CHARACTEROBJECT size mismatch");
static_assert(sizeof(ChAnimation) == 0x118, "ChAnimation size mismatch");
static_assert(sizeof(AnimationManager) == 0x18, "AnimationManager size mismatch");
static_assert(sizeof(TextureManager) == 0xA0, "TextureManager size mismatch");
static_assert(sizeof(CObjectManager) == 0x94, "CObjectManager size mismatch");
static_assert(sizeof(CRender) == 0x308, "CRender size mismatch");
