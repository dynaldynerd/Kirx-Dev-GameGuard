#include "pch.h"

#include "CTrap.h"

#include <cstring>
#include <cstdlib>

#include "CMapData.h"
#include "CObjectList.h"
#include "CPlayer.h"
#include "CPlayerDB.h"
#include "GlobalObjects.h"
#include "TrapItem_fld.h"
#include "WorldServerUtil.h"
#include "pnt_rect.h"

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

CTrap *CreateTrap(
  CMapData *pMap,
  unsigned __int16 wLayer,
  float *fPos,
  CPlayer *pMaster,
  int nTrapItemIndex)
{
  CTrap *trap = nullptr;
  for (int j = 0; j < MAX_TRAP; ++j)
  {
    if (!g_Trap[j].m_bLive)
    {
      trap = &g_Trap[j];
      break;
    }
  }
  if (!trap)
  {
    return nullptr;
  }

  _trap_create_setdata data;
  data.m_pMap = pMap;
  data.m_nLayerIndex = wLayer;
  data.m_pRecordSet = CRecordData::GetRecord(&g_Main.m_tblItemData[26], nTrapItemIndex);
  if (!data.m_pRecordSet)
  {
    return nullptr;
  }
  memcpy_0(data.m_fStartPos, fPos, sizeof(data.m_fStartPos));
  data.pMaster = pMaster;
  data.nTrapMaxAttackPnt = pMaster->m_nTrapMaxAttackPnt;
  if (trap->Create(&data))
  {
    return trap;
  }
  return nullptr;
}

unsigned __int8 IsOtherInvalidObjNear(
  CGameObject *pEster,
  float *pfEstPos,
  CTrap *pEstObj,
  _TrapItem_fld *pEstTrapItemInfo)
{
  const int currentSector = CGameObject::GetCurSecNum(pEster);
  _pnt_rect rect{};
  CMapData::GetRectInRadius(pEster->m_pCurMap, &rect, 3, currentSector);
  for (int y = rect.nStarty; y <= rect.nEndy; ++y)
  {
    for (int x = rect.nStartx; x <= rect.nEndx; ++x)
    {
      _sec_info *secInfo = CMapData::GetSecInfo(pEster->m_pCurMap);
      const unsigned int secIndex = secInfo->m_nSecNumW * y + x;
      CObjectList *sectorList = CMapData::GetSectorListObj(pEster->m_pCurMap, pEster->m_wMapLayerIndex, secIndex);
      if (!sectorList)
      {
        continue;
      }
      for (_object_list_point *node = sectorList->m_Head.m_pNext; node != &sectorList->m_Tail; node = node->m_pNext)
      {
        CGameObject *obj = node->m_pItem;
        if (!obj->m_ObjID.m_byKind
            && (obj->m_ObjID.m_byID == 7 || obj->m_ObjID.m_byID == 0)
            && obj != pEstObj)
        {
          if (obj->m_ObjID.m_byID == 7)
          {
            if (abs(obj->m_fCurPos[1] - pfEstPos[1]) <= 100.0f)
            {
              const int guardRange =
                static_cast<int>(*reinterpret_cast<float *>(&obj->m_pRecordSet[5].m_strCode[20]));
              const double dist = GetSqrt(obj->m_fCurPos, pfEstPos);
              if (static_cast<double>(guardRange) > dist)
              {
                return 15;
              }
            }
          }
          else
          {
            const CPlayer *targetPlayer = static_cast<CPlayer *>(obj);
            const CPlayer *esterPlayer = static_cast<CPlayer *>(pEster);
            const int targetRace = CPlayerDB::GetRaceCode(&targetPlayer->m_Param);
            const int esterRace = CPlayerDB::GetRaceCode(&esterPlayer->m_Param);
            if (targetRace != esterRace)
            {
              const double dist = GetSqrt(obj->m_fCurPos, pfEstPos);
              if (pEstTrapItemInfo->m_fGADst > dist)
              {
                return 18;
              }
            }
          }
        }
      }
    }
  }
  return 0;
}

