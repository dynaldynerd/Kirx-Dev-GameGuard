#include "pch.h"

#include "CUnmannedTraderUserInfoTable.h"
#include "CNetProcess.h"
#include "CPlayer.h"
#include "CPlayerDB.h"
#include "CRecordData.h"
#include "CUnmannedTraderGroupItemInfoTable.h"
#include "CUnmannedTraderTaxRateManager.h"
#include "CHolyStoneSystem.h"
#include "CHonorGuild.h"
#include "GlobalObjects.h"
#include "InvenKey.h"
#include "WorldServerUtil.h"
#include "lt_qry_case_unmandtrader_select_list.h"
#include "qry_case_unmandtrader_cancelitem.h"
#include "qry_case_unmandtrader_buy_get_info.h"
#include "qry_case_unmandtrader_cheat_updateregisttime.h"
#include "qry_case_unmandtrader_re_registsingleitem.h"
#include "qry_case_unmandtrader_re_registsingleitem_roll_back.h"
#include "qry_case_unmandtrader_registsingleitem.h"
#include "qry_case_unmandtrader_time_out_cancelitem.h"
#include "qry_case_unmandtrader_update_reprice.h"

#include <algorithm>
#include <cstdarg>
#include <cstring>
#include <ctime>

CUnmannedTraderUserInfoTable *CUnmannedTraderUserInfoTable::ms_Instance;
unsigned int CUnmannedTraderUserInfoTable::m_uiBuyRollBackCallCountSum;

CUnmannedTraderUserInfoTable::CUnmannedTraderUserInfoTable()
  : m_pkLogger(nullptr), m_pkServiceLogger(nullptr), m_veckInfo()
{
}

CUnmannedTraderUserInfoTable::~CUnmannedTraderUserInfoTable() = default;

CUnmannedTraderUserInfoTable *CUnmannedTraderUserInfoTable::Instance()
{
  if (!CUnmannedTraderUserInfoTable::ms_Instance)
  {
    CUnmannedTraderUserInfoTable::ms_Instance = new CUnmannedTraderUserInfoTable();
  }
  return CUnmannedTraderUserInfoTable::ms_Instance;
}

void CUnmannedTraderUserInfoTable::Destroy()
{
  if (CUnmannedTraderUserInfoTable::ms_Instance)
  {
    delete CUnmannedTraderUserInfoTable::ms_Instance;
    CUnmannedTraderUserInfoTable::ms_Instance = nullptr;
  }
}

bool CUnmannedTraderUserInfoTable::Init()
{
  CUnmannedTraderUserInfo info;
  this->m_veckInfo.assign(2532, info);
  if (this->m_veckInfo.size() != 2532)
  {
    return false;
  }

  for (unsigned short index = 0; index < 2532; ++index)
  {
    CUnmannedTraderUserInfo &entry = this->m_veckInfo[index];
    if (!entry.Init(index))
    {
      return false;
    }
  }
  return true;
}

bool CUnmannedTraderUserInfoTable::Load(
  unsigned __int8 byType,
  unsigned __int16 wInx,
  unsigned int dwSerial,
  _TRADE_DB_BASE *kInfo)
{
  if (this->m_veckInfo.empty() && this->m_veckInfo.size() <= wInx)
  {
    return false;
  }

  CUnmannedTraderUserInfo &user = this->m_veckInfo[wInx];
  return user.Load(byType, wInx, dwSerial, kInfo, this->m_pkLogger);
}

bool CUnmannedTraderUserInfoTable::CheckwIndexAndType(
  unsigned __int16 wInx,
  unsigned __int8 byType,
  const char * /*szCallFuncName*/)
{
  return !this->m_veckInfo.empty() && this->m_veckInfo.size() > wInx && byType < 2u;
}

void CUnmannedTraderUserInfoTable::Regist(
  unsigned __int16 wInx,
  unsigned __int8 byType,
  _a_trade_reg_item_request_clzo *pRequest)
{
  if (CheckwIndexAndType(wInx, byType, "CUnmannedTraderUserInfoTable::Regist(...)"))
  {
    this->m_veckInfo[wInx].Regist(byType, pRequest, this->m_pkLogger);
  }
}

void CUnmannedTraderUserInfoTable::ModifyPrice(
  unsigned __int16 wInx,
  unsigned __int8 byType,
  _a_trade_adjust_price_request_clzo *pRequest)
{
  if (CheckwIndexAndType(wInx, byType, "CUnmannedTraderUserInfoTable::ModifyPrice(...)"))
  {
    this->m_veckInfo[wInx].ModifyPrice(byType, pRequest, this->m_pkLogger);
  }
}

