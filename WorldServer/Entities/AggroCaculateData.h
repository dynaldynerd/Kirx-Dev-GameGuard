#pragma once

#include "IdaCompat.h"

struct  AggroCaculateData
{
  struct SpecialSkillValue
  {
    unsigned __int8 m_byAttackType;
    unsigned __int16 m_wIndex;
    int m_nValue;
  };

  AggroCaculateData();

  void Init();
  bool Load(const char *fileName);
  int GetDefault(unsigned int dwKind);
  int GetSpecialData(unsigned __int8 byAttackType, unsigned __int16 wIndex);
  int GetSize();
  bool PushSpecialData(unsigned __int8 byAttackType, unsigned __int16 wIndex, int nValue);

  int m_DefaultData[18];
  SpecialSkillValue m_SpecialData[50];
  int m_bLoad;
  int m_nTotalCount;
};

extern AggroCaculateData g_AggroCaculateData;
