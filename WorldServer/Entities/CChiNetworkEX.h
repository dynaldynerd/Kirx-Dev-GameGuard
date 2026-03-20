#pragma once

#include "IdaCompat.h"
#include "CNetwork.h"
#include "CMyTimer.h"

class CPlayer;

class  CChiNetworkEX : public CNetwork
{
public:
  CChiNetworkEX();
  ~CChiNetworkEX() override;

  static CChiNetworkEX *Instance();
  static void Destory();
  int Initialize();
  int LoadINIFile();
  int Send(
    unsigned __int8 *pbyType,
    unsigned int dwSID,
    char *szMsg,
    unsigned __int16 nLen);
  static bool __fastcall s_DataAnalysis(unsigned int dwProID, unsigned int dwClientIndex, _MSG_HEADER *pMsgHeader, char *pMsg);
  void AcceptClientCheck(unsigned int dwProID, unsigned int dwIndex, unsigned int dwSerial);
  void CloseClientCheck(unsigned int dwProID, unsigned int dwIndex, unsigned int dwSerial);
  void CheckApexLine();
  void Recv_ApexInform(unsigned int dwSID, unsigned int dwRecvSize, char *pMsg);
  void Recv_ApexKill(unsigned int dwSID, unsigned int dwRecvSize, char *pMsg);
  void Inform_For_Exit_By_ApexBlock(unsigned int dwAccountSerial);
  void Send_IP(CPlayer *pOne);
  void Send_Login(CPlayer *pOne);
  void Send_Logout(CPlayer *pOne);
  void Send_Trans(CPlayer *pOne, int dwRet);
  void Send_ClienInform(CPlayer *pOne, unsigned __int16 wSize, char *pBuf);

  static CChiNetworkEX *ms_pInstance;

  char m_ip[16];
  unsigned __int16 m_port;
  CMyTimer m_kCheckApexLineTimer;
};