void CUnmannedTraderUserInfoTable::CancelRegist(
  unsigned __int16 wInx,
  unsigned __int8 byType,
  _a_trade_clear_item_request_clzo *pRequest)
{
  if (CheckwIndexAndType(wInx, byType, "CUnmannedTraderUserInfoTable::CancelRegist(...)"))
  {
    this->m_veckInfo[wInx].CancelRegist(byType, pRequest, this->m_pkLogger);
  }
}

void CUnmannedTraderUserInfoTable::Buy(
  unsigned __int16 wInx,
  unsigned __int8 byType,
  _unmannedtrader_buy_item_request_clzo *pRequest)
{
  if (CheckwIndexAndType(wInx, byType, "CUnmannedTraderUserInfoTable::Buy(...)"))
  {
    this->m_veckInfo[wInx].Buy(byType, pRequest, this->m_pkLogger);
  }
}

void CUnmannedTraderUserInfoTable::Search(
  unsigned __int16 wInx,
  unsigned __int8 byType,
  _unmannedtrader_search_list_request_clzo *pRequest)
{
  if (CheckwIndexAndType(wInx, byType, "CUnmannedTraderUserInfoTable::Search(...)"))
  {
    this->m_veckInfo[wInx].Search(byType, pRequest, this->m_pkLogger);
  }
}

void CUnmannedTraderUserInfoTable::ReRegist(
  unsigned __int16 wInx,
  unsigned __int8 byType,
  _unmannedtrader_re_regist_request_clzo *pRequest)
{
  if (CheckwIndexAndType(wInx, byType, "CUnmannedTraderUserInfoTable::ReRegist(...)"))
  {
    this->m_veckInfo[wInx].ReRegist(byType, pRequest, this->m_pkLogger);
  }
}

void CUnmannedTraderUserInfoTable::SetLogger(CLogFile *pkLogger, CLogFile *pkServiceLogger)
{
  this->m_pkLogger = pkLogger;
  this->m_pkServiceLogger = pkServiceLogger;
}

CUnmannedTraderUserInfo *CUnmannedTraderUserInfoTable::Find(unsigned int dwSerial)
{
  const auto endIt = this->m_veckInfo.end();
  const auto it = std::find(this->m_veckInfo.begin(), endIt, dwSerial);
  if (it == endIt)
  {
    return &CUnmannedTraderUserInfo::ms_kNull;
  }
  return &(*it);
}

CUnmannedTraderUserInfo *CUnmannedTraderUserInfoTable::FindByIndex(unsigned __int16 wInx)
{
  if (this->m_veckInfo.size() > wInx)
  {
    return &this->m_veckInfo[wInx];
  }
  return nullptr;
}

CUnmannedTraderUserInfo *CUnmannedTraderUserInfoTable::FindUser(unsigned __int16 wInx, unsigned int dwSerial)
{
  CUnmannedTraderUserInfo *user = FindByIndex(wInx);
  if (user && user->operator==(dwSerial))
  {
    return user;
  }
  return Find(dwSerial);
}

bool CUnmannedTraderUserInfoTable::CheatCancelRegist(
  unsigned __int16 wInx,
  unsigned int dwOwnerSerial,
  unsigned __int8 byNth)
{
  CUnmannedTraderUserInfo *user = FindUser(wInx, dwOwnerSerial);
  if (!user || user->IsNull())
  {
    return false;
  }

  CPlayer *owner = user->FindOwner();
  if (!owner || !owner->m_bOper)
  {
    return false;
  }

  return user->CheatCancelRegist(byNth);
}

unsigned __int8 CUnmannedTraderUserInfoTable::GetMaxRegistCnt(unsigned __int16 wInx, unsigned int dwSerial)
{
  if (this->m_veckInfo.empty() || this->m_veckInfo.size() <= wInx)
  {
    return 0;
  }

  CUnmannedTraderUserInfo &info = this->m_veckInfo[wInx];
  if (!info.IsLogInState())
  {
    return 0;
  }
  if (info.GetSerial() != dwSerial)
  {
    return 0;
  }

  return info.GetMaxRegistCnt();
}

const CUnmannedTraderRegistItemInfo *CUnmannedTraderUserInfoTable::GetRegItemInfo(
  unsigned __int16 wInx,
  unsigned int dwSerial)
{
  if (this->m_veckInfo.empty() || this->m_veckInfo.size() <= wInx)
  {
    return nullptr;
  }

  CUnmannedTraderUserInfo &info = this->m_veckInfo[wInx];
  if (!info.IsLogInState())
  {
    return nullptr;
  }
  if (info.GetSerial() != dwSerial)
  {
    return nullptr;
  }

  return info.GetRegItemInfo();
}

