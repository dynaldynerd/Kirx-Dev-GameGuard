#include "mergefile.h"

#include <string.h>

#include "jerror.h"
#include "jmalloc.h"

namespace
{
  const unsigned int kInvalidMergeOffset = 0xFFFFFFFFu;

  CMergeFileManager *g_pMergeFileManager = NULL;

  void CopyLowerPath(char *po_pDest, size_t pi_uDestSize, const char *pi_pSrc)
  {
    if (!po_pDest || pi_uDestSize == 0)
    {
      return;
    }

    po_pDest[0] = 0;
    if (!pi_pSrc)
    {
      return;
    }

    strncpy(po_pDest, pi_pSrc, pi_uDestSize - 1);
    po_pDest[pi_uDestSize - 1] = 0;
    _strlwr(po_pDest);
  }

  unsigned int GetOneNameFromPath(char *po_pName, char *pi_pPathName, unsigned int *po_pPathIndex)
  {
    unsigned int l_dwNameCnt = 0;
    unsigned int l_dwPathIndex = 0;
    char *l_pPathName = pi_pPathName;

    if (!po_pName || !pi_pPathName || !po_pPathIndex)
    {
      return 0;
    }

    while (l_dwPathIndex < 0x80 && *l_pPathName == '\\')
    {
      ++l_dwPathIndex;
      ++l_pPathName;
    }

    while (l_dwPathIndex < 0x80 && *l_pPathName && *l_pPathName != '\\')
    {
      po_pName[l_dwNameCnt++] = *l_pPathName++;
      ++l_dwPathIndex;
    }

    po_pName[l_dwNameCnt] = 0;
    po_pName[l_dwNameCnt + 1] = 0;
    po_pName[l_dwNameCnt + 2] = 0;
    po_pName[l_dwNameCnt + 3] = 0;
    *po_pPathIndex = l_dwPathIndex;
    return l_dwNameCnt;
  }
}

CMergeFile::CMergeFile(void)
{
  memset(this, 0, sizeof(CMergeFile));
}

BOOL CMergeFile::LoadMergeFileHeader(char *pi_pFileName)
{
  float l_fVersion = 0.0f;
  FILE *l_pFile = NULL;
  int l_nIndexSize = 0;

  if (!pi_pFileName || !pi_pFileName[0])
  {
    return FALSE;
  }

  strncpy(mFileName, pi_pFileName, sizeof(mFileName) - 1);
  mFileName[sizeof(mFileName) - 1] = 0;

  l_pFile = fopen(mFileName, "rb");
  if (!l_pFile)
  {
    return FALSE;
  }

  fread(&l_fVersion, sizeof(float), 1, l_pFile);
  if (l_fVersion != 1.0f)
  {
    fclose(l_pFile);
    Warning(pi_pFileName, "<- merge file version mismatch.");
    return FALSE;
  }

  fread(&mCnt, sizeof(DWORD), 1, l_pFile);
  l_nIndexSize = sizeof(unsigned int) * mCnt;
  mHeaderSize = 8 + l_nIndexSize + l_nIndexSize + (int)(sizeof(_MERGE_FILE) * mCnt);
  mIndex = (unsigned int *)Dmalloc(l_nIndexSize + l_nIndexSize + (int)(sizeof(_MERGE_FILE) * mCnt));
  mOffset = mIndex + mCnt;
  mMergeFNF = (_MERGE_FILE *)(mOffset + mCnt);

  fread(mIndex, l_nIndexSize, 1, l_pFile);
  fread(mOffset, l_nIndexSize, 1, l_pFile);
  fread(mMergeFNF, sizeof(_MERGE_FILE) * mCnt, 1, l_pFile);
  fclose(l_pFile);

  return TRUE;
}

void CMergeFile::ReleaseMergeFileHeader(void)
{
  if (mIndex)
  {
    Dfree(mIndex);
    mIndex = NULL;
    mOffset = NULL;
    mMergeFNF = NULL;
  }
}

