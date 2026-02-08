#include "pch.h"

#include "CUnmannedTraderController.h"

#include <cstdarg>
#include <cstring>
#include "CRFWorldDatabase.h"
#include "CNetProcess.h"
#include "CPlayer.h"
#include "CPlayerDB.h"
#include "CUnmannedTraderEnvironmentValue.h"
#include "CUnmannedTraderGroupItemInfoTable.h"
#include "CUnmannedTraderScheduler.h"
#include "CUnmannedTraderTaxRateManager.h"
#include "CUnmannedTraderUserInfoTable.h"
#include "GlobalObjects.h"
#include "KorLocalTime.h"
#include "UIDGenerator.h"
#include "WorldServerUtil.h"
#include "unmannedtrader_page_info.h"
#include "unmannedtrader_buy_item_info.h"
#include "unmannedtrader_buy_item_result_zocl.h"
#include "unmannedtrader_registsingleitem.h"
#include "unmannedtrader_seller_info.h"

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


unsigned __int8 CUnmannedTraderController::GetEmptyRecordSerial(unsigned int *dwSerial, bool *pbRecordInserted)
{
  *pbRecordInserted = false;
  if (!g_Main.m_pWorldDB->Select_UnmannedTraderSingleItemEmptyRecordSerial(dwSerial))
  {
    return 0;
  }

  if (!g_Main.m_pWorldDB->Insert_UnmannedTraderSingleDefaultRecord(1u) &&
      !g_Main.m_pWorldDB->Insert_UnmannedTraderSingleDefaultRecord(1u))
  {
    return 24;
  }

  *pbRecordInserted = true;
  if (g_Main.m_pWorldDB->Select_UnmannedTraderSingleItemEmptyRecordSerial(dwSerial))
  {
    return 24;
  }

  return 0;
}

unsigned __int8 CUnmannedTraderController::CheckDBItemState(
  unsigned __int8 byType,
  unsigned int dwRegistSerial,
  unsigned __int8 *byState,
  unsigned __int8 *byProcRet)
{
  unsigned __int8 dbRet = g_Main.m_pWorldDB->Select_UnmannedTraderItemState(byType, dwRegistSerial, byState);
  if (dbRet == 2)
  {
    *byProcRet = 8;
    return 0;
  }
  if (dbRet == 1)
  {
    *byProcRet = 38;
    return 24;
  }

  switch (*byState)
  {
    case 0:
    case 8:
    case 9:
      *byProcRet = 45;
      break;
    case 1:
    case 2:
      *byProcRet = 0;
      break;
    case 3:
    case 4:
    case 10:
      *byProcRet = 40;
      break;
    case 5:
    case 6:
    case 7:
      *byProcRet = 41;
      break;
    default:
      return 0;
  }

  return 0;
}

void CUnmannedTraderController::Log(char *fmt, ...)
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

