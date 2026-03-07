#pragma once

#include "IdaCompat.h"

class CMergeFile;

/* 1402 */
class  CMergeFileManager
{
  char mPath[256];
  unsigned int mPathNameLeng;
  unsigned int mMergeFileNum;
  CMergeFile *mMergeFile;

public:
  CMergeFileManager();
  ~CMergeFileManager();
  void InitMergeFile(char *directoryPath);
  void ReleaseMergeFile();
  unsigned int GetFileSize(char *filePath);
  __int64 IsExistFile(char *filePath);
  struct _iobuf *LoadFileOffset(char *filePath, const char *mode);
};

