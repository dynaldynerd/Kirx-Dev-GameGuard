#pragma once

#include <windows.h>

#include "CLand.h"
#include "CCharacterDataMgr.h"
#include "CCharacterMgr.h"
#include "CItemDataMgr.h"
#include "CResourceDataMgr.h"
#include "RFClientUI.h"
#include "R3Engine/2ndclass/anicamera.h"
#include "R3Engine/2ndclass/r3d3dwrapper.h"
#include "R3Engine/2ndclass/r3move.h"

class CPlayer;
class CGameProgress;
class CNetworkMgr;
struct _reged_char_result_zone;

enum
{
  MAX_GAME_PROGRESS = 3,
  GPI_INVALID = 0xFF,
  GPI_TITLE = 0x0,
  GPI_LOGIN = 0x1,
  GPI_MAIN_GAME = 0x2
};

class CMainApp : public R3D3dWrapper
{
public:
  CMainApp();
  ~CMainApp();

  bool Initialize(HINSTANCE hInstance, LPCSTR commandLine);
  int Run();
  void Shutdown();

  BOOL Create(void);
  BOOL Destroy(void);
  void Create_GameProgress(BYTE pi_byGameProgressID);
  BOOL GameMainLoop(void);
  BOOL IsReadyToStart(void);
  BOOL InputProcess(void);
  BOOL RequestQuitProgram(DWORD pi_dwQuitWaitingTime = static_cast<DWORD>(-1),
                          char *pi_pMsg = NULL);
  BOOL GetDataFromLauncher(void);
  void SetClientWindowVisible(BOOL pi_bVisible);
  BOOL IsClientWindowVisible(void) const { return m_bClientWindowVisible; }
  BOOL IsOfflineRegedCharReplay(void) const { return m_bOfflineRegedCharReplay; }

  BOOL LoadMainGameData(void);
  void UnloadMainGameData(void);
  BOOL LoadLoginLobbyData(void);
  void UnloadLoginLobbyData(void);
  BOOL BuildLoginLobbyCharacterDummies(const _reged_char_result_zone &pi_stRegedCharResult);
  void ClearLoginLobbyCharacterDummies(void);
  BOOL BuildLoginLobbyCreatePreview(BYTE pi_byRaceSexCode,
                                    const char *pi_pClassCode,
                                    DWORD pi_dwBaseShape);
  void ClearLoginLobbyCreatePreview(void);
  void RotateLoginLobbyCreatePreview(float pi_fDeltaRotY);
  BOOL SetLoginLobbyOpeningCamera(void);
  BOOL PlayLoginLobbyCharacterEntryCamera(BYTE pi_bySlotIndex);
  BOOL PlayLoginLobbyCharacterNextCamera(BYTE pi_byOldSlotIndex);
  BOOL PlayLoginLobbyCharacterPrevCamera(BYTE pi_byOldSlotIndex);
  BOOL PlayLoginLobbyCharacterCreateRaceCamera(BYTE pi_bySlotIndex);
  BOOL PlayLoginLobbyCharacterCreateRaceCancelCamera(BYTE pi_bySlotIndex);
  BOOL PlayLoginLobbyCharacterCreateAttributeCamera(void);
  BOOL PlayLoginLobbyCharacterCreateAttributeCancelCamera(void);
  BOOL PlayLoginLobbyCharacterCreateDetailCamera(void);
  BOOL PlayLoginLobbyCharacterCreateDetailCancelCamera(void);
  BOOL PlayLoginLobbyCharacterCreateCompleteCamera(BYTE pi_bySlotIndex);
  BOOL IsLoginLobbyCameraAnimating(void) const { return m_bLoginLobbyCameraAnimating; }
  void FrameMoveLoginLobby(void);
  void RenderLoginLobby(void);
  HRESULT FrameMoveMainGame(void);
  HRESULT RenderMainGame(void);
  LRESULT MsgProcMainGame(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  HRESULT RestoreDeviceObjects() override;
  HRESULT InvalidateDeviceObjects() override;

  CLand &GetLand(void) { return m_land; }
  CCharacterMgr &GetCharacterMgr(void) { return m_CharacterMgr; }
  CNetworkMgr *GetNetworkMgr(void) { return m_pNetworkMgr; }
  CRFClientUI &GetUI(void) { return m_cUI; }
  const char *GetMapSelection(void) const { return m_szSelectedMap; }
  BOOL IsActive(void) const { return m_bIsActive; }
  BYTE GetGameProgressID(void) const { return m_byGameProgressID; }
  bool IsWindowedMode(void) const { return m_bWindowed ? true : false; }

protected:
  HRESULT FrameMove() override;
  HRESULT Render() override;
  LRESULT MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

private:
  bool ParseOfflineRegedCharReplay(LPCSTR commandLine);
  bool LoadMapSelection(LPCSTR commandLine);
  bool LoadMap(const char *mapSelection);
  BOOL PlayLoginLobbyCamera(const char *pi_pCameraName,
                            DWORD pi_dwStartFrame,
                            DWORD pi_dwEndFrame,
                            DWORD pi_dwFlag);
  BOOL LoadData(void);
  bool UnloadData(void);
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
  CLand m_land;
  CCharacterMgr m_CharacterMgr;
  CNetworkMgr *m_pNetworkMgr;
  CCharacterDataMgr m_cCharDataMgr;
  CItemDataMgr m_cItemDataMgr;
  CCharResDataMgr m_cCharResDataMgr;
  CRFClientUI m_cUI;
  CGameProgress *m_pGameProgress;
  CPlayer *m_pPlayer;
  CPlayer *m_pLoginLobbyCreatePreview;
  float m_fLoginLobbyCreatePreviewRotY;
  R3Camera m_camera;
  bool m_isInitialized;
  bool m_isMapLoaded;
  BOOL m_bIsActive;
  float m_fVersion;
  BYTE m_byGameProgressID;
  BOOL m_bCreatedAllObject;
  BOOL m_bLoadedAllData;
  BOOL m_bRequestQuitProgram;
  DWORD m_dwRequestQuitTime;
  DWORD m_dwQuitWaitingTime;
  BOOL m_bIsQuit;
  BOOL m_bClientWindowVisible;
  BOOL m_bOfflineRegedCharReplay;
  char m_mapName[MAX_PATH];
  char m_szSelectedMap[MAX_PATH];
  char m_szOfflineRegedCharPath[MAX_PATH];
  CAniCamera m_cLoginLobbyAniCamera;
  BOOL m_bLoginLobbyCameraAnimating;
  BOOL m_bLoginLobbyRandomStartCamera;
  Vector3f m_vecLoginLobbyCameraPos;
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

extern CMainApp *g_pMainApp;

#define _GetMainApp() g_pMainApp
