#include "pch.h"

#include "CMainThread.h"
#include "CRFWorldDatabase.h"
#include "CPotionMgr.h"
#include "PatriarchElectProcessor.h"
#include "WorldServerUtil.h"
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
  __int64 *v4; // rdi
  __int64 i; // rcx
  __int64 v7; // [rsp+0h] [rbp-168h] BYREF
  char Buffer[272]; // [rsp+40h] [rbp-128h] BYREF

  v4 = &v7;
  memset(Buffer, 0, 256);
  sprintf(Buffer, "{ CALL pAdd_PvpPoint( %d, %d, %d ) }", dwSerial, dwPoint, dwCashBag);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Check_GuildMemberCount( unsigned int dwGuildSerial)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-1A8h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+38h] [rbp-170h] BYREF
  SQLRETURN v7; // [rsp+44h] [rbp-164h]
  char Buffer[276]; // [rsp+60h] [rbp-148h] BYREF
  int TargetValue[7]; // [rsp+174h] [rbp-34h] BYREF

  v2 = &v5;
  TargetValue[0] = 0;
  sprintf(Buffer, "select MemberCount from tbl_guild where serial = %d", dwGuildSerial);
  if ( this->m_bSaveDBLog )
    this->Log( Buffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v7 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( !v7 || v7 == 1 )
    {
      v7 = SQLFetch(this->m_hStmtSelect);
      if ( !v7 || v7 == 1 )
      {
        v7 = SQLGetData(this->m_hStmtSelect, 1u, 5, TargetValue, 0LL, &StrLen_or_IndPtr);
        if ( !v7 || v7 == 1 )
        {
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          return TargetValue[0] < 50;
        }
        else
        {
          this->ErrorMsgLog( v7, Buffer, "SQLGetData", this->m_hStmtSelect);
          this->ErrorAction( v7, this->m_hStmtSelect);
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          return 0;
        }
      }
      else
      {
        if ( v7 != 100 )
        {
          this->ErrorMsgLog( v7, Buffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction( v7, this->m_hStmtSelect);
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return 0;
      }
    }
    else
    {
      if ( v7 != 100 )
      {
        this->ErrorMsgLog( v7, Buffer, "SQLExecDirect", this->m_hStmtSelect);
        this->ErrorAction( v7, this->m_hStmtSelect);
      }
      return 0;
    }
  }
  else
  {
    this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", Buffer);
    return 0;
  }
}

bool CRFWorldDatabase::CreateCharacterSelectLogTable(const char *szTableName)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-CA8h] BYREF
  char Buffer[3088]; // [rsp+80h] [rbp-C28h] BYREF

  v2 = &v5;
  memset_0(Buffer, 0, 0xC00uLL);
  sprintf(
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
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-168h] BYREF
  char Buffer[260]; // [rsp+40h] [rbp-128h] BYREF
  SQLRETURN v7; // [rsp+144h] [rbp-24h]

  v2 = &v5;
  sprintf(Buffer, "{ CALL pDelete_Base( %d ) }", dwCharacterSerial);
  if ( this->m_bSaveDBLog )
    this->FmtLog( "Query : %s", Buffer);
  if ( this->m_hStmtUpdate || this->ReConnectDataBase() )
  {
    v7 = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)Buffer, -3);
    if ( !v7 || v7 == 1 )
    {
      this->SetAutoCommitMode( 0);
      sprintf(Buffer, "{ CALL pUpdate_DeleteName_Step1( %d ) }", dwCharacterSerial);
      v7 = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)Buffer, -3);
      if ( !v7 || v7 == 1 )
      {
        sprintf(Buffer, "{ CALL pUpdate_DeleteName_Step2( %d ) }", dwCharacterSerial);
        v7 = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)Buffer, -3);
        if ( !v7 || v7 == 1 )
        {
          this->CommitTransaction();
          this->SetAutoCommitMode( 1);
          if ( this->m_bSaveDBLog )
            this->Log( "Query Success");
          return 1;
        }
        else
        {
          this->ErrorMsgLog( v7, Buffer, "SQLExecDirectA", this->m_hStmtUpdate);
          this->RollbackTransaction();
          this->SetAutoCommitMode( 1);
          this->ErrorAction( v7, this->m_hStmtUpdate);
          return 0;
        }
      }
      else
      {
        this->ErrorMsgLog( v7, Buffer, "SQLExecDirectA", this->m_hStmtUpdate);
        this->RollbackTransaction();
        this->SetAutoCommitMode( 1);
        this->ErrorAction( v7, this->m_hStmtUpdate);
        return 0;
      }
    }
    else
    {
      this->ErrorMsgLog( v7, Buffer, "SQLExecDirectA", this->m_hStmtUpdate);
      this->ErrorAction( v7, this->m_hStmtUpdate);
      return 0;
    }
  }
  else
  {
    this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", Buffer);
    return 0;
  }
}

bool CRFWorldDatabase::Delete_Guild( unsigned int dwGuildSerial)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-158h] BYREF
  char Buffer[272]; // [rsp+30h] [rbp-128h] BYREF

  v2 = &v5;
  memset(Buffer, 0, 256);
  sprintf(Buffer, "{ CALL pUpdate_DeleteGuild(%d) }", dwGuildSerial);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Delete_PatriarchComm( unsigned int dwSerial, char *pszDepDate)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-D8h] BYREF
  char Buffer[144]; // [rsp+30h] [rbp-A8h] BYREF

  v3 = &v6;
  sprintf(Buffer, "{ CALL pDelete_PatriarchComm( %d, '%s') }", dwSerial, pszDepDate);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Insert_CashLimSale()
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-458h] BYREF
  char Buffer[1040]; // [rsp+30h] [rbp-428h] BYREF

  v1 = &v4;
  memset(Buffer, 0, 1024);
  sprintf_s(Buffer, 0x400uLL, "{ CALL pInsert_CashLimSale( %d ) }", 0);
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
  __int64 *v10; // rdi
  __int64 i; // rcx
  __int64 v13; // [rsp+0h] [rbp-2B8h] BYREF
  SQLRETURN v14; // [rsp+50h] [rbp-268h]
  char Buffer[288]; // [rsp+70h] [rbp-248h] BYREF
  char szLog[272]; // [rsp+190h] [rbp-128h] BYREF

  v10 = &v13;
  sprintf(
    Buffer,
    "{ CALL pInsert_Base(N'%s', '%s', %d, '%s', %d, %d, %d ) }",
    pwszCharacterName,
    wszClassCode,
    dwAccountSerial,
    wszAccount,
    bySlotIndex,
    byRaceSexCode,
    dwBaseShape);
  if ( this->m_bSaveDBLog )
    this->Log( Buffer);
  if ( !this->m_hStmtUpdate && !this->ReConnectDataBase() )
  {
    this->ErrLog( "ReConnectDataBase Fail. Query : Insert_CharacterData");
    return 0;
  }
  this->SetAutoCommitMode( 0);
  v14 = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)Buffer, -3);
  if ( v14 && v14 != 1 )
  {
    this->ErrorMsgLog( v14, Buffer, "_SQLExecDirect", this->m_hStmtUpdate);
    this->RollbackTransaction();
    this->SetAutoCommitMode( 1);
    this->ErrorAction( v14, this->m_hStmtUpdate);
    return 0;
  }
  if ( this->m_bSaveDBLog )
    this->FmtLog( "%s Success", Buffer);
  if ( !this->Select_CharacterSerial( pwszCharacterName, pDwSerial) )
  {
    this->RollbackTransaction();
    this->SetAutoCommitMode( 1);
    return 0;
  }
  sprintf(szLog, "{ CALL pInsert_General( %d, %d ) }", *pDwSerial, nMapIndex);
  if ( this->m_bSaveDBLog )
    this->Log( szLog);
  v14 = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)szLog, -3);
  if ( v14 && v14 != 1 )
  {
    this->ErrorMsgLog( v14, szLog, "SQLExecDirectA", this->m_hStmtUpdate);
    this->RollbackTransaction();
    this->SetAutoCommitMode( 1);
    this->ErrorAction( v14, this->m_hStmtUpdate);
    return 0;
  }
  if ( this->m_bSaveDBLog )
    this->FmtLog( "%s Success", szLog);
  sprintf(szLog, "{ CALL pInsert_supplement( %d ) }", *pDwSerial);
  if ( this->m_bSaveDBLog )
    this->Log( szLog);
  v14 = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)szLog, -3);
  if ( v14 && v14 != 1 )
  {
    this->ErrorMsgLog( v14, szLog, "SQLExecDirectA", this->m_hStmtUpdate);
    this->RollbackTransaction();
    this->SetAutoCommitMode( 1);
    this->ErrorAction( v14, this->m_hStmtUpdate);
    return 0;
  }
  if ( this->m_bSaveDBLog )
    this->FmtLog( "%s Success", szLog);
  sprintf(szLog, "{ CALL pInsert_inven( %d ) }", *pDwSerial);
  if ( this->m_bSaveDBLog )
    this->Log( szLog);
  v14 = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)szLog, -3);
  if ( v14 && v14 != 1 )
  {
    this->ErrorMsgLog( v14, szLog, "SQLExecDirectA", this->m_hStmtUpdate);
    this->RollbackTransaction();
    this->SetAutoCommitMode( 1);
    this->ErrorAction( v14, this->m_hStmtUpdate);
    return 0;
  }
  if ( this->m_bSaveDBLog )
    this->FmtLog( "%s Success", szLog);
  sprintf(szLog, "{ CALL pInsert_Quest( %d ) }", *pDwSerial);
  if ( this->m_bSaveDBLog )
    this->Log( szLog);
  v14 = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)szLog, -3);
  if ( v14 && v14 != 1 )
  {
    this->ErrorMsgLog( v14, szLog, "SQLExecDirectA", this->m_hStmtUpdate);
    this->RollbackTransaction();
    this->SetAutoCommitMode( 1);
    this->ErrorAction( v14, this->m_hStmtUpdate);
    return 0;
  }
  if ( this->m_bSaveDBLog )
    this->FmtLog( "%s Success", szLog);
  sprintf(szLog, "{ CALL pInsert_UserInterface( %d ) }", *pDwSerial);
  if ( this->m_bSaveDBLog )
    this->Log( szLog);
  v14 = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)szLog, -3);
  if ( v14 && v14 != 1 )
  {
    this->ErrorMsgLog( v14, szLog, "SQLExecDirectA", this->m_hStmtUpdate);
    this->RollbackTransaction();
    this->SetAutoCommitMode( 1);
    this->ErrorAction( v14, this->m_hStmtUpdate);
    return 0;
  }
  if ( this->m_bSaveDBLog )
    this->FmtLog( "%s Success", szLog);
  if ( !((int)byRaceSexCode >> 1) )
  {
    sprintf(szLog, "{ CALL pInsert_Unit( %d ) }", *pDwSerial);
    if ( this->m_bSaveDBLog )
      this->Log( szLog);
    v14 = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)szLog, -3);
    if ( v14 && v14 != 1 )
    {
      this->ErrorMsgLog( v14, szLog, "SQLExecDirectA", this->m_hStmtUpdate);
      this->RollbackTransaction();
      this->SetAutoCommitMode( 1);
      this->ErrorAction( v14, this->m_hStmtUpdate);
      return 0;
    }
    if ( this->m_bSaveDBLog )
      this->FmtLog( "%s Success", szLog);
  }
  sprintf(szLog, "{ CALL pInsert_CombineEx_Result( %d ) }", *pDwSerial);
  if ( this->m_bSaveDBLog )
    this->Log( szLog);
  v14 = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)szLog, -3);
  if ( !v14 || v14 == 1 )
  {
    if ( this->m_bSaveDBLog )
      this->FmtLog( "%s Success", szLog);
    sprintf(szLog, "{ CALL pInsert_OreCutting( %d ) }", *pDwSerial);
    if ( this->m_bSaveDBLog )
      this->Log( szLog);
    v14 = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)szLog, -3);
    if ( !v14 || v14 == 1 )
    {
      if ( this->m_bSaveDBLog )
        this->FmtLog( "%s Success", szLog);
      this->CommitTransaction();
      this->SetAutoCommitMode( 1);
      return 1;
    }
    else
    {
      this->ErrorMsgLog( v14, szLog, "SQLExecDirectA", this->m_hStmtUpdate);
      this->RollbackTransaction();
      this->SetAutoCommitMode( 1);
      this->ErrorAction( v14, this->m_hStmtUpdate);
      return 0;
    }
  }
  else
  {
    this->ErrorMsgLog( v14, szLog, "SQLExecDirectA", this->m_hStmtUpdate);
    this->RollbackTransaction();
    this->SetAutoCommitMode( 1);
    this->ErrorAction( v14, this->m_hStmtUpdate);
    return 0;
  }
}

