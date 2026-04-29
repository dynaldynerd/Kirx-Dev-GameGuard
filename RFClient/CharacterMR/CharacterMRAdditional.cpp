#include "CharacterMRExtra.h"
#include "CharacterMRInternal.h"

#include "../AlphaMeshManager.h"
#include "../ChefEffect.h"
#include "../R3Engine/extension/effectentitylist.h"
#include "../R3Engine/extension/effectcore.h"

#include <algorithm>
#include <cctype>
#include <cfloat>
#include <cstdio>
#include <cstring>
#include <direct.h>
#include <vector>

#include <d3dx8.h>

void SetFustumNormalPlane(float (*pi_pfPlane)[4], D3DXMATRIX *pi_pMatrix);
void MultMatrixAsm(float *po_pOut, const float *pi_pA, const float *pi_pB);

struct FILESLOT
{
  char m_szFileName[260];
  BYTE m_bInUse;
  BYTE m_bComplete;
  BYTE m_abPad[6];
};

std::map<std::string, RFSINFO> g_RFSMap;
FILESLOT FileSlot[16];
threadcontrol ThreadC[4];

namespace
{
struct PtrVector
{
  void **m_pBegin;
  void **m_pEnd;
  void **m_pCapacity;
};

struct CharacterMRAnimationData
{
  WORD m_wRotCount;
  WORD m_wReserved0;
  BYTE *m_pRotKeys;
  WORD m_wTransCount;
  WORD m_wReserved1;
  BYTE *m_pTransKeys;
  WORD m_wScaleCount;
  WORD m_wReserved2;
  BYTE *m_pScaleKeys;
  WORD m_wAlphaCount;
  WORD m_wReserved3;
  BYTE *m_pAlphaKeys;
};

const size_t kObjectManagerVectorBegin = 0x04;
const size_t kObjectManagerVectorEnd = 0x08;
const size_t kObjectManagerVectorCapacity = 0x0C;
const size_t kObjectManagerPartName = 0x14;

const size_t kChInterfaceTexturePathOffset = 0x198 + 0x1C;

const DWORD kDDSMagic = 0x20534444;

char g_szStrTokBuffer[1024];

std::map<std::string, DWORD> g_mapEffectFileIDs;
std::map<DWORD, std::string> g_mapEffectFiles;
std::map<DWORD, DWORD> g_mapItemEffectPattern;
std::map<std::string, CEffectMap> g_mapEffectObjects;

void UppercaseInPlace(char *pio_pValue)
{
  if (!pio_pValue)
  {
    return;
  }

  for (char *p = pio_pValue; *p; ++p)
  {
    if (*p == '/')
    {
      *p = '\\';
    }
    else
    {
      *p = static_cast<char>(toupper(static_cast<unsigned char>(*p)));
    }
  }
}

std::string MakeUpperKey(const char *pi_pValue)
{
  std::string l_sKey = pi_pValue ? pi_pValue : "";
  for (size_t i = 0; i < l_sKey.size(); ++i)
  {
    if (l_sKey[i] == '/')
    {
      l_sKey[i] = '\\';
    }
    else
    {
      l_sKey[i] = static_cast<char>(toupper(static_cast<unsigned char>(l_sKey[i])));
    }
  }
  return l_sKey;
}

void CopyStringSafe(char *po_pDst, size_t pi_stDstSize, const char *pi_pSrc)
{
  if (!po_pDst || pi_stDstSize == 0)
  {
    return;
  }

  po_pDst[0] = '\0';
  if (pi_pSrc)
  {
    strncpy_s(po_pDst, pi_stDstSize, pi_pSrc, _TRUNCATE);
  }
}

void CombinePath(char *po_pDst, size_t pi_stDstSize, const char *pi_pPath, const char *pi_pFile)
{
  if (!po_pDst || pi_stDstSize == 0)
  {
    return;
  }

  po_pDst[0] = '\0';
  if (!pi_pFile || !pi_pFile[0])
  {
    return;
  }

  if (!pi_pPath || !pi_pPath[0])
  {
    CopyStringSafe(po_pDst, pi_stDstSize, pi_pFile);
    return;
  }

  const size_t l_stPathLen = strlen(pi_pPath);
  const bool l_bHasSlash = l_stPathLen > 0 &&
                           (pi_pPath[l_stPathLen - 1] == '\\' ||
                            pi_pPath[l_stPathLen - 1] == '/');
  _snprintf_s(po_pDst,
              pi_stDstSize,
              _TRUNCATE,
              l_bHasSlash ? "%s%s" : "%s\\%s",
              pi_pPath,
              pi_pFile);
}

PtrVector GetVector(void *pi_pOwner,
                    size_t pi_stBeginOffset,
                    size_t pi_stEndOffset,
                    size_t pi_stCapacityOffset)
{
  PtrVector l_stVector;
  l_stVector.m_pBegin = CharacterMR::Field<void **>(pi_pOwner, pi_stBeginOffset);
  l_stVector.m_pEnd = CharacterMR::Field<void **>(pi_pOwner, pi_stEndOffset);
  l_stVector.m_pCapacity = CharacterMR::Field<void **>(pi_pOwner, pi_stCapacityOffset);
  return l_stVector;
}

void SetVector(void *pi_pOwner,
               size_t pi_stBeginOffset,
               size_t pi_stEndOffset,
               size_t pi_stCapacityOffset,
               const PtrVector &pi_rVector)
{
  CharacterMR::Field<void **>(pi_pOwner, pi_stBeginOffset) = pi_rVector.m_pBegin;
  CharacterMR::Field<void **>(pi_pOwner, pi_stEndOffset) = pi_rVector.m_pEnd;
  CharacterMR::Field<void **>(pi_pOwner, pi_stCapacityOffset) = pi_rVector.m_pCapacity;
}

size_t VectorCount(const PtrVector &pi_rVector)
{
  if (!pi_rVector.m_pBegin || !pi_rVector.m_pEnd)
  {
    return 0;
  }
  return static_cast<size_t>(pi_rVector.m_pEnd - pi_rVector.m_pBegin);
}

bool AppendPointer(void *pi_pOwner,
                   size_t pi_stBeginOffset,
                   size_t pi_stEndOffset,
                   size_t pi_stCapacityOffset,
                   void *pi_pValue)
{
  PtrVector l_stVector = GetVector(pi_pOwner,
                                   pi_stBeginOffset,
                                   pi_stEndOffset,
                                   pi_stCapacityOffset);
  const size_t l_stCount = VectorCount(l_stVector);
  const size_t l_stCapacity =
    l_stVector.m_pBegin && l_stVector.m_pCapacity ?
      static_cast<size_t>(l_stVector.m_pCapacity - l_stVector.m_pBegin) :
      0;

  if (l_stCount >= l_stCapacity)
  {
    size_t l_stNewCapacity = l_stCapacity ? l_stCapacity * 2 : 1;
    void **l_ppNewData = static_cast<void **>(::operator new(sizeof(void *) * l_stNewCapacity));
    if (l_stCount)
    {
      memcpy(l_ppNewData, l_stVector.m_pBegin, sizeof(void *) * l_stCount);
    }
    ::operator delete(l_stVector.m_pBegin);
    l_stVector.m_pBegin = l_ppNewData;
    l_stVector.m_pEnd = l_ppNewData + l_stCount;
    l_stVector.m_pCapacity = l_ppNewData + l_stNewCapacity;
  }

  *l_stVector.m_pEnd = pi_pValue;
  ++l_stVector.m_pEnd;
  SetVector(pi_pOwner,
            pi_stBeginOffset,
            pi_stEndOffset,
            pi_stCapacityOffset,
            l_stVector);
  return true;
}

void RemovePointerAt(void *pi_pOwner,
                     size_t pi_stBeginOffset,
                     size_t pi_stEndOffset,
                     size_t pi_stCapacityOffset,
                     size_t pi_stIndex)
{
  PtrVector l_stVector = GetVector(pi_pOwner,
                                   pi_stBeginOffset,
                                   pi_stEndOffset,
                                   pi_stCapacityOffset);
  const size_t l_stCount = VectorCount(l_stVector);
  if (pi_stIndex >= l_stCount)
  {
    return;
  }

  for (size_t i = pi_stIndex + 1; i < l_stCount; ++i)
  {
    l_stVector.m_pBegin[i - 1] = l_stVector.m_pBegin[i];
  }
  --l_stVector.m_pEnd;
  SetVector(pi_pOwner,
            pi_stBeginOffset,
            pi_stEndOffset,
            pi_stCapacityOffset,
            l_stVector);
}

IDirect3DDevice8 *GetRenderDevice(CRender *pi_pRender)
{
  return pi_pRender ? CharacterMR::Field<IDirect3DDevice8 *>(pi_pRender, 0) : NULL;
}

ObjectMesh *GetMeshAt(CHARACTEROBJECT *pi_pCharacterObject, DWORD pi_dwIndex)
{
  if (!pi_pCharacterObject ||
      pi_dwIndex >= CharacterMR::Field<DWORD>(pi_pCharacterObject, CharacterMR::CHOBJ_MAX_OBJ))
  {
    return NULL;
  }

  ObjectMesh *l_pMesh =
    CharacterMR::Field<ObjectMesh *>(pi_pCharacterObject, CharacterMR::CHOBJ_MESH);
  if (!l_pMesh)
  {
    return NULL;
  }

  return reinterpret_cast<ObjectMesh *>(reinterpret_cast<BYTE *>(l_pMesh) +
                                        (pi_dwIndex * sizeof(ObjectMesh)));
}

ObjectMesh *FindObjectMeshByName(ObjectMesh *pi_pMesh, int pi_nCount, const char *pi_pName)
{
  if (!pi_pMesh || !pi_pName || !pi_pName[0])
  {
    return NULL;
  }

  for (int i = 0; i < pi_nCount; ++i)
  {
    ObjectMesh *l_pMesh =
      reinterpret_cast<ObjectMesh *>(reinterpret_cast<BYTE *>(pi_pMesh) +
                                     (i * sizeof(ObjectMesh)));
    if (!_stricmp(reinterpret_cast<const char *>(l_pMesh) + CharacterMR::OBJ_NAME, pi_pName))
    {
      return l_pMesh;
    }
  }
  return NULL;
}

void FreeAnimationData(ObjectMesh *pio_pObjectMesh)
{
  if (!pio_pObjectMesh)
  {
    return;
  }

  CharacterMRAnimationData *l_pData =
    CharacterMR::Field<CharacterMRAnimationData *>(pio_pObjectMesh,
                                                  CharacterMR::OBJ_ANIMATION_DATA);
  if (l_pData)
  {
    ::operator delete(l_pData);
  }
  CharacterMR::Field<CharacterMRAnimationData *>(pio_pObjectMesh,
                                                CharacterMR::OBJ_ANIMATION_DATA) = NULL;
  CharacterMR::Field<BYTE>(pio_pObjectMesh, CharacterMR::OBJ_ANIMATION_ENABLED) = 0;
  CharacterMR::Field<DWORD>(pio_pObjectMesh, CharacterMR::OBJ_ANIMATION_MAX_FRAME) = 0;
}

DWORD FindMaxAnimationFrame(const BYTE *pi_pAnimationBytes)
{
  if (!pi_pAnimationBytes)
  {
    return 0;
  }

  const WORD l_wObjectCount = *reinterpret_cast<const WORD *>(pi_pAnimationBytes);
  size_t l_stOffset = 2;
  DWORD l_dwMaxFrame = 0;
  for (WORD i = 0; i < l_wObjectCount; ++i)
  {
    l_stOffset += 100;
    const WORD l_wCounts[4] =
    {
      *reinterpret_cast<const WORD *>(pi_pAnimationBytes + l_stOffset + 0),
      *reinterpret_cast<const WORD *>(pi_pAnimationBytes + l_stOffset + 8),
      *reinterpret_cast<const WORD *>(pi_pAnimationBytes + l_stOffset + 16),
      *reinterpret_cast<const WORD *>(pi_pAnimationBytes + l_stOffset + 24)
    };
    l_stOffset += 32;

    const size_t l_astKeySizes[4] = {20, 16, 16, 8};
    for (int track = 0; track < 4; ++track)
    {
      for (WORD key = 0; key < l_wCounts[track]; ++key)
      {
        const WORD l_wFrame = *reinterpret_cast<const WORD *>(pi_pAnimationBytes + l_stOffset);
        if (l_wFrame > l_dwMaxFrame)
        {
          l_dwMaxFrame = l_wFrame;
        }
        l_stOffset += l_astKeySizes[track];
      }
    }
  }
  return l_dwMaxFrame;
}

void UnlockDDS(DWORD *pio_pBuffer)
{
  static const DWORD s_adwPassWord[32] =
  {
    0x764D802E, 0xF0D1F82E, 0x81863FBD, 0x3F3F2C58,
    0x6F672E2E, 0x783F403F, 0xC0F13F3C, 0x9F3BF6A5,
    0xD73F20C1, 0x85E9C1C8, 0x56EFBD86, 0x2EFBA13F,
    0x4C618687, 0xB44E3B21, 0x97AE5778, 0x2E4A2E3F,
    0x442E4C3F, 0xE85FC5CD, 0xBDEBECE9, 0x6CF7BBBE,
    0x2EE4F22E, 0x9F973F3F, 0xB921B39D, 0x3F546576,
    0xF0C6F6E6, 0xB2E2DB79, 0xEB2E2E4B, 0xABCAD3D3,
    0x9CEDC7EA, 0x65D0D9C7, 0x35FAB448, 0x9B6A2E2E
  };

  for (DWORD i = 0; i < 32; ++i)
  {
    pio_pBuffer[i] ^= s_adwPassWord[i];
  }
}

void FixDDSHeaderBytes(BYTE *pio_pBuffer, DWORD pi_dwBufferSize)
{
  if (!pio_pBuffer || pi_dwBufferSize < 128)
  {
    return;
  }

  DWORD *l_pHeader = reinterpret_cast<DWORD *>(pio_pBuffer);
  if (l_pHeader[20] == 5)
  {
    l_pHeader[20] = 4;
  }
  if (l_pHeader[20] == 0x41)
  {
    if (l_pHeader[22] == 16 && l_pHeader[26] != 0x00008000)
    {
      l_pHeader[23] = 0x00000F00;
    }
    else if (l_pHeader[22] == 32)
    {
      l_pHeader[23] = 0x00FF0000;
      l_pHeader[24] = 0x0000FF00;
      l_pHeader[25] = 0x000000FF;
      l_pHeader[26] = 0xFF000000;
    }
  }
  if (l_pHeader[19] == 0)
  {
    l_pHeader[19] = 32;
    l_pHeader[20] = 4;
  }
}

void LoadEffectObjectFileIntoMap(const char *pi_pFileName, CEffectMap &po_rMap)
{
  po_rMap.m_mapObjects.clear();
  if (!pi_pFileName || !pi_pFileName[0])
  {
    return;
  }

  FILE *l_pFile = fopen(pi_pFileName, "rb");
  if (!l_pFile)
  {
    return;
  }

  while (!feof(l_pFile))
  {
    WORD l_wRecordSize = 0;
    if (fread(&l_wRecordSize, 1, sizeof(l_wRecordSize), l_pFile) != sizeof(l_wRecordSize))
    {
      break;
    }
    if (l_wRecordSize < 176)
    {
      break;
    }

    std::vector<BYTE> l_vecRecord(l_wRecordSize);
    *reinterpret_cast<WORD *>(&l_vecRecord[0]) = l_wRecordSize;
    if (fread(&l_vecRecord[2], 1, l_wRecordSize - 2, l_pFile) != l_wRecordSize - 2)
    {
      break;
    }

    EffectObject l_stObject;
    ZeroMemory(l_stObject.m_abData, sizeof(l_stObject.m_abData));
    memcpy(l_stObject.m_abData + 156, &l_vecRecord[63], 4);
    memcpy(l_stObject.m_abData + 160, &l_vecRecord[64], 4);
    memcpy(l_stObject.m_abData + 164, &l_vecRecord[65], 4);
    memcpy(l_stObject.m_abData + 168, &l_vecRecord[66], 4);
    memcpy(l_stObject.m_abData + 172, &l_vecRecord[67], sizeof(DWORD));
    memcpy(l_stObject.m_abData + 176, &l_vecRecord[71], sizeof(float));
    memcpy(l_stObject.m_abData + 180, &l_vecRecord[75], sizeof(float));
    memcpy(l_stObject.m_abData + 184, &l_vecRecord[79], 4);
    memcpy(l_stObject.m_abData + 188, &l_vecRecord[80], 4);
    if (l_wRecordSize > 172)
    {
      memcpy(l_stObject.m_abData + 192, &l_vecRecord[172], sizeof(DWORD));
    }
    memcpy(l_stObject.m_abData + 196, &l_vecRecord[81], 4);
    memcpy(l_stObject.m_abData + 200, &l_vecRecord[82], 4);
    memcpy(l_stObject.m_abData + 204, &l_vecRecord[83], 4);
    memcpy(l_stObject.m_abData + 0xD0, &l_vecRecord[84], std::min<size_t>(64, l_vecRecord.size() - 84));
    if (l_wRecordSize > 148)
    {
      memcpy(l_stObject.m_abData + 288, &l_vecRecord[148], sizeof(float));
    }

    char l_szPartName[32] = {};
    if (l_wRecordSize > 152)
    {
      strncpy_s(l_szPartName,
                sizeof(l_szPartName),
                reinterpret_cast<const char *>(&l_vecRecord[152]),
                std::min<size_t>(20, l_vecRecord.size() - 152));
      UppercaseInPlace(l_szPartName);
    }
    po_rMap.m_mapObjects[l_szPartName] = l_stObject;
  }

  fclose(l_pFile);
}
}

