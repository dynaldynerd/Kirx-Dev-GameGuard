#include "pch.h"

#include "CRFWorldDatabase.h"
#include "CPotionMgr.h"
#include "PatriarchElectProcessor.h"
#include "WorldServerUtil.h"
#include "GlobalObjects.h"

#include <cstring>
#include <cstdio>
#include <ctime>
#include <sql.h>
#include <sqlext.h>

bool CRFWorldDatabase::Insert_Buddy(unsigned int dwSerial)
{
  char buffer[272]{};
  sprintf_s(buffer, "{ CALL pInsert_Buddy( %d ) }", dwSerial);
  return ExecUpdateQuery(buffer, 1);
}

unsigned __int8 CRFWorldDatabase::Select_Buddy(unsigned int dwSerial, _worlddb_buddy_info *pBuddyData)
{
  char buffer[260]{};
  SQLLEN indicator = 0;
  SQLRETURN ret = 0;
  int colIndex = 0;

  sprintf_s(buffer, "{ CALL pSelect_Buddy20071220( %d ) }", dwSerial);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }
  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == SQL_SUCCESS_WITH_INFO)
      {
        for (int j = 0; j < 50; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_LONG, &pBuddyData->BuddyData[j].dwSerial, 0, &indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
          ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_CHAR, pBuddyData->BuddyData[j].wszName, 17, &indicator);
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

    ErrorMsgLog(ret, buffer, "SQLExecDirect", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 1;
}

bool CRFWorldDatabase::Insert_BossCryRecord(unsigned int dwSerial)
{
  char buffer[272]{};
  sprintf_s(buffer, "Insert into tbl_CryMsg( Serial ) Values ( %d )", dwSerial);
  return ExecUpdateQuery(buffer, 1);
}

unsigned __int8 CRFWorldDatabase::Select_BossCryMsg(unsigned int dwSerial, _worlddb_crymsg_info *pCryMsg)
{
  char buffer[544]{};
  char source[260]{};
  SQLLEN indicator = 0;
  SQLRETURN ret = 0;
  int colIndex = 0;

  sprintf_s(buffer, "select ");
  for (int j = 0; j < 10; ++j)
  {
    if (j == 9)
    {
      sprintf_s(source, "CryMsg%d ", 10);
    }
    else
    {
      sprintf_s(source, "CryMsg%d, ", j + 1);
    }
    std::strcat(buffer, source);
  }
  sprintf_s(source, "from tbl_CryMsg where Serial = %d", dwSerial);
  std::strcat(buffer, source);

  if (m_bSaveDBLog)
  {
    Log(buffer);
  }
  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == SQL_SUCCESS_WITH_INFO)
      {
        for (int j = 0; j < 10; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++colIndex, 1, &pCryMsg->CryMsgData[j], 65, &indicator);
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

    ErrorMsgLog(ret, buffer, "SQLExecDirect", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 1;
}
unsigned __int8 CRFWorldDatabase::Select_CharacterGeneralInfo(
  unsigned int dwCharacterSerial,
  _worlddb_character_general_info *pCharacterData)
{
  char buffer[260]{};
  SQLLEN indicator = 0;
  SQLRETURN ret = 0;
  int colIndex = 0;
  int maxLevel = 0;
  int bagNum = 0;
  int mapCode = 0;

  sprintf_s(buffer, "{ CALL pSelect_CharacterGeneralInfo_20081216( %d ) }", dwCharacterSerial);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }
  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == SQL_SUCCESS_WITH_INFO)
      {
        colIndex = 1;
        ret = SQLGetData(m_hStmtSelect, colIndex, SQL_C_LONG, &pCharacterData->dwHP, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_LONG, &pCharacterData->dwFP, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_LONG, &pCharacterData->dwSP, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_DOUBLE, &pCharacterData->dExp, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_DOUBLE, &pCharacterData->dLoseExp, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_LONG, &maxLevel, 0, &indicator);
        pCharacterData->byMaxLevel = static_cast<unsigned __int8>(maxLevel);
        if (pCharacterData->byMaxLevel < 50)
        {
          pCharacterData->byMaxLevel = 50;
        }
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_LONG, &bagNum, 0, &indicator);
        pCharacterData->byBagNum = static_cast<unsigned __int8>(bagNum);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_LONG, &mapCode, 0, &indicator);
        pCharacterData->byMapCode = static_cast<unsigned __int8>(mapCode);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_FLOAT, pCharacterData->fStartPos, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_FLOAT, &pCharacterData->fStartPos[1], 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_FLOAT, &pCharacterData->fStartPos[2], 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_LONG, &pCharacterData->dwTotalPlayMin, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_CHAR, pCharacterData->szLeftResList, 160, &indicator);
        for (int j = 0; j < 7; ++j)
        {
          short equipmentDurability = 0;
          __int64 equipmentSerial = 0;
          ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_LONG, &pCharacterData->lEK[j], 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_SHORT, &equipmentDurability, 0, &indicator);
          pCharacterData->wED[j] = static_cast<unsigned __int16>(equipmentDurability);
          ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_SBIGINT, &equipmentSerial, 0, &indicator);
          pCharacterData->lnUID_E[j] = static_cast<unsigned __int64>(equipmentSerial);
          ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_LONG, &pCharacterData->dwET[j], 0, &indicator);
        }
        for (int j = 0; j < 88; ++j)
        {
          __int64 itemSerial = 0;
          ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_LONG, &pCharacterData->lF[j], 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_SBIGINT, &itemSerial, 0, &indicator);
          pCharacterData->lnUID_F[j] = static_cast<unsigned __int64>(itemSerial);
        }
        for (int j = 0; j < 2; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_LONG, &pCharacterData->dwWM[j], 0, &indicator);
        }
        for (int j = 0; j < 24; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_LONG, &pCharacterData->dwFM[j], 0, &indicator);
        }
        for (int j = 0; j < 48; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_LONG, &pCharacterData->dwSM[j], 0, &indicator);
        }
        for (int j = 0; j < 3; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_LONG, &pCharacterData->dwMI[j], 0, &indicator);
        }
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_LONG, &pCharacterData->dwSR, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_LONG, &pCharacterData->dwDM, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_LONG, &pCharacterData->dwPM, 0, &indicator);
        for (int j = 0; j < 4; ++j)
        {
          __int64 animusExp = 0;
          __int64 animusSerial = 0;
          ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_UTINYINT, &pCharacterData->byAK[j], 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_SBIGINT, &animusExp, 0, &indicator);
          pCharacterData->dwAD[j] = static_cast<unsigned __int64>(animusExp);
          ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_LONG, &pCharacterData->dwAP[j], 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_SBIGINT, &animusSerial, 0, &indicator);
          pCharacterData->lnUID_A[j] = static_cast<unsigned __int64>(animusSerial);
        }
        for (int j = 0; j < 3; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_SHORT, &pCharacterData->zClassHistory[j], 0, &indicator);
        }
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_LONG, &pCharacterData->dwClassInitCnt, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_UTINYINT, &pCharacterData->byLastClassGrade, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_DOUBLE, &pCharacterData->dPvPPoint, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_DOUBLE, &pCharacterData->dPvPCashBag, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_CHAR, pCharacterData->szBindMapCode, 12, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_CHAR, pCharacterData->szBindDummy, 12, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_LONG, &pCharacterData->dwGuildSerial, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_UTINYINT, &pCharacterData->byGuildGrade, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_LONG, &pCharacterData->dwDP, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_LONG, &pCharacterData->dwRadarDelayTime, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_LONG, &pCharacterData->dwTakeLastMentalTicket, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_LONG, &pCharacterData->dwTakeLastCriTicket, 0, &indicator);

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
  return 0;
}

