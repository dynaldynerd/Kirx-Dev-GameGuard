#include "pch.h"

#include "RFAcc.h"
#include "WorldServerUtil.h"

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <windows.h>

bool CRFAcc::ReadRfAccOptions(char *ip, unsigned __int64 ipSize, bool *trustedConnection, char *dbName, unsigned __int64 dbNameSize, char *id, unsigned __int64 idSize, char *password, unsigned __int64 passwordSize) {
  char currentDir[MAX_PATH]{};
  if (!GetCurrentDirectoryA(static_cast<DWORD>(sizeof(currentDir)), currentDir)) { return false; }

  char iniPath[MAX_PATH]{};
  sprintf_s(iniPath, "%s\\Initialize\\settings.ini", currentDir);

  char trustedBuffer[16]{};
  if (!ReadOptionAndWriteDefault(iniPath, "Billing", "IP", "(local)", ip, ipSize)) { return false; }
  if (!ReadOptionAndWriteDefault(iniPath, "Billing", "trusted_connection", "1", trustedBuffer, sizeof(trustedBuffer))) { return false; }
  if (!ReadOptionAndWriteDefault(iniPath, "Billing", "DB", "BILLING", dbName, dbNameSize)) { return false; }
  if (!ReadOptionAndWriteDefault(iniPath, "Billing", "ID", "sa", id, idSize)) { return false; }
  if (!ReadOptionAndWriteDefault(iniPath, "Billing", "PWD", "", password, passwordSize)) { return false; }
  if (ip[0] == '\0' || dbName[0] == '\0') { return false; }

  *trustedConnection = atoi(trustedBuffer) != 0;
  return true;
}

bool CRFAcc::IsNullOrEmpty(const char *value) const { return !value || value[0] == '\0'; }

void CRFAcc::CleanupStatements() {
  if (m_stmtCheckBalance != SQL_NULL_HSTMT) { SQLFreeHandle(SQL_HANDLE_STMT, m_stmtCheckBalance); m_stmtCheckBalance = SQL_NULL_HSTMT; }
  if (m_stmtInsertCashLog != SQL_NULL_HSTMT) { SQLFreeHandle(SQL_HANDLE_STMT, m_stmtInsertCashLog); m_stmtInsertCashLog = SQL_NULL_HSTMT; }
  if (m_stmtSelectUserCash != SQL_NULL_HSTMT) { SQLFreeHandle(SQL_HANDLE_STMT, m_stmtSelectUserCash); m_stmtSelectUserCash = SQL_NULL_HSTMT; }
  if (m_stmtUpdateUserCash != SQL_NULL_HSTMT) { SQLFreeHandle(SQL_HANDLE_STMT, m_stmtUpdateUserCash); m_stmtUpdateUserCash = SQL_NULL_HSTMT; }
  if (m_stmtUpdateCashLogReturn != SQL_NULL_HSTMT) { SQLFreeHandle(SQL_HANDLE_STMT, m_stmtUpdateCashLogReturn); m_stmtUpdateCashLogReturn = SQL_NULL_HSTMT; }
}

void CRFAcc::CleanupConnection() {
  if (m_hDbc != SQL_NULL_HDBC) { SQLDisconnect(m_hDbc); SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc); m_hDbc = SQL_NULL_HDBC; }
  if (m_hEnv != SQL_NULL_HENV) { SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv); m_hEnv = SQL_NULL_HENV; }
}

void CRFAcc::CleanupAll() {
  CleanupStatements();
  CleanupConnection();
  m_initialized = false;
}

bool CRFAcc::IsInitialized() const { return m_initialized && m_hDbc != SQL_NULL_HDBC; }

bool CRFAcc::TryConnectWithDriver(const char *driverName, const char *ip, bool trustedConnection, const char *dbName, const char *id, const char *password) {
  char connectionString[512]{};
  if (trustedConnection) { sprintf_s(connectionString, "DRIVER={%s};SERVER=%s;DATABASE=%s;Trusted_Connection=Yes;", driverName, ip, dbName); } else { sprintf_s(connectionString, "DRIVER={%s};SERVER=%s;DATABASE=%s;UID=%s;PWD=%s;", driverName, ip, dbName, id, password); }
  const SQLRETURN ret = SQLDriverConnectA(m_hDbc, nullptr, reinterpret_cast<SQLCHAR *>(connectionString), SQL_NTS, nullptr, 0, nullptr, SQL_DRIVER_NOPROMPT);
  return SQL_SUCCEEDED(ret);
}

