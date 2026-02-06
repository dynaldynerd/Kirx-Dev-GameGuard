#include "pch.h"

#include "CUnmannedTraderScheduler.h"
#include "CUnmannedTraderEnvironmentValue.h"
#include "WorldServerUtil.h"

CUnmannedTraderScheduler *CUnmannedTraderScheduler::ms_Instance = nullptr;

CUnmannedTraderScheduler::CUnmannedTraderScheduler()
  : m_iOldDay(0),
    m_bLoad(false),
    m_pkTimer(nullptr),
    m_pkLogger(nullptr),
    m_iterSchedule(),
    m_veckSchdule()
{
}

CUnmannedTraderScheduler *CUnmannedTraderScheduler::Instance()
{
  if (!CUnmannedTraderScheduler::ms_Instance)
  {
    CUnmannedTraderScheduler::ms_Instance = new CUnmannedTraderScheduler();
  }
  return CUnmannedTraderScheduler::ms_Instance;
}

bool CUnmannedTraderScheduler::Init()
{
  this->m_pkTimer = new CMyTimer();
  if (!this->m_pkTimer)
  {
    return false;
  }

  CUnmannedTraderSchedule schedule;
  this->m_veckSchdule.assign(CUnmannedTraderEnvironmentValue::Unmanned_Trader_Max_Schdule_Cnt, schedule);
  if (this->m_veckSchdule.size() != CUnmannedTraderEnvironmentValue::Unmanned_Trader_Max_Schdule_Cnt)
  {
    return false;
  }

  this->m_iterSchedule = this->m_veckSchdule.begin();
  this->m_pkTimer->BeginTimer(CUnmannedTraderEnvironmentValue::Unmanned_Trader_Check_Schedule_Delay);
  this->m_iOldDay = GetCurrentDay();
  return true;
}

void CUnmannedTraderScheduler::SetLogger(CLogFile *pkLogger)
{
  this->m_pkLogger = pkLogger;
}
