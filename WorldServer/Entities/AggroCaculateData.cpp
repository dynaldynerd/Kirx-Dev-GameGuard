#include "pch.h"

#include "AggroCaculateData.h"

#include <cstdlib>
#include <cstring>

#include "base_fld.h"
#include "CMainThread.h"
#include "WorldServerUtil.h"

AggroCaculateData::AggroCaculateData()
{
  Init();
}

void AggroCaculateData::Init()
{
  m_nTotalCount = 0;
  m_bLoad = 0;
  std::memset(m_DefaultData, 0, sizeof(m_DefaultData));
  std::memset(m_SpecialData, 0, sizeof(m_SpecialData));
}

int AggroCaculateData::GetDefault(unsigned int dwKind)
{
  if (dwKind < 18)
  {
    return m_DefaultData[dwKind];
  }
  return 0;
}

int AggroCaculateData::GetSize()
{
  return m_nTotalCount;
}

int AggroCaculateData::GetSpecialData(unsigned __int8 byAttackType, unsigned __int16 wIndex)
{
  const int maxCount = (GetSize() >= 50) ? 50 : GetSize();
  for (int j = 0; j < maxCount; ++j)
  {
    if (m_SpecialData[j].m_byAttackType == byAttackType && m_SpecialData[j].m_wIndex == wIndex)
    {
      return m_SpecialData[j].m_nValue;
    }
  }
  return 0;
}

int AggroCaculateData::PushSpecialData(unsigned __int8 byAttackType, unsigned __int16 wIndex, int nValue)
{
  if (m_nTotalCount >= 50)
  {
    return 0;
  }
  m_SpecialData[m_nTotalCount].m_byAttackType = byAttackType;
  m_SpecialData[m_nTotalCount].m_wIndex = wIndex;
  m_SpecialData[m_nTotalCount].m_nValue = nValue;
  ++m_nTotalCount;
  return 1;
}

int AggroCaculateData::Load(const char *fileName)
{
  Init();

  m_DefaultData[0] = GetPrivateProfileIntA("Default", "eGen", -1, fileName);
  m_DefaultData[1] = GetPrivateProfileIntA("Default", "eForce", -1, fileName);
  m_DefaultData[2] = GetPrivateProfileIntA("Default", "eLong_Attack", -1, fileName);
  m_DefaultData[3] = GetPrivateProfileIntA("Default", "eShort_Attack", -1, fileName);
  m_DefaultData[4] = GetPrivateProfileIntA("Default", "eClass_Skill", -1, fileName);
  m_DefaultData[5] = GetPrivateProfileIntA("Default", "eCurse_SkillForce", -1, fileName);
  m_DefaultData[6] = GetPrivateProfileIntA("Default", "eProfitSupport_SkillForce", -1, fileName);
  m_DefaultData[7] = GetPrivateProfileIntA("Default", "eAnimus_Attack", -1, fileName);
  m_DefaultData[12] = GetPrivateProfileIntA("Default", "eLowLevel_Target_Bonus", -1, fileName);
  m_DefaultData[13] = GetPrivateProfileIntA("Default", "eAnimus_Attack_warrior", -1, fileName);
  m_DefaultData[14] = GetPrivateProfileIntA("Default", "eAnimus_Attack_ranger", -1, fileName);
  m_DefaultData[15] = GetPrivateProfileIntA("Default", "eAnimus_Attack_healer", -1, fileName);
  m_DefaultData[16] = GetPrivateProfileIntA("Default", "eAnimus_Attack_force", -1, fileName);
  m_DefaultData[17] = GetPrivateProfileIntA("Default", "eTower_Attack", -1, fileName);
  m_DefaultData[8] = GetPrivateProfileIntA("Default", "eNear_Bonus", -1, fileName);
  m_DefaultData[9] = GetPrivateProfileIntA("Default", "eFirstAttack_Bonus", -1, fileName);
  m_DefaultData[10] = GetPrivateProfileIntA("Default", "eKingPower_Bonus", -1, fileName);
  m_DefaultData[11] = GetPrivateProfileIntA("Default", "eAccumDamage_Bonus", -1, fileName);

  for (int j = 0; j < 18; ++j)
  {
    if (m_DefaultData[j] <= 0 || m_DefaultData[j] > 1000)
    {
      return 0;
    }
  }

  int specialCount = GetPrivateProfileIntA("Special", "SFCount", -1, fileName);
  if (specialCount <= 0 || specialCount >= 50)
  {
    return 0;
  }

  for (int j = 0; j < specialCount; ++j)
  {
    char key[288]{};
    char returnedString[544]{};
    sprintf_s(key, sizeof(key), "SF_%d", j);
    GetPrivateProfileStringA("Special", key, "-1", returnedString, static_cast<DWORD>(sizeof(returnedString)), fileName);
    if (std::strncmp(returnedString, "-1", 2) == 0)
    {
      return 0;
    }

    char token0[64]{};
    char token1[64]{};
    char token2[64]{};
    char *tokens[3] = {token0, token1, token2};
    if (ParsingCommandA(returnedString, 3, tokens, 64) != 3)
    {
      return 0;
    }

    const int attackType = std::atoi(token1);
    const int value = std::atoi(token2);

    if (attackType == 1)
    {
      _base_fld *record = g_Main.m_tblEffectData[1].GetRecord(token0);
      if (!record)
      {
        return 0;
      }
      if (!PushSpecialData(1, static_cast<unsigned __int16>(record->m_dwIndex), value))
      {
        return 0;
      }
    }
    else if (attackType == 2)
    {
      _base_fld *record = g_Main.m_tblEffectData[2].GetRecord(token0);
      if (!record)
      {
        return 0;
      }
      if (!PushSpecialData(2, static_cast<unsigned __int16>(record->m_dwIndex), value))
      {
        return 0;
      }
    }
    else
    {
      _base_fld *record = g_Main.m_tblEffectData[0].GetRecord(token0);
      if (!record)
      {
        return 0;
      }
      if (!PushSpecialData(0, static_cast<unsigned __int16>(record->m_dwIndex), value))
      {
        return 0;
      }
    }
  }

  m_bLoad = 1;
  return m_bLoad != 0 ? 1 : 0;
}
