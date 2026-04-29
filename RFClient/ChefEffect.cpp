#include "ChefEffect.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <map>
#include <vector>

#include "CObject.h"
#include "R3Engine/1stclass/renderparticle.h"
#include "R3Engine/common/c_vector.h"
#include "R3Engine/common/commonutil.h"
#include "R3Engine/common/mergefile.h"
#include "R3Engine/extension/effectcore.h"

CParticleManager g_Particle;

namespace
{
const DWORD MAXLOADPARTICLE = 1024;
const DWORD MAX_DUMMY_PARTICLE = 5;
const DWORD MAXPARTICLE = 3;
const size_t kObjectMeshStride = 0x430;
const size_t kObjectMeshMatResultOffset = 0x1F0;
const size_t kEffectObjectParticleOffset = 0xD0;
const size_t kEffectObjectEntityOffset = 0x100;
const float kChefLocalParticleTimeScale = 0.65f;
const float kChefLocalDrawAlphaScale = 0.70f;

D3DXMATRIX g_matYZ(
  1.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 1.0f, 0.0f,
  0.0f, 1.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 1.0f);

Vector3f g_vecEffectArgv[10];

typedef void (*CHEF_LOAD_FUNC)(char *);

struct CHEF_EFFECT_OBJECT_ATTACHMENT
{
  float m_X;
  float m_Y;
  float m_Z;
  DWORD m_ID;
};

struct CHEF_LOCAL_EFFECT_FILE_ENTRY
{
  std::string m_sFileName;
  int m_nType;
  bool m_bLoadAttempted;
  bool m_bLoaded;

  CHEF_LOCAL_EFFECT_FILE_ENTRY(void)
    : m_nType(0),
      m_bLoadAttempted(false),
      m_bLoaded(false)
  {
  }
};

struct CHEF_LOCAL_PATTERN_ENTRY
{
  DWORD m_adwEffectFileID[9];
};

struct CHEF_LOCAL_ITEM_EFFECT_ENTRY
{
  DWORD m_adwPatternID[6];
};

struct CHEF_LOCAL_RESOLVED_MESH_EFFECT
{
  DWORD m_dwLookupMeshID;
  DWORD m_dwPatternID;
  DWORD m_dwEffectFileID;
  DWORD m_dwLevel;
  DWORD m_dwRaceOverride;
  int m_nPassLevel;
  CHEF_LOCAL_EFFECT_FILE_ENTRY *m_pEffectFile;

  CHEF_LOCAL_RESOLVED_MESH_EFFECT(void)
    : m_dwLookupMeshID(0),
      m_dwPatternID(0),
      m_dwEffectFileID(0),
      m_dwLevel(0),
      m_dwRaceOverride(0),
      m_nPassLevel(9),
      m_pEffectFile(NULL)
  {
  }
};

struct CHEF_LOCAL_EFFECT_OBJECT
{
  BYTE m_abData[292];
  char m_szPartName[32];
  bool m_bHasAttachment;

