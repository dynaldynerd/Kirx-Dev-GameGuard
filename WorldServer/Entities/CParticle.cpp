#include "pch.h"

#include "CParticle.h"

#include "CBsp.h"
#include "CEntity.h"
#include "R3EngineGlobals.h"
#include "WorldServerUtil.h"

#include <cmath>
#include <cstdlib>
#include <cstring>

struct _PRE_PARTICLE_LIST
{
  char name[64];
  unsigned int mFlag;
  int mCreateArea[3];
  float mLiveTime;
  float mStartGravity;
  float mEndGravity;
  float mPowerNormal[3];
  float mTimeSpeed;
  unsigned __int8 mATrack[6];
  unsigned __int8 mRTrack[6];
  unsigned __int8 mGTrack[6];
  unsigned __int8 mBTrack[6];
  float mScaleTrack[6];
};

static const _PRE_PARTICLE_LIST stPreParticleList[] = {
  { "", 0, { 0, 0, 0 }, 1.0f, 2.0f, 2.0f, { 0.0f, 0.0f, 0.0f }, 1.0f, { 255, 203, 152, 101, 50, 0 }, { 255, 255, 255, 255, 255, 255 }, { 255, 255, 255, 255, 255, 255 }, { 255, 255, 255, 255, 255, 255 }, { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f } },
  { "", 0, { 0, 0, 0 }, 4.0f, 2.0f, 2.0f, { 0.0f, 0.0f, 0.0f }, 1.0f, { 255, 203, 152, 101, 50, 0 }, { 255, 255, 255, 255, 255, 255 }, { 255, 255, 255, 255, 255, 255 }, { 255, 255, 255, 255, 255, 255 }, { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f } },
  { "", 0, { 0, 0, 0 }, 4.0f, -2.0f, -2.0f, { 0.0f, 0.0f, 0.0f }, 1.0f, { 255, 203, 152, 101, 50, 0 }, { 255, 255, 255, 255, 255, 255 }, { 255, 255, 255, 255, 255, 255 }, { 255, 255, 255, 255, 255, 255 }, { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f } },
  { "", 0, { 0, 0, 0 }, 1.0f, 2.0f, 2.0f, { 0.0f, 0.0f, 0.0f }, 1.0f, { 255, 203, 152, 101, 50, 0 }, { 255, 255, 255, 255, 255, 255 }, { 255, 255, 255, 255, 255, 255 }, { 255, 255, 255, 255, 255, 255 }, { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f } },
  { "", 0, { 0, 0, 0 }, 4.0f, 2.0f, -0.7f, { 0.0f, -5.0f, -10.0f }, 2.0f, { 255, 203, 152, 101, 50, 0 }, { 255, 255, 255, 255, 255, 255 }, { 255, 255, 255, 255, 255, 255 }, { 255, 255, 255, 255, 255, 255 }, { 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f } },
  { "", 0, { 100, 0, 100 }, 4.0f, -2.0f, -2.0f, { 0.0f, 0.0f, 0.0f }, 1.0f, { 255, 203, 152, 101, 50, 0 }, { 255, 255, 255, 255, 255, 255 }, { 255, 255, 255, 255, 255, 255 }, { 255, 255, 255, 255, 255, 255 }, { 0.2f, 1.0f, 1.0f, 1.0f, 1.0f, 4.0f } },
};

float sub_140517000(float result, float a2)
{
  const int minValue = static_cast<int>(result * 32767.0f);
  const int maxValue = static_cast<int>(a2 * 32767.0f);
  if (maxValue != minValue)
  {
    const DWORD tickCount = GetTickCount();
    return static_cast<float>(static_cast<int>(minValue + tickCount * rand() % (maxValue - minValue))) / 32767.0f;
  }
  return result;
}

float Noise1(int a1)
{
  return 1.0f
         - (float)((float)((((a1 << 13) ^ a1) * (15731 * ((a1 << 13) ^ a1) * ((a1 << 13) ^ a1) + 789221) - 771171059)
                         & 0x7FFFFFFF)
                 * 9.3132257e-10f);
}

void Vector3fTransform(float *const a1, float *const a2, float (*const a3)[4])
{
  const float x = a2[0];
  const float y = a2[1];
  const float z = a2[2];
  *a1 = ((y * (*a3)[4]) + (x * (*a3)[0]) + (z * (*a3)[8])) + (*a3)[12];
  a1[1] = ((y * (*a3)[5]) + (x * (*a3)[1]) + (z * (*a3)[9])) + (*a3)[13];
  a1[2] = ((y * (*a3)[6]) + (x * (*a3)[2]) + (z * (*a3)[10])) + (*a3)[14];
}

void sub_140518700(float *a1, float *a2, float *a3)
{
  *a3 = (float)(a2[2] * a1[1]) - (float)(a2[1] * a1[2]);
  a3[1] = (float)(*a2 * a1[2]) - (float)(*a1 * a2[2]);
  a3[2] = (float)(*a1 * a2[1]) - (float)(*a2 * a1[1]);
}

void _PARTICLE_ELEMENT::UpDate(float a2)
{
  mPos[0] += mDir[0] * a2;
  mPos[1] += mDir[1] * a2;
  mPos[2] += mDir[2] * a2;
  mDir[0] += mDirStep[0] * a2;
  mDir[1] += mDirStep[1] * a2;
  mDir[2] += mDirStep[2] * a2;
  mARGB[0] += mARGBStep[0] * a2;
  mARGB[1] += mARGBStep[1] * a2;
  mARGB[2] += mARGBStep[2] * a2;
  mARGB[3] += mARGBStep[3] * a2;
  mScale += mScaleStep * a2;
  mNowFrame += a2;
  mZRot += mZRotStep * a2;
  mYRot += mYRotStep * a2;
}

void GetEntityAnimationPos(float *const a1, CParticle *a2)
{
  CEntity *mEntity = a2->mEntity;
  *a1 = 0.0f;
  a1[1] = 0.0f;
  a1[2] = 0.0f;
  if ((a2->mFlag & 0x40) != 0)
  {
    int mMatGroupNum = mEntity->mMatGroupNum;
    if (mMatGroupNum > 1)
    {
      _ENTITY_M_GROUP *mMatGroup = mEntity->mMatGroup;
      unsigned int idx = mMatGroupNum - 1;
      unsigned __int16 *cur = &mMatGroup[mMatGroupNum - 1].ObjectId;
      while (!*cur)
      {
        --idx;
        cur -= 26;
      }
      _ENTITY_M_GROUP *group = &mMatGroup[idx];
      if (idx != static_cast<unsigned int>(mMatGroupNum))
      {
        mEntity->PrepareAnimation();
        double frame = static_cast<double>(a2->mTotalTime);
        frame = frame * 30.0;
        float mat[18]{};
        GetObjectMatrix(reinterpret_cast<float (*)[4]>(mat), group->ObjectId, mEntity->mObject, frame);
        *a1 = mat[12];
        a1[1] = mat[13];
        a1[2] = mat[14];
      }
    }
  }
}

CParticle::CParticle()
{
  std::memset(this, 0, sizeof(CParticle));
}

CParticle::~CParticle()
{
  _PARTICLE_ELEMENT *mElement = this->mElement;
  if (mElement)
    Dfree(mElement);
}

void CParticle::SetCreatePos(float *const a2)
{
  mCreatePos[0] = *a2;
  mCreatePos[1] = a2[1];
  mCreatePos[2] = a2[2];
}

