USE [master]
GO
CREATE DATABASE [RF_User]
GO
ALTER DATABASE [RF_User] SET COMPATIBILITY_LEVEL = 100
GO
USE [RF_User]
GO

/*
Legacy Thai RF_User auth objects that depended on the old id/pw layout are intentionally omitted.
RF_User now stores secure auth data through id_hmac / id_enc / password_hash and the application
generates those values.
*/
GO

CREATE TABLE [dbo].[tbl_block_ip](
	[nSerial] [int] IDENTITY(1,1) NOT NULL,
	[addr0] [tinyint] NOT NULL,
	[addr1] [tinyint] NOT NULL,
	[addr2] [tinyint] NOT NULL,
	[addr3] [tinyint] NOT NULL DEFAULT ((0)),
	[addr4] [tinyint] NOT NULL DEFAULT ((255)),
	[addr2range] [tinyint] NOT NULL DEFAULT ((255)),
	CONSTRAINT [PK_tbl_block_ip] PRIMARY KEY ([nSerial] ASC)
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_bugtime_connect_in_pcbang](
	[nworldcode] [int] NOT NULL,
	[navatorserial] [int] NULL,
	[account] [char](17) NULL
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_bugtime_connectpcbang](
	[f1] [tinyint] NOT NULL,
	[f2] [tinyint] NOT NULL,
	[f3] [tinyint] NOT NULL,
	[f4] [tinyint] NOT NULL
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_connect_in_bugtime](
	[ip] [nvarchar](16) NOT NULL
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_connect_pcbang](
	[ip] [nvarchar](16) NOT NULL
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_connectpcbang_charserial](
	[nWorldCode] [tinyint] NOT NULL,
	[nAvatorSerial] [int] NOT NULL
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_CristalBattle_Log](
	[ID] [int] IDENTITY(220,1) NOT NULL,
	[DID] [int] NOT NULL,
	[SID] [int] NOT NULL,
	[Nth] [tinyint] NOT NULL,
	[MasterRace] [tinyint] NOT NULL,
	[MasterName] [varchar](17) NOT NULL,
	[MasterClass] [char](4) NOT NULL,
	[B_BossName] [varchar](17) NOT NULL,
	[C_BossName] [varchar](17) NOT NULL,
	[A_BossName] [varchar](17) NOT NULL,
	[MasterSubBossName1] [varchar](17) NOT NULL,
	[MasterSubBossName2] [varchar](17) NOT NULL,
	[MasterSubBossName3] [varchar](17) NOT NULL,
	[MasterSubBossName4] [varchar](17) NOT NULL,
	[MasterSubBossName5] [varchar](17) NOT NULL,
	[StartTime] [datetime] NOT NULL,
	[GetTime] [datetime] NOT NULL DEFAULT (getdate()),
	[ChaosHour] [tinyint] NOT NULL,
	[ChaosMin] [tinyint] NOT NULL,
	[ElapseHour] [tinyint] NOT NULL DEFAULT ((0)),
	[ElapseMin] [tinyint] NOT NULL DEFAULT ((0)),
	[ElapseSec] [tinyint] NOT NULL DEFAULT ((0)),
	[KeeperHitter] [varchar](17) NOT NULL DEFAULT ('*'),
	[ByAnimus] [tinyint] NOT NULL DEFAULT ((255)),
	[DestroyRace] [tinyint] NULL DEFAULT ((255)),
	CONSTRAINT [PK_tbl_CristalBattle_Log] PRIMARY KEY ([ID] ASC)
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_exp_connectip](
	[F1] [tinyint] NOT NULL,
	[F2] [tinyint] NOT NULL,
	[F3] [tinyint] NOT NULL,
	[F4] [tinyint] NOT NULL
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_GMActLog](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[Account] [varchar](17) NOT NULL,
	[TID] [int] NOT NULL,
	[PID] [int] NOT NULL DEFAULT ((0)),
	[ActDate] [datetime] NOT NULL DEFAULT (getdate()),
	CONSTRAINT [PK_tbl_GMActLog] PRIMARY KEY ([ID] ASC)
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_GMActParam](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[Param1] [sql_variant] NULL,
	[Param2] [sql_variant] NULL,
	[Param3] [sql_variant] NULL,
	[Param4] [sql_variant] NULL,
	[Param5] [sql_variant] NULL,
	[Param6] [sql_variant] NULL,
	[Param7] [sql_variant] NULL,
	[Param8] [sql_variant] NULL,
	[Param9] [sql_variant] NULL,
	CONSTRAINT [PK_tbl_GMActParam] PRIMARY KEY ([ID] ASC)
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_GMActType](
	[ID] [int] NOT NULL,
	[bLog] [bit] NOT NULL,
	[TypeName] [varchar](50) NOT NULL,
	[TypeNameKor] [varchar](50) NOT NULL,
	CONSTRAINT [PK_tbl_GMActType] PRIMARY KEY ([ID] ASC)
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_GMjobLog](
	[nSerial] [int] IDENTITY(1,1) NOT NULL,
	[szGMID] [varchar](18) NOT NULL,
	[nKind] [tinyint] NOT NULL DEFAULT ((0)),
	[szAvatorName] [varchar](18) NULL,
	[nAvatorSerial] [int] NULL,
	[nAccountSerial] [int] NULL,
	[szDetail] [varchar](50) NOT NULL,
	[dtDate] [datetime] NOT NULL DEFAULT (getdate()),
	CONSTRAINT [PK_tbl_GMjobLog] PRIMARY KEY ([nSerial] ASC)
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_HolyQuest_Log](
	[serial] [int] IDENTITY(1,1) NOT NULL,
	[ServerName] [varchar](32) NOT NULL,
	[race] [tinyint] NOT NULL,
	[CharName] [varchar](17) NOT NULL,
	[LogDate] [datetime] NOT NULL DEFAULT (getdate()),
	[dstRace] [tinyint] NOT NULL,
	CONSTRAINT [PK_tbl_HolyQuest_Log] PRIMARY KEY ([serial] ASC)
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_HostAccount](
	[Account] [varchar](17) NOT NULL,
	[Password] [varchar](17) NOT NULL DEFAULT ('*'),
	[Serial] [int] IDENTITY(1,1) NOT NULL,
	[Grade] [tinyint] NOT NULL DEFAULT ((0)),
	[Depart] [varchar](24) NULL,
	[RealName] [varchar](12) NULL,
	[Grade1] [tinyint] NOT NULL DEFAULT ((0)),
	CONSTRAINT [PK_tbl_HostAccount] PRIMARY KEY ([Account] ASC)
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_iplist_pcbang](
	[IP1] [tinyint] NOT NULL,
	[IP2] [tinyint] NOT NULL,
	[IP3] [tinyint] NOT NULL,
	[IP4] [tinyint] NOT NULL,
	[lastip] [tinyint] NOT NULL,
	CONSTRAINT [PK_iplist_pcbangt] PRIMARY KEY ([IP1] ASC, [IP2] ASC, [IP3] ASC, [IP4] ASC)
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_protocolaccesslog](
	[Idx] [int] IDENTITY(1,1) NOT NULL,
	[ProtocolMsgID] [int] NOT NULL DEFAULT ((0)),
	[ProtocolID] [int] NOT NULL DEFAULT ((0)),
	[GMSerial] [int] NOT NULL DEFAULT ((0)),
	[AccessDate] [datetime] NOT NULL DEFAULT (getdate()),
	[ProtocolFld0] [binary](128) NULL,
	[ProtocolFld1] [binary](128) NULL,
	[ProtocolFld2] [binary](128) NULL,
	[ProtocolFld3] [binary](128) NULL,
	[ProtocolFld4] [binary](128) NULL,
	[ProtocolFld5] [binary](128) NULL,
	[ProtocolFld6] [binary](128) NULL,
	[ProtocolFld7] [binary](128) NULL,
	[ProtocolFld8] [binary](128) NULL,
	[ProtocolFld9] [binary](128) NULL,
	CONSTRAINT [PF_tbl_protocolaccesslog_Idx] PRIMARY KEY ([Idx] ASC)
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_protocolauthinfo](
	[Idx] [int] IDENTITY(1,1) NOT NULL,
	[dck] [bit] NOT NULL DEFAULT ((0)),
	[P_MsgID] [int] NOT NULL DEFAULT ((0)),
	[P_ID] [int] NOT NULL DEFAULT ((0)),
	[P_AuthType] [tinyint] NOT NULL DEFAULT ((0)),
	[GMID] [varchar](17) NOT NULL DEFAULT ('*'),
	[GMSerial] [int] NOT NULL DEFAULT ((0)),
	CONSTRAINT [PF_tbl_protocolauthinfo_Idx] PRIMARY KEY ([Idx] ASC)
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_protocolinfo](
	[Idx] [int] IDENTITY(1,1) NOT NULL,
	[dck] [bit] NOT NULL DEFAULT ((0)),
	[ProtocolMsgID] [int] NOT NULL DEFAULT ((0)),
	[ProtocolID] [int] NOT NULL DEFAULT ((0)),
	[ProtocolName] [varchar](64) NOT NULL DEFAULT ('*'),
	[ProtocolExplain] [varchar](256) NOT NULL DEFAULT ('*'),
	[ProtocolFldCnt] [tinyint] NOT NULL DEFAULT ((0)),
	[UpdateDate] [datetime] NULL DEFAULT (getdate()),
	CONSTRAINT [PF_tbl_protocolinfo_Idx] PRIMARY KEY ([Idx] ASC)
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_rfaccount](
	[id_hmac] [binary](32) NOT NULL,
	[id_enc] [varbinary](128) NOT NULL,
	[password_hash] [varchar](255) NOT NULL,
	[accounttype] [tinyint] NOT NULL DEFAULT ((0)),
	[birthdate] [smalldatetime] NULL,
	CONSTRAINT [PK_tbl_rfaccount] PRIMARY KEY ([id_hmac] ASC)
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_ServerUser_Log](
	[serial] [int] IDENTITY(1,1) NOT NULL,
	[dtDate] [datetime] NOT NULL,
	[nAverageUser] [int] NOT NULL DEFAULT ((0)),
	[nMaxUser] [int] NOT NULL DEFAULT ((0)),
	[ServerName] [varchar](32) NOT NULL,
	[nBellaUser] [int] NOT NULL DEFAULT ((0)),
	[nCoraUser] [int] NOT NULL DEFAULT ((0)),
	[nAccUser] [int] NOT NULL DEFAULT ((0)),
	CONSTRAINT [PK_tbl_ServerUser_Log] PRIMARY KEY ([serial] ASC)
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_StaffAccount](
	[Serial] [int] IDENTITY(2000000000,1) NOT NULL,
	[ID_hmac] [binary](32) NOT NULL,
	[ID_enc] [varbinary](128) NOT NULL,
	[PW_hash] [varchar](255) NOT NULL,
	[Grade] [tinyint] NOT NULL DEFAULT ((1)),
	[Depart] [varchar](32) NOT NULL,
	[RealName] [varchar](12) NOT NULL,
	[LastConnIP] [char](16) NOT NULL DEFAULT ('0'),
	[CreateDT] [datetime] NOT NULL DEFAULT (getdate()),
	[LastLoginDT] [datetime] NOT NULL DEFAULT ((0)),
	[LastLogoffDT] [datetime] NOT NULL DEFAULT ((0)),
	[TotalLogMin] [int] NOT NULL DEFAULT ((0)),
	[SubGrade] [tinyint] NOT NULL DEFAULT ((0)),
	[ExpireDT] [datetime] NOT NULL DEFAULT (dateadd(month,(1),getdate())),
	[ComClass] [varchar](12) NOT NULL DEFAULT ('??'),
	[BirthDay] [char](10) NOT NULL DEFAULT (((0)-(0))-(0)),
	CONSTRAINT [PK_tbl_StaffAccount] PRIMARY KEY ([Serial] ASC),
	CONSTRAINT [IX_tbl_StaffAccount] UNIQUE ([ID_hmac] ASC)
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_UserAccount](
	[serial] [int] IDENTITY(1,1) NOT NULL,
	[id_hmac] [binary](32) NOT NULL,
	[id_enc] [varbinary](128) NOT NULL,
	[createtime] [datetime] NOT NULL DEFAULT (getdate()),
	[createip] [char](16) NOT NULL DEFAULT ((0)),
	[lastlogintime] [datetime] NOT NULL DEFAULT (getdate()),
	[lastlogofftime] [datetime] NOT NULL DEFAULT (getdate()),
	[totallogmin] [int] NOT NULL DEFAULT ((0)),
	[lastconnectip] [char](16) NOT NULL DEFAULT ((0)),
	[pushclosetime] [datetime] NOT NULL DEFAULT (getdate()),
	[pusherip] [char](16) NOT NULL DEFAULT ((0)),
	[JoinCode] [int] NOT NULL DEFAULT ((0)),
	[LoginFailureCnt] [tinyint] NOT NULL DEFAULT ((0)),
	[fire_on] [smalldatetime] NOT NULL DEFAULT (getdate()),
	[fire_warning] [smalldatetime] NOT NULL DEFAULT (getdate()),
	[uilock] [tinyint] NOT NULL DEFAULT ((0)),
	[uilock_pw] [varchar](255) NOT NULL DEFAULT (''),
	[uilock_failcnt] [tinyint] NOT NULL DEFAULT ((0)),
	[uilock_update] [smalldatetime] NOT NULL DEFAULT ((0)),
	[uilock_hintindex] [tinyint] NOT NULL DEFAULT ((0)),
	[uilock_hintanswer] [varchar](255) NOT NULL DEFAULT (''),
	[uilock_find_pass_failcnt] [tinyint] NOT NULL DEFAULT ((0)),
	CONSTRAINT [PK_tbl_rfuser] PRIMARY KEY ([id_hmac] ASC),
	CONSTRAINT [IX_tbl_rfuser] UNIQUE ([serial] ASC)
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_UserBan](
	[nAccountSerial] [int] NOT NULL,
	[dtStartDate] [datetime] NOT NULL DEFAULT (getdate()),
	[nPeriod] [int] NOT NULL,
	[nKind] [tinyint] NOT NULL DEFAULT ((0)),
	[szReason] [varchar](32) NULL,
	[GMReason] [varchar](32) NOT NULL DEFAULT ('*'),
	[GMWriter] [varchar](32) NOT NULL DEFAULT ('*'),
	[reasontype] [tinyint] NOT NULL DEFAULT ((0)),
	CONSTRAINT [PK_tbl_UserBan] PRIMARY KEY ([nAccountSerial] ASC)
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_UserBan_Log](
	[nSerial] [int] IDENTITY(1,1) NOT NULL,
	[nAccountSerial] [int] NOT NULL,
	[dtStartdate] [datetime] NOT NULL DEFAULT (getdate()),
	[nPeriod] [int] NOT NULL,
	[nKind] [tinyint] NOT NULL DEFAULT ((0)),
	[szReason] [varchar](32) NULL,
	[GMReason] [varchar](32) NOT NULL DEFAULT ('*'),
	[GMWriter] [varchar](32) NOT NULL DEFAULT ('*'),
	CONSTRAINT [PK_tbl_UserBan_Log] PRIMARY KEY ([nSerial] ASC)
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_UserCount_Log](
	[serial] [int] IDENTITY(1,1) NOT NULL,
	[dtDate] [datetime] NOT NULL,
	[nAverageUser] [int] NOT NULL DEFAULT ((0)),
	[nMaxUser] [int] NOT NULL DEFAULT ((0)),
	[nMaxUserTestServer] [int] NOT NULL DEFAULT ((0)),
	CONSTRAINT [PK_tbl_UserCount_Log] PRIMARY KEY ([serial] ASC)
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_usercurrentstate](
	[serial] [int] NOT NULL,
	[state] [tinyint] NOT NULL DEFAULT ((0)),
	CONSTRAINT [PK_tbl_usercurrentstate] PRIMARY KEY ([serial] ASC)
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_UserPush_Log](
	[nAccountSerial] [int] NOT NULL DEFAULT ((0)),
	[dtDate] [datetime] NOT NULL DEFAULT (getdate()),
	[PushIP] [varchar](16) NOT NULL DEFAULT ((0)),
	[CloseIP] [varchar](16) NOT NULL DEFAULT ((0)),
	CONSTRAINT [PK_tbl_UserPush_Log] PRIMARY KEY ([nAccountSerial] ASC, [dtDate] ASC)
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_WorldHistory_Log](
	[serverName] [char](12) NOT NULL,
	[race] [char](12) NOT NULL,
	[avatorNum] [int] NOT NULL DEFAULT ((0)),
	[dalant] [bigint] NOT NULL DEFAULT ((0)),
	[gold] [bigint] NOT NULL DEFAULT ((0)),
	[lv] [int] NOT NULL,
	[logDate] [datetime] NOT NULL DEFAULT (getdate()),
	[idx] [int] IDENTITY(1,1) NOT NULL,
	CONSTRAINT [PK_tbl_WorldHistory_Log] PRIMARY KEY ([idx] ASC)
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_WorldServer_List](
	[Serial] [int] NOT NULL,
	[Name] [varchar](32) NOT NULL,
	[State] [int] NOT NULL,
	[LastUpdate] [datetime] NOT NULL DEFAULT (getdate()),
	[msrepl_tran_version] [uniqueidentifier] NOT NULL DEFAULT (newid()),
	CONSTRAINT [PK_tbl_WorldServer_List] PRIMARY KEY ([Serial] ASC)
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_UserLogout_Log](
	[nAccountSerial] [int] NOT NULL,
	[nCounter] [int] NOT NULL,
	[dtLoginDate] [datetime] NOT NULL,
	[dtLogoutDate] [datetime] NOT NULL,
	[ip] [char](16) NOT NULL,
	CONSTRAINT [PK_tbl_UserLogout_Log] PRIMARY KEY ([nAccountSerial] ASC, [nCounter] ASC)
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_fg_detected_account](
	[account] [varchar](16) NOT NULL,
	[detect_time] [datetime] NOT NULL DEFAULT (getdate()),
	CONSTRAINT [PK_tbl_fg_detected_account] PRIMARY KEY ([account] ASC, [detect_time] ASC)
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[tbl_fg_detected_ip](
	[ip] [int] NOT NULL,
	[detect_time] [datetime] NOT NULL DEFAULT (getdate()),
	CONSTRAINT [PK_tbl_fg_detected_ip] PRIMARY KEY ([ip] ASC, [detect_time] ASC)
) ON [PRIMARY]
GO

CREATE PROCEDURE [dbo].[pCheck_Today_LogTable]
AS
DECLARE @strSql nvarchar(500)
DECLARE @today varchar(9)
SET NOCOUNT ON
SET @today = convert(varchar(9), getdate(), 112)
SET @strSql = N'SELECT top 1 name FROM dbo.sysobjects WHERE id = object_id(''' + 'tbl_UserLogout_Log' + @today + ''')'
EXEC sp_executesql @strSql
IF @@rowcount < 1
BEGIN
    SET @strSql = N'CREATE TABLE [dbo].[tbl_UserLogout_Log' + @today + '] (' +
        N'[idx] [int] IDENTITY (1, 1) NOT NULL, ' +
        N'[nAccountSerial] [int] NOT NULL, ' +
        N'[nWorldCode] [int] NOT NULL, ' +
        N'[dtLoginDate] [datetime] NOT NULL, ' +
        N'[dtLogoutDate] [datetime] NOT NULL, ' +
        N'[nBillingType] [int] NOT NULL, ' +
        N'[nLevel] [int] NULL, ' +
        N'[nAvatorSerial] [int] NULL, ' +
        N'[Account] [char] (17) NULL, ' +
        N'[ip] [char] (16) NOT NULL ) ON [PRIMARY]'
    EXEC sp_executesql @strSql

    SET @strSql = N'ALTER TABLE [dbo].[tbl_UserLogout_Log' + @today + '] WITH NOCHECK ADD ' +
        N'CONSTRAINT [PK_tbl_UserLogout_Log' + @today + '] PRIMARY KEY NONCLUSTERED ([idx]) ON [PRIMARY]'
    EXEC sp_executesql @strSql

    SET @strSql = N'CREATE CLUSTERED INDEX [IX_tbl_UserLogout_Log_Logout' + @today + '] ON ' +
        N'[dbo].[tbl_UserLogout_Log' + @today + ']([dtLogoutDate]) ON [PRIMARY]'
    EXEC sp_executesql @strSql

    SET @strSql = N'CREATE INDEX [IX_tbl_UserLogout_Log' + @today + '] ON [dbo].[tbl_UserLogout_Log' + @today + '] ' +
        N'([dtLoginDate]) ON [PRIMARY]'
    EXEC sp_executesql @strSql
END
SET NOCOUNT OFF
GO

CREATE PROCEDURE [dbo].[pDelete_UserBan]
@nSerial int
AS
DELETE  tbl_UserBan where nAccountSerial=@nSerial
GO

CREATE PROCEDURE [dbo].[pInsert_BlockIP]
@ipA tinyint,
@ipB tinyint,
@ipC tinyint,
@ipD tinyint
as

insert tbl_block_ip(addr0, addr1, addr2, addr3, addr4, addr2range) values(@ipA, @ipB, @ipC, @ipD, @ipD, @ipC)
GO

CREATE PROCEDURE [dbo].[pInsert_GMjob]
@szGMID varchar(18),
@nKind int,
@szAvatorName varchar(18),
@nAvatorSerial int,
@nAccountSerial int,
@szDetail varchar(50)
AS
INSERT INTO 
tbl_GMjobLog( szGMID, nKind, szAvatorName, nAvatorSerial, nAccountSerial, szDetail )
VALUES( @szGMID, @nKind, @szAvatorName, @nAvatorSerial, @nAccountSerial, @szDetail )
GO

CREATE PROCEDURE [dbo].[pInsert_HolyQuestLog]
@race int,
@dstRace int,
@name varchar(17),
@server varchar(32)
AS
INSERT INTO tbl_HolyQuest_Log( race, dstRace, CharName, ServerName, LogDate ) VALUES( @race, @dstRace, @name, @server, getdate() )
GO

CREATE PROCEDURE [dbo].[pInsert_ServerUserLog]
@avrUser int,
@maxUser int,
@bUser int,
@cUser int,
@aUser int,
@server varchar(32),
@logDate varchar(17)
AS
INSERT INTO tbl_ServerUser_Log(dtDate, nAverageUser, nMaxUser, nBellaUser, nCoraUser, nAccUser, ServerName ) 
VALUES(@logDate, @avrUser, @maxUser, @bUser, @cUser, @aUser, @server )
GO

CREATE PROCEDURE [dbo].[pInsert_ServerUserLog2]
@avrUser int,
@maxUser int,
@bUser int,
@cUser int,
@aUser int,
@server varchar(32)
AS
INSERT INTO tbl_ServerUser_Log(dtDate, nAverageUser, nMaxUser, nBellaUser, nCoraUser, nAccUser, ServerName ) 
VALUES(getdate(), @avrUser, @maxUser, @bUser, @cUser, @aUser, @server )
GO

CREATE PROCEDURE [dbo].[pInsert_UserBan]
@nSerial int,
@nPeriod int,
@nKind int,
@szReason varchar(32)
AS
INSERT INTO 
tbl_UserBan( nAccountSerial, nPeriod, nKind, szReason  ) 
VALUES( @nSerial, @nPeriod, @nKind, @szReason )
GO

CREATE PROCEDURE [dbo].[pInsert_UserBan_20070302]
@nSerial int,
@nPeriod int,
@nKind int,
@szReason varchar(32),
@szWriter varchar(32)
AS
INSERT INTO 
[dbo].[tbl_UserBan] ( nAccountSerial, nPeriod, nKind, szReason,  GMReason, GMWriter) 
VALUES( @nSerial, @nPeriod, @nKind, @szReason, @szReason, @szWriter )
GO

CREATE PROCEDURE [dbo].[pInsert_UserBan_20071016]
@nSerial int, 
@nPeriod int,
@nKind int,
@szReason varchar(32),
@szWriter varchar(32),
@reasontype tinyint
as
insert into
[dbo].[tbl_UserBan] ( nAccountSerial, nPeriod, nKind, szReason,  GMReason, GMWriter, reasontype)
values ( @nSerial, @nPeriod, @nKind, @szReason, @szReason, @szWriter, @reasontype )
GO

CREATE PROCEDURE [dbo].[pInsert_UserBanLog]
@nSerial int,
@nPeriod int,
@nKind int,
@szReason varchar(32)
AS
INSERT INTO 
tbl_UserBan_Log( nAccountSerial, nPeriod, nKind, szReason ) 
VALUES( @nSerial, @nPeriod, @nKind, @szReason )
GO

CREATE PROCEDURE [dbo].[pInsert_UserBanLog_20070302]
@nSerial int,
@nPeriod int,
@nKind int,
@szReason varchar(32),
@szWriter varchar(32)
AS
INSERT INTO 
[dbo].[tbl_UserBan_Log] ( nAccountSerial, nPeriod, nKind, szReason, GMReason, GMWriter ) 
VALUES( @nSerial, @nPeriod, @nKind, @szReason, @szReason, @szWriter )
GO

CREATE PROCEDURE [dbo].[pInsert_UserConnLog]
@serial int,
@loginDate varchar(16),
@ip varchar(16)
AS
declare @counter int
set @counter = (SELECT count(nCounter) from tbl_UserLogout_Log where nAccountSerial = @serial)
INSERT INTO tbl_UserLogout_Log(nAccountSerial, nCounter, dtLoginDate, dtLogoutDate, ip) VALUES(@serial, @counter, @loginDate, getdate(), @ip)
GO

CREATE PROCEDURE [dbo].[pInsert_UserCountLog]  
@avrUser int,  
@maxUser int  
AS  
INSERT INTO [dbo].[tbl_UserCount_Log] (dtDate, nAverageUser, nMaxUser) VALUES(getdate(), @avrUser, @maxUser )
GO

CREATE PROCEDURE [dbo].[pInsert_UserCountLog20080218]
@avruser int,  
@maxuser int , 
@maxusertestserver int
as
insert [dbo].[tbl_UserCount_Log] ([dtDate], [nAverageUser], [nMaxUser], [nMaxUserTestServer]) values ( getdate(), @avruser, @maxuser, @maxusertestserver )
GO

CREATE PROCEDURE [dbo].[pInsert_UserCurrentState]
@serial int,
@state int
as
insert [dbo].[tbl_usercurrentstate] ([serial], [state]) values ( @serial, @state )
GO

CREATE PROCEDURE [dbo].[pInsert_UserLogoutLog]
@serial int,
@loginDate varchar(16),
@ip varchar(16)
AS
declare @counter int
set @counter = (SELECT count(nCounter) from tbl_UserLogout_Log where nAccountSerial = @serial)
INSERT INTO tbl_UserLogout_Log(nAccountSerial, nCounter, dtLoginDate, dtLogoutDate, ip) VALUES(@serial, @counter, @loginDate, getdate(), @ip)
GO

CREATE PROCEDURE [dbo].[pInsert_UserLogoutLog_Daily]
@serial int,
@loginDate varchar(16),
@ip varchar(16),
@worldcode int,
@billingtype int,
@level int,
@charSerial int,
@Account varchar(17)
AS
declare @today varchar(9)
declare @strSql nvarchar(500)
SET NOCOUNT ON
	set @today = convert(varchar(9), getdate(), 112)
	set @strSql = N'INSERT INTO tbl_UserLogout_Log' + @today + '(nAccountSerial, dtLoginDate, dtLogoutDate, ip, nWorldCode, nBillingType, nLevel, nAvatorSerial, Account) '
	set @strSql = @strSql + N' VALUES( '+ convert( varchar(8), @serial) + ',' + '''' + @loginDate + '''' + ','
	set @strSql = @strSql + N'''' + convert(varchar(32), getdate(), 20) + '''' + ',' + '''' + @ip +''''+ ',' + convert( varchar(8), @worldcode) + ','
	set @strSql = @strSql + convert(varchar(4), @billingtype) + ',' + convert(varchar(4), @level) + ','
	set @strSql = @strSql + convert(varchar(8), @charSerial)  + ',' +  '''' + @Account + ''''  + ') '
--	select @strSql
	exec sp_executesql  @strSql
SET NOCOUNT OFF
GO

CREATE PROCEDURE [dbo].[pInsert_UserLogoutLog_Daily20070115]    
@serial int,    
@loginDate varchar(16),    
@ip varchar(16),    
@worldcode int,    
@billingtype int,    
@level int,    
@charSerial int,    
@Account varchar(17)    
as
declare @today varchar(9)
declare @strSql nvarchar(1024)
set nocount on
 set @today = convert(varchar(9), getdate(), 112)
 set @strSql = N'INSERT INTO tbl_UserLogout_Log' + @today + '(nAccountSerial, dtLoginDate, dtLogoutDate, ip, nWorldCode, nBillingType, nLevel, nAvatorSerial, Account) '
 set @strSql = @strSql + N' VALUES( '+ convert( varchar, @serial) + ',' + '''' + @loginDate + '''' + ','
 set @strSql = @strSql + N'''' + convert(varchar(32), getdate(), 20) + '''' + ',' + '''' + @ip +''''+ ',' + convert( varchar, @worldcode) + ','
 set @strSql = @strSql + convert(varchar, @billingtype) + ',' + convert(varchar, @level) + ','
 set @strSql = @strSql + convert(varchar, @charSerial)  + ',' +  '''' + @Account + ''''  + ') '
-- select @strSql
 exec sp_executesql  @strSql    
set nocount off
GO

CREATE PROCEDURE [dbo].[pInsert_UserPushLog]
@serial int,
@pushIP varchar(16),
@closeIP varchar(16)
AS
INSERT INTO tbl_UserPush_Log(nAccountSerial, dtDate, PushIP, CloseIP) VALUES(@serial, getdate(), @pushIP, @closeIP)
GO

CREATE PROCEDURE [dbo].[pProcess_UserBan]    
@nSerial int    
as  
declare @dtBan datetime    
declare @dtBanAdd datetime    
declare @nPeriod int    
declare @nKind int    
declare @nRet int  
declare @tReasonType tinyint  
    
select @dtBan = dtStartdate, @tReasonType = reasontype  from tbl_userban where nAccountserial=@nSerial  
  
if  @dtBan is NULL     
begin    
    set @nRet = 0    
    select @nRet, @tReasonType  
    return    
end    
    
set @nPeriod = (select nPeriod from tbl_userban where nAccountserial=@nSerial)    
    
if @nPeriod = 999    
begin    
    set @nRet = 2    
    select @nRet, @tReasonType  
    return    
end    
    
set @dtBanAdd = dateadd( hh, @nPeriod, @dtBan)    
    
if @dtBanAdd <= (getdate())     
begin    
    set @nRet = 1    
    select @nRet, @tReasonType  
end    
else    
begin    
    set @nKind = (select nKind from tbl_userban where nAccountserial=@nSerial)    
    if @nKind = 0    
    begin    
        set @nRet = 2    
        select @nRet, @tReasonType  
    end    
    else    
    begin    
        set @nRet = 3    
        select @nRet, @tReasonType  
    end    
end    
    
return
GO

CREATE PROCEDURE [dbo].[prc_process_fg_detection]
@account varchar(12),
@ip int
as

declare @num_account int
declare @num_ip int
declare @detect_time smalldatetime
declare @check_time smalldatetime

set @num_account = 0
set @num_ip = 0
set @detect_time = getdate()
set @check_time = dateadd(hour, -12, @detect_time)

set nocount on

insert tbl_fg_detected_account( account, detect_time ) values( @account, @detect_time )
insert tbl_fg_detected_ip( ip, detect_time ) values( @ip, @detect_time )

select @num_account = count(*) from tbl_fg_detected_account where account = @account and detect_time >= @check_time
select @num_ip = count(*) from tbl_fg_detected_ip where ip = @ip and detect_time >= @check_time

set nocount off

select @num_account, @num_ip
GO

CREATE PROCEDURE [dbo].[pSelect_BlockIP]
@addr_0 tinyint,
@addr_1 tinyint,
@addr_2 tinyint,
@addr_3 tinyint
as
select top 1 nSerial from [dbo].[tbl_block_ip]
where addr0 = @addr_0 and addr1 = @addr_1 and addr2 = @addr_2 and @addr_3 between addr3 and addr4
GO

CREATE PROCEDURE [dbo].[pSelect_BlockIP_20070122]
@addr_0 tinyint,
@addr_1 tinyint,
@addr_2 tinyint,
@addr_3 tinyint
AS
select	nSerial
from	tbl_block_ip
where	addr0 = @addr_0 and 
	addr1 = @addr_1 and
	@addr_2 between addr2 and addr2range and
	@addr_3 between addr3 and addr4
GO

CREATE PROCEDURE [dbo].[pSelect_HolyQuestHistory]
@startDate varchar(16),
@endDate varchar(16)
AS
select datepart(yyyy,LogDate),datepart(mm,LogDate), datepart(dd,LogDate), datepart(hh,LogDate), ServerName, race, CharName
from tbl_HolyQuest_log where LogDate > @startDate and LogDate < @endDate order by LogDate
GO

CREATE PROCEDURE [dbo].[pSelect_HostAccountInfo]
@id varchar(16)
 AS
SELECT password, Grade FROM tbl_HostAccount WHERE Account = @id
GO

CREATE PROCEDURE [dbo].[pSelect_HostAccountInfo20061113]
@id varchar(16)
 AS
SELECT convert(varchar, password), Grade, Serial FROM [dbo].[tbl_HostAccount] WHERE Account =  @id
GO

CREATE PROCEDURE [dbo].[pselect_pcbangip]
@ip1 tinyint,
@ip2 tinyint, 
@ip3 tinyint, 
@ip4 tinyint 
as
select * from [dbo].[tbl_iplist_pcbang] where ip1 = @ip1 and ip2 = @ip2 and ip3 = @ip3 and @ip4 between ip4 and lastip
GO

CREATE PROCEDURE [dbo].[pSelect_UserBan]
@nSerial int
AS
SELECT  dtstartdate, nPeriod, nKind from tbl_UserBan where nAccountSerial=@nSerial
GO

CREATE PROCEDURE [dbo].[pSelect_UserBanHistory]
@nSerial int
AS
SELECT top 32 nKind, dtStartDate, nPeriod, szReason FROM tbl_UserBan_Log WHERE nAccountSerial=@nSerial order by nSerial desc
GO

CREATE PROCEDURE [dbo].[pSelect_UserBanHistory_20070302]
@nSerial int
AS
SELECT top 32 nKind, dtStartDate, nPeriod, szReason, GMWriter FROM [dbo].[tbl_UserBan_Log]
WHERE nAccountSerial=@nSerial order by nSerial desc
GO

CREATE PROCEDURE [dbo].[pSelect_UserCountInfo]
@startDate varchar(16),
@endDate varchar(16)
AS
select datepart(yyyy,dtDate),datepart(mm,dtDate), datepart(dd,dtDate), datepart(hh,dtDate), nAverageUser, nMaxUser from tbl_userCount_log where dtDate > @startDate and dtDate < @endDate order by dtDate
GO

CREATE PROCEDURE [dbo].[pUpdate_StaffLogoffDate]
@nAccountSerial int
AS
UPDATE tbl_StaffAccount SET LastLogoffDT = getdate()  WHERE serial=@nAccountSerial
GO

CREATE PROCEDURE [dbo].[pUpdate_UILock_Init]  
@accountserial int,  
@uilock_pw varchar(13),  
@hintindex tinyint,  
@hintanswer varchar(17),  
@ret tinyint output  
as

set @ret = 0  
declare @uilock tinyint  
  
select top 1 @uilock = [uilock] from [dbo].[tbl_useraccount] where [serial] = @accountserial  
  
if(@uilock is null) -- û  ø   ʴ´.  
begin  
 set @ret = 2  
 return  
end  
  
if(@uilock <> 0) -- ̹ ʱȭ Ǿ  0 ƴϴ.  
begin  
 set @ret = 1  
 return  
end  
  
  
set nocount on  
  
update [dbo].[tbl_useraccount]
set  [uilock] = 1,   
  [uilock_pw] = @uilock_pw,   
  [uilock_failcnt] = 0,  
  [uilock_update] = getdate(),  
  [uilock_hintindex] = @hintindex,  
  [uilock_hintanswer] = @hintanswer  
where [serial] = @accountserial  
  
set nocount off
GO

CREATE PROCEDURE [dbo].[pUpdate_UILock_Refresh]    
@accountserial int,    
@byFailCnt tinyint,  
@byFindPassFailCnt tinyint,    
@ret tinyint output    
as  
  
set @ret = 0    
    
declare @uilock tinyint    
declare @uilock_failcnt tinyint  
declare @uilock_findpass_failcnt tinyint   
    
select @uilock = [uilock], @uilock_failcnt = [uilock_failcnt], @uilock_findpass_failcnt = [uilock_find_pass_failcnt]  
from [dbo].[tbl_useraccount] where [serial] = @accountserial    
    
if( (@uilock is null) or (@uilock_failcnt is null) )-- û  ø   ʴ´.    
begin    
 set @ret = 2    
 return    
end    
    
if(@uilock = 0) --  ʱȭ   ̴.    
begin    
 set @ret = 1    
 return    
end    
    
if(@uilock_failcnt >= 5) -- 5 ̻   Ƚ    .    
begin    
 set @ret = 3    
 return    
end    
  
if(@uilock_findpass_failcnt >= 5) -- 5 ̻   Ƚ    .    
begin    
 set @ret = 4  
 return    
end    
    
set nocount on    
    
update [dbo].[tbl_useraccount]  
set  [uilock_failcnt] = @byFailCnt, [uilock_find_pass_failcnt] =@byFindPassFailCnt  
where [serial] = @accountserial    
    
set nocount off
GO

CREATE PROCEDURE [dbo].[pUpdate_UILock_Update]  
@accountserial int,  
@uilock_pw varchar(13),  
@uilock_hintindex tinyint,  
@uilock_hintanswer varchar(17),  
@ret tinyint output  
AS  
  
set @ret = 0  
  
declare @uilock tinyint  
  
select top 1 @uilock = uilock from tbl_useraccount where serial = @accountserial  
  
if(@uilock is null) -- û  ø   ʴ´.  
begin  
 set @ret = 2  
 return  
end  
  
if(@uilock = 0) --  ʱȭ   ̴.  
begin  
 set @ret = 1  
 return  
end  
  
  
set nocount on  
  
update tbl_useraccount  
set  uilock_pw = @uilock_pw,  
  uilock_failcnt = 0,  
  uilock_update = getdate(),  
  uilock_hintindex = @uilock_hintindex,  
  uilock_hintanswer = @uilock_hintanswer  
where serial = @accountserial  
  
set nocount off
GO

CREATE PROCEDURE [dbo].[pUpdate_UserBan]
@nSerial int,
@nPeriod int,
@nKind int,
@szReason varchar(32)
AS
UPDATE tbl_UserBan 
set nPeriod=@nPeriod, nKind=@nKind, dtStartdate=(getdate()), szReason=@szReason
where nAccountSerial=@nSerial
GO

CREATE PROCEDURE [dbo].[pUpdate_UserBan_20070302]
@nSerial int,
@nPeriod int,
@nKind int,
@szReason varchar(32),
@szWriter varchar(32)
AS
UPDATE [dbo].[tbl_UserBan]
set nPeriod=@nPeriod, nKind=@nKind, dtStartdate=(getdate()), szReason=@szReason, GMReason = @szReason, GMWriter = @szWriter
where nAccountSerial=@nSerial
GO

CREATE PROCEDURE [dbo].[pUpdate_UserBan_20071016]
@nSerial int, 
@nPeriod int,
@nKind int,
@szReason varchar(32),
@szWriter varchar(32),
@reasontype tinyint
as
update [dbo].[tbl_UserBan]
set nPeriod=@nPeriod, nKind=@nKind, dtStartdate=(getdate()), szReason=@szReason, GMReason = @szReason, GMWriter = @szWriter, reasontype = @reasontype
where nAccountSerial=@nSerial
GO

CREATE PROCEDURE [dbo].[pUpdate_UserCurrentState]
@serial int,
@state tinyint
as
update [dbo].[tbl_usercurrentstate] set [state] = @state where [serial] = @serial
GO

CREATE PROCEDURE [dbo].[pUpdate_UserLogoffDate]
@nAccountSerial int
AS
UPDATE tbl_UserAccount SET lastlogofftime = getdate()  WHERE serial=@nAccountSerial
GO

CREATE PROCEDURE [dbo].[pUpdate_WorldServer]
@Serial int,
@State int
AS
update tbl_WorldServer_List set State=@State, LastUpdate=getDate() where Serial=@Serial
GO
