#include "pch.h"

#include "WorldServerUtil.h"

#include "CTrap.h"

#include "CAttack.h"
#include "CBsp.h"
#include "CMapData.h"
#include "CNetProcess.h"
#include "CObjectList.h"
#include "CPlayer.h"
#include "CPlayerDB.h"
#include "GlobalObjects.h"
#include "pnt_rect.h"
#include "qry_case_addpvppoint.h"
#include "TrapItem_fld.h"
#include "trap_create_setdata.h"
#include "Packet/ZoneClientPacket.h"

#include <cstring>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <mmsystem.h>

namespace
{
char s_trapObjectName[256]{};
}

int CTrap::s_nLiveNum;
unsigned int CTrap::s_dwSerialCnt;

CTrap::CTrap()
{
  m_pMaster = nullptr;
}

CTrap::~CTrap() = default;

bool CTrap::Create(_trap_create_setdata *pData)
{
  if (!CCharacter::Create(pData))
  {
    return false;
  }

  const _TrapItem_fld *trapRecord = reinterpret_cast<const _TrapItem_fld *>(m_pRecordSet);
  m_nHP = static_cast<int>(trapRecord->m_fMaxHP);
  m_pMaster = pData->pMaster;
  m_dwMasterSerial = pData->pMaster->m_dwObjSerial;
  const char *charNameW = pData->pMaster->m_Param.GetCharNameW();
  std::strcpy(m_wszMasterName, charNameW);
  W2M(m_wszMasterName, m_aszMasterName, sizeof(m_aszMasterName));
  m_dMasterPvPPoint = pData->pMaster->m_Param.GetPvPPoint();
  m_byRaceCode = static_cast<unsigned __int8>(pData->pMaster->m_Param.GetRaceCode());
  m_dwObjSerial = static_cast<unsigned int>(GetNewSerial());
  m_dwStartMakeTime = GetLoopTime();
  m_bComplete = false;
  SetBreakTranspar(false);
  m_bBreakTransparBuffer = m_bBreakTranspar;
  m_nTrapMaxAttackPnt = pData->nTrapMaxAttackPnt;
  SendMsg_Create();
  ++s_nLiveNum;
  return true;
}

char CTrap::IsHaveEmpty()
{
  for (int index = 0; index < MAX_TRAP; ++index)
  {
    if (!g_Trap[index].m_bLive)
    {
      return 1;
    }
  }
  return 0;
}

__int64 CTrap::GetNewSerial()
{
  return s_dwSerialCnt++;
}

CTrap *CreateTrap(CMapData *pMap, unsigned __int16 wLayer, float *fPos, CPlayer *pMaster, int nTrapItemIndex)
{
  CTrap *trap = nullptr;
  for (int index = 0; index < MAX_TRAP; ++index)
  {
    if (!g_Trap[index].m_bLive)
    {
      trap = &g_Trap[index];
      break;
    }
  }

  if (!trap)
  {
    return nullptr;
  }

  _trap_create_setdata data{};
  data.m_pMap = pMap;
  data.m_nLayerIndex = wLayer;
  data.m_pRecordSet = g_Main.m_tblItemData[26].GetRecord(nTrapItemIndex);
  if (!data.m_pRecordSet)
  {
    return nullptr;
  }

  std::memcpy(data.m_fStartPos, fPos, sizeof(data.m_fStartPos));
  data.pMaster = pMaster;
  data.nTrapMaxAttackPnt = pMaster->m_nTrapMaxAttackPnt;
  if (trap->Create(&data))
  {
    return trap;
  }
  return nullptr;
}

__int64 CTrap::GetAttackDP()
{
  const _TrapItem_fld *recordSet = reinterpret_cast<const _TrapItem_fld *>(m_pRecordSet);
  return static_cast<unsigned int>(recordSet->m_nAttack_DP);
}

