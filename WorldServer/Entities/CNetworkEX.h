#pragma once

#include "IdaCompat.h"

#include "CNetProcess.h"

struct _CLID;

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
  bool DataAnalysis(
    unsigned int dwProID,
    unsigned int dwClientIndex,
    _MSG_HEADER *pMsgHeader,
    char *pMsg) override;
  void AcceptClientCheck(unsigned int dwProID, unsigned int dwIndex, unsigned int dwSerial) override;
  void CloseClientCheck(unsigned int dwProID, unsigned int dwIndex, unsigned int dwSerial) override;
  void AnsyncConnectComplete(unsigned int dwProID, unsigned int dwIndex, int nResult) override;

  void SetPassablePacket(unsigned int dwProID, unsigned __int8 byHeader1, unsigned __int8 byHeader2);
  void Close(unsigned int dwProID, unsigned int dwSocketIndex, bool bSlowClose, const char *pszLog);

  bool ClientLineAnalysis(unsigned int n, _MSG_HEADER *pMsgHeader, char *pMsg);
  bool AccountLineAnalysis(unsigned int n, _MSG_HEADER *pMsgHeader, char *pMsg);
  bool BillingLineAnalysis(int n, _MSG_HEADER *pMsgHeader, char *pMsg);
  bool WebAgentLineAnalysis(int n, _MSG_HEADER *pMsgHeader, char *pMsg);

  bool OpenWorldSuccessResult(unsigned int n, char *pMsg);
  bool OpenWorldFailureResult(unsigned int n, char *pMsg);
  bool ForceCloseCommand(unsigned int n, _CLID *pMsg);
  bool TransAccountInform(unsigned int n, char *pMsg);
  bool EnterWorldResult(unsigned int n, _CLID *pMsg);
  bool UILockInitResult(unsigned int n, char *pMsg);
  bool UILockUpdateResult(unsigned int n, char *pMsg);
  bool UILockRefreshResult(unsigned int n, char *pMsg);
  bool CheckIsBlockIPResult(unsigned int n, char *pMsg);
  bool ConEventTotalSalesCheck(int n, char *pBuf);
  bool DisconnectGuildWarCharacterRequest(int n, char *pBuf);
  bool ManageClientLimitRunRequest(char *pBuf);
  bool ManageClientForceExitRequest();
  bool CashDBInfoRecvResult(int n, char *pBuf);
  bool WorldServiceInform(unsigned int n, bool *pMsg);
  bool WorldExitInform(unsigned int n, char *pMsg);
  bool WorldMsgInform(unsigned int n, char *pMsg);
  bool ChatLockCommand(unsigned int n, _CLID *pMsg);

  bool BillingCloseRequest(int n, char *pBuf);
  bool BillingRemaintimePersonal(int n, char *pBuf);
  bool BillingRemaintimePCBang(int n, char *pBuf);
  bool BillingChangeType(int n, char *pBuf);
  bool BillingExpirePersonal(int n, char *pBuf);
  bool BillingExpirePCBang(int n, char *pBuf);
  bool BillingExpireIPOverflow(int n, char *pBuf);
  bool BillingDestroyModule(int n, char *pBuf);
  bool TaiwanBillingUserCertify(int n, char *pBuf);
  bool ChinaBillingChangePrimium(int n, char *pBuf);
  bool ZoneAliveCheckRequest(int n, char *pBuf);

  bool LogInWebAgentServer(unsigned int n, char *pBuf);
  bool SendRaceBossMsgFromWebRequest(int n, char *pBuf);
  bool LogInControllServer(unsigned int n, char *pBuf);
  bool CancelRaceBossSMSMsg(int n, char *pBuf);
  bool ConnectionStatusRequest(int n);
};

extern CNetworkEX g_Network;
