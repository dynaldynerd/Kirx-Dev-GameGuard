#include "pch.h"

#include "CGuardTower.h"

#include "CAttack.h"
#include "CAnimus.h"
#include "CMonster.h"
#include "CObjectList.h"
#include "WorldServerUtil.h"
#include "GlobalObjects.h"
#include "CPlayerDB.h"
#include "pnt_rect.h"
#include "GuardTowerItem_fld.h"

#include <cmath>
#include <mmsystem.h>

int CGuardTower::s_nLiveNum;
unsigned int CGuardTower::s_dwSerialCnt;
unsigned int CGuardTower::s_dwOldTick_CheckTemp;
__TEMP_WAIT_TOWER CGuardTower::s_Temp[MAX_PLAYER];

CGuardTower::CGuardTower()
{
  m_pMasterTwr = nullptr;
}

CGuardTower::~CGuardTower()
{
}

__int64 CGuardTower::AttackableHeight()
{
  return 50;
}

__int64 CGuardTower::GetAttackDP()
{
  const _GuardTowerItem_fld *recordSet = reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet);
  return static_cast<unsigned int>(recordSet->m_nAttack_DP);
}

float CGuardTower::GetAttackRange()
{
  const _GuardTowerItem_fld *recordSet = reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet);
  return static_cast<float>(recordSet->m_nGADst);
}

__int64 CGuardTower::GetDefFC(int /*nAttactPart*/, CCharacter * /*pAttChar*/, int * /*pnConvertPart*/)
{
  const _GuardTowerItem_fld *recordSet = reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet);
  return static_cast<unsigned int>(recordSet->m_nDefFc);
}

float CGuardTower::GetDefFacing(int /*nPart*/)
{
  const _GuardTowerItem_fld *recordSet = reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet);
  return recordSet->m_fDefFacing;
}

float CGuardTower::GetDefGap(int /*nPart*/)
{
  const _GuardTowerItem_fld *recordSet = reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet);
  return recordSet->m_fDefGap;
}

__int64 CGuardTower::GetDefSkill(bool /*bBackAttackDamage*/)
{
  const _GuardTowerItem_fld *recordSet = reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet);
  return static_cast<unsigned int>(recordSet->m_nDefSklUnit);
}

__int64 CGuardTower::GetFireTol()
{
  const _GuardTowerItem_fld *recordSet = reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet);
  return static_cast<unsigned int>(recordSet->m_nFireTol);
}

__int64 CGuardTower::GetGenAttackProb(CCharacter *pDst, int /*nPart*/, bool bBackAttack)
{
  const _GuardTowerItem_fld *recordSet = reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet);
  const double attackProb = static_cast<double>(recordSet->m_nAttSklUnit);
  const float levelTerm = static_cast<float>(pDst->GetLevel()) * 10.0f;
  const int defSkill = static_cast<int>(pDst->GetDefSkill(bBackAttack));

  int hitRate = static_cast<int>(attackProb - (levelTerm + static_cast<float>(defSkill)) / 4.0f + 95.0f);
  hitRate -= static_cast<int>(pDst->GetAvoidRate());

  if (hitRate <= 5)
  {
    hitRate = 5;
  }
  if (hitRate >= 95)
  {
    return 95;
  }
  return static_cast<unsigned int>(hitRate);
}

__int64 CGuardTower::GetHP()
{
  return static_cast<unsigned int>(m_nHP);
}

__int64 CGuardTower::GetLevel()
{
  const _GuardTowerItem_fld *recordSet = reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet);
  return static_cast<unsigned int>(recordSet->m_nLevel);
}

__int64 CGuardTower::GetMaxHP()
{
  const _GuardTowerItem_fld *recordSet = reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet);
  return static_cast<unsigned int>(recordSet->m_nMaxHP);
}

char *CGuardTower::GetObjName()
{
  static char objectName[256];
  if (m_pMasterTwr)
  {
    const char *masterName = m_pMasterTwr->m_Param.GetCharNameA();
    std::snprintf(
      objectName,
      sizeof(objectName),
      "[TOWER][%d] >> %s (pos: %s {%d, %d, %d}) (master: %s)",
      static_cast<int>(GetObjRace()),
      reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet)->m_strName,
      m_pCurMap->m_pMapSet->m_strCode,
      static_cast<int>(m_fCurPos[0]),
      static_cast<int>(m_fCurPos[1]),
      static_cast<int>(m_fCurPos[2]),
      masterName);
  }
  else
  {
    std::snprintf(
      objectName,
      sizeof(objectName),
      "[SYSTEM TOWER][%d] >> %s (pos: %s {%d, %d, %d})",
      static_cast<int>(GetObjRace()),
      reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet)->m_strName,
      m_pCurMap->m_pMapSet->m_strCode,
      static_cast<int>(m_fCurPos[0]),
      static_cast<int>(m_fCurPos[1]),
      static_cast<int>(m_fCurPos[2]));
  }
  return objectName;
}

