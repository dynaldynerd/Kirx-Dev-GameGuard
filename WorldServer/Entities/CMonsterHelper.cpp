#include "pch.h"

#include "CMonsterHelper.h"

#include "CMonster.h"
#include "CMonsterHierarchy.h"
#include "CPlayer.h"
#include "CMapData.h"
#include "CObjectList.h"
#include "CBsp.h"
#include "ObjectCreateSetData.h"
#include "pnt_rect.h"
#include "WorldServerUtil.h"

#include <cmath>
#include <cstring>

static void Normalize(float *vec)
{
  const float length = std::sqrt((vec[0] * vec[0]) + (vec[1] * vec[1]) + (vec[2] * vec[2]));
  if (length > 0.0f)
  {
    vec[0] = vec[0] / length;
    vec[1] = vec[1] / length;
    vec[2] = vec[2] / length;
  }
}

_NEAR_DATA::_NEAR_DATA()
{
  Init();
}

void _NEAR_DATA::Init()
{
  fLen = 0.0f;
  pChar = nullptr;
  bCanYouGoThere = 0;
}

int CMonsterHelper::CheckPreAttackRangeTargetAbleCharacter(CMonster *pMon, CGameObject *pTarget)
{
  const float dist = Get3DSqrt(pMon->m_fCurPos, pTarget->m_fCurPos);
  const float baseRate = (90.0f - dist) * 0.001f + pMon->m_pMonRec->m_fOffensiveRate;
  float successRate = baseRate;
  if (static_cast<double>(pMon->m_pMonRec->m_nPreAttRange) > dist)
  {
    successRate = 1.0f;
  }
  const float threshold = (1.0f - successRate) * 100.0f;
  return (threshold <= static_cast<float>(rand() % 100)) ? 1 : 0;
}

float CMonsterHelper::GetAngle(float *mon, float *plr)
{
  float dx = mon[0] - plr[0];
  float dz = mon[2] - plr[2];
  const float length = std::sqrt((dx * dx) + (dz * dz));
  if (length > 0.0f)
  {
    dx = dx / length;
    dz = dz / length;
  }

  const bool positiveZ = dz > 0.0f;
  const float dot = dx;
  float angle = std::acos(dot);
  if (!positiveZ)
  {
    angle = -0.0f - angle;
  }
  return angle;
}

void CMonsterHelper::GetDirection(float (*cur)[3], float (*tar)[3], float (*out)[3], float deg)
{
  const float angle = CMonsterHelper::GetAngle(reinterpret_cast<float *>(tar), reinterpret_cast<float *>(cur));
  const float dx = static_cast<float>(std::cos(angle)) * deg;
  const float dz = static_cast<float>(std::sin(angle)) * deg;
  (*out)[0] = (*cur)[0] + dx;
  (*out)[2] = (*cur)[2] + dz;
  (*out)[1] = (*cur)[1];
}

void CMonsterHelper::HierarcyHelpCast(CMonster *pMon)
{
  CMonster *parent = pMon->m_MonHierarcy.GetParent();
  const unsigned __int8 childKindCount = pMon->m_MonHierarcy.ChildKindCount();
  if (parent)
  {
    const int parentKindCount = parent->m_MonHierarcy.ChildKindCount();
    for (int kindIndex = 0; kindIndex < parentKindCount; ++kindIndex)
    {
      for (int index = 0;; ++index)
      {
        const unsigned int childCount = parent->m_MonHierarcy.GetChildCount(kindIndex);
        if (static_cast<unsigned int>(index) >= childCount)
        {
          break;
        }
        CMonster *child = parent->m_MonHierarcy.GetChild(kindIndex, index);
        if (child && child != pMon)
        {
          child->m_AI.SendExternMsg(1u, pMon, 0);
        }
      }
    }
    parent->m_AI.SendExternMsg(1u, pMon, 0);
  }
  else if (childKindCount)
  {
    for (int kindIndex = 0; kindIndex < childKindCount; ++kindIndex)
    {
      for (int index = 0;; ++index)
      {
        const unsigned int childCount = pMon->m_MonHierarcy.GetChildCount(kindIndex);
        if (static_cast<unsigned int>(index) >= childCount)
        {
          break;
        }
        CMonster *child = pMon->m_MonHierarcy.GetChild(kindIndex, index);
        if (child && child != pMon)
        {
          child->m_AI.SendExternMsg(1u, pMon, 0);
        }
      }
    }
  }
}

