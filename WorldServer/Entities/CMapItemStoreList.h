#pragma once

#include "IdaCompat.h"

class CItemStore;
class CMapData;

class  CMapItemStoreList
{
public:
  CMapItemStoreList();
  ~CMapItemStoreList();

  bool m_bUse;
  unsigned __int8 m_byType;
  int m_nSerial;
  int m_nItemStoreNum;
  CItemStore *m_ItemStore;

  void SetTypeNSerial(unsigned __int8 byType, int nSerial);
  char CopyItemStoreData(CMapItemStoreList *pDest);
  bool CreateStores(CMapData *pMap);
  bool SetItemStores(CMapData *pMap);
  CItemStore *GetItemStoreFromRecIndex(unsigned int dwRecIndex);
};
