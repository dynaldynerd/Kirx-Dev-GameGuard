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
          ret = SQLGetData(m_hStmtSelect, ++colIndex, 4, &pBuddyData->BuddyData[j], 0, &indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
          ret = SQLGetData(m_hStmtSelect, ++colIndex, 1, pBuddyData->BuddyData[j].wszName, 17, &indicator);
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
  unsigned __int64 animusExp = 0;

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
        ret = SQLGetData(m_hStmtSelect, colIndex, static_cast<SQLSMALLINT>(65518), pCharacterData, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, static_cast<SQLSMALLINT>(65518), &pCharacterData->dwFP, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, static_cast<SQLSMALLINT>(65518), &pCharacterData->dwSP, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, 8, &pCharacterData->dExp, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, 8, &pCharacterData->dLoseExp, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, 5, &pCharacterData->byMaxLevel, 0, &indicator);
        if (pCharacterData->byMaxLevel < 50)
        {
          pCharacterData->byMaxLevel = 50;
        }
        ret = SQLGetData(m_hStmtSelect, ++colIndex, 5, &pCharacterData->byBagNum, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, 5, &pCharacterData->byMapCode, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, 7, pCharacterData->fStartPos, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, 7, &pCharacterData->fStartPos[1], 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, 7, &pCharacterData->fStartPos[2], 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, static_cast<SQLSMALLINT>(65518), &pCharacterData->dwTotalPlayMin, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, 1, pCharacterData->szLeftResList, 160, &indicator);
        for (int j = 0; j < 7; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++colIndex, 4, &pCharacterData->lEK[j], 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++colIndex, 5, &pCharacterData->wED[j], 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++colIndex, static_cast<SQLSMALLINT>(65511), &pCharacterData->lnUID_E[j], 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++colIndex, 4, &pCharacterData->dwET[j], 0, &indicator);
        }
        for (int j = 0; j < 88; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++colIndex, 4, &pCharacterData->lF[j], 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++colIndex, static_cast<SQLSMALLINT>(65511), &pCharacterData->lnUID_F[j], 0, &indicator);
        }
        for (int j = 0; j < 2; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++colIndex, 4, &pCharacterData->dwWM[j], 0, &indicator);
        }
        for (int j = 0; j < 24; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++colIndex, 4, &pCharacterData->dwFM[j], 0, &indicator);
        }
        for (int j = 0; j < 48; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++colIndex, 4, &pCharacterData->dwSM[j], 0, &indicator);
        }
        for (int j = 0; j < 3; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++colIndex, 4, &pCharacterData->dwMI[j], 0, &indicator);
        }
        ret = SQLGetData(m_hStmtSelect, ++colIndex, 4, &pCharacterData->dwSR, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, 4, &pCharacterData->dwDM, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, 4, &pCharacterData->dwPM, 0, &indicator);
        for (int j = 0; j < 4; ++j)
        {
          animusExp = 0;
          ret = SQLGetData(m_hStmtSelect, ++colIndex, static_cast<SQLSMALLINT>(65508), &pCharacterData->byAK[j], 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++colIndex, static_cast<SQLSMALLINT>(65511), &animusExp, 0, &indicator);
          pCharacterData->dwAD[j] = animusExp;
          ret = SQLGetData(m_hStmtSelect, ++colIndex, 4, &pCharacterData->dwAP[j], 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++colIndex, static_cast<SQLSMALLINT>(65511), &pCharacterData->lnUID_A[j], 0, &indicator);
        }
        for (int j = 0; j < 3; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++colIndex, 5, &pCharacterData->zClassHistory[j], 0, &indicator);
        }
        ret = SQLGetData(m_hStmtSelect, ++colIndex, static_cast<SQLSMALLINT>(65518), &pCharacterData->dwClassInitCnt, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, static_cast<SQLSMALLINT>(65508), &pCharacterData->byLastClassGrade, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, 8, &pCharacterData->dPvPPoint, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, 8, &pCharacterData->dPvPCashBag, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, 1, pCharacterData->szBindMapCode, 12, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, 1, pCharacterData->szBindDummy, 12, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, static_cast<SQLSMALLINT>(65518), &pCharacterData->dwGuildSerial, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, static_cast<SQLSMALLINT>(65508), &pCharacterData->byGuildGrade, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, static_cast<SQLSMALLINT>(65518), &pCharacterData->dwDP, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, static_cast<SQLSMALLINT>(65518), &pCharacterData->dwRadarDelayTime, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, static_cast<SQLSMALLINT>(65518), &pCharacterData->dwTakeLastMentalTicket, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, ++colIndex, static_cast<SQLSMALLINT>(65518), &pCharacterData->dwTakeLastCriTicket, 0, &indicator);

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
          ret = SQLGetData(m_hStmtSelect, ++colIndex, static_cast<SQLSMALLINT>(65530), &questData->questList[j], 0, &indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
          ret = SQLGetData(m_hStmtSelect, ++colIndex, 5, &questData->questList[j].wIndex, 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++colIndex, 5, questData->questList[j].wNum, 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++colIndex, 5, &questData->questList[j].wNum[1], 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++colIndex, 5, &questData->questList[j].wNum[2], 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++colIndex, 4, &questData->questList[j].dwPassSec, 0, &indicator);
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
        ret = SQLGetData(m_hStmtSelect, 1u, static_cast<SQLSMALLINT>(65518), pSerial, 0, &indicator);
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
          ret = SQLGetData(m_hStmtSelect, 2u, static_cast<SQLSMALLINT>(65518), &pdwPunishment[targetValue[0]], 0, &indicator);
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
        ret = SQLGetData(m_hStmtSelect, 1u, 1, &pNpcQHis->List[j], 64, &indicator);
        ret = SQLGetData(m_hStmtSelect, 2u, static_cast<SQLSMALLINT>(65530), &pNpcQHis->List[j].byLevel, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 3u, static_cast<SQLSMALLINT>(65509), &pNpcQHis->List[j].nEndTime, 0, &indicator);
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
        ret = SQLGetData(m_hStmtSelect, 1u, 4, &pSupplement->dwAccumPlayTime, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 2u, 4, &pSupplement->dwLastResetDate, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 3u, static_cast<SQLSMALLINT>(65530), &pSupplement->byVoted, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 4u, static_cast<SQLSMALLINT>(65530), &pSupplement->VoteEnable, 0, &indicator);
        ret = SQLGetData(m_hStmtSelect, 5u, static_cast<SQLSMALLINT>(65509), &pSupplement->dwScanerCnt, 0, &indicator);
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
          static_cast<SQLSMALLINT>(65518),
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
          static_cast<SQLSMALLINT>(65518),
          &pTradeData->list[pTradeData->dwCnt].dwPrice,
          0,
          &indicator);
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

  sprintf_s(buffer, "{ CALL pSelect_AccountTrunk_20061115_%d( %d ) }", byRace, dwAccountSerial);
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
        ret = SQLGetData(m_hStmtSelect, ++column, static_cast<SQLSMALLINT>(65530), &pTrunkData->byExtSlotNum, 0, &indicator);
        for (int j = 0; j < 40; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, 4, &pTrunkData->ExttrunkKey[j], 0, &indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
          ret = SQLGetData(m_hStmtSelect, ++column, static_cast<SQLSMALLINT>(65511), &pTrunkData->ExttrunkKey[j].dwD, 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++column, 4, &pTrunkData->ExttrunkKey[j].dwU, 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++column, static_cast<SQLSMALLINT>(65530), &pTrunkData->ExttrunkKey[j].byRace, 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++column, static_cast<SQLSMALLINT>(65511), &pTrunkData->ExttrunkKey[j].lnUID, 0, &indicator);
          ret = SQLGetData(m_hStmtSelect, ++column, 4, &pTrunkData->ExttrunkKey[j].dwT, 0, &indicator);
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
          ret = SQLGetData(m_hStmtSelect, ++column, 5, &pUserinterfaceInfo->wLinkBoard[j], 0, &indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
        }
        for (int j = 0; j < 8; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, 4, &pUserinterfaceInfo->dwDamageCForce[j], 0, &indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
        }
        for (int j = 0; j < 8; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, 4, &pUserinterfaceInfo->dwHelpCForce[j], 0, &indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
        }
        for (int j = 0; j < 2; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, 4, &pUserinterfaceInfo->dwSkill[j], 0, &indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
        }
        for (int j = 0; j < 2; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, 4, &pUserinterfaceInfo->dwForce[j], 0, &indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
        }
        for (int j = 0; j < 2; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, 4, &pUserinterfaceInfo->dwCharacter[j], 0, &indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
        }
        for (int j = 0; j < 2; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, 4, &pUserinterfaceInfo->dwAnimus[j], 0, &indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
        }
        ret = SQLGetData(m_hStmtSelect, ++column, 4, &pUserinterfaceInfo->dwInven, 0, &indicator);
        for (int j = 0; j < 5; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, 4, &pUserinterfaceInfo->dwInvenBag[j], 0, &indicator);
          if (ret && ret != SQL_SUCCESS_WITH_INFO)
          {
            break;
          }
        }
        ret = SQLGetData(m_hStmtSelect, ++column, static_cast<SQLSMALLINT>(65508), &pUserinterfaceInfo->byLinkBoardLock, 0, &indicator);
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
          ret = SQLGetData(m_hStmtSelect, ++column, 5, &pUnitInfo->UnitInfo[j], 0, &indicator);
        }
        for (int j = 0; j < 4; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, 5, pUnitInfo->UnitInfo[j].byPart, 0, &indicator);
        }
        for (int j = 0; j < 4; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, 5, &pUnitInfo->UnitInfo[j].byPart[1], 0, &indicator);
        }
        for (int j = 0; j < 4; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, 5, &pUnitInfo->UnitInfo[j].byPart[2], 0, &indicator);
        }
        for (int j = 0; j < 4; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, 5, &pUnitInfo->UnitInfo[j].byPart[3], 0, &indicator);
        }
        for (int j = 0; j < 4; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, 5, &pUnitInfo->UnitInfo[j].byPart[4], 0, &indicator);
        }
        for (int j = 0; j < 4; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, 5, &pUnitInfo->UnitInfo[j].byPart[5], 0, &indicator);
        }
        for (int j = 0; j < 4; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, 4, &pUnitInfo->UnitInfo[j].dwGauge, 0, &indicator);
        }
        for (int j = 0; j < 4; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, 4, pUnitInfo->UnitInfo[j].dwBullet, 0, &indicator);
        }
        for (int j = 0; j < 4; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, 4, &pUnitInfo->UnitInfo[j].dwBullet[1], 0, &indicator);
        }
        for (int j = 0; j < 4; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, 4, &pUnitInfo->UnitInfo[j].nKeepingFee, 0, &indicator);
        }
        for (int j = 0; j < 4; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, 4, &pUnitInfo->UnitInfo[j].nPullingFee, 0, &indicator);
        }
        for (int j = 0; j < 4; ++j)
        {
          ret = SQLGetData(m_hStmtSelect, ++column, SQL_C_UBIGINT, &pUnitInfo->UnitInfo[j].dwCutTime, 0, &indicator);
        }
        for (int k = 0; k < 8; ++k)
        {
          for (int j = 0; j < 4; ++j)
          {
            ret = SQLGetData(m_hStmtSelect, ++column, 4, &pUnitInfo->UnitInfo[j].dwSpare[k], 0, &indicator);
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

