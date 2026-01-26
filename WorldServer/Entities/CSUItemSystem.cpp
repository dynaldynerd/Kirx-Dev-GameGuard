#include "pch.h"

#include "CSUItemSystem.h"

CSUItemSystem *CSUItemSystem::Instance()
{
  static CSUItemSystem s_instance;
  return &s_instance;
}

bool CSUItemSystem::SUItemSystem_Init()
{
  char pszErrMsg[144] = {};

  Class_Init();
  if (m_SUOrigin[0].ReadRecord(".\\script\\SetItemEff.dat", 0x404u, pszErrMsg))
  {
    m_bLoadData[0] = true;
    if (m_SetItemType.SetItemType_Init(m_SUOrigin))
    {
      m_bChangeData[0] = true;
      ++m_bySUItemCount;
      return true;
    }

    MyMessageBox("Failed Interpret Script data File : CSUItemSystem_Init()", "Error Interpeting Of SetItemType Data");
    return false;
  }

  MyMessageBox("Failed Load Script Data File : CSUItemSystem_Init()", pszErrMsg);
  return false;
}

bool CSUItemSystem::SUItemSystem_CheckData()
{
  if (m_SUOrigin[0].GetRecordNum() <= 0)
  {
    return false;
  }

  const int effectTypeCount = m_SetItemType.GetEffectTypeCount();
  if (effectTypeCount <= 0)
  {
    return false;
  }

  for (int set_pos = 0; set_pos < effectTypeCount; ++set_pos)
  {
    si_interpret *interpret = m_SetItemType.Getsi_interpret(set_pos);
    if (interpret == nullptr)
    {
      return false;
    }
    const unsigned __int8 count = interpret->GetEffectTypeCount();
    if (count > 8u)
    {
      return false;
    }
    for (int idx = 0; idx < count; ++idx)
    {
      if (interpret->GetCountOfItem(static_cast<unsigned int>(idx)) > 0xCu)
      {
        return false;
      }
      if (interpret->GetCountOfEffect(static_cast<unsigned int>(idx)) > 8u)
      {
        return false;
      }
    }
  }

  return true;
}

void CSUItemSystem::Class_Init()
{
  m_bySUItemCount = 0;
  m_bLoadData[0] = false;
  m_bChangeData[0] = false;
}
