#include "pch.h"

#include "CUnmannedTraderUserInfo.h"
#include "CNetworkEX.h"
#include "CNetProcess.h"
#include "CPlayer.h"
#include "CLogFile.h"
#include "CMoneySupplyMgr.h"
#include "CRecordData.h"
#include "CHolyStoneSystem.h"
#include "CHonorGuild.h"
#include "CUnmannedTraderGroupItemInfoTable.h"
#include "CUnmannedTraderItemState.h"
#include "CUnmannedTraderTaxRateManager.h"
#include "GlobalObjects.h"
#include "InvenKey.h"
#include "WorldServerUtil.h"
#include "a_trade_adjust_price_result_zocl.h"
#include "a_trade_clear_item_result_zocl.h"
#include "qry_case_unmandtrader_log_in_proc_update_complete.h"
#include "unmannedtrader_Regist_item_inform_zocl.h"
#include "unmannedtrader_close_item_inform_zocl.h"
#include "unmannedtrader_continue_item_inform_zocl.h"
#include "unmannedtrader_re_regist_result_zocl.h"
#include "unmannedtrader_regist_item_success_result_zocl.h"

#include <algorithm>
#include <cstring>
#include <cstdio>
#include <ctime>

CUnmannedTraderUserInfo CUnmannedTraderUserInfo::ms_kNull;

CUnmannedTraderUserInfo::CUnmannedTraderUserInfo()
  : m_eState(LOG_IN_STATE::UTUI_NONE),
    m_wInx(static_cast<unsigned short>(-1)),
    m_dwUserSerial(0),
    m_byRegistCnt(0),
    m_byMaxRegistCnt(0),
    m_kRequestState(),
    m_vecRegistItemInfo(),
    m_vecLoadItemInfo()
{
  Clear();
}

bool CUnmannedTraderUserInfo::Init(unsigned short wInx)
{
  this->m_wInx = wInx;

  CUnmannedTraderRegistItemInfo regInfo;
  this->m_vecRegistItemInfo.assign(10u, regInfo);
  if (this->m_vecRegistItemInfo.size() != 10u)
  {
    return false;
  }

  CUnmannedTraderRegistItemInfo loadInfo;
  this->m_vecLoadItemInfo.assign(20u, loadInfo);
  if (this->m_vecLoadItemInfo.size() != 20u)
  {
    return false;
  }

  this->m_kRequestState.ClearRequset();
  this->m_eState = LOG_IN_STATE::UTUI_EMPTY;
  return true;
}

void CUnmannedTraderUserInfo::Clear()
{
  if (IsNull())
  {
    return;
  }

  for (CUnmannedTraderRegistItemInfo &item : this->m_vecRegistItemInfo)
  {
    item.Clear();
  }

  ClearLoadItemInfo();
  this->m_dwUserSerial = 0;
  this->m_byRegistCnt = 0;
}

void CUnmannedTraderUserInfo::ClearLoadItemInfo()
{
  for (CUnmannedTraderRegistItemInfo &item : this->m_vecLoadItemInfo)
  {
    item.Clear();
  }
}

bool CUnmannedTraderUserInfo::IsNull()
{
  return this == &CUnmannedTraderUserInfo::ms_kNull;
}

void CUnmannedTraderUserInfo::ClearRequest()
{
  this->m_kRequestState.ClearRequset();
}

CPlayer *CUnmannedTraderUserInfo::FindOwner()
{
  CPlayer *player = nullptr;
  if (g_Player[this->m_wInx].m_id.dwSerial == this->m_dwUserSerial && g_Player[this->m_wInx].m_bLive)
  {
    player = &g_Player[this->m_wInx];
  }
  else
  {
    CPlayer *candidate = GetPtrPlayerFromSerial(g_Player, MAX_PLAYER, this->m_dwUserSerial);
    if (candidate && candidate->m_bOper)
    {
      player = candidate;
    }
  }

  return player ? player : nullptr;
}

unsigned __int16 CUnmannedTraderUserInfo::GetIndex()
{
  return this->m_wInx;
}

bool CUnmannedTraderUserInfo::IsLogInState()
{
  return this->m_eState == LOG_IN_STATE::UTUI_LOGIN;
}

unsigned int CUnmannedTraderUserInfo::GetSerial()
{
  return this->m_dwUserSerial;
}

unsigned __int8 CUnmannedTraderUserInfo::GetMaxRegistCnt()
{
  return this->m_byMaxRegistCnt;
}

const CUnmannedTraderRegistItemInfo *CUnmannedTraderUserInfo::GetRegItemInfo()
{
  if (IsNull() || this->m_vecRegistItemInfo.empty())
  {
    return nullptr;
  }

  return &this->m_vecRegistItemInfo[0];
}

void CUnmannedTraderUserInfo::SendSearchErrorResult(unsigned __int16 wInx, char byRet)
{
  char msg[532];
  msg[0] = byRet;

  unsigned __int8 pbyType[2]{};
  pbyType[0] = 30;
  pbyType[1] = 33;
  g_Network.m_pProcess[0]->LoadSendMsg(wInx, pbyType, msg, 0x1F3u);
}

void CUnmannedTraderUserInfo::LogOut(unsigned int dwSerial, CLogFile *pkLogger)
{
  if (this->m_dwUserSerial != dwSerial && pkLogger)
  {
    pkLogger->Write(
      "CUnmannedTraderUserInfo::LogOut( DWORD dwSerial )\r\n\t\tm_wInx(%u) m_dwSerial(%u) != dwSerial(%u)\r\n",
      this->m_wInx,
      this->m_dwUserSerial,
      dwSerial);
  }
  Clear();
  this->m_eState = LOG_IN_STATE::UTUI_EMPTY;
}

void CUnmannedTraderUserInfo::SendSearchResult(unsigned __int16 wInx, char *pLoadData)
{
  char *reqData = pLoadData;
  char msg[0x1F3];
  msg[0] = 0;

  const int totalCount = *reinterpret_cast<int *>(reqData + 12);
  std::memcpy(msg + 1, &totalCount, sizeof(totalCount));
  msg[5] = reqData[16];
  msg[6] = reqData[17];
  msg[7] = reqData[18];
  msg[8] = reqData[19];
  const int reserved = *reinterpret_cast<int *>(reqData + 20);
  std::memcpy(msg + 9, &reserved, sizeof(reserved));
  msg[13] = reqData[24];
  const int pageCount = *reinterpret_cast<int *>(reqData + 156);
  std::memcpy(msg + 14, &pageCount, sizeof(pageCount));
  const unsigned __int8 itemCount = static_cast<unsigned __int8>(reqData[160]);
  msg[18] = itemCount;

  __time64_t now{};
  time_18(reinterpret_cast<__int64 *>(&now));

  _INVENKEY key;
  for (unsigned int index = 0; index < itemCount; ++index)
  {
    const unsigned int dataOffset = 72 * index;
    char *item = msg + 19 + 48 * index;

    *reinterpret_cast<unsigned int *>(item) = *reinterpret_cast<unsigned int *>(reqData + dataOffset + 168);
    key.LoadDBKey(*reinterpret_cast<int *>(reqData + dataOffset + 172));
    item[4] = static_cast<char>(key.byTableCode);
    *reinterpret_cast<unsigned __int16 *>(item + 5) = key.wItemIndex;
    *reinterpret_cast<unsigned __int64 *>(item + 7) =
      *reinterpret_cast<unsigned __int64 *>(reqData + dataOffset + 176);
    *reinterpret_cast<unsigned int *>(item + 15) = *reinterpret_cast<unsigned int *>(reqData + dataOffset + 184);
    *reinterpret_cast<unsigned int *>(item + 19) = *reinterpret_cast<unsigned int *>(reqData + dataOffset + 188);
    *reinterpret_cast<unsigned int *>(item + 23) = *reinterpret_cast<unsigned int *>(reqData + dataOffset + 192);

    const char *source = reqData[dataOffset + 196] ? reqData + dataOffset + 196 : "NULL";
    strcpy_s(item + 27, 0x11u, source);

    const __int64 regTime =
      *reinterpret_cast<__int64 *>(reqData + dataOffset + 216) +
      3600 * static_cast<unsigned __int8>(reqData[dataOffset + 224]);
    *reinterpret_cast<unsigned int *>(item + 44) = static_cast<unsigned int>(regTime - now);
  }

  unsigned __int8 pbyType[2]{};
  pbyType[0] = 30;
  pbyType[1] = 33;
  g_Network.m_pProcess[0]->LoadSendMsg(wInx, pbyType, msg, 0x1F3u);
}

