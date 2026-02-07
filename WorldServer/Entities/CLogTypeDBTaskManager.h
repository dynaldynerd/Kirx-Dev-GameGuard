#pragma once

#include "IdaCompat.h"

#include <vector>

#include "CNetIndexList.h"

class CLogFile;
class CRFWorldDatabase;

enum LOG_TYPE_DB_TASK_TYPE : int
{
  lt_qry_case_none = -1,
  lt_qry_case_insert_settlementowner_log = 0,
  lt_qry_case_unmandtrader_select_list = 1,
  lt_qry_case_rename_potion_log = 2,
  lt_qry_case_max = 3,
};

class __cppobj CLogTypeDBTask
{
public:
  CLogTypeDBTask();
  ~CLogTypeDBTask();

  bool Init(unsigned int dwInx, unsigned int uiSize);
  void Clear();
  bool Set(unsigned __int8 byQueryType, char *pcData, unsigned __int16 wSize);
  void SetUse();
  void SetEmpty();
  void SetComplete();
  char *GetData();
  int GetQueryType();
  unsigned int GetInx();
  unsigned __int8 GetDBRet();
  unsigned __int8 GetProcRet();
  void SetRet(unsigned __int8 byDBRet, unsigned __int8 byProcRet);

public:
  unsigned int m_dwInx;
  bool m_bUse;
  bool m_bLoad;
  LOG_TYPE_DB_TASK_TYPE m_eQueryType;
  unsigned __int8 m_byDBQueryRet;
  unsigned __int8 m_byProcRet;
  char *m_pcData;
};

class __cppobj CLogTypeDBTaskPool
{
public:
  CLogTypeDBTaskPool();
  ~CLogTypeDBTaskPool();

  bool Init(unsigned int uiBuffSize, unsigned int uiMaxCnt, CLogFile *kLogger);
  void Destroy();
  CLogTypeDBTask *GetProc();
  CLogTypeDBTask *GetEmpty();
  CLogTypeDBTask *GetComplete();
  bool SetProc(CLogTypeDBTask *pTask, CLogFile *kLogger);
  bool SetEmpty(CLogTypeDBTask *pTask, CLogFile *kLogger);
  bool SetComplete(CLogTypeDBTask *pTask, CLogFile *kLogger);

public:
  bool m_bInit;
  CNetIndexList m_kInxProc;
  CNetIndexList m_kInxEmpty;
  CNetIndexList m_kInxComplete;
  std::vector<CLogTypeDBTask *> m_vecDat;
};

class __cppobj CLogTypeDBTaskManager
{
public:
  enum INIT_STATE : int
  {
    LTDTM_NONE = -1,
    LTDTM_INIT = 0,
    LTDTM_DB_INIT = 1,
  };

  CLogTypeDBTaskManager();
  ~CLogTypeDBTaskManager();

  static CLogTypeDBTaskManager *Instance();
  static void Destroy();

  bool Init();
  bool InitLogger();
  bool InitDB(const char *szDBName, const char *szDBIP);
  void Loop();
  void DBProcess();
  static void ProcThread(CLogTypeDBTaskManager *pParam);
  void ProcComplete();
  bool Push(unsigned __int8 byQueryType, char *pcData, unsigned __int16 wSize);
  void Log(const char *fmt, ...);
  bool GetDBProc();
  bool GetDBTaskConnectionStatus();
  bool GetDBTaskDataStatus();
  bool IsInitialized();
  void CleanUp();

public:
  INIT_STATE m_eState;
  bool m_bDBProc;
  CRFWorldDatabase *m_pkWorldDB;
  CLogTypeDBTaskPool m_kPool;
  CLogFile *m_pkLogger;

  static CLogTypeDBTaskManager *ms_Instance;
};