bool CRFWorldDatabase::Insert_Economy_History(unsigned int dwDate, _worlddb_economy_history_info *pEconomyData)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-408h] BYREF
  char Buffer[544]; // [rsp+F0h] [rbp-318h] BYREF
  _worlddb_economy_history_info pEconomyDataa; // [rsp+310h] [rbp-F8h] BYREF
  unsigned __int8 exist; // [rsp+3E4h] [rbp-24h]

  v3 = &v6;
  memset(Buffer, 0, 512);
  exist = this->Select_Exist_Economy( dwDate, &pEconomyDataa);
  if ( exist == 2 )
  {
    sprintf(
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
    sprintf(
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
  __int64 *v4; // rdi
  __int64 i; // rcx
  int Year; // eax
  __int64 v8; // [rsp+0h] [rbp-1C8h] BYREF
  char Buffer[280]; // [rsp+70h] [rbp-158h] BYREF
  ATL::CTime result;
  int Second; // [rsp+1A0h] [rbp-28h]
  int Minute; // [rsp+1A4h] [rbp-24h]
  int Hour; // [rsp+1A8h] [rbp-20h]
  int Day; // [rsp+1ACh] [rbp-1Ch]
  int Month; // [rsp+1B0h] [rbp-18h]

  v4 = &v8;
  memset_0(Buffer, 0, 0x100uLL);
  result = ATL::CTime::GetCurrentTime();
  Second = result.GetSecond();
  Minute = result.GetMinute();
  Hour = result.GetHour();
  Day = result.GetDay();
  Month = result.GetMonth();
  Year = result.GetYear();
  sprintf(
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
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-258h] BYREF
  char Buffer[528]; // [rsp+30h] [rbp-228h] BYREF

  v3 = &v6;
  memset_0(Buffer, 0, 0x200uLL);
  sprintf(Buffer, "{ CALL pInsert_Guild('%s', %d) }", pwszGuildName, byRace);
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
  __int64 *v9; // rdi
  __int64 i; // rcx
  __int64 v12; // [rsp+0h] [rbp-488h] BYREF
  char Buffer[1040]; // [rsp+60h] [rbp-428h] BYREF

  v9 = &v12;
  memset(Buffer, 0, 1024);
  sprintf(
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
  __int64 *v6; // rdi
  __int64 i; // rcx
  __int64 v9; // [rsp+0h] [rbp-478h] BYREF
  char Buffer[1040]; // [rsp+50h] [rbp-428h] BYREF

  v6 = &v9;
  sprintf_s(
    Buffer,
    0x400uLL,
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
  __int64 *v4; // rdi
  __int64 i; // rcx
  __int64 v7; // [rsp+0h] [rbp-168h] BYREF
  char Buffer[272]; // [rsp+40h] [rbp-128h] BYREF

  v4 = &v7;
  sprintf(Buffer, "{ CALL pInsert_Level_Log( %d, %d, %d ) }", dwCharacterSerial, byLevel, dwTotalPlayMin);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Insert_MacroData( unsigned int dwSerial)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-4D8h] BYREF
  int j; // [rsp+90h] [rbp-448h]
  bool updated; // [rsp+94h] [rbp-444h]
  char Buffer[1040]; // [rsp+B0h] [rbp-428h] BYREF

  v2 = &v5;
  updated = 0;
  memset(Buffer, 0, 1024);
  for ( j = 0; j < 3; ++j )
  {
    memset_0(Buffer, 0, 0x400uLL);
    sprintf(
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
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-158h] BYREF
  char Buffer[272]; // [rsp+30h] [rbp-128h] BYREF

  v2 = &v5;
  memset(Buffer, 0, 256);
  sprintf_s(Buffer, 0x100uLL, "insert [dbo].[tbl_event] (avatorserial) values (%u)", dwAvatorSerial);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Insert_Set_Limit_Run( unsigned __int8 *pData, int iSize)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-468h] BYREF
  char _Dest[1024]; // [rsp+40h] [rbp-428h] BYREF

  v3 = &v6;
  memset(_Dest, 0, sizeof(_Dest));
  sprintf_s(_Dest, sizeof(_Dest), "insert into [dbo].[tbl_sf_delay] ( [aserial], [effect] ) values (0, ?)");
  return this->ExecUpdateBinaryQuery(_Dest, reinterpret_cast<char *>(pData), iSize, 1);
}

bool CRFWorldDatabase::Insert_UserNum_Log( int nAvgUserNum, int nMaxUserNum)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-158h] BYREF
  char Buffer[272]; // [rsp+30h] [rbp-128h] BYREF

  v3 = &v6;
  sprintf(Buffer, "{ CALL pInsert_UserNum_Log( %d, %d ) }", nAvgUserNum, nMaxUserNum);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Insert_WeeklyGuildPvpPointSum( unsigned int dwSerial)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-158h] BYREF
  char Buffer[272]; // [rsp+30h] [rbp-128h] BYREF

  v2 = &v5;
  memset_0(Buffer, 0, 0x100uLL);
  sprintf(Buffer, "{ CALL pInsert_WeeklyGuildPVPPointSum(%u) }", dwSerial);
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
  __int64 *v13; // rdi
  __int64 i; // rcx
  __int64 v16; // [rsp+0h] [rbp-4B8h] BYREF
  char Buffer[1040]; // [rsp+80h] [rbp-438h] BYREF

  v13 = &v16;
  sprintf(
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
  __int64 *v11; // rdi
  __int64 i; // rcx
  __int64 v14; // [rsp+0h] [rbp-4B8h] BYREF
  char Buffer[1040]; // [rsp+80h] [rbp-438h] BYREF

  v11 = &v14;
  memset_0(Buffer, 0, 0x400uLL);
  sprintf(
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
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-1A8h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+38h] [rbp-170h] BYREF
  SQLRETURN v7; // [rsp+44h] [rbp-164h]
  char Buffer[260]; // [rsp+60h] [rbp-148h] BYREF
  int v9; // [rsp+164h] [rbp-44h]
  unsigned __int8 TargetValue[28]; // [rsp+174h] [rbp-34h] BYREF

  v2 = &v5;
  v9 = 0;
  TargetValue[0] = 0;
  sprintf(Buffer, "{ CALL pSelect_ArrangeInfo( %d ) }", dwSerial);
  if ( this->m_bSaveDBLog )
    this->Log( Buffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v7 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( !v7 || v7 == 1 )
    {
      v7 = SQLFetch(this->m_hStmtSelect);
      if ( !v7 || v7 == 1 )
      {
        v7 = SQLGetData(this->m_hStmtSelect, 1u, 65530, TargetValue, 0LL, &StrLen_or_IndPtr);
        if ( !v7 || v7 == 1 )
        {
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          if ( this->m_bSaveDBLog )
            this->FmtLog( "%s Success", Buffer);
          return TargetValue[0] == 0;
        }
        else
        {
          this->ErrorMsgLog( v7, Buffer, "SQLGetData", this->m_hStmtSelect);
          this->ErrorAction( v7, this->m_hStmtSelect);
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          return 0;
        }
      }
      else
      {
        if ( v7 != 100 )
        {
          this->ErrorMsgLog( v7, Buffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction( v7, this->m_hStmtSelect);
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return 0;
      }
    }
    else
    {
      if ( v7 != 100 )
      {
        this->ErrorMsgLog( v7, Buffer, "SQLExecDirect", this->m_hStmtSelect);
        this->ErrorAction( v7, this->m_hStmtSelect);
      }
      return 0;
    }
  }
  else
  {
    this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", Buffer);
    return 0;
  }
}

bool CRFWorldDatabase::Select_ChracterSerialRace(
        char *pwszCharacterName,
        unsigned int *pSerial,
        unsigned __int8 *pbyRace)
{
  __int64 *v4; // rdi
  __int64 i; // rcx
  __int64 v7; // [rsp+0h] [rbp-188h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+38h] [rbp-150h] BYREF
  SQLRETURN v9; // [rsp+44h] [rbp-144h]
  char Buffer[260]; // [rsp+60h] [rbp-128h] BYREF
  int v11; // [rsp+164h] [rbp-24h]

  v4 = &v7;
  v11 = 0;
  sprintf(Buffer, "{ CALL pSelect_CharacterSerialRace('%s') }", pwszCharacterName);
  if ( this->m_bSaveDBLog )
    this->Log( Buffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v9 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( !v9 || v9 == 1 )
    {
      v9 = SQLFetch(this->m_hStmtSelect);
      if ( !v9 || v9 == 1 )
      {
        v9 = SQLGetData(this->m_hStmtSelect, 1u, 65518, pSerial, 0LL, &StrLen_or_IndPtr);
        if ( !v9 || v9 == 1 )
        {
          v9 = SQLGetData(this->m_hStmtSelect, 2u, 65530, pbyRace, 0LL, &StrLen_or_IndPtr);
          if ( !v9 || v9 == 1 )
          {
            if ( this->m_hStmtSelect )
              SQLCloseCursor(this->m_hStmtSelect);
            if ( this->m_bSaveDBLog )
              this->FmtLog( "%s Success", Buffer);
            return 1;
          }
          else
          {
            this->ErrorMsgLog( v9, Buffer, "SQLGetData", this->m_hStmtSelect);
            this->ErrorAction( v9, this->m_hStmtSelect);
            if ( this->m_hStmtSelect )
              SQLCloseCursor(this->m_hStmtSelect);
            return 0;
          }
        }
        else
        {
          this->ErrorMsgLog( v9, Buffer, "SQLGetData", this->m_hStmtSelect);
          this->ErrorAction( v9, this->m_hStmtSelect);
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          return 0;
        }
      }
      else
      {
        if ( v9 != 100 )
        {
          this->ErrorMsgLog( v9, Buffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction( v9, this->m_hStmtSelect);
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return 0;
      }
    }
    else
    {
      if ( v9 != 100 )
      {
        this->ErrorMsgLog( v9, Buffer, "_SQLExecDirect", this->m_hStmtSelect);
        this->ErrorAction( v9, this->m_hStmtSelect);
      }
      return 0;
    }
  }
  else
  {
    this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", Buffer);
    return 0;
  }
}

bool CRFWorldDatabase::Select_GuildSerial(
        char *pwszGuildName,
        unsigned int *pdwGuildSerial)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-188h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+38h] [rbp-150h] BYREF
  SQLRETURN v8; // [rsp+44h] [rbp-144h]
  char Buffer[260]; // [rsp+60h] [rbp-128h] BYREF
  int v10; // [rsp+164h] [rbp-24h]

  v3 = &v6;
  v10 = 0;
  sprintf(Buffer, "{ CALL pSelect_GuildSerial('%s') }", pwszGuildName);
  if ( this->m_bSaveDBLog )
    this->Log( Buffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v8 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( !v8 || v8 == 1 )
    {
      v8 = SQLFetch(this->m_hStmtSelect);
      if ( !v8 || v8 == 1 )
      {
        v8 = SQLGetData(this->m_hStmtSelect, 1u, 65518, pdwGuildSerial, 0LL, &StrLen_or_IndPtr);
        if ( !v8 || v8 == 1 )
        {
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          if ( this->m_bSaveDBLog )
            this->FmtLog( "%s Success", Buffer);
          return 1;
        }
        else
        {
          this->ErrorMsgLog( v8, Buffer, "SQLGetData", this->m_hStmtSelect);
          this->ErrorAction( v8, this->m_hStmtSelect);
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          return 0;
        }
      }
      else
      {
        if ( v8 != 100 )
        {
          this->ErrorMsgLog( v8, Buffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction( v8, this->m_hStmtSelect);
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return 0;
      }
    }
    else
    {
      if ( v8 != 100 )
      {
        this->ErrorMsgLog( v8, Buffer, "_SQLExecDirect", this->m_hStmtSelect);
        this->ErrorAction( v8, this->m_hStmtSelect);
      }
      return 0;
    }
  }
  else
  {
    this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", Buffer);
    return 0;
  }
}

bool CRFWorldDatabase::Update_Dalant( unsigned int dwSerial, unsigned int dwDalant)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-D8h] BYREF
  char Buffer[144]; // [rsp+30h] [rbp-A8h] BYREF

  v3 = &v6;
  memset(Buffer, 0, 128);
  sprintf(Buffer, "UPDATE tbl_base SET Dalant = %d WHERE Serial = %d", dwDalant, dwSerial);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Update_GmGreet( _qry_case_gm_greetingmsg *pSheet)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-458h] BYREF
  char Buffer[1040]; // [rsp+30h] [rbp-428h] BYREF

  v2 = &v5;
  memset(Buffer, 0, 1024);
  sprintf(
    Buffer,
    "update tbl_GreetMsg set Name='%s', GMsg='%s' where useType=255",
    pSheet->in_gmname,
    pSheet->in_gmgreetingmsg);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Update_Gold( unsigned int dwSerial, unsigned int dwGold)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-D8h] BYREF
  char Buffer[144]; // [rsp+30h] [rbp-A8h] BYREF

  v3 = &v6;
  memset(Buffer, 0, 128);
  sprintf(Buffer, "UPDATE tbl_base SET Gold = %d WHERE Serial = %d", dwGold, dwSerial);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Update_GuildEmblem(
        unsigned int dwGuildSerial,
        long double dCurDalant,
        unsigned int dwEmblemBack,
        unsigned int dwEmblemMark)
{
  __int64 *v5; // rdi
  __int64 i; // rcx
  __int64 v8; // [rsp+0h] [rbp-178h] BYREF
  char Buffer[272]; // [rsp+50h] [rbp-128h] BYREF

  v5 = &v8;
  memset(Buffer, 0, 256);
  sprintf_s(
    Buffer,
    0x100uLL,
    "update [dbo].[tbl_guild] set emblemBack=%d, emblemMark=%d, dalant=%f where Serial=%d",
    dwEmblemBack,
    dwEmblemMark,
    (double)dCurDalant,
    dwGuildSerial);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Update_GuildGrade()
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-158h] BYREF
  char Buffer[272]; // [rsp+30h] [rbp-128h] BYREF

  v1 = &v4;
  memset(Buffer, 0, 256);
  sprintf(
    Buffer,
    "update tbl_Guild set Grade = r.Grade from tbl_Guild as g join tbl_GuildRankToday as r on g.serial = r.serial ");
  return this->ExecUpdateQuery( Buffer, 0);
}

bool CRFWorldDatabase::Update_GuildGreet( _qry_case_guild_greetingmsg *pSheet)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-458h] BYREF
  char Buffer[1040]; // [rsp+30h] [rbp-428h] BYREF

  v2 = &v5;
  memset(Buffer, 0, 1024);
  sprintf(Buffer, "update tbl_guild set GMsg='%s' where Serial=%d", pSheet->in_guildgreetingmsg, pSheet->in_guildserial);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Update_GuildMaster(
        unsigned int dwGuild_Serial,
        unsigned int dwGuildMaster_Serial,
        unsigned __int8 byGuildMaster_PrevGrade)
{
  __int64 *v4; // rdi
  __int64 i; // rcx
  __int64 v7; // [rsp+0h] [rbp-168h] BYREF
  char Buffer[272]; // [rsp+40h] [rbp-128h] BYREF

  v4 = &v7;
  sprintf(
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
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-158h] BYREF
  char Buffer[272]; // [rsp+30h] [rbp-128h] BYREF

  v3 = &v6;
  memset(Buffer, 0, 256);
  sprintf(Buffer, "update tbl_Guild set MemberCount = %d where serial = %d", wMemberNum, dwGuildSerial);
  return this->ExecUpdateQuery( Buffer, 0);
}