__int64 CGuardTower::GetObjRace()
{
  return m_byRaceCode;
}

__int64 CGuardTower::GetSoilTol()
{
  const _GuardTowerItem_fld *recordSet = reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet);
  return static_cast<unsigned int>(recordSet->m_nSoilTol);
}

__int64 CGuardTower::GetWaterTol()
{
  const _GuardTowerItem_fld *recordSet = reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet);
  return static_cast<unsigned int>(recordSet->m_nWaterTol);
}

float CGuardTower::GetWeaponAdjust()
{
  const _GuardTowerItem_fld *recordSet = reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet);
  return recordSet->m_fAttGap;
}

__int64 CGuardTower::GetWeaponClass()
{
  return 1;
}

float CGuardTower::GetWidth()
{
  const _GuardTowerItem_fld *recordSet = reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet);
  return static_cast<float>(recordSet->m_nWidth);
}

__int64 CGuardTower::GetWindTol()
{
  const _GuardTowerItem_fld *recordSet = reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet);
  return static_cast<unsigned int>(recordSet->m_nWindTol);
}

bool CGuardTower::IsBeAttackedAble(bool /*bFirst*/)
{
  return true;
}

char CGuardTower::IsBeDamagedAble(CCharacter *pAtter)
{
  const unsigned __int8 attackerId = pAtter->m_ObjID.m_byID;
  if (attackerId && attackerId != 3)
  {
    return 0;
  }

  CCharacter *validatedAttacker = nullptr;
  if (!attackerId)
  {
    validatedAttacker = pAtter;
  }
  else if (attackerId == 3)
  {
    validatedAttacker = static_cast<CAnimus *>(pAtter)->m_pMaster;
  }

  if (!validatedAttacker)
  {
    return 0;
  }

  float canMovePath[8]{};
  return static_cast<unsigned int>(
           pAtter->m_pCurMap->m_Level.mBsp->CanYouGoThere(
             pAtter->m_fCurPos,
             m_fCurPos,
             reinterpret_cast<float (*)[3]>(canMovePath)))
    != 0;
}

bool CGuardTower::IsValidTarget()
{
  if (!m_pTarget->m_bLive)
    return false;
  if (!m_pTarget->IsBeAttackedAble(true))
    return false;
  if (m_pTarget->m_bObserver)
    return false;
  if (m_pTarget->GetStealth(1))
    return false;

  const float targetDistance = GetSqrt(m_fCurPos, m_pTarget->m_fCurPos);
  const _GuardTowerItem_fld *recordSet = reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet);
  const float attackRange = static_cast<float>(recordSet->m_nGADst);
  if (targetDistance > attackRange + m_pTarget->GetWidth() / 2.0f)
    return false;

  if (!m_pTarget->IsAttackableInTown() && m_pTarget->IsInTown())
    return false;

  if (m_pTarget->GetObjRace() != 16 || m_pTarget->m_ObjID.m_byID != 1)
    return true;

  bool keepMonsterTarget = false;
  CCharacter *attackTarget = static_cast<CMonster *>(m_pTarget)->GetAttackTarget();
  if (attackTarget != nullptr)
  {
    if (!attackTarget->m_bLive || attackTarget->m_bCorpse)
      return false;

    if (attackTarget->m_ObjID.m_byID == 0 || attackTarget->m_ObjID.m_byID == 3)
    {
      if (attackTarget->GetObjRace() != GetObjRace())
        keepMonsterTarget = true;
    }

    if (m_pTarget == m_pMasterSetTarget)
      return true;
  }

  return keepMonsterTarget;
}

