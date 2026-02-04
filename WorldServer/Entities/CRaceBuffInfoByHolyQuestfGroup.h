#pragma once

#include "IdaCompat.h"
#include <vector>

#include "CRaceBuffInfoByHolyQuest.h"

class CPlayer;

class __cppobj CRaceBuffInfoByHolyQuestfGroup
{
public:
  enum RESULT_TYPE : int
  {
    RESULT_WIN = 0,
    RESULT_WIN_NOSCANNER = 1,
    RESULT_FAIL = 2,
    RESULT_LOSE = 3,
    RESULT_TYPE_MAX = 4,
  };

  explicit CRaceBuffInfoByHolyQuestfGroup(unsigned int nth);
  bool Init();
  bool Apply(unsigned int iResultType, CPlayer *pkDest);
  bool Release(unsigned int iResultType, CPlayer *pkDest);
  bool CreateComplete(unsigned int iResultType, CPlayer *pkDest);
  static const char *TYPE_NAME[4];

  unsigned int m_uiNTh;
  std::vector<CRaceBuffInfoByHolyQuest *> m_vecInfo;
};
