#include "pch.h"

#include "CDarkHoleChannel.h"

#include "CPlayer.h"
#include "darkhole_leader_change_inform_zocl.h"
#include "darkhole_outof_member_inform_zocl.h"
#include "GlobalObjects.h"

#include <mmsystem.h>

void _dh_player_mgr::_pos::init()
{
  pMap = nullptr;
}

_dh_player_mgr::_dh_player_mgr()
{
  Init();
}

void _dh_player_mgr::Init()
{
  pOne = nullptr;
  dwSerial = static_cast<unsigned int>(-1);
  LastPos.init();
}

bool _dh_player_mgr::IsFill()
{
  return pOne != nullptr;
}

CDarkHoleChannel::__enter_member::__enter_member()
{
  bActive = false;
  bDisnormalClose = false;
  dwDisconnectTime = 0;
}

CDarkHoleChannel::__enter_member::__enter_member(bool active, bool disnormal, unsigned int time)
{
  (void)time;
  bActive = active;
  bDisnormalClose = disnormal;
  dwDisconnectTime = timeGetTime();
}

void CDarkHoleChannel::SendMsg_PopMember(CPlayer *pPopMember, bool bDisconnect)
{
  _darkhole_outof_member_inform_zocl msg{};
  msg.dwMemberSerial = pPopMember->m_dwObjSerial;
  msg.bDisconnect = bDisconnect;

  unsigned __int8 type[2] = {35, 2};
  for (int j = 0; j < 32; ++j)
  {
    _dh_player_mgr *entry = &m_Quester[j];
    if (entry->IsFill())
    {
      const unsigned __int16 len = msg.size();
      g_Network.m_pProcess[0]->LoadSendMsg(
        entry->pOne->m_ObjID.m_wIndex,
        type,
        reinterpret_cast<char *>(&msg),
        len);
    }
  }
}

void CDarkHoleChannel::SendMsg_LeaderChange(CPlayer *pNewLeader)
{
  (void)pNewLeader;

  _darkhole_leader_change_inform_zocl msg{};
  msg.dwNewLeaderSerial = m_dwOpenerSerial;

  unsigned __int8 type[2] = {35, 3};
  for (int j = 0; j < 32; ++j)
  {
    _dh_player_mgr *entry = &m_Quester[j];
    if (entry->IsFill())
    {
      const unsigned __int16 len = msg.size();
      g_Network.m_pProcess[0]->LoadSendMsg(
        entry->pOne->m_ObjID.m_wIndex,
        type,
        reinterpret_cast<char *>(&msg),
        len);
    }
  }
}

char CDarkHoleChannel::ClearMember(CPlayer *pMember, bool bDisconnect, _dh_player_mgr::_pos *poutPlayerPos)
{
  pMember->m_pDHChannel = nullptr;

  _dh_player_mgr *target = nullptr;
  for (int j = 0; j < 32; ++j)
  {
    if (m_Quester[j].IsFill() && m_Quester[j].pOne == pMember)
    {
      target = &m_Quester[j];
      break;
    }
  }

  if (!target)
  {
    return 0;
  }

  if (poutPlayerPos)
  {
    memcpy_0(poutPlayerPos, &target->LastPos, sizeof(_dh_player_mgr::_pos));
  }

  target->Init();
  SendMsg_PopMember(pMember, bDisconnect);

  _dh_player_mgr *oldLeader = m_pLeaderPtr;
  if (m_pLeaderPtr == target)
  {
    m_pLeaderPtr = nullptr;
    unsigned int minEnterOrder = static_cast<unsigned int>(-1);
    for (int k = 0; k < 32; ++k)
    {
      if (m_Quester[k].IsFill() && minEnterOrder > static_cast<unsigned int>(m_Quester[k].nEnterOrder))
      {
        minEnterOrder = static_cast<unsigned int>(m_Quester[k].nEnterOrder);
        m_pLeaderPtr = &m_Quester[k];
      }
    }
    if (m_pLeaderPtr)
    {
      SendMsg_LeaderChange(m_pLeaderPtr->pOne);
    }
  }

  const DWORD now = timeGetTime();
  CDarkHoleChannel::__enter_member entry(0, bDisconnect, now);
  m_listEnterMember.FindNode(pMember->m_dwObjSerial, nullptr);
  m_listEnterMember.PushNode_Back(pMember->m_dwObjSerial, reinterpret_cast<char *>(&entry));
  return 1;
}