float CTrap::GetAttackRange()
{
  const _TrapItem_fld *recordSet = reinterpret_cast<const _TrapItem_fld *>(m_pRecordSet);
  return recordSet->m_fAttGap;
}

__int64 CTrap::GetDefFC(int nAttactPart, CCharacter *pAttChar, int *pnConvertPart)
{
  const _TrapItem_fld *recordSet = reinterpret_cast<const _TrapItem_fld *>(m_pRecordSet);
  return static_cast<unsigned int>(recordSet->m_nDefFc);
}

float CTrap::GetDefFacing(int nPart)
{
  const _TrapItem_fld *recordSet = reinterpret_cast<const _TrapItem_fld *>(m_pRecordSet);
  return recordSet->m_fDefFacing;
}

float CTrap::GetDefGap(int nPart)
{
  const _TrapItem_fld *recordSet = reinterpret_cast<const _TrapItem_fld *>(m_pRecordSet);
  return recordSet->m_fDefGap;
}

__int64 CTrap::GetDefSkill(bool bBackAttackDamage)
{
  const _TrapItem_fld *recordSet = reinterpret_cast<const _TrapItem_fld *>(m_pRecordSet);
  return static_cast<unsigned int>(recordSet->m_nDefSklUnit);
}

__int64 CTrap::GetFireTol()
{
  const _TrapItem_fld *recordSet = reinterpret_cast<const _TrapItem_fld *>(m_pRecordSet);
  return static_cast<unsigned int>(recordSet->m_nFireTol);
}

__int64 CTrap::GetGenAttackProb(CCharacter *pDst, int nPart, bool bBackAttack)
{
  const _TrapItem_fld *recordSet = reinterpret_cast<const _TrapItem_fld *>(m_pRecordSet);
  int hitRate = static_cast<int>(
    static_cast<double>(recordSet->m_nAttSklUnit)
    - (static_cast<float>(pDst->GetLevel()) * 10.0f + static_cast<float>(pDst->GetDefSkill(bBackAttack))) / 4.0f
    + 95.0f);
  hitRate -= static_cast<int>(pDst->GetAvoidRate());
  if (hitRate < 5)
    hitRate = 5;
  if (hitRate > 95)
    return 95;
  return static_cast<unsigned int>(hitRate);
}

__int64 CTrap::GetHP()
{
  return static_cast<unsigned int>(m_nHP);
}

__int64 CTrap::GetLevel()
{
  const _TrapItem_fld *recordSet = reinterpret_cast<const _TrapItem_fld *>(m_pRecordSet);
  return static_cast<unsigned int>(recordSet->m_nLevel);
}

__int64 CTrap::GetMaxHP()
{
  const _TrapItem_fld *recordSet = reinterpret_cast<const _TrapItem_fld *>(m_pRecordSet);
  return static_cast<unsigned int>(static_cast<int>(recordSet->m_fMaxHP));
}

char *CTrap::GetObjName()
{
  std::snprintf(
    s_trapObjectName,
    sizeof(s_trapObjectName),
    "[Trap][%d] >> %s (pos: %s {%d, %d, %d}) (master: %s)",
    static_cast<int>(GetObjRace()),
    reinterpret_cast<const _TrapItem_fld *>(m_pRecordSet)->m_strName,
    m_pCurMap->m_pMapSet->m_strCode,
    static_cast<int>(m_fCurPos[0]),
    static_cast<int>(m_fCurPos[1]),
    static_cast<int>(m_fCurPos[2]),
    m_aszMasterName);
  return s_trapObjectName;
}

__int64 CTrap::GetObjRace()
{
  return m_byRaceCode;
}

__int64 CTrap::AttackableHeight()
{
  return 50;
}

__int64 CTrap::GetSoilTol()
{
  const _TrapItem_fld *recordSet = reinterpret_cast<const _TrapItem_fld *>(m_pRecordSet);
  return static_cast<unsigned int>(recordSet->m_nSoilTol);
}

