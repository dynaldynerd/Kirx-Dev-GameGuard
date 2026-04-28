#include "Network/CNetworkMgr.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>

#pragma comment(lib, "bcrypt.lib")

#include "CItemDataMgr.h"
#include "RFClientApp.h"

namespace
{
constexpr DWORD kWorldProtocolVer = 126455;
constexpr char kClientVerCheckKey[] = "75a32d4a6daff548fcfcb40fea838a0d";
constexpr char kDebugNetworkLogPath[] = ".\\RFClientNetwork.log";
constexpr char kPacketDumpDir[] = ".\\_reports";
constexpr char kPacketTraceTextPath[] = ".\\_reports\\net_packets.log";
constexpr char kRegedCharResultDumpPath[] = ".\\_reports\\reged_char_result_last.bin";
constexpr char kRegedCharResultTextPath[] = ".\\_reports\\reged_char_result_last.txt";
constexpr DWORD kSpeedHackAnswerDelayMs = 5000;
constexpr DWORD kAopAesKeyWords = 6;
constexpr DWORD kAopAesKeySize = kAopAesKeyWords * sizeof(DWORD);

constexpr DWORD kAopServerEncKeyWords[kAopAesKeyWords] = {
  0x3C8173A8, 0x5D43BEDF, 0x4A9BEE6B, 0x6E6A18E8, 0x3C68C2D2, 0xD74D98D6,
};

constexpr DWORD kAopClientEncKeyWords[kAopAesKeyWords] = {
  0x3C8170D8, 0x5D43BE0F, 0x4E96E96B, 0x6E6A18E8, 0x0C38C2D2, 0xD74D9AD6,
};

int MinInt(int left, int right)
{
  return left < right ? left : right;
}

WORD ReadWordLE(const char *pi_pBuffer)
{
  return static_cast<WORD>(static_cast<unsigned char>(pi_pBuffer[0]) |
                           (static_cast<unsigned char>(pi_pBuffer[1]) << 8));
}

void WriteWordLE(char *po_pBuffer, WORD pi_wValue)
{
  po_pBuffer[0] = static_cast<char>(pi_wValue & 0xFF);
  po_pBuffer[1] = static_cast<char>((pi_wValue >> 8) & 0xFF);
}

void AppendAopKeyBytes(std::vector<unsigned char> &po_vecKey, const DWORD *pi_pWords)
{
  po_vecKey.clear();
  po_vecKey.reserve(kAopAesKeySize);
  for (DWORD i = 0; i < kAopAesKeyWords; ++i)
  {
    const DWORD l_dwWord = pi_pWords[i];
    po_vecKey.push_back(static_cast<unsigned char>((l_dwWord >> 24) & 0xFF));
    po_vecKey.push_back(static_cast<unsigned char>((l_dwWord >> 16) & 0xFF));
    po_vecKey.push_back(static_cast<unsigned char>((l_dwWord >> 8) & 0xFF));
    po_vecKey.push_back(static_cast<unsigned char>(l_dwWord & 0xFF));
  }
}

bool IsExistingLocalFile(const char *pi_pPath)
{
  const DWORD l_dwAttributes = pi_pPath ? GetFileAttributesA(pi_pPath) : INVALID_FILE_ATTRIBUTES;
  return l_dwAttributes != INVALID_FILE_ATTRIBUTES &&
         (l_dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

bool IsAopClientRoot(void)
{
  return IsExistingLocalFile(".\\Character\\Player\\Tex\\AC_75_U_065.dds") ||
         IsExistingLocalFile(".\\Character\\Player\\Tex\\BF_75_U_065.dds") ||
         IsExistingLocalFile(".\\Character\\Player\\Tex\\CM_75_U_065.dds");
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

void AppendTextLine(const char *pi_pPath, const char *pi_pText)
{
  if (!pi_pPath || !pi_pPath[0] || !pi_pText)
  {
    return;
  }

  HANDLE l_hFile = CreateFileA(pi_pPath,
                               FILE_APPEND_DATA,
                               FILE_SHARE_READ,
                               NULL,
                               OPEN_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);
  if (l_hFile == INVALID_HANDLE_VALUE)
  {
    return;
  }

  DWORD l_dwWritten = 0;
  WriteFile(l_hFile, pi_pText, static_cast<DWORD>(strlen(pi_pText)), &l_dwWritten, NULL);
  WriteFile(l_hFile, "\r\n", 2, &l_dwWritten, NULL);
  CloseHandle(l_hFile);
}

void BuildHexPreview(const char *pi_pData, int pi_nSize, char *po_pText, int pi_nTextSize)
{
  if (!po_pText || pi_nTextSize <= 0)
  {
    return;
  }

  po_pText[0] = '\0';
  if (!pi_pData || pi_nSize <= 0)
  {
    return;
  }

  const int l_nBytes = MinInt(pi_nSize, 24);
  int l_nOffset = 0;
  for (int i = 0; i < l_nBytes && l_nOffset < pi_nTextSize - 1; ++i)
  {
    const unsigned char l_byValue = static_cast<unsigned char>(pi_pData[i]);
    const int l_nWritten = _snprintf_s(po_pText + l_nOffset,
                                       pi_nTextSize - l_nOffset,
                                       _TRUNCATE,
                                       "%s%02X",
                                       i ? " " : "",
                                       static_cast<unsigned>(l_byValue));
    if (l_nWritten <= 0)
    {
      break;
    }

    l_nOffset += l_nWritten;
  }
}

void AppendPacketTrace(const char *pi_pDirection,
                       const _MSG_HEADER &pi_sHeader,
                       const char *pi_pPayload,
                       int pi_nPayloadSize,
                       bool pi_bAopCrypt)
{
  CreateDirectoryA(kPacketDumpDir, NULL);

  char l_szPreview[96];
  BuildHexPreview(pi_pPayload, pi_nPayloadSize, l_szPreview, sizeof(l_szPreview));

  char l_szLine[512];
  _snprintf_s(l_szLine,
              sizeof(l_szLine),
              _TRUNCATE,
              "%lu %s type=%u:%u msgSize=%u payload=%d aopCrypt=%u preview=%s",
              timeGetTime(),
              pi_pDirection ? pi_pDirection : "?",
              static_cast<unsigned>(pi_sHeader.m_byType[0]),
              static_cast<unsigned>(pi_sHeader.m_byType[1]),
              static_cast<unsigned>(pi_sHeader.m_wSize),
              pi_nPayloadSize,
              pi_bAopCrypt ? 1 : 0,
              l_szPreview);
  AppendTextLine(kPacketTraceTextPath, l_szLine);
}

bool WriteBinaryFile(const char *pi_pPath, const void *pi_pData, DWORD pi_dwSize)
{
  if (!pi_pPath || !pi_pPath[0] || (!pi_pData && pi_dwSize))
  {
    return false;
  }

  HANDLE l_hFile = CreateFileA(pi_pPath,
                               GENERIC_WRITE,
                               FILE_SHARE_READ,
                               NULL,
                               CREATE_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);
  if (l_hFile == INVALID_HANDLE_VALUE)
  {
    return false;
  }

  DWORD l_dwWritten = 0;
  const BOOL l_bOk = WriteFile(l_hFile, pi_pData, pi_dwSize, &l_dwWritten, NULL);
  CloseHandle(l_hFile);
  return l_bOk && l_dwWritten == pi_dwSize;
}

bool ReadBinaryFile(const char *pi_pPath, std::vector<char> &po_vecData)
{
  po_vecData.clear();
  if (!pi_pPath || !pi_pPath[0])
  {
    return false;
  }

  HANDLE l_hFile = CreateFileA(pi_pPath,
                               GENERIC_READ,
                               FILE_SHARE_READ,
                               NULL,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);
  if (l_hFile == INVALID_HANDLE_VALUE)
  {
    return false;
  }

  DWORD l_dwFileSize = GetFileSize(l_hFile, NULL);
  if (l_dwFileSize == INVALID_FILE_SIZE && GetLastError() != NO_ERROR)
  {
    CloseHandle(l_hFile);
    return false;
  }

  po_vecData.resize(l_dwFileSize);
  DWORD l_dwRead = 0;
  const BOOL l_bOk = ReadFile(l_hFile,
                              po_vecData.empty() ? NULL : po_vecData.data(),
                              l_dwFileSize,
                              &l_dwRead,
                              NULL);
  CloseHandle(l_hFile);
  if (!l_bOk || l_dwRead != l_dwFileSize)
  {
    po_vecData.clear();
    return false;
  }

  return true;
}

const char *GetRegedEquipTableName(BYTE pi_byRegedEquipIndex)
{
  static const char *kRegedEquipTableName[8] =
  {
    "Upper",
    "Lower",
    "Gauntlet",
    "Shoe",
    "Helmet",
    "Shield",
    "Weapon",
    "Cloak",
  };

  return pi_byRegedEquipIndex < 8 ? kRegedEquipTableName[pi_byRegedEquipIndex] : "?";
}

BYTE GetRegedEquipClientItemType(BYTE pi_byRegedEquipIndex)
{
  static const BYTE kRegedEquipClientItemType[8] =
  {
    IEPT_UPPER_PART,
    IEPT_LOWER_PART,
    IEPT_GLOVES,
    IEPT_SHOES,
    IEPT_HELMET,
    IEPT_SHIELD,
    IEPT_WEAPON,
    IEPT_CLOAK,
  };

  return pi_byRegedEquipIndex < 8 ? kRegedEquipClientItemType[pi_byRegedEquipIndex] : 0xFF;
}

void CopyRegedAvatarNameForLog(const _REGED_AVATOR_DB &pi_stAvatar,
                               char *po_pBuffer,
                               size_t pi_stBufferSize)
{
  if (!po_pBuffer || pi_stBufferSize == 0)
  {
    return;
  }

  ZeroMemory(po_pBuffer, pi_stBufferSize);
  const size_t l_stCopySize =
    sizeof(pi_stAvatar.m_wszAvatorName) < pi_stBufferSize - 1
      ? sizeof(pi_stAvatar.m_wszAvatorName)
      : pi_stBufferSize - 1;
  memcpy(po_pBuffer, pi_stAvatar.m_wszAvatorName, l_stCopySize);
  po_pBuffer[pi_stBufferSize - 1] = '\0';
}

void DumpRegedCharResultPayload(const char *pi_pPayload, int pi_nPayloadSize)
{
  if (!pi_pPayload || pi_nPayloadSize <= 0)
  {
    return;
  }

  CreateDirectoryA(kPacketDumpDir, NULL);
  WriteBinaryFile(kRegedCharResultDumpPath,
                  pi_pPayload,
                  static_cast<DWORD>(pi_nPayloadSize));

  _reged_char_result_zone l_sPacket;
  CopyPacket(l_sPacket, pi_pPayload, pi_nPayloadSize);
  HANDLE l_hTextFile = CreateFileA(kRegedCharResultTextPath,
                                   GENERIC_WRITE,
                                   FILE_SHARE_READ,
                                   NULL,
                                   CREATE_ALWAYS,
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL);
  if (l_hTextFile != INVALID_HANDLE_VALUE)
  {
    DWORD l_dwWritten = 0;
    char l_szText[1024];
    _snprintf_s(l_szText,
                sizeof(l_szText),
                _TRUNCATE,
                "reged_char_result_zone dump\npayloadSize=%d\nstructSize=%u\nret=%u\ncharNum=%u\nlock=%d,%d,%d\n",
                pi_nPayloadSize,
                static_cast<unsigned>(sizeof(_reged_char_result_zone)),
                static_cast<unsigned>(l_sPacket.byRetCode),
                static_cast<unsigned>(l_sPacket.byCharNum),
                l_sPacket.iLockType[0],
                l_sPacket.iLockType[1],
                l_sPacket.iLockType[2]);
    WriteFile(l_hTextFile, l_szText, static_cast<DWORD>(strlen(l_szText)), &l_dwWritten, NULL);

    CItemDataMgr *l_pItemDataMgr = _GetItemDataMgr();
    const bool l_bItemDataLoaded = l_pItemDataMgr && l_pItemDataMgr->IsLoaded();
    _snprintf_s(l_szText,
                sizeof(l_szText),
                _TRUNCATE,
                "itemDataLoaded=%u\n",
                l_bItemDataLoaded ? 1 : 0);
    WriteFile(l_hTextFile, l_szText, static_cast<DWORD>(strlen(l_szText)), &l_dwWritten, NULL);

    const BYTE l_byCharNum = l_sPacket.byCharNum > 3 ? 3 : l_sPacket.byCharNum;
    for (BYTE i = 0; i < l_byCharNum; ++i)
    {
      const _REGED_AVATOR_DB &l_sAvatar = l_sPacket.RegedList[i];
      char l_szName[sizeof(l_sAvatar.m_wszAvatorName) + 1];
      CopyRegedAvatarNameForLog(l_sAvatar, l_szName, sizeof(l_szName));

      _snprintf_s(l_szText,
                  sizeof(l_szText),
                  _TRUNCATE,
                  "\nrecord=%u name=%s slot=%u race=%u class=%s level=%u dalant=%lu gold=%lu base=0x%08lX lastConn=%lu\n",
                  static_cast<unsigned>(i),
                  l_szName,
                  static_cast<unsigned>(l_sAvatar.m_bySlotIndex),
                  static_cast<unsigned>(l_sAvatar.m_byRaceSexCode),
                  l_sAvatar.m_szClassCode,
                  static_cast<unsigned>(l_sAvatar.m_byLevel),
                  l_sAvatar.m_dwDalant,
                  l_sAvatar.m_dwGold,
                  l_sAvatar.m_dwBaseShape,
                  l_sAvatar.m_dwLastConnTime);
      WriteFile(l_hTextFile, l_szText, static_cast<DWORD>(strlen(l_szText)), &l_dwWritten, NULL);

      for (BYTE j = 0; j < 8; ++j)
      {
        const int l_nKey = static_cast<int>(l_sAvatar.m_EquipKey[j].zItemIndex);
        const BYTE l_byItemType = GetRegedEquipClientItemType(j);
        const ITEM_CLIENT_RECORD *l_pRecord =
          (l_bItemDataLoaded && l_nKey >= 0)
            ? l_pItemDataMgr->GetClientRecord(l_byItemType, static_cast<DWORD>(l_nKey))
            : NULL;
        _snprintf_s(l_szText,
                    sizeof(l_szText),
                    _TRUNCATE,
                    "  ek%u table=%s clientType=%u key=%d eu=%u itemRecords=%lu mesh=0x%08lX icon=0x%08lX dtCode=0x%08lX\n",
                    static_cast<unsigned>(j),
                    GetRegedEquipTableName(j),
                    static_cast<unsigned>(l_byItemType),
                    l_nKey,
                    static_cast<unsigned>(l_sAvatar.m_byEquipLv[j]),
                    l_bItemDataLoaded ? l_pItemDataMgr->GetRecordCount(l_byItemType) : 0,
                    l_pRecord ? l_pRecord->dwMeshID : static_cast<DWORD>(-1),
                    l_pRecord ? l_pRecord->dwIconID : static_cast<DWORD>(-1),
                    l_pRecord ? l_pRecord->dwDTCode : static_cast<DWORD>(-1));
        WriteFile(l_hTextFile, l_szText, static_cast<DWORD>(strlen(l_szText)), &l_dwWritten, NULL);
      }
    }

    CloseHandle(l_hTextFile);
  }

  char l_szLog[512];
  _snprintf_s(l_szLog,
              sizeof(l_szLog),
              _TRUNCATE,
              "Dumped reged_char_result_zone payload=%d path=%s ret=%u charNum=%u",
              pi_nPayloadSize,
              kRegedCharResultDumpPath,
              static_cast<unsigned>(l_sPacket.byRetCode),
              static_cast<unsigned>(l_sPacket.byCharNum));
  AppendNetworkLog(l_szLog);
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
    m_bHasUILockInform(FALSE),
    m_bUseAopTransportCrypt(false),
    m_bHasRetriedAopTransportCrypt(false),
    m_hAopAesAlgorithm(NULL),
    m_hAopClientEncryptKey(NULL),
    m_hAopServerDecryptKey(NULL),
    m_dwAopKeyObjectSize(0)
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
  ShutdownAopTransportCrypt();

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
  if (IsAopClientRoot())
  {
    if (!m_bUseAopTransportCrypt)
    {
      SetStatusText("AOP transport crypt enabled from client root");
    }
    m_bUseAopTransportCrypt = true;
    m_bHasRetriedAopTransportCrypt = true;
  }
  else if (!m_bHasRetriedAopTransportCrypt)
  {
    m_bUseAopTransportCrypt = false;
  }

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

  AppendPacketTrace("TX",
                    l_sHeader,
                    pi_nSize > 0 ? l_vecSendBuffer.data() + MSG_HEADER_SIZE : NULL,
                    pi_nSize,
                    m_bUseAopTransportCrypt);

  std::vector<char> l_vecWirePacket;
  if (!WrapOutgoingPacket(l_vecSendBuffer.data(), static_cast<int>(l_vecSendBuffer.size()), l_vecWirePacket))
  {
    return FALSE;
  }

  return SendBuffer(l_vecWirePacket.data(), static_cast<int>(l_vecWirePacket.size())) ? TRUE : FALSE;
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
  DumpRegedCharResultPayload(pi_pMsg, pi_nPayloadSize);
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

BOOL CNetworkMgr::LoadRegedCharResultDump(const char *pi_pPath)
{
  std::vector<char> l_vecPayload;
  if (!ReadBinaryFile(pi_pPath, l_vecPayload))
  {
    SetStatusText("Offline replay failed: cannot read %s", pi_pPath ? pi_pPath : "(null)");
    return FALSE;
  }

  if (l_vecPayload.size() < 14)
  {
    SetStatusText("Offline replay failed: reged payload too small size=%lu",
                  static_cast<unsigned long>(l_vecPayload.size()));
    return FALSE;
  }

  ResetConnectionState();
  m_bHasLauncherData = FALSE;
  m_bSentEnterWorldRequest = TRUE;
  m_bHasEnterWorldResult = TRUE;
  m_byResultOfEnterTheWorldServer = WORLD_ENTER_SUCCESS;
  m_bSentRegedCharRequest = TRUE;
  SystemMsg_RegedCharResult_zone(l_vecPayload.data(), static_cast<int>(l_vecPayload.size()));
  SetStatusText("Offline replay loaded reged_char_result_zone path=%s size=%lu",
                pi_pPath,
                static_cast<unsigned long>(l_vecPayload.size()));
  return TRUE;
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
      if (!m_bUseAopTransportCrypt && m_bSentEnterWorldRequest && !m_bHasEnterWorldResult)
      {
        EnableAopTransportCryptRetry("world server closed after raw enter-world request");
      }
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

      if (!m_bUseAopTransportCrypt && m_bSentEnterWorldRequest && !m_bHasEnterWorldResult)
      {
        EnableAopTransportCryptRetry("recv failed before enter-world result");
      }
      SetStatusText("recv failed error=%d", l_nError);
      CloseSocket();
      ResetConnectionState();
      return;
    }

    m_vecRecvBuffer.insert(m_vecRecvBuffer.end(), l_szRecvBuffer, l_szRecvBuffer + l_nRecv);
  }

  while (true)
  {
    _MSG_HEADER l_sHeader;
    std::vector<char> l_vecPacketBuffer;
    size_t l_nConsumedBytes = 0;
    if (!TryPopIncomingPacket(l_sHeader, l_vecPacketBuffer, l_nConsumedBytes))
    {
      break;
    }

    const int l_nPayloadSize = static_cast<int>(l_sHeader.m_wSize) - MSG_HEADER_SIZE;
    const char *l_pPayload = l_vecPacketBuffer.data() + MSG_HEADER_SIZE;
    AppendPacketTrace("RX", l_sHeader, l_pPayload, l_nPayloadSize, m_bUseAopTransportCrypt);
    ProcessPacket(l_sHeader, l_pPayload, l_nPayloadSize);

    m_vecRecvBuffer.erase(m_vecRecvBuffer.begin(), m_vecRecvBuffer.begin() + l_nConsumedBytes);
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

bool CNetworkMgr::EnsureAopTransportCrypt(void)
{
  if (m_hAopClientEncryptKey && m_hAopServerDecryptKey)
  {
    return true;
  }

  if (!m_hAopAesAlgorithm)
  {
    if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&m_hAopAesAlgorithm,
                                                    BCRYPT_AES_ALGORITHM,
                                                    NULL,
                                                    0)))
    {
      SetStatusText("BCryptOpenAlgorithmProvider failed for AOP transport crypt");
      ShutdownAopTransportCrypt();
      return false;
    }

    if (!BCRYPT_SUCCESS(BCryptSetProperty(m_hAopAesAlgorithm,
                                          BCRYPT_CHAINING_MODE,
                                          reinterpret_cast<PUCHAR>(const_cast<wchar_t *>(BCRYPT_CHAIN_MODE_ECB)),
                                          static_cast<ULONG>((wcslen(BCRYPT_CHAIN_MODE_ECB) + 1) * sizeof(wchar_t)),
                                          0)))
    {
      SetStatusText("BCryptSetProperty ECB failed for AOP transport crypt");
      ShutdownAopTransportCrypt();
      return false;
    }

    ULONG l_dwResultSize = 0;
    if (!BCRYPT_SUCCESS(BCryptGetProperty(m_hAopAesAlgorithm,
                                          BCRYPT_OBJECT_LENGTH,
                                          reinterpret_cast<PUCHAR>(&m_dwAopKeyObjectSize),
                                          sizeof(m_dwAopKeyObjectSize),
                                          &l_dwResultSize,
                                          0)))
    {
      SetStatusText("BCryptGetProperty object length failed for AOP transport crypt");
      ShutdownAopTransportCrypt();
      return false;
    }
  }

  if (!m_dwAopKeyObjectSize)
  {
    SetStatusText("Invalid AOP transport key object size");
    ShutdownAopTransportCrypt();
    return false;
  }

  std::vector<unsigned char> l_vecClientKeyBytes;
  AppendAopKeyBytes(l_vecClientKeyBytes, kAopClientEncKeyWords);
  std::vector<unsigned char> l_vecServerKeyBytes;
  AppendAopKeyBytes(l_vecServerKeyBytes, kAopServerEncKeyWords);

  m_vecAopClientKeyObject.assign(m_dwAopKeyObjectSize, 0);
  if (!BCRYPT_SUCCESS(BCryptGenerateSymmetricKey(m_hAopAesAlgorithm,
                                                 &m_hAopClientEncryptKey,
                                                 m_vecAopClientKeyObject.data(),
                                                 static_cast<ULONG>(m_vecAopClientKeyObject.size()),
                                                 l_vecClientKeyBytes.data(),
                                                 static_cast<ULONG>(l_vecClientKeyBytes.size()),
                                                 0)))
  {
    SetStatusText("BCryptGenerateSymmetricKey failed for AOP client encrypt key");
    ShutdownAopTransportCrypt();
    return false;
  }

  m_vecAopServerKeyObject.assign(m_dwAopKeyObjectSize, 0);
  if (!BCRYPT_SUCCESS(BCryptGenerateSymmetricKey(m_hAopAesAlgorithm,
                                                 &m_hAopServerDecryptKey,
                                                 m_vecAopServerKeyObject.data(),
                                                 static_cast<ULONG>(m_vecAopServerKeyObject.size()),
                                                 l_vecServerKeyBytes.data(),
                                                 static_cast<ULONG>(l_vecServerKeyBytes.size()),
                                                 0)))
  {
    SetStatusText("BCryptGenerateSymmetricKey failed for AOP server decrypt key");
    ShutdownAopTransportCrypt();
    return false;
  }

  return true;
}

void CNetworkMgr::ShutdownAopTransportCrypt(void)
{
  if (m_hAopClientEncryptKey)
  {
    BCryptDestroyKey(m_hAopClientEncryptKey);
    m_hAopClientEncryptKey = NULL;
  }

  if (m_hAopServerDecryptKey)
  {
    BCryptDestroyKey(m_hAopServerDecryptKey);
    m_hAopServerDecryptKey = NULL;
  }

  if (m_hAopAesAlgorithm)
  {
    BCryptCloseAlgorithmProvider(m_hAopAesAlgorithm, 0);
    m_hAopAesAlgorithm = NULL;
  }

  m_dwAopKeyObjectSize = 0;
  m_vecAopClientKeyObject.clear();
  m_vecAopServerKeyObject.clear();
}

void CNetworkMgr::EnableAopTransportCryptRetry(const char *pi_pReason)
{
  if (m_bUseAopTransportCrypt || m_bHasRetriedAopTransportCrypt)
  {
    return;
  }

  m_bUseAopTransportCrypt = true;
  m_bHasRetriedAopTransportCrypt = true;
  SetStatusText("Enabling AOP transport crypt retry: %s",
                pi_pReason ? pi_pReason : "world line compatibility fallback");
}

bool CNetworkMgr::WrapOutgoingPacket(const char *pi_pPlainPacket,
                                     int pi_nPlainSize,
                                     std::vector<char> &po_vecWirePacket)
{
  po_vecWirePacket.clear();

  if (!pi_pPlainPacket || pi_nPlainSize <= 0)
  {
    return false;
  }

  if (!m_bUseAopTransportCrypt)
  {
    po_vecWirePacket.assign(pi_pPlainPacket, pi_pPlainPacket + pi_nPlainSize);
    return true;
  }

  if (!EnsureAopTransportCrypt())
  {
    return false;
  }

  const BYTE l_byPaddingSize = static_cast<BYTE>((16 - (pi_nPlainSize & 0xF)) & 0xF);
  const int l_nCipherSize = pi_nPlainSize + l_byPaddingSize;
  const WORD l_wWireSize = static_cast<WORD>(l_nCipherSize + 3);

  po_vecWirePacket.assign(l_wWireSize, 0);
  WriteWordLE(po_vecWirePacket.data(), l_wWireSize);
  po_vecWirePacket[2] = static_cast<char>(l_byPaddingSize);
  AppendNetworkLog("AOP transport crypt: wrapping outgoing packet");

  unsigned char l_byVector[16] = {};
  ULONG l_dwResultSize = 0;
  const unsigned char *l_pInput = reinterpret_cast<const unsigned char *>(pi_pPlainPacket);
  unsigned char *l_pOutput = reinterpret_cast<unsigned char *>(po_vecWirePacket.data() + 3);
  int l_nRemaining = pi_nPlainSize;

  while (l_nRemaining > 0)
  {
    unsigned char l_byBlock[16];
    unsigned char l_byCipher[16];
    const int l_nBlockSize = MinInt(l_nRemaining, 16);

    for (int i = 0; i < l_nBlockSize; ++i)
    {
      l_byBlock[i] = static_cast<unsigned char>(l_byVector[i] ^ l_pInput[i]);
    }
    for (int i = l_nBlockSize; i < 16; ++i)
    {
      l_byBlock[i] = l_byVector[i];
    }

    if (!BCRYPT_SUCCESS(BCryptEncrypt(m_hAopClientEncryptKey,
                                      l_byBlock,
                                      sizeof(l_byBlock),
                                      NULL,
                                      NULL,
                                      0,
                                      l_byCipher,
                                      sizeof(l_byCipher),
                                      &l_dwResultSize,
                                      0)) ||
        l_dwResultSize != sizeof(l_byCipher))
    {
      SetStatusText("BCryptEncrypt failed for AOP transport packet");
      return false;
    }

    memcpy(l_pOutput, l_byCipher, sizeof(l_byCipher));
    memcpy(l_byVector, l_byCipher, sizeof(l_byVector));
    l_pOutput += sizeof(l_byCipher);
    l_pInput += l_nBlockSize;
    l_nRemaining -= l_nBlockSize;
  }

  return true;
}

bool CNetworkMgr::TryPopIncomingPacket(_MSG_HEADER &po_sHeader,
                                       std::vector<char> &po_vecPacketBuffer,
                                       size_t &po_nConsumedBytes)
{
  po_nConsumedBytes = 0;
  po_vecPacketBuffer.clear();

  if (!m_bUseAopTransportCrypt)
  {
    if (m_vecRecvBuffer.size() < MSG_HEADER_SIZE)
    {
      return false;
    }

    memcpy(&po_sHeader, m_vecRecvBuffer.data(), MSG_HEADER_SIZE);
    if (po_sHeader.m_wSize < MSG_HEADER_SIZE)
    {
      SetStatusText("Invalid packet size=%u", po_sHeader.m_wSize);
      CloseSocket();
      ResetConnectionState();
      return false;
    }

    if (m_vecRecvBuffer.size() < po_sHeader.m_wSize)
    {
      return false;
    }

    po_vecPacketBuffer.assign(m_vecRecvBuffer.begin(), m_vecRecvBuffer.begin() + po_sHeader.m_wSize);
    po_nConsumedBytes = po_sHeader.m_wSize;
    return true;
  }

  if (m_vecRecvBuffer.size() < 3)
  {
    return false;
  }

  if (!EnsureAopTransportCrypt())
  {
    CloseSocket();
    ResetConnectionState();
    return false;
  }

  const WORD l_wWireSize = ReadWordLE(m_vecRecvBuffer.data());
  const BYTE l_byPaddingSize = static_cast<BYTE>(m_vecRecvBuffer[2]);
  if (l_wWireSize < 19 || (l_wWireSize - 3) % 16 != 0 || l_byPaddingSize > 15)
  {
    SetStatusText("Invalid AOP transport packet size=%u padding=%u",
                  static_cast<unsigned>(l_wWireSize),
                  static_cast<unsigned>(l_byPaddingSize));
    CloseSocket();
    ResetConnectionState();
    return false;
  }

  if (m_vecRecvBuffer.size() < l_wWireSize)
  {
    return false;
  }

  const int l_nCipherSize = l_wWireSize - 3;
  const int l_nPlainSize = l_nCipherSize - l_byPaddingSize;
  if (l_nPlainSize < static_cast<int>(MSG_HEADER_SIZE))
  {
    SetStatusText("Invalid decrypted packet size=%d", l_nPlainSize);
    CloseSocket();
    ResetConnectionState();
    return false;
  }

  po_vecPacketBuffer.assign(l_nPlainSize, 0);
  unsigned char l_byVector[16] = {};
  ULONG l_dwResultSize = 0;
  const unsigned char *l_pInput = reinterpret_cast<const unsigned char *>(m_vecRecvBuffer.data() + 3);
  unsigned char *l_pOutput = reinterpret_cast<unsigned char *>(po_vecPacketBuffer.data());

  for (int l_nOffset = 0; l_nOffset < l_nCipherSize; l_nOffset += 16)
  {
    unsigned char l_byBlock[16];
    unsigned char l_byPlainBlock[16];

    if (!BCRYPT_SUCCESS(BCryptDecrypt(m_hAopServerDecryptKey,
                                      const_cast<PUCHAR>(l_pInput + l_nOffset),
                                      16,
                                      NULL,
                                      NULL,
                                      0,
                                      l_byBlock,
                                      sizeof(l_byBlock),
                                      &l_dwResultSize,
                                      0)) ||
        l_dwResultSize != sizeof(l_byBlock))
    {
      SetStatusText("BCryptDecrypt failed for AOP transport packet");
      CloseSocket();
      ResetConnectionState();
      return false;
    }

    for (int i = 0; i < 16; ++i)
    {
      l_byPlainBlock[i] = static_cast<unsigned char>(l_byVector[i] ^ l_byBlock[i]);
    }

    const int l_nCopySize = MinInt(l_nPlainSize - l_nOffset, 16);
    memcpy(l_pOutput + l_nOffset, l_byPlainBlock, l_nCopySize);
    memcpy(l_byVector, l_pInput + l_nOffset, sizeof(l_byVector));
  }

  memcpy(&po_sHeader, po_vecPacketBuffer.data(), MSG_HEADER_SIZE);
  if (po_sHeader.m_wSize != static_cast<WORD>(l_nPlainSize) || po_sHeader.m_wSize < MSG_HEADER_SIZE)
  {
    SetStatusText("Invalid decrypted packet size=%u plain=%d",
                  po_sHeader.m_wSize,
                  l_nPlainSize);
    CloseSocket();
    ResetConnectionState();
    return false;
  }

  po_nConsumedBytes = l_wWireSize;
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
