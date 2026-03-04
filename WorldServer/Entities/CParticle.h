#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"

#define _PARTICLE_SPARE_SIZE 127

struct _SAVE_PARTICLE
{
  char mEntityName[128];
  unsigned int mFlag;
  unsigned int mNum;
  unsigned int mAlphaType;
  int mCreateArea[3];
  float mLiveTime;
  float mGravity[3];
  float mPowerNormal[3];
  float mTimeSpeed;
  unsigned __int8 mATrack[12];
  unsigned __int8 mRTrack[12];
  unsigned __int8 mGTrack[12];
  unsigned __int8 mBTrack[12];
  float mScaleTrack[12];
  unsigned int spare[_PARTICLE_SPARE_SIZE];
};

/* 1398 */
class  CParticle
{
public:
  char mEntityName[128];
  int mNum;
  _PARTICLE_ELEMENT *mElement;
  CEntity *mEntity;
  float mTotalTime;
  float mLiveTime;
  float mStartTimeRange;
  float mTimeSpeed;
  float mGravity[3];
  float mStartPower[2][3];
  float mStartScale[2];
  float mStartZRot[2];
  float mStartYRot[2];
  unsigned __int8 mFlickerAlpha;
  float mFlickerTime;
  float mStartARGB[4][2];
  float mOnePerTimeEpsilon;
  float mRotMat[4][4];
  union
  {
    float mElasticity;
    float mTexRepeat;
  };
  float mZFront;
  float mEmitTime;
  unsigned __int16 mSpecialID;
  unsigned __int16 mTrackCnt;
  float mTimeTrack[12];
  unsigned __int8 mTrackFlag[12];
  unsigned __int8 mATrack[12];
  unsigned __int8 mRTrack[12][2];
  unsigned __int8 mGTrack[12][2];
  unsigned __int8 mBTrack[12][2];
  float mScaleTrack[12][2];
  float mZRotTrack[12][2];
  float mYRotTrack[12][2];
  float mPowerTrack[12][2][3];
  float mSpecialARGV[2][3];
  void *mBsp;
  unsigned int mFlag;
  unsigned int mAlphaType;
  float mStartPos[2][3];
  float mCreatePos[3];
  int mState;
  float mOnePerTime;
  float mOnePerTimeEpsilonTemp;
  float mParticleTimer;
  float mNextCreatorTime;

  CParticle();
  ~CParticle();
  __int64 Loop();
  __int64 RealLoop();
  __int64 SpecialLoop();
  __int64 SpecialLoop2();
  __int64 LoadParticleSPT(char *a2, int a3);
  void CopyParticleToSaveParticle(_SAVE_PARTICLE *sParticle);
  void CopySaveParticleToParticle(_SAVE_PARTICLE *sParticle);
  void InitParticle();
  void ReInitParticle(int num);
  void ReleaseParticle();
  void ReleaseEntity();
  void GetBBox(float *const getMin, float *const getMax);
  void GetFlickerARGB(int i, unsigned int *dwArgb);
  __int64 GetParticleState();
  void SetParticleState(int a2);
  void SetCreatePos(float *const pos);
  void SetPreCalcParticle(unsigned int type = 1);
  void ResetOnePerTime();
  void SetStartBoxArea();
  void InitElement(int a2, float a3);
  void GetPartcleStep(int a2, float a3);
  void CheckCollision(int a2, float a3);
};

