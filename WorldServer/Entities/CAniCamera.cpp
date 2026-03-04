#include "pch.h"

#include "CAniCamera.h"
#include "CMainThread.h"
#include "R3EngineGlobals.h"
#include "WorldServerUtil.h"

#include <cstdio>
#include <cstring>

namespace
{
float sAniCameraMatrix[16] = {};
int sAniExtKeyIndex0 = 0;
int sAniExtKeyIndex1 = 0;
float sAniCameraFov = 0.0f;

void SetAniCameraFov(float fov)
{
  sAniCameraFov = fov;
}

float SampleAniTrackLerp(int keyCount, float *track, int stride, double frameTime)
{
  const int lastIndex = keyCount - 1;
  int index = 0;

  if (lastIndex <= 0)
  {
    sAniExtKeyIndex0 = lastIndex;
    sAniExtKeyIndex1 = lastIndex;
    return 0.0f;
  }

  float *lhs = track;
  float *rhs = track + stride;
  while (static_cast<float>(frameTime) < *lhs || *rhs <= static_cast<float>(frameTime))
  {
    ++index;
    lhs += stride;
    rhs += stride;
    if (index >= lastIndex)
    {
      sAniExtKeyIndex0 = lastIndex;
      sAniExtKeyIndex1 = lastIndex;
      return 0.0f;
    }
  }

  sAniExtKeyIndex0 = index;
  sAniExtKeyIndex1 = index + 1;

  const float leftFrame = track[stride * index];
  return (static_cast<float>(frameTime) - leftFrame) / (track[stride * (index + 1)] - leftFrame);
}

void MakeRotationY180(float (*const matrix)[4])
{
  MatrixIdentity(matrix);
  matrix[0][0] = -1.0f;
  matrix[2][2] = -1.0f;
}
}

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

__int64 CAniCamera::GetCameraIndex(char *name)
{
  char lowerName[256] = {};
  const ptrdiff_t offset = lowerName - name;

  do
  {
    const char value = *name++;
    name[offset - 1] = value;
  } while (name[offset - 1]);

  _strlwr(lowerName);

  if (!mAniCameraNum)
  {
    return 0xFFFFFFFFLL;
  }

  unsigned int index = 0;
  while (strcmp(lowerName, AniCamera[index].mName))
  {
    ++index;
    if (index >= mAniCameraNum)
    {
      return 0xFFFFFFFFLL;
    }
  }

  return index;
}

_ANI_CAMERA *CAniCamera::GetCameraName(unsigned int index)
{
  if (mAniCameraNum > index)
  {
    return &AniCamera[index];
  }
  return nullptr;
}

__int64 CAniCamera::GetDummyID(char *name, int compareLoweredInput)
{
  unsigned int index = 0;

  if (compareLoweredInput)
  {
    if (mDummyNum)
    {
      char lowered[256] = {};
      const ptrdiff_t offset = lowered - name;
      while (true)
      {
        char *cursor = name;
        do
        {
          const char value = *cursor++;
          cursor[offset - 1] = value;
        } while (cursor[offset - 1]);

        _strlwr(lowered);
        if (!strcmp(name, lowered))
        {
          break;
        }

        if (++index >= mDummyNum)
        {
          return 0xFFFFFFFFLL;
        }
      }
      return index;
    }
  }
  else
  {
    const unsigned int dummyCount = mDummyNum;
    if (dummyCount)
    {
      while (strcmp(name, mDummy[index].ObjectName))
      {
        if (++index >= dummyCount)
        {
          return 0xFFFFFFFFLL;
        }
      }
      return index;
    }
  }

  return 0xFFFFFFFFLL;
}

