#include "pch.h"

#include "CRFWorldDatabase.h"
#include "WorldServerUtil.h"

#include "CNationSettingManager.h"
#include "pvppoint_guild_rank_info.h"
#include "total_guild_rank_info.h"
#include "unmannedtrader_buy_item_info.h"
#include "unmannedtrader_registsingleitem.h"
#include "unmannedtrader_seller_info.h"
#include "weeklyguildrank_owner_info.h"
#include "sel_patriarch_elect_state.h"
#include "raceboss_acc_winrate.h"

#include <atlcomtime.h>

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

bool CRFWorldDatabase::Insert_NpcData(unsigned int dwSerial)
{
  char buffer[272]{};
  sprintf(buffer, "{ CALL pInsert_NpcData( %d ) }", dwSerial);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::Insert_NpcData(unsigned int dwSerial, unsigned int *pNpcData)
{
  char buffer[272]{};
  sprintf(
    buffer,
    "{ CALL pInsert_NpcData2( %d, %d, %d, %d, %d, %d, %d ) }",
    dwSerial,
    pNpcData[0],
    pNpcData[1],
    pNpcData[2],
    pNpcData[3],
    pNpcData[4],
    pNpcData[5]);
  return ExecUpdateQuery(buffer, true);
}

unsigned __int8 CRFWorldDatabase::Select_NpcData(unsigned int dwSerial, unsigned int *pNpcData)
{
  SQLLEN indicator = 0;
  char buffer[260]{};
  int column = 0;

  sprintf(buffer, "{ CALL pSelect_NpcData( %d ) }", dwSerial);
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
        for (int j = 0; j < 6; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_ULONG, &pNpcData[j], 0, &indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
        }
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
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
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

    ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 1;
}

bool CRFWorldDatabase::Update_NpcData(unsigned int dwSerial, unsigned int *pNpcData)
{
  char buffer[272]{};
  sprintf(
    buffer,
    "update tbl_NpcData set Npc0 = %d, Npc1 = %d, Npc2 = %d, Npc3 = %d, Npc4 = %d, Npc5 = %d where Serial=%d ",
    pNpcData[0],
    pNpcData[1],
    pNpcData[2],
    pNpcData[3],
    pNpcData[4],
    pNpcData[5],
    dwSerial);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::Insert_AnimusData(unsigned int dwSerial, long double *pVal)
{
  if (!pVal)
  {
    return false;
  }

  char buffer[1040]{};
  sprintf(
    buffer,
    "{ CALL pInsert_AnimusData( %u, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f ) }",
    dwSerial,
    static_cast<double>(pVal[0]),
    static_cast<double>(pVal[1]),
    static_cast<double>(pVal[2]),
    static_cast<double>(pVal[3]),
    static_cast<double>(pVal[4]),
    static_cast<double>(pVal[5]));
  return ExecUpdateQuery(buffer, true);
}

unsigned __int8 CRFWorldDatabase::Select_AnimusData(unsigned int dwSerial, unsigned __int8 byRace, long double *pVal)
{
  SQLLEN indicator = 0;
  char buffer[260]{};

  sprintf(buffer, "{ CALL pSelect_AnimusData( %d, %d ) }", dwSerial, byRace);
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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_DOUBLE, &pVal[0], 0, &indicator);
        if (!ret || ret == SQL_SUCCESS_WITH_INFO)
        {
          ret = SQLGetData(m_hStmtSelect, 2u, SQL_C_DOUBLE, &pVal[1], 0, &indicator);
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

          ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
          ErrorAction(ret, m_hStmtSelect);
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
          }
          return 1;
        }

        ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
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
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
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

    ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 1;
}

bool CRFWorldDatabase::Update_AnimusData(unsigned int dwSerial, unsigned __int8 byRace, long double *pAnimusData)
{
  if (byRace > 2u)
  {
    return false;
  }

  char buffer[1040]{};
  sprintf(
    buffer,
    "update [dbo].[tbl_animusdata] set Data%u = %.0f, Data%u = %.0f where Serial = %u",
    2 * byRace,
    static_cast<double>(pAnimusData[0]),
    2 * byRace + 1,
    static_cast<double>(pAnimusData[1]),
    dwSerial);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::Insert_NpcLog(
  unsigned int dwSerial,
  char *wszName,
  unsigned int dwIndex,
  unsigned int dwOrgVal,
  unsigned int dwChgVal)
{
  char buffer[272]{};
  sprintf(
    buffer,
    "{ CALL pInsert_NpcLog( %d, '%s', %d, %d, %d ) }",
    dwSerial,
    wszName,
    dwIndex,
    dwOrgVal,
    dwChgVal);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::Insert_AnimusLog(
  unsigned int dwSerial,
  char *wszName,
  unsigned int dwIndex,
  long double dOrgVal,
  long double dChgVal)
{
  char buffer[1040]{};
  sprintf(
    buffer,
    "{ CALL pInsert_AnimusLog (%u, '%s', %u, %f, %f) }",
    dwSerial,
    wszName,
    dwIndex,
    static_cast<double>(dOrgVal),
    static_cast<double>(dChgVal));
  return ExecUpdateQuery(buffer, true);
}

unsigned __int8 CRFWorldDatabase::Select_TrunkMoney(unsigned int dwSerial, long double *pVal)
{
  SQLLEN indicator = 0;
  char buffer[272]{};

  if (!pVal)
  {
    return 0;
  }

  sprintf(buffer, "{ CALL pSelect_TrunkMoney( %u ) }", dwSerial);
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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_DOUBLE, &pVal[0], 0, &indicator);
        if (!ret || ret == SQL_SUCCESS_WITH_INFO)
        {
          ret = SQLGetData(m_hStmtSelect, 2u, SQL_C_DOUBLE, &pVal[1], 0, &indicator);
          if (!ret || ret == SQL_SUCCESS_WITH_INFO)
          {
            ret = SQLGetData(m_hStmtSelect, 3u, SQL_C_DOUBLE, &pVal[2], 0, &indicator);
            if (!ret || ret == SQL_SUCCESS_WITH_INFO)
            {
              ret = SQLGetData(m_hStmtSelect, 4u, SQL_C_DOUBLE, &pVal[3], 0, &indicator);
              if (!ret || ret == SQL_SUCCESS_WITH_INFO)
              {
                ret = SQLGetData(m_hStmtSelect, 5u, SQL_C_DOUBLE, &pVal[4], 0, &indicator);
                if (!ret || ret == SQL_SUCCESS_WITH_INFO)
                {
                  ret = SQLGetData(m_hStmtSelect, 6u, SQL_C_DOUBLE, &pVal[5], 0, &indicator);
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
                    return 1;
                  }

                  ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
                  ErrorAction(ret, m_hStmtSelect);
                  if (m_hStmtSelect)
                  {
                    SQLCloseCursor(m_hStmtSelect);
                  }
                  return 0;
                }

                ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
                ErrorAction(ret, m_hStmtSelect);
                if (m_hStmtSelect)
                {
                  SQLCloseCursor(m_hStmtSelect);
                }
                return 0;
              }

              ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
              ErrorAction(ret, m_hStmtSelect);
              if (m_hStmtSelect)
              {
                SQLCloseCursor(m_hStmtSelect);
              }
              return 0;
            }

            ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
            ErrorAction(ret, m_hStmtSelect);
            if (m_hStmtSelect)
            {
              SQLCloseCursor(m_hStmtSelect);
            }
            return 0;
          }

          ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
          ErrorAction(ret, m_hStmtSelect);
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
          }
          return 0;
        }

        ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        return 0;
      }

      if (ret != SQL_NO_DATA)
      {
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return 0;
    }

    if (ret != SQL_NO_DATA)
    {
      ErrorMsgLog(ret, buffer, "SQLExecDirect", m_hStmtSelect);
      ErrorAction(ret, m_hStmtSelect);
    }
    return 0;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 0;
}

char CRFWorldDatabase::Select_UnitData(unsigned int dwSerial, long double *pUnitData)
{
  SQLLEN indicator = 0;
  char buffer[260]{};

  sprintf(buffer, "{ CALL pSelect_UnitData( %u ) }", dwSerial);
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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_DOUBLE, &pUnitData[0], 0, &indicator);
        if (!ret || ret == SQL_SUCCESS_WITH_INFO)
        {
          ret = SQLGetData(m_hStmtSelect, 2u, SQL_C_DOUBLE, &pUnitData[1], 0, &indicator);
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

          ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
          ErrorAction(ret, m_hStmtSelect);
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
          }
          return 1;
        }

        ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
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
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
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

    ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 1;
}

