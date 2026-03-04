#pragma once

#include "IdaCompat.h"

#include "CNetIndexList.h"
#include "GlobalObjectDefs.h"

class CPlayer;

struct  GMRequestData
{
  GMRequestData();
  void Set(unsigned int dwSerial, const char *pwszName, unsigned int dwTime);

  unsigned int m_dwPlayerSerial;
  unsigned int m_dwCallTimer;
  char m_wszUserName[17];
};

class  GMCallMgr
{
public:
  GMCallMgr();
  void Init();
  void InitReqBuff();
  GMRequestData *PopReqEmptNode();
  void PushReqNode(GMRequestData *pInst);
  GMRequestData *GetGMRequestDataPtr(CPlayer *pOne);
  bool RequestGMCall(CPlayer *pOne, int bCall);
  bool RequestGMList(CPlayer *pOne, int nCurrPageIndex);
  char RequestAcceptGMCall(CPlayer *pOne, unsigned int dwUserSerial);

  char SendResponseGMCall(CPlayer *pOne, int bCallState);
  char SendResponseGMList(CPlayer *pOne, int nCurrPageIndex);
  void SendResponseAcceptResult(CPlayer *pOneGM, CPlayer *pOneUser, int nErrorCode);

  GMRequestData m_buffGMRequestData[MAX_PLAYER];
  CNetIndexList m_listGMRequestDataEmpty;
  CNetIndexList m_listGMRequestDataTask;
};


