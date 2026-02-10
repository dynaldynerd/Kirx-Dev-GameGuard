#include "pch.h"

#include "CItemStoreManager.h"
#include "CItemStore.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <new>
#include <atltime.h>
#include "CRFWorldDatabase.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

CItemStoreManager *CItemStoreManager::Instance()
{
  static CItemStoreManager instance;
  return &instance;
}

bool CItemStoreManager::Init(int nNormalListNum, int nInstanceListNum)
{
  if (nNormalListNum <= 0)
    return false;

  if (this->m_MapItemStoreList)
  {
    delete[] this->m_MapItemStoreList;
    this->m_MapItemStoreList = nullptr;
  }

  this->m_nMapItemStoreListNum = nNormalListNum;
  this->m_MapItemStoreList = new (std::nothrow) CMapItemStoreList[nNormalListNum];

  char pszErrMsg[144]{};
  if (this->m_tblItemStore.ReadRecord(".\\Script\\StoreList.dat", 0x37CC, pszErrMsg))
  {
    if (nInstanceListNum > 0)
    {
      if (this->m_InstanceItemStoreList)
      {
        delete[] this->m_InstanceItemStoreList;
        this->m_InstanceItemStoreList = nullptr;
      }
      this->m_nInstanceItemStoreListNum = nInstanceListNum;
      this->m_InstanceItemStoreList = new (std::nothrow) CMapItemStoreList[nInstanceListNum];
    }

    if (InitLogger())
    {
      SetNextEnforceInitTime();
      m_tmrCheckTime.BeginTimer(60000);
      m_tmrSaveTime.BeginTimer(180000);
      return true;
    }
  }
  else
  {
    MyMessageBox("CItemStoreManager::Init() : StorList.dat Read Fail!", pszErrMsg);
  }
  return false;
}

bool CItemStoreManager::InitLogger()
{
  CreateDirectoryA("..\\ZoneServerLog\\Systemlog\\ItemStoreSystem", nullptr);
  this->m_pkLogger = new (std::nothrow) CLogFile();
  if (this->m_pkLogger)
  {
    char Buffer[144];
    unsigned int KorLocalTime = GetKorLocalTime();
    sprintf_s(Buffer, "..\\ZoneServerLog\\Systemlog\\ItemStoreSystem\\ItemStoreSystem%d.log", KorLocalTime);
    this->m_pkLogger->SetWriteLogFile(Buffer, 1, 0, 1, 1);
    return true;
  }
  g_Main.m_logLoadingError.Write("CItemStoreManager::InitLogger() NULL == new CLogFile!\r\n");
  return false;
}

void CItemStoreManager::SetNextEnforceInitTime()
{
  CTime current = CTime::GetCurrentTime();
  CTime nextDay = current + CTimeSpan(1, 0, 0, 0);
  CTime nextMidnight(nextDay.GetYear(), nextDay.GetMonth(), nextDay.GetDay(), 0, 0, 0);
  this->m_tNextInitTime = nextMidnight.GetTime();
}

CMapItemStoreList *CItemStoreManager::GetMapItemStoreListByNum(int nMapNum)
{
  if (nMapNum < 0 || nMapNum >= m_nMapItemStoreListNum)
    return nullptr;
  return &m_MapItemStoreList[nMapNum];
}

CMapItemStoreList *CItemStoreManager::GetMapItemStoreListBySerial(int nSerial)
{
  for (int index = 0; index < m_nMapItemStoreListNum; ++index)
  {
    CMapItemStoreList *storeList = &m_MapItemStoreList[index];
    if (storeList->m_nSerial == nSerial)
      return storeList;
  }
  return nullptr;
}

CItemStore *CItemStoreManager::GetMapItemStoreFromList(int nMapNum, int nStoreNum)
{
  CMapItemStoreList *pList = GetMapItemStoreListByNum(nMapNum);
  if (!pList || nStoreNum < 0 || nStoreNum >= pList->m_nItemStoreNum)
    return nullptr;
  return &pList->m_ItemStore[nStoreNum];
}


void _qry_case_all_store_limit_item::__list::init()
{
  std::memset(this, 0, sizeof(*this));
  for (int j = 0; j < 16; ++j)
  {
    ItemData[j].Key.SetRelease();
  }
}

void _qry_case_all_store_limit_item::DataInit()
{
  dwCount = 0;
  for (unsigned int j = 0; j < dwMax; ++j)
  {
    pStoreList[j].init();
  }
}

void CItemStoreManager::Log(char *fmt, ...)
{
  if (!m_pkLogger)
  {
    return;
  }

  va_list va;
  va_start(va, fmt);
  m_pkLogger->WriteFromArg(fmt, va);
  va_end(va);
}