bool CRFWorldDatabase::Insert_UnitData(unsigned int dwSerial, long double *pUnitData)
{
  char buffer[1040]{};
  sprintf(
    buffer,
    "{ CALL pInsert_UnitData (%u, %.0f, %.0f) }",
    dwSerial,
    static_cast<double>(pUnitData[0]),
    static_cast<double>(pUnitData[1]));
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::Update_UnitData(unsigned int dwSerial, long double *pUnitData)
{
  char buffer[1040]{};
  sprintf(
    buffer,
    "{ CALL pUpdate_UnitData (%u, %.0f, %.0f) }",
    dwSerial,
    static_cast<double>(pUnitData[0]),
    static_cast<double>(pUnitData[1]));
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::Insert_UnitLog(
  unsigned int dwSerial,
  char *wszName,
  unsigned int dwIndex,
  long double dOrgVal,
  long double dChgVal)
{
  char buffer[1040]{};
  sprintf(
    buffer,
    "{ CALL pInsert_UnitLog (%u, '%s', %u, %f, %f) }",
    dwSerial,
    wszName,
    dwIndex,
    static_cast<double>(dOrgVal),
    static_cast<double>(dChgVal));
  return ExecUpdateQuery(buffer, true);
}

unsigned __int8 CRFWorldDatabase::Select_PlayerTimeLimitInfo(
  unsigned int dwAccountSerial,
  _worlddb_time_limit_info *pTimeLimiInfo)
{
  SQLLEN indicator = 0;
  char buffer[1048]{};

  sprintf_s(
    buffer,
    0x400u,
    "Select Fatigue, TLStatus, LastLogoutTime From [dbo].[tbl_TimeLimitInfo] Where AccountSerial = %d",
    dwAccountSerial);
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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, &pTimeLimiInfo->dwFatigue, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 2u, SQL_C_UTINYINT, &pTimeLimiInfo->byTLStatus, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 3u, SQL_C_ULONG, &pTimeLimiInfo->dwLastLogoutTime, 0, &indicator);
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

        if (ret != SQL_NO_DATA)
        {
          ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
          ErrorAction(ret, m_hStmtSelect);
        }
        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
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
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
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

unsigned __int8 CRFWorldDatabase::Select_PlayerTimeLimitInfo(
  unsigned int dwAccountSerial,
  unsigned int *pdwFatigue,
  unsigned __int8 *pbyStatus)
{
  SQLLEN indicator = 0;
  char buffer[1048]{};

  sprintf_s(
    buffer,
    0x400u,
    "Select Fatigue, TLStatus From [dbo].[tbl_TimeLimitInfo] Where AccountSerial = %d",
    dwAccountSerial);
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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, pdwFatigue, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 2u, SQL_C_UTINYINT, pbyStatus, 0, &indicator);
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

        if (ret != SQL_NO_DATA)
        {
          ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
          ErrorAction(ret, m_hStmtSelect);
        }
        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
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
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
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

bool CRFWorldDatabase::create_amine_personal()
{
  char buffer[272]{};
  memset(buffer, 0, 256);
  sprintf(buffer, "{ call pcreate_aminepersonal_inven }");
  return ExecUpdateQuery(buffer, true);
}

unsigned __int8 CRFWorldDatabase::select_amine_personal(unsigned int dwSerial)
{
  char buffer[260]{};
  memset(buffer, 0, 256);
  sprintf(buffer, "select top 1 avatorserial from [dbo].[tbl_aminepersonal_inven] where avatorserial = %u", dwSerial);

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

unsigned __int8 CRFWorldDatabase::select_amine_personal(unsigned int dwSerial, _personal_amine_inven *pInven)
{
  char buffer[260]{};
  memset(buffer, 0, 256);
  sprintf(buffer, "{ call pselect_aminepersonal_inven(%d) }", dwSerial);

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
        SQLLEN indicator[5]{};
        int column = 0;
        for (int j = 0; j < 40; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_ULONG, &pInven->list[j], 0, indicator);
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_BINARY, &pInven->list[j].byNum, 0, indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
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
        }
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

bool CRFWorldDatabase::insert_amine_personal(unsigned int dwSerial)
{
  char buffer[272]{};
  memset(buffer, 0, 256);
  sprintf(buffer, "{ call pinsert_aminepersonal_inven(%d) }", dwSerial);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::update_amine_personal(char *pQry)
{
  return ExecUpdateQuery(pQry, true);
}

bool CRFWorldDatabase::update_amine_dck(unsigned __int8 byType, unsigned __int8 byRace, unsigned int dwSerial)
{
  char buffer[144]{};
  memset(buffer, 0, 128);
  sprintf(buffer, "{ CALL pupdate_automine_dck(%d,%d,%d) }", byType, byRace, dwSerial);
  return ExecUpdateQuery(buffer, false);
}

bool CRFWorldDatabase::insert_amine_newowner(unsigned __int8 byType, unsigned __int8 byRace, unsigned int dwSerial)
{
  char buffer[144]{};
  memset(buffer, 0, 128);
  sprintf(buffer, "{ CALL pinsert_automine_newowner(%d,%d,%d) }", byType, byRace, dwSerial);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::update_amine_battery(
  unsigned __int8 byType,
  unsigned __int8 byRace,
  unsigned int dwSerial,
  unsigned int dwBattery)
{
  char buffer[144]{};
  memset(buffer, 0, 128);
  sprintf(buffer, "{ CALL pupdate_automine_battery(%d,%d,%d,%d) }", byType, byRace, dwSerial, dwBattery);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::update_amine_mineore(
  unsigned __int8 byType,
  unsigned __int8 byRace,
  unsigned int dwSerial,
  unsigned __int8 bySlot,
  unsigned int dwK,
  unsigned __int8 byNum,
  unsigned int dwGage)
{
  char buffer[1040]{};
  memset(buffer, 0, 1024);
  sprintf(
    buffer,
    "update [dbo].[tbl_automine_inven] set battery=%d, k_%d=%d, o_%d=%d where dck = 0 and collisiontype = %d and race = %d and guildserial = %d",
    dwGage,
    bySlot,
    dwK,
    bySlot,
    byNum,
    byType,
    byRace,
    dwSerial);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::update_amine_workstate(
  unsigned __int8 byType,
  unsigned __int8 byRace,
  unsigned int dwSerial,
  bool bWorking)
{
  char buffer[144]{};
  memset(buffer, 0, 128);
  sprintf(buffer, "{ CALL pupdate_automine_workingstate(%d,%d,%d,%d) }", byType, byRace, dwSerial, bWorking);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::update_amine_selore(
  unsigned __int8 byType,
  unsigned __int8 byRace,
  unsigned int dwSerial,
  unsigned __int8 byOreIdx)
{
  char buffer[144]{};
  memset(buffer, 0, 128);
  sprintf(buffer, "{ CALL pupdate_automine_selore(%d,%d,%d,%d) }", byType, byRace, dwSerial, byOreIdx);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::update_amine_moveore(
  unsigned __int8 byType,
  unsigned __int8 byRace,
  unsigned int dwSerial,
  unsigned __int8 bySSlot,
  unsigned int dwSK,
  unsigned __int8 bySNum,
  unsigned __int8 byDSlot,
  unsigned int dwDK,
  unsigned __int8 byDNum)
{
  char buffer[1040]{};
  memset(buffer, 0, 1024);
  sprintf(
    buffer,
    "update [dbo].[tbl_automine_inven] set k_%d=%d, o_%d=%d, k_%d=%d, o_%d=%d where dck = 0 and collisiontype = %d and race = %d and guildserial = %d",
    bySSlot,
    dwSK,
    bySSlot,
    bySNum,
    byDSlot,
    dwDK,
    byDSlot,
    byDNum,
    byType,
    byRace,
    dwSerial);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::CreateGuildBattleRankTable(char *szDate)
{
  char buffer[1040]{};
  sprintf(
    buffer,
    "select identity(int,1,1) as  rank, g.serial, g.race, g.grade, g.id, r.win, r.draw, r.lose, r.score into [dbo].[tbl_G"
    "uildBattleRank%s] from [dbo].[tbl_GuildBattleRank] as r join [dbo].[tbl_Guild] as g on r.serial = g.Serial and g.dck"
    " = 0 order by score desc, win desc, draw desc, lose",
    szDate);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::UpdateClearGuildBattleRank()
{
  char buffer[1040]{};
  sprintf(buffer, "{ CALL pUpdate_ClearGuildBattleRank }");
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::UpdateGuildBattleInfo(
  unsigned int dwID,
  unsigned int dwP1GuildSerial,
  unsigned int dwP2GuildSerial,
  unsigned int dwMapID,
  unsigned __int8 byNumber)
{
  char buffer[1040]{};
  sprintf(
    buffer,
    "{ CALL pUpdate_ReservedGuildBattleInfo( %u, %u, %u, %u, %u ) }",
    dwID,
    dwP1GuildSerial,
    dwP2GuildSerial,
    dwMapID,
    byNumber);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::UpdateGuildBattleScheduleInfo(
  unsigned int dwID,
  unsigned int dwSLID,
  unsigned __int8 byState,
  __int64 tStartTime,
  unsigned __int16 wTurmMin)
{
  ATL::CTime startTime(static_cast<__time64_t>(tStartTime));
  const int year = startTime.GetYear();
  const int month = startTime.GetMonth();
  const int day = startTime.GetDay();
  const int hour = startTime.GetHour();
  const int minute = startTime.GetMinute();
  const int second = startTime.GetSecond();

  char buffer[1048]{};
  sprintf(
    buffer,
    "{ CALL pUpdate_ScheduleInfo( %u, %u, '%04d-%02d-%02d %02d:%02d:%02d', %u ) }",
    dwID,
    byState,
    year,
    month,
    day,
    hour,
    minute,
    second,
    static_cast<unsigned int>(wTurmMin));
  return ExecUpdateQuery(buffer, true);
}

char CRFWorldDatabase::Select_GuildBattleRecord(unsigned int dwSerial, _guildbattle_totalrecord *pkInfo)
{
  if (!pkInfo)
  {
    return 0;
  }

  pkInfo->dwGuildSerial = dwSerial;

  char buffer[260]{};
  sprintf(buffer, "{ CALL pSelect_GuildBattleTotalRecord(%u) }", dwSerial);
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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, &pkInfo->dwTotWinCnt, 0, &indicator);
        if (!ret || ret == SQL_SUCCESS_WITH_INFO)
        {
          ret = SQLGetData(m_hStmtSelect, 2u, SQL_C_ULONG, &pkInfo->dwTotLoseCnt, 0, &indicator);
          if (!ret || ret == SQL_SUCCESS_WITH_INFO)
          {
            ret = SQLGetData(m_hStmtSelect, 3u, SQL_C_ULONG, &pkInfo->dwTotDrawCnt, 0, &indicator);
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
              return 1;
            }
          }
        }

        ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        return 0;
      }

      if (ret != SQL_NO_DATA)
      {
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return 0;
    }

    if (ret != SQL_NO_DATA)
    {
      ErrorMsgLog(ret, buffer, "SQLExecDirect", m_hStmtSelect);
      ErrorAction(ret, m_hStmtSelect);
    }
    return 0;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 0;
}

bool CRFWorldDatabase::Insert_GuildBatlleResultLogBattelInfo(
  char *szStartTime,
  char *szEndTime,
  unsigned int dwRedSerial,
  char *wszRedName,
  unsigned int dwBlueSerial,
  char *wszBlueName,
  unsigned int dwRedScore,
  unsigned int dwBlueScore,
  unsigned int dwRedMaxJoinCnt,
  unsigned int dwBlueMaxJoinCnt,
  unsigned int dwRedGoalCntSum,
  unsigned int dwBlueGoalCntSum,
  unsigned int dwRedKillCntSum,
  unsigned int dwBlueKillCntSum,
  unsigned __int8 byBattleResult,
  unsigned int dwMaxGoalCharacSerial,
  char *wszMaxGoalCharacName,
  unsigned int dwMaxKillCharacSerial,
  char *wszMaxKillCharacName,
  unsigned __int8 byJoinLimit,
  unsigned int dwGuildBattleCostGold,
  char *szBattleMapCode)
{
  char buffer[1040]{};
  sprintf_s(
    buffer,
    0x400u,
    "{ CALL pInsert_guildbattleresultlog('%s', '%s' , %d, '%s', %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %u, %d, '%s', %"
    "d, '%s', %u, %d, '%s') }",
    szStartTime,
    szEndTime,
    dwRedSerial,
    wszRedName,
    dwBlueSerial,
    wszBlueName,
    dwRedScore,
    dwBlueScore,
    dwRedMaxJoinCnt,
    dwBlueMaxJoinCnt,
    dwRedGoalCntSum,
    dwBlueGoalCntSum,
    dwRedKillCntSum,
    dwBlueKillCntSum,
    byBattleResult,
    dwMaxGoalCharacSerial,
    wszMaxGoalCharacName,
    dwMaxKillCharacSerial,
    wszMaxKillCharacName,
    byJoinLimit,
    dwGuildBattleCostGold,
    szBattleMapCode);
  return ExecUpdateQuery(buffer, true);
}

char CRFWorldDatabase::Select_BattleResultLogLatest(unsigned int *pkLogSerial)
{
  if (!pkLogSerial)
  {
    return 1;
  }

  *pkLogSerial = 0;

  char buffer[1028]{};
  sprintf(buffer, "{ CALL pSelect_guildbattleresulloglatest }");
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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, pkLogSerial, 0, &indicator);
        if (ret == SQL_NO_DATA)
        {
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
          }
          return 2;
        }

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
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
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

    ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 1;
}

bool CRFWorldDatabase::Update_BattleResultLogBattleResultAndPvpPoint(
  unsigned int kLogSerial,
  unsigned int dwRedSerial,
  unsigned int dwBlueSerial)
{
  char buffer[1024]{};
  sprintf_s(
    buffer,
    "{ CALL pUpdate_guildbattleresultlog( %u, %u, %u ) }",
    kLogSerial,
    dwRedSerial,
    dwBlueSerial);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::Insert_GuildBatlleResultLog(
  char *szStartTime,
  char *szEndTime,
  unsigned int dwRedSerial,
  char *wszRedName,
  unsigned int dwBlueSerial,
  char *wszBlueName,
  unsigned int dwRedScore,
  unsigned int dwBlueScore,
  unsigned int dwRedMaxJoinCnt,
  unsigned int dwBlueMaxJoinCnt,
  unsigned int dwRedGoalCntSum,
  unsigned int dwBlueGoalCntSum,
  unsigned int dwRedKillCntSum,
  unsigned int dwBlueKillCntSum,
  unsigned __int8 byBattleResult,
  unsigned int dwMaxGoalCharacSerial,
  char *wszMaxGoalCharacName,
  unsigned int dwMaxKillCharacSerial,
  char *wszMaxKillCharacName,
  unsigned __int8 byJoinLimit,
  unsigned int dwGuildBattleCostGold,
  char *szBattleMapCode)
{
  if (!Insert_GuildBatlleResultLogBattelInfo(
        szStartTime,
        szEndTime,
        dwRedSerial,
        wszRedName,
        dwBlueSerial,
        wszBlueName,
        dwRedScore,
        dwBlueScore,
        dwRedMaxJoinCnt,
        dwBlueMaxJoinCnt,
        dwRedGoalCntSum,
        dwBlueGoalCntSum,
        dwRedKillCntSum,
        dwBlueKillCntSum,
        byBattleResult,
        dwMaxGoalCharacSerial,
        wszMaxGoalCharacName,
        dwMaxKillCharacSerial,
        wszMaxKillCharacName,
        byJoinLimit,
        dwGuildBattleCostGold,
        szBattleMapCode))
  {
    return false;
  }

  unsigned int logSerial[1]{};
  return Select_BattleResultLogLatest(logSerial) && logSerial[0]
      && Update_BattleResultLogBattleResultAndPvpPoint(logSerial[0], dwRedSerial, dwBlueSerial);
}

unsigned __int8 CRFWorldDatabase::Select_TotalGuildRank(char *szDate, _total_guild_rank_info *pkInfo)
{
  if (!pkInfo)
  {
    return 0;
  }

  char buffer[1048]{};
  sprintf(
    buffer,
    "select top %u r.serial, r.GuildPower, g.id, g.race, g.grade, g.MasterSerial, (select name from [dbo].[tbl_base] wher"
    "e serial = g.MasterSerial ) as MasterName from [dbo].[tbl_GuildRank%s] as r join [dbo].[tbl_Guild] as g on r.serial "
    "= g.serial and g.dck = 0 order by race, rank",
    500,
    szDate);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), SQL_NTS);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      pkInfo->wCount = 0;
      for (;; ++pkInfo->wCount)
      {
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != SQL_SUCCESS_WITH_INFO)
        {
          break;
        }

        SQLLEN indicator = 0;
        ret = SQLGetData(
          m_hStmtSelect,
          1u,
          SQL_C_ULONG,
          &pkInfo->list[static_cast<unsigned __int64>(pkInfo->wCount)].dwSerial,
          0,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          2u,
          SQL_C_DOUBLE,
          &pkInfo->list[static_cast<unsigned __int64>(pkInfo->wCount)].dPowerPoint,
          0,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          3u,
          SQL_C_CHAR,
          pkInfo->list[static_cast<unsigned __int64>(pkInfo->wCount)].wszGuildName,
          17,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          2u,
          SQL_C_CHAR,
          pkInfo->list[static_cast<unsigned __int64>(pkInfo->wCount)].wszGuildName,
          17,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          4u,
          SQL_C_TINYINT,
          &pkInfo->list[static_cast<unsigned __int64>(pkInfo->wCount)].byRace,
          0,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          5u,
          SQL_C_TINYINT,
          &pkInfo->list[static_cast<unsigned __int64>(pkInfo->wCount)].byGrade,
          0,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          6u,
          SQL_C_ULONG,
          &pkInfo->list[static_cast<unsigned __int64>(pkInfo->wCount)].dwMasterSerial,
          0,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          7u,
          SQL_C_CHAR,
          pkInfo->list[static_cast<unsigned __int64>(pkInfo->wCount)].wszMasterName,
          17,
          &indicator);

        if (!pkInfo->list[static_cast<unsigned __int64>(pkInfo->wCount)].dwMasterSerial)
        {
          const char *noneString = CNationSettingManager::Instance()->GetNoneString();
          strcpy_0(
            pkInfo->list[static_cast<unsigned __int64>(pkInfo->wCount)].wszMasterName,
            noneString);
        }

        if (pkInfo->list[static_cast<unsigned __int64>(pkInfo->wCount)].byRace >= 3u)
        {
          ++pkInfo->wRaceCnt[3];
        }
        else
        {
          ++pkInfo->wRaceCnt[pkInfo->list[static_cast<unsigned __int64>(pkInfo->wCount)].byRace];
        }
        pkInfo->list[static_cast<unsigned __int64>(pkInfo->wCount)].wRank = 0;
      }

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

unsigned __int8 CRFWorldDatabase::Select_PvpPointGuildRank(char *szDate, _pvppoint_guild_rank_info *pkInfo)
{
  if (!pkInfo)
  {
    return 0;
  }

  char buffer[1048]{};
  sprintf(
    buffer,
    "select top %u r.[serial], r.[rank], r.[id], r.[race], r.[grade], r.[killpvppoint], r.[guildbattlepvppoint], r.[sumpv"
    "ppoint] from [dbo].[tbl_PvpPointGuildRank%s] as r join [dbo].[tbl_guild] as g on r.[serial] = g.[serial] and g.[dck]"
    " = 0 where r.[sumpvppoint] > 0 order by r.[race], r.[sumpvppoint] desc",
    500,
    szDate);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), SQL_NTS);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      for (pkInfo->wCount = 0;; ++pkInfo->wCount)
      {
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != SQL_SUCCESS_WITH_INFO)
        {
          break;
        }

        SQLLEN indicator = 0;
        ret = SQLGetData(
          m_hStmtSelect,
          1u,
          SQL_C_ULONG,
          &pkInfo->list[pkInfo->wCount],
          0,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          2u,
          SQL_C_USHORT,
          &pkInfo->list[pkInfo->wCount].wRank,
          0,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          3u,
          SQL_C_CHAR,
          pkInfo->list[pkInfo->wCount].wszGuildName,
          17,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          2u,
          SQL_C_CHAR,
          pkInfo->list[pkInfo->wCount].wszGuildName,
          17,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          4u,
          SQL_C_TINYINT,
          &pkInfo->list[pkInfo->wCount].byRace,
          0,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          5u,
          SQL_C_TINYINT,
          &pkInfo->list[pkInfo->wCount].byGrade,
          0,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          6u,
          SQL_C_DOUBLE,
          &pkInfo->list[pkInfo->wCount].dKillPvpPoint,
          0,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          7u,
          SQL_C_DOUBLE,
          &pkInfo->list[pkInfo->wCount].dGuildBattlePvpPoint,
          0,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          8u,
          SQL_C_DOUBLE,
          &pkInfo->list[pkInfo->wCount].dSumPvpPoint,
          0,
          &indicator);

        if (pkInfo->list[pkInfo->wCount].byRace >= 3u)
        {
          ++pkInfo->wRaceCnt[3];
        }
        else
        {
          ++pkInfo->wRaceCnt[pkInfo->list[pkInfo->wCount].byRace];
        }
      }

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

bool CRFWorldDatabase::Update_PvpPointGuildRankRecord(char *szDate, unsigned int dwSerial, unsigned __int16 wRank)
{
  char buffer[272]{};
  memset_0(buffer, 0, 0x100u);
  sprintf(buffer, "update [dbo].[tbl_PvpPointGuildRank%s] set rank=%u where serial=%u", szDate, wRank, dwSerial);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::Update_PvpPointGuildRankSumLv(
  char *szDate,
  unsigned __int8 byRace,
  unsigned __int8 byLimitCnt,
  unsigned __int8 byLimitGrade)
{
  char buffer[1040]{};
  memset_0(buffer, 0, 0x400u);
  sprintf(
    buffer,
    "update [dbo].[tbl_PvpPointGuildRank%s] set sumlv = nsl.newsumlv from ( select top %u serial, ( select sum(lv) from t"
    "bl_base as b join tbl_general as g on g.guildserial = [dbo].[tbl_PvpPointGuildRank%s].serial and b.serial = g.serial"
    " and b.dck = 0 ) as newsumlv from [dbo].[tbl_PvpPointGuildRank%s] where serial in ( select top 5 r.serial from [dbo]"
    ".[tbl_GuildBattleRank] as r join [dbo].[tbl_Guild] as g on r.serial = g.Serial where g.dck = 0 and g.race = %u and g"
    ".grade >= %u and ( r.win > 0 or r.lose > 0 or r.draw > 0 ) order by score desc, win desc, draw desc, lose ) order by"
    " sumpvppoint desc, killpvppoint desc, newsumlv desc ) as nsl where [dbo].[tbl_PvpPointGuildRank%s].serial = nsl.serial",
    szDate,
    static_cast<unsigned int>(byLimitCnt),
    szDate,
    szDate,
    static_cast<unsigned int>(byRace),
    static_cast<unsigned int>(byLimitGrade),
    szDate);
  return ExecUpdateQuery(buffer, false);
}

unsigned __int8 CRFWorldDatabase::Select_WeeklyGuildRankOwnerGuild(
  char *szDate,
  unsigned __int8 byRace,
  unsigned __int8 byLimitCnt,
  _weeklyguildrank_owner_info *pkInfo)
{
  if (!pkInfo || byRace >= 3u)
  {
    return 0;
  }

  char buffer[1048]{};
  sprintf(
    buffer,
    "select top %u serial, id, race, rank, grade, killpvppoint, guildbattlepvppoint, sumlv from [dbo].[tbl_PvpPointGuildR"
    "ank%s] as p where sumlv > 0 and race=%u order by sumpvppoint desc, killpvppoint desc, sumlv desc",
    byLimitCnt,
    szDate,
    byRace);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), SQL_NTS);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      for (pkInfo->wCount = static_cast<unsigned __int16>(2 * byRace);; ++pkInfo->wCount)
      {
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != SQL_SUCCESS_WITH_INFO)
        {
          break;
        }

        SQLLEN indicator = 0;
        ret = SQLGetData(
          m_hStmtSelect,
          1u,
          SQL_C_ULONG,
          &pkInfo->list[pkInfo->wCount],
          0,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          2u,
          SQL_C_CHAR,
          pkInfo->list[pkInfo->wCount].wszGuildName,
          17,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          2u,
          SQL_C_CHAR,
          pkInfo->list[pkInfo->wCount].wszGuildName,
          17,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          3u,
          SQL_C_TINYINT,
          &pkInfo->list[pkInfo->wCount].byRace,
          0,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          4u,
          SQL_C_USHORT,
          &pkInfo->list[pkInfo->wCount].wRank,
          0,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          5u,
          SQL_C_TINYINT,
          &pkInfo->list[pkInfo->wCount].byGrade,
          0,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          6u,
          SQL_C_DOUBLE,
          &pkInfo->list[pkInfo->wCount].dKillPvpPoint,
          0,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          7u,
          SQL_C_DOUBLE,
          &pkInfo->list[pkInfo->wCount].dGuildBattlePvpPoint,
          0,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          8u,
          SQL_C_ULONG,
          &pkInfo->list[pkInfo->wCount].dwSumLv,
          0,
          &indicator);

        if (pkInfo->list[pkInfo->wCount].byRace >= 3u)
        {
          ++pkInfo->wRaceCnt[3];
        }
        else
        {
          ++pkInfo->wRaceCnt[pkInfo->list[pkInfo->wCount].byRace];
        }
      }

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

bool CRFWorldDatabase::Create_PvpPointGuildRankTable(char *szDate)
{
  char buffer[1040]{};
  memset_0(buffer, 0, 0x400u);
  sprintf(
    buffer,
    "create table [dbo].[tbl_PvpPointGuildRank%s] ( [serial] [int] not null, [rank] [int] not null, [id] [varchar](17) no"
    "t null, [race]  [tinyint] not null, [grade] [smallint] not null, [killpvppoint] [float] not null, [guildbattlepvppoi"
    "nt] [float] not null, [sumpvppoint] [float] not null, [sumlv] [int] null constraint [DF_tbl_PvpPointGuildRank%s_suml"
    "v] default (0) constraint [tbl_PvpPointGuildRank%s_serial] primary key clustered ([serial]) on [Primary] ) on [PRIMARY]",
    szDate,
    szDate,
    szDate);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::Insert_PvpPointGuildRankData(char *szDate)
{
  char buffer[1040]{};
  memset_0(buffer, 0, 0x400u);
  sprintf(
    buffer,
    "insert into [dbo].[tbl_PvpPointGuildRank%s] select g.serial, 0 as rank, g.id, g.race, g.grade, s.killpvppoint, s.gui"
    "ldbattlepvppoint , s.killpvppoint+s.guildbattlepvppoint as sumpvppoint, 0 as sumlv from [dbo].[tbl_WeeklyGuildPVPPoi"
    "ntSum] as s join tbl_guild as g on s.serial = g.serial and g.dck = 0",
    szDate);
  return ExecUpdateQuery(buffer, false);
}

bool CRFWorldDatabase::Update_ClearWeeklyPvpPointSum()
{
  char buffer[272]{};
  memset_0(buffer, 0, 0x100u);
  sprintf(buffer, "update [dbo].[tbl_WeeklyGuildPVPPointSum] set killpvppoint = 0, guildbattlepvppoint = 0");
  return ExecUpdateQuery(buffer, false);
}

bool CRFWorldDatabase::Update_IncreaseWeeklyGuildGuildBattlePvpPointSum(unsigned int dwSerial, long double dPvpPoint)
{
  char buffer[1040]{};
  memset_0(buffer, 0, 0x400u);
  sprintf(buffer, "{ CALL pUpdate_WeeklyGuildGuildBattlePVPPoint(%u, %f) }", dwSerial, static_cast<double>(dPvpPoint));
  return ExecUpdateQuery(buffer, true);
}

__int64 CRFWorldDatabase::Select_PunishmentCount(unsigned __int8 byType, unsigned int dwAvatorSerial, unsigned int *pdwCnt)
{
  char buffer[260]{};
  sprintf(buffer, "{ CALL pSelect_PunishmentCount( %d, %d ) }", dwAvatorSerial, byType);

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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, pdwCnt, 0, &indicator);
        if (!ret || ret == SQL_SUCCESS_WITH_INFO)
        {
          if (m_bSaveDBLog)
          {
            FmtLog("%s Success", buffer);
          }
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
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
          ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
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
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
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

    ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 1;
}

bool CRFWorldDatabase::Update_Punishment(char *szData)
{
  unsigned int count = 0;
  const unsigned __int8 byType = static_cast<unsigned __int8>(szData[0]);
  const unsigned int avatorSerial = *reinterpret_cast<unsigned int *>(szData + 28);
  const int result = static_cast<int>(Select_PunishmentCount(byType, avatorSerial, &count));
  if (result == 2 || result == 1)
  {
    return false;
  }

  char buffer[264]{};
  if (count)
  {
    sprintf(
      buffer,
      "{ CALL pUpdate_Punishment( %d, %d, %d, %d ) }",
      *reinterpret_cast<unsigned int *>(szData + 28),
      *reinterpret_cast<unsigned int *>(szData + 32),
      byType,
      *reinterpret_cast<unsigned int *>(szData + 4));
  }
  else
  {
    sprintf(
      buffer,
      "{ CALL pInsert_Punishment( %d, %d, %d, %d ) }",
      *reinterpret_cast<unsigned int *>(szData + 28),
      *reinterpret_cast<unsigned int *>(szData + 32),
      byType,
      *reinterpret_cast<unsigned int *>(szData + 4));
  }
  return ExecUpdateQuery(buffer, true);
}

__int64 CRFWorldDatabase::Select_GetCharSerialByNameRace(
  char *pwszName,
  unsigned __int8 byRace,
  unsigned int *pSerial)
{
  char buffer[260]{};
  sprintf(buffer, "{ CALL pSelect_GetCharSerialByNameRace ( '%s', %d, %d ) }", pwszName, 2 * byRace, 2 * byRace + 1);

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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, pSerial, 0, &indicator);
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
          ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
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
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
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

    ErrorMsgLog(ret, buffer, "_SQLExecDirect", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 1;
}

unsigned __int8 CRFWorldDatabase::Select_GuildMasterLastConn(
  unsigned int dwSerial,
  unsigned int dwLimitConnTime,
  unsigned int *pdwLastConnTime)
{
  char buffer[260]{};
  sprintf(buffer, "{ CALL pSelect_GuildMasterLastConn( %d, %d ) }", dwSerial, dwLimitConnTime);

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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, pdwLastConnTime, 0, &indicator);
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
          ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
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
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
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

    ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 1;
}

char CRFWorldDatabase::Select_CharacterReName(char *pwszName, unsigned int *pSerial)
{
  char buffer[260]{};
  sprintf(buffer, "{ CALL pSelect_CharacterSerial ( '%s' ) }", pwszName);

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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, pSerial, 0, &indicator);
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
          return 1;
        }

        if (ret != SQL_NO_DATA)
        {
          ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
          ErrorAction(ret, m_hStmtSelect);
        }
        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        return 0;
      }

      unsigned __int8 result = 1;
      if (ret == SQL_NO_DATA)
      {
        result = 1;
      }
      else
      {
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
        result = 0;
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return result;
    }

    if (ret == SQL_NO_DATA)
    {
      return 1;
    }

    ErrorMsgLog(ret, buffer, "_SQLExecDirect", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 0;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 0;
}

bool CRFWorldDatabase::Update_CharacterReName(char *pwszName, unsigned int dwSerial)
{
  unsigned int serial = dwSerial;
  if (!Select_CharacterReName(pwszName, &serial))
  {
    return false;
  }

  char buffer[272]{};
  sprintf(buffer, "{ CALL pUpdate_CharacterReName(N'%s', %d ) }", pwszName, serial);
  return ExecUpdateQuery(buffer, true);
}

__int64 CRFWorldDatabase::Updatet_Account_Vote_Available(unsigned int dwSerial, unsigned __int8 *byVoteEnable)
{
  char buffer[280]{};
  memset(buffer, 0, 256);
  sprintf_s(
    buffer,
    0x100u,
    "declare @out_amount int exec pUpdatet_Account_Vote_Available  %d, @vote = @out_amount output select @out_amount",
    dwSerial);

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
        *byVoteEnable = 0;
        SQLLEN indicator = 0;
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, byVoteEnable, 0, &indicator);
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
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
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

bool CRFWorldDatabase::Update_Player_Vote_Info(
  unsigned int dwSerial,
  unsigned int dwAccPlayTime,
  unsigned __int8 IsVote,
  unsigned __int8 VoteEnable,
  unsigned int dwScanerData)
{
  char buffer[1040]{};
  memset(buffer, 0, 1024);
  sprintf_s(
    buffer,
    0x400u,
    "{ CALL pUpdate_Supplement_Ex_20080609( %d, %d, %d, %d, %d) }",
    dwSerial,
    dwAccPlayTime,
    IsVote,
    VoteEnable,
    dwScanerData);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::UpdateVotedReset_Supplement(unsigned int dwSerial)
{
  char buffer[1040]{};
  memset(buffer, 0, 1024);
  sprintf_s(
    buffer,
    0x400u,
    "Update [dbo].[tbl_Supplement] Set IsVoted = 0, VoteEnable = 1 From [dbo].[tbl_Supplement] as s join [dbo].[tbl_base]"
    " as b on s.serial = b.serial Where b.DCK = 0 AND b.accountserial = (select accountserial from tbl_base where serial =%d)",
    dwSerial);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::UpdateVotedReset_General(unsigned int dwSerial)
{
  char strQuery[1048]{};
  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  char dateBuffer[44]{};
  unsigned __int16 day = 0;
  if (systemTime.wDay >= 8u)
  {
    day = systemTime.wDay;
  }
  else
  {
    day = static_cast<unsigned __int16>(systemTime.wDay + 23);
    if (systemTime.wMonth >= 2u)
    {
      --systemTime.wMonth;
    }
    else
    {
      systemTime.wMonth = 12;
      --systemTime.wYear;
    }
  }

  sprintf_s(
    dateBuffer,
    0x1Eu,
    "%d-%d-%d %d:%d:%d",
    systemTime.wYear,
    systemTime.wMonth,
    day - 7,
    systemTime.wHour,
    systemTime.wMinute,
    systemTime.wSecond);
  sprintf_s(
    strQuery,
    0x400u,
    "Update tbl_general Set tmRaceBossVote = '%s' Where serial = %d",
    dateBuffer,
    dwSerial);
  return ExecUpdateQuery(strQuery, true);
}

bool CRFWorldDatabase::UpdateVotedReset_Cheat(unsigned int dwSerial)
{
  return UpdateVotedReset_Supplement(dwSerial) && UpdateVotedReset_General(dwSerial);
}

bool CRFWorldDatabase::UpdateServerResetToken(unsigned int dwToken, unsigned __int16 wProcType, unsigned int dwESerial)
{
  char buffer[1040]{};
  memset(buffer, 0, 1024);
  sprintf_s(buffer, 0x400u, "{ CALL pUpdate_Patriarch_Elect_ResetTime( %d, %d, %d ) }", dwToken, wProcType, dwESerial);
  return ExecUpdateQuery(buffer, true);
}

__int64 CRFWorldDatabase::Select_Player_Last_LogoutTime(unsigned int dwAccSerial, unsigned int *pdwLastLogoutTime)
{
  char buffer[1048]{};
  sprintf_s(
    buffer,
    0x400u,
    "Select LastLogoutTime From [dbo].[tbl_TimeLimitInfo] Where AccountSerial = %d",
    dwAccSerial);
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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, pdwLastLogoutTime, 0, &indicator);
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

        if (ret != SQL_NO_DATA)
        {
          ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
          ErrorAction(ret, m_hStmtSelect);
        }
        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
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
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
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

bool CRFWorldDatabase::Insert_PlayerTimeLimitInfo(unsigned int dwAccountSerial)
{
  char buffer[272]{};
  sprintf_s(buffer, 0x100u, "Insert into [dbo].[tbl_TimeLimitInfo] ( AccountSerial) values (%d)", dwAccountSerial);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::Update_Player_TimeLimit_Info(
  unsigned int dwAccSerial,
  unsigned int dwFatigue,
  unsigned __int8 wStatus)
{
  char buffer[1040]{};
  memset(buffer, 0, 1024);
  sprintf_s(buffer, 0x400u, "{ CALL pUpdate_TimeLimit_Info( %d, %d, %d) }", dwAccSerial, dwFatigue, wStatus);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::Insert_OreReset_Log(
  unsigned __int8 byType,
  int nLiveUsercnt,
  unsigned int dwOreRemain,
  unsigned int dwTAmount)
{
  char szDateTime[40]{};
  GetNowDateTime(szDateTime);

  char buffer[280]{};
  sprintf(
    buffer,
    "{CALL pInsert_RemainOre_Log (%d,'%s',%d,%d,%d)}",
    byType,
    szDateTime,
    nLiveUsercnt,
    dwOreRemain,
    dwTAmount);
  return ExecUpdateQuery(buffer, true);
}

__int64 CRFWorldDatabase::Select_PatriarchCommCount(unsigned int dwSerial, char *pszDate, unsigned int *dwCnt)
{
  char buffer[132]{};
  sprintf(
    buffer,
    "SELECT COUNT(*) FROM [dbo].[tbl_patriarch_comm] WHERE AvatorSerial=%d AND DepositDate='%s' AND DCK=0",
    dwSerial,
    pszDate);
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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_TINYINT, dwCnt, 0, &indicator);
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
          ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
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
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
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

    ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 1;
}

bool CRFWorldDatabase::Update_PatriarchComm(unsigned int dwSerial, unsigned int dwDalant, char *pszDepDate)
{
  char buffer[144]{};
  sprintf(buffer, "{ CALL pUpdate_PatriarchComm( %d, %d, '%s') }", dwSerial, dwDalant, pszDepDate);
  return ExecUpdateQuery(buffer, true);
}

unsigned __int8 CRFWorldDatabase::Insert_PatriarchComm(unsigned int dwSerial, unsigned int dwDalant, char *pszDepDate)
{
  unsigned int count = 0;
  const int result = static_cast<int>(Select_PatriarchCommCount(dwSerial, pszDepDate, &count));
  if (result == 1)
  {
    return 0;
  }
  if (result != 2 && count)
  {
    return Update_PatriarchComm(dwSerial, dwDalant, pszDepDate) != 0;
  }

  char buffer[132]{};
  sprintf(buffer, "{ CALL pInsert_PatriarchComm( %d, %d, '%s') }", dwSerial, dwDalant, pszDepDate);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::Insert_RaceBattleLog(_race_battle_log_info *pInfo)
{
  char szToday[56]{};
  GetTodayStr(szToday);

  char buffer[272]{};
  sprintf(
    buffer,
    "{ CALL pInsert_RaceBattleLog_070427( '%s', '%s', %d, %d, %d, %d, %d, %d, %d ) }",
    g_Main.m_wszWorldName,
    szToday,
    pInfo->byNth,
    pInfo->dwEndTime,
    pInfo->byWinRace,
    pInfo->byLoseRace,
    pInfo->dwBossSerilal0,
    pInfo->dwBossSerilal1,
    pInfo->dwBossSerilal2);
  return ExecUpdateQuery(buffer, true);
}

char CRFWorldDatabase::Select_CheckSumValue(unsigned int dwSerial, unsigned int *pdwCheckSum)
{
  char buffer[260]{};
  sprintf(buffer, "select FirstConnTime from tbl_base where serial=%d ", dwSerial);
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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, pdwCheckSum, 0, &indicator);
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
          return 1;
        }

        ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        return 0;
      }

      if (ret != SQL_NO_DATA)
      {
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return 0;
    }

    if (ret != SQL_NO_DATA)
    {
      ErrorMsgLog(ret, buffer, "SQLExecDirect", m_hStmtSelect);
      ErrorAction(ret, m_hStmtSelect);
    }
    return 0;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 0;
}

char CRFWorldDatabase::Select_PvpRate(
  unsigned int dwSerial,
  char *szDate,
  unsigned int *pdwRank,
  unsigned __int16 *pwRankRate)
{
  char buffer[260]{};
  sprintf(buffer, "select Rank, Rate from tbl_PvpRank%s where serial=%d", szDate, dwSerial);
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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, pdwRank, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 2u, SQL_C_USHORT, pwRankRate, 0, &indicator);
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

        ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
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
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
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

    ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 1;
}

bool CRFWorldDatabase::insert_atrade_taxrate(
  unsigned __int8 byRace,
  unsigned int dwSerial,
  char *wszName,
  unsigned int dwMatterDst,
  char *wszMatterDst,
  unsigned __int8 byCurrTax,
  unsigned int dwNext)
{
  char buffer[1040]{};
  memset(buffer, 0, 1024);
  sprintf(
    buffer,
    "Insert [dbo].[tbl_atradetaxrate] (Race, GSerial, GName, Tax, NextTax, suggester, suggestername) Values(%d, %d, '%s',"
    " %d, %d, %d, '%s')",
    byRace,
    dwSerial,
    wszName,
    byCurrTax,
    dwNext,
    dwMatterDst,
    wszMatterDst);
  return ExecUpdateQuery(buffer, true);
}

char CRFWorldDatabase::Select_CharacterName(
  unsigned int dwSerial,
  char *pwszCharacterName,
  char *szAccount)
{
  char buffer[260]{};
  sprintf(buffer, "select name, account from tbl_base where serial=%d", dwSerial);
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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_CHAR, pwszCharacterName, 17, &indicator);
        ret = SQLGetData(m_hStmtSelect, 2u, SQL_C_CHAR, szAccount, 17, &indicator);
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
          return 1;
        }

        ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        return 0;
      }

      if (ret != SQL_NO_DATA)
      {
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return 0;
    }

    if (ret != SQL_NO_DATA)
    {
      ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
      ErrorAction(ret, m_hStmtSelect);
    }
    return 0;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 0;
}

