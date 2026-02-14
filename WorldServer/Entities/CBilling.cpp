#include "pch.h"

#include "CBilling.h"

#include <cstring>

#include "CNetProcess.h"
#include "CNetworkEX.h"
#include "CPlayer.h"
#include "CUserDB.h"
#include "GlobalObjects.h"

CBilling::CBilling()
{
}

CBilling::~CBilling()
{
  // this is not a stub
}

void CBilling::Start()
{
  SendMsg_StartBilling();
  SendMsg_CurAllUserLogin();
}

void CBilling::SendMsg_StartBilling()
{
  if (!m_bOper)
  {
    return;
  }

  char szMsg[60]{};
  memcpy_0(szMsg, g_Main.m_szWorldName, 0x21u);

  unsigned __int8 pbyType[2]{29, 1};
  g_Network.m_pProcess[3]->LoadSendMsg(0, pbyType, szMsg, 0x21u);
}

void CBilling::SendMsg_CurAllUserLogin()
{
  if (!m_bOper)
  {
    return;
  }

  for (unsigned int index = 0; index < MAX_PLAYER; ++index)
  {
    CUserDB *user = &g_UserDB[index];
    if (user->m_bActive && !user->m_byUserDgr)
    {
      in_addr addr{};
      addr.S_un.S_addr = user->m_dwIP;
      char *ip = inet_ntoa(addr);

      const __int16 billingType = user->m_BillingInfo.iType;
      _SYSTEMTIME *endDate = &user->m_BillingInfo.stEndDate;
      const int remainTime = user->m_BillingInfo.lRemainTime;
      char *cms = user->m_BillingInfo.szCMS;
      SendMsg_Login(user->m_szAccountID, ip, cms, billingType, endDate, remainTime);
    }
  }
}

void CBilling::Login(CUserDB *pUserDB)
{
  in_addr addr{};
  addr.S_un.S_addr = pUserDB->m_dwIP;
  char *ip = inet_ntoa(addr);

  const __int16 billingType = pUserDB->m_BillingInfo.iType;
  _SYSTEMTIME *endDate = &pUserDB->m_BillingInfo.stEndDate;
  const int remainTime = pUserDB->m_BillingInfo.lRemainTime;
  char *cms = pUserDB->m_BillingInfo.szCMS;
  if (SendMsg_Login(pUserDB->m_szAccountID, ip, cms, billingType, endDate, remainTime))
  {
    pUserDB->SetBillingNoLogout(false);
  }
}

void CBilling::Alive(CUserDB *pUserDB)
{
  if (m_bOper
      && pUserDB->m_BillingInfo.iType <= 100
      && (pUserDB->m_BillingInfo.iType == 6 || pUserDB->m_BillingInfo.iType == 7))
  {
    char msg[0x27]{};
    memcpy_0(msg, pUserDB->m_szAccountID, 13);
    in_addr addr{};
    addr.S_un.S_addr = pUserDB->m_dwIP;
    char *ip = inet_ntoa(addr);
    memcpy_0(msg + 13, ip, 16);
    memcpy_0(msg + 29, pUserDB->m_BillingInfo.szCMS, 7);
    *reinterpret_cast<__int16 *>(msg + 36) = pUserDB->m_BillingInfo.iType;

    unsigned __int8 pbyType[2]{29, 5};
    g_Network.m_pProcess[3]->LoadSendMsg(0, pbyType, msg, 0x27u);
  }
}

void CBilling::Logout(CUserDB *pUserDB)
{
  if (m_bOper
      && pUserDB->m_BillingInfo.iType <= 100
      && (pUserDB->m_BillingInfo.iType == 6 || pUserDB->m_BillingInfo.iType == 7))
  {
    char msg[0x26]{};
    memcpy_0(msg, pUserDB->m_szAccountID, 13);
    in_addr addr{};
    addr.S_un.S_addr = pUserDB->m_dwIP;
    char *ip = inet_ntoa(addr);
    memcpy_0(msg + 13, ip, 16);
    memcpy_0(msg + 29, pUserDB->m_BillingInfo.szCMS, 7);
    *reinterpret_cast<__int16 *>(msg + 36) = pUserDB->m_BillingInfo.iType;

    unsigned __int8 pbyType[2]{29, 6};
    g_Network.m_pProcess[3]->LoadSendMsg(0, pbyType, msg, 0x26u);
  }
}

void CBilling::BillingClose(char *szID)
{
  if (m_bOper)
  {
    CPlayer *player = GetPtrPlayerFromAccount(g_Player, MAX_PLAYER, szID);
    if (player)
    {
      const int billingType = player->m_pUserDB->GetBillingType();
      if (billingType == 6 || billingType == 7)
      {
        player->m_pUserDB->ForceCloseCommand(1u, 0xFFFFFFFFu, false, "Billing close");
        player->m_pUserDB->m_BillingInfo.iType = 1;
      }
    }
  }
}

