#include "pch.h"

#include "CMainThread.h"
#include "CRFWorldDatabase.h"
#include "CMainThread.h"
#include "CPotionMgr.h"
#include "CUserRankingProcess.h"
#include "CGoldenBoxItemMgr.h"
#include "CItemStoreManager.h"
#include "worlddb_golden_box_item.h"
#include "guildroom_info.h"
#include "worlddb_guild_battle_rank_list.h"
#include "worlddb_guild_battle_schedule_list.h"
#include "worlddb_guild_battle_info.h"
#include "worlddb_guild_battle_reserved_schedule_info.h"
#include "worlddb_rankinguild_info.h"
#include "CPostData.h"
#include "WorldServerUtil.h"
#include "unmannedtrader_stade_id_info.h"

#include <cstring>
#include <cstdio>
#include <sql.h>
#include <sqlext.h>

int CRFWorldDatabase::Select_FailBattleCount(unsigned __int8 byRace,
        unsigned int dwSerial,
        unsigned int *dwCount)
{
  SQLLEN indicator = 0;
  SQLRETURN sqlRet = SQL_ERROR;
  char query[260]{};

  sprintf_s(query, sizeof(query), "{ CALL pSelect_FailRaceBattleCount_20070704( %d, %d ) }", dwSerial, byRace);
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }
  if (!this->m_hStmtSelect && !this->ReConnectDataBase())
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return 1;
  }

  sqlRet = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
  if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
  {
    sqlRet = SQLFetch(this->m_hStmtSelect);
    if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
    {
      sqlRet = SQLGetData(this->m_hStmtSelect, 1u, 4, dwCount, 0, &indicator);
      if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
      {
        if (this->m_hStmtSelect)
        {
          SQLCloseCursor(this->m_hStmtSelect);
        }
        if (this->m_bSaveDBLog)
        {
          this->FmtLog("%s Success", query);
        }
        return 0;
      }

      unsigned __int8 result = 0;
      if (sqlRet == SQL_NO_DATA)
      {
        result = 2;
      }
      else
      {
        this->ErrorMsgLog(sqlRet, query, "SQLGetData", this->m_hStmtSelect);
        this->ErrorAction(sqlRet, this->m_hStmtSelect);
        result = 1;
      }
      if (this->m_hStmtSelect)
      {
        SQLCloseCursor(this->m_hStmtSelect);
      }
      // narrowing cast for thunk return parity
      return static_cast<int>(result);
    }

    unsigned __int8 result = 0;
    if (sqlRet == SQL_NO_DATA)
    {
      result = 2;
    }
    else
    {
      this->ErrorMsgLog(sqlRet, query, "SQLFetch", this->m_hStmtSelect);
      this->ErrorAction(sqlRet, this->m_hStmtSelect);
      result = 1;
    }
    if (this->m_hStmtSelect)
    {
      SQLCloseCursor(this->m_hStmtSelect);
    }
    // narrowing cast for thunk return parity
    return static_cast<int>(result);
  }

  if (sqlRet == SQL_NO_DATA)
  {
    return 2;
  }

  this->ErrorMsgLog(sqlRet, query, "SQLExecDirectA", this->m_hStmtSelect);
  this->ErrorAction(sqlRet, this->m_hStmtSelect);
  return 1;
}

int CRFWorldDatabase::Select_LoseBattleCount(unsigned __int8 byRace,
        unsigned int dwSerial,
        unsigned int *dwCount)
{
  SQLLEN indicator = 0;
  SQLRETURN sqlRet = SQL_ERROR;
  char query[260]{};

  sprintf_s(query, sizeof(query), "{ CALL pSelect_LoseRaceBattleCount( %d, %d ) }", byRace, dwSerial);
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }
  if (!this->m_hStmtSelect && !this->ReConnectDataBase())
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return 1;
  }

  sqlRet = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
  if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
  {
    sqlRet = SQLFetch(this->m_hStmtSelect);
    if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
    {
      sqlRet = SQLGetData(this->m_hStmtSelect, 1u, 4, dwCount, 0, &indicator);
      if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
      {
        if (this->m_hStmtSelect)
        {
          SQLCloseCursor(this->m_hStmtSelect);
        }
        if (this->m_bSaveDBLog)
        {
          this->FmtLog("%s Success", query);
        }
        return 0;
      }

      unsigned __int8 result = 0;
      if (sqlRet == SQL_NO_DATA)
      {
        result = 2;
      }
      else
      {
        this->ErrorMsgLog(sqlRet, query, "SQLGetData", this->m_hStmtSelect);
        this->ErrorAction(sqlRet, this->m_hStmtSelect);
        result = 1;
      }
      if (this->m_hStmtSelect)
      {
        SQLCloseCursor(this->m_hStmtSelect);
      }
      // narrowing cast for thunk return parity
      return static_cast<int>(result);
    }

    unsigned __int8 result = 0;
    if (sqlRet == SQL_NO_DATA)
    {
      result = 2;
    }
    else
    {
      this->ErrorMsgLog(sqlRet, query, "SQLFetch", this->m_hStmtSelect);
      this->ErrorAction(sqlRet, this->m_hStmtSelect);
      result = 1;
    }
    if (this->m_hStmtSelect)
    {
      SQLCloseCursor(this->m_hStmtSelect);
    }
    // narrowing cast for thunk return parity
    return static_cast<int>(result);
  }

  if (sqlRet == SQL_NO_DATA)
  {
    return 2;
  }

  this->ErrorMsgLog(sqlRet, query, "SQLExecDirectA", this->m_hStmtSelect);
  this->ErrorAction(sqlRet, this->m_hStmtSelect);
  return 1;
}

int CRFWorldDatabase::Select_OreCutting(unsigned int dwSerial,
        _worlddb_ore_cutting *pOreCutting)
{
  char query[536]{};
  SQLLEN indicator = 0;
  SQLRETURN sqlRet = SQL_ERROR;

  sprintf_s(
    query,
    sizeof(query),
    "select K0,D0,K1,D1,K2,D2,K3,D3,K4,D4,K5,D5,K6,D6,K7,D7,K8,D8,K9,D9,K10,D10,K11,D11,K12,D12,K13,D13,K14,D14,K15,D15,K"
    "16,D16,K17,D17,K18,D18,K19,D19 from tbl_OreCutting where serial=%u",
    dwSerial);
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }
  if (!this->m_hStmtSelect && !this->ReConnectDataBase())
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return 1;
  }

  sqlRet = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
  if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
  {
    sqlRet = SQLFetch(this->m_hStmtSelect);
    if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
    {
      int columnNumber = 0;
      for (int oreIndex = 0; oreIndex < 20; ++oreIndex)
      {
        sqlRet = SQLGetData(this->m_hStmtSelect, ++columnNumber, 4, &pOreCutting->List[oreIndex], 0, &indicator);
        if (sqlRet && sqlRet != SQL_SUCCESS_WITH_INFO)
        {
          this->ErrorMsgLog(sqlRet, query, "SQLGetData", this->m_hStmtSelect);
          this->ErrorAction(sqlRet, this->m_hStmtSelect);
          if (this->m_hStmtSelect)
          {
            SQLCloseCursor(this->m_hStmtSelect);
          }
          return 1;
        }

        sqlRet = SQLGetData(
          this->m_hStmtSelect,
          ++columnNumber,
          4,
          &pOreCutting->List[oreIndex].dwDur,
          0,
          &indicator);
        if (sqlRet && sqlRet != SQL_SUCCESS_WITH_INFO)
        {
          this->ErrorMsgLog(sqlRet, query, "SQLGetData", this->m_hStmtSelect);
          this->ErrorAction(sqlRet, this->m_hStmtSelect);
          if (this->m_hStmtSelect)
          {
            SQLCloseCursor(this->m_hStmtSelect);
          }
          return 1;
        }
      }

      if (this->m_hStmtSelect)
      {
        SQLCloseCursor(this->m_hStmtSelect);
      }
      if (this->m_bSaveDBLog)
      {
        this->FmtLog("%s Success", query);
      }
      return 0;
    }

    unsigned __int8 result = 0;
    if (sqlRet == SQL_NO_DATA)
    {
      result = 2;
    }
    else
    {
      this->ErrorMsgLog(sqlRet, query, "SQLFetch", this->m_hStmtSelect);
      this->ErrorAction(sqlRet, this->m_hStmtSelect);
      result = 1;
    }
    if (this->m_hStmtSelect)
    {
      SQLCloseCursor(this->m_hStmtSelect);
    }
    // narrowing cast for thunk return parity
    return static_cast<int>(result);
  }

  if (sqlRet == SQL_NO_DATA)
  {
    return 2;
  }

  this->ErrorMsgLog(sqlRet, query, "SQLExecDirect", this->m_hStmtSelect);
  this->ErrorAction(sqlRet, this->m_hStmtSelect);
  return 1;
}

int CRFWorldDatabase::Select_PcBangFavorItem(unsigned int dwSerial,
        _worlddb_pcbang_favor_item *pPcBangFavorItem)
{
  char query[1048]{};
  SQLLEN indicator = 0;
  SQLRETURN sqlRet = SQL_ERROR;

  sprintf_s(query, sizeof(query), "{ CALL pSelect_PcbangItem( %d ) }", dwSerial);
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }
  if (!this->m_hStmtSelect && !this->ReConnectDataBase())
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return 1;
  }

  sqlRet = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
  if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
  {
    sqlRet = SQLFetch(this->m_hStmtSelect);
    if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
    {
      int columnNumber = 0;
      for (int itemIndex = 0; itemIndex < 50; ++itemIndex)
      {
        sqlRet = SQLGetData(
          this->m_hStmtSelect,
          ++columnNumber,
          65511,
          &pPcBangFavorItem->lnUID[itemIndex],
          0,
          &indicator);
      }

      if (this->m_hStmtSelect)
      {
        SQLCloseCursor(this->m_hStmtSelect);
      }
      if (this->m_bSaveDBLog)
      {
        this->FmtLog("%s Success", query);
      }
      return 0;
    }

    unsigned __int8 result = 0;
    if (sqlRet == SQL_NO_DATA)
    {
      result = 2;
    }
    else
    {
      this->ErrorMsgLog(sqlRet, query, "SQLFetch", this->m_hStmtSelect);
      this->ErrorAction(sqlRet, this->m_hStmtSelect);
      result = 1;
    }
    if (this->m_hStmtSelect)
    {
      SQLCloseCursor(this->m_hStmtSelect);
    }
    // narrowing cast for thunk return parity
    return static_cast<int>(result);
  }

  if (sqlRet == SQL_NO_DATA)
  {
    return 2;
  }

  this->ErrorMsgLog(sqlRet, query, "SQLExecDirect", this->m_hStmtSelect);
  this->ErrorAction(sqlRet, this->m_hStmtSelect);
  return 1;
}

int CRFWorldDatabase::Select_PotionDelay(unsigned int dwSerial,
        _worlddb_potion_delay_info *pPotionDelayInfo)
{
  char query[1024]{};
  char source[152]{};
  SQLLEN indicator = 0;
  SQLRETURN sqlRet = SQL_ERROR;

  sprintf_s(query, sizeof(query), "select ");
  for (int potionIndex = 0; potionIndex < 38; ++potionIndex)
  {
    if (g_PotionMgr.IsPotionDelayUseIndex(potionIndex))
    {
      sprintf_s(source, 128, "PD%d,", potionIndex);
      strcat_s(query, source);
    }
  }
  query[std::strlen(query) - 1] = ' ';
  sprintf_s(source, 128, "from tbl_potion_delay where serial=%u", dwSerial);
  strcat_s(query, source);
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }
  if (!this->m_hStmtSelect && !this->ReConnectDataBase())
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return 1;
  }

  sqlRet = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
  if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
  {
    sqlRet = SQLFetch(this->m_hStmtSelect);
    if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
    {
      int columnNumber = 1;
      for (int potionIndex = 0; potionIndex < 38; ++potionIndex)
      {
        if (!g_PotionMgr.IsPotionDelayUseIndex(potionIndex))
        {
          continue;
        }

        sqlRet = SQLGetData(
          this->m_hStmtSelect,
          columnNumber,
          4,
          &pPotionDelayInfo->dwPotionDelay[potionIndex],
          0,
          &indicator);
        if (sqlRet && sqlRet != SQL_SUCCESS_WITH_INFO)
        {
          this->ErrorMsgLog(sqlRet, query, "SQLGetData", this->m_hStmtSelect);
          this->ErrorAction(sqlRet, this->m_hStmtSelect);
          if (this->m_hStmtSelect)
          {
            SQLCloseCursor(this->m_hStmtSelect);
          }
          return 1;
        }

        ++columnNumber;
      }

      if (this->m_hStmtSelect)
      {
        SQLCloseCursor(this->m_hStmtSelect);
      }
      if (this->m_bSaveDBLog)
      {
        this->FmtLog("%s Success", query);
      }
      return 0;
    }

    unsigned __int8 result = 0;
    if (sqlRet == SQL_NO_DATA)
    {
      result = 2;
    }
    else
    {
      this->ErrorMsgLog(sqlRet, query, "SQLFetch", this->m_hStmtSelect);
      this->ErrorAction(sqlRet, this->m_hStmtSelect);
      result = 1;
    }
    if (this->m_hStmtSelect)
    {
      SQLCloseCursor(this->m_hStmtSelect);
    }
    // narrowing cast for thunk return parity
    return static_cast<int>(result);
  }

  if (sqlRet == SQL_NO_DATA)
  {
    return 2;
  }

  this->ErrorMsgLog(sqlRet, query, "SQLExecDirect", this->m_hStmtSelect);
  this->ErrorAction(sqlRet, this->m_hStmtSelect);
  return 1;
}

int CRFWorldDatabase::Select_PrimiumPlayTime(unsigned int dwAccSerial,
        _PCBANG_PLAY_TIME *kInfo)
{
  char query[280]{};
  SQLLEN indicator = 0;
  SQLRETURN sqlRet = SQL_ERROR;

  sprintf_s(query, "{ CALL pSelect_PrimiumPlayTime( %u ) }", dwAccSerial);
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }
  if (!this->m_hStmtSelect && !this->ReConnectDataBase())
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return 1;
  }

  sqlRet = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
  if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
  {
    sqlRet = SQLFetch(this->m_hStmtSelect);
    if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
    {
      sqlRet = SQLGetData(this->m_hStmtSelect, 1u, 4, &kInfo->dwLastConnTime, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 2u, 4, &kInfo->dwContPlayTime, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 3u, 65529, &kInfo->bForcedClose, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 4u, 65530, &kInfo->byReceiveCoupon, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 5u, 65530, &kInfo->byEnsureTime, 0, &indicator);
      if (this->m_hStmtSelect)
      {
        SQLCloseCursor(this->m_hStmtSelect);
      }
      if (this->m_bSaveDBLog)
      {
        this->FmtLog("%s Success", query);
      }
      return 0;
    }

    unsigned __int8 result = 0;
    if (sqlRet == SQL_NO_DATA)
    {
      result = 2;
    }
    else
    {
      this->ErrorMsgLog(sqlRet, query, "SQLFetch", this->m_hStmtSelect);
      this->ErrorAction(sqlRet, this->m_hStmtSelect);
      result = 1;
    }
    if (this->m_hStmtSelect)
    {
      SQLCloseCursor(this->m_hStmtSelect);
    }
    // narrowing cast for thunk return parity
    return static_cast<int>(result);
  }

  if (sqlRet == SQL_NO_DATA)
  {
    return 2;
  }

  this->ErrorMsgLog(sqlRet, query, "SQLExecDirect", this->m_hStmtSelect);
  this->ErrorAction(sqlRet, this->m_hStmtSelect);
  return 1;
}

int CRFWorldDatabase::Select_PvpOrderViewInfo(unsigned int dwSerial,
        _pvporderview_info *kInfo)
{
  char query[280]{};
  SQLLEN indicator = 0;
  SQLRETURN sqlRet = SQL_ERROR;
  unsigned __int16 dateFields[24]{};
  tm updatedTm{};

  sprintf_s(query, "{ CALL pSelect_PvpOrderView( %u ) }", dwSerial);
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }
  if (!this->m_hStmtSelect && !this->ReConnectDataBase())
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return 1;
  }

  sqlRet = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
  if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
  {
    sqlRet = SQLFetch(this->m_hStmtSelect);
    if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
    {
      sqlRet = SQLGetData(this->m_hStmtSelect, 1u, 93, dateFields, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 2u, 4, &kInfo->nDeath, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 3u, 4, &kInfo->nKill, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 4u, 8, &kInfo->dTodayStacked, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 5u, 8, &kInfo->dPvpPoint, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 6u, 8, &kInfo->dPvpTempCash, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 7u, 8, &kInfo->dPvpCash, 0, &indicator);
      for (int killerIndex = 0; killerIndex < 10; ++killerIndex)
      {
        sqlRet = SQLGetData(this->m_hStmtSelect, killerIndex + 8, 4, &kInfo->dwKillerSerial[killerIndex], 0, &indicator);
      }
      sqlRet = SQLGetData(this->m_hStmtSelect, 18, 65530, &kInfo->byContHaveCash, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 19, 65530, &kInfo->byContLoseCash, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 20, 65529, &kInfo->bRaceWarRecvr, 0, &indicator);

      updatedTm.tm_year = dateFields[0] - 1900;
      updatedTm.tm_mon = dateFields[1] - 1;
      updatedTm.tm_mday = dateFields[2];
      updatedTm.tm_hour = dateFields[3];
      updatedTm.tm_min = dateFields[4];
      updatedTm.tm_sec = dateFields[5];
      updatedTm.tm_isdst = -1;

      __time64_t updatedTime = mktime_3(&updatedTm);
      if (updatedTime == -1)
      {
        updatedTime = 0;
      }
      kInfo->tUpdatedate = updatedTime;

      if (this->m_hStmtSelect)
      {
        SQLCloseCursor(this->m_hStmtSelect);
      }
      if (this->m_bSaveDBLog)
      {
        this->FmtLog("%s Success", query);
      }
      return 0;
    }

    unsigned __int8 result = 0;
    if (sqlRet == SQL_NO_DATA)
    {
      result = 2;
    }
    else
    {
      this->ErrorMsgLog(sqlRet, query, "SQLFetch", this->m_hStmtSelect);
      this->ErrorAction(sqlRet, this->m_hStmtSelect);
      result = 1;
    }
    if (this->m_hStmtSelect)
    {
      SQLCloseCursor(this->m_hStmtSelect);
    }
    // narrowing cast for thunk return parity
    return static_cast<int>(result);
  }

  if (sqlRet == SQL_NO_DATA)
  {
    return 2;
  }

  this->ErrorMsgLog(sqlRet, query, "SQLExecDirect", this->m_hStmtSelect);
  this->ErrorAction(sqlRet, this->m_hStmtSelect);
  return 1;
}

