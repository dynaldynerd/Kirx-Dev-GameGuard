#include "CResObject.h"

#include <cmath>
#include <cstring>

#include "CCharacterMgr.h"
#include "R3Engine/common/commonutil.h"
#include "R3Engine/2ndclass/r3util.h"

namespace
{
void CopyVector3Local(float *po_pfDst, const float *pi_pfSrc)
{
  if (!po_pfDst || !pi_pfSrc)
  {
    return;
  }

  po_pfDst[0] = pi_pfSrc[0];
  po_pfDst[1] = pi_pfSrc[1];
  po_pfDst[2] = pi_pfSrc[2];
}
}

CResObject::CResObject()
  : m_dwStatusFlag(ROSF_RESET),
    m_pBone(NULL),
    m_dwOldBoneFrame(0),
    m_dwBoneFrame(160),
    m_fScale(1.0f),
    m_dwLightColor(LIGHT_NORMAL_COLOR),
    m_dwPrevLightColor(ID_INVALID),
    m_fAlphaDensity(1.0f)
{
  ZeroMemory(m_vecPos, sizeof(m_vecPos));
  ZeroMemory(m_vecPrevPos, sizeof(m_vecPrevPos));
  ZeroMemory(m_vecRot, sizeof(m_vecRot));
  ZeroMemory(&m_d3dMaterial, sizeof(m_d3dMaterial));
  ZeroMemory(&m_d3dLight, sizeof(m_d3dLight));

  m_vecBBoxMin[0] = -20.0f;
  m_vecBBoxMin[1] = 0.0f;
  m_vecBBoxMin[2] = -20.0f;
  m_vecBBoxMax[0] = 20.0f;
  m_vecBBoxMax[1] = 150.0f;
  m_vecBBoxMax[2] = 20.0f;

  Enable(ROSF_ANIMATION | ROSF_ANIMATION_LOOP | ROSF_MOVABLE);
}

CResObject::~CResObject()
{
}

void CResObject::Init_EveryFrame(void)
{
}

BOOL CResObject::Render(void)
{
  ChInterface *l_pCharIF = CCharacterMgr::GetCharIF();
  if (!l_pCharIF)
  {
    return FALSE;
  }

  if (IsEnable(ROSF_RENDER_BONE) && IsDisable(ROSF_CLIPPED) && m_pBone)
  {
    l_pCharIF->SetState();
    l_pCharIF->DrawCharacter(m_pBone, m_vecPos, m_vecRot[1], m_fScale, 0.0f);
    l_pCharIF->UnSetState();
  }

  return TRUE;
}

BOOL CResObject::Animation(DWORD /*pi_dwAniFrame*/)
{
  if (!IsEnable(ROSF_ANIMATION) || IsEnable(ROSF_ANIMATION_STOP_ENGINE) || !m_pBone)
  {
    return FALSE;
  }

  ChInterface *l_pCharIF = CCharacterMgr::GetCharIF();
  if (!l_pCharIF)
  {
    return FALSE;
  }

  m_dwOldBoneFrame = m_dwBoneFrame;
  if (m_pBone->m_OMaxFrame > 160)
  {
    float l_fBoneFrame = static_cast<float>(m_dwBoneFrame);
    l_fBoneFrame += (R3GetLoopTime() * 1000.0f) * 5.0f;
    if (l_fBoneFrame >= static_cast<float>(m_pBone->m_OMaxFrame))
    {
      l_fBoneFrame = IsEnable(ROSF_ANIMATION_LOOP) ? 160.0f : static_cast<float>(m_pBone->m_OMaxFrame);
    }
    m_dwBoneFrame = static_cast<DWORD>(l_fBoneFrame);
  }
  else
  {
    m_dwBoneFrame = 160;
  }

  m_pBone->m_Frame = m_dwBoneFrame;
  l_pCharIF->FrameMove(m_pBone);
  return TRUE;
}

