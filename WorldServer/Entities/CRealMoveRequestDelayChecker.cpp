#include "pch.h"

#include "CRealMoveRequestDelayChecker.h"

bool CRealMoveRequestDelayChecker::Init(unsigned int listCount)
{
  if (listCount == 0)
  {
    return false;
  }

  m_vecDelayList.assign(listCount, 0);
  m_kNodeInxOrderList.SetList(listCount, 0, false);
  for (unsigned int index = 0; index < listCount; ++index)
  {
    m_kNodeInxOrderList.PushNode_Back(index, nullptr);
  }
  return true;
}

