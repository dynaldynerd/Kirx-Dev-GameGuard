#include "CharacterMRInternal.h"

#include "../AlphaMeshManager.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <direct.h>
#include <map>
#include <string>
#include <vector>

ObjectData *ImportMeshData(char *pi_pFileName);
ObjectData *MakeMeshData(char *pi_pBuffer);
unsigned char *ReadAnimationData(char *pi_pFileName);
void InitVertex(ObjectData *pi_pObjectData,
                CHARACTEROBJECT *pi_pParentBone,
                CHARACTEROBJECT *pio_pCharacterObject,
                TextureManager *pi_pTextureManager);
void InitVertex(ObjectData *pi_pObjectData,
                CHARACTEROBJECT *pi_pParentBone,
                CHARACTEROBJECT *pio_pCharacterObject,
                TextureManager *pi_pTextureManager,
                bool pi_bUseImmediateFile);

namespace
{
struct PtrVector
{
  void **m_pBegin;
  void **m_pEnd;
  void **m_pCapacity;
};

struct ChTextureCompat
{
  char m_szName[256];
  DWORD m_dwID;
  IDirect3DTexture8 *m_pTexture;
  DWORD m_dwBytes;
};

struct MeshRecord
{
  const BYTE *m_pHeader;
  const BYTE *m_pVertices;
  const BYTE *m_pFaces;
  const BYTE *m_pPhysiqueNames;
  const BYTE *m_pPhysiqueRecords;
  DWORD m_dwVertexCount;
  DWORD m_dwFaceCount;
  DWORD m_dwPhysiqueNameCount;
  DWORD m_dwPhysiqueCount;
  bool m_bExpandedPhysique;
};

struct AnimationDataCompat
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

const size_t kVectorBeginOffsetObjectManager = 0x04;
const size_t kVectorEndOffsetObjectManager = 0x08;
const size_t kVectorCapacityOffsetObjectManager = 0x0C;
const size_t kObjectManagerNextIDOffset = 0x10;

const size_t kVectorBeginOffsetAnimationManager = 0x04;
const size_t kVectorEndOffsetAnimationManager = 0x08;
const size_t kVectorCapacityOffsetAnimationManager = 0x0C;
const size_t kAnimationManagerNextIDOffset = 0x14;

const size_t kVectorBeginOffsetTextureManager = 0x08;
const size_t kVectorEndOffsetTextureManager = 0x0C;
const size_t kVectorCapacityOffsetTextureManager = 0x10;
const size_t kTextureManagerMemoryOffset = 0x14;
const size_t kTextureManagerNextIDOffset = 0x18;
const size_t kTextureManagerPathOffset = 0x1C;

const size_t kChInterfaceMeshManagerOffset = 0x238;
const size_t kChInterfaceDefaultObjectManagerOffset = 0x70;
const size_t kChInterfaceBoneObjectManagerOffset = 0x104;

const size_t kObjectDataMeshCountOffset = 0x02;
const size_t kObjectDataMeshDataOffset = 0x04;
const size_t kObjectDataReserved0Offset = 0x0C;
const size_t kObjectDataReserved1Offset = 0x14;
const size_t kObjectDataExpandedAuxOffset = 0x18;

const size_t kMeshDataHeaderSize = 0x2B5;
const size_t kMeshDataVertexOffset = 0x2B8;
const size_t kMeshDataFaceOffset = 0x2BC;
const size_t kMeshDataPhysiqueOffset = 0x2C0;
const size_t kMeshDataPhysiqueNameCountOffset = 0x2C4;
const size_t kMeshDataPhysiqueNamesOffset = 0x2C8;
const size_t kMeshDataPackedPhysiqueOffset = 0x2CC;

const size_t kMeshVertexBytes = 28;
const size_t kMeshFaceBytes = 88;
const size_t kMeshPhysiqueBytes = 424;
const size_t kMeshPackedPhysiqueBytes = 40;
const size_t kMeshPhysiqueNameBytes = 100;

const DWORD kDDSMagic = 0x20534444;

float Clamp01(float pi_fValue)
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

void UppercaseInPlace(char *pio_szValue)
{
  if (!pio_szValue)
  {
    return;
  }

  for (char *p = pio_szValue; *p; ++p)
  {
    if (*p >= 'a' && *p <= 'z')
    {
      *p = static_cast<char>(*p - 'a' + 'A');
    }
    else if (*p == '/')
    {
      *p = '\\';
    }
  }
}

void LowercaseInPlace(char *pio_szValue)
{
  if (!pio_szValue)
  {
    return;
  }

  for (char *p = pio_szValue; *p; ++p)
  {
    if (*p >= 'A' && *p <= 'Z')
    {
      *p = static_cast<char>(*p - 'A' + 'a');
    }
    else if (*p == '/')
    {
      *p = '\\';
    }
  }
}

void CopyStringSafe(char *po_szDst, size_t pi_stDstSize, const char *pi_szSrc)
{
  if (!po_szDst || pi_stDstSize == 0)
  {
    return;
  }

  po_szDst[0] = '\0';
  if (!pi_szSrc)
  {
    return;
  }

  strncpy_s(po_szDst, pi_stDstSize, pi_szSrc, _TRUNCATE);
}

void CopyFixedString(char *po_szDst, size_t pi_stDstSize, const char *pi_pSrc, size_t pi_stSrcSize)
{
  if (!po_szDst || pi_stDstSize == 0)
  {
    return;
  }

  po_szDst[0] = '\0';
  if (!pi_pSrc || pi_stSrcSize == 0)
  {
    return;
  }

  size_t l_stLength = 0;
  while (l_stLength < pi_stSrcSize && pi_pSrc[l_stLength])
  {
    ++l_stLength;
  }
  if (l_stLength >= pi_stDstSize)
  {
    l_stLength = pi_stDstSize - 1;
  }
  memcpy(po_szDst, pi_pSrc, l_stLength);
  po_szDst[l_stLength] = '\0';
}

void CopyDirectoryPath(const char *pi_pPath, char *po_szDirectory, size_t pi_stDirectorySize)
{
  if (!po_szDirectory || pi_stDirectorySize == 0)
  {
    return;
  }

  po_szDirectory[0] = '\0';
  if (!pi_pPath)
  {
    return;
  }

  const char *l_pSlash = strrchr(pi_pPath, '\\');
  const char *l_pSlash2 = strrchr(pi_pPath, '/');
  if (l_pSlash2 && (!l_pSlash || l_pSlash2 > l_pSlash))
  {
    l_pSlash = l_pSlash2;
  }

  if (!l_pSlash)
  {
    return;
  }

  size_t l_stLength = static_cast<size_t>(l_pSlash - pi_pPath) + 1;
  if (l_stLength >= pi_stDirectorySize)
  {
    l_stLength = pi_stDirectorySize - 1;
  }
  memcpy(po_szDirectory, pi_pPath, l_stLength);
  po_szDirectory[l_stLength] = '\0';
}

void CombinePath(char *po_szOut,
                 size_t pi_stOutSize,
                 const char *pi_pPath,
                 const char *pi_pFileName)
{
  if (!po_szOut || pi_stOutSize == 0)
  {
    return;
  }

  po_szOut[0] = '\0';
  if (!pi_pFileName || !pi_pFileName[0])
  {
    return;
  }

  if (!pi_pPath || !pi_pPath[0])
  {
    CopyStringSafe(po_szOut, pi_stOutSize, pi_pFileName);
    return;
  }

  const size_t l_stPathLen = strlen(pi_pPath);
  const bool l_bHasSlash = l_stPathLen > 0 &&
                           (pi_pPath[l_stPathLen - 1] == '\\' ||
                            pi_pPath[l_stPathLen - 1] == '/');
  _snprintf_s(po_szOut,
              pi_stOutSize,
              _TRUNCATE,
              l_bHasSlash ? "%s%s" : "%s\\%s",
              pi_pPath,
              pi_pFileName);
}

void ChangeExtension(char *pio_szPath, size_t pi_stPathSize, const char *pi_pExt)
{
  if (!pio_szPath || !pi_pExt)
  {
    return;
  }

  char *l_pDot = strrchr(pio_szPath, '.');
  if (!l_pDot)
  {
    strncat_s(pio_szPath, pi_stPathSize, pi_pExt, _TRUNCATE);
    return;
  }

  *l_pDot = '\0';
  strncat_s(pio_szPath, pi_stPathSize, pi_pExt, _TRUNCATE);
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
  const size_t l_stCapacity = (l_stVector.m_pBegin && l_stVector.m_pCapacity)
                                ? static_cast<size_t>(l_stVector.m_pCapacity - l_stVector.m_pBegin)
                                : 0;

  if (l_stCount >= l_stCapacity)
  {
    size_t l_stNewCapacity = l_stCapacity ? l_stCapacity * 2 : 1;
    if (l_stNewCapacity < l_stCount + 1)
    {
      l_stNewCapacity = l_stCount + 1;
    }

    void **l_ppNewData =
      static_cast<void **>(::operator new(sizeof(void *) * l_stNewCapacity));
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

void ClearPointerVectorStorage(void *pi_pOwner,
                               size_t pi_stBeginOffset,
                               size_t pi_stEndOffset,
                               size_t pi_stCapacityOffset)
{
  PtrVector l_stVector = GetVector(pi_pOwner,
                                   pi_stBeginOffset,
                                   pi_stEndOffset,
                                   pi_stCapacityOffset);
  ::operator delete(l_stVector.m_pBegin);
  l_stVector.m_pBegin = NULL;
  l_stVector.m_pEnd = NULL;
  l_stVector.m_pCapacity = NULL;
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

IDirect3DDevice8 *GetTextureDevice(TextureManager *pi_pTextureManager)
{
  return pi_pTextureManager ? CharacterMR::Field<IDirect3DDevice8 *>(pi_pTextureManager, 0) : NULL;
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

void FixDDSHeader(BYTE *pio_pBuffer, DWORD pi_dwBufferSize)
{
  if (!pio_pBuffer || pi_dwBufferSize < 128)
  {
    return;
  }

  DWORD *l_pHeader = reinterpret_cast<DWORD *>(pio_pBuffer);
  DWORD &l_rPixelFormatSize = l_pHeader[19];
  DWORD &l_rPixelFormatFlags = l_pHeader[20];
  DWORD &l_rRGBBitCount = l_pHeader[22];
  DWORD &l_rRMask = l_pHeader[23];
  DWORD &l_rGMask = l_pHeader[24];
  DWORD &l_rBMask = l_pHeader[25];
  DWORD &l_rAMask = l_pHeader[26];

  if (l_rPixelFormatFlags == 5)
  {
    l_rPixelFormatFlags = 4;
  }
  if (l_rPixelFormatFlags == 0x41)
  {
    if (l_rRGBBitCount == 16)
    {
      if (l_rAMask != 0x00008000)
      {
        l_rRMask = 0x00000F00;
      }
    }
    else if (l_rRGBBitCount == 32)
    {
      l_rRMask = 0x00FF0000;
      l_rGMask = 0x0000FF00;
      l_rBMask = 0x000000FF;
      l_rAMask = 0xFF000000;
    }
  }
  if (l_rPixelFormatSize == 0)
  {
    l_rPixelFormatSize = 32;
    l_rPixelFormatFlags = 4;
  }
}

bool LoadTextureFile(IDirect3DDevice8 *pi_pDevice,
                     const char *pi_pFileName,
                     IDirect3DTexture8 **po_ppTexture)
{
  if (!pi_pDevice || !pi_pFileName || !po_ppTexture)
  {
    return false;
  }

  *po_ppTexture = NULL;
  if (SUCCEEDED(D3DXCreateTextureFromFileA(pi_pDevice, pi_pFileName, po_ppTexture)) &&
      *po_ppTexture)
  {
    return true;
  }

  std::vector<BYTE> l_vecBuffer;
  if (!CharacterMR::LoadBinaryFile(pi_pFileName, l_vecBuffer) ||
      l_vecBuffer.size() < 128)
  {
    return false;
  }

  DWORD *l_pMagic = reinterpret_cast<DWORD *>(&l_vecBuffer[0]);
  if (*l_pMagic != kDDSMagic)
  {
    UnlockDDS(l_pMagic);
  }
  if (*l_pMagic != kDDSMagic)
  {
    return false;
  }

  FixDDSHeader(&l_vecBuffer[0], static_cast<DWORD>(l_vecBuffer.size()));
  return SUCCEEDED(D3DXCreateTextureFromFileInMemory(pi_pDevice,
                                                     &l_vecBuffer[0],
                                                     static_cast<UINT>(l_vecBuffer.size()),
                                                     po_ppTexture)) &&
         *po_ppTexture != NULL;
}

DWORD EstimateTextureBytes(IDirect3DTexture8 *pi_pTexture)
{
  if (!pi_pTexture)
  {
    return 0;
  }

  D3DSURFACE_DESC l_stDesc;
  ZeroMemory(&l_stDesc, sizeof(l_stDesc));
  if (FAILED(pi_pTexture->GetLevelDesc(0, &l_stDesc)))
  {
    return 0;
  }
  return l_stDesc.Width * l_stDesc.Height * 4;
}

ChTextureCompat *GetTextureRecord(TextureManager *pi_pTextureManager, const char *pi_pTextureName)
{
  if (!pi_pTextureManager || !pi_pTextureName || !pi_pTextureName[0])
  {
    return NULL;
  }

  char l_szName[256];
  CopyStringSafe(l_szName, sizeof(l_szName), pi_pTextureName);
  UppercaseInPlace(l_szName);

  char l_szRftName[256];
  CopyStringSafe(l_szRftName, sizeof(l_szRftName), l_szName);
  ChangeExtension(l_szRftName, sizeof(l_szRftName), ".RFT");

  PtrVector l_stVector = GetVector(pi_pTextureManager,
                                   kVectorBeginOffsetTextureManager,
                                   kVectorEndOffsetTextureManager,
                                   kVectorCapacityOffsetTextureManager);
  for (void **it = l_stVector.m_pBegin; it && it != l_stVector.m_pEnd; ++it)
  {
    ChTextureCompat *l_pTexture = static_cast<ChTextureCompat *>(*it);
    if (l_pTexture &&
        (!strcmp(l_pTexture->m_szName, l_szName) ||
         !strcmp(l_pTexture->m_szName, l_szRftName)))
    {
      return l_pTexture;
    }
  }

  return NULL;
}

ChTextureCompat *GetTextureRecord(TextureManager *pi_pTextureManager, DWORD pi_dwTextureID)
{
  if (!pi_pTextureManager || pi_dwTextureID == 0)
  {
    return NULL;
  }

  PtrVector l_stVector = GetVector(pi_pTextureManager,
                                   kVectorBeginOffsetTextureManager,
                                   kVectorEndOffsetTextureManager,
                                   kVectorCapacityOffsetTextureManager);
  for (void **it = l_stVector.m_pBegin; it && it != l_stVector.m_pEnd; ++it)
  {
    ChTextureCompat *l_pTexture = static_cast<ChTextureCompat *>(*it);
    if (l_pTexture && l_pTexture->m_dwID == pi_dwTextureID)
    {
      return l_pTexture;
    }
  }

  return NULL;
}

void *CopyMeshDataBlock(const BYTE *pi_pSource, size_t pi_stSize)
{
  if (!pi_pSource || pi_stSize == 0)
  {
    return NULL;
  }

  void *l_pCopy = ::operator new(pi_stSize);
  memcpy(l_pCopy, pi_pSource, pi_stSize);
  return l_pCopy;
}

BYTE *BuildLegacyPhysiqueRecords(const BYTE *pi_pNames,
                                 DWORD pi_dwNameCount,
                                 const BYTE *pi_pPackedRecords,
                                 WORD pi_wPhysiqueCount)
{
  if (!pi_pPackedRecords || pi_wPhysiqueCount == 0)
  {
    return NULL;
  }

  BYTE *l_pLegacyRecords =
    static_cast<BYTE *>(::operator new(kMeshPhysiqueBytes * pi_wPhysiqueCount));
  ZeroMemory(l_pLegacyRecords, kMeshPhysiqueBytes * pi_wPhysiqueCount);

  for (WORD i = 0; i < pi_wPhysiqueCount; ++i)
  {
    const BYTE *l_pPackedRecord = pi_pPackedRecords + (i * kMeshPackedPhysiqueBytes);
    BYTE *l_pLegacyRecord = l_pLegacyRecords + (i * kMeshPhysiqueBytes);

    const DWORD l_dwVertexIndex = *reinterpret_cast<const DWORD *>(l_pPackedRecord + 0);
    const DWORD l_dwInfluenceCount = *reinterpret_cast<const DWORD *>(l_pPackedRecord + 4);
    *reinterpret_cast<DWORD *>(l_pLegacyRecord + 0) = l_dwVertexIndex;
    *reinterpret_cast<DWORD *>(l_pLegacyRecord + 4) = l_dwInfluenceCount;

    for (DWORD j = 0; j < l_dwInfluenceCount && j < 4; ++j)
    {
      const DWORD l_dwNameIndex =
        *reinterpret_cast<const DWORD *>(l_pPackedRecord + 8 + (j * 4));
      if (l_dwNameIndex != 0xFFFFFFFF &&
          l_dwNameIndex < pi_dwNameCount &&
          pi_pNames)
      {
        CopyFixedString(reinterpret_cast<char *>(l_pLegacyRecord + 8 + (j * 100)),
                        100,
                        reinterpret_cast<const char *>(pi_pNames +
                                                       (l_dwNameIndex * kMeshPhysiqueNameBytes)),
                        kMeshPhysiqueNameBytes);
      }

      *reinterpret_cast<float *>(l_pLegacyRecord + 408 + (j * 4)) =
        *reinterpret_cast<const float *>(l_pPackedRecord + 24 + (j * 4));
    }
  }

  return l_pLegacyRecords;
}

bool CollectMeshDataRecords(ObjectData *pi_pObjectData,
                            std::vector<MeshRecord> &po_vecRecords)
{
  po_vecRecords.clear();
  if (!pi_pObjectData)
  {
    return false;
  }

  const WORD l_wObjectCount =
    CharacterMR::Field<WORD>(pi_pObjectData, kObjectDataMeshCountOffset);
  MeshData *l_pMeshData =
    CharacterMR::Field<MeshData *>(pi_pObjectData, kObjectDataMeshDataOffset);
  if (l_wObjectCount == 0 || l_wObjectCount > 4096 || !l_pMeshData)
  {
    return false;
  }

  po_vecRecords.reserve(l_wObjectCount);
  for (WORD i = 0; i < l_wObjectCount; ++i)
  {
    MeshRecord l_stRecord;
    ZeroMemory(&l_stRecord, sizeof(l_stRecord));
    MeshData *l_pMesh = &l_pMeshData[i];
    l_stRecord.m_pHeader = reinterpret_cast<const BYTE *>(l_pMesh);

    l_stRecord.m_dwVertexCount =
      *reinterpret_cast<const WORD *>(l_stRecord.m_pHeader + 392);
    l_stRecord.m_dwFaceCount =
      *reinterpret_cast<const WORD *>(l_stRecord.m_pHeader + 394);
    const WORD l_wAuxCount =
      *reinterpret_cast<const WORD *>(l_stRecord.m_pHeader + 396);
    l_stRecord.m_dwPhysiqueCount = l_wAuxCount;

    l_stRecord.m_pVertices =
      CharacterMR::Field<BYTE *>(l_pMesh, kMeshDataVertexOffset);
    l_stRecord.m_pFaces =
      CharacterMR::Field<BYTE *>(l_pMesh, kMeshDataFaceOffset);
    l_stRecord.m_pPhysiqueRecords =
      CharacterMR::Field<BYTE *>(l_pMesh, kMeshDataPhysiqueOffset);

    po_vecRecords.push_back(l_stRecord);
  }

  return true;
}

void ReleaseObjectData(ObjectData *pio_pObjectData)
{
  if (!pio_pObjectData)
  {
    return;
  }

  MeshData *l_pMeshData =
    CharacterMR::Field<MeshData *>(pio_pObjectData, kObjectDataMeshDataOffset);
  delete[] l_pMeshData;
  CharacterMR::Field<MeshData *>(pio_pObjectData, kObjectDataMeshDataOffset) = NULL;

  ::operator delete(CharacterMR::Field<void *>(pio_pObjectData,
                                              kObjectDataReserved0Offset));
  ::operator delete(CharacterMR::Field<void *>(pio_pObjectData,
                                              kObjectDataReserved1Offset));
  CharacterMR::Field<void *>(pio_pObjectData, kObjectDataReserved0Offset) = NULL;
  CharacterMR::Field<void *>(pio_pObjectData, kObjectDataReserved1Offset) = NULL;
  ::operator delete(pio_pObjectData);
}

void ConvertVertex(CharacterVertex &po_rVertex, const BYTE *pi_pVertex)
{
  po_rVertex.x = *reinterpret_cast<const float *>(pi_pVertex + 0);
  po_rVertex.y = *reinterpret_cast<const float *>(pi_pVertex + 4);
  po_rVertex.z = *reinterpret_cast<const float *>(pi_pVertex + 8);
  po_rVertex.blend = 1.0f;
  po_rVertex.nx = *reinterpret_cast<const float *>(pi_pVertex + 16);
  po_rVertex.ny = *reinterpret_cast<const float *>(pi_pVertex + 20);
  po_rVertex.nz = *reinterpret_cast<const float *>(pi_pVertex + 24);
  po_rVertex.u = 0.0f;
  po_rVertex.v = 0.0f;
}

void SetUV(CharacterVertex &pio_rVertex, float pi_fU, float pi_fV)
{
  pio_rVertex.u = pi_fU;
  pio_rVertex.v = 1.0f - pi_fV;
}

const char *GetExpandedPhysiqueName(const MeshRecord &pi_rRecord, DWORD pi_dwNameIndex)
{
  if (!pi_rRecord.m_pPhysiqueNames ||
      pi_dwNameIndex >= pi_rRecord.m_dwPhysiqueNameCount)
  {
    return NULL;
  }
  return reinterpret_cast<const char *>(pi_rRecord.m_pPhysiqueNames + (pi_dwNameIndex * 100));
}

void AddUniquePhysiqueName(std::vector<std::string> &pio_vecNames,
                           const char *pi_pName)
{
  if (!pi_pName || !pi_pName[0])
  {
    return;
  }

  for (size_t i = 0; i < pio_vecNames.size(); ++i)
  {
    if (_stricmp(pio_vecNames[i].c_str(), pi_pName) == 0)
    {
      return;
    }
  }
  pio_vecNames.push_back(pi_pName);
}

void CollectPhysiqueNames(const MeshRecord &pi_rRecord,
                          std::vector<std::string> &po_vecNames)
{
  po_vecNames.clear();
  if (!pi_rRecord.m_pPhysiqueRecords || !pi_rRecord.m_dwPhysiqueCount)
  {
    return;
  }

  if (pi_rRecord.m_bExpandedPhysique)
  {
    for (DWORD i = 0; i < pi_rRecord.m_dwPhysiqueNameCount; ++i)
    {
      AddUniquePhysiqueName(po_vecNames, GetExpandedPhysiqueName(pi_rRecord, i));
    }
    return;
  }

  for (DWORD i = 0; i < pi_rRecord.m_dwPhysiqueCount; ++i)
  {
    const BYTE *l_pRecord = pi_rRecord.m_pPhysiqueRecords + (i * 424);
    const DWORD l_dwInfluenceCount = *reinterpret_cast<const DWORD *>(l_pRecord + 4);
    for (DWORD j = 0; j < l_dwInfluenceCount && j < 4; ++j)
    {
      AddUniquePhysiqueName(po_vecNames, reinterpret_cast<const char *>(l_pRecord + 8 + (j * 100)));
    }
  }
}

void SetVertexPhysiqueBlend(CharacterVertex &pio_rVertex,
                            DWORD pi_dwNameIndex,
                            float pi_fWeight)
{
  pio_rVertex.blend = (pi_dwNameIndex == 0) ? pi_fWeight : 1.0f - pi_fWeight;
}

void ApplyExpandedPhysiqueBlend(const MeshRecord &pi_rRecord,
                                std::vector<CharacterVertex> &pio_vecVertices)
{
  if (!pi_rRecord.m_pPhysiqueRecords)
  {
    return;
  }

  for (DWORD i = 0; i < pi_rRecord.m_dwPhysiqueCount; ++i)
  {
    const BYTE *l_pRecord = pi_rRecord.m_pPhysiqueRecords + (i * 40);
    const DWORD l_dwVertexIndex = *reinterpret_cast<const DWORD *>(l_pRecord + 0);
    const DWORD l_dwInfluenceCount = *reinterpret_cast<const DWORD *>(l_pRecord + 4);
    if (l_dwVertexIndex >= pio_vecVertices.size())
    {
      continue;
    }

    for (DWORD j = 0; j < l_dwInfluenceCount && j < 4; ++j)
    {
      const DWORD l_dwNameIndex = *reinterpret_cast<const DWORD *>(l_pRecord + 8 + (j * 4));
      if (l_dwNameIndex == 0xFFFFFFFF ||
          l_dwNameIndex >= pi_rRecord.m_dwPhysiqueNameCount)
      {
        continue;
      }

      const float l_fWeight = *reinterpret_cast<const float *>(l_pRecord + 24 + (j * 4));
      SetVertexPhysiqueBlend(pio_vecVertices[l_dwVertexIndex], l_dwNameIndex, l_fWeight);
    }
  }
}

void ApplyLegacyPhysiqueBlend(const MeshRecord &pi_rRecord,
                              std::vector<CharacterVertex> &pio_vecVertices)
{
  std::vector<std::string> l_vecNames;
  CollectPhysiqueNames(pi_rRecord, l_vecNames);
  if (l_vecNames.empty())
  {
    return;
  }

  for (DWORD i = 0; i < pi_rRecord.m_dwPhysiqueCount; ++i)
  {
    const BYTE *l_pRecord = pi_rRecord.m_pPhysiqueRecords + (i * 424);
    const DWORD l_dwVertexIndex = *reinterpret_cast<const DWORD *>(l_pRecord + 0);
    const DWORD l_dwInfluenceCount = *reinterpret_cast<const DWORD *>(l_pRecord + 4);
    if (l_dwVertexIndex >= pio_vecVertices.size())
    {
      continue;
    }

    for (DWORD j = 0; j < l_dwInfluenceCount && j < 4; ++j)
    {
      const char *l_pName = reinterpret_cast<const char *>(l_pRecord + 8 + (j * 100));
      const float l_fWeight = *reinterpret_cast<const float *>(l_pRecord + 408 + (j * 4));
      for (DWORD k = 0; k < l_vecNames.size(); ++k)
      {
        if (_stricmp(l_vecNames[k].c_str(), l_pName) == 0)
        {
          SetVertexPhysiqueBlend(pio_vecVertices[l_dwVertexIndex], k, l_fWeight);
          break;
        }
      }
    }
  }
}

void ApplyPhysiqueBlend(const MeshRecord &pi_rRecord,
                        std::vector<CharacterVertex> &pio_vecVertices)
{
  if (!pi_rRecord.m_pPhysiqueRecords || !pi_rRecord.m_dwPhysiqueCount)
  {
    return;
  }

  if (pi_rRecord.m_bExpandedPhysique)
  {
    ApplyExpandedPhysiqueBlend(pi_rRecord, pio_vecVertices);
  }
  else
  {
    ApplyLegacyPhysiqueBlend(pi_rRecord, pio_vecVertices);
  }
}

bool BuildMeshVertices(const MeshRecord &pi_rRecord,
                       std::vector<CharacterVertex> &po_vecVertices)
{
  po_vecVertices.clear();
  if ((pi_rRecord.m_dwVertexCount && !pi_rRecord.m_pVertices) ||
      (pi_rRecord.m_dwFaceCount && !pi_rRecord.m_pFaces))
  {
    return false;
  }
  if (!pi_rRecord.m_dwFaceCount)
  {
    return true;
  }
  if (!pi_rRecord.m_dwVertexCount)
  {
    return false;
  }

  std::vector<CharacterVertex> l_vecSourceVertices(pi_rRecord.m_dwVertexCount);
  for (DWORD i = 0; i < pi_rRecord.m_dwVertexCount; ++i)
  {
    ConvertVertex(l_vecSourceVertices[i], pi_rRecord.m_pVertices + (i * 28));
  }
  ApplyPhysiqueBlend(pi_rRecord, l_vecSourceVertices);

  po_vecVertices.resize(static_cast<size_t>(pi_rRecord.m_dwFaceCount) * 3);
  for (DWORD i = 0; i < pi_rRecord.m_dwFaceCount; ++i)
  {
    const BYTE *l_pFace = pi_rRecord.m_pFaces + (i * 88);
    const DWORD l_adwIndex[3] =
    {
      *reinterpret_cast<const DWORD *>(l_pFace + 0),
      *reinterpret_cast<const DWORD *>(l_pFace + 4),
      *reinterpret_cast<const DWORD *>(l_pFace + 8)
    };
    const float l_afUV[3][2] =
    {
      {
        *reinterpret_cast<const float *>(l_pFace + 48),
        *reinterpret_cast<const float *>(l_pFace + 52)
      },
      {
        *reinterpret_cast<const float *>(l_pFace + 60),
        *reinterpret_cast<const float *>(l_pFace + 64)
      },
      {
        *reinterpret_cast<const float *>(l_pFace + 72),
        *reinterpret_cast<const float *>(l_pFace + 76)
      }
    };

    for (DWORD j = 0; j < 3; ++j)
    {
      if (l_adwIndex[j] >= pi_rRecord.m_dwVertexCount)
      {
        return false;
      }
      CharacterVertex &l_rVertex = po_vecVertices[(i * 3) + j];
      l_rVertex = l_vecSourceVertices[l_adwIndex[j]];
      SetUV(l_rVertex, l_afUV[j][0], l_afUV[j][1]);
    }
  }

  return true;
}

bool CreateMeshVertexBuffer(IDirect3DDevice8 *pi_pDevice,
                            ObjectMesh *pio_pObjectMesh,
                            const std::vector<CharacterVertex> &pi_vecVertices)
{
  if (!pi_pDevice || !pio_pObjectMesh || pi_vecVertices.empty())
  {
    return false;
  }

  IDirect3DVertexBuffer8 *l_pVertexBuffer = NULL;
  const UINT l_uBufferSize =
    static_cast<UINT>(pi_vecVertices.size() * sizeof(CharacterVertex));
  if (FAILED(pi_pDevice->CreateVertexBuffer(l_uBufferSize,
                                            0,
                                            D3DFVF_CHARACTER_VERTEX,
                                            D3DPOOL_MANAGED,
                                            &l_pVertexBuffer)) ||
      !l_pVertexBuffer)
  {
    return false;
  }

  void *l_pLocked = NULL;
  if (FAILED(l_pVertexBuffer->Lock(0, l_uBufferSize, reinterpret_cast<BYTE **>(&l_pLocked), 0)) ||
      !l_pLocked)
  {
    l_pVertexBuffer->Release();
    return false;
  }

  memcpy(l_pLocked, &pi_vecVertices[0], l_uBufferSize);
  l_pVertexBuffer->Unlock();

  CharacterMR::Field<IDirect3DVertexBuffer8 *>(pio_pObjectMesh,
                                               CharacterMR::OBJ_VERTEX_BUFFER) = l_pVertexBuffer;
  CharacterMR::Field<DWORD>(pio_pObjectMesh,
                            CharacterMR::OBJ_VERTEX_COUNT) =
    static_cast<DWORD>(pi_vecVertices.size());
  CharacterMR::Field<DWORD>(pio_pObjectMesh, CharacterMR::OBJ_VERTEX_COUNT + 4) =
    static_cast<DWORD>(pi_vecVertices.size());
  return true;
}

const char *GetCharacterTexturePath(CHARACTEROBJECT *pi_pCharacterObject,
                                    TextureManager *pi_pTextureManager)
{
  const char *l_pCharacterPath =
    reinterpret_cast<const char *>(pi_pCharacterObject) + CharacterMR::CHOBJ_TEXTURE_PATH;
  if (l_pCharacterPath[0])
  {
    return l_pCharacterPath;
  }

  const char *l_pManagerPath =
    reinterpret_cast<const char *>(pi_pTextureManager) + kTextureManagerPathOffset;
  return l_pManagerPath;
}

void AssignMeshTexture(TextureManager *pi_pTextureManager,
                       CHARACTEROBJECT *pi_pCharacterObject,
                       ObjectMesh *pio_pObjectMesh,
                       const char *pi_pRawTextureName,
                       size_t pi_stTextureOffset,
                       size_t pi_stTextureIDOffset)
{
  if (!pi_pTextureManager || !pio_pObjectMesh || !pi_pRawTextureName)
  {
    return;
  }

  char l_szTextureName[128];
  CharacterMR::CopyBaseFileNameUpper(pi_pRawTextureName,
                                     l_szTextureName,
                                     sizeof(l_szTextureName));
  if (!l_szTextureName[0])
  {
    CharacterMR::Field<IDirect3DTexture8 *>(pio_pObjectMesh, pi_stTextureOffset) = NULL;
    CharacterMR::Field<DWORD>(pio_pObjectMesh, pi_stTextureIDOffset) = 0;
    return;
  }

  DWORD l_dwTextureID = pi_pTextureManager->GetTextureID(l_szTextureName);
  if (!l_dwTextureID)
  {
    pi_pTextureManager->AddTexture(GetCharacterTexturePath(pi_pCharacterObject, pi_pTextureManager),
                                   l_szTextureName);
    l_dwTextureID = pi_pTextureManager->GetTextureID(l_szTextureName);
  }

  CharacterMR::Field<DWORD>(pio_pObjectMesh, pi_stTextureIDOffset) = l_dwTextureID;
  CharacterMR::Field<IDirect3DTexture8 *>(pio_pObjectMesh, pi_stTextureOffset) =
    pi_pTextureManager->GetTexture2(l_dwTextureID);
}

void MultiplyMatrix(D3DXMATRIX *po_pOut,
                    const D3DXMATRIX *pi_pA,
                    const D3DXMATRIX *pi_pB)
{
  D3DXMATRIX l_matResult;
  D3DXMatrixMultiply(&l_matResult, pi_pA, pi_pB);
  *po_pOut = l_matResult;
}

void InitCharacterMRMatView(D3DXMATRIX *po_pMatrix)
{
  D3DXMatrixIdentity(po_pMatrix);
  po_pMatrix->_22 = 0.0f;
  po_pMatrix->_23 = 1.0f;
  po_pMatrix->_32 = 1.0f;
  po_pMatrix->_33 = 0.0f;
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

ObjectMesh *FindObjectMeshByName(ObjectMesh *pi_pObjectMesh,
                                 DWORD pi_dwObjectCount,
                                 const char *pi_pName)
{
  if (!pi_pObjectMesh || !pi_pName || !pi_pName[0])
  {
    return NULL;
  }

  for (DWORD i = 0; i < pi_dwObjectCount; ++i)
  {
    ObjectMesh *l_pMesh =
      reinterpret_cast<ObjectMesh *>(reinterpret_cast<BYTE *>(pi_pObjectMesh) +
                                     (i * sizeof(ObjectMesh)));
    if (!_stricmp(reinterpret_cast<const char *>(l_pMesh) + CharacterMR::OBJ_NAME,
                 pi_pName))
    {
      return l_pMesh;
    }
  }

  return NULL;
}

void MatchParents(ObjectMesh *pi_pParentMesh,
                  DWORD pi_dwParentCount,
                  ObjectMesh *pio_pChildMesh,
                  DWORD pi_dwChildCount,
                  bool pi_bRematchOnly)
{
  if (!pio_pChildMesh)
  {
    return;
  }

  for (DWORD i = 0; i < pi_dwChildCount; ++i)
  {
    ObjectMesh *l_pChild =
      reinterpret_cast<ObjectMesh *>(reinterpret_cast<BYTE *>(pio_pChildMesh) +
                                     (i * sizeof(ObjectMesh)));
    if (pi_bRematchOnly &&
        !CharacterMR::Field<DWORD>(l_pChild, CharacterMR::OBJ_PARENT_CHANGED))
    {
      continue;
    }

    const char *l_pParentName =
      reinterpret_cast<const char *>(l_pChild) + CharacterMR::OBJ_PARENT_NAME;
    ObjectMesh *l_pParent = FindObjectMeshByName(pi_pParentMesh,
                                                 pi_dwParentCount,
                                                 l_pParentName);
    if (!l_pParent)
    {
      l_pParent = FindObjectMeshByName(pio_pChildMesh,
                                       pi_dwChildCount,
                                       l_pParentName);
    }

    if (l_pParent)
    {
      CharacterMR::Field<ObjectMesh *>(l_pChild, CharacterMR::OBJ_PARENT) = l_pParent;
      CharacterMR::Field<BYTE>(l_pChild, CharacterMR::OBJ_HAS_PARENT) = 1;
      if (pi_pParentMesh != pio_pChildMesh)
      {
        CharacterMR::Field<DWORD>(l_pChild, CharacterMR::OBJ_PARENT_CHANGED) = 1;
      }
    }
  }
}

void AssignBlendParents(ObjectMesh *pi_pParentMesh,
                        DWORD pi_dwParentCount,
                        ObjectMesh *pio_pChildMesh,
                        DWORD pi_dwChildCount,
                        const std::vector<MeshRecord> &pi_vecRecords)
{
  if (!pio_pChildMesh || pi_vecRecords.size() < pi_dwChildCount)
  {
    return;
  }

  for (DWORD i = 0; i < pi_dwChildCount; ++i)
  {
    ObjectMesh *l_pChild =
      reinterpret_cast<ObjectMesh *>(reinterpret_cast<BYTE *>(pio_pChildMesh) +
                                     (i * sizeof(ObjectMesh)));

    std::vector<std::string> l_vecNames;
    CollectPhysiqueNames(pi_vecRecords[i], l_vecNames);
    if (l_vecNames.empty())
    {
      continue;
    }

    std::vector<ObjectMesh *> l_vecParents;
    for (size_t j = 0; j < l_vecNames.size(); ++j)
    {
      ObjectMesh *l_pParent = FindObjectMeshByName(pi_pParentMesh,
                                                   pi_dwParentCount,
                                                   l_vecNames[j].c_str());
      if (!l_pParent)
      {
        l_pParent = FindObjectMeshByName(pio_pChildMesh,
                                         pi_dwChildCount,
                                         l_vecNames[j].c_str());
      }
      if (l_pParent)
      {
        l_vecParents.push_back(l_pParent);
      }
    }

    if (l_vecParents.empty())
    {
      continue;
    }

    ObjectMesh **l_ppParents =
      static_cast<ObjectMesh **>(::operator new(sizeof(ObjectMesh *) * l_vecParents.size()));
    D3DXMATRIX *l_pBlendMatrices =
      static_cast<D3DXMATRIX *>(::operator new(sizeof(D3DXMATRIX) * l_vecParents.size()));
    for (size_t j = 0; j < l_vecParents.size(); ++j)
    {
      l_ppParents[j] = l_vecParents[j];
      D3DXMatrixIdentity(&l_pBlendMatrices[j]);
    }

    CharacterMR::Field<DWORD>(l_pChild, CharacterMR::OBJ_BLEND_COUNT) =
      static_cast<DWORD>(l_vecParents.size());
    CharacterMR::Field<ObjectMesh **>(l_pChild, CharacterMR::OBJ_BLEND_PARENT) =
      l_ppParents;
    CharacterMR::Field<D3DXMATRIX *>(l_pChild, CharacterMR::OBJ_BLEND_COUNT + 0x14) =
      l_pBlendMatrices;
  }
}

void RefreshObjectMeshLocalResults(ObjectMesh *pio_pObjectMesh,
                                   DWORD pi_dwObjectCount,
                                   bool pi_bRebuildParentLocal)
{
  if (!pio_pObjectMesh)
  {
    return;
  }

  for (DWORD i = 0; i < pi_dwObjectCount; ++i)
  {
    ObjectMesh *l_pMesh = &pio_pObjectMesh[i];
    ObjectMesh *l_pParent =
      CharacterMR::Field<ObjectMesh *>(l_pMesh, CharacterMR::OBJ_PARENT);
    if (pi_bRebuildParentLocal && l_pParent)
    {
      MultiplyMatrix(&CharacterMR::Field<D3DXMATRIX>(l_pMesh,
                                                     CharacterMR::OBJ_LOCAL_MATRIX),
                     &CharacterMR::Field<D3DXMATRIX>(l_pMesh,
                                                     CharacterMR::OBJ_BIND_MATRIX),
                     &CharacterMR::Field<D3DXMATRIX>(l_pParent,
                                                     CharacterMR::OBJ_INV_BIND_MATRIX));
    }
    memcpy(&CharacterMR::Field<D3DXMATRIX>(l_pMesh, CharacterMR::OBJ_RESULT_MATRIX),
           &CharacterMR::Field<D3DXMATRIX>(l_pMesh, CharacterMR::OBJ_LOCAL_MATRIX),
           sizeof(D3DXMATRIX));
  }
}

void BuildBlendMatrices(CHARACTEROBJECT *pio_pCharacterObject)
{
  const DWORD l_dwObjectCount =
    CharacterMR::Field<DWORD>(pio_pCharacterObject, CharacterMR::CHOBJ_MAX_OBJ);
  for (DWORD i = 0; i < l_dwObjectCount; ++i)
  {
    ObjectMesh *l_pMesh = GetMeshAt(pio_pCharacterObject, i);
    if (!l_pMesh)
    {
      continue;
    }

    const DWORD l_dwBlendCount =
      CharacterMR::Field<DWORD>(l_pMesh, CharacterMR::OBJ_BLEND_COUNT);
    ObjectMesh **l_ppBlendParents =
      CharacterMR::Field<ObjectMesh **>(l_pMesh, CharacterMR::OBJ_BLEND_PARENT);
    D3DXMATRIX *l_pBlendMatrices =
      CharacterMR::Field<D3DXMATRIX *>(l_pMesh, CharacterMR::OBJ_BLEND_COUNT + 0x14);
    if (!l_dwBlendCount || !l_ppBlendParents || !l_pBlendMatrices)
    {
      continue;
    }

    for (DWORD j = 0; j < l_dwBlendCount; ++j)
    {
      if (!l_ppBlendParents[j])
      {
        continue;
      }

      D3DXMATRIX l_matBind;
      MultiplyMatrix(&l_matBind,
                     &CharacterMR::Field<D3DXMATRIX>(l_pMesh, CharacterMR::OBJ_BIND_MATRIX),
                     &CharacterMR::Field<D3DXMATRIX>(l_ppBlendParents[j],
                                                     CharacterMR::OBJ_INV_BIND_MATRIX));
      MultiplyMatrix(&l_pBlendMatrices[j],
                     &l_matBind,
                     &CharacterMR::Field<D3DXMATRIX>(l_ppBlendParents[j],
                                                     CharacterMR::OBJ_RESULT_MATRIX));
    }
  }
}

bool BuildObjectMeshes(CRender *pi_pRender,
                       CHARACTEROBJECT *pio_pCharacterObject,
                       TextureManager *pi_pTextureManager,
                       const std::vector<MeshRecord> &pi_vecRecords,
                       bool pi_bRebuildParentLocal)
{
  if (!pio_pCharacterObject || pi_vecRecords.empty())
  {
    return false;
  }

  IDirect3DDevice8 *l_pDevice = pi_pRender ? GetRenderDevice(pi_pRender) : NULL;
  if (!l_pDevice && pi_pTextureManager)
  {
    l_pDevice = GetTextureDevice(pi_pTextureManager);
  }
  if (!l_pDevice)
  {
    return false;
  }

  const DWORD l_dwObjectCount = static_cast<DWORD>(pi_vecRecords.size());
  ObjectMesh *l_pObjectMeshes =
    static_cast<ObjectMesh *>(::operator new(sizeof(ObjectMesh) * l_dwObjectCount));
  for (DWORD i = 0; i < l_dwObjectCount; ++i)
  {
    CharacterMR::InitObjectMesh(&l_pObjectMeshes[i]);
  }

  for (DWORD i = 0; i < l_dwObjectCount; ++i)
  {
    ObjectMesh *l_pMesh = &l_pObjectMeshes[i];
    const MeshRecord &l_rRecord = pi_vecRecords[i];
    const BYTE *l_pHeader = l_rRecord.m_pHeader;

    CharacterMR::CopyBaseFileNameUpper(reinterpret_cast<const char *>(l_pHeader),
                                       reinterpret_cast<char *>(l_pMesh) + CharacterMR::OBJ_NAME,
                                       64);
    CharacterMR::CopyBaseFileNameUpper(reinterpret_cast<const char *>(l_pHeader + 100),
                                       reinterpret_cast<char *>(l_pMesh) + CharacterMR::OBJ_PARENT_NAME,
                                       64);

    memcpy(&CharacterMR::Field<D3DXMATRIX>(l_pMesh, CharacterMR::OBJ_BASE_MATRIX),
           l_pHeader + 200,
           sizeof(D3DXMATRIX));
    memcpy(&CharacterMR::Field<D3DXMATRIX>(l_pMesh, CharacterMR::OBJ_BIND_MATRIX),
           l_pHeader + 200,
           sizeof(D3DXMATRIX));
    memcpy(&CharacterMR::Field<D3DXMATRIX>(l_pMesh, CharacterMR::OBJ_LOCAL_MATRIX),
           l_pHeader + 264,
           sizeof(D3DXMATRIX));
    memcpy(&CharacterMR::Field<D3DXMATRIX>(l_pMesh, CharacterMR::OBJ_SOURCE_MATRIX),
           l_pHeader + 328,
           sizeof(D3DXMATRIX));

    D3DXMatrixInverse(&CharacterMR::Field<D3DXMATRIX>(l_pMesh,
                                                     CharacterMR::OBJ_INV_BIND_MATRIX),
                      NULL,
                      &CharacterMR::Field<D3DXMATRIX>(l_pMesh,
                                                     CharacterMR::OBJ_BIND_MATRIX));
    D3DXMATRIX l_matSourceInverse;
    D3DXMatrixInverse(&l_matSourceInverse,
                      NULL,
                      &CharacterMR::Field<D3DXMATRIX>(l_pMesh,
                                                     CharacterMR::OBJ_SOURCE_MATRIX));
    D3DXMatrixMultiply(&CharacterMR::Field<D3DXMATRIX>(l_pMesh, 0x2B0),
                       &l_matSourceInverse,
                       &CharacterMR::Field<D3DXMATRIX>(l_pMesh,
                                                      CharacterMR::OBJ_BIND_MATRIX));
    D3DXMatrixIdentity(&CharacterMR::Field<D3DXMATRIX>(l_pMesh,
                                                       CharacterMR::OBJ_RESULT_MATRIX));
    D3DXMatrixIdentity(&CharacterMR::Field<D3DXMATRIX>(l_pMesh,
                                                       CharacterMR::OBJ_PREV_RESULT_MATRIX));

    CharacterMR::Field<float>(l_pMesh, CharacterMR::OBJ_EXTRA_SCALE + 0) = 1.0f;
    CharacterMR::Field<float>(l_pMesh, CharacterMR::OBJ_EXTRA_SCALE + 4) = 1.0f;
    CharacterMR::Field<float>(l_pMesh, CharacterMR::OBJ_EXTRA_SCALE + 8) = 1.0f;

    AssignMeshTexture(pi_pTextureManager,
                      pio_pCharacterObject,
                      l_pMesh,
                      reinterpret_cast<const char *>(l_pHeader + 398),
                      CharacterMR::OBJ_TEXTURE0,
                      CharacterMR::OBJ_TEXTURE0_ID);
    AssignMeshTexture(pi_pTextureManager,
                      pio_pCharacterObject,
                      l_pMesh,
                      reinterpret_cast<const char *>(l_pHeader + 498),
                      CharacterMR::OBJ_TEXTURE1,
                      CharacterMR::OBJ_TEXTURE1_ID);

    CharacterMR::Field<DWORD>(l_pMesh, CharacterMR::OBJ_SCROLL_SPEED) = 1;
    CharacterMR::Field<DWORD>(l_pMesh, 0x31C) =
      *reinterpret_cast<const DWORD *>(l_pHeader + 598);
    CharacterMR::Field<float>(l_pMesh, 0x3C0) =
      *reinterpret_cast<const float *>(l_pHeader + 602);
    CharacterMR::Field<float>(l_pMesh, 0x3C4) =
      *reinterpret_cast<const float *>(l_pHeader + 606);
    CharacterMR::Field<float>(l_pMesh, 0x3C8) =
      *reinterpret_cast<const float *>(l_pHeader + 610);
    CharacterMR::Field<float>(l_pMesh, 0x3CC) =
      *reinterpret_cast<const float *>(l_pHeader + 614);
    CharacterMR::Field<float>(l_pMesh, 0x3D0) =
      *reinterpret_cast<const float *>(l_pHeader + 618);
    CharacterMR::Field<float>(l_pMesh, 0x3D4) =
      *reinterpret_cast<const float *>(l_pHeader + 622);
    CharacterMR::Field<BYTE>(l_pMesh, 0x3B4) = *(l_pHeader + 631);
    CharacterMR::Field<BYTE>(l_pMesh, 0x3B5) = *(l_pHeader + 632);
    CharacterMR::Field<BYTE>(l_pMesh, 0x3B6) = *(l_pHeader + 633);
    CharacterMR::Field<BYTE>(l_pMesh, 0x3BD) = *(l_pHeader + 634);
    CharacterMR::Field<BYTE>(l_pMesh, 0x3BC) = *(l_pHeader + 639);

    std::vector<CharacterVertex> l_vecVertices;
    if (!BuildMeshVertices(l_rRecord, l_vecVertices))
    {
      for (DWORD j = 0; j <= i; ++j)
      {
        CharacterMR::ReleaseObjectMesh(&l_pObjectMeshes[j]);
      }
      ::operator delete(l_pObjectMeshes);
      return false;
    }
    if (!l_vecVertices.empty())
    {
      if (!CreateMeshVertexBuffer(l_pDevice, l_pMesh, l_vecVertices))
      {
        for (DWORD j = 0; j <= i; ++j)
        {
          CharacterMR::ReleaseObjectMesh(&l_pObjectMeshes[j]);
        }
        ::operator delete(l_pObjectMeshes);
        return false;
      }
    }
    else
    {
      CharacterMR::Field<IDirect3DVertexBuffer8 *>(l_pMesh,
                                                   CharacterMR::OBJ_VERTEX_BUFFER) = NULL;
      CharacterMR::Field<DWORD>(l_pMesh, CharacterMR::OBJ_VERTEX_COUNT) = 0;
    }
  }

  MatchParents(l_pObjectMeshes, l_dwObjectCount, l_pObjectMeshes, l_dwObjectCount, false);
  RefreshObjectMeshLocalResults(l_pObjectMeshes,
                                l_dwObjectCount,
                                pi_bRebuildParentLocal);

  CHARACTEROBJECT *l_pParentBone =
    CharacterMR::Field<CHARACTEROBJECT *>(pio_pCharacterObject,
                                          CharacterMR::CHOBJ_PARENT_BONE);
  if (l_pParentBone && CharacterMR::Field<DWORD>(l_pParentBone, CharacterMR::CHOBJ_LOADED))
  {
    MatchParents(CharacterMR::Field<ObjectMesh *>(l_pParentBone, CharacterMR::CHOBJ_MESH),
                 CharacterMR::Field<DWORD>(l_pParentBone, CharacterMR::CHOBJ_MAX_OBJ),
                 l_pObjectMeshes,
                 l_dwObjectCount,
                 false);
    AssignBlendParents(CharacterMR::Field<ObjectMesh *>(l_pParentBone, CharacterMR::CHOBJ_MESH),
                       CharacterMR::Field<DWORD>(l_pParentBone, CharacterMR::CHOBJ_MAX_OBJ),
                       l_pObjectMeshes,
                       l_dwObjectCount,
                       pi_vecRecords);
  }
  else
  {
    AssignBlendParents(l_pObjectMeshes,
                       l_dwObjectCount,
                       l_pObjectMeshes,
                       l_dwObjectCount,
                       pi_vecRecords);
  }
  RefreshObjectMeshLocalResults(l_pObjectMeshes,
                                l_dwObjectCount,
                                pi_bRebuildParentLocal);

  CharacterMR::Field<ObjectMesh *>(pio_pCharacterObject, CharacterMR::CHOBJ_MESH) =
    l_pObjectMeshes;
  CharacterMR::Field<DWORD>(pio_pCharacterObject, CharacterMR::CHOBJ_MAX_OBJ) =
    l_dwObjectCount;
  return true;
}

void FreeAnimationData(ObjectMesh *pio_pObjectMesh)
{
  if (!pio_pObjectMesh)
  {
    return;
  }

  AnimationDataCompat *l_pAnimation =
    CharacterMR::Field<AnimationDataCompat *>(pio_pObjectMesh,
                                             CharacterMR::OBJ_ANIMATION_DATA);
  if (l_pAnimation)
  {
    ::operator delete(l_pAnimation);
  }
  CharacterMR::Field<AnimationDataCompat *>(pio_pObjectMesh,
                                            CharacterMR::OBJ_ANIMATION_DATA) = NULL;
  CharacterMR::Field<BYTE>(pio_pObjectMesh,
                           CharacterMR::OBJ_ANIMATION_ENABLED) = 0;
  CharacterMR::Field<DWORD>(pio_pObjectMesh,
                            CharacterMR::OBJ_ANIMATION_MAX_FRAME) = 0;
}

int MatchAnimationData(const BYTE *pi_pAnimationHeader,
                       AnimationDataCompat *pi_pAnimationData,
                       CHARACTEROBJECT *pio_pCharacterObject,
                       int pi_nStartIndex)
{
  if (!pi_pAnimationHeader || !pio_pCharacterObject)
  {
    return -1;
  }

  const DWORD l_dwObjectCount =
    CharacterMR::Field<DWORD>(pio_pCharacterObject, CharacterMR::CHOBJ_MAX_OBJ);
  if (pi_nStartIndex < 0 || static_cast<DWORD>(pi_nStartIndex) >= l_dwObjectCount)
  {
    return -1;
  }

  for (DWORD i = static_cast<DWORD>(pi_nStartIndex); i < l_dwObjectCount; ++i)
  {
    ObjectMesh *l_pMesh = GetMeshAt(pio_pCharacterObject, i);
    if (!l_pMesh)
    {
      continue;
    }

    if (_stricmp(reinterpret_cast<const char *>(l_pMesh) + CharacterMR::OBJ_NAME,
                reinterpret_cast<const char *>(pi_pAnimationHeader)) != 0)
    {
      continue;
    }

    const WORD l_wHasAnimation =
      *reinterpret_cast<const WORD *>(pi_pAnimationHeader + 100);
    const WORD l_wMaxFrame =
      *reinterpret_cast<const WORD *>(pi_pAnimationHeader + 102);
    FreeAnimationData(l_pMesh);

    if (l_wHasAnimation)
    {
      CharacterMR::Field<BYTE>(l_pMesh, CharacterMR::OBJ_ANIMATION_ENABLED) = 1;
      CharacterMR::Field<AnimationDataCompat *>(l_pMesh,
                                                CharacterMR::OBJ_ANIMATION_DATA) =
        pi_pAnimationData;
      CharacterMR::Field<DWORD>(l_pMesh, CharacterMR::OBJ_ANIMATION_MAX_FRAME) =
        l_wMaxFrame;
      DWORD &l_rMaxFrame =
        CharacterMR::Field<DWORD>(pio_pCharacterObject, CharacterMR::CHOBJ_MAX_FRAME);
      if (l_rMaxFrame < l_wMaxFrame)
      {
        l_rMaxFrame = l_wMaxFrame;
      }
    }
    else
    {
      if (pi_pAnimationData)
      {
        ::operator delete(pi_pAnimationData);
      }
    }
    return static_cast<int>(i);
  }

  return -1;
}

void ImportAnimationData(CHARACTEROBJECT *pio_pCharacterObject, BYTE *pi_pAnimationBytes)
{
  if (!pio_pCharacterObject || !pi_pAnimationBytes)
  {
    return;
  }

  int l_nMatchedIndex = 0;
  int l_nOffset = 2;
  BYTE l_byAnimationCount = pi_pAnimationBytes[0];
  while (l_byAnimationCount)
  {
    BYTE *l_pHeader = pi_pAnimationBytes + l_nOffset;
    AnimationDataCompat *l_pAnimationData =
      static_cast<AnimationDataCompat *>(::operator new(sizeof(AnimationDataCompat)));
    ZeroMemory(l_pAnimationData, sizeof(AnimationDataCompat));

    l_pAnimationData->m_wRotCount =
      *reinterpret_cast<WORD *>(pi_pAnimationBytes + l_nOffset + 140);
    int l_nDataOffset = l_nOffset + 142;
    l_pAnimationData->m_pRotKeys = pi_pAnimationBytes + l_nDataOffset;

    l_nDataOffset += 20 * l_pAnimationData->m_wRotCount;
    l_pAnimationData->m_wTransCount =
      *reinterpret_cast<WORD *>(pi_pAnimationBytes + l_nDataOffset);
    l_nDataOffset += 2;
    l_pAnimationData->m_pTransKeys = pi_pAnimationBytes + l_nDataOffset;

    l_nDataOffset += 16 * l_pAnimationData->m_wTransCount;
    l_pAnimationData->m_wScaleCount =
      *reinterpret_cast<WORD *>(pi_pAnimationBytes + l_nDataOffset);
    l_nDataOffset += 2;
    l_pAnimationData->m_pScaleKeys = pi_pAnimationBytes + l_nDataOffset;

    l_nDataOffset += 16 * l_pAnimationData->m_wScaleCount;
    l_pAnimationData->m_wAlphaCount =
      *reinterpret_cast<WORD *>(pi_pAnimationBytes + l_nDataOffset);
    l_nDataOffset += 2;
    l_pAnimationData->m_pAlphaKeys = pi_pAnimationBytes + l_nDataOffset;
    l_nOffset = l_nDataOffset + (8 * l_pAnimationData->m_wAlphaCount);

    const int l_nNewMatch =
      MatchAnimationData(l_pHeader, l_pAnimationData, pio_pCharacterObject, l_nMatchedIndex);
    if (l_nNewMatch == -1)
    {
      ::operator delete(l_pAnimationData);
    }
    else
    {
      l_nMatchedIndex = l_nNewMatch;
    }

    --l_byAnimationCount;
  }
}

DWORD FindMaxAnimationFrame(BYTE *pi_pAnimationBytes)
{
  if (!pi_pAnimationBytes)
  {
    return 0;
  }

  DWORD l_dwMaxFrame = 0;
  int l_nOffset = 2;
  BYTE l_byAnimationCount = pi_pAnimationBytes[0];
  while (l_byAnimationCount)
  {
    BYTE *l_pHeader = pi_pAnimationBytes + l_nOffset;
    const WORD l_wMaxFrame = *reinterpret_cast<WORD *>(l_pHeader + 102);
    if (l_dwMaxFrame < l_wMaxFrame)
    {
      l_dwMaxFrame = l_wMaxFrame;
    }

    int l_nDataOffset = l_nOffset + 142;
    const WORD l_wRotCount = *reinterpret_cast<WORD *>(l_pHeader + 140);
    l_nDataOffset += 20 * l_wRotCount;
    const WORD l_wTransCount = *reinterpret_cast<WORD *>(pi_pAnimationBytes + l_nDataOffset);
    l_nDataOffset += 2 + (16 * l_wTransCount);
    const WORD l_wScaleCount = *reinterpret_cast<WORD *>(pi_pAnimationBytes + l_nDataOffset);
    l_nDataOffset += 2 + (16 * l_wScaleCount);
    const WORD l_wAlphaCount = *reinterpret_cast<WORD *>(pi_pAnimationBytes + l_nDataOffset);
    l_nOffset = l_nDataOffset + 2 + (8 * l_wAlphaCount);

    --l_byAnimationCount;
  }

  return l_dwMaxFrame;
}

int FindKeyIndex(const BYTE *pi_pKeys,
                 WORD pi_wKeyCount,
                 DWORD pi_dwFrame,
                 DWORD pi_dwKeyStride,
                 DWORD pi_dwFrameOffset)
{
  if (!pi_pKeys || pi_wKeyCount < 2)
  {
    return -1;
  }

  if (pi_dwFrame >= *reinterpret_cast<const DWORD *>(pi_pKeys +
                                                     ((pi_wKeyCount - 1) * pi_dwKeyStride) +
                                                     pi_dwFrameOffset))
  {
    return static_cast<int>(pi_wKeyCount) - 2;
  }

  for (WORD i = 0; i < pi_wKeyCount - 1; ++i)
  {
    const DWORD l_dwNextFrame =
      *reinterpret_cast<const DWORD *>(pi_pKeys + ((i + 1) * pi_dwKeyStride) +
                                      pi_dwFrameOffset);
    if (pi_dwFrame < l_dwNextFrame)
    {
      return i;
    }
  }

  return 0;
}

float KeyRatio(const BYTE *pi_pKey,
               DWORD pi_dwFrame,
               DWORD pi_dwKeyStride,
               DWORD pi_dwFrameOffset)
{
  const DWORD l_dwFrame0 =
    *reinterpret_cast<const DWORD *>(pi_pKey + pi_dwFrameOffset);
  const DWORD l_dwFrame1 =
    *reinterpret_cast<const DWORD *>(pi_pKey + pi_dwKeyStride + pi_dwFrameOffset);
  if (l_dwFrame1 == l_dwFrame0)
  {
    return 0.0f;
  }
  return Clamp01(static_cast<float>(pi_dwFrame - l_dwFrame0) /
                 static_cast<float>(l_dwFrame1 - l_dwFrame0));
}

void ApplyObjectExtraScale(D3DXMATRIX &pio_rMatrix, ObjectMesh *pi_pMesh)
{
  if (!pi_pMesh)
  {
    return;
  }

  D3DXMATRIX l_matScale;
  D3DXMatrixScaling(&l_matScale,
                    CharacterMR::Field<float>(pi_pMesh,
                                              CharacterMR::OBJ_EXTRA_SCALE + 0),
                    CharacterMR::Field<float>(pi_pMesh,
                                              CharacterMR::OBJ_EXTRA_SCALE + 4),
                    CharacterMR::Field<float>(pi_pMesh,
                                              CharacterMR::OBJ_EXTRA_SCALE + 8));
  MultiplyMatrix(&pio_rMatrix, &pio_rMatrix, &l_matScale);
}

void ApplyScaleTrack(D3DXMATRIX &pio_rMatrix,
                     const AnimationDataCompat *pi_pAnimationData,
                     DWORD pi_dwFrame,
                     ObjectMesh *pi_pMesh)
{
  if (!pi_pAnimationData || !pi_pAnimationData->m_wScaleCount)
  {
    MultiplyMatrix(&pio_rMatrix,
                   &pio_rMatrix,
                   &CharacterMR::Field<D3DXMATRIX>(pi_pMesh,
                                                   CharacterMR::OBJ_SOURCE_MATRIX));
    ApplyObjectExtraScale(pio_rMatrix, pi_pMesh);
    return;
  }

  if (pi_pAnimationData->m_wScaleCount == 1)
  {
    const BYTE *l_pKey = pi_pAnimationData->m_pScaleKeys;
    pio_rMatrix._11 = *reinterpret_cast<const float *>(l_pKey + 0);
    pio_rMatrix._22 = *reinterpret_cast<const float *>(l_pKey + 4);
    pio_rMatrix._33 = *reinterpret_cast<const float *>(l_pKey + 8);
    ApplyObjectExtraScale(pio_rMatrix, pi_pMesh);
    return;
  }

  const int l_nIndex = FindKeyIndex(pi_pAnimationData->m_pScaleKeys,
                                    pi_pAnimationData->m_wScaleCount,
                                    pi_dwFrame,
                                    16,
                                    12);
  if (l_nIndex < 0)
  {
    ApplyObjectExtraScale(pio_rMatrix, pi_pMesh);
    return;
  }

  const BYTE *l_pKey0 = pi_pAnimationData->m_pScaleKeys + (l_nIndex * 16);
  const BYTE *l_pKey1 = l_pKey0 + 16;
  const float l_fRatio = KeyRatio(l_pKey0, pi_dwFrame, 16, 12);
  pio_rMatrix._11 =
    *reinterpret_cast<const float *>(l_pKey0 + 0) +
    (*reinterpret_cast<const float *>(l_pKey1 + 0) -
     *reinterpret_cast<const float *>(l_pKey0 + 0)) * l_fRatio;
  pio_rMatrix._22 =
    *reinterpret_cast<const float *>(l_pKey0 + 4) +
    (*reinterpret_cast<const float *>(l_pKey1 + 4) -
     *reinterpret_cast<const float *>(l_pKey0 + 4)) * l_fRatio;
  pio_rMatrix._33 =
    *reinterpret_cast<const float *>(l_pKey0 + 8) +
    (*reinterpret_cast<const float *>(l_pKey1 + 8) -
     *reinterpret_cast<const float *>(l_pKey0 + 8)) * l_fRatio;
  ApplyObjectExtraScale(pio_rMatrix, pi_pMesh);
}

void ApplyRotationTrack(D3DXMATRIX &pio_rMatrix,
                        const AnimationDataCompat *pi_pAnimationData,
                        DWORD pi_dwFrame,
                        ObjectMesh *pi_pMesh)
{
  if (!pi_pAnimationData || !pi_pAnimationData->m_wRotCount)
  {
    D3DXMATRIX l_matRot;
    ObjectMesh *l_pParent =
      CharacterMR::Field<ObjectMesh *>(pi_pMesh, CharacterMR::OBJ_PARENT);
    if (CharacterMR::Field<BYTE>(pi_pMesh, CharacterMR::OBJ_HAS_PARENT) &&
        l_pParent)
    {
      D3DXMATRIX l_matParentBindInverse;
      D3DXMatrixInverse(&l_matParentBindInverse,
                        NULL,
                        &CharacterMR::Field<D3DXMATRIX>(l_pParent, 0x2B0));
      MultiplyMatrix(&l_matRot,
                     &CharacterMR::Field<D3DXMATRIX>(pi_pMesh, 0x2B0),
                     &l_matParentBindInverse);
    }
    else
    {
      l_matRot = CharacterMR::Field<D3DXMATRIX>(pi_pMesh, 0x2B0);
    }
    l_matRot._41 = 0.0f;
    l_matRot._42 = 0.0f;
    l_matRot._43 = 0.0f;
    MultiplyMatrix(&pio_rMatrix, &pio_rMatrix, &l_matRot);
    return;
  }

  D3DXQUATERNION l_qRotation;
  if (pi_pAnimationData->m_wRotCount == 1)
  {
    const BYTE *l_pKey = pi_pAnimationData->m_pRotKeys;
    l_qRotation.x = *reinterpret_cast<const float *>(l_pKey + 0);
    l_qRotation.y = *reinterpret_cast<const float *>(l_pKey + 4);
    l_qRotation.z = *reinterpret_cast<const float *>(l_pKey + 8);
    l_qRotation.w = -*reinterpret_cast<const float *>(l_pKey + 12);
  }
  else
  {
    const int l_nIndex = FindKeyIndex(pi_pAnimationData->m_pRotKeys,
                                      pi_pAnimationData->m_wRotCount,
                                      pi_dwFrame,
                                      20,
                                      16);
    if (l_nIndex < 0)
    {
      return;
    }

    const BYTE *l_pKey0 = pi_pAnimationData->m_pRotKeys + (l_nIndex * 20);
    const BYTE *l_pKey1 = l_pKey0 + 20;
    D3DXQUATERNION l_q0(*reinterpret_cast<const float *>(l_pKey0 + 0),
                        *reinterpret_cast<const float *>(l_pKey0 + 4),
                        *reinterpret_cast<const float *>(l_pKey0 + 8),
                        *reinterpret_cast<const float *>(l_pKey0 + 12));
    D3DXQUATERNION l_q1(*reinterpret_cast<const float *>(l_pKey1 + 0),
                        *reinterpret_cast<const float *>(l_pKey1 + 4),
                        *reinterpret_cast<const float *>(l_pKey1 + 8),
                        *reinterpret_cast<const float *>(l_pKey1 + 12));
    D3DXQuaternionSlerp(&l_qRotation,
                        &l_q0,
                        &l_q1,
                        KeyRatio(l_pKey0, pi_dwFrame, 20, 16));
    l_qRotation.w = -l_qRotation.w;
  }

  D3DXMATRIX l_matRotation;
  D3DXMatrixRotationQuaternion(&l_matRotation, &l_qRotation);
  MultiplyMatrix(&pio_rMatrix, &pio_rMatrix, &l_matRotation);
}

void ApplyTranslationTrack(D3DXMATRIX &pio_rMatrix,
                           const AnimationDataCompat *pi_pAnimationData,
                           DWORD pi_dwFrame,
                           ObjectMesh *pi_pMesh)
{
  if (!pi_pAnimationData || !pi_pAnimationData->m_wTransCount)
  {
    pio_rMatrix._41 = CharacterMR::Field<float>(pi_pMesh, 0x1A0);
    pio_rMatrix._42 = CharacterMR::Field<float>(pi_pMesh, 0x1A4);
    pio_rMatrix._43 = CharacterMR::Field<float>(pi_pMesh, 0x1A8);
    return;
  }

  if (pi_pAnimationData->m_wTransCount == 1)
  {
    const BYTE *l_pKey = pi_pAnimationData->m_pTransKeys;
    pio_rMatrix._41 = *reinterpret_cast<const float *>(l_pKey + 0);
    pio_rMatrix._42 = *reinterpret_cast<const float *>(l_pKey + 4);
    pio_rMatrix._43 = *reinterpret_cast<const float *>(l_pKey + 8);
    return;
  }

  const int l_nIndex = FindKeyIndex(pi_pAnimationData->m_pTransKeys,
                                    pi_pAnimationData->m_wTransCount,
                                    pi_dwFrame,
                                    16,
                                    12);
  if (l_nIndex < 0)
  {
    return;
  }

  const BYTE *l_pKey0 = pi_pAnimationData->m_pTransKeys + (l_nIndex * 16);
  const BYTE *l_pKey1 = l_pKey0 + 16;
  const float l_fRatio = KeyRatio(l_pKey0, pi_dwFrame, 16, 12);
  pio_rMatrix._41 =
    *reinterpret_cast<const float *>(l_pKey0 + 0) +
    (*reinterpret_cast<const float *>(l_pKey1 + 0) -
     *reinterpret_cast<const float *>(l_pKey0 + 0)) * l_fRatio;
  pio_rMatrix._42 =
    *reinterpret_cast<const float *>(l_pKey0 + 4) +
    (*reinterpret_cast<const float *>(l_pKey1 + 4) -
     *reinterpret_cast<const float *>(l_pKey0 + 4)) * l_fRatio;
  pio_rMatrix._43 =
    *reinterpret_cast<const float *>(l_pKey0 + 8) +
    (*reinterpret_cast<const float *>(l_pKey1 + 8) -
     *reinterpret_cast<const float *>(l_pKey0 + 8)) * l_fRatio;
}

void ApplyAlphaTrack(ObjectMesh *pio_pObjectMesh,
                     const AnimationDataCompat *pi_pAnimationData,
                     DWORD pi_dwFrame)
{
  if (!pio_pObjectMesh || !pi_pAnimationData || !pi_pAnimationData->m_wAlphaCount)
  {
    return;
  }

  if (pi_pAnimationData->m_wAlphaCount == 1)
  {
    CharacterMR::Field<float>(pio_pObjectMesh, CharacterMR::OBJ_ALPHA) =
      *reinterpret_cast<const float *>(pi_pAnimationData->m_pAlphaKeys);
    return;
  }

  const int l_nIndex = FindKeyIndex(pi_pAnimationData->m_pAlphaKeys,
                                    pi_pAnimationData->m_wAlphaCount,
                                    pi_dwFrame,
                                    8,
                                    4);
  if (l_nIndex < 0)
  {
    return;
  }

  const BYTE *l_pKey0 = pi_pAnimationData->m_pAlphaKeys + (l_nIndex * 8);
  const BYTE *l_pKey1 = l_pKey0 + 8;
  const float l_fRatio = KeyRatio(l_pKey0, pi_dwFrame, 8, 4);
  CharacterMR::Field<float>(pio_pObjectMesh, CharacterMR::OBJ_ALPHA) =
    *reinterpret_cast<const float *>(l_pKey0 + 0) +
    (*reinterpret_cast<const float *>(l_pKey1 + 0) -
     *reinterpret_cast<const float *>(l_pKey0 + 0)) * l_fRatio;
}

void SetRenderStatesForCharacter(IDirect3DDevice8 *pi_pDevice)
{
  if (!pi_pDevice)
  {
    return;
  }

  pi_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
  pi_pDevice->SetRenderState(D3DRS_AMBIENT, 0xFFFFFFFF);
  pi_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
  pi_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
  pi_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
  pi_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
  pi_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
  pi_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
  pi_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
  pi_pDevice->SetRenderState(D3DRS_ALPHAREF, 0x08);
  pi_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
  pi_pDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_1WEIGHTS);
  pi_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
  pi_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
  pi_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
  pi_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
  pi_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
  pi_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
  pi_pDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
  pi_pDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
  pi_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
  pi_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
  pi_pDevice->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
  pi_pDevice->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
  pi_pDevice->SetVertexShader(D3DFVF_CHARACTER_VERTEX);
}

void SetObjectMeshTransform(IDirect3DDevice8 *pi_pDevice,
                            ObjectMesh *pi_pObjectMesh,
                            const D3DXMATRIX &pi_rWorldMatrix)
{
  if (!pi_pDevice || !pi_pObjectMesh)
  {
    return;
  }

  const DWORD l_dwBlendCount =
    CharacterMR::Field<DWORD>(pi_pObjectMesh, CharacterMR::OBJ_BLEND_COUNT);
  D3DXMATRIX *l_pBlendMatrices =
    CharacterMR::Field<D3DXMATRIX *>(pi_pObjectMesh,
                                     CharacterMR::OBJ_BLEND_COUNT + 0x14);
  if (l_dwBlendCount && l_pBlendMatrices)
  {
    for (DWORD i = 0; i < l_dwBlendCount; ++i)
    {
      D3DXMATRIX l_matBlendWorld;
      MultiplyMatrix(&l_matBlendWorld, &l_pBlendMatrices[i], &pi_rWorldMatrix);
      pi_pDevice->SetTransform(D3DTS_WORLDMATRIX(i), &l_matBlendWorld);
    }
    return;
  }

  D3DXMATRIX l_matObjectWorld;
  MultiplyMatrix(&l_matObjectWorld,
                 &CharacterMR::Field<D3DXMATRIX>(pi_pObjectMesh,
                                                 CharacterMR::OBJ_RESULT_MATRIX),
                 &pi_rWorldMatrix);
  pi_pDevice->SetTransform(D3DTS_WORLD, &l_matObjectWorld);
}

void DrawObjectMesh(IDirect3DDevice8 *pi_pDevice,
                    CHARACTEROBJECT *pi_pCharacterObject,
                    ObjectMesh *pi_pObjectMesh,
                    const D3DXMATRIX &pi_rWorldMatrix)
{
  if (!pi_pDevice || !pi_pCharacterObject || !pi_pObjectMesh ||
      CharacterMR::Field<DWORD>(pi_pObjectMesh, CharacterMR::OBJ_HIDE))
  {
    return;
  }

  IDirect3DVertexBuffer8 *l_pVertexBuffer =
    CharacterMR::Field<IDirect3DVertexBuffer8 *>(pi_pObjectMesh,
                                                CharacterMR::OBJ_VERTEX_BUFFER);
  const DWORD l_dwVertexCount =
    CharacterMR::Field<DWORD>(pi_pObjectMesh, CharacterMR::OBJ_VERTEX_COUNT);
  if (!l_pVertexBuffer || l_dwVertexCount < 3)
  {
    return;
  }

  D3DMATERIAL8 l_stMaterial =
    CharacterMR::Field<D3DMATERIAL8>(pi_pCharacterObject, CharacterMR::CHOBJ_MATERIAL);
  const float l_fMeshAlpha =
    CharacterMR::Field<float>(pi_pObjectMesh, CharacterMR::OBJ_ALPHA);
  l_stMaterial.Diffuse.a *= l_fMeshAlpha;
  l_stMaterial.Ambient.a *= l_fMeshAlpha;

  pi_pDevice->SetMaterial(&l_stMaterial);
  pi_pDevice->SetTexture(0,
                         CharacterMR::Field<IDirect3DTexture8 *>(pi_pObjectMesh,
                                                                  CharacterMR::OBJ_TEXTURE0));
  pi_pDevice->SetRenderState(D3DRS_CULLMODE,
                             CharacterMR::Field<BYTE>(pi_pObjectMesh, 0x3BC)
                               ? D3DCULL_NONE
                               : D3DCULL_CW);
  SetObjectMeshTransform(pi_pDevice, pi_pObjectMesh, pi_rWorldMatrix);
  pi_pDevice->SetStreamSource(0, l_pVertexBuffer, sizeof(CharacterVertex));
  pi_pDevice->SetVertexShader(D3DFVF_CHARACTER_VERTEX);
  pi_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, l_dwVertexCount / 3);
}

void DeleteTextureRecord(ChTextureCompat *pio_pTexture)
{
  if (!pio_pTexture)
  {
    return;
  }

  if (pio_pTexture->m_pTexture)
  {
    pio_pTexture->m_pTexture->Release();
    pio_pTexture->m_pTexture = NULL;
  }
  ::operator delete(pio_pTexture);
}
}

ObjectData *MakeMeshData(char *pi_pBuffer)
{
  if (!pi_pBuffer)
  {
    return NULL;
  }

  const BYTE *l_pBuffer = reinterpret_cast<const BYTE *>(pi_pBuffer);
  const WORD l_wObjectCount = *reinterpret_cast<const WORD *>(l_pBuffer);
  ObjectData *l_pObjectData = static_cast<ObjectData *>(::operator new(sizeof(ObjectData)));
  ZeroMemory(l_pObjectData, sizeof(ObjectData));
  CharacterMR::Field<WORD>(l_pObjectData, kObjectDataMeshCountOffset) = l_wObjectCount;

  if (l_wObjectCount == 0)
  {
    return l_pObjectData;
  }

  MeshData *l_pMeshData = new MeshData[l_wObjectCount];
  CharacterMR::Field<MeshData *>(l_pObjectData, kObjectDataMeshDataOffset) = l_pMeshData;

  size_t l_stOffset = sizeof(WORD);
  for (WORD i = 0; i < l_wObjectCount; ++i)
  {
    MeshData *l_pMesh = &l_pMeshData[i];
    const BYTE *l_pHeader = l_pBuffer + l_stOffset;
    memcpy(l_pMesh, l_pHeader, kMeshDataHeaderSize);
    l_stOffset += kMeshDataHeaderSize;

    const WORD l_wVertexCount = *reinterpret_cast<const WORD *>(l_pHeader + 392);
    const WORD l_wFaceCount = *reinterpret_cast<const WORD *>(l_pHeader + 394);
    const WORD l_wPhysiqueCount = *reinterpret_cast<const WORD *>(l_pHeader + 396);
    const DWORD l_dwExpandedAux = *reinterpret_cast<const DWORD *>(l_pHeader + 642);
    CharacterMR::Field<DWORD>(l_pObjectData, kObjectDataExpandedAuxOffset) = l_dwExpandedAux;

    const size_t l_stVertexBytes = static_cast<size_t>(l_wVertexCount) * kMeshVertexBytes;
    CharacterMR::Field<void *>(l_pMesh, kMeshDataVertexOffset) =
      CopyMeshDataBlock(l_pBuffer + l_stOffset, l_stVertexBytes);
    l_stOffset += l_stVertexBytes;

    const size_t l_stFaceBytes = static_cast<size_t>(l_wFaceCount) * kMeshFaceBytes;
    CharacterMR::Field<void *>(l_pMesh, kMeshDataFaceOffset) =
      CopyMeshDataBlock(l_pBuffer + l_stOffset, l_stFaceBytes);
    l_stOffset += l_stFaceBytes;

    if (l_dwExpandedAux)
    {
      const DWORD l_dwNameCount =
        *reinterpret_cast<const DWORD *>(l_pBuffer + l_stOffset);
      l_stOffset += sizeof(DWORD);
      CharacterMR::Field<DWORD>(l_pMesh, kMeshDataPhysiqueNameCountOffset) =
        l_dwNameCount;

      const size_t l_stNameBytes =
        static_cast<size_t>(l_dwNameCount) * kMeshPhysiqueNameBytes;
      BYTE *l_pNames =
        static_cast<BYTE *>(CopyMeshDataBlock(l_pBuffer + l_stOffset, l_stNameBytes));
      CharacterMR::Field<void *>(l_pMesh, kMeshDataPhysiqueNamesOffset) = l_pNames;
      l_stOffset += l_stNameBytes;

      const size_t l_stPackedPhysiqueBytes =
        static_cast<size_t>(l_wPhysiqueCount) * kMeshPackedPhysiqueBytes;
      BYTE *l_pPackedRecords =
        static_cast<BYTE *>(CopyMeshDataBlock(l_pBuffer + l_stOffset,
                                              l_stPackedPhysiqueBytes));
      CharacterMR::Field<void *>(l_pMesh, kMeshDataPackedPhysiqueOffset) =
        l_pPackedRecords;
      l_stOffset += l_stPackedPhysiqueBytes;

      CharacterMR::Field<void *>(l_pMesh, kMeshDataPhysiqueOffset) =
        BuildLegacyPhysiqueRecords(l_pNames,
                                   l_dwNameCount,
                                   l_pPackedRecords,
                                   l_wPhysiqueCount);
    }
    else if (l_wPhysiqueCount)
    {
      const size_t l_stPhysiqueBytes =
        static_cast<size_t>(l_wPhysiqueCount) * kMeshPhysiqueBytes;
      CharacterMR::Field<void *>(l_pMesh, kMeshDataPhysiqueOffset) =
        CopyMeshDataBlock(l_pBuffer + l_stOffset, l_stPhysiqueBytes);
      l_stOffset += l_stPhysiqueBytes;
    }
  }

  return l_pObjectData;
}

ObjectData *ImportMeshData(char *pi_pFileName)
{
  if (!pi_pFileName || !pi_pFileName[0])
  {
    return NULL;
  }

  FILE *l_pFile = NULL;
  if (fopen_s(&l_pFile, pi_pFileName, "rb") == 0 && l_pFile)
  {
    fseek(l_pFile, 0, SEEK_END);
    const long l_nFileSize = ftell(l_pFile);
    fseek(l_pFile, 0, SEEK_SET);
    if (l_nFileSize <= 0)
    {
      fclose(l_pFile);
      return NULL;
    }

    char *l_pBuffer = static_cast<char *>(::operator new(static_cast<size_t>(l_nFileSize)));
    const size_t l_stRead = fread(l_pBuffer, 1, static_cast<size_t>(l_nFileSize), l_pFile);
    fclose(l_pFile);
    if (l_stRead != static_cast<size_t>(l_nFileSize))
    {
      ::operator delete(l_pBuffer);
      return NULL;
    }

    ObjectData *l_pObjectData = MakeMeshData(l_pBuffer);
    ::operator delete(l_pBuffer);
    return l_pObjectData;
  }

  char *l_pRFSBuffer = ReadRFS(pi_pFileName);
  if (!l_pRFSBuffer)
  {
    return NULL;
  }

  ObjectData *l_pObjectData = MakeMeshData(l_pRFSBuffer);
  ::operator delete(l_pRFSBuffer);
  return l_pObjectData;
}

void InitVertex(ObjectData *pi_pObjectData,
                CHARACTEROBJECT *pi_pParentBone,
                CHARACTEROBJECT *pio_pCharacterObject,
                TextureManager *pi_pTextureManager)
{
  if (!pi_pObjectData || !pio_pCharacterObject)
  {
    return;
  }

  CharacterMR::Field<CHARACTEROBJECT *>(pio_pCharacterObject,
                                        CharacterMR::CHOBJ_PARENT_BONE) =
    pi_pParentBone;

  std::vector<MeshRecord> l_vecRecords;
  const bool l_bRebuildParentLocal =
    CharacterMR::Field<DWORD>(pi_pObjectData,
                              kObjectDataExpandedAuxOffset) == 0;
  if (!CollectMeshDataRecords(pi_pObjectData, l_vecRecords) ||
      !BuildObjectMeshes(NULL,
                         pio_pCharacterObject,
                         pi_pTextureManager,
                         l_vecRecords,
                         l_bRebuildParentLocal))
  {
    CharacterMR::Field<DWORD>(pio_pCharacterObject, CharacterMR::CHOBJ_ERROR) = 1;
  }
}

void InitVertex(ObjectData *pi_pObjectData,
                CHARACTEROBJECT *pi_pParentBone,
                CHARACTEROBJECT *pio_pCharacterObject,
                TextureManager *pi_pTextureManager,
                bool pi_bUseImmediateFile)
{
  UNREFERENCED_PARAMETER(pi_bUseImmediateFile);
  InitVertex(pi_pObjectData, pi_pParentBone, pio_pCharacterObject, pi_pTextureManager);
}

CRender *CRender::m_Render = NULL;
AnimationManager *AnimationManager::m_AManager = NULL;
TextureManager *TextureManager::m_TManager = NULL;
AlphaMeshManager g_AMeshManager;
extern std::map<std::string, RFSINFO> g_RFSMap;

namespace CharacterMR
{
void InitMaterial(D3DMATERIAL8 *po_pMaterial,
                  float pi_fRed,
                  float pi_fGreen,
                  float pi_fBlue,
                  float pi_fAlpha)
{
  if (!po_pMaterial)
  {
    return;
  }

  ZeroMemory(po_pMaterial, sizeof(*po_pMaterial));
  po_pMaterial->Diffuse.r = pi_fRed;
  po_pMaterial->Diffuse.g = pi_fGreen;
  po_pMaterial->Diffuse.b = pi_fBlue;
  po_pMaterial->Diffuse.a = pi_fAlpha;
  po_pMaterial->Ambient = po_pMaterial->Diffuse;
}

void InitObjectMesh(ObjectMesh *pio_pObjectMesh)
{
  if (!pio_pObjectMesh)
  {
    return;
  }

  ZeroMemory(pio_pObjectMesh, sizeof(ObjectMesh));
  Field<float>(pio_pObjectMesh, OBJ_ALPHA) = 1.0f;
  Field<float>(pio_pObjectMesh, 0x338) = 1.0f;
  Field<float>(pio_pObjectMesh, 0x33C) = 1.0f;
  Field<float>(pio_pObjectMesh, 0x340) = 1.0f;
  Field<float>(pio_pObjectMesh, 0x344) = 1.0f;
  Field<float>(pio_pObjectMesh, OBJ_EXTRA_SCALE + 0) = 1.0f;
  Field<float>(pio_pObjectMesh, OBJ_EXTRA_SCALE + 4) = 1.0f;
  Field<float>(pio_pObjectMesh, OBJ_EXTRA_SCALE + 8) = 1.0f;
  Field<DWORD>(pio_pObjectMesh, OBJ_TEXTURE0_ID) = 0xFFFFFFFF;
  Field<DWORD>(pio_pObjectMesh, OBJ_TEXTURE1_ID) = 0xFFFFFFFF;
  Field<DWORD>(pio_pObjectMesh, OBJ_TEXTURE2_ID) = 0xFFFFFFFF;
  D3DXMatrixIdentity(&Field<D3DXMATRIX>(pio_pObjectMesh, OBJ_BASE_MATRIX));
  D3DXMatrixIdentity(&Field<D3DXMATRIX>(pio_pObjectMesh, OBJ_BIND_MATRIX));
  D3DXMatrixIdentity(&Field<D3DXMATRIX>(pio_pObjectMesh, OBJ_INV_BIND_MATRIX));
  D3DXMatrixIdentity(&Field<D3DXMATRIX>(pio_pObjectMesh, OBJ_LOCAL_MATRIX));
  D3DXMatrixIdentity(&Field<D3DXMATRIX>(pio_pObjectMesh, OBJ_SOURCE_MATRIX));
  D3DXMatrixIdentity(&Field<D3DXMATRIX>(pio_pObjectMesh, OBJ_RESULT_MATRIX));
  D3DXMatrixIdentity(&Field<D3DXMATRIX>(pio_pObjectMesh, OBJ_PREV_RESULT_MATRIX));
}

void ReleaseObjectMesh(ObjectMesh *pio_pObjectMesh)
{
  if (!pio_pObjectMesh)
  {
    return;
  }

  IDirect3DVertexBuffer8 *l_pVertexBuffer =
    Field<IDirect3DVertexBuffer8 *>(pio_pObjectMesh, OBJ_VERTEX_BUFFER);
  if (l_pVertexBuffer)
  {
    l_pVertexBuffer->Release();
    Field<IDirect3DVertexBuffer8 *>(pio_pObjectMesh, OBJ_VERTEX_BUFFER) = NULL;
  }

  D3DXMATRIX *l_pBlendMatrices =
    Field<D3DXMATRIX *>(pio_pObjectMesh, OBJ_BLEND_COUNT + 0x14);
  if (l_pBlendMatrices)
  {
    ::operator delete(l_pBlendMatrices);
    Field<D3DXMATRIX *>(pio_pObjectMesh, OBJ_BLEND_COUNT + 0x14) = NULL;
  }

  ObjectMesh **l_ppBlendParents =
    Field<ObjectMesh **>(pio_pObjectMesh, OBJ_BLEND_PARENT);
  if (l_ppBlendParents)
  {
    ::operator delete(l_ppBlendParents);
    Field<ObjectMesh **>(pio_pObjectMesh, OBJ_BLEND_PARENT) = NULL;
  }
  Field<DWORD>(pio_pObjectMesh, OBJ_BLEND_COUNT) = 0;

  FreeAnimationData(pio_pObjectMesh);
}

void ReleaseCharacterObject(CHARACTEROBJECT *pio_pCharacterObject)
{
  if (!pio_pCharacterObject)
  {
    return;
  }

  ObjectMesh *l_pMesh = Field<ObjectMesh *>(pio_pCharacterObject, CHOBJ_MESH);
  DWORD l_dwObjectCount = Field<DWORD>(pio_pCharacterObject, CHOBJ_MAX_OBJ);
  for (DWORD i = 0; l_pMesh && i < l_dwObjectCount; ++i)
  {
    ReleaseObjectMesh(&l_pMesh[i]);
  }
  ::operator delete(l_pMesh);
  Field<ObjectMesh *>(pio_pCharacterObject, CHOBJ_MESH) = NULL;
  Field<DWORD>(pio_pCharacterObject, CHOBJ_MAX_OBJ) = 0;
}

CHARACTEROBJECT *CreateCharacterObject(const char *pi_pFileName,
                                       CHARACTEROBJECT *pi_pParentBone)
{
  CHARACTEROBJECT *l_pCharacterObject =
    static_cast<CHARACTEROBJECT *>(::operator new(sizeof(CHARACTEROBJECT)));
  ZeroMemory(l_pCharacterObject, sizeof(CHARACTEROBJECT));
  CopyStringSafe(l_pCharacterObject->m_szFileName,
                 sizeof(l_pCharacterObject->m_szFileName),
                 pi_pFileName ? pi_pFileName : "");
  Field<CHARACTEROBJECT *>(l_pCharacterObject, CHOBJ_PARENT_BONE) = pi_pParentBone;
  Field<DWORD>(l_pCharacterObject, CHOBJ_FRAME) = 160;
  Field<DWORD>(l_pCharacterObject, CHOBJ_TRANS_TIME) = 3000;
  Field<float>(l_pCharacterObject, CHOBJ_ALPHA) = 1.0f;
  InitMaterial(&Field<D3DMATERIAL8>(l_pCharacterObject, CHOBJ_MATERIAL),
               0.55f,
               0.55f,
               0.55f,
               1.0f);
  return l_pCharacterObject;
}

bool LoadBinaryFile(const char *pi_pFileName, std::vector<BYTE> &po_vecBuffer)
{
  po_vecBuffer.clear();
  if (!pi_pFileName || !pi_pFileName[0])
  {
    return false;
  }

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
  const size_t l_stRead = fread(&po_vecBuffer[0], 1, po_vecBuffer.size(), l_pFile);
  fclose(l_pFile);
  return l_stRead == po_vecBuffer.size();
}

char *LoadBinaryFileAlloc(const char *pi_pFileName)
{
  std::vector<BYTE> l_vecBuffer;
  if (!LoadBinaryFile(pi_pFileName, l_vecBuffer))
  {
    return NULL;
  }

  char *l_pBuffer = static_cast<char *>(::operator new(l_vecBuffer.size()));
  memcpy(l_pBuffer, &l_vecBuffer[0], l_vecBuffer.size());
  return l_pBuffer;
}

void CopyBaseFileNameUpper(const char *pi_pPath,
                           char *po_szFileName,
                           size_t pi_stFileNameSize)
{
  if (!po_szFileName || pi_stFileNameSize == 0)
  {
    return;
  }

  po_szFileName[0] = '\0';
  if (!pi_pPath || !pi_pPath[0])
  {
    return;
  }

  const char *l_pFileName = pi_pPath;
  const char *l_pSlash = strrchr(pi_pPath, '\\');
  const char *l_pSlash2 = strrchr(pi_pPath, '/');
  if (l_pSlash2 && (!l_pSlash || l_pSlash2 > l_pSlash))
  {
    l_pSlash = l_pSlash2;
  }
  if (l_pSlash && l_pSlash[1])
  {
    l_pFileName = l_pSlash + 1;
  }

  CopyFixedString(po_szFileName, pi_stFileNameSize, l_pFileName, 260);
  UppercaseInPlace(po_szFileName);
}
}

namespace
{
std::string MakeRFSKey(const char *pi_pFileName)
{
  char l_szKey[MAX_PATH] = {};
  if (pi_pFileName)
  {
    strncpy_s(l_szKey, sizeof(l_szKey), pi_pFileName, _TRUNCATE);
  }
  UppercaseInPlace(l_szKey);
  return l_szKey;
}

void EnsureTrailingSlash(char *pio_pPath, size_t pi_stPathSize)
{
  if (!pio_pPath || pi_stPathSize == 0 || !pio_pPath[0])
  {
    return;
  }

  const size_t l_stLen = strlen(pio_pPath);
  if (l_stLen + 1 >= pi_stPathSize)
  {
    return;
  }
  if (pio_pPath[l_stLen - 1] != '\\' && pio_pPath[l_stLen - 1] != '/')
  {
    pio_pPath[l_stLen] = '\\';
    pio_pPath[l_stLen + 1] = '\0';
  }
}

void AddRFSRecord(const char *pi_pRoot,
                  const char *pi_pRfsFileName,
                  const char *pi_pEntryName,
                  DWORD pi_dwOffset,
                  DWORD pi_dwSize)
{
  if (!pi_pRoot || !pi_pRfsFileName || !pi_pEntryName || pi_dwSize == 0)
  {
    return;
  }

  char l_szKey[MAX_PATH] = {};
  char l_szRfsPath[MAX_PATH] = {};
  _snprintf_s(l_szKey, sizeof(l_szKey), _TRUNCATE, "%s%s", pi_pRoot, pi_pEntryName);
  _snprintf_s(l_szRfsPath, sizeof(l_szRfsPath), _TRUNCATE, "%s%s", pi_pRoot, pi_pRfsFileName);
  UppercaseInPlace(l_szKey);

  RFSINFO &l_rInfo = g_RFSMap[l_szKey];
  l_rInfo.m_sRfsFileName = l_szRfsPath;
  l_rInfo.m_dwOffset = pi_dwOffset;
  l_rInfo.m_dwSize = pi_dwSize;
}

void ReadRFSIndexFile(const char *pi_pRoot, const char *pi_pRfsFileName)
{
  char l_szRfsPath[MAX_PATH] = {};
  _snprintf_s(l_szRfsPath, sizeof(l_szRfsPath), _TRUNCATE, "%s%s", pi_pRoot, pi_pRfsFileName);

  FILE *l_pRfs = NULL;
  if (fopen_s(&l_pRfs, l_szRfsPath, "rb") != 0 || !l_pRfs)
  {
    return;
  }

  DWORD l_dwCount = 0;
  fread(&l_dwCount, sizeof(l_dwCount), 1, l_pRfs);
  for (DWORD i = 0; i < l_dwCount; ++i)
  {
    char l_szEntryName[0x38] = {};
    DWORD l_dwOffset = 0;
    DWORD l_dwSize = 0;
    if (fread(l_szEntryName, 1, sizeof(l_szEntryName), l_pRfs) != sizeof(l_szEntryName) ||
        fread(&l_dwOffset, sizeof(l_dwOffset), 1, l_pRfs) != 1 ||
        fread(&l_dwSize, sizeof(l_dwSize), 1, l_pRfs) != 1)
    {
      break;
    }
    AddRFSRecord(pi_pRoot, pi_pRfsFileName, l_szEntryName, l_dwOffset, l_dwSize);
  }
  fclose(l_pRfs);
}

void MakeFullRFSPath(char *po_pPath, size_t pi_stPathSize, const char *pi_pFileName)
{
  if (!po_pPath || pi_stPathSize == 0)
  {
    return;
  }

  po_pPath[0] = '\0';
  if (!pi_pFileName || !pi_pFileName[0])
  {
    return;
  }

  char l_szFileName[MAX_PATH] = {};
  strncpy_s(l_szFileName, sizeof(l_szFileName), pi_pFileName, _TRUNCATE);
  for (char *p = l_szFileName; *p; ++p)
  {
    if (*p == '/')
    {
      *p = '\\';
    }
  }

  if (strchr(l_szFileName, ':') || l_szFileName[0] == '\\')
  {
    strncpy_s(po_pPath, pi_stPathSize, l_szFileName, _TRUNCATE);
  }
  else
  {
    char l_szCwd[MAX_PATH] = {};
    _getcwd(l_szCwd, sizeof(l_szCwd));
    _snprintf_s(po_pPath, pi_stPathSize, _TRUNCATE, "%s\\%s", l_szCwd, l_szFileName);
  }
  UppercaseInPlace(po_pPath);
}
}

void ReadRFSHeader(char *pi_pPath)
{
  if (!pi_pPath || !pi_pPath[0])
  {
    return;
  }

  char l_szRoot[MAX_PATH] = {};
  strncpy_s(l_szRoot, sizeof(l_szRoot), pi_pPath, _TRUNCATE);
  EnsureTrailingSlash(l_szRoot, sizeof(l_szRoot));

  char l_szInfoPath[MAX_PATH] = {};
  _snprintf_s(l_szInfoPath, sizeof(l_szInfoPath), _TRUNCATE, "%sRFSINFO.DAT", l_szRoot);

  FILE *l_pInfo = NULL;
  if (fopen_s(&l_pInfo, l_szInfoPath, "rb") == 0 && l_pInfo)
  {
    char l_szLine[256] = {};
    char l_szRfsName[256] = {};
    while (fgets(l_szLine, sizeof(l_szLine), l_pInfo))
    {
      if (sscanf_s(l_szLine, "%255s", l_szRfsName, static_cast<unsigned>(sizeof(l_szRfsName))) == 1)
      {
        ReadRFSIndexFile(l_szRoot, l_szRfsName);
      }
    }
    fclose(l_pInfo);
  }

  for (int i = 0; i < 1024; ++i)
  {
    char l_szRfsName[32] = {};
    _snprintf_s(l_szRfsName, sizeof(l_szRfsName), _TRUNCATE, "DATA%d.RFS", i);
    char l_szRfsPath[MAX_PATH] = {};
    _snprintf_s(l_szRfsPath, sizeof(l_szRfsPath), _TRUNCATE, "%s%s", l_szRoot, l_szRfsName);
    FILE *l_pProbe = NULL;
    if (fopen_s(&l_pProbe, l_szRfsPath, "rb") != 0 || !l_pProbe)
    {
      break;
    }
    fclose(l_pProbe);
    ReadRFSIndexFile(l_szRoot, l_szRfsName);
  }
}

void LoadFile(char *pi_pFileName)
{
  UNREFERENCED_PARAMETER(pi_pFileName);
}

char *GetFileFromRFS(char *pi_pFileName)
{
  std::map<std::string, RFSINFO>::iterator it = g_RFSMap.find(MakeRFSKey(pi_pFileName));
  if (it == g_RFSMap.end())
  {
    return NULL;
  }

  FILE *l_pRfs = NULL;
  if (fopen_s(&l_pRfs, it->second.m_sRfsFileName.c_str(), "rb") != 0 || !l_pRfs)
  {
    return NULL;
  }

  char *l_pData = static_cast<char *>(::operator new(it->second.m_dwSize));
  fseek(l_pRfs, static_cast<long>(it->second.m_dwOffset), SEEK_SET);
  const size_t l_stRead = fread(l_pData, 1, it->second.m_dwSize, l_pRfs);
  fclose(l_pRfs);
  if (l_stRead != it->second.m_dwSize)
  {
    ::operator delete(l_pData);
    return NULL;
  }
  return l_pData;
}

char *ReadRFS(char *pi_pFileName)
{
  char l_szFullPath[MAX_PATH] = {};
  MakeFullRFSPath(l_szFullPath, sizeof(l_szFullPath), pi_pFileName);
  if (!l_szFullPath[0])
  {
    return NULL;
  }

  char *l_pData = GetFileFromRFS(l_szFullPath);
  if (l_pData)
  {
    return l_pData;
  }

  char l_szRoot[MAX_PATH] = {};
  strncpy_s(l_szRoot, sizeof(l_szRoot), l_szFullPath, _TRUNCATE);
  char *l_pSlash = strrchr(l_szRoot, '\\');
  if (l_pSlash)
  {
    l_pSlash[1] = '\0';
    ReadRFSHeader(l_szRoot);
  }
  return GetFileFromRFS(l_szFullPath);
}

char *GetFileImme(char *pi_pFileName)
{
  char *l_pExt = GetExtention(pi_pFileName);
  if (!l_pExt)
  {
    return NULL;
  }

  char l_szExt[16] = {};
  strncpy_s(l_szExt, sizeof(l_szExt), l_pExt, _TRUNCATE);
  UppercaseInPlace(l_szExt);
  if (!strcmp(l_szExt, "ANI"))
  {
    return reinterpret_cast<char *>(ReadAnimationData(pi_pFileName));
  }
  if (!strcmp(l_szExt, "MSH") || !strcmp(l_szExt, "BN"))
  {
    return reinterpret_cast<char *>(ImportMeshData(pi_pFileName));
  }
  return NULL;
}

char *GetFile(char *pi_pFileName)
{
  return GetFileImme(pi_pFileName);
}

void MultMatrixAsm(float *po_pOut, const float *pi_pA, const float *pi_pB)
{
  if (!po_pOut || !pi_pA || !pi_pB)
  {
    return;
  }

  D3DXMatrixMultiply(reinterpret_cast<D3DXMATRIX *>(po_pOut),
                     reinterpret_cast<const D3DXMATRIX *>(pi_pA),
                     reinterpret_cast<const D3DXMATRIX *>(pi_pB));
}

void SetScale(D3DXMATRIX *pio_pMatrix, float *pi_pfScale)
{
  if (!pio_pMatrix || !pi_pfScale)
  {
    return;
  }

  D3DXMATRIX l_matScale;
  D3DXMatrixScaling(&l_matScale, pi_pfScale[0], pi_pfScale[1], pi_pfScale[2]);
  D3DXMatrixMultiply(pio_pMatrix, pio_pMatrix, &l_matScale);
}

int IsBBoxInFrustum(float *pi_pfMin, float *pi_pfMax, float (*const pi_pfPlane)[4])
{
  if (!pi_pfMin || !pi_pfMax || !pi_pfPlane)
  {
    return 0;
  }

  for (int i = 0; i < 6; ++i)
  {
    const float *l_pPlane = pi_pfPlane[i];
    const float l_fX = l_pPlane[0] < 0.0f ? pi_pfMax[0] : pi_pfMin[0];
    const float l_fY = l_pPlane[1] < 0.0f ? pi_pfMax[1] : pi_pfMin[1];
    const float l_fZ = l_pPlane[2] < 0.0f ? pi_pfMax[2] : pi_pfMin[2];
    if ((l_fX * l_pPlane[0]) +
        (l_fY * l_pPlane[1]) +
        (l_fZ * l_pPlane[2]) +
        l_pPlane[3] > 0.0f)
    {
      return 0;
    }
  }
  return 1;
}

void SetFustumNormalPlane(float (*pi_pfPlane)[4], D3DXMATRIX *pi_pMatrix)
{
  if (!pi_pfPlane || !pi_pMatrix)
  {
    return;
  }

  pi_pfPlane[0][0] = -(pi_pMatrix->_11 + pi_pMatrix->_14);
  pi_pfPlane[0][1] = -(pi_pMatrix->_21 + pi_pMatrix->_24);
  pi_pfPlane[0][2] = -(pi_pMatrix->_31 + pi_pMatrix->_34);
  pi_pfPlane[0][3] = -(pi_pMatrix->_44 + pi_pMatrix->_41);

  pi_pfPlane[1][0] = -(pi_pMatrix->_14 - pi_pMatrix->_11);
  pi_pfPlane[1][1] = -(pi_pMatrix->_24 - pi_pMatrix->_21);
  pi_pfPlane[1][2] = -(pi_pMatrix->_34 - pi_pMatrix->_31);
  pi_pfPlane[1][3] = -(pi_pMatrix->_44 - pi_pMatrix->_41);

  pi_pfPlane[2][0] = -(pi_pMatrix->_14 - pi_pMatrix->_12);
  pi_pfPlane[2][1] = -(pi_pMatrix->_24 - pi_pMatrix->_22);
  pi_pfPlane[2][2] = -(pi_pMatrix->_34 - pi_pMatrix->_32);
  pi_pfPlane[2][3] = -(pi_pMatrix->_44 - pi_pMatrix->_42);

  pi_pfPlane[3][0] = -(pi_pMatrix->_12 + pi_pMatrix->_14);
  pi_pfPlane[3][1] = -(pi_pMatrix->_22 + pi_pMatrix->_24);
  pi_pfPlane[3][2] = -(pi_pMatrix->_32 + pi_pMatrix->_34);
  pi_pfPlane[3][3] = -(pi_pMatrix->_44 + pi_pMatrix->_42);

  pi_pfPlane[4][0] = -(pi_pMatrix->_13 + pi_pMatrix->_14);
  pi_pfPlane[4][1] = -(pi_pMatrix->_23 + pi_pMatrix->_24);
  pi_pfPlane[4][2] = -(pi_pMatrix->_33 + pi_pMatrix->_34);
  pi_pfPlane[4][3] = -(pi_pMatrix->_44 + pi_pMatrix->_43);

  pi_pfPlane[5][0] = -(pi_pMatrix->_14 - pi_pMatrix->_13);
  pi_pfPlane[5][1] = -(pi_pMatrix->_24 - pi_pMatrix->_23);
  pi_pfPlane[5][2] = -(pi_pMatrix->_34 - pi_pMatrix->_33);
  pi_pfPlane[5][3] = -(pi_pMatrix->_44 - pi_pMatrix->_43);

  for (int i = 0; i < 6; ++i)
  {
    const float l_fLength = sqrtf((pi_pfPlane[i][0] * pi_pfPlane[i][0]) +
                                  (pi_pfPlane[i][1] * pi_pfPlane[i][1]) +
                                  (pi_pfPlane[i][2] * pi_pfPlane[i][2]));
    if (l_fLength <= 0.000001f)
    {
      continue;
    }

    const float l_fInvLength = 1.0f / l_fLength;
    pi_pfPlane[i][0] *= l_fInvLength;
    pi_pfPlane[i][1] *= l_fInvLength;
    pi_pfPlane[i][2] *= l_fInvLength;
    pi_pfPlane[i][3] *= l_fInvLength;
  }
}

ChAnimation::ChAnimation(void)
{
  ZeroMemory(m_data, sizeof(m_data));
}

ChAnimation::~ChAnimation(void)
{
  BYTE *l_pAnimationBytes = CharacterMR::Field<BYTE *>(this, 0x108);
  if (l_pAnimationBytes)
  {
    ::operator delete(l_pAnimationBytes);
    CharacterMR::Field<BYTE *>(this, 0x108) = NULL;
  }
}

bool ChAnimation::LoadRealAnimation(bool pi_bUseImmediateFile)
{
  if (CharacterMR::Field<DWORD>(this, 0) != 0)
  {
    return true;
  }

  UNREFERENCED_PARAMETER(pi_bUseImmediateFile);
  BYTE *l_pAnimationBytes =
    reinterpret_cast<BYTE *>(CharacterMR::LoadBinaryFileAlloc(reinterpret_cast<char *>(this) + 4));
  if (!l_pAnimationBytes)
  {
    return false;
  }

  CharacterMR::Field<BYTE *>(this, 0x108) = l_pAnimationBytes;
  CharacterMR::Field<DWORD>(this, 0x110) = FindMaxAnimationFrame(l_pAnimationBytes);
  CharacterMR::Field<DWORD>(this, 0) = 1;
  return true;
}

AnimationManager::~AnimationManager(void)
{
  PtrVector l_stVector = GetVector(this,
                                   kVectorBeginOffsetAnimationManager,
                                   kVectorEndOffsetAnimationManager,
                                   kVectorCapacityOffsetAnimationManager);
  for (void **it = l_stVector.m_pBegin; it && it != l_stVector.m_pEnd; ++it)
  {
    ChAnimation *l_pAnimation = static_cast<ChAnimation *>(*it);
    delete l_pAnimation;
  }
  ClearPointerVectorStorage(this,
                            kVectorBeginOffsetAnimationManager,
                            kVectorEndOffsetAnimationManager,
                            kVectorCapacityOffsetAnimationManager);
}

int AnimationManager::Init(void)
{
  CharacterMR::Field<DWORD>(this, 0x10) = 0;
  CharacterMR::Field<DWORD>(this, kAnimationManagerNextIDOffset) = 1;
  return 0;
}

unsigned long AnimationManager::GetAnimationID(char *pi_pAniName)
{
  ChAnimation *l_pAnimation = GetAnimationData(pi_pAniName);
  return l_pAnimation ? CharacterMR::Field<DWORD>(l_pAnimation, 0x104) : 0;
}

ChAnimation *AnimationManager::GetAnimationDataByNoPath(char *pi_pAniName)
{
  if (!pi_pAniName)
  {
    return NULL;
  }

  char l_szName[256];
  CopyStringSafe(l_szName, sizeof(l_szName), pi_pAniName);
  LowercaseInPlace(l_szName);

  PtrVector l_stVector = GetVector(this,
                                   kVectorBeginOffsetAnimationManager,
                                   kVectorEndOffsetAnimationManager,
                                   kVectorCapacityOffsetAnimationManager);
  for (void **it = l_stVector.m_pBegin; it && it != l_stVector.m_pEnd; ++it)
  {
    ChAnimation *l_pAnimation = static_cast<ChAnimation *>(*it);
    if (!l_pAnimation)
    {
      continue;
    }

    const char *l_pFileName = reinterpret_cast<const char *>(l_pAnimation) + 4;
    const char *l_pSlash = strrchr(l_pFileName, '\\');
    if (!l_pSlash)
    {
      l_pSlash = l_pFileName;
    }
    if (!strcmp(l_pSlash, l_szName))
    {
      return l_pAnimation;
    }
  }
  return NULL;
}

ChAnimation *AnimationManager::GetAnimationData(char *pi_pAniName)
{
  if (!pi_pAniName)
  {
    return NULL;
  }

  char l_szName[256];
  CopyStringSafe(l_szName, sizeof(l_szName), pi_pAniName);
  LowercaseInPlace(l_szName);

  PtrVector l_stVector = GetVector(this,
                                   kVectorBeginOffsetAnimationManager,
                                   kVectorEndOffsetAnimationManager,
                                   kVectorCapacityOffsetAnimationManager);
  for (void **it = l_stVector.m_pBegin; it && it != l_stVector.m_pEnd; ++it)
  {
    ChAnimation *l_pAnimation = static_cast<ChAnimation *>(*it);
    if (l_pAnimation &&
        !strcmp(reinterpret_cast<const char *>(l_pAnimation) + 4, l_szName))
    {
      return l_pAnimation;
    }
  }
  return NULL;
}

ChAnimation *AnimationManager::GetAnimationData(unsigned long pi_dwAniID)
{
  PtrVector l_stVector = GetVector(this,
                                   kVectorBeginOffsetAnimationManager,
                                   kVectorEndOffsetAnimationManager,
                                   kVectorCapacityOffsetAnimationManager);
  for (void **it = l_stVector.m_pBegin; it && it != l_stVector.m_pEnd; ++it)
  {
    ChAnimation *l_pAnimation = static_cast<ChAnimation *>(*it);
    if (l_pAnimation && CharacterMR::Field<DWORD>(l_pAnimation, 0x104) == pi_dwAniID)
    {
      return l_pAnimation;
    }
  }
  return NULL;
}

bool AnimationManager::AddAnimation(char *pi_pAniName)
{
  if (!pi_pAniName)
  {
    return false;
  }

  char l_szName[256];
  CopyStringSafe(l_szName, sizeof(l_szName), pi_pAniName);
  LowercaseInPlace(l_szName);
  if (GetAnimationData(l_szName))
  {
    return false;
  }

  ChAnimation *l_pAnimation = new ChAnimation;
  CharacterMR::Field<DWORD>(l_pAnimation, 0x104) =
    CharacterMR::Field<DWORD>(this, kAnimationManagerNextIDOffset);
  CopyStringSafe(reinterpret_cast<char *>(l_pAnimation) + 4, 256, l_szName);
  AppendPointer(this,
                kVectorBeginOffsetAnimationManager,
                kVectorEndOffsetAnimationManager,
                kVectorCapacityOffsetAnimationManager,
                l_pAnimation);
  ++CharacterMR::Field<DWORD>(this, kAnimationManagerNextIDOffset);
  return true;
}

void AnimationManager::DeleteAnimation(char *pi_pAniName)
{
  if (!pi_pAniName)
  {
    return;
  }

  PtrVector l_stVector = GetVector(this,
                                   kVectorBeginOffsetAnimationManager,
                                   kVectorEndOffsetAnimationManager,
                                   kVectorCapacityOffsetAnimationManager);
  for (size_t i = 0; l_stVector.m_pBegin && i < VectorCount(l_stVector); ++i)
  {
    ChAnimation *l_pAnimation = static_cast<ChAnimation *>(l_stVector.m_pBegin[i]);
    if (l_pAnimation &&
        !_stricmp(reinterpret_cast<const char *>(l_pAnimation) + 4, pi_pAniName))
    {
      delete l_pAnimation;
      RemovePointerAt(this,
                      kVectorBeginOffsetAnimationManager,
                      kVectorEndOffsetAnimationManager,
                      kVectorCapacityOffsetAnimationManager,
                      i);
      return;
    }
  }
}

void AnimationManager::DeleteAnimation2(unsigned long pi_dwAniID)
{
  PtrVector l_stVector = GetVector(this,
                                   kVectorBeginOffsetAnimationManager,
                                   kVectorEndOffsetAnimationManager,
                                   kVectorCapacityOffsetAnimationManager);
  for (size_t i = 0; l_stVector.m_pBegin && i < VectorCount(l_stVector); ++i)
  {
    ChAnimation *l_pAnimation = static_cast<ChAnimation *>(l_stVector.m_pBegin[i]);
    if (l_pAnimation && CharacterMR::Field<DWORD>(l_pAnimation, 0x104) == pi_dwAniID)
    {
      delete l_pAnimation;
      RemovePointerAt(this,
                      kVectorBeginOffsetAnimationManager,
                      kVectorEndOffsetAnimationManager,
                      kVectorCapacityOffsetAnimationManager,
                      i);
      return;
    }
  }
}

TextureManager::~TextureManager(void)
{
  Clear();
  ClearPointerVectorStorage(this,
                            kVectorBeginOffsetTextureManager,
                            kVectorEndOffsetTextureManager,
                            kVectorCapacityOffsetTextureManager);
}

int TextureManager::Init(void)
{
  CharacterMR::Field<DWORD>(this, kTextureManagerNextIDOffset) = 1;
  CharacterMR::Field<DWORD>(this, kTextureManagerMemoryOffset) = 0;
  CharacterMR::Field<char>(this, kTextureManagerPathOffset) = '\0';
  CharacterMR::Field<DWORD>(this, 0x9C) = 0;
  return 0;
}

bool TextureManager::AddTexture(char const *pi_pPathName, char const *pi_pFileName)
{
  if (!pi_pFileName || !pi_pFileName[0] || !GetTextureDevice(this))
  {
    return false;
  }

  char l_szTextureName[256];
  CharacterMR::CopyBaseFileNameUpper(pi_pFileName, l_szTextureName, sizeof(l_szTextureName));
  if (!l_szTextureName[0] || GetTextureRecord(this, l_szTextureName))
  {
    return false;
  }

  char l_szFileName[MAX_PATH];
  CombinePath(l_szFileName, sizeof(l_szFileName), pi_pPathName, l_szTextureName);

  IDirect3DTexture8 *l_pTexture = NULL;
  if (!LoadTextureFile(GetTextureDevice(this), l_szFileName, &l_pTexture))
  {
    char l_szRftName[MAX_PATH];
    CopyStringSafe(l_szRftName, sizeof(l_szRftName), l_szFileName);
    ChangeExtension(l_szRftName, sizeof(l_szRftName), ".RFT");
    if (!LoadTextureFile(GetTextureDevice(this), l_szRftName, &l_pTexture))
    {
      char l_szDdsName[MAX_PATH];
      CopyStringSafe(l_szDdsName, sizeof(l_szDdsName), l_szFileName);
      ChangeExtension(l_szDdsName, sizeof(l_szDdsName), ".DDS");
      if (!LoadTextureFile(GetTextureDevice(this), l_szDdsName, &l_pTexture))
      {
        return false;
      }
    }
  }

  ChTextureCompat *l_pRecord =
    static_cast<ChTextureCompat *>(::operator new(sizeof(ChTextureCompat)));
  ZeroMemory(l_pRecord, sizeof(*l_pRecord));
  CopyStringSafe(l_pRecord->m_szName, sizeof(l_pRecord->m_szName), l_szTextureName);
  l_pRecord->m_dwID = CharacterMR::Field<DWORD>(this, kTextureManagerNextIDOffset);
  l_pRecord->m_pTexture = l_pTexture;
  l_pRecord->m_dwBytes = EstimateTextureBytes(l_pTexture);

  AppendPointer(this,
                kVectorBeginOffsetTextureManager,
                kVectorEndOffsetTextureManager,
                kVectorCapacityOffsetTextureManager,
                l_pRecord);
  ++CharacterMR::Field<DWORD>(this, kTextureManagerNextIDOffset);
  CharacterMR::Field<DWORD>(this, kTextureManagerMemoryOffset) += l_pRecord->m_dwBytes;
  return true;
}

IDirect3DTexture8 *TextureManager::GetTexture(char *pi_pTextureName)
{
  ChTextureCompat *l_pRecord = GetTextureRecord(this, pi_pTextureName);
  return l_pRecord ? l_pRecord->m_pTexture : NULL;
}

IDirect3DTexture8 *TextureManager::GetTexture2(unsigned long pi_dwTextureID)
{
  ChTextureCompat *l_pRecord = GetTextureRecord(this, pi_dwTextureID);
  return l_pRecord ? l_pRecord->m_pTexture : NULL;
}

unsigned long TextureManager::GetTextureID(char const *pi_pTextureName)
{
  ChTextureCompat *l_pRecord = GetTextureRecord(this, pi_pTextureName);
  return l_pRecord ? l_pRecord->m_dwID : 0;
}

void TextureManager::DeleteTexture(char *pi_pTextureName)
{
  PtrVector l_stVector = GetVector(this,
                                   kVectorBeginOffsetTextureManager,
                                   kVectorEndOffsetTextureManager,
                                   kVectorCapacityOffsetTextureManager);
  for (size_t i = 0; l_stVector.m_pBegin && i < VectorCount(l_stVector); ++i)
  {
    ChTextureCompat *l_pRecord = static_cast<ChTextureCompat *>(l_stVector.m_pBegin[i]);
    if (l_pRecord && !_stricmp(l_pRecord->m_szName, pi_pTextureName))
    {
      DeleteTextureRecord(l_pRecord);
      RemovePointerAt(this,
                      kVectorBeginOffsetTextureManager,
                      kVectorEndOffsetTextureManager,
                      kVectorCapacityOffsetTextureManager,
                      i);
      return;
    }
  }
}

void TextureManager::DeleteTexture2(unsigned long pi_dwTextureID)
{
  PtrVector l_stVector = GetVector(this,
                                   kVectorBeginOffsetTextureManager,
                                   kVectorEndOffsetTextureManager,
                                   kVectorCapacityOffsetTextureManager);
  for (size_t i = 0; l_stVector.m_pBegin && i < VectorCount(l_stVector); ++i)
  {
    ChTextureCompat *l_pRecord = static_cast<ChTextureCompat *>(l_stVector.m_pBegin[i]);
    if (l_pRecord && l_pRecord->m_dwID == pi_dwTextureID)
    {
      DeleteTextureRecord(l_pRecord);
      RemovePointerAt(this,
                      kVectorBeginOffsetTextureManager,
                      kVectorEndOffsetTextureManager,
                      kVectorCapacityOffsetTextureManager,
                      i);
      return;
    }
  }
}

void TextureManager::Clear(void)
{
  PtrVector l_stVector = GetVector(this,
                                   kVectorBeginOffsetTextureManager,
                                   kVectorEndOffsetTextureManager,
                                   kVectorCapacityOffsetTextureManager);
  for (void **it = l_stVector.m_pBegin; it && it != l_stVector.m_pEnd; ++it)
  {
    DeleteTextureRecord(static_cast<ChTextureCompat *>(*it));
  }
  CharacterMR::Field<void **>(this, kVectorEndOffsetTextureManager) =
    CharacterMR::Field<void **>(this, kVectorBeginOffsetTextureManager);
}

CObjectManager::~CObjectManager(void)
{
  Clear();
  ClearPointerVectorStorage(this,
                            kVectorBeginOffsetObjectManager,
                            kVectorEndOffsetObjectManager,
                            kVectorCapacityOffsetObjectManager);
}

void CObjectManager::Clear(void)
{
  PtrVector l_stVector = GetVector(this,
                                   kVectorBeginOffsetObjectManager,
                                   kVectorEndOffsetObjectManager,
                                   kVectorCapacityOffsetObjectManager);
  for (void **it = l_stVector.m_pBegin; it && it != l_stVector.m_pEnd; ++it)
  {
    CHARACTEROBJECT *l_pCharacterObject = static_cast<CHARACTEROBJECT *>(*it);
    CharacterMR::ReleaseCharacterObject(l_pCharacterObject);
    ::operator delete(l_pCharacterObject);
  }
  CharacterMR::Field<void **>(this, kVectorEndOffsetObjectManager) =
    CharacterMR::Field<void **>(this, kVectorBeginOffsetObjectManager);
}

void CObjectManager::AddCharacter(CHARACTEROBJECT *pi_pCharacterObject)
{
  if (!pi_pCharacterObject)
  {
    return;
  }

  CharacterMR::Field<DWORD>(pi_pCharacterObject, 0) =
    CharacterMR::Field<DWORD>(this, kObjectManagerNextIDOffset);
  ++CharacterMR::Field<DWORD>(this, kObjectManagerNextIDOffset);
  AppendPointer(this,
                kVectorBeginOffsetObjectManager,
                kVectorEndOffsetObjectManager,
                kVectorCapacityOffsetObjectManager,
                pi_pCharacterObject);
}

CHARACTEROBJECT *CObjectManager::GetCharacter(char *pi_pName)
{
  if (!pi_pName)
  {
    return NULL;
  }

  PtrVector l_stVector = GetVector(this,
                                   kVectorBeginOffsetObjectManager,
                                   kVectorEndOffsetObjectManager,
                                   kVectorCapacityOffsetObjectManager);
  for (void **it = l_stVector.m_pBegin; it && it != l_stVector.m_pEnd; ++it)
  {
    CHARACTEROBJECT *l_pCharacterObject = static_cast<CHARACTEROBJECT *>(*it);
    if (l_pCharacterObject && !strcmp(l_pCharacterObject->m_szFileName, pi_pName))
    {
      return l_pCharacterObject;
    }
  }
  return NULL;
}

CHARACTEROBJECT *CObjectManager::GetCharacter(unsigned long pi_dwID)
{
  PtrVector l_stVector = GetVector(this,
                                   kVectorBeginOffsetObjectManager,
                                   kVectorEndOffsetObjectManager,
                                   kVectorCapacityOffsetObjectManager);
  for (void **it = l_stVector.m_pBegin; it && it != l_stVector.m_pEnd; ++it)
  {
    CHARACTEROBJECT *l_pCharacterObject = static_cast<CHARACTEROBJECT *>(*it);
    if (l_pCharacterObject && CharacterMR::Field<DWORD>(l_pCharacterObject, 0) == pi_dwID)
    {
      return l_pCharacterObject;
    }
  }
  return NULL;
}

unsigned long CObjectManager::GetID(char *pi_pName)
{
  CHARACTEROBJECT *l_pCharacterObject = GetCharacter(pi_pName);
  return l_pCharacterObject ? CharacterMR::Field<DWORD>(l_pCharacterObject, 0) : 0;
}

void CObjectManager::DeleteCharacter(char *pi_pName)
{
  PtrVector l_stVector = GetVector(this,
                                   kVectorBeginOffsetObjectManager,
                                   kVectorEndOffsetObjectManager,
                                   kVectorCapacityOffsetObjectManager);
  for (size_t i = 0; l_stVector.m_pBegin && i < VectorCount(l_stVector); ++i)
  {
    CHARACTEROBJECT *l_pCharacterObject = static_cast<CHARACTEROBJECT *>(l_stVector.m_pBegin[i]);
    if (l_pCharacterObject && !strcmp(l_pCharacterObject->m_szFileName, pi_pName))
    {
      CharacterMR::ReleaseCharacterObject(l_pCharacterObject);
      ::operator delete(l_pCharacterObject);
      RemovePointerAt(this,
                      kVectorBeginOffsetObjectManager,
                      kVectorEndOffsetObjectManager,
                      kVectorCapacityOffsetObjectManager,
                      i);
      return;
    }
  }
}

void CObjectManager::DeleteCharacter(unsigned long pi_dwID)
{
  PtrVector l_stVector = GetVector(this,
                                   kVectorBeginOffsetObjectManager,
                                   kVectorEndOffsetObjectManager,
                                   kVectorCapacityOffsetObjectManager);
  for (size_t i = 0; l_stVector.m_pBegin && i < VectorCount(l_stVector); ++i)
  {
    CHARACTEROBJECT *l_pCharacterObject = static_cast<CHARACTEROBJECT *>(l_stVector.m_pBegin[i]);
    if (l_pCharacterObject && CharacterMR::Field<DWORD>(l_pCharacterObject, 0) == pi_dwID)
    {
      CharacterMR::ReleaseCharacterObject(l_pCharacterObject);
      ::operator delete(l_pCharacterObject);
      RemovePointerAt(this,
                      kVectorBeginOffsetObjectManager,
                      kVectorEndOffsetObjectManager,
                      kVectorCapacityOffsetObjectManager,
                      i);
      return;
    }
  }
}

void CRender::Init(void)
{
  ZeroMemory(this, sizeof(CRender));
  CharacterMR::InitMaterial(&CharacterMR::Field<D3DMATERIAL8>(this, 8),
                            0.85f,
                            0.85f,
                            0.85f,
                            1.0f);
}

void CRender::SetState(void)
{
  SetRenderStatesForCharacter(GetRenderDevice(this));
}

void CRender::UnSetState(void)
{
  IDirect3DDevice8 *l_pDevice = GetRenderDevice(this);
  if (!l_pDevice)
  {
    return;
  }
  l_pDevice->SetTexture(0, NULL);
  l_pDevice->SetTexture(1, NULL);
  l_pDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
  l_pDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
  l_pDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
  l_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
  l_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
  l_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
  l_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
  l_pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
  l_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);
}

void CRender::SetLight(D3DLIGHT8 pi_stLight)
{
  IDirect3DDevice8 *l_pDevice = GetRenderDevice(this);
  if (!l_pDevice)
  {
    return;
  }
  memcpy(reinterpret_cast<BYTE *>(this) + 8, &pi_stLight, sizeof(pi_stLight));
  l_pDevice->SetLight(0, &pi_stLight);
  l_pDevice->LightEnable(0, TRUE);
}

bool CRender::LoadRealData(CHARACTEROBJECT *pi_pCharacterObject,
                           TextureManager *pi_pTextureManager,
                           bool pi_bUseImmediateFile)
{
  if (!pi_pCharacterObject)
  {
    return false;
  }
  if (CharacterMR::Field<DWORD>(pi_pCharacterObject, CharacterMR::CHOBJ_LOADED))
  {
    return true;
  }

  ObjectData *l_pObjectData = NULL;
  if (pi_bUseImmediateFile)
  {
    l_pObjectData =
      reinterpret_cast<ObjectData *>(GetFileImme(pi_pCharacterObject->m_szFileName));
  }
  else
  {
    LoadFile(pi_pCharacterObject->m_szFileName);
    l_pObjectData =
      reinterpret_cast<ObjectData *>(GetFile(pi_pCharacterObject->m_szFileName));
  }

  if (!l_pObjectData)
  {
    CharacterMR::Field<DWORD>(pi_pCharacterObject, CharacterMR::CHOBJ_ERROR) = 1;
    return false;
  }

  InitVertex(l_pObjectData,
             CharacterMR::Field<CHARACTEROBJECT *>(pi_pCharacterObject,
                                                   CharacterMR::CHOBJ_PARENT_BONE),
             pi_pCharacterObject,
             pi_pTextureManager ? pi_pTextureManager : &m_TM,
             pi_bUseImmediateFile);
  MatchTextureToMesh(pi_pTextureManager ? pi_pTextureManager : &m_TM, pi_pCharacterObject);
  ReleaseObjectData(l_pObjectData);

  if (CharacterMR::Field<DWORD>(pi_pCharacterObject, CharacterMR::CHOBJ_ERROR) ||
      !CharacterMR::Field<ObjectMesh *>(pi_pCharacterObject, CharacterMR::CHOBJ_MESH) ||
      !CharacterMR::Field<DWORD>(pi_pCharacterObject, CharacterMR::CHOBJ_MAX_OBJ))
  {
    CharacterMR::Field<DWORD>(pi_pCharacterObject, CharacterMR::CHOBJ_ERROR) = 1;
    return false;
  }

  CharacterMR::Field<DWORD>(pi_pCharacterObject, CharacterMR::CHOBJ_LOADED) = 1;
  FrameMove(pi_pCharacterObject);
  return true;
}

void CRender::FrameMove(CHARACTEROBJECT *pi_pCharacterObject)
{
  if (!pi_pCharacterObject ||
      !LoadRealData(pi_pCharacterObject, &m_TM, false))
  {
    return;
  }

  DWORD l_dwFrame =
    CharacterMR::Field<DWORD>(pi_pCharacterObject, CharacterMR::CHOBJ_FRAME);
  const DWORD l_dwMaxFrame =
    CharacterMR::Field<DWORD>(pi_pCharacterObject, CharacterMR::CHOBJ_MAX_FRAME);
  if (l_dwMaxFrame && l_dwFrame >= l_dwMaxFrame)
  {
    l_dwFrame = 160;
    CharacterMR::Field<DWORD>(pi_pCharacterObject, CharacterMR::CHOBJ_FRAME) = l_dwFrame;
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

    memcpy(&CharacterMR::Field<D3DXMATRIX>(l_pMesh, CharacterMR::OBJ_PREV_RESULT_MATRIX),
           &CharacterMR::Field<D3DXMATRIX>(l_pMesh, CharacterMR::OBJ_RESULT_MATRIX),
           sizeof(D3DXMATRIX));

    D3DXMATRIX l_matLocal;
    if (CharacterMR::Field<BYTE>(l_pMesh, CharacterMR::OBJ_ANIMATION_ENABLED))
    {
      D3DXMatrixIdentity(&l_matLocal);
      AnimationDataCompat *l_pAnimation =
        CharacterMR::Field<AnimationDataCompat *>(l_pMesh,
                                                 CharacterMR::OBJ_ANIMATION_DATA);
      ApplyAlphaTrack(l_pMesh, l_pAnimation, l_dwFrame);
      ApplyScaleTrack(l_matLocal, l_pAnimation, l_dwFrame, l_pMesh);
      ApplyRotationTrack(l_matLocal, l_pAnimation, l_dwFrame, l_pMesh);
      ApplyTranslationTrack(l_matLocal, l_pAnimation, l_dwFrame, l_pMesh);
    }
    else
    {
      l_matLocal = CharacterMR::Field<D3DXMATRIX>(l_pMesh,
                                                  CharacterMR::OBJ_LOCAL_MATRIX);
    }

    if (CharacterMR::Field<DWORD>(l_pMesh, 0x3E8))
    {
      const float l_fX = l_matLocal._41;
      const float l_fY = l_matLocal._42;
      const float l_fZ = l_matLocal._43;
      MultiplyMatrix(&l_matLocal,
                     &l_matLocal,
                     &CharacterMR::Field<D3DXMATRIX>(l_pMesh, 0x3EC));
      l_matLocal._41 = l_fX;
      l_matLocal._42 = l_fY;
      l_matLocal._43 = l_fZ;
    }

    ObjectMesh *l_pParent =
      CharacterMR::Field<ObjectMesh *>(l_pMesh, CharacterMR::OBJ_PARENT);
    if (l_pParent)
    {
      MultiplyMatrix(&CharacterMR::Field<D3DXMATRIX>(l_pMesh,
                                                     CharacterMR::OBJ_RESULT_MATRIX),
                     &l_matLocal,
                     &CharacterMR::Field<D3DXMATRIX>(l_pParent,
                                                     CharacterMR::OBJ_RESULT_MATRIX));
    }
    else
    {
      CharacterMR::Field<D3DXMATRIX>(l_pMesh,
                                     CharacterMR::OBJ_RESULT_MATRIX) = l_matLocal;
    }
  }

  CharacterMR::Field<DWORD>(pi_pCharacterObject, CharacterMR::CHOBJ_FORCE_RESULT) = 0;
  BuildBlendMatrices(pi_pCharacterObject);
}

void CRender::FrameStep(CHARACTEROBJECT *pi_pCharacterObject, int pi_nFrame)
{
  if (!pi_pCharacterObject ||
      !LoadRealData(pi_pCharacterObject, &m_TM, false))
  {
    return;
  }

  if (pi_nFrame)
  {
    CharacterMR::Field<DWORD>(pi_pCharacterObject, CharacterMR::CHOBJ_FRAME) =
      static_cast<DWORD>(pi_nFrame);
  }
  else
  {
    CharacterMR::Field<DWORD>(pi_pCharacterObject, CharacterMR::CHOBJ_FRAME) += 5;
  }
  FrameMove(pi_pCharacterObject);
}

void CRender::InitEffect(CHARACTEROBJECT *pi_pCharacterObject)
{
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
    CharacterMR::Field<DWORD>(l_pMesh, 0x310) = 0;
    CharacterMR::Field<DWORD>(l_pMesh, 0x318) = 0;
    CharacterMR::Field<float>(l_pMesh, CharacterMR::OBJ_ALPHA) = 1.0f;
    ZeroMemory(reinterpret_cast<BYTE *>(l_pMesh) + 0x36C, 0x30);
  }
}

void CRender::DrawCharacterMat(CHARACTEROBJECT *pi_pCharacterObject,
                               D3DXMATRIX *pi_pWorldMatrix,
                               float pi_fScroll)
{
  UNREFERENCED_PARAMETER(pi_fScroll);
  if (!pi_pCharacterObject || !pi_pWorldMatrix ||
      !LoadRealData(pi_pCharacterObject, &m_TM, false))
  {
    return;
  }

  SetState();
  IDirect3DDevice8 *l_pDevice = GetRenderDevice(this);
  const DWORD l_dwObjectCount =
    CharacterMR::Field<DWORD>(pi_pCharacterObject, CharacterMR::CHOBJ_MAX_OBJ);
  for (DWORD i = 0; i < l_dwObjectCount; ++i)
  {
    DrawObjectMesh(l_pDevice, pi_pCharacterObject, GetMeshAt(pi_pCharacterObject, i), *pi_pWorldMatrix);
  }
}

void CRender::DrawCharacter(CHARACTEROBJECT *pi_pCharacterObject,
                            float *const pi_pfPos,
                            float pi_fRotY,
                            float pi_fScale,
                            float pi_fScroll)
{
  UNREFERENCED_PARAMETER(pi_fScroll);
  if (!pi_pCharacterObject || !pi_pfPos ||
      !LoadRealData(pi_pCharacterObject, &m_TM, false))
  {
    return;
  }

  D3DXMATRIX l_matScale;
  D3DXMATRIX l_matRotation;
  D3DXMATRIX l_matTranslation;
  D3DXMATRIX l_matView;
  D3DXMATRIX l_matWorld;
  D3DXMatrixScaling(&l_matScale, pi_fScale, pi_fScale, pi_fScale);
  D3DXMatrixRotationZ(&l_matRotation, (180.0f - pi_fRotY) * D3DX_PI / 180.0f);
  D3DXMatrixTranslation(&l_matTranslation, pi_pfPos[0], pi_pfPos[2], pi_pfPos[1]);
  InitCharacterMRMatView(&l_matView);
  MultiplyMatrix(&l_matWorld, &l_matScale, &l_matRotation);
  MultiplyMatrix(&l_matWorld, &l_matWorld, &l_matTranslation);
  MultiplyMatrix(&l_matWorld, &l_matWorld, &l_matView);

  SetState();
  IDirect3DDevice8 *l_pDevice = GetRenderDevice(this);
  const DWORD l_dwObjectCount =
    CharacterMR::Field<DWORD>(pi_pCharacterObject, CharacterMR::CHOBJ_MAX_OBJ);
  for (DWORD i = 0; i < l_dwObjectCount; ++i)
  {
    DrawObjectMesh(l_pDevice, pi_pCharacterObject, GetMeshAt(pi_pCharacterObject, i), l_matWorld);
  }
}

void CRender::DrawCharacterShadow(CHARACTEROBJECT *pi_pCharacterObject,
                                  float *const pi_pfPos,
                                  float pi_fRotY,
                                  float pi_fAlpha,
                                  float pi_fScale)
{
  UNREFERENCED_PARAMETER(pi_fAlpha);
  DrawCharacter(pi_pCharacterObject, pi_pfPos, pi_fRotY, pi_fScale, 0.0f);
}

void CRender::MatchTextureToMesh(TextureManager *pi_pTextureManager,
                                 CHARACTEROBJECT *pi_pCharacterObject)
{
  if (!pi_pTextureManager || !pi_pCharacterObject)
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
    CharacterMR::Field<IDirect3DTexture8 *>(l_pMesh, CharacterMR::OBJ_TEXTURE0) =
      pi_pTextureManager->GetTexture2(CharacterMR::Field<DWORD>(l_pMesh,
                                                                CharacterMR::OBJ_TEXTURE0_ID));
    CharacterMR::Field<IDirect3DTexture8 *>(l_pMesh, CharacterMR::OBJ_TEXTURE1) =
      pi_pTextureManager->GetTexture2(CharacterMR::Field<DWORD>(l_pMesh,
                                                                CharacterMR::OBJ_TEXTURE1_ID));
    CharacterMR::Field<IDirect3DTexture8 *>(l_pMesh, CharacterMR::OBJ_TEXTURE2) =
      pi_pTextureManager->GetTexture2(CharacterMR::Field<DWORD>(l_pMesh,
                                                                CharacterMR::OBJ_TEXTURE2_ID));
  }
}

