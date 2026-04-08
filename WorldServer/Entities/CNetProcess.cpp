#include "pch.h"

#include "WorldServerUtil.h"

#include "CNetProcess.h"

#include "CNetworkEX.h"

#include <mmsystem.h>
#include <process.h>
#include <cstdarg>
#include <cstdlib>
#include <new>
#include <limits>
#include <string>

#include "NetCheckPackets.h"
#include "NetUtil.h"
#include "VCryptor.h"

#pragma comment(lib, "winmm.lib")

namespace
{
  constexpr unsigned int kCryptPacketBufferSize = 20000;

  bool IsMulOk(size_t count, size_t size)
  {
    if (count == 0 || size == 0)
    {
      return true;
    }
    return count <= (std::numeric_limits<size_t>::max)() / size;
  }

  template <typename T>
  T *AllocateVector(size_t count)
  {
    size_t bytes = IsMulOk(count, sizeof(T)) ? count * sizeof(T) : static_cast<size_t>(-1);
    if (bytes > (std::numeric_limits<size_t>::max)() - 8)
    {
      bytes = static_cast<size_t>(-1);
    }
    const size_t total = bytes + 8;
    auto *raw = static_cast<unsigned int *>(::operator new(total));
    raw[0] = static_cast<unsigned int>(count);
    raw[1] = 0;
    auto *data = static_cast<T *>(static_cast<void *>(&raw[2]));
    for (size_t i = 0; i < count; ++i)
    {
      new (data + i) T();
    }
    return data;
  }

  template <typename T>
  void DestroyVector(T *data)
  {
    if (!data)
    {
      return;
    }
    auto *raw = reinterpret_cast<unsigned int *>(data) - 2;
    const size_t count = raw[0];
    for (size_t i = 0; i < count; ++i)
    {
      data[i].~T();
    }
    ::operator delete(raw);
  }

  _check_query sQry{};
  unsigned __int8 sbyQryType[2] = {101, 1};

  void WritePacketLogLine(CLogFile &logFile, const char *format, ...)
  {
    va_list args;
    va_start(args, format);
    logFile.WriteFromArg(format, args);
    va_end(args);
  }

  void BuildPacketHex(std::string &out, const char *payload, unsigned int length)
  {
    out.clear();
    if (!payload || !length)
    {
      out = "-";
      return;
    }

    out.reserve(static_cast<size_t>(length) * 3u);
    static const char kHex[] = "0123456789ABCDEF";
    for (unsigned int i = 0; i < length; ++i)
    {
      const unsigned char value = static_cast<unsigned char>(payload[i]);
      out.push_back(kHex[value >> 4]);
      out.push_back(kHex[value & 0x0F]);
      if (i + 1 < length)
      {
        out.push_back(' ');
      }
    }
  }

  void WriteCryptFrameHeader(char *buffer, unsigned __int16 frameSize, unsigned __int8 padSize)
  {
    std::memcpy(buffer, &frameSize, sizeof(frameSize));
    buffer[2] = static_cast<char>(padSize);
  }

  void AppendPacketSnifferLogSend(
    CNetProcess *process,
    unsigned int socketIndex,
    unsigned __int8 processIndex,
    unsigned __int8 type0,
    unsigned __int8 type1,
    unsigned __int16 payloadLen,
    const char *payload)
  {
    if (!process || !process->m_pNetwork || processIndex != 0 || type0 >= 100u)
    {
      return;
    }

    if (!payload || !payloadLen)
    {
      WritePacketLogLine(
        process->m_pNetwork->m_LogFile,
        "PKT SEND prc(%u) idx(%u) type(%u,%u) len(%u) packet(-)",
        static_cast<unsigned int>(processIndex),
        socketIndex,
        static_cast<unsigned int>(type0),
        static_cast<unsigned int>(type1),
        static_cast<unsigned int>(payloadLen));
      return;
    }

    std::string packetHex;
    BuildPacketHex(packetHex, payload, static_cast<unsigned int>(payloadLen));
    WritePacketLogLine(
      process->m_pNetwork->m_LogFile,
      "PKT SEND prc(%u) idx(%u) type(%u,%u) len(%u) packet(%s)",
      static_cast<unsigned int>(processIndex),
      socketIndex,
      static_cast<unsigned int>(type0),
      static_cast<unsigned int>(type1),
      static_cast<unsigned int>(payloadLen),
      packetHex.c_str());
  }
}

_SOCK_TYPE_PARAM::_SOCK_TYPE_PARAM()
{
  this->m_bServer = 0;
  this->m_bAcceptIPCheck = 0;
  this->m_byProtocolID = 0;
  this->m_wSocketMaxNum = 1;
  this->m_dwIPCheckTerm = 120000;
  this->m_dwSocketRecycleTerm = 0;
}

_total_count::_total_count()
{
  std::memset(this, 0, sizeof(_total_count));
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
  {
    return false;
  }

  const DWORD now = timeGetTime();
  const int elapsed = static_cast<int>(now - m_dwTickOld);
  if (elapsed <= m_nTickTerm)
  {
    if (elapsed < 0)
    {
      m_dwTickOld = 0;
    }
    return false;
  }

  m_dwTickOld = now;
  return true;
}

_FORCE_CLOSE::_FORCE_CLOSE()
  : m_dwNodeNum(0),
    m_pFDData(nullptr)
{
}

_FORCE_CLOSE::~_FORCE_CLOSE()
{
  if (m_pFDData)
  {
    operator delete(m_pFDData);
    m_pFDData = nullptr;
  }
}

bool _FORCE_CLOSE::Init(unsigned int dwNodeNum)
{
  if (!dwNodeNum || dwNodeNum > 65535)
  {
    return false;
  }

  m_dwNodeNum = dwNodeNum;
  m_pFDData = static_cast<__FD_NODE *>(operator new(saturated_mul(dwNodeNum, 12)));
  m_tmFD.BeginTimer(2500);
  m_listFD.SetList(dwNodeNum);
  m_listFDEmpty.SetList(dwNodeNum);
  for (unsigned int index = 0; index < dwNodeNum; ++index)
  {
    m_listFDEmpty.PushNode_Back(index);
  }
  return true;
}

bool _FORCE_CLOSE::PushNode(unsigned int dwIndex, unsigned int dwSerial)
{
  unsigned int outIndex[5]{};
  if (!m_listFDEmpty.PopNode_Front(outIndex))
  {
    return false;
  }

  __FD_NODE *node = &m_pFDData[outIndex[0]];
  node->dwEventCreateTime = timeGetTime();
  node->dwObjIndex = dwIndex;
  node->dwObjSerial = dwSerial;
  m_listFD.PushNode_Back(outIndex[0]);
  return true;
}

_NET_TYPE_PARAM::_NET_TYPE_PARAM()
{
  _SOCK_TYPE_PARAM::_SOCK_TYPE_PARAM();
  m_bRealSockCheck = 0;
  m_bSystemLogFile = 0;
  m_bRecvLogFile = 0;
  m_bSendLogFile = 0;
  m_bOddMsgWriteLog = 0;
  m_bOddMsgDisconnect = 0;
  m_bSpeedHackCheck = 0;
  m_bKeyCheck = 0;
  m_bSvrToS = 0;
  m_bAnSyncConnect = 0;
  m_szModuleName[0] = 0;
  m_byRecvThreadNum = 2;
  m_byRecvSleepTime = 1;
  m_bySendSleepTime = 1;
  m_dwKeyCheckTerm = 120000;
  m_dwProcessMsgNumPerLoop = static_cast<unsigned int>(-1);
  m_dwSendBufferSize = 10000;
  m_dwRecvBufferSize = 10000;
  m_bPassSendQueueFull = 0;
  m_bSendSafe = 0;
  m_CryptType = 1;
  m_pVCryptParam = nullptr;
}

_thread_parameter::_thread_parameter()
{
  m_bStart = 0;
  m_pParam = nullptr;
  m_nIndex = -1;
}

void _thread_parameter::SetParameter(int bStart, void *pParam, int nIndex)
{
  m_bStart = bStart;
  m_pParam = pParam;
  m_nIndex = nIndex;
}

void _thread_parameter::EndThread()
{
  m_bStart = 0;
}

CNetFrameRate::CNetFrameRate()
{
  m_dwFrameCount = 0;
}

void CNetFrameRate::CalcFrameRate()
{
  const DWORD now = timeGetTime();
  const int elapsed = static_cast<int>(now - m_dwFrameTime);
  ++m_dwFrameCount;
  if (elapsed <= 1000)
  {
    if (elapsed < 0)
    {
      m_dwFrameTime = now;
      m_dwFrameCount = 0;
    }
  }
  else
  {
    m_dwFrames = 1000 * m_dwFrameCount / elapsed;
    m_dwFrameTime = now;
    m_dwFrameCount = 0;
  }
}

_NET_BUFFER::_NET_BUFFER()
  : m_nMaxSize(0),
    m_nEtrSize(0),
    m_sMainBuffer(nullptr),
    m_sTempBuffer(nullptr)
{
  Init();
}

_NET_BUFFER::~_NET_BUFFER()
{
  if (m_sMainBuffer)
  {
    operator delete(m_sMainBuffer);
    m_sMainBuffer = nullptr;
  }
}

bool _NET_BUFFER::AllocBuffer(unsigned int nMaxSize, unsigned int nEtrSize, char *pTemp)
{
  if (m_sMainBuffer)
  {
    return false;
  }

  m_nMaxSize = nMaxSize;
  m_nEtrSize = nEtrSize;
  m_sTempBuffer = pTemp;
  m_sMainBuffer = static_cast<char *>(operator new(m_nEtrSize + m_nMaxSize));
  return true;
}

char *_NET_BUFFER::GetPushPos()
{
  return &m_sMainBuffer[m_dwPushPnt];
}

