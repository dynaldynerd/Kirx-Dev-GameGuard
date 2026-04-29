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

struct CHARACTER_CREATE_RACE_LAYOUT
{
  RECT sUpperBoard;
  RECT sUpperBase;
  RECT sUpperBoardAni;
  RECT sRaceButton[3];
  RECT sRaceProfile;
  RECT sLowerBoard;
  RECT sLowerBase;
  RECT sOkButton;
  RECT sCancelButton;
};

struct CHARACTER_CREATE_ATTRIBUTE_LAYOUT
{
  RECT sPanel;
  RECT sTitleBase;
  RECT sLowerBase;
  RECT sOkButton;
  RECT sCancelButton;
  RECT sAttributeButton[4];
  RECT sDescText[2];
  RECT sBasicPointText[3];
  RECT sBasicPointBase[3];
  RECT sBattleSkillText[6];
  RECT sBattleSkillBase[6];
  RECT sMakeSkillText[3];
  RECT sMakeSkillBase[3];
};

struct CHARACTER_CREATE_DETAIL_LAYOUT
{
  RECT sPanel;
  RECT sTitleBase;
  RECT sLowerBase;
  RECT sOkButton;
  RECT sCancelButton;
  RECT sSelectItem[7];
  RECT sSelectLeftButton[7];
  RECT sSelectRightButton[7];
  RECT sNameInput;
};

struct CHARACTER_CREATE_ROTATE_LAYOUT
{
  RECT sLowerBoard;
  RECT sLowerBase;
  RECT sRotateLeftButton;
  RECT sRotateRightButton;
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
  void EnterCharacterCreateScreen(void);
  void LeaveCharacterCreateScreen(bool pi_bCreationCompleted = false);
  void ResetCharacterCreateState(void);
  void UpdateCharacterCreatePreview(void);
  bool IsCharacterCreateScreen(void) const;
  bool GetCharacterCreateRaceLayout(CHARACTER_CREATE_RACE_LAYOUT *po_pLayout) const;
  bool GetCharacterCreateAttributeLayout(CHARACTER_CREATE_ATTRIBUTE_LAYOUT *po_pLayout) const;
  bool GetCharacterCreateDetailLayout(CHARACTER_CREATE_DETAIL_LAYOUT *po_pLayout) const;
  bool GetCharacterCreateRotateLayout(CHARACTER_CREATE_ROTATE_LAYOUT *po_pLayout) const;
  BYTE GetCharacterCreateButtonAtPoint(int pi_nX, int pi_nY) const;
  void UpdateCharacterCreateInput(void);
  void UpdateCharacterCreateMouseInput(void);
  void ActivateCharacterCreateButton(BYTE pi_byButtonIndex);
  void ActivateCharacterCreateOk(void);
  BYTE GetCreateRaceSexCode(void) const;
  const char *GetCreateClassCode(void) const;
  DWORD GetCreateBaseShape(void) const;
  void AppendCreateCharacterInputChar(WPARAM pi_wParam);
  void ShowLoginMessage(const char *pi_pMessage);
  void HideLoginMessage(void);
  bool IsLoginMessageVisible(void) const;
  bool GetLoginMessageLayout(RECT *po_pBoxRect, RECT *po_pOkButtonRect) const;
  void UpdateLoginMessageInput(void);
  void RenderStatusOverlay(void) const;
  void RenderOpeningScreen(void) const;
  void RenderCreditsRoll(void) const;
  void RenderUILockScreen(void) const;
  void RenderCharacterSelectionScreen(void) const;
  void RenderCharacterCreateScreen(void) const;
  void RenderCharacterCreateRaceScreen(void) const;
  void RenderCharacterCreateAttributeScreen(void) const;
  void RenderCharacterCreateDetailScreen(void) const;
  void RenderCharacterCreateRotateControls(void) const;
  void RenderLoginMessageBox(void) const;

private:
  BYTE m_bySelectedCharacterSlot;
  BYTE m_byScreenMode;
  BYTE m_byMenuSelection;
  BYTE m_byMenuHover;
  BYTE m_byMenuPressed;
  BYTE m_byCharacterMenuHover;
  BYTE m_byCharacterMenuPressed;
  BYTE m_byCreateStep;
  BYTE m_byCreateRaceSelection;
  BYTE m_byCreateAttributeSelection;
  BYTE m_byCreateSexSelection;
  BYTE m_byCreateMenuHover;
  BYTE m_byCreateMenuPressed;
  BYTE m_abyCreatePartVariant[7];
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
  bool m_bLoginMessageVisible;
  bool m_bLoginMessageOkHover;
  bool m_bLoginMessageOkPressed;
  DWORD m_dwCreditsStartTick;
  void *m_pTitleLogoTexture;
  char m_szLoginMessage[128];
  char m_szCreateCharacterName[17];
  CHARACTER_SELECT_ANIMATION m_sUpperBoardAnimation;
  CHARACTER_SELECT_ANIMATION m_sLowerBoardAnimation;
  CSpriteMgr m_cLoginSpriteMgr;
  std::vector<TITLE_CREDIT_LINE> m_vecTitleCredits;
};
