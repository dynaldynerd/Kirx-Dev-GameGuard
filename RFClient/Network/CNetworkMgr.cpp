#include "Network/CNetworkMgr.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>

#include "RFClientApp.h"

namespace
{
constexpr DWORD kWorldProtocolVer = 126455;
constexpr char kClientVerCheckKey[] = "75a32d4a6daff548fcfcb40fea838a0d";
constexpr char kDebugNetworkLogPath[] = "D:\\Private Files\\playrf\\RFOnline\\RFClientNetwork.log";
constexpr DWORD kSpeedHackAnswerDelayMs = 5000;

int MinInt(int left, int right)
{
  return left < right ? left : right;
}

template <typename T>
void CopyPacket(T &packet, const char *payload, int payloadSize)
{
  packet = T();
  if (!payload || payloadSize <= 0)
  {
    return;
  }

  const int copySize = MinInt(payloadSize, static_cast<int>(sizeof(T)));
  memcpy(&packet, payload, copySize);
}

void AppendNetworkLog(const char *text)
{
  if (!text || !text[0])
  {
    return;
  }

  HANDLE outputFile = CreateFileA(kDebugNetworkLogPath,
                                  FILE_APPEND_DATA,
                                  FILE_SHARE_READ,
                                  NULL,
                                  OPEN_ALWAYS,
                                  FILE_ATTRIBUTE_NORMAL,
                                  NULL);
  if (outputFile == INVALID_HANDLE_VALUE)
  {
    return;
  }

  DWORD bytesWritten = 0;
  WriteFile(outputFile, text, static_cast<DWORD>(strlen(text)), &bytesWritten, NULL);
  WriteFile(outputFile, "\r\n", 2, &bytesWritten, NULL);
  CloseHandle(outputFile);
}

void AppendWorldEnterPacketLog(DWORD accountSerial,
                               const DWORD *rawKey,
                               const DWORD *sendKey,
                               DWORD protocolVer,
                               bool fullMode,
                               const char *clientVerCheckKey)
{
  char line[512];
  _snprintf_s(
    line,
    sizeof(line),
    _TRUNCATE,
    "World enter packet detail: account=%lu rawKey=%08lX,%08lX,%08lX,%08lX sendKey=%08lX,%08lX,%08lX,%08lX protocol=%lu full=%u clientVer=%s",
    accountSerial,
    rawKey ? rawKey[0] : 0,
    rawKey ? rawKey[1] : 0,
    rawKey ? rawKey[2] : 0,
    rawKey ? rawKey[3] : 0,
    sendKey ? sendKey[0] : 0,
    sendKey ? sendKey[1] : 0,
    sendKey ? sendKey[2] : 0,
    sendKey ? sendKey[3] : 0,
    protocolVer,
    fullMode ? 1 : 0,
    clientVerCheckKey ? clientVerCheckKey : "");
  AppendNetworkLog(line);
}
} // namespace

CNetworkMgr *g_pNetworkMgr = NULL;

CNetworkMgr::CNetworkMgr()
  : m_hSocket(INVALID_SOCKET),
    m_bCreated(FALSE),
    m_bWSAStarted(FALSE),
    m_bIsConnecting(false),
    m_bHasLauncherData(FALSE),
    m_nActiveServerID(GST_NONE),
    m_dwAccountIndex(0),
    m_dwAvatarIndex(0),
    m_dwLastestAvatarIndex(static_cast<DWORD>(-1)),
    m_dwSelectedDalant(0),
    m_dwSelectedGold(0),
    m_dwResponSpeedHackTime(0),
    m_byAvatarGrade(0),
    m_byResultOfEnterTheWorldServer(WORLD_ENTER_NOT_RECIVED),
    m_byResultOfUserInfo(USER_INFO_NOT_RECIVED),
    m_byResultOfCharacterSelect(CHAR_SELECT_NOT_RECIVED),
    m_byEnterWorldServerType(0),
    m_bSentEnterWorldRequest(FALSE),
    m_bHasEnterWorldResult(FALSE),
    m_bSentRegedCharRequest(FALSE),
    m_bHasRegedCharResult(FALSE),
    m_bSentSelCharRequest(FALSE),
    m_bHasSelCharResult(FALSE),
    m_bHasUILockInform(FALSE)
{
  ZeroMemory(m_bIsConnectedToServer, sizeof(m_bIsConnectedToServer));
  ZeroMemory(m_dwServerIP, sizeof(m_dwServerIP));
  ZeroMemory(m_dwServerPort, sizeof(m_dwServerPort));
  ZeroMemory(m_pServerName, sizeof(m_pServerName));
  ZeroMemory(m_dwWorldServerKey, sizeof(m_dwWorldServerKey));
  ZeroMemory(m_dwSpeedHackKey, sizeof(m_dwSpeedHackKey));
  ZeroMemory(m_pID, sizeof(m_pID));
  ZeroMemory(&m_ServerAddr, sizeof(m_ServerAddr));
  ZeroMemory(m_szStatusText, sizeof(m_szStatusText));
  ZeroMemory(&m_sSelCharResult, sizeof(m_sSelCharResult));
  ZeroMemory(&m_sUILockInform, sizeof(m_sUILockInform));
}

