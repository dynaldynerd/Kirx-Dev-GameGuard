#include "pch.h"

#include "CHolyKeeper.h"

#include <new>
#include <mmsystem.h>
#include <cmath>

#include "CAttack.h"
#include "CHolyStoneSystem.h"
#include "CNetProcess.h"
#include "CObjectList.h"
#include "CPlayer.h"
#include "CPlayerDB.h"
#include "CEventLootTable.h"
#include "CItemLootTable.h"
#include "CItemBox.h"
#include "CMapData.h"
#include "CMonster.h"
#include "GlobalObjects.h"
#include "TimeItem.h"
#include "WorldServerUtil.h"

namespace
{
char s_keeperObjectName[256]{};
float s_attackPivot[3]{};
}

int CHolyKeeper::s_nLiveNum = -1;
unsigned int CHolyKeeper::s_dwSerialCnt = 0xFFFFFFFFu;

CHolyKeeper::CHolyKeeper()
  : m_nHP(0),
    m_dwLastDestroyTime(0),
    m_pRec(nullptr),
    m_pPosCreate(nullptr),
    m_pPosActive(nullptr),
    m_pPosCenter(nullptr),
    m_nMasterRace(-1),
    m_bExit(false),
    m_bChaos(false),
    m_dwLastStopTime(static_cast<unsigned int>(-1)),
    m_pLastMoveTarget(nullptr),
    m_at(nullptr),
    m_ap{},
    m_bDamageAbleState(false),
    m_nCurrLootIndex(-1),
    m_nEndLootIndex(-1),
    m_nCurrDropIndex(0),
    m_wMagnifications(0),
    m_wRange(0),
    m_wDropCntOnce(0)
{
}

CHolyKeeper::~CHolyKeeper()
{
  if (m_at)
  {
    delete m_at;
    m_at = nullptr;
  }
}

__int64 CHolyKeeper::GetAttackDP()
{
  return static_cast<unsigned int>(m_pRec->m_nAttack_DP);
}

float CHolyKeeper::GetAttackRange()
{
  return m_pRec->m_fAttExt;
}

__int64 CHolyKeeper::GetDefFC(int nAttactPart, CCharacter *pAttChar, int *pnConvertPart)
{
if (nAttactPart == -1)
    return static_cast<unsigned int>(m_nDefPart[rand() % 5]);

  return static_cast<unsigned int>(m_nDefPart[nAttactPart]);
}

float CHolyKeeper::GetDefFacing(int nPart)
{
return m_pRec->m_fDefFacing;
}

float CHolyKeeper::GetDefGap(int nPart)
{
return m_pRec->m_fDefGap;
}

__int64 CHolyKeeper::GetDefSkill(bool bBackAttackDamage)
{
return static_cast<unsigned int>(static_cast<int>(m_pRec->m_fDefSklUnit));
}

__int64 CHolyKeeper::GetFireTol()
{
  return static_cast<unsigned int>(static_cast<int>(m_pRec->m_fFireTol));
}

__int64 CHolyKeeper::GetGenAttackProb(CCharacter *pDst, int nPart, bool bBackAttack)
{
int hitRate = static_cast<int>(
    (m_pRec->m_fAttSklUnit - (static_cast<float>(pDst->GetLevel()) + static_cast<float>(pDst->GetDefSkill(bBackAttack)))) / 4.0f
    + 95.0f);
  hitRate -= static_cast<int>(pDst->GetAvoidRate());

  if (pDst->m_bMove)
    hitRate = static_cast<int>(static_cast<float>(hitRate) * 0.5f);

  if (hitRate < 5)
    return 5;
  if (hitRate > 95)
    return 95;
  return static_cast<unsigned int>(hitRate);
}

__int64 CHolyKeeper::GetHP()
{
  return static_cast<unsigned int>(m_nHP);
}

__int64 CHolyKeeper::GetLevel()
{
  return static_cast<unsigned int>(static_cast<int>(m_pRec->m_fLevel));
}

__int64 CHolyKeeper::GetMaxHP()
{
  return static_cast<unsigned int>(static_cast<int>(m_pRec->m_fMaxHP));
}

