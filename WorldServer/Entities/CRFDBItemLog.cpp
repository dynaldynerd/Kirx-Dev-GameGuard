#include "pch.h"

#include "CRFDBItemLog.h"

#include <cstring>

CRFDBItemLog::CRFDBItemLog(unsigned int dwLocalDate)
  : m_bTblLtd(false), m_bTblLtdItemInfo(false), m_bTblLtdExpend(false), m_dwKorTime(dwLocalDate)
{
}

CRFDBItemLog::~CRFDBItemLog() = default;

bool CRFDBItemLog::CreateTblLtd(int nKorTime)
{
  char buffer[1040]{};
  std::memset(buffer, 0, 1024);
  sprintf_s(
    buffer,
    "CREATE TABLE [dbo].[tbl_ltd_%d] ( [Serial] [int] IDENTITY (1, 1) NOT NULL , [LogSerial] [datetime] NOT NULL, [MainTy"
    "pe] [tinyint] NOT NULL, [SubType] [tinyint] NOT NULL, [AccountSerial] [int] NOT NULL, [CharSerial] [int] NOT NULL, )"
    " ON [PRIMARY] ALTER TABLE [dbo].[tbl_ltd_%d] WITH NOCHECK ADD CONSTRAINT [PK_tbl_ltd_%d] PRIMARY KEY  CLUSTERED ([Se"
    "rial])  ON [PRIMARY] CREATE INDEX [IX_tbl_ltd_Logserial] ON [dbo].[tbl_ltd_%d]([LogSerial]) ON [PRIMARY] CREATE INDE"
    "X [IX_tbl_ltd_MainType] ON [dbo].[tbl_ltd_%d]([MainType]) ON [PRIMARY] CREATE INDEX [IX_tbl_ltd_SubType] ON [dbo].[t"
    "bl_ltd_%d]([SubType]) ON [PRIMARY] CREATE INDEX [IX_tbl_ltd_AccountSerial] ON [dbo].[tbl_ltd_%d]([AccountSerial]) ON"
    " [PRIMARY] CREATE INDEX [IX_tbl_ltd_CharSerial] ON [dbo].[tbl_ltd_%d]([CharSerial]) ON [PRIMARY]",
    nKorTime,
    nKorTime,
    nKorTime,
    nKorTime,
    nKorTime,
    nKorTime,
    nKorTime,
    nKorTime);
  return this->ExecUpdateQuery(buffer, true);
}

bool CRFDBItemLog::CreateTblLtd_Expend(int nKorTime)
{
  char buffer[1040]{};
  std::memset(buffer, 0, 1024);
  sprintf_s(
    buffer,
    " CREATE TABLE [dbo].[tbl_ltd_expend_%d] ( [LogSerial] [datetime] NOT NULL, [SubType] [tinyint] NOT NULL, [Expend] [n"
    "varchar] (128) NOT NULL ) ON [PRIMARY] CREATE INDEX [IX_tbl_ltd_expend_LogSerial] ON [dbo].[tbl_ltd_expend_%d]([LogS"
    "erial]) ON [PRIMARY] CREATE INDEX [IX_tbl_ltd_expend_SubType] ON [dbo].[tbl_ltd_expend_%d]([SubType]) ON [PRIMARY]",
    nKorTime,
    nKorTime,
    nKorTime);
  return this->ExecUpdateQuery(buffer, true);
}

