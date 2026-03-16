#include "pch.h"

#include "CItemStore.h"

#include <atltime.h>
#include <ctime>

#include "CMapData.h"
#include "CRecordData.h"
#include "DummyPosition.h"
#include "GlobalObjects.h"
#include "ItemExist_fld.h"
#include "npc_fld.h"
#include "StoreList_fld.h"
#include "TimeItem.h"
#include "WorldServerUtil.h"

_good_storage_info::_good_storage_info()
{
  std::memset(this, 0, sizeof(_good_storage_info));
}

void _limit_item_info::init()
{
  std::memset(this, 0, sizeof(_limit_item_info));
}

_limit_item_info::_limit_item_info()
{
  init();
}

CItemStore::CItemStore()
{
  std::memset(this, 0, sizeof(CItemStore));
}

CItemStore::~CItemStore()
{
  if (m_pStorageItem)
  {
    delete[] m_pStorageItem;
    m_pStorageItem = nullptr;
  }
  if (m_pLimitStorageItem)
  {
    delete[] m_pLimitStorageItem;
    m_pLimitStorageItem = nullptr;
  }
}

bool CItemStore::Init(int nIndex, CMapData *pExistMap, _store_dummy *pDum, _StoreList_fld *pRec)
{
  if (m_bLive)
  {
    return false;
  }

  m_bLive = true;
  m_nIndex = nIndex;
  m_nStorageItemNum = 0;
  m_pExistMap = pExistMap;
  m_pDum = pDum;
  m_pRec = pRec;
  m_bUpdate = false;
  m_bDBDataCheck = false;

  if (GetNpcRaceCode(&m_byNpcRaceCode))
  {
    if (m_byNpcRaceCode == 0 || m_byNpcRaceCode == 1 || m_byNpcRaceCode == 2 || m_byNpcRaceCode == 255)
    {
      m_nStorageItemNum = 0;
      if (m_pRec->m_nStore_trade != 7 && m_pRec->m_nStore_trade != 10)
      {
        m_pStorageItem = new _good_storage_info[216];
        m_pLimitStorageItem = new _limit_item_info[16];

        int limitItemCount = 0;
        for (int j = 0; j < 16; ++j)
        {
          int nTableCode = GetItemTableCode(m_pRec->m_sellLimitList[j].m_strItemCode);
          if (nTableCode == -1)
          {
            break;
          }
          m_pStorageItem[j].byItemTableCode = static_cast<unsigned __int8>(nTableCode);
          _ItemExist_fld *Record = static_cast<_ItemExist_fld *>(
            g_Main.m_tblItemData[nTableCode].GetRecord(m_pRec->m_sellLimitList[j].m_strItemCode));
          if (!Record)
          {
            MyMessageBox(
              "CItemStore::Init error",
              "%d Store %s Limit Item can't find index",
              nIndex,
              m_pRec->m_sellLimitList[j].m_strItemCode);
            break;
          }
          m_pStorageItem[j].wItemIndex = static_cast<unsigned __int16>(Record->m_dwIndex);
          int ItemStdPrice = GetItemStdPrice(
            nTableCode,
            Record->m_dwIndex,
            m_byNpcRaceCode,
            &m_pStorageItem[j].byMoneyUnit);
          m_pStorageItem[j].nStdPrice = ItemStdPrice;
          int ItemStdPoint = GetItemStdPoint(
            nTableCode,
            Record->m_dwIndex,
            m_byNpcRaceCode,
            &m_pStorageItem[j].byMoneyUnit);
          m_pStorageItem[j].nStdPoint = ItemStdPoint;
          int ItemProcPoint = GetItemProcPoint(
            nTableCode,
            Record->m_dwIndex,
            m_byNpcRaceCode,
            &m_pStorageItem[j].byMoneyUnit);
          m_pStorageItem[j].nResPoint = ItemProcPoint;
          int ItemKillPoint = GetItemKillPoint(
            nTableCode,
            Record->m_dwIndex,
            m_byNpcRaceCode,
            &m_pStorageItem[j].byMoneyUnit);
          m_pStorageItem[j].nKillPoint = ItemKillPoint;
          int ItemGoldPoint = GetItemGoldPoint(
            nTableCode,
            Record->m_dwIndex,
            m_byNpcRaceCode,
            &m_pStorageItem[j].byMoneyUnit);
          m_pStorageItem[j].nGoldPoint = ItemGoldPoint;
          m_pStorageItem[j].nStdPrice *= m_pRec->m_nPriceSet;
          m_pStorageItem[j].dwUpCode = m_pRec->m_nItemUpCode;
          if (m_pStorageItem[j].nStdPrice == -1)
          {
            MyMessageBox(
              "CItemStore::Init error",
              "price load error..(nTable: %d, pFld->m_dwIndex: %d)",
              nTableCode,
              Record->m_dwIndex);
            break;
          }
          m_pStorageItem[j].bExist = static_cast<signed char>(Record->m_bExist);
          unsigned int ItemDurPoint = GetItemDurPoint(nTableCode, Record->m_dwIndex);
          m_pStorageItem[j].dwDurPoint = ItemDurPoint;
          m_pStorageItem[j].byType = 1;
          m_pStorageItem[j].dwLimitIndex = j;
          ++m_nStorageItemNum;
          m_pLimitStorageItem[j].bLoad = true;
          m_pLimitStorageItem[j].dwStorageIndex = j;
          m_pLimitStorageItem[j].Key.byTableCode = static_cast<unsigned __int8>(nTableCode);
          m_pLimitStorageItem[j].Key.wItemIndex = static_cast<unsigned __int16>(Record->m_dwIndex);
          m_pLimitStorageItem[j].nLimitNum = m_pRec->m_sellLimitList[j].m_nMaxCount;
          ++m_nLimitStorageItemNum;
          ++limitItemCount;
        }
        if (m_nLimitStorageItemNum > 0)
        {
          SetLimitItemInitTime();
        }
        for (int j = limitItemCount; j < 216; ++j)
        {
          int nTableCode = GetItemTableCode(m_pRec->m_strItemlist[j]);
          if (nTableCode == -1)
          {
            break;
          }
          m_pStorageItem[j].byItemTableCode = static_cast<unsigned __int8>(nTableCode);
          _ItemExist_fld *itemRecord = static_cast<_ItemExist_fld *>(
            g_Main.m_tblItemData[nTableCode].GetRecord(m_pRec->m_strItemlist[j]));
          if (!itemRecord)
          {
            MyMessageBox(
              "CItemStore::Init error",
              "%d Store %s Item can't find index",
              nIndex,
              m_pRec->m_strItemlist[j]);
            break;
          }
          m_pStorageItem[j].wItemIndex = static_cast<unsigned __int16>(itemRecord->m_dwIndex);
          int itemStdPrice =
            GetItemStdPrice(nTableCode, itemRecord->m_dwIndex, m_byNpcRaceCode, &m_pStorageItem[j].byMoneyUnit);
          m_pStorageItem[j].nStdPrice = itemStdPrice;
          int itemStdPoint =
            GetItemStdPoint(nTableCode, itemRecord->m_dwIndex, m_byNpcRaceCode, &m_pStorageItem[j].byMoneyUnit);
          m_pStorageItem[j].nStdPoint = itemStdPoint;
          int itemProcPoint = GetItemProcPoint(
            nTableCode,
            itemRecord->m_dwIndex,
            m_byNpcRaceCode,
            &m_pStorageItem[j].byMoneyUnit);
          m_pStorageItem[j].nResPoint = itemProcPoint;
          int itemKillPoint = GetItemKillPoint(
            nTableCode,
            itemRecord->m_dwIndex,
            m_byNpcRaceCode,
            &m_pStorageItem[j].byMoneyUnit);
          m_pStorageItem[j].nKillPoint = itemKillPoint;
          int itemGoldPoint = GetItemGoldPoint(
            nTableCode,
            itemRecord->m_dwIndex,
            m_byNpcRaceCode,
            &m_pStorageItem[j].byMoneyUnit);
          m_pStorageItem[j].nGoldPoint = itemGoldPoint;
          m_pStorageItem[j].nStdPrice *= m_pRec->m_nPriceSet;
          m_pStorageItem[j].dwUpCode = m_pRec->m_nItemUpCode;
          if (m_pStorageItem[j].nStdPrice == -1)
          {
            MyMessageBox(
              "CItemStore::Init error",
              "price load error..(nTable: %d, pFld->m_dwIndex: %d)",
              nTableCode,
              itemRecord->m_dwIndex);
            break;
          }
          m_pStorageItem[j].bExist = static_cast<signed char>(itemRecord->m_bExist);
          unsigned int itemDurPoint = GetItemDurPoint(nTableCode, itemRecord->m_dwIndex);
          m_pStorageItem[j].dwDurPoint = itemDurPoint;
          ++m_nStorageItemNum;
        }
      }

      m_dwSecIndex = CalcSecIndex(m_pDum->m_pDumPos->m_fCenterPos[0], m_pDum->m_pDumPos->m_fCenterPos[2]);
      return true;
    }

    MyMessageBox(
      "CItemStore::Init error",
      "Map:%s, Store:%d.. unregistered race code (%d) ",
      pExistMap->m_pMapSet->m_strCode,
      nIndex,
      m_byNpcRaceCode);
    return false;
  }

  MyMessageBox(
    "CItemStore::Init error",
    "Map:%s, Store:%d..  unregistered npc code (%s) ",
    pExistMap->m_pMapSet->m_strCode,
    nIndex,
    m_pRec->m_strStore_NPCcode);
  return false;
}

