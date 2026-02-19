#pragma once

#include "IdaCompat.h"
#include "CGoldenBoxItemMgr.h"

struct _qry_case_post_list_regi;
struct _qry_case_post_serial_check;

struct _qry_case_alive_char_payload
{
  unsigned __int8 in_byCase;
  unsigned __int8 byPadding[3];
  unsigned int in_dwSerial;
  char in_w_szName[17];
  unsigned __int8 in_bySlot;
  unsigned __int8 out_AliveAvator[270];
};
static_assert(sizeof(_qry_case_alive_char_payload) == 296);

struct _log_sheet_usernum
{
  int nAveragePerHour;
  int nMaxPerHour;
  int nCount;
};

struct _log_sheet_economy
{
  unsigned int dwDate;
  unsigned int dwAlignPad;
  long double dTradeGold[3];
  long double dTradeDalant[3];
  int nMgrValue;
  int nAlignPad;
  long double dCutOre[6][3];
};

struct _qry_case_joinacguild_local
{
  unsigned int in_guildindex;
  unsigned int in_guildserial;
  unsigned int in_applierindex;
  unsigned int in_applierserial;
  unsigned int in_accepterserial;
  int in_Grade;
  int in_MemberNum;
};

struct  _qry_case_insertguild_local
{
  unsigned int in_guildindex;
  char in_w_guildName[17];
  unsigned __int8 in_guildRace;
  unsigned __int8 in_membernum;
  unsigned __int8 in_padding;
  unsigned int in_memberindex[8];
  unsigned int in_memberserial[8];
  char tmp_w_membername[136];
  unsigned __int8 tmp_lv[8];
  unsigned __int8 tmp_grade[8];
  unsigned int tmp_pvp[8];
  unsigned int out_guildserial;
  unsigned int tmp_Esterindex;
  unsigned int tmp_Esterserial;
};

struct _qry_case_inputgmoney_local
{
  unsigned int in_pusherserial;
  char in_w_pushername[17];
  unsigned int tmp_guildindex;
  unsigned int in_guildserial;
  unsigned int dwAddGold;
  unsigned int dwAddDalant;
  unsigned __int8 in_date[4];
  long double out_totalgold;
  long double out_totaldalant;
  unsigned __int8 byProcRet;
};

struct _qry_case_unmandtrader_time_out_cancelitem
{
  unsigned __int8 byType;
  unsigned __int8 byPadding0[3];
  unsigned int dwRegistSerial;
  unsigned __int8 byState;
  unsigned __int8 byPadding1[3];
  unsigned int dwOwnerSerial;
  unsigned __int8 byItemTableCode;
  unsigned __int8 byPadding2;
  unsigned __int16 wItemTableIndex;
  char szAccountID[13];
  char wszName[17];
  unsigned __int8 byReserved[30];
};
static_assert(sizeof(_qry_case_unmandtrader_time_out_cancelitem) == 80);

struct _qry_case_unmandtrader_log_in_proc_update_complete_create
{
  struct __list
  {
    unsigned __int8 byProcRet;
    unsigned __int8 byProcUpdate;
    unsigned int dwBuyer;
    unsigned int dwRegistSerial;
    unsigned __int8 byUpdateState;
  };

  unsigned __int16 wInx;
  unsigned int dwSeller;
  bool bAllSuccess;
  unsigned __int8 byType;
  unsigned __int16 wNum;
  __list List[20];
};
static_assert(sizeof(_qry_case_unmandtrader_log_in_proc_update_complete_create) == 332);

struct _qry_case_unmandtrader_cheat_updateregisttime
{
  struct __list
  {
    unsigned __int8 byProcRet;
    unsigned __int8 byState;
    unsigned __int16 wReserved;
    unsigned int dwRegistSerial;
  };

  unsigned __int8 byType;
  unsigned __int8 byNum;
  unsigned __int16 wInx;
  unsigned int dwOwnerSerial;
  __list List[10];
};

struct _qry_case_unmandtrader_lazyclean_flags
{
  bool bFlag[4];
};
static_assert(sizeof(_qry_case_unmandtrader_lazyclean_flags) == 4);

#pragma pack(push, 1)
struct _manage_client_limit_run_request_acwr
{
  unsigned __int16 wIndex;
  unsigned __int8 byZeroPad[4];
  unsigned __int8 byCode;
};
#pragma pack(pop)

struct qry_case_golden_box_item
{
  int nDBSerial;
  _db_golden_box_item NewData;
  _db_golden_box_item OldData;
};