bool CRFDBItemLog::CreateTblLtd_ItemInfo(int nKorTime)
{
  char buffer[1040]{};
  std::memset(buffer, 0, 1024);
  sprintf_s(
    buffer,
    " CREATE TABLE [dbo].[tbl_ltd_iteminfo_%d] ( [LogSerial] [datetime] NOT NULL, [SubType] [tinyint] NOT NULL, [ItemC] ["
    "varchar] (10) NOT NULL, [ItemU] [varchar] (10) NOT NULL, [ItemD] [int] NOT NULL, [ItemO] [tinyint] NULL, [Name] [var"
    "char] (64) NULL  ) ON [PRIMARY] CREATE INDEX [IX_tbl_ltd_iteminfo_LogSerial] ON [dbo].[tbl_ltd_iteminfo_%d]([LogSeri"
    "al]) ON [PRIMARY] CREATE INDEX [IX_tbl_ltd_iteminfo_SubType] ON [dbo].[tbl_ltd_iteminfo_%d]([SubType]) ON [PRIMARY]",
    nKorTime,
    nKorTime,
    nKorTime);
  return this->ExecUpdateQuery(buffer, true);
}

bool CRFDBItemLog::insert_ltd(_LTD *pl)
{
  char buffer[528]{};
  std::memset(buffer, 0, 512);
  sprintf_s(
    buffer,
    "insert into [dbo].[tbl_ltd_%d](LogSerial, MainType, SubType, AccountSerial, CharSerial) values ('%04d-%02d-%02d %02d"
    ":%02d:%02d.%03d',%d,%d,%d,%d)",
    this->m_dwKorTime,
    pl->m_timeLocal.wYear,
    pl->m_timeLocal.wMonth,
    pl->m_timeLocal.wDay,
    pl->m_timeLocal.wHour,
    pl->m_timeLocal.wMinute,
    pl->m_timeLocal.wSecond,
    pl->m_timeLocal.wMilliseconds,
    pl->m_byMainLogType,
    pl->m_bySubLogType,
    pl->m_dwAccountSerial,
    pl->m_dwCharSerial);
  return this->ExecUpdateQuery(buffer, true);
}

bool CRFDBItemLog::insert_iteminfo(_LTD_ITEMINFO *pi, unsigned __int8 byIndex)
{
  char buffer[264]{};
  std::memset(buffer, 0, 256);
  sprintf_s(
    buffer,
    "insert into tbl_ltd_iteminfo_%d (LogSerial, SubType, ItemC, ItemU, ItemD, ItemO, Name) values ('%04d-%02d-%02d %02d:"
    "%02d:%02d.%03d', %d, '%s', '%s', %d, %d, '%s')",
    this->m_dwKorTime,
    pi->m_timeLocal.wYear,
    pi->m_timeLocal.wMonth,
    pi->m_timeLocal.wDay,
    pi->m_timeLocal.wHour,
    pi->m_timeLocal.wMinute,
    pi->m_timeLocal.wSecond,
    pi->m_timeLocal.wMilliseconds,
    pi->m_bySubLogType,
    pi->m_ItemInfo[byIndex].m_szItemCode,
    pi->m_ItemInfo[byIndex].m_szUpgradeCode,
    pi->m_ItemInfo[byIndex].m_dwDur,
    pi->m_ItemInfo[byIndex].m_byOverlapNum,
    pi->m_ItemInfo[byIndex].m_szItemName);
  return this->ExecUpdateQuery(buffer, true);
}

bool CRFDBItemLog::insert_expend(_LTD_EXPEND *pe)
{
  char buffer[528]{};
  std::memset(buffer, 0, 512);
  sprintf_s(
    buffer,
    "insert into tbl_ltd_expend_%d(LogSerial, SubType, Expend)values ('%04d-%02d-%02d %02d:%02d:%02d.%03d', %d, '%s')",
    this->m_dwKorTime,
    pe->m_timeLocal.wYear,
    pe->m_timeLocal.wMonth,
    pe->m_timeLocal.wDay,
    pe->m_timeLocal.wHour,
    pe->m_timeLocal.wMinute,
    pe->m_timeLocal.wSecond,
    pe->m_timeLocal.wMilliseconds,
    pe->m_bySubLogType,
    pe->m_wszEtcInfo);
  return this->ExecUpdateQuery(buffer, true);
}

void CRFDBItemLog::SetKorTime(unsigned int dwKorTime)
{
  this->m_dwKorTime = dwKorTime;
}