void CParticle::CopyParticleToSaveParticle(_SAVE_PARTICLE *sParticle)
{
  std::memset(sParticle, 0, sizeof(_SAVE_PARTICLE));
  std::strcpy(sParticle->mEntityName, mEntityName);
  sParticle->mNum = mNum;
  sParticle->mAlphaType = mAlphaType;
  sParticle->mFlag = mFlag;
  sParticle->mLiveTime = mLiveTime;
  sParticle->mGravity[0] = mGravity[0];
  sParticle->mGravity[1] = mGravity[1];
  sParticle->mGravity[2] = mGravity[2];
  sParticle->mTimeSpeed = mTimeSpeed;
  std::memcpy(sParticle->mATrack, mATrack, 96);
}

void CParticle::CopySaveParticleToParticle(_SAVE_PARTICLE *sParticle)
{
  std::memset(this, 0, sizeof(CParticle));
  std::strcpy(mEntityName, sParticle->mEntityName);
  mFlag = sParticle->mFlag;
  mNum = static_cast<int>(sParticle->mNum);
  mAlphaType = sParticle->mAlphaType;
  mLiveTime = sParticle->mLiveTime;
  mTimeSpeed = sParticle->mTimeSpeed;
  std::memcpy(mATrack, sParticle->mATrack, 96);
}

void CParticle::SetPreCalcParticle(unsigned int type)
{
  if (type >= (sizeof(stPreParticleList) / sizeof(stPreParticleList[0])))
  {
    return;
  }

  mFlag = stPreParticleList[type].mFlag;
  mLiveTime = stPreParticleList[type].mLiveTime;
  mTimeSpeed = stPreParticleList[type].mTimeSpeed;
  std::memcpy(mATrack, stPreParticleList[type].mATrack, 96);
}

__int64 CParticle::GetParticleState()
{
  return static_cast<unsigned int>(mState);
}

void CParticle::ResetOnePerTime()
{
  mOnePerTime = (float)(mLiveTime / mTimeSpeed) / (float)mNum;
}

void CParticle::ReInitParticle(int a2)
{
  const int previousNum = mNum;
  if (previousNum == a2)
  {
    return;
  }

  mTotalTime = 0.0f;
  mElement = reinterpret_cast<_PARTICLE_ELEMENT *>(ReAlloc(reinterpret_cast<unsigned int *>(mElement), 104 * previousNum, 104 * a2));
  for (int index = previousNum; index < a2; ++index)
  {
    InitElement(index, 0.0f);
  }
  mNum = a2;
  mOnePerTime = (float)(mLiveTime / mTimeSpeed) / (float)a2;
}

void CParticle::ReleaseParticle()
{
  if (mElement)
  {
    Dfree(mElement);
  }
  mNum = 0;
  mElement = nullptr;
}

void CParticle::ReleaseEntity()
{
  CEntity *entity = mEntity;
  if (entity)
  {
    entity->ReleaseEntity();
    entity->~CEntity();
    operator delete(entity);
    mEntity = nullptr;
  }
}

void CParticle::GetBBox(float *const getMin, float *const getMax)
{
  *getMin = 65000.0f;
  getMin[1] = 65000.0f;
  getMin[2] = 65000.0f;
  *getMax = -65000.0f;
  getMax[1] = -65000.0f;
  getMax[2] = -65000.0f;

  _PARTICLE_ELEMENT elementBackup{};
  std::memcpy(&elementBackup, mElement, sizeof(_PARTICLE_ELEMENT));

  _ENTITY_M_GROUP *matGroup = &mEntity->mMatGroup[0];
  float bbMax = -65000.0f;
  for (int vertexIndex = 0; vertexIndex < static_cast<int>(matGroup->VCnt); ++vertexIndex)
  {
    _D3DR3VERTEX_TEX1 *vertex = &mEntity->mStaticVertexBuffer.m_VertexBufferTex1[vertexIndex + matGroup->VBMinIndex];
    const float absX = static_cast<float>(std::fabs(vertex->x));
    const float absY = static_cast<float>(std::fabs(vertex->y));
    const float absZ = static_cast<float>(std::fabs(vertex->z));
    if (bbMax < absX) bbMax = absX;
    if (bbMax < absY) bbMax = absY;
    if (bbMax < absZ) bbMax = absZ;
  }

  float startPosBackup[3][3]{};
  std::memcpy(startPosBackup[0], mStartPos[0], sizeof(float) * 3);
  std::memcpy(startPosBackup[1], mStartPos[1], sizeof(float) * 3);
  std::memcpy(startPosBackup[2], mCreatePos, sizeof(float) * 3);

  mCreatePos[0] = 0.0f;
  mCreatePos[1] = 0.0f;
  mCreatePos[2] = 0.0f;
  mStartPos[0][0] = 0.0f;
  mStartPos[0][1] = 0.0f;
  mStartPos[0][2] = 0.0f;
  mStartPos[1][0] = 0.0f;
  mStartPos[1][1] = 0.0f;
  mStartPos[1][2] = 0.0f;

  InitElement(0, 0.0f);
  for (float time = 0.0f; time < (mLiveTime / mTimeSpeed); time += 0.1f)
  {
    mElement[0].mTime = time;
    GetPartcleStep(0, 0.1f);

    if (mElement[0].mPos[0] < getMin[0])
      getMin[0] = mElement[0].mPos[0];
    if (mElement[0].mPos[1] < getMin[1])
      getMin[1] = mElement[0].mPos[1];
    if (mElement[0].mPos[2] < getMin[2])
      getMin[2] = mElement[0].mPos[2];

    if (mElement[0].mPos[0] > getMax[0])
      getMax[0] = mElement[0].mPos[0];
    if (mElement[0].mPos[1] > getMax[1])
      getMax[1] = mElement[0].mPos[1];
    if (mElement[0].mPos[2] > getMax[2])
      getMax[2] = mElement[0].mPos[2];
  }

  getMin[0] -= bbMax;
  getMin[1] -= bbMax;
  getMin[2] -= bbMax;
  getMax[0] += bbMax;
  getMax[1] += bbMax;
  getMax[2] += bbMax;

  std::memcpy(mStartPos[0], startPosBackup[0], sizeof(float) * 3);
  std::memcpy(mStartPos[1], startPosBackup[1], sizeof(float) * 3);
  std::memcpy(mCreatePos, startPosBackup[2], sizeof(float) * 3);

  getMin[0] += mStartPos[0][0];
  getMin[1] += mStartPos[0][1];
  getMin[2] += mStartPos[0][2];
  getMax[0] += mStartPos[1][0];
  getMax[1] += mStartPos[1][1];
  getMax[2] += mStartPos[1][2];

  std::memcpy(mElement, &elementBackup, sizeof(_PARTICLE_ELEMENT));
}

void CParticle::GetFlickerARGB(int i, unsigned int *dwArgb)
{
  if ((mElement[i].mFlag & 1) == 0)
  {
    return;
  }

  const unsigned int flickerTime = static_cast<unsigned int>(mFlickerTime * 65536.0f);
  if (flickerTime == 0)
  {
    return;
  }

  if (static_cast<unsigned int>(mElement[i].mNowFrame * 32768.0f) % flickerTime < (flickerTime >> 1))
  {
    const unsigned __int8 alpha = static_cast<unsigned __int8>((*dwArgb >> 24) & 0xFF);
    if (alpha > mFlickerAlpha)
    {
      *dwArgb = (*dwArgb & 0xFFFFFF) | (mFlickerAlpha << 24);
    }
  }
}

__int64 CParticle::Loop()
{
  const float loopTime = R3GetLoopTime();
  const bool useSpecialLoop = (mFlag & (1u << 18)) != 0;

  mTotalTime += loopTime;
  if (!useSpecialLoop)
  {
    return RealLoop();
  }

  const __int64 specialLoopResult = SpecialLoop();
  if (specialLoopResult == -1)
  {
    return RealLoop();
  }

  return specialLoopResult;
}

