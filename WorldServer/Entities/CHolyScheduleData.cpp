#include "pch.h"

#include "CHolyScheduleData.h"

CHolyScheduleData::CHolyScheduleData()
{
  Init();
}

CHolyScheduleData::~CHolyScheduleData()
{
  Init();
}

void CHolyScheduleData::Init()
{
  m_bSet = false;
  if (m_pSchedule)
  {
    delete[] m_pSchedule;
    m_pSchedule = nullptr;
  }
  m_nTotalSchedule = 0;
}

CHolyScheduleData::__HolyScheduleNode *CHolyScheduleData::GetIndex(int nNumOfTime)
{
  const int index = nNumOfTime - 1;
  if (index >= 0 && index < m_nTotalSchedule)
    return &m_pSchedule[index];
  return nullptr;
}

unsigned int CHolyScheduleData::GetTotalSceduleTerm(int nNumOfTime)
{
  CHolyScheduleData::__HolyScheduleNode *node = GetIndex(nNumOfTime);
  if (!node)
    return 0;
  return static_cast<unsigned int>(
    node->m_nSceneTime[4] +
    node->m_nSceneTime[6] +
    node->m_nSceneTime[3] +
    node->m_nSceneTime[5]);
}