void CUnmannedTraderUserInfoTable::CompleteCreate(unsigned __int16 wInx)
{
  if (!this->m_veckInfo.empty() && this->m_veckInfo.size() > wInx)
  {
    CUnmannedTraderUserInfo &info = this->m_veckInfo[wInx];
    info.CompleteCreate(this->m_pkLogger);
  }
}

void CUnmannedTraderUserInfoTable::CompleteSearch(
  unsigned __int8 byDBRet,
  unsigned __int8 byProcRet,
  _lt_qry_case_unmandtrader_select_list *pLoadData)
{
  CUnmannedTraderUserInfo *user = FindUser(pLoadData->wInx, pLoadData->dwOwnerSerial);
  if (!user->IsNull() && g_Player[user->GetIndex()].m_bOper)
  {
    user->ClearRequest();
    CPlayer *owner = user->FindOwner();
    if (owner)
    {
      if (byDBRet || byProcRet)
      {
        user->SendSearchErrorResult(owner->m_ObjID.m_wIndex, byProcRet);
      }
      else
      {
        user->SendSearchResult(owner->m_ObjID.m_wIndex, pLoadData);
      }
    }
  }
}

void CUnmannedTraderUserInfoTable::LogOut(unsigned __int16 wInx, unsigned int dwSerial)
{
  if (!this->m_veckInfo.empty() && this->m_veckInfo.size() > wInx)
  {
    this->m_veckInfo[wInx].LogOut(dwSerial, this->m_pkLogger);
  }
}

void CUnmannedTraderUserInfoTable::ClearRequest(unsigned __int16 wInx, unsigned int dwOwner)
{
  CUnmannedTraderUserInfo *user = FindUser(wInx, dwOwner);
  if (!user->IsNull())
  {
    CPlayer *owner = user->FindOwner();
    if (owner && owner->m_bOper)
    {
      user->ClearRequest();
    }
  }
}

bool CUnmannedTraderUserInfoTable::CompleteUpdateState(
  unsigned int dwOwnerSerial,
  unsigned int dwRegistSerial,
  unsigned __int8 byState)
{
  CUnmannedTraderUserInfo *user = Find(dwOwnerSerial);
  return !user->IsNull() && user->CompleteUpdateState(dwRegistSerial, byState, true);
}

void CUnmannedTraderUserInfoTable::CompleteRegist(
  unsigned __int8 byRet,
  _qry_case_unmandtrader_registsingleitem *pLoadData)
{
  CUnmannedTraderUserInfo *user =
    FindUser(pLoadData->wInx, pLoadData->dwOwnerSerial);
  CPlayer *owner = nullptr;
  if (!user->IsNull() && (owner = user->FindOwner()) != nullptr && owner->m_bOper)
  {
    user->ClearRequest();
    user->CompleteRegist(byRet, pLoadData, m_pkLogger);
  }
  else if (byRet)
  {
    g_Main.Push_ChargeItem(
      pLoadData->dwOwnerSerial,
      -1,
      pLoadData->dwTax,
      268435455,
      1u);
    Log(
      "CUnmannedTraderUserInfoTable::CompleteRegist( BYTE byRet, char * pLoadData )\r\n"
      "\t\tdwRegistSerial(%u) dwOwnerSerial(%u)\r\n"
      "\t\tPush_ChargeItem( dwOwnerSerial(%u), Tax(%u) )!\r\n",
      pLoadData->dwRegedSerial,
      pLoadData->dwOwnerSerial,
      pLoadData->dwOwnerSerial,
      pLoadData->dwTax);
  }
  else
  {
    CUnmannedTraderGroupItemInfoTable *groupTable = CUnmannedTraderGroupItemInfoTable::Instance();
    if (!groupTable->IncreaseVersion(pLoadData->byClass1, pLoadData->byClass2))
    {
      if (m_pkLogger)
      {
        m_pkLogger->Write(
          "CUnmannedTraderController::CompleteRegist(...)\r\n"
          "\t\tOwner : (%u)\r\n"
          "\t\tCUnmannedTraderGroupItemInfoTable::Instance()->IncreaseVersion(\r\n"
          "\t\tpkQuery->byClass1(%u), pkQuery->byClass2(%u) ) Fail!\r\n",
          pLoadData->dwOwnerSerial,
          pLoadData->byClass1,
          pLoadData->byClass2);
      }
    }
  }
}

