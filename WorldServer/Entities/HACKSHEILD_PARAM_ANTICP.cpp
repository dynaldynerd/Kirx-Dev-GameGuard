#include "pch.h"

#include "WorldServerUtil.h"

#include "HACKSHEILD_PARAM_ANTICP.h"

#include <cstring>

#include "CAsyncLogger.h"
#include "CUserDB.h"
#include "GlobalObjectDefs.h"
#include "GlobalObjects.h"
#include "enter_world_result_zone.h"

namespace
{
using AntiCpMakeGuidReqMsgFn = unsigned int(__cdecl *)(void *pOutMsg, unsigned __int8 *pGuidClientInfo);
using AntiCpMakeReqMsgFn = int(__cdecl *)(
  _HSHIELD_CLIENT_CONTEXT *pContext,
  void *pOutMsg,
  unsigned __int8 *pGuidClientInfo,
  unsigned int dwType);
using AntiCpAnalyzeGuidAckMsgFn = unsigned int(__cdecl *)(
  char *pMsg,
  unsigned __int8 *pGuidClientInfo,
  _HSHIELD_CLIENT_CONTEXT *pContext);

HMODULE _ResolveAntiCpModule()
{
  static HMODULE s_hModule = nullptr;
  if (s_hModule)
  {
    return s_hModule;
  }

  const char *kCandidateModules[] = {
    "AntiCpSvr.dll",
    "EhSvc.dll",
    "HackShield.dll",
    "RFACC.dll",
  };

  for (const char *moduleName : kCandidateModules)
  {
    HMODULE module = GetModuleHandleA(moduleName);
    if (!module)
    {
      module = LoadLibraryA(moduleName);
    }

    if (module)
    {
      s_hModule = module;
      return s_hModule;
    }
  }

  return nullptr;
}

FARPROC _ResolveAntiCpProc(const char *procNameUnderscore, const char *procNameNoUnderscore)
{
  HMODULE module = _ResolveAntiCpModule();
  if (!module)
  {
    return nullptr;
  }

  FARPROC proc = GetProcAddress(module, procNameUnderscore);
  if (proc)
  {
    return proc;
  }

  return GetProcAddress(module, procNameNoUnderscore);
}

AntiCpMakeGuidReqMsgFn _ResolveMakeGuidReqMsg()
{
  static AntiCpMakeGuidReqMsgFn s_fn = nullptr;
  static bool s_resolved = false;
  if (!s_resolved)
  {
    s_resolved = true;
    s_fn = reinterpret_cast<AntiCpMakeGuidReqMsgFn>(
      _ResolveAntiCpProc("_AntiCpSvr_MakeGuidReqMsg", "AntiCpSvr_MakeGuidReqMsg"));
  }
  return s_fn;
}

AntiCpMakeReqMsgFn _ResolveMakeReqMsg()
{
  static AntiCpMakeReqMsgFn s_fn = nullptr;
  static bool s_resolved = false;
  if (!s_resolved)
  {
    s_resolved = true;
    s_fn = reinterpret_cast<AntiCpMakeReqMsgFn>(
      _ResolveAntiCpProc("_AntiCpSvr_MakeReqMsg", "AntiCpSvr_MakeReqMsg"));
  }
  return s_fn;
}

AntiCpAnalyzeGuidAckMsgFn _ResolveAnalyzeGuidAckMsg()
{
  static AntiCpAnalyzeGuidAckMsgFn s_fn = nullptr;
  static bool s_resolved = false;
  if (!s_resolved)
  {
    s_resolved = true;
    s_fn = reinterpret_cast<AntiCpAnalyzeGuidAckMsgFn>(
      _ResolveAntiCpProc("_AntiCpSvr_AnalyzeGuidAckMsg", "AntiCpSvr_AnalyzeGuidAckMsg"));
  }
  return s_fn;
}

const char *_KickReason(unsigned __int8 byReason)
{
  if (byReason == 1)
  {
    return "KICK_REASON_CRC_ACK_DELAY";
  }

  switch (byReason)
  {
    case 2:
      return "KICK_REASON_FIRST_GUID_INVALID";
    case 3:
      return "KICK_REASON_CRC_ACK_INVALID";
    case 4:
      return "KICK_REASON_NOT_HACKSHEILD_CLIENT";
    default:
      return "What ??? ";
  }
}
} // namespace

HACKSHEILD_PARAM_ANTICP::HACKSHEILD_PARAM_ANTICP()
{
  Init();
}

void HACKSHEILD_PARAM_ANTICP::Init()
{
  m_nSocketIndex = -1;
  m_dwLastSyncQryTime = 0;
  m_byVerifyState = 0;
  memset_0(&m_CrcInfo, 0, sizeof(m_CrcInfo));
  memset_0(m_byGUIDClientInfo, 0, sizeof(m_byGUIDClientInfo));
}

