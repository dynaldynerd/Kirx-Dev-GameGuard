#include "pch.h"

#include "CMainThread.h"
#include "GlobalObjects.h"
#include "CLogFile.h"
#include "monster_fld.h"
#include "WorldServerUtil.h"

#include <cstdlib>

MonsterSFContDamageToleracne::MonsterSFContDamageToleracne()
{
  m_pMonster = nullptr;
  m_fToleranceProb = 0.0f;
  m_fToleranceProbMax = 0.0f;
  m_dwLastUpdateTime = 0;
}

void MonsterSFContDamageToleracne::Init(float fMaxTolValue)
{
  m_dwLastUpdateTime = GetLoopTime();
  float tolValue = fMaxTolValue;
  if (fMaxTolValue > 1.0f || fMaxTolValue < 0.0f)
  {
    tolValue = FLOAT_1_0;
  }
  m_fToleranceProbMax = tolValue;
  m_fToleranceProb = tolValue;
}

EmotionPresentationChecker::EmotionPresentationChecker()
{
  m_bIsSet = false;
  m_byType = 0;
  m_wIndex = static_cast<unsigned __int16>(-1);
  m_wRandIndex = static_cast<unsigned __int16>(-1);
  m_pTarget = nullptr;
}

void EmotionPresentationChecker::ReSet()
{
  m_bIsSet = false;
  m_pTarget = nullptr;
}

bool EmotionPresentationChecker::CheckEmotionState(
  CMonster *pThis,
  unsigned __int8 byCheckType,
  CCharacter *pTarget)
{
  if (m_bIsSet || !pThis)
  {
    return false;
  }
  if (!pThis->m_pMonRec)
  {
    return false;
  }

  for (int index = 0; index < 5 && !m_bIsSet; ++index)
  {
    const _monster_fld::_EmotionPresentation &checker = pThis->m_pMonRec->m_EmotionChecker[index];
    if (checker.m_nEmotionCon > 0 && checker.m_nEmotionCon == byCheckType)
    {
      const unsigned __int8 checkType = byCheckType;
      if (checkType && checkType <= 0xAu)
      {
        m_bIsSet = true;
        m_pTarget = nullptr;
        m_byType = checker.m_nEmotionClass;
        if (m_byType)
        {
          m_wIndex = static_cast<unsigned __int16>(std::atoi(checker.m_strEmotionCode));
        }
        else
        {
          m_wIndex = static_cast<unsigned __int16>(std::atoi(checker.m_strEmotionCode));
          unsigned __int16 usingNum = g_Main.m_MobMessage[m_wIndex].byUsingNum;
          if (!usingNum)
          {
            g_Main.m_logSystemError.Write(
              "g_Main.m_tblMobMessage.GetRecord(m_wIndex(%u)->m_nUsingNum == 0 !!",
              m_wIndex);
            usingNum = 1;
          }
          m_wRandIndex = static_cast<unsigned __int16>(GetTickCount() % usingNum);
        }
        if (checkType == 10 || checkType == 7)
        {
          m_pTarget = pTarget;
        }
      }
    }
  }

  return m_bIsSet;
}

MonsterStateData::MonsterStateData()
{
  ___u0.m_wSendChunkData = 0;
}

unsigned __int16 MonsterStateData::GetStateChunk()
{
  return ___u0.m_wSendChunkData;
}

bool MonsterStateData::operator!=(const MonsterStateData *rhs)
{
  return ___u0.m_wSendChunkData != rhs->___u0.m_wSendChunkData;
}

void MonsterSFContDamageToleracne::OnlyOnceInit(CMonster *pMonster)
{
  m_pMonster = pMonster;
}

bool MonsterSFContDamageToleracne::IsSFContDamage()
{
  const unsigned int prob = static_cast<unsigned int>(static_cast<int>(m_fToleranceProb * 100.0f));
  return prob <= static_cast<unsigned int>(rand() % 100);
}

float MonsterSFContDamageToleracne::GetToleranceProb()
{
  return m_fToleranceProb;
}

void MonsterSFContDamageToleracne::SetSFDamageToleracne_Variation(float fAddValue)
{
  m_fToleranceProb = m_fToleranceProb + fAddValue;
  if (m_fToleranceProb > m_fToleranceProbMax)
  {
    m_fToleranceProb = m_fToleranceProbMax;
  }
  if (m_fToleranceProb < 0.0f)
  {
    m_fToleranceProb = 0.0f;
  }
}

void MonsterSFContDamageToleracne::Update()
{
  if (m_pMonster && m_pMonster->m_bLive)
  {
    const unsigned int loopTime = GetLoopTime();
    unsigned int elapsed = 0;
    if (m_pMonster->GetMyDMGSFContCount() <= 0)
    {
      if (m_dwLastUpdateTime < loopTime)
      {
        elapsed = GetLoopTime() - m_dwLastUpdateTime;
      }
      else
      {
        elapsed = 0;
      }
      if (elapsed >= 0x3E8)
      {
        SetSFDamageToleracne_Variation(0.02f);
        m_dwLastUpdateTime = loopTime;
      }
    }
    else
    {
      m_dwLastUpdateTime = loopTime;
    }
  }
}
