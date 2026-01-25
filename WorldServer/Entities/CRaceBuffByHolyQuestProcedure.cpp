#include "pch.h"

#include "CRaceBuffByHolyQuestProcedure.h"

bool CRaceBuffByHolyQuestProcedure::Init()
{
  return m_kBuffHolyQestResultInfo.Load() && m_kBuffInfo.Init();
}
