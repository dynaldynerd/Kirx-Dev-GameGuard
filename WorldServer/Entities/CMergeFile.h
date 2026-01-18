#pragma once

#include "IdaCompat.h"

/* 1401 */
struct CMergeFile
{
  char mFileName[256];
  _MERGE_FILE *mMergeFNF;
  unsigned int *mOffset;
  unsigned int *mIndex;
  unsigned int mCnt;
  unsigned int mHeaderSize;
};