int CMonsterHelper::IsInSector(float *chkpos, float *src, float *dest, float angle, float radius, float *pfDist)
{
  float destPos[3];
  std::memcpy(destPos, dest, sizeof(destPos));
  if (src[0] == dest[0] && src[2] == dest[2])
  {
    destPos[2] = src[2] + 1.0f;
  }

  float targetDir[3]{
    chkpos[0] - src[0],
    chkpos[1] - src[1],
    chkpos[2] - src[2],
  };
  float dist = std::sqrt(
    (targetDir[0] * targetDir[0]) + (targetDir[1] * targetDir[1]) + (targetDir[2] * targetDir[2]));
  if (pfDist)
  {
    *pfDist = dist;
  }
  if (dist > radius)
  {
    return 0;
  }

  float lookDir[3]{
    destPos[0] - src[0],
    destPos[1] - src[1],
    destPos[2] - src[2],
  };
  Normalize(targetDir);
  Normalize(lookDir);
  const float dot = (targetDir[0] * lookDir[0]) + (targetDir[1] * lookDir[1]) + (targetDir[2] * lookDir[2]);
  const float degAngle = static_cast<float>(std::acos(dot)) * 360.0f / 6.283184f;
  return ((angle / 2.0f) > degAngle) ? 1 : 0;
}

CMonster *CMonsterHelper::SearchNearMonsterByDistance(CMonster *pMon, int dwDist)
{
  float nearest = static_cast<float>(dwDist);
  CGameObject *nearestObj = nullptr;
  if (!pMon)
  {
    return nullptr;
  }

  const int curSecNum = static_cast<int>(pMon->GetCurSecNum());
  _pnt_rect rect{};
  pMon->m_pCurMap->GetRectInRadius(&rect, 1, curSecNum);
  for (int y = rect.nStarty; y <= rect.nEndy; ++y)
  {
    for (int x = rect.nStartx; x <= rect.nEndx; ++x)
    {
      _sec_info *secInfo = pMon->m_pCurMap->GetSecInfo();
      const unsigned int secIndex = secInfo->m_nSecNumW * y + x;
      CObjectList *sectorList = pMon->m_pCurMap->GetSectorListObj(pMon->m_wMapLayerIndex, secIndex);
      if (!sectorList)
      {
        continue;
      }

      for (_object_list_point *node = sectorList->m_Head.m_pNext; node != &sectorList->m_Tail; node = node->m_pNext)
      {
        CGameObject *obj = node->m_pItem;
        _object_id *objId = &obj->m_ObjID;
        if (!obj->m_ObjID.m_byKind && objId->m_byID == 1 && objId->m_wIndex != pMon->m_ObjID.m_wIndex
            && std::abs(obj->m_fCurPos[1] - pMon->m_fCurPos[1]) <= 50.0f)
        {
          const float dist = GetSqrt(pMon->m_fCurPos, obj->m_fCurPos);
          if (nearest > dist)
          {
            nearest = dist;
            nearestObj = obj;
          }
        }
      }
    }
  }
  return reinterpret_cast<CMonster *>(nearestObj);
}

