#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"

#pragma pack(push, 1)
struct _open_world_request_wrac
{
  char szWorldName[33];
  unsigned __int8 byRunMode;
  unsigned int dwWorldServerIP;
  unsigned __int8 byHash[32];

  unsigned __int16 size() const;
};

struct _cashdb_setting_request_wrac
{
  unsigned __int16 size() const;
};

struct _uilock_user_refresh_info_request_wrac
{
  unsigned int dwAccountSerial;
  unsigned __int8 byFailCnt;
  unsigned __int8 byFindPassFailCount;
  _GLBID gidGlobal;

  unsigned __int16 size() const;
};

struct _logout_account_request_wrac
{
  _GLBID gidGlobal;

  unsigned __int16 size() const;
};
#pragma pack(pop)
