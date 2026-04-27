#ifndef _MERGEFILE_H_
#define _MERGEFILE_H_

#include <windows.h>
#include <stdio.h>

typedef struct
{
  char name[52];
  int file_length;
  WORD name_cnt;
  WORD cnt;
  DWORD start_index;
} _MERGE_FILE;

class CMergeFile
{
public:
  char mFileName[256];
  int mCnt;
  int mHeaderSize;
  unsigned int *mIndex;
  unsigned int *mOffset;
  _MERGE_FILE *mMergeFNF;

  CMergeFile(void);

  BOOL LoadMergeFileHeader(char *pi_pFileName);
  void ReleaseMergeFileHeader(void);
  unsigned int GetFileOffset(unsigned int pi_dwNodeIndex, char *pi_pFileName);
  unsigned int GetFileSize(unsigned int pi_dwNodeIndex, char *pi_pFileName);
  FILE *LoadFileOffset(char *pi_pFileName, char *pi_pMode);
  BOOL IsExistFile(char *pi_pFileName);
};

class CMergeFileManager
{
public:
  char mPathName[256];
  DWORD mMergeFileNum;
  DWORD mPathNameLeng;
  CMergeFile *mMergeFile;

  CMergeFileManager(void);
  ~CMergeFileManager(void);

  void InitMergeFile(char *pi_pPathName);
  void ReleaseMergeFile(void);
  FILE *LoadFileOffset(char *pi_pFileName, char *pi_pMode);
  BOOL IsExistFile(char *pi_pFileName);
  unsigned int GetFileSize(char *pi_pFileName);
};

CMergeFileManager *GetMergeFileManager(void);
void SetMergeFileManager(CMergeFileManager *pi_pMergeFileManager);
FILE *fopenMFM(char *pi_pFileName, char *pi_pMode);
BOOL IsExistFileAndMergeFile(char *pi_pFileName);
unsigned int GetFileSizeAndMergeFile(char *pi_pFileName);

#endif