char CRFWorldDatabase::Select_ItemCharge(
  unsigned int dwAvatorSerial,
  unsigned __int8 *pbyType,
  unsigned int *pDwItemCode_K,
  unsigned __int64 *pDwItemCode_D,
  unsigned int *pDwItemCode_U,
  unsigned int *pDwItemChargeIndex,
  int *piTime)
{
  char buffer[260]{};
  sprintf(buffer, "{ CALL pSelect_ItemCharge_20070130( %d ) }", dwAvatorSerial);

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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, pDwItemChargeIndex, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 2u, SQL_C_ULONG, pDwItemCode_K, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 3u, SQL_C_UBIGINT, pDwItemCode_D, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 4u, SQL_C_ULONG, pDwItemCode_U, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 5u, SQL_C_TINYINT, pbyType, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 6u, SQL_C_ULONG, piTime, 0, &indicator);
        if (ret == SQL_NO_DATA)
        {
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
          }
          return 0;
        }

        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        if (m_bSaveDBLog)
        {
          FmtLog("%s Success", buffer);
        }
        return 1;
      }

      if (ret != SQL_NO_DATA)
      {
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return 0;
    }

    if (ret != SQL_NO_DATA)
    {
      ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
      ErrorAction(ret, m_hStmtSelect);
    }
    return 0;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 0;
}