void CRender::SetTransform(CHARACTEROBJECT *pi_pCharacterObject,
                           int pi_nObjectIndex,
                           D3DXMATRIX &pi_matWorld)
{
  if (!pi_pCharacterObject)
  {
    return;
  }

  ObjectMesh *l_pMesh = GetMeshAt(pi_pCharacterObject, pi_nObjectIndex);
  IDirect3DDevice8 *l_pDevice = GetRenderDevice(this);
  if (!l_pMesh || !l_pDevice)
  {
    return;
  }

  SetObjectMeshTransform(l_pDevice, l_pMesh, pi_matWorld);
}

void ChInterface::InitCharacter(IDirect3DDevice8 *pi_pD3DDevice)
{
  CharacterMR::Field<IDirect3DDevice8 *>(this, 0) = pi_pD3DDevice;
  m_TM.m_pD3DDevice = pi_pD3DDevice;
}

void ChInterface::LoadAnimationData(CHARACTEROBJECT *pi_pCharacterObject,
                                    char *pi_pAniName)
{
  if (!pi_pCharacterObject || !pi_pAniName)
  {
    return;
  }

  ChAnimation l_stAnimation;
  CopyStringSafe(reinterpret_cast<char *>(&l_stAnimation) + 4, 256, pi_pAniName);
  if (l_stAnimation.LoadRealAnimation(true))
  {
    ImportAnimationData(pi_pCharacterObject,
                        CharacterMR::Field<BYTE *>(&l_stAnimation, 0x108));
  }
}

