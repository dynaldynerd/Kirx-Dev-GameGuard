#include "pch.h"

#include "CRealMoveRequestDelayChecker.h"

bool CRealMoveRequestDelayChecker::Init(unsigned int listCount)
{
  if (listCount == 0)
  {
    return false;
  }

  m_vecDelayList.assign(listCount, 0);
  CIndexList::SetList(&m_kNodeInxOrderList, listCount, 0, false);
  for (unsigned int index = 0; index < listCount; ++index)
  {
    CIndexList::PushNode_Back(&m_kNodeInxOrderList, index, nullptr);
  }
  return true;
}