void CAniCamera::GetDummyMatrixSub(float (*const matrix)[4], unsigned int index, double frameTime)
{
  const unsigned int parentId =
    static_cast<unsigned int>(GetDummyID(mDummy[index].ParentName, 0));
  if (parentId != 0xFFFFFFFFu)
  {
    GetDummyMatrixSub(matrix, parentId, static_cast<float>(frameTime));
  }

  float localMatrix[16] = {};
  float composed[16] = {};
  GetAniMatrix(reinterpret_cast<float (*)[4]>(localMatrix), &mDummy[index], frameTime);
  MatrixMultiply(reinterpret_cast<float (*)[4]>(composed), matrix, reinterpret_cast<float (*)[4]>(localMatrix));
  MatrixCopy(matrix, reinterpret_cast<float (*)[4]>(composed));
}

void CAniCamera::GetDummyMatrix(float (*const matrix)[4], unsigned int index, double elapsedSeconds)
{
  if (mDummyNum && index < mDummyNum)
  {
    const int startFrame = mStartFrame;
    float frame = static_cast<float>(elapsedSeconds) * 30.0f;
    const int loopSpan = mEndFrame - startFrame;
    if ((static_cast<float>(elapsedSeconds) * 30.0f) > static_cast<float>(loopSpan))
    {
      frame = (frame - static_cast<float>(static_cast<int>(frame))) + static_cast<float>(static_cast<int>(frame) % loopSpan);
    }

    frame = frame - static_cast<float>(startFrame);
    MatrixIdentity(matrix);
    GetDummyMatrixSub(matrix, index, frame);
    GetMatrixFrom3DSMAXMatrix(matrix);
  }
  else
  {
    MatrixIdentity(matrix);
  }
}

char *CAniCamera::GetDummyParentID(unsigned int index)
{
  if (index < mDummyNum)
  {
    return mDummy[index].ParentName;
  }
  return nullptr;
}

float *CAniCamera::GetMatrixAniCamera(double elapsedSeconds)
{
  _ANI_CAMERA *camera = &AniCamera[mPlayIndex];

  if (mAniCameraNum)
  {
    if (!mIsSetPerspect)
    {
      mIsSetPerspect = 1;
      SetAniCameraFov(camera->fov * 0.78799999f);
    }

    const int startFrame = mStartFrame;
    float frame = static_cast<float>(elapsedSeconds) * 30.0f;
    const int loopSpan = mEndFrame - startFrame;
    if ((static_cast<float>(elapsedSeconds) * 30.0f) > static_cast<float>(loopSpan))
    {
      frame = (frame - static_cast<float>(static_cast<int>(frame))) + static_cast<float>(static_cast<int>(frame) % loopSpan);
    }
    frame = frame - static_cast<float>(startFrame);

    float world[16] = {};
    float composed[16] = {};
    float local[16] = {};
    MatrixIdentity(reinterpret_cast<float (*)[4]>(world));

    for (unsigned int i = 0; i < camera->h_num; ++i)
    {
      GetAniMatrix(reinterpret_cast<float (*)[4]>(local), &camera->obj[i], frame);
      MatrixMultiply(reinterpret_cast<float (*)[4]>(composed), reinterpret_cast<float (*)[4]>(world), reinterpret_cast<float (*)[4]>(local));
      MatrixCopy(reinterpret_cast<float (*)[4]>(world), reinterpret_cast<float (*)[4]>(composed));
    }

    GetMatrixFrom3DSMAXMatrix(reinterpret_cast<float (*)[4]>(world));
    MatrixInvert(reinterpret_cast<float (*)[4]>(sAniCameraMatrix), reinterpret_cast<float (*)[4]>(world));

    float rotY[16] = {};
    MakeRotationY180(reinterpret_cast<float (*)[4]>(rotY));
    MatrixMultiply(reinterpret_cast<float (*)[4]>(world), reinterpret_cast<float (*)[4]>(rotY), reinterpret_cast<float (*)[4]>(sAniCameraMatrix));
    MatrixCopy(reinterpret_cast<float (*)[4]>(sAniCameraMatrix), reinterpret_cast<float (*)[4]>(world));

    sAniCameraMatrix[0] = 0.0f - sAniCameraMatrix[0];
    sAniCameraMatrix[4] = 0.0f - sAniCameraMatrix[4];
    sAniCameraMatrix[8] = 0.0f - sAniCameraMatrix[8];
    sAniCameraMatrix[12] = 0.0f - sAniCameraMatrix[12];
  }
  else
  {
    MatrixIdentity(reinterpret_cast<float (*)[4]>(sAniCameraMatrix));
  }

  return sAniCameraMatrix;
}

