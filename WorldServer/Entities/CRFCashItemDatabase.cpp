#include "pch.h"

#include "CRFCashItemDatabase.h"

#include <cstdio>
#include <cstring>
#include <sql.h>
#include <sqlext.h>

#include "CCashDBWorkManager.h"

CRFCashItemDatabase::CRFCashItemDatabase()
  : CRFNewDatabase()
{
}

CRFCashItemDatabase::~CRFCashItemDatabase() = default;

unsigned __int8 CRFCashItemDatabase::CallProc_RFOnlineAuth(_param_cash_select *rParam)
{
  char buffer[280]{};
  sprintf_s(
    buffer,
    "declare @out_amount int exec prc_rfonline_auth '%s', @s_amount = @out_amount output select @out_amount",
    rParam->in_szAcc);

  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), SQL_NTS);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == SQL_SUCCESS_WITH_INFO)
      {
        SQLLEN indicator = 0;
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, &rParam->out_dwCashAmount, 0, &indicator);
        if (!ret || ret == SQL_SUCCESS_WITH_INFO)
        {
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
          }
          if (m_bSaveDBLog)
          {
            FmtLog("%s Success", buffer);
          }
          return 0;
        }

        unsigned __int8 result = 0;
        if (ret == SQL_NO_DATA)
        {
          result = 2;
        }
        else
        {
          ErrorMsgLog(ret, buffer, "SQLExecDirect", m_hStmtSelect);
          ErrorAction(ret, m_hStmtSelect);
          result = 1;
        }
        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        return result;
      }

      unsigned __int8 result = 0;
      if (ret == SQL_NO_DATA)
      {
        result = 2;
      }
      else
      {
        ErrorMsgLog(ret, buffer, "SQLExecDirect", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
        result = 1;
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return result;
    }

    if (ret == SQL_NO_DATA)
    {
      return 2;
    }

    ErrorMsgLog(ret, buffer, "SQLExecDirect", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 1;
}

unsigned __int8 CRFCashItemDatabase::CallProc_RFOnlineUse(_param_cash_update *rParam, int nIdx)
{
  char wszQuery[5124]{};
  auto *workMgr = CTSingleton<CCashDBWorkManager>::Instance();
  workMgr->GetUseCashQueryStr(rParam, nIdx, wszQuery, 0x1400u);

  if (m_bSaveDBLog)
  {
    Log(wszQuery);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(wszQuery), SQL_NTS);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == SQL_SUCCESS_WITH_INFO)
      {
        char outState[16]{};
        SQLLEN indicator = 0;
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_CHAR, outState, 2, &indicator);
        if (!ret || ret == SQL_SUCCESS_WITH_INFO)
        {
          rParam->in_item[static_cast<unsigned __int64>(nIdx)].out_cState = outState[0];
          int outAmount[4] = {};
          ret = SQLGetData(m_hStmtSelect, 2u, SQL_C_ULONG, outAmount, 0, &indicator);
          if (!ret || ret == SQL_SUCCESS_WITH_INFO)
          {
            rParam->in_item[static_cast<unsigned __int64>(nIdx)].out_nCashAmount = outAmount[0];
            rParam->out_nCashAmount = outAmount[0];
            if (m_hStmtSelect)
            {
              SQLCloseCursor(m_hStmtSelect);
            }
            if (m_bSaveDBLog)
            {
              FmtLog("%s Success", wszQuery);
            }
            return 0;
          }

          unsigned __int8 result = 0;
          if (ret == SQL_NO_DATA)
          {
            result = 2;
          }
          else
          {
            ErrorMsgLog(ret, wszQuery, "SQLFetch", m_hStmtSelect);
            ErrorAction(ret, m_hStmtSelect);
            result = 1;
          }
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
          }
          return result;
        }

        unsigned __int8 result = 0;
        if (ret == SQL_NO_DATA)
        {
          result = 2;
        }
        else
        {
          ErrorMsgLog(ret, wszQuery, "SQLFetch", m_hStmtSelect);
          ErrorAction(ret, m_hStmtSelect);
          result = 1;
        }
        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        return result;
      }

      unsigned __int8 result = 0;
      if (ret == SQL_NO_DATA)
      {
        result = 2;
      }
      else
      {
        ErrorMsgLog(ret, wszQuery, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
        result = 1;
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return result;
    }

    if (ret == SQL_NO_DATA)
    {
      return 2;
    }

    ErrorMsgLog(ret, wszQuery, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", wszQuery);
  return 1;
}

unsigned __int8 CRFCashItemDatabase::CallProc_RFONLINE_Cancel(_param_cash_rollback::__list *list)
{
  char buffer[280]{};
  sprintf_s(
    buffer,
    "declare @out_status varchar(1) declare @out_amount int exec dbo.prc_rfonline_cancel %I64d, @s_status = @out_status "
    "output,@s_amount = @out_amount output select @out_status, @out_amount",
    list->in_lnUID);

  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), SQL_NTS);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == SQL_SUCCESS_WITH_INFO)
      {
        SQLLEN indicator = 0;
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_CHAR, &list->out_cStatus, 2, &indicator);
        if (!ret || ret == SQL_SUCCESS_WITH_INFO)
        {
          ret = SQLGetData(m_hStmtSelect, 2u, SQL_C_ULONG, &list->out_nCashAmount, 0, &indicator);
          if (!ret || ret == SQL_SUCCESS_WITH_INFO)
          {
            if (m_hStmtSelect)
            {
              SQLCloseCursor(m_hStmtSelect);
            }
            if (m_bSaveDBLog)
            {
              FmtLog("%s Success", buffer);
            }
            return 0;
          }

          unsigned __int8 result = 0;
          if (ret == SQL_NO_DATA)
          {
            result = 2;
          }
          else
          {
            ErrorMsgLog(ret, buffer, "SQLExecDirect", m_hStmtSelect);
            ErrorAction(ret, m_hStmtSelect);
            result = 1;
          }
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
          }
          return result;
        }

        unsigned __int8 result = 0;
        if (ret == SQL_NO_DATA)
        {
          result = 2;
        }
        else
        {
          ErrorMsgLog(ret, buffer, "SQLExecDirect", m_hStmtSelect);
          ErrorAction(ret, m_hStmtSelect);
          result = 1;
        }
        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        return result;
      }

      unsigned __int8 result = 0;
      if (ret == SQL_NO_DATA)
      {
        result = 2;
      }
      else
      {
        ErrorMsgLog(ret, buffer, "SQLExecDirect", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
        result = 1;
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return result;
    }

    if (ret == SQL_NO_DATA)
    {
      return 2;
    }

    ErrorMsgLog(ret, buffer, "SQLExecDirect", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 1;
}

bool CRFCashItemDatabase::CallProc_InsertCashItemLog(
  unsigned int dwSerial,
  unsigned __int8 byLv,
  char *szItemCode,
  char *szItemName,
  unsigned __int8 byNum,
  unsigned int dwCost)
{
  char buffer[1024]{};
  SYSTEMTIME sysTime{};
  GetLocalTime(&sysTime);

  sprintf_s(
    buffer,
    "{ CALL Prc_RFONLINE_TestServer_Buy_Log(%u, %d, '%s', '%s', %d, %d, '%04d-%02d-%02d %02d:%02d:%02d') }",
    dwSerial,
    byLv,
    szItemCode,
    szItemName,
    byNum,
    dwCost,
    sysTime.wYear,
    sysTime.wMonth,
    sysTime.wDay,
    sysTime.wHour,
    sysTime.wMinute,
    sysTime.wSecond);
  return ExecUpdateQuery(buffer, true);
}

unsigned __int8 CRFCashItemDatabase::CallProc_RFOnlineAvg_Event(unsigned int *iAvgCashSelling)
{
  char buffer[256]{};
  sprintf_s(
    buffer,
    "declare @out_amount int  exec Prc_RFONLINE_AVG_EVENT @t_amount = @out_amount output select @out_amount");

  unsigned __int8 result = SQLExecDirect_RetErrCode(buffer);
  if (result)
  {
    return result;
  }

  result = SQLFetch_RetErrCode(buffer);
  if (result)
  {
    return result;
  }

  unsigned __int16 columnNumber[14]{};
  columnNumber[0] = 1;
  result = SQLGetData_RetErrCode(buffer, columnNumber, SQL_C_ULONG, iAvgCashSelling);
  if (result)
  {
    return result;
  }

  SelectCleanUp(buffer);
  return 0;
}

void CRFCashItemDatabase::dhRExtractSubString(char *szSub, char *szFull, int n)
{
  if (szFull)
  {
    int count = 0;
    const int len = static_cast<int>(strlen_0(szFull));
    void *src = &szFull[len];
    for (int j = 0; j < n; ++j)
    {
      src = static_cast<char *>(src) - 1;
      if (src == szFull)
      {
        const size_t fullLen = strlen_0(szFull);
        memcpy_0(szSub, szFull, fullLen);
      }
      ++count;
    }
    if (src != szFull)
    {
      memcpy_0(szSub, src, count);
    }
  }
}
