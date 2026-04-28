#include "Network/CNetworkMgr.h"
#include "RFClientApp.h"

#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <vector>

#include "CGP_LogIn.h"
#include "CGP_MainGame.h"
#include "CGP_Title.h"
#include "CGameProgress.h"
#include "CPlayer.h"
#include "UIGlobal.h"
#include "R3Engine/1stclass/core.h"
#include "R3Engine/1stclass/r3d3d8.h"
#include "R3Engine/common/commonutil.h"
#include "R3Engine/common/r3input.h"
#include "R3Engine/common/r3math.h"
#include "R3Engine/2ndclass/r3enginekernel.h"
#include "R3Engine/2ndclass/r3move.h"
#include "R3Engine/2ndclass/r3sound.h"
#include "R3Engine/2ndclass/r3text.h"
#include "R3Engine/2ndclass/r3util.h"
#include "R3Engine/resource.h"

namespace
{
constexpr float kMinViewAngleX = -89.0f;
constexpr float kMaxViewAngleX = 89.0f;
constexpr float kMinCameraDistance = 10.0f;
constexpr float kMaxCameraDistance = 60.0f;
constexpr float kMinMapFarClipDistance = 6000.0f;
constexpr float kMaxMapFarClipDistance = 50000.0f;
constexpr float kWalkMoveSpeed = 220.0f;
constexpr float kRunMoveSpeed = 550.0f;
constexpr float kDefaultCameraDistance = 45.0f;
constexpr float kMouseWheelDistanceRate = 0.07f;
constexpr char kDebugCapturePath[] = ".\\RFClientCapture.bmp";
constexpr char kDebugCaptureLogPath[] = ".\\RFClientCapture.log";
constexpr DWORD kDefaultSetWorldIpXor = 0xCB9C4B3A;
constexpr WORD kDefaultSetWorldPortXor = 0x4FB6;
constexpr DWORD kDefaultSetAccountSerialXor = 0x6E65E0AF;
constexpr WORD kDefaultSetZeroFieldValue = 0x4B3A;
constexpr DWORD kDefaultSetBillingAccountSerialXor = 0xC89C183A;
constexpr DWORD kDefaultSetBillingTypeXor = 0xC89C183A;
constexpr DWORD kDefaultSetIsAdultXor = 0xD29C283B;
constexpr WORD kDefaultSetLanguageIndexXor = 0x32D7;
constexpr DWORD kDefaultSetWorldMasterKeyXor[KEY_NUM] = {0xCFCF22E6, 0x5BBCDE6F, 0xACDF5EDA, 0xBCCD1B37};
constexpr float kLoginLobbyCharacterTopPos[3][3] =
{
  {0.200f, 225.0f, -64.0f},
  {56.0f, 225.0f, 30.0f},
  {-56.0f, 225.0f, 30.0f}
};

float ClampFloat(float value, float minimum, float maximum)
{
  if (value < minimum)
  {
    return minimum;
  }

  if (value > maximum)
  {
    return maximum;
  }

  return value;
}

float MaxFloat(float left, float right)
{
  return left > right ? left : right;
}

float NormalizeAngle360(float value)
{
  while (value >= 360.0f)
  {
    value -= 360.0f;
  }

  while (value < 0.0f)
  {
    value += 360.0f;
  }

  return value;
}

float GetMoveSpeed(BYTE pi_byMoveMode)
{
  return (pi_byMoveMode == CMM_MOVE_RUN) ? kRunMoveSpeed : kWalkMoveSpeed;
}

bool IsShortcutModifierDown()
{
  return (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0 ||
         (GetAsyncKeyState(VK_MENU) & 0x8000) != 0 ||
         (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
}

bool IsKeyboardMessage(UINT pi_uMsg)
{
  return pi_uMsg == WM_KEYDOWN ||
         pi_uMsg == WM_KEYUP ||
         pi_uMsg == WM_SYSKEYDOWN ||
         pi_uMsg == WM_SYSKEYUP ||
         pi_uMsg == WM_CHAR ||
         pi_uMsg == WM_SYSCHAR ||
         pi_uMsg == WM_DEADCHAR ||
         pi_uMsg == WM_SYSDEADCHAR;
}

bool IsAcceleratorExitCommand(UINT pi_uMsg, WPARAM pi_wParam)
{
  return pi_uMsg == WM_COMMAND &&
         LOWORD(pi_wParam) == IDM_EXIT &&
         HIWORD(pi_wParam) == 1;
}

void HideWindowMenuBar(HWND pi_hWnd, DWORD pi_dwWindowStyle, int pi_nClientWidth, int pi_nClientHeight)
{
  if (!pi_hWnd || pi_nClientWidth <= 0 || pi_nClientHeight <= 0)
  {
    return;
  }

  if (GetMenu(pi_hWnd))
  {
    SetMenu(pi_hWnd, NULL);
  }

  RECT l_rcWindow = {0, 0, pi_nClientWidth, pi_nClientHeight};
  AdjustWindowRect(&l_rcWindow, pi_dwWindowStyle, FALSE);
  SetWindowPos(pi_hWnd,
               NULL,
               0,
               0,
               l_rcWindow.right - l_rcWindow.left,
               l_rcWindow.bottom - l_rcWindow.top,
               SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

bool GetMouseModeMoveRotY(bool pi_bMoveForward,
                          bool pi_bMoveBackward,
                          bool pi_bMoveLeft,
                          bool pi_bMoveRight,
                          float pi_fCameraPitch,
                          float &po_fRotY)
{
  if (pi_bMoveLeft && pi_bMoveRight)
  {
    return false;
  }

  if (pi_bMoveForward && pi_bMoveBackward)
  {
    return false;
  }

  if (pi_bMoveLeft && pi_bMoveForward)
  {
    po_fRotY = 360.0f - pi_fCameraPitch - 45.0f;
  }
  else if (pi_bMoveRight && pi_bMoveForward)
  {
    po_fRotY = 360.0f - pi_fCameraPitch + 45.0f;
  }
  else if (pi_bMoveLeft && pi_bMoveBackward)
  {
    po_fRotY = 360.0f - pi_fCameraPitch - 135.0f;
  }
  else if (pi_bMoveRight && pi_bMoveBackward)
  {
    po_fRotY = 360.0f - pi_fCameraPitch + 135.0f;
  }
  else if (pi_bMoveForward)
  {
    po_fRotY = 360.0f - pi_fCameraPitch;
  }
  else if (pi_bMoveBackward)
  {
    po_fRotY = 180.0f - pi_fCameraPitch;
  }
  else if (pi_bMoveLeft)
  {
    po_fRotY = 360.0f - pi_fCameraPitch - 90.0f;
  }
  else if (pi_bMoveRight)
  {
    po_fRotY = 360.0f - pi_fCameraPitch + 90.0f;
  }
  else
  {
    return false;
  }

  po_fRotY = NormalizeAngle360(po_fRotY);
  return true;
}

bool GetKeyboardModeMoveRotY(bool pi_bMoveForward,
                             bool pi_bMoveBackward,
                             bool pi_bMoveLeft,
                             bool pi_bMoveRight,
                             float pi_fCameraPitch,
                             float &po_fFacingRotY,
                             float &po_fMoveRotY)
{
  if (pi_bMoveLeft && pi_bMoveRight)
  {
    pi_bMoveLeft = false;
    pi_bMoveRight = false;
  }

  if (pi_bMoveForward && pi_bMoveBackward)
  {
    pi_bMoveForward = false;
    pi_bMoveBackward = false;
  }

  if (!pi_bMoveForward && !pi_bMoveBackward && !pi_bMoveLeft && !pi_bMoveRight)
  {
    return false;
  }

  po_fFacingRotY = NormalizeAngle360(360.0f - pi_fCameraPitch);

  if (pi_bMoveForward && pi_bMoveLeft)
  {
    po_fMoveRotY = po_fFacingRotY - 45.0f;
    po_fFacingRotY = NormalizeAngle360(po_fMoveRotY);
  }
  else if (pi_bMoveForward && pi_bMoveRight)
  {
    po_fMoveRotY = po_fFacingRotY + 45.0f;
    po_fFacingRotY = NormalizeAngle360(po_fMoveRotY);
  }
  else if (pi_bMoveBackward && pi_bMoveLeft)
  {
    po_fMoveRotY = po_fFacingRotY - 135.0f;
    po_fFacingRotY = NormalizeAngle360(po_fFacingRotY + 45.0f);
  }
  else if (pi_bMoveBackward && pi_bMoveRight)
  {
    po_fMoveRotY = po_fFacingRotY + 135.0f;
    po_fFacingRotY = NormalizeAngle360(po_fFacingRotY - 45.0f);
  }
  else if (pi_bMoveForward)
  {
    po_fMoveRotY = po_fFacingRotY;
  }
  else if (pi_bMoveBackward)
  {
    po_fMoveRotY = po_fFacingRotY + 180.0f;
  }
  else if (pi_bMoveLeft)
  {
    po_fMoveRotY = po_fFacingRotY - 90.0f;
  }
  else
  {
    po_fMoveRotY = po_fFacingRotY + 90.0f;
  }

  po_fMoveRotY = NormalizeAngle360(po_fMoveRotY);
  return true;
}

void PrepareShadow(float scale, float add_pos)
{
  LPDIRECT3DDEVICE8 pDevice = GetD3dDevice();
  _R3ENGINE_STATE *state = GetR3State();
  if (!pDevice || !state)
  {
    return;
  }

  D3DXMATRIX proj;
  D3DXMATRIX view;
  D3DXMATRIX rot;
  const float dg = 66.0f * 32.0f * scale;

  D3DXVECTOR3 vEyePt(-0.5f, dg * 5.0f, dg * 3.0f - add_pos + 0.5f);
  D3DXVECTOR3 vLookatPt(-0.5f, 0.0f, -add_pos + 0.5f);
  D3DXVECTOR4 out;
  D3DXVECTOR4 out1;
  D3DXMatrixRotationY(&rot, state->mMainLightYAngle);

  D3DXVec3Transform(&out1, &vLookatPt, &rot);
  D3DXVec3Transform(&out, &vEyePt, &rot);

  const D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
  D3DXMatrixLookAtLH(&view, reinterpret_cast<D3DXVECTOR3 *>(&out), reinterpret_cast<D3DXVECTOR3 *>(&out1), &vUpVec);
  pDevice->SetTransform(D3DTS_VIEW, &view);

  D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 1024.0f, 1.0f, 4.0f, 400000.0f);
  pDevice->SetTransform(D3DTS_PROJECTION, &proj);
  pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
  pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
}

bool IsPathLike(const char *value)
{
  return value &&
         (strchr(value, '\\') != nullptr || strchr(value, '/') != nullptr || strstr(value, ".bsp") != nullptr);
}

MAP_INFO *FindMapInfoBySelection(CLand &pi_rLand, const char *pi_pMapSelection)
{
  if (!pi_pMapSelection || !pi_pMapSelection[0])
  {
    return NULL;
  }

  for (int i = 0; i < pi_rLand.GetMaxMapNum(); ++i)
  {
    MAP_INFO *l_pMapInfo = pi_rLand.GetMapInfo(static_cast<BYTE>(i));
    if (!l_pMapInfo)
    {
      continue;
    }

    if (_stricmp(l_pMapInfo->pName, pi_pMapSelection) == 0)
    {
      return l_pMapInfo;
    }

    char l_szMapFileName[MAX_PATH];
    strcpy_s(l_szMapFileName, sizeof(l_szMapFileName), l_pMapInfo->pFileName);
    StripEXT(l_szMapFileName);
    StripPath(l_szMapFileName);
    if (_stricmp(l_szMapFileName, pi_pMapSelection) == 0)
    {
      return l_pMapInfo;
    }
  }

  return NULL;
}

void CopyMapSelection(char *destination, size_t destinationSize, LPCSTR commandLine)
{
  if (!destination || !destinationSize)
  {
    return;
  }

  destination[0] = '\0';
  if (!commandLine)
  {
    return;
  }

  while (*commandLine == ' ' || *commandLine == '\t')
  {
    ++commandLine;
  }

  if (*commandLine == '\"')
  {
    ++commandLine;
    const char *endQuote = strchr(commandLine, '\"');
    if (endQuote)
    {
      const size_t mapNameLength = static_cast<size_t>(endQuote - commandLine);
      strncpy_s(destination, destinationSize, commandLine, mapNameLength);
    }
  }
  else if (*commandLine)
  {
    const char *cursor = commandLine;
    while (*cursor && *cursor != ' ' && *cursor != '\t')
    {
      ++cursor;
    }

    const size_t mapNameLength = static_cast<size_t>(cursor - commandLine);
    strncpy_s(destination, destinationSize, commandLine, mapNameLength);
  }

}

bool ExtractCommandOptionValue(LPCSTR pi_pCommandLine,
                               const char *pi_pOptionName,
                               char *po_pValue,
                               size_t pi_stValueSize)
{
  if (!pi_pCommandLine || !pi_pOptionName || !po_pValue || !pi_stValueSize)
  {
    return false;
  }

  po_pValue[0] = '\0';
  const char *l_pOption = strstr(pi_pCommandLine, pi_pOptionName);
  if (!l_pOption)
  {
    return false;
  }

  const char *l_pCursor = l_pOption + strlen(pi_pOptionName);
  while (*l_pCursor == ' ' || *l_pCursor == '\t' || *l_pCursor == '=')
  {
    ++l_pCursor;
  }

  if (!*l_pCursor)
  {
    return true;
  }

  if (*l_pCursor == '\"')
  {
    ++l_pCursor;
    const char *l_pEndQuote = strchr(l_pCursor, '\"');
    const size_t l_stValueLength = l_pEndQuote
                                     ? static_cast<size_t>(l_pEndQuote - l_pCursor)
                                     : strlen(l_pCursor);
    strncpy_s(po_pValue, pi_stValueSize, l_pCursor, l_stValueLength);
    return true;
  }

  const char *l_pEnd = l_pCursor;
  while (*l_pEnd && *l_pEnd != ' ' && *l_pEnd != '\t')
  {
    ++l_pEnd;
  }

  strncpy_s(po_pValue, pi_stValueSize, l_pCursor, static_cast<size_t>(l_pEnd - l_pCursor));
  return true;
}

bool SaveSurfaceToBMP(const char *fileName, LPDIRECT3DSURFACE8 surface)
{
  if (!fileName || !surface)
  {
    return false;
  }

  D3DSURFACE_DESC surfaceDesc;
  if (FAILED(surface->GetDesc(&surfaceDesc)))
  {
    return false;
  }

  D3DLOCKED_RECT lockedRect;
  if (FAILED(surface->LockRect(&lockedRect, NULL, D3DLOCK_READONLY)))
  {
    return false;
  }

  const DWORD imageSize = surfaceDesc.Width * surfaceDesc.Height * 4;

  BITMAPFILEHEADER fileHeader;
  ZeroMemory(&fileHeader, sizeof(fileHeader));
  fileHeader.bfType = 0x4D42;
  fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
  fileHeader.bfSize = fileHeader.bfOffBits + imageSize;

  BITMAPINFOHEADER infoHeader;
  ZeroMemory(&infoHeader, sizeof(infoHeader));
  infoHeader.biSize = sizeof(BITMAPINFOHEADER);
  infoHeader.biWidth = static_cast<LONG>(surfaceDesc.Width);
  infoHeader.biHeight = -static_cast<LONG>(surfaceDesc.Height);
  infoHeader.biPlanes = 1;
  infoHeader.biBitCount = 32;
  infoHeader.biCompression = BI_RGB;
  infoHeader.biSizeImage = imageSize;

  HANDLE outputFile = CreateFileA(fileName,
                                  GENERIC_WRITE,
                                  0,
                                  NULL,
                                  CREATE_ALWAYS,
                                  FILE_ATTRIBUTE_NORMAL,
                                  NULL);
  if (outputFile == INVALID_HANDLE_VALUE)
  {
    surface->UnlockRect();
    return false;
  }

  DWORD written = 0;
  bool saved = WriteFile(outputFile, &fileHeader, sizeof(fileHeader), &written, NULL) != FALSE &&
               WriteFile(outputFile, &infoHeader, sizeof(infoHeader), &written, NULL) != FALSE;

  std::vector<BYTE> rowBuffer(surfaceDesc.Width * 4);
  for (UINT y = 0; saved && y < surfaceDesc.Height; ++y)
  {
    const BYTE *sourceRow = static_cast<const BYTE *>(lockedRect.pBits) + (y * lockedRect.Pitch);
    BYTE *destRow = rowBuffer.data();

    for (UINT x = 0; x < surfaceDesc.Width; ++x)
    {
      switch (surfaceDesc.Format)
      {
        case D3DFMT_A8R8G8B8:
        case D3DFMT_X8R8G8B8:
        {
          const DWORD color = reinterpret_cast<const DWORD *>(sourceRow)[x];
          destRow[(x * 4) + 0] = static_cast<BYTE>(color & 0xFF);
          destRow[(x * 4) + 1] = static_cast<BYTE>((color >> 8) & 0xFF);
          destRow[(x * 4) + 2] = static_cast<BYTE>((color >> 16) & 0xFF);
          destRow[(x * 4) + 3] = 0x00;
          break;
        }

        case D3DFMT_R5G6B5:
        {
          const WORD color = reinterpret_cast<const WORD *>(sourceRow)[x];
          destRow[(x * 4) + 0] = static_cast<BYTE>((color & 0x1F) << 3);
          destRow[(x * 4) + 1] = static_cast<BYTE>(((color >> 5) & 0x3F) << 2);
          destRow[(x * 4) + 2] = static_cast<BYTE>(((color >> 11) & 0x1F) << 3);
          destRow[(x * 4) + 3] = 0x00;
          break;
        }

        case D3DFMT_X1R5G5B5:
        case D3DFMT_A1R5G5B5:
        {
          const WORD color = reinterpret_cast<const WORD *>(sourceRow)[x];
          destRow[(x * 4) + 0] = static_cast<BYTE>((color & 0x1F) << 3);
          destRow[(x * 4) + 1] = static_cast<BYTE>(((color >> 5) & 0x1F) << 3);
          destRow[(x * 4) + 2] = static_cast<BYTE>(((color >> 10) & 0x1F) << 3);
          destRow[(x * 4) + 3] = 0x00;
          break;
        }

        default:
          saved = false;
          break;
      }
    }

    if (!saved)
    {
      break;
    }

    saved = WriteFile(outputFile, rowBuffer.data(), static_cast<DWORD>(rowBuffer.size()), &written, NULL) != FALSE;
  }

  CloseHandle(outputFile);
  surface->UnlockRect();
  return saved;
}

bool IsValidGroundHeight(float value)
{
  return value != 0.0f && value != 65535.0f && value != -65535.0f;
}

bool SampleGroundHeight(CLevel &level, float x, float z, float probeY, float &outY)
{
  float l_vecProbe[3];
  l_vecProbe[0] = x;
  l_vecProbe[1] = probeY;
  l_vecProbe[2] = z;

  outY = level.GetFirstYpos(l_vecProbe, 0);
  return IsValidGroundHeight(outY);
}

bool FindCharacterSpawnPos(CLevel &level, const _BSP_NODE &worldNode, float po_vecSpawnPos[3], float &po_fFlatness)
{
  const float centerX = (static_cast<float>(worldNode.bb_min[0]) + static_cast<float>(worldNode.bb_max[0])) * 0.5f;
  const float centerZ = (static_cast<float>(worldNode.bb_min[2]) + static_cast<float>(worldNode.bb_max[2])) * 0.5f;
  const float probeY = static_cast<float>(worldNode.bb_max[1]) + 500.0f;
  const float sampleOffset = 80.0f;
  const float ringStep = 160.0f;
  const float maxRadius = 1600.0f;
  const float directions[][2] =
  {
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {-1.0f, 0.0f},
    {0.0f, 1.0f},
    {0.0f, -1.0f},
    {0.70710677f, 0.70710677f},
    {0.70710677f, -0.70710677f},
    {-0.70710677f, 0.70710677f},
    {-0.70710677f, -0.70710677f}
  };

  bool l_bFound = false;
  float l_fBestFlatness = 1000000.0f;
  float l_vecBestPos[3] = {centerX, probeY, centerZ};

  for (float radius = 0.0f; radius <= maxRadius; radius += ringStep)
  {
    for (int directionIndex = 0; directionIndex < static_cast<int>(sizeof(directions) / sizeof(directions[0])); ++directionIndex)
    {
      const float candidateX = centerX + (directions[directionIndex][0] * radius);
      const float candidateZ = centerZ + (directions[directionIndex][1] * radius);
      float centerY = 0.0f;
      if (!SampleGroundHeight(level, candidateX, candidateZ, probeY, centerY))
      {
        continue;
      }

      float neighborHeights[4];
      if (!SampleGroundHeight(level, candidateX + sampleOffset, candidateZ, probeY, neighborHeights[0]) ||
          !SampleGroundHeight(level, candidateX - sampleOffset, candidateZ, probeY, neighborHeights[1]) ||
          !SampleGroundHeight(level, candidateX, candidateZ + sampleOffset, probeY, neighborHeights[2]) ||
          !SampleGroundHeight(level, candidateX, candidateZ - sampleOffset, probeY, neighborHeights[3]))
      {
        continue;
      }

      float minHeight = centerY;
      float maxHeight = centerY;
      for (int i = 0; i < 4; ++i)
      {
        if (neighborHeights[i] < minHeight)
        {
          minHeight = neighborHeights[i];
        }
        if (neighborHeights[i] > maxHeight)
        {
          maxHeight = neighborHeights[i];
        }
      }

      const float flatness = maxHeight - minHeight;
      if (!l_bFound || flatness < l_fBestFlatness)
      {
        l_bFound = true;
        l_fBestFlatness = flatness;
        l_vecBestPos[0] = candidateX;
        l_vecBestPos[1] = centerY;
        l_vecBestPos[2] = candidateZ;
      }

      if (flatness <= 12.0f)
      {
        po_vecSpawnPos[0] = candidateX;
        po_vecSpawnPos[1] = centerY;
        po_vecSpawnPos[2] = candidateZ;
        po_fFlatness = flatness;
        return true;
      }
    }
  }

  if (!l_bFound)
  {
    return false;
  }

  po_vecSpawnPos[0] = l_vecBestPos[0];
  po_vecSpawnPos[1] = l_vecBestPos[1];
  po_vecSpawnPos[2] = l_vecBestPos[2];
  po_fFlatness = l_fBestFlatness;
  return true;
}

#if defined(_DEBUG)
void AppendCaptureLog(const char *format, ...)
{
  HANDLE outputFile = CreateFileA(kDebugCaptureLogPath,
                                  FILE_APPEND_DATA,
                                  FILE_SHARE_READ,
                                  NULL,
                                  OPEN_ALWAYS,
                                  FILE_ATTRIBUTE_NORMAL,
                                  NULL);
  if (outputFile == INVALID_HANDLE_VALUE)
  {
    return;
  }

  char buffer[1024];
  va_list args;
  va_start(args, format);
  int writtenLength = _vsnprintf(buffer, sizeof(buffer) - 3, format, args);
  va_end(args);

  if (writtenLength < 0)
  {
    writtenLength = static_cast<int>(sizeof(buffer) - 3);
  }
  buffer[writtenLength++] = '\r';
  buffer[writtenLength++] = '\n';
  buffer[writtenLength] = '\0';

  DWORD bytesWritten = 0;
  WriteFile(outputFile, buffer, static_cast<DWORD>(writtenLength), &bytesWritten, NULL);
  CloseHandle(outputFile);
}
#endif
}

CMainApp *g_pMainApp = NULL;

CMainApp::CMainApp()
  : m_pNetworkMgr(NULL),
    m_pPlayer(NULL),
    m_pGameProgress(NULL),
    m_isInitialized(false),
    m_isMapLoaded(false),
    m_bIsActive(FALSE),
    m_fVersion(0.738f),
    m_byGameProgressID(GPI_TITLE),
    m_bCreatedAllObject(FALSE),
    m_bLoadedAllData(FALSE),
    m_bRequestQuitProgram(FALSE),
    m_dwRequestQuitTime(0),
    m_dwQuitWaitingTime(static_cast<DWORD>(-1)),
    m_bIsQuit(FALSE),
    m_bClientWindowVisible(TRUE),
    m_bOfflineRegedCharReplay(FALSE),
    m_bLoginLobbyCameraAnimating(FALSE),
    m_fViewAngleX(35.0f),
    m_fViewAngleY(225.0f),
    m_fPrevViewAngleX(35.0f),
    m_fPrevViewAngleY(225.0f),
    m_fPointDist(600.0f),
    m_mapFarClipDistance(kMinMapFarClipDistance),
    m_hasSpawnCameraTarget(false),
    m_bKeyboardMoveMode(false),
    m_isRightMouseDragging(false),
    m_nPrevX(0),
    m_nPrevY(0),
    m_pendingWheelDelta(0)
{
  m_mapName[0] = '\0';
  m_szSelectedMap[0] = '\0';
  m_szOfflineRegedCharPath[0] = '\0';
  m_vecLoginLobbyCameraPos[0] = 0.0f;
  m_vecLoginLobbyCameraPos[1] = 0.0f;
  m_vecLoginLobbyCameraPos[2] = 0.0f;
  m_cameraTarget[0] = 0.0f;
  m_cameraTarget[1] = 0.0f;
  m_cameraTarget[2] = 0.0f;
  m_cameraPosition[0] = 0.0f;
  m_cameraPosition[1] = 0.0f;
  m_cameraPosition[2] = 0.0f;
  m_spawnCameraTarget[0] = 0.0f;
  m_spawnCameraTarget[1] = 0.0f;
  m_spawnCameraTarget[2] = 0.0f;
  m_strWindowTitle = const_cast<char *>("RF_Online");
  g_pMainApp = this;
}

CMainApp::~CMainApp()
{
#if defined(_DEBUG)
  AppendCaptureLog("~CMainApp: begin");
#endif
  Shutdown();
#if defined(_DEBUG)
  AppendCaptureLog("~CMainApp: end");
#endif
}

bool CMainApp::Initialize(HINSTANCE hInstance, LPCSTR commandLine)
{
  if (m_isInitialized)
  {
    return true;
  }

#if defined(_DEBUG)
  AppendCaptureLog("Initialize: begin commandLine=%s", commandLine ? commandLine : "<null>");
#endif
  InitR3Engine();
  InitR3SoundSystem(".\\snd\\WaveList.spt");
  _R3ENGINE_STATE *l_pState = GetR3State();

  if (FAILED(InitR3D3D(hInstance, 0)))
  {
#if defined(_DEBUG)
    AppendCaptureLog("Initialize: InitR3D3D failed");
#endif
    ReleaseR3SoundSystem();
    ReleaseR3Engine();
    return false;
  }

  RECT l_rcClient = {};
  int l_nClientWidth = 0;
  int l_nClientHeight = 0;
  if (m_hWnd && GetClientRect(m_hWnd, &l_rcClient))
  {
    l_nClientWidth = l_rcClient.right - l_rcClient.left;
    l_nClientHeight = l_rcClient.bottom - l_rcClient.top;
  }
  else if (l_pState)
  {
    l_nClientWidth = static_cast<int>(l_pState->mScreenXsize);
    l_nClientHeight = static_cast<int>(l_pState->mScreenYsize);
  }

  HideWindowMenuBar(m_hWnd, m_dwWindowStyle, l_nClientWidth, l_nClientHeight);
  SetClientWindowVisible(FALSE);

  ParseOfflineRegedCharReplay(commandLine);
  if (m_bOfflineRegedCharReplay)
  {
    m_szSelectedMap[0] = '\0';
  }
  else
  {
    CopyMapSelection(m_szSelectedMap, sizeof(m_szSelectedMap), commandLine);
  }

  if (!Create())
  {
#if defined(_DEBUG)
    AppendCaptureLog("Initialize: Create failed");
#endif
    ReleaseR3D3D();
    ReleaseR3SoundSystem();
    ReleaseR3Engine();
    return false;
  }

  const BOOL l_bLoadedAbuseFilter = _LoadAbuseFilter("./DataTable/cryp.dat");
  const BOOL l_bLoadedAdvFilter = _LoadAdvFilter("./DataTable/advr.dat");

#if defined(_DEBUG)
  AppendCaptureLog("Initialize: success progress=%u map=%s",
                   static_cast<unsigned>(m_byGameProgressID),
                   m_szSelectedMap[0] ? m_szSelectedMap : "<empty>");
  if (m_bOfflineRegedCharReplay)
  {
    AppendCaptureLog("Initialize: offline reged char replay path=%s", m_szOfflineRegedCharPath);
  }
  AppendCaptureLog("Initialize: filter abuse=%d adv=%d",
                   l_bLoadedAbuseFilter ? 1 : 0,
                   l_bLoadedAdvFilter ? 1 : 0);
#endif
  m_isInitialized = true;
  return true;
}

int CMainApp::Run()
{
  BOOL l_bGotMsg = FALSE;
  MSG l_sMsg;
  ZeroMemory(&l_sMsg, sizeof(l_sMsg));
#if defined(_DEBUG)
  static bool s_loggedRunStart = false;
  if (!s_loggedRunStart)
  {
    AppendCaptureLog("Run: begin visible=%d active=%d ready=%d", m_bClientWindowVisible ? 1 : 0, m_bIsActive ? 1 : 0, IsReady() ? 1 : 0);
    s_loggedRunStart = true;
  }
#endif

  PeekMessage(&l_sMsg, NULL, 0U, 0U, PM_NOREMOVE);

  while (WM_QUIT != l_sMsg.message)
  {
  if (IsReady() && m_pNetworkMgr)
  {
      m_pNetworkMgr->OnLoop();
  }

    if (IsReady())
    {
      const BOOL l_bTickWhileHidden = !m_bClientWindowVisible || m_byGameProgressID != GPI_MAIN_GAME;
      l_bGotMsg = (m_bIsActive || l_bTickWhileHidden)
                    ? PeekMessage(&l_sMsg, NULL, 0U, 0U, PM_REMOVE)
                    : GetMessage(&l_sMsg, NULL, 0U, 0U);
    }
    else
    {
      l_bGotMsg = GetMessage(&l_sMsg, NULL, 0U, 0U);
    }

    if (l_bGotMsg)
    {
      TranslateMessage(&l_sMsg);
      DispatchMessage(&l_sMsg);
    }
    else if (IsReady())
    {
      const HRESULT l_hrPrepareLoop = PrepareLoop();
#if defined(_DEBUG)
      static bool s_loggedPrepareLoopFailure = false;
      static bool s_loggedPrepareLoopSuccess = false;
      if (SUCCEEDED(l_hrPrepareLoop))
      {
        if (!s_loggedPrepareLoopSuccess)
        {
          AppendCaptureLog("Run: PrepareLoop succeeded visible=%d active=%d", m_bClientWindowVisible ? 1 : 0, m_bIsActive ? 1 : 0);
          s_loggedPrepareLoopSuccess = true;
        }
      }
      else if (!s_loggedPrepareLoopFailure)
      {
        AppendCaptureLog("Run: PrepareLoop failed hr=0x%08X visible=%d active=%d", l_hrPrepareLoop, m_bClientWindowVisible ? 1 : 0, m_bIsActive ? 1 : 0);
        s_loggedPrepareLoopFailure = true;
      }
#endif
      if (S_OK == l_hrPrepareLoop)
      {
        GameMainLoop();
        EndLoop();
      }
    }
  }

  return static_cast<int>(l_sMsg.wParam);
}

void CMainApp::Shutdown()
{
#if defined(_DEBUG)
  AppendCaptureLog("Shutdown: begin initialized=%d mapLoaded=%d", m_isInitialized ? 1 : 0, m_isMapLoaded ? 1 : 0);
#endif
  if (!m_isInitialized)
  {
#if defined(_DEBUG)
    AppendCaptureLog("Shutdown: skip");
#endif
    return;
  }

  Destroy();
#if defined(_DEBUG)
  AppendCaptureLog("Shutdown: release d3d");
#endif
  ReleaseR3D3D();
#if defined(_DEBUG)
  AppendCaptureLog("Shutdown: release engine");
#endif
  ReleaseR3Engine();
  ReleaseR3SoundSystem();
  m_isInitialized = false;
  if (g_pMainApp == this)
  {
    g_pMainApp = NULL;
  }
#if defined(_DEBUG)
  AppendCaptureLog("Shutdown: end");
#endif
}

BOOL CMainApp::Create(void)
{
  if (m_bCreatedAllObject)
  {
    return FALSE;
  }

  if (!m_pNetworkMgr)
  {
    m_pNetworkMgr = new CNetworkMgr;
  }

  if (!m_pNetworkMgr->Create())
  {
    return FALSE;
  }

  if (!m_land.Create())
  {
    m_pNetworkMgr->Destroy();
    return FALSE;
  }

  if (!m_CharacterMgr.Initialize(GetD3dDevice()))
  {
    m_land.Destroy();
    m_pNetworkMgr->Destroy();
    return FALSE;
  }

  if (!m_cCharDataMgr.Create() ||
      !m_cItemDataMgr.Create())
  {
    m_CharacterMgr.Shutdown();
    m_land.Destroy();
    m_pNetworkMgr->Destroy();
    return FALSE;
  }

  g_pCharDataMgr = &m_cCharDataMgr;
  g_pItemDataMgr = &m_cItemDataMgr;
  g_pCharResDataMgr = &m_cCharResDataMgr;

  if (!m_cUI.Initialize())
  {
#if defined(_DEBUG)
    AppendCaptureLog("Create: m_cUI.Initialize failed");
#endif
  }

  if (!LoadData())
  {
    m_pNetworkMgr->Destroy();
    return FALSE;
  }

  Create_GameProgress(m_byGameProgressID);

  if (m_bOfflineRegedCharReplay)
  {
    if (!m_pNetworkMgr->LoadRegedCharResultDump(m_szOfflineRegedCharPath))
    {
#if defined(_DEBUG)
      AppendCaptureLog("Create: offline reged replay load failed path=%s", m_szOfflineRegedCharPath);
#endif
    }
    else
    {
#if defined(_DEBUG)
      AppendCaptureLog("Create: offline reged replay loaded path=%s", m_szOfflineRegedCharPath);
#endif
    }
  }
  else if (!GetDataFromLauncher())
  {
#if defined(_DEBUG)
    AppendCaptureLog("Create: launcher data missing; continuing without world bootstrap data");
#endif
  }

  m_bCreatedAllObject = TRUE;
  return TRUE;
}

BOOL CMainApp::Destroy(void)
{
  if (!m_bCreatedAllObject)
  {
    return FALSE;
  }

  if (m_pGameProgress)
  {
    m_pGameProgress->Destroy();
    delete m_pGameProgress;
    m_pGameProgress = NULL;
  }

  UnloadData();
  m_cUI.Shutdown();
  m_CharacterMgr.Shutdown();
  m_pPlayer = NULL;
  m_land.Destroy();
  if (m_pNetworkMgr)
  {
    m_pNetworkMgr->Destroy();
    delete m_pNetworkMgr;
    m_pNetworkMgr = NULL;
  }

  m_cItemDataMgr.Destroy();
  m_cCharDataMgr.Destroy();
  m_cCharResDataMgr.UnloadData();

  g_pItemDataMgr = NULL;
  g_pCharDataMgr = NULL;
  g_pCharResDataMgr = NULL;

  m_bCreatedAllObject = FALSE;
  return TRUE;
}

void CMainApp::Create_GameProgress(BYTE pi_byGameProgressID)
{
  if (m_pGameProgress)
  {
    m_pGameProgress->Destroy();
    delete m_pGameProgress;
    m_pGameProgress = NULL;
  }

  m_byGameProgressID = pi_byGameProgressID;
  if (m_byGameProgressID == GPI_TITLE)
  {
    m_pGameProgress = new CGP_Title;
  }
  else if (m_byGameProgressID == GPI_LOGIN)
  {
    m_pGameProgress = new CGP_LogIn;
  }
  else if (m_byGameProgressID == GPI_MAIN_GAME)
  {
    m_pGameProgress = new CGP_MainGame;
  }
}

BOOL CMainApp::LoadData(void)
{
  if (m_bLoadedAllData)
  {
    return TRUE;
  }

  if (!m_land.LoadData() ||
      !m_cCharResDataMgr.LoadData() ||
      !m_cCharDataMgr.LoadData() ||
      !m_cItemDataMgr.LoadData())
  {
    return FALSE;
  }

  m_bLoadedAllData = TRUE;
  return TRUE;
}

bool CMainApp::UnloadData(void)
{
  if (!m_bLoadedAllData)
  {
    return true;
  }

  m_cItemDataMgr.UnloadData();
  m_cCharDataMgr.UnloadData();
  m_cCharResDataMgr.UnloadData();
  m_land.Destroy();
  m_land.Create();

  m_bLoadedAllData = FALSE;
  return true;
}

BOOL CMainApp::GameMainLoop(void)
{
#if defined(_DEBUG)
  static bool s_loggedGameMainLoopEntry = false;
  if (!s_loggedGameMainLoopEntry)
  {
    AppendCaptureLog("GameMainLoop: first entry progress=%u created=%d loaded=%d visible=%d",
                     static_cast<unsigned>(m_byGameProgressID),
                     m_bCreatedAllObject ? 1 : 0,
                     m_bLoadedAllData ? 1 : 0,
                     m_bClientWindowVisible ? 1 : 0);
    s_loggedGameMainLoopEntry = true;
  }
#endif
  if (m_bIsQuit || !IsReadyToStart())
  {
    return FALSE;
  }

  R3CalculateTime();
  if (m_bIsActive)
  {
    InputProcess();
  }

  if (m_pGameProgress)
  {
    if (!m_pGameProgress->IsCreated())
    {
#if defined(_DEBUG)
      AppendCaptureLog("GameMainLoop: creating progress=%u", static_cast<unsigned>(m_byGameProgressID));
#endif
      if (!m_pGameProgress->Create())
      {
#if defined(_DEBUG)
        AppendCaptureLog("GameMainLoop: progress create failed progress=%u", static_cast<unsigned>(m_byGameProgressID));
#endif
        Sleep(1);
        return TRUE;
      }
#if defined(_DEBUG)
      AppendCaptureLog("GameMainLoop: progress create succeeded progress=%u", static_cast<unsigned>(m_byGameProgressID));
#endif
    }

    if (m_pGameProgress->MainLoop(m_bIsActive))
    {
      BYTE l_byNextProgressID = GPI_INVALID;
      if (m_byGameProgressID == GPI_TITLE)
      {
        l_byNextProgressID = GPI_LOGIN;
      }
      else if (m_byGameProgressID == GPI_LOGIN)
      {
        l_byNextProgressID = GPI_MAIN_GAME;
      }

      if (l_byNextProgressID == GPI_INVALID)
      {
        m_bIsQuit = TRUE;
        return FALSE;
      }

#if defined(_DEBUG)
      AppendCaptureLog("GameMainLoop: progress transition %u -> %u",
                       static_cast<unsigned>(m_byGameProgressID),
                       static_cast<unsigned>(l_byNextProgressID));
#endif
      Create_GameProgress(l_byNextProgressID);
    }
  }

  Sleep(1);
  return TRUE;
}

BOOL CMainApp::IsReadyToStart(void)
{
  return m_bCreatedAllObject && m_bLoadedAllData;
}

BOOL CMainApp::GetDataFromLauncher(void)
{
  if (!m_pNetworkMgr)
  {
    return FALSE;
  }

  HANDLE l_hFileMap = CreateFileA(".\\System\\DefaultSet.tmp",
                                  GENERIC_READ,
                                  FILE_SHARE_READ,
                                  NULL,
                                  OPEN_EXISTING,
                                  FILE_ATTRIBUTE_NORMAL,
                                  NULL);
  if (l_hFileMap == INVALID_HANDLE_VALUE)
  {
    m_pNetworkMgr->SetStatusText("GetDataFromLauncher: failed to open .\\System\\DefaultSet.tmp error=%lu", GetLastError());
    return FALSE;
  }

  DWORD l_dwFileSize = GetFileSize(l_hFileMap, NULL);
  if (l_dwFileSize == INVALID_FILE_SIZE && GetLastError() != NO_ERROR)
  {
    CloseHandle(l_hFileMap);
    m_pNetworkMgr->SetStatusText("GetDataFromLauncher: GetFileSize failed error=%lu", GetLastError());
    return FALSE;
  }

  std::vector<BYTE> l_vecFileData(static_cast<size_t>(l_dwFileSize));
  DWORD l_dwReadedSize = 0;
  const BOOL l_bReadOk = ReadFile(l_hFileMap,
                                  l_vecFileData.empty() ? NULL : l_vecFileData.data(),
                                  l_dwFileSize,
                                  &l_dwReadedSize,
                                  NULL);
  CloseHandle(l_hFileMap);

  if (!l_bReadOk)
  {
    m_pNetworkMgr->SetStatusText("GetDataFromLauncher: read failed size=%lu", l_dwReadedSize);
    return FALSE;
  }

  DWORD l_dwWorldIP = 0;
  WORD l_wWorldPort = 0;
  DWORD l_dwAccountSerial = 0;
  DWORD l_dwBillingAccountSerial = 0;
  DWORD l_dwBillingType = 0;
  DWORD l_dwIsAdult = 0;
  WORD l_wLanguageIndex = 0;
  WORD l_wZeroFieldValue = 0;
  DWORD l_dwWorldMasterKey[KEY_NUM]{};
  char l_szID[64]{};
  const char *l_pSetMode = "legacy";

  if (l_dwReadedSize >= sizeof(_ENTER_WORLD_TEMP_DATA_PATRON))
  {
    _ENTER_WORLD_TEMP_DATA_PATRON l_sData;
    ZeroMemory(&l_sData, sizeof(l_sData));
    memcpy(&l_sData, l_vecFileData.data(), sizeof(l_sData));

    l_sData.m_dwWorldIP ^= kDefaultSetWorldIpXor;
    l_sData.m_wWorldPort ^= kDefaultSetWorldPortXor;
    l_sData.m_dwAccountSerial ^= kDefaultSetAccountSerialXor;
    l_sData.m_dwBillingAccountSerial ^= kDefaultSetBillingAccountSerialXor;
    l_sData.m_dwBillingType ^= kDefaultSetBillingTypeXor;
    l_sData.m_dwIsAdult ^= kDefaultSetIsAdultXor;
    l_sData.m_wLanguageIndex ^= kDefaultSetLanguageIndexXor;
    for (int i = 0; i < KEY_NUM; ++i)
    {
      l_sData.m_dwWorldMasterKey[i] ^= kDefaultSetWorldMasterKeyXor[i];
    }

    l_dwWorldIP = l_sData.m_dwWorldIP;
    l_wWorldPort = l_sData.m_wWorldPort;
    l_dwAccountSerial = l_sData.m_dwAccountSerial;
    l_dwBillingAccountSerial = l_sData.m_dwBillingAccountSerial;
    l_dwBillingType = l_sData.m_dwBillingType;
    l_dwIsAdult = l_sData.m_dwIsAdult;
    l_wLanguageIndex = l_sData.m_wLanguageIndex;
    l_wZeroFieldValue = l_sData.m_wZeroFieldValue;
    memcpy(l_dwWorldMasterKey, l_sData.m_dwWorldMasterKey, sizeof(l_dwWorldMasterKey));
    memcpy(l_szID, l_sData.m_szID, sizeof(l_sData.m_szID));
    l_szID[sizeof(l_sData.m_szID)] = '\0';
    l_pSetMode = "patron";
  }
  else if (l_dwReadedSize >= sizeof(_ENTER_WORLD_TEMP_DATA))
  {
    _ENTER_WORLD_TEMP_DATA l_sData;
    ZeroMemory(&l_sData, sizeof(l_sData));
    memcpy(&l_sData, l_vecFileData.data(), sizeof(l_sData));

    l_sData.m_dwWorldIP ^= kDefaultSetWorldIpXor;
    l_sData.m_wWorldPort ^= kDefaultSetWorldPortXor;
    l_sData.m_dwAccountSerial ^= kDefaultSetAccountSerialXor;
    l_sData.m_dwBillingAccountSerial ^= kDefaultSetBillingAccountSerialXor;
    l_sData.m_dwBillingType ^= kDefaultSetBillingTypeXor;
    l_sData.m_dwIsAdult ^= kDefaultSetIsAdultXor;
    l_sData.m_wLanguageIndex ^= kDefaultSetLanguageIndexXor;
    for (int i = 0; i < KEY_NUM; ++i)
    {
      l_sData.m_dwWorldMasterKey[i] ^= kDefaultSetWorldMasterKeyXor[i];
    }

    l_dwWorldIP = l_sData.m_dwWorldIP;
    l_wWorldPort = l_sData.m_wWorldPort;
    l_dwAccountSerial = l_sData.m_dwAccountSerial;
    l_dwBillingAccountSerial = l_sData.m_dwBillingAccountSerial;
    l_dwBillingType = l_sData.m_dwBillingType;
    l_dwIsAdult = l_sData.m_dwIsAdult;
    l_wLanguageIndex = l_sData.m_wLanguageIndex;
    l_wZeroFieldValue = l_sData.m_wZeroFieldValue;
    memcpy(l_dwWorldMasterKey, l_sData.m_dwWorldMasterKey, sizeof(l_dwWorldMasterKey));
    memcpy(l_szID, l_sData.m_szID, sizeof(l_sData.m_szID));
    l_szID[sizeof(l_sData.m_szID)] = '\0';
    l_pSetMode = "default";
  }
  else if (l_dwReadedSize >= sizeof(_ENTER_WORLD_TEMP_DATA_LEGACY))
  {
    _ENTER_WORLD_TEMP_DATA_LEGACY l_sData;
    ZeroMemory(&l_sData, sizeof(l_sData));
    memcpy(&l_sData, l_vecFileData.data(), sizeof(l_sData));

    l_sData.m_dwWorldIP ^= kDefaultSetWorldIpXor;
    l_sData.m_wWorldPort ^= kDefaultSetWorldPortXor;
    l_sData.m_dwAccountSerial ^= kDefaultSetAccountSerialXor;
    for (int i = 0; i < KEY_NUM; ++i)
    {
      l_sData.m_dwWorldMasterKey[i] ^= kDefaultSetWorldMasterKeyXor[i];
    }

    l_dwWorldIP = l_sData.m_dwWorldIP;
    l_wWorldPort = l_sData.m_wWorldPort;
    l_dwAccountSerial = l_sData.m_dwAccountSerial;
    l_dwBillingAccountSerial = l_dwAccountSerial;
    memcpy(l_dwWorldMasterKey, l_sData.m_dwWorldMasterKey, sizeof(l_dwWorldMasterKey));
    memcpy(l_szID, l_sData.m_szID, sizeof(l_sData.m_szID));
    l_szID[sizeof(l_sData.m_szID)] = '\0';
  }
  else
  {
    m_pNetworkMgr->SetStatusText("GetDataFromLauncher: unsupported DefaultSet.tmp size=%lu", l_dwReadedSize);
    return FALSE;
  }

  m_pNetworkMgr->SetServerIP(GST_WORLD, l_dwWorldIP);
  m_pNetworkMgr->SetServerPort(GST_WORLD, l_wWorldPort);
  m_pNetworkMgr->SetAccountIndex(l_dwAccountSerial);
  m_pNetworkMgr->SetWorldServerKey(l_dwWorldMasterKey);
  m_pNetworkMgr->SetID(l_szID);

  m_pNetworkMgr->SetHasLauncherData(TRUE);
  m_pNetworkMgr->SetStatusText("DefaultSet.tmp loaded: mode=%s world=%s:%lu accountSerial=%lu billingSerial=%lu billingType=%lu adult=%lu lang=%u zero=0x%04X id=%s size=%lu",
                               l_pSetMode,
                               m_pNetworkMgr->GetAddressText(m_pNetworkMgr->GetServerIP(GST_WORLD)),
                               m_pNetworkMgr->GetServerPort(GST_WORLD),
                               m_pNetworkMgr->GetAccountIndex(),
                               l_dwBillingAccountSerial,
                               l_dwBillingType,
                               l_dwIsAdult,
                               static_cast<unsigned>(l_wLanguageIndex),
                               static_cast<unsigned>(l_wZeroFieldValue),
                               m_pNetworkMgr->GetID(),
                               l_dwFileSize);
  return TRUE;
}

BOOL CMainApp::InputProcess(void)
{
  if (!m_bIsActive)
  {
    return TRUE;
  }

  if (m_bRequestQuitProgram)
  {
    PostMessageA(m_hWnd, WM_CLOSE, 0, 0);
    m_bRequestQuitProgram = FALSE;
  }

  if ((GetKeyState(VK_MENU) & 0x8000) != 0 &&
      ((GetAsyncKeyState(VK_Q) & 0xFFFF0000) == 0xFFFF0000))
  {
    RequestQuitProgram();
  }

  return TRUE;
}

BOOL CMainApp::RequestQuitProgram(DWORD, char *)
{
  m_bIsQuit = TRUE;
  m_bRequestQuitProgram = FALSE;
  if (m_hWnd)
  {
    PostMessageA(m_hWnd, WM_CLOSE, 0, 0);
  }
  return TRUE;
}

void CMainApp::SetClientWindowVisible(BOOL pi_bVisible)
{
  if (!m_hWnd)
  {
    m_bClientWindowVisible = pi_bVisible;
    return;
  }

  const BOOL l_bVisible = pi_bVisible ? TRUE : FALSE;
  if (m_bClientWindowVisible == l_bVisible)
  {
    return;
  }

  ShowWindow(m_hWnd, l_bVisible ? SW_SHOW : SW_HIDE);
  if (l_bVisible)
  {
    UpdateWindow(m_hWnd);
  }

  m_bClientWindowVisible = l_bVisible;
}

HRESULT CMainApp::FrameMove()
{
  return S_OK;
}

HRESULT CMainApp::Render()
{
  return S_OK;
}

HRESULT CMainApp::RestoreDeviceObjects()
{
  R3LoadTextTexture();
  return R3D3dWrapper::RestoreDeviceObjects();
}

HRESULT CMainApp::InvalidateDeviceObjects()
{
  R3ReleaseTextTexture();
  return R3D3dWrapper::InvalidateDeviceObjects();
}

BOOL CMainApp::LoadMainGameData(void)
{
  if (m_isMapLoaded)
  {
    return TRUE;
  }

  if (!m_szSelectedMap[0])
  {
#if defined(_DEBUG)
    AppendCaptureLog("LoadMainGameData: no startup map selection, waiting for original client flow");
#endif
    return TRUE;
  }

  const char *l_pMapSelection = m_szSelectedMap;
  if (!LoadMap(l_pMapSelection))
  {
#if defined(_DEBUG)
    AppendCaptureLog("LoadMainGameData: LoadMap failed selection=%s", l_pMapSelection);
#endif
    return FALSE;
  }

  return TRUE;
}

void CMainApp::UnloadMainGameData(void)
{
  m_pPlayer = NULL;
  m_CharacterMgr.Clear();

  if (m_isMapLoaded)
  {
    m_land.UnloadLevel();
  }

  m_isMapLoaded = false;
  m_mapName[0] = '\0';
  m_hasSpawnCameraTarget = false;
  m_isRightMouseDragging = false;
  m_pendingWheelDelta = 0;
}

BOOL CMainApp::LoadLoginLobbyData(void)
{
  static const char kLoginLobbyPath[] = ".\\map\\lobby\\lobby.BSP";
  static const char kLoginLobbyCameraPath[] = ".\\map\\lobby\\lobby.cam";

  if (m_isMapLoaded && _stricmp(m_mapName, "lobby") == 0)
  {
    return TRUE;
  }

  if (!LoadMap(kLoginLobbyPath))
  {
#if defined(_DEBUG)
    AppendCaptureLog("LoadLoginLobbyData: LoadMap failed path=%s", kLoginLobbyPath);
#endif
    return FALSE;
  }

  // The opening scene is a lobby preview, not an in-world playable avatar.
  m_pPlayer = NULL;
  m_CharacterMgr.Clear();
  m_cLoginLobbyAniCamera.ReleaseAniCamera();
  m_bLoginLobbyCameraAnimating = FALSE;
  if (IsExistFile(const_cast<char *>(kLoginLobbyCameraPath)))
  {
    m_cLoginLobbyAniCamera.LoadAniCamera(const_cast<char *>(kLoginLobbyCameraPath));
    SetLoginLobbyOpeningCamera();
  }
  else
  {
    ResetCameraFromLoadedMap();
  }

  if (CLevel *l_pLevel = m_land.GetLevel())
  {
    if (m_cLoginLobbyAniCamera.IsLoadedAniCamera())
    {
      l_pLevel->SetCameraPos(m_vecLoginLobbyCameraPos);
      l_pLevel->SetViewMatrix(R3MoveGetViewMatrix());
    }
    else
    {
      l_pLevel->SetCameraPos(m_cameraPosition);
      l_pLevel->SetViewMatrix(R3MoveGetViewMatrix());
    }
  }

  UpdateProjectionParameters();
  return TRUE;
}

BOOL CMainApp::PlayLoginLobbyCamera(const char *pi_pCameraName,
                                    DWORD pi_dwStartFrame,
                                    DWORD pi_dwEndFrame,
                                    DWORD pi_dwFlag)
{
#if defined(_DEBUG)
  AppendCaptureLog("PlayLoginLobbyCamera: begin name=%s start=%u end=%u flag=%u loaded=%d",
                   pi_pCameraName ? pi_pCameraName : "(null)",
                   static_cast<unsigned>(pi_dwStartFrame),
                   static_cast<unsigned>(pi_dwEndFrame),
                   static_cast<unsigned>(pi_dwFlag),
                   m_cLoginLobbyAniCamera.IsLoadedAniCamera() ? 1 : 0);
#endif
  if (!pi_pCameraName || !pi_pCameraName[0] || !m_cLoginLobbyAniCamera.IsLoadedAniCamera())
  {
    return FALSE;
  }

  DWORD l_dwCameraIndex = m_cLoginLobbyAniCamera.GetCameraIndex(const_cast<char *>(pi_pCameraName));
  if (l_dwCameraIndex == static_cast<DWORD>(-1))
  {
#if defined(_DEBUG)
    AppendCaptureLog("PlayLoginLobbyCamera: missing name=%s", pi_pCameraName);
#endif
    return FALSE;
  }

#if defined(_DEBUG)
  AppendCaptureLog("PlayLoginLobbyCamera: SetPrePlayCamera index=%u",
                   static_cast<unsigned>(l_dwCameraIndex));
#endif
  m_cLoginLobbyAniCamera.SetPrePlayCamera(l_dwCameraIndex, pi_dwStartFrame, pi_dwEndFrame, pi_dwFlag);
  m_bLoginLobbyCameraAnimating = (pi_dwFlag == _CAM_FLAG_FINAL_STOP);

  float l_matLobbyCamera[4][4];
#if defined(_DEBUG)
  AppendCaptureLog("PlayLoginLobbyCamera: PlayAniCamera initial");
#endif
  if (m_cLoginLobbyAniCamera.PlayAniCamera(l_matLobbyCamera, 1.0f, FALSE))
  {
    R3SetCameraMatrix(m_vecLoginLobbyCameraPos, l_matLobbyCamera);
  }

  if (CLevel *l_pLevel = m_land.GetLevel())
  {
    l_pLevel->SetCameraPos(m_vecLoginLobbyCameraPos);
    l_pLevel->SetViewMatrix(R3MoveGetViewMatrix());
  }

  UpdateProjectionParameters();
#if defined(_DEBUG)
  AppendCaptureLog("PlayLoginLobbyCamera: end name=%s", pi_pCameraName);
#endif
  return TRUE;
}

BOOL CMainApp::SetLoginLobbyOpeningCamera(void)
{
  if (PlayLoginLobbyCamera("c_ex01", 0, _CAM_PLAY_FULL_FRAME, _CAM_FLAG_LOOP))
  {
    return TRUE;
  }

  if (PlayLoginLobbyCamera("c_ex02", 0, _CAM_PLAY_FULL_FRAME, _CAM_FLAG_LOOP))
  {
    return TRUE;
  }

  if (PlayLoginLobbyCamera("c_01", 0, _CAM_PLAY_FULL_FRAME, _CAM_FLAG_LOOP))
  {
    return TRUE;
  }

  return PlayLoginLobbyCamera("c_05", 0, _CAM_PLAY_FULL_FRAME, _CAM_FLAG_LOOP);
}

BOOL CMainApp::PlayLoginLobbyCharacterEntryCamera(BYTE pi_bySlotIndex)
{
  static const char *kCameraName[3] = { "c_01_01", "c_01_02", "c_01_03" };
  static const DWORD kEndFrame[3] = { 45, 40, 40 };
  const BYTE l_bySlotIndex = (pi_bySlotIndex > 2) ? 0 : pi_bySlotIndex;
  return PlayLoginLobbyCamera(kCameraName[l_bySlotIndex], 0, kEndFrame[l_bySlotIndex], _CAM_FLAG_FINAL_STOP);
}

BOOL CMainApp::PlayLoginLobbyCharacterNextCamera(BYTE pi_byOldSlotIndex)
{
  static const char *kCameraName[3] = { "c_02_01", "c_02_02", "c_02_03" };
  const BYTE l_byOldSlotIndex = (pi_byOldSlotIndex > 2) ? 0 : pi_byOldSlotIndex;
  return PlayLoginLobbyCamera(kCameraName[l_byOldSlotIndex], 0, 30, _CAM_FLAG_FINAL_STOP);
}

BOOL CMainApp::PlayLoginLobbyCharacterPrevCamera(BYTE pi_byOldSlotIndex)
{
  static const char *kCameraName[3] = { "c_02_03", "c_02_01", "c_02_02" };
  const BYTE l_byOldSlotIndex = (pi_byOldSlotIndex > 2) ? 0 : pi_byOldSlotIndex;
  return PlayLoginLobbyCamera(kCameraName[l_byOldSlotIndex], 30, 0, _CAM_FLAG_FINAL_STOP);
}

void CMainApp::UnloadLoginLobbyData(void)
{
  if (!m_isMapLoaded)
  {
    return;
  }

  m_pPlayer = NULL;
  m_CharacterMgr.Clear();
  m_cLoginLobbyAniCamera.ReleaseAniCamera();
  m_bLoginLobbyCameraAnimating = FALSE;
  m_vecLoginLobbyCameraPos[0] = 0.0f;
  m_vecLoginLobbyCameraPos[1] = 0.0f;
  m_vecLoginLobbyCameraPos[2] = 0.0f;
  m_land.UnloadLevel();
  m_isMapLoaded = false;
  m_mapName[0] = '\0';
  m_hasSpawnCameraTarget = false;
  m_isRightMouseDragging = false;
  m_pendingWheelDelta = 0;
}

void CMainApp::ClearLoginLobbyCharacterDummies(void)
{
  if (m_byGameProgressID != GPI_LOGIN)
  {
    return;
  }

  if (m_pPlayer)
  {
    m_pPlayer = NULL;
  }

  m_CharacterMgr.Clear();
}

BOOL CMainApp::BuildLoginLobbyCharacterDummies(const _reged_char_result_zone &pi_stRegedCharResult)
{
#if defined(_DEBUG)
  AppendCaptureLog("BuildLoginLobbyCharacterDummies: begin charNum=%u",
                   static_cast<unsigned>(pi_stRegedCharResult.byCharNum));
#endif
  if (m_byGameProgressID != GPI_LOGIN)
  {
#if defined(_DEBUG)
    AppendCaptureLog("BuildLoginLobbyCharacterDummies: skipped progress=%u",
                     static_cast<unsigned>(m_byGameProgressID));
#endif
    return FALSE;
  }

#if defined(_DEBUG)
  AppendCaptureLog("BuildLoginLobbyCharacterDummies: clear begin");
#endif
  ClearLoginLobbyCharacterDummies();
#if defined(_DEBUG)
  AppendCaptureLog("BuildLoginLobbyCharacterDummies: clear end");
#endif

  BOOL l_bLoadedAnyCharacter = FALSE;
  const BYTE l_byCharNum = pi_stRegedCharResult.byCharNum > 3
                             ? static_cast<BYTE>(3)
                             : pi_stRegedCharResult.byCharNum;
  for (BYTE i = 0; i < l_byCharNum; ++i)
  {
    const _REGED_AVATOR_DB &l_sRegedAvatar = pi_stRegedCharResult.RegedList[i];
#if defined(_DEBUG)
    AppendCaptureLog("BuildLoginLobbyCharacterDummies: record=%u slot=%u race=%u base=0x%08X",
                     static_cast<unsigned>(i),
                     static_cast<unsigned>(l_sRegedAvatar.m_bySlotIndex),
                     static_cast<unsigned>(l_sRegedAvatar.m_byRaceSexCode),
                     static_cast<unsigned>(l_sRegedAvatar.m_dwBaseShape));
#endif
    if (l_sRegedAvatar.m_bySlotIndex > 2)
    {
      continue;
    }

    CPlayer *l_pPlayer = m_CharacterMgr.AddPlayer(l_sRegedAvatar.m_bySlotIndex);
    if (!l_pPlayer)
    {
#if defined(_DEBUG)
      AppendCaptureLog("BuildLoginLobbyCharacterDummies: AddPlayer failed slot=%u",
                       static_cast<unsigned>(l_sRegedAvatar.m_bySlotIndex));
#endif
      continue;
    }

    char l_szAvatarName[MAX_NAME_LENGTH];
    ZeroMemory(l_szAvatarName, sizeof(l_szAvatarName));
    memcpy(l_szAvatarName,
           l_sRegedAvatar.m_wszAvatorName,
           sizeof(l_sRegedAvatar.m_wszAvatorName) < sizeof(l_szAvatarName)
             ? sizeof(l_sRegedAvatar.m_wszAvatorName)
             : sizeof(l_szAvatarName) - 1);
    l_szAvatarName[sizeof(l_szAvatarName) - 1] = '\0';
    l_pPlayer->SetName(l_szAvatarName[0] ? l_szAvatarName : "Player");

#if defined(_DEBUG)
    AppendCaptureLog("BuildLoginLobbyCharacterDummies: LoadRegedAvatar begin slot=%u",
                     static_cast<unsigned>(l_sRegedAvatar.m_bySlotIndex));
#endif
    if (!l_pPlayer->LoadRegedAvatar(l_sRegedAvatar))
    {
#if defined(_DEBUG)
      AppendCaptureLog("BuildLoginLobbyCharacterDummies: LoadRegedAvatar failed slot=%u",
                       static_cast<unsigned>(l_sRegedAvatar.m_bySlotIndex));
#endif
      continue;
    }
#if defined(_DEBUG)
    AppendCaptureLog("BuildLoginLobbyCharacterDummies: LoadRegedAvatar end slot=%u",
                     static_cast<unsigned>(l_sRegedAvatar.m_bySlotIndex));
#endif

    const BYTE l_bySlotIndex = l_sRegedAvatar.m_bySlotIndex;
#if defined(_DEBUG)
    AppendCaptureLog("BuildLoginLobbyCharacterDummies: placement begin slot=%u",
                     static_cast<unsigned>(l_bySlotIndex));
#endif
    l_pPlayer->SetPosition(kLoginLobbyCharacterTopPos[l_bySlotIndex][0],
                           kLoginLobbyCharacterTopPos[l_bySlotIndex][1],
                           kLoginLobbyCharacterTopPos[l_bySlotIndex][2]);
    l_pPlayer->SetRotY(180.0f - (static_cast<float>(l_bySlotIndex) * 120.0f));
    l_pPlayer->SetLightColor(D3DCOLOR_XRGB(128, 128, 128));
#if defined(_DEBUG)
    AppendCaptureLog("BuildLoginLobbyCharacterDummies: placement end slot=%u",
                     static_cast<unsigned>(l_bySlotIndex));
#endif
    l_bLoadedAnyCharacter = TRUE;
  }

#if defined(_DEBUG)
  AppendCaptureLog("BuildLoginLobbyCharacterDummies: end loadedAny=%d", l_bLoadedAnyCharacter ? 1 : 0);
#endif
  return l_bLoadedAnyCharacter;
}

void CMainApp::FrameMoveLoginLobby(void)
{
  if (!m_isMapLoaded)
  {
    return;
  }

#if defined(_DEBUG)
  static DWORD s_dwFrameMoveTraceCount = 0;
  const bool l_bTraceFrameMove = (s_dwFrameMoveTraceCount++ < 120);
  if (l_bTraceFrameMove)
  {
    AppendCaptureLog("FrameMoveLoginLobby: begin cameraLoaded=%d animating=%d",
                     m_cLoginLobbyAniCamera.IsLoadedAniCamera() ? 1 : 0,
                     m_bLoginLobbyCameraAnimating ? 1 : 0);
  }
#endif
  if (CLevel *l_pLevel = m_land.GetLevel())
  {
    if (m_cLoginLobbyAniCamera.IsLoadedAniCamera())
    {
      float l_matLobbyCamera[4][4];
      const BOOL l_bCameraPlaying = m_cLoginLobbyAniCamera.PlayAniCamera(l_matLobbyCamera, 1.0f);
      R3SetCameraMatrix(m_vecLoginLobbyCameraPos, l_matLobbyCamera);
      if (!l_bCameraPlaying)
      {
        m_bLoginLobbyCameraAnimating = FALSE;
      }
      l_pLevel->SetCameraPos(m_vecLoginLobbyCameraPos);
      l_pLevel->SetViewMatrix(R3MoveGetViewMatrix());
    }
    else
    {
      m_fViewAngleY = NormalizeAngle360(m_fViewAngleY - (8.0f * R3GetLoopTime()));
      ApplyCamera();
      l_pLevel->SetCameraPos(m_cameraPosition);
      l_pLevel->SetViewMatrix(R3MoveGetViewMatrix());
    }

#if defined(_DEBUG)
    if (l_bTraceFrameMove)
    {
      AppendCaptureLog("FrameMoveLoginLobby: level FrameMove begin");
    }
#endif
    l_pLevel->FrameMove();
#if defined(_DEBUG)
    if (l_bTraceFrameMove)
    {
      AppendCaptureLog("FrameMoveLoginLobby: level FrameMove end");
    }
#endif
  }
#if defined(_DEBUG)
  if (l_bTraceFrameMove)
  {
    AppendCaptureLog("FrameMoveLoginLobby: CharacterMgr FrameMove begin");
  }
#endif
  m_CharacterMgr.FrameMove();
#if defined(_DEBUG)
  if (l_bTraceFrameMove)
  {
    AppendCaptureLog("FrameMoveLoginLobby: CharacterMgr FrameMove end");
    AppendCaptureLog("FrameMoveLoginLobby: CharacterMgr Animation begin");
  }
#endif
  m_CharacterMgr.Animation();
#if defined(_DEBUG)
  if (l_bTraceFrameMove)
  {
    AppendCaptureLog("FrameMoveLoginLobby: CharacterMgr Animation end");
  }
#endif
  UpdateProjectionParameters();
#if defined(_DEBUG)
  if (l_bTraceFrameMove)
  {
    AppendCaptureLog("FrameMoveLoginLobby: end");
  }
#endif
}

void CMainApp::RenderLoginLobby(void)
{
  if (!m_isMapLoaded)
  {
    return;
  }

  Vector3f l_vecRenderPos;
  if (m_cLoginLobbyAniCamera.IsLoadedAniCamera())
  {
    l_vecRenderPos[0] = m_vecLoginLobbyCameraPos[0];
    l_vecRenderPos[1] = m_vecLoginLobbyCameraPos[1];
    l_vecRenderPos[2] = m_vecLoginLobbyCameraPos[2];
  }
  else
  {
    l_vecRenderPos[0] = m_cameraPosition[0];
    l_vecRenderPos[1] = m_cameraPosition[1];
    l_vecRenderPos[2] = m_cameraPosition[2];
  }

#if defined(_DEBUG)
  static DWORD s_dwRenderTraceCount = 0;
  const bool l_bTraceRender = (s_dwRenderTraceCount++ < 120);
  if (l_bTraceRender)
  {
    AppendCaptureLog("RenderLoginLobby: land Render begin");
  }
#endif
  m_land.Render(l_vecRenderPos);
#if defined(_DEBUG)
  if (l_bTraceRender)
  {
    AppendCaptureLog("RenderLoginLobby: land Render end");
    AppendCaptureLog("RenderLoginLobby: land RenderAlpha begin");
  }
#endif
  m_land.RenderAlpha(l_vecRenderPos);
#if defined(_DEBUG)
  if (l_bTraceRender)
  {
    AppendCaptureLog("RenderLoginLobby: land RenderAlpha end");
    AppendCaptureLog("RenderLoginLobby: CharacterMgr Render begin");
  }
#endif
  m_CharacterMgr.Render();
#if defined(_DEBUG)
  if (l_bTraceRender)
  {
    AppendCaptureLog("RenderLoginLobby: CharacterMgr Render end");
  }
#endif
}

HRESULT CMainApp::FrameMoveMainGame()
{
  R3CalculateTime();

  if (!m_isMapLoaded)
  {
    return S_OK;
  }

  UpdateCharacter();
  UpdateCamera();
  ApplyCamera();
  m_CharacterMgr.FrameMove();
  m_CharacterMgr.Animation();
  if (CLevel *l_pLevel = m_land.GetLevel())
  {
    l_pLevel->FrameMove();
  }
  UpdateProjectionParameters();
  return S_OK;
}

HRESULT CMainApp::RenderMainGame()
{
  static bool s_loggedPlayerProjection = false;

  if (!R3BeginScene())
  {
    return S_OK;
  }

  R3ClearFrameBuffer();
  if (m_isMapLoaded)
  {
    CLevel *l_pLevel = m_land.GetLevel();
    m_land.Render(m_cameraPosition);
    if (m_pPlayer && m_pPlayer->IsLoaded() && l_pLevel && l_pLevel->mBsp)
    {
      float l_vecPlayerPos[3];
      m_pPlayer->GetPosition(l_vecPlayerPos);
      m_pPlayer->SetLightColorFromMap(l_pLevel->mBsp->GetLightFromPoint(l_vecPlayerPos));
#if defined(_DEBUG)
      if (!s_loggedPlayerProjection)
      {
        D3DVIEWPORT8 l_viewport;
        D3DXMATRIX l_matView;
        D3DXMATRIX l_matProj;
        D3DXMATRIX l_matWorld;
        D3DXMatrixIdentity(&l_matWorld);

        if (SUCCEEDED(GetD3dDevice()->GetViewport(&l_viewport)) &&
            SUCCEEDED(GetD3dDevice()->GetTransform(D3DTS_VIEW, &l_matView)) &&
            SUCCEEDED(GetD3dDevice()->GetTransform(D3DTS_PROJECTION, &l_matProj)))
        {
          D3DXVECTOR3 l_vecSource(l_vecPlayerPos[0], l_vecPlayerPos[1], l_vecPlayerPos[2]);
          D3DXVECTOR3 l_vecProjected;
          D3DXVec3Project(&l_vecProjected,
                          &l_vecSource,
                          &l_viewport,
                          &l_matProj,
                          &l_matView,
                          &l_matWorld);
          AppendCaptureLog("Render: player projected screen=(%.2f, %.2f, %.4f) cameraPos=(%.2f, %.2f, %.2f) target=(%.2f, %.2f, %.2f) dist=%.2f",
                           l_vecProjected.x,
                           l_vecProjected.y,
                           l_vecProjected.z,
                           m_cameraPosition[0],
                           m_cameraPosition[1],
                           m_cameraPosition[2],
                           m_cameraTarget[0],
                           m_cameraTarget[1],
                           m_cameraTarget[2],
                           m_fPointDist);
          s_loggedPlayerProjection = true;
        }
      }
#endif
    }
    else
    {
      if (m_pPlayer)
      {
        m_pPlayer->SetLightColor(LIGHT_NORMAL_COLOR);
      }
    }

    if (!DrawPlayerShadowRender())
    {
      return S_OK;
    }

    m_CharacterMgr.Render();
    m_land.RenderAlpha(m_cameraPosition);
    m_cUI.Render(m_land, m_pPlayer, m_bKeyboardMoveMode, m_fViewAngleY);
  }

  R3EndScene();

#if defined(_DEBUG)
  static bool s_wroteDebugCapture = false;
  static int s_debugCaptureAttempts = 0;
  if (!s_wroteDebugCapture && m_isMapLoaded && s_debugCaptureAttempts < 5)
  {
    ++s_debugCaptureAttempts;
    s_wroteDebugCapture = SaveDebugFrameCapture(kDebugCapturePath);
  }
#endif

  return S_OK;
}

LRESULT CMainApp::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_ACTIVATEAPP:
      m_bIsActive = wParam ? TRUE : FALSE;
      break;

    case WM_CLOSE:
      m_bIsQuit = TRUE;
      PostQuitMessage(0);
      return 0;

    default:
      break;
  }

  if (IsAcceleratorExitCommand(message, wParam))
  {
    return 0;
  }

  if (!m_bIsActive && IsKeyboardMessage(message))
  {
    return 0;
  }

  if (m_pGameProgress)
  {
    const LRESULT l_nResult = m_pGameProgress->MsgProc(hWnd, message, wParam, lParam);
    if (l_nResult == 0)
    {
      return 0;
    }
  }

  return R3D3dWrapper::MsgProc(hWnd, message, wParam, lParam);
}

LRESULT CMainApp::MsgProcMainGame(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
    {
      const bool l_bIsAutoRepeat = (lParam & 0x40000000) != 0;
      const bool l_bShortcutAllowed = !IsShortcutModifierDown();

      if (wParam == VK_ESCAPE)
      {
        return 0;
      }

      if (wParam == VK_HOME)
      {
        ResetCameraFromLoadedMap();
        return 0;
      }

      if (wParam == VK_F12)
      {
        SaveDebugFrameCapture(kDebugCapturePath);
        return 0;
      }

      if (l_bShortcutAllowed && !l_bIsAutoRepeat && wParam == 'H')
      {
        m_bKeyboardMoveMode = !m_bKeyboardMoveMode;
#if defined(_DEBUG)
        AppendCaptureLog("MsgProc: move input mode=%s",
                         m_bKeyboardMoveMode ? "keyboard" : "mouse");
#endif
        return 0;
      }

      if (l_bShortcutAllowed && !l_bIsAutoRepeat && wParam == VK_OEM_1)
      {
        if (m_pPlayer)
        {
          m_pPlayer->SetWalkMode(m_pPlayer->GetWalkMode() == CMM_MOVE_RUN ? CMM_MOVE_WALK : CMM_MOVE_RUN);
        }
#if defined(_DEBUG)
        AppendCaptureLog("MsgProc: move mode=%s",
                         (!m_pPlayer || m_pPlayer->GetWalkMode() == CMM_MOVE_RUN) ? "run" : "walk");
#endif
        return 0;
      }

      break;
    }

    case WM_MOUSEWHEEL:
      m_pendingWheelDelta += GET_WHEEL_DELTA_WPARAM(wParam);
      return 0;

    case WM_CONTEXTMENU:
      return 0;

    default:
      break;
  }

