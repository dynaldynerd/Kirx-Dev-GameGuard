#pragma once

#include "IdaCompat.h"

/* 1315 */
class __cppobj __declspec(align(8)) CLevel
{
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
public:
  virtual ~CLevel() = default;
};