void ChInterface::LoadTextureData(TextureManager *pi_pTextureManager,
                                  char *pi_pPathName,
                                  char *pi_pFileName)
{
  if (pi_pTextureManager)
  {
    pi_pTextureManager->AddTexture(pi_pPathName, pi_pFileName);
  }
}

CHARACTEROBJECT *ChInterface::LoadMeshData(TextureManager *pi_pTextureManager,
                                           CObjectManager *pi_pObjectManager,
                                           char *pi_pFileName,
                                           bool pi_bUseImmediateFile,
                                           CHARACTEROBJECT *pi_pParentBone)
{
  CHARACTEROBJECT *l_pCharacterObject =
    LoadMeshData(pi_pObjectManager, pi_pFileName, pi_bUseImmediateFile, pi_pParentBone);
  if (l_pCharacterObject && pi_pTextureManager)
  {
    LoadRealData(l_pCharacterObject, pi_pTextureManager, pi_bUseImmediateFile);
  }
  return l_pCharacterObject;
}

CHARACTEROBJECT *ChInterface::LoadMeshData(char *pi_pFileName,
                                           bool pi_bUseImmediateFile,
                                           CHARACTEROBJECT *pi_pParentBone)
{
  CObjectManager *l_pObjectManager =
    reinterpret_cast<CObjectManager *>(reinterpret_cast<BYTE *>(this) +
                                       kChInterfaceDefaultObjectManagerOffset);
  if (GetMeshData(l_pObjectManager, pi_pFileName))
  {
    return NULL;
  }
  return LoadMeshData(l_pObjectManager, pi_pFileName, pi_bUseImmediateFile, pi_pParentBone);
}

