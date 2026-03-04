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

  _limit_item_db_data();
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

    __list();
    void init();
  };

  unsigned int dwMax;
  unsigned int dwCount;
  __list *pStoreList;

  _qry_case_all_store_limit_item();
  ~_qry_case_all_store_limit_item();

  char Init(unsigned int dwStoreNum);
  void DataInit();
};

struct _StoreList_fld;

/* 5781 */
class  CItemStoreManager
{
public:
  CItemStoreManager();
  ~CItemStoreManager();

  static CItemStoreManager *ms_Instance;
  static CItemStoreManager *Instance();
  static void Destroy();
  bool Init(int nMapNum, int nInstanceListNum);
  void Loop();
  char Load();
  bool InitLogger();
  void SetNextEnforceInitTime();
  unsigned __int8 UpdateStoreLimitItem();
  unsigned __int8 UpdateDisableInstanceStore(char *pData);
  void CompleteStoreLimitItem();
  void CompleteDisableInstanceStore(char *pData);
  void Log(const char *fmt, ...);

  void MakeLimitItemUpdateQuery(
    unsigned int dwSerial,
    unsigned __int8 byStoreType,
    int nTypeSerial,
    unsigned int dwStoreIndex,
    _limit_item_db_data *pItemData,
    unsigned __int64 dwLimitInitTime,
    char *pszQuery,
    int nBufSize);
  char SelectUsedRecordNum(unsigned int *pdwUsedNum);
  char SelectTotalRecordNum(unsigned int *pdwTotalNum);
  char InsertNotEnoughLimitItemRecord(int nNum);
  char SelectStoreLimitItem();
  void SetStoreLimitItemData(_qry_case_all_store_limit_item::__list *pData);
  void SetEnforceInitNormalStore();
  void SetUpdateDBDataDoNotCheck();
  char ResetInstanceItemStore(unsigned __int8 byStoreType, int nSerial);
  CMapItemStoreList *GetMapItemStoreListBySerial(int nSerial);
  CMapItemStoreList *GetInstanceStoreListBySerial(int nSerial);
  CMapItemStoreList *GetEmptyInstanceItemStore();

  CMapItemStoreList *GetMapItemStoreListByNum(int nMapNum);
  CItemStore *GetMapItemStoreFromList(int nMapNum, int nStoreNum);
  bool IsStorePointerValid(const CItemStore *store) const;
  bool IsStoreRecordPointerValid(const _StoreList_fld *record) const;

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
