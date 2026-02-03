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

public:
  CMergeFileManager();
  void InitMergeFile(char *a2);
  void ReleaseMergeFile();
  unsigned int GetFileSize(char *a2);
  __int64 IsExistFile(char *a2);
  struct _iobuf *LoadFileOffset(char *a2, const char *a3);
};

