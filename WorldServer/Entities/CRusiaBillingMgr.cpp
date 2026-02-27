#include "pch.h"

#include "CRusiaBillingMgr.h"

#include <cstdlib>
#include <cstring>
#include <new>
#include <objbase.h>
#include <odbcinst.h>
#include <windows.h>

#include "GlobalObjects.h"
#include "RFAcc.h"

#ifdef SQLConfigDataSource
#undef SQLConfigDataSource
#endif

CRusiaBillingMgr *CRusiaBillingMgr::m_pRusiaBill;

CRusiaBillingMgr::CRusiaBillingMgr()
  = default;

CRusiaBillingMgr::~CRusiaBillingMgr()
  = default;

CRusiaBillingMgr *CRusiaBillingMgr::Instance()
{
  const char *defaultDsn = "BILLING";
  const char *defaultServer = "(local)";
  const char *defaultPort = "1433";
  const char *defaultTrustedConnection = "1";

  char iniPath[MAX_PATH]{};
  char currentDir[MAX_PATH]{};
  if (GetCurrentDirectoryA(static_cast<DWORD>(sizeof(currentDir)), currentDir))
  {
    sprintf_s(iniPath, "%s\\Initialize\\Database.ini", currentDir);
  }

  char szDSN[64]{};
  char szServer[64]{};
  char szPort[32]{};
  char szTrustedConnection[16]{};
  if (iniPath[0] != '\0')
  {
    GetPrivateProfileStringA("Billing", "DB", defaultDsn, szDSN, static_cast<DWORD>(sizeof(szDSN)), iniPath);
    GetPrivateProfileStringA("Billing", "IP", defaultServer, szServer, static_cast<DWORD>(sizeof(szServer)), iniPath);
    GetPrivateProfileStringA("Billing", "Port", defaultPort, szPort, static_cast<DWORD>(sizeof(szPort)), iniPath);
    GetPrivateProfileStringA(
      "Billing",
      "trusted_connection",
      defaultTrustedConnection,
      szTrustedConnection,
      static_cast<DWORD>(sizeof(szTrustedConnection)),
      iniPath);
  }
  else
  {
    strcpy_s(szDSN, sizeof(szDSN), defaultDsn);
    strcpy_s(szServer, sizeof(szServer), defaultServer);
    strcpy_s(szPort, sizeof(szPort), defaultPort);
    strcpy_s(szTrustedConnection, sizeof(szTrustedConnection), defaultTrustedConnection);
  }

  if (!CRusiaBillingMgr::m_pRusiaBill)
  {
    auto *created = static_cast<CRusiaBillingMgr *>(operator new(sizeof(CRusiaBillingMgr)));
    if (created)
    {
      new (created) CRusiaBillingMgr();
    }
    CRusiaBillingMgr::m_pRusiaBill = created;
  }

  const unsigned long parsedPort = strtoul(szPort, nullptr, 10);
  const unsigned __int16 wPort = (parsedPort > 0 && parsedPort <= 0xFFFFu)
    ? static_cast<unsigned __int16>(parsedPort)
    : static_cast<unsigned __int16>(atoi(defaultPort));
  const bool bTrustedConnection = szTrustedConnection[0] != '\0' && szTrustedConnection[0] != '0';
  if (CRusiaBillingMgr::m_pRusiaBill->ConfigUserODBC(szDSN, szServer, szDSN, wPort, bTrustedConnection))
  {
    if (CRusiaBillingMgr::m_pRusiaBill->Init())
    {
      return CRusiaBillingMgr::m_pRusiaBill;
    }
    CRusiaBillingMgr::m_pRusiaBill->Release();
    return nullptr;
  }

  m_logBill.Write("CRusiaBillingMgr::ConfigUserODBC() Fail");
  CRusiaBillingMgr::m_pRusiaBill->Release();
  return nullptr;
}

void CRusiaBillingMgr::Release()
{
  CRusiaBillingMgr::m_pRusiaBill->Free();
  if (!CRusiaBillingMgr::m_pRusiaBill)
  {
    CRusiaBillingMgr::m_pRusiaBill = nullptr;
  }
}

__int64 CRusiaBillingMgr::Init()
{
  CoInitialize(nullptr);
  if (g_RFAcc.Init())
  {
    CoUninitialize();
    return 1;
  }

  m_logBill.Write("CRusiaBillingMgr::RFACC_Init() Fail");
  CoUninitialize();
  return 0;
}

