#include "pch.h"

#include "CRFWorldDatabase.h"
#include "DqsDbStructs.h"

#include <sql.h>
#include <sqlext.h>
#include <vector>

char CRFWorldDatabase::Update_CharacterData(unsigned int dwSerial, _worlddb_update_char_query *pUpdateQuery)
{
  SetAutoCommitMode(0);
  if (m_bSaveDBLog)
  {
    FmtLog("Update_CharacterData : %d", dwSerial);
  }
  if (!m_hStmtUpdate && !ReConnectDataBase())
  {
    ErrLog("ReConnectDataBase Fail. Query : Update_CharacterData");
    SetAutoCommitMode(1);
    return 0;
  }

  if (*pUpdateQuery->szBaseQuery)
  {
    if (!m_hStmtUpdate)
    {
      ErrLog("m_hStmtUpdate == SQL_NULL_HSTMT : Update_CharacterData");
      ReConnectDataBase();
      SetAutoCommitMode(1);
      return 0;
    }
    SQLRETURN ret = SQLExecDirectA(m_hStmtUpdate, reinterpret_cast<SQLCHAR *>(pUpdateQuery->szBaseQuery), SQL_NTS);
    if (ret && ret != SQL_SUCCESS_WITH_INFO)
    {
      ErrorMsgLog( ret, pUpdateQuery->szBaseQuery, "SQLExecDirectA", m_hStmtUpdate);
      RollbackTransaction();
      SetAutoCommitMode(1);
      ErrorAction( ret, m_hStmtUpdate);
      return 0;
    }
  }

  if (*pUpdateQuery->szGeneralQuery)
  {
    if (!m_hStmtUpdate)
    {
      ErrLog("m_hStmtUpdate == SQL_NULL_HSTMT : Update_CharacterData");
      ReConnectDataBase();
      SetAutoCommitMode(1);
      return 0;
    }
    SQLRETURN ret = SQLExecDirectA(m_hStmtUpdate, reinterpret_cast<SQLCHAR *>(pUpdateQuery->szGeneralQuery), SQL_NTS);
    if (ret && ret != SQL_SUCCESS_WITH_INFO)
    {
      ErrorMsgLog( ret, pUpdateQuery->szGeneralQuery, "SQLExecDirectA", m_hStmtUpdate);
      RollbackTransaction();
      SetAutoCommitMode(1);
      ErrorAction( ret, m_hStmtUpdate);
      return 0;
    }
  }

  if (*pUpdateQuery->szSupplementQuery)
  {
    if (!m_hStmtUpdate)
    {
      ErrLog("m_hStmtUpdate == SQL_NULL_HSTMT : Update_CharacterData");
      ReConnectDataBase();
      SetAutoCommitMode(1);
      return 0;
    }
    SQLRETURN ret = SQLExecDirectA(m_hStmtUpdate, reinterpret_cast<SQLCHAR *>(pUpdateQuery->szSupplementQuery), SQL_NTS);
    if (ret && ret != SQL_SUCCESS_WITH_INFO)
    {
      ErrorMsgLog( ret, pUpdateQuery->szSupplementQuery, "SQLExecDirectA", m_hStmtUpdate);
      RollbackTransaction();
      SetAutoCommitMode(1);
      ErrorAction( ret, m_hStmtUpdate);
      return 0;
    }
  }

  if (*pUpdateQuery->szInvenQuery)
  {
    if (!m_hStmtUpdate)
    {
      ErrLog("m_hStmtUpdate == SQL_NULL_HSTMT : Update_CharacterData");
      ReConnectDataBase();
      SetAutoCommitMode(1);
      return 0;
    }
    SQLRETURN ret = SQLExecDirectA(m_hStmtUpdate, reinterpret_cast<SQLCHAR *>(pUpdateQuery->szInvenQuery), SQL_NTS);
    if (ret && ret != SQL_SUCCESS_WITH_INFO)
    {
      ErrorMsgLog( ret, pUpdateQuery->szInvenQuery, "SQLExecDirectA", m_hStmtUpdate);
      RollbackTransaction();
      SetAutoCommitMode(1);
      ErrorAction( ret, m_hStmtUpdate);
      return 0;
    }
  }

  if (*pUpdateQuery->szUnitQuery)
  {
    if (!m_hStmtUpdate)
    {
      ErrLog("m_hStmtUpdate == SQL_NULL_HSTMT : Update_CharacterData");
      ReConnectDataBase();
      SetAutoCommitMode(1);
      return 0;
    }
    SQLRETURN ret = SQLExecDirectA(m_hStmtUpdate, reinterpret_cast<SQLCHAR *>(pUpdateQuery->szUnitQuery), SQL_NTS);
    if (ret && ret != SQL_SUCCESS_WITH_INFO)
    {
      ErrorMsgLog( ret, pUpdateQuery->szUnitQuery, "SQLExecDirectA", m_hStmtUpdate);
      RollbackTransaction();
      SetAutoCommitMode(1);
      ErrorAction( ret, m_hStmtUpdate);
      return 0;
    }
  }

  if (*pUpdateQuery->szUIQuery)
  {
    if (!m_hStmtUpdate)
    {
      ErrLog("m_hStmtUpdate == SQL_NULL_HSTMT : Update_CharacterData");
      ReConnectDataBase();
      SetAutoCommitMode(1);
      return 0;
    }
    SQLRETURN ret = SQLExecDirectA(m_hStmtUpdate, reinterpret_cast<SQLCHAR *>(pUpdateQuery->szUIQuery), SQL_NTS);
    if (ret && ret != SQL_SUCCESS_WITH_INFO)
    {
      ErrorMsgLog( ret, pUpdateQuery->szUIQuery, "SQLExecDirectA", m_hStmtUpdate);
      RollbackTransaction();
      SetAutoCommitMode(1);
      ErrorAction( ret, m_hStmtUpdate);
      return 0;
    }
  }

  if (*pUpdateQuery->szQuestQuery)
  {
    if (!m_hStmtUpdate)
    {
      ErrLog("m_hStmtUpdate == SQL_NULL_HSTMT : Update_CharacterData");
      ReConnectDataBase();
      SetAutoCommitMode(1);
      return 0;
    }
    SQLRETURN ret = SQLExecDirectA(m_hStmtUpdate, reinterpret_cast<SQLCHAR *>(pUpdateQuery->szQuestQuery), SQL_NTS);
    if (ret && ret != SQL_SUCCESS_WITH_INFO)
    {
      ErrorMsgLog( ret, pUpdateQuery->szQuestQuery, "SQLExecDirectA", m_hStmtUpdate);
      RollbackTransaction();
      SetAutoCommitMode(1);
      ErrorAction( ret, m_hStmtUpdate);
      return 0;
    }
  }

  if (*pUpdateQuery->wszBuddyQuery)
  {
    if (!m_hStmtUpdate)
    {
      ErrLog("m_hStmtUpdate == SQL_NULL_HSTMT : Update_CharacterData");
      ReConnectDataBase();
      SetAutoCommitMode(1);
      return 0;
    }
    SQLRETURN ret = SQLExecDirectA(m_hStmtUpdate, reinterpret_cast<SQLCHAR *>(pUpdateQuery->wszBuddyQuery), SQL_NTS);
    if (ret && ret != SQL_SUCCESS_WITH_INFO)
    {
      ErrorMsgLog( ret, pUpdateQuery->wszBuddyQuery, "_SQLExecDirect", m_hStmtUpdate);
      RollbackTransaction();
      SetAutoCommitMode(1);
      ErrorAction( ret, m_hStmtUpdate);
      return 0;
    }
  }

  if (*pUpdateQuery->wszTrunkQuery)
  {
    if (!m_hStmtUpdate)
    {
      ErrLog("m_hStmtUpdate == SQL_NULL_HSTMT : Update_CharacterData");
      ReConnectDataBase();
      SetAutoCommitMode(1);
      return 0;
    }
    SQLRETURN ret = SQLExecDirectA(m_hStmtUpdate, reinterpret_cast<SQLCHAR *>(pUpdateQuery->wszTrunkQuery), SQL_NTS);
    if (ret && ret != SQL_SUCCESS_WITH_INFO)
    {
      ErrorMsgLog( ret, pUpdateQuery->wszTrunkQuery, "SQLExecDirectW", m_hStmtUpdate);
      RollbackTransaction();
      SetAutoCommitMode(1);
      ErrorAction( ret, m_hStmtUpdate);
      return 0;
    }
  }

  if (*pUpdateQuery->szItemCombineExQuery)
  {
    if (!m_hStmtUpdate)
    {
      ErrLog("m_hStmtUpdate == SQL_NULL_HSTMT : Update_CharacterData");
      ReConnectDataBase();
      SetAutoCommitMode(1);
      return 0;
    }
    SQLRETURN ret =
      SQLExecDirectA(m_hStmtUpdate, reinterpret_cast<SQLCHAR *>(pUpdateQuery->szItemCombineExQuery), SQL_NTS);
    if (ret && ret != SQL_SUCCESS_WITH_INFO)
    {
      ErrorMsgLog( ret, pUpdateQuery->szItemCombineExQuery, "SQLExecDirectA", m_hStmtUpdate);
      RollbackTransaction();
      SetAutoCommitMode(1);
      ErrorAction( ret, m_hStmtUpdate);
      return 0;
    }
  }

  if (*pUpdateQuery->szAMPInvenQuery)
  {
    if (!m_hStmtUpdate)
    {
      ErrLog("m_hStmtUpdate == SQL_NULL_HSTMT : Update_CharacterData");
      ReConnectDataBase();
      SetAutoCommitMode(1);
      return 0;
    }
    SQLRETURN ret = SQLExecDirectA(m_hStmtUpdate, reinterpret_cast<SQLCHAR *>(pUpdateQuery->szAMPInvenQuery), SQL_NTS);
    if (ret && ret != SQL_SUCCESS_WITH_INFO)
    {
      ErrorMsgLog( ret, pUpdateQuery->szAMPInvenQuery, "SQLExecDirectA", m_hStmtUpdate);
      RollbackTransaction();
      SetAutoCommitMode(1);
      ErrorAction( ret, m_hStmtUpdate);
      return 0;
    }
  }

  if (*pUpdateQuery->szPvpPointLimitQuery)
  {
    if (!m_hStmtUpdate)
    {
      ErrLog("m_hStmtUpdate == SQL_NULL_HSTMT : Update_CharacterData");
      ReConnectDataBase();
      SetAutoCommitMode(1);
      return 0;
    }
    SQLRETURN ret =
      SQLExecDirectA(m_hStmtUpdate, reinterpret_cast<SQLCHAR *>(pUpdateQuery->szPvpPointLimitQuery), SQL_NTS);
    if (ret && ret != SQL_SUCCESS_WITH_INFO)
    {
      ErrorMsgLog( ret, pUpdateQuery->szPvpPointLimitQuery, "SQLExecDirectA", m_hStmtUpdate);
      RollbackTransaction();
      SetAutoCommitMode(1);
      ErrorAction( ret, m_hStmtUpdate);
      return 0;
    }
  }

  if (*pUpdateQuery->wszBossCryMsgQuery)
  {
    if (!m_hStmtUpdate)
    {
      ErrLog("m_hStmtUpdate == SQL_NULL_HSTMT : Update_CharacterData");
      ReConnectDataBase();
      SetAutoCommitMode(1);
      return 0;
    }
    SQLRETURN ret = SQLExecDirectA(m_hStmtUpdate, reinterpret_cast<SQLCHAR *>(pUpdateQuery->wszBossCryMsgQuery), SQL_NTS);
    if (ret && ret != SQL_SUCCESS_WITH_INFO)
    {
      ErrorMsgLog( ret, pUpdateQuery->wszBossCryMsgQuery, "_SQLExecDirect", m_hStmtUpdate);
      RollbackTransaction();
      SetAutoCommitMode(1);
      ErrorAction( ret, m_hStmtUpdate);
      return 0;
    }
  }

  if (*pUpdateQuery->szPvpOrderViewQurey)
  {
    if (!m_hStmtUpdate)
    {
      ErrLog("m_hStmtUpdate == SQL_NULL_HSTMT : Update_CharacterData");
      ReConnectDataBase();
      SetAutoCommitMode(1);
      return 0;
    }
    SQLRETURN ret =
      SQLExecDirectA(m_hStmtUpdate, reinterpret_cast<SQLCHAR *>(pUpdateQuery->szPvpOrderViewQurey), SQL_NTS);
    if (ret && ret != SQL_SUCCESS_WITH_INFO)
    {
      ErrorMsgLog( ret, pUpdateQuery->szPvpOrderViewQurey, "_SQLExecDirect", m_hStmtUpdate);
      RollbackTransaction();
      SetAutoCommitMode(1);
      ErrorAction( ret, m_hStmtUpdate);
      return 0;
    }
  }

  if (*pUpdateQuery->szNPCQuestQuery)
  {
    if (!m_hStmtUpdate)
    {
      ErrLog("m_hStmtUpdate == SQL_NULL_HSTMT : Update_CharacterData");
      ReConnectDataBase();
      SetAutoCommitMode(1);
      return 0;
    }
    SQLRETURN ret = SQLExecDirectA(m_hStmtUpdate, reinterpret_cast<SQLCHAR *>(pUpdateQuery->szNPCQuestQuery), SQL_NTS);
    if (ret && ret != SQL_SUCCESS_WITH_INFO)
    {
      ErrorMsgLog( ret, pUpdateQuery->szNPCQuestQuery, "_SQLExecDirect", m_hStmtUpdate);
      RollbackTransaction();
      SetAutoCommitMode(1);
      ErrorAction( ret, m_hStmtUpdate);
      return 0;
    }
  }

  if (*pUpdateQuery->szPcBangPlayTimeQuery)
  {
    if (!m_hStmtUpdate)
    {
      ErrLog("m_hStmtUpdate == SQL_NULLHSTMT : Update_ChaaterData");
      ReConnectDataBase();
      SetAutoCommitMode(1);
      return 0;
    }
    SQLRETURN ret =
      SQLExecDirectA(m_hStmtUpdate, reinterpret_cast<SQLCHAR *>(pUpdateQuery->szPcBangPlayTimeQuery), SQL_NTS);
    if (ret && ret != SQL_SUCCESS_WITH_INFO)
    {
      ErrorMsgLog( ret, pUpdateQuery->szPcBangPlayTimeQuery, "_SQLExecDirect", m_hStmtUpdate);
      RollbackTransaction();
      SetAutoCommitMode(1);
      ErrorAction( ret, m_hStmtUpdate);
      return 0;
    }
  }

  if (*pUpdateQuery->szPotionDelayQuery)
  {
    if (!m_hStmtUpdate)
    {
      ErrLog("m_hStmtUpdate == SQL_NULLHSTMT : Update_ChaaterData");
      ReConnectDataBase();
      SetAutoCommitMode(1);
      return 0;
    }
    SQLRETURN ret =
      SQLExecDirectA(m_hStmtUpdate, reinterpret_cast<SQLCHAR *>(pUpdateQuery->szPotionDelayQuery), SQL_NTS);
    if (ret && ret != SQL_SUCCESS_WITH_INFO)
    {
      ErrorMsgLog( ret, pUpdateQuery->szPotionDelayQuery, "SQLExecDirect", m_hStmtUpdate);
      RollbackTransaction();
      SetAutoCommitMode(1);
      ErrorAction( ret, m_hStmtUpdate);
      return 0;
    }
  }

  if (*pUpdateQuery->szOreCuttingQuery)
  {
    if (!m_hStmtUpdate)
    {
      ErrLog("m_hStmtUpdate == SQL_NULLHSTMT : Update_ChaaterData");
      ReConnectDataBase();
      SetAutoCommitMode(1);
      return 0;
    }
    SQLRETURN ret = SQLExecDirectA(m_hStmtUpdate, reinterpret_cast<SQLCHAR *>(pUpdateQuery->szOreCuttingQuery), SQL_NTS);
    if (ret && ret != SQL_SUCCESS_WITH_INFO)
    {
      ErrorMsgLog( ret, pUpdateQuery->szOreCuttingQuery, "SQLExecDirect", m_hStmtUpdate);
      RollbackTransaction();
      SetAutoCommitMode(1);
      ErrorAction( ret, m_hStmtUpdate);
      return 0;
    }
  }

  if (*pUpdateQuery->szPcBangFavorQuery)
  {
    if (!m_hStmtUpdate)
    {
      ErrLog("m_hStmtUpdate == SQL_NULLHSTMT : Update_ChaaterData");
      ReConnectDataBase();
      SetAutoCommitMode(1);
      return 0;
    }
    SQLRETURN ret =
      SQLExecDirectA(m_hStmtUpdate, reinterpret_cast<SQLCHAR *>(pUpdateQuery->szPcBangFavorQuery), SQL_NTS);
    if (ret && ret != SQL_SUCCESS_WITH_INFO)
    {
      ErrorMsgLog( ret, pUpdateQuery->szPcBangFavorQuery, "SQLExecDirect", m_hStmtUpdate);
      RollbackTransaction();
      SetAutoCommitMode(1);
      ErrorAction( ret, m_hStmtUpdate);
      return 0;
    }
  }

  if (*pUpdateQuery->wszExtTrunkQuery)
  {
    if (!m_hStmtUpdate)
    {
      ErrLog("m_hStmtUpdate == SQL_NULL_HSTMT : Update_CharacterData");
      ReConnectDataBase();
      SetAutoCommitMode(1);
      return 0;
    }
    SQLRETURN ret = SQLExecDirectA(m_hStmtUpdate, reinterpret_cast<SQLCHAR *>(pUpdateQuery->wszExtTrunkQuery), SQL_NTS);
    if (ret && ret != SQL_SUCCESS_WITH_INFO)
    {
      ErrorMsgLog( ret, pUpdateQuery->wszExtTrunkQuery, "SQLExecDirectW", m_hStmtUpdate);
      RollbackTransaction();
      SetAutoCommitMode(1);
      ErrorAction( ret, m_hStmtUpdate);
      return 0;
    }
  }

  if (*pUpdateQuery->szTimeLimitInfoQuery)
  {
    if (!m_hStmtUpdate)
    {
      ErrLog("m_hStmtUpdate == SQL_NULL_HSTMT : Update_CharacterData");
      ReConnectDataBase();
      SetAutoCommitMode(1);
      return 0;
    }
    SQLRETURN ret =
      SQLExecDirectA(m_hStmtUpdate, reinterpret_cast<SQLCHAR *>(pUpdateQuery->szTimeLimitInfoQuery), SQL_NTS);
    if (ret && ret != SQL_SUCCESS_WITH_INFO)
    {
      ErrorMsgLog( ret, pUpdateQuery->szTimeLimitInfoQuery, "SQLExecDirectA", m_hStmtUpdate);
      RollbackTransaction();
      SetAutoCommitMode(1);
      ErrorAction( ret, m_hStmtUpdate);
      return 0;
    }
  }

  CommitTransaction();
  SetAutoCommitMode(1);
  if (m_bSaveDBLog)
  {
    m_ProcessLogA.Write("Update_CharacterData Success : %d", dwSerial);
  }
  return 1;
}

