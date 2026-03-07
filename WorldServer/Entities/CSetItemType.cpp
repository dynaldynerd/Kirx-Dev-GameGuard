#include "pch.h"

#include "CSetItemType.h"

#include "CRecordData.h"

#include <cstring>
#include <cstdlib>

si_interpret::si_interpret()
{
  init();
}

void si_interpret::init()
{
  byEffectTypeCount = 0;
  for (unsigned __int8 j = 0; j < 8u; ++j)
  {
    effect_type[j].init();
  }
}

bool si_interpret::set_effect_interpret(_SetItemEff_fld *pFld)
{
  const int len = static_cast<int>(std::strlen(pFld->m_strEffrule));
  if (len >= 4)
  {
    if ((len % 4) != 0 || (len / 4) > 8)
    {
      byEffectTypeCount = 0;
      return true;
    }

    init();
    unsigned __int8 itemCount = 0;
    unsigned __int8 effectCount = 0;
    char buffer[36]{};
    int groupCount = len / 4;
    int j = 0;
    for (; j < groupCount; ++j)
    {
      std::memset(buffer, 0, 3);
      buffer[0] = pFld->m_strEffrule[4 * j];
      buffer[1] = pFld->m_strEffrule[4 * j + 1];
      buffer[2] = 0;
      itemCount = static_cast<unsigned __int8>(std::atoi(buffer));

      std::memset(buffer, 0, 3);
      buffer[0] = pFld->m_strEffrule[4 * j + 2];
      buffer[1] = pFld->m_strEffrule[4 * j + 3];
      buffer[2] = 0;
      effectCount = static_cast<unsigned __int8>(std::atoi(buffer));

      effect_type[j].set_effect_count_info(itemCount, effectCount);
    }

    byEffectTypeCount = static_cast<unsigned __int8>(j);
    effect_info[0].iEffectCode = pFld->m_nEff1Code;
    effect_info[0].fEffectValue = pFld->m_fEff1Unit;
    effect_info[1].iEffectCode = pFld->m_nEff2Code;
    effect_info[1].fEffectValue = pFld->m_fEff2Unit;
    effect_info[2].iEffectCode = pFld->m_nEff3Code;
    effect_info[2].fEffectValue = pFld->m_fEff3Unit;
    effect_info[3].iEffectCode = pFld->m_nEff4Code;
    effect_info[3].fEffectValue = pFld->m_fEff4Unit;
    effect_info[4].iEffectCode = pFld->m_nEff5Code;
    effect_info[4].fEffectValue = pFld->m_fEff5Unit;
    effect_info[5].iEffectCode = pFld->m_nEff6Code;
    effect_info[5].fEffectValue = pFld->m_fEff6Unit;
    effect_info[6].iEffectCode = pFld->m_nEff7Code;
    effect_info[6].fEffectValue = pFld->m_fEff7Unit;
    effect_info[7].iEffectCode = pFld->m_nEff8Code;
    effect_info[7].fEffectValue = pFld->m_fEff8Unit;
    return true;
  }

  byEffectTypeCount = 0;
  return true;
}

unsigned __int8 si_interpret::GetEffectTypeCount() const
{
  return byEffectTypeCount;
}

unsigned __int8 si_interpret::GetCountOfItem(unsigned int idx) const
{
  if (idx < 8)
  {
    return effect_type[idx].GetCountOfItem();
  }
  return 100;
}

unsigned __int8 si_interpret::GetCountOfEffect(unsigned int idx) const
{
  if (idx < 8)
  {
    return effect_type[idx].GetCountOfEffect();
  }
  return 100;
}

int si_interpret::GetEffectCode(unsigned int idx) const
{
  if (idx < 8)
  {
    return effect_info[idx].iEffectCode;
  }
  return 100;
}

float si_interpret::GetEffectValue(unsigned int idx) const
{
  if (idx < 8)
  {
    return effect_info[idx].fEffectValue;
  }
  return 100.0f;
}

si_effect::si_effect()
{
  init();
}

void si_effect::init()
{
  byTargetItemCount = 0;
  byTargetEffectCount = 0;
}

void si_effect::set_effect_count_info(unsigned __int8 byCountOfItem, unsigned __int8 byCountOfEffect)
{
  byTargetItemCount = byCountOfItem;
  byTargetEffectCount = byCountOfEffect;
}

unsigned __int8 si_effect::GetCountOfItem() const
{
  return byTargetItemCount;
}

unsigned __int8 si_effect::GetCountOfEffect() const
{
  return byTargetEffectCount;
}

CSetItemType::CSetItemType()
{
  m_pEffectCountInfo = nullptr;
  Class_Init();
}

CSetItemType::~CSetItemType()
{
  Class_Init();
}

void CSetItemType::Class_Init()
{
  if (m_pEffectCountInfo != nullptr)
  {
    for (int j = 0; j < m_iEffectTypeCount; ++j)
    {
      if (m_pEffectCountInfo[j] != nullptr)
      {
        delete m_pEffectCountInfo[j];
      }
    }
    delete[] m_pEffectCountInfo;
  }
  m_iEffectTypeCount = 0;
  m_pEffectCountInfo = nullptr;
}

bool CSetItemType::SetItemType_Init(CRecordData *prd)
{
  if (prd == nullptr)
  {
    return false;
  }
  if (m_iEffectTypeCount)
  {
    return false;
  }
  if (prd->GetRecordNum() <= 0)
  {
    return false;
  }

  const unsigned int recordNum = prd->GetRecordNum();
  m_pEffectCountInfo = new si_interpret *[recordNum];
  int n = 0;
  const int count = static_cast<int>(prd->GetRecordNum());
  while (n < count)
  {
    _SetItemEff_fld *pFld = reinterpret_cast<_SetItemEff_fld *>(prd->GetRecord(n));
    if (pFld == nullptr)
    {
      Class_Init();
      return false;
    }

    si_interpret *entry = new si_interpret();
    m_pEffectCountInfo[n] = entry;
    if (!entry->set_effect_interpret(pFld))
    {
      Class_Init();
      return false;
    }
    ++n;
  }

  m_iEffectTypeCount = n;
  return true;
}

bool CSetItemType::SetItemType_UnInit()
{
  Class_Init();
  return true;
}

int CSetItemType::GetEffectTypeCount() const
{
  return m_iEffectTypeCount;
}

si_interpret *CSetItemType::Getsi_interpret(int set_pos)
{
  if (set_pos < 0)
  {
    return nullptr;
  }
  if (m_pEffectCountInfo == nullptr)
  {
    return nullptr;
  }
  if (set_pos <= m_iEffectTypeCount)
  {
    return m_pEffectCountInfo[set_pos];
  }
  return nullptr;
}
