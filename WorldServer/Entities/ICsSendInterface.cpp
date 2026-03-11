#include "pch.h"

#include "ICsSendInterface.h"

#include <cstring>

#include "CashItemRemoteStore.h"
#include "CMainThread.h"
#include "CNetProcess.h"
#include "GlobalObjects.h"
#include "limitedsale_event_inform_zocl.h"
#include "WorldServerUtil.h"

void ICsSendInterface::SendMsg_GoodsList(unsigned __int16 wSock, const _param_cash_select *psheet)
{
  _result_csi_goods_list_zocl msg{};
  msg.nCashAmount = psheet->out_dwCashAmount;
  msg.bAdjustDiscountRate = psheet->in_bAdjustDiscount;
  msg.bOneNOne = psheet->in_bOneN_One;
  msg.bSetDiscount = psheet->in_bSetDiscount;
  for (int j = 0; j < 4; ++j)
  {
    msg.bySetDiscount[j] = psheet->in_bySetDiscount[j];
  }
  msg.bLimSale = psheet->in_bLimited_Sale;
  msg.byLimDiscount = psheet->in_byLimDiscount;

  unsigned __int8 pbyType[28]{};
  pbyType[0] = 57;
  pbyType[1] = 2;
  const unsigned __int16 nLen = msg.size();
  g_Network.m_pProcess[0]->LoadSendMsg(wSock, pbyType, reinterpret_cast<char *>(&msg), nLen);
}

void ICsSendInterface::SendMsg_Error(unsigned __int16 wSock, unsigned __int8 eCode)
{
  _result_csi_error_zocl msg{};
  msg.byRetCode = eCode;

  unsigned __int8 pbyType[36]{};
  strcpy(reinterpret_cast<char *>(pbyType), "9");
  const unsigned __int16 nLen = msg.size();
  g_Network.m_pProcess[0]->LoadSendMsg(wSock, pbyType, reinterpret_cast<char *>(&msg), nLen);
}

