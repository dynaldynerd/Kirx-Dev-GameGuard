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
#pragma pack(push, 1)
struct _apex_block_request_wrac
{
  char szAccountID[13];
  unsigned int dwAccountSerial;
  unsigned int dwIP;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _manage_client_limit_run_result_wrac
{
  unsigned __int8 byRet;
  _CLID idLocal;
  unsigned __int8 byLoginServerIndex;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _uilock_update_result_acwr
{
  char byRet;
  unsigned __int16 wUserIndex;
  char uszUILockPW[13];
  char byHintIndex;
  char uszHintAnswer[17];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _uilock_init_result_acwr
{
  char byRet;
  unsigned __int16 wUserIndex;
  char uszUILockPW[13];
  char byHintIndex;
  char uszHintAnswer[17];
};
#pragma pack(pop)

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

#pragma pack(push, 1)
struct _open_control_inform_acwr
{
  bool bControlOpen;
  unsigned int dwControlIP;
  unsigned __int16 wControlPort;
  unsigned int dwMasterKey[4];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _character_disconnect_request_acwr
{
  unsigned __int16 wClientIndex;
  char szCharacterName[17];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _world_msg_inform_acwr
{
  unsigned __int16 wMsgSize;
  char wszMsg[1281];
};
#pragma pack(pop)

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

#pragma pack(push, 1)
struct _uilock_user_refresh_info_result_acwr
{
  unsigned int dwAccountSerial;
  char byResult;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _check_is_block_ip_result_acwr
{
  char byRet;
  _CLID idLocal;
  unsigned int ulIP;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _trans_account_inform_acwr
{
  _GLBID gidGlobal;
  unsigned int dwKey[4];
  unsigned int dwClientIP;
  unsigned int dwAccountSerial;
  char szAccountID[13];
  char byUserDgr;
  char bySubDgr;
  bool bChatLock;
  __int16 iType;
  char szCMS[7];
  int lRemainTime;
  _SYSTEMTIME stEndDate;
  char byUILock;
  char byUILock_failcnt;
  char szUILock_pw[13];
  char szAccount_pw[13];
  char byUILock_HintIndex;
  char uszUILock_HintAnswer[17];
  char byUILockFindPassFailCount;
  unsigned int dwRequestMoveCharacterSerialList[3];
  unsigned int dwTournamentCharacterSerialList[3];
  bool bIsPcBang;
  int nTrans;
  bool bAgeLimit;
};
#pragma pack(pop)

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

#pragma pack(push, 1)
struct _fireguard_divide_block_request_wrac
{
  char szAccountID[13];
  unsigned int dwAccountSerial;
  unsigned int dwIP;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _user_num_report_wrac
{
  unsigned int dwAveragePerHour;
  unsigned int dwMaxPerHour;
  unsigned int dwPlayerPerRace[3];
  char szLogDate[17];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _fireguard_block_request_wrac
{
  char szAccountID[13];
  unsigned int dwAccountSerial;
  unsigned int dwIP;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _user_block_result_acwr
{
  char byBlockResult;
  _CLID idLocalForTarget;
  _CLID idLocalForGM;
  unsigned int dwAccountSerial;
  int bLogin;
};
#pragma pack(pop)

#pragma pack(pop)
