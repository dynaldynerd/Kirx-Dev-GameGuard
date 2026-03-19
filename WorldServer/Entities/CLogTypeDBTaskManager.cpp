#include "pch.h"

#include "CLogTypeDBTaskManager.h"

#include <cstdarg>
#include <new>
#include <process.h>

#include "CLogFile.h"
#include "CPotionMgr.h"
#include "CRFWorldDatabase.h"
#include "CUnmannedTraderController.h"
#include "CWeeklyGuildRankManager.h"
#include "CNationSettingManager.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "lt_qry_case_unmandtrader_select_list.h"

CLogTypeDBTaskManager *CLogTypeDBTaskManager::ms_Instance;
std::atomic<bool> CLogTypeDBTaskManager::s_bDBProcDestroy = false;
std::mutex CLogTypeDBTaskManager::s_mtxDestroy;

CLogTypeDBTask::CLogTypeDBTask()
  : m_dwInx(255),
    m_bUse(false),
    m_bLoad(false),
    m_eQueryType(lt_qry_case_none),
    m_byDBQueryRet(static_cast<unsigned __int8>(-1)),
    m_byProcRet(static_cast<unsigned __int8>(-1)),
    m_pcData(nullptr)
{
  Clear();
}

CLogTypeDBTask::~CLogTypeDBTask()
{
  if (m_pcData)
  {
    delete[] m_pcData;
    m_pcData = nullptr;
  }
  m_dwInx = 255;
  Clear();
}

bool CLogTypeDBTask::Init(unsigned int dwInx, unsigned int uiSize)
{
  if (!uiSize || dwInx == static_cast<unsigned int>(-1))
  {
    return false;
  }

  m_pcData = new (std::nothrow) char[uiSize];
  if (!m_pcData)
  {
    return false;
  }

  m_dwInx = dwInx;
  return true;
}

void CLogTypeDBTask::Clear()
{
  m_bUse = false;
  m_bLoad = false;
  m_eQueryType = lt_qry_case_none;
  m_byDBQueryRet = static_cast<unsigned __int8>(-1);
  m_byProcRet = static_cast<unsigned __int8>(-1);
}

bool CLogTypeDBTask::Set(unsigned __int8 byQueryType, char *pcData, unsigned __int16 wSize)
{
  if (byQueryType >= lt_qry_case_max || !pcData || wSize > 1024)
  {
    return false;
  }

  std::memcpy(m_pcData, pcData, wSize);
  m_eQueryType = static_cast<LOG_TYPE_DB_TASK_TYPE>(byQueryType);
  return true;
}

void CLogTypeDBTask::SetUse()
{
  m_bUse = true;
}

void CLogTypeDBTask::SetEmpty()
{
  m_bUse = false;
  m_bLoad = false;
}

void CLogTypeDBTask::SetComplete()
{
  m_bLoad = true;
}

char *CLogTypeDBTask::GetData()
{
  return m_pcData;
}

int CLogTypeDBTask::GetQueryType()
{
  return static_cast<int>(m_eQueryType);
}

unsigned int CLogTypeDBTask::GetInx()
{
  return m_dwInx;
}

unsigned __int8 CLogTypeDBTask::GetDBRet()
{
  return m_byDBQueryRet;
}

unsigned __int8 CLogTypeDBTask::GetProcRet()
{
  return m_byProcRet;
}

void CLogTypeDBTask::SetRet(unsigned __int8 byDBRet, unsigned __int8 byProcRet)
{
  m_byDBQueryRet = byDBRet;
  m_byProcRet = byProcRet;
}

CLogTypeDBTaskPool::CLogTypeDBTaskPool()
  : m_bInit(false),
    m_kInxProc(),
    m_kInxEmpty(),
    m_kInxComplete(),
    m_vecDat()
{
}

CLogTypeDBTaskPool::~CLogTypeDBTaskPool()
{
  Destroy();
}