unsigned __int8 CUnmannedTraderController::UpdateItemState(char *pData)
{
  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  if (g_Main.m_pWorldDB->Update_UnmannedTraderItemState(
        static_cast<unsigned __int8>(pData[0]),
        *reinterpret_cast<unsigned int *>(pData + 4),
        static_cast<unsigned __int8>(pData[8]),
        &systemTime))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 CUnmannedTraderController::UpdateRegistItem(char *pData)
{
  bool recordInserted = false;
  unsigned __int8 result = GetEmptyRecordSerial(reinterpret_cast<unsigned int *>(pData) + 5, &recordInserted);
  if (!result)
  {
    _unmannedtrader_registsingleitem info{};
    info.byType = static_cast<unsigned __int8>(pData[25]);
    info.bySellTurm = static_cast<unsigned __int8>(pData[26]);
    info.byRace = static_cast<unsigned __int8>(pData[27]);
    info.dwOwnerSerial = *reinterpret_cast<unsigned int *>(pData + 28);
    info.dwPrice = *reinterpret_cast<unsigned int *>(pData + 32);
    info.byInveninx = static_cast<unsigned __int8>(pData[36]);
    info.dwK = *reinterpret_cast<unsigned int *>(pData + 40);
    info.dwD = *reinterpret_cast<unsigned long long *>(pData + 48);
    info.dwU = *reinterpret_cast<unsigned int *>(pData + 56);
    info.byLv = static_cast<unsigned __int8>(pData[60]);
    info.byGrade = static_cast<unsigned __int8>(pData[61]);
    info.byClass1 = static_cast<unsigned __int8>(pData[62]);
    info.byClass2 = static_cast<unsigned __int8>(pData[63]);
    info.byClass3 = static_cast<unsigned __int8>(pData[64]);
    info.dwT = *reinterpret_cast<unsigned int *>(pData + 68);
    info.lnUID = *reinterpret_cast<unsigned long long *>(pData + 72);
    info.dwTax = *reinterpret_cast<unsigned int *>(pData + 80);

    if (!g_Main.m_pWorldDB->Regist_UnmannedTraderSingleItem(
          *reinterpret_cast<unsigned int *>(pData + 20),
          &info,
          recordInserted))
    {
      return 24;
    }

    pData[24] = recordInserted ? 1 : 0;
  }
  return result;
}


unsigned __int8 CUnmannedTraderController::UpdateCancelRegist(char *pData)
{
  pData[45] = 0;
  unsigned __int8 state = 0xFF;
  unsigned __int8 dbRet = CheckDBItemState(
    static_cast<unsigned __int8>(pData[38]),
    *reinterpret_cast<unsigned int *>(pData + 40),
    &state,
    reinterpret_cast<unsigned __int8 *>(pData + 45));
  if (pData[45])
  {
    return dbRet;
  }

  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  _unmannedtrader_buy_item_info buyInfo{};
  dbRet = g_Main.m_pWorldDB->Select_UnmannedTraderBuySingleItemInfo(
    static_cast<unsigned __int8>(pData[38]),
    *reinterpret_cast<unsigned int *>(pData + 40),
    &buyInfo);
  if (dbRet == 1)
  {
    pData[45] = 49;
    return 24;
  }
  if (dbRet == 2)
  {
    pData[45] = 50;
    return 0;
  }

  if (g_Main.m_pWorldDB->Update_UnmannedTraderItemState(
        static_cast<unsigned __int8>(pData[38]),
        *reinterpret_cast<unsigned int *>(pData + 40),
        static_cast<unsigned __int8>(pData[44]),
        &systemTime))
  {
    return 0;
  }

  pData[45] = 30;
  return 24;
}

unsigned __int8 CUnmannedTraderController::UpdateTimeOutCancelRegist(char *pData)
{
  pData[50] = 0;
  unsigned __int8 state = 0xFF;
  unsigned __int8 dbRet = CheckDBItemState(
    static_cast<unsigned __int8>(pData[0]),
    *reinterpret_cast<unsigned int *>(pData + 4),
    &state,
    reinterpret_cast<unsigned __int8 *>(pData + 50));
  if (pData[50])
  {
    return dbRet;
  }

  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  _unmannedtrader_buy_item_info buyInfo{};
  dbRet = g_Main.m_pWorldDB->Select_UnmannedTraderBuySingleItemInfo(
    static_cast<unsigned __int8>(pData[0]),
    *reinterpret_cast<unsigned int *>(pData + 4),
    &buyInfo);
  if (dbRet == 1)
  {
    pData[50] = 47;
    g_Main.m_pWorldDB->Update_UnmannedTraderItemState(
      static_cast<unsigned __int8>(pData[0]),
      *reinterpret_cast<unsigned int *>(pData + 4),
      9u,
      &systemTime);
    return 24;
  }
  if (dbRet == 2)
  {
    pData[50] = 48;
    return 0;
  }

  if (buyInfo.dwSeller == *reinterpret_cast<unsigned int *>(pData + 12))
  {
    *reinterpret_cast<unsigned int *>(pData + 52) = buyInfo.dwK;
    *reinterpret_cast<unsigned int *>(pData + 56) = static_cast<unsigned int>(buyInfo.dwD);
    *reinterpret_cast<unsigned int *>(pData + 60) = buyInfo.dwU;
    *reinterpret_cast<unsigned int *>(pData + 64) = buyInfo.dwT;
    if (buyInfo.lnUID)
    {
      *reinterpret_cast<unsigned long long *>(pData + 72) = buyInfo.lnUID;
    }
    else
    {
      *reinterpret_cast<unsigned long long *>(pData + 72) = UIDGenerator::getuid(g_Main.m_byWorldCode);
    }

    if (!pData[20] || !pData[33])
    {
      g_Main.m_pWorldDB->Select_CharacterName(
        *reinterpret_cast<unsigned int *>(pData + 12),
        pData + 33,
        pData + 20);
    }

    if (g_Main.m_pWorldDB->Update_UnmannedTraderItemState(
          static_cast<unsigned __int8>(pData[0]),
          *reinterpret_cast<unsigned int *>(pData + 4),
          static_cast<unsigned __int8>(pData[8]),
          &systemTime))
    {
      return 0;
    }

    pData[50] = 46;
    return 24;
  }

  pData[50] = 93;
  return 0;
}

unsigned __int8 CUnmannedTraderController::UpdateRePrice(char *pData)
{
  pData[20] = 0;
  unsigned __int8 state = 0xFF;
  unsigned __int8 dbRet = CheckDBItemState(
    static_cast<unsigned __int8>(pData[21]),
    *reinterpret_cast<unsigned int *>(pData + 24),
    &state,
    reinterpret_cast<unsigned __int8 *>(pData + 20));
  if (pData[20])
  {
    return dbRet;
  }

  if (g_Main.m_pWorldDB->Update_UnmannedTraderSellInfoPrice(
        static_cast<unsigned __int8>(pData[21]),
        *reinterpret_cast<unsigned int *>(pData + 24),
        *reinterpret_cast<unsigned int *>(pData + 8),
        *reinterpret_cast<unsigned int *>(pData + 28)))
  {
    return 0;
  }

  pData[20] = 27;
  return 24;
}


unsigned __int8 CUnmannedTraderController::SelectBuy(char *pData)
{
  unsigned int salesTotals[5]{};
  unsigned long long totalSales = 0;
  unsigned __int8 state = 0xFF;
  _unmannedtrader_seller_info sellerInfo{};

  for (int j = 0; j < static_cast<unsigned __int8>(pData[14]); ++j)
  {
    pData[72 * j + 84] = 0;
    CheckDBItemState(
      static_cast<unsigned __int8>(pData[13]),
      *reinterpret_cast<unsigned int *>(&pData[72 * j + 16]),
      &state,
      reinterpret_cast<unsigned __int8 *>(&pData[72 * j + 84]));
    if (!pData[72 * j + 84])
    {
      unsigned __int8 dbRet = g_Main.m_pWorldDB->Select_UnmannedTraderSellInfo(
        static_cast<unsigned __int8>(pData[13]),
        *reinterpret_cast<unsigned int *>(&pData[72 * j + 16]),
        static_cast<unsigned __int8>(pData[8]),
        &sellerInfo);
      if (dbRet == 1)
      {
        pData[72 * j + 84] = 31;
      }
      else if (dbRet == 2)
      {
        pData[72 * j + 84] = 32;
      }
      else if (sellerInfo.dwPrice == *reinterpret_cast<unsigned int *>(&pData[72 * j + 20]))
      {
        if (sellerInfo.dwSeller == *reinterpret_cast<unsigned int *>(pData + 4))
        {
          pData[72 * j + 84] = 39;
        }
        else
        {
          salesTotals[0] = 0;
          totalSales = 0;
          dbRet = g_Main.m_pWorldDB->Select_utSellWaitItems_SalesTotals(
            static_cast<unsigned __int8>(pData[13]),
            sellerInfo.dwSeller,
            salesTotals);
          if (dbRet == 1)
          {
            pData[72 * j + 84] = 31;
          }
          else
          {
            *reinterpret_cast<unsigned int *>(&pData[72 * j + 20]) = sellerInfo.dwPrice;
            *reinterpret_cast<unsigned int *>(&pData[72 * j + 24]) = sellerInfo.dwSeller;
            pData[72 * j + 28] = sellerInfo.byRaceSexCode;
            totalSales = salesTotals[0] + sellerInfo.dwDalant;
            if (totalSales > 2000000000ULL)
            {
              totalSales = 2000000000ULL;
            }
            *reinterpret_cast<unsigned int *>(&pData[72 * j + 32]) = static_cast<unsigned int>(totalSales);
            *reinterpret_cast<unsigned int *>(&pData[72 * j + 36]) = sellerInfo.dwGuildSerial;
            *reinterpret_cast<unsigned int *>(&pData[72 * j + 44]) = sellerInfo.dwAccountSerial;
            strcpy_s(&pData[72 * j + 48], 13, sellerInfo.szAccountID);
            strcpy_s(&pData[72 * j + 61], 17, sellerInfo.wszName);
            pData[72 * j + 40] = 0;
            if (CUnmannedTraderEnvironmentValue::Unmanned_Trader_Dev_Account_Start_Serial <=
                *reinterpret_cast<unsigned int *>(&pData[72 * j + 44]))
            {
              pData[72 * j + 40] = 3;
            }
          }
        }
      }
      else
      {
        pData[72 * j + 84] = 44;
      }
    }
  }

  return 0;
}

unsigned __int8 CUnmannedTraderController::UpdateBuy(char *pData)
{
  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);
  time_17(reinterpret_cast<__int64 *>(pData) + 2);

  _unmannedtrader_buy_item_info buyInfo{};

  for (int j = 0; j < static_cast<unsigned __int8>(pData[25]); ++j)
  {
    if (!pData[96 * j + 77])
    {
      CheckDBItemState(
        static_cast<unsigned __int8>(pData[24]),
        *reinterpret_cast<unsigned int *>(&pData[96 * j + 32]),
        reinterpret_cast<unsigned __int8 *>(&pData[96 * j + 84]),
        reinterpret_cast<unsigned __int8 *>(&pData[96 * j + 77]));
      if (!pData[96 * j + 77])
      {
        unsigned __int8 dbRet = g_Main.m_pWorldDB->Select_UnmannedTraderBuySingleItemInfo(
          static_cast<unsigned __int8>(pData[24]),
          *reinterpret_cast<unsigned int *>(&pData[96 * j + 32]),
          &buyInfo);
        if (dbRet == 1)
        {
          pData[96 * j + 77] = 31;
        }
        else if (dbRet == 2)
        {
          pData[96 * j + 77] = 32;
        }
        else if (buyInfo.dwPrice == *reinterpret_cast<unsigned int *>(&pData[96 * j + 108]))
        {
          pData[96 * j + 85] = buyInfo.byInveninx;
          *reinterpret_cast<unsigned int *>(&pData[96 * j + 88]) = buyInfo.dwK;
          *reinterpret_cast<unsigned long long *>(&pData[96 * j + 96]) = buyInfo.dwD;
          *reinterpret_cast<unsigned int *>(&pData[96 * j + 104]) = buyInfo.dwU;
          *reinterpret_cast<unsigned int *>(&pData[96 * j + 112]) = buyInfo.dwT;
          if (buyInfo.lnUID)
          {
            *reinterpret_cast<unsigned long long *>(&pData[96 * j + 120]) = buyInfo.lnUID;
          }
          else
          {
            *reinterpret_cast<unsigned long long *>(&pData[96 * j + 120]) = UIDGenerator::getuid(g_Main.m_byWorldCode);
          }
          if (!g_Main.m_pWorldDB->Update_UnmannedTraderResutlInfo(
                static_cast<unsigned __int8>(pData[24]),
                *reinterpret_cast<unsigned int *>(&pData[96 * j + 32]),
                0xAu,
                *reinterpret_cast<unsigned int *>(pData + 4),
                *reinterpret_cast<unsigned int *>(&pData[96 * j + 80]),
                &systemTime))
          {
            pData[96 * j + 77] = 33;
          }
        }
        else
        {
          pData[96 * j + 77] = 44;
        }
      }
    }
  }

  return 0;
}

