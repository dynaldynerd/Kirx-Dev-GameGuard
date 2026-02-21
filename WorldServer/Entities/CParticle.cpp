#include "pch.h"

#include "CParticle.h"

#include "CBsp.h"
#include "CEntity.h"
#include "R3EngineGlobals.h"
#include "WorldServerUtil.h"

#include <cmath>
#include <cstdlib>
#include <cstring>

float sub_140517000(float result, float a2)
{
  float v2 = a2 * 32767.0f;
  int v3 = static_cast<int>(result * 32767.0f);
  if (static_cast<int>(v2) != v3)
  {
    DWORD TickCount = GetTickCount();
    return static_cast<float>(static_cast<int>(v3 + TickCount * rand() % (static_cast<int>(v2) - v3))) / 32767.0f;
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
  float v3 = *a2;
  float v4 = a2[2];
  float v5 = a2[1];
  float v6 = *a2;
  *a1 = (float)((float)((float)(v5 * (*a3)[4]) + (float)(*a2 * (*a3)[0])) + (float)(v4 * (*a3)[8])) + (*a3)[12];
  a1[1] = (float)((float)((float)(v5 * (*a3)[5]) + (float)(v6 * (*a3)[1])) + (float)(v4 * (*a3)[9])) + (*a3)[13];
  a1[2] = (float)((float)((float)(v5 * (*a3)[6]) + (float)(v3 * (*a3)[2])) + (float)(v4 * (*a3)[10])) + (*a3)[14];
}

void sub_140518700(float *a1, float *a2, float *a3)
{
  *a3 = (float)(a2[2] * a1[1]) - (float)(a2[1] * a1[2]);
  a3[1] = (float)(*a2 * a1[2]) - (float)(*a1 * a2[2]);
  a3[2] = (float)(*a1 * a2[1]) - (float)(*a2 * a1[1]);
}

void _PARTICLE_ELEMENT::UpDate(float a2)
{
  float v2 = this->mDir[0];
  float v3 = this->mDir[1];
  float v4 = this->mDir[2];
  this->mPos[0] = (float)(v2 * a2) + this->mPos[0];
  this->mPos[1] = (float)(v3 * a2) + this->mPos[1];
  float v6 = v4 * a2;
  float v7 = a2 * this->mDirStep[0];
  this->mPos[2] = v6 + this->mPos[2];
  float v8 = a2 * this->mDirStep[1];
  this->mDir[0] = v7 + v2;
  float v9 = a2 * this->mDirStep[2];
  this->mDir[1] = v8 + v3;
  float v10 = a2 * this->mARGBStep[0];
  this->mDir[2] = v9 + v4;
  float v11 = (float)(a2 * this->mARGBStep[1]) + this->mARGB[1];
  this->mARGB[0] = v10 + this->mARGB[0];
  float v12 = (float)(a2 * this->mARGBStep[2]) + this->mARGB[2];
  this->mARGB[1] = v11;
  float v13 = (float)(a2 * this->mARGBStep[3]) + this->mARGB[3];
  this->mARGB[2] = v12;
  float v14 = (float)(a2 * this->mScaleStep) + this->mScale;
  this->mARGB[3] = v13;
  float v15 = (float)(a2 * this->mZRotStep) + this->mZRot;
  this->mScale = v14;
  float v16 = a2;
  float v17 = (float)(a2 * this->mYRotStep) + this->mYRot;
  float v18 = v16 + this->mNowFrame;
  this->mZRot = v15;
  this->mYRot = v17;
  this->mNowFrame = v18;
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
        float v11 = mat[13];
        float v12 = mat[14];
        *a1 = mat[12];
        a1[1] = v11;
        a1[2] = v12;
      }
    }
  }
}

CParticle::CParticle()
{
  memset_0(this, 0, sizeof(CParticle));
}