bool CAniCamera::IsLoadedAniCamera()
{
  return mAniCameraNum != 0;
}

void CAniCamera::LoadAniCamera(char *fileName)
{
  FILE *stream = fopen(fileName, "rb");
  if (!stream)
  {
    return;
  }

  if (mAniCameraNum)
  {
    ReleaseAniCamera();
  }

  float version = 0.0f;
  fread(&version, 4uLL, 1uLL, stream);
  if (version >= 1.2f)
  {
    fread(&mAniCameraNum, 4uLL, 1uLL, stream);
    if (mAniCameraNum)
    {
      fread(&mStartFrame, 4uLL, 1uLL, stream);
      fread(&mEndFrame, 4uLL, 1uLL, stream);

      AniCamera = static_cast<_ANI_CAMERA *>(Dmalloc(96 * mAniCameraNum));
      for (unsigned int cameraIndex = 0; cameraIndex < mAniCameraNum; ++cameraIndex)
      {
        fread(&AniCamera[cameraIndex], 0x40uLL, 1uLL, stream);
        fread(&AniCamera[cameraIndex].fov, 4uLL, 1uLL, stream);
        fread(&AniCamera[cameraIndex].tdist, 4uLL, 1uLL, stream);
        fread(&AniCamera[cameraIndex].h_num, 4uLL, 1uLL, stream);

        if (version < 1.3f)
        {
          AniCamera[cameraIndex].ext_num = 0;
        }
        else
        {
          fread(&AniCamera[cameraIndex].ext_num, 4uLL, 1uLL, stream);
        }

        AniCamera[cameraIndex].obj = static_cast<_ANI_OBJECT *>(
          Dmalloc(sizeof(_ANI_OBJECT) * AniCamera[cameraIndex].h_num + 20 * AniCamera[cameraIndex].ext_num));
        AniCamera[cameraIndex].ext = reinterpret_cast<_ANI_CAMERA_EXT *>(&AniCamera[cameraIndex].obj[AniCamera[cameraIndex].h_num]);

        if (version < 1.3f)
        {
          AniCamera[cameraIndex].ext = nullptr;
        }
        else
        {
          fread(AniCamera[cameraIndex].ext, 20LL * AniCamera[cameraIndex].ext_num, 1uLL, stream);
        }

        for (unsigned int nodeIndex = 0; nodeIndex < AniCamera[cameraIndex].h_num; ++nodeIndex)
        {
          fread(AniCamera[cameraIndex].obj[nodeIndex].s_matrix, 0x40uLL, 1uLL, stream);
          fread(AniCamera[cameraIndex].obj[nodeIndex].pos, 0xCuLL, 1uLL, stream);
          fread(AniCamera[cameraIndex].obj[nodeIndex].quat, 0x10uLL, 1uLL, stream);
          fread(&AniCamera[cameraIndex].obj[nodeIndex].Pos_cnt, 4uLL, 1uLL, stream);
          fread(&AniCamera[cameraIndex].obj[nodeIndex].Rot_cnt, 4uLL, 1uLL, stream);

          AniCamera[cameraIndex].obj[nodeIndex].Pos = static_cast<_POS_TRACK *>(
            Dmalloc(16 * AniCamera[cameraIndex].obj[nodeIndex].Pos_cnt + 20 * AniCamera[cameraIndex].obj[nodeIndex].Rot_cnt));
          AniCamera[cameraIndex].obj[nodeIndex].Rot = reinterpret_cast<_ROT_TRACK *>(
            &AniCamera[cameraIndex].obj[nodeIndex].Pos[AniCamera[cameraIndex].obj[nodeIndex].Pos_cnt]);

          fread(AniCamera[cameraIndex].obj[nodeIndex].Pos, 16LL * AniCamera[cameraIndex].obj[nodeIndex].Pos_cnt, 1uLL, stream);
          fread(AniCamera[cameraIndex].obj[nodeIndex].Rot, 20LL * AniCamera[cameraIndex].obj[nodeIndex].Rot_cnt, 1uLL, stream);

          AniCamera[cameraIndex].obj[nodeIndex].Scale = nullptr;
          AniCamera[cameraIndex].obj[nodeIndex].Scale_cnt = 0;
        }
      }

      fread(&mDummyNum, 4uLL, 1uLL, stream);
      if (mDummyNum)
      {
        mDummy = static_cast<_ANI_OBJECT *>(Dmalloc(sizeof(_ANI_OBJECT) * mDummyNum));
      }

      for (unsigned int dummyIndex = 0; dummyIndex < mDummyNum; ++dummyIndex)
      {
        fread(&mDummy[dummyIndex], 0x40uLL, 1uLL, stream);
        fread(mDummy[dummyIndex].ParentName, 0x40uLL, 1uLL, stream);
        fread(mDummy[dummyIndex].s_matrix, 0x40uLL, 1uLL, stream);
        fread(mDummy[dummyIndex].pos, 0xCuLL, 1uLL, stream);
        fread(mDummy[dummyIndex].quat, 0x10uLL, 1uLL, stream);
        fread(&mDummy[dummyIndex].Pos_cnt, 4uLL, 1uLL, stream);
        fread(&mDummy[dummyIndex].Rot_cnt, 4uLL, 1uLL, stream);

        mDummy[dummyIndex].Pos = static_cast<_POS_TRACK *>(
          Dmalloc(16 * mDummy[dummyIndex].Pos_cnt + 20 * mDummy[dummyIndex].Rot_cnt));
        mDummy[dummyIndex].Rot = reinterpret_cast<_ROT_TRACK *>(&mDummy[dummyIndex].Pos[mDummy[dummyIndex].Pos_cnt]);

        fread(mDummy[dummyIndex].Pos, 16LL * mDummy[dummyIndex].Pos_cnt, 1uLL, stream);
        fread(mDummy[dummyIndex].Rot, 20LL * mDummy[dummyIndex].Rot_cnt, 1uLL, stream);

        mDummy[dummyIndex].Scale = nullptr;
        mDummy[dummyIndex].Scale_cnt = 0;
      }
    }

    fclose(stream);
  }
  else
  {
    Error(fileName, byte_140883769);
  }
}