unsigned __int8 CUnmannedTraderController::UpdateBuyRollBack(char *pData)
{
  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  for (int j = 0; j < static_cast<unsigned __int8>(pData[10]); ++j)
  {
    pData[12 * j + 12] = 0;
    if (!g_Main.m_pWorldDB->Update_UnmannedTraderResutlInfo(
          static_cast<unsigned __int8>(pData[9]),
          *reinterpret_cast<unsigned int *>(&pData[12 * j + 16]),
          static_cast<unsigned __int8>(pData[12 * j + 20]),
          0,
          0,
          &systemTime))
    {
      pData[12 * j + 12] = 36;
    }
  }

  return 0;
}

unsigned __int8 CUnmannedTraderController::UpdateLazyClean(char *pData)
{
  return m_kLazyCleaner.UpdateClear(pData);
}


unsigned __int8 CUnmannedTraderController::UpdateBuyComplete(char *pData)
{
  pData[9] = 1;
  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  for (int j = 0; j < static_cast<unsigned __int8>(pData[11]); ++j)
  {
    pData[16 * j + 12] = 0;
    unsigned __int8 state = static_cast<unsigned __int8>(pData[16 * j + 13]);
    switch (state - 34)
    {
      case 0:
      case 3:
      case 8:
      case 9:
      case 65:
      case 116:
      case 117:
        if (!g_Main.m_pWorldDB->Update_UnmannedTraderResutlInfo(
              static_cast<unsigned __int8>(pData[10]),
              *reinterpret_cast<unsigned int *>(&pData[16 * j + 20]),
              static_cast<unsigned __int8>(pData[16 * j + 24]),
              0,
              0,
              &systemTime))
        {
          pData[16 * j + 12] = 1;
          pData[9] = 0;
        }
        break;
      case 47:
      case 56:
        if (!g_Main.m_pWorldDB->Update_UnmannedTraderItemState(
              static_cast<unsigned __int8>(pData[10]),
              *reinterpret_cast<unsigned int *>(&pData[16 * j + 20]),
              static_cast<unsigned __int8>(pData[16 * j + 24]),
              &systemTime))
        {
          pData[16 * j + 12] = 1;
          pData[9] = 0;
        }
        break;
      default:
        break;
    }
  }

  return 0;
}

