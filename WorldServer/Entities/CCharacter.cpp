#include "pch.h"

#include "CCharacter.h"

#include <cstring>
#include <mmsystem.h>

#include "ObjectCreateSetData.h"
#include "CRecordData.h"
#include "GlobalObjects.h"
#include "base_fld.h"
#include "WorldServerUtil.h"

CMyTimer::~CMyTimer() = default;

unsigned int _sf_continous::GetSFContCurTime()
{
  return timeGetTime() / 1000;
}

void _effect_parameter::AllocEffParam()
{
  if (m_pDataParam == nullptr)
  {
    m_pDataParam = new __param_data();
  }
  InitEffParam();
}

void _effect_parameter::InitEffParam()
{
  m_bLock = false;
  if (m_pDataParam == nullptr)
  {
    return;
  }
  for (int j = 0; j < 62; ++j)
  {
    m_pDataParam->m_fEff_Rate[j] = 1.0f;
  }
  for (int j = 0; j < 42; ++j)
  {
    m_pDataParam->m_fEff_Plus[j] = 0.0f;
  }
  for (int j = 0; j < 29; ++j)
  {
    m_pDataParam->m_bEff_State[j] = 0;
  }
  for (int j = 0; j < 83; ++j)
  {
    m_pDataParam->m_fEff_Have[j] = 0.0f;
  }
}

void _effect_parameter::InitEffHave()
{
  if (!m_pDataParam)
  {
    return;
  }

  for (int j = 0; j < 83; ++j)
  {
    m_pDataParam->m_fEff_Have[j] = 0.0f;
  }
}

bool _effect_parameter::GetEff_State(unsigned int nParamIndex)
{
  if (!m_pDataParam)
    return false;
  if (m_bLock)
    return false;
  if (nParamIndex <= 0x1C)
    return m_pDataParam->m_bEff_State[nParamIndex] > 0;
  return false;
}

float _effect_parameter::GetEff_Rate(unsigned int nParamIndex)
{
  if (!m_pDataParam)
  {
    return FLOAT_1_0;
  }
  if (m_bLock && nParamIndex >= 41 && nParamIndex <= 44)
  {
    return m_pDataParam->m_fEff_Rate[nParamIndex];
  }
  if (m_bLock)
  {
    return FLOAT_1_0;
  }
  if (nParamIndex < 0x3E)
  {
    return m_pDataParam->m_fEff_Rate[nParamIndex];
  }
  return FLOAT_1_0;
}

float _effect_parameter::GetEff_Plus(unsigned int nParamIndex)
{
  if (!m_pDataParam)
  {
    return 0.0f;
  }
  if (m_bLock)
  {
    return 0.0f;
  }
  if (nParamIndex < 0x2A)
  {
    return m_pDataParam->m_fEff_Plus[nParamIndex];
  }
  return 0.0f;
}

float _effect_parameter::GetEff_Have(unsigned int nParamIndex)
{
  if (!m_pDataParam)
  {
    return 0.0f;
  }
  if (m_bLock)
  {
    return 0.0f;
  }
  if (nParamIndex <= 0x52)
  {
    return m_pDataParam->m_fEff_Have[nParamIndex];
  }
  return 0.0f;
}

bool _effect_parameter::SetEff_Rate(unsigned int nParamIndex, float fVar, bool bAdd)
{
  if (!m_pDataParam)
    return false;
  if (nParamIndex >= 0x3E)
    return false;
  m_pDataParam->m_fEff_Rate[nParamIndex] =
    bAdd ? (m_pDataParam->m_fEff_Rate[nParamIndex] + fVar)
         : (m_pDataParam->m_fEff_Rate[nParamIndex] - fVar);
  return true;
}

bool _effect_parameter::SetEff_Plus(unsigned int nParamIndex, float fVar, bool bAdd)
{
  if (!m_pDataParam)
    return false;
  if (nParamIndex >= 0x2A)
    return false;
  m_pDataParam->m_fEff_Plus[nParamIndex] =
    bAdd ? (m_pDataParam->m_fEff_Plus[nParamIndex] + fVar)
         : (m_pDataParam->m_fEff_Plus[nParamIndex] - fVar);
  return true;
}

bool _effect_parameter::SetEff_State(unsigned int nParamIndex, bool bVar)
{
  if (!m_pDataParam)
    return false;
  if (nParamIndex > 0x1C)
    return false;
  m_pDataParam->m_bEff_State[nParamIndex] =
    bVar ? static_cast<char>(m_pDataParam->m_bEff_State[nParamIndex] + 1)
         : static_cast<char>(m_pDataParam->m_bEff_State[nParamIndex] - 1);
  return true;
}

void CCharacter::Init(_object_id *pID)
{
  CGameObject::Init(pID);
  m_fTarPos[0] = 0.0f;
  m_fTarPos[1] = 0.0f;
  m_fTarPos[2] = 0.0f;
  m_AroundNum = 0;
  for (int j = 0; j < 5; ++j)
  {
    m_AroundSlot[j] = nullptr;
  }
  m_dwNextGenAttackTime = static_cast<unsigned int>(-1);
  m_dwEffSerialCounter = 0;
  m_wLastContEffect = static_cast<unsigned __int16>(-1);
  m_wEffectTempValue = 0;
  m_dwPlayerSerial = 0;
  std::memset(m_wszPlayerName, 0, sizeof(m_wszPlayerName));
  m_nContEffectSec = -1;
  m_EP.InitEffParam();
  std::memset(m_SFCont, 0, sizeof(m_SFCont));
  std::memset(m_SFContAura, 0, sizeof(m_SFContAura));
  m_bLastContEffectUpdate = false;
  m_wLastContEffect = static_cast<unsigned __int16>(-1);
}