_base_fld *CItemStore::GetNpcRecord()
{
  return g_Main.m_tblNPC.GetRecord(m_pRec->m_strStore_NPCcode);
}

bool CItemStore::GetNpcRaceCode(unsigned __int8 *pbyRaceCode)
{
  _npc_fld *record = reinterpret_cast<_npc_fld *>(g_Main.m_tblNPC.GetRecord(m_pRec->m_strStore_NPCcode));
  if (!record)
    return false;
  *pbyRaceCode = static_cast<unsigned __int8>(record->m_nRace);
  return true;
}

unsigned int CItemStore::CalcSecIndex(float x, float z)
{
  _bsp_info *bspInfo = &m_pExistMap->m_BspInfo;
  const float localX = static_cast<float>(-bspInfo->m_nMapMinSize[0]) + x;
  const float localZ = static_cast<float>(bspInfo->m_nMapMaxSize[2]) - z;
  const int secX = static_cast<int>(localX) / 100;
  const int secY = static_cast<int>(localZ) / 100;
  _sec_info *secInfo = &m_pExistMap->m_SecInfo;
  return static_cast<unsigned int>(secInfo->m_nSecNumW * secY + secX);
}

void CItemStore::SetLimitItemInitTime()
{
  const unsigned __int64 initIntervalMs = 60000LL * m_pRec->m_nLimitItem_InitTime;
  const int totalDays = static_cast<int>(initIntervalMs / 86400000);
  const __int64 nowSec = static_cast<__int64>(time(nullptr));

  if (totalDays > 0)
  {
    CTimeSpan span(totalDays, 0, 0, 0);
    CTime base = CTime::GetCurrentTime();
    CTime target = base + span;
    CTime midnight(target.GetYear(), target.GetMonth(), target.GetDay(), 0, 0, 0);
    m_dwLimitInitTime = midnight.GetTime();
  }
  else
  {
    m_dwLimitInitTime = initIntervalMs + nowSec;
  }
}

