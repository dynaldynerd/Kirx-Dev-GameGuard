#include "pch.h"

#include "CAnimus.h"
#include "ObjectCreateSetData.h"
#include "animus_fld.h"
#include "CAttack.h"
#include "CGameObject.h"
#include "CNetProcess.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "attack_gen_result_zocl.h"

#include <cstdio>
#include <cstring>
#include <cmath>
#include <mmsystem.h>

unsigned int CAnimus::s_dwSerialCnt = 0;
CRecordData CAnimus::s_tblParameter[8];
unsigned int CAnimus::MAX_EXP[8]{};
int CAnimus::s_nLiveNum = 0;

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
    if (!CAnimus::s_tblParameter[nAnimusClass].ReadRecord(kFiles[nAnimusClass], 0x1B8u, errMsg))
    {
      MyMessageBox("CAnimus::SetStaticMember", errMsg);
      return false;
    }

    _animus_fld *record = GetAnimusFldFromLv(nAnimusClass, 0x40u);
    if (record == nullptr)
    {
      MyMessageBox("CAnimus::SetStaticMember", "::GetAnimusFldFromLv(...) Fail!");
      return false;
    }

    CAnimus::MAX_EXP[nAnimusClass] = static_cast<unsigned int>(record->m_nForLvUpExp - 1);
  }
  return true;
}

void CAnimus::Init(_object_id *pID)
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
}

char CAnimus::Create(_animus_create_setdata *pData)
{
  if (!CCharacter::Create(pData))
  {
    return 0;
  }

  m_byClassCode = static_cast<unsigned __int8>(m_pRecordSet->m_dwIndex);
  m_nHP = pData->nHP;
  m_nFP = pData->nFP;
  m_dwExp = pData->dwExp;
  m_pMaster = pData->pMaster;
  m_dwObjSerial = GetNewMonSerial();
  m_byRoleCode = static_cast<unsigned __int8>(m_pRecordSet[3].m_strCode[56]);
  m_nMaxAttackPnt = pData->nMaxAttackPnt;
  m_dwMasterSerial = pData->pMaster->m_dwObjSerial;
  strcpy_0(m_wszMasterName, pData->pMaster->m_Param.GetCharNameW());
  W2M(m_wszMasterName, m_aszMasterName, 0x11u);
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
  ++CAnimus::s_nLiveNum;
  return 1;
}

unsigned int CAnimus::GetNewMonSerial()
{
  return s_dwSerialCnt++;
}

void CAnimus::AIInit()
{
  m_pRecord = GetAnimusFldFromExp(m_byClassCode, m_dwExp);
  if (!m_pRecord)
  {
    return;
  }

  m_nMaxHP = m_pRecord->m_nMaxHP;
  m_nMaxFP = m_pRecord->m_nMaxFP;
  m_dwAIMode = 1;
  m_pTarget = nullptr;
  m_Mightiness = 1.0f;

  m_AITimer[1].Init(0x64u);
  m_AITimer[0].Init(0x64u);
  m_AITimer[2].Init(0xEA60u);
  m_AITimer[2].Set(0);
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
    if (m_pMaster)
    {
      m_pMaster->AlterMode_Animus(static_cast<unsigned __int8>(m_dwAIMode));
    }
  }
}

void CAnimus::ChangeMode_MasterCommand(unsigned int nMode)
{
  ChangeMode(nMode);
}

char CAnimus::ChangeTarget_MasterCommand(CCharacter *pTarget)
{
  if (m_byRoleCode == 3)
  {
    if (pTarget->m_ObjID.m_byID)
    {
      return 0;
    }

    CPlayer *targetPlayer = static_cast<CPlayer *>(pTarget);
    if (targetPlayer->m_bInGuildBattle && m_pMaster->m_bInGuildBattle)
    {
      if (targetPlayer->m_byGuildBattleColorInx != m_pMaster->m_byGuildBattleColorInx)
      {
        return 0;
      }
    }
    else
    {
      if (targetPlayer->m_bInGuildBattle || m_pMaster->m_bInGuildBattle)
      {
        return 0;
      }
      if (static_cast<int>(GetObjRace()) != static_cast<int>(pTarget->GetObjRace()))
      {
        return 0;
      }
    }
  }
  else
  {
    if (IsInTown())
    {
      return 0;
    }
    if (!pTarget->IsAttackableInTown() && pTarget->IsInTown())
    {
      return 0;
    }
    if (!pTarget->IsBeDamagedAble(this))
    {
      return 0;
    }

    if (pTarget->m_ObjID.m_byID)
    {
      if (static_cast<int>(GetObjRace()) == static_cast<int>(pTarget->GetObjRace()))
      {
        return 0;
      }
    }
    else
    {
      CPlayer *targetPlayer = static_cast<CPlayer *>(pTarget);
      if (targetPlayer->m_bInGuildBattle && m_pMaster->m_bInGuildBattle)
      {
        if (targetPlayer->m_byGuildBattleColorInx == m_pMaster->m_byGuildBattleColorInx)
        {
          return 0;
        }
      }
      else
      {
        if (targetPlayer->m_bInGuildBattle || m_pMaster->m_bInGuildBattle)
        {
          return 0;
        }
        if (static_cast<int>(GetObjRace()) == static_cast<int>(pTarget->GetObjRace())
            && !m_pMaster->IsChaosMode()
            && !targetPlayer->IsPunished(1u, false))
        {
          return 0;
        }
      }
    }
  }

  ChangeMode(0);
  m_pTarget = pTarget;
  return 1;
}

