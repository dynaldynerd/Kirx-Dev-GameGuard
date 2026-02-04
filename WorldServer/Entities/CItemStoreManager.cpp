#include "pch.h"

#include "CItemStoreManager.h"
#include "CItemStore.h"

#include <cstdio>
#include <cstring>
#include <new>
#include <atltime.h>
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

CItemStore *CItemStoreManager::GetMapItemStoreFromList(int nMapNum, int nStoreNum)
{
  CMapItemStoreList *pList = GetMapItemStoreListByNum(nMapNum);
  if (!pList || nStoreNum < 0 || nStoreNum >= pList->m_nItemStoreNum)
    return nullptr;
  return &pList->m_ItemStore[nStoreNum];
}
