#include "pch.h"

#include "CMainThread.h"
#include "CRFWorldDatabase.h"
#include "CPotionMgr.h"
#include "PatriarchElectProcessor.h"
#include "WorldServerUtil.h"
#include "worlddb_guild_member_info.h"
#include "worlddb_guild_money_io_info.h"
#include "worlddb_rankinguild_info.h"

#include <cstring>
#include <cstdio>
#include <atlcomtime.h>
#include <ctime>
#include <sql.h>
#include <sqlext.h>

bool CRFWorldDatabase::Add_PvpPoint(
        unsigned int dwSerial,
        unsigned int dwPoint,
        unsigned int dwCashBag)
{
  char Buffer[272]; // [rsp+40h] [rbp-128h] BYREF
  memset(Buffer, 0, 256);
  sprintf_s(Buffer, "{ CALL pAdd_PvpPoint( %d, %d, %d ) }", dwSerial, dwPoint, dwCashBag);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Check_GuildMemberCount( unsigned int dwGuildSerial)
{
  SQLLEN strLenOrIndPtr;
  SQLRETURN sqlStatus;
  char queryBuffer[276];
  int memberCount = 0;

  sprintf_s(queryBuffer, "select MemberCount from tbl_guild where serial = %d", dwGuildSerial);
  if ( this->m_bSaveDBLog )
    this->Log( queryBuffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    sqlStatus = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)queryBuffer, -3);
    if ( !sqlStatus || sqlStatus == 1 )
    {
      sqlStatus = SQLFetch(this->m_hStmtSelect);
      if ( !sqlStatus || sqlStatus == 1 )
      {
        sqlStatus = SQLGetData(this->m_hStmtSelect, 1u, 5, &memberCount, 0LL, &strLenOrIndPtr);
        if ( !sqlStatus || sqlStatus == 1 )
        {
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          return memberCount < 50;
        }
        else
        {
          this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLGetData", this->m_hStmtSelect);
          this->ErrorAction( sqlStatus, this->m_hStmtSelect);
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          return 0;
        }
      }
      else
      {
        if ( sqlStatus != 100 )
        {
          this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction( sqlStatus, this->m_hStmtSelect);
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return 0;
      }
    }
    else
    {
      if ( sqlStatus != 100 )
      {
        this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLExecDirect", this->m_hStmtSelect);
        this->ErrorAction( sqlStatus, this->m_hStmtSelect);
      }
      return 0;
    }
  }
  else
  {
    this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", queryBuffer);
    return 0;
  }
}

bool CRFWorldDatabase::CreateCharacterSelectLogTable(const char *szTableName)
{
  char Buffer[3088]; // [rsp+80h] [rbp-C28h] BYREF
  std::memset(Buffer, 0, 3072);
  sprintf_s(
    Buffer,
    "CREATE TABLE [dbo].[%s] ([ID] [int] IDENTITY (1, 1) NOT NULL ,[AccountSerial] [int] NOT NULL ,[Account] [varchar] (1"
    "7) NOT NULL ,[CharacSerial] [int] NOT NULL ,[CharacName] [varchar] (17) NOT NULL ,[LogDate] [datetime] NOT NULL ) ON"
    " [PRIMARY] ALTER TABLE [dbo].[%s] WITH NOCHECK ADD CONSTRAINT [PK_%s] PRIMARY KEY  CLUSTERED ([ID]) ON [PRIMARY] ALT"
    "ER TABLE [dbo].[%s] WITH NOCHECK ADD CONSTRAINT [DF_%s_AccountSerial] DEFAULT (0) FOR [AccountSerial], CONSTRAINT [D"
    "F_%s_Account] DEFAULT ('*') FOR [Account], CONSTRAINT [DF_%s_CharacName] DEFAULT ('*') FOR [CharacName], CONSTRAINT "
    "[DF_%s_LogDate] DEFAULT (0) FOR [LogDate] CREATE  INDEX [IX_%s_account_logdate] ON [dbo].[%s]([Account], [LogDate]) "
    "ON [PRIMARY] CREATE  INDEX [IX_%s_characname_logdate] ON [dbo].[%s]([CharacName], [LogDate]) ON [PRIMARY] ",
    szTableName,
    szTableName,
    szTableName,
    szTableName,
    szTableName,
    szTableName,
    szTableName,
    szTableName,
    szTableName,
    szTableName,
    szTableName,
    szTableName);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Delete_CharacterData( unsigned int dwCharacterSerial)
{
  char queryBuffer[260];
  SQLRETURN sqlStatus;

  sprintf_s(queryBuffer, "{ CALL pDelete_Base( %d ) }", dwCharacterSerial);
  if ( this->m_bSaveDBLog )
    this->FmtLog( "Query : %s", queryBuffer);
  if ( this->m_hStmtUpdate || this->ReConnectDataBase() )
  {
    sqlStatus = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)queryBuffer, -3);
    if ( !sqlStatus || sqlStatus == 1 )
    {
      this->SetAutoCommitMode( 0);
      sprintf_s(queryBuffer, "{ CALL pUpdate_DeleteName_Step1( %d ) }", dwCharacterSerial);
      sqlStatus = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)queryBuffer, -3);
      if ( !sqlStatus || sqlStatus == 1 )
      {
        sprintf_s(queryBuffer, "{ CALL pUpdate_DeleteName_Step2( %d ) }", dwCharacterSerial);
        sqlStatus = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)queryBuffer, -3);
        if ( !sqlStatus || sqlStatus == 1 )
        {
          this->CommitTransaction();
          this->SetAutoCommitMode( 1);
          if ( this->m_bSaveDBLog )
            this->Log( "Query Success");
          return 1;
        }
        else
        {
          this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLExecDirectA", this->m_hStmtUpdate);
          this->RollbackTransaction();
          this->SetAutoCommitMode( 1);
          this->ErrorAction( sqlStatus, this->m_hStmtUpdate);
          return 0;
        }
      }
      else
      {
        this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLExecDirectA", this->m_hStmtUpdate);
        this->RollbackTransaction();
        this->SetAutoCommitMode( 1);
        this->ErrorAction( sqlStatus, this->m_hStmtUpdate);
        return 0;
      }
    }
    else
    {
      this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLExecDirectA", this->m_hStmtUpdate);
      this->ErrorAction( sqlStatus, this->m_hStmtUpdate);
      return 0;
    }
  }
  else
  {
    this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", queryBuffer);
    return 0;
  }
}

bool CRFWorldDatabase::Delete_Guild( unsigned int dwGuildSerial)
{
  char Buffer[272]; // [rsp+30h] [rbp-128h] BYREF
  memset(Buffer, 0, 256);
  sprintf_s(Buffer, "{ CALL pUpdate_DeleteGuild(%d) }", dwGuildSerial);
  return this->ExecUpdateQuery( Buffer, 1);
}

unsigned __int8 CRFWorldDatabase::Delete_PatriarchComm( unsigned int dwSerial, char *pszDepDate)
{
  char Buffer[144]; // [rsp+30h] [rbp-A8h] BYREF
  sprintf_s(Buffer, "{ CALL pDelete_PatriarchComm( %d, '%s') }", dwSerial, pszDepDate);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Insert_CashLimSale()
{
  char Buffer[1040]; // [rsp+30h] [rbp-428h] BYREF
  memset(Buffer, 0, 1024);
  sprintf_s(Buffer, 1024, "{ CALL pInsert_CashLimSale( %d ) }", 0);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Insert_CharacterData(
        char *pwszCharacterName,
        char *wszClassCode,
        unsigned int dwAccountSerial,
        char *wszAccount,
        unsigned __int8 bySlotIndex,
        unsigned __int8 byRaceSexCode,
        unsigned int dwBaseShape,
        int nMapIndex,
        unsigned int *pDwSerial)
{
  SQLRETURN sqlStatus;
  char queryBuffer[288];
  char stepQuery[272];

  sprintf_s(
    queryBuffer,
    "{ CALL pInsert_Base(N'%s', '%s', %d, '%s', %d, %d, %d ) }",
    pwszCharacterName,
    wszClassCode,
    dwAccountSerial,
    wszAccount,
    bySlotIndex,
    byRaceSexCode,
    dwBaseShape);
  if ( this->m_bSaveDBLog )
    this->Log( queryBuffer);
  if ( !this->m_hStmtUpdate && !this->ReConnectDataBase() )
  {
    this->ErrLog( "ReConnectDataBase Fail. Query : Insert_CharacterData");
    return 0;
  }
  this->SetAutoCommitMode( 0);
  sqlStatus = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)queryBuffer, -3);
  if ( sqlStatus && sqlStatus != 1 )
  {
    this->ErrorMsgLog( sqlStatus, queryBuffer, "_SQLExecDirect", this->m_hStmtUpdate);
    this->RollbackTransaction();
    this->SetAutoCommitMode( 1);
    this->ErrorAction( sqlStatus, this->m_hStmtUpdate);
    return 0;
  }
  if ( this->m_bSaveDBLog )
    this->FmtLog( "%s Success", queryBuffer);
  if ( !this->Select_CharacterSerial( pwszCharacterName, pDwSerial) )
  {
    this->RollbackTransaction();
    this->SetAutoCommitMode( 1);
    return 0;
  }
  sprintf_s(stepQuery, "{ CALL pInsert_General( %d, %d ) }", *pDwSerial, nMapIndex);
  if ( this->m_bSaveDBLog )
    this->Log( stepQuery);
  sqlStatus = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)stepQuery, -3);
  if ( sqlStatus && sqlStatus != 1 )
  {
    this->ErrorMsgLog( sqlStatus, stepQuery, "SQLExecDirectA", this->m_hStmtUpdate);
    this->RollbackTransaction();
    this->SetAutoCommitMode( 1);
    this->ErrorAction( sqlStatus, this->m_hStmtUpdate);
    return 0;
  }
  if ( this->m_bSaveDBLog )
    this->FmtLog( "%s Success", stepQuery);
  sprintf_s(stepQuery, "{ CALL pInsert_supplement( %d ) }", *pDwSerial);
  if ( this->m_bSaveDBLog )
    this->Log( stepQuery);
  sqlStatus = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)stepQuery, -3);
  if ( sqlStatus && sqlStatus != 1 )
  {
    this->ErrorMsgLog( sqlStatus, stepQuery, "SQLExecDirectA", this->m_hStmtUpdate);
    this->RollbackTransaction();
    this->SetAutoCommitMode( 1);
    this->ErrorAction( sqlStatus, this->m_hStmtUpdate);
    return 0;
  }
  if ( this->m_bSaveDBLog )
    this->FmtLog( "%s Success", stepQuery);
  sprintf_s(stepQuery, "{ CALL pInsert_inven( %d ) }", *pDwSerial);
  if ( this->m_bSaveDBLog )
    this->Log( stepQuery);
  sqlStatus = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)stepQuery, -3);
  if ( sqlStatus && sqlStatus != 1 )
  {
    this->ErrorMsgLog( sqlStatus, stepQuery, "SQLExecDirectA", this->m_hStmtUpdate);
    this->RollbackTransaction();
    this->SetAutoCommitMode( 1);
    this->ErrorAction( sqlStatus, this->m_hStmtUpdate);
    return 0;
  }
  if ( this->m_bSaveDBLog )
    this->FmtLog( "%s Success", stepQuery);
  sprintf_s(stepQuery, "{ CALL pInsert_Quest( %d ) }", *pDwSerial);
  if ( this->m_bSaveDBLog )
    this->Log( stepQuery);
  sqlStatus = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)stepQuery, -3);
  if ( sqlStatus && sqlStatus != 1 )
  {
    this->ErrorMsgLog( sqlStatus, stepQuery, "SQLExecDirectA", this->m_hStmtUpdate);
    this->RollbackTransaction();
    this->SetAutoCommitMode( 1);
    this->ErrorAction( sqlStatus, this->m_hStmtUpdate);
    return 0;
  }
  if ( this->m_bSaveDBLog )
    this->FmtLog( "%s Success", stepQuery);
  sprintf_s(stepQuery, "{ CALL pInsert_UserInterface( %d ) }", *pDwSerial);
  if ( this->m_bSaveDBLog )
    this->Log( stepQuery);
  sqlStatus = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)stepQuery, -3);
  if ( sqlStatus && sqlStatus != 1 )
  {
    this->ErrorMsgLog( sqlStatus, stepQuery, "SQLExecDirectA", this->m_hStmtUpdate);
    this->RollbackTransaction();
    this->SetAutoCommitMode( 1);
    this->ErrorAction( sqlStatus, this->m_hStmtUpdate);
    return 0;
  }
  if ( this->m_bSaveDBLog )
    this->FmtLog( "%s Success", stepQuery);
  if ( !((int)byRaceSexCode >> 1) )
  {
    sprintf_s(stepQuery, "{ CALL pInsert_Unit( %d ) }", *pDwSerial);
    if ( this->m_bSaveDBLog )
      this->Log( stepQuery);
    sqlStatus = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)stepQuery, -3);
    if ( sqlStatus && sqlStatus != 1 )
    {
      this->ErrorMsgLog( sqlStatus, stepQuery, "SQLExecDirectA", this->m_hStmtUpdate);
      this->RollbackTransaction();
      this->SetAutoCommitMode( 1);
      this->ErrorAction( sqlStatus, this->m_hStmtUpdate);
      return 0;
    }
    if ( this->m_bSaveDBLog )
      this->FmtLog( "%s Success", stepQuery);
  }
  sprintf_s(stepQuery, "{ CALL pInsert_CombineEx_Result( %d ) }", *pDwSerial);
  if ( this->m_bSaveDBLog )
    this->Log( stepQuery);
  sqlStatus = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)stepQuery, -3);
  if ( !sqlStatus || sqlStatus == 1 )
  {
    if ( this->m_bSaveDBLog )
      this->FmtLog( "%s Success", stepQuery);
    sprintf_s(stepQuery, "{ CALL pInsert_OreCutting( %d ) }", *pDwSerial);
    if ( this->m_bSaveDBLog )
      this->Log( stepQuery);
    sqlStatus = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)stepQuery, -3);
    if ( !sqlStatus || sqlStatus == 1 )
    {
      if ( this->m_bSaveDBLog )
        this->FmtLog( "%s Success", stepQuery);
      this->CommitTransaction();
      this->SetAutoCommitMode( 1);
      return 1;
    }
    else
    {
      this->ErrorMsgLog( sqlStatus, stepQuery, "SQLExecDirectA", this->m_hStmtUpdate);
      this->RollbackTransaction();
      this->SetAutoCommitMode( 1);
      this->ErrorAction( sqlStatus, this->m_hStmtUpdate);
      return 0;
    }
  }
  else
  {
    this->ErrorMsgLog( sqlStatus, stepQuery, "SQLExecDirectA", this->m_hStmtUpdate);
    this->RollbackTransaction();
    this->SetAutoCommitMode( 1);
    this->ErrorAction( sqlStatus, this->m_hStmtUpdate);
    return 0;
  }
}

bool CRFWorldDatabase::Insert_Economy_History(unsigned int dwDate, _worlddb_economy_history_info *pEconomyData)
{
  char Buffer[544]; // [rsp+F0h] [rbp-318h] BYREF
  _worlddb_economy_history_info pEconomyDataa; // [rsp+310h] [rbp-F8h] BYREF
  unsigned __int8 exist; // [rsp+3E4h] [rbp-24h]
  memset(Buffer, 0, 512);
  exist = this->Select_Exist_Economy( dwDate, &pEconomyDataa);
  if ( exist == 2 )
  {
    sprintf_s(
      Buffer,
      "{ CALL pInsert_Economy_History( %d, %f, %f, %f, %f, %f, %f, %d, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f"
      ", %f, %f, %f, %f, %f ) }",
      dwDate,
      (double)pEconomyData->dTradeDalant[0],
      (double)pEconomyData->dTradeGold[0],
      (double)pEconomyData->dTradeDalant[1],
      (double)pEconomyData->dTradeGold[1],
      (double)pEconomyData->dTradeDalant[2],
      (double)pEconomyData->dTradeGold[2],
      pEconomyData->dwManageValue,
      (double)pEconomyData->dMineOre[0][0],
      (double)pEconomyData->dMineOre[1][0],
      (double)pEconomyData->dMineOre[2][0],
      (double)pEconomyData->dMineOre[0][1],
      (double)pEconomyData->dMineOre[1][1],
      (double)pEconomyData->dMineOre[2][1],
      (double)pEconomyData->dMineOre[0][2],
      (double)pEconomyData->dMineOre[1][2],
      (double)pEconomyData->dMineOre[2][2],
      (double)pEconomyData->dCutOre[0][0],
      (double)pEconomyData->dCutOre[1][0],
      (double)pEconomyData->dCutOre[2][0],
      (double)pEconomyData->dCutOre[0][1],
      (double)pEconomyData->dCutOre[1][1],
      (double)pEconomyData->dCutOre[2][1],
      (double)pEconomyData->dCutOre[0][2],
      (double)pEconomyData->dCutOre[1][2],
      (double)pEconomyData->dCutOre[2][2]);
    return this->ExecUpdateQuery( Buffer, 1);
  }
  else if ( exist )
  {
    return 0;
  }
  else
  {
    sprintf_s(
      Buffer,
      "{ CALL pUpdate_Economy_History( %d, %f, %f, %f, %f, %f, %f, %d, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f"
      ", %f, %f, %f, %f, %f ) }",
      dwDate,
      (double)pEconomyData->dTradeDalant[0],
      (double)pEconomyData->dTradeGold[0],
      (double)pEconomyData->dTradeDalant[1],
      (double)pEconomyData->dTradeGold[1],
      (double)pEconomyData->dTradeDalant[2],
      (double)pEconomyData->dTradeGold[2],
      pEconomyData->dwManageValue,
      (double)pEconomyData->dMineOre[0][0],
      (double)pEconomyData->dMineOre[1][0],
      (double)pEconomyData->dMineOre[2][0],
      (double)pEconomyData->dMineOre[0][1],
      (double)pEconomyData->dMineOre[1][1],
      (double)pEconomyData->dMineOre[2][1],
      (double)pEconomyData->dMineOre[0][2],
      (double)pEconomyData->dMineOre[1][2],
      (double)pEconomyData->dMineOre[2][2],
      (double)pEconomyData->dCutOre[0][0],
      (double)pEconomyData->dCutOre[1][0],
      (double)pEconomyData->dCutOre[2][0],
      (double)pEconomyData->dCutOre[0][1],
      (double)pEconomyData->dCutOre[1][1],
      (double)pEconomyData->dCutOre[2][1],
      (double)pEconomyData->dCutOre[0][2],
      (double)pEconomyData->dCutOre[1][2],
      (double)pEconomyData->dCutOre[2][2]);
    return this->ExecUpdateQuery( Buffer, 1);
  }
}

