#include "Network/CNetworkMgr.h"
#include "CGP_LogIn.h"

#include <cstdio>
#include <cstring>

#include <d3dx8tex.h>
#include <mmsystem.h>
#include <windowsx.h>

#include "RFClientApp.h"
#include "R3Engine/2ndclass/2dsprite.h"
#include "R3Engine/2ndclass/r3enginekernel.h"
#include "R3Engine/2ndclass/r3text.h"
#include "UIGlobal.h"

namespace
{
enum LOGIN_SCREEN_MODE
{
  LOGIN_SCREEN_OPENING = 0,
  LOGIN_SCREEN_UI_LOCK = 1,
  LOGIN_SCREEN_CHAR_SELECT = 2,
  LOGIN_SCREEN_CHAR_CREATE = 3
};

const int kFontWidth = 6;
const int kMessageBoxTextMargin = 5;
const int kMessageBoxColumnGap = 4;
const int kMessageBoxMargin = 15;
const int kMessageBoxOutlineMargin = 3;
const int kMessageBoxOutlineSize = 1;
const int kMessageBoxMinWidth = 200;
const int kMessageBoxMinHeight = 100;
const int kMessageBoxOkButtonWidth = 67;
const int kMessageBoxOkButtonHeight = 21;
const int kMessageBoxOkButtonRightMargin = 8;
const int kMessageBoxOkButtonBottomMargin = 8;
const DWORD kMessageBoxOutlineColor = 0xFF9593C1;
const DWORD kMessageBoxPanelColor = 0xB0181818;
const DWORD kMessageBoxOkButtonAction = 1;
const DWORD kMessageBoxOkButtonNormalFrame = 0;
const DWORD kMessageBoxOkButtonPressedFrame = 1;
const DWORD kMessageBoxOkButtonHoverFrame = 2;
const DWORD kOpeningMenuAction = 6;
const DWORD kOpeningMenuPanelFrame = 0;
const DWORD kOpeningMenuButtonNormalFrame = 1;
const DWORD kOpeningMenuButtonSelectedFrame = 2;
const DWORD kOpeningMenuButtonPressedFrame = 3;
const BYTE kInvalidMenuIndex = 0xFF;
const DWORD kOpeningMenuNormalTextColor = 0xFFB3B3B3;
const DWORD kOpeningMenuFocusedTextColor = 0xFFFFFFFF;
const int kFontHeight = 12;
const DWORD kCharacterSelectAction = 2;
const DWORD kCharacterSelectAniAction = 5;
const DWORD kCharacterSelectTextColor = 0xFF7FA9DC;
const DWORD kCharacterSelectButtonTextColor = 0xFFB9D0FF;
const DWORD kCharacterSelectButtonHoverTextColor = 0xFFE8E8E8;
const DWORD kCharacterSelectInfoBoardColor = 0xC0FFFFFF;
const DWORD kCharacterSelectButtonColor = 0xB0FFFFFF;
const DWORD kCharacterSelectSpriteColor = 0xFFFFFFFF;
const DWORD kCharacterSelectBoardMoveTime = 500;
const DWORD kCharacterSelectBoardCloseTime = 300;
const DWORD kCharacterSelectAniStopInterval = 2000;
const float kCharacterSelectAniFPS = 10.0f;
const BYTE kCharacterCreateDefaultItemCount = 5;
const BYTE kInvalidCharacterButton = 0xFF;
const BYTE kInvalidCharacterCreateButton = 0xFF;
const DWORD kCharacterCreateRaceAction = 3;
const DWORD kCharacterCreateDetailAction = 4;

enum CHARACTER_CREATE_STEP
{
  CHARACTER_CREATE_STEP_RACE = 0,
  CHARACTER_CREATE_STEP_ATTRIBUTE = 1,
  CHARACTER_CREATE_STEP_DETAIL = 2
};

enum CHARACTER_CREATE_BUTTON
{
  CHARACTER_CREATE_BUTTON_RACE_BELLATO = 0,
  CHARACTER_CREATE_BUTTON_RACE_CORA = 1,
  CHARACTER_CREATE_BUTTON_RACE_ACCRETIA = 2,
  CHARACTER_CREATE_BUTTON_OK = 3,
  CHARACTER_CREATE_BUTTON_CANCEL = 4,
  CHARACTER_CREATE_BUTTON_ATTRIBUTE_BASE = 10,
  CHARACTER_CREATE_BUTTON_DETAIL_LEFT_BASE = 20,
  CHARACTER_CREATE_BUTTON_DETAIL_RIGHT_BASE = 30,
  CHARACTER_CREATE_BUTTON_ROTATE_LEFT = 40,
  CHARACTER_CREATE_BUTTON_ROTATE_RIGHT = 41
};

enum CHARACTER_CREATE_SELECT_TYPE
{
  CHARACTER_CREATE_SELECT_SEX = 0,
  CHARACTER_CREATE_SELECT_HAIR = 1,
  CHARACTER_CREATE_SELECT_FACE = 2,
  CHARACTER_CREATE_SELECT_COAT = 3,
  CHARACTER_CREATE_SELECT_PANTS = 4,
  CHARACTER_CREATE_SELECT_GLOVE = 5,
  CHARACTER_CREATE_SELECT_SHOES = 6,
  CHARACTER_CREATE_SELECT_COUNT = 7
};

enum CHARACTER_CREATE_ATTRIBUTE
{
  CHARACTER_CREATE_ATTRIBUTE_MELEE = 0,
  CHARACTER_CREATE_ATTRIBUTE_MISSILE = 1,
  CHARACTER_CREATE_ATTRIBUTE_FORCE = 2,
  CHARACTER_CREATE_ATTRIBUTE_SPECIALIST = 3
};

enum CHARACTER_SELECT_BUTTON
{
  CHARACTER_SELECT_BUTTON_PREV = 0,
  CHARACTER_SELECT_BUTTON_NEXT = 1,
  CHARACTER_SELECT_BUTTON_CREATE = 2,
  CHARACTER_SELECT_BUTTON_DELETE = 3,
  CHARACTER_SELECT_BUTTON_CONNECT = 4,
  CHARACTER_SELECT_BUTTON_EXIT = 5
};

void DrawLoginLine(int x, int y, const char *text, DWORD color)
{
  const char *l_pText = text ? text : "";
  if (!l_pText[0])
  {
    return;
  }

  DrawR3Hangul(x, y, const_cast<char *>(l_pText), color, R3_HAN_OUTLINE);
}

void GetScreenSize(int *po_pScreenX, int *po_pScreenY)
{
  if (po_pScreenX)
  {
    *po_pScreenX = 0;
  }

  if (po_pScreenY)
  {
    *po_pScreenY = 0;
  }

  _R3ENGINE_STATE *l_pState = GetR3State();
  if (!l_pState)
  {
    return;
  }

  if (po_pScreenX)
  {
    *po_pScreenX = static_cast<int>(l_pState->mScreenXsize);
  }

  if (po_pScreenY)
  {
    *po_pScreenY = static_cast<int>(l_pState->mScreenYsize);
  }
}

void DrawBox(int left, int top, int right, int bottom, DWORD fillColor, DWORD borderColor)
{
  Draw2DRectangle(left, top, right, bottom, fillColor);
  Draw2DRectangle(left, top, right, top + 2, borderColor);
  Draw2DRectangle(left, bottom - 2, right, bottom, borderColor);
  Draw2DRectangle(left, top, left + 2, bottom, borderColor);
  Draw2DRectangle(right - 2, top, right, bottom, borderColor);
}

void DrawSpriteFrame(CSprite *pi_pSprite,
                     DWORD pi_dwActionNo,
                     DWORD pi_dwFrameNo,
                     int pi_nX,
                     int pi_nY,
                     int pi_nWidth = -1,
                     int pi_nHeight = -1,
                     DWORD pi_dwARGB = 0xFFFFFFFF)
{
  if (!pi_pSprite)
  {
    return;
  }

  pi_pSprite->SetAction(pi_dwActionNo);
  pi_pSprite->SetFrame(pi_dwFrameNo);
  if (pi_nWidth < 0)
  {
    pi_nWidth = static_cast<int>(pi_pSprite->GetSpriteOrgXL(pi_dwActionNo, pi_dwFrameNo));
  }

  if (pi_nHeight < 0)
  {
    pi_nHeight = static_cast<int>(pi_pSprite->GetSpriteOrgYL(pi_dwActionNo, pi_dwFrameNo));
  }

  pi_pSprite->DrawSprite(pi_nX, pi_nY, pi_nX + pi_nWidth, pi_nY + pi_nHeight, pi_dwARGB);
}

int GetCenteredTextX(int pi_nCenterX, const char *pi_pText)
{
  if (!pi_pText)
  {
    return pi_nCenterX;
  }

  return pi_nCenterX - GetR3TextWidth(pi_pText) / 2;
}

void GetLoginMessageBoardSize(const char *pi_pMessage, int *po_pWidth, int *po_pHeight)
{
  int l_nMaxCharNum = 0;
  int l_nLineNum = pi_pMessage && pi_pMessage[0] ? 1 : 0;
  int l_nCurrentCharNum = 0;
  if (pi_pMessage)
  {
    for (int i = 0; pi_pMessage[i] != '\0'; ++i)
    {
      if (pi_pMessage[i] == '\n')
      {
        if (l_nCurrentCharNum > l_nMaxCharNum)
        {
          l_nMaxCharNum = l_nCurrentCharNum;
        }
        l_nCurrentCharNum = 0;
        ++l_nLineNum;
      }
      else
      {
        ++l_nCurrentCharNum;
      }
    }
  }

  if (l_nCurrentCharNum > l_nMaxCharNum)
  {
    l_nMaxCharNum = l_nCurrentCharNum;
  }

  if (l_nLineNum <= 0)
  {
    l_nLineNum = 1;
  }

  if (po_pWidth)
  {
    *po_pWidth = l_nMaxCharNum * kFontWidth + kMessageBoxTextMargin * 2;
  }
  if (po_pHeight)
  {
    *po_pHeight = l_nLineNum * (kFontHeight + kMessageBoxColumnGap) +
                  kMessageBoxTextMargin * 2;
  }
}

void DrawSpriteSheetCell(CSprite *pi_pSprite,
                         DWORD pi_dwActionNo,
                         DWORD pi_dwFrameNo,
                         int pi_nX,
                         int pi_nY,
                         int pi_nWidth,
                         int pi_nHeight,
                         WORD pi_wObjectNumInLine,
                         int pi_nCellFrame,
                         DWORD pi_dwARGB)
{
  if (!pi_pSprite || pi_nWidth <= 0 || pi_nHeight <= 0 || pi_wObjectNumInLine == 0)
  {
    return;
  }

  pi_pSprite->SetAction(pi_dwActionNo);
  pi_pSprite->SetFrame(pi_dwFrameNo);
  const WORD l_wSpriteXL = pi_pSprite->GetSpriteXL(pi_dwActionNo, pi_dwFrameNo);
  const WORD l_wSpriteYL = pi_pSprite->GetSpriteYL(pi_dwActionNo, pi_dwFrameNo);
  if (!l_wSpriteXL || !l_wSpriteYL)
  {
    return;
  }

  const float l_fUVSizeX = static_cast<float>(pi_nWidth) / static_cast<float>(l_wSpriteXL);
  const float l_fUVSizeY = static_cast<float>(pi_nHeight) / static_cast<float>(l_wSpriteYL);
  const int l_nCellX = pi_nCellFrame % pi_wObjectNumInLine;
  const int l_nCellY = pi_nCellFrame / pi_wObjectNumInLine;
  float l_afUV[2][2];
  l_afUV[0][0] = l_nCellX * l_fUVSizeX;
  l_afUV[0][1] = l_afUV[0][0] + l_fUVSizeX;
  l_afUV[1][0] = l_nCellY * l_fUVSizeY;
  l_afUV[1][1] = l_afUV[1][0] + l_fUVSizeY;
  pi_pSprite->DrawSpriteUV(pi_nX, pi_nY, l_afUV, pi_nX + pi_nWidth, pi_nY + pi_nHeight, pi_dwARGB);
}

bool IsPointInRect(const RECT &pi_sRect, int pi_nX, int pi_nY)
{
  return pi_nX >= pi_sRect.left &&
         pi_nX < pi_sRect.right &&
         pi_nY >= pi_sRect.top &&
         pi_nY < pi_sRect.bottom;
}

void OffsetRectY(RECT *pio_pRect, int pi_nOffsetY)
{
  if (!pio_pRect || !pi_nOffsetY)
  {
    return;
  }

  pio_pRect->top += pi_nOffsetY;
  pio_pRect->bottom += pi_nOffsetY;
}

int LerpInt(int pi_nStart, int pi_nEnd, DWORD pi_dwElapsed, DWORD pi_dwDuration)
{
  if (pi_dwElapsed >= pi_dwDuration || !pi_dwDuration)
  {
    return pi_nEnd;
  }

  const float l_fT = static_cast<float>(pi_dwElapsed) / static_cast<float>(pi_dwDuration);
  return pi_nStart + static_cast<int>((pi_nEnd - pi_nStart) * l_fT);
}

void UpdateSpriteSheetAnimation(CHARACTER_SELECT_ANIMATION *pio_pAnimation, WORD pi_wTotalFrameNo)
{
  if (!pio_pAnimation || !pi_wTotalFrameNo)
  {
    return;
  }

  if (pio_pAnimation->bStop)
  {
    if (timeGetTime() - pio_pAnimation->dwStopTime > kCharacterSelectAniStopInterval)
    {
      pio_pAnimation->bStop = false;
    }
    return;
  }

  pio_pAnimation->fCurFrameNo += R3GetLoopTime() * kCharacterSelectAniFPS;
  if (pio_pAnimation->fCurFrameNo >= pi_wTotalFrameNo)
  {
    pio_pAnimation->bStop = true;
    pio_pAnimation->dwStopTime = timeGetTime();
    pio_pAnimation->fCurFrameNo = 0.0f;
  }
}

void FormatCommaNumber(DWORD pi_dwValue, char *po_pBuffer, size_t pi_stBufferSize)
{
  if (!po_pBuffer || pi_stBufferSize == 0)
  {
    return;
  }

  char l_szPlain[32];
  sprintf_s(l_szPlain, sizeof(l_szPlain), "%lu", static_cast<unsigned long>(pi_dwValue));
  const int l_nPlainLen = static_cast<int>(strlen(l_szPlain));
  const int l_nCommaCount = (l_nPlainLen > 0) ? (l_nPlainLen - 1) / 3 : 0;
  const int l_nOutputLen = l_nPlainLen + l_nCommaCount;
  if (static_cast<size_t>(l_nOutputLen + 1) > pi_stBufferSize)
  {
    po_pBuffer[0] = '\0';
    return;
  }

  po_pBuffer[l_nOutputLen] = '\0';
  int l_nSource = l_nPlainLen - 1;
  int l_nDest = l_nOutputLen - 1;
  int l_nDigitCount = 0;
  while (l_nSource >= 0)
  {
    if (l_nDigitCount == 3)
    {
      po_pBuffer[l_nDest--] = ',';
      l_nDigitCount = 0;
    }

    po_pBuffer[l_nDest--] = l_szPlain[l_nSource--];
    ++l_nDigitCount;
  }
}

void CopyRegedAvatarName(const _REGED_AVATOR_DB *pi_pRegedAvatar, char *po_pBuffer, size_t pi_stBufferSize)
{
  if (!po_pBuffer || pi_stBufferSize == 0)
  {
    return;
  }

  po_pBuffer[0] = '\0';
  if (!pi_pRegedAvatar)
  {
    return;
  }

  size_t l_stCopySize = sizeof(pi_pRegedAvatar->m_wszAvatorName);
  if (l_stCopySize >= pi_stBufferSize)
  {
    l_stCopySize = pi_stBufferSize - 1;
  }

  memcpy(po_pBuffer, pi_pRegedAvatar->m_wszAvatorName, l_stCopySize);
  po_pBuffer[l_stCopySize] = '\0';
}

void DrawCharacterSelectButtonLabel(const RECT &pi_sButtonRect,
                                    const char *pi_pText,
                                    int pi_nOffsetX,
                                    int pi_nOffsetY,
                                    bool pi_bHovered,
                                    bool pi_bPressed)
{
  const int l_nPressedOffset = pi_bPressed ? 1 : 0;
  DrawLoginLine(pi_sButtonRect.left + pi_nOffsetX + l_nPressedOffset,
                pi_sButtonRect.top + pi_nOffsetY + l_nPressedOffset,
                pi_pText,
                (pi_bHovered || pi_bPressed) ? kCharacterSelectButtonHoverTextColor : kCharacterSelectButtonTextColor);
}

void DrawCenteredCharacterSelectButtonLabel(const RECT &pi_sButtonRect,
                                            const char *pi_pText,
                                            bool pi_bHovered,
                                            bool pi_bPressed)
{
  const int l_nButtonWidth = pi_sButtonRect.right - pi_sButtonRect.left;
  const int l_nButtonHeight = pi_sButtonRect.bottom - pi_sButtonRect.top;
  const int l_nPressedOffset = pi_bPressed ? 1 : 0;
  DrawLoginLine(GetCenteredTextX(pi_sButtonRect.left + (l_nButtonWidth / 2), pi_pText) + l_nPressedOffset,
                pi_sButtonRect.top + ((l_nButtonHeight - kFontHeight) / 2) + l_nPressedOffset,
                pi_pText,
                (pi_bHovered || pi_bPressed) ? kCharacterSelectButtonHoverTextColor : kCharacterSelectButtonTextColor);
}

void DrawCharacterSelectButtonLabelFixedColor(const RECT &pi_sButtonRect,
                                              const char *pi_pText,
                                              int pi_nOffsetX,
                                              int pi_nOffsetY,
                                              bool pi_bPressed,
                                              DWORD pi_dwColor)
{
  const int l_nPressedOffset = pi_bPressed ? 1 : 0;
  DrawLoginLine(pi_sButtonRect.left + pi_nOffsetX + l_nPressedOffset,
                pi_sButtonRect.top + pi_nOffsetY + l_nPressedOffset,
                pi_pText,
                pi_dwColor);
}

void DrawCenteredCharacterSelectButtonLabelFixedColor(const RECT &pi_sButtonRect,
                                                      const char *pi_pText,
                                                      bool pi_bPressed,
                                                      DWORD pi_dwColor)
{
  const int l_nButtonWidth = pi_sButtonRect.right - pi_sButtonRect.left;
  const int l_nButtonHeight = pi_sButtonRect.bottom - pi_sButtonRect.top;
  const int l_nPressedOffset = pi_bPressed ? 1 : 0;
  DrawLoginLine(GetCenteredTextX(pi_sButtonRect.left + (l_nButtonWidth / 2), pi_pText) + l_nPressedOffset,
                pi_sButtonRect.top + ((l_nButtonHeight - kFontHeight) / 2) + l_nPressedOffset,
                pi_pText,
                pi_dwColor);
}

void CopyCreditText(char *po_pTarget, size_t pi_nTargetSize, const char *pi_pSource)
{
  if (!po_pTarget || !pi_nTargetSize)
  {
    return;
  }

  po_pTarget[0] = '\0';
  if (!pi_pSource)
  {
    return;
  }

  strncpy_s(po_pTarget, pi_nTargetSize, pi_pSource, _TRUNCATE);
  for (size_t i = 0; po_pTarget[i]; ++i)
  {
    if (po_pTarget[i] == '_')
    {
      po_pTarget[i] = ' ';
    }
  }
}

DWORD GetCreditLineColor(int)
{
  return 0xFFFFFFFF;
}

BYTE ClampCreateRaceSelection(BYTE pi_byRaceSelection)
{
  return pi_byRaceSelection < 3 ? pi_byRaceSelection : 0;
}

bool IsAccretiaCreateRace(BYTE pi_byRaceSelection)
{
  return ClampCreateRaceSelection(pi_byRaceSelection) == CHARACTER_CREATE_BUTTON_RACE_ACCRETIA;
}

BYTE GetNextCreateAttribute(BYTE pi_byAttribute, BYTE pi_byRaceSelection, bool pi_bForward)
{
  BYTE l_byAttribute = pi_byAttribute < 4 ? pi_byAttribute : 0;
  for (int i = 0; i < 4; ++i)
  {
    l_byAttribute = pi_bForward
                      ? static_cast<BYTE>((l_byAttribute + 1) % 4)
                      : static_cast<BYTE>((l_byAttribute == 0) ? 3 : l_byAttribute - 1);
    if (!IsAccretiaCreateRace(pi_byRaceSelection) ||
        l_byAttribute != CHARACTER_CREATE_ATTRIBUTE_FORCE)
    {
      return l_byAttribute;
    }
  }

  return CHARACTER_CREATE_ATTRIBUTE_MELEE;
}

const char *GetCreateRaceLabel(BYTE pi_byRaceSelection)
{
  static const char *kCreateRaceLabel[3] = { "BELLATO", "CORA", "ACCRETIA" };
  return kCreateRaceLabel[ClampCreateRaceSelection(pi_byRaceSelection)];
}

const char *GetCreateAttributeLabel(BYTE pi_byAttribute)
{
  static const char *kCreateAttributeLabel[4] = { "Warrior", "Ranger", "Spiritualist", "Specialist" };
  return pi_byAttribute < 4 ? kCreateAttributeLabel[pi_byAttribute] : "";
}

const char *GetCreateRaceBattleSkillLabel(BYTE pi_byRaceSelection)
{
  static const char *kRaceBattleSkillLabel[3] = { "MAU", "Summon", "Launcher" };
  return kRaceBattleSkillLabel[ClampCreateRaceSelection(pi_byRaceSelection)];
}

void DrawGaugeBar(const RECT &pi_sBaseRect, float pi_fRate, DWORD pi_dwColor)
{
  if (pi_fRate < 0.0f)
  {
    pi_fRate = 0.0f;
  }
  else if (pi_fRate > 1.0f)
  {
    pi_fRate = 1.0f;
  }

  Draw2DRectangle(pi_sBaseRect.left,
                  pi_sBaseRect.top,
                  pi_sBaseRect.right,
                  pi_sBaseRect.bottom,
                  0xA0101620);
  Draw2DRectangle(pi_sBaseRect.left,
                  pi_sBaseRect.top,
                  pi_sBaseRect.right,
                  pi_sBaseRect.top + 1,
                  0xC08EA8C8);
  Draw2DRectangle(pi_sBaseRect.left,
                  pi_sBaseRect.bottom - 1,
                  pi_sBaseRect.right,
                  pi_sBaseRect.bottom,
                  0xC0203040);

  const int l_nWidth = pi_sBaseRect.right - pi_sBaseRect.left - 2;
  const int l_nFillWidth = static_cast<int>(static_cast<float>(l_nWidth) * pi_fRate);
  if (l_nFillWidth > 0)
  {
    Draw2DRectangle(pi_sBaseRect.left + 1,
                    pi_sBaseRect.top + 1,
                    pi_sBaseRect.left + 1 + l_nFillWidth,
                    pi_sBaseRect.bottom - 1,
                    pi_dwColor);
  }
}

void DrawCreateAttributeGaugeBar(CSprite *pi_pSprite,
                                 const RECT &pi_sBaseRect,
                                 float pi_fRate,
                                 DWORD pi_dwGaugeFrame)
{
  if (!pi_pSprite)
  {
    return;
  }

  if (pi_fRate < 0.0f)
  {
    pi_fRate = 0.0f;
  }
  else if (pi_fRate > 1.0f)
  {
    pi_fRate = 1.0f;
  }

  DrawSpriteFrame(pi_pSprite,
                  kCharacterCreateDetailAction,
                  26,
                  pi_sBaseRect.left,
                  pi_sBaseRect.top,
                  pi_sBaseRect.right - pi_sBaseRect.left,
                  pi_sBaseRect.bottom - pi_sBaseRect.top,
                  0xFFFFFFFF);

  const int l_nFillWidth = static_cast<int>(
    static_cast<float>(pi_sBaseRect.right - pi_sBaseRect.left - 2) * pi_fRate);
  if (l_nFillWidth <= 0)
  {
    return;
  }

  float l_afUV[2][2];
  l_afUV[0][0] = 0.0f;
  l_afUV[0][1] = pi_fRate;
  l_afUV[1][0] = 0.0f;
  l_afUV[1][1] = 1.0f;

  pi_pSprite->SetAction(kCharacterCreateDetailAction);
  pi_pSprite->SetFrame(pi_dwGaugeFrame);
  pi_pSprite->DrawSpriteUV(pi_sBaseRect.left + 1,
                           pi_sBaseRect.top + 1,
                           l_afUV,
                           pi_sBaseRect.left + 1 + l_nFillWidth,
                           pi_sBaseRect.bottom - 1,
                           0xFFFFFFFF);
}

bool LoadTextureFile(const char *pi_pPath, void **po_ppTexture)
{
  if (!pi_pPath || !pi_pPath[0] || !po_ppTexture || !GetD3dDevice())
  {
    return false;
  }

  LPDIRECT3DTEXTURE8 l_pTexture = NULL;
  if (FAILED(D3DXCreateTextureFromFileA(GetD3dDevice(), pi_pPath, &l_pTexture)))
  {
    return false;
  }

  *po_ppTexture = l_pTexture;
  return true;
}

bool IsShortcutModifierDown()
{
  return (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0 ||
         (GetAsyncKeyState(VK_MENU) & 0x8000) != 0 ||
         (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
}

const _REGED_AVATOR_DB *FindRegedAvatarBySlot(const _reged_char_result_zone &regedCharResult, BYTE slotIndex)
{
  for (int i = 0; i < regedCharResult.byCharNum; ++i)
  {
    if (regedCharResult.RegedList[i].m_bySlotIndex == slotIndex)
    {
      return &regedCharResult.RegedList[i];
    }
  }

  return NULL;
}

BYTE FindFirstRegedAvatarSlot(const _reged_char_result_zone &regedCharResult)
{
  for (int i = 0; i < regedCharResult.byCharNum; ++i)
  {
    if (regedCharResult.RegedList[i].m_bySlotIndex <= 2)
    {
      return regedCharResult.RegedList[i].m_bySlotIndex;
    }
  }

  return static_cast<BYTE>(0xFF);
}

BYTE FindPreferredRegedAvatarSlot(const CNetworkMgr *networkMgr)
{
  if (!networkMgr)
  {
    return static_cast<BYTE>(0xFF);
  }

  const _reged_char_result_zone &regedCharResult = networkMgr->GetRegedCharResult();
  if (networkMgr->GetLastestAvatarIndex() != static_cast<DWORD>(-1) &&
      FindRegedAvatarBySlot(regedCharResult, static_cast<BYTE>(networkMgr->GetLastestAvatarIndex())))
  {
    return static_cast<BYTE>(networkMgr->GetLastestAvatarIndex());
  }

  return FindFirstRegedAvatarSlot(regedCharResult);
}
} // namespace

CGP_LogIn::CGP_LogIn()
  : m_bySelectedCharacterSlot(0xFF),
    m_byScreenMode(LOGIN_SCREEN_OPENING),
    m_byMenuSelection(kInvalidMenuIndex),
    m_byMenuHover(kInvalidMenuIndex),
    m_byMenuPressed(kInvalidMenuIndex),
    m_byCharacterMenuHover(kInvalidCharacterButton),
    m_byCharacterMenuPressed(kInvalidCharacterButton),
    m_byCreateStep(CHARACTER_CREATE_STEP_RACE),
    m_byCreateRaceSelection(0),
    m_byCreateAttributeSelection(CHARACTER_CREATE_ATTRIBUTE_MELEE),
    m_byCreateSexSelection(0),
    m_byCreateMenuHover(kInvalidCharacterCreateButton),
    m_byCreateMenuPressed(kInvalidCharacterCreateButton),
    m_nMouseX(-1),
    m_nMouseY(-1),
    m_bLeftButtonDown(false),
    m_bLeftButtonPressed(false),
    m_bLeftButtonReleased(false),
    m_dwStartRequestTick(0),
    m_dwCharacterSelectionOpenTick(0),
    m_dwCharacterSelectionCloseTick(0),
    m_bCharacterSelectionUIVisible(false),
    m_bCharacterSelectionUIClosing(false),
    m_bCharacterDummiesLoaded(false),
    m_bStartRequested(false),
    m_bCreditsMode(false),
    m_bLoginMessageVisible(false),
    m_bLoginMessageOkHover(false),
    m_bLoginMessageOkPressed(false),
    m_dwCreditsStartTick(0),
    m_pTitleLogoTexture(NULL)
{
  ZeroMemory(m_abVirtualKeyState, sizeof(m_abVirtualKeyState));
  ZeroMemory(m_abyCreatePartVariant, sizeof(m_abyCreatePartVariant));
  ZeroMemory(m_szLoginMessage, sizeof(m_szLoginMessage));
  ZeroMemory(m_szCreateCharacterName, sizeof(m_szCreateCharacterName));
  ResetCharacterSelectionAnimation();
}

CGP_LogIn::~CGP_LogIn()
{
  Destroy();
}

BOOL CGP_LogIn::Create(void)
{
  if (m_bIsCreated)
  {
    return TRUE;
  }

  if (!CGameProgress::Create())
  {
    return FALSE;
  }

  if (!LoadData())
  {
    return FALSE;
  }

  m_bIsCreated = TRUE;
  return TRUE;
}

BOOL CGP_LogIn::Destroy(void)
{
  if (m_bIsLoadedData)
  {
    UnloadData();
  }

  m_bIsCreated = FALSE;
  return CGameProgress::Destroy();
}

HRESULT CGP_LogIn::RestoreDeviceObjects(void)
{
  if (_GetMainApp())
  {
    _GetMainApp()->LoadLoginLobbyData();
  }

  LoadLoginSprites();
  if (!m_pTitleLogoTexture)
  {
    LoadTextureFile(".\\system\\titlelogo.dds", &m_pTitleLogoTexture) ||
      LoadTextureFile(".\\system\\titlelogo.tga", &m_pTitleLogoTexture);
  }

  return S_OK;
}

HRESULT CGP_LogIn::InvalidateDeviceObjects(void)
{
  UnloadLoginSprites();
  ReleaseTitleLogoTexture();
  if (_GetMainApp())
  {
    _GetMainApp()->UnloadLoginLobbyData();
  }

  return S_OK;
}

LRESULT CGP_LogIn::MsgProc(HWND, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_MOUSEMOVE || uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP)
  {
    m_nMouseX = GET_X_LPARAM(lParam);
    m_nMouseY = GET_Y_LPARAM(lParam);
  }

  if (uMsg == WM_LBUTTONDOWN)
  {
    m_bLeftButtonDown = true;
    m_bLeftButtonPressed = true;
    return 0;
  }

  if (uMsg == WM_LBUTTONUP)
  {
    m_bLeftButtonDown = false;
    m_bLeftButtonReleased = true;
    return 0;
  }

  if (m_byScreenMode == LOGIN_SCREEN_CHAR_CREATE &&
      m_byCreateStep == CHARACTER_CREATE_STEP_DETAIL &&
      uMsg == WM_CHAR)
  {
    AppendCreateCharacterInputChar(wParam);
    return 0;
  }

  if ((uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) &&
      (wParam == VK_RETURN || wParam == VK_SPACE))
  {
    return 0;
  }

  return 1;
}

HRESULT CGP_LogIn::FrameMove(void)
{
  if (_GetMainApp())
  {
    _GetMainApp()->FrameMoveLoginLobby();
  }

  UpdateWorldServerFlow();
  EnsureCharacterSelection();
  UpdateDisplayedScreen();
  CNetworkMgr *l_pNetworkMgr = _GetNetworkMgr();
  if (m_byScreenMode == LOGIN_SCREEN_CHAR_CREATE &&
      l_pNetworkMgr &&
      l_pNetworkMgr->HasAddCharResult() &&
      l_pNetworkMgr->GetAddCharResult().byRetCode == 0)
  {
    m_bySelectedCharacterSlot = l_pNetworkMgr->GetAddCharResult().byAddSlotIndex;
    m_bCharacterDummiesLoaded = false;
    LeaveCharacterCreateScreen(true);
  }
  UpdateCharacterSelectionUIState();
  if (m_byScreenMode == LOGIN_SCREEN_CHAR_SELECT)
  {
    if (IsCharacterSelectionUIVisible())
    {
      UpdateCharacterSelectionAnimation();
    }
  }
  else if (m_byScreenMode == LOGIN_SCREEN_CHAR_CREATE &&
           m_byCreateStep == CHARACTER_CREATE_STEP_RACE)
  {
    UpdateSpriteSheetAnimation(&m_sUpperBoardAnimation, 5);
  }
  return S_OK;
}

HRESULT CGP_LogIn::Render(void)
{
  if (!R3BeginScene())
  {
    return S_OK;
  }

  R3ClearFrameBuffer();
  if (ShouldRenderLoadingScreen())
  {
    RenderLoadingScreen();
    R3EndScene();
    return S_OK;
  }

  if (_GetMainApp())
  {
    _GetMainApp()->RenderLoginLobby();
  }

  RenderTitleLogo();

  if (m_byScreenMode == LOGIN_SCREEN_CHAR_SELECT)
  {
    if (IsCharacterSelectionUIVisible())
    {
      RenderCharacterSelectionScreen();
    }
  }
  else if (m_byScreenMode == LOGIN_SCREEN_CHAR_CREATE)
  {
    RenderCharacterCreateScreen();
  }
  else if (m_byScreenMode == LOGIN_SCREEN_UI_LOCK)
  {
    RenderUILockScreen();
  }
  else if (m_bCreditsMode)
  {
    RenderCreditsRoll();
  }
  else
  {
    RenderOpeningScreen();
  }

  RenderStatusOverlay();
  RenderLoginMessageBox();
  R3EndScene();
  return S_OK;
}

BOOL CGP_LogIn::InputProcess(void)
{
  UpdateWorldServerFlow();
  EnsureCharacterSelection();
  UpdateDisplayedScreen();
  if (ShouldRenderLoadingScreen())
  {
    return TRUE;
  }

  if (IsLoginMessageVisible())
  {
    UpdateLoginMessageInput();
    return TRUE;
  }

  if (m_byScreenMode == LOGIN_SCREEN_CHAR_SELECT)
  {
    UpdateCharacterSelectionInput();
  }
  else if (m_byScreenMode == LOGIN_SCREEN_CHAR_CREATE)
  {
    UpdateCharacterCreateInput();
  }
  else
  {
    UpdateOpeningInput();
  }

  return TRUE;
}

BOOL CGP_LogIn::LoadData(void)
{
  if (!_GetMainApp() || !_GetNetworkMgr())
  {
    return FALSE;
  }

  _GetMainApp()->SetClientWindowVisible(TRUE);
  m_bySelectedCharacterSlot = 0xFF;
  m_byScreenMode = LOGIN_SCREEN_OPENING;
  m_byMenuSelection = kInvalidMenuIndex;
  m_byMenuHover = kInvalidMenuIndex;
  m_byMenuPressed = kInvalidMenuIndex;
  m_byCharacterMenuHover = kInvalidCharacterButton;
  m_byCharacterMenuPressed = kInvalidCharacterButton;
  ResetCharacterCreateState();
  m_nMouseX = -1;
  m_nMouseY = -1;
  m_bLeftButtonDown = false;
  m_bLeftButtonPressed = false;
  m_bLeftButtonReleased = false;
  m_dwStartRequestTick = 0;
  m_dwCharacterSelectionOpenTick = 0;
  m_dwCharacterSelectionCloseTick = 0;
  m_bCharacterSelectionUIVisible = false;
  m_bCharacterSelectionUIClosing = false;
  m_bCharacterDummiesLoaded = false;
  m_bStartRequested = false;
  m_bCreditsMode = false;
  m_dwCreditsStartTick = 0;
  m_vecTitleCredits.clear();
  ZeroMemory(m_abVirtualKeyState, sizeof(m_abVirtualKeyState));
  ResetCharacterSelectionAnimation();
  LoadLoginSprites();
  PresentLoadingScreen();

  if (!_GetMainApp()->LoadLoginLobbyData())
  {
    _GetNetworkMgr()->SetStatusText("Failed to load .\\map\\lobby\\lobby.BSP for login bootstrap");
  }

  ReleaseTitleLogoTexture();
  LoadTextureFile(".\\system\\titlelogo.dds", &m_pTitleLogoTexture) ||
    LoadTextureFile(".\\system\\titlelogo.tga", &m_pTitleLogoTexture);
  m_bIsLoadedData = TRUE;
  return TRUE;
}

BOOL CGP_LogIn::UnloadData(void)
{
  StopCreditsRoll();
  UnloadLoginSprites();
  ReleaseTitleLogoTexture();
  if (_GetMainApp())
  {
    _GetMainApp()->ClearLoginLobbyCharacterDummies();
    _GetMainApp()->UnloadLoginLobbyData();
  }
  m_bCharacterDummiesLoaded = false;

  m_bIsLoadedData = FALSE;
  return TRUE;
}

BOOL CGP_LogIn::ConsumeKeyPress(int pi_nVirtualKey)
{
  if (pi_nVirtualKey < 0 || pi_nVirtualKey >= 256)
  {
    return FALSE;
  }

  const bool l_bIsDown = (GetAsyncKeyState(pi_nVirtualKey) & 0x8000) != 0;
  const bool l_bWasDown = m_abVirtualKeyState[pi_nVirtualKey];
  m_abVirtualKeyState[pi_nVirtualKey] = l_bIsDown;
  return (l_bIsDown && !l_bWasDown) ? TRUE : FALSE;
}

void CGP_LogIn::UpdateWorldServerFlow(void)
{
  if (!_GetMainApp() || !_GetNetworkMgr())
  {
    return;
  }

  CNetworkMgr *l_pNetworkMgr = _GetMainApp()->GetNetworkMgr();
  if (!l_pNetworkMgr || !l_pNetworkMgr->HasLauncherData())
  {
    return;
  }

  if (!l_pNetworkMgr->IsConnectedToServer(GST_WORLD) && !l_pNetworkMgr->IsConnectingToServer())
  {
    l_pNetworkMgr->Connect(GST_WORLD);
    return;
  }

  if (l_pNetworkMgr->IsConnectedToServer(GST_WORLD) &&
      !l_pNetworkMgr->HasSentEnterWorldRequest())
  {
    l_pNetworkMgr->SystemMsg_EnterWorldRequest_zone();
    return;
  }

  if (!l_pNetworkMgr->HasEnterWorldResult() ||
      l_pNetworkMgr->GetResultOfEnterTheWorldServer() != WORLD_ENTER_SUCCESS)
  {
    return;
  }

  if (!l_pNetworkMgr->HasSentRegedCharRequest())
  {
    l_pNetworkMgr->SystemMsg_RegedCharRequest_zone();
  }
}

void CGP_LogIn::EnsureCharacterSelection(void)
{
  CNetworkMgr *l_pNetworkMgr = _GetNetworkMgr();
  if (!l_pNetworkMgr ||
      !l_pNetworkMgr->HasRegedCharResult() ||
      l_pNetworkMgr->GetResultOfUserInfo() != USER_INFO_SUCCESS)
  {
    return;
  }

  if (m_bySelectedCharacterSlot > 2)
  {
    const BYTE l_byPreferredSlot = FindPreferredRegedAvatarSlot(l_pNetworkMgr);
    m_bySelectedCharacterSlot = (l_byPreferredSlot == static_cast<BYTE>(0xFF)) ? 0 : l_byPreferredSlot;
  }

  if (!m_bCharacterDummiesLoaded && _GetMainApp())
  {
    _GetMainApp()->BuildLoginLobbyCharacterDummies(l_pNetworkMgr->GetRegedCharResult());
    m_bCharacterDummiesLoaded = true;
  }

  if (!m_bStartRequested || !IsUILockResolved())
  {
    return;
  }
}

void CGP_LogIn::UpdateDisplayedScreen(void)
{
  const BYTE l_byOldScreenMode = m_byScreenMode;
  BYTE l_byNewScreenMode = LOGIN_SCREEN_OPENING;

  if (!m_bStartRequested)
  {
    l_byNewScreenMode = LOGIN_SCREEN_OPENING;
  }
  else
  {
    CNetworkMgr *l_pNetworkMgr = _GetNetworkMgr();
    if (!l_pNetworkMgr ||
        !l_pNetworkMgr->HasRegedCharResult() ||
        l_pNetworkMgr->GetResultOfUserInfo() != USER_INFO_SUCCESS)
    {
      l_byNewScreenMode = LOGIN_SCREEN_OPENING;
    }
    else
    {
      if (IsUILockResolved())
      {
        l_byNewScreenMode = (m_byScreenMode == LOGIN_SCREEN_CHAR_CREATE)
                              ? LOGIN_SCREEN_CHAR_CREATE
                              : LOGIN_SCREEN_CHAR_SELECT;
      }
      else
      {
        l_byNewScreenMode = LOGIN_SCREEN_UI_LOCK;
      }
    }
  }

  m_byScreenMode = l_byNewScreenMode;
  if (l_byOldScreenMode == m_byScreenMode)
  {
    return;
  }

  if (m_byScreenMode == LOGIN_SCREEN_CHAR_SELECT)
  {
    m_dwCharacterSelectionOpenTick = 0;
    m_dwCharacterSelectionCloseTick = 0;
    m_bCharacterSelectionUIVisible = false;
    m_bCharacterSelectionUIClosing = false;
    m_byCharacterMenuHover = kInvalidCharacterButton;
    m_byCharacterMenuPressed = kInvalidCharacterButton;
    ResetCharacterSelectionAnimation();
    if (_GetMainApp())
    {
      _GetMainApp()->PlayLoginLobbyCharacterEntryCamera(m_bySelectedCharacterSlot);
    }
  }
  else if (m_byScreenMode == LOGIN_SCREEN_CHAR_CREATE)
  {
    m_dwCharacterSelectionOpenTick = 0;
    m_dwCharacterSelectionCloseTick = 0;
    m_bCharacterSelectionUIVisible = false;
    m_bCharacterSelectionUIClosing = false;
    m_byCharacterMenuHover = kInvalidCharacterButton;
    m_byCharacterMenuPressed = kInvalidCharacterButton;
  }
  else if (m_byScreenMode == LOGIN_SCREEN_OPENING && _GetMainApp())
  {
    m_dwCharacterSelectionOpenTick = 0;
    m_dwCharacterSelectionCloseTick = 0;
    m_bCharacterSelectionUIVisible = false;
    m_bCharacterSelectionUIClosing = false;
    _GetMainApp()->SetLoginLobbyOpeningCamera();
  }
  else
  {
    m_dwCharacterSelectionOpenTick = 0;
    m_dwCharacterSelectionCloseTick = 0;
    m_bCharacterSelectionUIVisible = false;
    m_bCharacterSelectionUIClosing = false;
  }
}

void CGP_LogIn::UpdateOpeningInput(void)
{
  const BOOL l_bUpPressed = ConsumeKeyPress(VK_UP);
  const BOOL l_bDownPressed = ConsumeKeyPress(VK_DOWN);
  const BOOL l_bLeftPressed = ConsumeKeyPress(VK_LEFT);
  const BOOL l_bRightPressed = ConsumeKeyPress(VK_RIGHT);
  const BOOL l_bConfirmPressed = ConsumeKeyPress(VK_RETURN) || ConsumeKeyPress(VK_SPACE);
  const BOOL l_bEscapePressed = ConsumeKeyPress(VK_ESCAPE);

  if (m_bCreditsMode)
  {
    if (l_bConfirmPressed || l_bEscapePressed || m_bLeftButtonReleased)
    {
      StopCreditsRoll();
    }
    ClearOpeningMouseTransitions();
    return;
  }

  if (l_bEscapePressed)
  {
    if (m_bStartRequested && m_byScreenMode != LOGIN_SCREEN_OPENING)
    {
      m_bStartRequested = false;
      m_byScreenMode = LOGIN_SCREEN_OPENING;
    }

    return;
  }

  if (l_bUpPressed || l_bLeftPressed)
  {
    m_byMenuSelection = (m_byMenuSelection == kInvalidMenuIndex || m_byMenuSelection == 0)
                          ? 2
                          : static_cast<BYTE>(m_byMenuSelection - 1);
  }
  else if (l_bDownPressed || l_bRightPressed)
  {
    m_byMenuSelection = (m_byMenuSelection == kInvalidMenuIndex || m_byMenuSelection >= 2)
                          ? 0
                          : static_cast<BYTE>(m_byMenuSelection + 1);
  }

  UpdateOpeningMouseInput();

  if (!l_bConfirmPressed)
  {
    return;
  }

  if (m_byMenuSelection != kInvalidMenuIndex)
  {
    ActivateOpeningMenuItem(m_byMenuSelection);
  }
}

void CGP_LogIn::UpdateCharacterSelectionInput(void)
{
  const BOOL l_bLeftPressed = ConsumeKeyPress(VK_LEFT);
  const BOOL l_bRightPressed = ConsumeKeyPress(VK_RIGHT);
  const BOOL l_bConfirmPressed = ConsumeKeyPress(VK_RETURN) || ConsumeKeyPress(VK_SPACE);
  const BOOL l_bEscapePressed = ConsumeKeyPress(VK_ESCAPE);

  if (IsLoginMessageVisible())
  {
    UpdateLoginMessageInput();
    return;
  }

  CNetworkMgr *l_pNetworkMgr = _GetNetworkMgr();
  if (!l_pNetworkMgr ||
      l_pNetworkMgr->GetResultOfUserInfo() != USER_INFO_SUCCESS ||
      IsShortcutModifierDown())
  {
    ClearOpeningMouseTransitions();
    return;
  }

  if (m_bySelectedCharacterSlot > 2)
  {
    m_bySelectedCharacterSlot = 0;
  }

  if (!IsCharacterSelectionUIInteractive())
  {
    m_byCharacterMenuHover = kInvalidCharacterButton;
    m_byCharacterMenuPressed = kInvalidCharacterButton;
    ClearOpeningMouseTransitions();
    return;
  }

  UpdateCharacterSelectionMouseInput();

  if (l_bEscapePressed)
  {
    m_bStartRequested = false;
    m_byScreenMode = LOGIN_SCREEN_OPENING;
    m_dwCharacterSelectionOpenTick = 0;
    m_dwCharacterSelectionCloseTick = 0;
    m_bCharacterSelectionUIVisible = false;
    m_bCharacterSelectionUIClosing = false;
    if (_GetMainApp())
    {
      _GetMainApp()->ClearLoginLobbyCharacterDummies();
      _GetMainApp()->SetLoginLobbyOpeningCamera();
    }
    m_bCharacterDummiesLoaded = false;
  }
  else if (l_bLeftPressed)
  {
    StartCharacterSelectionUIClose();
    MoveCharacterSelectionSlot(false);
  }
  else if (l_bRightPressed)
  {
    StartCharacterSelectionUIClose();
    MoveCharacterSelectionSlot(true);
  }
  else if (l_bConfirmPressed)
  {
    SendSelectedCharacterRequest();
  }
}

void CGP_LogIn::UpdateCharacterSelectionUIState(void)
{
  if (m_byScreenMode != LOGIN_SCREEN_CHAR_SELECT)
  {
    return;
  }

  if (m_dwCharacterSelectionOpenTick &&
      timeGetTime() - m_dwCharacterSelectionOpenTick >= kCharacterSelectBoardMoveTime)
  {
    m_dwCharacterSelectionOpenTick = 0;
  }

  if (m_bCharacterSelectionUIClosing && m_dwCharacterSelectionCloseTick)
  {
    if (timeGetTime() - m_dwCharacterSelectionCloseTick >= kCharacterSelectBoardCloseTime)
    {
      m_bCharacterSelectionUIClosing = false;
      m_bCharacterSelectionUIVisible = false;
      m_dwCharacterSelectionCloseTick = 0;
    }
    return;
  }

  if (!m_bCharacterSelectionUIVisible &&
      (!_GetMainApp() || !_GetMainApp()->IsLoginLobbyCameraAnimating()))
  {
    StartCharacterSelectionUIOpen();
  }
}

void CGP_LogIn::StartCharacterSelectionUIOpen(void)
{
  m_bCharacterSelectionUIVisible = true;
  m_bCharacterSelectionUIClosing = false;
  m_dwCharacterSelectionCloseTick = 0;
  m_dwCharacterSelectionOpenTick = timeGetTime();
  m_byCharacterMenuHover = kInvalidCharacterButton;
  m_byCharacterMenuPressed = kInvalidCharacterButton;
  ResetCharacterSelectionAnimation();
}

void CGP_LogIn::StartCharacterSelectionUIClose(void)
{
  if (!m_bCharacterSelectionUIVisible || m_bCharacterSelectionUIClosing)
  {
    return;
  }

  m_bCharacterSelectionUIClosing = true;
  m_dwCharacterSelectionCloseTick = timeGetTime();
  m_dwCharacterSelectionOpenTick = 0;
  m_byCharacterMenuHover = kInvalidCharacterButton;
  m_byCharacterMenuPressed = kInvalidCharacterButton;
}

bool CGP_LogIn::IsCharacterSelectionUIVisible(void) const
{
  return m_bCharacterSelectionUIVisible;
}

bool CGP_LogIn::IsCharacterSelectionUIInteractive(void) const
{
  if (!m_bCharacterSelectionUIVisible || m_bCharacterSelectionUIClosing)
  {
    return false;
  }

  if (m_dwCharacterSelectionOpenTick &&
      timeGetTime() - m_dwCharacterSelectionOpenTick < kCharacterSelectBoardMoveTime)
  {
    return false;
  }

  return !_GetMainApp() || !_GetMainApp()->IsLoginLobbyCameraAnimating();
}

bool CGP_LogIn::IsUILockResolved(void) const
{
  return true;
}

void CGP_LogIn::ReleaseTitleLogoTexture(void)
{
  if (!m_pTitleLogoTexture)
  {
    return;
  }

  reinterpret_cast<LPDIRECT3DTEXTURE8>(m_pTitleLogoTexture)->Release();
  m_pTitleLogoTexture = NULL;
}

void CGP_LogIn::LoadLoginSprites(void)
{
  m_cLoginSpriteMgr.Init();
  m_cLoginSpriteMgr.LoadSprite(SP_ID_COMMON);
  m_cLoginSpriteMgr.LoadSprite(SP_ID_LOGIN);
  m_cLoginSpriteMgr.LoadSprite(SP_ID_LOADING);
}

void CGP_LogIn::UnloadLoginSprites(void)
{
  m_cLoginSpriteMgr.UnloadSpriteAll();
}

void CGP_LogIn::StartCreditsRoll(void)
{
  LoadCreditsFile();
  m_bCreditsMode = !m_vecTitleCredits.empty();
  m_dwCreditsStartTick = timeGetTime();
}

void CGP_LogIn::StopCreditsRoll(void)
{
  m_bCreditsMode = false;
  m_dwCreditsStartTick = 0;
}

void CGP_LogIn::LoadCreditsFile(void)
{
  if (!m_vecTitleCredits.empty())
  {
    return;
  }

  FILE *l_pStream = NULL;
  if (fopen_s(&l_pStream, ".\\system\\staff.txt", "rt") != 0 || !l_pStream)
  {
    return;
  }

  char l_szToken[256];
  char l_szCurrentLine[64];
  l_szCurrentLine[0] = '\0';
  int l_nCurrentStyle = 0;

  while (fscanf_s(l_pStream, "%255s", l_szToken, static_cast<unsigned>(sizeof(l_szToken))) == 1)
  {
    if (l_szToken[0] == '#')
    {
      if (l_szToken[1] != '\0')
      {
        if (l_szCurrentLine[0] != '\0')
        {
          TITLE_CREDIT_LINE l_sLine;
          CopyCreditText(l_sLine.szText, sizeof(l_sLine.szText), l_szCurrentLine);
          l_sLine.nStyle = l_nCurrentStyle;
          m_vecTitleCredits.push_back(l_sLine);
        }

        l_nCurrentStyle = l_szToken[1] - '0';
        CopyCreditText(l_szCurrentLine, sizeof(l_szCurrentLine), l_szToken + 2);
      }
      else
      {
        if (l_szCurrentLine[0] != '\0')
        {
          TITLE_CREDIT_LINE l_sLine;
          CopyCreditText(l_sLine.szText, sizeof(l_sLine.szText), l_szCurrentLine);
          l_sLine.nStyle = l_nCurrentStyle;
          m_vecTitleCredits.push_back(l_sLine);
          l_szCurrentLine[0] = '\0';
        }

        TITLE_CREDIT_LINE l_sBlankLine;
        l_sBlankLine.szText[0] = '\0';
        l_sBlankLine.nStyle = -1;
        m_vecTitleCredits.push_back(l_sBlankLine);
      }
    }
    else
    {
      if (l_szCurrentLine[0] != '\0')
      {
        strncat_s(l_szCurrentLine, sizeof(l_szCurrentLine), " ", _TRUNCATE);
      }
      strncat_s(l_szCurrentLine, sizeof(l_szCurrentLine), l_szToken, _TRUNCATE);
    }
  }

  if (l_szCurrentLine[0] != '\0')
  {
    TITLE_CREDIT_LINE l_sLine;
    CopyCreditText(l_sLine.szText, sizeof(l_sLine.szText), l_szCurrentLine);
    l_sLine.nStyle = l_nCurrentStyle;
    m_vecTitleCredits.push_back(l_sLine);
  }

  fclose(l_pStream);
}

void CGP_LogIn::RenderTitleLogo(void) const
{
  if (!m_pTitleLogoTexture ||
      m_byScreenMode == LOGIN_SCREEN_CHAR_SELECT ||
      m_byScreenMode == LOGIN_SCREEN_CHAR_CREATE)
  {
    return;
  }

  int l_nScreenX = 0;
  int l_nScreenY = 0;
  GetScreenSize(&l_nScreenX, &l_nScreenY);
  if (l_nScreenX <= 0 || l_nScreenY <= 0)
  {
    return;
  }

  LPDIRECT3DTEXTURE8 l_pTexture = reinterpret_cast<LPDIRECT3DTEXTURE8>(m_pTitleLogoTexture);
  D3DSURFACE_DESC l_sDesc;
  if (FAILED(l_pTexture->GetLevelDesc(0, &l_sDesc)) || !l_sDesc.Width || !l_sDesc.Height)
  {
    return;
  }

  int l_nMaxLogoWidth = 1600;
  int l_nMaxLogoHeight = 512;
  if (static_cast<int>(l_sDesc.Width) < l_nMaxLogoWidth)
  {
    l_nMaxLogoWidth = static_cast<int>(l_sDesc.Width);
  }
  if (static_cast<int>(l_sDesc.Height) < l_nMaxLogoHeight)
  {
    l_nMaxLogoHeight = static_cast<int>(l_sDesc.Height);
  }

  int l_nLogoWidth = static_cast<int>(l_sDesc.Width);
  int l_nLogoHeight = static_cast<int>(l_sDesc.Height);
  if (l_nScreenX < l_nMaxLogoWidth)
  {
    l_nLogoWidth = static_cast<int>(static_cast<double>(l_sDesc.Width) *
                                    static_cast<double>(l_nScreenX) /
                                    static_cast<double>(l_nMaxLogoWidth));
    l_nLogoHeight = l_nLogoWidth * static_cast<int>(l_sDesc.Height) / static_cast<int>(l_sDesc.Width);
  }

  const int l_nLogoLeft = (l_nScreenX - l_nLogoWidth) / 2;
  const int l_nLogoTop = static_cast<int>(static_cast<double>(l_nScreenY) * 0.43 -
                                          static_cast<double>(l_nLogoHeight / 2));
  Draw2DTexture(l_nLogoLeft,
                l_nLogoTop,
                l_nLogoLeft + l_nLogoWidth,
                l_nLogoTop + l_nLogoHeight,
                0xFEFFFFFF,
                m_pTitleLogoTexture);
}

void CGP_LogIn::RenderStatusOverlay(void) const
{
  return;
}

bool CGP_LogIn::IsStartupLoadingComplete(void) const
{
  CNetworkMgr *l_pNetworkMgr = _GetNetworkMgr();
  if (!l_pNetworkMgr)
  {
    return false;
  }

  if (l_pNetworkMgr->HasRegedCharResult())
  {
    return true;
  }

  if (!l_pNetworkMgr->HasLauncherData())
  {
    return false;
  }

  if (!l_pNetworkMgr->IsConnectedToServer(GST_WORLD))
  {
    return false;
  }

  return l_pNetworkMgr->HasEnterWorldResult() &&
         l_pNetworkMgr->GetResultOfEnterTheWorldServer() == WORLD_ENTER_SUCCESS;
}

bool CGP_LogIn::ShouldRenderLoadingScreen(void) const
{
  if (!m_bStartRequested)
  {
    return false;
  }

  CNetworkMgr *l_pNetworkMgr = _GetNetworkMgr();
  if (l_pNetworkMgr && l_pNetworkMgr->HasRegedCharResult())
  {
    return false;
  }

  if (!IsStartupLoadingComplete())
  {
    return true;
  }

  return !l_pNetworkMgr || !l_pNetworkMgr->HasRegedCharResult();
}

void CGP_LogIn::RenderLoadingScreen(void) const
{
  int l_nScreenX = 0;
  int l_nScreenY = 0;
  GetScreenSize(&l_nScreenX, &l_nScreenY);
  if (l_nScreenX <= 0 || l_nScreenY <= 0)
  {
    return;
  }

  DrawSpriteFrame(m_cLoginSpriteMgr.GetSprite(SP_ID_LOADING), 0, 0, 0, 0, l_nScreenX, l_nScreenY);
}

void CGP_LogIn::PresentLoadingScreen(void) const
{
  if (!R3BeginScene())
  {
    return;
  }

  R3ClearFrameBuffer();
  RenderLoadingScreen();
  R3EndScene();
  GetD3dDevice()->Present(NULL, NULL, NULL, NULL);
}

bool CGP_LogIn::GetOpeningMenuLayout(RECT *po_pPanelRect, RECT po_aButtonRect[3]) const
{
  int l_nScreenX = 0;
  int l_nScreenY = 0;
  GetScreenSize(&l_nScreenX, &l_nScreenY);
  if (l_nScreenX <= 0 || l_nScreenY <= 0)
  {
    return false;
  }

  CSprite *l_pLoginSprite = m_cLoginSpriteMgr.GetSprite(SP_ID_LOGIN);
  if (!l_pLoginSprite)
  {
    return false;
  }

  const int l_nButtonWidth = static_cast<int>(l_pLoginSprite->GetSpriteOrgXL(kOpeningMenuAction, kOpeningMenuButtonNormalFrame));
  const int l_nButtonHeight = static_cast<int>(l_pLoginSprite->GetSpriteOrgYL(kOpeningMenuAction, kOpeningMenuButtonNormalFrame));
  const int l_nPanelWidth = l_nButtonWidth + 30;
  const int l_nPanelHeight = (3 * (l_nButtonHeight + 9)) + 47;
  const int l_nPanelLeft = static_cast<int>((l_nScreenX - l_nPanelWidth) * 0.5);
  const int l_nPanelTop = static_cast<int>((l_nScreenY - l_nPanelHeight) * 0.9);
  const int l_nButtonLeft = l_nPanelLeft + (l_nPanelWidth - l_nButtonWidth) / 2;
  const int l_nButtonTop = l_nPanelTop + 33;

  if (po_pPanelRect)
  {
    SetRect(po_pPanelRect,
            l_nPanelLeft,
            l_nPanelTop,
            l_nPanelLeft + l_nPanelWidth,
            l_nPanelTop + l_nPanelHeight);
  }

  if (po_aButtonRect)
  {
    for (int i = 0; i < 3; ++i)
    {
      const int l_nCurrentTop = l_nButtonTop + (i * (l_nButtonHeight + 9));
      SetRect(&po_aButtonRect[i],
              l_nButtonLeft,
              l_nCurrentTop,
              l_nButtonLeft + l_nButtonWidth,
              l_nCurrentTop + l_nButtonHeight);
    }
  }

  return true;
}

BYTE CGP_LogIn::GetOpeningMenuItemAtPoint(int pi_nX, int pi_nY) const
{
  RECT l_aButtonRect[3];
  if (!GetOpeningMenuLayout(NULL, l_aButtonRect))
  {
    return kInvalidMenuIndex;
  }

  POINT l_ptMouse;
  l_ptMouse.x = pi_nX;
  l_ptMouse.y = pi_nY;
  for (int i = 0; i < 3; ++i)
  {
    if (PtInRect(&l_aButtonRect[i], l_ptMouse))
    {
      return static_cast<BYTE>(i);
    }
  }

  return kInvalidMenuIndex;
}

void CGP_LogIn::UpdateOpeningMouseInput(void)
{
  m_byMenuHover = GetOpeningMenuItemAtPoint(m_nMouseX, m_nMouseY);

  if (m_bLeftButtonPressed)
  {
    m_byMenuPressed = m_byMenuHover;
    if (m_byMenuHover != kInvalidMenuIndex)
    {
      m_byMenuSelection = m_byMenuHover;
    }
  }

  if (m_bLeftButtonReleased)
  {
    const BYTE l_byPressedMenu = m_byMenuPressed;
    m_byMenuPressed = kInvalidMenuIndex;
    if (l_byPressedMenu != kInvalidMenuIndex && l_byPressedMenu == m_byMenuHover)
    {
      ActivateOpeningMenuItem(l_byPressedMenu);
    }
  }

  if (!m_bLeftButtonDown)
  {
    m_byMenuPressed = kInvalidMenuIndex;
  }

  ClearOpeningMouseTransitions();
}

void CGP_LogIn::ClearOpeningMouseTransitions(void)
{
  m_bLeftButtonPressed = false;
  m_bLeftButtonReleased = false;
}

void CGP_LogIn::ActivateOpeningMenuItem(BYTE pi_byMenuIndex)
{
  if (pi_byMenuIndex == 0)
  {
    m_bStartRequested = true;
    if (!m_dwStartRequestTick)
    {
      m_dwStartRequestTick = timeGetTime();
    }
  }
  else if (pi_byMenuIndex == 1)
  {
    StartCreditsRoll();
  }
  else if (pi_byMenuIndex == 2 && _GetMainApp())
  {
    _GetMainApp()->RequestQuitProgram();
  }
}

bool CGP_LogIn::GetCharacterSelectionLayout(CHARACTER_SELECT_LAYOUT *po_pLayout) const
{
  if (!po_pLayout)
  {
    return false;
  }

  int l_nScreenX = 0;
  int l_nScreenY = 0;
  GetScreenSize(&l_nScreenX, &l_nScreenY);
  if (l_nScreenX <= 0 || l_nScreenY <= 0)
  {
    return false;
  }

  const int l_nUpperBoardWidth = 588;
  const int l_nUpperBoardHeight = 173;
  const int l_nUpperBaseWidth = 544;
  const int l_nUpperBaseHeight = 94;
  const int l_nUpperAniWidth = 50;
  const int l_nUpperAniHeight = 50;
  const int l_nInfoBoardWidth = 161;
  const int l_nInfoBoardHeight = 93;
  const int l_nLowerBoardWidth = 471;
  const int l_nLowerBoardHeight = 159;
  const int l_nLowerBaseWidth = 167;
  const int l_nLowerBaseHeight = 117;
  const int l_nLowerAniWidth = 92;
  const int l_nLowerAniHeight = 92;
  const int l_nSmallButtonWidth = 140;
  const int l_nSmallButtonHeight = 36;
  const int l_nWideButtonWidth = 180;
  const int l_nWideButtonHeight = 44;
  const int l_nConnectButtonWidth = 64;
  const int l_nConnectButtonHeight = 64;
  const int l_nExitButtonWidth = 98;
  const int l_nExitButtonHeight = 38;

  SetRect(&po_pLayout->sUpperBoard,
          (l_nScreenX - l_nUpperBoardWidth) / 2,
          10,
          (l_nScreenX - l_nUpperBoardWidth) / 2 + l_nUpperBoardWidth,
          10 + l_nUpperBoardHeight);
  SetRect(&po_pLayout->sUpperBase,
          po_pLayout->sUpperBoard.left + ((l_nUpperBoardWidth - l_nUpperBaseWidth) / 2),
          po_pLayout->sUpperBoard.top,
          po_pLayout->sUpperBoard.left + ((l_nUpperBoardWidth - l_nUpperBaseWidth) / 2) + l_nUpperBaseWidth,
          po_pLayout->sUpperBoard.top + l_nUpperBaseHeight);
  SetRect(&po_pLayout->sUpperBoardAni,
          po_pLayout->sUpperBase.left + 247,
          po_pLayout->sUpperBase.top + 20,
          po_pLayout->sUpperBase.left + 247 + l_nUpperAniWidth,
          po_pLayout->sUpperBase.top + 20 + l_nUpperAniHeight);
  SetRect(&po_pLayout->sLeftCharInfoBoard,
          po_pLayout->sUpperBase.left + 139 - l_nInfoBoardWidth,
          po_pLayout->sUpperBase.top + 84,
          po_pLayout->sUpperBase.left + 139,
          po_pLayout->sUpperBase.top + 84 + l_nInfoBoardHeight);
  SetRect(&po_pLayout->sRightCharInfoBoard,
          po_pLayout->sUpperBase.left + 404,
          po_pLayout->sUpperBase.top + 84,
          po_pLayout->sUpperBase.left + 404 + l_nInfoBoardWidth,
          po_pLayout->sUpperBase.top + 84 + l_nInfoBoardHeight);
  SetRect(&po_pLayout->sNameText,
          po_pLayout->sLeftCharInfoBoard.left + 15,
          po_pLayout->sLeftCharInfoBoard.top + 25,
          po_pLayout->sLeftCharInfoBoard.right,
          po_pLayout->sLeftCharInfoBoard.top + 25 + kFontHeight);
  SetRect(&po_pLayout->sLevelText,
          po_pLayout->sLeftCharInfoBoard.left + 15,
          po_pLayout->sLeftCharInfoBoard.top + 55,
          po_pLayout->sLeftCharInfoBoard.right,
          po_pLayout->sLeftCharInfoBoard.top + 55 + kFontHeight);
  SetRect(&po_pLayout->sDalantText,
          po_pLayout->sRightCharInfoBoard.left + 13,
          po_pLayout->sRightCharInfoBoard.top + 25,
          po_pLayout->sRightCharInfoBoard.right,
          po_pLayout->sRightCharInfoBoard.top + 25 + kFontHeight);
  SetRect(&po_pLayout->sGoldText,
          po_pLayout->sRightCharInfoBoard.left + 13,
          po_pLayout->sRightCharInfoBoard.top + 55,
          po_pLayout->sRightCharInfoBoard.right,
          po_pLayout->sRightCharInfoBoard.top + 55 + kFontHeight);

  SetRect(&po_pLayout->sLowerBoard,
          (l_nScreenX - l_nLowerBoardWidth) / 2,
          l_nScreenY - l_nLowerBoardHeight - 10,
          (l_nScreenX - l_nLowerBoardWidth) / 2 + l_nLowerBoardWidth,
          l_nScreenY - 10);
  SetRect(&po_pLayout->sLowerBase,
          po_pLayout->sLowerBoard.left + ((l_nLowerBoardWidth - l_nLowerBaseWidth) / 2),
          po_pLayout->sLowerBoard.top + 20,
          po_pLayout->sLowerBoard.left + ((l_nLowerBoardWidth - l_nLowerBaseWidth) / 2) + l_nLowerBaseWidth,
          po_pLayout->sLowerBoard.top + 20 + l_nLowerBaseHeight);
  SetRect(&po_pLayout->sLowerBoardAni,
          po_pLayout->sLowerBase.left + 37,
          po_pLayout->sLowerBase.top,
          po_pLayout->sLowerBase.left + 37 + l_nLowerAniWidth,
          po_pLayout->sLowerBase.top + l_nLowerAniHeight);
  SetRect(&po_pLayout->sCreateButton,
          po_pLayout->sLowerBase.left + 28 - l_nWideButtonWidth,
          po_pLayout->sLowerBase.top + 24 - l_nWideButtonHeight,
          po_pLayout->sLowerBase.left + 28,
          po_pLayout->sLowerBase.top + 24);
  SetRect(&po_pLayout->sDeleteButton,
          po_pLayout->sLowerBase.left + 137,
          po_pLayout->sLowerBase.top + 24 - l_nWideButtonHeight,
          po_pLayout->sLowerBase.left + 137 + l_nWideButtonWidth,
          po_pLayout->sLowerBase.top + 24);
  SetRect(&po_pLayout->sPrevButton,
          po_pLayout->sLowerBase.left + 16 - l_nSmallButtonWidth,
          po_pLayout->sLowerBase.top + 72,
          po_pLayout->sLowerBase.left + 16,
          po_pLayout->sLowerBase.top + 72 + l_nSmallButtonHeight);
  SetRect(&po_pLayout->sNextButton,
          po_pLayout->sLowerBase.left + 149,
          po_pLayout->sLowerBase.top + 72,
          po_pLayout->sLowerBase.left + 149 + l_nSmallButtonWidth,
          po_pLayout->sLowerBase.top + 72 + l_nSmallButtonHeight);
  SetRect(&po_pLayout->sConnectButton,
          po_pLayout->sLowerBase.left + 51,
          po_pLayout->sLowerBase.top + 13,
          po_pLayout->sLowerBase.left + 51 + l_nConnectButtonWidth,
          po_pLayout->sLowerBase.top + 13 + l_nConnectButtonHeight);
  SetRect(&po_pLayout->sExitButton,
          po_pLayout->sLowerBase.left + 82 - (l_nExitButtonWidth / 2),
          po_pLayout->sLowerBase.top + 100,
          po_pLayout->sLowerBase.left + 82 - (l_nExitButtonWidth / 2) + l_nExitButtonWidth,
          po_pLayout->sLowerBase.top + 100 + l_nExitButtonHeight);

  return true;
}

void CGP_LogIn::ApplyCharacterSelectionOpenAnimation(CHARACTER_SELECT_LAYOUT *pio_pLayout) const
{
  if (!pio_pLayout)
  {
    return;
  }

  int l_nScreenX = 0;
  int l_nScreenY = 0;
  GetScreenSize(&l_nScreenX, &l_nScreenY);
  if (l_nScreenX <= 0 || l_nScreenY <= 0)
  {
    return;
  }

  const int l_nUpperHeight = pio_pLayout->sUpperBoard.bottom - pio_pLayout->sUpperBoard.top;
  const int l_nLowerHeight = pio_pLayout->sLowerBoard.bottom - pio_pLayout->sLowerBoard.top;
  int l_nUpperTop = pio_pLayout->sUpperBoard.top;
  int l_nLowerTop = pio_pLayout->sLowerBoard.top;

  if (m_bCharacterSelectionUIClosing && m_dwCharacterSelectionCloseTick)
  {
    const DWORD l_dwElapsed = timeGetTime() - m_dwCharacterSelectionCloseTick;
    l_nUpperTop = LerpInt(pio_pLayout->sUpperBoard.top, -l_nUpperHeight, l_dwElapsed, kCharacterSelectBoardCloseTime);
    l_nLowerTop = LerpInt(pio_pLayout->sLowerBoard.top, l_nScreenY, l_dwElapsed, kCharacterSelectBoardCloseTime);
  }
  else if (m_dwCharacterSelectionOpenTick)
  {
    const DWORD l_dwElapsed = timeGetTime() - m_dwCharacterSelectionOpenTick;
    l_nUpperTop = LerpInt(-l_nUpperHeight, pio_pLayout->sUpperBoard.top, l_dwElapsed, kCharacterSelectBoardMoveTime);
    l_nLowerTop = LerpInt(l_nScreenY, pio_pLayout->sLowerBoard.top, l_dwElapsed, kCharacterSelectBoardMoveTime);
  }
  else
  {
    return;
  }

  const int l_nUpperOffsetY = l_nUpperTop - pio_pLayout->sUpperBoard.top;
  const int l_nLowerOffsetY = l_nLowerTop - pio_pLayout->sLowerBoard.top;

  OffsetRectY(&pio_pLayout->sUpperBoard, l_nUpperOffsetY);
  OffsetRectY(&pio_pLayout->sUpperBase, l_nUpperOffsetY);
  OffsetRectY(&pio_pLayout->sUpperBoardAni, l_nUpperOffsetY);
  OffsetRectY(&pio_pLayout->sLeftCharInfoBoard, l_nUpperOffsetY);
  OffsetRectY(&pio_pLayout->sRightCharInfoBoard, l_nUpperOffsetY);
  OffsetRectY(&pio_pLayout->sNameText, l_nUpperOffsetY);
  OffsetRectY(&pio_pLayout->sLevelText, l_nUpperOffsetY);
  OffsetRectY(&pio_pLayout->sDalantText, l_nUpperOffsetY);
  OffsetRectY(&pio_pLayout->sGoldText, l_nUpperOffsetY);

  OffsetRectY(&pio_pLayout->sLowerBoard, l_nLowerOffsetY);
  OffsetRectY(&pio_pLayout->sLowerBase, l_nLowerOffsetY);
  OffsetRectY(&pio_pLayout->sLowerBoardAni, l_nLowerOffsetY);
  OffsetRectY(&pio_pLayout->sCreateButton, l_nLowerOffsetY);
  OffsetRectY(&pio_pLayout->sDeleteButton, l_nLowerOffsetY);
  OffsetRectY(&pio_pLayout->sPrevButton, l_nLowerOffsetY);
  OffsetRectY(&pio_pLayout->sNextButton, l_nLowerOffsetY);
  OffsetRectY(&pio_pLayout->sConnectButton, l_nLowerOffsetY);
  OffsetRectY(&pio_pLayout->sExitButton, l_nLowerOffsetY);
}

BYTE CGP_LogIn::GetCharacterSelectionButtonAtPoint(int pi_nX, int pi_nY) const
{
  CHARACTER_SELECT_LAYOUT l_sLayout;
  if (!GetCharacterSelectionLayout(&l_sLayout))
  {
    return kInvalidCharacterButton;
  }
  ApplyCharacterSelectionOpenAnimation(&l_sLayout);

  if (IsPointInRect(l_sLayout.sPrevButton, pi_nX, pi_nY))
  {
    return CHARACTER_SELECT_BUTTON_PREV;
  }
  if (IsPointInRect(l_sLayout.sNextButton, pi_nX, pi_nY))
  {
    return CHARACTER_SELECT_BUTTON_NEXT;
  }
  if (IsPointInRect(l_sLayout.sCreateButton, pi_nX, pi_nY))
  {
    return CHARACTER_SELECT_BUTTON_CREATE;
  }
  if (IsPointInRect(l_sLayout.sDeleteButton, pi_nX, pi_nY))
  {
    return CHARACTER_SELECT_BUTTON_DELETE;
  }
  if (IsPointInRect(l_sLayout.sConnectButton, pi_nX, pi_nY))
  {
    return CHARACTER_SELECT_BUTTON_CONNECT;
  }
  if (IsPointInRect(l_sLayout.sExitButton, pi_nX, pi_nY))
  {
    return CHARACTER_SELECT_BUTTON_EXIT;
  }

  return kInvalidCharacterButton;
}

void CGP_LogIn::UpdateCharacterSelectionMouseInput(void)
{
  if (IsLoginMessageVisible())
  {
    UpdateLoginMessageInput();
    return;
  }

  CNetworkMgr *l_pNetworkMgr = _GetNetworkMgr();
  if (!l_pNetworkMgr || !l_pNetworkMgr->HasRegedCharResult())
  {
    ClearOpeningMouseTransitions();
    return;
  }

  BYTE l_byHoverButton = GetCharacterSelectionButtonAtPoint(m_nMouseX, m_nMouseY);

  m_byCharacterMenuHover = l_byHoverButton;

  if (m_bLeftButtonPressed)
  {
    m_byCharacterMenuPressed = m_byCharacterMenuHover;
  }

  if (m_bLeftButtonReleased)
  {
    const BYTE l_byPressedButton = m_byCharacterMenuPressed;
    m_byCharacterMenuPressed = kInvalidCharacterButton;
    if (l_byPressedButton != kInvalidCharacterButton && l_byPressedButton == m_byCharacterMenuHover)
    {
      ActivateCharacterSelectionButton(l_byPressedButton);
    }
  }

  if (!m_bLeftButtonDown)
  {
    m_byCharacterMenuPressed = kInvalidCharacterButton;
  }

  ClearOpeningMouseTransitions();
}

void CGP_LogIn::UpdateCharacterSelectionAnimation(void)
{
  UpdateSpriteSheetAnimation(&m_sUpperBoardAnimation, 5);
  UpdateSpriteSheetAnimation(&m_sLowerBoardAnimation, 12);
}

void CGP_LogIn::ResetCharacterSelectionAnimation(void)
{
  m_sUpperBoardAnimation.fCurFrameNo = 0.0f;
  m_sUpperBoardAnimation.bStop = false;
  m_sUpperBoardAnimation.dwStopTime = 0;
  m_sLowerBoardAnimation.fCurFrameNo = 0.0f;
  m_sLowerBoardAnimation.bStop = false;
  m_sLowerBoardAnimation.dwStopTime = 0;
}

void CGP_LogIn::ActivateCharacterSelectionButton(BYTE pi_byButtonIndex)
{
  if (pi_byButtonIndex == CHARACTER_SELECT_BUTTON_PREV)
  {
    StartCharacterSelectionUIClose();
    MoveCharacterSelectionSlot(false);
  }
  else if (pi_byButtonIndex == CHARACTER_SELECT_BUTTON_NEXT)
  {
    StartCharacterSelectionUIClose();
    MoveCharacterSelectionSlot(true);
  }
  else if (pi_byButtonIndex == CHARACTER_SELECT_BUTTON_CREATE)
  {
    CNetworkMgr *l_pNetworkMgr = _GetNetworkMgr();
    if (!l_pNetworkMgr || !l_pNetworkMgr->HasRegedCharResult())
    {
      return;
    }

    if (FindRegedAvatarBySlot(l_pNetworkMgr->GetRegedCharResult(), m_bySelectedCharacterSlot))
    {
      ShowLoginMessage("This character slot is full");
      return;
    }

    EnterCharacterCreateScreen();
  }
  else if (pi_byButtonIndex == CHARACTER_SELECT_BUTTON_DELETE)
  {
    CNetworkMgr *l_pNetworkMgr = _GetNetworkMgr();
    if (!l_pNetworkMgr || !l_pNetworkMgr->HasRegedCharResult())
    {
      return;
    }

    if (!FindRegedAvatarBySlot(l_pNetworkMgr->GetRegedCharResult(), m_bySelectedCharacterSlot))
    {
      ShowLoginMessage("There is no character in this slot.");
      return;
    }

    ShowLoginMessage("Character deletion flow is not implemented yet.");
  }
  else if (pi_byButtonIndex == CHARACTER_SELECT_BUTTON_CONNECT)
  {
    SendSelectedCharacterRequest();
  }
  else if (pi_byButtonIndex == CHARACTER_SELECT_BUTTON_EXIT && _GetMainApp())
  {
    _GetMainApp()->RequestQuitProgram();
  }
}

void CGP_LogIn::MoveCharacterSelectionSlot(bool pi_bNext)
{
  if (_GetMainApp() && _GetMainApp()->IsLoginLobbyCameraAnimating())
  {
    return;
  }

  const BYTE l_byOldSlotIndex = (m_bySelectedCharacterSlot > 2) ? 0 : m_bySelectedCharacterSlot;
  m_bySelectedCharacterSlot = pi_bNext
                                ? static_cast<BYTE>((l_byOldSlotIndex >= 2) ? 0 : l_byOldSlotIndex + 1)
                                : static_cast<BYTE>((l_byOldSlotIndex == 0) ? 2 : l_byOldSlotIndex - 1);
  m_byCharacterMenuHover = kInvalidCharacterButton;
  m_byCharacterMenuPressed = kInvalidCharacterButton;

  if (_GetMainApp())
  {
    if (pi_bNext)
    {
      _GetMainApp()->PlayLoginLobbyCharacterNextCamera(l_byOldSlotIndex);
    }
    else
    {
      _GetMainApp()->PlayLoginLobbyCharacterPrevCamera(l_byOldSlotIndex);
    }
  }
}

void CGP_LogIn::SendSelectedCharacterRequest(void)
{
  CNetworkMgr *l_pNetworkMgr = _GetNetworkMgr();
  if (!l_pNetworkMgr ||
      !l_pNetworkMgr->HasRegedCharResult() ||
      l_pNetworkMgr->GetResultOfUserInfo() != USER_INFO_SUCCESS ||
      IsShortcutModifierDown())
  {
    return;
  }

  const _reged_char_result_zone &l_sRegedCharResult = l_pNetworkMgr->GetRegedCharResult();
  if (!FindRegedAvatarBySlot(l_sRegedCharResult, m_bySelectedCharacterSlot))
  {
    ShowLoginMessage("Please create a character first.");
    return;
  }

  if (!l_pNetworkMgr->HasSentSelCharRequest() || l_pNetworkMgr->HasSelCharResult())
  {
    l_pNetworkMgr->SystemMsg_SelCharRequest_zone(m_bySelectedCharacterSlot);
  }
}

void CGP_LogIn::EnterCharacterCreateScreen(void)
{
  ResetCharacterCreateState();
  if (_GetMainApp())
  {
    _GetMainApp()->ClearLoginLobbyCreatePreview();
  }
  m_byScreenMode = LOGIN_SCREEN_CHAR_CREATE;
  m_bCharacterSelectionUIVisible = false;
  m_bCharacterSelectionUIClosing = false;
  m_dwCharacterSelectionOpenTick = 0;
  m_dwCharacterSelectionCloseTick = 0;
  m_byCharacterMenuHover = kInvalidCharacterButton;
  m_byCharacterMenuPressed = kInvalidCharacterButton;
  ResetCharacterSelectionAnimation();
  if (_GetMainApp())
  {
    _GetMainApp()->PlayLoginLobbyCharacterCreateRaceCamera(m_bySelectedCharacterSlot);
  }
  ClearOpeningMouseTransitions();
}

void CGP_LogIn::LeaveCharacterCreateScreen(bool pi_bCreationCompleted)
{
  if (_GetMainApp())
  {
    _GetMainApp()->ClearLoginLobbyCreatePreview();
  }
  ResetCharacterCreateState();
  m_byScreenMode = LOGIN_SCREEN_CHAR_SELECT;
  m_bCharacterSelectionUIVisible = false;
  m_bCharacterSelectionUIClosing = false;
  m_dwCharacterSelectionOpenTick = 0;
  m_dwCharacterSelectionCloseTick = 0;
  m_byCharacterMenuHover = kInvalidCharacterButton;
  m_byCharacterMenuPressed = kInvalidCharacterButton;
  ResetCharacterSelectionAnimation();
  if (_GetMainApp())
  {
    if (pi_bCreationCompleted)
    {
      _GetMainApp()->PlayLoginLobbyCharacterCreateCompleteCamera(m_bySelectedCharacterSlot);
    }
    else
    {
      _GetMainApp()->PlayLoginLobbyCharacterCreateRaceCancelCamera(m_bySelectedCharacterSlot);
    }
  }
  ClearOpeningMouseTransitions();
}

void CGP_LogIn::ResetCharacterCreateState(void)
{
  m_byCreateStep = CHARACTER_CREATE_STEP_RACE;
  m_byCreateRaceSelection = CHARACTER_CREATE_BUTTON_RACE_BELLATO;
  m_byCreateAttributeSelection = CHARACTER_CREATE_ATTRIBUTE_MELEE;
  m_byCreateSexSelection = 0;
  m_byCreateMenuHover = kInvalidCharacterCreateButton;
  m_byCreateMenuPressed = kInvalidCharacterCreateButton;
  ZeroMemory(m_abyCreatePartVariant, sizeof(m_abyCreatePartVariant));
  ZeroMemory(m_szCreateCharacterName, sizeof(m_szCreateCharacterName));
}

void CGP_LogIn::UpdateCharacterCreatePreview(void)
{
  if (!_GetMainApp())
  {
    return;
  }

  _GetMainApp()->BuildLoginLobbyCreatePreview(GetCreateRaceSexCode(),
                                              GetCreateClassCode(),
                                              GetCreateBaseShape());
}

bool CGP_LogIn::IsCharacterCreateScreen(void) const
{
  return m_byScreenMode == LOGIN_SCREEN_CHAR_CREATE;
}

bool CGP_LogIn::GetCharacterCreateRaceLayout(CHARACTER_CREATE_RACE_LAYOUT *po_pLayout) const
{
  if (!po_pLayout)
  {
    return false;
  }

  int l_nScreenX = 0;
  int l_nScreenY = 0;
  GetScreenSize(&l_nScreenX, &l_nScreenY);
  if (l_nScreenX <= 0 || l_nScreenY <= 0)
  {
    return false;
  }

  const int l_nUpperBoardWidth = 464;
  const int l_nUpperBoardHeight = 135;
  const int l_nUpperBaseWidth = 456;
  const int l_nUpperBaseHeight = 109;
  const int l_nLowerBoardWidth = 520;
  const int l_nLowerBoardHeight = 73;
  const int l_nLowerBaseWidth = 467;
  const int l_nLowerBaseHeight = 72;

  SetRect(&po_pLayout->sUpperBoard,
          (l_nScreenX - l_nUpperBoardWidth) / 2,
          10,
          (l_nScreenX - l_nUpperBoardWidth) / 2 + l_nUpperBoardWidth,
          10 + l_nUpperBoardHeight);
  SetRect(&po_pLayout->sUpperBase,
          po_pLayout->sUpperBoard.left + ((l_nUpperBoardWidth - l_nUpperBaseWidth) / 2),
          po_pLayout->sUpperBoard.top,
          po_pLayout->sUpperBoard.left + ((l_nUpperBoardWidth - l_nUpperBaseWidth) / 2) + l_nUpperBaseWidth,
          po_pLayout->sUpperBoard.top + l_nUpperBaseHeight);
  SetRect(&po_pLayout->sUpperBoardAni,
          po_pLayout->sUpperBase.left + 203,
          po_pLayout->sUpperBase.top + 20,
          po_pLayout->sUpperBase.left + 253,
          po_pLayout->sUpperBase.top + 70);
  for (int i = 0; i < 3; ++i)
  {
    SetRect(&po_pLayout->sRaceButton[i],
            po_pLayout->sUpperBase.left + 2 + (155 * i),
            po_pLayout->sUpperBase.top + 96,
            po_pLayout->sUpperBase.left + 2 + (155 * i) + 143,
            po_pLayout->sUpperBase.top + 134);
  }

  SetRect(&po_pLayout->sRaceProfile,
          (l_nScreenX - 486) / 2,
          (l_nScreenY - 309) / 2,
          (l_nScreenX - 486) / 2 + 486,
          (l_nScreenY - 309) / 2 + 309);
  SetRect(&po_pLayout->sLowerBoard,
          (l_nScreenX - l_nLowerBoardWidth) / 2,
          l_nScreenY - l_nLowerBoardHeight - 10,
          (l_nScreenX - l_nLowerBoardWidth) / 2 + l_nLowerBoardWidth,
          l_nScreenY - 10);
  SetRect(&po_pLayout->sLowerBase,
          po_pLayout->sLowerBoard.left + ((l_nLowerBoardWidth - l_nLowerBaseWidth) / 2),
          po_pLayout->sLowerBoard.top,
          po_pLayout->sLowerBoard.left + ((l_nLowerBoardWidth - l_nLowerBaseWidth) / 2) + l_nLowerBaseWidth,
          po_pLayout->sLowerBoard.top + l_nLowerBaseHeight);
  SetRect(&po_pLayout->sOkButton,
          po_pLayout->sLowerBoard.left,
          po_pLayout->sLowerBoard.top + 18,
          po_pLayout->sLowerBoard.left + 220,
          po_pLayout->sLowerBoard.top + 54);
  SetRect(&po_pLayout->sCancelButton,
          po_pLayout->sLowerBoard.left + 305,
          po_pLayout->sLowerBoard.top + 18,
          po_pLayout->sLowerBoard.left + 525,
          po_pLayout->sLowerBoard.top + 54);
  return true;
}

bool CGP_LogIn::GetCharacterCreateAttributeLayout(CHARACTER_CREATE_ATTRIBUTE_LAYOUT *po_pLayout) const
{
  if (!po_pLayout)
  {
    return false;
  }

  int l_nScreenX = 0;
  int l_nScreenY = 0;
  GetScreenSize(&l_nScreenX, &l_nScreenY);
  if (l_nScreenX <= 0 || l_nScreenY <= 0)
  {
    return false;
  }

  const int l_nPanelWidth = 211;
  const int l_nPanelHeight = 466;
  const int l_nPanelLeft = l_nScreenX - l_nPanelWidth - 30;
  const int l_nPanelTop = static_cast<int>(static_cast<float>(l_nScreenY - l_nPanelHeight) * 0.2f);
  SetRect(&po_pLayout->sPanel,
          l_nPanelLeft,
          l_nPanelTop,
          l_nPanelLeft + l_nPanelWidth,
          l_nPanelTop + l_nPanelHeight);
  SetRect(&po_pLayout->sTitleBase, l_nPanelLeft + 32, l_nPanelTop + 4, l_nPanelLeft + 178, l_nPanelTop + 30);
  SetRect(&po_pLayout->sLowerBase, l_nPanelLeft + 81, l_nPanelTop + 428, l_nPanelLeft + 131, l_nPanelTop + 465);
  SetRect(&po_pLayout->sOkButton, l_nPanelLeft + 16, l_nPanelTop + 432, l_nPanelLeft + 105, l_nPanelTop + 463);
  SetRect(&po_pLayout->sCancelButton, l_nPanelLeft + 106, l_nPanelTop + 432, l_nPanelLeft + 195, l_nPanelTop + 463);

  for (int i = 0; i < 4; ++i)
  {
    const int l_nButtonLeft = IsAccretiaCreateRace(m_byCreateRaceSelection)
                                ? l_nPanelLeft + 51 + (38 * ((i > CHARACTER_CREATE_ATTRIBUTE_FORCE) ? i - 1 : i))
                                : l_nPanelLeft + 33 + (38 * i);
    SetRect(&po_pLayout->sAttributeButton[i],
            l_nButtonLeft,
            l_nPanelTop + 52,
            l_nButtonLeft + 32,
            l_nPanelTop + 84);
  }

  SetRect(&po_pLayout->sDescText[0], l_nPanelLeft + 15, l_nPanelTop + 98, l_nPanelLeft + 196, l_nPanelTop + 112);
  SetRect(&po_pLayout->sDescText[1], l_nPanelLeft + 15, l_nPanelTop + 117, l_nPanelLeft + 196, l_nPanelTop + 131);

  for (int i = 0; i < 3; ++i)
  {
    SetRect(&po_pLayout->sBasicPointText[i], l_nPanelLeft + 16, l_nPanelTop + 171 + (i * 15), l_nPanelLeft + 88, l_nPanelTop + 185 + (i * 15));
    SetRect(&po_pLayout->sBasicPointBase[i], l_nPanelLeft + 90, l_nPanelTop + 174 + (i * 14), l_nPanelLeft + 196, l_nPanelTop + 182 + (i * 14));
    SetRect(&po_pLayout->sMakeSkillText[i], l_nPanelLeft + 16, l_nPanelTop + 370 + (i * 15), l_nPanelLeft + 88, l_nPanelTop + 384 + (i * 15));
    SetRect(&po_pLayout->sMakeSkillBase[i], l_nPanelLeft + 90, l_nPanelTop + 372 + (i * 14), l_nPanelLeft + 196, l_nPanelTop + 380 + (i * 14));
  }

  for (int i = 0; i < 6; ++i)
  {
    SetRect(&po_pLayout->sBattleSkillText[i], l_nPanelLeft + 16, l_nPanelTop + 250 + (i * 14), l_nPanelLeft + 88, l_nPanelTop + 264 + (i * 14));
    SetRect(&po_pLayout->sBattleSkillBase[i], l_nPanelLeft + 90, l_nPanelTop + 252 + (i * 14), l_nPanelLeft + 196, l_nPanelTop + 260 + (i * 14));
  }

  return true;
}

bool CGP_LogIn::GetCharacterCreateDetailLayout(CHARACTER_CREATE_DETAIL_LAYOUT *po_pLayout) const
{
  if (!po_pLayout)
  {
    return false;
  }

  int l_nScreenX = 0;
  int l_nScreenY = 0;
  GetScreenSize(&l_nScreenX, &l_nScreenY);
  if (l_nScreenX <= 0 || l_nScreenY <= 0)
  {
    return false;
  }

  const int l_nPanelWidth = 178;
  const int l_nPanelHeight = 326;
  const int l_nPanelLeft = l_nScreenX - l_nPanelWidth - 30;
  const int l_nPanelTop = static_cast<int>(static_cast<float>(l_nScreenY - l_nPanelHeight) * 0.38f);
  SetRect(&po_pLayout->sPanel,
          l_nPanelLeft,
          l_nPanelTop,
          l_nPanelLeft + l_nPanelWidth,
          l_nPanelTop + l_nPanelHeight);
  SetRect(&po_pLayout->sTitleBase, l_nPanelLeft + 16, l_nPanelTop + 4, l_nPanelLeft + 162, l_nPanelTop + 30);
  SetRect(&po_pLayout->sLowerBase, l_nPanelLeft + 65, l_nPanelTop + 289, l_nPanelLeft + 115, l_nPanelTop + 326);
  SetRect(&po_pLayout->sOkButton, l_nPanelLeft, l_nPanelTop + 295, l_nPanelLeft + 89, l_nPanelTop + 326);
  SetRect(&po_pLayout->sCancelButton, l_nPanelLeft + 89, l_nPanelTop + 295, l_nPanelLeft + 178, l_nPanelTop + 326);
  SetRect(&po_pLayout->sSelectItem[CHARACTER_CREATE_SELECT_SEX],
          l_nPanelLeft + 53,
          l_nPanelTop + 52,
          l_nPanelLeft + 124,
          l_nPanelTop + 69);
  for (int i = 1; i < CHARACTER_CREATE_SELECT_COUNT; ++i)
  {
    SetRect(&po_pLayout->sSelectItem[i],
            l_nPanelLeft + 31,
            l_nPanelTop + 94 + ((i - 1) * 22),
            l_nPanelLeft + 102,
            l_nPanelTop + 111 + ((i - 1) * 22));
  }

  for (int i = 0; i < CHARACTER_CREATE_SELECT_COUNT; ++i)
  {
    const int l_nMidY = (po_pLayout->sSelectItem[i].top + po_pLayout->sSelectItem[i].bottom) / 2;
    SetRect(&po_pLayout->sSelectLeftButton[i],
            po_pLayout->sSelectItem[i].left - 22,
            l_nMidY - 8,
            po_pLayout->sSelectItem[i].left - 1,
            l_nMidY + 9);
    SetRect(&po_pLayout->sSelectRightButton[i],
            po_pLayout->sSelectItem[i].right + 1,
            l_nMidY - 8,
            po_pLayout->sSelectItem[i].right + 22,
            l_nMidY + 9);
  }

  SetRect(&po_pLayout->sNameInput, l_nPanelLeft + 38, l_nPanelTop + 257, l_nPanelLeft + 140, l_nPanelTop + 269);
  return true;
}

bool CGP_LogIn::GetCharacterCreateRotateLayout(CHARACTER_CREATE_ROTATE_LAYOUT *po_pLayout) const
{
  if (!po_pLayout)
  {
    return false;
  }

  int l_nScreenX = 0;
  int l_nScreenY = 0;
  GetScreenSize(&l_nScreenX, &l_nScreenY);
  if (l_nScreenX <= 0 || l_nScreenY <= 0)
  {
    return false;
  }

  const int l_nLowerBoardWidth = 520;
  const int l_nLowerBoardHeight = 73;
  const int l_nLowerBaseWidth = 467;
  const int l_nLowerBaseHeight = 72;
  const int l_nLowerBoardLeft = (l_nScreenX - l_nLowerBoardWidth) / 2;
  const int l_nLowerBoardTop = l_nScreenY - l_nLowerBoardHeight - 10;

  SetRect(&po_pLayout->sLowerBoard,
          l_nLowerBoardLeft,
          l_nLowerBoardTop,
          l_nLowerBoardLeft + l_nLowerBoardWidth,
          l_nLowerBoardTop + l_nLowerBoardHeight);
  SetRect(&po_pLayout->sLowerBase,
          l_nLowerBoardLeft + ((l_nLowerBoardWidth - l_nLowerBaseWidth) / 2),
          l_nLowerBoardTop,
          l_nLowerBoardLeft + ((l_nLowerBoardWidth - l_nLowerBaseWidth) / 2) + l_nLowerBaseWidth,
          l_nLowerBoardTop + l_nLowerBaseHeight);
  SetRect(&po_pLayout->sRotateLeftButton,
          l_nLowerBoardLeft,
          l_nLowerBoardTop + 18,
          l_nLowerBoardLeft + 220,
          l_nLowerBoardTop + 54);
  SetRect(&po_pLayout->sRotateRightButton,
          l_nLowerBoardLeft + 305,
          l_nLowerBoardTop + 18,
          l_nLowerBoardLeft + 525,
          l_nLowerBoardTop + 54);
  return true;
}

BYTE CGP_LogIn::GetCharacterCreateButtonAtPoint(int pi_nX, int pi_nY) const
{
  if (m_byCreateStep == CHARACTER_CREATE_STEP_RACE)
  {
    CHARACTER_CREATE_RACE_LAYOUT l_sLayout;
    if (!GetCharacterCreateRaceLayout(&l_sLayout))
    {
      return kInvalidCharacterCreateButton;
    }

    for (BYTE i = 0; i < 3; ++i)
    {
      if (IsPointInRect(l_sLayout.sRaceButton[i], pi_nX, pi_nY))
      {
        return i;
      }
    }
    if (IsPointInRect(l_sLayout.sOkButton, pi_nX, pi_nY))
    {
      return CHARACTER_CREATE_BUTTON_OK;
    }
    if (IsPointInRect(l_sLayout.sCancelButton, pi_nX, pi_nY))
    {
      return CHARACTER_CREATE_BUTTON_CANCEL;
    }
  }
  else if (m_byCreateStep == CHARACTER_CREATE_STEP_ATTRIBUTE)
  {
    CHARACTER_CREATE_ATTRIBUTE_LAYOUT l_sLayout;
    if (!GetCharacterCreateAttributeLayout(&l_sLayout))
    {
      return kInvalidCharacterCreateButton;
    }

    for (BYTE i = 0; i < 4; ++i)
    {
      if (IsAccretiaCreateRace(m_byCreateRaceSelection) && i == CHARACTER_CREATE_ATTRIBUTE_FORCE)
      {
        continue;
      }
      if (IsPointInRect(l_sLayout.sAttributeButton[i], pi_nX, pi_nY))
      {
        return static_cast<BYTE>(CHARACTER_CREATE_BUTTON_ATTRIBUTE_BASE + i);
      }
    }
    if (IsPointInRect(l_sLayout.sOkButton, pi_nX, pi_nY))
    {
      return CHARACTER_CREATE_BUTTON_OK;
    }
    if (IsPointInRect(l_sLayout.sCancelButton, pi_nX, pi_nY))
    {
      return CHARACTER_CREATE_BUTTON_CANCEL;
    }
  }
  else if (m_byCreateStep == CHARACTER_CREATE_STEP_DETAIL)
  {
    CHARACTER_CREATE_DETAIL_LAYOUT l_sLayout;
    if (!GetCharacterCreateDetailLayout(&l_sLayout))
    {
      return kInvalidCharacterCreateButton;
    }

    CHARACTER_CREATE_ROTATE_LAYOUT l_sRotateLayout;
    if (GetCharacterCreateRotateLayout(&l_sRotateLayout))
    {
      if (IsPointInRect(l_sRotateLayout.sRotateLeftButton, pi_nX, pi_nY))
      {
        return CHARACTER_CREATE_BUTTON_ROTATE_LEFT;
      }
      if (IsPointInRect(l_sRotateLayout.sRotateRightButton, pi_nX, pi_nY))
      {
        return CHARACTER_CREATE_BUTTON_ROTATE_RIGHT;
      }
    }

    for (BYTE i = 0; i < CHARACTER_CREATE_SELECT_COUNT; ++i)
    {
      if (IsAccretiaCreateRace(m_byCreateRaceSelection) && i == CHARACTER_CREATE_SELECT_SEX)
      {
        continue;
      }
      if (IsPointInRect(l_sLayout.sSelectLeftButton[i], pi_nX, pi_nY))
      {
        return static_cast<BYTE>(CHARACTER_CREATE_BUTTON_DETAIL_LEFT_BASE + i);
      }
      if (IsPointInRect(l_sLayout.sSelectRightButton[i], pi_nX, pi_nY))
      {
        return static_cast<BYTE>(CHARACTER_CREATE_BUTTON_DETAIL_RIGHT_BASE + i);
      }
    }
    if (IsPointInRect(l_sLayout.sOkButton, pi_nX, pi_nY))
    {
      return CHARACTER_CREATE_BUTTON_OK;
    }
    if (IsPointInRect(l_sLayout.sCancelButton, pi_nX, pi_nY))
    {
      return CHARACTER_CREATE_BUTTON_CANCEL;
    }
  }

  return kInvalidCharacterCreateButton;
}

void CGP_LogIn::UpdateCharacterCreateInput(void)
{
  const BOOL l_bLeftPressed = ConsumeKeyPress(VK_LEFT);
  const BOOL l_bRightPressed = ConsumeKeyPress(VK_RIGHT);
  const BOOL l_bUpPressed = ConsumeKeyPress(VK_UP);
  const BOOL l_bDownPressed = ConsumeKeyPress(VK_DOWN);
  const BOOL l_bConfirmPressed = ConsumeKeyPress(VK_RETURN) || ConsumeKeyPress(VK_SPACE);
  const BOOL l_bEscapePressed = ConsumeKeyPress(VK_ESCAPE);

  if (!IsCharacterCreateScreen())
  {
    return;
  }

  if (IsLoginMessageVisible())
  {
    UpdateLoginMessageInput();
    return;
  }

  if (_GetMainApp() && _GetMainApp()->IsLoginLobbyCameraAnimating())
  {
    m_byCreateMenuHover = kInvalidCharacterCreateButton;
    m_byCreateMenuPressed = kInvalidCharacterCreateButton;
    ClearOpeningMouseTransitions();
    return;
  }

  UpdateCharacterCreateMouseInput();

  if (l_bEscapePressed)
  {
    ActivateCharacterCreateButton(CHARACTER_CREATE_BUTTON_CANCEL);
    return;
  }

  if (m_byCreateStep == CHARACTER_CREATE_STEP_RACE)
  {
    if (l_bLeftPressed)
    {
      m_byCreateRaceSelection = static_cast<BYTE>((m_byCreateRaceSelection == 0) ? 2 : m_byCreateRaceSelection - 1);
    }
    else if (l_bRightPressed)
    {
      m_byCreateRaceSelection = static_cast<BYTE>((m_byCreateRaceSelection + 1) % 3);
    }
  }
  else if (m_byCreateStep == CHARACTER_CREATE_STEP_ATTRIBUTE)
  {
    if (l_bLeftPressed || l_bUpPressed)
    {
      m_byCreateAttributeSelection = GetNextCreateAttribute(m_byCreateAttributeSelection, m_byCreateRaceSelection, false);
    }
    else if (l_bRightPressed || l_bDownPressed)
    {
      m_byCreateAttributeSelection = GetNextCreateAttribute(m_byCreateAttributeSelection, m_byCreateRaceSelection, true);
    }
  }
  else if (m_byCreateStep == CHARACTER_CREATE_STEP_DETAIL)
  {
    if (_GetMainApp() && m_bLeftButtonDown)
    {
      if (m_byCreateMenuPressed == CHARACTER_CREATE_BUTTON_ROTATE_LEFT)
      {
        _GetMainApp()->RotateLoginLobbyCreatePreview(1.0f);
      }
      else if (m_byCreateMenuPressed == CHARACTER_CREATE_BUTTON_ROTATE_RIGHT)
      {
        _GetMainApp()->RotateLoginLobbyCreatePreview(-1.0f);
      }
    }

    if (l_bUpPressed || l_bDownPressed)
    {
      m_byCreateMenuHover = kInvalidCharacterCreateButton;
    }
    if (_GetMainApp() && l_bLeftPressed)
    {
      _GetMainApp()->RotateLoginLobbyCreatePreview(1.0f);
    }
    else if (_GetMainApp() && l_bRightPressed)
    {
      _GetMainApp()->RotateLoginLobbyCreatePreview(-1.0f);
    }
  }

  if (l_bConfirmPressed)
  {
    ActivateCharacterCreateOk();
  }
}

void CGP_LogIn::UpdateCharacterCreateMouseInput(void)
{
  BYTE l_byHoverButton = GetCharacterCreateButtonAtPoint(m_nMouseX, m_nMouseY);
  m_byCreateMenuHover = l_byHoverButton;

  if (m_bLeftButtonPressed)
  {
    m_byCreateMenuPressed = m_byCreateMenuHover;
  }

  if (m_bLeftButtonReleased)
  {
    const BYTE l_byPressedButton = m_byCreateMenuPressed;
    m_byCreateMenuPressed = kInvalidCharacterCreateButton;
    if (l_byPressedButton != kInvalidCharacterCreateButton && l_byPressedButton == m_byCreateMenuHover)
    {
      ActivateCharacterCreateButton(l_byPressedButton);
    }
  }

  if (!m_bLeftButtonDown)
  {
    m_byCreateMenuPressed = kInvalidCharacterCreateButton;
  }

  ClearOpeningMouseTransitions();
}

void CGP_LogIn::ActivateCharacterCreateButton(BYTE pi_byButtonIndex)
{
  if (pi_byButtonIndex <= CHARACTER_CREATE_BUTTON_RACE_ACCRETIA &&
      m_byCreateStep == CHARACTER_CREATE_STEP_RACE)
  {
    m_byCreateRaceSelection = pi_byButtonIndex;
    if (IsAccretiaCreateRace(m_byCreateRaceSelection))
    {
      m_byCreateSexSelection = 0;
      if (m_byCreateAttributeSelection == CHARACTER_CREATE_ATTRIBUTE_FORCE)
      {
        m_byCreateAttributeSelection = CHARACTER_CREATE_ATTRIBUTE_MELEE;
      }
    }
    return;
  }

  if (pi_byButtonIndex >= CHARACTER_CREATE_BUTTON_ATTRIBUTE_BASE &&
      pi_byButtonIndex < CHARACTER_CREATE_BUTTON_ATTRIBUTE_BASE + 4 &&
      m_byCreateStep == CHARACTER_CREATE_STEP_ATTRIBUTE)
  {
    const BYTE l_byAttribute = static_cast<BYTE>(pi_byButtonIndex - CHARACTER_CREATE_BUTTON_ATTRIBUTE_BASE);
    if (!IsAccretiaCreateRace(m_byCreateRaceSelection) ||
        l_byAttribute != CHARACTER_CREATE_ATTRIBUTE_FORCE)
    {
      m_byCreateAttributeSelection = l_byAttribute;
    }
    return;
  }

  if (pi_byButtonIndex >= CHARACTER_CREATE_BUTTON_DETAIL_LEFT_BASE &&
      pi_byButtonIndex < CHARACTER_CREATE_BUTTON_DETAIL_LEFT_BASE + CHARACTER_CREATE_SELECT_COUNT &&
      m_byCreateStep == CHARACTER_CREATE_STEP_DETAIL)
  {
    const BYTE l_byIndex = static_cast<BYTE>(pi_byButtonIndex - CHARACTER_CREATE_BUTTON_DETAIL_LEFT_BASE);
    if (l_byIndex == CHARACTER_CREATE_SELECT_SEX)
    {
      if (!IsAccretiaCreateRace(m_byCreateRaceSelection))
      {
        m_byCreateSexSelection = static_cast<BYTE>(m_byCreateSexSelection ? 0 : 1);
      }
    }
    else
    {
      m_abyCreatePartVariant[l_byIndex] =
        static_cast<BYTE>((m_abyCreatePartVariant[l_byIndex] == 0)
                            ? kCharacterCreateDefaultItemCount - 1
                            : m_abyCreatePartVariant[l_byIndex] - 1);
    }
    UpdateCharacterCreatePreview();
    return;
  }

  if (pi_byButtonIndex >= CHARACTER_CREATE_BUTTON_DETAIL_RIGHT_BASE &&
      pi_byButtonIndex < CHARACTER_CREATE_BUTTON_DETAIL_RIGHT_BASE + CHARACTER_CREATE_SELECT_COUNT &&
      m_byCreateStep == CHARACTER_CREATE_STEP_DETAIL)
  {
    const BYTE l_byIndex = static_cast<BYTE>(pi_byButtonIndex - CHARACTER_CREATE_BUTTON_DETAIL_RIGHT_BASE);
    if (l_byIndex == CHARACTER_CREATE_SELECT_SEX)
    {
      if (!IsAccretiaCreateRace(m_byCreateRaceSelection))
      {
        m_byCreateSexSelection = static_cast<BYTE>(m_byCreateSexSelection ? 0 : 1);
      }
    }
    else
    {
      m_abyCreatePartVariant[l_byIndex] =
        static_cast<BYTE>((m_abyCreatePartVariant[l_byIndex] + 1) % kCharacterCreateDefaultItemCount);
    }
    UpdateCharacterCreatePreview();
    return;
  }

  if (m_byCreateStep == CHARACTER_CREATE_STEP_DETAIL &&
      (pi_byButtonIndex == CHARACTER_CREATE_BUTTON_ROTATE_LEFT ||
       pi_byButtonIndex == CHARACTER_CREATE_BUTTON_ROTATE_RIGHT))
  {
    if (_GetMainApp())
    {
      _GetMainApp()->RotateLoginLobbyCreatePreview(pi_byButtonIndex == CHARACTER_CREATE_BUTTON_ROTATE_LEFT ? 1.0f : -1.0f);
    }
    return;
  }

  if (pi_byButtonIndex == CHARACTER_CREATE_BUTTON_CANCEL)
  {
    if (m_byCreateStep == CHARACTER_CREATE_STEP_RACE)
    {
      LeaveCharacterCreateScreen();
    }
    else if (m_byCreateStep == CHARACTER_CREATE_STEP_ATTRIBUTE)
    {
      m_byCreateStep = CHARACTER_CREATE_STEP_RACE;
      m_byCreateMenuHover = kInvalidCharacterCreateButton;
      m_byCreateMenuPressed = kInvalidCharacterCreateButton;
      if (_GetMainApp())
      {
        _GetMainApp()->ClearLoginLobbyCreatePreview();
        _GetMainApp()->PlayLoginLobbyCharacterCreateAttributeCancelCamera();
      }
    }
    else
    {
      m_byCreateStep = CHARACTER_CREATE_STEP_ATTRIBUTE;
      m_byCreateMenuHover = kInvalidCharacterCreateButton;
      m_byCreateMenuPressed = kInvalidCharacterCreateButton;
      if (_GetMainApp())
      {
        _GetMainApp()->ClearLoginLobbyCreatePreview();
        _GetMainApp()->PlayLoginLobbyCharacterCreateDetailCancelCamera();
      }
    }
    return;
  }

  if (pi_byButtonIndex == CHARACTER_CREATE_BUTTON_OK)
  {
    ActivateCharacterCreateOk();
  }
}

void CGP_LogIn::ActivateCharacterCreateOk(void)
{
  if (m_byCreateStep == CHARACTER_CREATE_STEP_RACE)
  {
    m_byCreateStep = CHARACTER_CREATE_STEP_ATTRIBUTE;
    m_byCreateAttributeSelection = CHARACTER_CREATE_ATTRIBUTE_MELEE;
    m_byCreateMenuHover = kInvalidCharacterCreateButton;
    m_byCreateMenuPressed = kInvalidCharacterCreateButton;
    if (_GetMainApp())
    {
      _GetMainApp()->ClearLoginLobbyCreatePreview();
      _GetMainApp()->PlayLoginLobbyCharacterCreateAttributeCamera();
    }
    return;
  }

  if (m_byCreateStep == CHARACTER_CREATE_STEP_ATTRIBUTE)
  {
    m_byCreateStep = CHARACTER_CREATE_STEP_DETAIL;
    m_byCreateMenuHover = kInvalidCharacterCreateButton;
    m_byCreateMenuPressed = kInvalidCharacterCreateButton;
    if (_GetMainApp())
    {
      _GetMainApp()->PlayLoginLobbyCharacterCreateDetailCamera();
    }
    UpdateCharacterCreatePreview();
    return;
  }

  if (m_szCreateCharacterName[0] == '\0' ||
      strlen(m_szCreateCharacterName) > 16 ||
      !_IsAvailableCharName(m_szCreateCharacterName, _GetNetworkMgr() ? _GetNetworkMgr()->GetAvatarGrade() : 0))
  {
    ShowLoginMessage("Please enter a valid character name.");
    return;
  }

  CNetworkMgr *l_pNetworkMgr = _GetNetworkMgr();
  if (l_pNetworkMgr && l_pNetworkMgr->HasSentAddCharRequest() && !l_pNetworkMgr->HasAddCharResult())
  {
    return;
  }

  if (!l_pNetworkMgr ||
      !l_pNetworkMgr->SystemMsg_AddCharRequest_zone(m_bySelectedCharacterSlot,
                                                    m_szCreateCharacterName,
                                                    GetCreateRaceSexCode(),
                                                    GetCreateClassCode(),
                                                    GetCreateBaseShape()))
  {
    ShowLoginMessage("Character creation request failed.");
  }
}

BYTE CGP_LogIn::GetCreateRaceSexCode(void) const
{
  switch (ClampCreateRaceSelection(m_byCreateRaceSelection))
  {
    case CHARACTER_CREATE_BUTTON_RACE_CORA:
      return static_cast<BYTE>(m_byCreateSexSelection ? 3 : 2);

    case CHARACTER_CREATE_BUTTON_RACE_ACCRETIA:
      return 4;

    case CHARACTER_CREATE_BUTTON_RACE_BELLATO:
    default:
      return static_cast<BYTE>(m_byCreateSexSelection ? 1 : 0);
  }
}

const char *CGP_LogIn::GetCreateClassCode(void) const
{
  static const char *kClassCode[3][4] =
  {
    { "BWB0", "BRB0", "BFB0", "BSB0" },
    { "CWB0", "CRB0", "CFB0", "CSB0" },
    { "AWB0", "ARB0", "ASB0", "ASB0" }
  };

  const BYTE l_byRace = ClampCreateRaceSelection(m_byCreateRaceSelection);
  BYTE l_byAttribute = m_byCreateAttributeSelection < 4 ? m_byCreateAttributeSelection : 0;
  if (l_byRace == CHARACTER_CREATE_BUTTON_RACE_ACCRETIA &&
      l_byAttribute == CHARACTER_CREATE_ATTRIBUTE_FORCE)
  {
    l_byAttribute = CHARACTER_CREATE_ATTRIBUTE_MELEE;
  }

  return kClassCode[l_byRace][l_byAttribute];
}

DWORD CGP_LogIn::GetCreateBaseShape(void) const
{
  DWORD l_dwBaseShape = 0;
  l_dwBaseShape |= static_cast<DWORD>(m_abyCreatePartVariant[CHARACTER_CREATE_SELECT_COAT] & 0x0F) << 0;
  l_dwBaseShape |= static_cast<DWORD>(m_abyCreatePartVariant[CHARACTER_CREATE_SELECT_PANTS] & 0x0F) << 4;
  l_dwBaseShape |= static_cast<DWORD>(m_abyCreatePartVariant[CHARACTER_CREATE_SELECT_GLOVE] & 0x0F) << 8;
  l_dwBaseShape |= static_cast<DWORD>(m_abyCreatePartVariant[CHARACTER_CREATE_SELECT_SHOES] & 0x0F) << 12;
  l_dwBaseShape |= static_cast<DWORD>(m_abyCreatePartVariant[CHARACTER_CREATE_SELECT_HAIR] & 0x0F) << 16;
  l_dwBaseShape |= static_cast<DWORD>(m_abyCreatePartVariant[CHARACTER_CREATE_SELECT_FACE] & 0x0F) << 20;
  return l_dwBaseShape;
}

void CGP_LogIn::AppendCreateCharacterInputChar(WPARAM pi_wParam)
{
  const char l_chInput = static_cast<char>(pi_wParam & 0xFF);
  const size_t l_stNameLen = strlen(m_szCreateCharacterName);
  if (l_chInput == '\b')
  {
    if (l_stNameLen > 0)
    {
      m_szCreateCharacterName[l_stNameLen - 1] = '\0';
    }
    return;
  }

  if (l_chInput <= ' ' || l_chInput > '~' || l_stNameLen >= 16)
  {
    return;
  }

  m_szCreateCharacterName[l_stNameLen] = l_chInput;
  m_szCreateCharacterName[l_stNameLen + 1] = '\0';
}

void CGP_LogIn::ShowLoginMessage(const char *pi_pMessage)
{
  strncpy_s(m_szLoginMessage,
            sizeof(m_szLoginMessage),
            pi_pMessage ? pi_pMessage : "",
            _TRUNCATE);
  m_bLoginMessageVisible = true;
  m_bLoginMessageOkHover = false;
  m_bLoginMessageOkPressed = false;
  m_byMenuHover = kInvalidMenuIndex;
  m_byMenuPressed = kInvalidMenuIndex;
  m_byCharacterMenuHover = kInvalidCharacterButton;
  m_byCharacterMenuPressed = kInvalidCharacterButton;
  ClearOpeningMouseTransitions();
}

void CGP_LogIn::HideLoginMessage(void)
{
  m_bLoginMessageVisible = false;
  m_bLoginMessageOkHover = false;
  m_bLoginMessageOkPressed = false;
  m_szLoginMessage[0] = '\0';
  ClearOpeningMouseTransitions();
}

bool CGP_LogIn::IsLoginMessageVisible(void) const
{
  return m_bLoginMessageVisible;
}

bool CGP_LogIn::GetLoginMessageLayout(RECT *po_pBoxRect, RECT *po_pOkButtonRect) const
{
  int l_nScreenX = 0;
  int l_nScreenY = 0;
  GetScreenSize(&l_nScreenX, &l_nScreenY);
  if (l_nScreenX <= 0 || l_nScreenY <= 0)
  {
    return false;
  }

  int l_nMsgBoardWidth = 0;
  int l_nMsgBoardHeight = 0;
  GetLoginMessageBoardSize(m_szLoginMessage, &l_nMsgBoardWidth, &l_nMsgBoardHeight);

  int l_nBoxWidth = l_nMsgBoardWidth + kMessageBoxMargin * 2;
  int l_nBoxHeight = l_nMsgBoardHeight + kMessageBoxMargin * 2 + kMessageBoxOkButtonHeight;
  if (l_nBoxWidth < kMessageBoxMinWidth)
  {
    l_nBoxWidth = kMessageBoxMinWidth;
  }
  if (l_nBoxHeight < kMessageBoxMinHeight)
  {
    l_nBoxHeight = kMessageBoxMinHeight;
  }

  const int l_nBoxLeft = (l_nScreenX - l_nBoxWidth) / 2;
  const int l_nBoxTop = (l_nScreenY - l_nBoxHeight) / 2;

  if (po_pBoxRect)
  {
    SetRect(po_pBoxRect,
            l_nBoxLeft,
            l_nBoxTop,
            l_nBoxLeft + l_nBoxWidth,
            l_nBoxTop + l_nBoxHeight);
  }

  if (po_pOkButtonRect)
  {
    const int l_nOkButtonRight = l_nBoxLeft + l_nBoxWidth - kMessageBoxOkButtonRightMargin;
    const int l_nOkButtonBottom = l_nBoxTop + l_nBoxHeight - kMessageBoxOkButtonBottomMargin;
    SetRect(po_pOkButtonRect,
            l_nOkButtonRight - kMessageBoxOkButtonWidth,
            l_nOkButtonBottom - kMessageBoxOkButtonHeight,
            l_nOkButtonRight,
            l_nOkButtonBottom);
  }

  return true;
}

void CGP_LogIn::UpdateLoginMessageInput(void)
{
  RECT l_sBoxRect;
  RECT l_sOkButtonRect;
  if (!GetLoginMessageLayout(&l_sBoxRect, &l_sOkButtonRect))
  {
    HideLoginMessage();
    return;
  }

  m_bLoginMessageOkHover = IsPointInRect(l_sOkButtonRect, m_nMouseX, m_nMouseY);

  if (m_bLeftButtonPressed)
  {
    m_bLoginMessageOkPressed = m_bLoginMessageOkHover;
  }

  if (m_bLeftButtonReleased)
  {
    const bool l_bClose = m_bLoginMessageOkPressed && m_bLoginMessageOkHover;
    m_bLoginMessageOkPressed = false;
    if (l_bClose)
    {
      HideLoginMessage();
      return;
    }
  }

  if (ConsumeKeyPress(VK_RETURN) ||
      ConsumeKeyPress(VK_SPACE) ||
      ConsumeKeyPress(VK_ESCAPE))
  {
    HideLoginMessage();
    return;
  }

  m_byMenuHover = kInvalidMenuIndex;
  m_byMenuPressed = kInvalidMenuIndex;
  m_byCharacterMenuHover = kInvalidCharacterButton;
  m_byCharacterMenuPressed = kInvalidCharacterButton;
  ClearOpeningMouseTransitions();
}

void CGP_LogIn::RenderOpeningScreen(void) const
{
  CSprite *l_pLoginSprite = m_cLoginSpriteMgr.GetSprite(SP_ID_LOGIN);
  if (!l_pLoginSprite)
  {
    return;
  }

  RECT l_sPanelRect;
  RECT l_aButtonRect[3];
  if (!GetOpeningMenuLayout(&l_sPanelRect, l_aButtonRect))
  {
    return;
  }

  const char *l_apMenuText[3] = { "START", "CREDITS", "EXIT" };

  DrawSpriteFrame(l_pLoginSprite,
                  kOpeningMenuAction,
                  kOpeningMenuPanelFrame,
                  l_sPanelRect.left,
                  l_sPanelRect.top,
                  l_sPanelRect.right - l_sPanelRect.left,
                  l_sPanelRect.bottom - l_sPanelRect.top);

  for (int i = 0; i < 3; ++i)
  {
    const int l_nButtonWidth = l_aButtonRect[i].right - l_aButtonRect[i].left;
    const int l_nButtonHeight = l_aButtonRect[i].bottom - l_aButtonRect[i].top;
    DWORD l_dwFrame = kOpeningMenuButtonNormalFrame;
    if (m_byMenuPressed == i && m_bLeftButtonDown)
    {
      l_dwFrame = kOpeningMenuButtonPressedFrame;
    }
    else if (m_byMenuHover == i || m_byMenuSelection == i)
    {
      l_dwFrame = kOpeningMenuButtonSelectedFrame;
    }

    DrawSpriteFrame(l_pLoginSprite,
                    kOpeningMenuAction,
                    l_dwFrame,
                    l_aButtonRect[i].left,
                    l_aButtonRect[i].top,
                    l_nButtonWidth,
                    l_nButtonHeight);

    const DWORD l_dwTextColor = (l_dwFrame == kOpeningMenuButtonNormalFrame)
                                  ? kOpeningMenuNormalTextColor
                                  : kOpeningMenuFocusedTextColor;
    DrawLoginLine(GetCenteredTextX(l_aButtonRect[i].left + (l_nButtonWidth / 2), l_apMenuText[i]),
                  l_aButtonRect[i].top + ((l_nButtonHeight - kFontHeight) / 2),
                  l_apMenuText[i],
                  l_dwTextColor);
  }
}

void CGP_LogIn::RenderCreditsRoll(void) const
{
  if (m_vecTitleCredits.empty())
  {
    return;
  }

  int l_nScreenX = 0;
  int l_nScreenY = 0;
  GetScreenSize(&l_nScreenX, &l_nScreenY);
  if (l_nScreenX <= 0 || l_nScreenY <= 0)
  {
    return;
  }

  const float l_fElapsedSeconds = static_cast<float>(timeGetTime() - m_dwCreditsStartTick) / 1000.0f;
  float l_fCurrentY = static_cast<float>(l_nScreenY) - (l_fElapsedSeconds * 15.0f);
  for (size_t i = 0; i < m_vecTitleCredits.size(); ++i)
  {
    const TITLE_CREDIT_LINE &l_sLine = m_vecTitleCredits[i];
    if (l_sLine.nStyle == -1 || l_sLine.szText[0] == '\0')
    {
      l_fCurrentY += 14.0f;
      continue;
    }

    DrawLoginLine(GetCenteredTextX(l_nScreenX / 2, l_sLine.szText),
                  static_cast<int>(l_fCurrentY),
                  l_sLine.szText,
                  GetCreditLineColor(l_sLine.nStyle));
    l_fCurrentY += 18.0f;
  }

  if (l_fCurrentY <= -10.0f)
  {
    const_cast<CGP_LogIn *>(this)->StartCreditsRoll();
  }
}

void CGP_LogIn::RenderUILockScreen(void) const
{
  int l_nScreenX = 0;
  int l_nScreenY = 0;
  GetScreenSize(&l_nScreenX, &l_nScreenY);
  if (l_nScreenX <= 0 || l_nScreenY <= 0)
  {
    return;
  }

  DrawBox(60, 120, l_nScreenX - 60, l_nScreenY - 120, 0xB0181010, 0xFFD08080);
  DrawLoginLine(84, 156, "UI Lock / FireGuard gate reached before character selection.", 0xFFFFFFFF);
  DrawLoginLine(84,
                190,
                "This matches the latest-client flow for non-GM accounts, but RFClient does not implement the certify/find-password requests yet.",
                0xFFFFD0D0);
  DrawLoginLine(84,
                224,
                "Missing behavior stays explicit here instead of faking a completed UI lock screen.",
                0xFFFFD0D0);

  CNetworkMgr *l_pNetworkMgr = _GetNetworkMgr();
  if (l_pNetworkMgr && l_pNetworkMgr->HasUILockInform())
  {
    char l_szLine[256];
    const _uilock_inform_request_zocl &l_sUILockInform = l_pNetworkMgr->GetUILockInform();
    sprintf_s(l_szLine,
              "Inform state=%u fail=%u hint=%u findFail=%u",
              static_cast<unsigned>(static_cast<unsigned __int8>(l_sUILockInform.byInformType)),
              static_cast<unsigned>(static_cast<unsigned __int8>(l_sUILockInform.byFailCount)),
              static_cast<unsigned>(static_cast<unsigned __int8>(l_sUILockInform.byHintIndex)),
              static_cast<unsigned>(static_cast<unsigned __int8>(l_sUILockInform.byFindPassFailCount)));
    DrawLoginLine(84, 270, l_szLine, 0xFFFFFFFF);
  }

  DrawLoginLine(84, l_nScreenY - 168, "Press Esc to go back to the opening screen.", 0xFFE0E0E0);
}

void CGP_LogIn::RenderCharacterSelectionScreen(void) const
{
  CSprite *l_pLoginSprite = m_cLoginSpriteMgr.GetSprite(SP_ID_LOGIN);
  if (!l_pLoginSprite)
  {
    return;
  }

  CHARACTER_SELECT_LAYOUT l_sLayout;
  if (!GetCharacterSelectionLayout(&l_sLayout))
  {
    return;
  }
  ApplyCharacterSelectionOpenAnimation(&l_sLayout);

  CNetworkMgr *l_pNetworkMgr = _GetNetworkMgr();
  const _REGED_AVATOR_DB *l_pSelectedAvatar = NULL;
  if (l_pNetworkMgr && l_pNetworkMgr->HasRegedCharResult() &&
      l_pNetworkMgr->GetResultOfUserInfo() == USER_INFO_SUCCESS)
  {
    l_pSelectedAvatar = FindRegedAvatarBySlot(l_pNetworkMgr->GetRegedCharResult(), m_bySelectedCharacterSlot);
  }

  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterSelectAction,
                  0,
                  l_sLayout.sUpperBase.left,
                  l_sLayout.sUpperBase.top,
                  l_sLayout.sUpperBase.right - l_sLayout.sUpperBase.left,
                  l_sLayout.sUpperBase.bottom - l_sLayout.sUpperBase.top,
                  kCharacterSelectSpriteColor);
  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterSelectAction,
                  1,
                  l_sLayout.sLeftCharInfoBoard.left,
                  l_sLayout.sLeftCharInfoBoard.top,
                  l_sLayout.sLeftCharInfoBoard.right - l_sLayout.sLeftCharInfoBoard.left,
                  l_sLayout.sLeftCharInfoBoard.bottom - l_sLayout.sLeftCharInfoBoard.top,
                  kCharacterSelectInfoBoardColor);
  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterSelectAction,
                  2,
                  l_sLayout.sRightCharInfoBoard.left,
                  l_sLayout.sRightCharInfoBoard.top,
                  l_sLayout.sRightCharInfoBoard.right - l_sLayout.sRightCharInfoBoard.left,
                  l_sLayout.sRightCharInfoBoard.bottom - l_sLayout.sRightCharInfoBoard.top,
                  kCharacterSelectInfoBoardColor);
  DrawSpriteSheetCell(l_pLoginSprite,
                      kCharacterSelectAniAction,
                      0,
                      l_sLayout.sUpperBoardAni.left,
                      l_sLayout.sUpperBoardAni.top,
                      l_sLayout.sUpperBoardAni.right - l_sLayout.sUpperBoardAni.left,
                      l_sLayout.sUpperBoardAni.bottom - l_sLayout.sUpperBoardAni.top,
                      2,
                      static_cast<int>(m_sUpperBoardAnimation.fCurFrameNo),
                      kCharacterSelectSpriteColor);

  if (l_pSelectedAvatar)
  {
    char l_szName[32];
    char l_szMoney[32];
    char l_szLine[64];
    CopyRegedAvatarName(l_pSelectedAvatar, l_szName, sizeof(l_szName));

    sprintf_s(l_szLine, sizeof(l_szLine), "NAME : %s", l_szName);
    DrawLoginLine(l_sLayout.sNameText.left, l_sLayout.sNameText.top, l_szLine, kCharacterSelectTextColor);

    sprintf_s(l_szLine, sizeof(l_szLine), "Lvl. : %u", static_cast<unsigned>(l_pSelectedAvatar->m_byLevel));
    DrawLoginLine(l_sLayout.sLevelText.left, l_sLayout.sLevelText.top, l_szLine, kCharacterSelectTextColor);

    FormatCommaNumber(l_pSelectedAvatar->m_dwDalant, l_szMoney, sizeof(l_szMoney));
    sprintf_s(l_szLine, sizeof(l_szLine), "Dalant : %s", l_szMoney);
    DrawLoginLine(l_sLayout.sDalantText.left, l_sLayout.sDalantText.top, l_szLine, kCharacterSelectTextColor);

    FormatCommaNumber(l_pSelectedAvatar->m_dwGold, l_szMoney, sizeof(l_szMoney));
    sprintf_s(l_szLine, sizeof(l_szLine), "Gold : %s", l_szMoney);
    DrawLoginLine(l_sLayout.sGoldText.left, l_sLayout.sGoldText.top, l_szLine, kCharacterSelectTextColor);
  }

  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterSelectAction,
                  3,
                  l_sLayout.sLowerBase.left,
                  l_sLayout.sLowerBase.top,
                  l_sLayout.sLowerBase.right - l_sLayout.sLowerBase.left,
                  l_sLayout.sLowerBase.bottom - l_sLayout.sLowerBase.top,
                  kCharacterSelectSpriteColor);
  DrawSpriteSheetCell(l_pLoginSprite,
                      kCharacterSelectAniAction,
                      1,
                      l_sLayout.sLowerBoardAni.left,
                      l_sLayout.sLowerBoardAni.top,
                      l_sLayout.sLowerBoardAni.right - l_sLayout.sLowerBoardAni.left,
                      l_sLayout.sLowerBoardAni.bottom - l_sLayout.sLowerBoardAni.top,
                      2,
                      static_cast<int>(m_sLowerBoardAnimation.fCurFrameNo),
                      kCharacterSelectSpriteColor);

  const bool l_bPrevPressed = m_byCharacterMenuPressed == CHARACTER_SELECT_BUTTON_PREV && m_bLeftButtonDown;
  const bool l_bNextPressed = m_byCharacterMenuPressed == CHARACTER_SELECT_BUTTON_NEXT && m_bLeftButtonDown;
  const bool l_bCreatePressed = m_byCharacterMenuPressed == CHARACTER_SELECT_BUTTON_CREATE && m_bLeftButtonDown;
  const bool l_bDeletePressed = m_byCharacterMenuPressed == CHARACTER_SELECT_BUTTON_DELETE && m_bLeftButtonDown;
  const bool l_bConnectPressed = m_byCharacterMenuPressed == CHARACTER_SELECT_BUTTON_CONNECT && m_bLeftButtonDown;
  const bool l_bExitPressed = m_byCharacterMenuPressed == CHARACTER_SELECT_BUTTON_EXIT && m_bLeftButtonDown;
  const bool l_bPrevHovered = m_byCharacterMenuHover == CHARACTER_SELECT_BUTTON_PREV;
  const bool l_bNextHovered = m_byCharacterMenuHover == CHARACTER_SELECT_BUTTON_NEXT;
  const bool l_bCreateHovered = m_byCharacterMenuHover == CHARACTER_SELECT_BUTTON_CREATE;
  const bool l_bDeleteHovered = m_byCharacterMenuHover == CHARACTER_SELECT_BUTTON_DELETE;
  const bool l_bExitHovered = m_byCharacterMenuHover == CHARACTER_SELECT_BUTTON_EXIT;

  const DWORD l_dwCreateFrame = l_bCreatePressed ? 6 : (l_bCreateHovered ? 5 : 4);
  const DWORD l_dwDeleteFrame = l_bDeletePressed ? 10 : (l_bDeleteHovered ? 9 : 8);
  const DWORD l_dwPrevFrame = l_bPrevPressed ? 14 : (l_bPrevHovered ? 13 : 12);
  const DWORD l_dwNextFrame = l_bNextPressed ? 17 : (l_bNextHovered ? 16 : 15);
  const DWORD l_dwConnectFrame = l_bConnectPressed ? 19 : 18;
  const DWORD l_dwExitFrame = l_bExitPressed ? 22 : (l_bExitHovered ? 21 : 20);

  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterSelectAction,
                  l_dwCreateFrame,
                  l_sLayout.sCreateButton.left,
                  l_sLayout.sCreateButton.top,
                  l_sLayout.sCreateButton.right - l_sLayout.sCreateButton.left,
                  l_sLayout.sCreateButton.bottom - l_sLayout.sCreateButton.top,
                  kCharacterSelectButtonColor);
  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterSelectAction,
                  l_dwDeleteFrame,
                  l_sLayout.sDeleteButton.left,
                  l_sLayout.sDeleteButton.top,
                  l_sLayout.sDeleteButton.right - l_sLayout.sDeleteButton.left,
                  l_sLayout.sDeleteButton.bottom - l_sLayout.sDeleteButton.top,
                  kCharacterSelectButtonColor);
  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterSelectAction,
                  l_dwPrevFrame,
                  l_sLayout.sPrevButton.left,
                  l_sLayout.sPrevButton.top,
                  l_sLayout.sPrevButton.right - l_sLayout.sPrevButton.left,
                  l_sLayout.sPrevButton.bottom - l_sLayout.sPrevButton.top,
                  kCharacterSelectButtonColor);
  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterSelectAction,
                  l_dwNextFrame,
                  l_sLayout.sNextButton.left,
                  l_sLayout.sNextButton.top,
                  l_sLayout.sNextButton.right - l_sLayout.sNextButton.left,
                  l_sLayout.sNextButton.bottom - l_sLayout.sNextButton.top,
                  kCharacterSelectButtonColor);
  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterSelectAction,
                  l_dwConnectFrame,
                  l_sLayout.sConnectButton.left,
                  l_sLayout.sConnectButton.top,
                  l_sLayout.sConnectButton.right - l_sLayout.sConnectButton.left,
                  l_sLayout.sConnectButton.bottom - l_sLayout.sConnectButton.top,
                  kCharacterSelectSpriteColor);
  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterSelectAction,
                  l_dwExitFrame,
                  l_sLayout.sExitButton.left,
                  l_sLayout.sExitButton.top,
                  l_sLayout.sExitButton.right - l_sLayout.sExitButton.left,
                  l_sLayout.sExitButton.bottom - l_sLayout.sExitButton.top,
                  kCharacterSelectButtonColor);

  DrawCharacterSelectButtonLabel(l_sLayout.sCreateButton, "Create", 80, 18, l_bCreateHovered, l_bCreatePressed);
  DrawCharacterSelectButtonLabel(l_sLayout.sDeleteButton, "Delete", 33, 18, l_bDeleteHovered, l_bDeletePressed);
  DrawCharacterSelectButtonLabel(l_sLayout.sPrevButton, "Prev. Slot", 36, 10, l_bPrevHovered, l_bPrevPressed);
  DrawCharacterSelectButtonLabel(l_sLayout.sNextButton, "Next Slot", 26, 10, l_bNextHovered, l_bNextPressed);
  DrawCenteredCharacterSelectButtonLabel(l_sLayout.sExitButton, "End", l_bExitHovered, l_bExitPressed);
}

