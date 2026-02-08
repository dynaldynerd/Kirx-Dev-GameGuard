#include "pch.h"

#include "CUnmannedTraderUserInfo.h"
#include "CNetworkEX.h"
#include "CPlayer.h"
#include "CLogFile.h"
#include "GlobalObjects.h"
#include "InvenKey.h"
#include "WorldServerUtil.h"

#include <cstring>

CUnmannedTraderUserInfo CUnmannedTraderUserInfo::ms_kNull;

CUnmannedTraderUserInfo::CUnmannedTraderUserInfo()
  : m_eState(LOG_IN_STATE::UTUI_NONE),
    m_wInx(static_cast<unsigned short>(-1)),
    m_dwUserSerial(0),
    m_byRegistCnt(0),
    m_byMaxRegistCnt(0),
    m_kRequestState(),
    m_vecRegistItemInfo(),
    m_vecLoadItemInfo()
{
  Clear();
}

bool CUnmannedTraderUserInfo::Init(unsigned short wInx)
{
  this->m_wInx = wInx;

  CUnmannedTraderRegistItemInfo regInfo;
  this->m_vecRegistItemInfo.assign(10u, regInfo);
  if (this->m_vecRegistItemInfo.size() != 10u)
  {
    return false;
  }

  CUnmannedTraderRegistItemInfo loadInfo;
  this->m_vecLoadItemInfo.assign(20u, loadInfo);
  if (this->m_vecLoadItemInfo.size() != 20u)
  {
    return false;
  }

  this->m_kRequestState.ClearRequset();
  this->m_eState = LOG_IN_STATE::UTUI_EMPTY;
  return true;
}

void CUnmannedTraderUserInfo::Clear()
{
  if (IsNull())
  {
    return;
  }

  for (CUnmannedTraderRegistItemInfo &item : this->m_vecRegistItemInfo)
  {
    item.Clear();
  }

  ClearLoadItemInfo();
  this->m_dwUserSerial = 0;
  this->m_byRegistCnt = 0;
}

void CUnmannedTraderUserInfo::ClearLoadItemInfo()
{
  for (CUnmannedTraderRegistItemInfo &item : this->m_vecLoadItemInfo)
  {
    item.Clear();
  }
}

bool CUnmannedTraderUserInfo::IsNull()
{
  return this == &CUnmannedTraderUserInfo::ms_kNull;
}

void CUnmannedTraderUserInfo::ClearRequest()
{
  this->m_kRequestState.ClearRequset();
}

CPlayer *CUnmannedTraderUserInfo::FindOwner()
{
  CPlayer *player = nullptr;
  if (g_Player[this->m_wInx].m_id.dwSerial == this->m_dwUserSerial && g_Player[this->m_wInx].m_bLive)
  {
    player = &g_Player[this->m_wInx];
  }
  else
  {
    CPlayer *candidate = GetPtrPlayerFromSerial(g_Player, MAX_PLAYER, this->m_dwUserSerial);
    if (candidate && candidate->m_bOper)
    {
      player = candidate;
    }
  }

  return player ? player : nullptr;
}

unsigned __int16 CUnmannedTraderUserInfo::GetIndex()
{
  return this->m_wInx;
}

bool CUnmannedTraderUserInfo::IsLogInState()
{
  return this->m_eState == LOG_IN_STATE::UTUI_LOGIN;
}

unsigned int CUnmannedTraderUserInfo::GetSerial()
{
  return this->m_dwUserSerial;
}

unsigned __int8 CUnmannedTraderUserInfo::GetMaxRegistCnt()
{
  return this->m_byMaxRegistCnt;
}

const CUnmannedTraderRegistItemInfo *CUnmannedTraderUserInfo::GetRegItemInfo()
{
  if (IsNull() || this->m_vecRegistItemInfo.empty())
  {
    return nullptr;
  }

  return &this->m_vecRegistItemInfo[0];
}

void CUnmannedTraderUserInfo::SendSearchErrorResult(unsigned __int16 wInx, char byRet)
{
  char msg[532];
  msg[0] = byRet;

  unsigned __int8 pbyType[2]{};
  pbyType[0] = 30;
  pbyType[1] = 33;
  g_Network.m_pProcess[0]->LoadSendMsg(wInx, pbyType, msg, 0x1F3u);
}