__int64 CTrap::GetWaterTol()
{
  const _TrapItem_fld *recordSet = reinterpret_cast<const _TrapItem_fld *>(m_pRecordSet);
  return static_cast<unsigned int>(recordSet->m_nWaterTol);
}

float CTrap::GetWeaponAdjust()
{
  return 1.0f;
}

__int64 CTrap::GetWeaponClass()
{
  return 1;
}

float CTrap::GetWidth()
{
  return 5.0f;
}

__int64 CTrap::GetWindTol()
{
  const _TrapItem_fld *recordSet = reinterpret_cast<const _TrapItem_fld *>(m_pRecordSet);
  return static_cast<unsigned int>(recordSet->m_nWindTol);
}

bool CTrap::IsBeAttackedAble(bool bFirst)
{
// this is not a stub
  return true;
}

bool CTrap::IsInTown()
{
// this is not a stub
  return false;
}

void CTrap::Init(_object_id *pID)
{
  CCharacter::Init(pID);
  m_dwLastDestroyTime = 0;
  m_dwMasterSerial = static_cast<unsigned int>(-1);
  m_nHP = 0;
  m_pMaster = nullptr;
  m_byRaceCode = static_cast<unsigned __int8>(-1);
  std::memset(m_wszMasterName, 0, sizeof(m_wszMasterName));
  std::memset(m_aszMasterName, 0, sizeof(m_aszMasterName));
  m_dMasterPvPPoint = 0.0;
  m_dwStartMakeTime = 0;
  m_bComplete = false;
  m_bBreakTransparBuffer = false;
  m_nTrapMaxAttackPnt = 0;
}

void CTrap::MasterNetClose(long double dPvPPoint)
{
  m_pMaster = nullptr;
  m_dMasterPvPPoint = dPvPPoint;
}

void CTrap::MasterReStart(CPlayer *pMaster)
{
  m_pMaster = pMaster;
  m_dMasterPvPPoint = pMaster->m_Param.GetPvPPoint();
}

void CTrap::Loop()
{
  const DWORD now = timeGetTime();

  if (m_bComplete)
  {
    const unsigned int elapsed = now - m_dwStartMakeTime;
    const _TrapItem_fld *recordSet = reinterpret_cast<const _TrapItem_fld *>(m_pRecordSet);
    const unsigned int trapLifetime = 60000u * static_cast<unsigned int>(recordSet->m_nUpkeepTimeMin);
    if (elapsed <= trapLifetime)
    {
      if (SearchNearEnemy())
      {
        Attack(this);
        Destroy(0);
      }
    }
    else
    {
      Destroy(3u);
    }
  }
  else
  {
    const unsigned int elapsed = now - m_dwStartMakeTime;
    const _TrapItem_fld *recordSet = reinterpret_cast<const _TrapItem_fld *>(m_pRecordSet);
    unsigned int buildMs = 1000u * static_cast<unsigned int>(recordSet->m_nExpTimeSec);
    if (!g_Main.IsReleaseServiceMode())
      buildMs = 10000;

    if (elapsed > buildMs)
    {
      m_bComplete = true;
      SendMsg_TrapCompleteInform();
      if (m_pMaster != nullptr)
        m_pMaster->Emb_CheckActForQuest(16, m_pRecordSet->m_strCode, 1u, false);
    }
  }

  CheckTranspar();
}

void CTrap::OutOfSec()
{
  Destroy(1u);
}

bool CTrap::Destroy(unsigned __int8 byDesType)
{
  m_dwLastDestroyTime = timeGetTime();
  if (m_pMaster && m_pMaster->m_dwObjSerial == m_dwMasterSerial)
  {
    m_pMaster->_TrapDestroy(this, byDesType);
  }
  SendMsg_Destroy(byDesType);
  m_pMaster = nullptr;
  m_dwObjSerial = static_cast<unsigned int>(-1);
  m_dwMasterSerial = static_cast<unsigned int>(-1);
  --s_nLiveNum;
  return CCharacter::Destroy();
}

