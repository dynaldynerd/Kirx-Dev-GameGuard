#include "pch.h"

#include "WorldServerUtil.h"

#include "CBossMonsterScheduleSystem.h"

#include "CMainThread.h"
#include "CMonster.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <new>
#include <string>
#include <vector>

CBossMonsterScheduleSystem *CBossMonsterScheduleSystem::m_pInstance;

CBossMonsterScheduleSystem *CBossMonsterScheduleSystem::Instance()
{
  if (!m_pInstance)
  {
    auto *instance = static_cast<CBossMonsterScheduleSystem *>(operator new(sizeof(CBossMonsterScheduleSystem)));
    if (instance)
    {
      new (instance) CBossMonsterScheduleSystem();
    }
    m_pInstance = instance;
  }
  return m_pInstance;
}

CBossMonsterScheduleSystem::CBossMonsterScheduleSystem()
  : m_pMapOper(nullptr),
    m_pCurTBL(nullptr),
    m_MSG_POOL(),
    m_bRespawnMonster(false)
{
}

CBossMonsterScheduleSystem::~CBossMonsterScheduleSystem()
{
  Terminate(-1);
  if (m_pCurTBL)
  {
    delete m_pCurTBL;
    m_pCurTBL = nullptr;
  }
}

bool CBossMonsterScheduleSystem::Init(CMapOperation *mapOper)
{
  if (m_pCurTBL)
  {
    return false;
  }
  m_pMapOper = mapOper;
  m_bRespawnMonster = false;
  m_pCurTBL = MakeTBL(m_pMapOper);
  return CreateTaskPool() && CreateWorkerThread();
}

bool CBossMonsterScheduleSystem::CreateTaskPool()
{
  m_MSG_POOL.ReAllocPool(2000u);
  return true;
}

bool CBossMonsterScheduleSystem::CreateWorkerThread()
{
  US::ThreadParamInterface<CBossMonsterScheduleSystem, US::AbstractThreadPool> threadParam;
  threadParam.m_pOwner = this;
  return Create(
    &threadParam,
    US::CWinThread<US::ThreadParamInterface<CBossMonsterScheduleSystem, US::AbstractThreadPool>>::WorkerThread);
}

BossSchedule_TBL *CBossMonsterScheduleSystem::MakeTBL(CMapOperation *pMapOper)
{
  if (!pMapOper || pMapOper->m_nMapNum < 0)
  {
    return nullptr;
  }

  auto *tbl = new BossSchedule_TBL();
  if (!tbl)
  {
    return nullptr;
  }

  tbl->m_nCount = pMapOper->m_nMapNum;
  const size_t mapCount = static_cast<size_t>(tbl->m_nCount);
  tbl->m_MapScheduleList = static_cast<BossSchedule_Map **>(operator new[](saturated_mul(mapCount, sizeof(BossSchedule_Map *))));
  std::memset(tbl->m_MapScheduleList, 0, sizeof(BossSchedule_Map *) * mapCount);

  for (int nIndex = 0; nIndex < tbl->m_nCount; ++nIndex)
  {
    CMapData *pMap = &m_pMapOper->m_Map[nIndex];
    BossSchedule_Map *mapSchedule = MakeMap(nIndex, pMap);
    tbl->m_MapScheduleList[nIndex] = mapSchedule;
  }

  return tbl;
}

