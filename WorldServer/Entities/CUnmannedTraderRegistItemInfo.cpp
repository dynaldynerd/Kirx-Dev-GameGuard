#include "pch.h"

#include "CUnmannedTraderRegistItemInfo.h"
#include "CLogFile.h"
#include "CPlayer.h"
#include "CPlayerDB.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

#include <cstdio>
#include <ctime>

CUnmannedTraderRegistItemInfo::CUnmannedTraderRegistItemInfo()
  : m_dwRegistSerial(0),
    m_wItemSerial(0),
    m_dwETSerialNumber(0),
    m_dwPrice(0),
    m_tStartTime(0),
    m_bySellTurm(0),
    m_dwBuyerSerial(0),
    m_dwTax(0),
    m_tResultTime(0),
    m_wszBuyerName{},
    m_szBuyerAccount{},
    m_byTableCode(0),
    m_wItemIndex(0),
    m_byStorageIndex(0),
    m_dwD(0),
    m_dwU(0),
    m_kState()
{
  Clear();
}

CUnmannedTraderRegistItemInfo::~CUnmannedTraderRegistItemInfo()
{
  Clear();
}

void CUnmannedTraderRegistItemInfo::Clear()
{
  this->m_kState.Clear();
  this->m_dwRegistSerial = 0;
  this->m_wItemSerial = static_cast<unsigned short>(-1);
  this->m_dwETSerialNumber = 0;
  this->m_dwPrice = 0;
  this->m_tStartTime = 0;
  this->m_bySellTurm = 0;
  this->m_dwBuyerSerial = 0;
  this->m_dwTax = 0;
  this->m_tResultTime = 0;
  this->m_wszBuyerName[0] = '\0';
  this->m_szBuyerAccount[0] = '\0';
  this->m_byTableCode = static_cast<unsigned char>(-1);
  this->m_wItemIndex = 255;
  this->m_byStorageIndex = static_cast<unsigned char>(-1);
  this->m_dwD = static_cast<unsigned long long>(-1);
  this->m_dwU = 0x7FFFFFF;
}

bool CUnmannedTraderRegistItemInfo::Set(
  unsigned __int16 wInx,
  unsigned __int8 byInvenIndex,
  unsigned int uiInx,
  _TRADE_DB_BASE *kInfo,
  CLogFile *pkLogger)
{
  _STORAGE_LIST::_db_con *item = g_Player[wInx].m_Param.GetItem(byInvenIndex);
  if (uiInx < 10 && item)
  {
    if (m_kState.Set(kInfo->m_List[uiInx].byState))
    {
      m_dwRegistSerial = kInfo->m_List[uiInx].dwRegistSerial;
      m_wItemSerial = item->m_wSerial;
      m_dwETSerialNumber = item->m_dwETSerialNumber;
      m_dwPrice = kInfo->m_List[uiInx].dwPrice;
      m_tStartTime = kInfo->m_List[uiInx].tStartTime;
      m_bySellTurm = kInfo->m_List[uiInx].bySellTurm;
      m_dwTax = kInfo->m_List[uiInx].dwTax;
      m_tResultTime = kInfo->m_List[uiInx].tResultTime;
      if (IsSellWait())
      {
        m_dwBuyerSerial = kInfo->m_List[uiInx].dwBuyerSerial;
        strcpy_s(m_wszBuyerName, sizeof(m_wszBuyerName), kInfo->m_List[uiInx].wszBuyerName);
        strcpy_s(m_szBuyerAccount, sizeof(m_szBuyerAccount), kInfo->m_List[uiInx].szBuyerAccount);
      }
      m_byTableCode = item->m_byTableCode;
      m_wItemIndex = item->m_wItemIndex;
      m_byStorageIndex = item->m_byStorageIndex;
      m_dwD = item->m_dwDur;
      m_dwU = item->m_dwLv;
      if (IsRegist())
      {
        item->lock(true);
      }
      return true;
    }

    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderRegistItemInfo::Set(...)\r\n"
        "\t\tdwRegistSerial(%u)\r\n"
        "\t\tm_kState.Set( kInfo.m_List[uiInx(%u)].byState(%u) ) Faili!\r\n",
        kInfo->m_List[uiInx].dwRegistSerial,
        uiInx,
        kInfo->m_List[uiInx].byState);
    }
    return false;
  }

  if (pkLogger)
  {
    const unsigned __int8 maxSlot = static_cast<unsigned __int8>(g_Player[wInx].m_Param.GetUseSlot());
    pkLogger->Write(
      "CUnmannedTraderRegistItemInfo::Set(...)\r\n"
      "\t\tdwRegistSerial(%u) MaxSlot(%u) byInvenIndex(%u)\r\n"
      "\t\t( max_used_trade_num <= uiInx(%u) || 0 == pkItem(%p) ) == false!\r\n",
      kInfo->m_List[uiInx].dwRegistSerial,
      maxSlot,
      byInvenIndex,
      uiInx,
      item);
  }
  return false;
}

