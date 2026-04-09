#include "pch.h"

#include "CUnmannedTraderController.h"

#include <cstdarg>
#include <cstring>
#include <cwchar>
#include "CRFWorldDatabase.h"
#include "CNetProcess.h"
#include "CPlayer.h"
#include "CPlayerDB.h"
#include "CUnmannedTraderEnvironmentValue.h"
#include "CUnmannedTraderGroupItemInfoTable.h"
#include "CUnmannedTraderItemState.h"
#include "CUnmannedTraderScheduler.h"
#include "CUnmannedTraderTaxRateManager.h"
#include "CUnmannedTraderUserInfoTable.h"
#include "GlobalObjects.h"
#include "KorLocalTime.h"
#include "UIDGenerator.h"
#include "WorldServerUtil.h"
#include "lt_qry_case_unmandtrader_select_list.h"
#include "qry_case_unmandtrader_cancelitem.h"
#include "qry_case_unmandtrader_buy_get_info.h"
#include "qry_case_unmandtrader_buy_update.h"
#include "qry_case_unmandtrader_cheat_updateregisttime.h"
#include "qry_case_unmandtrader_lazyclean_flags.h"
#include "qry_case_unmandtrader_log_in_proc_update_complete.h"
#include "qry_case_unmandtrader_re_registsingleitem.h"
#include "qry_case_unmandtrader_re_registsingleitem_roll_back.h"
#include "qry_case_unmandtrader_registsingleitem.h"
#include "qry_case_unmandtrader_time_out_cancelitem.h"
#include "qry_case_unmandtrader_update_reprice.h"
#include "qry_case_unmandtrader_updateitemstate.h"
#include "unmannedtrader_page_info.h"
#include "unmannedtrader_buy_item_info.h"
#include "unmannedtrader_buy_item_result_zocl.h"
#include "unmannedtrader_registsingleitem.h"
#include "unmannedtrader_result_buyerinfo.h"
#include "unmannedtrader_seller_info.h"
#include "unmannedtrader_stade_id_info.h"

CUnmannedTraderController *CUnmannedTraderController::ms_Instance = nullptr;

CUnmannedTraderController *CUnmannedTraderController::Instance()
{
  if (!ms_Instance)
  {
    ms_Instance = new CUnmannedTraderController();
  }

  return ms_Instance;
}

void CUnmannedTraderController::Destroy()
{
  if (ms_Instance)
  {
    delete ms_Instance;
    ms_Instance = nullptr;
  }
}

CUnmannedTraderController::CUnmannedTraderController()
  : m_pkLogger(nullptr), m_pkServiceLogger(nullptr), m_kTradeInfo(), m_kLazyCleaner()
{
}

CUnmannedTraderController::~CUnmannedTraderController()
{
  if (m_pkLogger)
  {
    delete m_pkLogger;
    m_pkLogger = nullptr;
  }
  if (m_pkServiceLogger)
  {
    delete m_pkServiceLogger;
    m_pkServiceLogger = nullptr;
  }

  CUnmannedTraderUserInfoTable::Destroy();
  CUnmannedTraderScheduler::Destroy();
}

