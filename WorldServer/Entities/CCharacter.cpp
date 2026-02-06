#include "pch.h"

#include "CCharacter.h"

#include <cstring>
#include <mmsystem.h>

#include "ObjectCreateSetData.h"
#include "CRecordData.h"
#include "GlobalObjects.h"
#include "base_fld.h"
#include "WorldServerUtil.h"
#include "CBsp.h"
#include "CMapOperation.h"

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
      if (!CLevel::GetNextYposForServerFar(&m_pCurMap->m_Level, src, m_fCurPos, &m_fCurPos[1]))
      {
        if (!CLevel::GetNextYposFarProgress(&m_pCurMap->m_Level, src, m_fCurPos, &m_fCurPos[1]))
        {
          memcpy_0(m_fCurPos, src, sizeof(m_fCurPos));
          memcpy_0(m_fTarPos, m_fCurPos, sizeof(m_fTarPos));
          Stop();
          CGameObject::SendMsg_BreakStop(this);
        }
      }
    }
    else
    {
      m_fTarPos[1] = m_fCurPos[1];
      if (CLevel::GetNextYposForServer(&m_pCurMap->m_Level, m_fTarPos, &m_fTarPos[1])
          || CLevel::GetNextYposForServerFar(&m_pCurMap->m_Level, m_fCurPos, m_fTarPos, &m_fTarPos[1]))
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
      if (CBsp::CanYouGoThere(m_pCurMap->m_Level.mBsp, cur, checkPos, &scratch))
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
      if (CBsp::CanYouGoThere(m_pCurMap->m_Level.mBsp, cur, checkPos, &scratch))
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
  if (_effect_parameter::GetEff_State(&m_EP, 5) || _effect_parameter::GetEff_Plus(&m_EP, 21) > 0.0f
      || _effect_parameter::GetEff_State(&m_EP, 26))
  {
    CGameObject::SetBreakTranspar(this, true);
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

  if (_effect_parameter::GetEff_State(&pDst->m_EP, 12))
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
      if (CMonster::GetViewAngleCap(reinterpret_cast<CMonster *>(pDst), 1, outValue))
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
  float tolRate = 0.5f;
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
    if (pDst->GetLevel(pDst) >= 30)
    {
      return static_cast<unsigned int>(rand() % 300 + 1);
    }
  }

  (void)bBackAttack;
  return static_cast<unsigned int>(damage);
}

void CCharacter::SendMsg_AttackActEffect(unsigned __int8 byActEffect, CCharacter *pDamer)
{
  char szMsg[2]{};
  szMsg[0] = static_cast<char>(byActEffect);
  szMsg[1] = static_cast<char>(m_ObjID.m_byID);
  const unsigned int objSerial = m_dwObjSerial;
  const unsigned __int8 damerId = pDamer->m_ObjID.m_byID;
  const unsigned int damerSerial = pDamer->m_dwObjSerial;
  (void)objSerial;
  (void)damerId;
  (void)damerSerial;
  unsigned __int8 pbyType[36]{};
  pbyType[0] = 5;
  pbyType[1] = 24;
  CGameObject::CircleReport(this, pbyType, szMsg, 11, true);
}