char CRFWorldDatabase::Select_AccountItemCharge(
  unsigned int dwAccountSerial,
  unsigned __int8 *pbyType,
  long double *pdMoney,
  unsigned int *pdwItemCode_K,
  unsigned __int64 *pdwItemCode_D,
  unsigned int *pdwItemCode_U,
  unsigned __int8 *pbyRace,
  unsigned int *pdwDBID,
  int *piTime)
{
  char buffer[260]{};
  if (*pbyRace == 255)
  {
    sprintf(buffer, "{ CALL pSelect_TrunkItemChargeByType_20070420( %u, %d ) }", dwAccountSerial, *pbyType);
  }
  else
  {
    sprintf(
      buffer,
      "{ CALL pSelect_TrunkItemChargeByTypeRace_20070420( %u, %u, %u ) }",
      dwAccountSerial,
      *pbyType,
      *pbyRace);
  }

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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, pdwDBID, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 2u, SQL_C_TINYINT, pbyType, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 3u, SQL_C_DOUBLE, pdMoney, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 4u, SQL_C_ULONG, pdwItemCode_K, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 5u, SQL_C_UBIGINT, pdwItemCode_D, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 6u, SQL_C_ULONG, pdwItemCode_U, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 7u, SQL_C_TINYINT, pbyRace, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 8u, SQL_C_ULONG, piTime, 0, &indicator);
        if (ret == SQL_NO_DATA)
        {
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
          }
          return 0;
        }

        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        if (m_bSaveDBLog)
        {
          FmtLog("%s Success", buffer);
        }
        return 1;
      }

      if (ret != SQL_NO_DATA)
      {
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return 0;
    }

    if (ret != SQL_NO_DATA)
    {
      ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
      ErrorAction(ret, m_hStmtSelect);
    }
    return 0;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 0;
}