  return R3D3dWrapper::MsgProc(hWnd, message, wParam, lParam);
}

bool CMainApp::ParseOfflineRegedCharReplay(LPCSTR commandLine)
{
  m_bOfflineRegedCharReplay = FALSE;
  m_szOfflineRegedCharPath[0] = '\0';

  if (!commandLine || !commandLine[0])
  {
    return false;
  }

  char l_szReplayPath[MAX_PATH] = {};
  if (!ExtractCommandOptionValue(commandLine,
                                 "--offline-reged-char",
                                 l_szReplayPath,
                                 sizeof(l_szReplayPath)) &&
      !ExtractCommandOptionValue(commandLine,
                                 "-offline-reged-char",
                                 l_szReplayPath,
                                 sizeof(l_szReplayPath)))
  {
    return false;
  }

  if (!l_szReplayPath[0])
  {
    strcpy_s(l_szReplayPath, sizeof(l_szReplayPath), ".\\_reports\\reged_char_result_last.bin");
  }

  strcpy_s(m_szOfflineRegedCharPath, sizeof(m_szOfflineRegedCharPath), l_szReplayPath);
  m_bOfflineRegedCharReplay = TRUE;
  m_byGameProgressID = GPI_LOGIN;
  return true;
}

bool CMainApp::LoadMapSelection(LPCSTR commandLine)
{
  char selectedMap[MAX_PATH];
  CopyMapSelection(selectedMap, sizeof(selectedMap), commandLine);
  return LoadMap(selectedMap);
}