void CUnmannedTraderUserInfoTable::CompleteCancelRegist(
  unsigned __int8 byRet,
  _qry_case_unmandtrader_cancelitem *pLoadData)
{
  CUnmannedTraderUserInfo *user = FindUser(pLoadData->wInx, pLoadData->dwOwnerSerial);
  if (!user->IsNull() && g_Player[user->GetIndex()].m_bOper && user->FindOwner())
  {
    user->ClearRequest();
    user->CompleteCancelRegist(byRet, pLoadData, m_pkLogger);
  }
  else
  {
    ClearLogLogOutState(
      "Self Cancel",
      pLoadData->dwOwnerSerial,
      pLoadData->szAccount,
      pLoadData->wszName,
      pLoadData->dwRegistSerial,
      pLoadData->dwK,
      pLoadData->dwD,
      pLoadData->dwU);
  }
}

void CUnmannedTraderUserInfoTable::CompleteTimeOutClear(_qry_case_unmandtrader_time_out_cancelitem *pLoadData)
{
  CUnmannedTraderUserInfo *user = Find(pLoadData->dwOwnerSerial);
  if (!user->IsNull() && g_Player[user->GetIndex()].m_bOper && user->FindOwner())
  {
    user->CompleteTimeOutClear(pLoadData->dwRegistSerial, m_pkLogger);
  }
  else
  {
    ClearLogLogOutState(
      "Time Out",
      pLoadData->dwOwnerSerial,
      pLoadData->szAccountID,
      pLoadData->wszName,
      pLoadData->dwRegistSerial,
      pLoadData->dwK,
      pLoadData->dwD,
      pLoadData->dwU);
  }
}

void CUnmannedTraderUserInfoTable::CompleteReprice(
  unsigned __int8 byRet,
  _qry_case_unmandtrader_update_reprice *pLoadData)
{
  CUnmannedTraderUserInfo *user = FindUser(pLoadData->wInx, pLoadData->dwOwnerSerial);
  if (!user->IsNull() && g_Player[user->GetIndex()].m_bOper)
  {
    user->ClearRequest();
    user->CompleteReprice(byRet, pLoadData, m_pkLogger);
  }
}

void CUnmannedTraderUserInfoTable::CompleteBuy(
  unsigned __int8 byRet,
  _qry_case_unmandtrader_buy_update_wait *pLoadData,
  CUnmannedTraderTradeInfo *pkTaradInfo)
{
CUnmannedTraderUserInfo *pkBuyUser = nullptr;
  CPlayer *pkBuyPlayer = nullptr;
  if (SubCompleteBuyFindBuyer(pLoadData, &pkBuyUser, &pkBuyPlayer))
  {
    pkBuyUser->ClearRequest();

    _unmannedtrader_buy_item_result_zocl result{};
    std::memset(&result, 0, sizeof(result));
    result.byRetCode = 0;
    result.byNum = pLoadData->byNum;

    _qry_case_unmandtrader_buy_update_complete qry{};
    std::memset(&qry, 0, sizeof(qry));
    qry.wInx = pkBuyPlayer->m_ObjID.m_wIndex;
    qry.dwBuyer = pLoadData->dwBuyer;
    qry.byRace = static_cast<unsigned __int8>(pkBuyPlayer->m_Param.GetRaceCode());
    qry.byType = pLoadData->byType;
    qry.byNum = 0;

    unsigned __int8 successCount = 0;
    unsigned __int8 updateState8Count = 0;

    for (int j = 0; j < pLoadData->byNum; ++j)
    {
      if (SubCompleteBuyProcBuy(
            pkBuyPlayer,
            pkBuyUser,
            pLoadData->tResultTime,
            &pLoadData->List[j],
            &result.List[j],
            &qry.List[j],
            &qry.byNum,
            &result.dwPayDalant,
            pkTaradInfo))
      {
        ++successCount;
      }
      else if (qry.List[j].byUpdateState == 8)
      {
        ++updateState8Count;
      }
    }

    if (updateState8Count || successCount)
    {
      SubCompleteBuyIncreaseVesion(pLoadData->byDivision, pLoadData->byClass);
    }

    // Keep the original sparse slot layout, but make the DQS completion pass
    // scan the full processed request span so later valid entries are not skipped.
    qry.byNum = pLoadData->byNum;

    if (qry.byNum)
    {
      g_Main.PushDQSData(-1, nullptr, 68, reinterpret_cast<char *>(&qry), sizeof(qry));
    }

    if (successCount < result.byNum)
    {
      result.byRetCode = static_cast<unsigned __int8>(-1);
    }

    result.dwLeftDalant = pkBuyPlayer->m_Param.GetDalant();

    unsigned __int8 type[2]{};
    type[0] = 30;
    type[1] = 31;
    const unsigned __int16 size = static_cast<unsigned __int16>(result.size());
    g_Network.m_pProcess[0]->LoadSendMsg(pkBuyPlayer->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&result), size);
  }
}

