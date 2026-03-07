#include "pch.h"

#include "CAlpha.h"
#include "CBsp.h"
#include "WorldServerUtil.h"

namespace
{
void ByteSortForShort(unsigned int count, unsigned int *srcValue, __int16 *srcKey, unsigned int *tmpValue, __int16 *tmpKey)
{
  unsigned __int16 sortCount[260] = {};

  memset_0(sortCount, 0, sizeof(sortCount));
  for (unsigned int i = 0; i < count; ++i)
  {
    const unsigned int lowByte = static_cast<unsigned __int8>(srcKey[i]);
    ++sortCount[lowByte + 1];
  }

  for (int i = 1; i <= 256; ++i)
  {
    sortCount[i] = static_cast<unsigned __int16>(sortCount[i] + sortCount[i - 1]);
  }

  for (unsigned int i = 0; i < count; ++i)
  {
    const unsigned int lowByte = static_cast<unsigned __int8>(srcKey[i]);
    const unsigned __int16 dstIndex = sortCount[lowByte]++;
    tmpValue[dstIndex] = srcValue[i];
    tmpKey[dstIndex] = srcKey[i];
  }

  memset_0(sortCount, 0, sizeof(sortCount));
  for (unsigned int i = 0; i < count; ++i)
  {
    const unsigned int highByte = *(reinterpret_cast<unsigned __int8 *>(&srcKey[i]) + 1);
    ++sortCount[highByte + 1];
  }

  for (int i = 1; i <= 256; ++i)
  {
    sortCount[i] = static_cast<unsigned __int16>(sortCount[i] + sortCount[i - 1]);
  }

  for (unsigned int i = 0; i < count; ++i)
  {
    const unsigned int highByte = *(reinterpret_cast<unsigned __int8 *>(&tmpKey[i]) + 1);
    const unsigned __int16 dstIndex = sortCount[highByte]++;
    srcValue[dstIndex] = tmpValue[i];
  }
}
}

CAlpha::CAlpha()
{
  mAlphaFace = static_cast<unsigned int *>(Dmalloc(32768));
  mAlphaFaceZ = static_cast<__int16 *>(Dmalloc(16384));
  mTempAlphaFace = static_cast<unsigned int *>(Dmalloc(32768));
  mTempAlphaFaceZ = static_cast<__int16 *>(Dmalloc(16384));
  mAlphaFaceCnt = 0;
  mAlphaSize = 8192;
  mBsp = nullptr;
}

CAlpha::~CAlpha()
{
  Dfree(reinterpret_cast<_DWORD *>(mTempAlphaFaceZ));
  Dfree(mTempAlphaFace);
  Dfree(reinterpret_cast<_DWORD *>(mAlphaFaceZ));
  Dfree(mAlphaFace);
}

void CAlpha::InitAlpha(void *a2)
{
  mBsp = a2;
}

void CAlpha::CheckAlphaAlloc()
{
  const unsigned int alphaSize = mAlphaSize;
  if (mAlphaFaceCnt >= alphaSize)
  {
    const int grownSize = alphaSize + static_cast<int>(alphaSize) / 2;
    mAlphaFace = static_cast<unsigned int *>(ReAlloc(mAlphaFace, 4 * alphaSize, 4 * grownSize));
    mAlphaFaceZ = static_cast<__int16 *>(ReAlloc(mAlphaFaceZ, 2 * mAlphaSize, 2 * grownSize));
    mTempAlphaFace = static_cast<unsigned int *>(ReAlloc(mTempAlphaFace, 4 * mAlphaSize, 4 * grownSize));
    mTempAlphaFaceZ = static_cast<__int16 *>(ReAlloc(mTempAlphaFaceZ, 2 * mAlphaSize, 2 * grownSize));
    mAlphaSize = grownSize;
  }
}

unsigned int *CAlpha::GetAlphaFace()
{
  return mAlphaFace;
}

__int64 CAlpha::GetAlphaFaceCnt()
{
  return mAlphaFaceCnt;
}

void CAlpha::LoopInitAlphaStack()
{
  mAlphaFaceCnt = 0;
}

void CAlpha::SetAlphaEntityStack(unsigned __int16 faceIndex)
{
  mAlphaFace[mAlphaFaceCnt++] = faceIndex | 0x20000000;
  CheckAlphaAlloc();
}

void CAlpha::SetAlphaStack(unsigned __int16 faceIndex)
{
  mAlphaFace[mAlphaFaceCnt++] = faceIndex | 0x80000000;
  CheckAlphaAlloc();
}

void CAlpha::SetCoronaStack(unsigned __int16 faceIndex)
{
  mAlphaFace[mAlphaFaceCnt++] = faceIndex | 0x40000000;
  CheckAlphaAlloc();
}

unsigned int *CAlpha::SortAlphaStack(float *const cameraPos)
{
  CBsp *bsp = reinterpret_cast<CBsp *>(mBsp);

  for (unsigned int i = 0; i < mAlphaFaceCnt; ++i)
  {
    const unsigned int face = mAlphaFace[i];
    float point[3] = {};

    if ((face & 0x20000000) != 0)
    {
      const unsigned __int16 index = static_cast<unsigned __int16>(face);
      point[0] = bsp->mMapEntitiesList[index].Pos[0];
      point[1] = bsp->mMapEntitiesList[index].Pos[1];
      point[2] = bsp->mMapEntitiesList[index].Pos[2];
    }
    else
    {
      const unsigned __int16 index = static_cast<unsigned __int16>(face);
      point[0] = bsp->mMatGroup[index].Origin[0];
      point[1] = bsp->mMatGroup[index].Origin[1];
      point[2] = bsp->mMatGroup[index].Origin[2];
    }

    const float dx = point[0] - cameraPos[0];
    const float dy = point[1] - cameraPos[1];
    const float dz = point[2] - cameraPos[2];
    mAlphaFaceZ[i] = static_cast<__int16>(sqrtf_0((dx * dx) + (dy * dy) + (dz * dz)));
  }

  if (mAlphaFaceCnt == 0)
  {
    return nullptr;
  }

  ByteSortForShort(mAlphaFaceCnt, mAlphaFace, mAlphaFaceZ, mTempAlphaFace, mTempAlphaFaceZ);
  return mAlphaFace;
}
