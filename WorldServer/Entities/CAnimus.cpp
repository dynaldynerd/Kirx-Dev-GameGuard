#include "pch.h"

#include "CAnimus.h"
#include "ObjectCreateSetData.h"
#include "AnimusItem_fld.h"
#include "animus_fld.h"
#include "BulletItem_fld.h"
#include "CAttack.h"
#include "CMonster.h"
#include "CPartyPlayer.h"
#include "CGameObject.h"
#include "CPlayer.h"
#include "CNetProcess.h"
#include "GlobalObjects.h"
#include "PCBANG_PRIMIUM_FAVOR.h"
#include "WorldServerUtil.h"
#include "attack_gen_result_zocl.h"
#include "CMapData.h"

#include <cstdio>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <mmsystem.h>

unsigned int CAnimus::s_dwSerialCnt = 0;
CRecordData CAnimus::s_tblParameter[8];
unsigned int CAnimus::MAX_EXP[8];
int CAnimus::s_nLiveNum;

CAnimus::CAnimus()
  : m_pMaster(nullptr)
{
}

CAnimus::~CAnimus()
{
  // this is not a stub
}

namespace
{
  static char s_animusObjectName[256]{};
}

_animus_fld *GetAnimusFldFromLv(int nAnimusClass, unsigned int dwLv)
{
  CRecordData *table = &CAnimus::s_tblParameter[nAnimusClass];
  for (int n = 0; n < 65; ++n)
  {
    _animus_fld *record = reinterpret_cast<_animus_fld *>(table->GetRecord(n));
    if (record == nullptr)
    {
      return nullptr;
    }
    if (record->m_nLevel == dwLv)
    {
      return record;
    }
  }
  const int recordNum = static_cast<int>(table->GetRecordNum());
  return reinterpret_cast<_animus_fld *>(table->GetRecord(recordNum - 1));
}

bool CAnimus::SetStaticMember()
{
  static const char *kFiles[8] = {
    ".\\script\\PaimonCharacter.dat",
    ".\\script\\HecateCharacter.dat",
    ".\\script\\InannaCharacter.dat",
    ".\\script\\IsisCharacter.dat",
    ".\\script\\PaimonCharacter_S.dat",
    ".\\script\\HecateCharacter_S.dat",
    ".\\script\\InannaCharacter_S.dat",
    ".\\script\\IsisCharacter_S.dat",
  };

  char errMsg[136]{};
  for (int nAnimusClass = 0; nAnimusClass < 8; ++nAnimusClass)
  {
    if (!CAnimus::s_tblParameter[nAnimusClass].ReadRecord(
          kFiles[nAnimusClass],
          static_cast<int>(sizeof(_animus_fld)),
          errMsg))
    {
      MyMessageBox("CAnimus::SetStaticMember", errMsg);
      return false;
    }

    _animus_fld *record = GetAnimusFldFromLv(nAnimusClass, 64);
    if (record == nullptr)
    {
      MyMessageBox("CAnimus::SetStaticMember", "::GetAnimusFldFromLv(...) Fail!");
      return false;
    }

    CAnimus::MAX_EXP[nAnimusClass] = static_cast<unsigned int>(record->m_nForLvUpExp - 1);
  }
  return true;
}

bool CAnimus::Init(_object_id *pID)
{
  CCharacter::Init(pID);
  m_dwLastDestroyTime = 0;
  m_byClassCode = static_cast<unsigned __int8>(-1);
  m_nHP = 0;
  m_nFP = 0;
  m_dwExp = 0;
  m_pMaster = nullptr;
  m_dwMasterSerial = 0;
  std::memset(m_wszMasterName, 0, sizeof(m_wszMasterName));
  std::memset(m_aszMasterName, 0, sizeof(m_aszMasterName));
  m_byRoleCode = static_cast<unsigned __int8>(-1);
  m_fMoveSpeed = 0.0f;
  m_byPosRaceTown = static_cast<unsigned __int8>(-1);
  m_pBeforeTownCheckMap = nullptr;
  m_fBeforeTownCheckPos[0] = 0.0f;
  m_fBeforeTownCheckPos[1] = 0.0f;
  m_dwStunTime = 0;
  m_dwBeAttackedTargetTime = 0;
  m_pNextTarget = nullptr;
  m_nMaxAttackPnt = 0;
  m_pRecord = nullptr;
  m_nMaxHP = 0;
  m_nMaxFP = 0;
  m_Mightiness = 0.0f;
  for (int j = 0; j < 5; ++j)
  {
    m_DefPart[j] = 0;
  }
  m_dwAIMode = 0;
  m_pTarget = nullptr;
  return true;
}

bool CAnimus::Create(_animus_create_setdata *pData)
{
  if (!CCharacter::Create(pData))
  {
    return false;
  }

  m_byClassCode = static_cast<unsigned __int8>(m_pRecordSet->m_dwIndex);
  m_nHP = pData->nHP;
  m_nFP = pData->nFP;
  m_dwExp = pData->dwExp;
  m_pMaster = pData->pMaster;
  m_dwObjSerial = GetNewMonSerial();
  m_byRoleCode = static_cast<unsigned __int8>(reinterpret_cast<_AnimusItem_fld *>(m_pRecordSet)->m_nAnimusClass);
  m_nMaxAttackPnt = pData->nMaxAttackPnt;
  m_dwMasterSerial = pData->pMaster->m_dwObjSerial;
  std::strcpy(m_wszMasterName, pData->pMaster->m_Param.GetCharNameW());
  W2M(m_wszMasterName, m_aszMasterName, 17);
  m_pBeforeTownCheckMap = nullptr;
  CheckPosInTown();
  m_dwStunTime = 0;
  m_dwBeAttackedTargetTime = 0;
  m_pNextTarget = nullptr;
  AIInit();
  if (m_byRoleCode == 3)
  {
    m_pTarget = m_pMaster;
  }
  SendMsg_Create();
  if (m_pRecord->m_nUseFP > 0)
  {
    m_tmNextEatMasterFP = timeGetTime() + 1000;
  }

  int communionIndex = 0;
  for (int index = 0; index < 7; ++index)
  {
    _STORAGE_LIST::_db_con *communionItem = &m_pMaster->m_Param.m_dbEmbellish.m_pStorageList[index];
    if (communionItem->m_bLoad && communionItem->m_byTableCode == 10)
    {
      _BulletItem_fld *record =
        static_cast<_BulletItem_fld *>(g_Main.m_tblItemData[10].GetRecord(communionItem->m_wItemIndex));
      if (record && std::strcmp(record->m_strBulletType, "Q") == 0)
      {
        communionIndex = index + 1;
      }
    }
  }

  if (communionIndex)
  {
    m_pMaster->m_bCommunionEffectAnimus = true;
  }
  else
  {
    m_pMaster->m_bCommunionEffectAnimus = false;
  }

  ++CAnimus::s_nLiveNum;
  return true;
}

