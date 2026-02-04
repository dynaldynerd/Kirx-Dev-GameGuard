#include "pch.h"

#include "CHolyStoneSaveData.h"

void CHolyStoneSaveData::DefaultInit(CHolyScheduleData::__HolyScheduleNode *ScheduleNode)
{
  m_nSceneCode = 0;
  m_dwPassTimeInScene = 0;
  m_nStartStoneHP = 1000000;
  m_nHolyMasterRace = -1;
  m_nDestroyStoneRace = -1;
  m_byNumOfTime = 1;
  m_dwCumPlayerNum = 0;
  m_dwCumCount = 0;
  m_wStartYear = 0;
  m_byStartMonth = 0;
  m_byStartDay = 0;
  m_byStartHour = 0;
  m_byStartMin = 0;
  m_dwDestroyerSerial = static_cast<unsigned int>(-1);
  m_eDestroyerState = 3;
  for (int j = 0; j < 3; ++j)
    m_nStoneHP_Buffer[j] = 1000000;
  m_dwTerm[0] = ScheduleNode->m_nSceneTime[5]
              - (ScheduleNode->m_nSceneTime[2]
                 + ScheduleNode->m_nSceneTime[0]
                 + ScheduleNode->m_nSceneTime[1]);
  m_dwTerm[1] = ScheduleNode->m_nSceneTime[3];
  m_dwOreRemainAmount = static_cast<unsigned int>(-1);
  m_dwOreTotalAmount = static_cast<unsigned int>(-1);
  m_dwDestroyerGuildSerial = static_cast<unsigned int>(-1);
  m_byOreTransferCount = 0;
  m_dwOreTransferAmount = 0;
}
