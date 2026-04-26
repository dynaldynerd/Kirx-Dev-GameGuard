#pragma once

#include "CCharacter.h"

class CBaseNpc : public CCharacter
{
protected:
  CHARACTEROBJECT *m_pMesh;
  void *m_pData;
  BYTE m_byForceNo;

public:
  CBaseNpc();
  virtual ~CBaseNpc();

  BOOL Render(void) override;
  BOOL Animation(DWORD pi_dwAniFrame = static_cast<DWORD>(-1)) override;
  void FrameMove(void) override;
  void CreateShadow(void) override;

  void SetMeshPtr(CHARACTEROBJECT *pi_pMesh);
  CHARACTEROBJECT *GetMesh(void) const;
};
