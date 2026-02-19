#pragma once

#include "IdaCompat.h"

#include <winsock2.h>

#include "CLogFile.h"
#include "CNetCriticalSection.h"
#include "CNetIndexList.h"

struct _MSG_HEADER
{
  unsigned __int16 m_wSize;
  unsigned __int8 m_byType[2];
};

struct _SOCK_TYPE_PARAM
{
  unsigned __int8 m_byProtocolID;
  int m_bServer;
  unsigned __int16 m_wPort;
  unsigned __int16 m_wSocketMaxNum;
  int m_bAcceptIPCheck;
  unsigned int m_dwIPCheckTerm;
  unsigned int m_dwSocketRecycleTerm;

  _SOCK_TYPE_PARAM();
};

struct _total_count
{
  unsigned int m_dwAcceptNum;
  unsigned int m_dwSendAbleNum;
  long double m_dTotalRecvBYTESize;
  long double m_dTotalSendBYTESize;
  unsigned int m_dwTotalRecvErrNum;
  unsigned int m_dwTotalSendErrNum;
  unsigned int m_dwTotalSendBlockNum;
  unsigned int m_dwTotalRecvBlockNum;
  unsigned int m_dwBufferClean;
  unsigned int m_dwFdWriteCnt;
  unsigned int m_dwNoFindEmptySocket;
  unsigned int m_dwNoEventSelect;

  _total_count();
};

class  CNetTimer
{
public:
  CNetTimer();
  void BeginTimer(int dwTerm);
  bool CountingTimer();

  int m_nTickTerm;
  unsigned int m_dwTickOld;
  bool m_bOper;
};

struct _IP_CHECK_NODE
{
  unsigned int dwIP;
  unsigned int dwWaitStartTime;
};

struct _FORCE_CLOSE
{
  struct __FD_NODE
  {
    unsigned int dwObjIndex;
    unsigned int dwObjSerial;
    unsigned int dwEventCreateTime;
  };

  unsigned int m_dwNodeNum;
  __FD_NODE *m_pFDData;
  CNetTimer m_tmFD;
  CNetIndexList m_listFD;
  CNetIndexList m_listFDEmpty;

  _FORCE_CLOSE();
  ~_FORCE_CLOSE();
  bool Init(unsigned int dwNodeNum);
  bool PushNode(unsigned int dwIndex, unsigned int dwSerial);
};

struct _NET_TYPE_PARAM : _SOCK_TYPE_PARAM
{
  unsigned __int8 m_byRecvThreadNum;
  unsigned int m_dwSendBufferSize;
  unsigned int m_dwRecvBufferSize;
  bool m_bSendSafe;
  unsigned int m_dwSendSafeSize;
  int m_bSpeedHackCheck;
  int m_bRealSockCheck;
  int m_bSystemLogFile;
  int m_bRecvLogFile;
  int m_bSendLogFile;
  int m_bOddMsgWriteLog;
  int m_bOddMsgDisconnect;
  unsigned __int8 m_byRecvSleepTime;
  unsigned __int8 m_bySendSleepTime;
  int m_bSvrToS;
  int m_bAnSyncConnect;
  int m_bPassSendQueueFull;
  int m_bKeyCheck;
  unsigned int m_dwKeyCheckTerm;
  unsigned int m_dwProcessMsgNumPerLoop;
  char m_szModuleName[128];

  _NET_TYPE_PARAM();
};

struct _thread_parameter
{
  int m_bStart;
  void *m_pParam;
  int m_nIndex;

  _thread_parameter();
  void SetParameter(int bStart, void *pParam, int nIndex);
  void EndThread();
};

class  CNetFrameRate
{
public:
  unsigned int m_dwFrames;
  unsigned int m_dwFrameTime;
  unsigned int m_dwFrameCount;

  CNetFrameRate();
  void CalcFrameRate();
};

struct _NET_BUFFER
{
  __int64 GetLeftLoadSize();
  void AddPushPos(unsigned int dwAddSize);
  _NET_BUFFER();
  ~_NET_BUFFER();
  bool AllocBuffer(unsigned int nMaxSize, unsigned int nEtrSize, char *pTemp);
  char *GetPushPos();
  char *GetPopPoint(bool *pbMiss);
  char *GetSendPoint(int *pnSendSize, bool *pMiss);
  void AddPopPos(unsigned int dwAddSize);
  void Init();

