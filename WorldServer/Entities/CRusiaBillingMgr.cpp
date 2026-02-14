#include "pch.h"

#include "CRusiaBillingMgr.h"

#include <cstdlib>
#include <cstring>
#include <new>
#include <objbase.h>
#include <odbcinst.h>
#include <windows.h>

#include "GlobalObjects.h"

extern "C"
{
  unsigned int __stdcall RFACC_Init();
  unsigned int __stdcall RFACC_Free();
  unsigned int __stdcall RFACC_CheckBalance(const char *szUserID);
  unsigned int __stdcall RFACC_ChargeBalance(
    const char *szUserID,
    const char *szCharName,
    const char *szItemCode,
    unsigned int dwOverlapNum,
    const char *szServer,
    unsigned __int64 lnUID,
    int nPrice,
    int nDiscount);
  unsigned int __stdcall RFACC_Cancel(unsigned __int64 lnUID);
}

CRusiaBillingMgr *CRusiaBillingMgr::m_pRusiaBill = nullptr;

CRusiaBillingMgr::CRusiaBillingMgr()
  = default;

CRusiaBillingMgr::~CRusiaBillingMgr()
  = default;

CRusiaBillingMgr *CRusiaBillingMgr::Instance()
{
  char *szDSN = const_cast<char *>("COIN_TEST_RU");
  const char *v8 = "coin_test_ru";
  const char *v9 = "rftest";
  char *szServer = const_cast<char *>("192.168.1.185");
  char *String = const_cast<char *>("61433");

  if (g_Main.IsReleaseServiceMode())
  {
    szDSN = const_cast<char *>("BILLING");
    v8 = "RF_GAMESERVER";
    v9 = "r8wFipfC0FUo";
    szServer = const_cast<char *>("81.176.70.145");
    String = const_cast<char *>("1521");
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

  const unsigned __int16 wPort = static_cast<unsigned __int16>(atoi(String));
  if (CRusiaBillingMgr::m_pRusiaBill->ConfigUserODBC(szDSN, szServer, szDSN, wPort))
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
  if (RFACC_Init())
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
  if (RFACC_Free())
  {
    CoUninitialize();
    return 1;
  }

  m_logBill.Write("CRusiaBillingMgr::RFACC_Free() Fail");
  CoUninitialize();
  return 0;
}

bool CRusiaBillingMgr::ConfigUserODBC(char *szDSN, char *szServer, char *szDatabase, unsigned __int16 wPort)
{
  unsigned __int16 offset = 0;
  char buffer[272]{};

  offset = static_cast<unsigned __int16>(sprintf(buffer, "DSN=%s%c", szDSN, 0));
  offset = static_cast<unsigned __int16>(offset + sprintf(&buffer[offset], "DESCRIPTION=%s%c", szDatabase, 0));
  offset = static_cast<unsigned __int16>(offset + sprintf(&buffer[offset], "SERVER=%s,%u%c", szServer, wPort, 0));
  offset = static_cast<unsigned __int16>(offset + sprintf(&buffer[offset], "DATABASE=%s%c", szDatabase, 0));

  wchar_t bufferW[272]{};
  const int attributesLen = static_cast<int>(offset) + 2;
  const int wideLen = MultiByteToWideChar(
    CP_ACP,
    0,
    buffer,
    attributesLen,
    bufferW,
    static_cast<int>(sizeof(bufferW) / sizeof(bufferW[0])));
  if (wideLen <= 0)
  {
    return false;
  }
  return SQLConfigDataSourceW(nullptr, ODBC_ADD_SYS_DSN, L"SQL Server", bufferW) != 0;
}

bool CRusiaBillingMgr::LoadINIFile()
{
  char buffer[304]{};
  char fileName[288]{};

  memset(buffer, 0, 260);
  GetCurrentDirectoryA(0x104u, buffer);
  memset(fileName, 0, 260);
  sprintf(fileName, "%s\\rfacc.ini", buffer);

  memset_0(CRusiaBillingMgr::m_pRusiaBill->m_lpszDataBase, 0, sizeof(CRusiaBillingMgr::m_pRusiaBill->m_lpszDataBase));
  if (!GetPrivateProfileStringA(
        "RF Setting",
        "Server",
        "127.0.0.1",
        CRusiaBillingMgr::m_pRusiaBill->m_lpszDataBase,
        0x104u,
        fileName))
  {
    return false;
  }

  memset_0(CRusiaBillingMgr::m_pRusiaBill->m_lpszPort, 0, sizeof(CRusiaBillingMgr::m_pRusiaBill->m_lpszPort));
  if (!GetPrivateProfileStringA(
        "RF Setting",
        "Port",
        "0",
        CRusiaBillingMgr::m_pRusiaBill->m_lpszPort,
        0x104u,
        fileName))
  {
    return false;
  }

  memset_0(CRusiaBillingMgr::m_pRusiaBill->m_lpszDSN, 0, sizeof(CRusiaBillingMgr::m_pRusiaBill->m_lpszDSN));
  if (!GetPrivateProfileStringA(
        "RF Setting",
        "DSN",
        "RF_RusBill",
        CRusiaBillingMgr::m_pRusiaBill->m_lpszDSN,
        0x104u,
        fileName))
  {
    return false;
  }

  memset_0(CRusiaBillingMgr::m_pRusiaBill->m_lpszAccount, 0, sizeof(CRusiaBillingMgr::m_pRusiaBill->m_lpszAccount));
  if (!GetPrivateProfileStringA(
        "RF Setting",
        "User ID",
        "sa",
        CRusiaBillingMgr::m_pRusiaBill->m_lpszAccount,
        0x104u,
        fileName))
  {
    return false;
  }

  memset_0(CRusiaBillingMgr::m_pRusiaBill->m_lpszPassword, 0, sizeof(CRusiaBillingMgr::m_pRusiaBill->m_lpszPassword));
  return GetPrivateProfileStringA(
           "RF Setting",
           "Password",
           "na4001",
           CRusiaBillingMgr::m_pRusiaBill->m_lpszPassword,
           0x104u,
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
  rParam->out_dwCashAmount = static_cast<int>(RFACC_CheckBalance(rParam->in_szAcc));
  CoUninitialize();
  return 0;
}

__int64 CRusiaBillingMgr::CallFunc_Item_Buy(_param_cash_update *rParam, int nIdx)
{
  CoInitialize(nullptr);
  const float inDiscount = static_cast<float>(rParam->in_item[static_cast<__int64>(nIdx)].in_nDiscount);
  const float inPrice = static_cast<float>(rParam->in_item[static_cast<__int64>(nIdx)].in_nPrice);
  const unsigned int overlapNum = rParam->in_item[static_cast<__int64>(nIdx)].in_byOverlapNum;

  int priceBits = 0;
  int discountBits = 0;
  memcpy_0(&priceBits, &inPrice, sizeof(priceBits));
  memcpy_0(&discountBits, &inDiscount, sizeof(discountBits));

  if (RFACC_ChargeBalance(
        rParam->in_szAcc,
        rParam->in_szAvatorName,
        rParam->in_item[static_cast<__int64>(nIdx)].in_strItemCode,
        overlapNum,
        rParam->in_szSvrName,
        rParam->in_item[static_cast<__int64>(nIdx)].in_lnUID,
        priceBits,
        discountBits))
  {
    rParam->in_item[static_cast<__int64>(nIdx)].out_nCashAmount =
      static_cast<int>(RFACC_CheckBalance(rParam->in_szAcc));
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
  if (RFACC_Cancel(list->in_lnUID))
  {
    list->out_nCashAmount = static_cast<int>(RFACC_CheckBalance(szUserID));
    CoUninitialize();
    return 0;
  }

  m_logBill.Write("Item Buy Cancel Fail.");
  CoUninitialize();
  return 1;
}
