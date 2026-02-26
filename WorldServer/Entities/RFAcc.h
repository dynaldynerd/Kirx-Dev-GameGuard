#pragma once

#include "IdaCompat.h"

#include <mutex>
#include <sql.h>
#include <sqlext.h>

class CRFAcc
{
public:
  unsigned int Init();
  unsigned int Free();
  unsigned int CheckBalance(const char *szUserID);
  unsigned int ChargeBalance(const char *szUserID, const char *szCharName, const char *szItemCode, unsigned int dwOverlapNum, const char *szServer, unsigned __int64 lnUID, int nPrice, int nDiscount);
  unsigned int Cancel(unsigned __int64 lnUID);

private:
  bool ReadRfAccOptions(char *ip, unsigned __int64 ipSize, bool *trustedConnection, char *dbName, unsigned __int64 dbNameSize, char *id, unsigned __int64 idSize, char *password, unsigned __int64 passwordSize);
  bool IsNullOrEmpty(const char *value) const;
  bool TryConnectWithDriver(const char *driverName, const char *ip, bool trustedConnection, const char *dbName, const char *id, const char *password);
  bool OpenDatabaseConnection(const char *ip, bool trustedConnection, const char *dbName, const char *id, const char *password);
  bool PrepareStatement(SQLHSTMT *stmt, const char *query);
  bool PrepareStatements();
  bool IsInitialized() const;
  void CleanupStatements();
  void CleanupConnection();
  void CleanupAll();

private:
  std::mutex m_dbMutex;
  SQLHENV m_hEnv = SQL_NULL_HENV;
  SQLHDBC m_hDbc = SQL_NULL_HDBC;
  SQLHSTMT m_stmtCheckBalance = SQL_NULL_HSTMT;
  SQLHSTMT m_stmtInsertCashLog = SQL_NULL_HSTMT;
  SQLHSTMT m_stmtSelectUserCash = SQL_NULL_HSTMT;
  SQLHSTMT m_stmtUpdateUserCash = SQL_NULL_HSTMT;
  SQLHSTMT m_stmtUpdateCashLogReturn = SQL_NULL_HSTMT;
  bool m_initialized = false;
};
