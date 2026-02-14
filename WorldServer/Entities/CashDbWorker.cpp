#include "pch.h"

#include "CashDbWorker.h"

#include <cstdio>
#include <ctime>
#include <windows.h>

#include "CAsyncLogger.h"
#include "CCashDBWorkManager.h"
#include "CCashDbWorkerNULL.h"
#include "CNetProcess.h"
#include "CPlayer.h"
#include "CPlayerDB.h"
#include "CRecordData.h"
#include "CashItemRemoteStore.h"
#include "CNationSettingManager.h"
#include "CRFCashItemDatabase.h"
#include "GlobalObjects.h"
#include "ICsSendInterface.h"
#include "Task.h"
#include "TaskPool.h"
#include "TimeItem.h"
#include "UIDGenerator.h"
#include "WorldServerUtil.h"
#include "result_csi_buy_zocl.h"

CashDbWorker::CashDbWorker()
  : Worker("CashDbWorker", 2532, 2500),
    _pkDb(nullptr)
{
}

CashDbWorker::~CashDbWorker() = default;

void CashDbWorker::Release()
{
  if (_pkDb)
  {
    delete _pkDb;
    _pkDb = nullptr;
  }
}

bool CashDbWorker::Initialize()
{
  if (!_init_loggers())
  {
    return false;
  }
  if (!_init_database())
  {
    return false;
  }
  if (Worker::Initialize(1, 1))
  {
    return true;
  }
  MyMessageBox("CashDbWorker::Initialize()", "Worker::Initialize(1, 1) Fail!");
  return false;
}

void CashDbWorker::DoWork()
{
  unsigned int nIdx = 0;
  Task *pkTsk = _pkPool->PopWaitTsk(&nIdx);
  if (!pkTsk)
  {
    return;
  }

  int nCode = -1;
  const int taskCode = pkTsk->GetTaskCode();
  if (taskCode)
  {
    switch (taskCode)
    {
      case 1:
        nCode = _wait_tsk_cash_update(pkTsk);
        break;
      case 2:
        nCode = _wait_tsk_cash_rollback(pkTsk);
        break;
      case 3:
        nCode = _wait_tsk_cash_buy_dblog(pkTsk);
        break;
      case 4:
        nCode = _wait_tst_cash_total_selling_select(pkTsk);
        break;
      default:
        break;
    }
  }
  else
  {
    nCode = _wait_tsk_cash_select(pkTsk);
  }
  pkTsk->SetRetCode(nCode);
  _pkPool->PushCompleteTsk(nIdx);
}

void CashDbWorker::CompleteWork()
{
  auto *nationSetting = CTSingleton<CNationSettingManager>::Instance();
  if (!nationSetting->IsCashDBInit())
  {
    return;
  }

  unsigned int nIdx = 0;
  Task *pkTsk = _pkPool->PopCompleteTsk(&nIdx);
  if (!pkTsk)
  {
    return;
  }

  const int taskCode = pkTsk->GetTaskCode();
  if (taskCode)
  {
    switch (taskCode)
    {
      case 1:
        _complete_tsk_cash_update(pkTsk);
        break;
      case 2:
        _complete_tsk_cash_rollback(pkTsk);
        break;
      case 3:
        _complete_tsk_cashitem_buy_dblog(pkTsk);
        break;
      case 4:
        _complete_tsk_cash_total_selling_select(pkTsk);
        break;
      default:
        break;
    }
  }
  else
  {
    _complete_tsk_cash_select(pkTsk);
  }
  _pkPool->PushEmptyTsk(nIdx);
}