char *_NET_BUFFER::GetPopPoint(
  bool *pbMiss,
  bool bCryptFrame,
  unsigned __int16 *pwFrameSize,
  unsigned __int16 *pwPayloadSize)
{
  const int left = static_cast<int>(GetLeftLoadSize());
  const unsigned int popPoint = m_dwPopPnt;
  const unsigned __int16 headerSize = bCryptFrame ? 3 : 2;

  *pbMiss = false;
  if (pwFrameSize)
  {
    *pwFrameSize = 0;
  }
  if (pwPayloadSize)
  {
    *pwPayloadSize = 0;
  }

  if (!left)
  {
    return nullptr;
  }

  if (static_cast<unsigned int>(left) < headerSize)
  {
    *pbMiss = true;
    return nullptr;
  }

  const unsigned __int8 sizeLow = static_cast<unsigned __int8>(m_sMainBuffer[popPoint]);
  const unsigned __int8 sizeHigh = static_cast<unsigned __int8>(m_sMainBuffer[(popPoint + 1) % m_nMaxSize]);
  const unsigned __int16 size =
    static_cast<unsigned __int16>(sizeLow | (static_cast<unsigned __int16>(sizeHigh) << 8));

  if (pwFrameSize)
  {
    *pwFrameSize = size;
  }

  if (size < headerSize)
  {
    return nullptr;
  }

  if (pwPayloadSize)
  {
    *pwPayloadSize = static_cast<unsigned __int16>(size - headerSize);
  }

  if (size > left)
  {
    *pbMiss = true;
    return nullptr;
  }

  if (size + popPoint <= m_nMaxSize)
  {
    return &m_sMainBuffer[popPoint];
  }

  const unsigned int tail = m_nMaxSize - popPoint;
  std::memcpy(m_sTempBuffer, &m_sMainBuffer[popPoint], tail);
  std::memcpy(&m_sTempBuffer[tail], m_sMainBuffer, size - tail);
  return m_sTempBuffer;
}

char *_NET_BUFFER::GetSendPoint(int *pnSendSize, bool *pMiss)
{
  *pnSendSize = static_cast<int>(GetLeftLoadSize());
  const int popPoint = m_dwPopPnt;
  if (*pnSendSize > 0)
  {
    if (*pnSendSize + popPoint <= static_cast<int>(m_nMaxSize))
    {
      return &m_sMainBuffer[popPoint];
    }

    const unsigned int tail = m_nMaxSize - popPoint;
    if (tail <= m_nEtrSize)
    {
      std::memcpy(m_sTempBuffer, &m_sMainBuffer[popPoint], tail);
      std::memcpy(&m_sTempBuffer[tail], m_sMainBuffer, *pnSendSize - static_cast<int>(tail));
      return m_sTempBuffer;
    }

    *pMiss = true;
    return nullptr;
  }

  *pMiss = false;
  return nullptr;
}

void _NET_BUFFER::AddPopPos(unsigned int dwAddSize)
{
  m_csPop.Lock();
  const unsigned int next = dwAddSize + m_dwPopPnt;
  if (next < m_nMaxSize)
  {
    m_dwPopPnt = next;
  }
  else
  {
    m_dwPopPnt = next % m_nMaxSize;
    ++m_dwPopRot;
  }
  m_csPop.Unlock();
}

void _NET_BUFFER::Init()
{
  m_csPush.Lock();
  m_dwPushPnt = 0;
  m_dwPushRot = 0;
  m_csPush.Unlock();

  m_csPop.Lock();
  m_dwPopPnt = 0;
  m_dwPopRot = 0;
  m_csPop.Unlock();
}

int _NET_BUFFER::GetLeftLoadSize()
{
  m_csPop.Lock();
  m_csPush.Lock();
  const unsigned int left =
    m_dwPushPnt - m_dwPopPnt + m_nMaxSize * (m_dwPushRot - m_dwPopRot);
  m_csPop.Unlock();
  m_csPush.Unlock();
  // narrowing cast for thunk return parity
  return static_cast<int>(left);
}

void _NET_BUFFER::AddPushPos(unsigned int dwAddSize)
{
  m_csPush.Lock();
  const unsigned int next = dwAddSize + m_dwPushPnt;
  if (next < m_nMaxSize)
  {
    m_dwPushPnt = next;
  }
  else
  {
    m_dwPushPnt = next % m_nMaxSize;
    ++m_dwPushRot;
  }
  m_csPush.Unlock();
}

_socket::_socket()
{
  InitParam();
}

_socket::~_socket() = default;

void _socket::InitParam()
{
  m_nIndex = -1;
  m_dwTotalRecvMsg = 0;
  m_dwTotalSendMsg = 0;
  m_dwTotalRecvBlock = 0;
  m_dwLastCloseTime = 0;
  m_dwRecvPopMissTime = 0;
  m_dwSendSpeedHackTime = 0;
  m_dwResponSpeedHackTime = 0;
  m_bySpeedHackContCount = 0;
  m_bySpeedHackMissCount = 0;
  m_dwBoundResponTerm = 200000;
  m_bAccept = false;
  m_bSendable = false;
  m_bEnterCheck = false;
  m_Socket = static_cast<unsigned __int64>(-1);
  m_dwPingPongCount = 0;
  m_dwSpeedHackCount = 0;
  strcpy_s(m_szID, sizeof(m_szID), "null");
  m_hFGContext = nullptr;
}

CNetSocket::CNetSocket()
  : m_dwSerialCounter(0),
    m_bSetSocket(false),
    m_nIPCheckNodeNum(0),
    m_ndIPCheck(nullptr),
    m_dwIPCheckBufferList(nullptr),
    m_Socket(nullptr),
    m_Event(nullptr)
{
}

CNetSocket::~CNetSocket()
{
  if (m_bSetSocket)
  {
    Release();
  }
}

_socket *CNetSocket::GetSocket(unsigned int dwIndex)
{
  return &m_Socket[dwIndex];
}

bool CNetSocket::SetSocket(_SOCK_TYPE_PARAM *pType, char *pszErrMsg)
{
  std::memcpy(&m_SockType, pType, sizeof(m_SockType));

  const size_t count = pType->m_wSocketMaxNum;
  m_Socket = AllocateVector<_socket>(count);

  m_Event = static_cast<HANDLE *>(operator new(saturated_mul(count, sizeof(HANDLE))));
  for (unsigned int index = 0; index < m_SockType.m_wSocketMaxNum; ++index)
  {
    m_Event[index] = WSACreateEvent();
  }

  if (pType->m_bAcceptIPCheck)
  {
    m_nIPCheckNodeNum = 10 * pType->m_wSocketMaxNum;
    m_dwIPCheckBufferList = static_cast<unsigned int *>(operator new(saturated_mul(m_nIPCheckNodeNum, 4uLL)));
    m_ndIPCheck = static_cast<_IP_CHECK_NODE *>(operator new(saturated_mul(m_nIPCheckNodeNum, 8uLL)));
    m_listIPCheck.SetList(m_nIPCheckNodeNum);
    m_listIPCheck_Empty.SetList(m_nIPCheckNodeNum);
    for (unsigned int index = 0; static_cast<int>(index) < m_nIPCheckNodeNum; ++index)
    {
      m_listIPCheck_Empty.PushNode_Back(index);
    }
    m_tmrListCheckerIPCheck.BeginTimer(pType->m_dwIPCheckTerm / 2);
  }

  if (m_SockType.m_bServer && !InitAcceptSocket(pszErrMsg))
  {
    return false;
  }

  m_bSetSocket = true;
  return true;
}

bool CNetSocket::InitAcceptSocket(char *pszErrMsg)
{
  if (m_SockType.m_byProtocolID)
  {
    if (m_SockType.m_byProtocolID == 1)
    {
      m_sAccept = socket(6, 5, 1256);
    }
  }
  else
  {
    m_sAccept = socket(2, 1, 0);
  }

  if (m_sAccept == static_cast<unsigned __int64>(-1))
  {
    const int errorCode = WSAGetLastError();
    wsprintfA(pszErrMsg, "socket() Failure => %d", errorCode);
    return false;
  }

  int nameLen = 0;
  if (m_SockType.m_byProtocolID)
  {
    if (m_SockType.m_byProtocolID == 1)
    {
      nameLen = 14;
    }
  }
  else
  {
    nameLen = 16;
  }

  sockaddr name{};
  name.sa_family = 2;
  *reinterpret_cast<unsigned __int16 *>(name.sa_data) = htons(m_SockType.m_wPort);
  *reinterpret_cast<unsigned int *>(&name.sa_data[2]) = htonl(0);

  if (bind(m_sAccept, &name, nameLen) == -1)
  {
    const int errorCode = WSAGetLastError();
    wsprintfA(pszErrMsg, "bind() Failure => %d", errorCode);
    return false;
  }

  if (listen(m_sAccept, 5) == -1)
  {
    const int errorCode = WSAGetLastError();
    wsprintfA(pszErrMsg, "listen() Failure => %d", errorCode);
    return false;
  }

  m_AcceptEvent = WSACreateEvent();
  if (WSAEventSelect(m_sAccept, m_AcceptEvent, 8) == -1)
  {
    const int errorCode = WSAGetLastError();
    wsprintfA(pszErrMsg, "WSAEventSelect() Failure => %d", errorCode);
    return false;
  }

  return true;
}

unsigned int CNetSocket::Accept_Server()
{
  sockaddr addr{};
  int addrLen[5]{};
  addrLen[0] = 16;

  const SOCKET socketHandle = accept(m_sAccept, &addr, addrLen);
  if (socketHandle == static_cast<SOCKET>(-1))
  {
    return static_cast<unsigned int>(-1);
  }

  const unsigned int emptySocket = FindEmptySocket();
  if (emptySocket == static_cast<unsigned int>(-1))
  {
    closesocket(socketHandle);
    return static_cast<unsigned int>(-1);
  }

  _socket *socketInfo = &m_Socket[emptySocket];
  socketInfo->m_Socket = socketHandle;
  std::memcpy(&socketInfo->m_Addr, &addr, sizeof(socketInfo->m_Addr));

  if (m_SockType.m_bAcceptIPCheck)
  {
    bool matched = false;
    const int count = m_listIPCheck.CopyIndexList(m_dwIPCheckBufferList, m_nIPCheckNodeNum);
    for (int index = 0; index < count; ++index)
    {
      _IP_CHECK_NODE *node = &m_ndIPCheck[m_dwIPCheckBufferList[index]];
      if (node->dwIP == socketInfo->m_Addr.sin_addr.S_un.S_addr)
      {
        m_listIPCheck.FindNode(m_dwIPCheckBufferList[index]);
        m_listIPCheck_Empty.PushNode_Back(m_dwIPCheckBufferList[index]);
        matched = true;
        break;
      }
    }

    if (!matched)
    {
      closesocket(socketInfo->m_Socket);
      return static_cast<unsigned int>(-1);
    }
  }

  if (WSAEventSelect(socketInfo->m_Socket, m_Event[emptySocket], 33) == -1)
  {
    closesocket(socketInfo->m_Socket);
    return static_cast<unsigned int>(-1);
  }

  socketInfo->m_bAccept = true;
  socketInfo->m_bSendable = true;
  socketInfo->m_dwConnectTime = timeGetTime();
  socketInfo->m_dwLastRecvTime = timeGetTime();
  socketInfo->m_dwLastSendTime = socketInfo->m_dwLastRecvTime;
  socketInfo->m_dwSerial = m_dwSerialCounter++;
  ++m_TotalCount.m_dwAcceptNum;
  if (socketInfo->m_Addr.sin_addr.S_un.S_addr == *reinterpret_cast<unsigned int *>(byLocalIP))
  {
    GetIPAddress(&socketInfo->m_Addr);
  }

  return emptySocket;
}

