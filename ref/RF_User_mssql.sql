USE [master]
GO
/****** Object:  Database [RF_User]    Script Date: 12/01/2026 16:35:26 ******/
CREATE DATABASE [RF_User]
 CONTAINMENT = NONE
 WITH CATALOG_COLLATION = DATABASE_DEFAULT, LEDGER = OFF
GO
ALTER DATABASE [RF_User] SET COMPATIBILITY_LEVEL = 100
GO
IF (1 = FULLTEXTSERVICEPROPERTY('IsFullTextInstalled'))
begin
EXEC [RF_User].[dbo].[sp_fulltext_database] @action = 'disable'
end
GO
ALTER DATABASE [RF_User] SET ANSI_NULL_DEFAULT OFF 
GO
ALTER DATABASE [RF_User] SET ANSI_NULLS OFF 
GO
ALTER DATABASE [RF_User] SET ANSI_PADDING OFF 
GO
ALTER DATABASE [RF_User] SET ANSI_WARNINGS OFF 
GO
ALTER DATABASE [RF_User] SET ARITHABORT OFF 
GO
ALTER DATABASE [RF_User] SET AUTO_CLOSE OFF 
GO
ALTER DATABASE [RF_User] SET AUTO_SHRINK OFF 
GO
ALTER DATABASE [RF_User] SET AUTO_UPDATE_STATISTICS ON 
GO
ALTER DATABASE [RF_User] SET CURSOR_CLOSE_ON_COMMIT OFF 
GO
ALTER DATABASE [RF_User] SET CURSOR_DEFAULT  GLOBAL 
GO
ALTER DATABASE [RF_User] SET CONCAT_NULL_YIELDS_NULL OFF 
GO
ALTER DATABASE [RF_User] SET NUMERIC_ROUNDABORT OFF 
GO
ALTER DATABASE [RF_User] SET QUOTED_IDENTIFIER OFF 
GO
ALTER DATABASE [RF_User] SET RECURSIVE_TRIGGERS OFF 
GO
ALTER DATABASE [RF_User] SET  DISABLE_BROKER 
GO
ALTER DATABASE [RF_User] SET AUTO_UPDATE_STATISTICS_ASYNC OFF 
GO
ALTER DATABASE [RF_User] SET DATE_CORRELATION_OPTIMIZATION OFF 
GO
ALTER DATABASE [RF_User] SET TRUSTWORTHY OFF 
GO
ALTER DATABASE [RF_User] SET ALLOW_SNAPSHOT_ISOLATION OFF 
GO
ALTER DATABASE [RF_User] SET PARAMETERIZATION SIMPLE 
GO
ALTER DATABASE [RF_User] SET READ_COMMITTED_SNAPSHOT OFF 
GO
ALTER DATABASE [RF_User] SET HONOR_BROKER_PRIORITY OFF 
GO
ALTER DATABASE [RF_User] SET RECOVERY FULL 
GO
ALTER DATABASE [RF_User] SET  MULTI_USER 
GO
ALTER DATABASE [RF_User] SET PAGE_VERIFY TORN_PAGE_DETECTION  
GO
ALTER DATABASE [RF_User] SET DB_CHAINING OFF 
GO
ALTER DATABASE [RF_User] SET FILESTREAM( NON_TRANSACTED_ACCESS = OFF ) 
GO
ALTER DATABASE [RF_User] SET TARGET_RECOVERY_TIME = 0 SECONDS 
GO
ALTER DATABASE [RF_User] SET DELAYED_DURABILITY = DISABLED 
GO
ALTER DATABASE [RF_User] SET ACCELERATED_DATABASE_RECOVERY = OFF  
GO
EXEC sys.sp_db_vardecimal_storage_format N'RF_User', N'ON'
GO
ALTER DATABASE [RF_User] SET QUERY_STORE = OFF
GO
USE [RF_User]
GO
/****** Object:  User [rfbrazilaccount]    Script Date: 12/01/2026 16:35:27 ******/
CREATE USER [rfbrazilaccount] WITHOUT LOGIN WITH DEFAULT_SCHEMA=[rfbrazilaccount]
GO
/****** Object:  User [rf_merge]    Script Date: 12/01/2026 16:35:27 ******/
CREATE USER [rf_merge] WITHOUT LOGIN WITH DEFAULT_SCHEMA=[rf_merge]
GO
/****** Object:  User [rf_billing]    Script Date: 12/01/2026 16:35:27 ******/
CREATE USER [rf_billing] WITHOUT LOGIN WITH DEFAULT_SCHEMA=[rf_billing]
GO
/****** Object:  User [ph_user]    Script Date: 12/01/2026 16:35:27 ******/
CREATE USER [ph_user] WITHOUT LOGIN WITH DEFAULT_SCHEMA=[ph_user]
GO
/****** Object:  User [gferreira]    Script Date: 12/01/2026 16:35:27 ******/
CREATE USER [gferreira] WITHOUT LOGIN WITH DEFAULT_SCHEMA=[gferreira]
GO
/****** Object:  User [eventuser]    Script Date: 12/01/2026 16:35:27 ******/
CREATE USER [eventuser] WITHOUT LOGIN WITH DEFAULT_SCHEMA=[eventuser]
GO
/****** Object:  User [account_lug]    Script Date: 12/01/2026 16:35:27 ******/
CREATE USER [account_lug] WITHOUT LOGIN WITH DEFAULT_SCHEMA=[account_lug]
GO
ALTER ROLE [db_owner] ADD MEMBER [rfbrazilaccount]
GO
ALTER ROLE [db_datareader] ADD MEMBER [rf_merge]
GO
ALTER ROLE [db_owner] ADD MEMBER [rf_billing]
GO
ALTER ROLE [db_datareader] ADD MEMBER [ph_user]
GO
ALTER ROLE [db_ddladmin] ADD MEMBER [gferreira]
GO
ALTER ROLE [db_datareader] ADD MEMBER [gferreira]
GO
ALTER ROLE [db_datawriter] ADD MEMBER [gferreira]
GO
/****** Object:  Schema [account_lug]    Script Date: 12/01/2026 16:35:27 ******/
CREATE SCHEMA [account_lug]
GO
/****** Object:  Schema [eventuser]    Script Date: 12/01/2026 16:35:27 ******/
CREATE SCHEMA [eventuser]
GO
/****** Object:  Schema [gferreira]    Script Date: 12/01/2026 16:35:27 ******/
CREATE SCHEMA [gferreira]
GO
/****** Object:  Schema [ph_user]    Script Date: 12/01/2026 16:35:27 ******/
CREATE SCHEMA [ph_user]
GO
/****** Object:  Schema [rf_billing]    Script Date: 12/01/2026 16:35:27 ******/
CREATE SCHEMA [rf_billing]
GO
/****** Object:  Schema [rf_merge]    Script Date: 12/01/2026 16:35:27 ******/
CREATE SCHEMA [rf_merge]
GO
/****** Object:  Schema [rfbrazilaccount]    Script Date: 12/01/2026 16:35:27 ******/
CREATE SCHEMA [rfbrazilaccount]
GO
/****** Object:  Table [dbo].[nopach]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[nopach](
	[IP] [varchar](255) NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_block_ip]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_block_ip](
	[nSerial] [int] IDENTITY(1,1) NOT NULL,
	[addr0] [tinyint] NOT NULL,
	[addr1] [tinyint] NOT NULL,
	[addr2] [tinyint] NOT NULL,
	[addr3] [tinyint] NOT NULL,
	[addr4] [tinyint] NOT NULL,
	[addr2range] [tinyint] NOT NULL,
 CONSTRAINT [PK_tbl_block_ip] PRIMARY KEY CLUSTERED 
(
	[nSerial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_bugtime_connect_in_pcbang]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_bugtime_connect_in_pcbang](
	[nworldcode] [int] NOT NULL,
	[navatorserial] [int] NULL,
	[account] [char](17) NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_bugtime_connectpcbang]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_bugtime_connectpcbang](
	[f1] [tinyint] NOT NULL,
	[f2] [tinyint] NOT NULL,
	[f3] [tinyint] NOT NULL,
	[f4] [tinyint] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_chargeresultstr]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_chargeresultstr](
	[ID] [tinyint] NOT NULL,
	[strError] [varchar](32) NOT NULL,
 CONSTRAINT [PK_tbl_chargeresultstr] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 90, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_connect_in_bugtime]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_connect_in_bugtime](
	[ip] [nvarchar](16) NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_connect_pcbang]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_connect_pcbang](
	[ip] [nvarchar](16) NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_connectpcbang_charserial]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_connectpcbang_charserial](
	[nWorldCode] [tinyint] NOT NULL,
	[nAvatorSerial] [int] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_CristalBattle_Log]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
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
	[GetTime] [datetime] NOT NULL,
	[ChaosHour] [tinyint] NOT NULL,
	[ChaosMin] [tinyint] NOT NULL,
	[ElapseHour] [tinyint] NOT NULL,
	[ElapseMin] [tinyint] NOT NULL,
	[ElapseSec] [tinyint] NOT NULL,
	[KeeperHitter] [varchar](17) NOT NULL,
	[ByAnimus] [tinyint] NOT NULL,
	[DestroyRace] [tinyint] NULL,
 CONSTRAINT [PK_tbl_CristalBattle_Log] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_exp_connectip]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_exp_connectip](
	[F1] [tinyint] NOT NULL,
	[F2] [tinyint] NOT NULL,
	[F3] [tinyint] NOT NULL,
	[F4] [tinyint] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_GMActLog]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_GMActLog](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[Account] [varchar](17) NOT NULL,
	[TID] [int] NOT NULL,
	[PID] [int] NOT NULL,
	[ActDate] [datetime] NOT NULL,
 CONSTRAINT [PK_tbl_GMActLog] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_GMActParam]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
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
 CONSTRAINT [PK_tbl_GMActParam] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_GMActType]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_GMActType](
	[ID] [int] NOT NULL,
	[bLog] [bit] NOT NULL,
	[TypeName] [varchar](50) NOT NULL,
	[TypeNameKor] [varchar](50) NOT NULL,
 CONSTRAINT [PK_tbl_GMActType] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_GMjobLog]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_GMjobLog](
	[nSerial] [int] IDENTITY(1,1) NOT NULL,
	[szGMID] [varchar](18) NOT NULL,
	[nKind] [tinyint] NOT NULL,
	[szAvatorName] [varchar](18) NULL,
	[nAvatorSerial] [int] NULL,
	[nAccountSerial] [int] NULL,
	[szDetail] [varchar](50) NOT NULL,
	[dtDate] [datetime] NOT NULL,
 CONSTRAINT [PK_tbl_GMjobLog] PRIMARY KEY CLUSTERED 
(
	[nSerial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_HolyQuest_Log]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_HolyQuest_Log](
	[serial] [int] IDENTITY(1,1) NOT NULL,
	[ServerName] [varchar](32) NOT NULL,
	[race] [tinyint] NOT NULL,
	[CharName] [varchar](17) NOT NULL,
	[LogDate] [datetime] NOT NULL,
	[dstRace] [tinyint] NOT NULL,
 CONSTRAINT [PK_tbl_HolyQuest_Log] PRIMARY KEY NONCLUSTERED 
(
	[serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_HostAccount]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_HostAccount](
	[Account] [varchar](17) NOT NULL,
	[Password] [varchar](17) NOT NULL,
	[Serial] [int] IDENTITY(1,1) NOT NULL,
	[Grade] [tinyint] NOT NULL,
	[Depart] [varchar](24) NULL,
	[RealName] [varchar](12) NULL,
	[Grade1] [tinyint] NOT NULL,
 CONSTRAINT [PK_tbl_HostAccount] PRIMARY KEY CLUSTERED 
(
	[Account] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_iplist_pcbang]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_iplist_pcbang](
	[IP1] [tinyint] NOT NULL,
	[IP2] [tinyint] NOT NULL,
	[IP3] [tinyint] NOT NULL,
	[IP4] [tinyint] NOT NULL,
	[lastip] [tinyint] NOT NULL,
 CONSTRAINT [PK_iplist_pcbangt] PRIMARY KEY CLUSTERED 
(
	[IP1] ASC,
	[IP2] ASC,
	[IP3] ASC,
	[IP4] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_ItemBatch]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_ItemBatch](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[DCK] [bit] NULL,
	[RID] [tinyint] NULL,
	[account] [varchar](17) NOT NULL,
	[server] [varchar](32) NOT NULL,
	[name] [varchar](17) NOT NULL,
	[serial] [int] NOT NULL,
	[itemcode] [varchar](10) NOT NULL,
	[D] [int] NOT NULL,
	[U] [varchar](15) NOT NULL,
	[adddate] [datetime] NULL,
	[givedate] [datetime] NULL,
	[deldate] [datetime] NULL,
	[charger] [varchar](17) NULL,
	[deleter] [varchar](17) NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_LUAccount]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_LUAccount](
	[id] [binary](16) NOT NULL,
	[password] [binary](24) NOT NULL,
	[BCodeTU] [tinyint] NOT NULL,
	[accounttype] [tinyint] NULL,
	[Email] [varchar](50) NULL,
 CONSTRAINT [PK_RF_LUAccount] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 90, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_protocolaccesslog]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_protocolaccesslog](
	[Idx] [int] IDENTITY(1,1) NOT NULL,
	[ProtocolMsgID] [int] NOT NULL,
	[ProtocolID] [int] NOT NULL,
	[GMSerial] [int] NOT NULL,
	[AccessDate] [datetime] NOT NULL,
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
 CONSTRAINT [PF_tbl_protocolaccesslog_Idx] PRIMARY KEY CLUSTERED 
(
	[Idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_protocolauthinfo]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_protocolauthinfo](
	[Idx] [int] IDENTITY(1,1) NOT NULL,
	[dck] [bit] NOT NULL,
	[P_MsgID] [int] NOT NULL,
	[P_ID] [int] NOT NULL,
	[P_AuthType] [tinyint] NOT NULL,
	[GMID] [varchar](17) NOT NULL,
	[GMSerial] [int] NOT NULL,
 CONSTRAINT [PF_tbl_protocolauthinfo_Idx] PRIMARY KEY CLUSTERED 
(
	[Idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_protocolinfo]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_protocolinfo](
	[Idx] [int] IDENTITY(1,1) NOT NULL,
	[dck] [bit] NOT NULL,
	[ProtocolMsgID] [int] NOT NULL,
	[ProtocolID] [int] NOT NULL,
	[ProtocolName] [varchar](64) NOT NULL,
	[ProtocolExplain] [varchar](256) NOT NULL,
	[ProtocolFldCnt] [tinyint] NOT NULL,
	[UpdateDate] [datetime] NULL,
 CONSTRAINT [PF_tbl_protocolinfo_Idx] PRIMARY KEY CLUSTERED 
(
	[Idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_rfaccount]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_rfaccount](
	[id_hmac] [binary](32) NOT NULL,
	[id_enc] [varbinary](128) NOT NULL,
	[password_hash] [varchar](255) NOT NULL,
	[accounttype] [tinyint] NOT NULL,
	[birthdate] [smalldatetime] NOT NULL,
 CONSTRAINT [PK_tbl_rfaccount] PRIMARY KEY CLUSTERED (
	[id_hmac] ASC
)
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_ServerUser_Log]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_ServerUser_Log](
	[serial] [int] IDENTITY(1,1) NOT NULL,
	[dtDate] [datetime] NOT NULL,
	[nAverageUser] [int] NOT NULL,
	[nMaxUser] [int] NOT NULL,
	[ServerName] [varchar](32) NOT NULL,
	[nBellaUser] [int] NOT NULL,
	[nCoraUser] [int] NOT NULL,
	[nAccUser] [int] NOT NULL,
 CONSTRAINT [PK_tbl_ServerUser_Log] PRIMARY KEY CLUSTERED 
(
	[serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_StaffAccount]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_StaffAccount](
	[Serial] [int] IDENTITY(2000000000,1) NOT NULL,
	[ID_hmac] [binary](32) NOT NULL,
	[ID_enc] [varbinary](128) NOT NULL,
	[PW_hash] [varchar](255) NOT NULL,
	[Grade] [tinyint] NOT NULL,
	[Depart] [varchar](32) NOT NULL,
	[RealName] [varchar](12) NOT NULL,
	[LastConnIP] [char](16) NOT NULL,
	[CreateDT] [datetime] NOT NULL,
	[LastLoginDT] [datetime] NOT NULL,
	[LastLogoffDT] [datetime] NOT NULL,
	[TotalLogMin] [int] NOT NULL,
	[SubGrade] [tinyint] NOT NULL,
	[ExpireDT] [datetime] NOT NULL,
	[ComClass] [varchar](12) NOT NULL,
	[BirthDay] [char](10) NOT NULL,
 CONSTRAINT [PK_tbl_StaffAccount] PRIMARY KEY CLUSTERED (
	[Serial] ASC
),
 CONSTRAINT [IX_tbl_StaffAccount] UNIQUE NONCLUSTERED (
	[ID_hmac] ASC
)
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_StaffAccount_BKP]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_StaffAccount_BKP](
	[Serial] [int] IDENTITY(2000000000,1) NOT NULL,
	[ID_hmac] [binary](32) NOT NULL,
	[ID_enc] [varbinary](128) NOT NULL,
	[PW_hash] [varchar](255) NOT NULL,
	[Grade] [tinyint] NOT NULL,
	[Depart] [varchar](32) NOT NULL,
	[RealName] [varchar](12) NOT NULL,
	[LastConnIP] [char](16) NOT NULL,
	[CreateDT] [datetime] NOT NULL,
	[LastLoginDT] [datetime] NOT NULL,
	[LastLogoffDT] [datetime] NOT NULL,
	[TotalLogMin] [int] NOT NULL,
	[SubGrade] [tinyint] NOT NULL,
	[ExpireDT] [datetime] NOT NULL,
	[ComClass] [varchar](12) NOT NULL,
	[BirthDay] [char](10) NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_UserAccount]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_UserAccount](
	[serial] [int] IDENTITY(1,1) NOT NULL,
	[id_hmac] [binary](32) NOT NULL,
	[id_enc] [varbinary](128) NOT NULL,
	[createtime] [datetime] NOT NULL,
	[createip] [char](16) NOT NULL,
	[lastlogintime] [datetime] NOT NULL,
	[lastlogofftime] [datetime] NOT NULL,
	[totallogmin] [int] NOT NULL,
	[lastconnectip] [char](16) NOT NULL,
	[pushclosetime] [datetime] NOT NULL,
	[pusherip] [char](16) NOT NULL,
	[JoinCode] [int] NOT NULL,
	[LoginFailureCnt] [tinyint] NOT NULL,
	[fire_on] [smalldatetime] NOT NULL,
	[fire_warning] [smalldatetime] NOT NULL,
	[uilock] [tinyint] NOT NULL,
	[uilock_pw] [varchar](255) NOT NULL,
	[uilock_failcnt] [tinyint] NOT NULL,
	[uilock_update] [smalldatetime] NOT NULL,
	[uilock_hintindex] [tinyint] NOT NULL,
	[uilock_hintanswer] [varchar](255) NOT NULL,
	[uilock_find_pass_failcnt] [tinyint] NOT NULL,
 CONSTRAINT [PK_tbl_rfuser] PRIMARY KEY CLUSTERED (
	[id_hmac] ASC
),
 CONSTRAINT [IX_tbl_rfuser] UNIQUE NONCLUSTERED (
	[serial] ASC
)
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_UserBan]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_UserBan](
	[nAccountSerial] [int] NOT NULL,
	[dtStartDate] [datetime] NOT NULL,
	[nPeriod] [int] NOT NULL,
	[nKind] [tinyint] NOT NULL,
	[szReason] [varchar](32) NULL,
	[GMReason] [varchar](32) NOT NULL,
	[GMWriter] [varchar](32) NOT NULL,
	[ReasonType] [tinyint] NOT NULL,
 CONSTRAINT [PK_tbl_UserBan] PRIMARY KEY CLUSTERED 
(
	[nAccountSerial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 90, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_UserBan_Log]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_UserBan_Log](
	[nSerial] [int] IDENTITY(1,1) NOT NULL,
	[nAccountSerial] [int] NOT NULL,
	[dtStartdate] [datetime] NOT NULL,
	[nPeriod] [int] NOT NULL,
	[nKind] [tinyint] NOT NULL,
	[szReason] [varchar](32) NULL,
	[GMReason] [varchar](32) NOT NULL,
	[GMWriter] [varchar](32) NOT NULL,
 CONSTRAINT [PK_tbl_UserBan_Log] PRIMARY KEY CLUSTERED 
(
	[nSerial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_UserCount_Log]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_UserCount_Log](
	[serial] [int] IDENTITY(1,1) NOT NULL,
	[dtDate] [datetime] NOT NULL,
	[nAverageUser] [int] NOT NULL,
	[nMaxUser] [int] NOT NULL,
	[nMaxUserTestServer] [int] NOT NULL,
 CONSTRAINT [PK_tbl_UserCount_Log] PRIMARY KEY CLUSTERED 
(
	[serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_usercurrentstate]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_usercurrentstate](
	[serial] [int] NOT NULL,
	[state] [tinyint] NOT NULL,
 CONSTRAINT [PK_tbl_usercurrentstate] PRIMARY KEY CLUSTERED 
(
	[serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_UserPush_Log]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_UserPush_Log](
	[nAccountSerial] [int] NOT NULL,
	[dtDate] [datetime] NOT NULL,
	[PushIP] [varchar](16) NOT NULL,
	[CloseIP] [varchar](16) NOT NULL,
 CONSTRAINT [PK_tbl_UserPush_Log] PRIMARY KEY CLUSTERED 
(
	[nAccountSerial] ASC,
	[dtDate] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_WorldHistory_Log]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_WorldHistory_Log](
	[serverName] [char](12) NOT NULL,
	[race] [char](12) NOT NULL,
	[avatorNum] [int] NOT NULL,
	[dalant] [bigint] NOT NULL,
	[gold] [bigint] NOT NULL,
	[lv] [int] NOT NULL,
	[logDate] [datetime] NOT NULL,
	[idx] [int] IDENTITY(1,1) NOT NULL,
 CONSTRAINT [PK_tbl_WorldHistory_Log] PRIMARY KEY CLUSTERED 
(
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_WorldServer_List]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_WorldServer_List](
	[Serial] [int] NOT NULL,
	[Name] [varchar](32) NOT NULL,
	[State] [int] NOT NULL,
	[LastUpdate] [datetime] NOT NULL,
	[msrepl_tran_version] [uniqueidentifier] NOT NULL,
 CONSTRAINT [PK_tbl_WorldServer_List] PRIMARY KEY CLUSTERED 
(
	[Serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbNoPach]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbNoPach](
	[serial] [int] IDENTITY(1,1) NOT NULL,
	[id] [binary](16) NOT NULL,
	[createtime] [datetime] NOT NULL,
	[createip] [char](16) NOT NULL,
	[lastlogintime] [datetime] NOT NULL,
	[lastlogofftime] [datetime] NOT NULL,
	[totallogmin] [int] NOT NULL,
	[lastconnectip] [char](16) NOT NULL,
	[pushclosetime] [datetime] NOT NULL,
	[pusherip] [char](16) NOT NULL,
	[JoinCode] [int] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Index [IX_tbl_rfaccount]    Script Date: 12/01/2026 16:35:27 ******/
