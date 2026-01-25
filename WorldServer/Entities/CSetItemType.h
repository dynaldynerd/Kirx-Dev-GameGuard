#pragma once

#include "IdaCompat.h"

class CRecordData;

struct __cppobj si_effect
{
  unsigned __int8 byTargetItemCount;
  unsigned __int8 byTargetEffectCount;
};

struct __cppobj si_interpret
{
  struct effect_data
  {
    int iEffectCode;
    float fEffectValue;
  };

  unsigned __int8 byEffectTypeCount;
  si_effect effect_type[8];
  effect_data effect_info[8];
};

class __cppobj CSetItemType
{
public:
  CSetItemType();
  ~CSetItemType();

  void Class_Init();
  bool SetItemType_Init(CRecordData *prd);
  bool SetItemType_UnInit();
  int GetEffectTypeCount() const;
  si_interpret *Getsi_interpret(int set_pos);

private:
  int m_iEffectTypeCount;
  si_interpret **m_pEffectCountInfo;
};