RFSINFO::RFSINFO(void)
  : m_dwOffset(0),
    m_dwSize(0)
{
}

RFSINFO::RFSINFO(const RFSINFO &pi_rOther)
  : m_sRfsFileName(pi_rOther.m_sRfsFileName),
    m_dwOffset(pi_rOther.m_dwOffset),
    m_dwSize(pi_rOther.m_dwSize)
{
}

RFSINFO::~RFSINFO(void)
{
}

threadcontrol::threadcontrol(void)
  : m_hWorkEvent(NULL),
    m_hCompleteEvent(NULL),
    m_pWork(NULL)
{
}

threadcontrol::~threadcontrol(void)
{
  if (m_hWorkEvent)
  {
    CloseHandle(m_hWorkEvent);
    m_hWorkEvent = NULL;
  }
  if (m_hCompleteEvent)
  {
    CloseHandle(m_hCompleteEvent);
    m_hCompleteEvent = NULL;
  }
}

AnimationData::~AnimationData(void)
{
}

void InitFileLoad(void)
{
}

void LoadFile(void)
{
}

void __cdecl ReadData(HANDLE *pi_pEvents)
{
  UNREFERENCED_PARAMETER(pi_pEvents);
}

void __cdecl ReadData(void *pi_pData)
{
  ReadData(static_cast<HANDLE *>(pi_pData));
}

void __cdecl BossThread(void)
{
}

void __cdecl BossThread(void *pi_pData)
{
  UNREFERENCED_PARAMETER(pi_pData);
  BossThread();
}

bool FindDupSlot(char *pi_pFileName)
{
  if (!pi_pFileName)
  {
    return false;
  }

  for (int i = 0; i < 16; ++i)
  {
    if (FileSlot[i].m_bInUse && !_stricmp(FileSlot[i].m_szFileName, pi_pFileName))
    {
      return true;
    }
  }
  return false;
}

bool IsWorkComplete(void)
{
  for (int i = 0; i < 16; ++i)
  {
    if (FileSlot[i].m_bInUse && !FileSlot[i].m_bComplete)
    {
      return false;
    }
  }
  return true;
}

int FindEmptySlot(void)
{
  for (int i = 0; i < 16; ++i)
  {
    if (!FileSlot[i].m_bInUse)
    {
      return i;
    }
  }
  return -1;
}

int FindFirstSlot(void)
{
  for (int i = 0; i < 16; ++i)
  {
    if (FileSlot[i].m_bInUse && !FileSlot[i].m_bComplete)
    {
      return i;
    }
  }
  return -1;
}

char *GetExtention(char *pi_pFileName)
{
  if (!pi_pFileName)
  {
    return NULL;
  }

  char *l_pDot = strrchr(pi_pFileName, '.');
  return l_pDot ? l_pDot + 1 : pi_pFileName + strlen(pi_pFileName);
}

ChTexture::ChTexture(void)
  : m_dwID(0),
    m_pTexture(NULL),
    m_dwBytes(0)
{
  ZeroMemory(m_szName, sizeof(m_szName));
}

ChTexture::~ChTexture(void)
{
  if (m_pTexture)
  {
    m_pTexture->Release();
    m_pTexture = NULL;
  }
  m_dwID = 0;
  m_dwBytes = 0;
}

MeshData::MeshData(void)
{
  ZeroMemory(m_data, sizeof(m_data));
}

MeshData::~MeshData(void)
{
  ::operator delete(CharacterMR::Field<void *>(this, 0x2B8));
  ::operator delete(CharacterMR::Field<void *>(this, 0x2BC));
  ::operator delete(CharacterMR::Field<void *>(this, 0x2C0));
  ::operator delete(CharacterMR::Field<void *>(this, 0x2C8));
  ::operator delete(CharacterMR::Field<void *>(this, 0x2CC));
  CharacterMR::Field<void *>(this, 0x2B8) = NULL;
  CharacterMR::Field<void *>(this, 0x2BC) = NULL;
  CharacterMR::Field<void *>(this, 0x2C0) = NULL;
  CharacterMR::Field<void *>(this, 0x2C8) = NULL;
  CharacterMR::Field<void *>(this, 0x2CC) = NULL;
}

ObjectMesh::ObjectMesh(void)
{
  CharacterMR::InitObjectMesh(this);
}

ObjectMesh::~ObjectMesh(void)
{
  CharacterMR::ReleaseObjectMesh(this);
}

void ForceCharacterMRVectorDeletingDestructors(void)
{
  ObjectMesh *l_pObjectMesh = new ObjectMesh[1];
  delete[] l_pObjectMesh;

  MeshData *l_pMeshData = new MeshData[1];
  delete[] l_pMeshData;
}

void ObjectMesh::InitEffect(void)
{
  CharacterMR::Field<DWORD>(this, 0x310) = 0;
  CharacterMR::Field<DWORD>(this, 0x318) = 0;
  CharacterMR::Field<float>(this, CharacterMR::OBJ_ALPHA) = 1.0f;
  ZeroMemory(reinterpret_cast<BYTE *>(this) + 0x36C, 0x30);
}

BOOL ObjectMesh::IsHaveEffect(void)
{
  for (DWORD i = 0; i < 3; ++i)
  {
    if (CharacterMR::Field<DWORD>(this, 0x378 + (i * 4)))
    {
      return TRUE;
    }
  }
  return CharacterMR::Field<DWORD>(this, CharacterMR::OBJ_EFFECT_ENTITY) ? TRUE : FALSE;
}

CMeshManager::~CMeshManager(void)
{
  PtrVector l_stVector = GetVector(this, 0x04, 0x08, 0x0C);
  for (void **it = l_stVector.m_pBegin; it && it != l_stVector.m_pEnd; ++it)
  {
    CObjectManager *l_pManager = static_cast<CObjectManager *>(*it);
    if (l_pManager)
    {
      l_pManager->~CObjectManager();
      ::operator delete(l_pManager);
    }
  }
  ::operator delete(l_stVector.m_pBegin);
  ZeroMemory(m_data, sizeof(m_data));
}

void CMeshManager::AddObjectManager(CObjectManager *pi_pObjectManager)
{
  if (pi_pObjectManager)
  {
    AppendPointer(this, 0x04, 0x08, 0x0C, pi_pObjectManager);
  }
}