void CashDbWorker::GetUseCashQueryStr(
  _param_cash_update *rParam,
  int nIdx,
  char *wszQuery,
  unsigned __int64 tBufferSize)
{
  sprintf_s(
    wszQuery,
    tBufferSize,
    "declare @out_status char(1) declare @out_amount_s int  exec prc_rfonline_use @s_userid = '%s', @s_gcode = 'RFO',@s_"
    "pscode='%s', @s_price=%d,@s_quantity=%d,@s_discount=%d,@s_scode=%I64d,@s_server='%s',@s_character='%s', @s_Event=%d ,"
    " @s_status = @out_status output, @s_amount = @out_amount_s output select @out_status, @out_amount_s",
    rParam->in_szAcc,
    rParam->in_item[static_cast<unsigned __int64>(nIdx)].in_strItemCode,
    rParam->in_item[static_cast<unsigned __int64>(nIdx)].in_nPrice,
    rParam->in_item[static_cast<unsigned __int64>(nIdx)].in_byOverlapNum,
    rParam->in_item[static_cast<unsigned __int64>(nIdx)].in_nDiscount,
    rParam->in_item[static_cast<unsigned __int64>(nIdx)].in_lnUID,
    rParam->in_szSvrName,
    rParam->in_szAvatorName,
    rParam->in_item[static_cast<unsigned __int64>(nIdx)].in_nEventType);
}

int CashDbWorker::_wait_tsk_cash_select(Task *pkTsk)
{
  auto *rParam = reinterpret_cast<_param_cash_select *>(pkTsk->GetTaskBuf());
  return _pkDb->CallProc_RFOnlineAuth(rParam) != 0;
}

int CashDbWorker::_wait_tsk_cash_update(Task *pkTsk)
{
  auto *rParam = reinterpret_cast<_param_cash_update *>(pkTsk->GetTaskBuf());
  for (int nIdx = 0; nIdx < rParam->in_nNum10; ++nIdx)
  {
    auto *item = &rParam->in_item[nIdx];
    if (_pkDb->CallProc_RFOnlineUse(rParam, nIdx))
    {
      return 1;
    }
    item->out_cState = static_cast<char>(ConvertErrorCode(item->out_cState));
  }
  return 0;
}

int CashDbWorker::_wait_tsk_cash_rollback(Task *pkTsk)
{
  auto *sheet = reinterpret_cast<_param_cash_rollback *>(pkTsk->GetTaskBuf());
  for (int j = 0; j < sheet->in_byNum; ++j)
  {
    auto *entry = &sheet->data[j];
    if (_pkDb->CallProc_RFONLINE_Cancel(entry))
    {
      entry->out_nCashAmount = 0;
      sheet->out_nCashAmount = 0;
      entry->out_cStatus = 1;
    }
    else
    {
      entry->out_cStatus = static_cast<char>(ConvertErrorCode(entry->out_cStatus));
      if (entry->out_cStatus)
      {
        entry->out_nCashAmount = 0;
        sheet->out_nCashAmount = 0;
      }
      else
      {
        sheet->out_nCashAmount = entry->out_nCashAmount;
      }
    }
  }
  return 0;
}

void CashDbWorker::_complete_tsk_cash_select(Task *pkTsk)
{
  auto *psheet = reinterpret_cast<_param_cash_select *>(pkTsk->GetTaskBuf());
  if (pkTsk->GetRetCode() == 1)
  {
    _kLogger[0].Write(
      "[ERR_QRY] cash select >> AC:%u(%s) AV:%u",
      psheet->in_dwAccountSerial,
      psheet->in_szAcc,
      psheet->in_dwAvatorSerial);
    return;
  }

  CPlayer *player = _get_player(psheet->in_wSockIndex, psheet->in_dwAvatorSerial);
  if (player)
  {
    ICsSendInterface::SendMsg_GoodsList(psheet->in_wSockIndex, psheet);
    player->SetCashAmount(psheet->out_dwCashAmount);
    CPlayer::s_MgrItemHistory.read_cashamount(
      psheet->in_dwAccountSerial,
      psheet->in_dwAvatorSerial,
      player->GetCashAmount(),
      player->m_szItemHistoryFileName);
  }
}