bool CRFWorldDatabase::Insert_GuidRoom(
        unsigned int dwGuildSerial,
        unsigned __int8 byRoomType,
        unsigned __int8 byRace)
{
  int Year; // eax
  char Buffer[280]; // [rsp+70h] [rbp-158h] BYREF
  ATL::CTime result;
  int Second; // [rsp+1A0h] [rbp-28h]
  int Minute; // [rsp+1A4h] [rbp-24h]
  int Hour; // [rsp+1A8h] [rbp-20h]
  int Day; // [rsp+1ACh] [rbp-1Ch]
  int Month; // [rsp+1B0h] [rbp-18h]
  std::memset(Buffer, 0, 256);
  result = ATL::CTime::GetCurrentTime();
  Second = result.GetSecond();
  Minute = result.GetMinute();
  Hour = result.GetHour();
  Day = result.GetDay();
  Month = result.GetMonth();
  Year = result.GetYear();
  sprintf_s(
    Buffer,
    "insert into tbl_GuildRoom(dck, guildserial, roomtype, roomrace, roomrentdate, logdate) values(0, %d, %d, %d, '%04d-%"
    "02d-%02d %02d:%02d:%02d.000', getdate())",
    dwGuildSerial,
    byRoomType,
    byRace,
    Year,
    Month,
    Day,
    Hour,
    Minute,
    Second);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Insert_Guild( char *pwszGuildName, unsigned __int8 byRace)
{
  char Buffer[528]; // [rsp+30h] [rbp-228h] BYREF
  std::memset(Buffer, 0, 512);
  sprintf_s(Buffer, "{ CALL pInsert_Guild('%s', %d) }", pwszGuildName, byRace);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Insert_GuildMoneyHistory(
        unsigned int dwGuildSerial,
        long double dInoutDalant,
        long double dInoutGold,
        long double dResultDalant,
        long double dResultGold,
        char *wszDate,
        unsigned int dwAvatorSerial,
        const char *pwszName)
{
  char Buffer[1040]; // [rsp+60h] [rbp-428h] BYREF
  memset(Buffer, 0, 1024);
  sprintf_s(
    Buffer,
    "{ CALL pInsert_GuildMoneyHistory_Log( %d, %.0f, %.0f, %.0f, %.0f, '%s', %d, '%s' ) }",
    dwGuildSerial,
    (double)dInoutDalant,
    (double)dInoutGold,
    (double)dResultDalant,
    (double)dResultGold,
    wszDate,
    dwAvatorSerial,
    pwszName);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Insert_ItemChargeInGame(
        unsigned int dwAvatorSerial,
        unsigned int dwItemCode_K,
        unsigned int dwItemCode_D,
        unsigned int dwItemCode_U,
        unsigned __int8 byType)
{
  char Buffer[1040]; // [rsp+50h] [rbp-428h] BYREF
  sprintf_s(
    Buffer,
    1024,
    "{ CALL pInsert_ItemChargeInGame( %u, %d, %d, %d, %u ) }",
    dwAvatorSerial,
    dwItemCode_K,
    dwItemCode_D,
    dwItemCode_U,
    byType);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Insert_Level_Log(
        unsigned int dwCharacterSerial,
        unsigned __int8 byLevel,
        unsigned int dwTotalPlayMin)
{
  char Buffer[272]; // [rsp+40h] [rbp-128h] BYREF
  sprintf_s(Buffer, "{ CALL pInsert_Level_Log( %d, %d, %d ) }", dwCharacterSerial, byLevel, dwTotalPlayMin);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Insert_MacroData( unsigned int dwSerial)
{
  int j; // [rsp+90h] [rbp-448h]
  bool updated; // [rsp+94h] [rbp-444h]
  char Buffer[1040]; // [rsp+B0h] [rbp-428h] BYREF
  updated = 0;
  memset(Buffer, 0, 1024);
  for ( j = 0; j < 3; ++j )
  {
    std::memset(Buffer, 0, 1024);
    sprintf_s(
      Buffer,
      "Insert into tbl_Macro values(%d, %d,%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '', '', '', '', '', 0, 0, 0)",
      dwSerial,
      j,
      -1,
      -1,
      -1,
      -1,
      -1,
      -1,
      -1,
      -1,
      -1,
      -1,
      -1,
      -1,
      -1);
    updated = this->ExecUpdateQuery( Buffer, 1);
    if ( !updated )
      return 0;
  }
  return 1;
}

bool CRFWorldDatabase::Insert_RFEvent_ClassRefine( unsigned int dwAvatorSerial)
{
  char Buffer[272]; // [rsp+30h] [rbp-128h] BYREF
  memset(Buffer, 0, 256);
  sprintf_s(Buffer, 256, "insert [dbo].[tbl_event] (avatorserial) values (%u)", dwAvatorSerial);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Insert_Set_Limit_Run( unsigned __int8 *pData, int iSize)
{
  char _Dest[1024]; // [rsp+40h] [rbp-428h] BYREF
  memset(_Dest, 0, sizeof(_Dest));
  sprintf_s(_Dest, sizeof(_Dest), "insert into [dbo].[tbl_sf_delay] ( [aserial], [effect] ) values (0, ?)");
  return this->ExecUpdateBinaryQuery(_Dest, reinterpret_cast<char *>(pData), iSize, 1);
}

bool CRFWorldDatabase::Insert_UserNum_Log( int nAvgUserNum, int nMaxUserNum)
{
  char Buffer[272]; // [rsp+30h] [rbp-128h] BYREF
  sprintf_s(Buffer, "{ CALL pInsert_UserNum_Log( %d, %d ) }", nAvgUserNum, nMaxUserNum);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Insert_WeeklyGuildPvpPointSum( unsigned int dwSerial)
{
  char Buffer[272]; // [rsp+30h] [rbp-128h] BYREF
  std::memset(Buffer, 0, 256);
  sprintf_s(Buffer, "{ CALL pInsert_WeeklyGuildPVPPointSum(%u) }", dwSerial);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::InsertChangeClassLogAfterInitClass(
        unsigned int dwCharacSerial,
        unsigned __int8 byType,
        char *szPrevClass,
        char *szNextClass,
        int nClassInitCnt,
        unsigned __int8 byLastClassGrade,
        unsigned __int16 dwYear,
        unsigned __int8 byMonth,
        unsigned __int8 byDay,
        unsigned __int8 byHour,
        unsigned __int8 byMin,
        unsigned __int8 bySec)
{
  char Buffer[1040]; // [rsp+80h] [rbp-438h] BYREF
  sprintf_s(
    Buffer,
    "{ CALL pInsert_ClassLogAfterInitClass ( %d, %d, '%s', '%s', %d, %d, '%04d-%02d-%02d %02d:%02d:%02d' ) }",
    dwCharacSerial,
    byType,
    szPrevClass,
    szNextClass,
    nClassInitCnt,
    byLastClassGrade,
    dwYear,
    byMonth,
    byDay,
    byHour,
    byMin,
    bySec);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::InsertCharacterSelectLog(
        unsigned int dwAccountSerial,
        char *wszAccount,
        unsigned int dwCharacSerial,
        char *pwszCharacName,
        unsigned __int16 dwYear,
        unsigned __int8 byMonth,
        unsigned __int8 byDay,
        unsigned __int8 byHour,
        unsigned __int8 byMin,
        unsigned __int8 bySec)
{
  char Buffer[1040]; // [rsp+80h] [rbp-438h] BYREF
  std::memset(Buffer, 0, 1024);
  sprintf_s(
    Buffer,
    "insert into tbl_characterselect_log_%04d%02d ( AccountSerial, Account, CharacSerial, CharacName, LogDate ) values ( "
    "%d, '%s', %d, '%s', '%04d-%02d-%02d %02d:%02d:%02d' )",
    dwYear,
    byMonth,
    dwAccountSerial,
    wszAccount,
    dwCharacSerial,
    pwszCharacName,
    dwYear,
    byMonth,
    byDay,
    byHour,
    byMin,
    bySec);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Select_ArrangeInfo( unsigned int dwSerial)
{
  SQLLEN strLenOrIndPtr;
  SQLRETURN sqlStatus;
  char queryBuffer[260];
  unsigned __int8 arrangeInfo[28] = {};

  sprintf_s(queryBuffer, "{ CALL pSelect_ArrangeInfo( %d ) }", dwSerial);
  if ( this->m_bSaveDBLog )
    this->Log( queryBuffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    sqlStatus = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)queryBuffer, -3);
    if ( !sqlStatus || sqlStatus == 1 )
    {
      sqlStatus = SQLFetch(this->m_hStmtSelect);
      if ( !sqlStatus || sqlStatus == 1 )
      {
        sqlStatus = SQLGetData(this->m_hStmtSelect, 1u, static_cast<SQLSMALLINT>(65530), arrangeInfo, 0LL, &strLenOrIndPtr);
        if ( !sqlStatus || sqlStatus == 1 )
        {
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          if ( this->m_bSaveDBLog )
            this->FmtLog( "%s Success", queryBuffer);
          return arrangeInfo[0] == 0;
        }
        else
        {
          this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLGetData", this->m_hStmtSelect);
          this->ErrorAction( sqlStatus, this->m_hStmtSelect);
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          return 0;
        }
      }
      else
      {
        if ( sqlStatus != 100 )
        {
          this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction( sqlStatus, this->m_hStmtSelect);
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return 0;
      }
    }
    else
    {
      if ( sqlStatus != 100 )
      {
        this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLExecDirect", this->m_hStmtSelect);
        this->ErrorAction( sqlStatus, this->m_hStmtSelect);
      }
      return 0;
    }
  }
  else
  {
    this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", queryBuffer);
    return 0;
  }
}

bool CRFWorldDatabase::Select_ChracterSerialRace(
        char *pwszCharacterName,
        unsigned int *pSerial,
        unsigned __int8 *pbyRace)
{
  SQLLEN strLenOrIndPtr;
  SQLRETURN sqlStatus;
  char queryBuffer[260];

  sprintf_s(queryBuffer, "{ CALL pSelect_CharacterSerialRace('%s') }", pwszCharacterName);
  if ( this->m_bSaveDBLog )
    this->Log( queryBuffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    sqlStatus = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)queryBuffer, -3);
    if ( !sqlStatus || sqlStatus == 1 )
    {
      sqlStatus = SQLFetch(this->m_hStmtSelect);
      if ( !sqlStatus || sqlStatus == 1 )
      {
        sqlStatus = SQLGetData(this->m_hStmtSelect, 1u, static_cast<SQLSMALLINT>(65518), pSerial, 0LL, &strLenOrIndPtr);
        if ( !sqlStatus || sqlStatus == 1 )
        {
          sqlStatus = SQLGetData(this->m_hStmtSelect, 2u, static_cast<SQLSMALLINT>(65530), pbyRace, 0LL, &strLenOrIndPtr);
          if ( !sqlStatus || sqlStatus == 1 )
          {
            if ( this->m_hStmtSelect )
              SQLCloseCursor(this->m_hStmtSelect);
            if ( this->m_bSaveDBLog )
              this->FmtLog( "%s Success", queryBuffer);
            return 1;
          }
          else
          {
            this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLGetData", this->m_hStmtSelect);
            this->ErrorAction( sqlStatus, this->m_hStmtSelect);
            if ( this->m_hStmtSelect )
              SQLCloseCursor(this->m_hStmtSelect);
            return 0;
          }
        }
        else
        {
          this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLGetData", this->m_hStmtSelect);
          this->ErrorAction( sqlStatus, this->m_hStmtSelect);
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          return 0;
        }
      }
      else
      {
        if ( sqlStatus != 100 )
        {
          this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction( sqlStatus, this->m_hStmtSelect);
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return 0;
      }
    }
    else
    {
      if ( sqlStatus != 100 )
      {
        this->ErrorMsgLog( sqlStatus, queryBuffer, "_SQLExecDirect", this->m_hStmtSelect);
        this->ErrorAction( sqlStatus, this->m_hStmtSelect);
      }
      return 0;
    }
  }
  else
  {
    this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", queryBuffer);
    return 0;
  }
}

bool CRFWorldDatabase::Select_GuildSerial(
        char *pwszGuildName,
        unsigned int *pdwGuildSerial)
{
  SQLLEN strLenOrIndPtr;
  SQLRETURN sqlStatus;
  char queryBuffer[260];

  sprintf_s(queryBuffer, "{ CALL pSelect_GuildSerial('%s') }", pwszGuildName);
  if ( this->m_bSaveDBLog )
    this->Log( queryBuffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    sqlStatus = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)queryBuffer, -3);
    if ( !sqlStatus || sqlStatus == 1 )
    {
      sqlStatus = SQLFetch(this->m_hStmtSelect);
      if ( !sqlStatus || sqlStatus == 1 )
      {
        sqlStatus = SQLGetData(this->m_hStmtSelect, 1u, SQL_C_LONG, pdwGuildSerial, 0LL, &strLenOrIndPtr);
        if ( !sqlStatus || sqlStatus == 1 )
        {
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          if ( this->m_bSaveDBLog )
            this->FmtLog( "%s Success", queryBuffer);
          return 1;
        }
        else
        {
          this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLGetData", this->m_hStmtSelect);
          this->ErrorAction( sqlStatus, this->m_hStmtSelect);
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          return 0;
        }
      }
      else
      {
        if ( sqlStatus != 100 )
        {
          this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction( sqlStatus, this->m_hStmtSelect);
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return 0;
      }
    }
    else
    {
      if ( sqlStatus != 100 )
      {
        this->ErrorMsgLog( sqlStatus, queryBuffer, "_SQLExecDirect", this->m_hStmtSelect);
        this->ErrorAction( sqlStatus, this->m_hStmtSelect);
      }
      return 0;
    }
  }
  else
  {
    this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", queryBuffer);
    return 0;
  }
}

bool CRFWorldDatabase::Update_Dalant( unsigned int dwSerial, unsigned int dwDalant)
{
  char Buffer[144]; // [rsp+30h] [rbp-A8h] BYREF
  memset(Buffer, 0, 128);
  sprintf_s(Buffer, "UPDATE tbl_base SET Dalant = %d WHERE Serial = %d", dwDalant, dwSerial);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Update_GmGreet( _qry_case_gm_greetingmsg *pSheet)
{
  char Buffer[1040]; // [rsp+30h] [rbp-428h] BYREF
  memset(Buffer, 0, 1024);
  sprintf_s(
    Buffer,
    "update tbl_GreetMsg set Name='%s', GMsg='%s' where useType=255",
    pSheet->in_gmname,
    pSheet->in_gmgreetingmsg);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Update_Gold( unsigned int dwSerial, unsigned int dwGold)
{
  char Buffer[144]; // [rsp+30h] [rbp-A8h] BYREF
  memset(Buffer, 0, 128);
  sprintf_s(Buffer, "UPDATE tbl_base SET Gold = %d WHERE Serial = %d", dwGold, dwSerial);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Update_GuildEmblem(
        unsigned int dwGuildSerial,
        long double dCurDalant,
        unsigned int dwEmblemBack,
        unsigned int dwEmblemMark)
{
  char Buffer[272]; // [rsp+50h] [rbp-128h] BYREF
  memset(Buffer, 0, 256);
  sprintf_s(
    Buffer,
    256,
    "update [dbo].[tbl_guild] set emblemBack=%d, emblemMark=%d, dalant=%f where Serial=%d",
    dwEmblemBack,
    dwEmblemMark,
    (double)dCurDalant,
    dwGuildSerial);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Update_GuildGrade()
{
  char Buffer[272]; // [rsp+30h] [rbp-128h] BYREF
  memset(Buffer, 0, 256);
  sprintf_s(
    Buffer,
    "update tbl_Guild set Grade = r.Grade from tbl_Guild as g join tbl_GuildRankToday as r on g.serial = r.serial ");
  return this->ExecUpdateQuery( Buffer, 0);
}

bool CRFWorldDatabase::Update_GuildGreet( _qry_case_guild_greetingmsg *pSheet)
{
  char Buffer[1040]; // [rsp+30h] [rbp-428h] BYREF
  memset(Buffer, 0, 1024);
  sprintf_s(Buffer, "update tbl_guild set GMsg='%s' where Serial=%d", pSheet->in_guildgreetingmsg, pSheet->in_guildserial);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Update_GuildMaster(
        unsigned int dwGuild_Serial,
        unsigned int dwGuildMaster_Serial,
        unsigned __int8 byGuildMaster_PrevGrade)
{
  char Buffer[272]; // [rsp+40h] [rbp-128h] BYREF
  sprintf_s(
    Buffer,
    "{ CALL pUpdate_GuildMaster(%d, %d, %d) }",
    dwGuild_Serial,
    dwGuildMaster_Serial,
    byGuildMaster_PrevGrade);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Update_GuildMemberCount(
        unsigned int dwGuildSerial,
        unsigned __int16 wMemberNum)
{
  char Buffer[272]; // [rsp+30h] [rbp-128h] BYREF
  memset(Buffer, 0, 256);
  sprintf_s(Buffer, "update tbl_Guild set MemberCount = %d where serial = %d", wMemberNum, dwGuildSerial);
  return this->ExecUpdateQuery( Buffer, 0);
}

bool CRFWorldDatabase::Update_GuildRoom( unsigned int dwGuildSerial)
{
  char Buffer[272]; // [rsp+30h] [rbp-128h] BYREF
  std::memset(Buffer, 0, 256);
  sprintf_s(
    Buffer,
    "update tbl_GuildRoom set dck = 1, logdate = getdate() where dck = 0 and guildserial = %d",
    dwGuildSerial);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Update_InputGuildMoney(
        unsigned int dwGuildSerial,
        unsigned int dwDalant,
        unsigned int dwGold)
{
  char Buffer[272]; // [rsp+40h] [rbp-128h] BYREF
  memset(Buffer, 0, 256);
  sprintf_s(
    Buffer,
    "update tbl_guild set dalant = dalant + %u, gold = gold + %u where Serial = %u",
    dwDalant,
    dwGold,
    dwGuildSerial);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Update_MacroData(
        unsigned int dwSerial,
        _AIOC_A_MACRODATA *pMacro)
{
  SQLRETURN sqlStatus = 0;
  char queryBuffer[2080];
  char queryFragment[516];

  for ( int beltIndex = 0; beltIndex < 3; ++beltIndex )
  {
    const unsigned int spPotionCode = beltIndex >= 1 ? 0 : pMacro->mcr_Potion[beltIndex].Potion[2];
    const unsigned int fpPotionCode = beltIndex >= 1 ? 0 : pMacro->mcr_Potion[beltIndex].Potion[1];
    const unsigned int hpPotionCode = beltIndex >= 1 ? 0 : pMacro->mcr_Potion[beltIndex].Potion[0];
    const unsigned int action3 = beltIndex >= 3 ? 0 : pMacro->mcr_Action[beltIndex].Action[3];
    const unsigned int action2 = beltIndex >= 3 ? 0 : pMacro->mcr_Action[beltIndex].Action[2];
    const unsigned int action1 = beltIndex >= 3 ? 0 : pMacro->mcr_Action[beltIndex].Action[1];
    const unsigned int action0 = beltIndex >= 3 ? 0 : pMacro->mcr_Action[beltIndex].Action[0];
    const unsigned int action7 = beltIndex >= 3 ? 0 : pMacro->mcr_Action[beltIndex].Action[7];
    const unsigned int action6 = beltIndex >= 3 ? 0 : pMacro->mcr_Action[beltIndex].Action[6];
    const unsigned int action5 = beltIndex >= 3 ? 0 : pMacro->mcr_Action[beltIndex].Action[5];
    const unsigned int action4 = beltIndex >= 3 ? 0 : pMacro->mcr_Action[beltIndex].Action[4];
    const unsigned int action9 = beltIndex >= 3 ? 0 : pMacro->mcr_Action[beltIndex].Action[9];
    const unsigned int action8 = beltIndex >= 3 ? 0 : pMacro->mcr_Action[beltIndex].Action[8];
    const char *chat4 = beltIndex >= 2 ? "" : pMacro->mcr_Chat[beltIndex].Chat[4];
    const char *chat3 = beltIndex >= 2 ? "" : pMacro->mcr_Chat[beltIndex].Chat[3];
    const char *chat2 = beltIndex >= 2 ? "" : pMacro->mcr_Chat[beltIndex].Chat[2];
    const char *chat1 = beltIndex >= 2 ? "" : pMacro->mcr_Chat[beltIndex].Chat[1];
    const char *chat0 = beltIndex >= 2 ? "" : pMacro->mcr_Chat[beltIndex].Chat[0];
    const unsigned int spPotionValue = beltIndex >= 1 ? 0 : pMacro->mcr_Potion[beltIndex].PotionValue[2];
    const unsigned int fpPotionValue = beltIndex >= 1 ? 0 : pMacro->mcr_Potion[beltIndex].PotionValue[1];
    const unsigned int hpPotionValue = beltIndex >= 1 ? 0 : pMacro->mcr_Potion[beltIndex].PotionValue[0];

    std::memset(queryBuffer, 0, 2048);
    sprintf_s(queryBuffer, " Update tbl_Macro Set hp=%d, fp=%d, sp=%d,", hpPotionCode, fpPotionCode, spPotionCode);
    std::memset(queryFragment, 0, 512);
    sprintf_s(queryFragment, " action0=%d, action1=%d, action2=%d, action3=%d,", action0, action1, action2, action3);
    std::strcat(queryBuffer, queryFragment);
    std::memset(queryFragment, 0, 512);
    sprintf_s(queryFragment, " action4=%d, action5=%d, action6=%d, action7=%d,", action4, action5, action6, action7);
    std::strcat(queryBuffer, queryFragment);
    std::memset(queryFragment, 0, 512);
    sprintf_s(queryFragment, " action8=%d, action9=%d,", action8, action9);
    std::strcat(queryBuffer, queryFragment);
    std::memset(queryFragment, 0, 512);
    sprintf_s(
      queryFragment,
      " chat0='%s', chat1='%s', chat2='%s', chat3='%s', chat4='%s',",
      chat0,
      chat1,
      chat2,
      chat3,
      chat4);
    std::strcat(queryBuffer, queryFragment);
    std::memset(queryFragment, 0, 512);
    sprintf_s(queryFragment, " hpvalue=%d, fpvalue=%d, spvalue=%d", hpPotionValue, fpPotionValue, spPotionValue);
    std::strcat(queryBuffer, queryFragment);
    std::memset(queryFragment, 0, 512);
    sprintf_s(queryFragment, " where serial=%d and belt=%d", dwSerial, beltIndex);
    std::strcat(queryBuffer, queryFragment);
    if ( this->m_bSaveDBLog )
      this->Log( queryBuffer);
    if ( !this->m_hStmtSelect && !this->ReConnectDataBase() )
    {
      this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", queryBuffer);
      return 0;
    }
    sqlStatus = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)queryBuffer, -3);
    if ( sqlStatus )
    {
      if ( sqlStatus != 1 )
        break;
    }
    if ( this->m_bSaveDBLog )
      this->FmtLog( (wchar_t *)L"%s Success", queryBuffer);
  }
  if ( !sqlStatus || sqlStatus == 1 )
    return 1;
  if ( sqlStatus != 100 )
  {
    this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLExecDirect", this->m_hStmtSelect);
    this->ErrorAction( sqlStatus, this->m_hStmtSelect);
  }
  return 0;
}

bool CRFWorldDatabase::Update_PvpPointInfo(
        unsigned int dwSerial,
        __int16 *zClass,
        long double dPvpPoint)
{
  char Buffer[272]; // [rsp+50h] [rbp-128h] BYREF
  sprintf_s(
    Buffer,
    "{ CALL pUpdate_PvpPointInfo( %d, %d, %d, %d, %f ) }",
    dwSerial,
    *zClass,
    zClass[1],
    zClass[2],
    (double)dPvpPoint);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Update_RaceGreet( _qry_case_race_greetingmsg *pSheet)
{
  char Buffer[1040]; // [rsp+40h] [rbp-428h] BYREF
  memset(Buffer, 0, 1024);
  sprintf_s(
    Buffer,
    "update tbl_GreetMsg set Name='%s', GMsg='%s' where useType=%d",
    pSheet->in_bossname,
    pSheet->in_racegreetingmsg,
    pSheet->type);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Update_RFEvent_ClassRefine(
        unsigned int dwAvatorSerial,
        unsigned __int8 byRefineCnt,
        unsigned int dwRefineDate)
{
  char Buffer[272]; // [rsp+40h] [rbp-128h] BYREF
  memset(Buffer, 0, 256);
  sprintf_s(
    Buffer,
    256,
    "update [dbo].[tbl_event] set ClassRefineCnt=%d,ClassRefineDate=%d where avatorserial = %d",
    byRefineCnt,
    dwRefineDate,
    dwAvatorSerial);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Update_Set_Limit_Run( unsigned __int8 *pData, int iSize)
{
  char _Dest[1024]; // [rsp+40h] [rbp-428h] BYREF
  memset(_Dest, 0, sizeof(_Dest));
  sprintf_s(_Dest, sizeof(_Dest), "update [dbo].[tbl_sf_delay] set [effect] = ? where [aserial] = 0");
  return this->ExecUpdateBinaryQuery(_Dest, reinterpret_cast<char *>(pData), iSize, 1);
}

bool CRFWorldDatabase::Update_SetActive(
        unsigned int dwSerial,
        char *pwszActiveName,
        unsigned __int8 bySlot)
{
  char Buffer[272]; // [rsp+40h] [rbp-128h] BYREF
  sprintf_s(Buffer, "{ CALL pUpdate_SetActive( %d, '%s', %d ) }", dwSerial, pwszActiveName, bySlot);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Update_SetGuildMoney(
        unsigned int dwGuildSerial,
        long double dDalant,
        long double dGold)
{
  char Buffer[272]; // [rsp+40h] [rbp-128h] BYREF
  memset(Buffer, 0, 256);
  sprintf_s(
    Buffer,
    256,
    "update [dbo].[tbl_guild] set [Dalant]=%f, [Gold]=%f where [Serial]=%d",
    (double)dDalant,
    (double)dGold,
    dwGuildSerial);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Update_SFDelayInfo(
        unsigned int dwSerial,
        _worlddb_sf_delay_info *pSFDelay)
{
  char Buffer[272]; // [rsp+40h] [rbp-128h] BYREF
  sprintf_s(Buffer, "{ CALL pUpdate_SFDelay( %d, ? ) }", dwSerial);
  return this->ExecUpdateBinaryQuery(Buffer, reinterpret_cast<char *>(pSFDelay), 130, 0);
}

bool CRFWorldDatabase::Update_UserGuildData(
        unsigned int dwAvatorSerial,
        unsigned int dwGuildSerial,
        unsigned __int8 byGrade)
{
  char Buffer[272]; // [rsp+40h] [rbp-128h] BYREF
  memset(Buffer, 0, 256);
  sprintf_s(Buffer, "{ CALL pUpdate_UserGuildData_20070614( %d, %d, %d ) }", dwAvatorSerial, dwGuildSerial, byGrade);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::UpdateGuildMoney(unsigned int dwSerial, long double dDalant, long double dGold)
{
  char buffer[1040]{};
  sprintf_s(buffer, "update [dbo].[tbl_guild] set Dalant = %f, Gold = %f where serial = %u", static_cast<double>(dDalant), static_cast<double>(dGold), dwSerial);
  return ExecUpdateQuery(buffer, 1);
}

bool CRFWorldDatabase::Select_AllGuildData(_worlddb_guild_info *pGuildInfo)
{
  SQLLEN strLenOrInd = 0;
  SQLRETURN ret = SQL_ERROR;
  char query[10244]{};
  int index = 0;
  short guildGrade = 0;

  sprintf_s(
    query,
    "select top %d g.serial, g.grade, g.race, id, emblemBack, emblemMark, dalant, gold, masterserial, MasterBeforeGrade, "
    "r.totwin, r.totdraw, r.totlose, GMsg from tbl_guild as g left join tbl_GuildBattleRank as r on g.serial = r.serial "
    "where g.dck=0 order by g.serial",
    500);
  if (m_bSaveDBLog)
  {
    Log(query);
  }
  if (!m_hStmtSelect && !ReConnectDataBase())
  {
    ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return false;
  }

  ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), -3);
  if (!ret || ret == SQL_SUCCESS_WITH_INFO)
  {
    while (true)
    {
      ret = SQLFetch(m_hStmtSelect);
      if (ret && ret != SQL_SUCCESS_WITH_INFO)
      {
        break;
      }
      ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_LONG, &pGuildInfo->GuildData[index], 0, &strLenOrInd);
      ret = SQLGetData(m_hStmtSelect, 2u, SQL_C_SHORT, &guildGrade, 0, &strLenOrInd);
      pGuildInfo->GuildData[index].byGuildGrade = static_cast<unsigned __int8>(guildGrade);
      ret = SQLGetData(m_hStmtSelect, 3u, SQL_C_UTINYINT, &pGuildInfo->GuildData[index].byRace, 0, &strLenOrInd);
      ret = SQLGetData(m_hStmtSelect, 4u, SQL_C_CHAR, pGuildInfo->GuildData[index].wszGuildName, 17, &strLenOrInd);
      ret = SQLGetData(m_hStmtSelect, 5u, SQL_C_LONG, &pGuildInfo->GuildData[index].dwEmblemBack, 0, &strLenOrInd);
      ret = SQLGetData(m_hStmtSelect, 6u, SQL_C_LONG, &pGuildInfo->GuildData[index].dwEmblemMark, 0, &strLenOrInd);
      ret = SQLGetData(m_hStmtSelect, 7u, SQL_C_DOUBLE, &pGuildInfo->GuildData[index].dDalant, 0, &strLenOrInd);
      ret = SQLGetData(m_hStmtSelect, 8u, SQL_C_DOUBLE, &pGuildInfo->GuildData[index].dGold, 0, &strLenOrInd);
      ret = SQLGetData(m_hStmtSelect, 9u, SQL_C_LONG, &pGuildInfo->GuildData[index].dwMasterSerial, 0, &strLenOrInd);
      ret = SQLGetData(
        m_hStmtSelect,
        10,
        SQL_C_UTINYINT,
        &pGuildInfo->GuildData[index].byMasterPrevGrade,
        0,
        &strLenOrInd);
      ret = SQLGetData(m_hStmtSelect, 11, SQL_C_LONG, &pGuildInfo->GuildData[index].dwTotWin, 0, &strLenOrInd);
      ret = SQLGetData(m_hStmtSelect, 12, SQL_C_LONG, &pGuildInfo->GuildData[index].dwTotDraw, 0, &strLenOrInd);
      ret = SQLGetData(m_hStmtSelect, 13, SQL_C_LONG, &pGuildInfo->GuildData[index].dwTotLose, 0, &strLenOrInd);
      ret = SQLGetData(
        m_hStmtSelect,
        14,
        SQL_C_CHAR,
        pGuildInfo->GuildData[index].wszGreetingMsg,
        256,
        &strLenOrInd);
      ++index;
    }
    pGuildInfo->wGuildCount = static_cast<unsigned __int16>(index);
    if (m_hStmtSelect)
    {
      SQLCloseCursor(m_hStmtSelect);
    }
    if (m_bSaveDBLog)
    {
      FmtLog("%s Success", query);
    }
    return true;
  }

  if (ret == SQL_NO_DATA)
  {
    pGuildInfo->wGuildCount = static_cast<unsigned __int16>(index);
    return true;
  }

  ErrorMsgLog(ret, query, "SQLExecDirectA", m_hStmtSelect);
  ErrorAction(ret, m_hStmtSelect);
  return false;
}

bool CRFWorldDatabase::Select_GuildMemberData(
  unsigned __int16 wMaxMember,
  unsigned int dwGuildSerial,
  _worlddb_guild_member_info *pGuildMemberInfo)
{
  SQLLEN strLenOrInd = 0;
  SQLRETURN ret = SQL_ERROR;
  char query[260]{};
  int index = 0;
  int level = 0;
  double pvpPoint[3]{};
  short guildRank = 0;

  sprintf_s(
    query,
    "select top %u g.serial, g.pvppoint, g.guildstatus, g.guildrank, b.lv, b.name from tbl_general as g join tbl_base as "
    "b on g.serial = b.serial where g.guildserial = %d and b.dck=0 order by guildstatus ",
    wMaxMember,
    dwGuildSerial);
  if (m_bSaveDBLog)
  {
    Log(query);
  }
  if (!m_hStmtSelect && !ReConnectDataBase())
  {
    ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return false;
  }

  ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), -3);
  if (!ret || ret == SQL_SUCCESS_WITH_INFO)
  {
    while (true)
    {
      pvpPoint[0] = 0.0;
      ret = SQLFetch(m_hStmtSelect);
      if (ret && ret != SQL_SUCCESS_WITH_INFO)
      {
        break;
      }

      ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_LONG, &pGuildMemberInfo->MemberData[index], 0, &strLenOrInd);
      ret = SQLGetData(m_hStmtSelect, 2u, SQL_C_DOUBLE, pvpPoint, 0, &strLenOrInd);
      ret = SQLGetData(
        m_hStmtSelect,
        3u,
        SQL_C_UTINYINT,
        &pGuildMemberInfo->MemberData[index].byClassInGuild,
        0,
        &strLenOrInd);
      ret = SQLGetData(
        m_hStmtSelect,
        4u,
        SQL_C_SHORT,
        &guildRank,
        0,
        &strLenOrInd);
      pGuildMemberInfo->MemberData[index].wRank = static_cast<unsigned __int16>(guildRank);
      ret = SQLGetData(
        m_hStmtSelect,
        5u,
        SQL_C_LONG,
        &level,
        0,
        &strLenOrInd);
      pGuildMemberInfo->MemberData[index].byLv = static_cast<unsigned __int8>(level);
      ret = SQLGetData(
        m_hStmtSelect,
        6u,
        SQL_C_CHAR,
        pGuildMemberInfo->MemberData[index].wszName,
        17,
        &strLenOrInd);
      pGuildMemberInfo->MemberData[index].dwPvpPoint = static_cast<unsigned int>(pvpPoint[0]);
      ++index;
    }

    pGuildMemberInfo->wMemberCount = static_cast<unsigned __int16>(index);
    if (m_hStmtSelect)
    {
      SQLCloseCursor(m_hStmtSelect);
    }
    if (m_bSaveDBLog)
    {
      FmtLog("%s Success", query);
    }
    return true;
  }

  if (ret != SQL_NO_DATA)
  {
    ErrorMsgLog(ret, query, "SQLExecDirect", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
  }
  return false;
}

bool CRFWorldDatabase::Select_GuildMoneyIOData(
  unsigned int dwGuildSerial,
  _worlddb_guild_money_io_info *pGuildIOData)
{
  SQLLEN strLenOrInd = 0;
  SQLRETURN ret = SQL_ERROR;
  char query[260]{};
  int index = 0;
  long double ioDalant[4]{};
  long double ioGold[4]{};
  long double leftDalant[4]{};
  long double leftGold[4]{};
  char rawDate[44]{};
  char oneToken[16]{};

  sprintf_s(
    query,
    "select top 100 InoutDalant, InoutGold, ResultDalant, ResultGold, AvatorSerial, AvatorName, LogDate from "
    "tbl_GuildMoneyHistory_Log where GuildSerial = %d order by serial desc",
    dwGuildSerial);
  if (m_bSaveDBLog)
  {
    Log(query);
  }
  if (!m_hStmtSelect && !ReConnectDataBase())
  {
    ErrFmtLog("ReConnectDataBase Fail. Query : %s", query);
    return false;
  }

  ret = SQLExecDirectA(m_hStmtSelect, reinterpret_cast<SQLCHAR *>(query), -3);
  if (!ret || ret == SQL_SUCCESS_WITH_INFO)
  {
    while (true)
    {
      ioDalant[0] = 0.0;
      ioGold[0] = 0.0;
      leftDalant[0] = 0.0;
      leftGold[0] = 0.0;
      memset(rawDate, 0, 9);

      ret = SQLFetch(m_hStmtSelect);
      if (ret && ret != SQL_SUCCESS_WITH_INFO)
      {
        break;
      }

      ret = SQLGetData(m_hStmtSelect, 1u, SQL_C_DOUBLE, ioDalant, 0, &strLenOrInd);
      ret = SQLGetData(m_hStmtSelect, 2u, SQL_C_DOUBLE, ioGold, 0, &strLenOrInd);
      ret = SQLGetData(m_hStmtSelect, 3u, SQL_C_DOUBLE, leftDalant, 0, &strLenOrInd);
      ret = SQLGetData(m_hStmtSelect, 4u, SQL_C_DOUBLE, leftGold, 0, &strLenOrInd);
      ret = SQLGetData(m_hStmtSelect, 5u, SQL_C_LONG, &pGuildIOData->IOData[index].dwIOerSerial, 0, &strLenOrInd);
      ret = SQLGetData(m_hStmtSelect, 6u, SQL_C_CHAR, &pGuildIOData->IOData[index], 17, &strLenOrInd);
      ret = SQLGetData(m_hStmtSelect, 7u, SQL_C_CHAR, rawDate, 9, &strLenOrInd);
      if (ret && ret != SQL_SUCCESS_WITH_INFO)
      {
        break;
      }

      pGuildIOData->IOData[index].dIODalant = ioDalant[0];
      pGuildIOData->IOData[index].dIOGold = ioGold[0];
      pGuildIOData->IOData[index].dLeftDalant = leftDalant[0];
      pGuildIOData->IOData[index].dLeftGold = leftGold[0];
      for (int j = 0; j < 4; ++j)
      {
        std::memset(oneToken, 0, 3uLL);
        std::memcpy(oneToken, &rawDate[2 * j], 2uLL);
        pGuildIOData->IOData[index].byDate[j] = static_cast<unsigned __int8>(atoi(oneToken));
      }
      ++index;
    }

    pGuildIOData->wRecordCount = static_cast<unsigned __int16>(index);
    if (m_hStmtSelect)
    {
      SQLCloseCursor(m_hStmtSelect);
    }
    if (m_bSaveDBLog)
    {
      FmtLog("%s Success", query);
    }
    return true;
  }

  if (ret != SQL_NO_DATA)
  {
    ErrorMsgLog(ret, query, "SQLExecDirect", m_hStmtSelect);
    ErrorAction(ret, m_hStmtSelect);
  }
  return false;
}

bool CRFWorldDatabase::Select_GuildData(
        unsigned int dwGuildSerial,
        _worlddb_guild_info::__guild_info *pGuildData)
{
  SQLLEN strLenOrIndPtr;
  SQLRETURN sqlStatus;
  char queryBuffer[260];

  sprintf_s(queryBuffer, "select serial, grade, id, dalant, gold from tbl_guild where serial = %d", dwGuildSerial);
  if ( this->m_bSaveDBLog )
    this->Log( queryBuffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    sqlStatus = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)queryBuffer, -3);
    if ( !sqlStatus || sqlStatus == 1 )
    {
      sqlStatus = SQLFetch(this->m_hStmtSelect);
      if ( !sqlStatus || sqlStatus == 1 )
      {
        sqlStatus = SQLGetData(this->m_hStmtSelect, 1u, 4, pGuildData, 0LL, &strLenOrIndPtr);
        sqlStatus = SQLGetData(this->m_hStmtSelect, 2u, 5, &pGuildData->byGuildGrade, 0LL, &strLenOrIndPtr);
        sqlStatus = SQLGetData(this->m_hStmtSelect, 3u, 1, pGuildData->wszGuildName, 17LL, &strLenOrIndPtr);
        sqlStatus = SQLGetData(this->m_hStmtSelect, 4u, 8, &pGuildData->dDalant, 0LL, &strLenOrIndPtr);
        sqlStatus = SQLGetData(this->m_hStmtSelect, 5u, 8, &pGuildData->dGold, 0LL, &strLenOrIndPtr);
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        if ( this->m_bSaveDBLog )
          this->FmtLog( "%s Success", queryBuffer);
        return true;
      }
      else
      {
        if ( sqlStatus != 100 )
        {
          this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction( sqlStatus, this->m_hStmtSelect);
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return false;
      }
    }
    else
    {
      if ( sqlStatus != 100 )
      {
        this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLExecDirectA", this->m_hStmtSelect);
        this->ErrorAction( sqlStatus, this->m_hStmtSelect);
      }
      return false;
    }
  }
  else
  {
    this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", queryBuffer);
    return false;
  }
}

bool CRFWorldDatabase::SelectAllGuildSerial(
        unsigned int *pdwCount,
        unsigned int *pdwSerial)
{
  SQLLEN strLenOrIndPtr;
  SQLRETURN sqlStatus;
  char queryBuffer[10256];

  *pdwCount = 0;
  sprintf_s(queryBuffer, "select top %u [Serial] from [dbo].[tbl_guild] where [DCK]=0 order by serial", 500);
  if ( this->m_bSaveDBLog )
    this->Log( queryBuffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    sqlStatus = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)queryBuffer, -3);
    if ( !sqlStatus || sqlStatus == 1 )
    {
      while ( 1 )
      {
        sqlStatus = SQLFetch(this->m_hStmtSelect);
        if ( sqlStatus )
        {
          if ( sqlStatus != 1 )
            break;
        }
        sqlStatus = SQLGetData(this->m_hStmtSelect, 1u, 4, &pdwSerial[*pdwCount], 0LL, &strLenOrIndPtr);
        ++*pdwCount;
      }
      if ( this->m_hStmtSelect )
        SQLCloseCursor(this->m_hStmtSelect);
      if ( this->m_bSaveDBLog )
        this->FmtLog( "%s Success", queryBuffer);
      return true;
    }
    else if ( sqlStatus == 100 )
    {
      return true;
    }
    else
    {
      this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLExecDirectA", this->m_hStmtSelect);
      this->ErrorAction( sqlStatus, this->m_hStmtSelect);
      return false;
    }
  }
  else
  {
    this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", queryBuffer);
    return false;
  }
}

bool CRFWorldDatabase::SelectAllGuildSerialGrade(
        unsigned int *pdwCount,
        unsigned int *pdwSerial,
        unsigned __int8 *pbyGrade)
{
  SQLLEN strLenOrIndPtr;
  SQLRETURN sqlStatus;
  char queryBuffer[10256];
  short guildGrade = 0;

  *pdwCount = 0;
  sprintf_s(queryBuffer, "select top %u [Serial], [Grade] from [dbo].[tbl_guild] where [DCK]=0 order by serial", 500);
  if ( this->m_bSaveDBLog )
    this->Log( queryBuffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    sqlStatus = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)queryBuffer, -3);
    if ( !sqlStatus || sqlStatus == 1 )
    {
      while ( 1 )
      {
        sqlStatus = SQLFetch(this->m_hStmtSelect);
        if ( sqlStatus )
        {
          if ( sqlStatus != 1 )
            break;
        }
        sqlStatus = SQLGetData(this->m_hStmtSelect, 1u, 4, &pdwSerial[*pdwCount], 0LL, &strLenOrIndPtr);
        sqlStatus = SQLGetData(this->m_hStmtSelect, 2u, SQL_C_SHORT, &guildGrade, 0LL, &strLenOrIndPtr);
        pbyGrade[*pdwCount] = static_cast<unsigned __int8>(guildGrade);
        ++*pdwCount;
      }
      if ( this->m_hStmtSelect )
        SQLCloseCursor(this->m_hStmtSelect);
      if ( this->m_bSaveDBLog )
        this->FmtLog( "%s Success", queryBuffer);
      return true;
    }
    else if ( sqlStatus == 100 )
    {
      return true;
    }
    else
    {
      this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLExecDirectA", this->m_hStmtSelect);
      this->ErrorAction( sqlStatus, this->m_hStmtSelect);
      return false;
    }
  }
  else
  {
    this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", queryBuffer);
    return false;
  }
}

bool CRFWorldDatabase::Update_CharSlot( unsigned int dwAvatorSerial)
{
  SQLLEN strLenOrIndPtr;
  SQLRETURN sqlStatus;
  char queryBuffer[276];
  unsigned int accountSerial = 0;
  unsigned int characterSerialBySlot[3];
  int slotIndex;

  memset(queryBuffer, 0, 256);
  sprintf_s(queryBuffer, "select AccountSerial from tbl_base where serial=%d and dck=0", dwAvatorSerial);
  if ( this->m_bSaveDBLog )
    this->Log( "Update_CharSlot");
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    sqlStatus = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)queryBuffer, -3);
    if ( !sqlStatus || sqlStatus == 1 )
    {
      sqlStatus = SQLFetch(this->m_hStmtSelect);
      if ( !sqlStatus || sqlStatus == 1 )
      {
        sqlStatus = SQLGetData(this->m_hStmtSelect, 1u, SQL_C_LONG, &accountSerial, 0LL, &strLenOrIndPtr);
        if ( !sqlStatus || sqlStatus == 1 )
        {
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          memset(characterSerialBySlot, 0, sizeof(characterSerialBySlot));
          sprintf_s(queryBuffer, "select serial from tbl_base where AccountSerial=%d and dck=0 order by slot", accountSerial);
          sqlStatus = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)queryBuffer, -3);
          if ( !sqlStatus || sqlStatus == 1 )
          {
            for ( slotIndex = 0; slotIndex < 3; ++slotIndex )
            {
              sqlStatus = SQLFetch(this->m_hStmtSelect);
              if ( sqlStatus )
              {
                if ( sqlStatus != 1 )
                  break;
              }
              sqlStatus = SQLGetData(this->m_hStmtSelect, 1u, static_cast<SQLSMALLINT>(65518), &characterSerialBySlot[slotIndex], 0LL, &strLenOrIndPtr);
            }
            if ( this->m_hStmtSelect )
              SQLCloseCursor(this->m_hStmtSelect);
            for ( slotIndex = 0; slotIndex < 3; ++slotIndex )
            {
              if ( characterSerialBySlot[slotIndex] )
              {
                sprintf_s(queryBuffer, "update tbl_base set slot=%d where serial=%d", slotIndex, characterSerialBySlot[slotIndex]);
                this->ExecUpdateQuery( queryBuffer, 1);
              }
            }
            if ( this->m_bSaveDBLog )
              this->Log( "Update_CharSlot Success");
            return true;
          }
          else
          {
            if ( sqlStatus != 100 )
            {
              this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLExecDirectA", this->m_hStmtSelect);
              this->ErrorAction( sqlStatus, this->m_hStmtSelect);
            }
            return false;
          }
        }
        else
        {
          this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLGetData", this->m_hStmtSelect);
          this->ErrorAction( sqlStatus, this->m_hStmtSelect);
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          return false;
        }
      }
      else
      {
        if ( sqlStatus != 100 )
        {
          this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction( sqlStatus, this->m_hStmtSelect);
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return false;
      }
    }
    else
    {
      if ( sqlStatus != 100 )
      {
        this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLExecDirectA", this->m_hStmtSelect);
        this->ErrorAction( sqlStatus, this->m_hStmtSelect);
      }
      return false;
    }
  }
  else
  {
    this->FmtLog( "ReConnectDataBase Fail. Query : %s", queryBuffer);
    return false;
  }
}

bool CRFWorldDatabase::Update_GuildRank(char *szDate)
{
  if (!Update_GuildRank_Step1(szDate))
  {
    return false;
  }
  if (!Update_GuildRank_Step2(szDate))
  {
    return false;
  }
  return (Update_GuildRank_Step3(szDate)) != 0;
}

bool CRFWorldDatabase::Update_GuildRank_Step1( char *szDate)
{
  SQLRETURN sqlStatus;
  char queryBuffer[1040];

  memset(queryBuffer, 0, 1024);
  this->FmtLog(
    "CRFWorldDatabase::Update_GuildRank_Step1(char* szDate(%s)) : Update_GuildRank Start!",
    szDate);
  sprintf_s(queryBuffer, 1024, "tbl_GuildRank%s", szDate);
  if ( this->TableExist( queryBuffer) )
  {
    sprintf_s(queryBuffer, 1024, "drop table [dbo].[tbl_GuildRank%s]", szDate);
    if ( !this->ExecUpdateQuery( queryBuffer, 1) )
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_GuildRank_Step1(szDate(%s)) : Drop tbl_GuildRank%s Table Fail!",
        szDate,
        szDate);
      return false;
    }
  }
  this->SetAutoCommitMode( 0);
  this->FmtLog(
    "CRFWorldDatabase::Update_GuildRank_Step1(char* szDate(%s)) : Start Create #tbl_GuildRankTemp Table",
    szDate);
  sprintf_s(
    queryBuffer,
    1024,
    "select top %d g.GuildSerial as serial, sum(b.lv) + sum(g.pvppoint)/10000 as GuildPower into #tbl_GuildRankTemp from "
    "tbl_general as g join tbl_base as b on g.serial = b.serial group by g.GuildSerial having g.guildserial >= 0",
    500);
  sqlStatus = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)queryBuffer, -3);
  if ( sqlStatus && sqlStatus != 1 )
  {
    if ( sqlStatus != 100 )
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_GuildRank_Step1(char* szDate(%s)) : Create #tbl_GuildRankTemp Table Fail SQL_ERROR!",
        szDate);
      this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLExecDirect", this->m_hStmtUpdate);
      this->RollbackTransaction();
      this->SetAutoCommitMode( 1);
      return false;
    }
    this->FmtLog(
      "CRFWorldDatabase::Update_GuildRank_Step1(char* szDate(%s)) : Create #tbl_GuildRankTemp Table Fail NO_DATA!",
      szDate);
  }
  this->FmtLog(
    "CRFWorldDatabase::Update_GuildRank_Step1(char* szDate(%s)) : End Create #tbl_GuildRankTemp Table",
    szDate);
  this->FmtLog(
    "CRFWorldDatabase::Update_GuildRank_Step1(char* szDate(%s)) : Start Create tbl_GuildRank%s Table",
    szDate,
    szDate);
  sprintf_s(
    queryBuffer,
    1024,
    "select IDENTITY(int, 1, 1) AS Rank, serial, GuildPower, -1 as Rate, 1 as Grade into [dbo].[tbl_GuildRank%s] from #tb"
    "l_GuildRankTemp order by GuildPower desc",
    szDate);
  sqlStatus = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)queryBuffer, -3);
  if ( sqlStatus && sqlStatus != 1 )
  {
    if ( sqlStatus != 100 )
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_GuildRank_Step1(char* szDate(%s)) : Create tbl_GuildRank%s Table Fail SQL_ERROR!",
        szDate,
        szDate);
      this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLExecDirect", this->m_hStmtUpdate);
      this->RollbackTransaction();
      this->SetAutoCommitMode( 1);
      return false;
    }
    this->FmtLog(
      "CRFWorldDatabase::Update_GuildRank_Step1(char* szDate(%s)) : Create tbl_GuildRank%s Table Fail NO_DATA!",
      szDate,
      szDate);
  }
  this->FmtLog(
    "CRFWorldDatabase::Update_GuildRank_Step1(char* szDate(%s)) : End Create tbl_GuildRank%s Table",
    szDate,
    szDate);
  this->FmtLog(
    "CRFWorldDatabase::Update_GuildRank_Step1(char* szDate(%s)) : Start drop table #tbl_GuildRankTemp",
    szDate);
  sprintf_s(queryBuffer, "drop table #tbl_GuildRankTemp");
  if ( this->ExecUpdateQuery( queryBuffer, 1) )
  {
    this->CommitTransaction();
    this->SetAutoCommitMode( 1);
    this->FmtLog(
      "CRFWorldDatabase::Update_GuildRank_Step1(char* szDate(%s)) : End drop table #tbl_GuildRankTemp",
      szDate);
    return true;
  }
  else
  {
    this->FmtLog(
      "CRFWorldDatabase::Update_GuildRank_Step1(char* szDate(%s)) : drop table #tbl_GuildRankTemp Fail!",
      szDate);
    this->RollbackTransaction();
    this->SetAutoCommitMode( 1);
    return false;
  }
}

