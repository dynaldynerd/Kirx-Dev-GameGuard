#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CMapItemStoreList.h"
#include "CMyTimer.h"
#include "CPlayer.h"
#include "CRecordData.h"

struct _limit_item_db_data
{
  _INVENKEY Key;
  int nLimitNum;
};

struct _qry_case_all_store_limit_item
{
  struct __list
  {
    unsigned int dwDBSerial;
    bool bNewSerial;
    unsigned __int8 byType;
    int nTypeSerial;
    unsigned int dwStoreIndex;
    unsigned __int64 dwLimitInitTime;
    _limit_item_db_data ItemData[16];
    unsigned __int8 byRet;
  };

  unsigned int dwMax;
  unsigned int dwCount;
  __list *pStoreList;
};

/* 5781 */
class __cppobj CItemStoreManager
{
public:
  static CItemStoreManager *Instance();
  bool Init(int nMapNum, int nInstanceListNum);
  bool InitLogger();
  void SetNextEnforceInitTime();

  CMapItemStoreList *GetMapItemStoreListByNum(int nMapNum);
  CItemStore *GetMapItemStoreFromList(int nMapNum, int nStoreNum);

  CLogFile *m_pkLogger;
  CMyTimer m_tmrCheckTime;
  CMyTimer m_tmrSaveTime;
  __int64 m_tNextInitTime;
  _qry_case_all_store_limit_item m_Sheet;
  int m_nInstanceItemStoreListNum;
  CMapItemStoreList *m_InstanceItemStoreList;
  CRecordData m_tblItemStore;
  int m_nMapItemStoreListNum;
  CMapItemStoreList *m_MapItemStoreList;
  CItemStore *m_pLimitInitNormalStore[3];
};
