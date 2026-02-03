#include "pch.h"
#include "CMapItemStoreList.h"
#include "CItemStore.h"
#include "CMapData.h"

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
