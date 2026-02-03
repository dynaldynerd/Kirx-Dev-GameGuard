#pragma once

#include "IdaCompat.h"

/* 1398 */
class __cppobj CParticle
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
  __int64 LoadParticleSPT(char *a2, int a3);
  void InitParticle();
  void SetParticleState(int a2);
  void InitElement(int a2, float a3);
  void GetPartcleStep(int a2, float a3);
  void CheckCollision(int a2, float a3);
};