bool CNetSocket::Accept_Client(unsigned int dwSocketIndex)
{
  _socket *socketInfo = &m_Socket[dwSocketIndex];
  if (socketInfo->m_bAccept)
  {
    return false;
  }

  socketInfo->m_bAccept = true;
  socketInfo->m_bSendable = true;
  socketInfo->m_dwConnectTime = timeGetTime();
  socketInfo->m_dwLastRecvTime = timeGetTime();
  socketInfo->m_dwLastSendTime = socketInfo->m_dwLastRecvTime;
  socketInfo->m_dwSerial = m_dwSerialCounter++;
  ++m_TotalCount.m_dwAcceptNum;
  return true;
}

int CNetSocket::Connect(unsigned int n, sockaddr_in *pAddr)
{
  if (m_Socket[n].m_bAccept)
  {
    return -1;
  }

  int nameLen = 0;
  if (m_SockType.m_byProtocolID)
  {
    if (m_SockType.m_byProtocolID == 1)
    {
      m_Socket[n].m_Socket = socket(6, 5, 1256);
      nameLen = 14;
    }
  }
  else
  {
    m_Socket[n].m_Socket = socket(2, 1, 0);
    nameLen = 16;
  }

  if (connect(m_Socket[n].m_Socket, reinterpret_cast<const sockaddr *>(pAddr), nameLen) == -1)
  {
    closesocket(m_Socket[n].m_Socket);
    return WSAGetLastError();
  }

  if (WSAEventSelect(m_Socket[n].m_Socket, m_Event[n], 49) == -1)
  {
    return -3;
  }

  return 0;
}

bool CNetSocket::Send(unsigned int n, char *pBuf, int nSize, int *pnRet)
{
  *pnRet = send(m_Socket[n].m_Socket, pBuf, nSize, 0);
  if (*pnRet != -1)
  {
    return true;
  }

  *pnRet = WSAGetLastError();
  if (*pnRet == 10035)
  {
    ++m_TotalCount.m_dwTotalSendBlockNum;
    ++m_Socket[n].m_dwTotalRecvBlock;
  }
  else
  {
    ++m_TotalCount.m_dwTotalSendErrNum;
  }

  return false;
}

bool CNetSocket::Recv(unsigned int n, char *pBuf, int nBufMaxSize, int *pnRet)
{
  *pnRet = recv(m_Socket[n].m_Socket, pBuf, nBufMaxSize, 0);
  if (*pnRet != -1)
  {
    return true;
  }

  *pnRet = WSAGetLastError();
  if (*pnRet != 10035)
  {
    ++m_TotalCount.m_dwTotalRecvErrNum;
  }

  return false;
}

bool CNetSocket::CloseSocket(unsigned int n)
{
  shutdown(m_Socket[n].m_Socket, 1);
  EmptySocketBuffer(n);
  --m_TotalCount.m_dwAcceptNum;
  closesocket(m_Socket[n].m_Socket);
  m_Socket[n].InitParam();
  m_Socket[n].m_dwLastCloseTime = timeGetTime();
  return true;
}

void CNetSocket::CloseAll()
{
  for (unsigned int n = 0; n < m_SockType.m_wSocketMaxNum; ++n)
  {
    if (m_Socket[n].m_bAccept)
    {
      CloseSocket(n);
      Sleep(1u);
    }
  }

  if (m_SockType.m_bServer)
  {
    closesocket(m_sAccept);
  }
}

void CNetSocket::Release()
{
  if (!m_bSetSocket)
  {
    return;
  }

  CloseAll();
  if (m_SockType.m_bServer)
  {
    WSACloseEvent(m_AcceptEvent);
  }

  for (unsigned int n = 0; n < m_SockType.m_wSocketMaxNum; ++n)
  {
    WSACloseEvent(m_Event[n]);
  }

  m_bSetSocket = false;

  if (m_ndIPCheck)
  {
    operator delete(m_ndIPCheck);
  }
  if (m_dwIPCheckBufferList)
  {
    operator delete(m_dwIPCheckBufferList);
  }
  if (m_Socket)
  {
    DestroyVector(m_Socket);
  }
  if (m_Event)
  {
    operator delete(m_Event);
  }
}

unsigned int CNetSocket::FindEmptySocket()
{
  const DWORD now = timeGetTime();
  for (unsigned int index = 0; index < m_SockType.m_wSocketMaxNum; ++index)
  {
    if (!m_Socket[index].m_bAccept)
    {
      const unsigned int elapsed = now - m_Socket[index].m_dwLastCloseTime;
      if (elapsed > m_SockType.m_dwSocketRecycleTerm)
      {
        return index;
      }
    }
  }

  return static_cast<unsigned int>(-1);
}

void CNetSocket::EmptySocketBuffer(unsigned int n)
{
  char buf[132]{};
  int received = 0;
  do
  {
    received = recv(m_Socket[n].m_Socket, buf, 128, 0);
  }
  while (received && received != -1);
}

unsigned int CNetSocket::GetSocketIPAddress(unsigned int dwIndex)
{
  return m_Socket[dwIndex].m_Addr.sin_addr.S_un.S_addr;
}

_SOCK_TYPE_PARAM *CNetSocket::GetSocketType()
{
  return &m_SockType;
}

_total_count *CNetSocket::GetTotalCount()
{
  return &m_TotalCount;
}

void CNetSocket::OnLoop()
{
  if (m_tmrListCheckerIPCheck.CountingTimer())
  {
    const DWORD now = timeGetTime();
    unsigned int outIndex[5]{};
    while (m_listIPCheck.CopyFront(outIndex))
    {
      _IP_CHECK_NODE *node = &m_ndIPCheck[outIndex[0]];
      const unsigned int elapsed = now - node->dwWaitStartTime;
      if (elapsed <= m_SockType.m_dwIPCheckTerm)
      {
        break;
      }
      m_listIPCheck.PopNode_Front(outIndex);
      m_listIPCheck_Empty.PushNode_Back(outIndex[0]);
    }
  }
}

bool CNetSocket::PushIPCheckList(unsigned int dwIP)
{
  if (!m_SockType.m_bAcceptIPCheck)
  {
    return true;
  }

  unsigned int outIndex[4]{};
  if (!m_listIPCheck_Empty.PopNode_Front(outIndex))
  {
    return false;
  }

  m_ndIPCheck[outIndex[0]].dwIP = dwIP;
  m_ndIPCheck[outIndex[0]].dwWaitStartTime = timeGetTime();
  return m_listIPCheck.PushNode_Back(outIndex[0]);
}

CNetProcess::CNetProcess()
  : m_nIndex(0),
    m_nOddMsgNum(0),
    m_nTryConnectCount(0),
    m_bUseFG(false),
    m_bSetProcess(false),
    m_nEventThreadNum(0),
    m_pNetwork(nullptr),
    m_csRecv(nullptr),
    m_pRecvBuffer(nullptr),
    m_pSendBuffer(nullptr),
    m_AnsyncConnectData(nullptr),
    m_nKeyCheckNodeNum(0),
    m_ndKeyCheck(nullptr),
    m_dwKeyCheckBufferList(nullptr),
    m_sTempSendBuffer(nullptr),
    m_sTempRecvBuffer(nullptr),
    m_Cryptor(nullptr),
    m_DeCryptorBuffer(nullptr),
    m_EnCryptorBuffer(nullptr),
    m_TempBuffer(nullptr),
    m_bUseCrypt(false)
{
  m_dwCurTime = timeGetTime();
}

CNetProcess::~CNetProcess()
{
  if (m_bSetProcess)
  {
    Release();
  }
}