unsigned int CAnimus::GetNewMonSerial()
{
  return s_dwSerialCnt++;
}

void CAnimus::AIInit()
{
  m_pRecord = GetAnimusFldFromExp(m_byClassCode, m_dwExp);
  m_nMaxHP = m_pRecord->m_nMaxHP;
  m_nMaxFP = m_pRecord->m_nMaxFP;
  m_dwAIMode = 1;
  m_pTarget = nullptr;
  m_Mightiness = 1.0f;

  m_AITimer[1].Init(100);
  m_AITimer[0].Init(100);
  m_AITimer[2].Init(60000);
  m_AITimer[2].Set(0);
  m_Skill[0].Init(
    0,
    static_cast<int>(static_cast<float>(m_pRecord->m_nAttFcStd) * m_Mightiness),
    m_pRecord->m_nMinAFSelProb,
    m_pRecord->m_nMaxAFSelProb,
    static_cast<unsigned int>(m_pRecord->m_nAttExt),
    static_cast<unsigned int>(m_pRecord->m_nAttMoTime1),
    static_cast<unsigned int>(m_pRecord->m_nAttSpd),
    -1);
}

void CAnimus::CheckPosInTown()
{
  if (!m_pCurMap)
  {
    return;
  }

  if (m_pBeforeTownCheckMap == m_pCurMap
      && std::fabs(m_fCurPos[0] - m_fBeforeTownCheckPos[0]) <= 50.0f
      && std::fabs(m_fCurPos[2] - m_fBeforeTownCheckPos[1]) <= 50.0f)
  {
    return;
  }

  const unsigned __int8 race = static_cast<unsigned __int8>(GetObjRace());
  m_byPosRaceTown = m_pCurMap->GetRaceTown(m_fCurPos, race);
  m_pBeforeTownCheckMap = m_pCurMap;
  m_fBeforeTownCheckPos[0] = m_fCurPos[0];
  m_fBeforeTownCheckPos[1] = m_fCurPos[2];
}

void CAnimus::ChangeMode(unsigned int mode)
{
  if (m_dwAIMode != mode)
  {
    m_dwAIMode = mode;
    AlterMode_MasterReport(static_cast<unsigned __int8>(m_dwAIMode));
  }
}

void CAnimus::ChangeMode_MasterCommand(unsigned int nMode)
{
  ChangeMode(nMode);
}

bool CAnimus::ChangeTarget_MasterCommand(CCharacter *pTarget)
{
  if (m_byRoleCode == 3)
  {
    if (pTarget->m_ObjID.m_byID)
    {
      return false;
    }

    CPlayer *targetPlayer = static_cast<CPlayer *>(pTarget);
    if (targetPlayer->m_bInGuildBattle && m_pMaster->m_bInGuildBattle)
    {
      if (targetPlayer->m_byGuildBattleColorInx != m_pMaster->m_byGuildBattleColorInx)
      {
        return false;
      }
    }
    else
    {
      if (targetPlayer->m_bInGuildBattle || m_pMaster->m_bInGuildBattle)
      {
        return false;
      }
      if (static_cast<int>(GetObjRace()) != static_cast<int>(pTarget->GetObjRace()))
      {
        return false;
      }
    }
  }
  else
  {
    if (IsInTown())
    {
      return false;
    }
    if (!pTarget->IsAttackableInTown() && pTarget->IsInTown())
    {
      return false;
    }
    if (!pTarget->IsBeDamagedAble(this))
    {
      return false;
    }

    if (pTarget->m_ObjID.m_byID)
    {
      if (static_cast<int>(GetObjRace()) == static_cast<int>(pTarget->GetObjRace()))
      {
        return false;
      }
    }
    else
    {
      CPlayer *targetPlayer = static_cast<CPlayer *>(pTarget);
      if (targetPlayer->m_bInGuildBattle && m_pMaster->m_bInGuildBattle)
      {
        if (targetPlayer->m_byGuildBattleColorInx == m_pMaster->m_byGuildBattleColorInx)
        {
          return false;
        }
      }
      else
      {
        if (targetPlayer->m_bInGuildBattle || m_pMaster->m_bInGuildBattle)
        {
          return false;
        }
        if (static_cast<int>(GetObjRace()) == static_cast<int>(pTarget->GetObjRace())
            && !m_pMaster->IsChaosMode()
            && !targetPlayer->IsPunished(1u, false))
        {
          return false;
        }
      }
    }
  }

  ChangeMode(0);
  m_pTarget = pTarget;
  return true;
}

void CAnimus::Return_MasterRequest(unsigned __int8 byReturnType)
{
  if (m_pMaster)
  {
    m_pMaster->Return_AnimusAsk(byReturnType);
  }
}

void CAnimus::AlterMode_MasterReport(unsigned __int8 byMode)
{
  if (m_pMaster)
  {
    m_pMaster->AlterMode_Animus(byMode);
  }
}

void CAnimus::AlterHP_MasterReport()
{
  if (m_pMaster)
  {
    m_pMaster->AlterHP_Animus(static_cast<unsigned __int16>(m_nHP));
  }
}

void CAnimus::AlterFP_MasterReport()
{
  if (m_pMaster)
  {
    m_pMaster->AlterFP_Animus(static_cast<unsigned __int16>(m_nFP));
  }
}

void CAnimus::AlterExp_MasterReport(__int64 nAlterExp)
{
  if (m_pMaster)
  {
    m_pMaster->AlterExp_Animus(nAlterExp);
  }
}

void CAnimus::TransPoToMaster()
{
  if (!m_pMaster)
  {
    return;
  }

  const int qLen = GetQLen(m_pMaster->m_fCurPos, m_fCurPos);
  if (qLen >= 300)
  {
    m_pTarget = nullptr;
    Return_MasterRequest(3u);
  }
}