bool CLogTypeDBTaskPool::Init(unsigned int uiBuffSize, unsigned int uiMaxCnt, CLogFile *kLogger)
{
  if (!uiBuffSize || !uiMaxCnt)
  {
    if (kLogger)
    {
      kLogger->Write(
        "CLogTypeDBTaskPool::Init(...) : 0 == uiBuffSize(%u) || 0 == uiMaxCnt(%u) Invalid!",
        uiBuffSize,
        uiMaxCnt);
    }
    return false;
  }

  if (!m_kInxProc.SetList(uiMaxCnt) && kLogger)
  {
    kLogger->Write("CLogTypeDBTaskPool::Init(...) : m_kInxProc.SetList( uiMaxCnt(%u) ) Fail!", uiMaxCnt);
  }

  if (!m_kInxEmpty.SetList(uiMaxCnt) && kLogger)
  {
    kLogger->Write("CLogTypeDBTaskPool::Init(...) : m_kInxEmpty.SetList( uiMaxCnt(%u) ) Fail!", uiMaxCnt);
  }

  for (unsigned int dwIndex = 0; dwIndex < uiMaxCnt; ++dwIndex)
  {
    m_kInxEmpty.PushNode_Back(dwIndex);
  }

  if (uiMaxCnt != m_kInxEmpty.m_dwCount)
  {
    if (kLogger)
    {
      kLogger->Write(
        "CLogTypeDBTaskPool::Init(...) : ( uiMaxCnt(%u) != m_kInxEmpty.m_dwCount(%u) ) Invalid!",
        uiMaxCnt,
        m_kInxEmpty.m_dwCount);
    }
    return false;
  }

  if (!m_kInxComplete.SetList(uiMaxCnt) && kLogger)
  {
    kLogger->Write("CLogTypeDBTaskPool::Init(...) : m_kInxComplete.SetList( uiMaxCnt(%u) ) Fail!", uiMaxCnt);
  }

  m_vecDat.reserve(uiMaxCnt);
  for (unsigned int dwInx = 0; dwInx < uiMaxCnt; ++dwInx)
  {
    CLogTypeDBTask *task = new (std::nothrow) CLogTypeDBTask();
    m_vecDat.push_back(task);
    if (!task)
    {
      if (kLogger)
      {
        kLogger->Write(
          "CLogTypeDBTaskPool::Init(...) : MaxCnt(%u) BuffSize(%u) m_vecDat.back() NULL!",
          uiMaxCnt,
          uiBuffSize);
      }
      return false;
    }

    if (!task->Init(dwInx, uiBuffSize))
    {
      if (kLogger)
      {
        kLogger->Write(
          "CLogTypeDBTaskPool::Init(...) : MaxCnt(%u) m_vecDat.back()->Init( i(%u), uiBuffSize(%u) ) Fail!",
          uiMaxCnt,
          dwInx,
          uiBuffSize);
      }
      return false;
    }
  }

  m_bInit = true;
  return true;
}

void CLogTypeDBTaskPool::Destroy()
{
  if (m_vecDat.empty())
  {
    return;
  }

  for (CLogTypeDBTask *task : m_vecDat)
  {
    delete task;
  }
  m_vecDat.clear();
}

CLogTypeDBTask *CLogTypeDBTaskPool::GetProc()
{
  if (!m_bInit)
  {
    return nullptr;
  }

  unsigned int outIndex = static_cast<unsigned int>(-1);
  if (!m_kInxProc.PopNode_Front(&outIndex))
  {
    return nullptr;
  }

  if (m_vecDat.size() > outIndex)
  {
    return m_vecDat[outIndex];
  }

  return nullptr;
}

CLogTypeDBTask *CLogTypeDBTaskPool::GetEmpty()
{
  if (!m_bInit)
  {
    return nullptr;
  }

  unsigned int outIndex = static_cast<unsigned int>(-1);
  if (!m_kInxEmpty.PopNode_Front(&outIndex))
  {
    return nullptr;
  }

  if (m_vecDat.size() <= outIndex)
  {
    return nullptr;
  }

  m_vecDat[outIndex]->Clear();
  return m_vecDat[outIndex];
}

