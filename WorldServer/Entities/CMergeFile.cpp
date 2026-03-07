#include "pch.h"

#include "CMergeFile.h"
#include "WorldServerUtil.h"
#include "R3EngineGlobals.h"
#include "CMainThread.h"

static unsigned int GetOneNameFromPath(char *a1, char *a2, unsigned int *a3)
{
  unsigned int v3 = 0;
  unsigned int i = 0;
  char *v7 = a2;
  for (i = 0; i < 0x80; ++i)
  {
    if (*v7 != '\\')
      break;
    ++v7;
  }
  if (i < 0x80)
  {
    char *v9 = a1;
    char *v10 = &a2[i];
    while (i < 0x80)
    {
      char v11 = *v10;
      if (!v11 || v11 == '\\')
        break;
      ++i;
      *v9++ = v11;
      ++v3;
      ++v10;
    }
  }
  a1[v3] = 0;
  a1[v3 + 1] = 0;
  a1[v3 + 2] = 0;
  a1[v3 + 3] = 0;
  *a3 = i;
  return v3;
}

CMergeFile::CMergeFile()
{
  memset_0(mFileName, 0, sizeof(mFileName));
  mMergeFNF = nullptr;
  mOffset = nullptr;
  mIndex = nullptr;
  mCnt = 0;
  mHeaderSize = 0;
}

__int64 CMergeFile::LoadMergeFileHeader(char *a2)
{
  char *dst = mFileName;
  while ((*dst++ = *a2++) != '\0')
    ;

  FILE *v6 = fopen(mFileName, "rb");
  if (v6)
  {
    float Buffer = 0.0f;
    fread(&Buffer, 4, 1, v6);
    if (Buffer == 1.0f)
    {
      fread(&mCnt, 4, 1, v6);
      int v7 = 72 * mCnt;
      mHeaderSize = v7 + 8;
      unsigned int *v8 = static_cast<unsigned int *>(Dmalloc(v7));
      mIndex = v8;
      mOffset = &v8[mCnt];
      mMergeFNF = reinterpret_cast<_MERGE_FILE *>(mOffset + mCnt);
      fread(v8, 4 * mCnt, 1, v6);
      fread(mOffset, 4ULL * mCnt, 1, v6);
      fread(mMergeFNF, static_cast<unsigned long long>(mCnt) << 6, 1, v6);
      fclose(v6);
      return 1;
    }
    fclose(v6);
    Warning(const_cast<char *>("name"), const_cast<char *>(asc_140884778));
  }
  return 0;
}

void CMergeFile::ReleaseMergeFileHeader()
{
  if (mIndex)
  {
    Dfree(mIndex);
    mIndex = nullptr;
    mOffset = nullptr;
    mMergeFNF = nullptr;
  }
}

struct _iobuf *CMergeFile::LoadFileOffset(char *a2, const char *Mode)
{
  unsigned int FileOffset = GetFileOffset(0, a2);
  if (FileOffset == static_cast<unsigned int>(-1))
    return nullptr;
  FILE *v6 = fopen(mFileName, Mode);
  if (!v6)
    return nullptr;
  fseek(v6, FileOffset + mHeaderSize, 0);
  return v6;
}

unsigned int CMergeFile::GetFileOffset(unsigned int a2, char *a3)
{
  if (!a3 || !*a3)
    return 0;

  unsigned int v20[3]{};
  char v22[128]{};
  int v19 = static_cast<int>(GetOneNameFromPath(v22, a3, v20));
  _MERGE_FILE *mMerge = mMergeFNF;
  unsigned int v7 = 0;
  const _MERGE_FILE &parentEntry = mMerge[a2];
  if (parentEntry.cnt)
  {
    const unsigned int compareDwordCount = (static_cast<unsigned int>(v19) + 3) >> 2;
    do
    {
      const int v11 = static_cast<int>(mIndex[v7 + parentEntry.start_index]);
      if (mMerge[v11].name_cnt == v19)
      {
        if (compareDwordCount == 0
            || memcmp_0(mMerge[v11].___u0.name_cmp, v22, sizeof(unsigned int) * compareDwordCount) == 0)
        {
          if (mOffset[v11] != static_cast<unsigned int>(-1))
            return mOffset[v11];
          unsigned int result = GetFileOffset(v11, &a3[v20[0] + 1]);
          if (result != static_cast<unsigned int>(-1))
            return result;
        }
      }
      ++v7;
    } while (v7 < parentEntry.cnt);
  }
  return static_cast<unsigned int>(-1);
}

unsigned int CMergeFile::GetFileSize(unsigned int a2, char *a3)
{
  if (!a3 || !*a3)
    return 0;

  unsigned int v18[4]{};
  char v19[128]{};
  int v9 = static_cast<int>(GetOneNameFromPath(v19, a3, v18));
  _MERGE_FILE *mMerge = mMergeFNF;
  unsigned int v8 = 0;
  const _MERGE_FILE &parentEntry = mMerge[a2];
  if (parentEntry.cnt)
  {
    unsigned int v10 = v18[0];
    const unsigned int compareDwordCount = (static_cast<unsigned int>(v9) + 3) >> 2;
    do
    {
      const int v13 = static_cast<int>(mIndex[v8 + parentEntry.start_index]);
      if (mMerge[v13].name_cnt == v9)
      {
        if (compareDwordCount == 0
            || memcmp_0(mMerge[v13].___u0.name_cmp, v19, sizeof(unsigned int) * compareDwordCount) == 0)
        {
          if (mOffset[v13] != static_cast<unsigned int>(-1))
            return mMerge[v13].file_length;
          unsigned int result = GetFileOffset(v13, &a3[v10 + 1]);
          if (result != static_cast<unsigned int>(-1))
            return result;
        }
      }
      ++v8;
    } while (v8 < parentEntry.cnt);
  }
  return 0;
}

__int64 CMergeFile::IsExistFile(char *a2)
{
  if (!a2 || !*a2)
    return 0;
  _strlwr(a2);
  return GetFileOffset(0, a2) != static_cast<unsigned int>(-1);
}