CObjectManager *CMeshManager::GetObjectManager(char *pi_pName)
{
  if (!pi_pName)
  {
    return NULL;
  }

  char l_szName[128];
  CopyStringSafe(l_szName, sizeof(l_szName), pi_pName);
  UppercaseInPlace(l_szName);

  PtrVector l_stVector = GetVector(this, 0x04, 0x08, 0x0C);
  for (void **it = l_stVector.m_pBegin; it && it != l_stVector.m_pEnd; ++it)
  {
    CObjectManager *l_pManager = static_cast<CObjectManager *>(*it);
    if (l_pManager &&
        !strcmp(reinterpret_cast<const char *>(l_pManager) + kObjectManagerPartName, l_szName))
    {
      return l_pManager;
    }
  }
  return NULL;
}

void CMeshManager::DeleteObjectManager(char *pi_pName)
{
  if (!pi_pName)
  {
    return;
  }

  char l_szName[128];
  CopyStringSafe(l_szName, sizeof(l_szName), pi_pName);
  UppercaseInPlace(l_szName);

  PtrVector l_stVector = GetVector(this, 0x04, 0x08, 0x0C);
  const size_t l_stCount = VectorCount(l_stVector);
  for (size_t i = 0; i < l_stCount; ++i)
  {
    CObjectManager *l_pManager = static_cast<CObjectManager *>(l_stVector.m_pBegin[i]);
    if (!l_pManager ||
        strcmp(reinterpret_cast<const char *>(l_pManager) + kObjectManagerPartName, l_szName))
    {
      continue;
    }

    l_pManager->~CObjectManager();
    ::operator delete(l_pManager);
    RemovePointerAt(this, 0x04, 0x08, 0x0C, i);
    return;
  }
}

CRender::~CRender(void)
{
  m_TM.Clear();
}

int CRender::BeginShadow(unsigned long pi_dwSize)
{
  IDirect3DTexture8 *l_pTexture = NULL;
  IDirect3DDevice8 *l_pDevice = GetRenderDevice(this);
  if (!l_pDevice)
  {
    return 0;
  }

  if (pi_dwSize == 0)
  {
    pi_dwSize = 256;
  }

  if (FAILED(l_pDevice->CreateTexture(pi_dwSize,
                                      pi_dwSize,
                                      1,
                                      D3DUSAGE_RENDERTARGET,
                                      D3DFMT_A8R8G8B8,
                                      D3DPOOL_DEFAULT,
                                      &l_pTexture)))
  {
    return 0;
  }
  return reinterpret_cast<int>(l_pTexture);
}

void CRender::EndShadow(void)
{
}

void CRender::DrawMesh(ObjectMesh *pi_pObjectMesh, D3DXMATRIX *pi_pWorldMatrix, float pi_fScroll)
{
  UNREFERENCED_PARAMETER(pi_fScroll);
  IDirect3DDevice8 *l_pDevice = GetRenderDevice(this);
  if (!l_pDevice || !pi_pObjectMesh || !pi_pWorldMatrix)
  {
    return;
  }

  IDirect3DVertexBuffer8 *l_pVertexBuffer =
    CharacterMR::Field<IDirect3DVertexBuffer8 *>(pi_pObjectMesh,
                                                CharacterMR::OBJ_VERTEX_BUFFER);
  const DWORD l_dwVertexCount =
    CharacterMR::Field<DWORD>(pi_pObjectMesh, CharacterMR::OBJ_VERTEX_COUNT);
  if (!l_pVertexBuffer || !l_dwVertexCount)
  {
    return;
  }

  l_pDevice->SetTransform(D3DTS_WORLD, pi_pWorldMatrix);
  l_pDevice->SetVertexShader(D3DFVF_CHARACTER_VERTEX);
  l_pDevice->SetStreamSource(0, l_pVertexBuffer, sizeof(CharacterVertex));
  l_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, l_dwVertexCount / 3);
}

void CRender::DrawMesh(CHARACTEROBJECT *pi_pCharacterObject, D3DXMATRIX &pio_rWorldMatrix)
{
  if (!pi_pCharacterObject || !pi_pCharacterObject->m_pMesh)
  {
    return;
  }

  for (DWORD i = 0; i < pi_pCharacterObject->m_MaxObj; ++i)
  {
    DrawMesh(&pi_pCharacterObject->m_pMesh[i], &pio_rWorldMatrix, 0.0f);
  }
}

void CRender::DrawCHFace(CHARACTEROBJECT *pi_pCharacterObject)
{
  if (pi_pCharacterObject)
  {
    D3DXMATRIX l_matIdentity;
    D3DXMatrixIdentity(&l_matIdentity);
    DrawCharacterMat(pi_pCharacterObject, &l_matIdentity, 0.0f);
  }
}

void CRender::DrawEdge(CHARACTEROBJECT *pi_pCharacterObject)
{
  DrawCHFace(pi_pCharacterObject);
}

int CRender::DrawShadow(int pi_nObjectIndex,
                        float pi_fAlpha,
                        const void *pi_pWorldMatrix,
                        float pi_fScale,
                        int pi_nArg6,
                        int pi_nArg7,
                        int pi_nArg8)
{
  UNREFERENCED_PARAMETER(pi_nObjectIndex);
  UNREFERENCED_PARAMETER(pi_fAlpha);
  UNREFERENCED_PARAMETER(pi_pWorldMatrix);
  UNREFERENCED_PARAMETER(pi_fScale);
  UNREFERENCED_PARAMETER(pi_nArg6);
  UNREFERENCED_PARAMETER(pi_nArg7);
  UNREFERENCED_PARAMETER(pi_nArg8);
  return 1;
}

void CRender::DrawShadow(unsigned long pi_dwObjectIndex,
                         float pi_fAlpha,
                         D3DXMATRIX *pi_pWorldMatrix,
                         float pi_fScale,
                         D3DXVECTOR3 pi_vecLight)
{
  DrawShadow(static_cast<int>(pi_dwObjectIndex),
             pi_fAlpha,
             pi_pWorldMatrix,
             pi_fScale,
             static_cast<int>(pi_vecLight.x),
             static_cast<int>(pi_vecLight.y),
             static_cast<int>(pi_vecLight.z));
}

void CRender::DrawSilhouette(CHARACTEROBJECT *pi_pCharacterObject)
{
  DrawCHFace(pi_pCharacterObject);
}

void CRender::EdgeFrameMove(CHARACTEROBJECT *pi_pCharacterObject, float pi_fFrame)
{
  FrameStep(pi_pCharacterObject, static_cast<int>(pi_fFrame));
}

CHARACTERVERTEX *CRender::GetCurrentCHfacelist(char **po_ppFaceList, int *po_pFaceCount, float pi_fFrame)
{
  UNREFERENCED_PARAMETER(pi_fFrame);
  if (po_ppFaceList)
  {
    *po_ppFaceList = NULL;
  }
  if (po_pFaceCount)
  {
    *po_pFaceCount = 0;
  }
  return NULL;
}

CHARACTERVERTEX *CRender::GetCurrentCHfacelist(CHARACTEROBJECT *pi_pCharacterObject,
                                               int *po_pFaceCount,
                                               float pi_fFrame)
{
  UNREFERENCED_PARAMETER(pi_pCharacterObject);
  return GetCurrentCHfacelist(static_cast<char **>(NULL), po_pFaceCount, pi_fFrame);
}

D3DXVECTOR4 *CRender::GetCurrentfacelist(char **po_ppFaceList, int *po_pFaceCount, float pi_fFrame)
{
  UNREFERENCED_PARAMETER(pi_fFrame);
  if (po_ppFaceList)
  {
    *po_ppFaceList = NULL;
  }
  if (po_pFaceCount)
  {
    *po_pFaceCount = 0;
  }
  return NULL;
}

D3DXVECTOR4 *CRender::GetCurrentfacelist(CHARACTEROBJECT *pi_pCharacterObject,
                                         int *po_pFaceCount,
                                         float pi_fFrame)
{
  UNREFERENCED_PARAMETER(pi_pCharacterObject);
  return GetCurrentfacelist(static_cast<char **>(NULL), po_pFaceCount, pi_fFrame);
}

void CRender::GetSilhouette(CHARACTEROBJECT *pi_pCharacterObject)
{
  UNREFERENCED_PARAMETER(pi_pCharacterObject);
}

void CRender::GetSilhouette(CHARACTEROBJECT *pi_pCharacterObject, D3DXVECTOR3 &pio_rEyeVector)
{
  UNREFERENCED_PARAMETER(pio_rEyeVector);
  GetSilhouette(pi_pCharacterObject);
}

void CRender::MakeEdgelist(CHARACTEROBJECT *pi_pCharacterObject)
{
  UNREFERENCED_PARAMETER(pi_pCharacterObject);
}

void CRender::MakeIB(IDirect3DIndexBuffer8 **po_ppIndexBuffer, WORD *pi_pwIndex, unsigned long pi_dwIndexCount)
{
  if (po_ppIndexBuffer)
  {
    *po_ppIndexBuffer = NULL;
  }
  IDirect3DDevice8 *l_pDevice = GetRenderDevice(this);
  if (!l_pDevice || !po_ppIndexBuffer || !pi_pwIndex || pi_dwIndexCount == 0)
  {
    return;
  }

  if (FAILED(l_pDevice->CreateIndexBuffer(sizeof(WORD) * pi_dwIndexCount,
                                          0,
                                          D3DFMT_INDEX16,
                                          D3DPOOL_MANAGED,
                                          po_ppIndexBuffer)))
  {
    return;
  }

  void *l_pData = NULL;
  if (SUCCEEDED((*po_ppIndexBuffer)->Lock(0, sizeof(WORD) * pi_dwIndexCount, reinterpret_cast<BYTE **>(&l_pData), 0)))
  {
    memcpy(l_pData, pi_pwIndex, sizeof(WORD) * pi_dwIndexCount);
    (*po_ppIndexBuffer)->Unlock();
  }
}

int CRender::MakeVB(IDirect3DVertexBuffer8 **po_ppVertexBuffer,
                    CHARACTERVERTEX *pi_pVertex,
                    unsigned long pi_dwVertexCount)
{
  if (po_ppVertexBuffer)
  {
    *po_ppVertexBuffer = NULL;
  }
  IDirect3DDevice8 *l_pDevice = GetRenderDevice(this);
  if (!l_pDevice || !po_ppVertexBuffer || !pi_pVertex || pi_dwVertexCount == 0)
  {
    return 0;
  }

  const UINT l_dwBytes = sizeof(CHARACTERVERTEX) * pi_dwVertexCount;
  if (FAILED(l_pDevice->CreateVertexBuffer(l_dwBytes,
                                           0,
                                           D3DFVF_CHARACTER_VERTEX,
                                           D3DPOOL_MANAGED,
                                           po_ppVertexBuffer)))
  {
    return 0;
  }

  BYTE *l_pData = NULL;
  if (FAILED((*po_ppVertexBuffer)->Lock(0, l_dwBytes, &l_pData, 0)))
  {
    (*po_ppVertexBuffer)->Release();
    *po_ppVertexBuffer = NULL;
    return 0;
  }
  memcpy(l_pData, pi_pVertex, l_dwBytes);
  (*po_ppVertexBuffer)->Unlock();
  return 1;
}

void ChInterface::DeleteAnimationData(AnimationManager *pi_pAniManager, char *pi_pAniName)
{
  if (pi_pAniManager)
  {
    pi_pAniManager->DeleteAnimation(pi_pAniName);
  }
}

void ChInterface::DeleteAnimationData(AnimationManager *pi_pAniManager, unsigned long pi_dwAniID)
{
  if (pi_pAniManager)
  {
    pi_pAniManager->DeleteAnimation2(pi_dwAniID);
  }
}

void ChInterface::SetTexPath(char *pi_pTexPath)
{
  char *l_pTexturePath = reinterpret_cast<char *>(this) + kChInterfaceTexturePathOffset;
  CopyStringSafe(l_pTexturePath, 0x64, pi_pTexPath);
}

