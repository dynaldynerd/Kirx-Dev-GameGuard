#include "pch.h"

#include "CUnmannedTraderScheduler.h"
#include "CUnmannedTraderEnvironmentValue.h"
#include "DqsDbStructs.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

#include <cstdarg>

CUnmannedTraderScheduler *CUnmannedTraderScheduler::ms_Instance;

CUnmannedTraderScheduler::CUnmannedTraderScheduler()
  : m_iOldDay(0),
    m_bLoad(false),
    m_pkTimer(nullptr),
    m_pkLogger(nullptr),
    m_iterSchedule(),
    m_veckSchdule()
{
}

CUnmannedTraderScheduler::~CUnmannedTraderScheduler()
{
  m_pkLogger = nullptr;
}

CUnmannedTraderScheduler *CUnmannedTraderScheduler::Instance()
{
  if (!CUnmannedTraderScheduler::ms_Instance)
  {
    CUnmannedTraderScheduler::ms_Instance = new CUnmannedTraderScheduler();
  }
  return CUnmannedTraderScheduler::ms_Instance;
}

void CUnmannedTraderScheduler::Destroy()
{
  if (CUnmannedTraderScheduler::ms_Instance)
  {
    delete CUnmannedTraderScheduler::ms_Instance;
    CUnmannedTraderScheduler::ms_Instance = nullptr;
  }
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

bool CUnmannedTraderScheduler::Load()
{
  _unmannedtrader_reserved_schedule_info info{};
  const unsigned __int8 ret = g_Main.m_pWorldDB->Select_UnmannedTraderReservedSchedule(
    CUnmannedTraderEnvironmentValue::Unmanned_Trader_Max_Schdule_Cnt,
    &info);
  if (ret == 2)
  {
    m_bLoad = true;
    return true;
  }
  if (ret)
  {
    return false;
  }

  Update(&info);
  m_bLoad = true;
  return true;
}

void CUnmannedTraderScheduler::Loop()
{
  if (m_bLoad && m_pkTimer && m_pkTimer->CountingTimer())
  {
    DoDayChangedWork();
    if (m_iterSchedule == m_veckSchdule.end())
    {
      PushLoad();
    }
    else if (FindWaitItem())
    {
      if (m_iterSchedule->IsDone() ||
          (g_Main.m_pTimeLimitMgr->Find_Data(m_iterSchedule->GetOwnerSerial()) &&
           g_Main.m_pTimeLimitMgr->Find_Data(m_iterSchedule->GetOwnerSerial())->GetTLStatus() == 99))
      {
        const unsigned int ownerSerial = m_iterSchedule->GetOwnerSerial();
        if (g_Main.m_pTimeLimitMgr->Find_Data(ownerSerial) &&
            g_Main.m_pTimeLimitMgr->Find_Data(ownerSerial)->GetTLStatus() == 99)
        {
          m_iterSchedule->PushClear(true);
        }
        else
        {
          m_iterSchedule->PushClear(false);
        }
        ++m_iterSchedule;
      }
    }
  }
}

void CUnmannedTraderScheduler::SetLogger(CLogFile *pkLogger)
{
  this->m_pkLogger = pkLogger;
}

void CUnmannedTraderScheduler::DoDayChangedWork()
{
  if (IsDayChanged(&m_iOldDay))
  {
    for (unsigned int index = 0; index < m_veckSchdule.size(); ++index)
    {
      m_veckSchdule[index].Clear();
    }
    m_iterSchedule = m_veckSchdule.end();
  }
}

bool CUnmannedTraderScheduler::FindWaitItem()
{
  int count = 0;
  while (true)
  {
    if (static_cast<unsigned int>(count) > m_veckSchdule.size())
    {
      break;
    }
    if (m_iterSchedule == m_veckSchdule.end())
    {
      return false;
    }
    if (m_iterSchedule->IsWait())
    {
      return true;
    }
    ++count;
    ++m_iterSchedule;
  }
  return true;
}

std::vector<CUnmannedTraderSchedule>::iterator CUnmannedTraderScheduler::FindItem(
  unsigned __int8 byType,
  unsigned int dwRegistSerial)
{
  for (auto it = m_veckSchdule.begin(); it != m_veckSchdule.end(); ++it)
  {
    if (it->GetType() == byType && it->GetRegistSerial() == dwRegistSerial)
    {
      return it;
    }
  }
  return m_veckSchdule.end();
}

void CUnmannedTraderScheduler::Update(_unmannedtrader_reserved_schedule_info *pkInfo)
{
  if (!pkInfo->dwCnt || m_veckSchdule.empty())
  {
    return;
  }

  for (unsigned int index = 0; index < pkInfo->dwCnt; ++index)
  {
    m_veckSchdule[index].Clear();
    m_veckSchdule[index].Set(
      pkInfo->list[index].byType,
      pkInfo->list[index].dwItemSerial,
      pkInfo->list[index].tEndTime,
      pkInfo->list[index].dwOwnerSerial,
      pkInfo->list[index].dwK);
  }

  m_iterSchedule = m_veckSchdule.begin();
}

void CUnmannedTraderScheduler::ClearAll()
{
  for (unsigned int index = 0; index < m_veckSchdule.size(); ++index)
  {
    m_veckSchdule[index].Clear();
  }
}

void CUnmannedTraderScheduler::PushLoad()
{
  g_Main.PushDQSData(-1, nullptr, 58, nullptr, 0);
}

void CUnmannedTraderScheduler::CompleteClear(
  unsigned __int8 byDBQueryRet,
  unsigned __int8 byProcRet,
  unsigned __int8 byType,
  unsigned int dwRegistSerial)
{
  auto it = FindItem(byType, dwRegistSerial);
  if (it == m_veckSchdule.end())
  {
    if (!byDBQueryRet && !byProcRet)
    {
      Log(
        "CUnmannedTraderScheduler::CompleteClear( byType(%u), dwRegistSerial(%u) )\r\n"
        "\t\tFindItem( byType, dwRegistSerial ) Fail!\r\n",
        byType,
        dwRegistSerial);
    }
    return;
  }

  it->CompleteClear(byDBQueryRet, byProcRet);
}

void CUnmannedTraderScheduler::CheatPushLoad()
{
  ClearAll();
  PushLoad();
}

void CUnmannedTraderScheduler::Log(const char *fmt, ...)
{
  if (!m_pkLogger)
  {
    return;
  }

  va_list va;
  va_start(va, fmt);
  m_pkLogger->WriteFromArg(fmt, va);
  va_end(va);
}
