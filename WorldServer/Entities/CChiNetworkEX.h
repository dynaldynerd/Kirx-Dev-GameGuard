#pragma once

#include "IdaCompat.h"
#include "CNetwork.h"
#include "CMyTimer.h"

class CPlayer;

class __cppobj CChiNetworkEX : public CNetwork
{
public:
  CChiNetworkEX();

  static CChiNetworkEX *Instance();
  __int64 Send(
    unsigned __int8 *pbyType,
    unsigned int dwSID,
    char *szMsg,
    unsigned __int16 nLen);
  void Send_Trans(CPlayer *pOne, int dwRet);
  void Send_ClienInform(CPlayer *pOne, unsigned __int16 wSize, char *pBuf);

  static CChiNetworkEX *ms_pInstance;

  char m_ip[16];
  unsigned __int16 m_port;
  CMyTimer m_kCheckApexLineTimer;
};

