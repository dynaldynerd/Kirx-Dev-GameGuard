#pragma once

#include "../IdaCompat.h"
#include "../CMainThread.h"

// Zone -> Account packet structs.

// character_disconnect_result_wrac.h

#pragma pack(push, 1)
struct _character_disconnect_result_wrac
{
  unsigned __int16 wClientIndex;
  unsigned __int8 byResult;

  unsigned __int16 size() const;
};
#pragma pack(pop)

// enter_lobby_report_wrac.h

#pragma pack(push, 1)
struct _enter_lobby_report_wrac
{
  _GLBID gidGlobal;

  unsigned __int16 size() const;
};
#pragma pack(pop)

// enter_world_request_wrac.h

#pragma pack(push, 1)
struct _enter_world_request_wrac
{
  _GLBID gidGlobal;
  _CLID idLocal;
  unsigned int ulConnectIP;

  __int64 size() const;
};
#pragma pack(pop)

// holy_quest_report_wrac.h

#pragma pack(push, 1)
struct _holy_quest_report_wrac
{
  unsigned __int8 byRaceCode;
  char wszCharName[17];
  unsigned __int8 byDestroyedRaceCode;

  unsigned __int16 size() const
  {
    return static_cast<unsigned __int16>(sizeof(_holy_quest_report_wrac));
  }
};
#pragma pack(pop)

// select_avator_report_wrac.h

#pragma pack(push, 1)
struct _select_avator_report_wrac
{
  _GLBID gidGlobal;
  char wszCharName[17];
  unsigned int dwAvatorSerial;
  unsigned __int8 byLevel;

  __int64 size() const;
};
#pragma pack(pop)

// start_world_request_wrac.h

#pragma pack(push, 1)
struct _start_world_request_wrac
{
  unsigned int dwGateIP;
  unsigned __int16 wGatePort;

  unsigned __int16 size() const;
};
#pragma pack(pop)

// trans_account_report_wrac.h

#pragma pack(push, 1)
struct _trans_account_report_wrac
{
  _GLBID gidGlobal;
  unsigned __int8 byRetCode;

  unsigned __int16 size() const;
};
#pragma pack(pop)

// world_exit_report_wrac.h

struct _world_exit_report_wrac
{
  unsigned __int16 size() const
  {
    return 0;
  }
};

// world_account_ping_wrac.h

struct _world_account_ping_wrac
{
  unsigned __int16 size() const
  {
    return 0;
  }
};

// open_world_request_wrac.h

#pragma pack(push, 1)
struct _open_world_request_wrac
{
  char szWorldName[33];
  unsigned __int8 byRunMode;
  unsigned int dwWorldServerIP;
  unsigned __int8 byHash[32];

  unsigned __int16 size() const;
};
#pragma pack(pop)

static_assert(sizeof(_open_world_request_wrac) == 70, "open world request packet size mismatch");

// cashdb_setting_request_wrac.h

struct _cashdb_setting_request_wrac
{
  unsigned __int16 size() const;
};

// uilock_user_refresh_info_request_wrac.h

#pragma pack(push, 1)
struct _uilock_user_refresh_info_request_wrac
{
  unsigned int dwAccountSerial;
  unsigned __int8 byFailCnt;
  unsigned __int8 byFindPassFailCount;
  _GLBID gidGlobal;

  unsigned __int16 size() const;
};
#pragma pack(pop)

static_assert(sizeof(_uilock_user_refresh_info_request_wrac) == 14, "uilock refresh request packet size mismatch");

// logout_account_request_wrac.h

#pragma pack(push, 1)
struct _logout_account_request_wrac
{
  _GLBID gidGlobal;

  unsigned __int16 size() const;
};
#pragma pack(pop)

static_assert(sizeof(_logout_account_request_wrac) == 8, "logout account request packet size mismatch");

// apex_block_request_wrac.h

#pragma pack(push, 1)
struct _apex_block_request_wrac
{
  char szAccountID[13];
  unsigned int dwAccountSerial;
  unsigned int dwIP;
};
#pragma pack(pop)

static_assert(sizeof(_apex_block_request_wrac) == 21, "apex block packet size mismatch");

// manage_client_limit_run_result_wrac.h

#pragma pack(push, 1)
struct _manage_client_limit_run_result_wrac
{
  unsigned __int8 byRet;
  _CLID idLocal;
  unsigned __int8 byLoginServerIndex;
};
#pragma pack(pop)

static_assert(sizeof(_manage_client_limit_run_result_wrac) == 8, "client limit run result packet size mismatch");

// uilock_init_request_wrac.h

#pragma pack(push, 1)
struct _uilock_init_request_wrac
{
  unsigned __int16 wUserIndex;
  unsigned int dwAccountSerial;
  char uszUILockPW[13];
  char byHintIndex;
  char uszHintAnswer[17];
};
#pragma pack(pop)

static_assert(sizeof(_uilock_init_request_wrac) == 37, "uilock init request packet size mismatch");

// user_block_request_wrac.h

#pragma pack(push, 1)
struct _user_block_request_wrac
{
  char byBlockType;
  int iPeriod;
  _GLBID gidGlobalForTarget;
  _CLID idLocalForGM;
  char uszReason[32];
  char uszWriter[32];
  unsigned int dwAccountSerial;
};
#pragma pack(pop)

static_assert(sizeof(_user_block_request_wrac) == 87, "user block request packet size mismatch");

// uilock_update_request_wrac.h

#pragma pack(push, 1)
struct _uilock_update_request_wrac
{
  unsigned __int16 wUserIndex;
  unsigned int dwAccountSerial;
  char uszUILockPW[13];
  char byHintIndex;
  char uszHintAnswer[17];
};
#pragma pack(pop)

static_assert(sizeof(_uilock_update_request_wrac) == 37, "uilock update request packet size mismatch");

// fireguard_divide_block_request_wrac.h

#pragma pack(push, 1)
struct _fireguard_divide_block_request_wrac
{
  char szAccountID[13];
  unsigned int dwAccountSerial;
  unsigned int dwIP;
};
#pragma pack(pop)

static_assert(sizeof(_fireguard_divide_block_request_wrac) == 21, "fireguard divide block packet size mismatch");

// user_num_report_wrac.h

#pragma pack(push, 1)
struct _user_num_report_wrac
{
  unsigned int dwAveragePerHour;
  unsigned int dwMaxPerHour;
  unsigned int dwPlayerPerRace[3];
  char szLogDate[17];
};
#pragma pack(pop)

static_assert(sizeof(_user_num_report_wrac) == 37, "user num report packet size mismatch");

// fireguard_block_request_wrac.h

#pragma pack(push, 1)
struct _fireguard_block_request_wrac
{
  char szAccountID[13];
  unsigned int dwAccountSerial;
  unsigned int dwIP;
};
#pragma pack(pop)

static_assert(sizeof(_fireguard_block_request_wrac) == 21, "fireguard block packet size mismatch");