unsigned int CMergeFile::GetFileOffset(unsigned int pi_dwNodeIndex, char *pi_pFileName)
{
  char l_szOneName[128];
  unsigned int l_dwPathIndex = 0;
  unsigned int l_dwNameCnt = 0;
  unsigned int l_dwWordCnt = 0;

  if (!pi_pFileName || !pi_pFileName[0])
  {
    return 0;
  }

  l_dwNameCnt = GetOneNameFromPath(l_szOneName, pi_pFileName, &l_dwPathIndex);
  l_dwWordCnt = (l_dwNameCnt + 3) >> 2;

  if (mMergeFNF[pi_dwNodeIndex].cnt == 0)
  {
    return kInvalidMergeOffset;
  }

  for (unsigned int i = 0; i < mMergeFNF[pi_dwNodeIndex].cnt; ++i)
  {
    const unsigned int l_dwEntryIndex = mIndex[mMergeFNF[pi_dwNodeIndex].start_index + i];
    if (mMergeFNF[l_dwEntryIndex].name_cnt != l_dwNameCnt)
    {
      continue;
    }

    if (l_dwWordCnt && memcmp(mMergeFNF[l_dwEntryIndex].name, l_szOneName, l_dwWordCnt * sizeof(unsigned int)) != 0)
    {
      continue;
    }

    if (mOffset[l_dwEntryIndex] != kInvalidMergeOffset)
    {
      return mOffset[l_dwEntryIndex];
    }

    {
      const unsigned int l_dwOffset = GetFileOffset(l_dwEntryIndex, &pi_pFileName[l_dwPathIndex + 1]);
      if (l_dwOffset != kInvalidMergeOffset)
      {
        return l_dwOffset;
      }
    }
  }

  return kInvalidMergeOffset;
}

unsigned int CMergeFile::GetFileSize(unsigned int pi_dwNodeIndex, char *pi_pFileName)
{
  char l_szOneName[128];
  unsigned int l_dwPathIndex = 0;
  unsigned int l_dwNameCnt = 0;
  unsigned int l_dwWordCnt = 0;

  if (!pi_pFileName || !pi_pFileName[0])
  {
    return 0;
  }

  l_dwNameCnt = GetOneNameFromPath(l_szOneName, pi_pFileName, &l_dwPathIndex);
  l_dwWordCnt = (l_dwNameCnt + 3) >> 2;

  if (mMergeFNF[pi_dwNodeIndex].cnt == 0)
  {
    return 0;
  }

  for (unsigned int i = 0; i < mMergeFNF[pi_dwNodeIndex].cnt; ++i)
  {
    const unsigned int l_dwEntryIndex = mIndex[mMergeFNF[pi_dwNodeIndex].start_index + i];
    if (mMergeFNF[l_dwEntryIndex].name_cnt != l_dwNameCnt)
    {
      continue;
    }

    if (l_dwWordCnt && memcmp(mMergeFNF[l_dwEntryIndex].name, l_szOneName, l_dwWordCnt * sizeof(unsigned int)) != 0)
    {
      continue;
    }

    if (mOffset[l_dwEntryIndex] != kInvalidMergeOffset)
    {
      return (unsigned int)mMergeFNF[l_dwEntryIndex].file_length;
    }

    return GetFileSize(l_dwEntryIndex, &pi_pFileName[l_dwPathIndex + 1]);
  }

  return 0;
}

FILE *CMergeFile::LoadFileOffset(char *pi_pFileName, char *pi_pMode)
{
  unsigned int l_dwFileOffset = GetFileOffset(0, pi_pFileName);
  FILE *l_pFile = NULL;

  if (l_dwFileOffset == kInvalidMergeOffset)
  {
    return NULL;
  }

  l_pFile = fopen(mFileName, pi_pMode);
  if (!l_pFile)
  {
    return NULL;
  }

  fseek(l_pFile, l_dwFileOffset + mHeaderSize, SEEK_SET);
  return l_pFile;
}

