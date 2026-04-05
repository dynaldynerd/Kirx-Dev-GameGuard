#pragma once

#include "IdaCompat.h"
#include "CUnmannedTraderRegistItemInfo.h"
#include "CUnmannedTraderRequestLimiter.h"
#include "qry_case_unmandtrader_log_in_proc_update_complete.h"
#include <vector>

class CPlayer;
class CLogFile;
struct _TRADE_DB_BASE;
struct _a_trade_adjust_price_request_clzo;
struct _a_trade_clear_item_request_clzo;
struct _a_trade_reg_item_request_clzo;
struct _unmannedtrader_buy_item_request_clzo;
struct _unmannedtrader_re_regist_request_clzo;
struct _unmannedtrader_search_list_request_clzo;
struct _qry_case_unmandtrader_cheat_updateregisttime;
struct _qry_case_unmandtrader_cancelitem;
struct _lt_qry_case_unmandtrader_select_list;
struct _qry_case_unmandtrader_registsingleitem;
struct _qry_case_unmandtrader_buy_get_info;
struct _qry_case_unmandtrader_re_registsingleitem;
struct _qry_case_unmandtrader_re_registsingleitem_roll_back;
struct _qry_case_unmandtrader_update_reprice;

class  CUnmannedTraderUserInfo
{
public:
  enum class LOG_IN_STATE : int
  {
    UTUI_EMPTY = 0,
    UTUI_LOGIN = 1,
    UTUI_NONE = -1,
  };

  CUnmannedTraderUserInfo();
  ~CUnmannedTraderUserInfo();

