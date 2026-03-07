#include "pch.h"

#include "CPvpCashPoint.h"

#include "CMainThread.h"
#include "CMapData.h"
#include "CPlayer.h"
#include "CPvpOrderView.h"
#include "CPvpCashMng.h"
#include "GlobalObjects.h"
#include "pvp_cash_recover_itemlist_result_zocl.h"
#include "Packet/ZoneClientPacket.h"

CPvpCashPoint::CPvpCashPoint()
{
  m_KillerList.SetList(10, 248, false);
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

int CPvpCashPoint::GetMyClassVal(char *pClass)
{
  CPvpCashMng *pvpCash = CPvpCashMng::Instance();
  return pvpCash->GetMyClassVal(pClass);
}

double CPvpCashPoint::CalPvpCashPoint(int nDstLv, int nSrcLv, char *pSrcClass, unsigned __int8 nParty)
{
  if (!nParty)
  {
    nParty = 1;
  }

  const int levelAdjust = nDstLv - nSrcLv + 30;
  const int classVal = GetMyClassVal(pSrcClass);
  const double pvpCashPoint = static_cast<double>((classVal * levelAdjust) / nParty);
  if (pvpCashPoint < 0.0)
  {
    return 0.0;
  }

  return pvpCashPoint;
}

bool CPvpCashPoint::IsPvpMap(CPlayer *pOne)
{
  if (!pOne || !pOne->m_bOper)
  {
    return false;
  }

  return pOne->m_pCurMap->m_pMapSet->m_nRacePvpUsable[static_cast<int>(pOne->GetObjRace())] != 0;
}

bool CPvpCashPoint::CheckPvpLoseCondition(CPlayer *pKiller, CPlayer *pDier)
{
  if (!pKiller || !pDier)
  {
    return false;
  }

  if (!IsPvpMap(pKiller))
  {
    SendMsg_PvpCashInform(pDier->m_ObjID.m_wIndex, 9u);
    return false;
  }

  const unsigned int killerSerial = pKiller->m_Param.GetCharSerial();
  if (m_KillerList.IsInList(killerSerial, nullptr))
  {
    SendMsg_PvpCashInform(pDier->m_ObjID.m_wIndex, 3u);
    return false;
  }

  if (pKiller->m_Param.GetRaceCode() == pDier->m_Param.GetRaceCode())
  {
    return false;
  }

  const double pvpTempCash = pKiller->GetPvpOrderView()->GetPvpTempCash();
  const bool isPremium = pKiller->IsApplyPcbangPrimium();
  const int maxTempPoint = pKiller->m_kPvpCashPoint.GetMaxTempPoint(static_cast<unsigned __int8>(pKiller->GetLevel()), isPremium);
  return pvpTempCash < static_cast<double>(maxTempPoint);
}

bool CPvpCashPoint::CheckPvpHaveCondition(CPlayer *pKiller, CPlayer *pDier, double dOldTempPoint)
{
  if (!pKiller || !pDier)
  {
    return false;
  }

  if (!IsPvpMap(pKiller))
  {
    SendMsg_PvpCashInform(pKiller->m_ObjID.m_wIndex, 9u);
    return false;
  }

  const unsigned int killerSerial = pKiller->m_Param.GetCharSerial();
  if (pDier->m_kPvpCashPoint.m_KillerList.IsInList(killerSerial, nullptr))
  {
    SendMsg_PvpCashInform(pKiller->m_ObjID.m_wIndex, 4u);
    return false;
  }

  if (pKiller->m_Param.GetRaceCode() == pDier->m_Param.GetRaceCode())
  {
    return false;
  }

  return static_cast<double>(pDier->m_kPvpCashPoint.GetMinTempPoint(static_cast<unsigned __int8>(pDier->GetLevel())))
    < dOldTempPoint;
}

int CPvpCashPoint::GetMaxTempPoint(unsigned __int8 nLv, bool bPremium)
{
  CPvpCashMng *pvpCash = CPvpCashMng::Instance();
  return pvpCash->GetMaxTempPoint(nLv, bPremium);
}

int CPvpCashPoint::GetMinTempPoint(unsigned __int8 nLv)
{
  CPvpCashMng *pvpCash = CPvpCashMng::Instance();
  return pvpCash->GetMinTempPoint(nLv);
}

int CPvpCashPoint::GetContPvpHave()
{
  return m_byContPvpHave;
}

int CPvpCashPoint::GetContPvpLose()
{
  return m_byContPvpLose;
}

void CPvpCashPoint::SetContPvpHave(unsigned __int8 byCnt)
{
  m_byContPvpHave = byCnt;
}

void CPvpCashPoint::SetContPvpLose(unsigned __int8 byCnt)
{
  m_byContPvpLose = byCnt;
}

bool CPvpCashPoint::GetRaceWarRecvr()
{
  return m_bRaceWarRecvr;
}

void CPvpCashPoint::SetRaceWarRecvr(bool bUsed)
{
  m_bRaceWarRecvr = bUsed;
}

bool CPvpCashPoint::SetKillerList(unsigned int dwKillerSerial)
{
  if (m_KillerList.FindNode(dwKillerSerial, nullptr))
  {
    return false;
  }

  if (m_KillerList.GetSize() >= m_KillerList.m_dwMaxBufNum)
  {
    unsigned int outIndex = 0;
    m_KillerList.PopNode_Front(&outIndex, nullptr);
  }

  m_KillerList.PushNode_Back(dwKillerSerial, nullptr);
  return true;
}

void CPvpCashPoint::UpdateKillerList(_PVP_ORDER_VIEW_DB_BASE *pkInfo)
{
  m_KillerList.m_csList.Lock();
  CIndexList::_index_node *node = m_KillerList.m_Head.m_pNext;
  int index = 0;
  while (node != &m_KillerList.m_Tail)
  {
    pkInfo->dwKillerSerial[index] = node->m_dwIndex;
    node = node->m_pNext;
    ++index;
  }
  m_KillerList.m_csList.Unlock();
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
  _notify_pvp_cash_point_error_zocl msg{};
  msg.byRetCode = static_cast<char>(byError);
  unsigned __int8 pbyType[2]{59, 5};
  g_Network.m_pProcess[0]->LoadSendMsg(
    n,
    pbyType,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