bool CRFWorldDatabase::Update_GuildRoom( unsigned int dwGuildSerial)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-158h] BYREF
  char Buffer[272]; // [rsp+30h] [rbp-128h] BYREF

  v2 = &v5;
  memset_0(Buffer, 0, 0x100uLL);
  sprintf(
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
  __int64 *v4; // rdi
  __int64 i; // rcx
  __int64 v7; // [rsp+0h] [rbp-168h] BYREF
  char Buffer[272]; // [rsp+40h] [rbp-128h] BYREF

  v4 = &v7;
  memset(Buffer, 0, 256);
  sprintf(
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
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-B18h] BYREF
  SQLRETURN v7; // [rsp+40h] [rbp-AD8h]
  char Buffer[2080]; // [rsp+60h] [rbp-AB8h] BYREF
  char Source[516]; // [rsp+880h] [rbp-298h] BYREF
  int j; // [rsp+A84h] [rbp-94h]
  unsigned int v11; // [rsp+A90h] [rbp-88h]
  unsigned int v12; // [rsp+A94h] [rbp-84h]
  unsigned int v13; // [rsp+A98h] [rbp-80h]
  unsigned int v14; // [rsp+A9Ch] [rbp-7Ch]
  unsigned int v15; // [rsp+AA0h] [rbp-78h]
  unsigned int v16; // [rsp+AA4h] [rbp-74h]
  unsigned int v17; // [rsp+AA8h] [rbp-70h]
  unsigned int v18; // [rsp+AACh] [rbp-6Ch]
  unsigned int v19; // [rsp+AB0h] [rbp-68h]
  unsigned int v20; // [rsp+AB4h] [rbp-64h]
  unsigned int v21; // [rsp+AB8h] [rbp-60h]
  unsigned int v22; // [rsp+ABCh] [rbp-5Ch]
  unsigned int v23; // [rsp+AC0h] [rbp-58h]
  const char *v24; // [rsp+AC8h] [rbp-50h]
  const char *v25; // [rsp+AD0h] [rbp-48h]
  const char *v26; // [rsp+AD8h] [rbp-40h]
  const char *v27; // [rsp+AE0h] [rbp-38h]
  const char *v28; // [rsp+AE8h] [rbp-30h]
  unsigned int v29; // [rsp+AF0h] [rbp-28h]
  unsigned int v30; // [rsp+AF4h] [rbp-24h]
  unsigned int v31; // [rsp+AF8h] [rbp-20h]

  v3 = &v6;
  for ( j = 0; j < 3; ++j )
  {
    memset_0(Buffer, 0, 0x800uLL);
    if ( j >= 1 )
      v11 = 0;
    else
      v11 = pMacro->mcr_Potion[j].Potion[2];
    if ( j >= 1 )
      v12 = 0;
    else
      v12 = pMacro->mcr_Potion[j].Potion[1];
    if ( j >= 1 )
      v13 = 0;
    else
      v13 = pMacro->mcr_Potion[j].Potion[0];
    sprintf(Buffer, " Update tbl_Macro Set hp=%d, fp=%d, sp=%d,", v13, v12, v11);
    memset_0(Source, 0, 0x200uLL);
    if ( j >= 3 )
      v14 = 0;
    else
      v14 = pMacro->mcr_Action[j].Action[3];
    if ( j >= 3 )
      v15 = 0;
    else
      v15 = pMacro->mcr_Action[j].Action[2];
    if ( j >= 3 )
      v16 = 0;
    else
      v16 = pMacro->mcr_Action[j].Action[1];
    if ( j >= 3 )
      v17 = 0;
    else
      v17 = pMacro->mcr_Action[j].Action[0];
    sprintf(Source, " action0=%d, action1=%d, action2=%d, action3=%d,", v17, v16, v15, v14);
    strcat_0(Buffer, Source);
    memset_0(Source, 0, 0x200uLL);
    if ( j >= 3 )
      v18 = 0;
    else
      v18 = pMacro->mcr_Action[j].Action[7];
    if ( j >= 3 )
      v19 = 0;
    else
      v19 = pMacro->mcr_Action[j].Action[6];
    if ( j >= 3 )
      v20 = 0;
    else
      v20 = pMacro->mcr_Action[j].Action[5];
    if ( j >= 3 )
      v21 = 0;
    else
      v21 = pMacro->mcr_Action[j].Action[4];
    sprintf(Source, " action4=%d, action5=%d, action6=%d, action7=%d,", v21, v20, v19, v18);
    strcat_0(Buffer, Source);
    memset_0(Source, 0, 0x200uLL);
    if ( j >= 3 )
      v22 = 0;
    else
      v22 = pMacro->mcr_Action[j].Action[9];
    if ( j >= 3 )
      v23 = 0;
    else
      v23 = pMacro->mcr_Action[j].Action[8];
    sprintf(Source, " action8=%d, action9=%d,", v23, v22);
    strcat_0(Buffer, Source);
    memset_0(Source, 0, 0x200uLL);
    if ( j >= 2 )
      v24 = "";
    else
      v24 = pMacro->mcr_Chat[j].Chat[4];
    if ( j >= 2 )
      v25 = "";
    else
      v25 = pMacro->mcr_Chat[j].Chat[3];
    if ( j >= 2 )
      v26 = "";
    else
      v26 = pMacro->mcr_Chat[j].Chat[2];
    if ( j >= 2 )
      v27 = "";
    else
      v27 = pMacro->mcr_Chat[j].Chat[1];
    if ( j >= 2 )
      v28 = "";
    else
      v28 = pMacro->mcr_Chat[j].Chat[0];
    sprintf(
      Source,
      " chat0='%s', chat1='%s', chat2='%s', chat3='%s', chat4='%s',",
      v28,
      v27,
      v26,
      v25,
      v24);
    strcat_0(Buffer, Source);
    memset_0(Source, 0, 0x200uLL);
    if ( j >= 1 )
      v29 = 0;
    else
      v29 = pMacro->mcr_Potion[j].PotionValue[2];
    if ( j >= 1 )
      v30 = 0;
    else
      v30 = pMacro->mcr_Potion[j].PotionValue[1];
    if ( j >= 1 )
      v31 = 0;
    else
      v31 = pMacro->mcr_Potion[j].PotionValue[0];
    sprintf(Source, " hpvalue=%d, fpvalue=%d, spvalue=%d", v31, v30, v29);
    strcat_0(Buffer, Source);
    memset_0(Source, 0, 0x200uLL);
    sprintf(Source, " where serial=%d and belt=%d", dwSerial, j);
    strcat_0(Buffer, Source);
    if ( this->m_bSaveDBLog )
      this->Log( Buffer);
    if ( !this->m_hStmtSelect && !this->ReConnectDataBase() )
    {
      this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", Buffer);
      return 0;
    }
    v7 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( v7 )
    {
      if ( v7 != 1 )
        break;
    }
    if ( this->m_bSaveDBLog )
      this->FmtLog( (wchar_t *)L"%s Success", Buffer);
  }
  if ( !v7 || v7 == 1 )
    return 1;
  if ( v7 != 100 )
  {
    this->ErrorMsgLog( v7, Buffer, "SQLExecDirect", this->m_hStmtSelect);
    this->ErrorAction( v7, this->m_hStmtSelect);
  }
  return 0;
}

bool CRFWorldDatabase::Update_PvpPointInfo(
        unsigned int dwSerial,
        __int16 *zClass,
        long double dPvpPoint)
{
  __int64 *v4; // rdi
  __int64 i; // rcx
  __int64 v7; // [rsp+0h] [rbp-178h] BYREF
  char Buffer[272]; // [rsp+50h] [rbp-128h] BYREF

  v4 = &v7;
  sprintf(
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
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-468h] BYREF
  char Buffer[1040]; // [rsp+40h] [rbp-428h] BYREF

  v2 = &v5;
  memset(Buffer, 0, 1024);
  sprintf(
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
  __int64 *v4; // rdi
  __int64 i; // rcx
  __int64 v7; // [rsp+0h] [rbp-168h] BYREF
  char Buffer[272]; // [rsp+40h] [rbp-128h] BYREF

  v4 = &v7;
  memset(Buffer, 0, 256);
  sprintf_s(
    Buffer,
    0x100uLL,
    "update [dbo].[tbl_event] set ClassRefineCnt=%d,ClassRefineDate=%d where avatorserial = %d",
    byRefineCnt,
    dwRefineDate,
    dwAvatorSerial);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Update_Set_Limit_Run( unsigned __int8 *pData, int iSize)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-468h] BYREF
  char _Dest[1024]; // [rsp+40h] [rbp-428h] BYREF

  v3 = &v6;
  memset(_Dest, 0, sizeof(_Dest));
  sprintf_s(_Dest, sizeof(_Dest), "update [dbo].[tbl_sf_delay] set [effect] = ? where [aserial] = 0");
  return this->ExecUpdateBinaryQuery(_Dest, reinterpret_cast<char *>(pData), iSize, 1);
}