bool CBilling::SendMsg_Login(
  char *szID,
  char *szIP,
  char *szCMS,
  short iType,
  _SYSTEMTIME *pstEndDate,
  int lRemainTime)
{
  if (!m_bOper)
  {
    return false;
  }
  if (iType > 100)
  {
    return false;
  }
  if (iType != 6 && iType != 7)
  {
    return false;
  }

  char msg[0x3A]{};
  memcpy_0(msg, szID, 13);
  memcpy_0(msg + 13, szIP, 16);
  if (szCMS)
  {
    memcpy_0(msg + 29, szCMS, 7);
  }
  *reinterpret_cast<__int16 *>(msg + 36) = iType;
  *reinterpret_cast<int *>(msg + 38) = lRemainTime;
  if (pstEndDate)
  {
    memcpy_0(msg + 42, pstEndDate, 0x10u);
  }

  unsigned __int8 pbyType[2]{29, 4};
  g_Network.m_pProcess[3]->LoadSendMsg(0, pbyType, msg, 0x3Au);
  return true;
}

void CBilling::SetOper(bool bOper)
{
  m_bOper = bOper;
}

void CBilling::Remaintime_Personal(
  char *szID,
  __int16 iType,
  int lRemaintime,
  _SYSTEMTIME *pstEndDate)
{
  if (m_bOper)
  {
    CPlayer *player = GetPtrPlayerFromAccount(g_Player, MAX_PLAYER, szID);
    if (player)
    {
      if (player->m_pUserDB->m_BillingInfo.iType >= 100
          && (player->m_pUserDB->m_BillingInfo.iType == 6 || player->m_pUserDB->m_BillingInfo.iType == 7))
      {
        player->m_pUserDB->SetBillingData(player->m_pUserDB->m_BillingInfo.szCMS, iType, lRemaintime, pstEndDate);
        player->m_pUserDB->SendMsg_BillingInfo();
      }
    }
  }
}

void CBilling::Remaintime_PCBang(
  char *szCMSCode,
  __int16 iType,
  int lRemaintime,
  _SYSTEMTIME *pstEndDate)
{
  if (m_bOper && szCMSCode && strlen_0(szCMSCode))
  {
    for (int j = 0; j < MAX_PLAYER; ++j)
    {
      CPlayer *player = &g_Player[j];
      if (player && player->m_bLive && !strcmp_0(player->m_pUserDB->m_BillingInfo.szCMS, szCMSCode))
      {
        player->m_pUserDB->SetBillingData(player->m_pUserDB->m_BillingInfo.szCMS, iType, lRemaintime, pstEndDate);
        player->m_pUserDB->SendMsg_BillingInfo();
      }
    }
  }
}

void CBilling::Change_BillingType(
  char *szID,
  char *szCMSCode,
  __int16 iType,
  int lRemainTime,
  _SYSTEMTIME *pstEndDate,
  unsigned __int8 byReason)
{
  if (m_bOper)
  {
    CPlayer *player = GetPtrPlayerFromAccount(g_Player, MAX_PLAYER, szID);
    if (player)
    {
      if (player->m_pUserDB->m_BillingInfo.iType == 6 || player->m_pUserDB->m_BillingInfo.iType == 7)
      {
        if (iType == 6 || iType == 7)
        {
          player->m_pUserDB->SetBillingData(szCMSCode, iType, lRemainTime, pstEndDate);
        }
        else
        {
          player->m_pUserDB->SetBillingData(nullptr, iType, lRemainTime, pstEndDate);
        }
        player->m_pUserDB->SendMsg_BillingInfo();
      }
    }
  }
}

void CBilling::Expire_Personal(char *szID)
{
// this is not a stub
}

void CBilling::Expire_PCBang(char *szCMS)
{
  if (m_bOper && szCMS && strlen_0(szCMS))
  {
    for (int j = 0; j < MAX_PLAYER; ++j)
    {
      CPlayer *player = &g_Player[j];
      if (player && player->m_bLive && !strcmp_0(player->m_pUserDB->m_BillingInfo.szCMS, szCMS))
      {
        player->m_pUserDB->ForceCloseCommand(1u, 0xFFFFFFFFu, false, "Billing expire");
        player->m_pUserDB->m_BillingInfo.iType = 1;
      }
    }
  }
}

void CBilling::Expire_IPOverflow(char *szID)
{
// this is not a stub
}

void CBilling::Change_Primium(char *szID, bool bResult)
{
// this is not a stub
}

void CBilling::SendMsg_ZoneAliveCheck(unsigned int dwData)
{
  char szMsg[4]{};
  *reinterpret_cast<unsigned int *>(szMsg) = dwData;
  unsigned __int8 pbyType[2]{29, 91};
  g_Network.m_pProcess[3]->LoadSendMsg(0, pbyType, szMsg, 4u);
}
