#pragma once

#include "IdaCompat.h"
#include "D3D8Compat.h"
#include "D3DXMATRIX.h"
#include "CAniCamera.h"
#include "CTimer.h"
#include "CExtDummy.h"

class CBsp;
class CSkyBox;

/* 1315 */
class  CLevel
{
public:
  CLevel();
  virtual ~CLevel();

  char mMapName[256];
  float mCamPos[3];
  D3DXMATRIX mMatView;
  int mIsLoadedBsp;
  CBsp *mBsp;
  CSkyBox *mSkyBox;
  CAniCamera mAutoAniCam;
  CTimer mTimer;
  CExtDummy mDummy;
  unsigned int mLightTexMemSize;
  unsigned int mMapTexMemSize;
  unsigned int mSkyTexMemSize;
  unsigned int mEntityTexMemSize;
  unsigned int mEnvironment;

  float GetFirstYpos(float *fCenterPos, float *fMin, float *fMax);
  void LoadLevel(const char *szFileName);
  void ReleaseLevel();
  __int64 IsLoadedBsp();
  unsigned int GetEnvironment();
  void SetEnvironment(int nEnv);
  void ReLoadAllMaterial();
  void SetCameraPos(float *const a2);
  void SetViewMatrix(D3DXMATRIX *a2);
  void FrameMove();
  void DrawSkyBoxRender();
  void DrawMapRender();
  void DrawMapEntitiesRender();
  void DrawMapAlphaRender(float *const a2);
  void DrawLeafBBox();
  void DrawMatBBox();
  void PrepareShadowRender(float *const a2, void *a3, float a4, unsigned int a5, float a6, float a7);
  void DrawShadowRender(float *const a2, float *a3, float *a4);
  void HearMapSound();
  void GetFrustumNormalPlane(float (*const a2)[4]);
  void DrawBBox(float *const a2, float *const a3, int a4);
  void DrawBBox(__int16 *const a2, __int16 *const a3, int a4);
  void DrawTestBox(float *const a2, float *const a3, unsigned int a4);
  void ScreenShot();
  char *GetMapName();
  __int64 GetPointFromScreenRay(int a2, int a3, float (*a4)[3]);
  __int64 GetPointFromScreenRayFar(int a2, int a3, float (*a4)[3]);
  __int64 GetNextYpos(float *const a2, float *a3);
  __int64 GetNextYposFar(float *const a2, float *const a3, float *a4);
  __int64 GetNextYposNoAttr(float *const a2, float *a3);
  bool IsCollisionRayAABB(int a2, int a3, float *const a4, float *const a5, float (*a6)[3]);
  void CalcR3Fog();
  __int64 GetNextYposForServer(float *const a2, float *a3);
  __int64 GetNextYposForServerFar(float *const a2, float *const a3, float *a4);
  __int64 GetNextYposFarProgress(float *const a2, float *const a3, float *a4);
  unsigned int GetPath(float *const a2, float *const a3, float (*const a4)[3], unsigned int *a5, int a6);
  unsigned int GetPathFromDepth(float *const a2, float *const a3, int a4, float (*const a5)[3], unsigned int *a6);
public:
};


