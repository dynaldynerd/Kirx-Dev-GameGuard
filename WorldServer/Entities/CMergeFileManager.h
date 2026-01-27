#pragma once

#include "IdaCompat.h"

struct CMergeFile;

/* 1402 */
class __cppobj CMergeFileManager
{
  char mPath[256];
  unsigned int mPathNameLeng;
  unsigned int mMergeFileNum;
  CMergeFile *mMergeFile;
};