bool CRFWorldDatabase::Update_GuildRank_Step2( char *szDate)
{
  char queryBuffer[1040];

  this->FmtLog( "CRFWorldDatabase::Update_GuildRank_Step2(char* szDate(%s)) : Start Update Rate", szDate);
  memset(queryBuffer, 0, 1024);
  sprintf_s(
    queryBuffer,
    1024,
    "update tbl_GuildRank%s set Rate = ( (Rank*100)/(select count(*) from tbl_GuildRank%s) )",
    szDate,
    szDate);
  if ( this->ExecUpdateQuery( queryBuffer, 0) )
  {
    sprintf_s(
      queryBuffer,
      1024,
      "update tbl_GuildRank%s set Grade = 2 where rate <= 95 and GuildPower >= %d",
      szDate,
      g_Main.m_dwGuildPower[0]);
    if ( this->ExecUpdateQuery( queryBuffer, 0) )
    {
      sprintf_s(
        queryBuffer,
        1024,
        "update tbl_GuildRank%s set Grade = 3 where rate <= 85 and GuildPower >= %d",
        szDate,
        g_Main.m_dwGuildPower[1]);
      if ( this->ExecUpdateQuery( queryBuffer, 0) )
      {
        sprintf_s(
          queryBuffer,
          1024,
          "update tbl_GuildRank%s set Grade = 4 where rate <= 65 and GuildPower >= %d",
          szDate,
          g_Main.m_dwGuildPower[2]);
        if ( this->ExecUpdateQuery( queryBuffer, 0) )
        {
          sprintf_s(
            queryBuffer,
            1024,
            "update tbl_GuildRank%s set Grade = 5 where rate <= 35 and GuildPower >= %d",
            szDate,
            g_Main.m_dwGuildPower[3]);
          if ( this->ExecUpdateQuery( queryBuffer, 0) )
          {
            sprintf_s(
              queryBuffer,
              1024,
              "update tbl_GuildRank%s set Grade = 6 where rate <= 15 and GuildPower >= %d",
              szDate,
              g_Main.m_dwGuildPower[4]);
            if ( this->ExecUpdateQuery( queryBuffer, 0) )
            {
              sprintf_s(
                queryBuffer,
                1024,
                "update tbl_GuildRank%s set Grade = 7 where rate <= 5 and GuildPower >= %d",
                szDate,
                g_Main.m_dwGuildPower[5]);
              if ( this->ExecUpdateQuery( queryBuffer, 0) )
              {
                this->FmtLog(
                  "CRFWorldDatabase::Update_GuildRank_Step2(char* szDate(%s)) : End Update Rate",
                  szDate);
                return true;
              }
              else
              {
                this->FmtLog(
                  "CRFWorldDatabase::Update_GuildRank_Step2(char* szDate(%s)) : %s Fail!",
                  szDate,
                  queryBuffer);
                return false;
              }
            }
            else
            {
              this->FmtLog(
                "CRFWorldDatabase::Update_GuildRank_Step2(char* szDate(%s)) : %s Fail!",
                szDate,
                queryBuffer);
              return false;
            }
          }
          else
          {
            this->FmtLog(
              "CRFWorldDatabase::Update_GuildRank_Step2(char* szDate(%s)) : %s Fail!",
              szDate,
              queryBuffer);
            return false;
          }
        }
        else
        {
          this->FmtLog(
            "CRFWorldDatabase::Update_GuildRank_Step2(char* szDate(%s)) : %s Fail!",
            szDate,
            queryBuffer);
          return false;
        }
      }
      else
      {
        this->FmtLog(
          "CRFWorldDatabase::Update_GuildRank_Step2(char* szDate(%s)) : %s Fail!",
          szDate,
          queryBuffer);
        return false;
      }
    }
    else
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_GuildRank_Step2(char* szDate(%s)) : %s Fail!",
        szDate,
        queryBuffer);
      return false;
    }
  }
  else
  {
    this->FmtLog(
      "CRFWorldDatabase::Update_GuildRank_Step2(char* szDate(%s)) : %s Fail!",
      szDate,
      queryBuffer);
    return false;
  }
}

