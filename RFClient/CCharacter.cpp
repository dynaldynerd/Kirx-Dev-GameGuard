#include "CCharacter.h"

CCharacter::CCharacter()
  : m_dwCharTypeID(ID_INVALID),
    m_dwActionID(ID_INVALID),
    m_dwPrevActionID(ID_INVALID),
    m_dwActionTime(0)
{
}

CCharacter::~CCharacter()
{
}

BOOL CCharacter::Render(void)
{
  return CResObject::Render();
}

BOOL CCharacter::Animation(DWORD pi_dwAniFrame)
{
  return CResObject::Animation(pi_dwAniFrame);
}

void CCharacter::FrameMove(void)
{
  CResObject::FrameMove();
}

void CCharacter::CreateShadow(void)
{
  CResObject::CreateShadow();
}

void CCharacter::SetCharTypeID(DWORD pi_dwCharTypeID)
{
  m_dwCharTypeID = pi_dwCharTypeID;
}

DWORD CCharacter::GetCharTypeID(void) const
{
  return m_dwCharTypeID;
}

void CCharacter::SetAction(DWORD pi_dwActionID)
{
  m_dwPrevActionID = m_dwActionID;
  m_dwActionID = pi_dwActionID;
}

DWORD CCharacter::GetAction(void) const
{
  return m_dwActionID;
}
