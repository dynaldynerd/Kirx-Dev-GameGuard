DROP TABLE IF EXISTS tbl_cashList;
DROP TABLE IF EXISTS tbl_CashLog;
DROP TABLE IF EXISTS tbl_catalog;
DROP TABLE IF EXISTS tbl_paylog;
DROP TABLE IF EXISTS tbl_UserStatus;

CREATE TABLE tbl_cashList(
    id TEXT NOT NULL,
    name TEXT NOT NULL,
    cost TEXT NOT NULL,
    PRIMARY KEY (id, name)
);

CREATE TABLE tbl_CashLog(
    Serial INTEGER PRIMARY KEY AUTOINCREMENT,
    Account TEXT NOT NULL,
    Char TEXT NOT NULL,
    ItemID TEXT NOT NULL,
    Amount INTEGER NOT NULL,
    Server TEXT NOT NULL,
    UID INTEGER NOT NULL,
    Cost INTEGER NOT NULL,
    Discount REAL NOT NULL,
    Return INTEGER NOT NULL
);

CREATE TABLE tbl_catalog(
    idx INTEGER PRIMARY KEY AUTOINCREMENT,
    tableCode INTEGER NOT NULL,
    itemCode TEXT NOT NULL,
    icon INTEGER NOT NULL,
    name TEXT NOT NULL,
    K INTEGER NOT NULL,
    D INTEGER NOT NULL,
    U INTEGER NOT NULL,
    T INTEGER NOT NULL,
    Price INTEGER NOT NULL
);

CREATE TABLE tbl_paylog(
    PayID INTEGER NOT NULL PRIMARY KEY,
    UserID TEXT NULL,
    sum REAL NOT NULL,
    accept INTEGER NOT NULL DEFAULT 0
);

CREATE TABLE tbl_UserStatus(
    serial INTEGER PRIMARY KEY AUTOINCREMENT,
    id TEXT NOT NULL COLLATE BINARY UNIQUE,
    Status INTEGER NOT NULL DEFAULT 1,
    DTStartPrem TEXT NULL DEFAULT CURRENT_TIMESTAMP,
    DTEndPrem TEXT NULL DEFAULT CURRENT_TIMESTAMP,
    Cash INTEGER NOT NULL DEFAULT 0
);

CREATE INDEX IF NOT EXISTS IX_tbl_UserStatus_id ON tbl_UserStatus(id);

-- SQLite has no stored procedure support.
-- Runtime equivalents for RF_ActivatePremium, RF_ChargeBalance, RF_CheckAccountStatus,
-- RF_CheckBalance, commit_pay, and related Billing actions must be handled in application code.