unsigned int CMonsterHelper::SearchNearMonster(
  CMonster *pMon,
  _NEAR_DATA *NearChar,
  unsigned int dwArSize,
  int bTargetIgnore)
{
  if (pMon->m_pMonRec->m_nRaceCode == 3)
  {
    return 0;
  }
  if (!pMon || !NearChar || !dwArSize)
  {
    return 0;
  }

  for (unsigned int j = 0; j < dwArSize; ++j)
  {
    NearChar[j].Init();
  }

  CCharacter *targetChar = pMon->m_pTargetChar;
  const float limitLen = pMon->m_pMonRec->m_fMRefExt;
  const int curSecNum = static_cast<int>(pMon->GetCurSecNum());
  _pnt_rect rect{};
  pMon->m_pCurMap->GetRectInRadius(&rect, 4, curSecNum);

  for (int y = rect.nStarty; y <= rect.nEndy; ++y)
  {
    for (int x = rect.nStartx; x <= rect.nEndx; ++x)
    {
      _sec_info *secInfo = pMon->m_pCurMap->GetSecInfo();
      const unsigned int secIndex = secInfo->m_nSecNumW * y + x;
      CObjectList *sectorList = pMon->m_pCurMap->GetSectorListObj(pMon->m_wMapLayerIndex, secIndex);
      if (!sectorList)
      {
        continue;
      }

      _object_list_point *node = sectorList->m_Head.m_pNext;
      while (node != &sectorList->m_Tail)
      {
        CMonster *mon = reinterpret_cast<CMonster *>(node->m_pItem);
        node = node->m_pNext;

        _object_id *objId = &mon->m_ObjID;
        if (!mon->m_ObjID.m_byKind && objId->m_byID == 1 && objId->m_wIndex != pMon->m_ObjID.m_wIndex
            && (bTargetIgnore || !mon->m_pTargetChar)
            && std::abs(mon->m_fCurPos[1] - pMon->m_fCurPos[1]) <= 50.0f)
        {
          if (!targetChar || mon->m_pMonRec->m_nRaceCode != targetChar->GetObjRace())
          {
            if (pMon->GetMob_AsistType() || pMon->GetMob_SubRace() == mon->GetMob_SubRace()
                || mon->GetMob_AsistType())
            {
              const float dx = std::abs(mon->m_fCurPos[0] - pMon->m_fCurPos[0]);
              const float dz = std::abs(mon->m_fCurPos[2] - pMon->m_fCurPos[2]);
              const float len = (dx <= dz) ? dz : dx;
              const int lenInt = static_cast<int>(len);
              if (static_cast<float>(lenInt) <= limitLen)
              {
                if (NearChar->pChar)
                {
                  for (unsigned int n = 0; n < dwArSize; ++n)
                  {
                    if (NearChar[n].fLen > static_cast<float>(lenInt))
                    {
                      const int moveCount = static_cast<int>(dwArSize - 1 - n);
                      if (moveCount > 0)
                      {
                        std::memcpy(&NearChar[n + 1], &NearChar[n], 24LL * moveCount);
                      }
                      NearChar[n].fLen = static_cast<float>(lenInt);
                      NearChar[n].pChar = mon;
                      break;
                    }
                  }
                }
                else
                {
                  NearChar->fLen = static_cast<float>(lenInt);
                  NearChar->pChar = mon;
                }
              }
            }
          }
        }
      }
    }
  }

  CCharacter *firstReachable = nullptr;
  unsigned int filledCount = 0;
  for (unsigned int i = 0; i < dwArSize && NearChar[i].pChar; ++i)
  {
    float scratch[3]{};
    const int canGo =
      pMon->m_pCurMap->m_Level.mBsp->CanYouGoThere(pMon->m_fCurPos, NearChar[i].pChar->m_fCurPos, &scratch);
    NearChar[i].bCanYouGoThere = canGo;
    if (NearChar[i].bCanYouGoThere && !firstReachable)
    {
      firstReachable = NearChar[i].pChar;
    }
    ++filledCount;
  }

  if (firstReachable)
  {
    pMon->m_LifeCicle = GetLoopTime();
  }
  return filledCount;
}