BOOL CMergeFile::IsExistFile(char *pi_pFileName)
{
  return GetFileOffset(0, pi_pFileName) != kInvalidMergeOffset;
}

CMergeFileManager::CMergeFileManager(void)
{
  memset(this, 0, sizeof(CMergeFileManager));
}

CMergeFileManager::~CMergeFileManager(void)
{
  ReleaseMergeFile();
}

void CMergeFileManager::InitMergeFile(char *pi_pPathName)
{
  char l_szFindPath[256];
  HANDLE l_hFind = INVALID_HANDLE_VALUE;
  WIN32_FIND_DATAA l_stFindFileData;
  char *l_pFileNameTable = NULL;
  char *l_apFileNames[256];
  unsigned int l_dwMergeFileNum = 0;
  char l_szMergeFileName[256];

  ReleaseMergeFile();

  if (!pi_pPathName || !pi_pPathName[0])
  {
    return;
  }

  strncpy(mPathName, pi_pPathName, sizeof(mPathName) - 1);
  mPathName[sizeof(mPathName) - 1] = 0;

  sprintf(l_szFindPath, "%s\\*.*", pi_pPathName);
  l_hFind = FindFirstFileA(l_szFindPath, &l_stFindFileData);
  if (l_hFind == INVALID_HANDLE_VALUE)
  {
    return;
  }

  l_pFileNameTable = (char *)Dmalloc(0x10000);
  for (unsigned int i = 0; i < 256; ++i)
  {
    l_apFileNames[i] = &l_pFileNameTable[i * 256];
    l_apFileNames[i][0] = 0;
  }

  do
  {
    if (!strcmp(l_stFindFileData.cFileName, ".") || !strcmp(l_stFindFileData.cFileName, ".."))
    {
      continue;
    }

    if (l_stFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
      continue;
    }

    _strlwr(l_stFindFileData.cFileName);
    if (strlen(l_stFindFileData.cFileName) < 4)
    {
      continue;
    }

    if (strcmp(&l_stFindFileData.cFileName[strlen(l_stFindFileData.cFileName) - 4], ".rpk") != 0)
    {
      continue;
    }

    if (l_dwMergeFileNum >= 0x100)
    {
      continue;
    }

    strcpy(l_apFileNames[l_dwMergeFileNum], l_stFindFileData.cFileName);
    ++l_dwMergeFileNum;
  }
  while (FindNextFileA(l_hFind, &l_stFindFileData));

  FindClose(l_hFind);

  if (l_dwMergeFileNum)
  {
    mMergeFileNum = l_dwMergeFileNum;
    mPathNameLeng = strlen(pi_pPathName);
    mMergeFile = (CMergeFile *)Dmalloc(sizeof(CMergeFile) * mMergeFileNum);
    memset(mMergeFile, 0, sizeof(CMergeFile) * mMergeFileNum);

    for (unsigned int i = 0; i < mMergeFileNum; ++i)
    {
      sprintf(l_szMergeFileName, "%s%s", pi_pPathName, l_apFileNames[i]);
      _strlwr(l_szMergeFileName);
      mMergeFile[i].LoadMergeFileHeader(l_szMergeFileName);
    }
  }

  Dfree(l_pFileNameTable);
}

void CMergeFileManager::ReleaseMergeFile(void)
{
  if (mMergeFile)
  {
    for (unsigned int i = 0; i < mMergeFileNum; ++i)
    {
      mMergeFile[i].ReleaseMergeFileHeader();
    }
    Dfree(mMergeFile);
    mMergeFile = NULL;
  }
  mMergeFileNum = 0;
}