CCharacter *CGuardTower::SearchNearEnemy()
{
  if (m_pMasterSetTarget != nullptr)
    return m_pMasterSetTarget;

  const _GuardTowerItem_fld *recordSet = reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet);
  const int maxRange = recordSet->m_nGADst;

  CCharacter *priorityTargets[10]{};
  CCharacter *normalTargets[10]{};
  int priorityCount = 0;
  int normalCount = 0;

  _pnt_rect searchRect{};
  m_pCurMap->GetRectInRadius(&searchRect, 4, static_cast<unsigned int>(GetCurSecNum()));

  for (int y = searchRect.nStarty; y <= searchRect.nEndy; ++y)
  {
    for (int x = searchRect.nStartx; x <= searchRect.nEndx; ++x)
    {
      const unsigned int secIndex = static_cast<unsigned int>(m_pCurMap->GetSecInfo()->m_nSecNumW * y + x);
      CObjectList *sectorList = m_pCurMap->GetSectorListObj(m_wMapLayerIndex, secIndex);
      if (sectorList == nullptr)
        continue;

      for (_object_list_point *node = sectorList->m_Head.m_pNext; node != &sectorList->m_Tail; node = node->m_pNext)
      {
        CCharacter *candidate = static_cast<CCharacter *>(node->m_pItem);
        if (candidate == nullptr)
          continue;
        if (candidate->m_ObjID.m_byKind != 0 || candidate->m_bCorpse)
          continue;

        const int candidateRace = candidate->GetObjRace();
        if (candidateRace == GetObjRace())
          continue;
        if (!candidate->IsAttackableInTown() && candidate->IsInTown())
          continue;

        const unsigned __int8 candidateId = candidate->m_ObjID.m_byID;
        if (candidateId != 0 && candidateId != 3)
        {
          if (candidateId != 1 || priorityCount >= 10)
            continue;

          CCharacter *candidateAttackTarget = static_cast<CMonster *>(candidate)->GetAttackTarget();
          if (candidateAttackTarget == nullptr)
            continue;

          if (candidateAttackTarget->m_ObjID.m_byID == 0)
          {
            if (m_pMasterTwr != nullptr)
            {
              if (candidateAttackTarget != m_pMasterTwr)
                continue;
            }
            else if (candidateAttackTarget->GetObjRace() != GetObjRace())
            {
              continue;
            }
          }
        }

        if (candidate->GetStealth(1))
          continue;
        if (std::fabs(candidate->m_fCurPos[1] - m_fCurPos[1]) > 400.0f)
          continue;

        const float limitDistance = static_cast<float>(maxRange) + candidate->GetWidth() / 2.0f;
        if (GetSqrt(candidate->m_fCurPos, m_fCurPos) > limitDistance)
          continue;

        if (candidateId == 1)
        {
          priorityTargets[priorityCount++] = candidate;
        }
        else
        {
          if (candidateId == 0 && candidate->m_bObserver)
            continue;
          if (normalCount < 10)
            normalTargets[normalCount++] = candidate;
        }
      }
    }
  }

  float routePoints[7]{};
  if (normalCount > 0)
  {
    const int startIndex = rand() % normalCount;
    for (int offset = 0; offset < normalCount; ++offset)
    {
      const int index = (offset + startIndex) % normalCount;
      CCharacter *candidate = normalTargets[index];
      if (candidate != nullptr
          && m_pCurMap->m_Level.mBsp->CanYouGoThere(
            m_fCurPos,
            candidate->m_fCurPos,
            reinterpret_cast<float (*)[3]>(routePoints)))
      {
        return candidate;
      }
    }
  }

  if (priorityCount > 0)
  {
    const int startIndex = rand() % priorityCount;
    for (int offset = 0; offset < priorityCount; ++offset)
    {
      const int index = (offset + startIndex) % priorityCount;
      CCharacter *candidate = priorityTargets[index];
      if (candidate != nullptr
          && m_pCurMap->m_Level.mBsp->CanYouGoThere(
            m_fCurPos,
            candidate->m_fCurPos,
            reinterpret_cast<float (*)[3]>(routePoints)))
      {
        return candidate;
      }
    }
  }

  return nullptr;
}

