#pragma once

#include "IdaCompat.h"

/* 1315 */
class __cppobj __declspec(align(8)) CLevel
{
public:
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
  bool LoadLevel(const char *szFileName);
public:
  virtual ~CLevel() = default;
};