bool CRFWorldDatabase::Update_GuildRank_Step3( char *szDate)
{
  char queryBuffer[1040];

  this->FmtLog(
    "CRFWorldDatabase::Update_GuildRank_Step3(char* szDate(%s)) : Start Drop tbl_GuildRankToday Table",
    szDate);
  memset(queryBuffer, 0, 1024);
  if ( !this->TableExist( "tbl_GuildRankToday")
    || (sprintf_s(queryBuffer, 1024, "Drop Table tbl_GuildRankToday"), this->ExecUpdateQuery( queryBuffer, 1)) )
  {
    this->FmtLog(
      "CRFWorldDatabase::Update_GuildRank_Step3(char* szDate(%s)) : End Drop tbl_GuildRankToday Table",
      szDate);
    this->FmtLog(
      "CRFWorldDatabase::Update_GuildRank_Step3(char* szDate(%s)) : Start Create tbl_GuildRankToday Table",
      szDate);
    sprintf_s(
      queryBuffer,
      1024,
      "select Rank, Rate, Serial, GuildPower, Grade into [dbo].[tbl_GuildRankToday] from tbl_GuildRank%s",
      szDate);
    if ( this->ExecUpdateQuery( queryBuffer, 0) )
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_GuildRank_Step3(char* szDate(%s)) : End Create tbl_GuildRankToday Table",
        szDate);
      this->FmtLog(
        "CRFWorldDatabase::Update_GuildRank_Step3(char* szDate(%s)) : Update_GuildRank Success",
        szDate);
      return true;
    }
    else
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_GuildRank_Step3(char* szDate(%s)) : %s Fail!",
        szDate,
        queryBuffer);
      return false;
    }
  }
  else
  {
    this->FmtLog(
      "CRFWorldDatabase::Update_GuildRank_Step3(char* szDate(%s)) : %s Fail!",
      szDate,
      queryBuffer);
    return false;
  }
}

