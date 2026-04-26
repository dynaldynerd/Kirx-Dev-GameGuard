#pragma once

#include <windows.h>

#include "CLand.h"
#include "SpriteMgr.h"

class CPlayer;

class CRFClientUI
{
public:
  CRFClientUI();
  ~CRFClientUI();

  bool Initialize(void);
  void Shutdown(void);
  void Render(CLand &pi_rLand,
              const CPlayer *pi_pPlayer,
              bool pi_bKeyboardMoveMode,
              float pi_fCameraAngleY);

private:
  void RenderGaugeExpWindow(const CLand &pi_rLand,
                            const CPlayer *pi_pPlayer,
                            bool pi_bKeyboardMoveMode) const;
  void RenderHotKeyBoard(const CPlayer *pi_pPlayer,
                         bool pi_bKeyboardMoveMode) const;
  void RenderRadarWindow(const CLand &pi_rLand,
                         const CPlayer *pi_pPlayer,
                         float pi_fCameraAngleY) const;
  static void GetScreenSize(int *po_pScreenX, int *po_pScreenY);

private:
  CSpriteMgr m_cSpriteMgr;
  bool m_bInitialized;
};
