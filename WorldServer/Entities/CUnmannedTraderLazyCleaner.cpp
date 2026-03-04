#include "pch.h"

#include "CUnmannedTraderLazyCleaner.h"
#include <new>
#include "CUnmannedTraderEnvironmentValue.h"
#include "CMainThread.h"
#include "CRFWorldDatabase.h"
#include "GlobalObjects.h"

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
    char pQryData[4]{};
    g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x43u, pQryData, 4);
    m_bClearProcess = true;
  }
}

unsigned __int8 CUnmannedTraderLazyCleaner::UpdateClear(bool *p)
{
  bool *pbRemain = p;
  _SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  const unsigned __int8 ret1 = ProcUpdate(5u, &systemTime, pbRemain);
  const unsigned __int8 ret2 = ProcUpdate(3u, &systemTime, pbRemain + 1);
  const unsigned __int8 ret3 = ProcUpdate(0xBu, &systemTime, pbRemain + 2);
  const unsigned __int8 ret4 = ProcUpdate(7u, &systemTime, pbRemain + 3);

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
    0x32u,
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

void CUnmannedTraderLazyCleaner::CompleteUpdateClear(char *p)
{
  if (CUnmannedTraderEnvironmentValue::UNMANNEDTRADETRADER_LAZYCLEANER_UPDATE_MAX_RETRY_CNT > m_uiRetryCnt &&
      (p[0] || p[1] || p[2] || p[3]))
  {
    char pQryData[4]{};
    g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x43u, pQryData, 4);
    ++m_uiRetryCnt;
    m_bClearProcess = true;
    return;
  }

  m_bClearProcess = false;
  m_uiRetryCnt = 0;
}