bool CRFWorldDatabase::Update_RaceRank(char *szDate)
{
  if (!Update_RaceRank_Step1(szDate))
  {
    return false;
  }

  if (!Update_RaceRank_Step2(szDate))
  {
    return false;
  }

  if (!Update_RaceRank_Step3(szDate))
  {
    return false;
  }

  if (!Update_RaceRank_Step4(szDate))
  {
    return false;
  }

  if (!Update_RaceRank_Step5(szDate))
  {
    return false;
  }

  if (!Update_RaceRank_Step6(szDate))
  {
    return false;
  }

  if (!Update_RaceRank_Step7(szDate))
  {
    return false;
  }

  if (!Update_RaceRank_Step8(szDate))
  {
    return false;
  }

  return (Update_RaceRank_Step9(szDate)) != 0;
}

bool CRFWorldDatabase::Update_RaceRank_Step1( char *szDate)
{
  SQLRETURN sqlStatus;
  char queryBuffer[1028];
  unsigned long long connectTimeAddBySec;

  this->FmtLog( "CRFWorldDatabase Update_RaceRank(szDate(%s)) Start!", szDate);
  memset(queryBuffer, 0, 1024);
  if ( !this->m_hStmtUpdate && !this->ReConnectDataBase() )
  {
    this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", queryBuffer);
    return false;
  }
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step1(szDate(%s)) : Start Create #tbl_PvpRankB Table",
    szDate);
  this->SetAutoCommitMode( 0);
  connectTimeAddBySec = GetConnectTime_AddBySec(-2592000);
  sprintf_s(
    queryBuffer,
    1024,
    "select IDENTITY(int, 1, 1) AS Rank, -1 as Rate, tbl_base.serial, 0 as Race,  tbl_base.name, tbl_base.lv, tbl_general"
    ".PvpPoint, tbl_general.GuildSerial into #tbl_PvpRankB from tbl_general, tbl_base where tbl_base.serial=tbl_general.s"
    "erial and tbl_base.dck=0 and tbl_base.AccountSerial < 200000000 and tbl_general.class0 <> -1 and tbl_base.race in (0"
    ",1) and tbl_base.LastConnTime > %llu order by tbl_general.PvpPoint desc",
    connectTimeAddBySec);
  sqlStatus = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)queryBuffer, -3);
  if ( sqlStatus && sqlStatus != 1 )
  {
    if ( sqlStatus != 100 )
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_RaceRank_Step1(szDate(%s)) : Create #tbl_PvpRankB Table Fail SQL_ERROR!",
        szDate);
      this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLExecDirect", this->m_hStmtUpdate);
      this->RollbackTransaction();
      this->SetAutoCommitMode( 1);
      return false;
    }
    this->FmtLog(
      "CRFWorldDatabase::Update_RaceRank_Step1(szDate(%s)) : Create #tbl_PvpRankB Table Fail NO_DATA!",
      szDate);
  }
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step1(szDate(%s)) : End Create #tbl_PvpRankB Table",
    szDate);
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step1(szDate(%s)) : Start Set Rate #tbl_PvpRankB Table",
    szDate);
  sprintf_s(queryBuffer, "update #tbl_PvpRankB set Rate = ( (Rank*10000)/(select count(*) from #tbl_PvpRankB) )");
  sqlStatus = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)queryBuffer, -3);
  if ( sqlStatus && sqlStatus != 1 )
  {
    if ( sqlStatus != 100 )
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_RaceRank_Step1(szDate(%s)) : Set Rate #tbl_PvpRankB Table Fail SQL_ERROR!",
        szDate);
      this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLExecDirect", this->m_hStmtUpdate);
      this->RollbackTransaction();
      this->SetAutoCommitMode( 1);
      return false;
    }
    this->FmtLog(
      "CRFWorldDatabase::Update_RaceRank_Step1(szDate(%s)) : Set Rate #tbl_PvpRankB Table Fail NO_DATA!",
      szDate);
  }
  this->CommitTransaction();
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step1(szDate(%s)) : End Set Rate #tbl_PvpRankB Table",
    szDate);
  this->SetAutoCommitMode( 1);
  return true;
}

bool CRFWorldDatabase::Update_RaceRank_Step2( char *szDate)
{
  SQLRETURN sqlStatus;
  char queryBuffer[1028];
  unsigned long long connectTimeAddBySec;

  memset(queryBuffer, 0, 1024);
  if ( !this->m_hStmtUpdate && !this->ReConnectDataBase() )
  {
    this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", queryBuffer);
    return false;
  }
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step2(szDate(%s)) : Start Create #tbl_PvpRankC Table",
    szDate);
  this->SetAutoCommitMode( 0);
  connectTimeAddBySec = GetConnectTime_AddBySec(-2592000);
  sprintf_s(
    queryBuffer,
    1024,
    "select IDENTITY(int, 1, 1) AS Rank, -1 as Rate, tbl_base.serial, 1 as Race, tbl_base.name, tbl_base.lv, tbl_general."
    "PvpPoint, tbl_general.GuildSerial into #tbl_PvpRankC from tbl_general, tbl_base where tbl_base.serial=tbl_general.se"
    "rial and tbl_base.dck=0 and tbl_base.AccountSerial<200000000 and tbl_general.class0 <> -1 and tbl_base.race in (2,3)"
    " and tbl_base.LastConnTime > %llu order by tbl_general.PvpPoint desc",
    connectTimeAddBySec);
  sqlStatus = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)queryBuffer, -3);
  if ( sqlStatus && sqlStatus != 1 )
  {
    if ( sqlStatus != 100 )
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_RaceRank_Step2(szDate(%s)) : Create #tbl_PvpRankB Table Fail SQL_ERROR!",
        szDate);
      this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLExecDirect", this->m_hStmtUpdate);
      this->RollbackTransaction();
      this->SetAutoCommitMode( 1);
      if ( !this->Update_RaceRank_Step_6_1( szDate) )
        this->Update_RaceRank_Step_6_1( szDate);
      return false;
    }
    this->FmtLog(
      "CRFWorldDatabase::Update_RaceRank_Step2(szDate(%s)) : Create #tbl_PvpRankB Table Fail NO_DATA!",
      szDate);
  }
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step2(szDate(%s)) : End Create #tbl_PvpRankC Table",
    szDate);
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step2(szDate(%s)) : Start Set Rate #tbl_PvpRankC Table",
    szDate);
  sprintf_s(queryBuffer, "update #tbl_PvpRankC set Rate = ( (Rank*10000)/(select count(*) from #tbl_PvpRankC) )");
  sqlStatus = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)queryBuffer, -3);
  if ( !sqlStatus || sqlStatus == 1 || sqlStatus == 100 )
  {
    if ( sqlStatus == 100 )
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_RaceRank_Step1(szDate(%s)) : Set Rate #tbl_PvpRankC Table Fail NO_DATA!",
        szDate);
    }
    this->CommitTransaction();
    this->FmtLog(
      "CRFWorldDatabase::Update_RaceRank_Step2(szDate(%s)) : End Set Rate #tbl_PvpRankC Table",
      szDate);
    this->SetAutoCommitMode( 1);
    return true;
  }
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step1(szDate(%s)) : Set Rate #tbl_PvpRankC Table Fail SQL_ERROR!",
    szDate);
  this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLExecDirect", this->m_hStmtUpdate);
  this->RollbackTransaction();
  this->SetAutoCommitMode( 1);
  if ( !this->Update_RaceRank_Step_6_1( szDate) )
    this->Update_RaceRank_Step_6_1( szDate);
  return false;
}

bool CRFWorldDatabase::Update_RaceRank_Step3( char *szDate)
{
  SQLRETURN sqlStatus;
  char queryBuffer[1028];
  unsigned long long connectTimeAddBySec;

  memset(queryBuffer, 0, 1024);
  if ( !this->m_hStmtUpdate && !this->ReConnectDataBase() )
  {
    this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", queryBuffer);
    return false;
  }
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step2(szDate(%s)) : Start Create #tbl_PvpRankC Table",
    szDate);
  this->SetAutoCommitMode( 0);
  connectTimeAddBySec = GetConnectTime_AddBySec(-2592000);
  sprintf_s(
    queryBuffer,
    1024,
    "select IDENTITY(int, 1, 1) AS Rank, -1 as Rate, tbl_base.serial, 2 as Race,  tbl_base.name, tbl_base.lv, tbl_general"
    ".PvpPoint, tbl_general.GuildSerial into #tbl_PvpRankA from tbl_general, tbl_base where tbl_base.serial=tbl_general.s"
    "erial and tbl_base.dck=0 and tbl_base.AccountSerial<200000000 and tbl_general.class0 <> -1 and tbl_base.race in (4) "
    "and tbl_base.LastConnTime > %llu order by tbl_general.PvpPoint desc",
    connectTimeAddBySec);
  sqlStatus = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)queryBuffer, -3);
  if ( sqlStatus && sqlStatus != 1 )
  {
    if ( sqlStatus != 100 )
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_RaceRank_Step2(szDate(%s)) : Create #tbl_PvpRankB Table Fail SQL_ERROR!",
        szDate);
      this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLExecDirect", this->m_hStmtUpdate);
      this->RollbackTransaction();
      this->SetAutoCommitMode( 1);
      if ( !this->Update_RaceRank_Step_6_1( szDate) )
        this->Update_RaceRank_Step_6_1( szDate);
      if ( !this->Update_RaceRank_Step_6_2( szDate) )
        this->Update_RaceRank_Step_6_2( szDate);
      return false;
    }
    this->FmtLog(
      "CRFWorldDatabase::Update_RaceRank_Step2(szDate(%s)) : Create #tbl_PvpRankB Table Fail NO_DATA!",
      szDate);
  }
  this->SetAutoCommitMode( 1);
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step3(szDate(%s)) : End Create #tbl_PvpRankA Table",
    szDate);
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step3(szDate(%s)) : Start Set Rate #tbl_PvpRankA Table",
    szDate);
  sprintf_s(queryBuffer, "update #tbl_PvpRankA set Rate = ( (Rank*10000)/(select count(*) from #tbl_PvpRankA) )");
  sqlStatus = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)queryBuffer, -3);
  if ( !sqlStatus || sqlStatus == 1 || sqlStatus == 100 )
  {
    if ( sqlStatus == 100 )
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_RaceRank_Step1(szDate(%s)) : Set Rate #tbl_PvpRankA Table Fail NO_DATA!",
        szDate);
    }
    this->CommitTransaction();
    this->FmtLog(
      "CRFWorldDatabase::Update_RaceRank_Step3(szDate(%s)) : End Set Rate #tbl_PvpRankA Table",
      szDate);
    return true;
  }
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step1(szDate(%s)) : Set Rate #tbl_PvpRankA Table Fail SQL_ERROR!",
    szDate);
  this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLExecDirect", this->m_hStmtUpdate);
  this->RollbackTransaction();
  this->SetAutoCommitMode( 1);
  if ( !this->Update_RaceRank_Step_6_1( szDate) )
    this->Update_RaceRank_Step_6_1( szDate);
  if ( !this->Update_RaceRank_Step_6_2( szDate) )
    this->Update_RaceRank_Step_6_2( szDate);
  return false;
}

