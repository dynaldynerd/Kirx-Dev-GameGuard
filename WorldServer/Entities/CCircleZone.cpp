#include "pch.h"

#include "CCircleZone.h"

#include <new>

#include "CGameObject.h"
#include "CMapData.h"
#include "CNetProcess.h"
#include "DummyPosition.h"
#include "GlobalObjects.h"
#include "GuildBattle.h"

CCircleZone::CCircleZone()
  : m_eState(CZ_NONE),
    m_iPortalInx(-1),
    m_byColor(static_cast<unsigned __int8>(-1)),
    m_pkGoalPos(nullptr)
{
}

CCircleZone::~CCircleZone() = default;

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

void CCircleZone::SendMsg_FixPosition(int n)
{
  char msg[5]{};
  *reinterpret_cast<int *>(msg) = m_iPortalInx;
  msg[4] = static_cast<char>(m_byColor);

  unsigned __int8 packetType[2] = {4, static_cast<unsigned __int8>(-81)};
  g_Network.m_pProcess[0]->LoadSendMsg(
    n,
    packetType,
    msg,
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CCircleZone::SendMsgCreate()
{
  char payload[5]{};
  *reinterpret_cast<int *>(payload) = m_iPortalInx;
  payload[4] = static_cast<char>(m_byColor);

  unsigned __int8 type[2] = {3, 53};
  CircleReport(type, payload, 5, false);
}

void CCircleZone::SendMsgGoal()
{
  char payload[4]{};
  *reinterpret_cast<int *>(payload) = m_iPortalInx;

  unsigned __int8 type[2] = {4, static_cast<unsigned __int8>(-82)};
  CircleReport(type, payload, 4, false);
}