void CashDbWorker::_complete_tsk_cash_update(Task *pkTsk)
{
  auto *psheet = reinterpret_cast<_param_cash_update *>(pkTsk->GetTaskBuf());
  CPlayer *pOne = _get_player(psheet->in_wSockIndex, psheet->in_dwAvatorSerial);
  if (pOne && pOne->m_bOper && pOne->m_pUserDB)
  {
    if (pkTsk->GetRetCode())
    {
      ICsSendInterface::SendMsg_Error(psheet->in_wSockIndex, pkTsk->GetRetCode());
      return;
    }

    int successCount = 0;
    const unsigned int charSerial = pOne->m_Param.GetCharSerial();
    _param_cash_update sheetplus(pOne->m_pUserDB->m_dwAccountSerial, charSerial, pOne->m_ObjID.m_wIndex);
    strcpy_s(sheetplus.in_szAcc, sizeof(sheetplus.in_szAcc), pOne->m_pUserDB->m_szAccountID);
    strcpy_s(sheetplus.in_szSvrName, sizeof(sheetplus.in_szSvrName), g_Main.m_szWorldName);
    const char *charNameW = pOne->m_Param.GetCharNameW();
    strcpy_s(sheetplus.in_szAvatorName, sizeof(sheetplus.in_szAvatorName), charNameW);

    unsigned __int8 extraCount = 0;
    for (int nNum = 0; nNum < psheet->in_nNum10; ++nNum)
    {
      auto *pGII = &psheet->in_item[nNum];
      if (pGII->out_cState)
      {
        ICsSendInterface::SendMsg_Error(psheet->in_wSockIndex, pGII->out_cState);
        const int cashAmount = pOne->GetCashAmount();
        const char *charNameA = pOne->m_Param.GetCharNameA();
        _kLogger[0].Write(
          "Failed use cash(%d) : id:%s name:%s pscode:%s (remain:%d)",
          pGII->out_cState,
          psheet->in_szAcc,
          charNameA,
          pGII->in_strItemCode,
          cashAmount);
      }
      else
      {
        if (!_insert_to_inven(pOne, pGII))
        {
          ICsSendInterface::SendMsg_Error(psheet->in_wSockIndex, 15);
          _delete_from_inven(pOne, psheet, nNum);
          _all_rollback(psheet);
          return;
        }
        if (pGII->in_nEventType == 5 && psheet->in_bLimited_Sale)
        {
          const unsigned int itemIdx = pGII->in_wItemIdx;
          CashItemRemoteStore *store = CashItemRemoteStore::Instance();
          store->Set_LimitedSale_count(pGII->in_byTblCode, itemIdx);
          store = CashItemRemoteStore::Instance();
          store->Set_DB_LimitedSale_Event();
          store = CashItemRemoteStore::Instance();
          store->LimitedSale_check_count(pGII->in_byTblCode, itemIdx);
        }

        const char *itemKorName = GetItemKorName(pGII->in_byTblCode, pGII->in_wItemIdx);
        CAsyncLogger *logger = CAsyncLogger::Instance();
        const char *charNameA = pOne->m_Param.GetCharNameA();
        const unsigned int charSerialA = pOne->m_Param.GetCharSerial();
        logger->FormatLog(
          10,
          "[Name: %s(%d) AccountID: %s(%d)] : %s(%s) [UID: %I64u] [Price:%d Num:%d Event: %d]",
          charNameA,
          charSerialA,
          pOne->m_pUserDB->m_szAccountID,
          pOne->m_pUserDB->m_dwAccountSerial,
          pGII->in_strItemCode,
          itemKorName,
          pGII->in_lnUID,
          pGII->in_nPrice,
          pGII->in_byOverlapNum,
          pGII->in_nEventType);

        if (pGII->in_nEventType == 3 && psheet->in_bOneN_One)
        {
          auto *dupItem = &psheet->in_item[nNum];
          dupItem->in_lnUID = UIDGenerator::getuid(g_Main.m_byWorldCode);
          dupItem->in_nPrice = 0;
          memcpy_0(&sheetplus.in_item[static_cast<unsigned __int64>(extraCount)], dupItem, sizeof(sheetplus.in_item[0]));
          sheetplus.in_item[static_cast<unsigned __int64>(extraCount)].in_lnUID = dupItem->in_lnUID;
          sheetplus.in_item[static_cast<unsigned __int64>(extraCount)].in_nPrice = dupItem->in_nPrice;
          sheetplus.in_item[static_cast<unsigned __int64>(extraCount)].in_nLendType = dupItem->in_nEventType;
          sheetplus.in_item[static_cast<unsigned __int64>(extraCount)].in_dwLendTime = dupItem->in_dwLendTime;
          ++extraCount;
          if (!_insert_to_inven(pOne, dupItem))
          {
            ICsSendInterface::SendMsg_Error(psheet->in_wSockIndex, 15);
            _delete_from_inven(pOne, psheet, nNum);
            _all_rollback(psheet);
            return;
          }

          const char *dupKorName = GetItemKorName(dupItem->in_byTblCode, dupItem->in_wItemIdx);
          const char *dupCharNameA = pOne->m_Param.GetCharNameA();
          const unsigned int dupCharSerial = pOne->m_Param.GetCharSerial();
          logger = CAsyncLogger::Instance();
          logger->FormatLog(
            10,
            "[Name: %s(%d) AccountID: %s(%d)] [Insert 1+1 Event CASTITEM] : %s(%s) [UID: %I64u] [Price:%d Num:%d Event: %d]",
            dupCharNameA,
            dupCharSerial,
            pOne->m_pUserDB->m_szAccountID,
            pOne->m_pUserDB->m_dwAccountSerial,
            dupItem->in_strItemCode,
            dupKorName,
            dupItem->in_lnUID,
            dupItem->in_nPrice,
            dupItem->in_byOverlapNum,
            dupItem->in_nEventType);
        }
        ++successCount;
      }
    }

    sheetplus.in_nNum10 = extraCount;
    if (psheet->out_nCashAmount < 0)
    {
      psheet->out_nCashAmount = 0;
      const int cashAmount = pOne->GetCashAmount();
      const char *charNameA = pOne->m_Param.GetCharNameA();
      _kLogger[0].Write(
        "Wrong CashAmount : id:%s name:%s (Local:%d)",
        psheet->in_szAcc,
        charNameA,
        cashAmount);
    }

    if (successCount > 0)
    {
      ICsSendInterface::SendMsg_BuyCashItem(pOne->m_ObjID.m_wIndex, psheet, &sheetplus);
      pOne->SetCashAmount(psheet->out_nCashAmount);
      pOne->DeleteCouponItem(psheet->in_CouponItem, psheet->in_nCouponCnt);
    }
  }
  else
  {
    _kLogger[0].Write(
      "CashDbWorker::_complete_tsk_cash_update : Invalid Character!\r\n"
      "( !pOne || !pOne->m_bOper || NULL == pOne->m_pUserDB ) Invalid!");
    if (!pkTsk->GetRetCode())
    {
      _all_rollback(psheet);
      _kLogger[0].Write(
        "CashDbWorker::_complete_tsk_cash_update : Account:%u(%s) Character:%u(%s) -> _all_rollback!",
        psheet->in_dwAccountSerial,
        psheet->in_szAcc,
        psheet->in_dwAvatorSerial,
        psheet->in_szAvatorName);
    }
  }
}

