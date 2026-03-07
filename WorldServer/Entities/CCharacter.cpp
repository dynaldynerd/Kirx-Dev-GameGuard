#include "pch.h"

#include "CCharacter.h"

#include <cstring>
#include <cmath>
#include <mmsystem.h>

#include "ObjectCreateSetData.h"
#include "CAttack.h"
#include "CMapData.h"
#include "CObjectList.h"
#include "CMonster.h"
#include "CMonsterAggroMgr.h"
#include "CPlayer.h"
#include "CRecordData.h"
#include "GlobalObjects.h"
#include "base_fld.h"
#include "force_fld.h"
#include "skill_fld.h"
#include "WorldServerUtil.h"
#include "CBsp.h"
#include "CMapOperation.h"
#include "Packet/ZoneClientPacket.h"
#include "pnt_rect.h"

CMyTimer::~CMyTimer() = default;

int _effect_parameter::m_nInitCount;

_sf_continous::_sf_continous()
{
  m_bExist = false;
}

_effect_parameter::_effect_parameter()
{
  m_pDataParam = nullptr;
  m_bLock = false;
  ++m_nInitCount;
}

_effect_parameter::~_effect_parameter()
{
  if (m_pDataParam)
  {
    operator delete(m_pDataParam);
  }
}

CCharacter::CCharacter()
{
}

CCharacter::~CCharacter()
{
  // this is not a stub
}

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