CLogTypeDBTask *CLogTypeDBTaskPool::GetComplete()
{
  if (!m_bInit)
  {
    return nullptr;
  }

  unsigned int outIndex = static_cast<unsigned int>(-1);
  if (!m_kInxComplete.PopNode_Front(&outIndex))
  {
    return nullptr;
  }

  if (m_vecDat.size() > outIndex)
  {
    return m_vecDat[outIndex];
  }

  return nullptr;
}

bool CLogTypeDBTaskPool::SetProc(CLogTypeDBTask *pTask, CLogFile *kLogger)
{
  if (!m_bInit || !pTask)
  {
    return false;
  }

  const unsigned int index = pTask->GetInx();
  if (m_kInxProc.PushNode_Back(index))
  {
    pTask->SetUse();
    return true;
  }

  if (kLogger)
  {
    kLogger->Write(
      "CLogTypeDBTaskPool::SetProc(...) : QueryType(%d) m_kInxProc.PushNode_Back( pTask->GetInx()(%u) ) Fail!",
      pTask->GetQueryType(),
      pTask->GetInx());
  }
  return false;
}

bool CLogTypeDBTaskPool::SetEmpty(CLogTypeDBTask *pTask, CLogFile *kLogger)
{
  if (!m_bInit || !pTask)
  {
    return false;
  }

  const unsigned int index = pTask->GetInx();
  if (m_kInxEmpty.PushNode_Back(index))
  {
    pTask->SetEmpty();
    return true;
  }

  if (kLogger)
  {
    kLogger->Write(
      "CLogTypeDBTaskPool::SetEmpty(...) : QueryType(%d) m_kInxEmpty.PushNode_Back( pTask->GetInx()(%u) ) Fail!",
      pTask->GetQueryType(),
      pTask->GetInx());
  }
  return false;
}

bool CLogTypeDBTaskPool::SetComplete(CLogTypeDBTask *pTask, CLogFile *kLogger)
{
  if (!m_bInit || !pTask)
  {
    return false;
  }

  const unsigned int index = pTask->GetInx();
  if (m_kInxComplete.PushNode_Back(index))
  {
    pTask->SetComplete();
    return true;
  }

  if (kLogger)
  {
    kLogger->Write(
      "CLogTypeDBTaskPool::SetComplete(...) : QueryType(%d) m_kInxComplete.PushNode_Back( pTask->GetInx()(%u) ) Fail!",
      pTask->GetQueryType(),
      pTask->GetInx());
  }
  return false;
}

CLogTypeDBTaskManager::CLogTypeDBTaskManager()
  : m_eState(LTDTM_NONE),
    m_bDBProc(true),
    m_pkWorldDB(nullptr),
    m_kPool(),
    m_pkLogger(nullptr)
{
  // Yorozuya fix (non-IDA parity): reset stop flag for ProcThread.
  s_bDBProcDestroy = false;
}

CLogTypeDBTaskManager::~CLogTypeDBTaskManager()
{
  CleanUp();
}

CLogTypeDBTaskManager *CLogTypeDBTaskManager::Instance()
{
  if (!CLogTypeDBTaskManager::ms_Instance)
  {
    CLogTypeDBTaskManager::ms_Instance = new (std::nothrow) CLogTypeDBTaskManager();
  }
  return CLogTypeDBTaskManager::ms_Instance;
}

void CLogTypeDBTaskManager::Destroy()
{
  if (CLogTypeDBTaskManager::ms_Instance)
  {
    delete CLogTypeDBTaskManager::ms_Instance;
    CLogTypeDBTaskManager::ms_Instance = nullptr;
  }
}

bool CLogTypeDBTaskManager::Init()
{
  if (!InitLogger())
  {
    return false;
  }

  m_pkWorldDB = new (std::nothrow) CRFWorldDatabase();
  if (!m_pkWorldDB)
  {
    Log("CLogTypeDBTaskManager::Init() : new CRFWorldDatabase NULL!");
    return false;
  }

  if (!m_kPool.Init(1024, 5064, m_pkLogger))
  {
    Log("CLogTypeDBTaskManager::Init() : m_kPool.Init( %u, %u )!", 1024u, 5064u);
    return false;
  }

  if (_beginthread(reinterpret_cast<_beginthread_proc_type>(CLogTypeDBTaskManager::ProcThread), 0, this) == -1)
  {
    Log("CLogTypeDBTaskManager::Init() : _beginthread Fail!");
    return false;
  }

  m_eState = LTDTM_INIT;
  return true;
}

