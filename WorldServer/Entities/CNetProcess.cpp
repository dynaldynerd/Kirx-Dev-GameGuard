#include "pch.h"

#include "CNetProcess.h"

#include <mmsystem.h>

#include "CCRFG_SEND_BUFFER.h"
#include "CNetworkEX.h"

#pragma comment(lib, "winmm.lib")

_CCRFG_SEND_BUFFER g_FGSendData{};

extern "C" int __fastcall _CcrFG_rs_EncryptPacket(void *context, unsigned __int8 *buffer, int size)
{
  (void)context;
  (void)buffer;
  return size;
}

extern "C" unsigned int __fastcall _CcrFG_rs_GetLastError()
{
  return 0;
}

_socket *CNetSocket::GetSocket(unsigned int dwIndex)
{
  return &this->m_Socket[dwIndex];
}

CNetTimer::CNetTimer()
{
  m_bOper = false;
  m_dwTickOld = 0;
}

void CNetTimer::BeginTimer(int dwTerm)
{
  m_bOper = true;
  m_nTickTerm = dwTerm;
  m_dwTickOld = timeGetTime();
}

bool CNetTimer::CountingTimer()
{
  if (!m_bOper)
    return false;

  const DWORD now = timeGetTime();
  const int elapsed = static_cast<int>(now - m_dwTickOld);
  if (elapsed <= m_nTickTerm)
  {
    if (elapsed < 0)
      m_dwTickOld = 0;
    return false;
  }

  m_dwTickOld = now;
  return true;
}

__int64 _NET_BUFFER::GetLeftLoadSize()
{
  this->m_csPop.Lock();
  this->m_csPush.Lock();
  unsigned int left =
    this->m_dwPushPnt - this->m_dwPopPnt + this->m_nMaxSize * (this->m_dwPushRot - this->m_dwPopRot);
  this->m_csPop.Unlock();
  this->m_csPush.Unlock();
  return left;
}

void _NET_BUFFER::AddPushPos(unsigned int dwAddSize)
{
  this->m_csPush.Lock();
  unsigned int next = dwAddSize + this->m_dwPushPnt;
  if (next < this->m_nMaxSize)
  {
    this->m_dwPushPnt = next;
  }
  else
  {
    this->m_dwPushPnt = next % this->m_nMaxSize;
    ++this->m_dwPushRot;
  }
  this->m_csPush.Unlock();
}

bool _FORCE_CLOSE::PushNode(unsigned int dwIndex, unsigned int dwSerial)
{
  unsigned int outIndex = 0;
  if (!this->m_listFDEmpty.PopNode_Front(&outIndex))
    return false;

  _FORCE_CLOSE::__FD_NODE *node = &this->m_pFDData[outIndex];
  node->dwEventCreateTime = timeGetTime();
  node->dwObjIndex = dwIndex;
  node->dwObjSerial = dwSerial;
  this->m_listFD.PushNode_Back(outIndex);
  return true;
}

void CNetProcess::PushCloseNode(unsigned int nIndex)
{
  _socket *Socket = this->m_NetSocket.GetSocket(nIndex);
  if (Socket->m_bAccept && !this->m_listCloseEvent.IsInList(nIndex))
    this->m_listCloseEvent.PushNode_Back(nIndex);
}

