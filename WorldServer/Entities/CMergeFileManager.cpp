#include "pch.h"

#include "CMergeFileManager.h"
#include "CMergeFile.h"
#include "WorldServerUtil.h"
#include "R3EngineGlobals.h"

#include <windows.h>

CMergeFileManager::CMergeFileManager()
{
  memset_0(mPath, 0, sizeof(mPath));
  mPathNameLeng = 0;
  mMergeFileNum = 0;
  mMergeFile = nullptr;
}

CMergeFileManager::~CMergeFileManager()
{
  ReleaseMergeFile();
}

void CMergeFileManager::InitMergeFile(char *directoryPath)
{
  char *dst = mPath;
  char *src = directoryPath;
  while ((*dst++ = *src++) != '\0')
    ;

  char searchPattern[256]{};
  sprintf_s(searchPattern, "%s\\*.*", directoryPath);
  WIN32_FIND_DATAA findFileData{};
  HANDLE findHandle = FindFirstFileA(searchPattern, &findFileData);
  if (findHandle != INVALID_HANDLE_VALUE)
  {
    char *mergeFileNameStorage = static_cast<char *>(Dmalloc(0x10000));
    char *mergeFileNames[256]{};
    char *nextMergeFileName = mergeFileNameStorage;
    for (unsigned int slot = 0; slot < 256; ++slot)
    {
      mergeFileNames[slot] = nextMergeFileName;
      nextMergeFileName += 256;
    }

    unsigned int mergeFileCount = 0;
    char **mergeFileNameCursor = mergeFileNames;
    while (FindNextFileA(findHandle, &findFileData))
    {
      if (strcmp(".", findFileData.cFileName) && strcmp("..", findFileData.cFileName))
      {
        if ((findFileData.dwFileAttributes & 0x10) == 0)
        {
          _strlwr(findFileData.cFileName);
          const size_t fileNameLength = strlen(findFileData.cFileName) + 1;
          if (static_cast<unsigned int>(fileNameLength - 1) >= 4
              && findFileData.cFileName[fileNameLength - 2] == 'k'
              && findFileData.cFileName[fileNameLength - 3] == 'p'
              && findFileData.cFileName[fileNameLength - 4] == 'r')
          {
            if (mergeFileCount < 256)
            {
              strcpy_s(*mergeFileNameCursor, 256, findFileData.cFileName);
              ++mergeFileNameCursor;
            }
            ++mergeFileCount;
          }
        }
      }
    }
    FindClose(findHandle);
    if (mergeFileCount <= 0xFF)
    {
      if (mergeFileCount)
      {
        mMergeFileNum = mergeFileCount;
        mPathNameLeng = static_cast<unsigned int>(strlen(directoryPath));
        mMergeFile = static_cast<CMergeFile *>(Dmalloc(288 * mergeFileCount));
        if (mMergeFileNum)
        {
          char mergedFilePath[256]{};
          for (unsigned int fileIndex = 0; fileIndex < mMergeFileNum; ++fileIndex)
          {
            sprintf_s(mergedFilePath, "%s%s", directoryPath, mergeFileNames[fileIndex]);
            _strlwr(mergedFilePath);
            mMergeFile[fileIndex].LoadMergeFileHeader(mergedFilePath);
          }
        }
      }
    }
    else
      Warning(directoryPath, aRpkAaiAo);

    Dfree(mergeFileNameStorage);
  }
}

void CMergeFileManager::ReleaseMergeFile()
{
  if (mMergeFile)
  {
    for (unsigned int i = 0; i < mMergeFileNum; ++i)
      mMergeFile[i].ReleaseMergeFileHeader();
    Dfree(mMergeFile->mFileName);
    mMergeFile = nullptr;
    mMergeFileNum = 0;
  }
}

unsigned int CMergeFileManager::GetFileSize(char *filePath)
{
  _strlwr(filePath);
  if (!mMergeFileNum)
    return 0;

  for (unsigned int fileIndex = 0; fileIndex < mMergeFileNum; ++fileIndex)
  {
    const unsigned int fileSize = mMergeFile[fileIndex].GetFileSize(0, &filePath[mPathNameLeng]);
    if (fileSize)
      return fileSize;
  }
  return 0;
}

__int64 CMergeFileManager::IsExistFile(char *filePath)
{
  _strlwr(filePath);
  if (!mMergeFileNum)
    return 0;

  for (unsigned int fileIndex = 0; fileIndex < mMergeFileNum; ++fileIndex)
  {
    if (mMergeFile[fileIndex].IsExistFile(&filePath[mPathNameLeng]))
      return 1;
  }
  return 0;
}

struct _iobuf *CMergeFileManager::LoadFileOffset(char *filePath, const char *mode)
{
  _strlwr(filePath);
  if (!mMergeFileNum)
    return nullptr;

  for (unsigned int fileIndex = 0; fileIndex < mMergeFileNum; ++fileIndex)
  {
    struct _iobuf *result = mMergeFile[fileIndex].LoadFileOffset(&filePath[mPathNameLeng], mode);
    if (result)
      return result;
  }
  return nullptr;
}