bool CMainApp::LoadMap(const char *mapSelection)
{
  if (!mapSelection || !mapSelection[0])
  {
    return false;
  }

  char mapFilePath[MAX_PATH] = {};
  MAP_INFO *l_pMapInfo = NULL;
  bool l_bLoadByMapIndex = false;
  if (IsPathLike(mapSelection))
  {
    strcpy_s(mapFilePath, sizeof(mapFilePath), mapSelection);
  }
  else
  {
    l_pMapInfo = FindMapInfoBySelection(m_land, mapSelection);
    if (l_pMapInfo)
    {
      strcpy_s(mapFilePath, sizeof(mapFilePath), l_pMapInfo->pFileName);
      l_bLoadByMapIndex = true;
    }
    else
    {
      sprintf_s(mapFilePath, sizeof(mapFilePath), ".\\Map\\%s\\%s.bsp", mapSelection, mapSelection);
    }
  }

  if (!IsExistFile(mapFilePath))
  {
    return false;
  }

  if (m_isMapLoaded)
  {
    m_land.UnloadLevel();
    m_isMapLoaded = false;
  }

  strcpy_s(m_mapName, sizeof(m_mapName), mapFilePath);
  StripEXT(m_mapName);
  StripPath(m_mapName);

  bool l_bLoaded = false;
  if (l_bLoadByMapIndex && l_pMapInfo)
  {
    m_land.SetMapIndex(l_pMapInfo->byIndex);
    l_bLoaded = m_land.LoadLevel(l_pMapInfo->byIndex) != FALSE;
  }
  else
  {
    l_bLoaded = m_land.LoadLevel(mapFilePath) != FALSE;
  }

  CLevel *l_pLevel = m_land.GetLevel();
  if (!l_bLoaded || !l_pLevel || !l_pLevel->IsLoadedBsp())
  {
#if defined(_DEBUG)
    AppendCaptureLog("LoadMap: LoadLevel failed path=%s", mapFilePath);
#endif
    m_mapName[0] = '\0';
    return false;
  }

  m_camera.SetBspPtr(l_pLevel->mBsp);
  m_isMapLoaded = true;
#if defined(_DEBUG)
  AppendCaptureLog("LoadMap: success path=%s", mapFilePath);
#endif
  ResetCharacterFromLoadedMap();
  ResetCameraFromLoadedMap();
  return true;
}