  unsigned int m_nMaxSize;
  unsigned int m_nEtrSize;
  int m_dwPopRot;
  int m_dwPopPnt;
  int m_dwPushRot;
  int m_dwPushPnt;
  char *m_sMainBuffer;
  CNetCriticalSection m_csPush;
  CNetCriticalSection m_csPop;
  char *m_sTempBuffer;
};

struct _ANSYNC_CONNECT_DATA
{
  sockaddr_in Addr;
  int nResult;
};

struct _KEY_CHECK_NODE
{
  unsigned int dwSerial;
  unsigned int dwKey[4];
  unsigned int dwIP;
  unsigned int dwWaitStartTime;
};

struct  _socket
{
  unsigned __int64 m_Socket;
  int m_nIndex;
  unsigned int m_dwSerial;
  sockaddr_in m_Addr;
  char m_szID[20];
  bool m_bAccept;
  bool m_bSendable;
  bool m_bEnterCheck;
  unsigned int m_dwConnectTime;
  unsigned int m_dwLastCloseTime;
  unsigned int m_dwRealClientKeyValue[4];
  unsigned int m_dwTotalRecvMsg;
  unsigned int m_dwTotalSendMsg;
  unsigned int m_dwTotalRecvBlock;
  unsigned int m_dwBoundResponTerm;
  unsigned int m_dwSendBlockTime;
  unsigned int m_dwRecvPopMissTime;
  unsigned int m_dwSendSpeedHackTime;
  unsigned int m_dwResponSpeedHackTime;
  unsigned __int8 m_bySpeedHackContCount;
  unsigned __int8 m_bySpeedHackMissCount;
  unsigned int m_dwSpeedHackKey[4];
  unsigned int m_dwPingPongCount;
  unsigned int m_dwSpeedHackCount;
  unsigned int m_dwLastRecvTime;
  unsigned int m_dwLastSendTime;
  void *m_hFGContext;

  _socket();
  ~_socket();
  void InitParam();
};

class CNetWorking;

class  CNetSocket
{
public:
  CNetSocket();
  virtual ~CNetSocket();

  _socket *GetSocket(unsigned int dwIndex);
  bool SetSocket(_SOCK_TYPE_PARAM *pType, char *pszErrMsg);
  bool InitAcceptSocket(char *pszErrMsg);
  unsigned int Accept_Server();
  bool Accept_Client(unsigned int dwSocketIndex);
  int Connect(unsigned int n, sockaddr_in *pAddr);
  bool Send(unsigned int n, char *pBuf, int nSize, int *pnRet);
  bool Recv(unsigned int n, char *pBuf, int nBufMaxSize, int *pnRet);
  bool CloseSocket(unsigned int n);
  void CloseAll();
  void Release();
  unsigned int FindEmptySocket();
  void EmptySocketBuffer(unsigned int n);
  unsigned int GetSocketIPAddress(unsigned int dwIndex);
  _SOCK_TYPE_PARAM *GetSocketType();
  _total_count *GetTotalCount();
  void OnLoop();
  bool PushIPCheckList(unsigned int dwIP);

  unsigned __int64 m_sAccept;
  void *m_AcceptEvent;
  _socket *m_Socket;
  void **m_Event;
  _SOCK_TYPE_PARAM m_SockType;
  _total_count m_TotalCount;
  bool m_bSetSocket;
  unsigned int m_dwSerialCounter;
  int m_nIPCheckNodeNum;
  _IP_CHECK_NODE *m_ndIPCheck;
  unsigned int *m_dwIPCheckBufferList;
  CNetIndexList m_listIPCheck;
  CNetIndexList m_listIPCheck_Empty;
  CNetTimer m_tmrListCheckerIPCheck;
};

class  CNetProcess
{
public:
  CNetProcess();
  virtual ~CNetProcess();

