#pragma once

#include <windows.h>

#include "EdfFile.h"

class CItemDataMgr
{
public:
  CItemDataMgr();
  ~CItemDataMgr();

  BOOL Create(void);
  BOOL Destroy(void);

  BOOL LoadData(void);
  void UnloadData(void);
  BOOL IsLoaded(void) const;

private:
  BOOL m_bCreated;
  BOOL m_bLoaded;

  CEdfFile m_cItemData;
  CEdfFile m_cItemCashData;
  CEdfFile m_cItemCombineData;
  CEdfFile m_cItemPremiumData;
  CEdfFile m_cItemSetEffectData;
  CEdfFile m_cItemTimerData;

  CEdfFile m_cNationItemData;
  CEdfFile m_cNationItemCashData;
};

extern CItemDataMgr *g_pItemDataMgr;

#define _GetItemDataMgr() g_pItemDataMgr