unsigned __int8 CItemStoreManager::UpdateStoreLimitItem()
{
  char query[2052]{};
  std::memset(query, 0, 2048);

  for (unsigned int j = 0; j < m_Sheet.dwCount; ++j)
  {
    _qry_case_all_store_limit_item::__list *entry = &m_Sheet.pStoreList[j];
    if (entry->dwDBSerial)
    {
      MakeLimitItemUpdateQuery(
        entry->dwDBSerial,
        entry->byType,
        entry->nTypeSerial,
        entry->dwStoreIndex,
        entry->ItemData,
        entry->dwLimitInitTime,
        query,
        2048);
      if (!g_Main.m_pWorldDB->Update_LimitItemNum(query))
      {
        entry->byRet = 3;
      }
    }
    else
    {
      unsigned int dbSerial[7]{};
      unsigned __int8 dbRet = g_Main.m_pWorldDB->Select_LimitItemUsedRecord(
        entry->byType,
        entry->nTypeSerial,
        entry->dwStoreIndex,
        dbSerial);
      if (dbRet == 1)
      {
        entry->byRet = 4;
        continue;
      }
      if (!dbSerial[0])
      {
        dbRet = g_Main.m_pWorldDB->Select_LimitItemEmptyRecord(dbSerial);
        if (dbRet == 1)
        {
          entry->byRet = 1;
          continue;
        }
        if (!dbSerial[0] && !g_Main.m_pWorldDB->Insert_LimitItemRecord(dbSerial))
        {
          entry->byRet = 2;
          continue;
        }
      }

      MakeLimitItemUpdateQuery(
        dbSerial[0],
        entry->byType,
        entry->nTypeSerial,
        entry->dwStoreIndex,
        entry->ItemData,
        entry->dwLimitInitTime,
        query,
        2048);
      if (g_Main.m_pWorldDB->Update_LimitItemNum(query))
      {
        m_Sheet.pStoreList[j].bNewSerial = true;
        m_Sheet.pStoreList[j].dwDBSerial = dbSerial[0];
      }
      else
      {
        entry->byRet = 3;
      }
    }
  }

  return 0;
}

unsigned __int8 CItemStoreManager::UpdateDisableInstanceStore(char *pData)
{
  for (int j = 0; j < *reinterpret_cast<unsigned int *>(pData); ++j)
  {
    char *entry = reinterpret_cast<char *>(*reinterpret_cast<unsigned long long *>(pData + 8) + 8LL * j);
    if (*reinterpret_cast<unsigned int *>(entry + 4))
    {
      bool updated = g_Main.m_pWorldDB->Update_DisableInstanceStore(*reinterpret_cast<unsigned int *>(entry + 4));
      entry[0] = updated ? 1 : 0;
    }
  }

  return 0;
}

void CItemStoreManager::CompleteStoreLimitItem()
{
  for (unsigned int j = 0; j < m_Sheet.dwCount; ++j)
  {
    _qry_case_all_store_limit_item::__list *entry = &m_Sheet.pStoreList[j];
    if (entry->byRet)
    {
      Log(
        "CItemStoreManager::CompleteStoreLimitItem\r\n\t\tStore Limit Item DBUpdate Proc(ErrorCode:%d) Fail!\r\n",
        entry->byRet);
    }
    else if (entry->bNewSerial)
    {
      CItemStore *store = nullptr;
      CMapItemStoreList *storeList = nullptr;
      if (entry->byType)
      {
        if (entry->byType == 1)
        {
          storeList = GetInstanceStoreListBySerial(entry->nTypeSerial);
        }
      }
      else
      {
        storeList = GetMapItemStoreListBySerial(entry->nTypeSerial);
      }
      if (storeList)
      {
        store = storeList->GetItemStoreFromRecIndex(entry->dwStoreIndex);
        if (store)
        {
          store->m_dwDBSerial = entry->dwDBSerial;
        }
        else
        {
          Log(
            "CItemStoreManager::CompleteStoreLimitItem\r\n"
            "\t\t Store Limit Item NewSerial Set Fail! (StoreType:%d)(Serial:%d)\r\n",
            entry->byType,
            entry->nTypeSerial);
        }
      }
      else
      {
        Log(
          "CItemStoreManager::CompleteStoreLimitItem\r\n"
          "\t\t Store Limit Item NewSerial Set Fail! (StoreType:%d)(Serial:%d)\r\n",
          entry->byType,
          entry->nTypeSerial);
      }
    }
  }

  m_Sheet.DataInit();
}

void CItemStoreManager::CompleteDisableInstanceStore(char *pData)
{
  for (int j = 0; j < *reinterpret_cast<unsigned int *>(pData); ++j)
  {
    char *entry = reinterpret_cast<char *>(*reinterpret_cast<unsigned long long *>(pData + 8) + 8LL * j);
    if (!entry[0])
    {
      Log(
        "CItemStoreManager::CompleteDisableInstanceStore\r\n\t\tInstanceStore Disable Fail! (DBSerial:%d)\r\n",
        *reinterpret_cast<unsigned int *>(entry + 4));
    }
  }

  if (*reinterpret_cast<int *>(pData) > 0)
  {
    operator delete[](*reinterpret_cast<void **>(pData + 8));
  }
}
