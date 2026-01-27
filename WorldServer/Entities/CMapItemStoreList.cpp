#include "pch.h"
#include "CMapItemStoreList.h"

void CMapItemStoreList::SetTypeNSerial(unsigned __int8 byType, int nSerial)
{
    this->m_byType = byType;
    this->m_nSerial = nSerial;
}

bool CMapItemStoreList::CreateStores(CMapData *pMap)
{
    return true; // Stub
}