bool CRFWorldDatabase::Delete_ItemCharge(unsigned int dwItemChargeIndex)
{
  char buffer[272]{};
  sprintf(buffer, "{ CALL pDelete_ItemCharge( %d ) }", dwItemChargeIndex);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::Delete_TrunkItemCharge(unsigned int dwDBID)
{
  char buffer[272]{};
  sprintf(buffer, "{ CALL pDelete_TrunkCharge( %u ) }", dwDBID);
  return ExecUpdateQuery(buffer, true);
}

char CRFWorldDatabase::Select_LimitItemUsedRecord(
  unsigned __int8 byType,
  unsigned int dwTypeSerial,
  unsigned int dwStoreInx,
  unsigned int *pdwSerial)
{
  char buffer[260]{};
  sprintf(
    buffer,
    "select serial from tbl_StoreLimitItem_061212 where dck=0 and type=%d and typeserial=%d and storeinx=%d",
    byType,
    dwTypeSerial,
    dwStoreInx);
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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, pdwSerial, 0, &indicator);
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

        ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
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
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
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

char CRFWorldDatabase::Select_LimitItemEmptyRecord(unsigned int *pdwSerial)
{
  char buffer[132]{};
  sprintf(buffer, "select top 1 serial from tbl_StoreLimitItem_061212 where dck=1");
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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, pdwSerial, 0, &indicator);
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

        ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
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
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
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

char CRFWorldDatabase::Insert_LimitItemRecord(unsigned int *pdwSerial)
{
  char buffer[144]{};
  sprintf(buffer, "{ CALL pInsert_StoreLimitItemRecord_061212 }");
  if (!ExecUpdateQuery(buffer, true))
  {
    return 0;
  }

  sprintf(buffer, "Select count(serial) from tbl_StoreLimitItem_061212");
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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, pdwSerial, 0, &indicator);
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
          return 1;
        }

        ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        return 0;
      }

      if (ret != SQL_NO_DATA)
      {
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return 0;
    }

    if (ret != SQL_NO_DATA)
    {
      ErrorMsgLog(ret, buffer, "SQLExecDirect", m_hStmtSelect);
      ErrorAction(ret, m_hStmtSelect);
    }
    return 0;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 0;
}

