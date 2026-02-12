#include "pch.h"

#include "CDarkHole.h"

#include <cstring>
#include <mmsystem.h>

#include "CMainThread.h"
#include "CDarkHoleChannel.h"
#include "CDarkHoleDungeonQuest.h"
#include "CPlayerDB.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "darkhole_create_setdata.h"

int CDarkHole::s_nLiveNum = 0;
unsigned int CDarkHole::s_dwSerialCounter = 0;

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
  W2M(m_wszOpenerName, m_aszOpenerName, 0x11u);

  const DWORD now = timeGetTime();
  m_dwNextCloseTime = now + 15000;
  m_dwNextHurryTime = now + 40000;
  m_bHurry = false;
  ++s_nLiveNum;
  return 1;
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