int CNetProcess::LoadSendMsg(
  unsigned int dwClientIndex,
  unsigned __int8 *pbyType,
  char *szMsg,
  unsigned __int16 nLen)
{
  _socket *Socket = m_NetSocket.GetSocket(dwClientIndex);
  if (!Socket)
  {
    return 0;
  }
  if (!Socket->m_bAccept)
  {
    return 0;
  }
  if (!Socket->m_bSendable && m_bPassablePacket[pbyType[0]][pbyType[1]])
  {
    return 0;
  }

  _NET_BUFFER *sendBuffer = &m_pSendBuffer[dwClientIndex];
  if (m_Type.m_bSendSafe
      && static_cast<unsigned int>(sendBuffer->GetLeftLoadSize()) > m_Type.m_dwSendSafeSize
      && m_bPassablePacket[pbyType[0]][pbyType[1]])
  {
    return 0;
  }

  unsigned __int16 Src[8]{};
  Src[1] = *reinterpret_cast<unsigned __int16 *>(pbyType);
  Src[0] = static_cast<unsigned __int16>(nLen + 4);
  const unsigned int remain = sendBuffer->m_nEtrSize + sendBuffer->m_nMaxSize - sendBuffer->m_dwPushPnt;
  if (Src[0] > remain)
  {
    m_LogFile[2].Write(
      "LoadSendMsg ID(%s) Type(%u,%u) Len(%u) Remain(%u)",
      Socket->m_szID,
      pbyType[0],
      pbyType[1],
      nLen,
      remain);
    return 0;
  }

  const int left = static_cast<int>(sendBuffer->GetLeftLoadSize());
  if (static_cast<unsigned int>(Src[0]) + left > sendBuffer->m_nMaxSize)
  {
    m_pNetwork->ExpulsionSocket(m_nIndex, dwClientIndex, 3, nullptr);
    return 0;
  }

#if 0 // packet log trace disabled (debug only)
  if (m_Type.m_bSendLogFile && m_nIndex == 0 && m_pNetwork && m_pNetwork->m_pProcess[0] == this)
  {
    AppendPacketSnifferLogSend(
      this,
      dwClientIndex,
      static_cast<unsigned __int8>(m_nIndex),
      pbyType[0],
      pbyType[1],
      nLen,
      szMsg);
  }
#endif

  if (m_Cryptor && m_bUseCrypt)
  {
    if (Src[0] > kCryptPacketBufferSize)
    {
      m_LogFile[2].Write(
        "LoadSendMsg ID(%s) Type(%u,%u) Len(%u) Max(%u)",
        Socket->m_szID,
        pbyType[0],
        pbyType[1],
        nLen,
        kCryptPacketBufferSize);
      return 0;
    }

    unsigned __int8 padSize = static_cast<unsigned __int8>(Src[0] & 0xF);
    if (padSize)
    {
      padSize = static_cast<unsigned __int8>(16 - padSize);
    }

    const unsigned __int16 cryptFrameSize = static_cast<unsigned __int16>(Src[0] + padSize + 3);
    if (cryptFrameSize > remain)
    {
      m_LogFile[2].Write(
        "LoadSendMsg ID(%s) CryptSize(%u) Type(%u,%u) Len(%u) Remain(%u)",
        Socket->m_szID,
        static_cast<unsigned int>(cryptFrameSize),
        pbyType[0],
        pbyType[1],
        nLen,
        remain);
      return 0;
    }

    std::memcpy(m_TempBuffer, Src, 4uLL);
    if (nLen)
    {
      std::memcpy(m_TempBuffer + 4, szMsg, nLen);
    }

    if (!m_Cryptor->Encrypt(m_TempBuffer, Src[0], m_EnCryptorBuffer + 3, kCryptPacketBufferSize - 3))
    {
      m_LogFile[2].Write(
        "LoadSendMsg ID(%s) Type(%u,%u) Len(%u) : Cryptor->Encrypt fail",
        Socket->m_szID,
        pbyType[0],
        pbyType[1],
        nLen);
      return 0;
    }

    WriteCryptFrameHeader(m_EnCryptorBuffer, cryptFrameSize, padSize);
    std::memcpy(&sendBuffer->m_sMainBuffer[sendBuffer->m_dwPushPnt], m_EnCryptorBuffer, cryptFrameSize);
    const int tail = sendBuffer->m_nMaxSize - sendBuffer->m_dwPushPnt;
    if (tail < cryptFrameSize)
    {
      std::memcpy(sendBuffer->m_sMainBuffer, &sendBuffer->m_sMainBuffer[sendBuffer->m_nMaxSize], cryptFrameSize - tail);
    }

    sendBuffer->AddPushPos(cryptFrameSize);
    Socket->m_dwLastSendTime = m_dwCurTime;
    return 1;
  }

  std::memcpy(&sendBuffer->m_sMainBuffer[sendBuffer->m_dwPushPnt], Src, 4uLL);
  if (nLen)
  {
    std::memcpy(&sendBuffer->m_sMainBuffer[sendBuffer->m_dwPushPnt + 4], szMsg, nLen);
  }

  const int tail = sendBuffer->m_nMaxSize - sendBuffer->m_dwPushPnt;
  if (tail < Src[0])
  {
    std::memcpy(sendBuffer->m_sMainBuffer, &sendBuffer->m_sMainBuffer[sendBuffer->m_nMaxSize], Src[0] - tail);
  }

  sendBuffer->AddPushPos(Src[0]);
  Socket->m_dwLastSendTime = m_dwCurTime;
  return 1;
}

int CNetProcess::LoadSendMsg(
  unsigned int dwClientIndex,
  unsigned __int16 wType,
  char *szMsg,
  unsigned __int16 nLen)
{
  unsigned __int16 pbyType = wType;
  return LoadSendMsg(dwClientIndex, reinterpret_cast<unsigned __int8 *>(&pbyType), szMsg, nLen);
}

bool CNetProcess::SetProcess(int nIndex, _NET_TYPE_PARAM *pType, CNetWorking *pNetwork, bool bUseFG)
{
  char szFileName[160]{};
  char pszErrMsg[132]{};

  m_nIndex = nIndex;
  std::memcpy(&m_Type, pType, sizeof(m_Type));
  m_pNetwork = pNetwork;
  m_dwCurTime = timeGetTime();
  m_bUseFG = bUseFG;

  unsigned int korLocalTime = static_cast<unsigned int>(GetKorLocalTime());
  wsprintfA(
    szFileName,
    "%s\\%s_%s_Send%d.log",
    m_pNetwork->m_szLogPath,
    pNetwork->m_szSystemName,
    m_Type.m_szModuleName,
    korLocalTime);
  m_LogFile[0].SetWriteLogFile(szFileName, pType->m_bSendLogFile, 0, 1, 1);

  korLocalTime = static_cast<unsigned int>(GetKorLocalTime());
  wsprintfA(
    szFileName,
    "%s\\%s_%s_Recv%d.log",
    m_pNetwork->m_szLogPath,
    pNetwork->m_szSystemName,
    m_Type.m_szModuleName,
    korLocalTime);
  m_LogFile[1].SetWriteLogFile(szFileName, pType->m_bRecvLogFile, 0, 1, 1);

  korLocalTime = static_cast<unsigned int>(GetKorLocalTime());
  wsprintfA(
    szFileName,
    "%s\\%s_%s_System%d.log",
    m_pNetwork->m_szLogPath,
    pNetwork->m_szSystemName,
    m_Type.m_szModuleName,
    korLocalTime);
  m_LogFile[2].SetWriteLogFile(szFileName, pType->m_bSystemLogFile, 0, 1, 1);

  korLocalTime = static_cast<unsigned int>(GetKorLocalTime());
  wsprintfA(
    szFileName,
    "%s\\%s_%s_Hack%d.log",
    m_pNetwork->m_szLogPath,
    pNetwork->m_szSystemName,
    m_Type.m_szModuleName,
    korLocalTime);
  m_LogHack.SetWriteLogFile(szFileName, 1, 0, 1, 1);

  if (m_Type.m_wSocketMaxNum > 5120)
  {
    m_LogFile[2].Write("%d > %d Over Socket Num", m_Type.m_wSocketMaxNum, 5120);
    return false;
  }

  if (m_Type.m_byRecvThreadNum > 8u)
  {
    m_LogFile[2].Write("%d > %d Max RecvThread Num", m_Type.m_byRecvThreadNum, 8);
    return false;
  }

  m_listAcceptEvent.SetList(100 * m_Type.m_wSocketMaxNum);
  m_listCloseEvent.SetList(10 * m_Type.m_wSocketMaxNum);
  m_listRecvEvent.SetList(100 * m_Type.m_wSocketMaxNum);
  m_listAcceptReady.SetList(10 * m_Type.m_wSocketMaxNum);

  m_nEventThreadNum = m_Type.m_wSocketMaxNum / 64;
  if ((m_Type.m_wSocketMaxNum & 0x3F) != 0)
  {
    ++m_nEventThreadNum;
  }

  strcpy_s(pszErrMsg, sizeof(pszErrMsg), "null");
  std::memset(&pszErrMsg[5], 0, 123);

  if (!m_NetSocket.SetSocket(&m_Type, pszErrMsg))
  {
    m_LogFile[2].Write("m_NetSocket.SetSocket. fail : %s", pszErrMsg);
    return false;
  }

  const unsigned int socketCount = m_Type.m_wSocketMaxNum;
  m_pRecvBuffer = AllocateVector<_NET_BUFFER>(socketCount);
  m_pSendBuffer = AllocateVector<_NET_BUFFER>(socketCount);
  if (!m_pRecvBuffer)
  {
    m_LogFile[2].Write("SetProcess(%d)  m_pRecvBuffer mem alloc fail", nIndex);
    return false;
  }
  if (!m_pSendBuffer)
  {
    m_LogFile[2].Write("SetProcess(%d)  m_pSendBuffer mem alloc fail", nIndex);
    return false;
  }

  m_sTempSendBuffer = static_cast<char *>(operator new(m_Type.m_dwSendBufferSize));
  m_sTempRecvBuffer = static_cast<char *>(operator new(m_Type.m_dwRecvBufferSize));

  for (int idx = 0; idx < m_Type.m_wSocketMaxNum; ++idx)
  {
    m_pRecvBuffer[idx].AllocBuffer(m_Type.m_dwRecvBufferSize, m_Type.m_dwRecvBufferSize, m_sTempRecvBuffer);
    m_pSendBuffer[idx].AllocBuffer(m_Type.m_dwSendBufferSize, m_Type.m_dwSendBufferSize, m_sTempSendBuffer);
  }

  m_csRecv = AllocateVector<CNetCriticalSection>(socketCount);
  if (!m_csRecv)
  {
    m_LogFile[2].Write("SetProcess(%d)  m_csRecv mem alloc fail", nIndex);
    return false;
  }

  if (pType->m_bKeyCheck)
  {
    if (m_ndKeyCheck)
    {
      return false;
    }

    m_nKeyCheckNodeNum = 10 * pType->m_wSocketMaxNum;
    m_ndKeyCheck =
      static_cast<_KEY_CHECK_NODE *>(operator new(saturated_mul(m_nKeyCheckNodeNum, 28)));
    m_dwKeyCheckBufferList =
      static_cast<unsigned int *>(operator new(saturated_mul(m_nKeyCheckNodeNum, 4uLL)));
    if (!m_ndKeyCheck)
    {
      m_LogFile[2].Write("SetProcess(%d)  m_ndKeyCheck mem alloc fail", nIndex);
      return false;
    }
    if (!m_dwKeyCheckBufferList)
    {
      m_LogFile[2].Write("SetProcess(%d)  m_dwKeyCheckBufferList mem alloc fail", nIndex);
      return false;
    }

    m_listKeyCheck.SetList(m_nKeyCheckNodeNum);
    m_listKeyCheck_Empty.SetList(m_nKeyCheckNodeNum);
    for (unsigned int dwIndex = 0; static_cast<int>(dwIndex) < m_nKeyCheckNodeNum; ++dwIndex)
    {
      m_listKeyCheck_Empty.PushNode_Back(dwIndex);
    }
    m_tmrListCheckerKeyCheck.BeginTimer(pType->m_dwKeyCheckTerm / 2);
  }

  if (m_Type.m_bServer)
  {
    m_pmAcceptThread.SetParameter(1, this, 0);
    _beginthread(AcceptThread, 0, &m_pmAcceptThread);
  }

  for (int idx = 0; idx < m_nEventThreadNum; ++idx)
  {
    m_pmEventThread[idx].SetParameter(1, this, idx);
    _beginthread(NetEventThread, 0, &m_pmEventThread[idx]);
  }

  for (int idx = 0; idx < m_Type.m_byRecvThreadNum; ++idx)
  {
    m_pmRecvThread[idx].SetParameter(1, this, idx);
    _beginthread(RecvThread, 0, &m_pmRecvThread[idx]);
  }

  m_pmSendThread.SetParameter(1, this, 0);
  _beginthread(SendThread, 0, &m_pmSendThread);

  if (m_Type.m_bAnSyncConnect)
  {
    if (m_Type.m_bServer)
    {
      m_Type.m_bAnSyncConnect = 0;
    }
    else
    {
      m_AnsyncConnectData =
        static_cast<_ANSYNC_CONNECT_DATA *>(operator new(saturated_mul(socketCount, 20)));
      m_listAnsyncConnect.SetList(m_Type.m_wSocketMaxNum);
      m_listAnsyncConnectComplete.SetList(m_Type.m_wSocketMaxNum);
      m_pmConnectThread.SetParameter(1, this, 0);
      _beginthread(ConnectThread, 0, &m_pmConnectThread);
    }
  }

  m_FC.Init(10 * m_Type.m_wSocketMaxNum);
  m_tmrCheckResSH.BeginTimer(1000);
  m_tmrCheckKeyCT.BeginTimer(1000);
  m_tmrCheckRecvBreak.BeginTimer(1000);
  m_nOddMsgNum = 0;
  m_nTryConnectCount = 0;
  m_bSetProcess = true;

  for (int i = 0; i < 256; ++i)
  {
    for (int j = 0; j < 256; ++j)
    {
      m_bPassablePacket[i][j] = false;
    }
  }

  if (m_Type.m_CryptType == 0)
  {
    if (!SetCryptor(m_Type.m_CryptType, m_Type.m_pVCryptParam))
    {
      return false;
    }
  }

  return true;
}

