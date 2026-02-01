#pragma once

#include "D3D8Compat.h"

struct _R3ENGINE_STATE
{
  char MapPath[128];
  char SystemPath[128];
  char EntityPath[128];
  char EffectPath[128];
  char ScreenShotsPath[128];
  char SoundPath[128];
  char CharPath[128];
  char CharTexturePath[128];
  char MeshName[128];
  char BoneName[128];
  char StopAniName[128];
  char RunAniName[128];
  char SwordAniName[128];
  char SwordMeshName[128];
  char SwordTexturePath[128];

  int mbStartCamera;
  float mNear;
  float mFar;
  float mMipMapBias;
  float mFov;
  float mAsfectRatio;
  float mScreenXsize;
  float mScreenYsize;
  float mFlySpeed;
  unsigned int mRenderBits;
  unsigned int mZbufferBits;
  int mbFullScreen;
  unsigned int mAvailableTextureMem;
  int mIsUsedVertexShader;
  int mWideOn;
  int mIsDrawLogo;
  unsigned int mTextureUnitNum;

  float mGamma;
  unsigned int mTextureDetail;
  int mDetailTexture;
  unsigned int mSceneTextureDetail;
  unsigned int mShadowDetail;
  unsigned int mDynamicLight;
  float mSeeDistance;
  int mSoundOn;
  int mMusicOn;
  float mSoundVol;
  float mMusicVol;
  int mMouseAccelation;
  unsigned int mThisVidMemLevel;

  unsigned int mMaxTexSize;
  unsigned int mTempSize;
  int mShowLightMap;
  int mIsFog;
  int mIsNoFogSky;
  int mFogRangeEnable;
  float mFogStart;
  float mFogEnd;
  unsigned int mFogColor;

  float mMainFogStart;
  float mMainFogEnd;
  unsigned int mMainFogColor;

  char mLensTexName[128];
  int mLensFlareTex;
  float mLensFlareScale[16];
  float mLensFlarePos[3];

  char mEnvEntityName[128];
  unsigned int mEnvID;

  float mMatWorld[4][4];
  float mMatView[4][4];
  float mMatViewProj[4][4];
  float mInvMatView[4][4];
  float mMatProj[4][4];

  float mLightNomalForShadow[3];
  float mMainLightNomal[3];
  float mMainLightYAngle;

  int mBestTextureFormat;
  int mBestSpriteTextureFormat;
  int mBestAlphaTextureFormat;

  long mViewSx;
  long mViewSy;
  long mViewXL;
  long mViewYL;

  float mFrustumPlane[6][4];

  IDirect3DTexture8 *mLogoTexture;
  IDirect3DTexture8 *mDLightTexture;
};

extern _R3ENGINE_STATE stState;
