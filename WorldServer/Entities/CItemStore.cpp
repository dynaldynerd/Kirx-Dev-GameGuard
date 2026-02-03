#include "pch.h"

#include "CItemStore.h"

#include <atltime.h>
#include <ctime>

#include "CMapData.h"
#include "CRecordData.h"
#include "DummyPosition.h"
#include "GlobalObjects.h"
#include "StoreList_fld.h"
#include "WorldServerUtil.h"

_good_storage_info::_good_storage_info()
{
  memset_0(this, 0, sizeof(_good_storage_info));
}

void _limit_item_info::init()
{
  memset_0(this, 0, sizeof(_limit_item_info));
}

_limit_item_info::_limit_item_info()
{
  init();
}

CItemStore::CItemStore()
{
  memset_0(this, 0, sizeof(CItemStore));
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

        int v22 = 0;
        for (int j = 0; j < 16; ++j)
        {
          int nTableCode = GetItemTableCode(m_pRec->m_sellLimitList[j].m_strItemCode);
          if (nTableCode == -1)
          {
            break;
          }
          m_pStorageItem[j].byItemTableCode = static_cast<unsigned __int8>(nTableCode);
          _base_fld *Record =
            g_Main.m_tblItemData[nTableCode].GetRecord(m_pRec->m_sellLimitList[j].m_strItemCode);
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
          m_pStorageItem[j].bExist = static_cast<signed char>(Record[1].m_dwIndex);
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
          ++v22;
        }
        if (m_nLimitStorageItemNum > 0)
        {
          SetLimitItemInitTime();
        }
        for (int j = v22; j < 216; ++j)
        {
          int nTableCode = GetItemTableCode(m_pRec->m_strItemlist[j]);
          if (nTableCode == -1)
          {
            break;
          }
          m_pStorageItem[j].byItemTableCode = static_cast<unsigned __int8>(nTableCode);
          _base_fld *v25 = g_Main.m_tblItemData[nTableCode].GetRecord(m_pRec->m_strItemlist[j]);
          if (!v25)
          {
            MyMessageBox(
              "CItemStore::Init error",
              "%d Store %s Item can't find index",
              nIndex,
              m_pRec->m_strItemlist[j]);
            break;
          }
          m_pStorageItem[j].wItemIndex = static_cast<unsigned __int16>(v25->m_dwIndex);
          int v14 = GetItemStdPrice(nTableCode, v25->m_dwIndex, m_byNpcRaceCode, &m_pStorageItem[j].byMoneyUnit);
          m_pStorageItem[j].nStdPrice = v14;
          int v15 = GetItemStdPoint(nTableCode, v25->m_dwIndex, m_byNpcRaceCode, &m_pStorageItem[j].byMoneyUnit);
          m_pStorageItem[j].nStdPoint = v15;
          int v16 = GetItemProcPoint(
            nTableCode,
            v25->m_dwIndex,
            m_byNpcRaceCode,
            &m_pStorageItem[j].byMoneyUnit);
          m_pStorageItem[j].nResPoint = v16;
          int v17 = GetItemKillPoint(
            nTableCode,
            v25->m_dwIndex,
            m_byNpcRaceCode,
            &m_pStorageItem[j].byMoneyUnit);
          m_pStorageItem[j].nKillPoint = v17;
          int v18 = GetItemGoldPoint(
            nTableCode,
            v25->m_dwIndex,
            m_byNpcRaceCode,
            &m_pStorageItem[j].byMoneyUnit);
          m_pStorageItem[j].nGoldPoint = v18;
          m_pStorageItem[j].nStdPrice *= m_pRec->m_nPriceSet;
          m_pStorageItem[j].dwUpCode = m_pRec->m_nItemUpCode;
          if (m_pStorageItem[j].nStdPrice == -1)
          {
            MyMessageBox(
              "CItemStore::Init error",
              "price load error..(nTable: %d, pFld->m_dwIndex: %d)",
              nTableCode,
              v25->m_dwIndex);
            break;
          }
          m_pStorageItem[j].bExist = static_cast<signed char>(v25[1].m_dwIndex);
          unsigned int v19 = GetItemDurPoint(nTableCode, v25->m_dwIndex);
          m_pStorageItem[j].dwDurPoint = v19;
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
  _base_fld *Record = g_Main.m_tblNPC.GetRecord(m_pRec->m_strStore_NPCcode);
  if (!Record)
    return false;
  *pbyRaceCode = static_cast<unsigned __int8>(Record[1].m_strCode[60]);
  return true;
}

unsigned int CItemStore::CalcSecIndex(float x, float z)
{
  _bsp_info *BspInfo = &m_pExistMap->m_BspInfo;
  float v9 = static_cast<float>(-BspInfo->m_nMapMinSize[0]) + x;
  float v10 = static_cast<float>(BspInfo->m_nMapMaxSize[2]) - z;
  int v11 = static_cast<int>(v9) / 100;
  int v12 = static_cast<int>(v10) / 100;
  _sec_info *SecInfo = &m_pExistMap->m_SecInfo;
  return static_cast<unsigned int>(SecInfo->m_nSecNumW * v12 + v11);
}

void CItemStore::SetLimitItemInitTime()
{
  unsigned __int64 v9 = 60000LL * m_pRec->m_nLimitItem_InitTime;
  int lDays = static_cast<int>(v9 / 0x5265C00);
  __int64 nowSec = static_cast<__int64>(time(nullptr));

  if (lDays > 0)
  {
    CTimeSpan span(lDays, 0, 0, 0);
    CTime base = CTime::GetCurrentTime();
    CTime target = base + span;
    CTime midnight(target.GetYear(), target.GetMonth(), target.GetDay(), 0, 0, 0);
    m_dwLimitInitTime = midnight.GetTime();
  }
  else
  {
    m_dwLimitInitTime = v9 + nowSec;
  }
}
