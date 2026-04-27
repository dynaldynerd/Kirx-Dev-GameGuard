#include "CPlayer.h"

#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <vector>

#include <dbghelp.h>
#include <ddraw.h>

#include "CCharacterMgr.h"
#include "CItemDataMgr.h"
#include "CResourceDataMgr.h"
#include "Network/REGED_AVATOR_DB.h"
#include "R3Engine/common/commonutil.h"
#include "R3Engine/2ndclass/r3util.h"

#pragma comment(lib, "dbghelp.lib")

namespace
{
constexpr char kDebugCaptureLogPath[] = ".\\RFClientCapture.log";
constexpr char kPlayerMeshRFSPath[] = ".\\Character\\Player\\Mesh\\";
constexpr char kPlayerAniRFSPath[] = ".\\Character\\Player\\Ani\\";
constexpr char kPlayerTexRFSPath[] = ".\\Character\\Player\\Tex\\";
constexpr char kDecodedPlayerTexCachePath[] = ".\\_RFClientTextureCache\\Character\\Player\\Tex\\";
constexpr char kLoosePlayerMeshProbe[] = ".\\Character\\Player\\Mesh\\ACCRETIA_ARMOR_FACE_003.msh";
constexpr char kLoosePlayerAniProbe[] = ".\\Character\\Player\\Ani\\ACCRETIA_PEACE_STAND_NONE_NONE_01_00.ANI";
constexpr char kAccretiaStandAniName[] = "ACCRETIA_PEACE_STAND_NONE_NONE_01_00.ANI";
constexpr char kAccretiaWalkAniName[] = "ACCRETIA_PEACE_WALK_NONE_NONE_01_00.ANI";
constexpr char kAccretiaRunAniName[] = "ACCRETIA_PEACE_RUN_NONE_NONE_01_00.ANI";
constexpr char kAccretiaBwWalkAniName[] = "ACCRETIA_PEACE_BWWALK_NONE_NONE_01_00.ANI";
constexpr char kAccretiaLfWalkAniName[] = "ACCRETIA_PEACE_LFWALK_NONE_NONE_01_00.ANI";
constexpr char kAccretiaRtWalkAniName[] = "ACCRETIA_PEACE_RTWALK_NONE_NONE_01_00.ANI";
constexpr char kAccretiaBwRunAniName[] = "ACCRETIA_PEACE_BWRUN_NONE_NONE_01_00.ANI";
constexpr char kAccretiaLfRunAniName[] = "ACCRETIA_PEACE_LFRUN_NONE_NONE_01_00.ANI";
constexpr char kAccretiaRtRunAniName[] = "ACCRETIA_PEACE_RTRUN_NONE_NONE_01_00.ANI";
constexpr size_t kObjectMeshTextureOffset = 0x304;
constexpr size_t kObjectMeshStride = 0x430;
constexpr size_t kTextureManagerPathOffset = 0x1C;
constexpr size_t kTextureManagerPathSize = 0x64;
constexpr size_t kCharacterObjectTexturePathOffset = 0x114;
constexpr size_t kCharacterObjectTexturePathSize = 0xFC;
constexpr BYTE kBaseFixNum = 5;
constexpr DWORD kDefaultMeshCategory = 0x050000;
constexpr BYTE kServerBasePartToClientPart[kBaseFixNum] =
{
  CDPT_UPPER_PART,
  CDPT_LOWER_PART,
  CDPT_GLOVES,
  CDPT_SHOES,
  CDPT_HELMET
};
constexpr BYTE kServerBasePartToItemType[kBaseFixNum] =
{
  IEPT_UPPER_PART,
  IEPT_LOWER_PART,
  IEPT_GLOVES,
  IEPT_SHOES,
  IEPT_HELMET
};
constexpr BYTE kDefaultPartMeshCode[MAX_DEFAULT_PART] =
{
  1, // face
  2, // upper
  3, // lower
  4, // gloves
  5, // shoes
  0  // helmet
};

#if defined(_DEBUG)
void AppendPlayerLog(const char *format, ...);
#endif

bool IsTextureFileNameLocal(const char *pi_pFileName);

void SetCharacterObjectTexturePath(CHARACTEROBJECT *pi_pCharacterObject, const char *pi_pTexturePath)
{
  if (!pi_pCharacterObject || !pi_pTexturePath)
  {
    return;
  }

  char *l_pTexturePath = reinterpret_cast<char *>(pi_pCharacterObject) + kCharacterObjectTexturePathOffset;
  strncpy_s(l_pTexturePath, kCharacterObjectTexturePathSize, pi_pTexturePath, _TRUNCATE);
}

void SetTextureManagerPath(TextureManager *pi_pTextureManager, const char *pi_pTexturePath)
{
  if (!pi_pTextureManager || !pi_pTexturePath)
  {
    return;
  }

  char *l_pTexturePath = reinterpret_cast<char *>(pi_pTextureManager) + kTextureManagerPathOffset;
  strncpy_s(l_pTexturePath, kTextureManagerPathSize, pi_pTexturePath, _TRUNCATE);
}

float NormalizeAngle360Local(float pi_fValue)
{
  while (pi_fValue >= 360.0f)
  {
    pi_fValue -= 360.0f;
  }

  while (pi_fValue < 0.0f)
  {
    pi_fValue += 360.0f;
  }

  return pi_fValue;
}

constexpr char kDefaultAccretiaMeshName[MAX_DEFAULT_PART][64] =
{
  "ACCRETIA_ARMOR_FACE_003.MSH",
  "ACCRETIA_ARMOR_UPPER_000.MSH",
  "ACCRETIA_ARMOR_LOWER_000.MSH",
  "ACCRETIA_ARMOR_GLOVES_000.MSH",
  "ACCRETIA_ARMOR_SHOES_000.MSH",
  "ACCRETIA_ARMOR_HELMET_000.MSH"
};

DWORD MakePlayerMeshID(BYTE pi_byRaceSexCode, BYTE pi_byPartType, DWORD pi_dwVariant, bool pi_bDefaultMesh)
{
  if (pi_byPartType >= MAX_DEFAULT_PART)
  {
    return ID_INVALID;
  }

  return (static_cast<DWORD>(pi_byRaceSexCode) << 20) |
         (pi_bDefaultMesh ? kDefaultMeshCategory : 0) |
         (static_cast<DWORD>(kDefaultPartMeshCode[pi_byPartType]) << 8) |
         (pi_dwVariant & 0xFF);
}

DWORD MakeDefaultItemDTIndex(BYTE pi_byRaceSexCode, DWORD pi_dwVariant)
{
  return (static_cast<DWORD>(pi_byRaceSexCode) * 16) + (pi_dwVariant & 0x0F);
}

DWORD ApplyPlayerRaceToItemMeshID(DWORD pi_dwMeshID, BYTE pi_byRaceSexCode)
{
  if (((pi_dwMeshID & 0x000F0000) == 0x00000000) &&
      ((pi_dwMeshID & 0x0000FF00) != 0x00000700))
  {
    return (pi_dwMeshID & 0xFF0FFFFF) |
           (static_cast<DWORD>(pi_byRaceSexCode) << 20);
  }

  return pi_dwMeshID;
}

typedef struct
{
  DWORD dwMagic;
  DDSURFACEDESC2 ddsd;
} _DDS_Header;

void NormalizeTextureManagerPath(char *po_szPath, size_t pi_stPathSize, const char *pi_pTexturePath)
{
  if (!po_szPath || pi_stPathSize == 0)
  {
    return;
  }

  if (!pi_pTexturePath)
  {
    po_szPath[0] = '\0';
    return;
  }

  strcpy_s(po_szPath, pi_stPathSize, pi_pTexturePath);
  while (po_szPath[0])
  {
    const size_t l_stLength = strlen(po_szPath);
    if (l_stLength == 0)
    {
      break;
    }

    char &l_rLastChar = po_szPath[l_stLength - 1];
    if (l_rLastChar != '\\' && l_rLastChar != '/')
    {
      break;
    }

    l_rLastChar = '\0';
  }
}

void FixDDSHeader(BYTE *buf)
{
  _DDS_Header *header = (_DDS_Header *)buf;

  DWORD tempd2 = header->ddsd.ddpfPixelFormat.dwFlags;
  if (tempd2 == 5)
    header->ddsd.ddpfPixelFormat.dwFlags = 4;
  if (tempd2 == 0x41)
  {
    if (header->ddsd.ddpfPixelFormat.dwRGBBitCount == 16)
    {
      if (header->ddsd.ddpfPixelFormat.dwRGBAlphaBitMask != 0x00008000)
        header->ddsd.ddpfPixelFormat.dwRBitMask = 0x00000f00;
    }
    else if (header->ddsd.ddpfPixelFormat.dwRGBBitCount == 32)
    {
      header->ddsd.ddpfPixelFormat.dwRBitMask = 0x00ff0000;
      header->ddsd.ddpfPixelFormat.dwGBitMask = 0x0000ff00;
      header->ddsd.ddpfPixelFormat.dwBBitMask = 0x000000ff;
      header->ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0xff000000;
    }
  }
  if (header->ddsd.ddpfPixelFormat.dwSize == 0)
  {
    header->ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
    header->ddsd.ddpfPixelFormat.dwFlags = 4;
  }
}

void UnLockDDS(DWORD *buf)
{
  static const DWORD pass_word[32] =
  {
    0x764D802E, 0xF0D1F82E, 0x81863FBD, 0x3F3F2C58,
    0x6F672E2E, 0x783F403F, 0xF6A5C0F1, 0x9F3BF6A5,
    0xD73F20C1, 0xE9C1C8D7, 0xEFBD8685, 0xFBA13F56,
    0x61868687, 0x4E3B214C, 0x97AE5778, 0x3F2E4A2E,
    0x442E4C3F, 0xE8E95FC5, 0xBDBEECED, 0x2E6CF7BB,
    0x3F2EE4F2, 0x9F973F3F, 0x76B921B3, 0x3F546576,
    0xF0C6F6E6, 0xB2E2DB79, 0xEB2E2E4B, 0xABCAD3D3,
    0x9CEDC7EA, 0x48D0D9C7, 0x2E35FAB4, 0x9B6A2E2E
  };

  for (DWORD i = 0; i < 32; ++i)
    buf[i] ^= pass_word[i];
}

bool DecodeDDSBuffer(BYTE *pi_pBuffer, DWORD pi_dwBufferSize)
{
  if (!pi_pBuffer || pi_dwBufferSize < sizeof(_DDS_Header))
  {
    return false;
  }

  _DDS_Header *l_pHeader = (_DDS_Header *)pi_pBuffer;
  if (l_pHeader->dwMagic != 0x20534444)
  {
    UnLockDDS((DWORD *)pi_pBuffer);
    if (l_pHeader->dwMagic != 0x20534444)
    {
      return false;
    }
  }

  FixDDSHeader(pi_pBuffer);
  return true;
}

bool EnsureDirectoryPath(const char *pi_pPath)
{
  if (!pi_pPath || !pi_pPath[0])
  {
    return false;
  }

  char l_szPath[MAX_PATH];
  strcpy_s(l_szPath, sizeof(l_szPath), pi_pPath);
  const size_t l_stLength = strlen(l_szPath);
  for (size_t i = 0; i < l_stLength; ++i)
  {
    if (l_szPath[i] != '\\' && l_szPath[i] != '/')
    {
      continue;
    }

    const char l_chSaved = l_szPath[i];
    l_szPath[i] = '\0';
    if (l_szPath[0] && strcmp(l_szPath, ".") != 0 && strcmp(l_szPath, "..") != 0)
    {
      CreateDirectoryA(l_szPath, NULL);
    }
    l_szPath[i] = l_chSaved;
  }

  if (l_stLength > 0 && l_szPath[l_stLength - 1] != '\\' && l_szPath[l_stLength - 1] != '/')
  {
    CreateDirectoryA(l_szPath, NULL);
  }

  char l_szCheckPath[MAX_PATH];
  strcpy_s(l_szCheckPath, sizeof(l_szCheckPath), pi_pPath);
  NormalizeTextureManagerPath(l_szCheckPath, sizeof(l_szCheckPath), l_szCheckPath);
  const DWORD l_dwAttributes = GetFileAttributesA(l_szCheckPath);
  return l_dwAttributes != INVALID_FILE_ATTRIBUTES && (l_dwAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool LoadBinaryFile(const char *pi_pFileName, std::vector<BYTE> &po_vecBuffer)
{
  FILE *l_pFile = NULL;
  if (fopen_s(&l_pFile, pi_pFileName, "rb") != 0 || !l_pFile)
  {
    return false;
  }

  fseek(l_pFile, 0, SEEK_END);
  const long l_nFileSize = ftell(l_pFile);
  fseek(l_pFile, 0, SEEK_SET);
  if (l_nFileSize <= 0)
  {
    fclose(l_pFile);
    return false;
  }

  po_vecBuffer.resize(static_cast<size_t>(l_nFileSize));
  const size_t l_stReadSize = fread(po_vecBuffer.data(), 1, po_vecBuffer.size(), l_pFile);
  fclose(l_pFile);
  return l_stReadSize == po_vecBuffer.size();
}

bool SaveBinaryFile(const char *pi_pFileName, const BYTE *pi_pBuffer, size_t pi_stBufferSize)
{
  if (!pi_pFileName || !pi_pBuffer || pi_stBufferSize == 0)
  {
    return false;
  }

  FILE *l_pFile = NULL;
  if (fopen_s(&l_pFile, pi_pFileName, "wb") != 0 || !l_pFile)
  {
    return false;
  }

  const size_t l_stWriteSize = fwrite(pi_pBuffer, 1, pi_stBufferSize, l_pFile);
  fclose(l_pFile);
  return l_stWriteSize == pi_stBufferSize;
}

bool PrepareDecodedTextureFile(const char *pi_pSourceFileName, const char *pi_pCacheFileName)
{
  if (!pi_pSourceFileName || !pi_pCacheFileName)
  {
    return false;
  }

  const char *l_pExt = strrchr(pi_pSourceFileName, '.');
  if (!l_pExt)
  {
    return false;
  }

  WIN32_FILE_ATTRIBUTE_DATA l_stSourceAttr;
  WIN32_FILE_ATTRIBUTE_DATA l_stCacheAttr;
  if (GetFileAttributesExA(pi_pSourceFileName, GetFileExInfoStandard, &l_stSourceAttr) &&
      GetFileAttributesExA(pi_pCacheFileName, GetFileExInfoStandard, &l_stCacheAttr) &&
      (l_stCacheAttr.nFileSizeHigh != 0 || l_stCacheAttr.nFileSizeLow != 0) &&
      CompareFileTime(&l_stCacheAttr.ftLastWriteTime, &l_stSourceAttr.ftLastWriteTime) >= 0)
  {
    return true;
  }

  if (_stricmp(l_pExt, ".dds") == 0)
  {
    return CopyFileA(pi_pSourceFileName, pi_pCacheFileName, FALSE) != FALSE ||
           GetLastError() == ERROR_SUCCESS;
  }

  if (_stricmp(l_pExt, ".rft") != 0)
  {
    return false;
  }

  std::vector<BYTE> l_vecBuffer;
  if (!LoadBinaryFile(pi_pSourceFileName, l_vecBuffer))
  {
    return false;
  }

  if (!DecodeDDSBuffer(l_vecBuffer.data(), static_cast<DWORD>(l_vecBuffer.size())))
  {
    return false;
  }

  return SaveBinaryFile(pi_pCacheFileName, l_vecBuffer.data(), l_vecBuffer.size());
}

bool BuildDecodedTextureCache(const char *pi_pTexturePath, const char *pi_pCacheTexturePath)
{
  if (!pi_pTexturePath || !pi_pCacheTexturePath || !EnsureDirectoryPath(pi_pCacheTexturePath))
  {
    return false;
  }

  char l_szSearchPattern[MAX_PATH];
  sprintf_s(l_szSearchPattern, sizeof(l_szSearchPattern), "%s*.*", pi_pTexturePath);

  WIN32_FIND_DATAA l_stFindData;
  HANDLE l_hFind = FindFirstFileA(l_szSearchPattern, &l_stFindData);
  if (l_hFind == INVALID_HANDLE_VALUE)
  {
    return false;
  }

  DWORD l_dwPreparedCount = 0;
  do
  {
    if ((l_stFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 ||
        !IsTextureFileNameLocal(l_stFindData.cFileName))
    {
      continue;
    }

    char l_szSourceFileName[MAX_PATH];
    char l_szCacheFileName[MAX_PATH];
    sprintf_s(l_szSourceFileName, sizeof(l_szSourceFileName), "%s%s", pi_pTexturePath, l_stFindData.cFileName);
    sprintf_s(l_szCacheFileName, sizeof(l_szCacheFileName), "%s%s", pi_pCacheTexturePath, l_stFindData.cFileName);
    if (PrepareDecodedTextureFile(l_szSourceFileName, l_szCacheFileName))
    {
      ++l_dwPreparedCount;
    }
  } while (FindNextFileA(l_hFind, &l_stFindData));

  FindClose(l_hFind);
#if defined(_DEBUG)
  AppendPlayerLog("BuildDecodedTextureCache: src=%s dst=%s prepared=%u",
                  pi_pTexturePath,
                  pi_pCacheTexturePath,
                  static_cast<unsigned>(l_dwPreparedCount));
#endif
  return l_dwPreparedCount != 0;
}

void CopyVector3(float *po_pfDst, const float *pi_pfSrc)
{
  po_pfDst[0] = pi_pfSrc[0];
  po_pfDst[1] = pi_pfSrc[1];
  po_pfDst[2] = pi_pfSrc[2];
}

bool IsExistingFile(const char *path)
{
  const DWORD l_dwAttributes = path ? GetFileAttributesA(path) : INVALID_FILE_ATTRIBUTES;
  return l_dwAttributes != INVALID_FILE_ATTRIBUTES &&
         (l_dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

bool HasLooseFileMatching(const char *pi_pPattern)
{
  if (!pi_pPattern || !pi_pPattern[0])
  {
    return false;
  }

  WIN32_FIND_DATAA l_sFindData;
  HANDLE l_hFind = FindFirstFileA(pi_pPattern, &l_sFindData);
  if (l_hFind == INVALID_HANDLE_VALUE)
  {
    return false;
  }

  bool l_bFound = false;
  do
  {
    if ((l_sFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
    {
      l_bFound = true;
      break;
    }
  } while (FindNextFileA(l_hFind, &l_sFindData));

  FindClose(l_hFind);
  return l_bFound;
}

bool HasLoosePlayerResourceFiles(void)
{
  return HasLooseFileMatching(".\\Character\\Player\\Mesh\\*.msh") &&
         HasLooseFileMatching(".\\Character\\Player\\Ani\\*.ANI") &&
         (HasLooseFileMatching(".\\Character\\Player\\Tex\\*.dds") ||
          HasLooseFileMatching(".\\Character\\Player\\Tex\\*.rft") ||
          HasLooseFileMatching(".\\Character\\Player\\Tex\\*.RFT"));
}

bool IsTextureFileNameLocal(const char *pi_pFileName)
{
  if (!pi_pFileName)
  {
    return false;
  }

  const char *l_pExt = strrchr(pi_pFileName, '.');
  if (!l_pExt)
  {
    return false;
  }

  return _stricmp(l_pExt, ".rft") == 0 || _stricmp(l_pExt, ".dds") == 0;
}

bool TryCopyCString(const char *pi_pSource, char *po_szBuffer, size_t pi_stBufferSize)
{
  if (!pi_pSource || !po_szBuffer || pi_stBufferSize < 2 ||
      pi_pSource == reinterpret_cast<const char *>(-1))
  {
    return false;
  }

  __try
  {
    size_t l_stLength = 0;
    while (l_stLength + 1 < pi_stBufferSize)
    {
      const unsigned char l_byChar = static_cast<unsigned char>(pi_pSource[l_stLength]);
      if (l_byChar == '\0')
      {
        po_szBuffer[l_stLength] = '\0';
        return l_stLength > 0;
      }

      if (l_byChar < 32 || l_byChar >= 127)
      {
        return false;
      }

      po_szBuffer[l_stLength] = static_cast<char>(l_byChar);
      ++l_stLength;
    }
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    return false;
  }

  return false;
}

void CopyTextureFileName(const char *pi_pTextureName, char *po_szFileName, size_t pi_stFileNameSize)
{
  if (!po_szFileName || pi_stFileNameSize == 0)
  {
    return;
  }

  po_szFileName[0] = '\0';
  if (!pi_pTextureName || !pi_pTextureName[0])
  {
    return;
  }

  const char *l_pFileName = pi_pTextureName;
  const char *l_pSlash = strrchr(pi_pTextureName, '\\');
  const char *l_pSlash2 = strrchr(pi_pTextureName, '/');
  if (l_pSlash2 && (!l_pSlash || l_pSlash2 > l_pSlash))
  {
    l_pSlash = l_pSlash2;
  }

  if (l_pSlash && l_pSlash[1])
  {
    l_pFileName = l_pSlash + 1;
  }

  strcpy_s(po_szFileName, pi_stFileNameSize, l_pFileName);
}

bool TryLoadTextureCandidate(ChInterface *pi_pCharIF,
                             TextureManager *pi_pTextureManager,
                             const char *pi_pTexturePath,
                             const char *pi_pTextureFileName)
{
  UNREFERENCED_PARAMETER(pi_pCharIF);

  if (!pi_pCharIF || !pi_pTextureManager || !pi_pTexturePath || !pi_pTextureFileName || !pi_pTextureFileName[0])
  {
    return false;
  }

  char l_szFullPath[MAX_PATH];
  sprintf_s(l_szFullPath, sizeof(l_szFullPath), "%s%s", pi_pTexturePath, pi_pTextureFileName);

  if (pi_pTextureManager->GetTextureID(pi_pTextureFileName) != 0)
  {
    return true;
  }

  const char *l_apTexturePath[2] = {pi_pTexturePath, NULL};
  if (_stricmp(pi_pTexturePath, kPlayerTexRFSPath) == 0)
  {
    l_apTexturePath[1] = kDecodedPlayerTexCachePath;
  }

  for (int l_nPathIndex = 0; l_nPathIndex < 2; ++l_nPathIndex)
  {
    if (!l_apTexturePath[l_nPathIndex])
    {
      continue;
    }

    char l_szManagerPath[MAX_PATH];
    NormalizeTextureManagerPath(l_szManagerPath, sizeof(l_szManagerPath), l_apTexturePath[l_nPathIndex]);

    char l_szCandidateFileName[MAX_PATH];
    sprintf_s(l_szCandidateFileName, sizeof(l_szCandidateFileName), "%s\\%s", l_szManagerPath, pi_pTextureFileName);
    if (!IsExistingFile(l_szCandidateFileName))
    {
      continue;
    }

    if (pi_pTextureManager->AddTexture(l_szManagerPath, pi_pTextureFileName))
    {
      return true;
    }

    pi_pCharIF->LoadTextureData(pi_pTextureManager,
                                const_cast<char *>(l_apTexturePath[l_nPathIndex]),
                                const_cast<char *>(pi_pTextureFileName));
    if (pi_pTextureManager->GetTextureID(pi_pTextureFileName) != 0)
    {
      return true;
    }
  }

  return pi_pTextureManager->GetTextureID(pi_pTextureFileName) != 0;
}

void EnsureMeshTexturesLoaded(ChInterface *pi_pCharIF,
                              TextureManager *pi_pTextureManager,
                              CHARACTEROBJECT *pi_pMesh,
                              const char *pi_pTexturePath)
{
  if (!pi_pCharIF || !pi_pTextureManager || !pi_pMesh || !pi_pMesh->m_pMesh || !pi_pTexturePath || !pi_pTexturePath[0])
  {
    return;
  }

  DWORD l_dwValidTextureRefCount = 0;
  BYTE *l_pObjectMesh = reinterpret_cast<BYTE *>(pi_pMesh->m_pMesh);
  for (DWORD l_dwObjectIndex = 0; l_dwObjectIndex < pi_pMesh->m_MaxObj; ++l_dwObjectIndex)
  {
    BYTE *l_pTextureInfo = l_pObjectMesh + kObjectMeshTextureOffset;
    char *l_apTextureName[3] =
    {
      *reinterpret_cast<char **>(l_pTextureInfo + 0x04),
      *reinterpret_cast<char **>(l_pTextureInfo + 0x0C),
      *reinterpret_cast<char **>(l_pTextureInfo + 0x14)
    };

    for (int l_nTextureIndex = 0; l_nTextureIndex < 3; ++l_nTextureIndex)
    {
      char l_szTextureName[MAX_PATH];
      if (!TryCopyCString(l_apTextureName[l_nTextureIndex],
                          l_szTextureName,
                          sizeof(l_szTextureName)))
      {
        continue;
      }

      ++l_dwValidTextureRefCount;

      if (pi_pTextureManager->GetTextureID(l_szTextureName) != 0)
      {
        continue;
      }

      char l_szTextureFileName[MAX_PATH];
      CopyTextureFileName(l_szTextureName, l_szTextureFileName, sizeof(l_szTextureFileName));
      if (!l_szTextureFileName[0])
      {
        continue;
      }

      bool l_bLoaded = TryLoadTextureCandidate(pi_pCharIF,
                                               pi_pTextureManager,
                                               pi_pTexturePath,
                                               l_szTextureFileName);

      if (!l_bLoaded)
      {
        char *l_pExt = strrchr(l_szTextureFileName, '.');
        if (l_pExt)
        {
          strcpy_s(l_pExt, sizeof(l_szTextureFileName) - static_cast<size_t>(l_pExt - l_szTextureFileName), ".RFT");
          l_bLoaded = TryLoadTextureCandidate(pi_pCharIF,
                                              pi_pTextureManager,
                                              pi_pTexturePath,
                                              l_szTextureFileName);
        }
      }

#if defined(_DEBUG)
      AppendPlayerLog("EnsureMeshTexturesLoaded: obj=%u texSlot=%d name=%s loaded=%d texId=%u",
                      static_cast<unsigned>(l_dwObjectIndex),
                      l_nTextureIndex,
                      l_szTextureName,
                      l_bLoaded ? 1 : 0,
                      static_cast<unsigned>(pi_pTextureManager->GetTextureID(l_szTextureName)));
#endif
    }

    l_pObjectMesh += kObjectMeshStride;
  }

#if defined(_DEBUG)
  AppendPlayerLog("EnsureMeshTexturesLoaded: mesh=%p maxObj=%u validRefs=%u",
                  pi_pMesh,
                  static_cast<unsigned>(pi_pMesh->m_MaxObj),
                  static_cast<unsigned>(l_dwValidTextureRefCount));
#endif
}

#if defined(_DEBUG)
void AppendPlayerLog(const char *format, ...)
{
  HANDLE outputFile = CreateFileA(kDebugCaptureLogPath,
                                  FILE_APPEND_DATA,
                                  FILE_SHARE_READ,
                                  NULL,
                                  OPEN_ALWAYS,
                                  FILE_ATTRIBUTE_NORMAL,
                                  NULL);
  if (outputFile == INVALID_HANDLE_VALUE)
  {
    return;
  }

  char buffer[1024];
  va_list args;
  va_start(args, format);
  int writtenLength = _vsnprintf(buffer, sizeof(buffer) - 3, format, args);
  va_end(args);

  if (writtenLength < 0)
  {
    writtenLength = static_cast<int>(sizeof(buffer) - 3);
  }
  buffer[writtenLength++] = '\r';
  buffer[writtenLength++] = '\n';
  buffer[writtenLength] = '\0';

  DWORD bytesWritten = 0;
  WriteFile(outputFile, buffer, static_cast<DWORD>(writtenLength), &bytesWritten, NULL);
  CloseHandle(outputFile);
}

bool IsPrintableAsciiChar(unsigned char value)
{
  return value >= 32 && value < 127;
}

void AppendPlayerSymbolInfo(const void *pi_pAddress)
{
  if (!pi_pAddress)
  {
    return;
  }

  HANDLE l_hProcess = GetCurrentProcess();

  static bool s_bSymbolsInitialized = false;
  if (!s_bSymbolsInitialized)
  {
    SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
    s_bSymbolsInitialized = (SymInitialize(l_hProcess, NULL, TRUE) == TRUE);
    AppendPlayerLog("Symbols: initialized=%d moduleBase=%p",
                    s_bSymbolsInitialized ? 1 : 0,
                    GetModuleHandleA(NULL));
  }

  if (!s_bSymbolsInitialized)
  {
    return;
  }

  DWORD64 l_dwDisplacement64 = 0;
  BYTE l_abySymbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME] = {};
  SYMBOL_INFO *l_pSymbol = reinterpret_cast<SYMBOL_INFO *>(l_abySymbolBuffer);
  l_pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
  l_pSymbol->MaxNameLen = MAX_SYM_NAME;

  if (SymFromAddr(l_hProcess,
                  reinterpret_cast<DWORD64>(pi_pAddress),
                  &l_dwDisplacement64,
                  l_pSymbol))
  {
    AppendPlayerLog("Symbols: %p => %s + 0x%I64X",
                    pi_pAddress,
                    l_pSymbol->Name,
                    l_dwDisplacement64);
  }

  IMAGEHLP_LINE64 l_stLine = {};
  l_stLine.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
  DWORD l_dwDisplacement32 = 0;
  if (SymGetLineFromAddr64(l_hProcess,
                           reinterpret_cast<DWORD64>(pi_pAddress),
                           &l_dwDisplacement32,
                           &l_stLine))
  {
    AppendPlayerLog("Symbols: line %s:%lu + 0x%X",
                    l_stLine.FileName,
                    l_stLine.LineNumber,
                    static_cast<unsigned>(l_dwDisplacement32));
  }

  const DWORD64 l_dwModuleBase =
    SymGetModuleBase64(l_hProcess, reinterpret_cast<DWORD64>(pi_pAddress));
  if (l_dwModuleBase != 0)
  {
    AppendPlayerLog("Symbols: moduleBase=%p rva=0x%I64X",
                    reinterpret_cast<const void *>(l_dwModuleBase),
                    reinterpret_cast<DWORD64>(pi_pAddress) - l_dwModuleBase);
  }
}

int LogPlayerException(const char *pi_pLabel, EXCEPTION_POINTERS *pi_pExceptionPointers)
{
  DWORD l_dwCode = 0;
  const void *l_pAddress = NULL;
  ULONG_PTR l_nAccessType = 0;
  ULONG_PTR l_pViolationAddress = 0;

  if (pi_pExceptionPointers && pi_pExceptionPointers->ExceptionRecord)
  {
    l_dwCode = pi_pExceptionPointers->ExceptionRecord->ExceptionCode;
    l_pAddress = pi_pExceptionPointers->ExceptionRecord->ExceptionAddress;
    if (l_dwCode == EXCEPTION_ACCESS_VIOLATION &&
        pi_pExceptionPointers->ExceptionRecord->NumberParameters >= 2)
    {
      l_nAccessType = pi_pExceptionPointers->ExceptionRecord->ExceptionInformation[0];
      l_pViolationAddress = pi_pExceptionPointers->ExceptionRecord->ExceptionInformation[1];
    }
  }

  AppendPlayerLog("%s exception code=0x%08X address=%p accessType=%p violationAddress=%p",
                  pi_pLabel ? pi_pLabel : "SEH",
                  static_cast<unsigned>(l_dwCode),
                  l_pAddress,
                  reinterpret_cast<const void *>(l_nAccessType),
                  reinterpret_cast<const void *>(l_pViolationAddress));
  AppendPlayerSymbolInfo(l_pAddress);
  return EXCEPTION_EXECUTE_HANDLER;
}

bool TryReadDebugString(const char *pi_pCandidate, char *po_szBuffer, size_t pi_stBufferSize)
{
  if (!pi_pCandidate || !po_szBuffer || pi_stBufferSize < 2)
  {
    return false;
  }

  __try
  {
    size_t l_stLength = 0;
    while (l_stLength + 1 < pi_stBufferSize)
    {
      const unsigned char l_byChar = static_cast<unsigned char>(pi_pCandidate[l_stLength]);
      if (l_byChar == '\0')
      {
        po_szBuffer[l_stLength] = '\0';
        return l_stLength >= 4;
      }

      if (!IsPrintableAsciiChar(l_byChar))
      {
        return false;
      }

      po_szBuffer[l_stLength] = static_cast<char>(l_byChar);
      ++l_stLength;
    }
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    return false;
  }

  return false;
}

void DumpCharacterObjectDebug(const char *pi_pLabel, CHARACTEROBJECT *pi_pCharacterObject)
{
  if (!pi_pLabel || !pi_pCharacterObject)
  {
    return;
  }

  AppendPlayerLog("%s begin obj=%p", pi_pLabel, pi_pCharacterObject);

  const BYTE *l_pBytes = reinterpret_cast<const BYTE *>(pi_pCharacterObject);
  for (size_t l_stOffset = 0; l_stOffset + sizeof(DWORD) <= sizeof(CHARACTEROBJECT); l_stOffset += sizeof(DWORD))
  {
    const DWORD l_dwValue = *reinterpret_cast<const DWORD *>(l_pBytes + l_stOffset);
    char l_szBuffer[128];
    if (TryReadDebugString(reinterpret_cast<const char *>(l_dwValue), l_szBuffer, sizeof(l_szBuffer)))
    {
      if (strstr(l_szBuffer, "MSH") ||
          strstr(l_szBuffer, "BN") ||
          strstr(l_szBuffer, "ANI") ||
          strstr(l_szBuffer, "CHARACTER") ||
          strstr(l_szBuffer, "PLAYER") ||
          strstr(l_szBuffer, "TEX") ||
          strstr(l_szBuffer, "RFT"))
      {
        AppendPlayerLog("%s ptr[%03X]=%08X str=%s",
                        pi_pLabel,
                        static_cast<unsigned>(l_stOffset),
                        static_cast<unsigned>(l_dwValue),
                        l_szBuffer);
      }
    }
  }

  for (size_t l_stOffset = 0; l_stOffset < sizeof(CHARACTEROBJECT); ++l_stOffset)
  {
    if (!IsPrintableAsciiChar(l_pBytes[l_stOffset]))
    {
      continue;
    }

    char l_szBuffer[128];
    size_t l_stLength = 0;
    while ((l_stOffset + l_stLength) < sizeof(CHARACTEROBJECT) &&
           l_stLength + 1 < sizeof(l_szBuffer) &&
           IsPrintableAsciiChar(l_pBytes[l_stOffset + l_stLength]))
    {
      l_szBuffer[l_stLength] = static_cast<char>(l_pBytes[l_stOffset + l_stLength]);
      ++l_stLength;
    }

    if ((l_stOffset + l_stLength) >= sizeof(CHARACTEROBJECT) ||
        l_stLength < 4 ||
        l_pBytes[l_stOffset + l_stLength] != '\0')
    {
      continue;
    }

    l_szBuffer[l_stLength] = '\0';
    if (strstr(l_szBuffer, "MSH") ||
        strstr(l_szBuffer, "BN") ||
        strstr(l_szBuffer, "ANI") ||
        strstr(l_szBuffer, "CHARACTER") ||
        strstr(l_szBuffer, "PLAYER") ||
        strstr(l_szBuffer, "TEX") ||
        strstr(l_szBuffer, "RFT"))
    {
      AppendPlayerLog("%s bytes[%03X]=%s",
                      pi_pLabel,
                      static_cast<unsigned>(l_stOffset),
                      l_szBuffer);
    }

    l_stOffset += l_stLength;
  }

  AppendPlayerLog("%s end", pi_pLabel);
}
#endif
}

CPlayer::CPlayer()
  : m_pStandAni(NULL),
    m_pWalkAni(NULL),
    m_pRunAni(NULL),
    m_pBwWalkAni(NULL),
    m_pLfWalkAni(NULL),
    m_pRtWalkAni(NULL),
    m_pBwRunAni(NULL),
    m_pLfRunAni(NULL),
    m_pRtRunAni(NULL),
    m_pCurAni(NULL),
    m_bInitialized(false),
    m_bLoaded(false),
    m_bMountedRFS(false),
    m_bPreparedTextureCache(false),
    m_bUseBoneRender(false),
    m_byWalkMode(CMM_MOVE_RUN)
{
  ZeroMemory(m_pMesh, sizeof(m_pMesh));
  ZeroMemory(m_szBoneName, sizeof(m_szBoneName));
  ZeroMemory(m_szBBoxName, sizeof(m_szBBoxName));
  ZeroMemory(m_szAniName, sizeof(m_szAniName));
  ZeroMemory(m_szLastTextureLoadPath, sizeof(m_szLastTextureLoadPath));
  ZeroMemory(m_szMeshName, sizeof(m_szMeshName));
  SetObjectTypeID(CTI_PLAYER);
  SetCharTypeID(CTI_PLAYER);
  SetRotY(180.0f);
  Enable(ROSF_RENDER_MESH | ROSF_RENDER_SHADOW);
}

CPlayer::~CPlayer()
{
  Shutdown();
}

bool CPlayer::Initialize(void)
{
  if (m_bInitialized)
  {
    return true;
  }

  m_bInitialized = (CCharacterMgr::GetCharIF() != NULL) &&
                   (CCharacterMgr::GetAnimationMgr() != NULL) &&
                   (CCharacterMgr::GetTextureMgr() != NULL) &&
                   (CCharacterMgr::GetBoneMgr(CTI_PLAYER) != NULL) &&
                   (CCharacterMgr::GetMeshMgr(CTI_PLAYER) != NULL);
  return m_bInitialized;
}

void CPlayer::Shutdown()
{
  m_bLoaded = false;
  m_pStandAni = NULL;
  m_pWalkAni = NULL;
  m_pRunAni = NULL;
  m_pBwWalkAni = NULL;
  m_pLfWalkAni = NULL;
  m_pRtWalkAni = NULL;
  m_pBwRunAni = NULL;
  m_pLfRunAni = NULL;
  m_pRtRunAni = NULL;
  m_pCurAni = NULL;
  SetBonePtr(NULL);
  m_bUseBoneRender = false;
  m_dwBoneFrame = 160;
  m_dwOldBoneFrame = 0;
  m_byWalkMode = CMM_MOVE_RUN;
  m_bInitialized = false;
  m_bMountedRFS = false;
  m_bPreparedTextureCache = false;
  ZeroMemory(m_pMesh, sizeof(m_pMesh));
  ZeroMemory(m_szLastTextureLoadPath, sizeof(m_szLastTextureLoadPath));
}

bool CPlayer::LoadAnimation(char *pi_pAniName, ChAnimation **po_ppAnimation)
{
  ChInterface *l_pCharIF = CCharacterMgr::GetCharIF();
  AnimationManager *l_pAnimationMgr = CCharacterMgr::GetAnimationMgr();
  if (!pi_pAniName || !pi_pAniName[0] || !po_ppAnimation || !l_pCharIF || !l_pAnimationMgr)
  {
    return false;
  }

  ChAnimation *l_pAnimation = l_pCharIF->AddAnimationData(l_pAnimationMgr, pi_pAniName);
  if (!l_pAnimation)
  {
    l_pAnimation = l_pCharIF->GetAnimationData(l_pAnimationMgr, pi_pAniName);
  }

  if (!l_pAnimation)
  {
#if defined(_DEBUG)
    AppendPlayerLog("LoadAnimation: GetAnimationData failed %s", pi_pAniName);
#endif
    return false;
  }

  bool l_bLoaded = false;
  __try
  {
    l_bLoaded = l_pAnimation->LoadRealAnimation(true);
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
#if defined(_DEBUG)
    AppendPlayerLog("LoadAnimation: LoadRealAnimation raised exception code=0x%08X ani=%s",
                    GetExceptionCode(),
                    pi_pAniName);
#endif
    l_bLoaded = false;
  }

  if (!l_bLoaded)
  {
#if defined(_DEBUG)
    AppendPlayerLog("LoadAnimation: LoadRealAnimation failed %s", pi_pAniName);
#endif
    return false;
  }

  *po_ppAnimation = l_pAnimation;
#if defined(_DEBUG)
  AppendPlayerLog("LoadAnimation: success %s result=%p", pi_pAniName, l_pAnimation);
#endif
  return true;
}

bool CPlayer::SetAnimation(ChAnimation *pi_pAnimation)
{
  ChInterface *l_pCharIF = CCharacterMgr::GetCharIF();
  if (!pi_pAnimation || !m_pBone || !l_pCharIF)
  {
    return false;
  }

  if (m_pCurAni == pi_pAnimation)
  {
    return true;
  }

  if (!l_pCharIF->IsLoadAnimation(pi_pAnimation))
  {
#if defined(_DEBUG)
    AppendPlayerLog("SetAnimation: animation not loaded ani=%p", pi_pAnimation);
#endif
    return false;
  }

  __try
  {
    l_pCharIF->AnimationReset(m_pBone);
    l_pCharIF->MatchAnimationToMesh(pi_pAnimation, m_pBone);
    m_dwBoneFrame = 160;
    m_dwOldBoneFrame = 0;
    m_pBone->m_Transition = FALSE;
    m_pBone->m_TransFrame = 0;
    m_pBone->m_TransTime = 0;
    m_pBone->m_Frame = m_dwBoneFrame;
    l_pCharIF->FrameMove(m_pBone);
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
#if defined(_DEBUG)
    AppendPlayerLog("SetAnimation: MatchAnimationToMesh raised exception code=0x%08X ani=%p",
                    GetExceptionCode(),
                    pi_pAnimation);
#endif
    return false;
  }

  m_pCurAni = pi_pAnimation;
  m_bUseBoneRender = true;
#if defined(_DEBUG)
  AppendPlayerLog("SetAnimation: success ani=%p", pi_pAnimation);
#endif
  return true;
}

bool CPlayer::LoadBone(const BONE_DATA &pi_stBoneData)
{
  ChInterface *l_pCharIF = CCharacterMgr::GetCharIF();
  CObjectManager *l_pBoneMgr = CCharacterMgr::GetBoneMgr(CTI_PLAYER);
  if (!l_pCharIF || !l_pBoneMgr)
  {
    return false;
  }

  sprintf_s(m_szBoneName, sizeof(m_szBoneName), "%s%s", pi_stBoneData.pPathName, pi_stBoneData.pFileName);
  SetBonePtr(NULL);
  CHARACTEROBJECT *l_pStoredBone = NULL;
  __try
  {
    l_pCharIF->LoadMeshData(l_pBoneMgr, m_szBoneName, true, NULL);
    l_pStoredBone = l_pBoneMgr->GetCharacter(m_szBoneName);
    m_pBone = l_pCharIF->GetMeshData(l_pBoneMgr, m_szBoneName);
  }
  __except (LogPlayerException("LoadBone", GetExceptionInformation()))
  {
    l_pStoredBone = NULL;
    m_pBone = NULL;
  }

  if (!m_pBone && l_pStoredBone)
  {
    bool l_bBoneRealLoaded = false;
    __try
    {
      l_bBoneRealLoaded = l_pCharIF->LoadRealData(l_pStoredBone, &l_pCharIF->m_TM, true);
    }
    __except (LogPlayerException("LoadBone: LoadRealData", GetExceptionInformation()))
    {
      l_bBoneRealLoaded = false;
    }

    if (l_bBoneRealLoaded)
    {
      m_pBone = l_pStoredBone;
    }
  }

  SetBonePtr(m_pBone);
  if (!m_pBone)
  {
    return false;
  }

  sprintf_s(m_szBBoxName, sizeof(m_szBBoxName), "%s%s", pi_stBoneData.pPathName, pi_stBoneData.pBBoxName);
  if (!l_pCharIF->ImportBoundBox(m_szBBoxName, m_vecBBoxMin, m_vecBBoxMax) ||
      !IsValidBBoxHeight(m_vecBBoxMax[1] - m_vecBBoxMin[1]))
  {
    m_vecBBoxMin[0] = -20.0f;
    m_vecBBoxMin[1] = 0.0f;
    m_vecBBoxMin[2] = -20.0f;
    m_vecBBoxMax[0] = 20.0f;
    m_vecBBoxMax[1] = 150.0f;
    m_vecBBoxMax[2] = 20.0f;
  }

  return true;
}

bool CPlayer::LoadAccretia()
{
#if defined(_DEBUG)
  AppendPlayerLog("LoadAccretia: begin initialized=%d loaded=%d", m_bInitialized ? 1 : 0, m_bLoaded ? 1 : 0);
#endif
  if (!m_bInitialized && !Initialize())
  {
    return false;
  }

  if (m_bLoaded)
  {
    return true;
  }

  if (!MountRFS())
  {
#if defined(_DEBUG)
    AppendPlayerLog("LoadAccretia: MountRFS failed");
#endif
    return false;
  }

  BONE_DATA l_stBoneData = {};
  MESH_DATA l_astMeshData[MAX_DEFAULT_PART] = {};
  ANI_DATA l_stAniData = {};
  ChInterface *l_pCharIF = CCharacterMgr::GetCharIF();
  CObjectManager *l_pBoneMgr = CCharacterMgr::GetBoneMgr(CTI_PLAYER);
  if (!l_pCharIF || !l_pBoneMgr || !CCharacterMgr::GetMeshMgr(CTI_PLAYER))
  {
    return false;
  }

  if (!LoadResourceData(l_stBoneData, l_astMeshData, l_stAniData))
  {
#if defined(_DEBUG)
    AppendPlayerLog("LoadAccretia: LoadResourceData failed");
#endif
    return false;
  }
#if defined(_DEBUG)
  AppendPlayerLog("LoadAccretia: resource bone=%s%s ani=%s%s",
                  l_stBoneData.pPathName,
                  l_stBoneData.pFileName,
                  l_stAniData.pPathName,
                  l_stAniData.pFileName);
#endif

  sprintf_s(m_szBoneName, sizeof(m_szBoneName), "%s%s", l_stBoneData.pPathName, l_stBoneData.pFileName);
#if defined(_DEBUG)
  AppendPlayerLog("LoadAccretia: before AddBone %s", m_szBoneName);
#endif
  SetBonePtr(NULL);
  CHARACTEROBJECT *l_pStoredBone = NULL;
  __try
  {
    CHARACTEROBJECT *l_pLoadedBone = l_pCharIF->LoadMeshData(l_pBoneMgr,
                                                             m_szBoneName,
                                                             true,
                                                             NULL);
#if defined(_DEBUG)
    AppendPlayerLog("LoadAccretia: LoadMeshData(bone,true) result=%p", l_pLoadedBone);
#endif
  }
  __except (LogPlayerException("LoadAccretia: LoadMeshData(bone,true)", GetExceptionInformation()))
  {
  }

  __try
  {
    l_pStoredBone = l_pBoneMgr->GetCharacter(m_szBoneName);
#if defined(_DEBUG)
    AppendPlayerLog("LoadAccretia: bone mgr GetCharacter result=%p", l_pStoredBone);
#endif
  }
  __except (LogPlayerException("LoadAccretia: bone mgr GetCharacter", GetExceptionInformation()))
  {
    l_pStoredBone = NULL;
  }

  __try
  {
    m_pBone = l_pCharIF->GetMeshData(l_pBoneMgr, m_szBoneName);
#if defined(_DEBUG)
    AppendPlayerLog("LoadAccretia: GetMeshData(bone) result=%p", m_pBone);
#endif
  }
  __except (LogPlayerException("LoadAccretia: GetMeshData(bone)", GetExceptionInformation()))
  {
    m_pBone = NULL;
  }

  if (!m_pBone && l_pStoredBone)
  {
    bool l_bBoneRealLoaded = false;
    __try
    {
      l_bBoneRealLoaded = l_pCharIF->LoadRealData(l_pStoredBone,
                                                  &l_pCharIF->m_TM,
                                                  true);
#if defined(_DEBUG)
      AppendPlayerLog("LoadAccretia: fallback LoadRealData bone m_TM true result=%d",
                      l_bBoneRealLoaded ? 1 : 0);
#endif
    }
    __except (LogPlayerException("LoadAccretia: fallback LoadRealData bone m_TM true",
                                 GetExceptionInformation()))
    {
      l_bBoneRealLoaded = false;
    }

    if (l_bBoneRealLoaded)
    {
      m_pBone = l_pStoredBone;
    }
  }

  if (!m_pBone)
  {
#if defined(_DEBUG)
    AppendPlayerLog("LoadAccretia: GetBone failed %s", m_szBoneName);
#endif
  }
  SetBonePtr(m_pBone);
#if defined(_DEBUG)
  AppendPlayerLog("LoadAccretia: after AddBone %p", m_pBone);
  if (m_pBone)
  {
    DumpCharacterObjectDebug("LoadAccretia: bone after GetMeshData", m_pBone);
  }
#endif
  if (!m_pBone)
  {
#if defined(_DEBUG)
    AppendPlayerLog("LoadAccretia: AddBone failed %s", m_szBoneName);
#endif
    return false;
  }

  sprintf_s(m_szBBoxName, sizeof(m_szBBoxName), "%s%s", l_stBoneData.pPathName, l_stBoneData.pBBoxName);
#if defined(_DEBUG)
  AppendPlayerLog("LoadAccretia: before ImportBoundBox %s", m_szBBoxName);
#endif
  if (!l_pCharIF->ImportBoundBox(m_szBBoxName, m_vecBBoxMin, m_vecBBoxMax) ||
      !IsValidBBoxHeight(m_vecBBoxMax[1] - m_vecBBoxMin[1]))
  {
    m_vecBBoxMin[0] = -20.0f;
    m_vecBBoxMin[1] = 0.0f;
    m_vecBBoxMin[2] = -20.0f;
    m_vecBBoxMax[0] = 20.0f;
      m_vecBBoxMax[1] = 150.0f;
      m_vecBBoxMax[2] = 20.0f;
  }
#if defined(_DEBUG)
  AppendPlayerLog("LoadAccretia: after ImportBoundBox min=(%.2f, %.2f, %.2f) max=(%.2f, %.2f, %.2f)",
                  m_vecBBoxMin[0],
                  m_vecBBoxMin[1],
                  m_vecBBoxMin[2],
                  m_vecBBoxMax[0],
                  m_vecBBoxMax[1],
                  m_vecBBoxMax[2]);
#endif

  for (DWORD i = 0; i < MAX_PLAYER_RENDER_PART; ++i)
  {
#if defined(_DEBUG)
    AppendPlayerLog("LoadAccretia: before LoadPart index=%u mesh=%s%s",
                    static_cast<unsigned>(i),
                    l_astMeshData[i].pPathName,
                    l_astMeshData[i].pFileName);
#endif
    if (!LoadPart(i, l_astMeshData[i]))
    {
#if defined(_DEBUG)
      AppendPlayerLog("LoadAccretia: LoadPart failed index=%u mesh=%s%s",
                      static_cast<unsigned>(i),
                      l_astMeshData[i].pPathName,
                      l_astMeshData[i].pFileName);
#endif
      return false;
    }
#if defined(_DEBUG)
    AppendPlayerLog("LoadAccretia: after LoadPart index=%u meshPtr=%p",
                    static_cast<unsigned>(i),
                    m_pMesh[i]);
#endif
  }

  sprintf_s(m_szAniName, sizeof(m_szAniName), "%s%s", l_stAniData.pPathName, l_stAniData.pFileName);
  char l_szWalkAniName[MAX_PATH];
  char l_szRunAniName[MAX_PATH];
  char l_szBwWalkAniName[MAX_PATH];
  char l_szLfWalkAniName[MAX_PATH];
  char l_szRtWalkAniName[MAX_PATH];
  char l_szBwRunAniName[MAX_PATH];
  char l_szLfRunAniName[MAX_PATH];
  char l_szRtRunAniName[MAX_PATH];
  sprintf_s(l_szWalkAniName, sizeof(l_szWalkAniName), "%s%s", l_stAniData.pPathName, kAccretiaWalkAniName);
  sprintf_s(l_szRunAniName, sizeof(l_szRunAniName), "%s%s", l_stAniData.pPathName, kAccretiaRunAniName);
  sprintf_s(l_szBwWalkAniName, sizeof(l_szBwWalkAniName), "%s%s", l_stAniData.pPathName, kAccretiaBwWalkAniName);
  sprintf_s(l_szLfWalkAniName, sizeof(l_szLfWalkAniName), "%s%s", l_stAniData.pPathName, kAccretiaLfWalkAniName);
  sprintf_s(l_szRtWalkAniName, sizeof(l_szRtWalkAniName), "%s%s", l_stAniData.pPathName, kAccretiaRtWalkAniName);
  sprintf_s(l_szBwRunAniName, sizeof(l_szBwRunAniName), "%s%s", l_stAniData.pPathName, kAccretiaBwRunAniName);
  sprintf_s(l_szLfRunAniName, sizeof(l_szLfRunAniName), "%s%s", l_stAniData.pPathName, kAccretiaLfRunAniName);
  sprintf_s(l_szRtRunAniName, sizeof(l_szRtRunAniName), "%s%s", l_stAniData.pPathName, kAccretiaRtRunAniName);

  if (!LoadAnimation(m_szAniName, &m_pStandAni) ||
      !LoadAnimation(l_szWalkAniName, &m_pWalkAni) ||
      !LoadAnimation(l_szRunAniName, &m_pRunAni) ||
      !LoadAnimation(l_szBwWalkAniName, &m_pBwWalkAni) ||
      !LoadAnimation(l_szLfWalkAniName, &m_pLfWalkAni) ||
      !LoadAnimation(l_szRtWalkAniName, &m_pRtWalkAni) ||
      !LoadAnimation(l_szBwRunAniName, &m_pBwRunAni) ||
      !LoadAnimation(l_szLfRunAniName, &m_pLfRunAni) ||
      !LoadAnimation(l_szRtRunAniName, &m_pRtRunAni))
  {
    return false;
  }

  m_bUseBoneRender = SetAnimation(m_pStandAni);
#if defined(_DEBUG)
  AppendPlayerLog("LoadAccretia: bone animation ready=%d stand=%p walk=%p run=%p current=%p",
                  m_bUseBoneRender ? 1 : 0,
                  m_pStandAni,
                  m_pWalkAni,
                  m_pRunAni,
                  m_pCurAni);
#endif

  for (DWORD i = 0; i < MAX_PLAYER_RENDER_PART; ++i)
  {
    if (m_pMesh[i])
    {
#if defined(_DEBUG)
      AppendPlayerLog("LoadAccretia: before FrameMove mesh index=%u ptr=%p",
                      static_cast<unsigned>(i),
                      m_pMesh[i]);
#endif
      l_pCharIF->FrameMove(m_pMesh[i]);
#if defined(_DEBUG)
      AppendPlayerLog("LoadAccretia: after FrameMove mesh index=%u", static_cast<unsigned>(i));
#endif
    }
  }

  m_bLoaded = true;
#if defined(_DEBUG)
  AppendPlayerLog("LoadAccretia: success");
#endif
  return true;
}

bool CPlayer::LoadRegedAvatar(const _REGED_AVATOR_DB &pi_stRegedAvatar)
{
#if defined(_DEBUG)
  AppendPlayerLog("LoadRegedAvatar: begin slot=%u race=%u base=0x%08X",
                  static_cast<unsigned>(pi_stRegedAvatar.m_bySlotIndex),
                  static_cast<unsigned>(pi_stRegedAvatar.m_byRaceSexCode),
                  pi_stRegedAvatar.m_dwBaseShape);
#endif
  Shutdown();
  if (!m_bInitialized && !Initialize())
  {
    return false;
  }

  if (!MountRFS())
  {
    return false;
  }

  const BYTE l_byRaceSexCode = pi_stRegedAvatar.m_byRaceSexCode;
  if (l_byRaceSexCode >= 5)
  {
    return false;
  }

  CCharResDataMgr *l_pCharResDataMgr = _GetCharResDataMgr();
  CCharResData *l_pPlayerResData = l_pCharResDataMgr ? l_pCharResDataMgr->GetResourceList(RLI_PLAYER) : NULL;
  if (!l_pPlayerResData)
  {
    return false;
  }

  BONE_DATA *l_pBoneData = l_pPlayerResData->GetBoneData(l_byRaceSexCode);
  if (!l_pBoneData || !LoadBone(*l_pBoneData))
  {
    return false;
  }

  DWORD l_adwDefaultVariant[MAX_DEFAULT_PART] = {};
  l_adwDefaultVariant[CDPT_FACE] = (pi_stRegedAvatar.m_dwBaseShape >> 20) & 0x0F;
  for (BYTE i = 0; i < kBaseFixNum; ++i)
  {
    l_adwDefaultVariant[kServerBasePartToClientPart[i]] =
      (pi_stRegedAvatar.m_dwBaseShape >> (i * 4)) & 0x0F;
  }

  bool l_bLoadedAnyPart = false;
  CItemDataMgr *l_pItemDataMgr = _GetItemDataMgr();
  for (BYTE i = 0; i < MAX_DEFAULT_PART; ++i)
  {
    BYTE l_byItemType = IEPT_FACE;
    DWORD l_dwItemDTIndex = MakeDefaultItemDTIndex(l_byRaceSexCode, l_adwDefaultVariant[i]);
    bool l_bUseEquippedMesh = false;

    for (BYTE j = 0; j < kBaseFixNum; ++j)
    {
      if (kServerBasePartToClientPart[j] == i &&
          pi_stRegedAvatar.m_EquipKey[j].zItemIndex != -1)
      {
        l_byItemType = kServerBasePartToItemType[j];
        l_dwItemDTIndex = static_cast<DWORD>(pi_stRegedAvatar.m_EquipKey[j].zItemIndex);
        l_bUseEquippedMesh = true;
        break;
      }
    }

    if (!l_bUseEquippedMesh && i != CDPT_FACE)
    {
      for (BYTE j = 0; j < kBaseFixNum; ++j)
      {
        if (kServerBasePartToClientPart[j] == i)
        {
          l_byItemType = kServerBasePartToItemType[j];
          break;
        }
      }
    }

    DWORD l_dwMeshID = ID_INVALID;
    if (l_pItemDataMgr)
    {
      l_dwMeshID = l_pItemDataMgr->GetClientMeshID(l_byItemType, l_dwItemDTIndex);
      if (l_dwMeshID != ID_INVALID)
      {
        l_dwMeshID = ApplyPlayerRaceToItemMeshID(l_dwMeshID, l_byRaceSexCode);
      }
    }

    if (l_dwMeshID == ID_INVALID)
    {
      l_dwMeshID = MakePlayerMeshID(l_byRaceSexCode,
                                    i,
                                    l_adwDefaultVariant[i],
                                    !l_bUseEquippedMesh);
    }

    MESH_DATA *l_pMeshData = l_pPlayerResData->GetMeshData(l_dwMeshID);
    if (!l_pMeshData && l_bUseEquippedMesh)
    {
      const DWORD l_dwFallbackMeshID =
        MakePlayerMeshID(l_byRaceSexCode, i, l_adwDefaultVariant[i], true);
      l_pMeshData = l_pPlayerResData->GetMeshData(l_dwFallbackMeshID);
    }
    if (!l_pMeshData && l_adwDefaultVariant[i] != 0)
    {
      const DWORD l_dwFallbackMeshID = MakePlayerMeshID(l_byRaceSexCode, i, 0, true);
      l_pMeshData = l_pPlayerResData->GetMeshData(l_dwFallbackMeshID);
    }

    if (l_pMeshData && LoadPart(i, RLI_ITEM, *l_pMeshData))
    {
      l_bLoadedAnyPart = true;
    }
  }

  static const char *kRaceStandAniName[5] =
  {
    "BELMALE_PEACE_STAND_NONE_NONE_01_00.ANI",
    "BELFEMALE_PEACE_STAND_NONE_NONE_01_00.ANI",
    "CORMALE_PEACE_STAND_NONE_NONE_01_00.ANI",
    "CORFEMALE_PEACE_STAND_NONE_NONE_01_00.ANI",
    "ACCRETIA_PEACE_STAND_NONE_NONE_01_00.ANI"
  };

  static const char *kRaceLobbyIdleAniName[5] =
  {
    "BELMALE_COMMON_MOUNTIDLE_NONE_NONE_01_00.ANI",
    "BELFEMALE_COMMON_MOUNTIDLE_NONE_NONE_01_00.ANI",
    "CORMALE_COMMON_MOUNTIDLE_NONE_NONE_01_00.ANI",
    "CORFEMALE_COMMON_MOUNTIDLE_NONE_NONE_01_00.ANI",
    "ACCRETIA_COMMON_MOUNTIDLE_NONE_NONE_01_00.ANI"
  };

#if defined(_DEBUG)
  AppendPlayerLog("LoadRegedAvatar: parts loaded any=%d", l_bLoadedAnyPart ? 1 : 0);
#endif

  const char *l_apStandCandidates[2] =
  {
    kRaceStandAniName[l_byRaceSexCode],
    kRaceLobbyIdleAniName[l_byRaceSexCode]
  };

  for (DWORD i = 0; i < 2 && !m_pStandAni; ++i)
  {
    char l_szAniCandidate[MAX_PATH];
    sprintf_s(l_szAniCandidate,
              sizeof(l_szAniCandidate),
              ".\\CHARACTER\\PLAYER\\ANI\\%s",
              l_apStandCandidates[i]);

    if (!IsExistingFile(l_szAniCandidate))
    {
#if defined(_DEBUG)
      AppendPlayerLog("LoadRegedAvatar: lobby ani missing %s", l_szAniCandidate);
#endif
      continue;
    }

#if defined(_DEBUG)
    AppendPlayerLog("LoadRegedAvatar: lobby ani load begin %s", l_szAniCandidate);
#endif
    strcpy_s(m_szAniName, sizeof(m_szAniName), l_szAniCandidate);
    LoadAnimation(m_szAniName, &m_pStandAni);
  }

  if (m_pStandAni)
  {
#if defined(_DEBUG)
    AppendPlayerLog("LoadRegedAvatar: SetAnimation begin ani=%p", m_pStandAni);
#endif
    m_bUseBoneRender = SetAnimation(m_pStandAni);
  }

  m_bLoaded = l_bLoadedAnyPart;
#if defined(_DEBUG)
  AppendPlayerLog("LoadRegedAvatar: result loaded=%d", m_bLoaded ? 1 : 0);
#endif
  return m_bLoaded;
}

void CPlayer::FrameMove()
{
  CCharacter::FrameMove();
}

BOOL CPlayer::Animation(DWORD /*pi_dwAniFrame*/)
{
  if (!m_bLoaded)
  {
    return FALSE;
  }

  ChInterface *l_pCharIF = CCharacterMgr::GetCharIF();
  if (!l_pCharIF)
  {
    return FALSE;
  }

#if defined(_DEBUG)
  static DWORD s_dwAnimationTraceCount = 0;
  const bool l_bTraceAnimation = (s_dwAnimationTraceCount++ < 180);
  if (l_bTraceAnimation)
  {
    AppendPlayerLog("Animation: begin player=%u bone=%p curAni=%p useBone=%d",
                    static_cast<unsigned>(GetIndex()),
                    m_pBone,
                    m_pCurAni,
                    m_bUseBoneRender ? 1 : 0);
  }
#endif

  if (!m_pCurAni && !m_bUseBoneRender)
  {
#if defined(_DEBUG)
    if (l_bTraceAnimation)
    {
      AppendPlayerLog("Animation: no current animation player=%u",
                      static_cast<unsigned>(GetIndex()));
    }
#endif
    return TRUE;
  }

  if (m_bUseBoneRender && m_pBone)
  {
    __try
    {
      if (m_pBone->m_OMaxFrame > 160)
      {
        float l_fBoneFrame = static_cast<float>(m_dwBoneFrame);
        l_fBoneFrame += (R3GetLoopTime() * 1000.0f) * 5.0f;
        if (l_fBoneFrame >= static_cast<float>(m_pBone->m_OMaxFrame))
        {
          l_fBoneFrame = 160.0f;
        }
        m_dwOldBoneFrame = m_dwBoneFrame;
        m_dwBoneFrame = static_cast<DWORD>(l_fBoneFrame);
      }
      else
      {
        m_dwOldBoneFrame = m_dwBoneFrame;
        m_dwBoneFrame = 160;
      }

      m_pBone->m_Frame = m_dwBoneFrame;
#if defined(_DEBUG)
      if (l_bTraceAnimation)
      {
        AppendPlayerLog("Animation: bone FrameMove begin player=%u frame=%u",
                        static_cast<unsigned>(GetIndex()),
                        static_cast<unsigned>(m_dwBoneFrame));
      }
#endif
      l_pCharIF->FrameMove(m_pBone);
#if defined(_DEBUG)
      if (l_bTraceAnimation)
      {
        AppendPlayerLog("Animation: bone FrameMove end player=%u",
                        static_cast<unsigned>(GetIndex()));
      }
#endif
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
#if defined(_DEBUG)
      AppendPlayerLog("FrameMove: bone FrameMove raised exception code=0x%08X", GetExceptionCode());
#endif
      m_bUseBoneRender = false;
    }
  }

#if defined(_DEBUG)
  if (l_bTraceAnimation)
  {
    AppendPlayerLog("Animation: end player=%u", static_cast<unsigned>(GetIndex()));
  }
#endif
  return TRUE;
}

void CPlayer::SetMoveMode(bool pi_bMoving,
                          BYTE pi_byMoveMode,
                          bool pi_bUseMoveAniDirection,
                          float pi_fFacingRotY,
                          float pi_fMoveRotY)
{
  if (!m_bLoaded)
  {
    return;
  }

  ChAnimation *l_pAnimation = m_pStandAni;
  if (pi_bMoving)
  {
    l_pAnimation = (pi_byMoveMode == CMM_MOVE_RUN) ? m_pRunAni : m_pWalkAni;
    if (pi_bUseMoveAniDirection)
    {
      const float l_fRelativeMoveRotY = NormalizeAngle360Local(pi_fMoveRotY - pi_fFacingRotY);
      if (l_fRelativeMoveRotY >= 135.0f && l_fRelativeMoveRotY < 225.0f)
      {
        l_pAnimation = (pi_byMoveMode == CMM_MOVE_RUN) ? m_pBwRunAni : m_pBwWalkAni;
      }
      else if (l_fRelativeMoveRotY >= 225.0f && l_fRelativeMoveRotY < 315.0f)
      {
        l_pAnimation = (pi_byMoveMode == CMM_MOVE_RUN) ? m_pLfRunAni : m_pLfWalkAni;
      }
      else if (l_fRelativeMoveRotY >= 45.0f && l_fRelativeMoveRotY < 135.0f)
      {
        l_pAnimation = (pi_byMoveMode == CMM_MOVE_RUN) ? m_pRtRunAni : m_pRtWalkAni;
      }
    }

    if (!l_pAnimation)
    {
      l_pAnimation = m_pStandAni;
    }
  }

  if (!SetAnimation(l_pAnimation))
  {
#if defined(_DEBUG)
    AppendPlayerLog("SetMoveMode: SetAnimation failed moving=%d moveMode=%u ani=%p",
                    pi_bMoving ? 1 : 0,
                    static_cast<unsigned>(pi_byMoveMode),
                    l_pAnimation);
#endif
  }
}

void CPlayer::SetWalkMode(BYTE pi_byWalkMode)
{
  if (pi_byWalkMode != CMM_MOVE_WALK && pi_byWalkMode != CMM_MOVE_RUN)
  {
    return;
  }

  m_byWalkMode = pi_byWalkMode;
}

BOOL CPlayer::Render()
{
  if (!m_bLoaded)
  {
    return FALSE;
  }

  ChInterface *l_pCharIF = CCharacterMgr::GetCharIF();
  if (!l_pCharIF)
  {
    return FALSE;
  }

#if defined(_DEBUG)
  static DWORD s_dwRenderTraceCount = 0;
  const bool l_bTraceRender = (s_dwRenderTraceCount++ < 240);
  if (l_bTraceRender)
  {
    AppendPlayerLog("Render: begin player=%u pos=(%.2f, %.2f, %.2f) rotY=%.2f",
                    static_cast<unsigned>(GetIndex()),
                    m_vecPos[0],
                    m_vecPos[1],
                    m_vecPos[2],
                    m_vecRot[1]);
  }
#endif

  l_pCharIF->SetLight(m_d3dLight);

  for (DWORD i = 0; i < MAX_PLAYER_RENDER_PART; ++i)
  {
    if (!m_pMesh[i])
    {
      continue;
    }

    l_pCharIF->SetAlpha(m_pMesh[i], m_fAlphaDensity);
    l_pCharIF->SetMaterial(m_pMesh[i], m_d3dMaterial);
#if defined(_DEBUG)
    if (l_bTraceRender)
    {
      AppendPlayerLog("Render: DrawCharacter begin player=%u part=%u mesh=%p",
                      static_cast<unsigned>(GetIndex()),
                      static_cast<unsigned>(i),
                      m_pMesh[i]);
    }
#endif
    l_pCharIF->DrawCharacter(m_pMesh[i], m_vecPos, m_vecRot[1], m_fScale, 0.0f);
#if defined(_DEBUG)
    if (l_bTraceRender)
    {
      AppendPlayerLog("Render: DrawCharacter end player=%u part=%u",
                      static_cast<unsigned>(GetIndex()),
                      static_cast<unsigned>(i));
    }
#endif
  }
#if defined(_DEBUG)
  if (l_bTraceRender)
  {
    AppendPlayerLog("Render: end player=%u", static_cast<unsigned>(GetIndex()));
  }
#endif
  return TRUE;
}

void CPlayer::CreateShadow()
{
  ChInterface *l_pCharIF = CCharacterMgr::GetCharIF();
  if (!m_bLoaded || !l_pCharIF)
  {
    return;
  }

  l_pCharIF->SetState();
  for (DWORD i = 0; i < MAX_PLAYER_RENDER_PART; ++i)
  {
    if (!m_pMesh[i])
    {
      continue;
    }

    l_pCharIF->DrawCharacterShadow(m_pMesh[i], m_vecPos, m_vecRot[1], 0.8f * m_fAlphaDensity, m_fScale);
  }
  l_pCharIF->UnSetState();
}

BYTE CPlayer::GetWalkMode() const
{
  return m_byWalkMode;
}

bool CPlayer::IsLoaded() const
{
  return m_bLoaded;
}

bool CPlayer::MountRFS()
{
  if (m_bMountedRFS)
  {
    return true;
  }

  if ((IsExistingFile(kLoosePlayerMeshProbe) && IsExistingFile(kLoosePlayerAniProbe)) ||
      HasLoosePlayerResourceFiles())
  {
#if defined(_DEBUG)
    AppendPlayerLog("MountRFS: using loose player files");
#endif
    m_bMountedRFS = true;
#if defined(_DEBUG)
    AppendPlayerLog("MountRFS: success");
#endif
    return true;
  }

#if defined(_DEBUG)
  AppendPlayerLog("MountRFS: mesh");
#endif
  ReadRFSHeader(const_cast<char *>(kPlayerMeshRFSPath));
#if defined(_DEBUG)
  AppendPlayerLog("MountRFS: ani");
#endif
  ReadRFSHeader(const_cast<char *>(kPlayerAniRFSPath));
#if defined(_DEBUG)
  AppendPlayerLog("MountRFS: tex");
#endif
  ReadRFSHeader(const_cast<char *>(kPlayerTexRFSPath));
  m_bMountedRFS = true;
#if defined(_DEBUG)
  AppendPlayerLog("MountRFS: success");
#endif
  return true;
}

bool CPlayer::LoadResourceData(BONE_DATA &po_stBoneData,
                                       MESH_DATA po_astMeshData[MAX_DEFAULT_PART],
                                       ANI_DATA &po_stAniData)
{
  CCharResDataMgr *l_pCharResDataMgr = _GetCharResDataMgr();
  CCharResData *l_pPlayerResData = l_pCharResDataMgr ? l_pCharResDataMgr->GetResourceList(RLI_PLAYER) : NULL;
  if (!l_pPlayerResData)
  {
    return false;
  }

  bool l_bFoundBone = false;
  bool l_abFoundMesh[MAX_DEFAULT_PART] = {false, false, false, false, false, false};
  bool l_bFoundAni = false;

  if (BONE_DATA *l_pBoneData = l_pPlayerResData->GetBoneData(ID_DEFAULT_BONE_AC))
  {
    po_stBoneData = *l_pBoneData;
    l_bFoundBone = true;
  }

  const DWORD l_dwMeshNum = l_pPlayerResData->GetTotalMeshNum();
  for (DWORD i = 0; i < l_dwMeshNum; ++i)
  {
    MESH_DATA *l_pMeshData = l_pPlayerResData->GetMeshDataByOrder(i);
    if (!l_pMeshData)
    {
      continue;
    }

    const MESH_DATA &l_stMeshData = *l_pMeshData;
    if (l_stMeshData.dwBoneID != ID_DEFAULT_BONE_AC)
    {
      continue;
    }

    for (DWORD j = 0; j < MAX_DEFAULT_PART; ++j)
    {
      if (_stricmp(l_stMeshData.pFileName, kDefaultAccretiaMeshName[j]) == 0)
      {
        po_astMeshData[j] = l_stMeshData;
        l_abFoundMesh[j] = true;
      }
    }
  }

  const DWORD l_dwAniNum = l_pPlayerResData->GetTotalAniNum();
  for (DWORD i = 0; i < l_dwAniNum; ++i)
  {
    ANI_DATA *l_pAniData = l_pPlayerResData->GetAniDataByOrder(i);
    if (!l_pAniData)
    {
      continue;
    }

    const ANI_DATA &l_stAniData = *l_pAniData;
    if (_stricmp(l_stAniData.pFileName, kAccretiaStandAniName) == 0)
    {
      po_stAniData = l_stAniData;
      l_bFoundAni = true;
    }
  }

  for (DWORD i = 0; i < MAX_DEFAULT_PART; ++i)
  {
    if (!l_abFoundMesh[i])
    {
      return false;
    }
  }

  return l_bFoundBone && l_bFoundAni;
}

bool CPlayer::LoadPart(DWORD pi_dwPartType, const MESH_DATA &pi_stMeshData)
{
  return LoadPart(pi_dwPartType, RLI_PLAYER, pi_stMeshData);
}

bool CPlayer::LoadPart(DWORD pi_dwPartType, BYTE pi_byResourceList, const MESH_DATA &pi_stMeshData)
{
  ChInterface *l_pCharIF = CCharacterMgr::GetCharIF();
  CObjectManager *l_pBodyPartMgr = CCharacterMgr::GetMeshMgr(pi_byResourceList);
  if (pi_dwPartType >= MAX_PLAYER_RENDER_PART || !l_pCharIF || !l_pBodyPartMgr || !m_pBone)
  {
    return false;
  }

  TextureManager *l_pRenderTextureMgr = &l_pCharIF->m_TM;
  if (!l_pRenderTextureMgr)
  {
    return false;
  }

  sprintf_s(m_szMeshName[pi_dwPartType],
            sizeof(m_szMeshName[pi_dwPartType]),
            "%s%s",
            pi_stMeshData.pPathName,
            pi_stMeshData.pFileName);

  LoadTexturePath(pi_stMeshData.pTexturePath);
  SetTextureManagerPath(l_pRenderTextureMgr, pi_stMeshData.pTexturePath);

  CHARACTEROBJECT *l_pLoadedMesh = NULL;
  __try
  {
    l_pLoadedMesh = l_pCharIF->LoadMeshData(l_pRenderTextureMgr,
                                            l_pBodyPartMgr,
                                            m_szMeshName[pi_dwPartType],
                                            true,
                                            m_pBone);
#if defined(_DEBUG)
    AppendPlayerLog("LoadPart: LoadMeshData index=%u result=%p",
                    static_cast<unsigned>(pi_dwPartType),
                    l_pLoadedMesh);
#endif
  }
    __except (LogPlayerException("LoadPart: LoadMeshData", GetExceptionInformation()))
    {
 #if defined(_DEBUG)
    AppendPlayerLog("LoadPart: LoadMeshData failed index=%u",
                    static_cast<unsigned>(pi_dwPartType));
 #endif
    l_pLoadedMesh = NULL;
  }

  m_pMesh[pi_dwPartType] = l_pLoadedMesh;
  SetCharacterObjectTexturePath(m_pMesh[pi_dwPartType], pi_stMeshData.pTexturePath);
#if defined(_DEBUG)
  if (pi_dwPartType == 0 && m_pMesh[pi_dwPartType])
  {
    DumpCharacterObjectDebug("LoadPart: mesh0 after LoadMeshData", m_pMesh[pi_dwPartType]);
  }
#endif
#if defined(_DEBUG)
  AppendPlayerLog("LoadPart: using LoadMeshData result index=%u mesh=%p",
                  static_cast<unsigned>(pi_dwPartType),
                  m_pMesh[pi_dwPartType]);
#endif

  if (!m_pMesh[pi_dwPartType])
  {
#if defined(_DEBUG)
    AppendPlayerLog("LoadPart: LoadMeshData failed index=%u mesh=%s",
                    static_cast<unsigned>(pi_dwPartType),
                    m_szMeshName[pi_dwPartType]);
#endif
  }

  if (!m_pMesh[pi_dwPartType])
  {
    CHARACTEROBJECT *l_pStoredMesh = NULL;
    __try
    {
      l_pStoredMesh = l_pBodyPartMgr->GetCharacter(m_szMeshName[pi_dwPartType]);
#if defined(_DEBUG)
      AppendPlayerLog("LoadPart: raw GetCharacter index=%u result=%p",
                      static_cast<unsigned>(pi_dwPartType),
                      l_pStoredMesh);
#endif
    }
    __except (LogPlayerException("LoadPart: raw GetCharacter", GetExceptionInformation()))
    {
#if defined(_DEBUG)
      AppendPlayerLog("LoadPart: raw GetCharacter failed index=%u",
                      static_cast<unsigned>(pi_dwPartType));
#endif
      l_pStoredMesh = NULL;
    }

    if (l_pStoredMesh)
    {
      bool l_bMeshRealLoaded = false;
      __try
      {
        l_bMeshRealLoaded = l_pCharIF->LoadRealData(l_pStoredMesh, l_pRenderTextureMgr, false);
#if defined(_DEBUG)
        AppendPlayerLog("LoadPart: fallback LoadRealData extTM false index=%u result=%d",
                        static_cast<unsigned>(pi_dwPartType),
                        l_bMeshRealLoaded ? 1 : 0);
#endif
      }
      __except (LogPlayerException("LoadPart: fallback LoadRealData extTM false",
                                   GetExceptionInformation()))
      {
#if defined(_DEBUG)
        AppendPlayerLog("LoadPart: fallback LoadRealData extTM false failed index=%u",
                        static_cast<unsigned>(pi_dwPartType));
#endif
        l_bMeshRealLoaded = false;
      }

      if (!l_bMeshRealLoaded)
      {
        __try
        {
          l_bMeshRealLoaded = l_pCharIF->LoadRealData(l_pStoredMesh, l_pRenderTextureMgr, true);
#if defined(_DEBUG)
          AppendPlayerLog("LoadPart: fallback LoadRealData extTM true index=%u result=%d",
                          static_cast<unsigned>(pi_dwPartType),
                          l_bMeshRealLoaded ? 1 : 0);
#endif
        }
        __except (LogPlayerException("LoadPart: fallback LoadRealData extTM true",
                                     GetExceptionInformation()))
        {
#if defined(_DEBUG)
          AppendPlayerLog("LoadPart: fallback LoadRealData extTM true failed index=%u",
                          static_cast<unsigned>(pi_dwPartType));
#endif
          l_bMeshRealLoaded = false;
        }
      }

      if (l_bMeshRealLoaded)
      {
        m_pMesh[pi_dwPartType] = l_pStoredMesh;
        SetCharacterObjectTexturePath(m_pMesh[pi_dwPartType], pi_stMeshData.pTexturePath);
      }
    }
  }

  if (m_pMesh[pi_dwPartType])
  {
    SetCharacterObjectTexturePath(m_pMesh[pi_dwPartType], pi_stMeshData.pTexturePath);
    __try
    {
#if defined(_DEBUG)
      AppendPlayerLog("LoadPart: EnsureMeshTexturesLoaded begin index=%u",
                      static_cast<unsigned>(pi_dwPartType));
#endif
      EnsureMeshTexturesLoaded(l_pCharIF,
                               l_pRenderTextureMgr,
                               m_pMesh[pi_dwPartType],
                               pi_stMeshData.pTexturePath);
#if defined(_DEBUG)
      AppendPlayerLog("LoadPart: MatchTextureToMesh begin index=%u",
                      static_cast<unsigned>(pi_dwPartType));
#endif
      l_pCharIF->MatchTextureToMesh(l_pRenderTextureMgr, m_pMesh[pi_dwPartType]);
#if defined(_DEBUG)
      AppendPlayerLog("LoadPart: RematchParent begin index=%u",
                      static_cast<unsigned>(pi_dwPartType));
#endif
      l_pCharIF->RematchParent(m_pBone, m_pMesh[pi_dwPartType]);
#if defined(_DEBUG)
      AppendPlayerLog("LoadPart: AnimationReset begin index=%u",
                      static_cast<unsigned>(pi_dwPartType));
#endif
      l_pCharIF->AnimationReset(m_pMesh[pi_dwPartType]);
#if defined(_DEBUG)
      AppendPlayerLog("LoadPart: MatchTextureToMesh index=%u mesh=%p",
                      static_cast<unsigned>(pi_dwPartType),
                      m_pMesh[pi_dwPartType]);
#endif
    }
    __except (LogPlayerException("LoadPart: MatchTextureToMesh", GetExceptionInformation()))
    {
    }
  }
#if defined(_DEBUG)
  if (pi_dwPartType == 0 && m_pMesh[pi_dwPartType])
  {
    DumpCharacterObjectDebug("LoadPart: mesh0 after GetMeshData", m_pMesh[pi_dwPartType]);
  }
#endif

  return m_pMesh[pi_dwPartType] != NULL;
}

bool CPlayer::LoadTexturePath(const char *pi_pTexturePath)
{
  ChInterface *l_pCharIF = CCharacterMgr::GetCharIF();
  TextureManager *l_pTextureMgr = CCharacterMgr::GetTextureMgr();
  if (!l_pCharIF || !pi_pTexturePath || !pi_pTexturePath[0])
  {
    return false;
  }

  const char *l_pTextureLoadPath = pi_pTexturePath;
  if (_stricmp(pi_pTexturePath, kPlayerTexRFSPath) == 0 &&
      (!m_bPreparedTextureCache && BuildDecodedTextureCache(pi_pTexturePath, kDecodedPlayerTexCachePath)))
  {
    m_bPreparedTextureCache = true;
  }

  if (_stricmp(pi_pTexturePath, kPlayerTexRFSPath) == 0 && m_bPreparedTextureCache)
  {
    l_pTextureLoadPath = kDecodedPlayerTexCachePath;
  }

  if (m_szLastTextureLoadPath[0] &&
      _stricmp(m_szLastTextureLoadPath, l_pTextureLoadPath) == 0)
  {
    return true;
  }

  char l_szManagerPath[MAX_PATH];
  NormalizeTextureManagerPath(l_szManagerPath, sizeof(l_szManagerPath), l_pTextureLoadPath);

  char l_szSearchPattern[MAX_PATH];
  sprintf_s(l_szSearchPattern, sizeof(l_szSearchPattern), "%s*.*", l_pTextureLoadPath);

  WIN32_FIND_DATAA l_stFindData;
  HANDLE l_hFind = FindFirstFileA(l_szSearchPattern, &l_stFindData);
  if (l_hFind == INVALID_HANDLE_VALUE)
  {
    return false;
  }

  bool l_bFoundTextureFile = false;
  DWORD l_dwRegisteredCount = 0;
  char l_szSampleFileName[MAX_PATH] = "";
  do
  {
    if ((l_stFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 ||
        !IsTextureFileName(l_stFindData.cFileName))
    {
      continue;
    }

    l_bFoundTextureFile = true;
    if (!l_szSampleFileName[0])
    {
      strcpy_s(l_szSampleFileName, sizeof(l_szSampleFileName), l_stFindData.cFileName);
    }

    bool l_bRegistered = false;
    if (l_pTextureMgr)
    {
      l_bRegistered = l_pTextureMgr->AddTexture(l_szManagerPath, l_stFindData.cFileName) || l_bRegistered;
      if (!l_bRegistered)
      {
        l_pCharIF->LoadTextureData(l_pTextureMgr,
                                   const_cast<char *>(l_pTextureLoadPath),
                                   l_stFindData.cFileName);
        l_bRegistered = l_pTextureMgr->GetTextureID(l_stFindData.cFileName) != 0 || l_bRegistered;
      }
    }

    l_bRegistered = l_pCharIF->m_TM.AddTexture(l_szManagerPath, l_stFindData.cFileName) || l_bRegistered;
    if (l_pCharIF->m_TM.GetTextureID(l_stFindData.cFileName) == 0)
    {
      l_pCharIF->LoadTextureData(&l_pCharIF->m_TM,
                                 const_cast<char *>(l_pTextureLoadPath),
                                 l_stFindData.cFileName);
      l_bRegistered = l_pCharIF->m_TM.GetTextureID(l_stFindData.cFileName) != 0 || l_bRegistered;
    }
    if (l_bRegistered)
    {
      ++l_dwRegisteredCount;
    }
  } while (FindNextFileA(l_hFind, &l_stFindData));

  FindClose(l_hFind);
  if (l_bFoundTextureFile)
  {
    strcpy_s(m_szLastTextureLoadPath, sizeof(m_szLastTextureLoadPath), l_pTextureLoadPath);
  }
#if defined(_DEBUG)
  AppendPlayerLog("LoadTexturePath: path=%s loadPath=%s found=%d registered=%u sample=%s sampleId=%u sampleTex=%p",
                  pi_pTexturePath,
                  l_pTextureLoadPath,
                  l_bFoundTextureFile ? 1 : 0,
                  static_cast<unsigned>(l_dwRegisteredCount),
                  l_szSampleFileName[0] ? l_szSampleFileName : "<none>",
                  l_szSampleFileName[0] ? static_cast<unsigned>(l_pCharIF->m_TM.GetTextureID(l_szSampleFileName)) : 0,
                  l_szSampleFileName[0] ? l_pCharIF->m_TM.GetTexture(l_szSampleFileName) : NULL);
#endif
  return l_bFoundTextureFile;
}

bool CPlayer::IsTextureFileName(const char *pi_pFileName)
{
  if (!pi_pFileName)
  {
    return false;
  }

  const char *l_pExt = strrchr(pi_pFileName, '.');
  if (!l_pExt)
  {
    return false;
  }

  return _stricmp(l_pExt, ".rft") == 0 || _stricmp(l_pExt, ".dds") == 0;
}