int CRFWorldDatabase::Select_WinBattleCount(unsigned __int8 byRace,
        unsigned int dwSerial,
        unsigned int *dwCount)
{
  SQLLEN indicator = 0;
  SQLRETURN sqlRet = SQL_ERROR;
  char query[260]{};

  sprintf_s(query, sizeof(query), "{ CALL pSelect_WinRaceBattleCount( %d, %d ) }", byRace, dwSerial);
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }
  if (!this->m_hStmtSelect && !this->ReConnectDataBase())
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return 1;
  }

  sqlRet = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
  if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
  {
    sqlRet = SQLFetch(this->m_hStmtSelect);
    if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
    {
      sqlRet = SQLGetData(this->m_hStmtSelect, 1u, 4, dwCount, 0, &indicator);
      if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
      {
        if (this->m_hStmtSelect)
        {
          SQLCloseCursor(this->m_hStmtSelect);
        }
        if (this->m_bSaveDBLog)
        {
          this->FmtLog("%s Success", query);
        }
        return 0;
      }

      unsigned __int8 result = 0;
      if (sqlRet == SQL_NO_DATA)
      {
        result = 2;
      }
      else
      {
        this->ErrorMsgLog(sqlRet, query, "SQLGetData", this->m_hStmtSelect);
        this->ErrorAction(sqlRet, this->m_hStmtSelect);
        result = 1;
      }
      if (this->m_hStmtSelect)
      {
        SQLCloseCursor(this->m_hStmtSelect);
      }
      // narrowing cast for thunk return parity
      return static_cast<int>(result);
    }

    unsigned __int8 result = 0;
    if (sqlRet == SQL_NO_DATA)
    {
      result = 2;
    }
    else
    {
      this->ErrorMsgLog(sqlRet, query, "SQLFetch", this->m_hStmtSelect);
      this->ErrorAction(sqlRet, this->m_hStmtSelect);
      result = 1;
    }
    if (this->m_hStmtSelect)
    {
      SQLCloseCursor(this->m_hStmtSelect);
    }
    // narrowing cast for thunk return parity
    return static_cast<int>(result);
  }

  if (sqlRet == SQL_NO_DATA)
  {
    return 2;
  }

  this->ErrorMsgLog(sqlRet, query, "SQLExecDirectA", this->m_hStmtSelect);
  this->ErrorAction(sqlRet, this->m_hStmtSelect);
  return 1;
}

bool CRFWorldDatabase::Insert_ItemCombineEx(unsigned int dwCharacterSerial)
{
  char Buffer[272]; // [rsp+30h] [rbp-128h] BYREF

  sprintf_s(Buffer, "{ CALL pInsert_CombineEx_Result( %d ) }", dwCharacterSerial);
  return this->ExecUpdateQuery(Buffer, 1);
}

bool CRFWorldDatabase::Insert_NpcQuest_History(unsigned int dwSerial)
{
  char Buffer[272]; // [rsp+30h] [rbp-128h] BYREF

  sprintf_s(Buffer, "{ CALL pInsert_NpcQuest_History( %d ) }", dwSerial);
  return this->ExecUpdateQuery(Buffer, 1);
}

bool CRFWorldDatabase::Insert_OreCutting(unsigned int dwSerial)
{
  char Buffer[80]; // [rsp+30h] [rbp-68h] BYREF

  memset(Buffer, 0, 64);
  sprintf_s(Buffer, 64, "{ CALL pInsert_OreCutting( %d ) }", dwSerial);
  return this->ExecUpdateQuery(Buffer, 1);
}

bool CRFWorldDatabase::Insert_PcBangFavorItem(unsigned int dwSerial)
{
  char Buffer[1040]; // [rsp+30h] [rbp-428h] BYREF

  memset(Buffer, 0, 1024);
  sprintf_s(Buffer, 1024, "{ CALL pInsert_PcbangItem( %d ) }", dwSerial);
  return this->ExecUpdateQuery(Buffer, 1);
}

bool CRFWorldDatabase::Insert_PotionDelay(unsigned int dwSerial)
{
  char Buffer[80]; // [rsp+30h] [rbp-68h] BYREF

  memset(Buffer, 0, 64);
  sprintf_s(Buffer, 64, "{ CALL pInsert_potiondelay( %d ) }", dwSerial);
  return this->ExecUpdateQuery(Buffer, 1);
}

bool CRFWorldDatabase::Insert_PrimiumPlayTime(unsigned int dwAccSerial)
{
  char Buffer[272]; // [rsp+30h] [rbp-128h] BYREF

  sprintf_s(Buffer, "{ CALL pInsert_PrimiumPlayTime( %d ) }", dwAccSerial);
  return this->ExecUpdateQuery(Buffer, 1);
}

bool CRFWorldDatabase::Insert_PvpOrderViewInfo(unsigned int dwSerial)
{
  char Buffer[272]; // [rsp+30h] [rbp-128h] BYREF

  sprintf_s(Buffer, "{ CALL pInsert_PvpOrderView( %d ) }", dwSerial);
  return this->ExecUpdateQuery(Buffer, 1);
}

bool CRFWorldDatabase::Insert_PvpPointLimitInfoRecord(unsigned int dwSerial)
{
  char Buffer[272]; // [rsp+30h] [rbp-128h] BYREF

  sprintf_s(Buffer, "{ CALL pInsert_pplirecord( %d ) }", dwSerial);
  return this->ExecUpdateQuery(Buffer, 1);
}

unsigned __int8 CRFWorldDatabase::Select_ItemCombineEx(unsigned int dwSerial,
        _worlddb_itemcombineex_info *pdbItemCombineExInfo)
{
  SQLLEN indicator = 0;
  SQLRETURN sqlRet = SQL_ERROR;
  char query[260]{};

  sprintf_s(query, "{ CALL pSelect_CombineEx_Result_20071010( %d ) }", dwSerial);
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }
  if (!this->m_hStmtSelect && !this->ReConnectDataBase())
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return 1;
  }

  sqlRet = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
  if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
  {
    sqlRet = SQLFetch(this->m_hStmtSelect);
    if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
    {
      int columnNumber = 0;
      sqlRet = SQLGetData(this->m_hStmtSelect, ++columnNumber, 65530, pdbItemCombineExInfo, 0, &indicator);
      if (pdbItemCombineExInfo->bIsResult)
      {
        sqlRet = SQLGetData(this->m_hStmtSelect, ++columnNumber, 65518, &pdbItemCombineExInfo->dwCheckKey, 0, &indicator);
        sqlRet = SQLGetData(this->m_hStmtSelect, ++columnNumber, 65530, &pdbItemCombineExInfo->byDlgType, 0, &indicator);
        sqlRet = SQLGetData(this->m_hStmtSelect, ++columnNumber, 65518, &pdbItemCombineExInfo->dwDalant, 0, &indicator);
        sqlRet = SQLGetData(
          this->m_hStmtSelect,
          ++columnNumber,
          65530,
          &pdbItemCombineExInfo->byItemListCount,
          0,
          &indicator);
        sqlRet = SQLGetData(
          this->m_hStmtSelect,
          ++columnNumber,
          65530,
          &pdbItemCombineExInfo->byItemSelectCount,
          0,
          &indicator);
        for (int itemIndex = 0; itemIndex < 24; ++itemIndex)
        {
          sqlRet = SQLGetData(this->m_hStmtSelect, ++columnNumber, 4, &pdbItemCombineExInfo->invenKey[itemIndex], 0, &indicator);
          if (sqlRet && sqlRet != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
          sqlRet = SQLGetData(
            this->m_hStmtSelect,
            ++columnNumber,
            4,
            &pdbItemCombineExInfo->invenKey[itemIndex].dwD,
            0,
            &indicator);
          sqlRet = SQLGetData(
            this->m_hStmtSelect,
            ++columnNumber,
            4,
            &pdbItemCombineExInfo->invenKey[itemIndex].dwU,
            0,
            &indicator);
        }
        sqlRet = SQLGetData(
          this->m_hStmtSelect,
          ++columnNumber,
          4,
          &pdbItemCombineExInfo->dwResultEffectType,
          0,
          &indicator);
        sqlRet = SQLGetData(
          this->m_hStmtSelect,
          ++columnNumber,
          4,
          &pdbItemCombineExInfo->dwResultEffectMsgCode,
          0,
          &indicator);
      }

      if (this->m_hStmtSelect)
      {
        SQLCloseCursor(this->m_hStmtSelect);
      }
      if (this->m_bSaveDBLog)
      {
        this->FmtLog("%s Success", query);
      }
      return 0;
    }

    unsigned __int8 result = 0;
    if (sqlRet == SQL_NO_DATA)
    {
      result = 2;
    }
    else
    {
      this->ErrorMsgLog(sqlRet, query, "SQLFetch", this->m_hStmtSelect);
      this->ErrorAction(sqlRet, this->m_hStmtSelect);
      result = 1;
    }
    if (this->m_hStmtSelect)
    {
      SQLCloseCursor(this->m_hStmtSelect);
    }
    return result;
  }

  if (sqlRet == SQL_NO_DATA)
  {
    return 2;
  }

  this->ErrorMsgLog(sqlRet, query, "SQLExecDirectA", this->m_hStmtSelect);
  this->ErrorAction(sqlRet, this->m_hStmtSelect);
  return 1;
}

unsigned __int8 CRFWorldDatabase::Select_NpcQuest_History(unsigned int dwSerial,
        _worlddb_npc_quest_complete_history *pNpcQHis)
{
  SQLLEN indicator = 0;
  SQLRETURN sqlRet = SQL_ERROR;
  char query[260]{};

  sprintf_s(query, "{ CALL pSelect_NpcQuest_History_20080811( %d ) }", dwSerial);
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }
  if (!this->m_hStmtSelect && !this->ReConnectDataBase())
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return 1;
  }

  sqlRet = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
  if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
  {
    sqlRet = SQLFetch(this->m_hStmtSelect);
    if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
    {
      int columnNumber = 0;
      for (int historyIndex = 0; historyIndex < 70; ++historyIndex)
      {
        sqlRet = SQLGetData(this->m_hStmtSelect, ++columnNumber, 1, &pNpcQHis->List[historyIndex], 8, &indicator);
        sqlRet = SQLGetData(
          this->m_hStmtSelect,
          ++columnNumber,
          65530,
          &pNpcQHis->List[historyIndex].byLevel,
          0,
          &indicator);
        sqlRet = SQLGetData(
          this->m_hStmtSelect,
          ++columnNumber,
          4,
          &pNpcQHis->List[historyIndex].dwEventEndTime,
          0,
          &indicator);
        if (sqlRet && sqlRet != SQL_SUCCESS_WITH_INFO)
        {
          break;
        }
      }

      if (this->m_hStmtSelect)
      {
        SQLCloseCursor(this->m_hStmtSelect);
      }
      if (this->m_bSaveDBLog)
      {
        this->FmtLog("%s Success", query);
      }
      return 0;
    }

    unsigned __int8 result = 0;
    if (sqlRet == SQL_NO_DATA)
    {
      result = 2;
    }
    else
    {
      this->ErrorMsgLog(sqlRet, query, "SQLFetch", this->m_hStmtSelect);
      this->ErrorAction(sqlRet, this->m_hStmtSelect);
      result = 1;
    }
    if (this->m_hStmtSelect)
    {
      SQLCloseCursor(this->m_hStmtSelect);
    }
    return result;
  }

  if (sqlRet == SQL_NO_DATA)
  {
    return 2;
  }

  this->ErrorMsgLog(sqlRet, query, "SQLExecDirectA", this->m_hStmtSelect);
  this->ErrorAction(sqlRet, this->m_hStmtSelect);
  return 1;
}

unsigned __int8 CRFWorldDatabase::Select_PvpPointLimitInfo(unsigned int dwSerial,
        _pvppointlimit_info *kInfo)
{
  char query[280]{};
  SQLLEN indicator = 0;
  SQLRETURN sqlRet = SQL_ERROR;
  unsigned __int16 dateFields[24]{};
  tm updatedTm{};

  sprintf_s(query, "{ CALL pSelect_ppliinfo( %u ) }", dwSerial);
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }
  if (!this->m_hStmtSelect && !this->ReConnectDataBase())
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return 1;
  }

  sqlRet = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
  if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
  {
    sqlRet = SQLFetch(this->m_hStmtSelect);
    if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
    {
      sqlRet = SQLGetData(this->m_hStmtSelect, 1u, 93, dateFields, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 2u, 65529, &kInfo->bUseUp, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 3u, 65530, &kInfo->byLimitRate, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 4u, 8, &kInfo->dOriginalPoint, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 5u, 8, &kInfo->dLimitPoint, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 6u, 8, &kInfo->dUsePoint, 0, &indicator);

      updatedTm.tm_year = dateFields[0] - 1900;
      updatedTm.tm_mon = dateFields[1] - 1;
      updatedTm.tm_mday = dateFields[2];
      updatedTm.tm_hour = dateFields[3];
      updatedTm.tm_min = dateFields[4];
      updatedTm.tm_sec = dateFields[5];
      updatedTm.tm_isdst = -1;

      __time64_t updatedTime = mktime_3(&updatedTm);
      if (updatedTime == -1)
      {
        updatedTime = 0;
      }
      kInfo->tUpdatedate = updatedTime;

      if (this->m_hStmtSelect)
      {
        SQLCloseCursor(this->m_hStmtSelect);
      }
      if (this->m_bSaveDBLog)
      {
        this->FmtLog("%s Success", query);
      }
      return 0;
    }

    unsigned __int8 result = 0;
    if (sqlRet == SQL_NO_DATA)
    {
      result = 2;
    }
    else
    {
      this->ErrorMsgLog(sqlRet, query, "SQLFetch", this->m_hStmtSelect);
      this->ErrorAction(sqlRet, this->m_hStmtSelect);
      result = 1;
    }
    if (this->m_hStmtSelect)
    {
      SQLCloseCursor(this->m_hStmtSelect);
    }
    return result;
  }

  if (sqlRet == SQL_NO_DATA)
  {
    return 2;
  }

  this->ErrorMsgLog(sqlRet, query, "SQLExecDirect", this->m_hStmtSelect);
  this->ErrorAction(sqlRet, this->m_hStmtSelect);
  return 1;
}

