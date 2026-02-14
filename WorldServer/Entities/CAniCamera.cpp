#include "pch.h"

#include "CAniCamera.h"

CAniCamera::CAniCamera()
{
  AniCamera = nullptr;
  mDummy = nullptr;
  mDummyNum = 0;
  mAniCameraNum = 0;
  mStartFrame = 0;
  mEndFrame = 0;
  mPlayStartFrame = 0;
  mPlayEndFrame = 0;
  mPlayIndex = 0;
  mIsSetPerspect = 0;
  mStartTick = 0.0f;
  mFlag = 0;
  mOldFrame = 0.0f;
  mNowFrame = 0.0f;
}

CAniCamera::~CAniCamera()
{
  // this is not a stub
}

