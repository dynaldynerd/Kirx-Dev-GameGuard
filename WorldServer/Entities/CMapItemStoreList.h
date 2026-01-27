#pragma once

#include "IdaCompat.h"

class CItemStore;

class __cppobj CMapItemStoreList
{
public:
  bool m_bUse;
  unsigned __int8 m_byType;
  int m_nSerial;
  int m_nItemStoreNum;
  CItemStore *m_ItemStore;

  void SetTypeNSerial(unsigned __int8 byType, int nSerial);
  bool CreateStores(CMapData *pMap);
};