void CashDbWorker::_complete_tsk_cash_rollback(Task *pkTsk)
{
  auto *sheet = reinterpret_cast<_param_cash_rollback *>(pkTsk->GetTaskBuf());
  CPlayer *player = _get_player(sheet->in_wSockIndex, sheet->in_dwAvatorSerial);
  for (int j = 0; j < sheet->in_byNum; ++j)
  {
    const char *szRet = sheet->data[j].out_cStatus ? "FAILED" : "SUCCEED";
    if (player)
    {
      player->SetCashAmount(sheet->out_nCashAmount);
      CPlayer::s_MgrItemHistory.rollback_cashitem(
        const_cast<char *>(szRet),
        sheet->data[j].in_lnUID,
        sheet->data[j].in_strItemCode,
        sheet->data[j].out_nCashAmount,
        player->m_szItemHistoryFileName);
    }
    else
    {
      _kLogger[1].Write(
        "ROLLBACK_%s AC:%u AV:%u UID:%I64u ICODE:%s Cash:%d",
        szRet,
        sheet->in_dwAccountSerial,
        sheet->in_dwAvatorSerial,
        sheet->data[j].in_lnUID,
        sheet->data[j].in_strItemCode,
        sheet->data[j].out_nCashAmount);
    }
  }
}

void CashDbWorker::_complete_tsk_cashitem_buy_dblog(Task *pkTsk)
{
  auto *sheet = reinterpret_cast<_param_cashitem_dblog *>(pkTsk->GetTaskBuf());
  for (int j = 0; j < sheet->nBuyNum; ++j)
  {
    const auto &entry = sheet->data[j];
    if (entry.byRet)
    {
      _kLogger[0].Write(
        "Failed insert to DB >> tblcode:%d itemidx:%d num:%d cost:%d discount:%d",
        entry.byTblCode,
        entry.wItemIndex,
        entry.byOverlapNum,
        entry.dwCost,
        entry.iCashDiscount);
    }
  }
}

