#pragma once

#include "IdaCompat.h"

/* 1762 */
class __cppobj __declspec(align(4)) CSetItemEffect
{
  struct __declspec(align(4)) set_effect_info
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
  void Init_Info();
  void Init_Data(unsigned __int8 byIdx);
};