  int LoadSendMsg(unsigned int dwClientIndex, unsigned __int8 *pbyType, char *szMsg, unsigned __int16 nLen);
  int LoadSendMsg(unsigned int dwClientIndex, unsigned __int16 wType, char *szMsg, unsigned __int16 nLen);
  bool SetProcess(int nIndex, _NET_TYPE_PARAM *pType, CNetWorking *pNetwork, bool bUseFG);
  void Release();
  void CloseAll();
  void CloseSocket(unsigned int dwSocketIndex, bool bSlowClose);
  void OnLoop();
  void OnLoop_Receipt();
  void LogFileOperSetting(bool bRecv, bool bSend, bool bSystem);
  void IOLogFileOperSetting(bool bOper);
  void *GetContextHandle(unsigned __int16 wIndex);
  void SetContextHandle(void *hContextHandle, unsigned __int16 wIndex);
  unsigned int GetSendThreadFrame();
  bool StartSpeedHackCheck(unsigned int dwClientIndex, char *pszID);
  bool PushAnsyncConnect(unsigned int dwSocketIndex, sockaddr_in *pAddr);
  void CompleteAnsyncConnect();
  bool PushKeyCheckList(unsigned int dwSerial, unsigned int dwIP, unsigned int *pdwKey, int nUseKeyNum);
  bool FindKeyFromWaitList(unsigned int dwSocketIndex, unsigned int dwSerial, unsigned int *pdwKey, int nUseKeyNum);
  bool wt_AcceptClient(unsigned int *pdwClientIndex);
  bool wt_CloseClient(unsigned int dwClientIndex);
  void _Receipt();
  void _PopRecvMsg(unsigned __int16 wSocketIndex);
  bool _InternalPacketProcess(unsigned int dwSocketIndex, _MSG_HEADER *pMsgHeader, char *pMsg);
  void _SendLoop(unsigned int n);
  void _ForceCloseLoop();
  void _CheckSend(unsigned __int16 wSocketIndex);
  void _CheckWaitKey();
  void _CkeckKeyCertifyDeley();
  void _CkeckRecvBreak();
  void _CkeckSpeedHackDeley();
  void _ResponSpeedHack();
  void _SendSpeedHackCheckMsg(unsigned int n);
  static void AcceptThread(void *pv);
  static void NetEventThread(void *pv);
  static void RecvThread(void *pv);
  static void SendThread(void *pv);
  static void ConnectThread(void *pv);
  void PushCloseNode(unsigned int nIndex);

  CNetSocket m_NetSocket;
  _FORCE_CLOSE m_FC;
  _NET_TYPE_PARAM m_Type;
  int m_nIndex;
  int m_nOddMsgNum;
  unsigned int m_nTryConnectCount;
  bool m_bPassablePacket[256][256];
  bool m_bUseFG;
  bool m_bSetProcess;
  int m_nEventThreadNum;
  CNetWorking *m_pNetwork;
  CLogFile m_LogFile[3];
  CLogFile m_LogHack;
  CNetCriticalSection *m_csRecv;
  _NET_BUFFER *m_pRecvBuffer;
  _NET_BUFFER *m_pSendBuffer;
  _thread_parameter m_pmAcceptThread;
  _thread_parameter m_pmEventThread[81];
  _thread_parameter m_pmRecvThread[8];
  _thread_parameter m_pmSendThread;
  _thread_parameter m_pmConnectThread;
  CNetIndexList m_listAcceptEvent;
  CNetIndexList m_listCloseEvent;
  CNetIndexList m_listRecvEvent;
  CNetIndexList m_listAcceptReady;
  _ANSYNC_CONNECT_DATA *m_AnsyncConnectData;
  CNetIndexList m_listAnsyncConnect;
  CNetIndexList m_listAnsyncConnectComplete;
  CNetFrameRate m_fpsSendThread;
  int m_nKeyCheckNodeNum;
  _KEY_CHECK_NODE *m_ndKeyCheck;
  unsigned int *m_dwKeyCheckBufferList;
  CNetIndexList m_listKeyCheck;
  CNetIndexList m_listKeyCheck_Empty;
  CNetTimer m_tmrListCheckerKeyCheck;
  CNetTimer m_tmrCheckResSH;
  CNetTimer m_tmrCheckKeyCT;
  CNetTimer m_tmrCheckRecvBreak;
  char *m_sTempSendBuffer;
  char *m_sTempRecvBuffer;
  unsigned int m_dwCurTime;
};