void CashDbWorker::_complete_tsk_cash_total_selling_select(Task *pkTsk)
{
  auto *taskBuf = reinterpret_cast<unsigned int *>(pkTsk->GetTaskBuf());
  if (pkTsk->GetRetCode() == 1)
  {
    _kLogger[0].Write("[ERR_QRY] cash total selling select ");
    return;
  }

  CashItemRemoteStore *store = CashItemRemoteStore::Instance();
  store->Check_Grosssales(*taskBuf);
}

int CashDbWorker::_wait_tsk_cash_buy_dblog(Task *pkTsk)
{
  auto *sheet = reinterpret_cast<_param_cashitem_dblog *>(pkTsk->GetTaskBuf());
  for (int j = 0; j < sheet->nBuyNum; ++j)
  {
    auto &entry = sheet->data[j];
    _base_fld *record = g_Main.m_tblItemData[entry.byTblCode].GetRecord(entry.wItemIndex);
    const char *itemKorName = GetItemKorName(entry.byTblCode, entry.wItemIndex);
    entry.byRet = !_pkDb->CallProc_InsertCashItemLog(
      sheet->in_dwAvatorSerial,
      sheet->byLv,
      record->m_strCode,
      const_cast<char *>(itemKorName),
      entry.byOverlapNum,
      entry.dwCost);
  }
  return 0;
}

int CashDbWorker::_wait_tst_cash_total_selling_select(Task *pkTsk)
{
  auto *avgCashSelling = reinterpret_cast<unsigned int *>(pkTsk->GetTaskBuf());
  return _pkDb->CallProc_RFOnlineAvg_Event(avgCashSelling) != 0;
}