void CUnmannedTraderUserInfoTable::CompleteReRegist(_qry_case_unmandtrader_re_registsingleitem *pLoadData)
{
  CUnmannedTraderUserInfo *user = FindUser(pLoadData->wInx, pLoadData->dwOwnerSerial);
  CPlayer *owner = nullptr;
  if (!user->IsNull() && (owner = user->FindOwner()) != nullptr && owner->m_bOper)
  {
    user->ClearRequest();
    user->CompleteReRegist(pLoadData, m_pkLogger);
  }
  else
  {
    unsigned int sumTax = 0;
    for (unsigned __int8 j = 0; j < pLoadData->byNum; ++j)
    {
      const _qry_case_unmandtrader_re_registsingleitem::__list &entry = pLoadData->List[j];
      if (entry.byProcRet)
      {
        if (entry.bRegist && (entry.byProcRet == 202 || entry.byProcRet == 24))
        {
          sumTax += entry.dwTax;
        }
        Log(
          "CUnmannedTraderUserInfoTable::CompleteReRegist( BYTE byRet, char * pLoadData )\r\n"
          "\t\tdwRegistSerial(%u) dwOwnerSerial(%u) bRegist(%d) byProcRet(%u)\r\n",
          entry.dwRegistSerial,
          pLoadData->dwOwnerSerial,
          entry.bRegist,
          entry.byProcRet);
      }
      else
      {
        CUnmannedTraderGroupItemInfoTable *groupTable = CUnmannedTraderGroupItemInfoTable::Instance();
        if (!groupTable->IncreaseVersion(entry.byClass1, entry.byClass2))
        {
          if (m_pkLogger)
          {
            m_pkLogger->Write(
              "CUnmannedTraderController::CompleteRegist(...)\r\n"
              "\t\tOwner : (%u)\r\n"
              "\t\tCUnmannedTraderGroupItemInfoTable::Instance()->IncreaseVersion(\r\n"
              "\t\tpkQuery->byClass1(%u), pkQuery->byClass2(%u) ) Fail!\r\n",
              pLoadData->dwOwnerSerial,
              entry.byClass1,
              entry.byClass2);
          }
        }
      }
    }

    if (sumTax)
    {
      g_Main.Push_ChargeItem(pLoadData->dwOwnerSerial, -1, sumTax, 268435455, 1u);
      Log(
        "CUnmannedTraderUserInfoTable::CompleteReRegist( BYTE byRet, char * pLoadData )\r\n"
        "\t\tdwOwnerSerial(%u) Push_ChargeItem( dwSumTax(%u) )!\r\n",
        pLoadData->dwOwnerSerial,
        sumTax);
    }
  }
}

void CUnmannedTraderUserInfoTable::CompleteUpdateCheatRegistTime(
  _qry_case_unmandtrader_cheat_updateregisttime *pLoadData)
{
  CUnmannedTraderUserInfo *user = FindUser(pLoadData->wInx, pLoadData->dwOwnerSerial);
  if (!user->IsNull())
  {
    CPlayer *owner = user->FindOwner();
    if (owner && owner->m_bOper)
    {
      user->CompleteUpdateCheatRegistTime(pLoadData);
    }
  }
}

void CUnmannedTraderUserInfoTable::CompleteReRegistRollBack(
  _qry_case_unmandtrader_re_registsingleitem_roll_back *pData)
{
  CUnmannedTraderUserInfo *user = FindUser(pData->wInx, pData->dwOwnerSerial);
  if (!user->IsNull())
  {
    CPlayer *owner = user->FindOwner();
    if (owner && owner->m_bOper)
    {
      user->CompleteReRegistRollBack(pData, m_pkLogger);
    }
  }
}

CUnmannedTraderItemState::STATE CUnmannedTraderUserInfoTable::GetCloseItemForPassTimeUpdateInfo(
  unsigned int dwOwnerSerial,
  unsigned int dwRegistSerial,
  CPlayer **pkOwner)
{
  CUnmannedTraderUserInfo *user = Find(dwOwnerSerial);
  return user->GetCloseItemForPassTimeUpdateInfo(dwRegistSerial, pkOwner);
}