void CNetProcess::Release()
{
  if (!m_bSetProcess)
  {
    return;
  }

  for (unsigned int dwIndex = 0; static_cast<int>(dwIndex) < m_Type.m_wSocketMaxNum; ++dwIndex)
  {
    _socket *socket = m_NetSocket.GetSocket(dwIndex);
    if (socket->m_bAccept)
    {
      m_NetSocket.CloseSocket(dwIndex);
    }
  }

  if (m_Type.m_bServer)
  {
    m_pmAcceptThread.EndThread();
  }
  for (unsigned int dwIndex = 0; static_cast<int>(dwIndex) < m_nEventThreadNum; ++dwIndex)
  {
    m_pmEventThread[dwIndex].EndThread();
  }
  for (unsigned int dwIndex = 0; static_cast<int>(dwIndex) < m_Type.m_byRecvThreadNum; ++dwIndex)
  {
    m_pmRecvThread[dwIndex].EndThread();
  }
  m_pmSendThread.EndThread();
  if (m_Type.m_bAnSyncConnect)
  {
    m_pmConnectThread.EndThread();
  }

  Sleep(3000);

  if (m_ndKeyCheck)
  {
    operator delete(m_ndKeyCheck);
    m_ndKeyCheck = nullptr;
  }
  if (m_dwKeyCheckBufferList)
  {
    operator delete(m_dwKeyCheckBufferList);
    m_dwKeyCheckBufferList = nullptr;
  }
  if (m_csRecv)
  {
    DestroyVector(m_csRecv);
    m_csRecv = nullptr;
  }

  m_NetSocket.Release();

  if (m_pRecvBuffer)
  {
    DestroyVector(m_pRecvBuffer);
    m_pRecvBuffer = nullptr;
  }
  if (m_pSendBuffer)
  {
    DestroyVector(m_pSendBuffer);
    m_pSendBuffer = nullptr;
  }
  if (m_sTempSendBuffer)
  {
    operator delete(m_sTempSendBuffer);
    m_sTempSendBuffer = nullptr;
  }
  if (m_sTempRecvBuffer)
  {
    operator delete(m_sTempRecvBuffer);
    m_sTempRecvBuffer = nullptr;
  }
  if (m_Cryptor)
  {
    delete m_Cryptor;
    m_Cryptor = nullptr;
  }
  if (m_DeCryptorBuffer)
  {
    operator delete(m_DeCryptorBuffer);
    m_DeCryptorBuffer = nullptr;
  }
  if (m_EnCryptorBuffer)
  {
    operator delete(m_EnCryptorBuffer);
    m_EnCryptorBuffer = nullptr;
  }
  if (m_TempBuffer)
  {
    operator delete(m_TempBuffer);
    m_TempBuffer = nullptr;
  }
  if (m_AnsyncConnectData)
  {
    operator delete(m_AnsyncConnectData);
    m_AnsyncConnectData = nullptr;
  }

  m_bSetProcess = false;
}

bool CNetProcess::SetCryptor(unsigned __int8 byCryptType, VCryptorParam *pParam)
{
  if (m_DeCryptorBuffer)
  {
    operator delete(m_DeCryptorBuffer);
    m_DeCryptorBuffer = nullptr;
  }
  if (m_EnCryptorBuffer)
  {
    operator delete(m_EnCryptorBuffer);
    m_EnCryptorBuffer = nullptr;
  }
  if (m_TempBuffer)
  {
    operator delete(m_TempBuffer);
    m_TempBuffer = nullptr;
  }
  if (m_Cryptor)
  {
    delete m_Cryptor;
    m_Cryptor = nullptr;
  }

  m_Cryptor = VCryptor::Create(byCryptType, pParam);
  if (!m_Cryptor)
  {
    return false;
  }

  m_DeCryptorBuffer = static_cast<char *>(operator new(kCryptPacketBufferSize));
  m_EnCryptorBuffer = static_cast<char *>(operator new(kCryptPacketBufferSize));
  m_TempBuffer = static_cast<char *>(operator new(kCryptPacketBufferSize));
  if (!m_DeCryptorBuffer || !m_EnCryptorBuffer || !m_TempBuffer)
  {
    if (m_DeCryptorBuffer)
    {
      operator delete(m_DeCryptorBuffer);
      m_DeCryptorBuffer = nullptr;
    }
    if (m_EnCryptorBuffer)
    {
      operator delete(m_EnCryptorBuffer);
      m_EnCryptorBuffer = nullptr;
    }
    if (m_TempBuffer)
    {
      operator delete(m_TempBuffer);
      m_TempBuffer = nullptr;
    }
    delete m_Cryptor;
    m_Cryptor = nullptr;
    return false;
  }

  return true;
}

void CNetProcess::SetUseCrypt(bool bUseCrypt)
{
  m_bUseCrypt = bUseCrypt;
}

void CNetProcess::CloseAll()
{
  for (int nIndex = 0;
       static_cast<unsigned int>(nIndex) < static_cast<unsigned int>(m_Type.m_wSocketMaxNum);
       ++nIndex)
  {
    PushCloseNode(static_cast<unsigned int>(nIndex));
  }
}

void CNetProcess::CloseSocket(unsigned int dwSocketIndex, bool bSlowClose)
{
  m_pNetwork->CloseSocket(m_nIndex, dwSocketIndex, bSlowClose);
}

void CNetProcess::OnLoop()
{
  m_dwCurTime = timeGetTime();
  m_NetSocket.OnLoop();
  _Receipt();
  _CheckWaitKey();
  _ForceCloseLoop();
  _ResponSpeedHack();
  _CkeckSpeedHackDeley();
  _CkeckKeyCertifyDeley();
  _CkeckRecvBreak();
  for (unsigned __int16 socketIndex = 0; socketIndex < static_cast<int>(m_Type.m_wSocketMaxNum); ++socketIndex)
  {
    if (m_NetSocket.m_Socket[socketIndex].m_bAccept)
    {
      _PopRecvMsg(socketIndex);
      if (m_Type.m_bRealSockCheck)
      {
        _CheckSend(socketIndex);
      }
    }
  }
  CompleteAnsyncConnect();
}

void CNetProcess::OnLoop_Receipt()
{
  _Receipt();
}

void CNetProcess::LogFileOperSetting(bool bRecv, bool bSend, bool bSystem)
{
  m_LogFile[1].SetWriteAble(bRecv);
  m_LogFile[0].SetWriteAble(bSend);
  m_LogFile[2].SetWriteAble(bSystem);
}

void CNetProcess::IOLogFileOperSetting(bool bOper)
{
  m_LogFile[0].SetWriteAble(bOper);
  m_LogFile[1].SetWriteAble(bOper);
}

void *CNetProcess::GetContextHandle(unsigned __int16 wIndex)
{
  return m_NetSocket.m_Socket[wIndex].m_hFGContext;
}

void CNetProcess::SetContextHandle(void *hContextHandle, unsigned __int16 wIndex)
{
  if (hContextHandle)
  {
    m_NetSocket.m_Socket[wIndex].m_hFGContext = hContextHandle;
  }
}

unsigned int CNetProcess::GetSendThreadFrame()
{
  return m_fpsSendThread.m_dwFrames;
}

bool CNetProcess::StartSpeedHackCheck(unsigned int dwClientIndex, char *pszID)
{
  if (!m_Type.m_bSpeedHackCheck)
  {
    return false;
  }

  _socket *socket = m_NetSocket.GetSocket(dwClientIndex);
  if (!socket)
  {
    return false;
  }

  strcpy_s(socket->m_szID, sizeof(socket->m_szID), pszID);
  socket->m_dwSendSpeedHackTime = timeGetTime();

  unsigned int code[4]{};
  for (int j = 0; j < 4; ++j)
  {
    const int high = rand();
    const int highShift = high << 16;
    code[j] = rand() + highShift;
  }

  unsigned int *key = CalcCodeKey(code);
  std::memcpy(socket->m_dwSpeedHackKey, key, sizeof(socket->m_dwSpeedHackKey));

  char msg[16]{};
  std::memcpy(msg, code, sizeof(msg));

  unsigned __int8 type[2]{102, 2};
  if (!LoadSendMsg(dwClientIndex, type, msg, 16))
  {
    return false;
  }

  ++socket->m_dwSpeedHackCount;
  return true;
}

