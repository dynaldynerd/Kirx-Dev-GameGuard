#pragma once

#include "IdaCompat.h"

struct _HSHIELD_CLIENT_CONTEXT
{
  void *pGameCrc;
  void *pHShieldCrc;
  void *pEngineCrc;
};

class HACKSHEILD_PARAM_ANTICP
{
public:
  HACKSHEILD_PARAM_ANTICP();

  void Init();
  void CheckClient();
  void Kick(unsigned __int8 byReason, unsigned int dwRet);
  bool IsLogPass();
  char OnCheckSession_FirstVerify(int n);
  void OnConnect(int nIndex);
  void OnDisConnect();
  void OnLoop();
  bool OnRecvSession(
    class CHackShieldExSystem *mgr,
    int nIndex,
    unsigned __int8 byProtocol,
    unsigned __int64 tSize,
    char *pMsg);
  char OnRecvSession_ServerCheckSum_Request(unsigned int nIndex);
  char OnRecvSession_ClientCheckSum_Response(unsigned __int64 tSize, char *pMsg);
  char OnRecvSession_ClientCrc_Response(unsigned __int64 tSize, char *pMsg);

  int m_nSocketIndex;
  unsigned int m_dwLastSyncQryTime;
  unsigned __int8 m_byVerifyState;
  _HSHIELD_CLIENT_CONTEXT m_CrcInfo;
  unsigned __int8 m_byGUIDClientInfo[20];
};

