#include "pch.h"

#include "NPROTECT_PARAM_VER_30.h"

#include "CAsyncLogger.h"
#include "CNetProcess.h"
#include "CnProtectSystem.h"
#include "CUserDB.h"
#include "GlobalObjects.h"
#include "Packet/ClientZonePacket.h"
#include "Packet/ZoneClientPacket.h"
#include "WorldServerUtil.h"
#include "enter_world_result_zone.h"

namespace
{
const char *_nProtect_KickReason(unsigned __int8 byReason)
{
  switch (byReason)
  {
    case 1:
      return "KICK_REASON_INVALID_AUTH";
    case 2:
      return "KICK_REASON_NOT_NPROTECT_CLIENT";
    case 3:
      return "KICK_REASON_DELAY_AUTH";
    case 4:
      return "KICK_REASON_AUTH_FAIR_FAIL";
    default:
      return "What ??? ";
  }
}
} // namespace

bool NPROTECT_PARAM_VER_30::ms_bFirstChecker = false;

NPROTECT_PARAM_VER_30::NPROTECT_PARAM_VER_30()
  : m_CSAuth(), m_nSocketIndex(-1), m_dwLastSyncQryTime(0), m_byVerifyState(0), m_byVerChangeWarnningCount(0)
{
}

void NPROTECT_PARAM_VER_30::Init()
{
  m_byVerChangeWarnningCount = 0;
  m_nSocketIndex = -1;
  m_dwLastSyncQryTime = 0;
  m_byVerifyState = 0;
}

void NPROTECT_PARAM_VER_30::Close()
{
  m_byVerChangeWarnningCount = 0;
  m_nSocketIndex = -1;
  m_dwLastSyncQryTime = 0;
  m_byVerifyState = 0;
}

void NPROTECT_PARAM_VER_30::CheckClient(bool bFirstChecker)
{
  unsigned int returnedPacketSize = 0;
  const unsigned int dwRet = m_CSAuth.Get(&returnedPacketSize);
  if (dwRet >= 0xBB8)
  {
    CAsyncLogger::Instance()->FormatLog(
      CAsyncLogger::ALT_NPROTECT_LOG,
      "m_CSAuth.GetAuthQuery n(%d) Err(%u) Error!",
      m_nSocketIndex,
      dwRet);
    Kick(1, dwRet);
    return;
  }

  m_dwLastSyncQryTime = GetLoopTime();
  if (bFirstChecker)
  {
    m_byVerifyState = 1;
  }
  else
  {
    m_byVerifyState = 3;
  }

  if (returnedPacketSize)
  {
    _npr_msg_req_qry_zocl send{};
    std::memcpy(&send, m_CSAuth.packet, sizeof(m_CSAuth.packet));
    send.packetSize = returnedPacketSize;

    unsigned __int8 byType[2]{97, 2};
    g_Network.m_pProcess[0]->LoadSendMsg(
      m_nSocketIndex,
      byType,
      reinterpret_cast<char *>(&send),
      static_cast<unsigned __int16>(sizeof(send)));
  }
}

void NPROTECT_PARAM_VER_30::Kick(unsigned __int8 byReason, unsigned int dwRet)
{
  CAsyncLogger::Instance()->FormatLog(
    CAsyncLogger::ALT_NPROTECT_LOG,
    "NPROTECT_PARAM_VER_25::Kick n(%d) Reason ( %s ), ( %u ) Kick!",
    m_nSocketIndex,
    _nProtect_KickReason(byReason),
    dwRet);

  if (m_nSocketIndex < 0 || m_nSocketIndex >= MAX_PLAYER)
  {
    return;
  }

  CUserDB *pUserDB = &g_UserDB[m_nSocketIndex];
  if (byReason == 2)
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
    pUserDB->ForceCloseCommand(15, 0xFFFFFFFF, true, "nProtect Kick");
  }
  else if (m_byVerifyState && pUserDB->m_bActive)
  {
    pUserDB->ForceCloseCommand(15, 0xFFFFFFFF, false, "nProtect Kick");
  }
}