CHARACTEROBJECT *ChInterface::LoadMeshData(CObjectManager *pi_pObjectManager,
                                           char *pi_pFileName,
                                           bool pi_bUseImmediateFile,
                                           CHARACTEROBJECT *pi_pParentBone)
{
  if (!pi_pObjectManager || !pi_pFileName)
  {
    return NULL;
  }

  CHARACTEROBJECT *l_pCharacterObject =
    CharacterMR::CreateCharacterObject(pi_pFileName, pi_pParentBone);
  pi_pObjectManager->AddCharacter(l_pCharacterObject);
  UNREFERENCED_PARAMETER(pi_bUseImmediateFile);
  return l_pCharacterObject;
}

CHARACTEROBJECT *ChInterface::LoadBoneData(char *pi_pFileName, bool pi_bUseImmediateFile)
{
  CObjectManager *l_pObjectManager =
    reinterpret_cast<CObjectManager *>(reinterpret_cast<BYTE *>(this) +
                                       kChInterfaceBoneObjectManagerOffset);
  return LoadMeshData(l_pObjectManager, pi_pFileName, pi_bUseImmediateFile, NULL);
}

CHARACTEROBJECT *ChInterface::GetMeshData(char *pi_pFileName)
{
  CObjectManager *l_pObjectManager =
    reinterpret_cast<CObjectManager *>(reinterpret_cast<BYTE *>(this) +
                                       kChInterfaceDefaultObjectManagerOffset);
  return GetMeshData(l_pObjectManager, pi_pFileName);
}

