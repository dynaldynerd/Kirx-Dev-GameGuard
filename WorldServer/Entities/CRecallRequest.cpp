#include "pch.h"

#include "CRecallRequest.h"

#include "CPlayer.h"
#include "CPlayerDB.h"
#include "CPartyPlayer.h"
#include "CMapData.h"
#include "CGuildRoomSystem.h"
#include "CHolyStoneSystem.h"
#include "CPotionMgr.h"
#include "GlobalObjects.h"
#include "GuildBattle.h"

CRecallRequest::CRecallRequest(unsigned __int16 usID)
  : m_usID(usID)
{
  Clear();
}

void CRecallRequest::Clear()
{
  m_eState = CUnmannedTraderSchedule::STATE::EMPTY;
  m_pkOwner = nullptr;
  m_dwOwnerSerial = 0;
  m_pkDest = nullptr;
  m_dwDestSerial = 0;
  m_dwCloseTime = 0;
  m_bRecallParty = false;
  m_bStone = false;
  m_bBattleModeUse = false;
}

unsigned __int8 CRecallRequest::Regist(
  CPlayer *pkObj,
  CPlayer *pkDest,
  bool bRecallParty,
  bool bStone,
  bool bBattleModeUse)
{
  if (!pkObj || pkObj->m_ObjID.m_byID || !pkDest || pkDest->m_ObjID.m_byID || pkObj == pkDest)
  {
    return 3;
  }

  if (!pkDest->m_ObjID.m_byID)
  {
    if (pkDest->m_bInGuildBattle && pkDest->m_bTakeGravityStone)
    {
      return 10;
    }
  }

  if (bRecallParty)
  {
    if (!pkDest->m_bOper || !CPartyPlayer::IsPartyMember(pkObj->m_pPartyMgr, pkDest))
    {
      return 3;
    }
  }
  else
  {
    if (!bBattleModeUse && pkDest->Is_Battle_Mode(pkDest))
    {
      return 18;
    }
    if (CGameObject::GetCurSecNum(pkDest) == -1 || pkDest->m_bMapLoading)
    {
      return 21;
    }
    if (pkDest->m_pDHChannel || pkObj->m_pDHChannel)
    {
      if (pkObj->m_pDHChannel && pkDest->m_pDHChannel)
      {
        if (pkDest->m_pDHChannel != pkObj->m_pDHChannel)
        {
          return 22;
        }
      }
      else
      {
        if (!pkObj->m_pDHChannel && pkDest->m_pDHChannel)
        {
          return 22;
        }
        if (pkObj->m_pDHChannel && !pkDest->m_pDHChannel)
        {
          return 23;
        }
      }
    }
    if (pkObj->m_bInGuildBattle || pkDest->m_bInGuildBattle)
    {
      if (pkObj->m_bInGuildBattle && !pkDest->m_bInGuildBattle)
      {
        return 25;
      }
      if (!pkObj->m_bInGuildBattle || pkDest->m_bInGuildBattle)
      {
        return 9;
      }
      if (pkObj->m_bInGuildBattle && pkDest->m_bInGuildBattle)
      {
        const unsigned int ownerGuild = CPlayerDB::GetGuildSerial(&pkObj->m_Param);
        auto *battleMgr = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
        auto *ownerBattle = GUILD_BATTLE::CNormalGuildBattleManager::GetBattleByGuildSerial(battleMgr, ownerGuild);
        auto *ownerField = GUILD_BATTLE::CNormalGuildBattle::GetField(ownerBattle);
        CMapData *ownerMap = GUILD_BATTLE::CNormalGuildBattleField::GetMap(ownerField);

        const unsigned int destGuild = CPlayerDB::GetGuildSerial(&pkDest->m_Param);
        auto *destMgr = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
        auto *destBattle = GUILD_BATTLE::CNormalGuildBattleManager::GetBattleByGuildSerial(destMgr, destGuild);
        auto *destField = GUILD_BATTLE::CNormalGuildBattle::GetField(destBattle);
        CMapData *destMap = GUILD_BATTLE::CNormalGuildBattleField::GetMap(destField);
        if (ownerMap != destMap)
        {
          return 25;
        }
        if (pkDest->m_bTakeGravityStone)
        {
          return 10;
        }
      }
    }
  }

  if (CHolyStoneSystem::GetDestroyerState(&g_HolySys) == 2)
  {
    if (CHolyStoneSystem::GetDestroyerSerial(&g_HolySys) == pkDest->m_dwObjSerial)
    {
      return 20;
    }
  }

  if (bRecallParty)
  {
    if (pkObj->m_pCurMap != pkDest->m_pCurMap)
    {
      return 11;
    }
  }
  else
  {
    const int levelLimit = CMapData::GetLevelLimit(pkObj->m_pCurMap);
    if (pkDest->GetLevel(pkDest) < levelLimit)
    {
      return 19;
    }
    const int upperLimit = pkObj->m_pCurMap->m_pMapSet->m_nUpLevelLim;
    if (upperLimit != -1 && pkDest->GetLevel(pkDest) > upperLimit)
    {
      return 19;
    }
  }

  if (pkObj->m_Param.m_pGuild)
  {
    const int raceCode = CPlayerDB::GetRaceCode(&pkObj->m_Param);
    for (int j = 0; j < 2; ++j)
    {
      CGuildRoomSystem *guildRoom = CGuildRoomSystem::GetInstance();
      if (CGuildRoomSystem::GetMapData(guildRoom, raceCode, j) == pkObj->m_pCurMap)
      {
        if (!pkDest->m_Param.m_pGuild)
        {
          return 3;
        }
        const unsigned int destGuild = CPlayerDB::GetGuildSerial(&pkDest->m_Param);
        const unsigned int srcGuild = CPlayerDB::GetGuildSerial(&pkObj->m_Param);
        if (destGuild != srcGuild)
        {
          return 3;
        }
        break;
      }
    }
  }

  m_pkOwner = pkObj;
  m_dwOwnerSerial = m_pkOwner->m_dwObjSerial;
  m_pkDest = pkDest;
  m_dwDestSerial = pkDest->m_dwObjSerial;
  m_dwCloseTime = GetLoopTime() + 60000;
  m_eState = CUnmannedTraderSchedule::STATE::REG_WAIT;
  m_bRecallParty = bRecallParty;
  m_bStone = bStone;
  m_bBattleModeUse = bBattleModeUse;
  return 0;
}

