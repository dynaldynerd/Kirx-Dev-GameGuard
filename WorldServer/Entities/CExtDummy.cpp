#include "pch.h"

#include "CExtDummy.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "R3EngineGlobals.h"
#include "WorldServerUtil.h"

CExtDummy::CExtDummy()
  : mMaxNum(0),
    mNum(0),
    mDummy(nullptr)
{
}

CExtDummy::~CExtDummy()
{
  if (mDummy)
  {
    Dfree(mDummy);
  }
  mMaxNum = 0;
  mNum = 0;
  mDummy = nullptr;
}

bool CExtDummy::LoadExtDummy(const char *szFileName)
{
  FILE *stream = fopen(szFileName, "rt");
  if (!stream)
  {
    mNum = 0;
    return false;
  }

  mMaxNum = 0;
  mNum = 0;
  mDummy = nullptr;

  mMaxNum = 16;
  mDummy = static_cast<_EXT_DUMMY *>(Dmalloc(sizeof(_EXT_DUMMY) * mMaxNum));
  if (!mDummy)
  {
    fclose(stream);
    return false;
  }

  char token[256]{};
  if (fscanf(stream, "%s", token) == -1)
  {
    fclose(stream);
    return false;
  }

  while (strcmp(token, "script_begin") != 0)
  {
    if (fscanf(stream, "%s", token) == -1)
    {
      fclose(stream);
      return false;
    }
  }

  mNum = 0;
  if (fscanf(stream, "%s", token) != -1)
  {
    while (true)
    {
      _strlwr(token);
      if (!strcmp(token, "script_end"))
      {
        break;
      }

      if (token[0] == '*')
      {
        memset_0(&mDummy[mNum], 0, sizeof(mDummy[mNum]));
        strncpy(mDummy[mNum].mName, token, 0x1Fu);
        MatrixIdentity(mDummy[mNum].mMat);

        for (int i = 0; i < 3; ++i)
        {
          fscanf(stream, "%s", token);
          mDummy[mNum].mBBmin[i] = static_cast<float>(std::atof(token));
        }
        for (int i = 0; i < 3; ++i)
        {
          fscanf(stream, "%s", token);
          mDummy[mNum].mBBmax[i] = static_cast<float>(std::atof(token));
        }

        if (fscanf(stream, "%s", token) != -1)
        {
          do
          {
            if (!strcmp(token, "script_end"))
            {
              break;
            }
            if (!strcmp(token, "-node_tm"))
            {
              float *mat = &mDummy[mNum].mMat[0][0];
              for (int i = 0; i < 16; ++i)
              {
                fscanf(stream, "%s", token);
                mat[i] = static_cast<float>(std::atof(token));
              }
              MatrixInvert(mDummy[mNum].mInvMat, mDummy[mNum].mMat);
            }
            if (!strcmp(token, "-id"))
            {
              fscanf(stream, "%s", token);
              mDummy[mNum].mID = std::atoi(token);
            }
            if (!strcmp(token, "-music"))
            {
              mDummy[mNum].mFlag |= 1u;
            }
            if (!strcmp(token, "-interior"))
            {
              mDummy[mNum].mFlag |= 4u;
            }
            if (!strcmp(token, "-fog_color"))
            {
              mDummy[mNum].mFlag |= 2u;
              fscanf(stream, "%s", token);
              const int red = std::atoi(token);
              fscanf(stream, "%s", token);
              const int green = std::atoi(token);
              fscanf(stream, "%s", token);
              const int blue = std::atoi(token);
              mDummy[mNum].___u5.mdwArgv[0] =
                static_cast<unsigned int>(blue)
                | (static_cast<unsigned int>(green) << 8)
                | (static_cast<unsigned int>(red & 0xFF) << 16);
              mDummy[mNum].___u5.mdwArgv[1] = dword_184A79814;
              mDummy[mNum].___u5.mdwArgv[2] = static_cast<unsigned int>(qword_184A79818);
            }
            if (!strcmp(token, "-fog_start"))
            {
              fscanf(stream, "%s", token);
              const float fogStart = static_cast<float>(std::atof(token));
              mDummy[mNum].___u5.mdwArgv[1] = *reinterpret_cast<const unsigned int *>(&fogStart);
            }
            if (!strcmp(token, "-fog_end"))
            {
              fscanf(stream, "%s", token);
              const float fogEnd = static_cast<float>(std::atof(token));
              mDummy[mNum].___u5.mdwArgv[2] = *reinterpret_cast<const unsigned int *>(&fogEnd);
            }
          } while (token[0] != '*' && fscanf(stream, "%s", token) != -1);
        }

        if (++mNum >= mMaxNum)
        {
          _EXT_DUMMY *newDummy = static_cast<_EXT_DUMMY *>(
            ReAlloc(mDummy, sizeof(_EXT_DUMMY) * mMaxNum, sizeof(_EXT_DUMMY) * (mMaxNum + 32)));
          const unsigned int oldMax = mMaxNum;
          mDummy = newDummy;
          memset_0(&newDummy[oldMax], 0, sizeof(_EXT_DUMMY) * 32);
          mMaxNum += 32;
        }
      }
      else if (fscanf(stream, "%s", token) == -1)
      {
        break;
      }
    }
  }

  fclose(stream);
  return true;
}

bool CExtDummy::GetWorldFromLocal(float *fOutPos, unsigned int nDummyIndex, float *v7)
{
  if (!mDummy)
    return false;
  if (!mNum || nDummyIndex >= mNum)
    return false;

  Vector3fTransform(fOutPos, v7, mDummy[nDummyIndex].mMat);
  return true;
}

bool CExtDummy::IsInBBox(unsigned int nDummyIndex, float *const pos)
{
  if (!mDummy)
    return false;
  if (!mNum || nDummyIndex >= mNum)
    return false;

  float localPos[3]{};
  Vector3fTransform(localPos, pos, mDummy[nDummyIndex].mInvMat);
  return IsCollisionBBoxPoint(mDummy[nDummyIndex].mBBmin, mDummy[nDummyIndex].mBBmax, localPos) != 0;
}
