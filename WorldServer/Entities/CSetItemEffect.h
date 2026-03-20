#pragma once

#include "IdaCompat.h"
#include "SetItemEff_fld.h"

struct _AVATOR_DATA;

/* 1762 */
class  CSetItemEffect
{
  struct set_effect_info
  {
    bool m_bCheckSetEffect;
    unsigned int m_dwSetItem;
    unsigned __int8 m_bySetItemNum;
    unsigned __int8 m_bySetEffectNum;
  };

  set_effect_info m_setCount[6];
  set_effect_info m_resetInfo;
  unsigned __int8 m_byTotalSetCount;

public:
  CSetItemEffect();
  ~CSetItemEffect();
  void Init_Info();
  void Init_Data(unsigned __int8 byIdx);
  int SetOnEffect(
    _AVATOR_DATA *pData,
    unsigned int dwSetItem,
    unsigned __int8 bySetItemNum,
    unsigned __int8 bySetEffectNum);
  int SetOffEffect(unsigned int dwSetItem, unsigned __int8 bySetItemNum, unsigned __int8 bySetEffectNum);
  unsigned __int8 Check_EquipItem(_AVATOR_DATA *pData, _SetItemEff_fld *pSetFld);
  unsigned __int8 Check_Base_EquipItem(_AVATOR_DATA *pData, _SetItemEff_fld *pSetFld);
  unsigned __int8 Check_Other_EquipItem(_AVATOR_DATA *pData, _SetItemEff_fld *pSetFld);
  bool IsSetOn(unsigned int dwSetItem);
  bool IsSetOnComplete(unsigned int dwSetItem, unsigned __int8 bySetItemNum, unsigned __int8 bySetEffectNum);
  bool Attach_Set(unsigned int dwSetItem, unsigned __int8 bySetItemNum, unsigned __int8 bySetEffectNum);
  bool Detach_Set(unsigned int dwSetItem);
  bool Reset_Set(unsigned int dwSetItem, unsigned __int8 bySetItemNum, unsigned __int8 bySetEffectNum);
  void SetResetInfo(bool bSet, unsigned int dwSetItem, unsigned __int8 bySetItemNum, unsigned __int8 bySetEffectNum);
  unsigned __int8 GetResetEffectNum();
  unsigned __int8 GetResetItemNum();
  unsigned int GetResetIdx();
};