void CUnmannedTraderController::Loop()
{
  CUnmannedTraderScheduler::Instance()->Loop();
  CUnmannedTraderTaxRateManager::Instance()->Loop();
  m_kTradeInfo.Loop();
  m_kLazyCleaner.Loop();
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

bool CUnmannedTraderController::Load()
{
  if (!InsertStateRecord())
  {
    return false;
  }

  if (!UpdateClearDanglingOwnerRecord())
  {
    return false;
  }

  if (!InsertDefalutRecord())
  {
    return false;
  }

  CUnmannedTraderScheduler *scheduler = CUnmannedTraderScheduler::Instance();
  return scheduler->Load() && this->m_kTradeInfo.Init();
}

bool CUnmannedTraderController::Load(unsigned __int16 wInx, unsigned int dwSerial, _TRADE_DB_BASE *kInfo)
{
  CUnmannedTraderUserInfoTable *userInfoTable = CUnmannedTraderUserInfoTable::Instance();
  return userInfoTable->Load(0, wInx, dwSerial, kInfo);
}

void CUnmannedTraderController::Regist(unsigned __int16 wInx, _a_trade_reg_item_request_clzo *pRequest)
{
  CUnmannedTraderUserInfoTable *userInfoTable = CUnmannedTraderUserInfoTable::Instance();
  userInfoTable->Regist(wInx, 0, pRequest);
}

void CUnmannedTraderController::ModifyPrice(unsigned __int16 wInx, _a_trade_adjust_price_request_clzo *pRequest)
{
  CUnmannedTraderUserInfoTable *userInfoTable = CUnmannedTraderUserInfoTable::Instance();
  userInfoTable->ModifyPrice(wInx, 0, pRequest);
}

void CUnmannedTraderController::CancelRegist(unsigned __int16 wInx, _a_trade_clear_item_request_clzo *pRequest)
{
  CUnmannedTraderUserInfoTable *userInfoTable = CUnmannedTraderUserInfoTable::Instance();
  userInfoTable->CancelRegist(wInx, 0, pRequest);
}

void CUnmannedTraderController::Buy(unsigned __int16 wInx, _unmannedtrader_buy_item_request_clzo *pRequest)
{
  CUnmannedTraderUserInfoTable *userInfoTable = CUnmannedTraderUserInfoTable::Instance();
  userInfoTable->Buy(wInx, 0, pRequest);
}

void CUnmannedTraderController::Search(unsigned __int16 wInx, _unmannedtrader_search_list_request_clzo *pRequest)
{
  CUnmannedTraderUserInfoTable *userInfoTable = CUnmannedTraderUserInfoTable::Instance();
  userInfoTable->Search(wInx, 0, pRequest);
}

void CUnmannedTraderController::ReRegist(unsigned __int16 wInx, _unmannedtrader_re_regist_request_clzo *pRequest)
{
  CUnmannedTraderUserInfoTable *userInfoTable = CUnmannedTraderUserInfoTable::Instance();
  userInfoTable->ReRegist(wInx, 0, pRequest);
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

void CUnmannedTraderController::CompleteCreate(unsigned __int16 wInx)
{
  CUnmannedTraderUserInfoTable *userInfoTable = CUnmannedTraderUserInfoTable::Instance();
  userInfoTable->CompleteCreate(wInx);

  CUnmannedTraderTaxRateManager *taxRateManager = CUnmannedTraderTaxRateManager::Instance();
  taxRateManager->CompleteCreate(wInx);
}

void CUnmannedTraderController::CompleteCreateNotifyTradeInfo(unsigned __int8 byRace, unsigned __int16 wInx)
{
  m_kTradeInfo.NotifyIncome(byRace, wInx);
}

bool CUnmannedTraderController::CheatCancelRegist(
  unsigned __int16 wInx,
  unsigned int dwOwnerSerial,
  unsigned __int8 byNth)
{
  CUnmannedTraderUserInfoTable *userInfoTable = CUnmannedTraderUserInfoTable::Instance();
  return userInfoTable->CheatCancelRegist(wInx, dwOwnerSerial, byNth);
}

bool CUnmannedTraderController::InitLogger()
{
  CreateDirectoryA("..\\ZoneServerLog\\Systemlog\\UnmannedTrader", nullptr);
  clear_file("..\\ZoneServerLog\\Systemlog\\UnmannedTrader", 15);

  this->m_pkLogger = new CLogFile();
  if (!this->m_pkLogger)
  {
    g_Main.m_logLoadingError.Write("CUnmannedTraderController::InitLogger() NULL == new CLogFile!\r\n");
    return false;
  }

  const unsigned int korLocalTime = static_cast<unsigned int>(GetKorLocalTime());
  char path[128]{};
  sprintf_s(path, sizeof(path), "..\\ZoneServerLog\\Systemlog\\UnmannedTrader\\UnmannedTrader%u.log", korLocalTime);
  this->m_pkLogger->SetWriteLogFile(path, 1, 0, 1, 1);

  CreateDirectoryA("..\\ServiceLog\\UnmannedTrader", nullptr);
  clear_file("..\\ServiceLog\\UnmannedTrader", 15);

  this->m_pkServiceLogger = new CLogFile();
  if (!this->m_pkServiceLogger)
  {
    g_Main.m_logLoadingError.Write("CUnmannedTraderController::InitLogger() NULL == new CLogFile!\r\n");
    return false;
  }

  sprintf_s(path, sizeof(path), "..\\ZoneServerLog\\ServiceLog\\UnmannedTrader\\UnmannedTraderService.log");
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
  _lt_qry_case_unmandtrader_select_list *pData,
  CRFWorldDatabase *pkWorldDB,
  unsigned __int8 *byProcRet)
{
  if (!pkWorldDB)
  {
    *byProcRet = 60;
    return 1;
  }

  unsigned int count[4]{};
  const unsigned __int8 dbRet = pkWorldDB->Select_UnmannedTraderSearchGroupTotalRowCount(
    pData->byType,
    pData->byRace,
    pData->byClass1,
    pData->byClass2,
    pData->byClass3,
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
  pData->dwMaxPage = pageCount;
  if (!pageCount)
  {
    *byProcRet = 62;
    return 0;
  }

  if (static_cast<unsigned int>(pData->byPage) >= pageCount)
  {
    *byProcRet = 63;
    return 0;
  }

  const unsigned int excludeCount =
    CUnmannedTraderEnvironmentValue::Unmanned_Trader_Max_Row_Count_Search * pData->byPage;
  const unsigned __int8 pageRet = pkWorldDB->Select_UnmannedTraderSearchPageInfo(
    pData->byType,
    pData->byRace,
    pData->byClass1,
    pData->byClass2,
    pData->byClass3,
    CUnmannedTraderEnvironmentValue::Unmanned_Trader_Max_Row_Count_Search,
    excludeCount,
    pData->szSortQuery,
    reinterpret_cast<_unmannedtrader_page_info *>(&pData->dwCnt));
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
  _lt_qry_case_unmandtrader_select_list *pLoadData)
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

bool CUnmannedTraderController::InsertStateRecord()
{
  unsigned int stateCount = 0;
  char result = g_Main.m_pWorldDB->Select_UnmannedTraderItemStateInfoCnt(&stateCount);
  if (!result || result == 2)
  {
    _unmannedtrader_stade_id_info *stateInfo =
      static_cast<_unmannedtrader_stade_id_info *>(operator new[](saturated_mul(stateCount, 260)));
    if (!stateInfo)
    {
      Log(
        "CUnmannedTraderController::InsertStateRecord()\r\n\t\tnew _unmannedtrader_stade_id_info[%u] NULL!\r\n",
        stateCount);
      return false;
    }

    result = g_Main.m_pWorldDB->Select_UnmannedTraderItemStateInfo(stateInfo, stateCount);
    if (!result || result == 2)
    {
      unsigned int maxCompare = stateCount >= 14 ? 14 : stateCount;
      bool matched = true;
      for (unsigned int uiInx = 0; uiInx < maxCompare; ++uiInx)
      {
        const wchar_t *stateStr = CUnmannedTraderItemState::GetStateStrW(uiInx);
        if (wcscmp(stateStr, stateInfo[uiInx].wszDesc) != 0)
        {
          matched = false;
          break;
        }
      }

      operator delete(stateInfo);

      if (matched && stateCount == 14)
      {
        return true;
      }

      if (g_Main.m_pWorldDB->Truncate_UnmannedTraderItemStateRecord())
      {
        wchar_t **stateList = CUnmannedTraderItemState::GetStateStrList();
        unsigned int maxStateCnt = CUnmannedTraderItemState::GetMaxStateCnt();
        if (g_Main.m_pWorldDB->Insert_UnmannedTraderItemStateRecord(maxStateCnt, stateList))
        {
          return true;
        }

        Log(
          "CUnmannedTraderController::InsertStateRecord()\r\n"
          "\t\tg_Main.m_pWorldDB->Insert_UnmannedTraderItemStateRecord( %u, wszStateName ) Fail!\r\n",
          CUnmannedTraderItemState::GetMaxStateCnt());
        return false;
      }

      Log(
        "CUnmannedTraderController::InsertStateRecord()\r\n"
        "\t\tg_Main.m_pWorldDB->Truncate_UnmannedTraderItemStateRecord() Fail!\r\n");
      return false;
    }

    Log(
      "CUnmannedTraderController::InsertStateRecord()\r\n"
      "\t\tg_Main.m_pWorldDB->Select_UnmannedTraderItemStateInfo() Fail!\r\n");
    operator delete(stateInfo);
    return false;
  }

  Log(
    "CUnmannedTraderController::InsertStateRecord()\r\n"
    "\t\tg_Main.m_pWorldDB->Select_UnmannedTraderItemStateInfoCnt() Fail!\r\n");
  return false;
}

bool CUnmannedTraderController::UpdateClearDanglingOwnerRecord()
{
  if (g_Main.m_pWorldDB->Update_UnmannedTraderClearDanglingOwnerRecord())
  {
    return true;
  }

  Log(
    "CUnmannedTraderController::UpdateClearDanglingOwnerRecord()\r\n"
    "\t\tg_Main.m_pWorldDB->Update_UnmannedTraderClearDanglingOwnerRecord() Fail!\r\n");
  return false;
}

bool CUnmannedTraderController::InsertDefalutRecord()
{
  int emptyCount = g_Main.m_pWorldDB->Select_UnmannedTraderSingleItemEmptyRecordCnt();
  if (emptyCount >= 0)
  {
    if (CUnmannedTraderEnvironmentValue::Unmanned_Trader_Min_Limit_Empty_Record_Cnt
      <= static_cast<unsigned int>(emptyCount)
      || g_Main.m_pWorldDB->Insert_UnmannedTraderSingleDefaultRecord(
        CUnmannedTraderEnvironmentValue::Unmanned_Trader_Default_Empty_Record_Cnt))
    {
      return true;
    }

    Log(
      "CUnmannedTraderController::InsertDefalutRecord()\r\n"
      "\t\tg_Main.m_pWorldDB->Insert_UnmannedTraderSingleDefaultRecord( %u ) Fail!\r\n",
      CUnmannedTraderEnvironmentValue::Unmanned_Trader_Default_Empty_Record_Cnt);
    return false;
  }

  Log(
    "CUnmannedTraderController::InsertDefalutRecord()\r\n"
    "\t\tiCnt(%d) = g_Main.m_pWorldDB->Select_UnmannedTraderItemEmptyRecordCnt()!\r\n",
    emptyCount);
  return false;
}

void CUnmannedTraderController::Log(const char *fmt, ...)
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

unsigned __int8 CUnmannedTraderController::UpdateItemState(_qry_case_unmandtrader_updateitemstate *pData)
{
  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  if (g_Main.m_pWorldDB->Update_UnmannedTraderItemState(
        pData->byType,
        pData->dwRegistSerial,
        pData->byState,
        &systemTime))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 CUnmannedTraderController::UpdateRegistItem(_qry_case_unmandtrader_registsingleitem *pData)
{
  bool recordInserted = false;
  unsigned __int8 result = GetEmptyRecordSerial(&pData->dwRegedSerial, &recordInserted);
  if (!result)
  {
    _unmannedtrader_registsingleitem info{};
    info.byType = pData->byType;
    info.bySellTurm = pData->bySellTurm;
    info.byRace = pData->byRace;
    info.dwOwnerSerial = pData->dwOwnerSerial;
    info.dwPrice = pData->dwPrice;
    info.byInveninx = pData->byInveninx;
    info.dwK = pData->dwK;
    info.dwD = pData->dwD;
    info.dwU = pData->dwU;
    info.byLv = pData->byLv;
    info.byGrade = pData->byGrade;
    info.byClass1 = pData->byClass1;
    info.byClass2 = pData->byClass2;
    info.byClass3 = pData->byClass3;
    info.dwT = pData->dwT;
    info.lnUID = pData->lnUID;
    info.dwTax = pData->dwTax;

    if (!g_Main.m_pWorldDB->Regist_UnmannedTraderSingleItem(
          pData->dwRegedSerial,
          &info,
          recordInserted))
    {
      return 24;
    }

    pData->bInserted = recordInserted;
  }
  return result;
}


unsigned __int8 CUnmannedTraderController::UpdateCancelRegist(_qry_case_unmandtrader_cancelitem *pData)
{
  pData->byProcRet = 0;
  unsigned __int8 state = 255;
  unsigned __int8 dbRet = CheckDBItemState(
    pData->byType,
    pData->dwRegistSerial,
    &state,
    &pData->byProcRet);
  if (pData->byProcRet)
  {
    return dbRet;
  }

  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  _unmannedtrader_buy_item_info buyInfo{};
  dbRet = g_Main.m_pWorldDB->Select_UnmannedTraderBuySingleItemInfo(
    pData->byType,
    pData->dwRegistSerial,
    &buyInfo);
  if (dbRet == 1)
  {
    pData->byProcRet = 49;
    return 24;
  }
  if (dbRet == 2)
  {
    pData->byProcRet = 50;
    return 0;
  }

  if (g_Main.m_pWorldDB->Update_UnmannedTraderItemState(
        pData->byType,
        pData->dwRegistSerial,
        pData->byState,
        &systemTime))
  {
    return 0;
  }

  pData->byProcRet = 30;
  return 24;
}

unsigned __int8 CUnmannedTraderController::UpdateTimeOutCancelRegist(_qry_case_unmandtrader_time_out_cancelitem *pData)
{
  pData->byProcRet = 0;
  unsigned __int8 state = 255;
  unsigned __int8 dbRet = CheckDBItemState(
    pData->byType,
    pData->dwRegistSerial,
    &state,
    &pData->byProcRet);
  if (pData->byProcRet)
  {
    return dbRet;
  }

  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  _unmannedtrader_buy_item_info buyInfo{};
  dbRet = g_Main.m_pWorldDB->Select_UnmannedTraderBuySingleItemInfo(
    pData->byType,
    pData->dwRegistSerial,
    &buyInfo);
  if (dbRet == 1)
  {
    pData->byProcRet = 47;
    g_Main.m_pWorldDB->Update_UnmannedTraderItemState(
      pData->byType,
      pData->dwRegistSerial,
      9u,
      &systemTime);
    return 24;
  }
  if (dbRet == 2)
  {
    pData->byProcRet = 48;
    return 0;
  }

  if (buyInfo.dwSeller == pData->dwOwnerSerial)
  {
    pData->dwK = buyInfo.dwK;
    pData->dwD = static_cast<unsigned int>(buyInfo.dwD);
    pData->dwU = buyInfo.dwU;
    pData->dwT = buyInfo.dwT;
    if (buyInfo.lnUID)
    {
      pData->lnUID = buyInfo.lnUID;
    }
    else
    {
      pData->lnUID = UIDGenerator::getuid(g_Main.m_byWorldCode);
    }

    if (!pData->szAccountID[0] || !pData->wszName[0])
    {
      g_Main.m_pWorldDB->Select_CharacterName(
        pData->dwOwnerSerial,
        pData->wszName,
        pData->szAccountID);
    }

    if (g_Main.m_pWorldDB->Update_UnmannedTraderItemState(
          pData->byType,
          pData->dwRegistSerial,
          pData->byState,
          &systemTime))
    {
      return 0;
    }

    pData->byProcRet = 46;
    return 24;
  }

  pData->byProcRet = 93;
  return 0;
}

unsigned __int8 CUnmannedTraderController::UpdateRePrice(_qry_case_unmandtrader_update_reprice *pData)
{
  pData->byProcRet = 0;
  unsigned __int8 state = 255;
  unsigned __int8 dbRet = CheckDBItemState(
    pData->byType,
    pData->dwRegistSerial,
    &state,
    &pData->byProcRet);
  if (pData->byProcRet)
  {
    return dbRet;
  }

  if (g_Main.m_pWorldDB->Update_UnmannedTraderSellInfoPrice(
        pData->byType,
        pData->dwRegistSerial,
        pData->dwOwnerSerial,
        pData->dwNewPrice))
  {
    return 0;
  }

  pData->byProcRet = 27;
  return 24;
}


unsigned __int8 CUnmannedTraderController::SelectBuy(_qry_case_unmandtrader_buy_get_info *pData)
{
  unsigned int salesTotals[5]{};
  unsigned long long totalSales = 0;
  unsigned __int8 state = 255;
  _unmannedtrader_seller_info sellerInfo{};

  for (int j = 0; j < pData->byNum; ++j)
  {
    pData->List[j].byProcRet = 0;
    CheckDBItemState(
      pData->byType,
      pData->List[j].dwRegistSerial,
      &state,
      &pData->List[j].byProcRet);
    if (!pData->List[j].byProcRet)
    {
      unsigned __int8 dbRet = g_Main.m_pWorldDB->Select_UnmannedTraderSellInfo(
        pData->byType,
        pData->List[j].dwRegistSerial,
        pData->byRace,
        &sellerInfo);
      if (dbRet == 1)
      {
        pData->List[j].byProcRet = 31;
      }
      else if (dbRet == 2)
      {
        pData->List[j].byProcRet = 32;
      }
      else if (sellerInfo.dwPrice == pData->List[j].dwPrice)
      {
        if (sellerInfo.dwSeller == pData->dwBuyer)
        {
          pData->List[j].byProcRet = 39;
        }
        else
        {
          salesTotals[0] = 0;
          totalSales = 0;
          dbRet = g_Main.m_pWorldDB->Select_utSellWaitItems_SalesTotals(
            pData->byType,
            sellerInfo.dwSeller,
            salesTotals);
          if (dbRet == 1)
          {
            pData->List[j].byProcRet = 31;
          }
          else
          {
            pData->List[j].dwPrice = sellerInfo.dwPrice;
            pData->List[j].dwSeller = sellerInfo.dwSeller;
            pData->List[j].byRaceSexCode = sellerInfo.byRaceSexCode;
            totalSales = salesTotals[0] + sellerInfo.dwDalant;
            if (totalSales > static_cast<unsigned __int64>(MAX_DALANT))
            {
              totalSales = MAX_DALANT;
            }
            pData->List[j].dwDalant = static_cast<unsigned int>(totalSales);
            pData->List[j].dwGuildSerial = sellerInfo.dwGuildSerial;
            pData->List[j].dwAccountSerial = sellerInfo.dwAccountSerial;
            strcpy_s(pData->List[j].szAccountID, 13, sellerInfo.szAccountID);
            strcpy_s(pData->List[j].wszName, 17, sellerInfo.wszName);
            pData->List[j].byUserGrade = 0;
            if (CUnmannedTraderEnvironmentValue::Unmanned_Trader_Dev_Account_Start_Serial <=
                pData->List[j].dwAccountSerial)
            {
              pData->List[j].byUserGrade = 3;
            }
          }
        }
      }
      else
      {
        pData->List[j].byProcRet = 44;
      }
    }
  }

  return 0;
}

unsigned __int8 CUnmannedTraderController::UpdateBuy(_qry_case_unmandtrader_buy_update_wait *pData)
{
  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);
  time_17(&pData->tResultTime);

  _unmannedtrader_buy_item_info buyInfo{};

  for (int j = 0; j < pData->byNum; ++j)
  {
    _qry_case_unmandtrader_buy_update_wait::__list &entry = pData->List[j];
    if (!entry.byProcRet)
    {
      CheckDBItemState(
        pData->byType,
        entry.dwRegistSerial,
        &entry.byOldState,
        &entry.byProcRet);
      if (!entry.byProcRet)
      {
        unsigned __int8 dbRet = g_Main.m_pWorldDB->Select_UnmannedTraderBuySingleItemInfo(
          pData->byType,
          entry.dwRegistSerial,
          &buyInfo);
        if (dbRet == 1)
        {
          entry.byProcRet = 31;
        }
        else if (dbRet == 2)
        {
          entry.byProcRet = 32;
        }
        else if (buyInfo.dwPrice == entry.dwPrice)
        {
          entry.byInveninx = buyInfo.byInveninx;
          entry.dwK = buyInfo.dwK;
          entry.dwD = buyInfo.dwD;
          entry.dwU = buyInfo.dwU;
          entry.dwT = buyInfo.dwT;
          if (buyInfo.lnUID)
          {
            entry.lnUID = buyInfo.lnUID;
          }
          else
          {
            entry.lnUID = UIDGenerator::getuid(g_Main.m_byWorldCode);
          }
          if (!g_Main.m_pWorldDB->Update_UnmannedTraderResutlInfo(
                pData->byType,
                entry.dwRegistSerial,
                10,
                pData->dwBuyer,
                entry.dwTax,
                &systemTime))
          {
            entry.byProcRet = 33;
          }
        }
        else
        {
          entry.byProcRet = 44;
        }
      }
    }
  }

  return 0;
}

unsigned __int8 CUnmannedTraderController::UpdateBuyRollBack(_qry_case_unmandtrader_buy_update_rollback *pData)
{
  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  for (int j = 0; j < pData->byNum; ++j)
  {
    pData->List[j].byProcRet = 0;
    if (!g_Main.m_pWorldDB->Update_UnmannedTraderResutlInfo(
          pData->byType,
          pData->List[j].dwRegistSerial,
          pData->List[j].byOldState,
          0,
          0,
          &systemTime))
    {
      pData->List[j].byProcRet = 36;
    }
  }

  return 0;
}

unsigned __int8 CUnmannedTraderController::UpdateLazyClean(_qry_case_unmandtrader_lazyclean_flags *pData)
{
  return m_kLazyCleaner.UpdateClear(pData);
}


unsigned __int8 CUnmannedTraderController::UpdateBuyComplete(_qry_case_unmandtrader_buy_update_complete *pData)
{
  pData->bAllSuccess = true;
  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  for (int j = 0; j < pData->byNum; ++j)
  {
    _qry_case_unmandtrader_buy_update_complete::__list &entry = pData->List[j];
    entry.byProcRet = 0;
    unsigned __int8 state = entry.byProcUpdate;
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
              pData->byType,
              entry.dwRegistSerial,
              entry.byUpdateState,
              0,
              0,
              &systemTime))
        {
          entry.byProcRet = 1;
          pData->bAllSuccess = false;
        }
        break;
      case 47:
      case 56:
        if (!g_Main.m_pWorldDB->Update_UnmannedTraderItemState(
              pData->byType,
              entry.dwRegistSerial,
              entry.byUpdateState,
              &systemTime))
        {
          entry.byProcRet = 1;
          pData->bAllSuccess = false;
        }
        break;
      default:
        break;
    }
  }

  return 0;
}

