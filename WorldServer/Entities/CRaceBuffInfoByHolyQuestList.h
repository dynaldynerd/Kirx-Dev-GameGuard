#pragma once

#include "IdaCompat.h"
#include <vector>

#include "CRaceBuffInfoByHolyQuestfGroup.h"

class CPlayer;

class __cppobj CRaceBuffInfoByHolyQuestList
{
public:
  CRaceBuffInfoByHolyQuestList();
  ~CRaceBuffInfoByHolyQuestList();

  bool Init();
  unsigned int GetMaxThCnt();
  bool Apply(unsigned int uiContinueCnt, int iResultType, CPlayer *pkDest);
  bool Release(unsigned int uiContinueCnt, int iResultType, CPlayer *pkDest);
  bool CreateComplete(unsigned int uiContinueCnt, int iResultType, CPlayer *pkDest);

  std::vector<CRaceBuffInfoByHolyQuestfGroup *> m_vecInfo;
};
