#include "pch.h"

#include "CGravityStoneRegener.h"

#include <new>
#include <cstdlib>

#include "CGameObject.h"
#include "CMapData.h"
#include "CNetProcess.h"
#include "DummyPosition.h"
#include "GlobalObjects.h"
#include "GuildBattle.h"
#include "ObjectCreateSetData.h"
#include "Packet/ZoneClientPacket.h"
#include "WorldServerUtil.h"

unsigned int CGravityStoneRegener::ms_dwSerialCnt = 1;

CGravityStoneRegener::CGravityStoneRegener() : m_eState(GSR_NONE), m_iPortalInx(-1), m_pkRegenPos(nullptr)
{
}

CGravityStoneRegener::~CGravityStoneRegener() = default;

char CGravityStoneRegener::Create(CMapData *pkMap)
{
  if (m_eState == GSR_NONE || !pkMap)
  {
    return 0;
  }

  _object_create_setdata data;
  data.m_nLayerIndex = 0;
  data.m_pMap = pkMap;
  memcpy_0(data.m_fStartPos, m_pkRegenPos->m_fCenterPos, sizeof(data.m_fStartPos));
  data.m_pRecordSet = nullptr;
  if (!CGameObject::Create(&data))
  {
    return 0;
  }

  m_dwObjSerial = ms_dwSerialCnt++;
  m_eState = GSR_CREATE;
  return 1;
}

void CGravityStoneRegener::Destroy()
{
  m_eState = GSR_DESTROY;
  CCharacter::Destroy();
}

const char *CGravityStoneRegener::GetStateString(CString *strState)
{
  switch (m_eState)
  {
    case GSR_CREATE:
      *strState = "Create";
      break;
    case GSR_REGEN:
      *strState = "Regen";
      break;
    case GSR_TAKE:
      *strState = "Take";
      break;
    default:
      *strState = "None";
      break;
  }
#ifdef UNICODE
  static char stateText[32]{};
  size_t converted = 0;
  wcstombs_s(&converted, stateText, strState->GetString(), _TRUNCATE);
  return stateText;
#else
  return strState->GetString();
#endif
}

bool CGravityStoneRegener::IsNearPosition(const float *pfCurPos)
{
  return GetSqrt(m_fCurPos, const_cast<float *>(pfCurPos)) <= 100.0f;
}

int CGravityStoneRegener::Regen()
{
  if (m_eState != GSR_CREATE && m_eState != GSR_TAKE)
  {
    return -1;
  }

  m_eState = GSR_REGEN;
  SendMsgAlterState();
  return m_iPortalInx;
}

unsigned __int8 CGravityStoneRegener::Take(CMapData *pkMap, const float *pfCurPos)
{
  if (!pkMap)
  {
    return 110;
  }

  if (m_eState != GSR_REGEN)
  {
    return static_cast<unsigned __int8>(-121);
  }

  if (!IsNearPosition(pfCurPos))
  {
    return static_cast<unsigned __int8>(-120);
  }

  m_eState = GSR_TAKE;
  SendMsgAlterState();
  return 0;
}

void CGravityStoneRegener::CheatClearRegenState()
{
  if (m_eState == GSR_CREATE || m_eState == GSR_REGEN || m_eState == GSR_TAKE)
  {
    m_eState = GSR_CREATE;
    SendMsgAlterState();
  }
}

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

char CGravityStoneRegener::ClearRegen()
{
  if (m_eState == GSR_CREATE || m_eState == GSR_REGEN || m_eState == GSR_TAKE)
  {
    m_eState = GSR_CREATE;
    return 1;
  }

  const char *stateName[] = {"NONE", "INIT", "CREATE", "REGEN", "TAKE", "DESTROY"};
  if (m_eState > GSR_DESTROY)
  {
    m_eState = GSR_NONE;
  }

  GUILD_BATTLE::CGuildBattleLogger::Instance()->Log(
    "CGravityStoneRegener::ClearRegen() : (%s) : Invalid Goal State",
    stateName[static_cast<int>(m_eState) + 1]);
  return 0;
}

int CGravityStoneRegener::GetPortalInx() const
{
  return m_iPortalInx;
}

void CGravityStoneRegener::SendMsg_FixPosition(int n)
{
  _gravity_stone_regener_fix_position_zocl msg{};
  msg.iPortalInx = m_iPortalInx;
  msg.byState = static_cast<char>(m_eState == GSR_REGEN);

  unsigned __int8 packetType[2] = {4, static_cast<unsigned __int8>(-84)};
  g_Network.m_pProcess[0]->LoadSendMsg(
    n,
    packetType,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CGravityStoneRegener::SendMsgAlterState()
{
  _gravity_stone_regener_inform_zocl payload{};
  payload.iPortalInx = m_iPortalInx;
  payload.byState = static_cast<char>((m_eState == GSR_REGEN) ? 1 : 0);

  unsigned __int8 type[2] = {4, static_cast<unsigned __int8>(-83)};
  CircleReport(type, reinterpret_cast<char *>(&payload), static_cast<unsigned __int16>(sizeof(payload)), false);
}
