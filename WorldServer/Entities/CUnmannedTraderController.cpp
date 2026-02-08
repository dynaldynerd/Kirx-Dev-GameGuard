#include "pch.h"

#include "CUnmannedTraderController.h"
#include "CRFWorldDatabase.h"
#include "CUnmannedTraderEnvironmentValue.h"
#include "CUnmannedTraderGroupItemInfoTable.h"
#include "CUnmannedTraderScheduler.h"
#include "CUnmannedTraderTaxRateManager.h"
#include "CUnmannedTraderUserInfoTable.h"
#include "GlobalObjects.h"
#include "KorLocalTime.h"
#include "WorldServerUtil.h"
#include "unmannedtrader_page_info.h"

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

unsigned __int8 CUnmannedTraderController::GetMaxRegistCnt(unsigned __int16 wInx, unsigned int dwSerial)
{
  CUnmannedTraderUserInfoTable *userInfoTable = CUnmannedTraderUserInfoTable::Instance();
  return userInfoTable->GetMaxRegistCnt(wInx, dwSerial);
}

const CUnmannedTraderRegistItemInfo *CUnmannedTraderController::GetRegItemInfo(
  unsigned __int16 wInx,
  unsigned int dwSerial)
{
  CUnmannedTraderUserInfoTable *userInfoTable = CUnmannedTraderUserInfoTable::Instance();
  return userInfoTable->GetRegItemInfo(wInx, dwSerial);
}

void CUnmannedTraderController::LogOut(unsigned __int16 wInx, unsigned int dwSerial)
{
  CUnmannedTraderUserInfoTable *userInfoTable = CUnmannedTraderUserInfoTable::Instance();
  userInfoTable->LogOut(wInx, dwSerial);
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

unsigned __int8 CUnmannedTraderController::SelectSearchList(
  char *pData,
  CRFWorldDatabase *pkWorldDB,
  unsigned __int8 *byProcRet)
{
  char *reqData = pData;
  if (!pkWorldDB)
  {
    *byProcRet = 60;
    return 1;
  }

  unsigned int count[4]{};
  const unsigned __int8 dbRet = pkWorldDB->Select_UnmannedTraderSearchGroupTotalRowCount(
    static_cast<unsigned __int8>(reqData[8]),
    static_cast<unsigned __int8>(reqData[9]),
    static_cast<unsigned __int8>(reqData[16]),
    static_cast<unsigned __int8>(reqData[17]),
    static_cast<unsigned __int8>(reqData[18]),
    count);
  if (dbRet == 1)
  {
    *byProcRet = 61;
    return 2;
  }
  if (dbRet == 2)
  {
    *byProcRet = 62;
    return 2;
  }

  unsigned int pageCount = count[0] / CUnmannedTraderEnvironmentValue::Unmanned_Trader_Max_Row_Count_Search;
  if (count[0] % CUnmannedTraderEnvironmentValue::Unmanned_Trader_Max_Row_Count_Search)
  {
    ++pageCount;
  }
  *reinterpret_cast<unsigned int *>(reqData + 156) = pageCount;
  if (!pageCount)
  {
    *byProcRet = 62;
    return 0;
  }

  if (static_cast<unsigned __int8>(reqData[24]) >= pageCount)
  {
    *byProcRet = 63;
    return 0;
  }

  const unsigned int excludeCount =
    CUnmannedTraderEnvironmentValue::Unmanned_Trader_Max_Row_Count_Search * static_cast<unsigned __int8>(reqData[24]);
  const unsigned __int8 pageRet = pkWorldDB->Select_UnmannedTraderSearchPageInfo(
    static_cast<unsigned __int8>(reqData[8]),
    static_cast<unsigned __int8>(reqData[9]),
    static_cast<unsigned __int8>(reqData[16]),
    static_cast<unsigned __int8>(reqData[17]),
    static_cast<unsigned __int8>(reqData[18]),
    CUnmannedTraderEnvironmentValue::Unmanned_Trader_Max_Row_Count_Search,
    excludeCount,
    reqData + 25,
    reinterpret_cast<_unmannedtrader_page_info *>(reqData + 160));
  if (pageRet == 1)
  {
    *byProcRet = 64;
    return 2;
  }
  if (pageRet == 2)
  {
    *byProcRet = 65;
    return 2;
  }

  *byProcRet = 0;
  return 0;
}

void CUnmannedTraderController::CompleteSelectSearchList(
  unsigned __int8 byDBRet,
  unsigned __int8 byProcRet,
  char *pLoadData)
{
  CUnmannedTraderUserInfoTable *table = CUnmannedTraderUserInfoTable::Instance();
  table->CompleteSearch(byDBRet, byProcRet, pLoadData);
}