void CGuardTower::Attack(CCharacter *pTarget)
{
  CAttack attack(this);

  _attack_param attackParam{};
  attackParam.pDst = pTarget;
  attackParam.nPart = (pTarget != nullptr) ? pTarget->GetAttackRandomPart() : GetAttackRandomPart();
  const _GuardTowerItem_fld *recordSet = reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet);
  attackParam.nTol = recordSet->m_nProperty;
  attackParam.nClass = 1;
  attackParam.nMinAF = recordSet->m_nGAMinAF;
  attackParam.nMaxAF = recordSet->m_nGAMaxAF;
  attackParam.nMinSel = recordSet->m_nGAMinSelProb;
  attackParam.nMaxSel = recordSet->m_nGAMaxSelProb;

  attack.AttackGen(&attackParam, false, false);

  float pathPoints[8]{};
  if (!m_pCurMap->m_Level.mBsp->CanYouGoThere(
        m_pTarget->m_fCurPos,
        m_fCurPos,
        reinterpret_cast<float (*)[3]>(pathPoints)))
  {
    return;
  }

  if (attack.m_nDamagedObjNum > 0)
    SendMsg_Attack(&attack);

  for (int i = 0; i < attack.m_nDamagedObjNum; ++i)
  {
    _be_damaged_char &damaged = attack.m_DamList[i];
    damaged.m_pChar->SetDamage(
      damaged.m_nDamage,
      this,
      static_cast<int>(GetLevel()),
      attack.m_bIsCrtAtt,
      -1,
      0,
      true);
  }
}

void CGuardTower::Loop()
{
  const unsigned int loopTime = GetLoopTime();
  if (m_bComplete)
  {
    if (m_pTarget != nullptr)
    {
      if (!IsValidTarget())
      {
        m_pMasterSetTarget = nullptr;
        m_pTarget = nullptr;
      }
    }
    else
    {
      m_pTarget = SearchNearEnemy();
      if (m_pTarget != nullptr && !IsValidTarget())
      {
        m_pMasterSetTarget = nullptr;
        m_pTarget = nullptr;
      }
    }

    if (m_pTarget != nullptr && loopTime > m_dwNextGenAttackTime)
    {
      Attack(m_pTarget);

      const _GuardTowerItem_fld *recordSet = reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet);
      const unsigned int attackDelay = static_cast<unsigned int>(recordSet->m_nGASpd);
      const unsigned int nextTick = attackDelay + loopTime;
      if (nextTick < loopTime)
        m_dwNextGenAttackTime = attackDelay;
      else
        m_dwNextGenAttackTime = nextTick;
    }
  }
  else
  {
    const unsigned int elapsed = loopTime - m_dwStartMakeTime;
    const _GuardTowerItem_fld *recordSet = reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet);
    unsigned int buildMs = 1000u * static_cast<unsigned int>(recordSet->m_nExpTime);
    if (m_bQuick)
      buildMs = 5000;

    if (elapsed > buildMs)
    {
      m_bComplete = true;
      SendMsg_TowerCompleteInform();

      if (m_pMasterTwr != nullptr)
        m_pMasterTwr->Emb_CheckActForQuest(16, m_pRecordSet->m_strCode, 1u, 0);
    }
  }
}

void CGuardTower::SendMsg_Attack(CAttack *pAt)
{

  _attack_tower_inform_zocl msg{};
  msg.dwAtterSerial = m_dwObjSerial;
  msg.byAttackPart = static_cast<char>(pAt->m_pp->nPart);
  msg.bCritical = pAt->m_bIsCrtAtt;
  msg.byDstID = pAt->m_DamList[0].m_pChar->m_ObjID.m_byID;
  msg.dwDstSerial = pAt->m_DamList[0].m_pChar->m_dwObjSerial;
  msg.wDamage = static_cast<unsigned __int16>(pAt->m_DamList[0].m_nDamage);

  unsigned __int8 type[2] = {5, 15};
  CircleReport(type, reinterpret_cast<char *>(&msg), sizeof(msg), false);
}

void CGuardTower::SendMsg_TowerCompleteInform()
{

  _tower_complete_inform_zocl msg{};
  msg.dwTowerObjSerial = m_dwObjSerial;
  msg.dwMasterSerial = m_dwMasterSerial;

  unsigned __int8 type[2] = {17, 23};
  CircleReport(type, reinterpret_cast<char *>(&msg), sizeof(msg), false);
}

void CGuardTower::OutOfSec()
{
  Destroy(1u, false);
}

void CGuardTower::SendMsg_FixPosition(int n)
{

  _tower_fixpositon_zocl msg{};
  msg.wRecIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, msg.zCur, 3);
  if (m_bComplete)
  {
    msg.wCompLeftSec = 0;
  }
  else
  {
    const DWORD elapsedMs = timeGetTime() - m_dwStartMakeTime;
    msg.wCompLeftSec = static_cast<unsigned __int16>(elapsedMs / 1000);
  }
  msg.dwMasterSerial = m_dwMasterSerial;

  unsigned __int8 type[2] = {4, 14};
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