void CMainApp::ResetCharacterFromLoadedMap()
{
  m_pPlayer = NULL;
  m_CharacterMgr.Clear();

  CLevel *l_pLevel = m_land.GetLevel();
  if (!m_isMapLoaded || !l_pLevel || !l_pLevel->mBsp || !l_pLevel->mBsp->mNode || l_pLevel->mBsp->mNodeNum <= 1)
  {
    return;
  }

#if defined(_DEBUG)
  AppendCaptureLog("ResetCharacterFromLoadedMap: begin");
#endif

  const _BSP_NODE &worldNode = l_pLevel->mBsp->mNode[1];
  float l_vecSpawnPos[3];
  float l_fFlatness = 0.0f;
  if (!FindCharacterSpawnPos(*l_pLevel, worldNode, l_vecSpawnPos, l_fFlatness))
  {
    l_vecSpawnPos[0] = (static_cast<float>(worldNode.bb_min[0]) + static_cast<float>(worldNode.bb_max[0])) * 0.5f;
    l_vecSpawnPos[2] = (static_cast<float>(worldNode.bb_min[2]) + static_cast<float>(worldNode.bb_max[2])) * 0.5f;
    l_vecSpawnPos[1] = static_cast<float>(worldNode.bb_max[1]);

    const float l_fGroundHeight = l_pLevel->GetFirstYpos(l_vecSpawnPos, 0);
    if (IsValidGroundHeight(l_fGroundHeight))
    {
      l_vecSpawnPos[1] = l_fGroundHeight;
    }
  }

  m_spawnCameraTarget[0] = l_vecSpawnPos[0];
  m_spawnCameraTarget[1] = l_vecSpawnPos[1] + 10.0f;
  m_spawnCameraTarget[2] = l_vecSpawnPos[2];
  m_hasSpawnCameraTarget = true;

  if (m_byGameProgressID == GPI_LOGIN && _stricmp(m_mapName, "lobby") == 0)
  {
    m_bKeyboardMoveMode = false;
#if defined(_DEBUG)
    AppendCaptureLog("ResetCharacterFromLoadedMap: skip sample player for login lobby");
#endif
    return;
  }

  m_pPlayer = m_CharacterMgr.AddPlayer(0);
  if (!m_pPlayer || !m_pPlayer->LoadAccretia())
  {
#if defined(_DEBUG)
    AppendCaptureLog("ResetCharacterFromLoadedMap: LoadAccretia failed");
    AppendCaptureLog("ResetCharacterFromLoadedMap: spawn focus=(%.2f, %.2f, %.2f) flatness=%.2f",
                     m_spawnCameraTarget[0],
                     m_spawnCameraTarget[1],
                     m_spawnCameraTarget[2],
                     l_fFlatness);
#endif
    return;
  }
#if defined(_DEBUG)
  AppendCaptureLog("ResetCharacterFromLoadedMap: LoadAccretia succeeded");
#endif

  m_pPlayer->SetPosition(l_vecSpawnPos[0], l_vecSpawnPos[1], l_vecSpawnPos[2]);
  m_pPlayer->SetWalkMode(CMM_MOVE_RUN);
  m_pPlayer->SetRotY(180.0f);
  m_bKeyboardMoveMode = false;
#if defined(_DEBUG)
  AppendCaptureLog("ResetCharacterFromLoadedMap: player pos=(%.2f, %.2f, %.2f) flatness=%.2f",
                   l_vecSpawnPos[0],
                   l_vecSpawnPos[1],
                   l_vecSpawnPos[2],
                   l_fFlatness);
#endif
}