CHARACTEROBJECT *ChInterface::GetMeshData(CObjectManager *pi_pObjectManager, char *pi_pFileName)
{
  if (!pi_pObjectManager)
  {
    return NULL;
  }

  CHARACTEROBJECT *l_pCharacterObject = pi_pObjectManager->GetCharacter(pi_pFileName);
  if (!l_pCharacterObject)
  {
    return NULL;
  }
  return LoadRealData(l_pCharacterObject, &m_TM, false) ? l_pCharacterObject : NULL;
}

CHARACTEROBJECT *ChInterface::GetMeshData(CObjectManager *pi_pObjectManager, unsigned long pi_dwID)
{
  if (!pi_pObjectManager)
  {
    return NULL;
  }

  CHARACTEROBJECT *l_pCharacterObject = pi_pObjectManager->GetCharacter(pi_dwID);
  if (!l_pCharacterObject)
  {
    return NULL;
  }
  return LoadRealData(l_pCharacterObject, &m_TM, false) ? l_pCharacterObject : NULL;
}

unsigned long ChInterface::GetID(CObjectManager *pi_pObjectManager, char *pi_pFileName)
{
  return pi_pObjectManager ? pi_pObjectManager->GetID(pi_pFileName) : 0;
}

CHARACTEROBJECT *ChInterface::GetBoneData(char *pi_pFileName)
{
  CObjectManager *l_pObjectManager =
    reinterpret_cast<CObjectManager *>(reinterpret_cast<BYTE *>(this) +
                                       kChInterfaceBoneObjectManagerOffset);
  return GetMeshData(l_pObjectManager, pi_pFileName);
}

