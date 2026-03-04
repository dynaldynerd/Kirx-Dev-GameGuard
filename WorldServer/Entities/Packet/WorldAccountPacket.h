#pragma once

#include "../IdaCompat.h"
#include "../CMainThread.h"

// World -> Account packet structs.

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
