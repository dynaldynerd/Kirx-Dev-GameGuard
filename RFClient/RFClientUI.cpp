#include "RFClientUI.h"

#include <cmath>
#include <cstdio>
#include <cstring>

#include "CPlayer.h"
#include "R3Engine/1stclass/core.h"
#include "R3Engine/1stclass/r3d3d8.h"
#include "R3Engine/2ndclass/2dsprite.h"
#include "R3Engine/2ndclass/r3text.h"

namespace
{
int ScaleUiCoord(int pi_nValue, float pi_fScale)
{
  return static_cast<int>(std::lround(static_cast<float>(pi_nValue) * pi_fScale));
}

void DrawSpriteFrame(CSprite *pi_pSprite,
                     DWORD pi_dwActionNo,
                     DWORD pi_dwFrameNo,
                     int pi_nX,
                     int pi_nY,
                     int pi_nWidth = -1,
                     int pi_nHeight = -1)
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

  pi_pSprite->DrawSprite(pi_nX, pi_nY, pi_nX + pi_nWidth, pi_nY + pi_nHeight);
}

void DrawSpriteFrameTiledX(CSprite *pi_pSprite,
                           DWORD pi_dwActionNo,
                           DWORD pi_dwFrameNo,
                           int pi_nX,
                           int pi_nY,
                           int pi_nWidth)
{
  if (!pi_pSprite || pi_nWidth <= 0)
  {
    return;
  }

  const int l_nTileWidth = static_cast<int>(pi_pSprite->GetSpriteOrgXL(pi_dwActionNo, pi_dwFrameNo));
  const int l_nTileHeight = static_cast<int>(pi_pSprite->GetSpriteOrgYL(pi_dwActionNo, pi_dwFrameNo));
  if (l_nTileWidth <= 0 || l_nTileHeight <= 0)
  {
    return;
  }

  int l_nDrawX = pi_nX;
  int l_nRemainWidth = pi_nWidth;
  while (l_nRemainWidth > 0)
  {
    const int l_nDrawWidth = (l_nRemainWidth < l_nTileWidth) ? l_nRemainWidth : l_nTileWidth;
    DrawSpriteFrame(pi_pSprite,
                    pi_dwActionNo,
                    pi_dwFrameNo,
                    l_nDrawX,
                    pi_nY,
                    l_nDrawWidth,
                    l_nTileHeight);
    l_nDrawX += l_nDrawWidth;
    l_nRemainWidth -= l_nDrawWidth;
  }
}

void DrawTextLine(int pi_nX, int pi_nY, DWORD pi_dwColor, const char *pi_pText)
{
  if (!pi_pText || !pi_pText[0])
  {
    return;
  }

  DrawR3Hangul(pi_nX, pi_nY, const_cast<char *>(pi_pText), pi_dwColor, R3_HAN_OUTLINE);
}
}

CRFClientUI::CRFClientUI()
  : m_bInitialized(false)
{
}

CRFClientUI::~CRFClientUI()
{
  Shutdown();
}

bool CRFClientUI::Initialize(void)
{
  if (m_bInitialized)
  {
    return true;
  }

  m_cSpriteMgr.Init();
  m_cSpriteMgr.LoadSprite(SP_ID_COMMON);
  m_cSpriteMgr.LoadSprite(SP_ID_GAUGE_EXP);
  m_cSpriteMgr.LoadSprite(SP_ID_BELT);
  m_cSpriteMgr.LoadSprite(SP_ID_UNIT);

  m_bInitialized = m_cSpriteMgr.GetSprite(SP_ID_GAUGE_EXP) != NULL &&
                   m_cSpriteMgr.GetSprite(SP_ID_BELT) != NULL;
  return m_bInitialized;
}

void CRFClientUI::Shutdown(void)
{
  m_cSpriteMgr.UnloadSpriteAll();
  m_bInitialized = false;
}

void CRFClientUI::Render(CLand &pi_rLand,
                         const CPlayer *pi_pPlayer,
                         bool pi_bKeyboardMoveMode,
                         float pi_fCameraAngleY)
{
  if (!m_bInitialized)
  {
    return;
  }

  RenderGaugeExpWindow(pi_rLand, pi_pPlayer, pi_bKeyboardMoveMode);
  RenderRadarWindow(pi_rLand, pi_pPlayer, pi_fCameraAngleY);
  RenderHotKeyBoard(pi_pPlayer, pi_bKeyboardMoveMode);
}

