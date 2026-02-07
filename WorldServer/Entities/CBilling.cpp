#include "pch.h"

#include "CBilling.h"

#include "CNetworkEX.h"
#include "CUserDB.h"
#include "GlobalObjects.h"

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

  char msg[60]{};
  memcpy_0(msg, g_Main.m_szWorldName, 0x21u);

  unsigned __int8 type[2] = {29, 1};
  g_Network.m_pProcess[3]->LoadSendMsg(0, type, msg, 0x21u);
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

      const short billingType = user->m_BillingInfo.iType;
      _SYSTEMTIME *endDate = &user->m_BillingInfo.stEndDate;
      const int remainTime = user->m_BillingInfo.lRemainTime;
      char *cms = user->m_BillingInfo.szCMS;
      SendMsg_Login(user->m_szAccountID, ip, cms, billingType, endDate, remainTime);
    }
  }
}