void CResObject::FrameMove(void)
{
}

BOOL CResObject::RenderName(void)
{
  return FALSE;
}

void CResObject::CreateShadow(void)
{
  if (!IsEnable(ROSF_RENDER_SHADOW) || !m_pBone)
  {
    return;
  }

  ChInterface *l_pCharIF = CCharacterMgr::GetCharIF();
  if (!l_pCharIF)
  {
    return;
  }

  l_pCharIF->SetState();
  l_pCharIF->DrawCharacterShadow(m_pBone, m_vecPos, m_vecRot[1], 0.6f * m_fAlphaDensity, m_fScale);
  l_pCharIF->UnSetState();
}

void CResObject::Enable(DWORD pi_dwFlag)
{
  m_dwStatusFlag |= pi_dwFlag;
}

void CResObject::Disable(DWORD pi_dwFlag)
{
  m_dwStatusFlag &= ~pi_dwFlag;
}

BOOL CResObject::IsEnable(DWORD pi_dwFlag) const
{
  return (m_dwStatusFlag & pi_dwFlag) == pi_dwFlag;
}

BOOL CResObject::IsDisable(DWORD pi_dwFlag) const
{
  return !IsEnable(pi_dwFlag);
}

void CResObject::ResetStatusFlag(void)
{
  m_dwStatusFlag = ROSF_RESET;
}

void CResObject::SetPosition(float pi_fPosX, float pi_fPosY, float pi_fPosZ, BOOL pi_bSetPrevPosition)
{
  if (pi_bSetPrevPosition)
  {
    CopyVector3Local(m_vecPrevPos, m_vecPos);
  }

  m_vecPos[0] = pi_fPosX;
  m_vecPos[1] = pi_fPosY;
  m_vecPos[2] = pi_fPosZ;
}

void CResObject::GetPosition(float po_pfPos[3]) const
{
  CopyVector3Local(po_pfPos, m_vecPos);
}

float *CResObject::GetPosition(void)
{
  return m_vecPos;
}

const float *CResObject::GetPosition(void) const
{
  return m_vecPos;
}

void CResObject::SetRotY(float pi_fRotY)
{
  while (pi_fRotY >= 360.0f)
  {
    pi_fRotY -= 360.0f;
  }

  while (pi_fRotY < 0.0f)
  {
    pi_fRotY += 360.0f;
  }

  m_vecRot[1] = pi_fRotY;
}

float CResObject::GetRotY(void) const
{
  return m_vecRot[1];
}

void CResObject::SetScale(float pi_fScale)
{
  m_fScale = pi_fScale;
}

float CResObject::GetScale(void) const
{
  return m_fScale;
}

void CResObject::SetAlphaDensity(float pi_fAlpha)
{
  m_fAlphaDensity = pi_fAlpha;
}

float CResObject::GetAlphaDensity(void) const
{
  return m_fAlphaDensity;
}

void CResObject::SetLightColor(DWORD pi_dwColor, BYTE pi_bFlag)
{
  if (pi_bFlag == LIGHT_BACKUP)
  {
    m_dwPrevLightColor = m_dwLightColor;
  }
  else if (pi_bFlag == LIGHT_RESTORE && m_dwPrevLightColor != ID_INVALID)
  {
    pi_dwColor = m_dwPrevLightColor;
  }

  m_dwLightColor = pi_dwColor;
  GetMatLightFromColor(&m_d3dLight, &m_d3dMaterial, m_dwLightColor);
}

void CResObject::SetLightColorFromMap(DWORD pi_dwColor)
{
  SetLightColor(pi_dwColor);
}

DWORD CResObject::GetLightColor(void) const
{
  return m_dwLightColor;
}

void CResObject::SetBonePtr(CHARACTEROBJECT *pi_pBone)
{
  m_pBone = pi_pBone;
  m_dwBoneFrame = 160;
  m_dwOldBoneFrame = 0;
}

