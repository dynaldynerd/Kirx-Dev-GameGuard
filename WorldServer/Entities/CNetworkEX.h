#pragma once

#include "IdaCompat.h"

#include "CNetProcess.h"

class __cppobj __declspec(align(8)) CNetWorking
{
public:
  virtual ~CNetWorking() = default;

  _socket *GetSocket(unsigned int dwProID, unsigned int dwSocketIndex);
  void CloseSocket(unsigned int dwProID, unsigned int dwSocketIndex, bool bSlowClose);
  virtual bool ExpulsionSocket(unsigned int dwProID, unsigned int dwIndex, unsigned __int8 byReason, void *pvInfo);

  char m_szSystemName[128];
  unsigned int m_dwUseProcessNum;
  CLogFile m_LogFile;
  CNetProcess m_Process[4];
  CNetProcess *m_pProcess[4];
  bool m_bUseFG;
  char m_szServerName[33];
  char m_szLogPath[128];
};

class __cppobj __declspec(align(8)) CNetworkEX : public CNetWorking
{
public:
  unsigned int dwMonsterPosMiss;
  unsigned int dwPlayerPosMiss;
  unsigned __int8 m_byStatus[4];

  void SetPassablePacket(unsigned int dwProID, unsigned __int8 byHeader1, unsigned __int8 byHeader2);
};

extern CNetworkEX g_Network;
