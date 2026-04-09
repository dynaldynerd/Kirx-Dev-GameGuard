#include "pch.h"

#include "HACKSHEILD_PARAM_ANTICP.h"

#include <crtdbg.h>
#include <cstring>

#include "CAsyncLogger.h"
#include "CHackShieldExSystem.h"
#include "CNetProcess.h"
#include "CUserDB.h"
#include "GlobalObjects.h"
#include "Packet/ClientZonePacket.h"
#include "Packet/ZoneClientPacket.h"
#include "WorldServerUtil.h"
#include "enter_world_result_zone.h"

namespace
{
using AhnHSCreateServerObjectFn = unsigned __int64(__cdecl *)(const char *hsbPath);
using AhnHSCloseServerHandleFn = void(__cdecl *)(unsigned __int64 hServer);
using AhnHSCreateClientObjectFn = unsigned __int64(__cdecl *)(unsigned __int64 hServer);
using AhnHSCloseClientHandleFn = void(__cdecl *)(unsigned __int64 hClient);
using AhnHSMakeRequestFn = unsigned int(__cdecl *)(unsigned __int64 hClient, _AHNHS_TRANS_BUFFER *pRequestBuffer);
using AhnHSVerifyResponseExFn = unsigned int(__cdecl *)(
  unsigned __int64 hClient,
  const void *pResponse,
  unsigned int nLength,
  unsigned int *pErrorCode);

HMODULE ResolveHackShieldModule()
{
  static HMODULE s_hModule = nullptr;
  if (s_hModule)
  {
    return s_hModule;
  }

  const char *kCandidateModules[] = {
    "EhSvc.dll",
    "HackShield.dll",
    "HShield.dll",
    "AntiCpSvr.dll",
  };

  for (const char *moduleName : kCandidateModules)
  {
    HMODULE hModule = GetModuleHandleA(moduleName);
    if (!hModule)
    {
      hModule = LoadLibraryA(moduleName);
    }
    if (hModule)
    {
      s_hModule = hModule;
      return s_hModule;
    }
  }

  return nullptr;
}

FARPROC ResolveHackShieldProc(const char *pszName)
{
  HMODULE hModule = ResolveHackShieldModule();
  if (!hModule)
  {
    return nullptr;
  }

  FARPROC proc = GetProcAddress(hModule, pszName);
  if (proc)
  {
    return proc;
  }

  if (pszName[0] == '_')
  {
    return GetProcAddress(hModule, pszName + 1);
  }

  char altName[128]{};
  altName[0] = '_';
  strcpy_s(altName + 1, sizeof(altName) - 1, pszName);
  return GetProcAddress(hModule, altName);
}

const char *_KickReason(unsigned __int8 byReason)
{
  switch (byReason)
  {
    case 1:
      return "KICK_REASON_NOT_HACKSHEILD_CLIENT";
    case 2:
      return "KICK_REASON_CREATE_FAILED_CLIENT_HANDLE";
    case 3:
      return "KICK_REASON_CREATE_FAILED_MAKE_REQUEST";
    case 4:
      return "KICK_REASON_INVALID_VERIFY_STATE";
    case 5:
      return "KICK_REASON_HACKING_DETECTED";
    case 6:
      return "KICK_REASON_ACK_DELAY";
    default:
      return "What ??? ";
  }
}
} // namespace

unsigned __int64 HACKSHEILD_PARAM_ANTICPX_5381::ms_hServer = 0;

HACKSHEILD_PARAM_ANTICPX_5381::HACKSHEILD_PARAM_ANTICPX_5381()
  : m_nSocketIndex(-1), m_dwLastSyncQryTime(0), m_byVerifyState(0), m_hClient(0), m_stRequestBuf{}
{
}

void HACKSHEILD_PARAM_ANTICPX_5381::Init()
{
  m_nSocketIndex = -1;
  m_dwLastSyncQryTime = 0;
  m_byVerifyState = 0;
  m_hClient = 0;
  std::memset(&m_stRequestBuf, 0, sizeof(m_stRequestBuf));
}

