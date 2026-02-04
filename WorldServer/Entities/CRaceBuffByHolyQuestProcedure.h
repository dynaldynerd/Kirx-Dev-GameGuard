#pragma once

#include "IdaCompat.h"

#include "CUnmannedTraderRequestLimiter.h"
#include "CRaceBuffHolyQuestResultInfo.h"
#include "CRaceBuffInfoByHolyQuestList.h"

class CPlayer;

class __cppobj CRaceBuffByHolyQuestProcedure
{
public:
  using REQUEST_TYPE = CUnmannedTraderRequestLimiter::REQUEST_TYPE;

  CRaceBuffByHolyQuestProcedure();
  ~CRaceBuffByHolyQuestProcedure();
  bool Init();
  bool Request(int iType);
  void Loop();
  bool CreateComplete(CPlayer *pkPlayer);
  int CancelPlayerRaceBuff(
    CPlayer *pkPlayer,
    CRaceBuffInfoByHolyQuestfGroup::RESULT_TYPE eReleaseType,
    unsigned int uiReleaseLv);
  int GetRaceBuffLevel(CPlayer *pOne);

  REQUEST_TYPE m_eState;
  unsigned int m_uiProccessIndex;
  CRaceBuffHolyQuestResultInfo m_kBuffHolyQestResultInfo;
  CRaceBuffInfoByHolyQuestList m_kBuffInfo;

private:
  bool RequestSubProcCancelRaceBuff();
  bool RequestSubProcSetRaceBattleResult();
  bool RequestSubProcSetRaceBuff();
  void LoopSubProcSetRaceBuff();
};
