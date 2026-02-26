#include "pch.h"

#include "CRecallEffectController.h"

#include "CPlayer.h"
#include "CRecallRequest.h"
#include "CHolyStoneSystem.h"
#include "GuildBattle.h"
#include "CPotionMgr.h"
#include "map_fld.h"
#include "GlobalObjects.h"

CRecallEffectController::CRecallEffectController()
{
  m_uiInfoTotCnt = 0;
  m_ppkReqeust = nullptr;
  m_pkTimer = nullptr;
  m_pkEmptyInxList = nullptr;
  m_pkUseInxList = nullptr;
}

CRecallEffectController::~CRecallEffectController()
{
  CleanUp();
}

void CRecallEffectController::CleanUp()
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

  if (m_ppkReqeust)
  {
    for (unsigned int index = 0; index < m_uiInfoTotCnt; ++index)
    {
      delete m_ppkReqeust[index];
      m_ppkReqeust[index] = nullptr;
    }

    operator delete[](m_ppkReqeust);
    m_ppkReqeust = nullptr;
  }

  m_uiInfoTotCnt = 0;
}

namespace
{
GUILD_BATTLE::CNormalGuildBattle *GetBattleByGuildSerial(unsigned int guildSerial)
{
  GUILD_BATTLE::CNormalGuildBattleManager *manager = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
  return manager ? manager->GetBattleByGuildSerial(guildSerial) : nullptr;
}

CMapData *GetGuildBattleMap(unsigned int guildSerial)
{
  GUILD_BATTLE::CNormalGuildBattle *battle = GetBattleByGuildSerial(guildSerial);
  if (!battle || !battle->m_pkField)
  {
    return nullptr;
  }
  return battle->m_pkField->m_pkMap;
}
} // namespace

CRecallEffectController *CRecallEffectController::Instance()
{
  static CRecallEffectController s_instance;
  return &s_instance;
}

bool CRecallEffectController::Init(unsigned int infoCount)
{
  CleanUp();

  if (infoCount == 0)
  {
    return false;
  }

  m_pkTimer = new CMyTimer();
  m_pkEmptyInxList = new CNetIndexList();
  m_pkUseInxList = new CNetIndexList();
  m_ppkReqeust = new CRecallRequest *[infoCount];

  if (!m_pkTimer || !m_pkEmptyInxList || !m_pkUseInxList || !m_ppkReqeust)
  {
    return false;
  }

  memset_0(m_ppkReqeust, 0, sizeof(CRecallRequest *) * infoCount);
  for (unsigned int index = 0; index < infoCount; ++index)
  {
    m_ppkReqeust[index] = new CRecallRequest(static_cast<unsigned __int16>(index));
    if (!m_ppkReqeust[index])
    {
      return false;
    }
  }

  m_pkEmptyInxList->SetList(infoCount);
  m_pkUseInxList->SetList(infoCount);

  for (unsigned int index = 0; index < infoCount; ++index)
  {
    m_pkEmptyInxList->PushNode_Back(index);
  }

  m_uiInfoTotCnt = infoCount;
  m_pkTimer->BeginTimer(0x7530u);
  return true;
}

void CRecallEffectController::OnLoop()
{
  if (m_pkTimer && m_pkTimer->CountingTimer())
  {
    UpdateClose();
  }
}

void CRecallEffectController::UpdateClose()
{
  if (!m_pkUseInxList || !m_ppkReqeust || m_pkUseInxList->size() <= 0)
  {
    return;
  }

  unsigned int outIndex[5]{};
  if (m_pkUseInxList->CopyFront(outIndex))
  {
    CRecallRequest *request = m_ppkReqeust[outIndex[0]];
    if (request && request->IsClose())
    {
      CPlayer *owner = request->GetOwner();
      SendRecallReqeustResult(5u, owner);
      Close(request, false);
    }
  }
}

CRecallRequest *CRecallEffectController::GetEmpty()
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

  CRecallRequest *request = m_ppkReqeust[outIndex];
  request->Clear();
  return request;
}