__int64 CAniCamera::PlayAniCamera(float (*const matrix)[4], double speed, int useCurrentFrame)
{
  unsigned int isFinished = 1;

  if (mAniCameraNum)
  {
    float frame = 0.0f;
    if (useCurrentFrame)
    {
      frame = mNowFrame;
    }
    else
    {
      const float delta = (R3GetTime() - mStartTick) * static_cast<float>(speed);
      if (mPlayEndFrame < static_cast<int>(mPlayStartFrame))
      {
        frame = (static_cast<float>(mPlayStartFrame) / 30.0f) - delta;
      }
      else
      {
        frame = (static_cast<float>(mPlayStartFrame) / 30.0f) + delta;
      }
    }

    if (mPlayEndFrame < static_cast<int>(mPlayStartFrame))
    {
      if (frame <= 0.0f)
      {
        if (!mFlag)
        {
          frame = static_cast<float>(mPlayEndFrame) / 30.0f;
          mStartTick = R3GetTime();
          isFinished = 0;
        }
        else
        {
          frame = 0.0f;
        }
      }
    }
    else
    {
      const float endFrameSeconds = static_cast<float>(mPlayEndFrame) / 30.0f;
      if (frame >= endFrameSeconds)
      {
        if (!mFlag)
        {
          frame = static_cast<float>(static_cast<int>(mPlayStartFrame));
          mStartTick = R3GetTime();
          isFinished = 0;
        }
        else
        {
          frame = endFrameSeconds;
        }
      }
    }

    const float oldFrame = mNowFrame;
    mNowFrame = frame;
    mOldFrame = oldFrame;

    GetMatrixAniCamera(frame);
    MatrixCopy(matrix, reinterpret_cast<float (*)[4]>(sAniCameraMatrix));
  }

  return isFinished;
}

