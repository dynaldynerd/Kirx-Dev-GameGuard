#pragma once

#include "IdaCompat.h"

struct _MERGE_FILE;

/* 1401 */
class CMergeFile
{
public:
  char mFileName[256];
  _MERGE_FILE *mMergeFNF;
  unsigned int *mOffset;
  unsigned int *mIndex;
  unsigned int mCnt;
  unsigned int mHeaderSize;

public:
  CMergeFile();
  __int64 LoadMergeFileHeader(char *a2);
  void ReleaseMergeFileHeader();
  struct _iobuf *LoadFileOffset(char *a2, const char *a3);
  unsigned int GetFileOffset(unsigned int a2, char *a3);
  unsigned int GetFileSize(unsigned int a2, char *a3);
  __int64 IsExistFile(char *a2);
};