void _effect_parameter::SetLock(bool bLock)
{
  m_bLock = bLock;
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

float CCharacter::CalcDistForSec(float fSec, float fSpeed)
{
  return (fSec * 15.0f) * fSpeed;
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

unsigned int CCharacter::GetNextGenAttTime()
{
  return m_dwNextGenAttackTime;
}

void CCharacter::SetNextGenAttTime(unsigned int dwNextTime)
{
  m_dwNextGenAttackTime = dwNextTime;
}

__int64 CCharacter::GetSlot(CCharacter *p)
{
  for (int i = 0; i < 5; ++i)
  {
    if (m_AroundSlot[i] == p)
    {
      return static_cast<unsigned int>(i);
    }
  }
  return 0xFFFFFFFFLL;
}

bool CCharacter::GetStealth(bool bInvisible)
{
  if (m_bCorpse)
  {
    return false;
  }
  if (m_bBreakTranspar)
  {
    return false;
  }
  if (m_EP.GetEff_Plus(21) > 0.0f || m_EP.GetEff_State(5))
  {
    return true;
  }
  return bInvisible && m_EP.GetEff_State(26);
}

bool CCharacter::GetInvisible()
{
  if (m_bCorpse)
  {
    return false;
  }
  if (m_bBreakTranspar)
  {
    return false;
  }
  return m_EP.GetEff_State(26);
}

void CCharacter::SFContInit()
{
  for (int contCode = 0; contCode < 2; ++contCode)
  {
    for (int slot = 0; slot < 8; ++slot)
    {
      if (m_SFCont[contCode][slot].m_bExist)
      {
        RemoveSFContEffect(static_cast<unsigned __int8>(contCode), static_cast<unsigned __int16>(slot), true, false);
      }
      if (m_SFContAura[contCode][slot].m_bExist)
      {
        RemoveSFContEffect(static_cast<unsigned __int8>(contCode), static_cast<unsigned __int16>(slot), true, true);
      }
    }
  }

  m_bLastContEffectUpdate = false;
  m_wLastContEffect = static_cast<unsigned __int16>(-1);
  SendMsg_LastEffectChangeInform();
}

char CCharacter::RemoveAllContinousEffect()
{
  char removed = 0;
  for (unsigned int effectCodeType = 0; effectCodeType < 2; ++effectCodeType)
  {
    if (RemoveAllContinousEffectGroup(effectCodeType))
    {
      removed = 1;
    }
  }
  return removed;
}

bool CCharacter::RemoveAllContinousEffectGroup(unsigned int uiEffectCodeType)
{
  int removeCount = 0;
  for (int j = 0; j < 8; ++j)
  {
    _sf_continous *cont = &m_SFCont[uiEffectCodeType][j];
    if (cont->m_bExist)
    {
      RemoveSFContEffect(static_cast<unsigned __int8>(uiEffectCodeType), static_cast<unsigned __int16>(j), false, false);
      ++removeCount;
    }
  }
  return removeCount > 0;
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

  _cont_param_list *effectParamList = nullptr;
  if (cont->m_byEffectCode == 1)
  {
    _force_fld *record = static_cast<_force_fld *>(
      g_Main.m_tblEffectData[cont->m_byEffectCode].GetRecord(cont->m_wEffectIndex));
    if (record)
    {
      effectParamList = record->m_ContParamList;
    }
  }
  else
  {
    _skill_fld *record = static_cast<_skill_fld *>(
      g_Main.m_tblEffectData[cont->m_byEffectCode].GetRecord(cont->m_wEffectIndex));
    if (record)
    {
      effectParamList = record->m_ContParamList;
    }
  }

  if (effectParamList)
  {
    for (int j = 0; j < 5; ++j)
    {
      _cont_param_list *paramEntry = &effectParamList[j];
      int effectType = paramEntry->m_nContParamCode;
      if (effectType == -1)
        break;

      const int paramIndex = paramEntry->m_nContParamIndex;
      if (effectType == 0)
      {
        const float value = paramEntry->m_fContValue[cont->m_byLv];
        m_EP.SetEff_Rate(static_cast<unsigned int>(paramIndex), value, false);
      }
      else if (effectType == 1)
      {
        const float value = paramEntry->m_fContValue[cont->m_byLv];
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

void CCharacter::UpdateSFCont()
{
  if (!m_tmrSFCont.CountingTimer())
  {
    return;
  }

  const unsigned int sfContCurTime = _sf_continous::GetSFContCurTime();
  for (int contCode = 0; contCode < 2; ++contCode)
  {
    for (int slot = 0; slot < 8; ++slot)
    {
      _sf_continous *cont = &m_SFCont[contCode][slot];
      if (!cont->m_bExist)
      {
        continue;
      }

      const unsigned int elapsed = sfContCurTime - cont->m_dwStartSec;
      if (elapsed < cont->m_wDurSec)
      {
        const int leftTime = static_cast<int>(cont->m_wDurSec - elapsed);
        SFContUpdateTimeMessage(
          static_cast<unsigned __int8>(contCode),
          static_cast<unsigned __int8>(slot),
          leftTime);
      }
      else
      {
        RemoveSFContEffect(static_cast<unsigned __int8>(contCode), static_cast<unsigned __int16>(slot), false, false);
      }
    }
  }

  if (m_bLastContEffectUpdate)
  {
    unsigned int lastEffSerial = 0;
    _sf_continous *lastCont = nullptr;
    for (int contCode = 0; contCode < 2; ++contCode)
    {
      for (int slot = 0; slot < 8; ++slot)
      {
        _sf_continous *cont = &m_SFCont[contCode][slot];
        if (cont->m_bExist && lastEffSerial <= cont->m_dwEffSerial)
        {
          lastEffSerial = cont->m_dwEffSerial;
          lastCont = cont;
        }
      }
    }

    const unsigned __int16 oldLastContEffect = m_wLastContEffect;
    if (lastCont)
    {
      m_wLastContEffect =
        static_cast<unsigned __int16>(CalcEffectBit(lastCont->m_byEffectCode, lastCont->m_wEffectIndex));
    }
    else
    {
      m_wLastContEffect = static_cast<unsigned __int16>(-1);
    }

    if (oldLastContEffect != m_wLastContEffect)
    {
      SendMsg_LastEffectChangeInform();
    }
    m_bLastContEffectUpdate = false;
  }

  if (!m_ObjID.m_byID)
  {
    static_cast<CPlayer *>(this)->UpdateAuraSFCont();
  }
}

void CCharacter::AlterContDurSec(
  unsigned __int8 byContCode,
  unsigned __int16 wListIndex,
  unsigned int dwStartSec,
  unsigned __int16 wNewDur)
{
  _sf_continous *cont = &m_SFCont[byContCode][wListIndex];
  if (cont->m_bExist)
  {
    cont->m_dwStartSec = dwStartSec;
    cont->m_wDurSec = wNewDur;
  }
}

void CCharacter::RemoveSFContHelpByEffect(int nContParamCode, int nContParamIndex)
{
  for (int j = 0; j < 8; ++j)
  {
    _sf_continous *cont = &m_SFCont[1][j];
    if (!cont->m_bExist)
      continue;

    _cont_param_list *effectParamList = nullptr;
    if (cont->m_byEffectCode == 1)
    {
      _force_fld *record = static_cast<_force_fld *>(
        g_Main.m_tblEffectData[cont->m_byEffectCode].GetRecord(cont->m_wEffectIndex));
      if (record)
      {
        effectParamList = record->m_ContParamList;
      }
    }
    else
    {
      _skill_fld *record = static_cast<_skill_fld *>(
        g_Main.m_tblEffectData[cont->m_byEffectCode].GetRecord(cont->m_wEffectIndex));
      if (record)
      {
        effectParamList = record->m_ContParamList;
      }
    }

    if (!effectParamList)
      continue;

    for (int k = 0; k < 5; ++k)
    {
      _cont_param_list *paramEntry = &effectParamList[k];
      if (paramEntry->m_nContParamCode == -1)
        break;
      if (paramEntry->m_nContParamCode == nContParamCode && paramEntry->m_nContParamIndex == nContParamIndex)
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

void CCharacter::Move(float fSpeed)
{
  if (m_bMove)
  {
    memcpy_0(m_fOldPos, m_fCurPos, sizeof(m_fOldPos));
    const int yAngle = static_cast<int>(GetYAngle(m_fCurPos, m_fTarPos));
    const int distance = static_cast<int>(GetSqrt(m_fCurPos, m_fTarPos));
    const float moveLen = (R3GetLoopTime() * 15.0f) * fSpeed;
    float src[3]{};
    memcpy_0(src, m_fCurPos, sizeof(src));
    if (moveLen < static_cast<float>(distance))
    {
      const double angle = 6.283185307 * static_cast<double>(yAngle) / 65535.0;
      m_fCurPos[0] = m_fCurPos[0] - static_cast<float>(sin_0(angle) * moveLen);
      m_fCurPos[2] = m_fCurPos[2] - static_cast<float>(cos_0(angle) * moveLen);
      m_fCurPos[1] = src[1];
      if (!m_pCurMap->m_Level.GetNextYposForServerFar(src, m_fCurPos, &m_fCurPos[1]))
      {
        if (!m_pCurMap->m_Level.GetNextYposFarProgress(src, m_fCurPos, &m_fCurPos[1]))
        {
          memcpy_0(m_fCurPos, src, sizeof(m_fCurPos));
          memcpy_0(m_fTarPos, m_fCurPos, sizeof(m_fTarPos));
          Stop();
          SendMsg_BreakStop();
        }
      }
    }
    else
    {
      m_fTarPos[1] = m_fCurPos[1];
      if (m_pCurMap->m_Level.GetNextYposForServer(m_fTarPos, &m_fTarPos[1])
          || m_pCurMap->m_Level.GetNextYposForServerFar(m_fCurPos, m_fTarPos, &m_fTarPos[1]))
      {
        memcpy_0(m_fCurPos, m_fTarPos, sizeof(m_fCurPos));
      }
      else
      {
        memcpy_0(m_fCurPos, src, sizeof(m_fCurPos));
        memcpy_0(m_fTarPos, m_fCurPos, sizeof(m_fTarPos));
      }
      Stop();
    }
  }
  else
  {
    m_bMove = false;
  }
}

void CCharacter::MoveBreak(float fSpeed)
{
  const int slotIndex = m_nTotalObjIndex % 10;
  const int loopStartPoint = g_MapOper.m_nLoopStartPoint;
  int offset = 0;
  if (g_MapOper.m_nLoopStartPoint != slotIndex)
  {
    if (loopStartPoint <= slotIndex)
    {
      offset = 10 - (slotIndex - loopStartPoint);
    }
    else
    {
      offset = loopStartPoint - slotIndex;
    }
    if (offset > 0)
    {
      Move(fSpeed * (static_cast<float>(offset) / 10.0f));
    }
  }
}

__int64 CCharacter::RemoveSlot(CCharacter *p)
{
  for (int j = 0; j < 5; ++j)
  {
    if (m_AroundSlot[j] == p)
    {
      m_AroundSlot[j] = nullptr;
      --m_AroundNum;
      return 1;
    }
  }
  return 0;
}

__int64 CCharacter::GetNearEmptySlot(unsigned int pos, float dist, float *cur, float *target)
{
  unsigned int index = pos;
  if (pos > 4)
  {
    index = 4;
  }

  const double pi = 3.1415926535;
  int nextIndex = static_cast<int>(index);
  int prevIndex = static_cast<int>(index);
  for (int j = 0; j < 5; ++j)
  {
    if (nextIndex >= 5)
    {
      nextIndex = 0;
    }
    if (prevIndex < 0)
    {
      prevIndex = 4;
    }

    if (!m_AroundSlot[nextIndex])
    {
      float checkPos[3]{};
      memcpy_0(checkPos, target, sizeof(checkPos));
      const float angle = static_cast<float>(2.0 * pi * static_cast<float>(nextIndex + 1) / 5.0);
      checkPos[0] = checkPos[0] + static_cast<float>(std::cos(angle) * dist);
      checkPos[2] = checkPos[2] + static_cast<float>(std::sin(angle) * dist);
      float scratch[3]{};
      if (m_pCurMap->m_Level.mBsp->CanYouGoThere(cur, checkPos, &scratch))
      {
        return static_cast<unsigned int>(nextIndex);
      }
    }

    if (!m_AroundSlot[prevIndex])
    {
      float checkPos[3]{};
      memcpy_0(checkPos, target, sizeof(checkPos));
      const float angle = static_cast<float>(2.0 * pi * static_cast<float>(prevIndex + 1) / 5.0);
      checkPos[0] = checkPos[0] + static_cast<float>(std::cos(angle) * dist);
      checkPos[2] = checkPos[2] + static_cast<float>(std::sin(angle) * dist);
      float scratch[3]{};
      if (m_pCurMap->m_Level.mBsp->CanYouGoThere(cur, checkPos, &scratch))
      {
        return static_cast<unsigned int>(prevIndex);
      }
    }

    ++nextIndex;
    --prevIndex;
  }
  return 0xFFFFFFFFu;
}

__int64 CCharacter::InsertSlot(CCharacter *p, int pos)
{
  if (m_AroundSlot[pos] == p)
  {
    return 1;
  }
  if (m_AroundSlot[pos])
  {
    return 0;
  }
  m_AroundSlot[pos] = p;
  ++m_AroundNum;
  return 1;
}

void CCharacter::BreakStealth()
{
  if (m_EP.GetEff_State(5) || m_EP.GetEff_Plus(21) > 0.0f || m_EP.GetEff_State(26))
  {
    SetBreakTranspar(true);
  }
}

__int64 CCharacter::GetTotalTol(unsigned __int8 byAttTolType, int nDamPoint)
{
  const float damagePoint = static_cast<float>(nDamPoint);
  const float fireTol = static_cast<float>(GetFireTol()) / 100.0f;
  const float waterTol = static_cast<float>(GetWaterTol()) / 100.0f;
  const float soilTol = static_cast<float>(GetSoilTol()) / 100.0f;
  const float windTol = static_cast<float>(GetWindTol()) / 100.0f;
  const float lowRate = 0.1f;
  const float highRate = 1.0f - 0.1f;

  if (!byAttTolType)
  {
    return static_cast<unsigned int>(
      static_cast<int>(((damagePoint * -fireTol) * lowRate) - ((damagePoint * waterTol) * highRate)
                       + ((damagePoint * soilTol) * highRate)));
  }

  switch (byAttTolType)
  {
    case 1:
      return static_cast<unsigned int>(
        static_cast<int>(((damagePoint * fireTol) * highRate) - ((damagePoint * waterTol) * lowRate)
                         - ((damagePoint * windTol) * highRate)));
    case 2:
      return static_cast<unsigned int>(
        static_cast<int>(((damagePoint * -fireTol) * highRate) - ((damagePoint * soilTol) * lowRate)
                         + ((damagePoint * windTol) * highRate)));
    case 3:
      return static_cast<unsigned int>(
        static_cast<int>(((damagePoint * waterTol) * highRate) - ((damagePoint * soilTol) * highRate)
                         - ((damagePoint * windTol) * lowRate)));
    default:
      break;
  }

  return 0;
}

__int64 CCharacter::GetAttackDamPoint(int nAttPnt, int nAttPart, int nTolType, CCharacter *pDst, bool bBackAttack)
{
  float defFc = 0.0f;
  const int totalTol = static_cast<int>(GetTotalTol(static_cast<unsigned __int8>(nTolType), nAttPnt));
  int outPart[5]{};
  outPart[0] = nAttPart;

  if (pDst->m_EP.GetEff_State(12))
  {
    pDst->SetAttackPart(nAttPart);
  }
  else
  {
    defFc = static_cast<float>(pDst->GetDefFC(nAttPart, this, outPart));
    if (pDst->m_ObjID.m_byID == 1)
    {
      int outValue[4]{};
      outValue[0] = -1;
      if (reinterpret_cast<CMonster *>(pDst)->GetViewAngleCap(1, outValue))
      {
        float capRate = static_cast<float>(outValue[0]);
        if (capRate > 0.0f)
        {
          capRate = capRate / 100.0f;
          if (capRate <= 1.0f)
          {
            defFc = defFc * (1.0f - capRate);
          }
        }
      }
    }
  }

  if (defFc == -2.0f)
  {
    return 0xFFFFFFFEu;
  }

  float defFacing = pDst->GetDefFacing(outPart[0]);
  float defGap = pDst->GetDefGap(outPart[0]);
  if (outPart[0] == 5)
  {
    const float facing = pDst->GetDefFacing(nAttPart);
    if (defFacing > facing)
    {
      defFacing = pDst->GetDefFacing(nAttPart);
    }
    const float gap = pDst->GetDefGap(nAttPart);
    if (gap > defGap)
    {
      defGap = pDst->GetDefGap(nAttPart);
    }
  }

  if (defFc < 1.0f)
  {
    defFc = 1.0f;
  }

  const float weaponAdjust = GetWeaponAdjust();
  const float avgGap = (weaponAdjust + defGap) / 2.0f;
  const float facingRate = defFacing - 1.0f;
  const float valueA = (facingRate == 0.0f)
                         ? 0.0f
                         : ((defFacing * defFc) * defGap - defFc) / facingRate;
  const float valueB = valueA - (defFc * avgGap);
  const float valueC = (valueB == 0.0f) ? 0.0f : (valueA - defFc) / valueB;
  float tolRate = 1.2f;
  if (nTolType == -1)
  {
    tolRate = 1.0f;
  }

  int damage = static_cast<int>(((static_cast<float>(nAttPnt) * tolRate + static_cast<float>(totalTol))
                                 - (defFc * avgGap))
                                * valueC);
  if (damage < 1)
  {
    damage = 1;
  }

  if (!pDst->m_ObjID.m_byID && damage <= 300)
  {
    if (pDst->GetLevel() >= 30)
    {
      return static_cast<unsigned int>(rand() % 300 + 1);
    }
  }
return static_cast<unsigned int>(damage);
}

__int64 CCharacter::GetAttackRandomPart()
{
  if (!m_ObjID.m_byID)
  {
    const CPlayer *player = static_cast<const CPlayer *>(this);
    if (player->m_byDamagePart != static_cast<unsigned __int8>(-1))
    {
      return player->m_byDamagePart;
    }
  }

  int threshold[5]{};
  threshold[0] = 23;
  threshold[1] = 45;
  threshold[2] = 63;
  threshold[3] = 80;
  threshold[4] = 100;
  const int roll = rand() % 100;
  for (int j = 0; j < 5; ++j)
  {
    if (roll < threshold[j])
    {
      return static_cast<unsigned int>(j);
    }
  }
  return 0xFFFFFFFFLL;
}

void CCharacter::SendMsg_AttackActEffect(unsigned __int8 byActEffect, CCharacter *pDamer)
{
  _count_succ_inform_zocl msg{};
  msg.byActEffectCode = static_cast<char>(byActEffect);
  msg.byAtterID = static_cast<char>(m_ObjID.m_byID);
  msg.dwAtterSerial = m_dwObjSerial;
  msg.byDamerID = static_cast<char>(pDamer->m_ObjID.m_byID);
  msg.dwDamerSerial = pDamer->m_dwObjSerial;

  unsigned __int8 type[2] = {5, 24};
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(sizeof(msg)), true);
}

void CCharacter::SendMsg_LastEffectChangeInform()
{
  _last_effect_change_inform_zocl msg{};
  msg.byObjID = static_cast<char>(m_ObjID.m_byID);
  msg.dwSerial = m_dwObjSerial;
  msg.wLastContEffect = m_wLastContEffect;

  unsigned __int8 type[2] = {17, 17};
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(sizeof(msg)), true);
}

void CCharacter::SendMsg_RobedHP(const CCharacter *pkPerform, unsigned __int16 wRobedHP)
{
  _robed_hp_inform_zocl msg{};
  msg.idDster.byID = static_cast<char>(m_ObjID.m_byID);
  msg.idDster.wIndex = m_ObjID.m_wIndex;
  msg.idDster.dwSerial = m_dwObjSerial;
  msg.idPerformer.byID = static_cast<char>(pkPerform->m_ObjID.m_byID);
  msg.idPerformer.wIndex = pkPerform->m_ObjID.m_wIndex;
  msg.idPerformer.dwSerial = pkPerform->m_dwObjSerial;
  msg.wRobedHP = wRobedHP;

  unsigned __int8 type[2] = {17, 31};
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(sizeof(msg)), false);
}

int _CheckCumulativeSF(
  unsigned __int8 byEffectCode,
  int dwEffectIndex,
  int *nCumulMax,
  unsigned int *nEffectCount,
  char **pstrLinkCode)
{
  if (byEffectCode == 1)
  {
    _force_fld *record = static_cast<_force_fld *>(g_Main.m_tblEffectData[byEffectCode].GetRecord(dwEffectIndex));
    if (!record)
    {
      return 0;
    }
    *nEffectCount = static_cast<unsigned int>(record->m_nNewEffCount);
    *nCumulMax = record->m_nCumulCounter;
    *pstrLinkCode = record->m_strEffectCode;
    return static_cast<unsigned int>(record->m_bCumulType);
  }

  if (!byEffectCode || (byEffectCode > 1 && byEffectCode <= 3))
  {
    _skill_fld *record = static_cast<_skill_fld *>(g_Main.m_tblEffectData[byEffectCode].GetRecord(dwEffectIndex));
    if (!record)
    {
      return 0;
    }

    *nEffectCount = static_cast<unsigned int>(record->m_nNewEffCount);
    *nCumulMax = record->m_nCumulCounter;
    *pstrLinkCode = record->m_strEffectCode;
    return static_cast<unsigned int>(record->m_bCumulType);
  }

  return 0;
}

__int64 CCharacter::FindEffectDst(
  int nEffectCode,
  int nAreaType,
  int nLv,
  bool bBenefit,
  CCharacter *pOriDst,
  char *psActableDst,
  CCharacter **ppDsts)
{
  unsigned int count = 0;
  ppDsts[count++] = pOriDst;
  if (!nAreaType)
  {
    return count;
  }

  switch (nAreaType)
  {
    case 1:
      return static_cast<unsigned int>(
        _GetAreaEffectMember(pOriDst, bBenefit, s_nLimitRadius_0[nLv], pOriDst->m_fCurPos, psActableDst, &ppDsts[count]))
             + count;
    case 2:
      return static_cast<unsigned int>(_GetPartyEffectMember(pOriDst, false, &ppDsts[count])) + count;
    case 3:
      return static_cast<unsigned int>(_GetPartyEffectMember(pOriDst, true, &ppDsts[count])) + count;
    case 10:
      return static_cast<unsigned int>(_GetFlashEffectMember(
                                          pOriDst,
                                          bBenefit,
                                          s_nLimitRadius_0[nLv],
                                          s_nLimitAngle_0,
                                          pOriDst,
                                          psActableDst,
                                          &ppDsts[count]))
             + count;
    default:
      break;
  }

  return count;
}

__int64 CCharacter::FindPotionEffectDst(
  int nAreaType,
  int nEffectAreaVal,
  bool bBenefit,
  CCharacter *pOriDst,
  char *psActableDst,
  CCharacter **ppDsts,
  bool *pbPath)
{
  unsigned int count = 0;
  ppDsts[count++] = pOriDst;

  switch (nAreaType)
  {
    case 0:
      return count;
    case 1:
      return static_cast<unsigned int>(
        _GetAreaEffectMember(pOriDst, bBenefit, nEffectAreaVal, pOriDst->m_fCurPos, psActableDst, &ppDsts[count]))
             + count;
    case 2:
      return static_cast<unsigned int>(_GetPartyEffectMember(pOriDst, false, &ppDsts[count])) + count;
    case 3:
      return static_cast<unsigned int>(_GetPartyEffectMember(pOriDst, true, &ppDsts[count])) + count;
    case 4:
      if (pOriDst == this)
      {
        return count;
      }
      ppDsts[count] = this;
      pbPath[count] = true;
      return count + 1;
    case 10:
      return static_cast<unsigned int>(_GetFlashEffectMember(
                                          pOriDst,
                                          bBenefit,
                                          nEffectAreaVal,
                                          s_nLimitAngle_1,
                                          pOriDst,
                                          psActableDst,
                                          &ppDsts[count]))
             + count;
    default:
      break;
  }

  return count;
}

__int64 CCharacter::_GetAreaEffectMember(
  CCharacter *pOriDst,
  bool bBenefit,
  int nLimitRadius,
  float *pTar,
  char *psActableDst,
  CCharacter **ppDsts)
{
  const int secNum = m_pCurMap->GetSectorIndex(pTar);
  if (secNum == -1)
  {
    return 0;
  }

  int count = 0;
  _pnt_rect rect{};
  m_pCurMap->GetRectInRadius(&rect, 1, secNum);
  for (int y = rect.nStarty; y <= rect.nEndy; ++y)
  {
    for (int x = rect.nStartx; x <= rect.nEndx; ++x)
    {
      _sec_info *secInfo = m_pCurMap->GetSecInfo();
      const unsigned int secIndex = secInfo->m_nSecNumW * y + x;
      CObjectList *sectorList = m_pCurMap->GetSectorListObj(m_wMapLayerIndex, secIndex);
      if (!sectorList)
      {
        continue;
      }

      for (_object_list_point *node = sectorList->m_Head.m_pNext; node != &sectorList->m_Tail; node = node->m_pNext)
      {
        CCharacter *pDst = static_cast<CCharacter *>(node->m_pItem);
        if (pOriDst == pDst || pDst->m_ObjID.m_byKind || !IsEffectableDst(psActableDst, pDst))
        {
          continue;
        }
        if (count >= 29)
        {
          return static_cast<unsigned int>(count);
        }
        if (pDst->m_bCorpse)
        {
          continue;
        }

        if (bBenefit)
        {
          const int dstRace = static_cast<int>(pDst->GetObjRace());
          const int srcRace = static_cast<int>(GetObjRace());
          if (dstRace != srcRace)
          {
            continue;
          }
          if (!pDst->m_ObjID.m_byID)
          {
            CPlayer *dstPlayer = static_cast<CPlayer *>(pDst);
            if (dstPlayer->IsChaosMode() || dstPlayer->IsPunished(1u, false))
            {
              continue;
            }
          }
        }
        else
        {
          const int dstRace = static_cast<int>(pDst->GetObjRace());
          const int srcRace = static_cast<int>(GetObjRace());
          if (dstRace == srcRace && !pDst->m_ObjID.m_byID)
          {
            CPlayer *dstPlayer = static_cast<CPlayer *>(pDst);
            if (!dstPlayer->IsPunished(1u, false) && !dstPlayer->IsChaosMode())
            {
              continue;
            }
          }

          if (!(IsAttackableInTown() || pDst->IsAttackableInTown() || (!IsInTown() && !pDst->IsInTown())))
          {
            continue;
          }
          if (!pDst->IsBeDamagedAble(this))
          {
            continue;
          }
        }

        const float heightDiff = std::fabs(m_fCurPos[1] - pDst->m_fCurPos[1]);
        if (heightDiff <= 350.0f)
        {
          float dist = GetSqrt(pTar, pDst->m_fCurPos);
          float width = pDst->GetWidth();
          float effective = 0.0f;
          if (dist - (width / 2.0f) <= 0.0f)
          {
            effective = 0.0f;
          }
          else
          {
            dist = GetSqrt(pTar, pDst->m_fCurPos);
            width = pDst->GetWidth();
            effective = dist - (width / 2.0f);
          }
          if (static_cast<int>(effective) <= nLimitRadius)
          {
            ppDsts[count++] = pDst;
          }
        }
      }
    }
  }

  return static_cast<unsigned int>(count);
}

__int64 CCharacter::_GetPartyEffectMember(CCharacter *pOriDst, bool bCircle, CCharacter **ppDsts)
{
// this is not a stub
  return 0;
}

__int64 CCharacter::_GetFlashEffectMember(
  CCharacter *pOriDst,
  bool bBenefit,
  int nLimitRadius,
  int nLimitAngle,
  CCharacter *pOriTar,
  char *psActableDst,
  CCharacter **ppDsts)
{
  if (!pOriTar)
  {
    return 0;
  }

  unsigned int count = 0;
  ppDsts[count++] = pOriTar;
  if (pOriTar->GetCurSecNum() == static_cast<unsigned int>(-1))
  {
    return count;
  }

  _pnt_rect rect{};
  const unsigned int curSecNum = pOriTar->GetCurSecNum();
  m_pCurMap->GetRectInRadius(&rect, 1, static_cast<int>(curSecNum));
  for (int y = rect.nStarty; y <= rect.nEndy; ++y)
  {
    for (int x = rect.nStartx; x <= rect.nEndx; ++x)
    {
      _sec_info *secInfo = m_pCurMap->GetSecInfo();
      const unsigned int secIndex = secInfo->m_nSecNumW * y + x;
      CObjectList *sectorList = m_pCurMap->GetSectorListObj(m_wMapLayerIndex, secIndex);
      if (!sectorList)
      {
        continue;
      }

      for (_object_list_point *node = sectorList->m_Head.m_pNext; node != &sectorList->m_Tail; node = node->m_pNext)
      {
        CCharacter *pDst = static_cast<CCharacter *>(node->m_pItem);
        if (pOriDst == pDst || pDst->m_ObjID.m_byKind || !IsEffectableDst(psActableDst, pDst))
        {
          continue;
        }
        if (static_cast<int>(count) >= 29)
        {
          return count;
        }
        if (pDst->m_bCorpse)
        {
          continue;
        }

        if (bBenefit)
        {
          const int dstRace = static_cast<int>(pDst->GetObjRace());
          const int srcRace = static_cast<int>(GetObjRace());
          if (dstRace != srcRace)
          {
            continue;
          }
        }
        else
        {
          const int dstRace = static_cast<int>(pDst->GetObjRace());
          const int srcRace = static_cast<int>(GetObjRace());
          if (dstRace == srcRace && !pDst->m_ObjID.m_byID)
          {
            CPlayer *dstPlayer = static_cast<CPlayer *>(pDst);
            if (!dstPlayer->IsPunished(1u, false) && !dstPlayer->IsChaosMode())
            {
              continue;
            }
          }
          if (!(IsAttackableInTown() || pDst->IsAttackableInTown() || (!IsInTown() && !pDst->IsInTown())))
          {
            continue;
          }
          if (!pDst->IsBeDamagedAble(this))
          {
            continue;
          }
        }

        const float heightDiff = std::fabs(m_fCurPos[1] - pDst->m_fCurPos[1]);
        if (heightDiff <= 350.0f)
        {
          if (CAttack::IsCharInSector(
                pDst->m_fCurPos,
                m_fCurPos,
                pOriTar->m_fCurPos,
                static_cast<float>(nLimitAngle),
                static_cast<float>(nLimitRadius)))
          {
            ppDsts[count++] = pDst;
          }
        }
      }
    }
  }

  return count;
}

char CCharacter::IsDamageEffect(unsigned int uiEffectCodeType, unsigned __int16 wEffectIndex)
{
  auto hasFlagAt = [](_base_fld *record, size_t recordIndex, size_t byteOffset) -> bool
  {
    if (!record)
    {
      return false;
    }
    const char *const base = reinterpret_cast<const char *>(record + recordIndex);
    return *reinterpret_cast<const int *>(base + byteOffset) != 0;
  };

  if (uiEffectCodeType == 0)
  {
    _base_fld *record = g_Main.m_tblEffectData[0].GetRecord(wEffectIndex);
    if (record && !hasFlagAt(record, 13, 32))
    {
      return 1;
    }
    return 0;
  }

  if (uiEffectCodeType == 1)
  {
    _base_fld *record = g_Main.m_tblEffectData[1].GetRecord(wEffectIndex);
    if (hasFlagAt(record, 12, 48))
    {
      return 1;
    }
    return 0;
  }

  if (uiEffectCodeType == 2)
  {
    _base_fld *record = g_Main.m_tblEffectData[2].GetRecord(wEffectIndex);
    if (hasFlagAt(record, 13, 32))
    {
      return 1;
    }
  }

  return 0;
}

char CCharacter::IsPotionEffectableDst(char *psActableDst, CPlayer *pDst)
{
  if (psActableDst[0] == '1')
  {
    if (pDst == this)
    {
      return 1;
    }
  }
  else if (pDst == this)
  {
    return 0;
  }

  if (psActableDst[1] == '1')
  {
    if (pDst->m_ObjID.m_byID || m_ObjID.m_byID)
    {
      if (pDst->GetObjRace() == GetObjRace())
      {
        return 1;
      }
    }
    else
    {
      CPlayer *srcPlayer = static_cast<CPlayer *>(this);
      if (pDst->m_bInGuildBattle && srcPlayer->m_bInGuildBattle)
      {
        if (!pDst->m_bTakeGravityStone && srcPlayer->m_byGuildBattleColorInx == pDst->m_byGuildBattleColorInx)
        {
          return 1;
        }
      }
      else if (!pDst->m_bInGuildBattle && !srcPlayer->m_bInGuildBattle)
      {
        if (pDst->GetObjRace() == GetObjRace())
        {
          return 1;
        }
      }
    }
  }

  if (psActableDst[2] == '1')
  {
    if (pDst->m_ObjID.m_byID || m_ObjID.m_byID)
    {
      if (pDst->GetObjRace() != GetObjRace() && pDst->m_ObjID.m_byID != 1)
      {
        return 1;
      }
    }
    else
    {
      CPlayer *srcPlayer = static_cast<CPlayer *>(this);
      if (pDst->m_bInGuildBattle && srcPlayer->m_bInGuildBattle)
      {
        if (!pDst->m_bTakeGravityStone && srcPlayer->m_byGuildBattleColorInx != pDst->m_byGuildBattleColorInx)
        {
          return 1;
        }
      }
      else if (!pDst->m_bInGuildBattle && !srcPlayer->m_bInGuildBattle)
      {
        if (srcPlayer->IsChaosMode())
        {
          return 1;
        }
        if (pDst->IsPunished(1u, false))
        {
          return 1;
        }
        if (pDst->GetObjRace() != GetObjRace())
        {
          return 1;
        }
      }
    }
  }

  if (psActableDst[3] == '1')
  {
    if (m_ObjID.m_byID)
    {
      if (pDst->m_ObjID.m_byID == 1)
      {
        return 1;
      }
    }
    else
    {
      CPlayer *srcPlayer = static_cast<CPlayer *>(this);
      if (!srcPlayer->m_bInGuildBattle && pDst->m_ObjID.m_byID == 1)
      {
        return 1;
      }
    }
  }

  return 0;
}

char CCharacter::IsEffectableDst(char *psActableDst, CCharacter *pDst)
{
  if (psActableDst[0] == '1' && pDst == this)
  {
    return 1;
  }

  if (psActableDst[1] == '1')
  {
    if (pDst->m_ObjID.m_byID || m_ObjID.m_byID)
    {
      if (pDst->GetObjRace() == GetObjRace())
      {
        return 1;
      }
    }
    else
    {
      CPlayer *dstPlayer = static_cast<CPlayer *>(pDst);
      CPlayer *srcPlayer = static_cast<CPlayer *>(this);
      if (dstPlayer->m_bInGuildBattle && srcPlayer->m_bInGuildBattle)
      {
        if (!dstPlayer->m_bTakeGravityStone && srcPlayer->m_byGuildBattleColorInx == dstPlayer->m_byGuildBattleColorInx)
        {
          return 1;
        }
      }
      else if (!dstPlayer->m_bInGuildBattle && !srcPlayer->m_bInGuildBattle)
      {
        if (pDst->GetObjRace() == GetObjRace())
        {
          return 1;
        }
      }
    }
  }

  if (psActableDst[2] == '1')
  {
    if (pDst->m_ObjID.m_byID || m_ObjID.m_byID)
    {
      if (pDst->GetObjRace() != GetObjRace() && pDst->m_ObjID.m_byID != 1)
      {
        return 1;
      }
    }
    else
    {
      CPlayer *dstPlayer = static_cast<CPlayer *>(pDst);
      CPlayer *srcPlayer = static_cast<CPlayer *>(this);
      if (dstPlayer->m_bInGuildBattle && srcPlayer->m_bInGuildBattle)
      {
        if (!dstPlayer->m_bTakeGravityStone && srcPlayer->m_byGuildBattleColorInx != dstPlayer->m_byGuildBattleColorInx)
        {
          return 1;
        }
      }
      else if (!dstPlayer->m_bInGuildBattle && !srcPlayer->m_bInGuildBattle)
      {
        if (srcPlayer->IsChaosMode())
        {
          return 1;
        }
        if (dstPlayer->IsPunished(1u, false))
        {
          return 1;
        }
        if (pDst->GetObjRace() != GetObjRace())
        {
          return 1;
        }
      }
    }
  }

  if (psActableDst[3] == '1')
  {
    if (m_ObjID.m_byID)
    {
      if (pDst->m_ObjID.m_byID == 1)
      {
        return 1;
      }
    }
    else
    {
      CPlayer *srcPlayer = static_cast<CPlayer *>(this);
      if (!srcPlayer->m_bInGuildBattle && pDst->m_ObjID.m_byID == 1)
      {
        return 1;
      }
    }
  }

  return 0;
}

char CCharacter::AssistSkill(
  CCharacter *pDstChar,
  int nEffectCode,
  _skill_fld *pSkillFld,
  int nSkillLv,
  unsigned __int8 *pbyErrorCode,
  bool *pbUpMty)
{
  const bool benefit = pSkillFld->m_nContEffectType == 1;
  CCharacter *targets[33]{};
  g_tmpEffectedNum = static_cast<int>(FindEffectDst(
    nEffectCode,
    pSkillFld->m_nContAreaType,
    pSkillFld->m_nLv,
    benefit,
    pDstChar,
    pSkillFld->m_strActableDst,
    targets));

  bool upFlags[64]{};
  unsigned __int8 retCodes[48]{};
  std::memset(upFlags, 0, 0x1Eu);
  std::memset(retCodes, 0, 0x1Eu);

  if (pSkillFld->m_nContEffectType != -1)
  {
    for (int j = 0; j < g_tmpEffectedNum; ++j)
    {
      CCharacter *target = targets[j];
      const unsigned __int16 contDur = static_cast<unsigned __int16>(pSkillFld->m_nContEffectSec[nSkillLv - 1]);
      retCodes[j] = target->InsertSFContEffect(
        static_cast<unsigned __int8>(pSkillFld->m_nContEffectType),
        static_cast<unsigned __int8>(nEffectCode),
        pSkillFld->m_dwIndex,
        contDur,
        static_cast<unsigned __int8>(nSkillLv),
        &upFlags[j],
        static_cast<CPlayer *>(this));

      if (!m_ObjID.m_byID && !retCodes[j])
      {
        if (pSkillFld->m_nContEffectType)
        {
          if (pSkillFld->m_nContEffectType == 1 && !target->m_ObjID.m_byID)
          {
            CMonster *targetMonster = static_cast<CMonster *>(static_cast<CPlayer *>(target)->GetTargetObj());
            if (targetMonster && !targetMonster->m_ObjID.m_byKind && targetMonster->m_ObjID.m_byID == 1)
            {
              if (targetMonster->m_AggroMgr.SearchAggroNode(target)
                  || targetMonster->GetAttackTarget() == target)
              {
                targetMonster->m_AggroMgr.SetAggro(this, 0, nEffectCode, pSkillFld->m_dwIndex, 1, 0);
              }
            }
          }
        }
        else if (target->m_ObjID.m_byID == 1)
        {
          CMonster *monster = static_cast<CMonster *>(target);
          monster->m_AggroMgr.SetAggro(this, 0, nEffectCode, pSkillFld->m_dwIndex, 0, 0);
        }
      }
    }
  }

  if (pSkillFld->m_nTempEffectType != -1)
  {
    const float tempValue = pSkillFld->m_fTempValue[nSkillLv - 1];
    auto tempFunc = g_TempEffectFunc[pSkillFld->m_nTempEffectType];
    unsigned __int8 tempRet = 0;
    for (int j = 0; j < g_tmpEffectedNum; ++j)
    {
      if (!retCodes[j])
      {
        tempRet = 0;
        if (tempFunc(this, targets[j], tempValue, &tempRet))
        {
          upFlags[j] = 1;
        }
        if (tempRet)
        {
          retCodes[j] = tempRet;
        }
        else if (!m_ObjID.m_byID)
        {
          CCharacter *target = targets[j];
          if (target->m_ObjID.m_byID)
          {
            if (target->m_ObjID.m_byID == 1)
            {
              CMonster *monster = static_cast<CMonster *>(target);
              monster->m_AggroMgr.SetAggro(this, 0, nEffectCode, pSkillFld->m_dwIndex, 0, 1);
            }
          }
          else
          {
            CMonster *targetMonster = static_cast<CMonster *>(static_cast<CPlayer *>(target)->GetTargetObj());
            if (targetMonster && !targetMonster->m_ObjID.m_byKind && targetMonster->m_ObjID.m_byID == 1)
            {
              if (targetMonster->m_AggroMgr.SearchAggroNode(target)
                  || targetMonster->GetAttackTarget() == target)
              {
                targetMonster->m_AggroMgr.SetAggro(this, 0, nEffectCode, pSkillFld->m_dwIndex, 1, 1);
              }
            }
          }
        }
      }
    }
  }

  *pbyErrorCode = 0;
  *pbUpMty = false;
  char anySuccess = 0;
  for (int j = 0; j < g_tmpEffectedNum; ++j)
  {
    g_tmpEffectedList[j].pMem = targets[j];
    g_tmpEffectedList[j].byRetCode = retCodes[j];
    g_tmpEffectedList[j].wEffectValue = targets[j]->m_wEffectTempValue;
    if (!retCodes[j])
    {
      anySuccess = 1;
    }
    if (upFlags[j])
    {
      *pbUpMty = true;
    }
  }

  if (anySuccess)
  {
    *pbyErrorCode = 0;
  }
  else
  {
    *pbyErrorCode = retCodes[0];
    g_tmpEffectedNum = 0;
  }

  return anySuccess;
}

char CCharacter::AssistForce(
  CCharacter *pDstChar,
  _force_fld *pForceFld,
  int nForceLv,
  unsigned __int8 *pbyErrorCode,
  bool *pbUpMty)
{
  const bool benefit = pForceFld->m_nContEffectType == 1;
  CCharacter *targets[33]{};
  g_tmpEffectedNum = static_cast<int>(FindEffectDst(
    1,
    pForceFld->m_nContAreaType,
    pForceFld->m_nLv,
    benefit,
    pDstChar,
    pForceFld->m_strActableDst,
    targets));

  bool upFlags[64]{};
  unsigned __int8 retCodes[48]{};
  std::memset(upFlags, 0, 0x1Eu);
  std::memset(retCodes, 0, 0x1Eu);

  if (pForceFld->m_nContEffectType != -1)
  {
    for (int j = 0; j < g_tmpEffectedNum; ++j)
    {
      CCharacter *target = targets[j];
      const unsigned __int16 contDur = static_cast<unsigned __int16>(pForceFld->m_nContEffectSec[nForceLv - 1]);
      retCodes[j] = target->InsertSFContEffect(
        static_cast<unsigned __int8>(pForceFld->m_nContEffectType),
        1,
        pForceFld->m_dwIndex,
        contDur,
        static_cast<unsigned __int8>(nForceLv),
        &upFlags[j],
        static_cast<CPlayer *>(this));

      if (!m_ObjID.m_byID && !retCodes[j])
      {
        if (pForceFld->m_nContEffectType)
        {
          if (pForceFld->m_nContEffectType == 1 && !target->m_ObjID.m_byID)
          {
            CMonster *targetMonster = static_cast<CMonster *>(static_cast<CPlayer *>(target)->GetTargetObj());
            if (targetMonster && !targetMonster->m_ObjID.m_byKind && targetMonster->m_ObjID.m_byID == 1)
            {
              if (targetMonster->m_AggroMgr.SearchAggroNode(target)
                  || targetMonster->GetAttackTarget() == target)
              {
                targetMonster->m_AggroMgr.SetAggro(this, 0, 1, pForceFld->m_dwIndex, 1, 0);
              }
            }
          }
        }
        else if (target->m_ObjID.m_byID == 1)
        {
          CMonster *monster = static_cast<CMonster *>(target);
          monster->m_AggroMgr.SetAggro(this, 0, 1, pForceFld->m_dwIndex, 0, 0);
        }
      }
    }
  }

  if (pForceFld->m_nTempEffectType != -1)
  {
    const float tempValue = pForceFld->m_fTempValue[nForceLv - 1];
    auto tempFunc = g_TempEffectFunc[pForceFld->m_nTempEffectType];
    unsigned __int8 tempRet = 0;
    for (int j = 0; j < g_tmpEffectedNum; ++j)
    {
      if (!retCodes[j])
      {
        if (tempFunc(this, targets[j], tempValue, &tempRet))
        {
          upFlags[j] = 1;
        }
        if (tempRet)
        {
          retCodes[j] = tempRet;
        }
        else if (!m_ObjID.m_byID)
        {
          CCharacter *target = targets[j];
          if (target->m_ObjID.m_byID)
          {
            if (target->m_ObjID.m_byID == 1)
            {
              CMonster *monster = static_cast<CMonster *>(target);
              monster->m_AggroMgr.SetAggro(this, 0, 1, pForceFld->m_dwIndex, 0, 1);
            }
          }
          else
          {
            CMonster *targetMonster = static_cast<CMonster *>(static_cast<CPlayer *>(target)->GetTargetObj());
            if (targetMonster && !targetMonster->m_ObjID.m_byKind && targetMonster->m_ObjID.m_byID == 1)
            {
              if (targetMonster->m_AggroMgr.SearchAggroNode(target)
                  || targetMonster->GetAttackTarget() == target)
              {
                targetMonster->m_AggroMgr.SetAggro(this, 0, 1, pForceFld->m_dwIndex, 1, 1);
              }
            }
          }
        }
      }
    }
  }

  *pbyErrorCode = 0;
  *pbUpMty = false;
  char anySuccess = 0;
  for (int j = 0; j < g_tmpEffectedNum; ++j)
  {
    g_tmpEffectedList[j].pMem = targets[j];
    g_tmpEffectedList[j].byRetCode = retCodes[j];
    g_tmpEffectedList[j].wEffectValue = targets[j]->m_wEffectTempValue;
    if (!retCodes[j])
    {
      anySuccess = 1;
    }
    if (upFlags[j])
    {
      *pbUpMty = true;
    }
  }

  if (anySuccess)
  {
    *pbyErrorCode = 0;
  }
  else
  {
    *pbyErrorCode = retCodes[0];
    g_tmpEffectedNum = 0;
  }

  return anySuccess;
}

bool CCharacter::AssistSkillToOne(CCharacter *pDst, int nEffectCode, _skill_fld *pSkillFld, int nSkillLv)
{
  unsigned __int8 upFlag = 0;
  unsigned __int8 retCode = 0;

  if (pSkillFld->m_nContEffectType != -1)
  {
    const unsigned __int16 contDur = static_cast<unsigned __int16>(pSkillFld->m_nContEffectSec[nSkillLv - 1]);
    retCode = pDst->InsertSFContEffect(
      static_cast<unsigned __int8>(pSkillFld->m_nContEffectType),
      static_cast<unsigned __int8>(nEffectCode),
      pSkillFld->m_dwIndex,
      contDur,
      static_cast<unsigned __int8>(nSkillLv),
      reinterpret_cast<bool *>(&upFlag),
      static_cast<CPlayer *>(this));

    if (!m_ObjID.m_byID && !retCode)
    {
      if (pSkillFld->m_nContEffectType)
      {
        if (pSkillFld->m_nContEffectType == 1 && !pDst->m_ObjID.m_byID)
        {
          CMonster *targetMonster = static_cast<CMonster *>(static_cast<CPlayer *>(pDst)->GetTargetObj());
          if (targetMonster && !targetMonster->m_ObjID.m_byKind && targetMonster->m_ObjID.m_byID == 1
              && (targetMonster->m_AggroMgr.SearchAggroNode(pDst)
                  || targetMonster->GetAttackTarget() == pDst))
          {
            targetMonster->m_AggroMgr.SetAggro(this, 0, nEffectCode, pSkillFld->m_dwIndex, 1, 0);
          }
        }
      }
      else if (pDst->m_ObjID.m_byID == 1)
      {
        CMonster *monster = static_cast<CMonster *>(pDst);
        monster->m_AggroMgr.SetAggro(this, 0, nEffectCode, pSkillFld->m_dwIndex, 0, 0);
      }
    }
  }

  if (pSkillFld->m_nTempEffectType != -1)
  {
    const float tempValue = pSkillFld->m_fTempValue[nSkillLv - 1];
    auto tempFunc = g_TempEffectFunc[pSkillFld->m_nTempEffectType];
    if (retCode)
    {
      return false;
    }

    tempFunc(this, pDst, tempValue, &retCode);

    if (!retCode && !m_ObjID.m_byID)
    {
      if (pDst->m_ObjID.m_byID)
      {
        if (pDst->m_ObjID.m_byID == 1)
        {
          CMonster *monster = static_cast<CMonster *>(pDst);
          monster->m_AggroMgr.SetAggro(this, 0, nEffectCode, pSkillFld->m_dwIndex, 0, 1);
        }
      }
      else
      {
        CMonster *targetMonster = static_cast<CMonster *>(static_cast<CPlayer *>(pDst)->GetTargetObj());
        if (targetMonster && !targetMonster->m_ObjID.m_byKind && targetMonster->m_ObjID.m_byID == 1
            && (targetMonster->m_AggroMgr.SearchAggroNode(pDst)
                || targetMonster->GetAttackTarget() == pDst))
        {
          targetMonster->m_AggroMgr.SetAggro(this, 0, nEffectCode, pSkillFld->m_dwIndex, 1, 1);
        }
      }
    }
  }

  return retCode == 0;
}

bool CCharacter::AssistForceToOne(CCharacter *pDst, _force_fld *pForceFld, int nForceLv)
{
  unsigned __int8 upFlag = 0;
  unsigned __int8 retCode = 0;

  if (pForceFld->m_nContEffectType != -1)
  {
    const unsigned __int16 contDur = static_cast<unsigned __int16>(pForceFld->m_nContEffectSec[nForceLv - 1]);
    retCode = pDst->InsertSFContEffect(
      static_cast<unsigned __int8>(pForceFld->m_nContEffectType),
      1,
      pForceFld->m_dwIndex,
      contDur,
      static_cast<unsigned __int8>(nForceLv),
      reinterpret_cast<bool *>(&upFlag),
      static_cast<CPlayer *>(this));

    if (!m_ObjID.m_byID && !retCode)
    {
      if (pForceFld->m_nContEffectType)
      {
        if (pForceFld->m_nContEffectType == 1 && !pDst->m_ObjID.m_byID)
        {
          CMonster *targetMonster = static_cast<CMonster *>(static_cast<CPlayer *>(pDst)->GetTargetObj());
          if (targetMonster && !targetMonster->m_ObjID.m_byKind && targetMonster->m_ObjID.m_byID == 1
              && (targetMonster->m_AggroMgr.SearchAggroNode(pDst)
                  || targetMonster->GetAttackTarget() == pDst))
          {
            targetMonster->m_AggroMgr.SetAggro(this, 0, 1, pForceFld->m_dwIndex, 1, 0);
          }
        }
      }
      else if (pDst->m_ObjID.m_byID == 1)
      {
        CMonster *monster = static_cast<CMonster *>(pDst);
        monster->m_AggroMgr.SetAggro(this, 0, 1, pForceFld->m_dwIndex, 0, 0);
      }
    }
  }

  if (pForceFld->m_nTempEffectType != -1)
  {
    const float tempValue = pForceFld->m_fTempValue[nForceLv - 1];
    auto tempFunc = g_TempEffectFunc[pForceFld->m_nTempEffectType];
    if (retCode)
    {
      return false;
    }

    tempFunc(this, pDst, tempValue, &retCode);

    if (!retCode && !m_ObjID.m_byID)
    {
      if (pDst->m_ObjID.m_byID)
      {
        if (pDst->m_ObjID.m_byID == 1)
        {
          CMonster *monster = static_cast<CMonster *>(pDst);
          monster->m_AggroMgr.SetAggro(this, 0, 1, pForceFld->m_dwIndex, 0, 1);
        }
      }
      else
      {
        CMonster *targetMonster = static_cast<CMonster *>(static_cast<CPlayer *>(pDst)->GetTargetObj());
        if (targetMonster && !targetMonster->m_ObjID.m_byKind && targetMonster->m_ObjID.m_byID == 1
            && (targetMonster->m_AggroMgr.SearchAggroNode(pDst)
                || targetMonster->GetAttackTarget() == pDst))
        {
          targetMonster->m_AggroMgr.SetAggro(this, 0, 1, pForceFld->m_dwIndex, 1, 1);
        }
      }
    }
  }

  return retCode == 0;
}

unsigned __int8 CCharacter::InsertSFContEffect(
  unsigned __int8 byContCode,
  unsigned __int8 byEffectCode,
  unsigned int dwEffectIndex,
  unsigned __int16 wDurSec,
  unsigned __int8 byLv,
  bool *pbUpMty,
  CPlayer *pActChar)
{
  *pbUpMty = true;
  int cumulCount = 1;
  unsigned int effectCount = static_cast<unsigned int>(-1);
  int cumulMax = 1;
  char *linkCode = nullptr;
  _skill_fld *effectRecord = nullptr;
  bool isAura = false;

  if (!byContCode)
  {
    _base_fld *record = g_Main.m_tblEffectData[3].GetRecord("17");
    if (record)
    {
      if (m_EP.GetEff_Plus(23) > 0.0f && byEffectCode != 3 && dwEffectIndex != record->m_dwIndex)
      {
        return 10;
      }
    }
  }

  if (!byEffectCode)
  {
    effectRecord = static_cast<_skill_fld *>(g_Main.m_tblEffectData[0].GetRecord(dwEffectIndex));
    if (effectRecord && effectRecord->m_nClass == 4)
    {
      isAura = true;
    }
  }

  const unsigned int startSec = _sf_continous::GetSFContCurTime();
  for (int j = 0; j < 8; ++j)
  {
    _sf_continous *cont = isAura ? &m_SFContAura[byContCode][j] : &m_SFCont[byContCode][j];
    if (cont->m_bExist && cont->m_byEffectCode == byEffectCode && cont->m_wEffectIndex == dwEffectIndex)
    {
      if (cont->m_byLv > byLv)
      {
        return 11;
      }
      const unsigned int elapsed = startSec - cont->m_dwStartSec;
      const int leftSec = cont->m_wDurSec - elapsed;
      const float ratio = static_cast<float>(leftSec) / static_cast<float>(wDurSec);
      if (!isAura && cont->m_byLv == byLv && ratio > 0.1f)
      {
        return static_cast<unsigned __int8>(-13);
      }
      if (isAura
          && effectRecord->m_nClass == 4
          && !std::strncmp(effectRecord->m_strRangeEffCode, "-1", 2))
      {
        cont->m_dwStartSec = startSec;
        return 0;
      }
      if (!_CheckCumulativeSF(byEffectCode, dwEffectIndex, &cumulMax, &effectCount, &linkCode)
          && cont->m_wDurSec
          && ratio > 0.1f)
      {
        return 12;
      }
      cumulCount = cont->m_nCumulCounter + 1;
      RemoveSFContEffect(byContCode, j, false, isAura);
      break;
    }
  }

  _sf_continous *targetCont = nullptr;
  unsigned __int8 listIndex = static_cast<unsigned __int8>(-1);
  for (int j = 0; j < 8; ++j)
  {
    _sf_continous *cont = isAura ? &m_SFContAura[byContCode][j] : &m_SFCont[byContCode][j];
    if (!cont->m_bExist)
    {
      targetCont = cont;
      listIndex = static_cast<unsigned __int8>(j);
      break;
    }
  }

  if (!targetCont)
  {
    targetCont = isAura ? m_SFContAura[byContCode] : m_SFCont[byContCode];
    listIndex = 0;
    for (int j = 1; j < 8; ++j)
    {
      _sf_continous *cont = isAura ? &m_SFContAura[byContCode][j] : &m_SFCont[byContCode][j];
      _base_fld *record = g_Main.m_tblEffectData[3].GetRecord("17");
      if (record && targetCont->m_byEffectCode == 3 && targetCont->m_wEffectIndex == record->m_dwIndex)
      {
        targetCont = &m_SFCont[byContCode][j];
        listIndex = static_cast<unsigned __int8>(j);
      }
      if (cont->m_dwEffSerial < targetCont->m_dwEffSerial)
      {
        targetCont = cont;
        listIndex = static_cast<unsigned __int8>(j);
      }
    }

    const unsigned int elapsed = startSec - targetCont->m_dwStartSec;
    const int leftSec = targetCont->m_wDurSec - elapsed;
    if (targetCont->m_wDurSec && (static_cast<float>(leftSec) / static_cast<float>(wDurSec)) > 0.1f)
    {
      *pbUpMty = false;
    }
    RemoveSFContEffect(byContCode, listIndex, false, isAura);
  }

  float effRate = FLOAT_1_0;
  if (!byContCode && byEffectCode == 1)
  {
    effRate = m_EP.GetEff_Rate(8);
  }

  unsigned __int16 contEffectSec = wDurSec;
  if (m_nContEffectSec == -1)
  {
    contEffectSec = static_cast<unsigned __int16>(static_cast<int>(static_cast<float>(contEffectSec) * effRate));
    const int maxDur = 2 * wDurSec;
    if (contEffectSec > maxDur)
    {
      contEffectSec = static_cast<unsigned __int16>(maxDur);
    }
  }
  else
  {
    contEffectSec = static_cast<unsigned __int16>(m_nContEffectSec);
  }

  if (!byContCode)
  {
    const float effHave = m_EP.GetEff_Have(77);
    contEffectSec = static_cast<unsigned __int16>(static_cast<int>(static_cast<float>(contEffectSec) * (1.0f - effHave)));
  }

  if (static_cast<int>(effectCount) == cumulCount)
  {
    if (byEffectCode == 1)
    {
      _force_fld *record = static_cast<_force_fld *>(g_Main.m_tblEffectData[byEffectCode].GetRecord(linkCode));
      if (!record)
      {
        m_bLastContEffectUpdate = true;
        return 0;
      }
      contEffectSec = static_cast<unsigned __int16>(record->m_nContEffectSec[0]);
      _set_sf_cont(targetCont, byEffectCode, static_cast<unsigned __int16>(record->m_dwIndex), 1u, startSec, contEffectSec, 0);
    }
    else
    {
      _skill_fld *record = static_cast<_skill_fld *>(g_Main.m_tblEffectData[byEffectCode].GetRecord(linkCode));
      if (!record)
      {
        m_bLastContEffectUpdate = true;
        return 0;
      }
      contEffectSec = static_cast<unsigned __int16>(record->m_nContEffectSec[0]);
      _set_sf_cont(targetCont, byEffectCode, static_cast<unsigned __int16>(record->m_dwIndex), 1u, startSec, contEffectSec, 0);
    }
    if (!pActChar || pActChar->m_ObjID.m_byID)
    {
      if (!m_ObjID.m_byID)
      {
        static_cast<CPlayer *>(this)->SFContInsertMessage(byContCode, listIndex, isAura, nullptr);
      }
      else
      {
        SFContInsertMessage(byContCode, listIndex, isAura);
      }
      m_bLastContEffectUpdate = true;
      return 0;
    }
  }
  else
  {
    _set_sf_cont(targetCont, byEffectCode, static_cast<unsigned __int16>(dwEffectIndex), byLv, startSec, contEffectSec, cumulCount);
    if (!pActChar || pActChar->m_ObjID.m_byID)
    {
      if (!m_ObjID.m_byID)
      {
        static_cast<CPlayer *>(this)->SFContInsertMessage(byContCode, listIndex, isAura, nullptr);
      }
      else
      {
        SFContInsertMessage(byContCode, listIndex, isAura);
      }
      m_bLastContEffectUpdate = true;
      return 0;
    }
  }

  if (!m_ObjID.m_byID)
  {
    static_cast<CPlayer *>(this)->SFContInsertMessage(byContCode, listIndex, isAura, pActChar);
  }
  else
  {
    SFContInsertMessage(byContCode, listIndex, isAura);
  }
  m_bLastContEffectUpdate = true;
  return 0;
}

void CCharacter::_set_sf_cont(
  _sf_continous *pCont,
  unsigned __int8 byEffectCode,
  unsigned __int16 wEffectIndex,
  unsigned __int8 byLv,
  unsigned int dwStartSec,
  unsigned __int16 wDurSec,
  int nCumulCount)
{
  pCont->m_bExist = true;
  pCont->m_byEffectCode = byEffectCode;
  pCont->m_wEffectIndex = wEffectIndex;
  pCont->m_byLv = byLv;
  pCont->m_dwStartSec = dwStartSec;
  pCont->m_wDurSec = wDurSec;
  pCont->m_dwEffSerial = m_dwEffSerialCounter++;
  pCont->m_nCumulCounter = nCumulCount;

  _force_fld *record = static_cast<_force_fld *>(g_Main.m_tblEffectData[byEffectCode].GetRecord(wEffectIndex));
  char *effectList = nullptr;
  if (byEffectCode == 1)
  {
    effectList = reinterpret_cast<char *>(record->m_ContParamList);
  }
  else
  {
    effectList = reinterpret_cast<char *>(&record->m_ContParamList[1].m_fContValue[4]);
  }

  if (effectList)
  {
    for (int j = 0; j < 5; ++j)
    {
      char *entry = &effectList[36 * j];
      const int paramType = *reinterpret_cast<int *>(entry);
      if (paramType == -1)
      {
        break;
      }
      const int paramIndex = *(reinterpret_cast<int *>(entry) + 1);
      const float value = *reinterpret_cast<float *>(&entry[4 * byLv + 4]);
      if (paramType == 0)
      {
        m_EP.SetEff_Rate(paramIndex, value, true);
      }
      else if (paramType == 1)
      {
        m_EP.SetEff_Plus(paramIndex, value, true);
      }
      else if (paramType == 2)
      {
        m_EP.SetEff_State(paramIndex, true);
      }
    }
  }
}

void CCharacter::SendMsg_StunInform()
{
  _character_stun_inform_zocl msg{};
  msg.byObjID = static_cast<char>(m_ObjID.m_byID);
  msg.wObjIndex = m_ObjID.m_wIndex;
  msg.dwObjSerial = m_dwObjSerial;

  unsigned __int8 packetType[2] = {17, 16};
  CircleReport(packetType, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(sizeof(msg)), false);
}