void CTrap::Attack(CCharacter *pTarget)
{
  CAttack attack(this);
  _attack_param attackParam{};
  attackParam.pDst = pTarget;
  std::memcpy(attackParam.fArea, pTarget->m_fCurPos, sizeof(attackParam.fArea));
  if (pTarget)
  {
    attackParam.nPart = static_cast<int>(pTarget->GetAttackRandomPart());
  }
  else
  {
    attackParam.nPart = static_cast<int>(GetAttackRandomPart());
  }
  const _TrapItem_fld *recordSet = reinterpret_cast<const _TrapItem_fld *>(m_pRecordSet);
  attackParam.nTol = recordSet->m_nProperty;
  attackParam.nClass = 1;
  attackParam.nMinAF = recordSet->m_nGAMinAF;
  attackParam.nMaxAF = recordSet->m_nGAMaxAF;
  attackParam.nMinSel = recordSet->m_nGAMinSelProb;
  attackParam.nMaxSel = recordSet->m_nGAMaxSelProb;
  attackParam.nAttactType = 6;
  attackParam.nExtentRange = static_cast<int>(recordSet->m_fGADst);
  attackParam.nMaxAttackPnt = m_nTrapMaxAttackPnt;

  attack.AttackGen(&attackParam, false, false);
  if (attack.m_nDamagedObjNum > 0)
  {
    SendMsg_Attack(&attack);
  }

  for (int index = 0; index < attack.m_nDamagedObjNum; ++index)
  {
    CCharacter *damagedCharacter = attack.m_DamList[index].m_pChar;
    damagedCharacter->SetDamage(
      attack.m_DamList[index].m_nDamage,
      this,
      static_cast<int>(GetLevel()),
      attack.m_bIsCrtAtt,
      -1,
      0,
      true);
  }
}

CCharacter *CTrap::SearchNearEnemy()
{
  const _TrapItem_fld *recordSet = reinterpret_cast<const _TrapItem_fld *>(m_pRecordSet);
  const int attackRange = static_cast<int>(recordSet->m_fReactionDst);
  CCharacter *candidates[10]{};
  int candidateCount = 0;

  _pnt_rect rect{};
  m_pCurMap->GetRectInRadius(&rect, 1, static_cast<unsigned int>(GetCurSecNum()));
  for (int y = rect.nStarty; y <= rect.nEndy; ++y)
  {
    for (int x = rect.nStartx; x <= rect.nEndx; ++x)
    {
      const unsigned int secIndex = static_cast<unsigned int>(m_pCurMap->GetSecInfo()->m_nSecNumW * y + x);
      CObjectList *sectorPlayers = m_pCurMap->GetSectorListPlayer(m_wMapLayerIndex, secIndex);
      if (!sectorPlayers)
      {
        continue;
      }

      for (_object_list_point *node = sectorPlayers->m_Head.m_pNext; node != &sectorPlayers->m_Tail; node = node->m_pNext)
      {
        CCharacter *candidate = static_cast<CCharacter *>(node->m_pItem);
        if (!candidate->m_bCorpse)
        {
          const int candidateRace = static_cast<int>(candidate->GetObjRace());
          if (candidateRace != static_cast<int>(GetObjRace())
            && (candidate->IsAttackableInTown() || !candidate->IsInTown())
            && !candidate->GetStealth(true))
          {
            if (candidate->IsBeAttackedAble(true) && std::fabs(candidate->m_fCurPos[1] - m_fCurPos[1]) <= 100.0f)
            {
              const float maxDistance = static_cast<float>(attackRange) + candidate->GetWidth() / 2.0f;
              if (GetSqrt(candidate->m_fCurPos, m_fCurPos) <= maxDistance
                && candidate->m_ObjID.m_byID == 0
                && candidateCount < 10)
              {
                candidates[candidateCount++] = candidate;
              }
            }
          }
        }
      }
    }
  }

  if (candidateCount > 0)
  {
    const int randomStart = rand() % candidateCount;
    float tempPath[3]{};
    for (int offset = 0; offset < candidateCount; ++offset)
    {
      const int candidateIndex = (offset + randomStart) % candidateCount;
      if (m_pCurMap->m_Level.mBsp->CanYouGoThere(m_fCurPos, candidates[candidateIndex]->m_fCurPos, &tempPath))
      {
        return candidates[candidateIndex];
      }
    }
  }

  return nullptr;
}