unsigned __int8 CUnmannedTraderController::UpdateLogInComplete(
  _qry_case_unmandtrader_log_in_proc_update_complete *pData)
{
  pData->bAllSuccess = true;
  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);
  unsigned __int8 currentState = static_cast<unsigned __int8>(-1);
  unsigned __int8 queryRet = 0;

  for (int j = 0; j < pData->wNum; ++j)
  {
    _qry_case_unmandtrader_log_in_proc_update_complete::__list &entry = pData->List[j];
    entry.byProcRet = 0;
    switch (entry.byProcUpdate - 37)
    {
      case 0:
      case 45:
      case 46:
      case 53:
      case 54:
      case 57:
        if (!g_Main.m_pWorldDB->Update_UnmannedTraderItemState(
              pData->byType,
              entry.dwRegistSerial,
              entry.byUpdateState,
              &systemTime))
        {
          entry.byProcRet = 1;
          pData->bAllSuccess = false;
        }
        break;
      case 55:
        currentState = static_cast<unsigned __int8>(-1);
        queryRet = CheckDBItemState(
          pData->byType,
          entry.dwRegistSerial,
          &currentState,
          &entry.byProcRet);
        (void)queryRet;
        if (entry.byProcRet == 40)
        {
          if (currentState == 10)
          {
            if (g_Main.m_pWorldDB->Update_UnmannedTraderResutlInfo(
                  pData->byType,
                  entry.dwRegistSerial,
                  entry.byUpdateState,
                  0,
                  0,
                  &systemTime))
            {
              entry.byProcRet = 0;
            }
            else
            {
              entry.byProcRet = 1;
              pData->bAllSuccess = false;
            }
          }
          else if (currentState == 4)
          {
            _unmannedtrader_result_buyerinfo resultBuyerInfo{};
            queryRet = g_Main.m_pWorldDB->Select_UnmannedTraderResultBuyerInfo(
              pData->byType,
              entry.dwRegistSerial,
              &resultBuyerInfo);
            if (queryRet)
            {
              entry.byProcRet = static_cast<unsigned __int8>(-49);
              pData->bAllSuccess = false;
            }
            else
            {
              entry.byProcRet = static_cast<unsigned __int8>(-48);
              pData->bAllSuccess = false;
              entry.dwPrice = resultBuyerInfo.dwPrice;
              entry.dwTax = resultBuyerInfo.dwTax;
              entry.dwBuyer = resultBuyerInfo.dwBuyer;
              entry.dwAccountSerial = resultBuyerInfo.dwAccountSerial;
              strcpy_s(entry.szAccountID, resultBuyerInfo.szAccountID);
              strcpy_s(entry.wszName, resultBuyerInfo.wszName);
              entry.dbresulttime = resultBuyerInfo.dbresulttime;
            }
          }
          else
          {
            entry.byProcRet = static_cast<unsigned __int8>(-50);
          }
        }
        else
        {
          entry.byProcRet = static_cast<unsigned __int8>(-50);
        }
        break;
      case 59:
        entry.byProcUpdate = 92;
        pData->bAllSuccess = false;
        entry.byProcRet = 96;
        break;
      default:
        break;
    }
  }

  return 0;
}


