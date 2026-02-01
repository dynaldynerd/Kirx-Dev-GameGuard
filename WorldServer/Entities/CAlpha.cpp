#include "pch.h"

#include "CAlpha.h"
#include "WorldServerUtil.h"

CAlpha::CAlpha()
{
  mAlphaFace = static_cast<unsigned int *>(Dmalloc(0x8000));
  mAlphaFaceZ = static_cast<__int16 *>(Dmalloc(0x4000));
  mTempAlphaFace = static_cast<unsigned int *>(Dmalloc(0x8000));
  mTempAlphaFaceZ = static_cast<__int16 *>(Dmalloc(0x4000));
  mAlphaFaceCnt = 0;
  mAlphaSize = 0x2000;
  mBsp = nullptr;
}

void CAlpha::InitAlpha(void *a2)
{
  mBsp = a2;
}
