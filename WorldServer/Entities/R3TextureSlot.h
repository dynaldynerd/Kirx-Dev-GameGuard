#pragma once

#include "D3D8Compat.h"

struct R3TextureSlot
{
  IDirect3DTexture8 *mTexture;
  unsigned int mFlag;
  unsigned int mReserved;
};

static_assert(sizeof(R3TextureSlot) == 16);
