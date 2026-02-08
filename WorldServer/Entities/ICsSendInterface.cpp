#include "pch.h"

#include "ICsSendInterface.h"

#include <cstring>

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

void ICsSendInterface::SendMsg_LimitedsaleEventInform(
  unsigned __int16 wSock,
  unsigned __int8 byTableCode,
  unsigned int dwIndex,
  unsigned __int16 wNum)
{
  _limitedsale_event_inform_zocl msg{};
  const int msgSize = static_cast<int>(msg.size());
  memset_0(&msg, 0, msgSize);
  msg.byTableCode = byTableCode;
  msg.dwIndex = dwIndex;
  msg.wNum = wNum;

  unsigned __int8 pbyType[2]{};
  memcpy_0(pbyType, "9\n", 2);
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