bool CRFWorldDatabase::Update_LimitItemNum(char *pszQuery)
{
  return ExecUpdateQuery(pszQuery, true);
}

bool CRFWorldDatabase::Update_DisableInstanceStore(unsigned int dwSerial)
{
  char buffer[144]{};
  sprintf(buffer, "Update tbl_StoreLimitItem_061212 set dck=1 where serial=%d", dwSerial);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::Update_PostRegistry(
  unsigned int dwIndex,
  unsigned int dwSenderSerial,
  char *wszSendName,
  char *wszRecvName,
  char *wszTitle,
  char *wszContent,
  int nK,
  unsigned __int64 dwD,
  unsigned int dwU,
  unsigned int dwGold,
  unsigned __int8 bySendRace,
  unsigned __int8 bySenderDgr,
  unsigned __int64 lnUID)
{
  char buffer[1040]{};
  sprintf_s(
    buffer,
    0x400u,
    "update tbl_PostRegistry set dck=0,sendserial=%d,sendname='%s',recvname='%s',title='%s',content='%s',k=%d,d=%I64d,u=%"
    "d,gold=%d,sendrace=%d,userdgr=%d,uid=%I64d where serial=%d",
    dwSenderSerial,
    wszSendName,
    wszRecvName,
    wszTitle,
    wszContent,
    nK,
    dwD,
    dwU,
    dwGold,
    bySendRace,
    bySenderDgr,
    lnUID,
    dwIndex);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::Update_PostRegistryDisable(unsigned int dwIndex)
{
  char buffer[80]{};
  sprintf_s(buffer, 0x40u, "update tbl_PostRegistry set dck=1 where serial=%d", dwIndex);
  return ExecUpdateQuery(buffer, true);
}

char CRFWorldDatabase::Select_PostRecvSerialFromName(
  char *wszRecvName,
  unsigned int *pdwOutSerial,
  unsigned int *pdwAccSerial,
  unsigned int *pdwRace)
{
  char buffer[132]{};
  sprintf_s(buffer, 0x80u, "select Serial,AccountSerial,Race from tbl_base where Name='%s'", wszRecvName);
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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, pdwOutSerial, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 2u, SQL_C_ULONG, pdwAccSerial, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 3u, SQL_C_ULONG, pdwRace, 0, &indicator);
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

        ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
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
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
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

    ErrorMsgLog(ret, buffer, "_SQLExecDirect", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 1;
}

__int64 CRFWorldDatabase::Select_PostRecvStorageCheck(unsigned int dwSerial)
{
  unsigned __int16 count = 0;
  char buffer[144]{};
  sprintf_s(
    buffer,
    0x80u,
    "select count(serial) from tbl_PostStorage where owner=%d and dck=0 and poststate<%d",
    dwSerial,
    100);
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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_USHORT, &count, 0, &indicator);
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
          return count;
        }

        ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        return static_cast<unsigned int>(-2);
      }

      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      if (ret == SQL_NO_DATA)
      {
        return static_cast<unsigned int>(-1);
      }

      ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
      ErrorAction(ret, m_hStmtSelect);
      return static_cast<unsigned int>(-2);
    }

    if (ret == SQL_NO_DATA)
    {
      return static_cast<unsigned int>(-2);
    }

    ErrorMsgLog(ret, buffer, "SQLExecDirect", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return static_cast<unsigned int>(-1);
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return static_cast<unsigned int>(-1);
}

unsigned __int8 CRFWorldDatabase::Select_IsValidChar(unsigned int dwSerial, unsigned int *dwDbSerial)
{
  char buffer[260]{};
  sprintf(buffer, "{ CALL pSelect_IsValidChar( %d ) }", dwSerial);

  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == SQL_SUCCESS_WITH_INFO)
      {
        SQLLEN ind = 0;
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, dwDbSerial, 0LL, &ind);
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
          ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
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
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
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

    ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 1;
}

unsigned __int8 CRFWorldDatabase::Select_PatriarchWinCnt(
  unsigned __int8 byRace,
  unsigned int dwAvatorSerial,
  unsigned int *dwWinCnt)
{
  char buffer[516]{};
  sprintf(
    buffer,
    "select\tsum(WinCnt) from\t[dbo].[tbl_patriarch_candidate] where\tRace = %d and\tASerial = %d ",
    byRace,
    dwAvatorSerial);

  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == SQL_SUCCESS_WITH_INFO)
      {
        SQLLEN ind = 0;
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, dwWinCnt, 0LL, &ind);
        if (!ret || ret == SQL_SUCCESS_WITH_INFO)
        {
          if (m_bSaveDBLog)
          {
            FmtLog("%s Success", buffer);
          }
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
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
          ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
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
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
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

    ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 1;
}

unsigned __int8 CRFWorldDatabase::Select_PatriarchElectState(_sel_patriarch_elect_state *pSheet)
{
  char buffer[256]{};
  sprintf_s(buffer, "{ CALL pSelect_Patriarch_Elect_20080708 ( '%s' ) }", pSheet->szWorldName);

  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == SQL_SUCCESS_WITH_INFO)
      {
        SQLLEN ind = 0;
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, &pSheet->dwSerial, 0LL, &ind);
        ret = SQLGetData(m_hStmtSelect, 2u, SQL_C_USHORT, &pSheet->byProcType, 0LL, &ind);
        for (int i = 0; i < 3; ++i)
        {
          ret = SQLGetData(m_hStmtSelect, 3 * i + 3, SQL_C_ULONG, &pSheet->dwNonVoteCnt[i], 0LL, &ind);
          ret = SQLGetData(m_hStmtSelect, 3 * i + 4, SQL_C_ULONG, &pSheet->dwVoteCnt[i], 0LL, &ind);
          ret = SQLGetData(m_hStmtSelect, 3 * i + 5, SQL_C_ULONG, &pSheet->dwHighGradeNum[i], 0LL, &ind);
        }
        ret = SQLGetData(m_hStmtSelect, 12u, SQL_C_ULONG, &pSheet->dwResetServerToken, 0LL, &ind);
        if (!ret || ret == SQL_SUCCESS_WITH_INFO)
        {
          if (m_bSaveDBLog)
          {
            FmtLog("%s Success", buffer);
          }
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
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
          ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
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

unsigned __int8 CRFWorldDatabase::Select_UnmannedTraderReservedSchedule(
  unsigned int dwMaxCnt,
  _unmannedtrader_reserved_schedule_info *pkInfo)
{
  if (!pkInfo)
  {
    return 2;
  }

  char buffer[1048]{};
  sprintf(
    buffer,
    "select top %u s.type, s.serial, dateadd( hh, s.sellturm, s.regdate ) as endtime, s.owner, si.k from [dbo].[tbl_utsel"
    "linfo] as s join [dbo].[tbl_utresultinfo] as r on s.serial = r.serial and r.state in ( 1, 2 ) join [dbo].[tbl_utsing"
    "leiteminfo] as si on r.serial = si.serial order by endtime",
    dwMaxCnt);

  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), SQL_NTS);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      pkInfo->dwCnt = 0;
      for (;; ++pkInfo->dwCnt)
      {
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != SQL_SUCCESS_WITH_INFO)
        {
          break;
        }

        SQLLEN indicator = 0;
        ret = SQLGetData(
          m_hStmtSelect,
          1u,
          SQL_C_TINYINT,
          &pkInfo->list[pkInfo->dwCnt],
          0,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          2u,
          SQL_C_ULONG,
          &pkInfo->list[pkInfo->dwCnt].dwItemSerial,
          0,
          &indicator);

        SQL_TIMESTAMP_STRUCT timestamp{};
        ret = SQLGetData(m_hStmtSelect, 3u, SQL_C_TYPE_TIMESTAMP, &timestamp, 0, &indicator);

        ret = SQLGetData(
          m_hStmtSelect,
          4u,
          SQL_C_ULONG,
          &pkInfo->list[pkInfo->dwCnt].dwOwnerSerial,
          0,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          5u,
          SQL_C_ULONG,
          &pkInfo->list[pkInfo->dwCnt].dwK,
          0,
          &indicator);

        tm timeInfo{};
        timeInfo.tm_year = static_cast<int>(timestamp.year) - 1900;
        timeInfo.tm_mon = static_cast<int>(timestamp.month) - 1;
        timeInfo.tm_mday = static_cast<int>(timestamp.day);
        timeInfo.tm_hour = static_cast<int>(timestamp.hour);
        timeInfo.tm_min = static_cast<int>(timestamp.minute);
        timeInfo.tm_sec = static_cast<int>(timestamp.second);
        timeInfo.tm_isdst = -1;
        __time64_t endTime = mktime_3(&timeInfo);
        if (endTime == -1)
        {
          endTime = 0;
        }
        pkInfo->list[pkInfo->dwCnt].tEndTime = endTime;
      }

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