bool CUnmannedTraderUserInfo::operator==(unsigned int dwSerial)
{
  return this->m_dwUserSerial && this->m_dwUserSerial == dwSerial;
}

CUnmannedTraderUserInfo &CUnmannedTraderUserInfo::operator=(const CUnmannedTraderUserInfo &rhs)
{
  this->m_eState = rhs.m_eState;
  this->m_wInx = rhs.m_wInx;
  this->m_dwUserSerial = rhs.m_dwUserSerial;
  this->m_byRegistCnt = rhs.m_byRegistCnt;
  this->m_byMaxRegistCnt = rhs.m_byMaxRegistCnt;
  this->m_kRequestState.m_eState = rhs.m_kRequestState.m_eState;
  this->m_vecRegistItemInfo = rhs.m_vecRegistItemInfo;
  this->m_vecLoadItemInfo = rhs.m_vecLoadItemInfo;
  return *this;
}

std::vector<CUnmannedTraderRegistItemInfo>::iterator CUnmannedTraderUserInfo::Find(unsigned int dwRegistSerial)
{
  const size_t maxCount =
    std::min<size_t>(this->m_byMaxRegistCnt, this->m_vecRegistItemInfo.size());
  for (size_t index = 0; index < maxCount; ++index)
  {
    if (this->m_vecRegistItemInfo[index].GetRegistSerial() == dwRegistSerial)
    {
      return this->m_vecRegistItemInfo.begin() + static_cast<std::ptrdiff_t>(index);
    }
  }
  return this->m_vecRegistItemInfo.end();
}

std::vector<CUnmannedTraderRegistItemInfo>::iterator CUnmannedTraderUserInfo::FindEmpty()
{
  const size_t maxCount =
    std::min<size_t>(this->m_byMaxRegistCnt, this->m_vecRegistItemInfo.size());
  for (size_t index = 0; index < maxCount; ++index)
  {
    if (this->m_vecRegistItemInfo[index].IsEmpty())
    {
      return this->m_vecRegistItemInfo.begin() + static_cast<std::ptrdiff_t>(index);
    }
  }
  return this->m_vecRegistItemInfo.end();
}

void CUnmannedTraderUserInfo::CountRegistItem()
{
  this->m_byRegistCnt = 0;
  const size_t maxCount =
    std::min<size_t>(this->m_byMaxRegistCnt, this->m_vecRegistItemInfo.size());
  for (size_t index = 0; index < maxCount; ++index)
  {
    if (this->m_vecRegistItemInfo[index].IsRegist())
    {
      ++this->m_byRegistCnt;
    }
  }
}

void CUnmannedTraderUserInfo::GetCurrentRegItemStateStr(char *szStateStr, int iBuffSize)
{
  char szTran[0x400]{};
  char buffer[0x400]{};
  int index = 0;

  for (auto it = this->m_vecRegistItemInfo.begin(); it != this->m_vecRegistItemInfo.end(); ++it, ++index)
  {
    const CUnmannedTraderItemState::STATE state = it->GetState();
    wchar_t *stateStrW = CUnmannedTraderItemState::GetStateStrW(static_cast<unsigned int>(state));
    W2M(reinterpret_cast<char *>(stateStrW), szTran, 0x400u);
    sprintf_s(buffer, 0x400u, "%d:(%d)%s ", index, static_cast<int>(state), szTran);
    strcat_s(szStateStr, iBuffSize, buffer);
  }
}

CUnmannedTraderItemState::STATE CUnmannedTraderUserInfo::GetCloseItemForPassTimeUpdateInfo(
  unsigned int dwRegistSerial,
  CPlayer **pkOwner)
{
  if (IsNull())
  {
    return static_cast<CUnmannedTraderItemState::STATE>(6);
  }

  auto it = Find(dwRegistSerial);
  if (it == this->m_vecRegistItemInfo.end())
  {
    return static_cast<CUnmannedTraderItemState::STATE>(-1);
  }

  *pkOwner = FindOwner();
  if (!*pkOwner)
  {
    return static_cast<CUnmannedTraderItemState::STATE>(6);
  }

  if (it->IsRegist())
  {
    if (it->IsOverRegistTime())
    {
      return static_cast<CUnmannedTraderItemState::STATE>(13);
    }
    return static_cast<CUnmannedTraderItemState::STATE>(-1);
  }

  return static_cast<CUnmannedTraderItemState::STATE>(-1);
}

bool CUnmannedTraderUserInfo::CompleteUpdateState(
  unsigned int dwRegistSerial,
  unsigned __int8 byState,
  bool bReCountRegist)
{
  auto it = Find(dwRegistSerial);
  if (it == this->m_vecRegistItemInfo.end())
  {
    return false;
  }

  if (it->SetState(byState))
  {
    if (bReCountRegist)
    {
      CountRegistItem();
    }
    return true;
  }

  return false;
}