void CAnimus::GetTarget()
{
  if (!m_pMaster)
  {
    return;
  }

  const int qLenMaster = GetQLen(m_pMaster->m_fCurPos, m_fCurPos);
  if (qLenMaster >= 300)
  {
    m_pTarget = nullptr;
    Return_MasterRequest(3u);
    return;
  }

  if (qLenMaster >= m_pRecord->m_nViewExt)
  {
    m_pTarget = nullptr;
    ChangeMode(1u);
    return;
  }

  if (!m_pTarget)
  {
    return;
  }

  if (!m_pTarget->m_bLive || m_pTarget->m_bCorpse)
  {
    m_pTarget = nullptr;
    return;
  }

  const int qLenTarget = GetQLen(m_pTarget->m_fCurPos, m_fCurPos);
  if (qLenTarget >= m_pRecord->m_nViewExt)
  {
    m_pTarget = nullptr;
    ChangeMode(1u);
  }
}

bool CAnimus::GetMoveTarget(CCharacter *target, float fMoveSpeed, unsigned __int8 byMoveMode)
{
  if (!m_AITimer[1].Check())
  {
    return false;
  }

  if (!target)
  {
    return false;
  }

  float src[3]{};
  std::memcpy(src, target->m_fCurPos, sizeof(src));
  if (target->m_bMove)
  {
    if ((!byMoveMode && target == m_pMaster)
        || std::abs(static_cast<int>(GetYAngle(m_fCurPos, target->m_fCurPos))
                    - static_cast<int>(GetYAngle(target->m_fCurPos, target->m_fTarPos))) < 16384)
    {
      std::memcpy(src, target->m_fTarPos, sizeof(src));
    }
    else
    {
      std::memcpy(src, target->m_fCurPos, sizeof(src));
    }
  }

  float tar[3]{};
  std::memcpy(tar, src, sizeof(tar));

  float moveDist = 25.0f;
  if (target != m_pMaster)
  {
    moveDist = GetAttackRange() * 0.8f;
  }

  const float angle = GetAngle(m_fCurPos, src);
  tar[0] += static_cast<float>(std::cos(angle) * moveDist);
  tar[2] += static_cast<float>(std::sin(angle) * moveDist);

  if (byMoveMode != 1)
  {
    const double curDist = Get3DSqrt(m_fCurPos, src);
    const double moveTargetDist = Get3DSqrt(m_fCurPos, tar);
    if (moveTargetDist >= curDist)
    {
      return false;
    }

    int addDist = 0;
    if (!m_bMove)
    {
      addDist = 25;
    }

    for (int i = 0; i < 2; ++i)
    {
      if (m_Skill[i].m_bLoad && target != m_pMaster)
      {
        const double targetDist = Get3DSqrt(m_fCurPos, target->m_fCurPos);
        if (static_cast<float>(addDist + m_Skill[i].m_Len) >= targetDist)
        {
          return false;
        }
      }
    }
  }

  if (Get3DSqrt(m_fTarPos, tar) >= 25.0)
  {
    const bool bColl = byMoveMode != 0;
    if (SetTarPos(tar, bColl))
    {
      m_fMoveSpeed = fMoveSpeed;
      SendMsg_Move();
      return true;
    }
    return false;
  }

  if (m_fMoveSpeed != fMoveSpeed && m_bMove)
  {
    m_fMoveSpeed = fMoveSpeed;
    SendMsg_Move();
  }

  return false;
}

int CAnimus::GetAttackPart()
{
  const int randomValue = rand() % 100;
  if (randomValue <= 20)
  {
    return 4;
  }
  if (randomValue <= 43)
  {
    return 0;
  }
  if (randomValue <= 65)
  {
    return 1;
  }
  if (randomValue <= 83)
  {
    return 2;
  }
  if (randomValue > 100)
  {
    return -1;
  }
  return 3;
}

CCharacter *CAnimus::SearchNearEnemy()
{
  for (int j = 0; j < 5; ++j)
  {
    if (m_AroundSlot[j])
    {
      return m_AroundSlot[j];
    }
  }
  return nullptr;
}

CCharacter *CAnimus::SearchNearPlayerAttack()
{
  const float angle = GetAngle(m_fCurPos, m_pMaster->m_fCurPos);
  int side = static_cast<int>((angle / (2.0 * 3.1415926535) * 5.0) + 0.5) - 1;
  int left = side;
  int right = side;
  for (int j = 0; j < 5; ++j)
  {
    if (left >= 5)
    {
      left = 0;
    }
    if (right < 0)
    {
      right = 4;
    }
    if (m_pMaster->m_AroundSlot[left])
    {
      return m_pMaster->m_AroundSlot[left];
    }
    if (m_pMaster->m_AroundSlot[right])
    {
      return m_pMaster->m_AroundSlot[right];
    }
    ++left;
    --right;
  }
  return nullptr;
}

void CAnimus::make_gen_attack_param(CCharacter *pDst, unsigned __int8 byPart, _attack_param *pAP, int nSkillIndex)
{
  pAP->pDst = pDst;
  unsigned __int8 forcedPart = 255;
  if (pDst && !pDst->m_ObjID.m_byID)
  {
    forcedPart = static_cast<CPlayer *>(pDst)->m_byDamagePart;
  }

  if (!pDst || pDst->m_ObjID.m_byID || forcedPart == 255)
  {
    pAP->nPart = byPart;
  }
  else
  {
    pAP->nPart = forcedPart;
  }
  pAP->nClass = m_byRoleCode != 1;
  pAP->nTol = m_Skill[nSkillIndex].m_Element;
  pAP->nMinAF = m_Skill[nSkillIndex].m_MinDmg;
  pAP->nMaxAF = m_Skill[nSkillIndex].m_MaxDmg;
  if (m_byRoleCode == 4)
  {
    pAP->nMaxAF = static_cast<int>(static_cast<float>(pAP->nMaxAF) * m_pMaster->m_EP.GetEff_Rate(EFF_RATE_ANIMUS_ATTACK));
  }
  pAP->nMinSel = m_Skill[nSkillIndex].m_MinProb;
  pAP->nMaxSel = m_Skill[nSkillIndex].m_MaxProb;
  if (m_byRoleCode == 2)
  {
    pAP->nMaxSel = static_cast<int>(static_cast<float>(pAP->nMaxSel) - m_pMaster->m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_39));
    pAP->nAttactType = 6;
    pAP->nExtentRange = 15;
    if (pDst)
    {
      std::memcpy(pAP->fArea, pDst->m_fCurPos, sizeof(pAP->fArea));
    }
  }
  pAP->nMaxAttackPnt = m_nMaxAttackPnt;
}

