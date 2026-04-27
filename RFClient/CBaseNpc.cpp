#include "CBaseNpc.h"

#include "CCharacterMgr.h"

CBaseNpc::CBaseNpc()
  : m_pMesh(NULL),
    m_pData(NULL),
    m_byForceNo(0)
{
  SetObjectTypeID(CTI_NPC);
  SetCharTypeID(CTI_NPC);
}

CBaseNpc::~CBaseNpc()
{
}

BOOL CBaseNpc::Render(void)
{
  CCharacter::Render();

  if (!m_pMesh || IsEnable(ROSF_CLIPPED))
  {
    return m_pMesh != NULL;
  }

  ChInterface *l_pCharIF = CCharacterMgr::GetCharIF();
  if (!l_pCharIF)
  {
    return FALSE;
  }

  l_pCharIF->SetAlpha(m_pMesh, m_fAlphaDensity);
  l_pCharIF->SetMaterial(m_pMesh, m_d3dMaterial);
  l_pCharIF->DrawCharacter(m_pMesh, m_vecPos, m_vecRot[1], m_fScale, 0.0f);
  return TRUE;
}

BOOL CBaseNpc::Animation(DWORD pi_dwAniFrame)
{
  CCharacter::Animation(pi_dwAniFrame);

  if (!m_pMesh)
  {
    return FALSE;
  }

  ChInterface *l_pCharIF = CCharacterMgr::GetCharIF();
  if (!l_pCharIF)
  {
    return FALSE;
  }

  l_pCharIF->FrameMove(m_pMesh);
  return TRUE;
}

void CBaseNpc::FrameMove(void)
{
  CCharacter::FrameMove();
}

void CBaseNpc::CreateShadow(void)
{
  if (!m_pMesh)
  {
    return;
  }

  ChInterface *l_pCharIF = CCharacterMgr::GetCharIF();
  if (!l_pCharIF)
  {
    return;
  }

  l_pCharIF->SetState();
  l_pCharIF->DrawCharacterShadow(m_pMesh, m_vecPos, m_vecRot[1], 0.6f * m_fAlphaDensity, m_fScale);
  l_pCharIF->UnSetState();
}

void CBaseNpc::SetMeshPtr(CHARACTEROBJECT *pi_pMesh)
{
  m_pMesh = pi_pMesh;
}

CHARACTEROBJECT *CBaseNpc::GetMesh(void) const
{
  return m_pMesh;
}
