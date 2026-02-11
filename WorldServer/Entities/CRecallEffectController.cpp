#include "pch.h"

#include "CRecallEffectController.h"

#include "CRecallRequest.h"
#include "CPlayer.h"
#include "CPlayerDB.h"
#include "CMapData.h"
#include "CNetProcess.h"
#include "CHolyStoneSystem.h"
#include "GuildBattle.h"
#include "GlobalObjects.h"
#include "recall_request_result_zocl.h"
#include "notify_recall_request_zocl.h"
#include "decide_recall_error_result_zocl.h"

#include <new>

CRecallEffectController *CRecallEffectController::ms_Instance = nullptr;

CRecallEffectController::CRecallEffectController()
  : m_uiInfoTotCnt(0)
  , m_ppkReqeust(nullptr)
  , m_pkTimer(nullptr)
  , m_pkEmptyInxList(nullptr)
  , m_pkUseInxList(nullptr)
{
}

CRecallEffectController::~CRecallEffectController()
{
  CleanUp();
}

CRecallEffectController *CRecallEffectController::Instance()
{
  if (!ms_Instance)
  {
    ms_Instance = new CRecallEffectController();
  }
  return ms_Instance;
}

bool CRecallEffectController::Init(unsigned int infoCount)
{
  if (!infoCount)
  {
    return false;
  }

  m_pkTimer = new (std::nothrow) CMyTimer();
  m_pkEmptyInxList = new (std::nothrow) CNetIndexList();
  m_pkUseInxList = new (std::nothrow) CNetIndexList();
  m_ppkReqeust = new (std::nothrow) CRecallRequest *[infoCount];
  if (!m_pkTimer || !m_pkEmptyInxList || !m_pkUseInxList || !m_ppkReqeust)
  {
    return false;
  }

  memset(m_ppkReqeust, 0, sizeof(*m_ppkReqeust) * infoCount);
  for (unsigned int j = 0; j < infoCount; ++j)
  {
    m_ppkReqeust[j] = new (std::nothrow) CRecallRequest(static_cast<unsigned __int16>(j));
    if (!m_ppkReqeust[j])
    {
      return false;
    }
  }

  CNetIndexList::SetList(m_pkEmptyInxList, infoCount);
  CNetIndexList::SetList(m_pkUseInxList, infoCount);
  for (unsigned int index = 0; index < infoCount; ++index)
  {
    CNetIndexList::PushNode_Back(m_pkEmptyInxList, index);
  }

  m_uiInfoTotCnt = infoCount;
  CMyTimer::BeginTimer(m_pkTimer, 0x7530u);
  return true;
}

void CRecallEffectController::OnLoop()
{
  if (m_pkTimer)
  {
    if (CMyTimer::CountingTimer(m_pkTimer))
    {
      UpdateClose();
    }
  }
}