void CUnmannedTraderRegistItemInfo::RegistItem(
  unsigned int dwRegistSerial,
  unsigned __int16 wItemSerial,
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
  __int64 now = 0;
  time_18(&now);

  m_tStartTime = now;
  m_dwRegistSerial = dwRegistSerial;
  m_wItemSerial = wItemSerial;
  m_dwETSerialNumber = dwETSerialNumber;
  m_dwPrice = dwPrice;
  m_bySellTurm = bySellTurm;
  m_dwBuyerSerial = 0xFFFFFFFFu;
  m_dwTax = 0;
  m_tResultTime = 0;
  m_wszBuyerName[0] = '\0';
  m_szBuyerAccount[0] = '\0';
  m_byTableCode = byTableCode;
  m_wItemIndex = wItemIndex;
  m_byStorageIndex = byStorageIndex;
  m_dwD = dwD;
  m_dwU = dwU;

  const unsigned __int8 state = bInserted ? 2u : 1u;
  m_kState.Set(state);
}

void CUnmannedTraderRegistItemInfo::RepriceItem(unsigned int dwPrice)
{
  m_dwPrice = dwPrice;
}

void CUnmannedTraderRegistItemInfo::ReRegistItem(unsigned int dwPrice)
{
  __int64 now = 0;
  time_18(&now);

  m_tStartTime = now;
  m_dwPrice = dwPrice;
  m_kState.Set(1u);
}

void CUnmannedTraderRegistItemInfo::SellComplete(
  unsigned int dwPrice,
  unsigned int dwBuyerSerial,
  unsigned int dwTax,
  __int64 tResultTime,
  const char *wszBuyerName,
  const char *szBuyerAccount)
{
  m_dwPrice = dwPrice;
  m_dwBuyerSerial = dwBuyerSerial;
  m_dwTax = dwTax;
  m_tResultTime = tResultTime;
  strcpy_s(m_wszBuyerName, 0x11u, wszBuyerName);
  strcpy_s(m_szBuyerAccount, 0xDu, szBuyerAccount);
  m_kState.Set(3u);
}

