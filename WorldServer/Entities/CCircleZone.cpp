#include "pch.h"

#include "CCircleZone.h"

#include <new>

#include "CGameObject.h"
#include "CMapData.h"
#include "CNetProcess.h"
#include "DummyPosition.h"
#include "GlobalObjects.h"
#include "GuildBattle.h"
#include "ObjectCreateSetData.h"
#include "Packet/ZoneClientPacket.h"
#include "WorldServerUtil.h"

unsigned int CCircleZone::ms_dwSerialCnt = 1;

CCircleZone::CCircleZone()
  : m_eState(CZ_NONE),
    m_iPortalInx(-1),
    m_byColor(static_cast<unsigned __int8>(-1)),
    m_pkGoalPos(nullptr)
{
}

CCircleZone::~CCircleZone() = default;

char CCircleZone::Create(CMapData *pkMap, unsigned __int8 byColor)
{
  if (m_eState == CZ_NONE || !pkMap)
  {
    return 0;
  }

  _object_create_setdata data;
  data.m_nLayerIndex = 0;
  data.m_pMap = pkMap;
  memcpy_0(data.m_fStartPos, m_pkGoalPos->m_fCenterPos, sizeof(data.m_fStartPos));
  data.m_pRecordSet = nullptr;
  if (!CGameObject::Create(&data))
  {
    return 0;
  }

  m_dwObjSerial = ms_dwSerialCnt++;
  m_byColor = byColor;
  return 1;
}

void CCircleZone::Destroy()
{
  if (m_eState != CZ_NONE)
  {
    m_eState = CZ_DESTROY;
    m_byColor = static_cast<unsigned __int8>(-1);
    CCharacter::Destroy();
  }
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

unsigned __int8 CCircleZone::GetColor() const
{
  return m_byColor;
}

unsigned __int8 CCircleZone::Goal(CMapData *pkMap, const float *pfCurPos)
{
  if (!pkMap || m_eState == CZ_CREATE)
  {
    return 110;
  }
  if (!IsNearPosition(const_cast<float *>(pfCurPos)))
  {
    return static_cast<unsigned __int8>(-122);
  }
  SendMsgGoal();
  return 0;
}

bool CCircleZone::IsNearPosition(float *pfCurPos)
{
  return GetSqrt(m_fCurPos, pfCurPos) <= 200.0f;
}

void CCircleZone::SendMsg_FixPosition(int n)
{
  _circle_zone_fix_position_zocl msg{};
  msg.iPortalInx = m_iPortalInx;
  msg.byColor = static_cast<char>(m_byColor);

  unsigned __int8 packetType[2] = {4, static_cast<unsigned __int8>(-81)};
  g_Network.m_pProcess[0]->LoadSendMsg(
    n,
    packetType,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CCircleZone::SendMsgCreate()
{
  _create_circle_zone_stone_zocl msg{};
  msg.iPortalInx = m_iPortalInx;
  msg.byColor = static_cast<char>(m_byColor);

  unsigned __int8 type[2] = {3, 53};
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(sizeof(msg)), false);
}

void CCircleZone::SendMsgGoal()
{
  _circle_zone_goal_inform_zocl msg{};
  msg.iPortalInx = m_iPortalInx;

  unsigned __int8 type[2] = {4, static_cast<unsigned __int8>(-82)};
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(sizeof(msg)), false);
}