FILE *CMergeFileManager::LoadFileOffset(char *pi_pFileName, char *pi_pMode)
{
  char l_szFileName[256];

  if (!mMergeFileNum || !pi_pFileName || !pi_pFileName[0])
  {
    return NULL;
  }

  CopyLowerPath(l_szFileName, sizeof(l_szFileName), pi_pFileName);
  for (unsigned int i = 0; i < mMergeFileNum; ++i)
  {
    FILE *l_pFile = mMergeFile[i].LoadFileOffset(&l_szFileName[mPathNameLeng], pi_pMode);
    if (l_pFile)
    {
      return l_pFile;
    }
  }

  return NULL;
}

BOOL CMergeFileManager::IsExistFile(char *pi_pFileName)
{
  char l_szFileName[256];

  if (!mMergeFileNum || !pi_pFileName || !pi_pFileName[0])
  {
    return FALSE;
  }

  CopyLowerPath(l_szFileName, sizeof(l_szFileName), pi_pFileName);
  for (unsigned int i = 0; i < mMergeFileNum; ++i)
  {
    if (mMergeFile[i].IsExistFile(&l_szFileName[mPathNameLeng]))
    {
      return TRUE;
    }
  }

  return FALSE;
}

unsigned int CMergeFileManager::GetFileSize(char *pi_pFileName)
{
  char l_szFileName[256];

  if (!mMergeFileNum || !pi_pFileName || !pi_pFileName[0])
  {
    return 0;
  }

  CopyLowerPath(l_szFileName, sizeof(l_szFileName), pi_pFileName);
  for (unsigned int i = 0; i < mMergeFileNum; ++i)
  {
    const unsigned int l_dwFileSize = mMergeFile[i].GetFileSize(0, &l_szFileName[mPathNameLeng]);
    if (l_dwFileSize)
    {
      return l_dwFileSize;
    }
  }

  return 0;
}

CMergeFileManager *GetMergeFileManager(void)
{
  return g_pMergeFileManager;
}

void SetMergeFileManager(CMergeFileManager *pi_pMergeFileManager)
{
  g_pMergeFileManager = pi_pMergeFileManager;
}

FILE *fopenMFM(char *pi_pFileName, char *pi_pMode)
{
  CMergeFileManager *l_pMergeFileManager = GetMergeFileManager();

  if (l_pMergeFileManager)
  {
    FILE *l_pFile = l_pMergeFileManager->LoadFileOffset(pi_pFileName, pi_pMode);
    if (l_pFile)
    {
      return l_pFile;
    }
  }

  return fopen(pi_pFileName, pi_pMode);
}

BOOL IsExistFileAndMergeFile(char *pi_pFileName)
{
  FILE *l_pFile = NULL;

  if (!pi_pFileName || !pi_pFileName[0])
  {
    return FALSE;
  }

  if (g_pMergeFileManager && g_pMergeFileManager->IsExistFile(pi_pFileName))
  {
    return TRUE;
  }

  l_pFile = fopen(pi_pFileName, "rb");
  if (!l_pFile)
  {
    return FALSE;
  }

  fclose(l_pFile);
  return TRUE;
}

unsigned int GetFileSizeAndMergeFile(char *pi_pFileName)
{
  FILE *l_pFile = NULL;
  int l_nStart = 0;
  int l_nEnd = 0;

  if (!pi_pFileName || !pi_pFileName[0])
  {
    return 0;
  }

  if (g_pMergeFileManager)
  {
    const unsigned int l_dwMergedFileSize = g_pMergeFileManager->GetFileSize(pi_pFileName);
    if (l_dwMergedFileSize)
    {
      return l_dwMergedFileSize;
    }
  }

  l_pFile = fopen(pi_pFileName, "rb");
  if (!l_pFile)
  {
    return 0;
  }

  l_nStart = ftell(l_pFile);
  fseek(l_pFile, 0, SEEK_END);
  l_nEnd = ftell(l_pFile);
  fclose(l_pFile);

  return (unsigned int)(l_nEnd - l_nStart);
}