char *CHolyKeeper::GetObjName()
{
  std::snprintf(
    s_keeperObjectName,
    sizeof(s_keeperObjectName),
    "[KEEPER][%d] >> %s (pos: %s {%d, %d, %d})",
    static_cast<int>(GetObjRace()),
    m_pRec->m_strName,
    m_pCurMap->m_pMapSet->m_strCode,
    static_cast<int>(m_fCurPos[0]),
    static_cast<int>(m_fCurPos[1]),
    static_cast<int>(m_fCurPos[2]));
  return s_keeperObjectName;
}

__int64 CHolyKeeper::GetObjRace()
{
  if (m_nMasterRace == -1)
    return 16;

  return static_cast<unsigned int>(m_nMasterRace);
}

__int64 CHolyKeeper::GetSoilTol()
{
  return static_cast<unsigned int>(static_cast<int>(m_pRec->m_fSoilTol));
}

__int64 CHolyKeeper::GetWaterTol()
{
  return static_cast<unsigned int>(static_cast<int>(m_pRec->m_fWaterTol));
}

float CHolyKeeper::GetWeaponAdjust()
{
  return m_pRec->m_fDefGap;
}

__int64 CHolyKeeper::GetWeaponClass()
{
  return 1;
}

float CHolyKeeper::GetWidth()
{
  return m_pRec->m_fWidth;
}

__int64 CHolyKeeper::GetWindTol()
{
  return static_cast<unsigned int>(static_cast<int>(m_pRec->m_fWindTol));
}

bool CHolyKeeper::IsBeAttackedAble(bool bFirst)
{
// this is not a stub
  return true;
}

char CHolyKeeper::IsBeDamagedAble(CCharacter *pAtter)
{
  if (pAtter->m_ObjID.m_byID != 0)
    return true;

  CPlayer *player = static_cast<CPlayer *>(pAtter);
  return static_cast<char>(!g_Main.IsReleaseServiceMode() || player->m_byUserDgr == 0);
}

void CHolyKeeper::Init(_object_id *pID)
{
  CCharacter::Init(pID);
  m_dwLastDestroyTime = 0;
  m_nMasterRace = -1;
  m_pPosCreate = nullptr;
  m_pPosActive = nullptr;
  m_pPosCenter = nullptr;
  m_nHP = 0;
  m_pRec = nullptr;
  m_bExit = false;
  m_bChaos = false;
  m_dwLastStopTime = static_cast<unsigned int>(-1);
  for (int j = 0; j < 5; ++j)
  {
    m_nDefPart[j] = 0;
  }
  m_pLastMoveTarget = nullptr;
  m_at = nullptr;
  m_bDamageAbleState = false;
  m_nCurrLootIndex = -1;
  m_nEndLootIndex = -1;
  m_nCurrDropIndex = 0;
  m_wMagnifications = 0;
  m_wRange = 0;
  m_wDropCntOnce = 0;
}

bool CHolyKeeper::Create(_keeper_create_setdata *pData, int nCreateType)
{
  if (!CCharacter::Create(pData))
  {
    return false;
  }

  if (nCreateType == 0)
  {
    m_fCurPos[2] = m_fCurPos[2] - 300.0f;
  }

  m_pRec = reinterpret_cast<_monster_fld *>(m_pRecordSet);
  m_pRec->m_fAttExt = m_pRec->m_fAttExt * 2.0f;
  m_pPosCreate = pData->pPosCreate;
  m_pPosActive = pData->pPosActive;
  m_pPosCenter = pData->pPosCenter;

  const float xRange = m_pPosActive->m_fMax[0] - m_pPosActive->m_fMin[0];
  const float zRange = m_pPosActive->m_fMax[2] - m_pPosActive->m_fMin[2];
  const float activeRange = (xRange <= zRange) ? zRange : xRange;
  m_pRec->m_fAttExt = activeRange / 2.0f;

  m_nHP = static_cast<int>(m_pRec->m_fMaxHP);
  m_nMasterRace = pData->nMasterRace;
  m_bExit = false;
  m_bChaos = false;
  m_bDamageAbleState = false;
  m_dwLastStopTime = static_cast<unsigned int>(-1);
  m_pLastMoveTarget = nullptr;
  m_dwNextGenAttackTime = GetLoopTime();
  m_dwObjSerial = s_dwSerialCnt++;

  if (!m_at)
  {
    m_at = new (std::nothrow) CAttack(this);
  }

  m_ap.nTol = -1;
  m_ap.nClass = 1;
  m_ap.nAttactType = 6;
  const int minBase = static_cast<int>(m_pRec->m_fAttFcStd * 0.75f);
  if (m_pRec->m_fAttFcStd - static_cast<float>(minBase) <= 0.0f)
  {
    m_ap.nMinAF = 0;
  }
  else
  {
    const int span = static_cast<int>(m_pRec->m_fAttFcStd - static_cast<float>(minBase));
    m_ap.nMinAF = rand() % span + minBase;
  }
  m_ap.nMaxAF = static_cast<int>((m_pRec->m_fAttFcStd * 2.0f) - static_cast<float>(m_ap.nMinAF));
  m_ap.nMinSel = static_cast<int>(m_pRec->m_fMinAFSelProb);
  m_ap.nMaxSel = static_cast<int>(m_pRec->m_fMaxAFSelProb);
  m_ap.nExtentRange = 100;

  SetMaxVersion();
  for (int j = 0; j < 5; ++j)
  {
    m_nDefPart[j] = static_cast<int>(m_pRec->m_fStdDefFc);
  }
  SendMsg_Create();
  ++s_nLiveNum;
  return true;
}