void CUnmannedTraderUserInfoTable::ClearLogLogOutState(
  const char *szType,
  unsigned int dwOwner,
  const char *szAccount,
  char *wszName,
  unsigned int dwRegistSerial,
  unsigned int dwK,
  unsigned int dwD,
  unsigned int dwU)
{
  char szTran[44]{};
  W2M(wszName, szTran, 17);

  _INVENKEY key;
  key.LoadDBKey(dwK);

  _base_fld *record = g_Main.m_tblItemData[key.byTableCode].GetRecord(key.wItemIndex);
  if (record)
  {
    char dateBuffer[48]{};
    _strdate(dateBuffer);
    dateBuffer[5] = 0;

    char timeBuffer[40]{};
    _strtime(timeBuffer);
    timeBuffer[5] = 0;

    const char *accountName = *szAccount ? szAccount : "NULL";
    const char *ownerName = szTran[0] ? szTran : "NULL";
    const char *upgInfo = DisplayItemUpgInfo(key.byTableCode, dwU);

    ServiceLog(
      "OWNER_LOGOUT : Type(%s) %s(%u) ID(%s) RegistItemSerial(%u) %s_%u_@%s[null] [%s %s]\r\n",
      szType,
      ownerName,
      dwOwner,
      accountName,
      dwRegistSerial,
      record->m_strCode,
      dwD,
      upgInfo,
      dateBuffer,
      timeBuffer);
  }
  else
  {
    const char *ownerName = szTran[0] ? szTran : "NULL";
    const char *accountName = *szAccount ? szAccount : "NULL";
    Log(
      "CUnmannedTraderUserInfoTable::ClearLogLogOutState( dwOwner(%u) szAccount(%s), wszName(%s) dwRegistSerial(%u), dwK(%u), dwD(%u), dwU(%u) )\r\n"
      "\t\tType(%s) _base_fld * p = g_Main.m_tblItemData[kKey.byTableCode(%u)].GetRecord( kKey.wItemIndex(%u) ) NULL!\r\n",
      dwOwner,
      accountName,
      ownerName,
      dwRegistSerial,
      dwK,
      dwD,
      dwU,
      szType,
      key.byTableCode,
      key.wItemIndex);
  }
}

void CUnmannedTraderUserInfoTable::Log(const char *fmt, ...)
{
  if (!m_pkLogger)
  {
    return;
  }

  va_list args;
  va_start(args, fmt);
  m_pkLogger->WriteFromArg(fmt, args);
  va_end(args);
}

void CUnmannedTraderUserInfoTable::ServiceLog(const char *fmt, ...)
{
  if (!m_pkServiceLogger)
  {
    return;
  }

  va_list args;
  va_start(args, fmt);
  m_pkServiceLogger->WriteFromArg(fmt, args);
  va_end(args);
}

bool CUnmannedTraderUserInfoTable::SubCompleteBuyFindBuyer(
  _qry_case_unmandtrader_buy_update_wait *pkQuery,
  CUnmannedTraderUserInfo **ppkBuyUser,
  CPlayer **ppkBuyPlayer)
{
  *ppkBuyUser = FindUser(pkQuery->wInx, pkQuery->dwBuyer);
  if (!(*ppkBuyUser)->IsNull() && g_Player[(*ppkBuyUser)->GetIndex()].m_bOper)
  {
    *ppkBuyPlayer = (*ppkBuyUser)->FindOwner();
    if (*ppkBuyPlayer)
    {
      return true;
    }

    PushUpdateBuyRollBack(pkQuery);
    char buffer[1028]{};
    int offset = 0;
    for (unsigned __int8 j = 0; j < pkQuery->byNum; ++j)
    {
      const size_t remaining = sizeof(buffer) - static_cast<size_t>(offset);
      offset += sprintf_s(&buffer[offset], remaining, "%u ", pkQuery->List[j].dwRegistSerial);
    }
    ++m_uiBuyRollBackCallCountSum;
    Log(
      "SubCompleteBuyFindBuyer:: Count(%u) PushUpdateBuyRollBack Call! dwOwner(%u) pkBuyer->FindOwner() NULL!\r\n"
      "\t\tItemCnt(%u) %s\r\n",
      m_uiBuyRollBackCallCountSum,
      (*ppkBuyUser)->GetSerial(),
      pkQuery->byNum,
      buffer);
    return false;
  }

  PushUpdateBuyRollBack(pkQuery);
  char buffer[1028]{};
  int offset = 0;
  for (unsigned __int8 j = 0; j < pkQuery->byNum; ++j)
  {
    const size_t remaining = sizeof(buffer) - static_cast<size_t>(offset);
    offset += sprintf_s(&buffer[offset], remaining, "%u ", pkQuery->List[j].dwRegistSerial);
  }
  Log(
    "SubCompleteBuyFindBuyer:: Count(%u) PushUpdateBuyRollBack Call! dwOwner(%u) Not Connected!\r\n"
    "\t\tItemCnt(%u) %s\r\n",
    ++m_uiBuyRollBackCallCountSum,
    pkQuery->dwBuyer,
    pkQuery->byNum,
    buffer);
  return false;
}