bool CRFWorldDatabase::Update_IncreaseWeeklyGuildKillPvpPointSum(unsigned int dwSerial, long double dPvpPoint)
{
  char buffer[1040]{};
  std::memset(buffer, 0, 1024);
  sprintf_s(buffer, "{ CALL pUpdate_WeeklyGuildKillPVPPoint(%u, %f) }", dwSerial, static_cast<double>(dPvpPoint));
  return ExecUpdateQuery( buffer, true);
}

bool CRFWorldDatabase::Update_CristalBattleCharInfo(
  unsigned int dwSerial,
  unsigned __int8 byHSKTime,
  unsigned __int8 byPvpGrade,
  int iPvpPoint,
  unsigned __int16 wKillPoint,
  unsigned __int16 wDiePoint)
{
  if (byHSKTime >= 3u)
  {
    return false;
  }

  int pvpPoint[3]{};
  unsigned __int16 killPoint[3]{};
  unsigned __int16 diePoint[3]{};
  pvpPoint[byHSKTime] = iPvpPoint;
  killPoint[byHSKTime] = wKillPoint;
  diePoint[byHSKTime] = wDiePoint;

  char buffer[280]{};
  std::memset(buffer, 0, 256);
  sprintf_s(
    buffer,
    "Update tbl_general set Pvp_0=%d, Pk_0=%d, Pvp_1=%d, Pk_1=%d, Pvp_2=%d, Pk_2=%d, CharacterGrade=%d, Die_0=%d, Die_1=%"
    "d, Die_2=%d where serial=%d",
    pvpPoint[0],
    killPoint[0],
    pvpPoint[1],
    killPoint[1],
    pvpPoint[2],
    killPoint[2],
    byPvpGrade,
    diePoint[0],
    diePoint[1],
    diePoint[2],
    dwSerial);
  return ExecUpdateQuery( buffer, true);
}