void CAnimus::CalcAttExpForPlayer(CAttack *pAT)
{
  bool bPlayerGetExp = true;
  bool bAnimusGetExp = true;
  const int level = GetLevel();
  if (!m_pMaster || (level - 1 >= 50 && m_pMaster->GetLevel() < level - 1))
  {
    bAnimusGetExp = false;
  }

  bool bGetAttackExp = false;
  for (int i = 0; i < pAT->m_nDamagedObjNum; ++i)
  {
    CCharacter *pDst = pAT->m_DamList[i].m_pChar;
    const int damage = pAT->m_DamList[i].m_nDamage;
    bPlayerGetExp = m_pMaster->IsPassExpLimitLvDiff(pDst->GetLevel(), &bGetAttackExp);
    if (std::abs(GetLevel() - pDst->GetLevel()) > 10)
    {
      bAnimusGetExp = false;
    }

    if ((bPlayerGetExp || bAnimusGetExp) && pDst->m_ObjID.m_byID == 1 && damage > 1 && !IsInTown())
    {
      _monster_fld *pMonRec = reinterpret_cast<_monster_fld *>(pDst->m_pRecordSet);
      CMonster *pMon = static_cast<CMonster *>(pDst);
      int nLeftHP = pDst->GetHP() - damage;
      int nCurDam = damage;
      if (nLeftHP < 0)
      {
        nLeftHP = 0;
        nCurDam = pMon->GetHP();
      }

      const float fSetExt = (pMonRec->m_fExt * 0.69999999f) * (static_cast<float>(nCurDam) / pMonRec->m_fMaxHP);
      if (bAnimusGetExp)
      {
        const int nAddExp = static_cast<int>((fSetExt / 500.0f) + static_cast<float>(pMon->GetLevel()));
        AlterExp(nAddExp);
      }

      if (!nLeftHP)
      {
        float fKillExt = pMon->GetEmotionState() == 4 ? pMonRec->m_fExt * 0.5f : pMonRec->m_fExt * 0.30000001f;
        if (m_pMaster->m_pPartyMgr->IsPartyMode())
        {
          CPlayer *pMember[8]{};
          const unsigned __int8 byPartyNum = m_pMaster->_GetPartyMemberInCircle(pMember, 8, true);
          if (byPartyNum)
          {
            fKillExt *= CPlayer::s_fExpDivUnderParty_Kill[byPartyNum - 1];
          }

          float fTotalLv = 0.0f;
          float fPowMemLv[11]{};
          for (int k = 0; k < byPartyNum; ++k)
          {
            fPowMemLv[k] = std::pow(static_cast<float>(pMember[k]->GetLevel()), 3.0f);
            fTotalLv += fPowMemLv[k];
          }

          for (int k = 0; k < byPartyNum; ++k)
          {
            const float fAddKillExt = (fKillExt * fPowMemLv[k]) / fTotalLv;
            if (pMember[k] == m_pMaster)
            {
              if (bAnimusGetExp)
              {
                const int addExp = static_cast<int>((fAddKillExt / 500.0f) + static_cast<float>(pMon->GetLevel()));
                AlterExp(addExp);
              }
              if (bPlayerGetExp)
              {
                m_pMaster->AlterExp(fAddKillExt, false, false, false);
              }
            }

            if (pMember[k]->IsRidingUnit())
            {
              if (bPlayerGetExp)
              {
                pMember[k]->AlterExp(fAddKillExt, false, false, false);
                const int addExp = static_cast<int>((fAddKillExt / 180.0f) + static_cast<float>(pMon->GetLevel()));
                pMember[k]->Emb_AlterStat(6u, 0, addExp, 0, nullptr, true);
              }
            }
            else
            {
              if (bPlayerGetExp)
              {
                pMember[k]->AlterExp(fAddKillExt, false, false, false);
              }
              if (pMember[k] != m_pMaster && pMember[k]->m_pRecalledAnimusChar)
              {
                const int addExp = static_cast<int>((fAddKillExt / 5000.0f) + static_cast<float>(pMon->GetLevel()));
                pMember[k]->m_pRecalledAnimusChar->AlterExp(addExp);
              }
            }
          }
        }
        else
        {
          if (bAnimusGetExp)
          {
            const int addExp = static_cast<int>((fKillExt / 500.0f) + static_cast<float>(pMon->GetLevel()));
            AlterExp(addExp);
          }
          if (bPlayerGetExp)
          {
            m_pMaster->AlterExp(fKillExt, false, false, false);
          }
        }
      }
    }
  }
}

bool CAnimus::IsValidTarget()
{
  if (!m_pTarget)
  {
    return false;
  }
  if (!m_pTarget->m_bLive || m_pTarget->m_bCorpse)
  {
    return false;
  }
  if (!m_pTarget->IsBeAttackedAble(true))
  {
    return false;
  }
  if (m_byRoleCode == 1 || m_byRoleCode == 2 || m_byRoleCode == 4)
  {
    if (m_pTarget->m_bObserver)
    {
      return false;
    }
    if (m_pTarget->GetStealth(true))
    {
      return false;
    }
    if (IsInTown() && !m_pTarget->IsAttackableInTown())
    {
      return false;
    }
    if (!m_pTarget->IsAttackableInTown() && m_pTarget->IsInTown())
    {
      return false;
    }
  }
  else if (m_pTarget == m_pMaster)
  {
    return true;
  }
  return true;
}

bool CAnimus::Attack(int skill)
{
  if (!m_pTarget)
  {
    return false;
  }
  if (!IsValidTarget())
  {
    m_pTarget = nullptr;
    return false;
  }

  float canGoResult[7]{};
  if (!(unsigned int)m_pCurMap->m_Level.mBsp->CanYouGoThere(
        m_pTarget->m_fCurPos,
        m_fCurPos,
        reinterpret_cast<float (*)[3]>(canGoResult)))
  {
    m_pTarget = nullptr;
    return false;
  }

  if (!m_Skill[skill].m_Type)
  {
    const int attackPart = static_cast<int>(GetAttackPart());
    _attack_param attackParam;
    make_gen_attack_param(m_pTarget, static_cast<unsigned __int8>(attackPart), &attackParam, skill);

    CAttack attack(this);
    attack.AttackGen(&attackParam, false, false);
    SendMsg_Attack_Gen(&attack);
    if (!attack.m_bFailure && m_byRoleCode != 1)
    {
      CalcAttExpForPlayer(&attack);
    }

    for (int j = 0; j < attack.m_nDamagedObjNum; ++j)
    {
      attack.m_DamList[j].m_pChar->SetDamage(
        attack.m_DamList[j].m_nDamage,
        this,
        static_cast<int>(GetLevel()),
        attack.m_bIsCrtAtt,
        -1,
        0,
        true);
    }
  }
  return true;
}