__int64 CParticle::RealLoop()
{
  unsigned int liveCount = 0;
  const float loopTime = R3GetLoopTime();
  float frameTime = loopTime;
  if (mTimeSpeed * loopTime > 1.0f)
  {
    frameTime = loopTime / mTimeSpeed;
  }

  mParticleTimer += frameTime;
  if (mState != 0)
  {
    mNextCreatorTime -= frameTime;
  }

  for (int elementIndex = 0; elementIndex < mNum; ++elementIndex)
  {
    _PARTICLE_ELEMENT &element = mElement[elementIndex];
    if (element.mIsLive)
    {
      ++liveCount;
      element.mTime += frameTime;
      if (element.mTime <= mLiveTime / mTimeSpeed)
      {
        GetPartcleStep(elementIndex, frameTime);
      }
      else
      {
        element.mIsLive = 0;
        --liveCount;
        while (element.mTime > mLiveTime / mTimeSpeed)
        {
          element.mTime -= mLiveTime / mTimeSpeed;
        }
      }
    }

    const unsigned int flag = mFlag;
    if ((flag & 0x800) != 0)
    {
      continue;
    }

    if ((flag & 0x2000) != 0 && mParticleTimer > mEmitTime)
    {
      if (elementIndex == mNum - 1)
      {
        mFlag = flag | 0x800;
      }
      continue;
    }

    const float epsilon = mOnePerTimeEpsilonTemp;
    const float nextCreatorTime = mNextCreatorTime;
    if (epsilon + nextCreatorTime < 0.0f && !element.mIsLive)
    {
      ++liveCount;
      if (nextCreatorTime + frameTime < 0.0f)
      {
        const int left = static_cast<int>(nextCreatorTime * 65536.0f);
        const int right = static_cast<int>(frameTime * 65536.0f);
        mNextCreatorTime = static_cast<float>(left % right) * 0.000015258789f;
      }

      InitElement(elementIndex, -0.0f - (epsilon + mNextCreatorTime));
      element.mIsLive = 1;
      mNextCreatorTime = ((1.0f - mStartTimeRange) * mOnePerTime) + mNextCreatorTime;

      const DWORD tickCount = GetTickCount();
      const float noise = Noise1(static_cast<int>(tickCount));
      const unsigned int updatedFlag = mFlag;
      mOnePerTimeEpsilonTemp = ((noise - 0.5f) * mOnePerTimeEpsilon) * (1.0f - mStartTimeRange);
      if ((updatedFlag & 1) != 0 && elementIndex == mNum - 1)
      {
        mFlag = updatedFlag | 0x800;
      }
    }
  }

  return liveCount;
}

__int64 CParticle::SpecialLoop()
{
  switch (mSpecialID)
  {
    case 1:
      return -1;
    case 2:
      return SpecialLoop2();
    case 3:
      return -1;
    case 4:
    {
      const float dx = mSpecialARGV[0][0] - mSpecialARGV[1][0];
      const float dy = mSpecialARGV[0][1] - mSpecialARGV[1][1];
      const float dz = mSpecialARGV[0][2] - mSpecialARGV[1][2];
      mStartPos[1][1] = std::sqrt(dx * dx + dy * dy + dz * dz) + mStartPos[0][1];
      return -1;
    }
    default:
      break;
  }

  if (mSpecialID != 5)
  {
    return 0;
  }

  const float oldGravityX = mGravity[0];
  const float oldGravityY = mGravity[1];
  const float oldGravityZ = mGravity[2];
  const float oldStartPower00 = mStartPower[0][0];
  const float oldStartPower01 = mStartPower[0][1];
  const float oldStartPower02 = mStartPower[0][2];
  const float oldStartPower10 = mStartPower[1][0];
  const float oldStartPower11 = mStartPower[1][1];
  const float oldStartPower12 = mStartPower[1][2];

  const float dx = mSpecialARGV[0][0] - mSpecialARGV[1][0];
  const float dy = mSpecialARGV[0][1] - mSpecialARGV[1][1];
  const float dz = mSpecialARGV[0][2] - mSpecialARGV[1][2];
  const float distanceScale = std::sqrt(dx * dx + dy * dy + dz * dz) / 10.0f;

  mGravity[0] = oldGravityX * distanceScale;
  mGravity[1] = oldGravityY * distanceScale;
  mGravity[2] = oldGravityZ * distanceScale;
  mStartPower[0][0] = oldStartPower00 * distanceScale;
  mStartPower[0][1] = oldStartPower01 * distanceScale;
  mStartPower[0][2] = oldStartPower02 * distanceScale;
  mStartPower[1][0] = oldStartPower10 * distanceScale;
  mStartPower[1][1] = oldStartPower11 * distanceScale;
  mStartPower[1][2] = oldStartPower12 * distanceScale;

  const __int64 result = RealLoop();

  mGravity[0] = oldGravityX;
  mGravity[1] = oldGravityY;
  mGravity[2] = oldGravityZ;
  mStartPower[0][0] = oldStartPower00;
  mStartPower[0][1] = oldStartPower01;
  mStartPower[0][2] = oldStartPower02;
  mStartPower[1][0] = oldStartPower10;
  mStartPower[1][1] = oldStartPower11;
  mStartPower[1][2] = oldStartPower12;
  return result;
}

__int64 CParticle::SpecialLoop2()
{
  unsigned int liveCount = 0;
  const float loopTime = R3GetLoopTime();
  float frameTime = loopTime;
  if (mTimeSpeed * loopTime > 1.0f)
  {
    frameTime = loopTime / mTimeSpeed;
  }

  mParticleTimer += frameTime;
  if (mState != 0)
  {
    mNextCreatorTime -= frameTime;
  }

  for (int elementIndex = 0; elementIndex < mNum; ++elementIndex)
  {
    _PARTICLE_ELEMENT &element = mElement[elementIndex];
    if (element.mIsLive)
    {
      ++liveCount;
      element.mTime += frameTime;
      if (element.mTime <= mLiveTime / mTimeSpeed)
      {
        GetPartcleStep(elementIndex, frameTime);
      }
      else
      {
        element.mIsLive = 0;
        --liveCount;
        while (element.mTime > mLiveTime / mTimeSpeed)
        {
          element.mTime -= mLiveTime / mTimeSpeed;
        }
      }
    }

    const unsigned int flag = mFlag;
    if ((flag & 0x800) != 0)
    {
      continue;
    }

    if ((flag & 0x2000) != 0 && mParticleTimer > mEmitTime)
    {
      if (elementIndex == mNum - 1)
      {
        mFlag = flag | 0x800;
      }
      continue;
    }

    const float epsilon = mOnePerTimeEpsilonTemp;
    const float nextCreatorTime = mNextCreatorTime;
    if (epsilon + nextCreatorTime < 0.0f && !element.mIsLive)
    {
      ++liveCount;
      if (nextCreatorTime + frameTime < 0.0f)
      {
        const int left = static_cast<int>(nextCreatorTime * 65536.0f);
        const int right = static_cast<int>(frameTime * 65536.0f);
        mNextCreatorTime = static_cast<float>(left % right) * 0.000015258789f;
      }

      InitElement(elementIndex, -0.0f - (epsilon + mNextCreatorTime));

      int nearestLiveIndex = 0;
      float minLiveTime = 100000.0f;
      for (int candidateIndex = 0; candidateIndex < mNum; ++candidateIndex)
      {
        if (candidateIndex == elementIndex)
        {
          continue;
        }

        _PARTICLE_ELEMENT &candidate = mElement[candidateIndex];
        if (candidate.mIsLive && minLiveTime > candidate.mTime)
        {
          minLiveTime = candidate.mTime;
          nearestLiveIndex = candidateIndex;
        }
      }

      if (minLiveTime != 100000.0f)
      {
        _PARTICLE_ELEMENT &nearest = mElement[nearestLiveIndex];
        const float dx = nearest.mPos[0] - element.mPos[0];
        const float dy = nearest.mPos[1] - element.mPos[1];
        const float dz = nearest.mPos[2] - element.mPos[2];
        if (dx != 0.0f || dy != 0.0f || dz != 0.0f)
        {
          const float length = std::sqrt(dx * dx + dy * dy + dz * dz);
          const float base = mNextCreatorTime + mOnePerTimeEpsilonTemp;
          const float offset =
            ((-0.0f - (base * length)) / (((1.0f - mStartTimeRange) * mOnePerTime) - base));
          element.mPos[0] += (dx / length) * offset;
          element.mPos[1] += (dy / length) * offset;
          element.mPos[2] += (dz / length) * offset;
        }
      }

      element.mIsLive = 1;
      mNextCreatorTime = ((1.0f - mStartTimeRange) * mOnePerTime) + mNextCreatorTime;

      const DWORD tickCount = GetTickCount();
      const float noise = Noise1(static_cast<int>(tickCount));
      const unsigned int updatedFlag = mFlag;
      mOnePerTimeEpsilonTemp = ((noise - 0.5f) * mOnePerTimeEpsilon) * (1.0f - mStartTimeRange);
      if ((updatedFlag & 1) != 0 && elementIndex == mNum - 1)
      {
        mFlag = updatedFlag | 0x800;
      }
    }
  }

  return liveCount;
}