void HACKSHEILD_PARAM_ANTICPX_5381::CheckClient(bool bFirstChecker)
{
  const unsigned int dwRet = AhnHS_MakeRequest(m_hClient, &m_stRequestBuf);
  if (dwRet)
  {
    Kick(3, dwRet);
    return;
  }

  m_dwLastSyncQryTime = GetLoopTime();

  _hs_msg_req_qry_zocl send{};
  if (bFirstChecker)
  {
    m_byVerifyState = 1;
  }
  else
  {
    m_byVerifyState = 3;
  }

  std::memcpy(&send, &m_stRequestBuf, sizeof(send));
  unsigned __int8 byType[2]{98, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_nSocketIndex,
    byType,
    reinterpret_cast<char *>(&send),
    static_cast<unsigned __int16>(sizeof(send)));
}

void HACKSHEILD_PARAM_ANTICPX_5381::Kick(unsigned __int8 byReason, unsigned int dwRet)
{
  CAsyncLogger::Instance()->FormatLog(
    CAsyncLogger::ALT_HACKSHIELD_SYSTEM_LOG,
    "HACKSHEILD_PARAM_ANTICPX_5381::Kick n(%d) Reason ( %s ), ( %u ) Kick!",
    m_nSocketIndex,
    _KickReason(byReason),
    dwRet);

  if (m_nSocketIndex < 0 || m_nSocketIndex >= MAX_PLAYER)
  {
    return;
  }

  CUserDB *pUserDB = &g_UserDB[m_nSocketIndex];
  if (byReason == 1)
  {
    _enter_world_result_zone send{};
    send.byResult = static_cast<unsigned __int8>(-14);
    send.byUserGrade = 0;
    send.bySvrType = g_Main.m_byWorldType;

    unsigned __int8 byType[2]{1, 2};
    g_Network.m_pProcess[0]->LoadSendMsg(
      pUserDB->m_idWorld.wIndex,
      byType,
      reinterpret_cast<char *>(&send),
      send.size());
    pUserDB->ForceCloseCommand(14, 0xFFFFFFFF, true, "HackShield Kick");
  }
  else if (m_byVerifyState && pUserDB->m_bActive)
  {
    pUserDB->ForceCloseCommand(14, 0xFFFFFFFF, false, "HackShield Kick");
  }
}

bool HACKSHEILD_PARAM_ANTICPX_5381::OnRecvSession_First_MakeRequest(int nIndex)
{
  m_nSocketIndex = nIndex;
  _hs_msg_notify_zocl send{};
  send.byRet = 102;
  unsigned __int8 byType[2]{98, 4};
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_nSocketIndex,
    byType,
    reinterpret_cast<char *>(&send),
    static_cast<unsigned __int16>(sizeof(send)));
  m_dwLastSyncQryTime = GetLoopTime();
  m_byVerifyState = 4;
  return true;
}

bool HACKSHEILD_PARAM_ANTICPX_5381::OnRecvSession_VerifyResponse(unsigned __int64 tSize, _hs_msg_res_qry_clzo *pMsg)
{
  if (tSize != sizeof(_hs_msg_res_qry_clzo))
  {
    return false;
  }
  if (m_nSocketIndex < 0 || m_nSocketIndex >= MAX_PLAYER)
  {
    return false;
  }

  if (m_byVerifyState == 1 || m_byVerifyState == 3)
  {
    unsigned int dwErrorCode = 0;
    const unsigned int dwRet = AhnHS_VerifyResponseEx(m_hClient, pMsg, pMsg->AuthKey.nLength, &dwErrorCode);
    _hs_msg_notify_zocl send{};
    send.byRet = static_cast<char>(dwRet);
    if (dwRet == 101)
    {
      Kick(5, dwErrorCode);
      return true;
    }

    if (m_byVerifyState == 1)
    {
      unsigned __int8 byType[2]{98, 4};
      g_Network.m_pProcess[0]->LoadSendMsg(
        m_nSocketIndex,
        byType,
        reinterpret_cast<char *>(&send),
        static_cast<unsigned __int16>(sizeof(send)));
      m_byVerifyState = 2;
    }
    else
    {
      m_byVerifyState = 4;
    }
  }

  return true;
}