bool CAnimus::Heal(unsigned int skill)
{
  const bool isInanna = m_byRoleCode == 3;
  CPlayer *healedPlayer = m_pMaster;
  if (m_pTarget)
  {
    if (m_pTarget->m_ObjID.m_byID)
    {
      m_pTarget = m_pMaster;
      return false;
    }

    CPlayer *targetPlayer = static_cast<CPlayer *>(m_pTarget);
    if (targetPlayer->m_bInGuildBattle && m_pMaster->m_bInGuildBattle)
    {
      if (targetPlayer->m_byGuildBattleColorInx != m_pMaster->m_byGuildBattleColorInx)
      {
        if (isInanna)
        {
          AnimusDebugLog("InannaExp: Heal blocked (guild battle color mismatch)");
        }
        m_pTarget = m_pMaster;
        return false;
      }
      if (targetPlayer->m_bTakeGravityStone)
      {
        if (isInanna)
        {
          AnimusDebugLog("InannaExp: Heal blocked (target state invalid in guild battle)");
        }
        m_pTarget = m_pMaster;
        return false;
      }
      healedPlayer = targetPlayer;
    }
    else
    {
      if (targetPlayer->m_bInGuildBattle || m_pMaster->m_bInGuildBattle)
      {
        if (isInanna)
        {
          AnimusDebugLog("InannaExp: Heal blocked (guild battle or target state)");
        }
        m_pTarget = m_pMaster;
        return false;
      }
      if (static_cast<int>(GetObjRace()) == static_cast<int>(targetPlayer->GetObjRace()))
      {
        healedPlayer = targetPlayer;
      }
    }
  }

  m_pTarget = m_pMaster;
  const int currentHP = static_cast<int>(healedPlayer->GetHP());
  if (currentHP <= 0)
  {
    if (isInanna)
    {
      AnimusDebugLog("InannaExp: Heal early return (target HP <= 0)");
    }
    return false;
  }

  const float hpRate = static_cast<float>(currentHP) / static_cast<float>(static_cast<int>(healedPlayer->GetMaxHP()));
  if (hpRate >= 1.0f)
  {
    if (isInanna)
    {
      AnimusDebugLog("InannaExp: Heal early return (target already full HP)");
    }
    return false;
  }

  const float effectRate = m_pMaster->m_EP.GetEff_Rate(EFF_RATE_ANIMUS_HEAL);
  const int addHP = static_cast<int>(m_Skill[skill].GetDmg(effectRate));
  if (addHP <= 0)
  {
    if (isInanna)
    {
      AnimusDebugLog("InannaExp: Heal early return (addHP <= 0)");
    }
    return false;
  }

  SendMsg_AnimusActHealInform(healedPlayer->m_Param.GetCharSerial(), static_cast<__int16>(addHP));
  healedPlayer->SetHP(currentHP + addHP, false);
  healedPlayer->SendMsg_AlterHPInform();

  if (!IsInTown())
  {
    const int selfLevel = static_cast<int>(GetLevel());
    if (m_pMaster)
    {
      if (selfLevel - 1 < 50 || m_pMaster->GetLevel() >= selfLevel - 1)
      {
        const int addExp = 2 * addHP;
        if (addExp > 0)
        {
          if (isInanna)
          {
            AnimusDebugLog(
              "InannaExp: Heal gain addHP=%d addExp=%d animusLevel=%d masterLevel=%d",
              addHP,
              addExp,
              selfLevel,
              static_cast<int>(m_pMaster->GetLevel()));
          }
          AlterExp(addExp);
        }
      }
      else if (isInanna)
      {
        AnimusDebugLog(
          "InannaExp: Heal no exp (level gate) animusLevel=%d masterLevel=%d",
          selfLevel,
          static_cast<int>(m_pMaster->GetLevel()));
      }
    }
  }

  const int hpCost = addHP / 10;
  if (hpCost > 0)
  {
    m_nHP -= hpCost;
    if (m_nHP > 0)
    {
      AlterHP_MasterReport();
    }
    else
    {
      m_nHP = 0;
      Return_MasterRequest(1u);
    }
  }

  return true;
}

void CAnimus::Action()
{
  if (!m_AITimer[0].Check() || !m_pTarget)
  {
    return;
  }
  if (!IsValidTarget())
  {
    m_pTarget = nullptr;
    return;
  }

  const unsigned int now = timeGetTime();
  const unsigned int targetDistance = static_cast<unsigned int>(Get3DSqrt(m_fCurPos, m_pTarget->m_fCurPos));
  int extraRange = 0;
  if (!m_bMove)
  {
    extraRange = 25;
  }

  unsigned int skill = 0;
  for (; skill < 2; ++skill)
  {
    if (!m_Skill[skill].m_bLoad)
    {
      continue;
    }
    if (!m_Skill[skill].m_Active)
    {
      const int elapsed = static_cast<int>(now - m_Skill[skill].m_BefTime);
      if (elapsed > static_cast<int>(m_Skill[skill].m_Delay) || elapsed < 0)
      {
        m_Skill[skill].m_Active = 1;
      }
    }
    if (m_Skill[skill].m_Active && targetDistance <= static_cast<unsigned int>(extraRange + static_cast<int>(m_Skill[skill].m_Len)))
    {
      break;
    }
  }

  if (skill >= 2)
  {
    return;
  }

  m_Skill[skill].m_Active = 0;
  m_Skill[skill].m_BefTime = now;

  bool success = true;
  if (m_byRoleCode)
  {
    if (m_byRoleCode <= 2u || m_byRoleCode == 4)
    {
      success = Attack(static_cast<int>(skill));
    }
    else if (m_byRoleCode == 3)
    {
      success = Heal(skill);
    }
  }

  if (success)
  {
    m_AITimer[0].Set(m_Skill[skill].m_CastDelay);
    m_AITimer[1].Set(m_Skill[skill].m_CastDelay);
    if (m_bMove)
    {
      Stop();
    }
  }
}

void CAnimus::LifeTimeCheck()
{
  const double targetDist = Get3DSqrt(m_fCurPos, m_fTarPos);
  const bool hasMoveTarget = targetDist != 0.0;

  if (m_pTarget || hasMoveTarget)
  {
    m_AITimer[2].Set(0);
  }
  else if (m_AITimer[2].Check())
  {
    Return_MasterRequest(1u);
  }
}