__int64 CParticle::LoadParticleSPT(char *a2, int a3)
{
  FILE *particleFile = fopenMFM(a2, "rt");
  if (!particleFile)
  {
    fopenMFM(a2, "rt");
    Warning(a2, aAiAaiai_6);
    return 0;
  }

  char token[256]{};
  if (fscanf(particleFile, "%s", token) == -1)
  {
    Warning(a2, aAiAaiaoA);
    fclose(particleFile);
    return 0;
  }

  while (true)
  {
    if (!strcmp(token, "end"))
    {
      Warning(a2, aAiAaiaoA);
      fclose(particleFile);
      return 0;
    }
    if (!strcmp(token, "[Particle]"))
      break;
    if (fscanf(particleFile, "%s", token) == -1)
    {
      Warning(a2, aAiAaiaoA);
      fclose(particleFile);
      return 0;
    }
  }

  int hasTrackContext = -1;
  std::memset(this, 0, sizeof(CParticle));
  this->mStartScale[0] = 1.0f;
  this->mStartScale[1] = 1.0f;
  this->mStartARGB[0][0] = 255.0f;
  this->mStartARGB[1][0] = 255.0f;
  this->mStartARGB[2][0] = 255.0f;
  this->mStartARGB[3][0] = 255.0f;
  this->mStartARGB[0][1] = 255.0f;
  this->mStartARGB[1][1] = 255.0f;
  this->mStartARGB[2][1] = 255.0f;
  this->mStartARGB[3][1] = 255.0f;
  this->mTimeSpeed = 1.0f;
  this->mAlphaType = 3;
  int currentTrackIndex = 0;
  float parsedValue = 0.0f;
  float parsedValue2 = 0.0f;
  auto readNextToken = [&]() {
    return fscanf(particleFile, "%s", token) != -1;
  };
  if (readNextToken())
  {
    while (strcmp(token, "end"))
    {
      _strlwr(token);
      if (!strcmp(token, "entity_file"))
      {
        fscanf(particleFile, "%s", token);
        std::strcpy(this->mEntityName, token);
        CEntity *entityStorage = reinterpret_cast<CEntity *>(operator new(244));
        CEntity *entity = entityStorage ? new (entityStorage) CEntity() : nullptr;
        this->mEntity = entity;
        if (!entity || !(unsigned int)entity->LoadEntity(this->mEntityName, a3 | 1u))
        {
          fclose(particleFile);
          CEntity *mEntity = this->mEntity;
          if (mEntity)
          {
            mEntity->~CEntity();
            operator delete(mEntity);
          }
          this->mEntity = nullptr;
          return 0;
        }
        this->mEntity->RestoreTexMem();
      }
      if (!strcmp(token, "num"))
      {
        fscanf(particleFile, "%s", token);
        this->mNum = atoi(token);
      }
      if (!strcmp(token, "start_time_range"))
      {
        fscanf(particleFile, "%s", token);
        this->mStartTimeRange = static_cast<float>(atof(token));
      }
      if (!strcmp(token, "pos"))
      {
        fscanf(particleFile, "%s", token);
        if (strcmp(token, "box"))
        {
          if (!strcmp(token, "sphere_xz"))
          {
            this->mFlag |= 0x80000000u;
          }
          else
          {
            if (strcmp(token, "sphere_xy"))
            {
              Warning(token, (char *)&byte_1408851D8);
              if (!readNextToken())
                break;
              continue;
            }
            this->mFlag |= 0x40000000u;
          }
        }
        if (!(unsigned int)GetRandOrNum(particleFile, this->mStartPos[0], this->mStartPos[1]))
          this->mStartPos[1][0] = this->mStartPos[0][0];
        if (!(unsigned int)GetRandOrNum(particleFile, &this->mStartPos[0][1], &this->mStartPos[1][1]))
          this->mStartPos[1][1] = this->mStartPos[0][1];
        if (!(unsigned int)GetRandOrNum(particleFile, &this->mStartPos[0][2], &this->mStartPos[1][2]))
          this->mStartPos[1][2] = this->mStartPos[0][2];
      }
      if (!strcmp(token, "no_billboard"))
        this->mFlag |= 8u;
      if (!strcmp(token, "z_disable"))
        this->mFlag |= 4u;
      if (!strcmp(token, "live_time"))
        GetRandOrNum(particleFile, &this->mLiveTime, &parsedValue);
      if (!strcmp(token, "alpha_type"))
      {
        fscanf(particleFile, "%s", token);
        this->mAlphaType = atoi(token);
      }
      if (!strcmp(token, "always_live"))
        this->mFlag |= 0x20u;
      if (!strcmp(token, "time_speed"))
        GetRandOrNum(particleFile, &this->mTimeSpeed, &parsedValue);
      if (!strcmp(token, "gravity"))
      {
        GetRandOrNum(particleFile, this->mGravity, &parsedValue);
        GetRandOrNum(particleFile, &this->mGravity[1], &parsedValue);
        GetRandOrNum(particleFile, &this->mGravity[2], &parsedValue);
      }
      if (!strcmp(token, "start_power"))
      {
        if (!(unsigned int)GetRandOrNum(particleFile, this->mStartPower[0], this->mStartPower[1]))
          this->mStartPower[1][0] = this->mStartPower[0][0];
        if (!(unsigned int)GetRandOrNum(particleFile, &this->mStartPower[0][1], &this->mStartPower[1][1]))
          this->mStartPower[1][1] = this->mStartPower[0][1];
        if (!(unsigned int)GetRandOrNum(particleFile, &this->mStartPower[0][2], &this->mStartPower[1][2]))
          this->mStartPower[1][2] = this->mStartPower[0][2];
      }
      if (!strcmp(token, "start_scale") && !(unsigned int)GetRandOrNum(particleFile, this->mStartScale, &this->mStartScale[1]))
        this->mStartScale[1] = this->mStartScale[0];
      if (!strcmp(token, "start_zrot"))
      {
        this->mFlag |= 0x100u;
        if (!(unsigned int)GetRandOrNum(particleFile, this->mStartZRot, &this->mStartZRot[1]))
          this->mStartZRot[1] = this->mStartZRot[0];
      }
      if (!strcmp(token, "start_yrot"))
      {
        this->mFlag |= 0x1000u;
        if (!(unsigned int)GetRandOrNum(particleFile, this->mStartYRot, &this->mStartYRot[1]))
          this->mStartYRot[1] = this->mStartYRot[0];
      }
      if (!strcmp(token, "y_billboard"))
        this->mFlag |= 0x400u;
      if (!strcmp(token, "z_billboard"))
        this->mFlag |= 0x200u;
      if (!strcmp(token, "free"))
        this->mFlag |= 0x8000u;
      if (!strcmp(token, "night"))
        this->mFlag |= 0x10000u;
      if (!strcmp(token, "check_collision"))
        this->mFlag |= 0x10u;
      if (!strcmp(token, "entity_ani"))
        this->mFlag |= 0x40u;
      if (!strcmp(token, "z_front"))
      {
        this->mFlag |= 0x4000u;
        GetRandOrNum(particleFile, &this->mZFront, &parsedValue);
      }
      if (!strcmp(token, "emit_time"))
      {
        this->mFlag |= 0x2000u;
        GetRandOrNum(particleFile, &this->mEmitTime, &parsedValue);
      }
      if (!strcmp(token, "start_alpha"))
        GetRandOrNum(particleFile, this->mStartARGB[0], &this->mStartARGB[0][1]);
      if (!strcmp(token, "start_color"))
      {
        GetRandOrNum(particleFile, this->mStartARGB[1], &this->mStartARGB[1][1]);
        GetRandOrNum(particleFile, this->mStartARGB[2], &this->mStartARGB[2][1]);
        GetRandOrNum(particleFile, this->mStartARGB[3], &this->mStartARGB[3][1]);
      }
      if (!strcmp(token, "flicker_alpha"))
      {
        fscanf(particleFile, "%s", token);
        this->mFlickerAlpha = atoi(token);
      }
      if (!strcmp(token, "flicker_time"))
      {
        fscanf(particleFile, "%s", token);
        const float flickerTime = static_cast<float>(atof(token));
        this->mFlickerTime = flickerTime;
        if (this->mTimeSpeed != 0.0f)
          this->mFlickerTime = flickerTime / this->mTimeSpeed;
      }
      if (!strcmp(token, "flicker"))
        this->mFlag |= 0x20000u;
      if (!strcmp(token, "create_time_epsilon"))
        GetRandOrNum(particleFile, &this->mOnePerTimeEpsilon, &parsedValue);
      if (!strcmp(token, "elasticity"))
        GetRandOrNum(particleFile, &this->mElasticity, &parsedValue);
      if (!strcmp(token, "special_id"))
      {
        fscanf(particleFile, "%s", token);
        this->mSpecialID = atoi(token);
        this->mFlag |= 0x40000u;
      }
      if (!strcmp(token, "tex_repeat"))
      {
        fscanf(particleFile, "%s", token);
        this->mTexRepeat = static_cast<float>(atof(token));
      }
      if (!strcmp(token, "time"))
      {
        hasTrackContext = 0;
        fscanf(particleFile, "%s", token);
        currentTrackIndex = this->mTrackCnt;
        this->mTimeTrack[currentTrackIndex] = static_cast<float>(atof(token));
        this->mTrackCnt = currentTrackIndex + 1;
        if (this->mTrackCnt >= 12)
          Error(aAo_0, (char *)byte_140883769);
      }
      if (!strcmp(token, "power"))
      {
        if (hasTrackContext == -1)
        {
          Warning(token, aAi_2);
          if (!readNextToken())
            break;
          continue;
        }
        if (!(unsigned int)GetRandOrNum(particleFile, this->mPowerTrack[currentTrackIndex][0], this->mPowerTrack[currentTrackIndex][1]))
          this->mPowerTrack[currentTrackIndex][1][0] = this->mPowerTrack[currentTrackIndex][0][0];
        if (!(unsigned int)GetRandOrNum(particleFile, &this->mPowerTrack[currentTrackIndex][0][1], &this->mPowerTrack[currentTrackIndex][1][1]))
          this->mPowerTrack[currentTrackIndex][1][1] = this->mPowerTrack[currentTrackIndex][0][1];
        if (!(unsigned int)GetRandOrNum(particleFile, &this->mPowerTrack[currentTrackIndex][0][2], &this->mPowerTrack[currentTrackIndex][1][2]))
          this->mPowerTrack[currentTrackIndex][1][2] = this->mPowerTrack[currentTrackIndex][0][2];
        this->mTrackFlag[currentTrackIndex] |= 0x10u;
      }
      if (!strcmp(token, "alpha"))
      {
        if (hasTrackContext == -1)
        {
          Warning(token, aAi_2);
          if (!readNextToken())
            break;
          continue;
        }
        GetRandOrNum(particleFile, &parsedValue, &parsedValue);
        this->mATrack[currentTrackIndex] = static_cast<unsigned __int8>(parsedValue);
        this->mTrackFlag[currentTrackIndex] |= 0x80u;
      }
      if (!strcmp(token, "zrot"))
      {
        this->mTrackFlag[currentTrackIndex] |= 8u;
        this->mFlag |= 0x100u;
        if (hasTrackContext == -1)
        {
          Warning(token, aAi_2);
          if (!readNextToken())
            break;
          continue;
        }
        if (!(unsigned int)GetRandOrNum(particleFile, this->mZRotTrack[currentTrackIndex], &this->mZRotTrack[currentTrackIndex][1]))
          this->mZRotTrack[currentTrackIndex][1] = this->mZRotTrack[currentTrackIndex][0];
      }
      if (!strcmp(token, "yrot"))
      {
        this->mTrackFlag[currentTrackIndex] |= 4u;
        this->mFlag |= 0x1000u;
        if (hasTrackContext == -1)
        {
          Warning(token, aAi_2);
          if (!readNextToken())
            break;
          continue;
        }
        if (!(unsigned int)GetRandOrNum(particleFile, this->mYRotTrack[currentTrackIndex], &this->mYRotTrack[currentTrackIndex][1]))
          this->mYRotTrack[currentTrackIndex][1] = this->mYRotTrack[currentTrackIndex][0];
      }
      if (!strcmp(token, "flicker"))
        this->mTrackFlag[currentTrackIndex] |= 2u;
      if (!strcmp(token, "color"))
      {
        if (hasTrackContext == -1)
        {
          Warning(token, aAi_2);
          if (!readNextToken())
            break;
          continue;
        }
        GetRandOrNum(particleFile, &parsedValue, &parsedValue2);
        this->mRTrack[currentTrackIndex][0] = static_cast<unsigned __int8>(parsedValue);
        this->mRTrack[currentTrackIndex][1] = static_cast<unsigned __int8>(parsedValue2);
        GetRandOrNum(particleFile, &parsedValue, &parsedValue2);
        this->mGTrack[currentTrackIndex][0] = static_cast<unsigned __int8>(parsedValue);
        this->mGTrack[currentTrackIndex][1] = static_cast<unsigned __int8>(parsedValue2);
        GetRandOrNum(particleFile, &parsedValue, &parsedValue2);
        this->mBTrack[currentTrackIndex][0] = static_cast<unsigned __int8>(parsedValue);
        this->mBTrack[currentTrackIndex][1] = static_cast<unsigned __int8>(parsedValue2);
        this->mTrackFlag[currentTrackIndex] |= 0x40u;
      }
      if (!strcmp(token, "scale"))
      {
        if (hasTrackContext == -1)
        {
          Warning(token, aAi_2);
          if (!readNextToken())
            break;
          continue;
        }
        if (!(unsigned int)GetRandOrNum(particleFile, this->mScaleTrack[currentTrackIndex], &this->mScaleTrack[currentTrackIndex][1]))
          this->mScaleTrack[currentTrackIndex][1] = this->mScaleTrack[currentTrackIndex][0];
        this->mTrackFlag[currentTrackIndex] |= 0x20u;
      }
      if (!readNextToken())
        break;
    }
  }
  fclose(particleFile);
  return 1;
}