void CRFClientUI::RenderGaugeExpWindow(const CLand &pi_rLand,
                                       const CPlayer *pi_pPlayer,
                                       bool pi_bKeyboardMoveMode) const
{
  CSprite *l_pMainSprite = m_cSpriteMgr.GetSprite(SP_ID_GAUGE_EXP);
  if (!l_pMainSprite)
  {
    return;
  }

  const int l_nPosX = 8;
  const int l_nPosY = 8;
  DrawSpriteFrame(l_pMainSprite, 0, 0, l_nPosX, l_nPosY);

  char l_szName[64];
  strcpy_s(l_szName, sizeof(l_szName), (pi_pPlayer && pi_pPlayer->GetName()[0]) ? pi_pPlayer->GetName() : "Player");

  char l_szMode[64];
  sprintf_s(l_szMode,
            sizeof(l_szMode),
            "%s / %s",
            pi_bKeyboardMoveMode ? "Keyboard" : "Mouse",
            (pi_pPlayer && pi_pPlayer->GetWalkMode() == CMM_MOVE_WALK) ? "Walk" : "Run");

  const MAP_INFO *l_pMapInfo = pi_rLand.GetCurMapInfo();
  char l_szMapName[96];
  l_szMapName[0] = '\0';
  if (l_pMapInfo)
  {
    const char *l_pMapName = l_pMapInfo->pName_UI[0] ? l_pMapInfo->pName_UI : l_pMapInfo->pName;
    strcpy_s(l_szMapName, sizeof(l_szMapName), l_pMapName);
  }

  DrawTextLine(l_nPosX + 232, l_nPosY + 8, 0xFFFFFFFF, l_szName);
  DrawTextLine(l_nPosX + 232, l_nPosY + 26, 0xFFD0D0D0, l_szMode);
  DrawTextLine(l_nPosX + 232, l_nPosY + 44, 0xFFFFE090, l_szMapName);
}

void CRFClientUI::RenderHotKeyBoard(const CPlayer *pi_pPlayer,
                                    bool pi_bKeyboardMoveMode) const
{
  CSprite *l_pMainSprite = m_cSpriteMgr.GetSprite(SP_ID_BELT);
  if (!l_pMainSprite)
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

  const int l_nNativeBoardWidth = static_cast<int>(l_pMainSprite->GetSpriteOrgXL(11, 0));
  const int l_nBoardHeight = static_cast<int>(l_pMainSprite->GetSpriteOrgYL(11, 0));
  const int l_nBoardWidth = (l_nScreenX < l_nNativeBoardWidth) ? l_nScreenX : l_nNativeBoardWidth;
  const int l_nBoardX = (l_nScreenX - l_nBoardWidth) / 2;
  const int l_nBoardY = l_nScreenY - l_nBoardHeight;
  const float l_fBoardScale = static_cast<float>(l_nBoardWidth) / static_cast<float>(l_nNativeBoardWidth);

  DrawSpriteFrameTiledX(l_pMainSprite, 11, 5, 0, l_nBoardY, l_nScreenX);
  DrawSpriteFrame(l_pMainSprite, 11, 0, l_nBoardX, l_nBoardY, l_nBoardWidth, l_nBoardHeight);

  const int l_nIconSize = ScaleUiCoord(32, l_fBoardScale);
  const int l_nIconY = l_nBoardY + ScaleUiCoord(19, l_fBoardScale);
  const int l_nLeftGroupX = l_nBoardX + ScaleUiCoord(169, l_fBoardScale);
  const int l_nRightGroupX = l_nBoardX + ScaleUiCoord(757, l_fBoardScale);

  DWORD l_dwActionFrameNo = 0;
  DWORD l_dwMoveFrameNo = 3;
  DWORD l_dwAttackFrameNo = 5;

  if (pi_pPlayer && pi_pPlayer->GetWalkMode() == CMM_MOVE_WALK)
  {
    l_dwMoveFrameNo = 2;
  }

  if (!pi_bKeyboardMoveMode)
  {
    l_dwActionFrameNo = 1;
  }

  DrawSpriteFrame(l_pMainSprite,
                  12,
                  l_dwActionFrameNo,
                  l_nLeftGroupX,
                  l_nIconY,
                  l_nIconSize,
                  l_nIconSize);
  DrawSpriteFrame(l_pMainSprite,
                  12,
                  l_dwMoveFrameNo,
                  l_nLeftGroupX + l_nIconSize,
                  l_nIconY,
                  l_nIconSize,
                  l_nIconSize);
  DrawSpriteFrame(l_pMainSprite,
                  12,
                  l_dwAttackFrameNo,
                  l_nLeftGroupX + (l_nIconSize * 2),
                  l_nIconY,
                  l_nIconSize,
                  l_nIconSize);

  DrawSpriteFrame(l_pMainSprite,
                  13,
                  12,
                  l_nRightGroupX,
                  l_nIconY,
                  l_nIconSize,
                  l_nIconSize);
  DrawSpriteFrame(l_pMainSprite,
                  13,
                  1,
                  l_nRightGroupX + l_nIconSize,
                  l_nIconY,
                  l_nIconSize,
                  l_nIconSize);
  DrawSpriteFrame(l_pMainSprite,
                  13,
                  2,
                  l_nRightGroupX + (l_nIconSize * 2),
                  l_nIconY,
                  l_nIconSize,
                  l_nIconSize);
}