bool CRFAcc::OpenDatabaseConnection(const char *ip, bool trustedConnection, const char *dbName, const char *id, const char *password) {
  if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv))) { return false; }
  if (!SQL_SUCCEEDED(SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), SQL_IS_INTEGER))) { return false; }
  if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hDbc))) { return false; }
  SQLSetConnectAttr(m_hDbc, SQL_ATTR_LOGIN_TIMEOUT, reinterpret_cast<SQLPOINTER>(3), 0);

  const char *driverCandidates[] = {"ODBC Driver 18 for SQL Server", "ODBC Driver 17 for SQL Server", "SQL Server"};
  for (const char *driverName : driverCandidates) {
    if (TryConnectWithDriver(driverName, ip, trustedConnection, dbName, id, password)) { return true; }
  }
  return false;
}

bool CRFAcc::PrepareStatement(SQLHSTMT *stmt, const char *query) {
  if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, stmt))) { return false; }
  return SQL_SUCCEEDED(SQLPrepareA(*stmt, reinterpret_cast<SQLCHAR *>(const_cast<char *>(query)), SQL_NTS));
}

bool CRFAcc::PrepareStatements() {
  if (!PrepareStatement(&m_stmtCheckBalance, "SELECT Cash FROM dbo.tbl_UserStatus WITH (READUNCOMMITTED) WHERE ID=?;")) { return false; }
  if (!PrepareStatement(&m_stmtInsertCashLog, "INSERT INTO dbo.tbl_CashLog ([Account],[Char],[ItemID],[Amount],[Server],[UID],[Cost],[Discount],[Return]) VALUES (?,?,?,?,?,?,?,?,0);")) { return false; }
  if (!PrepareStatement(&m_stmtSelectUserCash, "SELECT Cash FROM dbo.tbl_UserStatus WHERE ID=?;")) { return false; }
  if (!PrepareStatement(&m_stmtUpdateUserCash, "UPDATE dbo.tbl_UserStatus SET Cash=? WHERE ID=?;")) { return false; }
  if (!PrepareStatement(&m_stmtUpdateCashLogReturn, "UPDATE dbo.tbl_CashLog SET [Return]=? WHERE Account=?;")) { return false; }
  return true;
}

unsigned int CRFAcc::Init() {
  std::lock_guard<std::mutex> lock(m_dbMutex);
  if (IsInitialized()) { return 1; }

  char ip[64]{};
  bool trustedConnection = false;
  char dbName[64]{};
  char id[64]{};
  char password[64]{};
  if (!ReadRfAccOptions(ip, sizeof(ip), &trustedConnection, dbName, sizeof(dbName), id, sizeof(id), password, sizeof(password))) { return 0; }
  
  CleanupAll();
  if (!OpenDatabaseConnection(ip, trustedConnection, dbName, id, password)) { CleanupAll(); return 0; }
  if (!PrepareStatements()) { CleanupAll(); return 0; }
  m_initialized = true;
  return 1;
}

unsigned int CRFAcc::Free() {
  std::lock_guard<std::mutex> lock(m_dbMutex);
  CleanupAll();
  return 1;
}

unsigned int CRFAcc::CheckBalance(const char *szUserID) {
  std::lock_guard<std::mutex> lock(m_dbMutex);
  if (!IsInitialized() || !szUserID || m_stmtCheckBalance == SQL_NULL_HSTMT) { return 0; }

  SQLCloseCursor(m_stmtCheckBalance);
  SQLFreeStmt(m_stmtCheckBalance, SQL_RESET_PARAMS);

  char userBuffer[65]{};
  strcpy_s(userBuffer, sizeof(userBuffer), szUserID);

  SQLLEN userLen = SQL_NTS;
  if (!SQL_SUCCEEDED(SQLBindParameter(m_stmtCheckBalance, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 64, 0, userBuffer, sizeof(userBuffer), &userLen))) { return 0; }
  if (!SQL_SUCCEEDED(SQLExecute(m_stmtCheckBalance))) { SQLCloseCursor(m_stmtCheckBalance); return 0; }

  const SQLRETURN fetchRet = SQLFetch(m_stmtCheckBalance);
  if (fetchRet == SQL_NO_DATA) { SQLCloseCursor(m_stmtCheckBalance); return 0; }
  if (!SQL_SUCCEEDED(fetchRet)) { SQLCloseCursor(m_stmtCheckBalance); return 0; }

  SQLINTEGER amount = 0;
  SQLLEN amountLen = SQL_NULL_DATA;
  const SQLRETURN getDataRet = SQLGetData(m_stmtCheckBalance, 1, SQL_C_SLONG, &amount, 0, &amountLen);
  SQLCloseCursor(m_stmtCheckBalance);
  if (!SQL_SUCCEEDED(getDataRet) || amountLen == SQL_NULL_DATA) { return 0; }
  if (amount <= 0) { return 0; }
  return static_cast<unsigned int>(amount);
}

