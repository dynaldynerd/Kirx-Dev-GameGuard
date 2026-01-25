#pragma once

#include "IdaCompat.h"
#include <vector>

#include "CRaceBuffInfoByHolyQuest.h"

class __cppobj CRaceBuffInfoByHolyQuestfGroup
{
public:
  explicit CRaceBuffInfoByHolyQuestfGroup(unsigned int nth);
  bool Init();
  static const char *TYPE_NAME[4];

  unsigned int m_uiNTh;
  std::vector<CRaceBuffInfoByHolyQuest *> m_vecInfo;
};
