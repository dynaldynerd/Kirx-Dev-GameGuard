#include "pch.h"

#include "CRFNewDatabase.h"
#include "WorldServerUtil.h"

#include <cstdio>
#include <cstring>
#include <odbcinst.h>
#include <sql.h>
#include <sqlext.h>
#include <windows.h>

CRFNewDatabase::CRFNewDatabase()
{
  m_hEnv = nullptr;
  m_hDbc = nullptr;
  m_hStmtSelect = nullptr;
  m_hStmtUpdate = nullptr;
  m_bConectionActive = false;
  m_bReconnectFailExit = true;
  memset_0(m_szOdbcName, 0, sizeof(m_szOdbcName));
  memset_0(m_szAccountName, 0, sizeof(m_szAccountName));
  memset_0(m_szPassword, 0, sizeof(m_szPassword));
  m_szLogUpperPath[0] = 0;
}

CRFNewDatabase::~CRFNewDatabase()
{
  Log("~CRFNewDatabase() : EndDataBase()");
  EndDataBase();
}

bool CRFNewDatabase::AllocSelectHandle()
{
  if (!SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hStmtSelect))
  {
    return true;
  }

  ErrLog("SELECT SQLAllocHandle Fail");
  return false;
}

bool CRFNewDatabase::AllocUpdateHandle()
{
  if (!SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hStmtUpdate))
  {
    return true;
  }

  ErrLog("UPDATE SQLAllocHandle Fail");
  return false;
}

void CRFNewDatabase::CheckLogFileHour()
{
  const unsigned __int8 prevHour = m_byLogFileHour;
  const unsigned __int8 localHour = GetLocalHour();
  if (prevHour != localHour)
  {
    char path[512]{};
    const unsigned int korLocalTime = GetKorLocalTime();
    sprintf_s(
      path,
      "%sDBLog\\DBProcess_%s_%d_U.log",
      m_szLogUpperPath,
      m_szOdbcName,
      korLocalTime);
    if (!m_ProcessLogW.m_bInit)
    {
      m_ProcessLogW.SetWriteLogFile(path, 1, 1, 1, 1);
    }

    memset_0(path, 0, sizeof(path));
    const unsigned int korLocalTimeA = GetKorLocalTime();
    sprintf_s(
      path,
      "%sDBLog\\DBProcess_%s_%d_A.log",
      m_szLogUpperPath,
      m_szOdbcName,
      korLocalTimeA);
    if (!m_ProcessLogA.m_bInit)
    {
      m_ProcessLogA.SetWriteLogFile(path, 1, 1, 1, 1);
    }

    memset_0(path, 0, sizeof(path));
    const unsigned int korLocalTimeErrorW = GetKorLocalTime();
    sprintf_s(
      path,
      "%sDBLog\\DBError_%s_%d_U.log",
      m_szLogUpperPath,
      m_szOdbcName,
      korLocalTimeErrorW);
    if (!m_ErrorLogW.m_bInit)
    {
      m_ErrorLogW.SetWriteLogFile(path, 1, 1, 1, 1);
    }

    memset_0(path, 0, sizeof(path));
    const unsigned int korLocalTimeErrorA = GetKorLocalTime();
    sprintf_s(
      path,
      "%sDBLog\\DBError_%s_%d_A.log",
      m_szLogUpperPath,
      m_szOdbcName,
      korLocalTimeErrorA);
    if (!m_ErrorLogA.m_bInit)
    {
      m_ErrorLogA.SetWriteLogFile(path, 1, 1, 1, 1);
    }

    m_byLogFileHour = GetLocalHour();
  }
}

bool CRFNewDatabase::CommitTransaction()
{
  const SQLRETURN ret = SQLEndTran(SQL_HANDLE_DBC, m_hDbc, SQL_COMMIT);
  return !ret || ret == SQL_SUCCESS_WITH_INFO;
}

bool CRFNewDatabase::ConfigUserODBC(
  const char *szDSN,
  const char *szServer,
  const char *szDatabase,
  unsigned __int16 wPort)
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

