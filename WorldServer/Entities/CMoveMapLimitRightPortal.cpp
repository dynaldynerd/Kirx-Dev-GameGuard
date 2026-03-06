#include "pch.h"

#include "CMoveMapLimitRightPortal.h"

#include "CNetworkEX.h"
#include "CHolyStoneSystem.h"
#include "GlobalObjects.h"
#include "notice_move_limit_map_msg_zocl.h"
#include "Packet/ZoneClientPacket.h"

CMoveMapLimitRightPortal::CMoveMapLimitRightPortal(int iType)
  : CMoveMapLimitRight(iType),
    m_pkRight(nullptr),
    m_bNotifyForceMoveStartPosition(false)
{
}

CMoveMapLimitRightPortal::~CMoveMapLimitRightPortal() = default;

void CMoveMapLimitRightPortal::Load(CPlayer *pkPlayer)
{
  m_pkRight = &pkPlayer->m_MinigTicket;
}

void CMoveMapLimitRightPortal::CreateComplete(CPlayer *pkPlayer)
{
  if (m_bNotifyForceMoveStartPosition)
  {
    unsigned __int8 type[2]{};
    type[0] = 59;
    type[1] = 1;

    _notice_move_limit_map_msg_zocl msg{};
    msg.byType = 2;
    g_Network.m_pProcess[0]->LoadSendMsg(
      pkPlayer->m_ObjID.m_wIndex,
      type,
      reinterpret_cast<char *>(&msg),
      static_cast<unsigned __int16>(sizeof(msg)));
    m_bNotifyForceMoveStartPosition = false;
  }
}

void CMoveMapLimitRightPortal::LogOut(CPlayer *pkPlayer)
{
m_pkRight = nullptr;
}

bool CMoveMapLimitRightPortal::IsHaveRight()
{
  if (!m_pkRight)
  {
    return false;
  }

  if (!g_HolySys.IsMinigeTicketCheck())
  {
    return true;
  }

  const unsigned __int8 byNumOfTime = g_HolySys.GetNumOfTime();
  const unsigned __int8 byCurrentHour = g_HolySys.GetStartHour();
  const unsigned __int8 startDay = g_HolySys.GetStartDay();
  const unsigned __int8 startMonth = g_HolySys.GetStartMonth();
  const unsigned __int16 startYear = g_HolySys.GetStartYear();

  return m_pkRight->AuthLastCriTicket(startYear, startMonth, startDay, byCurrentHour, byNumOfTime);
}

void CMoveMapLimitRightPortal::SetFlag(int iType, bool bFlag)
{
  if (iType == 0)
  {
    m_bNotifyForceMoveStartPosition = bFlag;
  }
}
