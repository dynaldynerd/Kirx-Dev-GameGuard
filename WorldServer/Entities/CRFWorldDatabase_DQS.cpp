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

  sprintf_s(buffer, "{ CALL pSelect_CharacterBaseInfoBySerial_20061115( %d ) }", dwAccountSerial);
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
        __int64 lastConnTime = 0;
        __int64 itemUid = 0;
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != SQL_SUCCESS_WITH_INFO)
        {
          break;
        }

        int slotIndex = 0;
        int race = 0;
        int level = 0;
        int equipmentKey = 0;
        SQLGetData(m_hStmtSelect, 1u, SQL_C_LONG, &pCharacterDataArray->CharacterInfo[count].dwSerial, 0, &indicator);
        SQLGetData(
          m_hStmtSelect,
          2u,
          SQL_C_CHAR,
          pCharacterDataArray->CharacterInfo[count].wszName,
          17,
          &indicator);
        SQLGetData(
          m_hStmtSelect,
          3u,
          SQL_C_LONG,
          &slotIndex,
          0,
          &indicator);
        pCharacterDataArray->CharacterInfo[count].bySlotIndex = static_cast<unsigned __int8>(slotIndex);
        SQLGetData(
          m_hStmtSelect,
          4u,
          SQL_C_LONG,
          &race,
          0,
          &indicator);
        pCharacterDataArray->CharacterInfo[count].byRace = static_cast<unsigned __int8>(race);
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
          SQL_C_LONG,
          &level,
          0,
          &indicator);
        pCharacterDataArray->CharacterInfo[count].byLevel = static_cast<unsigned __int8>(level);
        SQLGetData(
          m_hStmtSelect,
          7u,
          SQL_C_LONG,
          &pCharacterDataArray->CharacterInfo[count].dwDalant,
          0,
          &indicator);
        SQLGetData(
          m_hStmtSelect,
          8u,
          SQL_C_LONG,
          &pCharacterDataArray->CharacterInfo[count].dwGold,
          0,
          &indicator);
        SQLGetData(
          m_hStmtSelect,
          9u,
          SQL_C_LONG,
          &pCharacterDataArray->CharacterInfo[count].dwBaseShape,
          0,
          &indicator);
        SQLGetData(
          m_hStmtSelect,
          10u,
          SQL_C_SBIGINT,
          &lastConnTime,
          0,
          &indicator);
        pCharacterDataArray->CharacterInfo[count].dwLastConnTime = static_cast<unsigned __int64>(lastConnTime);
        for (int j = 0; j < 8; ++j)
        {
          equipmentKey = 0;
          SQLGetData(
            m_hStmtSelect,
            j + 11,
            SQL_C_LONG,
            &equipmentKey,
            0,
            &indicator);
          pCharacterDataArray->CharacterInfo[count].shEKArray[j] = static_cast<__int16>(equipmentKey);
        }
        for (int j = 0; j < 8; ++j)
        {
          SQLGetData(
            m_hStmtSelect,
            j + 19,
            SQL_C_LONG,
            &pCharacterDataArray->CharacterInfo[count].dwEUArray[j],
            0,
            &indicator);
        }
        for (int j = 0; j < 8; ++j)
        {
          itemUid = 0;
          SQLGetData(
            m_hStmtSelect,
            j + 27,
            SQL_C_SBIGINT,
            &itemUid,
            0,
            &indicator);
          pCharacterDataArray->CharacterInfo[count].lnUIDArray[j] = static_cast<unsigned __int64>(itemUid);
        }
        for (int j = 0; j < 8; ++j)
        {
          SQLGetData(
            m_hStmtSelect,
            j + 35,
            SQL_C_LONG,
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

unsigned __int8 CRFWorldDatabase::Select_CharacterBaseInfo(
  unsigned int dwCharacterSerial,
  _worlddb_character_base_info *pCharacterData)
{
  SQLLEN indicator = 0;
  char buffer[260]{};
  int column = 0;

  sprintf_s(buffer, "{ CALL pSelect_CharacterBaseInfo_20061115( %d ) }", dwCharacterSerial);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }

  if (m_hStmtSelect || ReConnectDataBase())
  {
    SQLRETURN ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), SQL_NTS);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      __int64 lastConnTime = 0;
      __int64 itemUid = 0;
      int slotIndex = 0;
      int race = 0;
      int level = 0;
      int equipmentKey = 0;
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == SQL_SUCCESS_WITH_INFO)
      {
        SQLGetData(m_hStmtSelect, ++column, SQL_C_CHAR, pCharacterData, 17, &indicator);
        SQLGetData(m_hStmtSelect, ++column, SQL_C_LONG, &slotIndex, 0, &indicator);
        pCharacterData->bySlotIndex = static_cast<unsigned __int8>(slotIndex);
        SQLGetData(m_hStmtSelect, ++column, SQL_C_LONG, &race, 0, &indicator);
        pCharacterData->byRace = static_cast<unsigned __int8>(race);
        SQLGetData(m_hStmtSelect, ++column, SQL_C_CHAR, pCharacterData->szClassCode, 5, &indicator);
        SQLGetData(m_hStmtSelect, ++column, SQL_C_LONG, &level, 0, &indicator);
        pCharacterData->byLevel = static_cast<unsigned __int8>(level);
        SQLGetData(m_hStmtSelect, ++column, SQL_C_LONG, &pCharacterData->dwDalant, 0, &indicator);
        SQLGetData(m_hStmtSelect, ++column, SQL_C_LONG, &pCharacterData->dwGold, 0, &indicator);
        SQLGetData(m_hStmtSelect, ++column, SQL_C_LONG, &pCharacterData->dwBaseShape, 0, &indicator);
        SQLGetData(m_hStmtSelect, ++column, SQL_C_SBIGINT, &lastConnTime, 0, &indicator);
        pCharacterData->dwLastConnTime = static_cast<unsigned __int64>(lastConnTime);
        for (int j = 0; j < 8; ++j)
        {
          equipmentKey = 0;
          SQLGetData(
            m_hStmtSelect,
            ++column,
            SQL_C_LONG,
            &equipmentKey,
            0,
            &indicator);
          pCharacterData->shEKArray[j] = static_cast<__int16>(equipmentKey);
        }
        for (int j = 0; j < 8; ++j)
        {
          SQLGetData(
            m_hStmtSelect,
            ++column,
            SQL_C_LONG,
            &pCharacterData->dwEUArray[j],
            0,
            &indicator);
        }
        for (int j = 0; j < 8; ++j)
        {
          itemUid = 0;
          SQLGetData(
            m_hStmtSelect,
            ++column,
            SQL_C_SBIGINT,
            &itemUid,
            0,
            &indicator);
          pCharacterData->lnUIDArray[j] = static_cast<unsigned __int64>(itemUid);
        }
        for (int j = 0; j < 8; ++j)
        {
          SQLGetData(
            m_hStmtSelect,
            ++column,
            SQL_C_LONG,
            &pCharacterData->dwETArray[j],
            0,
            &indicator);
        }
        ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_LONG, &pCharacterData->dwCheckSum, 0, &indicator);
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

  sprintf_s(buffer, "{ CALL pSelect_NotArrangeCharacter( %d ) }", dwAccountSerial);
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
        int level = 0;
        int raceSexCode = 0;
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != SQL_SUCCESS_WITH_INFO)
        {
          break;
        }

        SQLGetData(
          m_hStmtSelect,
          1u,
          SQL_C_LONG,
          &pCharData->ArrangeChar[count].dwSerial,
          0,
          &indicator);
        SQLGetData(
          m_hStmtSelect,
          2u,
          SQL_C_LONG,
          &level,
          0,
          &indicator);
        pCharData->ArrangeChar[count].byLv = static_cast<unsigned __int8>(level);
        SQLGetData(
          m_hStmtSelect,
          3u,
          SQL_C_LONG,
          &raceSexCode,
          0,
          &indicator);
        pCharData->ArrangeChar[count].byRaceSexCode = static_cast<unsigned __int8>(raceSexCode);
        SQLGetData(
          m_hStmtSelect,
          4u,
          SQL_C_LONG,
          &pCharData->ArrangeChar[count].dwDalant,
          0,
          &indicator);
        SQLGetData(
          m_hStmtSelect,
          5u,
          SQL_C_LONG,
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
  sprintf_s(buffer, "{ CALL pSelect_Equal_Name('%s') }", pwszCharacterName);
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

bool CRFWorldDatabase::Select_Equal_DeleteName_NoArranged(char *pwszCharacterName)
{
  char buffer[260]{};
  sprintf_s(buffer, "select deletename from tbl_base where deletename = '%s' and dck = 1 and arrange = 0 ", pwszCharacterName);
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
        return false;
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
      return (result) != 0;
    }

    if (ret == SQL_NO_DATA)
    {
      return true;
    }

    ErrorMsgLog(ret, buffer, "_SQLExecDirect", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return false;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return false;
}