CNetworkMgr::~CNetworkMgr()
{
  Destroy();
}

BOOL CNetworkMgr::Create(void)
{
  if (m_bCreated)
  {
    return TRUE;
  }

  WSADATA l_sWsaData;
  if (WSAStartup(MAKEWORD(2, 2), &l_sWsaData) != 0)
  {
    SetStatusText("CNetworkMgr::Create failed: WSAStartup");
    return FALSE;
  }

  m_bWSAStarted = TRUE;
  g_pNetworkMgr = this;
  m_bCreated = TRUE;
  SetStatusText("CNetworkMgr created");
  return TRUE;
}

BOOL CNetworkMgr::Destroy(void)
{
  if (!m_bCreated)
  {
    return FALSE;
  }

  CloseSocket();
  ResetConnectionState();

  if (m_bWSAStarted)
  {
    WSACleanup();
    m_bWSAStarted = FALSE;
  }

  if (g_pNetworkMgr == this)
  {
    g_pNetworkMgr = NULL;
  }

  m_bCreated = FALSE;
  return TRUE;
}

void CNetworkMgr::OnLoop(void)
{
  if (!m_bCreated)
  {
    return;
  }

  PumpConnect();
  PumpReceive();
  _ResponSpeedHack();
}

void CNetworkMgr::Connect(BYTE pi_byServerType)
{
  if (!m_bCreated)
  {
    return;
  }

  if (pi_byServerType >= MAX_GAME_SERVER)
  {
    SetStatusText("CNetworkMgr::Connect invalid server type=%u", pi_byServerType);
    return;
  }

  if (m_bIsConnecting || m_bIsConnectedToServer[pi_byServerType])
  {
    return;
  }

  if (!m_dwServerIP[pi_byServerType] || !m_dwServerPort[pi_byServerType])
  {
    SetStatusText("CNetworkMgr::Connect missing server address type=%u", pi_byServerType);
    return;
  }

  CloseSocket();
  ResetConnectionState();

  m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (m_hSocket == INVALID_SOCKET)
  {
    SetStatusText("CNetworkMgr::Connect socket failed error=%d", WSAGetLastError());
    return;
  }

  u_long l_dwNonBlocking = 1;
  ioctlsocket(m_hSocket, FIONBIO, &l_dwNonBlocking);

  ZeroMemory(&m_ServerAddr, sizeof(m_ServerAddr));
  m_ServerAddr.sin_family = AF_INET;
  m_ServerAddr.sin_addr.S_un.S_addr = m_dwServerIP[pi_byServerType];
  m_ServerAddr.sin_port = htons(static_cast<u_short>(m_dwServerPort[pi_byServerType]));

  m_nActiveServerID = static_cast<char>(pi_byServerType);

  const int l_nResult = connect(m_hSocket, reinterpret_cast<sockaddr *>(&m_ServerAddr), sizeof(m_ServerAddr));
  if (l_nResult == 0)
  {
    AcceptClientCheck(0, 0, 0);
    return;
  }

  const int l_nError = WSAGetLastError();
  if (l_nError == WSAEWOULDBLOCK || l_nError == WSAEINPROGRESS || l_nError == WSAEALREADY)
  {
    m_bIsConnecting = true;
    SetStatusText("Connecting to world server %s:%u",
                  GetAddressText(m_dwServerIP[pi_byServerType]),
                  m_dwServerPort[pi_byServerType]);
    return;
  }

  SetStatusText("CNetworkMgr::Connect failed error=%d", l_nError);
  CloseSocket();
  ResetConnectionState();
}

