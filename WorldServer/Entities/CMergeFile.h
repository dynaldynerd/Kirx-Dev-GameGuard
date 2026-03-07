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
  __int64 LoadMergeFileHeader(char *filePath);
  void ReleaseMergeFileHeader();
  struct _iobuf *LoadFileOffset(char *filePath, const char *mode);
  unsigned int GetFileOffset(unsigned int parentIndex, char *filePath);
  unsigned int GetFileSize(unsigned int parentIndex, char *filePath);
  __int64 IsExistFile(char *filePath);
};