unsigned __int8 CUnmannedTraderController::UpdateLogInComplete(char *pData)
{
  pData[8] = 1;
  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  for (int j = 0; j < *reinterpret_cast<unsigned __int16 *>(pData + 10); ++j)
  {
    pData[16 * j + 12] = 0;
    unsigned __int8 state = static_cast<unsigned __int8>(pData[16 * j + 13]);
    switch (state - 37)
    {
      case 0:
      case 45:
      case 46:
      case 53:
      case 54:
      case 57:
        if (!g_Main.m_pWorldDB->Update_UnmannedTraderItemState(
              static_cast<unsigned __int8>(pData[9]),
              *reinterpret_cast<unsigned int *>(&pData[16 * j + 20]),
              static_cast<unsigned __int8>(pData[16 * j + 24]),
              &systemTime))
        {
          pData[16 * j + 12] = 1;
          pData[8] = 0;
        }
        break;
      case 55:
        if (!g_Main.m_pWorldDB->Update_UnmannedTraderResutlInfo(
              static_cast<unsigned __int8>(pData[9]),
              *reinterpret_cast<unsigned int *>(&pData[16 * j + 20]),
              static_cast<unsigned __int8>(pData[16 * j + 24]),
              0,
              0,
              &systemTime))
        {
          pData[16 * j + 12] = 1;
          pData[8] = 0;
        }
        break;
      default:
        break;
    }
  }

  return 0;
}