void CNetworkMgr::Close(BYTE)
{
  CloseSocket();
  CloseClientCheck(0, 0, 0);
  SetStatusText("Network connection closed");
}

BOOL CNetworkMgr::SendNetMessage(DWORD, BYTE *pi_pType, void *pi_pMsg, int pi_nSize)
{
  if (m_hSocket == INVALID_SOCKET || !m_bIsConnectedToServer[GST_WORLD] || !pi_pType || pi_nSize < 0)
  {
    return FALSE;
  }

  _MSG_HEADER l_sHeader;
  l_sHeader.m_wSize = static_cast<WORD>(MSG_HEADER_SIZE + pi_nSize);
  l_sHeader.m_byType[0] = pi_pType[0];
  l_sHeader.m_byType[1] = pi_pType[1];

  std::vector<char> l_vecSendBuffer(l_sHeader.m_wSize);
  memcpy(l_vecSendBuffer.data(), &l_sHeader, MSG_HEADER_SIZE);
  if (pi_nSize > 0 && pi_pMsg)
  {
    memcpy(l_vecSendBuffer.data() + MSG_HEADER_SIZE, pi_pMsg, pi_nSize);
  }

  return SendBuffer(l_vecSendBuffer.data(), static_cast<int>(l_vecSendBuffer.size())) ? TRUE : FALSE;
}

BOOL CNetworkMgr::SystemMsg_EnterWorldRequest_zone(void)
{
  _enter_world_request_zone l_sSend;
  ZeroMemory(&l_sSend, sizeof(l_sSend));
  DWORD l_dwSendKey[CHECK_KEY_NUM]{};

  l_sSend.dwAccountSerial = m_dwAccountIndex;
  memcpy(l_dwSendKey, CalcCodeKey(m_dwWorldServerKey), sizeof(l_dwSendKey));
  memcpy(l_sSend.dwMasterKey, l_dwSendKey, sizeof(l_sSend.dwMasterKey));
  l_sSend.bFullMode = (_GetMainApp() && !_GetMainApp()->IsWindowedMode()) ? true : false;
  l_sSend.dwProtocolVer = kWorldProtocolVer;
  strcpy_s(l_sSend.szClientVerCheckKey, sizeof(l_sSend.szClientVerCheckKey), kClientVerCheckKey);

  BYTE l_byType[] = {system_msg, enter_world_request_zone};
  const BOOL l_bResult = SendNetMessage(NST_GAME_SERVER, l_byType, &l_sSend, sizeof(l_sSend));
  if (l_bResult)
  {
    m_bSentEnterWorldRequest = TRUE;
    m_bHasEnterWorldResult = FALSE;
    m_byResultOfEnterTheWorldServer = WORLD_ENTER_NOT_RECIVED;
    m_bSentRegedCharRequest = FALSE;
    m_bHasRegedCharResult = FALSE;
    m_bSentSelCharRequest = FALSE;
    m_bHasSelCharResult = FALSE;
    m_bHasUILockInform = FALSE;
    m_byResultOfUserInfo = USER_INFO_NOT_RECIVED;
    m_byResultOfCharacterSelect = CHAR_SELECT_NOT_RECIVED;
    m_dwAvatarIndex = 0;
    m_dwLastestAvatarIndex = static_cast<DWORD>(-1);
    m_dwSelectedDalant = 0;
    m_dwSelectedGold = 0;
    m_sRegedCharResult = _reged_char_result_zone();
    m_sNotArrangedCharInform = _not_arranged_char_inform_zocl();
    ZeroMemory(&m_sSelCharResult, sizeof(m_sSelCharResult));
    ZeroMemory(&m_sUILockInform, sizeof(m_sUILockInform));
    SetStatusText("World enter request sent: account=%lu protocol=%lu full=%u",
                  l_sSend.dwAccountSerial,
                  l_sSend.dwProtocolVer,
                  l_sSend.bFullMode ? 1 : 0);
    AppendWorldEnterPacketLog(l_sSend.dwAccountSerial,
                              m_dwWorldServerKey,
                              l_dwSendKey,
                              l_sSend.dwProtocolVer,
                              l_sSend.bFullMode,
                              l_sSend.szClientVerCheckKey);
  }
  else
  {
    SetStatusText("CNetworkMgr::SystemMsg_EnterWorldRequest_zone send failed");
  }

  return l_bResult;
}