bool CLogTypeDBTaskManager::InitLogger()
{
  m_pkLogger = new (std::nothrow) CLogFile();
  if (!m_pkLogger)
  {
    return false;
  }

  const unsigned int korLocalTime = GetKorLocalTime();
  char dest[128]{};
  sprintf_s(dest, sizeof(dest), "..\\ZoneServerLog\\Systemlog\\LogTypeDBTaskError%u.log", korLocalTime);
  m_pkLogger->SetWriteLogFile(dest, 1, 0, 1, 1);
  return true;
}

bool CLogTypeDBTaskManager::InitDB(const char *szDBName, const char *szDBIP)
{
  m_pkWorldDB->SetLogFile("..\\ZoneServerLog\\", szDBName);
  if (m_eState || !szDBName || !szDBIP)
  {
    return false;
  }

  char trustedConnectionValue[16]{};
  ReadOptionAndWriteDefault(
    ".\\Initialize\\Database.ini",
    "WorldDB",
    "trusted_connection",
    "0",
    trustedConnectionValue,
    sizeof(trustedConnectionValue));
  const bool bTrustedConnection = trustedConnectionValue[0] != '\0' && trustedConnectionValue[0] != '0';

  CNationSettingManager *settings = CNationSettingManager::Instance();
  const char *passWord = bTrustedConnection ? nullptr : (settings ? settings->GetWorldDBPW() : nullptr);
  const char *worldDBID = bTrustedConnection ? nullptr : (settings ? settings->GetWorldDBID() : nullptr);

  if (m_pkWorldDB
      && m_pkWorldDB->StartDataBase(g_Main.m_szWorldDBName, worldDBID, passWord))
  {
    g_Main.m_logLoadingError.Write("Log Type Start World DataBase Complete!!");
    m_eState = LTDTM_DB_INIT;
    return true;
  }

  MyMessageBox("CLogTypeDBTaskManager::InitDB()", "Connect World DB Failed!");
  return false;
}

void CLogTypeDBTaskManager::Loop()
{
  if (m_eState == LTDTM_DB_INIT)
  {
    ProcComplete();
    m_pkWorldDB->CheckLogFileHour();
  }
}

void CLogTypeDBTaskManager::ProcThread(CLogTypeDBTaskManager *pParam)
{
  CLogTypeDBTaskManager *manager = pParam;
  int loopCount = 0;
  while (manager->GetDBProc() && !s_bDBProcDestroy.load())
  {
    {
      std::unique_lock<std::mutex> lock(s_mtxDestroy);
      if (s_bDBProcDestroy.load())
      {
        break;
      }
      manager->DBProcess();
    }
    if (++loopCount > 100)
    {
      Sleep(1u);
      loopCount = 0;
    }
  }
  _endthread();
}

void CLogTypeDBTaskManager::DBProcess()
{
  CLogTypeDBTask *pTask = nullptr;
  unsigned __int8 dbRet = 0;
  unsigned __int8 procRet = 0;
  bool noDataError = false;

  while (true)
  {
    pTask = m_kPool.GetProc();
    if (!pTask)
    {
      break;
    }

    const int queryType = pTask->GetQueryType();
    if (queryType)
    {
      if (queryType == 1)
      {
        char *data = pTask->GetData();
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        dbRet = controller->SelectSearchList(
          reinterpret_cast<_lt_qry_case_unmandtrader_select_list *>(data),
          m_pkWorldDB,
          &procRet);
      }
      else if (queryType == 2)
      {
        char *data = pTask->GetData();
        if (!g_PotionMgr.InsertRenamePotion(m_pkWorldDB, data))
        {
          dbRet = 1;
        }
      }
    }
    else
    {
      char *data = pTask->GetData();
      CWeeklyGuildRankManager *rankManager = CWeeklyGuildRankManager::Instance();
      if (!rankManager->InsertSettlementOwner(m_pkWorldDB, data))
      {
        dbRet = 1;
      }
    }

    if (pTask->GetQueryType() == -1 || dbRet == 1 || (noDataError && dbRet == 2))
    {
      const char *retNames[] = {"db_result_sql_error", "db_result_sql_success", "db_result_no_data"};
      const char *retName = nullptr;
      if (pTask->GetQueryType() == -1)
      {
        retName = "lt_qry_case_none";
      }
      else
      {
        retName = retNames[dbRet];
      }
      Log("CLogTypeDBTaskManager::DBProcess() : case: %d,  ret: %s Fail!", pTask->GetQueryType(), retName);
    }

    pTask->SetRet(dbRet, procRet);
    m_kPool.SetComplete(pTask, m_pkLogger);
  }
}