char *CItemStore::GetNpcCode()
{
  if (m_pRec)
  {
    return m_pRec->m_strStore_NPCcode;
  }
  return nullptr;
}

float *CItemStore::GetStorePos()
{
  return m_pDum->m_pDumPos->m_fCenterPos;
}

_limit_item_info *CItemStore::GetLimitItem(int nIndex)
{
  if (nIndex >= 0 && nIndex < m_nLimitStorageItemNum)
  {
    return &m_pLimitStorageItem[nIndex];
  }
  return nullptr;
}

void CItemStore::InitLimitItemInfo()
{
  int tableCode = -1;
  for (int j = 0; j < 16; ++j)
  {
    tableCode = GetItemTableCode(m_pRec->m_sellLimitList[j].m_strItemCode);
    if (tableCode == -1)
    {
      m_pLimitStorageItem[j].init();
      continue;
    }

    auto *record = reinterpret_cast<unsigned __int16 *>(
      g_Main.m_tblItemData[tableCode].GetRecord(m_pRec->m_sellLimitList[j].m_strItemCode));
    if (!record)
    {
      m_pLimitStorageItem[j].init();
      continue;
    }

    m_pLimitStorageItem[j].bLoad = true;
    m_pLimitStorageItem[j].dwStorageIndex = j;
    m_pLimitStorageItem[j].Key.byTableCode = static_cast<unsigned __int8>(tableCode);
    m_pLimitStorageItem[j].Key.wItemIndex = *record;
    m_pLimitStorageItem[j].nLimitNum = m_pRec->m_sellLimitList[j].m_nMaxCount;
  }
}

