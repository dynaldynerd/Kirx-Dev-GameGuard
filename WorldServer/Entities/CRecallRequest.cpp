#include "pch.h"

#include "CRecallRequest.h"

#include "CPlayer.h"
#include "CPartyPlayer.h"
#include "CHolyStoneSystem.h"
#include "CGuildRoomSystem.h"
#include "CPotionMgr.h"
#include "GuildBattle.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

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

CRecallRequest::CRecallRequest(unsigned __int16 usID)
{
  m_usID = usID;
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

void CRecallRequest::Close(bool bDone)
{
  m_eState = bDone ? CUnmannedTraderSchedule::STATE::CANCEL_SUCC_COMPLETE
                   : CUnmannedTraderSchedule::STATE::WAIT_CANCEL;
  m_dwCloseTime = 0;
}

unsigned __int16 CRecallRequest::GetID()
{
  return m_usID;
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

  if (pkDest->m_bInGuildBattle && pkDest->m_bTakeGravityStone)
  {
    return 10;
  }

  if (bRecallParty)
  {
    if (!pkDest->m_bOper || !pkObj->m_pPartyMgr->IsPartyMember(pkDest))
    {
      return 3;
    }
  }
  else
  {
    if (!bBattleModeUse && pkDest->Is_Battle_Mode())
    {
      return 18;
    }

    if (pkDest->GetCurSecNum() == -1 || pkDest->m_bMapLoading)
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

      if (!pkObj->m_bInGuildBattle && pkDest->m_bInGuildBattle)
      {
        return 9;
      }

      if (pkObj->m_bInGuildBattle && pkDest->m_bInGuildBattle)
      {
        const CMapData *ownerMap = GetGuildBattleMap(pkObj->m_Param.GetGuildSerial());
        const CMapData *destMap = GetGuildBattleMap(pkDest->m_Param.GetGuildSerial());
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

  if (g_HolySys.GetDestroyerState() == 2)
  {
    if (g_HolySys.GetDestroyerSerial() == pkDest->m_dwObjSerial)
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
    const int levelLimit = pkObj->m_pCurMap->GetLevelLimit();
    if (static_cast<int>(pkDest->GetLevel()) < levelLimit)
    {
      return 19;
    }

    const int maxLevelLimit = pkObj->m_pCurMap->m_pMapSet->m_nUpLevelLim;
    if (maxLevelLimit != -1 && static_cast<int>(pkDest->GetLevel()) > maxLevelLimit)
    {
      return 19;
    }
  }

  if (pkObj->m_Param.m_pGuild)
  {
    const int raceCode = pkObj->m_Param.GetRaceCode();
    for (int roomType = 0; roomType < 2; ++roomType)
    {
      CGuildRoomSystem *roomSystem = CGuildRoomSystem::GetInstance();
      if (roomSystem->GetMapData(static_cast<unsigned __int8>(raceCode), static_cast<unsigned __int8>(roomType))
          == pkObj->m_pCurMap)
      {
        if (!pkDest->m_Param.m_pGuild)
        {
          return 3;
        }

        if (pkDest->m_Param.GetGuildSerial() != pkObj->m_Param.GetGuildSerial())
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
  m_dwDestSerial = m_pkDest->m_dwObjSerial;
  m_dwCloseTime = GetLoopTime() + 60000;
  m_eState = CUnmannedTraderSchedule::STATE::REG_WAIT;
  m_bRecallParty = bRecallParty;
  m_bStone = bStone;
  m_bBattleModeUse = bBattleModeUse;
  return 0;
}

bool CRecallRequest::IsWait()
{
  return m_eState == CUnmannedTraderSchedule::STATE::REG_WAIT;
}

bool CRecallRequest::IsTimeOut()
{
  return m_eState == CUnmannedTraderSchedule::STATE::WAIT_CANCEL;
}

bool CRecallRequest::IsRecallParty()
{
  return m_bRecallParty;
}

bool CRecallRequest::IsRecallAfterStoneState()
{
  return m_bStone;
}

bool CRecallRequest::IsBattleModeUse()
{
  return m_bBattleModeUse;
}

unsigned __int8 CRecallRequest::Recall(CPlayer *pkDest, bool bStone)
{
  if (pkDest->m_bInGuildBattle)
  {
    return 10;
  }

  if (!m_pkOwner || !m_pkOwner->m_pCurMap)
  {
    return 7;
  }

  CMapData *intoMap = m_pkOwner->m_pCurMap;
  float newPos[3]{};
  intoMap->GetRandPosInRange(m_pkOwner->m_fCurPos, 30, newPos);
  pkDest->OutOfMap(intoMap, m_pkOwner->m_wMapLayerIndex, 3u, newPos);
  pkDest->SendMsg_MovePortal(static_cast<char>(intoMap->m_pMapSet->m_dwIndex), newPos, 2u);

  if (bStone)
  {
    g_PotionMgr.InsertMovePotionStoneEffect(pkDest);
    pkDest->SenseState();
    pkDest->SendMsg_NewMovePotionResult();
  }

  return 0;
}

bool CRecallRequest::IsClose()
{
  return m_dwCloseTime != 0 && GetLoopTime() > m_dwCloseTime;
}

CPlayer *CRecallRequest::GetOwner()
{
  return m_pkOwner;
}
