#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CUnmannedTraderLazyCleaner.h"
#include "CUnmannedTraderTradeInfo.h"

class CRFWorldDatabase;
class CUnmannedTraderRegistItemInfo;
struct _a_trade_adjust_price_request_clzo;
struct _a_trade_clear_item_request_clzo;
struct _a_trade_reg_item_request_clzo;
struct _unmannedtrader_reserved_schedule_info;
struct _unmannedtrader_buy_item_request_clzo;
struct _unmannedtrader_re_regist_request_clzo;
struct _unmannedtrader_search_list_request_clzo;
struct _TRADE_DB_BASE;
struct _qry_case_unmandtrader_cancelitem;
struct _qry_case_unmandtrader_registsingleitem;
struct _qry_case_unmandtrader_time_out_cancelitem;
struct _qry_case_unmandtrader_update_reprice;
struct _qry_case_unmandtrader_updateitemstate;
struct _qry_case_unmandtrader_buy_get_info;
struct _qry_case_unmandtrader_buy_update_complete;
struct _qry_case_unmandtrader_buy_update_rollback;
struct _qry_case_unmandtrader_buy_update_wait;
struct _qry_case_unmandtrader_cheat_updateregisttime;
struct _qry_case_unmandtrader_lazyclean_flags;
struct _qry_case_unmandtrader_log_in_proc_update_complete;
struct _qry_case_unmandtrader_re_registsingleitem;
struct _qry_case_unmandtrader_re_registsingleitem_roll_back;
struct _lt_qry_case_unmandtrader_select_list;

class  CUnmannedTraderController
{
public:
  static CUnmannedTraderController *ms_Instance;

  CUnmannedTraderController();
  ~CUnmannedTraderController();

  static CUnmannedTraderController *Instance();
  static void Destroy();
  bool Init();
  void Loop();
  bool Load();
  bool Load(unsigned __int16 wInx, unsigned int dwSerial, _TRADE_DB_BASE *kInfo);
  void Regist(unsigned __int16 wInx, _a_trade_reg_item_request_clzo *pRequest);
  void ModifyPrice(unsigned __int16 wInx, _a_trade_adjust_price_request_clzo *pRequest);
  void CancelRegist(unsigned __int16 wInx, _a_trade_clear_item_request_clzo *pRequest);
  void Buy(unsigned __int16 wInx, _unmannedtrader_buy_item_request_clzo *pRequest);
  void Search(unsigned __int16 wInx, _unmannedtrader_search_list_request_clzo *pRequest);
  void ReRegist(unsigned __int16 wInx, _unmannedtrader_re_regist_request_clzo *pRequest);
  unsigned __int8 GetMaxRegistCnt(unsigned __int16 wInx, unsigned int dwSerial);
  const CUnmannedTraderRegistItemInfo *GetRegItemInfo(unsigned __int16 wInx, unsigned int dwSerial);
  void LogOut(unsigned __int16 wInx, unsigned int dwSerial);
  void CompleteCreate(unsigned __int16 wInx);
  void CompleteCreateNotifyTradeInfo(unsigned __int8 byRace, unsigned __int16 wInx);
  unsigned __int8 SelectSearchList(
    _lt_qry_case_unmandtrader_select_list *pData,
    CRFWorldDatabase *pkWorldDB,
    unsigned __int8 *byProcRet);
  void CompleteSelectSearchList(
    unsigned __int8 byDBRet,
    unsigned __int8 byProcRet,
    _lt_qry_case_unmandtrader_select_list *pLoadData);
  unsigned __int8 UpdateItemState(_qry_case_unmandtrader_updateitemstate *pData);
  unsigned __int8 UpdateRegistItem(_qry_case_unmandtrader_registsingleitem *pData);
  unsigned __int8 UpdateCancelRegist(_qry_case_unmandtrader_cancelitem *pData);
  unsigned __int8 UpdateTimeOutCancelRegist(_qry_case_unmandtrader_time_out_cancelitem *pData);
  unsigned __int8 UpdateRePrice(_qry_case_unmandtrader_update_reprice *pData);
  unsigned __int8 SelectBuy(_qry_case_unmandtrader_buy_get_info *pData);
  unsigned __int8 UpdateBuy(_qry_case_unmandtrader_buy_update_wait *pData);
  unsigned __int8 UpdateBuyRollBack(_qry_case_unmandtrader_buy_update_rollback *pData);
  unsigned __int8 UpdateLazyClean(_qry_case_unmandtrader_lazyclean_flags *pData);
  unsigned __int8 UpdateBuyComplete(_qry_case_unmandtrader_buy_update_complete *pData);
  unsigned __int8 UpdateLogInComplete(_qry_case_unmandtrader_log_in_proc_update_complete *pData);
  unsigned __int8 UpdateReRegist(_qry_case_unmandtrader_re_registsingleitem *pData);
  unsigned __int8 UpdateCheatRegistTime(_qry_case_unmandtrader_cheat_updateregisttime *pData);
  unsigned __int8 UpdateReRegistRollBack(_qry_case_unmandtrader_re_registsingleitem_roll_back *pData);
  bool CheatCancelRegist(unsigned __int16 wInx, unsigned int dwOwnerSerial, unsigned __int8 byNth);