void ChInterface::DeleteMeshData(char *pi_pFileName)
{
  CObjectManager *l_pObjectManager =
    reinterpret_cast<CObjectManager *>(reinterpret_cast<BYTE *>(this) +
                                       kChInterfaceDefaultObjectManagerOffset);
  l_pObjectManager->DeleteCharacter(pi_pFileName);
}

void ChInterface::DeleteMeshData(unsigned long pi_dwID)
{
  CObjectManager *l_pObjectManager =
    reinterpret_cast<CObjectManager *>(reinterpret_cast<BYTE *>(this) +
                                       kChInterfaceDefaultObjectManagerOffset);
  l_pObjectManager->DeleteCharacter(pi_dwID);
}

void ChInterface::DeleteBoneData(char *pi_pFileName)
{
  CObjectManager *l_pObjectManager =
    reinterpret_cast<CObjectManager *>(reinterpret_cast<BYTE *>(this) +
                                       kChInterfaceBoneObjectManagerOffset);
  l_pObjectManager->DeleteCharacter(pi_pFileName);
}

void ChInterface::DeleteBoneData(unsigned long pi_dwID)
{
  CObjectManager *l_pObjectManager =
    reinterpret_cast<CObjectManager *>(reinterpret_cast<BYTE *>(this) +
                                       kChInterfaceBoneObjectManagerOffset);
  l_pObjectManager->DeleteCharacter(pi_dwID);
}

