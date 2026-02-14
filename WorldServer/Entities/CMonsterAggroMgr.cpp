#include "pch.h"

#include "CMonsterAggroMgr.h"

#include "CMonster.h"
#include "CPlayer.h"
#include "WorldServerUtil.h"

CMonsterAggroMgr::CMonsterAggroMgr()
{
  m_pTopAggroCharacter = nullptr;
  m_pTopDamageCharacter = nullptr;
  m_pKingPowerDamageCharacter = nullptr;
  m_dwAggroCount = 0;
  m_dwAllResetLastTime = 0;
  m_dwShortRankLastTime = 0;
  m_dwAllResetTimer = 30000;
  m_dwShortRankTimer = 5000;
  m_pMonster = nullptr;
}

CMonsterAggroMgr::~CMonsterAggroMgr()
{
  // this is not a stub
}

void CMonsterAggroMgr::Init()
{
  for (int j = 0; j < 10; ++j)
  {
    m_AggroPool[j].Init();
  }
  m_dwAggroCount = 0;
  m_dwAllResetLastTime = 0;
  m_dwShortRankLastTime = 0;
  m_dwShortRankTimer = m_pMonster->GetAggroShortTime();
  m_dwAllResetTimer = m_pMonster->GetAggroResetTime();
  m_pTopDamageCharacter = nullptr;
  m_pTopAggroCharacter = nullptr;
  m_pKingPowerDamageCharacter = nullptr;
}

void CMonsterAggroMgr::ResetAggro()
{
  m_dwShortRankLastTime = m_dwShortRankTimer + GetLoopTime();
}

CCharacter *CMonsterAggroMgr::GetTopAggroCharacter()
{
  if (m_pTopAggroCharacter && m_pTopAggroCharacter->m_bLive && !m_pTopAggroCharacter->m_bCorpse)
  {
    return m_pTopAggroCharacter;
  }
  m_pTopAggroCharacter = nullptr;
  return nullptr;
}


void CMonsterAggroMgr::OnlyOnceInit(CMonster *pMonster)
{
  m_pMonster = pMonster;
  m_dwAllResetTimer = 30000;
  m_dwShortRankTimer = 5000;
}

CAggroNode *CMonsterAggroMgr::_SearchAggroNode(CCharacter *pCharacter)
{
  if (pCharacter)
  {
    for (int j = 0; j < 10; ++j)
    {
      if (m_AggroPool[j].m_pCharacter == pCharacter && m_AggroPool[j].m_dwObjectSerial == pCharacter->m_dwObjSerial
          && m_AggroPool[j].IsLive())
      {
        return &m_AggroPool[j];
      }
    }
  }
  return nullptr;
}

CAggroNode *CMonsterAggroMgr::_GetBlinkNode()
{
  for (int j = 0; j < 10; ++j)
  {
    if (!m_AggroPool[j].IsLive())
    {
      return &m_AggroPool[j];
    }
  }
  return nullptr;
}

CAggroNode *CMonsterAggroMgr::SearchAggroNode(CCharacter *pCharacter)
{
  return _SearchAggroNode(pCharacter);
}

void CMonsterAggroMgr::SetAggro(
  CCharacter *pCharacter,
  int nDam,
  int nAttackType,
  unsigned int dwAttackSerial,
  int bOtherPlayerSupport,
  int bTempSkill)
{
  if (m_pMonster->IsBeDamagedAble(pCharacter) && pCharacter && pCharacter->m_bLive && !pCharacter->m_bCorpse)
  {
    CAggroNode *node = _SearchAggroNode(pCharacter);
    float add = 0.0f;
    if (!pCharacter->m_ObjID.m_byKind)
    {
      add = pCharacter->m_EP.GetEff_Have(28);
    }

    if (node)
    {
      node->SetAggro(nDam, add, nAttackType, dwAttackSerial, bOtherPlayerSupport, 0, 0);
      SendChangeAggroData();
    }
    else
    {
      node = _GetBlinkNode();
      if (node)
      {
        node->Set(pCharacter);
        const int firstAttack = (m_dwAggroCount == 0);
        node->SetAggro(nDam, add, nAttackType, dwAttackSerial, bOtherPlayerSupport, firstAttack, bTempSkill);
        SendChangeAggroData();
        ++m_dwAggroCount;
      }
    }
  }
}

void CMonsterAggroMgr::SendChangeAggroData()
{
  for (int j = 0; j < 10; ++j)
  {
    if (m_AggroPool[j].IsLive())
    {
      CPlayer *player = static_cast<CPlayer *>(m_AggroPool[j].m_pCharacter);
      if (!player->m_ObjID.m_byKind && !player->m_ObjID.m_byID)
      {
        if (player->m_byUserDgr >= 3)
        {
          CMonster *target = static_cast<CMonster *>(player->GetTargetObj());
          if (target && target == m_pMonster)
          {
            player->SendMsg_MonsterAggroData(m_pMonster);
          }
        }
      }
    }
  }
}