void CMainApp::ResetCameraFromLoadedMap()
{
  CLevel *l_pLevel = m_land.GetLevel();
  if (!m_isMapLoaded || !l_pLevel || !l_pLevel->mBsp || !l_pLevel->mBsp->mNode || l_pLevel->mBsp->mNodeNum <= 1)
  {
    return;
  }

  const _BSP_NODE &worldNode = l_pLevel->mBsp->mNode[1];
  const float minX = static_cast<float>(worldNode.bb_min[0]);
  const float minY = static_cast<float>(worldNode.bb_min[1]);
  const float minZ = static_cast<float>(worldNode.bb_min[2]);
  const float maxX = static_cast<float>(worldNode.bb_max[0]);
  const float maxY = static_cast<float>(worldNode.bb_max[1]);
  const float maxZ = static_cast<float>(worldNode.bb_max[2]);
  const float spanX = maxX - minX;
  const float spanY = maxY - minY;
  const float spanZ = maxZ - minZ;

  const float horizontalSpan = MaxFloat(spanX, spanZ);
  const float mapDiagonal = sqrtf(spanX * spanX + spanY * spanY + spanZ * spanZ);
  m_fPointDist = ClampFloat(kDefaultCameraDistance, kMinCameraDistance, kMaxCameraDistance);
  m_mapFarClipDistance = ClampFloat(mapDiagonal * 1.5f, kMinMapFarClipDistance, kMaxMapFarClipDistance);
  m_fViewAngleX = 20.0f;
  m_fViewAngleY = 225.0f;
  m_fPrevViewAngleX = m_fViewAngleX;
  m_fPrevViewAngleY = m_fViewAngleY;
  m_isRightMouseDragging = false;
  m_nPrevX = gMouse.x;
  m_nPrevY = gMouse.y;
  m_pendingWheelDelta = 0;

  if (m_pPlayer && m_pPlayer->IsLoaded())
  {
    m_pPlayer->GetCameraTarget(m_cameraTarget);
    m_fPointDist = ClampFloat(kDefaultCameraDistance, kMinCameraDistance, kMaxCameraDistance);
    m_fViewAngleX = 20.0f;

    if (l_pLevel->mBsp)
    {
      const float l_afPitchCandidate[] = {20.0f, 28.0f, 35.0f};
      const float l_afYawOffset[] = {0.0f, 90.0f, -90.0f, 180.0f, 45.0f, -45.0f, 135.0f, -135.0f};
      bool l_bFoundClearView = false;
      for (int i = 0; !l_bFoundClearView && i < static_cast<int>(sizeof(l_afPitchCandidate) / sizeof(l_afPitchCandidate[0])); ++i)
      {
        for (int j = 0; j < static_cast<int>(sizeof(l_afYawOffset) / sizeof(l_afYawOffset[0])); ++j)
        {
          R3Camera l_cameraCandidate;
          l_cameraCandidate.SetPoint(m_cameraTarget[0], m_cameraTarget[1], m_cameraTarget[2]);
          l_cameraCandidate.SetThPhDist(l_afPitchCandidate[i], m_fViewAngleY + l_afYawOffset[j], m_fPointDist);
          l_cameraCandidate.MakeCameraAndViewMatrix();

          Vector3f l_vecCameraPos;
          l_cameraCandidate.GetPos(l_vecCameraPos);
          if (!l_pLevel->mBsp->IsCollisionFace(m_cameraTarget, l_vecCameraPos))
          {
            m_fViewAngleX = l_afPitchCandidate[i];
            m_fViewAngleY += l_afYawOffset[j];
            l_bFoundClearView = true;
            break;
          }
        }
      }
    }

    m_pPlayer->SetRotY(NormalizeAngle360(360.0f - m_fViewAngleY));
  }
  else
  {
    if (m_hasSpawnCameraTarget)
    {
      m_cameraTarget[0] = m_spawnCameraTarget[0];
      m_cameraTarget[1] = m_spawnCameraTarget[1];
      m_cameraTarget[2] = m_spawnCameraTarget[2];
      m_fPointDist = kDefaultCameraDistance;
      m_fViewAngleX = 20.0f;
    }
    else
    {
      m_cameraTarget[0] = (minX + maxX) * 0.5f;
      m_cameraTarget[1] = (minY + maxY) * 0.5f;
      m_cameraTarget[2] = (minZ + maxZ) * 0.5f;
    }
  }

  m_camera.SetPoint(m_cameraTarget[0], m_cameraTarget[1], m_cameraTarget[2]);
  m_camera.SetThPhDist(m_fViewAngleX, m_fViewAngleY, m_fPointDist);
  m_camera.MakeCameraAndViewMatrix();
  m_camera.GetPos(m_cameraPosition);
  l_pLevel->SetCameraPos(m_cameraPosition);
  l_pLevel->SetViewMatrix(R3MoveGetViewMatrix());
  UpdateProjectionParameters();
}