bool CRFWorldDatabase::Update_RaceRank_Step4( char *szDate)
{
  SQLRETURN sqlStatus;
  char queryBuffer[1040];

  memset(queryBuffer, 0, 1024);
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step4(szDate(%s)) : Start Create tbl_PvpRank%s Table",
    szDate,
    szDate);
  sprintf_s(queryBuffer, 1024, "tbl_PvpRank%s", szDate);
  if ( this->TableExist( queryBuffer) )
  {
    sprintf_s(queryBuffer, 1024, "drop table [dbo].[tbl_PvpRank%s]", szDate);
    if ( !this->ExecUpdateQuery( queryBuffer, 1) )
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_RaceRank_Step4(szDate(%s)) : Drop tbl_PvpRank%s Table Fail!",
        szDate,
        szDate);
      return false;
    }
  }
  this->SetAutoCommitMode( 0);
  sprintf_s(
    queryBuffer,
    1024,
    "CREATE TABLE [dbo].[tbl_PvpRank%s] ( [Rank] [int] NOT NULL, [Rate] [int] NOT NULL, [serial] [int] NOT NULL ,[name] ["
    "varchar] (17) NOT NULL ,[lv] [int] NOT NULL, [Race] [int] NOT NULL, [Grade] [smallint] NOT NULL ,[PvpPoint] [float] "
    "NOT NULL, [GuildSerial] [int] NOT NULL ,[GuildName] [varchar] (17) NOT NULL ,CONSTRAINT PK_tbl_PvpRank%s PRIMARY KEY"
    "  CLUSTERED ( [serial] ) on [PRIMARY] ) ON [PRIMARY]",
    szDate,
    szDate);
  if ( !this->ExecUpdateQuery( queryBuffer, 1) )
  {
    this->FmtLog(
      "CRFWorldDatabase::Update_RaceRank_Step4(szDate(%s)) : Create tbl_PvpRank%s Table Fail!",
      szDate,
      szDate);
    this->RollbackTransaction();
    this->SetAutoCommitMode( 1);
    return false;
  }
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step4(szDate(%s)) : End Create tbl_PvpRank%s Table",
    szDate,
    szDate);
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank(szDate(%s)) : Start Union #tbl_PvpRankB,C,A To tbl_PvpRank%s",
    szDate,
    szDate);
  sprintf_s(
    queryBuffer,
    1024,
    "insert tbl_PvpRank%s(Rank, Rate, Serial, Name, lv, race, PvpPoint, Grade, GuildSerial, GuildName) select 0, rate, se"
    "rial, name, lv, race, PvpPoint, 0, GuildSerial, '*' from #tbl_PvpRankB union select 0, rate, serial, name, lv, race,"
    " PvpPoint, 0, GuildSerial, '*' from #tbl_PvpRankC union select 0, rate, serial, name, lv, race, PvpPoint, 0, GuildSe"
    "rial, '*' from #tbl_PvpRankA ",
    szDate);
  sqlStatus = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)queryBuffer, -3);
  if ( sqlStatus && sqlStatus != 1 )
  {
    if ( sqlStatus != 100 )
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_RaceRank(szDate(%s)) : Union #tbl_PvpRankB,C,A To tbl_PvpRank%s Fail SQL_ERROR!",
        szDate,
        szDate);
      this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLExecDirect", this->m_hStmtUpdate);
      this->RollbackTransaction();
      this->SetAutoCommitMode( 1);
      this->Update_RaceRank_Step6( szDate);
      return false;
    }
    this->FmtLog(
      "CRFWorldDatabase::Update_RaceRank(szDate(%s)) : Union #tbl_PvpRankB,C,A To tbl_PvpRank%s Fail NO_DATA!",
      szDate,
      szDate);
  }
  this->CommitTransaction();
  this->SetAutoCommitMode( 1);
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank(szDate(%s)) : End Union #tbl_PvpRankB,C,A To tbl_PvpRank%s",
    szDate,
    szDate);
  return true;
}

bool CRFWorldDatabase::Update_RaceRank_Step5( char *szDate)
{
  char Buffer[1040]; // [rsp+30h] [rbp-428h] BYREF
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : Start Set Grade tbl_PvpRank%s",
    szDate,
    szDate);
  memset(Buffer, 0, 1024);
  sprintf_s(Buffer, 1024, "update tbl_PvpRank%s set grade=0", szDate);
  if ( this->ExecUpdateQuery( Buffer, 0) )
  {
    sprintf_s(Buffer, 1024, "update tbl_PvpRank%s set grade=3 where lv >= 30 and lv <= 34 and rate <= 6500", szDate);
    if ( this->ExecUpdateQuery( Buffer, 0) )
    {
      sprintf_s(
        Buffer,
        1024,
        "update tbl_PvpRank%s set grade=2 where lv >= 30 and lv <= 34 and rate > 6500 and rate <= 8500",
        szDate);
      if ( this->ExecUpdateQuery( Buffer, 0) )
      {
        sprintf_s(
          Buffer,
          1024,
          "update tbl_PvpRank%s set grade=1 where lv >= 30 and lv <= 34 and rate > 8500 and rate <= 9500",
          szDate);
        if ( this->ExecUpdateQuery( Buffer, 0) )
        {
          sprintf_s(
            Buffer,
            1024,
            "update tbl_PvpRank%s set grade=4 where lv >= 35 and lv <= 39 and rate <= 3500",
            szDate);
          if ( this->ExecUpdateQuery( Buffer, 0) )
          {
            sprintf_s(
              Buffer,
              1024,
              "update tbl_PvpRank%s set grade=3 where lv >= 35 and lv <= 39 and rate > 3500 and rate <= 6500",
              szDate);
            if ( this->ExecUpdateQuery( Buffer, 0) )
            {
              sprintf_s(
                Buffer,
                1024,
                "update tbl_PvpRank%s set grade=2 where lv >= 35 and lv <= 39 and rate > 6500 and rate <= 8500",
                szDate);
              if ( this->ExecUpdateQuery( Buffer, 0) )
              {
                sprintf_s(
                  Buffer,
                  1024,
                  "update tbl_PvpRank%s set grade=1 where lv >= 35 and lv <= 39 and rate > 8500 and rate <= 9500",
                  szDate);
                if ( this->ExecUpdateQuery( Buffer, 0) )
                {
                  sprintf_s(
                    Buffer,
                    1024,
                    "update tbl_PvpRank%s set grade=5 where lv >= 40 and lv <= 44 and rate <= 1500",
                    szDate);
                  if ( this->ExecUpdateQuery( Buffer, 0) )
                  {
                    sprintf_s(
                      Buffer,
                      1024,
                      "update tbl_PvpRank%s set grade=4 where lv >= 40 and lv <= 44 and rate > 1500 and rate <= 3500",
                      szDate);
                    if ( this->ExecUpdateQuery( Buffer, 0) )
                    {
                      sprintf_s(
                        Buffer,
                        1024,
                        "update tbl_PvpRank%s set grade=3 where lv >= 40 and lv <= 44 and rate > 3500 and rate <= 6500",
                        szDate);
                      if ( this->ExecUpdateQuery( Buffer, 0) )
                      {
                        sprintf_s(
                          Buffer,
                          1024,
                          "update tbl_PvpRank%s set grade=2 where lv >= 40 and lv <= 44 and rate > 6500 and rate <= 8500",
                          szDate);
                        if ( this->ExecUpdateQuery( Buffer, 0) )
                        {
                          sprintf_s(
                            Buffer,
                            1024,
                            "update tbl_PvpRank%s set grade=1 where lv >= 40 and lv <= 44 and rate > 8500 and rate <= 9500",
                            szDate);
                          if ( this->ExecUpdateQuery( Buffer, 0) )
                          {
                            sprintf_s(
                              Buffer,
                              1024,
                              "update tbl_PvpRank%s set grade=6 where lv >= 45 and lv <= 49 and rate <= 500",
                              szDate);
                            if ( this->ExecUpdateQuery( Buffer, 0) )
                            {
                              sprintf_s(
                                Buffer,
                                1024,
                                "update tbl_PvpRank%s set grade=5 where lv >= 45 and lv <= 49 and rate > 500 and rate <= 1500",
                                szDate);
                              if ( this->ExecUpdateQuery( Buffer, 0) )
                              {
                                sprintf_s(
                                  Buffer,
                                  1024,
                                  "update tbl_PvpRank%s set grade=4 where lv >= 45 and lv <= 49 and rate > 1500 and rate <= 3500",
                                  szDate);
                                if ( this->ExecUpdateQuery( Buffer, 0) )
                                {
                                  sprintf_s(
                                    Buffer,
                                    1024,
                                    "update tbl_PvpRank%s set grade=3 where lv >= 45 and lv <= 49 and rate > 3500 and rate <= 6500",
                                    szDate);
                                  if ( this->ExecUpdateQuery( Buffer, 0) )
                                  {
                                    sprintf_s(
                                      Buffer,
                                      1024,
                                      "update tbl_PvpRank%s set grade=2 where lv >= 45 and lv <= 49 and rate > 6500 and rate <= 8500",
                                      szDate);
                                    if ( this->ExecUpdateQuery( Buffer, 0) )
                                    {
                                      sprintf_s(
                                        Buffer,
                                        1024,
                                        "update tbl_PvpRank%s set grade=1 where lv >= 45 and lv <= 49 and rate > 8500 and rate <= 9500",
                                        szDate);
                                      if ( this->ExecUpdateQuery( Buffer, 0) )
                                      {
                                        sprintf_s(
                                          Buffer,
                                          1024,
                                          "update tbl_PvpRank%s set grade=7 where lv >= 50 and rate <= 100",
                                          szDate);
                                        if ( this->ExecUpdateQuery( Buffer, 0) )
                                        {
                                          sprintf_s(
                                            Buffer,
                                            1024,
                                            "update tbl_PvpRank%s set grade=6 where lv >= 50 and rate > 100 and rate <= 500",
                                            szDate);
                                          if ( this->ExecUpdateQuery( Buffer, 0) )
                                          {
                                            sprintf_s(
                                              Buffer,
                                              1024,
                                              "update tbl_PvpRank%s set grade=5 where lv >= 50 and rate > 500 and rate <= 1500",
                                              szDate);
                                            if ( this->ExecUpdateQuery( Buffer, 0) )
                                            {
                                              sprintf_s(
                                                Buffer,
                                                1024,
                                                "update tbl_PvpRank%s set grade=4 where lv >= 50 and rate > 1500 and rate <= 3500",
                                                szDate);
                                              if ( this->ExecUpdateQuery( Buffer, 0) )
                                              {
                                                sprintf_s(
                                                  Buffer,
                                                  1024,
                                                  "update tbl_PvpRank%s set grade=3 where lv >= 50 and rate > 3500 and rate <= 6500",
                                                  szDate);
                                                if ( this->ExecUpdateQuery( Buffer, 0) )
                                                {
                                                  sprintf_s(
                                                    Buffer,
                                                    1024,
                                                    "update tbl_PvpRank%s set grade=2 where lv >= 50 and rate > 6500 and rate <= 8500",
                                                    szDate);
                                                  if ( this->ExecUpdateQuery( Buffer, 0) )
                                                  {
                                                    sprintf_s(
                                                      Buffer,
                                                      1024,
                                                      "update tbl_PvpRank%s set grade=1 where lv >= 50 and rate > 8500 and rate <= 9500",
                                                      szDate);
                                                    if ( this->ExecUpdateQuery( Buffer, 0) )
                                                    {
                                                      this->FmtLog(
                                                        "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : End Set Gr"
                                                        "ade tbl_PvpRank%s",
                                                        szDate,
                                                        szDate);
                                                      return true;
                                                    }
                                                    else
                                                    {
                                                      this->FmtLog(
                                                        "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                                        szDate,
                                                        Buffer);
                                                      this->Update_RaceRank_Step6( szDate);
                                                      return false;
                                                    }
                                                  }
                                                  else
                                                  {
                                                    this->FmtLog(
                                                      "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                                      szDate,
                                                      Buffer);
                                                    this->Update_RaceRank_Step6( szDate);
                                                    return false;
                                                  }
                                                }
                                                else
                                                {
                                                  this->FmtLog(
                                                    "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                                    szDate,
                                                    Buffer);
                                                  this->Update_RaceRank_Step6( szDate);
                                                  return false;
                                                }
                                              }
                                              else
                                              {
                                                this->FmtLog(
                                                  "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                                  szDate,
                                                  Buffer);
                                                this->Update_RaceRank_Step6( szDate);
                                                return false;
                                              }
                                            }
                                            else
                                            {
                                              this->FmtLog(
                                                "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                                szDate,
                                                Buffer);
                                              this->Update_RaceRank_Step6( szDate);
                                              return false;
                                            }
                                          }
                                          else
                                          {
                                            this->FmtLog(
                                              "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                              szDate,
                                              Buffer);
                                            this->Update_RaceRank_Step6( szDate);
                                            return false;
                                          }
                                        }
                                        else
                                        {
                                          this->FmtLog(
                                            "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                            szDate,
                                            Buffer);
                                          this->Update_RaceRank_Step6( szDate);
                                          return false;
                                        }
                                      }
                                      else
                                      {
                                        this->FmtLog(
                                          "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                          szDate,
                                          Buffer);
                                        this->Update_RaceRank_Step6( szDate);
                                        return false;
                                      }
                                    }
                                    else
                                    {
                                      this->FmtLog(
                                        "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                        szDate,
                                        Buffer);
                                      this->Update_RaceRank_Step6( szDate);
                                      return false;
                                    }
                                  }
                                  else
                                  {
                                    this->FmtLog(
                                      "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                      szDate,
                                      Buffer);
                                    this->Update_RaceRank_Step6( szDate);
                                    return false;
                                  }
                                }
                                else
                                {
                                  this->FmtLog(
                                    "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                    szDate,
                                    Buffer);
                                  this->Update_RaceRank_Step6( szDate);
                                  return false;
                                }
                              }
                              else
                              {
                                this->FmtLog(
                                  "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                  szDate,
                                  Buffer);
                                this->Update_RaceRank_Step6( szDate);
                                return false;
                              }
                            }
                            else
                            {
                              this->FmtLog(
                                "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                szDate,
                                Buffer);
                              this->Update_RaceRank_Step6( szDate);
                              return false;
                            }
                          }
                          else
                          {
                            this->FmtLog(
                              "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                              szDate,
                              Buffer);
                            this->Update_RaceRank_Step6( szDate);
                            return false;
                          }
                        }
                        else
                        {
                          this->FmtLog(
                            "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                            szDate,
                            Buffer);
                          this->Update_RaceRank_Step6( szDate);
                          return false;
                        }
                      }
                      else
                      {
                        this->FmtLog(
                          "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                          szDate,
                          Buffer);
                        this->Update_RaceRank_Step6( szDate);
                        return false;
                      }
                    }
                    else
                    {
                      this->FmtLog(
                        "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                        szDate,
                        Buffer);
                      this->Update_RaceRank_Step6( szDate);
                      return false;
                    }
                  }
                  else
                  {
                    this->FmtLog(
                      "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                      szDate,
                      Buffer);
                    this->Update_RaceRank_Step6( szDate);
                    return false;
                  }
                }
                else
                {
                  this->FmtLog(
                    "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                    szDate,
                    Buffer);
                  this->Update_RaceRank_Step6( szDate);
                  return false;
                }
              }
              else
              {
                this->FmtLog(
                  "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                  szDate,
                  Buffer);
                this->Update_RaceRank_Step6( szDate);
                return false;
              }
            }
            else
            {
              this->FmtLog(
                "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                szDate,
                Buffer);
              this->Update_RaceRank_Step6( szDate);
              return false;
            }
          }
          else
          {
            this->FmtLog(
              "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
              szDate,
              Buffer);
            this->Update_RaceRank_Step6( szDate);
            return false;
          }
        }
        else
        {
          this->FmtLog( "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!", szDate, Buffer);
          this->Update_RaceRank_Step6( szDate);
          return false;
        }
      }
      else
      {
        this->FmtLog( "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!", szDate, Buffer);
        this->Update_RaceRank_Step6( szDate);
        return false;
      }
    }
    else
    {
      this->FmtLog( "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!", szDate, Buffer);
      this->Update_RaceRank_Step6( szDate);
      return false;
    }
  }
  else
  {
    this->FmtLog( "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!", szDate, Buffer);
    this->Update_RaceRank_Step6( szDate);
    return false;
  }
}

bool CRFWorldDatabase::Update_RaceRank_Step_6_1( char *szDate)
{
  char Buffer[1040]{};

  FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step_6_1(szDate(%s)) : Start drop #tbl_PvpRankB Table",
    szDate);
  memset(Buffer, 0, 1024);
  sprintf_s(Buffer, sizeof(Buffer), "drop table #tbl_PvpRankB");
  if (ExecUpdateQuery(Buffer, false))
  {
    FmtLog(
      "CRFWorldDatabase::Update_RaceRank_Step_6_1(szDate(%s)) : End drop #tbl_PvpRankB Table",
      szDate);
    return true;
  }

  FmtLog("CRFWorldDatabase::Update_RaceRank_Step_6_1(szDate(%s)) : %s Fail!", szDate, Buffer);
  return false;
}

