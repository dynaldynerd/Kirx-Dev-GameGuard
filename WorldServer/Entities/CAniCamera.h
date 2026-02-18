#pragma once

#include "IdaCompat.h"

struct _ANI_CAMERA;
struct _ANI_OBJECT;

/* 1427 */
class  CAniCamera
{
public:
  CAniCamera();
  ~CAniCamera();

private:
  _ANI_CAMERA *AniCamera;
  _ANI_OBJECT *mDummy;
  unsigned int mDummyNum;
  unsigned int mAniCameraNum;
  unsigned int mStartFrame;
  unsigned int mEndFrame;
  unsigned int mPlayStartFrame;
  unsigned int mPlayEndFrame;
  int mPlayIndex;
  int mIsSetPerspect;
  float mStartTick;
  unsigned int mFlag;
  float mOldFrame;
  float mNowFrame;
};