CHARACTEROBJECT *CResObject::GetBone(void) const
{
  return m_pBone;
}

void CResObject::SetBoundBox(const float pi_pfMin[3], const float pi_pfMax[3])
{
  if (pi_pfMin)
  {
    CopyVector3Local(m_vecBBoxMin, pi_pfMin);
  }

  if (pi_pfMax)
  {
    CopyVector3Local(m_vecBBoxMax, pi_pfMax);
  }
}

void CResObject::GetBoundBoxMin(float po_pfMin[3]) const
{
  CopyVector3Local(po_pfMin, m_vecBBoxMin);
}

void CResObject::GetBoundBoxMax(float po_pfMax[3]) const
{
  CopyVector3Local(po_pfMax, m_vecBBoxMax);
}

void CResObject::GetCameraTarget(float po_pfTarget[3]) const
{
  if (!po_pfTarget)
  {
    return;
  }

  po_pfTarget[0] = m_vecPos[0];
  po_pfTarget[2] = m_vecPos[2];

  const float l_fBBoxExtentY = m_vecBBoxMax[1] - m_vecBBoxMin[1];
  const float l_fBBoxExtentZ = m_vecBBoxMax[2] - m_vecBBoxMin[2];
  const float l_fBBoxExtent = (l_fBBoxExtentY > l_fBBoxExtentZ) ? l_fBBoxExtentY : l_fBBoxExtentZ;
  if (IsValidBBoxHeight(l_fBBoxExtent))
  {
    po_pfTarget[1] = m_vecPos[1] + (l_fBBoxExtent * 0.5f);
  }
  else
  {
    po_pfTarget[1] = m_vecPos[1] + 75.0f;
  }
}

float CResObject::GetCameraExtent(void) const
{
  const float l_fBBoxExtentY = m_vecBBoxMax[1] - m_vecBBoxMin[1];
  const float l_fBBoxExtentZ = m_vecBBoxMax[2] - m_vecBBoxMin[2];
  const float l_fBBoxExtent = (l_fBBoxExtentY > l_fBBoxExtentZ) ? l_fBBoxExtentY : l_fBBoxExtentZ;
  if (IsValidBBoxHeight(l_fBBoxExtent))
  {
    return l_fBBoxExtent;
  }

  return 75.0f;
}

void CResObject::GetScaleAddPos(float *po_pfScale, float *po_pfAddPos) const
{
  const float l_fExtentX = m_vecBBoxMax[0] - m_vecBBoxMin[0];
  const float l_fExtentY = m_vecBBoxMax[1] - m_vecBBoxMin[1];
  const float l_fExtentZ = m_vecBBoxMax[2] - m_vecBBoxMin[2];
  const float l_fHeight = (l_fExtentY > l_fExtentZ) ? l_fExtentY : l_fExtentZ;
  const float l_fHalfWidth = ((l_fExtentX * 0.5f) > m_vecBBoxMax[0]) ? (l_fExtentX * 0.5f) : m_vecBBoxMax[0];

  if (po_pfScale)
  {
    *po_pfScale = l_fHeight / 18.0f;
    if (!std::isfinite(*po_pfScale) || *po_pfScale <= 0.0f)
    {
      *po_pfScale = 1.0f;
    }
  }

  if (po_pfAddPos)
  {
    const float l_fDiff = (l_fHeight * 3.0f / 5.0f) > l_fHalfWidth ? (l_fHeight * 3.0f / 5.0f) : l_fHalfWidth;
    *po_pfAddPos = (l_fDiff + l_fHalfWidth - l_fHalfWidth * 2.0f) / 2.0f;
    if (!std::isfinite(*po_pfAddPos))
    {
      *po_pfAddPos = 10.0f;
    }
  }
}

bool CResObject::IsValidBBoxHeight(float pi_fHeight)
{
  return std::isfinite(pi_fHeight) && (pi_fHeight > 1.0f) && (pi_fHeight < 1000.0f);
}
