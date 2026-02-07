#pragma once

#include "IdaCompat.h"

#include "CNetProcess.h"

class __cppobj __declspec(align(8)) CNetWorking
{
public:
  CNetWorking();
  virtual ~CNetWorking();

  _socket *GetSocket(unsigned int dwProID, unsigned int dwSocketIndex);
  void CloseSocket(unsigned int dwProID, unsigned int dwSocketIndex, bool bSlowClose);
  bool SetNetSystem(unsigned int dwUseProcessNum, _NET_TYPE_PARAM *pType, char *szSystemName, char *pszLogPath);
  void Release();
  int Connect(unsigned int dwProID, unsigned int dwSocketIndex, in_addr dwIP, unsigned __int16 wPort);
  void OnLoop();
  void OnLoop_Receipt();
  void ProcessLogFile(unsigned int dwProID, bool bRecv, bool bSend, bool bSystem);
  unsigned int GetCheckRecvTime(unsigned int dwProID, unsigned int dwSocketIndex);

  virtual void UserLoop();
  virtual bool DataAnalysis(
    unsigned int dwProID,
    unsigned int dwClientIndex,
    _MSG_HEADER *pMsgHeader,
    char *pMsg);
  virtual void AcceptClientCheck(unsigned int dwProID, unsigned int dwIndex, unsigned int dwSerial);
  virtual void CloseClientCheck(unsigned int dwProID, unsigned int dwIndex, unsigned int dwSerial);
  virtual bool ExpulsionSocket(unsigned int dwProID, unsigned int dwIndex, unsigned __int8 byReason, void *pvInfo);
  virtual void AnsyncConnectComplete(unsigned int dwProID, unsigned int dwIndex, int nResult);

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

  void UserLoop() override;
  void AcceptClientCheck(unsigned int dwProID, unsigned int dwIndex, unsigned int dwSerial) override;
  void CloseClientCheck(unsigned int dwProID, unsigned int dwIndex, unsigned int dwSerial) override;
  void AnsyncConnectComplete(unsigned int dwProID, unsigned int dwIndex, int nResult) override;

  void SetPassablePacket(unsigned int dwProID, unsigned __int8 byHeader1, unsigned __int8 byHeader2);
  void Close(unsigned int dwProID, unsigned int dwSocketIndex, bool bSlowClose, const char *pszLog);
};

extern CNetworkEX g_Network;