bool CRFWorldDatabase::Update_RaceRank_Step_6_2( char *szDate)
{
  char Buffer[1040]{};

  FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step_6_2(szDate(%s)) : Start drop #tbl_PvpRankC Table",
    szDate);
  memset(Buffer, 0, 1024);
  sprintf_s(Buffer, sizeof(Buffer), "drop table #tbl_PvpRankC");
  if (ExecUpdateQuery(Buffer, false))
  {
    FmtLog(
      "CRFWorldDatabase::Update_RaceRank_Step_6_2(szDate(%s)) : End drop #tbl_PvpRankC Table",
      szDate);
    return true;
  }

  FmtLog("CRFWorldDatabase::Update_RaceRank_Step_6_2(szDate(%s)) : %s Fail!", szDate, Buffer);
  return false;
}

bool CRFWorldDatabase::Update_RaceRank_Step_6_3( char *szDate)
{
  char Buffer[1040]{};

  FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step_6_3(szDate(%s)) : Start drop #tbl_PvpRankA Table",
    szDate);
  memset(Buffer, 0, 1024);
  sprintf_s(Buffer, sizeof(Buffer), "drop table #tbl_PvpRankA");
  if (ExecUpdateQuery(Buffer, false))
  {
    FmtLog(
      "CRFWorldDatabase::Update_RaceRank_Step_6_3(szDate(%s)) : End drop #tbl_PvpRankA Table",
      szDate);
    return true;
  }

  FmtLog("CRFWorldDatabase::Update_RaceRank_Step_6_3(szDate(%s)) : %s Fail!", szDate, Buffer);
  return false;
}

bool CRFWorldDatabase::Update_RaceRank_Step6( char *szDate)
{
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step6(szDate(%s)) : Start drop #tbl_PvpRankB,C,A Table",
    szDate);
  if ( this->Update_RaceRank_Step_6_1( szDate) )
  {
    if ( this->Update_RaceRank_Step_6_2( szDate) )
    {
      if ( this->Update_RaceRank_Step_6_3( szDate) )
      {
        this->FmtLog(
          "CRFWorldDatabase::Update_RaceRank_Step6(szDate(%s)) : End drop #tbl_PvpRankB,C,A Table",
          szDate);
        return true;
      }
      else
      {
        this->Update_RaceRank_Step_6_3( szDate);
        return false;
      }
    }
    else
    {
      this->Update_RaceRank_Step_6_2( szDate);
      return false;
    }
  }
  else
  {
    this->Update_RaceRank_Step_6_1( szDate);
    return false;
  }
}

bool CRFWorldDatabase::Update_RaceRank_Step7( char *szDate)
{
  char Buffer[1028]; // [rsp+30h] [rbp-428h] BYREF
  int j; // [rsp+434h] [rbp-24h]
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step7(szDate(%s)) : Start Update Rank tbl_PvpRank%s Table",
    szDate,
    szDate);
  memset(Buffer, 0, 1024);
  for ( j = 0; j < 3; ++j )
  {
    sprintf_s(
      Buffer,
      "select IDENTITY(int, 1, 1) AS Rank, Serial into #tbl_PvpRank from tbl_PvpRank%s where race=%d order by Grade desc, Rate",
      szDate,
      j);
    if ( !this->ExecUpdateQuery( Buffer, 0) )
    {
      this->FmtLog( "CRFWorldDatabase::Update_RaceRank_Step7(szDate(%s)) : %s Fail!", szDate, Buffer);
      return false;
    }
    sprintf_s(
      Buffer,
      "update tbl_PvpRank%s set rank = t1.rank from(select serial, rank from #tbl_PvpRank) as t1 where tbl_PvpRank%s.serial = t1.serial",
      szDate,
      szDate);
    if ( !this->ExecUpdateQuery( Buffer, 0) )
    {
      this->FmtLog( "CRFWorldDatabase::Update_RaceRank_Step7(szDate(%s)) : %s Fail!", szDate, Buffer);
      return false;
    }
    sprintf_s(Buffer, "drop table #tbl_PvpRank");
    if ( !this->ExecUpdateQuery( Buffer, 0) )
    {
      this->FmtLog( "CRFWorldDatabase::Update_RaceRank_Step7(szDate(%s)) : %s Fail!", szDate, Buffer);
      return false;
    }
  }
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step7(szDate(%s)) : End Update Rank tbl_PvpRank%s Table",
    szDate,
    szDate);
  return true;
}

bool CRFWorldDatabase::Update_RaceRank_Step8( char *szDate)
{
  char Buffer[1040]; // [rsp+40h] [rbp-428h] BYREF
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step8(szDate(%s)) : Start Set GuildName tbl_PvpRank%s Table",
    szDate,
    szDate);
  memset(Buffer, 0, 1024);
  sprintf_s(
    Buffer,
    1024,
    "update tbl_PvpRank%s set GuildName = g.id from(select serial, id from tbl_Guild) as g where tbl_PvpRank%s.GuildSeria"
    "l = g.serial and tbl_PvpRank%s.GuildSerial > 0",
    szDate,
    szDate,
    szDate);
  if ( this->ExecUpdateQuery( Buffer, 0) )
  {
    this->FmtLog(
      "CRFWorldDatabase::Update_RaceRank_Step8(szDate(%s)) : End Set GuildName tbl_PvpRank%s Table",
      szDate,
      szDate);
    return true;
  }
  else
  {
    this->FmtLog( "CRFWorldDatabase::Update_RaceRank_Step8(szDate(%s)) : %s Fail!", szDate, Buffer);
    return false;
  }
}

bool CRFWorldDatabase::Update_RaceRank_Step9( char *szDate)
{
  char Buffer[1040]; // [rsp+30h] [rbp-428h] BYREF
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step9(szDate(%s)) : Start Drop tbl_PvpRankToday Table",
    szDate);
  memset(Buffer, 0, 1024);
  if ( !this->TableExist( "tbl_PvpRankToday")
    || (sprintf_s(Buffer, 1024, "Drop Table tbl_PvpRankToday"), this->ExecUpdateQuery( Buffer, 0)) )
  {
    this->FmtLog(
      "CRFWorldDatabase::Update_RaceRank_Step9(szDate(%s)) : End Drop tbl_PvpRankToday Table",
      szDate);
    this->FmtLog(
      "CRFWorldDatabase::Update_RaceRank_Step9(szDate(%s)) : Start Create tbl_PvpRankToday Table",
      szDate);
    sprintf_s(
      Buffer,
      1024,
      "select Rank, Rate, serial, name, lv, race, PvpPoint, Grade, GuildSerial, GuildName into [dbo].[tbl_PvpRankToday] f"
      "rom tbl_PvpRank%s",
      szDate);
    if ( this->ExecUpdateQuery( Buffer, 0) )
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_RaceRank_Step9(szDate(%s)) : End Create tbl_PvpRankToday Table",
        szDate);
      this->FmtLog( "CRFWorldDatabase Date(%s) : Update_RaceRank End Success!", szDate);
      return true;
    }
    else
    {
      this->FmtLog( "CRFWorldDatabase::Update_RaceRank_Step9(szDate(%s)) : %s Fail!", szDate, Buffer);
      return false;
    }
  }
  else
  {
    this->FmtLog( "CRFWorldDatabase::Update_RaceRank_Step9(szDate(%s)) : %s Fail!", szDate, Buffer);
    return false;
  }
}

bool CRFWorldDatabase::Update_RankInGuild_Step2( unsigned int dwGuildSerial)
{
  this->FmtLog( "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) Start!", dwGuildSerial);
  if ( this->ExecUpdateQuery(
         "update #tbl_RankInGuild set Rate = ( (Rank*10000)/(select count(*) from #tbl_RankInGuild) )",
         0) )
  {
    if ( this->ExecUpdateQuery( "update #tbl_RankInGuild set NewGrade=0", 0) )
    {
      if ( this->ExecUpdateQuery(
             "update #tbl_RankInGuild set NewGrade=3 where lv >= 30 and lv <= 34 and rate <= 6500",
             0) )
      {
        if ( this->ExecUpdateQuery(
               "update #tbl_RankInGuild set NewGrade=2 where lv >= 30 and lv <= 34 and rate > 6500 and rate <= 8500",
               0) )
        {
          if ( this->ExecUpdateQuery(
                 "update #tbl_RankInGuild set NewGrade=1 where lv >= 30 and lv <= 34 and rate > 8500 and rate <= 9500",
                 0) )
          {
            if ( this->ExecUpdateQuery(
                   "update #tbl_RankInGuild set NewGrade=4 where lv >= 35 and lv <= 39 and rate <= 3500",
                   0) )
            {
              if ( this->ExecUpdateQuery(
                     "update #tbl_RankInGuild set NewGrade=3 where lv >= 35 and lv <= 39 and rate > 3500 and rate <= 6500",
                     0) )
              {
                if ( this->ExecUpdateQuery(
                       "update #tbl_RankInGuild set NewGrade=2 where lv >= 35 and lv <= 39 and rate > 6500 and rate <= 8500",
                       0) )
                {
                  if ( this->ExecUpdateQuery(
                         "update #tbl_RankInGuild set NewGrade=1 where lv >= 35 and lv <= 39 and rate > 8500 and rate <= 9500",
                         0) )
                  {
                    if ( this->ExecUpdateQuery(
                           "update #tbl_RankInGuild set NewGrade=5 where lv >= 40 and lv <= 44 and rate <= 1500",
                           0) )
                    {
                      if ( this->ExecUpdateQuery(
                             "update #tbl_RankInGuild set NewGrade=4 where lv >= 40 and lv <= 44 and rate > 1500 and rate <= 3500",
                             0) )
                      {
                        if ( this->ExecUpdateQuery(
                               "update #tbl_RankInGuild set NewGrade=3 where lv >= 40 and lv <= 44 and rate > 3500 and rate <= 6500",
                               0) )
                        {
                          if ( this->ExecUpdateQuery(
                                 "update #tbl_RankInGuild set NewGrade=2 where lv >= 40 and lv <= 44 and rate > 6500 and rate <= 8500",
                                 0) )
                          {
                            if ( this->ExecUpdateQuery(
                                   "update #tbl_RankInGuild set NewGrade=1 where lv >= 40 and lv <= 44 and rate > 8500 and rate <= 9500",
                                   0) )
                            {
                              if ( this->ExecUpdateQuery(
                                     "update #tbl_RankInGuild set NewGrade=6 where lv >= 45 and lv <= 49 and rate <= 500",
                                     0) )
                              {
                                if ( this->ExecUpdateQuery(
                                       "update #tbl_RankInGuild set NewGrade=5 where lv >= 45 and lv <= 49 and rate > 500"
                                       " and rate <= 1500",
                                       0) )
                                {
                                  if ( this->ExecUpdateQuery(
                                         "update #tbl_RankInGuild set NewGrade=4 where lv >= 45 and lv <= 49 and rate > 1"
                                         "500 and rate <= 3500",
                                         0) )
                                  {
                                    if ( this->ExecUpdateQuery(
                                           "update #tbl_RankInGuild set NewGrade=3 where lv >= 45 and lv <= 49 and rate >"
                                           " 3500 and rate <= 6500",
                                           0) )
                                    {
                                      if ( this->ExecUpdateQuery(
                                             "update #tbl_RankInGuild set NewGrade=2 where lv >= 45 and lv <= 49 and rate"
                                             " > 6500 and rate <= 8500",
                                             0) )
                                      {
                                        if ( this->ExecUpdateQuery(
                                               "update #tbl_RankInGuild set NewGrade=1 where lv >= 45 and lv <= 49 and ra"
                                               "te > 8500 and rate <= 9500",
                                               0) )
                                        {
                                          if ( this->ExecUpdateQuery(
                                                 "update #tbl_RankInGuild set NewGrade=7 where lv >= 50 and rate <= 100",
                                                 0) )
                                          {
                                            if ( this->ExecUpdateQuery(
                                                   "update #tbl_RankInGuild set NewGrade=6 where lv >= 50 and rate > 100 and rate <= 500",
                                                   0) )
                                            {
                                              if ( this->ExecUpdateQuery(
                                                     "update #tbl_RankInGuild set NewGrade=5 where lv >= 50 and rate > 50"
                                                     "0 and rate <= 1500",
                                                     0) )
                                              {
                                                if ( this->ExecUpdateQuery(
                                                       "update #tbl_RankInGuild set NewGrade=4 where lv >= 50 and rate > "
                                                       "1500 and rate <= 3500",
                                                       0) )
                                                {
                                                  if ( this->ExecUpdateQuery(
                                                         "update #tbl_RankInGuild set NewGrade=3 where lv >= 50 and rate "
                                                         "> 3500 and rate <= 6500",
                                                         0) )
                                                  {
                                                    if ( this->ExecUpdateQuery(
                                                           "update #tbl_RankInGuild set NewGrade=2 where lv >= 50 and rat"
                                                           "e > 6500 and rate <= 8500",
                                                           0) )
                                                    {
                                                      if ( this->ExecUpdateQuery(
                                                             "update #tbl_RankInGuild set NewGrade=1 where lv >= 50 and r"
                                                             "ate > 8500 and rate <= 9500",
                                                             0) )
                                                      {
                                                        this->FmtLog(
                                                          "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) End!",
                                                          dwGuildSerial);
                                                        return true;
                                                      }
                                                      else
                                                      {
                                                        this->FmtLog(
                                                          "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) "
                                                          ") : update #tbl_RankInGuild set NewGrade=1 where lv >= 50 and "
                                                          "rate > 8500 and rate <= 9500 Fail!",
                                                          dwGuildSerial);
                                                        if ( !this->Update_RankInGuild_Step7() )
                                                          this->Update_RankInGuild_Step7();
                                                        return false;
                                                      }
                                                    }
                                                    else
                                                    {
                                                      this->FmtLog(
                                                        "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) "
                                                        ": update #tbl_RankInGuild set NewGrade=2 where lv >= 50 and rate"
                                                        " > 6500 and rate <= 8500 Fail!",
                                                        dwGuildSerial);
                                                      if ( !this->Update_RankInGuild_Step7() )
                                                        this->Update_RankInGuild_Step7();
                                                      return false;
                                                    }
                                                  }
                                                  else
                                                  {
                                                    this->FmtLog(
                                                      "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : "
                                                      "update #tbl_RankInGuild set NewGrade=3 where lv >= 50 and rate > 3"
                                                      "500 and rate <= 6500 Fail!",
                                                      dwGuildSerial);
                                                    if ( !this->Update_RankInGuild_Step7() )
                                                      this->Update_RankInGuild_Step7();
                                                    return false;
                                                  }
                                                }
                                                else
                                                {
                                                  this->FmtLog(
                                                    "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : up"
                                                    "date #tbl_RankInGuild set NewGrade=4 where lv >= 50 and rate > 1500 "
                                                    "and rate <= 3500 Fail!",
                                                    dwGuildSerial);
                                                  if ( !this->Update_RankInGuild_Step7() )
                                                    this->Update_RankInGuild_Step7();
                                                  return false;
                                                }
                                              }
                                              else
                                              {
                                                this->FmtLog(
                                                  "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : upda"
                                                  "te #tbl_RankInGuild set NewGrade=5 where lv >= 50 and rate > 500 and r"
                                                  "ate <= 1500 Fail!",
                                                  dwGuildSerial);
                                                if ( !this->Update_RankInGuild_Step7() )
                                                  this->Update_RankInGuild_Step7();
                                                return false;
                                              }
                                            }
                                            else
                                            {
                                              this->FmtLog(
                                                "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : update"
                                                " #tbl_RankInGuild set NewGrade=6 where lv >= 50 and rate > 100 and rate <= 500 Fail!",
                                                dwGuildSerial);
                                              if ( !this->Update_RankInGuild_Step7() )
                                                this->Update_RankInGuild_Step7();
                                              return false;
                                            }
                                          }
                                          else
                                          {
                                            this->FmtLog(
                                              "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : update #"
                                              "tbl_RankInGuild set NewGrade=7 where lv >= 50 and rate <= 100 Fail!",
                                              dwGuildSerial);
                                            if ( !this->Update_RankInGuild_Step7() )
                                              this->Update_RankInGuild_Step7();
                                            return false;
                                          }
                                        }
                                        else
                                        {
                                          this->FmtLog(
                                            "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : update #tb"
                                            "l_RankInGuild set NewGrade=1 where lv >= 45 and lv <= 49 and rate > 8500 and"
                                            " rate <= 9500 Fail!",
                                            dwGuildSerial);
                                          if ( !this->Update_RankInGuild_Step7() )
                                            this->Update_RankInGuild_Step7();
                                          return false;
                                        }
                                      }
                                      else
                                      {
                                        this->FmtLog(
                                          "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : update #tbl_"
                                          "RankInGuild set NewGrade=2 where lv >= 45 and lv <= 49 and rate > 6500 and rate <= 8500 Fail!",
                                          dwGuildSerial);
                                        if ( !this->Update_RankInGuild_Step7() )
                                          this->Update_RankInGuild_Step7();
                                        return false;
                                      }
                                    }
                                    else
                                    {
                                      this->FmtLog(
                                        "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : update #tbl_Ra"
                                        "nkInGuild set NewGrade=3 where lv >= 45 and lv <= 49 and rate > 3500 and rate <= 6500 Fail!",
                                        dwGuildSerial);
                                      if ( !this->Update_RankInGuild_Step7() )
                                        this->Update_RankInGuild_Step7();
                                      return false;
                                    }
                                  }
                                  else
                                  {
                                    this->FmtLog(
                                      "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : update #tbl_Rank"
                                      "InGuild set NewGrade=4 where lv >= 45 and lv <= 49 and rate > 1500 and rate <= 3500 Fail!",
                                      dwGuildSerial);
                                    if ( !this->Update_RankInGuild_Step7() )
                                      this->Update_RankInGuild_Step7();
                                    return false;
                                  }
                                }
                                else
                                {
                                  this->FmtLog(
                                    "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : update #tbl_RankIn"
                                    "Guild set NewGrade=5 where lv >= 45 and lv <= 49 and rate > 500 and rate <= 1500 Fail!",
                                    dwGuildSerial);
                                  if ( !this->Update_RankInGuild_Step7() )
                                    this->Update_RankInGuild_Step7();
                                  return false;
                                }
                              }
                              else
                              {
                                this->FmtLog(
                                  "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : update #tbl_RankInGu"
                                  "ild set NewGrade=6 where lv >= 45 and lv <= 49 and rate <= 500 Fail!",
                                  dwGuildSerial);
                                if ( !this->Update_RankInGuild_Step7() )
                                  this->Update_RankInGuild_Step7();
                                return false;
                              }
                            }
                            else
                            {
                              this->FmtLog(
                                "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : update #tbl_RankInGuil"
                                "d set NewGrade=1 where lv >= 40 and lv <= 44 and rate > 8500 and rate <= 9500 Fail!",
                                dwGuildSerial);
                              if ( !this->Update_RankInGuild_Step7() )
                                this->Update_RankInGuild_Step7();
                              return false;
                            }
                          }
                          else
                          {
                            this->FmtLog(
                              "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : update #tbl_RankInGuild "
                              "set NewGrade=2 where lv >= 40 and lv <= 44 and rate > 6500 and rate <= 8500 Fail!",
                              dwGuildSerial);
                            if ( !this->Update_RankInGuild_Step7() )
                              this->Update_RankInGuild_Step7();
                            return false;
                          }
                        }
                        else
                        {
                          this->FmtLog(
                            "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : update #tbl_RankInGuild se"
                            "t NewGrade=3 where lv >= 40 and lv <= 44 and rate > 3500 and rate <= 6500 Fail!",
                            dwGuildSerial);
                          if ( !this->Update_RankInGuild_Step7() )
                            this->Update_RankInGuild_Step7();
                          return false;
                        }
                      }
                      else
                      {
                        this->FmtLog(
                          "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : update #tbl_RankInGuild set "
                          "NewGrade=4 where lv >= 40 and lv <= 44 and rate > 1500 and rate <= 3500 Fail!",
                          dwGuildSerial);
                        if ( !this->Update_RankInGuild_Step7() )
                          this->Update_RankInGuild_Step7();
                        return false;
                      }
                    }
                    else
                    {
                      this->FmtLog(
                        "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : update #tbl_RankInGuild set Ne"
                        "wGrade=5 where lv >= 40 and lv <= 44 and rate <= 1500 Fail!",
                        dwGuildSerial);
                      if ( !this->Update_RankInGuild_Step7() )
                        this->Update_RankInGuild_Step7();
                      return false;
                    }
                  }
                  else
                  {
                    this->FmtLog(
                      "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : update #tbl_RankInGuild set NewG"
                      "rade=1 where lv >= 35 and lv <= 39 and rate > 8500 and rate <= 9500 Fail!",
                      dwGuildSerial);
                    if ( !this->Update_RankInGuild_Step7() )
                      this->Update_RankInGuild_Step7();
                    return false;
                  }
                }
                else
                {
                  this->FmtLog(
                    "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : update #tbl_RankInGuild set NewGra"
                    "de=2 where lv >= 35 and lv <= 39 and rate > 6500 and rate <= 8500 Fail!",
                    dwGuildSerial);
                  if ( !this->Update_RankInGuild_Step7() )
                    this->Update_RankInGuild_Step7();
                  return false;
                }
              }
              else
              {
                this->FmtLog(
                  "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : update #tbl_RankInGuild set NewGrade"
                  "=3 where lv >= 35 and lv <= 39 and rate > 3500 and rate <= 6500 Fail!",
                  dwGuildSerial);
                if ( !this->Update_RankInGuild_Step7() )
                  this->Update_RankInGuild_Step7();
                return false;
              }
            }
            else
            {
              this->FmtLog(
                "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : update #tbl_RankInGuild set NewGrade=4"
                " where lv >= 35 and lv <= 39 and rate <= 3500 Fail!",
                dwGuildSerial);
              if ( !this->Update_RankInGuild_Step7() )
                this->Update_RankInGuild_Step7();
              return false;
            }
          }
          else
          {
            this->FmtLog(
              "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : update #tbl_RankInGuild set NewGrade=1 w"
              "here lv >= 30 and lv <= 34 and rate > 8500 and rate <= 9500 Fail!",
              dwGuildSerial);
            if ( !this->Update_RankInGuild_Step7() )
              this->Update_RankInGuild_Step7();
            return false;
          }
        }
        else
        {
          this->FmtLog(
            "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : update #tbl_RankInGuild set NewGrade=2 whe"
            "re lv >= 30 and lv <= 34 and rate > 6500 and rate <= 8500",
            dwGuildSerial);
          if ( !this->Update_RankInGuild_Step7() )
            this->Update_RankInGuild_Step7();
          return false;
        }
      }
      else
      {
        this->FmtLog(
          "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : update #tbl_RankInGuild set NewGrade=3 where"
          " lv >= 30 and lv <= 34 and rate <= 6500 Fail!",
          dwGuildSerial);
        if ( !this->Update_RankInGuild_Step7() )
          this->Update_RankInGuild_Step7();
        return false;
      }
    }
    else
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : update #tbl_RankInGuild set NewGrade=0 Fail!",
        dwGuildSerial);
      if ( !this->Update_RankInGuild_Step7() )
        this->Update_RankInGuild_Step7();
      return false;
    }
  }
  else
  {
    this->FmtLog(
      "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : update #tbl_RankInGuild set Rate = ( (Rank*10000"
      ")/(select count(*) from #tbl_RankInGuild) ) Fail!",
      dwGuildSerial);
    if ( !this->Update_RankInGuild_Step7() )
      this->Update_RankInGuild_Step7();
    return false;
  }
}

