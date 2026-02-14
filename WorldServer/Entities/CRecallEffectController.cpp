#include "pch.h"

#include "CRecallEffectController.h"

#include "CPlayer.h"
#include "CRecallRequest.h"
#include "CHolyStoneSystem.h"
#include "GuildBattle.h"
#include "CPotionMgr.h"
#include "map_fld.h"
#include "GlobalObjects.h"

namespace
{
unsigned __int8 GetResistedRecall(
  CRecallEffectController *controller,
  unsigned __int16 requestID,
  CRecallRequest **outRequest)
{
  if (!controller || !outRequest)
  {
    return 3;
  }

  *outRequest = nullptr;
  for (unsigned int index = 0; index < controller->m_uiInfoTotCnt; ++index)
  {
    CRecallRequest *request = controller->m_ppkReqeust ? controller->m_ppkReqeust[index] : nullptr;
    if (!request || request->m_usID != requestID)
    {
      continue;
    }

    if (request->m_eState == CUnmannedTraderSchedule::STATE::REG_WAIT)
    {
      *outRequest = request;
      return 0;
    }

    if (request->m_eState == CUnmannedTraderSchedule::STATE::WAIT_CANCEL)
    {
      return 5;
    }

    return 3;
  }

  return 3;
}

void CloseRecallRequest(CRecallEffectController *controller, CRecallRequest *request, bool done)
{
  if (!controller || !request || !controller->m_pkUseInxList || !controller->m_pkEmptyInxList)
  {
    return;
  }

  const unsigned __int16 requestID = request->m_usID;
  if (!controller->m_pkUseInxList->FindNode(requestID))
  {
    return;
  }

  if (!controller->m_pkEmptyInxList->IsInList(requestID))
  {
    controller->m_pkEmptyInxList->PushNode_Front(requestID);
    request->m_eState = done ? CUnmannedTraderSchedule::STATE::CANCEL_SUCC_COMPLETE
                             : CUnmannedTraderSchedule::STATE::WAIT_CANCEL;
    request->m_dwCloseTime = 0;
  }
}

unsigned __int8 RecallRequestTarget(CRecallRequest *request, CPlayer *dest, bool stoneRecall)
{
  if (!request || !dest)
  {
    return 3;
  }

  if (dest->m_bInGuildBattle)
  {
    return 10;
  }

  CPlayer *owner = request->m_pkOwner;
  if (!owner || !owner->m_pCurMap)
  {
    return 7;
  }

  float newPos[3] = {};
  owner->m_pCurMap->GetRandPosInRange(owner->m_fCurPos, 30, newPos);
  dest->OutOfMap(owner->m_pCurMap, owner->m_wMapLayerIndex, 3u, newPos);
  dest->SendMsg_MovePortal(static_cast<char>(owner->m_pCurMap->m_pMapSet->m_dwIndex), newPos, 2u);

  if (stoneRecall)
  {
    g_PotionMgr.InsertMovePotionStoneEffect(dest);
    dest->SenseState();
  }

  return 0;
}

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
  m_uiInfoTotCnt = infoCount;
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
      CloseRecallRequest(this, request, false);
    }
  }
}

void CRecallEffectController::SendRecallReqeustResult(char byRet, CPlayer *pkObj)
{
  if (!pkObj)
  {
    return;
  }

  #pragma pack(push, 1)
  struct RecallRequestResultMsg
  {
    char byRetCode;
    char wszDstName[17];
  };
  #pragma pack(pop)

  RecallRequestResultMsg msg{};
  msg.byRetCode = byRet;
  strcpy_0(msg.wszDstName, pkObj->m_Param.GetCharNameW());

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

#pragma pack(push, 1)
  struct RecallRequestMessage
  {
    unsigned __int16 wRequestID;
    char wszPerformerName[17];
  };
#pragma pack(pop)

  RecallRequestMessage msg{};
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

#pragma pack(push, 1)
  struct DecideRecallErrorMessage
  {
    char byErr;
    int nCallerMapCode;
  };
#pragma pack(pop)

  DecideRecallErrorMessage msg{};
  msg.byErr = byErr;
  msg.nCallerMapCode = nCallerMapCode;

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
  unsigned __int8 resistedRecall = GetResistedRecall(this, dwRequestID, &request);
  if (resistedRecall)
  {
    SendDecideRecallErrorResultToDest(static_cast<char>(resistedRecall), pkObj, -1);
    return;
  }

  CPlayer *owner = request ? request->m_pkOwner : nullptr;
  if (!owner)
  {
    SendDecideRecallErrorResultToDest(3, pkObj, -1);
    CloseRecallRequest(this, request, true);
    return;
  }

  if (byAgree == 1)
  {
    SendRecallReqeustResult(4u, owner);
    CloseRecallRequest(this, request, true);
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
      CloseRecallRequest(this, request, true);
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

    const int minLevel = owner->m_pCurMap->m_pMapSet->m_nLevelLimit;
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

  resistedRecall = RecallRequestTarget(request, pkObj, request->m_bStone);
  if (resistedRecall)
  {
    SendDecideRecallErrorResultToDest(static_cast<char>(resistedRecall), pkObj, -1);
  }
  CloseRecallRequest(this, request, true);
}