bool CRFWorldDatabase::update_cristalbattle_date(unsigned int dwCharSerial, unsigned __int8 bHSKTime)
{
  char buffer[272]{};
  sprintf_s(buffer, "{ CALL pUpdate_CristalBattleDate( %d, %d ) }", dwCharSerial, bHSKTime);
  return ExecUpdateQuery( buffer, true);
}

bool CRFWorldDatabase::Insert_Supplement(unsigned int dwSerial)
{
  char buffer[80]{};
  memset(buffer, 0, 64);
  sprintf_s(buffer, 64, "{ CALL pInsert_supplement( %d ) }", dwSerial);
  return ExecUpdateQuery( buffer, true);
}

bool CRFWorldDatabase::Update_Post(char *szPostQuery)
{
  return ExecUpdateQuery( szPostQuery, true);
}

bool CRFWorldDatabase::Select_PostStorageRecordCheck()
{
  unsigned int targetValue[11]{};
  char buffer[144]{};
  memset(buffer, 0, 128);
  sprintf_s(buffer, 128, "select top 1 Serial from tbl_PostStorage where dck=1");
  if (m_bSaveDBLog)
  {
    Log( buffer);
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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, targetValue, 0, &indicator);
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

        ErrorMsgLog( ret, buffer, "SQLGetData", m_hStmtSelect);
        ErrorAction( ret, m_hStmtSelect);
        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        return false;
      }

      if (ret != SQL_NO_DATA)
      {
        ErrorMsgLog( ret, buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction( ret, m_hStmtSelect);
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return false;
    }

    if (ret != SQL_NO_DATA)
    {
      ErrorMsgLog( ret, buffer, "SQLExecDirect", m_hStmtSelect);
      ErrorAction( ret, m_hStmtSelect);
    }
    return false;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return false;
}