void CAnimus::Return_MasterRequest(unsigned __int8 byReturnType)
{
  if (m_pMaster)
  {
    m_pMaster->Return_AnimusAsk(byReturnType);
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

char CAnimus::GetMoveTarget(CCharacter *target, float fMoveSpeed, unsigned __int8 byMoveMode)
{
  if (!m_AITimer[1].Check())
  {
    return 0;
  }

  if (!target)
  {
    return 0;
  }

  float src[3]{};
  std::memcpy(src, target->m_fCurPos, sizeof(src));
  if (target->m_bMove)
  {
    if ((!byMoveMode && target == m_pMaster)
        || std::abs(static_cast<int>(GetYAngle(m_fCurPos, target->m_fCurPos))
                    - static_cast<int>(GetYAngle(target->m_fCurPos, target->m_fTarPos))) < 0x4000)
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
      return 0;
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
          return 0;
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
      return 1;
    }
    return 0;
  }

  if (m_fMoveSpeed != fMoveSpeed && m_bMove)
  {
    m_fMoveSpeed = fMoveSpeed;
    SendMsg_Move();
  }

  return 0;
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

  const float moveSpeed = static_cast<float>(m_pRecord->m_nMovSpd);

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
      GetMoveTarget(m_pTarget, moveSpeed, 0);
    }
    else
    {
      GetMoveTarget(m_pMaster, moveSpeed, 0);
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

void CAnimus::MasterAttack_MasterInform(CCharacter *pDst)
{
  if (!m_pTarget)
  {
    m_pTarget = pDst;
  }
}

__int64 CAnimus::GetAttackDP()
{
  return static_cast<unsigned int>(m_pRecord->m_nAttack_DP);
}

float CAnimus::GetAttackRange()
{
  return static_cast<float>(m_pRecord->m_nAttExt);
}

__int64 CAnimus::GetDefFC(int nAttactPart, CCharacter *pAttChar, int *pnConvertPart)
{
int baseDefFc = m_pRecord->m_nStdDefFc;
  if (m_pMaster != nullptr && m_byRoleCode == 1)
  {
    baseDefFc = static_cast<int>(static_cast<float>(baseDefFc) * m_pMaster->m_EP.GetEff_Rate(30));
  }
  return static_cast<unsigned int>(baseDefFc);
}

float CAnimus::GetDefFacing(int nPart)
{
return m_pRecord->m_fDefFacing;
}

float CAnimus::GetDefGap(int nPart)
{
return m_pRecord->m_fDefGap;
}

__int64 CAnimus::GetDefSkill(bool bBackAttackDamage)
{
return static_cast<unsigned int>(m_pRecord->m_nDefSklUnit);
}

__int64 CAnimus::GetFireTol()
{
  return static_cast<unsigned int>(m_pRecord->m_nFireTol);
}

__int64 CAnimus::GetGenAttackProb(CCharacter *pDst, int nPart, bool bBackAttack)
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
  return static_cast<unsigned int>(probability);
}

__int64 CAnimus::GetHP()
{
  return static_cast<unsigned int>(m_nHP);
}

__int64 CAnimus::GetLevel()
{
  return static_cast<unsigned int>(m_pRecord->m_nLevel);
}

__int64 CAnimus::GetMaxHP()
{
  if (m_nMaxHP <= 1)
  {
    return 1;
  }
  return static_cast<unsigned int>(m_nMaxHP);
}

char *CAnimus::GetObjName()
{
  const int raceCode = static_cast<int>(GetObjRace());
  const int posX = static_cast<int>(m_fCurPos[0]);
  const int posY = static_cast<int>(m_fCurPos[1]);
  const int posZ = static_cast<int>(m_fCurPos[2]);
  sprintf(
    s_animusObjectName,
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

__int64 CAnimus::GetObjRace()
{
  if (m_pMaster != nullptr)
  {
    return m_pMaster->GetObjRace();
  }
  return static_cast<unsigned int>(-1);
}

__int64 CAnimus::GetSoilTol()
{
  return static_cast<unsigned int>(m_pRecord->m_nSoilTol);
}

__int64 CAnimus::GetWaterTol()
{
  return static_cast<unsigned int>(m_pRecord->m_nWaterTol);
}

float CAnimus::GetWeaponAdjust()
{
  return m_pRecord->m_fAttGap;
}

__int64 CAnimus::GetWeaponClass()
{
  return m_byRoleCode != 1;
}

float CAnimus::GetWidth()
{
  return static_cast<float>(m_pRecord->m_nWidth);
}

__int64 CAnimus::GetWindTol()
{
  return static_cast<unsigned int>(m_pRecord->m_nWindTol);
}

bool CAnimus::IsInTown()
{
  return m_byPosRaceTown != 0xFF;
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

  if (m_pRecord && m_pRecord->m_nUseFP > 0 && m_tmNextEatMasterFP <= GetLoopTime())
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

__int64 CAnimus::SetDamage(int nDam, CCharacter *pDst, int nDstLv, bool bCrt)
{
if (nDam > 1)
  {
    m_nHP -= nDam;
    if (m_nHP <= 0)
      m_nHP = 0;

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
    Destroy();
  }

  return static_cast<unsigned int>(m_nHP);
}
