#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CUnmannedTraderItemState.h"
#include "CUnmannedTraderTradeInfo.h"
#include "CUnmannedTraderUserInfo.h"
#include "qry_case_unmandtrader_buy_update.h"
#include "unmannedtrader_buy_item_result_zocl.h"
#include <vector>

class CPlayer;
struct _TRADE_DB_BASE;
struct _a_trade_adjust_price_request_clzo;
struct _a_trade_clear_item_request_clzo;
struct _a_trade_reg_item_request_clzo;
struct _unmannedtrader_buy_item_request_clzo;
struct _unmannedtrader_re_regist_request_clzo;
struct _unmannedtrader_search_list_request_clzo;

class  CUnmannedTraderUserInfoTable
{
public:
  static CUnmannedTraderUserInfoTable *Instance();
  static void Destroy();

  CUnmannedTraderUserInfoTable();
  ~CUnmannedTraderUserInfoTable();
  bool Init();
  bool Load(unsigned __int8 byType, unsigned __int16 wInx, unsigned int dwSerial, _TRADE_DB_BASE *kInfo);
  bool CheckwIndexAndType(unsigned __int16 wInx, unsigned __int8 byType, const char *szCallFuncName);
  void Regist(unsigned __int16 wInx, unsigned __int8 byType, _a_trade_reg_item_request_clzo *pRequest);
  void ModifyPrice(unsigned __int16 wInx, unsigned __int8 byType, _a_trade_adjust_price_request_clzo *pRequest);
  void CancelRegist(unsigned __int16 wInx, unsigned __int8 byType, _a_trade_clear_item_request_clzo *pRequest);
  void Buy(unsigned __int16 wInx, unsigned __int8 byType, _unmannedtrader_buy_item_request_clzo *pRequest);
  void Search(unsigned __int16 wInx, unsigned __int8 byType, _unmannedtrader_search_list_request_clzo *pRequest);
  void ReRegist(unsigned __int16 wInx, unsigned __int8 byType, _unmannedtrader_re_regist_request_clzo *pRequest);
  void SetLogger(CLogFile *pkLogger, CLogFile *pkServiceLogger);
  CUnmannedTraderUserInfo *Find(unsigned int dwSerial);
  CUnmannedTraderUserInfo *FindByIndex(unsigned __int16 wInx);
  CUnmannedTraderUserInfo *FindUser(unsigned __int16 wInx, unsigned int dwSerial);
  bool CheatCancelRegist(unsigned __int16 wInx, unsigned int dwOwnerSerial, unsigned __int8 byNth);
  unsigned __int8 GetMaxRegistCnt(unsigned __int16 wInx, unsigned int dwSerial);
  const CUnmannedTraderRegistItemInfo *GetRegItemInfo(unsigned __int16 wInx, unsigned int dwSerial);
  void CompleteCreate(unsigned __int16 wInx);
  CUnmannedTraderItemState::STATE GetCloseItemForPassTimeUpdateInfo(
    unsigned int dwOwnerSerial,
    unsigned int dwRegistSerial,
    CPlayer **pkOwner);
  bool CompleteUpdateState(unsigned int dwOwnerSerial, unsigned int dwRegistSerial, unsigned __int8 byState);
  void CompleteRegist(unsigned __int8 byRet, char *pLoadData);
  void CompleteCancelRegist(unsigned __int8 byRet, char *pLoadData);
  void CompleteTimeOutClear(char *pLoadData);
  void CompleteReprice(unsigned __int8 byRet, char *pLoadData);
  void ClearRequest(unsigned __int16 wInx, unsigned int dwOwner);
  void CompleteBuy(
    unsigned __int8 byRet,
    _qry_case_unmandtrader_buy_update_wait *pLoadData,
    CUnmannedTraderTradeInfo *pkTaradInfo);
  void CompleteReRegist(char *pLoadData);
  void CompleteUpdateCheatRegistTime(char *pLoadData);
  void CompleteReRegistRollBack(unsigned __int16 wInx, unsigned int dwOwnerSerial, char *pData);
  void CompleteSearch(unsigned __int8 byDBRet, unsigned __int8 byProcRet, char *pLoadData);
  void LogOut(unsigned __int16 wInx, unsigned int dwSerial);
  void ClearLogLogOutState(
    const char *szType,
    unsigned int dwOwner,
    const char *szAccount,
    char *wszName,
    unsigned int dwRegistSerial,
    unsigned int dwK,
    unsigned int dwD,
    unsigned int dwU);
  void Log(const char *fmt, ...);
  void ServiceLog(const char *fmt, ...);
  bool SubCompleteBuyFindBuyer(
    _qry_case_unmandtrader_buy_update_wait *pkQuery,
    CUnmannedTraderUserInfo **ppkBuyUser,
    CPlayer **ppkBuyPlayer);
  bool SubCompleteBuyProcBuy(
    CPlayer *pkBuyPlayer,
    CUnmannedTraderUserInfo *pkBuyUser,
    __int64 tResultTime,
    const _qry_case_unmandtrader_buy_update_wait::__list *pkQueryList,
    _unmannedtrader_buy_item_result_zocl::__list *pSendResultList,
    _qry_case_unmandtrader_buy_update_complete::__list *pUpdateCompleteList,
    unsigned __int8 *byCompleteUpdateNum,
    unsigned int *pdwPayDalant,
    CUnmannedTraderTradeInfo *pkTaradInfo);
  bool SubCompleteBuyProcBuyResult(
    unsigned __int8 byRet,
    _qry_case_unmandtrader_buy_update_complete::__list *pUpdateCompleteList,
    unsigned __int8 *byCompleteUpdateNum);
  void SubCompleteBuyIncreaseVesion(unsigned __int8 byDivision, unsigned __int8 byClass);
  void PushUpdateBuyRollBack(_qry_case_unmandtrader_buy_update_wait *pkQuery);

  static CUnmannedTraderUserInfoTable *ms_Instance;
  static unsigned int m_uiBuyRollBackCallCountSum;

  CLogFile *m_pkLogger;
  CLogFile *m_pkServiceLogger;
  std::vector<CUnmannedTraderUserInfo> m_veckInfo;
};