void CashDbWorker::_all_rollback(const _param_cash_update *psheet)
{
  _param_cash_rollback sheet(psheet->in_dwAccountSerial, psheet->in_dwAvatorSerial, psheet->in_wSockIndex);
  strcpy_s(sheet.in_szAcc, sizeof(sheet.in_szAcc), psheet->in_szAcc);
  for (int j = 0; j < psheet->in_nNum10; ++j)
  {
    const auto *item = &psheet->in_item[j];
    if (!item->byRet)
    {
      sheet.data[sheet.in_byNum].in_lnUID = item->in_lnUID;
      sheet.data[sheet.in_byNum].in_nPrice = item->in_nPrice;
      sheet.data[sheet.in_byNum].in_nDiscount = item->in_nDiscount;
      sheet.data[sheet.in_byNum].in_byOverlapNum = item->in_byOverlapNum;
      strcpy_s(sheet.data[sheet.in_byNum++].in_strItemCode, 8u, item->in_strItemCode);
      const char *itemKorName = GetItemKorName(item->in_byTblCode, item->in_wItemIdx);
      _kLogger[0].Write("Rollback >> %I64d-%s(%s)", item->in_lnUID, item->in_strItemCode, itemKorName);
    }
  }
  const int size = static_cast<int>(sheet.size());
  PushTask(2, reinterpret_cast<unsigned __int8 *>(&sheet), size);
}

bool CashDbWorker::_init_database()
{
  auto *db = static_cast<CRFCashItemDatabase *>(operator new(sizeof(CRFCashItemDatabase)));
  if (db)
  {
    new (db) CRFCashItemDatabase();
  }
  _pkDb = db;
  if (!_pkDb)
  {
    return false;
  }

  auto *nationSetting = CTSingleton<CNationSettingManager>::Instance();
  if (nationSetting->IsCashDBInit())
  {
    MyMessageBox("CashDbWorker::_init_database()", "Cash DB Already Initialized!\r\n");
    return false;
  }

  if (!nationSetting->IsCashDBDSNSetted())
  {
    MyMessageBox("CashDbWorker::_init_database()", "Cash DB DSN Setting Not Setted!!\r\n");
    return false;
  }

  const char *dbName = nationSetting->GetCashDBName();
  _pkDb->SetLogFile("..\\ZoneServerLog\\", dbName);

  const unsigned __int16 wPort = nationSetting->GetCashDBPort();
  char *szDatabase = nationSetting->GetCashDBName();
  char *szServer = nationSetting->GetCashDBDBIP();
  const char *dsn = nationSetting->GetCashDBName();
  if (_pkDb->ConfigUserODBC(dsn, szServer, szDatabase, wPort))
  {
    char *passWord = nationSetting->GetCashDBPW();
    char *accountName = nationSetting->GetCashDBID();
    const char *dbNameStart = nationSetting->GetCashDBName();
    if (_pkDb->StartDataBase(dbNameStart, accountName, passWord))
    {
      nationSetting->SetCashDBInitState();
      _pkDb->SetReconnectFailExitFlag(false);
      return true;
    }

    const char *cashPw = nationSetting->GetCashDBPW();
    const char *cashId = nationSetting->GetCashDBID();
    const unsigned __int16 cashPort = nationSetting->GetCashDBPort();
    const char *cashDbName = nationSetting->GetCashDBName();
    const char *cashDbIp = nationSetting->GetCashDBDBIP();
    const char *cashDbDsn = nationSetting->GetCashDBName();
    const char *nationStr = nationSetting->GetNationCodeStr();
    MyMessageBox(
      "CashDbWorker::_init_database()",
      "StartDataBase Failed!\r\n"
      "Nation(%s) DSN(%s) IP(%s) DBName(%s) Port(%u)\r\n"
      "ID(%s) PW(%s)\r\n"
      "Check Cash DB DSN Setting!",
      nationStr,
      cashDbDsn,
      cashDbIp,
      cashDbName,
      cashPort,
      cashId,
      cashPw);
    return false;
  }

  const char *cashPw = nationSetting->GetCashDBPW();
  const char *cashId = nationSetting->GetCashDBID();
  const unsigned __int16 cashPort = nationSetting->GetCashDBPort();
  const char *cashDbName = nationSetting->GetCashDBName();
  const char *cashDbIp = nationSetting->GetCashDBDBIP();
  const char *cashDbDsn = nationSetting->GetCashDBName();
  const char *nationStr = nationSetting->GetNationCodeStr();
  MyMessageBox(
    "CashDbWorker::_init_database()",
    "Cash DB ODBC Setting Failed!\r\n"
    "Nation(%s) DSN(%s) IP(%s) DBName(%s) Port(%u)\r\n"
    "ID(%s) PW(%s)\r\n"
    "Check Cash DB DSN Setting!",
    nationStr,
    cashDbDsn,
    cashDbIp,
    cashDbName,
    cashPort,
    cashId,
    cashPw);
  return false;
}