bool CUnmannedTraderUserInfoTable::SubCompleteBuyProcBuy(
  CPlayer *pkBuyPlayer,
  CUnmannedTraderUserInfo *pkBuyUser,
  __int64 tResultTime,
  const _qry_case_unmandtrader_buy_update_wait::__list *pkQueryList,
  _unmannedtrader_buy_item_result_zocl::__list *pSendResultList,
  _qry_case_unmandtrader_buy_update_complete::__list *pUpdateCompleteList,
  unsigned __int8 *byCompleteUpdateNum,
  unsigned int *pdwPayDalant,
  CUnmannedTraderTradeInfo *pkTaradInfo)
{
  pSendResultList->byRet = pkQueryList->byProcRet;
  pSendResultList->dwPrice = pkQueryList->dwPrice;

  pUpdateCompleteList->byProcUpdate = static_cast<unsigned __int8>(-1);
  pUpdateCompleteList->dwSeller = pkQueryList->dwSeller;
  pUpdateCompleteList->dwRegistSerial = pkQueryList->dwRegistSerial;
  pUpdateCompleteList->byUpdateState = pkQueryList->byOldState;

  if (pSendResultList->byRet)
  {
    return false;
  }

  bool hasSeller = true;
  const unsigned int dwRealPrice = pkQueryList->dwPrice - pkQueryList->dwTax;
  CPlayer *pkSellPlayer = nullptr;
  CUnmannedTraderUserInfo *sellUser = Find(pkQueryList->dwSeller);
  if (!sellUser->IsNull() && g_Player[sellUser->GetIndex()].m_bOper &&
      (pkSellPlayer = sellUser->FindOwner()) != nullptr)
  {
    pSendResultList->byRet = sellUser->CheckSellComplete(
      pkSellPlayer,
      pkBuyPlayer,
      pkQueryList->dwRegistSerial,
      dwRealPrice,
      m_pkLogger);
    if (pSendResultList->byRet)
    {
      SubCompleteBuyProcBuyResult(pSendResultList->byRet, pUpdateCompleteList, byCompleteUpdateNum);
      return false;
    }
  }
  else
  {
    SubCompleteBuyProcBuyResult(81, pUpdateCompleteList, byCompleteUpdateNum);
    hasSeller = false;
  }

  pSendResultList->byRet = pkBuyUser->CheckBuyComplete(pkBuyPlayer, pkQueryList->dwPrice);
  if (pSendResultList->byRet)
  {
    SubCompleteBuyProcBuyResult(pSendResultList->byRet, pUpdateCompleteList, byCompleteUpdateNum);
    return false;
  }

  unsigned __int8 sellRet = 0;
  if (!hasSeller ||
      ((sellRet = sellUser->SellComplete(
          pkSellPlayer,
          pkBuyPlayer,
          pkQueryList->dwPrice,
          dwRealPrice,
          pkQueryList->dwTax,
          pkQueryList->dwRegistSerial,
          tResultTime,
          m_pkLogger)),
       SubCompleteBuyProcBuyResult(sellRet, pUpdateCompleteList, byCompleteUpdateNum),
       sellRet == 90))
  {
    unsigned __int16 wAddItemSerial[8]{};
    wAddItemSerial[0] = 255;
    unsigned __int8 buyRet = pkBuyUser->BuyComplete(
      pkBuyPlayer,
      pkQueryList->dwSeller,
      pkQueryList->wszName,
      pkQueryList->szAccountID,
      pkQueryList->dwRegistSerial,
      pkQueryList->dwK,
      pkQueryList->dwD,
      pkQueryList->dwU,
      pkQueryList->dwPrice,
      pkQueryList->lnUID,
      m_pkLogger,
      wAddItemSerial);
    if (buyRet)
    {
      SubCompleteBuyProcBuyResult(buyRet, pUpdateCompleteList, byCompleteUpdateNum);
      pSendResultList->byRet = buyRet;
      return false;
    }

    _INVENKEY key;
    key.LoadDBKey(pkQueryList->dwK);
    pSendResultList->dwNewItemSerial = wAddItemSerial[0];
    pSendResultList->byItemTableCode = key.byTableCode;
    pSendResultList->wItemIndex = key.wItemIndex;
    pSendResultList->dwDur = pkQueryList->dwD;
    pSendResultList->dwLv = pkQueryList->dwU;

    *pdwPayDalant += dwRealPrice;

    unsigned int dwTax = 0;
    unsigned int dwTaxTotal = 0;
    if (g_HolySys.GetHolyMasterRace() == -1)
    {
      dwTaxTotal = 75 * pkQueryList->dwTax / 100;
      dwTax = dwTaxTotal / 2;
      const int raceCode = pkBuyPlayer->m_Param.GetRaceCode();
      CUnmannedTraderTaxRateManager::Instance()->SetPatriarchTaxMoney(raceCode, dwTax);
      CHonorGuild::Instance()->SetGuildMaintainMoney(raceCode, dwTaxTotal, pkQueryList->dwSeller);
    }
    else
    {
      const int buyerRace = pkBuyPlayer->m_Param.GetRaceCode();
      const int holyMasterRace = g_HolySys.GetHolyMasterRace();
      if (buyerRace == holyMasterRace)
      {
        dwTaxTotal = 75 * pkQueryList->dwTax / 100;
      }
      else
      {
        dwTaxTotal = 50 * pkQueryList->dwTax / 100;
      }
      dwTax = dwTaxTotal / 2;
      CUnmannedTraderTaxRateManager::Instance()->SetPatriarchTaxMoney(holyMasterRace, dwTax);
      CHonorGuild::Instance()->SetGuildMaintainMoney(holyMasterRace, dwTaxTotal, pkQueryList->dwSeller);
    }

    if (pkTaradInfo)
    {
      pkTaradInfo->AddIncome(pkQueryList->dwPrice);
    }
    return true;
  }

  pSendResultList->byRet = sellRet;
  return false;
}