void CMainApp::UpdateCharacter()
{
  const float loopTime = R3GetLoopTime();
  CLevel *l_pLevel = m_land.GetLevel();
  if (!m_pPlayer || !m_pPlayer->IsLoaded() || !l_pLevel || !l_pLevel->mBsp)
  {
    return;
  }

  Vector3f l_vecPlayerPos;
  m_pPlayer->GetPosition(l_vecPlayerPos);

  const BYTE l_byWalkMode = m_pPlayer->GetWalkMode();
  const bool l_bAlphabeticShortcutAllowed = !IsShortcutModifierDown();
  const bool l_bMoveForward = m_bKeyboardMoveMode ? (l_bAlphabeticShortcutAllowed && IsKeyDown('W')) : IsKeyDown(VK_UP);
  const bool l_bMoveBackward = m_bKeyboardMoveMode ? (l_bAlphabeticShortcutAllowed && IsKeyDown('S')) : IsKeyDown(VK_DOWN);
  const bool l_bMoveLeft = m_bKeyboardMoveMode ? (l_bAlphabeticShortcutAllowed && IsKeyDown('A')) : IsKeyDown(VK_LEFT);
  const bool l_bMoveRight = m_bKeyboardMoveMode ? (l_bAlphabeticShortcutAllowed && IsKeyDown('D')) : IsKeyDown(VK_RIGHT);

  float l_fMoveRotY = m_pPlayer->GetRotY();
  float l_fFacingRotY = m_pPlayer->GetRotY();
  if (m_bKeyboardMoveMode)
  {
    if (!GetKeyboardModeMoveRotY(l_bMoveForward,
                                 l_bMoveBackward,
                                 l_bMoveLeft,
                                 l_bMoveRight,
                                 m_fViewAngleY,
                                 l_fFacingRotY,
                                 l_fMoveRotY))
    {
      m_pPlayer->SetMoveMode(false, l_byWalkMode);
      return;
    }
  }
  else if (!GetMouseModeMoveRotY(l_bMoveForward,
                                 l_bMoveBackward,
                                 l_bMoveLeft,
                                 l_bMoveRight,
                                 m_fViewAngleY,
                                 l_fMoveRotY))
  {
    m_pPlayer->SetMoveMode(false, l_byWalkMode);
    return;
  }

  BYTE l_byAppliedMoveMode = l_byWalkMode;
  if (m_bKeyboardMoveMode && l_bMoveBackward)
  {
    l_byAppliedMoveMode = CMM_MOVE_WALK;
  }

  const float l_fMovementStep = GetMoveSpeed(l_byAppliedMoveMode) * loopTime;

  Vector3f l_vecTargetPos;
  l_vecTargetPos[0] = l_vecPlayerPos[0] + (sinf(AngleToPi(l_fMoveRotY)) * l_fMovementStep);
  l_vecTargetPos[1] = l_vecPlayerPos[1];
  l_vecTargetPos[2] = l_vecPlayerPos[2] + (cosf(AngleToPi(l_fMoveRotY)) * l_fMovementStep);

  Vector3f l_vecReachablePos;
  if (!l_pLevel->mBsp->CanYouGoThere(l_vecPlayerPos, l_vecTargetPos, &l_vecReachablePos))
  {
    l_vecTargetPos[0] = l_vecReachablePos[0];
    l_vecTargetPos[2] = l_vecReachablePos[2];
  }

  float l_fResolvedY = l_vecPlayerPos[1];
  if (l_pLevel->GetNextYposForServerFar(l_vecPlayerPos, l_vecTargetPos, &l_fResolvedY) ||
      l_pLevel->GetNextYposFarProgress(l_vecPlayerPos, l_vecTargetPos, &l_fResolvedY))
  {
    l_vecTargetPos[1] = l_fResolvedY;
  }
  else
  {
    const float l_fGroundHeight = l_pLevel->GetFirstYpos(l_vecTargetPos, 0);
    if (IsValidGroundHeight(l_fGroundHeight))
    {
      l_vecTargetPos[1] = l_fGroundHeight;
    }
  }

  const float l_fAppliedMoveX = l_vecTargetPos[0] - l_vecPlayerPos[0];
  const float l_fAppliedMoveZ = l_vecTargetPos[2] - l_vecPlayerPos[2];
  if ((l_fAppliedMoveX * l_fAppliedMoveX) + (l_fAppliedMoveZ * l_fAppliedMoveZ) <= 0.0001f)
  {
    m_pPlayer->SetMoveMode(false, l_byWalkMode);
    return;
  }

  m_pPlayer->SetPosition(l_vecTargetPos[0], l_vecTargetPos[1], l_vecTargetPos[2]);
  m_pPlayer->SetRotY(m_bKeyboardMoveMode ? l_fFacingRotY : l_fMoveRotY);
  m_pPlayer->SetMoveMode(true,
                         l_byAppliedMoveMode,
                         m_bKeyboardMoveMode,
                         l_fFacingRotY,
                         l_fMoveRotY);
}

