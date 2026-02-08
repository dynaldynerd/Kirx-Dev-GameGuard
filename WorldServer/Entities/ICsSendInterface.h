#pragma once

#include "IdaCompat.h"
#include "param_cash.h"
#include "result_csi_buy_zocl.h"
#include "result_csi_error_zocl.h"
#include "result_csi_goods_list_zocl.h"

class __cppobj ICsSendInterface
{
public:
  static void SendMsg_GoodsList(unsigned __int16 wSock, const _param_cash_select *psheet);
  static void SendMsg_Error(unsigned __int16 wSock, unsigned __int8 eCode);
  static void SendMsg_LimitedsaleEventInform(
    unsigned __int16 wSock,
    unsigned __int8 byTableCode,
    unsigned int dwIndex,
    unsigned __int16 wNum);
  static void SendMsg_BuyCashItem(
    unsigned __int16 wSock,
    const _param_cash_update *psheet,
    const _param_cash_update *sheetplus);
};