void ChInterface::GetMinMaxSize(CHARACTEROBJECT *pi_pCharacterObject,
                                float *const po_pfMin,
                                float *const po_pfMax)
{
  if (!po_pfMin || !po_pfMax)
  {
    return;
  }

  po_pfMin[0] = po_pfMin[1] = po_pfMin[2] = FLT_MAX;
  po_pfMax[0] = po_pfMax[1] = po_pfMax[2] = -FLT_MAX;
  if (!pi_pCharacterObject)
  {
    return;
  }

  const DWORD l_dwObjectCount =
    CharacterMR::Field<DWORD>(pi_pCharacterObject, CharacterMR::CHOBJ_MAX_OBJ);
  for (DWORD i = 0; i < l_dwObjectCount; ++i)
  {
    ObjectMesh *l_pMesh = GetMeshAt(pi_pCharacterObject, i);
    if (!l_pMesh)
    {
      continue;
    }
    const float *l_pfMax = &CharacterMR::Field<float>(l_pMesh, 0x3CC);
    const float *l_pfMin = &CharacterMR::Field<float>(l_pMesh, 0x3D8);
    for (int axis = 0; axis < 3; ++axis)
    {
      po_pfMin[axis] = (std::min)(po_pfMin[axis], l_pfMin[axis]);
      po_pfMax[axis] = (std::max)(po_pfMax[axis], l_pfMax[axis]);
    }
  }
}

void ChInterface::SetNewParent(CHARACTEROBJECT *pi_pParent,
                               char *pi_pDummyName,
                               D3DXMATRIX *pi_pMatrix,
                               CHARACTEROBJECT *pi_pCharacterObject)
{
  if (!pi_pParent || !pi_pDummyName || !pi_pMatrix || !pi_pCharacterObject)
  {
    return;
  }

  ObjectMesh *l_pParentMesh = FindObjectMeshByName(
    CharacterMR::Field<ObjectMesh *>(pi_pParent, CharacterMR::CHOBJ_MESH),
    static_cast<int>(CharacterMR::Field<DWORD>(pi_pParent, CharacterMR::CHOBJ_MAX_OBJ)),
    pi_pDummyName);
  if (!l_pParentMesh)
  {
    l_pParentMesh = CharacterMR::Field<ObjectMesh *>(pi_pParent, CharacterMR::CHOBJ_MESH);
  }

  const DWORD l_dwObjectCount =
    CharacterMR::Field<DWORD>(pi_pCharacterObject, CharacterMR::CHOBJ_MAX_OBJ);
  for (DWORD i = 0; i < l_dwObjectCount; ++i)
  {
    ObjectMesh *l_pChild = GetMeshAt(pi_pCharacterObject, i);
    if (!l_pChild || CharacterMR::Field<BYTE>(l_pChild, CharacterMR::OBJ_HAS_PARENT))
    {
      continue;
    }

    CharacterMR::Field<BYTE>(l_pChild, CharacterMR::OBJ_PARENT_CHANGED) = 1;
    CharacterMR::Field<BYTE>(l_pChild, CharacterMR::OBJ_HAS_PARENT) = 1;
    CharacterMR::Field<ObjectMesh *>(l_pChild, CharacterMR::OBJ_PARENT) = l_pParentMesh;
    D3DXMatrixMultiply(&CharacterMR::Field<D3DXMATRIX>(l_pChild, CharacterMR::OBJ_BIND_MATRIX),
                       &CharacterMR::Field<D3DXMATRIX>(l_pChild, CharacterMR::OBJ_BIND_MATRIX),
                       pi_pMatrix);
  }
}

void ChInterface::SetNewParent(CHARACTEROBJECT *pi_pParent,
                               char *pi_pDummyName,
                               D3DXMATRIX &pio_rMatrix,
                               CHARACTEROBJECT *pi_pCharacterObject)
{
  SetNewParent(pi_pParent, pi_pDummyName, &pio_rMatrix, pi_pCharacterObject);
}

void ChInterface::LoadVertexShader(char *pi_pFileName)
{
  UNREFERENCED_PARAMETER(pi_pFileName);
}

void ChInterface::SetVertexShader(D3DXMATRIX pi_matWorld, CHARACTEROBJECT *pi_pCharacterObject)
{
  UNREFERENCED_PARAMETER(pi_matWorld);
  UNREFERENCED_PARAMETER(pi_pCharacterObject);
}

void __stdcall ChInterface::SetVertexShader(int pi_nArg1,
                                            int pi_nArg2,
                                            int pi_nArg3,
                                            int pi_nArg4,
                                            int pi_nArg5,
                                            int pi_nArg6,
                                            int pi_nArg7,
                                            int pi_nArg8,
                                            int pi_nArg9,
                                            int pi_nArg10,
                                            int pi_nArg11,
                                            int pi_nArg12,
                                            int pi_nArg13,
                                            int pi_nArg14,
                                            int pi_nArg15,
                                            int pi_nArg16,
                                            int pi_nArg17)
{
  UNREFERENCED_PARAMETER(pi_nArg1);
  UNREFERENCED_PARAMETER(pi_nArg2);
  UNREFERENCED_PARAMETER(pi_nArg3);
  UNREFERENCED_PARAMETER(pi_nArg4);
  UNREFERENCED_PARAMETER(pi_nArg5);
  UNREFERENCED_PARAMETER(pi_nArg6);
  UNREFERENCED_PARAMETER(pi_nArg7);
  UNREFERENCED_PARAMETER(pi_nArg8);
  UNREFERENCED_PARAMETER(pi_nArg9);
  UNREFERENCED_PARAMETER(pi_nArg10);
  UNREFERENCED_PARAMETER(pi_nArg11);
  UNREFERENCED_PARAMETER(pi_nArg12);
  UNREFERENCED_PARAMETER(pi_nArg13);
  UNREFERENCED_PARAMETER(pi_nArg14);
  UNREFERENCED_PARAMETER(pi_nArg15);
  UNREFERENCED_PARAMETER(pi_nArg16);
  UNREFERENCED_PARAMETER(pi_nArg17);
}

CParser::CParser(void)
  : m_nStatus(0)
{
}

CParser::~CParser(void)
{
}

std::string CParser::GetName(void)
{
  return m_sName;
}

void CParser::Parsing(std::string &pi_rToken)
{
  UNREFERENCED_PARAMETER(pi_rToken);
  m_nStatus = 1;
  CScript::SetParser(m_sName);
}

CScript *CScript::s_pCurrentScript = NULL;
CParser *CScript::s_pCurrentParser = NULL;
CScript *CScript::m_pScript = NULL;

CScript::CScript(void)
  : m_nState(0),
    m_stTokenIndex(0)
{
}

CScript::~CScript(void)
{
}

int CScript::AddParser(CParser *pi_pParser)
{
  if (!pi_pParser)
  {
    return 0;
  }

  std::string l_sName = pi_pParser->GetName();
  std::transform(l_sName.begin(), l_sName.end(), l_sName.begin(), ::toupper);
  m_mapParsers[l_sName] = pi_pParser;
  s_pCurrentScript = this;
  m_pScript = this;
  return 1;
}

int CScript::SetParser(CParser *pi_pParser)
{
  s_pCurrentParser = pi_pParser;
  return pi_pParser != NULL;
}

int CScript::SetParser(std::string &pio_rName)
{
  CParser *l_pParser = GetParser(pio_rName);
  s_pCurrentParser = l_pParser;
  return l_pParser != NULL;
}

CParser *CScript::GetParser(char *pi_pName)
{
  CScript *l_pScript = s_pCurrentScript ? s_pCurrentScript : m_pScript;
  if (!l_pScript || !pi_pName)
  {
    return NULL;
  }

  std::string l_sName = pi_pName;
  std::transform(l_sName.begin(), l_sName.end(), l_sName.begin(), ::toupper);
  std::map<std::string, CParser *>::iterator it =
    l_pScript->m_mapParsers.find(l_sName);
  return it == l_pScript->m_mapParsers.end() ? NULL : it->second;
}

CParser *CScript::GetParser(std::string &pio_rName)
{
  return GetParser(const_cast<char *>(pio_rName.c_str()));
}

int CScript::GetToken(std::string &po_rToken)
{
  CScript *l_pScript = s_pCurrentScript ? s_pCurrentScript : m_pScript;
  if (!l_pScript || l_pScript->m_stTokenIndex >= l_pScript->m_vecTokens.size())
  {
    return 0;
  }
  po_rToken = l_pScript->m_vecTokens[l_pScript->m_stTokenIndex++];
  return 1;
}

char *CScript::StrTok(char *pi_pString,
                      char *pi_pSkip,
                      char *pi_pDelimiter,
                      unsigned long pi_dwLength,
                      unsigned long &pio_rOffset)
{
  unsigned int l_dwOffset = static_cast<unsigned int>(pio_rOffset);
  char *l_pToken = StrTok(pi_pString,
                          pi_pSkip,
                          pi_pDelimiter,
                          static_cast<unsigned int>(pi_dwLength),
                          &l_dwOffset);
  pio_rOffset = l_dwOffset;
  return l_pToken;
}

char *CScript::StrTok(char *pi_pString,
                      char *pi_pSkip,
                      char *pi_pDelimiter,
                      unsigned int pi_dwLength,
                      unsigned int *pio_pdwOffset)
{
  if (!pi_pString || !pio_pdwOffset || *pio_pdwOffset >= pi_dwLength)
  {
    return NULL;
  }

  unsigned int l_dwOffset = *pio_pdwOffset;
  while (l_dwOffset < pi_dwLength &&
         pi_pSkip &&
         strchr(pi_pSkip, pi_pString[l_dwOffset]))
  {
    ++l_dwOffset;
  }

  if (l_dwOffset < pi_dwLength && pi_pString[l_dwOffset] == ';')
  {
    while (l_dwOffset < pi_dwLength && pi_pString[l_dwOffset] != '\n')
    {
      ++l_dwOffset;
    }
  }

  const bool l_bQuoted = l_dwOffset < pi_dwLength && pi_pString[l_dwOffset] == '"';
  if (l_bQuoted)
  {
    ++l_dwOffset;
  }

  const unsigned int l_dwBegin = l_dwOffset;
  while (l_dwOffset < pi_dwLength)
  {
    const char l_chValue = pi_pString[l_dwOffset];
    if ((l_bQuoted && l_chValue == '"') ||
        (!l_bQuoted &&
         ((pi_pDelimiter && strchr(pi_pDelimiter, l_chValue)) ||
          (pi_pSkip && strchr(pi_pSkip, l_chValue)))))
    {
      break;
    }
    ++l_dwOffset;
  }

  const unsigned int l_dwLength = std::min<unsigned int>(l_dwOffset - l_dwBegin,
                                                         sizeof(g_szStrTokBuffer) - 1);
  memcpy(g_szStrTokBuffer, pi_pString + l_dwBegin, l_dwLength);
  g_szStrTokBuffer[l_dwLength] = '\0';
  *pio_pdwOffset = l_dwOffset + (l_bQuoted ? 1 : 0);
  return g_szStrTokBuffer;
}

void CScript::ScriptLexer(char *pi_pString)
{
  m_vecTokens.clear();
  m_stTokenIndex = 0;
  s_pCurrentScript = this;
  m_pScript = this;
  if (!pi_pString)
  {
    m_nState = 2;
    return;
  }

  static char s_szSeps[] = " \t\r\n,";
  char *l_pContext = NULL;
  char *l_pToken = strtok_s(pi_pString, s_szSeps, &l_pContext);
  while (l_pToken)
  {
    UppercaseInPlace(l_pToken);
    m_vecTokens.push_back(l_pToken);
    l_pToken = strtok_s(NULL, s_szSeps, &l_pContext);
  }
  m_nState = 2;
}