void CItemStore::GetLimitItemAmount(_limit_amount_info *pAmountInfo)
{
  if (m_nLimitStorageItemNum > 0)
  {
    for (int j = 0; j < m_nLimitStorageItemNum; ++j)
    {
      if (m_pLimitStorageItem[j].bLoad)
      {
        const unsigned __int8 index = pAmountInfo->byItemNum;
        pAmountInfo->ItemInfo[index].dwLimitItemIndex = m_pLimitStorageItem[j].dwStorageIndex;
        pAmountInfo->ItemInfo[pAmountInfo->byItemNum++].wLimitNum = m_pLimitStorageItem[j].nLimitNum;
      }
    }
  }
}

unsigned __int8 CItemStore::IsSell(
  unsigned __int8 byOfferNum,
  _buy_offer *pOffer,
  unsigned int dwHasDalant,
  unsigned int dwHasGold,
  double dHasPoint,
  unsigned int *dwHasActPoint,
  unsigned __int8 *pbyActCode,
  float fDiscountRate,
  unsigned __int8 byRace,
  unsigned __int8 byPvpGrade)
{
  if (!m_pStorageItem || !m_pLimitStorageItem)
  {
    return 100;
  }

  SetZeroTradeMoney();
  const unsigned int texRate = eGetTexRate(byRace) + 10000;
  (void)eGetTex(byRace);
  constexpr unsigned __int64 kMaxTradeTotal = 0xFFFFFFFFui64;

  int limitCounts[16]{};
  for (int j = 0; j < byOfferNum; ++j)
  {
    if (pOffer[j].byGoodIndex >= m_nStorageItemNum)
    {
      return 17;
    }

    _good_storage_info *good = &m_pStorageItem[pOffer[j].byGoodIndex];
    // Yorozuya store fix (non-IDA): reject invalid non-stackable counts and
    // prevent 32-bit trade-total wrap before touching the legacy accumulators.
    if (pOffer[j].byGoodAmount > 1 && !IsOverLapItem(good->byItemTableCode))
    {
      return 100;
    }

    if (good->byType == 1)
    {
      limitCounts[good->dwLimitIndex] += pOffer[j].byGoodAmount;
      if (limitCounts[good->dwLimitIndex] > m_pLimitStorageItem[good->dwLimitIndex].nLimitNum)
      {
        return 19;
      }
    }

    unsigned __int8 moneyUnit[20]{};
    unsigned int price = CalcSellPrice(pOffer[j].byGoodIndex, moneyUnit);
    unsigned __int64 totalCost = static_cast<unsigned __int64>(pOffer[j].byGoodAmount) * price;
    const unsigned __int8 unit = moneyUnit[0];
    if (unit == 4)
    {
      *pbyActCode = 0;
    }
    else if (unit == 5)
    {
      *pbyActCode = 1;
    }
    else if (unit == 6)
    {
      *pbyActCode = 2;
    }

    if (fDiscountRate > 0.0f)
    {
      if (fDiscountRate > 1.0f)
      {
        fDiscountRate = FLOAT_1_0;
      }
      const unsigned __int64 discountRate = static_cast<unsigned int>(fDiscountRate * 10000.0f);
      totalCost -= (discountRate * totalCost) / 10000;
      if (!totalCost)
      {
        totalCost = 1;
      }
    }

    totalCost = static_cast<unsigned __int64>(texRate) * totalCost / 10000;
    if (unit)
    {
      switch (unit)
      {
        case 1:
        {
          const unsigned __int64 nextTradeGold = static_cast<unsigned __int64>(m_dwLastTradeGold) + totalCost;
          if (nextTradeGold > kMaxTradeTotal)
          {
            return 100;
          }
          m_dwLastTradeGold = static_cast<unsigned int>(nextTradeGold);
          if (dwHasGold < m_dwLastTradeGold)
          {
            return 14;
          }
          break;
        }
        case 2:
        case 3:
        {
          const unsigned __int64 nextTradePoint = static_cast<unsigned __int64>(m_dwLastTradePoint) + totalCost;
          if (nextTradePoint > kMaxTradeTotal)
          {
            return 100;
          }
          m_dwLastTradePoint = static_cast<unsigned int>(nextTradePoint);
          if (static_cast<double>(static_cast<int>(m_dwLastTradePoint)) > dHasPoint || dHasPoint < 0.0)
          {
            return 21;
          }
          break;
        }
        case 4:
        case 5:
        case 6:
        {
          const unsigned __int64 nextTradeActPoint =
            static_cast<unsigned __int64>(m_dwLastTradeActPoint[*pbyActCode]) + totalCost;
          if (nextTradeActPoint > kMaxTradeTotal)
          {
            return 100;
          }
          m_dwLastTradeActPoint[*pbyActCode] = static_cast<unsigned int>(nextTradeActPoint);
          if (dwHasActPoint[*pbyActCode] < m_dwLastTradeActPoint[*pbyActCode])
          {
            switch (unit)
            {
              case 4:
                return 25;
              case 5:
                return 26;
              case 6:
                return 27;
            }
          }
          break;
        }
        default:
          break;
      }
    }
    else
    {
      const unsigned __int64 nextTradeDalant = static_cast<unsigned __int64>(m_dwLastTradeDalant) + totalCost;
      if (nextTradeDalant > kMaxTradeTotal)
      {
        return 100;
      }
      m_dwLastTradeDalant = static_cast<unsigned int>(nextTradeDalant);
      if (dwHasDalant < m_dwLastTradeDalant)
      {
        return 13;
      }
    }
  }

  for (int j = 0; j < byOfferNum; ++j)
  {
    _good_storage_info *good = &m_pStorageItem[pOffer[j].byGoodIndex];
    pOffer[j].Item.m_byTableCode = good->byItemTableCode;
    pOffer[j].Item.m_wItemIndex = good->wItemIndex;
    pOffer[j].Item.m_dwDur = IsOverLapItem(good->byItemTableCode) ? pOffer[j].byGoodAmount : good->dwDurPoint;
    const unsigned __int8 itemKind = GetItemKindCode(good->byItemTableCode);
    if (itemKind)
    {
      if (itemKind != 1)
      {
        return 100;
      }
      pOffer[j].Item.m_dwLv = GetMaxParamFromExp(good->wItemIndex, pOffer[j].Item.m_dwDur);
    }
    else
    {
      pOffer[j].Item.m_dwLv = good->dwUpCode;
    }

    const _TimeItem_fld *timeRec = TimeItem::FindTimeRec(good->byItemTableCode, good->wItemIndex);
    if (timeRec)
    {
      __time32_t now{};
      _time32(&now);
      pOffer[j].Item.m_byCsMethod = timeRec->m_nCheckType;
      pOffer[j].Item.m_dwT = timeRec->m_nUseTime + now;
      pOffer[j].Item.m_dwLendRegdTime = now;
    }
  }
  return 0;
}