unsigned __int8 CUnmannedTraderRegistItemInfo::SellWaitItem(
  unsigned __int16 wInx,
  CLogFile *pkLogger,
  __int64 tResultTime,
  unsigned __int8 *byStorageInx)
{
  if (wInx >= MAX_PLAYER)
  {
    return 99;
  }

  if (!m_dwBuyerSerial)
  {
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderRegistItemInfo::SellWaitItem(...)\r\n"
        "\t\t(0 == m_dwBuyerSerial) m_dwRegistSerial(%u) Serial(%u) State(%d)\r\n",
        g_Player[wInx].m_id.dwSerial,
        m_dwRegistSerial,
        static_cast<int>(m_kState.GetState()));
    }
    return 94;
  }

  CPlayer *seller = &g_Player[wInx];
  _STORAGE_LIST::_storage_con *item = seller->m_Param.m_dbInven.GetPtrFromSerial(m_wItemSerial);
  if (!item)
  {
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderRegistItemInfo::SellWaitItem( WORD wInx(%u), CLogFile * pkLogger )\r\n"
        "\t\tName(%s) Serial(%u) Invalid DB Data!\r\n"
        "\t\tm_dwRegistSerial(%u)\r\n"
        "\t\tpkPlayer->m_Param.m_dbInven.GetPtrFromSerial( m_wItemSerial(%u) ) == NULL!\r\n",
        wInx,
        seller->m_Param.GetCharNameA(),
        seller->m_id.dwSerial,
        m_dwRegistSerial,
        m_wItemSerial);
    }
    return 37;
  }

  if (m_dwPrice <= m_dwTax)
  {
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderController::SellWaitItem(...) Exceed Tax Price!\r\n"
        "\t\t dwRegistSerial(%u) dwSeller(%u) dwBuyer(%u)\r\n"
        "\t\t dwPrice(%u) dwTax(%u)\r\n",
        m_dwRegistSerial,
        seller->m_dwObjSerial,
        m_dwBuyerSerial,
        m_dwPrice,
        m_dwTax);
    }
    m_dwTax = static_cast<unsigned int>(static_cast<float>(m_dwPrice) * 0.050000001f);
  }

  const unsigned int realSellDalant = m_dwPrice - m_dwTax;
  if (!realSellDalant || CanAddMoneyForMaxLimMoney(realSellDalant, seller->m_Param.GetDalant()))
  {
    _STORAGE_LIST::_db_con *dbItem = reinterpret_cast<_STORAGE_LIST::_db_con *>(item);
    item->lock(false);
    *byStorageInx = dbItem->m_byStorageIndex;
    seller->AddDalant(static_cast<int>(realSellDalant), true);

    char buyerName[17]{};
    W2M(m_wszBuyerName, buyerName, 0x11u);
    CPlayer::s_MgrItemHistory.auto_trade_sell(
      buyerName,
      m_dwBuyerSerial,
      m_szBuyerAccount,
      m_dwRegistSerial,
      reinterpret_cast<_STORAGE_LIST::_db_con *>(item),
      realSellDalant,
      m_dwTax,
      seller->m_Param.GetDalant(),
      seller->m_Param.GetGold(),
      seller->m_szItemHistoryFileName);

    std::time_t resultTime = static_cast<std::time_t>(tResultTime);
    tm *timeInfo = std::localtime(&resultTime);
    char timeBuffer[64]{};
    if (timeInfo)
    {
      std::sprintf(
        timeBuffer,
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
      std::sprintf(timeBuffer, "Invalid(%u)", static_cast<unsigned int>(tResultTime));
    }

    _base_fld *record = g_Main.m_tblItemData[item->m_byTableCode].GetRecord(item->m_wItemIndex);
    g_Main.m_logAutoTrade.Write(
      "%s %s >> sellwait (%u) user(%u)%s -> user(%u)%s price(%u) tax(%u)",
      record->m_strCode,
      timeBuffer,
      m_dwRegistSerial,
      m_dwBuyerSerial,
      buyerName,
      seller->m_dwObjSerial,
      seller->m_Param.GetCharNameA(),
      m_dwPrice,
      m_dwTax);
    return 0;
  }

  _STORAGE_LIST::_db_con *dbItem = reinterpret_cast<_STORAGE_LIST::_db_con *>(item);
  seller->Emb_DelStorage(0, dbItem->m_byStorageIndex, false, true, "CUnmannedTraderRegistItemInfo::SellWaitItem()");
  if (pkLogger)
  {
    pkLogger->Write(
      "CUnmannedTraderRegistItemInfo::SellWaitItem( WORD wInx(%u), DWORD & dwRealSellDalant, CLogFile * pkLogger )\r\n"
      "\t\tm_dwRegistSerial(%u) dwRealSellDalant(%u) = m_dwPrice(%u) - m_dwTax(%u)\r\n"
      "\t\tName(%s) Serial(%u) Cur(%u) Sell(%u) Exceed Max Money!\r\n",
      wInx,
      m_dwRegistSerial,
      realSellDalant,
      m_dwPrice,
      m_dwTax,
      seller->m_Param.GetCharNameA(),
      seller->m_id.dwSerial,
      seller->m_Param.GetDalant(),
      realSellDalant);
  }
  return 34;
}

void CUnmannedTraderRegistItemInfo::ClearBuyerInfo()
{
  m_dwBuyerSerial = 0;
  m_dwTax = 0;
  m_wszBuyerName[0] = '\0';
  m_szBuyerAccount[0] = '\0';
}

void CUnmannedTraderRegistItemInfo::ClearRegist()
{
  Clear();
  m_kState.Set(5u);
}

void CUnmannedTraderRegistItemInfo::ClearToWaitState()
{
  Clear();
  m_kState.Set(0u);
}

