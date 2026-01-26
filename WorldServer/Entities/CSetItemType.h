#pragma once

#include "IdaCompat.h"
#include "SetItemEff_fld.h"

class CRecordData;

struct __cppobj si_effect
{
  unsigned __int8 byTargetItemCount;
  unsigned __int8 byTargetEffectCount;

  void init();
  void set_effect_count_info(unsigned __int8 byCountOfItem, unsigned __int8 byCountOfEffect);
  unsigned __int8 GetCountOfItem() const;
  unsigned __int8 GetCountOfEffect() const;
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

  si_interpret();
  void init();
  bool set_effect_interpret(struct _SetItemEff_fld *pFld);
  unsigned __int8 GetEffectTypeCount() const;
  unsigned __int8 GetCountOfItem(unsigned int idx) const;
  unsigned __int8 GetCountOfEffect(unsigned int idx) const;
  int GetEffectCode(unsigned int idx) const;
  float GetEffectValue(unsigned int idx) const;
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
