#include "pch.h"

#include "CReturnGateController.h"
#include "CReturnGate.h"
#include "CPlayer.h"
#include "GlobalObjects.h"

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
  if (!CNetIndexList::IsInList(m_pkEmptyInxList, index))
  {
    CNetIndexList::PushNode_Front(m_pkEmptyInxList, pkGate->GetIndex());
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
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], pkObj->m_ObjID.m_wIndex, pbyType, msg, 1u);
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