int CNetProcess::LoadSendMsg(
  unsigned int dwClientIndex,
  unsigned __int8 *pbyType,
  char *szMsg,
  unsigned __int16 nLen)
{
  _socket *Socket = this->m_NetSocket.GetSocket(dwClientIndex);
  if (!Socket || !Socket->m_bAccept)
    return 0;

  if (!Socket->m_bSendable && this->m_bPassablePacket[pbyType[0]][pbyType[1]])
    return 0;

  _NET_BUFFER *sendBuffer = &this->m_pSendBuffer[dwClientIndex];
  if (this->m_Type.m_bSendSafe
    && sendBuffer->GetLeftLoadSize() > this->m_Type.m_dwSendSafeSize
    && this->m_bPassablePacket[pbyType[0]][pbyType[1]])
  {
    return 0;
  }

  unsigned __int16 Src[8]{};
  Src[1] = *reinterpret_cast<unsigned __int16 *>(pbyType);
  Src[0] = static_cast<unsigned __int16>(nLen + 4);

  if (this->m_nIndex || !this->m_bUseFG)
  {
    unsigned int remain = sendBuffer->m_nEtrSize + sendBuffer->m_nMaxSize - sendBuffer->m_dwPushPnt;
    if (Src[0] > remain)
    {
      this->m_LogFile[2].Write(
        "LoadSendMsg ID(%s) Type(%u,%u) Len(%u) Remain(%u)",
        Socket->m_szID,
        pbyType[0],
        pbyType[1],
        nLen,
        remain);
      return 0;
    }

    unsigned int left = static_cast<unsigned int>(sendBuffer->GetLeftLoadSize());
    if (static_cast<unsigned int>(Src[0]) + left > sendBuffer->m_nMaxSize)
    {
      this->m_pNetwork->ExpulsionSocket(this->m_nIndex, dwClientIndex, 3, nullptr);
      return 0;
    }

    memcpy_0(&sendBuffer->m_sMainBuffer[sendBuffer->m_dwPushPnt], Src, 4uLL);
    if (nLen)
      memcpy_0(&sendBuffer->m_sMainBuffer[sendBuffer->m_dwPushPnt + 4], szMsg, nLen);

    int tail = sendBuffer->m_nMaxSize - sendBuffer->m_dwPushPnt;
    if (tail < Src[0])
      memcpy_0(sendBuffer->m_sMainBuffer, &sendBuffer->m_sMainBuffer[sendBuffer->m_nMaxSize], Src[0] - tail);

    sendBuffer->AddPushPos(Src[0]);
    Socket->m_dwLastSendTime = this->m_dwCurTime;
    return 1;
  }

  if (Src[0] > 0x4E20u)
  {
    this->m_LogFile[2].Write(
      "LoadSendMsg ID(%s) Type(%u,%u) Len(%u) Max(%u)",
      Socket->m_szID,
      pbyType[0],
      pbyType[1],
      nLen,
      20000u);
    return 0;
  }

  memcpy_0(&g_FGSendData.sSendBuffer[2], Src, 4uLL);
  g_FGSendData.wMsgSize = 6;
  if (nLen)
  {
    g_FGSendData.wMsgSize += nLen;
    memcpy_0(&g_FGSendData.sSendBuffer[6], szMsg, nLen);
  }
  memcpy_0(g_FGSendData.sSendBuffer, &g_FGSendData.wMsgSize, 2uLL);

  int fgSize = _CcrFG_rs_EncryptPacket(
    Socket->m_hFGContext,
    reinterpret_cast<unsigned __int8 *>(g_FGSendData.sSendBuffer),
    20000);
  if (fgSize < 1)
  {
    if (fgSize)
    {
      unsigned int lastError = _CcrFG_rs_GetLastError();
      this->m_LogFile[2].Write(
        "SendMsg Error : _CcrFG_rs_EncryptPacket : nFGSize(%#x), _CcrFG_rs_GetLastError(%#x)",
        fgSize,
        lastError);
    }
    else
    {
      this->m_LogFile[2].Write(
        "SendMsg Warning : _CcrFG_rs_EncryptPacket : Before RecvMsg OverTime");
    }
  }

  memcpy_0(g_FGSendData.sSendBuffer, &fgSize, 2uLL);

  unsigned int remain = sendBuffer->m_nEtrSize + sendBuffer->m_nMaxSize - sendBuffer->m_dwPushPnt;
  if (static_cast<unsigned int>(fgSize) > remain)
  {
    this->m_LogFile[2].Write(
      "LoadSendMsg ID(%s) FGSize(%u) Type(%u,%u) Len(%u) Remain(%u)",
      Socket->m_szID,
      static_cast<unsigned int>(fgSize),
      pbyType[0],
      pbyType[1],
      nLen,
      remain);
    return 0;
  }

  unsigned int left = static_cast<unsigned int>(sendBuffer->GetLeftLoadSize());
  if (static_cast<unsigned int>(fgSize) + left > sendBuffer->m_nMaxSize)
  {
    this->m_pNetwork->ExpulsionSocket(this->m_nIndex, dwClientIndex, 3, nullptr);
    return 0;
  }

  memcpy_0(&sendBuffer->m_sMainBuffer[sendBuffer->m_dwPushPnt], g_FGSendData.sSendBuffer, fgSize);
  int tail = sendBuffer->m_nMaxSize - sendBuffer->m_dwPushPnt;
  if (tail < fgSize)
    memcpy_0(sendBuffer->m_sMainBuffer, &sendBuffer->m_sMainBuffer[sendBuffer->m_nMaxSize], fgSize - tail);

  sendBuffer->AddPushPos(static_cast<unsigned int>(fgSize));
  Socket->m_dwLastSendTime = this->m_dwCurTime;
  return 1;
}
