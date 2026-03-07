#include "pch.h"

#include "CDarkHole.h"

#include <cstring>
#include <mmsystem.h>

#include "CMainThread.h"
#include "CDarkHoleChannel.h"
#include "CDarkHoleDungeonQuest.h"
#include "CNetProcess.h"
#include "CPartyPlayer.h"
#include "CPlayerDB.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "darkhole_create_setdata.h"
#include "darkhole_create_zocl.h"
#include "darkhole_destroy_zocl.h"
#include "darkhole_fixpositon_zocl.h"
#include "darkhole_state_change_zocl.h"

int CDarkHole::s_nLiveNum;
unsigned int CDarkHole::s_dwSerialCounter;

CDarkHole::CDarkHole()
{
  m_pChannel = nullptr;
  m_dwChannelSerial = static_cast<unsigned int>(-1);
  m_dwNextCloseTime = 0;
  m_dwNextHurryTime = 0;
  m_bHurry = false;
}

CDarkHole::~CDarkHole()
{
}

void CDarkHole::Init(_object_id *pID)
{
  CGameObject::Init(pID);
  m_dwOpenerSerial = 0;
  std::memset(m_wszOpenerName, 0, sizeof(m_wszOpenerName));
  std::memset(m_aszOpenerName, 0, sizeof(m_aszOpenerName));
}

char CDarkHole::Create(_darkhole_create_setdata *pParam)
{
  if (!CGameObject::Create(pParam))
  {
    return 0;
  }

  m_dwObjSerial = s_dwSerialCounter++;
  m_pChannel = g_DarkHoleQuest.OpenChannel(m_pRecordSet->m_dwIndex, pParam->pOpener, this);
  if (m_pChannel)
  {
    m_dwChannelSerial = m_pChannel->m_dwChannelSerial;
  }

  m_dwOpenerSerial = pParam->pOpener->m_dwObjSerial;
  const char *openerName = pParam->pOpener->m_Param.GetCharNameW();
  strcpy_0(m_wszOpenerName, openerName);
  W2M(m_wszOpenerName, m_aszOpenerName, 17u);

  const DWORD now = timeGetTime();
  m_dwNextCloseTime = now + 15000;
  m_dwNextHurryTime = now + 40000;
  m_bHurry = false;
  ++s_nLiveNum;
  return 1;
}

bool CDarkHole::Destroy()
{
  if (m_pChannel == nullptr)
  {
    return false;
  }

  SendMsg_Destroy();
  m_pChannel->m_bCheckMemberClose = true;
  m_pChannel = nullptr;
  m_dwChannelSerial = static_cast<unsigned int>(-1);
  --s_nLiveNum;
  return CGameObject::Destroy();
}

bool CDarkHole::IsNewEnterAbleNum()
{
  if (!m_pChannel)
  {
    return false;
  }
  if (!m_pChannel->IsFill())
  {
    return false;
  }

  const unsigned int allMemberNum = m_pChannel->GetAllMemberNum();
  if (allMemberNum >= m_pChannel->m_pQuestSetup->dwMaxMember)
  {
    return false;
  }

  const unsigned int currentMemberNum = m_pChannel->GetCurrentMemberNum();
  return currentMemberNum < m_pChannel->m_pQuestSetup->dwMaxMember;
}

bool CDarkHole::IsNewEnterAblePlayer(CPlayer *pEnter)
{
  return m_pChannel && m_pChannel->CanYouEnterHole(pEnter);
}

bool CDarkHole::IsOpenPartyMember(CPlayer *pOpener)
{
  return m_pChannel && m_pChannel->IsOpenPartyMember(pOpener);
}

bool CDarkHole::GetEnterNewPos(_ENTER_DUNGEON_NEW_POS *pNewPos)
{
  return m_pChannel && m_pChannel->GetEnterNewPos(pNewPos);
}

bool CDarkHole::EnterPlayer(
  CPlayer *pEnter,
  CMapData *pOldMap,
  unsigned __int16 wOldLayer,
  float *fOldPos,
  bool bReconnect)
{
  return m_pChannel && m_pChannel->PushMember(pEnter, bReconnect, pOldMap, wOldLayer, fOldPos);
}

