#include "pch.h"

#include "CReturnGateController.h"
#include "CReturnGate.h"
#include "CReturnGateCreateParam.h"
#include "CPlayer.h"
#include "GlobalObjects.h"

#include <cstring>

CReturnGateController *CReturnGateController::ms_Instance = nullptr;

CReturnGateController::CReturnGateController()
{
  m_pkTimer = nullptr;
  m_pkEmptyInxList = nullptr;
  m_pkUseInxList = nullptr;
  m_uiGateTotCnt = 0;
  m_ppkGatePool = nullptr;
}

CReturnGateController::~CReturnGateController()
{
  CleanUp();
}

void CReturnGateController::CleanUp()
{
  if (m_pkTimer)
  {
    delete m_pkTimer;
    m_pkTimer = nullptr;
  }

  if (m_pkEmptyInxList)
  {
    delete m_pkEmptyInxList;
    m_pkEmptyInxList = nullptr;
  }

  if (m_pkUseInxList)
  {
    delete m_pkUseInxList;
    m_pkUseInxList = nullptr;
  }

  if (m_ppkGatePool)
  {
    for (unsigned int index = 0; index < m_uiGateTotCnt; ++index)
    {
      if (m_ppkGatePool[index])
      {
        delete m_ppkGatePool[index];
        m_ppkGatePool[index] = nullptr;
      }
    }
    delete[] m_ppkGatePool;
    m_ppkGatePool = nullptr;
  }

  m_uiGateTotCnt = 0;
}

CReturnGateController *CReturnGateController::Instance()
{
  if (!ms_Instance)
  {
    ms_Instance = new CReturnGateController();
  }
  return ms_Instance;
}

void CReturnGateController::Destroy()
{
  if (ms_Instance)
  {
    delete ms_Instance;
    ms_Instance = nullptr;
  }
}

bool CReturnGateController::Init(unsigned int gateCount)
{
  if (!gateCount)
  {
    g_Main.m_logReturnGate.Write("CReturnGateController::Init 0 == uiSize!");
    return false;
  }

  m_uiGateTotCnt = gateCount;
  m_pkTimer = new CMyTimer();
  m_pkEmptyInxList = new CNetIndexList();
  m_pkUseInxList = new CNetIndexList();
  m_ppkGatePool = new CReturnGate *[m_uiGateTotCnt];
  if (!m_pkTimer || !m_pkEmptyInxList || !m_pkUseInxList || !m_ppkGatePool)
  {
    return false;
  }

  std::memset(m_ppkGatePool, 0, sizeof(CReturnGate *) * m_uiGateTotCnt);
  _object_id objectId(1u, 3u, 0);
  for (unsigned int index = 0; index < m_uiGateTotCnt; ++index)
  {
    objectId.m_wIndex = static_cast<unsigned __int16>(index);
    m_ppkGatePool[index] = new CReturnGate(&objectId);
    if (!m_ppkGatePool[index])
    {
      g_Main.m_logReturnGate.Write("CReturnGateController::Init NULL == new CReturnGate!");
      return false;
    }
  }

  m_pkEmptyInxList->SetList(m_uiGateTotCnt);
  m_pkUseInxList->SetList(m_uiGateTotCnt);
  for (unsigned int index = 0; index < m_uiGateTotCnt; ++index)
  {
    m_pkEmptyInxList->PushNode_Back(index);
  }

  m_pkTimer->BeginTimer(0x2710u);
  return true;
}

void CReturnGateController::OnLoop()
{
  if (m_pkTimer && m_pkTimer->CountingTimer())
  {
    UpdateClose();
  }
}

void CReturnGateController::UpdateClose()
{
  if (m_pkUseInxList->size() <= 0)
  {
    return;
  }

  unsigned int outIndex[5]{};
  if (m_pkUseInxList->CopyFront(outIndex))
  {
    CReturnGate *gate = m_ppkGatePool[outIndex[0]];
    if (gate && gate->IsClose())
    {
      if (m_pkUseInxList->PopNode_Front(outIndex))
      {
        Close(gate);
      }
    }
  }
}