void CNetworkMgr::SystemMsg_EnterWorldResult_zone(char *pi_pMsg)
{
  if (!pi_pMsg)
  {
    return;
  }

  _enter_world_result_zone l_sRecv;
  memcpy(&l_sRecv, pi_pMsg, sizeof(l_sRecv));

  m_byResultOfEnterTheWorldServer = l_sRecv.byResult;
  m_byAvatarGrade = l_sRecv.byUserGrade;
  m_byEnterWorldServerType = l_sRecv.bySvrType;
  m_bHasEnterWorldResult = TRUE;

  SetStatusText("World enter result: result=%u avatarGrade=%u serverType=%u",
                m_byResultOfEnterTheWorldServer,
                m_byAvatarGrade,
                m_byEnterWorldServerType);
}

BOOL CNetworkMgr::SystemMsg_RegedCharRequest_zone(void)
{
  BYTE l_byType[] = {system_msg, reged_char_request_zone};
  const BOOL l_bResult = SendNetMessage(NST_GAME_SERVER, l_byType, NULL, 0);
  if (l_bResult)
  {
    m_bSentRegedCharRequest = TRUE;
    m_bHasRegedCharResult = FALSE;
    m_byResultOfUserInfo = USER_INFO_NOT_RECIVED;
    m_dwLastestAvatarIndex = static_cast<DWORD>(-1);
    m_sRegedCharResult = _reged_char_result_zone();
    m_sNotArrangedCharInform = _not_arranged_char_inform_zocl();
    SetStatusText("Registered character request sent");
  }
  else
  {
    SetStatusText("CNetworkMgr::SystemMsg_RegedCharRequest_zone send failed");
  }

  return l_bResult;
}

void CNetworkMgr::SystemMsg_RegedCharResult_zone(char *pi_pMsg, int pi_nPayloadSize)
{
  CopyPacket(m_sRegedCharResult, pi_pMsg, pi_nPayloadSize);

  if (m_sRegedCharResult.byCharNum > 3u)
  {
    m_sRegedCharResult.byCharNum = 0;
  }

  m_byResultOfUserInfo = m_sRegedCharResult.byRetCode;
  m_bHasRegedCharResult = TRUE;
  m_dwLastestAvatarIndex = static_cast<DWORD>(-1);

  DWORD l_dwPrevCharLastConnTime = 0;
  if (m_byResultOfUserInfo == USER_INFO_SUCCESS)
  {
    for (int i = 0; i < m_sRegedCharResult.byCharNum; ++i)
    {
      const _REGED_AVATOR_DB &l_sRegedAvatar = m_sRegedCharResult.RegedList[i];
      if (l_sRegedAvatar.m_bySlotIndex > 2)
      {
        continue;
      }

      if (m_dwLastestAvatarIndex == static_cast<DWORD>(-1) ||
          l_dwPrevCharLastConnTime < l_sRegedAvatar.m_dwLastConnTime)
      {
        m_dwLastestAvatarIndex = l_sRegedAvatar.m_bySlotIndex;
        l_dwPrevCharLastConnTime = l_sRegedAvatar.m_dwLastConnTime;
      }
    }
  }

  SetStatusText("Registered character result: result=%u charNum=%u latestSlot=%ld",
                m_byResultOfUserInfo,
                static_cast<unsigned>(m_sRegedCharResult.byCharNum),
                m_dwLastestAvatarIndex == static_cast<DWORD>(-1)
                  ? -1L
                  : static_cast<long>(m_dwLastestAvatarIndex));
}

