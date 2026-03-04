#pragma once

#include "IdaCompat.h"

struct _EXT_DUMMY;

/* 1415 */
class  CExtDummy
{
public:
  unsigned int mMaxNum;
  unsigned int mNum;
  _EXT_DUMMY *mDummy;

  CExtDummy();
  ~CExtDummy();
  void ReleaseExtDummy();
  bool LoadExtDummy(const char *szFileName);
  unsigned int GetTotalNum();
  _EXT_DUMMY *GetDummy(unsigned int nDummyIndex);
  void GetDummyList(int flagMask, unsigned int *outCount, unsigned int *outIndices);
  void DrawDummyBBox(unsigned int nDummyIndex);
  void DrawAllDummyBBox();
  bool GetLocalFromWorld(float *fOutPos, unsigned int nDummyIndex, float *v7);
  bool GetWorldFromLocal(float *fOutPos, unsigned int nDummyIndex, float *v7);
  bool IsInBBox(unsigned int nDummyIndex, float *const pos);
};