void CScript::ScriptFlieLexer(char *pi_pFileName)
{
  std::vector<BYTE> l_vecBuffer;
  if (!CharacterMR::LoadBinaryFile(pi_pFileName, l_vecBuffer))
  {
    m_vecTokens.clear();
    m_nState = 2;
    return;
  }
  l_vecBuffer.push_back('\0');
  ScriptLexer(reinterpret_cast<char *>(&l_vecBuffer[0]));
}

int CScript::ScriptParser(void)
{
  std::string l_sToken;
  while (GetToken(l_sToken))
  {
    CParser *l_pParser = GetParser(const_cast<char *>(l_sToken.c_str()));
    if (l_pParser)
    {
      l_pParser->Parsing(l_sToken);
    }
  }
  return 1;
}

EffectObject::EffectObject(void)
{
  ZeroMemory(m_abData, sizeof(m_abData));
}

EffectObject::EffectObject(const EffectObject &pi_rOther)
{
  memcpy(m_abData, pi_rOther.m_abData, sizeof(m_abData));
}

EffectObject::~EffectObject(void)
{
}

CEffectMap::CEffectMap(void)
{
}

CEffectMap::CEffectMap(const CEffectMap &pi_rOther)
  : m_mapObjects(pi_rOther.m_mapObjects)
{
}

CEffectMap::~CEffectMap(void)
{
  m_mapObjects.clear();
}

EffectFileList::EffectFileList(void)
{
}

EffectFileList g_EffectFileList;
ItemEffectList g_EffectItemList;
EffectList g_EFFMonList;
EffectList g_EFFNPCList;
EffectList g_EFFUnitList;
EffectPatternList g_PatternList;
EffectObjectManager EffectObjectManager::m_This;

void EffectFileList::LoadEffectFileList(char *pi_pFileName)
{
  FILE *l_pFile = fopen(pi_pFileName, "rb");
  if (!l_pFile)
  {
    return;
  }

  DWORD l_dwID = 0;
  char l_szFileName[512] = {};
  int l_nType = 0;
  while (fscanf(l_pFile, "%lu%511s%d", &l_dwID, l_szFileName, &l_nType) == 3)
  {
    UNREFERENCED_PARAMETER(l_nType);
    g_mapEffectFiles[l_dwID] = l_szFileName;
    g_mapEffectFileIDs[MakeUpperKey(l_szFileName)] = l_dwID;
  }
  fclose(l_pFile);
}

void EffectList::LoadEffectFileList(char *pi_pFileName)
{
  EffectFileList l_stList;
  l_stList.LoadEffectFileList(pi_pFileName);
}

void EffectPatternList::LoadEffectPatternList(char *pi_pFileName)
{
  FILE *l_pFile = fopen(pi_pFileName, "rb");
  if (!l_pFile)
  {
    return;
  }

  DWORD l_dwPattern = 0;
  DWORD l_adwEffect[9] = {};
  while (fscanf(l_pFile,
                "%lu%lu%lu%lu%lu%lu%lu%lu%lu%lu",
                &l_dwPattern,
                &l_adwEffect[0],
                &l_adwEffect[1],
                &l_adwEffect[2],
                &l_adwEffect[3],
                &l_adwEffect[4],
                &l_adwEffect[5],
                &l_adwEffect[6],
                &l_adwEffect[7],
                &l_adwEffect[8]) == 10)
  {
    g_mapItemEffectPattern[l_dwPattern] = l_adwEffect[0];
  }
  fclose(l_pFile);
}

ItemEffectList::ItemEffectList(void)
{
}

ItemEffectList::~ItemEffectList(void)
{
}

void ItemEffectList::LoadItemListFile(char *pi_pFileName)
{
  FILE *l_pFile = fopen(pi_pFileName, "rb");
  if (!l_pFile)
  {
    return;
  }

  DWORD l_dwMeshID = 0;
  DWORD l_adwPattern[6] = {};
  while (fscanf(l_pFile,
                "%lx%lu%lu%lu%lu%lu%lu",
                &l_dwMeshID,
                &l_adwPattern[0],
                &l_adwPattern[1],
                &l_adwPattern[2],
                &l_adwPattern[3],
                &l_adwPattern[4],
                &l_adwPattern[5]) == 7)
  {
    g_mapItemEffectPattern[l_dwMeshID] = l_adwPattern[0];
  }
  fclose(l_pFile);
}

EffectObjectManager::EffectObjectManager(void)
{
}

EffectObjectManager::~EffectObjectManager(void)
{
  Clear();
}

void EffectObjectManager::Clear(void)
{
  g_mapEffectObjects.clear();
}

void EffectObjectManager::LoadEffect(char *pi_pFileName)
{
  if (!pi_pFileName)
  {
    return;
  }
  CEffectMap &l_rMap = g_mapEffectObjects[MakeUpperKey(pi_pFileName)];
  LoadEffectObjectFileIntoMap(pi_pFileName, l_rMap);
}

void EffectObjectManager::ExportEffect(char *pi_pFileName, char *pi_pOutFileName)
{
  UNREFERENCED_PARAMETER(pi_pFileName);
  UNREFERENCED_PARAMETER(pi_pOutFileName);
}

int EffectObjectManager::GetEffect(char pi_chType,
                                   int pi_nEffectID,
                                   int pi_nPass,
                                   int pi_nLevel,
                                   DWORD *po_pdwEffectID,
                                   int pi_nFallback)
{
  UNREFERENCED_PARAMETER(pi_chType);
  UNREFERENCED_PARAMETER(pi_nPass);
  UNREFERENCED_PARAMETER(pi_nFallback);
  const DWORD l_dwKey = static_cast<DWORD>(pi_nEffectID);
  std::map<DWORD, DWORD>::iterator it = g_mapItemEffectPattern.find(l_dwKey);
  if (it == g_mapItemEffectPattern.end())
  {
    it = g_mapItemEffectPattern.find(static_cast<DWORD>(pi_nLevel));
  }
  if (it == g_mapItemEffectPattern.end())
  {
    return 0;
  }
  if (po_pdwEffectID)
  {
    *po_pdwEffectID = it->second;
  }
  return 1;
}

int EffectObjectManager::AddEffect(char pi_chType, int pi_nEffectID, char *pi_pFileName)
{
  UNREFERENCED_PARAMETER(pi_chType);
  if (!pi_pFileName)
  {
    return 0;
  }
  g_mapEffectFiles[static_cast<DWORD>(pi_nEffectID)] = pi_pFileName;
  g_mapEffectFileIDs[MakeUpperKey(pi_pFileName)] = static_cast<DWORD>(pi_nEffectID);
  return 1;
}

int EffectObjectManager::AddEffect(char pi_chType,
                                   int pi_nEffectID,
                                   int pi_nArg3,
                                   int pi_nArg4,
                                   char pi_chArg5)
{
  UNREFERENCED_PARAMETER(pi_chType);
  UNREFERENCED_PARAMETER(pi_nArg3);
  UNREFERENCED_PARAMETER(pi_nArg4);
  UNREFERENCED_PARAMETER(pi_chArg5);
  return pi_nEffectID ? 1 : 0;
}

void EffectObjectManager::AddEffect(EffectObject pi_stEffectObject)
{
  char l_szName[64] = {};
  memcpy(l_szName,
         pi_stEffectObject.m_abData,
         std::min<size_t>(sizeof(l_szName) - 1, sizeof(pi_stEffectObject.m_abData)));
  UppercaseInPlace(l_szName);
  if (!l_szName[0])
  {
    strcpy_s(l_szName, sizeof(l_szName), "DEFAULT");
  }

  CEffectMap &l_rMap = g_mapEffectObjects[l_szName];
  l_rMap.m_mapObjects[l_szName] = pi_stEffectObject;
}

int EffectObjectManager::SetEffect(void)
{
  return 1;
}

int EffectObjectManager::SetEffect(CHARACTEROBJECT *pi_pCharacterObject, char *pi_pEffectName)
{
  if (!pi_pCharacterObject || !pi_pEffectName)
  {
    return 0;
  }

  ApplyMeshEffect(pi_pCharacterObject, 0, 0);
  LoadEffect(pi_pEffectName);
  return 1;
}

void EffectObjectManager::DelEffect(int pi_nEffectID, int pi_nLevel)
{
  UNREFERENCED_PARAMETER(pi_nLevel);
  g_mapEffectFiles.erase(static_cast<DWORD>(pi_nEffectID));
}

void EffectObjectManager::SetEffectToMesh(ObjectMesh *pio_pObjectMesh,
                                          EffectObject *pi_pEffectObject,
                                          int pi_nPassLevel)
{
  if (!pio_pObjectMesh || !pi_pEffectObject)
  {
    return;
  }

  memcpy(reinterpret_cast<BYTE *>(pio_pObjectMesh) + 0x36C,
         pi_pEffectObject->m_abData,
         std::min<size_t>(0xC4, sizeof(pi_pEffectObject->m_abData)));
  CharacterMR::Field<DWORD>(pio_pObjectMesh, 0x328) =
    static_cast<DWORD>(pi_nPassLevel);
}

int EffectObjectManager::SetPartEffect(CHARACTEROBJECT *pi_pCharacterObject,
                                       char *pi_pPartName,
                                       char *pi_pEffectName)
{
  if (!pi_pCharacterObject || !pi_pPartName || !pi_pEffectName)
  {
    return 0;
  }
  EffectObjectManager l_stManager;
  l_stManager.SetPartEffectToMesh(pi_pCharacterObject, pi_pPartName, pi_pEffectName);
  return 1;
}

int EffectObjectManager::SetPartEffect(CHARACTEROBJECT *pi_pCharacterObject,
                                       char *pi_pPartName,
                                       int pi_nLevel)
{
  if (!pi_pCharacterObject || !pi_pPartName)
  {
    return 0;
  }

  for (DWORD i = 0; i < pi_pCharacterObject->m_MaxObj; ++i)
  {
    ObjectMesh *l_pMesh = GetMeshAt(pi_pCharacterObject, i);
    if (!l_pMesh)
    {
      continue;
    }
    if (!_stricmp(reinterpret_cast<const char *>(l_pMesh) + CharacterMR::OBJ_NAME, pi_pPartName))
    {
      CharacterMR::Field<DWORD>(l_pMesh, 0x328) = static_cast<DWORD>(pi_nLevel);
    }
  }
  return 1;
}

void EffectObjectManager::SetEffectToMesh(CHARACTEROBJECT *pi_pCharacterObject,
                                          DWORD pi_dwMeshID,
                                          DWORD pi_dwLevel)
{
  ApplyMeshEffect(pi_pCharacterObject, pi_dwMeshID, pi_dwLevel);
}

void EffectObjectManager::SetPartEffectToMesh(ObjectMesh *pio_pObjectMesh,
                                              char pi_chType,
                                              int pi_nArg4,
                                              int pi_nArg5,
                                              int pi_nArg6,
                                              int pi_nArg7)
{
  UNREFERENCED_PARAMETER(pi_chType);
  UNREFERENCED_PARAMETER(pi_nArg4);
  UNREFERENCED_PARAMETER(pi_nArg5);
  UNREFERENCED_PARAMETER(pi_nArg6);
  UNREFERENCED_PARAMETER(pi_nArg7);
  if (pio_pObjectMesh)
  {
    CharacterMR::Field<DWORD>(pio_pObjectMesh, 0x328) = 1;
  }
}

