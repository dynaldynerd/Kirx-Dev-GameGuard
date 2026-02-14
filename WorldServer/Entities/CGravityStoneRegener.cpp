#include "pch.h"

#include "CGravityStoneRegener.h"

#include <new>

#include "CGameObject.h"
#include "CMapData.h"
#include "CNetProcess.h"
#include "DummyPosition.h"
#include "GlobalObjects.h"
#include "GuildBattle.h"

CGravityStoneRegener::CGravityStoneRegener() : m_eState(GSR_NONE), m_iPortalInx(-1), m_pkRegenPos(nullptr)
{
}

CGravityStoneRegener::~CGravityStoneRegener() = default;

bool CGravityStoneRegener::Init(unsigned int uiMapInx, unsigned __int16 wInx, CMapData *pkMap)
{
  _object_id id(0, 8u, wInx);
  CGameObject::Init(&id);

  char returned[288]{};
  char keyName[288]{};
  char buffer[272]{};
  sprintf(buffer, "Map%u", uiMapInx);

  m_pkRegenPos = new (std::nothrow) _dummy_position();
  if (!m_pkRegenPos)
  {
    GUILD_BATTLE::CGuildBattleLogger::Instance()->Log(
      "CGravityStoneRegener::Init( %u, %d, pkMap ) : new _dummy_position NULL!",
      uiMapInx,
      wInx);
    return false;
  }

  sprintf(keyName, "BallRegenDummyName%d", m_ObjID.m_wIndex);
  GetPrivateProfileStringA(
    buffer,
    keyName,
    "X",
    returned,
    0xFFu,
    "./Initialize/NormalGuildBattle.ini");
  if (returned[0] == 'X')
  {
    GUILD_BATTLE::CGuildBattleLogger::Instance()->Log(
      "CGravityStoneRegener::Init( %u, %d, pkMap ) : GetPrivateProfileString( %s, %s, X, %s, 255, %s ) == 'X'!",
      uiMapInx,
      wInx,
      buffer,
      keyName,
      returned,
      "./Initialize/NormalGuildBattle.ini");
    return false;
  }

  if (!pkMap->LoadDummy(returned, m_pkRegenPos))
  {
    GUILD_BATTLE::CGuildBattleLogger::Instance()->Log(
      "CGravityStoneRegener::Init( %u, %d, pkMap ) : pkMap->LoadDummy( %s, m_pkRegenPos ) Fail!",
      uiMapInx,
      wInx,
      returned);
    return false;
  }

  m_iPortalInx = pkMap->GetPortalInx(m_pkRegenPos->m_szCode);
  if (m_iPortalInx < 0)
  {
    GUILD_BATTLE::CGuildBattleLogger::Instance()->Log(
      "CGravityStoneRegener::Init( %u, %d, pkMap )pkMap->GetPortalInx( %s ) Fail!",
      uiMapInx,
      wInx,
      m_pkRegenPos->m_szCode);
    return false;
  }

  m_eState = GSR_INIT;
  return true;
}

int CGravityStoneRegener::GetPortalInx() const
{
  return m_iPortalInx;
}

void CGravityStoneRegener::SendMsg_FixPosition(int n)
{
  char msg[5]{};
  *reinterpret_cast<int *>(msg) = m_iPortalInx;
  msg[4] = static_cast<char>(m_eState == GSR_REGEN);

  unsigned __int8 packetType[2] = {4, static_cast<unsigned __int8>(-84)};
  g_Network.m_pProcess[0]->LoadSendMsg(
    n,
    packetType,
    msg,
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CGravityStoneRegener::SendMsgAlterState()
{
  char payload[5]{};
  *reinterpret_cast<int *>(payload) = m_iPortalInx;
  payload[4] = (m_eState == GSR_REGEN) ? 1 : 0;

  unsigned __int8 type[2] = {4, static_cast<unsigned __int8>(-83)};
  CircleReport(type, payload, 5, false);
}
