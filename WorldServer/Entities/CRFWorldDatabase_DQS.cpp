#include "pch.h"

#include "CRFWorldDatabase.h"

#include <sql.h>
#include <sqlext.h>

#include "DqsDbStructs.h"

unsigned __int8 CRFWorldDatabase::Select_CharacterBaseInfoBySerial(
  unsigned int dwAccountSerial,
  _worlddb_character_base_info_array *pCharacterDataArray)
{
  SQLLEN indicator = 0;
  int count = 0;
  char buffer[260]{};

  sprintf(buffer, "{ CALL pSelect_CharacterBaseInfoBySerial_20061115( %d ) }", dwAccountSerial);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), SQL_NTS);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      while (true)
      {
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != SQL_SUCCESS_WITH_INFO)
        {
          break;
        }

        SQLGetData(m_hStmtSelect, 1u, SQL_C_ULONG, &pCharacterDataArray->CharacterInfo[count].dwSerial, 0, &indicator);
        SQLGetData(
          m_hStmtSelect,
          2u,
          SQL_C_CHAR,
          &pCharacterDataArray->CharacterInfo[count],
          17,
          &indicator);
        SQLGetData(
          m_hStmtSelect,
          3u,
          SQL_C_UTINYINT,
          &pCharacterDataArray->CharacterInfo[count].bySlotIndex,
          0,
          &indicator);
        SQLGetData(
          m_hStmtSelect,
          4u,
          SQL_C_UTINYINT,
          &pCharacterDataArray->CharacterInfo[count].byRace,
          0,
          &indicator);
        SQLGetData(
          m_hStmtSelect,
          5u,
          SQL_C_CHAR,
          pCharacterDataArray->CharacterInfo[count].szClassCode,
          5,
          &indicator);
        SQLGetData(
          m_hStmtSelect,
          6u,
          SQL_C_UTINYINT,
          &pCharacterDataArray->CharacterInfo[count].byLevel,
          0,
          &indicator);
        SQLGetData(
          m_hStmtSelect,
          7u,
          SQL_C_ULONG,
          &pCharacterDataArray->CharacterInfo[count].dwDalant,
          0,
          &indicator);
        SQLGetData(
          m_hStmtSelect,
          8u,
          SQL_C_ULONG,
          &pCharacterDataArray->CharacterInfo[count].dwGold,
          0,
          &indicator);
        SQLGetData(
          m_hStmtSelect,
          9u,
          SQL_C_ULONG,
          &pCharacterDataArray->CharacterInfo[count].dwBaseShape,
          0,
          &indicator);
        SQLGetData(
          m_hStmtSelect,
          10u,
          SQL_C_ULONG,
          &pCharacterDataArray->CharacterInfo[count].dwLastConnTime,
          0,
          &indicator);
        for (int j = 0; j < 8; ++j)
        {
          SQLGetData(
            m_hStmtSelect,
            j + 11,
            SQL_C_SSHORT,
            &pCharacterDataArray->CharacterInfo[count].shEKArray[j],
            0,
            &indicator);
        }
        for (int j = 0; j < 8; ++j)
        {
          SQLGetData(
            m_hStmtSelect,
            j + 19,
            SQL_C_ULONG,
            &pCharacterDataArray->CharacterInfo[count].dwEUArray[j],
            0,
            &indicator);
        }
        for (int j = 0; j < 8; ++j)
        {
          SQLGetData(
            m_hStmtSelect,
            j + 27,
            SQL_C_SBIGINT,
            &pCharacterDataArray->CharacterInfo[count].lnUIDArray[j],
            0,
            &indicator);
        }
        for (int j = 0; j < 8; ++j)
        {
          SQLGetData(
            m_hStmtSelect,
            j + 35,
            SQL_C_ULONG,
            &pCharacterDataArray->CharacterInfo[count].dwETArray[j],
            0,
            &indicator);
        }
        ++count;

        if (ret && ret != SQL_SUCCESS_WITH_INFO)
        {
          if (ret != SQL_NO_DATA)
          {
            ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
            ErrorAction(ret, m_hStmtSelect);
            if (m_hStmtSelect)
            {
              SQLCloseCursor(m_hStmtSelect);
            }
            return 1;
          }
          break;
        }
      }

      pCharacterDataArray->wCharacterCount = static_cast<unsigned __int16>(count);
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

