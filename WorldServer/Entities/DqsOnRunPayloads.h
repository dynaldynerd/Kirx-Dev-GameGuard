#pragma once

#include "IdaCompat.h"
#include "CGoldenBoxItemMgr.h"
#include "Packet/AccountZonePacket.h"
#include "qry_case_unmandtrader_cheat_updateregisttime.h"
#include "qry_case_unmandtrader_lazyclean_flags.h"
#include "qry_case_unmandtrader_log_in_proc_update_complete.h"
#include "qry_case_unmandtrader_time_out_cancelitem.h"

struct _qry_case_post_list_regi;
struct _qry_case_post_serial_check;

struct _qry_case_alive_char_payload
{
  unsigned __int8 in_byCase;
  unsigned int in_dwSerial;
  char in_w_szName[17];
  unsigned __int8 in_bySlot;
  unsigned __int8 out_AliveAvator[270];
  unsigned __int64 out_dwCanonicalLastConnTime;
};
static_assert(sizeof(_qry_case_alive_char_payload) == 304);

struct _log_sheet_usernum
{
  int nAveragePerHour;
  int nMaxPerHour;
  int nCount;

  int size() const
  {
    return sizeof(*this);
  }
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

struct qry_case_golden_box_item
{
  int nDBSerial;
  _db_golden_box_item NewData;
  _db_golden_box_item OldData;
};