unsigned __int8 CRFWorldDatabase::Select_Inven(
  unsigned int dwCharacterSerial,
  unsigned __int16 wBagCount,
  _worlddb_inven_info *pInvenData)
{
  char buffer[260]{};
  SQLLEN indicator = 0;
  SQLRETURN ret = 0;
  int colIndex = 0;

  sprintf_s(buffer, "{ CALL pSelect_inven_20061115_%d( %d ) }", wBagCount, dwCharacterSerial);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }
  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == SQL_SUCCESS_WITH_INFO)
      {
        for (int j = 0; j < 100; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++colIndex, 4, &pInvenData->invenKey[j], 0, &indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
          ret = SQLGetData(m_hStmtSelect, ++colIndex, static_cast<SQLSMALLINT>(65511), &pInvenData->invenKey[j].dwD, 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++colIndex, 4, &pInvenData->invenKey[j].dwU, 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++colIndex, static_cast<SQLSMALLINT>(65511), &pInvenData->invenKey[j].lnUID, 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++colIndex, 4, &pInvenData->invenKey[j].dwT, 0, &indicator);
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

unsigned __int8 CRFWorldDatabase::Select_Quest(unsigned int dwSerial, _worlddb_quest_array *questData)
{
  char buffer[260]{};
  SQLLEN indicator = 0;
  SQLRETURN ret = 0;
  int colIndex = 0;

  sprintf_s(buffer, "{ CALL pSelect_Quest_20070611( %d ) }", dwSerial);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }
  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == SQL_SUCCESS_WITH_INFO)
      {
        for (int j = 0; j < 30; ++j)
        {
          ret = SQLGetData(
            m_hStmtSelect,
            ++colIndex,
            SQL_C_UTINYINT,
            &questData->questList[j].byQuestType,
            0,
            &indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
          ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_SHORT, &questData->questList[j].wIndex, 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_SHORT, questData->questList[j].wNum, 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_SHORT, &questData->questList[j].wNum[1], 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_SHORT, &questData->questList[j].wNum[2], 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++colIndex, SQL_C_LONG, &questData->questList[j].dwPassSec, 0, &indicator);
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

bool CRFWorldDatabase::Insert_Quest(unsigned int dwSerial)
{
  char buffer[272]{};
  sprintf_s(buffer, "{ CALL pInsert_Quest( %d ) }", dwSerial);
  return ExecUpdateQuery(buffer, 1);
}

char CRFWorldDatabase::Select_CharacterSerial(char *pwszCharacterName, unsigned int *pSerial)
{
  char buffer[260]{};
  SQLLEN indicator = 0;
  SQLRETURN ret = 0;

  sprintf_s(buffer, "{ CALL pSelect_CharacterSerial('%s') }", pwszCharacterName);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }
  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == SQL_SUCCESS_WITH_INFO)
      {
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_LONG, pSerial, 0, &indicator);
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
      ErrorMsgLog(ret, buffer, "_SQLExecDirect", m_hStmtSelect);
      ErrorAction(ret, m_hStmtSelect);
    }
    return 0;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 0;
}