bool CUnmannedTraderUserInfoTable::SubCompleteBuyProcBuyResult(
  unsigned __int8 byRet,
  _qry_case_unmandtrader_buy_update_complete::__list *pUpdateCompleteList,
  unsigned __int8 *byCompleteUpdateNum)
{
  switch (byRet)
  {
    case 34:
    case 42:
    case 43:
    case 150:
    case 151:
      pUpdateCompleteList->byUpdateState = 1;
      break;
    case 37:
    case 99:
      pUpdateCompleteList->byUpdateState = 8;
      break;
    case 81:
      pUpdateCompleteList->byUpdateState = 4;
      break;
    case 90:
      pUpdateCompleteList->byUpdateState = 3;
      break;
    default:
      return false;
  }

  if (pUpdateCompleteList->byProcUpdate == 255)
  {
    ++*byCompleteUpdateNum;
  }
  pUpdateCompleteList->byProcUpdate = byRet;
  return true;
}

void CUnmannedTraderUserInfoTable::SubCompleteBuyIncreaseVesion(
  unsigned __int8 byDivision,
  unsigned __int8 byClass)
{
  CUnmannedTraderGroupItemInfoTable *groupTable = CUnmannedTraderGroupItemInfoTable::Instance();
  if (!groupTable->IncreaseVersion(byDivision, byClass))
  {
    Log(
      "CUnmannedTraderUserInfoTable::SubCompleteBuyIncreaseVesion( byDivision(%u), byClass(%u) )\r\n"
      "\t\tCUnmannedTraderGroupItemInfoTable::Instance()->IncreaseVersion( byDivision, byClass ) Fail!\r\n",
      byDivision,
      byClass);
  }
}

void CUnmannedTraderUserInfoTable::PushUpdateBuyRollBack(_qry_case_unmandtrader_buy_update_wait *pkQuery)
{
  _qry_case_unmandtrader_buy_update_rollback qry{};
  std::memset(&qry, 0, sizeof(qry));

  qry.wInx = pkQuery->wInx;
  qry.dwBuyer = pkQuery->dwBuyer;
  qry.byRace = pkQuery->byRace;
  qry.byType = pkQuery->byType;
  qry.byNum = pkQuery->byNum;

  unsigned __int8 outCount = 0;
  for (int j = 0; j < pkQuery->byNum; ++j)
  {
    if (!pkQuery->List[j].byProcRet)
    {
      qry.List[outCount].byProcRet = 0;
      qry.List[outCount].dwRegistSerial = pkQuery->List[j].dwRegistSerial;
      qry.List[outCount].byOldState = pkQuery->List[j].byOldState;
      ++outCount;
    }
  }
  qry.byNum = outCount;

  g_Main.PushDQSData(-1, nullptr, 66, reinterpret_cast<char *>(&qry), sizeof(qry));
}

