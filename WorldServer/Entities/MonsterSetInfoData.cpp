#include "pch.h"

#include "MonsterSetInfoData.h"

#include <cstdlib>
#include <cstring>

#include "CMainThread.h"
#include "WorldServerUtil.h"

MonsterSetInfoData::MonsterSetInfoData()
{
  Init();
}

void MonsterSetInfoData::Init()
{
  m_bLoad = 0;
  for (int j = 0; j < 4; ++j)
  {
    for (int k = 0; k < 7; ++k)
    {
      m_byLevel_ContSFTime[j][k] = static_cast<unsigned __int8>(k + 1);
    }
  }
  m_nMonsterLostTargetDistance = 285;
  m_fMonsterForcePowerRate = 40.0f;
  if (m_strRotMonBlk_Ar)
  {
    delete[] m_strRotMonBlk_Ar;
    m_strRotMonBlk_Ar = nullptr;
  }
  m_nMonBlkCount = 0;
}

bool MonsterSetInfoData::Load(const char *fileName)
{
  for (int j = 0; j < 4; ++j)
  {
    char key[288]{};
    char returnedString[516]{};
    sprintf_s(key, sizeof(key), "eLevel_ContSF_Time_%d", j);
    GetPrivateProfileStringA("Common", key, "-1", returnedString, static_cast<DWORD>(sizeof(returnedString)), fileName);
    if (std::strncmp(returnedString, "-1", 2) == 0)
    {
      return false;
    }

    char token0[64]{};
    char token1[64]{};
    char token2[64]{};
    char token3[64]{};
    char token4[64]{};
    char token5[64]{};
    char token6[64]{};
    char *tokens[7] = {token0, token1, token2, token3, token4, token5, token6};
    if (ParsingCommandA(returnedString, 7, tokens, 64) != 7)
    {
      return false;
    }

    for (int k = 0; k < 7; ++k)
    {
      m_byLevel_ContSFTime[j][k] = static_cast<unsigned __int8>(std::atoi(tokens[k]));
    }
  }

  m_nMonsterLostTargetDistance = GetPrivateProfileIntA("Common", "MonsterLostTargetDistance", 285, fileName);
  m_fMonsterForcePowerRate = static_cast<float>(GetPrivateProfileIntA("Common", "MonsterForcePowerRate", 40, fileName));
  m_nMonBlkCount = GetPrivateProfileIntA("MonsRotBlk", "m_nMonBlkCount", -1, fileName);

  if (m_nMonBlkCount > 0)
  {
    const size_t count = static_cast<size_t>(m_nMonBlkCount) * 2;
    m_strRotMonBlk_Ar = new char[count][64];
    bool ok = true;
    for (int j = 0; j < m_nMonBlkCount; ++j)
    {
      char key[288]{};
      char returnedString[516]{};
      sprintf_s(key, sizeof(key), "Blk_%d", j);
      GetPrivateProfileStringA("MonsRotBlk", key, "-1", returnedString, static_cast<DWORD>(sizeof(returnedString)), fileName);
      if (std::strncmp(returnedString, "-1", 2) == 0)
      {
        ok = false;
        break;
      }

      char token0[64]{};
      char token1[64]{};
      char *tokens[2] = {token0, token1};
      if (ParsingCommandA(returnedString, 2, tokens, 64) != 2)
      {
        ok = false;
        break;
      }

      for (int k = 0; k < 2; ++k)
      {
        strcpy_s(m_strRotMonBlk_Ar[(2 * j) + k], 64, tokens[k]);
      }
    }

    if (!ok)
    {
      delete[] m_strRotMonBlk_Ar;
      m_strRotMonBlk_Ar = nullptr;
      m_nMonBlkCount = 0;
      return false;
    }
  }

  for (int j = 0; j < 7; ++j)
  {
    char key[288]{};
    sprintf_s(key, sizeof(key), "lv_%d", j);
    const int val = GetPrivateProfileIntA("MonsDebuffTol", key, 100, fileName);
    m_fToleranceProbMax[j] = static_cast<float>(val) / 100.0f;
  }

  m_iMonsterLootRateSame = GetPrivateProfileIntA("DROPRATE", "SAME_WITH_MONSTER", 100, fileName);
  if (!m_iMonsterLootRateSame)
  {
    return false;
  }

  for (int j = 1; j <= 10; ++j)
  {
    char key[288]{};
    sprintf_s(key, sizeof(key), "BIG_%d_MONSTER", j);
    const int val = GetPrivateProfileIntA("DROPRATE", key, 0, fileName);
    m_iMonsterLootingRateUp[j - 1] = val;
    if (!m_iMonsterLootingRateUp[j - 1])
    {
      if (j == 1)
      {
        return false;
      }
      m_iMonsterLootingRateUp[j - 1] = m_iMonsterLootingRateUp[j - 1];
    }
  }
  m_iMonsterLootingRateUp[10] = m_iMonsterLootingRateUp[9];

  for (int j = 1; j <= 10; ++j)
  {
    char key[288]{};
    sprintf_s(key, sizeof(key), "SMALL_%d_MONSTER", j);
    const int val = GetPrivateProfileIntA("DROPRATE", key, 0, fileName);
    m_iMonsterLootingRateFlat[j + 10] = val;
    if (!m_iMonsterLootingRateFlat[j + 10])
    {
      if (j == 1)
      {
        return false;
      }
      m_iMonsterLootingRateFlat[j + 10] = m_iMonsterLootingRateFlat[j + 9];
    }
  }
  m_iMonsterLootingRateDown[10] = m_iMonsterLootingRateDown[9];

  m_bLoad = 1;
  return m_bLoad != 0;
}