int CRFWorldDatabase::Select_Punishment(
  unsigned int dwSerial,
  unsigned int *pdwElectSerial,
  unsigned int *pdwPunishment)
{
  char buffer[260]{};
  SQLLEN indicator = 0;
  SQLRETURN ret = 0;
  unsigned __int8 targetValue[16]{};

  PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
  unsigned int currentElectSerial = processor->GetCurrPatriarchElectSerial();
  sprintf_s(buffer, "{ CALL pSelect_Punishment( %d, %d ) }", dwSerial, currentElectSerial);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }
  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      do
      {
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != SQL_SUCCESS_WITH_INFO)
        {
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
          // narrowing cast for thunk return parity
          return static_cast<int>(result);
        }

        ret = SQLGetData(m_hStmtSelect, 1u, static_cast<SQLSMALLINT>(65530), targetValue, 0, &indicator);
        if (targetValue[0] <= 2u)
        {
          ret = SQLGetData(m_hStmtSelect, 2u, SQL_C_LONG, &pdwPunishment[targetValue[0]], 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, 3u, 4, &pdwElectSerial[targetValue[0]], 0, &indicator);
        }
      }
      while (!ret || ret == SQL_SUCCESS_WITH_INFO);

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
      // narrowing cast for thunk return parity
      return static_cast<int>(result);
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

int CRFWorldDatabase::Select_PatriarchVoted(
  unsigned __int8 byRace,
  unsigned int dwSerial,
  bool *bOverlapVote)
{
  char buffer[516]{};
  SQLLEN indicator = 0;
  SQLRETURN ret = 0;
  unsigned int targetValue[4]{};
sprintf_s(
    buffer,
    "SELECT\tDATEDIFF(day, tmracebossvote, getdate()) as days FROM \t[dbo].[tbl_general] WHERE\tSerial = %d",
    dwSerial);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }
  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == SQL_SUCCESS_WITH_INFO)
      {
        ret = SQLGetData(m_hStmtSelect, 1u, 4, targetValue, 0, &indicator);
        if (!ret || ret == SQL_SUCCESS_WITH_INFO)
        {
          *bOverlapVote = targetValue[0] < 6;
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
          *bOverlapVote = false;
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
        // narrowing cast for thunk return parity
        return static_cast<int>(result);
      }

      unsigned __int8 result = 0;
      if (ret == SQL_NO_DATA)
      {
        *bOverlapVote = false;
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
      // narrowing cast for thunk return parity
      return static_cast<int>(result);
    }

    if (ret == SQL_NO_DATA)
    {
      *bOverlapVote = false;
      return 2;
    }

    ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 1;
}