void CRecallEffectController::Close(CRecallRequest *pkRequest, bool bDone)
{
  if (!pkRequest || !m_pkUseInxList || !m_pkEmptyInxList)
  {
    return;
  }

  const unsigned __int16 requestID = pkRequest->GetID();
  if (m_pkUseInxList->FindNode(requestID))
  {
    if (!m_pkEmptyInxList->IsInList(requestID))
    {
      m_pkEmptyInxList->PushNode_Front(requestID);
      pkRequest->Close(bDone);
    }
  }
}

unsigned __int8 CRecallEffectController::GetResistedRecall(unsigned __int16 wID, CRecallRequest **pkRequest)
{
  *pkRequest = nullptr;

  for (unsigned int index = 0; index < m_uiInfoTotCnt; ++index)
  {
    CRecallRequest *request = m_ppkReqeust[index];
    if (request->GetID() == wID)
    {
      if (request->IsWait())
      {
        *pkRequest = request;
        return 0;
      }

      if (request->IsTimeOut())
      {
        return 5;
      }

      return 3;
    }
  }

  return 3;
}

unsigned __int8 CRecallEffectController::ProcessRequestRecall(
  CPlayer *pkPerformer,
  CPlayer *pkDest,
  CRecallRequest **pkRequest,
  bool bRecallParty,
  bool bStone,
  bool bBattleModeUse)
{
  *pkRequest = GetEmpty();
  if (!*pkRequest)
  {
    return 1;
  }

  const unsigned __int8 registRet = (*pkRequest)->Regist(pkPerformer, pkDest, bRecallParty, bStone, bBattleModeUse);
  if (registRet)
  {
    return registRet;
  }

  const unsigned __int16 requestID = (*pkRequest)->GetID();
  if (m_pkUseInxList->PushNode_Back(requestID))
  {
    return 0;
  }

  Close(*pkRequest, false);
  return 2;
}

bool CRecallEffectController::RequestRecall(
  CPlayer *pkPerformer,
  CPlayer *pkDest,
  bool bRecallParty,
  bool bStone,
  bool bBattleModeUse)
{
  CRecallRequest *request = nullptr;
  const unsigned __int8 processRet =
    ProcessRequestRecall(pkPerformer, pkDest, &request, bRecallParty, bStone, bBattleModeUse);
  if (processRet)
  {
    SendRecallReqeustResult(static_cast<char>(processRet), pkPerformer);
    return false;
  }

  SendRecallReqeustToDest(request->GetID(), pkPerformer, pkDest);
  return true;
}

