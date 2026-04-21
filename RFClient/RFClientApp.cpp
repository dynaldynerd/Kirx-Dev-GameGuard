#include "RFClientApp.h"

#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <vector>

#include "R3Engine/1stclass/core.h"
#include "R3Engine/1stclass/r3d3d8.h"
#include "R3Engine/common/commonutil.h"
#include "R3Engine/common/r3input.h"
#include "R3Engine/common/r3math.h"
#include "R3Engine/2ndclass/r3enginekernel.h"
#include "R3Engine/2ndclass/r3move.h"
#include "R3Engine/2ndclass/r3util.h"

namespace
{
constexpr char kDefaultMapSelection[] = "NeutralB";
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
constexpr char kDebugCapturePath[] = "D:\\Private Files\\playrf\\RFOnline\\RFClientCapture.bmp";
constexpr char kDebugCaptureLogPath[] = "D:\\Private Files\\playrf\\RFOnline\\RFClientCapture.log";

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
  }
  else if (pi_bMoveForward && pi_bMoveRight)
  {
    po_fMoveRotY = po_fFacingRotY + 45.0f;
  }
  else if (pi_bMoveBackward && pi_bMoveLeft)
  {
    po_fMoveRotY = po_fFacingRotY - 135.0f;
  }
  else if (pi_bMoveBackward && pi_bMoveRight)
  {
    po_fMoveRotY = po_fFacingRotY + 135.0f;
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

void CopyMapSelection(char *destination, size_t destinationSize, LPCSTR commandLine)
{
  if (!destination || !destinationSize)
  {
    return;
  }

  destination[0] = '\0';
  if (!commandLine)
  {
    strcpy_s(destination, destinationSize, kDefaultMapSelection);
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

  if (!destination[0])
  {
    strcpy_s(destination, destinationSize, kDefaultMapSelection);
  }
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

CRFClientApp::CRFClientApp()
  : m_isInitialized(false),
    m_isMapLoaded(false),
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
  m_cameraTarget[0] = 0.0f;
  m_cameraTarget[1] = 0.0f;
  m_cameraTarget[2] = 0.0f;
  m_cameraPosition[0] = 0.0f;
  m_cameraPosition[1] = 0.0f;
  m_cameraPosition[2] = 0.0f;
  m_spawnCameraTarget[0] = 0.0f;
  m_spawnCameraTarget[1] = 0.0f;
  m_spawnCameraTarget[2] = 0.0f;
  m_camera.SetBspPtr(m_level.mBsp);
  m_strWindowTitle = const_cast<char *>("RF_Online");
}

CRFClientApp::~CRFClientApp()
{
  Shutdown();
}

bool CRFClientApp::Initialize(HINSTANCE hInstance, LPCSTR commandLine)
{
  if (m_isInitialized)
  {
    return true;
  }

#if defined(_DEBUG)
  AppendCaptureLog("Initialize: begin commandLine=%s", commandLine ? commandLine : "<null>");
#endif
  InitR3Engine();
  if (FAILED(InitR3D3D(hInstance, 0)))
  {
#if defined(_DEBUG)
    AppendCaptureLog("Initialize: InitR3D3D failed");
#endif
    ReleaseR3Engine();
    return false;
  }

  if (!m_player.Initialize(GetD3dDevice()))
  {
#if defined(_DEBUG)
    AppendCaptureLog("Initialize: m_player.Initialize failed");
#endif
    ReleaseR3D3D();
    ReleaseR3Engine();
    return false;
  }

  if (!LoadMapSelection(commandLine))
  {
#if defined(_DEBUG)
    AppendCaptureLog("Initialize: LoadMapSelection failed");
#endif
    ReleaseR3D3D();
    ReleaseR3Engine();
    return false;
  }

#if defined(_DEBUG)
  AppendCaptureLog("Initialize: success mapLoaded=%d", m_isMapLoaded ? 1 : 0);
#endif
  m_isInitialized = true;
  return true;
}

int CRFClientApp::Run()
{
  return CD3DApplication::Run();
}

void CRFClientApp::Shutdown()
{
  if (!m_isInitialized && !m_isMapLoaded)
  {
    return;
  }

  if (m_isMapLoaded)
  {
    m_level.ReleaseLevel();
    m_isMapLoaded = false;
  }

  m_player.Shutdown();
  ReleaseR3D3D();
  ReleaseR3Engine();
  m_isInitialized = false;
}

HRESULT CRFClientApp::FrameMove()
{
  R3CalculateTime();

  if (!m_isMapLoaded)
  {
    return S_OK;
  }

  UpdateCharacter();
  UpdateCamera();
  ApplyCamera();
  m_player.FrameMove();
  m_level.FrameMove();
  UpdateProjectionParameters();
  return S_OK;
}

HRESULT CRFClientApp::Render()
{
  static bool s_loggedPlayerProjection = false;

  if (!R3BeginScene())
  {
    return S_OK;
  }

  R3ClearFrameBuffer();
  if (m_isMapLoaded)
  {
    m_level.SetCameraPos(m_cameraPosition);
    m_level.SetViewMatrix(R3MoveGetViewMatrix());
    m_level.DrawSkyBoxRender();
    m_level.DrawMapRender();
    m_level.DrawMapEntitiesRender();
    if (m_player.IsLoaded() && m_level.mBsp)
    {
      float l_vecPlayerPos[3];
      m_player.GetPosition(l_vecPlayerPos);
      m_player.SetMapColor(m_level.mBsp->GetLightFromPoint(l_vecPlayerPos));
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
      m_player.SetMapColor(0xFFFFFFFF);
    }

    if (!DrawPlayerShadowRender())
    {
      return S_OK;
    }

    m_player.Render();
    m_level.DrawMapAlphaRender(m_cameraPosition);
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

LRESULT CRFClientApp::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_KEYDOWN:
    {
      const bool l_bIsAutoRepeat = (lParam & 0x40000000) != 0;

      if (wParam == VK_ESCAPE)
      {
        PostMessageA(hWnd, WM_CLOSE, 0, 0);
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

      if (!l_bIsAutoRepeat && wParam == 'H')
      {
        m_bKeyboardMoveMode = !m_bKeyboardMoveMode;
#if defined(_DEBUG)
        AppendCaptureLog("MsgProc: move input mode=%s",
                         m_bKeyboardMoveMode ? "keyboard" : "mouse");
#endif
        return 0;
      }

      if (!l_bIsAutoRepeat && wParam == VK_OEM_1)
      {
        m_player.SetWalkMode(m_player.GetWalkMode() == CMM_MOVE_RUN ? CMM_MOVE_WALK : CMM_MOVE_RUN);
#if defined(_DEBUG)
        AppendCaptureLog("MsgProc: move mode=%s",
                         m_player.GetWalkMode() == CMM_MOVE_RUN ? "run" : "walk");
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

bool CRFClientApp::LoadMapSelection(LPCSTR commandLine)
{
  char selectedMap[MAX_PATH];
  CopyMapSelection(selectedMap, sizeof(selectedMap), commandLine);
  return LoadMap(selectedMap);
}

bool CRFClientApp::LoadMap(const char *mapSelection)
{
  if (!mapSelection || !mapSelection[0])
  {
    return false;
  }

  char mapFilePath[MAX_PATH];
  if (IsPathLike(mapSelection))
  {
    strcpy_s(mapFilePath, sizeof(mapFilePath), mapSelection);
  }
  else
  {
    sprintf_s(mapFilePath, sizeof(mapFilePath), ".\\Map\\%s\\%s.bsp", mapSelection, mapSelection);
  }

  if (!IsExistFile(mapFilePath))
  {
    return false;
  }

  if (m_isMapLoaded)
  {
    m_level.ReleaseLevel();
    m_isMapLoaded = false;
  }

  strcpy_s(m_mapName, sizeof(m_mapName), mapFilePath);
  StripEXT(m_mapName);
  StripPath(m_mapName);

  m_level.LoadLevel(mapFilePath);
  if (!m_level.IsLoadedBsp())
  {
#if defined(_DEBUG)
    AppendCaptureLog("LoadMap: LoadLevel failed path=%s", mapFilePath);
#endif
    m_mapName[0] = '\0';
    return false;
  }

  m_camera.SetBspPtr(m_level.mBsp);
  m_isMapLoaded = true;
#if defined(_DEBUG)
  AppendCaptureLog("LoadMap: success path=%s", mapFilePath);
#endif
  ResetCharacterFromLoadedMap();
  ResetCameraFromLoadedMap();
  return true;
}

void CRFClientApp::ResetCharacterFromLoadedMap()
{
  if (!m_isMapLoaded || !m_level.mBsp || !m_level.mBsp->mNode || m_level.mBsp->mNodeNum <= 1)
  {
    return;
  }

#if defined(_DEBUG)
  AppendCaptureLog("ResetCharacterFromLoadedMap: begin");
#endif

  const _BSP_NODE &worldNode = m_level.mBsp->mNode[1];
  float l_vecSpawnPos[3];
  float l_fFlatness = 0.0f;
  if (!FindCharacterSpawnPos(m_level, worldNode, l_vecSpawnPos, l_fFlatness))
  {
    l_vecSpawnPos[0] = (static_cast<float>(worldNode.bb_min[0]) + static_cast<float>(worldNode.bb_max[0])) * 0.5f;
    l_vecSpawnPos[2] = (static_cast<float>(worldNode.bb_min[2]) + static_cast<float>(worldNode.bb_max[2])) * 0.5f;
    l_vecSpawnPos[1] = static_cast<float>(worldNode.bb_max[1]);

    const float l_fGroundHeight = m_level.GetFirstYpos(l_vecSpawnPos, 0);
    if (IsValidGroundHeight(l_fGroundHeight))
    {
      l_vecSpawnPos[1] = l_fGroundHeight;
    }
  }

  m_spawnCameraTarget[0] = l_vecSpawnPos[0];
  m_spawnCameraTarget[1] = l_vecSpawnPos[1] + 10.0f;
  m_spawnCameraTarget[2] = l_vecSpawnPos[2];
  m_hasSpawnCameraTarget = true;

  if (!m_player.LoadAccretia())
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

  m_player.SetPosition(l_vecSpawnPos[0], l_vecSpawnPos[1], l_vecSpawnPos[2]);
  m_player.SetWalkMode(CMM_MOVE_RUN);
  m_player.SetRotY(180.0f);
  m_bKeyboardMoveMode = false;
#if defined(_DEBUG)
  AppendCaptureLog("ResetCharacterFromLoadedMap: player pos=(%.2f, %.2f, %.2f) flatness=%.2f",
                   l_vecSpawnPos[0],
                   l_vecSpawnPos[1],
                   l_vecSpawnPos[2],
                   l_fFlatness);
#endif
}

void CRFClientApp::ResetCameraFromLoadedMap()
{
  if (!m_isMapLoaded || !m_level.mBsp || !m_level.mBsp->mNode || m_level.mBsp->mNodeNum <= 1)
  {
    return;
  }

  const _BSP_NODE &worldNode = m_level.mBsp->mNode[1];
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

  if (m_player.IsLoaded())
  {
    m_player.GetCameraTarget(m_cameraTarget);
    m_fPointDist = ClampFloat(kDefaultCameraDistance, kMinCameraDistance, kMaxCameraDistance);
    m_fViewAngleX = 20.0f;

    if (m_level.mBsp)
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
          if (!m_level.mBsp->IsCollisionFace(m_cameraTarget, l_vecCameraPos))
          {
            m_fViewAngleX = l_afPitchCandidate[i];
            m_fViewAngleY += l_afYawOffset[j];
            l_bFoundClearView = true;
            break;
          }
        }
      }
    }

    m_player.SetRotY(NormalizeAngle360(360.0f - m_fViewAngleY));
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
  m_level.SetCameraPos(m_cameraPosition);
  m_level.SetViewMatrix(R3MoveGetViewMatrix());
  UpdateProjectionParameters();
}

void CRFClientApp::UpdateCharacter()
{
  const float loopTime = R3GetLoopTime();
  if (!m_player.IsLoaded() || !m_level.mBsp)
  {
    return;
  }

  Vector3f l_vecPlayerPos;
  m_player.GetPosition(l_vecPlayerPos);

  const BYTE l_byWalkMode = m_player.GetWalkMode();
  const bool l_bMoveForward = m_bKeyboardMoveMode ? IsKeyDown('W') : IsKeyDown(VK_UP);
  const bool l_bMoveBackward = m_bKeyboardMoveMode ? IsKeyDown('S') : IsKeyDown(VK_DOWN);
  const bool l_bMoveLeft = m_bKeyboardMoveMode ? IsKeyDown('A') : IsKeyDown(VK_LEFT);
  const bool l_bMoveRight = m_bKeyboardMoveMode ? IsKeyDown('D') : IsKeyDown(VK_RIGHT);

  float l_fMoveRotY = m_player.GetRotY();
  float l_fFacingRotY = m_player.GetRotY();
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
      m_player.SetMoveMode(false, l_byWalkMode);
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
    m_player.SetMoveMode(false, l_byWalkMode);
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
  if (!m_level.mBsp->CanYouGoThere(l_vecPlayerPos, l_vecTargetPos, &l_vecReachablePos))
  {
    l_vecTargetPos[0] = l_vecReachablePos[0];
    l_vecTargetPos[2] = l_vecReachablePos[2];
  }

  float l_fResolvedY = l_vecPlayerPos[1];
  if (m_level.GetNextYposForServerFar(l_vecPlayerPos, l_vecTargetPos, &l_fResolvedY) ||
      m_level.GetNextYposFarProgress(l_vecPlayerPos, l_vecTargetPos, &l_fResolvedY))
  {
    l_vecTargetPos[1] = l_fResolvedY;
  }
  else
  {
    const float l_fGroundHeight = m_level.GetFirstYpos(l_vecTargetPos, 0);
    if (IsValidGroundHeight(l_fGroundHeight))
    {
      l_vecTargetPos[1] = l_fGroundHeight;
    }
  }

  const float l_fAppliedMoveX = l_vecTargetPos[0] - l_vecPlayerPos[0];
  const float l_fAppliedMoveZ = l_vecTargetPos[2] - l_vecPlayerPos[2];
  if ((l_fAppliedMoveX * l_fAppliedMoveX) + (l_fAppliedMoveZ * l_fAppliedMoveZ) <= 0.0001f)
  {
    m_player.SetMoveMode(false, l_byWalkMode);
    return;
  }

  m_player.SetPosition(l_vecTargetPos[0], l_vecTargetPos[1], l_vecTargetPos[2]);
  m_player.SetRotY(m_bKeyboardMoveMode ? l_fFacingRotY : l_fMoveRotY);
  m_player.SetMoveMode(true, l_byAppliedMoveMode);
}

void CRFClientApp::UpdateCamera()
{
  const float loopTime = R3GetLoopTime();
  const float rotationStep = 90.0f * loopTime;
  const float zoomStep = 25.0f * loopTime;
  const bool isRightMouseDown = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;

  if (m_player.IsLoaded())
  {
    m_player.GetCameraTarget(m_cameraTarget);
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

  if (IsKeyDown('Q'))
  {
    m_fViewAngleY += rotationStep;
  }
  if (IsKeyDown('E'))
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

bool CRFClientApp::DrawPlayerShadowRender()
{
  if (!m_player.IsLoaded() || !m_level.mBsp)
  {
    return true;
  }

  float l_vecPlayerPos[3];
  m_player.GetPosition(l_vecPlayerPos);

  float l_fScale = 1.0f;
  float l_fAddPos = 10.0f;
  m_player.GetScaleAddPos(&l_fScale, &l_fAddPos);

  R3EndScene();
  ShadowBeginScene();
  PrepareShadow(l_fScale, l_fAddPos);
  m_player.CreateShadow();
  ShadowEndScene();

  m_level.PrepareShadowRender(l_vecPlayerPos, GetShadowTexture(), 0.8f, 3, l_fScale, l_fAddPos);
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

  m_level.SetCameraPos(m_cameraPosition);
  m_level.SetViewMatrix(R3MoveGetViewMatrix());
  m_level.DrawShadowRender(l_vecPlayerPos);
  return true;
}

void CRFClientApp::ApplyCamera()
{
  _R3ENGINE_STATE *state = GetR3State();
  m_camera.SetPoint(m_cameraTarget[0], m_cameraTarget[1], m_cameraTarget[2]);
  m_camera.SetThPhDist(m_fViewAngleX, m_fViewAngleY, m_fPointDist);
  m_camera.MakeCameraAndViewMatrix();
  m_camera.GetPos(m_cameraPosition);

  if (state && m_level.mBsp)
  {
    Vector3f l_vecAdjustedCameraPos;
    if (m_level.mBsp->IsCollisionFace(m_cameraTarget, m_cameraPosition, &l_vecAdjustedCameraPos, 2.0f))
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

  m_level.SetCameraPos(m_cameraPosition);
  m_level.SetViewMatrix(R3MoveGetViewMatrix());
}

void CRFClientApp::UpdateProjectionParameters()
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

bool CRFClientApp::SaveDebugFrameCapture(const char *fileName)
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

bool CRFClientApp::IsKeyDown(int virtualKey)
{
  return virtualKey >= 0 && virtualKey < 256 && (GetAsyncKeyState(virtualKey) & 0x8000) != 0;
}