BossSchedule_Map *CBossMonsterScheduleSystem::MakeMap(int nIndex, CMapData *pMap)
{
  if (!pMap->m_bUse)
  {
    return nullptr;
  }
  if (pMap->m_pMapSet->m_nMapType)
  {
    return nullptr;
  }
  if (!pMap->m_ls->IsActiveLayer())
  {
    return nullptr;
  }
  if (pMap->m_nMonBlockNum < 0)
  {
    return nullptr;
  }

  char iniPath[260]{};
  sprintf_s(iniPath, sizeof(iniPath), "..\\SystemSave\\%s_Boss.ini", pMap->m_pMapSet->m_strFileName);

  int scheduleCount = 0;
  for (int nBlockIndex = 0; nBlockIndex < pMap->m_nMonBlockNum; ++nBlockIndex)
  {
    _mon_block *block = &pMap->m_pMonBlock[nBlockIndex];
    _mon_block_fld *blockRec = block->m_pBlkRec;
    _MULTI_BLOCK *multiBlock = pMap->m_ls->m_pMB;
    int recordNum = static_cast<int>(multiBlock->m_ptbMonBlock[nBlockIndex].GetRecordNum());
    for (int j = 0; j < recordNum; ++j)
    {
      _mon_active *monAct = &pMap->m_ls->m_MonAct[nBlockIndex][j];
      if (monAct->m_nLimRegenNum > 0)
      {
        _mon_active_fld *actRec = monAct->m_pActRec;
        _monster_fld *record = static_cast<_monster_fld *>(g_Main.m_tblMonster.GetRecord(actRec->m_strCode));
        if (record)
        {
          if (record->m_bMonsterCondition)
          {
            ++scheduleCount;
          }
        }
      }
    }
}

  if (!scheduleCount)
  {
    return nullptr;
  }

  CIniFile rhs;
  auto *mapSchedule = new BossSchedule_Map();
  if (!mapSchedule)
  {
    return nullptr;
  }

  sprintf_s(mapSchedule->m_strMap, sizeof(mapSchedule->m_strMap), "%s", pMap->m_pMapSet->m_strFileName);
  mapSchedule->m_pSystem = this;
  mapSchedule->m_nCount = scheduleCount;
  mapSchedule->m_nIndex = nIndex;
  mapSchedule->m_INIFile.SetIniFilename(iniPath);
  rhs.SetIniFilename(iniPath);
  rhs.Load();

  mapSchedule->m_ScheduleList =
    static_cast<BossSchedule **>(operator new[](saturated_mul(static_cast<size_t>(mapSchedule->m_nCount), sizeof(BossSchedule *))));
  std::memset(mapSchedule->m_ScheduleList, 0, sizeof(BossSchedule *) * mapSchedule->m_nCount);

  int scheduleIndex = 0;
  for (int nBlockIndex = 0; nBlockIndex < pMap->m_nMonBlockNum; ++nBlockIndex)
  {
    _mon_block *block = &pMap->m_pMonBlock[nBlockIndex];
    _mon_block_fld *blockRec = block->m_pBlkRec;
    _MULTI_BLOCK *multiBlock = pMap->m_ls->m_pMB;
    int recordNum = static_cast<int>(multiBlock->m_ptbMonBlock[nBlockIndex].GetRecordNum());
    for (int nActIndex = 0; nActIndex < recordNum; ++nActIndex)
    {
      _mon_active *monAct = &pMap->m_ls->m_MonAct[nBlockIndex][nActIndex];
      if (monAct->m_nLimRegenNum > 0)
      {
        _mon_active_fld *actRec = monAct->m_pActRec;
        _monster_fld *record = static_cast<_monster_fld *>(g_Main.m_tblMonster.GetRecord(actRec->m_strCode));
        if (record)
        {
          if (record->m_bMonsterCondition)
          {
            BossSchedule *schedule = MakeSchedule(mapSchedule, monAct, block, nActIndex, nBlockIndex);
            mapSchedule->m_ScheduleList[scheduleIndex++] = schedule;
          }
        }
      }
    }
}

  DeleteFileA(iniPath);
  mapSchedule->SaveAll();
  mapSchedule->m_INIFile.Merge_Intersection(&rhs);
  mapSchedule->LoadAll();
  return mapSchedule;
}

BossSchedule *CBossMonsterScheduleSystem::MakeSchedule(
  BossSchedule_Map *pMapSchedule,
  _mon_active *pMonAct,
  _mon_block *pBlock,
  int nActIndex,
  int nBlockIndex)
{
  if (!pBlock)
  {
    return nullptr;
  }
  if (!pMonAct)
  {
    return nullptr;
  }

  auto *schedule = new BossSchedule();
  if (!schedule)
  {
    return nullptr;
  }

  strcpy_s(schedule->m_strMonCode, sizeof(schedule->m_strMonCode), pMonAct->m_pActRec->m_strCode);
  schedule->m_pBlock = pBlock;
  schedule->m_pMonAct = pMonAct;
  schedule->m_nBlockIndex = nBlockIndex;
  schedule->m_nActIndex = nActIndex;
  schedule->m_LiveCount = 0;
  schedule->m_LastRespawnSystemTime = ATL::CTime(0);
  schedule->m_pParent = pMapSchedule;
  sprintf_s(schedule->m_strSection, sizeof(schedule->m_strSection), "BL_%d_%d", schedule->m_nBlockIndex, schedule->m_nActIndex);
  pMonAct->SetBossSchedule(schedule);
  return schedule;
}