unsigned __int8 MonsterSetInfoData::GetLevelContSFTime(unsigned __int8 byEffectCode, unsigned __int8 byLevel)
{
  if (byEffectCode <= 3 && byLevel && byLevel <= 7)
  {
    return m_byLevel_ContSFTime[byEffectCode - 1][byLevel - 1];
  }
  return 1;
}

int MonsterSetInfoData::GetLostMonsterTargetDistance()
{
  return m_nMonsterLostTargetDistance;
}

float MonsterSetInfoData::GetMonsterForcePowerRate()
{
  return m_fMonsterForcePowerRate / 100.0f;
}

float MonsterSetInfoData::GetMaxToleranceProbMax(int nMonGrade)
{
  if (nMonGrade > 6)
  {
    return 1.0f;
  }
  return m_fToleranceProbMax[nMonGrade];
}

unsigned int MonsterSetInfoData::GetMonsterDropRate(int iDiffLevel)
{
  if (iDiffLevel <= 0)
  {
    if (iDiffLevel >= 0)
    {
      return static_cast<unsigned int>(m_iMonsterLootRateSame);
    }
    const int diff = std::abs(iDiffLevel);
    if (diff > 10)
    {
      return static_cast<unsigned int>(m_iMonsterLootingRateDown[10]);
    }
    return static_cast<unsigned int>(m_iMonsterLootingRateFlat[diff + 10]);
  }
  if (iDiffLevel > 10)
  {
    return static_cast<unsigned int>(m_iMonsterLootingRateUp[10]);
  }
  return static_cast<unsigned int>(m_iMonsterLootingRateUp[iDiffLevel - 1]);
}

bool MonsterSetInfoData::IsRotateBlock(_mon_block *pBlock)
{
  if (m_nMonBlkCount <= 0)
  {
    return false;
  }
  if (!m_strRotMonBlk_Ar)
  {
    return false;
  }
  if (!pBlock || !pBlock->m_pBlkRec || !pBlock->m_pMap || !pBlock->m_pMap->m_pMapSet)
  {
    return false;
  }

  const char *blockCode = pBlock->m_pBlkRec->m_strCode;
  const char *mapCode = pBlock->m_pMap->m_pMapSet->m_strCode;
  for (int j = 0; j < m_nMonBlkCount; ++j)
  {
    if (std::strcmp(mapCode, m_strRotMonBlk_Ar[2 * j]) == 0
      && std::strcmp(blockCode, m_strRotMonBlk_Ar[2 * j + 1]) == 0)
    {
      return true;
    }
  }
  return false;
}