void CRFClientUI::RenderRadarWindow(const CLand &pi_rLand,
                                    const CPlayer *pi_pPlayer,
                                    float pi_fCameraAngleY) const
{
  CSprite *l_pMainSprite = m_cSpriteMgr.GetSprite(SP_ID_BELT);
  if (!l_pMainSprite)
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

  const int l_nPosX = l_nScreenX - 154;
  const int l_nPosY = 8;
  DrawSpriteFrame(l_pMainSprite, 3, 0, l_nPosX, l_nPosY);

  const MAP_INFO *l_pMapInfo = pi_rLand.GetCurMapInfo();
  char l_szMapName[96];
  l_szMapName[0] = '\0';
  if (l_pMapInfo)
  {
    const char *l_pName = l_pMapInfo->pName_UI[0] ? l_pMapInfo->pName_UI : l_pMapInfo->pName;
    strcpy_s(l_szMapName, sizeof(l_szMapName), l_pName);
  }
  DrawTextLine(l_nPosX + 26, l_nPosY + 2, 0xFFFFFFFF, l_szMapName);

  const float l_fCenterX = static_cast<float>(l_nPosX + 73);
  const float l_fCenterY = static_cast<float>(l_nPosY + 70);
  const float l_fHeading = (360.0f - pi_fCameraAngleY) * (3.1415926535f / 180.0f);
  const float l_fArrowLength = 21.0f;

  Vector2f l_vecArrow[2];
  l_vecArrow[0][0] = l_fCenterX;
  l_vecArrow[0][1] = l_fCenterY;
  l_vecArrow[1][0] = l_fCenterX + (sinf(l_fHeading) * l_fArrowLength);
  l_vecArrow[1][1] = l_fCenterY - (cosf(l_fHeading) * l_fArrowLength);
  Draw2DLine(l_vecArrow[0], l_vecArrow[1], 0xFFFFE060);
  Draw2DRectangle(static_cast<LONG>(l_fCenterX - 2.0f),
                  static_cast<LONG>(l_fCenterY - 2.0f),
                  static_cast<LONG>(l_fCenterX + 2.0f),
                  static_cast<LONG>(l_fCenterY + 2.0f),
                  0xFFFFE060);

  if (pi_pPlayer)
  {
    Vector3f l_vecPlayerPos;
    pi_pPlayer->GetPosition(l_vecPlayerPos);

    char l_szPosition[64];
    sprintf_s(l_szPosition,
              sizeof(l_szPosition),
              "%5d  %5d",
              static_cast<int>(l_vecPlayerPos[0]),
              static_cast<int>(l_vecPlayerPos[2]));
    DrawTextLine(l_nPosX + 45, l_nPosY + 137, 0xFFD6D6D6, l_szPosition);
  }
}

void CRFClientUI::GetScreenSize(int *po_pScreenX, int *po_pScreenY)
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
