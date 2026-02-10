#include "pch.h"

#include "CUnmannedTraderSchedule.h"
#include "CUnmannedTraderItemState.h"
#include "CUnmannedTraderUserInfoTable.h"
#include "CPlayer.h"
#include "CPlayerDB.h"
#include "GlobalObjects.h"
#include "InvenKey.h"

CUnmannedTraderSchedule::CUnmannedTraderSchedule()
{
  Clear();
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
    char qryData[80]{};
    qryData[0] = static_cast<char>(m_byType);
    *reinterpret_cast<unsigned int *>(qryData + 4) = m_dwRegistSerial;
    qryData[8] = static_cast<char>(updateState);
    *reinterpret_cast<unsigned int *>(qryData + 12) = m_dwOwnerSerial;
    qryData[16] = static_cast<char>(m_byItemTableCode);
    *reinterpret_cast<unsigned __int16 *>(qryData + 18) = m_wItemTableIndex;

    if (owner)
    {
      strcpy_s(qryData + 20, 13, owner->m_pUserDB->m_szAccountID);
      const char *charName = owner->m_Param.GetCharNameW();
      strcpy_s(qryData + 33, 17, charName);
    }

    g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x3Eu, qryData, 80);
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
