#include "pch.h"

#include "CPvpCashPoint.h"

#include "CMainThread.h"
#include "CPvpCashMng.h"
#include "GlobalObjects.h"
#include "pvp_cash_recover_itemlist_result_zocl.h"

CPvpCashPoint::CPvpCashPoint()
{
  m_KillerList.SetList(0xAu, 0xF8u, false);
}

CPvpCashPoint::~CPvpCashPoint()
{
}

void CPvpCashPoint::Init(_PVP_ORDER_VIEW_DB_BASE *pkInfo)
{
  m_KillerList.ResetList();
  for (int j = 0; j < 10; ++j)
  {
    if (pkInfo->dwKillerSerial[j])
    {
      m_KillerList.PushNode_Back(pkInfo->dwKillerSerial[j], nullptr);
    }
  }
  m_byContPvpHave = pkInfo->byContHaveCash;
  m_byContPvpLose = pkInfo->byContLoseCash;
  m_bRaceWarRecvr = pkInfo->bRaceWarRecvr;
}

void CPvpCashPoint::KillerListInit()
{
  m_KillerList.ResetList();
}

void CPvpCashPoint::SendMsg_TalikList(unsigned int n)
{
  _pvp_cash_recover_itemlist_result_zocl msg{};

  CPvpCashMng *pvpCash = CPvpCashMng::Instance();
  msg.byItemNum = pvpCash->m_TalikList.byTalikNum;
  if (msg.byItemNum > 14u)
  {
    msg.byItemNum = 14;
  }

  for (int index = 0; index < msg.byItemNum; ++index)
  {
    msg.nTalikInfo[index] = pvpCash->m_TalikList.TalikInfo[index].nRecvrPoint;
  }

  unsigned __int8 type[2]{12, 20};
  g_Network.m_pProcess[0]->LoadSendMsg(
    n,
    type,
    reinterpret_cast<char *>(&msg),
    msg.size());
}

int CPvpCashPoint::GetTalikRecvrPoint(unsigned __int8 byTblCode, unsigned int dwIndex)
{
  CPvpCashMng *pvpCash = CPvpCashMng::Instance();
  return pvpCash->GetTalikRecvrPoint(byTblCode, dwIndex);
}

int CPvpCashPoint::GetMaxTempPoint(unsigned __int8 nLv, bool bPremium)
{
  CPvpCashMng *pvpCash = CPvpCashMng::Instance();
  return pvpCash->GetMaxTempPoint(nLv, bPremium);
}

void CPvpCashPoint::SendMsg_RecoverResult(unsigned int n, char byRet, int nRecvrPoint)
{

  _pvp_cash_point_recover_result_zocl msg{};
  msg.byRetCode = byRet;
  msg.nRecvrPoint = nRecvrPoint;

  unsigned __int8 type[2]{12, 22};
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&msg), 5u);
}

void CPvpCashPoint::SendMsg_PvpCashInform(unsigned int n, unsigned __int8 byError)
{
  char msg = static_cast<char>(byError);
  unsigned __int8 pbyType[2]{59, 5};
  g_Network.m_pProcess[0]->LoadSendMsg(n, pbyType, &msg, 1u);
}