void CLogTypeDBTaskManager::ProcComplete()
{
  CLogTypeDBTask *pTask = nullptr;
  while (true)
  {
    pTask = m_kPool.GetComplete();
    if (!pTask)
    {
      break;
    }

    const int queryType = pTask->GetQueryType();
    if (queryType == 1)
    {
      char *data = pTask->GetData();
      const unsigned __int8 procRet = pTask->GetProcRet();
      const unsigned __int8 dbRet = pTask->GetDBRet();
      CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
      controller->CompleteSelectSearchList(
        dbRet,
        procRet,
        reinterpret_cast<_lt_qry_case_unmandtrader_select_list *>(data));
    }

    m_kPool.SetEmpty(pTask, m_pkLogger);
  }
}

bool CLogTypeDBTaskManager::Push(unsigned __int8 byQueryType, char *pcData, unsigned __int16 wSize)
{
  CLogTypeDBTask *pTask = m_kPool.GetEmpty();
  if (!pTask)
  {
    Log(
      "CLogTypeDBTaskManager::Push( BYTE byQueryType(%u), BYTE * pcData, WORD wSize(%u) ) : m_kPool.GetEmpty() NULL!",
      byQueryType,
      wSize);

    char buffer[1040]{};
    sprintf_s(buffer, sizeof(buffer),
      "CLogTypeDBTaskManager::Push( BYTE byQueryType(%u), char * pcData, WORD wSize(%u) ) : m_kPool.GetEmpty() is NULL!",
      byQueryType,
      wSize);
    ServerProgramExit(buffer, 1);
  }

  if (pTask->Set(byQueryType, pcData, wSize))
  {
    return m_kPool.SetProc(pTask, m_pkLogger);
  }

  Log(
    "CLogTypeDBTaskManager::Push( BYTE byQueryType(%u), BYTE * pcData, WORD wSize(%u) ) : pkTask->Set( byQueryType, pcData"
    ", wSize ) Fail!",
    byQueryType,
    wSize);
  return false;
}

void CLogTypeDBTaskManager::Log(const char *fmt, ...)
{
  if (!m_pkLogger)
  {
    return;
  }

  va_list args;
  va_start(args, fmt);
  m_pkLogger->WriteFromArg(fmt, args);
  va_end(args);
}

bool CLogTypeDBTaskManager::GetDBProc()
{
  return m_bDBProc;
}

bool CLogTypeDBTaskManager::GetDBTaskConnectionStatus()
{
  return m_pkWorldDB && m_pkWorldDB->IsConectionActive();
}

bool CLogTypeDBTaskManager::GetDBTaskDataStatus()
{
  return m_pkWorldDB != nullptr;
}

bool CLogTypeDBTaskManager::IsInitialized()
{
  return m_eState == LTDTM_DB_INIT;
}

void CLogTypeDBTaskManager::CleanUp()
{
  // Yorozuya fix (non-IDA parity): stop DB thread before tearing down resources.
  {
    std::unique_lock<std::mutex> lock(s_mtxDestroy);
    s_bDBProcDestroy = true;
  }
  m_bDBProc = false;

  if (m_pkWorldDB)
  {
    delete m_pkWorldDB;
    m_pkWorldDB = nullptr;
  }

  if (m_pkLogger)
  {
    delete m_pkLogger;
    m_pkLogger = nullptr;
  }
}
