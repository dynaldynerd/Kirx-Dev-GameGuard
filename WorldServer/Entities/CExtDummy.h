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
  bool LoadExtDummy(const char *szFileName);
  unsigned int GetTotalNum();
  _EXT_DUMMY *GetDummy(unsigned int nDummyIndex);
  bool GetWorldFromLocal(float *fOutPos, unsigned int nDummyIndex, float *v7);
  bool IsInBBox(unsigned int nDummyIndex, float *const pos);
};

