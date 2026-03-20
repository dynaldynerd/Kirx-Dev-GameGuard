#include "pch.h"

#include "CAggroNode.h"
#include "CAnimus.h"
#include "CCharacter.h"
#include "GlobalObjects.h"
#include "CRecordData.h"
#include "skill_fld.h"
#include "force_fld.h"

CAggroNode::CAggroNode()
{
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
  }
  else if (nAttackType == -1)
  {
    if (m_pCharacter->m_ObjID.m_byID == 3)
    {
      defaultValue = g_AggroCaculateData.GetDefault(7);
      CAnimus *animus = static_cast<CAnimus *>(m_pCharacter);
      if (animus)
      {
        const unsigned char extraKind = animus->m_byRoleCode;
        switch (extraKind)
        {
          case 1:
            defaultValue += g_AggroCaculateData.GetDefault(13);
            break;
          case 2:
            defaultValue += g_AggroCaculateData.GetDefault(14);
            break;
          case 3:
            defaultValue += g_AggroCaculateData.GetDefault(15);
            break;
          case 4:
            defaultValue += g_AggroCaculateData.GetDefault(16);
            break;
          default:
            break;
        }
      }
    }
    else if (m_pCharacter->m_ObjID.m_byID == 4)
    {
      defaultValue += g_AggroCaculateData.GetDefault(17);
    }
    else
    {
      defaultValue = g_AggroCaculateData.GetDefault(0);
    }
  }
  else
  {
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

        _skill_fld *record = static_cast<_skill_fld *>(g_Main.m_tblEffectData[0].GetRecord(static_cast<int>(dwAttackSerial)));
        if (!record)
        {
          break;
        }

        if (record->m_nContEffectType)
        {
          if (record->m_nContEffectType != 1 || !bOtherPlayerSupport)
          {
            if (record->m_nClass)
            {
              if (record->m_nClass == 1)
              {
                defaultValue = g_AggroCaculateData.GetDefault(2);
              }
              else
              {
                defaultValue = g_AggroCaculateData.GetDefault(1);
              }
            }
            else
            {
              defaultValue = g_AggroCaculateData.GetDefault(3);
            }
            break;
          }

          defaultValue = g_AggroCaculateData.GetDefault(6);
          break;
        }

        defaultValue = g_AggroCaculateData.GetDefault(5);
        break;
      }
      case 1:
      {
        const int specialValue = g_AggroCaculateData.GetSpecialData(1, static_cast<unsigned __int16>(dwAttackSerial));
        if (specialValue)
        {
          defaultValue = specialValue;
          break;
        }

        _force_fld *record = static_cast<_force_fld *>(g_Main.m_tblEffectData[1].GetRecord(static_cast<int>(dwAttackSerial)));
        if (!record)
        {
          break;
        }

        if (record->m_nContEffectType)
        {
          if (record->m_nContEffectType != 1 || !bOtherPlayerSupport)
          {
            defaultValue = g_AggroCaculateData.GetDefault(1);
            break;
          }

          defaultValue = g_AggroCaculateData.GetDefault(6);
          break;
        }

        defaultValue = g_AggroCaculateData.GetDefault(5);
        break;
      }
      case 2:
      {
        const int specialValue = g_AggroCaculateData.GetSpecialData(2, static_cast<unsigned __int16>(dwAttackSerial));
        if (specialValue)
        {
          defaultValue = specialValue;
          break;
        }

        _skill_fld *record = static_cast<_skill_fld *>(g_Main.m_tblEffectData[2].GetRecord(static_cast<int>(dwAttackSerial)));
        if (record)
        {
          if (!record->m_nContEffectType)
          {
            defaultValue = g_AggroCaculateData.GetDefault(5);
            break;
          }

          if (record->m_nContEffectType != 1 || !bOtherPlayerSupport)
          {
            defaultValue = g_AggroCaculateData.GetDefault(4);
            break;
          }

          defaultValue = g_AggroCaculateData.GetDefault(6);
        }
        break;
      }
      default:
        break;
    }
  }
  m_nAggroData += defaultValue;
  if (bFirstAttack)
  {
    m_nAggroData += g_AggroCaculateData.GetDefault(9);
  }
  m_nAggroData = static_cast<int>(static_cast<float>(m_nAggroData)
                                  + static_cast<float>(m_nAggroData) * fAdd);
}

int CAggroNode::IsLive()
{
  const bool isLive = m_dwObjectSerial != static_cast<unsigned int>(-1) && m_pCharacter && m_pCharacter->m_bLive
                   && !m_pCharacter->m_bCorpse;
  return isLive ? 1 : 0;
}