bool CRFWorldDatabase::Insert_PostStorageRecord()
{
  char buffer[80]{};
  memset(buffer, 0, 64);
  sprintf_s(buffer, 64, "{ CALL pInsert_PostStorageRecord }");
  return ExecUpdateQuery( buffer, true);
}

bool CRFWorldDatabase::Select_PostStorageEmptyRecordSerial(unsigned int *pdwStorageSerial)
{
  unsigned int targetValue[11]{};
  char buffer[272]{};
  memset(buffer, 0, 256);
  sprintf_s(buffer, 256, "select top 1 Serial from tbl_PostStorage where dck=1");
  if (m_bSaveDBLog)
  {
    Log( buffer);
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
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, targetValue, 0, &indicator);
        if (!ret || ret == SQL_SUCCESS_WITH_INFO)
        {
          *pdwStorageSerial = targetValue[0];
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

        ErrorMsgLog( ret, buffer, "SQLGetData", m_hStmtSelect);
        ErrorAction( ret, m_hStmtSelect);
        if (m_hStmtSelect)
        {
          SQLCloseCursor(m_hStmtSelect);
        }
        return false;
      }

      if (ret != SQL_NO_DATA)
      {
        ErrorMsgLog( ret, buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction( ret, m_hStmtSelect);
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return false;
    }

    if (ret != SQL_NO_DATA)
    {
      ErrorMsgLog( ret, buffer, "SQLExecDirect", m_hStmtSelect);
      ErrorAction( ret, m_hStmtSelect);
    }
    return false;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return false;
}

bool CRFWorldDatabase::Update_PostStorageSendToRecver(
  unsigned int dwOwner,
  unsigned int dwPostSerial,
  unsigned __int8 byPostState,
  char *wszSendName,
  char *wszRecvName,
  char *wszTitle,
  char *wszContent,
  int nK,
  unsigned __int64 dwD,
  unsigned int dwU,
  unsigned int dwGold,
  unsigned __int8 byErr,
  unsigned __int16 wStorageIndex,
  unsigned __int8 *pbyNumber,
  bool bGetNumber,
  unsigned __int64 lnUID)
{
  const char *updatePostStorageQuery =
    "update tbl_PostStorage set postinx=%d,owner=%d,dck=0,poststate=%d,sendname='%s',recvname='%s',title='%s',content='%s'"
    ",k=%d,d=%I64d,u=%d,gold=%d,err=%d,uid=%I64d,sindex=%d where serial=%d";

  unsigned __int8 targetValue[44]{};
  char buffer[1056]{};
  memset(buffer, 0, 1024);

  if (byPostState < 0x64u && bGetNumber)
  {
    sprintf_s(
      buffer,
      1024,
      "select count(Serial) from tbl_PostStorage where owner=%d and dck=0 and poststate<%d",
      dwOwner,
      100);
    if (m_bSaveDBLog)
    {
      Log( buffer);
    }
    if (!m_hStmtSelect && !ReConnectDataBase())
    {
      ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
      return false;
    }

    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), SQL_NTS);
    if (ret && ret != SQL_SUCCESS_WITH_INFO)
    {
      if (ret != SQL_NO_DATA)
      {
        ErrorMsgLog( ret, buffer, "_SQLExecDirect", m_hStmtSelect);
        ErrorAction( ret, m_hStmtSelect);
      }
      return false;
    }

    ret = SQLFetch(m_hStmtSelect);
    if (ret && ret != SQL_SUCCESS_WITH_INFO)
    {
      if (ret != SQL_NO_DATA)
      {
        ErrorMsgLog( ret, buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction( ret, m_hStmtSelect);
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return false;
    }

    SQLLEN indicator = 0;
    ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_TINYINT, targetValue, 0, &indicator);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      *pbyNumber = targetValue[0];
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      if (m_bSaveDBLog)
      {
        FmtLog("%s Success", buffer);
      }
    }
    else
    {
      ErrorMsgLog( ret, buffer, "SQLGetData", m_hStmtSelect);
      ErrorAction( ret, m_hStmtSelect);
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return false;
    }
  }

  const int queryLen = _scprintf(
    updatePostStorageQuery,
    *pbyNumber,
    dwOwner,
    byPostState,
    wszSendName,
    wszRecvName,
    wszTitle,
    wszContent,
    nK,
    dwD,
    dwU,
    dwGold,
    byErr,
    lnUID,
    wStorageIndex,
    dwPostSerial);

  if (queryLen < 0)
  {
    ErrLog("Update_PostStorageSendToRecver : _scprintf failed");
    return false;
  }

  std::vector<char> strQuery(static_cast<size_t>(queryLen) + 1);
  const int writeLen = sprintf_s(
    strQuery.data(),
    strQuery.size(),
    updatePostStorageQuery,
    *pbyNumber,
    dwOwner,
    byPostState,
    wszSendName,
    wszRecvName,
    wszTitle,
    wszContent,
    nK,
    dwD,
    dwU,
    dwGold,
    byErr,
    lnUID,
    wStorageIndex,
    dwPostSerial);
  if (writeLen < 0)
  {
    ErrLog("Update_PostStorageSendToRecver : sprintf_s failed");
    return false;
  }

  return ExecUpdateQuery(strQuery.data(), true);
}