bool CHolyKeeper::Destroy(unsigned __int8 byDestroyCode, CCharacter * /*pAtter*/)
{
  m_dwLastDestroyTime = timeGetTime();
  m_nMasterRace = -1;
  m_pPosCreate = nullptr;
  m_pPosActive = nullptr;
  m_pPosCenter = nullptr;
  m_bExit = false;
  SendMsg_Destroy(byDestroyCode);
  if (byDestroyCode)
  {
    m_dwObjSerial = static_cast<unsigned int>(-1);
    --s_nLiveNum;
    return CCharacter::Destroy();
  }

  SetDropItem();
  DropItem();
  return true;
}

bool CHolyKeeper::Exit()
{
  if (m_bExit)
  {
    return true;
  }
  if (!m_pPosCreate)
  {
    return false;
  }

  m_bExit = true;
  if (SetTarPos(m_pPosCreate->m_fCenterPos, false))
  {
    SendMsg_Move();
  }
  return true;
}

void CHolyKeeper::SetMaxHP(int nMaxHP)
{
  m_pRec->m_fMaxHP = static_cast<float>(nMaxHP);
}

void CHolyKeeper::SetHP(int nHP)
{
  m_nHP = nHP;
}

void CHolyKeeper::SetDamageAbleState(bool bDamageAbState)
{
  m_bDamageAbleState = bDamageAbState;
}

void CHolyKeeper::SetStateChaos()
{
  if (!m_bChaos)
  {
    m_bChaos = true;
    m_nMasterRace = -1;
  }
}

void CHolyKeeper::SendMsg_Create()
{
  _keeper_create_zocl msg{};

  msg.wIndex = m_ObjID.m_wIndex;
  msg.wRecIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  msg.dwSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, msg.zPos, 3);
  unsigned __int8 pbyType[2] = {3, static_cast<unsigned __int8>(-44)};
  CircleReport(pbyType, reinterpret_cast<char *>(&msg), 14, 0);
}

void CHolyKeeper::SendMsg_Destroy(unsigned __int8 byDesType)
{
  _keeper_destroy_zocl msg{};

  msg.dwSerial = m_dwObjSerial;
  msg.byDestroyCode = byDesType;
  unsigned __int8 pbyType[2] = {3, static_cast<unsigned __int8>(-46)};
  CircleReport(pbyType, reinterpret_cast<char *>(&msg), 5, 0);
}

void CHolyKeeper::SendMsg_Move()
{
  _keeper_move_zocl msg{};

  msg.dwSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, msg.zCur, 3);
  msg.zTar[0] = static_cast<__int16>(static_cast<int>(m_fTarPos[0]));
  msg.zTar[1] = static_cast<__int16>(static_cast<int>(m_fTarPos[2]));
  msg.byMoveType = static_cast<char>(m_bExit);
  unsigned __int8 pbyType[2] = {4, static_cast<unsigned __int8>(-79)};
  CircleReport(pbyType, reinterpret_cast<char *>(&msg), 15, 0);
}