__int64 CRFWorldDatabase::Select_UnmannedTraderSingleItemEmptyRecordSerial(unsigned int *dwSerial)
{
  char buffer[260]{};
  sprintf(buffer, "select top 1 serial from tbl_utresultinfo where type=0 and state=0 order by serial");

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
        *dwSerial = 0;
        SQLLEN indicator = 0;
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, dwSerial, 0, &indicator);
        if (ret == SQL_NO_DATA)
        {
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
          }
          return 0xFFFFFFFBu;
        }

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

      unsigned int result = 0;
      if (ret == SQL_NO_DATA)
      {
        result = 0xFFFFFFFDu;
      }
      else
      {
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
        result = 0xFFFFFFFCu;
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return result;
    }

    if (ret == SQL_NO_DATA)
    {
      return 0xFFFFFFFEu;
    }

    ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 0;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 0xFFFFFFFFu;
}

char CRFWorldDatabase::Select_UnmannedTraderSingleItemBottomSerial(unsigned int *dwSerial)
{
  char buffer[260]{};
  sprintf(buffer, "select top 1 serial from [dbo].[tbl_utsingleiteminfo] order by serial desc");
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
        *dwSerial = 0;
        SQLLEN indicator = 0;
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, dwSerial, 0, &indicator);
        if (ret == SQL_NO_DATA)
        {
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
          }
          return 0;
        }

        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        if (m_bSaveDBLog)
        {
          FmtLog("%s Success", buffer);
        }
        return 1;
      }

      if (ret != SQL_NO_DATA)
      {
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return 0;
    }

    if (ret != SQL_NO_DATA)
    {
      ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
      ErrorAction(ret, m_hStmtSelect);
    }
    return 0;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 0;
}

char CRFWorldDatabase::Insert_UnmannedTraderSingleDefaultRecord(unsigned int dwRowCnt)
{
  if (!dwRowCnt)
  {
    return 0;
  }

  unsigned int serial[1]{};
  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  for (unsigned int j = 0; j < dwRowCnt; ++j)
  {
    char buffer[1044]{};
    sprintf(buffer, "{ CALL pInsert_defaultutsingleiteminfo }");
    if (!ExecUpdateQuery(buffer, true))
    {
      return 0;
    }

    if (!Select_UnmannedTraderSingleItemBottomSerial(serial))
    {
      return 0;
    }

    SetAutoCommitMode(false);
    sprintf(
      buffer,
      "{ CALL pInsert_defaultutsellinfo(0,%u,'%04d-%02d-%02d %02d:%02d:%02d.%03d') }",
      serial[0],
      systemTime.wYear,
      systemTime.wMonth,
      systemTime.wDay,
      systemTime.wHour,
      systemTime.wMinute,
      systemTime.wSecond,
      systemTime.wMilliseconds);
    if (!ExecUpdateQuery(buffer, true))
    {
      RollbackTransaction();
      SetAutoCommitMode(true);
      return 0;
    }

    sprintf(
      buffer,
      "{ CALL pInsert_defaultutresultinfo(0,%u,'%04d-%02d-%02d %02d:%02d:%02d.%03d') }",
      serial[0],
      systemTime.wYear,
      systemTime.wMonth,
      systemTime.wDay,
      systemTime.wHour,
      systemTime.wMinute,
      systemTime.wSecond,
      systemTime.wMilliseconds);
    if (!ExecUpdateQuery(buffer, true))
    {
      RollbackTransaction();
      SetAutoCommitMode(true);
      return 0;
    }

    CommitTransaction();
    SetAutoCommitMode(true);
  }

  return 1;
}

char CRFWorldDatabase::Select_UnmannedTraderItemState(
  unsigned __int8 byType,
  unsigned int dwRegistSerial,
  unsigned __int8 *byState)
{
  char buffer[260]{};
  sprintf(buffer, "{ CALL pSelect_utresultinfostate ( %u, %u ) }", byType, dwRegistSerial);
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
        *byState = static_cast<unsigned __int8>(-1);
        SQLLEN indicator = 0;
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_TINYINT, byState, 0, &indicator);
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

        ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
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
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
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

    ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 1;
}

