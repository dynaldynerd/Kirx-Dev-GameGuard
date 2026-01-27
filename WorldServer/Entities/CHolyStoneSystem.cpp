#include "pch.h"

#include "CHolyStoneSystem.h"

#include <cstring>
#include "GlobalObjects.h"
#include "CHolyStone.h"
#include "COreAmountMgr.h"
#include "WorldServerUtil.h"

bool CHolyStoneSystem::InitHolySystem(CHolyStoneSystem *pSystem)
{
  // CHolyStoneSystemDataMgr helpers are assumed to be implemented or reachable
  // if (CHolyStoneSystemDataMgr::LoadIni(pSystem)) ...
  
  // Minimal Logic for now to maintain IDA flow
  for (int j = 0; j < pSystem->m_nHolyStoneNum; ++j)
  {
    __holy_stone_data *v13 = &pSystem->m_HolyStoneData[j];
    _stone_create_setdata pData;
    pData.m_pMap = v13->pCreateMap;
    pData.m_nLayerIndex = 0;
    pData.m_pRecordSet = v13->pRec;
    std::memcpy(pData.m_fStartPos, v13->CreateDummy.m_fCenterPos, sizeof(pData.m_fStartPos));
    pData.pDumPosition = &v13->CreateDummy;
    pData.byMasterRace = (unsigned char)v13->nRace;
    CHolyStone::Create(&g_Stone[j], &pData);
  }

  COreAmountMgr *v4 = COreAmountMgr::Instance();
  if (!v4->LoadINI())
  {
    return false;
  }

  bool v15 = GetPrivateProfileIntA("Schedule", "ScheduleInit", 1, ".\\Initialize\\WorldSystem.ini") != 0;
  if (v15)
  {
    WritePrivateProfileStringA("Schedule", "ScheduleInit", "0", ".\\Initialize\\WorldSystem.ini");
    v4->InitRemainOreAmount(0xFFFFFFFF, 0xFFFFFFFF);
    v4->InitTransferOre(0, 0);
  }

  pSystem->m_bScheduleCodePre = 0;
  pSystem->InitQuestCash_Other();
  return true;
}

bool CHolyStoneSystem::ContinueStartSystem()
{
  return true;
}

void CHolyStoneSystem::InitQuestCash_Other()
{
  std::memset(this->m_cashQuestOther, 0, sizeof(this->m_cashQuestOther));
}