void CParticle::InitParticle()
{
  if (this->mNum < 1)
  {
    Warning(aA_0, (char *)byte_140883769);
    return;
  }

  if (this->mLiveTime == 0.0f)
  {
    Warning(aA_2, (char *)byte_140883769);
    return;
  }

  this->mTotalTime = 0.0f;
  this->mElement = reinterpret_cast<_PARTICLE_ELEMENT *>(Dmalloc(sizeof(_PARTICLE_ELEMENT) * this->mNum));
  for (int elementIndex = 0; elementIndex < this->mNum; ++elementIndex)
    this->InitElement(elementIndex, 0.0f);
  this->mOnePerTime = (float)(this->mLiveTime / this->mTimeSpeed) / (float)this->mNum;
}

void CParticle::InitElement(int a2, float a3)
{
  _PARTICLE_ELEMENT &element = this->mElement[a2];
  element.mTime = 0.0f;
  element.mFlag = 0;
  if ((this->mFlag & 0x20000u) != 0)
    element.mFlag |= 1u;

  float startDirection[3] = {
    sub_140517000(this->mStartPower[0][0], this->mStartPower[1][0]),
    sub_140517000(this->mStartPower[0][1], this->mStartPower[1][1]),
    sub_140517000(this->mStartPower[0][2], this->mStartPower[1][2]),
  };
  if ((this->mFlag & 0x8000u) != 0)
  {
    Vector3fTransform(element.mDir, startDirection, this->mRotMat);
  }
  else
  {
    element.mDir[0] = startDirection[0];
    element.mDir[1] = startDirection[1];
    element.mDir[2] = startDirection[2];
  }

  element.mDirStep[0] = 0.0f;
  element.mDirStep[1] = 0.0f;
  element.mDirStep[2] = 0.0f;

  const int particleFlag = this->mFlag;
  float localStartPos[3]{};
  float transformedStartPos[3]{};
  if (particleFlag >= 0)
  {
    if ((particleFlag & 0x40000000) != 0)
    {
      transformedStartPos[0] = sub_140517000(this->mStartPos[0][0], this->mStartPos[1][0]);
      transformedStartPos[1] = sub_140517000(this->mStartPos[0][1], this->mStartPos[1][1]);
      localStartPos[2] = sub_140517000(this->mStartPos[0][2], this->mStartPos[1][2]);
      localStartPos[0] = sub_140517000(-0.0f - transformedStartPos[0], transformedStartPos[0]);
      localStartPos[1] = std::sqrt((float)(1.0f - (float)((float)(localStartPos[0] * localStartPos[0]) / (float)(transformedStartPos[0] * transformedStartPos[0])))
                               * (float)(transformedStartPos[1] * transformedStartPos[1]));
      const int randomValue = rand();
      if (((randomValue >> 31) ^ (randomValue & 1)) == randomValue >> 31)
        localStartPos[1] = -0.0f - localStartPos[1];
    }
    else
    {
      localStartPos[0] = sub_140517000(this->mStartPos[0][0], this->mStartPos[1][0]);
      localStartPos[1] = sub_140517000(this->mStartPos[0][1], this->mStartPos[1][1]);
      localStartPos[2] = sub_140517000(this->mStartPos[0][2], this->mStartPos[1][2]);
    }
  }
  else
  {
    transformedStartPos[0] = sub_140517000(this->mStartPos[0][0], this->mStartPos[1][0]);
    transformedStartPos[2] = sub_140517000(this->mStartPos[0][2], this->mStartPos[1][2]);
    localStartPos[1] = sub_140517000(this->mStartPos[0][1], this->mStartPos[1][1]);
    localStartPos[0] = sub_140517000(-0.0f - transformedStartPos[0], transformedStartPos[0]);
    localStartPos[2] = std::sqrt((float)(1.0f - (float)((float)(localStartPos[0] * localStartPos[0]) / (float)(transformedStartPos[0] * transformedStartPos[0])))
                             * (float)(transformedStartPos[2] * transformedStartPos[2]));
    const int randomValue = rand();
    if (((randomValue >> 31) ^ (randomValue & 1)) == randomValue >> 31)
      localStartPos[2] = -0.0f - localStartPos[2];
  }

  element.mZRot = sub_140517000(this->mStartZRot[0], this->mStartZRot[1]);
  element.mYRot = sub_140517000(this->mStartYRot[0], this->mStartYRot[1]);
  if ((this->mFlag & 0x8000u) != 0)
  {
    Vector3fTransform(transformedStartPos, localStartPos, this->mRotMat);
    element.mPos[0] = transformedStartPos[0] + this->mCreatePos[0];
    element.mPos[1] = transformedStartPos[1] + this->mCreatePos[1];
    element.mPos[2] = transformedStartPos[2] + this->mCreatePos[2];
  }
  else
  {
    element.mPos[0] = localStartPos[0];
    element.mPos[1] = localStartPos[1];
    element.mPos[2] = localStartPos[2];
  }

  float entityAnimationPos[4]{};
  GetEntityAnimationPos(entityAnimationPos, this);
  element.mPos[0] = entityAnimationPos[0] + element.mPos[0];
  element.mPos[1] = entityAnimationPos[1] + element.mPos[1];
  element.mPos[2] = entityAnimationPos[2] + element.mPos[2];
  element.mNowFrame = 0.0f;
  element.mNowTrack = 0;
  element.mIsLive = 0;
  element.mTime = a3;
  if (element.mTime > (float)(this->mLiveTime / this->mTimeSpeed))
    element.mTime = 0.0f;

  this->GetPartcleStep(a2, element.mTime);
  if (this->mTrackCnt)
  {
    float trackDuration = this->mTimeTrack[0] - (float)(element.mTime * this->mTimeSpeed);
    if (static_cast<float>(std::fabs(trackDuration)) < 0.3f)
      trackDuration = FLOAT_1_0;
    if ((this->mTrackFlag[0] & 0x80u) == 0)
    {
      element.mARGBStep[0] = 0.0f;
    }
    else
    {
      element.mARGBStep[0] = (float)((float)this->mATrack[0] - sub_140517000(this->mStartARGB[0][0], this->mStartARGB[0][1])) / trackDuration;
    }
    if ((this->mTrackFlag[0] & 0x40) != 0)
    {
      element.mARGBStep[1] = (float)(sub_140517000((float)this->mRTrack[0][0], (float)this->mRTrack[0][1]) - sub_140517000(this->mStartARGB[1][0], this->mStartARGB[1][1]))
                           / trackDuration;
      element.mARGBStep[2] = (float)(sub_140517000((float)this->mGTrack[0][0], (float)this->mGTrack[0][1]) - sub_140517000(this->mStartARGB[2][0], this->mStartARGB[2][1]))
                           / trackDuration;
      element.mARGBStep[3] = (float)(sub_140517000((float)this->mBTrack[0][0], (float)this->mBTrack[0][1]) - sub_140517000(this->mStartARGB[3][0], this->mStartARGB[3][1]))
                           / trackDuration;
    }
    else
    {
      element.mARGBStep[1] = 0.0f;
      element.mARGBStep[2] = 0.0f;
      element.mARGBStep[3] = 0.0f;
    }
    if ((this->mTrackFlag[0] & 0x20) != 0)
    {
      element.mScaleStep = (float)(sub_140517000(this->mScaleTrack[0][0], this->mScaleTrack[0][1]) - sub_140517000(this->mStartScale[0], this->mStartScale[1])) / trackDuration;
    }
    else
    {
      element.mScaleStep = 0.0f;
    }
    if ((this->mTrackFlag[0] & 8) != 0)
      element.mZRotStep = sub_140517000(this->mZRotTrack[0][0], this->mZRotTrack[0][1]) / trackDuration;
    else
      element.mZRotStep = 0.0f;
    if ((this->mTrackFlag[0] & 4) != 0)
      element.mYRotStep = sub_140517000(this->mYRotTrack[0][0], this->mYRotTrack[0][1]) / trackDuration;
    else
      element.mYRotStep = 0.0f;
  }
  else
  {
    element.mARGBStep[0] = 0.0f;
    element.mARGBStep[1] = 0.0f;
    element.mARGBStep[2] = 0.0f;
    element.mARGBStep[3] = 0.0f;
    element.mScaleStep = 0.0f;
  }

  element.mARGB[0] = sub_140517000(this->mStartARGB[0][0], this->mStartARGB[0][1]);
  element.mARGB[1] = sub_140517000(this->mStartARGB[1][0], this->mStartARGB[1][1]);
  element.mARGB[2] = sub_140517000(this->mStartARGB[2][0], this->mStartARGB[2][1]);
  element.mARGB[3] = sub_140517000(this->mStartARGB[3][0], this->mStartARGB[3][1]);
  element.mScale = sub_140517000(this->mStartScale[0], this->mStartScale[1]);
  element.mZRot = sub_140517000(this->mStartZRot[0], this->mStartZRot[1]);
  element.mYRot = sub_140517000(this->mStartYRot[0], this->mStartYRot[1]);
}