BOOL CNetworkMgr::SystemMsg_SelCharRequest_zone(BYTE pi_byAvatarIndex)
{
  _sel_char_request_zone l_sSend{};
  l_sSend.bySlotIndex = static_cast<char>(pi_byAvatarIndex);

  BYTE l_byType[] = {system_msg, sel_char_request_zone};
  const BOOL l_bResult = SendNetMessage(NST_GAME_SERVER, l_byType, &l_sSend, sizeof(l_sSend));
  if (l_bResult)
  {
    m_bSentSelCharRequest = TRUE;
    m_bHasSelCharResult = FALSE;
    m_byResultOfCharacterSelect = CHAR_SELECT_NOT_RECIVED;
    ZeroMemory(&m_sSelCharResult, sizeof(m_sSelCharResult));
    SetStatusText("Character select request sent: slot=%u", static_cast<unsigned>(pi_byAvatarIndex));
  }
  else
  {
    SetStatusText("CNetworkMgr::SystemMsg_SelCharRequest_zone send failed");
  }

  return l_bResult;
}

void CNetworkMgr::SystemMsg_SelCharResult_zone(char *pi_pMsg, int pi_nPayloadSize)
{
  ZeroMemory(&m_sSelCharResult, sizeof(m_sSelCharResult));
  if (pi_pMsg && pi_nPayloadSize > 0)
  {
    memcpy(&m_sSelCharResult, pi_pMsg, MinInt(pi_nPayloadSize, static_cast<int>(sizeof(m_sSelCharResult))));
  }

  m_byResultOfCharacterSelect = m_sSelCharResult.byRetCode;
  m_dwAvatarIndex = m_sSelCharResult.dwWorldSerial;
  m_dwSelectedDalant = m_sSelCharResult.dwDalant;
  m_dwSelectedGold = m_sSelCharResult.dwGold;
  m_bHasSelCharResult = TRUE;

  SetStatusText("Character select result: result=%u slot=%u worldSerial=%lu",
                static_cast<unsigned>(m_byResultOfCharacterSelect),
                static_cast<unsigned>(static_cast<unsigned __int8>(m_sSelCharResult.bySlotIndex)),
                m_dwAvatarIndex);
}

BOOL CNetworkMgr::CheckMsg_CheckAnswer(void)
{
  _check_answer l_sAnswer{};
  BYTE l_byType[] = {check_msg, check_answer};
  return SendNetMessage(NST_GAME_SERVER, l_byType, &l_sAnswer, sizeof(l_sAnswer));
}

BOOL CNetworkMgr::SecureMsg_CheckSpeedHackAnswer(void)
{
  if (!m_dwResponSpeedHackTime)
  {
    return FALSE;
  }

  _check_speed_hack_ans l_sAnswer{};
  memcpy(l_sAnswer.dwKey, CalcCodeKey(m_dwSpeedHackKey), sizeof(l_sAnswer.dwKey));

  BYTE l_byType[] = {secure_msg, check_speed_hack_ans};
  const BOOL l_bResult = SendNetMessage(NST_GAME_SERVER, l_byType, &l_sAnswer, sizeof(l_sAnswer));
  if (l_bResult)
  {
    SetStatusText("Speed hack answer sent after %lu ms",
                  timeGetTime() - m_dwResponSpeedHackTime);
    m_dwResponSpeedHackTime = 0;
  }
  else
  {
    SetStatusText("SecureMsg_CheckSpeedHackAnswer send failed");
  }

  return l_bResult;
}

void CNetworkMgr::SetID(char *pi_pID)
{
  if (!pi_pID)
  {
    m_pID[0] = '\0';
    return;
  }

  strcpy_s(m_pID, sizeof(m_pID), pi_pID);
}

void CNetworkMgr::SetWorldServerKey(DWORD *pi_pKey)
{
  if (!pi_pKey)
  {
    ZeroMemory(m_dwWorldServerKey, sizeof(m_dwWorldServerKey));
    return;
  }

  memcpy(m_dwWorldServerKey, pi_pKey, sizeof(m_dwWorldServerKey));
}

void CNetworkMgr::SetStatusText(const char *pi_pFormat, ...)
{
  if (!pi_pFormat)
  {
    m_szStatusText[0] = '\0';
    return;
  }

  va_list l_sArgs;
  va_start(l_sArgs, pi_pFormat);
  vsnprintf_s(m_szStatusText, sizeof(m_szStatusText), _TRUNCATE, pi_pFormat, l_sArgs);
  va_end(l_sArgs);

  OutputDebugStringA(m_szStatusText);
  OutputDebugStringA("\n");
  AppendNetworkLog(m_szStatusText);
}

