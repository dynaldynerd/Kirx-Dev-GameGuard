#pragma once

#include "IdaCompat.h"
#include "CMyTimer.h"
#include "CNetIndexList.h"

class CRecallRequest;
class CPlayer;

class __cppobj CRecallEffectController
{
public:
  unsigned int m_uiInfoTotCnt;
  CRecallRequest **m_ppkReqeust;
  CMyTimer *m_pkTimer;
  CNetIndexList *m_pkEmptyInxList;
  CNetIndexList *m_pkUseInxList;

  static CRecallEffectController *Instance();
  bool Init(unsigned int infoCount);
  void OnLoop();
  void UpdateClose();
  void SendRecallReqeustResult(char byRet, CPlayer *pkObj);
  void SendRecallReqeustToDest(unsigned __int16 wRequestID, CPlayer *pkPerformer, CPlayer *pkDest);
  void SendDecideRecallErrorResultToDest(char byErr, CPlayer *pkDest, int nCallerMapCode);
  void DecideRecall(unsigned __int16 dwRequestID, unsigned __int8 byAgree, CPlayer *pkObj);
  bool RequestRecall(CPlayer *pkPerformer, CPlayer *pkDest, bool bRecallParty, bool bStone, bool bBattleModeUse);
  static void Destroy();
};