bool CRFWorldDatabase::Update_UnmannedTraderItemState(
  unsigned __int8 byType,
  unsigned int dwItemSerial,
  unsigned __int8 byState,
  const _SYSTEMTIME *kCurTime)
{
  char buffer[1040]{};
  sprintf(
    buffer,
    "{ CALL pUpdate_utitemstate( %u, %d, %u,'%04d-%02d-%02d %02d:%02d:%02d.%03d' ) }",
    byType,
    dwItemSerial,
    byState,
    kCurTime->wYear,
    kCurTime->wMonth,
    kCurTime->wDay,
    kCurTime->wHour,
    kCurTime->wMinute,
    kCurTime->wSecond,
    kCurTime->wMilliseconds);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::Update_UnmannedTraderSingleItemInfo(
  unsigned int dwRegSerial,
  const _unmannedtrader_registsingleitem *kInfo)
{
  char buffer[1040]{};
  sprintf_s(
    buffer,
    0x400u,
    "{ CALL pUpdate_utsingleiteminfo_20070601( %d, %u, %d, %I64d, %d, %u, %u, %u, %u, %u, %I64d, %d ) }",
    dwRegSerial,
    kInfo->byInveninx,
    kInfo->dwK,
    kInfo->dwD,
    kInfo->dwU,
    kInfo->byLv,
    kInfo->byGrade,
    kInfo->byClass1,
    kInfo->byClass2,
    kInfo->byClass3,
    kInfo->lnUID,
    kInfo->dwT);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::Update_UnmannedTraderSellInfo(
  unsigned int dwRegSerial,
  const _unmannedtrader_registsingleitem *kInfo,
  const _SYSTEMTIME *kCurTime)
{
  char buffer[1040]{};
  sprintf(
    buffer,
    "{ CALL pUpdate_sellInfo( %u, %d, %u,'%04d-%02d-%02d %02d:%02d:%02d.%03d', %d, %u, %u ) }",
    kInfo->byType,
    dwRegSerial,
    kInfo->byRace,
    kCurTime->wYear,
    kCurTime->wMonth,
    kCurTime->wDay,
    kCurTime->wHour,
    kCurTime->wMinute,
    kCurTime->wSecond,
    kCurTime->wMilliseconds,
    kInfo->dwOwnerSerial,
    kInfo->dwPrice,
    kInfo->bySellTurm);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::Update_UnmannedTraderResutlInfo(
  unsigned __int8 byType,
  unsigned int dwRegistSerial,
  unsigned __int8 byState,
  unsigned int dwBuyer,
  unsigned int dwTax,
  const _SYSTEMTIME *kCurTime)
{
  char buffer[1040]{};
  sprintf(
    buffer,
    "{ CALL pUpdate_utresutlinfobuy( %u, %u, %u, %u, %u,'%04d-%02d-%02d %02d:%02d:%02d.%03d' ) }",
    byType,
    dwRegistSerial,
    byState,
    dwBuyer,
    dwTax,
    kCurTime->wYear,
    kCurTime->wMonth,
    kCurTime->wDay,
    kCurTime->wHour,
    kCurTime->wMinute,
    kCurTime->wSecond,
    kCurTime->wMilliseconds);
  return ExecUpdateQuery(buffer, true);
}

char CRFWorldDatabase::Regist_UnmannedTraderSingleItem(
  unsigned int dwRegSerial,
  const _unmannedtrader_registsingleitem *kInfo,
  bool bInsertRecord)
{
  SetAutoCommitMode(false);

  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  const unsigned __int8 resultFlag = bInsertRecord ? 2 : 1;
  if (Update_UnmannedTraderSingleItemInfo(dwRegSerial, kInfo)
      && Update_UnmannedTraderSellInfo(dwRegSerial, kInfo, &systemTime)
      && Update_UnmannedTraderResutlInfo(0, dwRegSerial, resultFlag, 0, 0, &systemTime))
  {
    CommitTransaction();
    SetAutoCommitMode(true);
    return 1;
  }

  RollbackTransaction();
  SetAutoCommitMode(true);
  return 0;
}

char CRFWorldDatabase::Select_UnmannedTraderBuySingleItemInfo(
  unsigned __int8 byType,
  unsigned int dwRegistSerial,
  _unmannedtrader_buy_item_info *kData)
{
  char buffer[260]{};
  sprintf_s(buffer, 0x100u, "{ CALL pSelect_utbuysingleiteminfo_20061115( %u, %u ) }", byType, dwRegistSerial);
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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_TINYINT, kData, 0, &indicator);
        if (!ret || ret == SQL_SUCCESS_WITH_INFO)
        {
          ret = SQLGetData(m_hStmtSelect, 2u, SQL_C_ULONG, &kData->dwK, 0, &indicator);
          if (!ret || ret == SQL_SUCCESS_WITH_INFO)
          {
            ret = SQLGetData(m_hStmtSelect, 3u, SQL_C_UBIGINT, &kData->dwD, 0, &indicator);
            if (!ret || ret == SQL_SUCCESS_WITH_INFO)
            {
              ret = SQLGetData(m_hStmtSelect, 4u, SQL_C_ULONG, &kData->dwU, 0, &indicator);
              if (!ret || ret == SQL_SUCCESS_WITH_INFO)
              {
                ret = SQLGetData(m_hStmtSelect, 5u, SQL_C_ULONG, &kData->dwPrice, 0, &indicator);
                if (!ret || ret == SQL_SUCCESS_WITH_INFO)
                {
                  ret = SQLGetData(m_hStmtSelect, 6u, SQL_C_ULONG, &kData->dwSeller, 0, &indicator);
                  if (!ret || ret == SQL_SUCCESS_WITH_INFO)
                  {
                    ret = SQLGetData(m_hStmtSelect, 7u, SQL_C_UBIGINT, &kData->lnUID, 0, &indicator);
                    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
                    {
                      ret = SQLGetData(m_hStmtSelect, 8u, SQL_C_ULONG, &kData->dwT, 0, &indicator);
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
                    }
                  }
                }
              }
            }
          }
        }

        ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
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
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
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

    ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 1;
}

bool CRFWorldDatabase::Update_UnmannedTraderSellInfoPrice(
  unsigned __int8 byType,
  unsigned int dwRegistSerial,
  unsigned int dwOwner,
  unsigned int dwNewPrice)
{
  char buffer[1040]{};
  sprintf(buffer, "{ CALL pUpdate_utsellinfoprice( %u, %u, %u, %u ) }", byType, dwRegistSerial, dwOwner, dwNewPrice);
  return ExecUpdateQuery(buffer, false);
}

char CRFWorldDatabase::Select_UnmannedTraderSellInfo(
  unsigned __int8 byType,
  unsigned int dwRegistSerial,
  unsigned __int8 byRace,
  _unmannedtrader_seller_info *kData)
{
  char buffer[260]{};
  sprintf(buffer, "{ CALL pSelect_utsellinfo( %u, %u, %u ) }", byType, dwRegistSerial, byRace);
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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, kData, 0, &indicator);
        if (!ret || ret == SQL_SUCCESS_WITH_INFO)
        {
          ret = SQLGetData(m_hStmtSelect, 2u, SQL_C_ULONG, &kData->dwSeller, 0, &indicator);
          if (!ret || ret == SQL_SUCCESS_WITH_INFO)
          {
            ret = SQLGetData(m_hStmtSelect, 3u, SQL_C_TINYINT, &kData->byRaceSexCode, 0, &indicator);
            if (!ret || ret == SQL_SUCCESS_WITH_INFO)
            {
              ret = SQLGetData(m_hStmtSelect, 4u, SQL_C_ULONG, &kData->dwDalant, 0, &indicator);
              if (!ret || ret == SQL_SUCCESS_WITH_INFO)
              {
                ret = SQLGetData(m_hStmtSelect, 5u, SQL_C_ULONG, &kData->dwGuildSerial, 0, &indicator);
                if (!ret || ret == SQL_SUCCESS_WITH_INFO)
                {
                  ret = SQLGetData(m_hStmtSelect, 6u, SQL_C_ULONG, &kData->dwAccountSerial, 0, &indicator);
                  if (!ret || ret == SQL_SUCCESS_WITH_INFO)
                  {
                    ret = SQLGetData(m_hStmtSelect, 7u, SQL_C_CHAR, kData->szAccountID, 17, &indicator);
                    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
                    {
                      ret = SQLGetData(m_hStmtSelect, 8u, SQL_C_CHAR, kData->wszName, 17, &indicator);
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
                    }
                  }
                }
              }
            }
          }
        }

        ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
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
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
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

    ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 1;
}

unsigned __int8 CRFWorldDatabase::Select_utSellWaitItems_SalesTotals(
  unsigned __int8 byType,
  unsigned int dwOwnor,
  unsigned int *pSalesTotals)
{
  char buffer[1024]{};
  sprintf_s(buffer, sizeof(buffer), "{ CALL pSelect_utsellwaititems_salestotals( %u, %d ) }", byType, dwOwnor);

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

  unsigned __int16 column = 1;
  result = SQLGetData_RetErrCode(buffer, &column, SQL_C_ULONG, pSalesTotals);
  if (result)
  {
    return result;
  }
  SelectCleanUp(buffer);
  return 0;
}

unsigned __int8 CRFWorldDatabase::Select_UnmannedTraderRegister(
  unsigned __int8 byType,
  unsigned int dwRegistSerial,
  unsigned int *pdwRegister)
{
  char buffer[1040]{};
  sprintf_s(buffer, 0x400u, "{ CALL pSelect_utsellinfo_owner( %u, %u ) }", byType, dwRegistSerial);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (!m_hStmtSelect && !ReConnectDataBase())
  {
    ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
    return 1;
  }

  SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), SQL_NTS);
  if (ret && ret != SQL_SUCCESS_WITH_INFO)
  {
    if (ret == SQL_NO_DATA)
    {
      return 2;
    }
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ret = SQLFetch(m_hStmtSelect);
  if (ret && ret != SQL_SUCCESS_WITH_INFO)
  {
    if (m_hStmtSelect)
    {
      SQLCloseCursor(m_hStmtSelect);
    }
    if (ret == SQL_NO_DATA)
    {
      return 2;
    }
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  SQLLEN indicator = 0;
  ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, pdwRegister, 0, &indicator);
  if (ret && ret != SQL_SUCCESS_WITH_INFO)
  {
    if (m_hStmtSelect)
    {
      SQLCloseCursor(m_hStmtSelect);
    }
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

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

unsigned __int8 CRFWorldDatabase::Update_UnmannedTraderReRegist(
  unsigned __int8 byType,
  unsigned int dwRegistSerial,
  unsigned __int8 byState,
  unsigned int dwPrice,
  unsigned int dwTax,
  const _SYSTEMTIME *kCurTime)
{
  char buffer[1040]{};
  memset(buffer, 0, 1024);
  SetAutoCommitMode(false);
  sprintf(
    buffer,
    "{ CALL pUpdate_utsellinfo_reregist( %u, %u, '%04d-%02d-%02d %02d:%02d:%02d.%03d', %u ) }",
    byType,
    dwRegistSerial,
    kCurTime->wYear,
    kCurTime->wMonth,
    kCurTime->wDay,
    kCurTime->wHour,
    kCurTime->wMinute,
    kCurTime->wSecond,
    kCurTime->wMilliseconds,
    dwPrice);
  if (ExecUpdateQuery(buffer, true)
      && Update_UnmannedTraderResutlInfo(byType, dwRegistSerial, byState, 0, 0, kCurTime))
  {
    CommitTransaction();
    SetAutoCommitMode(true);
    return 1;
  }

  RollbackTransaction();
  SetAutoCommitMode(true);
  return 0;
}

bool CRFWorldDatabase::Update_UnmannedTraderCheatUpdateRegistDate(unsigned __int8 byType, unsigned int dwRegistSerial)
{
  char buffer[1040]{};
  sprintf(buffer, "{ CALL pUpdate_utcheatregdate( %u, %u ) }", byType, dwRegistSerial);
  return ExecUpdateQuery(buffer, true);
}

unsigned __int8 CRFWorldDatabase::Select_UnmannedTraderItemRecordCntByState(
  unsigned __int8 byType,
  unsigned __int8 byState,
  unsigned int *pdwSerial,
  unsigned __int16 wMaxCnt,
  unsigned __int16 *pwRecordCnt)
{
  char buffer[260]{};
  sprintf(buffer, "select top %u [serial] from tbl_utresultinfo where type=%u and state=%u", wMaxCnt, byType, byState);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), SQL_NTS);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      *pwRecordCnt = 0;
      do
      {
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != SQL_SUCCESS_WITH_INFO)
        {
          break;
        }
        SQLLEN indicator = 0;
        SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, &pdwSerial[*pwRecordCnt], 0, &indicator);
        ++*pwRecordCnt;
      } while (wMaxCnt > static_cast<int>(*pwRecordCnt));

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

    if (ret == SQL_NO_DATA)
    {
      return 2;
    }

    ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 1;
}

char CRFWorldDatabase::Update_UnmannedTraderSingleTypeClearUseCompleteRecord(
  unsigned int dwSerial,
  const _SYSTEMTIME *pkCurTime)
{
  char buffer[1048]{};
  SetAutoCommitMode(false);

  sprintf(buffer, "{ CALL pUpdateClear_utsingleiteminfo(%u) }", dwSerial);
  if (ExecUpdateQuery(buffer, true)
      && (sprintf(
            buffer,
            "{ CALL pUpdateClear_utsellinfo(%u,'%04d-%02d-%02d %02d:%02d:%02d.%03d') }",
            dwSerial,
            pkCurTime->wYear,
            pkCurTime->wMonth,
            pkCurTime->wDay,
            pkCurTime->wHour,
            pkCurTime->wMinute,
            pkCurTime->wSecond,
            pkCurTime->wMilliseconds),
          ExecUpdateQuery(buffer, true))
      && (sprintf(
            buffer,
            "{ CALL pUpdateClear_utresultinfo(%u,'%04d-%02d-%02d %02d:%02d:%02d.%03d') }",
            dwSerial,
            pkCurTime->wYear,
            pkCurTime->wMonth,
            pkCurTime->wDay,
            pkCurTime->wHour,
            pkCurTime->wMinute,
            pkCurTime->wSecond,
            pkCurTime->wMilliseconds),
          ExecUpdateQuery(buffer, true)))
  {
    CommitTransaction();
    SetAutoCommitMode(true);
    return 1;
  }

  RollbackTransaction();
  SetAutoCommitMode(true);
  return 0;
}

unsigned __int8 CRFWorldDatabase::Select_RaceBossAccumulationWinRate(
  unsigned __int8 byRace,
  unsigned int dwBossSerial,
  _raceboss_acc_winrate *windata)
{
  SQLLEN indicator = 0;
  char buffer[260]{};

  sprintf(buffer, "{ CALL pSelect_BossAccWinRate(%d, %d, '%s') }", byRace, dwBossSerial, g_Main.m_wszWorldName);
  windata->dwTotalCnt = 0;
  windata->dwWinCnt = 0;

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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, windata, 0, &indicator);
        if (!ret || ret == SQL_SUCCESS_WITH_INFO)
        {
          ret = SQLGetData(m_hStmtSelect, 2u, SQL_C_ULONG, &windata->dwWinCnt, 0, &indicator);
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
            ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
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
          ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
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
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
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

    ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 1;
}