const char *CNetworkMgr::GetAddressText(DWORD pi_dwAddress)
{
  static char s_szAddress[32];
  IN_ADDR l_sAddr;
  l_sAddr.S_un.S_addr = pi_dwAddress;
  const char *l_pAddress = inet_ntoa(l_sAddr);
  strcpy_s(s_szAddress, sizeof(s_szAddress), l_pAddress ? l_pAddress : "0.0.0.0");
  return s_szAddress;
}

void CNetworkMgr::AcceptClientCheck(DWORD, DWORD, DWORD)
{
  if (m_nActiveServerID >= 0 && m_nActiveServerID < MAX_GAME_SERVER)
  {
    m_bIsConnectedToServer[m_nActiveServerID] = TRUE;
  }

  m_bIsConnecting = false;
  m_vecRecvBuffer.clear();

  SetStatusText("Connected to world server %s:%u",
                GetAddressText(m_dwServerIP[GST_WORLD]),
                m_dwServerPort[GST_WORLD]);
}

void CNetworkMgr::CloseClientCheck(DWORD, DWORD, DWORD)
{
  ResetConnectionState();
}

void CNetworkMgr::ResetConnectionState(void)
{
  ZeroMemory(m_bIsConnectedToServer, sizeof(m_bIsConnectedToServer));
  m_bIsConnecting = false;
  m_nActiveServerID = GST_NONE;
  m_bSentEnterWorldRequest = FALSE;
  m_bHasEnterWorldResult = FALSE;
  m_bSentRegedCharRequest = FALSE;
  m_bHasRegedCharResult = FALSE;
  m_bSentSelCharRequest = FALSE;
  m_bHasSelCharResult = FALSE;
  m_bHasUILockInform = FALSE;
  m_byResultOfEnterTheWorldServer = WORLD_ENTER_NOT_RECIVED;
  m_byResultOfUserInfo = USER_INFO_NOT_RECIVED;
  m_byResultOfCharacterSelect = CHAR_SELECT_NOT_RECIVED;
  m_byAvatarGrade = 0;
  m_byEnterWorldServerType = 0;
  m_dwAvatarIndex = 0;
  m_dwLastestAvatarIndex = static_cast<DWORD>(-1);
  m_dwSelectedDalant = 0;
  m_dwSelectedGold = 0;
  ZeroMemory(m_dwSpeedHackKey, sizeof(m_dwSpeedHackKey));
  m_dwResponSpeedHackTime = 0;
  m_sRegedCharResult = _reged_char_result_zone();
  m_sNotArrangedCharInform = _not_arranged_char_inform_zocl();
  ZeroMemory(&m_sSelCharResult, sizeof(m_sSelCharResult));
  ZeroMemory(&m_sUILockInform, sizeof(m_sUILockInform));
  m_vecRecvBuffer.clear();
}

void CNetworkMgr::CloseSocket(void)
{
  if (m_hSocket != INVALID_SOCKET)
  {
    closesocket(m_hSocket);
    m_hSocket = INVALID_SOCKET;
  }
}

void CNetworkMgr::PumpConnect(void)
{
  if (!m_bIsConnecting || m_hSocket == INVALID_SOCKET)
  {
    return;
  }

  fd_set l_sWriteSet;
  fd_set l_sErrorSet;
  FD_ZERO(&l_sWriteSet);
  FD_ZERO(&l_sErrorSet);
  FD_SET(m_hSocket, &l_sWriteSet);
  FD_SET(m_hSocket, &l_sErrorSet);

  timeval l_sTimeout;
  l_sTimeout.tv_sec = 0;
  l_sTimeout.tv_usec = 0;

  const int l_nSelect = select(0, NULL, &l_sWriteSet, &l_sErrorSet, &l_sTimeout);
  if (l_nSelect <= 0)
  {
    return;
  }

  int l_nSocketError = 0;
  int l_nSocketErrorLen = sizeof(l_nSocketError);
  getsockopt(m_hSocket, SOL_SOCKET, SO_ERROR, reinterpret_cast<char *>(&l_nSocketError), &l_nSocketErrorLen);

  if (l_nSocketError == 0 && FD_ISSET(m_hSocket, &l_sWriteSet))
  {
    AcceptClientCheck(0, 0, 0);
    return;
  }

  SetStatusText("World server connect failed error=%d", l_nSocketError);
  CloseSocket();
  ResetConnectionState();
}

