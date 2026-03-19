#pragma once

#include "IdaCompat.h"

#include "CLogFile.h"
#include "CMyTimer.h"
#include "CNetProcess.h"
#include "CRecordData.h"
#include "StorageList.h"
#include "request_csi_buy_clzo.h"

#include <functional>
#ifndef _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#endif
#include <hash_map>

struct _CashShop_fld;
struct _db_cash_limited_sale;
struct _param_cashitem_dblog;
struct _param_cash_update;
struct _result_csi_buy_zocl;
class CPlayer;

struct _cash_discount_ini_
{
  int m_bUseCashDiscount;
  int m_bRepeat;
  unsigned __int8 m_byRepeatDay;
  unsigned __int16 m_wCsDiscount;
  int m_bCoEvent;
  int m_NextEventTime[3];
  int m_wYear[3];
  int m_byMonth[3];
  int m_byDay[3];
  int m_byHour[3];
  int m_byMinute[3];
  int m_cdeTime[3];

  _cash_discount_ini_();
};

struct  _cash_discount_
{
  unsigned __int8 m_cde_status;
  CNetTimer m_cde_timer;
  _FILETIME m_cde_ini_file_time;
  CLogFile m_cde_log;
  int m_cde_inform_before[2];
  _cash_discount_ini_ m_ini;

  _cash_discount_();
};

struct _cash_event_ini
{
  struct _cash_limited_sale
  {
    char m_szLimcode[64];
    unsigned __int16 m_wLimcount;
  };

  int m_bUseCashEvent;
  int m_bRepeat;
  unsigned __int8 m_byRepeatDay;
  int m_NextEventTime[3];
  int m_wYear[3];
  int m_byMonth[3];
  int m_byDay[3];
  int m_byHour[3];
  int m_byMinute[3];
  int m_EventTime[3];
  unsigned __int8 m_byDiscout[4];
  unsigned __int8 m_byLimDiscout;
  unsigned __int8 m_byDCK;
  unsigned __int8 m_byLimited_sale_num;
  _cash_limited_sale m_Limited_sale[20];

  _cash_event_ini();
};

struct  _cash_event
{
  unsigned __int8 m_event_status;
  CNetTimer m_event_timer;
  _FILETIME m_event_ini_file_time;
  CLogFile m_event_log;
  int m_event_inform_before[2];
  _cash_event_ini m_ini;

  _cash_event();
};

struct _con_event_ini
{
  int m_bUseConEvent;
  unsigned int m_dwCashMin;
  int m_iEventTime;
  unsigned __int8 m_byEventKind;
  char m_szStartMsg[8];
  char m_szMiddletMsg[8];
  char m_szEndMsg[8];
};

struct _cash_event_time
{
  int m_nYear[2];
  int m_nMonth[2];
  int m_nDay[2];
  int m_nHour[2];
  int m_nMinute[2];
  int m_EventTime[2];
};

struct  _con_event_
{
  unsigned __int8 m_conevent_status;
  _FILETIME m_conevent_ini_file_time;
  CLogFile m_conevent_log;
  bool m_bConEvent;
  _con_event_ini m_ini;
  _cash_event_time m_eventtime;

  _con_event_();
};

struct _cash_lim_sale
{
  struct LimEventItemInfo
  {
    unsigned __int8 byTableCode;
    unsigned int dwIndex;
    unsigned __int16 wCount;
  };

  unsigned __int8 DCK;
  unsigned __int8 m_byEventNum;
  LimEventItemInfo m_EventItemInfo[20];
};

class  CashItemRemoteStore
{
public:
  using CS_RCODE = int;

  struct _remain_num_of_good
  {
    char strCode[8];
    int nMaxNum;
    int nRemainNum;

    _remain_num_of_good();
  };