bool CRFWorldDatabase::Update_SetActive(
        unsigned int dwSerial,
        char *pwszActiveName,
        unsigned __int8 bySlot)
{
  __int64 *v4; // rdi
  __int64 i; // rcx
  __int64 v7; // [rsp+0h] [rbp-168h] BYREF
  char Buffer[272]; // [rsp+40h] [rbp-128h] BYREF

  v4 = &v7;
  sprintf(Buffer, "{ CALL pUpdate_SetActive( %d, '%s', %d ) }", dwSerial, pwszActiveName, bySlot);
  return this->ExecUpdateQuery( Buffer, 1);
}

bool CRFWorldDatabase::Update_SetGuildMoney(
        unsigned int dwGuildSerial,
        long double dDalant,
        long double dGold)
{
  __int64 *v4; // rdi
  __int64 i; // rcx
  __int64 v7; // [rsp+0h] [rbp-168h] BYREF
  char Buffer[272]; // [rsp+40h] [rbp-128h] BYREF

  v4 = &v7;
  memset(Buffer, 0, 256);
  sprintf_s(
    Buffer,
    0x100uLL,
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
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-168h] BYREF
  char Buffer[272]; // [rsp+40h] [rbp-128h] BYREF

  v3 = &v6;
  sprintf(Buffer, "{ CALL pUpdate_SFDelay( %d, ? ) }", dwSerial);
  return this->ExecUpdateBinaryQuery(Buffer, reinterpret_cast<char *>(pSFDelay), 130, 0);
}

bool CRFWorldDatabase::Update_UserGuildData(
        unsigned int dwAvatorSerial,
        unsigned int dwGuildSerial,
        unsigned __int8 byGrade)
{
  __int64 *v4; // rdi
  __int64 i; // rcx
  __int64 v7; // [rsp+0h] [rbp-168h] BYREF
  char Buffer[272]; // [rsp+40h] [rbp-128h] BYREF

  v4 = &v7;
  memset(Buffer, 0, 256);
  sprintf(Buffer, "{ CALL pUpdate_UserGuildData_20070614( %d, %d, %d ) }", dwAvatorSerial, dwGuildSerial, byGrade);
  return this->ExecUpdateQuery( Buffer, 1);
}

char CRFWorldDatabase::Select_GuildData(
        unsigned int dwGuildSerial,
        _worlddb_guild_info::__guild_info *pGuildData)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-188h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+38h] [rbp-150h] BYREF
  SQLRETURN v8; // [rsp+44h] [rbp-144h]
  char Buffer[260]; // [rsp+60h] [rbp-128h] BYREF
  int v10; // [rsp+164h] [rbp-24h]

  v3 = &v6;
  v10 = 0;
  sprintf(Buffer, "select serial, grade, id, dalant, gold from tbl_guild where serial = %d", dwGuildSerial);
  if ( this->m_bSaveDBLog )
    this->Log( Buffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v8 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( !v8 || v8 == 1 )
    {
      v8 = SQLFetch(this->m_hStmtSelect);
      if ( !v8 || v8 == 1 )
      {
        v8 = SQLGetData(this->m_hStmtSelect, 1u, 4, pGuildData, 0LL, &StrLen_or_IndPtr);
        v8 = SQLGetData(this->m_hStmtSelect, 2u, 5, &pGuildData->byGuildGrade, 0LL, &StrLen_or_IndPtr);
        v8 = SQLGetData(this->m_hStmtSelect, 3u, 1, pGuildData->wszGuildName, 17LL, &StrLen_or_IndPtr);
        v8 = SQLGetData(this->m_hStmtSelect, 4u, 8, &pGuildData->dDalant, 0LL, &StrLen_or_IndPtr);
        v8 = SQLGetData(this->m_hStmtSelect, 5u, 8, &pGuildData->dGold, 0LL, &StrLen_or_IndPtr);
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        if ( this->m_bSaveDBLog )
          this->FmtLog( "%s Success", Buffer);
        return 1;
      }
      else
      {
        if ( v8 != 100 )
        {
          this->ErrorMsgLog( v8, Buffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction( v8, this->m_hStmtSelect);
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return 0;
      }
    }
    else
    {
      if ( v8 != 100 )
      {
        this->ErrorMsgLog( v8, Buffer, "SQLExecDirectA", this->m_hStmtSelect);
        this->ErrorAction( v8, this->m_hStmtSelect);
      }
      return 0;
    }
  }
  else
  {
    this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", Buffer);
    return 0;
  }
}

char CRFWorldDatabase::SelectAllGuildSerial(
        unsigned int *pdwCount,
        unsigned int *pdwSerial)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-2888h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+38h] [rbp-2850h] BYREF
  SQLRETURN v8; // [rsp+44h] [rbp-2844h]
  char Buffer[10256]; // [rsp+60h] [rbp-2828h] BYREF

  v3 = &v6;
  *pdwCount = 0;
  sprintf(Buffer, "select top %u [Serial] from [dbo].[tbl_guild] where [DCK]=0 order by serial", 500);
  if ( this->m_bSaveDBLog )
    this->Log( Buffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v8 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( !v8 || v8 == 1 )
    {
      while ( 1 )
      {
        v8 = SQLFetch(this->m_hStmtSelect);
        if ( v8 )
        {
          if ( v8 != 1 )
            break;
        }
        v8 = SQLGetData(this->m_hStmtSelect, 1u, 4, &pdwSerial[*pdwCount], 0LL, &StrLen_or_IndPtr);
        ++*pdwCount;
      }
      if ( this->m_hStmtSelect )
        SQLCloseCursor(this->m_hStmtSelect);
      if ( this->m_bSaveDBLog )
        this->FmtLog( "%s Success", Buffer);
      return 1;
    }
    else if ( v8 == 100 )
    {
      return 1;
    }
    else
    {
      this->ErrorMsgLog( v8, Buffer, "SQLExecDirectA", this->m_hStmtSelect);
      this->ErrorAction( v8, this->m_hStmtSelect);
      return 0;
    }
  }
  else
  {
    this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", Buffer);
    return 0;
  }
}

char CRFWorldDatabase::SelectAllGuildSerialGrade(
        unsigned int *pdwCount,
        unsigned int *pdwSerial,
        unsigned __int8 *pbyGrade)
{
  __int64 *v4; // rdi
  __int64 i; // rcx
  __int64 v7; // [rsp+0h] [rbp-2888h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+38h] [rbp-2850h] BYREF
  SQLRETURN v9; // [rsp+44h] [rbp-2844h]
  char Buffer[10256]; // [rsp+60h] [rbp-2828h] BYREF

  v4 = &v7;
  *pdwCount = 0;
  sprintf(Buffer, "select top %u [Serial], [Grade] from [dbo].[tbl_guild] where [DCK]=0 order by serial", 500);
  if ( this->m_bSaveDBLog )
    this->Log( Buffer);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v9 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( !v9 || v9 == 1 )
    {
      while ( 1 )
      {
        v9 = SQLFetch(this->m_hStmtSelect);
        if ( v9 )
        {
          if ( v9 != 1 )
            break;
        }
        v9 = SQLGetData(this->m_hStmtSelect, 1u, 4, &pdwSerial[*pdwCount], 0LL, &StrLen_or_IndPtr);
        v9 = SQLGetData(this->m_hStmtSelect, 2u, 65521, &pbyGrade[*pdwCount], 0LL, &StrLen_or_IndPtr);
        ++*pdwCount;
      }
      if ( this->m_hStmtSelect )
        SQLCloseCursor(this->m_hStmtSelect);
      if ( this->m_bSaveDBLog )
        this->FmtLog( "%s Success", Buffer);
      return 1;
    }
    else if ( v9 == 100 )
    {
      return 1;
    }
    else
    {
      this->ErrorMsgLog( v9, Buffer, "SQLExecDirectA", this->m_hStmtSelect);
      this->ErrorAction( v9, this->m_hStmtSelect);
      return 0;
    }
  }
  else
  {
    this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", Buffer);
    return 0;
  }
}

char CRFWorldDatabase::Update_CharSlot( unsigned int dwAvatorSerial)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-1D8h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+38h] [rbp-1A0h] BYREF
  SQLRETURN v7; // [rsp+44h] [rbp-194h]
  char Buffer[276]; // [rsp+60h] [rbp-178h] BYREF
  unsigned int TargetValue[9]; // [rsp+174h] [rbp-64h] BYREF
  unsigned int v10[7]; // [rsp+198h] [rbp-40h] BYREF
  int j; // [rsp+1B4h] [rbp-24h]

  v2 = &v5;
  memset(Buffer, 0, 256);
  TargetValue[0] = 0;
  sprintf(Buffer, "select AccountSerial from tbl_base where serial=%d and dck=0", dwAvatorSerial);
  if ( this->m_bSaveDBLog )
    this->Log( "Update_CharSlot");
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v7 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( !v7 || v7 == 1 )
    {
      v7 = SQLFetch(this->m_hStmtSelect);
      if ( !v7 || v7 == 1 )
      {
        v7 = SQLGetData(this->m_hStmtSelect, 1u, 65518, TargetValue, 0LL, &StrLen_or_IndPtr);
        if ( !v7 || v7 == 1 )
        {
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          memset(v10, 0, 12);
          sprintf(Buffer, "select serial from tbl_base where AccountSerial=%d and dck=0 order by slot", TargetValue[0]);
          v7 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
          if ( !v7 || v7 == 1 )
          {
            for ( j = 0; j < 3; ++j )
            {
              v7 = SQLFetch(this->m_hStmtSelect);
              if ( v7 )
              {
                if ( v7 != 1 )
                  break;
              }
              v7 = SQLGetData(this->m_hStmtSelect, 1u, 65518, &v10[j], 0LL, &StrLen_or_IndPtr);
            }
            if ( this->m_hStmtSelect )
              SQLCloseCursor(this->m_hStmtSelect);
            for ( j = 0; j < 3; ++j )
            {
              if ( v10[j] )
              {
                sprintf(Buffer, "update tbl_base set slot=%d where serial=%d", j, v10[j]);
                this->ExecUpdateQuery( Buffer, 1);
              }
            }
            if ( this->m_bSaveDBLog )
              this->Log( "Update_CharSlot Success");
            return 1;
          }
          else
          {
            if ( v7 != 100 )
            {
              this->ErrorMsgLog( v7, Buffer, "SQLExecDirectA", this->m_hStmtSelect);
              this->ErrorAction( v7, this->m_hStmtSelect);
            }
            return 0;
          }
        }
        else
        {
          this->ErrorMsgLog( v7, Buffer, "SQLGetData", this->m_hStmtSelect);
          this->ErrorAction( v7, this->m_hStmtSelect);
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          return 0;
        }
      }
      else
      {
        if ( v7 != 100 )
        {
          this->ErrorMsgLog( v7, Buffer, "SQLFetch", this->m_hStmtSelect);
          this->ErrorAction( v7, this->m_hStmtSelect);
        }
        if ( this->m_hStmtSelect )
          SQLCloseCursor(this->m_hStmtSelect);
        return 0;
      }
    }
    else
    {
      if ( v7 != 100 )
      {
        this->ErrorMsgLog( v7, Buffer, "SQLExecDirectA", this->m_hStmtSelect);
        this->ErrorAction( v7, this->m_hStmtSelect);
      }
      return 0;
    }
  }
  else
  {
    this->FmtLog( "ReConnectDataBase Fail. Query : %s", Buffer);
    return 0;
  }
}