void CAnimus::Process()
{
  if (!m_pRecord || !m_pMaster)
  {
    return;
  }

  float moveSpeed = static_cast<float>(m_pRecord->m_nMovSpd);
  if (m_byRoleCode == 3)
  {
    moveSpeed = m_pMaster->GetMoveSpeed();
  }

  if (m_dwAIMode)
  {
    if (m_dwAIMode == 1)
    {
      if (m_byRoleCode != 3)
      {
        m_pTarget = nullptr;
      }

      GetMoveTarget(m_pMaster, moveSpeed, 1u);
      if (GetAttackRange() > Get3DSqrt(m_fCurPos, m_pMaster->m_fCurPos))
      {
        ChangeMode(0u);
      }
    }
  }
  else
  {
    if (m_byRoleCode == 3)
    {
      TransPoToMaster();
    }
    else
    {
      GetTarget();
    }

    if (m_pTarget)
    {
      Action();
      GetMoveTarget(m_pTarget, moveSpeed, 0);
    }
    else
    {
      GetMoveTarget(m_pMaster, m_pMaster->GetMoveSpeed(), 0);
    }
  }

  LifeTimeCheck();
  Move(m_fMoveSpeed);
}

void CAnimus::_ProcComsumeMaterFP()
{
  if (!m_pMaster || !m_pRecord)
  {
    return;
  }

  if (!m_pMaster->m_bCorpse)
  {
    int newFP = m_pMaster->GetFP() - m_pRecord->m_nUseFP;
    if (newFP < 0)
    {
      newFP = 0;
    }
    m_pMaster->SetFP(newFP, false);
  }

  if (m_pMaster->GetFP() <= 0)
  {
    m_pMaster->_AnimusReturn(0);
  }

  m_tmNextEatMasterFP = GetLoopTime() + 1000;
}

bool CAnimus::Destroy()
{
  m_dwLastDestroyTime = timeGetTime();
  SendMsg_Destroy();
  m_pMaster = nullptr;
  m_dwObjSerial = static_cast<unsigned int>(-1);
  m_tmNextEatMasterFP = static_cast<unsigned int>(-1);
  --CAnimus::s_nLiveNum;
  return CCharacter::Destroy();
}

void CAnimus::SendMsg_Destroy()
{
  _animus_destroy_zocl msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  msg.byDestroyCode = static_cast<char>(m_nHP <= 0);

  unsigned __int8 pbyType[36]{};
  pbyType[0] = 3;
  pbyType[1] = 26;
  CircleReport(pbyType, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(sizeof(msg)), false);
}

void CAnimus::SendMsg_Create()
{
  _animus_create_zocl msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.wRecIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  msg.dwSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, msg.zPos, 3);
  msg.byLv = static_cast<char>(m_pRecord ? m_pRecord->m_nLevel : 1);
  msg.dwMasterSerial = m_pMaster->m_dwObjSerial;

  unsigned __int8 type[2]{3, 18};
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(sizeof(msg)), false);
}

void CAnimus::SendMsg_Move()
{
  _animus_move_zocl msg{};
  msg.dwSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, msg.zCur, 3);
  msg.zTar[0] = static_cast<short>(m_fTarPos[0]);
  msg.zTar[1] = static_cast<short>(m_fTarPos[2]);
  msg.fSpeed = m_fMoveSpeed;

  unsigned __int8 type[2]{4, 7};
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(sizeof(msg)), false);
}

void CAnimus::SendMsg_Attack_Gen(CAttack *pAT)
{
  _attack_gen_result_zocl result{};
  result.byAtterID = m_ObjID.m_byID;
  result.dwAtterSerial = m_dwObjSerial;
  result.byAttackPart = static_cast<unsigned __int8>(pAT->m_pp->nPart);
  result.bCritical = pAT->m_bIsCrtAtt;
  result.wBulletIndex = static_cast<unsigned __int16>(-1);
  result.byListNum = static_cast<unsigned __int8>(pAT->m_nDamagedObjNum);
  for (int index = 0; index < pAT->m_nDamagedObjNum; ++index)
  {
    result.DamList[index].byDstID = pAT->m_DamList[index].m_pChar->m_ObjID.m_byID;
    result.DamList[index].dwDstSerial = pAT->m_DamList[index].m_pChar->m_dwObjSerial;
    result.DamList[index].wDamage = static_cast<unsigned __int16>(pAT->m_DamList[index].m_nDamage);
  }

  unsigned __int8 type[2]{5, 7};
  CircleReport(type, reinterpret_cast<char *>(&result), static_cast<unsigned __int16>(result.size()), false);
}

void CAnimus::SendMsg_LevelUp()
{
  _animus_lvup_inform_zocl msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  msg.byLv = static_cast<char>(m_pRecord ? m_pRecord->m_nLevel : 1);

  unsigned __int8 type[2]{22, 12};
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(sizeof(msg)), false);
}

void CAnimus::SendMsg_AnimusActHealInform(unsigned int dwDstSerial, __int16 nAddHP)
{
  _animus_act_heal_inform_zocl msg{};
  msg.dwAnimusSerial = m_dwObjSerial;
  msg.dwDstSerial = dwDstSerial;
  msg.wAddHP = static_cast<unsigned __int16>(nAddHP);

  unsigned __int8 type[2]{22, 14};
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(sizeof(msg)), false);
}

void CAnimus::SendMsg_NotifyGetExpInfo(unsigned __int64 dwOldExp, __int64 nAlterExp, unsigned __int64 dwCurExp)
{
  if (!g_Main.IsReleaseServiceMode())
  {
    static unsigned int s_AnimusExpNotifyIndex = 0;

    _trans_gm_msg_inform_zocl msg{};
    std::memset(&msg, 0, sizeof(msg));
    msg.wMsgSize = _snprintf(
      msg.wszChatData,
      255,
      "%u Animus Old Exp     : %d\n   Animus Alter Exp   : %d\n   Animus Cur Exp     : %d\n",
      ++s_AnimusExpNotifyIndex,
      static_cast<int>(dwOldExp),
      static_cast<int>(nAlterExp),
      static_cast<int>(dwCurExp));

    unsigned __int8 type[2] = {2, 13};
    const unsigned __int16 length = static_cast<unsigned __int16>(msg.size());
    g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), length);
  }
}

void CAnimus::MasterAttack_MasterInform(CCharacter *pDst)
{
  if (!m_pTarget)
  {
    m_pTarget = pDst;
  }
}

void CAnimus::MasterBeAttacked_MasterInform(CCharacter *pDst)
{
  if (!m_pTarget)
  {
    m_pTarget = pDst;
  }
}

int CAnimus::AttackableHeight()
{
  return 50;
}