CReturnGate *CReturnGateController::GetEmpty()
{
  if (!m_pkEmptyInxList)
  {
    return nullptr;
  }

  unsigned int outIndex = 0;
  if (!m_pkEmptyInxList->PopNode_Front(&outIndex))
  {
    return nullptr;
  }

  m_ppkGatePool[outIndex]->Clear();
  return m_ppkGatePool[outIndex];
}

bool CReturnGateController::IsExistOwner(CPlayer *pkObj)
{
  if (!m_pkUseInxList || m_pkUseInxList->size() <= 0)
  {
    return false;
  }

  m_pkUseInxList->m_csList.Lock();
  for (CNetIndexList::_index_node *node = m_pkUseInxList->m_Head.m_pNext; node != &m_pkUseInxList->m_Tail;
       node = node->m_pNext)
  {
    if (m_ppkGatePool[node->m_dwIndex]->GetOwner() == pkObj)
    {
      m_pkUseInxList->m_csList.Unlock();
      return true;
    }
  }

  m_pkUseInxList->m_csList.Unlock();
  return false;
}

CReturnGate *CReturnGateController::GetGate(unsigned int uiInx)
{
  if (m_uiGateTotCnt >= uiInx && m_ppkGatePool)
  {
    return m_ppkGatePool[uiInx];
  }
  return nullptr;
}

bool CReturnGateController::Open(CPlayer *pkOwner)
{
  if (!pkOwner)
  {
    g_Main.m_logReturnGate.Write("CReturnGateController::Open NULL == pkOwner");
    return false;
  }
  if (pkOwner->m_bInGuildBattle)
  {
    return false;
  }
  if (IsExistOwner(pkOwner))
  {
    g_Main.m_logReturnGate.Write(
      "CReturnGateController::Open ExistOwner! CharName:%s",
      pkOwner->m_Param.GetCharNameA());
    return false;
  }

  CReturnGate *emptyGate = GetEmpty();
  if (!emptyGate)
  {
    const int emptyCount = static_cast<int>(m_pkEmptyInxList->size());
    const int useCount = static_cast<int>(m_pkUseInxList->size());
    g_Main.m_logReturnGate.Write(
      "CReturnGateController::Open GetEmpty Failed! Use : %d Empty : %d",
      useCount,
      emptyCount);
    return false;
  }

  CReturnGateCreateParam param(pkOwner);
  if (!emptyGate->Open(&param))
  {
    return false;
  }
  if (m_pkUseInxList->PushNode_Back(emptyGate->GetIndex()))
  {
    return true;
  }

  g_Main.m_logReturnGate.Write("CReturnGateController::Open m_pkUseInxList->PushNode_Back Failed!");
  return false;
}

void CReturnGateController::Close(CReturnGate *pkGate)
{
  const unsigned __int16 index = pkGate->GetIndex();
  if (!m_pkEmptyInxList->IsInList(index))
  {
    m_pkEmptyInxList->PushNode_Front(pkGate->GetIndex());
    pkGate->Close();
  }
}

int CReturnGateController::ProcessEnter(unsigned int uiGateInx, CPlayer *pkObj)
{
  if (pkObj->m_bInGuildBattle)
  {
    return 6;
  }

  CReturnGate *gate = GetGate(uiGateInx);
  if (!gate || !gate->IsOpen())
  {
    return 1;
  }

  if (gate->IsValidOwner())
  {
    return gate->Enter(pkObj);
  }

  Close(gate);
  return 2;
}

void CReturnGateController::SendEnterResult(char iResult, CPlayer *pkObj)
{
  char msg[1]{};
  msg[0] = iResult;
  unsigned __int8 pbyType[2] = {8, 10};
  g_Network.m_pProcess[0]->LoadSendMsg(pkObj->m_ObjID.m_wIndex, pbyType, msg, 1u);
}

char CReturnGateController::Enter(unsigned int uiGateInx, CPlayer *pkObj)
{
  const int result = ProcessEnter(uiGateInx, pkObj);
  if (!result)
  {
    return 1;
  }

  SendEnterResult(static_cast<char>(result), pkObj);
  return 0;
}
