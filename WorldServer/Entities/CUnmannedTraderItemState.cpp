#include "pch.h"

#include "CUnmannedTraderItemState.h"
#include "GlobalObjects.h"
#include "qry_case_unmandtrader_updateitemstate.h"

namespace
{
  wchar_t s_utState0[] = L"STATE_0";
  wchar_t s_utState1[] = L"STATE_1";
  wchar_t s_utState2[] = L"STATE_2";
  wchar_t s_utState3[] = L"STATE_3";
  wchar_t s_utState4[] = L"STATE_4";
  wchar_t s_utState5[] = L"STATE_5";
  wchar_t s_utState6[] = L"STATE_6";
  wchar_t s_utState7[] = L"STATE_7";
  wchar_t s_utState8[] = L"STATE_8";
  wchar_t s_utState9[] = L"STATE_9";
  wchar_t s_utState10[] = L"STATE_10";
  wchar_t s_utState11[] = L"STATE_11";
  wchar_t s_utState12[] = L"STATE_12";
  wchar_t s_utState13[] = L"STATE_13";
  wchar_t s_utState14[] = L"STATE_14";
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

void CUnmannedTraderItemState::Clear()
{
  this->m_eState = STATE::NONE;
}

char CUnmannedTraderItemState::Set(unsigned __int8 byState)
{
  if (byState >= 0xEu)
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
  if (uiInx <= 0xEu)
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
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x3Bu, reinterpret_cast<char *>(&qry), size);
  return 1;
}