void CGP_LogIn::RenderCharacterCreateScreen(void) const
{
  if (_GetMainApp() && _GetMainApp()->IsLoginLobbyCameraAnimating())
  {
    return;
  }

  if (m_byCreateStep == CHARACTER_CREATE_STEP_RACE)
  {
    RenderCharacterCreateRaceScreen();
  }
  else if (m_byCreateStep == CHARACTER_CREATE_STEP_ATTRIBUTE)
  {
    RenderCharacterCreateAttributeScreen();
  }
  else
  {
    RenderCharacterCreateDetailScreen();
  }
}

void CGP_LogIn::RenderCharacterCreateRaceScreen(void) const
{
  CSprite *l_pLoginSprite = m_cLoginSpriteMgr.GetSprite(SP_ID_LOGIN);
  if (!l_pLoginSprite)
  {
    return;
  }

  CHARACTER_CREATE_RACE_LAYOUT l_sLayout;
  if (!GetCharacterCreateRaceLayout(&l_sLayout))
  {
    return;
  }

  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterCreateRaceAction,
                  13 + ClampCreateRaceSelection(m_byCreateRaceSelection),
                  l_sLayout.sRaceProfile.left,
                  l_sLayout.sRaceProfile.top,
                  l_sLayout.sRaceProfile.right - l_sLayout.sRaceProfile.left,
                  l_sLayout.sRaceProfile.bottom - l_sLayout.sRaceProfile.top,
                  0xD0FFFFFF);
  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterCreateRaceAction,
                  0,
                  l_sLayout.sUpperBase.left,
                  l_sLayout.sUpperBase.top,
                  l_sLayout.sUpperBase.right - l_sLayout.sUpperBase.left,
                  l_sLayout.sUpperBase.bottom - l_sLayout.sUpperBase.top,
                  kCharacterSelectSpriteColor);
  DrawSpriteSheetCell(l_pLoginSprite,
                      kCharacterSelectAniAction,
                      0,
                      l_sLayout.sUpperBoardAni.left,
                      l_sLayout.sUpperBoardAni.top,
                      l_sLayout.sUpperBoardAni.right - l_sLayout.sUpperBoardAni.left,
                      l_sLayout.sUpperBoardAni.bottom - l_sLayout.sUpperBoardAni.top,
                      2,
                      static_cast<int>(m_sUpperBoardAnimation.fCurFrameNo),
                      kCharacterSelectSpriteColor);

  for (BYTE i = 0; i < 3; ++i)
  {
    const bool l_bPressed = m_byCreateMenuPressed == i && m_bLeftButtonDown;
    const bool l_bHovered = m_byCreateMenuHover == i;
    const DWORD l_dwFrame = l_bPressed ? 3 : (l_bHovered ? 2 : 1);
    DrawSpriteFrame(l_pLoginSprite,
                    kCharacterCreateRaceAction,
                    l_dwFrame,
                    l_sLayout.sRaceButton[i].left,
                    l_sLayout.sRaceButton[i].top,
                    l_sLayout.sRaceButton[i].right - l_sLayout.sRaceButton[i].left,
                    l_sLayout.sRaceButton[i].bottom - l_sLayout.sRaceButton[i].top,
                    kCharacterSelectButtonColor);
    DrawCenteredCharacterSelectButtonLabelFixedColor(l_sLayout.sRaceButton[i],
                                                     GetCreateRaceLabel(i),
                                                     l_bPressed,
                                                     kCharacterSelectTextColor);
  }

  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterCreateRaceAction,
                  4,
                  l_sLayout.sLowerBase.left,
                  l_sLayout.sLowerBase.top,
                  l_sLayout.sLowerBase.right - l_sLayout.sLowerBase.left,
                  l_sLayout.sLowerBase.bottom - l_sLayout.sLowerBase.top,
                  kCharacterSelectSpriteColor);

  const bool l_bOkPressed = m_byCreateMenuPressed == CHARACTER_CREATE_BUTTON_OK && m_bLeftButtonDown;
  const bool l_bCancelPressed = m_byCreateMenuPressed == CHARACTER_CREATE_BUTTON_CANCEL && m_bLeftButtonDown;
  const bool l_bOkHovered = m_byCreateMenuHover == CHARACTER_CREATE_BUTTON_OK;
  const bool l_bCancelHovered = m_byCreateMenuHover == CHARACTER_CREATE_BUTTON_CANCEL;
  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterCreateRaceAction,
                  l_bOkPressed ? 7 : (l_bOkHovered ? 6 : 5),
                  l_sLayout.sOkButton.left,
                  l_sLayout.sOkButton.top,
                  l_sLayout.sOkButton.right - l_sLayout.sOkButton.left,
                  l_sLayout.sOkButton.bottom - l_sLayout.sOkButton.top,
                  kCharacterSelectButtonColor);
  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterCreateRaceAction,
                  l_bCancelPressed ? 11 : (l_bCancelHovered ? 10 : 9),
                  l_sLayout.sCancelButton.left,
                  l_sLayout.sCancelButton.top,
                  l_sLayout.sCancelButton.right - l_sLayout.sCancelButton.left,
                  l_sLayout.sCancelButton.bottom - l_sLayout.sCancelButton.top,
                  kCharacterSelectButtonColor);
  DrawCharacterSelectButtonLabelFixedColor(l_sLayout.sOkButton,
                                           "OK",
                                           60,
                                           12,
                                           l_bOkPressed,
                                           kCharacterSelectTextColor);
  DrawCharacterSelectButtonLabelFixedColor(l_sLayout.sCancelButton,
                                           "Cancel",
                                           110,
                                           12,
                                           l_bCancelPressed,
                                           kCharacterSelectTextColor);
}