bool CRFWorldDatabase::Insert_Start_NpcQuest_History(
  unsigned int dwSerial,
  char *szQuestCode,
  unsigned __int8 byLevel,
  char *szTime,
  __int64 nEndTime)
{
  char buffer[272]{};
  sprintf_s(
    buffer,
    256,
    "{ CALL pInsert_Start_Npc_Quest_History_Type1( %d, '%s', %d, '%s', %I64d) }",
    dwSerial,
    szQuestCode,
    byLevel,
    szTime,
    nEndTime);
  return ExecUpdateQuery( buffer, true);
}

bool CRFWorldDatabase::Update_Start_NpcQuest_History(
  unsigned int dwSerial,
  char *szQuestCode,
  unsigned __int8 byLevel,
  char *szTime,
  __int64 nEndTime)
{
  char buffer[272]{};
  sprintf_s(
    buffer,
    256,
    "{ CALL pUpdate_Start_Npc_Quest_History_Type1( %d, '%s', %d, '%s', %I64d) }",
    dwSerial,
    szQuestCode,
    byLevel,
    szTime,
    nEndTime);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::Update_Level(unsigned int dwSerial, unsigned __int8 byLv)
{
  char buffer[272]{};
  sprintf_s(buffer, "{ CALL pUpdate_Level( %d, %d ) }", dwSerial, byLv);
  return ExecUpdateQuery(buffer, true);
}

bool CRFWorldDatabase::Update_OutputGuildMoney(
  unsigned int dwGuildSerial,
  unsigned int dwDalant,
  unsigned int dwGold)
{
  char buffer[272]{};
  memset(buffer, 0, 256);
  sprintf_s(
    buffer,
    "update tbl_guild set dalant = dalant - %u, gold = gold - %u where Serial = %u",
    dwDalant,
    dwGold,
    dwGuildSerial);
  return ExecUpdateQuery(buffer, true);
}