void CNetworkMgr::PumpReceive(void)
{
  if (m_hSocket == INVALID_SOCKET || !m_bIsConnectedToServer[GST_WORLD])
  {
    return;
  }

  for (;;)
  {
    char l_szRecvBuffer[4096];
    const int l_nRecv = recv(m_hSocket, l_szRecvBuffer, sizeof(l_szRecvBuffer), 0);
    if (l_nRecv == 0)
    {
      SetStatusText("World server closed the connection");
      CloseSocket();
      ResetConnectionState();
      return;
    }

    if (l_nRecv == SOCKET_ERROR)
    {
      const int l_nError = WSAGetLastError();
      if (l_nError == WSAEWOULDBLOCK)
      {
        break;
      }

      SetStatusText("recv failed error=%d", l_nError);
      CloseSocket();
      ResetConnectionState();
      return;
    }

    m_vecRecvBuffer.insert(m_vecRecvBuffer.end(), l_szRecvBuffer, l_szRecvBuffer + l_nRecv);
  }

  while (m_vecRecvBuffer.size() >= MSG_HEADER_SIZE)
  {
    _MSG_HEADER l_sHeader;
    memcpy(&l_sHeader, m_vecRecvBuffer.data(), MSG_HEADER_SIZE);

    if (l_sHeader.m_wSize < MSG_HEADER_SIZE)
    {
      SetStatusText("Invalid packet size=%u", l_sHeader.m_wSize);
      CloseSocket();
      ResetConnectionState();
      return;
    }

    if (m_vecRecvBuffer.size() < l_sHeader.m_wSize)
    {
      break;
    }

    const int l_nPayloadSize = static_cast<int>(l_sHeader.m_wSize) - MSG_HEADER_SIZE;
    const char *l_pPayload = m_vecRecvBuffer.data() + MSG_HEADER_SIZE;
    ProcessPacket(l_sHeader, l_pPayload, l_nPayloadSize);

    m_vecRecvBuffer.erase(m_vecRecvBuffer.begin(), m_vecRecvBuffer.begin() + l_sHeader.m_wSize);
  }
}

void CNetworkMgr::_ResponSpeedHack(void)
{
  if (!m_dwResponSpeedHackTime || m_hSocket == INVALID_SOCKET || !m_bIsConnectedToServer[GST_WORLD])
  {
    return;
  }

  const DWORD l_dwNow = timeGetTime();
  const DWORD l_dwElapsed = l_dwNow - m_dwResponSpeedHackTime;
  if (l_dwElapsed <= kSpeedHackAnswerDelayMs)
  {
    return;
  }

  SecureMsg_CheckSpeedHackAnswer();
}

bool CNetworkMgr::SendBuffer(const char *pi_pBuffer, int pi_nSize)
{
  if (m_hSocket == INVALID_SOCKET || !pi_pBuffer || pi_nSize <= 0)
  {
    return false;
  }

  int l_nSent = 0;
  while (l_nSent < pi_nSize)
  {
    fd_set l_sWriteSet;
    FD_ZERO(&l_sWriteSet);
    FD_SET(m_hSocket, &l_sWriteSet);

    timeval l_sTimeout;
    l_sTimeout.tv_sec = 1;
    l_sTimeout.tv_usec = 0;

    const int l_nSelect = select(0, NULL, &l_sWriteSet, NULL, &l_sTimeout);
    if (l_nSelect <= 0)
    {
      return false;
    }

    const int l_nSend = send(m_hSocket, pi_pBuffer + l_nSent, pi_nSize - l_nSent, 0);
    if (l_nSend == SOCKET_ERROR)
    {
      const int l_nError = WSAGetLastError();
      if (l_nError == WSAEWOULDBLOCK)
      {
        continue;
      }

      SetStatusText("send failed error=%d", l_nError);
      return false;
    }

    l_nSent += l_nSend;
  }

  return true;
}

