#include "pch.h"

#include "CUnmannedTraderItemState.h"
#include "GlobalObjects.h"
#include "qry_case_unmandtrader_updateitemstate.h"

namespace
{
  wchar_t s_utState0[] = L"Wait";
  wchar_t s_utState1[] = L"RegistAtWaitRecord";
  wchar_t s_utState2[] = L"RegistAtNewRecord";
  wchar_t s_utState3[] = L"Sell Complete";
  wchar_t s_utState4[] = L"Sell Complete And Wait";
  wchar_t s_utState5[] = L"Cancel Regist";
  wchar_t s_utState6[] = L"Close For Pass Time Wait Notify";
  wchar_t s_utState7[] = L"Cancel Regist For Server Internal Error";
  wchar_t s_utState8[] = L"Cancel Sell Fail For Server Internal Error";
  wchar_t s_utState9[] = L"Close Fail For Server Internal error";
  wchar_t s_utState10[] = L"Sell Complete Update Wait";
  wchar_t s_utState11[] = L"Close For PassTime";
  wchar_t s_utState12[] = L"Regist Fail For Server Internal Error";
  wchar_t s_utState13[] = L"Close For Pass Time Wait Reregistration";
  wchar_t s_utState14[] = L"NULL";
}

wchar_t *CUnmannedTraderItemState::ms_wszStateName[15] = {
  s_utState0,
  s_utState1,
  s_utState2,
  s_utState3,
  s_utState4,
  s_utState5,
  s_utState6,
  s_utState7,
  s_utState8,
  s_utState9,
  s_utState10,
  s_utState11,
  s_utState12,
  s_utState13,
  s_utState14,
};

CUnmannedTraderItemState::CUnmannedTraderItemState()
  : m_eState(STATE::NONE)
{
  Clear();
}

CUnmannedTraderItemState::~CUnmannedTraderItemState()
{
  Clear();
}

void CUnmannedTraderItemState::Clear()
{
  this->m_eState = STATE::NONE;
}

char CUnmannedTraderItemState::Set(unsigned __int8 byState)
{
  if (byState >= 14)
  {
    return 0;
  }
  this->m_eState = static_cast<STATE>(byState);
  return 1;
}

unsigned int CUnmannedTraderItemState::GetState() const
{
  return static_cast<unsigned int>(m_eState);
}

__int64 CUnmannedTraderItemState::GetMaxStateCnt()
{
  return 14;
}

wchar_t **CUnmannedTraderItemState::GetStateStrList()
{
  return CUnmannedTraderItemState::ms_wszStateName;
}

wchar_t *CUnmannedTraderItemState::GetStateStrW(unsigned int uiInx)
{
  if (uiInx <= 14)
  {
    return CUnmannedTraderItemState::ms_wszStateName[uiInx];
  }
  return CUnmannedTraderItemState::ms_wszStateName[14];
}

char CUnmannedTraderItemState::PushUpdateState(
  unsigned __int8 byType,
  unsigned int dwRegistSerial,
  unsigned __int8 byState,
  unsigned int dwOwnerSerial,
  unsigned __int16 wItemSerial,
  unsigned __int8 byItemTableCode,
  unsigned __int16 wItemTableIndex)
{
  if (CUnmannedTraderItemState::GetMaxStateCnt() <= byState)
  {
    return 0;
  }

  _qry_case_unmandtrader_updateitemstate qry{};
  qry.byType = byType;
  qry.dwRegistSerial = dwRegistSerial;
  qry.byState = byState;
  qry.wItemSerial = wItemSerial;
  qry.dwOwnerSerial = dwOwnerSerial;
  qry.byItemTableCode = byItemTableCode;
  qry.wItemTableIndex = wItemTableIndex;

  const int size = static_cast<int>(qry.size());
  g_Main.PushDQSData(-1, nullptr, 59, reinterpret_cast<char *>(&qry), size);
  return 1;
}
