#pragma once

#include "IdaCompat.h"
#include "CMyTimer.h"
#include "CNetIndexList.h"

class CRecallRequest;
class CPlayer;
class CCharacter;

class __cppobj CRecallEffectController
{
public:
  static CRecallEffectController *ms_Instance;

  unsigned int m_uiInfoTotCnt;
  CRecallRequest **m_ppkReqeust;
  CMyTimer *m_pkTimer;
  CNetIndexList *m_pkEmptyInxList;
  CNetIndexList *m_pkUseInxList;

  ~CRecallEffectController();
  static CRecallEffectController *Instance();
  bool Init(unsigned int infoCount);
  void OnLoop();
  void UpdateClose();
  CRecallRequest *GetEmpty();
  void Close(CRecallRequest *pkRequest, bool bDone);
  unsigned __int8 GetResistedRecall(unsigned __int16 wID, CRecallRequest **pkRequest);
  char RequestRecall(CPlayer *pkPerformer, CPlayer *pkDest, bool bRecallParty, bool bStone, bool bBattleModeUse);
  unsigned __int8 ProcessRequestRecall(
    CPlayer *pkPerformer,
    CCharacter *pkDest,
    CRecallRequest **pkRequest,
    bool bRecallParty,
    bool bStone,
    bool bBattleModeUse);
  void SendRecallReqeustResult(char byRet, CPlayer *pkObj);
  void SendRecallReqeustToDest(unsigned __int16 wRequestID, CPlayer *pkPerformer, CPlayer *pkDest);
  void SendDecideRecallErrorResultToDest(char byErr, CPlayer *pkDest, int nCallerMapCode);
  void DecideRecall(unsigned __int16 dwRequestID, unsigned __int8 byAgree, CPlayer *pkObj);
  void CleanUp();
  static void Destroy();
};