__int64 CRusiaBillingMgr::Free()
{
  CoInitialize(nullptr);
  if (g_RFAcc.Free())
  {
    CoUninitialize();
    return 1;
  }

  m_logBill.Write("CRusiaBillingMgr::RFACC_Free() Fail");
  CoUninitialize();
  return 0;
}

bool CRusiaBillingMgr::ConfigUserODBC(char *szDSN, char *szServer, char *szDatabase, unsigned __int16 wPort, bool bTrustedConnection)
{
  char buffer[272]{};

  const int written = bTrustedConnection
    ? sprintf_s(buffer, sizeof(buffer), "DSN=%s%cDESCRIPTION=%s%cSERVER=%s,%u%cDATABASE=%s%cTrusted_Connection=Yes%c%c", szDSN, '\0', szDatabase, '\0', szServer, wPort, '\0', szDatabase, '\0', '\0')
    : sprintf_s(buffer, sizeof(buffer), "DSN=%s%cDESCRIPTION=%s%cSERVER=%s,%u%cDATABASE=%s%c%c", szDSN, '\0', szDatabase, '\0', szServer, wPort, '\0', szDatabase, '\0', '\0');
  if (written <= 0)
  {
    return false;
  }

  if (SQLConfigDataSource(nullptr, ODBC_ADD_SYS_DSN, "SQL Server", buffer) != 0)
  {
    return true;
  }
  if (SQLConfigDataSource(nullptr, ODBC_CONFIG_SYS_DSN, "SQL Server", buffer) != 0)
  {
    return true;
  }
  if (SQLConfigDataSource(nullptr, ODBC_ADD_DSN, "SQL Server", buffer) != 0)
  {
    return true;
  }
  if (SQLConfigDataSource(nullptr, ODBC_CONFIG_DSN, "SQL Server", buffer) != 0)
  {
    return true;
  }
  return false;
}

bool CRusiaBillingMgr::LoadINIFile()
{
  char buffer[304]{};
  char fileName[288]{};

  memset(buffer, 0, 260);
  GetCurrentDirectoryA(0x104u, buffer);
  memset(fileName, 0, 260);
  sprintf(fileName, "%s\\Initialize\\Database.ini", buffer);

  memset_0(CRusiaBillingMgr::m_pRusiaBill->m_lpszDataBase, 0, sizeof(CRusiaBillingMgr::m_pRusiaBill->m_lpszDataBase));
  if (!GetPrivateProfileStringA(
        "Billing",
        "IP",
        "(local)",
        CRusiaBillingMgr::m_pRusiaBill->m_lpszDataBase,
        static_cast<DWORD>(sizeof(CRusiaBillingMgr::m_pRusiaBill->m_lpszDataBase)),
        fileName))
  {
    return false;
  }

  memset_0(CRusiaBillingMgr::m_pRusiaBill->m_lpszPort, 0, sizeof(CRusiaBillingMgr::m_pRusiaBill->m_lpszPort));
  if (!GetPrivateProfileStringA(
        "Billing",
        "Port",
        "1433",
        CRusiaBillingMgr::m_pRusiaBill->m_lpszPort,
        static_cast<DWORD>(sizeof(CRusiaBillingMgr::m_pRusiaBill->m_lpszPort)),
        fileName))
  {
    return false;
  }

  memset_0(CRusiaBillingMgr::m_pRusiaBill->m_lpszDSN, 0, sizeof(CRusiaBillingMgr::m_pRusiaBill->m_lpszDSN));
  if (!GetPrivateProfileStringA(
        "Billing",
        "DB",
        "BILLING",
        CRusiaBillingMgr::m_pRusiaBill->m_lpszDSN,
        static_cast<DWORD>(sizeof(CRusiaBillingMgr::m_pRusiaBill->m_lpszDSN)),
        fileName))
  {
    return false;
  }

  memset_0(CRusiaBillingMgr::m_pRusiaBill->m_lpszAccount, 0, sizeof(CRusiaBillingMgr::m_pRusiaBill->m_lpszAccount));
  if (!GetPrivateProfileStringA(
        "Billing",
        "ID",
        "sa",
        CRusiaBillingMgr::m_pRusiaBill->m_lpszAccount,
        static_cast<DWORD>(sizeof(CRusiaBillingMgr::m_pRusiaBill->m_lpszAccount)),
        fileName))
  {
    return false;
  }

  memset_0(CRusiaBillingMgr::m_pRusiaBill->m_lpszPassword, 0, sizeof(CRusiaBillingMgr::m_pRusiaBill->m_lpszPassword));
  return GetPrivateProfileStringA(
           "Billing",
           "PWD",
           "",
           CRusiaBillingMgr::m_pRusiaBill->m_lpszPassword,
           static_cast<DWORD>(sizeof(CRusiaBillingMgr::m_pRusiaBill->m_lpszPassword)),
           fileName)
         != 0;
}

