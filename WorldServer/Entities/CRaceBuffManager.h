#pragma once

#include "IdaCompat.h"

#include "CRaceBuffByHolyQuestProcedure.h"

class __cppobj CRaceBuffManager
{
public:
  static CRaceBuffManager *Instance();
  void Loop();
  bool Init();
  bool RequestHolyQuestRaceBuff(int iType);
  bool CreateComplete(CPlayer *pkPlayer);

  CRaceBuffByHolyQuestProcedure m_kBuffByHolyQuest;
};
