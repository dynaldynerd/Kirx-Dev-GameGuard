#pragma once

#include "IdaCompat.h"
#include "StoragePos.h"

struct __cppobj __declspec(align(4)) _param_cash
{
  unsigned int in_dwAccountSerial;
  unsigned int in_dwAvatorSerial;
  unsigned __int16 in_wSockIndex;
  bool in_bAdjustDiscount;
  bool in_bOneN_One;
  bool in_bSetDiscount;
  bool in_bLimited_Sale;

  _param_cash(unsigned int dwAccount, unsigned int dwAvator, unsigned __int16 wSock);
  ~_param_cash();
};

struct __cppobj __declspec(align(2)) _param_cash_select : _param_cash
{
  char in_szAcc[13];
  unsigned int out_dwCashAmount;
  unsigned __int8 in_bySetDiscount[4];
  unsigned __int16 out_bReturn;
  unsigned __int8 in_byLimDiscount;

  _param_cash_select(unsigned int dwAc, unsigned int dwAv, unsigned __int16 wSock);
  ~_param_cash_select();
  __int64 size();
};

struct __cppobj _param_cash_update : _param_cash
{
  struct __cppobj __item
  {
    unsigned __int8 byRet;
    char in_strItemCode[8];
    unsigned __int8 in_byOverlapNum;
    unsigned __int8 in_byTblCode;
    unsigned __int16 in_wItemIdx;
    unsigned __int16 in_nDiscount;
    int in_nPrice;
    unsigned __int8 in_nLendType;
    unsigned int in_dwLendTime;
    unsigned __int8 in_nEventType;
    bool in_bIsApplyCoupon;
    unsigned __int64 in_lnUID;
    char out_cState;
    int out_nCashAmount;
    int out_nStdPrice;
    int out_nBuyPrice;
    unsigned __int16 out_wItemSerial;
    unsigned int out_dwT;

    __item();
  };

  char in_szAcc[13];
  char in_szSvrName[33];
  char in_szAvatorName[17];
  unsigned int in_nCashAmount;
  unsigned __int8 in_nNum10;
  unsigned __int8 in_bySetKind;
  unsigned __int8 in_nCouponCnt;
  _STORAGE_POS_INDIV in_CouponItem[3];
  int out_nCashAmount;
  unsigned int in_dwIP;
  unsigned __int16 out_bReturn;
  __item in_item[20];

  _param_cash_update(unsigned int dwAc, unsigned int dwAv, unsigned __int16 wSock);
  ~_param_cash_update();
  __int64 size();
};

struct __cppobj _param_cash_rollback : _param_cash
{
  struct __declspec(align(8)) __list
  {
    unsigned __int64 in_lnUID;
    char in_strItemCode[8];
    unsigned __int8 in_byOverlapNum;
    int in_nPrice;
    int in_nDiscount;
    int out_nRet;
    char out_cStatus;
    int out_nCashAmount;
    int out_nStatus;
  };

  char in_szAcc[13];
  int in_byNum;
  int out_nCashAmount;
  char in_szWorldName[33];
  char in_UserIP[15];
  char in_szAvatorName[17];
  __list data[20];

  _param_cash_rollback(unsigned int dwAc, unsigned int dwAv, unsigned __int16 wSock);
  ~_param_cash_rollback();
  __int64 size();
};

struct __cppobj _param_cashitem_dblog : _param_cash
{
  struct __list
  {
    unsigned __int8 byRet;
    unsigned __int8 byTblCode;
    unsigned __int16 wItemIndex;
    unsigned __int8 byOverlapNum;
    unsigned int dwCost;
    int iCashDiscount;
  };

  unsigned __int8 in_nCouponCnt;
  _STORAGE_POS_INDIV in_CouponItem[3];
  unsigned __int8 byLv;
  int nBuyNum;
  __list data[20];

  _param_cashitem_dblog(unsigned int dwAv);
  ~_param_cashitem_dblog();
  __int64 size();
};

struct _param_cash_total_selling
{
  unsigned int dwTotalSellCash;

  __int64 size();
};
