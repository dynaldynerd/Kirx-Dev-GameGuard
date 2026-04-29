#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include <bcrypt.h>
#include <vector>

#include "ClientZonePacket.h"
#include "NetCheckPackets.h"
#include "NetProcess.h"
#include "Pt_Zone_Client.h"
#include "Protocol.h"
#include "ZoneClientPacket.h"

class CNetworkMgr
{
public:
  CNetworkMgr();
  ~CNetworkMgr();

  BOOL Create(void);
  BOOL Destroy(void);
  void OnLoop(void);
  void Connect(BYTE pi_byServerType);
  void Close(BYTE pi_byServerType);

  BOOL SendNetMessage(DWORD pi_dwClientIndex, BYTE *pi_pType, void *pi_pMsg, int pi_nSize);
  BOOL SystemMsg_EnterWorldRequest_zone(void);
  void SystemMsg_EnterWorldResult_zone(char *pi_pMsg);
  BOOL SystemMsg_RegedCharRequest_zone(void);
  void SystemMsg_RegedCharResult_zone(char *pi_pMsg, int pi_nPayloadSize);
  BOOL LoadRegedCharResultDump(const char *pi_pPath);
  BOOL SystemMsg_AddCharRequest_zone(BYTE pi_byAvatarIndex,
                                     const char *pi_pName,
                                     BYTE pi_byRaceSexCode,
                                     const char *pi_pClassCode,
                                     DWORD pi_dwBaseShape);
  void SystemMsg_AddCharResult_zone(char *pi_pMsg, int pi_nPayloadSize);
  BOOL SystemMsg_DelCharRequest_zone(BYTE pi_byAvatarIndex);
  void SystemMsg_DelCharResult_zone(char *pi_pMsg, int pi_nPayloadSize);
  BOOL SystemMsg_SelCharRequest_zone(BYTE pi_byAvatarIndex);
  void SystemMsg_SelCharResult_zone(char *pi_pMsg, int pi_nPayloadSize);
  BOOL CheckMsg_CheckAnswer(void);
  BOOL SecureMsg_CheckSpeedHackAnswer(void);

  void SetID(char *pi_pID);
  void SetWorldServerKey(DWORD *pi_pKey);

  BYTE GetAvatarGrade(void) const { return m_byAvatarGrade; }
  char GetActiveServerID(void) const { return m_nActiveServerID; }
  BOOL IsConnectedToServer(BYTE pi_byServerType) const { return m_bIsConnectedToServer[pi_byServerType]; }
  BOOL IsConnectingToServer(void) const { return m_bIsConnecting ? TRUE : FALSE; }

  void SetAccountIndex(DWORD pi_dwIndex) { m_dwAccountIndex = pi_dwIndex; }
  DWORD GetAccountIndex(void) const { return m_dwAccountIndex; }
  void SetAvatarIndex(DWORD pi_dwIndex) { m_dwAvatarIndex = pi_dwIndex; }
  DWORD GetAvatarIndex(void) const { return m_dwAvatarIndex; }

  char *GetID(void) { return m_pID; }

  void SetResultOfEnterTheWorldServer(BYTE pi_byResult) { m_byResultOfEnterTheWorldServer = pi_byResult; }
  BYTE GetResultOfEnterTheWorldServer(void) const { return m_byResultOfEnterTheWorldServer; }
  void SetResultOfUserInfo(BYTE pi_byResult) { m_byResultOfUserInfo = pi_byResult; }
  BYTE GetResultOfUserInfo(void) const { return m_byResultOfUserInfo; }
  void SetResultOfCharacterSelect(BYTE pi_byResult) { m_byResultOfCharacterSelect = pi_byResult; }
  BYTE GetResultOfCharacterSelect(void) const { return m_byResultOfCharacterSelect; }
  BYTE GetEnterWorldServerType(void) const { return m_byEnterWorldServerType; }
  DWORD GetLastestAvatarIndex(void) const { return m_dwLastestAvatarIndex; }

  void SetServerIP(BYTE pi_byType, DWORD pi_dwIP) { m_dwServerIP[pi_byType] = pi_dwIP; }
  DWORD GetServerIP(BYTE pi_byType) const { return m_dwServerIP[pi_byType]; }

  void SetServerPort(BYTE pi_byType, DWORD pi_dwPort) { m_dwServerPort[pi_byType] = pi_dwPort; }
  DWORD GetServerPort(BYTE pi_byType) const { return m_dwServerPort[pi_byType]; }

  void SetHasLauncherData(BOOL pi_bFlag) { m_bHasLauncherData = pi_bFlag; }
  BOOL HasLauncherData(void) const { return m_bHasLauncherData; }