void CMainApp::UpdateCamera()
{
  const float loopTime = R3GetLoopTime();
  const float rotationStep = 90.0f * loopTime;
  const float zoomStep = 25.0f * loopTime;
  const bool isRightMouseDown = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;

  if (m_pPlayer && m_pPlayer->IsLoaded())
  {
    m_pPlayer->GetCameraTarget(m_cameraTarget);
  }

  if (isRightMouseDown)
  {
    if (!m_isRightMouseDragging)
    {
      m_isRightMouseDragging = true;
      m_nPrevX = gMouse.x;
      m_nPrevY = gMouse.y;
    }

    if ((gMouse.x != m_nPrevX) || (gMouse.y != m_nPrevY))
    {
      const float xRate = (GetR3State()->mScreenXsize / 1024.0f) * 0.4f;
      const float yRate = (GetR3State()->mScreenYsize / 768.0f) * 0.6f;
      const int deltaX = gMouse.x - m_nPrevX;
      const int deltaY = gMouse.y - m_nPrevY;

      m_fViewAngleX += (static_cast<float>(deltaY) / 2.0f) * xRate;
      m_fViewAngleY -= (static_cast<float>(deltaX) / 2.0f) * yRate;
    }

    m_nPrevX = gMouse.x;
    m_nPrevY = gMouse.y;
  }
  else
  {
    m_isRightMouseDragging = false;
    m_fPrevViewAngleX = m_fViewAngleX;
    m_fPrevViewAngleY = m_fViewAngleY;
    m_nPrevX = gMouse.x;
    m_nPrevY = gMouse.y;
  }

  const bool l_bAlphabeticShortcutAllowed = !IsShortcutModifierDown();
  if (l_bAlphabeticShortcutAllowed && IsKeyDown('Q'))
  {
    m_fViewAngleY += rotationStep;
  }
  if (l_bAlphabeticShortcutAllowed && IsKeyDown('E'))
  {
    m_fViewAngleY -= rotationStep;
  }

  if (m_bKeyboardMoveMode && IsKeyDown(VK_LEFT))
  {
    m_fViewAngleY += rotationStep;
  }
  if (m_bKeyboardMoveMode && IsKeyDown(VK_RIGHT))
  {
    m_fViewAngleY -= rotationStep;
  }
  if (m_bKeyboardMoveMode && IsKeyDown(VK_UP))
  {
    m_fViewAngleX += rotationStep * 0.5f;
  }
  if (m_bKeyboardMoveMode && IsKeyDown(VK_DOWN))
  {
    m_fViewAngleX -= rotationStep * 0.5f;
  }

  if (IsKeyDown(VK_PRIOR))
  {
    m_fPointDist -= zoomStep;
  }
  if (IsKeyDown(VK_NEXT))
  {
    m_fPointDist += zoomStep;
  }

  if (m_pendingWheelDelta)
  {
    m_fPointDist -= static_cast<float>(m_pendingWheelDelta) * kMouseWheelDistanceRate;
    m_pendingWheelDelta = 0;
  }

  while (m_fViewAngleY >= 360.0f)
  {
    m_fViewAngleY -= 360.0f;
  }
  while (m_fViewAngleY < 0.0f)
  {
    m_fViewAngleY += 360.0f;
  }

  m_fViewAngleX = ClampFloat(m_fViewAngleX, kMinViewAngleX, kMaxViewAngleX);
  m_fPointDist = ClampFloat(m_fPointDist, kMinCameraDistance, kMaxCameraDistance);
}