void CHolyKeeper::SetDropItem()
{
  m_nCurrLootIndex = CMonster::s_idxMonsterLoot[m_pRecordSet->m_dwIndex].nStartRecIndex;
  m_nEndLootIndex = CMonster::s_idxMonsterLoot[m_pRecordSet->m_dwIndex].nEndRecIndex;
  m_nCurrDropIndex = 0;
  CEventLootTable::_event_drop *record = g_Main.m_tblItemLoot.m_pTblEvent->GetRecord(m_pRecordSet->m_strCode);
  if (record)
  {
    m_wMagnifications = record->wMagnifications;
    m_wRange = record->wRange;
    m_wDropCntOnce = record->wDropCntOnce;
    m_tmrDropTime.BeginTimer(record->wDropDelay);
  }
  else
  {
    m_wMagnifications = 1;
    m_wRange = 400;
    m_wDropCntOnce = static_cast<unsigned __int16>(-1);
    m_tmrDropTime.BeginTimer(0);
  }
}

void CHolyKeeper::DropItem()
{
  int dropCount = 0;
  if (m_nCurrLootIndex == -1)
  {
    m_tmrDropTime.StopTimer();
    return;
  }

  while (m_nCurrLootIndex <= m_nEndLootIndex)
  {
    _base_fld *record = g_Main.m_tblItemLoot.m_tblLoot.GetRecord(m_nCurrLootIndex);
    if (record && *reinterpret_cast<int *>(&record[1].m_strCode[8]) > 0)
    {
      unsigned __int16 range = 400;
      unsigned __int16 dropNum = *reinterpret_cast<unsigned __int16 *>(&record[1].m_strCode[4]);
      dropNum = static_cast<unsigned __int16>(dropNum * m_wMagnifications);
      if (m_wRange)
      {
        range = m_wRange;
      }

      while (m_nCurrDropIndex < dropNum)
      {
        bool found = false;
        const int randHi = rand();
        const unsigned int rand32 = static_cast<unsigned int>(rand()) + (static_cast<unsigned int>(randHi) << 16);
        unsigned int probLimit = 0;
        if (m_pRecordSet)
        {
          probLimit = record[1].m_dwIndex;
          if (rand32 < probLimit)
          {
            CItemLootTable::_linker_code *linker = nullptr;
            int linkIndex = 0;
            for (int j = 0; j < 10; ++j)
            {
              linkIndex = rand() % *reinterpret_cast<int *>(&record[1].m_strCode[8]);
              linker = &g_Main.m_tblItemLoot.m_ppLinkCode[record->m_dwIndex][linkIndex];
              if (linker->bExist)
              {
                found = true;
                break;
              }
            }

            if (found)
            {
              float stdPos[3] = {0.0f, 0.0f, 0.0f};
              CMapData *map = m_pCurMap;
              if (map->GetRandPosVirtualDumExcludeStdRange(m_fCurPos, range, 0, stdPos))
              {
                unsigned int itemDurPoint = 0;
                if (IsOverLapItem(linker->byTableCode))
                {
                  itemDurPoint = 1;
                }
                else
                {
                  itemDurPoint = GetItemDurPoint(linker->byTableCode, linker->wItemIndex);
                }

                unsigned __int8 socketNum = GetDefItemUpgSocketNum(linker->byTableCode, linker->wItemIndex);
                unsigned __int8 useSockets = 0;
                if (socketNum)
                {
                  useSockets = static_cast<unsigned __int8>(rand() % socketNum + 1);
                }
                const unsigned int socketBits = GetBitAfterSetLimSocket(useSockets);

                _STORAGE_LIST::_db_con item{};
                item.m_byTableCode = linker->byTableCode;
                item.m_wItemIndex = linker->wItemIndex;
                item.m_dwDur = itemDurPoint;
                item.m_dwLv = socketBits;

                const _TimeItem_fld *timeRec = TimeItem::FindTimeRec(linker->byTableCode, linker->wItemIndex);
                if (timeRec && timeRec->m_nCheckType)
                {
                  __time32_t timeNow;
                  _time32(&timeNow);
                  item.m_byCsMethod = timeRec->m_nCheckType;
                  item.m_dwT = timeRec->m_nUseTime + timeNow;
                  item.m_dwLendRegdTime = timeNow;
                }

                if (CreateItemBox(&item, 6u, map, m_wMapLayerIndex, stdPos, false, nullptr, 0, 3u))
                {
                  ++dropCount;
                  g_Main.m_logEvent.Write(
                    "HolyKeeper Event Item >> %s, %d",
                    &record[1].m_strCode[8 * linkIndex + 12],
                    static_cast<unsigned int>(item.m_dwDur));
                }
                if (dropCount >= m_wDropCntOnce)
                {
                  return;
                }
              }
            }
          }
        }
        ++m_nCurrDropIndex;
      }
      m_nCurrDropIndex = 0;
    }
    ++m_nCurrLootIndex;
  }

  m_nCurrLootIndex = -1;
  m_tmrDropTime.StopTimer();
  m_dwObjSerial = static_cast<unsigned int>(-1);
  --s_nLiveNum;
  CCharacter::Destroy();
}