bool CRFWorldDatabase::Update_RankInGuild(unsigned int dwGuildSerial, _worlddb_rankinguild_info *pGuildMemberRankData)
{
  SQLRETURN ret = 0;
  SQLLEN indicator[5]{};
  char buffer[544]{};
  char source[132]{};
  int gradeValue = 2;
  unsigned int gradeRows[157]{};
  int levelValue = 0;
  double pvpPoint = 0.0;

  if (m_bSaveDBLog)
  {
    Log("Update_RankInGuild Start");
  }

  if (!(m_hStmtUpdate || ReConnectDataBase()))
  {
    ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
    return false;
  }

  sprintf_s(buffer, "select top %u IDENTITY(int, 1, 1) AS Rank, -1 as Rate, -1 as NewGrade, ", 50);
  std::strcat(buffer, "b.serial, b.lv, g.Pvppoint, g.GuildGrade as CurGrade into #tbl_RankInGuild ");
  std::strcat(buffer, "from tbl_general as g join tbl_base as b on g.serial = b.serial ");
  sprintf_s(source, "where g.guildserial=%d and b.dck=0 order by g.Pvppoint desc", dwGuildSerial);
  std::strcat(buffer, source);
  ret = SQLExecDirectA(m_hStmtUpdate, reinterpret_cast<SQLCHAR *>(buffer), SQL_NTS);
  if (ret && ret != SQL_SUCCESS_WITH_INFO)
  {
    ExecUpdateQuery("drop table #tbl_RankInGuild", false);
    if (ret != SQL_NO_DATA)
    {
      ErrorMsgLog(ret, buffer, "SQLExecDirect", m_hStmtUpdate);
    }
    return false;
  }

  ExecUpdateQuery("update #tbl_RankInGuild set Rate = ( (Rank*10000)/(select count(*) from #tbl_RankInGuild) )", false);
  ExecUpdateQuery("update #tbl_RankInGuild set NewGrade=0", false);
  ExecUpdateQuery("update #tbl_RankInGuild set NewGrade=3 where lv >= 30 and lv <= 34 and rate <= 6500", false);
  ExecUpdateQuery(
    "update #tbl_RankInGuild set NewGrade=2 where lv >= 30 and lv <= 34 and rate > 6500 and rate <= 8500",
    false);
  ExecUpdateQuery(
    "update #tbl_RankInGuild set NewGrade=1 where lv >= 30 and lv <= 34 and rate > 8500 and rate <= 9500",
    false);
  ExecUpdateQuery("update #tbl_RankInGuild set NewGrade=4 where lv >= 35 and lv <= 39 and rate <= 3500", false);
  ExecUpdateQuery(
    "update #tbl_RankInGuild set NewGrade=3 where lv >= 35 and lv <= 39 and rate > 3500 and rate <= 6500",
    false);
  ExecUpdateQuery(
    "update #tbl_RankInGuild set NewGrade=2 where lv >= 35 and lv <= 39 and rate > 6500 and rate <= 8500",
    false);
  ExecUpdateQuery(
    "update #tbl_RankInGuild set NewGrade=1 where lv >= 35 and lv <= 39 and rate > 8500 and rate <= 9500",
    false);
  ExecUpdateQuery("update #tbl_RankInGuild set NewGrade=5 where lv >= 40 and lv <= 44 and rate <= 1500", false);
  ExecUpdateQuery(
    "update #tbl_RankInGuild set NewGrade=4 where lv >= 40 and lv <= 44 and rate > 1500 and rate <= 3500",
    false);
  ExecUpdateQuery(
    "update #tbl_RankInGuild set NewGrade=3 where lv >= 40 and lv <= 44 and rate > 3500 and rate <= 6500",
    false);
  ExecUpdateQuery(
    "update #tbl_RankInGuild set NewGrade=2 where lv >= 40 and lv <= 44 and rate > 6500 and rate <= 8500",
    false);
  ExecUpdateQuery(
    "update #tbl_RankInGuild set NewGrade=1 where lv >= 40 and lv <= 44 and rate > 8500 and rate <= 9500",
    false);
  ExecUpdateQuery("update #tbl_RankInGuild set NewGrade=6 where lv >= 45 and lv <= 49 and rate <= 500", false);
  ExecUpdateQuery(
    "update #tbl_RankInGuild set NewGrade=5 where lv >= 45 and lv <= 49 and rate > 500 and rate <= 1500",
    false);
  ExecUpdateQuery(
    "update #tbl_RankInGuild set NewGrade=4 where lv >= 45 and lv <= 49 and rate > 1500 and rate <= 3500",
    false);
  ExecUpdateQuery(
    "update #tbl_RankInGuild set NewGrade=3 where lv >= 45 and lv <= 49 and rate > 3500 and rate <= 6500",
    false);
  ExecUpdateQuery(
    "update #tbl_RankInGuild set NewGrade=2 where lv >= 45 and lv <= 49 and rate > 6500 and rate <= 8500",
    false);
  ExecUpdateQuery(
    "update #tbl_RankInGuild set NewGrade=1 where lv >= 45 and lv <= 49 and rate > 8500 and rate <= 9500",
    false);
  ExecUpdateQuery("update #tbl_RankInGuild set NewGrade=7 where lv >= 50 and rate <= 100", false);
  ExecUpdateQuery(
    "update #tbl_RankInGuild set NewGrade=6 where lv >= 50 and rate > 100 and rate <= 500",
    false);
  ExecUpdateQuery(
    "update #tbl_RankInGuild set NewGrade=5 where lv >= 50 and rate > 500 and rate <= 1500",
    false);
  ExecUpdateQuery(
    "update #tbl_RankInGuild set NewGrade=4 where lv >= 50 and rate > 1500 and rate <= 3500",
    false);
  ExecUpdateQuery(
    "update #tbl_RankInGuild set NewGrade=3 where lv >= 50 and rate > 3500 and rate <= 6500",
    false);
  ExecUpdateQuery(
    "update #tbl_RankInGuild set NewGrade=2 where lv >= 50 and rate > 6500 and rate <= 8500",
    false);
  ExecUpdateQuery(
    "update #tbl_RankInGuild set NewGrade=1 where lv >= 50 and rate > 8500 and rate <= 9500",
    false);
  ExecUpdateQuery(
    "select IDENTITY(int, 1, 1) AS NewRank, -1 as Grade, serial, CurGrade, lv, Pvppoint into #tbl_RankInGuildAll from #tbl_RankInGuild order by NewGrade desc, rate ",
    false);
  ExecUpdateQuery(
    "select IDENTITY(int, 1, 1) AS NewRank, -1 as NewRate, -1 as Grade, serial, CurGrade, lv, PvpPoint into #tbl_RankInGuildCom from #tbl_RankInGuild where CurGrade <> 0 and CurGrade <> 3 order by NewGrade desc, rate ",
    false);
  ExecUpdateQuery(
    "update #tbl_RankInGuildCom set NewRate = ( (NewRank*10000)/(select count(*) from #tbl_RankInGuildCom) ) ",
    false);
  ExecUpdateQuery("update #tbl_RankInGuildCom set Grade = 2", false);
  ExecUpdateQuery("update #tbl_RankInGuildCom set Grade = 1 where NewRate <= 1000", false);
  ExecUpdateQuery(
    "update tbl_general set GuildGrade = Grade from ( select serial, Grade from #tbl_RankInGuildCom ) as rank where tbl_general.serial = rank.serial",
    false);
  ExecUpdateQuery(
    "update tbl_general set GuildRank = rank.NewRank\tfrom ( select serial, NewRank from #tbl_RankInGuildAll ) as rank where tbl_general.serial = rank.serial",
    false);

  std::memset(gradeRows, 0, 600);
  sprintf_s(buffer, "select serial, Grade from #tbl_RankInGuildCom order by Grade");
  if (!(m_hStmtSelect || ReConnectDataBase()))
  {
    ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
    return false;
  }

  ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), SQL_NTS);
  if (ret && ret != SQL_SUCCESS_WITH_INFO)
  {
    ExecUpdateQuery("drop table #tbl_RankInGuild", false);
    ExecUpdateQuery("drop table #tbl_RankInGuildAll", false);
    ExecUpdateQuery("drop table #tbl_RankInGuildCom", false);
    if (ret != SQL_NO_DATA)
    {
      ErrorMsgLog(ret, buffer, "SQLExecDirect", m_hStmtSelect);
      ErrorAction(ret, m_hStmtSelect);
    }
    return false;
  }

  int row = 0;
  while (true)
  {
    ret = SQLFetch(m_hStmtSelect);
    if (ret && ret != SQL_SUCCESS_WITH_INFO)
    {
      break;
    }

    ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_LONG, &gradeRows[3 * row], 0, indicator);
    ret = SQLGetData(m_hStmtSelect, 2u, SQL_C_LONG, &gradeValue, 0, indicator);
    gradeRows[3 * row + 1] = static_cast<unsigned int>(gradeValue & 0xFF);
    ++row;
  }

  if (m_hStmtSelect)
  {
    SQLCloseCursor(m_hStmtSelect);
  }

  sprintf_s(buffer, "select serial, lv, Pvppoint, CurGrade from #tbl_RankInGuildAll order by NewRank");
  if (!(m_hStmtSelect || ReConnectDataBase()))
  {
    ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
    return false;
  }

  ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), SQL_NTS);
  if (ret && ret != SQL_SUCCESS_WITH_INFO)
  {
    ExecUpdateQuery("drop table #tbl_RankInGuild", false);
    ExecUpdateQuery("drop table #tbl_RankInGuildAll", false);
    ExecUpdateQuery("drop table #tbl_RankInGuildCom", false);
    if (ret != SQL_NO_DATA)
    {
      ErrorMsgLog(ret, buffer, "SQLExecDirect", m_hStmtSelect);
      ErrorAction(ret, m_hStmtSelect);
    }
    return false;
  }

  row = 0;
  levelValue = 0;
  gradeValue = 0;
  pvpPoint = 0.0;
  while (true)
  {
    ret = SQLFetch(m_hStmtSelect);
    if (ret && ret != SQL_SUCCESS_WITH_INFO)
    {
      break;
    }

    ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_LONG, &pGuildMemberRankData->MemberData[row].dwSerial, 0, indicator);
    ret = SQLGetData(m_hStmtSelect, 2u, SQL_C_LONG, &levelValue, 0, indicator);
    ret = SQLGetData(m_hStmtSelect, 3u, SQL_C_DOUBLE, &pvpPoint, 0, indicator);
    ret = SQLGetData(m_hStmtSelect, 4u, SQL_C_LONG, &gradeValue, 0, indicator);
    pGuildMemberRankData->MemberData[row].byLv = static_cast<unsigned __int8>(levelValue);
    pGuildMemberRankData->MemberData[row].dwPvpPoint = static_cast<int>(pvpPoint);
    if (gradeValue == 2)
    {
      pGuildMemberRankData->MemberData[row].byGrade = 2;
    }
    else
    {
      pGuildMemberRankData->MemberData[row].byGrade = 0;
    }

    for (int gradeIndex = 0; gradeIndex < 50; ++gradeIndex)
    {
      if (pGuildMemberRankData->MemberData[row].dwSerial == gradeRows[3 * gradeIndex])
      {
        pGuildMemberRankData->MemberData[row].byGrade = static_cast<unsigned __int8>(gradeRows[3 * gradeIndex + 1]);
        break;
      }
    }

    ++row;
  }

  pGuildMemberRankData->wRecordCount = static_cast<unsigned __int16>(row);
  if (m_hStmtSelect)
  {
    SQLCloseCursor(m_hStmtSelect);
  }

  ExecUpdateQuery("drop table #tbl_RankInGuild", false);
  ExecUpdateQuery("drop table #tbl_RankInGuildAll", false);
  ExecUpdateQuery("drop table #tbl_RankInGuildCom", false);
  return true;
}

unsigned __int8 CRFWorldDatabase::Update_RankInGuild_Step1(unsigned int dwGuildSerial)
{
  SQLRETURN sqlRet = SQL_ERROR;
  char query[1040]{};

  this->FmtLog(
    "CRFWorldDatabase::Update_RankInGuild_Step1( DWORD dwGuildSerial(%u) ) Start",
    dwGuildSerial);
  if (this->m_hStmtUpdate || this->ReConnectDataBase())
  {
    sprintf_s(
      query,
      "select top %u IDENTITY(int, 1, 1) AS Rank, -1 as Rate, -1 as NewGrade, b.serial, b.lv, g.Pvppoint, g.GuildGrade as"
      " CurGrade into #tbl_RankInGuild from tbl_general as g join tbl_base as b on g.serial = b.serial where g.guildseria"
      "l=%d and b.dck=0 order by g.Pvppoint desc",
      50,
      dwGuildSerial);
    sqlRet = SQLExecDirectA(this->m_hStmtUpdate, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
    if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_RankInGuild_Step1( DWORD dwGuildSerial(%u) ) End",
        dwGuildSerial);
      return 0;
    }
    if (sqlRet == SQL_NO_DATA)
    {
      this->ExecUpdateQuery("drop table #tbl_RankInGuild", 0);
      this->FmtLog(
        "CRFWorldDatabase::Update_RankInGuild_Step1( DWORD dwGuildSerial(%u) ) No Data!",
        dwGuildSerial);
      return 2;
    }

    this->FmtLog(
      "CRFWorldDatabase::Update_RankInGuild_Step1( DWORD dwGuildSerial(%u) ) Sql Error!",
      dwGuildSerial);
    return 1;
  }

  this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
  return 1;
}

bool CRFWorldDatabase::Update_RankInGuild_Step7()
{

  this->FmtLog("CRFWorldDatabase::Update_RankInGuild_Step7() : Start drop #tbl_RankInGuild Table");
  if ( this->ExecUpdateQuery("drop table #tbl_RankInGuild", 0) )
    this->FmtLog("CRFWorldDatabase::Update_RankInGuild_Step7() : End drop #tbl_RankInGuild Table");
  else
    this->FmtLog("CRFWorldDatabase::Update_RankInGuild_Step7() : drop table #tbl_RankInGuild Fail!");
  return true;
}

bool CRFWorldDatabase::Update_RankInGuild_Step8()
{

  this->FmtLog("CRFWorldDatabase::Update_RankInGuild_Step8() : Start drop #tbl_RankInGuildAll Table");
  if ( this->ExecUpdateQuery("drop table #tbl_RankInGuildAll", 0) )
    this->FmtLog("CRFWorldDatabase::Update_RankInGuild_Step8() : End drop #tbl_RankInGuildAll Table");
  else
    this->FmtLog("CRFWorldDatabase::Update_RankInGuild_Step8() : drop table #tbl_RankInGuildAll Fail!");
  return true;
}

bool CRFWorldDatabase::Update_RankInGuild_Step9()
{

  this->FmtLog("CRFWorldDatabase::Update_RankInGuild_Step9() : Start drop #tbl_RankInGuildCom Table");
  if ( this->ExecUpdateQuery("drop table #tbl_RankInGuildCom", 0) )
    this->FmtLog("CRFWorldDatabase::Update_RankInGuild_Step9() : End drop #tbl_RankInGuildCom Table");
  else
    this->FmtLog("CRFWorldDatabase::Update_RankInGuild_Step9() : drop table #tbl_RankInGuildCom Fail!");
  return true;
}

int CRFWorldDatabase::Select_CashLimSale(_worlddb_cash_limited_sale *pcashlimitedsale)
{
  char query[1024]{};
  SQLLEN indicator = 0;
  SQLRETURN sqlRet = SQL_ERROR;

  sprintf_s(query, sizeof(query), "{ CALL pSelect_Cash_LimSale }");
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }
  if (!this->m_hStmtSelect && !this->ReConnectDataBase())
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return 1;
  }

  sqlRet = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
  if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
  {
    sqlRet = SQLFetch(this->m_hStmtSelect);
    if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
    {
      sqlRet = SQLGetData(this->m_hStmtSelect, 1u, 65530, pcashlimitedsale, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 2u, 65530, &pcashlimitedsale->byLimited_sale_num, 0, &indicator);
      int columnNumber = 2;
      for (int saleIndex = 0; saleIndex < pcashlimitedsale->byLimited_sale_num; ++saleIndex)
      {
        sqlRet = SQLGetData(this->m_hStmtSelect, ++columnNumber, 4, &pcashlimitedsale->List[saleIndex], 0, &indicator);
        sqlRet = SQLGetData(
          this->m_hStmtSelect,
          ++columnNumber,
          4,
          &pcashlimitedsale->List[saleIndex].nLimcount,
          0,
          &indicator);
      }

      if (this->m_hStmtSelect)
      {
        SQLCloseCursor(this->m_hStmtSelect);
      }
      if (this->m_bSaveDBLog)
      {
        this->FmtLog("%s Success", query);
      }
      return 0;
    }

    unsigned __int8 result = 0;
    if (sqlRet == SQL_NO_DATA)
    {
      result = 2;
    }
    else
    {
      this->ErrorMsgLog(sqlRet, query, "SQLFetch", this->m_hStmtSelect);
      this->ErrorAction(sqlRet, this->m_hStmtSelect);
      result = 1;
    }
    if (this->m_hStmtSelect)
    {
      SQLCloseCursor(this->m_hStmtSelect);
    }
    // narrowing cast for thunk return parity
    return static_cast<int>(result);
  }

  if (sqlRet == SQL_NO_DATA)
  {
    return 2;
  }

  this->ErrorMsgLog(sqlRet, query, "SQLExecDirect", this->m_hStmtSelect);
  this->ErrorAction(sqlRet, this->m_hStmtSelect);
  return 1;
}

bool CRFWorldDatabase::Update_DisappearOwnerRecord()
{
  char Buffer[1040]{};
  sprintf_s(
    Buffer,
    1024,
    "update [dbo].[tbl_PostStorage] set [dbo].[tbl_PostStorage].dck=1 from (select p.serial from [dbo].[tbl_PostStorage] "
    "as p join [dbo].[tbl_base] as b on p.owner <> 0 and p.owner = b.serial and b.dck=1) as d where [dbo].[tbl_PostStorag"
    "e].serial = d.serial");
  return ExecUpdateQuery(Buffer, 0);
}

