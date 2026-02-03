#include "pch.h"

#include "CCharacter.h"

#include <cstring>

#include "ObjectCreateSetData.h"

CMyTimer::~CMyTimer() = default;

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