char CRFWorldDatabase::Update_GuildRank_Step1( char *szDate)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-478h] BYREF
  SQLRETURN v6; // [rsp+30h] [rbp-448h]
  char Buffer[1040]; // [rsp+50h] [rbp-428h] BYREF

  v2 = &v5;
  memset(Buffer, 0, 1024);
  this->FmtLog(
    "CRFWorldDatabase::Update_GuildRank_Step1(char* szDate(%s)) : Update_GuildRank Start!",
    szDate);
  sprintf_s(Buffer, 0x400uLL, "tbl_GuildRank%s", szDate);
  if ( this->TableExist( Buffer) )
  {
    sprintf_s(Buffer, 0x400uLL, "drop table [dbo].[tbl_GuildRank%s]", szDate);
    if ( !this->ExecUpdateQuery( Buffer, 1) )
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_GuildRank_Step1(szDate(%s)) : Drop tbl_GuildRank%s Table Fail!",
        szDate,
        szDate);
      return 0;
    }
  }
  this->SetAutoCommitMode( 0);
  this->FmtLog(
    "CRFWorldDatabase::Update_GuildRank_Step1(char* szDate(%s)) : Start Create #tbl_GuildRankTemp Table",
    szDate);
  sprintf_s(
    Buffer,
    0x400uLL,
    "select top %d g.GuildSerial as serial, sum(b.lv) + sum(g.pvppoint)/10000 as GuildPower into #tbl_GuildRankTemp from "
    "tbl_general as g join tbl_base as b on g.serial = b.serial group by g.GuildSerial having g.guildserial >= 0",
    500);
  v6 = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)Buffer, -3);
  if ( v6 && v6 != 1 )
  {
    if ( v6 != 100 )
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_GuildRank_Step1(char* szDate(%s)) : Create #tbl_GuildRankTemp Table Fail SQL_ERROR!",
        szDate);
      this->ErrorMsgLog( v6, Buffer, "SQLExecDirect", this->m_hStmtUpdate);
      this->RollbackTransaction();
      this->SetAutoCommitMode( 1);
      return 0;
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
    Buffer,
    0x400uLL,
    "select IDENTITY(int, 1, 1) AS Rank, serial, GuildPower, -1 as Rate, 1 as Grade into [dbo].[tbl_GuildRank%s] from #tb"
    "l_GuildRankTemp order by GuildPower desc",
    szDate);
  v6 = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)Buffer, -3);
  if ( v6 && v6 != 1 )
  {
    if ( v6 != 100 )
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_GuildRank_Step1(char* szDate(%s)) : Create tbl_GuildRank%s Table Fail SQL_ERROR!",
        szDate,
        szDate);
      this->ErrorMsgLog( v6, Buffer, "SQLExecDirect", this->m_hStmtUpdate);
      this->RollbackTransaction();
      this->SetAutoCommitMode( 1);
      return 0;
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
  sprintf(Buffer, "drop table #tbl_GuildRankTemp");
  if ( this->ExecUpdateQuery( Buffer, 1) )
  {
    this->CommitTransaction();
    this->SetAutoCommitMode( 1);
    this->FmtLog(
      "CRFWorldDatabase::Update_GuildRank_Step1(char* szDate(%s)) : End drop table #tbl_GuildRankTemp",
      szDate);
    return 1;
  }
  else
  {
    this->FmtLog(
      "CRFWorldDatabase::Update_GuildRank_Step1(char* szDate(%s)) : drop table #tbl_GuildRankTemp Fail!",
      szDate);
    this->RollbackTransaction();
    this->SetAutoCommitMode( 1);
    return 0;
  }
}

char CRFWorldDatabase::Update_GuildRank_Step2( char *szDate)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-468h] BYREF
  char Buffer[1040]; // [rsp+40h] [rbp-428h] BYREF

  v2 = &v5;
  this->FmtLog( "CRFWorldDatabase::Update_GuildRank_Step2(char* szDate(%s)) : Start Update Rate", szDate);
  memset(Buffer, 0, 1024);
  sprintf_s(
    Buffer,
    0x400uLL,
    "update tbl_GuildRank%s set Rate = ( (Rank*100)/(select count(*) from tbl_GuildRank%s) )",
    szDate,
    szDate);
  if ( this->ExecUpdateQuery( Buffer, 0) )
  {
    sprintf_s(Buffer, 0x400uLL, "update tbl_GuildRank%s set Grade = 2 where rate <= 95 and GuildPower >= 300", szDate);
    if ( this->ExecUpdateQuery( Buffer, 0) )
    {
      sprintf_s(
        Buffer,
        0x400uLL,
        "update tbl_GuildRank%s set Grade = 3 where rate <= 85 and GuildPower >= 1500",
        szDate);
      if ( this->ExecUpdateQuery( Buffer, 0) )
      {
        sprintf_s(
          Buffer,
          0x400uLL,
          "update tbl_GuildRank%s set Grade = 4 where rate <= 65 and GuildPower >= 3000",
          szDate);
        if ( this->ExecUpdateQuery( Buffer, 0) )
        {
          sprintf_s(
            Buffer,
            0x400uLL,
            "update tbl_GuildRank%s set Grade = 5 where rate <= 35 and GuildPower >= 10000",
            szDate);
          if ( this->ExecUpdateQuery( Buffer, 0) )
          {
            sprintf_s(
              Buffer,
              0x400uLL,
              "update tbl_GuildRank%s set Grade = 6 where rate <= 15 and GuildPower >= 25000",
              szDate);
            if ( this->ExecUpdateQuery( Buffer, 0) )
            {
              sprintf_s(
                Buffer,
                0x400uLL,
                "update tbl_GuildRank%s set Grade = 7 where rate <= 5 and GuildPower >= 30000",
                szDate);
              if ( this->ExecUpdateQuery( Buffer, 0) )
              {
                this->FmtLog(
                  "CRFWorldDatabase::Update_GuildRank_Step2(char* szDate(%s)) : End Update Rate",
                  szDate);
                return 1;
              }
              else
              {
                this->FmtLog(
                  "CRFWorldDatabase::Update_GuildRank_Step2(char* szDate(%s)) : %s Fail!",
                  szDate,
                  Buffer);
                return 0;
              }
            }
            else
            {
              this->FmtLog(
                "CRFWorldDatabase::Update_GuildRank_Step2(char* szDate(%s)) : %s Fail!",
                szDate,
                Buffer);
              return 0;
            }
          }
          else
          {
            this->FmtLog(
              "CRFWorldDatabase::Update_GuildRank_Step2(char* szDate(%s)) : %s Fail!",
              szDate,
              Buffer);
            return 0;
          }
        }
        else
        {
          this->FmtLog(
            "CRFWorldDatabase::Update_GuildRank_Step2(char* szDate(%s)) : %s Fail!",
            szDate,
            Buffer);
          return 0;
        }
      }
      else
      {
        this->FmtLog(
          "CRFWorldDatabase::Update_GuildRank_Step2(char* szDate(%s)) : %s Fail!",
          szDate,
          Buffer);
        return 0;
      }
    }
    else
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_GuildRank_Step2(char* szDate(%s)) : %s Fail!",
        szDate,
        Buffer);
      return 0;
    }
  }
  else
  {
    this->FmtLog(
      "CRFWorldDatabase::Update_GuildRank_Step2(char* szDate(%s)) : %s Fail!",
      szDate,
      Buffer);
    return 0;
  }
}

char CRFWorldDatabase::Update_GuildRank_Step3( char *szDate)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-458h] BYREF
  char Buffer[1040]; // [rsp+30h] [rbp-428h] BYREF

  v2 = &v5;
  this->FmtLog(
    "CRFWorldDatabase::Update_GuildRank_Step3(char* szDate(%s)) : Start Drop tbl_GuildRankToday Table",
    szDate);
  memset(Buffer, 0, 1024);
  if ( !this->TableExist( "tbl_GuildRankToday")
    || (sprintf_s(Buffer, 0x400uLL, "Drop Table tbl_GuildRankToday"), this->ExecUpdateQuery( Buffer, 1)) )
  {
    this->FmtLog(
      "CRFWorldDatabase::Update_GuildRank_Step3(char* szDate(%s)) : End Drop tbl_GuildRankToday Table",
      szDate);
    this->FmtLog(
      "CRFWorldDatabase::Update_GuildRank_Step3(char* szDate(%s)) : Start Create tbl_GuildRankToday Table",
      szDate);
    sprintf_s(
      Buffer,
      0x400uLL,
      "select Rank, Rate, Serial, GuildPower, Grade into [dbo].[tbl_GuildRankToday] from tbl_GuildRank%s",
      szDate);
    if ( this->ExecUpdateQuery( Buffer, 0) )
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_GuildRank_Step3(char* szDate(%s)) : End Create tbl_GuildRankToday Table",
        szDate);
      this->FmtLog(
        "CRFWorldDatabase::Update_GuildRank_Step3(char* szDate(%s)) : Update_GuildRank Success",
        szDate);
      return 1;
    }
    else
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_GuildRank_Step3(char* szDate(%s)) : %s Fail!",
        szDate,
        Buffer);
      return 0;
    }
  }
  else
  {
    this->FmtLog(
      "CRFWorldDatabase::Update_GuildRank_Step3(char* szDate(%s)) : %s Fail!",
      szDate,
      Buffer);
    return 0;
  }
}

char CRFWorldDatabase::Update_RaceRank_Step1( char *szDate)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-478h] BYREF
  SQLRETURN v6; // [rsp+30h] [rbp-448h]
  char Buffer[1028]; // [rsp+50h] [rbp-428h] BYREF
  unsigned int ConnectTime_AddBySec; // [rsp+454h] [rbp-24h]

  v2 = &v5;
  this->FmtLog( "CRFWorldDatabase Update_RaceRank(szDate(%s)) Start!", szDate);
  memset(Buffer, 0, 1024);
  if ( !this->m_hStmtUpdate && !this->ReConnectDataBase() )
  {
    this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", Buffer);
    return 0;
  }
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step1(szDate(%s)) : Start Create #tbl_PvpRankB Table",
    szDate);
  this->SetAutoCommitMode( 0);
  ConnectTime_AddBySec = GetConnectTime_AddBySec(-2592000);
  sprintf_s(
    Buffer,
    0x400uLL,
    "select IDENTITY(int, 1, 1) AS Rank, -1 as Rate, tbl_base.serial, 0 as Race,  tbl_base.name, tbl_base.lv, tbl_general"
    ".PvpPoint, tbl_general.GuildSerial into #tbl_PvpRankB from tbl_general, tbl_base where tbl_base.serial=tbl_general.s"
    "erial and tbl_base.dck=0 and tbl_base.AccountSerial < 200000000 and tbl_general.class0 <> -1 and tbl_base.race in (0"
    ",1) and tbl_base.LastConnTime > %d order by tbl_general.PvpPoint desc",
    ConnectTime_AddBySec);
  v6 = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)Buffer, -3);
  if ( v6 && v6 != 1 )
  {
    if ( v6 != 100 )
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_RaceRank_Step1(szDate(%s)) : Create #tbl_PvpRankB Table Fail SQL_ERROR!",
        szDate);
      this->ErrorMsgLog( v6, Buffer, "SQLExecDirect", this->m_hStmtUpdate);
      this->RollbackTransaction();
      this->SetAutoCommitMode( 1);
      return 0;
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
  sprintf(Buffer, "update #tbl_PvpRankB set Rate = ( (Rank*10000)/(select count(*) from #tbl_PvpRankB) )");
  v6 = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)Buffer, -3);
  if ( v6 && v6 != 1 )
  {
    if ( v6 != 100 )
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_RaceRank_Step1(szDate(%s)) : Set Rate #tbl_PvpRankB Table Fail SQL_ERROR!",
        szDate);
      this->ErrorMsgLog( v6, Buffer, "SQLExecDirect", this->m_hStmtUpdate);
      this->RollbackTransaction();
      this->SetAutoCommitMode( 1);
      return 0;
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
  return 1;
}