unsigned __int8 CUnmannedTraderController::UpdateReRegist(char *pData)
{
  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  unsigned __int8 state = 0;
  unsigned __int8 procRet = 0;
  unsigned int registerOwner[4]{};

  for (int j = 0; j < static_cast<unsigned __int8>(pData[4]); ++j)
  {
    state = 0xFF;
    procRet = 0;
    CheckDBItemState(
      static_cast<unsigned __int8>(pData[0]),
      *reinterpret_cast<unsigned int *>(&pData[28 * j + 32]),
      &state,
      &procRet);
    if (procRet == 8 || procRet == 38)
    {
      pData[28 * j + 12] = procRet;
    }
    else if (state == 6 || state == 13)
    {
      if (pData[28 * j + 13])
      {
        if (pData[28 * j + 12])
        {
          if (!g_Main.m_pWorldDB->Update_UnmannedTraderItemState(
                static_cast<unsigned __int8>(pData[0]),
                *reinterpret_cast<unsigned int *>(&pData[28 * j + 32]),
                5u,
                &systemTime))
          {
            pData[28 * j + 12] = 30;
          }
        }
        else
        {
          state = g_Main.m_pWorldDB->Select_UnmannedTraderRegister(
            static_cast<unsigned __int8>(pData[0]),
            *reinterpret_cast<unsigned int *>(&pData[28 * j + 32]),
            registerOwner);
          if (state)
          {
            pData[28 * j + 12] = 24;
          }
          else if (registerOwner[0] == *reinterpret_cast<unsigned int *>(pData + 8))
          {
            if (!g_Main.m_pWorldDB->Update_UnmannedTraderReRegist(
                  static_cast<unsigned __int8>(pData[0]),
                  *reinterpret_cast<unsigned int *>(&pData[28 * j + 32]),
                  static_cast<unsigned __int8>(pData[28 * j + 36]),
                  *reinterpret_cast<unsigned int *>(&pData[28 * j + 28]),
                  *reinterpret_cast<unsigned int *>(&pData[28 * j + 16]),
                  &systemTime))
            {
              pData[28 * j + 12] = 24;
            }
          }
          else
          {
            pData[28 * j + 12] = static_cast<char>(-54);
          }
        }
      }
      else if (!g_Main.m_pWorldDB->Update_UnmannedTraderItemState(
                 static_cast<unsigned __int8>(pData[0]),
                 *reinterpret_cast<unsigned int *>(&pData[28 * j + 32]),
                 static_cast<unsigned __int8>(pData[28 * j + 36]),
                 &systemTime))
      {
        pData[28 * j + 12] = 30;
      }
    }
    else
    {
      pData[28 * j + 12] = static_cast<char>(-53);
    }
  }

  return 0;
}

unsigned __int8 CUnmannedTraderController::UpdateCheatRegistTime(char *pData)
{
  unsigned __int8 *data = reinterpret_cast<unsigned __int8 *>(pData);
  for (unsigned __int8 j = 0; j < data[1]; ++j)
  {
    CheckDBItemState(
      data[0],
      *reinterpret_cast<unsigned int *>(&data[8 * j + 12]),
      &data[8 * j + 9],
      &data[8 * j + 8]);
    if (!data[8 * j + 8] &&
        !g_Main.m_pWorldDB->Update_UnmannedTraderCheatUpdateRegistDate(
          data[0],
          *reinterpret_cast<unsigned int *>(&data[8 * j + 12])))
    {
      data[8 * j + 8] = 30;
    }
  }

  return 0;
}

unsigned __int8 CUnmannedTraderController::UpdateReRegistRollBack(char *pData)
{
  unsigned __int8 *data = reinterpret_cast<unsigned __int8 *>(pData);
  unsigned __int8 state = 0;
  unsigned __int8 procRet = 0;
  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  data[4] = 1;
  for (unsigned __int8 j = 0; j < data[1]; ++j)
  {
    state = 0;
    procRet = 0;
    CheckDBItemState(
      data[0],
      *reinterpret_cast<unsigned int *>(&data[8 * j + 16]),
      &state,
      &procRet);
    if (procRet)
    {
      data[8 * j + 12] = procRet;
      data[4] = 0;
    }
    else if (!g_Main.m_pWorldDB->Update_UnmannedTraderItemState(
               data[0],
               *reinterpret_cast<unsigned int *>(&data[8 * j + 16]),
               data[8 * j + 13],
               &systemTime))
    {
      data[8 * j + 12] = 30;
      data[4] = 0;
    }
  }

  return 0;
}


void CUnmannedTraderController::CompleteSelectReservedSchedule(
  unsigned __int8 byRet,
  _unmannedtrader_reserved_schedule_info *pLoadData)
{
  if (!byRet && pLoadData->dwCnt)
  {
    CUnmannedTraderScheduler *scheduler = CUnmannedTraderScheduler::Instance();
    scheduler->Update(pLoadData);
  }
}

void CUnmannedTraderController::CompleteUpdateState(unsigned __int8 byRet, char *pLoadData)
{
  unsigned __int8 state = static_cast<unsigned __int8>(pLoadData[8]);
  if ((state == 3 || state == 5 || state == 7) && !byRet)
  {
    CUnmannedTraderUserInfoTable *table = CUnmannedTraderUserInfoTable::Instance();
    if (table->CompleteUpdateState(
          *reinterpret_cast<unsigned int *>(pLoadData + 12),
          *reinterpret_cast<unsigned int *>(pLoadData + 4),
          static_cast<unsigned __int8>(pLoadData[8])))
    {
      CUnmannedTraderGroupItemInfoTable *groupTable = CUnmannedTraderGroupItemInfoTable::Instance();
      groupTable->IncreaseVersion(
        static_cast<unsigned __int8>(pLoadData[16]),
        *reinterpret_cast<unsigned __int16 *>(pLoadData + 18));
    }
  }

  if (byRet)
  {
    Log(
      "CUnmannedTraderController::CompleteUpdateState( byRet(%u), char * pLoadData )\r\n"
      "\t\tType(%u) RegistSerial(%u) State(%u) : Owner(%u) TableCode(%u) TableIndex(%u)\r\n"
      "\t\tRET_CODE_SUCCESS != byRet\r\n",
      byRet,
      byRet,
      static_cast<unsigned __int8>(pLoadData[0]),
      *reinterpret_cast<unsigned int *>(pLoadData + 4),
      static_cast<unsigned __int8>(pLoadData[8]),
      *reinterpret_cast<unsigned int *>(pLoadData + 12),
      static_cast<unsigned __int8>(pLoadData[16]));
  }
}