CPlayer *CMonsterHelper::SearchNearPlayer(CMonster *pMon, int nType)
{
  int type = nType;
  if (type < 0)
  {
    type = 0;
  }
  if (type >= 11)
  {
    type = 0;
  }

  CMapData *map = pMon->m_pCurMap;
  const float visualField = (pMon->GetVisualField() >= 300.0f) ? 300.0f : pMon->GetVisualField();
  const float radius = visualField;
  const int radiusCount = static_cast<int>((visualField / 100.0f) + 0.5f);
  const float angle = pMon->GetVisualAngle();

  _NEAR_DATA nearData[5]{};
  const int curSecNum = static_cast<int>(pMon->GetCurSecNum());
  _pnt_rect rect{};
  map->GetRectInRadius(&rect, radiusCount, curSecNum);

  for (int y = rect.nStarty; y <= rect.nEndy; ++y)
  {
    for (int x = rect.nStartx; x <= rect.nEndx; ++x)
    {
      _sec_info *secInfo = map->GetSecInfo();
      const unsigned int secIndex = secInfo->m_nSecNumW * y + x;
      CObjectList *sectorList = map->GetSectorListPlayer(pMon->m_wMapLayerIndex, secIndex);
      if (!sectorList)
      {
        continue;
      }

      _object_list_point *node = sectorList->m_Head.m_pNext;
      while (node != &sectorList->m_Tail)
      {
        CCharacter *candidate = reinterpret_cast<CCharacter *>(node->m_pItem);
        node = node->m_pNext;
        CPlayer *candidatePlayer = static_cast<CPlayer *>(candidate);

        if (!candidate->m_bCorpse && !candidate->GetStealth(true) && !candidate->m_ObjID.m_byID
            && !candidate->m_ObjID.m_byKind
            && !candidatePlayer->m_bInGuildBattle)
        {
          if (candidate->IsBeAttackedAble(true))
          {
            if (pMon->m_pMonRec->m_nRaceCode != candidate->GetObjRace())
            {
              float distanceValue = 0.0f;
              if (CMonsterHelper::CheckPreAttackRangeTargetAbleCharacter(pMon, candidate)
                  || CMonsterHelper::IsInSector(
                    candidate->m_fCurPos,
                    pMon->m_fCurPos,
                    pMon->m_fLookAtPos,
                    angle,
                    radius,
                    &distanceValue))
              {
                CCharacter *selected = candidate;
                switch (type)
                {
                  case 1:
                    if (candidatePlayer->m_byMoveType)
                    {
                      distanceValue = distanceValue - 100.0f;
                    }
                    else
                    {
                      selected = nullptr;
                    }
                    break;
                  case 2:
                    if (candidatePlayer->m_byModeType == 1)
                    {
                      distanceValue = distanceValue - 100.0f;
                    }
                    else
                    {
                      selected = nullptr;
                    }
                    break;
                  case 3:
                    distanceValue = static_cast<float>(candidate->GetHP());
                    break;
                  case 4:
                    distanceValue = -static_cast<float>(candidate->GetHP());
                    break;
                  case 5:
                    distanceValue = -static_cast<float>(10 * candidatePlayer->m_pmWpn.nGaMaxAF);
                    break;
                  case 6:
                    distanceValue = -static_cast<float>(candidate->GetLevel());
                    break;
                  case 7:
                    distanceValue = static_cast<float>(candidate->GetLevel());
                    break;
                  case 8:
                    if (candidate->GetObjRace())
                    {
                      selected = nullptr;
                    }
                    break;
                  case 9:
                    if (candidate->GetObjRace() != 1)
                    {
                      selected = nullptr;
                    }
                    break;
                  case 10:
                    if (candidate->GetObjRace() != 2)
                    {
                      selected = nullptr;
                    }
                    break;
                  default:
                    break;
                }

                if (selected)
                {
                  if (nearData[0].pChar)
                  {
                    for (int m = 0; m < 5; ++m)
                    {
                      if (nearData[m].fLen > distanceValue)
                      {
                        const int moveCount = 4 - m;
                        if (moveCount > 0)
                        {
                          std::memcpy(&nearData[m + 1], &nearData[m], 24LL * moveCount);
                        }
                        nearData[m].fLen = distanceValue;
                        nearData[m].pChar = selected;
                        break;
                      }
                    }
                  }
                  else
                  {
                    nearData[0].fLen = distanceValue;
                    nearData[0].pChar = selected;
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  CCharacter *selected = nullptr;
  for (int n = 0; n < 5 && nearData[n].pChar; ++n)
  {
    float scratch[3]{};
    if (map->m_Level.mBsp->CanYouGoThere(pMon->m_fCurPos, nearData[n].pChar->m_fCurPos, &scratch))
    {
      selected = nearData[n].pChar;
      break;
    }
  }

  if (selected)
  {
    pMon->m_LifeCicle = GetLoopTime();
  }
  return reinterpret_cast<CPlayer *>(selected);
}

int CMonsterHelper::SearchPatrolMovePos(CMonster *mon, float (*NewTar)[3])
{
  if (mon->IsRoateMonster())
  {
    return 0;
  }
  if (!mon->m_bMove)
  {
    CMonster *parent = mon->m_MonHierarcy.GetParent();
    for (int attempt = 0;; ++attempt)
    {
      if (attempt >= 5)
      {
        return 0;
      }

      if (!parent && mon->m_pDumPosition)
      {
        if (!mon->m_pCurMap->GetRandPosInDummy(mon->m_pDumPosition, reinterpret_cast<float *>(NewTar), true))
        {
          continue;
        }
      }
      else
      {
        const int baseAngle = (rand() % 2) ? 0 : 32767;
        const float angleValue = static_cast<float>((baseAngle + rand()) % 65535);
        const int range = mon->m_pMonRec->m_nMaxMoveDistance - mon->m_pMonRec->m_nMinMoveDistance;
        float moveDistance = static_cast<float>(mon->m_pMonRec->m_nMinMoveDistance);
        if (range > 0)
        {
          moveDistance = moveDistance + static_cast<float>(rand() % range);
        }
        if (moveDistance <= 2.0f)
        {
          return 0;
        }

        const double angle = 6.283185307 * angleValue / 65535.0;
        (*NewTar)[0] = mon->m_fCurPos[0] - static_cast<float>(std::sin(angle) * moveDistance);
        (*NewTar)[2] = mon->m_fCurPos[2] - static_cast<float>(std::cos(angle) * moveDistance);
        (*NewTar)[1] = mon->m_fCurPos[1];

        if (parent)
        {
          const int targetDist = static_cast<int>(Get3DSqrt(reinterpret_cast<float *>(NewTar), parent->m_fCurPos));
          const float parentLimit =
            static_cast<float>(mon->GetWidth() / 2.0) + 100.0f + static_cast<float>(parent->GetWidth() / 2.0);
          if (static_cast<float>(targetDist) > parentLimit)
          {
            const int curDist = static_cast<int>(Get3DSqrt(mon->m_fCurPos, parent->m_fCurPos));
            if (targetDist > curDist)
            {
              continue;
            }
          }
        }
      }

      float scratch[3]{};
      if (mon->m_pCurMap->m_Level.mBsp->CanYouGoThere(
            mon->m_fCurPos,
            reinterpret_cast<float *const>(NewTar),
            &scratch))
      {
        return 1;
      }

      if (attempt >= 4)
      {
        scratch[1] = mon->m_fCurPos[1];
        std::memcpy(NewTar, scratch, sizeof(float[3]));
        return 1;
      }
    }
  }
  return 0;
}

int CMonsterHelper::SearchTargetMovePos_MovingTarget(
  CMonster *pMon,
  CCharacter *pTargetCharacter,
  float (*tarPos)[3])
{
  const float range = pMon->GetAttackRange() * 0.69999999f;
  const double dist = GetSqrt(pTargetCharacter->m_fCurPos, pMon->m_fCurPos);
  if (range >= dist && range >= 1.0f)
  {
    return 0;
  }

  CMonsterHelper::GetDirection(
    reinterpret_cast<float (*)[3]>(pTargetCharacter->m_fCurPos),
    reinterpret_cast<float (*)[3]>(pTargetCharacter->m_fTarPos),
    tarPos,
    50.0f);
  return 1;
}

int CMonsterHelper::SearchTargetMovePos_StopTarget(
  CMonster *pMon,
  CCharacter *pTargetCharacter,
  float (*tarPos)[3])
{
  if (!pTargetCharacter || !pMon)
  {
    return 0;
  }

  float dist = pMon->GetAttackRange();
  dist = dist - (dist * 0.2f);
  const double space = Get3DSqrt(pTargetCharacter->m_fCurPos, pMon->m_fCurPos);
  if (dist >= space && dist >= 1.0f)
  {
    return 0;
  }

  const float angle = CMonsterHelper::GetAngle(pMon->m_fCurPos, pTargetCharacter->m_fCurPos);
  const float slots = 5.0f;
  int pos = static_cast<int>((angle / (2.0f * 3.1415926535f)) * slots + 0.5f) - 1;
  if (static_cast<unsigned int>(pos) > 4)
  {
    pos = 4;
  }

  pTargetCharacter->RemoveSlot(pMon);
  const int nearSlot = static_cast<int>(
    pTargetCharacter->GetNearEmptySlot(pos, dist, pMon->m_fCurPos, reinterpret_cast<float *>(tarPos)));
  if (nearSlot < 0)
  {
    std::memcpy(tarPos, pTargetCharacter->m_fCurPos, sizeof(float[3]));
    const int dx = static_cast<int>(std::cos(angle) * dist);
    const int dz = static_cast<int>(std::sin(angle) * dist);
    (*tarPos)[0] = (*tarPos)[0] + static_cast<float>(dx);
    (*tarPos)[2] = (*tarPos)[2] + static_cast<float>(dz);
  }
  else
  {
    if (!pTargetCharacter->InsertSlot(pMon, nearSlot))
    {
      return 0;
    }
    const float slotAngle = 2.0f * 3.1415926535f * static_cast<float>(nearSlot + 1) / slots;
    std::memcpy(tarPos, pTargetCharacter->m_fCurPos, sizeof(float[3]));
    const int dx = static_cast<int>(std::cos(slotAngle) * dist);
    const int dz = static_cast<int>(std::sin(slotAngle) * dist);
    (*tarPos)[0] = (*tarPos)[0] + static_cast<float>(dx);
    (*tarPos)[2] = (*tarPos)[2] + static_cast<float>(dz);

    float scratch[3]{};
    if (!pMon->m_pCurMap->m_Level.mBsp->CanYouGoThere(
          pMon->m_fCurPos,
          reinterpret_cast<float *const>(tarPos),
          &scratch))
    {
      std::memcpy(tarPos, pTargetCharacter->m_fCurPos, sizeof(float[3]));
      const int fallbackDx = static_cast<int>(std::cos(angle) * dist);
      const int fallbackDz = static_cast<int>(std::sin(angle) * dist);
      (*tarPos)[0] = (*tarPos)[0] + static_cast<float>(fallbackDx);
      (*tarPos)[2] = (*tarPos)[2] + static_cast<float>(fallbackDz);
    }
  }
  return 1;
}

void CMonsterHelper::TransPort(CMonster *mon, float *tarPos)
{
  _monster_create_setdata data;
  std::memcpy(data.m_fStartPos, tarPos, sizeof(data.m_fStartPos));
  data.m_nLayerIndex = mon->m_wMapLayerIndex;
  data.m_pMap = mon->m_pCurMap;
  data.m_pRecordSet = mon->m_pRecordSet;
  data.pActiveRec = mon->m_pActiveRec;
  data.bDungeon = mon->m_bDungeon;
  data.pDumPosition = mon->m_pDumPosition;
  data.pParent = mon->m_MonHierarcy.GetParent();
  const int hp = static_cast<int>(mon->GetHP());
  data.bRobExp = mon->m_bRobExp;
  mon->Destroy(1u, nullptr);
  mon->Create(&data);
  mon->SetHP(hp, true);
}