void CTrap::CheckTranspar()
{
  if (m_bBreakTransparBuffer != m_bBreakTranspar)
  {
    m_bBreakTransparBuffer = m_bBreakTranspar;
    SendMsg_AlterTranspar(!m_bBreakTransparBuffer);
  }
}

void CTrap::RecvKillMessage(CCharacter *pDier)
{
  if (m_pMaster)
  {
    if (m_pMaster->m_bLive && m_pMaster->m_bOper && m_pMaster->m_dwObjSerial == m_dwMasterSerial)
    {
      m_pMaster->RecvKillMessage(pDier);
    }
    return;
  }

  if (!pDier || pDier->m_ObjID.m_byID != 0 || m_dwMasterSerial == static_cast<unsigned int>(-1))
  {
    return;
  }

  auto *dierPlayer = reinterpret_cast<CPlayer *>(pDier);
  const double dierPvpPoint = static_cast<double>(dierPlayer->m_Param.GetPvPPoint());
  const double ratioNumerator = dierPvpPoint + 10000.0;
  const double ratioDenominator = static_cast<double>(m_dMasterPvPPoint) + 10000.0;
  double addPoint = ratioNumerator / ratioDenominator * 500.0 + 0.5;
  if (addPoint > dierPvpPoint)
  {
    addPoint = dierPvpPoint;
  }
  if (addPoint < 1.0)
  {
    addPoint = 1.0;
  }
  if (addPoint > 100000000.0)
  {
    addPoint = 100000000.0;
  }

  _qry_case_addpvppoint query{};
  query.dwSerial = m_dwMasterSerial;
  query.dwPoint = static_cast<unsigned int>(addPoint);
  query.dwCashBag = static_cast<unsigned int>(addPoint);
  g_Main.PushDQSData(-1, nullptr, 13, reinterpret_cast<char *>(&query), static_cast<int>(query.size()));

  g_Main.m_logPvP.Write(
    "%s [ %d ] DST: [ %d ] type: logoff_inc  >> pvp : %.0f  last: %.0f",
    m_aszMasterName,
    m_dwMasterSerial,
    dierPlayer->m_dwObjSerial,
    addPoint,
    static_cast<double>(m_dMasterPvPPoint) + addPoint);
  dierPlayer->AlterPvPPoint(-addPoint, die_dec, m_dwMasterSerial);
}

void CTrap::SendMsg_FixPosition(int n)
{

  _trap_fixpositon_zocl msg{};
  msg.wRecIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, msg.zCur, 3);
  msg.dwMasterSerial = m_dwMasterSerial;
  msg.bTranspar = !m_bBreakTransparBuffer;
  msg.byRaceCode = m_byRaceCode;

  if (m_bComplete)
    msg.wCompLeftSec = 0;
  else
    msg.wCompLeftSec = static_cast<unsigned __int16>((timeGetTime() - m_dwStartMakeTime) / 1000);

  unsigned __int8 type[2] = {4, 168};
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CTrap::SendMsg_Destroy(unsigned __int8 byDesType)
{
  _trap_destroy_zocl payload{};
  payload.dwSerial = m_dwObjSerial;
  payload.byDestroyCode = static_cast<char>(byDesType);

  unsigned __int8 type[2] = {3, static_cast<unsigned __int8>(-48)};
  CircleReport(
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)),
    false);

  if (m_pMaster && m_pMaster->m_dwObjSerial == m_dwMasterSerial)
  {
    g_Network.m_pProcess[0]->LoadSendMsg(
      m_pMaster->m_ObjID.m_wIndex,
      type,
      reinterpret_cast<char *>(&payload),
      static_cast<unsigned __int16>(sizeof(payload)));
  }
}