__int64 CGuardTower::SetDamage(
  int nDam,
  CCharacter *pDst,
  int /*nDstLv*/,
  bool /*bCrt*/,
  int /*nAttackType*/,
  unsigned int /*dwAttackSerial*/,
  bool /*bJadeReturn*/)
{
  if (nDam > 1)
  {
    const int updatedHp = m_nHP - nDam;
    m_nHP = (updatedHp <= 0) ? 0 : updatedHp;
  }

  if (m_nHP != 0)
  {
    if (m_pMasterTwr)
    {
      m_pMasterTwr->SendMsg_AlterTowerHP(m_wItemSerial, m_nHP);
    }

    const _GuardTowerItem_fld *recordSet = reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet);
    if (GetSqrt(m_fCurPos, pDst->m_fCurPos) > static_cast<float>(recordSet->m_nGADst))
    {
      if (m_pTarget)
      {
        const int attackerLevel = static_cast<int>(pDst->GetLevel());
        const int currentTargetLevel = static_cast<int>(m_pTarget->GetLevel());
        if (attackerLevel < currentTargetLevel)
        {
          m_pTarget = pDst;
        }
      }
      else
      {
        m_pTarget = pDst;
      }
    }
  }
  else
  {
    Destroy(1u, false);
  }

  return static_cast<unsigned int>(m_nHP);
}

bool CGuardTower::RobbedHP(CCharacter *pDst, int nDecHP)
{
  if (nDecHP >= m_nHP)
  {
    return false;
  }

  SetDamage(nDecHP, pDst, static_cast<int>(pDst->GetLevel()), false, -1, 0, true);
  SendMsg_RobedHP(pDst, static_cast<unsigned __int16>(nDecHP));
  return true;
}

bool CGuardTower::Create(_tower_create_setdata *pData)
{
  if (!CCharacter::Create(pData))
  {
    return false;
  }

  m_byRaceCode = pData->byRaceCode;
  if (pData->pMaster)
  {
    m_pMasterTwr = pData->pMaster;
    m_dwMasterSerial = pData->pMaster->m_dwObjSerial;
    m_pItem = pData->pItem;
    m_wItemSerial = pData->pItem->m_wSerial;
    m_nHP = static_cast<int>(pData->pItem->m_dwDur);
    m_bSystemStruct = false;
  }
  else
  {
    m_pMasterTwr = nullptr;
    m_dwMasterSerial = static_cast<unsigned int>(-1);
    m_pItem = nullptr;
    m_wItemSerial = static_cast<unsigned __int16>(-1);
    m_nHP = reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet)->m_nMaxHP;
    m_bSystemStruct = true;
    m_nIniIndex = pData->nIniIndex;
  }

  m_dwObjSerial = static_cast<unsigned int>(GetNewSerial());
  m_dwStartMakeTime = timeGetTime();
  m_bComplete = false;
  m_bQuick = pData->bQuick;
  m_pTarget = nullptr;
  m_pMasterSetTarget = nullptr;
  SendMsg_Create();
  ++CGuardTower::s_nLiveNum;
  return true;
}

void CGuardTower::Init(_object_id *pID)
{
  CCharacter::Init(pID);
  m_dwLastDestroyTime = 0;
  m_bSystemStruct = false;
  m_nHP = 0;
  m_pMasterTwr = nullptr;
  m_dwMasterSerial = 0;
  m_byRaceCode = static_cast<unsigned __int8>(-1);
  m_pItem = nullptr;
  m_wItemSerial = static_cast<unsigned __int16>(-1);
  m_bSystemStruct = false;
  m_nIniIndex = -1;
  m_dwStartMakeTime = 0;
  m_bComplete = false;
  m_bQuick = false;
  m_pTarget = nullptr;
  m_pMasterSetTarget = nullptr;
}

bool CGuardTower::Destroy(unsigned __int8 byDesType, bool bSystemBack)
{
  m_dwLastDestroyTime = timeGetTime();
  if (m_pMasterTwr && byDesType == 1)
  {
    m_pMasterTwr->_TowerDestroy(this);
  }
  SendMsg_Destroy(byDesType);

  if (!m_bSystemStruct || bSystemBack)
  {
    m_pMasterTwr = nullptr;
    m_dwObjSerial = static_cast<unsigned int>(-1);
    --CGuardTower::s_nLiveNum;
    return CCharacter::Destroy();
  }

  m_nHP = reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet)->m_nMaxHP;
  m_dwStartMakeTime = timeGetTime();
  m_bComplete = false;
  m_pTarget = nullptr;
  m_pMasterSetTarget = nullptr;
  m_dwObjSerial = static_cast<unsigned int>(GetNewSerial());
  SendMsg_Create();
  return true;
}