void CUnmannedTraderController::CompleteRegistItem(unsigned __int8 byRet, char *pLoadData)
{
  CUnmannedTraderUserInfoTable *table = CUnmannedTraderUserInfoTable::Instance();
  table->CompleteRegist(byRet, pLoadData);
}

void CUnmannedTraderController::CompleteCancelRegist(unsigned __int8 byRet, char *pLoadData)
{
  CUnmannedTraderUserInfoTable *table = CUnmannedTraderUserInfoTable::Instance();
  table->CompleteCancelRegist(byRet, pLoadData);
}

void CUnmannedTraderController::CompleteTimeOutCancelRegist(unsigned __int8 byRet, char *pLoadData)
{
  if (byRet || pLoadData[50])
  {
    const char *senderName = pLoadData[20] ? pLoadData + 20 : "NULL";
    const char *charName = pLoadData[33] ? pLoadData + 33 : "NULL";
    Log(
      "CUnmannedTraderController::CompleteTimeOutCancelRegist( byRet, char * pLoadData )\r\n"
      "\t\tType(%u) RegistSerial(%u) State(%u) : %s(%u) ID(%s) TableCode(%u) TableIndex(%u)\r\n"
      "\t\tProcRet(%u) RET_CODE_SUCCESS != byRet(%u)\r\n",
      static_cast<unsigned __int8>(pLoadData[0]),
      *reinterpret_cast<unsigned int *>(pLoadData + 4),
      static_cast<unsigned __int8>(pLoadData[8]),
      charName,
      *reinterpret_cast<unsigned int *>(pLoadData + 12),
      senderName,
      static_cast<unsigned __int8>(pLoadData[16]),
      *reinterpret_cast<unsigned __int16 *>(pLoadData + 18),
      static_cast<unsigned __int8>(pLoadData[50]),
      byRet);
  }
  else
  {
    CUnmannedTraderUserInfoTable *table = CUnmannedTraderUserInfoTable::Instance();
    table->CompleteTimeOutClear(pLoadData);
    CUnmannedTraderGroupItemInfoTable *groupTable = CUnmannedTraderGroupItemInfoTable::Instance();
    groupTable->IncreaseVersion(
      static_cast<unsigned __int8>(pLoadData[16]),
      *reinterpret_cast<unsigned __int16 *>(pLoadData + 18));
  }

  CUnmannedTraderScheduler *scheduler = CUnmannedTraderScheduler::Instance();
  scheduler->CompleteClear(
    byRet,
    static_cast<unsigned __int8>(pLoadData[50]),
    static_cast<unsigned __int8>(pLoadData[0]),
    *reinterpret_cast<unsigned int *>(pLoadData + 4));
}

void CUnmannedTraderController::CompleteReprice(unsigned __int8 byRet, char *pLoadData)
{
  CUnmannedTraderUserInfoTable *table = CUnmannedTraderUserInfoTable::Instance();
  table->CompleteReprice(byRet, pLoadData);
}