void CGP_LogIn::RenderCharacterCreateAttributeScreen(void) const
{
  CSprite *l_pLoginSprite = m_cLoginSpriteMgr.GetSprite(SP_ID_LOGIN);
  if (!l_pLoginSprite)
  {
    return;
  }

  CHARACTER_CREATE_ATTRIBUTE_LAYOUT l_sLayout;
  if (!GetCharacterCreateAttributeLayout(&l_sLayout))
  {
    return;
  }

  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterCreateDetailAction,
                  13,
                  l_sLayout.sPanel.left,
                  l_sLayout.sPanel.top,
                  l_sLayout.sPanel.right - l_sLayout.sPanel.left,
                  l_sLayout.sPanel.bottom - l_sLayout.sPanel.top,
                  0xC0FFFFFF);
  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterCreateDetailAction,
                  0,
                  l_sLayout.sTitleBase.left,
                  l_sLayout.sTitleBase.top,
                  l_sLayout.sTitleBase.right - l_sLayout.sTitleBase.left,
                  l_sLayout.sTitleBase.bottom - l_sLayout.sTitleBase.top,
                  0xC0FFFFFF);
  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterCreateDetailAction,
                  1,
                  l_sLayout.sLowerBase.left,
                  l_sLayout.sLowerBase.top,
                  l_sLayout.sLowerBase.right - l_sLayout.sLowerBase.left,
                  l_sLayout.sLowerBase.bottom - l_sLayout.sLowerBase.top,
                  0xC0FFFFFF);

  DrawLoginLine(GetCenteredTextX((l_sLayout.sTitleBase.left + l_sLayout.sTitleBase.right) / 2, "Create"),
                l_sLayout.sTitleBase.top + 7,
                "Create",
                0xFFD3D3D3);
  DrawLoginLine(GetCenteredTextX((l_sLayout.sPanel.left + l_sLayout.sPanel.right) / 2, "Class"),
                l_sLayout.sPanel.top + 35,
                "Class",
                0xFFD3D3D3);

  for (BYTE i = 0; i < 4; ++i)
  {
    if (IsAccretiaCreateRace(m_byCreateRaceSelection) && i == CHARACTER_CREATE_ATTRIBUTE_FORCE)
    {
      continue;
    }

    const bool l_bSelected = m_byCreateAttributeSelection == i;
    const bool l_bPressed = m_byCreateMenuPressed == static_cast<BYTE>(CHARACTER_CREATE_BUTTON_ATTRIBUTE_BASE + i) &&
                            m_bLeftButtonDown;
    const DWORD l_dwFrame = l_bSelected ? (14 + i * 3 + 2) : (l_bPressed ? (14 + i * 3 + 1) : (14 + i * 3));
    DrawSpriteFrame(l_pLoginSprite,
                    kCharacterCreateDetailAction,
                    l_dwFrame,
                    l_sLayout.sAttributeButton[i].left,
                    l_sLayout.sAttributeButton[i].top,
                    l_sLayout.sAttributeButton[i].right - l_sLayout.sAttributeButton[i].left,
                    l_sLayout.sAttributeButton[i].bottom - l_sLayout.sAttributeButton[i].top,
                    0xFFFFFFFF);
  }

  const BYTE l_byAttribute = m_byCreateAttributeSelection < 4 ? m_byCreateAttributeSelection : 0;
  DrawLoginLine(GetCenteredTextX((l_sLayout.sPanel.left + l_sLayout.sPanel.right) / 2, GetCreateAttributeLabel(l_byAttribute)),
                l_sLayout.sDescText[0].top,
                GetCreateAttributeLabel(l_byAttribute),
                0xFFB4DAE1);

  DrawLoginLine(GetCenteredTextX((l_sLayout.sPanel.left + l_sLayout.sPanel.right) / 2, "Status"),
                l_sLayout.sPanel.top + 146,
                "Status",
                0xFFF4D8C0);
  DrawLoginLine(GetCenteredTextX((l_sLayout.sPanel.left + l_sLayout.sPanel.right) / 2, "Combat Abilities"),
                l_sLayout.sPanel.top + 229,
                "Combat Abilities",
                0xFFF4D8C0);
  DrawLoginLine(GetCenteredTextX((l_sLayout.sPanel.left + l_sLayout.sPanel.right) / 2, "Craft Abilities"),
                l_sLayout.sPanel.top + 348,
                "Craft Abilities",
                0xFFF4D8C0);

  static const char *kBasicLabel[3] = { "HP", "FP", "SP" };
  const char *kBattleLabel[6] =
  {
    "Melee",
    "Ranged",
    GetCreateRaceBattleSkillLabel(m_byCreateRaceSelection),
    "Force",
    "Shield",
    "Defense"
  };
  static const char *kMakeLabel[3] = { "Weapons", "Armor", "Ammo" };
  const float l_fBasicBase = 0.35f + (0.05f * ClampCreateRaceSelection(m_byCreateRaceSelection));
  for (int i = 0; i < 3; ++i)
  {
    DrawLoginLine(l_sLayout.sBasicPointText[i].left, l_sLayout.sBasicPointText[i].top, kBasicLabel[i], 0xFFD3D3D3);
    DrawCreateAttributeGaugeBar(l_pLoginSprite,
                                l_sLayout.sBasicPointBase[i],
                                l_fBasicBase + (0.08f * i) + (0.04f * l_byAttribute),
                                27 + i);

    DrawLoginLine(l_sLayout.sMakeSkillText[i].left, l_sLayout.sMakeSkillText[i].top, kMakeLabel[i], 0xFFD3D3D3);
    DrawCreateAttributeGaugeBar(l_pLoginSprite,
                                l_sLayout.sMakeSkillBase[i],
                                l_byAttribute == CHARACTER_CREATE_ATTRIBUTE_SPECIALIST ? 0.82f - (0.08f * i) : 0.24f + (0.05f * i),
                                30);
  }

  for (int i = 0; i < 6; ++i)
  {
    DrawLoginLine(l_sLayout.sBattleSkillText[i].left, l_sLayout.sBattleSkillText[i].top, kBattleLabel[i], 0xFFD3D3D3);
    float l_fRate = 0.25f + (0.04f * i);
    if ((l_byAttribute == CHARACTER_CREATE_ATTRIBUTE_MELEE && i == 0) ||
        (l_byAttribute == CHARACTER_CREATE_ATTRIBUTE_MISSILE && i == 1) ||
        (l_byAttribute == CHARACTER_CREATE_ATTRIBUTE_FORCE && i == 3))
    {
      l_fRate = 0.82f;
    }
    DrawCreateAttributeGaugeBar(l_pLoginSprite, l_sLayout.sBattleSkillBase[i], l_fRate, 30);
  }

  const bool l_bOkPressed = m_byCreateMenuPressed == CHARACTER_CREATE_BUTTON_OK && m_bLeftButtonDown;
  const bool l_bCancelPressed = m_byCreateMenuPressed == CHARACTER_CREATE_BUTTON_CANCEL && m_bLeftButtonDown;
  const bool l_bOkHovered = m_byCreateMenuHover == CHARACTER_CREATE_BUTTON_OK;
  const bool l_bCancelHovered = m_byCreateMenuHover == CHARACTER_CREATE_BUTTON_CANCEL;
  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterCreateDetailAction,
                  l_bOkPressed ? 4 : (l_bOkHovered ? 3 : 2),
                  l_sLayout.sOkButton.left,
                  l_sLayout.sOkButton.top,
                  l_sLayout.sOkButton.right - l_sLayout.sOkButton.left,
                  l_sLayout.sOkButton.bottom - l_sLayout.sOkButton.top,
                  kCharacterSelectButtonColor);
  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterCreateDetailAction,
                  l_bCancelPressed ? 7 : (l_bCancelHovered ? 6 : 5),
                  l_sLayout.sCancelButton.left,
                  l_sLayout.sCancelButton.top,
                  l_sLayout.sCancelButton.right - l_sLayout.sCancelButton.left,
                  l_sLayout.sCancelButton.bottom - l_sLayout.sCancelButton.top,
                  kCharacterSelectButtonColor);
  DrawCenteredCharacterSelectButtonLabel(l_sLayout.sOkButton, "OK", l_bOkHovered, l_bOkPressed);
  DrawCenteredCharacterSelectButtonLabel(l_sLayout.sCancelButton, "Cancel", l_bCancelHovered, l_bCancelPressed);
}

