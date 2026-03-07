#include "pch.h"

#include "CNationSettingData.h"

#include "CNetworkEX.h"
#include "CPlayer.h"
#include "CUserDB.h"
#include "NameTxt_fld.h"
#include "Packet/ZoneAccountPacket.h"

#include <Windows.h>
#include <cwchar>
#include <cwctype>

CNationSettingData::CNationSettingData()
  : m_bServiceMode(false),
    m_iNationCode(-1),
    m_szNationCodeStr{0, 0, 0},
    m_iANSICodePage(0),
    m_szNoneString{},
    m_szCashDBName{},
    m_szCashDBIP{},
    m_szCashDBID{},
    m_szCashDBPW{},
    m_wCashDBPort(static_cast<unsigned __int16>(-4103)),
    m_eCashDBFlag(CDPS_NOT_REF),
    m_szWorldDBID{},
    m_szWorldDBPW{},
    m_wBillingForceCloseDelay(300),
    m_szVaildKey{},
    m_vecCheatData(),
    m_pGameGuardSystem(nullptr)
{
  strcpy_s(m_szNoneString, sizeof(m_szNoneString), "None");
  memset_0(m_szVaildKey, 0, sizeof(m_szVaildKey));
}

CNationSettingData::~CNationSettingData() = default;

int CNationSettingData::Init()
{
  // this is not a stub
  return 0;
}

void CNationSettingData::Loop()
{
  // this is not a stub
}

CashDbWorker *CNationSettingData::CreateWorker()
{
  // this is not a stub
  return nullptr;
}

CBilling *CNationSettingData::CreateBilling()
{
  // this is not a stub
  return nullptr;
}

const char *CNationSettingData::GetItemName(_NameTxt_fld *pFld)
{
// this is not a stub
  return nullptr;
}

bool CNationSettingData::ValidMacAddress()
{
  // this is not a stub
  return true;
}

bool CNationSettingData::IsNormalString(const char *szString)
{
  return IsNormalStringDefProc(szString, "[]");
}

bool CNationSettingData::IsNormalString(const wchar_t *wszString)
{
  return IsNormalStringDefProc(wszString, L"[]");
}

bool CNationSettingData::IsNormalChar(const wchar_t wcChar)
{
// this is not a stub
  return true;
}

void CNationSettingData::SetUnitPassiveValue(float *fUnitPv_DefFc)
{
  *fUnitPv_DefFc = 1.6f;
}

bool CNationSettingData::IsPersonalFreeFixedAmountBillingType(__int16 *pDest1, __int16 *pDest2)
{
// this is not a stub
  return false;
}

bool CNationSettingData::CheckEnterWorldRequest(int n, char *pBuf)
{
// this is not a stub
  return true;
}

void CNationSettingData::CreateComplete(CPlayer *pOne)
{
// this is not a stub
}

void CNationSettingData::NetClose(CPlayer *pOne)
{
// this is not a stub
}

int CNationSettingData::GetCashItemPrice(_CashShop_str_fld *pFld)
{
// this is not a stub
  return 0;
}

bool CNationSettingData::IsApplyPcbangPrimium(const CPlayer *pUser)
{
  if (!pUser)
  {
    return false;
  }
  return pUser->m_pUserDB && pUser->m_pUserDB->m_BillingInfo.IsPcBangType();
}

void CNationSettingData::SendCashDBDSNRequest()
{
  unsigned __int8 type[2] = {1, 31};
  _cashdb_setting_request_wrac msg{};
  const unsigned __int16 len = msg.size();
  g_Network.m_pProcess[1]->LoadSendMsg(0, type, reinterpret_cast<char *>(&msg), len);
}

bool CNationSettingData::ReadSystemPass()
{
  // this is not a stub
  return true;
}

char *CNationSettingData::GetNoneString()
{
  return m_szNoneString;
}

bool CNationSettingData::IsCashDBUseExtRef()
{
  return m_eCashDBFlag == CDPS_USE_REF;
}

bool CNationSettingData::IsCashDBInit()
{
  return m_eCashDBFlag == CDPS_INIT;
}

bool CNationSettingData::IsCashDBDSNSetted()
{
  return m_eCashDBFlag == CDPS_SET;
}

INationGameGuardSystem *CNationSettingData::GetGameGuardSystem()
{
  return m_pGameGuardSystem;
}

void CNationSettingData::SetCashDBInitFlag()
{
  m_eCashDBFlag = CDPS_INIT;
}

