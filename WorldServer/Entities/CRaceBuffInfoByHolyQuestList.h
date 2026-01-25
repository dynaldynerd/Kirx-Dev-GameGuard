#pragma once

#include "IdaCompat.h"
#include <vector>

#include "CRaceBuffInfoByHolyQuestfGroup.h"

class __cppobj CRaceBuffInfoByHolyQuestList
{
public:
  bool Init();

  std::vector<CRaceBuffInfoByHolyQuestfGroup *> m_vecInfo;
};