int CRFWorldDatabase::Select_PostStorageEmptyRecord()
{
  SQLLEN StrLen_or_IndPtr = 0;
  SQLRETURN ret = 0;
  char Buffer[148]{};
  unsigned int TargetValue[4]{};

  sprintf_s(Buffer, 128, "select count(serial) from tbl_PostStorage where dck=1");
  if (m_bSaveDBLog)
  {
    Log(Buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(Buffer), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == 1)
      {
        TargetValue[0] = 0;
        ret = SQLGetData(m_hStmtSelect, 1u, 4, TargetValue, 0, &StrLen_or_IndPtr);
        if (ret == 100)
        {
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
          }
          // narrowing cast for thunk return parity
          return static_cast<int>(4294967290LL);
        }

        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        if (m_bSaveDBLog)
        {
          FmtLog("%s Success", Buffer);
        }
        // narrowing cast for thunk return parity
        return static_cast<int>(TargetValue[0]);
      }

      unsigned int result = 0;
      if (ret == 100)
      {
        result = 4294967292;
      }
      else
      {
        ErrorMsgLog(ret, Buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
        result = 4294967291;
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      // narrowing cast for thunk return parity
      return static_cast<int>(result);
    }
    if (ret == 100)
    {
      return -2;
    }

    ErrorMsgLog(ret, Buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 4294967293;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
  return -1;
}

unsigned __int8 CRFWorldDatabase::Select_PostRegistryData(unsigned int dwMax, CPostData *pPostData)
{
  SQLLEN StrLen_or_IndPtr = 0;
  SQLRETURN ret = 0;
  char Buffer[84]{};
  unsigned __int8 sendRaceBuffer[32]{};
  unsigned __int8 senderDgrBuffer[32]{};
  unsigned int TargetValue[8]{};
  int pl_nKey[8]{};
  unsigned char stateBuffer[28]{};

  sprintf_s(Buffer, 64, "select * from tbl_PostRegistry where dck=0");
  if (m_bSaveDBLog)
  {
    Log(Buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(Buffer), -3);
    if (!ret || ret == 1)
    {
      while (true)
      {
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != 1)
        {
          break;
        }

        ret = SQLGetData(m_hStmtSelect, 1u, 65518, TargetValue, 0, &StrLen_or_IndPtr);
        if (TargetValue[0] < dwMax)
        {
          ret = SQLGetData(m_hStmtSelect, 2u, 65529, stateBuffer, 0, &StrLen_or_IndPtr);
          ret = SQLGetData(
            m_hStmtSelect,
            3u,
            4,
            &pPostData[TargetValue[0]].m_dwSenderSerial,
            0,
            &StrLen_or_IndPtr);
          ret = SQLGetData(
            m_hStmtSelect,
            4u,
            1,
            pPostData[TargetValue[0]].m_wszSendName,
            17,
            &StrLen_or_IndPtr);
          ret = SQLGetData(
            m_hStmtSelect,
            5u,
            1,
            pPostData[TargetValue[0]].m_wszRecvName,
            17,
            &StrLen_or_IndPtr);
          ret = SQLGetData(
            m_hStmtSelect,
            6u,
            1,
            pPostData[TargetValue[0]].m_wszTitle,
            21,
            &StrLen_or_IndPtr);
          ret = SQLGetData(
            m_hStmtSelect,
            7u,
            1,
            pPostData[TargetValue[0]].m_wszContent,
            201,
            &StrLen_or_IndPtr);
          ret = SQLGetData(m_hStmtSelect, 8u, 4, pl_nKey, 0, &StrLen_or_IndPtr);
          ret = SQLGetData(m_hStmtSelect, 9u, 65511, &pPostData[TargetValue[0]].m_dwDur, 0, &StrLen_or_IndPtr);
          ret = SQLGetData(m_hStmtSelect, 10, 4, &pPostData[TargetValue[0]].m_dwUpt, 0, &StrLen_or_IndPtr);
          ret = SQLGetData(m_hStmtSelect, 11, 4, &pPostData[TargetValue[0]].m_dwGold, 0, &StrLen_or_IndPtr);
          ret = SQLGetData(m_hStmtSelect, 12, 5, sendRaceBuffer, 0, &StrLen_or_IndPtr);
          ret = SQLGetData(m_hStmtSelect, 13, 5, senderDgrBuffer, 0, &StrLen_or_IndPtr);
          ret = SQLGetData(
            m_hStmtSelect,
            14,
            65511,
            &pPostData[TargetValue[0]].m_lnUID,
            0,
            &StrLen_or_IndPtr);
          pPostData[TargetValue[0]].m_bySendRace = sendRaceBuffer[0];
          pPostData[TargetValue[0]].m_bySenderDgr = senderDgrBuffer[0];
          pPostData[TargetValue[0]].m_Key.LoadDBKey(pl_nKey[0]);
          pPostData[TargetValue[0]].m_byState = 0;
        }
      }

      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      if (m_bSaveDBLog)
      {
        FmtLog("%s Success", Buffer);
      }
      return 0;
    }

    if (ret == 100)
    {
      return 2;
    }

    ErrorMsgLog(ret, Buffer, "SQLExecDirect", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
  return 1;
}

bool CRFWorldDatabase::Insert_PSDefaultRecord(unsigned int dwCum)
{
  char Buffer[68]{};
  sprintf_s(Buffer, 64, "{ CALL pInsert_PostStorageRecord }");
  for (unsigned int j = 0; j < dwCum; ++j)
  {
    if (!ExecUpdateQuery(Buffer, 1))
    {
      return false;
    }
  }
  return true;
}

unsigned __int8 CRFWorldDatabase::Select_RaceBossCurrentWinRate(
  unsigned __int8 byRace,
  char *szDate,
  unsigned int *dwTotalCnt,
  unsigned int *dwWinCnt)
{
  SQLLEN StrLen_or_IndPtr = 0;
  SQLRETURN ret = 0;
  char query[256]{};

  sprintf_s(
    query,
    "{ CALL pSelect_BossWinRate(%d, N'%s', '%s') }",
    byRace,
    g_Main.m_wszWorldName,
    szDate);

  if (m_bSaveDBLog)
  {
    Log(query);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == 1)
      {
        ret = SQLGetData(m_hStmtSelect, 1u, 65518, dwTotalCnt, 0, &StrLen_or_IndPtr);
        if (!ret || ret == 1)
        {
          ret = SQLGetData(m_hStmtSelect, 2u, 65518, dwWinCnt, 0, &StrLen_or_IndPtr);
          if (!ret || ret == 1)
          {
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
          unsigned __int8 result = 0;
          if (ret == 100)
          {
            result = 2;
          }
          else
          {
            ErrorMsgLog(ret, query, "SQLGetData", m_hStmtSelect);
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
        if (ret == 100)
        {
          result = 2;
        }
        else
        {
          ErrorMsgLog(ret, query, "SQLGetData", m_hStmtSelect);
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
      if (ret == 100)
      {
        result = 2;
      }
      else
      {
        ErrorMsgLog(ret, query, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
        result = 1;
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return result;
    }

    if (ret == 100)
    {
      return 2;
    }

    ErrorMsgLog(ret, query, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
  return 1;
}

int CRFWorldDatabase::Select_UnmannedTraderItemStateInfoCnt(unsigned int *pdwCnt)
{
  SQLLEN StrLen_or_IndPtr = 0;
  SQLRETURN ret = 0;
  char Buffer[260]{};

  sprintf_s(Buffer, 256, "select count([id]) from [dbo].[tbl_utresultstateid]");
  if (m_bSaveDBLog)
  {
    Log(Buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(Buffer), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == 1)
      {
        *pdwCnt = 0;
        ret = SQLGetData(m_hStmtSelect, 1u, 4, pdwCnt, 0, &StrLen_or_IndPtr);
        if (ret == 100)
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
          FmtLog("%s Success", Buffer);
        }
        return 0;
      }

      if (ret != 100)
      {
        ErrorMsgLog(ret, Buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return 1;
    }

    if (ret == 100)
    {
      return 2;
    }

    ErrorMsgLog(ret, Buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
  return 1;
}

int CRFWorldDatabase::Select_UnmannedTraderItemStateInfo(
  _unmannedtrader_stade_id_info *pkInfo,
  unsigned int dwMaxCnt)
{
  SQLLEN StrLen_or_IndPtr = 0;
  SQLRETURN ret = 0;
  char Buffer[260]{};

  sprintf_s(Buffer, sizeof(Buffer), "select top %u [id], [desc] from [dbo].[tbl_utresultstateid]", dwMaxCnt);
  if (m_bSaveDBLog)
  {
    Log(Buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(Buffer), -3);
    if (!ret || ret == 1)
    {
      for (int j = 0;; ++j)
      {
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != 1)
        {
          break;
        }

        ret = SQLGetData(m_hStmtSelect, 1u, 65518, &pkInfo[j].dwID, 0, &StrLen_or_IndPtr);
        ret = SQLGetData(m_hStmtSelect, 2u, 65528, pkInfo[j].wszDesc, 128, &StrLen_or_IndPtr);
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      if (m_bSaveDBLog)
      {
        FmtLog("%s Success", Buffer);
      }
      return 0;
    }

    if (ret == 100)
    {
      return 2;
    }

    ErrorMsgLog(ret, Buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
  return 1;
}

bool CRFWorldDatabase::Truncate_UnmannedTraderItemStateRecord()
{
  char Buffer[1040]{};
  sprintf_s(Buffer, sizeof(Buffer), "truncate table [dbo].[tbl_utresultstateid]");
  return ExecUpdateQuery(Buffer, 1);
}

bool CRFWorldDatabase::Insert_UnmannedTraderItemStateRecord(unsigned int dwRowCnt, wchar_t **ppwszStr)
{
  if (!dwRowCnt)
  {
    return false;
  }

  SetAutoCommitMode(0);
  for (unsigned int j = 0; j < dwRowCnt; ++j)
  {
    wchar_t query[1026]{};
    swprintf_s(
      query,
      L"insert into [dbo].[tbl_utresultstateid] ([id], [desc]) values ( %u, N'%s' )",
      j,
      ppwszStr[j]);
    if (!ExecUpdateQuery(query, 1))
    {
      RollbackTransaction();
      SetAutoCommitMode(1);
      return false;
    }
  }
  CommitTransaction();
  SetAutoCommitMode(1);
  return true;
}

bool CRFWorldDatabase::Update_UnmannedTraderClearDanglingOwnerRecord()
{
  char Buffer[1040]{};
  sprintf_s(
    Buffer,
    "update [dbo].[tbl_utresultinfo] set [dbo].[tbl_utresultinfo].state = 0 from ( select s.type, s.serial, s.owner, r.st"
    "ate from [dbo].[tbl_utsellinfo] as s join [dbo].[tbl_utresultinfo] as r on s.owner <> 0 and r.type = s.type and s.se"
    "rial = r.serial join [dbo].[tbl_base] as b on s.owner = b.serial and b.dck = 1 ) as d where [dbo].[tbl_utresultinfo]"
    ".type = d.type and [dbo].[tbl_utresultinfo].serial = d.serial");
  return ExecUpdateQuery(Buffer, 0);
}

int CRFWorldDatabase::Select_UnmannedTraderSingleItemEmptyRecordCnt()
{
  SQLLEN StrLen_or_IndPtr = 0;
  SQLRETURN ret = 0;
  char Buffer[260]{};
  unsigned int TargetValue[7]{};

  sprintf_s(Buffer, sizeof(Buffer), "select count(serial) from tbl_utresultinfo where type = 0 and state = 0");
  if (m_bSaveDBLog)
  {
    Log(Buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(Buffer), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == 1)
      {
        TargetValue[0] = 0;
        ret = SQLGetData(m_hStmtSelect, 1u, 4, TargetValue, 0, &StrLen_or_IndPtr);
        if (ret == 100)
        {
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
          }
          // narrowing cast for thunk return parity
          return static_cast<int>(4294967290LL);
        }

        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        if (m_bSaveDBLog)
        {
          FmtLog("%s Success", Buffer);
        }
        // narrowing cast for thunk return parity
        return static_cast<int>(TargetValue[0]);
      }

      unsigned int result = 0;
      if (ret == 100)
      {
        result = 4294967292;
      }
      else
      {
        ErrorMsgLog(ret, Buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
        result = 4294967291;
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      // narrowing cast for thunk return parity
      return static_cast<int>(result);
    }

    if (ret == 100)
    {
      return -2;
    }

    ErrorMsgLog(ret, Buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 4294967293;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
  return -1;
}

int CRFWorldDatabase::select_atrade_taxrate(
  unsigned __int8 byRace,
  char *pwszName,
  unsigned __int8 *byCurrTax,
  unsigned __int8 *byNextTax)
{
  SQLLEN StrLen_or_IndPtr = 0;
  SQLRETURN ret = 0;
  char Buffer[132]{};

  memset(Buffer, 0, 128);
  sprintf_s(
    Buffer,
    "SELECT TOP 1 SuggesterName, Tax, NextTax FROM [dbo].[tbl_ATradeTaxRate] WHERE Race=%d ORDER BY serial DESC",
    byRace);
  if (m_bSaveDBLog)
  {
    Log(Buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(Buffer), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == 1)
      {
        ret = SQLGetData(m_hStmtSelect, 1u, 1, pwszName, 17, &StrLen_or_IndPtr);
        ret = SQLGetData(m_hStmtSelect, 2u, 65530, byCurrTax, 0, &StrLen_or_IndPtr);
        ret = SQLGetData(m_hStmtSelect, 3u, 65530, byNextTax, 0, &StrLen_or_IndPtr);
        if (!ret || ret == 1)
        {
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
          }
          if (m_bSaveDBLog)
          {
            FmtLog("%s Success", Buffer);
          }
          return 0;
        }

        unsigned __int8 result = 0;
        if (ret == 100)
        {
          result = 2;
        }
        else
        {
          ErrorMsgLog(ret, Buffer, "SQLExecDirect", m_hStmtSelect);
          ErrorAction(ret, m_hStmtSelect);
          result = 1;
        }
        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        // narrowing cast for thunk return parity
        return static_cast<int>(result);
      }

      unsigned __int8 result = 0;
      if (ret == 100)
      {
        result = 2;
      }
      else
      {
        ErrorMsgLog(ret, Buffer, "SQLExecDirect", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
        result = 1;
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      // narrowing cast for thunk return parity
      return static_cast<int>(result);
    }

    if (ret == 100)
    {
      return 2;
    }

    ErrorMsgLog(ret, Buffer, "SQLExecDirect", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
  return 1;
}

unsigned __int8 CRFWorldDatabase::Select_CheckGreetRecord(int nUseType)
{
  char Buffer[152]{};
  SQLLEN strLen = 0;
  SQLRETURN ret = 0;
  unsigned int targetValue[4]{};

  sprintf_s(Buffer, "select count(useType) from tbl_GreetMsg where useType = %d", nUseType);
  if (this->m_bSaveDBLog)
  {
    this->Log(Buffer);
  }

  if (this->m_hStmtSelect || this->ReConnectDataBase())
  {
    ret = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(Buffer), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(this->m_hStmtSelect);
      if (!ret || ret == 1)
      {
        ret = SQLGetData(this->m_hStmtSelect, 1u, 65530, targetValue, 0, &strLen);
        if (ret != 100 && targetValue[0])
        {
          if (this->m_hStmtSelect)
          {
            SQLCloseCursor(this->m_hStmtSelect);
          }
          if (this->m_bSaveDBLog)
          {
            this->FmtLog("%s Success", Buffer);
          }
          return 0;
        }

        if (this->m_hStmtSelect)
        {
          SQLCloseCursor(this->m_hStmtSelect);
        }
        return 2;
      }
      else
      {
        char result = 0;
        if (ret == 100)
        {
          result = 2;
        }
        else
        {
          this->ErrorMsgLog(ret, Buffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction(ret, this->m_hStmtSelect);
          result = 1;
        }
        if (this->m_hStmtSelect)
        {
          SQLCloseCursor(this->m_hStmtSelect);
        }
        return result;
      }
    }
    else if (ret == 100)
    {
      return 2;
    }
    else
    {
      this->ErrorMsgLog(ret, Buffer, "SQLExecDirect", this->m_hStmtSelect);
      this->ErrorAction(ret, this->m_hStmtSelect);
      return 1;
    }
  }

  this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
  return 1;
}

bool CRFWorldDatabase::Insert_GreetingRecord(int nUseType, const char *wszName, const char *wszMessage)
{
  char Buffer[528]{};

  sprintf_s(Buffer, "{CALL pInsert_GreetingMsgRecord_071119(%d,'%s','%s')}", nUseType, wszName, wszMessage);
  return this->ExecUpdateQuery(Buffer, true);
}

bool CRFWorldDatabase::LoadGreetingMsg(
  char *pwszGMGreetingmsg,
  char *pwszRaceGreetingmsgA,
  char *pwszRaceGreetingmsgB,
  char *pwszRaceGreetingmsgC,
  char *pwszGMName,
  char *pwszNameA,
  char *pwszNameB,
  char *pwszNameC)
{
  char Buffer[280]{};
  SQLLEN strLen = 0;
  SQLRETURN ret = 0;

  sprintf_s(Buffer, "select GMsg,Name from tbl_GreetMsg where usetype = %d", 0);
  if (this->m_bSaveDBLog)
  {
    this->Log(Buffer);
  }
  if (this->m_hStmtSelect || this->ReConnectDataBase())
  {
    ret = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(Buffer), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(this->m_hStmtSelect);
      if (!ret || ret == 1)
      {
        ret = SQLGetData(this->m_hStmtSelect, 1u, 1, pwszRaceGreetingmsgA, 256, &strLen);
        ret = SQLGetData(this->m_hStmtSelect, 2u, 1, pwszNameA, 17, &strLen);
        if (this->m_hStmtSelect)
        {
          SQLCloseCursor(this->m_hStmtSelect);
        }

        sprintf_s(Buffer, "select GMsg,Name from tbl_GreetMsg where usetype = %d", 1);
        if (this->m_bSaveDBLog)
        {
          this->Log(Buffer);
        }
        if (this->m_hStmtSelect || this->ReConnectDataBase())
        {
          ret = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(Buffer), -3);
          if (!ret || ret == 1)
          {
            ret = SQLFetch(this->m_hStmtSelect);
            if (!ret || ret == 1)
            {
              ret = SQLGetData(this->m_hStmtSelect, 1u, 1, pwszRaceGreetingmsgB, 256, &strLen);
              ret = SQLGetData(this->m_hStmtSelect, 2u, 1, pwszNameB, 17, &strLen);
              if (this->m_hStmtSelect)
              {
                SQLCloseCursor(this->m_hStmtSelect);
              }

              sprintf_s(Buffer, "select GMsg,Name from tbl_GreetMsg where usetype = %d", 2);
              if (this->m_bSaveDBLog)
              {
                this->Log(Buffer);
              }
              if (this->m_hStmtSelect || this->ReConnectDataBase())
              {
                ret = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(Buffer), -3);
                if (!ret || ret == 1)
                {
                  ret = SQLFetch(this->m_hStmtSelect);
                  if (!ret || ret == 1)
                  {
                    ret = SQLGetData(this->m_hStmtSelect, 1u, 1, pwszRaceGreetingmsgC, 256, &strLen);
                    ret = SQLGetData(this->m_hStmtSelect, 2u, 1, pwszNameC, 17, &strLen);
                    if (this->m_hStmtSelect)
                    {
                      SQLCloseCursor(this->m_hStmtSelect);
                    }

                    sprintf_s(Buffer, "select GMsg,Name from tbl_GreetMsg where usetype = %d", 255);
                    if (this->m_bSaveDBLog)
                    {
                      this->Log(Buffer);
                    }
                    if (this->m_hStmtSelect || this->ReConnectDataBase())
                    {
                      ret = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(Buffer), -3);
                      if (!ret || ret == 1)
                      {
                        ret = SQLFetch(this->m_hStmtSelect);
                        if (!ret || ret == 1)
                        {
                          ret = SQLGetData(this->m_hStmtSelect, 1u, 1, pwszGMGreetingmsg, 256, &strLen);
                          ret = SQLGetData(this->m_hStmtSelect, 2u, 1, pwszGMName, 17, &strLen);
                          if (this->m_hStmtSelect)
                          {
                            SQLCloseCursor(this->m_hStmtSelect);
                          }
                          if (this->m_bSaveDBLog)
                          {
                            this->FmtLog("%s Success", Buffer);
                          }
                          return true;
                        }
                        else
                        {
                          if (ret != 100)
                          {
                            this->ErrorMsgLog(ret, Buffer, "SQLFetch", this->m_hStmtSelect);
                            this->ErrorAction(ret, this->m_hStmtSelect);
                          }
                          if (this->m_hStmtSelect)
                          {
                            SQLCloseCursor(this->m_hStmtSelect);
                          }
                          return false;
                        }
                      }
                      else
                      {
                        if (ret != 100)
                        {
                          this->ErrorMsgLog(ret, Buffer, "SQLExecDirect", this->m_hStmtSelect);
                          this->ErrorAction(ret, this->m_hStmtSelect);
                        }
                        return false;
                      }
                    }
                    else
                    {
                      this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
                      return false;
                    }
                  }
                  else
                  {
                    if (ret != 100)
                    {
                      this->ErrorMsgLog(ret, Buffer, "SQLFetch", this->m_hStmtSelect);
                      this->ErrorAction(ret, this->m_hStmtSelect);
                    }
                    if (this->m_hStmtSelect)
                    {
                      SQLCloseCursor(this->m_hStmtSelect);
                    }
                    return false;
                  }
                }
                else
                {
                  if (ret != 100)
                  {
                    this->ErrorMsgLog(ret, Buffer, "SQLExecDirect", this->m_hStmtSelect);
                    this->ErrorAction(ret, this->m_hStmtSelect);
                  }
                  return false;
                }
              }
              else
              {
                this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
                return false;
              }
            }
            else
            {
              if (ret != 100)
              {
                this->ErrorMsgLog(ret, Buffer, "SQLFetch", this->m_hStmtSelect);
                this->ErrorAction(ret, this->m_hStmtSelect);
              }
              if (this->m_hStmtSelect)
              {
                SQLCloseCursor(this->m_hStmtSelect);
              }
              return false;
            }
          }
          else
          {
            if (ret != 100)
            {
              this->ErrorMsgLog(ret, Buffer, "SQLExecDirect", this->m_hStmtSelect);
              this->ErrorAction(ret, this->m_hStmtSelect);
            }
            return false;
          }
        }
        else
        {
          this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
          return false;
        }
      }
      else
      {
        if (ret != 100)
        {
          this->ErrorMsgLog(ret, Buffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction(ret, this->m_hStmtSelect);
        }
        if (this->m_hStmtSelect)
        {
          SQLCloseCursor(this->m_hStmtSelect);
        }
        return false;
      }
    }
    else
    {
      if (ret != 100)
      {
        this->ErrorMsgLog(ret, Buffer, "SQLExecDirect", this->m_hStmtSelect);
        this->ErrorAction(ret, this->m_hStmtSelect);
      }
      return false;
    }
  }
  else
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
    return false;
  }
}

unsigned __int8 CRFWorldDatabase::Select_LimitInfo(unsigned __int8 *pData, unsigned __int64 tDataSize)
{
  char query[1024]{};
  unsigned __int16 columnNumber[14]{};

  sprintf_s(query, sizeof(query), "select [effect] from [dbo].[tbl_sf_delay] where [aserial] = 0");
  unsigned __int8 ret = SQLExecDirect_RetErrCode(query);
  if (ret)
  {
    return ret;
  }
  ret = SQLFetch_RetErrCode(query);
  if (ret)
  {
    return ret;
  }
  columnNumber[0] = 1;
  ret = SQLGetData_Binary_RetErrCode(query, columnNumber, pData, tDataSize);
  if (ret)
  {
    return ret;
  }
  SelectCleanUp(query);
  return 0;
}

bool CRFWorldDatabase::Select_BattleTournamentInfo(TournamentWinner *pWinnerInfo, int nMax)
{
  char Buffer[152]{};
  SQLLEN strLen = 0;
  SQLRETURN ret = 0;

  sprintf_s(
    Buffer,
    sizeof(Buffer),
    "Select top %d CharacterSerial,CharacterName,BattleWinGrade from tbl_battletournament",
    nMax);
  if (this->m_bSaveDBLog)
  {
    this->Log(Buffer);
  }

  if (this->m_hStmtSelect || this->ReConnectDataBase())
  {
    ret = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(Buffer), -3);
    if (!ret || ret == 1)
    {
      for (int j = 0; ; ++j)
      {
        ret = SQLFetch(this->m_hStmtSelect);
        if (ret == 100)
        {
          break;
        }
        if (ret && ret != 1)
        {
          this->ErrorMsgLog(ret, Buffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction(ret, this->m_hStmtSelect);
          if (this->m_hStmtSelect)
          {
            SQLCloseCursor(this->m_hStmtSelect);
          }
          return false;
        }
        if (j >= nMax)
        {
          break;
        }
        ret = SQLGetData(this->m_hStmtSelect, 1u, 4, &pWinnerInfo[j], 0, &strLen);
        ret = SQLGetData(this->m_hStmtSelect, 2u, 1, pWinnerInfo[j].wszCharName, 17, &strLen);
        ret = SQLGetData(this->m_hStmtSelect, 3u, 65530, &pWinnerInfo[j].byGrade, 0, &strLen);
        if (ret && ret != 1)
        {
          if (ret != 100)
          {
            this->ErrorMsgLog(ret, Buffer, "SQLGetData", this->m_hStmtSelect);
            this->ErrorAction(ret, this->m_hStmtSelect);
          }
          if (this->m_hStmtSelect)
          {
            SQLCloseCursor(this->m_hStmtSelect);
          }
          return false;
        }
      }
      if (this->m_hStmtSelect)
      {
        SQLCloseCursor(this->m_hStmtSelect);
      }
      if (this->m_bSaveDBLog)
      {
        this->FmtLog("%s Success", Buffer);
      }
      return true;
    }
    else if (ret == 100)
    {
      return true;
    }
    else
    {
      this->ErrorMsgLog(ret, Buffer, "SQLExecDirect", this->m_hStmtSelect);
      this->ErrorAction(ret, this->m_hStmtSelect);
      return false;
    }
  }

  this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
  return false;
}

int CRFWorldDatabase::Select_GodenBoxItem(_worlddb_golden_box_item *goldenboxitem, int *pnSerial)
{
  char query[1024]{};
  SQLLEN strLen = 0;
  SQLRETURN ret = 0;

  sprintf_s(query, sizeof(query), "{ CALL pSelect_GoldBoxItem }");
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }
  if (this->m_hStmtSelect || this->ReConnectDataBase())
  {
    ret = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(this->m_hStmtSelect);
      if (!ret || ret == 1)
      {
        int column = 1;
        ret = SQLGetData(this->m_hStmtSelect, 1u, 4, pnSerial, 0, &strLen);
        ret = SQLGetData(this->m_hStmtSelect, ++column, 65530, &goldenboxitem->bydck, 0, &strLen);
        ret = SQLGetData(this->m_hStmtSelect, ++column, 4, &goldenboxitem->dwStarterBoxCnt, 0, &strLen);
        for (int j = 0; ; ++j)
        {
          const unsigned __int8 loopCount = CGoldenBoxItemMgr::Instance()->GetLoopCount();
          if (j >= loopCount)
          {
            break;
          }
          int maxCount = -1;
          ret = SQLGetData(
            this->m_hStmtSelect,
            ++column,
            4,
            &goldenboxitem->nBox_item_code[j],
            0,
            &strLen);
          ret = SQLGetData(this->m_hStmtSelect, ++column, 4, &maxCount, 0, &strLen);
          goldenboxitem->wBox_item_max[j] = static_cast<unsigned __int16>(maxCount);
          ret = SQLGetData(
            this->m_hStmtSelect,
            ++column,
            65530,
            &goldenboxitem->bygolden_item_num[j],
            0,
            &strLen);
        }
        for (int k = 0; ; ++k)
        {
          const unsigned __int8 loopCount = CGoldenBoxItemMgr::Instance()->GetLoopCount();
          if (k >= loopCount)
          {
            break;
          }
          for (int m = 0; m < 100; ++m)
          {
            ret = SQLGetData(
              this->m_hStmtSelect,
              ++column,
              4,
              &goldenboxitem->List[k][m],
              0,
              &strLen);
            ret = SQLGetData(
              this->m_hStmtSelect,
              ++column,
              4,
              &goldenboxitem->List[k][m].wGoldencount,
              0,
              &strLen);
          }
        }
        if (this->m_hStmtSelect)
        {
          SQLCloseCursor(this->m_hStmtSelect);
        }
        if (this->m_bSaveDBLog)
        {
          this->FmtLog("%s Success", query);
        }
        return 0;
      }
      else
      {
        unsigned __int8 result = 0;
        if (ret == 100)
        {
          result = 2;
        }
        else
        {
          this->ErrorMsgLog(ret, query, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction(ret, this->m_hStmtSelect);
          result = 1;
        }
        if (this->m_hStmtSelect)
        {
          SQLCloseCursor(this->m_hStmtSelect);
        }
        // narrowing cast for thunk return parity
        return static_cast<int>(result);
      }
    }
    else if (ret == 100)
    {
      return 2;
    }
    else
    {
      this->ErrorMsgLog(ret, query, "SQLExecDirect", this->m_hStmtSelect);
      this->ErrorAction(ret, this->m_hStmtSelect);
      return 1;
    }
  }

  this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
  return 1;
}

bool CRFWorldDatabase::Select_GuildRoomInfo(_guildroom_info *pInfo)
{
  char query[256]{};
  SQLLEN strLen = 0;
  SQLRETURN ret = 0;
  unsigned __int8 count = 0;

  std::memset(query, 0, sizeof(query));
  sprintf_s(
    query,
    "select gr.guildserial, g.id, gr.roomtype, gr.roomrace, gr.roomrentdate from tbl_GuildRoom as gr inner join tbl_Guild"
    " as g on gr.guildserial = g.serial where gr.dck = 0 order by g.serial desc");

  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }

  if (this->m_hStmtSelect || this->ReConnectDataBase())
  {
    ret = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), -3);
    if (!ret || ret == 1)
    {
      count = 0;
      do
      {
        ret = SQLFetch(this->m_hStmtSelect);
        if (ret && ret != 1)
        {
          break;
        }
        ret = SQLGetData(this->m_hStmtSelect, 1u, 65518, &pInfo->info[count], 0, &strLen);
        ret = SQLGetData(this->m_hStmtSelect, 2u, 1, pInfo->info[count].uszGuildName, 17, &strLen);
        ret = SQLGetData(this->m_hStmtSelect, 3u, 65530, &pInfo->info[count].byRoomType, 0, &strLen);
        ret = SQLGetData(this->m_hStmtSelect, 4u, 65530, &pInfo->info[count].byRace, 0, &strLen);
        ret = SQLGetData(this->m_hStmtSelect, 5u, 93, &pInfo->info[count++].ts, 0, &strLen);
        if (count >= 90)
        {
          break;
        }
      } while (!ret || ret == 1);

      pInfo->byCount = count;
      if (this->m_hStmtSelect)
      {
        SQLCloseCursor(this->m_hStmtSelect);
      }
      if (this->m_bSaveDBLog)
      {
        this->FmtLog("%s Success", query);
      }
      return true;
    }
    else
    {
      if (ret != 100)
      {
        this->ErrorMsgLog(ret, query, "SQLExecDirect", this->m_hStmtSelect);
        this->ErrorAction(ret, this->m_hStmtSelect);
      }
      return false;
    }
  }

  this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
  return false;
}

unsigned __int8 CRFWorldDatabase::Select_UsedLimitItemRecordNum(unsigned int *pdwUsedNum)
{
  char query[132]{};
  SQLLEN strLen = 0;
  SQLRETURN ret = 0;

  memset(query, 0, 128);
  sprintf_s(query, 128, "select count(serial) from tbl_StoreLimitItem_061212 where dck=0");
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }

  if (this->m_hStmtSelect || this->ReConnectDataBase())
  {
    ret = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(this->m_hStmtSelect);
      if (!ret || ret == 1)
      {
        ret = SQLGetData(this->m_hStmtSelect, 1u, 65518, pdwUsedNum, 0, &strLen);
        if (!ret || ret == 1)
        {
          if (this->m_hStmtSelect)
          {
            SQLCloseCursor(this->m_hStmtSelect);
          }
          if (this->m_bSaveDBLog)
          {
            this->FmtLog("%s Success", query);
          }
          return 0;
        }

        this->ErrorMsgLog(ret, query, "SQLGetData", this->m_hStmtSelect);
        this->ErrorAction(ret, this->m_hStmtSelect);
        if (this->m_hStmtSelect)
        {
          SQLCloseCursor(this->m_hStmtSelect);
        }
        return 1;
      }

      unsigned __int8 result = 0;
      if (ret == 100)
      {
        result = 2;
      }
      else
      {
        this->ErrorMsgLog(ret, query, "SQLFetch", this->m_hStmtSelect);
        this->ErrorAction(ret, this->m_hStmtSelect);
        result = 1;
      }
      if (this->m_hStmtSelect)
      {
        SQLCloseCursor(this->m_hStmtSelect);
      }
      return result;
    }

    if (ret == 100)
    {
      return 2;
    }

    this->ErrorMsgLog(ret, query, "SQLExecDirect", this->m_hStmtSelect);
    this->ErrorAction(ret, this->m_hStmtSelect);
    return 1;
  }

  this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
  return 1;
}

unsigned __int8 CRFWorldDatabase::Select_TotalRecordNum(unsigned int *pdwTotalNum)
{
  char query[132]{};
  SQLLEN strLen = 0;
  SQLRETURN ret = 0;

  memset(query, 0, 128);
  sprintf_s(query, 128, "select count(serial) from tbl_StoreLimitItem_061212");
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }

  if (this->m_hStmtSelect || this->ReConnectDataBase())
  {
    ret = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(this->m_hStmtSelect);
      if (!ret || ret == 1)
      {
        ret = SQLGetData(this->m_hStmtSelect, 1u, 65518, pdwTotalNum, 0, &strLen);
        if (!ret || ret == 1)
        {
          if (this->m_hStmtSelect)
          {
            SQLCloseCursor(this->m_hStmtSelect);
          }
          if (this->m_bSaveDBLog)
          {
            this->FmtLog("%s Success", query);
          }
          return 0;
        }

        this->ErrorMsgLog(ret, query, "SQLGetData", this->m_hStmtSelect);
        this->ErrorAction(ret, this->m_hStmtSelect);
        if (this->m_hStmtSelect)
        {
          SQLCloseCursor(this->m_hStmtSelect);
        }
        return 1;
      }

      unsigned __int8 result = 0;
      if (ret == 100)
      {
        result = 2;
      }
      else
      {
        this->ErrorMsgLog(ret, query, "SQLFetch", this->m_hStmtSelect);
        this->ErrorAction(ret, this->m_hStmtSelect);
        result = 1;
      }
      if (this->m_hStmtSelect)
      {
        SQLCloseCursor(this->m_hStmtSelect);
      }
      return result;
    }

    if (ret == 100)
    {
      return 2;
    }

    this->ErrorMsgLog(ret, query, "SQLExecDirect", this->m_hStmtSelect);
    this->ErrorAction(ret, this->m_hStmtSelect);
    return 1;
  }

  this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
  return 1;
}

unsigned __int8 CRFWorldDatabase::Select_StoreLimitItem(_qry_case_all_store_limit_item *pData)
{
  char query[516]{};
  SQLLEN strLen = 0;
  SQLRETURN ret = 0;
  int j = 0;
  unsigned int count = 0;
  int targetValue[8]{};
  int limitValues[4]{};
  const int maxItems = 16;

  memset(query, 0, 512);
  sprintf_s(
    query,
    512,
    "select k0,k1,k2,k3,k4,k5,k6,k7,k8,k9,k10,k11,k12,k13,k14,k15,num0,num1,num2,num3,num4,num5,num6,num7,num8,num9,num10"
    ",num11,num12,num13,num14,num15,serial,type,typeserial,storeinx,resettime from tbl_StoreLimitItem_061212 where dck=0");

  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }

  if (this->m_hStmtSelect || this->ReConnectDataBase())
  {
    ret = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), -3);
    if (!ret || ret == 1)
    {
      count = 0;
      while (true)
      {
        ret = SQLFetch(this->m_hStmtSelect);
        if (ret && ret != 1)
        {
          pData->dwCount = count;
          if (this->m_hStmtSelect)
          {
            SQLCloseCursor(this->m_hStmtSelect);
          }
          if (this->m_bSaveDBLog)
          {
            this->FmtLog("%s Success", query);
          }
          return 0;
        }

        for (j = 1; j <= maxItems; ++j)
        {
          ret = SQLGetData(this->m_hStmtSelect, j, 4, targetValue, 0, &strLen);
          if (ret && ret != 1)
          {
            this->ErrorMsgLog(ret, query, "SQLExecDirect", this->m_hStmtSelect);
            this->ErrorAction(ret, this->m_hStmtSelect);
            if (this->m_hStmtSelect)
            {
              SQLCloseCursor(this->m_hStmtSelect);
            }
            return 1;
          }
          pData->pStoreList[count].ItemData[j - 1].Key.LoadDBKey(targetValue[0]);
        }

        while (true)
        {
          if (j > 2 * maxItems)
          {
            ret = SQLGetData(this->m_hStmtSelect, j, 65518, &pData->pStoreList[count], 0, &strLen);
            ret = SQLGetData(this->m_hStmtSelect, j + 1, 5, &pData->pStoreList[count].byType, 0, &strLen);
            ret = SQLGetData(this->m_hStmtSelect, j + 2, 65518, &pData->pStoreList[count].nTypeSerial, 0, &strLen);
            ret = SQLGetData(this->m_hStmtSelect, j + 3, 65518, &pData->pStoreList[count].dwStoreIndex, 0, &strLen);
            ret = SQLGetData(
              this->m_hStmtSelect,
              j + 4,
              65511,
              &pData->pStoreList[count++].dwLimitInitTime,
              0,
              &strLen);
            break;
          }

          ret = SQLGetData(this->m_hStmtSelect, j, 4, limitValues, 0, &strLen);
          if (ret && ret != 1)
          {
            this->ErrorMsgLog(ret, query, "SQLExecDirect", this->m_hStmtSelect);
            this->ErrorAction(ret, this->m_hStmtSelect);
            if (this->m_hStmtSelect)
            {
              SQLCloseCursor(this->m_hStmtSelect);
            }
            return 1;
          }
          pData->pStoreList[count].ItemData[j - maxItems - 1].nLimitNum = limitValues[0];
          ++j;
        }
      }
    }

    if (ret == 100)
    {
      return 2;
    }

    this->ErrorMsgLog(ret, query, "SQLExecDirect", this->m_hStmtSelect);
    this->ErrorAction(ret, this->m_hStmtSelect);
    return 1;
  }

  this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
  return 1;
}

bool CRFWorldDatabase::Insert_GoldenBoxItem()
{
  char Buffer[1040]{};

  sprintf_s(Buffer, sizeof(Buffer), "{ CALL pInsert_GoldBoxItem( %d ) }", 0);
  return this->ExecUpdateQuery(Buffer, true);
}

bool CRFWorldDatabase::Insert_DefaultWeeklyGuildPvpPointSumRecord()
{
  char Buffer[272]{};

  sprintf_s(Buffer, "{ CALL pInsert_DefaultRecordWeeklyGuildPVPPointSum }");
  return this->ExecUpdateQuery(Buffer, false);
}

unsigned __int8 CRFWorldDatabase::Select_Economy_History(
  _worlddb_economy_history_info_array *pEconomyData,
  unsigned int dwDate)
{
  SQLLEN StrLen_or_IndPtr{};
  char Buffer[260]{};
  int rowCount = 0;

  sprintf_s(Buffer, "{ CALL pSelect_Last_Economy_History ( %d )}", dwDate);
  if (m_bSaveDBLog)
  {
    Log(Buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if (!ret || ret == 1)
    {
      while (true)
      {
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != 1)
        {
          break;
        }

        ret = SQLGetData(
          m_hStmtSelect,
          1u,
          8,
          pEconomyData->EconomyData[rowCount].dTradeDalant,
          0LL,
          &StrLen_or_IndPtr);
        ret = SQLGetData(
          m_hStmtSelect,
          2u,
          8,
          &pEconomyData->EconomyData[rowCount].dTradeDalant[1],
          0LL,
          &StrLen_or_IndPtr);
        ret = SQLGetData(
          m_hStmtSelect,
          3u,
          8,
          &pEconomyData->EconomyData[rowCount].dTradeDalant[2],
          0LL,
          &StrLen_or_IndPtr);
        ret = SQLGetData(
          m_hStmtSelect,
          4u,
          8,
          &pEconomyData->EconomyData[rowCount],
          0LL,
          &StrLen_or_IndPtr);
        ret = SQLGetData(
          m_hStmtSelect,
          5u,
          8,
          &pEconomyData->EconomyData[rowCount].dTradeGold[1],
          0LL,
          &StrLen_or_IndPtr);
        ret = SQLGetData(
          m_hStmtSelect,
          6u,
          8,
          &pEconomyData->EconomyData[rowCount].dTradeGold[2],
          0LL,
          &StrLen_or_IndPtr);
        ret = SQLGetData(
          m_hStmtSelect,
          7u,
          4,
          &pEconomyData->EconomyData[rowCount].dwManageValue,
          0LL,
          &StrLen_or_IndPtr);

        SQLUSMALLINT column = 8;
        for (int j = 0; j < 3; ++j)
        {
          for (int k = 0; k < 3; ++k)
          {
            ret = SQLGetData(
              m_hStmtSelect,
              column++,
              8,
              &pEconomyData->EconomyData[rowCount].dMineOre[k][j],
              0LL,
              &StrLen_or_IndPtr);
            if (ret)
            {
              if (ret != 1)
              {
                break;
              }
            }
          }
        }

        for (int j = 0; j < 3; ++j)
        {
          for (int m = 0; m < 3; ++m)
          {
            ret = SQLGetData(
              m_hStmtSelect,
              column++,
              8,
              &pEconomyData->EconomyData[rowCount].dCutOre[m][j],
              0LL,
              &StrLen_or_IndPtr);
            if (ret)
            {
              if (ret != 1)
              {
                break;
              }
            }
          }
        }

        ++rowCount;
        if (ret)
        {
          if (ret != 1)
          {
            break;
          }
        }
      }

      pEconomyData->wRowCount = static_cast<unsigned __int16>(rowCount);
      if (m_hStmtUpdate)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      if (m_bSaveDBLog)
      {
        FmtLog("%s Success", Buffer);
      }
      return 0;
    }
    else if (ret == 100)
    {
      return 2;
    }
    else
    {
      ErrorMsgLog(ret, Buffer, "SQLExecDirectA", m_hStmtSelect);
      ErrorAction(ret, m_hStmtSelect);
      return 1;
    }
  }
  else
  {
    ErrFmtLog("ReConnectDataBase Fail. Query : %s", Buffer);
    return 1;
  }
}

bool CRFWorldDatabase::UpdateClearGuildBattleInfo(unsigned int dwStartID, unsigned int dwEndID)
{
  char buffer[1040]{};
  sprintf_s(buffer, "{ CALL pUpdate_ClearGuildBattleInfo( %u, %u ) }", dwStartID, dwEndID);
  return ExecUpdateQuery(buffer, 1);
}

bool CRFWorldDatabase::UpdateWinGuildBattleResult(unsigned int dwGuildSerial, unsigned int dwScore)
{
  char buffer[1040]{};
  sprintf_s(buffer, "{ CALL pUpdate_WinGuildBattleRank( %u, %u ) }", dwGuildSerial, dwScore);
  return ExecUpdateQuery(buffer, 1);
}

bool CRFWorldDatabase::UpdateLoseGuildBattleResult(unsigned int dwGuildSerial, unsigned int dwScore)
{
  char buffer[1040]{};
  sprintf_s(buffer, "{ CALL pUpdate_LoseGuildBattleRank( %u, %u ) }", dwGuildSerial, dwScore);
  return ExecUpdateQuery(buffer, 1);
}

bool CRFWorldDatabase::UpdateDrawGuildBattleResult(unsigned int dwGuildSerial, unsigned int dwScore)
{
  char buffer[1040]{};
  sprintf_s(buffer, "{ CALL pUpdate_DrawGuildBattleRank( %u, %u ) }", dwGuildSerial, dwScore);
  return ExecUpdateQuery(buffer, 1);
}

bool CRFWorldDatabase::SelectGuildBattleRankRecord(unsigned int dwGuildSerial)
{
  SQLLEN strLenOrInd = 0;
  char buffer[1044]{};
  sprintf_s(buffer, "{ CALL pSelect_GuildBattleRank(%u) }", dwGuildSerial);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == 1)
      {
        unsigned int value = 0;
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, &value, 0, &strLenOrInd);
        if (ret == SQL_NO_DATA)
        {
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
          }
          return false;
        }

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

      if (ret != SQL_NO_DATA)
      {
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return false;
    }

    if (ret != SQL_NO_DATA)
    {
      ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
      ErrorAction(ret, m_hStmtSelect);
    }
    return false;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return false;
}

bool CRFWorldDatabase::InsertGuildBattleRankRecord(unsigned int dwGuildSerial)
{
  char buffer[1040]{};
  sprintf_s(buffer, "{ CALL pInsert_GuildBattleRank( %u ) }", dwGuildSerial);
  return ExecUpdateQuery(buffer, 1);
}

bool CRFWorldDatabase::SelectGuildBattleRankList(
  unsigned __int8 byRace,
  _worlddb_guild_battle_rank_list *pkInfo)
{
  if (!pkInfo)
  {
    return false;
  }

  char buffer[1048]{};
  SQLLEN strLenOrInd = 0;
  sprintf_s(buffer, "{ CALL pSelect_GuildBattleRankList(%u) }", byRace);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == 1)
    {
      pkInfo->wCount = 0;
      unsigned __int16 rank = 1;
      for (;; pkInfo->list[pkInfo->wCount++].nRank = rank)
      {
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != 1)
        {
          break;
        }

        SQLGetData(m_hStmtSelect, 1u, 65518, &pkInfo->list[pkInfo->wCount].dwSerial, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 2u, 65530, &pkInfo->list[pkInfo->wCount].byGrade, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 3u, 1, pkInfo->list[pkInfo->wCount].wszName, 17, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 4u, 65518, &pkInfo->list[pkInfo->wCount].dwWin, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 5u, 65518, &pkInfo->list[pkInfo->wCount].dwDraw, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 6u, 65518, &pkInfo->list[pkInfo->wCount].dwLose, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 7u, 65518, &pkInfo->list[pkInfo->wCount].dwScore, 0, &strLenOrInd);

        if (pkInfo->wCount)
        {
          const unsigned __int16 prevIndex = pkInfo->wCount - 1;
          if (pkInfo->list[prevIndex].dwScore != pkInfo->list[pkInfo->wCount].dwScore
              || pkInfo->list[prevIndex].dwWin != pkInfo->list[pkInfo->wCount].dwWin
              || pkInfo->list[prevIndex].dwDraw != pkInfo->list[pkInfo->wCount].dwDraw
              || pkInfo->list[prevIndex].dwLose != pkInfo->list[pkInfo->wCount].dwLose)
          {
            rank = pkInfo->wCount + 1;
          }
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
      return true;
    }
    if (ret == 100)
    {
      return true;
    }

    ErrorMsgLog(ret, buffer, "SQLExecDirect", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return false;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return false;
}

bool CRFWorldDatabase::SelectGuildBattleRerservedList(
  unsigned int uiStartSLID,
  unsigned int uiEndSLID,
  _worlddb_guild_battle_reserved_schedule_info *pkInfo)
{
  if (!pkInfo)
  {
    return false;
  }

  SQLLEN strLenOrInd = 0;
  char buffer[1028]{};
  unsigned int count = 0;

  sprintf_s(buffer, "{ CALL pSelect_GuildBattleReservedSchedule( %u, %u ) }", uiStartSLID, uiEndSLID);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == 1)
    {
      do
      {
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != 1)
        {
          break;
        }

        SQLGetData(m_hStmtSelect, 1u, 65518, &pkInfo->list[count], 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 2u, 1, pkInfo->list[count].wsz1PName, 17, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 3u, 65530, &pkInfo->list[count].by1PRace, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 4u, 65518, &pkInfo->list[count].dw2PGuildSerial, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 5u, 1, pkInfo->list[count].wsz2PName, 17, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 6u, 65530, &pkInfo->list[count].by2PRace, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 7u, 65530, &pkInfo->list[count].byStartHour, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 8u, 65530, &pkInfo->list[count].byStartMin, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 9u, 65530, &pkInfo->list[count].byEndHour, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 10u, 65530, &pkInfo->list[count++].byEndMin, 0, &strLenOrInd);
      }
      while (count < 46);

      pkInfo->wCount = count;
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

    if (ret != 100)
    {
      ErrorMsgLog(ret, buffer, "SQLExecDirect", m_hStmtSelect);
      ErrorAction(ret, m_hStmtSelect);
    }
    return false;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return false;
}

unsigned __int8 CRFWorldDatabase::SelectGuildBattleScheduleInfoID(unsigned int dwID)
{
  SQLLEN indicator = 0;
  SQLRETURN ret = 0;
  char buffer[1028]{};
  unsigned int targetValue = 0;

  sprintf_s(buffer, "{ CALL pSelect_GuildBattleScheduleID( %u ) }", dwID);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (!(m_hStmtSelect || ReConnectDataBase()))
  {
    ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
    return 1;
  }

  ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), SQL_NTS);
  if (ret && ret != SQL_SUCCESS_WITH_INFO)
  {
    if (ret == SQL_NO_DATA)
    {
      return 2;
    }

    ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ret = SQLFetch(m_hStmtSelect);
  if (ret && ret != SQL_SUCCESS_WITH_INFO)
  {
    char result = 0;
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

  targetValue = 0;
  ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, &targetValue, 0, &indicator);
  if (ret == SQL_NO_DATA)
  {
    if (m_hStmtSelect)
    {
      SQLCloseCursor(m_hStmtSelect);
    }
    return 2;
  }

  if (targetValue == dwID)
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

  return 2;
}