void CUnmannedTraderUserInfo::CompleteRegist(unsigned __int8 byRet, char *pLoadData, CLogFile *pkLogger)
{
  char *query = pLoadData;
  const unsigned __int16 sepaSerial = *reinterpret_cast<unsigned __int16 *>(query + 8);
  const unsigned __int16 itemSerial =
    sepaSerial == 0xFFFF ? *reinterpret_cast<unsigned __int16 *>(query + 2) : sepaSerial;
  const unsigned int registSerial = *reinterpret_cast<unsigned int *>(query + 20);
  const unsigned int price = *reinterpret_cast<unsigned int *>(query + 32);
  const unsigned int taxDalant = *reinterpret_cast<unsigned int *>(query + 80);
  const unsigned int ownerSerial = *reinterpret_cast<unsigned int *>(query + 28);
  const unsigned int dbKey = *reinterpret_cast<unsigned int *>(query + 40);
  const unsigned __int64 dbDur = *reinterpret_cast<unsigned __int64 *>(query + 48);
  const unsigned int dbLv = *reinterpret_cast<unsigned int *>(query + 56);
  const unsigned __int8 sellTurm = static_cast<unsigned __int8>(query[26]);
  const unsigned __int8 storageIndex = static_cast<unsigned __int8>(query[17]);
  const bool inserted = query[24] != 0;
  const unsigned __int8 byType = static_cast<unsigned __int8>(query[25]);
  const unsigned __int8 class1 = static_cast<unsigned __int8>(query[62]);
  const unsigned __int8 class2 = static_cast<unsigned __int8>(query[63]);

  CPlayer *owner = FindOwner();
  if (!owner || !owner->m_bOper)
  {
    return;
  }

  _STORAGE_LIST::_storage_con *regItem = owner->m_Param.m_dbInven.GetPtrFromSerial(itemSerial);
  if (!regItem)
  {
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteRegist( BYTE byRet, char * pLoadData, CLogFile * pkLogger )\r\n"
        "\t\tpkRegister->m_Param.m_dbInven.GetPtrFromSerial( wItemSerial(%u) ) NULL!\r\n",
        itemSerial);
    }
    return;
  }

  if (byRet)
  {
    if (sepaSerial != 0xFFFF)
    {
      owner->Emb_AlterDurPoint(
        0,
        static_cast<unsigned __int8>(query[16]),
        static_cast<int>(static_cast<unsigned __int8>(query[10])),
        false,
        false);
      owner->Emb_DelStorage(
        0,
        static_cast<unsigned __int8>(query[17]),
        false,
        false,
        "CUnmannedTraderUserInfo::CompleteRegist()");
    }

    regItem->lock(false);
    owner->AddDalant(static_cast<int>(taxDalant), true);
    const unsigned int leftDalant = owner->m_Param.GetDalant();
    SendRegistItemErrorResult(owner->m_ObjID.m_wIndex, 0x18u, itemSerial, leftDalant);
    return;
  }

  _INVENKEY invenKey;
  invenKey.LoadDBKey(dbKey);
  if (CompleteRegistItem(
        registSerial,
        itemSerial,
        regItem->m_dwETSerialNumber,
        price,
        sellTurm,
        invenKey.byTableCode,
        invenKey.wItemIndex,
        storageIndex,
        dbDur,
        dbLv,
        inserted))
  {
    const unsigned int leftDalant = owner->m_Param.GetDalant();
    CPlayer::s_MgrItemHistory.reg_auto_trade(
      owner->m_ObjID.m_wIndex,
      registSerial,
      reinterpret_cast<_STORAGE_LIST::_db_con *>(regItem),
      price,
      taxDalant,
      leftDalant,
      owner->m_szItemHistoryFileName);

    SendRegistItemSuccessResult(owner->m_Param.GetDalant(), owner->m_ObjID.m_wIndex, pLoadData);

    if (g_HolySys.GetHolyMasterRace() == -1)
    {
      const unsigned int totalTax = 75 * taxDalant / 0x64u;
      const unsigned int halfTax = totalTax / 2;
      const int race = owner->m_Param.GetRaceCode();
      CUnmannedTraderTaxRateManager::Instance()->SetPatriarchTaxMoney(race, halfTax);
      CHonorGuild::Instance()->SetGuildMaintainMoney(race, totalTax, owner->m_dwObjSerial);
    }
    else
    {
      const int ownerRace = owner->m_Param.GetRaceCode();
      const int holyMasterRace = g_HolySys.GetHolyMasterRace();
      unsigned int totalTax = 0;
      if (ownerRace == holyMasterRace)
      {
        totalTax = 75 * taxDalant / 0x64u;
      }
      else
      {
        totalTax = 50 * taxDalant / 0x64u;
      }
      const unsigned int halfTax = totalTax / 2;
      CUnmannedTraderTaxRateManager::Instance()->SetPatriarchTaxMoney(holyMasterRace, halfTax);
      CHonorGuild::Instance()->SetGuildMaintainMoney(holyMasterRace, totalTax, owner->m_dwObjSerial);
    }

    if (!CUnmannedTraderGroupItemInfoTable::Instance()->IncreaseVersion(
          invenKey.byTableCode,
          invenKey.wItemIndex,
          class1,
          class2))
    {
      const char *ownerName = owner->m_Param.GetCharNameA();
      if (pkLogger)
      {
        pkLogger->Write(
          "CUnmannedTraderController::ComleteRegistItem( BYTE byRet(%u), char * pLoadData )\r\n"
          "\t\tOwner : (%u)(%u)%s\r\n"
          "\t\tCUnmannedTraderGroupItemInfoTable::Instance()->IncreaseVersion(\r\n"
          "\t\tkInvenKey.byTableCode(%u), kInvenKey.wItemIndex(%u), pkQuery->byClass1(%u), pkQuery->byClass2(%u) ) Fail!\r\n",
          0,
          this->m_dwUserSerial,
          owner->m_dwObjSerial,
          ownerName,
          invenKey.byTableCode,
          invenKey.wItemIndex,
          class1,
          class2);
      }
    }
    return;
  }

  char stateStr[2048]{};
  GetCurrentRegItemStateStr(stateStr, 2048);
  const char *ownerName = owner->m_Param.GetCharNameA();
  if (pkLogger)
  {
    pkLogger->Write(
      "CUnmannedTraderUserInfo::ComleteRegistItem( BYTE byRet(%u), char * pLoadData )\r\n"
      "\t\tOwner : (%u)(%u)%s\r\n"
      "\t\tCurrent Reg Item State %s\r\n"
      "\t\tCompleteRegistItem( pkQuery->dwRegedSerial(%u), wItemSerial(%u), pRegItem->m_dwETSerialNumber(%u), pkQuery->dwPrice(%u), pkQuery->dwTax(%u)\r\n"
      "\t\t, pkQuery->bySellTurm(%u), kInvenKey.byTableCode(%u), kInvenKey.wItemIndex(%u)\r\n"
      "\t\t, pkQuery->bInserted, pkLogger ) Fail!\r\n",
      0,
      this->m_dwUserSerial,
      owner->m_dwObjSerial,
      ownerName,
      stateStr,
      registSerial,
      itemSerial,
      regItem->m_dwETSerialNumber,
      price,
      taxDalant,
      sellTurm,
      invenKey.byTableCode,
      invenKey.wItemIndex);
  }

  if (sepaSerial != 0xFFFF)
  {
    owner->Emb_AlterDurPoint(
      0,
      static_cast<unsigned __int8>(query[16]),
      static_cast<int>(static_cast<unsigned __int8>(query[10])),
      false,
      false);
    owner->Emb_DelStorage(
      0,
      static_cast<unsigned __int8>(query[17]),
      false,
      false,
      "CUnmannedTraderUserInfo::CompleteRegist()");
  }
  regItem->lock(false);
  owner->AddDalant(static_cast<int>(taxDalant), true);
  const unsigned int leftDalant = owner->m_Param.GetDalant();
  SendRegistItemErrorResult(owner->m_ObjID.m_wIndex, 6u, itemSerial, leftDalant);
  CUnmannedTraderItemState::PushUpdateState(
    byType,
    registSerial,
    0xCu,
    ownerSerial,
    itemSerial,
    0xFFu,
    0xFFFFu);
}

bool CUnmannedTraderUserInfo::CompleteRegistItem(
  unsigned int dwRegistSerial,
  unsigned __int16 dwItemSerial,
  unsigned int dwETSerialNumber,
  unsigned int dwPrice,
  unsigned __int8 bySellTurm,
  unsigned __int8 byTableCode,
  unsigned __int16 wItemIndex,
  unsigned __int8 byStorageIndex,
  unsigned __int64 dwD,
  unsigned int dwU,
  bool bInserted)
{
  auto it = FindEmpty();
  if (it == this->m_vecRegistItemInfo.end())
  {
    return false;
  }

  it->RegistItem(
    dwRegistSerial,
    dwItemSerial,
    dwETSerialNumber,
    dwPrice,
    bySellTurm,
    byTableCode,
    wItemIndex,
    byStorageIndex,
    dwD,
    dwU,
    bInserted);
  CountRegistItem();
  return true;
}

