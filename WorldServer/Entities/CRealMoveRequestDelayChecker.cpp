#include "pch.h"

#include "CRealMoveRequestDelayChecker.h"

#include <cstdio>
#include <ctime>

#include "CNetworkEX.h"
#include "CPlayer.h"
#include "CUserDB.h"
#include "GlobalObjects.h"

CRealMoveRequestDelayChecker::CRealMoveRequestDelayChecker()
{
  m_wTotalMissCount = 0;
  m_bPrevRet = true;
  m_wContinueMissCount = 0;
  m_wSingleMissCount = 0;
  m_wContinueValiedCount = 0;
  m_wTotalContinueValiedCount = 0;
}

CRealMoveRequestDelayChecker::~CRealMoveRequestDelayChecker()
{
}

bool CRealMoveRequestDelayChecker::Init(unsigned int listCount)
{
  if (listCount == 0)
  {
    g_Main.m_logLoadingError.Write("CRealMoveRequestDelayChecker::init() iListCnt(0) Fail!");
    return false;
  }

  m_vecDelayList.assign(listCount, 0);
  m_kNodeInxOrderList.SetList(listCount, 0, false);
  for (unsigned int index = 0; index < listCount; ++index)
  {
    m_kNodeInxOrderList.PushNode_Back(index, nullptr);
  }
  m_wTotalMissCount = 0;
  m_bPrevRet = false;
  m_wContinueMissCount = 0;
  m_wSingleMissCount = 0;
  m_wContinueValiedCount = 0;
  m_wTotalContinueValiedCount = 0;
  return true;
}

bool CRealMoveRequestDelayChecker::Check(CPlayer *pkUser)
{
  if (m_vecDelayList.empty())
  {
    return false;
  }

  unsigned int index = static_cast<unsigned int>(-1);
  if (!GetCurInx(&index))
  {
    return false;
  }

  const unsigned int curTime = static_cast<unsigned int>(clock());
  bool valid = true;

  if (m_vecDelayList[index] != 0)
  {
    if (CheckDelay(curTime, index))
    {
      if (m_bPrevRet)
      {
        ++m_wContinueValiedCount;
      }
      if (m_wContinueValiedCount >= 3 && m_wContinueMissCount)
      {
        --m_wContinueMissCount;
      }
      m_bPrevRet = true;
      if (m_wContinueValiedCount >= 3)
      {
        m_wContinueValiedCount = 0;
        ++m_wTotalContinueValiedCount;
        if (m_wTotalContinueValiedCount >= 2)
        {
          m_wTotalContinueValiedCount = 0;
          if (m_wTotalMissCount)
          {
            --m_wTotalMissCount;
          }
        }
      }
    }
    else
    {
      m_wContinueValiedCount = 0;
      if (m_bPrevRet)
      {
        ++m_wSingleMissCount;
        if (m_wSingleMissCount >= 10)
        {
          m_wSingleMissCount = 0;
          ++m_wTotalMissCount;
        }
      }
      else
      {
        ++m_wContinueMissCount;
      }

      m_bPrevRet = false;
      if (m_wContinueMissCount >= 3)
      {
        valid = false;
        m_wContinueMissCount = 0;
        ++m_wTotalMissCount;
      }

      if (m_wTotalMissCount >= 5)
      {
        valid = false;
        m_wTotalMissCount = 0;
        char buffer[144]{};
        sprintf_s(
          buffer,
          "CLOSE>> Invalid Real Move Request, id:%s, char:%s",
          pkUser->m_pUserDB->m_szAccountID,
          pkUser->m_Param.GetCharNameA());
        g_Network.Close(0, pkUser->m_ObjID.m_wIndex, false, buffer);
      }
    }

    IncNodeIndex();
    if (!GetCurInx(&index))
    {
      return false;
    }
    m_vecDelayList[index] = curTime;
    return valid;
  }

  m_vecDelayList[index] = curTime;
  return true;
}

void CRealMoveRequestDelayChecker::Reset()
{
  if (m_vecDelayList.empty())
  {
    return;
  }

  unsigned int index = static_cast<unsigned int>(-1);
  if (GetCurInx(&index))
  {
    m_vecDelayList[index] = 0;
  }
}

bool CRealMoveRequestDelayChecker::GetCurInx(unsigned int *pInx)
{
  if (!pInx)
  {
    return false;
  }

  *pInx = static_cast<unsigned int>(-1);

  bool copied = false;
  m_kNodeInxOrderList.m_csList.Lock();
  CIndexList::_index_node *front = m_kNodeInxOrderList.m_Head.m_pNext;
  if (front != &m_kNodeInxOrderList.m_Tail)
  {
    *pInx = front->m_dwIndex;
    copied = true;
  }
  m_kNodeInxOrderList.m_csList.Unlock();

  if (!copied)
  {
    g_Main.m_logSystemError.Write(
      "CRealMoveRequestDelayChecker::Reset() : m_kNodeInxOrderList.CopyFront() Fail!");
    return false;
  }

  if (*pInx == static_cast<unsigned int>(-1) || m_vecDelayList.size() <= *pInx)
  {
    g_Main.m_logSystemError.Write("CRealMoveRequestDelayChecker::Reset() : *pInx(%u) Invalid!", *pInx);
    return false;
  }
  return true;
}

bool CRealMoveRequestDelayChecker::CheckDelay(unsigned int dwCurTime, unsigned int dwIndex)
{
  if (m_vecDelayList[dwIndex] == 0)
  {
    return true;
  }
  const unsigned int delta = dwCurTime - m_vecDelayList[dwIndex];
  return delta >= 500;
}

void CRealMoveRequestDelayChecker::IncNodeIndex()
{
  if (!m_kNodeInxOrderList.m_pBufNode || m_kNodeInxOrderList.m_dwMaxBufNum == 0)
  {
    return;
  }

  unsigned int outIndex = 0;
  if (m_kNodeInxOrderList.PopNode_Front(&outIndex, nullptr))
  {
    if (!m_kNodeInxOrderList.PushNode_Back(outIndex, nullptr))
    {
      g_Main.m_logSystemError.Write(
        "CRealMoveRequestDelayChecker::IncNodeIndex() : m_kNodeInxOrderList.PushNode_Back() Fail!");
    }
  }
  else
  {
    g_Main.m_logSystemError.Write(
      "CRealMoveRequestDelayChecker::IncNodeIndex() : m_kNodeInxOrderList.PopNode_Front() Fail!");
  }
}