void CGP_LogIn::RenderCharacterCreateDetailScreen(void) const
{
  CSprite *l_pLoginSprite = m_cLoginSpriteMgr.GetSprite(SP_ID_LOGIN);
  if (!l_pLoginSprite)
  {
    return;
  }

  CHARACTER_CREATE_DETAIL_LAYOUT l_sLayout;
  if (!GetCharacterCreateDetailLayout(&l_sLayout))
  {
    return;
  }

  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterCreateDetailAction,
                  8,
                  l_sLayout.sPanel.left,
                  l_sLayout.sPanel.top,
                  l_sLayout.sPanel.right - l_sLayout.sPanel.left,
                  l_sLayout.sPanel.bottom - l_sLayout.sPanel.top,
                  0xC0FFFFFF);
  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterCreateDetailAction,
                  0,
                  l_sLayout.sTitleBase.left,
                  l_sLayout.sTitleBase.top,
                  l_sLayout.sTitleBase.right - l_sLayout.sTitleBase.left,
                  l_sLayout.sTitleBase.bottom - l_sLayout.sTitleBase.top,
                  0xC0FFFFFF);
  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterCreateDetailAction,
                  1,
                  l_sLayout.sLowerBase.left,
                  l_sLayout.sLowerBase.top,
                  l_sLayout.sLowerBase.right - l_sLayout.sLowerBase.left,
                  l_sLayout.sLowerBase.bottom - l_sLayout.sLowerBase.top,
                  0xC0FFFFFF);

  DrawLoginLine(GetCenteredTextX((l_sLayout.sTitleBase.left + l_sLayout.sTitleBase.right) / 2, "Create"),
                l_sLayout.sTitleBase.top + 7,
                "Create",
                0xFFD3D3D3);
  DrawLoginLine(GetCenteredTextX((l_sLayout.sPanel.left + l_sLayout.sPanel.right) / 2, "Gender"),
                l_sLayout.sPanel.top + 38,
                "Gender",
                0xFFD3D3D3);
  DrawLoginLine(GetCenteredTextX((l_sLayout.sPanel.left + l_sLayout.sPanel.right) / 2, "Name"),
                l_sLayout.sPanel.top + 241,
                "Name",
                0xFFD3D3D3);

  static const char *kSelectLabel[CHARACTER_CREATE_SELECT_COUNT] =
  {
    "Sex",
    "Hair",
    "Face",
    "Upper",
    "Lower",
    "Arms",
    "Feet"
  };

  for (int i = 0; i < CHARACTER_CREATE_SELECT_COUNT; ++i)
  {
    const bool l_bSexDisabled = IsAccretiaCreateRace(m_byCreateRaceSelection) &&
                                i == CHARACTER_CREATE_SELECT_SEX;
    char l_szValue[32];
    if (i == CHARACTER_CREATE_SELECT_SEX)
    {
      sprintf_s(l_szValue,
                sizeof(l_szValue),
                "%s",
                IsAccretiaCreateRace(m_byCreateRaceSelection)
                  ? "A"
                  : (m_byCreateSexSelection ? "F" : "M"));
    }
    else
    {
      sprintf_s(l_szValue, sizeof(l_szValue), "%s", kSelectLabel[i]);
    }

    DrawLoginLine(l_sLayout.sSelectItem[i].left,
                  l_sLayout.sSelectItem[i].top + 2,
                  l_szValue,
                  l_bSexDisabled ? 0xFF808080 : 0xFFD3D3D3);

    if (!l_bSexDisabled)
    {
      const BYTE l_byLeftButton = static_cast<BYTE>(CHARACTER_CREATE_BUTTON_DETAIL_LEFT_BASE + i);
      const BYTE l_byRightButton = static_cast<BYTE>(CHARACTER_CREATE_BUTTON_DETAIL_RIGHT_BASE + i);
      const bool l_bLeftPressed = m_byCreateMenuPressed == l_byLeftButton && m_bLeftButtonDown;
      const bool l_bRightPressed = m_byCreateMenuPressed == l_byRightButton && m_bLeftButtonDown;
      DrawSpriteFrame(l_pLoginSprite,
                      kCharacterCreateDetailAction,
                      l_bLeftPressed ? 12 : 11,
                      l_sLayout.sSelectLeftButton[i].left,
                      l_sLayout.sSelectLeftButton[i].top,
                      l_sLayout.sSelectLeftButton[i].right - l_sLayout.sSelectLeftButton[i].left,
                      l_sLayout.sSelectLeftButton[i].bottom - l_sLayout.sSelectLeftButton[i].top,
                      kCharacterSelectButtonColor);
      DrawSpriteFrame(l_pLoginSprite,
                      kCharacterCreateDetailAction,
                      l_bRightPressed ? 10 : 9,
                      l_sLayout.sSelectRightButton[i].left,
                      l_sLayout.sSelectRightButton[i].top,
                      l_sLayout.sSelectRightButton[i].right - l_sLayout.sSelectRightButton[i].left,
                      l_sLayout.sSelectRightButton[i].bottom - l_sLayout.sSelectRightButton[i].top,
                      kCharacterSelectButtonColor);
    }
  }

  Draw2DRectangle(l_sLayout.sNameInput.left,
                  l_sLayout.sNameInput.top,
                  l_sLayout.sNameInput.right,
                  l_sLayout.sNameInput.bottom,
                  0xC0101010);
  Draw2DRectangle(l_sLayout.sNameInput.left,
                  l_sLayout.sNameInput.top,
                  l_sLayout.sNameInput.right,
                  l_sLayout.sNameInput.top + 1,
                  0xC07FA9DC);
  DrawLoginLine(l_sLayout.sNameInput.left + 4,
                l_sLayout.sNameInput.top,
                m_szCreateCharacterName,
                0xFFFFFFFF);

  const bool l_bOkPressed = m_byCreateMenuPressed == CHARACTER_CREATE_BUTTON_OK && m_bLeftButtonDown;
  const bool l_bCancelPressed = m_byCreateMenuPressed == CHARACTER_CREATE_BUTTON_CANCEL && m_bLeftButtonDown;
  const bool l_bOkHovered = m_byCreateMenuHover == CHARACTER_CREATE_BUTTON_OK;
  const bool l_bCancelHovered = m_byCreateMenuHover == CHARACTER_CREATE_BUTTON_CANCEL;
  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterCreateDetailAction,
                  l_bOkPressed ? 4 : (l_bOkHovered ? 3 : 2),
                  l_sLayout.sOkButton.left,
                  l_sLayout.sOkButton.top,
                  l_sLayout.sOkButton.right - l_sLayout.sOkButton.left,
                  l_sLayout.sOkButton.bottom - l_sLayout.sOkButton.top,
                  kCharacterSelectButtonColor);
  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterCreateDetailAction,
                  l_bCancelPressed ? 7 : (l_bCancelHovered ? 6 : 5),
                  l_sLayout.sCancelButton.left,
                  l_sLayout.sCancelButton.top,
                  l_sLayout.sCancelButton.right - l_sLayout.sCancelButton.left,
                  l_sLayout.sCancelButton.bottom - l_sLayout.sCancelButton.top,
                  kCharacterSelectButtonColor);
  DrawCenteredCharacterSelectButtonLabel(l_sLayout.sOkButton, "OK", l_bOkHovered, l_bOkPressed);
  DrawCenteredCharacterSelectButtonLabel(l_sLayout.sCancelButton, "Cancel", l_bCancelHovered, l_bCancelPressed);
  RenderCharacterCreateRotateControls();
}