bool CRFWorldDatabase::UpdateClearGuildBattleScheduleInfo(unsigned int uiStartListID, unsigned int uiEndListID)
{
  char buffer[1040]{};
  sprintf_s(buffer, "{ CALL pUpdate_ClearGuildBattleScheduleInfo( %u, %u ) }", uiStartListID, uiEndListID);
  return ExecUpdateQuery(buffer, 1);
}

int CRFWorldDatabase::SelectRowCountGuildBattleInfo()
{
  SQLLEN strLenOrInd = 0;
  char buffer[1028]{};

  sprintf_s(buffer, "select count(id) from [dbo].[tbl_ReservedGuildBattleInfo]");
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == 1)
      {
        unsigned int count = 0;
        ret = SQLGetData(m_hStmtSelect, 1u, 4, &count, 0, &strLenOrInd);
        if (ret == 100)
        {
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
          }
          return 4294967290;
        }

        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        if (m_bSaveDBLog)
        {
          FmtLog("%s Success", buffer);
        }
        // narrowing cast for thunk return parity
        return static_cast<int>(count);
      }

      unsigned int result = 0;
      if (ret == 100)
      {
        result = static_cast<unsigned int>(-4);
      }
      else
      {
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
        result = static_cast<unsigned int>(-5);
      }

      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      // narrowing cast for thunk return parity
      return static_cast<int>(result);
    }
    if (ret == 100)
    {
      return -2;
    }

    ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 4294967293;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  // narrowing cast for thunk return parity
  return static_cast<int>(static_cast<__int64>(-1));
}

