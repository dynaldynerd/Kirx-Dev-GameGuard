#pragma once

#include "IdaCompat.h"
#include "USArray.h"

#include "StorageList.h"

class CMapData;
struct _base_fld;

class  CLuaLooting_Novus_Item
{
public:
  class _State
  {
  public:
    bool m_bExist;

    _State();
    bool operator==(const _State &rhs) const;

    static _State ms_cEmpty;
  };

  CLuaLooting_Novus_Item();
  ~CLuaLooting_Novus_Item();

  _STORAGE_LIST::_db_con m_Item;
  CMapData *m_pMap;
  unsigned __int16 m_wLayerIndex;
  unsigned __int8 m_byCreateType;
  float m_fLootPos[3];
  unsigned __int16 m_wLootRange;
  unsigned int m_dwLootCount;
};

class  CLuaLootingMgr
{
public:
  static CLuaLootingMgr *Instance();
  static void Destroy();

  bool InitSDM(unsigned int dwLoopLootingCount, unsigned int dwLoopLootingTerm);
  bool AddNovusItem(
    const char *strItemCode,
    CMapData *pMap,
    unsigned __int16 wLayerIndex,
    float *fPos,
    unsigned __int16 wLootRange,
    unsigned int dwOverlapCnt,
    unsigned int dwItemNum,
    unsigned __int8 byCreateType);
  void Loop();

private:
  CLuaLootingMgr();
  ~CLuaLootingMgr();

  static CLuaLootingMgr *ms_Instance;

  US::CArrayEx<CLuaLooting_Novus_Item, CLuaLooting_Novus_Item::_State> m_Loot_Novus_ItemArEx;
  unsigned int m_dwLoopLootingCount;
  unsigned int m_dwLoopLootingTerm;
  unsigned int m_dwAddNodeCount;
  unsigned int m_dwNextLootingTime;
};