void CUnmannedTraderUserInfo::CompleteCancelRegist(unsigned __int8 byRet, char *pLoadData, CLogFile *pkLogger)
{
  char *query = pLoadData;
  CPlayer *owner = FindOwner();
  if (!owner || !owner->m_bLive)
  {
    return;
  }

  const unsigned __int8 procRet = static_cast<unsigned __int8>(query[45]);
  if (byRet || procRet)
  {
    SendCancelRegistErrorResult(owner->m_ObjID.m_wIndex, procRet);
    return;
  }

  const unsigned __int16 itemSerial = *reinterpret_cast<unsigned __int16 *>(query + 2);
  _STORAGE_LIST::_storage_con *regItem = owner->m_Param.m_dbInven.GetPtrFromSerial(itemSerial);
  if (!regItem)
  {
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteCancelRegist( BYTE byRet, char * pLoadData, CLogFile * pkLogger )\r\n"
        "\t\tpkQuery->byType(%u), pkQuery->dwRegistSerial(%u)\r\n"
        "\t\tpkRegister->m_Param.m_dbInven.GetPtrFromSerial( wItemSerial(%u) ) NULL!\r\n",
        static_cast<unsigned __int8>(query[38]),
        *reinterpret_cast<unsigned int *>(query + 40),
        itemSerial);
    }
    SendCancelRegistErrorResult(owner->m_ObjID.m_wIndex, 8u);
    return;
  }

  regItem->lock(false);
  const unsigned int registSerial = *reinterpret_cast<unsigned int *>(query + 40);
  if (CompleteCancelRegistItem(registSerial, itemSerial, pkLogger))
  {
    if (!CUnmannedTraderGroupItemInfoTable::Instance()->IncreaseVersion(
          regItem->m_byTableCode,
          regItem->m_wItemIndex))
    {
      if (pkLogger)
      {
        pkLogger->Write(
          "CUnmannedTraderUserInfo::CompleteCancelRegist( BYTE byRet, char * pLoadData, CLogFile * pkLogger )\r\n"
          "\t\tCUnmannedTraderGroupItemInfoTable::Instance()->IncreaseVersion( pRegItem->m_byTableCode(%u), pRegItem->m_wItemIndex(%u) )\r\n",
          regItem->m_byTableCode,
          regItem->m_wItemIndex);
      }
    }

    SendCancelRegistSuccessResult(owner->m_ObjID.m_wIndex, itemSerial, registSerial);
    CPlayer::s_MgrItemHistory.self_cancel_auto_trade(
      owner->m_ObjID.m_wIndex,
      registSerial,
      reinterpret_cast<_STORAGE_LIST::_db_con *>(regItem),
      owner->m_szItemHistoryFileName);
  }
  else
  {
    SendCancelRegistErrorResult(owner->m_ObjID.m_wIndex, 0x1Du);
  }
}

bool CUnmannedTraderUserInfo::CompleteCancelRegistItem(
  unsigned int dwRegistSerial,
  unsigned __int16 dwItemSerial,
  CLogFile *pkLogger)
{
  auto it = Find(dwRegistSerial);
  if (it == this->m_vecRegistItemInfo.end())
  {
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteCancelRegistItem( DWORD dwRegistSerial(%u), WORD dwItemSerial(%u) )\r\n"
        "\t\t Find( dwRegistSerial(%u) ) NULL!\r\n",
        dwRegistSerial,
        dwItemSerial,
        dwRegistSerial);
    }
    return false;
  }

  if (it->GetItemSerial() != dwItemSerial)
  {
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteCancelRegistItem( DWORD dwRegistSerial(%u), WORD dwItemSerial(%u) )\r\n"
        "\t\t (*iFind).GetItemSerial()(%u) != dwItemSerial(%u) Invalid!\r\n",
        dwRegistSerial,
        dwItemSerial,
        it->GetItemSerial(),
        dwRegistSerial);
    }
  }

  it->ClearRegist();
  CountRegistItem();
  return true;
}

void CUnmannedTraderUserInfo::CompleteTimeOutClear(unsigned int dwRegistSerial, CLogFile *pkLogger)
{
  auto it = Find(dwRegistSerial);
  if (it == this->m_vecRegistItemInfo.end())
  {
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteTimeOutClear(...)\r\n\t\tFind( dwRegistSerial(%u) ) Item Fail!\r\n",
        dwRegistSerial);
    }
    return;
  }

  const unsigned __int16 itemSerial = it->GetItemSerial();
  it->SetState(0xDu);
  CountRegistItem();

  CPlayer *owner = FindOwner();
  if (owner && owner->m_bOper)
  {
    _STORAGE_LIST::_db_con *regItem = owner->m_Param.m_dbInven.GetPtrFromSerial(itemSerial);
    if (regItem)
    {
      CPlayer::s_MgrItemHistory.time_out_cancel_auto_trade(
        owner->m_ObjID.m_wIndex,
        dwRegistSerial,
        regItem,
        owner->m_szItemHistoryFileName);

      const int race = owner->m_Param.GetRaceCode();
      const unsigned __int8 taxRate =
        static_cast<unsigned __int8>(CUnmannedTraderTaxRateManager::Instance()->GetTaxRate(race) * 100.0f);
      const unsigned int price = it->GetPrice();
      SendNotifyCloseItem(owner->m_ObjID.m_wIndex, itemSerial, dwRegistSerial, price, taxRate);
    }
    else
    {
      const char *ownerName = owner->m_Param.GetCharNameA();
      if (pkLogger)
      {
        pkLogger->Write(
          "CUnmannedTraderUserInfo::CompleteTimeOutClear(...)\r\n"
          "\t\tOnwer : Serial(%u) Name(%s)\r\n"
          "\t\tdwRegistSerial(%u)\r\n"
          "\t\tpkOwner->m_Param.m_dbInven.GetPtrFromSerial( (*i).GetItemSerial(%u) ) NULL!\r\n",
          owner->m_dwObjSerial,
          ownerName,
          dwRegistSerial,
          itemSerial);
      }
      NotifyRegistItem();
    }
    return;
  }

  const int ownerOper = owner ? owner->m_bOper : -1;
  if (pkLogger)
  {
    pkLogger->Write(
      "CUnmannedTraderUserInfo::CompleteTimeOutClear( dwRegistSerial(%u) )\r\n"
      "\t\t( 0 == pkOwner || !pkOwner->m_bOper(%d) )\r\n",
      dwRegistSerial,
      ownerOper);
  }
}

