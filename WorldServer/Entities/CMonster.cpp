#include "pch.h"

#include "CMonster.h"

#include <cstring>

CMonster:: _monster_loot_index *CMonster::s_idxMonsterLoot = nullptr;

void CMonster::Init(_object_id *pID)
{
  CCharacter::Init(pID);
  m_EP.AllocEffParam();
  m_dwLastDestroyTime = 0;
  m_bDungeon = false;
  m_bOper = false;
  m_MonsterSkillPool.Init();
  m_bApparition = false;
  m_dwDestroyNextTime = static_cast<unsigned int>(-1);
  m_dwLastRecoverTime = static_cast<unsigned int>(-1);
  m_fCreatePos[0] = 0.0f;
  m_fCreatePos[1] = 0.0f;
  m_fCreatePos[2] = 0.0f;
  m_fLookAtPos[0] = 0.0f;
  m_fLookAtPos[1] = 0.0f;
  m_fLookAtPos[2] = 0.0f;
  m_bRobExp = false;
  m_bRewardExp = false;
  m_bStdItemLoot = false;
  m_pActiveRec = nullptr;
  m_pMonRec = nullptr;
  m_pDumPosition = nullptr;
  m_nHP = 0;
  m_byCreateDate[0] = 0;
  m_byCreateDate[1] = 0;
  m_byCreateDate[2] = 0;
  m_byCreateDate[3] = 0;
  m_LifeMax = 0;
  m_LifeCicle = static_cast<unsigned int>(-1);
  m_fStartLookAtPos[0] = 0.0f;
  m_fStartLookAtPos[1] = 0.0f;
  m_fStartLookAtPos[2] = 0.0f;
  m_bRotateMonster = false;
  m_pTargetChar = nullptr;
  for (int j = 0; j < 5; ++j)
  {
    m_DefPart[j] = 0;
  }
  m_nEventItemNum = 0;
  std::memset(m_eventItem, 0, sizeof(m_eventItem));
  m_pEventRespawn = nullptr;
  m_pEventSet = nullptr;
  _InitSDM_LootTBL();
}

void CMonster::_InitSDM_LootTBL()
{
  if (s_idxMonsterLoot != nullptr)
  {
    return;
  }
  const int recordNum = static_cast<int>(g_Main.GetMonsterRecordNum());
  if (recordNum <= 0)
  {
    return;
  }
  s_idxMonsterLoot = new _monster_loot_index[recordNum];
  if (s_idxMonsterLoot == nullptr)
  {
    return;
  }
  for (int n = 0; n < recordNum; ++n)
  {
    s_idxMonsterLoot[n].nStartRecIndex = -1;
    s_idxMonsterLoot[n].nEndRecIndex = -1;
  }
}