void CRecallEffectController::UpdateClose()
{
  if (!m_pkUseInxList)
  {
    return;
  }

  if (!CNetIndexList::size(m_pkUseInxList))
  {
    return;
  }

  unsigned int index = 0;
  if (CNetIndexList::CopyFront(m_pkUseInxList, &index))
  {
    CRecallRequest *request = m_ppkReqeust[index];
    if (request && CRecallRequest::IsClose(request))
    {
      CPlayer *owner = CRecallRequest::GetOwner(request);
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

  unsigned int index = 0;
  if (!CNetIndexList::PopNode_Front(m_pkEmptyInxList, &index))
  {
    return nullptr;
  }

  CRecallRequest::Clear(m_ppkReqeust[index]);
  return m_ppkReqeust[index];
}

void CRecallEffectController::Close(CRecallRequest *pkRequest, bool bDone)
{
  const unsigned __int16 requestId = CRecallRequest::GetID(pkRequest);
  if (CNetIndexList::FindNode(m_pkUseInxList, requestId))
  {
    if (!CNetIndexList::IsInList(m_pkEmptyInxList, requestId))
    {
      CNetIndexList::PushNode_Front(m_pkEmptyInxList, requestId);
      CRecallRequest::Close(pkRequest, bDone);
    }
  }
}

unsigned __int8 CRecallEffectController::GetResistedRecall(unsigned __int16 wID, CRecallRequest **pkRequest)
{
  for (unsigned int j = 0; j < m_uiInfoTotCnt; ++j)
  {
    if (CRecallRequest::GetID(m_ppkReqeust[j]) == wID)
    {
      if (CRecallRequest::IsWait(m_ppkReqeust[j]))
      {
        *pkRequest = m_ppkReqeust[j];
        return 0;
      }
      if (CRecallRequest::IsTimeOut(m_ppkReqeust[j]))
      {
        return 5;
      }
      return 3;
    }
  }
  return 3;
}

char CRecallEffectController::RequestRecall(
  CPlayer *pkPerformer,
  CPlayer *pkDest,
  bool bRecallParty,
  bool bStone,
  bool bBattleModeUse)
{
  CRecallRequest *request = nullptr;
  const unsigned __int8 result = ProcessRequestRecall(
    pkPerformer,
    pkDest,
    &request,
    bRecallParty,
    bStone,
    bBattleModeUse);
  if (result)
  {
    SendRecallReqeustResult(result, pkPerformer);
    return false;
  }

  const unsigned __int16 requestId = CRecallRequest::GetID(request);
  SendRecallReqeustToDest(requestId, pkPerformer, pkDest);
  return true;
}

unsigned __int8 CRecallEffectController::ProcessRequestRecall(
  CPlayer *pkPerformer,
  CCharacter *pkDest,
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

  const unsigned __int8 result = CRecallRequest::Regist(
    *pkRequest,
    pkPerformer,
    static_cast<CPlayer *>(pkDest),
    bRecallParty,
    bStone,
    bBattleModeUse);
  if (result)
  {
    return result;
  }

  const unsigned __int16 requestId = CRecallRequest::GetID(*pkRequest);
  if (CNetIndexList::PushNode_Back(m_pkUseInxList, requestId))
  {
    return 0;
  }

  Close(*pkRequest, false);
  return 2;
}

void CRecallEffectController::SendRecallReqeustResult(char byRet, CPlayer *pkObj)
{
  _recall_request_result_zocl result{};
  result.byRet = static_cast<unsigned __int8>(byRet);
  const char *destName = CPlayerDB::GetCharNameW(&pkObj->m_Param);
  strcpy_0(result.wszDestName, destName);

  unsigned __int8 type[2]{17, 32};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    pkObj->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&result),
    sizeof(result));
}

void CRecallEffectController::SendRecallReqeustToDest(
  unsigned __int16 wRequestID,
  CPlayer *pkPerformer,
  CPlayer *pkDest)
{
  _notify_recall_request_zocl msg{};
  msg.wRequestID = wRequestID;
  const char *performerName = CPlayerDB::GetCharNameW(&pkPerformer->m_Param);
  strcpy_0(msg.wszPerformerName, performerName);

  unsigned __int8 type[2]{17, 33};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    pkDest->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    sizeof(msg));
}

