#include "pch.h"

#include "CSetItemType.h"

#include "CRecordData.h"

CSetItemType::CSetItemType()
  : m_iEffectTypeCount(0),
    m_pEffectCountInfo(nullptr)
{
}

CSetItemType::~CSetItemType()
{
  Class_Init();
}

void CSetItemType::Class_Init()
{
  m_iEffectTypeCount = 0;
  if (m_pEffectCountInfo != nullptr)
  {
    delete[] m_pEffectCountInfo;
    m_pEffectCountInfo = nullptr;
  }
}

bool CSetItemType::SetItemType_Init(CRecordData *prd)
{
  if (prd == nullptr)
  {
    return false;
  }
  if (m_iEffectTypeCount != 0)
  {
    return false;
  }

  m_iEffectTypeCount = static_cast<int>(prd->GetRecordNum());
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
  if (set_pos < 0 || set_pos >= m_iEffectTypeCount || m_pEffectCountInfo == nullptr)
  {
    return nullptr;
  }
  return m_pEffectCountInfo[set_pos];
}