bool CRFWorldDatabase::Update_RankInGuild_Step3( unsigned int dwGuildSerial)
{
  this->FmtLog(
    "CRFWorldDatabase::Update_RankInGuild_Step3( dwGuildSerial(%u) ) : Start!",
    dwGuildSerial);
  if ( this->ExecUpdateQuery(
         "select IDENTITY(int, 1, 1) AS NewRank, -1 as Grade, serial, CurGrade, lv, Pvppoint into #tbl_RankInGuildAll fro"
         "m #tbl_RankInGuild order by NewGrade desc, rate ",
         0) )
  {
    if ( this->ExecUpdateQuery(
           "select IDENTITY(int, 1, 1) AS NewRank, -1 as NewRate, -1 as Grade, serial, CurGrade, lv, PvpPoint into #tbl_R"
           "ankInGuildCom from #tbl_RankInGuild where CurGrade <> 0 and CurGrade <> 3 order by NewGrade desc, rate ",
           0) )
    {
      if ( this->ExecUpdateQuery(
             "update #tbl_RankInGuildCom set NewRate = ( (NewRank*10000)/(select count(*) from #tbl_RankInGuildCom) ) ",
             0) )
      {
        this->FmtLog(
          "CRFWorldDatabase::Update_RankInGuild_Step3( dwGuildSerial(%u) ) : End!",
          dwGuildSerial);
        return true;
      }
      else
      {
        this->FmtLog(
          "CRFWorldDatabase::Update_RankInGuild_Step3( dwGuildSerial(%u) ) : update #tbl_RankInGuildCom set NewRate = ( ("
          "NewRank*10000)/(select count(*) from #tbl_RankInGuildCom) ) ",
          dwGuildSerial);
        this->Update_RankInGuild_Step6();
        return false;
      }
    }
    else
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_RankInGuild_Step3( dwGuildSerial(%u) ) : select IDENTITY(int, 1, 1) AS NewRank, -1 as N"
        "ewRate, -1 as Grade, serial, CurGrade, lv, PvpPoint into #tbl_RankInGuildCom from #tbl_RankInGuild where CurGrad"
        "e <> 0 and CurGrade <> 3 order by NewGrade desc, rate Fail!",
        dwGuildSerial);
      if ( !this->Update_RankInGuild_Step7() )
        this->Update_RankInGuild_Step7();
      if ( !this->Update_RankInGuild_Step8() )
        this->Update_RankInGuild_Step8();
      return false;
    }
  }
  else
  {
    this->FmtLog(
      "CRFWorldDatabase::Update_RankInGuild_Step3( dwGuildSerial(%u) ) : select IDENTITY(int, 1, 1) AS NewRank, -1 as Gra"
      "de, serial, CurGrade, lv, Pvppoint into #tbl_RankInGuildAll from #tbl_RankInGuild order by NewGrade desc, rate Fail!",
      dwGuildSerial);
    if ( !this->Update_RankInGuild_Step7() )
      this->Update_RankInGuild_Step7();
    return false;
  }
}

bool CRFWorldDatabase::Update_RankInGuild_Step4( unsigned int dwGuildSerial)
{
  this->FmtLog(
    "CRFWorldDatabase::Update_RankInGuild_Step4( dwGuildSerial(%u) ) : Start Apply tb_gneral!",
    dwGuildSerial);
  if ( this->ExecUpdateQuery(
         "update tbl_general set GuildRank = rank.NewRank\tfrom ( select serial, NewRank from #tbl_RankInGuildAll ) as ra"
         "nk where tbl_general.serial = rank.serial",
         0) )
  {
    this->FmtLog(
      "CRFWorldDatabase::Update_RankInGuild_Step4( dwGuildSerial(%u) ) : End Apply tbl_general!",
      dwGuildSerial);
    return true;
  }
  else
  {
    this->FmtLog(
      "CRFWorldDatabase::Update_RankInGuild_Step4( dwGuildSerial(%u) ) : update tbl_general set GuildRank = rank.NewRank\t"
      "from ( select serial, NewRank from #tbl_RankInGuildAll ) as rank where tbl_general.serial = rank.serial Fail!",
      dwGuildSerial);
    this->Update_RankInGuild_Step6();
    return false;
  }
}

bool CRFWorldDatabase::Update_RankInGuild_Step5(
        unsigned int dwGuildSerial,
        _worlddb_rankinguild_info *pGuildMemberRankData)
{
  SQLLEN strLenOrIndPtr;
  SQLRETURN sqlStatus;
  char queryBuffer[1028];
  int memberIndex;
  int fetchedGrade = 2;
  unsigned int gradeBySerial[157];
  unsigned int memberLevel = 0;
  double memberPvpPoint = 0.0;
  int gradeIndex;

  this->FmtLog(
    "CRFWorldDatabase::Update_RankInGuild_Step5( dwGuildSerial(%u), pGuildMemberRankData ) : Start Set Return Result!",
    dwGuildSerial);
  memset(queryBuffer, 0, 1024);
  memberIndex = 0;
  std::memset(gradeBySerial, 0, 600);
  this->FmtLog(
    "CRFWorldDatabase::Update_RankInGuild_Step5( dwGuildSerial(%u), pGuildMemberRankData ) : Start Get Grade from #tbl_RankInGuildCom",
    dwGuildSerial);
  sprintf_s(queryBuffer, 1024, "select top %u serial, Grade from #tbl_RankInGuildCom order by Grade", 50);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    sqlStatus = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)queryBuffer, -3);
    if ( !sqlStatus || sqlStatus == 1 )
    {
      for ( memberIndex = 0; ; ++memberIndex )
      {
        sqlStatus = SQLFetch(this->m_hStmtSelect);
        if ( sqlStatus )
        {
          if ( sqlStatus != 1 )
            break;
        }
        sqlStatus = SQLGetData(this->m_hStmtSelect, 1u, 4, &gradeBySerial[3 * memberIndex], 0LL, &strLenOrIndPtr);
        sqlStatus = SQLGetData(this->m_hStmtSelect, 2u, 4, &fetchedGrade, 0LL, &strLenOrIndPtr);
        gradeBySerial[3 * memberIndex + 1] = static_cast<unsigned int>(fetchedGrade & 0xFF);
      }
      if ( this->m_hStmtSelect )
        SQLCloseCursor(this->m_hStmtSelect);
      this->FmtLog(
        "CRFWorldDatabase::Update_RankInGuild_Step5( dwGuildSerial(%u), pGuildMemberRankData ) : End Get Grade from #tbl_RankInGuildCom",
        dwGuildSerial);
      this->FmtLog(
        "CRFWorldDatabase::Update_RankInGuild_Step5( dwGuildSerial(%u), pGuildMemberRankData ) : Start Get Grade from #tb"
        "l_RankInGuildAll",
        dwGuildSerial);
      sprintf_s(
        queryBuffer,
        sizeof(queryBuffer),
        "select serial, lv, Pvppoint, CurGrade from #tbl_RankInGuildAll order by NewRank");
      if ( this->m_hStmtSelect || this->ReConnectDataBase() )
      {
        sqlStatus = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)queryBuffer, -3);
        if ( !sqlStatus || sqlStatus == 1 )
        {
          memberIndex = 0;
          memberLevel = 0;
          memberPvpPoint = 0.0;
          while ( 1 )
          {
            sqlStatus = SQLFetch(this->m_hStmtSelect);
            if ( sqlStatus )
            {
              if ( sqlStatus != 1 )
                break;
            }
            sqlStatus = SQLGetData(this->m_hStmtSelect, 1u, 4, &pGuildMemberRankData->MemberData[memberIndex], 0LL, &strLenOrIndPtr);
            sqlStatus = SQLGetData(this->m_hStmtSelect, 2u, static_cast<SQLSMALLINT>(65518), &memberLevel, 0LL, &strLenOrIndPtr);
            sqlStatus = SQLGetData(this->m_hStmtSelect, 3u, 8, &memberPvpPoint, 0LL, &strLenOrIndPtr);
            sqlStatus = SQLGetData(
                   this->m_hStmtSelect,
                   4u,
                   4,
                   &pGuildMemberRankData->MemberData[memberIndex].byGrade,
                   0LL,
                   &strLenOrIndPtr);
            pGuildMemberRankData->MemberData[memberIndex].byLv = memberLevel;
            pGuildMemberRankData->MemberData[memberIndex].dwPvpPoint = (int)memberPvpPoint;
            if ( pGuildMemberRankData->MemberData[memberIndex].byGrade == 2 )
              pGuildMemberRankData->MemberData[memberIndex].byGrade = 2;
            else
              pGuildMemberRankData->MemberData[memberIndex].byGrade = 0;
            for ( gradeIndex = 0; gradeIndex < 50; ++gradeIndex )
            {
              if ( pGuildMemberRankData->MemberData[memberIndex].dwSerial == gradeBySerial[3 * gradeIndex] )
              {
                pGuildMemberRankData->MemberData[memberIndex].byGrade = gradeBySerial[3 * gradeIndex + 1];
                break;
              }
            }
            ++memberIndex;
          }
          pGuildMemberRankData->wRecordCount = memberIndex;
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          this->FmtLog(
            "CRFWorldDatabase::Update_RankInGuild_Step5( dwGuildSerial(%u), pGuildMemberRankData ) : End Set Return Result!",
            dwGuildSerial);
          return true;
        }
        else
        {
          this->Update_RankInGuild_Step6();
          if ( sqlStatus == 100 )
          {
            this->FmtLog(
              "CRFWorldDatabase::Update_RankInGuild_Step5( dwGuildSerial(%u), pGuildMemberRankData ) : %s : NO_DATA!",
              dwGuildSerial,
              queryBuffer);
          }
          else
          {
            this->FmtLog(
              "CRFWorldDatabase::Update_RankInGuild_Step5( dwGuildSerial(%u), pGuildMemberRankData ) : %s : Sql Error!",
              dwGuildSerial,
              queryBuffer);
            this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLExecDirect", this->m_hStmtSelect);
            this->ErrorAction( sqlStatus, this->m_hStmtSelect);
          }
          return false;
        }
      }
      else
      {
        this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", queryBuffer);
        return false;
      }
    }
    else
    {
      this->Update_RankInGuild_Step6();
      if ( sqlStatus == 100 )
      {
        this->FmtLog(
          "CRFWorldDatabase::Update_RankInGuild_Step5( dwGuildSerial(%u), pGuildMemberRankData ) : %s : NO_DATA!",
          dwGuildSerial,
          queryBuffer);
      }
      else
      {
        this->FmtLog(
          "CRFWorldDatabase::Update_RankInGuild_Step5( dwGuildSerial(%u), pGuildMemberRankData ) : %s : Sql Error!",
          dwGuildSerial,
          queryBuffer);
        this->ErrorMsgLog( sqlStatus, queryBuffer, "SQLExecDirect", this->m_hStmtSelect);
        this->ErrorAction( sqlStatus, this->m_hStmtSelect);
      }
      return false;
    }
  }
  else
  {
    this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", queryBuffer);
    return false;
  }
}

bool CRFWorldDatabase::Update_RankInGuild_Step6()
{
  this->FmtLog(
    "CRFWorldDatabase::Update_RankInGuild_Step6() : Start drop #tbl_RankInGuild, #tbl_RankInGuildAll, #tbl_RankInGuildCom Table");
  if ( this->Update_RankInGuild_Step7() )
  {
    if ( this->Update_RankInGuild_Step8() )
    {
      if ( this->Update_RankInGuild_Step9() )
      {
        this->FmtLog(
          "CRFWorldDatabase::Update_RankInGuild_Step6() : End drop #tbl_RankInGuild, #tbl_RankInGuildAll, #tbl_RankInGuildCom Table");
        return true;
      }
      else
      {
        this->Update_RankInGuild_Step9();
        return false;
      }
    }
    else
    {
      this->Update_RankInGuild_Step8();
      return false;
    }
  }
  else
  {
    this->Update_RankInGuild_Step7();
    return false;
  }
}