void CUnmannedTraderController::CompleteSelectBuyInfo(unsigned __int8 byRet, char *pLoadData)
{
  char qryData[2]{};
  unsigned int ownerSerial = 0;
  char byType = 0;
  char byRace = 0;
  char byListNum = 0;
  char bySellTurn = 0;

  std::memset(qryData, 0, sizeof(qryData));
  *reinterpret_cast<unsigned __int16 *>(qryData) = *reinterpret_cast<unsigned __int16 *>(pLoadData);
  ownerSerial = *reinterpret_cast<unsigned int *>(pLoadData + 4);
  byType = pLoadData[8];
  bySellTurn = pLoadData[13];
  byListNum = pLoadData[14];
  byRace = pLoadData[10];
  char byClass1 = pLoadData[11];
  char byClass2 = pLoadData[12];

  unsigned int addMoney = 0;
  unsigned char addMoneyCount = 0;
  char destination[960]{};
  unsigned int listData[2]{};

  for (int j = 0; j < static_cast<unsigned __int8>(pLoadData[14]); ++j)
  {
    listData[24 * j] = *reinterpret_cast<unsigned int *>(&pLoadData[72 * j + 16]);
    listData[24 * j + 1] = *reinterpret_cast<unsigned int *>(&pLoadData[72 * j + 24]);
    strcpy_s(&destination[96 * j], 13, &pLoadData[72 * j + 48]);
    *reinterpret_cast<unsigned int *>(&destination[96 * j + 16]) =
      *reinterpret_cast<unsigned int *>(&pLoadData[72 * j + 44]);
    strcpy_s(&destination[96 * j + 20], 17, &pLoadData[72 * j + 61]);
    *reinterpret_cast<unsigned int *>(&destination[96 * j + 68]) =
      *reinterpret_cast<unsigned int *>(&pLoadData[72 * j + 20]);
    destination[96 * j + 37] = pLoadData[72 * j + 84];

    if (!destination[96 * j + 37])
    {
      CPlayer *seller = GetPtrPlayerFromSerial(g_Player, 2532, *reinterpret_cast<unsigned int *>(&pLoadData[72 * j + 24]));
      if (seller && !seller->m_bLive)
      {
        pLoadData[72 * j + 28] = CPlayerDB::GetRaceSexCode(&seller->m_Param);
        *reinterpret_cast<unsigned int *>(&pLoadData[72 * j + 32]) = CPlayerDB::GetDalant(&seller->m_Param);
        *reinterpret_cast<unsigned int *>(&pLoadData[72 * j + 36]) = CPlayerDB::GetGuildSerial(&seller->m_Param);
        pLoadData[72 * j + 40] = seller->m_byUserDgr;
      }

      CUnmannedTraderTaxRateManager *taxMgr = CUnmannedTraderTaxRateManager::Instance();
      unsigned int tax = taxMgr->GetTax(
        static_cast<unsigned __int8>(pLoadData[8]),
        *reinterpret_cast<unsigned int *>(&pLoadData[72 * j + 36]),
        *reinterpret_cast<unsigned int *>(&pLoadData[72 * j + 20]));
      *reinterpret_cast<unsigned int *>(&destination[96 * j + 40]) = tax;

      if (*reinterpret_cast<unsigned int *>(&pLoadData[72 * j + 20]) <=
          *reinterpret_cast<unsigned int *>(&destination[96 * j + 40]))
      {
        Log(
          "CUnmannedTraderController::CompleteSelectBuyInfo(...) Exceed Tax Price!\r\n"
          "\t\t dwRegistSerial(%u) dwSeller(%u)\r\n"
          "\t\t dwBuyer(%u) dwGuildSerial(%u) dwCurDalant(%u)\r\n"
          "\t\t dwPrice(%u) dwTax(%u)\r\n",
          listData[24 * j],
          listData[24 * j + 1],
          ownerSerial,
          *reinterpret_cast<unsigned int *>(&pLoadData[72 * j + 36]),
          *reinterpret_cast<unsigned int *>(&pLoadData[72 * j + 32]),
          *reinterpret_cast<unsigned int *>(&pLoadData[72 * j + 20]),
          *reinterpret_cast<unsigned int *>(&destination[96 * j + 40]));
        *reinterpret_cast<unsigned int *>(&destination[96 * j + 40]) =
          static_cast<int>(static_cast<float>(*reinterpret_cast<int *>(&pLoadData[72 * j + 20])) * 0.050000001f);
      }

      addMoney += *reinterpret_cast<unsigned int *>(&pLoadData[72 * j + 20]) -
                  *reinterpret_cast<unsigned int *>(&destination[96 * j + 40]);
      if (CanAddMoneyForMaxLimMoney(addMoney, *reinterpret_cast<unsigned int *>(&pLoadData[72 * j + 32])))
      {
        if (pLoadData[9] == pLoadData[72 * j + 40])
        {
          ++addMoneyCount;
        }
        else
        {
          destination[96 * j + 37] = 35;
        }
      }
      else
      {
        destination[96 * j + 37] = 34;
      }
    }
  }

  if (addMoneyCount)
  {
    g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x41u, qryData, 992);
  }
  else
  {
    CPlayer *buyer = GetPtrPlayerFromSerial(g_Player, 2532, ownerSerial);
    if (buyer && buyer->m_bOper)
    {
      _unmannedtrader_buy_item_result_zocl result{};
      result.byRetCode = static_cast<unsigned __int8>(-1);
      result.byNum = static_cast<unsigned __int8>(pLoadData[14]);
      for (int k = 0; k < static_cast<unsigned __int8>(pLoadData[14]); ++k)
      {
        result.List[k].byRet = destination[96 * k + 37];
        result.List[k].dwPrice = *reinterpret_cast<unsigned int *>(&destination[96 * k + 68]);
      }
      result.dwLeftDalant = CPlayerDB::GetDalant(&buyer->m_Param);

      unsigned __int8 type[28]{};
      type[0] = 30;
      type[1] = 31;
      unsigned __int16 size = static_cast<unsigned __int16>(result.size());
      g_Network.m_pProcess[0]->LoadSendMsg(buyer->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&result), size);
    }

    CUnmannedTraderUserInfoTable *table = CUnmannedTraderUserInfoTable::Instance();
    table->ClearRequest(*reinterpret_cast<unsigned __int16 *>(qryData), ownerSerial);
  }
}

void CUnmannedTraderController::CompleteBuy(unsigned __int8 byRet, char *pLoadData)
{
  CUnmannedTraderUserInfoTable *table = CUnmannedTraderUserInfoTable::Instance();
  table->CompleteBuy(byRet, pLoadData, &m_kTradeInfo);
}

void CUnmannedTraderController::CompleteBuyRollBack(unsigned __int8 byRet, char *pLoadData)
{
  for (int j = 0; j < static_cast<unsigned __int8>(pLoadData[10]); ++j)
  {
    const char *result = pLoadData[12 * j + 12] ? "Fail" : "Success";
    Log(
      "CUnmannedTraderController::CompleteBuyRollBack( BYTE byRet, char * pLoadData )\r\n"
      "\t\tType(%u) Regist Serial(%u), Old State(%u) wInx(%u) Race(%u) BuyerSerial(%u) RollBack %s\r\n",
      static_cast<unsigned __int8>(pLoadData[9]),
      *reinterpret_cast<unsigned int *>(&pLoadData[12 * j + 16]),
      static_cast<unsigned __int8>(pLoadData[12 * j + 20]),
      *reinterpret_cast<unsigned __int16 *>(pLoadData),
      static_cast<unsigned __int8>(pLoadData[8]),
      *reinterpret_cast<unsigned int *>(pLoadData + 4),
      result);
  }
}

