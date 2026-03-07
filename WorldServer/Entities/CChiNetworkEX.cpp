#include "pch.h"

#include "CChiNetworkEX.h"

#include <new>
#include <cstdlib>
#include <cstring>

#include "CAsyncLogger.h"
#include "CNetProcess.h"
#include "CPlayer.h"
#include "CUserDB.h"
#include "CChiNetworkEXLocalStructs.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "apex_id.h"
#include "apex_send_trans.h"
#include "Packet/ZoneAccountPacket.h"

CChiNetworkEX *CChiNetworkEX::ms_pInstance;

CChiNetworkEX::CChiNetworkEX() : CNetwork(), m_ip{}, m_port(0), m_kCheckApexLineTimer()
{
}

CChiNetworkEX::~CChiNetworkEX() = default;

CChiNetworkEX *CChiNetworkEX::Instance()
{
  if (!ms_pInstance)
  {
    ms_pInstance = new (std::nothrow) CChiNetworkEX();
  }
  return ms_pInstance;
}

void CChiNetworkEX::Destory()
{
  if (ms_pInstance)
  {
    delete ms_pInstance;
    ms_pInstance = nullptr;
  }
}

__int64 CChiNetworkEX::Initialize()
{
  if (!LoadINIFile())
  {
    return -1;
  }
  if (!LoadDll(const_cast<char *>("CHI_NETD.dll")))
  {
    return -2;
  }

  SetDataAnalysisFunc(s_DataAnalysis);
  InitNetwork();
  m_kCheckApexLineTimer.BeginTimer(10000u);
  return 0;
}

__int64 CChiNetworkEX::LoadINIFile()
{
  if (!GetPrivateProfileStringA("Apex_Setting", "Server", "127.0.0.1", m_ip, static_cast<DWORD>(sizeof(m_ip)), ".\\Initialize\\Apex.ini"))
  {
    return 0;
  }

  char portText[260]{};
  if (!GetPrivateProfileStringA("Apex_Setting", "Port", "0", portText, static_cast<DWORD>(sizeof(portText)), ".\\Initialize\\Apex.ini"))
  {
    return 0;
  }

  m_port = static_cast<unsigned __int16>(atoi(portText));
  return 1;
}

__int64 CChiNetworkEX::Send(
  unsigned __int8 *pbyType,
  unsigned int dwSID,
  char *szMsg,
  unsigned __int16 nLen)
{
  const int sendLength = nLen;
  if (!LoadSendMsg)
  {
    return 0;
  }

  return LoadSendMsg(0, 0, pbyType, dwSID, szMsg, static_cast<unsigned __int16>(sendLength));
}

bool __fastcall CChiNetworkEX::s_DataAnalysis(unsigned int dwProID, unsigned int dwClientIndex, _MSG_HEADER *pMsgHeader, char *pMsg)
{
  const ApexMessageHeader *header = reinterpret_cast<const ApexMessageHeader *>(pMsgHeader);
  const unsigned __int8 packetType = header->byType;
  CAsyncLogger::Instance()->FormatLog(12, "s_DataAnalysis - %c", packetType);

  if (packetType == 'D')
  {
    const unsigned int payloadSize = header->dwSize - sizeof(ApexMessageHeader);
    Instance()->Recv_ApexInform(header->dwSID, payloadSize, pMsg);
  }
  else if (packetType == 'K')
  {
    const unsigned int payloadSize = header->dwSize - sizeof(ApexMessageHeader);
    Instance()->Recv_ApexKill(header->dwSID, payloadSize, pMsg);
  }

  return true;
}

void CChiNetworkEX::AcceptClientCheck(unsigned int dwProID, unsigned int dwIndex, unsigned int dwSerial)
{
  // this is not a stub
}

void CChiNetworkEX::CloseClientCheck(unsigned int dwProID, unsigned int dwIndex, unsigned int dwSerial)
{
  if (CloseSocket)
  {
    CloseSocket(dwProID, dwIndex, false);
  }
}

void CChiNetworkEX::CheckApexLine()
{
  if (!m_kCheckApexLineTimer.CountingTimer())
  {
    return;
  }

  if (!GetSocket)
  {
    return;
  }

  _socket *socket = GetSocket(0, 0);
  if (!socket || socket->m_bAccept)
  {
    return;
  }

  OutputDebugLog("CChiNetworkEX::CheckApexLine()");
  if (Connect)
  {
    Connect(0, 0, inet_addr(m_ip), m_port);
  }
}

void CChiNetworkEX::Recv_ApexInform(unsigned int dwSID, unsigned int dwRecvSize, char *pMsg)
{
  CPlayer *player = GetPtrPlayerFromAccountSerial(g_Player, MAX_PLAYER, dwSID);
  if (player)
  {
    player->SendMsg_ApexInform(dwRecvSize, pMsg);
    CAsyncLogger::Instance()->FormatLog(12, "ApexMsg(D)..Recv_ApexInform - %d", dwSID);
  }
  else
  {
    CAsyncLogger::Instance()->FormatLog(12, "ApexMsg(D)..Recv_ApexInform - %d - pOne NULL", dwSID);
  }
}