int CashDbWorker::ConvertErrorCode(char state)
{
  switch (state)
  {
    case '1':
      return 2;
    case '2':
      return 3;
    case '3':
      return 4;
    case '4':
      return 5;
    case '5':
      return 6;
    case '6':
      return 7;
    case '7':
      return 8;
    case '8':
      return 9;
    case '9':
      return 10;
    case 'F':
      return 13;
    case 'T':
      return 0;
    default:
      return 1;
  }
}

bool CashDbWorker::_init_loggers()
{
  CreateDirectoryA("..\\ZoneServerLog\\ServiceLog\\PartiallyPaid", nullptr);
  CreateDirectoryA("..\\ZoneServerLog\\SystemLog\\PartiallyPaid", nullptr);

  char buffer[256]{};
  unsigned int logTime = GetKorLocalTime();
  sprintf_s(buffer, "..\\ZoneServerLog\\ServiceLog\\PartiallyPaid\\%d.his", logTime);
  _kLogger[1].SetWriteLogFile(buffer, 1, 0, 1, 1);

  memset_0(buffer, 0, sizeof(buffer));
  logTime = GetKorLocalTime();
  sprintf_s(buffer, "..\\ZoneServerLog\\SystemLog\\PartiallyPaid\\%d.sys", logTime);
  _kLogger[0].SetWriteLogFile(buffer, 1, 0, 1, 1);
  return true;
}

CPlayer *CashDbWorker::_get_player(unsigned __int16 wSock, unsigned int dwAvator)
{
  if (g_Player[wSock].m_Param.GetCharSerial() == dwAvator)
  {
    return &g_Player[wSock];
  }
  return nullptr;
}

bool CashDbWorker::_insert_to_inven(CPlayer *const pOne, _param_cash_update::__item *pGII)
{
  g_Main.m_tblItemData[pGII->in_byTblCode].GetRecord(pGII->in_wItemIdx);

  _STORAGE_LIST::_db_con con;
  con.m_byTableCode = pGII->in_byTblCode;
  con.m_wItemIndex = pGII->in_wItemIdx;
  if (IsOverLapItem(pGII->in_byTblCode))
  {
    con.m_dwDur = pGII->in_byOverlapNum;
  }
  else
  {
    con.m_dwDur = GetItemDurPoint(pGII->in_byTblCode, pGII->in_wItemIdx);
  }

  const unsigned __int8 defSocketNum = GetDefItemUpgSocketNum(pGII->in_byTblCode, pGII->in_wItemIdx);
  con.m_dwLv = defSocketNum;
  con.m_dwLv = GetBitAfterSetLimSocket(defSocketNum);
  con.m_wSerial = pOne->m_Param.GetNewItemSerial();
  pGII->out_wItemSerial = con.m_wSerial;

  const _TimeItem_fld *timeRec = TimeItem::FindTimeRec(con.m_byTableCode, con.m_wItemIndex);
  if (pGII->in_nLendType)
  {
    con.m_byCsMethod = timeRec->m_nCheckType;
    __time32_t now[5];
    _time32(now);
    con.m_dwT = pGII->in_dwLendTime + now[0];
    con.m_dwLendRegdTime = now[0];
  }

  _STORAGE_LIST::_db_con *added = pOne->Emb_AddStorage(0, &con, 0, 1);
  if (added)
  {
    const int cashType = GetUsePcCashType(added->m_byTableCode, added->m_wItemIndex);
    if (added->m_byTableCode == 18 && !pOne->IsUsableAccountType(cashType))
    {
      pOne->SendMsg_PremiumCashItemUse(added->m_wSerial);
    }
    pGII->out_dwT = added->m_dwT;
    CPlayer::s_MgrItemHistory.buy_to_inven_cashitem(
      pGII->in_byTblCode,
      pGII->in_wItemIdx,
      pGII->in_nPrice,
      pGII->in_nDiscount,
      pGII->in_byOverlapNum,
      pGII->out_nBuyPrice,
      pGII->out_nCashAmount,
      pOne->m_szItemHistoryFileName,
      pGII->in_lnUID,
      pGII->in_nEventType);
    return true;
  }

  const char *itemKorName = GetItemKorName(pGII->in_byTblCode, pGII->in_wItemIdx);
  _kLogger[0].Write(
    "Failed _insert_to_inven() >> %s [price:%d discount:%d]",
    itemKorName,
    pGII->in_nPrice,
    pGII->in_nDiscount);
  return false;
}