  CashItemRemoteStore();
  ~CashItemRemoteStore();
  static CashItemRemoteStore *Instance();
  bool Initialize();
  void Loop_TatalCashEvent();
  bool GoodsList(unsigned __int16 wSock, char *pPacket);
  bool Buy(unsigned __int16 wSock, char *pPacket);
  bool CheatBuy(unsigned __int16 wSock, char *szItemCode, char nNum);
  bool CheatLoadCashAmount(unsigned __int16 wSock, int nNum);
  bool GoodsListBuyByCash(unsigned __int16 wSock, char *pPacket);
  void Loop_Cash_Event();
  bool Check_CashEvent_INI(unsigned __int8 byEventType);
  void Check_CashEvent_Status(unsigned __int8 byEventType);
  void Loop_ContEvent();
  void loop_cash_discount_event();
  void Check_Conditional_Event_INI();
  void Check_Conditional_Event_Status();
  void Get_Conditional_Event_Name(unsigned __int8 byEventType, char *szEventName);
  unsigned __int8 Get_Conditional_Event_Status();
  void GetEvnetTime(_cash_event_time *pETime, int itime);
  int GetRemainNumOfGood(unsigned __int16 wStoreIndex);
  int GetRemainNumOfGood(char *strCode);
  void Inform_CashEvent(unsigned __int16 wIndex);
  void Inform_ConditionalEvent(unsigned __int16 wIndex);
  void Inform_ConditionalEvent_Status_All(unsigned __int8 byEventType, unsigned __int8 byStatus, char *pszMsg);
  bool isConEventTime();
  void inform_cashdiscount_event(unsigned __int16 widx);
  void inform_cashdiscount_status_all(unsigned __int8 byType, _cash_discount_ini_ *pIni);
  void force_endup_cash_discount_event();
  bool Sell(unsigned __int16 wSock, char *pPacket);
  bool start_cde(int iBegin_TT, int iB30_TT, int iB5_TT, int iEnd_TT);
  bool start_cashevent(int iBegin_TT, int iB30_TT, int iB5_TT, int iEnd_TT, unsigned __int8 byEventType);
  bool start_conevent(int iBegin_TT, int iEnd_TT, unsigned __int8 byEventType);
  static const _CashShop_fld *FindCashRec(unsigned int nTbl, int nIdx);
  void Check_Grosssales(unsigned int dwTotalSellCash);
  void Check_Total_Selling();
  void Loop_Check_Total_Selling();
  void Set_LimitedSale_count(unsigned __int8 byTableCode, unsigned int dwIndex);
  void Set_DB_LimitedSale_Event();
  void Set_FROMDB_LimitedSale_Event(_db_cash_limited_sale *Sheet);
  bool LimitedSale_check_count(unsigned __int8 byTableCode, unsigned int dwIndex);
  unsigned __int16 BuyLimSale(unsigned __int8 byTableCode, unsigned int dwIndex);
  bool GoodsListBuyByGold(unsigned __int16 wSock, char *pPacket);
  bool BuyByCash(unsigned __int16 wSock, char *pPacket);
  bool BuyByGold(unsigned __int16 wSock, _request_csi_buy_clzo *pPacket);
  int CheckCouponType(_STORAGE_POS_INDIV *pCoupon, CPlayer *pOne, unsigned __int8 byCouponNum);
  bool UseDiscountCoupon(_param_cash_update *pBuyList, _STORAGE_POS_INDIV pCoupon, CPlayer *pOne);
  bool IsUsableCoupon(
    _request_csi_buy_clzo *pBuyList,
    _STORAGE_POS_INDIV pCoupon,
    CPlayer *pOne,
    bool *bCheck);
  bool IsBuyCashItemByGold() const;

  CLogFile _kLoggers[2];
  CRecordData _kRecGoods;
  CRecordData _kRecConEventMSG;
  _remain_num_of_good *_pkRemainInfo;
  _cash_discount_ m_cde;
  _cash_event m_cash_event[3];
  _con_event_ m_con_event;
  CMyTimer m_TotalEventTimer;
  _cash_lim_sale m_lim_event;
  _cash_lim_sale m_lim_event_New;
  _cash_lim_sale m_lim_event_Old;
  bool _bIsBuyCashItemByGold;
  CLogFile _kSysLog;

private:
  using CashMap =
    stdext::hash_map<int, const _CashShop_fld *, stdext::hash_compare<int, std::less<int>>>;

