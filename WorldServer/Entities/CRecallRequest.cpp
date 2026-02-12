#include "pch.h"

#include "CRecallRequest.h"

#include "WorldServerUtil.h"

bool CRecallRequest::IsClose()
{
  return m_dwCloseTime != 0 && GetLoopTime() > m_dwCloseTime;
}

CPlayer *CRecallRequest::GetOwner()
{
  return m_pkOwner;
}