void CParticle::GetPartcleStep(int a2, float a3)
{
  float particleTime = a3 * this->mTimeSpeed;
  if (particleTime > this->mLiveTime)
    particleTime = this->mLiveTime;

  this->CheckCollision(a2, particleTime);

  _PARTICLE_ELEMENT &element = this->mElement[a2];
  if (this->mTrackCnt > element.mNowTrack)
  {
    float timeSpeed = this->mTimeSpeed;
    bool updatedTrack = false;
    if ((float)(timeSpeed * element.mTime) >= this->mTimeTrack[element.mNowTrack])
    {
      while (true)
      {
        const int nextTrack = element.mNowTrack + 1;
        if (this->mTrackCnt - 1 < nextTrack)
          break;

        float trackDuration = this->mTimeTrack[nextTrack] - (float)(timeSpeed * element.mTime);
        if (trackDuration >= 0.0f)
        {
          updatedTrack = true;
          if (static_cast<float>(std::fabs(trackDuration)) < 0.3f)
            trackDuration = FLOAT_1_0;

          if ((this->mTrackFlag[element.mNowTrack] & 0x10) != 0)
          {
            float targetDirection[3] = {
              sub_140517000(this->mPowerTrack[element.mNowTrack][0][0], this->mPowerTrack[element.mNowTrack][1][0]),
              sub_140517000(this->mPowerTrack[element.mNowTrack][0][1], this->mPowerTrack[element.mNowTrack][1][1]),
              sub_140517000(this->mPowerTrack[element.mNowTrack][0][2], this->mPowerTrack[element.mNowTrack][1][2]),
            };
            if ((this->mFlag & 0x8000u) != 0)
            {
              Vector3fTransform(element.mDirStep, targetDirection, this->mRotMat);
            }
            else
            {
              element.mDirStep[0] = targetDirection[0];
              element.mDirStep[1] = targetDirection[1];
              element.mDirStep[2] = targetDirection[2];
            }
            element.mDirStep[0] = element.mDirStep[0] - (float)(element.mDir[0] / trackDuration);
            element.mDirStep[1] = element.mDirStep[1] - (float)(element.mDir[1] / trackDuration);
            element.mDirStep[2] = element.mDirStep[2] - (float)(element.mDir[2] / trackDuration);
          }

          if ((this->mTrackFlag[element.mNowTrack] & 2) != 0)
          {
            if ((element.mFlag & 1) != 0)
              element.mFlag &= ~1u;
            else
              element.mFlag |= 1u;
          }

          if ((this->mTrackFlag[nextTrack] & 0x80u) == 0)
          {
            element.mARGBStep[0] = 0.0f;
          }
          else
          {
            element.mARGBStep[0] = (float)((float)this->mATrack[nextTrack] - element.mARGB[0]) / trackDuration;
          }

          if ((this->mTrackFlag[nextTrack] & 0x40) != 0)
          {
            element.mARGBStep[1] = (float)(sub_140517000((float)this->mRTrack[nextTrack][0], (float)this->mRTrack[nextTrack][1]) - element.mARGB[1]) / trackDuration;
            element.mARGBStep[2] = (float)(sub_140517000((float)this->mGTrack[nextTrack][0], (float)this->mGTrack[nextTrack][1]) - element.mARGB[2]) / trackDuration;
            element.mARGBStep[3] = (float)(sub_140517000((float)this->mBTrack[nextTrack][0], (float)this->mBTrack[nextTrack][1]) - element.mARGB[3]) / trackDuration;
          }
          else
          {
            element.mARGBStep[1] = 0.0f;
            element.mARGBStep[2] = 0.0f;
            element.mARGBStep[3] = 0.0f;
          }

          if ((this->mTrackFlag[nextTrack] & 0x20) != 0)
          {
            element.mScaleStep = (float)(sub_140517000(this->mScaleTrack[nextTrack][0], this->mScaleTrack[nextTrack][1]) - element.mScale) / trackDuration;
          }
          else
          {
            element.mScaleStep = 0.0f;
          }

          if ((this->mTrackFlag[nextTrack] & 8) != 0)
            element.mZRotStep = sub_140517000(this->mZRotTrack[nextTrack][0], this->mZRotTrack[nextTrack][1]) / trackDuration;
          else
            element.mZRotStep = 0.0f;
          if ((this->mTrackFlag[nextTrack] & 4) != 0)
            element.mYRotStep = sub_140517000(this->mYRotTrack[nextTrack][0], this->mYRotTrack[nextTrack][1]) / trackDuration;
          else
            element.mYRotStep = 0.0f;
        }

        ++element.mNowTrack;
        if (this->mTrackCnt > element.mNowTrack)
        {
          timeSpeed = this->mTimeSpeed;
          if ((float)(timeSpeed * element.mTime) >= this->mTimeTrack[element.mNowTrack])
            continue;
        }
        return;
      }

      if (!updatedTrack)
      {
        element.mDirStep[0] = 0.0f;
        element.mDirStep[1] = 0.0f;
        element.mDirStep[2] = 0.0f;
        element.mARGBStep[0] = 0.0f;
        element.mARGBStep[1] = 0.0f;
        element.mARGBStep[2] = 0.0f;
        element.mARGBStep[3] = 0.0f;
        element.mScaleStep = 0.0f;
      }
    }
  }
}

