#pragma once

#include "IdaCompat.h"
#include "CRecordData.h"

struct _PotionItem_fld;

class __cppobj CPotionMgr
{
public:
  struct __cppobj PotionInnerData
  {
    bool m_bPotionDelayIndexList[38];
  };

  struct __declspec(align(8)) __cppobj _ParamData
  {
    float fValue;
    _PotionItem_fld *pPotionFld;
    unsigned int dwCurTime;
    unsigned int dwEndTime;
    unsigned int dwNextTime;
  };

  PotionInnerData m_PotionInnerData;
  CRecordData m_tblPotionEffectData;
  CRecordData m_tblPotionCheckData;
};