void CUnmannedTraderUserInfo::LogOut(unsigned int dwSerial, CLogFile *pkLogger)
{
  if (this->m_dwUserSerial != dwSerial && pkLogger)
  {
    pkLogger->Write(
      "CUnmannedTraderUserInfo::LogOut( DWORD dwSerial )\r\n\t\tm_wInx(%u) m_dwSerial(%u) != dwSerial(%u)\r\n",
      this->m_wInx,
      this->m_dwUserSerial,
      dwSerial);
  }
  Clear();
  this->m_eState = LOG_IN_STATE::UTUI_EMPTY;
}

void CUnmannedTraderUserInfo::SendSearchResult(unsigned __int16 wInx, char *pLoadData)
{
  char *reqData = pLoadData;
  char msg[0x1F3];
  msg[0] = 0;

  const int totalCount = *reinterpret_cast<int *>(reqData + 12);
  std::memcpy(msg + 1, &totalCount, sizeof(totalCount));
  msg[5] = reqData[16];
  msg[6] = reqData[17];
  msg[7] = reqData[18];
  msg[8] = reqData[19];
  const int reserved = *reinterpret_cast<int *>(reqData + 20);
  std::memcpy(msg + 9, &reserved, sizeof(reserved));
  msg[13] = reqData[24];
  const int pageCount = *reinterpret_cast<int *>(reqData + 156);
  std::memcpy(msg + 14, &pageCount, sizeof(pageCount));
  const unsigned __int8 itemCount = static_cast<unsigned __int8>(reqData[160]);
  msg[18] = itemCount;

  __time64_t now{};
  time_18(reinterpret_cast<__int64 *>(&now));

  _INVENKEY key;
  for (unsigned int index = 0; index < itemCount; ++index)
  {
    const unsigned int dataOffset = 72 * index;
    char *item = msg + 19 + 48 * index;

    *reinterpret_cast<unsigned int *>(item) = *reinterpret_cast<unsigned int *>(reqData + dataOffset + 168);
    key.LoadDBKey(*reinterpret_cast<int *>(reqData + dataOffset + 172));
    item[4] = static_cast<char>(key.byTableCode);
    *reinterpret_cast<unsigned __int16 *>(item + 5) = key.wItemIndex;
    *reinterpret_cast<unsigned __int64 *>(item + 7) =
      *reinterpret_cast<unsigned __int64 *>(reqData + dataOffset + 176);
    *reinterpret_cast<unsigned int *>(item + 15) = *reinterpret_cast<unsigned int *>(reqData + dataOffset + 184);
    *reinterpret_cast<unsigned int *>(item + 19) = *reinterpret_cast<unsigned int *>(reqData + dataOffset + 188);
    *reinterpret_cast<unsigned int *>(item + 23) = *reinterpret_cast<unsigned int *>(reqData + dataOffset + 192);

    const char *source = reqData[dataOffset + 196] ? reqData + dataOffset + 196 : "NULL";
    strcpy_s(item + 27, 0x11u, source);

    const __int64 regTime =
      *reinterpret_cast<__int64 *>(reqData + dataOffset + 216) +
      3600 * static_cast<unsigned __int8>(reqData[dataOffset + 224]);
    *reinterpret_cast<unsigned int *>(item + 44) = static_cast<unsigned int>(regTime - now);
  }

  unsigned __int8 pbyType[2]{};
  pbyType[0] = 30;
  pbyType[1] = 33;
  g_Network.m_pProcess[0]->LoadSendMsg(wInx, pbyType, msg, 0x1F3u);
}

bool CUnmannedTraderUserInfo::operator==(unsigned int dwSerial)
{
  return this->m_dwUserSerial && this->m_dwUserSerial == dwSerial;
}

CUnmannedTraderUserInfo &CUnmannedTraderUserInfo::operator=(const CUnmannedTraderUserInfo &rhs)
{
  this->m_eState = rhs.m_eState;
  this->m_wInx = rhs.m_wInx;
  this->m_dwUserSerial = rhs.m_dwUserSerial;
  this->m_byRegistCnt = rhs.m_byRegistCnt;
  this->m_byMaxRegistCnt = rhs.m_byMaxRegistCnt;
  this->m_kRequestState.m_eState = rhs.m_kRequestState.m_eState;
  this->m_vecRegistItemInfo = rhs.m_vecRegistItemInfo;
  this->m_vecLoadItemInfo = rhs.m_vecLoadItemInfo;
  return *this;
}