void CUnmannedTraderUserInfo::CompleteReprice(unsigned __int8 byRet, char *pLoadData, CLogFile *pkLogger)
{
  char *query = pLoadData;
  CPlayer *receiver = FindOwner();
  if (receiver && receiver->m_bLive)
  {
    const unsigned __int8 procRet = static_cast<unsigned __int8>(query[20]);
    if (byRet || procRet)
    {
      SendRepriceErrorResult(receiver, procRet);
      receiver->AddDalant(static_cast<int>(*reinterpret_cast<unsigned int *>(query + 16)), true);
      return;
    }

    const unsigned __int16 itemSerial = *reinterpret_cast<unsigned __int16 *>(query + 12);
    _STORAGE_LIST::_db_con *regItem = receiver->m_Param.m_dbInven.GetPtrFromSerial(itemSerial);
    if (regItem)
    {
      const unsigned int registSerial = *reinterpret_cast<unsigned int *>(query + 24);
      const unsigned int newPrice = *reinterpret_cast<unsigned int *>(query + 28);
      if (CompleteRepriceItem(registSerial, itemSerial, newPrice))
      {
        if (!CUnmannedTraderGroupItemInfoTable::Instance()->IncreaseVersion(
              regItem->m_byTableCode,
              regItem->m_wItemIndex))
        {
          if (pkLogger)
          {
            pkLogger->Write(
              "CUnmannedTraderUserInfo::CompleteReprice( BYTE byRet(%u), char * pLoadData, CLogFile * pkLogger )\r\n"
              "\t\tpkQuery->dwRegistSerial(%u), pkQuery->dwNewPrice(%u)\r\n"
              "\t\tCUnmannedTraderGroupItemInfoTable::Instance()->IncreaseVersion( pRegItem->m_byTableCode(%u), pRegItem->m_wItemIndex(%u) )\r\n",
              0,
              registSerial,
              newPrice,
              regItem->m_byTableCode,
              regItem->m_wItemIndex);
          }
        }

        SendRepriceSuccessResult(
          receiver,
          itemSerial,
          newPrice,
          registSerial,
          *reinterpret_cast<unsigned int *>(query + 16));
        CPlayer::s_MgrItemHistory.price_auto_trade(
          receiver->m_ObjID.m_wIndex,
          registSerial,
          regItem,
          *reinterpret_cast<unsigned int *>(query + 16),
          *reinterpret_cast<unsigned int *>(query),
          newPrice,
          receiver->m_szItemHistoryFileName);
      }
      else
      {
        if (pkLogger)
        {
          pkLogger->Write(
            "CUnmannedTraderUserInfo::CompleteReprice( BYTE byRet, char * pLoadData, CLogFile * pkLogger )\r\n"
            "\t\tCompleteRepriceItem( pkQuery->dwRegistSerial(%u), pkQuery->wItemSerial(%u), pkQuery->dwNewPrice(%u), pkLogger ) Fail!\r\n",
            registSerial,
            itemSerial,
            newPrice);
        }
        SendRepriceErrorResult(receiver, 0x1Cu);
      }
    }
    else
    {
      if (pkLogger)
      {
        pkLogger->Write(
          "CCUnmannedTraderUserInfo::CompleteReprice( BYTE byRet, char * pLoadData, CLogFile * pkLogger )\r\n"
          "\t\tpkQuery->dwRegistSerial(%u), pkQuery->dwNewPrice(%u)\r\n"
          "\t\tpkRegister->m_Param.m_dbInven.GetPtrFromSerial( wItemSerial(%u) ) NULL!\r\n",
          *reinterpret_cast<unsigned int *>(query + 24),
          *reinterpret_cast<unsigned int *>(query + 28),
          itemSerial);
      }
      SendRepriceErrorResult(receiver, 8u);
    }
    return;
  }

  if (byRet || query[20])
  {
    g_Main.Push_ChargeItem(
      *reinterpret_cast<unsigned int *>(query + 8),
      0xFFFFFFFF,
      *reinterpret_cast<unsigned int *>(query + 16),
      0xFFFFFFFu,
      1u);
  }
}

bool CUnmannedTraderUserInfo::CompleteRepriceItem(
  unsigned int dwRegistSerial,
  unsigned __int16 dwItemSerial,
  unsigned int dwPrice)
{
  auto it = Find(dwRegistSerial);
  if (it == this->m_vecRegistItemInfo.end() || it->GetItemSerial() != dwItemSerial)
  {
    return false;
  }

  it->RepriceItem(dwPrice);
  return true;
}

void CUnmannedTraderUserInfo::CompleteReRegist(char *pLoadData, CLogFile *pkLogger)
{
  CPlayer *owner = FindOwner();
  if (!owner)
  {
    return;
  }

  unsigned char *query = reinterpret_cast<unsigned char *>(pLoadData);
  _qry_case_unmandtrader_log_in_proc_update_complete updateComplete{};
  std::memset(&updateComplete, 0, sizeof(updateComplete));
  updateComplete.byType = query[0];
  updateComplete.wInx = this->m_wInx;
  updateComplete.dwSeller = this->m_dwUserSerial;

  _unmannedtrader_re_regist_result_zocl result{};
  result.byNum = query[4];

  for (unsigned __int8 j = 0; j < static_cast<unsigned __int8>(query[4]); ++j)
  {
    const size_t entryOffset = 28u * j + 12u;
    const unsigned __int8 procRet = query[entryOffset];
    const unsigned __int8 registFlag = query[entryOffset + 1];
    const unsigned __int16 itemSerial = *reinterpret_cast<unsigned __int16 *>(query + entryOffset + 2);
    const unsigned int taxDalant = *reinterpret_cast<unsigned int *>(query + entryOffset + 4);
    const unsigned int listIndex = *reinterpret_cast<unsigned int *>(query + entryOffset + 8);
    const unsigned int price = *reinterpret_cast<unsigned int *>(query + entryOffset + 16);
    const unsigned int registSerial = *reinterpret_cast<unsigned int *>(query + entryOffset + 20);

    result.List[j].byRet = procRet;
    result.List[j].bRegist = registFlag;
    result.List[j].wItemSerial = itemSerial;
    result.List[j].dwPrice = price;
    result.List[j].dwRegedSerial = registSerial;
    result.List[j].dwListIndex = listIndex;
    result.List[j].dwTax = taxDalant;

    _STORAGE_LIST::_storage_con *regItem = owner->m_Param.m_dbInven.GetPtrFromSerial(itemSerial);
    if (regItem)
    {
      if (registFlag)
      {
        if (procRet)
        {
          if (procRet == 202 || procRet == 24)
          {
            owner->AddDalant(static_cast<int>(taxDalant), true);
          }
          if (CompleteCancelRegistItem(registSerial, itemSerial, pkLogger))
          {
            regItem->lock(false);
          }
          else if (pkLogger)
          {
            pkLogger->Write(
              "CUnmannedTraderUserInfo::CompleteReRegist(...) : if( pkQuery->List[i].bRegist && 0 != pkQuery->List[i].byProcRet(%u) )"
              "CompleteCancelRegistItem( pkQuery->List[%u].dwRegedSerial(%u), pkQuery->List[%u].wItemSerial(%u) Fail!",
              procRet,
              j,
              registSerial,
              itemSerial);
          }
        }
        else if (CompleteReRegistItem(registSerial, itemSerial, price, pkLogger, &query[entryOffset]))
        {
          const unsigned int leftDalant = owner->m_Param.GetDalant();
          CPlayer::s_MgrItemHistory.re_reg_auto_trade(
            owner->m_ObjID.m_wIndex,
            registSerial,
            reinterpret_cast<_STORAGE_LIST::_db_con *>(regItem),
            price,
            taxDalant,
            leftDalant,
            owner->m_szItemHistoryFileName);

          if (g_HolySys.GetHolyMasterRace() == -1)
          {
            const unsigned int totalTax = 75 * taxDalant / 0x64u;
            const unsigned int halfTax = totalTax / 2;
            const int race = owner->m_Param.GetRaceCode();
            CUnmannedTraderTaxRateManager::Instance()->SetPatriarchTaxMoney(race, halfTax);
            CHonorGuild::Instance()->SetGuildMaintainMoney(race, totalTax, owner->m_dwObjSerial);
          }
          else
          {
            const int ownerRace = owner->m_Param.GetRaceCode();
            const int holyMasterRace = g_HolySys.GetHolyMasterRace();
            unsigned int totalTax = 0;
            if (ownerRace == holyMasterRace)
            {
              totalTax = 75 * taxDalant / 0x64u;
            }
            else
            {
              totalTax = 50 * taxDalant / 0x64u;
            }
            const unsigned int halfTax = totalTax / 2;
            CUnmannedTraderTaxRateManager::Instance()->SetPatriarchTaxMoney(holyMasterRace, halfTax);
            CHonorGuild::Instance()->SetGuildMaintainMoney(holyMasterRace, totalTax, owner->m_dwObjSerial);
          }
        }
        else
        {
          result.List[j].byRet = query[entryOffset];
          owner->AddDalant(static_cast<int>(taxDalant), true);
          updateComplete.List[updateComplete.byNum].byOldState = 12;
          updateComplete.List[updateComplete.byNum].dwRegistSerial = registSerial;
          ++updateComplete.byNum;
        }
      }
      else if (CompleteCancelRegistItem(registSerial, itemSerial, pkLogger))
      {
        regItem->lock(false);
        CPlayer::s_MgrItemHistory.self_cancel_auto_trade(
          owner->m_ObjID.m_wIndex,
          registSerial,
          reinterpret_cast<_STORAGE_LIST::_db_con *>(regItem),
          owner->m_szItemHistoryFileName);
      }
      else if (pkLogger)
      {
        pkLogger->Write(
          "CUnmannedTraderUserInfo::CompleteReRegist(...) : if( !pkQuery->List[i].bRegist )CompleteCancelRegistItem( pkQuery->List[%u].dwRegedSerial(%u), pkQuery->List[%u].wItemSerial(%u) Fail!",
          j,
          registSerial,
          itemSerial);
      }
    }
    else
    {
      query[entryOffset] = 25;
      result.List[j].byRet = 25;
      const char *ownerName = owner->m_Param.GetCharNameA();
      if (pkLogger)
      {
        pkLogger->Write(
          "CUnmannedTraderUserInfo::CompleteReRegist(...)\r\n"
          "\t\tOnwer : Serial(%u) Name(%s)\r\n"
          "\t\tdwRegistSerial(%u)\r\n"
          "\t\tpkRegister->m_Param.m_dbInven.GetPtrFromSerial( wItemSerial(%u) ) NULL!\r\n",
          owner->m_dwObjSerial,
          ownerName,
          registSerial,
          itemSerial);
      }
      if (registFlag)
      {
        owner->AddDalant(static_cast<int>(taxDalant), true);
      }
      updateComplete.List[updateComplete.byNum].byOldState = 11;
      updateComplete.List[updateComplete.byNum].dwRegistSerial = registSerial;
      ++updateComplete.byNum;
    }
  }

  if (updateComplete.byNum)
  {
    g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x93u, reinterpret_cast<char *>(&updateComplete), 92);
  }

  result.dwLeftDalant = owner->m_Param.GetDalant();
  unsigned __int8 pbyType[16]{};
  pbyType[0] = 30;
  pbyType[1] = 37;
  const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_wInx, pbyType, reinterpret_cast<char *>(&result), len);
}