int CAnimus::GetAttackDP()
{
  return static_cast<int>(m_pRecord->m_nAttack_DP);
}

float CAnimus::GetAttackRange()
{
  return static_cast<float>(m_pRecord->m_nAttExt);
}

int CAnimus::GetDefFC(int nAttactPart, CCharacter *pAttChar, int *pnConvertPart)
{
int baseDefFc = m_pRecord->m_nStdDefFc;
  if (m_pMaster != nullptr && m_byRoleCode == 1)
  {
    baseDefFc = static_cast<int>(static_cast<float>(baseDefFc) * m_pMaster->m_EP.GetEff_Rate(EFF_RATE_ANIMUS_DEFENSE));
  }
  return baseDefFc;
}

float CAnimus::GetDefFacing(int nPart)
{
return m_pRecord->m_fDefFacing;
}

float CAnimus::GetDefGap(int nPart)
{
return m_pRecord->m_fDefGap;
}

int CAnimus::GetDefSkill(bool bBackAttackDamage)
{
  return static_cast<int>(m_pRecord->m_nDefSklUnit);
}

int CAnimus::GetFireTol()
{
  return static_cast<int>(m_pRecord->m_nFireTol);
}

int CAnimus::GetGenAttackProb(CCharacter *pDst, int nPart, bool bBackAttack)
{
  const double attackSkill = static_cast<double>(m_pRecord->m_nAttSklUnit);
  const float targetLevel = static_cast<float>(pDst->GetLevel());
  const int targetDefSkill = static_cast<int>(m_pTarget->GetDefSkill(bBackAttack));
  int probability = static_cast<int>(attackSkill - (targetLevel + static_cast<float>(targetDefSkill)) / 4.0f + 95.0f);
  probability -= static_cast<int>(pDst->GetAvoidRate());

  if (probability < 5)
  {
    return 5;
  }
  if (probability > 95)
  {
    return 95;
  }
  return probability;
}

int CAnimus::GetHP()
{
  return m_nHP;
}

int CAnimus::GetLevel()
{
  return static_cast<int>(m_pRecord->m_nLevel);
}

unsigned __int8 CAnimus::GetMaxLevel()
{
  // narrowing cast for thunk return parity
  return static_cast<unsigned __int8>(m_pMaster->m_Param.GetMaxLevel());
}

void CAnimus::AlterExp(__int64 nAddExp)
{
  if (m_pMaster && m_pMaster->m_bOper)
  {
    if (GetLevel() < GetMaxLevel())
    {
      double playerPenalty = DOUBLE_1_0;
      playerPenalty = g_Main.m_pTimeLimitMgr->GetPlayerPenalty(m_pMaster->m_id.wIndex);

      const __int64 baseExp = nAddExp;
      int addExpWithRate = 0;
      if (m_pMaster->IsApplyPcbangPrimium())
      {
        addExpWithRate = static_cast<int>(
          static_cast<float>(static_cast<int>(nAddExp))
          + static_cast<float>(static_cast<int>(baseExp) * (PCBANG_PRIMIUM_FAVOR::ANIMUS_EXP - 1.0f)));
      }
      else
      {
        addExpWithRate = static_cast<int>(
          static_cast<float>(static_cast<int>(nAddExp))
          + static_cast<float>(static_cast<int>(baseExp) * (ANIMUS_EXP_RATE - 1.0f)));
      }

      const double scaledExp = static_cast<double>(addExpWithRate) * playerPenalty;
      unsigned __int64 addExp = static_cast<unsigned int>(static_cast<int>(scaledExp));
      if (static_cast<int>(scaledExp) && m_pRecord->m_nForLvUpExp - m_dwExp < addExp)
      {
        if (m_pRecord->m_nForLvUpExp - m_dwExp <= 9223372036854775807)
        {
          addExp = m_pRecord->m_nForLvUpExp - m_dwExp;
        }
        else
        {
          addExp = 9223372036854775807;
          g_Main.m_logSystemError.Write(
            "CAnimus::AlterExp(__int64 nAddExp(%I64d) ) : m_pRecord->m_nForLvUpExp - m_dwExp(%I64u) Invalid!",
            9223372036854775807,
            m_pRecord->m_nForLvUpExp - m_dwExp);
        }

        if ((-1 - m_dwExp) < addExp)
        {
          const unsigned __int64 oldAddExp = addExp;
          if ((-1 - m_dwExp) <= 9223372036854775807)
          {
            addExp = -1 - m_dwExp;
            g_Main.m_logSystemError.Write(
              "CAnimus::AlterExp( __int64 nAddExp ) : _UI64_MAX < m_dwExp(%I64u), nOldExp(%I64u) : nAddExp(%I64d) Invalid!",
              m_dwExp,
              oldAddExp,
              addExp);
          }
          else
          {
            addExp = 9223372036854775807;
            g_Main.m_logSystemError.Write(
              "CAnimus::AlterExp( __int64 nAddExp ) : ( _UI64_MAX < m_dwExp + nAddExp ) && ( _I64_MAX < _UI64_MAX - m_dwExp )\r\nnOldExp(%I64u) : nAddExp(%I64d) Invalid!",
              oldAddExp,
              9223372036854775807);
          }
        }
      }

      if (addExp)
      {
        const unsigned __int64 oldExp = m_dwExp;
        const unsigned __int64 newExp = addExp + m_dwExp;
        _animus_fld *newAnimusField = GetAnimusFldFromExp(m_byClassCode, newExp);
        if (newAnimusField == m_pRecord)
        {
          m_dwExp = newExp;
          AlterExp_MasterReport(static_cast<__int64>(addExp));
        }
        else if (m_pMaster)
        {
          if ((m_pMaster->GetLevel() >= 50 || newAnimusField->m_nLevel > 50)
              && (m_pMaster->GetLevel() < 50 || newAnimusField->m_nLevel > m_pMaster->GetLevel() + 1))
          {
            m_dwExp = newExp - 1;
            AlterExp_MasterReport(static_cast<__int64>(addExp - 1));
          }
          else
          {
            m_dwExp = newExp;
            AlterExp_MasterReport(static_cast<__int64>(addExp));
            m_pRecord = newAnimusField;
            SendMsg_LevelUp();
            m_nMaxHP = m_pRecord->m_nMaxHP;
            m_nMaxFP = m_pRecord->m_nMaxFP;
          }
        }
        SendMsg_NotifyGetExpInfo(oldExp, addExp, m_dwExp);
      }
    }
  }
}