int CRFWorldDatabase::SelectRowCountGuildBattleScheduleInfo()
{
  SQLLEN strLenOrInd = 0;
  char buffer[260]{};

  sprintf_s(buffer, "select count(id) from [dbo].[tbl_GuildBattleScheduleInfo]");
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == 1)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == 1)
      {
        unsigned int count = 0;
        ret = SQLGetData(m_hStmtSelect, 1u, 4, &count, 0, &strLenOrInd);
        if (ret == 100)
        {
          if (m_hStmtSelect)
          {
            SQLCloseCursor(m_hStmtSelect);
          }
          return 4294967291;
        }

        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        if (m_bSaveDBLog)
        {
          FmtLog("%s Success", buffer);
        }
        // narrowing cast for thunk return parity
        return static_cast<int>(count);
      }

      unsigned int result = 0;
      if (ret == 100)
      {
        result = static_cast<unsigned int>(-3);
      }
      else
      {
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
        result = static_cast<unsigned int>(-4);
      }

      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      // narrowing cast for thunk return parity
      return static_cast<int>(result);
    }
    if (ret == 100)
    {
      return -2;
    }

    ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 0;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  // narrowing cast for thunk return parity
  return static_cast<int>(static_cast<__int64>(-1));
}

bool CRFWorldDatabase::LoadGuildBattleInfo(
  unsigned int dwStartID,
  unsigned int dwRowCnt,
  _worlddb_guild_battle_info *pkInfo)
{
  if (!pkInfo)
  {
    return false;
  }

  SQLLEN strLenOrInd = 0;
  char buffer[1028]{};
  unsigned int count = 0;

  sprintf_s(buffer, "{ CALL pSelect_ReservedGuildBattleInfo(%u,%u) }", dwStartID, dwRowCnt + dwStartID);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == 1)
    {
      do
      {
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != 1)
        {
          break;
        }
        SQLGetData(m_hStmtSelect, 1u, 65518, &pkInfo->list[count], 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 2u, 65518, &pkInfo->list[count].dwP1GuildSerial, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 3u, 65518, &pkInfo->list[count].dwP2GuildSerial, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 4u, 65518, &pkInfo->list[count].dwMapID, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 5u, 65530, &pkInfo->list[count++].byNumber, 0, &strLenOrInd);
      }
      while (dwRowCnt > count);

      pkInfo->wCount = count;
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

    if (ret != 100)
    {
      ErrorMsgLog(ret, buffer, "SQLExecDirect", m_hStmtSelect);
      ErrorAction(ret, m_hStmtSelect);
    }
    return false;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return false;
}