bool CUnmannedTraderRegistItemInfo::IsEmpty()
{
  const unsigned int state = m_kState.GetState();
  return state == static_cast<unsigned int>(CUnmannedTraderItemState::STATE::FIREEL)
      || state == static_cast<unsigned int>(CUnmannedTraderItemState::STATE::WINDEL)
      || state == 5
      || state == (static_cast<unsigned int>(CUnmannedTraderItemState::STATE::WINDEL) | 0x4)
      || state == (static_cast<unsigned int>(CUnmannedTraderItemState::STATE::WINDEL) | 0x8)
      || state == 8
      || state == 9
      || state == 12;
}

bool CUnmannedTraderRegistItemInfo::IsRegist()
{
  const unsigned int state = m_kState.GetState();
  return state == static_cast<unsigned int>(CUnmannedTraderItemState::STATE::WATEREL)
      || state == static_cast<unsigned int>(CUnmannedTraderItemState::STATE::SOILEL);
}

bool CUnmannedTraderRegistItemInfo::IsSellWait()
{
  return m_kState.GetState() == 4;
}

bool CUnmannedTraderRegistItemInfo::IsSellUpdateWait()
{
  return m_kState.GetState() == 10;
}

bool CUnmannedTraderRegistItemInfo::IsOverRegistTime()
{
  return GetLeftSec() == 0;
}

unsigned __int16 CUnmannedTraderRegistItemInfo::GetItemIndex()
{
  return m_wItemIndex;
}

unsigned __int16 CUnmannedTraderRegistItemInfo::GetItemSerial()
{
  return m_wItemSerial;
}

unsigned int CUnmannedTraderRegistItemInfo::GetBuyerSerial()
{
  return m_dwBuyerSerial;
}

unsigned __int8 CUnmannedTraderRegistItemInfo::GetTableCode()
{
  return m_byTableCode;
}

CUnmannedTraderItemState::STATE CUnmannedTraderRegistItemInfo::GetState()
{
  return static_cast<CUnmannedTraderItemState::STATE>(m_kState.GetState());
}

__int64 *CUnmannedTraderRegistItemInfo::GetStartTimePtr()
{
  return &m_tStartTime;
}

__int64 CUnmannedTraderRegistItemInfo::GetStartTime()
{
  return m_tStartTime;
}

__int64 CUnmannedTraderRegistItemInfo::GetResultTime()
{
  return m_tResultTime;
}

__int64 CUnmannedTraderRegistItemInfo::GetLeftSec()
{
  __int64 now = 0;
  time_18(&now);

  const __int64 endTime = m_tStartTime + 3600 * m_bySellTurm;
  if (endTime <= now)
  {
    return 0;
  }

  return static_cast<unsigned int>(endTime - now);
}

unsigned __int8 CUnmannedTraderRegistItemInfo::GetSellTurm()
{
  return m_bySellTurm;
}

unsigned int CUnmannedTraderRegistItemInfo::GetPrice()
{
  return m_dwPrice;
}

unsigned int CUnmannedTraderRegistItemInfo::GetTax()
{
  return m_dwTax;
}

unsigned int CUnmannedTraderRegistItemInfo::GetETSerial()
{
  return m_dwETSerialNumber;
}

unsigned int CUnmannedTraderRegistItemInfo::GetU()
{
  return m_dwU;
}

unsigned __int64 CUnmannedTraderRegistItemInfo::GetD()
{
  return m_dwD;
}

unsigned __int8 CUnmannedTraderRegistItemInfo::GetStorageIndex()
{
  return m_byStorageIndex;
}

unsigned int CUnmannedTraderRegistItemInfo::GetRegistSerial()
{
  return m_dwRegistSerial;
}

bool CUnmannedTraderRegistItemInfo::SetState(unsigned __int8 byState)
{
  return m_kState.Set(byState) != 0;
}

void CUnmannedTraderRegistItemInfo::SetOverRegistTime()
{
  __int64 now = 0;
  time_18(&now);
  m_tStartTime = now - 3600 * m_bySellTurm;
}

bool CUnmannedTraderRegistItemInfo::IsWaitNoitfyClose()
{
  const unsigned int state = m_kState.GetState();
  return state == 6 || state == 13;
}