  bool _InitLoggers();
  bool _ReadGoods();
  bool _MakeLinkTable(char *szMsg, int nSize);
  bool LoadNationalPrice(CRecordData *krecPrice);
  bool __CheckGoods(CRecordData *krecPrice);
  void Load_Conditional_Event();
  void Change_Conditional_Event_Status();
  void load_con_event_ini(_con_event_ini *pIni, _FILETIME *pft);
  void Set_Conditional_Evnet_Status(unsigned __int8 byStatus);
  void load_cash_discount_event();
  void load_cde_ini(_cash_discount_ini_ *pIni, _FILETIME *pft);
  bool ChangeDiscountEventTime();
  void check_cash_discount_ini();
  void check_cash_discount_status();
  void update_ini(_cash_discount_ini_ *pNewIni);
  void log_about_cash_event(const char *szLoadInfo, _cash_discount_ini_ *pIni);
  void check_loaded_cde_status();
  unsigned __int8 get_cde_status();
  void set_cde_status(unsigned __int8 byStatus);
  bool is_cde_time();
  bool IsEventTime(unsigned __int8 byEventType);
  unsigned __int8 GetSetDiscout(unsigned __int8 bySetKind);
  unsigned __int8 GetLimDiscout();
  CS_RCODE _check_buyitem(
    unsigned __int8 byRaceSex,
    const _request_csi_buy_clzo::__item *pCsItem,
    const _CashShop_fld *pFld);
  void _buybygold_set_cashitem_dblog_sheet(CPlayer *pOne, _param_cashitem_dblog *pSheet);
  CS_RCODE _buybygold_check_valid(CPlayer *pOne, _request_csi_buy_clzo *pRecv, _param_cashitem_dblog *pSheet);
  CS_RCODE _buybygold_check_coupon(CPlayer *pOne, _request_csi_buy_clzo *pRecv, _param_cashitem_dblog *pSheet);
  CS_RCODE _buybygold_buy_single_item(
    CPlayer *pOne,
    _request_csi_buy_clzo *pRecv,
    _request_csi_buy_clzo::__item *pSrc,
    _param_cashitem_dblog *pSheet,
    bool *bCouponUse,
    _result_csi_buy_zocl *Send);
  CS_RCODE _buybygold_buy_single_item_check_item(
    CPlayer *pOne,
    _request_csi_buy_clzo::__item *pSrc,
    _param_cashitem_dblog *pSheet,
    _CashShop_fld **pCsFld);
  unsigned int _buybygold_buy_single_item_calc_price(
    CPlayer *pOne,
    _request_csi_buy_clzo *pRecv,
    _request_csi_buy_clzo::__item *pSrc,
    _param_cashitem_dblog *pSheet,
    _CashShop_fld *pCsFld,
    bool *bCouponUseCheck,
    _result_csi_buy_zocl *Send,
    unsigned int *dwDiscount);
  unsigned int _buybygold_buy_single_item_calc_price_discount(_CashShop_fld *pCsFld, unsigned __int8 byOverlapNum);
  unsigned int _buybygold_buy_single_item_calc_price_one_n_one(
    unsigned __int8 bySetKind,
    int nCsPrice,
    unsigned __int8 byOverlapNum);
  unsigned int _buybygold_buy_single_item_calc_price_limitsale(int nCsPrice, unsigned __int8 byOverlapNum);
  unsigned int _buybygold_buy_single_item_calc_price_coupon(
    CPlayer *pOne,
    _request_csi_buy_clzo *pRecv,
    unsigned __int8 byOverlapNum,
    int nCsPrice,
    bool *bCouponUseCheck,
    unsigned int *dwDiscount);
  CS_RCODE _buybygold_buy_single_item_proc_price(
    CPlayer *pOne,
    _request_csi_buy_clzo *pRecv,
    _request_csi_buy_clzo::__item *pSrc,
    _param_cashitem_dblog *pSheet,
    _CashShop_fld *pCsFld,
    bool *bCouponUseCheck,
    _result_csi_buy_zocl *Send,
    unsigned int *dwPrice,
    unsigned int *dwDiscountRate);
  void _buybygold_buy_single_item_setsenddata(_STORAGE_LIST::_db_con *GiveItem, _result_csi_buy_zocl *Send);
  void _buybygold_buy_single_item_setbuydblog(
    _param_cashitem_dblog *pSheet,
    _STORAGE_LIST::_db_con *GiveItem,
    unsigned int dwPrice,
    unsigned int dwDiscountRate);
  CS_RCODE _buybygold_buy_single_item_give_item(
    CPlayer *pOne,
    _request_csi_buy_clzo::__item *pSrc,
    _STORAGE_LIST::_db_con *GiveItem);
  void _buybygold_buy_single_item_proc_complete(
    CPlayer *pOne,
    _request_csi_buy_clzo::__item *pSrc,
    _param_cashitem_dblog *pSheet,
    _CashShop_fld *pCsFld,
    _STORAGE_LIST::_db_con *GiveItem,
    _result_csi_buy_zocl *Send,
    unsigned int dwPrice,
    unsigned int dwDiscountRate,
    bool *bCouponUseCheck,
    bool *bCouponUse);
  CS_RCODE _buybygold_buy_single_item_additional_process(
    CPlayer *pOne,
    _request_csi_buy_clzo::__item *pSrc,
    _param_cashitem_dblog *pSheet,
    _result_csi_buy_zocl *Send);
  void _buybygold_complete(
    CPlayer *pOne,
    _result_csi_buy_zocl *Send,
    _request_csi_buy_clzo *pRecv,
    _request_csi_buy_clzo::__item *pSrc,
    _param_cashitem_dblog *pSheet,
    bool bCouponUse);
  bool SetNextDiscountEventTime();
  void Load_Cash_Event();
  unsigned __int8 Get_CashEvent_Status(unsigned __int8 byEventType);
  bool ChangeEventTime(unsigned __int8 byEventType);
  void Set_LimitedSale_DCK(unsigned __int8 byEventType, unsigned __int8 byDCK);
  void Set_LimitedSale_Event();
  void Inform_CashEvent_Status_All(unsigned __int8 byEventType, unsigned __int8 byStatus, _cash_event_ini *pIni);
  void Set_CashEvent_Status(unsigned __int8 byEventType, unsigned __int8 byStatus);
  void Load_Event_INI(_cash_event_ini *pIni, _FILETIME *pft, char *pEventType);
  void Update_INI(_cash_event_ini *pNewIni, unsigned __int8 byEventType);
  void Load_LimitedSale_Event_INI(_cash_event_ini *pIni, _FILETIME *pft, char *pEventType);
  void Set_LimitedSale_Event_Ini(_cash_event_ini *pIni);
  void Check_Loaded_Event_Status(unsigned __int8 byEventType);
  bool SetNextEventTime(unsigned __int8 byEventType);
  bool LoadBuyCashMode();

  static CashItemRemoteStore *_pkInstance;
  static CashMap *_phmapTbl;
};

void Get_CashEvent_Name(unsigned __int8 byEventType, char *szEventName);