unsigned __int8 CUnmannedTraderController::UpdateReRegist(_qry_case_unmandtrader_re_registsingleitem *pData)
{
  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  unsigned __int8 state = 0;
  unsigned __int8 procRet = 0;
  unsigned int registerOwner[4]{};

  for (int j = 0; j < pData->byNum; ++j)
  {
    _qry_case_unmandtrader_re_registsingleitem::__list &entry = pData->List[j];
    state = 255;
    procRet = 0;
    CheckDBItemState(
      pData->byType,
      entry.dwRegistSerial,
      &state,
      &procRet);
    if (procRet == 8 || procRet == 38)
    {
      entry.byProcRet = procRet;
    }
    else if (state == 6 || state == 13)
    {
      if (entry.bRegist)
      {
        if (entry.byProcRet)
        {
          if (!g_Main.m_pWorldDB->Update_UnmannedTraderItemState(
                pData->byType,
                entry.dwRegistSerial,
                5u,
                &systemTime))
          {
            entry.byProcRet = 30;
          }
        }
        else
        {
          state = g_Main.m_pWorldDB->Select_UnmannedTraderRegister(
            pData->byType,
            entry.dwRegistSerial,
            registerOwner);
          if (state)
          {
            entry.byProcRet = 24;
          }
          else if (registerOwner[0] == pData->dwOwnerSerial)
          {
            if (!g_Main.m_pWorldDB->Update_UnmannedTraderReRegist(
                  pData->byType,
                  entry.dwRegistSerial,
                  entry.byUpdateState,
                  entry.dwPrice,
                  entry.dwTax,
                  &systemTime))
            {
              entry.byProcRet = 24;
            }
          }
          else
          {
            entry.byProcRet = static_cast<unsigned __int8>(-54);
          }
        }
      }
      else if (!g_Main.m_pWorldDB->Update_UnmannedTraderItemState(
                 pData->byType,
                 entry.dwRegistSerial,
                 entry.byUpdateState,
                 &systemTime))
      {
        entry.byProcRet = 30;
      }
    }
    else
    {
      entry.byProcRet = static_cast<unsigned __int8>(-53);
    }
  }

  return 0;
}