void CRusiaBillingMgr::ArrangeString(char *szDest, char *szSorc, char cToken)
{
  if (szDest && szSorc)
  {
    char *src = szSorc;
    for (int j = 0;; ++j)
    {
      const int len = static_cast<int>(strlen_0(szSorc));
      if (j >= len)
      {
        break;
      }
      if (*src == cToken)
      {
        ++src;
      }
      else if (src)
      {
        *szDest++ = *src++;
      }
    }
  }
}

char *CRusiaBillingMgr::dhExtractSubString(char *szSub, char *szFull, char cToken)
{
  if (!szFull)
  {
    return nullptr;
  }
  if (!cToken)
  {
    return nullptr;
  }

  char *pos = szFull;
  while (*pos != cToken)
  {
    if (!++pos)
    {
      return nullptr;
    }
  }
  if (pos != szFull)
  {
    memcpy_0(szSub, szFull, static_cast<size_t>(pos - szFull));
  }
  return pos + 1;
}

void CRusiaBillingMgr::dhRExtractSubString(char *szSub, char *szFull, char cToken)
{
  if (szFull && cToken)
  {
    int count = 0;
    const int len = static_cast<int>(strlen_0(szFull));
    char *pos = &szFull[len];
    while (*pos != cToken)
    {
      if (--pos == szFull)
      {
        const size_t fullLen = strlen_0(szFull);
        memcpy_0(szSub, szFull, fullLen);
      }
      ++count;
    }
    if (pos != szFull)
    {
      memcpy_0(szSub, pos + 1, count - 1);
    }
  }
}

void CRusiaBillingMgr::DeleteMem()
{
  // this is not a stub
}

__int64 CRusiaBillingMgr::CallFunc_RFOnline_Auth(_param_cash_select *rParam)
{
  CoInitialize(nullptr);
  rParam->out_dwCashAmount = static_cast<int>(g_RFAcc.CheckBalance(rParam->in_szAcc));
  CoUninitialize();
  return 0;
}

__int64 CRusiaBillingMgr::CallFunc_Item_Buy(_param_cash_update *rParam, int nIdx)
{
  CoInitialize(nullptr);
  const int inDiscount = static_cast<int>(rParam->in_item[static_cast<__int64>(nIdx)].in_nDiscount);
  const int inPrice = rParam->in_item[static_cast<__int64>(nIdx)].in_nPrice;
  const unsigned int overlapNum = rParam->in_item[static_cast<__int64>(nIdx)].in_byOverlapNum;

  if (g_RFAcc.ChargeBalance(
        rParam->in_szAcc,
        rParam->in_szAvatorName,
        rParam->in_item[static_cast<__int64>(nIdx)].in_strItemCode,
        overlapNum,
        rParam->in_szSvrName,
        rParam->in_item[static_cast<__int64>(nIdx)].in_lnUID,
        inPrice,
        inDiscount))
  {
    rParam->in_item[static_cast<__int64>(nIdx)].out_nCashAmount =
      static_cast<int>(g_RFAcc.CheckBalance(rParam->in_szAcc));
    rParam->out_nCashAmount = rParam->in_item[static_cast<__int64>(nIdx)].out_nCashAmount;
    CoUninitialize();
    return 0;
  }

  m_logBill.Write("Item Buy Fail.");
  CoUninitialize();
  return 1;
}

__int64 CRusiaBillingMgr::CallFunc_Item_Cancel(_param_cash_rollback::__list *list, char *szUserID)
{
  CoInitialize(nullptr);
  if (g_RFAcc.Cancel(list->in_lnUID))
  {
    list->out_nCashAmount = static_cast<int>(g_RFAcc.CheckBalance(szUserID));
    CoUninitialize();
    return 0;
  }

  m_logBill.Write("Item Buy Cancel Fail.");
  CoUninitialize();
  return 1;
}
