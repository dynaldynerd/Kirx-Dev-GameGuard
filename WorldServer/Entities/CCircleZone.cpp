#include "pch.h"

#include "CCircleZone.h"

#include <new>

#include "CGameObject.h"
#include "CMapData.h"
#include "DummyPosition.h"
#include "GuildBattle.h"

CCircleZone::CCircleZone()
  : m_eState(CZ_NONE),
    m_iPortalInx(-1),
    m_byColor(static_cast<unsigned __int8>(-1)),
    m_pkGoalPos(nullptr)
{
}

bool CCircleZone::Init(
  unsigned int uiMapInx,
  int iPlayerInx,
  int iNth,
  unsigned __int16 wInx,
  CMapData *pkMap)
{
  _object_id id(0, 9u, wInx);
  CGameObject::Init(&id);

  char returned[288]{};
  char keyName[288]{};
  char buffer[272]{};
  sprintf(buffer, "Map%d", uiMapInx);

  m_pkGoalPos = new (std::nothrow) _dummy_position();
  if (!m_pkGoalPos)
  {
    GUILD_BATTLE::CGuildBattleLogger::Instance()->Log("CCircleZone::Init() : new _dummy_position NULL!");
    return false;
  }

  sprintf(keyName, "%dPGoalPosDummyName%d", iPlayerInx, iNth);
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
      "CCircleZone::Init() : GetPrivateProfileString( %s, %s, X, %s, 255, %s ) == 'X'!",
      buffer,
      keyName,
      returned,
      "./Initialize/NormalGuildBattle.ini");
    return false;
  }

  if (!pkMap->LoadDummy(returned, m_pkGoalPos))
  {
    GUILD_BATTLE::CGuildBattleLogger::Instance()->Log(
      "CCircleZone::Init() : pkMap->LoadDummy( %s, m_pkGoalPos ) Fail!",
      returned);
    return false;
  }

  m_iPortalInx = pkMap->GetPortalInx(m_pkGoalPos->m_szCode);
  if (m_iPortalInx < 0)
  {
    GUILD_BATTLE::CGuildBattleLogger::Instance()->Log(
      "CGravityStoneRegener::Init()pkMap->GetPortalInx( %s ) Fail!",
      m_pkGoalPos->m_szCode);
    return false;
  }

  m_eState = CZ_INIT;
  return true;
}

int CCircleZone::GetPortalInx() const
{
  return m_iPortalInx;
}

