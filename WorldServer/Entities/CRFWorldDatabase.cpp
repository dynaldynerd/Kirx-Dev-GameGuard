#include "pch.h"

#include "CRFWorldDatabase.h"
#include "WorldServerUtil.h"

#include <ctime>
#include <sql.h>
#include <sqlext.h>

CRFWorldDatabase::CRFWorldDatabase()
  : CRFNewDatabase()
{
}

CRFWorldDatabase::~CRFWorldDatabase() = default;

bool CRFWorldDatabase::Insert_RenamePotionLog(unsigned int dwSerial, char *wszOldName, char *wszNewName)
{
  char buffer[1040]{};
  sprintf(buffer, "{ CALL pInsert_RenamePotionLog( %d, '%s', '%s' ) }", dwSerial, wszOldName, wszNewName);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::Insert_SettlementOwnerLog(
  unsigned __int8 byNth,
  unsigned __int8 byRace,
  unsigned int dwGuildSerial,
  const char *wszGuildName,
  unsigned __int16 wRank,
  unsigned __int8 byGrade,
  long double dKillPvpPoint,
  long double dGuildBattlePvpPoint,
  unsigned int dwSumLv)
{
  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  char buffer[1040]{};
  sprintf(
    buffer,
    "{ CALL pInsert_settlementownerlog('%04d-%02d-%02d %02d:%02d', %u, %u, %d, '%s', %d, %u, %f, %f, %d) }",
    systemTime.wYear,
    systemTime.wMonth,
    systemTime.wDay,
    systemTime.wHour,
    systemTime.wMinute,
    byNth,
    byRace,
    dwGuildSerial,
    wszGuildName,
    wRank,
    byGrade,
    static_cast<double>(dKillPvpPoint),
    static_cast<double>(dGuildBattlePvpPoint),
    dwSumLv);
  return ExecUpdateQuery(buffer, true);
}

unsigned __int8 CRFWorldDatabase::Select_UnmannedTraderSearchGroupTotalRowCount(
  unsigned __int8 byType,
  unsigned __int8 byRace,
  unsigned __int8 byClass1,
  unsigned __int8 byClass2,
  unsigned __int8 byClass3,
  unsigned int *dwCount)
{
  char buffer[1028]{};
  sprintf_s(
    buffer,
    0x400u,
    "select count(si.serial) from [dbo].[tbl_utsingleiteminfo] as si join [dbo].[tbl_utsellinfo] as s on s.type = %u and "
    "s.race = %u and s.serial = si.serial join [dbo].[tbl_utresultinfo] as r on r.type = %u and s.serial = r.serial where"
    " r.state in (1, 2) and si.class1=%u and si.class2=%u and si.class3=%u",
    byType,
    byRace,
    byType,
    byClass1,
    byClass2,
    byClass3);

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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, dwCount, 0, &indicator);
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

        ErrorAction(ret, m_hStmtSelect);
        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        return 1;
      }

      unsigned __int8 result = 0;
      if (ret == SQL_NO_DATA)
      {
        result = 2;
      }
      else
      {
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

    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 1;
}