void CRecallEffectController::SendDecideRecallErrorResultToDest(char byErr, CPlayer *pkDest, int nCallerMapCode)
{
  _decide_recall_error_result_zocl msg{};
  msg.byErr = static_cast<unsigned __int8>(byErr);
  msg.nCallerMapCode = nCallerMapCode;

  unsigned __int8 type[2]{17, 35};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    pkDest->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    sizeof(msg));
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
  unsigned __int8 retCode = GetResistedRecall(dwRequestID, &request);
  if (retCode)
  {
    SendDecideRecallErrorResultToDest(retCode, pkObj, -1);
    return;
  }

  if (byAgree == 1)
  {
    CPlayer *owner = CRecallRequest::GetOwner(request);
    SendRecallReqeustResult(4u, owner);
    Close(request, true);
    return;
  }

  if (CPlayer::IsSiegeMode(pkObj))
  {
    SendDecideRecallErrorResultToDest(0x1Au, pkObj, -1);
    return;
  }

  if (CHolyStoneSystem::GetDestroyerState(&g_HolySys) == 2
      && CHolyStoneSystem::GetDestroyerSerial(&g_HolySys) == pkObj->m_dwObjSerial)
  {
    SendDecideRecallErrorResultToDest(0x14u, pkObj, -1);
    return;
  }

  if (CGameObject::GetCurSecNum(CRecallRequest::GetOwner(request)) == -1
      || CRecallRequest::GetOwner(request)->m_bMapLoading)
  {
    SendDecideRecallErrorResultToDest(0x15u, pkObj, -1);
    return;
  }

  if (CRecallRequest::IsRecallParty(request))
  {
    if (pkObj->m_pCurMap != CRecallRequest::GetOwner(request)->m_pCurMap)
    {
      CPlayer *owner = CRecallRequest::GetOwner(request);
      SendDecideRecallErrorResultToDest(0xCu, pkObj, owner->m_pCurMap->m_nMapCode);
      SendRecallReqeustResult(0xDu, owner);
      Close(request, true);
      return;
    }
  }
  else
  {
    if (!CRecallRequest::IsBattleModeUse(request) && pkObj->Is_Battle_Mode(pkObj))
    {
      SendDecideRecallErrorResultToDest(0x12u, pkObj, -1);
      return;
    }

    CPlayer *owner = CRecallRequest::GetOwner(request);
    const int levelLimit = CMapData::GetLevelLimit(owner->m_pCurMap);
    const int upperLimit = owner->m_pCurMap->m_pMapSet->m_nUpLevelLim;
    const int playerLevel = pkObj->GetLevel(pkObj);
    if (playerLevel < levelLimit
        || (upperLimit != -1 && pkObj->GetLevel(pkObj) > upperLimit))
    {
      SendDecideRecallErrorResultToDest(0x13u, pkObj, -1);
      return;
    }

    if (pkObj->m_pDHChannel || CRecallRequest::GetOwner(request)->m_pDHChannel)
    {
      if (pkObj->m_pDHChannel && CRecallRequest::GetOwner(request)->m_pDHChannel)
      {
        if (pkObj->m_pDHChannel != CRecallRequest::GetOwner(request)->m_pDHChannel)
        {
          SendDecideRecallErrorResultToDest(0x16u, pkObj, -1);
          return;
        }
      }
      else
      {
        if (!pkObj->m_pDHChannel && CRecallRequest::GetOwner(request)->m_pDHChannel)
        {
          SendDecideRecallErrorResultToDest(0x16u, pkObj, -1);
          return;
        }
        if (pkObj->m_pDHChannel && !CRecallRequest::GetOwner(request)->m_pDHChannel)
        {
          SendDecideRecallErrorResultToDest(0x17u, pkObj, -1);
          return;
        }
      }
    }

    if (pkObj->m_bInGuildBattle || CRecallRequest::GetOwner(request)->m_bInGuildBattle)
    {
      if (pkObj->m_bInGuildBattle && !CRecallRequest::GetOwner(request)->m_bInGuildBattle)
      {
        SendDecideRecallErrorResultToDest(0x19u, pkObj, -1);
        return;
      }
      if (!pkObj->m_bInGuildBattle || CRecallRequest::GetOwner(request)->m_bInGuildBattle)
      {
        SendDecideRecallErrorResultToDest(9u, pkObj, -1);
        return;
      }
      if (pkObj->m_bInGuildBattle && CRecallRequest::GetOwner(request)->m_bInGuildBattle)
      {
        const unsigned int playerGuild = CPlayerDB::GetGuildSerial(&pkObj->m_Param);
        auto *battleMgr = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
        auto *playerBattle = GUILD_BATTLE::CNormalGuildBattleManager::GetBattleByGuildSerial(battleMgr, playerGuild);
        auto *playerField = GUILD_BATTLE::CNormalGuildBattle::GetField(playerBattle);
        CMapData *playerMap = GUILD_BATTLE::CNormalGuildBattleField::GetMap(playerField);

        const unsigned int ownerGuild = CPlayerDB::GetGuildSerial(&CRecallRequest::GetOwner(request)->m_Param);
        auto *ownerMgr = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
        auto *ownerBattle = GUILD_BATTLE::CNormalGuildBattleManager::GetBattleByGuildSerial(ownerMgr, ownerGuild);
        auto *ownerField = GUILD_BATTLE::CNormalGuildBattle::GetField(ownerBattle);
        CMapData *ownerMap = GUILD_BATTLE::CNormalGuildBattleField::GetMap(ownerField);
        if (playerMap != ownerMap)
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

  const bool recallAfterStone = CRecallRequest::IsRecallAfterStoneState(request);
  retCode = CRecallRequest::Recall(request, pkObj, recallAfterStone);
  if (retCode)
  {
    SendDecideRecallErrorResultToDest(retCode, pkObj, -1);
  }
  Close(request, true);
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
    for (unsigned int j = 0; j < m_uiInfoTotCnt; ++j)
    {
      delete m_ppkReqeust[j];
    }
    delete[] m_ppkReqeust;
    m_ppkReqeust = nullptr;
  }
}

void CRecallEffectController::Destroy()
{
  if (ms_Instance)
  {
    delete ms_Instance;
    ms_Instance = nullptr;
  }
}