void CRFNewDatabase::DiagRecALog(__int16 sqlRet, SQLSMALLINT HandleType, void *Handle)
{
  if (sqlRet == SQL_SUCCESS_WITH_INFO || sqlRet == SQL_ERROR)
  {
    SQLCHAR sqlState[44]{};
    SQLCHAR errorMsg[532]{};
    SQLINTEGER nativeError[8]{};
    SQLSMALLINT msgLen[8]{};
    msgLen[0] = 0;
    for (int index = 1;
         index < 11 && !SQLGetDiagRecA(HandleType, Handle, index, sqlState, nativeError, errorMsg, 512, msgLen);
         ++index)
    {
      ErrFmtLog(
        "SqlState:%s, NativeError:%d  Msg:%s ",
        reinterpret_cast<const char *>(sqlState),
        nativeError[0],
        reinterpret_cast<const char *>(errorMsg));
    }
  }
}

void CRFNewDatabase::DiagRecWLog(__int16 sqlRet, SQLSMALLINT HandleType, void *Handle)
{
  if (sqlRet == SQL_SUCCESS_WITH_INFO || sqlRet == SQL_ERROR)
  {
    SQLWCHAR sqlState[28]{};
    SQLWCHAR errorMsg[522]{};
    SQLINTEGER nativeError[8]{};
    SQLSMALLINT msgLen[8]{};
    msgLen[0] = 0;
    for (int index = 1;
         index < 11 && !SQLGetDiagRecW(HandleType, Handle, index, sqlState, nativeError, errorMsg, 1024, msgLen);
         ++index)
    {
      ErrFmtLog(
        L"SqlState:%s, NativeError:%d  Msg:%s ",
        sqlState,
        static_cast<unsigned int>(nativeError[0]),
        errorMsg);
    }
  }
}

void CRFNewDatabase::EndDataBase()
{
  Log(" ");
  Log("EndDatabase : Start");
  if (m_bConectionActive)
  {
    Log("EndDatabase() : FreeHandle() Start");
    FreeSelectHandle();
    FreeUpdateHandle();
    if (m_hDbc)
    {
      SQLDisconnect(m_hDbc);
    }
    if (m_hEnv)
    {
      SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
    }
    if (m_hDbc)
    {
      SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc);
    }
    Log("EndDatabase() : FreeHandle() End");
  }

  m_bConectionActive = false;
  m_hEnv = nullptr;
  m_hDbc = nullptr;
  m_hStmtSelect = nullptr;
  m_hStmtUpdate = nullptr;
  Log("EndDatabase : End");
}

bool CRFNewDatabase::EroorActionProcSQL_ERROR(void *SQLStmt)
{
  SQLCHAR sqlState[44]{};
  SQLCHAR errorMsg[532]{};
  SQLINTEGER nativeError[8]{};
  SQLSMALLINT msgLen[8]{};
  msgLen[0] = 0;

  for (int index = 1;
       index < 11 && !SQLGetDiagRecA(SQL_HANDLE_STMT, SQLStmt, index, sqlState, nativeError, errorMsg, 512, msgLen);
       ++index)
  {
    if (!strncmp("08S01", reinterpret_cast<const char *>(sqlState), 5u)
        || !strncmp("24000", reinterpret_cast<const char *>(sqlState), 5u)
        || nativeError[0] == 10054)
    {
      ErrFmtLog(
        "EroorActionProcSQL_ERROR : DB Error!(%s) -> ReConnectDataBase()",
        reinterpret_cast<const char *>(sqlState));
      if (!ReConnectDataBase())
      {
        return false;
      }
    }
  }

  return true;
}

void CRFNewDatabase::ErrFmtLog(const char *fmt, ...)
{
  va_list va;
  va_start(va, fmt);
  m_ErrorLogA.WriteFromArg(fmt, va);
  va_end(va);
}

void CRFNewDatabase::ErrFmtLog(const wchar_t *lpcwFmt, ...)
{
  va_list va;
  va_start(va, lpcwFmt);
  m_ErrorLogW.WriteFromArg(lpcwFmt, va);
  va_end(va);
}

void CRFNewDatabase::ErrLog(const char *szLog)
{
  m_ErrorLogA.WriteString(szLog);
}

