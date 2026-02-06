#include "pch.h"

#include "CGravityStone.h"

#include "CGameObject.h"

CGravityStone::CGravityStone(unsigned __int16 wInx) : m_dwTakeLimitTime(0), m_pkOwner(nullptr)
{
  _object_id id(1u, 4u, wInx);
  CGameObject::Init(&id);
  m_dwObjSerial = 0;
  m_dwTakeLimitTime = 0;
}