char CRFWorldDatabase::Update_RaceRank_Step2( char *szDate)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-478h] BYREF
  SQLRETURN v6; // [rsp+30h] [rbp-448h]
  char Buffer[1028]; // [rsp+50h] [rbp-428h] BYREF
  unsigned int ConnectTime_AddBySec; // [rsp+454h] [rbp-24h]

  v2 = &v5;
  memset(Buffer, 0, 1024);
  if ( !this->m_hStmtUpdate && !this->ReConnectDataBase() )
  {
    this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", Buffer);
    return 0;
  }
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step2(szDate(%s)) : Start Create #tbl_PvpRankC Table",
    szDate);
  this->SetAutoCommitMode( 0);
  ConnectTime_AddBySec = GetConnectTime_AddBySec(-2592000);
  sprintf_s(
    Buffer,
    0x400uLL,
    "select IDENTITY(int, 1, 1) AS Rank, -1 as Rate, tbl_base.serial, 1 as Race, tbl_base.name, tbl_base.lv, tbl_general."
    "PvpPoint, tbl_general.GuildSerial into #tbl_PvpRankC from tbl_general, tbl_base where tbl_base.serial=tbl_general.se"
    "rial and tbl_base.dck=0 and tbl_base.AccountSerial<200000000 and tbl_general.class0 <> -1 and tbl_base.race in (2,3)"
    " and tbl_base.LastConnTime > %d order by tbl_general.PvpPoint desc",
    ConnectTime_AddBySec);
  v6 = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)Buffer, -3);
  if ( v6 && v6 != 1 )
  {
    if ( v6 != 100 )
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_RaceRank_Step2(szDate(%s)) : Create #tbl_PvpRankB Table Fail SQL_ERROR!",
        szDate);
      this->ErrorMsgLog( v6, Buffer, "SQLExecDirect", this->m_hStmtUpdate);
      this->RollbackTransaction();
      this->SetAutoCommitMode( 1);
      if ( !this->Update_RaceRank_Step_6_1( szDate) )
        this->Update_RaceRank_Step_6_1( szDate);
      return 0;
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
  sprintf(Buffer, "update #tbl_PvpRankC set Rate = ( (Rank*10000)/(select count(*) from #tbl_PvpRankC) )");
  v6 = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)Buffer, -3);
  if ( !v6 || v6 == 1 )
    goto LABEL_21;
  if ( v6 == 100 )
  {
    this->FmtLog(
      "CRFWorldDatabase::Update_RaceRank_Step1(szDate(%s)) : Set Rate #tbl_PvpRankC Table Fail NO_DATA!",
      szDate);
LABEL_21:
    this->CommitTransaction();
    this->FmtLog(
      "CRFWorldDatabase::Update_RaceRank_Step2(szDate(%s)) : End Set Rate #tbl_PvpRankC Table",
      szDate);
    this->SetAutoCommitMode( 1);
    return 1;
  }
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step1(szDate(%s)) : Set Rate #tbl_PvpRankC Table Fail SQL_ERROR!",
    szDate);
  this->ErrorMsgLog( v6, Buffer, "SQLExecDirect", this->m_hStmtUpdate);
  this->RollbackTransaction();
  this->SetAutoCommitMode( 1);
  if ( !this->Update_RaceRank_Step_6_1( szDate) )
    this->Update_RaceRank_Step_6_1( szDate);
  return 0;
}

char CRFWorldDatabase::Update_RaceRank_Step3( char *szDate)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-478h] BYREF
  SQLRETURN v6; // [rsp+30h] [rbp-448h]
  char Buffer[1028]; // [rsp+50h] [rbp-428h] BYREF
  unsigned int ConnectTime_AddBySec; // [rsp+454h] [rbp-24h]

  v2 = &v5;
  memset(Buffer, 0, 1024);
  if ( !this->m_hStmtUpdate && !this->ReConnectDataBase() )
  {
    this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", Buffer);
    return 0;
  }
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step2(szDate(%s)) : Start Create #tbl_PvpRankC Table",
    szDate);
  this->SetAutoCommitMode( 0);
  ConnectTime_AddBySec = GetConnectTime_AddBySec(-2592000);
  sprintf_s(
    Buffer,
    0x400uLL,
    "select IDENTITY(int, 1, 1) AS Rank, -1 as Rate, tbl_base.serial, 2 as Race,  tbl_base.name, tbl_base.lv, tbl_general"
    ".PvpPoint, tbl_general.GuildSerial into #tbl_PvpRankA from tbl_general, tbl_base where tbl_base.serial=tbl_general.s"
    "erial and tbl_base.dck=0 and tbl_base.AccountSerial<200000000 and tbl_general.class0 <> -1 and tbl_base.race in (4) "
    "and tbl_base.LastConnTime > %d order by tbl_general.PvpPoint desc",
    ConnectTime_AddBySec);
  v6 = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)Buffer, -3);
  if ( v6 && v6 != 1 )
  {
    if ( v6 != 100 )
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_RaceRank_Step2(szDate(%s)) : Create #tbl_PvpRankB Table Fail SQL_ERROR!",
        szDate);
      this->ErrorMsgLog( v6, Buffer, "SQLExecDirect", this->m_hStmtUpdate);
      this->RollbackTransaction();
      this->SetAutoCommitMode( 1);
      if ( !this->Update_RaceRank_Step_6_1( szDate) )
        this->Update_RaceRank_Step_6_1( szDate);
      if ( !this->Update_RaceRank_Step_6_2( szDate) )
        this->Update_RaceRank_Step_6_2( szDate);
      return 0;
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
  sprintf(Buffer, "update #tbl_PvpRankA set Rate = ( (Rank*10000)/(select count(*) from #tbl_PvpRankA) )");
  v6 = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)Buffer, -3);
  if ( !v6 || v6 == 1 )
    goto LABEL_25;
  if ( v6 == 100 )
  {
    this->FmtLog(
      "CRFWorldDatabase::Update_RaceRank_Step1(szDate(%s)) : Set Rate #tbl_PvpRankA Table Fail NO_DATA!",
      szDate);
LABEL_25:
    this->CommitTransaction();
    this->FmtLog(
      "CRFWorldDatabase::Update_RaceRank_Step3(szDate(%s)) : End Set Rate #tbl_PvpRankA Table",
      szDate);
    return 1;
  }
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step1(szDate(%s)) : Set Rate #tbl_PvpRankA Table Fail SQL_ERROR!",
    szDate);
  this->ErrorMsgLog( v6, Buffer, "SQLExecDirect", this->m_hStmtUpdate);
  this->RollbackTransaction();
  this->SetAutoCommitMode( 1);
  if ( !this->Update_RaceRank_Step_6_1( szDate) )
    this->Update_RaceRank_Step_6_1( szDate);
  if ( !this->Update_RaceRank_Step_6_2( szDate) )
    this->Update_RaceRank_Step_6_2( szDate);
  return 0;
}

char CRFWorldDatabase::Update_RaceRank_Step4( char *szDate)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-478h] BYREF
  SQLRETURN v6; // [rsp+30h] [rbp-448h]
  char Buffer[1040]; // [rsp+50h] [rbp-428h] BYREF

  v2 = &v5;
  memset(Buffer, 0, 1024);
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step4(szDate(%s)) : Start Create tbl_PvpRank%s Table",
    szDate,
    szDate);
  sprintf_s(Buffer, 0x400uLL, "tbl_PvpRank%s", szDate);
  if ( this->TableExist( Buffer) )
  {
    sprintf_s(Buffer, 0x400uLL, "drop table [dbo].[tbl_PvpRank%s]", szDate);
    if ( !this->ExecUpdateQuery( Buffer, 1) )
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_RaceRank_Step4(szDate(%s)) : Drop tbl_PvpRank%s Table Fail!",
        szDate,
        szDate);
      return 0;
    }
  }
  this->SetAutoCommitMode( 0);
  sprintf_s(
    Buffer,
    0x400uLL,
    "CREATE TABLE [dbo].[tbl_PvpRank%s] ( [Rank] [int] NOT NULL, [Rate] [int] NOT NULL, [serial] [int] NOT NULL ,[name] ["
    "varchar] (17) NOT NULL ,[lv] [int] NOT NULL, [Race] [int] NOT NULL, [Grade] [smallint] NOT NULL ,[PvpPoint] [float] "
    "NOT NULL, [GuildSerial] [int] NOT NULL ,[GuildName] [varchar] (17) NOT NULL ,CONSTRAINT PK_tbl_PvpRank%s PRIMARY KEY"
    "  CLUSTERED ( [serial] ) on [PRIMARY] ) ON [PRIMARY]",
    szDate,
    szDate);
  if ( !this->ExecUpdateQuery( Buffer, 1) )
  {
    this->FmtLog(
      "CRFWorldDatabase::Update_RaceRank_Step4(szDate(%s)) : Create tbl_PvpRank%s Table Fail!",
      szDate,
      szDate);
    this->RollbackTransaction();
    this->SetAutoCommitMode( 1);
    return 0;
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
    Buffer,
    0x400uLL,
    "insert tbl_PvpRank%s(Rank, Rate, Serial, Name, lv, race, PvpPoint, Grade, GuildSerial, GuildName) select 0, rate, se"
    "rial, name, lv, race, PvpPoint, 0, GuildSerial, '*' from #tbl_PvpRankB union select 0, rate, serial, name, lv, race,"
    " PvpPoint, 0, GuildSerial, '*' from #tbl_PvpRankC union select 0, rate, serial, name, lv, race, PvpPoint, 0, GuildSe"
    "rial, '*' from #tbl_PvpRankA ",
    szDate);
  v6 = SQLExecDirectA(this->m_hStmtUpdate, (SQLCHAR *)Buffer, -3);
  if ( v6 && v6 != 1 )
  {
    if ( v6 != 100 )
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_RaceRank(szDate(%s)) : Union #tbl_PvpRankB,C,A To tbl_PvpRank%s Fail SQL_ERROR!",
        szDate,
        szDate);
      this->ErrorMsgLog( v6, Buffer, "SQLExecDirect", this->m_hStmtUpdate);
      this->RollbackTransaction();
      this->SetAutoCommitMode( 1);
      this->Update_RaceRank_Step6( szDate);
      return 0;
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
  return 1;
}

