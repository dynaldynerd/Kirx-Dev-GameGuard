#pragma once

#include "IdaCompat.h"

struct _EXT_DUMMY;

/* 1415 */
class __cppobj CExtDummy
{
public:
  unsigned int mMaxNum;
  unsigned int mNum;
  _EXT_DUMMY *mDummy;

  CExtDummy();
  ~CExtDummy();
  bool LoadExtDummy(const char *szFileName);
  _EXT_DUMMY *GetDummy(unsigned int nDummyIndex);
  void GetDummyList(int nFlag, unsigned int *pnNum, unsigned int *pList);
  bool GetWorldFromLocal(float *fOutPos, unsigned int nDummyIndex, float *v7);
  bool IsInBBox(unsigned int nDummyIndex, float *const pos);
};

