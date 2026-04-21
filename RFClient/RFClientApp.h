#pragma once

#include <windows.h>

#include "RFClientPlayer.h"
#include "R3Engine/2ndclass/level.h"
#include "R3Engine/2ndclass/r3d3dwrapper.h"
#include "R3Engine/2ndclass/r3move.h"

class CRFClientApp : public R3D3dWrapper
{
public:
  CRFClientApp();
  ~CRFClientApp();

  bool Initialize(HINSTANCE hInstance, LPCSTR commandLine);
  int Run();
  void Shutdown();

protected:
  HRESULT FrameMove() override;
  HRESULT Render() override;
  LRESULT MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

private:
  bool LoadMapSelection(LPCSTR commandLine);
  bool LoadMap(const char *mapSelection);
  void ResetCharacterFromLoadedMap();
  void ResetCameraFromLoadedMap();
  void UpdateCharacter();
  void UpdateCamera();
  bool DrawPlayerShadowRender();
  void ApplyCamera();
  void UpdateProjectionParameters();
  bool SaveDebugFrameCapture(const char *fileName);
  static bool IsKeyDown(int virtualKey);

private:
  CLevel m_level;
  CRFClientPlayer m_player;
  R3Camera m_camera;
  bool m_isInitialized;
  bool m_isMapLoaded;
  char m_mapName[MAX_PATH];
  float m_cameraTarget[3];
  float m_cameraPosition[3];
  float m_spawnCameraTarget[3];
  float m_fViewAngleX;
  float m_fViewAngleY;
  float m_fPrevViewAngleX;
  float m_fPrevViewAngleY;
  float m_fPointDist;
  float m_mapFarClipDistance;
  bool m_hasSpawnCameraTarget;
  bool m_bKeyboardMoveMode;
  bool m_isRightMouseDragging;
  int m_nPrevX;
  int m_nPrevY;
  int m_pendingWheelDelta;
};
