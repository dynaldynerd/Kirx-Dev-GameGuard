#include "pch.h"

#include "CUnmannedTraderController.h"
#include "CUnmannedTraderGroupItemInfoTable.h"
#include "CUnmannedTraderScheduler.h"
#include "CUnmannedTraderTaxRateManager.h"
#include "CUnmannedTraderUserInfoTable.h"
#include "GlobalObjects.h"
#include "KorLocalTime.h"
#include "WorldServerUtil.h"

CUnmannedTraderController *CUnmannedTraderController::Instance()
{
  static CUnmannedTraderController s_instance;
  return &s_instance;
}

bool CUnmannedTraderController::Init()
{
  if (!InitLogger())
  {
    return false;
  }

  CUnmannedTraderUserInfoTable *userInfoTable = CUnmannedTraderUserInfoTable::Instance();
  if (!userInfoTable->Init())
  {
    return false;
  }

  CUnmannedTraderGroupItemInfoTable *groupItemInfoTable = CUnmannedTraderGroupItemInfoTable::Instance();
  if (!groupItemInfoTable->Init())
  {
    return false;
  }

  CUnmannedTraderScheduler *scheduler = CUnmannedTraderScheduler::Instance();
  if (!scheduler->Init())
  {
    return false;
  }

  CUnmannedTraderTaxRateManager *taxRateManager = CUnmannedTraderTaxRateManager::Instance();
  return taxRateManager->Init(this->m_pkLogger) && this->m_kLazyCleaner.Init();
}

bool CUnmannedTraderController::InitLogger()
{
  CreateDirectoryA("..\\ZoneServerLog\\Systemlog\\UnmannedTrader", nullptr);
  clear_file("..\\ZoneServerLog\\Systemlog\\UnmannedTrader", 0xF);

  this->m_pkLogger = new CLogFile();
  if (!this->m_pkLogger)
  {
    g_Main.m_logLoadingError.Write("CUnmannedTraderController::InitLogger() NULL == new CLogFile!\r\n");
    return false;
  }

  const unsigned int korLocalTime = GetKorLocalTime();
  char path[128]{};
  sprintf_s(path, "..\\ZoneServerLog\\Systemlog\\UnmannedTrader\\UnmannedTrader%d.log", korLocalTime);
  this->m_pkLogger->SetWriteLogFile(path, 1, 0, 1, 1);

  CreateDirectoryA("..\\ServiceLog\\UnmannedTrader", nullptr);
  clear_file("..\\ServiceLog\\UnmannedTrader", 0xF);

  this->m_pkServiceLogger = new CLogFile();
  if (!this->m_pkServiceLogger)
  {
    g_Main.m_logLoadingError.Write("CUnmannedTraderController::InitLogger() NULL == new CLogFile!\r\n");
    return false;
  }

  sprintf_s(path, "..\\ZoneServerLog\\ServiceLog\\UnmannedTrader\\UnmannedTraderService.log", korLocalTime);
  this->m_pkServiceLogger->SetWriteLogFile(path, 1, 0, 1, 1);

  CUnmannedTraderGroupItemInfoTable *groupItemInfoTable = CUnmannedTraderGroupItemInfoTable::Instance();
  groupItemInfoTable->SetLogger(this->m_pkLogger);

  CUnmannedTraderUserInfoTable *userInfoTable = CUnmannedTraderUserInfoTable::Instance();
  userInfoTable->SetLogger(this->m_pkLogger, this->m_pkServiceLogger);

  CUnmannedTraderScheduler *scheduler = CUnmannedTraderScheduler::Instance();
  scheduler->SetLogger(this->m_pkLogger);

  return true;
}
