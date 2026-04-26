#pragma once

#include "CResObject.h"

class CCharacter : public CResObject
{
protected:
  DWORD m_dwCharTypeID;
  DWORD m_dwActionID;
  DWORD m_dwPrevActionID;
  DWORD m_dwActionTime;

public:
  CCharacter();
  virtual ~CCharacter();

  BOOL Render(void) override;
  BOOL Animation(DWORD pi_dwAniFrame = static_cast<DWORD>(-1)) override;
  void FrameMove(void) override;
  void CreateShadow(void) override;

  void SetCharTypeID(DWORD pi_dwCharTypeID);
  DWORD GetCharTypeID(void) const;
  void SetAction(DWORD pi_dwActionID);
  DWORD GetAction(void) const;
};