void EffectObjectManager::SetPartEffectToMesh(CHARACTEROBJECT *pi_pCharacterObject,
                                              char *pi_pPartName,
                                              char *pi_pEffectName)
{
  if (!pi_pCharacterObject || !pi_pPartName || !pi_pEffectName)
  {
    return;
  }

  EffectObjectManager::LoadEffect(pi_pEffectName);
  CEffectMap &l_rMap = g_mapEffectObjects[MakeUpperKey(pi_pEffectName)];
  std::string l_sPart = MakeUpperKey(pi_pPartName);
  std::map<std::string, EffectObject>::iterator l_it = l_rMap.m_mapObjects.find(l_sPart);
  if (l_it == l_rMap.m_mapObjects.end())
  {
    l_it = l_rMap.m_mapObjects.find("");
  }
  if (l_it == l_rMap.m_mapObjects.end())
  {
    return;
  }

  for (DWORD i = 0; i < pi_pCharacterObject->m_MaxObj; ++i)
  {
    ObjectMesh *l_pMesh = GetMeshAt(pi_pCharacterObject, i);
    if (!l_pMesh)
    {
      continue;
    }
    if (_stricmp(reinterpret_cast<const char *>(l_pMesh) + CharacterMR::OBJ_NAME, pi_pPartName))
    {
      continue;
    }
    memcpy(reinterpret_cast<BYTE *>(l_pMesh) + 0x36C,
           l_it->second.m_abData,
           std::min<size_t>(0xC4, sizeof(l_it->second.m_abData)));
  }
}

void EffectMainParser::Parsing(int pi_nArg)
{
  UNREFERENCED_PARAMETER(pi_nArg);
}

bool IsHaveMeshEffect(CHARACTEROBJECT *pi_pCharacterObject,
                      DWORD pi_dwMeshID,
                      DWORD pi_dwLevel,
                      DWORD pi_dwArg4)
{
  UNREFERENCED_PARAMETER(pi_dwArg4);
  return HasMeshEffect(pi_pCharacterObject, pi_dwMeshID, pi_dwLevel);
}

void SetMeshEffect(CHARACTEROBJECT *pi_pCharacterObject,
                   DWORD pi_dwMeshID,
                   DWORD pi_dwLevel,
                   DWORD pi_dwArg4)
{
  UNREFERENCED_PARAMETER(pi_dwArg4);
  ApplyMeshEffect(pi_pCharacterObject, pi_dwMeshID, pi_dwLevel);
}

ObjectMesh *AddObjectMesh(ObjectMesh *pi_pBaseMesh,
                          unsigned long pi_dwBaseCount,
                          ObjectMesh *pi_pAppendMesh,
                          unsigned long pi_dwAppendCount)
{
  const unsigned long l_dwNewCount = pi_dwBaseCount + pi_dwAppendCount;
  if (l_dwNewCount == 0)
  {
    return NULL;
  }

  ObjectMesh *l_pMesh = static_cast<ObjectMesh *>(::operator new(sizeof(ObjectMesh) * l_dwNewCount));
  for (unsigned long i = 0; i < l_dwNewCount; ++i)
  {
    CharacterMR::InitObjectMesh(&l_pMesh[i]);
  }
  if (pi_pBaseMesh && pi_dwBaseCount)
  {
    memcpy(l_pMesh, pi_pBaseMesh, sizeof(ObjectMesh) * pi_dwBaseCount);
  }
  if (pi_pAppendMesh && pi_dwAppendCount)
  {
    memcpy(l_pMesh + pi_dwBaseCount, pi_pAppendMesh, sizeof(ObjectMesh) * pi_dwAppendCount);
  }
  return l_pMesh;
}

ObjectMesh *FindParent(char *pi_pName, ObjectMesh *pi_pObjectMesh, int pi_nCount)
{
  return FindObjectMeshByName(pi_pObjectMesh, pi_nCount, pi_pName);
}

int FindParent(ObjectMesh *pi_pParentMesh,
               int pi_nParentCount,
               ObjectMesh *pio_pChildMesh,
               int pi_nChildCount)
{
  int l_nMatched = 0;
  for (int i = 0; i < pi_nChildCount; ++i)
  {
    ObjectMesh *l_pChild =
      reinterpret_cast<ObjectMesh *>(reinterpret_cast<BYTE *>(pio_pChildMesh) +
                                     (i * sizeof(ObjectMesh)));
    ObjectMesh *l_pParent =
      FindObjectMeshByName(pi_pParentMesh,
                           pi_nParentCount,
                           reinterpret_cast<const char *>(l_pChild) + CharacterMR::OBJ_PARENT_NAME);
    if (l_pParent)
    {
      CharacterMR::Field<ObjectMesh *>(l_pChild, CharacterMR::OBJ_PARENT) = l_pParent;
      CharacterMR::Field<BYTE>(l_pChild, CharacterMR::OBJ_HAS_PARENT) = 1;
      ++l_nMatched;
    }
  }
  return l_nMatched;
}

void FindBlendParent(ObjectMesh *pi_pParentMesh,
                     int pi_nParentCount,
                     ObjectMesh *pio_pChildMesh,
                     int pi_nChildCount,
                     ObjectData *pi_pObjectData)
{
  UNREFERENCED_PARAMETER(pi_pObjectData);
  FindParent(pi_pParentMesh, pi_nParentCount, pio_pChildMesh, pi_nChildCount);
}

int RematchParent(ObjectMesh *pi_pParentMesh,
                  int pi_nParentCount,
                  ObjectMesh *pio_pChildMesh,
                  int pi_nChildCount)
{
  return FindParent(pi_pParentMesh, pi_nParentCount, pio_pChildMesh, pi_nChildCount);
}

void ConvertVtToD3D(CHARACTERVERTEX *po_pVertex, EXPTVERTEX *pi_pVertex)
{
  if (!po_pVertex || !pi_pVertex)
  {
    return;
  }
  BYTE *l_pSrc = reinterpret_cast<BYTE *>(pi_pVertex);
  po_pVertex->x = *reinterpret_cast<float *>(l_pSrc + 0);
  po_pVertex->y = *reinterpret_cast<float *>(l_pSrc + 4);
  po_pVertex->z = *reinterpret_cast<float *>(l_pSrc + 8);
  po_pVertex->blend = 1.0f;
  po_pVertex->nx = *reinterpret_cast<float *>(l_pSrc + 16);
  po_pVertex->ny = *reinterpret_cast<float *>(l_pSrc + 20);
  po_pVertex->nz = *reinterpret_cast<float *>(l_pSrc + 24);
  po_pVertex->u = 0.0f;
  po_pVertex->v = 0.0f;
}

void ConvertVtToD3D(CHARACTERVERTEX &po_rVertex, EXPTVERTEX &pi_rVertex)
{
  ConvertVtToD3D(&po_rVertex, &pi_rVertex);
}

int ConvertUV(CHARACTERVERTEX *pio_pVertex, float pi_fU, float pi_fV)
{
  if (!pio_pVertex)
  {
    return 0;
  }
  pio_pVertex->u = pi_fU;
  pio_pVertex->v = 1.0f - pi_fV;
  return 1;
}

void ConvertUV(CHARACTERVERTEX &pio_rVertex, tagTEXCOORDFLOAT pi_stTexCoord)
{
  ConvertUV(&pio_rVertex, pi_stTexCoord.m_fU, pi_stTexCoord.m_fV);
}

void DecodeTwoBlendPhysiqueData(CHARACTERVERTEX *pio_pVertex,
                                int pi_nVertexCount,
                                PHYSIQUE *pi_pPhysique,
                                int pi_nPhysiqueCount)
{
  UNREFERENCED_PARAMETER(pi_pPhysique);
  UNREFERENCED_PARAMETER(pi_nPhysiqueCount);
  for (int i = 0; pio_pVertex && i < pi_nVertexCount; ++i)
  {
    pio_pVertex[i].blend = 1.0f;
  }
}

int MatchAnimationData(const unsigned char *pi_pAnimationHeader,
                       CHARACTEROBJECT *pio_pCharacterObject,
                       unsigned char *pi_pAnimationBytes)
{
  if (!pi_pAnimationHeader || !pio_pCharacterObject || !pi_pAnimationBytes)
  {
    return 0;
  }

  const char *l_pName = reinterpret_cast<const char *>(pi_pAnimationHeader);
  const WORD l_wRotCount = *reinterpret_cast<const WORD *>(pi_pAnimationHeader + 100);
  const WORD l_wTransCount = *reinterpret_cast<const WORD *>(pi_pAnimationHeader + 108);
  const WORD l_wScaleCount = *reinterpret_cast<const WORD *>(pi_pAnimationHeader + 116);
  const WORD l_wAlphaCount = *reinterpret_cast<const WORD *>(pi_pAnimationHeader + 124);
  BYTE *l_pKeyData = const_cast<BYTE *>(pi_pAnimationHeader + 132);

  const DWORD l_dwObjectCount =
    CharacterMR::Field<DWORD>(pio_pCharacterObject, CharacterMR::CHOBJ_MAX_OBJ);
  for (DWORD i = 0; i < l_dwObjectCount; ++i)
  {
    ObjectMesh *l_pMesh = GetMeshAt(pio_pCharacterObject, i);
    if (!l_pMesh ||
        _stricmp(reinterpret_cast<const char *>(l_pMesh) + CharacterMR::OBJ_NAME, l_pName))
    {
      continue;
    }

    FreeAnimationData(l_pMesh);
    CharacterMRAnimationData *l_pData =
      static_cast<CharacterMRAnimationData *>(::operator new(sizeof(CharacterMRAnimationData)));
    ZeroMemory(l_pData, sizeof(CharacterMRAnimationData));
    l_pData->m_wRotCount = l_wRotCount;
    l_pData->m_pRotKeys = l_pKeyData;
    l_pKeyData += static_cast<size_t>(l_wRotCount) * 20;
    l_pData->m_wTransCount = l_wTransCount;
    l_pData->m_pTransKeys = l_pKeyData;
    l_pKeyData += static_cast<size_t>(l_wTransCount) * 16;
    l_pData->m_wScaleCount = l_wScaleCount;
    l_pData->m_pScaleKeys = l_pKeyData;
    l_pKeyData += static_cast<size_t>(l_wScaleCount) * 16;
    l_pData->m_wAlphaCount = l_wAlphaCount;
    l_pData->m_pAlphaKeys = l_pKeyData;

    CharacterMR::Field<BYTE>(l_pMesh, CharacterMR::OBJ_ANIMATION_ENABLED) = 1;
    CharacterMR::Field<CharacterMRAnimationData *>(l_pMesh,
                                                  CharacterMR::OBJ_ANIMATION_DATA) = l_pData;
    CharacterMR::Field<DWORD>(l_pMesh, CharacterMR::OBJ_ANIMATION_MAX_FRAME) =
      FindMaxAnimationFrame(pi_pAnimationBytes);
    return 1;
  }
  return 0;
}

int MatchAnimationData(AnimationHeader *pi_pAnimationHeader,
                       AnimationData *pi_pAnimationData,
                       CHARACTEROBJECT *pio_pCharacterObject,
                       int pi_nStartIndex)
{
  if (!pi_pAnimationHeader || !pio_pCharacterObject)
  {
    if (pi_pAnimationData)
    {
      ::operator delete(pi_pAnimationData);
    }
    return -1;
  }

  const DWORD l_dwObjectCount =
    CharacterMR::Field<DWORD>(pio_pCharacterObject, CharacterMR::CHOBJ_MAX_OBJ);
  if (pi_nStartIndex >= static_cast<int>(l_dwObjectCount))
  {
    return -1;
  }

  const char *l_pName = reinterpret_cast<const char *>(pi_pAnimationHeader);
  for (DWORD i = static_cast<DWORD>(pi_nStartIndex); i < l_dwObjectCount; ++i)
  {
    ObjectMesh *l_pMesh = GetMeshAt(pio_pCharacterObject, i);
    if (!l_pMesh ||
        _stricmp(reinterpret_cast<const char *>(l_pMesh) + CharacterMR::OBJ_NAME, l_pName))
    {
      continue;
    }

    FreeAnimationData(l_pMesh);
    CharacterMR::Field<AnimationData *>(l_pMesh, CharacterMR::OBJ_ANIMATION_DATA) = pi_pAnimationData;
    CharacterMR::Field<BYTE>(l_pMesh, CharacterMR::OBJ_ANIMATION_ENABLED) =
      pi_pAnimationData ? 1 : 0;
    return static_cast<int>(i);
  }

  if (pi_pAnimationData)
  {
    ::operator delete(pi_pAnimationData);
  }
  return -1;
}