void CRFNewDatabase::ErrorAction(__int16 sqlRet, void *SQLStmt)
{
  if (sqlRet == SQL_INVALID_HANDLE)
  {
    ErrFmtLog("SQL_INVALID_HANDLE -> ReConnectDataBase()");
    if (!ReConnectDataBase() && m_bReconnectFailExit)
    {
      ServerProgramExit("CRFNewDatabase::ErrorAction : DB Connection Refused! ReConnectDataBase Fail!", 1);
    }
  }
  else if (sqlRet == SQL_ERROR && !EroorActionProcSQL_ERROR(SQLStmt) && m_bReconnectFailExit)
  {
    ServerProgramExit("CRFNewDatabase::ErrorAction : DB Connection Refused! ReConnectDataBase Fail!", 1);
  }
}

void CRFNewDatabase::ErrorMsgLog(__int16 sqlRet, const char *strQuery, const char *strKind, void *SQLStmt)
{
  ErrFmtLog("Query : %s", strQuery);
  DiagRecALog(sqlRet, SQL_HANDLE_STMT, SQLStmt);

  if (sqlRet == SQL_INVALID_HANDLE)
  {
    ErrFmtLog("%s Returns : SQL_INVALID_HANDLE", strKind);
    return;
  }

  switch (sqlRet)
  {
    case SQL_ERROR:
      ErrFmtLog("%s Returns : SQL_ERROR", strKind);
      break;
    case SQL_STILL_EXECUTING:
      ErrFmtLog("%s Returns : SQL_STILL_EXECUTING", strKind);
      break;
    case SQL_NEED_DATA:
      ErrFmtLog("%s Returns : SQL_NEED_DATA", strKind);
      break;
    case SQL_NO_DATA:
      ErrFmtLog("%s Returns : SQL_NO_DATA", strKind);
      break;
    default:
      ErrFmtLog("%s Returns : sqlRet = %d", strKind, sqlRet);
      break;
  }
}

void CRFNewDatabase::ErrorMsgLog(__int16 sqlRet, const wchar_t *strQuery, const wchar_t *strKind, void *SQLStmt)
{
  ErrFmtLog(L"Query : %s", strQuery);
  DiagRecWLog(sqlRet, SQL_HANDLE_STMT, SQLStmt);

  if (sqlRet == SQL_INVALID_HANDLE)
  {
    ErrFmtLog(L"%s Returns : SQL_INVALID_HANDLE", strKind);
    return;
  }

  switch (sqlRet)
  {
    case SQL_ERROR:
      ErrFmtLog(L"%s Returns : SQL_ERROR", strKind);
      break;
    case SQL_STILL_EXECUTING:
      ErrFmtLog(L"%s Returns : SQL_STILL_EXECUTING", strKind);
      break;
    case SQL_NEED_DATA:
      ErrFmtLog(L"%s Returns : SQL_NEED_DATA", strKind);
      break;
    case SQL_NO_DATA:
      ErrFmtLog(L"%s Returns : SQL_NO_DATA", strKind);
      break;
    default:
      ErrFmtLog(L"%s Returns : sqlRet = %d", strKind, static_cast<unsigned int>(sqlRet));
      break;
  }
}