  bool Init(unsigned short wInx);
  bool Load(
    unsigned __int8 byType,
    unsigned __int16 wInx,
    unsigned int dwSerial,
    _TRADE_DB_BASE *kInfo,
    CLogFile *pkLogger);
  bool SetLoadInfo(unsigned __int8 byType, unsigned int dwSerial, _TRADE_DB_BASE *kInfo, CLogFile *pkLogger);
  void Clear();
  void ClearLoadItemInfo();
  void ClearRequest();
  void Regist(unsigned __int8 byType, _a_trade_reg_item_request_clzo *pRequest, CLogFile *pkLogger);
  void ModifyPrice(unsigned __int8 byType, _a_trade_adjust_price_request_clzo *pRequest, CLogFile *pkLogger);
  void CancelRegist(unsigned __int8 byType, _a_trade_clear_item_request_clzo *pRequest, CLogFile *pkLogger);
  void Buy(unsigned __int8 byType, _unmannedtrader_buy_item_request_clzo *pRequest, CLogFile *pkLogger);
  void Search(unsigned __int8 byType, _unmannedtrader_search_list_request_clzo *pRequest, CLogFile *pkLogger);
  void ReRegist(unsigned __int8 byType, _unmannedtrader_re_regist_request_clzo *pRequest, CLogFile *pkLogger);
  unsigned __int8 CheckRegist(
    unsigned __int8 byType,
    _a_trade_reg_item_request_clzo *pRequest,
    CLogFile *pkLogger,
    unsigned __int8 *byTempSlotIndex,
    unsigned __int8 *byDivision,
    unsigned __int8 *byClass,
    unsigned __int8 *bySubClass,
    unsigned int *dwListIndex,
    unsigned int *dwTax);
  unsigned __int8 RegistItem(
    unsigned __int8 byType,
    _a_trade_reg_item_request_clzo *pRequest,
    unsigned __int8 byTempSlotIndex,
    unsigned __int8 byDivision,
    unsigned __int8 byClass,
    unsigned __int8 bySubClass,
    unsigned int dwListIndex,
    unsigned int dwTax);
  unsigned __int8 CheckModifyPrice(
    unsigned __int8 byType,
    _a_trade_adjust_price_request_clzo *pRequest,
    unsigned int *dwOldPrice,
    CLogFile *pkLogger,
    unsigned int *pdwTax);
  unsigned __int8 CheckCancelRegist(
    unsigned __int8 byType,
    _a_trade_clear_item_request_clzo *pRequest,
    CLogFile *pkLogger);
  unsigned __int8 CheckBuy(
    unsigned __int8 byType,
    _unmannedtrader_buy_item_request_clzo *pRequest,
    CPlayer **pkBuyer);
  unsigned __int8 CheckSearch(
    unsigned __int8 byType,
    _unmannedtrader_search_list_request_clzo *pRequest,
    unsigned int *dwListIndex,
    unsigned int *dwCurVer);
  unsigned __int8 CheckReRegist(
    unsigned __int8 byType,
    CLogFile *pkLogger,
    unsigned __int16 wItemSerial,
    unsigned __int8 byAmount,
    unsigned __int8 byItemTableCode,
    unsigned __int16 wItemIndex,
    unsigned int dwRegistSerial,
    unsigned int dwPrice,
    unsigned __int8 *pbyDivision,
    unsigned __int8 *pbyClass,
    unsigned __int8 *pbySubClass,
    unsigned int *pdwTax,
    unsigned int *pdwListIndex);
  std::vector<CUnmannedTraderRegistItemInfo>::iterator Find(unsigned int dwRegistSerial);
  std::vector<CUnmannedTraderRegistItemInfo>::iterator FindRegist(
    std::vector<CUnmannedTraderRegistItemInfo>::iterator iterStart);
  std::vector<CUnmannedTraderRegistItemInfo>::iterator FindEmpty();
  void CountRegistItem();
  void GetCurrentRegItemStateStr(char *szStateStr, int iBuffSize);
  CUnmannedTraderItemState::STATE GetCloseItemForPassTimeUpdateInfo(
    unsigned int dwRegistSerial,
    CPlayer **pkOwner);
  bool CompleteUpdateState(unsigned int dwRegistSerial, unsigned __int8 byState, bool bReCountRegist);
  void CompleteCreate(CLogFile *pkLogger);
  void SetAllItemState(unsigned __int8 byState, unsigned __int8 byMaxCnt);
  void PrcoSellUpdateWaitItem(
    _qry_case_unmandtrader_log_in_proc_update_complete *pkResult,
    unsigned __int8 byGroupType,
    CLogFile *pkLogger);
  void ProcSellWaitItem(
    _qry_case_unmandtrader_log_in_proc_update_complete *pkResult,
    unsigned __int8 byGroupType,
    CLogFile *pkLogger);
  void NotifyCloseItem(
    _qry_case_unmandtrader_log_in_proc_update_complete *pkResult,
    CLogFile *pkLogger);
  bool CheckIsUpdatedTaxRate(unsigned __int8 byTax, CLogFile *pkLogger);
  void CompleteRegist(
    unsigned __int8 byRet,
    _qry_case_unmandtrader_registsingleitem *pLoadData,
    CLogFile *pkLogger);
  bool CompleteRegistItem(
    unsigned int dwRegistSerial,
    unsigned __int16 dwItemSerial,
    unsigned int dwETSerialNumber,
    unsigned int dwPrice,
    unsigned __int8 bySellTurm,
    unsigned __int8 byTableCode,
    unsigned __int16 wItemIndex,
    unsigned __int8 byStorageIndex,
    unsigned __int64 dwD,
    unsigned int dwU,
    bool bInserted);
  void CompleteCancelRegist(
    unsigned __int8 byRet,
    _qry_case_unmandtrader_cancelitem *pLoadData,
    CLogFile *pkLogger);
  bool CompleteCancelRegistItem(
    unsigned int dwRegistSerial,
    unsigned __int16 dwItemSerial,
    CLogFile *pkLogger);
  void CompleteTimeOutClear(unsigned int dwRegistSerial, CLogFile *pkLogger);
  void CompleteReprice(
    unsigned __int8 byRet,
    _qry_case_unmandtrader_update_reprice *pLoadData,
    CLogFile *pkLogger);
  bool CompleteRepriceItem(unsigned int dwRegistSerial, unsigned __int16 dwItemSerial, unsigned int dwPrice);
  void CompleteReRegist(_qry_case_unmandtrader_re_registsingleitem *pLoadData, CLogFile *pkLogger);
  bool CompleteReRegistItem(
    unsigned int dwRegistSerial,
    unsigned __int16 dwItemSerial,
    unsigned int dwPrice,
    CLogFile *pkLogger,
    unsigned __int8 *pbyProcRet);
  void CompleteUpdateCheatRegistTime(_qry_case_unmandtrader_cheat_updateregisttime *pLoadData);
  void CompleteReRegistRollBack(_qry_case_unmandtrader_re_registsingleitem_roll_back *pData, CLogFile *pkLogger);
  unsigned __int8 CheckSellComplete(
    CPlayer *pkSellPlayer,
    CPlayer *pkBuyer,
    unsigned int dwRegistSerial,
    unsigned int dwRealPrice,
    CLogFile *pkLogger);
  unsigned __int8 SellComplete(
    CPlayer *pkSellPlayer,
    CPlayer *pkBuyer,
    unsigned int dwOriPrice,
    unsigned int dwRealPrice,
    unsigned int dwTax,
    unsigned int dwRegistSerial,
    __int64 tResultTime,
    CLogFile *pkLogger);
  unsigned __int8 CheckBuyComplete(CPlayer *pkBuyer, unsigned int dwPrice);
  unsigned __int8 BuyComplete(
    CPlayer *pkBuyer,
    unsigned int dwSellerSerial,
    const char *wszSellerName,
    const char *szSellerAccountName,
    unsigned int dwRegistSerial,
    unsigned int dwK,
    unsigned __int64 dwD,
    unsigned int dwU,
    unsigned int dwPrice,
    unsigned __int64 lnUID,
    CLogFile *pkLogger,
    unsigned __int16 *wAddItemSerial);
  void NotifyRegistItem();
  void SendRegistItemSuccessResult(
    unsigned int dwLeftDalant,
    unsigned __int16 wInx,
    const _qry_case_unmandtrader_registsingleitem *pLoadData);
  void SendRegistItemErrorResult(
    unsigned __int16 wInx,
    char byRet,
    unsigned __int16 wItemSerial,
    unsigned int dwRetParam1);
  void SendCancelRegistSuccessResult(unsigned __int16 wInx, unsigned __int16 wItemSerial, unsigned int dwRegistSerial);
  void SendCancelRegistErrorResult(unsigned __int16 wInx, unsigned __int8 byRet);
  void SendRepriceSuccessResult(
    CPlayer *pReceiver,
    unsigned __int16 wItemSerial,
    unsigned int dwNewPrice,
    unsigned int dwRegistSerial,
    unsigned int dwTax);
  void SendRepriceErrorResult(CPlayer *pReceiver, unsigned __int8 byRet);
  void SendBuyErrorResult(unsigned __int16 wInx, unsigned __int8 byRet);
  void SendSellInfom(
    unsigned __int16 wInx,
    unsigned __int16 wItemSerial,
    unsigned int dwAddDalant,
    unsigned int dwTaxDalant,
    unsigned int dwTotalDalant);
  void SendWaitingRegisterItemNotifyRegisterSuccess(
    unsigned __int16 wInx,
    unsigned int dwRegistSerial,
    unsigned __int16 wItemSerial);
  void SendNotifyCloseItem(
    unsigned __int16 wInx,
    unsigned __int16 wItemSerial,
    unsigned int dwRegistSerial,
    unsigned int dwPrice,
    unsigned __int8 byTaxRate);
  void LockedItemSetUnlock(
    const _qry_case_unmandtrader_log_in_proc_update_complete::__list *pLoadData,
    CLogFile *pkLogger);
  CPlayer *FindOwner();
  unsigned __int16 GetIndex();
  bool IsLogInState();
  unsigned int GetSerial();
  unsigned __int8 GetMaxRegistCnt();
  const CUnmannedTraderRegistItemInfo *GetRegItemInfo();
  bool IsNull();
  void SendSearchErrorResult(unsigned __int16 wInx, char byRet);
  void SendSearchResult(unsigned __int16 wInx, _lt_qry_case_unmandtrader_select_list *pLoadData);
  void LogOut(unsigned int dwSerial, CLogFile *pkLogger);
  bool CheatCancelRegist(unsigned __int8 byNth);
  bool CheatCancelRegistAll();
  bool CheatCancelRegistSingle(unsigned __int8 byNth);

  bool operator==(unsigned int dwSerial);
  CUnmannedTraderUserInfo &operator=(const CUnmannedTraderUserInfo &rhs);

  static CUnmannedTraderUserInfo ms_kNull;

  LOG_IN_STATE m_eState;
  unsigned short m_wInx;
  unsigned int m_dwUserSerial;
  unsigned char m_byRegistCnt;
  unsigned char m_byMaxRegistCnt;
  CUnmannedTraderRequestLimiter m_kRequestState;
  std::vector<CUnmannedTraderRegistItemInfo> m_vecRegistItemInfo;
  std::vector<CUnmannedTraderRegistItemInfo> m_vecLoadItemInfo;
};