void CParticle::SetParticleState(int a2)
{
  this->mState = a2;
  switch (a2)
  {
    case 1:
    {
      for (int elementIndex = 0; elementIndex < this->mNum; ++elementIndex)
      {
        this->InitElement(elementIndex, 0.0f);
        this->mElement[elementIndex].mIsLive = 0;
      }
      break;
    }
    case 2:
    {
      float oldestTime = FLOAT_N1_0;
      int firstInactiveIndex = -1;
      int oldestLiveIndex = -1;
      if (this->mNum > 0)
      {
        for (int elementIndex = 0; elementIndex < this->mNum; ++elementIndex)
        {
          _PARTICLE_ELEMENT &element = this->mElement[elementIndex];
          if (!element.mIsLive)
          {
            firstInactiveIndex = elementIndex;
            break;
          }
          if (element.mTime > oldestTime)
          {
            oldestTime = element.mTime;
            oldestLiveIndex = elementIndex;
          }
        }
        if (firstInactiveIndex >= 0)
          this->InitElement(firstInactiveIndex, 0.0f);
        if (oldestLiveIndex >= 0)
          this->InitElement(oldestLiveIndex, 0.0f);
      }
      break;
    }
    case 3:
    {
      for (int elementIndex = 0; elementIndex < this->mNum; ++elementIndex)
      {
        const DWORD tickCount = GetTickCount();
        const float epsilon = (float)(Noise1(static_cast<int>(tickCount)) - 0.5f) * this->mOnePerTimeEpsilon;
        this->mOnePerTimeEpsilonTemp = epsilon;
        this->InitElement(elementIndex, (float)(epsilon + this->mOnePerTime) * (float)elementIndex);
      }
      break;
    }
    default:
      break;
  }
}

