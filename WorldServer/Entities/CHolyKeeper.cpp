#include "pch.h"

#include "CHolyKeeper.h"

#include <new>
#include <mmsystem.h>

#include "CAttack.h"
#include "CEventLootTable.h"
#include "CItemLootTable.h"
#include "CItemBox.h"
#include "CMapData.h"
#include "CMonster.h"
#include "GlobalObjects.h"
#include "TimeItem.h"
#include "WorldServerUtil.h"

int CHolyKeeper::s_nLiveNum = 0;
unsigned int CHolyKeeper::s_dwSerialCnt = 0;

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
#pragma pack(push, 1)
  struct CreateMsg
  {
    unsigned __int16 wRecordIndex;
    unsigned __int16 wIndex;
    unsigned int dwObjSerial;
    unsigned __int16 posShort[3];
  } msg{};
#pragma pack(pop)

  msg.wIndex = m_ObjID.m_wIndex;
  msg.wRecordIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  msg.dwObjSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, reinterpret_cast<short *>(msg.posShort), 3);
  unsigned __int8 pbyType[2] = {3, static_cast<unsigned __int8>(-44)};
  CircleReport(pbyType, reinterpret_cast<char *>(&msg), 14, 0);
}

void CHolyKeeper::SendMsg_Destroy(unsigned __int8 byDesType)
{
#pragma pack(push, 1)
  struct DestroyMsg
  {
    unsigned int dwObjSerial;
    unsigned __int8 byDesType;
  } msg{};
#pragma pack(pop)

  msg.dwObjSerial = m_dwObjSerial;
  msg.byDesType = byDesType;
  unsigned __int8 pbyType[2] = {3, static_cast<unsigned __int8>(-46)};
  CircleReport(pbyType, reinterpret_cast<char *>(&msg), 5, 0);
}

void CHolyKeeper::SendMsg_Move()
{
#pragma pack(push, 1)
  struct MoveMsg
  {
    unsigned int dwObjSerial;
    unsigned __int16 posShort[3];
    unsigned __int16 tarX;
    unsigned __int16 tarZ;
    unsigned __int8 byExit;
  } msg{};
#pragma pack(pop)

  msg.dwObjSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, reinterpret_cast<short *>(msg.posShort), 3);
  msg.tarX = static_cast<unsigned __int16>(m_fTarPos[0]);
  msg.tarZ = static_cast<unsigned __int16>(m_fTarPos[2]);
  msg.byExit = static_cast<unsigned __int8>(m_bExit);
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