void CGP_LogIn::RenderCharacterCreateRotateControls(void) const
{
  CSprite *l_pLoginSprite = m_cLoginSpriteMgr.GetSprite(SP_ID_LOGIN);
  if (!l_pLoginSprite)
  {
    return;
  }

  CHARACTER_CREATE_ROTATE_LAYOUT l_sLayout;
  if (!GetCharacterCreateRotateLayout(&l_sLayout))
  {
    return;
  }

  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterCreateRaceAction,
                  4,
                  l_sLayout.sLowerBase.left,
                  l_sLayout.sLowerBase.top,
                  l_sLayout.sLowerBase.right - l_sLayout.sLowerBase.left,
                  l_sLayout.sLowerBase.bottom - l_sLayout.sLowerBase.top,
                  kCharacterSelectSpriteColor);

  const bool l_bLeftPressed = m_byCreateMenuPressed == CHARACTER_CREATE_BUTTON_ROTATE_LEFT && m_bLeftButtonDown;
  const bool l_bRightPressed = m_byCreateMenuPressed == CHARACTER_CREATE_BUTTON_ROTATE_RIGHT && m_bLeftButtonDown;
  const bool l_bLeftHovered = m_byCreateMenuHover == CHARACTER_CREATE_BUTTON_ROTATE_LEFT;
  const bool l_bRightHovered = m_byCreateMenuHover == CHARACTER_CREATE_BUTTON_ROTATE_RIGHT;

  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterCreateRaceAction,
                  l_bLeftPressed ? 7 : (l_bLeftHovered ? 6 : 5),
                  l_sLayout.sRotateLeftButton.left,
                  l_sLayout.sRotateLeftButton.top,
                  l_sLayout.sRotateLeftButton.right - l_sLayout.sRotateLeftButton.left,
                  l_sLayout.sRotateLeftButton.bottom - l_sLayout.sRotateLeftButton.top,
                  kCharacterSelectButtonColor);
  DrawSpriteFrame(l_pLoginSprite,
                  kCharacterCreateRaceAction,
                  l_bRightPressed ? 11 : (l_bRightHovered ? 10 : 9),
                  l_sLayout.sRotateRightButton.left,
                  l_sLayout.sRotateRightButton.top,
                  l_sLayout.sRotateRightButton.right - l_sLayout.sRotateRightButton.left,
                  l_sLayout.sRotateRightButton.bottom - l_sLayout.sRotateRightButton.top,
                  kCharacterSelectButtonColor);
  DrawCharacterSelectButtonLabel(l_sLayout.sRotateLeftButton, "Rotate L", 60, 12, l_bLeftHovered, l_bLeftPressed);
  DrawCharacterSelectButtonLabel(l_sLayout.sRotateRightButton, "Rotate R", 110, 12, l_bRightHovered, l_bRightPressed);
}