bool CRFNewDatabase::ExecUpdateBinaryQuery(const char *strQuery, char *buf, int size, bool bNoDataError)
{
  if (m_bSaveDBLog)
  {
    FmtLog("Query : %s", strQuery);
  }

  if (m_hStmtUpdate || ReConnectDataBase())
  {
    SQLLEN pcbValue = SQL_LEN_DATA_AT_EXEC(size);
    SQLRETURN ret = SQLBindParameter(
      m_hStmtUpdate,
      1u,
      SQL_PARAM_INPUT,
      SQL_C_BINARY,
      SQL_LONGVARBINARY,
      size,
      0,
      reinterpret_cast<SQLPOINTER>(1),
      0,
      &pcbValue);
    ret = SQLExecDirectA(m_hStmtUpdate, reinterpret_cast<SQLCHAR *>(const_cast<char *>(strQuery)), SQL_NTS);
    if (ret == SQL_ERROR || ret == SQL_NO_DATA)
    {
      if (ret == SQL_ERROR || bNoDataError)
      {
        ErrorMsgLog(ret, strQuery, "SQLExecDirect", m_hStmtUpdate);
        ErrorAction(ret, m_hStmtUpdate);
        return false;
      }
      return true;
    }

    while (ret == SQL_NEED_DATA)
    {
      SQLPOINTER value = nullptr;
      ret = SQLParamData(m_hStmtUpdate, &value);
      if (ret == SQL_NEED_DATA && value == reinterpret_cast<SQLPOINTER>(1))
      {
        for (char *src = buf; src < &buf[size]; src += 1024)
        {
          const signed __int64 left = &buf[size] - src;
          const signed __int64 chunk = left <= 1024 ? left : 1024;
          const int chunkSize = static_cast<int>(chunk);
          unsigned char data[1032]{};
          memcpy_0(data, src, chunkSize);

          const SQLRETURN putRet = SQLPutData(m_hStmtUpdate, data, chunkSize);
          if (static_cast<unsigned int>(putRet) >= 0xFFFFFFFEu)
          {
            ErrorMsgLog(ret, strQuery, "SQLExecDirect", m_hStmtUpdate);
            ErrorAction(ret, m_hStmtUpdate);
            return false;
          }
        }
      }
    }

    if (m_bSaveDBLog)
    {
      FmtLog("ExecUpdateQuery : %s Query Success", strQuery);
    }
    return true;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", strQuery);
  return false;
}

bool CRFNewDatabase::ExecUpdateQuery(const char *strQuery, bool bNoDataError)
{
  if (m_bSaveDBLog)
  {
    FmtLog("Query : %s", strQuery);
  }

  if (m_hStmtUpdate || ReConnectDataBase())
  {
    const SQLRETURN ret =
      SQLExecDirectA(m_hStmtUpdate, reinterpret_cast<SQLCHAR *>(const_cast<char *>(strQuery)), SQL_NTS);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      if (m_bSaveDBLog)
      {
        FmtLog("ExecUpdateQuery : %s Query Success", strQuery);
      }
      return true;
    }

    if (ret != SQL_NO_DATA || bNoDataError)
    {
      ErrorMsgLog(ret, strQuery, "SQLExecDirect", m_hStmtUpdate);
      ErrorAction(ret, m_hStmtUpdate);
      return false;
    }

    return true;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", strQuery);
  return false;
}

bool CRFNewDatabase::ExecUpdateQuery(const wchar_t *wstrQuery, bool bNoDataError)
{
  if (m_bSaveDBLog)
  {
    FmtLog(L"Query : %s", wstrQuery);
  }

  if (m_hStmtUpdate || ReConnectDataBase())
  {
    const SQLRETURN ret =
      SQLExecDirectW(m_hStmtUpdate, reinterpret_cast<SQLWCHAR *>(const_cast<wchar_t *>(wstrQuery)), SQL_NTS);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      if (m_bSaveDBLog)
      {
        FmtLog(L"ExecUpdateQuery : %s Query Success", wstrQuery);
      }
      return true;
    }

    if (ret != SQL_NO_DATA || bNoDataError)
    {
      ErrorMsgLog(ret, wstrQuery, L"SQLExecDirect", m_hStmtUpdate);
      ErrorAction(ret, m_hStmtUpdate);
      return false;
    }

    return true;
  }

  ErrFmtLog(L"ReConnectDataBase Fail. Query : %s", wstrQuery);
  return false;
}

void CRFNewDatabase::FmtLog(const char *fmt, ...)
{
  va_list va;
  va_start(va, fmt);
  m_ProcessLogA.WriteFromArg(fmt, va);
  va_end(va);
}

void CRFNewDatabase::FmtLog(const wchar_t *lpcwFmt, ...)
{
  va_list va;
  va_start(va, lpcwFmt);
  m_ProcessLogW.WriteFromArg(lpcwFmt, va);
  va_end(va);
}

bool CRFNewDatabase::FreeSelectHandle()
{
  if (!m_hStmtSelect)
  {
    return true;
  }

  const SQLRETURN ret = SQLFreeHandle(SQL_HANDLE_STMT, m_hStmtSelect);
  m_hStmtSelect = nullptr;
  if (!ret)
  {
    return true;
  }

  ErrLog("SELECT SQLFreeHandle Fail");
  return false;
}

bool CRFNewDatabase::FreeUpdateHandle()
{
  if (!m_hStmtUpdate)
  {
    return true;
  }

  const SQLRETURN ret = SQLFreeHandle(SQL_HANDLE_STMT, m_hStmtUpdate);
  m_hStmtUpdate = nullptr;
  if (!ret)
  {
    return true;
  }

  ErrLog("UPDATE SQLFreeHandle Fail");
  return false;
}

