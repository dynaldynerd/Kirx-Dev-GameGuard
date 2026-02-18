#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"

#include <cstddef>
#include <sqltypes.h>

/* 1126 */
class  __declspec(align(8)) CRFNewDatabase
{
public:
  CRFNewDatabase();
  virtual ~CRFNewDatabase();

  bool AllocSelectHandle();
  bool AllocUpdateHandle();
  void CheckLogFileHour();
  bool CommitTransaction();
  bool ConfigUserODBC(const char *szDSN, const char *szServer, const char *szDatabase, unsigned __int16 wPort);
  void DiagRecALog(__int16 sqlRet, SQLSMALLINT HandleType, void *Handle);
  void DiagRecWLog(__int16 sqlRet, SQLSMALLINT HandleType, void *Handle);
  void EndDataBase();
  bool EroorActionProcSQL_ERROR(void *SQLStmt);
  void ErrFmtLog(const char *fmt, ...);
  void ErrFmtLog(const wchar_t *lpcwFmt, ...);
  void ErrLog(const char *szLog);
  void ErrorAction(__int16 sqlRet, void *SQLStmt);
  void ErrorMsgLog(__int16 sqlRet, const char *strQuery, const char *strKind, void *SQLStmt);
  void ErrorMsgLog(__int16 sqlRet, const wchar_t *strQuery, const wchar_t *strKind, void *SQLStmt);
  bool ExecUpdateBinaryQuery(const char *strQuery, char *buf, int size, bool bNoDataError);
  bool ExecUpdateQuery(const char *strQuery, bool bNoDataError);
  bool ExecUpdateQuery(const wchar_t *wstrQuery, bool bNoDataError);
  void FmtLog(const char *fmt, ...);
  void FmtLog(const wchar_t *lpcwFmt, ...);
  bool FreeSelectHandle();
  bool FreeUpdateHandle();
  unsigned __int8 GetLocalHour();
  bool IsConectionActive();
  void Log(const char *szLog);
  bool ReConnectDataBase();
  bool RollbackTransaction();
  bool Select_NextHourDate(unsigned __int8 byAddHour, char *szDate);
  void SelectCleanUp(const char *strQuery);
  void SetAutoCommitMode(bool bAutoCommit);
  void SetLogFile(const char *szUpperLogPath, const char *szOdbcName);
  void SetReconnectFailExitFlag(bool bFlag);
  unsigned __int8 SQLExecDirect_RetErrCode(const char *strQuery);
  unsigned __int8 SQLFetch_RetErrCode(const char *strQuery);
  unsigned __int8 SQLGetData_Binary_RetErrCode(
    const char *strQuery,
    unsigned __int16 *ColumnNumber,
    unsigned __int8 *pData,
    rsize_t tDataSize);
  unsigned __int8 SQLGetData_RetErrCode(
    const char *strQuery,
    unsigned __int16 *ColumnNumber,
    SQLSMALLINT TargetType,
    void *TargetValue);
  bool StartDataBase(const char *odbcName, const char *accountName, const char *passWord);
  bool TableExist(const char *szTableName);

  void *m_hEnv;
  void *m_hDbc;
  void *m_hStmtSelect;
  void *m_hStmtUpdate;
  bool m_bConectionActive;
  bool m_bSaveDBLog;
  CLogFile m_ProcessLogW;
  CLogFile m_ErrorLogW;
  CLogFile m_ProcessLogA;
  CLogFile m_ErrorLogA;
  unsigned __int8 m_byLogFileHour;
  char m_szOdbcName[32];
  char m_szAccountName[32];
  char m_szPassword[32];
  bool m_bReconnectFailExit;
  char m_szLogUpperPath[128];
};

