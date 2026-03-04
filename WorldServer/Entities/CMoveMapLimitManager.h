#pragma once

#include "IdaCompat.h"

#include "CMoveMapLimitInfoList.h"
#include "CMoveMapLimitRightInfoList.h"

class CPlayer;

class  CMoveMapLimitManager
{
public:
  CMoveMapLimitManager();
  ~CMoveMapLimitManager();

  static CMoveMapLimitManager *Instance();
  static void Destroy();
  void Loop();
  bool Init();
  void Load(CPlayer *pkPlayer);
  char MoveLimitMapZoneRequest(int iUserInx, char *pRequest);
  bool Request(
    int iLimitType,
    int iRequetType,
    int iMapInx,
    unsigned int dwStoreRecordIndex,
    int iUserInx,
    char *pRequest);
  bool RequestElanMapUserForceMoveHQ();
  void LogIn(CPlayer *pkPlayer);
  void LogOut(CPlayer *pkPlayer);
  void CreateComplete(CPlayer *pkPlayer);

  CMoveMapLimitRightInfoList m_kRightInfo;
  CMoveMapLimitInfoList m_kLimitInfo;

private:
  static CMoveMapLimitManager *ms_Instance;
};
