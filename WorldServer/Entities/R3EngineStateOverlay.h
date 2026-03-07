#pragma once

#include "R3EngineState.h"

#include <cstddef>

struct R3EngineStateOverlay
{
  _R3ENGINE_STATE m_Base;
  unsigned char m_PadToSkyTextureName[2144 - sizeof(_R3ENGINE_STATE)];
  char mSkyTextureName[128];
  unsigned int mSkyTextureNameReserved;
  unsigned int mSkyLayerData[16];
  unsigned char m_PadToSecondaryTextureName[16];
  char mSecondaryTextureName[128];
  char mTertiaryTextureName[128];
};

static_assert(offsetof(R3EngineStateOverlay, mSkyTextureName) == 2144);
static_assert(offsetof(R3EngineStateOverlay, mSkyLayerData) == 2276);
static_assert(offsetof(R3EngineStateOverlay, mSecondaryTextureName) == 2356);
static_assert(offsetof(R3EngineStateOverlay, mTertiaryTextureName) == 2484);
