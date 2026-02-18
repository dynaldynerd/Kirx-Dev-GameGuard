#pragma once

#include "IdaCompat.h"

/* 1421 */
class  CAlpha
{
  unsigned int mAlphaFaceCnt;
  unsigned int *mAlphaFace;
  __int16 *mAlphaFaceZ;
  unsigned int *mTempAlphaFace;
  __int16 *mTempAlphaFaceZ;
  int mAlphaSize;
  void *mBsp;

public:
  CAlpha();
  void InitAlpha(void *a2);
};

