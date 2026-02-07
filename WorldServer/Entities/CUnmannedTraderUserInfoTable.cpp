#include "pch.h"

#include "CUnmannedTraderUserInfoTable.h"
#include "GlobalObjects.h"

#include <algorithm>

CUnmannedTraderUserInfoTable *CUnmannedTraderUserInfoTable::ms_Instance = nullptr;

CUnmannedTraderUserInfoTable::CUnmannedTraderUserInfoTable()
  : m_pkLogger(nullptr), m_pkServiceLogger(nullptr), m_veckInfo()
{
}

CUnmannedTraderUserInfoTable *CUnmannedTraderUserInfoTable::Instance()
{
  if (!CUnmannedTraderUserInfoTable::ms_Instance)
  {
    CUnmannedTraderUserInfoTable::ms_Instance = new CUnmannedTraderUserInfoTable();
  }
  return CUnmannedTraderUserInfoTable::ms_Instance;
}

bool CUnmannedTraderUserInfoTable::Init()
{
  CUnmannedTraderUserInfo info;
  this->m_veckInfo.assign(0x9E4u, info);
  if (this->m_veckInfo.size() != 0x9E4u)
  {
    return false;
  }

  for (unsigned short index = 0; index < 0x9E4u; ++index)
  {
    CUnmannedTraderUserInfo &entry = this->m_veckInfo[index];
    if (!entry.Init(index))
    {
      return false;
    }
  }
  return true;
}

void CUnmannedTraderUserInfoTable::SetLogger(CLogFile *pkLogger, CLogFile *pkServiceLogger)
{
  this->m_pkLogger = pkLogger;
  this->m_pkServiceLogger = pkServiceLogger;
}

CUnmannedTraderUserInfo *CUnmannedTraderUserInfoTable::Find(unsigned int dwSerial)
{
  const auto endIt = this->m_veckInfo.end();
  const auto it = std::find(this->m_veckInfo.begin(), endIt, dwSerial);
  if (it == endIt)
  {
    return &CUnmannedTraderUserInfo::ms_kNull;
  }
  return &(*it);
}

CUnmannedTraderUserInfo *CUnmannedTraderUserInfoTable::FindByIndex(unsigned __int16 wInx)
{
  if (this->m_veckInfo.size() > wInx)
  {
    return &this->m_veckInfo[wInx];
  }
  return nullptr;
}

CUnmannedTraderUserInfo *CUnmannedTraderUserInfoTable::FindUser(unsigned __int16 wInx, unsigned int dwSerial)
{
  CUnmannedTraderUserInfo *user = FindByIndex(wInx);
  if (user && user->operator==(dwSerial))
  {
    return user;
  }
  return Find(dwSerial);
}

void CUnmannedTraderUserInfoTable::CompleteSearch(
  unsigned __int8 byDBRet,
  unsigned __int8 byProcRet,
  char *pLoadData)
{
  CUnmannedTraderUserInfo *user =
    FindUser(*reinterpret_cast<unsigned __int16 *>(pLoadData), *reinterpret_cast<unsigned int *>(pLoadData + 4));
  if (!user->IsNull() && g_Player[user->GetIndex()].m_bOper)
  {
    user->ClearRequest();
    CPlayer *owner = user->FindOwner();
    if (owner)
    {
      if (byDBRet || byProcRet)
      {
        user->SendSearchErrorResult(owner->m_ObjID.m_wIndex, byProcRet);
      }
      else
      {
        user->SendSearchResult(owner->m_ObjID.m_wIndex, pLoadData);
      }
    }
  }
}