bool CUnmannedTraderUserInfo::CompleteReRegistItem(
  unsigned int dwRegistSerial,
  unsigned __int16 dwItemSerial,
  unsigned int dwPrice,
  CLogFile *pkLogger,
  unsigned __int8 *pbyProcRet)
{
  auto it = Find(dwRegistSerial);
  if (it == this->m_vecRegistItemInfo.end())
  {
    *pbyProcRet = 25;
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteReRegistItem(...)\r\n\t\t Find( dwRegistSerial(%u) ) Not Exist!\r\n",
        dwRegistSerial);
    }
    return false;
  }

  if (it->GetItemSerial() == dwItemSerial)
  {
    it->ReRegistItem(dwPrice);
    CountRegistItem();
    return true;
  }

  *pbyProcRet = 25;
  if (pkLogger)
  {
    pkLogger->Write(
      "CUnmannedTraderUserInfo::CompleteReRegistItem( DWORD dwRegistSerial(%u), WORD dwItemSerial(%u) )\r\n"
      "\t\t (*iFind).GetItemSerial()(%u) != dwItemSerial(%u) Invalid!\r\n",
      dwRegistSerial,
      it->GetItemSerial(),
      dwItemSerial,
      dwRegistSerial);
  }
  return false;
}

void CUnmannedTraderUserInfo::CompleteUpdateCheatRegistTime(char *pLoadData)
{
  unsigned char *data = reinterpret_cast<unsigned char *>(pLoadData);
  for (unsigned __int8 j = 0; j < static_cast<unsigned __int8>(data[1]); ++j)
  {
    if (!data[8 * j + 8])
    {
      auto it = Find(*reinterpret_cast<unsigned int *>(data + 8 * j + 12));
      if (it != this->m_vecRegistItemInfo.end())
      {
        it->SetOverRegistTime();
      }
    }
  }
}

void CUnmannedTraderUserInfo::CompleteReRegistRollBack(char *pData, CLogFile *pkLogger)
{
  if (pkLogger)
  {
    pkLogger->Write(
      "CUnmannedTraderUserInfo::CompleteReRegistRollBack( char * pLoadData, CLogFile * pkLogger )\r\n"
      "\t\tType(%u) wInx(%u) Owner(%u)\r\n",
      static_cast<unsigned __int8>(*pData),
      *reinterpret_cast<unsigned __int16 *>(pData + 2),
      *reinterpret_cast<unsigned int *>(pData + 4));
  }

  for (int j = 0; j < static_cast<unsigned __int8>(pData[1]); ++j)
  {
    if (!pData[8 * j + 12] &&
        !CompleteUpdateState(*reinterpret_cast<unsigned int *>(pData + 8 * j + 16), pData[8 * j + 13], false))
    {
      if (pkLogger)
      {
        pkLogger->Write(
          "\t\t(%d)Nth RegistSerial(%u) CompleteUpdateState(..) State(%u) Fail!\r\n",
          j,
          *reinterpret_cast<unsigned int *>(pData + 8 * j + 16),
          static_cast<unsigned __int8>(pData[8 * j + 12]));
      }
    }
    if (pkLogger)
    {
      pkLogger->Write(
        "\t\t(%d)Nth RegistSerial(%u) byProcRet(%u)\r\n",
        j,
        *reinterpret_cast<unsigned int *>(pData + 8 * j + 16),
        static_cast<unsigned __int8>(pData[8 * j + 12]));
    }
  }

  CountRegistItem();
}

unsigned __int8 CUnmannedTraderUserInfo::CheckSellComplete(
  CPlayer *pkSellPlayer,
  CPlayer *pkBuyer,
  unsigned int dwRegistSerial,
  unsigned int dwRealPrice,
  CLogFile *pkLogger)
{
  auto it = Find(dwRegistSerial);
  if (it == this->m_vecRegistItemInfo.end())
  {
    if (pkLogger)
    {
      const char *sellerName = pkSellPlayer ? pkSellPlayer->m_Param.GetCharNameA() : "NULL";
      pkLogger->Write(
        "CUnmannedTraderRegistItemInfo::CheckSellComplete(...)\r\n"
        "\t\terror_used_invaled_item_local_sellinfo_lack!\r\n"
        "\t\tName(%s) Serial(%u) Invalid DB Data!\r\n"
        "\t\tFind( dwRegistSeria(%u) ) NULL!\r\n",
        sellerName,
        pkSellPlayer ? pkSellPlayer->m_id.dwSerial : 0,
        dwRegistSerial);
    }
    return 37;
  }

  if (!pkSellPlayer || !pkBuyer || !pkLogger)
  {
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderRegistItemInfo::CheckSellComplete(...)\r\n"
        "\t\t( NULL == pkSellPlayer(%p) || NULL == pkBuyer(%p) || NULL == pkLogger(%p) )\r\n"
        "\t\tdwRegistSeria(%u) error_used_internal_error!\r\n",
        pkSellPlayer,
        pkBuyer,
        pkLogger,
        dwRegistSerial);
    }
    it->ClearToWaitState();
    if (g_Player[this->m_wInx].m_bOper)
    {
      NotifyRegistItem();
    }
    return 99;
  }

  const unsigned __int16 itemSerial = it->GetItemSerial();
  if (pkSellPlayer->m_Param.m_dbInven.GetPtrFromSerial(itemSerial))
  {
    const unsigned int dalant = pkSellPlayer->m_Param.GetDalant();
    if (CanAddMoneyForMaxLimMoney(dwRealPrice, dalant))
    {
      return 0;
    }
    return 34;
  }

  const char *sellerName = pkSellPlayer->m_Param.GetCharNameA();
  pkLogger->Write(
    "CUnmannedTraderRegistItemInfo::CheckSellComplete(...)\r\n"
    "\t\terror_used_invaled_item_local_sellinfo_lack!\r\n"
    "\t\tName(%s) Serial(%u) Invalid DB Data!\r\n"
    "\t\tdwRegistSerial(%u)\r\n"
    "\t\tpkPlayer->m_Param.m_dbInven.GetPtrFromSerial( m_wItemSerial(%u) ) == NULL!\r\n",
    sellerName,
    pkSellPlayer->m_id.dwSerial,
    dwRegistSerial,
    itemSerial);
  it->ClearToWaitState();
  if (g_Player[this->m_wInx].m_bOper)
  {
    NotifyRegistItem();
  }
  return 37;
}