int CRFWorldDatabase::Select_PatriarchRefundCount(
  unsigned __int8 byRaceCode,
  unsigned int dwAvatorSerial,
  unsigned int *pdwCnt)
{
  char buffer[1028]{};
  SQLLEN indicator = 0;
  SQLRETURN ret = 0;

  sprintf_s(
    buffer,
    "select\tsum(refund) from\t[dbo].[tbl_patriarch_candidate] where\trace = %d and\taserial = %d ",
    byRaceCode,
    dwAvatorSerial);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }
  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == SQL_SUCCESS_WITH_INFO)
      {
        ret = SQLGetData(m_hStmtSelect, 1u, static_cast<SQLSMALLINT>(65530), pdwCnt, 0, &indicator);
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
        // narrowing cast for thunk return parity
        return static_cast<int>(result);
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
      // narrowing cast for thunk return parity
      return static_cast<int>(result);
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

unsigned __int8 CRFWorldDatabase::Select_SFDelayInfo(unsigned int dwSerial, _worlddb_sf_delay_info *pSFDelay)
{
  char buffer[260]{};
  unsigned __int8 chunk[1048]{};
  SQLLEN indicator = 0;
  SQLRETURN ret = 0;
  int bytes = 0;

  sprintf_s(buffer, "{ CALL pSelect_SFDelay( %d ) }", dwSerial);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }
  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == SQL_SUCCESS_WITH_INFO)
      {
        char *writePtr = reinterpret_cast<char *>(pSFDelay);
        while (true)
        {
          ret = SQLGetData(m_hStmtSelect, 1u, static_cast<SQLSMALLINT>(65534), chunk, 1024, &indicator);
          if (ret == SQL_NO_DATA)
          {
            break;
          }
          if (indicator == SQL_NULL_DATA)
          {
            ErrorMsgLog(ret, buffer, "SQLGetData", m_hStmtSelect);
            ErrorAction(ret, m_hStmtSelect);
            if (m_hStmtSelect)
            {
              SQLCloseCursor(m_hStmtSelect);
            }
            return 1;
          }

          bytes = ret ? 1024 : static_cast<int>(indicator);
          memcpy(writePtr, chunk, bytes);
          writePtr += bytes;
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

bool CRFWorldDatabase::Insert_SFDelayInfo(unsigned int dwSerial, _worlddb_sf_delay_info *pSFDelay)
{
  char buffer[272]{};
  sprintf_s(buffer, "{ CALL pInsert_SFDelay( %d, ? ) }", dwSerial);
  return ExecUpdateBinaryQuery(buffer, reinterpret_cast<char *>(pSFDelay), 130, false);
}

unsigned __int8 CRFWorldDatabase::Select_Start_NpcQuest_History_Count(unsigned int dwSerial, unsigned int *pdwCount)
{
  char buffer[260]{};
  SQLLEN indicator = 0;
  SQLRETURN ret = 0;

  *pdwCount = 0;
  sprintf_s(
    buffer,
    sizeof(buffer),
    "declare @out_amount int  exec pSelect_Start_Npc_Quest_History_Count %d, @s_count = @out_amount output  select @out_amount",
    dwSerial);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }
  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == SQL_SUCCESS_WITH_INFO)
      {
        ret = SQLGetData(m_hStmtSelect, 1u, 4, pdwCount, 0, &indicator);
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

unsigned __int8 CRFWorldDatabase::Select_Start_NpcQuest_History(
  unsigned int dwSerial,
  _worlddb_start_npc_quest_complete_history *pNpcQHis,
  unsigned int dwCount)
{
  char buffer[260]{};
  SQLLEN indicator = 0;
  SQLRETURN ret = 0;

  sprintf_s(buffer, sizeof(buffer), "{ CALL pSelect_Start_Npc_Quest_History( %d ) }", dwSerial);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }
  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      for (unsigned int j = 0; j < dwCount; ++j)
      {
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != SQL_SUCCESS_WITH_INFO)
        {
          break;
        }
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_CHAR, pNpcQHis->List[j].szQuestCode, 64, &indicator);
        ret = SQLGetData(m_hStmtSelect, 2u, SQL_C_UTINYINT, &pNpcQHis->List[j].byLevel, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 3u, SQL_C_SBIGINT, &pNpcQHis->List[j].nEndTime, 0, &indicator);
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

    ErrorMsgLog(ret, buffer, "SQLExecDirectA", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
    return 1;
  }

  ErrFmtLog("ReConnectDataBase Fail. Query : %s", buffer);
  return 1;
}