void CTrap::SendMsg_Create()
{
  _trap_create_zocl payload{};
  payload.wRecIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  payload.wIndex = m_ObjID.m_wIndex;
  payload.dwSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, payload.zPos, 3);
  payload.dwMasterSerial = m_dwMasterSerial;
  payload.byRaceCode = static_cast<char>(m_byRaceCode);

  unsigned __int8 type[2] = {3, static_cast<unsigned __int8>(-43)};
  CircleReport(type, reinterpret_cast<char *>(&payload), static_cast<unsigned __int16>(sizeof(payload)), false);
}

void CTrap::SendMsg_Attack(CAttack *pAt)
{
  const unsigned __int8 listCount = pAt->m_nDamagedObjNum > 32 ? 0 : static_cast<unsigned __int8>(pAt->m_nDamagedObjNum);
  _attack_trap_inform_zocl payload{};
  payload.dwAtterSerial = m_dwObjSerial;
  payload.bCritical = pAt->m_bIsCrtAtt;
  payload.byListNum = static_cast<char>(listCount);

  for (unsigned __int8 index = 0; index < listCount; ++index)
  {
    payload.DamList[index].byDstID = static_cast<char>(pAt->m_DamList[index].m_pChar->m_ObjID.m_byID);
    payload.DamList[index].dwDstSerial = pAt->m_DamList[index].m_pChar->m_dwObjSerial;
    payload.DamList[index].wDamage = static_cast<unsigned __int16>(pAt->m_DamList[index].m_nDamage);
    payload.DamList[index].bActive = false;
    payload.DamList[index].wActiveDamage = 0;
  }

  const unsigned __int16 payloadLength = static_cast<unsigned __int16>(326 - 10 * (32 - listCount));
  unsigned __int8 type[2] = {5, static_cast<unsigned __int8>(-104)};
  CircleReport(type, reinterpret_cast<char *>(&payload), payloadLength, false);
}

void CTrap::SendMsg_TrapCompleteInform()
{
  _trap_complete_inform_zocl payload{};
  payload.dwTrapSerial = m_dwObjSerial;

  unsigned __int8 type[2] = {17, static_cast<unsigned __int8>(-25)};
  CircleReport(type, reinterpret_cast<char *>(&payload), static_cast<unsigned __int16>(sizeof(payload)), false);
}

void CTrap::SendMsg_AlterTranspar(bool bTranspar)
{
  _trap_alter_transpar_inform_zocl payload{};
  payload.dwTrapSerial = m_dwObjSerial;
  payload.bTranspar = bTranspar;

  unsigned __int8 type[2] = {4, 35};
  CircleReport(type, reinterpret_cast<char *>(&payload), static_cast<unsigned __int16>(sizeof(payload)), false);
}

__int64 CTrap::SetDamage(int nDam, CCharacter *pDst, int nDstLv)
{
  if (nDam > 1)
  {
    if (m_nHP - nDam <= 0)
    {
      m_nHP = 0;
    }
    else
    {
      m_nHP -= nDam;
    }
  }

  if (m_nHP)
  {
    SetBreakTranspar(true);
  }
  else
  {
    Attack(this);
    Destroy(1u);
  }

  return static_cast<unsigned int>(m_nHP);
}

__int64 CTrap::SetDamage(
  int nDam,
  CCharacter *pDst,
  int nDstLv,
  bool bCrt,
  int nAttackType,
  unsigned int dwAttackSerial,
  bool bJadeReturn)
{
return SetDamage(nDam, pDst, nDstLv);
}
