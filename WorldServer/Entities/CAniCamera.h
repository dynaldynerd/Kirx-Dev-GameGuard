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
  __int64 GetCameraIndex(char *name);
  _ANI_CAMERA *GetCameraName(unsigned int index);
  __int64 GetDummyID(char *name, int compareLoweredInput);
  void GetDummyMatrix(float (*const matrix)[4], unsigned int index, double elapsedSeconds);
  void GetDummyMatrixSub(float (*const matrix)[4], unsigned int index, double frameTime);
  char *GetDummyParentID(unsigned int index);
  float *GetMatrixAniCamera(double elapsedSeconds);
  bool IsLoadedAniCamera();
  void LoadAniCamera(char *fileName);
  __int64 PlayAniCamera(float (*const matrix)[4], double speed, int useCurrentFrame);
  void ReleaseAniCamera();
  __int64 SetExtCamAni();
  void SetNowFrame(float nowFrame);
  void SetPrePlayCamera(unsigned int cameraIndex, unsigned int startFrame, unsigned int endFrame, unsigned int loopFlag);

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

