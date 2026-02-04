#include "pch.h"

#include "CNuclearBomb.h"

bool CNuclearBomb::Init(_object_id *pID)
{
  CCharacter::Init(pID);
  m_pMaster = nullptr;
  m_bUse = false;
  m_dwDelayTime = 0;
  m_dwDurTime = 0;
  m_dwStartTime = 0;
  m_nStartDmLoop = 0;
  return true;
}

bool CNuclearBomb::GetUse()
{
  return m_bUse;
}

unsigned __int8 CNuclearBomb::GetBombStatus()
{
  return m_byBombState;
}

float *CNuclearBomb::GetMissilePos()
{
  return m_fDropPos;
}