void CUnmannedTraderController::ComleteLazyClean(char *pData)
{
  m_kLazyCleaner.CompleteUpdateClear(pData);
}

void CUnmannedTraderController::CompleteBuyComplete(char *pData)
{
  if (!pData[9])
  {
    Log(
      "CUnmannedTraderController::CompleteBuyRollBack( BYTE byRet, char * pLoadData )\r\n"
      "\t\tType(%u) wInx(%u) Race(%u) BuyerSerial(%u)\r\n",
      static_cast<unsigned __int8>(pData[10]),
      *reinterpret_cast<unsigned __int16 *>(pData),
      static_cast<unsigned __int8>(pData[8]),
      *reinterpret_cast<unsigned int *>(pData + 4));
    for (int j = 0; j < static_cast<unsigned __int8>(pData[11]); ++j)
    {
      if (static_cast<unsigned __int8>(pData[16 * j + 13]) != 255 && pData[16 * j + 12])
      {
        Log(
          "\t\t(%d)Nth Regist Serial(%u) dwSeller(%u) UpdateState(%u) byProcUpdate(%u) DB Error!\r\n",
          j,
          *reinterpret_cast<unsigned int *>(&pData[16 * j + 20]),
          *reinterpret_cast<unsigned int *>(&pData[16 * j + 16]),
          static_cast<unsigned __int8>(pData[16 * j + 24]),
          static_cast<unsigned __int8>(pData[16 * j + 13]));
      }
    }
  }
}

void CUnmannedTraderController::CompleteLogInCompete(char *pData)
{
  if (!pData[8])
  {
    Log(
      "CUnmannedTraderController::CompleteLogInCompete( BYTE byRet, char * pLoadData )\r\n"
      "\t\tType(%u) wInx(%u) dwSeller(%u)\r\n",
      static_cast<unsigned __int8>(pData[9]),
      *reinterpret_cast<unsigned __int16 *>(pData),
      *reinterpret_cast<unsigned int *>(pData + 4));
    for (int j = 0; j < *reinterpret_cast<unsigned __int16 *>(pData + 10); ++j)
    {
      if (static_cast<unsigned __int8>(pData[16 * j + 13]) != 255 && pData[16 * j + 12])
      {
        Log(
          "\t\t(%d)Nth Regist Serial(%u) dwBuyer(%u) UpdateState(%u) byProcUpdate(%u) DB Error!\r\n",
          j,
          *reinterpret_cast<unsigned int *>(&pData[16 * j + 20]),
          *reinterpret_cast<unsigned int *>(&pData[16 * j + 16]),
          static_cast<unsigned __int8>(pData[16 * j + 24]),
          static_cast<unsigned __int8>(pData[16 * j + 13]));
      }
    }
  }
}

void CUnmannedTraderController::CompleteReRegist(char *pLoadData)
{
  CUnmannedTraderUserInfoTable *table = CUnmannedTraderUserInfoTable::Instance();
  table->CompleteReRegist(pLoadData);
}

void CUnmannedTraderController::CompleteUpdateCheatRegistTime(char *pLoadData)
{
  unsigned __int8 *data = reinterpret_cast<unsigned __int8 *>(pLoadData);
  if (*reinterpret_cast<unsigned __int16 *>(pLoadData + 2) < 0x9E4u && data[1])
  {
    CUnmannedTraderUserInfoTable *table = CUnmannedTraderUserInfoTable::Instance();
    table->CompleteUpdateCheatRegistTime(pLoadData);
    CUnmannedTraderScheduler *scheduler = CUnmannedTraderScheduler::Instance();
    scheduler->CheatPushLoad();

    CPlayer *player = GetPtrPlayerFromSerial(g_Player, 2532, *reinterpret_cast<unsigned int *>(data + 4));
    if (player && player->m_bOper)
    {
      char buffer[260]{};
      sprintf(buffer, "Cnt : %u", data[1]);
      CPlayer::SendData_ChatTrans(player, 0, 0xFFFFFFFF, 0xFFu, 0, buffer, 0xFFu, 0);
      sprintf(buffer, "th  serial  state  ret");
      CPlayer::SendData_ChatTrans(player, 0, 0xFFFFFFFF, 0xFFu, 0, buffer, 0xFFu, 0);
      for (unsigned __int8 j = 0; j < data[1]; ++j)
      {
        sprintf(
          buffer,
          " %u    %u       %u     %u",
          j,
          *reinterpret_cast<unsigned int *>(&data[8 * j + 12]),
          static_cast<unsigned __int8>(data[8 * j + 9]),
          static_cast<unsigned __int8>(data[8 * j + 8]));
        CPlayer::SendData_ChatTrans(player, 0, 0xFFFFFFFF, 0xFFu, 0, buffer, 0xFFu, 0);
      }
    }
  }
}

void CUnmannedTraderController::CompleteReRegistRollBack(char *pData)
{
  unsigned __int8 *data = reinterpret_cast<unsigned __int8 *>(pData);
  CUnmannedTraderUserInfoTable *table = CUnmannedTraderUserInfoTable::Instance();
  table->CompleteReRegistRollBack(
    *reinterpret_cast<unsigned __int16 *>(data + 2),
    *reinterpret_cast<unsigned int *>(data + 8),
    pData);
}
