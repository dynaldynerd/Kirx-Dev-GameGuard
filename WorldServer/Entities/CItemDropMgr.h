#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CMapData.h"
#include "CNetIndexList.h"

struct _base_fld;

struct  _DropItemGroupInfo
{
  struct  _drop_create_info
  {
    unsigned __int8 byTableCode;
    _base_fld *pFld;
    unsigned int dwDur;
    unsigned __int8 byCreateCode;
    CMapData *pMap;
    unsigned __int16 wLayerIndex;
    float fDropPos[3];
    int nDropRange;
  };

  _drop_create_info m_DropItem;
  unsigned int m_dwDropCount;
};

/* 5913 */
class  CItemDropMgr
{
  _DropItemGroupInfo m_Pool[333];
  CNetIndexList m_listEmpty;
  CNetIndexList m_listTask;
  unsigned int m_dwTotalDropCount;
  CLogFile *m_pLogFile;
};