bool HACKSHEILD_PARAM_ANTICPX_5381::IsLogPass()
{
  return m_byVerifyState >= 2;
}

bool HACKSHEILD_PARAM_ANTICPX_5381::OnCheckSession_FirstVerify(int n)
{
  if (IsLogPass())
  {
    return true;
  }

  m_nSocketIndex = n;
  Kick(1, 0);
  return false;
}

void HACKSHEILD_PARAM_ANTICPX_5381::OnConnect(int nIndex)
{
  (void)nIndex;
  Init();
  m_hClient = AhnHS_CreateClientObject(ms_hServer);
  _ASSERTE(m_hClient != 0);
}

void HACKSHEILD_PARAM_ANTICPX_5381::OnDisConnect()
{
  if (m_hClient)
  {
    AhnHS_CloseClientHandle(m_hClient);
  }
  Init();
}

void HACKSHEILD_PARAM_ANTICPX_5381::OnLoop()
{
  // Intentional non-parity: mock HackShield never sends active challenge packets.
}

bool HACKSHEILD_PARAM_ANTICPX_5381::OnRecvSession(
  CHackShieldExSystem *mgr,
  int nIndex,
  unsigned __int8 byProtocol,
  unsigned __int64 tSize,
  char *pMsg)
{
  (void)mgr;
  (void)byProtocol;
  (void)tSize;
  (void)pMsg;

  // Intentional non-parity: mock HackShield accepts any client packet and returns success notify.
  m_nSocketIndex = nIndex;
  _hs_msg_notify_zocl send{};
  send.byRet = 102;
  unsigned __int8 byType[2]{98, 4};
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_nSocketIndex,
    byType,
    reinterpret_cast<char *>(&send),
    static_cast<unsigned __int16>(sizeof(send)));
  m_dwLastSyncQryTime = GetLoopTime();
  m_byVerifyState = 4;
  return true;
}

unsigned __int64 AhnHS_CreateServerObject(const char *hsbPath)
{
  static auto fn = reinterpret_cast<AhnHSCreateServerObjectFn>(ResolveHackShieldProc("_AhnHS_CreateServerObject"));
  return fn ? fn(hsbPath) : 0;
}

void AhnHS_CloseServerHandle(unsigned __int64 hServer)
{
  static auto fn = reinterpret_cast<AhnHSCloseServerHandleFn>(ResolveHackShieldProc("_AhnHS_CloseServerHandle"));
  if (fn && hServer)
  {
    fn(hServer);
  }
}

unsigned __int64 AhnHS_CreateClientObject(unsigned __int64 hServer)
{
  static auto fn = reinterpret_cast<AhnHSCreateClientObjectFn>(ResolveHackShieldProc("_AhnHS_CreateClientObject"));
  return fn ? fn(hServer) : 0;
}

void AhnHS_CloseClientHandle(unsigned __int64 hClient)
{
  static auto fn = reinterpret_cast<AhnHSCloseClientHandleFn>(ResolveHackShieldProc("_AhnHS_CloseClientHandle"));
  if (fn && hClient)
  {
    fn(hClient);
  }
}

unsigned int AhnHS_MakeRequest(unsigned __int64 hClient, _AHNHS_TRANS_BUFFER *pRequestBuffer)
{
  static auto fn = reinterpret_cast<AhnHSMakeRequestFn>(ResolveHackShieldProc("_AhnHS_MakeRequest"));
  return fn ? fn(hClient, pRequestBuffer) : 1;
}

unsigned int AhnHS_VerifyResponseEx(
  unsigned __int64 hClient,
  const void *pResponse,
  unsigned int nLength,
  unsigned int *pErrorCode)
{
  static auto fn = reinterpret_cast<AhnHSVerifyResponseExFn>(ResolveHackShieldProc("_AhnHS_VerifyResponseEx"));
  return fn ? fn(hClient, pResponse, nLength, pErrorCode) : 101;
}