void CParticle::SetStartBoxArea()
{
  if (mNum <= 0)
  {
    return;
  }

  for (int i = 0; i < mNum; ++i)
  {
    _PARTICLE_ELEMENT &element = mElement[i];
    if (!element.mIsLive)
    {
      continue;
    }

    const float createX = mCreatePos[0];
    const float createZ = mCreatePos[2];
    const float widthX = mStartPos[1][0] - mStartPos[0][0];
    const float widthZ = mStartPos[1][2] - mStartPos[0][2];

    float localX = element.mPos[0] - createX;
    if (mStartPos[0][0] > localX)
    {
      localX += widthX;
    }
    if (localX > mStartPos[1][0])
    {
      localX -= widthX;
    }
    element.mPos[0] = createX + localX;

    float localZ = element.mPos[2] - createZ;
    if (mStartPos[0][2] > localZ)
    {
      localZ += widthZ;
    }
    if (localZ > mStartPos[1][2])
    {
      localZ -= widthZ;
    }
    element.mPos[2] = createZ + localZ;
  }
}

void CParticle::CheckCollision(int a2, float a3)
{
  if ((this->mFlag & 0x10) != 0)
  {
    CBsp *bsp = reinterpret_cast<CBsp *>(this->mBsp);
    if (bsp)
    {
      _PARTICLE_ELEMENT &element = this->mElement[a2];
      float nextPos[3] = {
        (float)(a3 * element.mDir[0]) + element.mPos[0],
        (float)(a3 * element.mDir[1]) + element.mPos[1],
        (float)(a3 * element.mDir[2]) + element.mPos[2],
      };
      float nowPos[3]{};
      if ((this->mFlag & 0x8000u) != 0)
      {
        nowPos[0] = element.mPos[0];
        nowPos[1] = element.mPos[1];
        nowPos[2] = element.mPos[2];
      }
      else
      {
        nextPos[0] += this->mCreatePos[0];
        nextPos[1] += this->mCreatePos[1];
        nextPos[2] += this->mCreatePos[2];
        nowPos[0] = this->mCreatePos[0] + element.mPos[0];
        nowPos[1] = this->mCreatePos[1] + element.mPos[1];
        nowPos[2] = this->mCreatePos[2] + element.mPos[2];
      }

      float collisionPos[4]{};
      float collisionNormal[4]{};
      if (bsp->IsCollisionFace(nowPos, nextPos, reinterpret_cast<float (*)[3]>(collisionPos), reinterpret_cast<float (*)[4]>(collisionNormal)))
      {
        float reflectionAxis[3]{};
        sub_140518700(collisionNormal, element.mDir, reflectionAxis);

        float mirrorVector[3]{};
        float mirrorNormal[3]{};
        const float planeOffset = 0.0f;
        if (reflectionAxis[0] == 0.0f || reflectionAxis[1] == 0.0f || reflectionAxis[2] == 0.0f)
        {
          mirrorVector[0] = FLOAT_1_0;
          mirrorNormal[0] = FLOAT_1_0;
        }
        else
        {
          sub_140518700(collisionNormal, reflectionAxis, mirrorVector);
          const float reflectionLength = std::sqrt((float)((float)(reflectionAxis[1] * reflectionAxis[1]) + (float)(reflectionAxis[0] * reflectionAxis[0]))
                                               + (float)(reflectionAxis[2] * reflectionAxis[2]));
          reflectionAxis[0] /= reflectionLength;
          reflectionAxis[1] /= reflectionLength;
          reflectionAxis[2] /= reflectionLength;
          const float mirrorLength = std::sqrt((float)((float)(mirrorVector[1] * mirrorVector[1]) + (float)(mirrorVector[0] * mirrorVector[0]))
                                           + (float)(mirrorVector[2] * mirrorVector[2]));
          mirrorNormal[0] = mirrorVector[0] / mirrorLength;
          mirrorNormal[1] = mirrorVector[1] / mirrorLength;
          mirrorNormal[2] = mirrorVector[2] / mirrorLength;
          mirrorVector[0] = mirrorNormal[0];
          mirrorVector[1] = mirrorNormal[1];
          mirrorVector[2] = mirrorNormal[2];
        }

        float mirrorMatrix[16]{};
        mirrorMatrix[15] = FLOAT_1_0;
        mirrorMatrix[0] = 1.0f - (float)((float)(mirrorNormal[0] * 2.0f) * mirrorNormal[0]);
        mirrorMatrix[1] = 0.0f - (float)((float)(mirrorNormal[0] * 2.0f) * mirrorNormal[1]);
        mirrorMatrix[2] = 0.0f - (float)((float)(mirrorNormal[0] * 2.0f) * mirrorNormal[2]);
        mirrorMatrix[4] = 0.0f - (float)((float)(mirrorNormal[1] * 2.0f) * mirrorNormal[0]);
        mirrorMatrix[5] = 1.0f - (float)((float)(mirrorNormal[1] * 2.0f) * mirrorNormal[1]);
        mirrorMatrix[6] = 0.0f - (float)((float)(mirrorNormal[1] * 2.0f) * mirrorNormal[2]);
        mirrorMatrix[8] = 0.0f - (float)((float)(mirrorNormal[2] * 2.0f) * mirrorNormal[0]);
        mirrorMatrix[9] = 0.0f - (float)((float)(mirrorNormal[2] * 2.0f) * mirrorNormal[1]);
        mirrorMatrix[10] = 1.0f - (float)((float)(mirrorNormal[2] * 2.0f) * mirrorNormal[2]);
        mirrorMatrix[12] = 0.0f - (float)((float)(planeOffset * 2.0f) * mirrorNormal[0]);
        mirrorMatrix[13] = 0.0f - (float)((float)(planeOffset * 2.0f) * mirrorNormal[1]);
        mirrorMatrix[14] = 0.0f - (float)((float)(planeOffset * 2.0f) * mirrorNormal[2]);

        float currentDirection[3] = { element.mDir[0], element.mDir[1], element.mDir[2] };
        Vector3fTransform(mirrorVector, currentDirection, reinterpret_cast<float (*)[4]>(mirrorMatrix));
        element.mDir[0] = -0.0f - (float)(mirrorVector[0] * this->mElasticity);
        element.mDir[1] = -0.0f - (float)(mirrorVector[1] * this->mElasticity);
        element.mDir[2] = -0.0f - (float)(mirrorVector[2] * this->mElasticity);
        element.mDirStep[0] = 0.0f;
        element.mDirStep[1] = 0.0f;
        element.mDirStep[2] = 0.0f;

        if ((this->mFlag & 0x8000u) != 0)
        {
          element.mPos[0] = collisionPos[0];
        }
        else
        {
          collisionPos[1] = collisionPos[1] + (float)(0.0f - this->mCreatePos[1]);
          collisionPos[2] = collisionPos[2] + (float)(0.0f - this->mCreatePos[2]);
          element.mPos[0] = collisionPos[0] + (float)(0.0f - this->mCreatePos[0]);
        }
        element.mPos[1] = collisionPos[1];
        element.mPos[2] = collisionPos[2];
      }
    }
  }

  _PARTICLE_ELEMENT &element = this->mElement[a2];
  element.UpDate(a3);
  element.mDir[0] = (float)(a3 * this->mGravity[0]) + element.mDir[0];
  element.mDir[1] = (float)(a3 * this->mGravity[1]) + element.mDir[1];
  element.mDir[2] = (float)(a3 * this->mGravity[2]) + element.mDir[2];
}
