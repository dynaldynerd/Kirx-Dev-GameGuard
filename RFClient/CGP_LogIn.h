#pragma once

#include <vector>

#include "CGameProgress.h"
#include "SpriteMgr.h"

struct TITLE_CREDIT_LINE
{
  char szText[64];
  int nStyle;
};

struct CHARACTER_SELECT_ANIMATION
{
  float fCurFrameNo;
  bool bStop;
  DWORD dwStopTime;
};

struct CHARACTER_SELECT_LAYOUT
{
  RECT sUpperBoard;
  RECT sUpperBase;
  RECT sUpperBoardAni;
  RECT sLeftCharInfoBoard;
  RECT sRightCharInfoBoard;
  RECT sNameText;
  RECT sLevelText;
  RECT sDalantText;
  RECT sGoldText;
  RECT sLowerBoard;
  RECT sLowerBase;
  RECT sLowerBoardAni;
  RECT sPrevButton;
  RECT sNextButton;
  RECT sCreateButton;
  RECT sDeleteButton;
  RECT sConnectButton;
  RECT sExitButton;
};

class CGP_LogIn : public CGameProgress
{
public:
  CGP_LogIn();
  ~CGP_LogIn();

  BOOL Create(void) override;
  BOOL Destroy(void) override;

  HRESULT RestoreDeviceObjects(void) override;
  HRESULT InvalidateDeviceObjects(void) override;
  LRESULT MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

protected:
  HRESULT FrameMove(void) override;
  HRESULT Render(void) override;
  BOOL InputProcess(void) override;

  BOOL LoadData(void) override;
  BOOL UnloadData(void) override;

private:
  BOOL ConsumeKeyPress(int pi_nVirtualKey);
  void UpdateWorldServerFlow(void);
  void EnsureCharacterSelection(void);
  void UpdateDisplayedScreen(void);
  void UpdateOpeningInput(void);
  void UpdateCharacterSelectionInput(void);
  bool IsUILockResolved(void) const;
  void ReleaseTitleLogoTexture(void);
  void LoadLoginSprites(void);
  void UnloadLoginSprites(void);
  void StartCreditsRoll(void);
  void StopCreditsRoll(void);
  void LoadCreditsFile(void);
  void RenderTitleLogo(void) const;
  bool IsStartupLoadingComplete(void) const;
  bool ShouldRenderLoadingScreen(void) const;
  void RenderLoadingScreen(void) const;
  void PresentLoadingScreen(void) const;
  bool GetOpeningMenuLayout(RECT *po_pPanelRect, RECT po_aButtonRect[3]) const;
  BYTE GetOpeningMenuItemAtPoint(int pi_nX, int pi_nY) const;
  void UpdateOpeningMouseInput(void);
  void ClearOpeningMouseTransitions(void);
  void ActivateOpeningMenuItem(BYTE pi_byMenuIndex);
  bool GetCharacterSelectionLayout(CHARACTER_SELECT_LAYOUT *po_pLayout) const;
  void UpdateCharacterSelectionUIState(void);
  void StartCharacterSelectionUIOpen(void);
  void StartCharacterSelectionUIClose(void);
  bool IsCharacterSelectionUIVisible(void) const;
  bool IsCharacterSelectionUIInteractive(void) const;
  void ApplyCharacterSelectionOpenAnimation(CHARACTER_SELECT_LAYOUT *pio_pLayout) const;
  BYTE GetCharacterSelectionButtonAtPoint(int pi_nX, int pi_nY) const;
  void UpdateCharacterSelectionMouseInput(void);
  void UpdateCharacterSelectionAnimation(void);
  void ResetCharacterSelectionAnimation(void);
  void ActivateCharacterSelectionButton(BYTE pi_byButtonIndex);
  void MoveCharacterSelectionSlot(bool pi_bNext);
  void SendSelectedCharacterRequest(void);
  void RenderStatusOverlay(void) const;
  void RenderOpeningScreen(void) const;
  void RenderCreditsRoll(void) const;
  void RenderUILockScreen(void) const;
  void RenderCharacterSelectionScreen(void) const;

private:
  BYTE m_bySelectedCharacterSlot;
  BYTE m_byScreenMode;
  BYTE m_byMenuSelection;
  BYTE m_byMenuHover;
  BYTE m_byMenuPressed;
  BYTE m_byCharacterMenuHover;
  BYTE m_byCharacterMenuPressed;
  bool m_abVirtualKeyState[256];
  int m_nMouseX;
  int m_nMouseY;
  bool m_bLeftButtonDown;
  bool m_bLeftButtonPressed;
  bool m_bLeftButtonReleased;
  DWORD m_dwStartRequestTick;
  DWORD m_dwCharacterSelectionOpenTick;
  DWORD m_dwCharacterSelectionCloseTick;
  bool m_bCharacterSelectionUIVisible;
  bool m_bCharacterSelectionUIClosing;
  bool m_bCharacterDummiesLoaded;
  bool m_bStartRequested;
  bool m_bCreditsMode;
  DWORD m_dwCreditsStartTick;
  void *m_pTitleLogoTexture;
  CHARACTER_SELECT_ANIMATION m_sUpperBoardAnimation;
  CHARACTER_SELECT_ANIMATION m_sLowerBoardAnimation;
  CSpriteMgr m_cLoginSpriteMgr;
  std::vector<TITLE_CREDIT_LINE> m_vecTitleCredits;
};