unsigned __int8 CRFNewDatabase::GetLocalHour()
{
  SYSTEMTIME localTime{};
  GetLocalTime(&localTime);
  return static_cast<unsigned __int8>(localTime.wHour);
}

bool CRFNewDatabase::IsConectionActive()
{
  return m_bConectionActive;
}

void CRFNewDatabase::Log(const char *szLog)
{
  m_ProcessLogA.WriteString(szLog);
}

bool CRFNewDatabase::ReConnectDataBase()
{
  Log(" ");
  Log("ReConnectDataBase : Start");
  ErrLog(" ");
  ErrLog("ReConnectDataBase : Start");

  EndDataBase();

  for (int count = 0; count < 5; ++count)
  {
    if (StartDataBase(m_szOdbcName, m_szAccountName, m_szPassword))
    {
      FmtLog("ReConnectDataBase Count : %d Success.", count);
      ErrFmtLog("ReConnectDataBase Count : %d Success.", count);
      return true;
    }
    FmtLog("ReConnectDataBase Fail. Count : %d", count);
    ErrFmtLog("ReConnectDataBase Fail. Count : %d", count);
    Sleep(0x3E8u);
  }

  Log("ReConnectDataBase : Fail End");
  ErrLog("ReConnectDataBase : Fail End");
  return false;
}

bool CRFNewDatabase::RollbackTransaction()
{
  const SQLRETURN ret = SQLEndTran(SQL_HANDLE_DBC, m_hDbc, SQL_ROLLBACK);
  return !ret || ret == SQL_SUCCESS_WITH_INFO;
}

bool CRFNewDatabase::Select_NextHourDate(unsigned __int8 byAddHour, char *szDate)
{
  char buffer[272]{};
  sprintf_s(
    buffer,
    0x100u,
    "select convert( varchar(9), dateadd(hour, %d, getdate()) , 112 )",
    byAddHour);

  if (m_bSaveDBLog)
  {
    m_ProcessLogA.WriteString(buffer);
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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_CHAR, szDate, 9, &indicator);
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
          return true;
        }

        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
        return false;
      }

      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      if (ret != SQL_NO_DATA)
      {
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
      }
      return false;
    }

    if (ret != SQL_NO_DATA)
    {
      ErrorMsgLog(ret, buffer, "SQLExecDirect", m_hStmtSelect);
      ErrorAction(ret, m_hStmtSelect);
    }
    return false;
  }

  ErrFmtLog("ReConnectDataBase() Fail. Query : %s", buffer);
  return false;
}

void CRFNewDatabase::SelectCleanUp(const char *strQuery)
{
  if (m_hStmtSelect)
  {
    SQLCloseCursor(m_hStmtSelect);
  }
  if (m_bSaveDBLog)
  {
    FmtLog("%s Success", strQuery);
  }
}

void CRFNewDatabase::SetAutoCommitMode(bool bAutoCommit)
{
  if (bAutoCommit)
  {
    SQLSetConnectAttr(m_hDbc, SQL_ATTR_AUTOCOMMIT, reinterpret_cast<SQLPOINTER>(1), 0);
  }
  else
  {
    SQLSetConnectAttr(m_hDbc, SQL_ATTR_AUTOCOMMIT, nullptr, 0);
  }
}

void CRFNewDatabase::SetLogFile(const char *szUpperLogPath, const char *szOdbcName)
{
  sprintf_s(m_szLogUpperPath, sizeof(m_szLogUpperPath), "%s", szUpperLogPath);

  char path[512]{};
  sprintf_s(path, "%sDBLog", m_szLogUpperPath);
  CreateDirectoryA(path, nullptr);

  char buffer[544]{};
  const unsigned int date = GetKorLocalTime();
  sprintf(buffer, "%sDBLog\\DBProcess_%s_%d_U.log", m_szLogUpperPath, szOdbcName, date);
  if (!m_ProcessLogW.m_bInit)
  {
    m_ProcessLogW.SetWriteLogFile(buffer, 1, 1, 1, 1);
  }

  memset_0(buffer, 0, sizeof(buffer));
  const unsigned int dateA = GetKorLocalTime();
  sprintf(buffer, "%sDBLog\\DBProcess_%s_%d_A.log", m_szLogUpperPath, szOdbcName, dateA);
  if (!m_ProcessLogA.m_bInit)
  {
    m_ProcessLogA.SetWriteLogFile(buffer, 1, 1, 1, 1);
  }

  memset_0(buffer, 0, sizeof(buffer));
  const unsigned int dateErrorW = GetKorLocalTime();
  sprintf(buffer, "%sDBLog\\DBError_%s_%d_U.log", m_szLogUpperPath, szOdbcName, dateErrorW);
  if (!m_ErrorLogW.m_bInit)
  {
    m_ErrorLogW.SetWriteLogFile(buffer, 1, 1, 1, 1);
  }

  memset_0(buffer, 0, sizeof(buffer));
  const unsigned int dateErrorA = GetKorLocalTime();
  sprintf(buffer, "%sDBLog\\DBError_%s_%d_A.log", m_szLogUpperPath, szOdbcName, dateErrorA);
  if (!m_ErrorLogA.m_bInit)
  {
    m_ErrorLogA.SetWriteLogFile(buffer, 1, 1, 1, 1);
  }

  m_byLogFileHour = GetLocalHour();
  m_bSaveDBLog = false;
}