void CNationSettingData::SetCashDBDSN(
  char *szIP,
  char *szDBName,
  char *szAccount,
  char *szPassword,
  unsigned __int16 dwPort)
{
  strcpy_s(m_szCashDBName, sizeof(m_szCashDBName), szDBName);
  strcpy_s(m_szCashDBIP, sizeof(m_szCashDBIP), szIP);
  strcpy_s(m_szCashDBID, sizeof(m_szCashDBID), szAccount);
  strcpy_s(m_szCashDBPW, sizeof(m_szCashDBPW), szPassword);
  m_wCashDBPort = dwPort;
  SetCashDBDSNSetFlag();
}

void CNationSettingData::SetCashDBDSNSetFlag()
{
  m_eCashDBFlag = CDPS_SET;
}

void CNationSettingData::SetCahsDBUseExtRefFlag()
{
  m_eCashDBFlag = CDPS_USE_REF;
}

bool CNationSettingData::CheckDBCSCompleteString(
  unsigned int nCodePage,
  const char *strData,
  unsigned __int64 *pCharacterCount)
{
  if (pCharacterCount)
  {
    *pCharacterCount = 0;
  }
  if (!strData)
  {
    return false;
  }

  const size_t length = strlen_0(strData);
  unsigned __int64 characterCount = 0;
  for (size_t index = 0;; ++index)
  {
    if (index >= length)
    {
      return true;
    }

    ++characterCount;
    if (pCharacterCount)
    {
      *pCharacterCount = characterCount;
    }

    if (IsDBCSLeadByteEx(nCodePage, strData[index]))
    {
      if (index == length - 1)
      {
        --characterCount;
        if (pCharacterCount)
        {
          *pCharacterCount = characterCount;
        }
        return false;
      }
      ++index;
    }
  }
}

bool CNationSettingData::IsNormalStringDefProc(const char *szString, const char *szException)
{
  if (!CheckDBCSCompleteString(m_iANSICodePage, szString, nullptr))
  {
    return false;
  }

  wchar_t wideCharStr[272];
  wchar_t exceptionStr[258];
  memset(wideCharStr, 0, sizeof(wchar_t) * 256);
  memset(exceptionStr, 0, sizeof(wchar_t) * 256);
  MultiByteToWideChar(m_iANSICodePage, 0, szString, -1, wideCharStr, 256);
  if (szException)
  {
    MultiByteToWideChar(m_iANSICodePage, 0, szException, -1, exceptionStr, 256);
  }

  for (int index = 0;; ++index)
  {
    if (index >= lstrlenW(wideCharStr))
    {
      break;
    }

    const wchar_t ch = wideCharStr[index];
    if ((ch < 0x41 || ch > 0x5A) && (ch < 0x61 || ch > 0x7A) && !iswctype(ch, 4u))
    {
      if (!IsNormalChar(ch) && !iswctype(ch, 259))
      {
        if (!szException)
        {
          return false;
        }
        wchar_t subStr[14];
        subStr[0] = ch;
        subStr[1] = 0;
        if (!wcsstr(exceptionStr, subStr))
        {
          return false;
        }
      }
    }
  }
  return true;
}

bool CNationSettingData::IsNormalStringDefProc(const wchar_t *wszString, const wchar_t *wszException)
{
  for (int index = 0;; ++index)
  {
    if (index >= lstrlenW(wszString))
    {
      break;
    }

    const wchar_t ch = wszString[index];
    if ((ch < 0x41 || ch > 0x5A) && (ch < 0x61 || ch > 0x7A) && !iswctype(ch, 4u))
    {
      if (!IsNormalChar(ch) && !iswctype(ch, 259))
      {
        if (!wszException)
        {
          return false;
        }
        wchar_t subStr[10];
        subStr[0] = ch;
        subStr[1] = 0;
        if (!wcsstr(wszException, subStr))
        {
          return false;
        }
      }
    }
  }
  return true;
}

bool CNationSettingData::GetFireGuardEnableSetting()
{
  char returnedString[40];
  memset(returnedString, 0, 16);
  GetPrivateProfileStringA("FireGuard Use", "Use", "X", returnedString, 16, ".\\fireguard\\fgrs.ini");
  if (!strcmp_0(returnedString, "X"))
  {
    return false;
  }
  if (!_stricmp(returnedString, "true"))
  {
    return true;
  }
  _stricmp(returnedString, "false");
  return false;
}

bool CNationSettingData::GetTimeLimitEnableSetting()
{
  char returnedString[40];
  memset(returnedString, 0, 16);
  GetPrivateProfileStringA("Time Limit", "Use", "X", returnedString, 16, ".\\Initialize\\WorldSystem.ini");
  if (!strcmp_0(returnedString, "X"))
  {
    return false;
  }
  if (!_stricmp(returnedString, "true"))
  {
    return true;
  }
  _stricmp(returnedString, "false");
  return false;
}
