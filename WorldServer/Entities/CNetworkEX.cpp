#include "pch.h"

#include "CNetworkEX.h"

CNetworkEX g_Network{};

_socket *CNetWorking::GetSocket(unsigned int dwProID, unsigned int dwSocketIndex)
{
  if (dwProID < this->m_dwUseProcessNum)
    return this->m_Process[dwProID].m_NetSocket.GetSocket(dwSocketIndex);
  return nullptr;
}

void CNetWorking::CloseSocket(unsigned int dwProID, unsigned int dwSocketIndex, bool bSlowClose)
{
  if (!bSlowClose)
  {
    this->m_Process[dwProID].PushCloseNode(dwSocketIndex);
    return;
  }

  _socket *Socket = this->m_Process[dwProID].m_NetSocket.GetSocket(dwSocketIndex);
  if (!this->m_Process[dwProID].m_FC.PushNode(dwSocketIndex, Socket->m_dwSerial))
    this->m_Process[dwProID].PushCloseNode(dwSocketIndex);
}

bool CNetWorking::ExpulsionSocket(unsigned int dwProID, unsigned int dwIndex, unsigned __int8 byReason, void *pvInfo)
{
  (void)byReason;
  (void)pvInfo;
  this->CloseSocket(dwProID, dwIndex, false);
  return true;
}

void CNetworkEX::SetPassablePacket(unsigned int dwProID, unsigned __int8 byHeader1, unsigned __int8 byHeader2)
{
  if (dwProID >= 4)
  {
    return;
  }
  if (m_pProcess[dwProID] == nullptr)
  {
    return;
  }
  m_pProcess[dwProID]->m_bPassablePacket[byHeader1][byHeader2] = true;
}

void CNetworkEX::Close(unsigned int dwProID, unsigned int dwSocketIndex, bool bSlowClose, const char *pszLog)
{
  if (pszLog)
  {
    m_LogFile.Write(pszLog);
  }
  CloseSocket(dwProID, dwSocketIndex, bSlowClose);
}
