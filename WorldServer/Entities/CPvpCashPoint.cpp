#include "pch.h"

#include "CPvpCashPoint.h"

#include "CMainThread.h"

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