unsigned __int8 CRFWorldDatabase::LoadGuildBattleScheduleInfo(
  unsigned int uiStartListID,
  unsigned int uiScheduleUnitCnt,
  _worlddb_guild_battle_schedule_list *pkInfo)
{
  if (!uiScheduleUnitCnt || !pkInfo)
  {
    return 0;
  }

  SQLLEN strLenOrInd = 0;
  char buffer[1028]{};

  sprintf_s(buffer, "{ CALL pSelect_ReservedGuildBattleSchedule( %u, %u ) }", uiStartListID, uiStartListID);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == 1)
    {
      unsigned int count = 0;
      pkInfo->wCount = 0;
      do
      {
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != 1)
        {
          break;
        }
        SQLGetData(m_hStmtSelect, 1u, 65518, &pkInfo->list[count], 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 2u, 65518, &pkInfo->list[count].dwSLID, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 3u, 65530, &pkInfo->list[count].ucState, 0, &strLenOrInd);

        unsigned __int16 targetValue[24]{};
        SQLGetData(m_hStmtSelect, 4u, 93, targetValue, 0, &strLenOrInd);
        SQLGetData(m_hStmtSelect, 5u, 65519, &pkInfo->list[count].wTumeMin, 0, &strLenOrInd);

        tm timeInfo{};
        timeInfo.tm_year = targetValue[0] - 1900;
        timeInfo.tm_mon = targetValue[1] - 1;
        timeInfo.tm_mday = targetValue[2];
        timeInfo.tm_hour = targetValue[3];
        timeInfo.tm_min = targetValue[4];
        timeInfo.tm_sec = targetValue[5];
        timeInfo.tm_isdst = -1;
        __time64_t timestamp = mktime_3(&timeInfo);
        if (timestamp == -1)
        {
          timestamp = 0;
        }
        pkInfo->list[count++].tStartTime = timestamp;
        pkInfo->wCount = count;
      }
      while (uiScheduleUnitCnt > count);

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

    if (ret == 100)
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

bool CRFWorldDatabase::DeleteGuildBattleInfo()
{
  char buffer[1040]{};
  sprintf_s(buffer, "truncate table tbl_ReservedGuildBattleInfo");
  return ExecUpdateQuery(buffer, 1);
}

bool CRFWorldDatabase::DeleteGuildBattleScheduleInfo()
{
  char buffer[1040]{};
  sprintf_s(buffer, "truncate table [dbo].[tbl_GuildBattleScheduleInfo]");
  return ExecUpdateQuery(buffer, 1);
}

bool CRFWorldDatabase::InsertGuildBattleDefaultRecord(unsigned int dwRowCnt)
{
  if (!dwRowCnt)
  {
    return false;
  }
  if (!DeleteGuildBattleInfo())
  {
    return false;
  }

  char buffer[1028]{};
  for (unsigned int j = 0; j < dwRowCnt; ++j)
  {
    sprintf_s(buffer, "{ CALL pInsert_GuildBattleInfo( %u, 0, 0, 0, 0 ) }", j);
    if (!ExecUpdateQuery(buffer, 1))
    {
      DeleteGuildBattleInfo();
      return false;
    }
  }
  return true;
}

bool CRFWorldDatabase::InsertGuildBattleScheduleDefaultRecord(
  unsigned int uiDayCnt,
  unsigned int uiMapCnt,
  unsigned __int8 byMaxHour,
  unsigned __int8 byUnitTimeCntPerTime)
{
  if (!uiDayCnt || !uiMapCnt || !byUnitTimeCntPerTime)
  {
    return false;
  }
  if (uiDayCnt > 0x1E || byMaxHour > 24)
  {
    return false;
  }
  if (!DeleteGuildBattleScheduleInfo())
  {
    return false;
  }

  char buffer[1028]{};
  const unsigned int unitPerDay = byUnitTimeCntPerTime * byMaxHour;
  for (unsigned int j = 0; j < uiDayCnt; ++j)
  {
    for (unsigned int k = 0; k < uiMapCnt; ++k)
    {
      const unsigned int slid = k + uiMapCnt * j;
      for (unsigned int m = 0; m < unitPerDay; ++m)
      {
        const unsigned int id = j * unitPerDay * uiMapCnt + m + k * unitPerDay;
        sprintf_s(
          buffer,
          "INSERT INTO [dbo].[tbl_GuildBattleScheduleInfo]( [ID], [SLID], [State], [StartTime], [BattleTurm] )VALUES( %d, %d, 0, 0, 0 )",
          id,
          slid);
        if (!ExecUpdateQuery(buffer, 1))
        {
          DeleteGuildBattleScheduleInfo();
          return false;
        }
      }
    }
  }
  return true;
}

unsigned __int8 CRFWorldDatabase::Select_CharNumInWorld(unsigned int dwAccountSerial,
        unsigned __int8 *byCharNum)
{
  SQLLEN indicator = 0;
  SQLRETURN sqlRet = SQL_ERROR;
  char query[260]{};

  sprintf_s(query, "{ CALL pSelect_CharNumInWorld( %d ) }", dwAccountSerial);
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }
  if (!this->m_hStmtSelect && !this->ReConnectDataBase())
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return 1;
  }

  sqlRet = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
  if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
  {
    sqlRet = SQLFetch(this->m_hStmtSelect);
    if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
    {
      sqlRet = SQLGetData(this->m_hStmtSelect, 1u, 65530, byCharNum, 0, &indicator);
      if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
      {
        if (this->m_hStmtSelect)
        {
          SQLCloseCursor(this->m_hStmtSelect);
        }
        if (this->m_bSaveDBLog)
        {
          this->FmtLog("%s Success", query);
        }
        return 0;
      }

      unsigned __int8 result = 0;
      if (sqlRet == SQL_NO_DATA)
      {
        result = 2;
      }
      else
      {
        this->ErrorMsgLog(sqlRet, query, "SQLExecDirectA", this->m_hStmtSelect);
        this->ErrorAction(sqlRet, this->m_hStmtSelect);
        result = 1;
      }
      if (this->m_hStmtSelect)
      {
        SQLCloseCursor(this->m_hStmtSelect);
      }
      return result;
    }

    unsigned __int8 result = 0;
    if (sqlRet == SQL_NO_DATA)
    {
      result = 2;
    }
    else
    {
      this->ErrorMsgLog(sqlRet, query, "SQLExecDirectA", this->m_hStmtSelect);
      this->ErrorAction(sqlRet, this->m_hStmtSelect);
      result = 1;
    }
    if (this->m_hStmtSelect)
    {
      SQLCloseCursor(this->m_hStmtSelect);
    }
    return result;
  }

  if (sqlRet == SQL_NO_DATA)
  {
    return 2;
  }

  this->ErrorMsgLog(sqlRet, query, "SQLExecDirectA", this->m_hStmtSelect);
  this->ErrorAction(sqlRet, this->m_hStmtSelect);
  return 1;
}

unsigned __int8 CRFWorldDatabase::Select_Exist_Economy(
  unsigned int dwDate,
  _worlddb_economy_history_info *pEconomyData)
{
  SQLLEN indicator = 0;
  SQLRETURN sqlRet = SQL_ERROR;
  char query[260]{};

  sprintf_s(query, "{ CALL pSelect_Exist_Economy( %d ) }", dwDate);
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }
  if (!this->m_hStmtSelect && !this->ReConnectDataBase())
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return 1;
  }

  sqlRet = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
  if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
  {
    sqlRet = SQLFetch(this->m_hStmtSelect);
    if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
    {
      sqlRet = SQLGetData(this->m_hStmtSelect, 1u, 8, pEconomyData->dTradeDalant, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 2u, 8, &pEconomyData->dTradeDalant[1], 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 3u, 8, &pEconomyData->dTradeDalant[2], 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 4u, 8, pEconomyData, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 5u, 8, &pEconomyData->dTradeGold[1], 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 6u, 8, &pEconomyData->dTradeGold[2], 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 7u, 4, &pEconomyData->dwManageValue, 0, &indicator);

      SQLUSMALLINT columnNumber = 8;
      for (int row = 0; row < 3; ++row)
      {
        for (int column = 0; column < 3; ++column)
        {
          sqlRet = SQLGetData(this->m_hStmtSelect, columnNumber++, 8, &pEconomyData->dMineOre[column][row], 0, &indicator);
          if (sqlRet && sqlRet != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
        }
      }

      for (int row = 0; row < 3; ++row)
      {
        for (int column = 0; column < 3; ++column)
        {
          sqlRet = SQLGetData(this->m_hStmtSelect, columnNumber++, 8, &pEconomyData->dCutOre[column][row], 0, &indicator);
          if (sqlRet && sqlRet != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
        }
      }

      if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
      {
        if (this->m_hStmtSelect)
        {
          SQLCloseCursor(this->m_hStmtSelect);
        }
        if (this->m_bSaveDBLog)
        {
          this->FmtLog("%s Success", query);
        }
        return 0;
      }

      this->ErrorMsgLog(sqlRet, query, "SQLGetData", this->m_hStmtSelect);
      this->ErrorAction(sqlRet, this->m_hStmtSelect);
      if (this->m_hStmtSelect)
      {
        SQLCloseCursor(this->m_hStmtSelect);
      }
      return 1;
    }

    unsigned __int8 result = 0;
    if (sqlRet == SQL_NO_DATA)
    {
      result = 2;
    }
    else
    {
      this->ErrorMsgLog(sqlRet, query, "SQLFetch", this->m_hStmtSelect);
      this->ErrorAction(sqlRet, this->m_hStmtSelect);
      result = 1;
    }
    if (this->m_hStmtSelect)
    {
      SQLCloseCursor(this->m_hStmtSelect);
    }
    return result;
  }

  if (sqlRet == SQL_NO_DATA)
  {
    return 2;
  }

  this->ErrorMsgLog(sqlRet, query, "SQLExecDirectA", this->m_hStmtSelect);
  this->ErrorAction(sqlRet, this->m_hStmtSelect);
  return 1;
}

unsigned __int8 CRFWorldDatabase::Select_Limit_Run_Record()
{
  char query[1024]{};
  unsigned __int8 result = 0;
  unsigned int targetValue[8]{};
  unsigned __int16 columnNumber[14]{};

  sprintf_s(query, sizeof(query), "select [aserial] from [dbo].[tbl_sf_delay] where [aserial] = 0");
  result = this->SQLExecDirect_RetErrCode(query);
  if (result)
  {
    return result;
  }
  result = this->SQLFetch_RetErrCode(query);
  if (result)
  {
    return result;
  }
  targetValue[0] = 0;
  columnNumber[0] = 1;
  result = this->SQLGetData_RetErrCode(query, columnNumber, 4, targetValue);
  if (result)
  {
    return result;
  }
  this->SelectCleanUp(query);
  return 0;
}

unsigned __int8 CRFWorldDatabase::Select_MacroData(unsigned int dwSerial,
        _AIOC_A_MACRODATA *pMacro)
{
  SQLLEN indicator = 0;
  SQLRETURN sqlRet = SQL_ERROR;
  char query[260]{};

  sprintf_s(
    query,
    "Select hp, fp, sp, action0, action1, action2, action3, action4, action5, action6, action7, action8, action9, chat0, "
    "chat1, chat2, chat3, chat4, hpvalue, fpvalue, spvalue from tbl_macro where serial=%d order by belt asc",
    dwSerial);

  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }

  if (!this->m_hStmtSelect && !this->ReConnectDataBase())
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return 1;
  }

  sqlRet = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
  if (sqlRet && sqlRet != SQL_SUCCESS_WITH_INFO)
  {
    if (sqlRet == SQL_NO_DATA)
    {
      return 2;
    }

    this->ErrorMsgLog(sqlRet, query, "_SQLExecDirect", this->m_hStmtSelect);
    this->ErrorAction(sqlRet, this->m_hStmtSelect);
    return 1;
  }

  int fetchedRowCount = 0;
  for (;; ++fetchedRowCount)
  {
    unsigned int potionKinds[3]{};
    unsigned int actionSlots[10]{};
    char chatColumns[5][256]{};
    unsigned int potionValues[3]{};

    sqlRet = SQLFetch(this->m_hStmtSelect);
    if (sqlRet && sqlRet != SQL_SUCCESS_WITH_INFO)
    {
      break;
    }

    sqlRet = SQLGetData(this->m_hStmtSelect, 1u, 4, &potionKinds[0], 0, &indicator);
    sqlRet = SQLGetData(this->m_hStmtSelect, 2u, 4, &potionKinds[1], 0, &indicator);
    sqlRet = SQLGetData(this->m_hStmtSelect, 3u, 4, &potionKinds[2], 0, &indicator);

    for (unsigned int actionIndex = 0; actionIndex < 10; ++actionIndex)
    {
      sqlRet = SQLGetData(this->m_hStmtSelect, 4u + actionIndex, 4, &actionSlots[actionIndex], 0, &indicator);
    }

    for (unsigned int chatIndex = 0; chatIndex < 5; ++chatIndex)
    {
      sqlRet = SQLGetData(this->m_hStmtSelect, 14u + chatIndex, 1, chatColumns[chatIndex], 255, &indicator);
    }

    sqlRet = SQLGetData(this->m_hStmtSelect, 19u, 4, &potionValues[0], 0, &indicator);
    sqlRet = SQLGetData(this->m_hStmtSelect, 20u, 4, &potionValues[1], 0, &indicator);
    sqlRet = SQLGetData(this->m_hStmtSelect, 21u, 4, &potionValues[2], 0, &indicator);

    if (fetchedRowCount < 1)
    {
      for (unsigned int potionIndex = 0; potionIndex < 3; ++potionIndex)
      {
        pMacro->mcr_Potion[fetchedRowCount].Potion[potionIndex] = potionKinds[potionIndex];
        pMacro->mcr_Potion[fetchedRowCount].PotionValue[potionIndex] = potionValues[potionIndex];
      }
    }

    if (fetchedRowCount < 3)
    {
      for (unsigned int actionIndex = 0; actionIndex < 10; ++actionIndex)
      {
        pMacro->mcr_Action[fetchedRowCount].Action[actionIndex] = actionSlots[actionIndex];
      }
    }

    if (fetchedRowCount < 2)
    {
      for (unsigned int chatIndex = 0; chatIndex < 5; ++chatIndex)
      {
        std::strcpy(pMacro->mcr_Chat[fetchedRowCount].Chat[chatIndex], chatColumns[chatIndex]);
      }
    }
  }

  if (!fetchedRowCount)
  {
    if (this->m_hStmtSelect)
    {
      SQLCloseCursor(this->m_hStmtSelect);
    }
    return 2;
  }

  if (sqlRet == SQL_SUCCESS_WITH_INFO || (fetchedRowCount == 3 && sqlRet == SQL_NO_DATA))
  {
    if (this->m_hStmtSelect)
    {
      SQLCloseCursor(this->m_hStmtSelect);
    }
    if (this->m_bSaveDBLog)
    {
      this->FmtLog("%s Success", query);
    }
    return 0;
  }

  this->ErrorMsgLog(sqlRet, query, "SQLGetData", this->m_hStmtSelect);
  this->ErrorAction(sqlRet, this->m_hStmtSelect);
  if (this->m_hStmtSelect)
  {
    SQLCloseCursor(this->m_hStmtSelect);
  }
  return 1;
}

int CRFWorldDatabase::Select_PatriarchComm(unsigned int dwSerial,
        _patriarch_comm_list *pOutList)
{
  SQLLEN indicator = 0;
  SQLRETURN sqlRet = SQL_ERROR;
  char today[56]{};
  char query[144]{};

  GetTodayStr(today);
  sprintf_s(query, "{ CALL pSelect_PatriarchComm( %d, '%s' ) }", dwSerial, today);
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }
  if (!this->m_hStmtSelect && !this->ReConnectDataBase())
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return 1;
  }

  sqlRet = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
  if (sqlRet && sqlRet != SQL_SUCCESS_WITH_INFO)
  {
    if (sqlRet == SQL_NO_DATA)
    {
      return 2;
    }
    this->ErrorMsgLog(sqlRet, query, "SQLExecDirectA", this->m_hStmtSelect);
    this->ErrorAction(sqlRet, this->m_hStmtSelect);
    return 1;
  }

  while (true)
  {
    sqlRet = SQLFetch(this->m_hStmtSelect);
    if (sqlRet && sqlRet != SQL_SUCCESS_WITH_INFO)
    {
      if (sqlRet != SQL_NO_DATA)
      {
        this->ErrorMsgLog(sqlRet, query, "SQLFetch", this->m_hStmtSelect);
        this->ErrorAction(sqlRet, this->m_hStmtSelect);
        if (this->m_hStmtSelect)
        {
          SQLCloseCursor(this->m_hStmtSelect);
        }
        return 1;
      }
      break;
    }

    sqlRet = SQLGetData(this->m_hStmtSelect, 1u, 4, &pOutList->List[pOutList->dwCount], 0, &indicator);
    sqlRet = SQLGetData(this->m_hStmtSelect, 2u, 1, pOutList->List[pOutList->dwCount].pszDepDate, 9, &indicator);
    if (sqlRet && sqlRet != SQL_SUCCESS_WITH_INFO)
    {
      break;
    }

    ++pOutList->dwCount;
  }

  if (sqlRet == SQL_NO_DATA)
  {
    if (this->m_hStmtSelect)
    {
      SQLCloseCursor(this->m_hStmtSelect);
    }
    if (this->m_bSaveDBLog)
    {
      this->FmtLog("%s Success", query);
    }
    return 0;
  }

  this->ErrorMsgLog(sqlRet, query, "SQLFetch", this->m_hStmtSelect);
  this->ErrorAction(sqlRet, this->m_hStmtSelect);
  if (this->m_hStmtSelect)
  {
    SQLCloseCursor(this->m_hStmtSelect);
  }
  return 1;
}

