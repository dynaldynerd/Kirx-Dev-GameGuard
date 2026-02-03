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

void CMergeFileManager::InitMergeFile(char *a2)
{
  char *v2 = a2;
  char *dst = mPath;
  while ((*dst++ = *a2++) != '\0')
    ;

  char Buffer[256]{};
  sprintf_s(Buffer, "%s\\*.*", v2);
  WIN32_FIND_DATAA FindFileData{};
  HANDLE FirstFileA = FindFirstFileA(Buffer, &FindFileData);
  if (FirstFileA != reinterpret_cast<HANDLE>(-1))
  {
    unsigned int *v6 = static_cast<unsigned int *>(Dmalloc(0x10000));
    unsigned int *v7 = v6;
    unsigned long long v8 = 256;
    unsigned long long *v9 = nullptr;
    unsigned char v21[2048]{};
    v9 = reinterpret_cast<unsigned long long *>(v21);
    do
    {
      *v9++ = reinterpret_cast<unsigned long long>(v7);
      v7 += 64;
      --v8;
    } while (v8);

    unsigned int v12 = 0;
    unsigned long long *v11 = reinterpret_cast<unsigned long long *>(v21);
    while (FindNextFileA(FirstFileA, &FindFileData))
    {
      if (strcmp(".", FindFileData.cFileName) && strcmp("..", FindFileData.cFileName))
      {
        if ((FindFileData.dwFileAttributes & 0x10) == 0)
        {
          _strlwr(FindFileData.cFileName);
          size_t v13 = strlen(FindFileData.cFileName) + 1;
          if ((unsigned int)(v13 - 1) >= 4
              && FindFileData.cFileName[(unsigned int)(v13 - 2)] == 'k'
              && FindFileData.cFileName[(unsigned int)(v13 - 3)] == 'p'
              && FindFileData.cFileName[(unsigned int)(v13 - 4)] == 'r')
          {
            char *cFileName = FindFileData.cFileName;
            __int64 v15 = static_cast<__int64>(*v11) - reinterpret_cast<__int64>(FindFileData.cFileName);
            char v16;
            do
            {
              v16 = *cFileName++;
              cFileName[v15 - 1] = v16;
            } while (v16);
            ++v12;
            ++v11;
          }
        }
      }
    }
    FindClose(FirstFileA);
    if (v12 <= 0xFF)
    {
      if (v12)
      {
        mMergeFileNum = v12;
        mPathNameLeng = static_cast<unsigned int>(strlen(v2));
        CMergeFile *v17 = static_cast<CMergeFile *>(Dmalloc(288 * v12));
        mMergeFile = v17;
        if (mMergeFileNum)
        {
          const char **v19 = reinterpret_cast<const char **>(v21);
          unsigned int v10 = 0;
          char String[256]{};
          do
          {
            sprintf_s(String, "%s%s", v2, *v19);
            _strlwr(String);
            mMergeFile[v10].LoadMergeFileHeader(String);
            v10 = v10 + 1;
            ++v19;
          } while (v10 < mMergeFileNum);
        }
      }
    }
    else
    {
      Warning(v2, aRpkAaiAo);
    }
    Dfree(v6);
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

unsigned int CMergeFileManager::GetFileSize(char *a2)
{
  _strlwr(a2);
  unsigned int result = 0;
  if (!mMergeFileNum)
    return 0;
  unsigned int v4 = 0;
  while (true)
  {
    result = mMergeFile[v4].GetFileSize(0, &a2[mPathNameLeng]);
    if (result)
      break;
    v4 = v4 + 1;
    if (v4 >= mMergeFileNum)
      return 0;
  }
  return result;
}

__int64 CMergeFileManager::IsExistFile(char *a2)
{
  _strlwr(a2);
  unsigned int v4 = 0;
  if (!mMergeFileNum)
    return 0;
  while (!mMergeFile[v4].IsExistFile(&a2[mPathNameLeng]))
  {
    v4 = v4 + 1;
    if (v4 >= mMergeFileNum)
      return 0;
  }
  return 1;
}

struct _iobuf *CMergeFileManager::LoadFileOffset(char *a2, const char *a3)
{
  _strlwr(a2);
  unsigned int v6 = 0;
  if (!mMergeFileNum)
    return nullptr;
  while (true)
  {
    struct _iobuf *result = mMergeFile[v6].LoadFileOffset(&a2[mPathNameLeng], a3);
    if (result)
      return result;
    v6 = v6 + 1;
    if (v6 >= mMergeFileNum)
      return nullptr;
  }
}
