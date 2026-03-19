#pragma once

#include "IdaCompat.h"

struct _mon_block;

struct  MonsterSetInfoData
{
  MonsterSetInfoData();

  void Init();
  int Load(const char *fileName);
  unsigned __int8 GetLevelContSFTime(unsigned __int8 byEffectCode, unsigned __int8 byLevel);
  int GetLostMonsterTargetDistance();
  float GetMonsterForcePowerRate();
  float GetMaxToleranceProbMax(int nMonGrade);
  unsigned int GetMonsterDropRate(int iDiffLevel);
  bool IsRotateBlock(_mon_block *pBlock);

  unsigned __int8 m_byLevel_ContSFTime[4][7];
  int m_nMonsterLostTargetDistance;
  float m_fMonsterForcePowerRate;
  char (*m_strRotMonBlk_Ar)[64];
  int m_nMonBlkCount;
  float m_fToleranceProbMax[7];
  int m_bLoad;
  int m_iMonsterLootRateSame;
  union
  {
    struct
    {
      int m_iMonsterLootingRateUp[11];
      int m_iMonsterLootingRateDown[11];
    };
    int m_iMonsterLootingRateFlat[22];
  };
};

extern MonsterSetInfoData g_MonsterSetInfoData;
