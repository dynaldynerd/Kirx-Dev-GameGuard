#include "pch.h"

#include "CGuardTower.h"

#include "CAttack.h"
#include "CMonster.h"
#include "CObjectList.h"
#include "WorldServerUtil.h"
#include "GlobalObjects.h"
#include "CPlayerDB.h"
#include "pnt_rect.h"

#include <cmath>
#include <mmsystem.h>

int CGuardTower::s_nLiveNum = 0;
unsigned int CGuardTower::s_dwSerialCnt = 0;
unsigned int CGuardTower::s_dwOldTick_CheckTemp = 0;
__TEMP_WAIT_TOWER CGuardTower::s_Temp[MAX_PLAYER]{};

__int64 CGuardTower::AttackableHeight()
{
  return 50;
}

__int64 CGuardTower::GetAttackDP()
{
  return *reinterpret_cast<unsigned int *>(&m_pRecordSet[5].m_strCode[20]);
}

float CGuardTower::GetAttackRange()
{
  return static_cast<float>(*reinterpret_cast<int *>(&m_pRecordSet[5].m_strCode[24]));
}

__int64 CGuardTower::GetDefFC(int /*nAttactPart*/, CCharacter * /*pAttChar*/, int * /*pnConvertPart*/)
{
  return *reinterpret_cast<unsigned int *>(&m_pRecordSet[5].m_strCode[56]);
}

float CGuardTower::GetDefFacing(int /*nPart*/)
{
  return *reinterpret_cast<float *>(&m_pRecordSet[6].m_dwIndex);
}

float CGuardTower::GetDefGap(int /*nPart*/)
{
  return *reinterpret_cast<float *>(&m_pRecordSet[5].m_strCode[60]);
}

__int64 CGuardTower::GetDefSkill(bool /*bBackAttackDamage*/)
{
  return *reinterpret_cast<unsigned int *>(&m_pRecordSet[5].m_strCode[52]);
}

__int64 CGuardTower::GetFireTol()
{
  return *reinterpret_cast<unsigned int *>(&m_pRecordSet[6].m_strCode[4]);
}

__int64 CGuardTower::GetGenAttackProb(CCharacter *pDst, int /*nPart*/, bool bBackAttack)
{
  const double attackProb = static_cast<double>(*reinterpret_cast<int *>(&m_pRecordSet[5].m_strCode[32]));
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
  return *reinterpret_cast<unsigned int *>(&m_pRecordSet[5].m_strCode[4]);
}

__int64 CGuardTower::GetMaxHP()
{
  return *reinterpret_cast<unsigned int *>(&m_pRecordSet[6].m_strCode[40]);
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
      &m_pRecordSet[2].m_strCode[56],
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
      &m_pRecordSet[2].m_strCode[56],
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
  return *reinterpret_cast<unsigned int *>(&m_pRecordSet[6].m_strCode[12]);
}

__int64 CGuardTower::GetWaterTol()
{
  return *reinterpret_cast<unsigned int *>(&m_pRecordSet[6].m_strCode[8]);
}

float CGuardTower::GetWeaponAdjust()
{
  return *reinterpret_cast<float *>(&m_pRecordSet[5].m_strCode[16]);
}

__int64 CGuardTower::GetWeaponClass()
{
  return 1;
}

float CGuardTower::GetWidth()
{
  return static_cast<float>(*reinterpret_cast<int *>(&m_pRecordSet[5].m_strCode[12]));
}

__int64 CGuardTower::GetWindTol()
{
  return *reinterpret_cast<unsigned int *>(&m_pRecordSet[6].m_strCode[16]);
}

bool CGuardTower::IsBeAttackedAble(bool /*bFirst*/)
{
  return true;
}