void CChiNetworkEX::Recv_ApexKill(unsigned int dwSID, unsigned int dwRecvSize, char *pMsg)
{
  CPlayer *player = GetPtrPlayerFromAccountSerial(g_Player, MAX_PLAYER, dwSID);
  if (!player)
  {
    CAsyncLogger::Instance()->FormatLog(12, "ApexMsg(K)..Recv_ApexKill - !pOne:%d", dwSID);
  }

  const int action = *reinterpret_cast<int *>(pMsg);
  if (action == 1)
  {
    if (player)
    {
      player->m_pUserDB->ForceCloseCommand(12, 268435455, true, "ApexItemServer Kick");
      CAsyncLogger::Instance()->FormatLog(12, "APEX_USER_KICK - %d, CPlayer::s_nLiveNum : %d", dwSID, CPlayer::s_nLiveNum);
    }
    return;
  }

  if (action == 0)
  {
    Inform_For_Exit_By_ApexBlock(dwSID);
    CAsyncLogger::Instance()->FormatLog(12, "Inform_For_Exit_By_ApexBlock - %d", dwSID);
    if (player)
    {
      player->m_pUserDB->ForceCloseCommand(13, 268435455, true, "ApexItemServer Block");
    }
    return;
  }

  CAsyncLogger::Instance()->FormatLog(
    12,
    "ApexMsg(K)..Recv_ApexKill - %d, if(dwAction == 1 || dwAction == 0) else?? ",
    dwSID);
}

void CChiNetworkEX::Inform_For_Exit_By_ApexBlock(unsigned int dwAccountSerial)
{
  _apex_block_request_wrac request{};
  strcpy_s(request.szAccountID, sizeof(request.szAccountID), "Apex");
  request.dwAccountSerial = dwAccountSerial;
  request.dwIP = 0;

  unsigned __int8 pbyType[2]{1, 28};
  g_Network.m_pProcess[1]->LoadSendMsg(
    0,
    pbyType,
    reinterpret_cast<char *>(&request),
    static_cast<unsigned __int16>(sizeof(request)));
}

void CChiNetworkEX::Send_Trans(CPlayer *pOne, int dwRet)
{
  _apex_send_trans sendData{};
  sendData.m_nRet = dwRet;

  const unsigned __int16 packetLength = static_cast<unsigned __int16>(sendData.size());
  CUserDB *userDb = pOne->m_pUserDB;
  _apex_id packetType(82);
  Send(reinterpret_cast<unsigned __int8 *>(packetType.operator&()), userDb->m_dwAccountSerial, reinterpret_cast<char *>(&sendData), packetLength);

  CAsyncLogger::Instance()->FormatLog(12, "Send_Trans - %d", userDb->m_dwAccountSerial);
}

void CChiNetworkEX::Send_ClienInform(CPlayer *pOne, unsigned __int16 wSize, char *pBuf)
{
  const unsigned __int16 payloadLength = static_cast<unsigned __int16>(wSize - 4);
  CUserDB *userDb = pOne->m_pUserDB;
  _apex_id packetType(84);
  Send(reinterpret_cast<unsigned __int8 *>(packetType.operator&()), userDb->m_dwAccountSerial, pBuf, payloadLength);

  CAsyncLogger::Instance()->FormatLog(12, "Send_ClienInform - %d", userDb->m_dwAccountSerial);
}

void CChiNetworkEX::Send_IP(CPlayer *pOne)
{
  _apex_send_ip sendData{};
  sendData.m_byOnce = 1;
  sendData.m_dwIp = pOne->m_pUserDB->m_dwIP;

  const unsigned __int16 packetLength = 5;
  CUserDB *userDb = pOne->m_pUserDB;
  _apex_id packetType(83);
  Send(reinterpret_cast<unsigned __int8 *>(packetType.operator&()), userDb->m_dwAccountSerial, reinterpret_cast<char *>(&sendData), packetLength);

  CAsyncLogger::Instance()->FormatLog(12, "Send_IP - %d", userDb->m_dwAccountSerial);
}

void CChiNetworkEX::Send_Login(CPlayer *pOne)
{
  char sendData[13]{};
  memcpy_0(sendData, pOne->m_pUserDB->m_szAccountID, sizeof(sendData));

  const unsigned __int16 packetLength = 13;
  CUserDB *userDb = pOne->m_pUserDB;
  _apex_id packetType(76);
  Send(reinterpret_cast<unsigned __int8 *>(packetType.operator&()), userDb->m_dwAccountSerial, sendData, packetLength);

  CAsyncLogger::Instance()->FormatLog(12, "Send_Login - %d", userDb->m_dwAccountSerial);
}

void CChiNetworkEX::Send_Logout(CPlayer *pOne)
{
  if (!pOne->m_bLoad)
  {
    return;
  }

  char sendData[13]{};
  memcpy_0(sendData, pOne->m_pUserDB->m_szAccountID, sizeof(sendData));

  const unsigned __int16 packetLength = 13;
  CUserDB *userDb = pOne->m_pUserDB;
  _apex_id packetType(71);
  Send(reinterpret_cast<unsigned __int8 *>(packetType.operator&()), userDb->m_dwAccountSerial, sendData, packetLength);

  CAsyncLogger::Instance()->FormatLog(12, "Send_Logout - %d", userDb->m_dwAccountSerial);
}

