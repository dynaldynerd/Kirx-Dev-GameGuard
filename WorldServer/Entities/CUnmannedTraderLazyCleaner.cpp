#include "pch.h"

#include "CUnmannedTraderLazyCleaner.h"
#include <new>
#include "CUnmannedTraderEnvironmentValue.h"
#include "CMainThread.h"
#include "CRFWorldDatabase.h"
#include "GlobalObjects.h"
#include "qry_case_unmandtrader_lazyclean_flags.h"

CUnmannedTraderLazyCleaner::CUnmannedTraderLazyCleaner()
  : m_pkTimer(nullptr), m_bClearProcess(false), m_uiRetryCnt(0)
{
}

CUnmannedTraderLazyCleaner::~CUnmannedTraderLazyCleaner()
{
  if (m_pkTimer)
  {
    delete m_pkTimer;
    m_pkTimer = nullptr;
  }
}

bool CUnmannedTraderLazyCleaner::Init()
{
  this->m_pkTimer = new (std::nothrow) CMyTimer();
  if (!this->m_pkTimer)
  {
    return false;
  }

  this->m_pkTimer->BeginTimer(CUnmannedTraderEnvironmentValue::UNMANNEDTRADETRADER_LAZYCLEANER_LOOP_DELAY);
  return true;
}

void CUnmannedTraderLazyCleaner::Loop()
{
  if (!m_bClearProcess && m_pkTimer && m_pkTimer->CountingTimer())
  {
    _qry_case_unmandtrader_lazyclean_flags qryData{};
    g_Main.PushDQSData(-1, nullptr, 67, reinterpret_cast<char *>(&qryData), sizeof(qryData));
    m_bClearProcess = true;
  }
}

unsigned __int8 CUnmannedTraderLazyCleaner::UpdateClear(_qry_case_unmandtrader_lazyclean_flags *pFlags)
{
  _SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  const unsigned __int8 ret1 = ProcUpdate(5u, &systemTime, &pFlags->bFlag[0]);
  const unsigned __int8 ret2 = ProcUpdate(3u, &systemTime, &pFlags->bFlag[1]);
  const unsigned __int8 ret3 = ProcUpdate(11, &systemTime, &pFlags->bFlag[2]);
  const unsigned __int8 ret4 = ProcUpdate(7u, &systemTime, &pFlags->bFlag[3]);

  if (ret1 == 1 || ret2 == 1 || ret3 == 1 || ret4 == 1)
  {
    return 24;
  }
  return 0;
}

unsigned __int8 CUnmannedTraderLazyCleaner::ProcUpdate(
  unsigned __int8 byState,
  const _SYSTEMTIME *pCurTime,
  bool *pbRemain)
{
  if (byState >= 0xEu || !g_Main.m_pWorldDB)
  {
    return 0;
  }

  unsigned int pdwSerial[57]{};
  unsigned __int16 pwRecordCnt[8]{};
  unsigned __int16 successCnt = 0;

  unsigned __int8 result = g_Main.m_pWorldDB->Select_UnmannedTraderItemRecordCntByState(
    0,
    byState,
    pdwSerial,
    50,
    pwRecordCnt);
  if (!result)
  {
    for (int j = 0; j < pwRecordCnt[0]; ++j)
    {
      if (g_Main.m_pWorldDB->Update_UnmannedTraderSingleTypeClearUseCompleteRecord(pdwSerial[j], pCurTime))
      {
        ++successCnt;
      }
    }
  }

  *pbRemain = pwRecordCnt[0] > successCnt;
  return result;
}

void CUnmannedTraderLazyCleaner::CompleteUpdateClear(_qry_case_unmandtrader_lazyclean_flags *pFlags)
{
  if (CUnmannedTraderEnvironmentValue::UNMANNEDTRADETRADER_LAZYCLEANER_UPDATE_MAX_RETRY_CNT > m_uiRetryCnt &&
      (pFlags->bFlag[0] || pFlags->bFlag[1] || pFlags->bFlag[2] || pFlags->bFlag[3]))
  {
    _qry_case_unmandtrader_lazyclean_flags qryData{};
    g_Main.PushDQSData(-1, nullptr, 67, reinterpret_cast<char *>(&qryData), sizeof(qryData));
    ++m_uiRetryCnt;
    m_bClearProcess = true;
    return;
  }

  m_bClearProcess = false;
  m_uiRetryCnt = 0;
}
