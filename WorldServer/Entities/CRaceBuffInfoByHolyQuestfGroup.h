#pragma once

#include "IdaCompat.h"
#include <vector>

#include "CRaceBuffInfoByHolyQuest.h"

class __cppobj CRaceBuffInfoByHolyQuestfGroup
{
public:
  unsigned int m_uiNTh;
  std::vector<CRaceBuffInfoByHolyQuest *> m_vecInfo;
};