unsigned __int8 CItemStore::IsBuy(
  unsigned __int8 byOfferNum,
  _sell_offer *pOffer,
  float fDiscountRate,
  unsigned __int8 byRace)
{
  if (!m_pStorageItem)
  {
    return 100;
  }

  SetZeroTradeMoney();
  const float texAdjust = FLOAT_1_0 - eGetTex(byRace);
  const float oreRate = eGetOreRate(byRace);
  const int oreRateScaled = static_cast<int>(oreRate * 10000.0f);
  const unsigned int texRate = 10000 - eGetTexRate(byRace);

  for (int j = 0; j < byOfferNum; ++j)
  {
    if (pOffer[j].pItem->m_byTableCode == 19)
    {
      return 15;
    }
  }

  for (int j = 0; j < byOfferNum; ++j)
  {
    const int itemIndex = pOffer[j].pItem->m_wItemIndex;
    const int tableCode = pOffer[j].pItem->m_byTableCode;
    unsigned __int8 moneyKind[16]{};
    int basePrice = GetItemStdPrice(tableCode, itemIndex, byRace, moneyKind);
    unsigned int value = static_cast<unsigned int>(pOffer[j].byAmount) * basePrice / 2;
    if (IsAbrItem(pOffer[j].pItem->m_byTableCode, pOffer[j].pItem->m_wItemIndex))
    {
      const int itemDurPoint = GetItemDurPoint(pOffer[j].pItem->m_byTableCode, pOffer[j].pItem->m_wItemIndex);
      const unsigned __int64 dur = pOffer[j].pItem->m_dwDur;
      float durRatio = static_cast<float>(static_cast<int>(dur)) / static_cast<float>(itemDurPoint);
      if (durRatio > FLOAT_1_0)
      {
        durRatio = FLOAT_1_0;
      }
      const unsigned int durScaled = static_cast<unsigned int>(durRatio * 10000.0f);
      value = static_cast<unsigned int>(static_cast<unsigned __int64>(durScaled) * value / 10000);
    }

    if (IsItemEquipCivil(tableCode, itemIndex, 2 * byRace))
    {
      value = static_cast<unsigned int>(static_cast<unsigned __int64>(texRate) * value / 10000);
    }

    if (pOffer[j].pItem->m_byTableCode == 17)
    {
      value = static_cast<unsigned int>(static_cast<unsigned __int64>(oreRateScaled) * value / 10000);
    }

    if (!moneyKind[0]
      || moneyKind[0] == 2
      || moneyKind[0] == 4
      || moneyKind[0] == 5
      || moneyKind[0] == 6)
    {
      m_dwLastTradeDalant += value;
    }
    else if (moneyKind[0] == 1 || moneyKind[0] == 3)
    {
      m_dwLastTradeGold += value;
    }
  }

  if (fDiscountRate > 0.0f && m_dwLastTradeDalant > 1)
  {
    if (fDiscountRate > 1.0f)
    {
      fDiscountRate = FLOAT_1_0;
    }
    const int rateScaled = static_cast<int>(fDiscountRate * 10000.0f);
    const unsigned __int64 addValue = static_cast<unsigned __int64>(rateScaled) * m_dwLastTradeDalant;
    const unsigned int current = m_dwLastTradeDalant;
    m_dwLastTradeDalant = static_cast<unsigned int>(addValue / 10000 + current);
  }
return 0;
}