bool NPROTECT_PARAM_VER_30::OnRecvSession_Request_Start(int nIndex)
{
  m_nSocketIndex = nIndex;
  _npr_msg_notify_zocl send{};
  send.byRet = 0;
  unsigned __int8 byType[2]{97, 4};
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_nSocketIndex,
    byType,
    reinterpret_cast<char *>(&send),
    static_cast<unsigned __int16>(sizeof(send)));
  m_dwLastSyncQryTime = GetLoopTime();
  m_byVerifyState = 4;
  m_byVerChangeWarnningCount = 0;
  return true;
}

bool NPROTECT_PARAM_VER_30::OnRecvSession_Response_Qry(unsigned __int64 tSize, _npr_msg_res_qry_clzo *pMsg)
{
  if (tSize != sizeof(_npr_msg_res_qry_clzo))
  {
    return false;
  }
  if (m_nSocketIndex < 0 || m_nSocketIndex >= MAX_PLAYER)
  {
    return false;
  }

  if (m_byVerifyState == 1 || m_byVerifyState == 3)
  {
    std::memcpy(m_CSAuth.packet, pMsg, sizeof(m_CSAuth.packet));
    const unsigned int returnedPacketSize = pMsg->packetSize;
    const unsigned int dwRet = m_CSAuth.Check(returnedPacketSize);
    if (dwRet >= 0xBB8)
    {
      CAsyncLogger::Instance()->FormatLog(
        CAsyncLogger::ALT_NPROTECT_LOG,
        "m_CSAuth.Check() n(%d) Reason ( %u ) Kick!",
        m_nSocketIndex,
        dwRet);

      _npr_msg_notify_zocl send{};
      send.byRet = dwRet;
      unsigned __int8 byType[2]{97, 4};
      g_Network.m_pProcess[0]->LoadSendMsg(
        m_nSocketIndex,
        byType,
        reinterpret_cast<char *>(&send),
        static_cast<unsigned __int16>(sizeof(send)));
      Kick(1, dwRet);
      return true;
    }

    m_byVerChangeWarnningCount = 0;
    if (m_byVerifyState == 1)
    {
      _npr_msg_notify_zocl send{};
      send.byRet = dwRet;
      unsigned __int8 byType[2]{97, 4};
      g_Network.m_pProcess[0]->LoadSendMsg(
        m_nSocketIndex,
        byType,
        reinterpret_cast<char *>(&send),
        static_cast<unsigned __int16>(sizeof(send)));
    }

    m_byVerifyState = 4;
    return true;
  }

  Kick(4, 0xFFFFFFFF);
  return false;
}

bool NPROTECT_PARAM_VER_30::IsLogPass()
{
  return !ms_bFirstChecker || m_byVerifyState >= 2;
}

bool NPROTECT_PARAM_VER_30::OnCheckSession_FirstVerify(int n)
{
  if (IsLogPass())
  {
    return true;
  }

  m_nSocketIndex = n;
  Kick(2, 0);
  return false;
}

void NPROTECT_PARAM_VER_30::OnConnect(int nIndex)
{
  (void)nIndex;
  Init();
  m_CSAuth.Init(1);
}

void NPROTECT_PARAM_VER_30::OnLoop()
{
  // Intentional non-parity: mock nProtect never sends active challenge packets.
}

void NPROTECT_PARAM_VER_30::OnDisConnect()
{
  m_CSAuth.Close();
  Close();
}

bool NPROTECT_PARAM_VER_30::OnRecvSession(
  CnProtectSystem *mgr,
  int nIndex,
  unsigned __int8 byProtocol,
  unsigned __int64 tSize,
  char *pMsg)
{
  (void)mgr;
  (void)byProtocol;
  (void)tSize;
  (void)pMsg;

  // Intentional non-parity: mock nProtect accepts any client packet and returns success notify.
  m_nSocketIndex = nIndex;
  _npr_msg_notify_zocl send{};
  send.byRet = 0;
  unsigned __int8 byType[2]{97, 4};
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_nSocketIndex,
    byType,
    reinterpret_cast<char *>(&send),
    static_cast<unsigned __int16>(sizeof(send)));
  m_dwLastSyncQryTime = GetLoopTime();
  m_byVerifyState = 4;
  m_byVerChangeWarnningCount = 0;
  return true;
}