char CGuardTower::IsBeDamagedAble(CCharacter *pAtter)
{
  if (pAtter->m_ObjID.m_byID != 0 && pAtter->m_ObjID.m_byID != 3)
  {
    return 0;
  }

  CCharacter *attackSource = nullptr;
  if (pAtter->m_ObjID.m_byID == 0)
  {
    attackSource = pAtter;
  }
  else
  {
    CGuardTower *attackerTower = static_cast<CGuardTower *>(pAtter);
    attackSource = attackerTower->m_pMasterTwr;
  }

  if (!attackSource)
  {
    return 0;
  }

  float resultPath[8]{};
  return static_cast<unsigned int>(
           attackSource->m_pCurMap->m_Level.mBsp->CanYouGoThere(
             attackSource->m_fCurPos,
             m_fCurPos,
             reinterpret_cast<float (*)[3]>(resultPath)))
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
  const float attackRange = static_cast<float>(*reinterpret_cast<int *>(&m_pRecordSet[5].m_strCode[24]));
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

  const int maxRange = *reinterpret_cast<int *>(&m_pRecordSet[5].m_strCode[24]);

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
  attackParam.nTol = *reinterpret_cast<int *>(m_pRecordSet[6].m_strCode);
  attackParam.nClass = 1;
  attackParam.nMinAF = *reinterpret_cast<int *>(&m_pRecordSet[5].m_strCode[36]);
  attackParam.nMaxAF = *reinterpret_cast<int *>(&m_pRecordSet[5].m_strCode[40]);
  attackParam.nMinSel = *reinterpret_cast<int *>(&m_pRecordSet[5].m_strCode[44]);
  attackParam.nMaxSel = *reinterpret_cast<int *>(&m_pRecordSet[5].m_strCode[48]);

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

      const unsigned int attackDelay = *reinterpret_cast<unsigned int *>(&m_pRecordSet[5].m_strCode[28]);
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
    unsigned int buildMs = 1000u * (*reinterpret_cast<unsigned int *>(&m_pRecordSet[4].m_strCode[60]));
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
#pragma pack(push, 1)
  struct GuardTowerAttackMsg
  {
    unsigned int dwTowerSerial;
    unsigned __int8 byPart;
    unsigned __int8 byCritical;
    unsigned __int8 byTargetId;
    unsigned int dwTargetSerial;
    __int16 nDamage;
  };
#pragma pack(pop)

  GuardTowerAttackMsg msg{};
  msg.dwTowerSerial = m_dwObjSerial;
  msg.byPart = static_cast<unsigned __int8>(pAt->m_pp->nPart);
  msg.byCritical = static_cast<unsigned __int8>(pAt->m_bIsCrtAtt);
  msg.byTargetId = pAt->m_DamList[0].m_pChar->m_ObjID.m_byID;
  msg.dwTargetSerial = pAt->m_DamList[0].m_pChar->m_dwObjSerial;
  msg.nDamage = static_cast<__int16>(pAt->m_DamList[0].m_nDamage);

  unsigned __int8 type[2] = {5, 15};
  CircleReport(type, reinterpret_cast<char *>(&msg), sizeof(msg), false);
}

void CGuardTower::SendMsg_TowerCompleteInform()
{
#pragma pack(push, 1)
  struct GuardTowerCompleteMsg
  {
    unsigned int dwTowerSerial;
    unsigned int dwMasterSerial;
  };
#pragma pack(pop)

  GuardTowerCompleteMsg msg{};
  msg.dwTowerSerial = m_dwObjSerial;
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
#pragma pack(push, 1)
  struct GuardTowerFixPositionMsg
  {
    unsigned __int16 wRecordIndex;
    unsigned __int16 wIndex;
    unsigned int dwObjSerial;
    __int16 pos[3];
    __int16 nBuildSec;
    unsigned int dwMasterSerial;
  };
#pragma pack(pop)

  GuardTowerFixPositionMsg msg{};
  msg.wRecordIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwObjSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, msg.pos, 3);
  if (m_bComplete)
  {
    msg.nBuildSec = 0;
  }
  else
  {
    const DWORD elapsedMs = timeGetTime() - m_dwStartMakeTime;
    msg.nBuildSec = static_cast<__int16>(elapsedMs / 1000);
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

    if (GetSqrt(m_fCurPos, pDst->m_fCurPos) > static_cast<float>(*reinterpret_cast<int *>(&m_pRecordSet[5].m_strCode[24])))
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

  m_nHP = *reinterpret_cast<int *>(&m_pRecordSet[6].m_strCode[40]);
  m_dwStartMakeTime = timeGetTime();
  m_bComplete = false;
  m_pTarget = nullptr;
  m_pMasterSetTarget = nullptr;
  m_dwObjSerial = static_cast<unsigned int>(GetNewSerial());
  SendMsg_Create();
  return true;
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
#pragma pack(push, 1)
  struct GuardTowerDestroyMsg
  {
    unsigned __int16 wIndex;
    unsigned int dwObjSerial;
    unsigned __int8 byDesType;
    unsigned int dwMasterSerial;
  };
#pragma pack(pop)

  GuardTowerDestroyMsg msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwObjSerial = m_dwObjSerial;
  msg.byDesType = byDesType;
  msg.dwMasterSerial = m_dwMasterSerial;

  unsigned __int8 type[2] = {3, 27};
  CircleReport(type, reinterpret_cast<char *>(&msg), sizeof(msg), false);
}

void CGuardTower::SendMsg_Create()
{
#pragma pack(push, 1)
  struct GuardTowerCreateMsg
  {
    unsigned __int16 wIndex;
    unsigned __int16 wRecordIndex;
    unsigned int dwObjSerial;
    short pos[3];
    unsigned int dwMasterSerial;
  };
#pragma pack(pop)

  GuardTowerCreateMsg msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.wRecordIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  msg.dwObjSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, msg.pos, 3);
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

  const int attackRange = *reinterpret_cast<int *>(&m_pRecordSet[5].m_strCode[24]);
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