void CRFNewDatabase::SetReconnectFailExitFlag(bool bFlag)
{
  m_bReconnectFailExit = bFlag;
}

unsigned __int8 CRFNewDatabase::SQLExecDirect_RetErrCode(const char *strQuery)
{
  if (m_bSaveDBLog)
  {
    Log(strQuery);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    const SQLRETURN ret =
      SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(const_cast<char *>(strQuery)), SQL_NTS);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      return 0;
    }
    if (ret == SQL_NO_DATA)
    {
      return 2;
    }

    ErrorMsgLog(ret, strQuery, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", strQuery);
  return 1;
}

unsigned __int8 CRFNewDatabase::SQLFetch_RetErrCode(const char *strQuery)
{
  const SQLRETURN ret = SQLFetch(m_hStmtSelect);
  if (!ret || ret == SQL_SUCCESS_WITH_INFO)
  {
    return 0;
  }

  unsigned __int8 result = 0;
  if (ret == SQL_NO_DATA)
  {
    result = 2;
  }
  else
  {
    ErrorMsgLog(ret, strQuery, "SQLFetch", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    result = 1;
  }

  if (m_hStmtSelect)
  {
    SQLCloseCursor(m_hStmtSelect);
  }

  return result;
}

unsigned __int8 CRFNewDatabase::SQLGetData_Binary_RetErrCode(
  const char *strQuery,
  unsigned __int16 *ColumnNumber,
  unsigned __int8 *pData,
  rsize_t tDataSize)
{
  unsigned int dataSize = 0;
  unsigned int totalSize = 0;
  SQLLEN indicator = 0;

  while (true)
  {
    unsigned char targetValue[1048]{};
    const SQLRETURN ret =
      SQLGetData(m_hStmtSelect, *ColumnNumber, SQL_C_BINARY, targetValue, 1024, &indicator);
    if (ret == SQL_NO_DATA)
    {
      ++*ColumnNumber;
      return 0;
    }

    if (indicator == SQL_NULL_DATA)
    {
      ErrorMsgLog(ret, strQuery, "SQLGetData_Binary_RetErrCode", m_hStmtSelect);
      ErrorAction(ret, m_hStmtSelect);
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return 1;
    }

    const SQLLEN chunk = ret ? 1024 : indicator;
    dataSize = static_cast<unsigned int>(chunk);
    totalSize += static_cast<unsigned int>(chunk);
    memcpy_s(pData, tDataSize, targetValue, static_cast<size_t>(chunk));
    pData += dataSize;
    tDataSize -= dataSize;
  }
}

unsigned __int8 CRFNewDatabase::SQLGetData_RetErrCode(
  const char *strQuery,
  unsigned __int16 *ColumnNumber,
  SQLSMALLINT TargetType,
  void *TargetValue)
{
  const SQLUSMALLINT column = (*ColumnNumber)++;
  SQLLEN indicator = 0;
  const SQLRETURN ret = SQLGetData(m_hStmtSelect, column, TargetType, TargetValue, 0, &indicator);
  if (!ret || ret == SQL_SUCCESS_WITH_INFO)
  {
    if (ret == SQL_SUCCESS_WITH_INFO)
    {
      ErrorMsgLog(ret, strQuery, "SQLGetData_RetErrCode", m_hStmtSelect);
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
    ErrorMsgLog(ret, strQuery, "SQLGetData_RetErrCode", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    result = 1;
  }

  if (m_hStmtSelect)
  {
    SQLCloseCursor(m_hStmtSelect);
  }

  return result;
}

bool CRFNewDatabase::StartDataBase(const char *odbcName, const char *accountName, const char *passWord)
{
  FmtLog("StartDatabase : %s", odbcName);
  m_bConectionActive = false;

  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_ENV, nullptr, &m_hEnv);
  if (ret)
  {
    Log("SQLAllocHandle( SQL_HANDLE_ENV ) Failed!");
    if (m_hEnv)
    {
      SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
      m_hEnv = nullptr;
    }
    return false;
  }

  if (SQLSetEnvAttr(
        m_hEnv,
        SQL_ATTR_ODBC_VERSION,
        reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3),
        SQL_IS_INTEGER))
  {
    Log("SQLSetEnvAttr() Failed!");
    if (m_hEnv)
    {
      SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
      m_hEnv = nullptr;
    }
    return false;
  }

  ret = SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hDbc);
  if (ret)
  {
    Log("SQLAllocHandle( SQL_HANDLE_DBC ) Failed!");
    if (m_hEnv)
    {
      SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
      m_hEnv = nullptr;
    }
    if (m_hDbc)
    {
      SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc);
      m_hDbc = nullptr;
    }
    return false;
  }

  const size_t odbcLen = strlen_0(odbcName);
  memcpy_0(m_szOdbcName, odbcName, odbcLen);
  const size_t accountLen = strlen_0(accountName);
  memcpy_0(m_szAccountName, accountName, accountLen);
  const size_t passLen = strlen_0(passWord);
  memcpy_0(m_szPassword, passWord, passLen);

  SQLSetConnectAttr(m_hDbc, SQL_ATTR_LOGIN_TIMEOUT, reinterpret_cast<SQLPOINTER>(3), 0);
  ret = SQLConnectA(
    m_hDbc,
    reinterpret_cast<SQLCHAR *>(const_cast<char *>(odbcName)),
    SQL_NTS,
    reinterpret_cast<SQLCHAR *>(const_cast<char *>(accountName)),
    SQL_NTS,
    reinterpret_cast<SQLCHAR *>(const_cast<char *>(passWord)),
    SQL_NTS);

  if (!ret || ret == SQL_SUCCESS_WITH_INFO)
  {
    if (AllocSelectHandle())
    {
      if (AllocUpdateHandle())
      {
        m_bConectionActive = true;
        FmtLog("StartDatabase Success : %s", odbcName);
        return true;
      }

      FmtLog("StartDataBase Fail : %s", odbcName);
      if (m_hEnv)
      {
        SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
        m_hEnv = nullptr;
      }
      if (m_hDbc)
      {
        SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc);
        m_hDbc = nullptr;
      }
      return false;
    }

    FmtLog("StartDataBase Fail : %s", odbcName);
    if (m_hEnv)
    {
      SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
      m_hEnv = nullptr;
    }
    if (m_hDbc)
    {
      SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc);
      m_hDbc = nullptr;
    }
    return false;
  }

  FmtLog("StartDataBase Fail : %s", odbcName);
  if (m_hEnv)
  {
    SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
    m_hEnv = nullptr;
  }
  if (m_hDbc)
  {
    SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc);
    m_hDbc = nullptr;
  }
  return false;
}

bool CRFNewDatabase::TableExist(const char *szTableName)
{
  char query[256]{};
  sprintf_s(
    query,
    "SELECT top 1 name FROM dbo.sysobjects WHERE id = object_id('%s')",
    szTableName);

  if (m_bSaveDBLog)
  {
    Log(query);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == SQL_SUCCESS_WITH_INFO)
      {
        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        if (m_bSaveDBLog)
        {
          FmtLog("%s Success", query);
        }
        return true;
      }

      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      if (ret != SQL_NO_DATA)
      {
        ErrorMsgLog(ret, query, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
      }
      return false;
    }

    if (ret != SQL_NO_DATA)
    {
      ErrorMsgLog(ret, query, "SQLExecDirect", m_hStmtSelect);
      ErrorAction(ret, m_hStmtSelect);
    }
    return false;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
  return false;
}