  BOOL HasSentEnterWorldRequest(void) const { return m_bSentEnterWorldRequest; }
  BOOL HasEnterWorldResult(void) const { return m_bHasEnterWorldResult; }
  BOOL HasSentRegedCharRequest(void) const { return m_bSentRegedCharRequest; }
  BOOL HasRegedCharResult(void) const { return m_bHasRegedCharResult; }
  BOOL HasSentAddCharRequest(void) const { return m_bSentAddCharRequest; }
  BOOL HasAddCharResult(void) const { return m_bHasAddCharResult; }
  BOOL HasSentDelCharRequest(void) const { return m_bSentDelCharRequest; }
  BOOL HasDelCharResult(void) const { return m_bHasDelCharResult; }
  BOOL HasSentSelCharRequest(void) const { return m_bSentSelCharRequest; }
  BOOL HasSelCharResult(void) const { return m_bHasSelCharResult; }
  const _reged_char_result_zone &GetRegedCharResult(void) const { return m_sRegedCharResult; }
  const _add_char_result_zone &GetAddCharResult(void) const { return m_sAddCharResult; }
  const _del_char_result_zone &GetDelCharResult(void) const { return m_sDelCharResult; }
  const _sel_char_result_zone &GetSelCharResult(void) const { return m_sSelCharResult; }
  const _not_arranged_char_inform_zocl &GetNotArrangedCharInform(void) const { return m_sNotArrangedCharInform; }
  BOOL HasUILockInform(void) const { return m_bHasUILockInform; }
  const _uilock_inform_request_zocl &GetUILockInform(void) const { return m_sUILockInform; }
  DWORD GetSelectedDalant(void) const { return m_dwSelectedDalant; }
  DWORD GetSelectedGold(void) const { return m_dwSelectedGold; }
  const char *GetStatusText(void) const { return m_szStatusText; }

  void ClearAddCharResult(void);
  void ClearDelCharResult(void);
  void SetStatusText(const char *pi_pFormat, ...);
  const char *GetAddressText(DWORD pi_dwAddress);

private:
  bool EnsureAopTransportCrypt(void);
  void ShutdownAopTransportCrypt(void);
  void EnableAopTransportCryptRetry(const char *pi_pReason);
  bool WrapOutgoingPacket(const char *pi_pPlainPacket, int pi_nPlainSize, std::vector<char> &po_vecWirePacket);
  bool TryPopIncomingPacket(_MSG_HEADER &po_sHeader,
                            std::vector<char> &po_vecPacketBuffer,
                            size_t &po_nConsumedBytes);

private:
  void AcceptClientCheck(DWORD dwProID, DWORD dwClientIndex, DWORD dwSerial);
  void CloseClientCheck(DWORD dwProID, DWORD dwClientIndex, DWORD dwSerial);

  void ResetConnectionState(void);
  void CloseSocket(void);
  void PumpConnect(void);
  void PumpReceive(void);
  void _ResponSpeedHack(void);
  bool SendBuffer(const char *pi_pBuffer, int pi_nSize);
  bool ProcessPacket(const _MSG_HEADER &pi_sHeader, const char *pi_pPayload, int pi_nPayloadSize);

private:
  SOCKET m_hSocket;
  BOOL m_bCreated;
  BOOL m_bWSAStarted;
  bool m_bIsConnecting;
  BOOL m_bHasLauncherData;
  BOOL m_bIsConnectedToServer[MAX_GAME_SERVER];
  DWORD m_dwServerIP[MAX_GAME_SERVER];
  DWORD m_dwServerPort[MAX_GAME_SERVER];
  char m_pServerName[MAX_GAME_SERVER][32];
  char m_nActiveServerID;
  DWORD m_dwAccountIndex;
  DWORD m_dwAvatarIndex;
  DWORD m_dwLastestAvatarIndex;
  DWORD m_dwSelectedDalant;
  DWORD m_dwSelectedGold;
  DWORD m_dwWorldServerKey[KEY_NUM];
  DWORD m_dwSpeedHackKey[CHECK_KEY_NUM];
  DWORD m_dwResponSpeedHackTime;
  char m_pID[32];
  BYTE m_byAvatarGrade;
  BYTE m_byResultOfEnterTheWorldServer;
  BYTE m_byResultOfUserInfo;
  BYTE m_byResultOfCharacterSelect;
  BYTE m_byEnterWorldServerType;
  BOOL m_bSentEnterWorldRequest;
  BOOL m_bHasEnterWorldResult;
  BOOL m_bSentRegedCharRequest;
  BOOL m_bHasRegedCharResult;
  BOOL m_bSentAddCharRequest;
  BOOL m_bHasAddCharResult;
  BOOL m_bSentDelCharRequest;
  BOOL m_bHasDelCharResult;
  BOOL m_bSentSelCharRequest;
  BOOL m_bHasSelCharResult;
  BOOL m_bHasUILockInform;
  sockaddr_in m_ServerAddr;
  std::vector<char> m_vecRecvBuffer;
  _reged_char_result_zone m_sRegedCharResult;
  _add_char_result_zone m_sAddCharResult;
  _del_char_result_zone m_sDelCharResult;
  _sel_char_result_zone m_sSelCharResult;
  _not_arranged_char_inform_zocl m_sNotArrangedCharInform;
  _uilock_inform_request_zocl m_sUILockInform;
  char m_szStatusText[256];
  bool m_bUseAopTransportCrypt;
  bool m_bHasRetriedAopTransportCrypt;
  BCRYPT_ALG_HANDLE m_hAopAesAlgorithm;
  BCRYPT_KEY_HANDLE m_hAopClientEncryptKey;
  BCRYPT_KEY_HANDLE m_hAopServerDecryptKey;
  DWORD m_dwAopKeyObjectSize;
  std::vector<unsigned char> m_vecAopClientKeyObject;
  std::vector<unsigned char> m_vecAopServerKeyObject;
};

extern CNetworkMgr *g_pNetworkMgr;

#define _GetNetworkMgr() g_pNetworkMgr
