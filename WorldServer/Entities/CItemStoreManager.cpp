#include "pch.h"

#include "CItemStoreManager.h"
#include "CItemStore.h"
#include "StoreList_fld.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <new>
#include <atltime.h>
#include "CRFWorldDatabase.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

CItemStoreManager *CItemStoreManager::ms_Instance = nullptr;

CItemStoreManager *CItemStoreManager::Instance()
{
  if (!ms_Instance)
  {
    ms_Instance = new CItemStoreManager();
  }
  return ms_Instance;
}

void CItemStoreManager::Destroy()
{
  if (ms_Instance)
  {
    delete ms_Instance;
    ms_Instance = nullptr;
  }
}

_limit_item_db_data::_limit_item_db_data()
{
}

_qry_case_all_store_limit_item::__list::__list()
{
  init();
}

_qry_case_all_store_limit_item::_qry_case_all_store_limit_item()
  : dwMax(0),
    dwCount(0),
    pStoreList(nullptr)
{
}

_qry_case_all_store_limit_item::~_qry_case_all_store_limit_item()
{
  if (pStoreList)
  {
    operator delete[](pStoreList);
    pStoreList = nullptr;
  }
}

CItemStoreManager::CItemStoreManager()
{
  m_nInstanceItemStoreListNum = 0;
  m_InstanceItemStoreList = nullptr;
  m_nMapItemStoreListNum = 0;
  m_MapItemStoreList = nullptr;
}