char CRFWorldDatabase::Update_RaceRank_Step5( char *szDate)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-458h] BYREF
  char Buffer[1040]; // [rsp+30h] [rbp-428h] BYREF

  v2 = &v5;
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : Start Set Grade tbl_PvpRank%s",
    szDate,
    szDate);
  memset(Buffer, 0, 1024);
  sprintf_s(Buffer, 0x400uLL, "update tbl_PvpRank%s set grade=0", szDate);
  if ( this->ExecUpdateQuery( Buffer, 0) )
  {
    sprintf_s(Buffer, 0x400uLL, "update tbl_PvpRank%s set grade=3 where lv >= 30 and lv <= 34 and rate <= 6500", szDate);
    if ( this->ExecUpdateQuery( Buffer, 0) )
    {
      sprintf_s(
        Buffer,
        0x400uLL,
        "update tbl_PvpRank%s set grade=2 where lv >= 30 and lv <= 34 and rate > 6500 and rate <= 8500",
        szDate);
      if ( this->ExecUpdateQuery( Buffer, 0) )
      {
        sprintf_s(
          Buffer,
          0x400uLL,
          "update tbl_PvpRank%s set grade=1 where lv >= 30 and lv <= 34 and rate > 8500 and rate <= 9500",
          szDate);
        if ( this->ExecUpdateQuery( Buffer, 0) )
        {
          sprintf_s(
            Buffer,
            0x400uLL,
            "update tbl_PvpRank%s set grade=4 where lv >= 35 and lv <= 39 and rate <= 3500",
            szDate);
          if ( this->ExecUpdateQuery( Buffer, 0) )
          {
            sprintf_s(
              Buffer,
              0x400uLL,
              "update tbl_PvpRank%s set grade=3 where lv >= 35 and lv <= 39 and rate > 3500 and rate <= 6500",
              szDate);
            if ( this->ExecUpdateQuery( Buffer, 0) )
            {
              sprintf_s(
                Buffer,
                0x400uLL,
                "update tbl_PvpRank%s set grade=2 where lv >= 35 and lv <= 39 and rate > 6500 and rate <= 8500",
                szDate);
              if ( this->ExecUpdateQuery( Buffer, 0) )
              {
                sprintf_s(
                  Buffer,
                  0x400uLL,
                  "update tbl_PvpRank%s set grade=1 where lv >= 35 and lv <= 39 and rate > 8500 and rate <= 9500",
                  szDate);
                if ( this->ExecUpdateQuery( Buffer, 0) )
                {
                  sprintf_s(
                    Buffer,
                    0x400uLL,
                    "update tbl_PvpRank%s set grade=5 where lv >= 40 and lv <= 44 and rate <= 1500",
                    szDate);
                  if ( this->ExecUpdateQuery( Buffer, 0) )
                  {
                    sprintf_s(
                      Buffer,
                      0x400uLL,
                      "update tbl_PvpRank%s set grade=4 where lv >= 40 and lv <= 44 and rate > 1500 and rate <= 3500",
                      szDate);
                    if ( this->ExecUpdateQuery( Buffer, 0) )
                    {
                      sprintf_s(
                        Buffer,
                        0x400uLL,
                        "update tbl_PvpRank%s set grade=3 where lv >= 40 and lv <= 44 and rate > 3500 and rate <= 6500",
                        szDate);
                      if ( this->ExecUpdateQuery( Buffer, 0) )
                      {
                        sprintf_s(
                          Buffer,
                          0x400uLL,
                          "update tbl_PvpRank%s set grade=2 where lv >= 40 and lv <= 44 and rate > 6500 and rate <= 8500",
                          szDate);
                        if ( this->ExecUpdateQuery( Buffer, 0) )
                        {
                          sprintf_s(
                            Buffer,
                            0x400uLL,
                            "update tbl_PvpRank%s set grade=1 where lv >= 40 and lv <= 44 and rate > 8500 and rate <= 9500",
                            szDate);
                          if ( this->ExecUpdateQuery( Buffer, 0) )
                          {
                            sprintf_s(
                              Buffer,
                              0x400uLL,
                              "update tbl_PvpRank%s set grade=6 where lv >= 45 and lv <= 49 and rate <= 500",
                              szDate);
                            if ( this->ExecUpdateQuery( Buffer, 0) )
                            {
                              sprintf_s(
                                Buffer,
                                0x400uLL,
                                "update tbl_PvpRank%s set grade=5 where lv >= 45 and lv <= 49 and rate > 500 and rate <= 1500",
                                szDate);
                              if ( this->ExecUpdateQuery( Buffer, 0) )
                              {
                                sprintf_s(
                                  Buffer,
                                  0x400uLL,
                                  "update tbl_PvpRank%s set grade=4 where lv >= 45 and lv <= 49 and rate > 1500 and rate <= 3500",
                                  szDate);
                                if ( this->ExecUpdateQuery( Buffer, 0) )
                                {
                                  sprintf_s(
                                    Buffer,
                                    0x400uLL,
                                    "update tbl_PvpRank%s set grade=3 where lv >= 45 and lv <= 49 and rate > 3500 and rate <= 6500",
                                    szDate);
                                  if ( this->ExecUpdateQuery( Buffer, 0) )
                                  {
                                    sprintf_s(
                                      Buffer,
                                      0x400uLL,
                                      "update tbl_PvpRank%s set grade=2 where lv >= 45 and lv <= 49 and rate > 6500 and rate <= 8500",
                                      szDate);
                                    if ( this->ExecUpdateQuery( Buffer, 0) )
                                    {
                                      sprintf_s(
                                        Buffer,
                                        0x400uLL,
                                        "update tbl_PvpRank%s set grade=1 where lv >= 45 and lv <= 49 and rate > 8500 and rate <= 9500",
                                        szDate);
                                      if ( this->ExecUpdateQuery( Buffer, 0) )
                                      {
                                        sprintf_s(
                                          Buffer,
                                          0x400uLL,
                                          "update tbl_PvpRank%s set grade=7 where lv >= 50 and rate <= 100",
                                          szDate);
                                        if ( this->ExecUpdateQuery( Buffer, 0) )
                                        {
                                          sprintf_s(
                                            Buffer,
                                            0x400uLL,
                                            "update tbl_PvpRank%s set grade=6 where lv >= 50 and rate > 100 and rate <= 500",
                                            szDate);
                                          if ( this->ExecUpdateQuery( Buffer, 0) )
                                          {
                                            sprintf_s(
                                              Buffer,
                                              0x400uLL,
                                              "update tbl_PvpRank%s set grade=5 where lv >= 50 and rate > 500 and rate <= 1500",
                                              szDate);
                                            if ( this->ExecUpdateQuery( Buffer, 0) )
                                            {
                                              sprintf_s(
                                                Buffer,
                                                0x400uLL,
                                                "update tbl_PvpRank%s set grade=4 where lv >= 50 and rate > 1500 and rate <= 3500",
                                                szDate);
                                              if ( this->ExecUpdateQuery( Buffer, 0) )
                                              {
                                                sprintf_s(
                                                  Buffer,
                                                  0x400uLL,
                                                  "update tbl_PvpRank%s set grade=3 where lv >= 50 and rate > 3500 and rate <= 6500",
                                                  szDate);
                                                if ( this->ExecUpdateQuery( Buffer, 0) )
                                                {
                                                  sprintf_s(
                                                    Buffer,
                                                    0x400uLL,
                                                    "update tbl_PvpRank%s set grade=2 where lv >= 50 and rate > 6500 and rate <= 8500",
                                                    szDate);
                                                  if ( this->ExecUpdateQuery( Buffer, 0) )
                                                  {
                                                    sprintf_s(
                                                      Buffer,
                                                      0x400uLL,
                                                      "update tbl_PvpRank%s set grade=1 where lv >= 50 and rate > 8500 and rate <= 9500",
                                                      szDate);
                                                    if ( this->ExecUpdateQuery( Buffer, 0) )
                                                    {
                                                      this->FmtLog(
                                                        "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : End Set Gr"
                                                        "ade tbl_PvpRank%s",
                                                        szDate,
                                                        szDate);
                                                      return 1;
                                                    }
                                                    else
                                                    {
                                                      this->FmtLog(
                                                        "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                                        szDate,
                                                        Buffer);
                                                      this->Update_RaceRank_Step6( szDate);
                                                      return 0;
                                                    }
                                                  }
                                                  else
                                                  {
                                                    this->FmtLog(
                                                      "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                                      szDate,
                                                      Buffer);
                                                    this->Update_RaceRank_Step6( szDate);
                                                    return 0;
                                                  }
                                                }
                                                else
                                                {
                                                  this->FmtLog(
                                                    "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                                    szDate,
                                                    Buffer);
                                                  this->Update_RaceRank_Step6( szDate);
                                                  return 0;
                                                }
                                              }
                                              else
                                              {
                                                this->FmtLog(
                                                  "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                                  szDate,
                                                  Buffer);
                                                this->Update_RaceRank_Step6( szDate);
                                                return 0;
                                              }
                                            }
                                            else
                                            {
                                              this->FmtLog(
                                                "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                                szDate,
                                                Buffer);
                                              this->Update_RaceRank_Step6( szDate);
                                              return 0;
                                            }
                                          }
                                          else
                                          {
                                            this->FmtLog(
                                              "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                              szDate,
                                              Buffer);
                                            this->Update_RaceRank_Step6( szDate);
                                            return 0;
                                          }
                                        }
                                        else
                                        {
                                          this->FmtLog(
                                            "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                            szDate,
                                            Buffer);
                                          this->Update_RaceRank_Step6( szDate);
                                          return 0;
                                        }
                                      }
                                      else
                                      {
                                        this->FmtLog(
                                          "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                          szDate,
                                          Buffer);
                                        this->Update_RaceRank_Step6( szDate);
                                        return 0;
                                      }
                                    }
                                    else
                                    {
                                      this->FmtLog(
                                        "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                        szDate,
                                        Buffer);
                                      this->Update_RaceRank_Step6( szDate);
                                      return 0;
                                    }
                                  }
                                  else
                                  {
                                    this->FmtLog(
                                      "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                      szDate,
                                      Buffer);
                                    this->Update_RaceRank_Step6( szDate);
                                    return 0;
                                  }
                                }
                                else
                                {
                                  this->FmtLog(
                                    "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                    szDate,
                                    Buffer);
                                  this->Update_RaceRank_Step6( szDate);
                                  return 0;
                                }
                              }
                              else
                              {
                                this->FmtLog(
                                  "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                  szDate,
                                  Buffer);
                                this->Update_RaceRank_Step6( szDate);
                                return 0;
                              }
                            }
                            else
                            {
                              this->FmtLog(
                                "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                                szDate,
                                Buffer);
                              this->Update_RaceRank_Step6( szDate);
                              return 0;
                            }
                          }
                          else
                          {
                            this->FmtLog(
                              "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                              szDate,
                              Buffer);
                            this->Update_RaceRank_Step6( szDate);
                            return 0;
                          }
                        }
                        else
                        {
                          this->FmtLog(
                            "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                            szDate,
                            Buffer);
                          this->Update_RaceRank_Step6( szDate);
                          return 0;
                        }
                      }
                      else
                      {
                        this->FmtLog(
                          "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                          szDate,
                          Buffer);
                        this->Update_RaceRank_Step6( szDate);
                        return 0;
                      }
                    }
                    else
                    {
                      this->FmtLog(
                        "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                        szDate,
                        Buffer);
                      this->Update_RaceRank_Step6( szDate);
                      return 0;
                    }
                  }
                  else
                  {
                    this->FmtLog(
                      "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                      szDate,
                      Buffer);
                    this->Update_RaceRank_Step6( szDate);
                    return 0;
                  }
                }
                else
                {
                  this->FmtLog(
                    "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                    szDate,
                    Buffer);
                  this->Update_RaceRank_Step6( szDate);
                  return 0;
                }
              }
              else
              {
                this->FmtLog(
                  "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                  szDate,
                  Buffer);
                this->Update_RaceRank_Step6( szDate);
                return 0;
              }
            }
            else
            {
              this->FmtLog(
                "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
                szDate,
                Buffer);
              this->Update_RaceRank_Step6( szDate);
              return 0;
            }
          }
          else
          {
            this->FmtLog(
              "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!",
              szDate,
              Buffer);
            this->Update_RaceRank_Step6( szDate);
            return 0;
          }
        }
        else
        {
          this->FmtLog( "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!", szDate, Buffer);
          this->Update_RaceRank_Step6( szDate);
          return 0;
        }
      }
      else
      {
        this->FmtLog( "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!", szDate, Buffer);
        this->Update_RaceRank_Step6( szDate);
        return 0;
      }
    }
    else
    {
      this->FmtLog( "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!", szDate, Buffer);
      this->Update_RaceRank_Step6( szDate);
      return 0;
    }
  }
  else
  {
    this->FmtLog( "CRFWorldDatabase::Update_RaceRank_Step5(szDate(%s)) : %s Fail!", szDate, Buffer);
    this->Update_RaceRank_Step6( szDate);
    return 0;
  }
}

char CRFWorldDatabase::Update_RaceRank_Step_6_1( char *szDate)
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
    return 1;
  }

  FmtLog("CRFWorldDatabase::Update_RaceRank_Step_6_1(szDate(%s)) : %s Fail!", szDate, Buffer);
  return 0;
}

char CRFWorldDatabase::Update_RaceRank_Step_6_2( char *szDate)
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
    return 1;
  }

  FmtLog("CRFWorldDatabase::Update_RaceRank_Step_6_2(szDate(%s)) : %s Fail!", szDate, Buffer);
  return 0;
}

char CRFWorldDatabase::Update_RaceRank_Step_6_3( char *szDate)
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
    return 1;
  }

  FmtLog("CRFWorldDatabase::Update_RaceRank_Step_6_3(szDate(%s)) : %s Fail!", szDate, Buffer);
  return 0;
}

char CRFWorldDatabase::Update_RaceRank_Step6( char *szDate)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-28h] BYREF

  v2 = &v5;
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
        return 1;
      }
      else
      {
        this->Update_RaceRank_Step_6_3( szDate);
        return 0;
      }
    }
    else
    {
      this->Update_RaceRank_Step_6_2( szDate);
      return 0;
    }
  }
  else
  {
    this->Update_RaceRank_Step_6_1( szDate);
    return 0;
  }
}

char CRFWorldDatabase::Update_RaceRank_Step7( char *szDate)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-458h] BYREF
  char Buffer[1028]; // [rsp+30h] [rbp-428h] BYREF
  int j; // [rsp+434h] [rbp-24h]

  v2 = &v5;
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step7(szDate(%s)) : Start Update Rank tbl_PvpRank%s Table",
    szDate,
    szDate);
  memset(Buffer, 0, 1024);
  for ( j = 0; j < 3; ++j )
  {
    sprintf(
      Buffer,
      "select IDENTITY(int, 1, 1) AS Rank, Serial into #tbl_PvpRank from tbl_PvpRank%s where race=%d order by Grade desc, Rate",
      szDate,
      j);
    if ( !this->ExecUpdateQuery( Buffer, 0) )
    {
      this->FmtLog( "CRFWorldDatabase::Update_RaceRank_Step7(szDate(%s)) : %s Fail!", szDate, Buffer);
      return 0;
    }
    sprintf(
      Buffer,
      "update tbl_PvpRank%s set rank = t1.rank from(select serial, rank from #tbl_PvpRank) as t1 where tbl_PvpRank%s.serial = t1.serial",
      szDate,
      szDate);
    if ( !this->ExecUpdateQuery( Buffer, 0) )
    {
      this->FmtLog( "CRFWorldDatabase::Update_RaceRank_Step7(szDate(%s)) : %s Fail!", szDate, Buffer);
      return 0;
    }
    sprintf(Buffer, "drop table #tbl_PvpRank");
    if ( !this->ExecUpdateQuery( Buffer, 0) )
    {
      this->FmtLog( "CRFWorldDatabase::Update_RaceRank_Step7(szDate(%s)) : %s Fail!", szDate, Buffer);
      return 0;
    }
  }
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step7(szDate(%s)) : End Update Rank tbl_PvpRank%s Table",
    szDate,
    szDate);
  return 1;
}