CREATE NONCLUSTERED INDEX [IX_tbl_rfaccount] ON [dbo].[tbl_rfaccount]
(
	[password_hash] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
ALTER TABLE [dbo].[tbl_block_ip] ADD  CONSTRAINT [DF_tbl_permit_ip_addr3]  DEFAULT ((0)) FOR [addr3]
GO
ALTER TABLE [dbo].[tbl_block_ip] ADD  CONSTRAINT [DF_tbl_permit_ip_addr4]  DEFAULT ((255)) FOR [addr4]
GO
ALTER TABLE [dbo].[tbl_block_ip] ADD  CONSTRAINT [DF_tbl_block_ip_addr2range]  DEFAULT ((255)) FOR [addr2range]
GO
ALTER TABLE [dbo].[tbl_chargeresultstr] ADD  CONSTRAINT [DF_tbl_chargeresultstr_strError]  DEFAULT ('') FOR [strError]
GO
ALTER TABLE [dbo].[tbl_CristalBattle_Log] ADD  CONSTRAINT [DF_tbl_CristalBattle_Log_GetTime]  DEFAULT (getdate()) FOR [GetTime]
GO
ALTER TABLE [dbo].[tbl_CristalBattle_Log] ADD  CONSTRAINT [DF_tbl_CristalBattle_Log_ElapseHour]  DEFAULT ((0)) FOR [ElapseHour]
GO
ALTER TABLE [dbo].[tbl_CristalBattle_Log] ADD  CONSTRAINT [DF_tbl_CristalBattle_Log_ElapseMin]  DEFAULT ((0)) FOR [ElapseMin]
GO
ALTER TABLE [dbo].[tbl_CristalBattle_Log] ADD  CONSTRAINT [DF_tbl_CristalBattle_Log_ElapseSec]  DEFAULT ((0)) FOR [ElapseSec]
GO
ALTER TABLE [dbo].[tbl_CristalBattle_Log] ADD  CONSTRAINT [DF_tbl_cristalbattle_log_KeeperHitter]  DEFAULT ('*') FOR [KeeperHitter]
GO
ALTER TABLE [dbo].[tbl_CristalBattle_Log] ADD  CONSTRAINT [DF_tbl_cristalbattle_log_ByAnimus]  DEFAULT ((255)) FOR [ByAnimus]
GO
ALTER TABLE [dbo].[tbl_CristalBattle_Log] ADD  CONSTRAINT [DF_tbl_cristalbattle_log_DestroyRace]  DEFAULT ((255)) FOR [DestroyRace]
GO
ALTER TABLE [dbo].[tbl_GMActLog] ADD  CONSTRAINT [DF_tbl_GMActLog_PID]  DEFAULT ((0)) FOR [PID]
GO
ALTER TABLE [dbo].[tbl_GMActLog] ADD  CONSTRAINT [DF_tbl_GMActLog_ActDate]  DEFAULT (getdate()) FOR [ActDate]
GO
ALTER TABLE [dbo].[tbl_GMjobLog] ADD  CONSTRAINT [DF_tbl_GMjobLog_nKind]  DEFAULT ((0)) FOR [nKind]
GO
ALTER TABLE [dbo].[tbl_GMjobLog] ADD  CONSTRAINT [DF_tbl_GMjobLog_dtDate]  DEFAULT (getdate()) FOR [dtDate]
GO
ALTER TABLE [dbo].[tbl_HolyQuest_Log] ADD  CONSTRAINT [DF_tbl_HolyQuest_Log_LogDate]  DEFAULT (getdate()) FOR [LogDate]
GO
ALTER TABLE [dbo].[tbl_HostAccount] ADD  CONSTRAINT [DF_tbl_HostAccount_Password]  DEFAULT ('*') FOR [Password]
GO
ALTER TABLE [dbo].[tbl_HostAccount] ADD  CONSTRAINT [DF_tbl_HostAccount_Active]  DEFAULT ((0)) FOR [Grade]
GO
ALTER TABLE [dbo].[tbl_HostAccount] ADD  CONSTRAINT [DF_tbl_HostAccount_UserNum]  DEFAULT ((0)) FOR [Grade1]
GO
ALTER TABLE [dbo].[tbl_ItemBatch] ADD  CONSTRAINT [DF_tbl_ItemBatch_DCK]  DEFAULT (0) FOR [DCK]
GO
ALTER TABLE [dbo].[tbl_ItemBatch] ADD  CONSTRAINT [DF_tbl_ItemBatch_RID]  DEFAULT (11) FOR [RID]
GO
ALTER TABLE [dbo].[tbl_ItemBatch] ADD  CONSTRAINT [DF_tbl_ItemBatch_D]  DEFAULT (1) FOR [D]
GO
ALTER TABLE [dbo].[tbl_ItemBatch] ADD  CONSTRAINT [DF_tbl_ItemBatch_U]  DEFAULT (0x0fffffff) FOR [U]
GO
ALTER TABLE [dbo].[tbl_ItemBatch] ADD  CONSTRAINT [DF_tbl_ItemBatch_addda]  DEFAULT (getdate()) FOR [adddate]
GO
ALTER TABLE [dbo].[tbl_LUAccount] ADD  CONSTRAINT [DF_tbl_LUAccount]  DEFAULT (0) FOR [BCodeTU]
GO
ALTER TABLE [dbo].[tbl_LUAccount] ADD  DEFAULT ((0)) FOR [accounttype]
GO
ALTER TABLE [dbo].[tbl_protocolaccesslog] ADD  CONSTRAINT [DF_tbl_protocolaccesslog_ProtocolMsgID]  DEFAULT ((0)) FOR [ProtocolMsgID]
GO
ALTER TABLE [dbo].[tbl_protocolaccesslog] ADD  CONSTRAINT [DF_tbl_protocolaccesslog_ProtocolID]  DEFAULT ((0)) FOR [ProtocolID]
GO
ALTER TABLE [dbo].[tbl_protocolaccesslog] ADD  CONSTRAINT [DF_tbl_protocolaccesslog_GMSerial]  DEFAULT ((0)) FOR [GMSerial]
GO
ALTER TABLE [dbo].[tbl_protocolaccesslog] ADD  CONSTRAINT [DF_tbl_protocolaccesslog_AccessDate]  DEFAULT (getdate()) FOR [AccessDate]
GO
ALTER TABLE [dbo].[tbl_protocolauthinfo] ADD  CONSTRAINT [DF_tbl_protocolauthinfo_dck]  DEFAULT ((0)) FOR [dck]
GO
ALTER TABLE [dbo].[tbl_protocolauthinfo] ADD  CONSTRAINT [DF_tbl_protocolauthinfo_P_MsgID]  DEFAULT ((0)) FOR [P_MsgID]
GO
ALTER TABLE [dbo].[tbl_protocolauthinfo] ADD  CONSTRAINT [DF_tbl_protocolauthinfo_P_ID]  DEFAULT ((0)) FOR [P_ID]
GO
ALTER TABLE [dbo].[tbl_protocolauthinfo] ADD  CONSTRAINT [DF_tbl_protocolauthinfo_P_AuthType]  DEFAULT ((0)) FOR [P_AuthType]
GO
ALTER TABLE [dbo].[tbl_protocolauthinfo] ADD  CONSTRAINT [DF_tbl_protocolauthinfo_GMID]  DEFAULT ('*') FOR [GMID]
GO
ALTER TABLE [dbo].[tbl_protocolauthinfo] ADD  CONSTRAINT [DF_tbl_protocolauthinfo_GMSerial]  DEFAULT ((0)) FOR [GMSerial]
GO
ALTER TABLE [dbo].[tbl_protocolinfo] ADD  CONSTRAINT [DF_tbl_protocolinfo_dck]  DEFAULT ((0)) FOR [dck]
GO
ALTER TABLE [dbo].[tbl_protocolinfo] ADD  CONSTRAINT [DF_tbl_protocolinfo_ProtocolMsgID]  DEFAULT ((0)) FOR [ProtocolMsgID]
GO
ALTER TABLE [dbo].[tbl_protocolinfo] ADD  CONSTRAINT [DF_tbl_protocolinfo_ProtocolID]  DEFAULT ((0)) FOR [ProtocolID]
GO
ALTER TABLE [dbo].[tbl_protocolinfo] ADD  CONSTRAINT [DF_tbl_protocolinfo_ProtocolName]  DEFAULT ('*') FOR [ProtocolName]
GO
ALTER TABLE [dbo].[tbl_protocolinfo] ADD  CONSTRAINT [DF_tbl_protocolinfo_ProtocolExplain]  DEFAULT ('*') FOR [ProtocolExplain]
GO
ALTER TABLE [dbo].[tbl_protocolinfo] ADD  CONSTRAINT [DF_tbl_protocolinfo_ProtocolFldCnt]  DEFAULT ((0)) FOR [ProtocolFldCnt]
GO
ALTER TABLE [dbo].[tbl_protocolinfo] ADD  CONSTRAINT [DF_tbl_protocolinfo_UpdateDate]  DEFAULT (getdate()) FOR [UpdateDate]
GO
GO
ALTER TABLE [dbo].[tbl_ServerUser_Log] ADD  CONSTRAINT [DF_tbl_ServerUser_Log_nAverageUser]  DEFAULT ((0)) FOR [nAverageUser]
GO
ALTER TABLE [dbo].[tbl_ServerUser_Log] ADD  CONSTRAINT [DF_tbl_ServerUser_Log_nMaxUser]  DEFAULT ((0)) FOR [nMaxUser]
GO
ALTER TABLE [dbo].[tbl_ServerUser_Log] ADD  CONSTRAINT [DF_tbl_ServerUser_Log_nBellaUser]  DEFAULT ((0)) FOR [nBellaUser]
GO
ALTER TABLE [dbo].[tbl_ServerUser_Log] ADD  CONSTRAINT [DF_tbl_ServerUser_Log_nCoraUser]  DEFAULT ((0)) FOR [nCoraUser]
GO
ALTER TABLE [dbo].[tbl_ServerUser_Log] ADD  CONSTRAINT [DF_tbl_ServerUser_Log_nAccUser]  DEFAULT ((0)) FOR [nAccUser]
GO
ALTER TABLE [dbo].[tbl_StaffAccount] ADD  CONSTRAINT [DF_tbl_StaffAccount_Grade]  DEFAULT ((1)) FOR [Grade]
GO
ALTER TABLE [dbo].[tbl_StaffAccount] ADD  CONSTRAINT [DF_tbl_StaffAccount_LastConnIP]  DEFAULT ('0') FOR [LastConnIP]
GO
ALTER TABLE [dbo].[tbl_StaffAccount] ADD  CONSTRAINT [DF_tbl_StaffAccount_CreateDT]  DEFAULT (getdate()) FOR [CreateDT]
GO
ALTER TABLE [dbo].[tbl_StaffAccount] ADD  CONSTRAINT [DF_tbl_StaffAccount_LastLoginDT]  DEFAULT ((0)) FOR [LastLoginDT]
GO
ALTER TABLE [dbo].[tbl_StaffAccount] ADD  CONSTRAINT [DF_tbl_StaffAccount_LastLogoffDT]  DEFAULT ((0)) FOR [LastLogoffDT]
GO
ALTER TABLE [dbo].[tbl_StaffAccount] ADD  CONSTRAINT [DF_tbl_StaffAccount_TotalLogMin]  DEFAULT ((0)) FOR [TotalLogMin]
GO
ALTER TABLE [dbo].[tbl_StaffAccount] ADD  CONSTRAINT [DF_tbl_StaffAccount_SubGrade]  DEFAULT ((0)) FOR [SubGrade]
GO
ALTER TABLE [dbo].[tbl_StaffAccount] ADD  CONSTRAINT [DF_tbl_StaffAccount_ExpireDT]  DEFAULT (dateadd(month,(1),getdate())) FOR [ExpireDT]
GO
ALTER TABLE [dbo].[tbl_StaffAccount] ADD  CONSTRAINT [DF_tbl_StaffAccount_ComClass]  DEFAULT ('??') FOR [ComClass]
GO
ALTER TABLE [dbo].[tbl_StaffAccount] ADD  CONSTRAINT [DF_tbl_StaffAccount_BirthDay]  DEFAULT (((0)-(0))-(0)) FOR [BirthDay]
GO
ALTER TABLE [dbo].[tbl_UserAccount] ADD  CONSTRAINT [DF_tbl_rfuser_CreateTime]  DEFAULT (getdate()) FOR [createtime]
GO
ALTER TABLE [dbo].[tbl_UserAccount] ADD  CONSTRAINT [DF_tbl_rfuser_createip]  DEFAULT ((0)) FOR [createip]
GO
ALTER TABLE [dbo].[tbl_UserAccount] ADD  CONSTRAINT [DF_tbl_rfuser_LastLogTime]  DEFAULT (getdate()) FOR [lastlogintime]
GO
ALTER TABLE [dbo].[tbl_UserAccount] ADD  CONSTRAINT [DF_tbl_rfuser_lastlogintime1]  DEFAULT (getdate()) FOR [lastlogofftime]
GO
ALTER TABLE [dbo].[tbl_UserAccount] ADD  CONSTRAINT [DF_tbl_rfuser_TotalLogMin]  DEFAULT ((0)) FOR [totallogmin]
GO
ALTER TABLE [dbo].[tbl_UserAccount] ADD  CONSTRAINT [DF_tbl_rfuser_lastconnectip]  DEFAULT ((0)) FOR [lastconnectip]
GO
ALTER TABLE [dbo].[tbl_UserAccount] ADD  CONSTRAINT [DF_tbl_rfuser_pushclosetime]  DEFAULT (getdate()) FOR [pushclosetime]
GO
ALTER TABLE [dbo].[tbl_UserAccount] ADD  CONSTRAINT [DF_tbl_rfuser_pusherip]  DEFAULT ((0)) FOR [pusherip]
GO
ALTER TABLE [dbo].[tbl_UserAccount] ADD  CONSTRAINT [DF_tbl_rfuser_JoinCode]  DEFAULT ((0)) FOR [JoinCode]
GO
ALTER TABLE [dbo].[tbl_UserAccount] ADD  CONSTRAINT [DF_tbl_useraccount_loginfailurecnt]  DEFAULT ((0)) FOR [LoginFailureCnt]
GO
ALTER TABLE [dbo].[tbl_UserAccount] ADD  CONSTRAINT [DF_tbl_useraccount_fire_on]  DEFAULT (getdate()) FOR [fire_on]
GO
ALTER TABLE [dbo].[tbl_UserAccount] ADD  CONSTRAINT [DF_tbl_useraccount_fire_warning]  DEFAULT (getdate()) FOR [fire_warning]
GO
ALTER TABLE [dbo].[tbl_UserAccount] ADD  CONSTRAINT [DF_tbl_useraccount_uilock]  DEFAULT ((0)) FOR [uilock]
GO
ALTER TABLE [dbo].[tbl_UserAccount] ADD  CONSTRAINT [DF_tbl_useraccount_uilock_pw]  DEFAULT ((0)) FOR [uilock_pw]
GO
ALTER TABLE [dbo].[tbl_UserAccount] ADD  CONSTRAINT [DF_tbl_useraccount_uilock_failcnt]  DEFAULT ((0)) FOR [uilock_failcnt]
GO
ALTER TABLE [dbo].[tbl_UserAccount] ADD  CONSTRAINT [DF_tbl_useraccount_uilock_update]  DEFAULT ((0)) FOR [uilock_update]
GO
ALTER TABLE [dbo].[tbl_UserAccount] ADD  CONSTRAINT [DF_tbl_useraccount_uilock_hintindex]  DEFAULT ((0)) FOR [uilock_hintindex]
GO
ALTER TABLE [dbo].[tbl_UserAccount] ADD  CONSTRAINT [DF_tbl_useraccount_uilock_hintanswer]  DEFAULT ((0)) FOR [uilock_hintanswer]
GO
ALTER TABLE [dbo].[tbl_UserAccount] ADD  CONSTRAINT [DF_tbl_useraccount_uilock_find_pass_failcnt]  DEFAULT ((0)) FOR [uilock_find_pass_failcnt]
GO
ALTER TABLE [dbo].[tbl_UserBan] ADD  CONSTRAINT [DF_tbl_UserBan_dtStartDate]  DEFAULT (getdate()) FOR [dtStartDate]
GO
ALTER TABLE [dbo].[tbl_UserBan] ADD  CONSTRAINT [DF_tbl_UserBan_nKind]  DEFAULT (0) FOR [nKind]
GO
ALTER TABLE [dbo].[tbl_UserBan] ADD  CONSTRAINT [DF_tbl_userban_GMReason]  DEFAULT ('*') FOR [GMReason]
GO
ALTER TABLE [dbo].[tbl_UserBan] ADD  CONSTRAINT [DF_tbl_userban_GMWriter]  DEFAULT ('*') FOR [GMWriter]
GO
ALTER TABLE [dbo].[tbl_UserBan] ADD  CONSTRAINT [DF_tbl_UserBan_ReasonType]  DEFAULT (0) FOR [ReasonType]
GO
ALTER TABLE [dbo].[tbl_UserBan_Log] ADD  CONSTRAINT [DF_tbl_UserBan_Log_dtStartdate]  DEFAULT (getdate()) FOR [dtStartdate]
GO
ALTER TABLE [dbo].[tbl_UserBan_Log] ADD  CONSTRAINT [DF_tbl_UserBan_Log_nKind]  DEFAULT ((0)) FOR [nKind]
GO
ALTER TABLE [dbo].[tbl_UserBan_Log] ADD  CONSTRAINT [DF_tbl_userban_log_GMReason]  DEFAULT ('*') FOR [GMReason]
GO
ALTER TABLE [dbo].[tbl_UserBan_Log] ADD  CONSTRAINT [DF_tbl_userban_log_GMWriter]  DEFAULT ('*') FOR [GMWriter]
GO
ALTER TABLE [dbo].[tbl_UserCount_Log] ADD  CONSTRAINT [DF_tbl_Usercount_Log_nAverageUser]  DEFAULT ((0)) FOR [nAverageUser]
GO
ALTER TABLE [dbo].[tbl_UserCount_Log] ADD  CONSTRAINT [DF_tbl_Usercount_Log_nMaxUser]  DEFAULT ((0)) FOR [nMaxUser]
GO
ALTER TABLE [dbo].[tbl_UserCount_Log] ADD  CONSTRAINT [DF_tbl_Usercount_Log_nMaxUserTestServer]  DEFAULT ((0)) FOR [nMaxUserTestServer]
GO
ALTER TABLE [dbo].[tbl_usercurrentstate] ADD  CONSTRAINT [DF_tbl_usercurrentstate_state]  DEFAULT ((0)) FOR [state]
GO
ALTER TABLE [dbo].[tbl_UserPush_Log] ADD  CONSTRAINT [DF_tbl_UserPush_Log_AccountSerial]  DEFAULT ((0)) FOR [nAccountSerial]
GO
ALTER TABLE [dbo].[tbl_UserPush_Log] ADD  CONSTRAINT [DF_tbl_UserPush_Log_Date]  DEFAULT (getdate()) FOR [dtDate]
GO
ALTER TABLE [dbo].[tbl_UserPush_Log] ADD  CONSTRAINT [DF_tbl_UserPush_Log_PushIP1]  DEFAULT ((0)) FOR [PushIP]
GO
ALTER TABLE [dbo].[tbl_UserPush_Log] ADD  CONSTRAINT [DF_tbl_UserPush_Log_CloseIP1]  DEFAULT ((0)) FOR [CloseIP]
GO
ALTER TABLE [dbo].[tbl_WorldHistory_Log] ADD  CONSTRAINT [DF_tbl_WorldHistory_Log_avatorNum]  DEFAULT ((0)) FOR [avatorNum]
GO
ALTER TABLE [dbo].[tbl_WorldHistory_Log] ADD  CONSTRAINT [DF_tbl_WorldHistory_Log_dalant]  DEFAULT ((0)) FOR [dalant]
GO
ALTER TABLE [dbo].[tbl_WorldHistory_Log] ADD  CONSTRAINT [DF_tbl_WorldHistory_Log_gold]  DEFAULT ((0)) FOR [gold]
GO
ALTER TABLE [dbo].[tbl_WorldHistory_Log] ADD  CONSTRAINT [DF_tbl_WorldHistory_Log_logDate]  DEFAULT (getdate()) FOR [logDate]
GO
ALTER TABLE [dbo].[tbl_WorldServer_List] ADD  CONSTRAINT [DF_tbl_WorldServer_List_LastUpdate]  DEFAULT (getdate()) FOR [LastUpdate]
GO
ALTER TABLE [dbo].[tbl_WorldServer_List] ADD  DEFAULT (newid()) FOR [msrepl_tran_version]
GO
ALTER TABLE [dbo].[tbl_GMActLog]  WITH NOCHECK ADD  CONSTRAINT [FK_CID] FOREIGN KEY([PID])
REFERENCES [dbo].[tbl_GMActParam] ([ID])
GO
ALTER TABLE [dbo].[tbl_GMActLog] CHECK CONSTRAINT [FK_CID]
GO
ALTER TABLE [dbo].[tbl_GMActLog]  WITH NOCHECK ADD  CONSTRAINT [FK_TID] FOREIGN KEY([TID])
REFERENCES [dbo].[tbl_GMActType] ([ID])
GO
ALTER TABLE [dbo].[tbl_GMActLog] CHECK CONSTRAINT [FK_TID]
GO
/****** Object:  StoredProcedure [dbo].[LUG_InsertAccount]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[LUG_UpdatePassword]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pCheck_Today_LogTable]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO
/****** Object:  StoredProcedure [dbo].[pDelete_UserBan]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
/****** Object:  StoredProcedure [dbo].[pInsert_BlockIP]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pInsert_Dev]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
/****** Object:  StoredProcedure [dbo].[pInsert_GMjob]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pInsert_HolyQuestLog]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO
/****** Object:  StoredProcedure [dbo].[pInsert_ServerUserLog]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
/****** Object:  StoredProcedure [dbo].[pInsert_ServerUserLog2]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pInsert_Staff]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO
/****** Object:  StoredProcedure [dbo].[pInsert_User]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
/****** Object:  StoredProcedure [dbo].[pInsert_UserBan]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pInsert_UserBan_20070302]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pInsert_UserBan_20071016]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pInsert_UserBanLog]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pInsert_UserBanLog_20070302]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pInsert_UserConnLog]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
/****** Object:  StoredProcedure [dbo].[pInsert_UserCountLog]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pInsert_UserCountLog20080218]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pInsert_UserCurrentState]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pInsert_UserLogoutLog]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
/****** Object:  StoredProcedure [dbo].[pInsert_UserLogoutLog_Daily]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO
/****** Object:  StoredProcedure [dbo].[pInsert_UserLogoutLog_Daily20070115]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pInsert_UserPushLog]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pProcess_UserBan]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[prc_process_fg_detection]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pSelect_AccountPass]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO
/****** Object:  StoredProcedure [dbo].[pSelect_AccountPass_061122]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pSelect_AccountPass_China]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO
/****** Object:  StoredProcedure [dbo].[pSelect_BlockIP]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pSelect_BlockIP_20070122]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pSelect_BraAccountInfo]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pSelect_HolyQuestHistory]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO
/****** Object:  StoredProcedure [dbo].[pSelect_HostAccountInfo]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pSelect_HostAccountInfo20061113]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pselect_pcbangip]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pSelect_StaffExpire]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO
/****** Object:  StoredProcedure [dbo].[pSelect_StaffInfo]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO
/****** Object:  StoredProcedure [dbo].[pSelect_StaffInfoEx]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pSelect_UserBan]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
/****** Object:  StoredProcedure [dbo].[pSelect_UserBanHistory]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pSelect_UserBanHistory_20070302]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pSelect_UserCountInfo]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
/****** Object:  StoredProcedure [dbo].[pSelect_UserInfo]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
/****** Object:  StoredProcedure [dbo].[pSelect_UserInfo_20070612]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pSelect_UserInfoEx]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
/****** Object:  StoredProcedure [dbo].[pSelect_UserInfoEx_20070221]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pSelect_UserInfoEx_20070718]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pSelect_UserSerial]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
/****** Object:  StoredProcedure [dbo].[pUpdate_FireguardBlock]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pUpdate_FireguardBlock20071016]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pUpdate_StaffLoginDate]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pUpdate_StaffLogoffDate]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
/****** Object:  StoredProcedure [dbo].[pUpdate_UILock_Init]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pUpdate_UILock_Refresh]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pUpdate_UILock_Update]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pUpdate_UserBan]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pUpdate_UserBan_20070302]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pUpdate_UserBan_20071016]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pUpdate_UserCurrentState]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pUpdate_UserLoginDate]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
/****** Object:  StoredProcedure [dbo].[pUpdate_UserLogoffDate]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
/****** Object:  StoredProcedure [dbo].[pUpdate_UserPushDate]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
/****** Object:  StoredProcedure [dbo].[pUpdate_WorldServer]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Object:  StoredProcedure [dbo].[pWork_SelLoginCharacter]    Script Date: 12/01/2026 16:35:27 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
USE [master]
GO
ALTER DATABASE [RF_User] SET  READ_WRITE 
GO