CParticle::~CParticle()
{
  _PARTICLE_ELEMENT *mElement = this->mElement;
  if (mElement)
    Dfree(mElement);
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
      mStartPos[1][1] = sqrtf_0(dx * dx + dy * dy + dz * dz) + mStartPos[0][1];
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
  const float distanceScale = sqrtf_0(dx * dx + dy * dy + dz * dz) / 10.0f;

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
          const float length = sqrtf_0(dx * dx + dy * dy + dz * dz);
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
  FILE *v5 = fopenMFM(a2, "rt");
  FILE *v6 = v5;
  if (!v5)
  {
    fopenMFM(a2, "rt");
    Warning(a2, aAiAaiai_6);
    return 0;
  }

  char String[256]{};
  int v24 = a3;
  if (fscanf(v5, "%s", String) == -1)
  {
LABEL_142:
    Warning(a2, aAiAaiaoA);
    fclose(v6);
    return 0;
  }

  while (true)
  {
    if (!strcmp(String, "end"))
      goto LABEL_142;
    if (!strcmp(String, "[Particle]"))
      break;
    if (fscanf(v6, "%s", String) == -1)
      goto LABEL_142;
  }

  int v8 = -1;
  memset_0(this, 0, sizeof(CParticle));
  float *mStartScale = this->mStartScale;
  this->mStartScale[0] = 1.0f;
  float *v10 = &this->mStartScale[1];
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
  int mTrackCnt = 0;
  float v22 = 0.0f;
  float v23 = 0.0f;
  if (fscanf(v6, "%s", String) != -1)
  {
    while (strcmp(String, "end"))
    {
      _strlwr(String);
      if (!strcmp(String, "entity_file"))
      {
        fscanf(v6, "%s", String);
        char *v12 = String;
        char v13;
        do
        {
          v13 = *v12;
          v12[(char *)this - String] = *v12;
          ++v12;
        } while (v13);
        CEntity *v14 = reinterpret_cast<CEntity *>(operator new(0xF4uLL));
        CEntity *v15 = v14 ? new (v14) CEntity() : nullptr;
        this->mEntity = v15;
        if (!v15 || !(unsigned int)v15->LoadEntity(this->mEntityName, v24 | 1u))
        {
          fclose(v6);
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
      if (!strcmp(String, "num"))
      {
        fscanf(v6, "%s", String);
        this->mNum = atoi(String);
      }
      if (!strcmp(String, "start_time_range"))
      {
        fscanf(v6, "%s", String);
        float v16 = static_cast<float>(atof(String));
        this->mStartTimeRange = v16;
      }
      if (!strcmp(String, "pos"))
      {
        fscanf(v6, "%s", String);
        if (strcmp(String, "box"))
        {
          if (!strcmp(String, "sphere_xz"))
          {
            this->mFlag |= 0x80000000;
          }
          else
          {
            if (strcmp(String, "sphere_xy"))
            {
              Warning(String, (char *)&byte_1408851D8);
              goto LABEL_136;
            }
            this->mFlag |= 0x40000000u;
          }
        }
        if (!(unsigned int)GetRandOrNum(v6, this->mStartPos[0], this->mStartPos[1]))
          this->mStartPos[1][0] = this->mStartPos[0][0];
        if (!(unsigned int)GetRandOrNum(v6, &this->mStartPos[0][1], &this->mStartPos[1][1]))
          this->mStartPos[1][1] = this->mStartPos[0][1];
        if (!(unsigned int)GetRandOrNum(v6, &this->mStartPos[0][2], &this->mStartPos[1][2]))
          this->mStartPos[1][2] = this->mStartPos[0][2];
      }
      if (!strcmp(String, "no_billboard"))
        this->mFlag |= 8u;
      if (!strcmp(String, "z_disable"))
        this->mFlag |= 4u;
      if (!strcmp(String, "live_time"))
        GetRandOrNum(v6, &this->mLiveTime, &v22);
      if (!strcmp(String, "alpha_type"))
      {
        fscanf(v6, "%s", String);
        this->mAlphaType = atoi(String);
      }
      if (!strcmp(String, "always_live"))
        this->mFlag |= 0x20u;
      if (!strcmp(String, "time_speed"))
        GetRandOrNum(v6, &this->mTimeSpeed, &v22);
      if (!strcmp(String, "gravity"))
      {
        GetRandOrNum(v6, this->mGravity, &v22);
        GetRandOrNum(v6, &this->mGravity[1], &v22);
        GetRandOrNum(v6, &this->mGravity[2], &v22);
      }
      if (!strcmp(String, "start_power"))
      {
        if (!(unsigned int)GetRandOrNum(v6, this->mStartPower[0], this->mStartPower[1]))
          this->mStartPower[1][0] = this->mStartPower[0][0];
        if (!(unsigned int)GetRandOrNum(v6, &this->mStartPower[0][1], &this->mStartPower[1][1]))
          this->mStartPower[1][1] = this->mStartPower[0][1];
        if (!(unsigned int)GetRandOrNum(v6, &this->mStartPower[0][2], &this->mStartPower[1][2]))
          this->mStartPower[1][2] = this->mStartPower[0][2];
      }
      if (!strcmp(String, "start_scale") && !(unsigned int)GetRandOrNum(v6, mStartScale, v10))
        *v10 = *mStartScale;
      if (!strcmp(String, "start_zrot"))
      {
        this->mFlag |= 0x100u;
        if (!(unsigned int)GetRandOrNum(v6, this->mStartZRot, &this->mStartZRot[1]))
          this->mStartZRot[1] = this->mStartZRot[0];
      }
      if (!strcmp(String, "start_yrot"))
      {
        this->mFlag |= 0x1000u;
        if (!(unsigned int)GetRandOrNum(v6, this->mStartYRot, &this->mStartYRot[1]))
          this->mStartYRot[1] = this->mStartYRot[0];
      }
      if (!strcmp(String, "y_billboard"))
        this->mFlag |= 0x400u;
      if (!strcmp(String, "z_billboard"))
        this->mFlag |= 0x200u;
      if (!strcmp(String, "free"))
        this->mFlag |= 0x8000u;
      if (!strcmp(String, "night"))
        this->mFlag |= 0x10000u;
      if (!strcmp(String, "check_collision"))
        this->mFlag |= 0x10u;
      if (!strcmp(String, "entity_ani"))
        this->mFlag |= 0x40u;
      if (!strcmp(String, "z_front"))
      {
        this->mFlag |= 0x4000u;
        GetRandOrNum(v6, &this->mZFront, &v22);
      }
      if (!strcmp(String, "emit_time"))
      {
        this->mFlag |= 0x2000u;
        GetRandOrNum(v6, &this->mEmitTime, &v22);
      }
      if (!strcmp(String, "start_alpha"))
        GetRandOrNum(v6, this->mStartARGB[0], &this->mStartARGB[0][1]);
      if (!strcmp(String, "start_color"))
      {
        GetRandOrNum(v6, this->mStartARGB[1], &this->mStartARGB[1][1]);
        GetRandOrNum(v6, this->mStartARGB[2], &this->mStartARGB[2][1]);
        GetRandOrNum(v6, this->mStartARGB[3], &this->mStartARGB[3][1]);
      }
      if (!strcmp(String, "flicker_alpha"))
      {
        fscanf(v6, "%s", String);
        this->mFlickerAlpha = atoi(String);
      }
      if (!strcmp(String, "flicker_time"))
      {
        fscanf(v6, "%s", String);
        float v17 = static_cast<float>(atof(String));
        this->mFlickerTime = v17;
        float mTimeSpeed = this->mTimeSpeed;
        if (mTimeSpeed != 0.0f)
          this->mFlickerTime = v17 / mTimeSpeed;
      }
      if (!strcmp(String, "flicker"))
        this->mFlag |= 0x20000u;
      if (!strcmp(String, "create_time_epsilon"))
        GetRandOrNum(v6, &this->mOnePerTimeEpsilon, &v22);
      if (!strcmp(String, "elasticity"))
        GetRandOrNum(v6, &this->mElasticity, &v22);
      if (!strcmp(String, "special_id"))
      {
        fscanf(v6, "%s", String);
        this->mSpecialID = atoi(String);
        this->mFlag |= 0x40000u;
      }
      if (!strcmp(String, "tex_repeat"))
      {
        fscanf(v6, "%s", String);
        float v19 = static_cast<float>(atof(String));
        this->mElasticity = v19;
      }
      if (!strcmp(String, "time"))
      {
        v8 = 0;
        fscanf(v6, "%s", String);
        float v20 = static_cast<float>(atof(String));
        this->mTimeTrack[this->mTrackCnt] = v20;
        mTrackCnt = this->mTrackCnt;
        this->mTrackCnt = mTrackCnt + 1;
        if (static_cast<unsigned __int16>(mTrackCnt + 1) >= 0xCu)
          Error(aAo_0, (char *)byte_140883769);
      }
      if (!strcmp(String, "power"))
      {
        if (v8 == -1)
          goto LABEL_132;
        if (!(unsigned int)GetRandOrNum(v6, this->mPowerTrack[mTrackCnt][0], this->mPowerTrack[mTrackCnt][1]))
          this->mPowerTrack[mTrackCnt][1][0] = this->mPowerTrack[mTrackCnt][0][0];
        if (!(unsigned int)GetRandOrNum(v6, &this->mPowerTrack[mTrackCnt][0][1], &this->mPowerTrack[mTrackCnt][1][1]))
          this->mPowerTrack[mTrackCnt][1][1] = this->mPowerTrack[mTrackCnt][0][1];
        if (!(unsigned int)GetRandOrNum(v6, &this->mPowerTrack[mTrackCnt][0][2], &this->mPowerTrack[mTrackCnt][1][2]))
          this->mPowerTrack[mTrackCnt][1][2] = this->mPowerTrack[mTrackCnt][0][2];
        this->mTrackFlag[mTrackCnt] |= 0x10u;
        v10 = &this->mStartScale[1];
      }
      if (!strcmp(String, "alpha"))
      {
        if (v8 == -1)
          goto LABEL_132;
        GetRandOrNum(v6, &v22, &v22);
        this->mATrack[mTrackCnt] = static_cast<unsigned __int8>(v22);
        this->mTrackFlag[mTrackCnt] |= 0x80u;
      }
      if (!strcmp(String, "zrot"))
      {
        this->mTrackFlag[mTrackCnt] |= 8u;
        this->mFlag |= 0x100u;
        if (v8 == -1)
          goto LABEL_132;
        if (!(unsigned int)GetRandOrNum(v6, this->mZRotTrack[mTrackCnt], &this->mZRotTrack[mTrackCnt][1]))
          this->mZRotTrack[mTrackCnt][1] = this->mZRotTrack[mTrackCnt][0];
      }
      if (!strcmp(String, "yrot"))
      {
        this->mTrackFlag[mTrackCnt] |= 4u;
        this->mFlag |= 0x1000u;
        if (v8 == -1)
          goto LABEL_132;
        if (!(unsigned int)GetRandOrNum(v6, this->mYRotTrack[mTrackCnt], &this->mYRotTrack[mTrackCnt][1]))
          this->mYRotTrack[mTrackCnt][1] = this->mYRotTrack[mTrackCnt][0];
      }
      if (!strcmp(String, "flicker"))
        this->mTrackFlag[mTrackCnt] |= 2u;
      if (!strcmp(String, "color"))
      {
        if (v8 == -1)
          goto LABEL_132;
        GetRandOrNum(v6, &v22, &v23);
        this->mRTrack[mTrackCnt][0] = static_cast<unsigned __int8>(v22);
        this->mRTrack[mTrackCnt][1] = static_cast<unsigned __int8>(v23);
        GetRandOrNum(v6, &v22, &v23);
        this->mGTrack[mTrackCnt][0] = static_cast<unsigned __int8>(v22);
        this->mGTrack[mTrackCnt][1] = static_cast<unsigned __int8>(v23);
        GetRandOrNum(v6, &v22, &v23);
        this->mBTrack[mTrackCnt][0] = static_cast<unsigned __int8>(v22);
        this->mBTrack[mTrackCnt][1] = static_cast<unsigned __int8>(v23);
        this->mTrackFlag[mTrackCnt] |= 0x40u;
      }
      if (strcmp(String, "scale"))
        goto LABEL_136;
      if (v8 == -1)
      {
LABEL_132:
        Warning(String, aAi_2);
        goto LABEL_136;
      }
      if (!(unsigned int)GetRandOrNum(v6, this->mScaleTrack[mTrackCnt], &this->mScaleTrack[mTrackCnt][1]))
        this->mScaleTrack[mTrackCnt][1] = this->mScaleTrack[mTrackCnt][0];
      this->mTrackFlag[mTrackCnt] |= 0x20u;
LABEL_136:
      if (fscanf(v6, "%s", String) == -1)
        break;
      mStartScale = this->mStartScale;
    }
  }
  fclose(v6);
  return 1;
}

void CParticle::InitParticle()
{
  int mNum = this->mNum;
  if (mNum >= 1)
  {
    if (this->mLiveTime == 0.0f)
    {
      Warning(aA_2, (char *)byte_140883769);
    }
    else
    {
      int v3 = 0;
      this->mTotalTime = 0.0f;
      _PARTICLE_ELEMENT *v4 = reinterpret_cast<_PARTICLE_ELEMENT *>(Dmalloc(104 * mNum));
      bool v5 = this->mNum <= 0;
      this->mElement = v4;
      if (!v5)
      {
        do
          this->InitElement(v3++, 0.0f);
        while (v3 < this->mNum);
      }
      this->mOnePerTime = (float)(this->mLiveTime / this->mTimeSpeed) / (float)this->mNum;
    }
  }
  else
  {
    Warning(aA_0, (char *)byte_140883769);
  }
}

void CParticle::InitElement(int a2, float a3)
{
  __int64 v5 = a2;
  this->mElement[v5].mTime = 0.0f;
  this->mElement[v5].mFlag = 0;
  if (_bittest(reinterpret_cast<const LONG *>(&this->mFlag), 0x11u))
    this->mElement[v5].mFlag |= 1u;
  float v6 = sub_140517000(this->mStartPower[0][0], this->mStartPower[1][0]);
  float v7 = this->mStartPower[1][1];
  float v42 = v6;
  float v8 = sub_140517000(this->mStartPower[0][1], v7);
  float v9 = this->mStartPower[1][2];
  float v43 = v8;
  float v44 = sub_140517000(this->mStartPower[0][2], v9);
  if (_bittest(reinterpret_cast<const LONG *>(&this->mFlag), 0xFu))
  {
    Vector3fTransform(this->mElement[v5].mDir, &v42, this->mRotMat);
  }
  else
  {
    this->mElement[v5].mDir[0] = v42;
    this->mElement[v5].mDir[1] = v43;
    this->mElement[v5].mDir[2] = v44;
  }
  this->mElement[v5].mDirStep[0] = 0.0f;
  this->mElement[v5].mDirStep[1] = 0.0f;
  this->mElement[v5].mDirStep[2] = 0.0f;
  int mFlag = this->mFlag;
  float v36 = 0.0f;
  float v37 = 0.0f;
  float v38 = 0.0f;
  float v39 = 0.0f;
  float v40 = 0.0f;
  float v41 = 0.0f;
  if (mFlag >= 0)
  {
    if ((mFlag & 0x40000000) != 0)
    {
      float v17 = sub_140517000(this->mStartPos[0][0], this->mStartPos[1][0]);
      float v18 = this->mStartPos[1][1];
      v39 = v17;
      float v19 = sub_140517000(this->mStartPos[0][1], v18);
      float v20 = this->mStartPos[1][2];
      v40 = v19;
      v38 = sub_140517000(this->mStartPos[0][2], v20);
      float v21 = sub_140517000(-0.0f - v39, v39);
      v37 = sqrtf_0((float)(1.0f - (float)((float)(v21 * v21) / (float)(v39 * v39))) * (float)(v40 * v40));
      int v22 = rand();
      if (((v22 >> 31) ^ (v22 & 1)) == v22 >> 31)
        v37 = -0.0f - v37;
      v36 = v21;
    }
    else
    {
      float v23 = sub_140517000(this->mStartPos[0][0], this->mStartPos[1][0]);
      float v24 = this->mStartPos[1][1];
      v36 = v23;
      float v25 = sub_140517000(this->mStartPos[0][1], v24);
      float v26 = this->mStartPos[1][2];
      v37 = v25;
      v38 = sub_140517000(this->mStartPos[0][2], v26);
    }
  }
  else
  {
    float v11 = sub_140517000(this->mStartPos[0][0], this->mStartPos[1][0]);
    float v12 = this->mStartPos[1][2];
    v39 = v11;
    float v13 = sub_140517000(this->mStartPos[0][2], v12);
    float v14 = this->mStartPos[1][1];
    v41 = v13;
    v37 = sub_140517000(this->mStartPos[0][1], v14);
    float v15 = sub_140517000(-0.0f - v39, v39);
    v38 = sqrtf_0((float)(1.0f - (float)((float)(v15 * v15) / (float)(v39 * v39))) * (float)(v41 * v41));
    int v16 = rand();
    if (((v16 >> 31) ^ (v16 & 1)) == v16 >> 31)
      v38 = -0.0f - v38;
    v36 = v15;
  }
  this->mElement[v5].mZRot = sub_140517000(this->mStartZRot[0], this->mStartZRot[1]);
  this->mElement[v5].mYRot = sub_140517000(this->mStartYRot[0], this->mStartYRot[1]);
  if (_bittest(reinterpret_cast<const LONG *>(&this->mFlag), 0xFu))
  {
    Vector3fTransform(&v39, &v36, this->mRotMat);
    this->mElement[v5].mPos[0] = v39 + this->mCreatePos[0];
    this->mElement[v5].mPos[1] = v40 + this->mCreatePos[1];
    this->mElement[v5].mPos[2] = v41 + this->mCreatePos[2];
  }
  else
  {
    this->mElement[v5].mPos[0] = v36;
    this->mElement[v5].mPos[1] = v37;
    this->mElement[v5].mPos[2] = v38;
  }
  float v45[4]{};
  GetEntityAnimationPos(v45, this);
  this->mElement[v5].mPos[0] = v45[0] + this->mElement[v5].mPos[0];
  this->mElement[v5].mPos[1] = v45[1] + this->mElement[v5].mPos[1];
  this->mElement[v5].mPos[2] = v45[2] + this->mElement[v5].mPos[2];
  this->mElement[v5].mNowFrame = 0.0f;
  this->mElement[v5].mNowTrack = 0;
  this->mElement[v5].mIsLive = 0;
  this->mElement[v5].mTime = a3;
  if (this->mElement[v5].mTime > (float)(this->mLiveTime / this->mTimeSpeed))
    this->mElement[v5].mTime = 0.0f;
  this->GetPartcleStep(a2, this->mElement[v5].mTime);
  if (this->mTrackCnt)
  {
    float v29 = this->mTimeTrack[0] - (float)(this->mElement[v5].mTime * this->mTimeSpeed);
    float v30 = static_cast<float>(std::fabs(v29));
    if (v30 < 0.3f)
      v29 = FLOAT_1_0;
    if ((this->mTrackFlag[0] & 0x80u) == 0)
    {
      this->mElement[v5].mARGBStep[0] = 0.0f;
    }
    else
    {
      float v31 = (float)this->mATrack[0];
      this->mElement[v5].mARGBStep[0] = (float)(v31 - sub_140517000(this->mStartARGB[0][0], this->mStartARGB[0][1])) / v29;
    }
    if ((this->mTrackFlag[0] & 0x40) != 0)
    {
      float v32 = sub_140517000((float)this->mRTrack[0][0], (float)this->mRTrack[0][1]);
      this->mElement[v5].mARGBStep[1] = (float)(v32 - sub_140517000(this->mStartARGB[1][0], this->mStartARGB[1][1])) / v29;
      float v33 = sub_140517000((float)this->mGTrack[0][0], (float)this->mGTrack[0][1]);
      this->mElement[v5].mARGBStep[2] = (float)(v33 - sub_140517000(this->mStartARGB[2][0], this->mStartARGB[2][1])) / v29;
      float v34 = sub_140517000((float)this->mBTrack[0][0], (float)this->mBTrack[0][1]);
      this->mElement[v5].mARGBStep[3] = (float)(v34 - sub_140517000(this->mStartARGB[3][0], this->mStartARGB[3][1])) / v29;
    }
    else
    {
      this->mElement[v5].mARGBStep[1] = 0.0f;
      this->mElement[v5].mARGBStep[2] = 0.0f;
      this->mElement[v5].mARGBStep[3] = 0.0f;
    }
    if ((this->mTrackFlag[0] & 0x20) != 0)
    {
      float v35 = sub_140517000(this->mScaleTrack[0][0], this->mScaleTrack[0][1]);
      this->mElement[v5].mScaleStep = (float)(v35 - sub_140517000(this->mStartScale[0], this->mStartScale[1])) / v29;
    }
    else
    {
      this->mElement[v5].mScaleStep = 0.0f;
    }
    if ((this->mTrackFlag[0] & 8) != 0)
      this->mElement[v5].mZRotStep = sub_140517000(this->mZRotTrack[0][0], this->mZRotTrack[0][1]) / v29;
    else
      this->mElement[v5].mZRotStep = 0.0f;
    if ((this->mTrackFlag[0] & 4) != 0)
      this->mElement[v5].mYRotStep = sub_140517000(this->mYRotTrack[0][0], this->mYRotTrack[0][1]) / v29;
    else
      this->mElement[v5].mYRotStep = 0.0f;
  }
  else
  {
    this->mElement[v5].mARGBStep[0] = 0.0f;
    this->mElement[v5].mARGBStep[1] = 0.0f;
    this->mElement[v5].mARGBStep[2] = 0.0f;
    this->mElement[v5].mARGBStep[3] = 0.0f;
    this->mElement[v5].mScaleStep = 0.0f;
  }
  this->mElement[v5].mARGB[0] = sub_140517000(this->mStartARGB[0][0], this->mStartARGB[0][1]);
  this->mElement[v5].mARGB[1] = sub_140517000(this->mStartARGB[1][0], this->mStartARGB[1][1]);
  this->mElement[v5].mARGB[2] = sub_140517000(this->mStartARGB[2][0], this->mStartARGB[2][1]);
  this->mElement[v5].mARGB[3] = sub_140517000(this->mStartARGB[3][0], this->mStartARGB[3][1]);
  this->mElement[v5].mScale = sub_140517000(this->mStartScale[0], this->mStartScale[1]);
  this->mElement[v5].mZRot = sub_140517000(this->mStartZRot[0], this->mStartZRot[1]);
  this->mElement[v5].mYRot = sub_140517000(this->mStartYRot[0], this->mStartYRot[1]);
}

void CParticle::GetPartcleStep(int a2, float a3)
{
  float mLiveTime = a3 * this->mTimeSpeed;
  __int64 v4 = a2;
  if (mLiveTime > this->mLiveTime)
    mLiveTime = this->mLiveTime;
  this->CheckCollision(a2, mLiveTime);
  _PARTICLE_ELEMENT *mElement = this->mElement;
  unsigned __int16 mTrackCnt = this->mTrackCnt;
  __int64 v8 = v4;
  unsigned __int16 v9 = mElement[v4].mNowTrack;
  _PARTICLE_ELEMENT *v10 = &mElement[v4];
  if (mTrackCnt > v9)
  {
    float mTimeSpeed = this->mTimeSpeed;
    int v12 = 0;
    if ((float)(mTimeSpeed * this->mElement[v8].mTime) >= this->mTimeTrack[this->mElement[v8].mNowTrack])
    {
      while (1)
      {
        int v13 = v9 + 1;
        if (mTrackCnt - 1 < v13)
          break;
        __int64 v14 = v13;
        float v15 = this->mTimeTrack[v13] - (float)(mTimeSpeed * v10->mTime);
        if (v15 >= 0.0f)
        {
          v12 = 1;
          float v16 = static_cast<float>(std::fabs(v15));
          if (v16 < 0.3f)
            v15 = FLOAT_1_0;
          if ((this->mTrackFlag[v9] & 0x10) != 0)
          {
            __int64 mNowTrack = this->mElement[v8].mNowTrack;
            float v18 = sub_140517000(this->mPowerTrack[mNowTrack][0][0], this->mPowerTrack[mNowTrack][1][0]);
            _PARTICLE_ELEMENT *v19 = this->mElement;
            float v29 = v18;
            float v20 = sub_140517000(this->mPowerTrack[v19[v8].mNowTrack][0][1], this->mPowerTrack[v19[v8].mNowTrack][1][1]);
            _PARTICLE_ELEMENT *v21 = this->mElement;
            float v30 = v20;
            float v22 = sub_140517000(this->mPowerTrack[v21[v8].mNowTrack][0][2], this->mPowerTrack[v21[v8].mNowTrack][1][2]);
            unsigned __int8 v23 = _bittest(reinterpret_cast<const LONG *>(&this->mFlag), 0xFu);
            float v31 = v22;
            if (v23)
            {
              Vector3fTransform(this->mElement[v8].mDirStep, &v29, this->mRotMat);
            }
            else
            {
              this->mElement[v8].mDirStep[0] = v29;
              this->mElement[v8].mDirStep[1] = v30;
              this->mElement[v8].mDirStep[2] = v31;
            }
            this->mElement[v8].mDirStep[0] = this->mElement[v8].mDirStep[0] - (float)(this->mElement[v8].mDir[0] / v15);
            this->mElement[v8].mDirStep[1] = this->mElement[v8].mDirStep[1] - (float)(this->mElement[v8].mDir[1] / v15);
            this->mElement[v8].mDirStep[2] = this->mElement[v8].mDirStep[2] - (float)(this->mElement[v8].mDir[2] / v15);
          }
          _PARTICLE_ELEMENT *v24 = this->mElement;
          if ((this->mTrackFlag[v24[v8].mNowTrack] & 2) != 0)
          {
            if ((v24[v8].mFlag & 1) != 0)
              v24[v8].mFlag &= ~1u;
            else
              this->mElement[v8].mFlag |= 1u;
          }
          if ((this->mTrackFlag[v14] & 0x80u) == 0)
            this->mElement[v8].mARGBStep[0] = 0.0f;
          else
            this->mElement[v8].mARGBStep[0] = (float)((float)this->mATrack[v14] - this->mElement[v8].mARGB[0]) / v15;
          if ((this->mTrackFlag[v14] & 0x40) != 0)
          {
            _PARTICLE_ELEMENT *v25 = this->mElement;
            v25[v8].mARGBStep[1] = (float)(sub_140517000((float)this->mRTrack[v14][0], (float)this->mRTrack[v14][1]) - v25[v8].mARGB[1]) / v15;
            _PARTICLE_ELEMENT *v26 = this->mElement;
            v26[v8].mARGBStep[2] = (float)(sub_140517000((float)this->mGTrack[v14][0], (float)this->mGTrack[v14][1]) - v26[v8].mARGB[2]) / v15;
            _PARTICLE_ELEMENT *v27 = this->mElement;
            v27[v8].mARGBStep[3] = (float)(sub_140517000((float)this->mBTrack[v14][0], (float)this->mBTrack[v14][1]) - v27[v8].mARGB[3]) / v15;
          }
          else
          {
            this->mElement[v8].mARGBStep[1] = 0.0f;
            this->mElement[v8].mARGBStep[2] = 0.0f;
            this->mElement[v8].mARGBStep[3] = 0.0f;
          }
          if ((this->mTrackFlag[v14] & 0x20) != 0)
            this->mElement[v8].mScaleStep = (float)(sub_140517000(this->mScaleTrack[v14][0], this->mScaleTrack[v14][1])
                                                   - this->mElement[v8].mScale)
                                           / v15;
          else
            this->mElement[v8].mScaleStep = 0.0f;
          if ((this->mTrackFlag[v14] & 8) != 0)
            this->mElement[v8].mZRotStep = sub_140517000(this->mZRotTrack[v14][0], this->mZRotTrack[v14][1]) / v15;
          else
            this->mElement[v8].mZRotStep = 0.0f;
          if ((this->mTrackFlag[v14] & 4) != 0)
            this->mElement[v8].mYRotStep = sub_140517000(this->mYRotTrack[v14][0], this->mYRotTrack[v14][1]) / v15;
          else
            this->mElement[v8].mYRotStep = 0.0f;
        }
        ++this->mElement[v8].mNowTrack;
        _PARTICLE_ELEMENT *v28 = this->mElement;
        mTrackCnt = this->mTrackCnt;
        if (mTrackCnt > v28[v8].mNowTrack)
        {
          mTimeSpeed = this->mTimeSpeed;
          v9 = v28[v8].mNowTrack;
          v10 = &v28[v8];
          if ((float)(mTimeSpeed * v28[v8].mTime) >= this->mTimeTrack[v9])
            continue;
        }
        return;
      }
      if (!v12)
      {
        this->mElement[v8].mDirStep[0] = 0.0f;
        this->mElement[v8].mDirStep[1] = 0.0f;
        this->mElement[v8].mDirStep[2] = 0.0f;
        this->mElement[v8].mARGBStep[0] = 0.0f;
        this->mElement[v8].mARGBStep[1] = 0.0f;
        this->mElement[v8].mARGBStep[2] = 0.0f;
        this->mElement[v8].mARGBStep[3] = 0.0f;
        this->mElement[v8].mScaleStep = 0.0f;
      }
    }
  }
}

void CParticle::SetParticleState(int a2)
{
  this->mState = a2;
  int v2 = a2 - 1;
  if (v2)
  {
    int v4 = v2 - 1;
    if (v4)
    {
      if (v4 == 1)
      {
        for (int i = 0; i < this->mNum; ++i)
        {
          DWORD TickCount = GetTickCount();
          float v7 = (float)(Noise1(static_cast<int>(TickCount)) - 0.5f) * this->mOnePerTimeEpsilon;
          this->mOnePerTimeEpsilonTemp = v7;
          this->InitElement(i, (float)(v7 + this->mOnePerTime) * (float)i);
        }
      }
    }
    else
    {
      float v8 = FLOAT_N1_0;
      int v9 = 0;
      int v10 = -1;
      if (this->mNum > 0)
      {
        float *p_mTime = &this->mElement->mTime;
        while (*reinterpret_cast<unsigned __int8 *>(reinterpret_cast<char *>(p_mTime) + 61))
        {
          if (*p_mTime > v8)
          {
            v8 = *p_mTime;
            v10 = v9;
          }
          ++v9;
          p_mTime += 26;
          if (v9 >= this->mNum)
            goto LABEL_15;
        }
        this->InitElement(v9, 0.0f);
LABEL_15:
        if (v10 >= 0)
          this->InitElement(v10, 0.0f);
      }
    }
  }
  else
  {
    int v12 = 0;
    if (this->mNum > 0)
    {
      __int64 v13 = 0;
      do
      {
        this->InitElement(v12++, 0.0f);
        this->mElement[v13++].mIsLive = 0;
      } while (v12 < this->mNum);
    }
  }
}

void CParticle::CheckCollision(int a2, float a3)
{
  unsigned int mFlag = this->mFlag;
  __int64 v4 = a2;
  if ((mFlag & 0x10) != 0)
  {
    CBsp *mBsp = reinterpret_cast<CBsp *>(this->mBsp);
    if (mBsp)
    {
      _PARTICLE_ELEMENT *mElement = this->mElement;
      unsigned __int64 v9 = static_cast<unsigned __int64>(a2);
      float v10 = (float)(a3 * mElement[v9].mDir[0]) + mElement[v9].mPos[0];
      float v43 = v10;
      float v11 = (float)(a3 * mElement[v9].mDir[1]) + mElement[v9].mPos[1];
      float v44 = v11;
      float v12 = (float)(a3 * mElement[v9].mDir[2]) + mElement[v9].mPos[2];
      float v45 = v12;
      float v40 = 0.0f;
      float v41 = 0.0f;
      float v42 = 0.0f;
      if ((mFlag & 0x8000) != 0)
      {
        v40 = mElement[v9].mPos[0];
        v41 = mElement[v9].mPos[1];
        v42 = mElement[v9].mPos[2];
      }
      else
      {
        float v13 = this->mCreatePos[0];
        v43 = v10 + v13;
        float v14 = this->mCreatePos[1];
        v44 = v11 + v14;
        float v15 = this->mCreatePos[2];
        v45 = v12 + v15;
        v40 = v13 + mElement[v9].mPos[0];
        v41 = v14 + mElement[v9].mPos[1];
        v42 = v15 + mElement[v9].mPos[2];
      }
      float v39[4]{};
      float v47[4]{};
      if (mBsp->IsCollisionFace(&v40, &v43, reinterpret_cast<float (*)[3]>(v39), reinterpret_cast<float (*)[4]>(v47)))
      {
        float v32 = 0.0f;
        float v33 = 0.0f;
        float v34 = 0.0f;
        sub_140518700(v47, this->mElement[v9].mDir, &v32);
        float v16 = v32;
        float v17 = v33;
        float v18 = v34;
        float v35 = 0.0f;
        float v36 = 0.0f;
        float v37 = 0.0f;
        float v19 = 0.0f;
        float v20 = 0.0f;
        float v21 = 0.0f;
        float v38 = 0.0f;
        if (v32 == 0.0f || v33 == 0.0f || v34 == 0.0f)
        {
          v35 = FLOAT_1_0;
          v36 = 0.0f;
          v37 = 0.0f;
          v19 = FLOAT_1_0;
          v20 = 0.0f;
          v21 = 0.0f;
        }
        else
        {
          sub_140518700(v47, &v32, &v35);
          float v22 = sqrtf_0((float)((float)(v17 * v17) + (float)(v16 * v16)) + (float)(v18 * v18));
          v33 = v17 / v22;
          float v23 = v36;
          v34 = v18 / v22;
          float v24 = v35;
          v32 = v16 / v22;
          float v25 = v37;
          float v26 = sqrtf_0((float)((float)(v23 * v23) + (float)(v24 * v24)) + (float)(v25 * v25));
          v19 = v24 / v26;
          v20 = v23 / v26;
          v21 = v25 / v26;
          v35 = v19;
          v36 = v20;
          v37 = v21;
        }
        float v46[16]{};
        v46[15] = FLOAT_1_0;
        v46[11] = 0.0f;
        v46[7] = 0.0f;
        v46[3] = 0.0f;
        v46[0] = 1.0f - (float)((float)(v19 * 2.0f) * v19);
        v46[1] = 0.0f - (float)((float)(v19 * 2.0f) * v20);
        v46[2] = 0.0f - (float)((float)(v19 * 2.0f) * v21);
        v46[4] = 0.0f - (float)((float)(v20 * 2.0f) * v19);
        v46[5] = 1.0f - (float)((float)(v20 * 2.0f) * v20);
        v46[10] = 1.0f - (float)((float)(v21 * 2.0f) * v21);
        v46[6] = 0.0f - (float)((float)(v20 * 2.0f) * v21);
        v46[8] = 0.0f - (float)((float)(v21 * 2.0f) * v19);
        v46[9] = 0.0f - (float)((float)(v21 * 2.0f) * v20);
        v46[12] = 0.0f - (float)((float)(v38 * 2.0f) * v19);
        v46[13] = 0.0f - (float)((float)(v38 * 2.0f) * v20);
        v46[14] = 0.0f - (float)((float)(v38 * 2.0f) * v21);
        v32 = this->mElement[v9].mDir[0];
        v33 = this->mElement[v9].mDir[1];
        v34 = this->mElement[v9].mDir[2];
        Vector3fTransform(&v35, &v32, reinterpret_cast<float (*)[4]>(v46));
        this->mElement[v9].mDir[0] = -0.0f - (float)(v35 * this->mElasticity);
        this->mElement[v9].mDir[1] = -0.0f - (float)(v36 * this->mElasticity);
        this->mElement[v9].mDir[2] = -0.0f - (float)(v37 * this->mElasticity);
        this->mElement[v9].mDirStep[0] = 0.0f;
        this->mElement[v9].mDirStep[1] = 0.0f;
        this->mElement[v9].mDirStep[2] = 0.0f;
        if (_bittest(reinterpret_cast<const LONG *>(&this->mFlag), 0xFu))
        {
          this->mElement[v9].mPos[0] = v39[0];
        }
        else
        {
          _PARTICLE_ELEMENT *v28 = this->mElement;
          float v29 = 0.0f - this->mCreatePos[0];
          float v30 = 0.0f - this->mCreatePos[2];
          v39[1] = v39[1] + (float)(0.0f - this->mCreatePos[1]);
          v39[2] = v39[2] + v30;
          v28[v9].mPos[0] = v39[0] + v29;
        }
        this->mElement[v9].mPos[1] = v39[1];
        this->mElement[v9].mPos[2] = v39[2];
      }
    }
  }
  this->mElement[v4].UpDate(a3);
  __int64 v31 = 40LL * v4;
  *(float *)((char *)this->mElement->mDir + v31) = (float)(a3 * this->mGravity[0]) + *(float *)((char *)this->mElement->mDir + v31);
  *(float *)((char *)&this->mElement->mDir[1] + v31) = (float)(a3 * this->mGravity[1]) + *(float *)((char *)&this->mElement->mDir[1] + v31);
  *(float *)((char *)&this->mElement->mDir[2] + v31) = (float)(a3 * this->mGravity[2]) + *(float *)((char *)&this->mElement->mDir[2] + v31);
}
