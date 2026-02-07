#include "pch.h"

#include "TRC_AutoTrade.h"
#include "KorLocalTime.h"

_suggested_matter_change_taxrate::_suggested_matter_change_taxrate()
{
  init();
}

void _suggested_matter_change_taxrate::init()
{
  memset_0(this, 0, sizeof(_suggested_matter_change_taxrate));
  this->dwNext = 5;
}

TRC_AutoTrade::TRC_AutoTrade()
  : m_bInit(false),
    m_byCurDay(0),
    m_wCurMonth(0),
    m_wCurYear(0),
    m_bChangeTaxRate(false),
    m_fCommonTaxRate(0.050000001f),
    m_pOwnerGuild(nullptr),
    m_Controller(),
    m_sysLog(),
    m_serviceLog(),
    m_dwTrade(0),
    m_dIncomeMoney(0.0),
    m_byRace(0),
    m_suggested()
{
  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);
  this->m_byCurDay = systemTime.wDay;
  this->m_wCurMonth = systemTime.wMonth;
  this->m_wCurYear = systemTime.wYear;
}

TRC_AutoTrade::TRC_AutoTrade(unsigned char byRace)
  : m_bInit(false),
    m_byCurDay(0),
    m_wCurMonth(0),
    m_wCurYear(0),
    m_bChangeTaxRate(false),
    m_fCommonTaxRate(0.050000001f),
    m_pOwnerGuild(nullptr),
    m_Controller(),
    m_sysLog(),
    m_serviceLog(),
    m_dwTrade(0),
    m_dIncomeMoney(0.0),
    m_byRace(byRace),
    m_suggested()
{
  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);
  this->m_byCurDay = systemTime.wDay;
  this->m_wCurMonth = systemTime.wMonth;
  this->m_wCurYear = systemTime.wYear;
}

bool TRC_AutoTrade::Initialzie()
{
  CreateDirectoryA("..\\ZoneServerLog\\ServiceLog\\ATradeTax", nullptr);
  CreateDirectoryA("..\\ZoneServerLog\\SystemLog\\Concession", nullptr);

  char buffer[144]{};
  const unsigned int korLocalTime = GetKorLocalTime();
  sprintf(buffer, "..\\ZoneServerLog\\ServiceLog\\ATradeTax\\atrade_earn_%d_%d.log", this->m_byRace, korLocalTime);
  this->m_serviceLog.SetWriteLogFile(buffer, 1, 0, 1, 1);

  const unsigned int korLocalTime2 = GetKorLocalTime();
  sprintf(buffer, "..\\ZoneServerLog\\SystemLog\\Concession\\system_TRC_%d_%d.log", this->m_byRace, korLocalTime2);
  this->m_sysLog.SetWriteLogFile(buffer, 1, 0, 1, 1);

  this->m_Controller.setLimitTaxRate(0.050000001f, 0.2f);
  return true;
}

bool TRC_AutoTrade::IsOwnerGuild(unsigned int nGuildSerial)
{
  return m_pOwnerGuild && nGuildSerial == m_pOwnerGuild->m_dwSerial;
}

float TRC_AutoTrade::get_taxrate()
{
  return m_Controller.getCurTaxRate();
}