void CDarkHole::Loop()
{
  const DWORD now = timeGetTime();
  if (m_pChannel != nullptr
      && m_pChannel->m_dwChannelSerial == m_dwChannelSerial
      && m_dwNextCloseTime >= now)
  {
    if (!m_bHurry && m_dwNextHurryTime < now)
    {
      m_bHurry = true;
      SendMsg_StateChange();
    }
    return;
  }

  Destroy();
}

void CDarkHole::SendMsg_Create()
{
  _darkhole_create_zocl packet{};
  packet.wQuestIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  packet.wIndex = m_ObjID.m_wIndex;
  packet.dwSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, packet.zPos, 3);
  packet.bHurry = static_cast<unsigned __int8>(m_bHurry);
  strcpy_0(packet.wszOpenerName, m_wszOpenerName);
  packet.dwOpenerSerial = m_dwOpenerSerial;

  unsigned __int8 packetType[2] = {35, 101};
  CircleReport(packetType, reinterpret_cast<char *>(&packet), packet.size(), false);
}

void CDarkHole::SendMsg_Destroy()
{
  if (m_pChannel == nullptr)
  {
    return;
  }

  _darkhole_destroy_zocl packet{};
  packet.wIndex = m_ObjID.m_wIndex;
  packet.dwSerial = m_dwObjSerial;

  unsigned __int8 packetType[2] = {35, 102};
  if (m_pChannel->m_pQuestSetup != nullptr)
  {
    if (m_pChannel->m_pQuestSetup->bPartyOnly)
    {
      if (m_pChannel->m_pPartyMng->IsPartyMode())
      {
        CPartyPlayer **partyMembers = m_pChannel->m_pPartyMng->GetPtrPartyMember();
        if (partyMembers != nullptr)
        {
          for (int memberIndex = 0; memberIndex < 8; ++memberIndex)
          {
            CPartyPlayer *member = partyMembers[memberIndex];
            if (member != nullptr && g_Player[member->m_id.wIndex].m_bLive)
            {
              g_Network.m_pProcess[0]->LoadSendMsg(
                member->m_id.wIndex,
                packetType,
                reinterpret_cast<char *>(&packet),
                packet.size());
            }
          }
        }
      }
    }
    else
    {
      m_pChannel->SendMsg_GateDestroy(packetType, reinterpret_cast<char *>(&packet), packet.size());
      CircleReport(packetType, reinterpret_cast<char *>(&packet), packet.size(), false);
    }
  }
}

void CDarkHole::SendMsg_StateChange()
{
  _darkhole_state_change_zocl packet{};
  packet.wIndex = m_ObjID.m_wIndex;
  packet.dwSerial = m_dwObjSerial;
  packet.bHurry = static_cast<unsigned __int8>(m_bHurry);

  unsigned __int8 packetType[2] = {35, 103};
  CircleReport(packetType, reinterpret_cast<char *>(&packet), packet.size(), false);
}

void CDarkHole::SendMsg_FixPosition(int n)
{
  _darkhole_fixpositon_zocl packet{};
  packet.wQuestIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  packet.wIndex = m_ObjID.m_wIndex;
  packet.dwSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, packet.zPos, 3);
  packet.bHurry = static_cast<unsigned __int8>(m_bHurry);
  strcpy_0(packet.wszOpenerName, m_wszOpenerName);
  packet.dwOpenerSerial = m_dwOpenerSerial;

  unsigned __int8 packetType[2] = {35, 104};
  g_Network.m_pProcess[0]->LoadSendMsg(
    n,
    packetType,
    reinterpret_cast<char *>(&packet),
    packet.size());
}

CDarkHole *SerarchEmptyDarkHole()
{
  for (int index = 0; index < MAX_DARKHOLE; ++index)
  {
    if (!g_DarkHole[index].m_bLive)
    {
      return &g_DarkHole[index];
    }
  }
  return nullptr;
}

char IsExistDarkHoleOpenGate()
{
  for (int index = 0; index < MAX_DARKHOLE; ++index)
  {
    if (g_DarkHole[index].m_pChannel)
    {
      return 1;
    }
  }
  return 0;
}