char CGuardTower::IsHaveEmpty()
{
  for (int j = 0; j < MAX_TOWER; ++j)
  {
    if (!g_Tower[j].m_bLive)
    {
      return 1;
    }
  }
  return 0;
}

bool CGuardTower::IsInTown()
{
  // this is not a stub
  return false;
}

void CGuardTower::RecvKillMessage(CCharacter *pDier)
{
  if (m_pMasterTwr)
  {
    m_pMasterTwr->RecvKillMessage(pDier);
  }
}

void CGuardTower::OnLoop_Static()
{
  const DWORD currentTime = timeGetTime();
  if (currentTime - CGuardTower::s_dwOldTick_CheckTemp >= 0x2710)
  {
    CGuardTower::s_dwOldTick_CheckTemp = currentTime;
    for (int playerIndex = 0; playerIndex < MAX_PLAYER; ++playerIndex)
    {
      __TEMP_WAIT_TOWER *tempInfo = &CGuardTower::s_Temp[playerIndex];
      if (tempInfo->dwMasterSerial != static_cast<unsigned int>(-1) && currentTime - tempInfo->dwPushTime > 0x493E0)
      {
        tempInfo->dwMasterSerial = static_cast<unsigned int>(-1);
      }
    }
  }
}

void CGuardTower::SendMsg_Destroy(unsigned __int8 byDesType)
{

  _tower_destroy_zocl msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  msg.byDestroyCode = byDesType;
  msg.dwMasterSerial = m_dwMasterSerial;

  unsigned __int8 type[2] = {3, 27};
  CircleReport(type, reinterpret_cast<char *>(&msg), sizeof(msg), false);
}

void CGuardTower::SendMsg_Create()
{

  _tower_create_zocl msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.wRecIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  msg.dwSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, msg.zPos, 3);
  msg.dwMasterSerial = m_dwMasterSerial;

  unsigned __int8 type[2] = {3, 19};
  CircleReport(type, reinterpret_cast<char *>(&msg), sizeof(msg), false);
}

void CGuardTower::NotifyOwnerAttackInform(CCharacter *pDst)
{
  if (!pDst || m_pMasterSetTarget == pDst)
  {
    return;
  }

  if (std::fabs(pDst->m_fCurPos[1] - m_fCurPos[1]) > 400.0f)
  {
    return;
  }

  const _GuardTowerItem_fld *recordSet = reinterpret_cast<const _GuardTowerItem_fld *>(m_pRecordSet);
  const int attackRange = recordSet->m_nGADst;
  const float searchRadius = static_cast<float>(attackRange) + pDst->GetWidth() / 2.0f;
  if (GetSqrt(m_fCurPos, pDst->m_fCurPos) <= searchRadius)
  {
    m_pMasterSetTarget = pDst;
    m_pTarget = nullptr;
  }
}

__int64 CGuardTower::GetNewSerial()
{
  return s_dwSerialCnt++;
}

CGuardTower *CreateSystemTower(
  CMapData *pMap,
  unsigned __int16 wLayer,
  float *fPos,
  int nTowerIndex,
  unsigned __int8 byRaceCode,
  int nIniIndex)
{
  CGuardTower *tower = nullptr;
  for (int j = 0; j < MAX_TOWER; ++j)
  {
    if (!g_Tower[j].m_bLive)
    {
      tower = &g_Tower[j];
      break;
    }
  }

  if (!tower)
  {
    return nullptr;
  }

  _tower_create_setdata data;
  data.m_pMap = pMap;
  data.m_nLayerIndex = wLayer;
  data.m_pRecordSet = g_Main.m_tblItemData[25].GetRecord(nTowerIndex);
  if (!data.m_pRecordSet)
  {
    return nullptr;
  }
  memcpy_0(data.m_fStartPos, fPos, sizeof(data.m_fStartPos));
  data.pMaster = nullptr;
  data.byRaceCode = byRaceCode;
  data.pItem = nullptr;
  data.nIniIndex = nIniIndex;

  if (tower->Create(&data))
  {
    return tower;
  }
  return nullptr;
}

