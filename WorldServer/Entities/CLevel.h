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
class  __declspec(align(8)) CLevel
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
  __int64 IsLoadedBsp();
  unsigned int GetEnvironment();
  void SetEnvironment(int nEnv);
  __int64 GetNextYposForServer(float *const a2, float *a3);
  __int64 GetNextYposForServerFar(float *const a2, float *const a3, float *a4);
  __int64 GetNextYposFarProgress(float *const a2, float *const a3, float *a4);
  unsigned int GetPath(float *const a2, float *const a3, float (*const a4)[3], unsigned int *a5, int a6);
  unsigned int GetPathFromDepth(float *const a2, float *const a3, int a4, float (*const a5)[3], unsigned int *a6);
public:
};