unsigned int CRFAcc::ChargeBalance(const char *szUserID, const char *szCharName, const char *szItemCode, unsigned int dwOverlapNum, const char *szServer, unsigned __int64 lnUID, int nPrice, int nDiscount) {
  std::lock_guard<std::mutex> lock(m_dbMutex);
  if (!IsInitialized() || !szUserID || !szCharName || !szItemCode || !szServer || m_stmtInsertCashLog == SQL_NULL_HSTMT || m_stmtSelectUserCash == SQL_NULL_HSTMT || m_stmtUpdateUserCash == SQL_NULL_HSTMT || m_stmtUpdateCashLogReturn == SQL_NULL_HSTMT) { return 0; }

  SQLINTEGER priceValue = static_cast<SQLINTEGER>(nPrice);
  SQLINTEGER discountValue = static_cast<SQLINTEGER>(nDiscount);
  SQLINTEGER quantityValue = static_cast<SQLINTEGER>(dwOverlapNum);
  SQLBIGINT uidValue = static_cast<SQLBIGINT>(lnUID);
  char priceText[32]{};
  sprintf_s(priceText, "%d", static_cast<int>(priceValue));

  char accountBuffer[65]{};
  char characterBuffer[65]{};
  char itemCodeBuffer[65]{};
  char serverBuffer[65]{};
  strcpy_s(accountBuffer, sizeof(accountBuffer), szUserID);
  strcpy_s(characterBuffer, sizeof(characterBuffer), szCharName);
  strcpy_s(itemCodeBuffer, sizeof(itemCodeBuffer), szItemCode);
  strcpy_s(serverBuffer, sizeof(serverBuffer), szServer);

  SQLCloseCursor(m_stmtInsertCashLog);
  SQLFreeStmt(m_stmtInsertCashLog, SQL_RESET_PARAMS);

  SQLLEN accountLen = SQL_NTS;
  SQLLEN characterLen = SQL_NTS;
  SQLLEN itemCodeLen = SQL_NTS;
  SQLLEN amountLen = 0;
  SQLLEN serverLen = SQL_NTS;
  SQLLEN uidLen = 0;
  SQLLEN priceTextLen = SQL_NTS;
  SQLLEN discountLen = 0;

  if (!SQL_SUCCEEDED(SQLBindParameter(m_stmtInsertCashLog, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 32, 0, accountBuffer, sizeof(accountBuffer), &accountLen))) { return 0; }
  if (!SQL_SUCCEEDED(SQLBindParameter(m_stmtInsertCashLog, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20, 0, characterBuffer, sizeof(characterBuffer), &characterLen))) { return 0; }
  if (!SQL_SUCCEEDED(SQLBindParameter(m_stmtInsertCashLog, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20, 0, itemCodeBuffer, sizeof(itemCodeBuffer), &itemCodeLen))) { return 0; }
  if (!SQL_SUCCEEDED(SQLBindParameter(m_stmtInsertCashLog, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &quantityValue, 0, &amountLen))) { return 0; }
  if (!SQL_SUCCEEDED(SQLBindParameter(m_stmtInsertCashLog, 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20, 0, serverBuffer, sizeof(serverBuffer), &serverLen))) { return 0; }
  if (!SQL_SUCCEEDED(SQLBindParameter(m_stmtInsertCashLog, 6, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, 0, 0, &uidValue, 0, &uidLen))) { return 0; }
  if (!SQL_SUCCEEDED(SQLBindParameter(m_stmtInsertCashLog, 7, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20, 0, priceText, sizeof(priceText), &priceTextLen))) { return 0; }
  if (!SQL_SUCCEEDED(SQLBindParameter(m_stmtInsertCashLog, 8, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &discountValue, 0, &discountLen))) { return 0; }
  if (!SQL_SUCCEEDED(SQLExecute(m_stmtInsertCashLog))) { SQLCloseCursor(m_stmtInsertCashLog); return 0; }
  SQLCloseCursor(m_stmtInsertCashLog);

  if (IsNullOrEmpty(szUserID) || priceValue <= 0 || quantityValue <= 0 || discountValue < 0 || discountValue >= 100 || IsNullOrEmpty(szItemCode) || IsNullOrEmpty(szServer) || IsNullOrEmpty(szCharName)) { return 0; }

  SQLCloseCursor(m_stmtSelectUserCash);
  SQLFreeStmt(m_stmtSelectUserCash, SQL_RESET_PARAMS);

  SQLLEN selectAccountLen = SQL_NTS;
  if (!SQL_SUCCEEDED(SQLBindParameter(m_stmtSelectUserCash, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 32, 0, accountBuffer, sizeof(accountBuffer), &selectAccountLen))) { return 0; }
  if (!SQL_SUCCEEDED(SQLExecute(m_stmtSelectUserCash))) { SQLCloseCursor(m_stmtSelectUserCash); return 0; }
  if (!SQL_SUCCEEDED(SQLFetch(m_stmtSelectUserCash))) { SQLCloseCursor(m_stmtSelectUserCash); return 0; }

  SQLINTEGER cashBalance = 0;
  SQLLEN cashBalanceLen = 0;
  if (!SQL_SUCCEEDED(SQLGetData(m_stmtSelectUserCash, 1, SQL_C_SLONG, &cashBalance, 0, &cashBalanceLen))) { SQLCloseCursor(m_stmtSelectUserCash); return 0; }
  SQLCloseCursor(m_stmtSelectUserCash);

  long long totalAmount = 0;
  if (discountValue == 0) { totalAmount = static_cast<long long>(priceValue) * static_cast<long long>(quantityValue); } else { totalAmount = (static_cast<long long>(priceValue) * static_cast<long long>(quantityValue)) / static_cast<long long>(discountValue); }
  if (totalAmount > static_cast<long long>(cashBalance)) { return 0; }

  const long long newCashBalance64 = static_cast<long long>(cashBalance) - totalAmount;
  if (newCashBalance64 < static_cast<long long>(INT_MIN) || newCashBalance64 > static_cast<long long>(INT_MAX)) { return 0; }

  SQLINTEGER newCashBalance = static_cast<SQLINTEGER>(newCashBalance64);
  SQLCloseCursor(m_stmtUpdateUserCash);
  SQLFreeStmt(m_stmtUpdateUserCash, SQL_RESET_PARAMS);

  SQLLEN newCashLen = 0;
  SQLLEN updateAccountLen = SQL_NTS;
  if (!SQL_SUCCEEDED(SQLBindParameter(m_stmtUpdateUserCash, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &newCashBalance, 0, &newCashLen))) { return 0; }
  if (!SQL_SUCCEEDED(SQLBindParameter(m_stmtUpdateUserCash, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 32, 0, accountBuffer, sizeof(accountBuffer), &updateAccountLen))) { return 0; }
  if (!SQL_SUCCEEDED(SQLExecute(m_stmtUpdateUserCash))) { SQLCloseCursor(m_stmtUpdateUserCash); return 0; }
  SQLCloseCursor(m_stmtUpdateUserCash);

  SQLLEN affectedRows = 0;
  if (!SQL_SUCCEEDED(SQLRowCount(m_stmtUpdateUserCash, &affectedRows)) || affectedRows <= 0) { return 0; }

  SQLCloseCursor(m_stmtUpdateCashLogReturn);
  SQLFreeStmt(m_stmtUpdateCashLogReturn, SQL_RESET_PARAMS);

  SQLINTEGER returnCode = 1;
  SQLLEN returnCodeLen = 0;
  SQLLEN logAccountLen = SQL_NTS;
  bool canUpdateCashLog = true;
  if (!SQL_SUCCEEDED(SQLBindParameter(m_stmtUpdateCashLogReturn, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &returnCode, 0, &returnCodeLen))) { canUpdateCashLog = false; }
  if (canUpdateCashLog) { if (!SQL_SUCCEEDED(SQLBindParameter(m_stmtUpdateCashLogReturn, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 32, 0, accountBuffer, sizeof(accountBuffer), &logAccountLen))) { canUpdateCashLog = false; } }
  if (canUpdateCashLog) { SQLExecute(m_stmtUpdateCashLogReturn); }
  SQLCloseCursor(m_stmtUpdateCashLogReturn);
  return 1;
}

unsigned int CRFAcc::Cancel(unsigned __int64 lnUID) {
  (void)lnUID;
  return 0;
}