bool CNetProcess::PushAnsyncConnect(unsigned int dwSocketIndex, sockaddr_in *pAddr)
{
  if (!m_Type.m_bAnSyncConnect)
  {
    return false;
  }
  if (m_listAnsyncConnect.IsInList(dwSocketIndex))
  {
    return false;
  }

  _socket *socket = m_NetSocket.GetSocket(dwSocketIndex);
  if (socket->m_bAccept)
  {
    return false;
  }

  std::memcpy(&m_AnsyncConnectData[dwSocketIndex], pAddr, 16);
  return m_listAnsyncConnect.PushNode_Back(dwSocketIndex);
}

void CNetProcess::CompleteAnsyncConnect()
{
  if (!m_Type.m_bAnSyncConnect)
  {
    return;
  }

  unsigned int outIndex[4]{};
  while (m_listAnsyncConnectComplete.PopNode_Front(outIndex))
  {
    _ANSYNC_CONNECT_DATA *data = &m_AnsyncConnectData[outIndex[0]];
    m_pNetwork->AnsyncConnectComplete(m_nIndex, outIndex[0], data->nResult);
  }
}

bool CNetProcess::PushKeyCheckList(
  unsigned int dwSerial,
  unsigned int dwIP,
  unsigned int *pdwKey,
  int nUseKeyNum)
{
  if (!m_Type.m_bKeyCheck)
  {
    return false;
  }
  if (nUseKeyNum > 4)
  {
    return false;
  }

  const int count = m_listKeyCheck.CopyIndexList(m_dwKeyCheckBufferList, m_nKeyCheckNodeNum);
  for (int j = 0; j < count; ++j)
  {
    _KEY_CHECK_NODE *node = &m_ndKeyCheck[m_dwKeyCheckBufferList[j]];
    if (node->dwSerial == dwSerial)
    {
      node->dwSerial = dwSerial;
      std::memcpy(node->dwKey, pdwKey, 4LL * nUseKeyNum);
      node->dwIP = dwIP;
      node->dwWaitStartTime = timeGetTime();
      return true;
    }
  }

  unsigned int outIndex[4]{};
  if (!m_listKeyCheck_Empty.PopNode_Front(outIndex))
  {
    return false;
  }

  m_ndKeyCheck[outIndex[0]].dwSerial = dwSerial;
  std::memcpy(m_ndKeyCheck[outIndex[0]].dwKey, pdwKey, 4LL * nUseKeyNum);
  m_ndKeyCheck[outIndex[0]].dwIP = dwIP;
  m_ndKeyCheck[outIndex[0]].dwWaitStartTime = timeGetTime();
  return m_listKeyCheck.PushNode_Back(outIndex[0]);
}

bool CNetProcess::FindKeyFromWaitList(
  unsigned int dwSocketIndex,
  unsigned int dwSerial,
  unsigned int *pdwKey,
  int nUseKeyNum)
{
  if (!m_Type.m_bKeyCheck)
  {
    return false;
  }
  if (nUseKeyNum > 4)
  {
    return false;
  }

  _socket *socket = m_NetSocket.GetSocket(dwSocketIndex);
  if (!socket)
  {
    return false;
  }

  const unsigned int socketIp = socket->m_Addr.sin_addr.S_un.S_addr;
  int foundIndex = -1;
  const int count = m_listKeyCheck.CopyIndexList(m_dwKeyCheckBufferList, m_nKeyCheckNodeNum);
  for (int j = 0; j < count; ++j)
  {
    _KEY_CHECK_NODE *node = &m_ndKeyCheck[m_dwKeyCheckBufferList[j]];
    bool match = true;
    if (node->dwSerial == dwSerial)
    {
      if (m_Type.m_bAcceptIPCheck && node->dwIP != socketIp)
      {
        break;
      }
      for (int k = 0; k < nUseKeyNum; ++k)
      {
        if (node->dwKey[k] != pdwKey[k])
        {
          match = false;
          break;
        }
      }
      if (match)
      {
        foundIndex = j;
        break;
      }
    }
  }

  if (foundIndex == -1)
  {
    return false;
  }

  m_listKeyCheck.FindNode(m_dwKeyCheckBufferList[foundIndex]);
  m_listKeyCheck_Empty.PushNode_Back(m_dwKeyCheckBufferList[foundIndex]);
  return true;
}

bool CNetProcess::wt_AcceptClient(unsigned int *pdwClientIndex)
{
  ++m_nTryConnectCount;
  if (m_Type.m_bServer)
  {
    *pdwClientIndex = m_NetSocket.Accept_Server();
    if (*pdwClientIndex == static_cast<unsigned int>(-1))
    {
      return false;
    }
  }
  else if (!m_NetSocket.Accept_Client(*pdwClientIndex))
  {
    return false;
  }

  m_pRecvBuffer[*pdwClientIndex].Init();
  m_pSendBuffer[*pdwClientIndex].Init();
  return true;
}

bool CNetProcess::wt_CloseClient(unsigned int dwClientIndex)
{
  if (!m_NetSocket.m_Socket[dwClientIndex].m_bAccept)
  {
    return false;
  }

  m_NetSocket.CloseSocket(dwClientIndex);
  m_pRecvBuffer[dwClientIndex].Init();
  m_pSendBuffer[dwClientIndex].Init();
  return true;
}

void CNetProcess::_Receipt()
{
  unsigned int outIndex[4]{};
  int loopCount = 0;
  while (m_listAcceptEvent.PopNode_Front(outIndex))
  {
    if (wt_AcceptClient(outIndex))
    {
      _socket *socket = m_NetSocket.GetSocket(outIndex[0]);
      if (m_Type.m_bServer)
      {
        if (!m_Type.m_bKeyCheck)
        {
          socket->m_bEnterCheck = true;
        }
        m_pNetwork->AcceptClientCheck(m_nIndex, outIndex[0], socket->m_dwSerial);
      }
      else
      {
        socket->m_bEnterCheck = true;
        m_listAcceptReady.PushNode_Back(outIndex[0]);
      }
    }

    if (loopCount++ > 100)
    {
      break;
    }
  }

  loopCount = 0;
  while (m_listCloseEvent.PopNode_Front(outIndex))
  {
    if (wt_CloseClient(outIndex[0]))
    {
      _socket *socket = m_NetSocket.GetSocket(outIndex[0]);
      m_pNetwork->CloseClientCheck(m_nIndex, outIndex[0], socket->m_dwSerial);
    }

    if (loopCount++ > 100)
    {
      break;
    }
  }

  const DWORD now = timeGetTime();
  loopCount = 0;
  while (m_listAcceptReady.CopyFront(outIndex))
  {
    _socket *socket = m_NetSocket.GetSocket(outIndex[0]);
    const unsigned int elapsed = now - socket->m_dwConnectTime;
    if (elapsed <= 1000)
    {
      break;
    }

    if (socket->m_bAccept)
    {
      m_pNetwork->AcceptClientCheck(m_nIndex, outIndex[0], socket->m_dwSerial);
    }
    m_listAcceptReady.PopNode_Front(outIndex);

    if (loopCount++ > 100)
    {
      break;
    }
  }
}

void CNetProcess::_PopRecvMsg(unsigned __int16 wSocketIndex)
{
  _socket *socket = m_NetSocket.GetSocket(wSocketIndex);
  _NET_BUFFER *recvBuffer = &m_pRecvBuffer[wSocketIndex];
  int processed = 0;
  const DWORD now = timeGetTime();
  const bool useCrypt = m_Cryptor && m_bUseCrypt;

  while (true)
  {
    bool miss = false;
    unsigned __int16 frameSize = 0;
    unsigned __int16 payloadSize = 0;
    char *popPoint = recvBuffer->GetPopPoint(&miss, useCrypt, &frameSize, &payloadSize);
    if (!popPoint)
    {
      if (miss)
      {
        if (socket->m_dwRecvPopMissTime)
        {
          const unsigned int elapsed = now - socket->m_dwRecvPopMissTime;
          if (elapsed > 5000)
          {
            PushCloseNode(wSocketIndex);
            m_LogFile[2].Write("Socket(%d): _PopRecvMsg : bMiss = close", wSocketIndex);
          }
        }
        else
        {
          socket->m_dwRecvPopMissTime = now;
          if (!socket->m_dwRecvPopMissTime)
          {
            socket->m_dwRecvPopMissTime = 1;
          }
        }
      }
      return;
    }

    if (socket->m_dwRecvPopMissTime)
    {
      socket->m_dwRecvPopMissTime = 0;
    }

    _MSG_HEADER *msgHeader = reinterpret_cast<_MSG_HEADER *>(popPoint);
    unsigned __int16 msgSize = 0;
    if (useCrypt)
    {
      if (frameSize < 3)
      {
        PushCloseNode(wSocketIndex);
        return;
      }

      const unsigned __int8 padSize = static_cast<unsigned __int8>(popPoint[2]);
      if (payloadSize < padSize)
      {
        PushCloseNode(wSocketIndex);
        return;
      }

      const unsigned int plainSize = payloadSize - padSize;
      if (!plainSize || plainSize > kCryptPacketBufferSize)
      {
        PushCloseNode(wSocketIndex);
        return;
      }

      if (!m_Cryptor->Decrypt(popPoint + 3, plainSize, m_DeCryptorBuffer, kCryptPacketBufferSize))
      {
        m_LogFile[2].Write(
          "Socket(%d): Cryptor->Decrypt fail, PlainSize(%u)",
          wSocketIndex,
          plainSize);
        PushCloseNode(wSocketIndex);
        return;
      }

      msgHeader = reinterpret_cast<_MSG_HEADER *>(m_DeCryptorBuffer);
      popPoint = m_DeCryptorBuffer;
      msgSize = frameSize;
    }
    else
    {
      msgSize = frameSize;
    }

    if (!socket->m_bEnterCheck)
    {
      if (m_pNetwork->ExpulsionSocket(m_nIndex, wSocketIndex, 1, msgHeader))
      {
        return;
      }
    }

    bool ok = true;
    if (msgHeader->m_byType[0] <= 100)
    {
      ok = m_pNetwork->DataAnalysis(m_nIndex, wSocketIndex, msgHeader, popPoint + 4);
    }
    else
    {
      ok = _InternalPacketProcess(wSocketIndex, msgHeader, popPoint + 4);
    }

    if (!ok)
    {
      if (m_Type.m_bOddMsgWriteLog)
      {
        ++m_nOddMsgNum;
        const char *ipStr = inet_ntoa(socket->m_Addr.sin_addr);
        m_LogFile[2].Write(
          "OddMsg(%d, %d, %d): ID(%s), IP(%s)",
          msgHeader->m_byType[0],
          msgHeader->m_byType[1],
          msgHeader->m_wSize,
          socket->m_szID,
          ipStr);
      }
      if (m_Type.m_bOddMsgDisconnect)
      {
        PushCloseNode(wSocketIndex);
        recvBuffer->Init();
        return;
      }
    }

    socket->m_dwLastRecvTime = now;
    recvBuffer->AddPopPos(msgSize);
    if (static_cast<unsigned int>(++processed) > m_Type.m_dwProcessMsgNumPerLoop)
    {
      return;
    }
  }
}