unsigned __int8 CUnmannedTraderController::UpdateCheatRegistTime(
  _qry_case_unmandtrader_cheat_updateregisttime *pData)
{
  for (unsigned __int8 j = 0; j < pData->byNum; ++j)
  {
    _qry_case_unmandtrader_cheat_updateregisttime::__list &entry = pData->List[j];
    CheckDBItemState(
      pData->byType,
      entry.dwRegistSerial,
      &entry.byState,
      &entry.byProcRet);
    if (!entry.byProcRet && !g_Main.m_pWorldDB->Update_UnmannedTraderCheatUpdateRegistDate(
                              pData->byType,
                              entry.dwRegistSerial))
    {
      entry.byProcRet = 30;
    }
  }

  return 0;
}

unsigned __int8 CUnmannedTraderController::UpdateReRegistRollBack(
  _qry_case_unmandtrader_re_registsingleitem_roll_back *pData)
{
  unsigned __int8 state = 0;
  unsigned __int8 procRet = 0;
  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  pData->bAllSuccess = true;
  for (unsigned __int8 j = 0; j < pData->byNum; ++j)
  {
    _qry_case_unmandtrader_re_registsingleitem_roll_back::__list &entry = pData->List[j];
    state = 0;
    procRet = 0;
    CheckDBItemState(
      pData->byType,
      entry.dwRegistSerial,
      &state,
      &procRet);
    if (procRet)
    {
      entry.byProcRet = procRet;
      pData->bAllSuccess = false;
    }
    else if (!g_Main.m_pWorldDB->Update_UnmannedTraderItemState(
               pData->byType,
               entry.dwRegistSerial,
               entry.byState,
               &systemTime))
    {
      entry.byProcRet = 30;
      pData->bAllSuccess = false;
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

void CUnmannedTraderController::CompleteUpdateState(
  unsigned __int8 byRet,
  _qry_case_unmandtrader_updateitemstate *pLoadData)
{
  unsigned __int8 state = pLoadData->byState;
  if ((state == 3 || state == 5 || state == 7) && !byRet)
  {
    CUnmannedTraderUserInfoTable *table = CUnmannedTraderUserInfoTable::Instance();
    if (table->CompleteUpdateState(
          pLoadData->dwOwnerSerial,
          pLoadData->dwRegistSerial,
          pLoadData->byState))
    {
      CUnmannedTraderGroupItemInfoTable *groupTable = CUnmannedTraderGroupItemInfoTable::Instance();
      groupTable->IncreaseVersion(pLoadData->byItemTableCode, pLoadData->wItemTableIndex);
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
      pLoadData->byType,
      pLoadData->dwRegistSerial,
      pLoadData->byState,
      pLoadData->dwOwnerSerial,
      pLoadData->byItemTableCode);
  }
}

void CUnmannedTraderController::CompleteRegistItem(
  unsigned __int8 byRet,
  _qry_case_unmandtrader_registsingleitem *pLoadData)
{
  CUnmannedTraderUserInfoTable *table = CUnmannedTraderUserInfoTable::Instance();
  table->CompleteRegist(byRet, pLoadData);
}

void CUnmannedTraderController::CompleteCancelRegist(
  unsigned __int8 byRet,
  _qry_case_unmandtrader_cancelitem *pLoadData)
{
  CUnmannedTraderUserInfoTable *table = CUnmannedTraderUserInfoTable::Instance();
  table->CompleteCancelRegist(byRet, pLoadData);
}

void CUnmannedTraderController::CompleteTimeOutCancelRegist(
  unsigned __int8 byRet,
  _qry_case_unmandtrader_time_out_cancelitem *pLoadData)
{
  if (byRet || pLoadData->byProcRet)
  {
    const char *senderName = pLoadData->szAccountID[0] ? pLoadData->szAccountID : "NULL";
    const char *charName = pLoadData->wszName[0] ? pLoadData->wszName : "NULL";
    Log(
      "CUnmannedTraderController::CompleteTimeOutCancelRegist( byRet, char * pLoadData )\r\n"
      "\t\tType(%u) RegistSerial(%u) State(%u) : %s(%u) ID(%s) TableCode(%u) TableIndex(%u)\r\n"
      "\t\tProcRet(%u) RET_CODE_SUCCESS != byRet(%u)\r\n",
      pLoadData->byType,
      pLoadData->dwRegistSerial,
      pLoadData->byState,
      charName,
      pLoadData->dwOwnerSerial,
      senderName,
      pLoadData->byItemTableCode,
      pLoadData->wItemTableIndex,
      pLoadData->byProcRet,
      byRet);
  }
  else
  {
    CUnmannedTraderUserInfoTable *table = CUnmannedTraderUserInfoTable::Instance();
    table->CompleteTimeOutClear(pLoadData);
    CUnmannedTraderGroupItemInfoTable *groupTable = CUnmannedTraderGroupItemInfoTable::Instance();
    groupTable->IncreaseVersion(pLoadData->byItemTableCode, pLoadData->wItemTableIndex);
  }

  CUnmannedTraderScheduler *scheduler = CUnmannedTraderScheduler::Instance();
  scheduler->CompleteClear(byRet, pLoadData->byProcRet, pLoadData->byType, pLoadData->dwRegistSerial);
}

void CUnmannedTraderController::CompleteReprice(
  unsigned __int8 byRet,
  _qry_case_unmandtrader_update_reprice *pLoadData)
{
  CUnmannedTraderUserInfoTable *table = CUnmannedTraderUserInfoTable::Instance();
  table->CompleteReprice(byRet, pLoadData);
}


void CUnmannedTraderController::CompleteSelectBuyInfo(
  unsigned __int8 byRet,
  _qry_case_unmandtrader_buy_get_info *pLoadData)
{
  (void)byRet;

  _qry_case_unmandtrader_buy_update_wait qryData{};
  qryData.wInx = pLoadData->wInx;
  qryData.dwBuyer = pLoadData->dwBuyer;
  qryData.byRace = pLoadData->byRace;
  qryData.byDivision = pLoadData->byDivision;
  qryData.byClass = pLoadData->byClass;
  qryData.bySubClass = pLoadData->bySubClass;
  qryData.byType = pLoadData->byType;
  qryData.byNum = pLoadData->byNum;

  unsigned int addMoney = 0;
  unsigned char addMoneyCount = 0;

  for (int j = 0; j < pLoadData->byNum; ++j)
  {
    const _qry_case_unmandtrader_buy_get_info::__list &infoEntry = pLoadData->List[j];
    _qry_case_unmandtrader_buy_update_wait::__list &updateEntry = qryData.List[j];
    updateEntry.dwRegistSerial = infoEntry.dwRegistSerial;
    updateEntry.dwSeller = infoEntry.dwSeller;
    strcpy_s(updateEntry.szAccountID, 13, infoEntry.szAccountID);
    updateEntry.dwAccountSerial = infoEntry.dwAccountSerial;
    strcpy_s(updateEntry.wszName, 17, infoEntry.wszName);
    updateEntry.byProcRet = infoEntry.byProcRet;
    updateEntry.dwPrice = infoEntry.dwPrice;

    if (!updateEntry.byProcRet)
    {
      CPlayer *seller = GetPtrPlayerFromSerial(g_Player, 2532, infoEntry.dwSeller);
      if (seller && !seller->m_bLive)
      {
        pLoadData->List[j].byRaceSexCode = static_cast<unsigned __int8>(seller->m_Param.GetRaceSexCode());
        pLoadData->List[j].dwDalant = seller->m_Param.GetDalant();
        pLoadData->List[j].dwGuildSerial = seller->m_Param.GetGuildSerial();
        pLoadData->List[j].byUserGrade = seller->m_byUserDgr;
      }

      CUnmannedTraderTaxRateManager *taxMgr = CUnmannedTraderTaxRateManager::Instance();
      updateEntry.dwTax = taxMgr->GetTax(pLoadData->byType, pLoadData->List[j].dwGuildSerial, pLoadData->List[j].dwPrice);

      if (pLoadData->List[j].dwPrice <= updateEntry.dwTax)
      {
        Log(
          "CUnmannedTraderController::CompleteSelectBuyInfo(...) Exceed Tax Price!\r\n"
          "\t\t dwRegistSerial(%u) dwSeller(%u)\r\n"
          "\t\t dwBuyer(%u) dwGuildSerial(%u) dwCurDalant(%u)\r\n"
          "\t\t dwPrice(%u) dwTax(%u)\r\n",
          infoEntry.dwRegistSerial,
          infoEntry.dwSeller,
          pLoadData->dwBuyer,
          pLoadData->List[j].dwGuildSerial,
          pLoadData->List[j].dwDalant,
          pLoadData->List[j].dwPrice,
          updateEntry.dwTax);
        updateEntry.dwTax = static_cast<unsigned int>(static_cast<float>(pLoadData->List[j].dwPrice) * 0.050000001f);
      }

      addMoney += pLoadData->List[j].dwPrice - updateEntry.dwTax;
      if (CanAddMoneyForMaxLimMoney(addMoney, pLoadData->List[j].dwDalant))
      {
        if (pLoadData->byUserGrade == pLoadData->List[j].byUserGrade)
        {
          ++addMoneyCount;
        }
        else
        {
          updateEntry.byProcRet = 35;
        }
      }
      else
      {
        updateEntry.byProcRet = 34;
      }
    }
  }

  if (addMoneyCount)
  {
    g_Main.PushDQSData(-1, nullptr, 65, reinterpret_cast<char *>(&qryData), sizeof(qryData));
  }
  else
  {
    CPlayer *buyer = GetPtrPlayerFromSerial(g_Player, 2532, pLoadData->dwBuyer);
    if (buyer && buyer->m_bOper)
    {
      _unmannedtrader_buy_item_result_zocl result{};
      result.byRetCode = static_cast<unsigned __int8>(-1);
      result.byNum = static_cast<unsigned __int8>(pLoadData->byNum);
      for (int k = 0; k < pLoadData->byNum; ++k)
      {
        result.List[k].byRet = qryData.List[k].byProcRet;
        result.List[k].dwPrice = qryData.List[k].dwPrice;
      }
      result.dwLeftDalant = buyer->m_Param.GetDalant();

      unsigned __int8 type[28]{};
      type[0] = 30;
      type[1] = 31;
      unsigned __int16 size = static_cast<unsigned __int16>(result.size());
      g_Network.m_pProcess[0]->LoadSendMsg(buyer->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&result), size);
    }

    CUnmannedTraderUserInfoTable *table = CUnmannedTraderUserInfoTable::Instance();
    table->ClearRequest(qryData.wInx, pLoadData->dwBuyer);
  }
}

void CUnmannedTraderController::CompleteBuy(unsigned __int8 byRet, _qry_case_unmandtrader_buy_update_wait *pLoadData)
{
  CUnmannedTraderUserInfoTable *table = CUnmannedTraderUserInfoTable::Instance();
  table->CompleteBuy(byRet, pLoadData, &m_kTradeInfo);
}

void CUnmannedTraderController::CompleteBuyRollBack(
  unsigned __int8 byRet,
  _qry_case_unmandtrader_buy_update_rollback *pLoadData)
{
  (void)byRet;

  for (int j = 0; j < pLoadData->byNum; ++j)
  {
    const char *result = pLoadData->List[j].byProcRet ? "Fail" : "Success";
    Log(
      "CUnmannedTraderController::CompleteBuyRollBack( BYTE byRet, char * pLoadData )\r\n"
      "\t\tType(%u) Regist Serial(%u), Old State(%u) wInx(%u) Race(%u) BuyerSerial(%u) RollBack %s\r\n",
      pLoadData->byType,
      pLoadData->List[j].dwRegistSerial,
      pLoadData->List[j].byOldState,
      pLoadData->wInx,
      pLoadData->byRace,
      pLoadData->dwBuyer,
      result);
  }
}

void CUnmannedTraderController::ComleteLazyClean(_qry_case_unmandtrader_lazyclean_flags *pData)
{
  m_kLazyCleaner.CompleteUpdateClear(pData);
}

void CUnmannedTraderController::CompleteBuyComplete(_qry_case_unmandtrader_buy_update_complete *pData)
{
  if (!pData->bAllSuccess)
  {
    Log(
      "CUnmannedTraderController::CompleteBuyRollBack( BYTE byRet, char * pLoadData )\r\n"
      "\t\tType(%u) wInx(%u) Race(%u) BuyerSerial(%u)\r\n",
      pData->byType,
      pData->wInx,
      pData->byRace,
      pData->dwBuyer);
    for (int j = 0; j < pData->byNum; ++j)
    {
      const _qry_case_unmandtrader_buy_update_complete::__list &entry = pData->List[j];
      if (entry.byProcUpdate != 255 && entry.byProcRet)
      {
        Log(
          "\t\t(%d)Nth Regist Serial(%u) dwSeller(%u) UpdateState(%u) byProcUpdate(%u) DB Error!\r\n",
          j,
          entry.dwRegistSerial,
          entry.dwSeller,
          entry.byUpdateState,
          entry.byProcUpdate);
      }
    }
  }
}

void CUnmannedTraderController::CompleteLogInCompete(_qry_case_unmandtrader_log_in_proc_update_complete *pData)
{
  Log(
    "CUnmannedTraderController::CompleteLogInCompete( BYTE byRet, char * pLoadData )\r\n"
    "\t\tType(%u) wInx(%u) dwSeller(%u)\r\n",
    pData->byType,
    pData->wInx,
    pData->dwSeller);

  for (int j = 0; j < pData->wNum; ++j)
  {
    const _qry_case_unmandtrader_log_in_proc_update_complete::__list &entry = pData->List[j];
    if (entry.byProcUpdate == 255)
    {
      continue;
    }

    if (entry.byProcUpdate == 92)
    {
      CUnmannedTraderUserInfoTable *table = CUnmannedTraderUserInfoTable::Instance();
      table->CompleteUpdateRegistSellUpdateWaitItem(pData, j);
      continue;
    }

    if (entry.byProcRet)
    {
      Log(
        "\t\t(%d)Nth Regist Serial(%u) dwBuyer(%u) UpdateState(%u) byProcUpdate(%u) DB Error!\r\n",
        j,
        entry.dwRegistSerial,
        entry.dwBuyer,
        entry.byUpdateState,
        entry.byProcUpdate);
    }
  }
}

void CUnmannedTraderController::CompleteReRegist(_qry_case_unmandtrader_re_registsingleitem *pLoadData)
{
  CUnmannedTraderUserInfoTable *table = CUnmannedTraderUserInfoTable::Instance();
  table->CompleteReRegist(pLoadData);
}

void CUnmannedTraderController::CompleteUpdateCheatRegistTime(
  _qry_case_unmandtrader_cheat_updateregisttime *pLoadData)
{
  if (pLoadData->wInx < 0x9E4u && pLoadData->byNum)
  {
    CUnmannedTraderUserInfoTable *table = CUnmannedTraderUserInfoTable::Instance();
    table->CompleteUpdateCheatRegistTime(pLoadData);
    CUnmannedTraderScheduler *scheduler = CUnmannedTraderScheduler::Instance();
    scheduler->CheatPushLoad();

    CPlayer *player = GetPtrPlayerFromSerial(g_Player, 2532, pLoadData->dwOwnerSerial);
    if (player && player->m_bOper)
    {
      char buffer[260]{};
      sprintf_s(buffer, "Cnt : %u", pLoadData->byNum);
      player->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, buffer, 0xFFu, nullptr);
      sprintf_s(buffer, "th  serial  state  ret");
      player->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, buffer, 0xFFu, nullptr);
      for (unsigned __int8 j = 0; j < pLoadData->byNum; ++j)
      {
        sprintf_s(
          buffer,
          " %u    %u       %u     %u",
          j,
          pLoadData->List[j].dwRegistSerial,
          pLoadData->List[j].byState,
          pLoadData->List[j].byProcRet);
        player->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, false, buffer, 0xFFu, nullptr);
      }
    }
  }
}

void CUnmannedTraderController::CompleteReRegistRollBack(
  _qry_case_unmandtrader_re_registsingleitem_roll_back *pData)
{
  CUnmannedTraderUserInfoTable *table = CUnmannedTraderUserInfoTable::Instance();
  table->CompleteReRegistRollBack(pData);
}
