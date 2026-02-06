#include "pch.h"

#include "CAggroNode.h"
#include "CCharacter.h"
#include "GlobalObjects.h"
#include "CRecordData.h"

CAggroNode::CAggroNode()
{
  __int64 stackFill = 0;
  auto *fillPtr = &stackFill;
  for (int fillCount = 8; fillCount; --fillCount)
  {
    *reinterpret_cast<unsigned int *>(fillPtr) = 0xCCCCCCCC;
    fillPtr = reinterpret_cast<__int64 *>(reinterpret_cast<char *>(fillPtr) + 4);
  }
  Init();
}

void CAggroNode::Init()
{
  m_pCharacter = nullptr;
  m_dwObjectSerial = static_cast<unsigned int>(-1);
  m_nAggroData = 0;
  m_nDamageData = 0;
  m_nKingPowerDamage = 0;
}

void CAggroNode::Set(CCharacter *pCharacter)
{
  __int64 stackFill = 0;
  auto *fillPtr = &stackFill;
  for (int fillCount = 8; fillCount; --fillCount)
  {
    *reinterpret_cast<unsigned int *>(fillPtr) = 0xCCCCCCCC;
    fillPtr = reinterpret_cast<__int64 *>(reinterpret_cast<char *>(fillPtr) + 4);
  }

  Init();
  if (pCharacter)
  {
    m_pCharacter = pCharacter;
    m_dwObjectSerial = pCharacter->m_dwObjSerial;
  }
}

void CAggroNode::SetAggro(
  int nDam,
  float fAdd,
  int nAttackType,
  unsigned int dwAttackSerial,
  int bOtherPlayerSupport,
  int bFirstAttack,
  int bTempSkill)
{
  __int64 stackFill = 0;
  auto *fillPtr = &stackFill;
  for (int fillCount = 28; fillCount; --fillCount)
  {
    *reinterpret_cast<unsigned int *>(fillPtr) = 0xCCCCCCCC;
    fillPtr = reinterpret_cast<__int64 *>(reinterpret_cast<char *>(fillPtr) + 4);
  }

  int defaultValue = 0;

  if (m_nKingPowerDamage < nDam)
  {
    m_nKingPowerDamage = nDam;
  }

  m_nDamageData += nDam;
  if (m_nDamageData < 0)
  {
    m_nDamageData = 0;
  }

  if (bTempSkill)
  {
    const int specialValue = g_AggroCaculateData.GetSpecialData(
      static_cast<unsigned __int8>(nAttackType),
      static_cast<unsigned __int16>(dwAttackSerial));
    if (specialValue)
    {
      defaultValue = specialValue;
    }
    goto APPLY_AGGRO;
  }

  if (nAttackType == -1)
  {
    if (m_pCharacter->m_ObjID.m_byID == 3)
    {
      defaultValue = g_AggroCaculateData.GetDefault(7u);
      CCharacter *character = m_pCharacter;
      if (character)
      {
        const unsigned char extraKind =
          reinterpret_cast<unsigned char *>(&character[1].m_nScreenPos[1])[2];
        switch (extraKind)
        {
          case 1:
            defaultValue += g_AggroCaculateData.GetDefault(0xDu);
            break;
          case 2:
            defaultValue += g_AggroCaculateData.GetDefault(0xEu);
            break;
          case 3:
            defaultValue += g_AggroCaculateData.GetDefault(0xFu);
            break;
          case 4:
            defaultValue += g_AggroCaculateData.GetDefault(0x10u);
            break;
          default:
            break;
        }
      }
    }
    else if (m_pCharacter->m_ObjID.m_byID == 4)
    {
      defaultValue += g_AggroCaculateData.GetDefault(0x11u);
    }
    else
    {
      defaultValue = g_AggroCaculateData.GetDefault(0);
    }
    goto APPLY_AGGRO;
  }

  switch (nAttackType)
  {
    case 0:
    {
      const int specialValue = g_AggroCaculateData.GetSpecialData(0, static_cast<unsigned __int16>(dwAttackSerial));
      if (specialValue)
      {
        defaultValue = specialValue;
        break;
      }

      _base_fld *record = g_Main.m_tblEffectData[0].GetRecord(static_cast<int>(dwAttackSerial));
      if (!record)
      {
        break;
      }

      if (*reinterpret_cast<int *>(&record[13].m_strCode[32]))
      {
        if (*reinterpret_cast<int *>(&record[13].m_strCode[32]) != 1 || !bOtherPlayerSupport)
        {
          if (record[1].m_dwIndex)
          {
            if (record[1].m_dwIndex == 1)
            {
              defaultValue = g_AggroCaculateData.GetDefault(2u);
            }
            else
            {
              defaultValue = g_AggroCaculateData.GetDefault(1u);
            }
          }
          else
          {
            defaultValue = g_AggroCaculateData.GetDefault(3u);
          }
          break;
        }

        defaultValue = g_AggroCaculateData.GetDefault(6u);
        break;
      }

      defaultValue = g_AggroCaculateData.GetDefault(5u);
      break;
    }
    case 1:
    {
      const int specialValue = g_AggroCaculateData.GetSpecialData(1u, static_cast<unsigned __int16>(dwAttackSerial));
      if (specialValue)
      {
        defaultValue = specialValue;
        break;
      }

      _base_fld *record = g_Main.m_tblEffectData[1].GetRecord(static_cast<int>(dwAttackSerial));
      if (!record)
      {
        break;
      }

      if (*reinterpret_cast<int *>(&record[12].m_strCode[48]))
      {
        if (*reinterpret_cast<int *>(&record[12].m_strCode[48]) != 1 || !bOtherPlayerSupport)
        {
          defaultValue = g_AggroCaculateData.GetDefault(1u);
          break;
        }

        defaultValue = g_AggroCaculateData.GetDefault(6u);
        break;
      }

      defaultValue = g_AggroCaculateData.GetDefault(5u);
      break;
    }
    case 2:
    {
      const int specialValue = g_AggroCaculateData.GetSpecialData(2u, static_cast<unsigned __int16>(dwAttackSerial));
      if (specialValue)
      {
        defaultValue = specialValue;
        break;
      }

      _base_fld *record = g_Main.m_tblEffectData[2].GetRecord(static_cast<int>(dwAttackSerial));
      if (record)
      {
        if (!*reinterpret_cast<int *>(&record[13].m_strCode[32]))
        {
          defaultValue = g_AggroCaculateData.GetDefault(5u);
          break;
        }

        if (*reinterpret_cast<int *>(&record[13].m_strCode[32]) != 1 || !bOtherPlayerSupport)
        {
          defaultValue = g_AggroCaculateData.GetDefault(4u);
          break;
        }

        defaultValue = g_AggroCaculateData.GetDefault(6u);
      }
      break;
    }
    default:
      break;
  }

APPLY_AGGRO:
  m_nAggroData += defaultValue;
  if (bFirstAttack)
  {
    m_nAggroData += g_AggroCaculateData.GetDefault(9u);
  }
  m_nAggroData = static_cast<int>(static_cast<float>(m_nAggroData)
                                  + static_cast<float>(m_nAggroData) * fAdd);
}

bool CAggroNode::IsLive()
{
  int stackFill = 0;
  int *fillPtr = &stackFill;
  for (int fillCount = 4; fillCount; --fillCount)
  {
    *fillPtr++ = -858993460;
  }

  return m_dwObjectSerial != static_cast<unsigned int>(-1) && m_pCharacter && m_pCharacter->m_bLive
         && !m_pCharacter->m_bCorpse;
}