unsigned __int8 CUnmannedTraderUserInfo::SellComplete(
  CPlayer *pkSellPlayer,
  CPlayer *pkBuyer,
  unsigned int dwOriPrice,
  unsigned int dwRealPrice,
  unsigned int dwTax,
  unsigned int dwRegistSerial,
  __int64 tResultTime,
  CLogFile *pkLogger)
{
  (void)pkLogger;
  auto it = Find(dwRegistSerial);
  const unsigned __int16 itemSerial = it->GetItemSerial();
  _STORAGE_LIST::_db_con *pItem = pkSellPlayer->m_Param.m_dbInven.GetPtrFromSerial(itemSerial);
  if (!pItem)
  {
    return 37;
  }

  if (!pkSellPlayer->Emb_DelStorage(
        0,
        pItem->m_byStorageIndex,
        false,
        true,
        "CUnmannedTraderUserInfo::SellComplete()"))
  {
    g_Main.m_logSystemError.Write(
      "CUnmannedTraderUserInfo::SellComplete() : User(%u) pkSellPlayer->Emb_DelStorage() Fail!",
      this->m_dwUserSerial);
    return static_cast<unsigned __int8>(-105);
  }

  pkSellPlayer->AddDalant(static_cast<int>(dwRealPrice), true);
  const char *buyerAccount = pkBuyer->m_pUserDB->m_szAccountID;
  const char *buyerNameW = pkBuyer->m_Param.GetCharNameW();
  it->SellComplete(dwOriPrice, pkBuyer->m_dwObjSerial, dwTax, tResultTime, buyerNameW, buyerAccount);
  CountRegistItem();

  const int level = pkSellPlayer->m_Param.GetLevel();
  if (level == 30 || level == 40 || level == 50 || level == 60)
  {
    CMoneySupplyMgr::Instance()->UpdateFeeMoneyData(pkSellPlayer->m_Param.GetRaceCode(), level, dwTax);
  }

  const unsigned int leftGold = pkSellPlayer->m_Param.GetGold();
  const unsigned int leftDalant = pkSellPlayer->m_Param.GetDalant();
  const char *buyerID = pkBuyer->m_pUserDB->m_szAccountID;
  const char *buyerNameA = pkBuyer->m_Param.GetCharNameA();
  CPlayer::s_MgrItemHistory.auto_trade_sell(
    buyerNameA,
    pkBuyer->m_dwObjSerial,
    buyerID,
    dwRegistSerial,
    pItem,
    dwRealPrice,
    dwTax,
    leftDalant,
    leftGold,
    pkSellPlayer->m_szItemHistoryFileName);

  tm *timeInfo = localtime_13(&tResultTime);
  char buffer[64]{};
  if (timeInfo)
  {
    sprintf_s(
      buffer,
      0x40u,
      "%04d-%02d-%02d %02d:%02d:%02d",
      timeInfo->tm_year + 1900,
      timeInfo->tm_mon + 1,
      timeInfo->tm_mday,
      timeInfo->tm_hour,
      timeInfo->tm_min,
      timeInfo->tm_sec);
  }
  else
  {
    sprintf_s(buffer, 0x40u, "Invalid(%u)", static_cast<unsigned int>(tResultTime));
  }

  _base_fld *record = g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
  const char *sellerNameA = pkSellPlayer->m_Param.GetCharNameA();
  const char *buyerNameLog = pkBuyer->m_Param.GetCharNameA();
  g_Main.m_logAutoTrade.Write(
    "%s %s >> sell (%u) user(%u)%s -> user(%u)%s price(%u) tax(%u)",
    record->m_strCode,
    buffer,
    dwRegistSerial,
    pkBuyer->m_dwObjSerial,
    buyerNameLog,
    pkSellPlayer->m_dwObjSerial,
    sellerNameA,
    dwOriPrice,
    dwTax);

  const unsigned int finalDalant = pkSellPlayer->m_Param.GetDalant();
  SendSellInfom(pkSellPlayer->m_ObjID.m_wIndex, pItem->m_wSerial, dwRealPrice, dwTax, finalDalant);
  return 90;
}

unsigned __int8 CUnmannedTraderUserInfo::CheckBuyComplete(CPlayer *pkBuyer, unsigned int dwPrice)
{
  if (dwPrice > pkBuyer->m_Param.GetDalant())
  {
    return 43;
  }
  if (pkBuyer->m_Param.m_dbInven.GetIndexEmptyCon() == 255)
  {
    return 42;
  }
  return 0;
}

unsigned __int8 CUnmannedTraderUserInfo::BuyComplete(
  CPlayer *pkBuyer,
  unsigned int dwSellerSerial,
  const char *wszSellerName,
  const char *szSellerAccountName,
  unsigned int dwRegistSerial,
  unsigned int dwK,
  unsigned __int64 dwD,
  unsigned int dwU,
  unsigned int dwPrice,
  unsigned __int64 lnUID,
  CLogFile *pkLogger,
  unsigned __int16 *wAddItemSerial)
{
  (void)pkLogger;
  _INVENKEY key;
  key.LoadDBKey(dwK);
  _STORAGE_LIST::_db_con item;
  std::memset(&item, 0, sizeof(item));
  item.m_byTableCode = key.byTableCode;
  item.m_wItemIndex = key.wItemIndex;
  item.m_dwDur = dwD;
  item.m_dwLv = dwU;
  item.m_wSerial = pkBuyer->m_Param.GetNewItemSerial();
  *wAddItemSerial = item.m_wSerial;
  item.m_lnUID = lnUID;

  if (pkBuyer->Emb_AddStorage(0, &item, false, true))
  {
    pkBuyer->SubDalant(dwPrice);
    char szTran[17]{};
    W2M(const_cast<char *>(wszSellerName), szTran, 0x11u);
    const unsigned int leftGold = pkBuyer->m_Param.GetGold();
    const unsigned int leftDalant = pkBuyer->m_Param.GetDalant();
    CPlayer::s_MgrItemHistory.auto_trade_buy(
      szTran,
      dwSellerSerial,
      szSellerAccountName,
      dwRegistSerial,
      &item,
      dwPrice,
      leftDalant,
      leftGold,
      pkBuyer->m_szItemHistoryFileName);
    return 0;
  }

  g_Main.m_logSystemError.Write(
    "CUnmannedTraderUserInfo::BuyComplete() : User(%u) pkSellPlayer->Emb_AddStorage() Fail!",
    this->m_dwUserSerial);
  return static_cast<unsigned __int8>(-106);
}