void CAniCamera::ReleaseAniCamera()
{
  if (mAniCameraNum)
  {
    for (unsigned int cameraIndex = 0; cameraIndex < mAniCameraNum; ++cameraIndex)
    {
      for (unsigned int nodeIndex = 0; nodeIndex < AniCamera[cameraIndex].h_num; ++nodeIndex)
      {
        _ANI_OBJECT *obj = AniCamera[cameraIndex].obj;
        if (obj[nodeIndex].Pos_cnt + obj[nodeIndex].Rot_cnt)
        {
          Dfree(reinterpret_cast<_DWORD *>(&obj[nodeIndex].Pos->frame));
        }
      }

      Dfree(AniCamera[cameraIndex].obj->ObjectName);
    }

    Dfree(AniCamera->mName);

    if (mDummyNum)
    {
      for (unsigned int dummyIndex = 0; dummyIndex < mDummyNum; ++dummyIndex)
      {
        if (mDummy[dummyIndex].Pos_cnt + mDummy[dummyIndex].Rot_cnt)
        {
          Dfree(reinterpret_cast<_DWORD *>(&mDummy[dummyIndex].Pos->frame));
        }
      }
    }

    if (mDummy)
    {
      Dfree(mDummy);
    }

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
}

__int64 CAniCamera::SetExtCamAni()
{
  _ANI_CAMERA *camera = &AniCamera[mPlayIndex];
  const int extCount = static_cast<int>(camera->ext_num);

  if (extCount)
  {
    const float lerp = SampleAniTrackLerp(extCount, &camera->ext->frame, 5, mNowFrame * 30.0f);
    const float value0 = *(reinterpret_cast<float *>(reinterpret_cast<char *>(camera->ext) + 20LL * sAniExtKeyIndex0 + 4));
    const float value1 = *(reinterpret_cast<float *>(reinterpret_cast<char *>(camera->ext) + 20LL * sAniExtKeyIndex1 + 4));
    SetAniCameraFov(((value1 - value0) * lerp + value0) * 0.78799999f);
    return 1;
  }

  SetAniCameraFov(camera->fov * 0.78799999f);
  return 0;
}

void CAniCamera::SetNowFrame(float nowFrame)
{
  mStartTick = (R3GetTime() - nowFrame) - (static_cast<float>(static_cast<int>(mPlayStartFrame)) / 30.0f);
}

void CAniCamera::SetPrePlayCamera(
  unsigned int cameraIndex,
  unsigned int startFrame,
  unsigned int endFrame,
  unsigned int loopFlag)
{
  const unsigned int cameraCount = mAniCameraNum;
  if (cameraCount)
  {
    if (startFrame == 0xFFFFFFFFu && endFrame == 0xFFFFFFFFu)
    {
      startFrame = mStartFrame;
      endFrame = mEndFrame;
    }

    if (startFrame < mStartFrame)
    {
      startFrame = mStartFrame;
    }

    if (endFrame < mStartFrame)
    {
      endFrame = mStartFrame;
    }

    if (startFrame > mEndFrame)
    {
      startFrame = mEndFrame;
    }

    if (endFrame > mEndFrame)
    {
      endFrame = mEndFrame;
    }

    if (cameraIndex >= cameraCount)
    {
      cameraIndex = cameraCount - 1;
    }

    mPlayStartFrame = startFrame;
    mPlayEndFrame = endFrame;

    if (cameraCount <= cameraIndex)
    {
      cameraIndex = cameraCount - 1;
    }

    mPlayIndex = static_cast<int>(cameraIndex);
    mIsSetPerspect = 0;
    mStartTick = R3GetTime();
    mFlag = loopFlag;
  }
  else
  {
    Warning(const_cast<char *>("A"), byte_140883769);
  }
}