void ImportAnimationData(CHARACTEROBJECT *pio_pCharacterObject, unsigned char *pi_pAnimationBytes)
{
  if (!pio_pCharacterObject || !pi_pAnimationBytes)
  {
    return;
  }

  const WORD l_wObjectCount = *reinterpret_cast<WORD *>(pi_pAnimationBytes);
  BYTE *l_pHeader = pi_pAnimationBytes + 2;
  for (WORD i = 0; i < l_wObjectCount; ++i)
  {
    MatchAnimationData(l_pHeader, pio_pCharacterObject, pi_pAnimationBytes);
    const WORD l_wRot = *reinterpret_cast<WORD *>(l_pHeader + 100);
    const WORD l_wTrans = *reinterpret_cast<WORD *>(l_pHeader + 108);
    const WORD l_wScale = *reinterpret_cast<WORD *>(l_pHeader + 116);
    const WORD l_wAlpha = *reinterpret_cast<WORD *>(l_pHeader + 124);
    l_pHeader += 132 +
                 static_cast<size_t>(l_wRot) * 20 +
                 static_cast<size_t>(l_wTrans) * 16 +
                 static_cast<size_t>(l_wScale) * 16 +
                 static_cast<size_t>(l_wAlpha) * 8;
  }
  CharacterMR::Field<DWORD>(pio_pCharacterObject, CharacterMR::CHOBJ_MAX_FRAME) =
    FindMaxAnimationFrame(pi_pAnimationBytes);
}

void ImportTextureData(TextureManager *pi_pTextureManager, char *pi_pPathName, char *pi_pFileName)
{
  if (pi_pTextureManager)
  {
    pi_pTextureManager->AddTexture(pi_pPathName, pi_pFileName);
  }
}

void FindMaxAni(ChAnimation *pio_pAnimation, unsigned char *pi_pAnimationBytes)
{
  if (pio_pAnimation)
  {
    CharacterMR::Field<DWORD>(pio_pAnimation, 0x110) = FindMaxAnimationFrame(pi_pAnimationBytes);
  }
}

unsigned char *ReadAnimationData(char *pi_pFileName)
{
  return reinterpret_cast<unsigned char *>(CharacterMR::LoadBinaryFileAlloc(pi_pFileName));
}

void ScaleLoad(char *pi_pFileName, CHARACTEROBJECT *pio_pCharacterObject)
{
  UNREFERENCED_PARAMETER(pi_pFileName);
  if (pio_pCharacterObject)
  {
    for (DWORD i = 0; i < pio_pCharacterObject->m_MaxObj; ++i)
    {
      ObjectMesh *l_pMesh = GetMeshAt(pio_pCharacterObject, i);
      if (l_pMesh)
      {
        CharacterMR::Field<float>(l_pMesh, CharacterMR::OBJ_EXTRA_SCALE + 0) = 1.0f;
        CharacterMR::Field<float>(l_pMesh, CharacterMR::OBJ_EXTRA_SCALE + 4) = 1.0f;
        CharacterMR::Field<float>(l_pMesh, CharacterMR::OBJ_EXTRA_SCALE + 8) = 1.0f;
      }
    }
  }
}

int ImportBoundBox(char *pi_pFileName, char *po_pMin, char *po_pMax)
{
  if (!pi_pFileName || !po_pMin || !po_pMax)
  {
    return 0;
  }

  FILE *l_pFile = fopen(pi_pFileName, "rb");
  if (!l_pFile)
  {
    return 0;
  }

  char l_szSkip[256];
  fread(l_szSkip, 1, sizeof(l_szSkip), l_pFile);
  fread(po_pMax, 1, sizeof(float) * 3, l_pFile);
  fread(po_pMin, 1, sizeof(float) * 3, l_pFile);
  fclose(l_pFile);
  return 1;
}

int ImportBoundBox(char *pi_pFileName,
                   char *const pi_pPathName,
                   float *const po_pfMin,
                   float *const po_pfMax)
{
  char l_szPath[MAX_PATH] = {};
  if (pi_pPathName && pi_pPathName[0])
  {
    CombinePath(l_szPath, sizeof(l_szPath), pi_pPathName, pi_pFileName);
  }
  else
  {
    CopyStringSafe(l_szPath, sizeof(l_szPath), pi_pFileName);
  }

  return ImportBoundBox(l_szPath,
                        reinterpret_cast<char *>(po_pfMin),
                        reinterpret_cast<char *>(po_pfMax));
}

unsigned int LoadAndCreateShader(int pi_nArg1,
                                 DWORD pi_dwBytes,
                                 int pi_nArg3,
                                 int pi_nArg4,
                                 int pi_nArg5)
{
  UNREFERENCED_PARAMETER(pi_nArg1);
  UNREFERENCED_PARAMETER(pi_dwBytes);
  UNREFERENCED_PARAMETER(pi_nArg3);
  UNREFERENCED_PARAMETER(pi_nArg4);
  UNREFERENCED_PARAMETER(pi_nArg5);
  return 0;
}

FILE *CHGetFileSize(char *pi_pFileName)
{
  return fopen(pi_pFileName, "rb");
}

int CHGetMipMapSkipSize(DWORD *pi_pHeader,
                        unsigned int pi_dwWidth,
                        unsigned int pi_dwHeight,
                        unsigned int pi_dwMipLevel)
{
  UNREFERENCED_PARAMETER(pi_pHeader);
  unsigned int l_dwSize = 0;
  for (unsigned int i = 0; i < pi_dwMipLevel; ++i)
  {
    l_dwSize += (std::max)(1U, pi_dwWidth >> i) * (std::max)(1U, pi_dwHeight >> i) * 4;
  }
  return static_cast<int>(l_dwSize);
}

IDirect3DTexture8 *CreateTexture(IDirect3DDevice8 *pi_pDevice,
                                 unsigned long pi_dwWidth,
                                 unsigned long pi_dwHeight)
{
  if (!pi_pDevice)
  {
    return NULL;
  }

  IDirect3DTexture8 *l_pTexture = NULL;
  if (FAILED(pi_pDevice->CreateTexture(pi_dwWidth,
                                       pi_dwHeight,
                                       1,
                                       0,
                                       D3DFMT_A8R8G8B8,
                                       D3DPOOL_MANAGED,
                                       &l_pTexture)))
  {
    return NULL;
  }
  return l_pTexture;
}

IDirect3DSurface8 *CreateSurface(IDirect3DDevice8 *pi_pDevice,
                                 unsigned long pi_dwWidth,
                                 unsigned long pi_dwHeight)
{
  if (!pi_pDevice)
  {
    return NULL;
  }

  IDirect3DSurface8 *l_pSurface = NULL;
  if (FAILED(pi_pDevice->CreateImageSurface(pi_dwWidth,
                                            pi_dwHeight,
                                            D3DFMT_A8R8G8B8,
                                            &l_pSurface)))
  {
    return NULL;
  }
  return l_pSurface;
}

int CHR3LoadDDSFromFP(IDirect3DDevice8 *pi_pDevice,
                      FILE *pi_pFile,
                      size_t pi_stSize,
                      IDirect3DTexture8 **po_ppTexture,
                      int pi_nArg5,
                      int pi_nArg6,
                      int pi_nArg7)
{
  UNREFERENCED_PARAMETER(pi_nArg5);
  UNREFERENCED_PARAMETER(pi_nArg6);
  UNREFERENCED_PARAMETER(pi_nArg7);
  if (!pi_pDevice || !pi_pFile || !po_ppTexture || pi_stSize == 0)
  {
    return 0;
  }

  *po_ppTexture = NULL;
  std::vector<BYTE> l_vecBuffer(pi_stSize);
  if (fread(&l_vecBuffer[0], 1, pi_stSize, pi_pFile) != pi_stSize)
  {
    return 0;
  }

  DWORD *l_pMagic = reinterpret_cast<DWORD *>(&l_vecBuffer[0]);
  if (*l_pMagic != kDDSMagic && pi_stSize >= 128)
  {
    UnlockDDS(l_pMagic);
  }
  if (*l_pMagic != kDDSMagic)
  {
    return 0;
  }

  FixDDSHeaderBytes(&l_vecBuffer[0], static_cast<DWORD>(l_vecBuffer.size()));
  return SUCCEEDED(D3DXCreateTextureFromFileInMemory(pi_pDevice,
                                                     &l_vecBuffer[0],
                                                     static_cast<UINT>(l_vecBuffer.size()),
                                                     po_ppTexture)) &&
         *po_ppTexture != NULL;
}

IDirect3DTexture8 *CHR3LoadDDS(IDirect3DDevice8 *pi_pDevice,
                               char *pi_pFileName,
                               int pi_nArg3,
                               int pi_nArg4,
                               int pi_nArg5,
                               int pi_nArg6)
{
  UNREFERENCED_PARAMETER(pi_nArg3);
  UNREFERENCED_PARAMETER(pi_nArg4);
  UNREFERENCED_PARAMETER(pi_nArg5);
  UNREFERENCED_PARAMETER(pi_nArg6);
  if (!pi_pDevice || !pi_pFileName)
  {
    return NULL;
  }

  FILE *l_pFile = fopen(pi_pFileName, "rb");
  if (!l_pFile)
  {
    return NULL;
  }
  fseek(l_pFile, 0, SEEK_END);
  const long l_nSize = ftell(l_pFile);
  fseek(l_pFile, 0, SEEK_SET);

  IDirect3DTexture8 *l_pTexture = NULL;
  if (l_nSize > 0)
  {
    CHR3LoadDDSFromFP(pi_pDevice,
                      l_pFile,
                      static_cast<size_t>(l_nSize),
                      &l_pTexture,
                      0,
                      0,
                      0);
  }
  fclose(l_pFile);
  return l_pTexture;
}

IDirect3DTexture8 *CHR3LoadDDS(IDirect3DDevice8 *pi_pDevice,
                               char *pi_pFileName,
                               unsigned long pi_dwSize,
                               bool pi_bMipMap,
                               unsigned long pi_dwArg5,
                               unsigned long pi_dwArg6,
                               unsigned long pi_dwArg7)
{
  UNREFERENCED_PARAMETER(pi_dwSize);
  return CHR3LoadDDS(pi_pDevice,
                     pi_pFileName,
                     pi_bMipMap ? 1 : 0,
                     static_cast<int>(pi_dwArg5),
                     static_cast<int>(pi_dwArg6),
                     static_cast<int>(pi_dwArg7));
}

DWORD *FixDDSHeader(DWORD *pio_pHeader)
{
  FixDDSHeaderBytes(reinterpret_cast<BYTE *>(pio_pHeader), 128);
  return pio_pHeader;
}

void Draw2DSprite(IDirect3DDevice8 *pi_pDevice,
                  IDirect3DTexture8 *pi_pTexture,
                  float pi_fX,
                  float pi_fY,
                  float pi_fWidth,
                  float pi_fHeight,
                  DWORD pi_dwColor)
{
  UNREFERENCED_PARAMETER(pi_pDevice);
  UNREFERENCED_PARAMETER(pi_pTexture);
  UNREFERENCED_PARAMETER(pi_fX);
  UNREFERENCED_PARAMETER(pi_fY);
  UNREFERENCED_PARAMETER(pi_fWidth);
  UNREFERENCED_PARAMETER(pi_fHeight);
  UNREFERENCED_PARAMETER(pi_dwColor);
}

void Draw2DSprite(IDirect3DDevice8 *pi_pDevice,
                  float pi_fX,
                  float pi_fY,
                  float pi_fWidth,
                  float pi_fHeight,
                  void *pi_pTexture,
                  unsigned long pi_dwColor)
{
  Draw2DSprite(pi_pDevice,
               static_cast<IDirect3DTexture8 *>(pi_pTexture),
               pi_fX,
               pi_fY,
               pi_fWidth,
               pi_fHeight,
               pi_dwColor);
}

