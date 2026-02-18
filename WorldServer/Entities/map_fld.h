#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)
struct  _map_fld : _base_fld
{
  char m_strFileName[64];
  int m_nMapType;
  int m_nLayerNum;
  int m_nRaceVillageCode;
  int m_nMonsterSetFileNum;
  int m_nMapClass;
  int m_nRadius;
  int m_nLevelLimit;
  int m_nUpLevelLim;
  int m_nPotionLim;
  int m_nRacePvpUsable[3];
};
#pragma pack(pop)
