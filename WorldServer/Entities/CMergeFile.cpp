#include "pch.h"

#include "CMergeFile.h"
#include "WorldServerUtil.h"
#include "R3EngineGlobals.h"
#include "CMainThread.h"

static unsigned int GetOneNameFromPath(char *outName, char *path, unsigned int *consumedLength)
{
  unsigned int nameLength = 0;
  unsigned int pathIndex = 0;
  char *pathCursor = path;
  for (pathIndex = 0; pathIndex < 128; ++pathIndex)
  {
    if (*pathCursor != '\\')
      break;
    ++pathCursor;
  }
  if (pathIndex < 128)
  {
    char *nameCursor = outName;
    char *componentCursor = &path[pathIndex];
    while (pathIndex < 128)
    {
      const char currentCharacter = *componentCursor;
      if (!currentCharacter || currentCharacter == '\\')
        break;
      ++pathIndex;
      *nameCursor++ = currentCharacter;
      ++nameLength;
      ++componentCursor;
    }
  }
  outName[nameLength] = 0;
  outName[nameLength + 1] = 0;
  outName[nameLength + 2] = 0;
  outName[nameLength + 3] = 0;
  *consumedLength = pathIndex;
  return nameLength;
}

CMergeFile::CMergeFile()
{
  std::memset(mFileName, 0, sizeof(mFileName));
  mMergeFNF = nullptr;
  mOffset = nullptr;
  mIndex = nullptr;
  mCnt = 0;
  mHeaderSize = 0;
}

__int64 CMergeFile::LoadMergeFileHeader(char *filePath)
{
  char *dst = mFileName;
  while ((*dst++ = *filePath++) != '\0')
    ;

  FILE *mergeFile = fopen(mFileName, "rb");
  if (mergeFile)
  {
    float version = 0.0f;
    fread(&version, 4, 1, mergeFile);
    if (version == 1.0f)
    {
      fread(&mCnt, 4, 1, mergeFile);
      const int headerBytes = 72 * mCnt;
      mHeaderSize = headerBytes + 8;
      unsigned int *headerData = static_cast<unsigned int *>(Dmalloc(headerBytes));
      mIndex = headerData;
      mOffset = &headerData[mCnt];
      mMergeFNF = reinterpret_cast<_MERGE_FILE *>(mOffset + mCnt);
      fread(headerData, 4 * mCnt, 1, mergeFile);
      fread(mOffset, 4ULL * mCnt, 1, mergeFile);
      fread(mMergeFNF, static_cast<unsigned long long>(mCnt) << 6, 1, mergeFile);
      fclose(mergeFile);
      return 1;
    }
    fclose(mergeFile);
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

struct _iobuf *CMergeFile::LoadFileOffset(char *filePath, const char *mode)
{
  const unsigned int fileOffset = GetFileOffset(0, filePath);
  if (fileOffset == static_cast<unsigned int>(-1))
    return nullptr;
  FILE *mergeFile = fopen(mFileName, mode);
  if (!mergeFile)
    return nullptr;
  fseek(mergeFile, fileOffset + mHeaderSize, 0);
  return mergeFile;
}

unsigned int CMergeFile::GetFileOffset(unsigned int parentIndex, char *filePath)
{
  if (!filePath || !*filePath)
    return 0;

  unsigned int consumedLength[3]{};
  char pathName[128]{};
  const int nameLength = static_cast<int>(GetOneNameFromPath(pathName, filePath, consumedLength));
  _MERGE_FILE *mergeFileEntries = mMergeFNF;
  unsigned int childIndex = 0;
  const _MERGE_FILE &parentEntry = mergeFileEntries[parentIndex];
  if (parentEntry.cnt)
  {
    const unsigned int compareDwordCount = (static_cast<unsigned int>(nameLength) + 3) >> 2;
    do
    {
      const int mergeEntryIndex = static_cast<int>(mIndex[childIndex + parentEntry.start_index]);
      if (mergeFileEntries[mergeEntryIndex].name_cnt == nameLength)
      {
        if (compareDwordCount == 0
            || std::memcmp(mergeFileEntries[mergeEntryIndex].___u0.name_cmp, pathName, sizeof(unsigned int) * compareDwordCount) == 0)
        {
          if (mOffset[mergeEntryIndex] != static_cast<unsigned int>(-1))
            return mOffset[mergeEntryIndex];
          const unsigned int result = GetFileOffset(mergeEntryIndex, &filePath[consumedLength[0] + 1]);
          if (result != static_cast<unsigned int>(-1))
            return result;
        }
      }
      ++childIndex;
    } while (childIndex < parentEntry.cnt);
  }
  return static_cast<unsigned int>(-1);
}

unsigned int CMergeFile::GetFileSize(unsigned int parentIndex, char *filePath)
{
  if (!filePath || !*filePath)
    return 0;

  unsigned int consumedLength[4]{};
  char pathName[128]{};
  const int nameLength = static_cast<int>(GetOneNameFromPath(pathName, filePath, consumedLength));
  _MERGE_FILE *mergeFileEntries = mMergeFNF;
  unsigned int childIndex = 0;
  const _MERGE_FILE &parentEntry = mergeFileEntries[parentIndex];
  if (parentEntry.cnt)
  {
    const unsigned int nextPathOffset = consumedLength[0];
    const unsigned int compareDwordCount = (static_cast<unsigned int>(nameLength) + 3) >> 2;
    do
    {
      const int mergeEntryIndex = static_cast<int>(mIndex[childIndex + parentEntry.start_index]);
      if (mergeFileEntries[mergeEntryIndex].name_cnt == nameLength)
      {
        if (compareDwordCount == 0
            || std::memcmp(mergeFileEntries[mergeEntryIndex].___u0.name_cmp, pathName, sizeof(unsigned int) * compareDwordCount) == 0)
        {
          if (mOffset[mergeEntryIndex] != static_cast<unsigned int>(-1))
            return mergeFileEntries[mergeEntryIndex].file_length;
          const unsigned int result = GetFileOffset(mergeEntryIndex, &filePath[nextPathOffset + 1]);
          if (result != static_cast<unsigned int>(-1))
            return result;
        }
      }
      ++childIndex;
    } while (childIndex < parentEntry.cnt);
  }
  return 0;
}

__int64 CMergeFile::IsExistFile(char *filePath)
{
  if (!filePath || !*filePath)
    return 0;
  _strlwr(filePath);
  return GetFileOffset(0, filePath) != static_cast<unsigned int>(-1);
}
