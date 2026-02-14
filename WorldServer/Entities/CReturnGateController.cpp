#include "pch.h"

#include "CReturnGateController.h"
#include "CReturnGate.h"
#include "CPlayer.h"
#include "GlobalObjects.h"

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
  static CReturnGateController s_instance;
  return &s_instance;
}

bool CReturnGateController::Init(unsigned int gateCount)
{
  m_uiGateTotCnt = gateCount;
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

CReturnGate *CReturnGateController::GetGate(unsigned int uiInx)
{
  if (m_uiGateTotCnt >= uiInx && m_ppkGatePool)
  {
    return m_ppkGatePool[uiInx];
  }
  return nullptr;
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