unsigned __int8 CRecallRequest::Recall(CPlayer *pkDest, bool bStone)
{
  if (pkDest->m_bInGuildBattle)
  {
    return 10;
  }
  if (!m_pkOwner->m_pCurMap)
  {
    return 7;
  }

  CMapData *intoMap = m_pkOwner->m_pCurMap;
  float newPos[3]{};
  CMapData::GetRandPosInRange(intoMap, m_pkOwner->m_fCurPos, 30, newPos);
  CPlayer::OutOfMap(pkDest, intoMap, m_pkOwner->m_wMapLayerIndex, 3u, newPos);
  CPlayer::SendMsg_MovePortal(pkDest, intoMap->m_pMapSet->m_dwIndex, newPos, 2u);
  if (bStone)
  {
    CPotionMgr::InsertMovePotionStoneEffect(&g_PotionMgr, pkDest);
    CPlayer::SenseState(pkDest);
    CPlayer::SendMsg_NewMovePotionResult(pkDest);
  }
  return 0;
}

unsigned __int16 CRecallRequest::GetID()
{
  return m_usID;
}

CPlayer *CRecallRequest::GetOwner()
{
  return m_pkOwner;
}

bool CRecallRequest::IsRecallParty()
{
  return m_bRecallParty;
}

bool CRecallRequest::IsBattleModeUse()
{
  return m_bBattleModeUse;
}

bool CRecallRequest::IsRecallAfterStoneState()
{
  return m_bStone;
}

bool CRecallRequest::IsTimeOut()
{
  return m_eState == CUnmannedTraderSchedule::STATE::WAIT_CANCEL;
}

bool CRecallRequest::IsWait()
{
  return m_eState == CUnmannedTraderSchedule::STATE::REG_WAIT;
}

bool CRecallRequest::IsClose()
{
  return m_dwCloseTime && GetLoopTime() > m_dwCloseTime;
}

void CRecallRequest::Close(bool bDone)
{
  m_eState = bDone ? CUnmannedTraderSchedule::STATE::CANCEL_SUCC_COMPLETE : CUnmannedTraderSchedule::STATE::WAIT_CANCEL;
  m_dwCloseTime = 0;
}