bool CMainApp::DrawPlayerShadowRender()
{
  CLevel *l_pLevel = m_land.GetLevel();
  if (!m_pPlayer || !m_pPlayer->IsLoaded() || !l_pLevel || !l_pLevel->mBsp)
  {
    return true;
  }

  float l_vecPlayerPos[3];
  m_pPlayer->GetPosition(l_vecPlayerPos);

  float l_fScale = 1.0f;
  float l_fAddPos = 10.0f;
  m_pPlayer->GetScaleAddPos(&l_fScale, &l_fAddPos);

  R3EndScene();
  ShadowBeginScene();
  PrepareShadow(l_fScale, l_fAddPos);
  m_pPlayer->CreateShadow();
  ShadowEndScene();

  l_pLevel->PrepareShadowRender(l_vecPlayerPos, GetShadowTexture(), 0.8f, 3, l_fScale, l_fAddPos);
  if (!R3BeginScene())
  {
    return false;
  }

  _R3ENGINE_STATE *state = GetR3State();
  if (state && GetD3dDevice())
  {
    GetD3dDevice()->SetTransform(D3DTS_VIEW, &state->mMatView);
    GetD3dDevice()->SetTransform(D3DTS_PROJECTION, &state->mMatProj);
    if (state->mIsFog)
    {
      GetD3dDevice()->SetRenderState(D3DRS_FOGENABLE, TRUE);
    }
  }

  l_pLevel->SetCameraPos(m_cameraPosition);
  l_pLevel->SetViewMatrix(R3MoveGetViewMatrix());
  l_pLevel->DrawShadowRender(l_vecPlayerPos);
  return true;
}

void CMainApp::ApplyCamera()
{
  _R3ENGINE_STATE *state = GetR3State();
  CLevel *l_pLevel = m_land.GetLevel();
  m_camera.SetPoint(m_cameraTarget[0], m_cameraTarget[1], m_cameraTarget[2]);
  m_camera.SetThPhDist(m_fViewAngleX, m_fViewAngleY, m_fPointDist);
  m_camera.MakeCameraAndViewMatrix();
  m_camera.GetPos(m_cameraPosition);

  if (state && l_pLevel && l_pLevel->mBsp)
  {
    Vector3f l_vecAdjustedCameraPos;
    if (l_pLevel->mBsp->IsCollisionFace(m_cameraTarget, m_cameraPosition, &l_vecAdjustedCameraPos, 2.0f))
    {
      m_cameraPosition[0] = l_vecAdjustedCameraPos[0];
      m_cameraPosition[1] = l_vecAdjustedCameraPos[1];
      m_cameraPosition[2] = l_vecAdjustedCameraPos[2];

      D3DXVECTOR3 l_vecEye(m_cameraPosition[0], m_cameraPosition[1], m_cameraPosition[2]);
      D3DXVECTOR3 l_vecAt(m_cameraTarget[0], m_cameraTarget[1], m_cameraTarget[2]);
      D3DXVECTOR3 l_vecUp(0.0f, 1.0f, 0.0f);
      D3DXMatrixLookAtLH(&state->mMatView, &l_vecEye, &l_vecAt, &l_vecUp);
      MatrixMultiply(state->mMatViewProj.m, state->mMatProj.m, state->mMatView.m);
      D3DXMatrixInverse(&state->mInvMatView, NULL, &state->mMatView);
      if (GetD3dDevice())
      {
        GetD3dDevice()->SetTransform(D3DTS_VIEW, &state->mMatView);
      }
    }
  }

  if (l_pLevel)
  {
    l_pLevel->SetCameraPos(m_cameraPosition);
    l_pLevel->SetViewMatrix(R3MoveGetViewMatrix());
  }
}

void CMainApp::UpdateProjectionParameters()
{
  _R3ENGINE_STATE *state = GetR3State();
  if (!state || state->mScreenYsize <= 0.0f)
  {
    return;
  }

  state->mAsfectRatio = state->mScreenXsize / state->mScreenYsize;
  if (m_isMapLoaded && state->mFar < m_mapFarClipDistance)
  {
    state->mFar = m_mapFarClipDistance;
  }

  D3DXMatrixPerspectiveFovLH(&state->mMatProj, state->mFov, state->mAsfectRatio, state->mNear, state->mFar);
  if (GetD3dDevice())
  {
    GetD3dDevice()->SetTransform(D3DTS_PROJECTION, &state->mMatProj);
  }
  MatrixMultiply(state->mMatViewProj.m, state->mMatProj.m, state->mMatView.m);
}

bool CMainApp::SaveDebugFrameCapture(const char *fileName)
{
  if (!fileName || !GetD3dDevice())
  {
#if defined(_DEBUG)
    AppendCaptureLog("SaveDebugFrameCapture: invalid input device=%p file=%s",
                     GetD3dDevice(),
                     fileName ? fileName : "<null>");
#endif
    return false;
  }

  LPDIRECT3DSURFACE8 backBuffer = NULL;
  HRESULT hr = GetD3dDevice()->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);
  if (FAILED(hr))
  {
#if defined(_DEBUG)
    AppendCaptureLog("SaveDebugFrameCapture: GetBackBuffer failed hr=0x%08X", hr);
#endif
    return false;
  }

  D3DSURFACE_DESC backBufferDesc;
  hr = backBuffer->GetDesc(&backBufferDesc);
  if (FAILED(hr))
  {
#if defined(_DEBUG)
    AppendCaptureLog("SaveDebugFrameCapture: GetDesc failed hr=0x%08X", hr);
#endif
    backBuffer->Release();
    return false;
  }

  LPDIRECT3DSURFACE8 captureSurface = NULL;
  hr = GetD3dDevice()->CreateImageSurface(backBufferDesc.Width, backBufferDesc.Height, backBufferDesc.Format, &captureSurface);
  if (FAILED(hr))
  {
#if defined(_DEBUG)
    AppendCaptureLog("SaveDebugFrameCapture: CreateImageSurface failed hr=0x%08X format=%u size=%ux%u",
                     hr,
                     static_cast<unsigned>(backBufferDesc.Format),
                     static_cast<unsigned>(backBufferDesc.Width),
                     static_cast<unsigned>(backBufferDesc.Height));
#endif
    backBuffer->Release();
    return false;
  }

  bool saved = false;
  hr = GetD3dDevice()->CopyRects(backBuffer, NULL, 0, captureSurface, NULL);
  if (SUCCEEDED(hr))
  {
    saved = SaveSurfaceToBMP(fileName, captureSurface);
#if defined(_DEBUG)
    AppendCaptureLog("SaveDebugFrameCapture: CopyRects succeeded format=%u size=%ux%u saved=%d",
                     static_cast<unsigned>(backBufferDesc.Format),
                     static_cast<unsigned>(backBufferDesc.Width),
                     static_cast<unsigned>(backBufferDesc.Height),
                     saved ? 1 : 0);
#endif
  }
  else
  {
#if defined(_DEBUG)
    AppendCaptureLog("SaveDebugFrameCapture: CopyRects failed hr=0x%08X", hr);
#endif
    hr = GetD3dDevice()->GetFrontBuffer(captureSurface);
    if (SUCCEEDED(hr))
    {
      saved = SaveSurfaceToBMP(fileName, captureSurface);
#if defined(_DEBUG)
      AppendCaptureLog("SaveDebugFrameCapture: GetFrontBuffer succeeded saved=%d", saved ? 1 : 0);
#endif
    }
#if defined(_DEBUG)
    else
    {
      AppendCaptureLog("SaveDebugFrameCapture: GetFrontBuffer failed hr=0x%08X", hr);
    }
#endif
  }

  backBuffer->Release();
  captureSurface->Release();
  return saved;
}

bool CMainApp::IsKeyDown(int virtualKey)
{
  return virtualKey >= 0 && virtualKey < 256 && (GetAsyncKeyState(virtualKey) & 0x8000) != 0;
}