ChAnimation *ChInterface::AddAnimationData(AnimationManager *pi_pAniManager, char *pi_pAniName)
{
  if (!pi_pAniManager || !pi_pAniName)
  {
    return NULL;
  }

  ChAnimation *l_pAnimation = pi_pAniManager->GetAnimationData(pi_pAniName);
  if (l_pAnimation)
  {
    return l_pAnimation;
  }

  pi_pAniManager->AddAnimation(pi_pAniName);
  return pi_pAniManager->GetAnimationData(pi_pAniName);
}

ChAnimation *ChInterface::GetAnimationData(AnimationManager *pi_pAniManager, char *pi_pAniName)
{
  return pi_pAniManager ? pi_pAniManager->GetAnimationData(pi_pAniName) : NULL;
}

bool ChInterface::IsLoadAnimation(ChAnimation *pi_pAnimation)
{
  return pi_pAnimation ? CharacterMR::Field<DWORD>(pi_pAnimation, 0) != 0 : false;
}

void ChInterface::MatchAnimationToMesh(ChAnimation *pi_pAnimation,
                                       CHARACTEROBJECT *pi_pCharacterObject)
{
  if (!pi_pAnimation || !pi_pCharacterObject)
  {
    return;
  }

  CHARACTEROBJECT *l_pParentBone =
    CharacterMR::Field<CHARACTEROBJECT *>(pi_pCharacterObject,
                                          CharacterMR::CHOBJ_PARENT_BONE);
  if (l_pParentBone && !CharacterMR::Field<DWORD>(l_pParentBone, CharacterMR::CHOBJ_LOADED))
  {
    return;
  }

  if (LoadRealData(pi_pCharacterObject, &m_TM, false) &&
      pi_pAnimation->LoadRealAnimation(false))
  {
    ImportAnimationData(pi_pCharacterObject,
                        CharacterMR::Field<BYTE *>(pi_pAnimation, 0x108));
    CharacterMR::Field<DWORD>(pi_pCharacterObject, CharacterMR::CHOBJ_FRAME) = 160;
    FrameMove(pi_pCharacterObject);
  }
}