BossSchedule *CBossMonsterScheduleSystem::LoadSchedule(BossSchedule_Map *pMapSchedule, INI_Section *pSection)
{
  if (!pSection)
  {
    return nullptr;
  }
  if (!pMapSchedule)
  {
    return nullptr;
  }

  CMapData *map = m_pMapOper->GetMap(pMapSchedule->m_strMap);
  if (!map)
  {
    return nullptr;
  }
  if (map->m_pMapSet->m_nMapType)
  {
    return nullptr;
  }
  if (!map->m_ls->IsActiveLayer())
  {
    return nullptr;
  }

  std::vector<std::string> stringlist;
  if (SplitString(pSection->m_strSection, "_", &stringlist) == 3)
  {
    const int blockIndex = atoi(stringlist[1].c_str());
    const int actIndex = atoi(stringlist[2].c_str());

    INI_Key *key = pSection->GetKey(BossSchedule::ms_KeyPair[1]);
    if (!key)
    {
      return nullptr;
    }
    INI_Key *keyLiveCount = pSection->GetKey(BossSchedule::ms_KeyPair[2]);
    if (!keyLiveCount)
    {
      return nullptr;
    }

    const unsigned __int16 liveCount = BossSchedule::Make_LiveCount(keyLiveCount->m_strValue);
    ATL::CTime lastRespawn;
    BossSchedule::Make_LastTimeRespawnSystemTime(&lastRespawn, key->m_strValue);

    if (blockIndex < map->m_nMonBlockNum)
    {
      _mon_block *block = &map->m_pMonBlock[blockIndex];
      _MULTI_BLOCK *multiBlock = map->m_ls->m_pMB;
      const int recordNum = static_cast<int>(multiBlock->m_ptbMonBlock[blockIndex].GetRecordNum());
      if (actIndex < recordNum)
      {
        _mon_active *monAct = &map->m_ls->m_MonAct[blockIndex][actIndex];
        if (monAct)
        {
          auto *schedule = new BossSchedule();
          if (!schedule)
          {
            return nullptr;
          }
          strcpy_s(schedule->m_strMonCode, sizeof(schedule->m_strMonCode), monAct->m_pActRec->m_strCode);
          schedule->m_pBlock = block;
          schedule->m_pMonAct = monAct;
          schedule->m_nBlockIndex = blockIndex;
          schedule->m_nActIndex = actIndex;
          schedule->m_LiveCount = liveCount;
          schedule->m_LastRespawnSystemTime = lastRespawn;
          schedule->m_pParent = pMapSchedule;
          sprintf_s(schedule->m_strSection, sizeof(schedule->m_strSection), "BL_%d_%d", schedule->m_nBlockIndex, schedule->m_nActIndex);
          monAct->SetBossSchedule(schedule);
          return schedule;
        }
        return nullptr;
      }
      return nullptr;
    }
    return nullptr;
  }
  return nullptr;
}

void CBossMonsterScheduleSystem::Savechedule(BossSchedule_Map *pMapSchedule, BossSchedule *pSchedule)
{
  if (pMapSchedule)
  {
    char strBuff[80]{};
    pMapSchedule->m_INIFile.WriteString(pSchedule->m_strSection, BossSchedule::ms_KeyPair[0], pSchedule->m_strMonCode);
    pSchedule->Make_LastTimeRespawnSystemTimeString(strBuff, 64);
    pMapSchedule->m_INIFile.WriteString(pSchedule->m_strSection, BossSchedule::ms_KeyPair[1], strBuff);
    pSchedule->Make_LiveCountString(strBuff, 64);
    pMapSchedule->m_INIFile.WriteString(pSchedule->m_strSection, BossSchedule::ms_KeyPair[2], strBuff);
  }
}

void CBossMonsterScheduleSystem::AnalysisMsg(ScheduleMSG *pMSG)
{
  if (pMSG->m_byKey < 3u && m_pCurTBL && pMSG->m_wIniFileIndex < m_pCurTBL->m_nCount)
  {
    const char *strKey = BossSchedule::ms_KeyPair[pMSG->m_byKey];
    BossSchedule_Map *mapSchedule = m_pCurTBL->m_MapScheduleList[pMSG->m_wIniFileIndex];
    if (mapSchedule)
    {
      mapSchedule->m_INIFile.WriteString(pMSG->m_strSection, strKey, pMSG->m_strValue);
    }
  }
}

__int64 CBossMonsterScheduleSystem::WorkProc()
{
  m_bRunning = true;
  while (IsRunning())
  {
    ScheduleMSG *pMSG = m_MSG_POOL.PopProc();
    if (pMSG)
    {
      AnalysisMsg(pMSG);
      m_MSG_POOL.PushEmpty(pMSG);
    }
    Sleep(1u);
  }
  return 0;
}

void CBossMonsterScheduleSystem::RespawnMonster()
{
  if (!m_bRespawnMonster && m_pCurTBL)
  {
    for (int j = 0; j < m_pCurTBL->m_nCount; ++j)
    {
      BossSchedule_Map *mapSchedule = m_pCurTBL->m_MapScheduleList[j];
      if (mapSchedule)
      {
        CMapData *map = m_pMapOper->GetMap(mapSchedule->m_strMap);
        for (int k = 0; k < mapSchedule->m_nCount; ++k)
        {
          BossSchedule *schedule = mapSchedule->m_ScheduleList[k];
          if (schedule)
          {
            const int liveCount = schedule->m_LiveCount;
            for (int m = 0; m < liveCount; ++m)
            {
              _mon_block *block = schedule->m_pBlock;
              _mon_active *active = schedule->m_pMonAct;
              const int dummyIndex = block->SelectDummyIndex();
              CreateRespawnMonster(
                map,
                0,
                active->m_wMonRecIndex,
                active,
                block->m_pDumPos[dummyIndex],
                1,
                1,
                0,
                0,
                1);
            }
          }
        }
      }
    }
    m_bRespawnMonster = true;
  }
}