void Draw2DSprite(IDirect3DDevice8 *pi_pDevice,
                  float pi_fX,
                  float pi_fY,
                  float pi_fWidth,
                  float pi_fHeight,
                  float *const pi_pfUV0,
                  float *const pi_pfUV1,
                  void *pi_pTexture,
                  unsigned long pi_dwColor)
{
  UNREFERENCED_PARAMETER(pi_pfUV0);
  UNREFERENCED_PARAMETER(pi_pfUV1);
  Draw2DSprite(pi_pDevice,
               static_cast<IDirect3DTexture8 *>(pi_pTexture),
               pi_fX,
               pi_fY,
               pi_fWidth,
               pi_fHeight,
               pi_dwColor);
}

void Draw2DSprite(IDirect3DDevice8 *pi_pDevice,
                  IDirect3DTexture8 *pi_pTexture,
                  RECT *pi_pSrcRect,
                  float pi_fX,
                  float pi_fY,
                  DWORD pi_dwColor)
{
  UNREFERENCED_PARAMETER(pi_pSrcRect);
  Draw2DSprite(pi_pDevice, pi_pTexture, pi_fX, pi_fY, 0.0f, 0.0f, pi_dwColor);
}

void _R3CalculateTime(void)
{
}

void InitMaterial(D3DMATERIAL8 *po_pMaterial,
                  float pi_fRed,
                  float pi_fGreen,
                  float pi_fBlue,
                  float pi_fAlpha)
{
  CharacterMR::InitMaterial(po_pMaterial, pi_fRed, pi_fGreen, pi_fBlue, pi_fAlpha);
}

void InitMaterial(D3DMATERIAL8 &po_rMaterial,
                  float pi_fRed,
                  float pi_fGreen,
                  float pi_fBlue,
                  float pi_fAlpha)
{
  InitMaterial(&po_rMaterial, pi_fRed, pi_fGreen, pi_fBlue, pi_fAlpha);
}

void SetFustumNormalPlane(float (*const pio_pPlane)[4], D3DXMATRIX &pio_rMatrix)
{
  SetFustumNormalPlane(const_cast<float (*)[4]>(pio_pPlane), &pio_rMatrix);
}

void GetNormal(D3DXVECTOR3 *po_pNormal,
               D3DXVECTOR3 *pi_p0,
               D3DXVECTOR3 *pi_p1,
               D3DXVECTOR3 *pi_p2)
{
  if (!po_pNormal || !pi_p0 || !pi_p1 || !pi_p2)
  {
    return;
  }

  D3DXVECTOR3 l_vecA = *pi_p1 - *pi_p0;
  D3DXVECTOR3 l_vecB = *pi_p2 - *pi_p0;
  D3DXVec3Cross(po_pNormal, &l_vecA, &l_vecB);
  D3DXVec3Normalize(po_pNormal, po_pNormal);
}

bool FindEdgeAddNormal(EDGE *pio_pEdges,
                       int pi_nEdgeCount,
                       EDGE *pi_pEdge,
                       D3DXVECTOR3 *pi_pNormal)
{
  if (!pio_pEdges || !pi_pEdge || !pi_pNormal)
  {
    return false;
  }

  for (int i = 0; i < pi_nEdgeCount; ++i)
  {
    if ((pio_pEdges[i].m_wIndex[0] == pi_pEdge->m_wIndex[1] &&
         pio_pEdges[i].m_wIndex[1] == pi_pEdge->m_wIndex[0]) ||
        (pio_pEdges[i].m_wIndex[0] == pi_pEdge->m_wIndex[0] &&
         pio_pEdges[i].m_wIndex[1] == pi_pEdge->m_wIndex[1]))
    {
      pio_pEdges[i].m_vecNormal[1] = *pi_pNormal;
      return true;
    }
  }
  return false;
}

bool FindEdgeAddNormal(EDGE *pio_pEdges,
                       int pi_nEdgeCount,
                       EDGE *pi_pEdge,
                       D3DXVECTOR3 &pio_rNormal)
{
  return FindEdgeAddNormal(pio_pEdges, pi_nEdgeCount, pi_pEdge, &pio_rNormal);
}

void AddEdge(EDGE *pio_pEdges, EDGE *pi_pEdge, int *pio_pEdgeCount, D3DXVECTOR3 *pi_pNormal)
{
  if (!pio_pEdges || !pi_pEdge || !pio_pEdgeCount || !pi_pNormal)
  {
    return;
  }

  if (FindEdgeAddNormal(pio_pEdges, *pio_pEdgeCount, pi_pEdge, pi_pNormal))
  {
    return;
  }

  pio_pEdges[*pio_pEdgeCount] = *pi_pEdge;
  pio_pEdges[*pio_pEdgeCount].m_vecNormal[0] = *pi_pNormal;
  pio_pEdges[*pio_pEdgeCount].m_vecNormal[1] = *pi_pNormal;
  ++(*pio_pEdgeCount);
}

void AddEdge(EDGE *pio_pEdges, EDGE *pi_pEdge, int &pio_rEdgeCount, D3DXVECTOR3 &pio_rNormal)
{
  AddEdge(pio_pEdges, pi_pEdge, &pio_rEdgeCount, &pio_rNormal);
}

void MeshAlphaBlend(unsigned int pi_dwFrame, ObjectMesh *pio_pObjectMesh)
{
  UNREFERENCED_PARAMETER(pi_dwFrame);
  if (pio_pObjectMesh)
  {
    CharacterMR::Field<float>(pio_pObjectMesh, CharacterMR::OBJ_ALPHA) =
      (std::max)(0.0f, (std::min)(1.0f, CharacterMR::Field<float>(pio_pObjectMesh, CharacterMR::OBJ_ALPHA)));
  }
}

void MeshAlphaBlend(int pi_nFrame, ObjectMesh *pio_pObjectMesh)
{
  MeshAlphaBlend(static_cast<unsigned int>(pi_nFrame), pio_pObjectMesh);
}

void MeshRotBlend(D3DXMATRIX *pio_pMatrix, int pi_nFrame, ObjectMesh *pi_pObjectMesh)
{
  UNREFERENCED_PARAMETER(pi_nFrame);
  UNREFERENCED_PARAMETER(pi_pObjectMesh);
  if (pio_pMatrix)
  {
    D3DXMatrixIdentity(pio_pMatrix);
  }
}

void MeshRotBlend(D3DXMATRIX &pio_rMatrix, int pi_nFrame, ObjectMesh *pi_pObjectMesh)
{
  MeshRotBlend(&pio_rMatrix, pi_nFrame, pi_pObjectMesh);
}

void MeshScaleBlend(D3DXMATRIX *pio_pMatrix, int pi_nFrame, ObjectMesh *pi_pObjectMesh)
{
  UNREFERENCED_PARAMETER(pi_nFrame);
  if (pio_pMatrix)
  {
    const float sx = pi_pObjectMesh ? CharacterMR::Field<float>(pi_pObjectMesh, CharacterMR::OBJ_EXTRA_SCALE + 0) : 1.0f;
    const float sy = pi_pObjectMesh ? CharacterMR::Field<float>(pi_pObjectMesh, CharacterMR::OBJ_EXTRA_SCALE + 4) : 1.0f;
    const float sz = pi_pObjectMesh ? CharacterMR::Field<float>(pi_pObjectMesh, CharacterMR::OBJ_EXTRA_SCALE + 8) : 1.0f;
    D3DXMatrixScaling(pio_pMatrix, sx, sy, sz);
  }
}

void MeshScaleBlend(D3DXMATRIX &pio_rMatrix, int pi_nFrame, ObjectMesh *pi_pObjectMesh)
{
  MeshScaleBlend(&pio_rMatrix, pi_nFrame, pi_pObjectMesh);
}

void MeshTransBlend(D3DXMATRIX *pio_pMatrix, int pi_nFrame, ObjectMesh *pi_pObjectMesh)
{
  UNREFERENCED_PARAMETER(pi_nFrame);
  UNREFERENCED_PARAMETER(pi_pObjectMesh);
  if (pio_pMatrix)
  {
    D3DXMatrixIdentity(pio_pMatrix);
  }
}

void MeshTransBlend(D3DXMATRIX &pio_rMatrix, int pi_nFrame, ObjectMesh *pi_pObjectMesh)
{
  MeshTransBlend(&pio_rMatrix, pi_nFrame, pi_pObjectMesh);
}

int SetTextureEffect(int pi_nArg1, int pi_nArg2, float pi_fArg3)
{
  UNREFERENCED_PARAMETER(pi_nArg1);
  UNREFERENCED_PARAMETER(pi_nArg2);
  UNREFERENCED_PARAMETER(pi_fArg3);
  return 1;
}

void SetTextureEffect(unsigned long pi_dwStage,
                      EFFECTWAVETYPE pi_eWaveType,
                      float pi_fFrame,
                      float pi_fSpeed)
{
  UNREFERENCED_PARAMETER(pi_fSpeed);
  SetTextureEffect(static_cast<int>(pi_dwStage), static_cast<int>(pi_eWaveType), pi_fFrame);
}

void DrawP(IDirect3DDevice8 *pi_pDevice, int pi_nArg2)
{
  UNREFERENCED_PARAMETER(pi_pDevice);
  UNREFERENCED_PARAMETER(pi_nArg2);
}

void DrawP(IDirect3DDevice8 *pi_pDevice, int pi_nArg2, int pi_nArg3)
{
  UNREFERENCED_PARAMETER(pi_nArg3);
  DrawP(pi_pDevice, pi_nArg2);
}

void AlphaMeshManager::AddAlphaMesh(AlphaMesh &pio_rAlphaMesh)
{
  if (m_nAlphaMeshCount >= 1024)
  {
    return;
  }

  AlphaMesh *l_pEntry = &m_aAlphaMesh[m_nAlphaMeshCount];
  memcpy(l_pEntry, &pio_rAlphaMesh, sizeof(AlphaMesh));
  l_pEntry->m_fViewZ = GetViewZ(l_pEntry);
  l_pEntry->m_pNext = NULL;

  if (!m_pHead)
  {
    m_pHead = l_pEntry;
    ++m_nAlphaMeshCount;
    return;
  }

  AlphaMesh *l_pPrev = NULL;
  AlphaMesh *l_pCurrent = m_pHead;
  int l_nGuard = 0;
  while (l_pCurrent && l_pCurrent->m_fViewZ > l_pEntry->m_fViewZ)
  {
    if (!l_pCurrent->m_pNext)
    {
      l_pCurrent->m_pNext = l_pEntry;
      ++m_nAlphaMeshCount;
      return;
    }
    l_pPrev = l_pCurrent;
    l_pCurrent = l_pCurrent->m_pNext;
    if (++l_nGuard >= 1024)
    {
      ++m_nAlphaMeshCount;
      return;
    }
  }

  l_pEntry->m_pNext = l_pCurrent;
  if (l_pCurrent == m_pHead)
  {
    m_pHead = l_pEntry;
  }
  else if (l_pPrev)
  {
    l_pPrev->m_pNext = l_pEntry;
  }
  ++m_nAlphaMeshCount;
}

void AlphaMeshManager::AddAlphaMesh(ObjectMesh *pi_pObjectMesh,
                                    CHARACTEROBJECT *pi_pCharacterObject,
                                    D3DXMATRIX *pi_pWorldMatrix,
                                    float pi_fScroll)
{
  UNREFERENCED_PARAMETER(pi_pObjectMesh);
  UNREFERENCED_PARAMETER(pi_pCharacterObject);
  UNREFERENCED_PARAMETER(pi_pWorldMatrix);
  UNREFERENCED_PARAMETER(pi_fScroll);
}

float AlphaMeshManager::GetViewZ(AlphaMesh *pi_pAlphaMesh)
{
  if (!pi_pAlphaMesh)
  {
    return 0.0f;
  }

  float l_afMatrix[16] = {};
  MultMatrixAsm(l_afMatrix, pi_pAlphaMesh->m_afMatrix, m_afViewMatrix);
  return l_afMatrix[14];
}

float AlphaMeshManager::GetViewZ(D3DXMATRIX *pi_pWorldMatrix)
{
  return pi_pWorldMatrix ? pi_pWorldMatrix->_43 : 0.0f;
}