void CGP_LogIn::RenderLoginMessageBox(void) const
{
  if (!IsLoginMessageVisible())
  {
    return;
  }

  RECT l_sBoxRect;
  RECT l_sOkButtonRect;
  if (!GetLoginMessageLayout(&l_sBoxRect, &l_sOkButtonRect))
  {
    return;
  }

  CSprite *l_pUiSprite = m_cLoginSpriteMgr.GetSprite(SP_ID_COMMON);
  const bool l_bOkPressed = m_bLoginMessageOkPressed && m_bLeftButtonDown;
  const int l_nBoxWidth = l_sBoxRect.right - l_sBoxRect.left;
  const int l_nBoxHeight = l_sBoxRect.bottom - l_sBoxRect.top;

  Draw2DRectangle(l_sBoxRect.left,
                  l_sBoxRect.top,
                  l_sBoxRect.right,
                  l_sBoxRect.bottom,
                  kMessageBoxPanelColor);

  Draw2DRectangle(l_sBoxRect.left + kMessageBoxOutlineMargin,
                  l_sBoxRect.top + kMessageBoxOutlineMargin,
                  l_sBoxRect.right - kMessageBoxOutlineMargin,
                  l_sBoxRect.top + kMessageBoxOutlineMargin + kMessageBoxOutlineSize,
                  kMessageBoxOutlineColor);
  Draw2DRectangle(l_sBoxRect.left + kMessageBoxOutlineMargin,
                  l_sBoxRect.bottom - kMessageBoxOutlineMargin - kMessageBoxOutlineSize,
                  l_sBoxRect.right - kMessageBoxOutlineMargin,
                  l_sBoxRect.bottom - kMessageBoxOutlineMargin,
                  kMessageBoxOutlineColor);
  Draw2DRectangle(l_sBoxRect.left + kMessageBoxOutlineMargin,
                  l_sBoxRect.top + kMessageBoxOutlineMargin,
                  l_sBoxRect.left + kMessageBoxOutlineMargin + kMessageBoxOutlineSize,
                  l_sBoxRect.bottom - kMessageBoxOutlineMargin,
                  kMessageBoxOutlineColor);
  Draw2DRectangle(l_sBoxRect.right - kMessageBoxOutlineMargin - kMessageBoxOutlineSize,
                  l_sBoxRect.top + kMessageBoxOutlineMargin,
                  l_sBoxRect.right - kMessageBoxOutlineMargin,
                  l_sBoxRect.bottom - kMessageBoxOutlineMargin,
                  kMessageBoxOutlineColor);

  int l_nMsgBoardWidth = 0;
  int l_nMsgBoardHeight = 0;
  GetLoginMessageBoardSize(m_szLoginMessage, &l_nMsgBoardWidth, &l_nMsgBoardHeight);
  const int l_nMsgLeft = l_sBoxRect.left + ((l_nBoxWidth - l_nMsgBoardWidth) / 2);
  const int l_nMsgTop = l_sBoxRect.top + ((l_nBoxHeight - l_nMsgBoardHeight) / 2);

  char l_szLine[128];
  int l_nLineStart = 0;
  int l_nLineIndex = 0;
  for (int i = 0;; ++i)
  {
    if (m_szLoginMessage[i] != '\n' && m_szLoginMessage[i] != '\0')
    {
      continue;
    }

    const int l_nLineLen = i - l_nLineStart;
    const int l_nCopyLen = l_nLineLen < static_cast<int>(sizeof(l_szLine) - 1)
                             ? l_nLineLen
                             : static_cast<int>(sizeof(l_szLine) - 1);
    memcpy(l_szLine, m_szLoginMessage + l_nLineStart, l_nCopyLen);
    l_szLine[l_nCopyLen] = '\0';
    DrawLoginLine(l_nMsgLeft + kMessageBoxTextMargin,
                  l_nMsgTop + kMessageBoxTextMargin + (l_nLineIndex * (kFontHeight + kMessageBoxColumnGap)),
                  l_szLine,
                  0xFFFFFFFF);
    ++l_nLineIndex;

    if (m_szLoginMessage[i] == '\0')
    {
      break;
    }
    l_nLineStart = i + 1;
  }

  const DWORD l_dwOkFrame = l_bOkPressed
                              ? kMessageBoxOkButtonPressedFrame
                              : (m_bLoginMessageOkHover
                                   ? kMessageBoxOkButtonHoverFrame
                                   : kMessageBoxOkButtonNormalFrame);
  if (l_pUiSprite)
  {
    DrawSpriteFrame(l_pUiSprite,
                    kMessageBoxOkButtonAction,
                    l_dwOkFrame,
                    l_sOkButtonRect.left,
                    l_sOkButtonRect.top,
                    l_sOkButtonRect.right - l_sOkButtonRect.left,
                    l_sOkButtonRect.bottom - l_sOkButtonRect.top,
                    0xFFFFFFFF);
  }
  else
  {
    Draw2DRectangle(l_sOkButtonRect.left,
                    l_sOkButtonRect.top,
                    l_sOkButtonRect.right,
                    l_sOkButtonRect.bottom,
                    l_bOkPressed ? 0xD0101820 : 0xC0202E38);
    Draw2DRectangle(l_sOkButtonRect.left,
                    l_sOkButtonRect.top,
                    l_sOkButtonRect.right,
                    l_sOkButtonRect.top + 1,
                    m_bLoginMessageOkHover ? 0xF0D0E0F0 : 0xD08EA0AE);
    Draw2DRectangle(l_sOkButtonRect.left,
                    l_sOkButtonRect.bottom - 1,
                    l_sOkButtonRect.right,
                    l_sOkButtonRect.bottom,
                    0xD0202830);
    Draw2DRectangle(l_sOkButtonRect.left,
                    l_sOkButtonRect.top,
                    l_sOkButtonRect.left + 1,
                    l_sOkButtonRect.bottom,
                    m_bLoginMessageOkHover ? 0xF0D0E0F0 : 0xD08EA0AE);
    Draw2DRectangle(l_sOkButtonRect.right - 1,
                    l_sOkButtonRect.top,
                    l_sOkButtonRect.right,
                    l_sOkButtonRect.bottom,
                    0xD0202830);
  }

  DrawLoginLine(GetCenteredTextX((l_sOkButtonRect.left + l_sOkButtonRect.right) / 2, "OK"),
                l_sOkButtonRect.top + ((kMessageBoxOkButtonHeight - kFontHeight) / 2),
                "OK",
                0xFFFFFFFF);
}
