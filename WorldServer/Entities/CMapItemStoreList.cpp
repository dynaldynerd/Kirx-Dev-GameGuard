#include "pch.h"
#include "CMapItemStoreList.h"
#include "CItemStore.h"
#include "CMapData.h"
#include "StoreList_fld.h"

#include <cstring>

CMapItemStoreList::CMapItemStoreList()
{
    m_bUse = false;
    m_byType = 0;
    m_nSerial = -1;
    m_nItemStoreNum = 0;
    m_ItemStore = nullptr;
}

CMapItemStoreList::~CMapItemStoreList()
{
    if (m_ItemStore)
    {
        delete[] m_ItemStore;
        m_ItemStore = nullptr;
    }
}

void CMapItemStoreList::SetTypeNSerial(unsigned __int8 byType, int nSerial)
{
    this->m_bUse = true;
    this->m_byType = byType;
    this->m_nSerial = nSerial;
}

char CMapItemStoreList::CopyItemStoreData(CMapItemStoreList *pDest)
{
    if (m_bUse || !pDest)
    {
        return 0;
    }

    m_nItemStoreNum = pDest->m_nItemStoreNum;
    if (m_nItemStoreNum <= 0)
    {
        return 0;
    }

    m_ItemStore = new CItemStore[m_nItemStoreNum];
    memcpy_s(m_ItemStore, 120ULL * m_nItemStoreNum, pDest->m_ItemStore, 120ULL * pDest->m_nItemStoreNum);

    for (int j = 0; j < m_nItemStoreNum; ++j)
    {
        m_ItemStore[j].m_bUpdate = true;
        m_ItemStore[j].m_dwDBSerial = 0;
        m_ItemStore[j].SetLimitItemInitTime();

        m_ItemStore[j].m_pLimitStorageItem = new _limit_item_info[16];
        m_ItemStore[j].m_pStorageItem = new _good_storage_info[216];

        memcpy_s(
            m_ItemStore[j].m_pLimitStorageItem,
            256,
            pDest->m_ItemStore[j].m_pLimitStorageItem,
            256);
        memcpy_s(
            m_ItemStore[j].m_pStorageItem,
            9616,
            pDest->m_ItemStore[j].m_pStorageItem,
            9616);
    }

    return 1;
}

bool CMapItemStoreList::CreateStores(CMapData *pMap)
{
    if (pMap->m_nItemStoreDumNum > 0)
    {
        m_nItemStoreNum = pMap->m_nItemStoreDumNum;
        m_ItemStore = new CItemStore[m_nItemStoreNum];
        return SetItemStores(pMap);
    }

    m_nItemStoreNum = 0;
    return true;
}

bool CMapItemStoreList::SetItemStores(CMapData *pMap)
{
    if (m_nItemStoreNum <= 0)
        return true;
    if (!pMap || !pMap->m_pItemStoreDummy)
        return false;

    for (int nIndex = 0; nIndex < m_nItemStoreNum; ++nIndex)
    {
        _store_dummy *pDum = &pMap->m_pItemStoreDummy[nIndex];
        m_ItemStore[nIndex].Init(
            nIndex,
            pMap,
            pDum,
            reinterpret_cast<_StoreList_fld *>(pDum->m_pStoreRec));
    }
    return true;
}

CItemStore *CMapItemStoreList::GetItemStoreFromRecIndex(unsigned int dwRecIndex)
{
    for (int j = 0; j < m_nItemStoreNum; ++j)
    {
        CItemStore *store = &m_ItemStore[j];
        if (store->m_pRec->m_dwIndex == dwRecIndex)
        {
            return store;
        }
    }
    return nullptr;
}