void CAnimus::CalcDefExp(CCharacter *pAttackObj, int nDamage)
{
  if (pAttackObj)
  {
    if (pAttackObj->m_ObjID.m_byID == 1 && nDamage > 1 && !IsInTown())
    {
      const int selfLevel = static_cast<int>(GetLevel());
      const int attackLevel = static_cast<int>(pAttackObj->GetLevel());
      if (std::abs(selfLevel - attackLevel) <= 10)
      {
        if (m_pMaster)
        {
          const int checkLevel = static_cast<int>(GetLevel());
          if (((m_pMaster->GetLevel() < 50) && checkLevel <= 50)
              || ((m_pMaster->GetLevel() >= 50) && checkLevel <= m_pMaster->GetLevel() + 1))
          {
            const int addExp = 4 * nDamage;
            if (addExp)
            {
              AlterExp(addExp);
            }
          }
        }
      }
    }
  }
}

int CAnimus::GetMaxHP()
{
  if (m_nMaxHP <= 1)
  {
    return 1;
  }
  return m_nMaxHP;
}

char *CAnimus::GetObjName()
{
  const int raceCode = static_cast<int>(GetObjRace());
  const int posX = static_cast<int>(m_fCurPos[0]);
  const int posY = static_cast<int>(m_fCurPos[1]);
  const int posZ = static_cast<int>(m_fCurPos[2]);
  sprintf_s(s_animusObjectName, sizeof(s_animusObjectName),
    "[ANIMUS][%d] >> %s (pos: %s {%d, %d, %d}) (master: %s)",
    raceCode,
    m_pRecord->m_strCode,
    m_pCurMap->m_pMapSet->m_strCode,
    posX,
    posY,
    posZ,
    m_aszMasterName);
  return s_animusObjectName;
}

int CAnimus::GetObjRace()
{
  if (m_pMaster != nullptr)
  {
    return m_pMaster->GetObjRace();
  }
  return -1;
}

int CAnimus::GetSoilTol()
{
  return static_cast<int>(m_pRecord->m_nSoilTol);
}

int CAnimus::GetWaterTol()
{
  return static_cast<int>(m_pRecord->m_nWaterTol);
}

float CAnimus::GetWeaponAdjust()
{
  return m_pRecord->m_fAttGap;
}

int CAnimus::GetWeaponClass()
{
  return m_byRoleCode != 1 ? 1 : 0;
}

float CAnimus::GetWidth()
{
  return static_cast<float>(m_pRecord->m_nWidth);
}

int CAnimus::GetWindTol()
{
  return static_cast<int>(m_pRecord->m_nWindTol);
}

bool CAnimus::IsInTown()
{
  return m_byPosRaceTown != 255;
}

bool CAnimus::IsBeAttackedAble(bool bFirst)
{
// this is not a stub
  return true;
}

void CAnimus::Loop()
{
  if (m_dwStunTime != 0)
  {
    const unsigned int elapsed = GetLoopTime() - m_dwStunTime;
    if (elapsed > static_cast<unsigned int>(m_pRecord->m_nCrtMoTime + 1000))
      m_dwStunTime = 0;
  }

  if (!m_dwStunTime)
  {
    Process();
    CheckPosInTown();
  }

  if (m_pRecord->m_nUseFP > 0 && m_tmNextEatMasterFP <= GetLoopTime())
  {
    _ProcComsumeMaterFP();
  }
}

void CAnimus::OutOfSec()
{
  Destroy();
}

void CAnimus::SendMsg_FixPosition(int n)
{
  _animus_fixpositon_zocl packet{};
  packet.wRecIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  packet.wIndex = m_ObjID.m_wIndex;
  packet.dwSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, packet.zCur, 3);
  packet.byLv = m_pRecord != nullptr ? static_cast<char>(m_pRecord->m_nLevel) : 1;
  packet.wLastEffectCode = m_wLastContEffect;
  packet.dwMasterSerial = m_pMaster->m_dwObjSerial;

  unsigned __int8 packetType[2] = {4, 13};
  g_Network.m_pProcess[0]->LoadSendMsg(
    n,
    packetType,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CAnimus::SendMsg_RealMovePoint(int n)
{
  _animus_real_move_zocl packet{};
  packet.wRecIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  packet.wIndex = m_ObjID.m_wIndex;
  packet.dwSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, packet.zCur, 3);
  packet.zTar[0] = static_cast<short>(static_cast<int>(m_fTarPos[0]));
  packet.zTar[1] = static_cast<short>(static_cast<int>(m_fTarPos[2]));
  packet.byLv = m_pRecord != nullptr ? static_cast<char>(m_pRecord->m_nLevel) : 1;
  packet.wLastEffectCode = m_wLastContEffect;
  packet.dwMasterSerial = m_pMaster->m_dwObjSerial;
  packet.fSpeed = m_fMoveSpeed;

  unsigned __int8 packetType[2] = {4, 24};
  g_Network.m_pProcess[0]->LoadSendMsg(
    n,
    packetType,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

int CAnimus::SetDamage(
  int nDam,
  CCharacter *pDst,
  int nDstLv,
  bool bCrt,
  int nAttackType,
  unsigned int dwAttackSerial,
  bool bJadeReturn)
{
  (void)nAttackType;
  (void)dwAttackSerial;
  (void)bJadeReturn;
  (void)nDstLv;

  if (m_byRoleCode == 1)
  {
    CalcDefExp(pDst, nDam);
  }

  if (nDam > 1)
  {
    m_nHP -= nDam;
    if (m_nHP <= 0)
      m_nHP = 0;
    AlterHP_MasterReport();

    if (bCrt)
    {
      m_dwStunTime = GetLoopTime();
      if (m_bMove)
      {
        MoveBreak(m_fMoveSpeed);
        Stop();
      }
    }
  }

  if (m_nHP == 0)
  {
    AlterExp(static_cast<unsigned int>(static_cast<int>(-0.0f - m_pRecord->m_fPenalty)));
    Return_MasterRequest(1u);
  }

  return m_nHP;
}

void CAnimus::RecvKillMessage(CCharacter *pDier)
{
  if (m_pMaster)
  {
    m_pMaster->RecvKillMessage(pDier);
  }
}

bool CAnimus::RobbedHP(CCharacter *pDst, int nDecHP)
{
  if (nDecHP >= m_nHP)
  {
    return false;
  }

  SetDamage(nDecHP, pDst, static_cast<int>(pDst->GetLevel()), false, -1, 0, true);
  SendMsg_RobedHP(pDst, static_cast<unsigned __int16>(nDecHP));
  return true;
}

