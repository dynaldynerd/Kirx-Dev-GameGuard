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
  ~CAlpha();
  void CheckAlphaAlloc();
  unsigned int *GetAlphaFace();
  __int64 GetAlphaFaceCnt();
  void LoopInitAlphaStack();
  void SetAlphaEntityStack(unsigned __int16 faceIndex);
  void SetAlphaStack(unsigned __int16 faceIndex);
  void SetCoronaStack(unsigned __int16 faceIndex);
  unsigned int *SortAlphaStack(float *const cameraPos);
  void InitAlpha(void *a2);
};