bool CNetProcess::_InternalPacketProcess(unsigned int dwSocketIndex, _MSG_HEADER *pMsgHeader, char *pMsg)
{
  if (pMsgHeader->m_byType[0] == 'e')
  {
    return pMsgHeader->m_byType[1] == 1 || pMsgHeader->m_byType[1] == 2 || pMsgHeader->m_byType[1] == 3;
  }

  if (pMsgHeader->m_byType[0] != 'f')
  {
    return false;
  }

  switch (pMsgHeader->m_byType[1])
  {
    case 2:
    {
      _socket *socket = m_NetSocket.GetSocket(dwSocketIndex);
      socket->m_dwResponSpeedHackTime = timeGetTime();
      std::memcpy(socket->m_dwSpeedHackKey, pMsg, sizeof(socket->m_dwSpeedHackKey));
      return true;
    }
    case 3:
    {
      _socket *socket = m_NetSocket.GetSocket(dwSocketIndex);
      if (socket)
      {
        if (std::memcmp(pMsg, socket->m_dwSpeedHackKey, 16uLL))
        {
          m_LogHack.Write("SH: %s (key)", socket->m_szID);
          PushCloseNode(dwSocketIndex);
          return true;
        }

        const DWORD elapsed = timeGetTime() - socket->m_dwSendSpeedHackTime;
        if (elapsed < 5000)
        {
          if (++socket->m_bySpeedHackContCount > 5u)
          {
            if (elapsed >= 4700)
            {
              m_LogHack.Write("SH: %s (T:%d) O", socket->m_szID, elapsed);
            }
            else
            {
              m_LogHack.Write("SH: %s (T:%d) X", socket->m_szID, elapsed);
              PushCloseNode(dwSocketIndex);
            }
            return true;
          }

          if (elapsed < 4500)
          {
            m_LogHack.Write("SH %s (T:%d) XX", socket->m_szID, elapsed);
            PushCloseNode(dwSocketIndex);
            return true;
          }
        }

        socket->m_bySpeedHackMissCount = 0;
        _SendSpeedHackCheckMsg(dwSocketIndex);
      }
      return true;
    }
    case 4:
    {
      if (m_NetSocket.GetSocket(dwSocketIndex))
      {
        m_pNetwork->ExpulsionSocket(m_nIndex, dwSocketIndex, 4, nullptr);
      }
      return true;
    }
    case 5:
    {
      if (m_NetSocket.GetSocket(dwSocketIndex))
      {
        m_pNetwork->ExpulsionSocket(m_nIndex, dwSocketIndex, 6, nullptr);
      }
      return true;
    }
    default:
      return false;
  }
}

void CNetProcess::_SendLoop(unsigned int n)
{
  _NET_BUFFER *sendBuffer = &m_pSendBuffer[n];
  _socket *socket = m_NetSocket.GetSocket(n);
  bool miss = false;
  int sendSize = 0;
  char *buf = sendBuffer->GetSendPoint(&sendSize, &miss);
  if (buf)
  {
    int ret = 0;
    if (m_NetSocket.Send(n, buf, sendSize, &ret))
    {
      if (ret <= 0)
      {
        m_LogFile[2].Write("_SendLoop(%d) SendSize(%u) Ret(%d)", n, static_cast<unsigned int>(sendSize), ret);
      }
      else
      {
        sendBuffer->AddPopPos(ret);
      }
    }
    else if (ret == 10035)
    {
      if (socket->m_bSendable)
      {
        const int queued = sendBuffer->GetLeftLoadSize();
        const unsigned int queuedSize = queued > 0 ? static_cast<unsigned int>(queued) : 0;
        const unsigned int freeSize = queuedSize < sendBuffer->m_nMaxSize ? sendBuffer->m_nMaxSize - queuedSize : 0;
        m_LogFile[2].Write(
          "SendWouldBlock ID(%s) Sock(%u) Ret(%d) SendSize(%u) Queued(%u) Free(%u) Max(%u)",
          socket->m_szID,
          n,
          ret,
          static_cast<unsigned int>(sendSize),
          queuedSize,
          freeSize,
          sendBuffer->m_nMaxSize);
      }
      socket->m_bSendable = false;
      socket->m_dwSendBlockTime = timeGetTime();
    }
    else
    {
      PushCloseNode(n);
    }
  }
  else
  {
    if (miss)
    {
      m_LogFile[2].Write("GetSendPoint(id:%s).. error", socket->m_szID);
      PushCloseNode(n);
    }
    if (sendSize < 0)
    {
      m_LogFile[2].Write(
        "_SendLoop: GetSendPoint(%d) nSendSize(%d) < 0 .. PopRot:%d PopPnt:%d PushRot:%d PushPnt:%d",
        n,
        sendSize,
        sendBuffer->m_dwPopRot,
        sendBuffer->m_dwPopPnt,
        sendBuffer->m_dwPushRot,
        sendBuffer->m_dwPushPnt);
    }
  }
}

void CNetProcess::_ForceCloseLoop()
{
  if (!m_FC.m_tmFD.CountingTimer())
  {
    return;
  }

  unsigned int outIndex[4]{};
  while (m_FC.m_listFD.CopyFront(outIndex))
  {
    _FORCE_CLOSE::__FD_NODE *node = &m_FC.m_pFDData[outIndex[0]];
    const unsigned int elapsed = m_dwCurTime - node->dwEventCreateTime;
    if (elapsed <= 5000)
    {
      break;
    }

    _socket *socket = m_NetSocket.GetSocket(node->dwObjIndex);
    if (node->dwObjSerial == socket->m_dwSerial)
    {
      PushCloseNode(node->dwObjIndex);
    }
    m_FC.m_listFD.PopNode_Front(outIndex);
    m_FC.m_listFDEmpty.PushNode_Back(outIndex[0]);
  }
}

void CNetProcess::_CheckSend(unsigned __int16 wSocketIndex)
{
  _socket *socket = &m_NetSocket.m_Socket[wSocketIndex];
  if (socket->m_bAccept)
  {
    const unsigned int elapsed = m_dwCurTime - socket->m_dwLastSendTime;
    if (elapsed > 2500)
    {
      const unsigned __int16 nLen = static_cast<unsigned __int16>(sQry.size());
      LoadSendMsg(wSocketIndex, sbyQryType, &sQry.sDum, nLen);
    }
  }
}

void CNetProcess::_CheckWaitKey()
{
  if (!m_Type.m_bKeyCheck || !m_tmrListCheckerKeyCheck.CountingTimer())
  {
    return;
  }

  const DWORD now = timeGetTime();
  unsigned int outIndex[4]{};
  while (m_listKeyCheck.CopyFront(outIndex))
  {
    _KEY_CHECK_NODE *node = &m_ndKeyCheck[outIndex[0]];
    const unsigned int elapsed = now - node->dwWaitStartTime;
    if (elapsed <= m_Type.m_dwKeyCheckTerm)
    {
      break;
    }
    m_listKeyCheck.PopNode_Front(outIndex);
    m_listKeyCheck_Empty.PushNode_Back(outIndex[0]);
  }
}

void CNetProcess::_CkeckKeyCertifyDeley()
{
  if (!m_Type.m_bKeyCheck || !m_Type.m_bServer || !m_tmrCheckKeyCT.CountingTimer())
  {
    return;
  }

  const DWORD now = timeGetTime();
  for (unsigned int dwIndex = 0; static_cast<int>(dwIndex) < m_Type.m_wSocketMaxNum; ++dwIndex)
  {
    _socket *socket = m_NetSocket.GetSocket(dwIndex);
    if (socket->m_bAccept && !socket->m_bEnterCheck)
    {
      const unsigned int elapsed = now - socket->m_dwConnectTime;
      if (elapsed >= 120000)
      {
        m_pNetwork->ExpulsionSocket(m_nIndex, dwIndex, 2, nullptr);
      }
    }
  }
}

void CNetProcess::_CkeckRecvBreak()
{
  if (!m_tmrCheckRecvBreak.CountingTimer())
  {
    return;
  }

  timeGetTime();
  for (unsigned int dwIndex = 0; static_cast<int>(dwIndex) < m_Type.m_wSocketMaxNum; ++dwIndex)
  {
    _socket *socket = m_NetSocket.GetSocket(dwIndex);
    if (socket->m_bAccept && socket->m_dwRecvPopMissTime)
    {
      m_listRecvEvent.PushNode_Back(dwIndex);
    }
  }
}

void CNetProcess::_CkeckSpeedHackDeley()
{
  if (!m_Type.m_bSpeedHackCheck || !m_Type.m_bServer || !m_tmrCheckResSH.CountingTimer())
  {
    return;
  }

  const DWORD now = timeGetTime();
  for (unsigned int dwIndex = 0; static_cast<int>(dwIndex) < m_Type.m_wSocketMaxNum; ++dwIndex)
  {
    _socket *socket = m_NetSocket.GetSocket(dwIndex);
    if (socket->m_bAccept && socket->m_bEnterCheck)
    {
      if (socket->m_dwSendSpeedHackTime)
      {
        const unsigned int elapsed = now - socket->m_dwSendSpeedHackTime;
        if (elapsed > 0x2BF20 && socket->m_dwLastRecvTime > socket->m_dwSendSpeedHackTime + 100000)
        {
          if (!socket->m_bySpeedHackMissCount)
          {
            _SendSpeedHackCheckMsg(dwIndex);
            ++socket->m_bySpeedHackMissCount;
          }
          if (socket->m_bySpeedHackMissCount > 1u)
          {
            m_pNetwork->ExpulsionSocket(m_nIndex, dwIndex, 0, nullptr);
          }
        }
      }
    }
  }
}