int CRFWorldDatabase::Select_Supplement(
  unsigned int dwSerial,
  _worlddb_character_supplement_info *pSupplement)
{
  char buffer[88]{};
  SQLLEN indicator = 0;
  SQLRETURN ret = 0;

  sprintf_s(buffer, sizeof(buffer), "{ CALL pSelect_Supplement_DataInteg( %d ) }", dwSerial);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }
  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == SQL_SUCCESS_WITH_INFO)
      {
        ret = SQLGetData(m_hStmtSelect, 1u, 8, &pSupplement->dPvpPointLeak, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 2u, static_cast<SQLSMALLINT>(65529), &pSupplement->bLastAttBuff, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 3u, SQL_C_UBIGINT, &pSupplement->dwBufPotionEndTime, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 4u, 4, &pSupplement->dwRaceBuffClear, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 5u, SQL_C_UBIGINT, &pSupplement->dwGuildEntryDelay, 0, &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          6u,
          static_cast<SQLSMALLINT>(65530),
          &pSupplement->byPlayerInteg,
          0,
          &indicator);
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
      // narrowing cast for thunk return parity
      return static_cast<int>(result);
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

int CRFWorldDatabase::Select_Supplement_Ex(
  unsigned int dwSerial,
  _worlddb_character_supplement_info *pSupplement)
{
  char buffer[88]{};
  SQLLEN indicator = 0;
  SQLRETURN ret = 0;
  int voted = 0;
  int voteEnable = 0;
  __int64 scannerCnt = 0;

  sprintf_s(
    buffer,
    sizeof(buffer),
    "{ CALL pSelect_Supplement_Ex_20080707( %d, %d) }",
    dwSerial,
    g_Main.m_dwServerResetToken);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }
  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == SQL_SUCCESS_WITH_INFO)
      {
        ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_LONG, &pSupplement->dwAccumPlayTime, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 2u, SQL_C_LONG, &pSupplement->dwLastResetDate, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 3u, SQL_C_LONG, &voted, 0, &indicator);
        pSupplement->byVoted = static_cast<unsigned __int8>(voted);
        ret = SQLGetData(m_hStmtSelect, 4u, SQL_C_LONG, &voteEnable, 0, &indicator);
        pSupplement->VoteEnable = static_cast<unsigned __int8>(voteEnable);
        ret = SQLGetData(m_hStmtSelect, 5u, SQL_C_SBIGINT, &scannerCnt, 0, &indicator);
        pSupplement->dwScanerCnt = static_cast<unsigned __int64>(scannerCnt);
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
      // narrowing cast for thunk return parity
      return static_cast<int>(result);
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