void CHolyKeeper::Loop()
{
  if (m_tmrDropTime.m_bOper)
  {
    if (m_tmrDropTime.CountingTimer())
      DropItem();
    return;
  }

  if (g_HolySys.GetSceneCode() != 3)
    CheckAttack();
  CheckExit();
}

void CHolyKeeper::OutOfSec()
{
  Destroy(1u, nullptr);
}

void CHolyKeeper::SendMsg_FixPosition(int n)
{

  _keeper_fixpositon_zocl msg{};
  msg.wRecIndex = static_cast<unsigned __int16>(m_pRec->m_dwIndex);
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, msg.zCur, 3);

  unsigned __int8 type[2] = {4, 167};
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CHolyKeeper::SendMsg_RealMovePoint(int n)
{

  _keeper_real_move_zocl msg{};
  msg.wRecIndex = static_cast<unsigned __int16>(m_pRec->m_dwIndex);
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, msg.zCur, 3);
  msg.zTar[0] = static_cast<__int16>(static_cast<int>(m_fTarPos[0]));
  msg.zTar[1] = static_cast<__int16>(static_cast<int>(m_fTarPos[2]));

  unsigned __int8 type[2] = {4, 125};
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

__int64 CHolyKeeper::SetDamage(int nDam, CCharacter *pDst, int nDstLv)
{
if (g_HolySys.GetSceneCode() != 3)
    return static_cast<unsigned int>(m_nHP);
  if (g_HolySys.GetHolyMasterRace() == pDst->GetObjRace())
    return static_cast<unsigned int>(m_nHP);

  if (nDam > 1)
  {
    m_nHP -= nDam;
    if (m_nHP < 0)
      m_nHP = 0;
  }

  if (m_nHP == 0)
    Destroy(0, nullptr);

  return static_cast<unsigned int>(m_nHP);
}

