#include "pch.h"

#include "CSetItemEffect.h"

void CSetItemEffect::Init_Info()
{
  for (int j = 0; j < 6; ++j)
  {
    Init_Data(static_cast<unsigned __int8>(j));
  }
  m_byTotalSetCount = 0;
}

void CSetItemEffect::Init_Data(unsigned __int8 byIdx)
{
  if (byIdx < 6)
  {
    m_setCount[byIdx].m_bCheckSetEffect = false;
    m_setCount[byIdx].m_dwSetItem = 0;
    m_setCount[byIdx].m_bySetItemNum = 0;
    m_setCount[byIdx].m_bySetEffectNum = 0;
  }
}