unsigned __int8 CRFWorldDatabase::Select_PostContent(unsigned int dwPostSerial,
        char *wszContent,
        int nSize)
{
  SQLLEN indicator = 0;
  SQLRETURN sqlRet = SQL_ERROR;
  char query[132]{};

  sprintf_s(query, 128, "select content from tbl_PostStorage where serial=%d", dwPostSerial);
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }
  if (!this->m_hStmtSelect && !this->ReConnectDataBase())
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return 1;
  }

  sqlRet = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
  if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
  {
    sqlRet = SQLFetch(this->m_hStmtSelect);
    if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
    {
      sqlRet = SQLGetData(this->m_hStmtSelect, 1u, 1, wszContent, nSize, &indicator);
      if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
      {
        if (this->m_hStmtSelect)
        {
          SQLCloseCursor(this->m_hStmtSelect);
        }
        if (this->m_bSaveDBLog)
        {
          this->FmtLog("%s Success", query);
        }
        return 0;
      }

      this->ErrorMsgLog(sqlRet, query, "SQLGetData", this->m_hStmtSelect);
      this->ErrorAction(sqlRet, this->m_hStmtSelect);
      if (this->m_hStmtSelect)
      {
        SQLCloseCursor(this->m_hStmtSelect);
      }
      return 1;
    }

    unsigned __int8 result = 0;
    if (sqlRet == SQL_NO_DATA)
    {
      result = 2;
    }
    else
    {
      this->ErrorMsgLog(sqlRet, query, "SQLFetch", this->m_hStmtSelect);
      this->ErrorAction(sqlRet, this->m_hStmtSelect);
      result = 1;
    }
    if (this->m_hStmtSelect)
    {
      SQLCloseCursor(this->m_hStmtSelect);
    }
    return result;
  }

  if (sqlRet == SQL_NO_DATA)
  {
    return 2;
  }

  this->ErrorMsgLog(sqlRet, query, "SQLExecDirectA", this->m_hStmtSelect);
  this->ErrorAction(sqlRet, this->m_hStmtSelect);
  return 1;
}

unsigned __int8 CRFWorldDatabase::Select_PostStorageList(unsigned int dwOwner,
        _post_storage_list *pListData)
{
  SQLLEN indicator = 0;
  SQLRETURN sqlRet = SQL_ERROR;
  char query[528]{};

  pListData->dwCount = 0;
  sprintf_s(
    query,
    512,
    "select top %d postinx,serial,poststate,sendname,title,k,d,u,gold,uid,sindex from tbl_PostStorage where owner=%d and "
    "poststate<%d and dck=0",
    50,
    dwOwner,
    100);
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }
  if (!this->m_hStmtSelect && !this->ReConnectDataBase())
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return 1;
  }

  sqlRet = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
  if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
  {
    while (true)
    {
      sqlRet = SQLFetch(this->m_hStmtSelect);
      if (sqlRet && sqlRet != SQL_SUCCESS_WITH_INFO)
      {
        break;
      }

      sqlRet = SQLGetData(this->m_hStmtSelect, 1u, 65530, &pListData->List[pListData->dwCount].byIndex, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 2u, 65518, &pListData->List[pListData->dwCount], 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 3u, 65530, &pListData->List[pListData->dwCount].byState, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 4u, 1, pListData->List[pListData->dwCount].wszSendName, 17, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 5u, 1, pListData->List[pListData->dwCount].wszTitle, 21, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 6u, 4, &pListData->List[pListData->dwCount].nK, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 7u, 65511, &pListData->List[pListData->dwCount].dwDur, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 8u, 4, &pListData->List[pListData->dwCount].dwUpt, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 9u, 65518, &pListData->List[pListData->dwCount].dwGold, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 10, 65511, &pListData->List[pListData->dwCount].lnUID, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 11, 5, &pListData->List[pListData->dwCount].wStorageIndex, 0, &indicator);
      ++pListData->dwCount;
    }

    if (this->m_hStmtSelect)
    {
      SQLCloseCursor(this->m_hStmtSelect);
    }
    if (this->m_bSaveDBLog)
    {
      this->FmtLog("%s Success", query);
    }
    return 0;
  }

  if (sqlRet == SQL_NO_DATA)
  {
    return 2;
  }

  this->ErrorMsgLog(sqlRet, query, "SQLExecDirectA", this->m_hStmtSelect);
  this->ErrorAction(sqlRet, this->m_hStmtSelect);
  return 1;
}

unsigned __int8 CRFWorldDatabase::Select_PvpRankInfo(unsigned __int8 byRace,
        char *szDate,
        _PVP_RANK_DATA *rankData)
{
  SQLLEN indicator = 0;
  SQLRETURN sqlRet = SQL_ERROR;
  char query[260]{};

  int rankIndex = 0;
  sprintf_s(
    query,
    "select top 100 Rank,Lv,Rate,Grade,PvpPoint,Name,GuildName,serial,guildserial from tbl_PvpRank%s where race = %d orde"
    "r by grade desc, rate ",
    szDate,
    byRace);
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }
  if (!this->m_hStmtSelect && !this->ReConnectDataBase())
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return 1;
  }

  sqlRet = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
  if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
  {
    do
    {
      sqlRet = SQLFetch(this->m_hStmtSelect);
      if (sqlRet && sqlRet != SQL_SUCCESS_WITH_INFO)
      {
        break;
      }
      sqlRet = SQLGetData(this->m_hStmtSelect, 1u, 65508, &rankData[rankIndex], 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 2u, 65508, &rankData[rankIndex].byLv, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 3u, 65519, &rankData[rankIndex].wRate, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 4u, 65508, &rankData[rankIndex].byGrade, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 5u, 8, &rankData[rankIndex].dPvpPoint, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 6u, 1, rankData[rankIndex].wszName, 17, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 7u, 1, rankData[rankIndex].wszGuildName, 17, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 8u, 4, &rankData[rankIndex].dwAvatorSerial, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 9u, 4, &rankData[rankIndex++].dwGuildSerial, 0, &indicator);
      if (rankIndex >= 100)
      {
        break;
      }
    } while (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO);

    if (this->m_hStmtSelect)
    {
      SQLCloseCursor(this->m_hStmtSelect);
    }
    if (this->m_bSaveDBLog)
    {
      this->FmtLog("%s Success", query);
    }
    return 0;
  }

  if (sqlRet == SQL_NO_DATA)
  {
    return 2;
  }

  this->ErrorMsgLog(sqlRet, query, "SQLExecDirectA", this->m_hStmtSelect);
  this->ErrorAction(sqlRet, this->m_hStmtSelect);
  return 1;
}

unsigned __int8 CRFWorldDatabase::Select_RegeAvator_For_Lobby_Logout(unsigned int dwAccountSerial,
        _rege_char_data *pRegeCharData)
{
  char query[1024]{};
  SQLLEN indicator = 0;
  SQLRETURN sqlRet = SQL_ERROR;

  sprintf_s(
    query,
    sizeof(query),
    "select Serial,Name,Slot,Lv,Dalant,Gold from tbl_base where AccountSerial=%d and DCK=0",
    dwAccountSerial);
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }
  if (!this->m_hStmtSelect && !this->ReConnectDataBase())
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return 1;
  }

  sqlRet = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
  if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
  {
    int charCount = 0;
    while (true)
    {
      sqlRet = SQLFetch(this->m_hStmtSelect);
      if (sqlRet && sqlRet != SQL_SUCCESS_WITH_INFO)
      {
        break;
      }
      sqlRet = SQLGetData(this->m_hStmtSelect, 1u, 65518, &pRegeCharData->RegeList[charCount].dwCharSerial, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 2u, 1, pRegeCharData->RegeList[charCount].szCharName, 17, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 3u, 65519, &pRegeCharData->RegeList[charCount], 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 4u, 4, &pRegeCharData->RegeList[charCount].nLevel, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 5u, 4, &pRegeCharData->RegeList[charCount].dwDalant, 0, &indicator);
      sqlRet = SQLGetData(this->m_hStmtSelect, 6u, 4, &pRegeCharData->RegeList[charCount++].dwGold, 0, &indicator);
      if (charCount >= 3)
      {
        break;
      }
      if (sqlRet && sqlRet != SQL_SUCCESS_WITH_INFO)
      {
        if (sqlRet != SQL_NO_DATA)
        {
          this->ErrorMsgLog(sqlRet, query, "SQLExecDirectA", this->m_hStmtSelect);
          this->ErrorAction(sqlRet, this->m_hStmtSelect);
          if (this->m_hStmtSelect)
          {
            SQLCloseCursor(this->m_hStmtSelect);
          }
          return 1;
        }
        break;
      }
    }

    pRegeCharData->nCharNum = charCount;
    if (this->m_hStmtSelect)
    {
      SQLCloseCursor(this->m_hStmtSelect);
    }
    if (this->m_bSaveDBLog)
    {
      this->FmtLog("%s Success", query);
    }
    return 0;
  }

  if (sqlRet == SQL_NO_DATA)
  {
    return 2;
  }

  this->ErrorMsgLog(sqlRet, query, "SQLExecDirectA", this->m_hStmtSelect);
  this->ErrorAction(sqlRet, this->m_hStmtSelect);
  return 1;
}

unsigned __int8 CRFWorldDatabase::Select_ReturnPost(unsigned int dwMax,
        unsigned int dwMasterSerial,
        _return_post_list *pRetData)
{
  SQLLEN indicator = 0;
  SQLRETURN sqlRet = SQL_ERROR;
  unsigned int targetValue[11]{};
  char query[516]{};

  pRetData->dwCount = 0;
  pRetData->bContinue = 0;
  sprintf_s(
    query,
    512,
    "select count(serial) from tbl_PostStorage where owner=%d and poststate=%d and dck=0",
    dwMasterSerial,
    100);
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }
  if (!this->m_hStmtSelect && !this->ReConnectDataBase())
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return 1;
  }

  sqlRet = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
  if (sqlRet && sqlRet != SQL_SUCCESS_WITH_INFO)
  {
    if (sqlRet == SQL_NO_DATA)
    {
      return 2;
    }
    this->ErrorMsgLog(sqlRet, query, "SQLExecDirectA", this->m_hStmtSelect);
    this->ErrorAction(sqlRet, this->m_hStmtSelect);
    return 1;
  }

  sqlRet = SQLFetch(this->m_hStmtSelect);
  if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
  {
    sqlRet = SQLGetData(this->m_hStmtSelect, 1u, 65518, targetValue, 0, &indicator);
    if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
    {
      if (this->m_hStmtSelect)
      {
        SQLCloseCursor(this->m_hStmtSelect);
      }
      if (this->m_bSaveDBLog)
      {
        this->FmtLog("%s Success", query);
      }

      if (targetValue[0])
      {
        sprintf_s(
          query,
          512,
          "select top %d serial,poststate,recvname,title,content,k,d,u,gold,err,uid from tbl_PostStorage where owner=%d a"
          "nd poststate=%d and dck=0",
          dwMax,
          dwMasterSerial,
          100);
        if (this->m_bSaveDBLog)
        {
          this->Log(query);
        }
        if (!this->m_hStmtSelect && !this->ReConnectDataBase())
        {
          this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
          return 1;
        }

        sqlRet = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
        if (sqlRet && sqlRet != SQL_SUCCESS_WITH_INFO)
        {
          if (sqlRet == SQL_NO_DATA)
          {
            return 2;
          }
          this->ErrorMsgLog(sqlRet, query, "SQLExecDirectA", this->m_hStmtSelect);
          this->ErrorAction(sqlRet, this->m_hStmtSelect);
          return 1;
        }

        while (true)
        {
          sqlRet = SQLFetch(this->m_hStmtSelect);
          if (sqlRet && sqlRet != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }

          sqlRet = SQLGetData(this->m_hStmtSelect, 1u, 65518, &pRetData->List[pRetData->dwCount], 0, &indicator);
          sqlRet = SQLGetData(this->m_hStmtSelect, 2u, 65530, &pRetData->List[pRetData->dwCount].byState, 0, &indicator);
          sqlRet = SQLGetData(this->m_hStmtSelect, 3u, 1, pRetData->List[pRetData->dwCount].wszRecvName, 17, &indicator);
          sqlRet = SQLGetData(this->m_hStmtSelect, 4u, 1, pRetData->List[pRetData->dwCount].wszTitle, 21, &indicator);
          sqlRet = SQLGetData(this->m_hStmtSelect, 5u, 1, pRetData->List[pRetData->dwCount].wszContent, 201, &indicator);
          sqlRet = SQLGetData(this->m_hStmtSelect, 6u, 4, &pRetData->List[pRetData->dwCount].nK, 0, &indicator);
          sqlRet = SQLGetData(this->m_hStmtSelect, 7u, 65511, &pRetData->List[pRetData->dwCount].dwDur, 0, &indicator);
          sqlRet = SQLGetData(this->m_hStmtSelect, 8u, 65518, &pRetData->List[pRetData->dwCount].dwUpt, 0, &indicator);
          sqlRet = SQLGetData(this->m_hStmtSelect, 9u, 65518, &pRetData->List[pRetData->dwCount].dwGold, 0, &indicator);
          sqlRet = SQLGetData(this->m_hStmtSelect, 10, 65530, &pRetData->List[pRetData->dwCount].byErr, 0, &indicator);
          sqlRet = SQLGetData(this->m_hStmtSelect, 11, 65511, &pRetData->List[pRetData->dwCount].lnUID, 0, &indicator);
          ++pRetData->dwCount;
        }

        if (this->m_hStmtSelect)
        {
          SQLCloseCursor(this->m_hStmtSelect);
        }
        if (this->m_bSaveDBLog)
        {
          this->FmtLog("%s Success", query);
        }
      }
      return 0;
    }
    this->ErrorMsgLog(sqlRet, query, "SQLGetData", this->m_hStmtSelect);
    this->ErrorAction(sqlRet, this->m_hStmtSelect);
    if (this->m_hStmtSelect)
    {
      SQLCloseCursor(this->m_hStmtSelect);
    }
    return 1;
  }

  unsigned __int8 result = 0;
  if (sqlRet == SQL_NO_DATA)
  {
    result = 2;
  }
  else
  {
    this->ErrorMsgLog(sqlRet, query, "SQLFetch", this->m_hStmtSelect);
    this->ErrorAction(sqlRet, this->m_hStmtSelect);
    result = 1;
  }
  if (this->m_hStmtSelect)
  {
    SQLCloseCursor(this->m_hStmtSelect);
  }
  return result;
}

int CRFWorldDatabase::Select_RFEvent_ClassRefine(unsigned int dwAvatorSerial,
        unsigned __int8 *byRefinedCnt,
        unsigned int *dwRefineDate)
{
  SQLLEN indicator = 0;
  SQLRETURN sqlRet = SQL_ERROR;
  char query[256]{};

  sprintf_s(
    query,
    sizeof(query),
    "select ClassRefineCnt,ClassRefineDate from [dbo].[tbl_event] where avatorserial = %d",
    dwAvatorSerial);
  if (this->m_bSaveDBLog)
  {
    this->Log(query);
  }
  if (!this->m_hStmtSelect && !this->ReConnectDataBase())
  {
    this->ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return 1;
  }

  sqlRet = SQLExecDirectA(this->m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), SQL_NTS);
  if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
  {
    sqlRet = SQLFetch(this->m_hStmtSelect);
    if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
    {
      sqlRet = SQLGetData(this->m_hStmtSelect, 1u, 65530, byRefinedCnt, 0, &indicator);
      if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
      {
        sqlRet = SQLGetData(this->m_hStmtSelect, 2u, 4, dwRefineDate, 0, &indicator);
        if (!sqlRet || sqlRet == SQL_SUCCESS_WITH_INFO)
        {
          if (this->m_hStmtSelect)
          {
            SQLCloseCursor(this->m_hStmtSelect);
          }
          if (this->m_bSaveDBLog)
          {
            this->FmtLog("%s Success", query);
          }
          return 0;
        }

        unsigned __int8 result = 0;
        if (sqlRet == SQL_NO_DATA)
        {
          result = 2;
        }
        else
        {
          this->ErrorMsgLog(sqlRet, query, "SQLGetData", this->m_hStmtSelect);
          this->ErrorAction(sqlRet, this->m_hStmtSelect);
          result = 1;
        }
        if (this->m_hStmtSelect)
        {
          SQLCloseCursor(this->m_hStmtSelect);
        }
        // narrowing cast for thunk return parity
        return static_cast<int>(result);
      }

      unsigned __int8 result = 0;
      if (sqlRet == SQL_NO_DATA)
      {
        result = 2;
      }
      else
      {
        this->ErrorMsgLog(sqlRet, query, "SQLGetData", this->m_hStmtSelect);
        this->ErrorAction(sqlRet, this->m_hStmtSelect);
        result = 1;
      }
      if (this->m_hStmtSelect)
      {
        SQLCloseCursor(this->m_hStmtSelect);
      }
      // narrowing cast for thunk return parity
      return static_cast<int>(result);
    }

    unsigned __int8 result = 0;
    if (sqlRet == SQL_NO_DATA)
    {
      result = 2;
    }
    else
    {
      this->ErrorMsgLog(sqlRet, query, "SQLExecDirect", this->m_hStmtSelect);
      this->ErrorAction(sqlRet, this->m_hStmtSelect);
      result = 1;
    }
    if (this->m_hStmtSelect)
    {
      SQLCloseCursor(this->m_hStmtSelect);
    }
    // narrowing cast for thunk return parity
    return static_cast<int>(result);
  }

  if (sqlRet == SQL_NO_DATA)
  {
    return 2;
  }

  this->ErrorMsgLog(sqlRet, query, "SQLExecDirect", this->m_hStmtSelect);
  this->ErrorAction(sqlRet, this->m_hStmtSelect);
  return 1;
}

