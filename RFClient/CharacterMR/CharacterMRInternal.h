#pragma once

#include "../Character.h"

#include <vector>

static const DWORD D3DFVF_CHARACTER_VERTEX =
  D3DFVF_XYZB1 | D3DFVF_NORMAL | D3DFVF_TEX1;

namespace CharacterMR
{
  enum CharacterObjectOffset
  {
    CHOBJ_LOADED = 0x104,
    CHOBJ_ERROR = 0x108,
    CHOBJ_PARENT_BONE = 0x10C,
    CHOBJ_TEXTURE_PATH = 0x114,
    CHOBJ_MAX_OBJ = 0x210,
    CHOBJ_MESH = 0x214,
    CHOBJ_OLD_FRAME = 0x298,
    CHOBJ_FRAME = 0x29C,
    CHOBJ_MAX_FRAME = 0x2A0,
    CHOBJ_TRANSITION = 0x2A4,
    CHOBJ_FORCE_RESULT = 0x2A8,
    CHOBJ_TRANS_FRAME = 0x2AC,
    CHOBJ_TRANS_TIME = 0x2B0,
    CHOBJ_MATERIAL = 0x2B4,
    CHOBJ_ALPHA = 0x304
  };

  enum ObjectMeshOffset
  {
    OBJ_NAME = 0x000,
    OBJ_PARENT_NAME = 0x040,
    OBJ_HAS_PARENT = 0x080,
    OBJ_PARENT_CHANGED = 0x081,
    OBJ_PARENT = 0x088,
    OBJ_BLEND_COUNT = 0x08C,
    OBJ_BLEND_PARENT = 0x090,
    OBJ_VERTEX_BUFFER = 0x0A4,
    OBJ_BASE_MATRIX = 0x0B0,
    OBJ_BIND_MATRIX = 0x0F0,
    OBJ_INV_BIND_MATRIX = 0x130,
    OBJ_LOCAL_MATRIX = 0x170,
    OBJ_SOURCE_MATRIX = 0x1B0,
    OBJ_RESULT_MATRIX = 0x1F0,
    OBJ_PREV_RESULT_MATRIX = 0x230,
    OBJ_ANIMATION_ENABLED = 0x2F8,
    OBJ_ANIMATION_DATA = 0x2FC,
    OBJ_ANIMATION_MAX_FRAME = 0x300,
    OBJ_TEXTURE0 = 0x304,
    OBJ_TEXTURE0_ID = 0x308,
    OBJ_TEXTURE1 = 0x30C,
    OBJ_TEXTURE1_ID = 0x310,
    OBJ_TEXTURE2 = 0x314,
    OBJ_TEXTURE2_ID = 0x318,
    OBJ_VERTEX_COUNT = 0x2F0,
    OBJ_LAST_TICK = 0x34C,
    OBJ_SCROLL = 0x350,
    OBJ_SCROLL_SPEED = 0x360,
    OBJ_ALPHA = 0x3B8,
    OBJ_EXTRA_SCALE = 0x3D8,
    OBJ_EFFECT_ENTITY = 0x3A8,
    OBJ_EFFECT_PARTICLE = 0x3B0,
    OBJ_HIDE = 0x42C
  };

  template <typename T>
  inline T &Field(void *pi_pBase, size_t pi_stOffset)
  {
    return *reinterpret_cast<T *>(reinterpret_cast<BYTE *>(pi_pBase) + pi_stOffset);
  }

  template <typename T>
  inline const T &Field(const void *pi_pBase, size_t pi_stOffset)
  {
    return *reinterpret_cast<const T *>(reinterpret_cast<const BYTE *>(pi_pBase) + pi_stOffset);
  }

  void InitMaterial(D3DMATERIAL8 *po_pMaterial,
                    float pi_fRed,
                    float pi_fGreen,
                    float pi_fBlue,
                    float pi_fAlpha);
  void InitObjectMesh(ObjectMesh *pio_pObjectMesh);
  void ReleaseObjectMesh(ObjectMesh *pio_pObjectMesh);
  void ReleaseCharacterObject(CHARACTEROBJECT *pio_pCharacterObject);
  CHARACTEROBJECT *CreateCharacterObject(const char *pi_pFileName,
                                         CHARACTEROBJECT *pi_pParentBone);
  bool LoadBinaryFile(const char *pi_pFileName, std::vector<BYTE> &po_vecBuffer);
  char *LoadBinaryFileAlloc(const char *pi_pFileName);
  void CopyBaseFileNameUpper(const char *pi_pPath,
                             char *po_szFileName,
                             size_t pi_stFileNameSize);
}