void CUnmannedTraderUserInfo::NotifyRegistItem()
{
  unsigned __int8 byDivision[32]{};
  unsigned __int8 byClass[32]{};
  unsigned __int8 bySubClass[44]{};
  byDivision[0] = static_cast<unsigned __int8>(-1);
  byClass[0] = static_cast<unsigned __int8>(-1);
  bySubClass[0] = static_cast<unsigned __int8>(-1);

  _unmannedtrader_Regist_item_inform_zocl info{};
  for (int j = 0; j < this->m_byMaxRegistCnt; ++j)
  {
    CUnmannedTraderRegistItemInfo &item = this->m_vecRegistItemInfo[j];
    if (item.IsRegist())
    {
      info.List[info.byNum].wItemSerial = item.GetItemSerial();
      info.List[info.byNum].dwRegistSerial = item.GetRegistSerial();
      info.List[info.byNum].dwPrice = item.GetPrice();
      info.List[info.byNum].dwLeftSec = static_cast<unsigned int>(item.GetLeftSec());
      info.List[info.byNum].dwListIndex = 0;

      unsigned int *listIndex = &info.List[info.byNum].dwListIndex;
      const unsigned __int16 itemIndex = item.GetItemIndex();
      const unsigned __int8 tableCode = item.GetTableCode();
      CUnmannedTraderGroupItemInfoTable::Instance()->GetGroupID(
        tableCode,
        itemIndex,
        byDivision,
        byClass,
        bySubClass,
        listIndex);
      ++info.byNum;
    }
  }

  unsigned __int8 pbyType[16]{};
  pbyType[0] = 30;
  pbyType[1] = 25;
  const unsigned __int16 len = static_cast<unsigned __int16>(info.size());
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_wInx, pbyType, reinterpret_cast<char *>(&info), len);
}

void CUnmannedTraderUserInfo::SendRegistItemSuccessResult(
  unsigned int dwLeftDalant,
  unsigned __int16 wInx,
  char *pLoadData)
{
  _unmannedtrader_regist_item_success_result_zocl result{};
  result.wItemSerial = *reinterpret_cast<unsigned __int16 *>(pLoadData + 2);
  result.dwPrice = *reinterpret_cast<unsigned int *>(pLoadData + 32);
  result.dwRegedSerial = *reinterpret_cast<unsigned int *>(pLoadData + 20);
  result.dwListIndex = *reinterpret_cast<unsigned int *>(pLoadData + 12);
  result.dwTax = *reinterpret_cast<unsigned int *>(pLoadData + 80);
  result.dwLeftDalant = dwLeftDalant;
  result.wSepaSerial = *reinterpret_cast<unsigned __int16 *>(pLoadData + 8);
  result.bySepaAmount = static_cast<unsigned __int8>(pLoadData[10]);

  unsigned __int8 pbyType[16]{};
  pbyType[0] = 30;
  pbyType[1] = 28;
  const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
  g_Network.m_pProcess[0]->LoadSendMsg(wInx, pbyType, reinterpret_cast<char *>(&result), len);
}

void CUnmannedTraderUserInfo::SendRegistItemErrorResult(
  unsigned __int16 wInx,
  char byRet,
  unsigned __int16 wItemSerial,
  unsigned int dwRetParam1)
{
  char msg[7]{};
  msg[0] = byRet;
  *reinterpret_cast<unsigned __int16 *>(msg + 1) = wItemSerial;
  *reinterpret_cast<unsigned int *>(msg + 3) = static_cast<unsigned int>(-1);

  if (byRet == 6 || byRet == 24 || byRet == static_cast<char>(-56))
  {
    *reinterpret_cast<unsigned int *>(msg + 3) = dwRetParam1;
  }

  unsigned __int8 pbyType[16]{};
  pbyType[0] = 30;
  pbyType[1] = 29;
  g_Network.m_pProcess[0]->LoadSendMsg(wInx, pbyType, msg, 7u);
}

void CUnmannedTraderUserInfo::SendCancelRegistSuccessResult(
  unsigned __int16 wInx,
  unsigned __int16 wItemSerial,
  unsigned int dwRegistSerial)
{
  _a_trade_clear_item_result_zocl result{};
  result.byRetCode = 0;
  result.wItemSerial = wItemSerial;
  result.dwRegistSerial = dwRegistSerial;

  unsigned __int8 pbyType[16]{};
  pbyType[0] = 30;
  pbyType[1] = 12;
  const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
  g_Network.m_pProcess[0]->LoadSendMsg(wInx, pbyType, reinterpret_cast<char *>(&result), len);
}

void CUnmannedTraderUserInfo::SendCancelRegistErrorResult(unsigned __int16 wInx, unsigned __int8 byRet)
{
  _a_trade_clear_item_result_zocl result{};
  result.byRetCode = byRet;

  unsigned __int8 pbyType[16]{};
  pbyType[0] = 30;
  pbyType[1] = 12;
  const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
  g_Network.m_pProcess[0]->LoadSendMsg(wInx, pbyType, reinterpret_cast<char *>(&result), len);
}

void CUnmannedTraderUserInfo::SendRepriceSuccessResult(
  CPlayer *pReceiver,
  unsigned __int16 wItemSerial,
  unsigned int dwNewPrice,
  unsigned int dwRegistSerial,
  unsigned int dwTax)
{
  _a_trade_adjust_price_result_zocl result{};
  result.byRetCode = 0;
  result.wItemSerial = wItemSerial;
  result.dwNewPrice = dwNewPrice;
  result.dwRegistSerial = dwRegistSerial;
  result.dwLeftDalant = pReceiver->m_Param.GetDalant();
  result.byTaxRate = 1;
  result.dwTax = dwTax;

  unsigned __int8 pbyType[16]{};
  pbyType[0] = 30;
  pbyType[1] = 10;
  const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
  g_Network.m_pProcess[0]->LoadSendMsg(pReceiver->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&result), len);
}

void CUnmannedTraderUserInfo::SendRepriceErrorResult(CPlayer *pReceiver, unsigned __int8 byRet)
{
  _a_trade_adjust_price_result_zocl result{};
  std::memset(&result.byTaxRate, 0, 0x13u);
  result.byRetCode = byRet;
  result.byTaxRate = 1;

  unsigned __int8 pbyType[16]{};
  pbyType[0] = 30;
  pbyType[1] = 10;
  const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
  g_Network.m_pProcess[0]->LoadSendMsg(pReceiver->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&result), len);
}

void CUnmannedTraderUserInfo::SendSellInfom(
  unsigned __int16 wInx,
  unsigned __int16 wItemSerial,
  unsigned int dwAddDalant,
  unsigned int dwTaxDalant,
  unsigned int dwTotalDalant)
{
  char msg[14]{};
  *reinterpret_cast<unsigned __int16 *>(msg) = wItemSerial;
  *reinterpret_cast<unsigned int *>(msg + 2) = dwAddDalant;
  *reinterpret_cast<unsigned int *>(msg + 6) = dwTaxDalant;
  *reinterpret_cast<unsigned int *>(msg + 10) = dwTotalDalant;

  unsigned __int8 pbyType[16]{};
  pbyType[0] = 30;
  pbyType[1] = 9;
  g_Network.m_pProcess[0]->LoadSendMsg(wInx, pbyType, msg, 0xEu);
}

void CUnmannedTraderUserInfo::SendNotifyCloseItem(
  unsigned __int16 wInx,
  unsigned __int16 wItemSerial,
  unsigned int dwRegistSerial,
  unsigned int dwPrice,
  unsigned __int8 byTaxRate)
{
  (void)byTaxRate;
  _unmannedtrader_close_item_inform_zocl inform{};
  inform.byTaxRate = 1;
  inform.dwPrice = dwPrice;
  inform.wItemSerial = wItemSerial;
  inform.dwRegistSerial = dwRegistSerial;

  unsigned __int8 pbyType[16]{};
  pbyType[0] = 30;
  pbyType[1] = 27;
  const unsigned __int16 len = static_cast<unsigned __int16>(inform.size());
  g_Network.m_pProcess[0]->LoadSendMsg(wInx, pbyType, reinterpret_cast<char *>(&inform), len);
}
