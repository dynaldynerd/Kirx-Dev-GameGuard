#include "pch.h"

#include "CChiNetworkEX.h"

#include <new>

#include "CAsyncLogger.h"
#include "CPlayer.h"
#include "CUserDB.h"
#include "GlobalObjects.h"
#include "apex_id.h"
#include "apex_send_trans.h"
#include "CChiNetworkEXLocalStructs.h"

CChiNetworkEX *CChiNetworkEX::ms_pInstance = nullptr;

CChiNetworkEX::CChiNetworkEX() : CNetwork(), m_ip{}, m_port(0), m_kCheckApexLineTimer()
{
}

CChiNetworkEX *CChiNetworkEX::Instance()
{
  if (!ms_pInstance)
  {
    ms_pInstance = new (std::nothrow) CChiNetworkEX();
  }
  return ms_pInstance;
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

void CChiNetworkEX::Inform_For_Exit_By_ApexBlock(unsigned int dwAccountSerial)
{

  ApexBlockRequest request{};
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
  _apex_id packetType(0x52u);
  Send(reinterpret_cast<unsigned __int8 *>(packetType.operator&()), userDb->m_dwAccountSerial, reinterpret_cast<char *>(&sendData), packetLength);

  CAsyncLogger::Instance()->FormatLog(12, "Send_Trans - %d", userDb->m_dwAccountSerial);
}

void CChiNetworkEX::Send_ClienInform(CPlayer *pOne, unsigned __int16 wSize, char *pBuf)
{
  const unsigned __int16 payloadLength = static_cast<unsigned __int16>(wSize - 4);
  CUserDB *userDb = pOne->m_pUserDB;
  _apex_id packetType(0x54u);
  Send(reinterpret_cast<unsigned __int8 *>(packetType.operator&()), userDb->m_dwAccountSerial, pBuf, payloadLength);

  CAsyncLogger::Instance()->FormatLog(12, "Send_ClienInform - %d", userDb->m_dwAccountSerial);
}