int CRFWorldDatabase::Select_Supplement_ActPoint(
  unsigned int dwSerial,
  _worlddb_character_supplement_info *pSupplement)
{
  char buffer[88]{};
  SQLLEN indicator = 0;
  SQLRETURN ret = 0;

  sprintf_s(buffer, sizeof(buffer), "{ CALL pSelect_Supplement_Ex_20080930( %d) }", dwSerial);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }
  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == SQL_SUCCESS_WITH_INFO)
      {
        for (int j = 0; j < 3; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, j + 1, 4, &pSupplement->dwActionPoint[j], 0, &indicator);
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
      // narrowing cast for thunk return parity
      return static_cast<int>(result);
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

unsigned __int8 CRFWorldDatabase::Select_Trade(
  unsigned __int8 byType,
  unsigned int dwSerial,
  unsigned __int8 byRace,
  _worlddb_trade_info *pTradeData)
{
  char buffer[1048]{};
  SQLLEN indicator = 0;
  SQLRETURN ret = 0;
  unsigned short startTimeFields[24]{};
  unsigned short resultTimeFields[24]{};
  tm timeValue{};

  if (!pTradeData)
  {
    return 2;
  }

  sprintf_s(
    buffer,
    "select top %u r.[state], s.[serial], i.[inveninx], s.[price], s.[regdate], s.[sellturm], r.[buyer], r.[tax], r.[resu"
    "ltdate], b.[Name], b.[Account] from [dbo].[tbl_utsellinfo] as s join [dbo].[tbl_utresultinfo] as r on s.[type] = %u "
    "and s.[owner] = %u and s.[race] = %u and s.[type] = r.[type] and s.[serial] = r.[serial] and r.[state] in ( 1, 2, 4,"
    " 6, 10, 13 ) join [dbo].[tbl_utsingleiteminfo] as i on r.[serial]  = i.[serial] left join [dbo].[tbl_base] as b on r"
    ".[buyer] = b.[serial] order by s.[regdate]",
    20,
    byType,
    dwSerial,
    byRace);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }
  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      pTradeData->dwCnt = 0;
      do
      {
        double priceValue = 0.0;
        ret = SQLFetch(m_hStmtSelect);
        if (ret && ret != SQL_SUCCESS_WITH_INFO)
        {
          break;
        }
        ret = SQLGetData(
          m_hStmtSelect,
          1u,
          static_cast<SQLSMALLINT>(65530),
          &pTradeData->list[pTradeData->dwCnt],
          0,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          2u,
          SQL_C_LONG,
          &pTradeData->list[pTradeData->dwCnt].dwRegistSerial,
          0,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          3u,
          static_cast<SQLSMALLINT>(65530),
          &pTradeData->list[pTradeData->dwCnt].byInvenIndex,
          0,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          4u,
          SQL_C_DOUBLE,
          &priceValue,
          0,
          &indicator);
        pTradeData->list[pTradeData->dwCnt].dwPrice = static_cast<unsigned int>(priceValue);
        ret = SQLGetData(m_hStmtSelect, 5u, 93, startTimeFields, 0, &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          6u,
          static_cast<SQLSMALLINT>(65530),
          &pTradeData->list[pTradeData->dwCnt].bySellTurm,
          0,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          7u,
          4,
          &pTradeData->list[pTradeData->dwCnt].dwBuyerSerial,
          0,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          8u,
          4,
          &pTradeData->list[pTradeData->dwCnt].dwTax,
          0,
          &indicator);
        ret = SQLGetData(m_hStmtSelect, 9u, 93, resultTimeFields, 0, &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          10,
          1,
          pTradeData->list[pTradeData->dwCnt].wszBuyerName,
          17,
          &indicator);
        ret = SQLGetData(
          m_hStmtSelect,
          11,
          1,
          pTradeData->list[pTradeData->dwCnt].szBuyerAccount,
          13,
          &indicator);

        timeValue.tm_year = startTimeFields[0] - 1900;
        timeValue.tm_mon = startTimeFields[1] - 1;
        timeValue.tm_mday = startTimeFields[2];
        timeValue.tm_hour = startTimeFields[3];
        timeValue.tm_min = startTimeFields[4];
        timeValue.tm_sec = startTimeFields[5];
        timeValue.tm_isdst = -1;
        __int64 timestamp = mktime_3(&timeValue);
        if (timestamp == -1)
        {
          timestamp = 0;
        }
        pTradeData->list[pTradeData->dwCnt].tStartTime = timestamp;

        timeValue.tm_year = resultTimeFields[0] - 1900;
        timeValue.tm_mon = resultTimeFields[1] - 1;
        timeValue.tm_mday = resultTimeFields[2];
        timeValue.tm_hour = resultTimeFields[3];
        timeValue.tm_min = resultTimeFields[4];
        timeValue.tm_sec = resultTimeFields[5];
        timeValue.tm_isdst = -1;
        timestamp = mktime_3(&timeValue);
        if (timestamp == -1)
        {
          timestamp = 0;
        }
        pTradeData->list[pTradeData->dwCnt++].tResultTime = timestamp;
      }
      while (pTradeData->dwCnt < 10);

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

unsigned __int8 CRFWorldDatabase::Select_AccountTrunk(
  unsigned int dwAccountSerial,
  unsigned __int8 byRace,
  _worlddb_trunk_info *pTrunkData)
{
  char buffer[260]{};
  SQLLEN indicator = 0;
  SQLRETURN ret = 0;
  int column = 0;

  sprintf_s(buffer, "{ CALL pSelect_AccountTrunk_TrunkInteg_%d( %d ) }", byRace, dwAccountSerial);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }
  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == SQL_SUCCESS_WITH_INFO)
      {
        ret = SQLGetData(m_hStmtSelect, ++column, 1, pTrunkData, 13, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++column, 8, &pTrunkData->dDalant, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++column, 8, &pTrunkData->dGold, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++column, static_cast<SQLSMALLINT>(65530), &pTrunkData->byHintIndex, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++column, 1, pTrunkData->wszHintAnswer, 17, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++column, static_cast<SQLSMALLINT>(65530), &pTrunkData->bySlotNum, 0, &indicator);
        for (int j = 0; j < 100; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, 4, &pTrunkData->trunkKey[j], 0, &indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
          ret = SQLGetData(m_hStmtSelect, ++column, static_cast<SQLSMALLINT>(65511), &pTrunkData->trunkKey[j].dwD, 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++column, 4, &pTrunkData->trunkKey[j].dwU, 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++column, static_cast<SQLSMALLINT>(65530), &pTrunkData->trunkKey[j].byRace, 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++column, static_cast<SQLSMALLINT>(65511), &pTrunkData->trunkKey[j].lnUID, 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++column, 4, &pTrunkData->trunkKey[j].dwT, 0, &indicator);
        }
        ret = SQLGetData(m_hStmtSelect, ++column, static_cast<SQLSMALLINT>(65530), &pTrunkData->byTrunkInteg, 0, &indicator);
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

bool CRFWorldDatabase::Insert_AccountTrunk(unsigned int dwAccountSerial)
{
  char buffer[272]{};
  sprintf_s(buffer, "{ CALL pInsert_AccountTrunk( %d ) }", dwAccountSerial);
  return ExecUpdateQuery(buffer, 1);
}

unsigned __int8 CRFWorldDatabase::Select_AccountTrunkExtend(unsigned int dwAccountSerial, _worlddb_trunk_info *pTrunkData)
{
  char buffer[260]{};
  SQLLEN indicator = 0;
  SQLRETURN ret = 0;
  int column = 0;

  sprintf_s(buffer, "{ CALL pSelect_AccountTrunk_Extend(%d) }", dwAccountSerial);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }
  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == SQL_SUCCESS_WITH_INFO)
      {
        ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_UTINYINT, &pTrunkData->byExtSlotNum, 0, &indicator);
        for (int j = 0; j < 40; ++j)
        {
          __int64 itemData = 0;
          __int64 itemSerial = 0;
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_LONG, &pTrunkData->ExttrunkKey[j].lK, 0, &indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_SBIGINT, &itemData, 0, &indicator);
          pTrunkData->ExttrunkKey[j].dwD = static_cast<unsigned __int64>(itemData);
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_LONG, &pTrunkData->ExttrunkKey[j].dwU, 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_UTINYINT, &pTrunkData->ExttrunkKey[j].byRace, 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_SBIGINT, &itemSerial, 0, &indicator);
          pTrunkData->ExttrunkKey[j].lnUID = static_cast<unsigned __int64>(itemSerial);
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_LONG, &pTrunkData->ExttrunkKey[j].dwT, 0, &indicator);
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