__int64 CHolyKeeper::SetDamage(
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

void CHolyKeeper::CheckAttack()
{
  if (m_bMove || m_bExit || m_nHP <= 0)
    return;

  const unsigned int now = GetLoopTime();
  if (now < m_dwNextGenAttackTime)
    return;

  const float speedFactor = m_bChaos ? 4.0f : 2.0f;
  m_dwNextGenAttackTime = static_cast<unsigned int>(static_cast<float>(m_dwNextGenAttackTime) + m_pRec->m_fAttSpd * speedFactor);

  CCharacter *target = SearchAttackTarget();
  if (target == nullptr)
    return;

  m_ap.pDst = target;
  memcpy_0(m_ap.fArea, target->m_fCurPos, sizeof(m_ap.fArea));
  m_ap.nPart = 0;

  m_at->AttackGen(&m_ap, false, false);
  for (int i = 0; i < m_at->m_nDamagedObjNum; ++i)
  {
    _be_damaged_char &damaged = m_at->m_DamList[i];
    damaged.m_pChar->SetDamage(
      damaged.m_nDamage,
      this,
      static_cast<int>(GetLevel()),
      false,
      -1,
      0,
      true);
  }

  SendMsg_Attack();
}

void CHolyKeeper::CheckExit()
{
  if (!m_bExit)
    return;

  bool shouldDestroy = false;
  if (m_bMove)
    shouldDestroy = m_pCurMap->m_Dummy.IsInBBox(m_pPosCreate->m_wLineIndex, m_fCurPos);
  else
    shouldDestroy = true;

  if (shouldDestroy)
  {
    Destroy(1u, nullptr);
    m_bExit = false;
  }
}

void CHolyKeeper::CheckMove()
{
  if (m_bMove)
    return;

  if (m_pLastMoveTarget == nullptr)
  {
    const unsigned int now = GetLoopTime();
    if (m_dwLastStopTime == static_cast<unsigned int>(-1))
    {
      m_dwLastStopTime = now;
      return;
    }
    if (m_pRec->m_fWaitTime >= static_cast<float>(static_cast<int>(now - m_dwLastStopTime)))
      return;
  }

  if (SearchAttackTarget() != nullptr)
  {
    m_dwLastStopTime = static_cast<unsigned int>(-1);
    return;
  }

  const int maxMoveDistance = m_pRec->m_nMaxMoveDistance;
  CPlayer *moveTarget = SearchMoveTarget();
  float targetPos[3]{m_fCurPos[0], m_fCurPos[1], m_fCurPos[2]};

  if (moveTarget != nullptr)
  {
    int range = static_cast<int>(m_pRec->m_fAttExt);
    if (m_bChaos)
      range /= 2;

    if (GetSqrt(m_fCurPos, moveTarget->m_fCurPos) <= static_cast<float>(range))
    {
      m_dwLastStopTime = static_cast<unsigned int>(-1);
      return;
    }

    const float angle = GetYAngle(m_fCurPos, moveTarget->m_fCurPos);
    const float rad = (6.283185307f * angle) / 65535.0f;
    targetPos[0] = m_fCurPos[0] - std::sin(rad) * static_cast<float>(2 * maxMoveDistance);
    targetPos[2] = m_fCurPos[2] - std::cos(rad) * static_cast<float>(2 * maxMoveDistance);
    if (!m_pCurMap->m_Dummy.IsInBBox(m_pPosActive->m_wLineIndex, targetPos))
      moveTarget = nullptr;
  }

  if (moveTarget == nullptr)
  {
    if (GetSqrt(m_fCurPos, m_pPosCreate->m_fCenterPos) <= static_cast<float>(maxMoveDistance))
    {
      m_dwLastStopTime = static_cast<unsigned int>(-1);
      return;
    }

    const float angle = GetYAngle(m_fCurPos, m_pPosCreate->m_fCenterPos);
    const float rad = (6.283185307f * angle) / 65535.0f;
    targetPos[0] = m_fCurPos[0] - std::sin(rad) * static_cast<float>(maxMoveDistance);
    targetPos[2] = m_fCurPos[2] - std::cos(rad) * static_cast<float>(maxMoveDistance);
  }

  if (SetTarPos(targetPos, false))
  {
    SendMsg_Move();
    m_pLastMoveTarget = moveTarget;
  }
  m_dwLastStopTime = static_cast<unsigned int>(-1);
}

void CHolyKeeper::CheckCurPos()
{
  if (m_pCurMap->m_Dummy.IsInBBox(m_pPosActive->m_wLineIndex, m_fCurPos))
    return;

  m_pLastMoveTarget = nullptr;
  memcpy_0(m_fCurPos, m_pPosCreate->m_fCenterPos, sizeof(m_fCurPos));
  Stop();
  g_Main.m_logSystemError.Write("Out of Area Keeper");
}

CPlayer *CHolyKeeper::SearchMoveTarget()
{
  CPlayer *candidates[MAX_PLAYER]{};
  int count = 0;

  for (int i = 0; i < MAX_PLAYER; ++i)
  {
    if (!m_bPlayerCircleList[i])
      continue;

    CPlayer *player = &g_Player[i];
    if (!player->m_bLive || player->m_bCorpse)
      continue;
    if (player->m_Param.GetRaceCode() == m_nMasterRace)
      continue;
    if (g_Main.IsReleaseServiceMode() && player->m_byUserDgr != 0)
      continue;
    if (player->GetStealth(1))
      continue;
    if (std::fabs(player->m_fCurPos[1] - m_fCurPos[1]) > 200.0f)
      continue;
    if (player->m_pCurMap != m_pCurMap)
      continue;
    if (player->GetCurSecNum() == static_cast<unsigned int>(-1))
      continue;
    if (!m_pCurMap->m_Dummy.IsInBBox(m_pPosActive->m_wLineIndex, player->m_fCurPos))
      continue;

    if (m_pLastMoveTarget == player)
      return player;

    candidates[count++] = player;
    if (count >= MAX_PLAYER)
      break;
  }

  m_pLastMoveTarget = nullptr;
  if (count == 0)
    return nullptr;

  return candidates[rand() % count];
}

CCharacter *CHolyKeeper::SearchAttackTarget()
{
  CCharacter *candidates[MAX_PLAYER]{};
  int count = 0;
  const float *attackPivot = GetAttackPivot();

  int attackRange = static_cast<int>(m_pRec->m_fAttExt);
  if (m_bChaos)
    attackRange /= 2;

  for (int i = 0; i < MAX_PLAYER; ++i)
  {
    if (!m_bPlayerCircleList[i])
      continue;

    CPlayer *player = &g_Player[i];
    if (!player->m_bLive || player->m_bCorpse)
      continue;
    if (player->m_Param.GetRaceCode() == m_nMasterRace)
      continue;
    if (g_Main.IsReleaseServiceMode() && player->m_byUserDgr != 0)
      continue;
    if (player->GetStealth(1))
      continue;
    if (std::fabs(player->m_fCurPos[1] - attackPivot[1]) > 200.0f)
      continue;
    if (player->m_pCurMap != m_pCurMap)
      continue;
    if (player->GetCurSecNum() == static_cast<unsigned int>(-1))
      continue;

    const float dx = std::fabs(player->m_fCurPos[0] - attackPivot[0]);
    const float dz = std::fabs(player->m_fCurPos[2] - attackPivot[2]);
    const int chebyshev = static_cast<int>((dx > dz) ? dx : dz);
    if (chebyshev > attackRange)
      continue;

    if (m_pLastMoveTarget == nullptr
        && !m_pCurMap->m_Dummy.IsInBBox(m_pPosActive->m_wLineIndex, player->m_fCurPos))
    {
      continue;
    }

    if (m_pLastMoveTarget == player)
      return player;

    candidates[count++] = player;
    if (count >= MAX_PLAYER)
      break;
  }

  if (count == 0)
    return nullptr;

  return candidates[rand() % count];
}

float *CHolyKeeper::GetAttackPivot()
{
  if (m_pRec == nullptr)
    return m_fCurPos;

  const float radius = (m_pRec->m_fAttExt * 4.0f) / 7.0f;
  const int slot = rand() % 8;
  const float rad = (6.283185307f * static_cast<float>(slot)) / 8.0f;
  s_attackPivot[0] = m_fCurPos[0] + std::cos(rad) * radius;
  s_attackPivot[2] = m_fCurPos[2] + std::sin(rad) * radius;
  s_attackPivot[1] = m_fCurPos[1];
  return s_attackPivot;
}

void CHolyKeeper::SendMsg_Attack()
{


  _attack_keeper_inform_zocl msg{};
  msg.dwAtterSerial = m_dwObjSerial;
  msg.bCritical = m_at->m_bIsCrtAtt;
  msg.byListNum = static_cast<unsigned __int8>(m_at->m_nDamagedObjNum);
  for (int i = 0; i < m_at->m_nDamagedObjNum; ++i)
  {
    msg.DamList[i].byDstID = m_at->m_DamList[i].m_pChar->m_ObjID.m_byID;
    msg.DamList[i].dwDstSerial = m_at->m_DamList[i].m_pChar->m_dwObjSerial;
    msg.DamList[i].wDamage = static_cast<unsigned __int16>(m_at->m_DamList[i].m_nDamage);
  }

  if (msg.byListNum > 32)
  {
    msg.byListNum = 0;
  }
  const int packetSize = 326 - 10 * (32 - msg.byListNum);
  const unsigned __int8 type[2] = {5, 151};
  CircleReport(const_cast<unsigned __int8 *>(type), reinterpret_cast<char *>(&msg), packetSize, false);
}