CItemStoreManager::~CItemStoreManager()
{
  if (m_MapItemStoreList)
  {
    delete[] m_MapItemStoreList;
    m_MapItemStoreList = nullptr;
  }

  if (m_InstanceItemStoreList)
  {
    delete[] m_InstanceItemStoreList;
    m_InstanceItemStoreList = nullptr;
  }
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
  if (this->m_tblItemStore.ReadRecord(".\\Script\\StoreList.dat", 14284, pszErrMsg))
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

char CItemStoreManager::Load()
{
  unsigned int storeNum = 0;
  for (unsigned int j = 0; static_cast<int>(j) < this->m_nMapItemStoreListNum; ++j)
  {
    for (int k = 0; k < this->m_MapItemStoreList[j].m_nItemStoreNum; ++k)
    {
      if (this->m_MapItemStoreList[j].m_ItemStore[k].m_nLimitStorageItemNum > 0)
      {
        ++storeNum;
      }
    }
  }

  unsigned int usedNum = 0;
  if (!SelectUsedRecordNum(&usedNum))
  {
    return 0;
  }

  storeNum += 90;
  if (storeNum)
  {
    if (storeNum < usedNum)
    {
      storeNum = usedNum;
    }

    unsigned int totalNum = 0;
    if (!SelectTotalRecordNum(&totalNum))
    {
      return 0;
    }

    if (storeNum > totalNum && !InsertNotEnoughLimitItemRecord(storeNum - totalNum))
    {
      return 0;
    }

    if (!this->m_Sheet.Init(storeNum))
    {
      return 0;
    }

    if (!SelectStoreLimitItem())
    {
      return 0;
    }

    for (unsigned int j = 0; j < this->m_Sheet.dwCount; ++j)
    {
      SetStoreLimitItemData(&this->m_Sheet.pStoreList[j]);
    }

    SetEnforceInitNormalStore();
  }

  SetUpdateDBDataDoNotCheck();
  this->m_Sheet.DataInit();
  return 1;
}

bool CItemStoreManager::InitLogger()
{
  CreateDirectoryA("..\\ZoneServerLog\\Systemlog\\ItemStoreSystem", nullptr);
  this->m_pkLogger = new (std::nothrow) CLogFile();
  if (this->m_pkLogger)
  {
    char Buffer[144];
    unsigned int KorLocalTime = GetKorLocalTime();
    sprintf_s(Buffer, "..\\ZoneServerLog\\Systemlog\\ItemStoreSystem\\ItemStoreSystem%u.log", KorLocalTime);
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

void CItemStoreManager::Loop()
{
  if (!m_tmrCheckTime.CountingTimer())
  {
    return;
  }

  __time64_t currentTime = 0;
  _time64(&currentTime);

  if (currentTime > m_tNextInitTime)
  {
    for (int normalIndex = 0; normalIndex < 3; ++normalIndex)
    {
      CItemStore *normalStore = m_pLimitInitNormalStore[normalIndex];
      if (!normalStore)
      {
        continue;
      }

      normalStore->SetLimitItemInitTime();
      if (normalStore->m_nLimitStorageItemNum > 0)
      {
        normalStore->InitLimitItemInfo();
        normalStore->UpdateLimitItemNum(true);
      }
    }

    for (int listIndex = 0; listIndex < m_nInstanceItemStoreListNum; ++listIndex)
    {
      CMapItemStoreList *storeList = &m_InstanceItemStoreList[listIndex];
      for (int storeIndex = 0; storeIndex < storeList->m_nItemStoreNum; ++storeIndex)
      {
        CItemStore *store = &storeList->m_ItemStore[storeIndex];
        store->SetLimitItemInitTime();
        if (store->m_nLimitStorageItemNum > 0)
        {
          store->InitLimitItemInfo();
          store->UpdateLimitItemNum(true);
        }
      }
    }

    SetNextEnforceInitTime();
  }

  for (int listIndex = 0; listIndex < m_nMapItemStoreListNum; ++listIndex)
  {
    CMapItemStoreList *storeList = &m_MapItemStoreList[listIndex];
    for (int storeIndex = 0; storeIndex < storeList->m_nItemStoreNum; ++storeIndex)
    {
      CItemStore *store = &storeList->m_ItemStore[storeIndex];
      if (store->m_nLimitStorageItemNum > 0 && store->m_dwLimitInitTime <= static_cast<unsigned __int64>(currentTime))
      {
        store->SetLimitItemInitTime();
        store->InitLimitItemInfo();
        store->UpdateLimitItemNum(true);
      }
    }
  }

  for (int listIndex = 0; listIndex < m_nInstanceItemStoreListNum; ++listIndex)
  {
    CMapItemStoreList *storeList = &m_InstanceItemStoreList[listIndex];
    if (!storeList->m_bUse)
    {
      continue;
    }

    for (int storeIndex = 0; storeIndex < storeList->m_nItemStoreNum; ++storeIndex)
    {
      CItemStore *store = &storeList->m_ItemStore[storeIndex];
      if (store->m_nLimitStorageItemNum > 0 && store->m_dwLimitInitTime <= static_cast<unsigned __int64>(currentTime))
      {
        store->SetLimitItemInitTime();
        store->InitLimitItemInfo();
        store->UpdateLimitItemNum(true);
      }
    }
  }

  if (!m_tmrSaveTime.CountingTimer())
  {
    return;
  }

  unsigned int updateCount = 0;
  bool hasUpdate = false;
  auto copyUpdatedStore = [&](CMapItemStoreList *storeList)
  {
    if (!storeList->m_bUse)
    {
      return;
    }

    for (int storeIndex = 0; storeIndex < storeList->m_nItemStoreNum; ++storeIndex)
    {
      CItemStore *store = &storeList->m_ItemStore[storeIndex];
      if (!store->m_bUpdate || store->m_nLimitStorageItemNum <= 0)
      {
        continue;
      }

      _qry_case_all_store_limit_item::__list *sheetEntry = &m_Sheet.pStoreList[updateCount];
      sheetEntry->dwDBSerial = store->m_dwDBSerial;
      sheetEntry->bNewSerial = false;
      sheetEntry->byType = storeList->m_byType;
      sheetEntry->nTypeSerial = storeList->m_nSerial;
      sheetEntry->dwStoreIndex = store->m_pRec->m_dwIndex;
      sheetEntry->dwLimitInitTime = store->m_dwLimitInitTime;

      for (int itemIndex = 0; itemIndex < 16; ++itemIndex)
      {
        if (store->m_pLimitStorageItem[itemIndex].Key.CovDBKey() != -1)
        {
          sheetEntry->ItemData[itemIndex].Key = store->m_pLimitStorageItem[itemIndex].Key;
          sheetEntry->ItemData[itemIndex].nLimitNum = store->m_pLimitStorageItem[itemIndex].nLimitNum;
        }
      }

      ++updateCount;
      store->m_bUpdate = false;
      hasUpdate = true;
    }
  };

  for (int listIndex = 0; listIndex < m_nMapItemStoreListNum; ++listIndex)
  {
    copyUpdatedStore(&m_MapItemStoreList[listIndex]);
  }

  for (int listIndex = 0; listIndex < m_nInstanceItemStoreListNum; ++listIndex)
  {
    copyUpdatedStore(&m_InstanceItemStoreList[listIndex]);
  }

  if (hasUpdate && g_Main.m_pWorldDB)
  {
    m_Sheet.dwCount = updateCount;
    g_Main.PushDQSData(-1, nullptr, 112, nullptr, 0);
  }
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

void CItemStoreManager::MakeLimitItemUpdateQuery(
  unsigned int dwSerial,
  unsigned __int8 byStoreType,
  int nTypeSerial,
  unsigned int dwStoreIndex,
  _limit_item_db_data *pItemData,
  unsigned __int64 dwLimitInitTime,
  char *pszQuery,
  int nBufSize)
{
  char buffer[132]{};

  std::memset(buffer, 0, sizeof(buffer));
  sprintf_s(
    pszQuery,
    nBufSize,
    "Update tbl_StoreLimitItem_061212 set dck=0,type=%d,typeserial=%d,storeinx=%d,resettime=%I64d,",
    byStoreType,
    nTypeSerial,
    dwStoreIndex,
    dwLimitInitTime);

  for (int j = 0; j < 16; ++j)
  {
    int key = static_cast<int>(pItemData[j].Key.CovDBKey());
    sprintf_s(buffer, 128, "k%d=%d,num%d=%d,", j, key, j, pItemData[j].nLimitNum);
    strcat_s(pszQuery, nBufSize, buffer);
  }

  pszQuery[std::strlen(pszQuery) - 1] = 32;
  sprintf_s(buffer, 128, "where serial=%d", dwSerial);
  strcat_s(pszQuery, nBufSize, buffer);
  std::strlen(pszQuery);
}

CMapItemStoreList *CItemStoreManager::GetInstanceStoreListBySerial(int nSerial)
{
  for (int j = 0; j < m_nInstanceItemStoreListNum; ++j)
  {
    if (m_InstanceItemStoreList[j].m_bUse && m_InstanceItemStoreList[j].m_nSerial == nSerial)
    {
      return &m_InstanceItemStoreList[j];
    }
  }
  return nullptr;
}

CMapItemStoreList *CItemStoreManager::GetEmptyInstanceItemStore()
{
  for (int j = 0; j < this->m_nInstanceItemStoreListNum; ++j)
  {
    if (!this->m_InstanceItemStoreList[j].m_bUse)
    {
      return &this->m_InstanceItemStoreList[j];
    }
  }
  return nullptr;
}


void _qry_case_all_store_limit_item::__list::init()
{
  std::memset(this, 0, sizeof(*this));
  for (int j = 0; j < 16; ++j)
  {
    ItemData[j].Key.SetRelease();
  }
}

char _qry_case_all_store_limit_item::Init(unsigned int dwStoreNum)
{
  if (!dwStoreNum)
  {
    return 0;
  }

  if (pStoreList)
  {
    operator delete[](pStoreList);
  }

  pStoreList = new (std::nothrow) __list[dwStoreNum];
  if (!pStoreList)
  {
    return 0;
  }

  dwMax = dwStoreNum;
  dwCount = 0;
  DataInit();
  return 1;
}

void _qry_case_all_store_limit_item::DataInit()
{
  dwCount = 0;
  for (unsigned int j = 0; j < dwMax; ++j)
  {
    pStoreList[j].init();
  }
}

void CItemStoreManager::Log(const char *fmt, ...)
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

char CItemStoreManager::SelectUsedRecordNum(unsigned int *pdwUsedNum)
{
  if (g_Main.m_pWorldDB->Select_UsedLimitItemRecordNum(pdwUsedNum) != 1)
  {
    return 1;
  }

  Log(
    "CItemStoreManager::SelectUsedRecordNum\r\n\t\tg_Main.m_pWorldDB->Select_UsedLimitItemRecordNum() Fail!\r\n");
  return 0;
}

char CItemStoreManager::SelectTotalRecordNum(unsigned int *pdwTotalNum)
{
  if (g_Main.m_pWorldDB->Select_TotalRecordNum(pdwTotalNum) != 1)
  {
    return 1;
  }

  Log(
    "CItemStoreManager::SelectTotalRecordNum\r\n\t\tg_Main.m_pWorldDB->Select_TotalRecordNum() Fail!\r\n");
  return 0;
}

char CItemStoreManager::InsertNotEnoughLimitItemRecord(int nNum)
{
  unsigned int serial[4]{};
  for (int j = 0; j < nNum; ++j)
  {
    if (!g_Main.m_pWorldDB->Insert_LimitItemRecord(serial))
    {
      Log(
        "CItemStoreManager::InsertNotEnoughLimitItemRecord\r\n\t\tg_Main.m_pWorldDB->Insert_LimitItemRecord() Fail!\r\n");
      return 0;
    }
  }

  return 1;
}

char CItemStoreManager::SelectStoreLimitItem()
{
  if (g_Main.m_pWorldDB->Select_StoreLimitItem(&m_Sheet) != 1)
  {
    return 1;
  }

  Log(
    "CItemStoreManager::SelectStoreLimitItem\r\n\t\tg_Main.m_pWorldDB->Select_StoreLimitItem() Fail!\r\n");
  return 0;
}

void CItemStoreManager::SetStoreLimitItemData(_qry_case_all_store_limit_item::__list *pData)
{
  if (!pData || pData->nTypeSerial < 0)
  {
    return;
  }

  CMapItemStoreList *storeList = nullptr;
  if (pData->byType)
  {
    if (pData->byType == 1)
    {
      storeList = GetInstanceStoreListBySerial(pData->nTypeSerial);
    }
  }
  else
  {
    storeList = GetMapItemStoreListBySerial(pData->nTypeSerial);
  }

  if (!storeList)
  {
    return;
  }

  CItemStore *store = storeList->GetItemStoreFromRecIndex(pData->dwStoreIndex);
  if (!store)
  {
    return;
  }

  store->m_dwDBSerial = pData->dwDBSerial;
  if (pData->dwLimitInitTime)
  {
    store->m_dwLimitInitTime = pData->dwLimitInitTime;
  }
  else
  {
    store->UpdateLimitItemNum(true);
  }

  char changed = 0;
  for (int nIndex = 0; nIndex < 16; ++nIndex)
  {
    _limit_item_info *limitItem = store->GetLimitItem(nIndex);
    if (limitItem)
    {
      const int storeKey = static_cast<int>(limitItem->Key.CovDBKey());
      const int dbKey = static_cast<int>(pData->ItemData[nIndex].Key.CovDBKey());
      if (storeKey != dbKey)
      {
        changed = 1;
      }
      if (storeKey != -1 && storeKey == dbKey)
      {
        limitItem->nLimitNum = pData->ItemData[nIndex].nLimitNum;
      }
    }
  }

  if (changed)
  {
    store->UpdateLimitItemNum(true);
  }

  store->m_bDBDataCheck = true;
}

void CItemStoreManager::SetEnforceInitNormalStore()
{
  const char *codes[3] = {"030F1", "040F1", "050F1"};
  int found = 0;

  for (int j = 0; j < m_nMapItemStoreListNum && found < 3; ++j)
  {
    CMapItemStoreList *list = &m_MapItemStoreList[j];
    for (int k = 0; k < list->m_nItemStoreNum && found < 3; ++k)
    {
      CItemStore *store = &list->m_ItemStore[k];
      for (int codeIdx = found; codeIdx < 3; ++codeIdx)
      {
        if (!std::strcmp(codes[codeIdx], store->m_pRec->m_strStore_NPCcode))
        {
          m_pLimitInitNormalStore[found++] = store;
          break;
        }
      }
    }
  }
}

void CItemStoreManager::SetUpdateDBDataDoNotCheck()
{
  for (int j = 0; j < m_nMapItemStoreListNum; ++j)
  {
    for (int k = 0; k < m_MapItemStoreList[j].m_nItemStoreNum; ++k)
    {
      if (m_MapItemStoreList[j].m_ItemStore[k].m_nLimitStorageItemNum > 0
          && !m_MapItemStoreList[j].m_ItemStore[k].m_bDBDataCheck)
      {
        m_MapItemStoreList[j].m_ItemStore[k].UpdateLimitItemNum(true);
      }
    }
  }
}

char CItemStoreManager::ResetInstanceItemStore(unsigned __int8 byStoreType, int nSerial)
{
  if (byStoreType >= 2u)
  {
    return 0;
  }

  _qry_case_disable_instance_store query{};

  for (int j = 0; j < m_nInstanceItemStoreListNum; ++j)
  {
    CMapItemStoreList *list = &m_InstanceItemStoreList[j];
    if (list->m_bUse && list->m_byType == byStoreType && list->m_nSerial == nSerial)
    {
      list->m_bUse = false;
      if (list->m_nItemStoreNum > 0)
      {
        query.dwCount = list->m_nItemStoreNum;
        query.pEntry = static_cast<_qry_case_disable_instance_store_entry *>(
          operator new[](saturated_mul(query.dwCount, sizeof(_qry_case_disable_instance_store_entry))));
        std::memset(query.pEntry, 0, sizeof(_qry_case_disable_instance_store_entry) * list->m_nItemStoreNum);
      }

      int removed = 0;
      for (int k = 0; k < list->m_nItemStoreNum; ++k)
      {
        operator delete[](list->m_ItemStore[k].m_pStorageItem);
        operator delete[](list->m_ItemStore[k].m_pLimitStorageItem);
        list->m_ItemStore[k].m_pStorageItem = nullptr;
        list->m_ItemStore[k].m_pLimitStorageItem = nullptr;
        if (query.pEntry)
        {
          query.pEntry[k].dwDBSerial = list->m_ItemStore->m_dwDBSerial;
        }
        ++removed;
      }

      if (list->m_nItemStoreNum > 0)
      {
        delete[] list->m_ItemStore;
        list->m_nItemStoreNum -= removed;
        if (list->m_nItemStoreNum < 0)
        {
          list->m_nItemStoreNum = 0;
        }
      }

      list->m_ItemStore = nullptr;
      break;
    }
  }

  if (static_cast<int>(query.dwCount) <= 0)
  {
    return 0;
  }

  g_Main.PushDQSData(-1, nullptr, 113, reinterpret_cast<char *>(&query), sizeof(query));
  return 1;
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

unsigned __int8 CItemStoreManager::UpdateDisableInstanceStore(_qry_case_disable_instance_store *pData)
{
  for (unsigned int j = 0; j < pData->dwCount; ++j)
  {
    _qry_case_disable_instance_store_entry *entry = &pData->pEntry[j];
    if (entry->dwDBSerial)
    {
      const bool updated = g_Main.m_pWorldDB->Update_DisableInstanceStore(entry->dwDBSerial);
      entry->byResult = updated ? 1 : 0;
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

void CItemStoreManager::CompleteDisableInstanceStore(_qry_case_disable_instance_store *pData)
{
  for (unsigned int j = 0; j < pData->dwCount; ++j)
  {
    _qry_case_disable_instance_store_entry *entry = &pData->pEntry[j];
    if (!entry->byResult)
    {
      Log(
        "CItemStoreManager::CompleteDisableInstanceStore\r\n\t\tInstanceStore Disable Fail! (DBSerial:%d)\r\n",
        entry->dwDBSerial);
    }
  }

  if (static_cast<int>(pData->dwCount) > 0)
  {
    operator delete[](pData->pEntry);
  }
}