float CItemStore::CalcBuyPrice(unsigned __int8 byTableCode, unsigned __int16 wItemIndex, unsigned __int8 *pbyMoneyUnit)
{
  int multi = 2;
  if (m_byNpcRaceCode != 255)
  {
    const unsigned __int8 raceSex = static_cast<unsigned __int8>(2 * m_byNpcRaceCode);
    if (!IsItemEquipCivil(byTableCode, wItemIndex, raceSex))
    {
      multi = 2;
    }
  }
  const int npcRace = m_byNpcRaceCode;
  const float price = static_cast<float>(GetItemStdPrice(byTableCode, wItemIndex, npcRace, pbyMoneyUnit));
  return price / static_cast<float>(multi);
}

unsigned int CItemStore::CalcSellPrice(int nGoodIndex, unsigned __int8 *pbyMoneyUnit)
{
  *pbyMoneyUnit = m_pStorageItem[nGoodIndex].byMoneyUnit;
  switch (m_pStorageItem[nGoodIndex].byMoneyUnit)
  {
    case 2:
    case 3:
      return static_cast<unsigned int>(m_pStorageItem[nGoodIndex].nStdPoint);
    case 4:
      return static_cast<unsigned int>(m_pStorageItem[nGoodIndex].nResPoint);
    case 5:
      return static_cast<unsigned int>(m_pStorageItem[nGoodIndex].nKillPoint);
    case 6:
      return static_cast<unsigned int>(m_pStorageItem[nGoodIndex].nGoldPoint);
    default:
      return static_cast<unsigned int>(m_pStorageItem[nGoodIndex].nStdPrice);
  }
}

void CItemStore::SetZeroTradeMoney()
{
  m_dwLastTradePoint = 0;
  std::memset(m_dwLastTradeActPoint, 0, sizeof(m_dwLastTradeActPoint));
  m_dwLastTradeDalant = 0;
  m_dwLastTradeGold = 0;
}

unsigned int CItemStore::GetLastTradeDalant()
{
  return m_dwLastTradeDalant;
}

unsigned int CItemStore::GetLastTradeGold()
{
  return m_dwLastTradeGold;
}

unsigned int CItemStore::GetLastTradePoint()
{
  return m_dwLastTradePoint;
}

unsigned int CItemStore::GetLastTradeActPoint(int nActPoint)
{
  return m_dwLastTradeActPoint[nActPoint];
}

void CItemStore::SubLimitItemNum(int nLimitItemIndex, int nSubNum)
{
  if (nLimitItemIndex < m_nLimitStorageItemNum)
  {
    m_pLimitStorageItem[nLimitItemIndex].nLimitNum -= nSubNum;
    if (m_pLimitStorageItem[nLimitItemIndex].nLimitNum < 0)
    {
      m_pLimitStorageItem[nLimitItemIndex].nLimitNum = 0;
    }
  }
}

void CItemStore::UpdateLimitItemNum(bool bUpdate)
{
  m_bUpdate = bUpdate;
}
