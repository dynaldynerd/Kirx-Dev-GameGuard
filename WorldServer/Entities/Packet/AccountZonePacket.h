#pragma once

#include "../IdaCompat.h"
#include "../CMainThread.h"

// Account -> Zone packet structs.

// enter_world_result_acwr.h

#pragma pack(push, 1)
struct _enter_world_result_acwr
{
  _CLID idLocal;
  unsigned __int8 byRetCode;
};
#pragma pack(pop)

static_assert(sizeof(_enter_world_result_acwr) == 7, "enter world result packet size mismatch");

// force_close_command_acwr.h

#pragma pack(push, 1)
struct _force_close_command_acwr
{
  _CLID idLocal;
  bool bDirectly;
  unsigned __int8 byKickType;
  unsigned int dwPushIP;
};
#pragma pack(pop)

static_assert(sizeof(_force_close_command_acwr) == 12, "force close packet size mismatch");

// open_world_failure_acwr.h

#pragma pack(push, 1)
struct _open_world_failure_acwr
{
  char szErrCode[64];
};
#pragma pack(pop)

static_assert(sizeof(_open_world_failure_acwr) == 64, "open world failure packet size mismatch");

// open_world_success_acwr.h

#pragma pack(push, 1)
struct _open_world_success_acwr
{
  unsigned __int8 byWorldCode;
  char szDBName[32];
  char szDBIP[17];
  unsigned __int8 byWorldType;
};
#pragma pack(pop)

static_assert(sizeof(_open_world_success_acwr) == 51, "open world success packet size mismatch");

// world_service_inform_acwr.h

#pragma pack(push, 1)
struct _world_service_inform_acwr
{
  bool bService;
};
#pragma pack(pop)

static_assert(sizeof(_world_service_inform_acwr) == 1, "world service inform packet size mismatch");

// world_exit_inform_acwr.h

struct _world_exit_inform_acwr
{
};

// chat_lock_command_acwr.h

#pragma pack(push, 1)
struct _chat_lock_command_acwr
{
  _CLID idLocal;
  unsigned __int16 wBlockTimeH;
};
#pragma pack(pop)

static_assert(sizeof(_chat_lock_command_acwr) == 8, "chat lock command packet size mismatch");

// cashdb_info_recv_result_acwr.h

#pragma pack(push, 1)
struct _cashdb_info_recv_result_acwr
{
  char szIP[48];
  char szDBName[32];
  char szAccount[32];
  char szPassword[32];
  unsigned int dwPort;
};
#pragma pack(pop)

static_assert(sizeof(_cashdb_info_recv_result_acwr) == 148, "cashdb info packet size mismatch");

// character_disconnect_request_acwr.h

#pragma pack(push, 1)
struct _character_disconnect_request_acwr
{
  unsigned __int16 wClientIndex;
  char szCharacterName[17];
};
#pragma pack(pop)

static_assert(sizeof(_character_disconnect_request_acwr) == 19, "character disconnect packet size mismatch");

// disconnect_guild_war_character_request_acwr.h

#pragma pack(push, 1)
struct _disconnect_guild_war_character_request_acwr
{
  unsigned __int16 wClientIndex;
  char szCharacterName[17];
};
#pragma pack(pop)

static_assert(sizeof(_disconnect_guild_war_character_request_acwr) == 19, "guild war disconnect packet size mismatch");

// open_control_inform_acwr.h

#pragma pack(push, 1)
struct _open_control_inform_acwr
{
  bool bControlOpen;
  unsigned int dwControlIP;
  unsigned __int16 wControlPort;
  unsigned int dwMasterKey[4];
};
#pragma pack(pop)

static_assert(sizeof(_open_control_inform_acwr) == 23, "open control inform packet size mismatch");

// world_msg_inform_acwr.h

#pragma pack(push, 1)
struct _world_msg_inform_acwr
{
  unsigned __int16 wMsgSize;
  char wszMsg[1281];
};
#pragma pack(pop)

static_assert(sizeof(_world_msg_inform_acwr) == 1283, "world msg inform packet size mismatch");

// uilock_user_refresh_info_result_acwr.h

#pragma pack(push, 1)
struct _uilock_user_refresh_info_result_acwr
{
  unsigned int dwAccountSerial;
  char byResult;
};
#pragma pack(pop)

static_assert(sizeof(_uilock_user_refresh_info_result_acwr) == 5, "uilock refresh result packet size mismatch");

// check_is_block_ip_result_acwr.h

#pragma pack(push, 1)
struct _check_is_block_ip_result_acwr
{
  char byRet;
  _CLID idLocal;
  unsigned int ulIP;
};
#pragma pack(pop)

static_assert(sizeof(_check_is_block_ip_result_acwr) == 11, "block ip result packet size mismatch");

// trans_account_inform_acwr.h

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

static_assert(sizeof(_trans_account_inform_acwr) == 154, "trans account inform packet size mismatch");

// uilock_update_result_acwr.h

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

static_assert(sizeof(_uilock_update_result_acwr) == 34, "uilock update result packet size mismatch");

// uilock_init_result_acwr.h

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

static_assert(sizeof(_uilock_init_result_acwr) == 34, "uilock init result packet size mismatch");

// user_block_result_acwr.h

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

static_assert(sizeof(_user_block_result_acwr) == 21, "user block result packet size mismatch");

// manage_client_limit_run_request_acwr.h

#pragma pack(push, 1)
struct _manage_client_limit_run_request_acwr
{
  unsigned __int16 wIndex;
  unsigned __int8 byZeroPad[4];
  unsigned __int8 byCode;
};
#pragma pack(pop)

static_assert(sizeof(_manage_client_limit_run_request_acwr) == 7, "client limit run request packet size mismatch");
