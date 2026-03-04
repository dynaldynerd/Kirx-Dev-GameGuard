#pragma once

#include "IdaCompat.h"

#include "CRaceBuffByHolyQuestProcedure.h"

class  CRaceBuffManager
{
public:
  CRaceBuffManager();
  ~CRaceBuffManager();

  static CRaceBuffManager *Instance();
  static void Destroy();
  void Loop();
  bool Init();
  bool RequestHolyQuestRaceBuff(int iType);
  bool CreateComplete(CPlayer *pkPlayer);
  int GetRaceBuffLevel(CPlayer *pOne);
  int CancelPlayerRaceBuff(
    CPlayer *pkPlayer,
    CRaceBuffInfoByHolyQuestfGroup::RESULT_TYPE eReleaseType,
    unsigned int uiContinueCnt);

  CRaceBuffByHolyQuestProcedure m_kBuffByHolyQuest;

private:
  static CRaceBuffManager *ms_Instance;
};