void CRecallEffectController::SendRecallReqeustResult(char byRet, CPlayer *pkObj)
{
  if (!pkObj)
  {
    return;
  }


  _recall_request_result_zocl msg{};
  msg.byRet = static_cast<unsigned __int8>(byRet);
  strcpy_0(msg.wszDestName, pkObj->m_Param.GetCharNameW());

  unsigned __int8 type[2] = {17, 32};
  g_Network.m_pProcess[0]->LoadSendMsg(
    pkObj->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CRecallEffectController::SendRecallReqeustToDest(
  unsigned __int16 wRequestID,
  CPlayer *pkPerformer,
  CPlayer *pkDest)
{
  if (!pkPerformer || !pkDest)
  {
    return;
  }


  _notify_recall_request_zocl msg{};
  msg.wRequestID = wRequestID;
  strcpy_0(msg.wszPerformerName, pkPerformer->m_Param.GetCharNameW());

  unsigned __int8 pbyType[2]{17, 33};
  g_Network.m_pProcess[0]->LoadSendMsg(
    pkDest->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CRecallEffectController::SendDecideRecallErrorResultToDest(char byErr, CPlayer *pkDest, int nCallerMapCode)
{
  if (!pkDest)
  {
    return;
  }


  _decide_recall_error_result_zocl msg{};
  msg.byErr = byErr;
  msg.nMapCode = nCallerMapCode;

  unsigned __int8 pbyType[2]{17, 35};
  g_Network.m_pProcess[0]->LoadSendMsg(
    pkDest->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CRecallEffectController::DecideRecall(unsigned __int16 dwRequestID, unsigned __int8 byAgree, CPlayer *pkObj)
{
  if (!pkObj || !pkObj->m_bOper)
  {
    return;
  }

  if (byAgree && byAgree != 1)
  {
    SendDecideRecallErrorResultToDest(6u, pkObj, -1);
    return;
  }

  CRecallRequest *request = nullptr;
  unsigned __int8 resistedRecall = GetResistedRecall(dwRequestID, &request);
  if (resistedRecall)
  {
    SendDecideRecallErrorResultToDest(static_cast<char>(resistedRecall), pkObj, -1);
    return;
  }

  CPlayer *owner = request ? request->m_pkOwner : nullptr;
  if (!owner)
  {
    SendDecideRecallErrorResultToDest(3, pkObj, -1);
    Close(request, true);
    return;
  }

  if (byAgree == 1)
  {
    SendRecallReqeustResult(4u, owner);
    Close(request, true);
    return;
  }

  if (pkObj->IsSiegeMode())
  {
    SendDecideRecallErrorResultToDest(0x1Au, pkObj, -1);
    return;
  }

  if (g_HolySys.GetDestroyerState() == 2 && g_HolySys.GetDestroyerSerial() == pkObj->m_dwObjSerial)
  {
    SendDecideRecallErrorResultToDest(0x14u, pkObj, -1);
    return;
  }

  if (owner->GetCurSecNum() == -1 || owner->m_bMapLoading)
  {
    SendDecideRecallErrorResultToDest(0x15u, pkObj, -1);
    return;
  }

  if (request->m_bRecallParty)
  {
    if (pkObj->m_pCurMap != owner->m_pCurMap)
    {
      SendDecideRecallErrorResultToDest(0xCu, pkObj, owner->m_pCurMap->m_nMapCode);
      SendRecallReqeustResult(0xDu, owner);
      Close(request, true);
      return;
    }
  }
  else
  {
    if (!request->m_bBattleModeUse && pkObj->Is_Battle_Mode())
    {
      SendDecideRecallErrorResultToDest(0x12u, pkObj, -1);
      return;
    }

    const int minLevel = owner->m_pCurMap->GetLevelLimit();
    const int maxLevel = owner->m_pCurMap->m_pMapSet->m_nUpLevelLim;
    const int playerLevel = static_cast<int>(pkObj->GetLevel());
    if (playerLevel < minLevel || (maxLevel != -1 && playerLevel > maxLevel))
    {
      SendDecideRecallErrorResultToDest(0x13u, pkObj, -1);
      return;
    }

    if (pkObj->m_pDHChannel || owner->m_pDHChannel)
    {
      if (pkObj->m_pDHChannel && owner->m_pDHChannel)
      {
        if (pkObj->m_pDHChannel != owner->m_pDHChannel)
        {
          SendDecideRecallErrorResultToDest(0x16u, pkObj, -1);
          return;
        }
      }
      else if (!pkObj->m_pDHChannel && owner->m_pDHChannel)
      {
        SendDecideRecallErrorResultToDest(0x16u, pkObj, -1);
        return;
      }
      else if (pkObj->m_pDHChannel && !owner->m_pDHChannel)
      {
        SendDecideRecallErrorResultToDest(0x17u, pkObj, -1);
        return;
      }
    }

    if (pkObj->m_bInGuildBattle || owner->m_bInGuildBattle)
    {
      if (pkObj->m_bInGuildBattle && !owner->m_bInGuildBattle)
      {
        SendDecideRecallErrorResultToDest(0x19u, pkObj, -1);
        return;
      }

      if (!pkObj->m_bInGuildBattle && owner->m_bInGuildBattle)
      {
        SendDecideRecallErrorResultToDest(9u, pkObj, -1);
        return;
      }

      if (pkObj->m_bInGuildBattle && owner->m_bInGuildBattle)
      {
        const CMapData *destMap = GetGuildBattleMap(pkObj->m_Param.GetGuildSerial());
        const CMapData *ownerMap = GetGuildBattleMap(owner->m_Param.GetGuildSerial());
        if (destMap != ownerMap)
        {
          SendDecideRecallErrorResultToDest(0x19u, pkObj, -1);
          return;
        }

        if (pkObj->m_bTakeGravityStone)
        {
          SendDecideRecallErrorResultToDest(0xAu, pkObj, -1);
          return;
        }
      }
    }
  }

  resistedRecall = request->Recall(pkObj, request->IsRecallAfterStoneState());
  if (resistedRecall)
  {
    SendDecideRecallErrorResultToDest(static_cast<char>(resistedRecall), pkObj, -1);
  }
  Close(request, true);
}
