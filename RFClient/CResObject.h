#pragma once

#include "CObject.h"
#include "Character.h"
#include "R3Engine/common/c_vector.h"

#define ROSF_INVALID 0xFFFFFFFF

#define ROSF_NONE 0x0
#define ROSF_RESET ROSF_NONE
#define ROSF_RENDER_BONE 0x1
#define ROSF_RENDER_MESH 0x2
#define ROSF_RENDER 0x3
#define ROSF_RENDER_ALPHA 0x4
#define ROSF_RENDER_ALPHA_STEALTH 0x8
#define ROSF_RENDER_BBOX 0x10
#define ROSF_RENDER_NAME 0x20
#define ROSF_RENDER_SHADOW 0x40
#define ROSF_ANIMATION 0x80
#define ROSF_ANIMATION_LOOP 0x100
#define ROSF_ANIMATION_STOP 0x200
#define ROSF_ANIMATION_STOP_ENGINE 0x400
#define ROSF_ANIMATION_INTERPOLATION 0x800
#define ROSF_CLIPPED 0x1000
#define ROSF_CHECK_Y_POS 0x2000
#define ROSF_RENDER_DEFAULT_MESH 0x4000
#define ROSF_MOVABLE 0x8000
#define ROSF_OUT_OF_VISUAL_FIELD_SERVER 0x10000
#define ROSF_OUT_OF_VISUAL_FIELD_CLIENT 0x20000
#define ROSF_HIDE 0x40000
#define ROSF_AFTER_IMAGE_OF_OBJECT 0x80000
#define ROSF_DO_NOT_PICK 0x100000
#define ROSF_OBSERVER 0x200000
#define ROSF_RENDER_EFFECT 0x20000000
#define ROSF_RENDER_INFO 0x40000000
#define ROSF_DESTROY 0x80000000

#define LIGHT_DEFAULT_COLOR D3DCOLOR_XRGB(128, 128, 128)
#define LIGHT_ENEMY_COLOR1 D3DCOLOR_XRGB(255, 0, 0)
#define LIGHT_ENEMY_COLOR2 D3DCOLOR_XRGB(11, 210, 131)
#define LIGHT_COMPANION_COLOR D3DCOLOR_XRGB(205, 82, 16)
#define LIGHT_NORMAL_COLOR D3DCOLOR_XRGB(200, 200, 200)

#define LIGHT_NONE 0
#define LIGHT_BACKUP 1
#define LIGHT_RESTORE 2

class CResObject : public CObject
{
protected:
  Vector3f m_vecPos;
  Vector3f m_vecPrevPos;
  Vector3f m_vecRot;
  DWORD m_dwStatusFlag;
  CHARACTEROBJECT *m_pBone;
  DWORD m_dwOldBoneFrame;
  DWORD m_dwBoneFrame;
  float m_fScale;
  D3DMATERIAL8 m_d3dMaterial;
  D3DLIGHT8 m_d3dLight;
  DWORD m_dwLightColor;
  DWORD m_dwPrevLightColor;
  float m_fAlphaDensity;
  float m_vecBBoxMin[3];
  float m_vecBBoxMax[3];

public:
  CResObject();
  virtual ~CResObject();

  virtual void Init_EveryFrame(void);
  virtual BOOL Render(void);
  virtual BOOL Animation(DWORD pi_dwAniFrame = static_cast<DWORD>(-1));
  virtual void FrameMove(void);
  virtual BOOL RenderName(void);
  virtual void CreateShadow(void);

  virtual void Enable(DWORD pi_dwFlag);
  virtual void Disable(DWORD pi_dwFlag);
  virtual BOOL IsEnable(DWORD pi_dwFlag) const;
  virtual BOOL IsDisable(DWORD pi_dwFlag) const;
  void ResetStatusFlag(void);

  virtual void SetPosition(float pi_fPosX, float pi_fPosY, float pi_fPosZ, BOOL pi_bSetPrevPosition = TRUE);
  void GetPosition(float po_pfPos[3]) const;
  float *GetPosition(void);
  const float *GetPosition(void) const;

  virtual void SetRotY(float pi_fRotY);
  float GetRotY(void) const;
  void SetScale(float pi_fScale);
  float GetScale(void) const;
  virtual void SetAlphaDensity(float pi_fAlpha);
  float GetAlphaDensity(void) const;

  virtual void SetLightColor(DWORD pi_dwColor, BYTE pi_bFlag = LIGHT_NONE);
  virtual void SetLightColorFromMap(DWORD pi_dwColor);
  DWORD GetLightColor(void) const;

  void SetBonePtr(CHARACTEROBJECT *pi_pBone);
  CHARACTEROBJECT *GetBone(void) const;
  void SetBoundBox(const float pi_pfMin[3], const float pi_pfMax[3]);
  void GetBoundBoxMin(float po_pfMin[3]) const;
  void GetBoundBoxMax(float po_pfMax[3]) const;

  void GetCameraTarget(float po_pfTarget[3]) const;
  float GetCameraExtent(void) const;
  void GetScaleAddPos(float *po_pfScale, float *po_pfAddPos) const;

protected:
  static bool IsValidBBoxHeight(float pi_fHeight);
};