void CashDbWorker::_delete_from_inven(CPlayer *const pOne, const _param_cash_update *psheet, int nNum)
{
  for (int j = 0; j < nNum; ++j)
  {
    const unsigned __int16 itemSerial = psheet->in_item[j].out_wItemSerial;
    _STORAGE_LIST::_db_con *item = pOne->m_Param.m_dbInven.GetPtrFromSerial(itemSerial);
    if (item)
    {
      if (!pOne->Emb_DelStorage(0, item->m_byStorageIndex, 0, 1, "CashDbWorker::_delete_from_inven"))
      {
        _kLogger[0].Write(
          "CashDbWorker::_delete_from_inven : Emb_DelStorage Failed! item::%s",
          psheet->in_item[j].in_strItemCode);
      }
      CPlayer::s_MgrItemHistory.cashitem_del_from_inven(
        item->m_byTableCode,
        item->m_wItemIndex,
        psheet->in_item[j].in_lnUID,
        pOne->m_szItemHistoryFileName);
    }
    else
    {
      _kLogger[0].Write("Failed delete item::%s", psheet->in_item[j].in_strItemCode);
    }
  }
}

bool CashDbWorker::PushTask(int nTaskCode, unsigned __int8 *p, unsigned __int64 size)
{
  return _pkPool->PushWaitTsk(nTaskCode, p, size) == 0;
}

bool CashDbWorker::IsNULL()
{
  return this == &CCashDbWorkerNULL::ms_NULL;
}

bool CashDbWorker::GetBillingDBConnectionStatus()
{
  return _pkDb && _pkDb->IsConectionActive();
}

void CashDbWorker::SendMsgSucceedBuy(unsigned __int16 wSock, const _param_cash_update *sheet)
{
  _result_csi_buy_zocl msg{};
  msg.nNum = sheet->in_nNum10;
  msg.nCashAmount = sheet->out_nCashAmount;
  for (int j = 0; j < sheet->in_nNum10; ++j)
  {
    msg.item[j].byTblCode = sheet->in_item[j].in_byTblCode;
    msg.item[j].wItemIdx = sheet->in_item[j].in_wItemIdx;
    msg.item[j].dwDur = sheet->in_item[j].in_byOverlapNum;
    msg.item[j].dwUp = GetDefItemUpgSocketNum(sheet->in_item[j].in_byTblCode, sheet->in_item[j].in_wItemIdx);
    msg.item[j].dwItemSerial = sheet->in_item[j].out_wItemSerial;
  }
  unsigned __int8 type[2] = {57, 4};
  const unsigned __int16 len = msg.size();
  g_Network.m_pProcess[0]->LoadSendMsg(wSock, type, reinterpret_cast<char *>(&msg), len);
}
