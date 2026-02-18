#pragma once

#include "IdaCompat.h"

#include "CNetIndexList.h"
#include "GlobalObjectDefs.h"

class CPlayer;

struct  __declspec(align(8)) GMRequestData
{
  unsigned int m_dwPlayerSerial;
  unsigned int m_dwCallTimer;
  char m_wszUserName[17];
};

class  GMCallMgr
{
public:
  char SendResponseGMCall(CPlayer *pOne, int bCallState);
  char SendResponseGMList(CPlayer *pOne, int nCurrPageIndex);
  void SendResponseAcceptResult(CPlayer *pOneGM, CPlayer *pOneUser, int nErrorCode);

  GMRequestData m_buffGMRequestData[MAX_PLAYER];
  CNetIndexList m_listGMRequestDataEmpty;
  CNetIndexList m_listGMRequestDataTask;
};