bool CRFWorldDatabase::Insert_AccountTrunkExtend(unsigned int dwAccountSerial)
{
  char buffer[272]{};
  sprintf_s(buffer, "{ CALL pInsert_AccountTrunkExtend( %d ) }", dwAccountSerial);
  return ExecUpdateQuery(buffer, 1);
}

unsigned __int8 CRFWorldDatabase::Select_UserInterface(unsigned int dwSerial, _worlddb_userinterface_info *pUserinterfaceInfo)
{
  char buffer[260]{};
  SQLLEN indicator = 0;
  SQLRETURN ret = 0;
  int column = 0;

  sprintf_s(buffer, "{ CALL pSelect_UserInterface0513( %d ) }", dwSerial);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }
  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == SQL_SUCCESS_WITH_INFO)
      {
        for (int j = 0; j < 50; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_SHORT, &pUserinterfaceInfo->wLinkBoard[j], 0, &indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
        }
        for (int j = 0; j < 8; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_LONG, &pUserinterfaceInfo->dwDamageCForce[j], 0, &indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
        }
        for (int j = 0; j < 8; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_LONG, &pUserinterfaceInfo->dwHelpCForce[j], 0, &indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
        }
        for (int j = 0; j < 2; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_LONG, &pUserinterfaceInfo->dwSkill[j], 0, &indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
        }
        for (int j = 0; j < 2; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_LONG, &pUserinterfaceInfo->dwForce[j], 0, &indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
        }
        for (int j = 0; j < 2; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_LONG, &pUserinterfaceInfo->dwCharacter[j], 0, &indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
        }
        for (int j = 0; j < 2; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_LONG, &pUserinterfaceInfo->dwAnimus[j], 0, &indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
        }
        ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_LONG, &pUserinterfaceInfo->dwInven, 0, &indicator);
        for (int j = 0; j < 5; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_LONG, &pUserinterfaceInfo->dwInvenBag[j], 0, &indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
        }
        ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_UTINYINT, &pUserinterfaceInfo->byLinkBoardLock, 0, &indicator);
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

