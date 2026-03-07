#include "pch.h"

#include "WorldServerUtil.h"

#include "CUnmannedTraderSchedule.h"
#include "CUnmannedTraderItemState.h"
#include "CUnmannedTraderUserInfoTable.h"
#include "CPlayer.h"
#include "CPlayerDB.h"
#include "GlobalObjects.h"
#include "InvenKey.h"
#include "qry_case_unmandtrader_time_out_cancelitem.h"

CUnmannedTraderSchedule::CUnmannedTraderSchedule()
{
  Clear();
}

CUnmannedTraderSchedule::~CUnmannedTraderSchedule()
{
  // this is not a stub
}

void CUnmannedTraderSchedule::Clear()
{
  this->m_eState = STATE::EMPTY;
  this->m_byType = static_cast<unsigned char>(-1);
  this->m_dwRegistSerial = 0;
  this->m_tEndTime = 0;
  this->m_dwOwnerSerial = 0;
  this->m_byItemTableCode = static_cast<unsigned char>(-1);
  this->m_wItemTableIndex = static_cast<unsigned short>(-1);
}

void CUnmannedTraderSchedule::Set(
  unsigned __int8 byType,
  unsigned int dwSerial,
  __int64 tEndTime,
  unsigned int dwOwnerSerial,
  unsigned int dwK)
{
  m_eState = STATE::REG_WAIT;
  m_byType = byType;
  m_dwRegistSerial = dwSerial;
  m_tEndTime = tEndTime;
  m_dwOwnerSerial = dwOwnerSerial;

  _INVENKEY key;
  key.LoadDBKey(dwK);
  m_byItemTableCode = key.byTableCode;
  m_wItemTableIndex = key.wItemIndex;
}

unsigned __int8 CUnmannedTraderSchedule::GetType()
{
  return m_byType;
}

unsigned int CUnmannedTraderSchedule::GetRegistSerial()
{
  return m_dwRegistSerial;
}

unsigned int CUnmannedTraderSchedule::GetOwnerSerial()
{
  return m_dwOwnerSerial;
}

bool CUnmannedTraderSchedule::IsWait()
{
  return m_eState == STATE::REG_WAIT;
}

bool CUnmannedTraderSchedule::IsDone()
{
  __int64 now = 0;
  time_19(&now);
  return m_tEndTime <= now;
}

void CUnmannedTraderSchedule::PushClear(bool bTimeLimit)
{
  m_eState = STATE::WAIT_CANCEL;

  CPlayer *owner = nullptr;
  CUnmannedTraderItemState::STATE updateState = CUnmannedTraderItemState::STATE::NONE;
  if (bTimeLimit)
  {
    updateState = static_cast<CUnmannedTraderItemState::STATE>(13);
  }
  else
  {
    CUnmannedTraderUserInfoTable *table = CUnmannedTraderUserInfoTable::Instance();
    updateState = table->GetCloseItemForPassTimeUpdateInfo(m_dwOwnerSerial, m_dwRegistSerial, &owner);
  }

  if (updateState != CUnmannedTraderItemState::STATE::NONE &&
      (updateState == static_cast<CUnmannedTraderItemState::STATE>(6) ||
       updateState == static_cast<CUnmannedTraderItemState::STATE>(13) ||
       updateState == static_cast<CUnmannedTraderItemState::STATE>(
         static_cast<int>(CUnmannedTraderItemState::STATE::WINDEL) | 0x8)))
  {
    _qry_case_unmandtrader_time_out_cancelitem qryData{};
    qryData.byType = m_byType;
    qryData.dwRegistSerial = m_dwRegistSerial;
    qryData.byState = static_cast<unsigned __int8>(updateState);
    qryData.dwOwnerSerial = m_dwOwnerSerial;
    qryData.byItemTableCode = m_byItemTableCode;
    qryData.wItemTableIndex = m_wItemTableIndex;

    if (owner)
    {
      strcpy_s(qryData.szAccountID, 13, owner->m_pUserDB->m_szAccountID);
      const char *charName = owner->m_Param.GetCharNameW();
      strcpy_s(qryData.wszName, 17, charName);
    }

    g_Main.PushDQSData(-1, nullptr, 62, reinterpret_cast<char *>(&qryData), sizeof(qryData));
  }
}

void CUnmannedTraderSchedule::CompleteClear(unsigned __int8 byDBQueryRet, unsigned __int8 byProcRet)
{
  Clear();
  if (byDBQueryRet || byProcRet)
  {
    m_eState = STATE::CANCEL_FAIL_COMPLETE;
  }
  else
  {
    m_eState = STATE::CANCEL_SUCC_COMPLETE;
  }
}