void HACKSHEILD_PARAM_ANTICP::CheckClient()
{
  char reqPayload[0xA0]{};
  unsigned int reqType = (m_byVerifyState == 2) ? 15u : 1u;
  AntiCpMakeReqMsgFn makeReqMsg = _ResolveMakeReqMsg();
  const int reqResult = makeReqMsg ? makeReqMsg(&m_CrcInfo, reqPayload, m_byGUIDClientInfo, reqType) : -1;
  m_dwLastSyncQryTime = GetLoopTime();

  if (reqResult)
  {
    CAsyncLogger *logger = CAsyncLogger::Instance();
    logger->FormatLog(
      CAsyncLogger::ALT_HACKSHIELD_SYSTEM_LOG,
      "_AntiCpSvr_MakeReqMsg n(%d) Err(%u) Error!",
      m_nSocketIndex,
      reqResult);
    return;
  }

  m_byVerifyState = 3;

  char msg[0xA2]{};
  *reinterpret_cast<unsigned __int16 *>(msg) = 0;
  memcpy_0(msg + 2, reqPayload, sizeof(reqPayload));

  unsigned __int8 type[2]{};
  type[0] = 98;
  type[1] = 4;
  g_Network.m_pProcess[0]->LoadSendMsg(m_nSocketIndex, type, msg, 0xA2u);
}

void HACKSHEILD_PARAM_ANTICP::Kick(unsigned __int8 byReason, unsigned int dwRet)
{
  const char *reason = _KickReason(byReason);
  CAsyncLogger *logger = CAsyncLogger::Instance();
  logger->FormatLog(
    CAsyncLogger::ALT_HACKSHIELD_SYSTEM_LOG,
    "HACKSHEILD_PARAM_ANTICP::Kick n(%d) Reason ( %s ), ( %u ) Kick!",
    m_nSocketIndex,
    reason,
    dwRet);

  if (m_nSocketIndex < 0 || m_nSocketIndex >= MAX_PLAYER)
  {
    return;
  }

  CUserDB *user = &g_UserDB[m_nSocketIndex];
  if (byReason == 4)
  {
    _enter_world_result_zone result{};
    result.byResult = static_cast<unsigned __int8>(-14);
    result.byUserGrade = 0;
    result.bySvrType = g_Main.m_byWorldType;

    unsigned __int8 type[2]{};
    type[0] = 1;
    type[1] = 2;
    g_Network.m_pProcess[0]->LoadSendMsg(
      user->m_idWorld.wIndex,
      type,
      reinterpret_cast<char *>(&result),
      static_cast<unsigned __int16>(result.size()));
    user->ForceCloseCommand(0xEu, 0xFFFFFFFFu, true, "HackShield Kick");
    return;
  }

  if (m_byVerifyState && user->m_bActive)
  {
    user->ForceCloseCommand(0xEu, 0xFFFFFFFFu, false, "HackShield Kick");
  }
}

char HACKSHEILD_PARAM_ANTICP::OnRecvSession_ServerCheckSum_Request(unsigned int nIndex)
{
  Init();

  char guidRequest[20]{};
  char msg[0x16]{};
  AntiCpMakeGuidReqMsgFn makeGuidReqMsg = _ResolveMakeGuidReqMsg();
  const unsigned int ret = makeGuidReqMsg ? makeGuidReqMsg(guidRequest, m_byGUIDClientInfo) : 1u;
  *reinterpret_cast<unsigned __int16 *>(msg) = static_cast<unsigned __int16>(ret);
  memcpy_0(msg + 2, guidRequest, sizeof(guidRequest));

  unsigned __int8 type[2]{};
  type[0] = 98;
  type[1] = 2;
  g_Network.m_pProcess[0]->LoadSendMsg(nIndex, type, msg, 0x16u);

  if (ret)
  {
    Kick(2u, ret);
    Init();
    return 1;
  }

  m_nSocketIndex = static_cast<int>(nIndex);
  m_byVerifyState = 1;
  return 1;
}

char HACKSHEILD_PARAM_ANTICP::OnRecvSession_ClientCheckSum_Response(unsigned __int64 tSize, char *pMsg)
{
  if (tSize != 342)
  {
    return 0;
  }

  if (m_nSocketIndex < 0 || m_nSocketIndex >= MAX_PLAYER)
  {
    return 0;
  }

  if (m_byVerifyState == 1)
  {
    AntiCpAnalyzeGuidAckMsgFn analyzeGuidAckMsg = _ResolveAnalyzeGuidAckMsg();
    const unsigned int ret = analyzeGuidAckMsg ? analyzeGuidAckMsg(pMsg + 2, m_byGUIDClientInfo, &m_CrcInfo) : 1u;
    if (ret)
    {
      Kick(2u, ret);
    }
    else
    {
      char msg[2]{};
      *reinterpret_cast<unsigned __int16 *>(msg) = 0;

      unsigned __int8 type[2]{};
      type[0] = 98;
      type[1] = 6;
      g_Network.m_pProcess[0]->LoadSendMsg(m_nSocketIndex, type, msg, 2u);
      m_byVerifyState = 2;
    }
    return 1;
  }

  Kick(2u, 0xFFFFFFFFu);
  return 1;
}
