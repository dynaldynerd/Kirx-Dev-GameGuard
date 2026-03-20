#include "pch.h"

#include "CGravityStone.h"

#include "CGameObject.h"
#include "CMapData.h"
#include "CPlayer.h"
#include "CUserDB.h"
#include "ObjectCreateSetData.h"
#include "CNetProcess.h"
#include "GlobalObjects.h"
#include "Packet/ZoneClientPacket.h"
#include "WorldServerUtil.h"

unsigned int CGravityStone::ms_dwSerialCnt = 1;

CGravityStone::CGravityStone(unsigned __int16 wInx) : m_dwTakeLimitTime(0), m_pkOwner(nullptr)
{
  _object_id id(1u, 4u, wInx);
  CGameObject::Init(&id);
  m_dwObjSerial = 0;
  m_dwTakeLimitTime = 0;
}

CGravityStone::~CGravityStone() = default;

unsigned __int8 CGravityStone::Get(
  unsigned __int16 wIndex,
  unsigned int dwObjSerial,
  CPlayer *pkPlayer)
{
  if (!m_bLive)
  {
    return static_cast<unsigned __int8>(-124);
  }

  if (m_ObjID.m_wIndex != wIndex || m_dwObjSerial != dwObjSerial)
  {
    return static_cast<unsigned __int8>(-124);
  }

  if (pkPlayer->IsRidingUnit())
  {
    return static_cast<unsigned __int8>(-117);
  }

  if (!IsNearPosition(pkPlayer->m_fCurPos))
  {
    return static_cast<unsigned __int8>(-126);
  }

  SetOwner(pkPlayer);
  Destroy();
  return 0;
}

unsigned __int8 CGravityStone::CheatGet(CPlayer *pkPlayer)
{
  return Get(m_ObjID.m_wIndex, m_dwObjSerial, pkPlayer);
}

CPlayer *CGravityStone::GetOwner()
{
  return m_pkOwner;
}

float *CGravityStone::GetOwnerCurPos()
{
  if (m_pkOwner)
  {
    return m_pkOwner->m_fCurPos;
  }
  return nullptr;
}

bool CGravityStone::IsNearPosition(const float *pfCurPos)
{
  return GetSqrt(m_fCurPos, const_cast<float *>(pfCurPos)) <= 30.0f;
}

bool CGravityStone::IsValidOwner(CPlayer *pkPlayer)
{
  return m_pkOwner && m_pkOwner == pkPlayer && m_pkOwner->m_pUserDB->m_dwSerial == pkPlayer->m_pUserDB->m_dwSerial;
}

bool CGravityStone::IsInTown()
{
  return m_pkOwner && m_pkOwner->IsInTown();
}

bool CGravityStone::CheckTakeTimeLimit()
{
  if (!m_pkOwner || !m_dwTakeLimitTime)
  {
    return false;
  }

  if (m_dwTakeLimitTime > GetLoopTime())
  {
    return false;
  }

  Clear();
  return true;
}

bool CGravityStone::Regen(_object_create_setdata *pParam)
{
  if (!Create(pParam))
  {
    return false;
  }
  m_dwObjSerial = ms_dwSerialCnt++;
  SendMsg_Create();
  return true;
}

unsigned __int8 CGravityStone::Drop(CPlayer *pkPlayer)
{
  if (!IsValidOwner(pkPlayer))
  {
    return static_cast<unsigned __int8>(-125);
  }

  _object_create_setdata param{};
  param.m_pRecordSet = nullptr;
  param.m_nLayerIndex = m_pkOwner->m_wMapLayerIndex;
  param.m_pMap = m_pkOwner->m_pCurMap;
  if (!param.m_pMap->GetRandPosVirtualDumExcludeStdRange(
        m_pkOwner->m_fCurPos,
        300,
        100,
        param.m_fStartPos))
  {
    param.m_fStartPos[0] = m_pkOwner->m_fCurPos[0];
    param.m_fStartPos[1] = m_pkOwner->m_fCurPos[1];
    param.m_fStartPos[2] = m_pkOwner->m_fCurPos[2];
  }

  if (!Regen(&param))
  {
    return static_cast<unsigned __int8>(-123);
  }
  m_pkOwner->ClearGravityStone();
  m_pkOwner = nullptr;
  return 0;
}

void CGravityStone::SetOwner(CPlayer *pkPlayer)
{
  if (pkPlayer)
  {
    m_dwTakeLimitTime = GetLoopTime() + 120000;
    m_pkOwner = pkPlayer;
    m_pkOwner->TakeGravityStone();
  }
}

void CGravityStone::SendMsg_Create()
{
  _create_gravity_stone_zocl msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.wRecIndex = 89;
  msg.dwObjSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, msg.zPos, 3);

  unsigned __int8 type[2] = {3, 51};
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(sizeof(msg)), false);
}

void CGravityStone::SendMsg_Destroy()
{
  _destroy_gravity_stone_zocl msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  unsigned __int8 type[2] = {3, 52};
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(sizeof(msg)), false);
}

void CGravityStone::SendMsg_FixPosition(int n)
{
  _gravity_stone_fix_position_zocl msg{};
  msg.wObjIndex = m_ObjID.m_wIndex;
  msg.wRecIndex = 89;
  msg.dwObjSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, msg.zPos, 3);

  unsigned __int8 packetType[2] = {4, static_cast<unsigned __int8>(-85)};
  g_Network.m_pProcess[0]->LoadSendMsg(
    n,
    packetType,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CGravityStone::Destroy()
{
  if (m_bLive)
  {
    SendMsg_Destroy();
    CGameObject::Destroy();
    m_dwObjSerial = 0;
  }
}

void CGravityStone::Clear()
{
  if (m_pkOwner)
  {
    m_pkOwner->ClearGravityStone();
  }
  m_dwTakeLimitTime = 0;
  m_pkOwner = nullptr;
  Destroy();
}