unsigned __int8 CRFWorldDatabase::Select_UnmannedTraderSearchPageInfo(
  unsigned __int8 byType,
  unsigned __int8 byRace,
  unsigned __int8 byClass1,
  unsigned __int8 byClass2,
  unsigned __int8 byClass3,
  unsigned int dwMaxRowCount,
  unsigned int dwExcludeRowCount,
  const char *szSortQuery,
  _unmannedtrader_page_info *pkInfo)
{
  char orderBuffer[152]{};
  if (szSortQuery)
  {
    sprintf_s(orderBuffer, 0x80u, "order by %s", szSortQuery);
  }

  static const char kEmptySortOuter[] = "";
  static const char kEmptySortInner[] = "";
  const char *outerOrder = orderBuffer[0] ? orderBuffer : kEmptySortOuter;
  const char *innerOrder = orderBuffer[0] ? orderBuffer : kEmptySortInner;

  char query[2080]{};
  sprintf_s(
    query,
    0x800u,
    "select top %u si.[serial], si.[k], si.[d], si.[u], s.[price], s.[owner], b.[Name], s.[regdate], s.[sellturm], si.[s]"
    ", si.[t] from [dbo].[tbl_utsingleiteminfo] as si join [dbo].[tbl_utsellinfo] as s on s.type=%u and s.race = %u and s"
    ".serial=si.serial join [dbo].[tbl_utresultinfo] as r on r.type=%u and s.serial=r.serial left join [dbo].[tbl_base] a"
    "s b on s.owner = b.serial where r.state in (1, 2) and si.class1=%u and si.class2=%u and si.class3=%u and si.serial n"
    "ot in ( select top %u si.serial from [dbo].[tbl_utsingleiteminfo] as si join [dbo].[tbl_utsellinfo] as s on s.type=%"
    "u and s.race = %u and s.serial = si.serial join [dbo].[tbl_utresultinfo] as r on r.type=%u and s.serial=r.serial whe"
    "re r.state in (1, 2) and si.class1=%u and si.class2=%u and si.class3=%u %s ) %s",
    dwMaxRowCount,
    byType,
    byRace,
    byType,
    byClass1,
    byClass2,
    byClass3,
    dwExcludeRowCount,
    byType,
    byRace,
    byType,
    byClass1,
    byClass2,
    byClass3,
    innerOrder,
    outerOrder);

  if (m_bSaveDBLog)
  {
    Log(query);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      pkInfo->dwCnt = 0;
      do
      {
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != SQL_SUCCESS_WITH_INFO)
        {
          break;
        }

        SQLLEN indicator = 0;
        SQLGetData(m_hStmtSelect, 1u, SQL_C_LONG, &pkInfo->list[pkInfo->dwCnt].dwSerial, 0, &indicator);
        SQLGetData(m_hStmtSelect, 2u, SQL_C_LONG, &pkInfo->list[pkInfo->dwCnt].dwK, 0, &indicator);
        SQLGetData(m_hStmtSelect, 3u, SQL_C_SBIGINT, &pkInfo->list[pkInfo->dwCnt].dwD, 0, &indicator);
        SQLGetData(m_hStmtSelect, 4u, SQL_C_LONG, &pkInfo->list[pkInfo->dwCnt].dwU, 0, &indicator);
        SQLGetData(m_hStmtSelect, 5u, SQL_C_ULONG, &pkInfo->list[pkInfo->dwCnt].dwPrice, 0, &indicator);
        SQLGetData(m_hStmtSelect, 6u, SQL_C_LONG, &pkInfo->list[pkInfo->dwCnt].dwOwner, 0, &indicator);
        pkInfo->list[pkInfo->dwCnt].wszOwnerName[0] = 0;
        SQLGetData(
          m_hStmtSelect,
          7u,
          SQL_C_CHAR,
          pkInfo->list[pkInfo->dwCnt].wszOwnerName,
          17,
          &indicator);
        SQL_TIMESTAMP_STRUCT timestamp{};
        SQLGetData(m_hStmtSelect, 8u, SQL_C_TYPE_TIMESTAMP, &timestamp, 0, &indicator);
        SQLGetData(m_hStmtSelect, 9u, SQL_C_TINYINT, &pkInfo->list[pkInfo->dwCnt].bySellturm, 0, &indicator);
        SQLGetData(m_hStmtSelect, 10u, SQL_C_SBIGINT, &pkInfo->list[pkInfo->dwCnt].lnUID, 0, &indicator);
        SQLGetData(m_hStmtSelect, 11u, SQL_C_LONG, &pkInfo->list[pkInfo->dwCnt].dwT, 0, &indicator);

        tm timeInfo{};
        timeInfo.tm_year = timestamp.year - 1900;
        timeInfo.tm_mon = timestamp.month - 1;
        timeInfo.tm_mday = timestamp.day;
        timeInfo.tm_hour = timestamp.hour;
        timeInfo.tm_min = timestamp.minute;
        timeInfo.tm_sec = timestamp.second;
        timeInfo.tm_isdst = -1;

        __time64_t regTime = mktime_3(&timeInfo);
        if (regTime == -1)
        {
          regTime = 0;
        }
        pkInfo->list[pkInfo->dwCnt++].tRegdate = regTime;
      }
      while (dwMaxRowCount > pkInfo->dwCnt);

      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      if (m_bSaveDBLog)
      {
        FmtLog("%s Success", query);
      }
      return 0;
    }

    if (ret == SQL_NO_DATA)
    {
      return 2;
    }

    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
  return 1;
}