void ICsSendInterface::SendMsg_CashDiscountEventInform(
  unsigned __int16 wSock,
  unsigned __int8 byEventType,
  _cash_discount_ini_ *pIni)
{
  _cash_discount_event_inform_zocl msg{};
  if (byEventType < 8u)
  {
    msg.byEventType = (pIni && pIni->m_bCoEvent) ? 2 : byEventType;
  }
  else
  {
    msg.byEventType = 6;
  }

  if (pIni)
  {
    msg.wCsDiscount = pIni->m_wCsDiscount;
    for (unsigned __int8 j = 0; j < 2u; ++j)
    {
      msg.wYear[j] = static_cast<unsigned __int16>(pIni->m_wYear[j]);
      msg.byMonth[j] = static_cast<unsigned __int8>(pIni->m_byMonth[j]);
      msg.byDay[j] = static_cast<unsigned __int8>(pIni->m_byDay[j]);
      msg.byHour[j] = static_cast<unsigned __int8>(pIni->m_byHour[j]);
      msg.byMinute[j] = static_cast<unsigned __int8>(pIni->m_byMinute[j]);
    }
  }
  else
  {
    msg.byEventType = 6;
  }

  unsigned __int8 pbyType[2]{57, 7};
  g_Network.m_pProcess[0]->LoadSendMsg(
    wSock,
    pbyType,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void ICsSendInterface::SendMsg_CashEventInform(
  unsigned __int16 wSock,
  unsigned __int8 byEventType,
  unsigned __int8 byStatus,
  _cash_event_ini *pIni,
  _cash_lim_sale *pLim)
{
  _cash_event_inform_zocl msg{};
  msg.byEventType = (byEventType < 3u) ? byEventType : 6;
  if (byStatus < 8u)
  {
    msg.byEventStatus = byStatus;
  }
  else
  {
    msg.byEventType = 6;
  }

  if (pIni)
  {
    if (byEventType == 2)
    {
      msg.LimitedSale.byCount = pIni->m_byLimited_sale_num;
      msg.LimitedSale.byDiscount = pIni->m_byLimDiscout;
      for (int j = 0; j < msg.LimitedSale.byCount; ++j)
      {
        msg.LimitedSale.item[j].byTableCode = pLim->m_EventItemInfo[j].byTableCode;
        msg.LimitedSale.item[j].dwIndex = pLim->m_EventItemInfo[j].dwIndex;
        msg.LimitedSale.item[j].wNum = pLim->m_EventItemInfo[j].wCount;
      }

      _SYSTEMTIME systemTime{};
      GetLocalTime(&systemTime);
      msg.wYear[0] = systemTime.wYear;
      msg.byMonth[0] = static_cast<unsigned __int8>(systemTime.wMonth);
      msg.byDay[0] = static_cast<unsigned __int8>(systemTime.wDay);
      msg.byHour[0] = static_cast<unsigned __int8>(systemTime.wHour);
      msg.byMinute[0] = static_cast<unsigned __int8>(systemTime.wMinute);
      msg.wYear[1] = static_cast<unsigned __int16>(pIni->m_wYear[1]);
      msg.byMonth[1] = static_cast<unsigned __int8>(pIni->m_byMonth[1]);
      msg.byDay[1] = static_cast<unsigned __int8>(pIni->m_byDay[1]);
      msg.byHour[1] = static_cast<unsigned __int8>(pIni->m_byHour[1]);
      msg.byMinute[1] = static_cast<unsigned __int8>(pIni->m_byMinute[1]);
    }
    else
    {
      for (int k = 0; k < 2; ++k)
      {
        msg.wYear[k] = static_cast<unsigned __int16>(pIni->m_wYear[k]);
        msg.byMonth[k] = static_cast<unsigned __int8>(pIni->m_byMonth[k]);
        msg.byDay[k] = static_cast<unsigned __int8>(pIni->m_byDay[k]);
        msg.byHour[k] = static_cast<unsigned __int8>(pIni->m_byHour[k]);
        msg.byMinute[k] = static_cast<unsigned __int8>(pIni->m_byMinute[k]);
      }
    }
  }
  else
  {
    msg.byEventType = 6;
  }

  unsigned __int8 pbyType[2]{57, 8};
  g_Network.m_pProcess[0]->LoadSendMsg(
    wSock,
    pbyType,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void ICsSendInterface::SendMsg_ConditionalEventInform(
  unsigned __int16 wSock,
  unsigned __int8 byEventType,
  unsigned __int16 wCsDiscount,
  unsigned __int8 byStatus,
  const char *pEMsg)
{
  _conditional_event_inform_zocl msg{};
  msg.byEventType = byEventType;
  msg.byEventStatus = byStatus;
  msg.wCsDiscount = wCsDiscount;
  if (pEMsg)
  {
    std::strcpy(msg.szMsgCode, pEMsg);
  }
  else
  {
    msg.szMsgCode[0] = '\0';
  }

  unsigned __int8 pbyType[2]{57, 9};
  g_Network.m_pProcess[0]->LoadSendMsg(
    wSock,
    pbyType,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void ICsSendInterface::SendMsg_LimitedsaleEventInform(
  unsigned __int16 wSock,
  unsigned __int8 byTableCode,
  unsigned int dwIndex,
  unsigned __int16 wNum)
{
  _limitedsale_event_inform_zocl msg{};
  const int msgSize = static_cast<int>(msg.size());
  std::memset(&msg, 0, msgSize);
  msg.byTableCode = byTableCode;
  msg.dwIndex = dwIndex;
  msg.wNum = wNum;

  unsigned __int8 pbyType[2]{};
  std::memcpy(pbyType, "9\n", 2);
  const unsigned __int16 nLen = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg(wSock, pbyType, reinterpret_cast<char *>(&msg), nLen);
}

void ICsSendInterface::SendMsg_BuyCashItem(
  unsigned __int16 wSock,
  const _param_cash_update *psheet,
  const _param_cash_update *sheetplus)
{
  _result_csi_buy_zocl msg{};
  msg.nNum = 0;
  msg.nCashAmount = psheet->out_nCashAmount;

  for (int j = 0; j < psheet->in_nNum10; ++j)
  {
    if (!psheet->in_item[static_cast<__int64>(j)].out_cState)
    {
      msg.item[msg.nNum].byTblCode = psheet->in_item[static_cast<__int64>(j)].in_byTblCode;
      msg.item[msg.nNum].wItemIdx = psheet->in_item[static_cast<__int64>(j)].in_wItemIdx;
      if (IsOverLapItem(psheet->in_item[static_cast<__int64>(j)].in_byTblCode))
      {
        msg.item[msg.nNum].dwDur = psheet->in_item[static_cast<__int64>(j)].in_byOverlapNum;
      }
      else
      {
        msg.item[msg.nNum].dwDur =
          GetItemDurPoint(psheet->in_item[static_cast<__int64>(j)].in_byTblCode,
            psheet->in_item[static_cast<__int64>(j)].in_wItemIdx);
      }
      msg.item[msg.nNum].dwUp = GetDefItemUpgSocketNum(
        psheet->in_item[static_cast<__int64>(j)].in_byTblCode,
        psheet->in_item[static_cast<__int64>(j)].in_wItemIdx);
      msg.item[msg.nNum].dwItemSerial = psheet->in_item[static_cast<__int64>(j)].out_wItemSerial;
      msg.item[msg.nNum].byCsMethod = psheet->in_item[static_cast<__int64>(j)].in_nLendType;
      msg.item[msg.nNum++].dwT = psheet->in_item[static_cast<__int64>(j)].out_dwT;
    }
  }

  if (sheetplus)
  {
    for (int k = 0; k < sheetplus->in_nNum10; ++k)
    {
      msg.item[msg.nNum].byTblCode = sheetplus->in_item[static_cast<__int64>(k)].in_byTblCode;
      msg.item[msg.nNum].wItemIdx = sheetplus->in_item[static_cast<__int64>(k)].in_wItemIdx;
      if (IsOverLapItem(sheetplus->in_item[static_cast<__int64>(k)].in_byTblCode))
      {
        msg.item[msg.nNum].dwDur = sheetplus->in_item[static_cast<__int64>(k)].in_byOverlapNum;
      }
      else
      {
        msg.item[msg.nNum].dwDur =
          GetItemDurPoint(sheetplus->in_item[static_cast<__int64>(k)].in_byTblCode,
            sheetplus->in_item[static_cast<__int64>(k)].in_wItemIdx);
      }
      msg.item[msg.nNum].dwUp = GetDefItemUpgSocketNum(
        sheetplus->in_item[static_cast<__int64>(k)].in_byTblCode,
        sheetplus->in_item[static_cast<__int64>(k)].in_wItemIdx);
      msg.item[msg.nNum].dwItemSerial = sheetplus->in_item[static_cast<__int64>(k)].out_wItemSerial;
      msg.item[msg.nNum].byCsMethod = sheetplus->in_item[static_cast<__int64>(k)].in_nLendType;
      msg.item[msg.nNum++].dwT = sheetplus->in_item[static_cast<__int64>(k)].out_dwT;
    }
  }

  msg.bAdjustDiscount = psheet->in_bAdjustDiscount;
  if (msg.nNum)
  {
    unsigned __int8 pbyType[36]{};
    pbyType[0] = 57;
    pbyType[1] = 4;
    const unsigned __int16 nLen = msg.size();
    g_Network.m_pProcess[0]->LoadSendMsg(wSock, pbyType, reinterpret_cast<char *>(&msg), nLen);
  }
}