char CRFWorldDatabase::Update_RaceRank_Step8( char *szDate)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-468h] BYREF
  char Buffer[1040]; // [rsp+40h] [rbp-428h] BYREF

  v2 = &v5;
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step8(szDate(%s)) : Start Set GuildName tbl_PvpRank%s Table",
    szDate,
    szDate);
  memset(Buffer, 0, 1024);
  sprintf_s(
    Buffer,
    0x400uLL,
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
    return 1;
  }
  else
  {
    this->FmtLog( "CRFWorldDatabase::Update_RaceRank_Step8(szDate(%s)) : %s Fail!", szDate, Buffer);
    return 0;
  }
}

char CRFWorldDatabase::Update_RaceRank_Step9( char *szDate)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-458h] BYREF
  char Buffer[1040]; // [rsp+30h] [rbp-428h] BYREF

  v2 = &v5;
  this->FmtLog(
    "CRFWorldDatabase::Update_RaceRank_Step9(szDate(%s)) : Start Drop tbl_PvpRankToday Table",
    szDate);
  memset(Buffer, 0, 1024);
  if ( !this->TableExist( "tbl_PvpRankToday")
    || (sprintf_s(Buffer, 0x400uLL, "Drop Table tbl_PvpRankToday"), this->ExecUpdateQuery( Buffer, 0)) )
  {
    this->FmtLog(
      "CRFWorldDatabase::Update_RaceRank_Step9(szDate(%s)) : End Drop tbl_PvpRankToday Table",
      szDate);
    this->FmtLog(
      "CRFWorldDatabase::Update_RaceRank_Step9(szDate(%s)) : Start Create tbl_PvpRankToday Table",
      szDate);
    sprintf_s(
      Buffer,
      0x400uLL,
      "select Rank, Rate, serial, name, lv, race, PvpPoint, Grade, GuildSerial, GuildName into [dbo].[tbl_PvpRankToday] f"
      "rom tbl_PvpRank%s",
      szDate);
    if ( this->ExecUpdateQuery( Buffer, 0) )
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_RaceRank_Step9(szDate(%s)) : End Create tbl_PvpRankToday Table",
        szDate);
      this->FmtLog( "CRFWorldDatabase Date(%s) : Update_RaceRank End Success!", szDate);
      return 1;
    }
    else
    {
      this->FmtLog( "CRFWorldDatabase::Update_RaceRank_Step9(szDate(%s)) : %s Fail!", szDate, Buffer);
      return 0;
    }
  }
  else
  {
    this->FmtLog( "CRFWorldDatabase::Update_RaceRank_Step9(szDate(%s)) : %s Fail!", szDate, Buffer);
    return 0;
  }
}

char CRFWorldDatabase::Update_RankInGuild_Step2( unsigned int dwGuildSerial)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-28h] BYREF

  v2 = &v5;
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
                                                        return 1;
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
                                                        return 0;
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
                                                      return 0;
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
                                                    return 0;
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
                                                  return 0;
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
                                                return 0;
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
                                              return 0;
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
                                            return 0;
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
                                          return 0;
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
                                        return 0;
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
                                      return 0;
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
                                    return 0;
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
                                  return 0;
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
                                return 0;
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
                              return 0;
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
                            return 0;
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
                          return 0;
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
                        return 0;
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
                      return 0;
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
                    return 0;
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
                  return 0;
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
                return 0;
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
              return 0;
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
            return 0;
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
          return 0;
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
        return 0;
      }
    }
    else
    {
      this->FmtLog(
        "CRFWorldDatabase::Update_RankInGuild_Step2( dwGuildSerial(%u) ) : update #tbl_RankInGuild set NewGrade=0 Fail!",
        dwGuildSerial);
      if ( !this->Update_RankInGuild_Step7() )
        this->Update_RankInGuild_Step7();
      return 0;
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
    return 0;
  }
}

char CRFWorldDatabase::Update_RankInGuild_Step3( unsigned int dwGuildSerial)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-28h] BYREF

  v2 = &v5;
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
        return 1;
      }
      else
      {
        this->FmtLog(
          "CRFWorldDatabase::Update_RankInGuild_Step3( dwGuildSerial(%u) ) : update #tbl_RankInGuildCom set NewRate = ( ("
          "NewRank*10000)/(select count(*) from #tbl_RankInGuildCom) ) ",
          dwGuildSerial);
        this->Update_RankInGuild_Step6();
        return 0;
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
      return 0;
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
    return 0;
  }
}

char CRFWorldDatabase::Update_RankInGuild_Step4( unsigned int dwGuildSerial)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-28h] BYREF

  v2 = &v5;
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
    return 1;
  }
  else
  {
    this->FmtLog(
      "CRFWorldDatabase::Update_RankInGuild_Step4( dwGuildSerial(%u) ) : update tbl_general set GuildRank = rank.NewRank\t"
      "from ( select serial, NewRank from #tbl_RankInGuildAll ) as rank where tbl_general.serial = rank.serial Fail!",
      dwGuildSerial);
    this->Update_RankInGuild_Step6();
    return 0;
  }
}

char CRFWorldDatabase::Update_RankInGuild_Step5(
        unsigned int dwGuildSerial,
        _worlddb_rankinguild_info *pGuildMemberRankData)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-768h] BYREF
  SQLLEN StrLen_or_IndPtr; // [rsp+38h] [rbp-730h] BYREF
  SQLRETURN v8; // [rsp+44h] [rbp-724h]
  char Buffer[1028]; // [rsp+60h] [rbp-708h] BYREF
  int j; // [rsp+464h] [rbp-304h]
  int v11[11]; // [rsp+474h] [rbp-2F4h] BYREF
  unsigned int TargetValue[157]; // [rsp+4A0h] [rbp-2C8h] BYREF
  unsigned int v13[9]; // [rsp+714h] [rbp-54h] BYREF
  double v14; // [rsp+738h] [rbp-30h] BYREF
  int k; // [rsp+744h] [rbp-24h]

  v3 = &v6;
  this->FmtLog(
    "CRFWorldDatabase::Update_RankInGuild_Step5( dwGuildSerial(%u), pGuildMemberRankData ) : Start Set Return Result!",
    dwGuildSerial);
  memset(Buffer, 0, 1024);
  j = 0;
  v11[0] = 2;
  memset_0(TargetValue, 0, 0x258uLL);
  this->FmtLog(
    "CRFWorldDatabase::Update_RankInGuild_Step5( dwGuildSerial(%u), pGuildMemberRankData ) : Start Get Grade from #tbl_RankInGuildCom",
    dwGuildSerial);
  sprintf_s(Buffer, 0x400uLL, "select top %u serial, Grade from #tbl_RankInGuildCom order by Grade", 50);
  if ( this->m_hStmtSelect || this->ReConnectDataBase() )
  {
    v8 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
    if ( !v8 || v8 == 1 )
    {
      for ( j = 0; ; ++j )
      {
        v8 = SQLFetch(this->m_hStmtSelect);
        if ( v8 )
        {
          if ( v8 != 1 )
            break;
        }
        v8 = SQLGetData(this->m_hStmtSelect, 1u, 4, &TargetValue[3 * j], 0LL, &StrLen_or_IndPtr);
        v8 = SQLGetData(this->m_hStmtSelect, 2u, 4, v11, 0LL, &StrLen_or_IndPtr);
        TargetValue[3 * j + 1] = static_cast<unsigned int>(v11[0] & 0xFF);
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
        Buffer,
        sizeof(Buffer),
        "select serial, lv, Pvppoint, CurGrade from #tbl_RankInGuildAll order by NewRank");
      if ( this->m_hStmtSelect || this->ReConnectDataBase() )
      {
        v8 = SQLExecDirectA(this->m_hStmtSelect, (SQLCHAR *)Buffer, -3);
        if ( !v8 || v8 == 1 )
        {
          j = 0;
          v13[0] = 0;
          v14 = 0.0;
          while ( 1 )
          {
            v8 = SQLFetch(this->m_hStmtSelect);
            if ( v8 )
            {
              if ( v8 != 1 )
                break;
            }
            v8 = SQLGetData(this->m_hStmtSelect, 1u, 4, &pGuildMemberRankData->MemberData[j], 0LL, &StrLen_or_IndPtr);
            v8 = SQLGetData(this->m_hStmtSelect, 2u, 65518, v13, 0LL, &StrLen_or_IndPtr);
            v8 = SQLGetData(this->m_hStmtSelect, 3u, 8, &v14, 0LL, &StrLen_or_IndPtr);
            v8 = SQLGetData(
                   this->m_hStmtSelect,
                   4u,
                   4,
                   &pGuildMemberRankData->MemberData[j].byGrade,
                   0LL,
                   &StrLen_or_IndPtr);
            pGuildMemberRankData->MemberData[j].byLv = v13[0];
            pGuildMemberRankData->MemberData[j].dwPvpPoint = (int)v14;
            if ( pGuildMemberRankData->MemberData[j].byGrade == 2 )
              pGuildMemberRankData->MemberData[j].byGrade = 2;
            else
              pGuildMemberRankData->MemberData[j].byGrade = 0;
            for ( k = 0; k < 50; ++k )
            {
              if ( pGuildMemberRankData->MemberData[j].dwSerial == TargetValue[3 * k] )
              {
                pGuildMemberRankData->MemberData[j].byGrade = TargetValue[3 * k + 1];
                break;
              }
            }
            ++j;
          }
          pGuildMemberRankData->wRecordCount = j;
          if ( this->m_hStmtSelect )
            SQLCloseCursor(this->m_hStmtSelect);
          this->FmtLog(
            "CRFWorldDatabase::Update_RankInGuild_Step5( dwGuildSerial(%u), pGuildMemberRankData ) : End Set Return Result!",
            dwGuildSerial);
          return 1;
        }
        else
        {
          this->Update_RankInGuild_Step6();
          if ( v8 == 100 )
          {
            this->FmtLog(
              "CRFWorldDatabase::Update_RankInGuild_Step5( dwGuildSerial(%u), pGuildMemberRankData ) : %s : NO_DATA!",
              dwGuildSerial,
              Buffer);
          }
          else
          {
            this->FmtLog(
              "CRFWorldDatabase::Update_RankInGuild_Step5( dwGuildSerial(%u), pGuildMemberRankData ) : %s : Sql Error!",
              dwGuildSerial,
              Buffer);
            this->ErrorMsgLog( v8, Buffer, "SQLExecDirect", this->m_hStmtSelect);
            this->ErrorAction( v8, this->m_hStmtSelect);
          }
          return 0;
        }
      }
      else
      {
        this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", Buffer);
        return 0;
      }
    }
    else
    {
      this->Update_RankInGuild_Step6();
      if ( v8 == 100 )
      {
        this->FmtLog(
          "CRFWorldDatabase::Update_RankInGuild_Step5( dwGuildSerial(%u), pGuildMemberRankData ) : %s : NO_DATA!",
          dwGuildSerial,
          Buffer);
      }
      else
      {
        this->FmtLog(
          "CRFWorldDatabase::Update_RankInGuild_Step5( dwGuildSerial(%u), pGuildMemberRankData ) : %s : Sql Error!",
          dwGuildSerial,
          Buffer);
        this->ErrorMsgLog( v8, Buffer, "SQLExecDirect", this->m_hStmtSelect);
        this->ErrorAction( v8, this->m_hStmtSelect);
      }
      return 0;
    }
  }
  else
  {
    this->ErrFmtLog( "ReConnectDataBase Fail. Query : %s", Buffer);
    return 0;
  }
}

char CRFWorldDatabase::Update_RankInGuild_Step6()
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  v1 = &v4;
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
        return 1;
      }
      else
      {
        this->Update_RankInGuild_Step9();
        return 0;
      }
    }
    else
    {
      this->Update_RankInGuild_Step8();
      return 0;
    }
  }
  else
  {
    this->Update_RankInGuild_Step7();
    return 0;
  }
}