bool CRFWorldDatabase::Insert_UserInterface(unsigned int dwSerial)
{
  char buffer[272]{};
  sprintf_s(buffer, "{ CALL pInsert_UserInterface( %d ) }", dwSerial);
  return ExecUpdateQuery(buffer, 1);
}

unsigned __int8 CRFWorldDatabase::Select_Unit(unsigned int dwCharacterSerial, _worlddb_unit_info_array *pUnitInfo)
{
  char buffer[260]{};
  SQLLEN indicator = 0;
  SQLRETURN ret = 0;
  int column = 0;

  sprintf_s(buffer, "{ CALL pSelect_Unit( %d ) }", dwCharacterSerial);
  if (m_bSaveDBLog)
  {
    Log(buffer);
  }
  if (m_hStmtSelect || ReConnectDataBase())
  {
    ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(buffer), -3);
    if (!ret || ret == SQL_SUCCESS_WITH_INFO)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (!ret || ret == SQL_SUCCESS_WITH_INFO)
      {
        for (int j = 0; j < 4; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_UTINYINT, &pUnitInfo->UnitInfo[j].byFrame, 0, &indicator);
        }
        for (int j = 0; j < 4; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_UTINYINT, pUnitInfo->UnitInfo[j].byPart, 0, &indicator);
        }
        for (int j = 0; j < 4; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_UTINYINT, &pUnitInfo->UnitInfo[j].byPart[1], 0, &indicator);
        }
        for (int j = 0; j < 4; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_UTINYINT, &pUnitInfo->UnitInfo[j].byPart[2], 0, &indicator);
        }
        for (int j = 0; j < 4; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_UTINYINT, &pUnitInfo->UnitInfo[j].byPart[3], 0, &indicator);
        }
        for (int j = 0; j < 4; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_UTINYINT, &pUnitInfo->UnitInfo[j].byPart[4], 0, &indicator);
        }
        for (int j = 0; j < 4; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_UTINYINT, &pUnitInfo->UnitInfo[j].byPart[5], 0, &indicator);
        }
        for (int j = 0; j < 4; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_LONG, &pUnitInfo->UnitInfo[j].dwGauge, 0, &indicator);
        }
        for (int j = 0; j < 4; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_LONG, pUnitInfo->UnitInfo[j].dwBullet, 0, &indicator);
        }
        for (int j = 0; j < 4; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_LONG, &pUnitInfo->UnitInfo[j].dwBullet[1], 0, &indicator);
        }
        for (int j = 0; j < 4; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_LONG, &pUnitInfo->UnitInfo[j].nKeepingFee, 0, &indicator);
        }
        for (int j = 0; j < 4; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_LONG, &pUnitInfo->UnitInfo[j].nPullingFee, 0, &indicator);
        }
        for (int j = 0; j < 4; ++j)
        {
          __int64 cutTime = 0;
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_SBIGINT, &cutTime, 0, &indicator);
          pUnitInfo->UnitInfo[j].dwCutTime = static_cast<unsigned __int64>(cutTime);
        }
        for (int k = 0; k < 8; ++k)
        {
          for (int j = 0; j < 4; ++j)
          {
            ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_LONG, &pUnitInfo->UnitInfo[j].dwSpare[k], 0, &indicator);
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

bool CRFWorldDatabase::Insert_Unit(unsigned int dwCharacterSerial)
{
  char buffer[272]{};
  sprintf_s(buffer, "{ CALL pInsert_Unit( %d ) }", dwCharacterSerial);
  return ExecUpdateQuery(buffer, 1);
}