void CCharacter::Stop()
{
  memcpy_0(m_fTarPos, m_fCurPos, sizeof(m_fTarPos));
  if (m_bMove)
  {
    m_bMove = false;
  }
}

bool CCharacter::Create(_character_create_setdata *pData)
{
  if (!CGameObject::Create(pData))
  {
    return false;
  }

  memcpy_0(m_fTarPos, m_fCurPos, sizeof(m_fTarPos));
  m_dwNextGenAttackTime = GetLoopTime();
  m_nContEffectSec = -1;
  m_tmrSFCont.BeginTimer(1000);
  m_wEffectTempValue = 0;
  m_dwEffSerialCounter = 1;
  memset_0(m_SFCont, 0, sizeof(m_SFCont));
  memset_0(m_SFContAura, 0, sizeof(m_SFContAura));
  m_bLastContEffectUpdate = false;
  m_wLastContEffect = static_cast<unsigned __int16>(-1);
  return true;
}

bool CCharacter::Destroy()
{
  m_EP.InitEffParam();
  memset_0(m_SFCont, 0, sizeof(m_SFCont));
  memset_0(m_SFContAura, 0, sizeof(m_SFContAura));
  m_bLastContEffectUpdate = false;
  m_wLastContEffect = static_cast<unsigned __int16>(-1);
  return CGameObject::Destroy() != 0;
}

bool CCharacter::SetTarPos(float *fTarPos, bool bColl)
{
  if (!m_pCurMap->IsMapIn(fTarPos))
  {
    return false;
  }

  float crossPos[3]{};
  if (!bColl || m_pCurMap->m_Level.mBsp->CanYouGoThere(m_fCurPos, fTarPos, reinterpret_cast<float(*)[3]>(crossPos)))
  {
    memcpy_0(m_fTarPos, fTarPos, sizeof(m_fTarPos));
  }
  else
  {
    crossPos[1] = m_fCurPos[1];
    memcpy_0(m_fTarPos, crossPos, sizeof(m_fTarPos));
  }

  Go();
  ResetSlot();
  return true;
}

void CCharacter::Go()
{
  if (!m_bMove)
  {
    m_bMove = true;
  }
}

void CCharacter::ResetSlot()
{
  if (m_AroundNum)
  {
    memset_0(m_AroundSlot, 0, sizeof(m_AroundSlot));
    m_AroundNum = 0;
  }
}

void CCharacter::RemoveSFContEffect(
  unsigned __int8 byContCode,
  unsigned __int16 wListIndex,
  bool bInit,
  bool bAura)
{
  _sf_continous *contList = bAura ? m_SFContAura[byContCode] : m_SFCont[byContCode];
  _sf_continous *cont = &contList[wListIndex];
  if (!cont->m_bExist)
    return;

  auto *record = reinterpret_cast<_base_fld *>(
    g_Main.m_tblEffectData[cont->m_byEffectCode].GetRecord(cont->m_wEffectIndex));
  char *effectParamBase = nullptr;
  if (cont->m_byEffectCode == 1)
    effectParamBase = &record[12].m_strCode[56];
  else
    effectParamBase = &record[13].m_strCode[48];

  if (effectParamBase)
  {
    for (int j = 0; j < 5; ++j)
    {
      char *paramEntry = &effectParamBase[36 * j];
      int effectType = *reinterpret_cast<int *>(paramEntry);
      if (effectType == -1)
        break;

      const int paramIndex = *reinterpret_cast<int *>(paramEntry + 4);
      if (effectType == 0)
      {
        const float value = *reinterpret_cast<float *>(paramEntry + 4 * cont->m_byLv + 4);
        m_EP.SetEff_Rate(static_cast<unsigned int>(paramIndex), value, false);
      }
      else if (effectType == 1)
      {
        const float value = *reinterpret_cast<float *>(paramEntry + 4 * cont->m_byLv + 4);
        m_EP.SetEff_Plus(static_cast<unsigned int>(paramIndex), value, false);
      }
      else if (effectType == 2)
      {
        m_EP.SetEff_State(static_cast<unsigned int>(paramIndex), false);
      }
    }
  }

  cont->m_bExist = false;
  SFContDelMessage(byContCode, static_cast<unsigned __int8>(wListIndex), !bInit, bAura);
  m_bLastContEffectUpdate = true;
}

void CCharacter::RemoveSFContHelpByEffect(int nContParamCode, int nContParamIndex)
{
  for (int j = 0; j < 8; ++j)
  {
    _sf_continous *cont = &m_SFCont[1][j];
    if (!cont->m_bExist)
      continue;

    auto *record = reinterpret_cast<_base_fld *>(
      g_Main.m_tblEffectData[cont->m_byEffectCode].GetRecord(cont->m_wEffectIndex));
    char *effectParamBase = nullptr;
    if (cont->m_byEffectCode == 1)
      effectParamBase = &record[12].m_strCode[56];
    else
      effectParamBase = &record[13].m_strCode[48];

    if (!effectParamBase)
      continue;

    for (int k = 0; k < 5; ++k)
    {
      int *paramEntry = reinterpret_cast<int *>(effectParamBase + 36 * k);
      if (*paramEntry == -1)
        break;
      if (paramEntry[0] == nContParamCode && paramEntry[1] == nContParamIndex)
      {
        RemoveSFContEffect(1, static_cast<unsigned __int16>(j), false, false);
        break;
      }
    }
  }
}

unsigned int CCharacter::CalcEffectBit(unsigned __int16 wEffectCode, unsigned __int16 wEffectIndex)
{
  return static_cast<unsigned int>(wEffectIndex | (wEffectCode << 12));
}
