#include "pch.h"

#include "CMonsterAggroMgr.h"

#include "AggroCaculateData.h"
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

void CMonsterAggroMgr::Process()
{
  const unsigned int loopTime = GetLoopTime();
  if (m_dwAggroCount)
  {
    if (loopTime - m_dwAllResetLastTime >= m_dwAllResetTimer)
    {
      Init();
      m_dwAllResetLastTime = loopTime;
    }

    if (static_cast<int>(loopTime - m_dwShortRankLastTime) >= static_cast<int>(m_dwShortRankTimer))
    {
      _ShortRank();
      SendChangeAggroData();
      m_dwShortRankLastTime = loopTime;
    }
  }
  else
  {
    m_dwShortRankLastTime = loopTime;
    m_dwAllResetLastTime = loopTime;
  }
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

void CMonsterAggroMgr::_ShortRank()
{
  CCharacter *previousTopAggroCharacter = m_pTopAggroCharacter;
  m_pTopDamageCharacter = nullptr;
  m_pTopAggroCharacter = nullptr;
  m_pKingPowerDamageCharacter = nullptr;

  int topAggroData = -1000;
  int topDamageData = 0;
  int topKingPowerDamage = 0;
  int minLevel = 10000;
  int minLevelNodeIndex = -1;

  for (int nodeIndex = 0; nodeIndex < 10; ++nodeIndex)
  {
    CAggroNode &node = m_AggroPool[nodeIndex];
    if (node.IsLive())
    {
      if (node.m_nDamageData > topDamageData)
      {
        topDamageData = node.m_nDamageData;
        m_pTopDamageCharacter = node.m_pCharacter;
      }

      if (node.m_nKingPowerDamage > topKingPowerDamage)
      {
        topKingPowerDamage = node.m_nKingPowerDamage;
        m_pKingPowerDamageCharacter = node.m_pCharacter;
      }

      if (node.m_pCharacter && !node.m_pCharacter->m_ObjID.m_byID)
      {
        const int playerLevel = static_cast<int>(node.m_pCharacter->GetLevel());
        if (playerLevel < minLevel)
        {
          minLevelNodeIndex = nodeIndex;
        }
      }
    }
    else
    {
      node.Init();
    }
  }

  for (int nodeIndex = 0; nodeIndex < 10; ++nodeIndex)
  {
    CAggroNode &node = m_AggroPool[nodeIndex];

    if (node.IsLive() && m_pKingPowerDamageCharacter == node.m_pCharacter)
    {
      node.m_nAggroData += g_AggroCaculateData.GetDefault(10u);
    }

    if (node.IsLive() && m_pKingPowerDamageCharacter == node.m_pCharacter)
    {
      node.m_nAggroData += g_AggroCaculateData.GetDefault(11u);
    }

    if (node.IsLive())
    {
      const float dist = GetSqrt(m_pMonster->m_fCurPos, node.m_pCharacter->m_fCurPos);
      if (m_pMonster->GetBonusInAreaAggro() >= dist)
      {
        node.m_nAggroData += g_AggroCaculateData.GetDefault(8u);
      }
    }

    if (minLevelNodeIndex != -1 && node.IsLive())
    {
      node.m_nAggroData += g_AggroCaculateData.GetDefault(12u);
    }

    if (node.IsLive() && node.m_nAggroData > topAggroData)
    {
      topAggroData = node.m_nAggroData;
      m_pTopAggroCharacter = node.m_pCharacter;
    }
  }

  if (previousTopAggroCharacter != m_pTopAggroCharacter && m_pTopAggroCharacter)
  {
    m_pMonster->CheckEventEmotionPresentation(10u, m_pTopAggroCharacter);
    m_pMonster->m_AI.SendExternMsg(0u, m_pTopAggroCharacter, 0u);
  }
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