  CHEF_LOCAL_EFFECT_OBJECT(void)
    : m_bHasAttachment(false)
  {
    ZeroMemory(m_abData, sizeof(m_abData));
    ZeroMemory(m_szPartName, sizeof(m_szPartName));
  }
};

struct CHEF_LOCAL_EFFECT_OBJECT_FILE
{
  std::vector<CHEF_LOCAL_EFFECT_OBJECT> m_vecObjects;
};

std::map<DWORD, CHEF_LOCAL_EFFECT_FILE_ENTRY> g_mapLocalEffectFiles;
std::map<DWORD, CHEF_LOCAL_PATTERN_ENTRY> g_mapLocalPatterns;
std::map<DWORD, CHEF_LOCAL_ITEM_EFFECT_ENTRY> g_mapLocalItemEffects;
std::map<std::string, CHEF_LOCAL_EFFECT_OBJECT_FILE> g_mapLocalEffectObjects;
typedef std::map<DWORD, const CHEF_LOCAL_EFFECT_OBJECT *> CHEF_LOCAL_ATTACHED_EFFECT_OBJECT_MAP;
std::map<CHARACTEROBJECT *, CHEF_LOCAL_ATTACHED_EFFECT_OBJECT_MAP> g_mapLocalAttachedEffectObjects;
CMergeFileManager g_ChefMergeFileManager;
bool g_bChefMergeFileManagerInitialized = false;

BYTE *GetObjectMeshBytes(CHARACTEROBJECT *pi_pCharacterObject, DWORD pi_dwObjectIndex);

float ClampUnitFloat(float pi_fValue)
{
  if (pi_fValue < 0.0f)
  {
    return 0.0f;
  }
  if (pi_fValue > 1.0f)
  {
    return 1.0f;
  }
  return pi_fValue;
}

void ApplyChefLocalParticleTimeScale(CParticle &pio_rParticle)
{
  if (pio_rParticle.mTimeSpeed <= 0.0f)
  {
    return;
  }

  pio_rParticle.mTimeSpeed *= kChefLocalParticleTimeScale;
  if (pio_rParticle.mTimeSpeed < 0.001f)
  {
    pio_rParticle.mTimeSpeed = 0.001f;
  }
}

#if defined(_DEBUG)
void AppendChefEffectLog(const char *format, ...)
{
  HANDLE outputFile = CreateFileA(".\\RFClientCapture.log",
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
  const int length = _vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
  va_end(args);

  if (length > 0)
  {
    DWORD written = 0;
    WriteFile(outputFile, buffer, static_cast<DWORD>(strlen(buffer)), &written, NULL);
    WriteFile(outputFile, "\r\n", 2, &written, NULL);
  }

  CloseHandle(outputFile);
}

DWORD MakeChefDebugKey(BYTE pi_byCategory, const void *pi_pPointer, DWORD pi_dwValue)
{
  const DWORD l_dwPointerBits =
    static_cast<DWORD>(reinterpret_cast<UINT_PTR>(pi_pPointer));
  return (static_cast<DWORD>(pi_byCategory) << 24) ^
         (l_dwPointerBits >> 4) ^
         (pi_dwValue * 2654435761UL);
}

bool ShouldLogChefEffectEvent(DWORD pi_dwKey)
{
  static std::map<DWORD, bool> s_mapLoggedEvents;
  if (s_mapLoggedEvents.find(pi_dwKey) != s_mapLoggedEvents.end())
  {
    return false;
  }

  s_mapLoggedEvents[pi_dwKey] = true;
  return true;
}
#endif

bool FileExists(const char *pi_pFileName)
{
  const DWORD l_dwAttributes = GetFileAttributesA(pi_pFileName);
  return l_dwAttributes != INVALID_FILE_ATTRIBUTES &&
         (l_dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

void EnsureChefMergeFileManager(void)
{
  if (g_bChefMergeFileManagerInitialized)
  {
    return;
  }

  char l_szChefPath[] = ".\\Chef\\";
  g_ChefMergeFileManager.InitMergeFile(l_szChefPath);
  g_bChefMergeFileManagerInitialized = true;
#if defined(_DEBUG)
  AppendChefEffectLog("ChefEffect: Chef merge files mounted count=%u",
                      static_cast<unsigned>(g_ChefMergeFileManager.mMergeFileNum));
#endif
}

bool IsPathSpace(char pi_chValue)
{
  return pi_chValue == ' ' ||
         pi_chValue == '\t' ||
         pi_chValue == '\r' ||
         pi_chValue == '\n';
}

void AssignTrimmedFileName(std::string &po_rFileName, const char *pi_pFileName)
{
  po_rFileName.clear();
  if (!pi_pFileName)
  {
    return;
  }

  const char *l_pBegin = pi_pFileName;
  while (*l_pBegin && IsPathSpace(*l_pBegin))
  {
    ++l_pBegin;
  }

  const char *l_pEnd = l_pBegin + strlen(l_pBegin);
  while (l_pEnd > l_pBegin && IsPathSpace(*(l_pEnd - 1)))
  {
    --l_pEnd;
  }

  if (l_pEnd == l_pBegin)
  {
    return;
  }

  po_rFileName.assign(l_pBegin, l_pEnd);
  if (!_stricmp(po_rFileName.c_str(), "NULL") ||
      !strcmp(po_rFileName.c_str(), "0"))
  {
    po_rFileName.clear();
  }
}

bool LoadLocalEffectFileList(char *pi_pFileName)
{
  FILE *l_pFile = fopen(pi_pFileName, "rb");
  if (!l_pFile)
  {
    return false;
  }

  g_mapLocalEffectFiles.clear();

  DWORD l_dwID = 0;
  int l_nType = 0;
  char l_szFileName[512] = {};
  DWORD l_dwMaxID = 0;
  DWORD l_dwLoadedCount = 0;

  while (fscanf(l_pFile, "%lu%511s%d", &l_dwID, l_szFileName, &l_nType) == 3)
  {
    CHEF_LOCAL_EFFECT_FILE_ENTRY l_stEntry;
    AssignTrimmedFileName(l_stEntry.m_sFileName, l_szFileName);
    if (l_stEntry.m_sFileName.empty())
    {
      continue;
    }

    l_stEntry.m_nType = l_nType;
    g_mapLocalEffectFiles[l_dwID] = l_stEntry;
    if (l_dwID > l_dwMaxID)
    {
      l_dwMaxID = l_dwID;
    }
    ++l_dwLoadedCount;
  }

  fclose(l_pFile);

#if defined(_DEBUG)
  AppendChefEffectLog("ChefEffect: local EffectFileList loaded count=%u maxID=%u",
                      static_cast<unsigned>(l_dwLoadedCount),
                      static_cast<unsigned>(l_dwMaxID));
#endif
  return !g_mapLocalEffectFiles.empty();
}

bool LoadLocalPatternList(char *pi_pFileName)
{
  FILE *l_pFile = fopen(pi_pFileName, "rb");
  if (!l_pFile)
  {
    return false;
  }

  g_mapLocalPatterns.clear();

  DWORD l_dwID = 0;
  DWORD l_dwMaxID = 0;
  DWORD l_dwLoadedCount = 0;
  CHEF_LOCAL_PATTERN_ENTRY l_stEntry;
  while (fscanf(l_pFile,
                "%lu%lu%lu%lu%lu%lu%lu%lu%lu%lu",
                &l_dwID,
                &l_stEntry.m_adwEffectFileID[0],
                &l_stEntry.m_adwEffectFileID[1],
                &l_stEntry.m_adwEffectFileID[2],
                &l_stEntry.m_adwEffectFileID[3],
                &l_stEntry.m_adwEffectFileID[4],
                &l_stEntry.m_adwEffectFileID[5],
                &l_stEntry.m_adwEffectFileID[6],
                &l_stEntry.m_adwEffectFileID[7],
                &l_stEntry.m_adwEffectFileID[8]) == 10)
  {
    g_mapLocalPatterns[l_dwID] = l_stEntry;
    if (l_dwID > l_dwMaxID)
    {
      l_dwMaxID = l_dwID;
    }
    ++l_dwLoadedCount;
  }

  fclose(l_pFile);

#if defined(_DEBUG)
  AppendChefEffectLog("ChefEffect: local PatternList loaded count=%u maxID=%u",
                      static_cast<unsigned>(l_dwLoadedCount),
                      static_cast<unsigned>(l_dwMaxID));
#endif
  return !g_mapLocalPatterns.empty();
}

bool LoadLocalItemEffectList(char *pi_pFileName)
{
  FILE *l_pFile = fopen(pi_pFileName, "rb");
  if (!l_pFile)
  {
    return false;
  }

  g_mapLocalItemEffects.clear();

  DWORD l_dwMeshID = 0;
  DWORD l_dwMaxMeshID = 0;
  DWORD l_dwLoadedCount = 0;
  CHEF_LOCAL_ITEM_EFFECT_ENTRY l_stEntry;
  while (fscanf(l_pFile,
                "%lx%lu%lu%lu%lu%lu%lu",
                &l_dwMeshID,
                &l_stEntry.m_adwPatternID[0],
                &l_stEntry.m_adwPatternID[1],
                &l_stEntry.m_adwPatternID[2],
                &l_stEntry.m_adwPatternID[3],
                &l_stEntry.m_adwPatternID[4],
                &l_stEntry.m_adwPatternID[5]) == 7)
  {
    g_mapLocalItemEffects[l_dwMeshID] = l_stEntry;
    if (l_dwMeshID > l_dwMaxMeshID)
    {
      l_dwMaxMeshID = l_dwMeshID;
    }
    ++l_dwLoadedCount;
  }

  fclose(l_pFile);

#if defined(_DEBUG)
  AppendChefEffectLog("ChefEffect: local ItemEffectList loaded count=%u maxMeshID=0x%08X",
                      static_cast<unsigned>(l_dwLoadedCount),
                      static_cast<unsigned>(l_dwMaxMeshID));
#endif
  return !g_mapLocalItemEffects.empty();
}

DWORD GetChefRaceOverride(CHARACTEROBJECT *pi_pCharacterObject)
{
  if (!pi_pCharacterObject)
  {
    return 0;
  }

  const DWORD l_dwRaceCode = pi_pCharacterObject->m_ID >> 20;
  return (l_dwRaceCode == 1 || l_dwRaceCode == 3) ? 5 : 0;
}

DWORD GetChefRaceEffectPrefix(DWORD pi_dwRaceCode)
{
  switch (pi_dwRaceCode)
  {
  case 0:
  case 1:
    return 0x00700000;
  case 2:
  case 3:
    return 0x00800000;
  case 4:
    return 0x00400000;
  default:
    return 0;
  }
}

DWORD GetChefBaseArmorMeshID(DWORD pi_dwMeshID)
{
  if ((pi_dwMeshID & 0x000F0000) == 0 &&
      (pi_dwMeshID & 0x0000FF00) != 0x00000700)
  {
    return pi_dwMeshID & 0xFF0FFFFF;
  }

  return pi_dwMeshID;
}

void CopyUpperToken(char *po_pBuffer,
                    size_t pi_stBufferSize,
                    const char *pi_pSource,
                    size_t pi_stSourceSize)
{
  if (!po_pBuffer || pi_stBufferSize == 0)
  {
    return;
  }

  po_pBuffer[0] = '\0';
  if (!pi_pSource || pi_stSourceSize == 0)
  {
    return;
  }

  size_t l_stLength = 0;
  while (l_stLength < pi_stSourceSize &&
         pi_pSource[l_stLength] &&
         l_stLength + 1 < pi_stBufferSize)
  {
    po_pBuffer[l_stLength] = pi_pSource[l_stLength];
    ++l_stLength;
  }
  po_pBuffer[l_stLength] = '\0';
  _strupr_s(po_pBuffer, pi_stBufferSize);
}

bool LocalEffectObjectHasAttachment(const BYTE *pi_pObjectData)
{
  if (!pi_pObjectData)
  {
    return false;
  }

  const CHEF_EFFECT_OBJECT_ATTACHMENT *l_pParticle =
    reinterpret_cast<const CHEF_EFFECT_OBJECT_ATTACHMENT *>(pi_pObjectData + kEffectObjectParticleOffset);
  for (DWORD i = 0; i < MAXPARTICLE; ++i)
  {
    if (l_pParticle[i].m_ID)
    {
      return true;
    }
  }

  const CHEF_EFFECT_OBJECT_ATTACHMENT *l_pEntity =
    reinterpret_cast<const CHEF_EFFECT_OBJECT_ATTACHMENT *>(pi_pObjectData + kEffectObjectEntityOffset);
  return l_pEntity->m_ID != 0;
}

void FillLocalEffectObjectFromRecord(CHEF_LOCAL_EFFECT_OBJECT &po_rObject,
                                     const BYTE *pi_pRecord)
{
  ZeroMemory(po_rObject.m_abData, sizeof(po_rObject.m_abData));
  ZeroMemory(po_rObject.m_szPartName, sizeof(po_rObject.m_szPartName));
  po_rObject.m_bHasAttachment = false;

  if (!pi_pRecord)
  {
    return;
  }

  CopyUpperToken(po_rObject.m_szPartName,
                 sizeof(po_rObject.m_szPartName),
                 reinterpret_cast<const char *>(pi_pRecord + 152),
                 20);

  *reinterpret_cast<DWORD *>(po_rObject.m_abData + 156) = pi_pRecord[63];
  *reinterpret_cast<DWORD *>(po_rObject.m_abData + 160) = pi_pRecord[64];
  *reinterpret_cast<DWORD *>(po_rObject.m_abData + 164) = pi_pRecord[65];
  *reinterpret_cast<DWORD *>(po_rObject.m_abData + 168) = pi_pRecord[66];
  memcpy(po_rObject.m_abData + 172, pi_pRecord + 67, sizeof(DWORD));
  memcpy(po_rObject.m_abData + 176, pi_pRecord + 71, sizeof(float));
  memcpy(po_rObject.m_abData + 180, pi_pRecord + 75, sizeof(float));
  *reinterpret_cast<DWORD *>(po_rObject.m_abData + 184) = pi_pRecord[79];
  *reinterpret_cast<DWORD *>(po_rObject.m_abData + 188) = pi_pRecord[80];
  memcpy(po_rObject.m_abData + 192, pi_pRecord + 172, sizeof(DWORD));
  *reinterpret_cast<DWORD *>(po_rObject.m_abData + 196) = pi_pRecord[81];
  *reinterpret_cast<DWORD *>(po_rObject.m_abData + 200) = pi_pRecord[82];
  *reinterpret_cast<DWORD *>(po_rObject.m_abData + 204) = pi_pRecord[83];
  memcpy(po_rObject.m_abData + kEffectObjectParticleOffset, pi_pRecord + 84, 64);
  memcpy(po_rObject.m_abData + 288, pi_pRecord + 148, sizeof(float));

  po_rObject.m_bHasAttachment = LocalEffectObjectHasAttachment(po_rObject.m_abData);
}

bool LoadLocalEffectObjectFile(const char *pi_pFileName)
{
  if (!pi_pFileName || !pi_pFileName[0])
  {
    return false;
  }

  std::map<std::string, CHEF_LOCAL_EFFECT_OBJECT_FILE>::iterator l_itCached =
    g_mapLocalEffectObjects.find(pi_pFileName);
  if (l_itCached != g_mapLocalEffectObjects.end())
  {
    return !l_itCached->second.m_vecObjects.empty();
  }

  FILE *l_pFile = fopen(pi_pFileName, "rb");
  if (!l_pFile)
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

  std::vector<BYTE> l_vecFile(static_cast<size_t>(l_nFileSize));
  if (fread(&l_vecFile[0], 1, l_vecFile.size(), l_pFile) != l_vecFile.size())
  {
    fclose(l_pFile);
    return false;
  }
  fclose(l_pFile);

  CHEF_LOCAL_EFFECT_OBJECT_FILE l_stFile;
  size_t l_stOffset = 0;
  while (l_stOffset + sizeof(WORD) <= l_vecFile.size())
  {
    const WORD l_wRecordSize =
      *reinterpret_cast<const WORD *>(&l_vecFile[l_stOffset]);
    if (l_wRecordSize < 176 || l_stOffset + l_wRecordSize > l_vecFile.size())
    {
#if defined(_DEBUG)
      AppendChefEffectLog("ChefEffect: invalid EFF record file=%s offset=%u recordSize=%u fileSize=%u",
                          pi_pFileName,
                          static_cast<unsigned>(l_stOffset),
                          static_cast<unsigned>(l_wRecordSize),
                          static_cast<unsigned>(l_vecFile.size()));
#endif
      return false;
    }

    CHEF_LOCAL_EFFECT_OBJECT l_stObject;
    FillLocalEffectObjectFromRecord(l_stObject, &l_vecFile[l_stOffset]);
    l_stFile.m_vecObjects.push_back(l_stObject);
    l_stOffset += l_wRecordSize;
  }

  if (l_stOffset != l_vecFile.size())
  {
    return false;
  }

  g_mapLocalEffectObjects[pi_pFileName] = l_stFile;
#if defined(_DEBUG)
  AppendChefEffectLog("ChefEffect: local EFF parsed file=%s records=%u",
                      pi_pFileName,
                      static_cast<unsigned>(l_stFile.m_vecObjects.size()));
#endif
  return !g_mapLocalEffectObjects[pi_pFileName].m_vecObjects.empty();
}

DWORD MakeObjectMeshEffectColor(const CHEF_LOCAL_EFFECT_OBJECT &pi_rObject,
                                int pi_nPassLevel)
{
  if (pi_nPassLevel == 9)
  {
    const float l_fAlpha = *reinterpret_cast<const float *>(pi_rObject.m_abData + 176);
    if (l_fAlpha <= 0.0f)
    {
      return 0xffffffff;
    }
    return static_cast<DWORD>((l_fAlpha - 0.0000001f) * 4294967295.0f);
  }

  return static_cast<DWORD>(static_cast<double>(pi_nPassLevel) *
                            4294967296.0 *
                            0.09090909361839294);
}

void ClearLocalEffectAttachments(CHARACTEROBJECT *pi_pCharacterObject)
{
  if (!pi_pCharacterObject)
  {
    return;
  }

  g_mapLocalAttachedEffectObjects.erase(pi_pCharacterObject);
}

const CHEF_LOCAL_EFFECT_OBJECT *GetAttachedLocalEffectObject(CHARACTEROBJECT *pi_pCharacterObject,
                                                            DWORD pi_dwObjectIndex)
{
  std::map<CHARACTEROBJECT *, CHEF_LOCAL_ATTACHED_EFFECT_OBJECT_MAP>::iterator l_itCharacter =
    g_mapLocalAttachedEffectObjects.find(pi_pCharacterObject);
  if (l_itCharacter == g_mapLocalAttachedEffectObjects.end())
  {
    return NULL;
  }

  CHEF_LOCAL_ATTACHED_EFFECT_OBJECT_MAP::iterator l_itObject =
    l_itCharacter->second.find(pi_dwObjectIndex);
  if (l_itObject == l_itCharacter->second.end())
  {
    return NULL;
  }

  return l_itObject->second;
}

bool AttachLocalEffectObjects(CHARACTEROBJECT *pi_pCharacterObject,
                              const char *pi_pFileName,
                              int pi_nPassLevel)
{
  if (!pi_pCharacterObject || !pi_pCharacterObject->m_pMesh || !pi_pFileName)
  {
    return false;
  }

  std::map<std::string, CHEF_LOCAL_EFFECT_OBJECT_FILE>::iterator l_itFile =
    g_mapLocalEffectObjects.find(pi_pFileName);
  if (l_itFile == g_mapLocalEffectObjects.end())
  {
    return false;
  }

  DWORD l_dwAttachCount = 0;
  for (DWORD i = 0; i < pi_pCharacterObject->m_MaxObj; ++i)
  {
    BYTE *l_pObjectMesh = GetObjectMeshBytes(pi_pCharacterObject, i);
    if (!l_pObjectMesh)
    {
      continue;
    }

    char l_szObjectName[256];
    CopyUpperToken(l_szObjectName,
                   sizeof(l_szObjectName),
                   reinterpret_cast<const char *>(l_pObjectMesh),
                   255);

    CHEF_LOCAL_EFFECT_OBJECT *l_pSelected = NULL;
    for (std::vector<CHEF_LOCAL_EFFECT_OBJECT>::iterator it =
           l_itFile->second.m_vecObjects.begin();
         it != l_itFile->second.m_vecObjects.end();
         ++it)
    {
      if (!it->m_bHasAttachment)
      {
        continue;
      }

      if (!it->m_szPartName[0] || !strcmp(it->m_szPartName, l_szObjectName))
      {
        l_pSelected = &(*it);
      }
    }

    if (!l_pSelected)
    {
      continue;
    }

    g_mapLocalAttachedEffectObjects[pi_pCharacterObject][i] = l_pSelected;
    ++l_dwAttachCount;
#if defined(_DEBUG)
    if (ShouldLogChefEffectEvent(MakeChefDebugKey(1,
                                                  l_pSelected,
                                                  i ^ static_cast<DWORD>(pi_nPassLevel))))
    {
      AppendChefEffectLog("ChefEffect: local attach obj=%u meshName=%s effectName=%s color=0x%08X",
                          static_cast<unsigned>(i),
                          l_szObjectName,
                          l_pSelected->m_szPartName,
                          static_cast<unsigned>(MakeObjectMeshEffectColor(*l_pSelected, pi_nPassLevel)));
    }
#endif
  }

  return l_dwAttachCount != 0;
}

bool FindLocalItemEffectEntry(DWORD pi_dwMeshID,
                              DWORD pi_dwRaceCode,
                              DWORD *po_pdwLookupMeshID,
                              CHEF_LOCAL_ITEM_EFFECT_ENTRY **po_ppEntry)
{
  if (po_pdwLookupMeshID)
  {
    *po_pdwLookupMeshID = pi_dwMeshID;
  }
  if (po_ppEntry)
  {
    *po_ppEntry = NULL;
  }

  std::map<DWORD, CHEF_LOCAL_ITEM_EFFECT_ENTRY>::iterator l_itExact =
    g_mapLocalItemEffects.find(pi_dwMeshID);
  if (l_itExact != g_mapLocalItemEffects.end())
  {
    if (po_pdwLookupMeshID)
    {
      *po_pdwLookupMeshID = l_itExact->first;
    }
    if (po_ppEntry)
    {
      *po_ppEntry = &l_itExact->second;
    }
    return true;
  }

  const DWORD l_dwBaseMeshID = GetChefBaseArmorMeshID(pi_dwMeshID);
  const DWORD l_dwRacePrefix = GetChefRaceEffectPrefix(pi_dwRaceCode);
  if (l_dwRacePrefix)
  {
    const DWORD l_dwRaceMeshID = l_dwRacePrefix | l_dwBaseMeshID;
    std::map<DWORD, CHEF_LOCAL_ITEM_EFFECT_ENTRY>::iterator l_itRace =
      g_mapLocalItemEffects.find(l_dwRaceMeshID);
    if (l_itRace != g_mapLocalItemEffects.end())
    {
      if (po_pdwLookupMeshID)
      {
        *po_pdwLookupMeshID = l_itRace->first;
      }
      if (po_ppEntry)
      {
        *po_ppEntry = &l_itRace->second;
      }
      return true;
    }
  }

  return false;
}

bool ResolveLocalMeshEffect(CHARACTEROBJECT *pi_pCharacterObject,
                            DWORD pi_dwMeshID,
                            DWORD pi_dwLevel,
                            CHEF_LOCAL_RESOLVED_MESH_EFFECT *po_pResolved)
{
  if (!pi_pCharacterObject || pi_dwMeshID == ID_INVALID || pi_dwLevel >= 9 || !po_pResolved)
  {
    return false;
  }

  const DWORD l_dwRaceCode = pi_pCharacterObject->m_ID >> 20;
  const DWORD l_dwRaceOverride = GetChefRaceOverride(pi_pCharacterObject);
  CHEF_LOCAL_ITEM_EFFECT_ENTRY *l_pItemEntry = NULL;
  DWORD l_dwLookupMeshID = pi_dwMeshID;
  if (!FindLocalItemEffectEntry(pi_dwMeshID, l_dwRaceCode, &l_dwLookupMeshID, &l_pItemEntry) ||
      !l_pItemEntry)
  {
    return false;
  }

  const DWORD l_dwPatternIndex = (l_dwRaceOverride < 6) ? l_dwRaceOverride : 0;
  const DWORD l_dwPatternID = l_pItemEntry->m_adwPatternID[l_dwPatternIndex];
  if (!l_dwPatternID)
  {
    return false;
  }

  std::map<DWORD, CHEF_LOCAL_PATTERN_ENTRY>::iterator l_itPattern =
    g_mapLocalPatterns.find(l_dwPatternID);
  if (l_itPattern == g_mapLocalPatterns.end())
  {
    return false;
  }

  const DWORD l_dwEffectFileID = l_itPattern->second.m_adwEffectFileID[pi_dwLevel];
  if (!l_dwEffectFileID)
  {
    return false;
  }

  std::map<DWORD, CHEF_LOCAL_EFFECT_FILE_ENTRY>::iterator l_itEffectFile =
    g_mapLocalEffectFiles.find(l_dwEffectFileID);
  if (l_itEffectFile == g_mapLocalEffectFiles.end() ||
      l_itEffectFile->second.m_sFileName.empty())
  {
    return false;
  }

  po_pResolved->m_dwLookupMeshID = l_dwLookupMeshID;
  po_pResolved->m_dwPatternID = l_dwPatternID;
  po_pResolved->m_dwEffectFileID = l_dwEffectFileID;
  po_pResolved->m_dwLevel = pi_dwLevel;
  po_pResolved->m_dwRaceOverride = l_dwRaceOverride;
  po_pResolved->m_nPassLevel = (l_itEffectFile->second.m_nType == 0)
                                 ? 9
                                 : static_cast<int>(pi_dwLevel);
  po_pResolved->m_pEffectFile = &l_itEffectFile->second;
  return true;
}

bool LoadResolvedEffectFile(CHEF_LOCAL_RESOLVED_MESH_EFFECT &pio_rResolved)
{
  if (!pio_rResolved.m_pEffectFile)
  {
    return false;
  }

  CHEF_LOCAL_EFFECT_FILE_ENTRY &l_rEffectFile = *pio_rResolved.m_pEffectFile;
  if (l_rEffectFile.m_bLoadAttempted)
  {
    return l_rEffectFile.m_bLoaded;
  }

  l_rEffectFile.m_bLoadAttempted = true;
  if (!FileExists(l_rEffectFile.m_sFileName.c_str()))
  {
#if defined(_DEBUG)
    AppendChefEffectLog("ChefEffect: missing EFF file effectID=%u file=%s",
                        static_cast<unsigned>(pio_rResolved.m_dwEffectFileID),
                        l_rEffectFile.m_sFileName.c_str());
#endif
    l_rEffectFile.m_bLoaded = false;
    return false;
  }

  l_rEffectFile.m_bLoaded = LoadLocalEffectObjectFile(l_rEffectFile.m_sFileName.c_str());
  return l_rEffectFile.m_bLoaded;
}

bool ApplyLocalMeshEffect(CHARACTEROBJECT *pi_pCharacterObject,
                          DWORD pi_dwMeshID,
                          DWORD pi_dwLevel)
{
  ClearLocalEffectAttachments(pi_pCharacterObject);

  CHEF_LOCAL_RESOLVED_MESH_EFFECT l_stResolved;
  if (!ResolveLocalMeshEffect(pi_pCharacterObject,
                              pi_dwMeshID,
                              pi_dwLevel,
                              &l_stResolved))
  {
#if defined(_DEBUG)
    AppendChefEffectLog("ChefEffect: local resolve miss objectID=0x%08X meshID=0x%08X level=%u",
                        pi_pCharacterObject ? static_cast<unsigned>(pi_pCharacterObject->m_ID) : 0,
                        static_cast<unsigned>(pi_dwMeshID),
                        static_cast<unsigned>(pi_dwLevel));
#endif
    return false;
  }

  const bool l_bLoaded = LoadResolvedEffectFile(l_stResolved);
  bool l_bSet = false;
  if (l_bLoaded && l_stResolved.m_pEffectFile)
  {
    l_bSet = AttachLocalEffectObjects(pi_pCharacterObject,
                                      l_stResolved.m_pEffectFile->m_sFileName.c_str(),
                                      l_stResolved.m_nPassLevel);
  }

#if defined(_DEBUG)
  if (ShouldLogChefEffectEvent(MakeChefDebugKey(2,
                                                pi_pCharacterObject,
                                                pi_dwMeshID ^ l_stResolved.m_dwEffectFileID)))
  {
    AppendChefEffectLog("ChefEffect: local apply objectID=0x%08X meshID=0x%08X lookup=0x%08X level=%u raceOverride=%u pattern=%u effectID=%u passLevel=%d loaded=%d set=%d file=%s",
                        pi_pCharacterObject ? static_cast<unsigned>(pi_pCharacterObject->m_ID) : 0,
                        static_cast<unsigned>(pi_dwMeshID),
                        static_cast<unsigned>(l_stResolved.m_dwLookupMeshID),
                        static_cast<unsigned>(pi_dwLevel),
                        static_cast<unsigned>(l_stResolved.m_dwRaceOverride),
                        static_cast<unsigned>(l_stResolved.m_dwPatternID),
                        static_cast<unsigned>(l_stResolved.m_dwEffectFileID),
                        l_stResolved.m_nPassLevel,
                        l_bLoaded ? 1 : 0,
                        l_bSet ? 1 : 0,
                        l_stResolved.m_pEffectFile ? l_stResolved.m_pEffectFile->m_sFileName.c_str() : "");
  }
#endif
  return l_bSet;
}

void LoadRootEffectEntityList(char *pi_pFileName)
{
  CreateEffectsList(pi_pFileName);
}

void LoadRootMagicSptList(char *pi_pFileName)
{
  CreateMagicSptList(pi_pFileName);
}

void LoadChefParticleList(char *pi_pFileName)
{
  g_Particle.LoadParticleIni(pi_pFileName);
}

void LoadChefEntityList(char *pi_pFileName)
{
  g_Particle.LoadEntityIni(pi_pFileName);
}

bool RunLoadStep(const char *pi_pLabel,
                 CHEF_LOAD_FUNC pi_pLoadFunc,
                 char *pi_pFileName,
                 bool pi_bRequired)
{
  if (!pi_pLoadFunc || !pi_pFileName)
  {
    return false;
  }

  if (!FileExists(pi_pFileName))
  {
#if defined(_DEBUG)
    AppendChefEffectLog("ChefEffect: %s missing file=%s required=%d",
                        pi_pLabel ? pi_pLabel : "(unknown)",
                        pi_pFileName,
                        pi_bRequired ? 1 : 0);
#endif
    return !pi_bRequired;
  }

#if defined(_DEBUG)
  const DWORD l_dwStartTick = GetTickCount();
  AppendChefEffectLog("ChefEffect: %s begin file=%s",
                      pi_pLabel ? pi_pLabel : "(unknown)",
                      pi_pFileName);
#endif
  __try
  {
    pi_pLoadFunc(pi_pFileName);
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
#if defined(_DEBUG)
    AppendChefEffectLog("ChefEffect: %s exception code=0x%08X elapsed=%u",
                        pi_pLabel ? pi_pLabel : "(unknown)",
                        static_cast<unsigned>(GetExceptionCode()),
                        static_cast<unsigned>(GetTickCount() - l_dwStartTick));
#endif
    return !pi_bRequired;
  }

#if defined(_DEBUG)
  AppendChefEffectLog("ChefEffect: %s success elapsed=%u",
                      pi_pLabel ? pi_pLabel : "(unknown)",
                      static_cast<unsigned>(GetTickCount() - l_dwStartTick));
#endif
  return true;
}

bool InitializeRootEffectScriptsOnce(void)
{
  static bool s_bAttempted = false;
  static bool s_bLoaded = false;

  if (s_bAttempted)
  {
    return s_bLoaded;
  }
  s_bAttempted = true;

  char l_szEffectEntityList[] = ".\\Effect\\EffectEntityList.spt";
  char l_szMagicSptList[] = ".\\Effect\\MagicSptList.spt";

  if (!RunLoadStep("EffectEntityList", LoadRootEffectEntityList, l_szEffectEntityList, true))
  {
    return false;
  }

  if (!RunLoadStep("MagicSptList", LoadRootMagicSptList, l_szMagicSptList, true))
  {
    return false;
  }

  s_bLoaded = true;
  return true;
}

BYTE *GetObjectMeshBytes(CHARACTEROBJECT *pi_pCharacterObject, DWORD pi_dwObjectIndex)
{
  if (!pi_pCharacterObject || !pi_pCharacterObject->m_pMesh ||
      pi_dwObjectIndex >= pi_pCharacterObject->m_MaxObj)
  {
    return NULL;
  }

  return reinterpret_cast<BYTE *>(pi_pCharacterObject->m_pMesh) +
         (pi_dwObjectIndex * kObjectMeshStride);
}

D3DXMATRIX *GetObjectMeshMatResult(CHARACTEROBJECT *pi_pCharacterObject, DWORD pi_dwObjectIndex)
{
  BYTE *l_pObjectMesh = GetObjectMeshBytes(pi_pCharacterObject, pi_dwObjectIndex);
  if (!l_pObjectMesh)
  {
    return NULL;
  }

  return reinterpret_cast<D3DXMATRIX *>(l_pObjectMesh + kObjectMeshMatResultOffset);
}

BYTE *GetObjectMeshEffectObject(CHARACTEROBJECT *pi_pCharacterObject, DWORD pi_dwObjectIndex)
{
  const CHEF_LOCAL_EFFECT_OBJECT *l_pObject =
    GetAttachedLocalEffectObject(pi_pCharacterObject, pi_dwObjectIndex);
  if (!l_pObject)
  {
    return NULL;
  }

  return const_cast<BYTE *>(l_pObject->m_abData);
}

CHEF_EFFECT_OBJECT_ATTACHMENT *GetEffectObjectParticle(CHARACTEROBJECT *pi_pCharacterObject,
                                                       DWORD pi_dwObjectIndex,
                                                       DWORD pi_dwParticleIndex)
{
  BYTE *l_pEffectObject = GetObjectMeshEffectObject(pi_pCharacterObject, pi_dwObjectIndex);
  if (!l_pEffectObject || pi_dwParticleIndex >= MAXPARTICLE)
  {
    return NULL;
  }

  return reinterpret_cast<CHEF_EFFECT_OBJECT_ATTACHMENT *>(
           l_pEffectObject + kEffectObjectParticleOffset) +
         pi_dwParticleIndex;
}

CHEF_EFFECT_OBJECT_ATTACHMENT *GetEffectObjectEntity(CHARACTEROBJECT *pi_pCharacterObject,
                                                     DWORD pi_dwObjectIndex)
{
  BYTE *l_pEffectObject = GetObjectMeshEffectObject(pi_pCharacterObject, pi_dwObjectIndex);
  if (!l_pEffectObject)
  {
    return NULL;
  }

  return reinterpret_cast<CHEF_EFFECT_OBJECT_ATTACHMENT *>(l_pEffectObject + kEffectObjectEntityOffset);
}

} // namespace

CHPARTICLE::CHPARTICLE(void)
  : m_bLoad(FALSE),
    m_bRealLoad(FALSE),
    m_bAbsAxis(TRUE),
    m_Count(0)
{
  m_fScale[0] = 1.0f;
  m_fScale[1] = 1.0f;
  m_fScale[2] = 1.0f;
}

void CHPARTICLE::Release(void)
{
  m_Particle[0].ReleaseParticle();
  m_Particle[0].ReleaseEntity();
  m_bLoad = FALSE;
  m_bRealLoad = FALSE;
}

BOOL CHPARTICLE::RealLoad(void)
{
  if (m_sFileName.empty())
  {
    return FALSE;
  }

  EnsureChefMergeFileManager();
  CMergeFileManager *l_pPreviousMergeFileManager = GetMergeFileManager();
  SetMergeFileManager(&g_ChefMergeFileManager);
  __try
  {
    if (!m_Particle[0].LoadParticleSPT(const_cast<char *>(m_sFileName.c_str()), _DO_NOT_MANAGE_TEXTURE))
    {
      const BOOL l_bExists =
        IsExistFileAndMergeFile(const_cast<char *>(m_sFileName.c_str()));
      SetMergeFileManager(l_pPreviousMergeFileManager);
#if defined(_DEBUG)
      AppendChefEffectLog("ChefEffect: particle real-load failed file=%s exists=%d mergeFiles=%u",
                          m_sFileName.c_str(),
                          l_bExists ? 1 : 0,
                          static_cast<unsigned>(g_ChefMergeFileManager.mMergeFileNum));
#endif
      return FALSE;
    }

    ApplyChefLocalParticleTimeScale(m_Particle[0]);
    m_Particle[0].InitParticle();
    m_Particle[0].SetParticleState(_PARTICLE_STATE_START);
    Vector3f l_vecZero = {0.0f, 0.0f, 0.0f};
    m_Particle[0].SetCreatePos(l_vecZero);

    for (DWORD i = 1; i < MAX_DUMMY_PARTICLE; ++i)
    {
      memcpy(&m_Particle[i], &m_Particle[0], sizeof(CParticle));
      m_Particle[i].InitParticle();
      m_Particle[i].SetParticleState(_PARTICLE_STATE_START);
      m_Particle[i].SetCreatePos(l_vecZero);
    }
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    SetMergeFileManager(l_pPreviousMergeFileManager);
#if defined(_DEBUG)
    AppendChefEffectLog("ChefEffect: particle real-load exception code=0x%08X file=%s",
                        static_cast<unsigned>(GetExceptionCode()),
                        m_sFileName.c_str());
#endif
    return FALSE;
  }
  SetMergeFileManager(l_pPreviousMergeFileManager);

  m_bRealLoad = TRUE;
  return TRUE;
}

CHENTITY::CHENTITY(void)
  : m_bLoad(FALSE),
    m_bRealLoad(FALSE),
    m_bAbsAxis(TRUE)
{
  m_fScale[0] = 0.0f;
  m_fScale[1] = 0.0f;
  m_fScale[2] = 0.0f;
  m_fRot[0] = 0.0f;
  m_fRot[1] = 0.0f;
  m_fRot[2] = 0.0f;
}

void CHENTITY::Release(void)
{
  m_Entity.ReleaseEntity();
  m_bLoad = FALSE;
  m_bRealLoad = FALSE;
}

BOOL CHENTITY::RealLoad(void)
{
  if (m_sFileName.empty())
  {
    return FALSE;
  }

  EnsureChefMergeFileManager();
  CMergeFileManager *l_pPreviousMergeFileManager = GetMergeFileManager();
  SetMergeFileManager(&g_ChefMergeFileManager);
  __try
  {
    if (!m_Entity.LoadEntity(const_cast<char *>(m_sFileName.c_str()), _DO_NOT_MANAGE_TEXTURE))
    {
      const BOOL l_bExists =
        IsExistFileAndMergeFile(const_cast<char *>(m_sFileName.c_str()));
      SetMergeFileManager(l_pPreviousMergeFileManager);
#if defined(_DEBUG)
      AppendChefEffectLog("ChefEffect: entity real-load failed file=%s exists=%d mergeFiles=%u",
                          m_sFileName.c_str(),
                          l_bExists ? 1 : 0,
                          static_cast<unsigned>(g_ChefMergeFileManager.mMergeFileNum));
#endif
      return FALSE;
    }

    m_Entity.RestoreTexMem();
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    SetMergeFileManager(l_pPreviousMergeFileManager);
#if defined(_DEBUG)
    AppendChefEffectLog("ChefEffect: entity real-load exception code=0x%08X file=%s",
                        static_cast<unsigned>(GetExceptionCode()),
                        m_sFileName.c_str());
#endif
    return FALSE;
  }
  SetMergeFileManager(l_pPreviousMergeFileManager);

  m_bRealLoad = TRUE;
  return TRUE;
}

CParticleManager::CParticleManager(void)
  : m_Counter(0)
{
}

CParticleManager::~CParticleManager(void)
{
  ReleaseParticle();
  ReleaseEntity();
  m_Counter = 0;
}

CParticle *CParticleManager::GetParticle(DWORD pi_dwID)
{
  if (pi_dwID >= MAXLOADPARTICLE || !m_Particle[pi_dwID].m_bLoad)
  {
    return NULL;
  }

  if (!m_Particle[pi_dwID].m_bRealLoad && !m_Particle[pi_dwID].RealLoad())
  {
    m_Particle[pi_dwID].m_bLoad = FALSE;
    return NULL;
  }

  const DWORD l_dwIndex = m_Particle[pi_dwID].m_Count % MAX_DUMMY_PARTICLE;
  ++m_Particle[pi_dwID].m_Count;
  return &m_Particle[pi_dwID].m_Particle[l_dwIndex];
}

CEntity *CParticleManager::GetEntity(DWORD pi_dwID)
{
  if (pi_dwID >= MAXLOADPARTICLE || !m_Entity[pi_dwID].m_bLoad)
  {
    return NULL;
  }

  if (!m_Entity[pi_dwID].m_bRealLoad && !m_Entity[pi_dwID].RealLoad())
  {
    m_Entity[pi_dwID].m_bLoad = FALSE;
    return NULL;
  }

  return &m_Entity[pi_dwID].m_Entity;
}

CHPARTICLE *CParticleManager::GetCHParticle(DWORD pi_dwID)
{
  if (pi_dwID >= MAXLOADPARTICLE)
  {
    return NULL;
  }

  return &m_Particle[pi_dwID];
}

CHENTITY *CParticleManager::GetCHEntity(DWORD pi_dwID)
{
  if (pi_dwID >= MAXLOADPARTICLE)
  {
    return NULL;
  }

  return &m_Entity[pi_dwID];
}

void CParticleManager::AddParticle(char *pi_pFileName, DWORD pi_dwID, CHPARTICLE pi_stParticle)
{
  if (!pi_pFileName || pi_dwID >= MAXLOADPARTICLE)
  {
    return;
  }

  AssignTrimmedFileName(m_Particle[pi_dwID].m_sFileName, pi_pFileName);
  if (m_Particle[pi_dwID].m_sFileName.empty())
  {
    return;
  }

  m_Particle[pi_dwID].m_fScale[0] = pi_stParticle.m_fScale[0];
  m_Particle[pi_dwID].m_fScale[1] = pi_stParticle.m_fScale[1];
  m_Particle[pi_dwID].m_fScale[2] = pi_stParticle.m_fScale[2];
  m_Particle[pi_dwID].m_bAbsAxis = pi_stParticle.m_bAbsAxis;
  m_Particle[pi_dwID].m_bLoad = TRUE;
  ++m_Counter;
}

void CParticleManager::AddEntity(char *pi_pFileName, DWORD pi_dwID, CHENTITY pi_stEntity)
{
  if (!pi_pFileName || pi_dwID >= MAXLOADPARTICLE)
  {
    return;
  }

  AssignTrimmedFileName(m_Entity[pi_dwID].m_sFileName, pi_pFileName);
  if (m_Entity[pi_dwID].m_sFileName.empty())
  {
    return;
  }

  m_Entity[pi_dwID].m_fScale[0] = pi_stEntity.m_fScale[0];
  m_Entity[pi_dwID].m_fScale[1] = pi_stEntity.m_fScale[1];
  m_Entity[pi_dwID].m_fScale[2] = pi_stEntity.m_fScale[2];
  memcpy(m_Entity[pi_dwID].m_fRot, pi_stEntity.m_fRot, sizeof(float) * 3);
  m_Entity[pi_dwID].m_bAbsAxis = pi_stEntity.m_bAbsAxis;
  m_Entity[pi_dwID].m_bLoad = TRUE;
  ++m_Counter;
}

BOOL CParticleManager::LoadParticleIni(char *pi_pFileName)
{
  if (!pi_pFileName)
  {
    return FALSE;
  }

  char l_szSection[256] = "PART";
  const DWORD l_dwMaxParticle = GetPrivateProfileIntA("PARTICLE_INFO",
                                                      "MAXPARTICLE",
                                                      0,
                                                      pi_pFileName);
  for (DWORD i = 0; i < l_dwMaxParticle; ++i)
  {
    char l_szFileName[256] = {};
    char l_szValue[256] = {};
    CHPARTICLE l_stParticle;
    DWORD l_dwIndex = 0;

    sprintf(l_szSection, "PARTICLE%u", static_cast<unsigned>(i + 1));
    GetPrivateProfileStringA(l_szSection, "INDEX", "0", l_szFileName, sizeof(l_szFileName), pi_pFileName);
    sscanf(l_szFileName, "%u", &l_dwIndex);
    GetPrivateProfileStringA(l_szSection, "PARTICLE", "NULL", l_szFileName, sizeof(l_szFileName), pi_pFileName);
    GetPrivateProfileStringA(l_szSection, "SCALEX", "1.0", l_szValue, sizeof(l_szValue), pi_pFileName);
    sscanf(l_szValue, "%f", &l_stParticle.m_fScale[0]);
    GetPrivateProfileStringA(l_szSection, "SCALEY", "1.0", l_szValue, sizeof(l_szValue), pi_pFileName);
    sscanf(l_szValue, "%f", &l_stParticle.m_fScale[1]);
    GetPrivateProfileStringA(l_szSection, "SCALEZ", "1.0", l_szValue, sizeof(l_szValue), pi_pFileName);
    sscanf(l_szValue, "%f", &l_stParticle.m_fScale[2]);
    GetPrivateProfileStringA(l_szSection, "ABSAXIS", "1", l_szValue, sizeof(l_szValue), pi_pFileName);
    int l_nAbsAxis = 1;
    sscanf(l_szValue, "%d", &l_nAbsAxis);
    l_stParticle.m_bAbsAxis = l_nAbsAxis ? TRUE : FALSE;
    AddParticle(l_szFileName, l_dwIndex, l_stParticle);
  }

  return TRUE;
}

BOOL CParticleManager::LoadEntityIni(char *pi_pFileName)
{
  if (!pi_pFileName)
  {
    return FALSE;
  }

  char l_szSection[256] = "PART";
  const DWORD l_dwMaxEntity = GetPrivateProfileIntA("ENTITY_INFO",
                                                   "MAXENTITY",
                                                   0,
                                                   pi_pFileName);
  for (DWORD i = 0; i < l_dwMaxEntity; ++i)
  {
    char l_szFileName[256] = {};
    char l_szValue[256] = {};
    CHENTITY l_stEntity;
    DWORD l_dwIndex = 0;

    sprintf(l_szSection, "ENTITY%u", static_cast<unsigned>(i + 1));
    GetPrivateProfileStringA(l_szSection, "INDEX", "0", l_szFileName, sizeof(l_szFileName), pi_pFileName);
    sscanf(l_szFileName, "%u", &l_dwIndex);
    GetPrivateProfileStringA(l_szSection, "ENTITY", "NULL", l_szFileName, sizeof(l_szFileName), pi_pFileName);
    GetPrivateProfileStringA(l_szSection, "SCALEX", "1.0", l_szValue, sizeof(l_szValue), pi_pFileName);
    sscanf(l_szValue, "%f", &l_stEntity.m_fScale[0]);
    GetPrivateProfileStringA(l_szSection, "SCALEY", "1.0", l_szValue, sizeof(l_szValue), pi_pFileName);
    sscanf(l_szValue, "%f", &l_stEntity.m_fScale[1]);
    GetPrivateProfileStringA(l_szSection, "SCALEZ", "1.0", l_szValue, sizeof(l_szValue), pi_pFileName);
    sscanf(l_szValue, "%f", &l_stEntity.m_fScale[2]);
    GetPrivateProfileStringA(l_szSection, "ABSAXIS", "1", l_szValue, sizeof(l_szValue), pi_pFileName);
    int l_nAbsAxis = 1;
    sscanf(l_szValue, "%d", &l_nAbsAxis);
    l_stEntity.m_bAbsAxis = l_nAbsAxis ? TRUE : FALSE;
    GetPrivateProfileStringA(l_szSection, "ROTX", "0.0", l_szValue, sizeof(l_szValue), pi_pFileName);
    sscanf(l_szValue, "%f", &l_stEntity.m_fRot[0]);
    GetPrivateProfileStringA(l_szSection, "ROTY", "0.0", l_szValue, sizeof(l_szValue), pi_pFileName);
    sscanf(l_szValue, "%f", &l_stEntity.m_fRot[1]);
    GetPrivateProfileStringA(l_szSection, "ROTZ", "0.0", l_szValue, sizeof(l_szValue), pi_pFileName);
    sscanf(l_szValue, "%f", &l_stEntity.m_fRot[2]);
    AddEntity(l_szFileName, l_dwIndex, l_stEntity);
  }

  return TRUE;
}

void CParticleManager::ReleaseParticle(void)
{
  for (DWORD i = 0; i < MAXLOADPARTICLE; ++i)
  {
    m_Particle[i].Release();
  }
}

void CParticleManager::ReleaseEntity(void)
{
  for (DWORD i = 0; i < MAXLOADPARTICLE; ++i)
  {
    m_Entity[i].Release();
  }
}

void CParticleManager::ResetParticleCount(void)
{
  for (DWORD i = 0; i < MAXLOADPARTICLE; ++i)
  {
    m_Particle[i].m_Count = 0;
  }
}

bool InitializeChefEffectsOnce(void)
{
  static bool s_bAttempted = false;
  static bool s_bLoaded = false;

  if (s_bAttempted)
  {
    return s_bLoaded;
  }
  s_bAttempted = true;

  char l_szEffectFileList[] = ".\\CHEF\\EffectFileList.txt";
  char l_szPatternList[] = ".\\CHEF\\PatternList.txt";
  char l_szItemEffectList[] = ".\\CHEF\\ItemEffectList.txt";
  char l_szParticleList[] = ".\\CHEF\\Particle.INI";
  char l_szEntityList[] = ".\\CHEF\\Entity.INI";

  if (!InitializeRootEffectScriptsOnce())
  {
#if defined(_DEBUG)
    AppendChefEffectLog("ChefEffect: root Effect scripts unavailable");
#endif
    return false;
  }

  if (!FileExists(l_szEffectFileList) ||
      !FileExists(l_szPatternList) ||
      !FileExists(l_szItemEffectList) ||
      !FileExists(l_szParticleList) ||
      !FileExists(l_szEntityList))
  {
#if defined(_DEBUG)
    AppendChefEffectLog("ChefEffect: missing required list file effect=%d pattern=%d item=%d particle=%d entity=%d",
                        FileExists(l_szEffectFileList) ? 1 : 0,
                        FileExists(l_szPatternList) ? 1 : 0,
                        FileExists(l_szItemEffectList) ? 1 : 0,
                        FileExists(l_szParticleList) ? 1 : 0,
                        FileExists(l_szEntityList) ? 1 : 0);
#endif
    return false;
  }

#if defined(_DEBUG)
  const DWORD l_dwStartTick = GetTickCount();
  AppendChefEffectLog("ChefEffect: initialize begin");
#endif
  if (RunLoadStep("Particle.INI", LoadChefParticleList, l_szParticleList, true) &&
      RunLoadStep("Entity.INI", LoadChefEntityList, l_szEntityList, true) &&
      LoadLocalEffectFileList(l_szEffectFileList) &&
      LoadLocalPatternList(l_szPatternList) &&
      LoadLocalItemEffectList(l_szItemEffectList))
  {
    s_bLoaded = true;
#if defined(_DEBUG)
    AppendChefEffectLog("ChefEffect: optional Monster/Unit/Npc effect lists deferred");
    AppendChefEffectLog("ChefEffect: initialize success elapsed=%u",
                        static_cast<unsigned>(GetTickCount() - l_dwStartTick));
#endif
  }
  else
  {
#if defined(_DEBUG)
    AppendChefEffectLog("ChefEffect: initialize failed elapsed=%u",
                        static_cast<unsigned>(GetTickCount() - l_dwStartTick));
#endif
    s_bLoaded = false;
  }

  return s_bLoaded;
}

void ApplyMeshEffect(CHARACTEROBJECT *pi_pCharacterObject,
                     DWORD pi_dwMeshID,
                     DWORD pi_dwLevel)
{
  if (!pi_pCharacterObject || pi_dwMeshID == ID_INVALID)
  {
    return;
  }

  ApplyLocalMeshEffect(pi_pCharacterObject, pi_dwMeshID, pi_dwLevel);
}

bool HasMeshEffect(CHARACTEROBJECT *pi_pCharacterObject,
                   DWORD pi_dwMeshID,
                   DWORD pi_dwLevel)
{
  if (!pi_pCharacterObject || pi_dwMeshID == ID_INVALID)
  {
    return false;
  }

  CHEF_LOCAL_RESOLVED_MESH_EFFECT l_stResolved;
  return ResolveLocalMeshEffect(pi_pCharacterObject,
                                pi_dwMeshID,
                                pi_dwLevel,
                                &l_stResolved);
}

void SetCharacterctEffectArgv(DWORD pi_dwID, Vector3f pi_vecPos)
{
  if (pi_dwID >= 10)
  {
    return;
  }

  Vector3fCopy(pi_vecPos, g_vecEffectArgv[pi_dwID]);
}

void ResetChefParticleCount(void)
{
  g_Particle.ResetParticleCount();
}

void DrawCharacterctEffect(CHARACTEROBJECT *pi_pCharacterObject,
                           float pi_afPos[3],
                           float pi_fRotY,
                           float pi_fAlpha,
                           float pi_fScale)
{
  if (!pi_pCharacterObject || !pi_pCharacterObject->m_pMesh || !pi_afPos)
  {
    return;
  }

  static float s_fEntityScroll = 0.0f;
  static float s_fBaseTime = 0.0f;
  const float l_fCurTime = R3GetTime();
  if (s_fBaseTime == 0.0f)
  {
    s_fBaseTime = l_fCurTime;
  }

  const float l_fDelta = (l_fCurTime - s_fBaseTime) * 0.5f;
  s_fBaseTime = l_fCurTime;
  s_fEntityScroll += l_fDelta * 30.0f;

  const float l_fDrawAlpha = ClampUnitFloat(pi_fAlpha * kChefLocalDrawAlphaScale);
  const DWORD l_dwAlpha = (static_cast<DWORD>(l_fDrawAlpha * 0xff000000) & 0xff000000) + 0x00ffffff;

  for (DWORD i = 0; i < pi_pCharacterObject->m_MaxObj; ++i)
  {
    D3DXMATRIX *l_pMatResult = GetObjectMeshMatResult(pi_pCharacterObject, i);
    if (!l_pMatResult)
    {
      continue;
    }

    for (DWORD j = 0; j < MAXPARTICLE; ++j)
    {
      CHEF_EFFECT_OBJECT_ATTACHMENT *l_pParticleInfo = GetEffectObjectParticle(pi_pCharacterObject, i, j);
      if (!l_pParticleInfo || !l_pParticleInfo->m_ID)
      {
        continue;
      }

      CParticle *l_pParticle = g_Particle.GetParticle(l_pParticleInfo->m_ID);
      CHPARTICLE *l_pCHParticle = g_Particle.GetCHParticle(l_pParticleInfo->m_ID);
      if (!l_pParticle || !l_pCHParticle)
      {
#if defined(_DEBUG)
        if (ShouldLogChefEffectEvent(MakeChefDebugKey(3,
                                                      pi_pCharacterObject,
                                                      (i << 16) ^ l_pParticleInfo->m_ID)))
        {
          AppendChefEffectLog("ChefEffect: particle draw miss obj=%u particleID=%u",
                              static_cast<unsigned>(i),
                              static_cast<unsigned>(l_pParticleInfo->m_ID));
        }
#endif
        continue;
      }

#if defined(_DEBUG)
      if (ShouldLogChefEffectEvent(MakeChefDebugKey(4,
                                                    pi_pCharacterObject,
                                                    (i << 16) ^ l_pParticleInfo->m_ID)))
      {
        AppendChefEffectLog("ChefEffect: particle draw obj=%u particleID=%u",
                            static_cast<unsigned>(i),
                            static_cast<unsigned>(l_pParticleInfo->m_ID));
      }
#endif
      Vector3fCopy(g_vecEffectArgv[0], l_pParticle->mSpecialARGV[0]);
      Vector3fCopy(g_vecEffectArgv[1], l_pParticle->mSpecialARGV[1]);

      D3DXMATRIX l_matWorld;
      D3DXMATRIX l_matRot;
      if (l_pCHParticle->m_bAbsAxis)
      {
        D3DXMatrixIdentity(&l_matWorld);
        D3DXMatrixRotationY(&l_matRot, D3DXToRadian(-(180.0f - pi_fRotY)));
        D3DXMatrixMultiply(&l_matWorld, &l_matRot, &l_matWorld);

        D3DXVECTOR4 l_vecPos(l_pMatResult->_41 * pi_fScale,
                             l_pMatResult->_43 * pi_fScale,
                             l_pMatResult->_42 * pi_fScale,
                             0.0f);
        D3DXVec3Transform(&l_vecPos, reinterpret_cast<D3DXVECTOR3 *>(&l_vecPos), &l_matRot);
        l_matWorld._41 = pi_afPos[0] + l_vecPos.x + l_pParticleInfo->m_X;
        l_matWorld._42 = pi_afPos[1] + l_vecPos.y + l_pParticleInfo->m_Y;
        l_matWorld._43 = pi_afPos[2] + l_vecPos.z + l_pParticleInfo->m_Z;

        if (l_pParticle->Loop() == 0)
        {
          return;
        }
        memcpy(l_pParticle->mRotMat, l_matRot.m, sizeof(float) * 16);
        l_pParticle->SetCreatePos(l_matWorld.m[3]);
        DrawCParticle(l_pParticle, l_pParticle->mEntity, l_matWorld.m, l_dwAlpha);
      }
      else
      {
        D3DXMatrixIdentity(&l_matWorld);
        l_matWorld._11 *= pi_fScale;
        l_matWorld._22 *= pi_fScale;
        l_matWorld._33 *= pi_fScale;

        D3DXMatrixRotationZ(&l_matRot, D3DXToRadian(180.0f - pi_fRotY));
        D3DXMatrixMultiply(&l_matWorld, &l_matRot, &l_matWorld);
        D3DXMatrixMultiply(&l_matWorld, &l_matWorld, &g_matYZ);
        D3DXMatrixMultiply(&l_matWorld, l_pMatResult, &l_matWorld);

        l_matWorld._41 = pi_afPos[0] + l_matWorld._41 + l_pParticleInfo->m_X;
        l_matWorld._42 = pi_afPos[1] + l_matWorld._42 + l_pParticleInfo->m_Y;
        l_matWorld._43 = pi_afPos[2] + l_matWorld._43 + l_pParticleInfo->m_Z;
        memcpy(l_matRot.m, l_matWorld.m, sizeof(float) * 16);
        l_matRot._41 = 0.0f;
        l_matRot._42 = 0.0f;
        l_matRot._43 = 0.0f;

        if (l_pParticle->Loop() == 0)
        {
          return;
        }
        memcpy(l_pParticle->mRotMat, l_matRot.m, sizeof(float) * 16);
        l_pParticle->SetCreatePos(l_matWorld.m[3]);
        DrawCParticle(l_pParticle, l_pParticle->mEntity, l_matWorld.m, l_dwAlpha);
      }
    }

    CHEF_EFFECT_OBJECT_ATTACHMENT *l_pEntityInfo = GetEffectObjectEntity(pi_pCharacterObject, i);
    if (!l_pEntityInfo || !l_pEntityInfo->m_ID)
    {
      continue;
    }

    CEntity *l_pEntity = g_Particle.GetEntity(l_pEntityInfo->m_ID);
    CHENTITY *l_pCHEntity = g_Particle.GetCHEntity(l_pEntityInfo->m_ID);
    if (!l_pEntity || !l_pCHEntity)
    {
#if defined(_DEBUG)
      if (ShouldLogChefEffectEvent(MakeChefDebugKey(5,
                                                    pi_pCharacterObject,
                                                    (i << 16) ^ l_pEntityInfo->m_ID)))
      {
        AppendChefEffectLog("ChefEffect: entity draw miss obj=%u entityID=%u",
                            static_cast<unsigned>(i),
                            static_cast<unsigned>(l_pEntityInfo->m_ID));
      }
#endif
      continue;
    }

#if defined(_DEBUG)
    if (ShouldLogChefEffectEvent(MakeChefDebugKey(6,
                                                  pi_pCharacterObject,
                                                  (i << 16) ^ l_pEntityInfo->m_ID)))
    {
      AppendChefEffectLog("ChefEffect: entity draw obj=%u entityID=%u",
                          static_cast<unsigned>(i),
                          static_cast<unsigned>(l_pEntityInfo->m_ID));
    }
#endif
    D3DXMATRIX l_matWorld;
    D3DXMATRIX l_matRot;
    if (l_pCHEntity->m_bAbsAxis)
    {
      D3DXMatrixIdentity(&l_matWorld);
      l_matWorld._11 *= pi_fScale;
      l_matWorld._22 *= pi_fScale;
      l_matWorld._33 *= pi_fScale;

      D3DXMatrixRotationY(&l_matRot, D3DXToRadian(-(180.0f - pi_fRotY)));
      D3DXMatrixMultiply(&l_matWorld, &l_matRot, &l_matWorld);

      D3DXVECTOR4 l_vecPos(l_pMatResult->_41,
                           l_pMatResult->_43,
                           l_pMatResult->_42,
                           0.0f);
      D3DXVec3Transform(&l_vecPos, reinterpret_cast<D3DXVECTOR3 *>(&l_vecPos), &l_matRot);
      l_matWorld._41 = pi_afPos[0] + l_vecPos.x + l_pEntityInfo->m_X;
      l_matWorld._42 = pi_afPos[1] + l_vecPos.y + l_pEntityInfo->m_Y;
      l_matWorld._43 = pi_afPos[2] + l_vecPos.z + l_pEntityInfo->m_Z;
      l_pEntity->DrawEntity(l_matWorld.m, l_dwAlpha, s_fEntityScroll);
    }
    else
    {
      D3DXMatrixIdentity(&l_matWorld);
      l_matWorld._11 *= pi_fScale;
      l_matWorld._22 *= pi_fScale;
      l_matWorld._33 *= pi_fScale;

      D3DXMatrixRotationZ(&l_matRot, D3DXToRadian(180.0f - pi_fRotY));
      D3DXMatrixMultiply(&l_matWorld, &l_matRot, &l_matWorld);

      l_matWorld._41 = pi_afPos[0] + l_pEntityInfo->m_X;
      l_matWorld._43 = pi_afPos[1] + l_pEntityInfo->m_Y;
      l_matWorld._42 = pi_afPos[2] + l_pEntityInfo->m_Z;
      D3DXMatrixMultiply(&l_matWorld, &l_matWorld, &g_matYZ);
      D3DXMatrixMultiply(&l_matWorld, l_pMatResult, &l_matWorld);

      D3DXMATRIX l_matInv;
      D3DXMatrixInverse(&l_matInv, NULL, &g_matYZ);
      D3DXMatrixMultiply(&l_matWorld, &l_matInv, &l_matWorld);
      l_pEntity->DrawEntity(l_matWorld.m, l_dwAlpha, s_fEntityScroll);
    }
  }
}