void ChInterface::AnimationReset(CHARACTEROBJECT *pi_pCharacterObject)
{
  if (!pi_pCharacterObject)
  {
    return;
  }

  const DWORD l_dwObjectCount =
    CharacterMR::Field<DWORD>(pi_pCharacterObject, CharacterMR::CHOBJ_MAX_OBJ);
  for (DWORD i = 0; i < l_dwObjectCount; ++i)
  {
    ObjectMesh *l_pMesh = GetMeshAt(pi_pCharacterObject, i);
    if (l_pMesh)
    {
      FreeAnimationData(l_pMesh);
      CharacterMR::Field<float>(l_pMesh, CharacterMR::OBJ_ALPHA) = 1.0f;
    }
  }
  CharacterMR::Field<DWORD>(pi_pCharacterObject, CharacterMR::CHOBJ_FRAME) = 160;
  CharacterMR::Field<DWORD>(pi_pCharacterObject, CharacterMR::CHOBJ_MAX_FRAME) = 0;
}

void ChInterface::AddPartToMM(char *pi_pPartName)
{
  if (!pi_pPartName)
  {
    return;
  }

  if (GetPart(pi_pPartName))
  {
    return;
  }

  CObjectManager *l_pObjectManager =
    static_cast<CObjectManager *>(::operator new(sizeof(CObjectManager)));
  ZeroMemory(l_pObjectManager, sizeof(CObjectManager));
  char *l_pPartName = reinterpret_cast<char *>(l_pObjectManager) + 20;
  CopyStringSafe(l_pPartName, sizeof(CObjectManager) - 20, pi_pPartName);
  UppercaseInPlace(l_pPartName);
  AppendPointer(reinterpret_cast<BYTE *>(this) + kChInterfaceMeshManagerOffset,
                0x04,
                0x08,
                0x0C,
                l_pObjectManager);
}

CObjectManager *ChInterface::GetPart(char *pi_pPartName)
{
  if (!pi_pPartName)
  {
    return NULL;
  }

  char l_szPartName[128];
  CopyStringSafe(l_szPartName, sizeof(l_szPartName), pi_pPartName);
  UppercaseInPlace(l_szPartName);

  void *l_pMeshManager = reinterpret_cast<BYTE *>(this) + kChInterfaceMeshManagerOffset;
  PtrVector l_stVector = GetVector(l_pMeshManager, 0x04, 0x08, 0x0C);
  for (void **it = l_stVector.m_pBegin; it && it != l_stVector.m_pEnd; ++it)
  {
    CObjectManager *l_pObjectManager = static_cast<CObjectManager *>(*it);
    if (l_pObjectManager &&
        !strcmp(reinterpret_cast<const char *>(l_pObjectManager) + 20, l_szPartName))
    {
      return l_pObjectManager;
    }
  }
  return NULL;
}

void ChInterface::SetAlpha(CHARACTEROBJECT *pi_pCharacterObject, float pi_fAlpha)
{
  if (!pi_pCharacterObject)
  {
    return;
  }

  CharacterMR::Field<float>(pi_pCharacterObject, CharacterMR::CHOBJ_ALPHA) = pi_fAlpha;
  const DWORD l_dwObjectCount =
    CharacterMR::Field<DWORD>(pi_pCharacterObject, CharacterMR::CHOBJ_MAX_OBJ);
  for (DWORD i = 0; i < l_dwObjectCount; ++i)
  {
    ObjectMesh *l_pMesh = GetMeshAt(pi_pCharacterObject, i);
    if (l_pMesh)
    {
      CharacterMR::Field<float>(l_pMesh, CharacterMR::OBJ_ALPHA) = pi_fAlpha;
    }
  }
}

float ChInterface::GetAlpha(CHARACTEROBJECT *pi_pCharacterObject)
{
  return pi_pCharacterObject ?
    CharacterMR::Field<float>(pi_pCharacterObject, CharacterMR::CHOBJ_ALPHA) :
    0.0f;
}

void ChInterface::SetMaterial(CHARACTEROBJECT *pi_pCharacterObject, D3DMATERIAL8 &pi_stMaterial)
{
  if (pi_pCharacterObject)
  {
    CharacterMR::Field<D3DMATERIAL8>(pi_pCharacterObject, CharacterMR::CHOBJ_MATERIAL) =
      pi_stMaterial;
  }
}

int ChInterface::ImportBoundBox(char *pi_pFileName, float *const po_pfMin, float *const po_pfMax)
{
  if (!pi_pFileName || !po_pfMin || !po_pfMax)
  {
    return 0;
  }

  FILE *l_pFile = NULL;
  if (fopen_s(&l_pFile, pi_pFileName, "rb") != 0 || !l_pFile)
  {
    return 0;
  }

  char l_szBuffer[256];
  fread(l_szBuffer, 1, sizeof(l_szBuffer), l_pFile);
  fread(po_pfMax, 1, sizeof(float) * 3, l_pFile);
  fread(po_pfMin, 1, sizeof(float) * 3, l_pFile);
  fclose(l_pFile);
  return 1;
}

void ChInterface::RematchParent(CHARACTEROBJECT *pi_pParentBone,
                                CHARACTEROBJECT *pi_pCharacterObject)
{
  if (!pi_pParentBone || !pi_pCharacterObject)
  {
    return;
  }

  MatchParents(CharacterMR::Field<ObjectMesh *>(pi_pParentBone, CharacterMR::CHOBJ_MESH),
               CharacterMR::Field<DWORD>(pi_pParentBone, CharacterMR::CHOBJ_MAX_OBJ),
               CharacterMR::Field<ObjectMesh *>(pi_pCharacterObject, CharacterMR::CHOBJ_MESH),
               CharacterMR::Field<DWORD>(pi_pCharacterObject, CharacterMR::CHOBJ_MAX_OBJ),
               true);
}

void ChInterface::RestoreParent(CHARACTEROBJECT *pi_pCharacterObject)
{
  if (!pi_pCharacterObject)
  {
    return;
  }

  const DWORD l_dwObjectCount =
    CharacterMR::Field<DWORD>(pi_pCharacterObject, CharacterMR::CHOBJ_MAX_OBJ);
  for (DWORD i = 0; i < l_dwObjectCount; ++i)
  {
    ObjectMesh *l_pMesh = GetMeshAt(pi_pCharacterObject, i);
    if (!l_pMesh || !CharacterMR::Field<DWORD>(l_pMesh, CharacterMR::OBJ_PARENT_CHANGED))
    {
      continue;
    }
    CharacterMR::Field<BYTE>(l_pMesh, CharacterMR::OBJ_HAS_PARENT) = 0;
    CharacterMR::Field<ObjectMesh *>(l_pMesh, CharacterMR::OBJ_PARENT) = NULL;
    CharacterMR::Field<DWORD>(l_pMesh, CharacterMR::OBJ_PARENT_CHANGED) = 0;
    memcpy(&CharacterMR::Field<D3DXMATRIX>(l_pMesh, CharacterMR::OBJ_BIND_MATRIX),
           &CharacterMR::Field<D3DXMATRIX>(l_pMesh, CharacterMR::OBJ_BASE_MATRIX),
           sizeof(D3DXMATRIX));
  }
}

void ChInterface::SetEyeVec(char *pi_pDummyName,
                            CHARACTEROBJECT *pi_pCharacterObject,
                            D3DXMATRIX pi_matRot)
{
  if (!pi_pDummyName || !pi_pCharacterObject)
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

    if (!_stricmp(reinterpret_cast<const char *>(l_pMesh) + CharacterMR::OBJ_NAME,
                 pi_pDummyName))
    {
      CharacterMR::Field<DWORD>(l_pMesh, 0x3E8) = 1;
      CharacterMR::Field<D3DXMATRIX>(l_pMesh, 0x3EC) = pi_matRot;
    }
  }
}

int CHARACTEROBJECT::IsHaveEffect(void)
{
  const DWORD l_dwObjectCount = CharacterMR::Field<DWORD>(this, CharacterMR::CHOBJ_MAX_OBJ);
  for (DWORD i = 0; i < l_dwObjectCount; ++i)
  {
    ObjectMesh *l_pMesh = GetMeshAt(this, i);
    if (!l_pMesh)
    {
      continue;
    }

    for (DWORD j = 0; j < 3; ++j)
    {
      if (CharacterMR::Field<DWORD>(l_pMesh, 0x378 + (j * 4)))
      {
        return 1;
      }
    }
    if (CharacterMR::Field<DWORD>(l_pMesh, CharacterMR::OBJ_EFFECT_ENTITY))
    {
      return 1;
    }
  }
  return 0;
}

void AlphaMeshManager::DrawAlpahMesh(void)
{
  // CharacterMRCompat draws alpha-capable meshes immediately instead of using
  // the original deferred AlphaMeshManager queue.
}