void CNetProcess::_ResponSpeedHack()
{
  if (!m_Type.m_bSpeedHackCheck || m_Type.m_bServer)
  {
    return;
  }

  const DWORD now = timeGetTime();
  for (unsigned int dwIndex = 0; static_cast<int>(dwIndex) < m_Type.m_wSocketMaxNum; ++dwIndex)
  {
    _socket *socket = m_NetSocket.GetSocket(dwIndex);
    if (socket->m_dwResponSpeedHackTime)
    {
      const unsigned int elapsed = now - socket->m_dwResponSpeedHackTime;
      if (elapsed > 5000)
      {
        _check_speed_hack_ans ans{};
        unsigned int *key = CalcCodeKey(socket->m_dwSpeedHackKey);
        std::memcpy(&ans, key, sizeof(ans));
        unsigned __int8 type[2]{102, 3};
        const unsigned __int16 nLen = static_cast<unsigned __int16>(ans.size());
        LoadSendMsg(dwIndex, type, reinterpret_cast<char *>(&ans), nLen);
        ++socket->m_dwSpeedHackCount;
        socket->m_dwResponSpeedHackTime = 0;
      }
    }
  }
}

void CNetProcess::_SendSpeedHackCheckMsg(unsigned int n)
{
  _socket *socket = m_NetSocket.GetSocket(n);
  unsigned int code[4]{};
  for (int j = 0; j < 4; ++j)
  {
    const int high = rand();
    const int highShift = high << 16;
    code[j] = rand() + highShift;
  }

  unsigned int *key = CalcCodeKey(code);
  std::memcpy(socket->m_dwSpeedHackKey, key, sizeof(socket->m_dwSpeedHackKey));

  char msg[16]{};
  std::memcpy(msg, code, sizeof(msg));
  unsigned __int8 type[2]{102, 2};
  LoadSendMsg(n, type, msg, 16);
  socket->m_dwSendSpeedHackTime = timeGetTime();
  ++socket->m_dwSpeedHackCount;
}

void CNetProcess::AcceptThread(void *pv)
{
  auto *param = static_cast<_thread_parameter *>(pv);
  auto *process = static_cast<CNetProcess *>(param->m_pParam);
  HANDLE *events = &process->m_NetSocket.m_AcceptEvent;

  while (param->m_bStart)
  {
    const DWORD waitResult = WSAWaitForMultipleEvents(1u, events, 0, -1, 0);
    if (!waitResult)
    {
      WSANETWORKEVENTS networkEvents{};
      if (WSAEnumNetworkEvents(process->m_NetSocket.m_sAccept, process->m_NetSocket.m_AcceptEvent, &networkEvents)
          != -1
          && (networkEvents.lNetworkEvents & 8) != 0
          && !networkEvents.iErrorCode[3])
      {
        process->m_listAcceptEvent.PushNode_Back(0);
      }
    }
    ResetEvent(*events);
  }

  _endthreadex(1u);
}

void CNetProcess::NetEventThread(void *pv)
{
  auto *param = static_cast<_thread_parameter *>(pv);
  const int threadIndex = param->m_nIndex;
  auto *process = static_cast<CNetProcess *>(param->m_pParam);
  HANDLE *events = process->m_NetSocket.m_Event + 64 * threadIndex;
  CNetSocket *netSocket = &process->m_NetSocket;
  DWORD eventCount = 64;
  if ((threadIndex << 6) + 64 > process->m_Type.m_wSocketMaxNum)
  {
    eventCount = process->m_Type.m_wSocketMaxNum & 0x3F;
  }

  while (param->m_bStart)
  {
    const DWORD waitResult = WSAWaitForMultipleEvents(eventCount, events, 0, -1, 0);
    if (waitResult == -1)
    {
      ResetEvent(*(events - 1));
    }
    else if (waitResult < 64)
    {
      const unsigned int socketIndex = (threadIndex << 6) + waitResult;
      WSANETWORKEVENTS networkEvents{};
      if (WSAEnumNetworkEvents(netSocket->m_Socket[socketIndex].m_Socket, events[waitResult], &networkEvents) != -1)
      {
        if ((networkEvents.lNetworkEvents & 0x10) == 0 || networkEvents.iErrorCode[4])
        {
          if ((networkEvents.lNetworkEvents & 1) == 0 || networkEvents.iErrorCode[0])
          {
            if ((networkEvents.lNetworkEvents & 0x20) != 0 && !networkEvents.iErrorCode[5])
            {
              process->PushCloseNode(socketIndex);
            }
          }
          else
          {
            process->m_listRecvEvent.PushNode_Back(socketIndex);
          }
        }
        else
        {
          process->m_listAcceptEvent.PushNode_Back(socketIndex);
        }
      }
      ResetEvent(events[waitResult]);
    }
  }

  _endthreadex(22);
}

void CNetProcess::RecvThread(void *pv)
{
  auto *param = static_cast<_thread_parameter *>(pv);
  auto *process = static_cast<CNetProcess *>(param->m_pParam);
  CNetSocket *netSocket = &process->m_NetSocket;
  int sleepCounter = 0;
  unsigned int outIndex[4]{};

  while (param->m_bStart)
  {
    while (process->m_listRecvEvent.PopNode_Front(outIndex))
    {
      const unsigned int socketIndex = outIndex[0];
      process->m_csRecv[socketIndex].Lock();

      _NET_BUFFER *recvBuffer = &process->m_pRecvBuffer[socketIndex];
      _socket *socket = netSocket->GetSocket(socketIndex);
      const int bufMax = recvBuffer->m_nEtrSize + recvBuffer->m_nMaxSize - recvBuffer->m_dwPushPnt;
      char *pushPos = recvBuffer->GetPushPos();

      int ret = 0;
      const bool ok = netSocket->Recv(socketIndex, pushPos, bufMax, &ret);
      if (!ok || ret <= 0)
      {
        if (ret != 10035)
        {
          process->PushCloseNode(socketIndex);
          if (process->m_Type.m_bSvrToS)
          {
            process->m_LogFile[2].Write("recv(%d).. error.. code(%d)", socketIndex, ret);
          }
        }
        process->m_csRecv[socketIndex].Unlock();
        Sleep(0);
        continue;
      }

      const int left = static_cast<int>(recvBuffer->GetLeftLoadSize());
      if (ret + left <= static_cast<int>(recvBuffer->m_nMaxSize))
      {
        const int tail = recvBuffer->m_nMaxSize - recvBuffer->m_dwPushPnt;
        if (tail < ret)
        {
          std::memcpy(recvBuffer->m_sMainBuffer, &recvBuffer->m_sMainBuffer[recvBuffer->m_nMaxSize], ret - tail);
        }
        recvBuffer->AddPushPos(ret);
        process->m_csRecv[socketIndex].Unlock();
        Sleep(0);
      }
      else
      {
        process->m_csRecv[socketIndex].Unlock();
        process->m_LogFile[2].Write("%d) RecvThread(%d).. list overflow", process->m_nIndex, socketIndex);
        process->PushCloseNode(socketIndex);
      }
    }

    if (++sleepCounter > process->m_Type.m_byRecvSleepTime)
    {
      Sleep(1u);
      sleepCounter = 0;
    }
  }

  _endthreadex(0);
}

void CNetProcess::SendThread(void *pv)
{
  auto *param = static_cast<_thread_parameter *>(pv);
  auto *process = static_cast<CNetProcess *>(param->m_pParam);
  CNetSocket *netSocket = &process->m_NetSocket;
  int sleepCounter = 0;

  while (param->m_bStart)
  {
    process->m_fpsSendThread.CalcFrameRate();
    const DWORD now = timeGetTime();
    for (unsigned int dwIndex = 0; dwIndex < process->m_Type.m_wSocketMaxNum; ++dwIndex)
    {
      _socket *socket = netSocket->GetSocket(dwIndex);
      if (!socket->m_bAccept)
      {
        continue;
      }

      if (!socket->m_bSendable)
      {
        const unsigned int elapsed = now - socket->m_dwSendBlockTime;
        if (elapsed <= 1000)
        {
          continue;
        }
        _NET_BUFFER *sendBuffer = &process->m_pSendBuffer[dwIndex];
        const int queued = sendBuffer->GetLeftLoadSize();
        const unsigned int queuedSize = queued > 0 ? static_cast<unsigned int>(queued) : 0;
        const unsigned int freeSize = queuedSize < sendBuffer->m_nMaxSize ? sendBuffer->m_nMaxSize - queuedSize : 0;
        process->m_LogFile[2].Write(
          "SendUnblock ID(%s) Sock(%u) BlockMs(%u) Queued(%u) Free(%u) Max(%u)",
          socket->m_szID,
          dwIndex,
          elapsed,
          queuedSize,
          freeSize,
          sendBuffer->m_nMaxSize);
        socket->m_bSendable = true;
      }

      process->_SendLoop(dwIndex);
      if (!(dwIndex % 100))
      {
        Sleep(0);
      }
    }

    if (++sleepCounter > process->m_Type.m_bySendSleepTime)
    {
      Sleep(1u);
      sleepCounter = 0;
    }
  }

  _endthreadex(0);
}

void CNetProcess::ConnectThread(void *pv)
{
  auto *param = static_cast<_thread_parameter *>(pv);
  auto *process = static_cast<CNetProcess *>(param->m_pParam);
  CNetSocket *netSocket = &process->m_NetSocket;
  unsigned int outIndex[4]{};

  while (param->m_bStart)
  {
    while (process->m_listAnsyncConnect.CopyFront(outIndex))
    {
      _ANSYNC_CONNECT_DATA *data = &process->m_AnsyncConnectData[outIndex[0]];
      const int result = netSocket->Connect(outIndex[0], &data->Addr);
      data->nResult = result;
      if (process->m_listAnsyncConnect.PopNode_Front(outIndex))
      {
        process->m_listAnsyncConnectComplete.PushNode_Back(outIndex[0]);
      }
    }
    Sleep(10);
  }

  _endthreadex(0);
}

void CNetProcess::PushCloseNode(unsigned int nIndex)
{
  _socket *socket = m_NetSocket.GetSocket(nIndex);
  if (socket->m_bAccept && !m_listCloseEvent.IsInList(nIndex))
  {
    m_listCloseEvent.PushNode_Back(nIndex);
  }
}