char CRFWorldDatabase::Select_CharacterBaseInfo(
  unsigned int dwCharacterSerial,
  _worlddb_character_base_info *pCharacterData)
{
  SQLLEN indicator = 0;
  char buffer[260]{};
  int column = 0;

  sprintf(buffer, "{ CALL pSelect_CharacterBaseInfo_20061115( %d ) }", dwCharacterSerial);
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
        SQLGetData(m_hStmtSelect, ++column, SQL_C_CHAR, pCharacterData, 17, &indicator);
        SQLGetData(m_hStmtSelect, ++column, SQL_C_UTINYINT, &pCharacterData->bySlotIndex, 0, &indicator);
        SQLGetData(m_hStmtSelect, ++column, SQL_C_UTINYINT, &pCharacterData->byRace, 0, &indicator);
        SQLGetData(m_hStmtSelect, ++column, SQL_C_CHAR, pCharacterData->szClassCode, 5, &indicator);
        SQLGetData(m_hStmtSelect, ++column, SQL_C_UTINYINT, &pCharacterData->byLevel, 0, &indicator);
        SQLGetData(m_hStmtSelect, ++column, SQL_C_ULONG, &pCharacterData->dwDalant, 0, &indicator);
        SQLGetData(m_hStmtSelect, ++column, SQL_C_ULONG, &pCharacterData->dwGold, 0, &indicator);
        SQLGetData(m_hStmtSelect, ++column, SQL_C_ULONG, &pCharacterData->dwBaseShape, 0, &indicator);
        SQLGetData(m_hStmtSelect, ++column, SQL_C_ULONG, &pCharacterData->dwLastConnTime, 0, &indicator);
        for (int j = 0; j < 8; ++j)
        {
          SQLGetData(
            m_hStmtSelect,
            ++column,
            SQL_C_SSHORT,
            &pCharacterData->shEKArray[j],
            0,
            &indicator);
        }
        for (int j = 0; j < 8; ++j)
        {
          SQLGetData(
            m_hStmtSelect,
            ++column,
            SQL_C_ULONG,
            &pCharacterData->dwEUArray[j],
            0,
            &indicator);
        }
        for (int j = 0; j < 8; ++j)
        {
          SQLGetData(
            m_hStmtSelect,
            ++column,
            SQL_C_SBIGINT,
            &pCharacterData->lnUIDArray[j],
            0,
            &indicator);
        }
        for (int j = 0; j < 8; ++j)
        {
          SQLGetData(
            m_hStmtSelect,
            ++column,
            SQL_C_ULONG,
            &pCharacterData->dwETArray[j],
            0,
            &indicator);
        }
        ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_ULONG, &pCharacterData->dwCheckSum, 0, &indicator);
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

      if (ret != SQL_NO_DATA)
      {
        ErrorMsgLog(ret, buffer, "SQLFetch", m_hStmtSelect);
        ErrorAction(ret, m_hStmtSelect);
      }
      if (m_hStmtSelect)
      {
        SQLCloseCursor(m_hStmtSelect);
      }
      return ret == SQL_NO_DATA ? 2 : 1;
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

unsigned __int8 CRFWorldDatabase::Select_NotArrangeCharacter(
  unsigned int dwAccountSerial,
  _worlddb_arrange_char_info *pCharData)
{
  SQLLEN indicator = 0;
  int count = 0;
  char buffer[260]{};

  sprintf(buffer, "{ CALL pSelect_NotArrangeCharacter( %d ) }", dwAccountSerial);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), SQL_NTS);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      while (true)
      {
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != SQL_SUCCESS_WITH_INFO)
        {
          break;
        }

        SQLGetData(
          m_hStmtSelect,
          1u,
          SQL_C_ULONG,
          &pCharData->ArrangeChar[count],
          0,
          &indicator);
        SQLGetData(
          m_hStmtSelect,
          2u,
          SQL_C_UTINYINT,
          &pCharData->ArrangeChar[count].byLv,
          0,
          &indicator);
        SQLGetData(
          m_hStmtSelect,
          3u,
          SQL_C_UTINYINT,
          &pCharData->ArrangeChar[count].byRaceSexCode,
          0,
          &indicator);
        SQLGetData(
          m_hStmtSelect,
          4u,
          SQL_C_ULONG,
          &pCharData->ArrangeChar[count].dwDalant,
          0,
          &indicator);
        SQLGetData(
          m_hStmtSelect,
          5u,
          SQL_C_ULONG,
          &pCharData->ArrangeChar[count].dwGold,
          0,
          &indicator);
        SQLGetData(
          m_hStmtSelect,
          6u,
          SQL_C_CHAR,
          pCharData->ArrangeChar[count].wszName,
          17,
          &indicator);
        SQLGetData(
          m_hStmtSelect,
          7u,
          SQL_C_CHAR,
          pCharData->ArrangeChar[count].szServer,
          33,
          &indicator);
        SQLGetData(
          m_hStmtSelect,
          8u,
          SQL_C_CHAR,
          pCharData->ArrangeChar[count].szClassCode,
          5,
          &indicator);
        ++count;
      }

      pCharData->byCount = static_cast<unsigned __int8>(count);
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

char CRFWorldDatabase::Select_Equal_Name(char *pwszCharacterName)
{
  char buffer[260]{};
  sprintf(buffer, "{ CALL pSelect_Equal_Name('%s') }", pwszCharacterName);
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

      char result = 1;
      if (ret != SQL_NO_DATA)
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

char CRFWorldDatabase::Select_Equal_DeleteName_NoArranged(char *pwszCharacterName)
{
  char buffer[260]{};
  sprintf(buffer, "{ CALL pSelect_Equal_DeleteName_NoArranged('%s') }", pwszCharacterName);
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

      char result = 1;
      if (ret != SQL_NO_DATA)
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