  void CompleteSelectReservedSchedule(unsigned __int8 byRet, _unmannedtrader_reserved_schedule_info *pLoadData);
  void CompleteUpdateState(unsigned __int8 byRet, _qry_case_unmandtrader_updateitemstate *pLoadData);
  void CompleteRegistItem(unsigned __int8 byRet, _qry_case_unmandtrader_registsingleitem *pLoadData);
  void CompleteCancelRegist(unsigned __int8 byRet, _qry_case_unmandtrader_cancelitem *pLoadData);
  void CompleteTimeOutCancelRegist(unsigned __int8 byRet, _qry_case_unmandtrader_time_out_cancelitem *pLoadData);
  void CompleteReprice(unsigned __int8 byRet, _qry_case_unmandtrader_update_reprice *pLoadData);
  void CompleteSelectBuyInfo(unsigned __int8 byRet, _qry_case_unmandtrader_buy_get_info *pLoadData);
  void CompleteBuy(unsigned __int8 byRet, _qry_case_unmandtrader_buy_update_wait *pLoadData);
  void CompleteBuyRollBack(unsigned __int8 byRet, _qry_case_unmandtrader_buy_update_rollback *pLoadData);
  void ComleteLazyClean(_qry_case_unmandtrader_lazyclean_flags *pData);
  void CompleteBuyComplete(_qry_case_unmandtrader_buy_update_complete *pData);
  void CompleteLogInCompete(_qry_case_unmandtrader_log_in_proc_update_complete *pData);
  void CompleteReRegist(_qry_case_unmandtrader_re_registsingleitem *pLoadData);
  void CompleteUpdateCheatRegistTime(_qry_case_unmandtrader_cheat_updateregisttime *pLoadData);
  void CompleteReRegistRollBack(_qry_case_unmandtrader_re_registsingleitem_roll_back *pData);

private:
  bool InitLogger();
  bool InsertStateRecord();
  bool UpdateClearDanglingOwnerRecord();
  bool InsertDefalutRecord();
  unsigned __int8 GetEmptyRecordSerial(unsigned int *dwSerial, bool *pbRecordInserted);
  unsigned __int8 CheckDBItemState(
    unsigned __int8 byType,
    unsigned int dwRegistSerial,
    unsigned __int8 *byState,
    unsigned __int8 *byProcRet);
  void Log(const char *fmt, ...);

public:
  CLogFile *m_pkLogger;
  CLogFile *m_pkServiceLogger;
  CUnmannedTraderTradeInfo m_kTradeInfo;
  CUnmannedTraderLazyCleaner m_kLazyCleaner;
};