bool CNetworkMgr::ProcessPacket(const _MSG_HEADER &pi_sHeader, const char *pi_pPayload, int pi_nPayloadSize)
{
  if (pi_sHeader.m_byType[0] == system_msg && pi_sHeader.m_byType[1] == enter_world_result_zone)
  {
    if (pi_nPayloadSize >= static_cast<int>(sizeof(_enter_world_result_zone)))
    {
      SystemMsg_EnterWorldResult_zone(const_cast<char *>(pi_pPayload));
      return true;
    }

    SetStatusText("enter_world_result_zone payload too small size=%d", pi_nPayloadSize);
    return false;
  }

  if (pi_sHeader.m_byType[0] == system_msg && pi_sHeader.m_byType[1] == reged_char_result_zone)
  {
    if (pi_nPayloadSize >= 14)
    {
      SystemMsg_RegedCharResult_zone(const_cast<char *>(pi_pPayload), pi_nPayloadSize);
      return true;
    }

    SetStatusText("reged_char_result_zone payload too small size=%d", pi_nPayloadSize);
    return false;
  }

  if (pi_sHeader.m_byType[0] == system_msg && pi_sHeader.m_byType[1] == sel_char_result_zone)
  {
    if (pi_nPayloadSize >= static_cast<int>(sizeof(_sel_char_result_zone)))
    {
      SystemMsg_SelCharResult_zone(const_cast<char *>(pi_pPayload), pi_nPayloadSize);
      return true;
    }

    SetStatusText("sel_char_result_zone payload too small size=%d", pi_nPayloadSize);
    return false;
  }

  if (pi_sHeader.m_byType[0] == system_msg && pi_sHeader.m_byType[1] == not_arranged_char_inform_zocl)
  {
    m_sNotArrangedCharInform = _not_arranged_char_inform_zocl();
    if (pi_pPayload && pi_nPayloadSize > 0)
    {
      memcpy(&m_sNotArrangedCharInform,
             pi_pPayload,
             MinInt(pi_nPayloadSize, static_cast<int>(sizeof(m_sNotArrangedCharInform))));
    }
    return true;
  }

  if (pi_sHeader.m_byType[0] == system_msg &&
      (pi_sHeader.m_byType[1] == server_notify_inform_zone ||
       pi_sHeader.m_byType[1] == alive_char_result_zocl))
  {
    return true;
  }

  if (pi_sHeader.m_byType[0] == ui_msg && pi_sHeader.m_byType[1] == uilock_inform_request_zocl)
  {
    if (pi_nPayloadSize >= static_cast<int>(sizeof(_uilock_inform_request_zocl)))
    {
      ZeroMemory(&m_sUILockInform, sizeof(m_sUILockInform));
      memcpy(&m_sUILockInform, pi_pPayload, sizeof(m_sUILockInform));
      m_bHasUILockInform = TRUE;
      return true;
    }

    SetStatusText("uilock_inform_request_zocl payload too small size=%d", pi_nPayloadSize);
    return false;
  }

  if (pi_sHeader.m_byType[0] == check_msg && pi_sHeader.m_byType[1] == check_query)
  {
    CheckMsg_CheckAnswer();
    return true;
  }

  if (pi_sHeader.m_byType[0] == secure_msg && pi_sHeader.m_byType[1] == check_speed_hack_ask)
  {
    if (pi_nPayloadSize >= static_cast<int>(sizeof(_check_speed_hack_ask)))
    {
      const _check_speed_hack_ask *l_pAsk =
        reinterpret_cast<const _check_speed_hack_ask *>(pi_pPayload);
      memcpy(m_dwSpeedHackKey, l_pAsk->dwSrcKey, sizeof(m_dwSpeedHackKey));
      m_dwResponSpeedHackTime = timeGetTime();
      SetStatusText("Speed hack ask queued");
      return true;
    }

    SetStatusText("check_speed_hack_ask payload too small size=%d", pi_nPayloadSize);
    return false;
  }

  SetStatusText("Unhandled packet type=(%u,%u) payload=%d",
                pi_sHeader.m_byType[0],
                pi_sHeader.m_byType[1],
                pi_nPayloadSize);
  return true;
}
